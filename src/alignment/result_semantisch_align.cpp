/*
 * result_semantisch_align.cpp
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

#include "alignment/result_semantisch_align.h"

result_semantisch_align::result_semantisch_align(double iSigma, double iMax, double iHit) : ISigma(iSigma), IMax(iMax), IHit(iHit) {}

result_semantisch_align::result_semantisch_align() : result_semantisch_align(0, 0, 0) {}