#ifndef _ReportParseError_h_
#define _ReportParseError_h_

#include "Lexer.h"

#include <boost/tuple/tuple.hpp>
#include <boost/xpressive/xpressive.hpp>


namespace parse {

    template <typename Stream>
    struct info_visitor
    {
        typedef void result_type;
        typedef info_visitor<Stream> this_type;
        typedef boost::spirit::utf8_string string;

        enum indented_or_flat {
            indented,
            flat
        };

        info_visitor(Stream& os, const string& tag, std::size_t indent, indented_or_flat indented_or_flat_ = indented) :
            m_os(os),
            m_tag(tag),
            m_indent(indent),
            m_indented_or_flat(indented_or_flat_)
            {}

        void indent() const
            {
                if (m_indent)
                    m_os << std::string(m_indent, ' ');
            }

        std::string prepare(const string& s) const
            {
                std::string str = s;
                if (str == parse::lexer::bool_regex)
                    str = "boolean (true or false)";
                else if (str == parse::lexer::string_regex)
                    str = "string";
                else if (str == parse::lexer::int_regex)
                    str = "integer";
                else if (str == parse::lexer::double_regex)
                    str = "real number";
                else if (str.find("(?i:") == 0)
                    str = str.substr(4, str.size() - 5);
                return str;
            }

        void print(const string& str) const
            { m_os << prepare(str); }

        void operator()(boost::spirit::info::nil) const
        {
            indent();
            print(m_tag);
        }

        void operator()(const string& str) const
        {
            indent();
            print(str);
        }

        void operator()(const boost::spirit::info& what) const
        { boost::apply_visitor(this_type(m_os, what.tag, m_indent, m_indented_or_flat), what.value); }

        void operator()(const std::pair<boost::spirit::info, boost::spirit::info>& pair) const
        {
            const boost::spirit::info* infos = &pair.first;
            multi_info(infos, infos + 2);
        }

        void operator()(const std::list<boost::spirit::info>& l) const
        { multi_info(l.begin(), l.end()); }

        template <typename Iter>
        void multi_info(Iter first, const Iter last) const
        {
            if (m_tag == "sequence" || m_tag == "expect") {
                if (first->tag.find(" =") == first->tag.size() - 2)
                    ++first;
                const string* value = boost::get<string>(&first->value);
                if (value && *value == "[") {
                    for (; first != last; ++first) {
                        boost::apply_visitor(this_type(m_os, first->tag, 1, flat), first->value);
                    }
                } else {
                    boost::apply_visitor(this_type(m_os, first->tag, 1, flat), first->value);
                }
            } else if (m_tag == "alternative") {
                std::size_t indent_ = m_indented_or_flat == flat ? 1 : m_indent + 4;
                boost::apply_visitor(this_type(m_os, first->tag, indent_, m_indented_or_flat), first->value);
                indent();
                ++first;
                for (; first != last; ++first) {
                    m_os << "-OR-";
                    boost::apply_visitor(this_type(m_os, first->tag, indent_, m_indented_or_flat), first->value);
                }
            }
        }

        Stream& m_os;
        const string& m_tag;
        int m_indent;
        indented_or_flat m_indented_or_flat;
    };

    template <typename Stream>
    void pretty_print(Stream& os, boost::spirit::info const& what)
    {
        info_visitor<Stream> v(os, what.tag, 1);
        boost::apply_visitor(v, what.value);
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
