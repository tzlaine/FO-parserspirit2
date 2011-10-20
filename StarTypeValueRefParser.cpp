#include "ValueRefParserImpl.h"

#include "EnumParser.h"

#include <GG/ReportParseError.h>


namespace {

    struct star_type_parser_rules
    {
        star_type_parser_rules()
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
                    %=   tok.StarType_
                    ;

                constant
                    =    parse::enum_parser<StarType>() [ _val = new_<ValueRef::Constant<StarType> >(_1) ]
                    |    tok.int_ [ _val = new_<ValueRef::Constant<StarType> >(static_cast_<StarType>(_1)) ]
                    ;

                variable
                    =    first_token [ push_back(_a, _1) ] > '.'
                    >   -(container_token [ push_back(_a, _1) ] > '.')
                    >    final_token [ push_back(_a, _1), _val = new_<ValueRef::Variable<StarType> >(_a) ]
                    ;

#if HAVE_CONDITION_PARSER
                initialize_nonnumeric_statistic_parser<StarType>(statistic, final_token);
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

        typedef parse::value_ref_parser_rule<StarType>::type rule;
        typedef variable_rule<StarType>::type variable_rule;
#if HAVE_CONDITION_PARSER
        typedef statistic_rule<StarType>::type statistic_rule;
#endif

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
    const value_ref_parser_rule<StarType>::type& value_ref_parser<StarType>()
    {
        static const star_type_parser_rules retval;
        return retval.primary_expr;
    }

}
