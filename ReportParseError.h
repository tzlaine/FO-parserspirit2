#ifndef _ReportParseError_h_
#define _ReportParseError_h_

#include "Lexer.h"

#include <boost/tuple/tuple.hpp>
#include <boost/xpressive/xpressive.hpp>


namespace parse {

    template <typename Stream>
    struct printer
    {
        enum state {
            none,
            alternative,
            sequence
        };

        typedef boost::spirit::utf8_string string;

        printer(Stream& os) : m_os(os) {}

        std::string indentation() const
            {
                std::size_t indent = 0;
                for (std::size_t i = 0; i < m_state.size(); ++i) {
                    indent += boost::get<0>(m_state[i]) == alternative ? 1 : 0;
                }
                return '\n' + std::string(indent * 4, ' ');
            }

        void increment_element_of(state s) const
            {
                for (std::size_t i = m_state.size(); 0 < i; ) {
                    --i;
                    if (boost::get<0>(m_state[i]) == s) {
                        ++boost::get<1>(m_state[i]);
                        if (s == sequence)
                            s = alternative;
                        else
                            break;
                    }
                }
            }

        std::size_t element_of(state s) const
            {
                std::size_t retval = 0;
                for (std::size_t i = m_state.size(); 0 < i; ) {
                    --i;
                    if (boost::get<0>(m_state[i]) == s) {
                        retval = boost::get<1>(m_state[i]);
                        break;
                    }
                }
                return retval;
            }

        void element(string const& tag, string const& value, std::size_t depth) const
        {
            while (m_state.size() < depth + 1) {
                if (m_state.empty())
                    m_state.push_back(state_tuple(none, 0));
                else
                    m_state.push_back(m_state.back());
            }
            while (depth + 1 < m_state.size()) {
                state_tuple popped_state = m_state.back();
                m_state.pop_back();
                if (!m_state.empty() && boost::get<1>(popped_state) && boost::get<0>(m_state.back()) == sequence)
                    boost::get<1>(m_state.back()) += boost::get<1>(popped_state);
            }

            if (value == "") {
                if (tag == "alternative") {
                    m_state.push_back(state_tuple(alternative, 0));
                    return;
                } else if (tag == "expect" || tag == "sequence") {
                    m_state.push_back(state_tuple(sequence, 0));
                    return;
                } else {
                    m_os << '<' << tag << '>';
                }
            } else {
                std::string str;
                {
                    std::stringstream ss;
                    ss << value;
                    str = ss.str();
                }
                if (str == parse::lexer::bool_regex)
                    str = "boolean (true or false)";
                else if (str == parse::lexer::string_regex)
                    str = "string";
                else if (str == parse::lexer::int_regex)
                    str = "integer";
                else if (str == parse::lexer::double_regex)
                    str = "real number";
                if (!m_state.empty()) {
                    const state s = boost::get<0>(m_state.back());
                    increment_element_of(s);
                    const std::size_t element_of_sequence = element_of(sequence);
                    const std::size_t element_of_alternative = element_of(alternative);
                    if (s == sequence && 2u <= element_of_sequence)
                        m_os << ' ';
                    else if (s == sequence && element_of_sequence == 1u && 2u <= element_of_alternative ||
                             s == alternative && 2u <= element_of_alternative)
                        m_os << indentation() << "- OR -" << indentation();
                    else if (depth)
                        m_os << indentation();
                }
                m_os << str;
            }
        }

        Stream& m_os;
        typedef boost::tuple<state, std::size_t> state_tuple; // state, number of elements printed in this state
        mutable std::vector<state_tuple> m_state;
        mutable std::vector<int> m_bias;
    };

    template <typename Stream>
    void pretty_print(Stream& os, boost::spirit::info const& what)
    {
        printer<Stream> pr(os);
        boost::spirit::basic_info_walker<printer<Stream> > w(pr, what.tag, 0);
        boost::apply_visitor(w, what.value);
        if (what.tag == "alternative")
            os << '\n';
        else
            os << ' ';
    }

    namespace detail {

        inline void default_send_error_string(const std::string& str)
        { std::cerr << str; }

        extern const char* s_filename;
        extern GG::text_iterator* s_text_it;
        extern GG::text_iterator s_begin;
        extern GG::text_iterator s_end;

    }

    template <typename TokenType>
    struct report_error_
    {
        template <typename Arg1, typename Arg2, typename Arg3, typename Arg4>
        struct result
        { typedef void type; };

        template <typename Arg1, typename Arg2, typename Arg3, typename Arg4>
        void operator()(Arg1 first, Arg2, Arg3 it, Arg4 rule_name) const
            {
                std::string error_string;
                generate_error_string(first, it, rule_name, error_string);
                send_error_string(error_string);
            }

        static boost::function<void (const std::string&)> send_error_string;

    private:
        std::pair<GG::text_iterator, unsigned int> line_start_and_line_number(GG::text_iterator error_position) const
            {
                unsigned int line = 1;
                GG::text_iterator it = detail::s_begin;
                GG::text_iterator line_start = detail::s_begin;
                while (it != error_position) {
                    bool eol = false;
                    if (it != error_position && *it == '\r') {
                        eol = true;
                        line_start = ++it;
                    }
                    if (it != error_position && *it == '\n') {
                        eol = true;
                        line_start = ++it;
                    }
                    if (eol)
                        ++line;
                    else
                        ++it;
                }
                return std::pair<GG::text_iterator, unsigned int>(line_start, line);
            }

        std::string get_line(GG::text_iterator line_start) const
            {
                GG::text_iterator line_end = line_start;
                while (line_end != detail::s_end && *line_end != '\r' && *line_end != '\n') {
                    ++line_end;
                }
                return std::string(line_start, line_end);
            }

        template <typename TokenIter>
        void generate_error_string(const TokenIter& first,
                                   const TokenIter& it,
                                   const boost::spirit::info& rule_name,
                                   std::string& str) const
            {
                std::stringstream is;

                GG::text_iterator line_start;
                unsigned int line_number;
                GG::text_iterator text_it = it->matched().begin();
                if (it->matched().begin() == it->matched().end()) {
                    text_it = *detail::s_text_it;
                    if (text_it != detail::s_end)
                        ++text_it;
                }
                boost::tie(line_start, line_number) = line_start_and_line_number(text_it);
                std::size_t column_number = std::distance(line_start, text_it);

                is << detail::s_filename << ":" << line_number << ":" << column_number << ": "
                   << "Parse error.  Expected ";

                {
                    std::stringstream os;
                    pretty_print(os, rule_name);
                    using namespace boost::xpressive;
                    sregex regex = sregex::compile("(?<=\\[ ).+(?= \\])");
                    is << regex_replace(os.str(), regex, "$&, ...");
                }

                if (text_it == detail::s_end) {
                    is << "before end of input.\n";
                } else {
                    is << " here:\n"
                       << "  " << get_line(line_start) << "\n"
                       << "  " << std::string(column_number, ' ') << '^' << std::endl;
                }

                str = is.str();
            }
    };

    template <typename TokenType>
    boost::function<void (const std::string&)> report_error_<TokenType>::send_error_string =
        &detail::default_send_error_string;

    extern const boost::phoenix::function<report_error_<GG::token_type> > report_error;

}

#endif
