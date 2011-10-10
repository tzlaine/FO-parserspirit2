#include "ConditionParserImpl.h"


namespace parse {

    const condition_parser_rule& condition_parser()
    {
        boost::spirit::qi::_1_type _1;
        boost::spirit::qi::_val_type _val;
        static condition_parser_rule retval
            =    detail::condition_parser_1() [ _val = _1 ]
            |    detail::condition_parser_2() [ _val = _1 ]
            |    detail::condition_parser_3() [ _val = _1 ]
            ;
        return retval;
    }

}
