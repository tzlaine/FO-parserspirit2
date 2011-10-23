// -*- C++ -*-
#ifndef _Lexer_h_
#define _Lexer_h_

#define BOOST_SPIRIT_NO_PREDEFINED_TERMINALS

#include <GG/ReportParseError.h>

#include "../universe/Enums.h"
#include "../universe/Names.h"
#include "../universe/ValueRefFwd.h"


/** \namespace parse \brief The namespace that encloses the script file lexer
    and parser. */
namespace parse {

/** The type of iterator used by the script file lexer. */
typedef boost::spirit::line_pos_iterator<std::string::const_iterator> text_iterator;

/** The type of token used by the script file lexer. */
typedef boost::spirit::lex::lexertl::token<
    text_iterator,
    boost::mpl::vector<
        bool,
        int,
        double,
        PlanetSize,
        PlanetType,
        PlanetEnvironment,
        UniverseObjectType,
        StarType,
        EmpireAffiliationType,
        UnlockableItemType,
        TechType,
        CombatFighterType,
        ShipPartClass,
        ShipSlotType,
        CaptureResult,
        ValueRef::StatisticType,
        adobe::name_t,
        std::string
    >
> token_type;

typedef boost::spirit::lex::lexertl::actor_lexer<token_type> spirit_lexer_base_type;

/** The script file lexer. */
struct lexer :
    boost::spirit::lex::lexer<spirit_lexer_base_type>
{
    static const lexer& instance();

    /** \name Comment tokens */ ///@{
    boost::spirit::lex::token_def<boost::spirit::lex::omit> inline_comment;
    boost::spirit::lex::token_def<boost::spirit::lex::omit> end_of_line_comment;
    //@}

    /** \name Tokens for common C++ types and builtins. */ ///@{
    boost::spirit::lex::token_def<bool> bool_;
    boost::spirit::lex::token_def<int> int_;
    boost::spirit::lex::token_def<double> double_;
    boost::spirit::lex::token_def<std::string> string;
    //@}

    /** \name Enum tokens.  Note that some values are left out of some
        enumerations, because they are also used elsewhere.  This implies that
        1) parsers for these types should use the tokens below, plus any
        keyword tokens that belong with that enumeration; and that 2) when you
        add a new enumerator to an enumeration, you need to check that it is
        not also a keyword as well. */ ///@{
    boost::spirit::lex::token_def<PlanetSize> planet_size_enum;
    boost::spirit::lex::token_def<PlanetType> planet_type_enum;
    boost::spirit::lex::token_def<PlanetEnvironment> planet_environment_enum;
    boost::spirit::lex::token_def<UniverseObjectType> universe_object_type_enum;
    boost::spirit::lex::token_def<StarType> star_type_enum;
    boost::spirit::lex::token_def<EmpireAffiliationType> empire_affiliation_type_enum;
    boost::spirit::lex::token_def<UnlockableItemType> unlockable_item_type_enum;
    boost::spirit::lex::token_def<TechType> tech_type_enum;
    boost::spirit::lex::token_def<CombatFighterType> combat_fighter_type_enum;
    boost::spirit::lex::token_def<ShipPartClass> ship_part_class_enum;
    boost::spirit::lex::token_def<ShipSlotType> ship_slot_type_enum;
    boost::spirit::lex::token_def<CaptureResult> capture_result_enum;
    boost::spirit::lex::token_def<ValueRef::StatisticType> statistic_type_enum;
    //@}

    /** \name Keyword tokens.  These should be kept in lexicographically
        sorted order, so that finding, adding, and removing tokens is a bit
        easier.  See the note above the Enum tokens section. */ ///@{
#define NAME_TOKEN(r, _, name) boost::spirit::lex::token_def<adobe::name_t> BOOST_PP_CAT(name, _);
    BOOST_PP_SEQ_FOR_EACH(NAME_TOKEN, _, NAMES_SEQ)
#undef NAME_TOKEN
    //@}

    /** \name Error token. */ ///@{
    boost::spirit::lex::token_def<boost::spirit::lex::omit> error_token;
    //@}

    /** Returns the token_def<adobe::name_t> associated with \a name. */
    const boost::spirit::lex::token_def<adobe::name_t>& name_token(adobe::name_t name) const;

private:
    /** Ctor. */
    lexer();

    std::map<adobe::name_t, boost::spirit::lex::token_def<adobe::name_t>*> m_name_tokens;
};

/** The type of iterator passed to the script file parser by the script file
    lexer. */
typedef lexer::iterator_type token_iterator;

typedef lexer::lexer_def lexer_def;

/** The type of the skip-parser, defined in the script file lexer, used by the
    script file parser iterator. */
typedef boost::spirit::qi::in_state_skipper<lexer_def> skipper_type;

extern const boost::phoenix::function<GG::report_error_<token_type> > report_error;

}

namespace boost { namespace spirit { namespace traits {

    // If you want to create a token with a custom value type, you musr
    // declare the conversion handler here, and define it in the .cpp file.

#define ASSIGN_TO_ATTRIBUTE_DECL(T)                                          \
    template <>                                                              \
    struct assign_to_attribute_from_iterators<T, parse::text_iterator, void> \
    { static void call(const parse::text_iterator& first, const parse::text_iterator& last, T& attr); }

    // These template specializations are required by Spirit.Lex to automatically
    // convert an iterator pair to an adobe::name_t in the lexer.
    ASSIGN_TO_ATTRIBUTE_DECL(adobe::name_t);

    // HACK! This is only necessary because of a bug in Spirit in Boost
    // versions <= 1.45.
    ASSIGN_TO_ATTRIBUTE_DECL(bool);

    ASSIGN_TO_ATTRIBUTE_DECL(PlanetSize);
    ASSIGN_TO_ATTRIBUTE_DECL(PlanetType);
    ASSIGN_TO_ATTRIBUTE_DECL(PlanetEnvironment);
    ASSIGN_TO_ATTRIBUTE_DECL(UniverseObjectType);
    ASSIGN_TO_ATTRIBUTE_DECL(StarType);
    ASSIGN_TO_ATTRIBUTE_DECL(EmpireAffiliationType);
    ASSIGN_TO_ATTRIBUTE_DECL(UnlockableItemType);
    ASSIGN_TO_ATTRIBUTE_DECL(TechType);
    ASSIGN_TO_ATTRIBUTE_DECL(CombatFighterType);
    ASSIGN_TO_ATTRIBUTE_DECL(ShipPartClass);
    ASSIGN_TO_ATTRIBUTE_DECL(ShipSlotType);
    ASSIGN_TO_ATTRIBUTE_DECL(CaptureResult);
    ASSIGN_TO_ATTRIBUTE_DECL(ValueRef::StatisticType);

#undef ASSIGN_TO_ATTRIBUTE_DECL

} } }

#endif
