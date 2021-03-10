cmake_minimum_required(VERSION 3.16)
project(bpm21)

set(CMAKE_CXX_STANDARD 20)

set(FORT_ENABLE_TESTING OFF CACHE INTERNAL "")
add_subdirectory(submodules/antlr4/runtime/Cpp)
include_directories(submodules/antlr4/runtime/Cpp/runtime/src)
include_directories(antlr4/cpp/antlr4)
include_directories(include)

set(antlr_sources antlr4/cpp/antlr4/DADBaseListener.cpp
                  antlr4/cpp/antlr4/DADBaseVisitor.cpp
                  antlr4/cpp/antlr4/DADLexer.cpp
                  antlr4/cpp/antlr4/DADListener.cpp
                  antlr4/cpp/antlr4/DADParser.cpp
                  antlr4/cpp/antlr4/DADVisitor.cpp
                  )


add_executable(bpm21

        main.cpp

        ${antlr_sources}

        src/declare/DeclareModelParse.cpp
        include/declare/DeclareModelParse.h
        src/ltlf/PropositionalizedAtomsSet.cpp
        include/ltlf/PropositionalizedAtomsSet.h
        include/utils/numeric/hash_combine.h
        src/utils/numeric/hash_combine.cpp
        src/ltlf/ltlf.cpp include/ltlf/ltlf.h
        src/declare/DeclareDataAware.cpp
        include/declare/DeclareDataAware.h
        src/ltlf/DataPredicate.cpp
        include/ltlf/DataPredicate.h
        include/utils/structures/set_operations.h)

target_link_libraries(bpm21 antlr4_static)