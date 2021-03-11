/*
 * segment_partition_tree.h
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
// Created by giacomo on 03/01/21.
//

#ifndef CUCCIOLO_SEGMENT_PARTITION_TREE_H
#define CUCCIOLO_SEGMENT_PARTITION_TREE_H

#include <structures/query_interval_set/structures/node_recur.h>

template <typename T, typename PrevNext> struct segment_partition_tree {
    PrevNext     indexer;
    struct node_recur<T> element;

    segment_partition_tree(T min, T max) : element(min, max) {}

    friend std::ostream &operator<<(std::ostream &os, const segment_partition_tree &tree) {
        os << " min: " << tree.element.min << " max: " << tree.element.max << std::endl;
        for (size_t i = 0, N = tree.element.children.size(); i<N; i++)
            tree.element.children.at(i).print(os, 2);
        return os;
    }
};

#endif //CUCCIOLO_SEGMENT_PARTITION_TREE_H
