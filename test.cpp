#define ADOBE_STD_SERIALIZATION

#include "Lexer.h"

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>

#include <GG/adobe/name.hpp>
#include <GG/adobe/array.hpp>

namespace parse { namespace detail {

    struct report_error_
    {
        template <typename Arg1, typename Arg2, typename Arg3, typename Arg4>
        struct result
        { typedef void type; };

        template <typename Arg1, typename Arg2, typename Arg3, typename Arg4>
        void operator()(Arg1 first, Arg2 last, Arg3 it, Arg4 rule_name) const
            {
                std::string error_string;
                generate_error_string(*first, *it, rule_name, first == last, error_string);
                send_error_string(error_string);
            }

        static boost::function<void (const std::string&)> send_error_string;
        static void default_send_error_string(const std::string& str);

    private:
        void generate_error_string(const token_type& first,
                                   const token_type& it,
                                   const boost::spirit::info& rule_name,
                                   bool at_end,
                                   std::string& str) const;
    };

    extern const boost::phoenix::function<report_error_> report_error;

} }

namespace parse { namespace detail {

    const boost::phoenix::function<report_error_> report_error;

    void report_error_::default_send_error_string(const std::string& str)
    { std::cerr << str; }

    void report_error_::generate_error_string(const token_type& first,
                                              const token_type& it,
                                              const boost::spirit::info& rule_name,
                                              bool at_end,
                                              std::string& str) const
    {
        std::stringstream is;

        is << it.filename() << ":" << (it.line_number() + 1) << ": "
           << "Parse error: expected " << rule_name;

        if (at_end) {
            is << " before end of input.\n";
        } else {
            std::string match(it.matched_range().first, it.matched_range().second);
            text_iterator it_start = it.matched_range().first;

            // Use the end of the token's matched range, if its entire match was
            // whitespace.
            if (match.find_first_not_of(" \t\n\r\f\v") == std::string::npos)
                it_start = it.matched_range().second;

            if (it_start < it.line_start())
                it_start = it.line_start();

            std::string line_start_though_it_start(it.line_start(), it_start);

            // expand tabs (ugh!)
            {
                std::string line_start_though_it_start_copy;
                std::swap(line_start_though_it_start_copy, line_start_though_it_start);
                line_start_though_it_start.clear();
                std::string tab(4, ' ');
                for (std::size_t i = 0; i < line_start_though_it_start_copy.size(); ++i) {
                    if (line_start_though_it_start_copy[i] == '\t')
                        line_start_though_it_start += tab;
                    else
                        line_start_though_it_start += line_start_though_it_start_copy[i];
                }
            }

            std::string position_indicator(line_start_though_it_start.size(), '~');

            is << " here:\n"
               << "  " << line_start_though_it_start << match << "\n"
               << "  " << position_indicator << '^'
               << std::endl;
        }

        str = is.str();
    }

    boost::function<void (const std::string&)> detail::report_error_::send_error_string =
        &detail::report_error_::default_send_error_string;

} }

struct lex_test_rules
{
    lex_test_rules(const parse::lexer& tok)
        {
            namespace qi = boost::spirit::qi;
            namespace phoenix = boost::phoenix;
            using qi::_1;
            using phoenix::val;

            rule
                =   * (
                           tok.inline_comment [ std::cout << val("<INLINE COMMENT>\n") ]
                        |  tok.end_of_line_comment [ std::cout << val("<END OF LINE COMMENT>\n") ]
                        |  tok.bool_ [ std::cout << _1 << " BOOL\n" ]
                        |  tok.int_ [ std::cout << _1 << " INT\n" ]
                        |  tok.double_ [ std::cout << _1 << " DOUBLE\n" ]
                        |  tok.string [ std::cout << _1 << " STRING\n" ]
                        |  tok.planet_size [ std::cout << _1 << " PLANET_SIZE\n" ]
                        |  tok.planet_type [ std::cout << _1 << " PLANET_TYPE\n" ]
                        |  tok.planet_environment [ std::cout << _1 << " PLANET_ENVIRONMENT\n" ]
                        |  tok.universe_object_type [ std::cout << _1 << " UNIVERSE_OBJECT_TYPE\n" ]
                        |  tok.star_type [ std::cout << _1 << " STAR_TYPE\n" ]
                        |  tok.empire_affiliation_type [ std::cout << _1 << " EMPIRE_AFFILIATION_TYPE\n" ]
                        |  tok.unlockable_item_type [ std::cout << _1 << " UNLOCKABLE_ITEM_TYPE\n" ]
                        |  tok.tech_type [ std::cout << _1 << " TECH_TYPE\n" ]
                        |  tok.combat_fighter_type [ std::cout << _1 << " COMBAT_FIGHTER_TYPE\n" ]
                        |  tok.ship_part_class [ std::cout << _1 << " SHIP_PART_CLASS\n" ]
                        |  tok.ship_slot_type [ std::cout << _1 << " SHIP_SLOT_TYPE\n" ]
                        |  tok.capture_result [ std::cout << _1 << " CAPTURE_RESULT\n" ]
                        |  tok.fleet [ std::cout << _1 << "\n" ]
                        |  tok.planet [ std::cout << _1 << "\n" ]
                        |  tok.system [ std::cout << _1 << "\n" ]
                        |  tok.asteroids [ std::cout << val("'asteroids'\n") ]
                        |  tok.gas_giant [ std::cout << val("'gasgiant'\n") ]
                        |  tok.building [ std::cout << val("'building'\n") ]
                        |  tok.fuel [ std::cout << _1 << "\n" ]
                        |  tok.shield [ std::cout << _1 << "\n" ]
                        |  tok.troops [ std::cout << _1 << "\n" ]
                        |  tok.stealth [ std::cout << _1 << "\n" ]
                        |  tok.detection [ std::cout << _1 << "\n" ]
                        |  tok.battle_speed [ std::cout << _1 << "\n" ]
                        |  tok.starlane_speed [ std::cout << _1 << "\n" ]
                        |  tok.source [ std::cout << _1 << "\n" ]
                        |  tok.target [ std::cout << _1 << "\n" ]
                        |  tok.localcandidate [ std::cout << _1 << "\n" ]
                        |  tok.rootcandidate [ std::cout << _1 << "\n" ]
                        |  tok.value [ std::cout << _1 << "\n" ]
                        |  tok.scope [ std::cout << _1 << "\n" ]
                        |  tok.activation [ std::cout << _1 << "\n" ]
                        |  tok.stacking_group [ std::cout << _1 << "\n" ]
                        |  tok.effects [ std::cout << _1 << "\n" ]
                        |  tok.name [ std::cout << _1 << "\n" ]
                        |  tok.description [ std::cout << _1 << "\n" ]
                        |  tok.short_description [ std::cout << _1 << "\n" ]
                        |  tok.build_cost [ std::cout << _1 << "\n" ]
                        |  tok.build_time [ std::cout << _1 << "\n" ]
                        |  tok.capture_result [ std::cout << _1 << "\n" ]
                        |  tok.effects_groups [ std::cout << _1 << "\n" ]
                        |  tok.graphic [ std::cout << _1 << "\n" ]
                        |  tok.tech_type [ std::cout << _1 << "\n" ]
                        |  tok.category [ std::cout << _1 << "\n" ]
                        |  tok.research_cost [ std::cout << _1 << "\n" ]
                        |  tok.research_turns [ std::cout << _1 << "\n" ]
                        |  tok.prerequisites [ std::cout << _1 << "\n" ]
                        |  tok.unlock [ std::cout << _1 << "\n" ]
                        |  tok.type [ std::cout << _1 << "\n" ]
                        |  tok.environment [ std::cout << _1 << "\n" ]
                        |  tok.environments [ std::cout << _1 << "\n" ]
                        |  tok.foci [ std::cout << _1 << "\n" ]
                        |  tok.location [ std::cout << _1 << "\n" ]
                        |  tok.class_ [ std::cout << _1 << "\n" ]
                        |  tok.speed [ std::cout << _1 << "\n" ]
                        |  tok.slots [ std::cout << _1 << "\n" ]
                        |  tok.mountable_slot_types [ std::cout << _1 << "\n" ]
                        |  tok.colour [ std::cout << _1 << "\n" ]
                        |  tok.position [ std::cout << _1 << "\n" ]
                        |  tok.damage [ std::cout << _1 << "\n" ]
                        |  tok.anti_ship_damage [ std::cout << _1 << "\n" ]
                        |  tok.anti_fighter_damage [ std::cout << _1 << "\n" ]
                        |  tok.rof [ std::cout << _1 << "\n" ]
                        |  tok.range [ std::cout << _1 << "\n" ]
                        |  tok.fighter_weapon_range [ std::cout << _1 << "\n" ]
                        |  tok.health [ std::cout << _1 << "\n" ]
                        |  tok.structure [ std::cout << _1 << "\n" ]
                        |  tok.fighter_type [ std::cout << _1 << "\n" ]
                        |  tok.launch_rate [ std::cout << _1 << "\n" ]
                        |  tok.capacity [ std::cout << _1 << "\n" ]
                        |  tok.hull [ std::cout << _1 << "\n" ]
                        |  tok.parts [ std::cout << _1 << "\n" ]
                        |  tok.ships [ std::cout << _1 << "\n" ]
                        |  tok.model [ std::cout << _1 << "\n" ]
                        |  tok.lookup_strings [ std::cout << _1 << "\n" ]
                        |  tok.researchable [ std::cout << _1 << "\n" ]
                        |  tok.producible [ std::cout << _1 << "\n" ]
                        |  tok.spawn_rate [ std::cout << _1 << "\n" ]
                        |  tok.spawn_limit [ std::cout << _1 << "\n" ]
                        |  tok.property [ std::cout << _1 << "\n" ]
                        |  tok.condition [ std::cout << _1 << "\n" ]
                        |  tok.statistic_type [ std::cout << _1 << " STATISTIC_TYPE\n" ]
                        |  tok.int_variable_final [ std::cout << _1 << " INT_VARIABLE_FINAL\n" ]
                        |  '='
                        |  '+'
                        |  '-'
                        |  '*'
                        |  '/'
                        |  '.'
                        |  ','
                        |  '('
                        |  ')'
                        |  '['
                        |  ']'
                      )
                ;

#define NAME(x)
// x.name(#x); debug(x)
            NAME(rule);
#undef NAME
        }

    typedef boost::spirit::qi::rule<
        parse::token_iterator,
        parse::skipper_type
    > test_rule;

    test_rule rule;
};

bool handle_token(parse::token_type token)
{
    const std::pair<std::string::const_iterator, std::string::const_iterator>& match = token.matched_range();
    std::cout << "Token='" << std::string(match.first, match.second) << "' value=" << token.value() << " type=" << token.value().which() << "\n";
    return true;
}

int main(int arc, char* argv[])
{
    parse::lexer lexer;
    lex_test_rules test(lexer);

    std::string str = argv[1];

    std::string::const_iterator first = str.begin();
    const std::string::const_iterator last = str.end();
    boost::spirit::lex::tokenize_and_phrase_parse(first, last, lexer, test.rule, boost::spirit::qi::in_state("WS")[lexer.self]);

    if (first == last)
        std::cout << "Successful parse!\n";
    else
        std::cout << "Could not parse the end: \"" << std::string(--first, last) << "\"\n";

    return 0;
}


// g++ test.cpp Lexer.cpp -I /usr/local/include/boost-1_47 -I ../GG -lGiGi

// ./test 'TIny small medium large huge asteroids gasgiant swamp toxic inferno radiated barren tundra desert terran ocean asteroids gasgiant true false "a" "afkghagjh"65.7 5 6. 5 [] = +-*/.,()/*!!!!*/ uninhabitable hostile poor adequate good building ship fleet planet populationcenter productioncenter system blue white yellow orange red neutron blackhole nostar theempire enemyof allyof anyempire building shippart shiphull tech theory application refinement interceptor bomber shortrange missiles fighters pointdefense shield armour troops detection stealth fuel colony battlespeed starlanespeed external internal capture destroy retain count sum mean rms mode max min spread stdev product owner id creationturn age producedbyempireid designid fleetid planetid systemid finaldestinationid nextsystemid previoussystemid numships farming targetfarming industry targetindustry research targetresearch trade targettrade mining targetmining construction targetconstruction population targetpopulation health targethealth maxfuel fuel maxshield shield maxdefense defense maxtroops troops maxstructure structure supply stealth detection foodconsumption battlespeed starlanespeed tradestockpile mineralstockpile foodstockpile distancetosource name species buildingtype focus source target localcandidate rootcandidate value property condition planetsize planettype nextbetterplanettype planetenvironment objecttype startype //sdfsfd'
