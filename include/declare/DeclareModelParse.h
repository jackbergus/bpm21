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
    bool do_renaming;
public:
    ~DeclareModelParse() {}
    DeclareModelParse();

    /**
     * Parses a file, and loads the model as a vector of clauses
     * @param stream
     * @return
     */
    std::vector<DeclareDataAware> load(std::ifstream &stream, bool do_xes_renaming);

    ltlf load_model_to_semantics(std::ifstream &stream, bool do_xes_renaming, bool is_simplified_xes);

    ////////////////////////////
    //// Parsing functions ////
    ///////////////////////////
    antlrcpp::Any visitData_aware_declare(DADParser::Data_aware_declareContext *ctx) override;
    antlrcpp::Any visitNary_prop(DADParser::Nary_propContext *ctx) override;
    antlrcpp::Any visitUnary_prop(DADParser::Unary_propContext *ctx) override;
    antlrcpp::Any visitFields(DADParser::FieldsContext *ctx) override;
    antlrcpp::Any visitDisj(DADParser::DisjContext *ctx) override;
    antlrcpp::Any visitConj_or_atom(DADParser::Conj_or_atomContext *ctx) override;
    antlrcpp::Any visitTop(DADParser::TopContext *ctx) override;
    antlrcpp::Any visitIn_atom(DADParser::In_atomContext *ctx) override;
    antlrcpp::Any visitAtom_conj(DADParser::Atom_conjContext *ctx) override;
    antlrcpp::Any visitAtom(DADParser::AtomContext *ctx) override;
    antlrcpp::Any visitLt(DADParser::LtContext *ctx) override;
    antlrcpp::Any visitLeq(DADParser::LeqContext *ctx) override;
    antlrcpp::Any visitGt(DADParser::GtContext *ctx) override;
    antlrcpp::Any visitGeq(DADParser::GeqContext *ctx) override;
    antlrcpp::Any visitEq(DADParser::EqContext *ctx) override;
    antlrcpp::Any visitNeq(DADParser::NeqContext *ctx) override;
};


#endif //BPM21_DECLAREMODELPARSE_H
