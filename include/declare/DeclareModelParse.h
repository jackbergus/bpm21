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

class DeclareModelParse : public DADBaseVisitor {
    const char empty[0] = {};
    antlr4::ANTLRInputStream stream;
    std::istringstream sstr;
    DADParser* parser;
    DADLexer*  lexer;

    antlr4::CommonTokenStream* tokens;
    DADParser::Data_aware_declareContext *expr;
    std::string orig_for_move_or_copy;

    void allocateAnew();
public:
    DeclareModelParse();
    DeclareModelParse(const std::string& expr);

};


#endif //BPM21_DECLAREMODELPARSE_H
