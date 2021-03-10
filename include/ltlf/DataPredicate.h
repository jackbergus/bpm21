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
#include <utils/numeric/hash_combine.h>

enum numeric_atom_cases {
    LT,
    GT,
    LEQ,
    GEQ,
    EQ,
    NEQ
};

struct DataPredicate {
    std::string                       var;
    numeric_atom_cases                casusu;
    std::variant<std::string, double> value;

    DataPredicate();;
    DataPredicate(const std::string &var, numeric_atom_cases casusu, const std::variant<std::string, double> &value);
    DataPredicate(const std::string &var, numeric_atom_cases casusu, const std::string &value);
    DataPredicate(const std::string &var, numeric_atom_cases casusu, const double &value);
    DataPredicate(const DataPredicate& ) = default;
    DataPredicate(DataPredicate&& ) = default;
    DataPredicate& operator=(const DataPredicate&) = default;
    DataPredicate& operator=(DataPredicate&&) = default;
    DataPredicate negate() const;

    friend std::ostream &operator<<(std::ostream &os, const DataPredicate &predicate);
    bool operator==(const DataPredicate &rhs) const;
    bool operator!=(const DataPredicate &rhs) const;
};

namespace std {
    template <>
    struct hash<DataPredicate>
    {
        std::size_t operator()(const DataPredicate& k) const {
            return hash_combine(hash_combine(hash_combine(31, k.var), k.casusu), k.value);
        }
    };

}

#endif //BPM21_DATAPREDICATE_H
