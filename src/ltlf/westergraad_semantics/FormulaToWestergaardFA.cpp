/*
 * FormulaToWestergaardFA.cpp
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
// Created by giacomo on 12/03/21.
//

#include <ltlf/westergraad_semantics/FormulaToWestergaardFA.h>
#define DEBUG

FormulaToWestergaardFA::FormulaToWestergaardFA(const ltlf &formula) : normalized_formula{formula.nnf().simplify()} {
    int final = fa.addUniqueStateOrGetExisting(ltlf::True());
    fa.addToFinalNodesFromId(final);
    pozzo = fa.addUniqueStateOrGetExisting(ltlf::True().negate());
#ifdef DEBUG
    std::cout << "Final=" << final << std::endl;
#endif
    fa.addNewEdgeFromId(final, final, WestergaardNFALabel<std::string>::Sigma());
    fa.addNewEdgeFromId(pozzo, pozzo, WestergaardNFALabel<std::string>::Sigma());
#ifdef DEBUG
    std::cout << "⊤ ~~{*}~~> ⊤" << std::endl;
    std::cout << "bot ~~{*}~~> bot" << std::endl;
#endif
    first = true;
    fillUp();
}

void FormulaToWestergaardFA::asMultigraph(NodeLabelBijectionFA<ltlf, std::string> &multigraph, const std::vector<std::string> &allSigma) {
    fillUp();
    std::unordered_set<std::string> localSymbols = totalSymbols;
    localSymbols.insert(allSigma.begin(), allSigma.end());

    for (size_t element = 0, N =  fa.maximumNodeId(); element<N; element++) {
        auto node = fa.getUniqueLabel(element);
        int neueId = multigraph.addUniqueStateOrGetExisting(node);
        if (fa.isFinalNodeByID(element))
            multigraph.addToFinalNodesFromId(neueId);
        if (fa.isInitialNodeByID(element))
            multigraph.addToInitialNodesFromId(neueId);
    }

    for (size_t element = 0, N =  fa.maximumNodeId(); element<N; element++) {
        auto node = fa.getUniqueLabel(element);
        auto multigraphNodeId = multigraph.getId(node);
        for (const std::pair<WestergaardNFALabel<std::string>, int>& edge : fa.outgoingEdges(node)) {
            for (const std::string& label : localSymbols) {
                if (edge.first(label)) {
                    multigraph.addNewEdgeL(multigraphNodeId, fa.getUniqueLabel(edge.second), label);
                }
            }
        }
    }
}

int FormulaToWestergaardFA::_visit(const ltlf &formula, int pos) {
    auto it = fa.hasNode(formula);
    if (it) {
        return it.value();
    } else {
        int fId = fa.addUniqueStateOrGetExisting(formula);
        if (formula.isPotentialFinalState()) {
#ifdef DEBUG
            std::cout << "Final = " << fId << std::endl;
#endif
            fa.addToFinalNodesFromId(fId);
        }
        if (first) {
#ifdef DEBUG
            std::cout << "Init = " << fId << std::endl;
#endif
            fa.addToInitialNodesFromId(fId);
            first=false;
        }
        auto expanded = formula.stepwise_expand();
        auto actions = formula.possibleActionsUpToNext();
        std::string anyString = actions.different_label();
        auto allActions = formula.propositionalize();
        std::unordered_set<std::string> except;
        for (const std::string& y : allActions) {
            std::unordered_set<std::string> S{y};
            except.insert(y);
            totalSymbols.insert(y);
            auto next_cp = expanded.interpret(S);
            auto next = next_cp.nnf().simplify().oversimplify();
            if (next.casusu != FALSE) {
                auto nextId = _visit(next, pos+1);
                if (nextId== fId) {
                    except.erase(y);
                } else {
#ifdef DEBUG
                    std::cout << std::string(pos, '\t') << /*formula*/fId << " ~~{" << y  <<"}~~> " << /*next*/nextId << std::endl;
#endif
                    fa.addNewEdgeFromId(fId, nextId, WestergaardNFALabel<std::string>::LabelSet(S));
                }
            } else {
#ifdef DEBUG
                std::cout << std::string(pos, '\t') << /*formula*/fId << " ~~{" << y  <<"}~~> bot" << std::endl;
#endif
                fa.addNewEdgeFromId(fId, pozzo, WestergaardNFALabel<std::string>::LabelSet(S));
            }
        }
        std::unordered_set<std::string> S{anyString};
        auto next_cp = expanded.interpret(S);
        auto next = next_cp.nnf().simplify().oversimplify();
        if (next.casusu != FALSE) {
            auto nextId = _visit(next, pos+1);
#ifdef DEBUG
            std::cout << /*formula*/fId << " ~~ Othw\\{";
            for (const auto& y : except)
                std::cout << y << ',';
            std::cout << "}" << "~~> " << /*next*/nextId << std::endl;
#endif
            fa.addNewEdgeFromId(fId, nextId, WestergaardNFALabel<std::string>::Otherwise(except));
        }
        return fId;
    }
}

void FormulaToWestergaardFA::fillUp() {
    if (first)
        _visit(normalized_formula);
}

std::ostream &operator<<(std::ostream &os, const FormulaToWestergaardFA &fa) {
    return os << fa.fa;
}

void FormulaToWestergaardFA::asMultigraphIgnoreNodeLabels(NodeLabelBijectionFA<std::string, std::string> &multigraph,
                                                          const std::vector<std::string> &allSigma) {
    fillUp();
    std::unordered_set<std::string> localSymbols = totalSymbols;
    localSymbols.insert(allSigma.begin(), allSigma.end());

    std::unordered_map<ltlf, size_t> inverseMap;

    for (size_t element = 0, N =  fa.maximumNodeId(); element<N; element++) {
        auto node = fa.getUniqueLabel(element);
        inverseMap[node] = element;
        int neueId = multigraph.addUniqueStateOrGetExisting(std::to_string(element));
        if (fa.isFinalNodeByID(element))
            multigraph.addToFinalNodesFromId(neueId);
        if (fa.isInitialNodeByID(element))
            multigraph.addToInitialNodesFromId(neueId);
    }

    for (size_t element = 0, N =  fa.maximumNodeId(); element<N; element++) {
        auto node = fa.getUniqueLabel(element);
        auto multigraphNodeId = multigraph.getId(std::to_string(element));
        for (const std::pair<WestergaardNFALabel<std::string>, int>& edge : fa.outgoingEdges(node)) {
            for (const std::string& label : localSymbols) {
                if (edge.first(label)) {
                    multigraph.addNewEdgeL(multigraphNodeId, std::to_string(inverseMap.at(fa.getUniqueLabel(edge.second))), label);
                }
            }
        }
    }
}
