/*
 * string_interval_tree_t.cpp
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

#include "pipeline/string_interval_tree_t.h"

string_interval_tree_t::string_interval_tree_t() : segment_partition_tree{DataPredicate::MIN_STRING, DataPredicate::MAX_STRING} {}

std::vector<std::pair<std::string, std::string>>
string_interval_tree_t::findInterval(const std::string &left, const std::string &right) {
    return find_interval(indexer, element, left, right);
}

void string_interval_tree_t::perform_insertion() {
    bulk_insertion.emplace_back(DataPredicate::MIN_STRING, DataPredicate::MAX_STRING);
    std::sort(bulk_insertion.begin(), bulk_insertion.end(), interval_comparator_inverse<std::string, StringPrevNext>());
    // Inserting one interval at a time

    bulk_insertion.erase( unique( bulk_insertion.begin(), bulk_insertion.end() ), bulk_insertion.end() );
    for (const auto& i : bulk_insertion) {
        /*std::cout << i << " = " <<*/ insert_interval(this->indexer, this->element, i.first, i.second) /*<< std::endl*/;
        ///std::cout << tree << std::endl;
        ///std::cout << std::endl;
    }
    //std::cout << *this << std::endl;
    minimize_tree<std::string, StringPrevNext>(indexer, element);
    /*for (const auto& i : bulk_insertion) {
        std::cout << i << " = " << find_interval(indexer, element, i.first, i.second) << std::endl;
    }*/
    //std::cout << *this << std::endl;
}
