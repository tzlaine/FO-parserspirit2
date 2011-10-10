#include "ValueRefParserImpl.h"

#include "EnumParser.h"

#include <GG/ReportParseError.h>


namespace {

    struct universe_object_type_parser_rules
    {
        universe_object_type_parser_rules()
            {
                using qi::_1;
                using qi::_a;
                using qi::_val;
                using phoenix::new_;
                using phoenix::push_back;
                using phoenix::static_cast_;

                const parse::lexer& tok = parse::lexer::instance();

                const name_token_rule& first_token = int_var_first_token();
                const name_token_rule& container_token = int_var_container_token();

                final_token
                    %=   tok.ObjectType_
                    ;

                constant
                    =    parse::enum_parser<UniverseObjectType>() [ _val = new_<ValueRef::Constant<UniverseObjectType> >(_1) ]
                    |    tok.int_ [ _val = new_<ValueRef::Constant<UniverseObjectType> >(static_cast_<UniverseObjectType>(_1)) ]
                    ;

                variable
                    =    first_token [ push_back(_a, _1) ] > '.'
                    >   -(container_token [ push_back(_a, _1) ] > '.')
                    >    final_token [ push_back(_a, _1), _val = new_<ValueRef::Variable<UniverseObjectType> >(_a) ]
                    ;


#if HAVE_CONDITION_PARSER
                initialize_nonnumeric_statistic_parser<UniverseObjectType>(statistic, final_token);
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

        typedef parse::value_ref_parser_rule<UniverseObjectType>::type rule;
        typedef variable_rule<UniverseObjectType>::type variable_rule;
#if HAVE_CONDITION_PARSER
        typedef statistic_rule<UniverseObjectType>::type statistic_rule;
#endif
        typedef binary_op_expr_rule<UniverseObjectType>::type binary_expression_rule;

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
    const value_ref_parser_rule<UniverseObjectType>::type& value_ref_parser<UniverseObjectType>()
    {
        static const universe_object_type_parser_rules retval;
        return retval.primary_expr;
    }

}
