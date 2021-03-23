/*
 * FLLOATScriptRunner.h
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
// Created by giacomo on 13/03/21.
//

#ifndef BPM21_FLLOATSCRIPTRUNNER_H
#define BPM21_FLLOATSCRIPTRUNNER_H

#define PY_SSIZE_T_CLEAN
#include <string>
#include <iomanip>


class FLLOATScriptRunner {
public:
    FLLOATScriptRunner();
    ~FLLOATScriptRunner();
    void process_expression(const std::string& file_to_parse);
};



#endif //BPM21_FLLOATSCRIPTRUNNER_H
