// -*- C++ -*-
#ifndef _ValueRefParser_h_
#define _ValueRefParser_h_

#include "Lexer.h"

#include <boost/spirit/include/qi.hpp>


namespace parse {

    template <typename T>
    struct value_ref_parser_rule
    {
        typedef boost::spirit::qi::rule<
            parse::token_iterator,
            ValueRef::ValueRefBase<T>* (), // TODO: Rename this ValueRef::Base in the FO code.
            parse::skipper_type
        > type;
    };

    /** Returns a const reference to the ValueRef parser for the type \a T. */
    template <typename T>
    const typename value_ref_parser_rule<T>::type& value_ref_parser(const parse::lexer& tok);

    template <>
    const value_ref_parser_rule<int>::type& value_ref_parser<int>(const parse::lexer& tok);

    template <>
    const value_ref_parser_rule<double>::type& value_ref_parser<double>(const parse::lexer& tok);

    template <>
    const value_ref_parser_rule<std::string>::type& value_ref_parser<std::string>(const parse::lexer& tok);

    template <>
    const value_ref_parser_rule<PlanetSize>::type& value_ref_parser<PlanetSize>(const parse::lexer& tok);

    template <>
    const value_ref_parser_rule<PlanetType>::type& value_ref_parser<PlanetType>(const parse::lexer& tok);

    template <>
    const value_ref_parser_rule<PlanetEnvironment>::type& value_ref_parser<PlanetEnvironment>(const parse::lexer& tok);

    template <>
    const value_ref_parser_rule<UniverseObjectType>::type& value_ref_parser<UniverseObjectType>(const parse::lexer& tok);

    template <>
    const value_ref_parser_rule<StarType>::type& value_ref_parser<StarType>(const parse::lexer& tok);

}

#endif
