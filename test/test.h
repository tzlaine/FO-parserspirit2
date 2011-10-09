// -*- C++ -*-
#include "../Lexer.h"
#include "../ValueRefParser.h"

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>


struct lexer_test_rules
{
    lexer_test_rules(const parse::lexer& tok);

    typedef boost::spirit::qi::rule<
        parse::token_iterator,
        parse::skipper_type
    > test_rule;

    test_rule lexer;
};

enum test_type {
    unknown,
    lexer,
    int_value_ref_parser,
    double_value_ref_parser,
    string_value_ref_parser,
    planet_size_value_ref_parser,
    planet_type_value_ref_parser,
    planet_environment_value_ref_parser,
    universe_object_type_value_ref_parser,
    star_type_value_ref_parser,
    condition_parser,
    effect_parser
};

void print_help();
