#include "test.h"

#include "../ConditionParser.h"
#include "../EffectParser.h"
#include "../universe/ValueRef.h"

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include <fstream>


void print_help()
{
    std::cout << "Usage: test lexer|int_value_ref_parser|double_value_ref_parser|string_value_ref_parser|planet_size_value_ref_parser|planet_type_value_ref_parser|planet_environment_value_ref_parser|universe_object_type_value_ref_parser|star_type_value_ref_parser|int_value_ref_evaluation|double_value_ref_evaluation|condition_parser|effect_parser <-f filename>|<test string>" << std::endl;
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
    CASE(int_value_ref_evaluation);
    CASE(double_value_ref_evaluation);
    CASE(condition_parser);
    CASE(effect_parser);
#undef CASE

    // TODO: Add error reporting mode that shows what the error messages look
    // like if you strip out one token at a time from the input strings.

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

    const parse::lexer& l = parse::lexer::instance();

    unsigned int failures = 0;
    for (std::size_t i = 0; i < strings.size(); ++i) {
        const std::string& string = strings[i];
        if (string.empty())
            continue;

        const std::size_t equals_position =
            test == int_value_ref_evaluation || test == double_value_ref_evaluation ?
            string.find('=') :
            std::string::npos;

        parse::text_iterator first(string.begin());
        const parse::text_iterator last(equals_position == std::string::npos ? string.end() : string.begin() + equals_position);

        bool success = false;

        GG::detail::s_begin = first;
        GG::detail::s_end = last;
        GG::detail::s_filename = argc == 4 ? argv[3] : "command-line";
        parse::token_iterator it = l.begin(first, last);
        const parse::token_iterator end_it = l.end();

        boost::spirit::qi::in_state_type in_state;

        try {
            switch (test) {
            case lexer: {
                lexer_test_rules rules;
                success = boost::spirit::qi::phrase_parse(it, end_it, rules.lexer, in_state("WS")[l.self]);
                break;
            }
            case int_value_ref_parser: {
                success = boost::spirit::qi::phrase_parse(it, end_it, parse::value_ref_parser<int>(), in_state("WS")[l.self]);
                break;
            }
            case double_value_ref_parser: {
                success = boost::spirit::qi::phrase_parse(it, end_it, parse::value_ref_parser<double>(), in_state("WS")[l.self]);
                break;
            }
            case string_value_ref_parser: {
                success = boost::spirit::qi::phrase_parse(it, end_it, parse::value_ref_parser<std::string>(), in_state("WS")[l.self]);
                break;
            }
            case planet_size_value_ref_parser: {
                success = boost::spirit::qi::phrase_parse(it, end_it, parse::value_ref_parser<PlanetSize>(), in_state("WS")[l.self]);
                break;
            }
            case planet_type_value_ref_parser: {
                success = boost::spirit::qi::phrase_parse(it, end_it, parse::value_ref_parser<PlanetType>(), in_state("WS")[l.self]);
                break;
            }
            case planet_environment_value_ref_parser: {
                success = boost::spirit::qi::phrase_parse(it, end_it, parse::value_ref_parser<PlanetEnvironment>(), in_state("WS")[l.self]);
                break;
            }
            case universe_object_type_value_ref_parser: {
                success = boost::spirit::qi::phrase_parse(it, end_it, parse::value_ref_parser<UniverseObjectType>(), in_state("WS")[l.self]);
                break;
            }
            case star_type_value_ref_parser: {
                success = boost::spirit::qi::phrase_parse(it, end_it, parse::value_ref_parser<StarType>(), in_state("WS")[l.self]);
                break;
            }
            case int_value_ref_evaluation: {
                ValueRef::ValueRefBase<int>* result = 0;
                success = boost::spirit::qi::phrase_parse(it, end_it, parse::value_ref_parser<int>(), in_state("WS")[l.self], result);
                assert(equals_position != std::string::npos);
                const int expected_result = boost::lexical_cast<int>(string.substr(equals_position + 1, -1));
                if (result->Eval() == expected_result) {
                    std::cout <<  "Correct evaluation." << std::endl;
                } else {
                    std::cout <<  "Incorrect evaluation of \"" << string << "\".  Got " << result->Eval() << "." << std::endl;
                    ++failures;
                }
                break;
            }
            case double_value_ref_evaluation: {
                ValueRef::ValueRefBase<double>* result = 0;
                success = boost::spirit::qi::phrase_parse(it, end_it, parse::value_ref_parser<double>(), in_state("WS")[l.self], result);
                assert(equals_position != std::string::npos);
                const double expected_result = boost::lexical_cast<double>(string.substr(equals_position + 1, -1));
                if (result->Eval() == expected_result) {
                    std::cout <<  "Correct evaluation." << std::endl;
                } else {
                    std::cout <<  "Incorrect evaluation of \"" << string << "\".  Got " << result->Eval() << "." << std::endl;
                    ++failures;
                }
                break;
            }
            case condition_parser: {
                success = boost::spirit::qi::phrase_parse(it, end_it, parse::condition_parser(), in_state("WS")[l.self]);
                break;
            }
            case effect_parser: {
                success = boost::spirit::qi::phrase_parse(it, end_it, parse::effect_parser(), in_state("WS")[l.self]);
                break;
            }
            default:
                break;
            }

            if (success && it == end_it) {
                if (test != int_value_ref_evaluation && test != double_value_ref_evaluation)
                    std::cout <<  "Successful parse." << std::endl;
            } else {
                std::cout <<  "Failed parse of \"" << string << "\"." << std::endl;
                ++failures;
            }
        } catch (const boost::spirit::qi::expectation_failure<parse::token_iterator>&) {
            std::cout <<  "Failed parse of \"" << string << "\" (qi::expectation_failure<> exception)." << std::endl;
            ++failures;
        }
    }

    if (1u < strings.size()) {
        if (failures)
            std::cout << failures << " failures total." << std::endl;
        else
            std::cout << "All parses successful." << std::endl;
    }

    return failures;
}
