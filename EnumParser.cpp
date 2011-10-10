#include "EnumParser.h"


namespace qi = boost::spirit::qi;

namespace parse {

    template <>
    const enum_parser_rule<PlanetSize>::type& enum_parser<PlanetSize>()
    {
        qi::_1_type _1;
        qi::_val_type _val;
        const parse::lexer& tok = parse::lexer::instance();
        static enum_parser_rule<PlanetSize>::type retval
            =    tok.planet_size_enum [ _val = _1 ]
            |    tok.Asteroids_ [ _val = SZ_ASTEROIDS ]
            |    tok.GasGiant_ [ _val = SZ_GASGIANT ]
            ;
        return retval;
    }

    template <>
    const enum_parser_rule<PlanetType>::type& enum_parser<PlanetType>()
    {
        qi::_1_type _1;
        qi::_val_type _val;
        const parse::lexer& tok = parse::lexer::instance();
        static enum_parser_rule<PlanetType>::type retval
            =    tok.planet_type_enum [ _val = _1 ]
            |    tok.Asteroids_ [ _val = PT_ASTEROIDS ]
            |    tok.GasGiant_ [ _val = PT_GASGIANT ]
            ;
        return retval;
    }

    template <>
    const enum_parser_rule<PlanetEnvironment>::type& enum_parser<PlanetEnvironment>()
    {
        qi::_1_type _1;
        qi::_val_type _val;
        const parse::lexer& tok = parse::lexer::instance();
        static enum_parser_rule<PlanetEnvironment>::type retval
            =    tok.planet_environment_enum [ _val = _1 ]
            ;
        return retval;
    }

    template <>
    const enum_parser_rule<UniverseObjectType>::type& enum_parser<UniverseObjectType>()
    {
        qi::_1_type _1;
        qi::_val_type _val;
        const parse::lexer& tok = parse::lexer::instance();
        static enum_parser_rule<UniverseObjectType>::type retval
            =    tok.universe_object_type_enum [ _val = _1 ]
            |    tok.Building_ [ _val = OBJ_BUILDING ]
            |    tok.Fleet_ [ _val = OBJ_FLEET ]
            |    tok.Planet_ [ _val = OBJ_PLANET ]
            |    tok.System_ [ _val = OBJ_SYSTEM ]
            ;
        return retval;
    }

    template <>
    const enum_parser_rule<StarType>::type& enum_parser<StarType>()
    {
        qi::_1_type _1;
        qi::_val_type _val;
        const parse::lexer& tok = parse::lexer::instance();
        static enum_parser_rule<StarType>::type retval
            =    tok.star_type_enum [ _val = _1 ]
            ;
        return retval;
    }

}
