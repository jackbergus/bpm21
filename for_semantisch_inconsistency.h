//
// Created by giacomo on 24/03/21.
//

#ifndef BPM21_FOR_SEMANTISCH_INCONSISTENCY_H
#define BPM21_FOR_SEMANTISCH_INCONSISTENCY_H

#include <string>
#include <vector>
#include <graphs/FlexibleFA.h>
#include <alignment/result_semantisch_align.h>

/**
 * Compute the inconsistency measures by performing the best alignment towards a graph, rather than
 * trying to unfold it.
 */
struct for_semantisch_inconsistency {
    std::string epsilon;
    std::vector<FlexibleFA<std::string, size_t>> V;

    for_semantisch_inconsistency(const std::string& epsilon);

    void collect_traces_from_model_clause_as_graph(const FlexibleFA<size_t, std::string>& g);

    result_semantisch_align compute_in_triplicate(const std::vector<std::string>& log);
};


#endif //BPM21_FOR_SEMANTISCH_INCONSISTENCY_H