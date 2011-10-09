#include "test.h"

#include "../ConditionParser.h"

#include <GG/ReportParseError.h>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
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
    CASE(condition_parser);
    CASE(effect_parser);
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
        // Get rid of terminating FF char.
        if (!str.empty())
            str.resize(str.size() - 1);
    } else {
        str = argv[2];
    }

    std::vector<std::string> strings;
    boost::algorithm::split(strings,
                            str,
                            boost::algorithm::is_any_of("\n\r"),
                            boost::algorithm::token_compress_on);

    parse::lexer l;

    unsigned int failures = 0;
    for (std::size_t i = 0; i < strings.size(); ++i) {
        const std::string& string = strings[i];
        if (string.empty())
            continue;

        parse::text_iterator first(string.begin());
        const parse::text_iterator last(string.end());

        bool success = false;

        GG::detail::s_begin = first;
        GG::detail::s_end = last;
        GG::detail::s_filename = argc == 4 ? argv[3] : "command-line";
        parse::token_iterator it = l.begin(first, last);
        const parse::token_iterator end_it = l.end();

        try {
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
            case condition_parser: {
                success = boost::spirit::qi::phrase_parse(it, end_it, parse::condition_parser(l), boost::spirit::qi::in_state("WS")[l.self]);
                break;
            }
            case effect_parser: {
                std::cout << "Effect parser not yet implemented." << std::endl;
                exit(1);
                break;
            }
            default:
                break;
            }

            if (success && it == end_it) {
                std::cout <<  "Successful parse.\n";
            } else {
                std::cout <<  "Failed parse of \"" << string << "\".\n";
                ++failures;
            }
        } catch (const boost::spirit::qi::expectation_failure<parse::token_iterator>&) {
            std::cout <<  "Failed parse of \"" << string << "\" (qi::expectation_failure<> exception).\n";
            ++failures;
        }
    }

    if (1u < strings.size()) {
        if (failures)
            std::cout << failures << " failures total.\n";
        else
            std::cout << "All parses successful.\n";
    }

    return failures;
}
