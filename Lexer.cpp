#include "Lexer.h"

#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <boost/spirit/home/phoenix.hpp>


namespace {

    // NOTE: The use of parse_into_strings() and find_match() is a bit
    // brittle.  The pair of them rely on the enum string listing the
    // enumerators in the same order that they are listed in the actual
    // enumeration's definition, with no gaps.  If this is not the case, a
    // custom mapping function must be used.  See
    // assign_to_attribute_from_iterators<UniverseObjectType, ...>::call()
    // near the end of the file for an example.

    std::vector<std::string> parse_into_strings(const std::string& string)
    {
        std::vector<std::string> retval;
        std::string strings(string.begin() + 4, string.end() - 1);
        boost::algorithm::split(retval, strings, boost::algorithm::is_any_of("|"));
        return retval;
    }

    long find_match(const std::vector<std::string>& strings, const parse::text_iterator& first, const parse::text_iterator& last)
    {
        long retval = -1;
        std::string match(first, last);
        boost::algorithm::to_lower(match);
        std::vector<std::string>::const_iterator it = std::find(strings.begin(), strings.end(), match);
        if (it != strings.end())
            retval = it - strings.begin();
        return retval;
    }

    const std::string planet_size_str("(?i:tiny|small|medium|large|huge)");
    const PlanetSize first_planet_size = SZ_TINY;
    const std::string planet_type_str("(?i:swamp|toxic|inferno|radiated|barren|tundra|desert|terran|ocean)");
    const PlanetType first_planet_type = PT_SWAMP;
    const std::string planet_environment_str("(?i:uninhabitable|hostile|poor|adequate|good)");
    const PlanetEnvironment first_planet_environment = PE_UNINHABITABLE;
    const std::string universe_object_type_str("(?i:ship|populationcenter|productioncenter)");
    const std::string star_type_str("(?i:blue|white|yellow|orange|red|neutron|blackhole|nostar)");
    const StarType first_star_type = STAR_BLUE;
    const std::string empire_affiliation_type_str("(?i:theempire|enemyof|allyof|anyempire)");
    const EmpireAffiliationType first_empire_affiliation_type = AFFIL_SELF;
    const std::string unlockable_item_type_str("(?i:shippart|shiphull)");
    const UnlockableItemType first_unlockable_item_type = UIT_SHIP_PART;
    const std::string tech_type_str("(?i:theory|application|refinement)");
    const TechType first_tech_type = TT_THEORY;
    const std::string combat_fighter_type_str("(?i:interceptor|bomber)");
    const CombatFighterType first_combat_fighter_type = INTERCEPTOR;
    const std::string ship_part_class_str("(?i:shortrange|missiles|fighters|pointdefense|armour|colony)");
    const ShipPartClass first_ship_part_class = PC_SHORT_RANGE;
    const std::string ship_slot_type_str("(?i:external|internal)");
    const ShipSlotType first_ship_slot_type = SL_EXTERNAL;
    const std::string capture_result_str("(?i:capture|retain)");
    const CaptureResult first_capture_result = CR_CAPTURE;
    const std::string statistic_type_str("(?i:count|sum|mean|rms|max|min|spread|stdev|product)");

    struct strip_quotes_
    {
        template <typename Arg1, typename Arg2>
        struct result
        { typedef std::string type; };

        std::string operator()(const parse::text_iterator& start, const parse::text_iterator& end) const
            {
                std::string::const_iterator start_ = start.base();
                std::string::const_iterator end_ = end.base();
                return std::string(++start_, --end_);
            }
    };
    const boost::phoenix::function<strip_quotes_> strip_quotes;
}

using namespace parse;

const char* lexer::bool_regex = "(?i:true|false)";
const char* lexer::int_regex = "\\d+";
const char* lexer::double_regex = "\\d+\\.\\d*";
const char* lexer::string_regex = "\\\"[^\\\"]*\\\"";

lexer::lexer() :
    inline_comment("\\/\\*[^*]*\\*+([^/*][^*]*\\*+)*\\/"),
    end_of_line_comment("\\/\\/.*$"),

    bool_(bool_regex),
    int_(int_regex),
    double_(double_regex),
    string(string_regex),

    planet_size_enum(planet_size_str.c_str()),
    planet_type_enum(planet_type_str.c_str()),
    planet_environment_enum(planet_environment_str.c_str()),
    universe_object_type_enum(universe_object_type_str.c_str()),
    star_type_enum(star_type_str.c_str()),
    empire_affiliation_type_enum(empire_affiliation_type_str.c_str()),
    unlockable_item_type_enum(unlockable_item_type_str.c_str()),
    tech_type_enum(tech_type_str.c_str()),
    combat_fighter_type_enum(combat_fighter_type_str.c_str()),
    ship_part_class_enum(ship_part_class_str.c_str()),
    ship_slot_type_enum(ship_slot_type_str.c_str()),
    capture_result_enum(capture_result_str.c_str()),
    statistic_type_enum(statistic_type_str.c_str()),

#define NAME_TOKEN(r, _, name) BOOST_PP_CAT(name, _)("(?i:" BOOST_PP_STRINGIZE(name) ")"),
    BOOST_PP_SEQ_FOR_EACH(NAME_TOKEN, _, NAMES_SEQ)
#undef NAME_TOKEN

    error_token("\\S+?")

    // TODO: Is Design used for ShipDesign? (if so, replace)
    // TODO: Get rid of underscore in Lookup_Strings.
    // TODO: Can we replace OwnedBy with Owner? (if so, replace)
    // TODO: Can PlanetEnvironment be replaced with Environment?
    // TODO: Get rid of underscore in Short_Description.
    // TODO: Size is used for PlanetSize (so replace).
{
    namespace lex = boost::spirit::lex;

    lex::_end_type _end;
    lex::_start_type _start;
    lex::_val_type _val;
    using boost::phoenix::bind;
    using boost::phoenix::construct;

    self
        =     inline_comment
        |     end_of_line_comment
        |     '='
        |     '+'
        |     '-'
        |     '*'
        |     '/'
        |     '.'
        |     ','
        |     '('
        |     ')'
        |     '['
        |     ']'
        ;

    self
        +=    bool_
        |     int_
        |     double_
        |     string [ _val = strip_quotes(_start, _end) ]
        ;

    self
        +=    planet_size_enum
        |     planet_type_enum
        |     planet_environment_enum
        |     universe_object_type_enum
        |     star_type_enum
        |     empire_affiliation_type_enum
        |     unlockable_item_type_enum
        |     tech_type_enum
        |     combat_fighter_type_enum
        |     ship_part_class_enum
        |     ship_slot_type_enum
        |     capture_result_enum
        |     statistic_type_enum
        ;

#define NAME_TOKEN(r, _, name)                                          \
    {                                                                   \
        adobe::name_t n(BOOST_PP_CAT(name, _name));                     \
        self += BOOST_PP_CAT(name, _) [ _val = n ];                     \
        m_name_tokens[n] = &BOOST_PP_CAT(name, _);                      \
    }
    BOOST_PP_SEQ_FOR_EACH(NAME_TOKEN, _, NAMES_SEQ)
#undef NAME_TOKEN

    self
        +=    error_token
        ;

    self("WS") = lex::token_def<>("\\s+");
}

const lexer& lexer::instance()
{
    static const lexer retval;
    return retval;
}

const boost::spirit::lex::token_def<adobe::name_t>& lexer::name_token(adobe::name_t name) const
{
    std::map<adobe::name_t, boost::spirit::lex::token_def<adobe::name_t>*>::const_iterator it = m_name_tokens.find(name);
    assert(it != m_name_tokens.end());
    return *it->second;
}

const boost::phoenix::function<GG::report_error_<parse::token_type> > parse::report_error;

namespace boost { namespace spirit { namespace traits {

    // This template specialization is required by Spirit.Lex to automatically
    // convert an iterator pair to an adobe::name_t in the lexer.
    void assign_to_attribute_from_iterators<adobe::name_t, parse::text_iterator, void>::
    call(const parse::text_iterator& first, const parse::text_iterator& last, adobe::name_t& attr)
    {
        std::string str(first, last);
        boost::algorithm::to_lower(str);
        attr = adobe::name_t(str.c_str());
    }

    void assign_to_attribute_from_iterators<bool, parse::text_iterator, void>::
    call(const parse::text_iterator& first, const parse::text_iterator& last, bool& attr)
    { attr = *first == 't' || *first == 'T' ? true : false; }

#define ASSIGN_TO_ATTRIBUTE_DEF(T, name)                                                \
    void assign_to_attribute_from_iterators<T, parse::text_iterator, void>::            \
    call(const parse::text_iterator& first, const parse::text_iterator& last, T& attr)  \
    {                                                                                   \
        static const std::vector<std::string> strings = parse_into_strings(name##_str); \
        long position = find_match(strings, first, last);                               \
        if (position != -1)                                                             \
            position += first_##name;                                                   \
        attr = static_cast<T>(position);                                                \
    }

    ASSIGN_TO_ATTRIBUTE_DEF(PlanetSize, planet_size);
    ASSIGN_TO_ATTRIBUTE_DEF(PlanetType, planet_type);
    ASSIGN_TO_ATTRIBUTE_DEF(PlanetEnvironment, planet_environment);

    void assign_to_attribute_from_iterators<UniverseObjectType, parse::text_iterator, void>::
    call(const parse::text_iterator& first, const parse::text_iterator& last, UniverseObjectType& attr)
    {
        typedef std::map<std::string, UniverseObjectType> MapType;
        static const MapType strings = boost::assign::list_of<std::pair<std::string, UniverseObjectType> >
            ("building", OBJ_BUILDING)
            ("ship", OBJ_SHIP)
            ("fleet", OBJ_FLEET)
            ("planet", OBJ_PLANET)
            ("populationcenter", OBJ_POP_CENTER)
            ("productioncenter", OBJ_PROD_CENTER)
            ("system", OBJ_SYSTEM)
            ;
        std::string match(first, last);
        boost::algorithm::to_lower(match);
        MapType::const_iterator it = strings.find(match);
        if (it == strings.end())
            attr = INVALID_UNIVERSE_OBJECT_TYPE;
        else
            attr = it->second;
    }

    ASSIGN_TO_ATTRIBUTE_DEF(StarType, star_type);
    ASSIGN_TO_ATTRIBUTE_DEF(EmpireAffiliationType, empire_affiliation_type);
    ASSIGN_TO_ATTRIBUTE_DEF(UnlockableItemType, unlockable_item_type);
    ASSIGN_TO_ATTRIBUTE_DEF(TechType, tech_type);
    ASSIGN_TO_ATTRIBUTE_DEF(CombatFighterType, combat_fighter_type);

    void assign_to_attribute_from_iterators<ShipPartClass, parse::text_iterator, void>::
    call(const parse::text_iterator& first, const parse::text_iterator& last, ShipPartClass& attr)
    {
        typedef std::map<std::string, ShipPartClass> MapType;
        static const MapType strings = boost::assign::list_of<std::pair<std::string, ShipPartClass> >
            ("shortrange", PC_SHORT_RANGE)
            ("missiles", PC_MISSILES)
            ("fighters", PC_FIGHTERS)
            ("pointdefense", PC_POINT_DEFENSE)
            ("shield", PC_SHIELD)
            ("armour", PC_ARMOUR)
            ("troops", PC_TROOPS)
            ("detection", PC_DETECTION)
            ("stealth", PC_STEALTH)
            ("fuel", PC_FUEL)
            ("colony", PC_COLONY)
            ("battlespeed", PC_BATTLE_SPEED)
            ("starlanespeed", PC_STARLANE_SPEED)
            ;
        std::string match(first, last);
        boost::algorithm::to_lower(match);
        MapType::const_iterator it = strings.find(match);
        if (it == strings.end())
            attr = INVALID_SHIP_PART_CLASS;
        else
            attr = it->second;
    }

    ASSIGN_TO_ATTRIBUTE_DEF(ShipSlotType, ship_slot_type);
    ASSIGN_TO_ATTRIBUTE_DEF(CaptureResult, capture_result);

    void assign_to_attribute_from_iterators<ValueRef::StatisticType, parse::text_iterator, void>::
    call(const parse::text_iterator& first, const parse::text_iterator& last, ValueRef::StatisticType& attr)
    {
        typedef std::map<std::string, ValueRef::StatisticType> MapType;
        static const MapType strings = boost::assign::list_of<std::pair<std::string, ValueRef::StatisticType> >
            ("count", ValueRef::COUNT)
            ("sum", ValueRef::SUM)
            ("mean", ValueRef::MEAN)
            ("rms", ValueRef::RMS)
            ("mode", ValueRef::MODE)
            ("max", ValueRef::MAX)
            ("min", ValueRef::MIN)
            ("spread", ValueRef::SPREAD)
            ("stdev", ValueRef::STDEV)
            ("product", ValueRef::PRODUCT)
            ;
        std::string match(first, last);
        boost::algorithm::to_lower(match);
        MapType::const_iterator it = strings.find(match);
        if (it == strings.end())
            attr = ValueRef::INVALID_STATISTIC_TYPE;
        else
            attr = it->second;
    }

#undef ASSIGN_TO_ATTRIBUTE_DEF

} } }
