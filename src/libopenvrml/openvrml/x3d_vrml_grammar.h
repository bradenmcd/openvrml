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

# ifndef OPENVRML_X3D_VRML_GRAMMAR_H
#   define OPENVRML_X3D_VRML_GRAMMAR_H

#   include <openvrml/vrml97_grammar.h>

namespace openvrml {

    const vrml_parse_error profile_expected = vrml_parse_error_end;
    const vrml_parse_error unrecognized_profile_id =
        vrml_parse_error(vrml_parse_error_end + 1);
    const vrml_parse_error unrecognized_component_id_or_level =
        vrml_parse_error(vrml_parse_error_end + 2);
    const vrml_parse_error colon_expected =
        vrml_parse_error(vrml_parse_error_end + 3);
    const vrml_parse_error bool_or_lbracket_expected =
        vrml_parse_error(vrml_parse_error_end + 4);
    const vrml_parse_error bool_or_rbracket_expected =
        vrml_parse_error(vrml_parse_error_end + 5);
    const vrml_parse_error color_rgba_expected =
        vrml_parse_error(vrml_parse_error_end + 6);
    const vrml_parse_error color_rgba_or_lbracket_expected =
        vrml_parse_error(vrml_parse_error_end + 7);
    const vrml_parse_error color_rgba_or_rbracket_expected =
        vrml_parse_error(vrml_parse_error_end + 8);
    const vrml_parse_error eventin_deprecated =
        vrml_parse_error(vrml_parse_error_end + 9);
    const vrml_parse_error eventout_deprecated =
        vrml_parse_error(vrml_parse_error_end + 10);
    const vrml_parse_error exposedfield_deprecated =
        vrml_parse_error(vrml_parse_error_end + 11);
    const vrml_parse_error field_deprecated =
        vrml_parse_error(vrml_parse_error_end + 12);

    const vrml_parse_assertion expect_profile(profile_expected);
    const vrml_parse_assertion expect_colon(colon_expected);
    const vrml_parse_assertion expect_color_rgba(color_rgba_expected);
    const vrml_parse_assertion deprecated_eventin(eventin_deprecated);
    const vrml_parse_assertion deprecated_eventout(eventout_deprecated);
    const vrml_parse_assertion deprecated_exposedfield(exposedfield_deprecated);
    const vrml_parse_assertion deprecated_field(field_deprecated);

    inline const char * x3d_vrml_parse_error_msg(const vrml_parse_error error)
    {
        static const char * msg[] = {
            "expected PROFILE",
            "unrecognized profile identifier",
            "unrecognized component identifier or unsupported level",
            "expected :",
            "expected TRUE, FALSE, or [",
            "expected TRUE, FALSE, or ]",
            "expected an RGBA color value",
            "expected an RGBA color value or [",
            "expected an RGBA color value or ]",
            "use of deprecated keyword \"eventIn\".  Use \"inputOnly\" instead.",
            "use of deprecated keyword \"eventOut\".  Use \"outputOnly\" instead.",
            "use of deprecated keyword \"exposedField\".  Use \"inputOutput\" instead.",
            "use of deprecated keyword \"field\".  Use \"initializeOnly\" instead."
        };
        return (error < vrml_parse_error_end)
            ? vrml97_parse_error_msg(error)
            : msg[error - vrml_parse_error_end];
    }

    struct x3d_vrml_parse_error_handler {
        explicit x3d_vrml_parse_error_handler(std::ostream & out = std::cerr):
            vrml_handler(out),
            out_(out)
        {}

        template <typename ScannerT, typename ErrorT>
        boost::spirit::classic::error_status<> operator()(const ScannerT & scan,
                                                 const ErrorT & err) const
        {
            using std::endl;
            using boost::spirit::classic::error_status;
            using boost::spirit::classic::file_position;

            const file_position fpos = err.where.get_position();
            if (err.descriptor >= vrml_parse_error_end) {
                if (err.descriptor == eventin_deprecated
                    || err.descriptor == eventout_deprecated
                    || err.descriptor == exposedfield_deprecated
                    || err.descriptor == field_deprecated) {
                    this->out_ << fpos.file << ':' << fpos.line << ':'
                               << fpos.column << ": warning: "
                               << x3d_vrml_parse_error_msg(err.descriptor)
                               << endl;
                    scan.first = err.where;
                    return error_status<>(error_status<>::accept, 0);
                }

                this->out_ << fpos.file << ':' << fpos.line << ':'
                           << fpos.column << ": error: "
                           << x3d_vrml_parse_error_msg(err.descriptor) << endl;
                return error_status<>(error_status<>::fail);
            }

            return vrml_handler(scan, err);
        }

    private:
        vrml97_parse_error_handler vrml_handler;
        std::ostream & out_;
    };


    template <>
    struct get_mftype_parse_error<boost::spirit::classic::functor_parser<bool_parser> > {
        static const vrml_parse_error element_or_lbracket_value =
            bool_or_lbracket_expected;
        static const vrml_parse_error element_or_rbracket_value =
            bool_or_rbracket_expected;
    };

    template <>
    struct get_mftype_parse_error<boost::spirit::classic::functor_parser<image_parser> > {
        static const vrml_parse_error element_or_lbracket_value =
            int32_or_lbracket_expected;
        static const vrml_parse_error element_or_rbracket_value =
            int32_or_rbracket_expected;
    };


    struct color_rgba_parser {

        typedef openvrml::color_rgba result_t;

        template <typename ScannerT>
        std::ptrdiff_t operator()(const ScannerT & scan,
                                  result_t & result) const
        {
            using boost::spirit::classic::match_result;
            using phoenix::arg1;
            using phoenix::var;
            typedef typename boost::spirit::classic::rule<ScannerT> rule_t;
            typedef typename match_result<ScannerT, result_t>::type match_t;

            rule_t rule
                =   intensity_p[var(result.rgba[0]) = arg1]
                    >> intensity_p[var(result.rgba[1]) = arg1]
                    >> intensity_p[var(result.rgba[2]) = arg1]
                    >> intensity_p[var(result.rgba[3]) = arg1]
                ;
            match_t match = rule.parse(scan);
            return match.length();
        }
    };

    const boost::spirit::classic::functor_parser<color_rgba_parser> color_rgba_p;

    template <>
    struct get_mftype_parse_error<boost::spirit::classic::functor_parser<color_rgba_parser> > {
        static const vrml_parse_error element_or_lbracket_value =
            color_rgba_or_lbracket_expected;
        static const vrml_parse_error element_or_rbracket_value =
            color_rgba_or_rbracket_expected;
    };


    struct vec2d_parser {

        typedef openvrml::vec2d result_t;

        template <typename ScannerT>
        std::ptrdiff_t operator()(const ScannerT & scan,
                                  result_t & result) const
        {
            using boost::spirit::classic::match_result;
            using boost::spirit::classic::real_p;
            using phoenix::arg1;
            using phoenix::var;
            typedef typename boost::spirit::classic::rule<ScannerT> rule_t;
            typedef typename match_result<ScannerT, result_t>::type match_t;
            rule_t rule
                =   real_p[var(result.vec[0]) = arg1]
                    >> real_p[var(result.vec[1]) = arg1]
                ;
            match_t match = rule.parse(scan);
            return match.length();
        }
    };

    const boost::spirit::classic::functor_parser<vec2d_parser> vec2d_p;

    template <>
    struct get_mftype_parse_error<boost::spirit::classic::functor_parser<vec2d_parser> > {
        static const vrml_parse_error element_or_lbracket_value =
            vec2_or_lbracket_expected;
        static const vrml_parse_error element_or_rbracket_value =
            vec2_or_rbracket_expected;
    };


    struct vec3d_parser {

        typedef openvrml::vec3d result_t;

        template <typename ScannerT>
        std::ptrdiff_t operator()(const ScannerT & scan,
                                  result_t & result) const
        {
            using boost::spirit::classic::match_result;
            using boost::spirit::classic::real_p;
            using phoenix::arg1;
            using phoenix::var;
            typedef typename boost::spirit::classic::rule<ScannerT> rule_t;
            typedef typename match_result<ScannerT, result_t>::type match_t;
            rule_t rule
                =   real_p[var(result.vec[0]) = arg1]
                    >> real_p[var(result.vec[1]) = arg1]
                    >> real_p[var(result.vec[2]) = arg1]
                ;
            match_t match = rule.parse(scan);
            return match.length();
        }
    };

    const boost::spirit::classic::functor_parser<vec3d_parser> vec3d_p;

    template <>
    struct get_mftype_parse_error<boost::spirit::classic::functor_parser<vec3d_parser> > {
        static const vrml_parse_error element_or_lbracket_value =
            vec3_or_lbracket_expected;
        static const vrml_parse_error element_or_rbracket_value =
            vec3_or_rbracket_expected;
    };


    struct null_x3d_vrml_parse_actions : null_vrml97_parse_actions {
        struct on_profile_statement_t {
            void operator()(const std::string & /* profile_id */) const
            {}
        } on_profile_statement;

        struct on_component_statement_t {
            void operator()(const std::string & /* component_id */,
                            int32 /* level */) const
            {}
        } on_component_statement;

        struct on_meta_statement_t {
            void operator()(const std::string & /* name */,
                            const std::string & /* value */) const
            {}
        } on_meta_statement;

        struct on_import_statement_t {
            void operator()(const std::string & /* inline_node_name_id */,
                            const std::string & /* exported_node_name_id */,
                            const std::string & /* imported_node_name_id */) const
            {}
        } on_import_statement;

        struct on_export_statement_t {
            void operator()(const std::string & /* node_name_id */,
                            const std::string & /* exported_node_name_id */) const
            {}
        } on_export_statement;

        struct on_sfcolorrgba_t {
            void operator()(const color_rgba & /* val */) const
            {}
        } on_sfcolorrgba;

        struct on_sfdouble_t {
            void operator()(double /* val */) const
            {}
        } on_sfdouble;

        struct on_sfvec2d_t {
            void operator()(const vec2d & /* val */) const
            {}
        } on_sfvec2d;

        struct on_sfvec3d_t {
            void operator()(const vec3d & /* val */) const
            {}
        } on_sfvec3d;

        struct on_mfbool_t {
            void operator()(const std::vector<bool> & /* val */) const
            {}
        } on_mfbool;

        struct on_mfcolorrgba_t {
            void operator()(const std::vector<color_rgba> & /* val */) const
            {}
        } on_mfcolorrgba;

        struct on_mfdouble_t {
            void operator()(const std::vector<double> & /* val */) const
            {}
        } on_mfdouble;

        struct on_mfimage_t {
            void operator()(const std::vector<image> & /* val */) const
            {}
        } on_mfimage;

        struct on_mfvec2d_t {
            void operator()(const std::vector<vec2d> & /* val */) const
            {}
        } on_mfvec2d;

        struct on_mfvec3d_t {
            void operator()(const std::vector<vec3d> & /* val */) const
            {}
        } on_mfvec3d;
    };


    struct profile_statement_closure :
        boost::spirit::classic::closure<profile_statement_closure,
                               std::string> {
        member1 profile;
    };

    struct component_statement_closure :
        boost::spirit::classic::closure<component_statement_closure,
                               std::pair<std::string, int32> > {
        member1 component;
    };

    struct set_component_id_function {
        template <typename Component, typename String>
        struct result {
            typedef void type;
        };

        template <typename Component, typename String>
        void operator()(Component & component,
                        const String & component_id) const
        {
            component.first = component_id;
        }
    };

    const phoenix::function<set_component_id_function> set_component_id;

    struct set_component_level_function {
        template <typename Component, typename Int32>
        struct result {
            typedef void type;
        };

        template <typename Component, typename Int32>
        void operator()(Component & component, Int32 level) const
        {
            component.second = level;
        }
    };

    const phoenix::function<set_component_level_function> set_component_level;

    struct meta_statement_closure :
        boost::spirit::classic::closure<meta_statement_closure,
                               std::pair<std::string, std::string> > {
        member1 metadata;
    };

    struct set_meta_name_function {
        template <typename Metadata, typename String>
        struct result {
            typedef void type;
        };

        template <typename Metadata, typename String>
        void operator()(Metadata & metadata, const String & name) const
        {
            metadata.first = name;
        }
    };

    const phoenix::function<set_meta_name_function> set_meta_name;

    struct set_meta_value_function {
        template <typename Metadata, typename String>
        struct result {
            typedef void type;
        };

        template <typename Metadata, typename String>
        void operator()(Metadata & metadata, const String & value) const
        {
            metadata.second = value;
        }
    };

    const phoenix::function<set_meta_value_function> set_meta_value;

    struct import_statement_closure :
        boost::spirit::classic::closure<import_statement_closure,
                               const defs_t::value_type *,
                               std::string,
                               std::string> {
        member1 inline_node_name_id;
        member2 exported_node_name_id;
        member3 imported_node_name_id;
    };

    struct export_statement_closure :
        boost::spirit::classic::closure<export_statement_closure,
                               const defs_t::value_type *,
                               std::string> {
        member1 node_name_id;
        member2 exported_node_name_id;
    };

    template <typename Actions = null_x3d_vrml_parse_actions,
              typename ErrorHandler = x3d_vrml_parse_error_handler>
    struct x3d_vrml_grammar :
        boost::spirit::classic::grammar<x3d_vrml_grammar<Actions, ErrorHandler> > {

        template <typename ScannerT>
        struct definition :
            vrml97_grammar<Actions, ErrorHandler>::
            template definition<ScannerT> {

            typedef typename vrml97_grammar<Actions, ErrorHandler>::
                template definition<ScannerT>
                base_t;

            typedef typename base_t::rule_type rule_type;
            typedef typename base_t::chset chset;

            typedef boost::spirit::classic::rule<ScannerT,
                                        profile_statement_closure::context_t>
                profile_statement_rule_type;

            typedef boost::spirit::classic::rule<ScannerT,
                                        component_statement_closure::context_t>
                component_statement_rule_type;

            typedef boost::spirit::classic::rule<ScannerT,
                                        meta_statement_closure::context_t>
                meta_statement_rule_type;

            typedef boost::spirit::classic::rule<ScannerT,
                                        import_statement_closure::context_t>
                import_statement_rule_type;

            typedef boost::spirit::classic::rule<ScannerT,
                                        export_statement_closure::context_t>
                export_statement_rule_type;

            struct load_profile_t {
                explicit load_profile_t(scope_stack_t & scope_stack):
                    scope_stack(scope_stack)
                {}

                template <typename IteratorT>
                void operator()(IteratorT first, IteratorT last) const
                {
                    std::auto_ptr<node_type_decls> node_types;
                    try {
                        node_types = profile(std::string(first, last));
                    } catch (std::invalid_argument &) {
                        boost::spirit::classic::throw_(first, unrecognized_profile_id);
                    }
                    this->scope_stack.top().node_body_repo = *node_types;
                }
            private:
                scope_stack_t & scope_stack;
            } load_profile;

            struct load_component_t {
                explicit load_component_t(scope_stack_t & scope_stack):
                    scope_stack(scope_stack)
                {}

                template <typename IteratorT>
                void operator()(IteratorT first, IteratorT last) const
                {
                    using std::string;
                    const string component(first, last);
                    const string::size_type colon_index =
                        component.find_first_of(':');
                    const string component_id =
                        component.substr(0, colon_index);
                    size_t level;
                    std::istringstream in(component.substr(colon_index + 1));
                    in >> level;
                    try {
                        add_component(this->scope_stack.top().node_body_repo,
                                      component_id,
                                      level);
                    } catch (std::invalid_argument &) {
                        boost::spirit::classic::throw_(
                            first,
                            unrecognized_component_id_or_level);
                    }
                }
            private:
                scope_stack_t & scope_stack;
            } load_component;

            struct on_profile_statement_t {
                explicit on_profile_statement_t(const Actions & actions):
                    actions(actions)
                {}

                void operator()(const std::string & profile_id) const
                {
                    this->actions.on_profile_statement(profile_id);
                }
            private:
                const Actions & actions;
            } on_profile_statement;

            struct on_component_statement_t {
                explicit on_component_statement_t(const Actions & actions):
                    actions(actions)
                {}

                void operator()(
                    const std::pair<std::string, int32> & component) const
                {
                    this->actions.on_component_statement(component.first,
                                                         component.second);
                }
            private:
                const Actions & actions;
            } on_component_statement;

            struct on_meta_statement_t {
                explicit on_meta_statement_t(const Actions & actions):
                    actions(actions)
                {}

                void operator()(
                    const std::pair<std::string, std::string> & metadata) const
                {
                    this->actions.on_meta_statement(metadata.first,
                                                    metadata.second);
                }
            private:
                const Actions & actions;
            } on_meta_statement;

            struct on_import_statement_function {
                template <typename DefEntryPtr,
                          typename ExportedNodeNameId,
                          typename ImportedNodeNameId>
                struct result {
                    typedef void type;
                };

                explicit on_import_statement_function(const Actions & actions):
                    actions(actions)
                {}

                template <typename DefEntryPtr,
                          typename ExportedNodeNameId,
                          typename ImportedNodeNameId>
                void operator()(DefEntryPtr inline_node,
                                ExportedNodeNameId exported_node_name_id,
                                ImportedNodeNameId imported_node_name_id) const
                {
                    actions.on_import_statement(inline_node->first,
                                                exported_node_name_id,
                                                imported_node_name_id);
                }

            private:
                const Actions & actions;
            };

            phoenix::function<on_import_statement_function> on_import_statement;

            struct on_export_statement_function {
                template <typename DefEntryPtr,
                          typename ExportedNodeNameId>
                struct result {
                    typedef void type;
                };

                explicit on_export_statement_function(const Actions & actions):
                    actions(actions)
                {}

                template <typename DefEntryPtr,
                          typename ExportedNodeNameId>
                void operator()(DefEntryPtr node,
                                ExportedNodeNameId exported_node_name_id) const
                {
                    actions.on_export_statement(node->first,
                                                exported_node_name_id);
                }

            private:
                const Actions & actions;
            };

            phoenix::function<on_export_statement_function> on_export_statement;

            using base_t::node_name_id_p;

            using base_t::keywords;
            using base_t::vrml_scene;
            using base_t::statement;
            using base_t::id;
            using base_t::proto_statement;
            using base_t::route_statement;
            using base_t::node_statement;
            using base_t::eventin;
            using base_t::eventout;
            using base_t::exposedfield;
            using base_t::field;
            using base_t::field_type;

            profile_statement_rule_type profile_statement;
            component_statement_rule_type component_statement;
            meta_statement_rule_type meta_statement;
            import_statement_rule_type import_statement;
            export_statement_rule_type export_statement;

            chset invalid_id_rest_char, invalid_id_first_char;

            explicit definition(const x3d_vrml_grammar & self);

            const boost::spirit::classic::rule<ScannerT> & start() const
            {
                return this->vrml_scene;
            }

            using base_t::mftype_parser;

            static const boost::spirit::classic::functor_parser<
                typename base_t::template mftype_parser<
                    boost::spirit::classic::functor_parser<openvrml::color_rgba_parser> > >
                mfcolorrgba_p;

            static const boost::spirit::classic::functor_parser<
                typename base_t::template mftype_parser<
                    boost::spirit::classic::functor_parser<openvrml::bool_parser> > >
                mfbool_p;

            static const boost::spirit::classic::functor_parser<
                typename base_t::template mftype_parser<
                    boost::spirit::classic::real_parser<
                        double,
                        boost::spirit::classic::real_parser_policies<double> > > >
                mfdouble_p;

            static const boost::spirit::classic::functor_parser<
                typename base_t::template mftype_parser<
                    boost::spirit::classic::functor_parser<openvrml::image_parser> > >
                mfimage_p;

            static const boost::spirit::classic::functor_parser<
                typename base_t::template mftype_parser<
                    boost::spirit::classic::functor_parser<openvrml::vec2d_parser> > >
                mfvec2d_p;

            static const boost::spirit::classic::functor_parser<
                typename base_t::template mftype_parser<
                    boost::spirit::classic::functor_parser<openvrml::vec3d_parser> > >
                mfvec3d_p;

            virtual const boost::spirit::classic::stored_rule<ScannerT>
            get_field_value_parser(const field_value::type_id field_type) const
            {
                boost::spirit::classic::stored_rule<ScannerT> r;
                switch (field_type) {
                case field_value::sfcolorrgba_id:
                    r = expect_color_rgba(color_rgba_p)[
                            base_t::self.actions.on_sfcolorrgba
                        ];
                    break;
                case field_value::sfdouble_id:
                    r = expect_float(boost::spirit::classic::real_p)[
                            base_t::self.actions.on_sfdouble
                        ];
                    break;
                case field_value::sfvec2d_id:
                    r = expect_vec2(vec2d_p)[this->self.actions.on_sfvec2d];
                    break;
                case field_value::sfvec3d_id:
                    r = expect_vec3(vec3d_p)[this->self.actions.on_sfvec3d];
                    break;
                case field_value::mfbool_id:
                    r = mfbool_p[base_t::self.actions.on_mfbool];
                    break;
                case field_value::mfcolorrgba_id:
                    r = mfcolorrgba_p[base_t::self.actions.on_mfcolorrgba];
                    break;
                case field_value::mfdouble_id:
                    r = mfdouble_p[base_t::self.actions.on_mfdouble];
                    break;
                case field_value::mfimage_id:
                    r = mfimage_p[base_t::self.actions.on_mfimage];
                    break;
                case field_value::mfvec2d_id:
                    r = mfvec2d_p[base_t::self.actions.on_mfvec2d];
                    break;
                case field_value::mfvec3d_id:
                    r = mfvec3d_p[base_t::self.actions.on_mfvec3d];
                    break;
                default:
                    r = base_t::get_field_value_parser(field_type);
                }
                return r;
            }
        };

        explicit x3d_vrml_grammar(
            const Actions & actions = Actions(),
            const ErrorHandler & handler = ErrorHandler()):
            vrml97_g(actions, handler)
        {}

    private:
        vrml97_grammar<Actions, ErrorHandler> vrml97_g;
    };

    template <typename Actions, typename ErrorHandler>
    template <typename ScannerT>
    const boost::spirit::classic::functor_parser<
        typename vrml97_grammar<Actions, ErrorHandler>::template definition<ScannerT>::
        template mftype_parser<boost::spirit::classic::functor_parser<openvrml::bool_parser> > >
    x3d_vrml_grammar<Actions, ErrorHandler>::definition<ScannerT>::mfbool_p =
        typename vrml97_grammar<Actions, ErrorHandler>::template definition<ScannerT>::
        template mftype_parser<
            boost::spirit::classic::functor_parser<openvrml::bool_parser>
        >(bool_p);

    template <typename Actions, typename ErrorHandler>
    template <typename ScannerT>
    const boost::spirit::classic::functor_parser<
        typename vrml97_grammar<Actions, ErrorHandler>::template definition<ScannerT>::
        template mftype_parser<
            boost::spirit::classic::real_parser<
                double,
                boost::spirit::classic::real_parser_policies<double> > > >
    x3d_vrml_grammar<Actions, ErrorHandler>::definition<ScannerT>::mfdouble_p =
        typename vrml97_grammar<Actions, ErrorHandler>::template definition<ScannerT>::
        template mftype_parser<
            boost::spirit::classic::real_parser<
               double,
               boost::spirit::classic::real_parser_policies<double>
            >
        >(boost::spirit::classic::real_p);

    template <typename Actions, typename ErrorHandler>
    template <typename ScannerT>
    const boost::spirit::classic::functor_parser<
        typename vrml97_grammar<Actions, ErrorHandler>::template definition<ScannerT>::
        template mftype_parser<boost::spirit::classic::functor_parser<openvrml::color_rgba_parser> > >
    x3d_vrml_grammar<Actions, ErrorHandler>::definition<ScannerT>::mfcolorrgba_p =
        typename vrml97_grammar<Actions, ErrorHandler>::template definition<ScannerT>::
        template mftype_parser<
            boost::spirit::classic::functor_parser<openvrml::color_rgba_parser>
        >(color_rgba_p);

    template <typename Actions, typename ErrorHandler>
    template <typename ScannerT>
    const boost::spirit::classic::functor_parser<
        typename vrml97_grammar<Actions, ErrorHandler>::template definition<ScannerT>::
        template mftype_parser<boost::spirit::classic::functor_parser<openvrml::image_parser> > >
    x3d_vrml_grammar<Actions, ErrorHandler>::definition<ScannerT>::mfimage_p =
        typename vrml97_grammar<Actions, ErrorHandler>::template definition<ScannerT>::
        template mftype_parser<
            boost::spirit::classic::functor_parser<openvrml::image_parser>
        >(image_p);

    template <typename Actions, typename ErrorHandler>
    template <typename ScannerT>
    const boost::spirit::classic::functor_parser<
        typename vrml97_grammar<Actions, ErrorHandler>::template definition<ScannerT>::
        template mftype_parser<boost::spirit::classic::functor_parser<openvrml::vec2d_parser> > >
    x3d_vrml_grammar<Actions, ErrorHandler>::definition<ScannerT>::mfvec2d_p =
        typename vrml97_grammar<Actions, ErrorHandler>::template definition<ScannerT>::
        template mftype_parser<
            boost::spirit::classic::functor_parser<openvrml::vec2d_parser>
        >(vec2d_p);

    template <typename Actions, typename ErrorHandler>
    template <typename ScannerT>
    const boost::spirit::classic::functor_parser<
        typename vrml97_grammar<Actions, ErrorHandler>::template definition<ScannerT>::
        template mftype_parser<boost::spirit::classic::functor_parser<openvrml::vec3d_parser> > >
    x3d_vrml_grammar<Actions, ErrorHandler>::definition<ScannerT>::mfvec3d_p =
        typename vrml97_grammar<Actions, ErrorHandler>::template definition<ScannerT>::
        template mftype_parser<
            boost::spirit::classic::functor_parser<openvrml::vec3d_parser>
        >(vec3d_p);

    template <typename Actions, typename ErrorHandler>
    template <typename ScannerT>
    x3d_vrml_grammar<Actions, ErrorHandler>::definition<ScannerT>::
    definition(const x3d_vrml_grammar & self):
        base_t(self.vrml97_g),
        load_profile(base_t::scope_stack),
        load_component(base_t::scope_stack),
        on_profile_statement(self.vrml97_g.actions),
        on_component_statement(self.vrml97_g.actions),
        on_meta_statement(self.vrml97_g.actions),
        on_import_statement(on_import_statement_function(self.vrml97_g.actions)),
        on_export_statement(on_export_statement_function(self.vrml97_g.actions)),
        invalid_id_rest_char(base_t::invalid_id_rest_char | ':'),
        invalid_id_first_char(base_t::invalid_id_first_char | ':')
    {
        using std::string;
        using namespace boost::spirit::classic;
        using namespace phoenix;

        BOOST_SPIRIT_DEBUG_NODE(profile_statement);
        BOOST_SPIRIT_DEBUG_NODE(component_statement);
        BOOST_SPIRIT_DEBUG_NODE(meta_statement);

        guard<vrml_parse_error> g;

        keywords.add("AS")("COMPONENT")("EXPORT")("IMPORT")("META")
            ("PROFILE")("inputOnly")("outputOnly")("inputOutput")
            ("initializeOnly");

        vrml_scene
            =  g(
                    profile_statement[on_profile_statement]
                    >> *component_statement[on_component_statement]
                    >> *meta_statement[on_meta_statement]
                    >> *statement >> end_p
                )[self.vrml97_g.error_handler]
            ;

        profile_statement
            =   expect_profile(str_p("PROFILE"))
                >> expect_id(id[load_profile][
                    profile_statement.profile = construct_<string>(arg1, arg2)
                ])
            ;

        component_statement
            =   "COMPONENT" >> (
                    expect_id(id)[
                        set_component_id(component_statement.component,
                                         construct_<string>(arg1, arg2))
                    ] >> expect_colon(ch_p(':'))
                    >> expect_int32(int32_p)[
                        set_component_level(component_statement.component,
                                            arg1)
                    ]
                )[load_component]
            ;

        meta_statement
            =   "META" >> expect_string(string_p)[
                    set_meta_name(meta_statement.metadata, arg1)
                ]
                >> expect_string(string_p)[
                    set_meta_value(meta_statement.metadata, arg1)
                ]
            ;

        statement
            =   proto_statement
            |   route_statement
            |   node_statement
            |   import_statement
            |   export_statement
            ;

        import_statement
            =   "IMPORT" >> node_name_id_p[
                    import_statement.inline_node_name_id = arg1
                ]
                >> '.' >> id[
                    import_statement.exported_node_name_id =
                        construct_<string>(arg1, arg2)
                ]
                >> "AS" >> id[
                    import_statement.imported_node_name_id =
                        construct_<string>(arg1, arg2)
                ]
                >> eps_p[
                    on_import_statement(import_statement.inline_node_name_id,
                                        import_statement.exported_node_name_id,
                                        import_statement.imported_node_name_id)
                ]
            ;

        export_statement
            =   "EXPORT" >> node_name_id_p[export_statement.node_name_id = arg1]
                >> "AS" >> id[
                    export_statement.exported_node_name_id =
                        construct_<string>(arg1, arg2)
                ]
            ;

        id
            =   lexeme_d[
                    ((anychar_p - vrml97_space_p - invalid_id_first_char)
                     >> *(anychar_p - vrml97_space_p - invalid_id_rest_char))
                    - (keywords
                       >> (anychar_p - (anychar_p - vrml97_space_p
                                        - invalid_id_rest_char)))
                ]
            ;

        eventin
            =   str_p("inputOnly")
            |   str_p("eventIn")
                >> g(deprecated_eventin(nothing_p))[
                    self.vrml97_g.error_handler
                ]
            ;

        eventout
            =   str_p("outputOnly")
            |   str_p("eventOut")
                >> g(deprecated_eventout(nothing_p))[
                    self.vrml97_g.error_handler
                ]
            ;

        exposedfield
            =   str_p("inputOutput")
            |   str_p("exposedField")
                >> g(deprecated_exposedfield(nothing_p))[
                    self.vrml97_g.error_handler
                ]
            ;

        field
            =   str_p("initializeOnly")
            |   str_p("field")
                >> g(deprecated_field(nothing_p))[self.vrml97_g.error_handler]
            ;

        field_type
            =   str_p("MFBool")[field_type.type = field_value::mfbool_id]
            |   str_p("MFColor")[field_type.type = field_value::mfcolor_id]
            |   str_p("MFColorRGBA")[field_type.type = field_value::mfcolorrgba_id]
            |   str_p("MFFloat")[field_type.type = field_value::mffloat_id]
            |   str_p("MFDouble")[field_type.type = field_value::mfdouble_id]
            |   str_p("MFImage")[field_type.type = field_value::mfimage_id]
            |   str_p("MFInt32")[field_type.type = field_value::mfint32_id]
            |   str_p("MFNode")[field_type.type = field_value::mfnode_id]
            |   str_p("MFRotation")[field_type.type = field_value::mfrotation_id]
            |   str_p("MFString")[field_type.type = field_value::mfstring_id]
            |   str_p("MFTime")[field_type.type = field_value::mftime_id]
            |   str_p("MFVec2f")[field_type.type = field_value::mfvec2f_id]
            |   str_p("MFVec2d")[field_type.type = field_value::mfvec2d_id]
            |   str_p("MFVec3f")[field_type.type = field_value::mfvec3f_id]
            |   str_p("MFVec3d")[field_type.type = field_value::mfvec3d_id]
            |   str_p("SFBool")[field_type.type = field_value::sfbool_id]
            |   str_p("SFColor")[field_type.type = field_value::sfcolor_id]
            |   str_p("SFColorRGBA")[field_type.type = field_value::sfcolorrgba_id]
            |   str_p("SFFloat")[field_type.type = field_value::sffloat_id]
            |   str_p("SFDouble")[field_type.type = field_value::sfdouble_id]
            |   str_p("SFImage")[field_type.type = field_value::sfimage_id]
            |   str_p("SFInt32")[field_type.type = field_value::sfint32_id]
            |   str_p("SFNode")[field_type.type = field_value::sfnode_id]
            |   str_p("SFRotation")[field_type.type = field_value::sfrotation_id]
            |   str_p("SFString")[field_type.type = field_value::sfstring_id]
            |   str_p("SFTime")[field_type.type = field_value::sftime_id]
            |   str_p("SFVec2f")[field_type.type = field_value::sfvec2f_id]
            |   str_p("SFVec2d")[field_type.type = field_value::sfvec2d_id]
            |   str_p("SFVec3f")[field_type.type = field_value::sfvec3f_id]
            |   str_p("SFVec3d")[field_type.type = field_value::sfvec3d_id]
            ;
    }
}

# endif // ifndef OPENVRML_X3D_VRML_GRAMMAR_H
