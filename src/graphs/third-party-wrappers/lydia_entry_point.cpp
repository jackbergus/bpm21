/*
 * lydia_entry_point.cpp
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
// Created by giacomo on 01/03/21.
//

#include "graphs/third-party-wrappers/lydia_entry_point.h"
#include <lydia/parser/ltlf/driver.cpp>

lydia_entry_point::lydia_entry_point() : dfa_strategy{}, translator{dfa_strategy}, driver{new whitemech::lydia::parsers::ltlf::LTLfDriver()} {

}

whitemech::lydia::ldlf_ptr lydia_entry_point::parse_formula_from_ltlf_file(const std::string &filename) {
    std::filesystem::path formula_path(filename);
    driver->parse(formula_path.string().c_str());
    return driver->result;
}

#ifdef TRUE
#undef TRUE
#endif
#ifdef FALSE
#undef FALSE
#endif

whitemech::lydia::ldlf_ptr lydia_entry_point::convert_formula_from_objects(const ltlf &formula) {
    switch (formula.casusu) {
        case ACT: {
            auto f = driver->add_LTLfAtom(formula.act);
            if (formula.is_negated) {
                auto neg = driver->add_LTLfNot(f);
                return neg;
            } else {
                return f;
            }
        }

        case NEG_OF: {
            auto acc = convert_formula_from_objects(formula.args.at(0));
            auto neg = driver->add_LTLfNot(acc);
            return neg;
        }

        case OR: {
            auto acc1 = convert_formula_from_objects(formula.args.at(0));
            auto acc2 = convert_formula_from_objects(formula.args.at(1));
            auto or_ = driver->add_LTLfOr(acc1, acc2);
            return or_;
        }

        case AND:{
            auto acc1 = convert_formula_from_objects(formula.args.at(0));
            auto acc2 = convert_formula_from_objects(formula.args.at(1));
            auto or_ = driver->add_LTLfAnd(acc1, acc2);
            return or_;
        }

        case NEXT:{
            auto acc = convert_formula_from_objects(formula.args.at(0));
            auto neg = driver->add_LTLfNext(acc);
            return neg;
        }

        case UNTIL: {
            auto acc1 = convert_formula_from_objects(formula.args.at(0));
            auto acc2 = convert_formula_from_objects(formula.args.at(1));
            auto u = driver->add_LTLfUntil(acc1, acc2);
            return u;
        }

        case RELEASE:{
            auto acc1 = convert_formula_from_objects(formula.args.at(0));
            auto acc2 = convert_formula_from_objects(formula.args.at(1));
            auto u = driver->add_LTLfRelease(acc1, acc2);
            return u;
        }

        case formula_t::TRUE:
            return driver->add_LTLfTrue();
        default:
            return driver->add_LTLfFalse();

    }
}
