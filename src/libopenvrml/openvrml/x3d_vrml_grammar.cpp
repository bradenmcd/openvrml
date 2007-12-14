// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
/**
 * @file openvrml/x3d_vrml_grammar.h
 *
 * @brief Classic VRML X3D Spirit grammar.
 */

/**
 * @var const openvrml::vrml_parse_error openvrml::profile_expected
 *
 * @brief The @c PROFILE keyword was expected.
 */

/**
 * @var const openvrml::vrml_parse_error openvrml::unrecognized_profile_id
 *
 * @brief The profile identifier was not recognized as a supported profile.
 */

/**
 * @var const openvrml::vrml_parse_error openvrml::unrecognized_component_id_or_level
 *
 * @brief The component identifier was unrecognized or the component level is
 *        unsupported.
 */

/**
 * @var const openvrml::vrml_parse_error openvrml::colon_expected
 *
 * @brief A &ldquo;:&rdquo; was expected.
 */

/**
 * @var const openvrml::vrml_parse_error openvrml::bool_or_lbracket_expected
 *
 * @brief A boolean value (i.e., @c TRUE or @c FALSE) or &ldquo;[&rdquo; was
 *        expected.
 */

/**
 * @var const openvrml::vrml_parse_error openvrml::bool_or_rbracket_expected
 *
 * @brief A boolean value (i.e., @c TRUE or @c FALSE) or &ldquo;]&rdquo; was
 *        expected.
 */

/**
 * @var const openvrml::vrml_parse_error openvrml::color_rgba_expected
 *
 * @brief An RGBA %color value was expected.
 */

/**
 * @var const openvrml::vrml_parse_error openvrml::color_rgba_or_lbracket_expected
 *
 * @brief An RGBA %color value or &ldquo;[&rdquo; was expected.
 */

/**
 * @var const openvrml::vrml_parse_error openvrml::color_rgba_or_rbracket_expected
 *
 * @brief An RGBA %color value or &ldquo;]&rdquo; was expected.
 */

/**
 * @var const openvrml::vrml_parse_error openvrml::eventin_deprecated
 *
 * @brief The @c eventIn keyword is deprecated.
 */

/**
 * @var const openvrml::vrml_parse_error openvrml::eventout_deprecated
 *
 * @brief The @c eventOut keyword is deprecated.
 */

/**
 * @var const openvrml::vrml_parse_error openvrml::exposedfield_deprecated
 *
 * @brief The @c exposedField keyword is deprecated.
 */

/**
 * @var const openvrml::vrml_parse_error openvrml::field_deprecated
 *
 * @brief The @c field keyword is deprecated.
 */

/**
 * @fn const char * openvrml::x3d_vrml_parse_error_msg(vrml_parse_error error)
 *
 * @brief Get the error messages associated with a @c vrml_parse_error.
 *
 * @param[in] error error identifier.
 *
 * @return the error messages associated with @p error.
 */

/**
 * @struct openvrml::x3d_vrml_parse_error_handler openvrml/x3d_vrml_grammar.h
 *
 * @brief A Spirit error handler that emits error and warning messages
 *        to a @c std::ostream.
 *
 * The following @c openvrml::vrml_parse_error%s are treated as warnings:
 *
 * - @c rotation_axis_not_normalized
 * - @c eventin_deprecated
 * - @c eventout_deprecated
 * - @c exposedfield_deprecated
 * - @c field_deprecated
 *
 * All other @c vrml_parse_error%s are treated as errors.
 */

/**
 * @fn openvrml::x3d_vrml_parse_error_handler::x3d_vrml_parse_error_handler(std::ostream & out)
 *
 * @brief Construct.
 *
 * @param[in] out   output stream.
 */

/**
 * @fn boost::spirit::error_status<> openvrml::x3d_vrml_parse_error_handler::operator()(const ScannerT & scan, const ErrorT & err) const
 *
 * @brief Handle a parser error.
 *
 * @param[in] scan  the scanner.
 * @param[in] err   a @c boost::spirit::parser_error.
 */

/**
 * @struct openvrml::color_rgba_parser openvrml/x3d_vrml_grammar.h
 *
 * @brief The implementation of the @c color_rgba_p functor parser.
 */

/**
 * @typedef openvrml::color_rgba openvrml::color_rgba_parser::result_t
 *
 * @brief Parse result.
 */

/**
 * @fn std::ptrdiff_t openvrml::color_rgba_parser::operator()(const ScannerT & scan, result_t & result) const
 *
 * @brief Parse an RGBA %color value.
 *
 * @param[in] scan      a scanner.
 * @param[out] result   the parse result.
 */

/**
 * @var const boost::spirit::functor_parser<openvrml::color_rgba_parser> openvrml::color_rgba_p
 *
 * @brief A Spirit parser for RGBA %color values.
 */

/**
 * @struct openvrml::vec2d_parser openvrml/x3d_vrml_grammar.h
 *
 *  @brief The implementation of the @c vec2d_p functor parser.
 */

/**
 * @typedef openvrml::vec2d openvrml::vec2d_parser::result_t
 *
 * @brief Parse result.
 */

/**
 * @fn std::ptrdiff_t openvrml::vec2d_parser::operator()(const ScannerT & scan, result_t & result) const
 *
 * @brief Parse a double-precision 2-component vector value.
 *
 * @param[in] scan      a scanner.
 * @param[out] result   the parse result.
 */

/**
 * @var const boost::spirit::functor_parser<openvrml::vec2d_parser> openvrml::vec2d_p
 *
 * @brief A Spirit parser for double-precision 2-component vector values.
 */

/**
 * @struct openvrml::x3d_vrml_grammar openvrml/x3d_vrml_grammar.h
 *
 * @brief A <a href="http://spirit.sourceforge.net">Spirit</a> grammar for
 *        parsing Classic VRML X3D.
 */

/**
 * @struct openvrml::x3d_vrml_grammar::definition openvrml/x3d_vrml_grammar.h
 *
 * @brief Classic VRML X3D grammar definition.
 */
