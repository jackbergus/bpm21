/*
 * double_interval_tree_t.h
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

#ifndef BPM21_DOUBLE_INTERVAL_TREE_T_H
#define BPM21_DOUBLE_INTERVAL_TREE_T_H


#include <pipeline/DoublePrevNext.h>
#include <structures/query_interval_set.h>

struct double_interval_tree_t : public segment_partition_tree<double, DoublePrevNext> {
    ~double_interval_tree_t() {}
    double_interval_tree_t();

    std::vector<std::pair<double, double>> bulk_insertion;

    std::vector<std::pair<double, double>> findInterval(double left, double right);
    void perform_insertion();
};



#endif //BPM21_DOUBLE_INTERVAL_TREE_T_H
