// -*- C++ -*-
#include "ValueRefParser.h"

#include "../universe/ValueRef.h"

#include <boost/spirit/home/phoenix.hpp>


#define HAVE_CONDITION_PARSER 0

namespace qi = boost::spirit::qi;
namespace phoenix = boost::phoenix;


#define DEBUG_VALUEREF_PARSE 0
#if DEBUG_VALUEREF_PARSE
#define NAME(x) x.name(#x); debug(x)
#else
#define NAME(x) x.name(#x)
#endif

// This is just here to satisfy the requirements of qi::debug(<rule>).
namespace adobe {
    inline std::ostream& operator<<(std::ostream& os, const std::vector<name_t>& name_vec)
    {
        os << "[ ";
        for (std::size_t i = 0; i < name_vec.size(); ++i) {
            os << name_vec[i] << " ";
        }
        os << "]";
        return os;
    }
}

typedef qi::rule<
    parse::token_iterator,
    adobe::name_t (),
    parse::skipper_type
> name_token_rule;

template <typename T>
struct variable_rule
{
    typedef qi::rule<
        parse::token_iterator,
        ValueRef::Variable<T>* (),
        qi::locals<std::vector<adobe::name_t> >,
        parse::skipper_type
    > type;
};

#if HAVE_CONDITION_PARSER
template <typename T>
struct statistic_rule
{
    typedef qi::rule<
        parse::token_iterator,
        ValueRef::ValueRefBase<T>* (),
        qi::locals<
            std::vector<adobe::name_t>,
            ValueRef::StatisticType,
            Condition::ConditionBase* // TODO: Change spelling to Condition::Base in the universe code.
        >,
        parse::skipper_type
    > type;
};
#endif

template <typename T>
struct binary_op_expr_rule
{
    typedef qi::rule<
        parse::token_iterator,
        ValueRef::ValueRefBase<T>* (),
        qi::locals<
            ValueRef::OpType,
            ValueRef::ValueRefBase<T>*,
            ValueRef::ValueRefBase<T>*
        >,
        parse::skipper_type
    > type;
};

template <typename T>
void initialize_expression_parsers(
    typename parse::value_ref_parser_rule<T>::type& negate_expr,
    typename binary_op_expr_rule<T>::type& multiplicative_expr,
    typename binary_op_expr_rule<T>::type& additive_expr,
    typename parse::value_ref_parser_rule<T>::type& expr,
    typename parse::value_ref_parser_rule<T>::type& primary_expr
)
{
    using qi::_1;
    using qi::_a;
    using qi::_b;
    using qi::_c;
    using qi::_val;
    using qi::lit;
    using phoenix::new_;

    negate_expr
        =    '-' > primary_expr [ _val = new_<ValueRef::Operation<T> >(ValueRef::NEGATE, _1) ]
        |    primary_expr [ _val = _1 ]
        ;

    multiplicative_expr
        = (
               negate_expr [ _b = _1 ]
           >>  (
                    lit('*') [_a = ValueRef::TIMES]
                |   lit('/') [_a = ValueRef::DIVIDES]
               )
           >   multiplicative_expr [ _c = _1 ]
          )
          [ _val = new_<ValueRef::Operation<T> >(_a, _b, _c) ]
        | negate_expr [ _val = _1 ]
        ;

    additive_expr
        = (
               multiplicative_expr [ _b = _1 ]
           >>  (
                    lit('+') [_a = ValueRef::PLUS]
                |   lit('-') [_a = ValueRef::MINUS]
               )
           >   additive_expr [ _c = _1 ]
          )
          [ _val = new_<ValueRef::Operation<T> >(_a, _b, _c) ]
        | multiplicative_expr [ _val = _1 ]
        ;

    expr
        %=   additive_expr
        ;
}

#if HAVE_CONDITION_PARSER
template <typename T>
void initialize_numeric_statistic_parser(
    typename statistic_rule<T>::type& statistic,
    const name_token_rule& final_token,
    const parse::lexer& tok
)
{
    statistic
        = (
               (
                    tok.number [ _b = ValueRef::COUNT ]
                >>  tok.condition
                >   condition/*TODO!*/ [ _c = _1 ]
               )
           |   (
                    tok.statistic_type_enum [ _b = _1 ]
                >>  tok.property
                >> -((tok.planet | tok.system | tok.fleet) [ push_back(_a, _1) ] > '.')
                >   final_token [ push_back(_a, _1) ]
                >   tok.condition
                >   condition/*TODO!*/ [ _c = _1 ]
               )
          )
          [ _val = new_<ValueRef::Statistic<T> >(_a, _b, _c) ]
        ;
}

template <typename T>
void initialize_nonnumeric_statistic_parser(
    typename statistic_rule<T>::type& statistic,
    const name_token_rule& final_token,
    const parse::lexer& tok
)
{
    statistic
        = (
               tok.statistic_type_enum [ _b = _1 ] // TODO: Should be "mode" only.
           >>  tok.property
           >> -((tok.planet | tok.system | tok.fleet) [ push_back(_a, _1) ] > '.')
           >   final_token [ push_back(_a, _1) ]
           >   tok.condition
           >   condition/*TODO!*/ [ _c = _1 ]
          )
          [ _val = new_<ValueRef::Statistic<T> >(_a, _b, _c) ]
        ;
}
#endif

const name_token_rule& int_var_first_token(const parse::lexer& tok);
const name_token_rule& int_var_container_token(const parse::lexer& tok);
const name_token_rule& int_var_final_token(const parse::lexer& tok);
const name_token_rule& double_var_final_token(const parse::lexer& tok);
