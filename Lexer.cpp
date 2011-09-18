#include "Lexer.h"

#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/assign/list_of.hpp>


namespace {

    // NOTE: The use of parse_into_strings() and find_match() is a bit
    // brittle.  The pair of them rely on the enum string listing the
    // enumerators in the same order that they are listed in the actual
    // enumeration's definition, with no gaps.  If this is not the case, a
    // custom mapping function must be used.  See
    // assign_to_attribute_from_iterators<UniverseObjectType, ...>::call()
    // near the end of the file for an example.

    std::vector<std::string> parse_into_strings(const std::string& string)
    {
        std::vector<std::string> retval;
        std::string strings(string.begin() + 4, string.end() - 1);
        boost::algorithm::split(retval, strings, boost::algorithm::is_any_of("|"));
        return retval;
    }

    long find_match(const std::vector<std::string>& strings, const parse::text_iterator& first, const parse::text_iterator& last)
    {
        long retval = -1;
        std::string match(first, last);
        boost::algorithm::to_lower(match);
        std::vector<std::string>::const_iterator it = std::find(strings.begin(), strings.end(), match);
        if (it != strings.end())
            retval = it - strings.begin();
        return retval;
    }

    const std::string planet_size_str("(?i:tiny|small|medium|large|huge)");
    const PlanetSize first_planet_size = SZ_TINY;
    const std::string planet_type_str("(?i:swamp|toxic|inferno|radiated|barren|tundra|desert|terran|ocean)");
    const PlanetType first_planet_type = PT_SWAMP;
    const std::string planet_environment_str("(?i:uninhabitable|hostile|poor|adequate|good)");
    const PlanetEnvironment first_planet_environment = PE_UNINHABITABLE;
    const std::string universe_object_type_str("(?i:ship|populationcenter|productioncenter)");
    const UniverseObjectType first_universe_object_type = OBJ_SHIP;
    const std::string star_type_str("(?i:blue|white|yellow|orange|red|neutron|blackhole|nostar)");
    const StarType first_star_type = STAR_BLUE;
    const std::string empire_affiliation_type_str("(?i:theempire|enemyof|allyof|anyempire)");
    const EmpireAffiliationType first_empire_affiliation_type = AFFIL_SELF;
    const std::string unlockable_item_type_str("(?i:shippart|shiphull)");
    const UnlockableItemType first_unlockable_item_type = UIT_SHIP_PART;
    const std::string tech_type_str("(?i:theory|application|refinement)");
    const TechType first_tech_type = TT_THEORY;
    const std::string combat_fighter_type_str("(?i:interceptor|bomber)");
    const CombatFighterType first_combat_fighter_type = INTERCEPTOR;
    const std::string ship_part_class_str("(?i:shortrange|missiles|fighters|pointdefense|armour|colony)");
    const ShipPartClass first_ship_part_class = PC_SHORT_RANGE;
    const std::string ship_slot_type_str("(?i:external|internal)");
    const ShipSlotType first_ship_slot_type = SL_EXTERNAL;
    const std::string capture_result_str("(?i:capture|destroy|retain)");
    const CaptureResult first_capture_result = CR_CAPTURE;
    const std::string statistic_type_str("(?i:count|sum|mean|rms|mode|max|min|spread|stdev|product)");
    ValueRef::StatisticType first_statistic_type = ValueRef::COUNT;
}

using namespace parse;

lexer::lexer() :
    inline_comment("\\/\\*[^*]*\\*+([^/*][^*]*\\*+)*\\/"),
    end_of_line_comment("\\/\\/.*$"),

    bool_("(?i:true|false)"),
    int_("\\d+"),
    double_("\\d+\\.\\d*"),
    string("\\\"[^\\\"]*\\\""),

    planet_size_enum(planet_size_str.c_str()),
    planet_type_enum(planet_type_str.c_str()),
    planet_environment_enum(planet_environment_str.c_str()),
    universe_object_type_enum(universe_object_type_str.c_str()),
    star_type_enum(star_type_str.c_str()),
    empire_affiliation_type_enum(empire_affiliation_type_str.c_str()),
    unlockable_item_type_enum(unlockable_item_type_str.c_str()),
    tech_type_enum(tech_type_str.c_str()),
    combat_fighter_type_enum(combat_fighter_type_str.c_str()),
    ship_part_class_enum(ship_part_class_str.c_str()),
    ship_slot_type_enum(ship_slot_type_str.c_str()),
    capture_result_enum(capture_result_str.c_str()),
    statistic_type_enum(statistic_type_str.c_str()),

    activation("(?i:activation)"),
    added_before("(?i:addedbefore)"),
    added_since("(?i:addedsince)"),
    affiliation("(?i:affiliation)"),
    age("(?i:age)"),
    alignment("(?i:alignment)"),
    all("(?i:all)"),
    and_("(?i:and)"),
    anti_fighter_damage("(?i:antifighterdamage)"),
    anti_ship_damage("(?i:antishipdamage)"),
    armed("(?i:armed)"),
    asteroids("(?i:asteroids)"),
    battle_speed("(?i:battlespeed)"),
    build_cost("(?i:buildcost)"),
    building("(?i:building)"),
    building_type("(?i:buildingtype)"),
    build_time("(?i:buildtime)"),
    capacity("(?i:capacity)"),
    capital("(?i:capital)"),
    capture_result("(?i:captureresult)"),
    category("(?i:category)"),
    class_("(?i:class)"),
    colour("(?i:colour)"),
    condition("(?i:condition)"),
    construction("(?i:construction)"),
    contained_by("(?i:containedby)"),
    contains("(?i:contains)"),
    created_on_turn("(?i:createdonturn)"),
    creation_turn("(?i:creationturn)"),
    damage("(?i:damage)"),
    defense("(?i:defense)"),
    description("(?i:description)"),
    design_has_hull("(?i:designhashull)"),
    design_has_part_class("(?i:designhaspartclass)"),
    design_has_part("(?i:designhaspart)"),
    design("(?i:design)"), // TODO: is this used for shipdesign? (if so, replace)
    design_id("(?i:designid)"),
    detection("(?i:detection)"),
    distance("(?i:distance)"),
    distance_to_source("(?i:distancetosource)"),
    effects_group("(?i:effectsgroup)"),
    effects_groups("(?i:effectsgroups)"),
    effects("(?i:effects)"),
    empire("(?i:empire)"),
    environment("(?i:environment)"),
    environments("(?i:environments)"),
    explored_by_empire("(?i:exploredbyempire)"),
    farming("(?i:farming)"),
    fighter_type("(?i:fightertype)"),
    fighter_weapon_range("(?i:fighterweaponrange)"),
    final_destination_id("(?i:finaldestinationid)"),
    fleet_id("(?i:fleetid)"),
    fleet("(?i:fleet)"),
    fleet_supplyable_by_empire("(?i:fleetsupplyablebyempire)"),
    foci("(?i:foci)"),
    focus("(?i:focus)"),
    food_consumption("(?i:foodconsumption)"),
    food_stockpile("(?i:foodstockpile)"),
    fuel("(?i:fuel)"),
    gas_giant("(?i:gasgiant)"),
    graphic("(?i:graphic)"),
    has_special("(?i:hasspecial)"),
    health("(?i:health)"),
    high("(?i:high)"),
    homeworld("(?i:homeworld)"),
    hull("(?i:hull)"),
    id("(?i:id)"),
    industry("(?i:industry)"),
    in_system("(?i:insystem)"),
    item("(?i:item)"),
    jumps("(?i:jumps)"),
    launch_rate("(?i:launchrate)"),
    local_candidate("(?i:localcandidate)"),
    location("(?i:location)"),
    lookup_strings("(?i:lookup_strings)"),
    low("(?i:low)"),
    max_defense("(?i:maxdefense)"),
    max_fuel("(?i:maxfuel)"),
    maximum_number_of("(?i:maximumnumberof)"),
    max_shield("(?i:maxshield)"),
    max_structure("(?i:maxstructure)"),
    max_troops("(?i:maxtroops)"),
    mineral_stockpile("(?i:mineralstockpile)"),
    minimum_number_of("(?i:minimumnumberof)"),
    mining("(?i:mining)"),
    model("(?i:model)"),
    mode_number_of("(?i:modenumberof)"),
    monster_fleet("(?i:monsterfleet)"),
    monster("(?i:monster)"),
    mountable_slot_types("(?i:mountableslottypes)"),
    name("(?i:name)"),
    next_better_planet_type("(?i:nextbetterplanettype)"),
    next_system_id("(?i:nextsystemid)"),
    not_("(?i:not)"),
    number("(?i:number)"),
    number_of("(?i:numberof)"),
    num_ships("(?i:numships)"),
    object("(?i:object)"),
    object_type("(?i:objecttype)"),
    or_("(?i:or)"),
    ownedby("(?i:ownedby)"), // TODO: can we replace this with 'owner'? (if so, replace)
    owner_food_stockpile("(?i:ownerfoodstockpile)"),
    owner_has_tech("(?i:ownerhastech)"),
    owner("(?i:owner)"),
    owner_mineral_stockpile("(?i:ownermineralstockpile)"),
    owner_trade_stockpile("(?i:ownertradestockpile)"),
    part("(?i:part)"),
    parts("(?i:parts)"),
    planetbound("(?i:planetbound)"),
    planet_environment("(?i:planetenvironment)"), // TODO: can this be replaced with 'environment'?
    planet_id("(?i:planetid)"),
    planet("(?i:planet)"),
    planet_size("(?i:planetsize)"),
    planet_type("(?i:planettype)"),
    population("(?i:population)"),
    position("(?i:position)"),
    prerequisites("(?i:prerequisites)"),
    previous_system_id("(?i:previoussystemid)"),
    probability("(?i:probability)"),
    produced_by_empireid("(?i:producedbyempireid)"),
    produced_by_empire("(?i:producedbyempire)"),
    producible("(?i:producible)"),
    property("(?i:property)"),
    random("(?i:random)"),
    range("(?i:range)"),
    researchable("(?i:researchable)"),
    research_cost("(?i:researchcost)"),
    research("(?i:research)"),
    research_turns("(?i:researchturns)"),
    resource_supply_connected_by_empire("(?i:resourcesupplyconnectedbyempire)"),
    rof("(?i:rof)"),
    root_candidate("(?i:rootcandidate)"),
    scope("(?i:scope)"),
    shield("(?i:shield)"),
    shipdesign("(?i:shipdesign)"),
    ships("(?i:ships)"),
    short_description("(?i:short_description)"),
    size("(?i:size)"), // TODO: used for planetsize (so replace)
    slot("(?i:slot)"),
    slots("(?i:slots)"),
    sortby("(?i:sortby)"),
    source("(?i:source)"),
    spacebound("(?i:spacebound)"),
    spawn_limit("(?i:spawnlimit)"),
    spawn_rate("(?i:spawnrate)"),
    special("(?i:special)"),
    species("(?i:species)"),
    speed("(?i:speed)"),
    stacking_group("(?i:stackinggroup)"),
    star("(?i:star)"),
    starlane_speed("(?i:starlanespeed)"),
    star_type("(?i:startype)"),
    stationary("(?i:stationary)"),
    stealth("(?i:stealth)"),
    structure("(?i:structure)"),
    supply("(?i:supply)"),
    system_id("(?i:systemid)"),
    system("(?i:system)"),
    target_construction("(?i:targetconstruction)"),
    target_farming("(?i:targetfarming)"),
    target_health("(?i:targethealth)"),
    target_industry("(?i:targetindustry)"),
    target("(?i:target)"),
    target_mining("(?i:targetmining)"),
    target_population("(?i:targetpopulation)"),
    target_research("(?i:targetresearch)"),
    target_trade("(?i:targettrade)"),
    tech("(?i:tech)"),
    tech_type("(?i:techtype)"),
    trade("(?i:trade)"),
    trade_stockpile("(?i:tradestockpile)"),
    troops("(?i:troops)"),
    turn("(?i:turn)"),
    type("(?i:type)"),
    unlock("(?i:unlock)"),
    value("(?i:value)"),
    visible_to_empire("(?i:visibletoempire)"),
    within_distance("(?i:withindistance)"),
    within_starlane_jumps("(?i:withinstarlanejumps)")
{
    namespace lex = boost::spirit::lex;

    self
        =     inline_comment
        |     end_of_line_comment

        |     bool_
        |     int_
        |     double_
        |     string

        |     planet_size_enum
        |     planet_type_enum
        |     planet_environment_enum
        |     universe_object_type_enum
        |     star_type_enum
        |     empire_affiliation_type_enum
        |     unlockable_item_type_enum
        |     tech_type_enum
        |     combat_fighter_type_enum
        |     ship_part_class_enum
        |     ship_slot_type_enum
        |     capture_result_enum
        |     statistic_type_enum

        |     activation
        |     added_before
        |     added_since
        |     affiliation
        |     age
        |     alignment
        |     all
        |     and_
        |     anti_fighter_damage
        |     anti_ship_damage
        |     armed
        |     asteroids
        |     battle_speed
        |     build_cost
        |     building
        |     building_type
        |     build_time
        |     capacity
        |     capital
        |     capture_result
        |     category
        |     class_
        |     colour
        |     condition
        |     construction
        |     contained_by
        |     contains
        |     created_on_turn
        |     creation_turn
        |     damage
        |     defense
        |     description
        |     design
        |     design_has_hull
        |     design_has_part
        |     design_has_part_class
        |     design_id
        |     detection
        |     distance
        |     distance_to_source
        |     effects
        |     effects_group
        |     effects_groups
        |     empire
        |     environment
        |     environments
        |     explored_by_empire
        |     farming
        |     fighter_type
        |     fighter_weapon_range
        |     final_destination_id
        |     fleet
        |     fleet_id
        |     fleet_supplyable_by_empire
        |     foci
        |     focus
        |     food_consumption
        |     food_stockpile
        |     fuel
        |     gas_giant
        |     graphic
        |     has_special
        |     health
        |     high
        |     homeworld
        |     hull
        |     id
        |     industry
        |     in_system
        |     int_variable_final
        |     item
        |     jumps
        |     launch_rate
        |     local_candidate
        |     location
        |     lookup_strings
        |     low
        |     max_defense
        |     max_fuel
        |     maximum_number_of
        |     max_shield
        |     max_structure
        |     max_troops
        |     mineral_stockpile
        |     minimum_number_of
        |     mining
        |     model
        |     mode_number_of
        |     monster
        |     monster_fleet
        |     mountable_slot_types
        |     name
        |     next_better_planet_type
        |     next_system_id
        |     not_
        |     number
        |     number_of
        |     num_ships
        |     object
        |     object_type
        |     or_
        |     ownedby
        |     owner
        |     owner_food_stockpile
        |     owner_has_tech
        |     owner_mineral_stockpile
        |     owner_trade_stockpile
        |     part
        |     parts
        |     planet
        |     planetbound
        |     planet_environment
        |     planet_id
        |     planet_size
        |     planet_type
        |     population
        |     position
        |     prerequisites
        |     previous_system_id
        |     probability
        |     produced_by_empire
        |     produced_by_empireid
        |     producible
        |     property
        |     random
        |     range
        |     research
        |     researchable
        |     research_cost
        |     research_turns
        |     resource_supply_connected_by_empire
        |     rof
        |     root_candidate
        |     scope
        |     shield
        |     shipdesign
        |     ships
        |     short_description
        |     size
        |     slot
        |     slots
        |     sortby
        |     source
        |     spacebound
        |     spawn_limit
        |     spawn_rate
        |     special
        |     species
        |     speed
        |     stacking_group
        |     star
        |     starlane_speed
        |     star_type
        |     stationary
        |     stealth
        |     structure
        |     supply
        |     system
        |     system_id
        |     target
        |     target_construction
        |     target_farming
        |     target_health
        |     target_industry
        |     target_mining
        |     target_population
        |     target_research
        |     target_trade
        |     tech
        |     tech_type
        |     trade
        |     trade_stockpile
        |     troops
        |     turn
        |     type
        |     unlock
        |     value
        |     visible_to_empire
        |     within_distance
        |     within_starlane_jumps

        |     '='
        |     '+'
        |     '-'
        |     '*'
        |     '/'
        |     '.'
        |     ','
        |     '('
        |     ')'
        |     '['
        |     ']'
        ;

    self("WS") = lex::token_def<>("\\s+");
}

namespace boost { namespace spirit { namespace traits {

    // This template specialization is required by Spirit.Lex to automatically
    // convert an iterator pair to an adobe::name_t in the lexer.
    void assign_to_attribute_from_iterators<adobe::name_t, parse::text_iterator, void>::
    call(const parse::text_iterator& first, const parse::text_iterator& last, adobe::name_t& attr)
    {
        std::string str(first, last);
        boost::algorithm::to_lower(str);
        attr = adobe::name_t(str.c_str());
    }

    void assign_to_attribute_from_iterators<bool, parse::text_iterator, void>::
    call(const parse::text_iterator& first, const parse::text_iterator& last, bool& attr)
    { attr = *first == 't' || *first == 'T' ? true : false; }

#define ASSIGN_TO_ATTRIBUTE_DEF(T, name)                                                \
    void assign_to_attribute_from_iterators<T, parse::text_iterator, void>::            \
    call(const parse::text_iterator& first, const parse::text_iterator& last, T& attr)  \
    {                                                                                   \
        static const std::vector<std::string> strings = parse_into_strings(name##_str); \
        long position = find_match(strings, first, last);                               \
        if (position != -1)                                                             \
            position += first_##name;                                                   \
        attr = static_cast<T>(position);                                                \
    }

    ASSIGN_TO_ATTRIBUTE_DEF(PlanetSize, planet_size);
    ASSIGN_TO_ATTRIBUTE_DEF(PlanetType, planet_type);
    ASSIGN_TO_ATTRIBUTE_DEF(PlanetEnvironment, planet_environment);

    void assign_to_attribute_from_iterators<UniverseObjectType, parse::text_iterator, void>::
    call(const parse::text_iterator& first, const parse::text_iterator& last, UniverseObjectType& attr)
    {
        typedef std::map<std::string, UniverseObjectType> MapType;
        static const MapType strings = boost::assign::list_of<std::pair<std::string, UniverseObjectType> >
            ("building", OBJ_BUILDING)
            ("ship", OBJ_SHIP)
            ("fleet", OBJ_FLEET)
            ("planet", OBJ_PLANET)
            ("populationcenter", OBJ_POP_CENTER)
            ("productioncenter", OBJ_PROD_CENTER)
            ("system", OBJ_SYSTEM)
            ;
        std::string match(first, last);
        boost::algorithm::to_lower(match);
        MapType::const_iterator it = strings.find(match);
        if (it == strings.end())
            attr = INVALID_UNIVERSE_OBJECT_TYPE;
        else
            attr = it->second;
    }

    ASSIGN_TO_ATTRIBUTE_DEF(StarType, star_type);
    ASSIGN_TO_ATTRIBUTE_DEF(EmpireAffiliationType, empire_affiliation_type);
    ASSIGN_TO_ATTRIBUTE_DEF(UnlockableItemType, unlockable_item_type);
    ASSIGN_TO_ATTRIBUTE_DEF(TechType, tech_type);
    ASSIGN_TO_ATTRIBUTE_DEF(CombatFighterType, combat_fighter_type);

    void assign_to_attribute_from_iterators<ShipPartClass, parse::text_iterator, void>::
    call(const parse::text_iterator& first, const parse::text_iterator& last, ShipPartClass& attr)
    {
        typedef std::map<std::string, ShipPartClass> MapType;
        static const MapType strings = boost::assign::list_of<std::pair<std::string, ShipPartClass> >
            ("shortrange", PC_SHORT_RANGE)
            ("missiles", PC_MISSILES)
            ("fighters", PC_FIGHTERS)
            ("pointdefense", PC_POINT_DEFENSE)
            ("shield", PC_SHIELD)
            ("armour", PC_ARMOUR)
            ("troops", PC_TROOPS)
            ("detection", PC_DETECTION)
            ("stealth", PC_STEALTH)
            ("fuel", PC_FUEL)
            ("colony", PC_COLONY)
            ("battlespeed", PC_BATTLE_SPEED)
            ("starlanespeed", PC_STARLANE_SPEED)
            ;
        std::string match(first, last);
        boost::algorithm::to_lower(match);
        MapType::const_iterator it = strings.find(match);
        if (it == strings.end())
            attr = INVALID_SHIP_PART_CLASS;
        else
            attr = it->second;
    }

    ASSIGN_TO_ATTRIBUTE_DEF(ShipSlotType, ship_slot_type);
    ASSIGN_TO_ATTRIBUTE_DEF(CaptureResult, capture_result);
    ASSIGN_TO_ATTRIBUTE_DEF(ValueRef::StatisticType, statistic_type);

#undef ASSIGN_TO_ATTRIBUTE_DEF

} } }
