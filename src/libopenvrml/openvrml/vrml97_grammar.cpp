/**
 * @class openvrml::vrml97_space_parser openvrml/vrml97_grammar.h
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
 * @class openvrml::vrml97_skip_grammar openvrml/vrml97_grammar.h
 *
 * @brief A <a href="http://spirit.sourceforge.net">Spirit</a> grammar
 *        appropriate for use as a skip parser for VRML97 and VRML-encoded
 *        X3D.
 */

/**
 * @internal
 *
 * @struct openvrml::vrml97_skip_grammar::definition
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
 * @var openvrml::vrml_parse_error openvrml::image_expected
 *
 * @brief An image value was expected.
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
 * @var openvrml::vrml_parse_error openvrml::vec2f_expected
 *
 * @brief A vec2f value was expected.
 */

/**
 * @var openvrml::vrml_parse_error openvrml::vec2f_or_lbracket_expected
 *
 * @brief A vec2f value or &ldquo;[&rdquo; was expected.
 */

/**
 * @var openvrml::vrml_parse_error openvrml::vec2f_or_rbracket_expected
 *
 * @brief A vec2f value or &ldquo;]&rdquo; was expected.
 */

/**
 * @var openvrml::vrml_parse_error openvrml::vec3f_expected
 *
 * @brief A vec3f value was expected.
 */

/**
 * @var openvrml::vrml_parse_error openvrml::vec3f_or_lbracket_expected
 *
 * @brief A vec3f value or &ldquo;[&rdquo; was expected.
 */

/**
 * @var openvrml::vrml_parse_error openvrml::vec3f_or_rbracket_expected
 *
 * @brief A vec3f value or &ldquo;]&rdquo; was expected.
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
 * @fn std::ostream & openvrml::operator<<(std::ostream &, vrml_parse_error)
 *
 * @brief Stream output.
 *
 * @param[in,out] out    output stream.
 * @param[in] error      error identifier.
 *
 * @return @p out.
 */

/**
 * @class openvrml::vrml97_grammar openvrml/vrml97_grammar.h
 *
 * @brief A <a href="http://spirit.sourceforge.net">Spirit</a> grammar for
 *        parsing VRML97.
 */

/**
 * @example pretty_print.cpp
 *
 * A pretty-printer using @c openvrml::vrml97_grammar.
 */
