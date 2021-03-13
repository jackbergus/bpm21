/*
 * SimpleXESSerializer.h
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
// Created by giacomo on 13/03/21.
//

#ifndef BPM21_SIMPLEXESSERIALIZER_H
#define BPM21_SIMPLEXESSERIALIZER_H

#include <string>
#include <iomanip>
#include <chrono>
#include <iostream>
#include <date/date.h>

namespace cr = std::chrono;

#include <vector>
#include <string>

void serialize_non_data_log(const std::vector<std::vector<std::string>>& simple_log, const std::string& xes_file);


#endif //BPM21_SIMPLEXESSERIALIZER_H
