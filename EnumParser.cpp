#include "EnumParser.h"


namespace qi = boost::spirit::qi;

using qi::_1;
using qi::_val;

namespace parse {

    template <>
    const enum_parser_rule<PlanetSize>::type& enum_parser<PlanetSize>(const parse::lexer& tok)
    {
        static enum_parser_rule<PlanetSize>::type retval
            =    tok.planet_size_enum [ _val = _1 ]
            |    tok.asteroids [ _val = SZ_ASTEROIDS ]
            |    tok.gas_giant [ _val = SZ_GASGIANT ]
            ;
        return retval;
    }

    template <>
    const enum_parser_rule<PlanetType>::type& enum_parser<PlanetType>(const parse::lexer& tok)
    {
        static enum_parser_rule<PlanetType>::type retval
            =    tok.planet_type_enum [ _val = _1 ]
            |    tok.asteroids [ _val = PT_ASTEROIDS ]
            |    tok.gas_giant [ _val = PT_GASGIANT ]
            ;
        return retval;
    }

    template <>
    const enum_parser_rule<PlanetEnvironment>::type& enum_parser<PlanetEnvironment>(const parse::lexer& tok)
    {
        static enum_parser_rule<PlanetEnvironment>::type retval
            =    tok.planet_environment_enum [ _val = _1 ]
            ;
        return retval;
    }

    template <>
    const enum_parser_rule<UniverseObjectType>::type& enum_parser<UniverseObjectType>(const parse::lexer& tok)
    {
        static enum_parser_rule<UniverseObjectType>::type retval
            =    tok.universe_object_type_enum [ _val = _1 ]
            |    tok.building [ _val = OBJ_BUILDING ]
            ;
        return retval;
    }

    template <>
    const enum_parser_rule<StarType>::type& enum_parser<StarType>(const parse::lexer& tok)
    {
        static enum_parser_rule<StarType>::type retval
            =    tok.star_type_enum [ _val = _1 ]
            ;
        return retval;
    }

}
