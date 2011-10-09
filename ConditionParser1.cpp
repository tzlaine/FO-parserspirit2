#include "ConditionParserImpl.h"

#include "ValueRefParser.h"
#include "../universe/Condition.h"

#include <GG/ReportParseError.h>

#include <boost/spirit/home/phoenix.hpp>


namespace qi = boost::spirit::qi;
namespace phoenix = boost::phoenix;


#define NAME(x) x.name(#x)
//; debug(x)

namespace {

    struct condition_parser_rules_1
    {
        condition_parser_rules_1(const parse::lexer& tok)
            {
                const parse::value_ref_parser_rule<int>::type& int_value_ref =
                    parse::value_ref_parser<int>(tok);

                const parse::value_ref_parser_rule<std::string>::type& string_value_ref =
                    parse::value_ref_parser<std::string>(tok);

                using qi::_1;
                using qi::_a;
                using qi::_b;
                using qi::_val;
                using phoenix::new_;
                using phoenix::push_back;

                string_ref_vec
                    =    (
                              '['
                          >  +string_value_ref [ push_back(_val, _1) ]
                          >   ']'
                         )
                    |    string_value_ref [ push_back(_val, _1) ]
                    ;

                all
                    =    tok.All_ [ _val = new_<Condition::All>() ]
                    ;

                source
                    =    tok.Source_ [ _val = new_<Condition::Source>() ]
                    ;

                root_candidate
                    =    tok.RootCandidate_ [ _val = new_<Condition::RootCandidate>() ]
                    ;

                target
                    =    tok.Target_ [ _val = new_<Condition::Target>() ]
                    ;

                stationary
                    =    tok.Stationary_ [ _val = new_<Condition::Stationary>() ]
                    ;

                capital
                    =    tok.Capital_ [ _val = new_<Condition::Capital>() ]
                    ;

                monster
                    =    tok.Monster_ [ _val = new_<Condition::Monster>() ]
                    ;

                armed
                    =    tok.Armed_ [ _val = new_<Condition::Armed>() ]
                    ;

                owned_by
                    =    (
                              tok.OwnedBy_
                          >   tok.Affiliation_ > '='
                          >   tok.empire_affiliation_type_enum [ _a = _1 ]
                          >   tok.Empire_ > '='
                          >   int_value_ref [ _val = new_<Condition::EmpireAffiliation>(_1, _a) ]
                         )
                    |    (
                              tok.OwnedBy_
                          >   tok.Affiliation_ > '='
                          >   tok.empire_affiliation_type_enum [ _val = new_<Condition::EmpireAffiliation>(_1) ]
                         )
                    ;

                homeworld
                    =    (
                              tok.Homeworld_
                          >   tok.Name_ > '='
                          >   string_ref_vec [ _val = new_<Condition::Homeworld>(_1) ]
                         )
                    |    tok.Homeworld_ [ _val = new_<Condition::Homeworld>() ]
                    ;

                building
                    =    tok.Building_
                    >    tok.Name_ > '='
                    >    string_ref_vec [ _val = new_<Condition::Building>(_1) ]
                    ;

                species
                    =    (
                              tok.Species_
                          >>  tok.Name_ > '='
                          >   string_ref_vec [ _val = new_<Condition::Species>(_1) ]
                         )
                    |    tok.Species_ [ _val = new_<Condition::Species>() ]
                    ;

                focus_type
                    =    (
                              tok.FocusType_
                          >>  tok.Type_ > '='
                          >   string_ref_vec [ _val = new_<Condition::FocusType>(_1) ]
                         )
                    |    tok.FocusType_ [ _val = new_<Condition::FocusType>(std::vector<const ValueRef::ValueRefBase<std::string>*>()) ]
                    ;

                start
                    %=   all
                    |    source
                    |    root_candidate
                    |    target
                    |    stationary
                    |    capital
                    |    monster
                    |    armed
                    |    owned_by
                    |    homeworld
                    |    building
                    |    species
                    |    focus_type
                    |    planet_type
                    |    planet_size
                    |    planet_environment
                    |    object_type
                    |    meter_value
                    |    and_
                    |    or_
                    |    not_
                    ;
            }

        typedef boost::spirit::qi::rule<
            parse::token_iterator,
            std::vector<const ValueRef::ValueRefBase<std::string>*> (),
            parse::skipper_type
        > string_ref_vec_rule;

        typedef boost::spirit::qi::rule<
            parse::token_iterator,
            Condition::ConditionBase* (),
            qi::locals<EmpireAffiliationType>,
            parse::skipper_type
        > owned_by_rule;

        typedef boost::spirit::qi::rule<
            parse::token_iterator,
            Condition::ConditionBase* (),
            qi::locals<
                MeterType,
                ValueRef::ValueRefBase<double>*,
                ValueRef::ValueRefBase<double>*
            >,
            parse::skipper_type
        > value_ref_ints_rule;

        string_ref_vec_rule string_ref_vec;
        parse::condition_parser_rule all;
        parse::condition_parser_rule source;
        parse::condition_parser_rule root_candidate;
        parse::condition_parser_rule target;
        parse::condition_parser_rule stationary;
        parse::condition_parser_rule capital;
        parse::condition_parser_rule monster;
        parse::condition_parser_rule armed;
        owned_by_rule owned_by;
        parse::condition_parser_rule homeworld;
        parse::condition_parser_rule building;
        parse::condition_parser_rule species;
        parse::condition_parser_rule focus_type;
        parse::condition_parser_rule planet_type;
        parse::condition_parser_rule planet_size;
        parse::condition_parser_rule planet_environment;
        parse::condition_parser_rule object_type;
        value_ref_ints_rule meter_value;
        parse::condition_parser_rule and_;
        parse::condition_parser_rule or_;
        parse::condition_parser_rule not_;
        parse::condition_parser_rule start;
    };

}

namespace parse { namespace detail {

    const condition_parser_rule& condition_parser_1(const lexer& tok)
    {
        static condition_parser_rules_1 retval(tok);
        return retval.start;
    }

} }
