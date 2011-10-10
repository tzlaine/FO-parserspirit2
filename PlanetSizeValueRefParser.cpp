#include "ValueRefParserImpl.h"

#include "EnumParser.h"

#include <GG/ReportParseError.h>


namespace {

    struct planet_size_parser_rules
    {
        planet_size_parser_rules()
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
                    %=   tok.PlanetSize_
                    ;

                constant
                    =    parse::enum_parser<PlanetSize>() [ _val = new_<ValueRef::Constant<PlanetSize> >(_1) ]
                    |    tok.int_ [ _val = new_<ValueRef::Constant<PlanetSize> >(static_cast_<PlanetSize>(_1)) ]
                    ;

                variable
                    =    first_token [ push_back(_a, _1) ] > '.'
                    >   -(container_token [ push_back(_a, _1) ] > '.')
                    >    final_token [ push_back(_a, _1), _val = new_<ValueRef::Variable<PlanetSize> >(_a) ]
                    ;

#if HAVE_CONDITION_PARSER
                initialize_nonnumeric_statistic_parser<PlanetSize>(statistic, final_token);
#endif

                primary_expr
                    %=   constant
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
                NAME(primary_expr);

#if 0  // TODO: Fix this!
                qi::on_error<qi::fail>(primary_expr, parse::report_error(_1, _2, _3, _4));
#endif
            }

        typedef parse::value_ref_parser_rule<PlanetSize>::type rule;
        typedef variable_rule<PlanetSize>::type variable_rule;
#if HAVE_CONDITION_PARSER
        typedef statistic_rule<PlanetSize>::type statistic_rule;
#endif
        typedef binary_op_expr_rule<PlanetSize>::type binary_expression_rule;

        name_token_rule final_token;
        rule constant;
        variable_rule variable;
#if HAVE_CONDITION_PARSER
        statistic_rule statistic;
#endif
        rule primary_expr;
    };

}


namespace parse {

    template <>
    const value_ref_parser_rule<PlanetSize>::type& value_ref_parser<PlanetSize>()
    {
        static const planet_size_parser_rules retval;
        return retval.primary_expr;
    }

}
