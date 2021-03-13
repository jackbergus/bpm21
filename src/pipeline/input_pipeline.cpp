/*
 * input_pipeline.cpp
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

 
//
// Created by giacomo on 11/03/21.
//

#include <declare/DeclareModelParse.h>
#include "pipeline/input_pipeline.h"
#include "pipeline/SimpleXESSerializer.h"

#ifdef TRUE
#undef TRUE
#endif
#ifdef FALSE
#undef FALSE
#endif

void input_pipeline::print_sigma(std::ostream &os) {
    os << "Sigma " << std::endl << std::endl;
    os << " * Single acts: " << std::endl;
    os << "================" << std::endl;
    for (const auto& ref : act_atoms) {
        os << "    " << ref << std::endl;
    }
    os << std::endl << std::endl;
    os << " * Sigma Boxes: " << std::endl;
    os << "================" << std::endl;
    std::pair<std::string, size_t> cp;
    for (const auto& k : interval_map) {
        os << "   - " << k.first << std::endl;
        cp.first = k.first;
        for (size_t i = 0, N = max_ctam_iteration.at(cp.first); i<N; i++) {
            cp.second = i;
            os << "        * "
               << clause_to_atomization_map.at(cp)
               << " --> "
               << k.second.at(i)
               << std::endl;
        }
    }


    os << std::endl << std::endl;
    os << " * Predicate Conversions: " << std::endl;
    os << "==========================" << std::endl;
    for (const auto& k : Mcal) {
        os << "   - " << k.first<< " --> " << k.second << std::endl;
    }
}

input_pipeline::input_pipeline(const std::string &fresh_atom_label) : fresh_atom_label{fresh_atom_label} {
    count_fresh_atoms = 1;
    model = ltlf::True();
}

std::string input_pipeline::generate_fresh_atom() {
    return fresh_atom_label+std::to_string(count_fresh_atoms++);
}

void input_pipeline::run_pipeline(const std::string &file) {
    final_model = model;
    init_pipeline(file);

    if ((!double_map.empty()) || (!(string_map.empty()))) {
        decompose_and_atomize();
        final_model = setInterpretCompoundSubatom(model);
        std::cout << "Atomized = " << final_model << std::endl;
    }
}

input_pipeline::semantic_atom_set input_pipeline::atom_decomposition(const std::string &act, bool isNegated) {
    semantic_atom_set S;
    auto it = interval_map.find(act);
    if (it == interval_map.end()) {
        assert(act_atoms.contains(act));
        S.insert(act);
    } else {
        std::pair<std::string, size_t> cp;
        cp.first = act;
        for (size_t i = 0, N = max_ctam_iteration.at(act); i<N; i++) {
            cp.second = i;
            S.insert(clause_to_atomization_map.at(cp));
        }
    }
    return isNegated ? unordered_difference(atom_universe, S) : S;
}

input_pipeline::semantic_atom_set input_pipeline::interval_decomposition(const DataPredicate &pred, bool isNegated) {
    semantic_atom_set S;
    if (pred.isStringPredicate()) {
        const auto V = std::get<0>(pred.decompose_into_intervals());
        auto& ref = string_map.at(pred.label).at(pred.var);
        for (const auto& cp : V) {
            for (const auto& I : ref.findInterval(cp.first, cp.second)) {
                DataPredicate dp{pred.label, pred.var, I.first, I.second};
                assert(Mcal.contains(dp));
                auto v = Mcal.at(dp);
                S.insert(v.begin(), v.end());
            }
        }
    } else {
        const auto V = std::get<1>(pred.decompose_into_intervals());
        auto& ref = double_map.at(pred.label).at(pred.var);
        for (const auto& cp : V) {
            for (const auto& I : ref.findInterval(cp.first, cp.second)) {
                DataPredicate dp{pred.label, pred.var, I.first, I.second};
                assert(Mcal.contains(dp));
                auto v = Mcal.at(dp);
                S.insert(v.begin(), v.end());
            }
        }
    }
    return isNegated ? unordered_difference(atom_universe, S) : S;
}

ltlf input_pipeline::setInterpretCompoundSubatom(const ltlf &formula) {
    if (formula.is_compound_predicate) {
        switch (formula.casusu) {
            case ACT: {
                assert(act_atoms.contains(formula.act));
                return formula;
            }
            case OR: {
                assert(formula.args.at(0).is_negated);
                semantic_atom_set left= atom_decomposition(formula.args.at(0).act);
                {
                    semantic_atom_set right= _setInterpretCompoundSubatom(formula.args.at(1));
                    left = unordered_difference(left, right);
                }
                auto right = extractLtlfFormulaFromSubAtoms(formula.args.at(1));
                for (const std::string& act : left) {
                    right = ltlf::Or(ltlf::Act(act).negate(), right);
                }
                return right.setBeingCompound(true);
            }
            case AND:
                return extractLtlfFormulaFromSubAtoms(formula).setBeingCompound(true);
            default:
                std::cerr << "Unexpected case: " << formula << std::endl;
                throw std::runtime_error("Unexpected case");
        }
    } else {
        switch (formula.casusu) {
            case ACT: {
                if (act_atoms.contains(formula.act))
                    return formula;
                else
                    return extractLtlfFormulaFromSubAtoms(formula);
            }
            case NUMERIC_ATOM:
                return extractLtlfFormulaFromSubAtoms(formula);

            case NEG_OF:
                return ltlf::Neg(setInterpretCompoundSubatom(formula.args.at(0)));

            case OR:
                return ltlf::Or(setInterpretCompoundSubatom(formula.args.at(0)),
                                setInterpretCompoundSubatom(formula.args.at(1)));

            case AND:
                return ltlf::And(setInterpretCompoundSubatom(formula.args.at(0)),
                                 setInterpretCompoundSubatom(formula.args.at(1)));

            case NEXT:
                return ltlf::Next(setInterpretCompoundSubatom(formula.args.at(0)));

            case UNTIL:
                return ltlf::Until(setInterpretCompoundSubatom(formula.args.at(0)),
                                   setInterpretCompoundSubatom(formula.args.at(1)));

            case RELEASE:
                return ltlf::Release(setInterpretCompoundSubatom(formula.args.at(0)),
                                     setInterpretCompoundSubatom(formula.args.at(1)));

            default:
                return formula;
        }
    }
}

ltlf input_pipeline::extractLtlfFormulaFromSubAtoms(const ltlf &formula) {
    semantic_atom_set S= _setInterpretCompoundSubatom(formula);
    bool first = true;
    ltlf result = ltlf::True().negate();
    for (const std::string& act : S) {
        if (first) {
            result = ltlf::Act(act);
            first = false;
        } else
            result = ltlf::Or(ltlf::Act(act), result);
    }
    return result;
}

input_pipeline::semantic_atom_set input_pipeline::_setInterpretCompoundSubatom(const ltlf &formula) {
    semantic_atom_set S;
    switch (formula.casusu) {
        case ACT:
            return atom_decomposition(formula.act, formula.is_negated);

        case NEG_OF:
            return unordered_difference(atom_universe, _setInterpretCompoundSubatom(formula.args.at(0)));

        case OR: {
            semantic_atom_set left = _setInterpretCompoundSubatom(formula.args.at(0));
            semantic_atom_set right = _setInterpretCompoundSubatom(formula.args.at(1));
            S.insert(left.begin(), left.end());
            S.insert(right.begin(), right.end());
            return S;
        }

            break;
        case AND:
            return unordered_intersection(_setInterpretCompoundSubatom(formula.args.at(0)),
                                          _setInterpretCompoundSubatom(formula.args.at(1)));

        case NEXT:
        case UNTIL:
        case RELEASE:
            throw std::runtime_error("ERROR: next, until, and Release cannot appear within a compound predicate!");

        case TRUE:
            return atom_universe;

        case FALSE:
            return S;

        case NUMERIC_ATOM: {
            auto atom = formula.numeric_atom;
            atom.asInterval();
            //std::cout << "DEBUG = " << formula.numeric_atom << " ~= " << atom << std::endl;
            assert(!formula.is_negated);
            return interval_decomposition(atom, formula.is_negated);
        }
    }
    assert(false);
}

void input_pipeline::decompose_and_atomize() {
    std::cout << "Generating the distinct intervals from the elements" << std::endl;

    for (auto ref = string_map.begin(); ref != string_map.cend(); ref++){
        for (auto& ref2 : ref->second) {
            std::vector<DataPredicate> result;
            ref2.second.perform_insertion();
            for (const auto& I : ref2.second.collect_intervals()) {
                result.emplace_back(ref->first, ref2.first, I.first, I.second);
            }
            interval_map[ref->first].emplace_back(result);
        }
    }
    //std::cout << std::setprecision(50);
    for (auto ref = double_map.begin(); ref != double_map.cend(); ref++){
        for (auto& ref2 : ref->second) {
            std::vector<DataPredicate> result;
            ref2.second.perform_insertion();
            for (const auto& I : ref2.second.collect_intervals()) {
                result.emplace_back(ref->first, ref2.first, I.first, I.second);
            }
            interval_map[ref->first].emplace_back(result);
        }
    }

    std::cout << "Generating the interval composition box" << std::endl;
    for (auto& ref: interval_map) {
        std::vector<std::vector<DataPredicate>> W;
        for (const auto& v : cartesian_product(ref.second)) {
            std::vector<DataPredicate> V;
            V.insert(V.end(), v.begin(), v.end());
            W.emplace_back(V);
        }
        ref.second = W;
        for (size_t i = 0, N = W.size(); i<N; i++) {
            std::string FA = generate_fresh_atom();
            atom_universe.insert(FA);
            clause_to_atomization_map[std::make_pair(ref.first, i)] = FA;
            for (const DataPredicate& pred : W.at(i)) {
                Mcal[pred].emplace_back(FA);
            }
        }
        max_ctam_iteration[ref.first] = W.size();
    }
}

void input_pipeline::init_pipeline(const std::string &file) {
    DeclareModelParse mp;
    std::ifstream stream (file);

    std::cout << "Parsing the file, and putting it in NNF, and simplifying it!" << std::endl;
    model = mp.load_model_to_semantics(stream).nnf().simplify().reduce().oversimplify();
    std::cout << "Model = " <<  model << std::endl;

    std::cout << "Collecting the atoms from the formula" << std::endl;
    pipeline_scratch(model, map1, act_universe, double_map, string_map);

    std::cout << "Collecting the atoms associated to no interval" << std::endl;
    for (const auto& act : act_universe) {
        auto it1 = double_map.find(act);
        bool test1 = (it1 == double_map.end()) || (it1->second.empty());
        auto it2 = string_map.find(act);
        bool test2 = (it2 == string_map.end()) || (it2->second.empty());
        if (test1 && test2) {
            act_atoms.insert(act);
            atom_universe.insert(act);
        }
    }
}

void input_pipeline::print_equivalence_classes(std::ostream &os) {
    std::pair<std::string, size_t> cp;
    for (const auto& k : interval_map) {
        cp.first = k.first;
        std::vector<std::string> uollano;
        for (size_t i = 0, N = max_ctam_iteration.at(cp.first); i<N; i++) {
            cp.second = i;
            uollano.emplace_back(clause_to_atomization_map.at(cp));
        }
        for (size_t i = 0, N = uollano.size(); i<N; i++) {
            auto simi = uollano.at(i);
            for (size_t j = 0; j<i; j++) {
                os << simi << " " << uollano.at(j) << std::endl;
                os << uollano.at(j) << " " << simi << std::endl;
            }
        }
    }
}

std::vector<std::vector<std::string>> input_pipeline::toCanonicalTraces(
        const std::vector<std::vector<std::pair<std::string, std::unordered_map<std::string, std::variant<std::string, double>>>>> &data_log,
        std::unordered_set<std::string> &SigmaAll) {
    std::vector<std::vector<std::string>> formaggino;
    SigmaAll.insert(atom_universe.begin(), atom_universe.end());
    for (const std::vector<std::pair<std::string, std::unordered_map<std::string, std::variant<std::string, double>>>>& trace : data_log) {
        std::vector<std::string> fantasma;
        for (const std::pair<std::string, std::unordered_map<std::string, std::variant<std::string, double>>>& event : trace) {
            const std::string& event_label = event.first;
            if ((!act_universe.contains(event_label)) || (act_atoms.contains(event_label))) {
                fantasma.emplace_back(event_label);
                SigmaAll.insert(event_label);
            } else {
                for (const auto& key_value : event.second) {
                    semantic_atom_set S;
                    bool first = true;
                    if (std::holds_alternative<std::string>(key_value.second)) {
                        auto& ref = string_map.at(event_label).at(key_value.first);
                        const std::string V = std::get<0>(key_value.second);
                        for (const auto& I : ref.collect_intervals()) {
                            DataPredicate dp{event_label, key_value.first, I.first, I.second};
                            if (dp.test(V)) {
                                assert(Mcal.contains(dp));
                                auto v = Mcal.at(dp);
                                if (first) {
                                    S.insert(v.begin(), v.end());
                                    first = false;
                                } else {
                                    semantic_atom_set S2;
                                    S2.insert(v.begin(), v.end());
                                    S = unordered_intersection(S, S2);
                                }
                            }
                        }
                    } else {
                        const double V = std::get<1>(key_value.second);
                        auto& ref = double_map.at(event_label).at(key_value.first);
                        for (const auto& I : ref.collect_intervals()) {
                            DataPredicate dp{event_label, key_value.first, I.first, I.second};
                            if (dp.test(V)) {
                                assert(Mcal.contains(dp));
                                auto v = Mcal.at(dp);
                                if (first) {
                                    S.insert(v.begin(), v.end());
                                    first = false;
                                } else {
                                    semantic_atom_set S2;
                                    S2.insert(v.begin(), v.end());
                                    S = unordered_intersection(S, S2);
                                }
                            }
                        }
                    }
                    assert(S.size() == 1);
                    fantasma.emplace_back(*S.begin());
                }
            }
        }
        formaggino.emplace_back(fantasma);
    }
    return formaggino;
}

#include <declare/DataTraceParse.h>

std::vector<std::vector<std::string>>
input_pipeline::convert_trace_labels(const std::string &file, std::unordered_set<std::string> &SigmaAll,
                                     bool serialize_original_log_to_xes) {
    DataTraceParse dtp;
    std::ifstream f{file};
    const auto data_log = dtp.load(f);
    if (serialize_original_log_to_xes) {
        serialize_data_log(data_log, file+".xes");
    }
    return toCanonicalTraces(data_log,  SigmaAll);
}

void input_pipeline::print_atomized_traces(const std::string &input_file, const std::string &file_text_and_xes,
                                           std::unordered_set<std::string> &SigmaAll, bool serialize_original_to_xes) {
    const auto log = convert_trace_labels(input_file, SigmaAll, serialize_original_to_xes);
    {
        serialize_non_data_log(log, file_text_and_xes+".xes");
    }
    {
        std::ofstream f{file_text_and_xes+".txt"};
        for (const std::vector<std::string>& atom_trace : log) {
            for (size_t i = 0, N = atom_trace.size(); i<N; i++) {
                f << atom_trace.at(i);
                if (i!= (N-1))
                    f << " ";
                else
                    f << std::endl;
            }
        }
    }

}

#include <graphs/FlexibleFA.h>
#include <ltlf/westergraad_semantics/FormulaToWestergaardFA.h>
#include <graphs/algorithms/shiftLabelsToNodes.h>
#include <graphs/algorithms/minimizeDFA.h>

std::string ignore3(const std::string& x, const std::string& y) {
    return x+y;
}

#include <filesystem>
#include <graphs/third-party-wrappers/ParseFFLOATDot.h>

namespace fs = std::filesystem;

FlexibleFA<size_t, std::string>
input_pipeline::decompose_ltlf_for_tiny_graphs(const ltlf &formula, std::unordered_set<std::string> &SigmaAll,
                                               const std::string &single_line_clause_file) {
#if 1
    std::vector<std::string> SigmaVector;
    SigmaVector.insert(SigmaVector.end(), SigmaAll.begin(), SigmaAll.end());
    std::vector<FlexibleFA<size_t, std::string>> GraphVector;
    size_t N_graphs = 0;
    {
        std::vector<ltlf> formulas_to_dfas;
        if (formula.casusu == AND) {
            auto falsehood = ltlf::True().negate().simplify();
            auto truth = ltlf::True();
            std::unordered_set<ltlf> set;
            formula.collectStructuralElements(AND, set, true);
            if (set.contains(truth))
                set.erase(truth);
            if (!set.contains(falsehood)) {
                bool doNotInsert = false;
                for (const auto &arg : set) {
                    if (set.contains(arg.negate().simplify())) {
                        doNotInsert = true;
                        break;
                    }
                }
                if (doNotInsert) {
                    set.clear();
                }
                formulas_to_dfas.insert(formulas_to_dfas.end(), set.begin(), set.end());
            }
        } else {
            formulas_to_dfas.emplace_back(formula);
        }

        // Creating the actual file, to be parsed by the python script
        N_graphs = formulas_to_dfas.size();

        if (N_graphs > 0) {
            std::ofstream file{single_line_clause_file};
            for (auto it = formulas_to_dfas.begin(); it != formulas_to_dfas.end(); it++) {
                file << *it << std::endl;
                file.flush();
            }
            file.close();
        }
    }


    if (N_graphs > 0) {

        // Parsing the file in Python, and then generating the sub-elements
        fs::path slcf_path = single_line_clause_file;
        pyscript.process_expression(fs::absolute(slcf_path).string());

        // Getting the graphs from FLLOAT
        for (size_t i = 1; i<=N_graphs; i++) {
            ParseFFLOATDot graph_loader;
            std::ifstream graph_operand_file{single_line_clause_file + "_graph_" + std::to_string(i) +".dot"};
            GraphVector.emplace_back(graph_loader.parse(graph_operand_file, SigmaAll));
        }
    }

#if 0
    if (GraphVector.empty()) {
        return {};
    } else if (GraphVector.size() == 1) {
        FlexibleFA<size_t, std::string> G;
        {
            FlexibleFA<size_t, std::string> l = GraphVector.at(0).shiftLabelsToEdges();
            minimizeDFA<size_t, std::string>(l).ignoreNodeLabels(G);
        }
        return G;
    } else {
        FlexibleFA<std::string, size_t> curr;
        {
            FlexibleFA<size_t, std::string> g = FlexibleFA<std::string, size_t>::crossProductWithNodeLabels(GraphVector.at(0), GraphVector.at(1)).shiftLabelsToEdges().makeDFAAsInTheory();
            FlexibleFA<size_t, std::string> G;
            {
                FlexibleFA<std::vector<size_t>, std::string> gr = minimizeDFA<size_t, std::string>(g);
                gr.ignoreNodeLabels(G);
            }
            curr = G.shiftLabelsToNodes();
        }
        /*for (size_t i = 2, N = GraphVector.size(); i<N; i++) {
            FlexibleFA<size_t, std::string> g = FlexibleFA<std::string, std::string>::crossProductWithNodeLabels(curr, GraphVector.at(i)).shiftLabelsToEdges().makeDFAAsInTheory();
            FlexibleFA<size_t, std::string> G;
            {
                FlexibleFA<std::vector<size_t>, std::string> gr = minimizeDFA<size_t, std::string>(g);
                gr.ignoreNodeLabels(G);
            }
            curr = G.shiftLabelsToNodes();
        }*/
        auto l = curr.shiftLabelsToEdges();
        FlexibleFA<size_t, std::string> G;
        minimizeDFA<size_t, std::string>(l).ignoreNodeLabels(G);
        return G;
    }
#else
    return {};
#endif
#else
    return {};
#endif
}
