#define FUSION_MAX_VECTOR_SIZE 20

#include "ParseImpl.h"
#include "Label.h"
#include "../universe/Species.h"


namespace {

    struct insert_species_
    {
        template <typename Arg1, typename Arg2>
        struct result
        { typedef void type; };

        void operator()(std::map<std::string, Species*>& species, Species* specie) const
            {
                if (!species.insert(std::make_pair(specie->Name(), specie)).second) {
                    std::string error_str = "ERROR: More than one species in species.txt has the name " + specie->Name();
                    throw std::runtime_error(error_str.c_str());
                }
            }
    };
    const boost::phoenix::function<insert_species_> insert_species;

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
                using phoenix::construct;
                using phoenix::insert;
                using phoenix::new_;
                using phoenix::push_back;

                focus_type
                    =    tok.Focus_
                    >    parse::label(Name_name)        > tok.string [ _a = _1 ]
                    >    parse::label(Description_name) > tok.string [ _b = _1 ]
                    >    parse::label(Location_name)    > parse::detail::condition_parser [ _c = _1 ]
                    >    parse::label(Graphic_name)     > tok.string [ _val = construct<FocusType>(_a, _b, _c, _1) ]
                    ;

                environment_map_element
                    =    parse::label(Type_name)        > parse::enum_parser<PlanetType>() [ _a = _1 ]
                    >    parse::label(Environment_name) > parse::enum_parser<PlanetEnvironment>() [ _val = construct<std::pair<PlanetType, PlanetEnvironment> >(_a, _1) ]
                    ;

                environment_map
                    =    '[' > +environment_map_element [ insert(_val, _1) ] > ']'
                    |    environment_map_element [ insert(_val, _1) ]
                    ;

                species
                    =    tok.Species_
                    >    parse::label(Name_name)        > tok.string [ _a = _1 ]
                    >    parse::label(Description_name) > tok.string [ _b = _1 ]
                    >   -tok.Playable_ [ _c = true ]
                    >   -tok.CanProduceShips_ [ _d = true ]
                    >   -tok.CanColonize_ [ _e = true ]
                    >   -(
                              tok.Foci_
                          >   (
                                   '[' > +focus_type [ push_back(_f, _1) ] > ']'
                               |   focus_type [ push_back(_f, _1) ]
                              )
                         )
                    >   -(
                              tok.EffectsGroups_
                          >   parse::detail::effects_group_parser() [ _g = _1 ]
                         )
                    >   -(
                              tok.Environments_
                          >   environment_map [ _h = _1 ]
                         )
                    >    parse::label(Graphic_name) > tok.string [ insert_species(_r1, new_<Species>(_a, _b, _f, _h, _g, _c, _d, _e, _1)) ]
                    ;

                start
                    =   +species(_r1)
                    ;

                qi::on_error<qi::fail>(start, parse::report_error(_1, _2, _3, _4));
            }

        typedef boost::spirit::qi::rule<
            parse::token_iterator,
            FocusType (),
            qi::locals<
                std::string,
                std::string,
                Condition::ConditionBase*
            >,
            parse::skipper_type
        > focus_type_rule;

        typedef boost::spirit::qi::rule<
            parse::token_iterator,
            std::pair<PlanetType, PlanetEnvironment> (),
            qi::locals<PlanetType>,
            parse::skipper_type
        > environment_map_element_rule;

        typedef boost::spirit::qi::rule<
            parse::token_iterator,
            std::map<PlanetType, PlanetEnvironment> (),
            parse::skipper_type
        > environment_map_rule;

        typedef boost::spirit::qi::rule<
            parse::token_iterator,
            void (std::map<std::string, Species*>&),
            qi::locals<
                std::string,
                std::string,
                bool,
                bool,
                bool,
                std::vector<FocusType>,
                std::vector<boost::shared_ptr<const Effect::EffectsGroup> >,
                std::map<PlanetType, PlanetEnvironment>
            >,
            parse::skipper_type
        > species_rule;

        typedef boost::spirit::qi::rule<
            parse::token_iterator,
            void (std::map<std::string, Species*>&),
            parse::skipper_type
        > start_rule;

        focus_type_rule focus_type;
        environment_map_element_rule environment_map_element;
        environment_map_rule environment_map;
        species_rule species;
        start_rule start;
    };

}

namespace parse {

    bool species(const boost::filesystem::path& path, std::map<std::string, Species*>& species_)
    { return detail::parse_file<rules, std::map<std::string, Species*> >(path, species_); }

}
