#include "ValueRefParserImpl.h"

#include <GG/ReportParseError.h>


namespace {

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

                const name_token_rule& first_token = int_var_first_token(tok);
                const name_token_rule& container_token = int_var_container_token(tok);

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
                            |   int_var_final_token(tok)
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

                const name_token_rule& first_token = int_var_first_token(tok);
                const name_token_rule& container_token = int_var_container_token(tok);

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
                            |   int_var_final_token(tok)
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

                const name_token_rule& first_token = int_var_first_token(tok);
                const name_token_rule& container_token = int_var_container_token(tok);

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

                const name_token_rule& first_token = int_var_first_token(tok);
                const name_token_rule& container_token = int_var_container_token(tok);

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

                const name_token_rule& first_token = int_var_first_token(tok);
                const name_token_rule& container_token = int_var_container_token(tok);

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

                const name_token_rule& first_token = int_var_first_token(tok);
                const name_token_rule& container_token = int_var_container_token(tok);

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

                const name_token_rule& first_token = int_var_first_token(tok);
                const name_token_rule& container_token = int_var_container_token(tok);

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
