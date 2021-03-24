/*
 * aaltaf_wrapper.cpp
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
// Created by giacomo on 24/03/21.
//
#ifdef TRUE
#undef TRUE
#endif
#ifdef FALSE
#undef FALSE
#endif
#include "wrappers/aaltaf_wrapper.h"
#ifdef TRUE
#undef TRUE
#endif
#ifdef FALSE
#undef FALSE
#endif
#include <formula/aalta_formula.h>
#include <carchecker.h>

double check_formula(const std::string& ltlf_string, bool simplify) {
    aalta::aalta_formula* af;
    //set tail id to be 1
    af = aalta::aalta_formula::TAIL ();
    af = aalta::aalta_formula(ltlf_string.c_str(), true).unique();
    //af = af->nnf ();
    af = af->add_tail ();
    af = af->remove_wnext ();
    if (simplify) {
        af = af->simplify ();
        af = af->split_next ();
    }
    aalta::CARChecker checker (af, false, true);
    bool res = checker.check ();
    //printf ("%s\n", res ? "sat" : "unsat");
    return checker.get_inconsistency_measure(res);
}