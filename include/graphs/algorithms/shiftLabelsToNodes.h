/*
 * shiftLabelsToNodes.h
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

#ifndef CLASSIFIERS_SHIFTLABELSTONODES_H
#define CLASSIFIERS_SHIFTLABELSTONODES_H

#include <vector>
#include <unordered_map>
#include <graphs/FlexibleFA.h>
#include <graphs/NodeLabelBijectionFA.h>

template <typename NodeElement, typename EdgeLabel>
FlexibleFA<EdgeLabel, NodeElement> shiftLabelsToNodes(const NodeLabelBijectionFA<NodeElement, EdgeLabel>& graph) {
    FlexibleFA<EdgeLabel, NodeElement> result;
    std::unordered_map<size_t , size_t> edgeToNewNodeMap;
    std::unordered_map<size_t, std::vector<size_t>> outgoingEdges;

    for (size_t arcId = 0, N = graph.maximumEdgeId(); arcId<N; arcId++) {
        std::pair<size_t,size_t> arc = graph.g.edge_from_id(arcId);
        int arcSrc = arc.first;
        outgoingEdges[arcSrc].emplace_back(arcId);
        int arcIdAsNode = result.addNewNodeWithLabel(graph.getEdgeLabel(arcId));
        edgeToNewNodeMap.emplace(arcId, arcIdAsNode);

        if (graph.init().contains(arcSrc))
            result.addToInitialNodesFromId(arcIdAsNode);
        if (graph.fini().contains(arc.second))
            result.addToFinalNodesFromId(arcIdAsNode);
    }
    for (size_t arcId = 0, N = graph.maximumEdgeId(); arcId<N; arcId++) {
        std::pair<size_t,size_t> arc = graph.g.edge_from_id(arcId);
        int src = edgeToNewNodeMap.at(arcId);
        int origArcDst = arc.second;
        auto it = outgoingEdges.find(origArcDst);
        if (it != outgoingEdges.end()) {
            auto dstLabel = graph.getUniqueLabel(origArcDst);
            for (int arc2 : it->second) {
                result.addNewEdgeFromId(src, edgeToNewNodeMap.at(arc2), dstLabel);
            }
        }
    }
    return result;
}

#endif //CLASSIFIERS_SHIFTLABELSTONODES_H
