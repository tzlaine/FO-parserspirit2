#include "EnumParser.h"


namespace qi = boost::spirit::qi;

namespace {

    qi::_1_type _1;
    qi::_val_type _val;

}

namespace parse {

    template <>
    const enum_parser_rule<PlanetSize>::type& enum_parser<PlanetSize>()
    {
        const parse::lexer& tok = parse::lexer::instance();
        static enum_parser_rule<PlanetSize>::type retval
            =    tok.planet_size_enum [ _val = _1 ]
            |    tok.Asteroids_ [ _val = SZ_ASTEROIDS ]
            |    tok.GasGiant_ [ _val = SZ_GASGIANT ]
            ;
        retval.name("PlanetSize");
        return retval;
    }

    template <>
    const enum_parser_rule<PlanetType>::type& enum_parser<PlanetType>()
    {
        const parse::lexer& tok = parse::lexer::instance();
        static enum_parser_rule<PlanetType>::type retval
            =    tok.planet_type_enum [ _val = _1 ]
            |    tok.Asteroids_ [ _val = PT_ASTEROIDS ]
            |    tok.GasGiant_ [ _val = PT_GASGIANT ]
            ;
        retval.name("PlanetType");
        return retval;
    }

    template <>
    const enum_parser_rule<PlanetEnvironment>::type& enum_parser<PlanetEnvironment>()
    {
        const parse::lexer& tok = parse::lexer::instance();
        static enum_parser_rule<PlanetEnvironment>::type retval
            =    tok.planet_environment_enum [ _val = _1 ]
            ;
        retval.name("PlanetEnvironment");
        return retval;
    }

    template <>
    const enum_parser_rule<UniverseObjectType>::type& enum_parser<UniverseObjectType>()
    {
        const parse::lexer& tok = parse::lexer::instance();
        static enum_parser_rule<UniverseObjectType>::type retval
            =    tok.universe_object_type_enum [ _val = _1 ]
            |    tok.Building_ [ _val = OBJ_BUILDING ]
            |    tok.Fleet_ [ _val = OBJ_FLEET ]
            |    tok.Planet_ [ _val = OBJ_PLANET ]
            |    tok.System_ [ _val = OBJ_SYSTEM ]
            ;
        retval.name("UniverseObjectType");
        return retval;
    }

    template <>
    const enum_parser_rule<StarType>::type& enum_parser<StarType>()
    {
        const parse::lexer& tok = parse::lexer::instance();
        static enum_parser_rule<StarType>::type retval
            =    tok.star_type_enum [ _val = _1 ]
            ;
        retval.name("StarType");
        return retval;
    }

    template <>
    const enum_parser_rule<MeterType>::type& enum_parser<MeterType>()
    {
        static enum_parser_rule<MeterType>::type retval
            =    non_ship_part_meter_type_enum() [ _val = _1 ]
            |    ship_part_meter_type_enum() [ _val = _1 ]
            ;
        retval.name("MeterType");
        return retval;
    }

    template <>
    const enum_parser_rule<EmpireAffiliationType>::type& enum_parser<EmpireAffiliationType>()
    {
        const parse::lexer& tok = parse::lexer::instance();
        static enum_parser_rule<EmpireAffiliationType>::type retval
            =    tok.empire_affiliation_type_enum [ _val = _1 ]
            ;
        retval.name("EmpireAffiliationType");
        return retval;
    }

    template <>
    const enum_parser_rule<UnlockableItemType>::type& enum_parser<UnlockableItemType>()
    {
        const parse::lexer& tok = parse::lexer::instance();
        static enum_parser_rule<UnlockableItemType>::type retval
            =    tok.unlockable_item_type_enum [ _val = _1 ]
            |    tok.Building_ [ _val = UIT_BUILDING ]
            |    tok.ShipDesign_ [ _val = UIT_SHIP_DESIGN ]
            |    tok.Tech_ [ _val = UIT_TECH ]
            ;
        retval.name("UnlockableItemType");
        return retval;
    }

    template <>
    const enum_parser_rule<TechType>::type& enum_parser<TechType>()
    {
        const parse::lexer& tok = parse::lexer::instance();
        static enum_parser_rule<TechType>::type retval
            =    tok.tech_type_enum [ _val = _1 ]
            ;
        retval.name("TechType");
        return retval;
    }

    template <>
    const enum_parser_rule<ShipSlotType>::type& enum_parser<ShipSlotType>()
    {
        const parse::lexer& tok = parse::lexer::instance();
        static enum_parser_rule<ShipSlotType>::type retval
            =    tok.ship_slot_type_enum [ _val = _1 ]
            ;
        retval.name("ShipSlotType");
        return retval;
    }

    template <>
    const enum_parser_rule<ShipPartClass>::type& enum_parser<ShipPartClass>()
    {
        const parse::lexer& tok = parse::lexer::instance();
        static enum_parser_rule<ShipPartClass>::type retval
            =    tok.ship_part_class_enum [ _val = _1 ]
            ;
        retval.name("ShipPartClass");
        return retval;
    }

    template <>
    const enum_parser_rule<CombatFighterType>::type& enum_parser<CombatFighterType>()
    {
        const parse::lexer& tok = parse::lexer::instance();
        static enum_parser_rule<CombatFighterType>::type retval
            =    tok.combat_fighter_type_enum [ _val = _1 ]
            ;
        retval.name("CombatFighterType");
        return retval;
    }

    template <>
    const enum_parser_rule<CaptureResult>::type& enum_parser<CaptureResult>()
    {
        const parse::lexer& tok = parse::lexer::instance();
        static enum_parser_rule<CaptureResult>::type retval
            =    tok.capture_result_enum [ _val = _1 ]
            |    tok.Destroy_ [ _val = CR_DESTROY ]
            ;
        retval.name("CaptureResult");
        return retval;
    }

    template <>
    const enum_parser_rule<ValueRef::StatisticType>::type& enum_parser<ValueRef::StatisticType>()
    {
        const parse::lexer& tok = parse::lexer::instance();
        static enum_parser_rule<ValueRef::StatisticType>::type retval
            =    tok.statistic_type_enum [ _val = _1 ]
            |    tok.Mode_ [ _val = ValueRef::MODE ]
            ;
        retval.name("StatisticType");
        return retval;
    }

    const enum_parser_rule<MeterType>::type& non_ship_part_meter_type_enum()
    {
        const parse::lexer& tok = parse::lexer::instance();
        static enum_parser_rule<MeterType>::type retval
            =    tok.TargetConstruction_ [ _val = METER_TARGET_CONSTRUCTION ]
            |    tok.TargetFarming_ [ _val = METER_TARGET_FARMING ]
            |    tok.TargetHealth_ [ _val = METER_TARGET_HEALTH ]
            |    tok.TargetIndustry_ [ _val = METER_TARGET_INDUSTRY ]
            |    tok.TargetMining_ [ _val = METER_TARGET_MINING ]
            |    tok.TargetPopulation_ [ _val = METER_TARGET_POPULATION ]
            |    tok.TargetResearch_ [ _val = METER_TARGET_RESEARCH ]
            |    tok.TargetTrade_ [ _val = METER_TARGET_TRADE ]

            |    tok.MaxDefense_ [ _val = METER_MAX_DEFENSE ]
            |    tok.MaxFuel_ [ _val = METER_MAX_FUEL ]
            |    tok.MaxShield_ [ _val = METER_MAX_SHIELD ]
            |    tok.MaxStructure_ [ _val = METER_MAX_STRUCTURE ]
            |    tok.MaxTroops_ [ _val = METER_MAX_TROOPS ]

            |    tok.Construction_ [ _val = METER_CONSTRUCTION ]
            |    tok.Farming_ [ _val = METER_FARMING ]
            |    tok.Health_ [ _val = METER_HEALTH ]
            |    tok.Industry_ [ _val = METER_INDUSTRY ]
            |    tok.Mining_ [ _val = METER_MINING ]
            |    tok.Population_ [ _val = METER_POPULATION ]
            |    tok.Research_ [ _val = METER_RESEARCH ]
            |    tok.Trade_ [ _val = METER_TRADE ]

            |    tok.Defense_ [ _val = METER_DEFENSE ]
            |    tok.Fuel_ [ _val = METER_FUEL ]
            |    tok.Shield_ [ _val = METER_SHIELD ]
            |    tok.Structure_ [ _val = METER_STRUCTURE ]
            |    tok.Troops_ [ _val = METER_TROOPS ]

            |    tok.FoodConsumption_ [ _val = METER_FOOD_CONSUMPTION ]
            |    tok.Supply_ [ _val = METER_SUPPLY ]
            |    tok.Stealth_ [ _val = METER_STEALTH ]
            |    tok.Detection_ [ _val = METER_DETECTION ]
            |    tok.BattleSpeed_ [ _val = METER_BATTLE_SPEED ]
            |    tok.StarlaneSpeed_ [ _val = METER_STARLANE_SPEED ]
            ;
        retval.name("non-ship-part MeterType");
        return retval;
    }

    const enum_parser_rule<MeterType>::type& ship_part_meter_type_enum()
    {
        const parse::lexer& tok = parse::lexer::instance();
        static enum_parser_rule<MeterType>::type retval
            =    tok.Damage_ [ _val = METER_DAMAGE ]
            |    tok.ROF_ [ _val = METER_ROF ]
            |    tok.Range_ [ _val = METER_RANGE ]
            |    tok.Speed_ [ _val = METER_SPEED ]
            |    tok.Capacity_ [ _val = METER_CAPACITY ]
            |    tok.AntiShipDamage_ [ _val = METER_ANTI_SHIP_DAMAGE ]
            |    tok.AntiFighterDamage_ [ _val = METER_ANTI_FIGHTER_DAMAGE ]
            |    tok.LaunchRate_ [ _val = METER_LAUNCH_RATE ]
            |    tok.FighterWeaponRange_ [ _val = METER_FIGHTER_WEAPON_RANGE ]
            |    tok.Stealth_ [ _val = METER_STEALTH ]
            |    tok.Detection_ [ _val = METER_DETECTION ]
            |    tok.Structure_ [ _val = METER_STRUCTURE ]
            ;
        retval.name("ship-part MeterType");
        return retval;
    }

}
