/*
 * DeclareModelParse.h
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
// Created by giacomo on 09/03/21.
//

#ifndef BPM21_DECLAREMODELPARSE_H
#define BPM21_DECLAREMODELPARSE_H

namespace antlr4 {
    class ANTLRInputStream;
    class CommonTokenStream;
}
class DADLexer;
class DADParser;
#include <DADBaseVisitor.h>
#include <sstream>
#include "DeclareDataAware.h"
#include <utils/strings.h>
#include <magic_enum.hpp>
#include <fstream>

class DeclareModelParse : public DADBaseVisitor {
    //DADParser::Data_aware_declareContext *expr;
    std::string orig_for_move_or_copy;

public:
    ~DeclareModelParse() {
    }
    DeclareModelParse();

    std::vector<DeclareDataAware> load(std::ifstream& stream);

    antlrcpp::Any visitData_aware_declare(DADParser::Data_aware_declareContext *ctx) override {
        std::vector<DeclareDataAware> v;
        if (ctx) {
            for (DADParser::DeclareContext* ptr : ctx->declare()) {
                v.emplace_back(visit(ptr).as<DeclareDataAware>());
            }
        }
        return {v};
    }

    antlrcpp::Any visitNary_prop(DADParser::Nary_propContext *ctx) override {
        DeclareDataAware dda;
        if (ctx) {
            std::tie (dda.left_act, dda.dnf_left_map) =
                    visitFields(ctx->fields(0)).as<std::pair<std::string,
                            std::vector<std::unordered_map<std::string, DataPredicate>>>>();
            std::tie (dda.right_act, dda.dnf_right_map) =
                    visitFields(ctx->fields(1)).as<std::pair<std::string,
                            std::vector<std::unordered_map<std::string, DataPredicate>>>>();
            dda.n = 0;
            dda.casusu =
                    magic_enum::enum_cast<declare_templates>(ctx->LABEL()->getText()).value();
        }
        return {dda};
    }

    antlrcpp::Any visitUnary_prop(DADParser::Unary_propContext *ctx) override {
        DeclareDataAware dda;
        if (ctx) {
            std::tie (dda.left_act, dda.dnf_left_map) =
                    visitFields(ctx->fields()).as<std::pair<std::string,
                            std::vector<std::unordered_map<std::string, DataPredicate>>>>();
            dda.n = std::stoul(ctx->INTNUMBER()->getText());
            dda.casusu =
                    magic_enum::enum_cast<declare_templates>(ctx->LABEL()->getText()).value();
        }
        return {dda};
    }

    antlrcpp::Any visitFields(DADParser::FieldsContext *ctx) override {
        std::pair<std::string,
                std::vector<std::unordered_map<std::string, DataPredicate>>> cp;
        if (ctx) {
            cp.first = ctx->LABEL()->getText();
            cp.second = visit(ctx->prop()).as<std::vector<std::unordered_map<std::string, DataPredicate>>>();
        }
        return {cp};
    }

    antlrcpp::Any visitDisj(DADParser::DisjContext *ctx) override {
        std::vector<std::unordered_map<std::string, DataPredicate>> v;
        if (ctx) {
            std::unordered_map<std::string, DataPredicate> M = visit(ctx->prop_within_dijunction()).as<std::unordered_map<std::string, DataPredicate>>();
            v = visit(ctx->prop()).as<std::vector<std::unordered_map<std::string, DataPredicate>>>();
            v.emplace_back(M);
        }
        return {v};
    }

    antlrcpp::Any visitConj_or_atom(DADParser::Conj_or_atomContext *ctx) override {
        std::vector<std::unordered_map<std::string, DataPredicate>> v;
        if (ctx) {
            std::unordered_map<std::string, DataPredicate> M = visit(ctx->prop_within_dijunction()).as<std::unordered_map<std::string, DataPredicate>>();
            v.emplace_back(M);
        }
        return {v};
    }

    antlrcpp::Any visitTop(DADParser::TopContext *ctx) override {
        return {std::vector<std::unordered_map<std::string, DataPredicate>>{}};
    }

    antlrcpp::Any visitIn_atom(DADParser::In_atomContext *ctx) override {
        std::unordered_map<std::string, DataPredicate> v;
        if (ctx) {
            DataPredicate pred = visitAtom(ctx->atom()).as<DataPredicate>();
            v[pred.var] = pred;
        }
        return {v};
    }

    antlrcpp::Any visitAtom_conj(DADParser::Atom_conjContext *ctx) override {
        std::unordered_map<std::string, DataPredicate> v;
        if (ctx) {
            v = visit(ctx->prop_within_dijunction()).as<std::unordered_map<std::string, DataPredicate>>();
            DataPredicate baseCase = visitAtom(ctx->atom()).as<DataPredicate>();
            auto it = v.find(baseCase.var);
            if (it != v.end())
                it->second.intersect_with(baseCase);
            else
                v[baseCase.var] = baseCase;
        }
        return {v};
    }

    antlrcpp::Any visitAtom(DADParser::AtomContext *ctx) override {
        if (ctx) {
            DataPredicate pred;
            pred.var = ctx->VAR()->getText();
            pred.casusu = visit(ctx->rel()).as<numeric_atom_cases>();
            if (ctx->STRING()) {
                pred.value = UNESCAPE(ctx->STRING()->getText());
            } else if (ctx->NUMBER()) {
                pred.value = std::stod(ctx->NUMBER()->getText());
            } else {
                pred.value = 0.0;
            }
            return {pred};
        }
        return {};
    }

    antlrcpp::Any visitLt(DADParser::LtContext *ctx) override {
        return {LT};
    }

    antlrcpp::Any visitLeq(DADParser::LeqContext *ctx) override {
        return {LEQ};
    }

    antlrcpp::Any visitGt(DADParser::GtContext *ctx) override {
        return {GT};
    }

    antlrcpp::Any visitGeq(DADParser::GeqContext *ctx) override {
        return {GEQ};
    }

    antlrcpp::Any visitEq(DADParser::EqContext *ctx) override {
        return {EQ};
    }

    antlrcpp::Any visitNeq(DADParser::NeqContext *ctx) override {
        return {NEQ};
    }

};


#endif //BPM21_DECLAREMODELPARSE_H
