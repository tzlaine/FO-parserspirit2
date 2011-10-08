#include "EffectParser.h"

#include "../universe/Effect.h"

#include <GG/ReportParseError.h>

#include <boost/spirit/home/phoenix.hpp>


namespace qi = boost::spirit::qi;
namespace phoenix = boost::phoenix;


#define NAME(x) x.name(#x); debug(x)

namespace {

    struct effect_parser_rules
    {
        effect_parser_rules(const parse::lexer& tok)
            {
                using qi::_1;
                using qi::_a;
                using qi::_b;
                using qi::_c;
                using qi::_d;
                using qi::_e;
                using qi::_val;
                using phoenix::new_;

                set_meter
                    =    tok.set
                    >    enum_parser<MeterType>() [ _a = _1 ] // TODO: Only non-ship meter types.
                    >    tok.value > '='
                    >    tok.double_ [ _val = new_<Effect::SetMeter>(_a, _1) ]
                    ;

                set_ship_part_meter
                    =    tok.set
                    >    enum_parser<MeterType>() [ _a = _1 ] // TODO: Only non-ship meter types.
                    >>   (
                              (
                                   tok.part_class > '='
                               >   enum_parser<ShipPartClass>() [ _b = _1 ]
                               >   tok.value > '='
                               >   double_value_ref [ _e = _1 ]
                               >   tok.slot_type > '='
                               >   enum_parser<ShipSlotType>() [ _val = new_<Effect::SetShipPartMeter>(_a, _b, _e, _1) ]
                              )
                          |   (
                                   tok.fighter_type > '='
                               >   enum_parser<CombatFighterType>() [ _c = _1 ]
                               >   tok.value > '='
                               >   double_value_ref [ _e = _1 ]
                               >   tok.slot_type > '='
                               >   enum_parser<ShipSlotType>() [ _val = new_<Effect::SetShipPartMeter>(_a, _c, _e, _1) ]
                              )
                          |   (
                                   tok.part_name > '='
                               >   tok.string [ _d = _1 ]
                               >   tok.value > '='
                               >   double_value_ref [ _e = _1 ]
                               >   tok.slot_type > '='
                               >   enum_parser<ShipSlotType>() [ _val = new_<Effect::SetShipPartMeter>(_a, _d, _e, _1) ]
                              )
                         )
                    ;

                set_empire_meter
                    =    tok.set_empire_meter
                    >>   (
                              (
                                   tok.empire > '='
                               >   int_value_ref [ _a = _1 ]
                               >   tok.meter > '='
                               >   tok.string [ _b = _1 ]
                               >   tok.value > '='
                               >   double_value_ref [ _val = new_<Effect::SetEmpireMeter>(_a, _b, _1) ]
                              )
                          |   (
                                   tok.meter > '='
                               >   tok.string [ _b = _1 ]
                               >   tok.value > '='
                               >   double_value_ref [ _val = new_<Effect::SetEmpireMeter>(_b, _1) ]
                              )
                         )
                    ;

                set_empire_stockpile
                    =    (
                              tok.set_empire_food_stockpile [ _a = RE_FOOD ]
                          |   tok.set_empire_mineral_stockpile [ _a = RE_MINERALS ]
                          |   tok.set_empire_trade_stockpile [ _a = RE_TRADE ]
                         )
                    >>   (
                              tok.value > '='
                          >   double_value_ref [ _val = new_<Effect::SetEmpireStockpile>(_a, _1) ]
                         )
                    |    (
                              tok.empire > '='
                          >   int_value_ref [ _b = _1 ]
                          >   tok.value > '='
                          >   double_value_ref [ _val = new_<Effect::SetEmpireStockpile>(_b, _a, _1) ]
                         )
                    ;

                set_empire_capital
                    =    tok.set_empire_capital
                    >>   (
                              tok.empire > '='
                          |   int_value_ref [ _val = new_<Effect::SetEmpireCapital>(_1) ]
                         )
                    |    eps [ _val = new_<Effect::SetEmpireCapital>() ]
                    ;

                set_planet_type
                    =    tok.set_planet_type
                    >    tok.type > '='
                    >    planet_type_value_ref [ _val = new_<Effect::SetPlanetType>(_1) ]
                    ;

                set_planet_size
                    =    tok.set_planet_size
                    >    tok.size > '='
                    >    planet_size_value_ref [ _val = new_<Effect::SetPlanetSize>(_1) ]
                    ;

                set_species
                    =    tok.set_species
                    >    tok.name > '='
                    >    string_value_ref [ _val = new_<Effect::SetSpecies>(_1) ]
                    ;

                set_owner
                    =    tok.set_owner
                    >    tok.empire > '='
                    >    int_value_ref [ _val = new_<Effect::SetOwner>(_1) ]
                    ;

                create_planet
                    =    tok.create_planet
                    >    tok.type > '='
                    >    planet_type_value_ref [ _a = _1 ]
                    >    tok.endpoint > '='
                    >    planet_type_value_ref [ new_<Effect::CreatePlanet>(_a, _1) ]
                    ;

                create_building
                    =    tok.create_building
                    >    tok.name > '='
                    >    tok.string [ _val = new_<Effect::CreateBuilding>(_1) ]
                    ;

                create_ship
                    =    tok.create_ship
                    >>   (
                              tok.design_name >> '='
                          >>  int_value_ref [ _b = _1 ]
                          >   tok.empire > '='
                          >   int_value_ref [ _c = _1 ]
                          >   tok.species > '='
                          >   string_value_ref [ _val = new_<Effect::CreateShip>(_b, _c, _1) ]
                         )
                    |    (
                              tok.design_name >> '='
                          >>  tok.string [ _a = _1 ]
                          >>  tok.empire >> '='
                          >>  int_value_ref [ _b = _1 ]
                          >>  tok.species > '='
                          >   string_value_ref [ _val = new_<Effect::CreateShip>(_a, _b, _1) ]
                         )
                    |    (
                              tok.design_name >> '='
                          >>  tok.string [ _a = _1 ]
                          >>  tok.empire > '='
                          >   string_value_ref [ _val = new_<Effect::CreateShip>(_a, _1) ]
                         )
                    |    (
                              tok.design_name > '='
                          >  tok.string [ _val = new_<Effect::CreateShip>(_1) ]
                         )
                    ;

                move_to
                    =    tok.move_to
                    >    tok.destination > '='
                    >    condition_parser(tok) [ _val = new_<Effect::MoveTo>(_1) ]
                    ;

                set_destination
                    =    tok.set_destination
                    >    tok.destination > '='
                    >    condition_parser(tok) [ _val = new_<Effect::SetDestination>(_1) ]
                    ;

                destroy
                    =    tok.destroy [ _val = new_<Effect::Destroy>() ]
                    ;

                victory
                    =    tok.victory
                    >    tok.reason > '='
                    >    string_value_ref [ _val = new_<Effect::Victory>(_1) ]
                    ;

                add_special
                    =    tok.add_special
                    >    tok.name > '='
                    >    string_value_ref [ _val = new_<Effect::AddSpecial>(_1) ]
                    ;

                remove_special
                    =    tok.remove_special
                    >    tok.name > '='
                    >    string_value_ref [ _val = new_<Effect::RemoveSpecial>(_1) ]
                    ;

                add_starlanes
                    =    tok.add_starlanes
                    >    tok.endpoint > '='
                    >    condition_parser(tok) [ _val = new_<Effect::AddStarlanes>(_1) ]
                    ;

                remove_starlanes
                    =    tok.remove_starlanes
                    >    tok.endpoint > '='
                    >    condition_parser(tok) [ _val = new_<Effect::RemoveStarlanes>(_1) ]
                    ;

                set_star_type
                    =    tok.set_star_type
                    >    tok.type > '='
                    >    star_type_value_ref [ _val = new_<Effect::SetStarType>(_1) ]
                    ;

                set_tech_availability
                    =    (
                              tok.give_tech_to_owner [ _a = true, _b = true ]
                          |   tok.revoke_tech_from_owner [ _a = false, _b = true ]
                          |   tok.unlock_tech_items_for_owner [ _a = true, _b = false ]
                          |   tok.lock_tech_items_for_owner [ _a = false, _b = false ]
                         )
                    >    tok.name > '='
                    >    tok.string
                         [ _val = new_<Effect::SetTechAvailability>(_1, new_<ValueRef::Variable<int> >(ValueRef::EFFECT_TARGET_REFERENCE, "Owner"), _a, _b) ]
                    ;

        generate_sitrep_message =
            (str_p("generatesitrepmessage")
             >> message_label >>                name_p[generate_sitrep_message.template_string = arg1]
             >> !(parameters_label >>           string_and_string_ref_vector[generate_sitrep_message.parameters = arg1])
             >> (affiliation_label >>           affiliation_type_p[generate_sitrep_message.affiliation = arg1]
                 |                              eps_p[generate_sitrep_message.affiliation = val(AFFIL_SELF)]
             >> empire_label >>                 int_expr_p[generate_sitrep_message.recipient_empire = arg1])
             [generate_sitrep_message.this_ = new_<Effect::GenerateSitRepMessage>(generate_sitrep_message.template_string,
                                                                                  generate_sitrep_message.parameters,
                                                                                  generate_sitrep_message.recipient_empire,
                                                                                  generate_sitrep_message.affiliation)])
            | (str_p("generatesitrepmessage")
               >> message_label >>              name_p[generate_sitrep_message.template_string = arg1]
               >> !(parameters_label >>         string_and_string_ref_vector[generate_sitrep_message.parameters = arg1])
               >> (affiliation_label >>         affiliation_type_p[generate_sitrep_message.affiliation = arg1]
                   |                            eps_p[generate_sitrep_message.affiliation = val(AFFIL_ANY)])
               [generate_sitrep_message.this_ = new_<Effect::GenerateSitRepMessage>(generate_sitrep_message.template_string,
                                                                                    generate_sitrep_message.parameters,
                                                                                    generate_sitrep_message.affiliation)]);

                generate_sitrep_message
                    =    tok.generate_sitrep_message
                    >    tok.message > '='
                    >    tok.string [ _a = _1 ]
                    >> - (
                              tok.parameters > '='
                          >   string_and_string_ref_vector [ _b = _1 ]
                         )
                    >>   (
                              (
                                   affiliation >> '='
                               >>  enum_parser<EmpireAffiliationType>() [ _c = _1 ]
                               |   eps [ _c = AFFIL_SELF ]
                              )
                          >>  tok.empire > '='
                          >   int_value_ref [ _val = new_<Effect::GenerateSitRepMessage>(_a, _b, _1, _c) ]
                         )
                         )
                    |    (
                              affiliation > '='
                          >   enum_parser<EmpireAffiliationType>() [ _c = _1 ]
                          |   eps [ _c = AFFIL_ANY ]
                         )
                         [ _val = new_<Effect::GenerateSitRepMessage>(_a, _b, _c) ]
                    ;

                string_and_string_ref_vector
                    =    
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
        parse::effect_parser_rule start;
    };

}

namespace parse {

    const effect_parser_rule& effect_parser(const parse::lexer& tok)
    {
        static effect_parser_rules retval(tok);
        return retval.start;
    }

}
