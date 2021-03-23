/*
 * pipeline_utils.h
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

#ifndef BPM21_PIPELINE_UTILS_H
#define BPM21_PIPELINE_UTILS_H

#include <structures/query_interval_set.h>
#include <utils/structures/set_operations.h>
#include <utils/numeric/hash_combine.h>
#include <utils/numeric/pair_hash.h>
#include <utils/numeric/uset_hash.h>
#include <utils/numeric/vector_hash.h>
#include <ltlf/ltlf.h>

using label_var_atoms_map_t = std::unordered_map<std::string, std::unordered_map<std::string, std::unordered_set<DataPredicate>>>;
using label_set_t = std::unordered_set<std::string>;

#include <pipeline/double_interval_tree_t.h>
#include <pipeline/string_interval_tree_t.h>

using double_intervals_map_t = std::unordered_map<std::string, std::unordered_map<std::string, double_interval_tree_t>>;
using string_intervals_map_t = std::unordered_map<std::string, std::unordered_map<std::string, string_interval_tree_t>>;

void pipeline_scratch(const ltlf& formula,
                      label_var_atoms_map_t& map,
                      label_set_t& S,
                      double_intervals_map_t& double_intervals,
                      string_intervals_map_t& string_intervals);

#endif //BPM21_PIPELINE_UTILS_H
