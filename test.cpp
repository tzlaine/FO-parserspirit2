#include "test.h"

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

    std::string::const_iterator first = str.begin();
    const std::string::const_iterator last = str.end();

    parse::lexer l;

    bool success = false;

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

    if (success) {
        if (it == end_it)
            std::cout << "Successful parse!\n";
        else
            std::cout << "Could not parse the end: \"" << std::string(--first, last) << "\"\n";
    } else {
        std::cout << "Could not parse the end: \"" << std::string(--first, last) << "\"\n";
    }

    return 0;
}

// ./test lexer 'tiny small medium large huge asteroids gasgiant swamp toxic inferno radiated barren tundra desert terran ocean asteroids gasgiant true false "a" "afkghagjh"65.7 5 6. 5 [] = +-*/.,()/*!!!!*/ uninhabitable hostile poor adequate good building ship fleet planet populationcenter productioncenter system blue white yellow orange red neutron blackhole nostar theempire enemyof allyof anyempire building shippart shiphull tech theory application refinement interceptor bomber shortrange missiles fighters pointdefense shield armour troops detection stealth fuel colony battlespeed starlanespeed external internal capture destroy retain count sum mean rms mode max min spread stdev product owner id creationturn age producedbyempireid designid fleetid planetid systemid finaldestinationid nextsystemid previoussystemid numships farming targetfarming industry targetindustry research targetresearch trade targettrade mining targetmining construction targetconstruction population targetpopulation health targethealth maxfuel fuel maxshield shield maxdefense defense maxtroops troops maxstructure structure supply stealth detection foodconsumption battlespeed starlanespeed tradestockpile mineralstockpile foodstockpile distancetosource name species buildingtype focus source target localcandidate rootcandidate value property condition planetsize planettype nextbetterplanettype planetenvironment objecttype startype activation addedbefore addedsince affiliation age alignment all and antifighterdamage antishipdamage armed asteroids battlespeed buildcost building buildingtype buildtime capacity capital captureresult category class colour condition construction containedby contains createdonturn creationturn currentturn damage defense description design designhashull designhaspart designhaspartclass designid detection distance distancetosource effects effectsgroup effectsgroups empire environment environments exploredbyempire farming fightertype fighterweaponrange finaldestinationid fleet fleetid fleetsupplyablebyempire foci focus foodconsumption foodstockpile fuel gasgiant graphic hasspecial health high homeworld hull id industry insystem item jumps launchrate localcandidate location lookup_strings low maxdefense maxfuel maximumnumberof maxshield maxstructure maxtroops mineralstockpile minimumnumberof mining model modenumberof monster monsterfleet mountableslottypes name nextbetterplanettype nextsystemid not number numberof numships object objecttype or ownedby owner ownerfoodstockpile ownerhastech ownermineralstockpile ownertradestockpile part parts planet planetbound planetenvironment planetid planetsize planettype population position prerequisites previoussystemid probability producedbyempire producedbyempireid producible property random range research researchable researchcost researchturns resourcesupplyconnectedbyempire rof rootcandidate scope shield shipdesign ships short_description size slot slots sortby source spacebound spawnlimit spawnrate special species speed stackinggroup star starlanespeed startype stationary stealth structure supply system systemid target targetconstruction targetfarming targethealth targetindustry targetmining targetpopulation targetresearch targettrade tech techtype trade tradestockpile troops turn type unlock value visibletoempire withindistance withinstarlanejumps //sdfsfd'
