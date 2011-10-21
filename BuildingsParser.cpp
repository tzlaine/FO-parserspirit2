#define FUSION_MAX_VECTOR_SIZE 20

#include "ParseImpl.h"
#include "../universe/Building.h"


namespace {

    struct insert_
    {
        template <typename Arg1, typename Arg2>
        struct result
        { typedef void type; };

        void operator()(std::map<std::string, BuildingType*>& building_types, BuildingType* building_type) const
            {
                if (!building_types.insert(std::make_pair(building_type->Name(), building_type)).second) {
                    std::string error_str = "ERROR: More than one building type in buildings.txt has the name " + building_type->Name();
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
                qi::_r1_type _r1;
                qi::_val_type _val;
                qi::eps_type eps;
                using phoenix::new_;

                building_type
                    =    tok.BuildingType_
                    >    tok.Name_ > '='
                    >    tok.string [ _a = _1 ]
                    >    tok.Description_ > '='
                    >    tok.string [ _b = _1 ]
                    >    tok.BuildCost_ > '='
                    >    tok.double_ [ _c = _1 ]
                    >    tok.BuildTime_ > '='
                    >    tok.int_ [ _d = _1 ]
                    >    (
                             tok.Producible_ [ _e = true ]
                          |  tok.Unproducible_ [ _e = false ]
                         )
                    >    tok.Location_ > '='
                    >    parse::detail::condition_parser [ _f = _1 ]
                    >    tok.CaptureResult_ > '='
                    >    (
                              parse::enum_parser<CaptureResult>() [ _g = _1 ]
                          |   eps [ _g = CR_CAPTURE ]
                         )
#if 0
                    >   -(
                              tok.EffectsGroups_ > '='
                          >   effects_group [ _h = _1 ]
                         )
#endif
                    >    tok.Graphic_ > '='
                    >    tok.string [ insert(_r1, new_<BuildingType>(_a, _b, _c, _d, _e, _g, _f, _h, _1)) ]
                    ;

                start
                    =   +building_type(_val)
                    ;
            }

        typedef boost::spirit::qi::rule<
            parse::token_iterator,
            void (std::map<std::string, BuildingType*>&),
            qi::locals<
                std::string,
                std::string,
                double,
                int,
                bool,
                Condition::ConditionBase*,
                CaptureResult,
                std::vector<boost::shared_ptr<const Effect::EffectsGroup> >
            >,
            parse::skipper_type
        > building_type_rule;

        typedef boost::spirit::qi::rule<
            parse::token_iterator,
            std::map<std::string, BuildingType*> (),
            parse::skipper_type
        > start_rule;

        building_type_rule building_type;
        start_rule start;
    };

}

namespace parse {

    void buildings(const boost::filesystem::path& path, std::map<std::string, BuildingType*>& building_types)
    { parse_file<rules, std::map<std::string, BuildingType*> >(path, building_types); }

}
