/*
 * FLLOATScriptRunner.cpp
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

#include "graphs/third-party-wrappers/FLLOATScriptRunner.h"

FLLOATScriptRunner::FLLOATScriptRunner() {
    //Py_Initialize();
}

FLLOATScriptRunner::~FLLOATScriptRunner() {
    //Py_Finalize();
}

#include <filesystem>
namespace fs = std::filesystem;

void FLLOATScriptRunner::process_expression(const std::string &file_to_parse) {
    fs::path path = "python";
    path = path / "ltlf_to_graph_dot_files.py";
    std::string script_file = fs::absolute(path).string();
    std::string run = "python3 ";
    {
        std::stringstream ss;
        ss << std::quoted(script_file);
        run += ss.str();
    }
    run += " ";
    {
        std::stringstream ss;
        ss << std::quoted(file_to_parse);
        run += ss.str();
    }
    system(run.c_str());
    #if 0
    FILE*        exp_file;
    PyObject*    main_module, * global_dict, * expression;
    char func_name[] = "generate_graphs";
    //std::string var_init = "filepath=";
    //var_init += std::quoted(file)._M_string;

    // Initialize a global variable for
    // display of expression results
    //PyRun_SimpleString(var_init.c_str());

    // Open and execute the Python file
    exp_file = fopen(script_file.c_str(), "r");
    PyRun_SimpleFile(exp_file, script_file.c_str());

    // Get a reference to the main module
    // and global dictionary
    main_module = PyImport_AddModule("__main__");
    global_dict = PyModule_GetDict(main_module);

    PyObject *arglist = Py_BuildValue(file_to_parse.c_str());
    // Extract a reference to the function "func_name"
    // from the global dictionary
    expression =
            PyDict_GetItemString(global_dict, func_name);

    //while(num--) {
    // Make a call to the function referenced
    // by "expression"
    PyObject_CallObject(expression, arglist);
    //}
    Py_DECREF(arglist);
#endif
}
