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
#include <declare/DeclareModelParse.h>
#include <DADLexer.h>
#include <DADParser.h>
#include <vector>

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

#include <structures/query_interval_set.h>
#include <utils/structures/set_operations.h>

struct DoublePrevNext  {
    double getPrev(double elem) const {
        return PREV_DOUBLE(elem);
    }
    double getNext(double elem) const {
        return NEXT_DOUBLE(elem);
    }
    double distance(double l, double r) const {
        return std::abs(l-r);
    }
};

struct double_interval_tree_t : public segment_partition_tree<double, DoublePrevNext> {
    ~double_interval_tree_t() {}
    double_interval_tree_t() : segment_partition_tree{DataPredicate::MIN_DOUBLE, DataPredicate::MAX_DOUBLE} {}

    std::vector<std::pair<double, double>> bulk_insertion;

    std::vector<std::pair<double, double>> findInterval(double left, double right) {
        return find_interval(indexer, element, left, right);
    }

    void perform_insertion() {
        bulk_insertion.emplace_back(DataPredicate::MIN_DOUBLE, DataPredicate::MAX_DOUBLE);
        std::sort(bulk_insertion.begin(), bulk_insertion.end(), interval_comparator_inverse<double, DoublePrevNext>());
        // Inserting one interval at a time
        bulk_insertion.erase( unique( bulk_insertion.begin(), bulk_insertion.end() ), bulk_insertion.end() );
        for (const auto& i : bulk_insertion) {
            /*std::cout << i << " = "<< */ insert_interval(this->indexer, this->element, i.first, i.second) /*<< std::endl*/;
            ///std::cout << tree << std::endl;
            ///std::cout << std::endl;
        }
        minimize_tree<double, DoublePrevNext>(indexer, element);
        /*for (const auto& i : bulk_insertion) {
            std::cout << i << " = " << find_interval(indexer, element, i.first, i.second) << std::endl;
        }*/
        std::cout << *this << std::endl;
    }
};


struct StringPrevNext {
    std::string getPrev(const std::string& elem) const {
        return PREV_STRING(elem);
    }
    std::string getNext(const std::string& elem) const {
        return NEXT_STRING(elem);
    }
    size_t distance(const std::string& l, const std::string& r) const {
        return (l == r) ? 0 : 1;
    }
};
struct string_interval_tree_t : public segment_partition_tree<std::string, StringPrevNext> {
    ~string_interval_tree_t() {}
    string_interval_tree_t() : segment_partition_tree{DataPredicate::MIN_STRING, DataPredicate::MAX_STRING} {}

    std::vector<std::pair<std::string, std::string>> findInterval(const std::string& left, const std::string& right)  {
        return find_interval(indexer, element, left, right);
    }

    std::vector<std::pair<std::string, std::string>> bulk_insertion;
    void perform_insertion() {
        bulk_insertion.emplace_back(DataPredicate::MIN_STRING, DataPredicate::MAX_STRING);
        std::sort(bulk_insertion.begin(), bulk_insertion.end(), interval_comparator_inverse<std::string, StringPrevNext>());
        // Inserting one interval at a time

        bulk_insertion.erase( unique( bulk_insertion.begin(), bulk_insertion.end() ), bulk_insertion.end() );
        for (const auto& i : bulk_insertion) {
            /*std::cout << i << " = " <<*/ insert_interval(this->indexer, this->element, i.first, i.second) /*<< std::endl*/;
            ///std::cout << tree << std::endl;
            ///std::cout << std::endl;
        }
        minimize_tree<std::string, StringPrevNext>(indexer, element);
        /*for (const auto& i : bulk_insertion) {
            std::cout << i << " = " << find_interval(indexer, element, i.first, i.second) << std::endl;
        }*/
        std::cout << *this << std::endl;
    }
};

using label_var_atoms_map_t = std::unordered_map<std::string, std::unordered_map<std::string, std::unordered_set<DataPredicate>>>;
using label_set_t = std::unordered_set<std::string>;
using double_intervals_map_t = std::unordered_map<std::string, std::unordered_map<std::string, double_interval_tree_t>>;
using string_intervals_map_t = std::unordered_map<std::string, std::unordered_map<std::string, string_interval_tree_t>>;


void pipeline_scratch(const ltlf& formula,
                      label_var_atoms_map_t& map,
                      label_set_t& S,
                      double_intervals_map_t& double_intervals,
                      string_intervals_map_t& string_intervals) {
    switch (formula.casusu) {
        case ACT:
            S.insert(formula.act);
            break;

        case NEG_OF:
        case OR:
        case AND:
        case NEXT:
        case UNTIL:
        case RELEASE:
            for (const ltlf& subF : formula.args)
                pipeline_scratch(subF, map, S, double_intervals, string_intervals);
            break;

        case NUMERIC_ATOM:
            map[formula.numeric_atom.label][formula.numeric_atom.var].insert(formula.numeric_atom);
            if (formula.numeric_atom.isStringPredicate()) {
                auto  V = std::get<0>(formula.numeric_atom.decompose_into_intervals());
                auto& D = string_intervals[formula.numeric_atom.label]
                                          [formula.numeric_atom.var].bulk_insertion;
                D.insert(D.end(), V.begin(), V.end());
            } else {
                auto  V = std::get<1>(formula.numeric_atom.decompose_into_intervals());
                auto& D = double_intervals[formula.numeric_atom.label]
                                          [formula.numeric_atom.var].bulk_insertion;
                D.insert(D.end(), V.begin(), V.end());
            }
            break;

        case TRUE:
        case FALSE:
            break;
    }

}


struct input_pipeline {
    label_var_atoms_map_t map1;
    label_set_t           act_universe;
    label_set_t           act_atoms;
    double_intervals_map_t  double_map;
    string_intervals_map_t  string_map;

    using semantic_atom_set = std::unordered_set<std::string>;

    std::string fresh_atom_label;
    std::unordered_map<std::pair<std::string, size_t>, std::string> clause_to_atomization_map;
    std::unordered_map<std::string, std::vector<std::vector<DataPredicate>>> interval_map;
    std::unordered_map<DataPredicate, std::vector<std::string>> Mcal;
    semantic_atom_set atom_universe;

    void print_sigma(std::ostream& os) {
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

    input_pipeline(const std::string& fresh_atom_label) : fresh_atom_label{fresh_atom_label} {
        count_fresh_atoms = 1;
    }

    std::string generate_fresh_atom() {
        return fresh_atom_label+std::to_string(count_fresh_atoms++);
    }

    void run_pipeline(const std::string& file) {
        init_pipeline(file);
        if ((!double_map.empty()) || (!(string_map.empty()))) {
            decompose_and_atomize();
        }
    }

    semantic_atom_set atom_decomposition(const std::string& act, bool isNegated = false) {
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
        return isNegated ? unordered_difference(act_universe, S) : S;
    }

    semantic_atom_set interval_decomposition(const DataPredicate& pred, bool isNegated = false) {
        semantic_atom_set S;
        if (pred.isStringPredicate()) {
            for (const auto& cp : std::get<0>(pred.decompose_into_intervals())) {
                for (const auto& I : string_map.at(pred.label).at(pred.var).findInterval(cp.first, cp.second)) {
                    DataPredicate dp{pred.label, pred.var, I.first, I.second};
                    assert(Mcal.contains(dp));
                    auto v = Mcal.at(dp);
                    S.insert(v.begin(), v.end());
                }
            }
        } else {
            for (const auto& cp : std::get<1>(pred.decompose_into_intervals())) {
                for (const auto& I : double_map.at(pred.label).at(pred.var).findInterval(cp.first, cp.second)) {
                    DataPredicate dp{pred.label, pred.var, I.first, I.second};
                    assert(Mcal.contains(dp));
                    auto v = Mcal.at(dp);
                    S.insert(v.begin(), v.end());
                }
            }
        }
        return isNegated ? unordered_difference(act_universe, S) : S;
    }

    ltlf setInterpretCompoundSubatom(const ltlf& formula) {
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

    ltlf extractLtlfFormulaFromSubAtoms(const ltlf &formula) {
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

private:
    semantic_atom_set _setInterpretCompoundSubatom(const ltlf& formula) {
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
                assert(!formula.is_negated);
                return interval_decomposition(atom, formula.is_negated);
            }
        }
    }

    size_t count_fresh_atoms;

    void decompose_and_atomize() {
        std::cout << "Generating the distinct intervals from the elements" << std::endl;

        for (auto ref = string_map.begin(); ref != string_map.cend(); ){
            for (auto& ref2 : ref->second) {
                std::vector<DataPredicate> result;
                ref2.second.perform_insertion();
                for (const auto& I : ref2.second.collect_intervals()) {
                    result.emplace_back(ref->first, ref2.first, I.first, I.second);
                }
                interval_map[ref->first].emplace_back(result);
            }
            ref = string_map.erase(ref);
        }
        //std::cout << std::setprecision(50);
        for (auto ref = double_map.begin(); ref != double_map.cend(); ){
            for (auto& ref2 : ref->second) {
                std::vector<DataPredicate> result;
                ref2.second.perform_insertion();
                for (const auto& I : ref2.second.collect_intervals()) {
                    result.emplace_back(ref->first, ref2.first, I.first, I.second);
                }
                interval_map[ref->first].emplace_back(result);
            }
            ref = double_map.erase(ref);
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

    void init_pipeline(const std::string& file) {
        DeclareModelParse mp;
        std::ifstream stream (file);

        std::cout << "Parsing the file, and putting it in NNF, and simplifying it!" << std::endl;
        ltlf formula = mp.load_model_to_semantics(stream).nnf().simplify().reduce().oversimplify();
        std::cout << formula << std::endl;

        std::cout << "Collecting the atoms from the formula" << std::endl;
        pipeline_scratch(formula, map1, act_universe, double_map, string_map);

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

};

struct IntPrevNext  {
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

int main() {

#if 1
    input_pipeline Pip{"fa"};
    Pip.run_pipeline("test_file.txt");

    std::ofstream f{"sigma.txt"};
    Pip.print_sigma(f);
    f.close();

#else
    segment_partition_tree<size_t, IntPrevNext> S(0, 10);
    insert_interval(S.indexer, S.element, 0UL, 10UL);
    insert_interval(S.indexer, S.element, 0UL, 4UL);
    insert_interval(S.indexer, S.element, 5UL, 5UL);
    insert_interval(S.indexer, S.element, 6UL, 10UL);
    minimize_tree(S.indexer, S.element);
    std::cout << S << std::endl;
#endif

}
