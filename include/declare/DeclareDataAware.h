/*
 * DeclareDataAware.h
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
// Created by giacomo on 10/03/21.
//

#ifndef BPM21_DECLAREDATAAWARE_H
#define BPM21_DECLAREDATAAWARE_H

/**
 * Definition of all the types of Declare statements
 */
enum declare_tempaltes {
    EXISTENCE,
    ABSENCE,
    EXACTLY,
    INIT,
    RESP_EXISTENCE,
    CO_EXISTENCE,
    RESPONSE,
    PRECEDENCE,
    ALT_RESPONSE,
    ALT_PRECEDENCE,
    CHAIN_RESPONSE,
    CHAIN_PRECEDENCE,
    SUCCESSION,
    NOT_SUCCESSION
};

#include <string>
#include <unordered_map>
#include <vector>
#include <ltlf/DataPredicate.h>
#include <utils/numeric/pair_hash.h>
#include <utils/numeric/vector_hash.h>

struct DeclareDataAware {
    declare_tempaltes casusu;

    size_t n;
    std::string left_act, right_act;

    // Each map represents a conjunction among different atoms over distinct variables, while the vector represents the disjunction
    std::vector<std::unordered_map<std::string, DataPredicate>> dnf_left_map, dnf_right_map;

    DeclareDataAware() = default;
    DeclareDataAware(const DeclareDataAware&) = default;
    DeclareDataAware( DeclareDataAware&&) = default;
    DeclareDataAware& operator=(const DeclareDataAware& )=default;
    DeclareDataAware& operator=( DeclareDataAware&& )=default;

    static struct basic_declare Existence(size_t n, const std::string& act);
    static struct basic_declare Absence(size_t n, const std::string& act);
    static struct basic_declare Exactly(size_t n, const std::string& act);
    static struct basic_declare Init(size_t n, const std::string& act);
    static struct basic_declare RespondedExistence(const std::string& left, const std::string& right);
    static struct basic_declare CoExistence(const std::string &left, const std::string &right);
    static struct basic_declare Response(const std::string& left, const std::string& right);
    static struct basic_declare Precedence(const std::string& left, const std::string& right);
    static struct basic_declare AlternateResponse(const std::string& left, const std::string& right);
    static struct basic_declare AlternatePrecedence(const std::string& left, const std::string& right);
    static struct basic_declare ChainResponse(const std::string& left, const std::string& right);
    static struct basic_declare ChainPrecedence(const std::string& left, const std::string& right);
    static struct basic_declare Succession(const std::string& left, const std::string& right);
    static struct basic_declare NotSuccession(const std::string& left, const std::string& right);

};



namespace std {
    template <>
    struct hash<DeclareDataAware> {
        std::size_t operator()(const DeclareDataAware& k) const {
            size_t seed = 31;
            seed = hash_combine(seed, k.casusu);
            seed = hash_combine(seed, k.n);
            seed = hash_combine(hash_combine(seed, k.left_act), k.dnf_left_map);
            return hash_combine(hash_combine(seed, k.right_act), k.dnf_right_map);
        }
    };

}

#endif //BPM21_DECLAREDATAAWARE_H
