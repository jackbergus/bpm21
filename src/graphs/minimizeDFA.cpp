/*
 * minimizeDFA.cpp
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
// Created by giacomo on 28/02/21.
//

#include <graphs/algorithms/minimizeDFA.h>

void marcaRec(std::pair<size_t, size_t> cp,
              std::unordered_map<std::pair<size_t, size_t>, std::variant<std::unordered_set<std::pair<size_t, size_t>>, bool>> &M) {
    if (cp.second < cp.first)
        std::swap(cp.first, cp.second);
    auto it = M.find(cp);
    std::unordered_set<std::pair<size_t,size_t>> ls;
    if (std::holds_alternative<std::unordered_set<std::pair<size_t,size_t>>>(it->second)) {
        ls = std::get<std::unordered_set<std::pair<size_t,size_t>>>(it->second);
    }
    //std::cout << "\t\t<" << graph.getNodeLabel(it->first.first) <<',' << graph.getNodeLabel(it->first.second) << "> marked!" << std::endl;
    it->second = {false};
    for (std::pair<size_t,size_t> x : ls) {
        if (x.second < x.first)
            std::swap(x.first, x.second);
        marcaRec(x, M);
    }
}

void marcaRec3(size_t M_offset, std::vector<table_content> &M) {
    auto& ref = M[M_offset];
    if (!ref.is_bool) {
        std::vector<size_t> ls;
        std::swap(ls, ref.S);
        std::sort( ls.begin(), ls.end() );
        ls.erase( std::unique( ls.begin(), ls.end() ), ls.end() );
        ref = {false, ref.left, ref.right};
        for (size_t offset2 : ls) {
            marcaRec3(offset2, M);
        }
    }

}

/*
void marcaRec2(size_t cp, size_t max_V, std::vector<table_content> &M) {
    size_t cp_second = DOVETAIL_RIGHT(cp, max_V);
    size_t cp_first = DOVETAIL_LEFT(cp, max_V);
    if (cp_second < cp_first)
        std::swap(cp_first, cp_second);
    auto it = &M[cp];
    //std::cout << "\t\t{" << cp_first <<';' << cp_second << "} marked!" << std::endl;
    *it = {false, std::make_pair(cp_first, cp_second)};
    if (it->is_set) for (size_t x : it->S) {
        size_t x_second = DOVETAIL_RIGHT(x, max_V);
        size_t x_first = DOVETAIL_LEFT(x, max_V);
        if (x_second < x_first)
            std::swap(x_first, x_second);
        marcaRec2(DOVETAIL(x_first, x_second, max_V), max_V, M);
    }
}
*/