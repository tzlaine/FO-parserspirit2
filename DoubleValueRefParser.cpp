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

}

const name_token_rule& double_var_final_token(const parse::lexer& tok)
{ return get_double_parser_rules(tok).final_token; }

namespace parse {

    template <>
    const value_ref_parser_rule<double>::type& value_ref_parser<double>(const parse::lexer& tok)
    { return get_double_parser_rules(tok).expr; }

}
