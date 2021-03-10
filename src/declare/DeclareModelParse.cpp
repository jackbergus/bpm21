/*
 * DeclareModelParse.cpp
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

#include "declare/DeclareModelParse.h"
#include <DADLexer.h>

DeclareModelParse::DeclareModelParse() {}

std::vector<DeclareDataAware> DeclareModelParse::load(std::ifstream& stream) {
    antlr4::ANTLRInputStream input(stream);
    DADLexer lexer(&input);
    antlr4::CommonTokenStream tokens(&lexer);
    DADParser parser(&tokens);
    return visit(parser.data_aware_declare()).as<std::vector<DeclareDataAware>>();
}
