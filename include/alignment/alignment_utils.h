/*
 * alignment_utils.cpp
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
// Created by giacomo on 24/03/21.
//

#ifndef BPM21_ALIGNMENT_UTILS_H
#define BPM21_ALIGNMENT_UTILS_H

#include <string>
#include <graphs/FlexibleFA.h>

/**
 * Approximate graph alignment, assuming that the node labels represent just one character
 * @param x             Graph
 * @param path          Path on top of which we want to perform the alignment
 * @param epsilon       Epsilon character (.)
 * @return
 */
double align(const FlexibleFA<std::string, size_t>& x, const std::string& path, const std::string& epsilon);

#endif //BPM21_ALIGNMENT_UTILS_H
