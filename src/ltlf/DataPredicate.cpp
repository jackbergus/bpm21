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
    if (predicate.casusu == INTERVAL) {
        double isString = std::holds_alternative<std::string>(predicate.value);
        if (isString)
            os << std::get<std::string>(predicate.value);
        else
            os << std::get<double>(predicate.value);
        os << " ≤ " << predicate.var << " ≤ ";
        if (isString)
            os << std::get<std::string>(predicate.value_upper_bound);
        else
            os << std::get<double>(predicate.value_upper_bound);
        if (!predicate.exceptions.empty()) {
            os << "\\ {";
            size_t N = predicate.exceptions.size();
            size_t i = 0;
            for (const auto& x : predicate.exceptions) {
                if (isString)
                    os << std::get<std::string>(x);
                else
                    os << std::get<double>(x);
                if (i != (N-1)) {
                    os << ", ";
                    i++;
                }
            }
            os << "}";
        }
        return os;
    } else {
        os << predicate.var;
        switch (predicate.casusu) {
            case LT:
                os << " < "; break;
            case GT:
                os << " > "; break;
            case LEQ:
                os << " ≤ "; break;
            case GEQ:
                os << " ≥ "; break;
            case EQ:
                os << " = "; break;
            case NEQ:
                os << " ≠ "; break;
        }
        if (std::holds_alternative<std::string>(predicate.value))
            return os << std::get<std::string>(predicate.value);
        else
            return os << std::get<double>(predicate.value);
    }

}

std::string prev_char(const std::string &val, size_t max_size) {
    static const char MIN_CHAR = std::numeric_limits<char>::min();
    static const char MAX_CHAR = std::numeric_limits<char>::max();
    if (val.empty()) return val;

    std::string result = val;
    size_t idx = val.size()-1;
    char last_char = val.at(idx);
    if (last_char == MIN_CHAR) {
        result.pop_back();
    } else {
        result[val.size()-1] = last_char-1;
    }
    return result;
}

std::string next_char(const std::string &val, size_t max_size) {
    static const char MIN_CHAR = std::numeric_limits<char>::min();
    static const char MAX_CHAR = std::numeric_limits<char>::max();
    static const std::string MAX_STRING = std::string(MAX_CHAR, max_size);
    if (val == MAX_STRING) return val;

    std::string next = val;
    size_t idx = next.size();
    if (idx < max_size) {
        next += MIN_CHAR;
    } else {
        if (next.at(idx) != MAX_CHAR) {
            next[idx]++;
            return next;
        } else {
            idx--;
            while (!next.empty()) {
                if (next.at(idx) != MAX_CHAR) {
                    next[idx]++;
                    return next;
                } else {
                    next.pop_back();
                    idx--;
                }
            }
            if (next.empty())
                return MAX_STRING;
        }

    }
    return next;
}


#include <cassert>

void DataPredicate::intersect_with(const DataPredicate& predicate) {
    assert(var == predicate.var);
    if (casusu == predicate.casusu) {
        switch (casusu) {
            case LT:
            case LEQ:
                value = std::min(value, predicate.value);
                break;

            case GT:
            case GEQ:
                value = std::max(value, predicate.value);
                break;

            case EQ:
                // The intersection of equality should consider that the equivalence is among identical values
                assert(value == predicate.value);
                break;

            case NEQ:
                casusu = INTERVAL;
                exceptions.insert(value);
                exceptions.insert(predicate.value);
                break;

            case INTERVAL:
                value = std::min(value, predicate.value);
                value_upper_bound = std::max(value_upper_bound, predicate.value_upper_bound);
                break;
        }
    } else {
        asInterval();
        DataPredicate rightCopy = predicate;
        rightCopy.asInterval();

        value = std::max(value, rightCopy.value);
        value_upper_bound = std::min(value_upper_bound, rightCopy.value_upper_bound);

        std::unordered_set<std::variant<std::string, double>> S;
        for (const auto& x : exceptions) {
            if ((value <= x) && (x <= value_upper_bound))
                S.insert(x);
        }
        for (const auto& x : rightCopy.exceptions) {
            if ((value <= x) && (x <= value_upper_bound))
                S.insert(x);
        }
        exceptions = S;
    }
}

void DataPredicate::asInterval() {
    if (casusu == INTERVAL) return;
    static double MIN_DOUBLE = -std::numeric_limits<double>::max();
    static double MAX_DOUBLE = std::numeric_limits<double>::max();
    static std::string MIN_STRING;
    static std::string MAX_STRING = std::string(MAXIMUM_STRING_LENGTH, std::numeric_limits<char>::max());

    bool isString = std::holds_alternative<std::string>(value);
    std::variant<std::string, double> prev, next;
    std::variant<std::string, double> min, max;
    if (isString)  {
        std::string s = std::get<std::string>(value);
        min = MIN_STRING;
        max = MAX_STRING;
        prev = prev_char(s, MAXIMUM_STRING_LENGTH);
        next = next_char(s, MAXIMUM_STRING_LENGTH);
    } else {
        double x_val = std::get<double>(value);
        min = MIN_DOUBLE;
        max = MAX_DOUBLE;
        prev = std::nextafter(x_val, -DBL_MAX);
        next = std::nextafter(x_val, DBL_MAX);
    }

    switch (casusu) {
        case LT:
            value = min;
            value_upper_bound = prev;
            break;
        case GT:
            value = next;
            value_upper_bound = max;
            break;
        case LEQ:
            value_upper_bound = value;
            value = min;
            break;
        case GEQ:
            value_upper_bound = max;
            break;
        case EQ:
            value_upper_bound = value;
            break;
        case NEQ:
            exceptions.insert(value);
            value = min;
            value_upper_bound = max;
            break;
    }

    casusu = INTERVAL;
}

bool DataPredicate::test(const std::string &val) const {
    bool isString = std::holds_alternative<std::string>(value);
    if (!isString) return false;
    std::string current = std::get<std::string>(value);
    switch (casusu) {
        case LT:
            return val < current;
        case GT:
            return val > current;
        case LEQ:
            return val <= current;
        case GEQ:
            return val >= current;
        case EQ:
            return val == current;
        case NEQ:
            return val != current;
        case INTERVAL: {
            std::variant<std::string,double> curr{current};
            if (exceptions.contains(curr)) return false;
            return ((current <= val)
                    && (val <= std::get<std::string>(value_upper_bound)));
        }
    }
}

bool DataPredicate::test(double val) const {
    bool isDouble = std::holds_alternative<double>(value);
    if (!isDouble) return false;
    double current = std::get<double>(value);
    switch (casusu) {
        case LT:
            return val < current;
        case GT:
            return val > current;
        case LEQ:
            return val <= current;
        case GEQ:
            return val >= current;
        case EQ:
            return val == current;
        case NEQ:
            return val != current;
        case INTERVAL: {
            std::variant<std::string,double> curr{current};
            if (exceptions.contains(curr)) return false;
            return ((current <= val)
                    && (val <= std::get<double>(value_upper_bound)));
        }
    }
}
