// -*- C++ -*-
#ifndef _Lexer_h_
#define _Lexer_h_

#include <GG/Token.h>
#include <GG/adobe/name.hpp>

#include <boost/spirit/include/lex_lexertl.hpp>

#include "../universe/Enums.h"
#include "../universe/ValueRefFwd.h"


namespace parse {

typedef std::string::const_iterator text_iterator;

typedef GG::position_tracking_token<
    text_iterator,
    boost::mpl::vector<
        bool,
        int,
        double,
        PlanetSize,
        PlanetType,
        PlanetEnvironment,
        UniverseObjectType,
        StarType,
        EmpireAffiliationType,
        UnlockableItemType,
        TechType,
        CombatFighterType,
        ShipPartClass,
        ShipSlotType,
        CaptureResult,
        ValueRef::StatisticType,
        adobe::name_t,
        std::string
    >
> token_type;

typedef boost::spirit::lex::lexertl::lexer<token_type> spirit_lexer_base_type;

struct lexer :
    boost::spirit::lex::lexer<spirit_lexer_base_type>
{
    lexer();

    boost::spirit::lex::token_def<boost::spirit::lex::omit> inline_comment;
    boost::spirit::lex::token_def<boost::spirit::lex::omit> end_of_line_comment;

    boost::spirit::lex::token_def<bool> bool_;
    boost::spirit::lex::token_def<int> int_;
    boost::spirit::lex::token_def<double> double_;
    boost::spirit::lex::token_def<std::string> string;

    boost::spirit::lex::token_def<PlanetSize> planet_size_enum;
    boost::spirit::lex::token_def<PlanetType> planet_type_enum;
    boost::spirit::lex::token_def<PlanetEnvironment> planet_environment_enum;
    boost::spirit::lex::token_def<UniverseObjectType> universe_object_type;
    boost::spirit::lex::token_def<StarType> star_type_enum;
    boost::spirit::lex::token_def<EmpireAffiliationType> empire_affiliation_type;
    boost::spirit::lex::token_def<UnlockableItemType> unlockable_item_type;
    boost::spirit::lex::token_def<TechType> tech_type_enum;
    boost::spirit::lex::token_def<CombatFighterType> combat_fighter_type;
    boost::spirit::lex::token_def<ShipPartClass> ship_part_class;
    boost::spirit::lex::token_def<ShipSlotType> ship_slot_type;
    boost::spirit::lex::token_def<CaptureResult> capture_result_enum;

    // Many of these must be tokenized separately, since they are overloaded
    // (e.g. building could be an UniverseObjectType or an
    // UnlockableItemType).
    boost::spirit::lex::token_def<adobe::name_t> fleet;
    boost::spirit::lex::token_def<adobe::name_t> monster_fleet;
    boost::spirit::lex::token_def<adobe::name_t> planet;
    boost::spirit::lex::token_def<adobe::name_t> system;
    boost::spirit::lex::token_def<boost::spirit::lex::omit> asteroids;
    boost::spirit::lex::token_def<boost::spirit::lex::omit> gas_giant;
    boost::spirit::lex::token_def<boost::spirit::lex::omit> building;
    boost::spirit::lex::token_def<adobe::name_t> fuel;
    boost::spirit::lex::token_def<adobe::name_t> shield;
    boost::spirit::lex::token_def<adobe::name_t> troops;
    boost::spirit::lex::token_def<adobe::name_t> stealth;
    boost::spirit::lex::token_def<adobe::name_t> detection;
    boost::spirit::lex::token_def<adobe::name_t> battle_speed;
    boost::spirit::lex::token_def<adobe::name_t> starlane_speed;
    boost::spirit::lex::token_def<adobe::name_t> scope;
    boost::spirit::lex::token_def<adobe::name_t> activation;
    boost::spirit::lex::token_def<adobe::name_t> stacking_group;
    boost::spirit::lex::token_def<adobe::name_t> effects;
    boost::spirit::lex::token_def<adobe::name_t> name;
    boost::spirit::lex::token_def<adobe::name_t> description;
    boost::spirit::lex::token_def<adobe::name_t> short_description;
    boost::spirit::lex::token_def<adobe::name_t> build_cost;
    boost::spirit::lex::token_def<adobe::name_t> build_time;
    boost::spirit::lex::token_def<adobe::name_t> capture_result;
    boost::spirit::lex::token_def<adobe::name_t> effects_group;
    boost::spirit::lex::token_def<adobe::name_t> effects_groups;
    boost::spirit::lex::token_def<adobe::name_t> graphic;
    boost::spirit::lex::token_def<adobe::name_t> tech_type;

    boost::spirit::lex::token_def<adobe::name_t> special;
    boost::spirit::lex::token_def<adobe::name_t> species;
    boost::spirit::lex::token_def<adobe::name_t> building_type;
    boost::spirit::lex::token_def<adobe::name_t> focus;
    boost::spirit::lex::token_def<adobe::name_t> planetbound;
    boost::spirit::lex::token_def<adobe::name_t> spacebound;
    boost::spirit::lex::token_def<adobe::name_t> item;
    boost::spirit::lex::token_def<adobe::name_t> tech;
    boost::spirit::lex::token_def<adobe::name_t> part;
    boost::spirit::lex::token_def<adobe::name_t> shipdesign;
    boost::spirit::lex::token_def<adobe::name_t> alignment;
    boost::spirit::lex::token_def<adobe::name_t> low;
    boost::spirit::lex::token_def<adobe::name_t> high;
    boost::spirit::lex::token_def<adobe::name_t> number;
    boost::spirit::lex::token_def<adobe::name_t> empire;
    boost::spirit::lex::token_def<adobe::name_t> affiliation;
    boost::spirit::lex::token_def<adobe::name_t> type;
    boost::spirit::lex::token_def<adobe::name_t> size;
    boost::spirit::lex::token_def<adobe::name_t> probability;
    boost::spirit::lex::token_def<adobe::name_t> distance;
    boost::spirit::lex::token_def<adobe::name_t> jumps;
    boost::spirit::lex::token_def<adobe::name_t> sortby;
    boost::spirit::lex::token_def<adobe::name_t> design;
    boost::spirit::lex::token_def<adobe::name_t> all;
    boost::spirit::lex::token_def<adobe::name_t> ownedby;
    boost::spirit::lex::token_def<adobe::name_t> stationary;
    boost::spirit::lex::token_def<adobe::name_t> homeworld;
    boost::spirit::lex::token_def<adobe::name_t> capital;
    boost::spirit::lex::token_def<adobe::name_t> monster;
    boost::spirit::lex::token_def<adobe::name_t> armed;
    boost::spirit::lex::token_def<adobe::name_t> and_;
    boost::spirit::lex::token_def<adobe::name_t> or_;
    boost::spirit::lex::token_def<adobe::name_t> not_;
    boost::spirit::lex::token_def<adobe::name_t> id;
    boost::spirit::lex::token_def<adobe::name_t> added_since;
    boost::spirit::lex::token_def<adobe::name_t> added_before;
    boost::spirit::lex::token_def<adobe::name_t> owner_has_tech;
    boost::spirit::lex::token_def<adobe::name_t> has_special;
    boost::spirit::lex::token_def<adobe::name_t> design_has_hull;
    boost::spirit::lex::token_def<adobe::name_t> design_has_part;
    boost::spirit::lex::token_def<adobe::name_t> design_has_part_class;
    boost::spirit::lex::token_def<adobe::name_t> produced_by_empire;
    boost::spirit::lex::token_def<adobe::name_t> visible_to_empire;
    boost::spirit::lex::token_def<adobe::name_t> explored_by_empire;
    boost::spirit::lex::token_def<adobe::name_t> fleet_supplyable_by_empire;
    boost::spirit::lex::token_def<adobe::name_t> in_system;
    boost::spirit::lex::token_def<adobe::name_t> object;
    boost::spirit::lex::token_def<adobe::name_t> within_distance;
    boost::spirit::lex::token_def<adobe::name_t> within_starlane_jumps;
    boost::spirit::lex::token_def<adobe::name_t> turn;
    boost::spirit::lex::token_def<adobe::name_t> created_on_turn;
    boost::spirit::lex::token_def<adobe::name_t> number_of;
    boost::spirit::lex::token_def<adobe::name_t> maximum_number_of;
    boost::spirit::lex::token_def<adobe::name_t> minimum_number_of;
    boost::spirit::lex::token_def<adobe::name_t> mode_number_of;
    boost::spirit::lex::token_def<adobe::name_t> contains;
    boost::spirit::lex::token_def<adobe::name_t> contained_by;
    boost::spirit::lex::token_def<adobe::name_t> star;
    boost::spirit::lex::token_def<adobe::name_t> random;
    boost::spirit::lex::token_def<adobe::name_t> owner_food_stockpile;
    boost::spirit::lex::token_def<adobe::name_t> owner_mineral_stockpile;
    boost::spirit::lex::token_def<adobe::name_t> owner_trade_stockpile;
    boost::spirit::lex::token_def<adobe::name_t> resource_supply_connected_by_empire;

    boost::spirit::lex::token_def<adobe::name_t> farming;
    boost::spirit::lex::token_def<adobe::name_t> target_farming;
    boost::spirit::lex::token_def<adobe::name_t> industry;
    boost::spirit::lex::token_def<adobe::name_t> target_industry;
    boost::spirit::lex::token_def<adobe::name_t> research;
    boost::spirit::lex::token_def<adobe::name_t> target_research;
    boost::spirit::lex::token_def<adobe::name_t> trade;
    boost::spirit::lex::token_def<adobe::name_t> target_trade;
    boost::spirit::lex::token_def<adobe::name_t> mining;
    boost::spirit::lex::token_def<adobe::name_t> target_mining;
    boost::spirit::lex::token_def<adobe::name_t> construction;
    boost::spirit::lex::token_def<adobe::name_t> target_construction;
    boost::spirit::lex::token_def<adobe::name_t> population;
    boost::spirit::lex::token_def<adobe::name_t> target_population;
    boost::spirit::lex::token_def<adobe::name_t> target_health;
    boost::spirit::lex::token_def<adobe::name_t> max_fuel;
    boost::spirit::lex::token_def<adobe::name_t> max_shield;
    boost::spirit::lex::token_def<adobe::name_t> max_defense;
    boost::spirit::lex::token_def<adobe::name_t> defense;
    boost::spirit::lex::token_def<adobe::name_t> max_troops;
    boost::spirit::lex::token_def<adobe::name_t> max_structure;
    boost::spirit::lex::token_def<adobe::name_t> supply;
    boost::spirit::lex::token_def<adobe::name_t> food_consumption;

    boost::spirit::lex::token_def<adobe::name_t> trade_stockpile;
    boost::spirit::lex::token_def<adobe::name_t> mineral_stockpile;
    boost::spirit::lex::token_def<adobe::name_t> food_stockpile;
    boost::spirit::lex::token_def<adobe::name_t> distance_to_source;

    boost::spirit::lex::token_def<adobe::name_t> category;
    boost::spirit::lex::token_def<adobe::name_t> research_cost;
    boost::spirit::lex::token_def<adobe::name_t> research_turns;
    boost::spirit::lex::token_def<adobe::name_t> prerequisites;
    boost::spirit::lex::token_def<adobe::name_t> unlock;
    boost::spirit::lex::token_def<adobe::name_t> environment;
    boost::spirit::lex::token_def<adobe::name_t> environments;
    boost::spirit::lex::token_def<adobe::name_t> foci;
    boost::spirit::lex::token_def<adobe::name_t> location;
    boost::spirit::lex::token_def<adobe::name_t> class_;
    boost::spirit::lex::token_def<adobe::name_t> speed;
    boost::spirit::lex::token_def<adobe::name_t> slot;
    boost::spirit::lex::token_def<adobe::name_t> slots;
    boost::spirit::lex::token_def<adobe::name_t> mountable_slot_types;
    boost::spirit::lex::token_def<adobe::name_t> colour;
    boost::spirit::lex::token_def<adobe::name_t> position;
    boost::spirit::lex::token_def<adobe::name_t> damage;
    boost::spirit::lex::token_def<adobe::name_t> anti_ship_damage;
    boost::spirit::lex::token_def<adobe::name_t> anti_fighter_damage;
    boost::spirit::lex::token_def<adobe::name_t> rof;
    boost::spirit::lex::token_def<adobe::name_t> range;
    boost::spirit::lex::token_def<adobe::name_t> fighter_weapon_range;
    boost::spirit::lex::token_def<adobe::name_t> health;
    boost::spirit::lex::token_def<adobe::name_t> structure;
    boost::spirit::lex::token_def<adobe::name_t> fighter_type;
    boost::spirit::lex::token_def<adobe::name_t> launch_rate;
    boost::spirit::lex::token_def<adobe::name_t> capacity;
    boost::spirit::lex::token_def<adobe::name_t> hull;
    boost::spirit::lex::token_def<adobe::name_t> parts;
    boost::spirit::lex::token_def<adobe::name_t> ships;
    boost::spirit::lex::token_def<adobe::name_t> model;
    boost::spirit::lex::token_def<adobe::name_t> lookup_strings;
    boost::spirit::lex::token_def<adobe::name_t> researchable;
    boost::spirit::lex::token_def<adobe::name_t> producible;
    boost::spirit::lex::token_def<adobe::name_t> spawn_rate;
    boost::spirit::lex::token_def<adobe::name_t> spawn_limit;
    boost::spirit::lex::token_def<adobe::name_t> property;
    boost::spirit::lex::token_def<adobe::name_t> condition;
    boost::spirit::lex::token_def<adobe::name_t> source;
    boost::spirit::lex::token_def<adobe::name_t> target;
    boost::spirit::lex::token_def<adobe::name_t> localcandidate;
    boost::spirit::lex::token_def<adobe::name_t> rootcandidate;
    boost::spirit::lex::token_def<adobe::name_t> value;

    boost::spirit::lex::token_def<ValueRef::StatisticType> statistic_type;

    boost::spirit::lex::token_def<adobe::name_t> int_variable_final;
    boost::spirit::lex::token_def<adobe::name_t> planet_size;
    boost::spirit::lex::token_def<adobe::name_t> planet_type;
    boost::spirit::lex::token_def<adobe::name_t> next_better_planet_type;
    boost::spirit::lex::token_def<adobe::name_t> planet_environment;
    boost::spirit::lex::token_def<adobe::name_t> object_type;
    boost::spirit::lex::token_def<adobe::name_t> star_type;
};

typedef lexer::iterator_type token_iterator;

typedef lexer::lexer_def lexer_def;

typedef boost::spirit::qi::in_state_skipper<lexer_def> skipper_type;

}

namespace boost { namespace spirit { namespace traits {

#define ASSIGN_TO_ATTRIBUTE_DECL(T)                                          \
    template <>                                                              \
    struct assign_to_attribute_from_iterators<T, parse::text_iterator, void> \
    { static void call(const parse::text_iterator& first, const parse::text_iterator& last, T& attr); }

    // These template specializations are required by Spirit.Lex to automatically
    // convert an iterator pair to an adobe::name_t in the lexer.
    ASSIGN_TO_ATTRIBUTE_DECL(adobe::name_t);

    // HACK! This is only necessary because of a bug in Spirit in Boost
    // versions <= 1.45.
    ASSIGN_TO_ATTRIBUTE_DECL(bool);

    ASSIGN_TO_ATTRIBUTE_DECL(PlanetSize);
    ASSIGN_TO_ATTRIBUTE_DECL(PlanetType);
    ASSIGN_TO_ATTRIBUTE_DECL(PlanetEnvironment);
    ASSIGN_TO_ATTRIBUTE_DECL(UniverseObjectType);
    ASSIGN_TO_ATTRIBUTE_DECL(StarType);
    ASSIGN_TO_ATTRIBUTE_DECL(EmpireAffiliationType);
    ASSIGN_TO_ATTRIBUTE_DECL(UnlockableItemType);
    ASSIGN_TO_ATTRIBUTE_DECL(TechType);
    ASSIGN_TO_ATTRIBUTE_DECL(CombatFighterType);
    ASSIGN_TO_ATTRIBUTE_DECL(ShipPartClass);
    ASSIGN_TO_ATTRIBUTE_DECL(ShipSlotType);
    ASSIGN_TO_ATTRIBUTE_DECL(CaptureResult);
    ASSIGN_TO_ATTRIBUTE_DECL(ValueRef::StatisticType);

#undef ASSIGN_TO_ATTRIBUTE_DECL

} } }

#endif
