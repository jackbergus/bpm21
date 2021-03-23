/*
 * ParseFFLOATDot.h
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

#ifndef BPM21_PARSEFFLOATDOT_H
#define BPM21_PARSEFFLOATDOT_H

namespace antlr4 {
    class ANTLRInputStream;
    class CommonTokenStream;
}
class DADLexer;
class DADParser;
#include <DOTBaseVisitor.h>
#include <sstream>
#include <utils/strings.h>
#include <magic_enum.hpp>
#include <fstream>
#include <graphs/NodeLabelBijectionFA.h>
#include <graphs/FlexibleFA.h>
#include <ltlf/ltlf.h>

class ParseFFLOATDot : public DOTVisitor {

    NodeLabelBijectionFA<std::string, ltlf> parsing_result;
public:

    bool need_back_conversion;
    const std::unordered_map<std::string, std::string>* back_conv;

    ParseFFLOATDot() : back_conv{nullptr}, need_back_conversion{false} {}
    ~ParseFFLOATDot()  {}

    FlexibleFA<size_t, std::string> parse(std::ifstream& stream,
                                          const std::unordered_set<std::string>& SigmaAll);


    antlrcpp::Any visitGraph(DOTParser::GraphContext *context) override {
        throw std::runtime_error("Unexpected invocation: visitGraph");
    }
    antlrcpp::Any visitStmt_list(DOTParser::Stmt_listContext *context) override {
        throw std::runtime_error("Unexpected invocation: visitStmt_list");
    }
    antlrcpp::Any visitStmt(DOTParser::StmtContext *context) override {
        if (context) {
            if (context->node_stmt()) {
                return visitNode_stmt(context->node_stmt());
            } else if (context->edge_stmt()) {
                return visitEdge_stmt(context->edge_stmt());
            }
        }
        return {};
    }
    antlrcpp::Any visitAttr_stmt(DOTParser::Attr_stmtContext *context) override {
        throw std::runtime_error("Unexpected invocation: visitAttr_stmt");
    }
    antlrcpp::Any visitAttr_list(DOTParser::Attr_listContext *context) override {
        throw std::runtime_error("Unexpected invocation: visitAttr_list");
    }
    antlrcpp::Any visitA_list(DOTParser::A_listContext *context) override {
        throw std::runtime_error("Unexpected invocation: visitA_list");
    }
    antlrcpp::Any visitEdge_stmt(DOTParser::Edge_stmtContext *context) override;
    antlrcpp::Any visitEdgeRHS(DOTParser::EdgeRHSContext *context) override {
        throw std::runtime_error("Unexpected invocation: visitEdgeRHS");

    }
    antlrcpp::Any visitEdgeop(DOTParser::EdgeopContext *context) override {
        throw std::runtime_error("Unexpected invocation: visitEdgeop");

    }
    antlrcpp::Any visitNode_stmt(DOTParser::Node_stmtContext *context) override {
        if (context) {
            std::string id = context->node_id()->getText();
            if (id == "fake") return {};
            int idV = parsing_result.addUniqueStateOrGetExisting(id);
            if (context->attr_list())
            for (const auto& ls : context->attr_list()->a_list()) {
                auto v = ls->id();
                for (size_t i = 0, N = v.size()/2; i<N; i++) {
                    size_t offset_attr = i*2;
                    size_t offset_val = i*2+1;
                    std::string key = v.at(offset_attr)->getText();
                    std::string val = v.at(offset_val)->getText();
                    if ((key == "root") && (val == "true")) {
                        parsing_result.addToInitialNodesFromId(idV);
                    } else if ((key == "shape") && (val == "doublecircle")) {
                        parsing_result.addToFinalNodesFromId(idV);
                    }
                }
            }
        }
        return {};
    }
    antlrcpp::Any visitNode_id(DOTParser::Node_idContext *context) override {
        throw std::runtime_error("Unexpected invocation: visitNode_id");
    }
    antlrcpp::Any visitPort(DOTParser::PortContext *context) override {
        throw std::runtime_error("Unexpected invocation: visitPort");
    }
    antlrcpp::Any visitSubgraph(DOTParser::SubgraphContext *context) override {
        throw std::runtime_error("Unexpected invocation: visitSubgraph");
    }
    antlrcpp::Any visitId(DOTParser::IdContext *context) override {
        throw std::runtime_error("Unexpected invocation: visitId");
    }

};


FlexibleFA<size_t, std::string> getFAFromLTLFEdges(const std::unordered_set<std::string> &SigmaAll,
                                                   const NodeLabelBijectionFA<std::string, ltlf> &parsing_result2) ;

#endif //BPM21_PARSEFFLOATDOT_H
