/*
 * minimizeDFA.h
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

#ifndef CLASSIFIERS_MINIMIZEDFA_H
#define CLASSIFIERS_MINIMIZEDFA_H


#include <utils/numeric/pair_hash.h>
#include <utils/numeric/vector_hash.h>
#include <utils/numeric/uset_hash.h>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <variant>
#include <graphs/FlexibleFA.h>
#include <iostream>
#include <utils/eq_class/equivalence_class.h>

void marcaRec(std::pair<size_t,size_t> cp, std::unordered_map<std::pair<size_t, size_t>, std::variant<std::unordered_set<std::pair<size_t,size_t>>, bool>>& M);
/*
template<typename NodeElement, typename EdgeLabel>
void marcaDebug(const FlexibleFA<NodeElement, EdgeLabel>& graph, std::pair<size_t, size_t> cp,
              std::unordered_map<std::pair<size_t, size_t>, std::variant<std::unordered_set<std::pair<size_t, size_t>>, bool>> &M) {
    if (cp.second < cp.first)
        std::swap(cp.first, cp.second);
    auto it = M.find(cp);
    std::unordered_set<std::pair<size_t,size_t>> ls;
    if (std::holds_alternative<std::unordered_set<std::pair<size_t,size_t>>>(it->second)) {
        ls = std::get<std::unordered_set<std::pair<size_t,size_t>>>(it->second);
    }
    std::cout << "\t\t<" << graph.getNodeLabel(it->first.first) <<',' << graph.getNodeLabel(it->first.second) << "> marked!" << std::endl;
    it->second = {false};
    for (std::pair<size_t,size_t> x : ls) {
        if (x.second < x.first)
            std::swap(x.first, x.second);
        marcaRec(x, M);
    }
}*/



template<typename NodeElement, typename EdgeLabel>
FlexibleFA<std::vector<NodeElement>, EdgeLabel> minimizeDFA(FlexibleFA<NodeElement, EdgeLabel>& graph) {
    std::cout << "minimizeDFA" << std::endl;

    std::vector<size_t> v = graph.getNodeIds();
    std::vector<size_t> notFinals;
    std::unordered_set<EdgeLabel> sigma;
    std::unordered_map<size_t, std::unordered_map<EdgeLabel, size_t>> localDelta;
    std::cout << " * algorithm initialization" << std::endl;
    size_t max_graph_id = 0;
    for (size_t qp : v) {
        if (!graph.final_nodes.contains(qp)) {
            notFinals.emplace_back(qp);
        }
        for (const std::pair<EdgeLabel, size_t> &edge : graph.outgoingEdges(qp)) {
            sigma.insert(edge.first);
            localDelta[qp][edge.first] = edge.second;
        }
        max_graph_id = std::max(max_graph_id, qp);
    }
    max_graph_id++;
    std::unordered_map<std::pair<size_t, size_t>, std::variant<std::unordered_set<std::pair<size_t, size_t>>, bool>> M(max_graph_id);


    std::cout << " * algorithm sorting" << std::endl;
    std::sort(v.begin(), v.end());
    {
        std::pair<size_t, size_t> cp;
        {
            for (size_t i = 0, N = v.size(); i < N; i++) {
                std::cout << " * iteration " << i << std::endl;
                size_t pp = v.at(i);
                cp.first = pp;
                for (size_t j = i + 1; j < N; j++) {
                    size_t qp = v.at(j);
                    cp.second = qp;
                    if (qp < pp) {
                        std::swap(cp.first, cp.second);
                    }
                    //std::cout << '{' << graph.getNodeLabel(cp.first) <<',' << graph.getNodeLabel(cp.second) <<'}' << std::endl;
                    if ((graph.final_nodes.contains(pp) && (!graph.final_nodes.contains(qp))) ||
                        (graph.final_nodes.contains(qp) && (!graph.final_nodes.contains(pp)))) {
                        M[cp] = {false};
                    } else {
                        M[cp] = {std::unordered_set<std::pair<size_t, size_t>>{}};
                    }
                }
            }
        }

        std::cout << " * Pair" << std::endl;
        for (auto &cp2 : M) {

            if (!std::holds_alternative<bool>(cp2.second)) {
                //std::cout << '<' << graph.getNodeLabel(cp2.first.first) <<',' << graph.getNodeLabel(cp2.first.second) <<'>' << std::endl;
                bool Xfound = false;
                std::unordered_set<std::pair<size_t, size_t>> L;
                for (const auto &a : sigma) {
                    std::unordered_set<size_t> S;
                    S.insert(cp2.first.first);
                    S.insert(cp2.first.second);
                    auto S2 = graph.Move2(S, a);
                    auto it = S2.begin();
                    if (S2.size() > 1) {
                        cp.first = *it;
                        cp.second = *(++it);
                        if (cp.first > cp.second)
                            std::swap(cp.first, cp.second);
                        if (std::holds_alternative<bool>(M[cp])) {
                            //std::cout << "\t--> <" << graph.getNodeLabel(cp.first) <<',' << graph.getNodeLabel(cp.second) <<"> XXX" << std::endl;
                            marcaRec(/*graph,*/ cp2.first, M);
                            Xfound = true;
                            break;
                        } else {
                            if (cp.first != cp.second) {
                                //std::cout << "\t--> <" << graph.getNodeLabel(cp.first) <<',' << graph.getNodeLabel(cp.second) <<">  <<=" << std::endl;
                                L.insert(cp);
                            }
                        }
                    }

                }
                if (!Xfound) {
                    for (const std::pair<size_t, size_t> &cp3 : L) {
                        auto it = M.find(cp3);
                        assert(!std::holds_alternative<bool>(it->second));
                        std::get<std::unordered_set<std::pair<size_t, size_t>>>(it->second).insert(cp2.first);
                    }
                }
            }
        }
    }

    size_t vCount = 0;
    std::unordered_set<size_t> equivalentNodes, VS;
    VS.insert(v.begin(), v.end());
    FlexibleFA<std::vector<NodeElement>, EdgeLabel> result;
    std::unordered_map<std::unordered_set<size_t>, size_t> nodeToId;
    std::unordered_map<size_t, std::unordered_set<size_t>> full_eq_class;
    {
        std::unordered_set<size_t> preserved;
        equivalence_class<size_t> cl;
        std::cout << " * init equivalence class" << std::endl;
        for (const auto& cp : M) {
            if (!std::holds_alternative<bool>(cp.second)) {
                size_t l = cp.first.first, r = cp.first.second;
                if (l>r) std::swap(l, r);
                cl.insert(l, r);
            }
        }
        std::cout << " * calculate eq class" << std::endl;
        for (const auto& cp : cl.calculateEquivalenceClass()) {
            equivalentNodes.insert(cp.second.begin(), cp.second.end());
            auto it = nodeToId.emplace(cp.second, vCount);
            if (it.second) {
                vCount++;
                std::vector<NodeElement> nl;
                for (const auto& x : cp.second) {
                    nl.emplace_back(graph.getNodeLabel(x));
                }
                it.second = result.addNewNodeWithLabel(nl);
            }
            for (const auto& elements : cp.second) {
                full_eq_class[elements] = cp.second;
            }
        }

        std::cout << " * Unordered difference" << std::endl;
        for (const auto& cp : unordered_difference(VS, equivalentNodes)) {
            full_eq_class[cp] = {cp};
            nodeToId.emplace(std::unordered_set<size_t>{cp}, result.addNewNodeWithLabel(std::vector<NodeElement>{graph.getNodeLabel(cp)}));
        }
    }

    std::cout << " * graph recreation" << std::endl;
    for (size_t fin : graph.fini()) {
        result.addToFinalNodesFromId(nodeToId.at(full_eq_class.at(fin)));
    }
    for (size_t ini : graph.init()) {
        result.addToInitialNodesFromId(nodeToId.at(full_eq_class.at(ini)));
    }
    for (const auto& node : nodeToId) {
        std::cout << "{" ;
        for (size_t id : node.first)
            std::cout << graph.getNodeLabel(id) << ",";
        std::cout << "}"  << std::endl;

#if 1
        for (const std::pair<EdgeLabel, std::unordered_set<size_t>>& eOut :
                graph.Move3(node.first)) {
#if 1
            std::unordered_set<size_t> S;
            for (const auto& elem : eOut.second) {
                auto f = full_eq_class.at(elem);
                S.insert(f.begin(), f.end());
            }
            auto it = nodeToId.find(S);
            assert(it != nodeToId.end());
#else
            auto it = nodeToId.find(eOut.second);
            // TODO: assert(it != nodeToId.end());
            if (it != nodeToId.end())
#endif
            /*std::cout << "{" ;
            for (size_t id : node.first)
                std::cout << graph.getNodeLabel(id) << ",";
            std::cout << "}"  ;
            std::cout <<  "--[" << eOut.first << "]-->";

            std::cout << "{" ;
            for (size_t id : S)
                std::cout << graph.getNodeLabel(id) << ",";
            std::cout << "}"  << std::endl;*/
            result.addNewEdgeFromId(node.second, it->second, eOut.first);
        }
#endif
    }

    result.pruneUnreachableNodes();
    return result;
}



#endif //CLASSIFIERS_MINIMIZEDFA_H
