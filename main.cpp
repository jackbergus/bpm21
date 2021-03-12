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

#if 1
    /*{
        input_pipeline Pip{"fa"};
        Pip.run_pipeline("ex_3.txt");
        {
            std::ofstream f{"eq_classes_3.txt"};
            Pip.print_equivalence_classes(f);
        }

    }*/
    {
        input_pipeline Pip{"fa"};
        Pip.run_pipeline("ex_1.txt");
        {
            std::ofstream f{"eq_classes_1.txt"};
            Pip.print_equivalence_classes(f);
        }
        {
            std::ofstream f{"log_1_out.txt"};
            Pip.print_atomized_traces("log_1.txt", f);
        }
    }
    {
        input_pipeline Pip{"fa"};
        Pip.run_pipeline("ex_2.txt");
        {
            std::ofstream f{"eq_classes_2.txt"};
            Pip.print_equivalence_classes(f);
        }
        {
            std::ofstream f{"log_2_out.txt"};
            Pip.print_atomized_traces("log_2.txt", f);
        }
    }


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
