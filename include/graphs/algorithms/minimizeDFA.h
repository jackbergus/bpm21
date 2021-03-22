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

#include <unordered_set>



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

#define DOVETAIL(x, y, N)       ((x) * (N) + (y))
#define DOVETAIL_LEFT(x, N)     ((x) / (N))
#define DOVETAIL_RIGHT(x,N)     ((x) % (N))
#define DOVETAIL_CP(cp, N)      DOVETAIL(cp.first, cp.second, N)


#define DOVETAIL2(i,j,N)        (((N)-(i)-1)*((N)-i)/2 + (N) - (j))

struct table_content {
    std::vector<size_t> S;
    bool                       cross;
    bool                       is_set;
    bool                       is_bool;
    size_t                     left;
    size_t                     right;

    size_t const_dovetail(size_t max) const {
        return DOVETAIL(left, right, max);
    }


    table_content() : is_set{false}, is_bool{false}, cross{false}, S{}, left{0}, right{0} {}
    table_content(const std::vector<size_t>& S, size_t left, size_t right) : is_set{true}, is_bool{false}, cross{false}, S{S}, left{left}, right{right} {}
    table_content(bool cross, size_t left, size_t right) : is_set{true}, is_bool{true}, cross{cross}, S{}, left{left}, right{right} {}
    table_content(const table_content& ) = default;
    table_content(table_content&& ) = default;
    table_content& operator=(const table_content& ) = default;
    table_content& operator=( table_content&& ) = default;
};

/*void marcaRec2(size_t cp, size_t max_V,
              std::vector<table_content> &M);*/

void marcaRec3(size_t M_offset, std::vector<table_content> &M);

#include <boost/dynamic_bitset.hpp>

template<typename NodeElement, typename EdgeLabel>
FlexibleFA<std::vector<NodeElement>, EdgeLabel> minimizeDFA(FlexibleFA<NodeElement, EdgeLabel>& graph) {
    std::cout << "minimizeDFA" << std::endl;
    /*{
        std::ofstream before{"before.dot"};
        graph.dot(before, false);
    }*/

    std::vector<size_t> v = graph.getNodeIds();
    size_t V = v.size();
    FlexibleFA<std::vector<NodeElement>, EdgeLabel> result;

    // Skipping the algorithm if the graph contains zero or just one node!
    if (V == 0) {
        return result;
    } else if (V == 1) {
        size_t n = v.at(0);
        size_t id = result.addNewNodeWithLabel({graph.getNodeLabel(n)});
        if (graph.isInitialNodeByID(n))
            result.addToFinalNodesFromId(id);
        if (graph.isFinalNodeByID(n))
            result.addToFinalNodesFromId(id);
        for (auto& edge : graph.outgoingEdges(n)) {
            assert(edge.second == n);
            result.addNewEdgeFromId(id, id, edge.first);
        }
        return result;
    }

    // Running the algorithm for the graphs containing at least two nodes.
    std::vector<bool> isFinal(V, false);
    std::vector<bool> isInitial(V, false);
    std::unordered_set<EdgeLabel> sigma;
    std::vector<NodeElement> node_labels(V);
    size_t max_graph_id = 0;
    std::vector<std::unordered_map<EdgeLabel, size_t>> localDelta(V);
    std::unordered_map<size_t, size_t> node_to_pos;
    {
        size_t i = 0;
        for (size_t qp : v) {
            node_to_pos[qp] = i;
            isFinal[i] = graph.isFinalNodeByID(qp);
            isInitial[i] = graph.isInitialNodeByID(qp);
            node_labels[i] = graph.getNodeLabel(qp);
            i++;
        }
    }
    for (size_t qp : v) {
        for (const std::pair<EdgeLabel, size_t> &edge : graph.outgoingEdges(qp)) {
            sigma.insert(edge.first);
            localDelta[node_to_pos.at(qp)][edge.first] = node_to_pos.at(edge.second);
        }
    }
    size_t Tabsize = (V <= 1) ? 0 : ((V*(V-1))/2);
    //std::unordered_map<std::pair<size_t, size_t>, std::variant<std::unordered_set<std::pair<size_t, size_t>>, bool>> M(max_graph_id);
    std::vector<table_content> M(Tabsize);

    std::cout << " * algorithm sorting" << std::endl;
    std::sort(v.begin(), v.end());
    adjacency_graph graph_tr_clos;
    {
        {
            for (size_t j = 0; j < V; j++) {
                assert( j == graph_tr_clos.add_node() );
                size_t qp = v.at(j);
                for (size_t i = 0; i < j; i++) {
                    size_t pp = v.at(i);
                    if ((isFinal[i] && (!isFinal[j])) ||
                        (isFinal[j] && (!isFinal[i]))) {
                        M[DOVETAIL2(i,j,V-1)] = {false, i, j};
                    } else {
                        M[DOVETAIL2(i,j,V-1)] = {std::vector<size_t>{}, i, j};
                    }
                }
            }
        }

        std::cout << " * Pair" << std::endl;
        for (size_t pos = 0, tabPos = M.size(); pos<tabPos; pos++) {
            auto cp2 = &M[pos];
            size_t left = cp2->left;
            size_t right = cp2->right;
            if (!cp2->is_bool) {
                bool Xfound = false;
                std::vector<size_t> L;
                for (const auto &a : sigma) {
                    size_t cp_first = localDelta[left][a];
                    size_t cp_second = localDelta[right][a];
                    if (cp_first != cp_second) {
                        if (cp_first > cp_second)
                            std::swap(cp_first, cp_second);
                        size_t local_pos = DOVETAIL2(cp_first, cp_second, V-1);
                        if (M[local_pos].is_bool) {
                            marcaRec3(pos, M); // Marca X
                            Xfound = true;
                            break;
                        } else {
                            L.emplace_back(local_pos);
                        }
                    }

                }
                if (!Xfound) {
                    for (const size_t &cp3 : L) {
                        M[cp3].S.emplace_back(pos);
                    }
                }
            }
        }
    }

    using bitset = boost::dynamic_bitset<>;
    std::cout << " * Equivalence Classes" << std::endl;
    std::vector<std::unordered_set<size_t>> trivial_eq_map(V);
    std::unordered_set<size_t> equivalentNodes, VS;
    VS.insert(v.begin(), v.end());
    for (size_t pos = 0, tabPos = M.size(); pos<tabPos; pos++) {
        auto cp = &M[pos];
        size_t l = cp->left;
        size_t r = cp->right;
        if (!cp->is_bool) {
            equivalentNodes.insert(l);
            equivalentNodes.insert(r);
            //trivial_eq_map[l].insert(r);
            //trivial_eq_map[r].insert(l);
            graph_tr_clos.add_undirected_edge(l, r);
        }
    }

    std::cout << " * Transitive Closure" << std::endl;
    std::vector<bool> visited_E(V, false);
    std::unordered_map<std::unordered_set<size_t>, size_t> eq_map;
    for (size_t k : equivalentNodes) {
        if (!visited_E[k]) {
            std::unordered_set<size_t> visited;
            graph_tr_clos.DFSUtil(k, visited);
            std::vector<NodeElement> nl;
            for (size_t x : visited) {
                trivial_eq_map[x] = visited;
                visited_E[x] = true;
                nl.emplace_back(node_labels[x]);
            }
            eq_map.emplace(visited, result.addNewNodeWithLabel(nl));
        }
    }
    /*for (size_t k = 0; k < V; k++){
        auto& refK = trivial_eq_map[k];
        for (size_t i = 0; i < V; i++) {
            auto& refI = trivial_eq_map[i];
            for (size_t j = 0; j < V; j++) {
                if (refI.contains(k) && refK.contains(j)) {
                    refI.insert(j);
                }
            }
        }
    }

    for (const std::unordered_set<size_t>& ref : trivial_eq_map) {
        if (ref.empty()) continue;
        auto it = eq_map.emplace(ref, 0);
        if (it.second) {
            std::cout << "{" << std::endl;
            for (size_t id : ref)
                std::cout << node_labels[id] << ",";
            std::cout << "}"  << std::endl;
            std::vector<NodeElement> nl;
            for (const size_t& x : ref) {
                nl.emplace_back(node_labels[x]);
            }
            it.first->second = result.addNewNodeWithLabel(nl);
        }
    }*/

    std::cout << " * Unordered difference" << std::endl;
    for (const auto& cp : unordered_difference(VS, equivalentNodes)) {
        std::cout << cp << std::endl;
        trivial_eq_map[cp] = {cp};
        eq_map[{cp}] = result.addNewNodeWithLabel({node_labels[cp]});
    }

    std::cout << " * graph recreation" << std::endl;
    for (size_t pos = 0; pos<V; pos++) {
        if (isInitial[pos]) {
            assert(eq_map.contains(trivial_eq_map.at(pos)));
            result.addToInitialNodesFromId(eq_map.at(trivial_eq_map.at(pos)));
        }
        if (isFinal[pos]) {
            assert(eq_map.contains(trivial_eq_map.at(pos)));
            result.addToFinalNodesFromId(eq_map.at(trivial_eq_map.at(pos)));
        }
    }

    for (const auto& node : eq_map) {
        assert(!node.first.empty());
        std::cout << "{" ;
        for (size_t id : node.first)
            std::cout << node_labels[id] << ",";
        std::cout << "} == " << eq_map.at(node.first) << std::endl;

        std::unordered_map<EdgeLabel, std::unordered_set<size_t>> reachable_in_old_graph;
        for (const size_t& p : node.first) {
            for (const std::pair<EdgeLabel, size_t>& cp: localDelta.at(p)) {
                auto id2 = eq_map.at(trivial_eq_map.at(cp.second));
                reachable_in_old_graph[cp.first].insert(id2);
            }
        }

        for (const std::pair<EdgeLabel, std::unordered_set<size_t>>& eOut : reachable_in_old_graph) {
            if (!(eOut.second.size() == 1)) {
                for (const auto& ref : reachable_in_old_graph) {
                    std::cout << "\t --[" << ref.first << "]->" << ref.second << std::endl;
                }
                assert(eOut.second.size() == 1);
            }
            result.addNewEdgeFromId(node.second, *eOut.second.begin(), eOut.first);
        }

    }

    result.pruneUnreachableNodes();
    /*{
        std::ofstream after{"after.dot"};
        result.dot(after, false);
    }*/
    return result;
}

template<typename NodeElement, typename EdgeLabel>
FlexibleFA<std::vector<NodeElement>, EdgeLabel> minimizeDFA_old(FlexibleFA<NodeElement, EdgeLabel>& graph) {
    std::unordered_map<std::pair<size_t, size_t>, std::variant<std::unordered_set<std::pair<size_t, size_t>>, bool>> M;

    std::vector<size_t> v = graph.getNodeIds();
    std::vector<size_t> notFinals;
    std::unordered_set<EdgeLabel> sigma;
    std::unordered_map<size_t, std::unordered_map<EdgeLabel, size_t>> localDelta;
    for (size_t qp : v) {
        if (!graph.final_nodes.contains(qp)) {
            notFinals.emplace_back(qp);
        }
        for (const std::pair<EdgeLabel, size_t> &edge : graph.outgoingEdges(qp)) {
            sigma.insert(edge.first);
            localDelta[qp][edge.first] = edge.second;
        }
    }

    std::sort(v.begin(), v.end());
    {
        std::pair<size_t, size_t> cp;
        {
            for (size_t i = 0, N = v.size(); i < N; i++) {
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
        for (const auto& cp : M) {
            if (!std::holds_alternative<bool>(cp.second)) {
                size_t l = cp.first.first, r = cp.first.second;
                if (l>r) std::swap(l, r);
                cl.insert(l, r);
            }
        }
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
        for (const auto& cp : unordered_difference(VS, equivalentNodes)) {
            full_eq_class[cp] = {cp};
            nodeToId.emplace(std::unordered_set<size_t>{cp}, result.addNewNodeWithLabel(std::vector<NodeElement>{graph.getNodeLabel(cp)}));
        }
    }

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

    //result.pruneUnreachableNodes();
    return result;
}



#endif //CLASSIFIERS_MINIMIZEDFA_H
