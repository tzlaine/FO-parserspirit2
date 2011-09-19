#include "ValueRefParser.h"

#include "ValueRef.h"

#include <boost/spirit/home/phoenix.hpp>


#define HAVE_CONDITION_PARSER 0

namespace qi = boost::spirit::qi;
namespace phoenix = boost::phoenix;

using qi::_1;
using qi::_a;
using qi::_b;
using qi::_c;
using qi::_val;
using qi::double_;
using qi::int_;
using qi::lit;
using phoenix::new_;
using phoenix::push_back;
using phoenix::ref;
using phoenix::static_cast_;

namespace {

    typedef qi::rule<
        parse::token_iterator,
        adobe::name_t (),
        parse::skipper_type
    > name_token_rule;

    const name_token_rule& variable_container(const parse::lexer& tok)
    {
        static name_token_rule retval;
        static bool once;
        if (once) {
            retval
                %=   tok.planet
                |    tok.system
                |    tok.fleet
                ;
            once = false;
        }
        return retval;
    }

    template <typename T>
    struct variable_rule
    {
        typedef qi::rule<
            parse::token_iterator,
            ValueRef::ValueRefBase<T>* (),
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
                ValueRef::ValueRefBase<T>*,
                ValueRef::OpType,
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
        negate_expr
            =    (
                      '-'
                  >   primary_expr [ _val = new_<ValueRef::Operation<T> >(ValueRef::NEGATE, _1) ]
                 )
            |    primary_expr [ _val = _1 ]
            ;

        multiplicative_expr
            = (
                   negate_expr [ _a = _1 ]
               >>  (
                        lit('*') [_a = ValueRef::TIMES]
                    |   lit('/') [_a = ValueRef::DIVIDES]
                   )
               >   multiplicative_expr [ _b = _1 ]
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
            %=   negate_expr
            ;
    }

    template <typename T>
    void initialize_variable_parser(
        typename variable_rule<T>::type& variable,
        name_token_rule& final_token,
        const parse::lexer& tok
    )
    {
        const name_token_rule& variable_container = ::variable_container(tok);

        variable
            = (
                   (
                        (
                            tok.source [ push_back(_a, _1) ]
                         |  tok.target [ push_back(_a, _1) ]
                         |  tok.local_candidate [ push_back(_a, _1) ]
                         |  tok.root_candidate [ push_back(_a, _1) ]
                        )
                    >>  '.'
                    >> -(variable_container [ push_back(_a, _1) ] > '.')
                    >   final_token [ push_back(_a, _1) ]
                   )
               |   (
                       tok.current_turn [ push_back(_a, _1) ] // TODO: Doesn't belong here for any but int, double, and string, right?
                    |  tok.value [ push_back(_a, _1) ]
                   )
              )
              [ _val = new_<ValueRef::Variable<T> >(_a) ]
            ;
    }

#if HAVE_CONDITION_PARSER
    template <typename T>
    void initialize_numeric_statistic_parser(
        typename statistic_rule<T>::type& statistic,
        name_token_rule& final_token,
        const parse::lexer& tok
    )
    {
        const name_token_rule& variable_container = ::variable_container(tok);

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
                    >> -(variable_container [ push_back(_a, _1) ] > '.')
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
        name_token_rule& final_token,
        const parse::lexer& tok
    )
    {
        const name_token_rule& variable_container = ::variable_container(tok);

        statistic
            = (
                   tok.statistic_type_enum [ _b = _1 ] // TODO: Should be "mode" only.
               >>  tok.property
               >> -(variable_container [ push_back(_a, _1) ] > '.')
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
                    =    double_ [ _val = new_<ValueRef::Constant<int> >(static_cast_<int>(_1)) ]
                    |    int_ [ _val = new_<ValueRef::Constant<int> >(_1) ]
                    ;

                initialize_variable_parser<int>(variable, final_token, tok);

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
            }

        typedef typename parse::value_ref_parser_rule<int>::type rule;
        typedef typename variable_rule<int>::type variable_rule;
#if HAVE_CONDITION_PARSER
        typedef typename statistic_rule<int>::type statistic_rule;
#endif
        typedef typename binary_op_expr_rule<int>::type binary_expression_rule;

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
                    =    double_ [ _val = new_<ValueRef::Constant<double> >(_1) ]
                    |    int_ [ _val = new_<ValueRef::Constant<double> >(static_cast_<double>(_1)) ]
                    ;

                initialize_variable_parser<double>(variable, final_token, tok);

#if HAVE_CONDITION_PARSER
                initialize_numeric_statistic_parser<double>(statistic, final_token, tok);
#endif

                initialize_expression_parsers<double>(negate_expr,
                                                      multiplicative_expr,
                                                      additive_expr,
                                                      expr,
                                                      primary_expr);

                int_variable
                    =    get_int_parser_rules(tok).variable [ _val = new_<ValueRef::StaticCast<int, double> >(_1) ]
                    ;

                primary_expr
                    %=   '(' > expr > ')'
                    |    constant
                    |    variable
                    |    int_variable
#if HAVE_CONDITION_PARSER
                    |    statistic
#endif
                    ;
            }

        typedef typename parse::value_ref_parser_rule<double>::type rule;
        typedef typename variable_rule<double>::type variable_rule;
#if HAVE_CONDITION_PARSER
        typedef typename statistic_rule<double>::type statistic_rule;
#endif
        typedef typename binary_op_expr_rule<double>::type binary_expression_rule;

        name_token_rule final_token;
        rule constant;
        variable_rule variable;
        variable_rule int_variable;
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
                final_token
                    %=   tok.name
                    |    tok.species
                    |    tok.building_type
                    |    tok.focus
                    ;

                constant
                    =    tok.string [ _val = new_<ValueRef::Constant<std::string> >(_1) ]
#if 0
                    |    as_string [
                          |   tok.planet_size_enum
                          |   tok.planet_type_enum
                          |   tok.planet_environment_enum
                          |   tok.universe_object_type_enum
                          |   tok.star_type_enum
                          |   double_
                          |   int_
                         ]
#endif
                    ;

                initialize_variable_parser<std::string>(variable, final_token, tok);

#if HAVE_CONDITION_PARSER
                initialize_nonnumeric_statistic_parser<std::string>(statistic, final_token, tok);
#endif

                int_variable
                    =    get_int_parser_rules(tok).variable [ _val = new_<ValueRef::StringCast<int> >(_1) ]
                    ;

                double_variable
                    =    get_double_parser_rules(tok).variable [ _val = new_<ValueRef::StringCast<double> >(_1) ]
                    ;

                primary_expr
                    %=   '(' > expr > ')'
                    |    constant
                    |    variable
                    |    int_variable
                    |    double_variable
#if HAVE_CONDITION_PARSER
                    |    statistic
#endif
                    ;
            }

        typedef typename parse::value_ref_parser_rule<std::string>::type rule;
        typedef typename variable_rule<std::string>::type variable_rule;
#if HAVE_CONDITION_PARSER
        typedef typename statistic_rule<std::string>::type statistic_rule;
#endif
        typedef typename binary_op_expr_rule<std::string>::type binary_expression_rule;

        name_token_rule final_token;
        rule constant;
        variable_rule variable;
        variable_rule int_variable;
        variable_rule double_variable;
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
                final_token
                    %=   tok.planet_size
                    ;

                constant
                    =    tok.planet_size_enum [ _val = new_<ValueRef::Constant<PlanetSize> >(_1) ]
                    |    int_ [ _val = new_<ValueRef::Constant<PlanetSize> >(static_cast_<PlanetSize>(_1)) ]
                    ;

                initialize_variable_parser<PlanetSize>(variable, final_token, tok);

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

        typedef typename parse::value_ref_parser_rule<PlanetSize>::type rule;
        typedef typename variable_rule<PlanetSize>::type variable_rule;
#if HAVE_CONDITION_PARSER
        typedef typename statistic_rule<PlanetSize>::type statistic_rule;
#endif
        typedef typename binary_op_expr_rule<PlanetSize>::type binary_expression_rule;

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
                final_token
                    %=   tok.planet_type
                    |    tok.next_better_planet_type
                    ;

                constant
                    =    tok.planet_type_enum [ _val = new_<ValueRef::Constant<PlanetType> >(_1) ]
                    |    int_ [ _val = new_<ValueRef::Constant<PlanetType> >(static_cast_<PlanetType>(_1)) ]
                    ;

                initialize_variable_parser<PlanetType>(variable, final_token, tok);

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

        typedef typename parse::value_ref_parser_rule<PlanetType>::type rule;
        typedef typename variable_rule<PlanetType>::type variable_rule;
#if HAVE_CONDITION_PARSER
        typedef typename statistic_rule<PlanetType>::type statistic_rule;
#endif
        typedef typename binary_op_expr_rule<PlanetType>::type binary_expression_rule;

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
                final_token
                    %=   tok.planet_environment
                    ;

                constant
                    =    tok.planet_environment_enum [ _val = new_<ValueRef::Constant<PlanetEnvironment> >(_1) ]
                    |    int_ [ _val = new_<ValueRef::Constant<PlanetEnvironment> >(static_cast_<PlanetEnvironment>(_1)) ]
                    ;

                initialize_variable_parser<PlanetEnvironment>(variable, final_token, tok);

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

        typedef typename parse::value_ref_parser_rule<PlanetEnvironment>::type rule;
        typedef typename variable_rule<PlanetEnvironment>::type variable_rule;
#if HAVE_CONDITION_PARSER
        typedef typename statistic_rule<PlanetEnvironment>::type statistic_rule;
#endif
        typedef typename binary_op_expr_rule<PlanetEnvironment>::type binary_expression_rule;

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
                final_token
                    %=   tok.object_type
                    ;

                constant
                    =    tok.universe_object_type_enum [ _val = new_<ValueRef::Constant<UniverseObjectType> >(_1) ]
                    |    int_ [ _val = new_<ValueRef::Constant<UniverseObjectType> >(static_cast_<UniverseObjectType>(_1)) ]
                    ;

                initialize_variable_parser<UniverseObjectType>(variable, final_token, tok);

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

        typedef typename parse::value_ref_parser_rule<UniverseObjectType>::type rule;
        typedef typename variable_rule<UniverseObjectType>::type variable_rule;
#if HAVE_CONDITION_PARSER
        typedef typename statistic_rule<UniverseObjectType>::type statistic_rule;
#endif
        typedef typename binary_op_expr_rule<UniverseObjectType>::type binary_expression_rule;

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
                final_token
                    %=   tok.star_type
                    ;

                constant
                    =    tok.star_type_enum [ _val = new_<ValueRef::Constant<StarType> >(_1) ]
                    |    int_ [ _val = new_<ValueRef::Constant<StarType> >(static_cast_<StarType>(_1)) ]
                    ;

                initialize_variable_parser<StarType>(variable, final_token, tok);

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

        typedef typename parse::value_ref_parser_rule<StarType>::type rule;
        typedef typename variable_rule<StarType>::type variable_rule;
#if HAVE_CONDITION_PARSER
        typedef typename statistic_rule<StarType>::type statistic_rule;
#endif
        typedef typename binary_op_expr_rule<StarType>::type binary_expression_rule;

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
    { return get_int_parser_rules(tok).primary_expr; }

    template <>
    const value_ref_parser_rule<double>::type& value_ref_parser<double>(const parse::lexer& tok)
    { return get_double_parser_rules(tok).primary_expr; }

    template <>
    const value_ref_parser_rule<std::string>::type& value_ref_parser<std::string>(const parse::lexer& tok)
    {
        static const string_parser_rules retval(tok);
        return retval.primary_expr;
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
