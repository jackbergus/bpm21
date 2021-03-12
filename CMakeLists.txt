cmake_minimum_required(VERSION 3.16)
project(bpm21)

set(CMAKE_CXX_STANDARD 20)


find_package(Threads REQUIRED)

set(FORT_ENABLE_TESTING OFF CACHE INTERNAL "")
add_subdirectory(submodules/antlr4/runtime/Cpp)
include_directories(submodules/antlr4/runtime/Cpp/runtime/src)
include_directories(antlr4/cpp/antlr4)
include_directories(include)

include_directories(submodules/json/include)
include_directories(submodules/magic_enum/include)

add_subdirectory(submodules/lydia-full)
include_directories(submodules/lydia-full/lib/include)
include_directories(submodules/lydia-full/third_party/spdlog/include)

set(pipeline
        src/pipeline/DoublePrevNext.cpp
        src/pipeline/double_interval_tree_t.cpp
        src/pipeline/StringPrevNext.cpp
        src/pipeline/string_interval_tree_t.cpp
        src/pipeline/pipeline_utils.cpp
        src/pipeline/input_pipeline.cpp)

set(graphs

        src/graphs/adjacency_graph.cpp
        src/graphs/minimizeDFA.cpp)

set(antlr_sources antlr4/cpp/antlr4/DADBaseListener.cpp
                  antlr4/cpp/antlr4/DADBaseVisitor.cpp
                  antlr4/cpp/antlr4/DADLexer.cpp
                  antlr4/cpp/antlr4/DADListener.cpp
                  antlr4/cpp/antlr4/DADParser.cpp
                  antlr4/cpp/antlr4/DADVisitor.cpp)

add_executable(bpm21
        main.cpp

        ${antlr_sources}
        ${pipeline}

        src/graphs/third-party-wrappers/lydia_entry_point.cpp

        src/declare/DeclareModelParse.cpp
        src/ltlf/PropositionalizedAtomsSet.cpp
        src/utils/numeric/hash_combine.cpp
        src/ltlf/ltlf.cpp include/ltlf/ltlf.h
        src/declare/DeclareDataAware.cpp
        src/ltlf/DataPredicate.cpp)

target_link_libraries(bpm21 antlr4_static lydia Threads::Threads)
