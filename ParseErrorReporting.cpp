#include "ParseErrorReporting.h"


namespace parse { namespace detail {

    const boost::phoenix::function<report_error_> report_error = report_error_();

    void report_error_::default_send_error_string(const std::string& str)
    { std::cerr << str; }

    void report_error_::generate_error_string(const token_type& first,
                                              const token_type& it,
                                              const boost::spirit::info& rule_name,
                                              bool at_end,
                                              std::string& str) const
    {
        std::stringstream is;

        is << it.filename() << ":" << (it.line_number() + 1) << ": "
           << "Parse error: expected " << rule_name;

        if (at_end) {
            is << " before end of input.\n";
        } else {
            std::string match(it.matched_range().first, it.matched_range().second);
            text_iterator it_start = it.matched_range().first;

            // Use the end of the token's matched range, if its entire match was
            // whitespace.
            if (match.find_first_not_of(" \t\n\r\f\v") == std::string::npos)
                it_start = it.matched_range().second;

            if (it_start < it.line_start())
                it_start = it.line_start();

            std::string line_start_though_it_start(it.line_start(), it_start);

            // expand tabs (ugh!)
            {
                std::string line_start_though_it_start_copy;
                std::swap(line_start_though_it_start_copy, line_start_though_it_start);
                line_start_though_it_start.clear();
                std::string tab(4, ' ');
                for (std::size_t i = 0; i < line_start_though_it_start_copy.size(); ++i) {
                    if (line_start_though_it_start_copy[i] == '\t')
                        line_start_though_it_start += tab;
                    else
                        line_start_though_it_start += line_start_though_it_start_copy[i];
                }
            }

            std::string position_indicator(line_start_though_it_start.size(), '~');

            is << " here:\n"
               << "  " << line_start_though_it_start << match << "\n"
               << "  " << position_indicator << '^'
               << std::endl;
        }

        str = is.str();
    }

    boost::function<void (const std::string&)> detail::report_error_::send_error_string =
        &detail::report_error_::default_send_error_string;

} }
