#include "ValueRefParserImpl.h"

#include <GG/ReportParseError.h>


namespace {

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
