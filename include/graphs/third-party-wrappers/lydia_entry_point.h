/*
 * lydia_entry_point.h
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
// Created by giacomo on 01/03/21.
//

#ifndef CLASSIFIERS_LYDIA_ENTRY_POINT_H
#define CLASSIFIERS_LYDIA_ENTRY_POINT_H

#include <iostream>
#include <istream>
#include <lydia/dfa/mona_dfa.hpp>
#include <lydia/to_dfa/core.hpp>
#include <lydia/to_dfa/strategies/compositional/base.hpp>
#include <lydia/utils/print.hpp>
#include <filesystem>
#include <ltlf/ltlf.h>

#include <lydia/parser/ltlf/driver.hpp>
#include <graphs/FlexibleFA.h>
#include <unordered_map>

/**
 * An utility entry point for better accessing Lydia
 */
class lydia_entry_point {
    whitemech::lydia::CompositionalStrategy dfa_strategy;
    whitemech::lydia::Translator translator;
    whitemech::lydia::parsers::ltlf::LTLfDriver* driver;

public:
    /**
     * Default system initialisation
     */
    lydia_entry_point();

    /**
     * Generates an ldlf expression from a \n-ending file containing an LTLf expression
     * @param filename
     * @return
     */
    whitemech::lydia::ldlf_ptr parse_formula_from_ltlf_file(const std::string& filename);

    FlexibleFA<size_t, std::string> generate_graph(const std::unordered_set<std::string> &SigmaAll, const ltlf& element) {
        return print_map(SigmaAll, generate_map(convert_formula_from_objects(element), "lydia.pdf"));
    }

    whitemech::lydia::ldlf_ptr convert_formula_from_objects(const ltlf& element);

    std::vector<std::pair<std::pair<int,int>, std::vector<std::vector<whitemech::lydia::atom>>>> generate_map(whitemech::lydia::ldlf_ptr parsed_formula, const std::string& file_pdf) {
        auto my_dfa = translator.to_dfa(*parsed_formula);
        auto my_mona_dfa =
                std::dynamic_pointer_cast<whitemech::lydia::mona_dfa>(my_dfa);
        auto num = my_mona_dfa->get_nb_variables();
        std::vector<unsigned> x(num);
        return whitemech::lydia::print_mona_dfa(my_mona_dfa->get_dfa(), file_pdf,
                                                my_mona_dfa->get_nb_variables(), my_mona_dfa->names);
    }

    static FlexibleFA<size_t, std::string> print_map(/*std::ostream& o, */
                          const std::unordered_set<std::string> &SigmaAll,
                          const std::vector<std::pair<std::pair<int,int>, std::vector<std::vector<whitemech::lydia::atom>>>>& map) {
        //static const std::string wedge{" ∧ "};
        FlexibleFA<size_t, std::string> result;
        int min_id = std::numeric_limits<int>::max();
        int max_id = -std::numeric_limits<int>::max();
        int n_vertices = 0;
        for (const auto& cp : map) {
            const std::pair<int, int>& edge = cp.first;
            min_id = std::min(min_id, edge.first);
            max_id = std::min(max_id, edge.first);
            min_id = std::min(min_id, edge.second);
            max_id = std::min(max_id, edge.second);
        }
        n_vertices = max_id - min_id + 1;
        for (int i = 0; i<n_vertices; i++) {
            size_t id = result.addNewNodeWithLabel(min_id+i);
            assert (id == i);
        }
        for (const auto& cp : map) {
            const std::pair<int, int>& edge = cp.first;
            const std::vector<std::vector<whitemech::lydia::atom>>& clause = cp.second;

            //std::cout << edge.first << "-->" << edge.second << std::endl;
            ltlf dist_formula = ltlf::True();
            for (size_t j = 0, M = clause.size(); j<M; j++) {
                const std::vector<whitemech::lydia::atom>& disj = clause.at(j);
                ltlf formula = ltlf::True();
                //std::cout << "\t\t";
                for (size_t i = 0, N = disj.size(); i<N; i++) {
                    ltlf atom = ltlf::Act(disj.at(i).name);
                    if (disj.at(i).is_negated)
                        atom = atom.negate();
                    if (i == 0) {
                        formula = atom;
                    } else {
                        formula = ltlf::And(atom, formula);
                    }
                    /*std::cout << (disj.at(i).is_negated ? "~" : "") << disj.at(i).name;
                    if (i != (N-1)) {
                        std::cout << wedge;
                    } else {
                        std::cout << std::endl;
                    }*/
                }
                if (j == 0) {
                    dist_formula = formula;
                } else {
                    dist_formula = ltlf::Or(dist_formula, formula);
                }
            }

            for (const std::string& act : SigmaAll) {
                if (dist_formula.easy_interpret(act)) {
                    //   std::cerr << '\t' << act << std::endl;
                    result.addNewEdgeFromId(edge.first - min_id, edge.second - min_id, act);
                }
            }
        }

        return result;
    }


    ~lydia_entry_point() {
        delete driver;
    }

};



#endif //CLASSIFIERS_LYDIA_ENTRY_POINT_H
