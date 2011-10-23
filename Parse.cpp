#include "ParseImpl.h"

#include "EffectParser.h"
#include "Label.h"
#include "ValueRefParser.h"
#include "../universe/Effect.h"


namespace {

    struct effects_group_rules
    {
        effects_group_rules()
            {
                const parse::lexer& tok = parse::lexer::instance();

                qi::_1_type _1;
                qi::_a_type _a;
                qi::_b_type _b;
                qi::_c_type _c;
                qi::_d_type _d;
                qi::_val_type _val;
                qi::lit_type lit;
                using phoenix::construct;
                using phoenix::new_;
                using phoenix::push_back;

                effects_group
                    =    tok.EffectsGroup_
                    >    parse::label(Scope_name) > parse::detail::condition_parser [ _a = _1 ]
                    >   -(
                              parse::label(Activation_name) > parse::detail::condition_parser [ _b = _1 ]
                         )
                    >   -(
                              parse::label(StackingGroup_name) > tok.string [ _c = _1 ]
                         )
                    >    parse::label(Effects_name) > '[' > (+parse::effect_parser()) [ _d = _1 ] > lit(']')
                         [ _val = new_<Effect::EffectsGroup>(_a, _b, _d, _c) ]
                    ;

                start
                    =    '[' > +effects_group [ push_back(_val, construct<boost::shared_ptr<const Effect::EffectsGroup> >(_1)) ] > ']'
                    |    effects_group [ push_back(_val, construct<boost::shared_ptr<const Effect::EffectsGroup> >(_1)) ]
                    ;
            }

        typedef boost::spirit::qi::rule<
            parse::token_iterator,
            Effect::EffectsGroup* (),
            qi::locals<
                Condition::ConditionBase*,
                Condition::ConditionBase*,
                std::string,
                std::vector<Effect::EffectBase*>
            >,
            parse::skipper_type
        > effects_group_rule;

        effects_group_rule effects_group;
        parse::detail::effects_group_rule start;
    };

    struct color_parser_rules
    {
        color_parser_rules()
            {
                qi::_1_type _1;
                qi::_a_type _a;
                qi::_b_type _b;
                qi::_c_type _c;
                qi::_pass_type _pass;
                qi::_val_type _val;
                qi::eps_type eps;
                qi::uint_type uint_;
                using phoenix::construct;
                using phoenix::if_;

                channel
                    =    uint_
                         [
                             if_ (_1 <= 255u) [
                                 _pass = false
                             ] .else_ [
                                 _val = _1
                             ]
                         ]
                    ;

                start
                    =    channel [ _a = _1 ]
                    >    channel [ _b = _1 ]
                    >    channel [ _c = _1 ]
                    >    (
                              channel [ construct<GG::Clr>(_a, _b, _c, _1) ]
                          |   eps [ construct<GG::Clr>(_a, _b, _c, 255) ]
                         )
                    ;
            }

        typedef boost::spirit::qi::rule<
            parse::token_iterator,
            unsigned int (),
            parse::skipper_type
        > rule;

        rule channel;
        parse::detail::color_parser_rule start;
    };

    struct item_spec_parser_rules
    {
        item_spec_parser_rules()
            {
                const parse::lexer& tok = parse::lexer::instance();

                qi::_1_type _1;
                qi::_a_type _a;
                qi::_val_type _val;
                using phoenix::construct;

                start
                    =    tok.Item_
                    >    parse::label(Type_name) > parse::enum_parser<UnlockableItemType>() [ _a = _1 ]
                    >    parse::label(Name_name) > tok.string [ _val = construct<ItemSpec>(_a, _1) ]
                    ;
            }

        parse::detail::item_spec_parser_rule start;
    };

}

namespace parse {

    void init()
    {
        value_ref_parser<int>();
        condition_parser();
    }

    namespace detail {

        effects_group_rule& effects_group_parser()
        {
            static effects_group_rules rules;
            return rules.start;
        }

        color_parser_rule& color_parser()
        {
            static color_parser_rules rules;
            return rules.start;
        }

        item_spec_parser_rule& item_spec_parser()
        {
            static item_spec_parser_rules rules;
            return rules.start;
        }

        void parse_file_common(const boost::filesystem::path& path,
                               const parse::lexer& l,
                               parse::text_iterator& first,
                               parse::token_iterator& it)
        {
            const std::string filename = path.string();

            std::string file_contents;
            {
                boost::filesystem::ifstream ifs(path);
                if (ifs) {
                    while (ifs) {
                        file_contents += ifs.get();
                    }
                } else {
                    Logger().errorStream() << "Unable to open data file " << filename;
                    return;
                }
            }

            first = parse::text_iterator(file_contents.begin());
            parse::text_iterator last(file_contents.end());

            GG::detail::s_begin = first;
            GG::detail::s_end = last;
            GG::detail::s_filename = filename.c_str();
            it = l.begin(first, last);
        }

    }

}
