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

    static void print_map(std::ostream& o, const std::vector<std::pair<std::pair<int,int>, std::vector<std::vector<whitemech::lydia::atom>>>>& map) {
        static const std::string wedge{" ∧ "};
        for (const auto& cp : map) {
            const std::pair<int, int>& edge = cp.first;
            const std::vector<std::vector<whitemech::lydia::atom>>& clause = cp.second;
            std::cout << edge.first << "-->" << edge.second << std::endl;
            for (const std::vector<whitemech::lydia::atom>& disj : clause) {
                std::cout << "\t\t";
                for (size_t i = 0, N = disj.size(); i<N; i++) {
                    std::cout << (disj.at(i).is_negated ? "~" : "") << disj.at(i).name;
                    if (i != (N-1)) {
                        std::cout << wedge;
                    } else {
                        std::cout << std::endl;
                    }
                }
            }
        }
    }


    ~lydia_entry_point() {
        delete driver;
    }

};



#endif //CLASSIFIERS_LYDIA_ENTRY_POINT_H
