#include "ValueRefParser.h"

#include "../universe/ValueRef.h"

#include <GG/ReportParseError.h>

#include <boost/spirit/home/phoenix.hpp>


#define HAVE_CONDITION_PARSER 0

namespace qi = boost::spirit::qi;
namespace phoenix = boost::phoenix;


#define DEBUG_VALUEREF_PARSE 0
#if DEBUG_VALUEREF_PARSE
#define NAME(x) x.name(#x); debug(x)
#else
#define NAME(x) x.name(#x)
#endif

// This is just here to satisfy the requirements of qi::debug(<rule>).
namespace adobe {
    std::ostream& operator<<(std::ostream& os, const std::vector<name_t>& name_vec)
    {
        os << "[ ";
        for (std::size_t i = 0; i < name_vec.size(); ++i) {
            os << name_vec[i] << " ";
        }
        os << "]";
        return os;
    }
}

namespace {

    typedef qi::rule<
        parse::token_iterator,
        adobe::name_t (),
        parse::skipper_type
    > name_token_rule;

    template <typename T>
    struct variable_rule
    {
        typedef qi::rule<
            parse::token_iterator,
            ValueRef::Variable<T>* (),
            qi::locals<std::vector<adobe::name_t> >,
            parse::skipper_type
        > type;
    };

#if HAVE_CONDITION_PARSER
    template <typename T>
    struct statistic_rule
    {
        typedef qi::rule<
            parse::token_iterator,
            ValueRef::ValueRefBase<T>* (),
            qi::locals<
                std::vector<adobe::name_t>,
                ValueRef::StatisticType,
                Condition::ConditionBase* // TODO: Change spelling to Condition::Base in the universe code.
            >,
            parse::skipper_type
        > type;
    };
#endif

    template <typename T>
    struct binary_op_expr_rule
    {
        typedef qi::rule<
            parse::token_iterator,
            ValueRef::ValueRefBase<T>* (),
            qi::locals<
                ValueRef::OpType,
                ValueRef::ValueRefBase<T>*,
                ValueRef::ValueRefBase<T>*
            >,
            parse::skipper_type
        > type;
    };

    template <typename T>
    void initialize_expression_parsers(
        typename parse::value_ref_parser_rule<T>::type& negate_expr,
        typename binary_op_expr_rule<T>::type& multiplicative_expr,
        typename binary_op_expr_rule<T>::type& additive_expr,
        typename parse::value_ref_parser_rule<T>::type& expr,
        typename parse::value_ref_parser_rule<T>::type& primary_expr
    )
    {
        using qi::_1;
        using qi::_a;
        using qi::_b;
        using qi::_c;
        using qi::_val;
        using qi::lit;
        using phoenix::new_;

        negate_expr
            =    '-' > primary_expr [ _val = new_<ValueRef::Operation<T> >(ValueRef::NEGATE, _1) ]
            |    primary_expr [ _val = _1 ]
            ;

        multiplicative_expr
            = (
                   negate_expr [ _b = _1 ]
               >>  (
                        lit('*') [_a = ValueRef::TIMES]
                    |   lit('/') [_a = ValueRef::DIVIDES]
                   )
               >   multiplicative_expr [ _c = _1 ]
              )
              [ _val = new_<ValueRef::Operation<T> >(_a, _b, _c) ]
            | negate_expr [ _val = _1 ]
            ;

        additive_expr
            = (
                   multiplicative_expr [ _b = _1 ]
               >>  (
                        lit('+') [_a = ValueRef::PLUS]
                    |   lit('-') [_a = ValueRef::MINUS]
                   )
               >   additive_expr [ _c = _1 ]
              )
              [ _val = new_<ValueRef::Operation<T> >(_a, _b, _c) ]
            | multiplicative_expr [ _val = _1 ]
            ;

        expr
            %=   additive_expr
            ;
    }

#if HAVE_CONDITION_PARSER
    template <typename T>
    void initialize_numeric_statistic_parser(
        typename statistic_rule<T>::type& statistic,
        const name_token_rule& final_token,
        const parse::lexer& tok
    )
    {
        statistic
            = (
                   (
                        tok.number [ _b = ValueRef::COUNT ]
                    >>  tok.condition
                    >   condition/*TODO!*/ [ _c = _1 ]
                   )
               |   (
                        tok.statistic_type_enum [ _b = _1 ]
                    >>  tok.property
                    >> -((tok.planet | tok.system | tok.fleet) [ push_back(_a, _1) ] > '.')
                    >   final_token [ push_back(_a, _1) ]
                    >   tok.condition
                    >   condition/*TODO!*/ [ _c = _1 ]
                   )
              )
              [ _val = new_<ValueRef::Statistic<T> >(_a, _b, _c) ]
            ;
    }

    template <typename T>
    void initialize_nonnumeric_statistic_parser(
        typename statistic_rule<T>::type& statistic,
        const name_token_rule& final_token,
        const parse::lexer& tok
    )
    {
        statistic
            = (
                   tok.statistic_type_enum [ _b = _1 ] // TODO: Should be "mode" only.
               >>  tok.property
               >> -((tok.planet | tok.system | tok.fleet) [ push_back(_a, _1) ] > '.')
               >   final_token [ push_back(_a, _1) ]
               >   tok.condition
               >   condition/*TODO!*/ [ _c = _1 ]
              )
              [ _val = new_<ValueRef::Statistic<T> >(_a, _b, _c) ]
            ;
    }
#endif

    struct int_parser_rules
    {
        int_parser_rules(const parse::lexer& tok)
            {
                using qi::_1;
                using qi::_2;
                using qi::_3;
                using qi::_4;
                using qi::_a;
                using qi::_val;
                using qi::lit;
                using phoenix::new_;
                using phoenix::push_back;
                using phoenix::static_cast_;

                first_token
                    %=   tok.source
                    |    tok.target
                    |    tok.local_candidate
                    |    tok.root_candidate
                    ;

                container_token
                    %=   tok.planet
                    |    tok.system
                    |    tok.fleet
                    ;

                // TODO: Should we apply elements of this list only to certain
                // containers?  For example, if one writes "Source.Planet.",
                // "NumShips" should not follow.
                final_token
                    %=   tok.owner
                    |    tok.id
                    |    tok.creation_turn
                    |    tok.age
                    |    tok.produced_by_empire_id
                    |    tok.design_id
                    |    tok.fleet_id
                    |    tok.planet_id
                    |    tok.system_id
                    |    tok.final_destination_id
                    |    tok.next_system_id
                    |    tok.previous_system_id
                    |    tok.num_ships
                    ;

                constant
                    =    tok.double_ [ _val = new_<ValueRef::Constant<int> >(static_cast_<int>(_1)) ]
                    |    tok.int_ [ _val = new_<ValueRef::Constant<int> >(_1) ]
                    ;

                variable
                    = (
                           (
                                first_token [ push_back(_a, _1) ] > '.'
                            >  -(container_token [ push_back(_a, _1) ] > '.')
                            >   final_token [ push_back(_a, _1) ]
                           )
                       |   (
                               tok.current_turn
                            |  tok.value
                           )
                           [ push_back(_a, _1) ]
                      )
                      [ _val = new_<ValueRef::Variable<int> >(_a) ]
                    ;

#if HAVE_CONDITION_PARSER
                initialize_numeric_statistic_parser<int>(statistic, final_token, tok);
#endif

                initialize_expression_parsers<int>(negate_expr,
                                                   multiplicative_expr,
                                                   additive_expr,
                                                   expr,
                                                   primary_expr);

                primary_expr
                    %=   '(' > expr > ')'
                    |    constant
                    |    variable
#if HAVE_CONDITION_PARSER
                    |    statistic
#endif
                    ;

                NAME(first_token);
                NAME(container_token);
                NAME(final_token);
                NAME(constant);
                NAME(variable);
#if HAVE_CONDITION_PARSER
                NAME(statistic);
#endif
                NAME(negate_expr);
                NAME(multiplicative_expr);
                NAME(additive_expr);
                NAME(expr);
                NAME(primary_expr);

                qi::on_error<qi::fail>(expr, parse::report_error(_1, _2, _3, _4));
            }

        typedef parse::value_ref_parser_rule<int>::type rule;
        typedef variable_rule<int>::type variable_rule;
#if HAVE_CONDITION_PARSER
        typedef statistic_rule<int>::type statistic_rule;
#endif
        typedef binary_op_expr_rule<int>::type binary_expression_rule;

        name_token_rule first_token;
        name_token_rule container_token;
        name_token_rule final_token;
        rule constant;
        variable_rule variable;
#if HAVE_CONDITION_PARSER
        statistic_rule statistic;
#endif
        rule negate_expr;
        binary_expression_rule multiplicative_expr;
        binary_expression_rule additive_expr;
        rule expr;
        rule primary_expr;
    };

    int_parser_rules& get_int_parser_rules(const parse::lexer& tok)
    {
        static int_parser_rules retval(tok);
        return retval;
    }

    struct double_parser_rules
    {
        double_parser_rules(const parse::lexer& tok)
            {
                using qi::_1;
                using qi::_a;
                using qi::_val;
                using phoenix::new_;
                using phoenix::push_back;
                using phoenix::static_cast_;

                name_token_rule& first_token = get_int_parser_rules(tok).first_token;
                name_token_rule& container_token = get_int_parser_rules(tok).container_token;

                final_token
                    %=   tok.farming
                    |    tok.target_farming
                    |    tok.industry
                    |    tok.target_industry
                    |    tok.research
                    |    tok.target_research
                    |    tok.trade
                    |    tok.target_trade
                    |    tok.mining
                    |    tok.target_mining
                    |    tok.construction
                    |    tok.target_construction
                    |    tok.population
                    |    tok.target_population
                    |    tok.health
                    |    tok.target_health
                    |    tok.max_fuel
                    |    tok.fuel
                    |    tok.max_shield
                    |    tok.shield
                    |    tok.max_defense
                    |    tok.defense
                    |    tok.max_troops
                    |    tok.troops
                    |    tok.max_structure
                    |    tok.structure
                    |    tok.supply
                    |    tok.stealth
                    |    tok.detection
                    |    tok.food_consumption
                    |    tok.battle_speed
                    |    tok.starlane_speed
                    |    tok.trade_stockpile
                    |    tok.mineral_stockpile
                    |    tok.food_stockpile
                    |    tok.distance_to_source
                    ;

                constant
                    =    tok.double_ [ _val = new_<ValueRef::Constant<double> >(_1) ]
                    |    tok.int_ [ _val = new_<ValueRef::Constant<double> >(static_cast_<double>(_1)) ]
                    ;

                variable
                    = (
                           first_token [ push_back(_a, _1) ] > '.'
                       >  -(container_token [ push_back(_a, _1) ] > '.')
                       >   (
                                final_token
                                [ push_back(_a, _1), _val = new_<ValueRef::Variable<double> >(_a) ]
                            |   get_int_parser_rules(tok).final_token
                                [ push_back(_a, _1), _val = new_<ValueRef::StaticCast<int, double> >(new_<ValueRef::Variable<int> >(_a)) ]
                           )
                      )
                    | (
                          tok.current_turn
                       |  tok.value
                      )
                      [ push_back(_a, _1), _val = new_<ValueRef::StaticCast<int, double> >(new_<ValueRef::Variable<int> >(_a)) ]
                    ;

#if HAVE_CONDITION_PARSER
                initialize_numeric_statistic_parser<double>(statistic, final_token, tok);
#endif

                initialize_expression_parsers<double>(negate_expr,
                                                      multiplicative_expr,
                                                      additive_expr,
                                                      expr,
                                                      primary_expr);

                primary_expr
                    %=   '(' > expr > ')'
                    |    constant
                    |    variable
#if HAVE_CONDITION_PARSER
                    |    statistic
#endif
                    ;

                NAME(final_token);
                NAME(constant);
                NAME(variable);
#if HAVE_CONDITION_PARSER
                NAME(statistic);
#endif
                NAME(negate_expr);
                NAME(multiplicative_expr);
                NAME(additive_expr);
                NAME(expr);
                NAME(primary_expr);

#if 0  // TODO: Fix this!
                qi::on_error<qi::fail>(expr, parse::report_error(_1, _2, _3, _4));
#endif
            }

        typedef parse::value_ref_parser_rule<double>::type rule;
        typedef variable_rule<double>::type variable_rule;
#if HAVE_CONDITION_PARSER
        typedef statistic_rule<double>::type statistic_rule;
#endif
        typedef binary_op_expr_rule<double>::type binary_expression_rule;

        name_token_rule final_token;
        rule constant;
        variable_rule variable;
#if HAVE_CONDITION_PARSER
        statistic_rule statistic;
#endif
        rule negate_expr;
        binary_expression_rule multiplicative_expr;
        binary_expression_rule additive_expr;
        rule expr;
        rule primary_expr;
    };

    double_parser_rules& get_double_parser_rules(const parse::lexer& tok)
    {
        static double_parser_rules retval(tok);
        return retval;
    }

    struct string_parser_rules
    {
        string_parser_rules(const parse::lexer& tok)
            {
                using qi::_1;
                using qi::_a;
                using qi::_val;
                using qi::as_string;
                using phoenix::push_back;
                using phoenix::new_;

                name_token_rule& first_token = get_int_parser_rules(tok).first_token;
                name_token_rule& container_token = get_int_parser_rules(tok).container_token;

                final_token
                    %=   tok.name
                    |    tok.species
                    |    tok.building_type
                    |    tok.focus
                    ;

                constant
                    =    tok.string [ _val = new_<ValueRef::Constant<std::string> >(_1) ]
#if 1 // TODO: Turn this on; verify that it works (requires Boost 1.47).
                    |    as_string [
                              tok.planet_size_enum
                          |   tok.planet_type_enum
                          |   tok.planet_environment_enum
                          |   tok.universe_object_type_enum
                          |   tok.star_type_enum
                          |   tok.double_
                          |   tok.int_
                         ]
#endif
                    ;

                variable
                    = (
                           first_token [ push_back(_a, _1) ] > '.'
                       >  -(container_token [ push_back(_a, _1) ] > '.')
                       >   (
                                final_token
                                [ push_back(_a, _1), _val = new_<ValueRef::Variable<std::string> >(_a) ]
                            |   get_int_parser_rules(tok).final_token
                                [ push_back(_a, _1), _val = new_<ValueRef::StringCast<int> >(new_<ValueRef::Variable<int> >(_a)) ]
                            |   get_double_parser_rules(tok).final_token
                                [ push_back(_a, _1), _val = new_<ValueRef::StringCast<double> >(new_<ValueRef::Variable<double> >(_a)) ]
                           )
                      )
                    | (
                          tok.current_turn
                       |  tok.value
                      )
                      [ push_back(_a, _1), _val = new_<ValueRef::StringCast<int> >(new_<ValueRef::Variable<int> >(_a)) ]
                    ;

#if HAVE_CONDITION_PARSER
                initialize_nonnumeric_statistic_parser<std::string>(statistic, final_token, tok);
#endif

                expr
                    %=   primary_expr
                    ;

                primary_expr
                    %=   '(' > expr > ')'
                    |    constant
                    |    variable
#if HAVE_CONDITION_PARSER
                    |    statistic
#endif
                    ;

                NAME(final_token);
                NAME(constant);
                NAME(variable);
#if HAVE_CONDITION_PARSER
                NAME(statistic);
#endif
                NAME(negate_expr);
                NAME(multiplicative_expr);
                NAME(additive_expr);
                NAME(expr);
                NAME(primary_expr);

#if 0  // TODO: Fix this!
                qi::on_error<qi::fail>(expr, parse::report_error(_1, _2, _3, _4));
#endif
            }

        typedef parse::value_ref_parser_rule<std::string>::type rule;
        typedef variable_rule<std::string>::type variable_rule;
#if HAVE_CONDITION_PARSER
        typedef statistic_rule<std::string>::type statistic_rule;
#endif
        typedef binary_op_expr_rule<std::string>::type binary_expression_rule;

        name_token_rule final_token;
        rule constant;
        variable_rule variable;
#if HAVE_CONDITION_PARSER
        statistic_rule statistic;
#endif
        rule negate_expr;
        binary_expression_rule multiplicative_expr;
        binary_expression_rule additive_expr;
        rule expr;
        rule primary_expr;
    };

    struct planet_size_parser_rules
    {
        planet_size_parser_rules(const parse::lexer& tok)
            {
                using qi::_1;
                using qi::_a;
                using qi::_val;
                using phoenix::new_;
                using phoenix::push_back;
                using phoenix::static_cast_;

                name_token_rule& first_token = get_int_parser_rules(tok).first_token;
                name_token_rule& container_token = get_int_parser_rules(tok).container_token;

                final_token
                    %=   tok.planet_size
                    ;

                constant
                    =    tok.planet_size_enum [ _val = new_<ValueRef::Constant<PlanetSize> >(_1) ]
                    |    tok.int_ [ _val = new_<ValueRef::Constant<PlanetSize> >(static_cast_<PlanetSize>(_1)) ]
                    ;

                variable
                    =    first_token [ push_back(_a, _1) ] > '.'
                    >   -(container_token [ push_back(_a, _1) ] > '.')
                    >    final_token [ push_back(_a, _1), _val = new_<ValueRef::Variable<PlanetSize> >(_a) ]
                    ;

#if HAVE_CONDITION_PARSER
                initialize_nonnumeric_statistic_parser<PlanetSize>(statistic, final_token, tok);
#endif

                primary_expr
                    %=   constant
                    |    variable
#if HAVE_CONDITION_PARSER
                    |    statistic
#endif
                    ;
            }

        typedef parse::value_ref_parser_rule<PlanetSize>::type rule;
        typedef variable_rule<PlanetSize>::type variable_rule;
#if HAVE_CONDITION_PARSER
        typedef statistic_rule<PlanetSize>::type statistic_rule;
#endif
        typedef binary_op_expr_rule<PlanetSize>::type binary_expression_rule;

        name_token_rule container_token;
        name_token_rule final_token;
        rule constant;
        variable_rule variable;
#if HAVE_CONDITION_PARSER
        statistic_rule statistic;
#endif
        rule primary_expr;
    };

    struct planet_type_parser_rules
    {
        planet_type_parser_rules(const parse::lexer& tok)
            {
                using qi::_1;
                using qi::_a;
                using qi::_val;
                using phoenix::new_;
                using phoenix::push_back;
                using phoenix::static_cast_;

                name_token_rule& first_token = get_int_parser_rules(tok).first_token;
                name_token_rule& container_token = get_int_parser_rules(tok).container_token;

                final_token
                    %=   tok.planet_type
                    |    tok.next_better_planet_type
                    ;

                constant
                    =    tok.planet_type_enum [ _val = new_<ValueRef::Constant<PlanetType> >(_1) ]
                    |    tok.int_ [ _val = new_<ValueRef::Constant<PlanetType> >(static_cast_<PlanetType>(_1)) ]
                    ;

                variable
                    =    first_token [ push_back(_a, _1) ] > '.'
                    >   -(container_token [ push_back(_a, _1) ] > '.')
                    >    final_token [ push_back(_a, _1), _val = new_<ValueRef::Variable<PlanetType> >(_a) ]
                    ;

#if HAVE_CONDITION_PARSER
                initialize_nonnumeric_statistic_parser<PlanetType>(statistic, final_token, tok);
#endif

                primary_expr
                    %=   constant
                    |    variable
#if HAVE_CONDITION_PARSER
                    |    statistic
#endif
                    ;
            }

        typedef parse::value_ref_parser_rule<PlanetType>::type rule;
        typedef variable_rule<PlanetType>::type variable_rule;
#if HAVE_CONDITION_PARSER
        typedef statistic_rule<PlanetType>::type statistic_rule;
#endif
        typedef binary_op_expr_rule<PlanetType>::type binary_expression_rule;

        name_token_rule container_token;
        name_token_rule final_token;
        rule constant;
        variable_rule variable;
#if HAVE_CONDITION_PARSER
        statistic_rule statistic;
#endif
        rule primary_expr;
    };

    struct planet_environment_parser_rules
    {
        planet_environment_parser_rules(const parse::lexer& tok)
            {
                using qi::_1;
                using qi::_a;
                using qi::_val;
                using phoenix::new_;
                using phoenix::push_back;
                using phoenix::static_cast_;

                name_token_rule& first_token = get_int_parser_rules(tok).first_token;
                name_token_rule& container_token = get_int_parser_rules(tok).container_token;

                final_token
                    %=   tok.planet_environment
                    ;

                constant
                    =    tok.planet_environment_enum [ _val = new_<ValueRef::Constant<PlanetEnvironment> >(_1) ]
                    |    tok.int_ [ _val = new_<ValueRef::Constant<PlanetEnvironment> >(static_cast_<PlanetEnvironment>(_1)) ]
                    ;

                variable
                    =    first_token [ push_back(_a, _1) ] > '.'
                    >   -(container_token [ push_back(_a, _1) ] > '.')
                    >    final_token [ push_back(_a, _1), _val = new_<ValueRef::Variable<PlanetEnvironment> >(_a) ]
                    ;

#if HAVE_CONDITION_PARSER
                initialize_nonnumeric_statistic_parser<PlanetEnvironment>(statistic, final_token, tok);
#endif

                primary_expr
                    %=   constant
                    |    variable
#if HAVE_CONDITION_PARSER
                    |    statistic
#endif
                    ;
            }

        typedef parse::value_ref_parser_rule<PlanetEnvironment>::type rule;
        typedef variable_rule<PlanetEnvironment>::type variable_rule;
#if HAVE_CONDITION_PARSER
        typedef statistic_rule<PlanetEnvironment>::type statistic_rule;
#endif
        typedef binary_op_expr_rule<PlanetEnvironment>::type binary_expression_rule;

        name_token_rule container_token;
        name_token_rule final_token;
        rule constant;
        variable_rule variable;
#if HAVE_CONDITION_PARSER
        statistic_rule statistic;
#endif
        rule primary_expr;
    };

    struct universe_object_type_parser_rules
    {
        universe_object_type_parser_rules(const parse::lexer& tok)
            {
                using qi::_1;
                using qi::_a;
                using qi::_val;
                using phoenix::new_;
                using phoenix::push_back;
                using phoenix::static_cast_;

                name_token_rule& first_token = get_int_parser_rules(tok).first_token;
                name_token_rule& container_token = get_int_parser_rules(tok).container_token;

                final_token
                    %=   tok.object_type
                    ;

                constant
                    =    tok.universe_object_type_enum [ _val = new_<ValueRef::Constant<UniverseObjectType> >(_1) ]
                    |    tok.int_ [ _val = new_<ValueRef::Constant<UniverseObjectType> >(static_cast_<UniverseObjectType>(_1)) ]
                    ;

                variable
                    =    first_token [ push_back(_a, _1) ] > '.'
                    >   -(container_token [ push_back(_a, _1) ] > '.')
                    >    final_token [ push_back(_a, _1), _val = new_<ValueRef::Variable<UniverseObjectType> >(_a) ]
                    ;


#if HAVE_CONDITION_PARSER
                initialize_nonnumeric_statistic_parser<UniverseObjectType>(statistic, final_token, tok);
#endif

                primary_expr
                    %=   constant
                    |    variable
#if HAVE_CONDITION_PARSER
                    |    statistic
#endif
                    ;
            }

        typedef parse::value_ref_parser_rule<UniverseObjectType>::type rule;
        typedef variable_rule<UniverseObjectType>::type variable_rule;
#if HAVE_CONDITION_PARSER
        typedef statistic_rule<UniverseObjectType>::type statistic_rule;
#endif
        typedef binary_op_expr_rule<UniverseObjectType>::type binary_expression_rule;

        name_token_rule container_token;
        name_token_rule final_token;
        rule constant;
        variable_rule variable;
#if HAVE_CONDITION_PARSER
        statistic_rule statistic;
#endif
        rule primary_expr;
    };

    struct star_type_parser_rules
    {
        star_type_parser_rules(const parse::lexer& tok)
            {
                using qi::_1;
                using qi::_a;
                using qi::_val;
                using phoenix::new_;
                using phoenix::push_back;
                using phoenix::static_cast_;

                name_token_rule& first_token = get_int_parser_rules(tok).first_token;
                name_token_rule& container_token = get_int_parser_rules(tok).container_token;

                final_token
                    %=   tok.star_type
                    ;

                constant
                    =    tok.star_type_enum [ _val = new_<ValueRef::Constant<StarType> >(_1) ]
                    |    tok.int_ [ _val = new_<ValueRef::Constant<StarType> >(static_cast_<StarType>(_1)) ]
                    ;

                variable
                    =    first_token [ push_back(_a, _1) ] > '.'
                    >   -(container_token [ push_back(_a, _1) ] > '.')
                    >    final_token [ push_back(_a, _1), _val = new_<ValueRef::Variable<StarType> >(_a) ]
                    ;

#if HAVE_CONDITION_PARSER
                initialize_nonnumeric_statistic_parser<StarType>(statistic, final_token, tok);
#endif

                primary_expr
                    %=   constant
                    |    variable
#if HAVE_CONDITION_PARSER
                    |    statistic
#endif
                    ;
            }

        typedef parse::value_ref_parser_rule<StarType>::type rule;
        typedef variable_rule<StarType>::type variable_rule;
#if HAVE_CONDITION_PARSER
        typedef statistic_rule<StarType>::type statistic_rule;
#endif
        typedef binary_op_expr_rule<StarType>::type binary_expression_rule;

        name_token_rule container_token;
        name_token_rule final_token;
        rule constant;
        variable_rule variable;
#if HAVE_CONDITION_PARSER
        statistic_rule statistic;
#endif
        rule primary_expr;
    };

}


namespace parse {

    template <>
    const value_ref_parser_rule<int>::type& value_ref_parser<int>(const parse::lexer& tok)
    { return get_int_parser_rules(tok).expr; }

    template <>
    const value_ref_parser_rule<double>::type& value_ref_parser<double>(const parse::lexer& tok)
    { return get_double_parser_rules(tok).expr; }

    template <>
    const value_ref_parser_rule<std::string>::type& value_ref_parser<std::string>(const parse::lexer& tok)
    {
        static const string_parser_rules retval(tok);
        return retval.expr;
    }

    template <>
    const value_ref_parser_rule<PlanetSize>::type& value_ref_parser<PlanetSize>(const parse::lexer& tok)
    {
        static const planet_size_parser_rules retval(tok);
        return retval.primary_expr;
    }

    template <>
    const value_ref_parser_rule<PlanetType>::type& value_ref_parser<PlanetType>(const parse::lexer& tok)
    {
        static const planet_type_parser_rules retval(tok);
        return retval.primary_expr;
    }

    template <>
    const value_ref_parser_rule<PlanetEnvironment>::type& value_ref_parser<PlanetEnvironment>(const parse::lexer& tok)
    {
        static const planet_environment_parser_rules retval(tok);
        return retval.primary_expr;
    }

    template <>
    const value_ref_parser_rule<UniverseObjectType>::type& value_ref_parser<UniverseObjectType>(const parse::lexer& tok)
    {
        static const universe_object_type_parser_rules retval(tok);
        return retval.primary_expr;
    }

    template <>
    const value_ref_parser_rule<StarType>::type& value_ref_parser<StarType>(const parse::lexer& tok)
    {
        static const star_type_parser_rules retval(tok);
        return retval.primary_expr;
    }

}
