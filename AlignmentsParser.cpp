#include "ParseImpl.h"

#include "../Empire/Empire.h"



namespace {

    std::vector<boost::shared_ptr<const Effect::EffectsGroup> >* g_effects_groups = 0;

    struct rules
    {
        rules()
            {
                const parse::lexer& tok = parse::lexer::instance();

                qi::_1_type _1;
                qi::_2_type _2;
                qi::_3_type _3;
                qi::_4_type _4;
                qi::_a_type _a;
                qi::_b_type _b;
                qi::_r1_type _r1;
                using phoenix::construct;
                using phoenix::new_;
                using phoenix::push_back;

                alignment
                    =    tok.Alignment_
                    >    tok.Name_ > '='
                    >    tok.string [ _a = _1 ]
                    >    tok.Description_ > '='
                    >    tok.string [ _b = _1 ]
                    >    tok.Graphic_ > '='
                    >    tok.string [ push_back(_r1, construct<Alignment>(_a, _b, _1)) ]
                    ;

                start
                    =   +(
                              alignment(_r1)
                          >> -(
                                   tok.AlignmentEffects_
                               >   tok.EffectsGroups_ > '='
                               >   parse::detail::effects_group_parser() [ phoenix::ref(*g_effects_groups) = _1 ]
                              )
                         )
                    ;
            }

        typedef boost::spirit::qi::rule<
            parse::token_iterator,
            void (std::vector<Alignment>&),
            qi::locals<
                std::string,
                std::string
            >,
            parse::skipper_type
        > alignment_rule;

        typedef boost::spirit::qi::rule<
            parse::token_iterator,
            void (std::vector<Alignment>&),
            parse::skipper_type
        > start_rule;

        alignment_rule alignment;
        start_rule start;
    };

}

namespace parse {

    void alignments(const boost::filesystem::path& path,
                    std::vector<Alignment>& alignments_,
                    std::vector<boost::shared_ptr<const Effect::EffectsGroup> >& effects_groups)
    {
        g_effects_groups = &effects_groups;
        detail::parse_file<rules, std::vector<Alignment> >(path, alignments_);
    }

}
