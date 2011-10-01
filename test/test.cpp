#include "test.h"

#include <GG/ReportParseError.h>

#include <fstream>


void print_help()
{
    std::cout << "Usage: test lexer|int_value_ref_parser|double_value_ref_parser|string_value_ref_parser|planet_size_value_ref_parser|planet_type_value_ref_parser|planet_environment_value_ref_parser|universe_object_type_value_ref_parser|star_type_value_ref_parser <-f filename>|<test string>" << std::endl;
}

int main(int argc, char* argv[])
{
    if (argc < 3) {
        print_help();
        exit(1);
    }

    const std::string test_str = argv[1];
    test_type test = unknown;
#define CASE(x) if (test_str == #x) test = x
    CASE(lexer);
    CASE(int_value_ref_parser);
    CASE(double_value_ref_parser);
    CASE(string_value_ref_parser);
    CASE(planet_size_value_ref_parser);
    CASE(planet_type_value_ref_parser);
    CASE(planet_environment_value_ref_parser);
    CASE(universe_object_type_value_ref_parser);
    CASE(star_type_value_ref_parser);
#undef CASE

    if (test == unknown) {
        print_help();
        exit(1);
    }

    std::string str;
    if (argc == 4) {
        if (std::string(argv[2]) != "-f") {
            print_help();
            exit(1);
        }
        std::ifstream ifs(argv[3]);
        while (ifs) {
            str += ifs.get();
        }
    } else {
        str = argv[2];
    }

    parse::text_iterator first(str.begin());
    const parse::text_iterator last(str.end());

    parse::lexer l;

    bool success = false;

    GG::detail::s_begin = first;
    GG::detail::s_end = last;
    GG::detail::s_filename = argc == 4 ? argv[3] : "command-line";
    parse::token_iterator it = l.begin(first, last);
    const parse::token_iterator end_it = l.end();

    switch (test) {
    case lexer: {
        lexer_test_rules rules(l);
        success = boost::spirit::qi::phrase_parse(it, end_it, rules.lexer, boost::spirit::qi::in_state("WS")[l.self]);
        break;
    }
    case int_value_ref_parser: {
        success = boost::spirit::qi::phrase_parse(it, end_it, parse::value_ref_parser<int>(l), boost::spirit::qi::in_state("WS")[l.self]);
        break;
    }
    case double_value_ref_parser: {
        success = boost::spirit::qi::phrase_parse(it, end_it, parse::value_ref_parser<double>(l), boost::spirit::qi::in_state("WS")[l.self]);
        break;
    }
    case string_value_ref_parser: {
        success = boost::spirit::qi::phrase_parse(it, end_it, parse::value_ref_parser<std::string>(l), boost::spirit::qi::in_state("WS")[l.self]);
        break;
    }
    case planet_size_value_ref_parser: {
        success = boost::spirit::qi::phrase_parse(it, end_it, parse::value_ref_parser<PlanetSize>(l), boost::spirit::qi::in_state("WS")[l.self]);
        break;
    }
    case planet_type_value_ref_parser: {
        success = boost::spirit::qi::phrase_parse(it, end_it, parse::value_ref_parser<PlanetType>(l), boost::spirit::qi::in_state("WS")[l.self]);
        break;
    }
    case planet_environment_value_ref_parser: {
        success = boost::spirit::qi::phrase_parse(it, end_it, parse::value_ref_parser<PlanetEnvironment>(l), boost::spirit::qi::in_state("WS")[l.self]);
        break;
    }
    case universe_object_type_value_ref_parser: {
        success = boost::spirit::qi::phrase_parse(it, end_it, parse::value_ref_parser<UniverseObjectType>(l), boost::spirit::qi::in_state("WS")[l.self]);
        break;
    }
    case star_type_value_ref_parser: {
        success = boost::spirit::qi::phrase_parse(it, end_it, parse::value_ref_parser<StarType>(l), boost::spirit::qi::in_state("WS")[l.self]);
        break;
    }
    default:
        break;
    }

    if (success && it == end_it)
        std::cout <<  "Successful parse.\n";
    else
        std::cout <<  "Failed parse.\n";

    return 0;
}

// ./test lexer 'tiny small medium large huge asteroids gasgiant swamp toxic inferno radiated barren tundra desert terran ocean asteroids gasgiant true false "a" "afkghagjh"65.7 5 6. 5 [] = +-*/.,()/*!!!!*/ uninhabitable hostile poor adequate good building ship fleet planet populationcenter productioncenter system blue white yellow orange red neutron blackhole nostar theempire enemyof allyof anyempire building shippart shiphull tech theory application refinement interceptor bomber shortrange missiles fighters pointdefense shield armour troops detection stealth fuel colony battlespeed starlanespeed external internal capture destroy retain count sum mean rms mode max min spread stdev product owner id creationturn age producedbyempireid designid fleetid planetid systemid finaldestinationid nextsystemid previoussystemid numships farming targetfarming industry targetindustry research targetresearch trade targettrade mining targetmining construction targetconstruction population targetpopulation health targethealth maxfuel fuel maxshield shield maxdefense defense maxtroops troops maxstructure structure supply stealth detection foodconsumption battlespeed starlanespeed tradestockpile mineralstockpile foodstockpile distancetosource name species buildingtype focus source target localcandidate rootcandidate value property condition planetsize planettype nextbetterplanettype planetenvironment objecttype startype activation addedbefore addedsince affiliation age alignment all and antifighterdamage antishipdamage armed asteroids battlespeed buildcost building buildingtype buildtime capacity capital captureresult category class colour condition construction containedby contains createdonturn creationturn currentturn damage defense description design designhashull designhaspart designhaspartclass designid detection distance distancetosource effects effectsgroup effectsgroups empire environment environments exploredbyempire farming fightertype fighterweaponrange finaldestinationid fleet fleetid fleetsupplyablebyempire foci focus foodconsumption foodstockpile fuel gasgiant graphic hasspecial health high homeworld hull id industry insystem item jumps launchrate localcandidate location lookup_strings low maxdefense maxfuel maximumnumberof maxshield maxstructure maxtroops mineralstockpile minimumnumberof mining model modenumberof monster monsterfleet mountableslottypes name nextbetterplanettype nextsystemid not number numberof numships object objecttype or ownedby owner ownerfoodstockpile ownerhastech ownermineralstockpile ownertradestockpile part parts planet planetbound planetenvironment planetid planetsize planettype population position prerequisites previoussystemid probability producedbyempire producedbyempireid producible property random range research researchable researchcost researchturns resourcesupplyconnectedbyempire rof rootcandidate scope shield shipdesign ships short_description size slot slots sortby source spacebound spawnlimit spawnrate special species speed stackinggroup star starlanespeed startype stationary stealth structure supply system systemid target targetconstruction targetfarming targethealth targetindustry targetmining targetpopulation targetresearch targettrade tech techtype trade tradestockpile troops turn type unlock value visibletoempire withindistance withinstarlanejumps //sdfsfd'

/*

All the int value refs below should be run through the double and std::string
value ref parsers too.  Similarly, all the double value refs below should be
run through the std::string value ref parser.

int_value_ref_parser "1"
int_value_ref_parser "-1"
int_value_ref_parser "- 1"
int_value_ref_parser "(1)"
int_value_ref_parser "(-1)"
int_value_ref_parser "(- 1)"
int_value_ref_parser "((1))"
int_value_ref_parser "(((-1)))"
int_value_ref_parser "((((- 1))))"
int_value_ref_parser "1 + 2"
int_value_ref_parser "-1+2"
int_value_ref_parser "- 1 +2"
int_value_ref_parser "1 * 1 + 2"
int_value_ref_parser "-1+2* 2"
int_value_ref_parser "- 1* 3 +2 * 4"
int_value_ref_parser "1 * 1 + 2 +1"
int_value_ref_parser "-1+2* 2 + 1"
int_value_ref_parser "- 1* 3 +2 * 4 + 1"
int_value_ref_parser "1 * 1 + 2 *1"
int_value_ref_parser "-1+2* 2 * 1"
int_value_ref_parser "- 1* 3 +2 * 4 * 1"

int_value_ref_parser "Source.Owner"
int_value_ref_parser "Source.ID"
int_value_ref_parser "Source.CreationTurn"
int_value_ref_parser "Source.Age"
int_value_ref_parser "Source.ProducedByEmpireID"
int_value_ref_parser "Source.DesignID"
int_value_ref_parser "Source.FleetID"
int_value_ref_parser "Source.PlanetID"
int_value_ref_parser "Source.SystemID"
int_value_ref_parser "Source.FinalDestinationID"
int_value_ref_parser "Source.NextSystemID"
int_value_ref_parser "Source.PreviousSystemID"
int_value_ref_parser "Source.NumShips"

int_value_ref_parser "1 + 1 - Source.NumShips / 1 * 1"

int_value_ref_parser "Target.Owner"
int_value_ref_parser "Target.ID"
int_value_ref_parser "Target.CreationTurn"
int_value_ref_parser "Target.Age"
int_value_ref_parser "Target.ProducedByEmpireID"
int_value_ref_parser "Target.DesignID"
int_value_ref_parser "Target.FleetID"
int_value_ref_parser "Target.PlanetID"
int_value_ref_parser "Target.SystemID"
int_value_ref_parser "Target.FinalDestinationID"
int_value_ref_parser "Target.NextSystemID"
int_value_ref_parser "Target.PreviousSystemID"
int_value_ref_parser "Target.NumShips"

int_value_ref_parser "Source.System.Owner"
int_value_ref_parser "Source.System.ID"
int_value_ref_parser "Source.System.CreationTurn"
int_value_ref_parser "Source.System.Age"
int_value_ref_parser "Source.System.ProducedByEmpireID"
int_value_ref_parser "Source.System.DesignID"
int_value_ref_parser "Source.System.FleetID"
int_value_ref_parser "Source.System.PlanetID"
int_value_ref_parser "Source.System.SystemID"
int_value_ref_parser "Source.System.FinalDestinationID"
int_value_ref_parser "Source.System.NextSystemID"
int_value_ref_parser "Source.System.PreviousSystemID"
int_value_ref_parser "Source.System.NumShips"

int_value_ref_parser "Target.System.Owner"
int_value_ref_parser "Target.System.ID"
int_value_ref_parser "Target.System.CreationTurn"
int_value_ref_parser "Target.System.Age"
int_value_ref_parser "Target.System.ProducedByEmpireID"
int_value_ref_parser "Target.System.DesignID"
int_value_ref_parser "Target.System.FleetID"
int_value_ref_parser "Target.System.PlanetID"
int_value_ref_parser "Target.System.SystemID"
int_value_ref_parser "Target.System.FinalDestinationID"
int_value_ref_parser "Target.System.NextSystemID"
int_value_ref_parser "Target.System.PreviousSystemID"
int_value_ref_parser "Target.System.NumShips"

int_value_ref_parser "Source.Planet.Owner"
int_value_ref_parser "Source.Planet.ID"
int_value_ref_parser "Source.Planet.CreationTurn"
int_value_ref_parser "Source.Planet.Age"
int_value_ref_parser "Source.Planet.ProducedByEmpireID"
int_value_ref_parser "Source.Planet.DesignID"
int_value_ref_parser "Source.Planet.FleetID"
int_value_ref_parser "Source.Planet.PlanetID"
int_value_ref_parser "Source.Planet.SystemID"
int_value_ref_parser "Source.Planet.FinalDestinationID"
int_value_ref_parser "Source.Planet.NextSystemID"
int_value_ref_parser "Source.Planet.PreviousSystemID"
int_value_ref_parser "Source.Planet.NumShips"

int_value_ref_parser "Target.Planet.Owner"
int_value_ref_parser "Target.Planet.ID"
int_value_ref_parser "Target.Planet.CreationTurn"
int_value_ref_parser "Target.Planet.Age"
int_value_ref_parser "Target.Planet.ProducedByEmpireID"
int_value_ref_parser "Target.Planet.DesignID"
int_value_ref_parser "Target.Planet.FleetID"
int_value_ref_parser "Target.Planet.PlanetID"
int_value_ref_parser "Target.Planet.SystemID"
int_value_ref_parser "Target.Planet.FinalDestinationID"
int_value_ref_parser "Target.Planet.NextSystemID"
int_value_ref_parser "Target.Planet.PreviousSystemID"
int_value_ref_parser "Target.Planet.NumShips"

int_value_ref_parser "Source.Fleet.Owner"
int_value_ref_parser "Source.Fleet.ID"
int_value_ref_parser "Source.Fleet.CreationTurn"
int_value_ref_parser "Source.Fleet.Age"
int_value_ref_parser "Source.Fleet.ProducedByEmpireID"
int_value_ref_parser "Source.Fleet.DesignID"
int_value_ref_parser "Source.Fleet.FleetID"
int_value_ref_parser "Source.Fleet.PlanetID"
int_value_ref_parser "Source.Fleet.SystemID"
int_value_ref_parser "Source.Fleet.FinalDestinationID"
int_value_ref_parser "Source.Fleet.NextSystemID"
int_value_ref_parser "Source.Fleet.PreviousSystemID"
int_value_ref_parser "Source.Fleet.NumShips"

int_value_ref_parser "Target.Fleet.Owner"
int_value_ref_parser "Target.Fleet.ID"
int_value_ref_parser "Target.Fleet.CreationTurn"
int_value_ref_parser "Target.Fleet.Age"
int_value_ref_parser "Target.Fleet.ProducedByEmpireID"
int_value_ref_parser "Target.Fleet.DesignID"
int_value_ref_parser "Target.Fleet.FleetID"
int_value_ref_parser "Target.Fleet.PlanetID"
int_value_ref_parser "Target.Fleet.SystemID"
int_value_ref_parser "Target.Fleet.FinalDestinationID"
int_value_ref_parser "Target.Fleet.NextSystemID"
int_value_ref_parser "Target.Fleet.PreviousSystemID"
int_value_ref_parser "Target.Fleet.NumShips"

int_value_ref_parser "CurrentTurn"
int_value_ref_parser "Value"

double_value_ref_parser "1.0"
double_value_ref_parser "-1.0"
double_value_ref_parser "- 1.0"
double_value_ref_parser "(1.0)"
double_value_ref_parser "(-1.0)"
double_value_ref_parser "(- 1.0)"
double_value_ref_parser "((1.0))"
double_value_ref_parser "(((-1.0)))"
double_value_ref_parser "((((- 1.0))))"
double_value_ref_parser "1.0 + 2.0"
double_value_ref_parser "-1.0+2.0"
double_value_ref_parser "- 1.0 +2.0"
double_value_ref_parser "1.0 * 1.0 + 2.0"
double_value_ref_parser "-1.0+2.0* 2.0"
double_value_ref_parser "- 1.0* 3.0 +2.0 * 4.0"
double_value_ref_parser "1.0 * 1.0 + 2.0 +1.0"
double_value_ref_parser "-1.0+2.0* 2.0 + 1.0"
double_value_ref_parser "- 1.0* 3.0 +2.0 * 4.0 + 1.0"
double_value_ref_parser "1.0 * 1.0 + 2.0 *1.0"
double_value_ref_parser "-1.0+2.0* 2.0 * 1.0"
double_value_ref_parser "- 1.0* 3.0 +2.0 * 4.0 * 1.0"

double_value_ref_parser "Source.Farming"
double_value_ref_parser "Source.TargetFarming"
double_value_ref_parser "Source.Industry"
double_value_ref_parser "Source.TargetIndustry"
double_value_ref_parser "Source.Research"
double_value_ref_parser "Source.TargetResearch"
double_value_ref_parser "Source.Trade"
double_value_ref_parser "Source.TargetTrade"
double_value_ref_parser "Source.Mining"
double_value_ref_parser "Source.TargetMining"
double_value_ref_parser "Source.Construction"
double_value_ref_parser "Source.TargetConstruction"
double_value_ref_parser "Source.Population"
double_value_ref_parser "Source.TargetPopulation"
double_value_ref_parser "Source.Health"
double_value_ref_parser "Source.TargetHealth"
double_value_ref_parser "Source.MaxFuel"
double_value_ref_parser "Source.Fuel"
double_value_ref_parser "Source.MaxShield"
double_value_ref_parser "Source.Shield"
double_value_ref_parser "Source.MaxDefense"
double_value_ref_parser "Source.Defense"
double_value_ref_parser "Source.MaxTroops"
double_value_ref_parser "Source.Troops"
double_value_ref_parser "Source.MaxStructure"
double_value_ref_parser "Source.Structure"
double_value_ref_parser "Source.Supply"
double_value_ref_parser "Source.Stealth"
double_value_ref_parser "Source.Detection"
double_value_ref_parser "Source.FoodConsumption"
double_value_ref_parser "Source.BattleSpeed"
double_value_ref_parser "Source.StarlaneSpeed"
double_value_ref_parser "Source.TradeStockpile"
double_value_ref_parser "Source.MineralStockpile"
double_value_ref_parser "Source.FoodStockpile"
double_value_ref_parser "Source.DistanceToSource"

int_value_ref_parser "1 + 1 - Source.FoodStockpile / 1 * 1"

double_value_ref_parser "Target.Farming"
double_value_ref_parser "Target.TargetFarming"
double_value_ref_parser "Target.Industry"
double_value_ref_parser "Target.TargetIndustry"
double_value_ref_parser "Target.Research"
double_value_ref_parser "Target.TargetResearch"
double_value_ref_parser "Target.Trade"
double_value_ref_parser "Target.TargetTrade"
double_value_ref_parser "Target.Mining"
double_value_ref_parser "Target.TargetMining"
double_value_ref_parser "Target.Construction"
double_value_ref_parser "Target.TargetConstruction"
double_value_ref_parser "Target.Population"
double_value_ref_parser "Target.TargetPopulation"
double_value_ref_parser "Target.Health"
double_value_ref_parser "Target.TargetHealth"
double_value_ref_parser "Target.MaxFuel"
double_value_ref_parser "Target.Fuel"
double_value_ref_parser "Target.MaxShield"
double_value_ref_parser "Target.Shield"
double_value_ref_parser "Target.MaxDefense"
double_value_ref_parser "Target.Defense"
double_value_ref_parser "Target.MaxTroops"
double_value_ref_parser "Target.Troops"
double_value_ref_parser "Target.MaxStructure"
double_value_ref_parser "Target.Structure"
double_value_ref_parser "Target.Supply"
double_value_ref_parser "Target.Stealth"
double_value_ref_parser "Target.Detection"
double_value_ref_parser "Target.FoodConsumption"
double_value_ref_parser "Target.BattleSpeed"
double_value_ref_parser "Target.StarlaneSpeed"
double_value_ref_parser "Target.TradeStockpile"
double_value_ref_parser "Target.MineralStockpile"
double_value_ref_parser "Target.FoodStockpile"
double_value_ref_parser "Target.DistanceToSource"

double_value_ref_parser "Source.System.Farming"
double_value_ref_parser "Source.System.TargetFarming"
double_value_ref_parser "Source.System.Industry"
double_value_ref_parser "Source.System.TargetIndustry"
double_value_ref_parser "Source.System.Research"
double_value_ref_parser "Source.System.TargetResearch"
double_value_ref_parser "Source.System.Trade"
double_value_ref_parser "Source.System.TargetTrade"
double_value_ref_parser "Source.System.Mining"
double_value_ref_parser "Source.System.TargetMining"
double_value_ref_parser "Source.System.Construction"
double_value_ref_parser "Source.System.TargetConstruction"
double_value_ref_parser "Source.System.Population"
double_value_ref_parser "Source.System.TargetPopulation"
double_value_ref_parser "Source.System.Health"
double_value_ref_parser "Source.System.TargetHealth"
double_value_ref_parser "Source.System.MaxFuel"
double_value_ref_parser "Source.System.Fuel"
double_value_ref_parser "Source.System.MaxShield"
double_value_ref_parser "Source.System.Shield"
double_value_ref_parser "Source.System.MaxDefense"
double_value_ref_parser "Source.System.Defense"
double_value_ref_parser "Source.System.MaxTroops"
double_value_ref_parser "Source.System.Troops"
double_value_ref_parser "Source.System.MaxStructure"
double_value_ref_parser "Source.System.Structure"
double_value_ref_parser "Source.System.Supply"
double_value_ref_parser "Source.System.Stealth"
double_value_ref_parser "Source.System.Detection"
double_value_ref_parser "Source.System.FoodConsumption"
double_value_ref_parser "Source.System.BattleSpeed"
double_value_ref_parser "Source.System.StarlaneSpeed"
double_value_ref_parser "Source.System.TradeStockpile"
double_value_ref_parser "Source.System.MineralStockpile"
double_value_ref_parser "Source.System.FoodStockpile"
double_value_ref_parser "Source.System.DistanceToSource"

double_value_ref_parser "Target.System.Farming"
double_value_ref_parser "Target.System.TargetFarming"
double_value_ref_parser "Target.System.Industry"
double_value_ref_parser "Target.System.TargetIndustry"
double_value_ref_parser "Target.System.Research"
double_value_ref_parser "Target.System.TargetResearch"
double_value_ref_parser "Target.System.Trade"
double_value_ref_parser "Target.System.TargetTrade"
double_value_ref_parser "Target.System.Mining"
double_value_ref_parser "Target.System.TargetMining"
double_value_ref_parser "Target.System.Construction"
double_value_ref_parser "Target.System.TargetConstruction"
double_value_ref_parser "Target.System.Population"
double_value_ref_parser "Target.System.TargetPopulation"
double_value_ref_parser "Target.System.Health"
double_value_ref_parser "Target.System.TargetHealth"
double_value_ref_parser "Target.System.MaxFuel"
double_value_ref_parser "Target.System.Fuel"
double_value_ref_parser "Target.System.MaxShield"
double_value_ref_parser "Target.System.Shield"
double_value_ref_parser "Target.System.MaxDefense"
double_value_ref_parser "Target.System.Defense"
double_value_ref_parser "Target.System.MaxTroops"
double_value_ref_parser "Target.System.Troops"
double_value_ref_parser "Target.System.MaxStructure"
double_value_ref_parser "Target.System.Structure"
double_value_ref_parser "Target.System.Supply"
double_value_ref_parser "Target.System.Stealth"
double_value_ref_parser "Target.System.Detection"
double_value_ref_parser "Target.System.FoodConsumption"
double_value_ref_parser "Target.System.BattleSpeed"
double_value_ref_parser "Target.System.StarlaneSpeed"
double_value_ref_parser "Target.System.TradeStockpile"
double_value_ref_parser "Target.System.MineralStockpile"
double_value_ref_parser "Target.System.FoodStockpile"
double_value_ref_parser "Target.System.DistanceToSource"

double_value_ref_parser "Source.Planet.Farming"
double_value_ref_parser "Source.Planet.TargetFarming"
double_value_ref_parser "Source.Planet.Industry"
double_value_ref_parser "Source.Planet.TargetIndustry"
double_value_ref_parser "Source.Planet.Research"
double_value_ref_parser "Source.Planet.TargetResearch"
double_value_ref_parser "Source.Planet.Trade"
double_value_ref_parser "Source.Planet.TargetTrade"
double_value_ref_parser "Source.Planet.Mining"
double_value_ref_parser "Source.Planet.TargetMining"
double_value_ref_parser "Source.Planet.Construction"
double_value_ref_parser "Source.Planet.TargetConstruction"
double_value_ref_parser "Source.Planet.Population"
double_value_ref_parser "Source.Planet.TargetPopulation"
double_value_ref_parser "Source.Planet.Health"
double_value_ref_parser "Source.Planet.TargetHealth"
double_value_ref_parser "Source.Planet.MaxFuel"
double_value_ref_parser "Source.Planet.Fuel"
double_value_ref_parser "Source.Planet.MaxShield"
double_value_ref_parser "Source.Planet.Shield"
double_value_ref_parser "Source.Planet.MaxDefense"
double_value_ref_parser "Source.Planet.Defense"
double_value_ref_parser "Source.Planet.MaxTroops"
double_value_ref_parser "Source.Planet.Troops"
double_value_ref_parser "Source.Planet.MaxStructure"
double_value_ref_parser "Source.Planet.Structure"
double_value_ref_parser "Source.Planet.Supply"
double_value_ref_parser "Source.Planet.Stealth"
double_value_ref_parser "Source.Planet.Detection"
double_value_ref_parser "Source.Planet.FoodConsumption"
double_value_ref_parser "Source.Planet.BattleSpeed"
double_value_ref_parser "Source.Planet.StarlaneSpeed"
double_value_ref_parser "Source.Planet.TradeStockpile"
double_value_ref_parser "Source.Planet.MineralStockpile"
double_value_ref_parser "Source.Planet.FoodStockpile"
double_value_ref_parser "Source.Planet.DistanceToSource"

double_value_ref_parser "Target.Planet.Farming"
double_value_ref_parser "Target.Planet.TargetFarming"
double_value_ref_parser "Target.Planet.Industry"
double_value_ref_parser "Target.Planet.TargetIndustry"
double_value_ref_parser "Target.Planet.Research"
double_value_ref_parser "Target.Planet.TargetResearch"
double_value_ref_parser "Target.Planet.Trade"
double_value_ref_parser "Target.Planet.TargetTrade"
double_value_ref_parser "Target.Planet.Mining"
double_value_ref_parser "Target.Planet.TargetMining"
double_value_ref_parser "Target.Planet.Construction"
double_value_ref_parser "Target.Planet.TargetConstruction"
double_value_ref_parser "Target.Planet.Population"
double_value_ref_parser "Target.Planet.TargetPopulation"
double_value_ref_parser "Target.Planet.Health"
double_value_ref_parser "Target.Planet.TargetHealth"
double_value_ref_parser "Target.Planet.MaxFuel"
double_value_ref_parser "Target.Planet.Fuel"
double_value_ref_parser "Target.Planet.MaxShield"
double_value_ref_parser "Target.Planet.Shield"
double_value_ref_parser "Target.Planet.MaxDefense"
double_value_ref_parser "Target.Planet.Defense"
double_value_ref_parser "Target.Planet.MaxTroops"
double_value_ref_parser "Target.Planet.Troops"
double_value_ref_parser "Target.Planet.MaxStructure"
double_value_ref_parser "Target.Planet.Structure"
double_value_ref_parser "Target.Planet.Supply"
double_value_ref_parser "Target.Planet.Stealth"
double_value_ref_parser "Target.Planet.Detection"
double_value_ref_parser "Target.Planet.FoodConsumption"
double_value_ref_parser "Target.Planet.BattleSpeed"
double_value_ref_parser "Target.Planet.StarlaneSpeed"
double_value_ref_parser "Target.Planet.TradeStockpile"
double_value_ref_parser "Target.Planet.MineralStockpile"
double_value_ref_parser "Target.Planet.FoodStockpile"
double_value_ref_parser "Target.Planet.DistanceToSource"

double_value_ref_parser "Source.Fleet.Farming"
double_value_ref_parser "Source.Fleet.TargetFarming"
double_value_ref_parser "Source.Fleet.Industry"
double_value_ref_parser "Source.Fleet.TargetIndustry"
double_value_ref_parser "Source.Fleet.Research"
double_value_ref_parser "Source.Fleet.TargetResearch"
double_value_ref_parser "Source.Fleet.Trade"
double_value_ref_parser "Source.Fleet.TargetTrade"
double_value_ref_parser "Source.Fleet.Mining"
double_value_ref_parser "Source.Fleet.TargetMining"
double_value_ref_parser "Source.Fleet.Construction"
double_value_ref_parser "Source.Fleet.TargetConstruction"
double_value_ref_parser "Source.Fleet.Population"
double_value_ref_parser "Source.Fleet.TargetPopulation"
double_value_ref_parser "Source.Fleet.Health"
double_value_ref_parser "Source.Fleet.TargetHealth"
double_value_ref_parser "Source.Fleet.MaxFuel"
double_value_ref_parser "Source.Fleet.Fuel"
double_value_ref_parser "Source.Fleet.MaxShield"
double_value_ref_parser "Source.Fleet.Shield"
double_value_ref_parser "Source.Fleet.MaxDefense"
double_value_ref_parser "Source.Fleet.Defense"
double_value_ref_parser "Source.Fleet.MaxTroops"
double_value_ref_parser "Source.Fleet.Troops"
double_value_ref_parser "Source.Fleet.MaxStructure"
double_value_ref_parser "Source.Fleet.Structure"
double_value_ref_parser "Source.Fleet.Supply"
double_value_ref_parser "Source.Fleet.Stealth"
double_value_ref_parser "Source.Fleet.Detection"
double_value_ref_parser "Source.Fleet.FoodConsumption"
double_value_ref_parser "Source.Fleet.BattleSpeed"
double_value_ref_parser "Source.Fleet.StarlaneSpeed"
double_value_ref_parser "Source.Fleet.TradeStockpile"
double_value_ref_parser "Source.Fleet.MineralStockpile"
double_value_ref_parser "Source.Fleet.FoodStockpile"
double_value_ref_parser "Source.Fleet.DistanceToSource"

double_value_ref_parser "Target.Fleet.Farming"
double_value_ref_parser "Target.Fleet.TargetFarming"
double_value_ref_parser "Target.Fleet.Industry"
double_value_ref_parser "Target.Fleet.TargetIndustry"
double_value_ref_parser "Target.Fleet.Research"
double_value_ref_parser "Target.Fleet.TargetResearch"
double_value_ref_parser "Target.Fleet.Trade"
double_value_ref_parser "Target.Fleet.TargetTrade"
double_value_ref_parser "Target.Fleet.Mining"
double_value_ref_parser "Target.Fleet.TargetMining"
double_value_ref_parser "Target.Fleet.Construction"
double_value_ref_parser "Target.Fleet.TargetConstruction"
double_value_ref_parser "Target.Fleet.Population"
double_value_ref_parser "Target.Fleet.TargetPopulation"
double_value_ref_parser "Target.Fleet.Health"
double_value_ref_parser "Target.Fleet.TargetHealth"
double_value_ref_parser "Target.Fleet.MaxFuel"
double_value_ref_parser "Target.Fleet.Fuel"
double_value_ref_parser "Target.Fleet.MaxShield"
double_value_ref_parser "Target.Fleet.Shield"
double_value_ref_parser "Target.Fleet.MaxDefense"
double_value_ref_parser "Target.Fleet.Defense"
double_value_ref_parser "Target.Fleet.MaxTroops"
double_value_ref_parser "Target.Fleet.Troops"
double_value_ref_parser "Target.Fleet.MaxStructure"
double_value_ref_parser "Target.Fleet.Structure"
double_value_ref_parser "Target.Fleet.Supply"
double_value_ref_parser "Target.Fleet.Stealth"
double_value_ref_parser "Target.Fleet.Detection"
double_value_ref_parser "Target.Fleet.FoodConsumption"
double_value_ref_parser "Target.Fleet.BattleSpeed"
double_value_ref_parser "Target.Fleet.StarlaneSpeed"
double_value_ref_parser "Target.Fleet.TradeStockpile"
double_value_ref_parser "Target.Fleet.MineralStockpile"
double_value_ref_parser "Target.Fleet.FoodStockpile"
double_value_ref_parser "Target.Fleet.DistanceToSource"

double_value_ref_parser "CurrentTurn"
double_value_ref_parser "Value"

*/
