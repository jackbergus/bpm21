/*
 * foreign_data_loads.h
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
// Created by giacomo on 15/03/21.
//

#ifndef BPM21_FOREIGN_DATA_LOADS_H
#define BPM21_FOREIGN_DATA_LOADS_H

#include <variant>
#include <string>
#include <vector>
#include <unordered_map>
#include <utils/xml_utils.h>

std::vector<std::vector<std::string>> load_xes_ignore_data(const std::string &filename, bool conservative_for_pddl = true);
std::vector<std::vector<std::pair<std::string, std::unordered_map<std::string, std::variant<std::string, double>>>>> load_xes_with_data(const std::string &filename, bool conservative_for_pddl = true);

#endif //BPM21_FOREIGN_DATA_LOADS_H
