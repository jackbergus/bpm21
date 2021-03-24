/*
 * alignment_utils.cpp
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
// Created by giacomo on 24/03/21.
//

#include "alignment/alignment_utils.h"


#include <lemon/smart_graph.h>
#include <lemon/connectivity.h>
#include <lemon/dijkstra.h>
#include <lemon/dfs.h>

double align(const FlexibleFA<std::string, size_t>& x, const std::string& path, const std::string& epsilon) {

    lemon::SmartDigraph g;
    lemon::SmartDigraph::ArcMap<int> costMap(g);

    FlexibleFA<std::string, size_t> graph = x;
    //FlexibleFA<std::string, size_t> printer;
    {
        size_t ini = graph.addNewNodeWithLabel(epsilon);
        for (size_t s : graph.initial_nodes) {
            graph.addNewEdgeFromId(ini, s, 0);
        }
        graph.initial_nodes.clear();
        graph.initial_nodes.insert(ini);
    }
    if (graph.final_nodes.size()> 1) {
        size_t fin = graph.addNewNodeWithLabel(epsilon);
        for (size_t f : graph.final_nodes) {
            graph.addNewEdgeFromId(f, fin, 0);
        }
        graph.final_nodes.clear();
        graph.final_nodes.insert(fin);
    }

    size_t tmpIni = *graph.initial_nodes.begin();
    size_t tmpFin = *graph.final_nodes.begin();
    std::vector<size_t> V = graph.getNodeIds();
    std::unordered_map<size_t, size_t> map;
    size_t Vsize = V.size();
    size_t pathSize = path.size();
    size_t init = -1, fini = -1;

    for (size_t i = 0, N = Vsize; i<N; i++) {
        size_t actualId = V.at(i);
        std::string label = graph.getNodeLabel(V.at(i));
        map[V.at(i)] = i;
        for (size_t j = 0, M = pathSize; j<M+1; j++) {
            size_t cmp = (i * (M+1))+j;
            //size_t tmpp = printer.addNewNodeWithLabel(label);
            if ((actualId == tmpIni) && (j == 0)) {
                init = cmp;
                //printer.addToInitialNodesFromId(tmpp);
                ///std::cout <<"=>";
            }
            ///std::cout << cmp << " ~ " << label;
            if ((actualId == tmpFin) && (j == M)) {
                fini = cmp;
                //printer.addToFinalNodesFromId(tmpp);
                ///std::cout << "XXX";
            }
            ///std::cout << std::endl;
            auto node = g.addNode();
            assert(g.id(node) == cmp);
        }
    }

    for (size_t i = 0, N = Vsize; i<N; i++) {
        size_t v = V.at(i);

        for (const std::pair<size_t, size_t>& edge : graph.outgoingEdges(v)) {
            for (size_t t = 0, M = pathSize; t < M + 1; t++) {
                size_t src = i*(M+1) + t;
                auto srcNode = lemon::SmartDigraph::nodeFromId(src);
                size_t dst_1 = map[edge.second]*(M+1) + t;

                std::string element = graph.getNodeLabel(edge.second);
                if (element == epsilon) {
                    ///std::cout << src << "-[0]->"  << dst_1 << std::endl;
                    //printer.addNewEdgeFromId(src, dst_1, 0);
                    costMap[g.addArc(srcNode, lemon::SmartDigraph::nodeFromId(dst_1))] = 0; // Epsilon moves have zero costs
                } else {
                    ///std::cout << src << "-[1]->"  << dst_1 << std::endl;
                    //printer.addNewEdgeFromId(src, dst_1, 1);
                    costMap[g.addArc(srcNode, lemon::SmartDigraph::nodeFromId(dst_1))] = 1; // Non-epsilon moves have costs!
                }

                if (t <= M-1) {
                    std::string t_str{path.at(t)};
                    size_t dst_2 = map[edge.second]*(M+1) + (t+1);
                    if (t_str == element) {
                        ///std::cout << src << "-[0]->"  << dst_2 << std::endl;
                        //printer.addNewEdgeFromId(src, dst_2, 0);
                        costMap[g.addArc(srcNode, lemon::SmartDigraph::nodeFromId(dst_2))] = 0;
                    } else {
                        ///std::cout << src << "-[1]->"  << dst_2 << std::endl;
                        //printer.addNewEdgeFromId(src, dst_2, 1);
                        costMap[g.addArc(srcNode, lemon::SmartDigraph::nodeFromId(dst_2))] = 1;
                    }

                    size_t dst_3 = i*(M+1) + (t+1);
                    ///std::cout << src << "-[1]->"  << dst_3 << std::endl;
                    //printer.addNewEdgeFromId(src, dst_3, 1);
                    costMap[g.addArc(srcNode, lemon::SmartDigraph::nodeFromId(dst_3))] = 1;
                }
            }
        }
    }


    /*{
        printer.pruneUnreachableNodes();
        std::ofstream f{"file.dot"};
        printer.dot(f, false);
        f.close();
    }*/

    lemon::Dijkstra<lemon::SmartDigraph, lemon::SmartDigraph::ArcMap<int>> dij{g, costMap};
    dij.init();
    dij.addSource(lemon::SmartDigraph::nodeFromId(init));
    dij.start();

    auto& d = dij.distMap();
    return (double)d[lemon::SmartDigraph::nodeFromId(fini)];
}
