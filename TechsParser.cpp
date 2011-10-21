#define FUSION_MAX_VECTOR_SIZE 20

#include "ParseImpl.h"
#include "../universe/Species.h"


namespace {

    std::set<std::string> g_categories_seen;
    std::map<std::string, TechCategory*>* g_categories = 0;

    struct insert_tech_
    {
        template <typename Arg1, typename Arg2>
        struct result
        { typedef void type; };

        void operator()(TechManager::TechContainer& techs, Tech* tech) const
            {
                g_categories_seen.insert(tech->Category());
                if (techs.get<TechManager::NameIndex>().find(tech->Name()) != techs.get<TechManager::NameIndex>().end()) {
                    std::string error_str = "ERROR: More than one tech in techs.txt has the name " + tech->Name();
                    throw std::runtime_error(error_str.c_str());
                }
                if (tech->Prerequisites().find(tech->Name()) != tech->Prerequisites().end()) {
                    std::string error_str = "ERROR: Tech " + tech->Name() + " depends on itself!";
                    throw std::runtime_error(error_str.c_str());
                }
                techs.insert(tech);
            }
    };
    const boost::phoenix::function<insert_tech_> insert_tech;

    struct insert_category_
    {
        template <typename Arg1, typename Arg2>
        struct result
        { typedef void type; };

        void operator()(std::map<std::string, TechCategory*>& categories, TechCategory* category) const
            {
                if (!categories.insert(std::make_pair(category->name, category)).second) {
                    std::string error_str = "ERROR: More than one tech category in techs.txt name " + category->name;
                    throw std::runtime_error(error_str.c_str());
                }
            }
    };
    const boost::phoenix::function<insert_category_> insert_category;

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
                qi::_h_type _h;
                qi::_r1_type _r1;
                qi::_r1_type _r2;
                qi::_val_type _val;
                qi::eps_type eps;
                using phoenix::construct;
                using phoenix::insert;
                using phoenix::new_;
                using phoenix::push_back;

                tech_info
                    =    tok.Name_ > '='
                    >    tok.string [ _a = _1 ]
                    >    tok.Description_ > '='
                    >    tok.string [ _b = _1 ]
                    >    tok.Short_Description_ > '=' // TODO: Get rid of underscore.
                    >    tok.string [ _c = _1 ]
                    >    tok.TechType_ > '='
                    >    parse::enum_parser<TechType>() [ _d = _1 ]
                    >    tok.Category_ > '='
                    >    tok.string [ _e = _1 ]
                    >    (
                              tok.ResearchCost_ > '='
                          >   tok.double_ [ _f = _1 ]
                          |   eps [ _f = 1.0 ]
                         )
                    >    (
                              tok.ResearchTurns_ > '='
                          >   tok.int_ [ _g = _1 ]
                          |   eps [ _g = 1 ]
                         )
                    >    (
                              tok.Unresearchable_ [ _h = false ]
                          |   tok.Researchable_ [ _h = true ]
                          |   eps [ _h = true ]
                         )
                         [ _val = construct<Tech::TechInfo>(_a, _b, _c, _e, _d, _f, _g, _h) ]
                    ;

                tech
                    =    tok.Tech_
                    >    tech_info [ _a = _1 ]
                    >   -(
                              tok.Prerequisites_ > '='
                          >   (
                                   '[' > +tok.string [ insert(_b, _1) ] > ']'
                               |   tok.string [ insert(_b, _1) ]
                              )
                         )
                    >   -(
                              tok.Unlock_ > '='
                          >   (
                                   '[' > +parse::detail::item_spec_parser() [ push_back(_c, _1) ] > ']'
                               |   parse::detail::item_spec_parser() [ push_back(_c, _1) ]
                              )
                         )
                    >   -(
                              tok.EffectsGroups_ > '='
                          >   parse::detail::effects_group_parser() [ _d = _1 ]
                         )
                    >   -(
                              tok.Graphic_ > '='
                          >   tok.string [ _e = _1 ]
                         )
                         [ insert_tech(_r1, new_<Tech>(_a, _d, _b, _c, _e)) ]
                    ;

                category
                    =    tok.Category_ > '='
                    >    tok.Name_ > '='
                    >    tok.string [ _a = _1 ]
                    >    tok.Graphic_ > '='
                    >    tok.string [ _b = _1 ]
                    >    tok.Colour_ > '='
                    >    parse::detail::color_parser() [ insert_category(_r1, new_<TechCategory>(_a, _b, _1)) ]
                    ;

                start
                    =   +(
                              tech(_r1)
                          |   category(phoenix::ref(*g_categories)) // TODO: Using _r2 here as I would lke to do seems to give GCC 4.6 fits.
                         )
                    ;
            }

        typedef boost::spirit::qi::rule<
            parse::token_iterator,
            Tech::TechInfo (),
            qi::locals<
                std::string,
                std::string,
                std::string,
                TechType,
                std::string,
                double,
                int,
                bool
            >,
            parse::skipper_type
        > tech_info_rule;

        typedef boost::spirit::qi::rule<
            parse::token_iterator,
            void (TechManager::TechContainer&),
            qi::locals<
                Tech::TechInfo,
                std::set<std::string>,
                std::vector<ItemSpec>,
                std::vector<boost::shared_ptr<const Effect::EffectsGroup> >,
                std::string
            >,
            parse::skipper_type
        > tech_rule;

        typedef boost::spirit::qi::rule<
            parse::token_iterator,
            void (std::map<std::string, TechCategory*>&),
            qi::locals<
                std::string,
                std::string
            >,
            parse::skipper_type
        > category_rule;

        typedef boost::spirit::qi::rule<
            parse::token_iterator,
            void (TechManager::TechContainer&),
            parse::skipper_type
        > start_rule;

        tech_info_rule tech_info;
        tech_rule tech;
        category_rule category;
        start_rule start;
    };

}

namespace parse {

    void techs(const boost::filesystem::path& path, TechManager::TechContainer& techs_, std::map<std::string, TechCategory*>& categories)
    {
        g_categories_seen.clear();
        g_categories = &categories;
        detail::parse_file<rules, TechManager::TechContainer>(path, techs_);
    }

}
