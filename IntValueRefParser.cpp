#include "ValueRefParserImpl.h"

#include <GG/ReportParseError.h>


namespace {

    struct int_parser_rules
    {
        int_parser_rules()
            {
                qi::_1_type _1;
                qi::_2_type _2;
                qi::_3_type _3;
                qi::_4_type _4;
                qi::_a_type _a;
                qi::_val_type _val;
                qi::lit_type lit;
                using phoenix::new_;
                using phoenix::push_back;
                using phoenix::static_cast_;

                const parse::lexer& tok = parse::lexer::instance();

                first_token
                    %=   tok.Source_
                    |    tok.Target_
                    |    tok.LocalCandidate_
                    |    tok.RootCandidate_
                    ;

                container_token
                    %=   tok.Planet_
                    |    tok.System_
                    |    tok.Fleet_
                    ;

                // TODO: Should we apply elements of this list only to certain
                // containers?  For example, if one writes "Source.Planet.",
                // "NumShips" should not follow.
                final_token
                    %=   tok.Owner_
                    |    tok.ID_
                    |    tok.CreationTurn_
                    |    tok.Age_
                    |    tok.ProducedByEmpireID_
                    |    tok.DesignID_
                    |    tok.FleetID_
                    |    tok.PlanetID_
                    |    tok.SystemID_
                    |    tok.FinalDestinationID_
                    |    tok.NextSystemID_
                    |    tok.PreviousSystemID_
                    |    tok.NumShips_
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
                               tok.CurrentTurn_
                            |  tok.Value_
                           )
                           [ push_back(_a, _1) ]
                      )
                      [ _val = new_<ValueRef::Variable<int> >(_a) ]
                    ;

#if HAVE_CONDITION_PARSER
                initialize_numeric_statistic_parser<int>(statistic, final_token);
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

    int_parser_rules& get_int_parser_rules()
    {
        static int_parser_rules retval;
        return retval;
    }

}

const name_token_rule& int_var_first_token()
{ return get_int_parser_rules().first_token; }

const name_token_rule& int_var_container_token()
{ return get_int_parser_rules().container_token; }

const name_token_rule& int_var_final_token()
{ return get_int_parser_rules().final_token; }

namespace parse {

    template <>
    const value_ref_parser_rule<int>::type& value_ref_parser<int>()
    { return get_int_parser_rules().expr; }

}
