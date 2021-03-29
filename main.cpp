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
#include <DADParser.h>
#include <pipeline/input_pipeline.h>

#include "declare/DataTraceParse.h"
#include "graphs/third-party-wrappers/ParseFFLOATDot.h"
#include "pipeline/foreign_data_loads.h"

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

#include <utils/stream_out.h>
#include <declare/DeclareDataAware.h>

std::ostream &operator<<(std::ostream &os, const std::variant<std::string, double> &insertion) {
    if (std::holds_alternative<std::string>(insertion))
        return os << std::get<std::string>(insertion);
    else
        return os << std::get<double >(insertion);
}


void pipeline(size_t n, size_t mod, const std::vector<size_t>& lengths) {

    std::unordered_set<size_t> to_test_correct;
    std::set<std::vector<std::string>> relevant_traces;
    std::vector<std::vector<std::vector<std::string>>> M;

    std::string N =  std::to_string(n);
    std::string converted_file = "/media/giacomo/Data/bz/CLionProjects/bpm21/data/converted/"+N+"constr";
    std::vector<std::string> LOGS;

    for (size_t i = 0; i<=mod; i++) {
        std::string I = std::to_string(i);
        for (size_t single_len : lengths) {
            if (i == 0)
                to_test_correct.insert(LOGS.size());
            LOGS.emplace_back("/media/giacomo/Data/bz/CLionProjects/bpm21/data/curr/"+N+" CONSTRAINTS/"+I+"_mod/length_"+std::to_string(single_len)+".xes");
        }
    }

    input_pipeline Pip{"p"};
    Pip.run_pipeline(converted_file + ".txt", true);
    std::unordered_set<std::string> SigmaAll;
    {
        std::ofstream f{converted_file+"_eq_classes.txt"};
        Pip.print_equivalence_classes(f);
    }
    //
    {
        std::ofstream f{converted_file+"_sigma.txt"};
        Pip.print_sigma(f);
    }
    {
        size_t i = 0;
        for (const std::string& elements : LOGS) {
            std::cout << "\t\t- log dumping: " << elements << std::endl;
            auto log = Pip.print_atomized_traces(elements, elements+"_atomized_1", SigmaAll, true);
            if (to_test_correct.contains(i)) {
                std::cout << "\t\t\t adding relevant traces" << elements << std::endl;
                M.emplace_back(log);
            }
            i++;
        }
    }
    {
        std::ofstream f{converted_file+"_graph.dot"};
        std::string single_line{converted_file+"_single_line_clause.txt"};
        auto g= Pip.decompose_genmodel_for_tiny_graphs(SigmaAll, single_line, false);
        g.dot(f, false);
        f.flush(); f.close();
        for (const std::vector<std::vector<std::string>>& log : M) {
            size_t j = 1;
            for (const std::vector<std::string>& trace : log) {
                std::cout << trace.size() << " testing " << j << "..." << std::endl;
                g.test_correctness(trace);
                j++;
            }
        }
    }
}


void pipeline_all() {
    {
        std::vector<size_t> lengths = {10,15,20,25,30};
        pipeline(3, 0, lengths);
        pipeline(5, 0, lengths);
    }
    {
        std::vector<size_t> lengths = {15,20,25,30};
        pipeline(7, 0, lengths);
    }
    {
        std::vector<size_t> lengths = {20,25,30};
        pipeline(10, 0, lengths);
    }
}

int main() {
   pipeline_all();
   return 0;
}
