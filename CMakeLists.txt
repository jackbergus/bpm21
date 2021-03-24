cmake_minimum_required(VERSION 3.16)
project(bpm21)

set(CMAKE_CXX_STANDARD 20)


find_package(Threads REQUIRED)

add_library(rapidxml INTERFACE)
target_include_directories(rapidxml INTERFACE rapidxml/ )

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

find_package(Boost REQUIRED regex)
include_directories(${BOOST_INCLUDE_DIRS})
message("boost lib: ${Boost_LIBRARIES}")
message("boost inc:${Boost_INCLUDE_DIR}")

set(pipeline
        src/pipeline/DoublePrevNext.cpp
        src/pipeline/double_interval_tree_t.cpp
        src/pipeline/StringPrevNext.cpp
        src/pipeline/string_interval_tree_t.cpp
        src/pipeline/pipeline_utils.cpp
        src/pipeline/input_pipeline.cpp
        src/pipeline/SimpleXESSerializer.cpp
        include/pipeline/SimpleXESSerializer.h
        src/pipeline/foreign_data_loads.cpp
        include/pipeline/foreign_data_loads.h src/wrappers/aaltaf_wrapper.cpp src/alignment/result_semantisch_align.cpp include/alignment/result_semantisch_align.h for_semantisch_inconsistency.cpp for_semantisch_inconsistency.h)

set(graphs


        src/graphs/adjacency_graph.cpp
        src/graphs/minimizeDFA.cpp
        src/ltlf/westergraad_semantics/FormulaToWestergaardFA.cpp
        include/ltlf/westergraad_semantics/FormulaToWestergaardFA.h
        include/ltlf/westergraad_semantics/WestergaardNFALabel.h src/graphs/third-party-wrappers/FLLOATScriptRunner.cpp include/graphs/third-party-wrappers/FLLOATScriptRunner.h fast_minimization.cpp fast_minimization.h)

set(antlr_sources antlr4/cpp/antlr4/DADBaseListener.cpp
                  antlr4/cpp/antlr4/DADLexer.cpp
                  antlr4/cpp/antlr4/DADParser.cpp
                  antlr4/cpp/antlr4/DADVisitor.cpp
                  src/declare/DeclareModelParse.cpp

        antlr4/cpp/antlr4/DOTBaseListener.cpp
        antlr4/cpp/antlr4/DOTLexer.cpp
        antlr4/cpp/antlr4/DOTParser.cpp
        antlr4/cpp/antlr4/DOTVisitor.cpp


        antlr4/cpp/antlr4/FLLOATPropBaseListener.cpp
        antlr4/cpp/antlr4/FLLOATPropLexer.cpp
        antlr4/cpp/antlr4/FLLOATPropParser.cpp
        antlr4/cpp/antlr4/FLLOATPropVisitor.cpp

        antlr4/cpp/antlr4/TracesBaseListener.cpp
        antlr4/cpp/antlr4/TracesBaseVisitor.cpp
        antlr4/cpp/antlr4/TracesLexer.cpp
        antlr4/cpp/antlr4/TracesParser.cpp
        antlr4/cpp/antlr4/TracesVisitor.cpp
        src/declare/DataTraceParse.cpp
        include/declare/DataTraceParse.h src/graphs/third-party-wrappers/ParseFFLOATDot.cpp include/graphs/third-party-wrappers/ParseFFLOATDot.h src/graphs/third-party-wrappers/FLLOATSimplePropParser.cpp include/graphs/third-party-wrappers/FLLOATSimplePropParser.h)

add_executable(bpm21
        main.cpp

        ${antlr_sources}
        ${pipeline}
        ${graphs}

        src/graphs/third-party-wrappers/lydia_entry_point.cpp
        src/utils/xml_utils.cpp
        src/ltlf/PropositionalizedAtomsSet.cpp
        src/utils/numeric/hash_combine.cpp
        src/ltlf/ltlf.cpp include/ltlf/ltlf.h
        src/declare/DeclareDataAware.cpp
        src/ltlf/DataPredicate.cpp
        src/alignment/alignment_utils.cpp
        src/alignment/result_semantisch_align.cpp )

include_directories(submodules/date/include)

add_subdirectory(submodules/aaltaf)
include_directories(submodules/aaltaf/include)
include_directories(submodules/aaltaf/submodules)
include_directories(submodules/aaltaf/submodules/minisat)
include_directories(submodules/aaltaf/submodules/ltlparser)

add_subdirectory(submodules/eigen)
include_directories(submodules/eigen)


include_directories(submodules/probtrace/include/)


add_subdirectory(submodules/lemon)
include_directories(submodules/lemon)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/submodules/lemon)

target_link_libraries(bpm21 antlr4_static ${Boost_LIBRARIES} Threads::Threads lydia z aaltaf Eigen3::Eigen lemon)
