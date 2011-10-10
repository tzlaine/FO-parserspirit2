#include "test.h"


lexer_test_rules::lexer_test_rules()
{
    namespace qi = boost::spirit::qi;
    namespace phoenix = boost::phoenix;
    qi::_1_type _1;
    using phoenix::val;

    const parse::lexer& tok = parse::lexer::instance();

    lexer
        =   * (
                   tok.inline_comment [ std::cout << val("<INLINE COMMENT>\n") ]
                |  tok.end_of_line_comment [ std::cout << val("<END OF LINE COMMENT>\n") ]

                |  tok.bool_ [ std::cout << _1 << " BOOL\n" ]
                |  tok.int_ [ std::cout << _1 << " INT\n" ]
                |  tok.double_ [ std::cout << _1 << " DOUBLE\n" ]
                |  tok.string [ std::cout << _1 << " STRING\n" ]

                |  tok.planet_size_enum [ std::cout << _1 << " PLANET_SIZE\n" ]
                |  tok.planet_type_enum [ std::cout << _1 << " PLANET_TYPE\n" ]
                |  tok.planet_environment_enum [ std::cout << _1 << " PLANET_ENVIRONMENT\n" ]
                |  tok.universe_object_type_enum [ std::cout << _1 << " UNIVERSE_OBJECT_TYPE\n" ]
                |  tok.star_type_enum [ std::cout << _1 << " STAR_TYPE\n" ]
                |  tok.empire_affiliation_type_enum [ std::cout << _1 << " EMPIRE_AFFILIATION_TYPE\n" ]
                |  tok.unlockable_item_type_enum [ std::cout << _1 << " UNLOCKABLE_ITEM_TYPE\n" ]
                |  tok.tech_type_enum [ std::cout << _1 << " TECH_TYPE\n" ]
                |  tok.combat_fighter_type_enum [ std::cout << _1 << " COMBAT_FIGHTER_TYPE\n" ]
                |  tok.ship_part_class_enum [ std::cout << _1 << " SHIP_PART_CLASS\n" ]
                |  tok.ship_slot_type_enum [ std::cout << _1 << " SHIP_SLOT_TYPE\n" ]
                |  tok.capture_result_enum [ std::cout << _1 << " CAPTURE_RESULT\n" ]
                |  tok.statistic_type_enum [ std::cout << _1 << " STATISTIC_TYPE\n" ]

#define NAME_TOKEN(r, _, name) | tok.BOOST_PP_CAT(name, _) [ std::cout << _1 << "\n" ]
                   BOOST_PP_SEQ_FOR_EACH(NAME_TOKEN, _, NAMES_SEQ)
#undef NAME_TOKEN

                |  '='
                |  '+'
                |  '-'
                |  '*'
                |  '/'
                |  '.'
                |  ','
                |  '('
                |  ')'
                |  '['
                |  ']'
              )
        ;

#define NAME(x)
// x.name(#x); debug(x)
    NAME(lexer);
#undef NAME
}
