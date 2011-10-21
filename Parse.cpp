#include "ParseImpl.h"

#include "EffectParser.h"
#include "../universe/Effect.h"


namespace {

    struct rules
    {
        rules()
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
                    >    tok.Scope_ > '='
                    >    parse::detail::condition_parser [ _a = _1 ]
                    >   -(
                              tok.Activation_ > '='
                          >   parse::detail::condition_parser [ _b = _1 ]
                         )
                    >   -(
                              tok.StackingGroup_ > '='
                          >   tok.string [ _c = _1 ]
                         )
                    >    tok.Effects_ > '='
                    >    '['
                    >    (+parse::effect_parser()) [ _d = _1 ]
                    >    lit(']') [ _val = new_<Effect::EffectsGroup>(_a, _b, _d, _c) ]
                    ;

                start
                    =    effects_group [ push_back(_val, construct<boost::shared_ptr<const Effect::EffectsGroup> >(_1)) ]
                    |    '[' > +effects_group [ push_back(_val, construct<boost::shared_ptr<const Effect::EffectsGroup> >(_1)) ] > ']'
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
        parse::effects_group_rule start;
    };

}

namespace parse {

    effects_group_rule& effects_group_parser()
    {
        static rules rules_;
        return rules_.start;
    }

}
