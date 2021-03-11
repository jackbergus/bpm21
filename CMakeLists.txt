cmake_minimum_required(VERSION 3.16)
project(bpm21)

set(CMAKE_CXX_STANDARD 20)

set(FORT_ENABLE_TESTING OFF CACHE INTERNAL "")
add_subdirectory(submodules/antlr4/runtime/Cpp)
include_directories(submodules/antlr4/runtime/Cpp/runtime/src)
include_directories(antlr4/cpp/antlr4)
include_directories(include)

include_directories(submodules/json/include)
include_directories(submodules/magic_enum/include)

set(antlr_sources antlr4/cpp/antlr4/DADBaseListener.cpp
                  antlr4/cpp/antlr4/DADBaseVisitor.cpp
                  antlr4/cpp/antlr4/DADLexer.cpp
                  antlr4/cpp/antlr4/DADListener.cpp
                  antlr4/cpp/antlr4/DADParser.cpp
                  antlr4/cpp/antlr4/DADVisitor.cpp
        src/pipeline/DoublePrevNext.cpp include/pipeline/DoublePrevNext.h src/pipeline/double_interval_tree_t.cpp include/pipeline/double_interval_tree_t.h src/pipeline/StringPrevNext.cpp include/pipeline/StringPrevNext.h src/pipeline/string_interval_tree_t.cpp include/pipeline/string_interval_tree_t.h src/pipeline/pipeline_utils.cpp include/pipeline/pipeline_utils.h src/pipeline/input_pipeline.cpp include/pipeline/input_pipeline.h)

add_executable(bpm21
        main.cpp

        ${antlr_sources}

        src/declare/DeclareModelParse.cpp
        src/ltlf/PropositionalizedAtomsSet.cpp
        src/utils/numeric/hash_combine.cpp
        src/ltlf/ltlf.cpp include/ltlf/ltlf.h
        src/declare/DeclareDataAware.cpp
        src/ltlf/DataPredicate.cpp)

target_link_libraries(bpm21 antlr4_static)