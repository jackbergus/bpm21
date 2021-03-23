/*
 * nfa2dfa.h
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
// Created by giacomo on 28/02/21.
//

#ifndef CLASSIFIERS_NFA2DFA_H
#define CLASSIFIERS_NFA2DFA_H

#include <graphs/FlexibleFA.h>
#include <graphs/NodeLabelBijectionFA.h>
#include <iostream>

template<typename NodeElement, typename EdgeLabel>
NodeLabelBijectionFA<std::unordered_set<NodeElement>, EdgeLabel> NFA2DFA(FlexibleFA<NodeElement, EdgeLabel>& graph, const EdgeLabel& epsilon) {
    NodeLabelBijectionFA<std::unordered_set<NodeElement>, EdgeLabel> result;
    // E-closure for the initial nodes
    std::unordered_set<size_t> tS;

    // Defining a set of sets
    std::unordered_set<std::unordered_set<size_t>> calT, unmarked{}, marked{}, initial;

    auto S = graph.ClosureId(graph.init(), epsilon);
    calT.insert(S);
    unmarked.insert(S);
    initial.insert(S);

    // Defining the transition for the final DFA
    std::unordered_map<std::unordered_set<size_t>, std::unordered_map<EdgeLabel, std::unordered_set<size_t>>> trDelta;

    // While there is still an unmarked P in calT
    while (!unmarked.empty()) {

        // Extract an element, and mark it!
        std::unordered_set<size_t> P = *unmarked.begin();
        unmarked.erase(unmarked.begin());
        marked.insert(P);

        for (const auto& label_to_set : graph.Move(P)) {
            if (label_to_set.first == epsilon) continue;
            std::unordered_set<size_t> R = graph.Closure(label_to_set.second, epsilon);
            std::cout << P << " --[" << label_to_set.first << "]--> "<< R << std::endl;
            if (!marked.contains(R)) {
                calT.insert(R);
                unmarked.insert(R);
            }
            trDelta[P][label_to_set.first] = R;
        }
    }


    // now, construct the NFA from the collected information!
    std::unordered_map<std::unordered_set<size_t>, size_t> neueNodeIds;
    for (const std::unordered_set<size_t>& S : calT) {
        std::unordered_set<NodeElement> lS;
        for (int x : S)
            lS.insert(graph.getNodeLabel(x));
        int lSId = result.addUniqueStateOrGetExisting(lS);
        neueNodeIds[S] = lSId;

        if (!unordered_intersection(S, graph.fini()).empty()) {
            result.addToFinalNodes(lS);
        }
        if (initial.contains(S)) {
            result.addToInitialNodesFromId(lSId);
        }
    }

    for (const auto& src_to_label_to_dst : trDelta) {
        int src = neueNodeIds.at(src_to_label_to_dst.first);
        for (const auto& label_to_ds : src_to_label_to_dst.second) {
            result.addNewEdgeFromId(src, neueNodeIds.at(label_to_ds.second), label_to_ds.first);
        }
    }

    return result;
}

#endif //CLASSIFIERS_NFA2DFA_H
