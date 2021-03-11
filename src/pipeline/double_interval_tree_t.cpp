/*
 * double_interval_tree_t.cpp
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

#include "pipeline/double_interval_tree_t.h"

double_interval_tree_t::double_interval_tree_t() : segment_partition_tree{DataPredicate::MIN_DOUBLE, DataPredicate::MAX_DOUBLE} {}

std::vector<std::pair<double, double>> double_interval_tree_t::findInterval(double left, double right) {
    return find_interval(indexer, element, left, right);
}

void double_interval_tree_t::perform_insertion() {
    bulk_insertion.emplace_back(DataPredicate::MIN_DOUBLE, DataPredicate::MAX_DOUBLE);
    std::sort(bulk_insertion.begin(), bulk_insertion.end(), interval_comparator_inverse<double, DoublePrevNext>());
    // Inserting one interval at a time
    bulk_insertion.erase( unique( bulk_insertion.begin(), bulk_insertion.end() ), bulk_insertion.end() );
    for (const auto& i : bulk_insertion) {
        /*std::cout << i << " = "<< */ insert_interval(this->indexer, this->element, i.first, i.second) /*<< std::endl*/;
        ///std::cout << tree << std::endl;
        ///std::cout << std::endl;
    }
    minimize_tree<double, DoublePrevNext>(indexer, element);
    /*for (const auto& i : bulk_insertion) {
        std::cout << i << " = " << find_interval(indexer, element, i.first, i.second) << std::endl;
    }*/
    //std::cout << *this << std::endl;
}
