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
using double_intervals_map_t = std::unordered_map<std::pair<std::string, std::string>, double_interval_tree_t>;
using string_intervals_map_t = std::unordered_map<std::pair<std::string, std::string>, string_interval_tree_t>;


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
                auto& D = string_intervals[std::make_pair(formula.numeric_atom.label,
                                                          formula.numeric_atom.var)].bulk_insertion;
                D.insert(D.end(), V.begin(), V.end());
            } else {
                auto  V = std::get<1>(formula.numeric_atom.decompose_into_intervals());
                auto& D = double_intervals[std::make_pair(formula.numeric_atom.label,
                                                          formula.numeric_atom.var)].bulk_insertion;
                D.insert(D.end(), V.begin(), V.end());
            }
            break;

        case TRUE:
        case FALSE:
            break;
    }

}

int main() {

#if 1
    DeclareModelParse mp;
    std::ifstream stream ("test_file.txt");
    std::cout << "Parsing the file, and putting it in NNF, and simplifying it!" << std::endl;
    ltlf formula = mp.load_model_to_semantics(stream).nnf();
    std::cout << formula << std::endl;

    exit(0);
    label_var_atoms_map_t map1;
    label_set_t           act_universe;
    double_intervals_map_t  double_map;
    string_intervals_map_t  string_map;
    std::cout << "Collecting the atoms from the formula" << std::endl;
    pipeline_scratch(formula, map1, act_universe, double_map, string_map);

    std::cout << "Generating the distinct intervals from the elements" << std::endl;
    for (auto& ref : double_map)
        ref.second.perform_insertion();
    for (auto& ref : string_map)
        ref.second.perform_insertion();
#else
    std::string prev = PREV_STRING("");
    std::string next = NEXT_STRING("");
    std::cout << prev << std::endl;
    std::cout << next << std::endl;
#endif

}
