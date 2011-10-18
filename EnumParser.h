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

    template <>
    const enum_parser_rule<MeterType>::type& enum_parser<MeterType>();

    template <>
    const enum_parser_rule<EmpireAffiliationType>::type& enum_parser<EmpireAffiliationType>();

    template <>
    const enum_parser_rule<ShipSlotType>::type& enum_parser<ShipSlotType>();

    template <>
    const enum_parser_rule<ShipPartClass>::type& enum_parser<ShipPartClass>();

    template <>
    const enum_parser_rule<CombatFighterType>::type& enum_parser<CombatFighterType>();

    const enum_parser_rule<MeterType>::type& non_ship_part_meter_type_enum();

    const enum_parser_rule<MeterType>::type& ship_part_meter_type_enum();

}

#endif
