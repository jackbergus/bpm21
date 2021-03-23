/*
 * FormulaToWestergaardFA.h
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
// Created by giacomo on 12/03/21.
//

#ifndef CLASSIFIERS_FORMULATOWESTERGAARDFA_H
#define CLASSIFIERS_FORMULATOWESTERGAARDFA_H


#include <ltlf/ltlf.h>
#include <utils/numeric/pair_hash.h>
#include <utils/numeric/vector_hash.h>
#include <utils/numeric/uset_hash.h>
#include <graphs/NodeLabelBijectionFA.h>
#include <iostream>
#include <utils/structures/set_operations.h>
#include <cassert>
#include <ltlf/westergraad_semantics/WestergaardNFALabel.h>

//#define DEBUG

struct FormulaToWestergaardFA {
    ltlf normalized_formula;
    NodeLabelBijectionFA<ltlf, WestergaardNFALabel<std::string>> fa;
    std::unordered_set<std::string> totalSymbols;
    int pozzo;

    FormulaToWestergaardFA(const ltlf& formula);
    void asMultigraph(NodeLabelBijectionFA<ltlf, std::string>& multigraph, const std::vector<std::string>& allSigma);
    void asMultigraphIgnoreNodeLabels(NodeLabelBijectionFA<std::string, std::string> &multigraph, const std::vector<std::string> &allSigma);

    friend std::ostream &operator<<(std::ostream &os, const FormulaToWestergaardFA &fa);

private:
    void fillUp();
    bool first;
    int _visit(const ltlf& formula, int pos = 1);
};


#endif //CLASSIFIERS_FORMULATOWESTERGAARDFA_H