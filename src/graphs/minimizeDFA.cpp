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

void marcaRec2(std::pair<size_t, size_t> cp, size_t max_graph_id,
              std::vector<std::variant<std::unordered_set<std::pair<size_t, size_t>>, bool>> &M) {
    if (cp.second < cp.first)
        std::swap(cp.first, cp.second);
    size_t id = cp.first * max_graph_id + cp.second;
    auto& it = M.at(id);
    std::unordered_set<std::pair<size_t,size_t>> ls;
    if (std::holds_alternative<std::unordered_set<std::pair<size_t,size_t>>>(it)) {
        ls = std::get<std::unordered_set<std::pair<size_t,size_t>>>(it);
    }
    //std::cout << "\t\t<" << graph.getNodeLabel(it->first.first) <<',' << graph.getNodeLabel(it->first.second) << "> marked!" << std::endl;
    M[id] = {false};
    for (std::pair<size_t,size_t> x : ls) {
        if (x.second < x.first)
            std::swap(x.first, x.second);
        marcaRec2(x,max_graph_id, M);
    }
}
