
/*
 * vector_hash.h
 * This file is part of Classifiers
 *
 * Copyright (C) 2021 - Giacomo Bergami
 *
 * ProbabilisticTraceAlignment is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * ProbabilisticTraceAlignment is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ProbabilisticTraceAlignment. If not, see <http://www.gnu.org/licenses/>.
 */


//
// Created by giacomo on 14/11/20.
//
#ifndef CLASSIFIERS_PAIR_HASH_H
#define CLASSIFIERS_PAIR_HASH_H

#include <utils/numeric/hash_combine.h>
#include <unordered_map>

namespace std {
    template <typename T, typename K>
    struct hash<std::pair<T, K>>
    {
        std::size_t operator()(const std::pair<T, K>& k) const {
            size_t init = 31;
            init = hash_combine<T>(init, k.first);
            init = hash_combine<K>(init, k.second);
            return init;
        }
    };

}

namespace std {
    template <typename T, typename K>
    struct hash<std::unordered_map<T, K>>
    {
        std::size_t operator()(const std::unordered_map<T, K>& k) const {
            size_t init = 31;
            std::hash<std::pair<T, K>> hash_pair;
            for (const std::pair<T, K>& cp : k)
                init = hash_combine(init, cp);
            return init;
        }
    };

}

/*
template <typename T>
struct vector_hash {
    std::size_t operator()(const std::vector<T>& k) const
    {
        size_t init = 31;
        for (const auto& x : k) init = hash_combine<T>(init, x);
        return init;
    }
};*/

#endif //FUZZYSTRINGMATCHING2_VECTOR_HASH_H
