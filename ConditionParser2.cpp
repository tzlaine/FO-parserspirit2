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

    struct condition_parser_rules_2
    {
        condition_parser_rules_2()
            {
                const parse::lexer& tok = parse::lexer::instance();

                const parse::value_ref_parser_rule<int>::type& int_value_ref =
                    parse::value_ref_parser<int>();

                qi::_1_type _1;
                qi::_a_type _a;
                qi::_b_type _b;
                qi::_val_type _val;
                using phoenix::new_;

                has_special
                    =    tok.HasSpecial_
                    >    tok.Name_ > '='
                    >    tok.string [ _val = new_<Condition::HasSpecial>(_1) ]
                    ;

                has_special_since_turn
                    =    tok.HasSpecialSinceTurn_
                    >    tok.Name_ > '='
                    >    tok.string [ _a = _1 ]
                    >    tok.Low_ > '='
                    >    int_value_ref [ _b = _1 ]
                    >    tok.High_ > '='
                    >    int_value_ref [ _val = new_<Condition::HasSpecial>(_a, _b, _1) ]
                    ;

                owner_has_tech
                    =    tok.OwnerHasTech_
                    >    tok.Name_ > '='
                    >    tok.string [ _val = new_<Condition::OwnerHasTech>(_1) ]
                    ;

                design_has_hull
                    =    tok.DesignHasHull_
                    >    tok.Name_ > '='
                    >    tok.string [ _val = new_<Condition::DesignHasHull>(_1) ]
                    ;

                design_has_part
                    =    tok.DesignHasPart_
                    >    tok.Low_ > '='
                    >    int_value_ref [ _a = _1 ]
                    >    tok.High_ > '='
                    >    int_value_ref [ _b = _1 ]
                    >    tok.Class_ > '='
                    >    tok.string [ _val = new_<Condition::DesignHasPart>(_a, _b, _1) ]
                    ;

                design_has_part_class
                    =    tok.DesignHasPartClass_
                    >    tok.Low_ > '='
                    >    int_value_ref [ _a = _1 ]
                    >    tok.High_ > '='
                    >    int_value_ref [ _b = _1 ]
                    >    tok.Class_ > '='
                    >    tok.ship_part_class_enum [ _val = new_<Condition::DesignHasPartClass>(_a, _b, _1) ]
                    ;

                predefined_design
                    =    tok.Design_
                    >>   tok.Name_ > '=' // TODO: make this condition Design = "name"
                    >    tok.string [ _val = new_<Condition::PredefinedShipDesign>(_1) ]
                    ;

                design_number
                    =    tok.Design_
                    >>   tok.Design_ > '=' // TODO: make this condition Design = #
                    >    int_value_ref [ _val = new_<Condition::NumberedShipDesign>(_1) ]
                    ;

                produced_by_empire
                    =    tok.ProducedByEmpire_
                    >    tok.Empire_ > '='
                    >    int_value_ref [ _val = new_<Condition::ProducedByEmpire>(_1) ]
                    ;

                visible_to_empire
                    =    tok.VisibleToEmpire_
                    >    tok.Empire_ > '='
                    >    int_value_ref [ _val = new_<Condition::VisibleToEmpire>(_1) ]
                    ;

                explored_by_empire
                    =    tok.ExploredByEmpire_
                    >    tok.Empire_ > '='
                    >    int_value_ref [ _val = new_<Condition::ExploredByEmpire>(_1) ]
                    ;

                fleet_supplyable_by_empire
                    =    tok.FleetSupplyableByEmpire_
                    >    tok.Empire_ > '='
                    >    int_value_ref [ _val = new_<Condition::FleetSupplyableByEmpire>(_1) ]
                    ;

                in_system
                    =    tok.InSystem_
                    >    tok.ID_ > '='
                    >    int_value_ref [ _val = new_<Condition::InSystem>(_1) ]
                    ;

                object_id
                    =    tok.Object_
                    >    tok.ID_ > '='
                    >    int_value_ref [ _val = new_<Condition::ObjectID>(_1) ]
                    ;

                start
                    %=   has_special
                    |    has_special_since_turn
                    |    owner_has_tech
                    |    design_has_hull
                    |    design_has_part
                    |    design_has_part_class
                    |    predefined_design
                    |    design_number
                    |    produced_by_empire
                    |    visible_to_empire
                    |    explored_by_empire
                    |    fleet_supplyable_by_empire
                    |    in_system
                    |    object_id
                    ;

                NAME(has_special);
                NAME(has_special_since_turn);
                NAME(owner_has_tech);
                NAME(design_has_hull);
                NAME(design_has_part);
                NAME(design_has_part_class);
                NAME(predefined_design);
                NAME(design_number);
                NAME(produced_by_empire);
                NAME(visible_to_empire);
                NAME(explored_by_empire);
                NAME(fleet_supplyable_by_empire);
                NAME(in_system);
                NAME(object_id);
            }

        typedef boost::spirit::qi::rule<
            parse::token_iterator,
            Condition::ConditionBase* (),
            qi::locals<
                ValueRef::ValueRefBase<int>*,
                ValueRef::ValueRefBase<int>*
            >,
            parse::skipper_type
        > value_ref_ints_rule;

        typedef boost::spirit::qi::rule<
            parse::token_iterator,
            Condition::ConditionBase* (),
            qi::locals<
                std::string,
                ValueRef::ValueRefBase<int>*
            >,
            parse::skipper_type
        > string_int_ref_rule;

        parse::condition_parser_rule has_special;
        string_int_ref_rule has_special_since_turn;
        parse::condition_parser_rule owner_has_tech;
        parse::condition_parser_rule design_has_hull;
        value_ref_ints_rule design_has_part;
        value_ref_ints_rule design_has_part_class;
        parse::condition_parser_rule predefined_design;
        parse::condition_parser_rule design_number;
        parse::condition_parser_rule produced_by_empire;
        parse::condition_parser_rule visible_to_empire;
        parse::condition_parser_rule explored_by_empire;
        parse::condition_parser_rule fleet_supplyable_by_empire;
        parse::condition_parser_rule in_system;
        parse::condition_parser_rule object_id;
        parse::condition_parser_rule start;
    };

}

namespace parse { namespace detail {

    const condition_parser_rule& condition_parser_2()
    {
        static condition_parser_rules_2 retval;
        return retval.start;
    }

} }
