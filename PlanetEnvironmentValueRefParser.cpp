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

                const name_token_rule& first_token = int_var_first_token();
                const name_token_rule& container_token = int_var_container_token();

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

                NAME(final_token);
                NAME(constant);
                NAME(variable);
                NAME(statistic);
                NAME(primary_expr);

#if 0  // TODO: Fix this!
                qi::on_error<qi::fail>(primary_expr, parse::report_error(_1, _2, _3, _4));
#endif
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
    const value_ref_parser_rule<PlanetEnvironment>::type& value_ref_parser<PlanetEnvironment>()
    {
        static const planet_environment_parser_rules retval;
        return retval.primary_expr;
    }

}
