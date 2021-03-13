/*
 * DataTraceParse.cpp
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

#include "declare/DataTraceParse.h"
#include <TracesLexer.h>
#include <TracesParser.h>

std::vector<std::vector<std::pair<std::string, std::unordered_map<std::string, std::variant<std::string, double>>>>> DataTraceParse::load(std::ifstream &stream) {
    antlr4::ANTLRInputStream input(stream);
    TracesLexer lexer(&input);
    antlr4::CommonTokenStream tokens(&lexer);
    TracesParser parser(&tokens);
    return visit(parser.log()).as<std::vector<std::vector<std::pair<std::string, std::unordered_map<std::string, std::variant<std::string, double>>>>>>();
}

antlrcpp::Any DataTraceParse::visitLog(TracesParser::LogContext *ctx) {
    std::vector<std::vector<std::pair<std::string, std::unordered_map<std::string, std::variant<std::string, double>>>>> log;
    if (ctx) {
        for (const auto& ptr : ctx->trace()) {
            auto it = visitTrace(ptr);
            if (it.isNotNull()) {
                log.emplace_back(it.as<std::vector<std::pair<std::string, std::unordered_map<std::string, std::variant<std::string, double>>>>>());
            }
        }
    }
    return {log};
}

antlrcpp::Any DataTraceParse::visitTrace(TracesParser::TraceContext *ctx) {
    std::vector<std::pair<std::string, std::unordered_map<std::string, std::variant<std::string, double>>>> trace;
    if (ctx) {
        for (const auto& ptr : ctx->event()) {
            auto it = visitEvent(ptr);
            if (it.isNotNull()) {
                trace.emplace_back(it.as<std::pair<std::string, std::unordered_map<std::string, std::variant<std::string, double>>>>());
            }
        }
    }
    return {trace};
}

antlrcpp::Any DataTraceParse::visitEvent(TracesParser::EventContext *ctx) {
    if (ctx) {
        std::pair<std::string, std::unordered_map<std::string, std::variant<std::string, double>>> event;
        event.first = ctx->LABEL()->getText();
        std::transform(event.first.begin(), event.first.end(), event.first.begin(), ::tolower);
        event.second = visitData_part(ctx->data_part()).as<std::unordered_map<std::string, std::variant<std::string, double>>>();
        return {event};
    }
    return {};
}

antlrcpp::Any DataTraceParse::visitData_part(TracesParser::Data_partContext *ctx) {
    std::unordered_map<std::string, std::variant<std::string, double>> map;
    if (ctx) {
        for (const auto& ptr : ctx->field()) {
            auto it = visitField(ptr).as<std::unordered_map<std::string, std::variant<std::string, double>>>();
            map.insert(it.begin(), it.end());
        }
    }
    return {map};
}

antlrcpp::Any DataTraceParse::visitField(TracesParser::FieldContext *ctx) {
    std::unordered_map<std::string, std::variant<std::string, double>> map;
    if (ctx) {
        if (ctx->NUMBER()) {
            map[ctx->VAR()->getText()] = std::stod(ctx->NUMBER()->getText());
        } else {
            assert(ctx->STRING());
            map[ctx->VAR()->getText()] = UNESCAPE(ctx->STRING()->getText());
        }
    }
    return {map};
}
