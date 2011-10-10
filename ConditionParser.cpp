#include "ConditionParserImpl.h"


namespace parse {

    const condition_parser_rule& condition_parser()
    {
        static const condition_parser_rule& rule_1 = detail::condition_parser_1();
        static const condition_parser_rule& rule_2 = detail::condition_parser_2();
        static const condition_parser_rule& rule_3 = detail::condition_parser_3();
        static condition_parser_rule retval;
        static bool once = true;
        if (once) {
            retval
                %=   rule_1
                |    rule_2
                |    rule_3
                ;
            once = false;
        }
        return retval;
    }

}
