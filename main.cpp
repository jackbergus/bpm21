/*
 * main.cpp
 * This file is part of bpm21
 *
 * Copyright (C) 2021 - Giacomo Bergami
 *
 * bpm21 is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * bpm21 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with bpm21. If not, see <http://www.gnu.org/licenses/>.
 */
#include <iostream>
#include <ltlf/DataPredicate.h>
#include <pipeline/input_pipeline.h>
#include "declare/DataTraceParse.h"
#include "pipeline/foreign_data_loads.h"
#include "for_semantisch_inconsistency.h"

#define     LIMIT_SIMPLIFIC_THR  (5)

struct elements {
    std::pair<std::string, std::string> files;

    ltlf                            pos_model,          neg_model;
    FlexibleFA<std::string, size_t> pos_global_graph,   neg_global_graph;
    double                          pos_graph_size,     neg_graph_size;
    for_semantisch_inconsistency    pos_trace_unfolder, neg_trace_unfolder;

    elements(const std::string& epsilon) : pos_trace_unfolder{epsilon}, neg_trace_unfolder{epsilon} {}
};

void print_syntax_metrics(std::ostream& out, double d1, double d2, double d3, const std::string& strategy) {
    out << " Syntactic distances, with strategy " << strategy << std::endl;
    out << " a) Straightforward normalization " << std::endl;
    {
        double d1L = (d3 <= std::numeric_limits<double>::min()) ? std::numeric_limits<double>::infinity() : 1.0-(std::min(d1,d3))/std::max(d3,d1);
        double d2L = (d3 <= std::numeric_limits<double>::min()) ? std::numeric_limits<double>::infinity() :1.0-(std::min(d2,d3))/std::max(d3,d2);
        out << "    * Left distance from intersection: " << d1L << std::endl;
        out << "    * Right distance from intersection: " << d2L << std::endl;
        out << "    * Left distance from Right (Avg): " << ((d3 <= std::numeric_limits<double>::min()) ? std::numeric_limits<double>::infinity() :((d1L+d2L)/2.0)) << std::endl;
    }
    out << " b) Kernel-based normalization " << std::endl;
    {
        double d1L = /*((d1*d3) <= std::numeric_limits<double>::min()) ? std::numeric_limits<double>::infinity() :*/ std::sqrt(1.0 - 1.0*(std::min(d1,d3)/std::sqrt(d1*d3)));
        double d2L = /*((d2*d3) <= std::numeric_limits<double>::min()) ? std::numeric_limits<double>::infinity() :*/ std::sqrt(1.0 - 1.0*(std::min(d2,d3)/std::sqrt(d2*d3)));
        out << "    * Left distance from intersection: " << d1L << std::endl;
        out << "    * Right distance from intersection: " << d2L << std::endl;
        out << "    * Left distance from Right (Avg): " << ((d1L+d2L)/2.0) << std::endl;
    }
    out  << std::endl << std::endl;
}

#include <Eigen/Sparse>
#include <Eigen/Dense>
#include <Eigen/LU>
#include <formula/aalta_formula.h>
#include<Eigen/SparseCholesky>
#include <wrappers/aaltaf_wrapper.h>

double between_graph_classification_simple_kernel(FlexibleFA<std::string, size_t>& g, bool transpose = false) {
    /*auto U = left.getNodeIds();
    size_t N = U.size();
    auto V = right.getNodeIds();
    size_t M = V.size();
    std::unordered_map<size_t, size_t> mapL, mapR;
    for (size_t i = 0; i<std::min(N,M); i++) {
        mapL[U.at(i)] = i;
        mapR[V.at(i)] = i;
    }
    if (N<M) {
        for (size_t i = N; i<M; i++) {
            mapR[V.at(i)] = i;
        }
    } else if (N>M) {
        for (size_t i = M; i<N; i++) {
            mapR[U.at(i)] = i;
        }
    }*/
    typedef Eigen::SparseMatrix<double, Eigen::RowMajor> SpMat;
    typedef Eigen::Triplet<double> T;
    std::vector<T> tripletList;

    size_t N = g.maximumNodeId();
    size_t M = g.maximumEdgeId();
    tripletList.reserve(M);
    SpMat A(N, N);

    for (size_t id : g.getNodeIds()) {
        tripletList.emplace_back(id, id, 1.0);
        for (const auto& edge : g.outgoingEdges(id)) {
            tripletList.emplace_back(id, edge.second, 1.0);
        }
    }

    A.setFromTriplets(tripletList.begin(), tripletList.end());

    if (transpose) {
        A = A * A.transpose();
    }
    SpMat D(N, N);

    {
        Eigen::VectorXd rowSums(N); rowSums.setZero();
        for (int k = 0; k < A.outerSize(); ++k){
            for (SpMat::InnerIterator it(A, k); it; ++it){
                size_t i = it.row();
                //size_t j = it.col();
                double v = it.value();
                rowSums(i) += v;
                assert(v >= 0.0);
            }
        }

        tripletList.clear();
        for (size_t i = 0; i<N; i++) {
            tripletList.emplace_back(i, i, 1/std::sqrt(rowSums(i)));
        }
        D.setFromTriplets(tripletList.begin(), tripletList.end());
    }

    A = D * A * D;

    double maxCoeff;
    {
        Eigen::VectorXd rowSums(N); rowSums.setZero();
        for (int k = 0; k < A.outerSize(); ++k){
            for (SpMat::InnerIterator it(A, k); it; ++it){
                size_t i = it.row();
                //size_t j = it.col();
                double v = it.value();
                rowSums(i) += v;
                assert(v >= 0.0);
            }
        }

        maxCoeff = (1.0/rowSums.maxCoeff());
    }

    A = maxCoeff * A;
    SpMat I(N, N);
    I.setIdentity();
    SpMat tmp = I - A;

    Eigen::SimplicialLLT<SpMat> solver;
    Eigen::VectorXd b(N);

    solver.compute(tmp);
    if (solver.info() != Eigen::Success) {
        return 0.0;
    }
    Eigen::VectorXd x = solver.solve(b);
    return x.sum();
}

struct normalization_results {
    double similarity,normalized_similarity,distance,normalized_distance;

    normalization_results(double similarity, double normalizedSimilarity, double distance, double normalizedDistance)
            : similarity(similarity), normalized_similarity(normalizedSimilarity), distance(distance),
              normalized_distance(normalizedDistance) {}
    normalization_results() : normalization_results(0,0,0,0) {}
    normalization_results(const normalization_results&) = default;
    normalization_results(normalization_results&&) = default;
    normalization_results& operator=(const normalization_results&) = default;
    normalization_results& operator=(normalization_results&&) = default;
};

input_pipeline &
getSemanticsDecomposition(bool semantisch, input_pipeline &Pip, std::unordered_set<std::string> &SigmaAll2,
                          const std::unordered_map<std::string, std::string> &map,
                          const std::vector<std::string> &pos_atom_log,
                          for_semantisch_inconsistency &ref,
                          const std::string &Uppercase, const ltlf &test, const std::string &str);

void between_graph_classification_normalized_similarity(std::ostream& out, FlexibleFA<std::string, size_t>& left, FlexibleFA<std::string, size_t>& right, FlexibleFA<std::string, size_t>& g, const std::string& strategy) {
    double L = between_graph_classification_simple_kernel(left);
    double R = between_graph_classification_simple_kernel(right);
    double LR = between_graph_classification_simple_kernel(g);

    double distance = std::sqrt(L+R-2*LR);
    normalization_results res{LR, (LR/std::sqrt(L*R)), distance, distance/(distance+1.0)};
    std::cout << "Strategy: " << strategy << std::endl;
    std::cout << " - Random Walks Kernel: " << res.similarity << std::endl;
    std::cout << " - Normalized Random Walks Kernel: " << res.normalized_similarity << std::endl;
    std::cout << " - Random Walks Distance: " << res.distance << std::endl;
    std::cout << " - Normalized Random Walks Distance: " << res.normalized_distance << std::endl;
}

//#define NOSKIP


void trient(const std::vector<std::pair<std::string, std::string>>& models) {
    Eigen::initParallel();
    std::vector<struct elements> mappings;
    bool doNotVisitLoopsTwice = false;
    bool semantisch = true;

    // Loading the log
    std::string atoms{"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"};
    input_pipeline Pip{"p"};
    std::unordered_set<std::string> SigmaAll, SigmaAll2;
    std::unordered_map<std::string, std::string> map;

    // Negative log
    auto neg_log = Pip.print_atomized_traces("data/trient/logs/X-RayNegTest.xes", "data/trient/logs/neg", SigmaAll, true, true);

    // Positive log
    auto pos_log = Pip.print_atomized_traces("data/trient/logs/X-RayPosTest.xes", "data/trient/logs/pos", SigmaAll, true, true);

    std::cout << "n-events: " << SigmaAll.size() << std::endl;
    size_t max_trace_length = 0;
    for (const auto& trace : neg_log)
        max_trace_length = std::max(max_trace_length, trace.size());
    for (const auto& trace : pos_log)
        max_trace_length = std::max(max_trace_length, trace.size());
    std::cout << "max-trace-length: " << max_trace_length << std::endl;

    /*double k = LIMIT_SIMPLIFIC_THR;
    double min_cost = std::pow(APPROX_PATH_COST, max_trace_length-2)*std::pow(LIMIT_VISIT_COST, k);*/
    size_t atom_i = 0;

    // Mapping elements
    for (const std::string& act : SigmaAll) {
        std::string k{atoms.at(atom_i++)};
        map[act] = k;
        SigmaAll2.insert(k);
    }


    std::vector<std::string> neg_atom_log;
    neg_atom_log.reserve((neg_log.size()));
    for (const auto& vector : neg_log) {
        std::string trace = "";
        for (const std::string& event : vector)
            trace += map[event];
        neg_atom_log.emplace_back(trace);
    }
    {
        std::unordered_set<std::string> s;
        for (auto& i : neg_atom_log)
            s.insert(i);
        neg_atom_log.assign( s.begin(), s.end() );
    }
    std::cout << " Unique Negs: " << neg_atom_log.size() << std::endl;

    std::vector<std::string> pos_atom_log;
    pos_atom_log.reserve(pos_log.size());
    for (const auto& vector : pos_log) {
        std::string trace = "";
        for (const std::string& event : vector)
            trace += map[event];
        pos_atom_log.emplace_back(trace);
    }
    {
        std::unordered_set<std::string> s;
        for (auto& i : pos_atom_log)
            s.insert(i);
        pos_atom_log.assign( s.begin(), s.end() );
    }
    std::cout << " Unique Pos: "<< pos_atom_log.size() << std::endl;

    size_t N = 0;
    for (const std::pair<std::string, std::string>& pos_neg : models) {
        std::cout << "Dataset #" << N << std::endl;
        std::cout << " pos = " << pos_neg.first << std::endl;
        std::cout << " neg = " << pos_neg.second << std::endl << std::endl;
        auto& ref = mappings.emplace_back(".");

        ////// Positive Model
        std::cout << " ** Loading positive model: " << pos_neg.first << std::endl;
        Pip.run_pipeline( pos_neg.first, false);
        ref.pos_model = Pip.model;
        FlexibleFA<size_t, std::string> pos_graph = Pip.lydia_script.generate_graph(SigmaAll,Pip.model);
        ref.pos_global_graph = pos_graph.shiftLabelsToNodes();
        ref.pos_graph_size = /*ref.pos_global_graph*/pos_graph.size();
        auto final_pos = Pip.final_model;
        {
            std::stringstream ss;
            ss << ref.pos_model;
            std::cout << " - Positive model syntax inconsistency = " << check_formula(ss.str()) << std::endl;
        }

        ////// Negative Model
        std::cout << " ** Loading negative model: " << pos_neg.second << std::endl;
        Pip.run_pipeline( pos_neg.second, false);
        ref.neg_model = Pip.model;
        FlexibleFA<size_t, std::string> neg_graph = Pip.lydia_script.generate_graph(SigmaAll,Pip.model);
        ref.neg_global_graph = neg_graph.shiftLabelsToNodes();
        ref.neg_graph_size = /*ref.neg_global_graph*/neg_graph.size();
        auto final_neg = Pip.final_model;
        N++;
    }

    for (size_t iter = 0; iter < N; iter++) {
        for (size_t i = 0; i < iter; i++) {
            std::cout << "Dataset comparison : " << i << " vs. " << iter << std::endl;

            auto& leftI = mappings[i];
            auto& leftJ = mappings[iter];

#if 1
            {
                std::cout << " [Computing the intersection:] " << std::endl;

                {
                    ltlf conj_pos = ltlf::And(leftI.pos_model, leftJ.pos_model);
                    std::stringstream ss;
                    ss << conj_pos;
                    double inc = check_formula(ss.str());
                    std::cout << " - PositiveL&PositiveR syntax inconsistency = " << inc << std::endl;
                    if (inc == 0.0) {
                        std::cout << "° Positive datasets" << std::endl;
                        auto g1 = Pip.lydia_script.generate_graph(SigmaAll, conj_pos).shiftLabelsToNodes();
                        auto g2 = FlexibleFA<std::string, size_t>::crossProductWithNodeLabels(leftI.neg_global_graph, leftJ.neg_global_graph);
                        print_syntax_metrics(std::cout, leftI.pos_graph_size, leftJ.pos_graph_size, g1.size(), "AUTOMATON");
                        print_syntax_metrics(std::cout, leftI.pos_graph_size, leftJ.pos_graph_size, g2.size(), "PRODUCT");
                        between_graph_classification_normalized_similarity(std::cout,leftI.pos_global_graph, leftJ.pos_global_graph, g1, "AUTOMATON");
                        between_graph_classification_normalized_similarity(std::cout,leftI.pos_global_graph, leftJ.pos_global_graph, g2, "PRODUCT");
                    }
                }
                {
                    ltlf conj_neg = ltlf::And(leftI.neg_model, leftJ.neg_model);
                    std::stringstream ss;
                    ss << conj_neg;
                    double inc = check_formula(ss.str());
                    std::cout << " - NegativeL&NegativeR syntax inconsistency = " << inc << std::endl;
                    if (inc == 0.0) {
                        std::cout << "° Negative datasets" << std::endl;
                        auto g1 = Pip.lydia_script.generate_graph(SigmaAll,conj_neg).shiftLabelsToNodes();
                        auto g2 = FlexibleFA<std::string, size_t>::crossProductWithNodeLabels(leftI.neg_global_graph, leftJ.neg_global_graph);
                        print_syntax_metrics(std::cout, leftI.neg_graph_size, leftJ.neg_graph_size, g1.size(), "AUTOMATON");
                        print_syntax_metrics(std::cout, leftI.neg_graph_size, leftJ.neg_graph_size, g2.size(), "PRODUCT");
                        between_graph_classification_normalized_similarity(std::cout, leftI.neg_global_graph, leftJ.neg_global_graph, g1, "AUTOMATON");
                        between_graph_classification_normalized_similarity(std::cout, leftI.neg_global_graph, leftJ.neg_global_graph, g2, "PRODUCT");
                    }
                }


            }
#endif

            std::cout << std::endl << std::endl << std::endl;
        }
    }


}

input_pipeline &
getSemanticsDecomposition(bool semantisch, input_pipeline &Pip, std::unordered_set<std::string> &SigmaAll2,
                          const std::unordered_map<std::string, std::string> &map,
                          const std::vector<std::string> &pos_atom_log, for_semantisch_inconsistency &ref,
                          const std::string &Uppercase, const ltlf &test, const std::string &str) {
    Pip.final_model = test;
    Pip.decompose_genmodel_for_tiny_graphs(SigmaAll2, str+"out", false, true, map);
    //size_t X = Pip.GraphVector.size();

    std::cout << " ** Loading chunks " << Uppercase << " graph: " << str << std::endl;
    if (semantisch)  {
        for (const auto& g : Pip.GraphVector) {
            ref.collect_traces_from_model_clause_as_graph(g);
        }
        auto result = ref.compute_in_triplicate(pos_atom_log);
        std::cout << " - " << Uppercase << " model semantic (I_Sigma) inconsistency = " << result.ISigma << std::endl;
        std::cout << " - " << Uppercase << " model semantic (I_Max) inconsistency = " << result.IMax << std::endl;
        std::cout << " - " << Uppercase << " model semantic (I_Hit) inconsistency = " << result.IHit << std::endl;
    }
    return Pip;
}

int main() {


    std::vector<std::pair<std::string, std::string>> dataset;
    dataset.emplace_back("data/trient/miner/pos.sdecl", "data/trient/miner/neg.sdecl");
    dataset.emplace_back("data/trient/explainer/pos.sdecl", "data/trient/explainer/neg.sdecl");
    trient(dataset);
    aalta::aalta_formula::destroy();


    //specific_testing();
    //testing3();
  // pipeline_all();
    //romano2();
   // test();
   // testing2();
}
