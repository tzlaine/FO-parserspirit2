#include "ParseImpl.h"


namespace {

    struct rules
    {
        rules()
            {
                const parse::lexer& tok = parse::lexer::instance();

                qi::_1_type _1;
                qi::_2_type _2;
                qi::_3_type _3;
                qi::_4_type _4;
                qi::_a_type _a;
                qi::_b_type _b;
                qi::_r1_type _r1;
                using phoenix::new_;
                using phoenix::push_back;

                fleet_plan
                    =    tok.Fleet_
                    >    tok.Name_ > '='
                    >    tok.string [ _a = _1 ]
                    >    tok.Ships_ > '='
                    >    (
                              '[' > +tok.string [ push_back(_b, _1) ] > ']'
                          |   tok.string [ push_back(_b, _1) ]
                         )
                         [ push_back(_r1, new_<FleetPlan>(_a, _b)) ]
                    ;

                start
                    =   +fleet_plan(_r1)
                    ;
            }

        typedef boost::spirit::qi::rule<
            parse::token_iterator,
            void (std::vector<FleetPlan*>&),
            qi::locals<
                std::string,
                std::vector<std::string>
            >,
            parse::skipper_type
        > fleet_plan_rule;

        typedef boost::spirit::qi::rule<
            parse::token_iterator,
            void (std::vector<FleetPlan*>&),
            parse::skipper_type
        > start_rule;

        fleet_plan_rule fleet_plan;
        start_rule start;
    };

}

namespace parse {

    void fleet_plans(const boost::filesystem::path& path, std::vector<FleetPlan*>& fleet_plans_)
    { detail::parse_file<rules, std::vector<FleetPlan*> >(path, fleet_plans_); }

}
