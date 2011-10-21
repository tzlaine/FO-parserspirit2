#define FUSION_MAX_VECTOR_SIZE 20
#define PHOENIX_LIMIT 11

#include "ParseImpl.h"
#include "EnumParser.h"


namespace {

    struct insert_
    {
        template <typename Arg1, typename Arg2>
        struct result
        { typedef void type; };

        void operator()(std::map<std::string, PartType*>& part_types, PartType* part_type) const
            {
                if (!part_types.insert(std::make_pair(part_type->Name(), part_type)).second) {
                    std::string error_str = "ERROR: More than one ship part in ship_parts.txt has the name " + part_type->Name();
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
                qi::_h_type _h;
                qi::_i_type _i;
                qi::_j_type _j;
                qi::_r1_type _r1;
                qi::_val_type _val;
                qi::eps_type eps;
                using phoenix::construct;
                using phoenix::new_;
                using phoenix::push_back;

                part_stats
                    =    (
                              tok.Type_ > '='
                          >   parse::enum_parser<CombatFighterType>() [ _a = _1 ]
                          >   tok.AntiShipDamage_ > '='
                          >   tok.double_ [ _b = _1 ]
                          >   tok.AntiFighterDamage_ > '='
                          >   tok.double_ [ _c = _1 ]
                          >   tok.LaunchRate_ > '='
                          >   tok.double_ [ _d = _1 ]
                          >   tok.FighterWeaponRange_ > '='
                          >   tok.double_ [ _e = _1 ]
                          >   tok.Speed_ > '='
                          >   tok.double_ [ _f = _1 ]
                          >   tok.Stealth_ > '='
                          >   tok.double_ [ _g = _1 ]
                          >   tok.Structure_ > '='
                          >   tok.double_ [ _h = _1 ]
                          >   tok.Detection_ > '='
                          >   tok.double_ [ _i = _1 ]
                          >   tok.Capacity_ > '='
                          >   tok.int_ [ _val = construct<FighterStats>(_a, _b, _c, _d, _e, _f, _g, _h, _i, _1) ]
                         )
                    |    (
                              tok.Damage_ > '='
                          >   tok.double_ [ _b = _1 ]
                          >   tok.ROF_ > '='
                          >   tok.double_ [ _c = _1 ]
                          >   tok.Range_ > '='
                          >   tok.double_ [ _d = _1 ]
                          >>  (
                                   tok.Speed_ > '='
                               >   tok.double_ [ _e = _1 ]
                               >   tok.Stealth_ > '='
                               >   tok.double_ [ _f = _1 ]
                               >   tok.Structure_ > '='
                               >   tok.double_ [ _g = _1 ]
                               >   tok.Capacity_ > '='
                               >   tok.int_ [ _val = construct<LRStats>(_b, _c, _d, _e, _f, _g, _1) ]
                               |   eps [ _val = construct<DirectFireStats>(_b, _c, _d) ]
                              )
                         )
                    |    (
                              tok.Capacity_ > '='
                          >   tok.double_ [ _val = _1 ]
                         )
                    ;

                part_type
                    =    tok.Part_
                    >    tok.Name_ > '='
                    >    tok.string [ _a = _1 ]
                    >    tok.Description_ > '='
                    >    tok.string [ _b = _1 ]
                    >    tok.PartClass_ > '='
                    >    parse::enum_parser<ShipPartClass>() [ _c = _1 ]
                    >    part_stats [ _d = _1 ]
                    >    tok.BuildCost_ > '='
                    >    tok.double_ [ _e = _1 ]
                    >    tok.BuildTime_ > '='
                    >    tok.int_ [ _f = _1 ]
                    >    (
                              tok.Unproducible_ [ _g = false ]
                          |   tok.Producible_ [ _g = true ]
                          |   eps [ _g = true ]
                         )
                    >    tok.MountableSlotTypes_ > '='
                    >    (
                              '[' > +parse::enum_parser<ShipSlotType>() [ push_back(_h, _1) ] > ']'
                          |   parse::enum_parser<ShipSlotType>() [ push_back(_h, _1) ]
                         )
                    >    tok.Location_ > '='
                    >    parse::detail::condition_parser [ _i = _1 ]
                    >   -(
                              tok.EffectsGroups_
                          >   parse::detail::effects_group_parser() [ _j = _1 ]
                         )
                    >    tok.Graphic_ > '='
                    >    tok.string [ insert(_r1, new_<PartType>(_a, _b, _c, _d, _e, _f, _g, _h, _i, _j, _1)) ]
                    ;

                start
                    =   +part_type(_r1)
                    ;
            }

        typedef boost::spirit::qi::rule<
            parse::token_iterator,
            PartTypeStats (),
            qi::locals<
                CombatFighterType,
                double,
                double,
                double,
                double,
                double,
                double,
                double,
                double
            >,
            parse::skipper_type
        > part_stats_rule;

        typedef boost::spirit::qi::rule<
            parse::token_iterator,
            void (std::map<std::string, PartType*>&),
            qi::locals<
                std::string,
                std::string,
                ShipPartClass,
                PartTypeStats,
                double,
                int,
                bool,
                std::vector<ShipSlotType>,
                Condition::ConditionBase*,
                std::vector<boost::shared_ptr<const Effect::EffectsGroup> >
            >,
            parse::skipper_type
        > part_type_rule;

        typedef boost::spirit::qi::rule<
            parse::token_iterator,
            void (std::map<std::string, PartType*>&),
            parse::skipper_type
        > start_rule;

        part_stats_rule part_stats;
        part_type_rule part_type;
        start_rule start;
    };

}

namespace parse {

    void ship_parts(const boost::filesystem::path& path, std::map<std::string, PartType*>& parts)
    { detail::parse_file<rules, std::map<std::string, PartType*> >(path, parts); }

}
