/*
 * ltlf.h
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
// Created by giacomo on 18/02/21.
//

#ifndef CLASSIFIERS_LTLF_H
#define CLASSIFIERS_LTLF_H


#include <unordered_set>

enum formula_t {
    ACT,
    NEG_OF,
    OR,
    AND,
    NEXT,
    UNTIL,
    RELEASE,
    TRUE,
    FALSE,
    NUMERIC_ATOM
};



#include <string>
#include <vector>
#include <ostream>
#include <ltlf/PropositionalizedAtomsSet.h>
#include "DataPredicate.h"


struct ltlf {
    formula_t              casusu;
    std::string            act;
    std::vector<ltlf>      args;
    bool                   is_negated;
    bool                   is_compound_predicate;
    DataPredicate          numeric_atom;

    // C++ constructors
    ltlf();
    ltlf(const std::string& act);
    ltlf(formula_t citki);
    ltlf(const ltlf& ) = default;
    ltlf(ltlf&&      ) = default;
    ltlf& operator=(const ltlf&) = default;
    ltlf& operator=(ltlf&&     ) = default;

    // Semantic constructors
    static struct ltlf True();
    static struct ltlf Act(const std::string& act);
    static struct ltlf Interval(const std::string &var, numeric_atom_cases case_, const std::string &value);
    static struct ltlf Interval(const std::string &var, numeric_atom_cases case_, double value);
    static struct ltlf Interval(const DataPredicate& value);
    static struct ltlf Neg(const ltlf& sub);
    static struct ltlf Next(const ltlf& sub);
    static struct ltlf Or(const ltlf& left, const ltlf& right);
    static struct ltlf And(const ltlf& left, const ltlf& right);
    static struct ltlf Until(const ltlf& left, const ltlf& right);
    static struct ltlf Release(const ltlf& left, const ltlf& right);
    static struct ltlf Diamond(const ltlf& sub);
    static struct ltlf Box(const ltlf& sub);
    static struct ltlf Last();
    static struct ltlf Implies(const ltlf& left, const ltlf& right);
    static struct ltlf Equivalent(const ltlf& left, const ltlf& right);
    static struct ltlf WeakUntil(const ltlf& left, const ltlf& right);

    // Structural operators

    const struct ltlf& reduce() const {
        return *this;
    }
    struct ltlf& setBeingCompound(bool isCompound) {
        is_compound_predicate = isCompound;
        return *this;
    }
    struct ltlf simplify() const;
    struct ltlf oversimplify() const;
    struct ltlf negate() const;
    struct ltlf nnf() const;
    struct ltlf stepwise_expand() const;
    //std::unordered_set<struct ltlf> propositionalize() const;
    //PropositionalizedAtomsSet possibleActionsUpToNext() const;

    //std::unordered_set<std::string> allActions() const;


    /*struct ltlf interpret(const std::unordered_set<std::string>& I) const {
        return nnf().simplify().stepwise_expand()._interpret(I);
    }

    struct ltlf interpret2(const std::unordered_set<struct ltlf>& I) const {
        return nnf().simplify().stepwise_expand()._interpret2(I);
    }*/

    /*bool isPotentialFinalState() const {
        return nnf().simplify()._isPotentialFinalState().casusu == TRUE;
    }*/

    void collectStructuralElements(formula_t type, std::unordered_set<ltlf>& set, bool simplificationType) const;
    bool containsElement(formula_t type, const ltlf& item, bool simplificationType) const;

    // Printing

    friend std::ostream &operator<<(std::ostream &os, const ltlf &syntax);

    // Comparison Operators
    bool operator==(const ltlf &rhs) const;
    bool operator!=(const ltlf &rhs) const;

private:
   //void _propositionalize(std::unordered_set<struct ltlf>& atoms, bool blockNext = false) const;
   // struct ltlf _interpret(const std::unordered_set<std::string>& map) const;
    //void _actionsUpToNext(PropositionalizedAtomsSet &atoms, bool isTerminal = true) const;
    //struct ltlf _isPotentialFinalState() const;
    //struct ltlf _interpret2(const std::unordered_set<struct ltlf>& map) const;
   // void _allActions(std::unordered_set<std::string> &labels) const;
};

#include <utils/numeric/vector_hash.h>
#include <iostream>

namespace std {
    template <>
    struct hash<struct ltlf>
    {
        std::size_t operator()(const ltlf& k) const
        {
            size_t init = 31;
            if ((k.casusu == OR) || (k.casusu == AND)) {
                std::unordered_set<ltlf> elements;
                k.collectStructuralElements(k.casusu, elements, true);
                for (const auto& arg : elements)
                    init += (*this)(arg);
            } else {
                for (const auto& x : k.args)
                    init = hash_combine<ltlf>(init, x);
            }
            size_t f= hash_combine<bool>(hash_combine<std::string>(hash_combine<formula_t>(init, k.casusu), k.act), k.is_negated);
            return f;
        }
    };

}


#endif //GRAPHOS_LTL_SYNTAX_H
