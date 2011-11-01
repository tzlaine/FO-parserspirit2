#include "ReportParseError.h"


namespace parse {

    namespace detail {

        const char* s_filename = 0;
        GG::text_iterator* s_text_it = 0;
        GG::text_iterator s_begin;
        GG::text_iterator s_end;

    }

    const boost::phoenix::function<report_error_<GG::token_type> > report_error;

}
