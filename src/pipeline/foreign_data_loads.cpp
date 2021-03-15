/*
 * foreign_data_loads.cpp
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
// Created by giacomo on 15/03/21.
//

#include "pipeline/foreign_data_loads.h"
#include <string>
#include <iostream>
#include <boost/regex.hpp>
using namespace boost;

std::string istr(const smatch &m) {
    return generateStringFromNumber(std::stoul(m[0].str()));
}

std::vector<std::vector<std::string>> load_xes_ignore_data(const std::string &filename) {

    rapidxml::xml_document<> doc;
    std::vector<char> buffer;
    rapidxml::xml_node<> *root_node = init_XML_parse(filename, "log", doc, buffer);
    std::string concept_name{"concept:name"}/*, timeTimestamp{"time:timestamp"}*/;
    std::vector<std::vector<std::string>> log;

    SIBLING_ITERATE(trace, root_node, "trace", false) {
        std::vector<std::string>& transaction = log.emplace_back();
        long long int previous = 0;

        SIBLING_ITERATE(event, trace, "event", false) {

            std::string eventName = ".";
            long long int timestamp = 0;

            SIBLING_ITERATE(t, event, "string", false) {
                if (GET_ATTRIBUTE(t, "key") == concept_name) {
                    eventName = GET_ATTRIBUTE(t, "value");
                    STRIP_ALL_SPACES(eventName);
                    break;
                }
            }

            /*SIBLING_ITERATE(t, event, "date", false) {
                if (GET_ATTRIBUTE(t, "key") == timeTimestamp) {
                    timestamp = parse8601(GET_ATTRIBUTE(t, "value"));
                    break;
                }
            }
            if (timestamp == 0) timestamp += (previous+1);*/
            eventName = regex_replace(eventName, regex("[[:digit:]]+"), istr);
            STRIP_ALL_SPACES(eventName);
            STRIP_NOT_ALPHAS(eventName);
            eventName[0] = toupper(eventName[0]);
            eventName.erase(remove_if(eventName.begin(), eventName.end(), isspace), eventName.end());
            transaction.emplace_back(eventName/*, timestamp*/);
        }
    }

    return log;
}

std::vector<std::vector<std::pair<std::string, std::unordered_map<std::string, std::variant<std::string, double>>>>>

load_xes_with_data(const std::string &filename) {

    rapidxml::xml_document<> doc;
    std::vector<char> buffer;
    rapidxml::xml_node<> *root_node = init_XML_parse(filename, "log", doc, buffer);
    std::string concept_name{"concept:name"}/*, timeTimestamp{"time:timestamp"}*/;
    std::vector<std::vector<std::pair<std::string, std::unordered_map<std::string, std::variant<std::string, double>>>>> log;

    SIBLING_ITERATE(trace, root_node, "trace", false) {
        std::vector<std::pair<std::string, std::unordered_map<std::string, std::variant<std::string, double>>>>& transaction = log.emplace_back();
        long long int previous = 0;

        SIBLING_ITERATE(event, trace, "event", false) {
            std::pair<std::string, std::unordered_map<std::string, std::variant<std::string, double>>> parsed_event;
            std::string eventName = ".";
            long long int timestamp = 0;

            SIBLING_ITERATE2(t, event) {
                std::string attribute = GET_ATTRIBUTE(t, "key");
                std::string value     = GET_ATTRIBUTE(t, "value");
                std::string tag_name  = t->name();

                if (tag_name == "string") {
                    if (attribute == concept_name) {
                        eventName = value;
                        eventName = regex_replace(eventName, regex("\\d+"), istr);
                        STRIP_ALL_SPACES(eventName);
                        STRIP_NOT_ALPHAS(eventName);
                        break;
                    } else {
                        STRIP_NOT_ALPHAS(attribute);
                        TO_LOWER(attribute);
                        parsed_event.second[attribute] = value;
                    }
                } else if (tag_name == "float") {
                    STRIP_NOT_ALPHAS(attribute);
                    TO_LOWER(attribute);
                    parsed_event.second[attribute] = std::stod(value);
                } else if (tag_name == "int") {
                    STRIP_NOT_ALPHAS(attribute);
                    TO_LOWER(attribute);
                    parsed_event.second[attribute] = std::stod(value);
                } else if (tag_name == "date") {
                    STRIP_NOT_ALPHAS(attribute);
                    parsed_event.second["time"] = (float)parse8601(value);
                }
            }

            eventName[0] = toupper(eventName[0]);
            eventName.erase(remove_if(eventName.begin(), eventName.end(), isspace), eventName.end());
            parsed_event.first = eventName;

            transaction.emplace_back(parsed_event);
        }
    }

    return log;
}


