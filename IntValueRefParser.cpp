#include "ValueRefParserImpl.h"

#include <GG/ReportParseError.h>


namespace {

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

}

const name_token_rule& int_var_first_token(const parse::lexer& tok)
{ return get_int_parser_rules(tok).first_token; }

const name_token_rule& int_var_container_token(const parse::lexer& tok)
{ return get_int_parser_rules(tok).container_token; }

const name_token_rule& int_var_final_token(const parse::lexer& tok)
{ return get_int_parser_rules(tok).final_token; }

namespace parse {

    template <>
    const value_ref_parser_rule<int>::type& value_ref_parser<int>(const parse::lexer& tok)
    { return get_int_parser_rules(tok).expr; }

}
