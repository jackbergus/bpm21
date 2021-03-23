//
// Created by giacomo on 10/03/21.
//

#ifndef BPM21_STRINGS_H
#define BPM21_STRINGS_H

#include <string>
#include <cstdio>
#ifndef EOF
#define EOF     (std::char_traits<char>::eof())
#endif

#include <nlohmann/json.hpp>
#define UNESCAPE(x)                  nlohmann::json::parse(x).get<std::string>()

#endif //BPM21_STRINGS_H
