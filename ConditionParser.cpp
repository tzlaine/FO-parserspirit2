#include "ConditionParserImpl.h"


namespace parse {

    namespace detail {

        condition_parser_rule condition_parser;

    }

    const condition_parser_rule& condition_parser()
    {
        static bool once = true;
        if (once) {
            detail::condition_parser
                %=   detail::condition_parser_1()
                |    detail::condition_parser_2()
                |    detail::condition_parser_3()
                ;
            once = false;
        }
        return detail::condition_parser;
    }

}
