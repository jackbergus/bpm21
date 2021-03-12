/*
 * input_pipeline.h
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

#ifndef BPM21_INPUT_PIPELINE_H
#define BPM21_INPUT_PIPELINE_H

#include <pipeline/pipeline_utils.h>
#include <graphs/third-party-wrappers/lydia_entry_point.h>

struct input_pipeline {
    label_var_atoms_map_t map1;
    label_set_t           act_universe;
    label_set_t           act_atoms;
    double_intervals_map_t  double_map;
    string_intervals_map_t  string_map;
    ltlf model;

    lydia_entry_point     lydia_ep;

    using semantic_atom_set = std::unordered_set<std::string>;

    std::string fresh_atom_label;
    std::unordered_map<std::pair<std::string, size_t>, std::string> clause_to_atomization_map;
    std::unordered_map<std::string, std::vector<std::vector<DataPredicate>>> interval_map;
    std::unordered_map<DataPredicate, std::vector<std::string>> Mcal;
    semantic_atom_set atom_universe;

    input_pipeline(const std::string& fresh_atom_label);

    void run_pipeline(const std::string& file);
    ltlf setInterpretCompoundSubatom(const ltlf& formula);
    ltlf extractLtlfFormulaFromSubAtoms(const ltlf &formula);

    void print_sigma(std::ostream& os);

private:
    size_t count_fresh_atoms;

    std::string generate_fresh_atom();

    semantic_atom_set atom_decomposition(const std::string& act, bool isNegated = false);
    semantic_atom_set interval_decomposition(const DataPredicate& pred, bool isNegated = false);

    semantic_atom_set _setInterpretCompoundSubatom(const ltlf& formula);
    void decompose_and_atomize();
    void init_pipeline(const std::string& file);
};


#endif //BPM21_INPUT_PIPELINE_H
