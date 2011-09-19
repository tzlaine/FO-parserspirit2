// -*- C++ -*-
#define ADOBE_STD_SERIALIZATION

#include "Lexer.h"

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

struct value_ref_test_rules
{
    value_ref_test_rules(const parse::lexer& tok);

    typedef boost::spirit::qi::rule<
        parse::token_iterator,
        parse::skipper_type
    > test_rule;

    test_rule value_ref;
};

enum test_type {
    unknown,
    lexer,
    value_ref_parser
};

inline void print_help()
{ std::cout << "Usage: test lexer|value_ref_parser \"test string\"" << std::endl; }
