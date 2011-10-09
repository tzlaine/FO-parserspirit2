#include "ConditionParserImpl.h"


namespace parse {

    const condition_parser_rule& condition_parser(const lexer& tok)
    {
        static const condition_parser_rule& rules_1 = detail::condition_parser_1(tok);
        static const condition_parser_rule& rules_2 = detail::condition_parser_2(tok);
        static const condition_parser_rule& rules_3 = detail::condition_parser_3(tok);
        static condition_parser_rule retval;
        static bool once = true;
        if (once) {
            retval
                %=   rules_1
                |    rules_2
                |    rules_3
                ;
            once = false;
        }
        return retval;
    }

}
