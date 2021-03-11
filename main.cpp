/*
 * main.cpp
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

 

#include <iostream>
#include <ltlf/DataPredicate.h>
#include <fstream>
#include <declare/DeclareModelParse.h>
#include <DADLexer.h>
#include <DADParser.h>

void test_data_predicate() {
    DataPredicate x_1{"x", LT, 0.2};
    DataPredicate x_2{"x", GEQ, -1.0};
    std::cout << x_1 << std::endl;
    std::cout << x_2 << std::endl;
    x_1.intersect_with(x_2);
    std::cout << x_1 << std::endl;

    DataPredicate x_3{"x", NEQ, 5.0};
    x_1.intersect_with(x_3);
    std::cout << x_1 << std::endl;

    DataPredicate x_4{"x", NEQ, 0.001};
    x_1.intersect_with(x_4);
    std::cout << x_1 << std::endl;
}

int main() {

    DeclareModelParse mp;
    std::ifstream stream ("test_file.txt");
    ltlf formula = mp.load_model_to_semantics(stream).nnf().simplify();

}
