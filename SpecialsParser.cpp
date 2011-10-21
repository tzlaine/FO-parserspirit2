#define FUSION_MAX_VECTOR_SIZE 20

#include "ParseImpl.h"
#include "../universe/Special.h"


namespace {

    struct insert_
    {
        template <typename Arg1, typename Arg2>
        struct result
        { typedef void type; };

        void operator()(std::map<std::string, Special*>& specials, Special* special) const
            {
                if (!specials.insert(std::make_pair(special->Name(), special)).second) {
                    std::string error_str = "ERROR: More than one special in specials.txt has the name " + special->Name();
                    throw std::runtime_error(error_str.c_str());
                }
            }
    };
    const boost::phoenix::function<insert_> insert;

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
                qi::_c_type _c;
                qi::_d_type _d;
                qi::_e_type _e;
                qi::_f_type _f;
                qi::_g_type _g;
                qi::_r1_type _r1;
                qi::_val_type _val;
                qi::eps_type eps;
                using phoenix::new_;

                special
                    =    tok.Special_
                    >    tok.Name_ > '='
                    >    tok.string [ _a = _1 ]
                    >    tok.Description_ > '='
                    >    tok.string [ _b = _1 ]
                    >    (
                              tok.SpawnRate_ > '='
                          >   tok.double_ [ _c = _1 ]
                          |   eps [ _c = 1.0 ]
                         )
                    >    (
                              tok.SpawnLimit_ > '='
                          >   tok.int_ [ _d = _1 ]
                          |   eps [ _d = 9999 ]
                         )
                    >   -(
                              tok.Location_ > '='
                          >   parse::detail::condition_parser [ _e = _1 ]
                         )
                    >   -(
                              tok.EffectsGroups_ > '='
                          >   parse::effects_group_parser() [ _f = _1 ]
                         )
                    >    tok.Graphic_ > '='
                    >    tok.string [ insert(_r1, new_<Special>(_a, _b, _f, _c, _d, _e, _1)) ]
                    ;

                start
                    =   +special(_val)
                    ;
            }

        typedef boost::spirit::qi::rule<
            parse::token_iterator,
            void (std::map<std::string, Special*>&),
            qi::locals<
                std::string,
                std::string,
                double,
                int,
                Condition::ConditionBase*,
                std::vector<boost::shared_ptr<const Effect::EffectsGroup> >
            >,
            parse::skipper_type
        > special_rule;

        typedef boost::spirit::qi::rule<
            parse::token_iterator,
            std::map<std::string, Special*> (),
            parse::skipper_type
        > start_rule;

        special_rule special;
        start_rule start;
    };

}

namespace parse {

    void specials(const boost::filesystem::path& path, std::map<std::string, Special*>& specials_)
    { parse_file<rules, std::map<std::string, Special*> >(path, specials_); }

}