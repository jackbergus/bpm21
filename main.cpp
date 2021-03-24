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
#include <fstream>
#include <DADParser.h>
#include <pipeline/input_pipeline.h>

#include "declare/DataTraceParse.h"
#include "graphs/third-party-wrappers/ParseFFLOATDot.h"
#include "pipeline/foreign_data_loads.h"

#if 0
void test_data_predicate() {
    DataPredicate x_1{"x", LT, 0.2};
    DataPredicate x_2{"x", GEQ, -1.0};
    std::cout << x_1 << std::endl;
    std::cout << x_2 << std::endl;
    x_1.intersect_with(x_2);
    std::cout << x_1 << std::endl;

    DataPredicate x_3{"x", NEQ, 5.0};
    x_1.intersect_with(x_3);
    std::cout << x_1 << std::endl;

    DataPredicate x_4{"x", NEQ, 0.001};
    x_1.intersect_with(x_4);
    std::cout << x_1 << std::endl;
}

#include <utils/stream_out.h>
#include <declare/DeclareDataAware.h>

std::ostream &operator<<(std::ostream &os, const std::variant<std::string, double> &insertion) {
    if (std::holds_alternative<std::string>(insertion))
        return os << std::get<std::string>(insertion);
    else
        return os << std::get<double >(insertion);
}

void test() {
    /*{
        input_pipeline Pip{"fa"};
        Pip.run_pipeline("ex_1.txt", true);
        std::unordered_set<std::string> SigmaAll;
        {
            std::ofstream f{"eq_classes_1.txt"};
            Pip.print_equivalence_classes(f);
        }
        {
            Pip.print_atomized_traces("log_1.txt", "log_atomized_1", SigmaAll, true);
        }
        {
            std::ofstream f{"ex_1_sigma.txt"};
            Pip.print_sigma(f);
        }
        {
            std::ofstream f{"graph_1.dot"};
            std::string single_line{"single_line_clause_1.txt"};
            Pip.decompose_genmodel_for_tiny_graphs(SigmaAll, single_line, false).dot(f, false);
            f.flush(); f.close();
        }
    }*/
    {
        input_pipeline Pip{"fa"};
        Pip.run_pipeline("ex_2.txt", true);
        std::unordered_set<std::string> SigmaAll;
        {
            std::ofstream f{"eq_classes_2.txt"};
            Pip.print_equivalence_classes(f);
        }
        {
            Pip.print_atomized_traces("log_2.txt", "log_atomized_2", SigmaAll, true);
        }
        {
            std::ofstream f{"ex_2_sigma.txt"};
            Pip.print_sigma(f);
        }
        {
            std::ofstream f{"graph_2.dot"};
            std::string single_line{"single_line_clause_2.txt"};
            Pip.decompose_genmodel_for_tiny_graphs(SigmaAll, single_line, false).dot(f, false);
            f.flush(); f.close();
        }
    }
}

void pipeline(size_t n, size_t mod, const std::vector<size_t>& lengths) {

    std::unordered_set<size_t> to_test_correct;
    std::set<std::vector<std::string>> relevant_traces;
    std::vector<std::vector<std::vector<std::string>>> M;

    std::string N =  std::to_string(n);
    std::string converted_file = "/media/giacomo/Data/bz/CLionProjects/bpm21/data/converted/"+N+"constr";
    std::vector<std::string> LOGS;
#if 0

    LOGS.emplace_back("/media/giacomo/Data/bz/CLionProjects/bpm21/data/curr/"+N+" CONSTRAINTS/3_mod/length_10.xes");
#else
    for (size_t i = 0; i<=mod; i++) {
        std::string I = std::to_string(i);
        for (size_t single_len : lengths) {
            if (i == 0)
                to_test_correct.insert(LOGS.size());
            LOGS.emplace_back("/media/giacomo/Data/bz/CLionProjects/bpm21/data/curr/"+N+" CONSTRAINTS/"+I+"_mod/length_"+std::to_string(single_len)+".xes");
        }
    }
#endif

    input_pipeline Pip{"p"};
    Pip.run_pipeline(converted_file + ".txt", true);
    std::unordered_set<std::string> SigmaAll;
    {
        std::ofstream f{converted_file+"_eq_classes.txt"};
        Pip.print_equivalence_classes(f);
    }
    //
    {
        std::ofstream f{converted_file+"_sigma.txt"};
        Pip.print_sigma(f);
    }
    {
        size_t i = 0;
        for (const std::string& elements : LOGS) {
            std::cout << "\t\t- log dumping: " << elements << std::endl;
            auto log = Pip.print_atomized_traces(elements, elements+"_atomized_1", SigmaAll, true);
            if (to_test_correct.contains(i)) {
                std::cout << "\t\t\t adding relevant traces" << elements << std::endl;
                M.emplace_back(log);
            }
            i++;
        }
    }
    {
        std::ofstream f{converted_file+"_graph.dot"};
        std::string single_line{converted_file+"_single_line_clause.txt"};
        auto g= Pip.decompose_genmodel_for_tiny_graphs(SigmaAll, single_line, false);
        g.dot(f, false);
        f.flush(); f.close();
        for (const std::vector<std::vector<std::string>>& log : M) {
            size_t j = 1;
            for (const std::vector<std::string>& trace : log) {
                std::cout << trace.size() << " testing " << j << "..." << std::endl;
                g.test_correctness(trace);
                j++;
            }
        }
    }
}

void specific_testing() {
    input_pipeline Pip{"p"};
    std::cout << "run_pipeline" << std::endl;
    Pip.run_pipeline("/media/giacomo/Data/bz/CLionProjects/bpm21/data/converted/3constr_testing.txt", true);

    std::unordered_set<size_t> to_test_correct;
    std::unordered_set<std::string> SigmaAll;
    std::vector<std::string> LOGS;
    std::set<std::vector<std::string>> relevant_traces;
    for (size_t i = 0; i<=0; i++) {
        std::string I = std::to_string(i);
        for (size_t single_len : {10,15,20,25,30}) {
            if (std::filesystem::exists("/media/giacomo/Data/bz/CLionProjects/bpm21/data/curr/3 CONSTRAINTS/"+I+"_mod/length_"+std::to_string(single_len)+".xes")) {
                if (i == 0)
                    to_test_correct.insert(LOGS.size());
                std::cout << "()Adding to vector: /media/giacomo/Data/bz/CLionProjects/bpm21/data/curr/3 CONSTRAINTS/"+I+"_mod/length_"+std::to_string(single_len)+".xes" << std::endl;
                LOGS.emplace_back("/media/giacomo/Data/bz/CLionProjects/bpm21/data/curr/3 CONSTRAINTS/"+I+"_mod/length_"+std::to_string(single_len)+".xes");
            }
        }
    }
    {
        std::ofstream f{"3_sigma.txt"};
        Pip.print_sigma(f);
    }
    std::vector<std::vector<std::vector<std::string>>> M;
    size_t i = 0;
    for (const std::string& elements : LOGS) {
        std::cout << "\t\t- log dumping: " << elements << std::endl;
        auto log = Pip.print_atomized_traces(elements, elements+"_atomized_1", SigmaAll, true);
        if (to_test_correct.contains(i)) {
            std::cout << "\t\t\t adding relevant traces" << elements << std::endl;
            M.emplace_back(log);
        }
        i++;
    }
    {
        std::ofstream f{"3_graph.dot"};
        std::string single_line{"3_single_line_clause.txt"};
        std::cout << " Decomposing the operand " << std::endl;
        auto g = Pip.decompose_genmodel_for_tiny_graphs(SigmaAll, single_line, false);
        g.dot(f, false);
        f.flush(); f.close();
        for (const std::vector<std::vector<std::string>>& log : M) {
            size_t j = 1;
            for (const std::vector<std::string>& trace : log) {
                std::cout << trace.size() << " testing " << j << "..." << std::endl;
                g.test_correctness(trace);
                j++;
            }
        }
    }

}

void romano() {
    input_pipeline Pip{"p"};
    Pip.run_pipeline( "/media/giacomo/Data/bz/CLionProjects/bpm21/data/additional/BPIC15_1.sdecl", false);
    {
        std::ofstream f{"/media/giacomo/Data/bz/CLionProjects/bpm21/data/additional/BPIC15_1.dot"};
        std::string single_line{"/media/giacomo/Data/bz/CLionProjects/bpm21/data/additional/BPIC15_1_single_line_clause_1.txt"};
        Pip.decompose_genmodel_for_tiny_graphs(Pip.atom_universe, single_line, true).dot(f, false);
        f.flush(); f.close();
    }
}

void romano2() {
    for (size_t i = 5; i!=0; i--) {
        std::cout << "~~" << i << "~~~~~~~~~~~~~~~" << std::endl;
        input_pipeline Pip{"p"};
        std::string I = std::to_string(i);
        auto S = Pip.atom_universe;
        S.insert("*");
        Pip.run_pipeline( "/media/giacomo/Data/bz/CLionProjects/bpm21/data/additional/BPIC15_"+I+".sdecl", false);
        {
            std::ofstream f{"/media/giacomo/Data/bz/CLionProjects/bpm21/data/additional/BPIC15_"+I+".dot"};
            std::string single_line{"/media/giacomo/Data/bz/CLionProjects/bpm21/data/additional/BPIC15_"+I+"_single_line_clause_1.txt"};
            Pip.decompose_genmodel_for_tiny_graphs(S, single_line, true).dot(f, false);
            f.flush(); f.close();
        }
    }

}

void old() {





#if 1
    /*{
        input_pipeline Pip{"fa"};
        Pip.run_pipeline("ex_3.txt");
        {
            std::ofstream f{"eq_classes_3.txt"};
            Pip.print_equivalence_classes(f);
        }

    }*/
#if 1
    {
        input_pipeline Pip{"fa"};
        Pip.run_pipeline("ex_3.txt", true);
        std::unordered_set<std::string> SigmaAll;
        {
            std::ofstream f{"eq_classes_3.txt"};
            Pip.print_equivalence_classes(f);
        }
        {
            Pip.print_atomized_traces("log_1.txt", "log_atomized_1", SigmaAll, true);
        }
        {
            std::ofstream f{"graph_1.dot"};
            std::string single_line{"single_line_clause_1.txt"};
            Pip.decompose_genmodel_for_tiny_graphs(SigmaAll, single_line, false).dot(f, false);
            f.flush(); f.close();
        }
    }
    /*{
        input_pipeline Pip{"fa"};
        Pip.run_pipeline("ex_1.txt");
        std::unordered_set<std::string> SigmaAll;
        {
            std::ofstream f{"eq_classes_1.txt"};
            Pip.print_equivalence_classes(f);
        }
        {
            Pip.print_atomized_traces("log_1.txt", "log_atomized_1", SigmaAll, true);
        }
        {
            std::ofstream f{"graph_1.dot"};
            std::string single_line{"single_line_clause_1.txt"};
            Pip.decompose_genmodel_for_tiny_graphs(SigmaAll, single_line, false).dot(f, false);
            f.flush(); f.close();
        }
    }
    {
        input_pipeline Pip{"fa"};
        Pip.run_pipeline("ex_2.txt");
        std::unordered_set<std::string> SigmaAll;
        {
            std::ofstream f{"eq_classes_2.txt"};
            Pip.print_equivalence_classes(f);
        }
        {
            Pip.print_atomized_traces("log_2.txt", "log_atomized_2", SigmaAll, true);
        }
        {
            std::ofstream f{"graph_2.dot"};
            std::string single_line{"single_line_clause_2.txt"};
            Pip.decompose_genmodel_for_tiny_graphs(SigmaAll, single_line, false).dot(f, false);
            f.flush(); f.close();
        }
    }*/
#endif

/*
    std::ifstream f{"/media/giacomo/Data/bz/CLionProjects/bpm21/single_line_clause_1.txt_graph_1"};
    ParseFFLOATDot test;
    auto parsing_result = test.parse(f, {"fa1", "fa2", "fa3", "a", "b"});
    std::ofstream g1{"back.dot"};
    parsing_result.dot(g1, false);
    g1.flush(); g1.close();*/

    /*FLLOATScriptRunner psr;
    psr.process_expression("/media/giacomo/Data/bz/CLionProjects/bpm21/single_line_clause_1.txt");
*/

#else
    DataTraceParse dtp;
    std::ifstream file{"log_test.txt"};

    for (const auto& trace : dtp.load(file))
        std::cout << trace <<std::endl;
    /*struct IntPrevNext  {
    size_t getPrev(size_t elem) const {
        return (elem == 0) ? elem : (elem-1);
    }
    size_t getNext(size_t elem) const {
        return (elem == std::numeric_limits<size_t>::max()) ? elem : elem+1;
    }
    size_t distance(size_t l, size_t r) const {
        return std::labs(l-r);
    }
};

segment_partition_tree<size_t, IntPrevNext> S(0, 10);
    insert_interval(S.indexer, S.element, 0UL, 10UL);
    insert_interval(S.indexer, S.element, 0UL, 4UL);
    insert_interval(S.indexer, S.element, 5UL, 5UL);
    insert_interval(S.indexer, S.element, 6UL, 10UL);
    minimize_tree(S.indexer, S.element);
    std::cout << S << std::endl;*/
#endif
}

void pipeline_all() {
    {
        std::vector<size_t> lengths = {10/*,15,20,25,30*/};
        //pipeline(3, 0, lengths);
        //pipeline(5, 0, lengths);
    }
    {
        std::vector<size_t> lengths = {15/*,20,25,30*/};
        //pipeline(7, 0, lengths);
    }
    {
        std::vector<size_t> lengths = {20/*,25,30*/};
        pipeline(10, 0, lengths);
    }
}

void testing2() {
    input_pipeline Pip{"p"};
    Pip.run_pipeline( "/media/giacomo/Data/bz/CLionProjects/bpm21/data/testing.txt", false);
    {
        std::ofstream f{"/media/giacomo/Data/bz/CLionProjects/bpm21/data/testing.dot"};
        std::string single_line{"/media/giacomo/Data/bz/CLionProjects/bpm21/data/testing_single_line_clause_1.txt"};
        auto cp = Pip.atom_universe;
        Pip.decompose_genmodel_for_tiny_graphs(cp, single_line, false).dot(f, false);
        f.flush(); f.close();
    }
}


void testing() {
    input_pipeline Pip{"p"};
    Pip.run_pipeline( "/media/giacomo/Data/bz/CLionProjects/bpm21/data/testing.sdecl", false);
    {
        std::ofstream f{"/media/giacomo/Data/bz/CLionProjects/bpm21/data/testing.dot"};
        std::string single_line{"/media/giacomo/Data/bz/CLionProjects/bpm21/data/testing_single_line_clause_1.txt"};
        auto cp = Pip.atom_universe;
        Pip.decompose_genmodel_for_tiny_graphs(cp, single_line, true).dot(f, false);
        f.flush(); f.close();
    }
}

void testing3() {
    input_pipeline Pip{"p"};
    Pip.run_pipeline( "/media/giacomo/Data/bz/CLionProjects/bpm21/data/trient/learner/pos.sdecl", false);
    std::unordered_set<std::string> SigmaAll;
    {
        std::ofstream f{"eq_classes_2.txt"};
        Pip.print_equivalence_classes(f);
    }
    {
        Pip.print_atomized_traces("log_2.txt", "log_atomized_2", SigmaAll, true);
    }
    {
        std::ofstream f{"ex_2_sigma.txt"};
        Pip.print_sigma(f);
    }
}

#endif

#ifdef TRUE
#undef TRUE
#endif
#ifdef FALSE
#undef FALSE
#endif

#include <formula/aalta_formula.h>
#include <carchecker.h>



double check_formula(const std::string& ltlf_string, bool simplify = false) {
    std::cout << "check_formula" << std::endl;
    aalta::aalta_formula* af;
    //set tail id to be 1
    af = aalta::aalta_formula::TAIL ();
    af = aalta::aalta_formula(ltlf_string.c_str(), true).unique();
    //af = af->nnf ();
    af = af->add_tail ();
    af = af->remove_wnext ();
    if (simplify) {
        af = af->simplify ();
        af = af->split_next ();
    }
    aalta::CARChecker checker (af, false, true);
    bool res = checker.check ();
    //printf ("%s\n", res ? "sat" : "unsat");
    return checker.get_inconsistency_measure(res);
}

#include    <ReadGraph.h>
#define     LIMIT_VISIT_COST     (0.001)
#define     LIMIT_SIMPLIFIC_THR  (3)
#define     APPROX_PATH_COST     (0.999999)


void load_to_ReadGraph(const std::string& epsilon, const FlexibleFA<std::string, size_t>& x, ReadGraph& out) {
    size_t max_node_id = 0;
    size_t edge_count = 0;
    FlexibleFA<std::string, size_t> input = x;
    if (input.initial_nodes.size()>1) {
        size_t I = input.addNewNodeWithLabel(epsilon);
        for (size_t init : input.initial_nodes) {
            input.addNewEdgeFromId(I, init, 0);
        }
        input.initial_nodes.clear();
        input.initial_nodes.insert(I);
    }
    assert(input.initial_nodes.size() == 1);
    if (input.final_nodes.size() > 1) {
        size_t F = input.addNewNodeWithLabel(epsilon);
        for (size_t fini : input.final_nodes) {
            input.addNewEdgeFromId(fini, F, 0);
        }
        input.final_nodes.clear();
        input.final_nodes.insert(F);
    }
    assert(input.final_nodes.size() == 1);
    {
        std::ofstream F{"file.dot"};
        input.dot(F, false);
        F.close();
    }
    auto v = input.getNodeIds();
    for (const auto& nodeId : v) {
        max_node_id = std::max(max_node_id, nodeId);
        for (const auto& edge : input.outgoingEdges(nodeId)) {
            edge_count++;
        }
    }
    out.init(max_node_id, edge_count, *input.initial_nodes.begin(), *input.final_nodes.begin());
    for (const auto& nodeId : v) {
        out.addNode(nodeId, input.getNodeLabel(nodeId));
        for (const auto& edge : input.outgoingEdges(nodeId)) {
            std::cout << nodeId << " " << edge.second << std::endl;
            out.addEdge(nodeId, edge.second, (nodeId == edge.second) ? LIMIT_VISIT_COST : APPROX_PATH_COST);
        }
    }
    std::cout << " MAX = " << (max_node_id) << std::endl;
    out.finalizeEdgesMatrix(1.0);
}

#include "submodules/probtrace/include/export/SemanticInconsistencyAdapter.h"


#include <lemon/smart_graph.h>
#include <lemon/connectivity.h>
#include <lemon/dijkstra.h>
#include <lemon/dfs.h>

double align(const FlexibleFA<std::string, size_t>& x, const std::string& path, const std::string& epsilon) {

    lemon::SmartDigraph g;
    lemon::SmartDigraph::ArcMap<int> costMap(g);

    FlexibleFA<std::string, size_t> graph = x;
    //FlexibleFA<std::string, size_t> printer;
    {
        size_t ini = graph.addNewNodeWithLabel(epsilon);
        for (size_t s : graph.initial_nodes) {
            graph.addNewEdgeFromId(ini, s, 0);
        }
        graph.initial_nodes.clear();
        graph.initial_nodes.insert(ini);
    }
    if (graph.final_nodes.size()> 1) {
        size_t fin = graph.addNewNodeWithLabel(epsilon);
        for (size_t f : graph.final_nodes) {
            graph.addNewEdgeFromId(f, fin, 0);
        }
        graph.final_nodes.clear();
        graph.final_nodes.insert(fin);
    }

    size_t tmpIni = *graph.initial_nodes.begin();
    size_t tmpFin = *graph.final_nodes.begin();
    std::vector<size_t> V = graph.getNodeIds();
    std::unordered_map<size_t, size_t> map;
    size_t Vsize = V.size();
    size_t pathSize = path.size();
    size_t init = -1, fini = -1;

    for (size_t i = 0, N = Vsize; i<N; i++) {
        size_t actualId = V.at(i);
        std::string label = graph.getNodeLabel(V.at(i));
        map[V.at(i)] = i;
        for (size_t j = 0, M = pathSize; j<M+1; j++) {
            size_t cmp = (i * (M+1))+j;
            //size_t tmpp = printer.addNewNodeWithLabel(label);
            if ((actualId == tmpIni) && (j == 0)) {
                init = cmp;
                //printer.addToInitialNodesFromId(tmpp);
                ///std::cout <<"=>";
            }
            ///std::cout << cmp << " ~ " << label;
            if ((actualId == tmpFin) && (j == M)) {
                fini = cmp;
                //printer.addToFinalNodesFromId(tmpp);
                ///std::cout << "XXX";
            }
            ///std::cout << std::endl;
            auto node = g.addNode();
            assert(g.id(node) == cmp);
        }
    }

    for (size_t i = 0, N = Vsize; i<N; i++) {
        size_t v = V.at(i);

        for (const std::pair<size_t, size_t>& edge : graph.outgoingEdges(v)) {
            for (size_t t = 0, M = pathSize; t < M + 1; t++) {
                size_t src = i*(M+1) + t;
                auto srcNode = lemon::SmartDigraph::nodeFromId(src);
                size_t dst_1 = map[edge.second]*(M+1) + t;

                std::string element = graph.getNodeLabel(edge.second);
                if (element == epsilon) {
                    ///std::cout << src << "-[0]->"  << dst_1 << std::endl;
                    //printer.addNewEdgeFromId(src, dst_1, 0);
                    costMap[g.addArc(srcNode, lemon::SmartDigraph::nodeFromId(dst_1))] = 0; // Epsilon moves have zero costs
                } else {
                    ///std::cout << src << "-[1]->"  << dst_1 << std::endl;
                    //printer.addNewEdgeFromId(src, dst_1, 1);
                    costMap[g.addArc(srcNode, lemon::SmartDigraph::nodeFromId(dst_1))] = 1; // Non-epsilon moves have costs!
                }

                if (t <= M-1) {
                    std::string t_str{path.at(t)};
                    size_t dst_2 = map[edge.second]*(M+1) + (t+1);
                    if (t_str == element) {
                        ///std::cout << src << "-[0]->"  << dst_2 << std::endl;
                        //printer.addNewEdgeFromId(src, dst_2, 0);
                        costMap[g.addArc(srcNode, lemon::SmartDigraph::nodeFromId(dst_2))] = 0;
                    } else {
                        ///std::cout << src << "-[1]->"  << dst_2 << std::endl;
                        //printer.addNewEdgeFromId(src, dst_2, 1);
                        costMap[g.addArc(srcNode, lemon::SmartDigraph::nodeFromId(dst_2))] = 1;
                    }

                    size_t dst_3 = i*(M+1) + (t+1);
                    ///std::cout << src << "-[1]->"  << dst_3 << std::endl;
                    //printer.addNewEdgeFromId(src, dst_3, 1);
                    costMap[g.addArc(srcNode, lemon::SmartDigraph::nodeFromId(dst_3))] = 1;
                }
            }
        }
    }


    /*{
        printer.pruneUnreachableNodes();
        std::ofstream f{"file.dot"};
        printer.dot(f, false);
        f.close();
    }*/

    lemon::Dijkstra<lemon::SmartDigraph, lemon::SmartDigraph::ArcMap<int>> dij{g, costMap};
    dij.init();
    dij.addSource(lemon::SmartDigraph::nodeFromId(init));
    dij.start();

    auto& d = dij.distMap();
    return (double)d[lemon::SmartDigraph::nodeFromId(fini)];
}

struct result_semantish_align {
    double ISigma;
    double IMax;
    double IHit;

    result_semantish_align(double iSigma, double iMax, double iHit) : ISigma(iSigma), IMax(iMax), IHit(iHit) {}
    result_semantish_align() : result_semantish_align(0,0,0) {}
    result_semantish_align(const result_semantish_align&) = default;
    result_semantish_align(result_semantish_align&&) = default;
    result_semantish_align& operator=(const result_semantish_align&) = default;
    result_semantish_align& operator=(result_semantish_align&&) = default;
};

struct for_semantisch_inconsistency {
    std::string epsilon;
    std::vector<FlexibleFA<std::string, size_t>> V;

    for_semantisch_inconsistency(const std::string& epsilon) : epsilon{epsilon} {}

    void collect_traces_from_model_clause_as_graph(const FlexibleFA<size_t, std::string>& g) {
        V.emplace_back(g.shiftLabelsToNodes());
    }

    result_semantish_align compute_in_triplicate(const std::vector<std::string>& log) {
        if (log.empty()) return {0.0, 0.0, 0.0};
        double result_sigma = std::numeric_limits<double>::max();
        double result_max = std::numeric_limits<double>::max();
        double result_hit = std::numeric_limits<double>::max();
        for (const std::string &trace : log) {
            double cost_sigma = 0.0;
            double cost_max = -std::numeric_limits<double>::max();
            double cost_hit = 0.0;
            for (const auto &graph : V) {
                double alignment = align(graph, trace, epsilon);
                cost_sigma += alignment;
                cost_max = std::max(alignment, cost_max);
                if (alignment > 0.0) cost_hit++;
            }
            result_sigma = std::min(result_sigma, cost_sigma);
            result_max = std::min(result_max, cost_max);
            result_hit = std::min(result_max, cost_hit);
        }
        return {result_sigma, result_max, result_hit};
    }
};

struct elements {
    std::pair<std::string, std::string> files;

    ltlf                            pos_model;
    ltlf                            neg_model;
    FlexibleFA<size_t, std::string> pos_global_graph;
    double                          pos_graph_size;
    FlexibleFA<size_t, std::string> neg_global_graph;
    double                          neg_graph_size;
    for_semantisch_inconsistency    pos_trace_unfolder;
    for_semantisch_inconsistency    neg_trace_unfolder;

    elements(const std::string& epsilon) : pos_trace_unfolder{epsilon}, neg_trace_unfolder{epsilon} {}
    /*std::unordered_set<std::string> collect_label_intersection(bool isPos) {
        std::unordered_set<std::string> S;
        bool first = true;
        for (const std::vector<struct path_info>& x : (isPos ? pos_trace_unfolder : neg_trace_unfolder)
                                                       .logs_from_different_clauses) {
            std::unordered_set<std::string> CL;
            for (const struct path_info& trace : x) {
                CL.insert(trace.path);
            }
            if (first) {
                S = CL;
                first = false;
            } else {
                if (!S.empty())
                    S = unordered_intersection(S, CL);
            }
        }
        return S;
    }*/

    /*elements(const std::string& varepsilon, bool doNotVisitLoopsTwice, size_t maxPathLength, double min_cost) :
    pos_trace_unfolder{varepsilon, doNotVisitLoopsTwice, maxPathLength, min_cost},
    neg_trace_unfolder{varepsilon, doNotVisitLoopsTwice, maxPathLength, min_cost} {};*/
};

void print_syntax_metrics(std::ostream& out, double d1, double d2, double d3) {
    out << " Syntactic distances " << std::endl;
    out << " a) Straightforward normalization " << std::endl;
    {
        double d1L = (d3 <= std::numeric_limits<double>::min()) ? std::numeric_limits<double>::infinity() : 1.0-(d1)/d3;
        double d2L = (d3 <= std::numeric_limits<double>::min()) ? std::numeric_limits<double>::infinity() :1.0-(d2)/d3;
        out << "    * Left distance from intersection: " << d1L << std::endl;
        out << "    * Right distance from intersection: " << d2L << std::endl;
        out << "    * Left distance from Right (Avg): " << ((d3 <= std::numeric_limits<double>::min()) ? std::numeric_limits<double>::infinity() :((d1L+d2L)/2.0)) << std::endl;
    }
    out << " b) Kernel-based normalization " << std::endl;
    {
        double d1L = ((d1*d3) <= std::numeric_limits<double>::min()) ? std::numeric_limits<double>::infinity() : std::sqrt(1.0 - 1.0*(d1/std::sqrt(d1*d3)));
        double d2L = ((d2*d3) <= std::numeric_limits<double>::min()) ? std::numeric_limits<double>::infinity() : std::sqrt(1.0 - 1.0*(d2/std::sqrt(d2*d3)));
        out << "    * Left distance from intersection: " << d1L << std::endl;
        out << "    * Right distance from intersection: " << d2L << std::endl;
        out << "    * Left distance from Right (Avg): " << ((d1L+d2L)/2.0) << std::endl;
    }
    out  << std::endl << std::endl;
}

#include "submodules/probtrace/include/distances/strings/Levenstein.h"

double alignment_distance(std::unordered_set<std::string>& left, std::unordered_set<std::string>& right) {
    double cost = 0.0;
    for (const std::string& L : left) {
        for (const std::string& R : right) {
            cost += GeneralizedLevensteinDistance(L, R);
        }
    }
    return cost;
}

void print_semantic_metrics(std::ostream& out, std::unordered_set<std::string>& left, std::unordered_set<std::string>& right) {
    out << " Semantic distances " << std::endl;
    out << " a) Finite Set Intersection " << std::endl;
    double L = left.size();
    double R = right.size();
    double LA = alignment_distance(left, left);
    double RA = alignment_distance(right, right);
    {
        double num = unordered_intersection(left, right).size();
        out << "    * Non-Normalized distance: " << num << std::endl;
        out << "    * Normalized distance: " << num/std::sqrt(L*R) << std::endl;
    }
    out << " b) Alignment-based Intersection " << std::endl;
    {
        double num = alignment_distance(left, right);
        out << "    * Non-Normalized distance: " << num << std::endl;
        out << "    * Normalized distance: " << num/std::sqrt(LA*RA) << std::endl;
    }
    out  << std::endl << std::endl;
}

// A utility function to find the vertex with minimum distance value, from
// the set of vertices not yet included in shortest path tree
size_t minDistance(std::vector<double>& dist, std::vector<bool>& sptSet)
{
    // Initialize min value
    double min = std::numeric_limits<double>::max();
    size_t min_index = -1;
    size_t V = dist.size();

    for (int v = 0; v < V; v++)
        if (sptSet[v] == false && dist[v] <= min)
            min = dist[v], min_index = v;

    return min_index;
}


void trient(const std::vector<std::pair<std::string, std::string>>& models) {

    std::vector<struct elements> mappings;
    bool doNotVisitLoopsTwice = false;
    bool semantisch = true;

    // Loading the log
    std::string atoms{"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"};
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

    double k = LIMIT_SIMPLIFIC_THR;
    double min_cost = std::pow(APPROX_PATH_COST, max_trace_length-2)*std::pow(LIMIT_VISIT_COST, k);
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

        std::cout << " ** Loading total pos graph: " << pos_neg.first << std::endl;
        ref.pos_global_graph = Pip.decompose_genmodel_for_tiny_graphs(SigmaAll2, pos_neg.first+"out", false, true, map);
        ref.pos_graph_size = ref.pos_global_graph.size();
        size_t X = Pip.GraphVector.size();
        {
            std::stringstream ss;
            ss << ref.pos_model;
            std::cout << " - Positive model syntax inconsistency = " << check_formula(ss.str(), X > LIMIT_SIMPLIFIC_THR) << std::endl;
        }
        std::cout << " ** Loading chunks pos graph: " << pos_neg.first << std::endl;
        if (semantisch)  {
            for (const auto& g : Pip.GraphVector) {
                ref.pos_trace_unfolder.collect_traces_from_model_clause_as_graph(g);
            }
            auto result = ref.pos_trace_unfolder.compute_in_triplicate(pos_atom_log);
            std::cout << " - Positive model semantic (I_Sigma) inconsistency = " << result.ISigma << std::endl;
            std::cout << " - Positive model semantic (I_Max) inconsistency = " << result.IMax << std::endl;
            std::cout << " - Positive model semantic (I_Hit) inconsistency = " << result.IHit << std::endl;
        }


        ////// Negative Model
        std::cout << " ** Loading negative model: " << pos_neg.second << std::endl;
        Pip.run_pipeline( pos_neg.second, false);
        ref.neg_model = Pip.model;
        std::cout << " ** Loading total neg graph: " << pos_neg.first << std::endl;
        ref.neg_global_graph = Pip.decompose_genmodel_for_tiny_graphs(SigmaAll2, pos_neg.first+"out", false, true, map);
        ref.neg_graph_size = ref.neg_global_graph.size();
        size_t Y = Pip.GraphVector.size();
        {
            std::stringstream ss;
            ss << ref.neg_model;
            std::cout << " - Negative model syntax inconsistency = " << check_formula(ss.str(), Y > LIMIT_SIMPLIFIC_THR) << std::endl;
        }
        {
            std::stringstream ss;
            ss << ltlf::And(ref.neg_model, ref.pos_model);
            std::cout << " - Positive&Negative syntax inconsistency = " << check_formula(ss.str(), (X+Y)>(LIMIT_SIMPLIFIC_THR*2)) << std::endl;
        }
        if (semantisch) {
            std::cout << " ** Loading chunks neg graph: " << pos_neg.first << std::endl;
            for (const auto& g : Pip.GraphVector) {
                ref.neg_trace_unfolder.collect_traces_from_model_clause_as_graph(g);
            }
            auto result = ref.neg_trace_unfolder.compute_in_triplicate(pos_atom_log);
            std::cout << " - Negative model semantic (I_Sigma) inconsistency = " << result.ISigma << std::endl;
            std::cout << " - Negative model semantic (I_Max) inconsistency = " << result.IMax << std::endl;
            std::cout << " - Negative model semantic (I_Hit) inconsistency = " << result.IHit << std::endl;
        }

        N++;
    }

    for (size_t iter = 0; iter < N; iter++) {
        for (size_t i = 0; i < iter; i++) {
            std::cout << "Dataset comparison : " << i << " vs. " << iter << std::endl;

            auto& leftI = mappings[i];
            auto& leftJ = mappings[iter];

            {
                std::cout << " [Computing the intersection:] " << std::endl;

                {
                    ltlf conj_pos = ltlf::And(leftI.pos_model, leftJ.pos_model);
                    std::stringstream ss;
                    ss << conj_pos;
                    double inc = check_formula(ss.str());
                    std::cout << " - PositiveL&PositiveR syntax inconsistency = " << inc << std::endl;
                    if (inc == 0.0) {
                        FlexibleFA<size_t, std::string> pos_graph =  Pip.decompose_ltlf_for_tiny_graphs(conj_pos, SigmaAll2, "cross1", false, true,
                                                                                                        {});
                        std::cout << "° Positive datasets" << std::endl;
                        print_syntax_metrics(std::cout, leftI.pos_graph_size, leftJ.pos_graph_size, pos_graph.size());
                    }
                }
                {
                    ltlf conj_neg = ltlf::And(leftI.neg_model, leftJ.neg_model);
                    std::stringstream ss;
                    ss << conj_neg;
                    double inc = check_formula(ss.str());
                    std::cout << " - NegativeL&NegativeR syntax inconsistency = " << inc << std::endl;
                    if (inc == 0.0) {
                        FlexibleFA<size_t, std::string> pos_graph = Pip.lydia_script.generate_graph(SigmaAll2,conj_neg);
                        std::cout << "° Negative datasets" << std::endl;
                        print_syntax_metrics(std::cout, leftI.neg_graph_size, leftJ.neg_graph_size, pos_graph.size());
                    }
                }


            }

            if (false) {
                {
                    std::cout << "° Positive datasets" << std::endl;
                    auto Ipos = leftI.collect_label_intersection(true);
                    auto Jpos = leftJ.collect_label_intersection(true);
                    print_semantic_metrics(std::cout, Ipos, Jpos);
                }

                {

                    std::cout << "° Negative datasets" << std::endl;
                    auto Ineg = leftI.collect_label_intersection(false);
                    auto Jneg = leftJ.collect_label_intersection(false);
                    print_semantic_metrics(std::cout, Ineg, Jneg);
                }
            }
            std::cout << std::endl << std::endl << std::endl;
        }
    }


}

int main() {

#if 0
    std::vector<std::pair<std::string, std::string>> dataset;
    dataset.emplace_back("data/trient/miner/pos.sdecl", "data/trient/miner/neg.sdecl");
    dataset.emplace_back("data/trient/learner/pos.sdecl", "data/trient/learner/neg.sdecl");
    trient(dataset);
    aalta::aalta_formula::destroy();
#endif

    FlexibleFA<std::string, size_t> graph;
    size_t src = graph.addNewNodeWithLabel("a");
    size_t dst1 = graph.addNewNodeWithLabel("c");
    size_t dst2 = graph.addNewNodeWithLabel("d");
    graph.addNewEdgeFromId(src, dst1, 0);
    graph.addNewEdgeFromId(src, dst2, 0);
    graph.addToFinalNodesFromId(dst1);
    graph.addToFinalNodesFromId(dst2);
    graph.addToInitialNodesFromId(src);
    std::cout << align(graph, "defg", ".") << std::endl;
    std::cout << align(graph, "ac", ".") << std::endl;
    std::cout << align(graph, "acc", ".") << std::endl;
    std::cout << align(graph, "acd", ".") << std::endl;
    std::cout << align(graph, "da", ".") << std::endl;

    //specific_testing();
    //testing3();
  // pipeline_all();
    //romano2();
   // test();
   // testing2();
}
