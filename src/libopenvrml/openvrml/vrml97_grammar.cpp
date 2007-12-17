// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML
//
// Copyright 2007  Braden McDaniel
//
// This library is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation; either version 3 of the License, or (at
// your option) any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
// License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, see <http://www.gnu.org/licenses/>.
//

/**
 * @file openvrml/vrml97_grammar.h
 *
 * @brief VRML97 Spirit grammar and associated parsers.
 */

/**
 * @struct openvrml::vrml97_space_parser openvrml/vrml97_grammar.h
 *
 * @brief A <a href="http://spirit.sourceforge.net">Spirit</a> space parser
 *        appropriate for use with VRML97 and VRML-encoded X3D.
 */

/**
 * @typedef openvrml::vrm97_space_parser openvrml::vrml97_space_parser::self_t
 *
 * @brief Self type.
 */

/**
 * @fn bool openvrml::vrml97_space_parser::test(const CharT &) const
 *
 * @brief Test for a match.
 *
 * @param[in] c a character.
 *
 * @return @c true for whitespace characters or comma (&ldquo;,&rdquo;);
 *         @c false otherwise.
 */

/**
 * @var const openvrml::vrml97_space_parser openvrml::vrml97_space_p
 *
 * @brief A <a href="http://spirit.sourceforge.net">Spirit</a> space parser
 *        appropriate for use with VRML97 and VRML-encoded X3D.
 */


/**
 * @struct openvrml::vrml97_skip_grammar openvrml/vrml97_grammar.h
 *
 * @brief A <a href="http://spirit.sourceforge.net">Spirit</a> grammar
 *        appropriate for use as a skip parser for VRML97 and VRML-encoded
 *        X3D.
 */

/**
 * @internal
 *
 * @struct openvrml::vrml97_skip_grammar::definition openvrml/vrml97_grammar.h
 *
 * @brief @c vrml97_skip_grammar definition.
 */

/**
 * @typedef boost::spirit::rule<ScannerT> openvrml::vrml97_skip_grammar::definition<ScannerT>::rule_t
 *
 * @brief Rule type.
 */

/**
 * @var openvrml::vrml97_skip_grammar::definition<ScannerT>::rule_t openvrml::vrml97_skip_grammar::definition<ScannerT>::skip
 *
 * @brief The skip rule.
 */

/**
 * @fn openvrml::vrml97_skip_grammar::definition<ScannerT>::definition(const vrml97_skip_grammar &)
 *
 * @brief Construct.
 *
 * @param[in] self  a reference to the grammar.
 */

/**
 * @fn openvrml::vrml97_skip_grammar::definition<ScannerT>::rule_t openvrml::vrml97_skip_grammar::definition<ScannerT>::start() const
 *
 * @brief The start rule.
 *
 * @return the start rule.
 */


/**
 * @enum openvrml::vrml_parse_error
 *
 * @brief Identifiers used to designate parse error conditions.
 */

/**
 * @var openvrml::vrml_parse_error openvrml::dot_expected
 *
 * @brief A &ldquo;.&rdquo; was expected.
 */

/**
 * @var openvrml::vrml_parse_error openvrml::lbrace_expected
 *
 * @brief A &ldquo;{&rdquo; was expected.
 */

/**
 * @var openvrml::vrml_parse_error openvrml::lbracket_expected
 *
 * @brief A &ldquo;[&rdquo; was expected.
 */

/**
 * @var openvrml::vrml_parse_error openvrml::id_expected
 *
 * @brief An identifier was expected.
 */

/**
 * @var openvrml::vrml_parse_error openvrml::to_expected
 *
 * @brief The @c TO keyword was expected.
 */

/**
 * @var openvrml::vrml_parse_error openvrml::is_expected
 *
 * @brief The @c IS keyword was expected.
 */

/**
 * @var openvrml::vrml_parse_error openvrml::interface_type_or_rbracket_expected
 *
 * @brief An interface type or &ldquo;]&rdquo; was expected.
 */

/**
 * @var openvrml::vrml_parse_error openvrml::field_type_expected
 *
 * @brief A field type was expected.
 */

/**
 * @var openvrml::vrml_parse_error openvrml::bool_expected
 *
 * @brief A keyword @c TRUE or @c FALSE was expected.
 */

/**
 * @var openvrml::vrml_parse_error openvrml::color_expected
 *
 * @brief A color value was expected.
 */

/**
 * @var openvrml::vrml_parse_error openvrml::color_or_lbracket_expected
 *
 * @brief A color value or &ldquo;[&rdquo; was expected.
 */

/**
 * @var openvrml::vrml_parse_error openvrml::color_or_rbracket_expected
 *
 * @brief A color value or &ldquo;]&rdquo; was expected.
 */

/**
 * @var openvrml::vrml_parse_error openvrml::float_expected
 *
 * @brief A floating-point value was expected.
 */

/**
 * @var openvrml::vrml_parse_error openvrml::float_or_lbracket_expected
 *
 * @brief A floating-point value or &ldquo;[&rdquo; was expected.
 */

/**
 * @var openvrml::vrml_parse_error openvrml::float_or_rbracket_expected
 *
 * @brief A floating-point value or &ldquo;]&rdquo; was expected.
 */

/**
 * @var openvrml::vrml_parse_error openvrml::int32_expected
 *
 * @brief A integer value was expected.
 */

/**
 * @var openvrml::vrml_parse_error openvrml::int32_or_lbracket_expected
 *
 * @brief A integer value or &ldquo;[&rdquo; was expected.
 */

/**
 * @var openvrml::vrml_parse_error openvrml::int32_or_rbracket_expected
 *
 * @brief A integer value or &ldquo;]&rdquo; was expected.
 */

/**
 * @var openvrml::vrml_parse_error openvrml::rotation_expected
 *
 * @brief A rotation value was expected.
 */

/**
 * @var openvrml::vrml_parse_error openvrml::rotation_or_lbracket_expected
 *
 * @brief A rotation value or &ldquo;[&rdquo; was expected.
 */

/**
 * @var openvrml::vrml_parse_error openvrml::rotation_or_rbracket_expected
 *
 * @brief A rotation value or &ldquo;]&rdquo; was expected.
 */

/**
 * @var openvrml::vrml_parse_error openvrml::string_expected
 *
 * @brief A string value was expected.
 */

/**
 * @var openvrml::vrml_parse_error openvrml::string_or_lbracket_expected
 *
 * @brief A string value or &ldquo;[&rdquo; was expected.
 */

/**
 * @var openvrml::vrml_parse_error openvrml::string_or_rbracket_expected
 *
 * @brief A string value or &ldquo;]&rdquo; was expected.
 */

/**
 * @var openvrml::vrml_parse_error openvrml::vec2_expected
 *
 * @brief A 2-component vector value was expected.
 */

/**
 * @var openvrml::vrml_parse_error openvrml::vec2_or_lbracket_expected
 *
 * @brief A 2-component vector value or &ldquo;[&rdquo; was expected.
 */

/**
 * @var openvrml::vrml_parse_error openvrml::vec2_or_rbracket_expected
 *
 * @brief A 2-component vector value or &ldquo;]&rdquo; was expected.
 */

/**
 * @var openvrml::vrml_parse_error openvrml::vec3_expected
 *
 * @brief A 3-component vector value was expected.
 */

/**
 * @var openvrml::vrml_parse_error openvrml::vec3_or_lbracket_expected
 *
 * @brief A 3-component vector value or &ldquo;[&rdquo; was expected.
 */

/**
 * @var openvrml::vrml_parse_error openvrml::vec3_or_rbracket_expected
 *
 * @brief A 3-component vector value or &ldquo;]&rdquo; was expected.
 */

/**
 * @var openvrml::vrml_parse_error openvrml::node_expected
 *
 * @brief A node was expected.
 */

/**
 * @var openvrml::vrml_parse_error openvrml::node_or_lbracket_expected
 *
 * @brief A node or &ldquo;[&rdquo; was expected.
 */

/**
 * @var openvrml::vrml_parse_error openvrml::node_or_rbracket_expected
 *
 * @brief A node or &ldquo;]&rdquo; was expected.
 */

/**
 * @var openvrml::vrml_parse_error openvrml::interface_collision
 *
 * @brief An interface conflicts with one that was previously declared.
 */

/**
 * @var openvrml::vrml_parse_error openvrml::unknown_node_type_id
 *
 * @brief Encountered an unknown node type identifier.
 */

/**
 * @fn const char * openvrml::vrml97_parse_error_msg(vrml_parse_error error)
 *
 * @brief Get the error message associated with a @c vrml_parse_error.
 *
 * @param[in] error error identifier.
 *
 * @return the error message associated with @p error.
 */

/**
 * @struct openvrml::vrml97_parse_error_handler openvrml/vrml97_grammar.h
 *
 * @brief A Spirit error handler that emits error and warning messages
 *        to a @c std::ostream.
 *
 * @c rotation_axis_not_normalized is treated as a warning.  All other
 * @c vrml_parse_error%s are treated as errors.
 */

/**
 * @internal
 *
 * @var std::ostream & openvrml::vrml97_parse_error_handler::out_
 *
 * @brief Output stream.
 */

/**
 * @fn openvrml::vrml97_parse_error_handler::vrml97_parse_error_handler(std::ostream & out)
 *
 * @brief Construct.
 *
 * @param[in] out   output stream.
 */

/**
 * @fn boost::spirit::error_status<> openvrml::vrml97_parse_error_handler::operator()(const ScannerT & scan, const ErrorT & err) const
 *
 * @brief Handle a parser error.
 *
 * @param[in] scan  the scanner.
 * @param[in] err   a @c boost::spirit::parser_error.
 */

/**
 * @struct openvrml::null_vrml97_parse_actions openvrml/vrml97_grammar.h
 *
 * @brief No-op semantic actions for @c vrml97_grammar.
 *
 * This @c struct includes a no-op function for each semantic action
 * called by @c vrml97_grammar.  It is the default value of
 * @c vrml97_grammar%'s @c Actions template parameter.  It is generally
 * convenient to inherit this @c struct when providing your own semantic
 * actions, as doing so will provide a default no-op action where none has
 * been implemented or none needs to occur.
 */

/**
 * @struct openvrml::vrml97_grammar openvrml/vrml97_grammar.h
 *
 * @brief A <a href="http://spirit.sourceforge.net">Spirit</a> grammar for
 *        parsing VRML97.
 */

/**
 * @struct openvrml::vrml97_grammar::definition openvrml/vrml97_grammar.h
 *
 * @brief VRML97 grammar definition.
 */

/**
 * @example pretty_print.cpp
 *
 * A pretty-printer using @c openvrml::vrml97_grammar.
 */
