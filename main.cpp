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
std::ostream &operator<<(std::ostream &os, const std::variant<std::string, double> &insertion) {
    if (std::holds_alternative<std::string>(insertion))
        return os << std::get<std::string>(insertion);
    else
        return os << std::get<double >(insertion);
}

int main() {

    std::string converted_file = "/media/giacomo/Data/bz/CLionProjects/bpm21/data/converted/3constr";
    std::vector<std::string> LOGS = {
            "/media/giacomo/Data/bz/CLionProjects/bpm21/data/curr/3 CONSTRAINTS/0_mod/length_10.xes",
            "/media/giacomo/Data/bz/CLionProjects/bpm21/data/curr/3 CONSTRAINTS/0_mod/length_15.xes",
            "/media/giacomo/Data/bz/CLionProjects/bpm21/data/curr/3 CONSTRAINTS/0_mod/length_20.xes",
            "/media/giacomo/Data/bz/CLionProjects/bpm21/data/curr/3 CONSTRAINTS/0_mod/length_25.xes",
            "/media/giacomo/Data/bz/CLionProjects/bpm21/data/curr/3 CONSTRAINTS/0_mod/length_30.xes",
            "/media/giacomo/Data/bz/CLionProjects/bpm21/data/curr/3 CONSTRAINTS/1_mod/length_10.xes",
            "/media/giacomo/Data/bz/CLionProjects/bpm21/data/curr/3 CONSTRAINTS/1_mod/length_15.xes",
            "/media/giacomo/Data/bz/CLionProjects/bpm21/data/curr/3 CONSTRAINTS/1_mod/length_20.xes",
            "/media/giacomo/Data/bz/CLionProjects/bpm21/data/curr/3 CONSTRAINTS/1_mod/length_25.xes",
            "/media/giacomo/Data/bz/CLionProjects/bpm21/data/curr/3 CONSTRAINTS/1_mod/length_30.xes",
            "/media/giacomo/Data/bz/CLionProjects/bpm21/data/curr/3 CONSTRAINTS/2_mod/length_10.xes",
            "/media/giacomo/Data/bz/CLionProjects/bpm21/data/curr/3 CONSTRAINTS/2_mod/length_15.xes",
            "/media/giacomo/Data/bz/CLionProjects/bpm21/data/curr/3 CONSTRAINTS/2_mod/length_20.xes",
            "/media/giacomo/Data/bz/CLionProjects/bpm21/data/curr/3 CONSTRAINTS/2_mod/length_25.xes",
            "/media/giacomo/Data/bz/CLionProjects/bpm21/data/curr/3 CONSTRAINTS/2_mod/length_30.xes",
            "/media/giacomo/Data/bz/CLionProjects/bpm21/data/curr/3 CONSTRAINTS/3_mod/length_10.xes",
            "/media/giacomo/Data/bz/CLionProjects/bpm21/data/curr/3 CONSTRAINTS/3_mod/length_15.xes",
            "/media/giacomo/Data/bz/CLionProjects/bpm21/data/curr/3 CONSTRAINTS/3_mod/length_20.xes",
            "/media/giacomo/Data/bz/CLionProjects/bpm21/data/curr/3 CONSTRAINTS/3_mod/length_25.xes",
            "/media/giacomo/Data/bz/CLionProjects/bpm21/data/curr/3 CONSTRAINTS/3_mod/length_30.xes"
    };

    input_pipeline Pip{"p"};
    Pip.run_pipeline(converted_file+".txt");
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
    /*{
        std::ofstream f{"/media/giacomo/Data/bz/CLionProjects/bpm21/data/converted/3constr_graph.dot"};
        std::string single_line{"/media/giacomo/Data/bz/CLionProjects/bpm21/data/converted/3constr_single_line_clause.txt"};
        Pip.decompose_genmodel_for_tiny_graphs(SigmaAll, single_line).dot(f, false);
        f.flush(); f.close();
    }*/
    exit(1);

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
        Pip.run_pipeline("ex_3.txt");
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
            Pip.decompose_genmodel_for_tiny_graphs(SigmaAll, single_line).dot(f, false);
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
            Pip.decompose_genmodel_for_tiny_graphs(SigmaAll, single_line).dot(f, false);
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
            Pip.decompose_genmodel_for_tiny_graphs(SigmaAll, single_line).dot(f, false);
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
