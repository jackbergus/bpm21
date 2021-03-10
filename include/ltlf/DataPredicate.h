/*
 * DataPredicate.h
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
// Created by giacomo on 10/03/21.
//

#ifndef BPM21_DATAPREDICATE_H
#define BPM21_DATAPREDICATE_H

#include <string>
#include <variant>
#include <ostream>

namespace std {
    template <>
    struct hash<std::variant<std::string, double>>
    {
        std::size_t operator()(const std::variant<std::string, double>& k) const {
            return std::holds_alternative<double>(k) ?
                   (13*std::hash<double>{}(std::get<double>(k))) :
                   (31*std::hash<std::string>{}(std::get<std::string>(k)));
        }
    };

}

#include <utils/numeric/hash_combine.h>
#include <utils/numeric/uset_hash.h>
#include <cmath>
#include <cfloat>

enum numeric_atom_cases {
    LT,
    GT,
    LEQ,
    GEQ,
    EQ,
    NEQ,
    INTERVAL
};

std::string prev_char(const std::string& val, size_t max_size);
std::string next_char(const std::string& val, size_t max_size);

#define MAXIMUM_STRING_LENGTH       (1000)


struct DataPredicate {
    std::string                       var;
    numeric_atom_cases                casusu;
    std::variant<std::string, double> value;
    std::variant<std::string, double> value_upper_bound;
    std::unordered_set<std::variant<std::string, double>> exceptions;

    DataPredicate();
    DataPredicate(const std::string &var, numeric_atom_cases casusu, const std::variant<std::string, double> &value);
    DataPredicate(const std::string &var, numeric_atom_cases casusu, const std::string &value);
    DataPredicate(const std::string &var, numeric_atom_cases casusu, const double &value);
    DataPredicate(const DataPredicate& ) = default;
    DataPredicate(DataPredicate&& ) = default;
    DataPredicate& operator=(const DataPredicate&) = default;
    DataPredicate& operator=(DataPredicate&&) = default;
    DataPredicate negate() const;

    friend std::ostream &operator<<(std::ostream &os, const DataPredicate &predicate);
    void asInterval();
    void intersect_with(const DataPredicate& predicate);
    bool test(const std::string& val) const;
    bool test(double val) const;
};

namespace std {
    template <>
    struct hash<DataPredicate>
    {
        std::size_t operator()(const DataPredicate& k) const {
            size_t seed = 31;
            seed = hash_combine(seed, k.value);
            seed = hash_combine(seed, k.casusu);
            seed = hash_combine(seed, k.var);
            if (k.casusu == INTERVAL) {
                seed = hash_combine(seed, k.value_upper_bound);
                return hash_combine(seed, k.exceptions);
            }
            return seed;
        }
    };

}

#endif //BPM21_DATAPREDICATE_H
