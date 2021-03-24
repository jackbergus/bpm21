/*
 * lydia_entry_point.cpp
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

#include "graphs/third-party-wrappers/lydia_entry_point.h"
#include <lydia/parser/ltlf/driver.cpp>
#include <graphs/algorithms/minimizeDFA.h>

lydia_entry_point::lydia_entry_point() : dfa_strategy{}, translator{dfa_strategy}, driver{new whitemech::lydia::parsers::ltlf::LTLfDriver()} {

}

whitemech::lydia::ldlf_ptr lydia_entry_point::parse_formula_from_ltlf_file(const std::string &filename) {
    std::filesystem::path formula_path(filename);
    driver->parse(formula_path.string().c_str());
    return driver->result;
}

#ifdef TRUE
#undef TRUE
#endif
#ifdef FALSE
#undef FALSE
#endif

whitemech::lydia::ldlf_ptr lydia_entry_point::convert_formula_from_objects(const ltlf &formula) {
    switch (formula.casusu) {
        case ACT: {
            auto f = driver->add_LTLfAtom(formula.act);
            if (formula.is_negated) {
                auto neg = driver->add_LTLfNot(f);
                return neg;
            } else {
                return f;
            }
        }

        case NEG_OF: {
            auto acc = convert_formula_from_objects(formula.args.at(0));
            auto neg = driver->add_LTLfNot(acc);
            return neg;
        }

        case OR: {
            auto acc1 = convert_formula_from_objects(formula.args.at(0));
            auto acc2 = convert_formula_from_objects(formula.args.at(1));
            auto or_ = driver->add_LTLfOr(acc1, acc2);
            return or_;
        }

        case AND:{
            auto acc1 = convert_formula_from_objects(formula.args.at(0));
            auto acc2 = convert_formula_from_objects(formula.args.at(1));
            auto or_ = driver->add_LTLfAnd(acc1, acc2);
            return or_;
        }

        case NEXT:{
            auto acc = convert_formula_from_objects(formula.args.at(0));
            auto neg = driver->add_LTLfNext(acc);
            return neg;
        }

        case UNTIL: {
            auto acc1 = convert_formula_from_objects(formula.args.at(0));
            auto acc2 = convert_formula_from_objects(formula.args.at(1));
            auto u = driver->add_LTLfUntil(acc1, acc2);
            return u;
        }

        case RELEASE:{
            auto acc1 = convert_formula_from_objects(formula.args.at(0));
            auto acc2 = convert_formula_from_objects(formula.args.at(1));
            auto u = driver->add_LTLfRelease(acc1, acc2);
            return u;
        }

        case formula_t::TRUE:
            return driver->add_LTLfTrue();
        default:
            return driver->add_LTLfFalse();

    }
}

std::vector<std::pair<std::pair<int, int>, std::vector<std::vector<whitemech::lydia::atom>>>>
lydia_entry_point::generate_map(whitemech::lydia::ldlf_ptr parsed_formula, const std::string &file_pdf,
                                std::vector<size_t> &begins, std::vector<size_t> &ends) {
    auto my_dfa = translator.to_dfa(*parsed_formula);
    auto my_mona_dfa =
            std::dynamic_pointer_cast<whitemech::lydia::mona_dfa>(my_dfa);
    auto num = my_mona_dfa->get_nb_variables();
    std::vector<unsigned> x(num);
    return whitemech::lydia::print_mona_dfa(my_mona_dfa->get_dfa(), file_pdf,
                                            ends,
                                            begins,
                                            my_mona_dfa->get_nb_variables(),
                                            my_mona_dfa->names);
}

FlexibleFA<size_t, std::string> lydia_entry_point::print_map(const std::unordered_set<std::string> &SigmaAll,
                                                             std::vector<size_t> &begins, std::vector<size_t> &ends,
                                                             const std::vector<std::pair<std::pair<int, int>, std::vector<std::vector<whitemech::lydia::atom>>>> &map) {
    //static const std::string wedge{" ∧ "};
    FlexibleFA<size_t, std::string> result;
    std::unordered_set<size_t> F{ends.begin(), ends.end()}, I{begins.begin(), begins.end()};
    int min_id = std::numeric_limits<int>::max();
    int max_id = -std::numeric_limits<int>::max();
    int n_vertices = 0;
    for (const auto& cp : map) {
        const std::pair<int, int>& edge = cp.first;
        min_id = std::min(min_id, edge.first);
        max_id = std::max(max_id, edge.first);
        min_id = std::min(min_id, edge.second);
        max_id = std::max(max_id, edge.second);
    }
    n_vertices = max_id - min_id + 1;
    for (int i = 0; i<n_vertices; i++) {
        size_t id = result.addNewNodeWithLabel(min_id+i);
        if (I.contains(min_id+i))
            result.addToInitialNodesFromId(id);
        if (F.contains(min_id+i))
            result.addToFinalNodesFromId(id);
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

    FlexibleFA<size_t, std::string> result2;
    minimizeDFA<size_t, std::string>(result).ignoreNodeLabels2(result2);
    return result2;
}
