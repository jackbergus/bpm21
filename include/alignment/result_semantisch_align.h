/*
 * result_semantisch_align.h
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

#ifndef BPM21_RESULT_SEMANTISCH_ALIGN_H
#define BPM21_RESULT_SEMANTISCH_ALIGN_H


/**
 * The three possible semantic inconsistency metrics for the graph, that are using the actual log
 * to measure the goodness of the representation
 */
struct result_semantisch_align {
    double ISigma;  // Summation of all the misalignments between the clauses
    double IMax;    // Maximum inconsistency misalignment among the clauses
    double IHit;    // Number of inconsistent clauses

    result_semantisch_align(double iSigma, double iMax, double iHit);
    result_semantisch_align();
    result_semantisch_align(const result_semantisch_align&) = default;
    result_semantisch_align(result_semantisch_align&&) = default;
    result_semantisch_align& operator=(const result_semantisch_align&) = default;
    result_semantisch_align& operator=(result_semantisch_align&&) = default;
};


#endif //BPM21_RESULT_SEMANTISCH_ALIGN_H
