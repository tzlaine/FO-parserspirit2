// -*- C++ -*-
#ifndef _EnumParser_h_
#define _EnumParser_h_

#include "Lexer.h"

#include <boost/spirit/include/qi.hpp>


namespace parse {

    template <typename E>
    struct enum_parser_rule
    {
        typedef boost::spirit::qi::rule<
            parse::token_iterator,
            E (),
            parse::skipper_type
        > type;
    };

    /** Returns a const reference to the parser for the enum \a E. */
    template <typename E>
    const typename enum_parser_rule<E>::type& enum_parser();

    template <>
    const enum_parser_rule<PlanetSize>::type& enum_parser<PlanetSize>();

    template <>
    const enum_parser_rule<PlanetType>::type& enum_parser<PlanetType>();

    template <>
    const enum_parser_rule<PlanetEnvironment>::type& enum_parser<PlanetEnvironment>();

    template <>
    const enum_parser_rule<UniverseObjectType>::type& enum_parser<UniverseObjectType>();

    template <>
    const enum_parser_rule<StarType>::type& enum_parser<StarType>();

}

#endif
