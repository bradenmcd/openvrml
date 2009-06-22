// -*- mode: c++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML
//
// Copyright 2009  Braden McDaniel
//
// This library is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation; either version 3 of the License, or (at your
// option) any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
// License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, see <http://www.gnu.org/licenses/>.
//

# include "parse_vrml.h"
# include <openvrml/x3d_vrml_grammar.h>
# include <boost/algorithm/string/predicate.hpp>

bool openvrml::local::anonymous_stream_id(const openvrml::local::uri & id)
{
    const std::string str(id);
    return str.length() > anonymous_stream_id_prefix.length()
        && std::equal(anonymous_stream_id_prefix.begin(),
                      anonymous_stream_id_prefix.end(),
                      str.begin());
}

namespace {

    struct OPENVRML_LOCAL parse_error {
        size_t line, column;
        std::string message;

        parse_error(): line(0), column(0) {}
    };

    struct OPENVRML_LOCAL error_handler {
        error_handler(openvrml::browser & b, parse_error & error):
            browser_(b),
            error_(error)
        {}

        template <typename ScannerT, typename ErrorT>
        boost::spirit::classic::error_status<> operator()(const ScannerT & scan,
                                                 const ErrorT & err) const
        {
            using std::endl;
            using std::string;
            using boost::spirit::classic::error_status;
            using boost::spirit::classic::file_position;

            const file_position fpos = err.where.get_position();

            //
            // Warnings we want to spew directly to the browser.
            //
            if (   err.descriptor == openvrml::rotation_axis_not_normalized
                || err.descriptor == openvrml::eventin_deprecated
                || err.descriptor == openvrml::eventout_deprecated
                || err.descriptor == openvrml::exposedfield_deprecated
                || err.descriptor == openvrml::field_deprecated) {
                std::ostringstream warn;
                warn << fpos.file << ':' << fpos.line << ':' << fpos.column
                     << ": warning: "
                     << x3d_vrml_parse_error_msg(err.descriptor);
                this->browser_.err(warn.str());
                scan.first = err.where;
                return error_status<>(error_status<>::accept, 0);
            }

            //
            // Errors presumably halt parsing and result in a failure.  We
            // need enough information to create an invalid_vrml exception in
            // parse_vrml.
            //
            this->error_.line = fpos.line;
            this->error_.column = fpos.column;
            this->error_.message = x3d_vrml_parse_error_msg(err.descriptor);

            return error_status<>(error_status<>::fail);
        }

    private:
        openvrml::browser & browser_;
        parse_error & error_;
    };
}

/**
 * @internal
 *
 * @brief Parse a VRML stream.
 *
 * @param[in,out] in    input stream.
 * @param[in]     uri   URI associated with @p in.
 * @param[in]     type  MIME media type of the data to be read from @p in.
 * @param[in]     scene a @c scene.
 * @param[out]    nodes the root @c node%s.
 * @param[out]    meta  the @c scene metadata.
 *
 * @exception openvrml::bad_media_type
 * @exception openvrml::invalid_vrml
 */
void
openvrml::local::
parse_vrml(std::istream & in,
           const std::string & uri,
           const std::string & type,
           const openvrml::scene & scene,
           std::vector<boost::intrusive_ptr<openvrml::node> > & nodes,
           std::map<std::string, std::string> & meta)
{
    using std::istream;
    using std::istreambuf_iterator;
    using boost::algorithm::iequals;
    using boost::spirit::classic::multi_pass;
    using boost::spirit::classic::make_multi_pass;
    using boost::spirit::classic::position_iterator;

    typedef multi_pass<istreambuf_iterator<char> > multi_pass_iterator_t;
    typedef istream::char_type char_t;

    vrml97_skip_grammar skip_g;

    if (iequals(type, vrml_media_type) || iequals(type, x_vrml_media_type)) {
        multi_pass_iterator_t
            in_begin(make_multi_pass(istreambuf_iterator<char_t>(in))),
            in_end(make_multi_pass(istreambuf_iterator<char_t>()));

        typedef position_iterator<multi_pass_iterator_t> iterator_t;

        iterator_t first(in_begin, in_end, uri), last;

        parse_error error;
        error_handler handler(scene.browser(), error);
        vrml97_parse_actions actions(uri, scene, nodes);
        vrml97_grammar<vrml97_parse_actions, error_handler>
            g(actions, handler);

        BOOST_SPIRIT_DEBUG_NODE(skip_g);
        BOOST_SPIRIT_DEBUG_NODE(g);

        if (!parse(first, last, g, skip_g).full) {
            throw openvrml::invalid_vrml(uri,
                                         error.line,
                                         error.column,
                                         error.message);
        }
    } else if (iequals(type, x3d_vrml_media_type)) {
        multi_pass_iterator_t
            in_begin(make_multi_pass(istreambuf_iterator<char_t>(in))),
            in_end(make_multi_pass(istreambuf_iterator<char_t>()));

        typedef position_iterator<multi_pass_iterator_t> iterator_t;

        iterator_t first(in_begin, in_end, uri), last;

        parse_error error;
        error_handler handler(scene.browser(), error);
        x3d_vrml_parse_actions actions(uri, scene, nodes, meta);
        x3d_vrml_grammar<x3d_vrml_parse_actions, error_handler>
            g(actions, handler);

        BOOST_SPIRIT_DEBUG_NODE(skip_g);
        BOOST_SPIRIT_DEBUG_NODE(g);

        if (!parse(first, last, g, skip_g).full) {
            throw openvrml::invalid_vrml(uri,
                                         error.line,
                                         error.column,
                                         error.message);
        }
    } else {
        throw bad_media_type(type);
    }
}
