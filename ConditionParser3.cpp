#include "ConditionParserImpl.h"

#include "ValueRefParser.h"
#include "../universe/Condition.h"

#include <GG/ReportParseError.h>

#include <boost/spirit/home/phoenix.hpp>


namespace qi = boost::spirit::qi;
namespace phoenix = boost::phoenix;


#define NAME(x) x.name(#x)
//; debug(x)

namespace {

    struct condition_parser_rules_3
    {
        condition_parser_rules_3(const parse::lexer& tok)
            {
                const parse::value_ref_parser_rule<int>::type& int_value_ref =
                    parse::value_ref_parser<int>(tok);

                const parse::value_ref_parser_rule<double>::type& double_value_ref =
                    parse::value_ref_parser<double>(tok);

                const parse::value_ref_parser_rule< ::StarType>::type& star_type_value_ref =
                    parse::value_ref_parser< ::StarType>(tok);

                using qi::_1;
                using qi::_a;
                using qi::_b;
                using qi::_c;
                using qi::_d;
                using qi::_val;
                using phoenix::new_;
                using phoenix::push_back;

                within_distance
                    =    tok.WithinDistance_
                    >    tok.Distance_ > '='
                    >    double_value_ref [ _a = _1 ]
                    >    tok.Condition_ > '='
                    >    parse::condition_parser(tok) [ _val = new_<Condition::WithinDistance>(_a, _1) ]
                    ;

                within_starlane_jumps
                    =    tok.WithinStarlaneJumps_
                    >    tok.Jumps_ > '='
                    >    int_value_ref [ _a = _1 ]
                    >    tok.Condition_ > '='
                    >    parse::condition_parser(tok) [ _val = new_<Condition::WithinStarlaneJumps>(_a, _1) ]
                    ;

                number
                    =    tok.Number_
                    >    tok.Low_ > '='
                    >    int_value_ref [ _a = _1 ]
                    >    tok.High_ > '='
                    >    int_value_ref [ _b = _1 ]
                    >    tok.Condition_ > '='
                    >    parse::condition_parser(tok) [ _val = new_<Condition::Number>(_a, _b, _1) ]
                    ;

                turn
                    =    tok.Turn_
                    >    tok.Low_ > '='
                    >    int_value_ref [ _a = _1 ]
                    >    tok.High_ > '='
                    >    int_value_ref [ _val = new_<Condition::Turn>(_a, _1) ]
                    ;

                created_on_turn
                    =    tok.CreatedOnTurn_
                    >    tok.Low_ > '='
                    >    int_value_ref [ _a = _1 ]
                    >    tok.High_ > '='
                    >    int_value_ref [ _val = new_<Condition::Turn>(_a, _1) ]
                    ;

                number_of
                    =    (
                              tok.NumberOf_
                          >   tok.Number_ > '='
                          >   int_value_ref [ _a = _1 ]
                          >   tok.Condition_ > '='
                          >   parse::condition_parser(tok) [ _val = new_<Condition::SortedNumberOf>(_a, _1) ]
                         )
                    |    (
                              (
                                   tok.MaximumNumberOf_ [ _b = Condition::SORT_MAX ]
                               |   tok.MinimumNumberOf_ [ _b = Condition::SORT_MIN ]
                               |   tok.ModeNumberOf_ [ _b = Condition::SORT_MODE ]
                              )
                          >   tok.Number_ > '='
                          >   int_value_ref [ _a = _1 ]
                          >   tok.SortKey_ > '='
                          >   double_value_ref [ _c = _1 ]
                          >   tok.Condition_ > '='
                          >   parse::condition_parser(tok) [ _val = new_<Condition::SortedNumberOf>(_a, _c, _b, _1) ]
                         )
                    ;

                contains
                    =    tok.Contains_
                    >    tok.Condition_ > '='
                    >    parse::condition_parser(tok) [ _val = new_<Condition::Contains>(_1) ]
                    ;

                contained_by
                    =    tok.ContainedBy_
                    >    tok.Condition_ > '='
                    >    parse::condition_parser(tok) [ _val = new_<Condition::ContainedBy>(_1) ]
                    ;

                star_type
                    =    tok.Star_
                    >    tok.Type_ > '='
                    >>   (
                              ('[' > +star_type_value_ref [ push_back(_a, _1) ] > ']')
                          |   star_type_value_ref [ push_back(_a, _1) ]
                         )
                         [ _val = new_<Condition::StarType>(_a) ]
                    ;

                random
                    =    tok.Random_
                    >    tok.Probability_ > '='
                    >    double_value_ref [ _val = new_<Condition::Chance>(_1) ]
                    ;

                owner_stockpile
                    =    (
                              tok.OwnerFoodStockpile_ [ _a = RE_FOOD ]
                          |   tok.OwnerMineralStockpile_ [ _a = RE_MINERALS ]
                          |   tok.OwnerTradeStockpile_ [ _a = RE_TRADE ]
                         )
                    >    tok.Low_ > '='
                    >    double_value_ref [ _b = _1 ]
                    >    tok.High_ > '='
                    >    double_value_ref [ _val = new_<Condition::EmpireStockpileValue>(_a, _b, _1) ]
                    ;

                resource_supply_connected
                    =    tok.ResourceSupplyConnected_
                    >    tok.Empire_ > '='
                    >    int_value_ref [ _a = _1 ]
                    >    tok.Condition_ > '='
                    >    parse::condition_parser(tok) [ _val = new_<Condition::ResourceSupplyConnectedByEmpire>(_a, _1) ]
                    ;

                start
                    %=   within_distance
                    |    within_starlane_jumps
                    |    number
                    |    turn
                    |    created_on_turn
                    |    number_of
                    |    contains
                    |    contained_by
                    |    star_type
                    |    random
                    |    owner_stockpile
                    |    resource_supply_connected
                    ;
            }

        typedef boost::spirit::qi::rule<
            parse::token_iterator,
            Condition::ConditionBase* (),
            qi::locals<ValueRef::ValueRefBase<int>*>,
            parse::skipper_type
        > int_ref_rule;

        typedef boost::spirit::qi::rule<
            parse::token_iterator,
            Condition::ConditionBase* (),
            qi::locals<ValueRef::ValueRefBase<double>*>,
            parse::skipper_type
        > double_ref_rule;

        typedef boost::spirit::qi::rule<
            parse::token_iterator,
            Condition::ConditionBase* (),
            qi::locals<
                ValueRef::ValueRefBase<int>*,
                ValueRef::ValueRefBase<int>*
            >,
            parse::skipper_type
        > int_ref_int_ref_rule;

        typedef boost::spirit::qi::rule<
            parse::token_iterator,
            Condition::ConditionBase* (),
            qi::locals<
                ValueRef::ValueRefBase<int>*,
                Condition::SortingMethod,
                ValueRef::ValueRefBase<double>*
            >,
            parse::skipper_type
        > int_ref_sorting_method_double_ref_rule;

        typedef boost::spirit::qi::rule<
            parse::token_iterator,
            Condition::ConditionBase* (),
            qi::locals<
                ResourceType,
                ValueRef::ValueRefBase<double>*
            >,
            parse::skipper_type
        > resource_type_double_ref_rule;

        typedef boost::spirit::qi::rule<
            parse::token_iterator,
            Condition::ConditionBase* (),
            qi::locals<std::vector<const ValueRef::ValueRefBase<StarType>*> >,
            parse::skipper_type
        > star_type_vec_rule;

        double_ref_rule within_distance;
        int_ref_rule within_starlane_jumps;
        int_ref_int_ref_rule number;
        int_ref_rule turn;
        int_ref_rule created_on_turn;
        int_ref_sorting_method_double_ref_rule number_of;
        parse::condition_parser_rule contains;
        parse::condition_parser_rule contained_by;
        star_type_vec_rule star_type;
        parse::condition_parser_rule random;
        resource_type_double_ref_rule owner_stockpile;
        int_ref_rule resource_supply_connected;
        parse::condition_parser_rule start;
    };

}

namespace parse { namespace detail {

    const condition_parser_rule& condition_parser_3(const lexer& tok)
    {
        static condition_parser_rules_3 retval(tok);
        return retval.start;
    }

} }
