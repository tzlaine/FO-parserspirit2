#include "ParseImpl.h"


namespace {

    struct rules
    {
        rules()
            {
                qi::_1_type _1;
                qi::_r1_type _r1;
                using phoenix::push_back;

                start
                    =   +parse::detail::item_spec_parser() [ push_back(_r1, _1) ]
                    ;
            }

        typedef boost::spirit::qi::rule<
            parse::token_iterator,
            void (std::vector<ItemSpec>&),
            parse::skipper_type
        > start_rule;

        start_rule start;
    };

}

namespace parse {

    void items(const boost::filesystem::path& path, std::vector<ItemSpec>& items_)
    { detail::parse_file<rules, std::vector<ItemSpec> >(path, items_); }

}
