#include "Lexer.h"

#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/assign/list_of.hpp>


namespace {

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
    universe_object_type(universe_object_type_str.c_str()),
    star_type_enum(star_type_str.c_str()),
    empire_affiliation_type(empire_affiliation_type_str.c_str()),
    unlockable_item_type(unlockable_item_type_str.c_str()),
    tech_type_enum(tech_type_str.c_str()),
    combat_fighter_type(combat_fighter_type_str.c_str()),
    ship_part_class(ship_part_class_str.c_str()),
    ship_slot_type(ship_slot_type_str.c_str()),
    capture_result_enum(capture_result_str.c_str()),
    statistic_type(statistic_type_str.c_str()),
    fleet("(?i:fleet)"),
    monster_fleet("(?i:monsterfleet)"),
    planet("(?i:planet)"),
    system("(?i:system)"),
    asteroids("(?i:asteroids)"),
    gas_giant("(?i:gasgiant)"),
    building("(?i:building)"),
    fuel("(?i:fuel)"),
    shield("(?i:shield)"),
    troops("(?i:troops)"),
    stealth("(?i:stealth)"),
    detection("(?i:detection)"),
    battle_speed("(?i:battlespeed)"),
    starlane_speed("(?i:starlanespeed)"),
    scope("(?i:scope)"),
    activation("(?i:activation)"),
    stacking_group("(?i:stackinggroup)"),
    effects("(?i:effects)"),
    name("(?i:name)"),
    description("(?i:description)"),
    short_description("(?i:short_description)"),
    build_cost("(?i:buildcost)"),
    build_time("(?i:buildtime)"),
    capture_result("(?i:captureresult)"),
    effects_group("(?i:effectsgroup)"),
    effects_groups("(?i:effectsgroups)"),
    graphic("(?i:graphic)"),
    tech_type("(?i:techtype)"),
    special("(?i:special)"),
    species("(?i:species)"),
    building_type("(?i:buildingtype)"),
    focus("(?i:focus)"),
    planetbound("(?i:planetbound)"),
    spacebound("(?i:spacebound)"),
    item("(?i:item)"),
    tech("(?i:tech)"),
    part("(?i:part)"),
    shipdesign("(?i:shipdesign)"),
    alignment("(?i:alignment)"),
    low("(?i:low)"),
    high("(?i:high)"),
    number("(?i:number)"),
    empire("(?i:empire)"),
    affiliation("(?i:affiliation)"),
    type("(?i:type)"),
    size("(?i:size)"), // TODO: used for planetsize (so replace)
    probability("(?i:probability)"),
    distance("(?i:distance)"),
    jumps("(?i:jumps)"),
    sortby("(?i:sortby)"),
    design("(?i:design)"), // TODO: is this used for shipdesign? (if so, replace)
    all("(?i:all)"),
    ownedby("(?i:ownedby)"), // TODO: can we replace this with 'owner'? (if so, replace)
    stationary("(?i:stationary)"),
    homeworld("(?i:homeworld)"),
    capital("(?i:capital)"),
    monster("(?i:monster)"),
    armed("(?i:armed)"),
    and_("(?i:and)"),
    or_("(?i:or)"),
    not_("(?i:not)"),
    id("(?i:id)"),
    added_since("(?i:addedsince)"),
    added_before("(?i:addedbefore)"),
    owner_has_tech("(?i:ownerhastech)"),
    has_special("(?i:hasspecial)"),
    design_has_hull("(?i:designhashull)"),
    design_has_part("(?i:designhaspart)"),
    design_has_part_class("(?i:designhaspartclass)"),
    produced_by_empire("(?i:producedbyempire)"),
    visible_to_empire("(?i:visibletoempire)"),
    explored_by_empire("(?i:exploredbyempire)"),
    fleet_supplyable_by_empire("(?i:fleetsupplyablebyempire)"),
    in_system("(?i:insystem)"),
    object("(?i:object)"),
    within_distance("(?i:withindistance)"),
    within_starlane_jumps("(?i:withinstarlanejumps)"),
    turn("(?i:turn)"),
    created_on_turn("(?i:createdonturn)"),
    number_of("(?i:numberof)"),
    maximum_number_of("(?i:maximumnumberof)"),
    minimum_number_of("(?i:minimumnumberof)"),
    mode_number_of("(?i:modenumberof)"),
    contains("(?i:contains)"),
    contained_by("(?i:containedby)"),
    star("(?i:star)"),
    random("(?i:random)"),
    owner_food_stockpile("(?i:ownerfoodstockpile)"),
    owner_mineral_stockpile("(?i:ownermineralstockpile)"),
    owner_trade_stockpile("(?i:ownertradestockpile)"),
    resource_supply_connected_by_empire("(?i:resourcesupplyconnectedbyempire)"),
    farming("(?i:farming)"),
    target_farming("(?i:targetfarming)"),
    industry("(?i:industry)"),
    target_industry("(?i:targetindustry)"),
    research("(?i:research)"),
    target_research("(?i:targetresearch)"),
    trade("(?i:trade)"),
    target_trade("(?i:targettrade)"),
    mining("(?i:mining)"),
    target_mining("(?i:targetmining)"),
    construction("(?i:construction)"),
    target_construction("(?i:targetconstruction)"),
    population("(?i:population)"),
    target_population("(?i:targetpopulation)"),
    target_health("(?i:targethealth)"),
    max_fuel("(?i:maxfuel)"),
    max_shield("(?i:maxshield)"),
    max_defense("(?i:maxdefense)"),
    defense("(?i:defense)"),
    max_troops("(?i:maxtroops)"),
    max_structure("(?i:maxstructure)"),
    supply("(?i:supply)"),
    food_consumption("(?i:foodconsumption)"),
    trade_stockpile("(?i:tradestockpile)"),
    mineral_stockpile("(?i:mineralstockpile)"),
    food_stockpile("(?i:foodstockpile)"),
    distance_to_source("(?i:distancetosource)"),
    category("(?i:category)"),
    research_cost("(?i:researchcost)"),
    research_turns("(?i:researchturns)"),
    prerequisites("(?i:prerequisites)"),
    unlock("(?i:unlock)"),
    environment("(?i:environment)"),
    environments("(?i:environments)"),
    foci("(?i:foci)"),
    location("(?i:location)"),
    class_("(?i:class)"),
    speed("(?i:speed)"),
    slot("(?i:slot)"),
    slots("(?i:slots)"),
    mountable_slot_types("(?i:mountableslottypes)"),
    colour("(?i:colour)"),
    position("(?i:position)"),
    damage("(?i:damage)"),
    anti_ship_damage("(?i:antishipdamage)"),
    anti_fighter_damage("(?i:antifighterdamage)"),
    rof("(?i:rof)"),
    range("(?i:range)"),
    fighter_weapon_range("(?i:fighterweaponrange)"),
    health("(?i:health)"),
    structure("(?i:structure)"),
    fighter_type("(?i:fightertype)"),
    launch_rate("(?i:launchrate)"),
    capacity("(?i:capacity)"),
    hull("(?i:hull)"),
    parts("(?i:parts)"),
    ships("(?i:ships)"),
    model("(?i:model)"),
    lookup_strings("(?i:lookup_strings)"),
    researchable("(?i:researchable)"),
    producible("(?i:producible)"),
    spawn_rate("(?i:spawnrate)"),
    spawn_limit("(?i:spawnlimit)"),
    property("(?i:property)"),
    condition("(?i:condition)"),
    source("(?i:source)"),
    target("(?i:target)"),
    local_candidate("(?i:localcandidate)"),
    root_candidate("(?i:rootcandidate)"),
    value("(?i:value)"),
    planet_size("(?i:planetsize)"),
    planet_type("(?i:planettype)"),
    next_better_planet_type("(?i:nextbetterplanettype)"),
    planet_environment("(?i:planetenvironment)"), // TODO: can this be replaced with 'environment'?
    object_type("(?i:objecttype)"),
    owner("(?i:owner)"),
    creation_turn("(?i:creationturn)"),
    age("(?i:age)"),
    produced_by_empireid("(?i:producedbyempireid)"),
    design_id("(?i:designid)"),
    fleet_id("(?i:fleetid)"),
    planet_id("(?i:planetid)"),
    system_id("(?i:systemid)"),
    final_destination_id("(?i:finaldestinationid)"),
    next_system_id("(?i:nextsystemid)"),
    previous_system_id("(?i:previoussystemid)"),
    num_ships("(?i:numships)"),
    star_type("(?i:startype)")
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
        |     universe_object_type
        |     star_type_enum
        |     empire_affiliation_type
        |     unlockable_item_type
        |     tech_type_enum
        |     combat_fighter_type
        |     ship_part_class
        |     ship_slot_type
        |     capture_result_enum
        |     fleet
        |     monster_fleet
        |     planet
        |     system
        |     asteroids
        |     gas_giant
        |     building
        |     fuel
        |     shield
        |     troops
        |     stealth
        |     detection
        |     battle_speed
        |     starlane_speed
        |     scope
        |     activation
        |     stacking_group
        |     effects
        |     name
        |     description
        |     short_description
        |     build_cost
        |     build_time
        |     capture_result
        |     effects_group
        |     effects_groups
        |     graphic
        |     tech_type
        |     special
        |     species
        |     building_type
        |     focus
        |     planetbound
        |     spacebound
        |     item
        |     tech
        |     part
        |     shipdesign
        |     alignment
        |     low
        |     high
        |     number
        |     empire
        |     affiliation
        |     type
        |     size
        |     probability
        |     distance
        |     jumps
        |     sortby
        |     design
        |     all
        |     ownedby
        |     stationary
        |     homeworld
        |     capital
        |     monster
        |     armed
        |     and_
        |     or_
        |     not_
        |     id
        |     added_since
        |     added_before
        |     owner_has_tech
        |     has_special
        |     design_has_hull
        |     design_has_part
        |     design_has_part_class
        |     produced_by_empire
        |     visible_to_empire
        |     explored_by_empire
        |     fleet_supplyable_by_empire
        |     in_system
        |     object
        |     within_distance
        |     within_starlane_jumps
        |     turn
        |     created_on_turn
        |     number_of
        |     maximum_number_of
        |     minimum_number_of
        |     mode_number_of
        |     contains
        |     contained_by
        |     star
        |     random
        |     owner_food_stockpile
        |     owner_mineral_stockpile
        |     owner_trade_stockpile
        |     resource_supply_connected_by_empire
        |     farming
        |     target_farming
        |     industry
        |     target_industry
        |     research
        |     target_research
        |     trade
        |     target_trade
        |     mining
        |     target_mining
        |     construction
        |     target_construction
        |     population
        |     target_population
        |     target_health
        |     max_fuel
        |     max_shield
        |     max_defense
        |     defense
        |     max_troops
        |     max_structure
        |     supply
        |     food_consumption
        |     trade_stockpile
        |     mineral_stockpile
        |     food_stockpile
        |     distance_to_source
        |     category
        |     research_cost
        |     research_turns
        |     prerequisites
        |     unlock
        |     environment
        |     environments
        |     foci
        |     location
        |     class_
        |     speed
        |     slot
        |     slots
        |     mountable_slot_types
        |     colour
        |     position
        |     damage
        |     anti_ship_damage
        |     anti_fighter_damage
        |     rof
        |     range
        |     fighter_weapon_range
        |     health
        |     structure
        |     fighter_type
        |     launch_rate
        |     capacity
        |     hull
        |     parts
        |     ships
        |     model
        |     lookup_strings
        |     researchable
        |     producible
        |     spawn_rate
        |     spawn_limit
        |     property
        |     condition
        |     source
        |     target
        |     local_candidate
        |     root_candidate
        |     value
        |     statistic_type
        |     int_variable_final
        |     planet_size
        |     planet_type
        |     next_better_planet_type
        |     planet_environment
        |     object_type
        |     owner
        |     creation_turn
        |     age
        |     produced_by_empireid
        |     design_id
        |     fleet_id
        |     planet_id
        |     system_id
        |     final_destination_id
        |     next_system_id
        |     previous_system_id
        |     num_ships
        |     star_type
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

    // These template specializations are required by Spirit.Lex to automatically
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
