/*
 * ParseFFLOATDot.cpp
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
// Created by giacomo on 13/03/21.
//

#include "graphs/third-party-wrappers/ParseFFLOATDot.h"
#include "graphs/third-party-wrappers/FLLOATSimplePropParser.h"
#include <DOTLexer.h>

FlexibleFA<size_t, std::string> ParseFFLOATDot::parse(std::ifstream& stream, const std::unordered_set<std::string>& SigmaAll) {
    antlr4::ANTLRInputStream input(stream);
    DOTLexer lexer(&input);
    antlr4::CommonTokenStream tokens(&lexer);
    tokens.fill();
    DOTParser parser(&tokens);
    for (const auto& ptr :
            parser.graph()->stmt_list()->stmt()) {
        visitStmt(ptr);
    }

    FlexibleFA<size_t, std::string> result;
    std::unordered_map<size_t, size_t> idConv;
    for (size_t nodeId = 0, N = parsing_result.maximumNodeId(); nodeId<N; nodeId++) {
            size_t src = result.addNewNodeWithLabel(nodeId);
        idConv[nodeId] = src;
    }
    for (size_t nodeId = 0, N = parsing_result.maximumNodeId(); nodeId<N; nodeId++) {
        size_t src = idConv.at(nodeId);
        for (const auto&edge : parsing_result.outgoingEdges(nodeId)) {
            size_t dst = idConv.at(edge.second);
            for (const std::string& act : SigmaAll) {
                if (edge.first.easy_interpret(act)) {
                    result.addNewEdgeFromId(src, dst, act);
                }
            }
        }
    }
    return result;
}

#include <sstream>

antlrcpp::Any ParseFFLOATDot::visitEdge_stmt(DOTParser::Edge_stmtContext *context) {
    if (context) {
        std::string src = context->node_id()->getText();
        if (src == "fake") return {};
        std::string dst = context->edgeRHS()->node_id(0)->getText();
        if (dst == "fake") return {};
        int srcId = parsing_result.getId(src);
        int dstId = parsing_result.getId(dst);
        for (const auto& ls : context->attr_list()->a_list()) {
            auto v = ls->id();
            for (size_t i = 0, N = v.size()/2; i<N; i++) {
                size_t offset_attr = i*2;
                size_t offset_val = i*2+1;
                std::string key = v.at(offset_attr)->getText();
                std::string val = v.at(offset_val)->getText();
                if ((key == "label")) {
                    try {
                        val = UNESCAPE(val);
                    } catch(...) {
                        //
                    }
                    FLLOATSimplePropParser parser;
                    std::stringstream ss;
                    ss.str(val);
                    parsing_result.addNewEdgeFromId(srcId, dstId, parser.parse(ss));
                }
            }
        }
    }
    return {};
}
