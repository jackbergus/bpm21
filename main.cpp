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

void test() {
    {
        input_pipeline Pip{"fa"};
        Pip.run_pipeline("ex_1.txt", true);
        std::unordered_set<std::string> SigmaAll;
        {
            std::ofstream f{"eq_classes_1.txt"};
            Pip.print_equivalence_classes(f);
        }
        {
            Pip.print_atomized_traces("log_1.txt", "log_atomized_1", SigmaAll, true);
        }
        {
            std::ofstream f{"ex_1_sigma.txt"};
            Pip.print_sigma(f);
        }
        {
            std::ofstream f{"graph_1.dot"};
            std::string single_line{"single_line_clause_1.txt"};
            Pip.decompose_genmodel_for_tiny_graphs(SigmaAll, single_line, false).dot(f, false);
            f.flush(); f.close();
        }
    }
    {
        input_pipeline Pip{"fa"};
        Pip.run_pipeline("ex_2.txt", true);
        std::unordered_set<std::string> SigmaAll;
        {
            std::ofstream f{"eq_classes_2.txt"};
            Pip.print_equivalence_classes(f);
        }
        {
            Pip.print_atomized_traces("log_2.txt", "log_atomized_2", SigmaAll, true);
        }
        {
            std::ofstream f{"ex_2_sigma.txt"};
            Pip.print_sigma(f);
        }
        {
            std::ofstream f{"graph_2.dot"};
            std::string single_line{"single_line_clause_2.txt"};
            Pip.decompose_genmodel_for_tiny_graphs(SigmaAll, single_line, false).dot(f, false);
            f.flush(); f.close();
        }
    }
}

void pipeline(size_t n, size_t mod, const std::vector<size_t>& lengths) {
    std::string N =  std::to_string(n);
    std::string converted_file = "/media/giacomo/Data/bz/CLionProjects/bpm21/data/converted/"+N+"constr";
    std::vector<std::string> LOGS;
#if 0

    LOGS.emplace_back("/media/giacomo/Data/bz/CLionProjects/bpm21/data/curr/"+N+" CONSTRAINTS/3_mod/length_10.xes");
#else
    for (size_t i = 0; i<=mod; i++) {
        std::string I = std::to_string(i);
        for (size_t single_len : lengths) {
            LOGS.emplace_back("/media/giacomo/Data/bz/CLionProjects/bpm21/data/curr/"+N+" CONSTRAINTS/"+I+"_mod/length_"+std::to_string(single_len)+".xes");
        }
    }
#endif

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
        for (const std::string& elements : LOGS) {
            std::cout << "\t\t- log dumping: " << elements << std::endl;
            Pip.print_atomized_traces(elements, elements+"_atomized_1", SigmaAll, true);
        }
    }
    {
        std::ofstream f{converted_file+"_graph.dot"};
        std::string single_line{converted_file+"_single_line_clause.txt"};
        Pip.decompose_genmodel_for_tiny_graphs(SigmaAll, single_line, false).dot(f, false);
        f.flush(); f.close();
    }
}

void romano() {
    input_pipeline Pip{"p"};
    Pip.run_pipeline( "/media/giacomo/Data/bz/CLionProjects/bpm21/data/additional/BPIC15_1.sdecl", false);
    {
        std::ofstream f{"/media/giacomo/Data/bz/CLionProjects/bpm21/data/additional/BPIC15_1.dot"};
        std::string single_line{"/media/giacomo/Data/bz/CLionProjects/bpm21/data/additional/BPIC15_1_single_line_clause_1.txt"};
        Pip.decompose_genmodel_for_tiny_graphs(Pip.atom_universe, single_line, true).dot(f, false);
        f.flush(); f.close();
    }
}

void romano2() {
    for (size_t i = 5; i!=0; i--) {
        std::cout << "~~" << i << "~~~~~~~~~~~~~~~" << std::endl;
        input_pipeline Pip{"p"};
        std::string I = std::to_string(i);
        auto S = Pip.atom_universe;
        S.insert("*");
        Pip.run_pipeline( "/media/giacomo/Data/bz/CLionProjects/bpm21/data/additional/BPIC15_"+I+".sdecl", false);
        {
            std::ofstream f{"/media/giacomo/Data/bz/CLionProjects/bpm21/data/additional/BPIC15_"+I+".dot"};
            std::string single_line{"/media/giacomo/Data/bz/CLionProjects/bpm21/data/additional/BPIC15_"+I+"_single_line_clause_1.txt"};
            Pip.decompose_genmodel_for_tiny_graphs(S, single_line, true).dot(f, false);
            f.flush(); f.close();
        }
    }

}

void old() {





#if 1
    /*{
        input_pipeline Pip{"fa"};
        Pip.run_pipeline("ex_3.txt");
        {
            std::ofstream f{"eq_classes_3.txt"};
            Pip.print_equivalence_classes(f);
        }

    }*/
#if 1
    {
        input_pipeline Pip{"fa"};
        Pip.run_pipeline("ex_3.txt", true);
        std::unordered_set<std::string> SigmaAll;
        {
            std::ofstream f{"eq_classes_3.txt"};
            Pip.print_equivalence_classes(f);
        }
        {
            Pip.print_atomized_traces("log_1.txt", "log_atomized_1", SigmaAll, true);
        }
        {
            std::ofstream f{"graph_1.dot"};
            std::string single_line{"single_line_clause_1.txt"};
            Pip.decompose_genmodel_for_tiny_graphs(SigmaAll, single_line, false).dot(f, false);
            f.flush(); f.close();
        }
    }
    /*{
        input_pipeline Pip{"fa"};
        Pip.run_pipeline("ex_1.txt");
        std::unordered_set<std::string> SigmaAll;
        {
            std::ofstream f{"eq_classes_1.txt"};
            Pip.print_equivalence_classes(f);
        }
        {
            Pip.print_atomized_traces("log_1.txt", "log_atomized_1", SigmaAll, true);
        }
        {
            std::ofstream f{"graph_1.dot"};
            std::string single_line{"single_line_clause_1.txt"};
            Pip.decompose_genmodel_for_tiny_graphs(SigmaAll, single_line, false).dot(f, false);
            f.flush(); f.close();
        }
    }
    {
        input_pipeline Pip{"fa"};
        Pip.run_pipeline("ex_2.txt");
        std::unordered_set<std::string> SigmaAll;
        {
            std::ofstream f{"eq_classes_2.txt"};
            Pip.print_equivalence_classes(f);
        }
        {
            Pip.print_atomized_traces("log_2.txt", "log_atomized_2", SigmaAll, true);
        }
        {
            std::ofstream f{"graph_2.dot"};
            std::string single_line{"single_line_clause_2.txt"};
            Pip.decompose_genmodel_for_tiny_graphs(SigmaAll, single_line, false).dot(f, false);
            f.flush(); f.close();
        }
    }*/
#endif

/*
    std::ifstream f{"/media/giacomo/Data/bz/CLionProjects/bpm21/single_line_clause_1.txt_graph_1"};
    ParseFFLOATDot test;
    auto parsing_result = test.parse(f, {"fa1", "fa2", "fa3", "a", "b"});
    std::ofstream g1{"back.dot"};
    parsing_result.dot(g1, false);
    g1.flush(); g1.close();*/

    /*FLLOATScriptRunner psr;
    psr.process_expression("/media/giacomo/Data/bz/CLionProjects/bpm21/single_line_clause_1.txt");
*/

#else
    DataTraceParse dtp;
    std::ifstream file{"log_test.txt"};

    for (const auto& trace : dtp.load(file))
        std::cout << trace <<std::endl;
    /*struct IntPrevNext  {
    size_t getPrev(size_t elem) const {
        return (elem == 0) ? elem : (elem-1);
    }
    size_t getNext(size_t elem) const {
        return (elem == std::numeric_limits<size_t>::max()) ? elem : elem+1;
    }
    size_t distance(size_t l, size_t r) const {
        return std::labs(l-r);
    }
};

segment_partition_tree<size_t, IntPrevNext> S(0, 10);
    insert_interval(S.indexer, S.element, 0UL, 10UL);
    insert_interval(S.indexer, S.element, 0UL, 4UL);
    insert_interval(S.indexer, S.element, 5UL, 5UL);
    insert_interval(S.indexer, S.element, 6UL, 10UL);
    minimize_tree(S.indexer, S.element);
    std::cout << S << std::endl;*/
#endif
}

void pipeline_all() {
    {
        std::vector<size_t> lengths = {10,15,20,25,30};
        pipeline(3, 3, lengths);
        pipeline(5, 3, lengths);
    }
    {
        std::vector<size_t> lengths = {15,20,25,30};
        pipeline(7, 3, lengths);
    }
    {
        std::vector<size_t> lengths = {20,25,30};
        pipeline(10, 3, lengths);
    }
}

void testing2() {
    input_pipeline Pip{"p"};
    Pip.run_pipeline( "/media/giacomo/Data/bz/CLionProjects/bpm21/data/testing.txt", false);
    {
        std::ofstream f{"/media/giacomo/Data/bz/CLionProjects/bpm21/data/testing.dot"};
        std::string single_line{"/media/giacomo/Data/bz/CLionProjects/bpm21/data/testing_single_line_clause_1.txt"};
        auto cp = Pip.atom_universe;
        Pip.decompose_genmodel_for_tiny_graphs(cp, single_line, false).dot(f, false);
        f.flush(); f.close();
    }
}


void testing() {
    input_pipeline Pip{"p"};
    Pip.run_pipeline( "/media/giacomo/Data/bz/CLionProjects/bpm21/data/testing.sdecl", false);
    {
        std::ofstream f{"/media/giacomo/Data/bz/CLionProjects/bpm21/data/testing.dot"};
        std::string single_line{"/media/giacomo/Data/bz/CLionProjects/bpm21/data/testing_single_line_clause_1.txt"};
        auto cp = Pip.atom_universe;
        Pip.decompose_genmodel_for_tiny_graphs(cp, single_line, true).dot(f, false);
        f.flush(); f.close();
    }
}

int main() {
    //pipeline_all();
    //romano2();
    // test();
    testing2();
}
