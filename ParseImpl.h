// -*- C++ -*-
#ifndef _ParseImpl_h_
#define _ParseImpl_h_

#include "ConditionParserImpl.h"
#include "EnumParser.h"
#include "../util/MultiplayerCommon.h"

#include <boost/filesystem/fstream.hpp>
#include <boost/spirit/home/phoenix.hpp>
#include <boost/spirit/include/qi.hpp>


namespace qi = boost::spirit::qi;
namespace phoenix = boost::phoenix;

template <typename Rules, typename Data>
void parse_file(const boost::filesystem::path& path, Data& data)
{
    const std::string filename = path.string();

    std::string file_contents;
    {
        boost::filesystem::ifstream ifs(path);
        if (ifs) {
            std::getline(ifs, file_contents, '\0');
        } else {
            Logger().errorStream() << "Unable to open data file " << filename;
            return;
        }
    }

    parse::text_iterator first(file_contents.begin());
    const parse::text_iterator last(file_contents.end());

    const parse::lexer& l = parse::lexer::instance();

    GG::detail::s_begin = first;
    GG::detail::s_end = last;
    GG::detail::s_filename = filename.c_str();
    parse::token_iterator it = l.begin(first, last);
    const parse::token_iterator end_it = l.end();

    boost::spirit::qi::in_state_type in_state;

    static Rules rules;

    boost::spirit::qi::phrase_parse(it, end_it, rules.start, in_state("WS")[l.self], data);
}

#endif
