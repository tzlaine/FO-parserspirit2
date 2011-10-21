// -*- C++ -*-
#ifndef _ParseImpl_h_
#define _ParseImpl_h_

#include "ConditionParserImpl.h"
#include "EnumParser.h"
#include "Parse.h"
#include "../util/MultiplayerCommon.h"

#include <boost/filesystem/fstream.hpp>
#include <boost/spirit/home/phoenix.hpp>
#include <boost/spirit/include/qi.hpp>


namespace qi = boost::spirit::qi;
namespace phoenix = boost::phoenix;

namespace parse { namespace detail {

    typedef boost::spirit::qi::rule<
        token_iterator,
        std::vector<boost::shared_ptr<const Effect::EffectsGroup> > (),
        skipper_type
    > effects_group_rule;

    effects_group_rule& effects_group_parser();

    void parse_file_common(const boost::filesystem::path& path,
                           const lexer& l,
                           text_iterator& first,
                           token_iterator& it);

    template <typename Rules, typename Arg1>
    void parse_file(const boost::filesystem::path& path, Arg1& arg1)
    {
        text_iterator first;
        token_iterator it;

        const lexer& l = lexer::instance();

        parse_file_common(path, l, first, it);

        boost::spirit::qi::in_state_type in_state;
        using boost::phoenix::ref;

        static Rules rules;

        boost::spirit::qi::phrase_parse(it, l.end(), rules.start(ref(arg1)), in_state("WS")[l.self]);
    }

    template <typename Rules, typename Arg1, typename Arg2>
    void parse_file(const boost::filesystem::path& path, Arg1& arg1, Arg2& arg2)
    {
        text_iterator first;
        token_iterator it;

        const lexer& l = lexer::instance();

        parse_file_common(path, l, first, it);

        boost::spirit::qi::in_state_type in_state;
        using boost::phoenix::ref;

        static Rules rules;

        boost::spirit::qi::phrase_parse(it, l.end(), rules.start(ref(arg1), ref(arg2)), in_state("WS")[l.self]);
    }

} }

#endif
