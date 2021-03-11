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
        for (size_t i = 0, N = k.second.size(); i<N; i++) {
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
    init_pipeline(file);
    if ((!double_map.empty()) || (!(string_map.empty()))) {
        decompose_and_atomize();
        ltlf atomized_model = setInterpretCompoundSubatom(model);
        std::cout << "Atomized = " << atomized_model << std::endl;
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
        for (size_t i = 0, N = it->second.size(); i<N; i++) {
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
            /*assert(string_map.contains(pred.label));
            assert(string_map.at(pred.label).contains(pred.var));*/
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
            /*assert(double_map.contains(pred.label));
            assert(double_map.at(pred.label).contains(pred.var));*/
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
        return extractLtlfFormulaFromSubAtoms(formula).setBeingCompound(true);
    } else {
        switch (formula.casusu) {
            case ACT:
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
            std::cout << "DEBUG = " << formula.numeric_atom << " ~= " << atom << std::endl;
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
