/*
 * ltlf.cpp
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

#include <ltlf/ltlf.h>
#include <z3++.h>

ltlf::ltlf() : casusu{TRUE}, is_negated{false}, is_compound_predicate{false} { }

ltlf::ltlf(const std::string &act) : casusu{ACT}, act{act}, is_negated{false}, is_compound_predicate{false}  {}

ltlf::ltlf(formula_t citki) : casusu{citki}, is_negated{false}, is_compound_predicate{false}   {}

struct ltlf ltlf::True() { return {}; }

struct ltlf ltlf::Act(const std::string &act) { return {act}; }

struct ltlf ltlf::Neg(const ltlf &sub) {
    ltlf formula{NEG_OF};
    formula.args.emplace_back(sub);
    return formula;
}

struct ltlf ltlf::Next(const ltlf &sub) {
    ltlf formula{NEXT};
    formula.args.emplace_back(sub);
    return formula;
}

struct ltlf ltlf::Or(const ltlf &left, const ltlf &right) {
    struct ltlf formula{OR};
    formula.args.emplace_back(left);
    formula.args.emplace_back(right);
    return formula;
}

struct ltlf ltlf::And(const ltlf &left, const ltlf &right) {
    struct ltlf formula{AND};
    formula.args.emplace_back(left);
    formula.args.emplace_back(right);
    return formula;
}

struct ltlf ltlf::Diamond(const ltlf &sub) {
    return Until(True(), sub);
}

struct ltlf ltlf::Box(const ltlf &sub) {
    return Neg(Diamond(Neg(sub)));
}

struct ltlf ltlf::Last() {
    return Neg(Next(True()));
}

struct ltlf ltlf::Until(const ltlf &left, const ltlf &right) {
    struct ltlf formula{UNTIL};
    formula.args.emplace_back(left);
    formula.args.emplace_back(right);
    return formula;
}

struct ltlf ltlf::Release(const ltlf &left, const ltlf &right) {
    struct ltlf formula{RELEASE};
    formula.args.emplace_back(left);
    formula.args.emplace_back(right);
    return formula;
}

struct ltlf ltlf::Implies(const ltlf &left, const ltlf &right) {
    return Or(Neg(left), right);
}

struct ltlf ltlf::Equivalent(const ltlf &left, const ltlf &right) {
    return Or(And(left, right), And(left.negate(), right.negate()));
}

struct ltlf ltlf::WeakUntil(const ltlf &left, const ltlf &right) {
    return Or(Until(left, right), Box(left));
}


std::ostream &operator<<(std::ostream &os, const ltlf &syntax) {
    std::string reset = "";
    /*if (syntax.is_compound_predicate) {
        os << "\033[32m";
        reset = "\033[0m";
    }*/
    if (syntax.is_negated)
        os << "!";
    switch (syntax.casusu) {
        case ACT:
            return os << syntax.act << reset;
        case NUMERIC_ATOM:
            return os << syntax.numeric_atom<< reset;
        case NEG_OF:
            return os << "(!(" << syntax.args[0] << "))"<< reset;
        case OR:
            return os << "(" << syntax.args[0] << ") | (" << syntax.args[1] << ')'<< reset;
        case AND:
            return os << "(" << syntax.args[0] << ") & (" << syntax.args[1] << ')'<< reset;
        case NEXT:
            return os << "X(" << syntax.args[0] << ")"<< reset;
        case UNTIL:
            return os << "(" << syntax.args[0] << ") U (" << syntax.args[1] << ')'<< reset;
        case RELEASE:
            return os << "(" << syntax.args[0] << ") R (" << syntax.args[1] << ')'<< reset;
        case TRUE:
            return os << "true"<< reset;
        default:
            return os << "false"<< reset;
    }
}

struct ltlf ltlf::replace_with_unique_name(const std::unordered_map<std::string, std::string> &map) const {
    switch (casusu) {
        case ACT:
            if (map.contains(act)) {
                auto f = ltlf::Act(map.at(act));
                if (is_negated)
                    f = f.negate();
                return f;
            } else {
                assert(false);
            }

        case NEG_OF:
            return ltlf::Neg(args.at(0).replace_with_unique_name(map));
        case NEXT:
            return ltlf::Next(args.at(0).replace_with_unique_name(map));
        case OR:
            return ltlf::Or(args.at(0).replace_with_unique_name(map),
                            args.at(1).replace_with_unique_name(map));
        case AND:
            return ltlf::And(args.at(0).replace_with_unique_name(map),
                            args.at(1).replace_with_unique_name(map));

        case UNTIL:
            return ltlf::Until(args.at(0).replace_with_unique_name(map),
                            args.at(1).replace_with_unique_name(map));
        case RELEASE:
            return ltlf::Release(args.at(0).replace_with_unique_name(map),
                            args.at(1).replace_with_unique_name(map));
        case TRUE:
        case FALSE:
            return *this;
        default:
            throw std::runtime_error("ERROR: the expression shall not contain an interval");
    }
}

bool ltlf::easy_interpret(const std::string &map) const {
    switch (casusu) {
        case TRUE:
            return true;
        case FALSE:
            return false;
        case ACT:
            if (!is_negated) {
                bool res = (map == (act));
                return res;
            } else {
                bool res = !(map == (act));
                return res;
            }
        case NEG_OF:
            return !args.at(0).easy_interpret(map);
        case OR: {
            return args.at(0).easy_interpret(map) || args.at(1).easy_interpret( map);
        }
        case AND: {
            return args.at(0).easy_interpret(map) && args.at(1).easy_interpret( map);
        }
        case NEXT:
            return args.at(0).easy_interpret(map); // Stops the interpretation before the current element
        default:
            assert(false);
    }
}

/*void ltlf::_allActions(std::unordered_set<std::string> &labels) const {
    switch (casusu) {
        case ACT: {}
            labels.insert(act);
            break;
        case TRUE:
        case FALSE:
            break;
        default:
            for (const auto& arg : args)
                arg._allActions(labels);
    }
}*/

/*std::unordered_set<std::string> ltlf::allActions() const {
    std::unordered_set<std::string> result;
    _allActions(result);
    return result;
}*/

struct ltlf ltlf::oversimplify() const {
    auto falsehood = True().negate().simplify();
    auto truth = True();
    switch (casusu) {
        case NEG_OF:
            return this->nnf().oversimplify().setBeingCompound(is_compound_predicate);
        case OR: {

            {
                std::unordered_set<ltlf> set;
                std::unordered_set<ltlf> removed;
                std::vector<ltlf> toIterateOn{set.begin(), set.end()};
                collectStructuralElements(OR, set, true);
                if (set.contains(falsehood))
                    set.erase(falsehood);
                if (set.contains(truth))
                    return truth;
                for (const auto& arg : set)
                    if (set.contains(arg.negate().nnf().simplify()))
                        return truth;
                for (const auto& arg : toIterateOn) {
                    if (removed.contains(arg)) continue;
                    if ((arg.casusu == UNTIL) && (set.contains(arg.args.at(1)))) {
                        removed.emplace(arg.args.at(1));
                        set.erase(arg.args.at(1));
                    }
                    for (auto it = set.begin(); it != set.end(); ) {
                        if (*it == arg) continue;
                        if ((it->casusu == AND) && (it->containsElement(AND, arg, true))) {
                            removed.emplace(*it);
                            it = set.erase(it);
                        } else if ((it->casusu == RELEASE) && (it->args.at(1) == arg)) {
                            removed.emplace(*it);
                            it = set.erase(it);
                        } else {
                            it++;
                        }
                    }
                }
                assert(set.size() >0);
                if (set.size()== 1) {
                    auto f = *set.begin();
                    f.setBeingCompound(is_compound_predicate);
                    return f;
                } else {
                    auto f = *set.begin();
                    auto it = set.begin(); it++;
                    for (; it != set.end(); it++) {
                        f = Or(*it, f);
                    }
                    return f.setBeingCompound(is_compound_predicate);
                }
            }
        }
        case AND: {

            {
                std::unordered_set<ltlf> set;
                collectStructuralElements(AND, set, true);
                if (set.contains(truth))
                    set.erase(truth);
                if (set.contains(falsehood))
                    return falsehood;
                for (const auto& arg : set)
                    if (set.contains(arg.negate().simplify()))
                        return falsehood;
                assert(set.size() >0);
                if (set.size()== 1) {
                    auto f = *set.begin();
                    f.setBeingCompound(is_compound_predicate);
                    return f;
                } else {
                    auto f = *set.begin();
                    auto it = set.begin(); it++;
                    for (; it != set.end(); it++) {
                        f = And(*it, f);
                    }
                    return f.setBeingCompound(is_compound_predicate);
                }
            }
        }
        case NEXT: {
            auto arg = args.at(0).oversimplify();
            switch (arg.casusu) {
                case TRUE:
                case FALSE:
                    return arg.setBeingCompound(is_compound_predicate);
                case OR:
                    return Or(Next(arg.args.at(0)), Next(arg.args.at(1))).oversimplify().setBeingCompound(is_compound_predicate);
                case AND:
                    return And(Next(arg.args.at(0)), Next(arg.args.at(1))).oversimplify().setBeingCompound(is_compound_predicate);
                default:
                    return Next(arg).setBeingCompound(is_compound_predicate);
            }
        }
        case UNTIL: {
            auto left = args.at(0).oversimplify();
            auto right = args.at(1).oversimplify();
            if (right.casusu == OR) {
                return Or(Until(left, right.args.at(0)), Until(left, right.args.at(1))).setBeingCompound(is_compound_predicate);
            } else if (left.casusu == AND) {
                return And(Until(left.args.at(0), right), Until(left.args.at(1), right)).setBeingCompound(is_compound_predicate);
            } else {
                return Until(left, right).setBeingCompound(is_compound_predicate);
            }
        }
        case RELEASE:
            return ltlf::Release(args.at(0).oversimplify(), args.at(1).oversimplify()).setBeingCompound(is_compound_predicate);
        default:
            return {*this};
    }
}

void ltlf::collectStructuralElements(formula_t type, std::unordered_set<ltlf> &set, bool simplificationType) const {
    if (type != casusu) return;
    else for (const auto& arg : args) {
            if (arg.casusu == type) arg.collectStructuralElements(type, set, simplificationType);
            else set.emplace(simplificationType ? arg.oversimplify() : arg.simplify());
        }
}

bool ltlf::containsElement(formula_t type, const ltlf& item, bool simplificationType) const {
    if (type != casusu) return false;
    else {
        for (const auto& arg : args) {
            if (arg.casusu == type) if (arg.containsElement(type, item, simplificationType)) return true;
                else {
                    auto v = simplificationType ? arg.oversimplify() : arg.simplify();
                    if (v == item) return true;
                }
        }
        return false;
    }
}

/*struct ltlf ltlf::_interpret2(const std::unordered_set<struct ltlf> &map) const {
    switch (casusu) {
        case ACT: {
            if (map.contains(*this))
                return True();
        }
            return map.contains(*this) ? True() : ltlf{FALSE};
        case NEG_OF:
        case UNTIL:
        case RELEASE:
            assert(false);
        case OR: {
            return Or(args.at(0)._interpret2(map), args.at(1)._interpret2(map)).simplify();
        }
        case AND: {
            return And(args.at(0)._interpret2(map), args.at(1)._interpret2(map)).simplify();
        }
        case NEXT:
            return args.at(0); // Stops the interpretation before the current element
        default:
            return {*this};
    }
}*/

struct ltlf ltlf::_isPotentialFinalState() const {
    switch (casusu) {
        case ACT:
        case TRUE:
        case FALSE:
        case NEG_OF:
        case NEXT:
        case UNTIL:
        case NUMERIC_ATOM:
            return {FALSE};

        case OR:
            return Or(args.at(0)._isPotentialFinalState(), args.at(1)._isPotentialFinalState()).nnf().simplify();

        case AND:
            return And(args.at(0)._isPotentialFinalState(), args.at(1)._isPotentialFinalState()).nnf().simplify();

        case RELEASE:
            return True();

        default:
            throw std::runtime_error("Unexpected case");
    }
}

#include <cassert>


void ltlf::_actionsUpToNext(PropositionalizedAtomsSet &atoms, bool isTerminal) const {
    switch (casusu) {
        case ACT: {}
            atoms.insert(act, isTerminal);
            break;
        case NEG_OF:
            std::cerr << "Error: this should be always called after negation normal form, so I should not be able to call this" << std::endl;
            assert(false);
        case NEXT:
            return;
            //args.at(0)._actionsUpToNext(atoms, false);
        case TRUE:
        case FALSE:
            break;
        case OR:
        case AND:
            args.at(0)._actionsUpToNext(atoms, isTerminal);
            args.at(1)._actionsUpToNext(atoms, isTerminal);
            break;
        case UNTIL:
        case RELEASE:
        case NUMERIC_ATOM:
            std::cerr << "Error: this should be always called after next normal form, so I should not be able to call this" << std::endl;
            assert(false);
    }
}

struct ltlf ltlf::_interpret(const std::unordered_set<std::string>& map) const {
    switch (casusu) {
        case ACT:
            if (!is_negated) {
                return map.contains(act) ? True() : ltlf{FALSE};
            } else {
                return map.contains(act) ? ltlf{FALSE}: True();
            }
        case NEG_OF:
        case UNTIL:
        case RELEASE:
            assert(false);
        case OR: {
            return Or(args.at(0)._interpret(map), args.at(1)._interpret(map)).simplify();
        }
        case AND: {
            return And(args.at(0)._interpret(map), args.at(1)._interpret(map)).simplify();
        }
        case NEXT:
            return args.at(0); // Stops the interpretation before the current element
        default:
            return {*this};
    }
}

PropositionalizedAtomsSet ltlf::possibleActionsUpToNext() const {
    PropositionalizedAtomsSet result;
    nnf().stepwise_expand()._actionsUpToNext(result, true);
    return result;
}

std::unordered_set<std::string> ltlf::propositionalize() const {
    std::unordered_set<std::string> result;
    simplify()._propositionalize(result);
    return result;
}


void ltlf::_propositionalize(std::unordered_set<std::string > &atoms, bool blockNext) const {
    switch (casusu) {
        case ACT: {}
            atoms.insert({act});
            break;
        case NEG_OF:
        case NEXT:
            if (!blockNext)
                args.at(0)._propositionalize(atoms, blockNext);
            break;
        case OR:
        case AND:

        case UNTIL:  //TODO
        case RELEASE://TODO
            args.at(0)._propositionalize(atoms, blockNext);
            args.at(1)._propositionalize(atoms, blockNext);
            break;
        /*Original
         *
         * case UNTIL:
        case RELEASE:
            if (!atoms.contains(*this)) {
                atoms.insert(*this);
                stepwise_expand()._propositionalize(atoms, blockNext);
            }
            break;*/
        case TRUE:
        case FALSE:
            break;
        case NUMERIC_ATOM:
            throw std::runtime_error("ERROR: the expression shall not contain an interval");
    }
}

struct ltlf ltlf::stepwise_expand() const {
    switch (casusu) {
        case NEXT: {
            return ltlf::Next(args.at(0).simplify());
        }
        case OR: {
            auto left = args.at(0).simplify()
                    .stepwise_expand();
            auto right = args.at(1).simplify().stepwise_expand();
            return ltlf::Or(left, right).simplify();
        }
        case AND: {
            auto left = args.at(0).simplify().stepwise_expand();
            auto right = args.at(1).simplify().stepwise_expand();
            return ltlf::And(left, right).simplify();
        }
        case NEG_OF:
            return args.at(0).negate();
        case UNTIL: {
            auto left = args.at(0).simplify();
            auto right = args.at(1).simplify();
            return ltlf::Or(right, ltlf::And(left, ltlf::Next(*this))).simplify().stepwise_expand();
        }
        case RELEASE:{
            auto left = args.at(0).simplify();
            auto right = args.at(1).simplify();
            return ltlf::And(right, ltlf::Or(left, ltlf::Next(*this))).simplify().stepwise_expand();
        }

        default:
            return {*this};
    }
}

bool ltlf::operator==(const ltlf &rhs) const {
    bool ca = casusu == rhs.casusu;
    if (!ca) return false;

    bool preliminar = (act == rhs.act) &&(is_negated == rhs.is_negated);
    if (!preliminar) return false;

    switch (casusu) {
        case TRUE:
        case FALSE:
        case ACT:
        case NEXT:
        case UNTIL:
        case RELEASE:
        case NEG_OF:
            return (args == rhs.args);

        case NUMERIC_ATOM:
            return (numeric_atom == rhs.numeric_atom);

        case AND:
        case OR: {
            std::unordered_set<ltlf> leftS, rightS;
            collectStructuralElements(casusu, leftS, true);
            rhs.collectStructuralElements(casusu, rightS, true);

            for (const auto& arg : leftS)
                if (!rightS.contains(arg)) return false;
            for (const auto & arg : rightS)
                if (!leftS.contains(arg)) return false;
            return true;
        }

        default:
            throw std::runtime_error("Unexpected case");
    }

}

bool ltlf::operator!=(const ltlf &rhs) const {
    return !(rhs == *this);
}

struct ltlf ltlf::simplify() const {
    switch (casusu) {
        case NEG_OF:
            return this->nnf().simplify();
        case OR: {
            auto left = args.at(0).simplify();
            auto right = args.at(1).simplify();
            if (left == right)
                return left.setBeingCompound(is_compound_predicate);
            if ((left.casusu == TRUE) || (right.casusu == TRUE) || ((left == right.negate())))
                return True();
            else if (left.casusu == FALSE)
                return right.setBeingCompound(is_compound_predicate);
            else if ((right.casusu == TRUE) || (left == right))
                return left.setBeingCompound(is_compound_predicate);
            else if (right.casusu == AND) {
                right.args[0] = right.args.at(0).simplify();
                right.args[1] = right.args.at(1).simplify();
                if ((left == right.args.at(0)) || (left == right.args.at(1))) {
                    return left.setBeingCompound(is_compound_predicate);
                } else {
                    return ltlf::Or(left, right).setBeingCompound(is_compound_predicate);
                }
            } else if (left.casusu == AND) {
                left.args[0] = left.args.at(0).simplify();
                left.args[1] = left.args.at(1).simplify();
                if ((right == left.args.at(0)) || (right == left.args.at(1))) {
                    return right;
                } else {
                    return ltlf::Or(left, right).setBeingCompound(is_compound_predicate);
                }
            } else {
                return ltlf::Or(left, right).setBeingCompound(is_compound_predicate);
            }
        }
        case AND: {
            auto left = args.at(0).simplify();
            auto right = args.at(1).simplify();
            if (left == right)
                return left.setBeingCompound(is_compound_predicate);
            if ((left.casusu == FALSE) || (right.casusu == FALSE))
                return True().negate();
            else if (left.casusu == TRUE)
                return right.setBeingCompound(is_compound_predicate);
            else if (right.casusu == TRUE)
                return left.setBeingCompound(is_compound_predicate);
            else if (right.casusu == OR) {
                right.args[0] = right.args.at(0).simplify();
                right.args[1] = right.args.at(1).simplify();
                if ((left == right.args.at(0)) || (left == right.args.at(1))) {
                    return left.setBeingCompound(is_compound_predicate);
                } else {
                    return ltlf::Or(ltlf::And(left, right.args.at(0)),
                                    ltlf::And(left, right.args.at(1))).simplify().setBeingCompound(is_compound_predicate);
                }
            } else if (left.casusu == OR) {
                left.args[0] = left.args.at(0).simplify();
                left.args[1] = left.args.at(1).simplify();
                if ((right == left.args.at(0)) || (right == left.args.at(1))) {
                    return right.setBeingCompound(is_compound_predicate);
                } else {
                    return ltlf::Or(ltlf::And(right, left.args.at(0)),
                                    ltlf::And(right, left.args.at(1))).simplify().setBeingCompound(is_compound_predicate);
                }
            } else if (left.casusu == AND) {
                left.args[0] = left.args.at(0).simplify();
                left.args[1] = left.args.at(1).simplify();
                if (((right == left.args.at(0)))&&((right == left.args.at(1)))) {
                    return right.setBeingCompound(is_compound_predicate);
                } else if ((right == left.args.at(0))) {
                    return ltlf::And(right, left.args.at(1)).setBeingCompound(is_compound_predicate);
                } else if ((right == left.args.at(1))) {
                    return ltlf::And(right, left.args.at(0)).setBeingCompound(is_compound_predicate);
                } else {
                    return *this;
                }
            } else if (right.casusu == AND) {
                right.args[0] = right.args.at(0).simplify();
                right.args[1] = right.args.at(1).simplify();
                if (((left == right.args.at(0)))&&((left == right.args.at(1)))) {
                    return left;
                } else if ((left == right.args.at(0))) {
                    return ltlf::And(left, right.args.at(1)).setBeingCompound(is_compound_predicate);
                } else if ((left == right.args.at(1))) {
                    return ltlf::And(left, right.args.at(0)).setBeingCompound(is_compound_predicate);
                } else {
                    return *this;
                }
            } else {
                return ltlf::And(left, right).setBeingCompound(is_compound_predicate);
            }
        }
        case NEXT: {
            return ltlf::Next(args.at(0).simplify()).setBeingCompound(is_compound_predicate);
        }
        case UNTIL:
            return ltlf::Until(args.at(0).simplify(), args.at(1).simplify()).setBeingCompound(is_compound_predicate);
        case RELEASE:
            return ltlf::Release(args.at(0).simplify(), args.at(1).simplify()).setBeingCompound(is_compound_predicate);

        default:
            return {*this};
    }
}



#include <tuple>




struct ltlf ltlf::negate() const {
    switch (casusu) {
        case ACT: {
            struct ltlf curr = *this;
            curr.is_negated = !curr.is_negated;
            return curr.setBeingCompound(is_compound_predicate);
        }
        case NUMERIC_ATOM: {
            DataPredicate local = numeric_atom;
            switch (numeric_atom.casusu) {
                case LT:
                    local.casusu = GEQ;
                    return ltlf::Interval(local).setBeingCompound(is_compound_predicate);

                case GT:
                    local.casusu = LEQ;
                    return ltlf::Interval(local).setBeingCompound(is_compound_predicate);

                case GEQ:
                    local.casusu = LT;
                    return ltlf::Interval(local).setBeingCompound(is_compound_predicate);

                case LEQ:
                    local.casusu = GT;
                    return ltlf::Interval(local).setBeingCompound(is_compound_predicate);

                case EQ:
                    local.casusu = NEQ;
                    return ltlf::Interval(local).setBeingCompound(is_compound_predicate);

                case NEQ:
                    local.casusu = EQ;
                    return ltlf::Interval(local).setBeingCompound(is_compound_predicate);

                case INTERVAL: {
                    ltlf formula = ltlf::True().negate();
                    bool isFormulaSet = false;

                    bool isString = std::holds_alternative<std::string>(numeric_atom.value);
                    std::variant<std::string, double> l_min, l_max;
                    if (isString) {
                        l_min = DataPredicate::MIN_STRING;
                        l_max = DataPredicate::MAX_STRING;
                    } else {
                        l_min = DataPredicate::MIN_DOUBLE;
                        l_max = DataPredicate::MAX_DOUBLE;
                    }

                    if (numeric_atom.value != l_min) {
                        isFormulaSet = true;
                        DataPredicate min;
                        min.value = l_min;
                        min.value_upper_bound = DataPredicate::prev_of(numeric_atom.value);
                        min.var = numeric_atom.var;
                        min.label = numeric_atom.label;
                        formula = ltlf::Interval(min);
                    }
                    if (numeric_atom.value_upper_bound != l_max) {
                        DataPredicate max;
                        max.value = DataPredicate::next_of(numeric_atom.value_upper_bound);
                        max.value_upper_bound = l_max;
                        max.var = numeric_atom.var;
                        max.label = numeric_atom.label;
                        if (isFormulaSet) {
                            formula = ltlf::Or(ltlf::Interval(max), formula);
                        } else {
                            isFormulaSet = true;
                            formula = ltlf::Interval(max);
                        }
                    }

                    for (const auto& elem : numeric_atom.exceptions) {
                        DataPredicate element;
                        element.value = elem;
                        element.value_upper_bound = elem;
                        element.var = numeric_atom.var;
                        element.label = numeric_atom.label;
                        if (isFormulaSet) {
                            formula = ltlf::Or(ltlf::Interval(element), formula);
                        } else {
                            isFormulaSet = true;
                            formula = ltlf::Interval(element);
                        }
                    }

                    return formula.setBeingCompound(is_compound_predicate);
                }
            }
        }
        case NEG_OF:
            if (args.at(0).casusu == NEG_OF)
                return args.at(0).args.at(0).nnf();
            else
                return args.at(0).simplify();
        case OR:
            return And(args.at(0).negate(), args.at(1).negate()).setBeingCompound(is_compound_predicate);
        case AND:
            return Or(args.at(0).negate(), args.at(1).negate()).setBeingCompound(is_compound_predicate);
        case NEXT:
            return Next(args.at(0).negate()).setBeingCompound(is_compound_predicate);
        case UNTIL:
            return Release(args.at(0).negate(), args.at(1).negate()).setBeingCompound(is_compound_predicate);
        case RELEASE:
            return Until(args.at(0).negate(), args.at(1).negate()).setBeingCompound(is_compound_predicate);
        case TRUE:
            return {FALSE};
        case FALSE:
            return True();
        default:
            throw std::runtime_error("Unexpected case");
    }
}

struct ltlf ltlf::nnf() const {
    switch (casusu) {
        case NEG_OF:
            if (args.at(0).casusu == NEG_OF)
                return args.at(0).args.at(0).nnf();
            else
                return args.at(0).negate();
        case OR:
            return Or(args.at(0).nnf(), args.at(1).nnf()).setBeingCompound(is_compound_predicate);
        case AND:
            return And(args.at(0).nnf(), args.at(1).nnf()).setBeingCompound(is_compound_predicate);
        case NEXT:
            return Next(args.at(0).nnf()).setBeingCompound(is_compound_predicate);
        case UNTIL:
            return Until(args.at(0).nnf(), args.at(1).nnf()).setBeingCompound(is_compound_predicate);
        case RELEASE:
            return Release(args.at(0).nnf(), args.at(1).nnf()).setBeingCompound(is_compound_predicate);
        default:
            return {*this};
    }
}


struct ltlf ltlf::Interval(const std::string &var, numeric_atom_cases case_, const std::string& value) {
    ltlf formula;
    formula.numeric_atom = {var, case_, value};
    formula.casusu = NUMERIC_ATOM;
    return formula;
}

struct ltlf ltlf::Interval(const std::string &var, numeric_atom_cases case_, double value) {
    ltlf formula;
    formula.numeric_atom = {var, case_, value};
    formula.casusu = NUMERIC_ATOM;
    return formula;
}

struct ltlf ltlf::Interval(const DataPredicate &value) {
    ltlf formula;
    formula.numeric_atom = value;
    formula.casusu = NUMERIC_ATOM;
    return formula;
}
