#include "EnumParser.h"


namespace qi = boost::spirit::qi;

#define DEBUG_PARSERS 0

namespace {

    qi::_1_type _1;
    qi::_val_type _val;

}

namespace parse {

    template <>
    enum_parser_rule<PlanetSize>::type& enum_parser<PlanetSize>()
    {
        const parse::lexer& tok = parse::lexer::instance();
        static enum_parser_rule<PlanetSize>::type retval
            =    tok.planet_size_enum [ _val = _1 ]
            |    tok.Asteroids_ [ _val = SZ_ASTEROIDS ]
            |    tok.GasGiant_ [ _val = SZ_GASGIANT ]
            ;
        static bool once = true;
        if (once) {
            retval.name("PlanetSize");
#if DEBUG_PARSERS
            debug(retval);
#endif
            once = false;
        }
        return retval;
    }

    template <>
    enum_parser_rule<PlanetType>::type& enum_parser<PlanetType>()
    {
        const parse::lexer& tok = parse::lexer::instance();
        static enum_parser_rule<PlanetType>::type retval
            =    tok.planet_type_enum [ _val = _1 ]
            |    tok.Asteroids_ [ _val = PT_ASTEROIDS ]
            |    tok.GasGiant_ [ _val = PT_GASGIANT ]
            ;
        static bool once = true;
        if (once) {
            retval.name("PlanetType");
#if DEBUG_PARSERS
            debug(retval);
#endif
            once = false;
        }
        return retval;
    }

    template <>
    enum_parser_rule<PlanetEnvironment>::type& enum_parser<PlanetEnvironment>()
    {
        const parse::lexer& tok = parse::lexer::instance();
        static enum_parser_rule<PlanetEnvironment>::type retval
            =    tok.planet_environment_enum [ _val = _1 ]
            ;
        static bool once = true;
        if (once) {
            retval.name("PlanetEnvironment");
#if DEBUG_PARSERS
            debug(retval);
#endif
            once = false;
        }
        return retval;
    }

    template <>
    enum_parser_rule<UniverseObjectType>::type& enum_parser<UniverseObjectType>()
    {
        const parse::lexer& tok = parse::lexer::instance();
        static enum_parser_rule<UniverseObjectType>::type retval
            =    tok.universe_object_type_enum [ _val = _1 ]
            |    tok.Building_ [ _val = OBJ_BUILDING ]
            |    tok.Fleet_ [ _val = OBJ_FLEET ]
            |    tok.Planet_ [ _val = OBJ_PLANET ]
            |    tok.System_ [ _val = OBJ_SYSTEM ]
            ;
        static bool once = true;
        if (once) {
            retval.name("UniverseObjectType");
#if DEBUG_PARSERS
            debug(retval);
#endif
            once = false;
        }
        return retval;
    }

    template <>
    enum_parser_rule<StarType>::type& enum_parser<StarType>()
    {
        const parse::lexer& tok = parse::lexer::instance();
        static enum_parser_rule<StarType>::type retval
            =    tok.star_type_enum [ _val = _1 ]
            ;
        static bool once = true;
        if (once) {
            retval.name("StarType");
#if DEBUG_PARSERS
            debug(retval);
#endif
            once = false;
        }
        return retval;
    }

    template <>
    enum_parser_rule<EmpireAffiliationType>::type& enum_parser<EmpireAffiliationType>()
    {
        const parse::lexer& tok = parse::lexer::instance();
        static enum_parser_rule<EmpireAffiliationType>::type retval
            =    tok.empire_affiliation_type_enum [ _val = _1 ]
            ;
        static bool once = true;
        if (once) {
            retval.name("EmpireAffiliationType");
#if DEBUG_PARSERS
            debug(retval);
#endif
            once = false;
        }
        return retval;
    }

    template <>
    enum_parser_rule<UnlockableItemType>::type& enum_parser<UnlockableItemType>()
    {
        const parse::lexer& tok = parse::lexer::instance();
        static enum_parser_rule<UnlockableItemType>::type retval
            =    tok.unlockable_item_type_enum [ _val = _1 ]
            |    tok.Building_ [ _val = UIT_BUILDING ]
            |    tok.ShipDesign_ [ _val = UIT_SHIP_DESIGN ]
            |    tok.Tech_ [ _val = UIT_TECH ]
            ;
        static bool once = true;
        if (once) {
            retval.name("UnlockableItemType");
#if DEBUG_PARSERS
            debug(retval);
#endif
            once = false;
        }
        return retval;
    }

    template <>
    enum_parser_rule<TechType>::type& enum_parser<TechType>()
    {
        const parse::lexer& tok = parse::lexer::instance();
        static enum_parser_rule<TechType>::type retval
            =    tok.tech_type_enum [ _val = _1 ]
            ;
        static bool once = true;
        if (once) {
            retval.name("TechType");
#if DEBUG_PARSERS
            debug(retval);
#endif
            once = false;
        }
        return retval;
    }

    template <>
    enum_parser_rule<ShipSlotType>::type& enum_parser<ShipSlotType>()
    {
        const parse::lexer& tok = parse::lexer::instance();
        static enum_parser_rule<ShipSlotType>::type retval
            =    tok.ship_slot_type_enum [ _val = _1 ]
            ;
        static bool once = true;
        if (once) {
            retval.name("ShipSlotType");
#if DEBUG_PARSERS
            debug(retval);
#endif
            once = false;
        }
        return retval;
    }

    template <>
    enum_parser_rule<ShipPartClass>::type& enum_parser<ShipPartClass>()
    {
        const parse::lexer& tok = parse::lexer::instance();
        static enum_parser_rule<ShipPartClass>::type retval
            =    tok.ship_part_class_enum [ _val = _1 ]
            |    tok.Shield_ [ _val = PC_SHIELD ]
            |    tok.Troops_ [ _val = PC_TROOPS ]
            |    tok.Detection_ [ _val = PC_DETECTION ]
            |    tok.Stealth_ [ _val = PC_STEALTH ]
            |    tok.Fuel_ [ _val = PC_FUEL ]
            |    tok.BattleSpeed_ [ _val = PC_BATTLE_SPEED ]
            |    tok.StarlaneSpeed_ [ _val = PC_STARLANE_SPEED ]
            ;
        static bool once = true;
        if (once) {
            retval.name("ShipPartClass");
#if DEBUG_PARSERS
            debug(retval);
#endif
            once = false;
        }
        return retval;
    }

    template <>
    enum_parser_rule<CombatFighterType>::type& enum_parser<CombatFighterType>()
    {
        const parse::lexer& tok = parse::lexer::instance();
        static enum_parser_rule<CombatFighterType>::type retval
            =    tok.combat_fighter_type_enum [ _val = _1 ]
            ;
        static bool once = true;
        if (once) {
            retval.name("CombatFighterType");
#if DEBUG_PARSERS
            debug(retval);
#endif
            once = false;
        }
        return retval;
    }

    template <>
    enum_parser_rule<CaptureResult>::type& enum_parser<CaptureResult>()
    {
        const parse::lexer& tok = parse::lexer::instance();
        static enum_parser_rule<CaptureResult>::type retval
            =    tok.capture_result_enum [ _val = _1 ]
            |    tok.Destroy_ [ _val = CR_DESTROY ]
            ;
        static bool once = true;
        if (once) {
            retval.name("CaptureResult");
#if DEBUG_PARSERS
            debug(retval);
#endif
            once = false;
        }
        return retval;
    }

    template <>
    enum_parser_rule<ValueRef::StatisticType>::type& enum_parser<ValueRef::StatisticType>()
    {
        const parse::lexer& tok = parse::lexer::instance();
        static enum_parser_rule<ValueRef::StatisticType>::type retval
            =    tok.statistic_type_enum [ _val = _1 ]
            |    tok.Mode_ [ _val = ValueRef::MODE ]
            ;
        static bool once = true;
        if (once) {
            retval.name("StatisticType");
#if DEBUG_PARSERS
            debug(retval);
#endif
            once = false;
        }
        return retval;
    }

    enum_parser_rule<MeterType>::type& non_ship_part_meter_type_enum()
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
        static bool once = true;
        if (once) {
            retval.name("non-ship-part MeterType");
#if DEBUG_PARSERS
            debug(retval);
#endif
            once = false;
        }
        return retval;
    }

    enum_parser_rule<MeterType>::type& set_non_ship_part_meter_type_enum()
    {
        const parse::lexer& tok = parse::lexer::instance();
        static enum_parser_rule<MeterType>::type retval
            =    tok.SetTargetConstruction_ [ _val = METER_TARGET_CONSTRUCTION ]
            |    tok.SetTargetFarming_ [ _val = METER_TARGET_FARMING ]
            |    tok.SetTargetHealth_ [ _val = METER_TARGET_HEALTH ]
            |    tok.SetTargetIndustry_ [ _val = METER_TARGET_INDUSTRY ]
            |    tok.SetTargetMining_ [ _val = METER_TARGET_MINING ]
            |    tok.SetTargetPopulation_ [ _val = METER_TARGET_POPULATION ]
            |    tok.SetTargetResearch_ [ _val = METER_TARGET_RESEARCH ]
            |    tok.SetTargetTrade_ [ _val = METER_TARGET_TRADE ]

            |    tok.SetMaxDefense_ [ _val = METER_MAX_DEFENSE ]
            |    tok.SetMaxFuel_ [ _val = METER_MAX_FUEL ]
            |    tok.SetMaxShield_ [ _val = METER_MAX_SHIELD ]
            |    tok.SetMaxStructure_ [ _val = METER_MAX_STRUCTURE ]
            |    tok.SetMaxTroops_ [ _val = METER_MAX_TROOPS ]

            |    tok.SetConstruction_ [ _val = METER_CONSTRUCTION ]
            |    tok.SetFarming_ [ _val = METER_FARMING ]
            |    tok.SetHealth_ [ _val = METER_HEALTH ]
            |    tok.SetIndustry_ [ _val = METER_INDUSTRY ]
            |    tok.SetMining_ [ _val = METER_MINING ]
            |    tok.SetPopulation_ [ _val = METER_POPULATION ]
            |    tok.SetResearch_ [ _val = METER_RESEARCH ]
            |    tok.SetTrade_ [ _val = METER_TRADE ]

            |    tok.SetDefense_ [ _val = METER_DEFENSE ]
            |    tok.SetFuel_ [ _val = METER_FUEL ]
            |    tok.SetShield_ [ _val = METER_SHIELD ]
            |    tok.SetStructure_ [ _val = METER_STRUCTURE ]
            |    tok.SetTroops_ [ _val = METER_TROOPS ]

            |    tok.SetFoodConsumption_ [ _val = METER_FOOD_CONSUMPTION ]
            |    tok.SetSupply_ [ _val = METER_SUPPLY ]
            |    tok.SetStealth_ [ _val = METER_STEALTH ]
            |    tok.SetDetection_ [ _val = METER_DETECTION ]
            |    tok.SetBattleSpeed_ [ _val = METER_BATTLE_SPEED ]
            |    tok.SetStarlaneSpeed_ [ _val = METER_STARLANE_SPEED ]
            ;
        static bool once = true;
        if (once) {
            retval.name("non-ship-part MeterType");
#if DEBUG_PARSERS
            debug(retval);
#endif
            once = false;
        }
        return retval;
    }

    enum_parser_rule<MeterType>::type& set_ship_part_meter_type_enum()
    {
        const parse::lexer& tok = parse::lexer::instance();
        static enum_parser_rule<MeterType>::type retval
            =    tok.SetDamage_ [ _val = METER_DAMAGE ]
            |    tok.SetROF_ [ _val = METER_ROF ]
            |    tok.SetRange_ [ _val = METER_RANGE ]
            |    tok.SetSpeed_ [ _val = METER_SPEED ]
            |    tok.SetCapacity_ [ _val = METER_CAPACITY ]
            |    tok.SetAntiShipDamage_ [ _val = METER_ANTI_SHIP_DAMAGE ]
            |    tok.SetAntiFighterDamage_ [ _val = METER_ANTI_FIGHTER_DAMAGE ]
            |    tok.SetLaunchRate_ [ _val = METER_LAUNCH_RATE ]
            |    tok.SetFighterWeaponRange_ [ _val = METER_FIGHTER_WEAPON_RANGE ]
            |    tok.SetStealth_ [ _val = METER_STEALTH ]
            |    tok.SetDetection_ [ _val = METER_DETECTION ]
            |    tok.SetStructure_ [ _val = METER_STRUCTURE ]
            ;
        static bool once = true;
        if (once) {
            retval.name("ship-part MeterType");
#if DEBUG_PARSERS
            debug(retval);
#endif
            once = false;
        }
        return retval;
    }

}
