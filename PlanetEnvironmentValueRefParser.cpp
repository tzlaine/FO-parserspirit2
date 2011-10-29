#include "ValueRefParserImpl.h"

#include "EnumParser.h"

#include <GG/ReportParseError.h>


namespace {

    struct planet_environment_parser_rules
    {
        planet_environment_parser_rules()
            {
                qi::_1_type _1;
                qi::_a_type _a;
                qi::_val_type _val;
                using phoenix::new_;
                using phoenix::push_back;
                using phoenix::static_cast_;

                const parse::lexer& tok = parse::lexer::instance();

                final_token
                    %=   tok.PlanetEnvironment_
                    ;

                constant
                    =    parse::enum_parser<PlanetEnvironment>() [ _val = new_<ValueRef::Constant<PlanetEnvironment> >(_1) ]
                    |    tok.int_ [ _val = new_<ValueRef::Constant<PlanetEnvironment> >(static_cast_<PlanetEnvironment>(_1)) ]
                    ;

                variable
                    =    first_token [ push_back(_a, _1) ] > '.'
                    >   -(container_token [ push_back(_a, _1) ] > '.')
                    >    final_token [ push_back(_a, _1), _val = new_<ValueRef::Variable<PlanetEnvironment> >(_a) ]
                    ;

                initialize_nonnumeric_statistic_parser<PlanetEnvironment>(statistic, final_token);

                primary_expr
                    %=   constant
                    |    variable
                    |    statistic
                    ;

                final_token.name("PlanetEnvironment variable name (e.g., PlanetEnvironment)");
                constant.name("PlanetEnvironment");
                variable.name("PlanetEnvironment variable");
                statistic.name("PlanetEnvironment statistic");
                primary_expr.name("PlanetEnvironment expression");

                DEBUG_RULE(final_token);
                DEBUG_RULE(constant);
                DEBUG_RULE(variable);
                DEBUG_RULE(statistic);
                DEBUG_RULE(primary_expr);
            }

        typedef parse::value_ref_parser_rule<PlanetEnvironment>::type rule;
        typedef variable_rule<PlanetEnvironment>::type variable_rule;
        typedef statistic_rule<PlanetEnvironment>::type statistic_rule;

        name_token_rule final_token;
        rule constant;
        variable_rule variable;
        statistic_rule statistic;
        rule primary_expr;
    };

}


namespace parse {

    template <>
    value_ref_parser_rule<PlanetEnvironment>::type& value_ref_parser<PlanetEnvironment>()
    {
        static planet_environment_parser_rules retval;
        return retval.primary_expr;
    }

}
