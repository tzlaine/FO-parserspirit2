cmake_minimum_required(VERSION 2.6)
cmake_policy(VERSION 2.6.4)

set(CMAKE_VERBOSE_MAKEFILE false)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_HOME_DIRECTORY}/../cmake)
include(FindGiGi)

find_package(Boost 1.44.0)
find_package(GiGi)

include_directories(${Boost_INCLUDE_DIRS} ${GIGI_INCLUDE_DIRS})

add_executable(test
    test.cpp
    lexer_test_rules.cpp
    value_ref_test_rules.cpp
    Lexer.cpp
    ValueRefParser.cpp
)

target_link_libraries(test ${GIGI_GIGI_LIBRARY})
