#include "ValueRefParserImpl.h"

#include <GG/ReportParseError.h>


namespace {

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

}


namespace parse {

    template <>
    const value_ref_parser_rule<PlanetType>::type& value_ref_parser<PlanetType>(const parse::lexer& tok)
    {
        static const planet_type_parser_rules retval(tok);
        return retval.primary_expr;
    }

}
