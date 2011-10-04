#include "ValueRefParserImpl.h"

#include <GG/ReportParseError.h>


namespace {

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
                            |   double_var_final_token(tok)
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

}


namespace parse {

    template <>
    const value_ref_parser_rule<std::string>::type& value_ref_parser<std::string>(const parse::lexer& tok)
    {
        static const string_parser_rules retval(tok);
        return retval.expr;
    }

}
