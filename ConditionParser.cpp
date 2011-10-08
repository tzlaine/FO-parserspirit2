#include "ConditionParser.h"

#include "../universe/Condition.h"

#include <GG/ReportParseError.h>

#include <boost/spirit/home/phoenix.hpp>


namespace qi = boost::spirit::qi;
namespace phoenix = boost::phoenix;


#define NAME(x) x.name(#x); debug(x)

namespace {

    struct condition_parser_rules_1
    {
        condition_parser_rules_1(const parse::lexer& tok)
            {
                const parse::value_ref_parser_rule<int>::type& int_value_ref =
                    parse::value_ref_parser<int>(tok);

                const parse::value_ref_parser_rule<double>::type& double_value_ref =
                    parse::value_ref_parser<double>(tok);

                const parse::value_ref_parser_rule<std::string>::type& string_value_ref =
                    parse::value_ref_parser<std::string>(tok);

                const parse::value_ref_parser_rule< ::PlanetType>::type& planet_type_value_ref =
                    parse::value_ref_parser< ::PlanetType>(tok);

                const parse::value_ref_parser_rule< ::PlanetSize>::type& planet_size_value_ref =
                    parse::value_ref_parser< ::PlanetSize>(tok);

                const parse::value_ref_parser_rule< ::PlanetEnvironment>::type& planet_environment_value_ref =
                    parse::value_ref_parser< ::PlanetEnvironment>(tok);

                const parse::value_ref_parser_rule< ::UniverseObjectType>::type& universe_object_type_value_ref =
                    parse::value_ref_parser< ::UniverseObjectType>(tok);

                using qi::_1;
                using qi::_a;
                using qi::_b;
                using qi::_val;
                using phoenix::new_;

                all
                    =    tok.all [ _val = new_<Condition::All>() ]
                    ;

                source
                    =    tok.source [ _val = new_<Condition::Source>() ]
                    ;

                root_candidate
                    =    tok.root_candidate [ _val = new_<Condition::RootCandidate>() ]
                    ;

                target
                    =    tok.target [ _val = new_<Condition::Target>() ]
                    ;

                stationary
                    =    tok.stationary [ _val = new_<Condition::Stationary>() ]
                    ;

                capital
                    =    tok.capital [ _val = new_<Condition::Capital>() ]
                    ;

                monster
                    =    tok.monster [ _val = new_<Condition::Monster>() ]
                    ;

                armed
                    =    tok.armed [ _val = new_<Condition::Armed>() ]
                    ;

                owned_by
                    =    (
                              tok.owned_by
                          >   tok.affiliation > '='
                          >   tok.affiliation_enum [ _a = _1 ]
                          >   tok.empire > '='
                          >   int_value_ref [ _val = new_<Condition::EmpireAffiliation>(_1, _a) ]
                         )
                    |    (
                              tok.owned_by
                          >   tok.affiliation > '='
                          >   tok.affiliation_enum [ _val = new_<Condition::EmpireAffiliation>(_1) ]
                         )
                    ;

                homeworld
                    =    (
                              tok.homeworld
                          >   tok.name > '='
                          >   string_vec [ _val = new_<Condition::Homeworld>(_1) ]
                         )
                    |    tok.homeworld [ _val = new_<Condition::Homeworld>() ]
                    ;

                building
                    =    tok.building
                    >    tok.name > '='
                    >    string_vec [ _val = new_<Condition::Building>(_1) ]
                    ;

                species
                    =    (
                              tok.species
                          >>  tok.name > '='
                          >   string_vec [ _val = new_<Condition::Species>(_1) ]
                         )
                    |    tok.species [ _val = new_<Condition::Species>() ]
                    ;

                focus_type
                    =    (
                              tok.focus_type
                          >>  tok.type > '='
                          >   string_vec [ _val = new_<Condition::FocusType>(_1) ]
                         )
                    |    tok.focus_type [ _val = new_<Condition::FocusType>() ]
                    ;

                start
                    %=   all
                    |    source
                    |    root_candidate
                    |    target
                    |    stationary
                    |    capital
                    |    monster
                    |    armed
                    |    owned_by
                    |    homeworld
                    |    building
                    |    species
                    |    focus_type
                    |    planet_type
                    |    planet_size
                    |    planet_environment
                    |    object_type
                    |    meter_value
                    |    and_
                    |    or_
                    |    not_
                    ;
            }

        typedef boost::spirit::qi::rule<
            parse::token_iterator,
            Condition::ConditionBase* (),
            qi::locals<EmpireAffiliationType>,
            parse::skipper_type
        > owned_by_rule;

        typedef boost::spirit::qi::rule<
            parse::token_iterator,
            Condition::ConditionBase* (),
            qi::locals<
                MeterType,
                ValueRef::ValueRefBase<double>*,
                ValueRef::ValueRefBase<double>*
            >,
            parse::skipper_type
        > value_ref_ints_rule;

        parse::condition_parser_rule all;
        parse::condition_parser_rule source;
        parse::condition_parser_rule root_candidate;
        parse::condition_parser_rule target;
        parse::condition_parser_rule stationary;
        parse::condition_parser_rule capital;
        parse::condition_parser_rule monster;
        parse::condition_parser_rule armed;
        owned_by_rule owned_by;
        parse::condition_parser_rule homeworld;
        parse::condition_parser_rule building;
        parse::condition_parser_rule species;
        parse::condition_parser_rule focus_type;
        parse::condition_parser_rule planet_type;
        parse::condition_parser_rule planet_size;
        parse::condition_parser_rule planet_environment;
        parse::condition_parser_rule object_type;
        value_ref_ints_rule meter_value;
        parse::condition_parser_rule and_;
        parse::condition_parser_rule or_;
        parse::condition_parser_rule not_;
        parse::condition_parser_rule start;
    };

    struct condition_parser_rules_2
    {
        condition_parser_rules_2(const parse::lexer& tok)
            {
                const parse::value_ref_parser_rule<int>::type& int_value_ref =
                    parse::value_ref_parser<int>(tok);

                const parse::value_ref_parser_rule< ::ShipPartClass>::type& ship_part_class_ref =
                    parse::value_ref_parser< ::ShipPartClass>(tok);

                using qi::_1;
                using qi::_a;
                using qi::_b;
                using qi::_val;
                using phoenix::new_;

                has_special
                    =    tok.has_special
                    >    tok.name > '='
                    >    tok.string [ _val = new_<Condition::HasSpecial>(_1) ]
                    ;

                has_special_since_turn
                    =    tok.has_special_since_turn
                    >    tok.name > '='
                    >    tok.string [ _a = _1 ]
                    >    tok.low > '='
                    >    int_value_ref [ _b = _1 ]
                    >    tok.high > '='
                    >    int_value_ref [ _val = new_<Condition::HasSpecial>(_a, _b, _1) ]
                    ;

                owner_has_tech
                    =    tok.owner_has_tech
                    >    tok.name > '='
                    >    tok.string [ _val = new_<Condition::OwnerHasTech>(_1) ]
                    ;

                design_has_hull
                    =    tok.design_has_hull
                    >    tok.name > '='
                    >    tok.string [ _val = new_<Condition::DesignHasHull>(_1) ]
                    ;

                design_has_part
                    =    tok.design_has_part
                    >    tok.low > '='
                    >    int_value_ref [ _a = _1 ]
                    >    tok.high > '='
                    >    int_value_ref [ _b = _1 ]
                    >    tok.class_ > '='
                    >    tok.string [ _val = new_<Condition::DesignHasPart>(_a, _b, _1) ]
                    ;

                design_has_part_class
                    =    tok.design_has_part_class
                    >    tok.low > '='
                    >    int_value_ref [ _a = _1 ]
                    >    tok.high > '='
                    >    int_value_ref [ _b = _1 ]
                    >    tok.class_ > '='
                    >    ship_part_class_ref [ _val = new_<Condition::DesignHasPartClass>(_a, _b, _1) ]
                    ;

                predefined_design
                    =    tok.design
                    >>   tok.name > '=' // TODO: make this condition Design = "name"
                    >    tok.string [ _val = new_<Condition::PredefinedShipDesign>(_1) ]
                    ;

                design_number
                    =    tok.design
                    >>   tok.design > '=' // TODO: make this condition Design = #
                    >    int_value_ref [ _val = new_<Condition::NumberedShipDesign>(_1) ]
                    ;

                produced_by_empire
                    =    tok.produced_by_empire
                    >    tok.empire > '='
                    >    int_value_ref [ _val = new_<Condition::ProducedByEmpire>(_1) ]
                    ;

                visible_to_empire
                    =    tok.visible_to_empire
                    >    tok.empire > '='
                    >    int_value_ref [ _val = new_<Condition::VisibleToEmpire>(_1) ]
                    ;

                explored_by_empire
                    =    tok.explored_by_empire
                    >    tok.empire > '='
                    >    int_value_ref [ _val = new_<Condition::ExploredByEmpire>(_1) ]
                    ;

                fleet_supplyable_by_empire
                    =    tok.fleet_supplyable_by_empire
                    >    tok.empire > '='
                    >    int_value_ref [ _val = new_<Condition::FleetSupplyableByEmpire>(_1) ]
                    ;

                in_system
                    =    tok.in_system
                    >    tok.id > '='
                    >    int_value_ref [ _val = new_<Condition::InSystem>(_1) ]
                    ;

                object_id
                    =    tok.object
                    >    tok.id > '='
                    >    int_value_ref [ _val = new_<Condition::ObjectID>(_1) ]
                    ;

                start
                    %=   has_special
                    |    has_special_since_turn
                    |    owner_has_tech
                    |    design_has_hull
                    |    design_has_part
                    |    design_has_part_class
                    |    predefined_design
                    |    design_number
                    |    produced_by_empire
                    |    visible_to_empire
                    |    explored_by_empire
                    |    fleet_supplyable_by_empire
                    |    in_system
                    |    object_id
                    ;
            }

        typedef boost::spirit::qi::rule<
            parse::token_iterator,
            Condition::ConditionBase* (),
            qi::locals<
                ValueRef::ValueRefBase<int>*,
                ValueRef::ValueRefBase<int>*
            >,
            parse::skipper_type
        > value_ref_ints_rule;

        typedef boost::spirit::qi::rule<
            parse::token_iterator,
            Condition::ConditionBase* (),
            qi::locals<
                MeterType,
                std::string*,
                ValueRef::ValueRefBase<int>*
            >,
            parse::skipper_type
        > string_int_ref_rule;

        parse::condition_parser_rule has_special;
        string_int_ref has_special_since_turn;
        parse::condition_parser_rule owner_has_tech;
        parse::condition_parser_rule design_has_hull;
        value_ref_ints_rule design_has_part;
        value_ref_ints_rule design_has_part_class;
        parse::condition_parser_rule predefined_design;
        parse::condition_parser_rule design_number;
        parse::condition_parser_rule produced_by_empire;
        parse::condition_parser_rule visible_to_empire;
        parse::condition_parser_rule explored_by_empire;
        parse::condition_parser_rule fleet_supplyable_by_empire;
        parse::condition_parser_rule in_system;
        parse::condition_parser_rule object_id;
        parse::condition_parser_rule start;
    };

    struct condition_parser_rules_3
    {
        condition_parser_rules_3(const parse::lexer& tok)
            {
                const parse::value_ref_parser_rule<int>::type& int_value_ref =
                    parse::value_ref_parser<int>(tok);

                const parse::value_ref_parser_rule<double>::type& double_value_ref =
                    parse::value_ref_parser<double>(tok);

                const parse::value_ref_parser_rule< ::StarType>::type& star_type_value_ref =
                    parse::value_ref_parser< ::StartType>(tok);

                using qi::_1;
                using qi::_a;
                using qi::_b;
                using qi::_c;
                using qi::_d;
                using qi::_val;
                using phoenix::new_;
                using phoenix::push_back;

                within_distance
                    =    tok.within_distance
                    >    tok.distance > '='
                    >    double_value_ref [ _a = _1 ]
                    >    tok.condition > '='
                    >    parse::condition_parser(tok) [ _val = new_<Condition::WithinDistance>(_a, _1) ]
                    ;

                within_starlane_jumps
                    =    tok.within_starlane_jumps
                    >    tok.jumps > '='
                    >    int_value_ref [ _a = _1 ]
                    >    tok.condition > '='
                    >    parse::condition_parser(tok) [ _val = new_<Condition::WithinStarlaneJumps>(_a, _1) ]
                    ;

                number
                    =    tok.number
                    >    tok.low > '='
                    >    int_value_ref [ _a = _1 ]
                    >    tok.high > '='
                    >    int_value_ref [ _b = _1 ]
                    >    tok.condition > '='
                    >    parse::condition_parser(tok) [ _val = new_<Condition::Number>(_a, _b, _1) ]
                    ;

                turn
                    =    tok.turn
                    >    tok.low > '='
                    >    int_value_ref [ _a = _1 ]
                    >    tok.high > '='
                    >    int_value_ref [ _val = new_<Condition::Turn>(_a, _1) ]
                    ;

                created_on_turn
                    =    tok.created_on_turn
                    >    tok.low > '='
                    >    int_value_ref [ _a = _1 ]
                    >    tok.high > '='
                    >    int_value_ref [ _val = new_<Condition::Turn>(_a, _1) ]
                    ;

                number_of
                    =    (
                              tok.number_of
                          >   tok.number > '='
                          >   int_value_ref [ _a = _1 ]
                          >   tok.condition > '='
                          >   parse::condition_parser(tok) [ _val = new_<Condition::SortedNumberOf>(_a, _1) ]
                         )
                    |    (
                              (
                                   tok.maximum_number_of [ _b = Condition::SORT_MAX ]
                               |   tok.minimum_number_of [ _b = Condition::SORT_MIN ]
                               |   tok.mode_number_of [ _b = Condition::SORT_MODE ]
                              )
                          >   tok.number > '='
                          >   int_value_ref [ _a = _1 ]
                          >   tok.sort_key > '='
                          >   double_value_ref [ _c = _1 ]
                          >   tok.condition > '='
                          >   parse::condition_parser(tok) [ _val = new_<Condition::SortedNumberOf>(_a, _b, _c, _1) ]
                         )
                    ;

                contains
                    =    tok.contains
                    >    tok.condition > '='
                    >    parse::condition_parser(tok) [ _val = new_<Condition::Contains>(_1) ]
                    ;

                contained_by
                    =    tok.contained_by
                    >    tok.condition > '='
                    >    parse::condition_parser(tok) [ _val = new_<Condition::ContainedBy>(_1) ]
                    ;

                star_type
                    =    tok.star
                    >    tok.type > '='
                    >>   (
                              ('[' > +star_type_value_ref [ push_back(_a, _1) ] > ']')
                          |   star_type_value_ref [ push_back(_a, _1) ]
                         )
                         [ _val = new_<Condition::StarType>(_a) ]
                    ;

                random
                    =    tok.random
                    >    tok.probability > '='
                    >    double_value_ref [ _val = new_<Condition::Chance>(_1) ]
                    ;

                owner_stockpile
                    =    (
                              tok.owner_food_stockpile [ _a = RE_FOOD ]
                          |   tok.owner_mineral_stockpile [ _a = RE_MINERALS ]
                          |   tok.owner_trade_stockpile [ _a = RE_TRADE ]
                         )
                    >    tok.low > '='
                    >    double_value_ref [ _b = _1 ]
                    >    tok.high > '='
                    >    double_value_ref [ _val = new_<Condition::EmpireStockpileValue>(_a, _b, _1) ]
                    ;

                resource_supply_connected
                    =    tok.resource_supply_connected
                    >    tok.empire > '='
                    >    int_value_ref [ _a = _1 ]
                    >    tok.condition > '='
                    >    parse::condition_parser(tok) [ _val = new_<Condition::ResourceSupplyConnectedByEmpire>(_a, _1) ]
                    ;

                start
                    %=   within_distance
                    |    within_starlane_jumps
                    |    number
                    |    turn
                    |    created_on_turn
                    |    number_of
                    |    contains
                    |    contained_by
                    |    star_type
                    |    random
                    |    owner_stockpile
                    |    resource_supply_connected
                    ;
            }

        typedef boost::spirit::qi::rule<
            parse::token_iterator,
            Condition::ConditionBase* (),
            qi::locals<ValueRef::ValueRefBase<int>*>,
            parse::skipper_type
        > int_ref_rule;

        typedef boost::spirit::qi::rule<
            parse::token_iterator,
            Condition::ConditionBase* (),
            qi::locals<ValueRef::ValueRefBase<int>*>,
            parse::skipper_type
        > double_ref_rule;

        typedef boost::spirit::qi::rule<
            parse::token_iterator,
            Condition::ConditionBase* (),
            qi::locals<
                ValueRef::ValueRefBase<int>*,
                ValueRef::ValueRefBase<int>*
            >,
            parse::skipper_type
        > int_ref_int_ref_rule;

        typedef boost::spirit::qi::rule<
            parse::token_iterator,
            Condition::ConditionBase* (),
            qi::locals<
                ValueRef::ValueRefBase<int>*,
                Condition::SortingMethod,
                ValueRef::ValueRefBase<double>*
            >,
            parse::skipper_type
        > int_ref_sorting_method_double_ref_rule;

        typedef boost::spirit::qi::rule<
            parse::token_iterator,
            Condition::ConditionBase* (),
            qi::locals<
                ResourceType,
                ValueRef::ValueRefBase<double>*
            >,
            parse::skipper_type
        > resource_type_double_ref_rule;

        typedef boost::spirit::qi::rule<
            parse::token_iterator,
            Condition::ConditionBase* (),
            qi::locals<std::vector<StarType> >,
            parse::skipper_type
        > star_type_vec_rule;

        parse::condition_parser_rule within_distance;
        int_ref_rule within_starlane_jumps;
        int_ref_int_ref_rule number;
        int_ref_rule turn;
        int_ref_rule created_on_turn;
        int_ref_sorting_method_double_ref_rule number_of;
        parse::condition_parser_rule contains;
        parse::condition_parser_rule contained_by;
        star_type_vec_rule star_type;
        parse::condition_parser_rule random;
        resource_type_double_ref_rule owner_stockpile;
        int_ref_rule resource_supply_connected;
        parse::condition_parser_rule start;
    };

}

namespace parse {

    const condition_parser_rule& condition_parser(const parse::lexer& tok)
    {
        static condition_parser_rules_1 rules_1(tok);
        static condition_parser_rules_2 rules_2(tok);
        static condition_parser_rules_3 rules_3(tok);
        static condition_parser_rule retval;
        static bool once = true;
        if (once) {
            retval
                %=   rules_1.start
                |    rules_2.start
                |    rules_3.start
                ;
            once = false;
        }
        return retval;
    }

}
