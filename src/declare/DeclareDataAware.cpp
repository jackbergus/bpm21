/*
 * DeclareDataAware.cpp
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


#include <magic_enum.hpp>
#include <utils/xml_utils.h>
#include "declare/DeclareDataAware.h"

void print_dnf(std::ostream &os, const std::vector<std::unordered_map<std::string, DataPredicate>> &map) {
    static std::string OR{" ∨ "};
    for (size_t i = 0, N = map.size(); i<N; i++) {
        const std::unordered_map<std::string, DataPredicate>& elem = map.at(i);
        if (N>1) os << '(';
        print_conj(os, elem);
        if (N>1) os << ')';
        if (i != (N-1)) {
            os << OR;
        }
    }
}

void print_conj(std::ostream &os, const std::unordered_map<std::string, DataPredicate> &map) {
    static std::string AND{" ∧ "};
    size_t i = 0, N = map.size();
    for (const std::pair<std::string, DataPredicate>& elem : map) {
        os << elem.second;
        if (i != (N-1)) {
            os << AND;
        }
        i++;
    }
}


std::ostream &operator<<(std::ostream &os, const DeclareDataAware &aware) {
    os << magic_enum::enum_name(aware.casusu);
    os << '(' << aware.left_act << ", ";
    if (aware.dnf_left_map.empty())
        os << "true";
    else
        print_dnf(os, aware.dnf_left_map);
    os << ", ";
    if (!aware.right_act.empty()) {
        os << aware.right_act << ", ";
        if (aware.dnf_right_map.empty())
            os << "true";
        else
            print_dnf(os, aware.dnf_right_map);
    } else {
        os << aware.n;
    }
    return os << " )";
}

ltlf map_conj(const std::unordered_map<std::string, DataPredicate> &map) {
    ltlf result = ltlf::True();
    size_t i = 0, N = map.size();
    for (const std::pair<std::string, DataPredicate>& elem : map) {
        if (i == 0) {
            result = ltlf::Interval(elem.second);
        } else {
            result = ltlf::And(ltlf::Interval(elem.second), result);
        }
        i++;
    }
    return result;
}

ltlf map_disj(const std::vector<std::unordered_map<std::string, DataPredicate>> &map) {
    ltlf result = ltlf::True();
    for (size_t i = 0, N = map.size(); i<N; i++) {
        const std::unordered_map<std::string, DataPredicate>& elem = map.at(i);
        if (i == 0) {
            result = map_conj(elem);
        } else {
            result = ltlf::Or(map_conj(elem), result);
        }
    }
    return result;
}

ltlf DeclareDataAware::toFiniteSemantics() const {
    ltlf left = ltlf::And(ltlf::Act(left_act), map_disj(dnf_left_map)).simplify().setBeingCompound(true);

    ltlf right = ltlf::True();
    if (!right_act.empty()) {
        right = ltlf::And(ltlf::Act(right_act), map_disj(dnf_right_map)).simplify().setBeingCompound(true);
    }

    switch (casusu) {
        case Existence:
            if (n > 1) {
                return ltlf::Diamond(
                        ltlf::And(left, ltlf::Next(doExistence(n - 1, left_act, dnf_left_map).toFiniteSemantics()))
                );
            } else if (n == 1) {
                return ltlf::Diamond(left);
            } else {
                return ltlf::Diamond(left).negate();
            }
        case Absence:
            return ltlf::Neg(doExistence(n, left_act, dnf_left_map).toFiniteSemantics());

        case Absence2:
            return ltlf::Neg(ltlf::Diamond(ltlf::And(left, ltlf::Diamond(left))));

        case Exactly:
            return ltlf::And(doExistence(n, left_act, dnf_left_map).toFiniteSemantics(), doAbsence(n + 1, left_act, dnf_left_map).toFiniteSemantics());

        case Init:
            return left;

        case RespExistence:
            return ltlf::Implies(ltlf::Diamond(left),
                                 ltlf::Diamond(right));

        case Choice:
            return ltlf::Or(ltlf::Diamond(left), ltlf::Diamond(right));

        case ExlChoice:
            return ltlf::And(ltlf::Or(ltlf::Diamond(left), ltlf::Diamond(right)),
                             ltlf::Neg(ltlf::And(ltlf::Diamond(left), ltlf::Diamond(right))));

        case CoExistence:
            return ltlf::And(ltlf::Implies(ltlf::Diamond(left),
                                           ltlf::Diamond(right)),
                             ltlf::Implies(ltlf::Diamond(right),
                                           ltlf::Diamond(left)));

        case Response:
            return ltlf::Box(ltlf::Implies(left,
                                           ltlf::Diamond(right)));

        case NegationResponse:
            return ltlf::Neg(ltlf::Box(ltlf::Implies(left,
                                           ltlf::Diamond(right))));

        case Precedence:
            return ltlf::WeakUntil(ltlf::Neg(right), left);

        case NegationPrecedence:
            return ltlf::Neg(ltlf::WeakUntil(ltlf::Neg(right), left));

        case AltResponse:
            return ltlf::Box(ltlf::Implies(right,
                                           ltlf::Next(ltlf::Until(left.negate(), right))));


        case NegationAltResponse:
            return ltlf::Neg(ltlf::Box(ltlf::Implies(right,
                                           ltlf::Next(ltlf::Until(left.negate(), right)))));

        case AltPrecedence: {
            struct ltlf base = ltlf::WeakUntil(right.negate(), left);
            return ltlf::And(base, ltlf::Box(ltlf::Implies(right,
                                                           ltlf::Next(base))));
        }

        case AltSuccession: {
            auto L = ltlf::Box(ltlf::Implies(right,
                                                ltlf::Next(ltlf::Until(left.negate(), right))));
            struct ltlf base = ltlf::WeakUntil(right.negate(), left);
            auto R =  ltlf::And(base, ltlf::Box(ltlf::Implies(right,
                                                           ltlf::Next(base))));
            return ltlf::And(L, R);
        }

        case ChainResponse:
            return ltlf::Box(ltlf::Implies(left, ltlf::Next(right)));

        case ChainPrecedence:
            return ltlf::Box(ltlf::Implies( ltlf::Next(right), left));

        case ChainSuccession:
            return ltlf::Box(ltlf::Equivalent(left, ltlf::Next(right)));

        case Succession: {
            struct ltlf base = ltlf::WeakUntil(right.negate(), left);
            return ltlf::And(base, ltlf::Box(ltlf::Implies(left, ltlf::Diamond(right))));
        }

        case End:
            return ltlf::Diamond(ltlf::And(left, ltlf::Neg(ltlf::Next(ltlf::Or(left, left.negate())))));

        case NotPrecedence:
            return ltlf::Box(ltlf::Implies(left, ltlf::Neg(ltlf::Diamond(right))));

        case NotSuccession:
            return ltlf::Box(ltlf::Implies(left,
                                           ltlf::Neg(ltlf::Diamond(right))));

        case NotResponse:
            return ltlf::Box(ltlf::Implies(left, ltlf::Neg(ltlf::Box(right))));

        case NotChainPrecedence:
            return ltlf::Box(ltlf::Implies(left, ltlf::Neg(ltlf::Next(right))));

        case NotCoExistence:
            return ltlf::Neg(ltlf::And(ltlf::Diamond(left), ltlf::Diamond(right)));

        case NegSuccession:
            return ltlf::Box(ltlf::Implies(left, ltlf::Neg(ltlf::Box(right))));

        case NotChainResponse:
            return ltlf::Box(ltlf::Implies(left, ltlf::Neg(ltlf::Next(right))));

        case NegChainSuccession:
            return ltlf::Box(ltlf::Equivalent(left, ltlf::Next(right.negate())));

        case NotChainSuccession:
            return ltlf::Box(ltlf::Implies(left, ltlf::Next(right.negate())));

        case NotRespExistence:
            return ltlf::Implies(left, ltlf::Neg(ltlf::Diamond(right)));
    }
}

DeclareDataAware DeclareDataAware::doExistence(size_t n, const std::string &left_act,
                                               const std::vector<std::unordered_map<std::string, DataPredicate>> &dnf_left_map) {
    DeclareDataAware sol;
    sol.n = n;
    sol.casusu = Existence;
    sol.left_act = left_act;
    sol.dnf_left_map = dnf_left_map;
    return sol;
}

DeclareDataAware DeclareDataAware::doAbsence(size_t n, const std::string &left_act,
                                             const std::vector<std::unordered_map<std::string, DataPredicate>> &dnf_left_map) {
    DeclareDataAware sol;
    sol.n = n;
    sol.casusu = Absence;
    sol.left_act = left_act;
    sol.dnf_left_map = dnf_left_map;
    return sol;
}

DeclareDataAware DeclareDataAware::parse_declare_non_data_string(const std::string &line) {
    DeclareDataAware pattern;
    std::string nextLine;

    ssize_t pos = line.find('[');
    assert(pos != std::string::npos);
    std::string pattern_name = line.substr(0, pos);
    pattern.casusu = magic_enum::enum_cast<declare_templates>(pattern_name).value();

    pattern_name = line.substr(pos+1);
    pos = pattern_name.find(',');
    assert(pos != std::string::npos);
    pattern.left_act = pattern_name.substr(0, pos);
    STRIP_ALL_SPACES(pattern.left_act);

    pattern_name = pattern_name.substr(pos+1);
    pos = pattern_name.find(']');
    assert(pos != std::string::npos);
    std::string second_or_number = pattern_name.substr(0, pos);
    STRIP_ALL_SPACES(second_or_number);

    char* p;
    unsigned long converted = strtoul(second_or_number.c_str(), &p, 10);
    if (*p) {
        pattern.right_act = second_or_number;
        pattern.n = 1;
    } else {
        pattern.n = converted;
    }

    return pattern;
}

#include <fstream>

std::vector<DeclareDataAware> DeclareDataAware::load_simplified_declare_model(std::ifstream &infile) {
    std::string line;
    std::vector<DeclareDataAware> V;
    while (std::getline(infile, line)) {
        V.emplace_back(DeclareDataAware::parse_declare_non_data_string(line));
    }
    return V;
}
