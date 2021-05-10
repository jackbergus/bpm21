//
// Created by giacomo on 24/03/21.
//

#include <alignment/alignment_utils.h>
#include "for_semantisch_inconsistency.h"

for_semantisch_inconsistency::for_semantisch_inconsistency(const std::string &epsilon) : epsilon{epsilon} {}

void for_semantisch_inconsistency::collect_traces_from_model_clause_as_graph(const FlexibleFA<size_t, std::string> &g) {
    V.emplace_back(g.shiftLabelsToNodes());
}

result_semantisch_align for_semantisch_inconsistency::compute_in_triplicate(const std::vector<std::string> &log) {
    if (log.empty()) return {0.0, 0.0, 0.0};
    double result_sigma = std::numeric_limits<double>::max();
    double result_max = std::numeric_limits<double>::max();
    double result_hit = std::numeric_limits<double>::max();
    for (const std::string &trace : log) {
        double cost_sigma = 0.0;
        double cost_max = -std::numeric_limits<double>::max();
        double cost_hit = 0.0;
        size_t i = 1;
        for (const auto &graph : V) {
            double alignment = align(graph, trace, epsilon);
            /*if (alignment> 0.0) {
                std::ofstream file{"f.dot"};
                graph.dot(file, false);
                file.close();
                std::cerr << trace << " " << i << std::endl;
                exit(1);
            }*/
            i++;
            cost_sigma += alignment;
            cost_max = std::max(alignment, cost_max);
            if (alignment > 0.0) cost_hit++;
        }
        result_sigma = std::min(result_sigma, cost_sigma);
        result_max = std::min(result_max, cost_max);
        result_hit = std::min(result_max, cost_hit);
    }
    return {result_sigma, result_max, result_hit};
}

void for_semantisch_inconsistency::collect_clauses_from_fsi(const for_semantisch_inconsistency &ref) {
    V.insert(V.end(), ref.V.begin(), ref.V.end());
}
