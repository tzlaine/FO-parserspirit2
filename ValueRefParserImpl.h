// -*- C++ -*-
#include "ValueRefParser.h"

#include "ConditionParserImpl.h"
#include "EnumParser.h"
#include "../universe/ValueRef.h"

#include <boost/spirit/home/phoenix.hpp>


namespace qi = boost::spirit::qi;
namespace phoenix = boost::phoenix;


#define DEBUG_VALUEREF_PARSE 0
#if DEBUG_VALUEREF_PARSE
#define NAME(x) x.name(#x); debug(x)
#else
#define NAME(x) x.name(#x)
#endif

// These are just here to satisfy the requirements of qi::debug(<rule>).
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

namespace boost {
    inline std::ostream& operator<<(std::ostream& os, const std::vector<variant<ValueRef::OpType, ValueRef::ValueRefBase<int>*> >& name_vec)
    { return os << "[int expression stack]"; }

    inline std::ostream& operator<<(std::ostream& os, const std::vector<variant<ValueRef::OpType, ValueRef::ValueRefBase<double>*> >& name_vec)
    { return os << "[double expression stack]"; }
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

template <typename T>
struct operator_or_operand
{ typedef boost::variant<ValueRef::OpType, ValueRef::ValueRefBase<T>*> type; };

template <typename T>
struct operator_or_operand_vector
{ typedef std::vector<typename operator_or_operand<T>::type> type; };

template <typename T>
struct expression_value_type;

template <typename T>
struct expression_value_type<std::vector<boost::variant<ValueRef::OpType, ValueRef::ValueRefBase<T>*> > >
{ typedef T type; };

template <typename T>
struct multiplicative_expr_rule
{
    typedef qi::rule<
        parse::token_iterator,
        void (typename operator_or_operand_vector<T>::type&),
        qi::locals<ValueRef::OpType>,
        parse::skipper_type
    > type;
};

template <typename T>
struct additive_expr_rule
{
    typedef qi::rule<
        parse::token_iterator,
        ValueRef::ValueRefBase<T>* (),
        qi::locals<
            typename operator_or_operand_vector<T>::type,
            ValueRef::OpType
        >,
        parse::skipper_type
    > type;
};

struct make_expression_
{
    template <typename Arg>
    struct result
    { typedef ValueRef::ValueRefBase<typename expression_value_type<Arg>::type>* type; };

    template <typename Arg>
    typename result<Arg>::type operator()(const Arg& arg) const
        {
            typedef typename expression_value_type<Arg>::type value_type;
            std::vector<ValueRef::ValueRefBase<value_type>*> operand_stack;
            operand_stack.reserve(arg.size());
            const typename Arg::const_iterator end_it = arg.end();
            for (typename Arg::const_iterator it = arg.begin(); it != end_it; ++it) {
                if (const ValueRef::OpType* op = boost::get<ValueRef::OpType>(&*it)) { // operator
                    ValueRef::ValueRefBase<value_type>* right = operand_stack.back();
                    operand_stack.pop_back();
                    ValueRef::ValueRefBase<value_type>* left = operand_stack.back();
                    operand_stack.pop_back();
                    // TODO: Constant folding.
                    operand_stack.push_back(new ValueRef::Operation<value_type>(*op, left, right));
                } else {
                    operand_stack.push_back(boost::get<ValueRef::ValueRefBase<value_type>*>(*it));
                }
            }
            return operand_stack[0];
        }
};
const boost::phoenix::function<make_expression_> make_expression;

template <typename T>
void initialize_expression_parsers(
    typename parse::value_ref_parser_rule<T>::type& negate_expr,
    typename multiplicative_expr_rule<T>::type& multiplicative_expr,
    typename additive_expr_rule<T>::type& additive_expr,
    typename parse::value_ref_parser_rule<T>::type& expr,
    typename parse::value_ref_parser_rule<T>::type& primary_expr
)
{
    qi::_1_type _1;
    qi::_a_type _a;
    qi::_b_type _b;
    qi::_r1_type _r1;
    qi::_val_type _val;
    qi::lit_type lit;
    using phoenix::new_;
    using phoenix::push_back;

    negate_expr
        =    '-' > primary_expr [ _val = new_<ValueRef::Operation<T> >(ValueRef::NEGATE, _1) ]
        |    primary_expr [ _val = _1 ]
        ;

    multiplicative_expr
        =    negate_expr [ push_back(_r1, _1) ]
        >>  *(
                  (
                       lit('*') [ _a = ValueRef::TIMES ]
                   |   lit('/') [ _a = ValueRef::DIVIDES ]
                  )
              >   negate_expr [ push_back(_r1, _1) ]
             ) [ push_back(_r1, _a) ]
        ;

    additive_expr
        =    (
                  multiplicative_expr(_a)
              >> *(
                       (
                            lit('+') [ _b = ValueRef::PLUS ]
                        |   lit('-') [ _b = ValueRef::MINUS ]
                       )
                   >   multiplicative_expr(_a)
                  ) [ push_back(_a, _b) ]
             )
             [ _val = make_expression(_a) ]
        ;

    expr
        %=   additive_expr
        ;
}

template <typename T>
void initialize_numeric_statistic_parser(
    typename statistic_rule<T>::type& statistic,
    const name_token_rule& final_token
)
{
    const parse::lexer& tok = parse::lexer::instance();

    qi::_1_type _1;
    qi::_a_type _a;
    qi::_b_type _b;
    qi::_c_type _c;
    qi::_val_type _val;
    using phoenix::new_;
    using phoenix::push_back;

    statistic
        =    (
                  (
                       tok.Number_ [ _b = ValueRef::COUNT ]
                   >>  tok.Condition_ > '='
                   >   parse::detail::condition_parser [ _c = _1 ]
                  )
              |   (
                       parse::enum_parser<ValueRef::StatisticType>() [ _b = _1 ]
                   >>  tok.Property_ > '='
                   >> -((tok.Planet_ | tok.System_ | tok.Fleet_) [ push_back(_a, _1) ] > '.')
                   >   final_token [ push_back(_a, _1) ]
                   >   tok.Condition_ > '='
                   >   parse::detail::condition_parser [ _c = _1 ]
                  )
             )
             [ _val = new_<ValueRef::Statistic<T> >(_a, _b, _c) ]
        ;
}

template <typename T>
void initialize_nonnumeric_statistic_parser(
    typename statistic_rule<T>::type& statistic,
    const name_token_rule& final_token
)
{
    const parse::lexer& tok = parse::lexer::instance();

    qi::_1_type _1;
    qi::_a_type _a;
    qi::_b_type _b;
    qi::_c_type _c;
    qi::_val_type _val;
    using phoenix::new_;
    using phoenix::push_back;

    statistic
        =    (
                  tok.Mode_ [ _b = ValueRef::MODE ]
              >>  tok.Property_ > '='
              >> -((tok.Planet_ | tok.System_ | tok.Fleet_) [ push_back(_a, _1) ] > '.')
              >   final_token [ push_back(_a, _1) ]
              >   tok.Condition_ > '='
              >   parse::detail::condition_parser [ _c = _1 ]
             )
             [ _val = new_<ValueRef::Statistic<T> >(_a, _b, _c) ]
        ;
}

const name_token_rule& int_var_first_token();
const name_token_rule& int_var_container_token();
const name_token_rule& int_var_final_token();
const name_token_rule& double_var_final_token();
