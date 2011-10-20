#include "ValueRefParserImpl.h"

#include <GG/ReportParseError.h>


namespace {

    struct double_parser_rules
    {
        double_parser_rules()
            {
                qi::_1_type _1;
                qi::_a_type _a;
                qi::_val_type _val;
                using phoenix::new_;
                using phoenix::push_back;
                using phoenix::static_cast_;

                const parse::lexer& tok = parse::lexer::instance();

                final_token
                    %=   tok.Farming_
                    |    tok.TargetFarming_
                    |    tok.Industry_
                    |    tok.TargetIndustry_
                    |    tok.Research_
                    |    tok.TargetResearch_
                    |    tok.Trade_
                    |    tok.TargetTrade_
                    |    tok.Mining_
                    |    tok.TargetMining_
                    |    tok.Construction_
                    |    tok.TargetConstruction_
                    |    tok.Population_
                    |    tok.TargetPopulation_
                    |    tok.Health_
                    |    tok.TargetHealth_
                    |    tok.MaxFuel_
                    |    tok.Fuel_
                    |    tok.MaxShield_
                    |    tok.Shield_
                    |    tok.MaxDefense_
                    |    tok.Defense_
                    |    tok.MaxTroops_
                    |    tok.Troops_
                    |    tok.MaxStructure_
                    |    tok.Structure_
                    |    tok.Supply_
                    |    tok.Stealth_
                    |    tok.Detection_
                    |    tok.FoodConsumption_
                    |    tok.BattleSpeed_
                    |    tok.StarlaneSpeed_
                    |    tok.TradeStockpile_
                    |    tok.MineralStockpile_
                    |    tok.FoodStockpile_
                    |    tok.DistanceToSource_
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
                            |   int_var_final_token()
                                [ push_back(_a, _1), _val = new_<ValueRef::StaticCast<int, double> >(new_<ValueRef::Variable<int> >(_a)) ]
                           )
                      )
                    | (
                          tok.CurrentTurn_
                       |  tok.Value_
                      )
                      [ push_back(_a, _1), _val = new_<ValueRef::StaticCast<int, double> >(new_<ValueRef::Variable<int> >(_a)) ]
                    ;

                initialize_numeric_statistic_parser<double>(statistic, final_token);

                initialize_expression_parsers<double>(negate_expr,
                                                      multiplicative_expr,
                                                      additive_expr,
                                                      expr,
                                                      primary_expr);

                primary_expr
                    %=   '(' > expr > ')'
                    |    constant
                    |    variable
                    |    statistic
                    ;

                NAME(final_token);
                NAME(constant);
                NAME(variable);
                NAME(statistic);
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
        typedef statistic_rule<double>::type statistic_rule;
        typedef multiplicative_expr_rule<double>::type multiplicative_expression_rule;
        typedef additive_expr_rule<double>::type additive_expression_rule;

        name_token_rule final_token;
        rule constant;
        variable_rule variable;
        statistic_rule statistic;
        rule negate_expr;
        multiplicative_expression_rule multiplicative_expr;
        additive_expression_rule additive_expr;
        rule expr;
        rule primary_expr;
    };

    double_parser_rules& get_double_parser_rules()
    {
        static double_parser_rules retval;
        return retval;
    }

}

const name_token_rule& double_var_final_token()
{ return get_double_parser_rules().final_token; }

namespace parse {

    template <>
    const value_ref_parser_rule<double>::type& value_ref_parser<double>()
    { return get_double_parser_rules().expr; }

}
