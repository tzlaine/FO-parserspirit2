#include "ConditionParserImpl.h"

#include "EnumParser.h"
#include "Label.h"
#include "ValueRefParser.h"
#include "../universe/Condition.h"
#include "../universe/ValueRef.h"

#include <GG/ReportParseError.h>

#include <boost/spirit/home/phoenix.hpp>


namespace qi = boost::spirit::qi;
namespace phoenix = boost::phoenix;


#define NAME(x) x.name(#x)
//; debug(x)

namespace {

    struct condition_parser_rules_1
    {
        condition_parser_rules_1()
            {
                const parse::lexer& tok = parse::lexer::instance();

                const parse::value_ref_parser_rule<int>::type& int_value_ref =
                    parse::value_ref_parser<int>();

                const parse::value_ref_parser_rule<double>::type& double_value_ref =
                    parse::value_ref_parser<double>();

                const parse::value_ref_parser_rule<std::string>::type& string_value_ref =
                    parse::value_ref_parser<std::string>();

                const parse::value_ref_parser_rule<PlanetType>::type& planet_type_value_ref =
                    parse::value_ref_parser<PlanetType>();

                const parse::value_ref_parser_rule<PlanetSize>::type& planet_size_value_ref =
                    parse::value_ref_parser<PlanetSize>();

                const parse::value_ref_parser_rule<PlanetEnvironment>::type& planet_environment_value_ref =
                    parse::value_ref_parser<PlanetEnvironment>();

                const parse::value_ref_parser_rule<UniverseObjectType>::type& universe_object_type_value_ref =
                    parse::value_ref_parser<UniverseObjectType>();

                qi::_1_type _1;
                qi::_2_type _2;
                qi::_3_type _3;
                qi::_4_type _4;
                qi::_a_type _a;
                qi::_b_type _b;
                qi::_val_type _val;
                qi::eps_type eps;
                qi::lit_type lit;
                using phoenix::new_;
                using phoenix::push_back;

                string_ref_vec
                    =    '[' > +string_value_ref [ push_back(_val, _1) ] > ']'
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
                    =    tok.OwnedBy_
                    >>   parse::label(Affiliation_name) >> parse::enum_parser<EmpireAffiliationType>() [ _a = _1 ]
                    >>   (
                              parse::label(Empire_name) >> int_value_ref [ _val = new_<Condition::EmpireAffiliation>(_1, _a) ]
                          |   eps [ _val = new_<Condition::EmpireAffiliation>(_a) ]
                         )
                    ;

                homeworld
                    =    (
                              tok.Homeworld_
                          >>  parse::label(Name_name) >> string_ref_vec [ _val = new_<Condition::Homeworld>(_1) ]
                         )
                    |    tok.Homeworld_ [ _val = new_<Condition::Homeworld>() ]
                    ;

                building
                    =    tok.Building_
                    >    parse::label(Name_name) > string_ref_vec [ _val = new_<Condition::Building>(_1) ]
                    ;

                species
                    =    (
                              tok.Species_
                          >>  parse::label(Name_name) >> string_ref_vec [ _val = new_<Condition::Species>(_1) ]
                         )
                    |    tok.Species_ [ _val = new_<Condition::Species>() ]
                    ;

                focus_type
                    =    (
                              tok.FocusType_
                          >>  parse::label(Type_name) >> string_ref_vec [ _val = new_<Condition::FocusType>(_1) ]
                         )
                    |    tok.FocusType_ [ _val = new_<Condition::FocusType>(std::vector<const ValueRef::ValueRefBase<std::string>*>()) ]
                    ;

                planet_type
                    =    tok.Planet_
                    >>   parse::label(Type_name)
                    >>   (
                              '[' >> +planet_type_value_ref [ push_back(_a, _1) ] >> ']'
                          |   planet_type_value_ref [ push_back(_a, _1) ]
                         )
                         [ _val = new_<Condition::PlanetType>(_a) ]
                    ;

                planet_size
                    =    tok.Planet_
                    >>   parse::label(PlanetSize_name)
                    >>   (
                              '[' >> +planet_size_value_ref [ push_back(_a, _1) ] >> ']'
                          |   planet_size_value_ref [ push_back(_a, _1) ]
                         )
                         [ _val = new_<Condition::PlanetSize>(_a) ]
                    ;

                planet_environment
                    =    tok.Planet_
                    >>   parse::label(Environment_name)
                    >>   (
                              '[' > +planet_environment_value_ref [ push_back(_a, _1) ] > ']'
                          |   planet_environment_value_ref [ push_back(_a, _1) ]
                         )
                         [ _val = new_<Condition::PlanetEnvironment>(_a) ]
                    ;

                object_type
                    =    parse::enum_parser<UniverseObjectType>() [ _val = new_<Condition::Type>(new_<ValueRef::Constant<UniverseObjectType> >(_1)) ]
                    |    (
                              tok.ObjectType_
                          >   parse::label(Type_name) > universe_object_type_value_ref [ _val = new_<Condition::Type>(_1) ]
                         )
                    ;

                meter_value
                    =    parse::non_ship_part_meter_type_enum() [ _a = _1 ]
                    >    parse::label(Low_name)  > double_value_ref [ _b = _1 ]
                    >    parse::label(High_name) > double_value_ref [ _val = new_<Condition::MeterValue>(_a, _b, _1) ]
                    ;

                and_
                    =    tok.And_
                    >    '[' > parse::detail::condition_parser [ push_back(_a, _1) ] > +parse::detail::condition_parser [ push_back(_a, _1) ] > lit(']')
                         [ _val = new_<Condition::And>(_a) ]
                    ;

                or_
                    =    tok.Or_
                    >    '[' > parse::detail::condition_parser [ push_back(_a, _1) ] > +parse::detail::condition_parser [ push_back(_a, _1) ] > lit(']')
                         [ _val = new_<Condition::Or>(_a) ]
                    ;

                not_
                    =    tok.Not_
                    >    parse::detail::condition_parser [ _val = new_<Condition::Not>(_1) ]
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

                NAME(string_ref_vec);
                NAME(all);
                NAME(source);
                NAME(root_candidate);
                NAME(target);
                NAME(stationary);
                NAME(capital);
                NAME(monster);
                NAME(armed);
                NAME(owned_by);
                NAME(homeworld);
                NAME(building);
                NAME(species);
                NAME(focus_type);
                NAME(planet_type);
                NAME(planet_size);
                NAME(planet_environment);
                NAME(object_type);
                NAME(meter_value);
                NAME(and_);
                NAME(or_);
                NAME(not_);
                NAME(start);

                qi::on_error<qi::fail>(start, parse::report_error(_1, _2, _3, _4));
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
                ValueRef::ValueRefBase<double>*
            >,
            parse::skipper_type
        > meter_value_rule;

        typedef boost::spirit::qi::rule<
            parse::token_iterator,
            Condition::ConditionBase* (),
            qi::locals<std::vector<const ValueRef::ValueRefBase<PlanetType>*> >,
            parse::skipper_type
        > planet_type_rule;

        typedef boost::spirit::qi::rule<
            parse::token_iterator,
            Condition::ConditionBase* (),
            qi::locals<std::vector<const ValueRef::ValueRefBase<PlanetSize>*> >,
            parse::skipper_type
        > planet_size_rule;

        typedef boost::spirit::qi::rule<
            parse::token_iterator,
            Condition::ConditionBase* (),
            qi::locals<std::vector<const ValueRef::ValueRefBase<PlanetEnvironment>*> >,
            parse::skipper_type
        > planet_environment_rule;

        typedef boost::spirit::qi::rule<
            parse::token_iterator,
            Condition::ConditionBase* (),
            qi::locals<std::vector<const Condition::ConditionBase*> >,
            parse::skipper_type
        > and_or_rule;

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
        planet_type_rule planet_type;
        planet_size_rule planet_size;
        planet_environment_rule planet_environment;
        parse::condition_parser_rule object_type;
        meter_value_rule meter_value;
        and_or_rule and_;
        and_or_rule or_;
        parse::condition_parser_rule not_;
        parse::condition_parser_rule start;
    };

}

namespace parse { namespace detail {

    const condition_parser_rule& condition_parser_1()
    {
        static condition_parser_rules_1 retval;
        return retval.start;
    }

} }
