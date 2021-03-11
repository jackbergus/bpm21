/*
 * pipeline_utils.cpp
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

#include <pipeline/pipeline_utils.h>

void pipeline_scratch(const ltlf &formula, label_var_atoms_map_t &map, label_set_t &S,
                      double_intervals_map_t &double_intervals, string_intervals_map_t &string_intervals) {
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
