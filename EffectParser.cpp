#include "EffectParser.h"

#include "ConditionParserImpl.h"
#include "EnumParser.h"
#include "ValueRefParser.h"
#include "../universe/Effect.h"
#include "../universe/ValueRef.h"

#include <GG/ReportParseError.h>

#include <boost/spirit/home/phoenix.hpp>


namespace qi = boost::spirit::qi;
namespace phoenix = boost::phoenix;


#define NAME(x) x.name(#x)
//; debug(x)

namespace {

    struct effect_parser_rules
    {
        effect_parser_rules()
            {
                qi::_1_type _1;
                qi::_a_type _a;
                qi::_b_type _b;
                qi::_c_type _c;
                qi::_d_type _d;
                qi::_e_type _e;
                qi::_val_type _val;
                qi::eps_type eps;
                using phoenix::new_;
                using phoenix::construct;
                using phoenix::push_back;

                const parse::lexer& tok = parse::lexer::instance();

                const parse::value_ref_parser_rule<int>::type& int_value_ref =
                    parse::value_ref_parser<int>();

                const parse::value_ref_parser_rule<double>::type& double_value_ref =
                    parse::value_ref_parser<double>();

                const parse::value_ref_parser_rule<std::string>::type& string_value_ref =
                    parse::value_ref_parser<std::string>();

                const parse::value_ref_parser_rule<PlanetType>::type& planet_type_value_ref =
                    parse::value_ref_parser<PlanetType>();

                const parse::value_ref_parser_rule<PlanetSize>::type& planet_size_value_ref =
                    parse::value_ref_parser<PlanetSize>();

                const parse::value_ref_parser_rule<StarType>::type& star_type_value_ref =
                    parse::value_ref_parser<StarType>();

                set_meter
                    =    tok.Set_
                    >>   parse::non_ship_part_meter_type_enum() [ _a = _1 ]
                    >    tok.Value_ > '='
                    >    double_value_ref [ _val = new_<Effect::SetMeter>(_a, _1) ]
                    ;

                set_ship_part_meter
                    =    tok.Set_
                    >    parse::ship_part_meter_type_enum() [ _a = _1 ]
                    >>   (
                              (
                                   tok.PartClass_ > '='
                               >   parse::enum_parser<ShipPartClass>() [ _b = _1 ]
                               >   tok.Value_ > '='
                               >   double_value_ref [ _e = _1 ]
                               >   tok.SlotType_ > '='
                               >   parse::enum_parser<ShipSlotType>() [ _val = new_<Effect::SetShipPartMeter>(_a, _b, _e, _1) ]
                              )
                          |   (
                                   tok.FighterType_ > '='
                               >   parse::enum_parser<CombatFighterType>() [ _c = _1 ]
                               >   tok.Value_ > '='
                               >   double_value_ref [ _e = _1 ]
                               >   tok.SlotType_ > '='
                               >   parse::enum_parser<ShipSlotType>() [ _val = new_<Effect::SetShipPartMeter>(_a, _c, _e, _1) ]
                              )
                          |   (
                                   tok.PartName_ > '='
                               >   tok.string [ _d = _1 ]
                               >   tok.Value_ > '='
                               >   double_value_ref [ _e = _1 ]
                               >   tok.SlotType_ > '='
                               >   parse::enum_parser<ShipSlotType>() [ _val = new_<Effect::SetShipPartMeter>(_a, _d, _e, _1) ]
                              )
                         )
                    ;

                set_empire_meter
                    =    tok.SetEmpireMeter_
                    >>   (
                              (
                                   tok.Empire_ > '='
                               >   int_value_ref [ _a = _1 ]
                               >   tok.Meter_ > '='
                               >   tok.string [ _b = _1 ]
                               >   tok.Value_ > '='
                               >   double_value_ref [ _val = new_<Effect::SetEmpireMeter>(_a, _b, _1) ]
                              )
                          |   (
                                   tok.Meter_ > '='
                               >   tok.string [ _b = _1 ]
                               >   tok.Value_ > '='
                               >   double_value_ref [ _val = new_<Effect::SetEmpireMeter>(_b, _1) ]
                              )
                         )
                    ;

                set_empire_stockpile
                    =    (
                              tok.SetEmpireFoodStockpile_ [ _a = RE_FOOD ]
                          |   tok.SetEmpireMineralStockpile_ [ _a = RE_MINERALS ]
                          |   tok.SetEmpireTradeStockpile_ [ _a = RE_TRADE ]
                         )
                    >>   (
                              (
                                   tok.Value_ > '='
                               >   double_value_ref [ _val = new_<Effect::SetEmpireStockpile>(_a, _1) ]
                              )
                          |   (
                                   tok.Empire_ > '='
                               >   int_value_ref [ _b = _1 ]
                               >   tok.Value_ > '='
                               >   double_value_ref [ _val = new_<Effect::SetEmpireStockpile>(_b, _a, _1) ]
                              )
                         )
                    ;

                set_empire_capital
                    =    tok.SetEmpireCapital_
                    >>   (
                              (
                                   tok.Empire_ > '='
                               >   int_value_ref [ _val = new_<Effect::SetEmpireCapital>(_1) ]
                              )
                          |   eps [ _val = new_<Effect::SetEmpireCapital>() ]
                         )
                    ;

                set_planet_type
                    =    tok.SetPlanetType_
                    >    tok.Type_ > '='
                    >    planet_type_value_ref [ _val = new_<Effect::SetPlanetType>(_1) ]
                    ;

                set_planet_size
                    =    tok.SetPlanetSize_
                    >    tok.PlanetSize_ > '='
                    >    planet_size_value_ref [ _val = new_<Effect::SetPlanetSize>(_1) ]
                    ;

                set_species
                    =    tok.SetSpecies_
                    >    tok.Name_ > '='
                    >    string_value_ref [ _val = new_<Effect::SetSpecies>(_1) ]
                    ;

                set_owner
                    =    tok.SetOwner_
                    >    tok.Empire_ > '='
                    >    int_value_ref [ _val = new_<Effect::SetOwner>(_1) ]
                    ;

                create_planet
                    =    tok.CreatePlanet_
                    >    tok.Type_ > '='
                    >    planet_type_value_ref [ _a = _1 ]
                    >    tok.Endpoint_ > '='  // TODO: Shouldn't this be PlanetSize?
                    >    planet_size_value_ref [ new_<Effect::CreatePlanet>(_a, _1) ]
                    ;

                create_building
                    =    tok.CreateBuilding_
                    >    tok.Name_ > '='
                    >    string_value_ref [ _val = new_<Effect::CreateBuilding>(_1) ]
                    ;

                create_ship
                    =    tok.CreateShip_
                    >>   (
                              tok.DesignName_ >> '='
                          >>  int_value_ref [ _b = _1 ]
                          >   tok.Empire_ > '='
                          >   int_value_ref [ _c = _1 ]
                          >   tok.Species_ > '='
                          >   string_value_ref [ _val = new_<Effect::CreateShip>(_b, _c, _1) ]
                         )
                    |    (
                              tok.DesignName_ >> '='
                          >>  tok.string [ _a = _1 ]
                          >>  tok.Empire_ >> '='
                          >>  int_value_ref [ _b = _1 ]
                          >>  tok.Species_ > '='
                          >   string_value_ref [ _val = new_<Effect::CreateShip>(_a, _b, _1) ]
                         )
                    |    (
                              tok.DesignName_ >> '='
                          >>  tok.string [ _a = _1 ]
                          >>  tok.Empire_ > '='
                          >   int_value_ref [ _val = new_<Effect::CreateShip>(_a, _1) ]
                         )
                    |    (
                              tok.DesignName_ > '='
                          >   tok.string [ _val = new_<Effect::CreateShip>(_1) ]
                         )
                    ;

                move_to
                    =    tok.MoveTo_
                    >    tok.Destination_ > '='
                    >    parse::detail::condition_parser [ _val = new_<Effect::MoveTo>(_1) ]
                    ;

                set_destination
                    =    tok.SetDestination_
                    >    tok.Destination_ > '='
                    >    parse::detail::condition_parser [ _val = new_<Effect::SetDestination>(_1) ]
                    ;

                destroy
                    =    tok.Destroy_ [ _val = new_<Effect::Destroy>() ]
                    ;

                victory
                    =    tok.Victory_
                    >    tok.Reason_ > '='
                    >    tok.string [ _val = new_<Effect::Victory>(_1) ]
                    ;

                add_special
                    =    tok.AddSpecial_
                    >    tok.Name_ > '='
                    >    tok.string [ _val = new_<Effect::AddSpecial>(_1) ]
                    ;

                remove_special
                    =    tok.RemoveSpecial_
                    >    tok.Name_ > '='
                    >    tok.string [ _val = new_<Effect::RemoveSpecial>(_1) ]
                    ;

                add_starlanes
                    =    tok.AddStarlanes_
                    >    tok.Endpoint_ > '='
                    >    parse::detail::condition_parser [ _val = new_<Effect::AddStarlanes>(_1) ]
                    ;

                remove_starlanes
                    =    tok.RemoveStarlanes_
                    >    tok.Endpoint_ > '='
                    >    parse::detail::condition_parser [ _val = new_<Effect::RemoveStarlanes>(_1) ]
                    ;

                set_star_type
                    =    tok.SetStarType_
                    >    tok.Type_ > '='
                    >    star_type_value_ref [ _val = new_<Effect::SetStarType>(_1) ]
                    ;

                set_tech_availability
                    =    (
                              tok.GiveTechToOwner_ [ _a = true, _b = true ]
                          |   tok.RevokeTechFromOwner_ [ _a = false, _b = true ]
                          |   tok.UnlockTechItemsForOwner_ [ _a = true, _b = false ]
                          |   tok.LockTechItemsForOwner_ [ _a = false, _b = false ]
                         )
                    >    tok.Name_ > '='
                    >    tok.string
                         [ _val = new_<Effect::SetTechAvailability>(_1, new_<ValueRef::Variable<int> >(ValueRef::EFFECT_TARGET_REFERENCE, "Owner"), _a, _b) ]
                    ;

                generate_sitrep_message
                    =    tok.GenerateSitrepMessage_
                    >    tok.Message_ > '='
                    >    tok.string [ _a = _1 ]
                    >> - (
                              tok.Parameters_ > '='
                          >   string_and_string_ref_vector [ _b = _1 ]
                         )
                    >>   (
                              (
                                   tok.Affiliation_ >> '='
                               >>  parse::enum_parser<EmpireAffiliationType>() [ _c = _1 ]
                               |   eps [ _c = AFFIL_SELF ]
                              )
                          >>  tok.Empire_ > '='
                          >   int_value_ref [ _val = new_<Effect::GenerateSitRepMessage>(_a, _b, _1, _c) ]
                         )
                    |    (
                              tok.Affiliation_ > '='
                          >   parse::enum_parser<EmpireAffiliationType>() [ _c = _1 ]
                          |   eps [ _c = AFFIL_ANY ]
                         )
                         [ _val = new_<Effect::GenerateSitRepMessage>(_a, _b, _c) ]
                    ;

                string_and_string_ref // TODO: Try to make this simpler.
                    =    tok.Tag_ > '='
                    >    tok.string [ _a = _1 ]
                    >    tok.Label_ > '='
                    >    string_value_ref [ _val = construct<string_and_string_ref_pair>(_a, _1) ]
                    ;

                string_and_string_ref_vector
                    =    (
                             '['
                          > +string_and_string_ref [ push_back(_val, _1) ]
                          >  ']'
                         )
                    |    string_and_string_ref [ push_back(_val, _1) ]
                    ;

                start
                    %=   set_meter
                    |    set_ship_part_meter
                    |    set_empire_meter
                    |    set_empire_stockpile
                    |    set_empire_capital
                    |    set_planet_type
                    |    set_planet_size
                    |    set_species
                    |    set_owner
                    |    create_planet
                    |    create_building
                    |    create_ship
                    |    move_to
                    |    set_destination
                    |    destroy
                    |    victory
                    |    add_special
                    |    remove_special
                    |    add_starlanes
                    |    remove_starlanes
                    |    set_star_type
                    |    set_tech_availability
                    |    generate_sitrep_message
                    ;
            }

        typedef boost::spirit::qi::rule<
            parse::token_iterator,
            Effect::EffectBase* (),
            qi::locals<MeterType>,
            parse::skipper_type
        > set_meter_rule;

        typedef boost::spirit::qi::rule<
            parse::token_iterator,
            Effect::EffectBase* (),
            qi::locals<
                MeterType,
                ShipPartClass,
                CombatFighterType,
                std::string,
                ValueRef::ValueRefBase<double>*
            >,
            parse::skipper_type
        > set_ship_part_meter_rule;

        typedef boost::spirit::qi::rule<
            parse::token_iterator,
            Effect::EffectBase* (),
            qi::locals<
                ValueRef::ValueRefBase<int>*,
                std::string
            >,
            parse::skipper_type
        > set_empire_meter_rule;

        typedef boost::spirit::qi::rule<
            parse::token_iterator,
            Effect::EffectBase* (),
            qi::locals<
                ResourceType,
                ValueRef::ValueRefBase<int>*
            >,
            parse::skipper_type
        > set_empire_stockpile_rule;

        typedef boost::spirit::qi::rule<
            parse::token_iterator,
            Effect::EffectBase* (),
            qi::locals<ValueRef::ValueRefBase< ::PlanetType>*>,
            parse::skipper_type
        > create_planet_rule;

        typedef boost::spirit::qi::rule<
            parse::token_iterator,
            Effect::EffectBase* (),
            qi::locals<
                std::string,
                ValueRef::ValueRefBase<int>*,
                ValueRef::ValueRefBase<int>*
            >,
            parse::skipper_type
        > create_ship_rule;

        typedef boost::spirit::qi::rule<
            parse::token_iterator,
            Effect::EffectBase* (),
            qi::locals<
                bool,
                bool
            >,
            parse::skipper_type
        > set_tech_availability_rule;

        typedef boost::spirit::qi::rule<
            parse::token_iterator,
            Effect::EffectBase* (),
            qi::locals<
                std::string,
                std::vector<std::pair<std::string, const ValueRef::ValueRefBase<std::string>*> >,
                EmpireAffiliationType
            >,
            parse::skipper_type
        > generate_sitrep_message_rule;

        typedef std::pair<std::string, const ValueRef::ValueRefBase<std::string>*> string_and_string_ref_pair;

        typedef boost::spirit::qi::rule<
            parse::token_iterator,
            string_and_string_ref_pair (),
            qi::locals<std::string>, // TODO: Consider making this an adobe::name_t, and removing the quotes in the script source files.
            parse::skipper_type
        > string_and_string_ref_rule;

        typedef boost::spirit::qi::rule<
            parse::token_iterator,
            std::vector<string_and_string_ref_pair> (),
            parse::skipper_type
        > string_and_string_ref_vector_rule;

        set_meter_rule set_meter;
        set_ship_part_meter_rule set_ship_part_meter;
        set_empire_meter_rule set_empire_meter;
        set_empire_stockpile_rule set_empire_stockpile;
        parse::effect_parser_rule set_empire_capital;
        parse::effect_parser_rule set_planet_type;
        parse::effect_parser_rule set_planet_size;
        parse::effect_parser_rule set_species;
        parse::effect_parser_rule set_owner;
        create_planet_rule create_planet;
        parse::effect_parser_rule create_building;
        create_ship_rule create_ship;
        parse::effect_parser_rule move_to;
        parse::effect_parser_rule set_destination;
        parse::effect_parser_rule destroy;
        parse::effect_parser_rule victory;
        parse::effect_parser_rule add_special;
        parse::effect_parser_rule remove_special;
        parse::effect_parser_rule add_starlanes;
        parse::effect_parser_rule remove_starlanes;
        parse::effect_parser_rule set_star_type;
        set_tech_availability_rule set_tech_availability;
        generate_sitrep_message_rule generate_sitrep_message;
        string_and_string_ref_rule string_and_string_ref;
        string_and_string_ref_vector_rule string_and_string_ref_vector;
        parse::effect_parser_rule start;
    };

}

namespace parse {

    const effect_parser_rule& effect_parser()
    {
        static effect_parser_rules retval;
        return retval.start;
    }

}
