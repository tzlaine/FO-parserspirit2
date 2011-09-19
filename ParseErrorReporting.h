// -*- C++ -*-
#ifndef _ParseErrorReporting_
#define _ParseErrorReporting_

#include "Lexer.h"

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>


namespace parse { namespace detail {

    struct report_error_
    {
        template <typename Arg1, typename Arg2, typename Arg3, typename Arg4>
        struct result
        { typedef void type; };

        template <typename Arg1, typename Arg2, typename Arg3, typename Arg4>
        void operator()(Arg1 first, Arg2 last, Arg3 it, Arg4 rule_name) const
            {
                std::string error_string;
                generate_error_string(*first, *it, rule_name, first == last, error_string);
                send_error_string(error_string);
            }

        static boost::function<void (const std::string&)> send_error_string;
        static void default_send_error_string(const std::string& str);

    private:
        void generate_error_string(const token_type& first,
                                   const token_type& it,
                                   const boost::spirit::info& rule_name,
                                   bool at_end,
                                   std::string& str) const;
    };

    extern const boost::phoenix::function<report_error_> report_error;

} }

#endif
