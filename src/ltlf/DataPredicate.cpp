/*
 * DataPredicate.cpp
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

#include "ltlf/DataPredicate.h"

DataPredicate DataPredicate::negate() const {
    switch (casusu) {
        case LT:
            return {var, GEQ, value};
        case GT:
            return {var, LEQ, value};
        case LEQ:
            return {var, GT, value};
        case GEQ:
            return {var, LT, value};
        case EQ:
            return {var, NEQ, value};
        case NEQ:
            return {var, EQ, value};
    }
}

DataPredicate::DataPredicate() : var{"x"}, casusu{EQ}, value{0.0} {}

DataPredicate::DataPredicate(const std::string &var, numeric_atom_cases casusu, const std::variant<std::string, double> &value) : var(
        var), casusu(casusu), value(value) {}

DataPredicate::DataPredicate(const std::string &var, numeric_atom_cases casusu, const std::string &value) : var(var), casusu(casusu), value(value) {}

DataPredicate::DataPredicate(const std::string &var, numeric_atom_cases casusu, const double &value) : var(var), casusu(casusu), value(value) {}

std::ostream &operator<<(std::ostream &os, const DataPredicate &predicate) {
    os << predicate.var;
    switch (predicate.casusu) {
        case LT:
            os << " < ";
        case GT:
            os << " > ";
        case LEQ:
            os << " ≤ ";
        case GEQ:
            os << " ≥ ";
        case EQ:
            os << " = ";
        case NEQ:
            os << " ≠ ";
    }
    if (std::holds_alternative<std::string>(predicate.value))
        return os << std::get<std::string>(predicate.value);
    else
        return os << std::get<double>(predicate.value);
}

bool DataPredicate::operator==(const DataPredicate &rhs) const {
    return var == rhs.var &&
           casusu == rhs.casusu &&
           value == rhs.value;
}

bool DataPredicate::operator!=(const DataPredicate &rhs) const {
    return !(rhs == *this);
}
