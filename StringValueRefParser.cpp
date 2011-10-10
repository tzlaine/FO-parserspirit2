#include "ValueRefParserImpl.h"

#include "EnumParser.h"

#include <GG/ReportParseError.h>


namespace {

    struct string_parser_rules
    {
        string_parser_rules()
            {
                qi::_1_type _1;
                qi::_a_type _a;
                qi::_val_type _val;
                qi::as_string_type as_string;
                using phoenix::push_back;
                using phoenix::new_;

                const parse::lexer& tok = parse::lexer::instance();

                const name_token_rule& first_token = int_var_first_token();
                const name_token_rule& container_token = int_var_container_token();

                final_token
                    %=   tok.Name_
                    |    tok.Species_
                    |    tok.BuildingType_
                    |    tok.Focus_
                    ;

                constant
                    =    tok.string [ _val = new_<ValueRef::Constant<std::string> >(_1) ]
                    |    as_string [
                              parse::enum_parser<PlanetSize>()
                          |   parse::enum_parser<PlanetType>()
                          |   parse::enum_parser<PlanetEnvironment>()
                          |   parse::enum_parser<UniverseObjectType>()
                          |   parse::enum_parser<StarType>()
                          |   tok.double_
                          |   tok.int_
                         ]
                    ;

                variable
                    = (
                           first_token [ push_back(_a, _1) ] > '.'
                       >  -(container_token [ push_back(_a, _1) ] > '.')
                       >   (
                                final_token
                                [ push_back(_a, _1), _val = new_<ValueRef::Variable<std::string> >(_a) ]
                            |   int_var_final_token()
                                [ push_back(_a, _1), _val = new_<ValueRef::StringCast<int> >(new_<ValueRef::Variable<int> >(_a)) ]
                            |   double_var_final_token()
                                [ push_back(_a, _1), _val = new_<ValueRef::StringCast<double> >(new_<ValueRef::Variable<double> >(_a)) ]
                           )
                      )
                    | (
                          tok.CurrentTurn_
                       |  tok.Value_
                      )
                      [ push_back(_a, _1), _val = new_<ValueRef::StringCast<int> >(new_<ValueRef::Variable<int> >(_a)) ]
                    ;

#if HAVE_CONDITION_PARSER
                initialize_nonnumeric_statistic_parser<std::string>(statistic, final_token);
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
    const value_ref_parser_rule<std::string>::type& value_ref_parser<std::string>()
    {
        static const string_parser_rules retval;
        return retval.expr;
    }

}
