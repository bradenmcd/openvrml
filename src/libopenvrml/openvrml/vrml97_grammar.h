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

# ifndef OPENVRML_VRML97_GRAMMAR_H
#   define OPENVRML_VRML97_GRAMMAR_H

#   include <stack>
#   include <boost/spirit.hpp>
#   include <boost/spirit/actor.hpp>
#   include <boost/spirit/dynamic.hpp>
#   include <boost/spirit/phoenix.hpp>
#   include <boost/test/floating_point_comparison.hpp>
#   include <openvrml/browser.h>

namespace openvrml {

    struct vrml97_space_parser :
        boost::spirit::char_parser<vrml97_space_parser> {

        typedef vrml97_space_parser self_t;

        template <typename CharT>
        bool test(const CharT & c) const
        {
            return isspace(c) || c == ',';
        }
    };

    const vrml97_space_parser vrml97_space_p = vrml97_space_parser();


    struct vrml97_skip_grammar : boost::spirit::grammar<vrml97_skip_grammar> {
        template <typename ScannerT>
        struct definition {
            typedef boost::spirit::rule<ScannerT> rule_t;

            rule_t skip;

            explicit definition(const vrml97_skip_grammar & self);

            const rule_t & start() const;
        };
    };

    template <typename ScannerT>
    vrml97_skip_grammar::definition<ScannerT>::
    definition(const vrml97_skip_grammar & /* self */)
    {
        using namespace boost::spirit;

        BOOST_SPIRIT_DEBUG_NODE(skip);

        skip
            = vrml97_space_p
            | comment_p("#")
            ;
    }

    template <typename ScannerT>
    const typename vrml97_skip_grammar::definition<ScannerT>::rule_t &
    vrml97_skip_grammar::definition<ScannerT>::start() const
    {
        return this->skip;
    }

    enum vrml_parse_error {
        dot_expected,
        lbrace_expected,
        lbracket_expected,
        id_expected,
        to_expected,
        is_expected,
        interface_type_or_rbracket_expected,
        field_type_expected,
        bool_expected,
        color_expected,
        color_or_lbracket_expected,
        color_or_rbracket_expected,
        float_expected,
        float_or_lbracket_expected,
        float_or_rbracket_expected,
        int32_expected,
        int32_or_lbracket_expected,
        int32_or_rbracket_expected,
        rotation_expected,
        rotation_or_lbracket_expected,
        rotation_or_rbracket_expected,
        string_expected,
        string_or_lbracket_expected,
        string_or_rbracket_expected,
        vec2_expected,
        vec2_or_lbracket_expected,
        vec2_or_rbracket_expected,
        vec3_expected,
        vec3_or_lbracket_expected,
        vec3_or_rbracket_expected,
        node_expected,
        node_or_lbracket_expected,
        node_or_rbracket_expected,
        interface_collision,
        unknown_node_type_id,
        node_type_already_exists,
        unknown_node_name_id,
        unknown_node_interface_id,
        proto_interface_id_expected,
        incompatible_proto_interface,
        eventin_id_expected,
        eventout_id_expected,
        event_value_type_mismatch,
        field_or_prototype_or_route_or_rbrace_expected,
        script_interface_or_field_or_prototype_or_route_or_rbrace_expected,
        rotation_axis_not_normalized,
        vrml_parse_error_end,
        vrml_parse_error_max = INT_MAX
    };

    template <typename Parser>
    struct get_mftype_parse_error;

    template <>
    struct get_mftype_parse_error<boost::spirit::real_parser<float, boost::spirit::real_parser_policies<float> > > {
        static const vrml_parse_error element_or_lbracket_value =
            float_or_lbracket_expected;
        static const vrml_parse_error element_or_rbracket_value =
            float_or_rbracket_expected;
    };

    template <>
    struct get_mftype_parse_error<boost::spirit::real_parser<double, boost::spirit::real_parser_policies<double> > > {
        static const vrml_parse_error element_or_lbracket_value =
            float_or_lbracket_expected;
        static const vrml_parse_error element_or_rbracket_value =
            float_or_rbracket_expected;
    };

    inline const char * vrml97_parse_error_msg(const vrml_parse_error error)
    {
        static const char * msg[] = {
            "expected .",
            "expected {",
            "expected [",
            "expected an identifier",
            "expected TO",
            "expected IS",
            "expected an interface type or ]",
            "expected a field type",
            "expected TRUE or FALSE",
            "expected a color value",
            "expected a color value or [",
            "expected a color value or ]",
            "expected a floating-point value",
            "expected a floating-point value or [",
            "expected a floating-point value or ]",
            "expected an integer value",
            "expected an integer value or [",
            "expected an integer value or ]",
            "expected a rotation value",
            "expected a rotation value or [",
            "expected a rotation value or ]",
            "expected a string value",
            "expected a string value or [",
            "expected a string value or ]",
            "expected a 2-component vector value",
            "expected a 2-component vector value or [",
            "expected a 2-component vector value or ]",
            "expected a 3-component vector value",
            "expected a 3-component vector value or [",
            "expected a 3-component vector value or ]",
            "expected a node",
            "expected a node or [",
            "expected a node or ]",
            "interface conflicts with previous declaration",
            "unknown node type identifier",
            "node type already exists",
            "unknown node name identifier",
            "unknown node interface identifier",
            "expected a PROTO interface identifier",
            "incompatible PROTO interface",
            "expected an eventIn identifier",
            "expected an eventOut identifier",
            "eventIn value type does not match eventOut value type",
            "expected a field declaration, PROTO, EXTERNPROTO, ROUTE, or }",
            "expected an interface declaration, a field declaration, PROTO, EXTERNPROTO, ROUTE, or }",
            "rotation axis should be a normalized vector"
        };
        return msg[error];
    }

    typedef boost::spirit::assertion<vrml_parse_error> vrml_parse_assertion;

    const vrml_parse_assertion expect_dot(dot_expected);
    const vrml_parse_assertion expect_lbrace(lbrace_expected);
    const vrml_parse_assertion expect_lbracket(lbracket_expected);
    const vrml_parse_assertion expect_id(id_expected);
    const vrml_parse_assertion expect_to(to_expected);
    const vrml_parse_assertion expect_is(is_expected);
    const vrml_parse_assertion expect_interface_type_or_rbracket(
        interface_type_or_rbracket_expected);
    const vrml_parse_assertion expect_field_type(field_type_expected);
    const vrml_parse_assertion expect_bool(bool_expected);
    const vrml_parse_assertion expect_color(color_expected);
    const vrml_parse_assertion expect_float(float_expected);
    const vrml_parse_assertion expect_int32(int32_expected);
    const vrml_parse_assertion expect_rotation(rotation_expected);
    const vrml_parse_assertion expect_string(string_expected);
    const vrml_parse_assertion expect_vec2(vec2_expected);
    const vrml_parse_assertion expect_vec3(vec3_expected);
    const vrml_parse_assertion expect_node(node_expected);
    const vrml_parse_assertion expect_unique_node_interface(
        interface_collision);
    const vrml_parse_assertion expect_proto_interface_id(
        proto_interface_id_expected);
    const vrml_parse_assertion expect_compatible_proto_interface(
        incompatible_proto_interface);
    const vrml_parse_assertion expect_eventin_id(eventin_id_expected);
    const vrml_parse_assertion expect_eventout_id(eventout_id_expected);
    const vrml_parse_assertion expect_eventin_value_type_matches_eventout(
        event_value_type_mismatch);
    const vrml_parse_assertion expect_field_or_prototype_or_route_or_rbrace(
        field_or_prototype_or_route_or_rbrace_expected);
    const vrml_parse_assertion
        expect_script_interface_or_field_or_prototype_or_route_or_rbrace(
            script_interface_or_field_or_prototype_or_route_or_rbrace_expected);
    const vrml_parse_assertion expect_normalized_rotation(
        rotation_axis_not_normalized);

    struct vrml97_parse_error_handler {
        explicit vrml97_parse_error_handler(std::ostream & out = std::cerr):
            out_(out)
        {}

        template <typename ScannerT, typename ErrorT>
        boost::spirit::error_status<> operator()(const ScannerT & scan,
                                                 const ErrorT & err) const
        {
            using std::endl;
            using boost::spirit::error_status;
            using boost::spirit::file_position;

            const file_position fpos = err.where.get_position();
            if (err.descriptor == rotation_axis_not_normalized) {
                this->out_ << fpos.file << ':' << fpos.line << ':'
                           << fpos.column << ": warning: "
                           << vrml97_parse_error_msg(err.descriptor)
                           << endl;
                scan.first = err.where;
                return error_status<>(error_status<>::accept, 0);
            }

            this->out_ << fpos.file << ':' << fpos.line << ':' << fpos.column
                       << ": error: "
                       << vrml97_parse_error_msg(err.descriptor) << endl;

            return error_status<>(error_status<>::fail);
        }

    private:
        std::ostream & out_;
    };


    const boost::spirit::real_parser<float, boost::spirit::real_parser_policies<float> >
        float_p =
            boost::spirit::real_parser<float, boost::spirit::real_parser_policies<float> >();


    struct bool_parser {
        typedef bool result_t;

        template <typename ScannerT>
        std::ptrdiff_t operator()(const ScannerT & scan,
                                  result_t & result) const
        {
            using namespace boost::spirit;
            using namespace phoenix;
            typedef typename match_result<ScannerT, result_t>::type match_t;
            match_t match
                =   (str_p("FALSE")[var(result) = false]
                     |   str_p("TRUE")[var(result) = true]
                    ).parse(scan);
            return match.length();
        }
    };

    const boost::spirit::functor_parser<bool_parser> bool_p;


    struct int32_parser {
        typedef openvrml::int32 result_t;

        template <typename ScannerT>
        std::ptrdiff_t operator()(const ScannerT & scan,
                                  result_t & result) const
        {
            using namespace boost::spirit;
            using namespace phoenix;
            typedef typename match_result<ScannerT, result_t>::type match_t;
            match_t match
                =   (   lexeme_d[
                            ch_p('0') >> chset_p("Xx")
                            >> hex_p[var(result) = arg1]
                        ]
                    |   int_p[var(result) = arg1]
                    ).parse(scan);
            return match.length();
        }
    };

    const boost::spirit::functor_parser<int32_parser> int32_p;

    template <>
    struct get_mftype_parse_error<boost::spirit::functor_parser<int32_parser> > {
        static const vrml_parse_error element_or_lbracket_value =
            int32_or_lbracket_expected;
        static const vrml_parse_error element_or_rbracket_value =
            int32_or_rbracket_expected;
    };


    struct intensity_parser {

        typedef float result_t;

        struct valid {
            explicit valid(result_t & value):
                value_(value)
            {}

            bool operator()() const
            {
                return this->value_ >= 0.0 && this->value_ <= 1.0;
            }

        private:
            result_t & value_;
        };

        template <typename ScannerT>
        std::ptrdiff_t operator()(const ScannerT & scan,
                                  result_t & result) const
        {
            using boost::spirit::eps_p;
            using boost::spirit::match_result;
            using phoenix::arg1;
            using phoenix::var;
            typedef typename match_result<ScannerT, result_t>::type match_t;
            typedef typename boost::spirit::rule<ScannerT> rule_t;

            rule_t rule
                =   float_p[var(result) = arg1] >> eps_p(valid(result))
                ;
            match_t match = rule.parse(scan);
            return match.length();
        }
    };

    const boost::spirit::functor_parser<intensity_parser> intensity_p;


    struct color_parser {

        typedef openvrml::color result_t;

        template <typename ScannerT>
        std::ptrdiff_t operator()(const ScannerT & scan,
                                  result_t & result) const
        {
            using boost::spirit::match_result;
            using phoenix::arg1;
            using phoenix::var;
            typedef typename boost::spirit::rule<ScannerT> rule_t;
            typedef typename match_result<ScannerT, result_t>::type match_t;

            rule_t rule
                =   intensity_p[var(result.rgb[0]) = arg1]
                    >> intensity_p[var(result.rgb[1]) = arg1]
                    >> intensity_p[var(result.rgb[2]) = arg1]
                ;
            match_t match = rule.parse(scan);
            return match.length();
        }
    };

    const boost::spirit::functor_parser<color_parser> color_p;

    template <>
    struct get_mftype_parse_error<boost::spirit::functor_parser<color_parser> > {
        static const vrml_parse_error element_or_lbracket_value =
            color_or_lbracket_expected;
        static const vrml_parse_error element_or_rbracket_value =
            color_or_rbracket_expected;
    };


    template <typename RotationNotNormalizedHandler>
    struct rotation_parser {

        struct is_normalized {
            is_normalized(float & x, float & y, float & z):
                x_(x),
                y_(y),
                z_(z)
            {}

            bool operator()() const
            {
                using boost::test_tools::check_is_close;
                using boost::test_tools::percent_tolerance;
                const float length = sqrt(this->x_ * this->x_
                                          + this->y_ * this->y_
                                          + this->z_ * this->z_);
                return check_is_close(length, 1.0f,
                                      percent_tolerance(1.0e-6f));
            }

        private:
            float & x_, & y_, & z_;
        };

        typedef openvrml::rotation result_t;

        rotation_parser(const RotationNotNormalizedHandler & handler):
            rotation_not_normalized_handler_(handler)
        {}

        template <typename ScannerT>
        std::ptrdiff_t operator()(const ScannerT & scan,
                                  result_t & result) const
        {
            using boost::spirit::match_result;
            using boost::spirit::eps_p;
            using boost::spirit::guard;
            using phoenix::arg1;
            using phoenix::var;
            typedef typename boost::spirit::rule<ScannerT> rule_t;
            typedef typename match_result<ScannerT, result_t>::type match_t;
            guard<vrml_parse_error> g;
            rule_t rule
                =   float_p[var(result.rot[0]) = arg1]
                    >> float_p[var(result.rot[1]) = arg1]
                    >> float_p[var(result.rot[2]) = arg1]
                    >> g(expect_normalized_rotation(
                             eps_p(is_normalized(result.rot[0],
                                                 result.rot[1],
                                                 result.rot[2]))))[
                           this->rotation_not_normalized_handler_
                       ]
                    >> float_p[var(result.rot[3]) = arg1]
                ;

            match_t m = rule.parse(scan);

            return m.length();
        }

    private:
        const RotationNotNormalizedHandler & rotation_not_normalized_handler_;
    };

    template <typename RotationNotNormalizedHandler>
    struct get_mftype_parse_error<boost::spirit::functor_parser<rotation_parser<RotationNotNormalizedHandler> > > {
        static const vrml_parse_error element_or_lbracket_value =
            rotation_or_lbracket_expected;
        static const vrml_parse_error element_or_rbracket_value =
            rotation_or_rbracket_expected;
    };


    struct vec2f_parser {

        typedef openvrml::vec2f result_t;

        template <typename ScannerT>
        std::ptrdiff_t operator()(const ScannerT & scan,
                                  result_t & result) const
        {
            using boost::spirit::match_result;
            using phoenix::arg1;
            using phoenix::var;
            typedef typename boost::spirit::rule<ScannerT> rule_t;
            typedef typename match_result<ScannerT, result_t>::type match_t;
            rule_t rule
                =   float_p[var(result.vec[0]) = arg1]
                    >> float_p[var(result.vec[1]) = arg1]
                ;
            match_t match = rule.parse(scan);
            return match.length();
        }
    };

    const boost::spirit::functor_parser<vec2f_parser> vec2f_p;

    template <>
    struct get_mftype_parse_error<boost::spirit::functor_parser<vec2f_parser> > {
        static const vrml_parse_error element_or_lbracket_value =
            vec2_or_lbracket_expected;
        static const vrml_parse_error element_or_rbracket_value =
            vec2_or_rbracket_expected;
    };


    struct vec3f_parser {

        typedef openvrml::vec3f result_t;

        template <typename ScannerT>
        std::ptrdiff_t operator()(const ScannerT & scan,
                                  result_t & result) const
        {
            using boost::spirit::match_result;
            using phoenix::arg1;
            using phoenix::var;
            typedef typename boost::spirit::rule<ScannerT> rule_t;
            typedef typename match_result<ScannerT, result_t>::type match_t;
            rule_t rule
                =   float_p[var(result.vec[0]) = arg1]
                    >> float_p[var(result.vec[1]) = arg1]
                    >> float_p[var(result.vec[2]) = arg1]
                ;
            match_t match = rule.parse(scan);
            return match.length();
        }
    };

    const boost::spirit::functor_parser<vec3f_parser> vec3f_p;

    template <>
    struct get_mftype_parse_error<boost::spirit::functor_parser<vec3f_parser> > {
        static const vrml_parse_error element_or_lbracket_value =
            vec3_or_lbracket_expected;
        static const vrml_parse_error element_or_rbracket_value =
            vec3_or_rbracket_expected;
    };


    struct image_parser {

        struct set_pixel {
            set_pixel(openvrml::image & img, size_t & index):
                img_(img),
                index_(index)
            {}

            template <typename NumT>
            void operator()(NumT val) const
            {
                this->img_.pixel(this->index_, val);
            }

        private:
            openvrml::image & img_;
            size_t & index_;
        };

        struct resize_image {
            resize_image(openvrml::image & img,
                         size_t & x, size_t & y, size_t & comp):
                img_(img),
                x_(x),
                y_(y),
                comp_(comp)
            {}

            template <typename IteratorT>
            void operator()(IteratorT, IteratorT) const
            {
                openvrml::image temp(this->x_, this->y_, this->comp_);
                this->img_.swap(temp);
            }

        private:
            openvrml::image & img_;
            size_t & x_, & y_, & comp_;
        };

        typedef openvrml::image result_t;

        template <typename ScannerT>
        std::ptrdiff_t operator()(const ScannerT & scan,
                                  result_t & result) const
        {
            using boost::ref;
            using boost::spirit::repeat_p;
            using boost::spirit::eps_p;
            using phoenix::arg1;
            using phoenix::var;

            using boost::spirit::match_result;
            typedef typename boost::spirit::rule<ScannerT> rule_t;
            typedef typename match_result<ScannerT, result_t>::type match_t;

            size_t x = 0, y = 0, comp = 0, pixels = 0, index = 0;
            rule_t rule
                =   int32_p[var(pixels) = arg1, var(x) = arg1]
                    >> expect_int32(int32_p)[var(pixels) *= arg1,
                                             var(y) = arg1]
                    >> expect_int32(int32_p)[var(comp) = arg1]
                    // Just resize the image once we have the x, y,
                    // and comp values to avoid unnecessary
                    // reallocation.
                    >> eps_p[resize_image(result, x, y, comp)]
                    >> repeat_p(ref(pixels))[
                        expect_int32(int32_p)[set_pixel(result, index)]
                                             [var(index) += 1]
                    ]
                ;
            match_t match = rule.parse(scan);
            return match.length();
        };
    };

    const boost::spirit::functor_parser<image_parser> image_p;


    struct get_string_content_function {
        template <typename Iterator1, typename Iterator2>
        struct result {
            typedef std::string type;
        };

        template <typename Iterator1, typename Iterator2>
        std::string operator()(Iterator1 first, Iterator2 last) const
        {
            const std::string str(first, last);
            return str.substr(1, str.size() - 2);
        }
    };

    const phoenix::function<get_string_content_function> get_string_content;

    struct string_parser {

        typedef std::string result_t;

        template <typename ScannerT>
        std::ptrdiff_t operator()(const ScannerT & scan,
                                  result_t & result) const
        {
            using boost::spirit::chlit;
            using boost::spirit::strlit;
            using boost::spirit::anychar_p;
            using boost::spirit::lexeme_d;
            using boost::spirit::match_result;
            using phoenix::arg1;
            using phoenix::arg2;
            using phoenix::var;
            typedef typename boost::spirit::rule<ScannerT> rule_t;
            typedef typename match_result<ScannerT, result_t>::type match_t;

            std::string str;

            rule_t rule
                =   lexeme_d[
                    chlit<>('"') >> *(strlit<>("\\\"")
                                      | (anychar_p - chlit<>('"')))
                        >> chlit<>('"')
                    ][var(result) = get_string_content(arg1, arg2)]
                ;
            match_t match = rule.parse(scan);
            return match.length();
        }
    };

    const boost::spirit::functor_parser<string_parser> string_p;

    template <>
    struct get_mftype_parse_error<
        boost::spirit::functor_parser<string_parser> > {
        static const vrml_parse_error element_or_lbracket_value =
            string_or_lbracket_expected;
        static const vrml_parse_error element_or_rbracket_value =
            string_or_rbracket_expected;
    };


    typedef std::multimap<std::string, node_interface_set> script_node_types_t;

    typedef std::pair<node_type_decls::key_type, node_type_decls::mapped_type>
        node_type_decl;

    typedef std::map<std::string, const node_type_decls::value_type *> defs_t;


    struct parse_scope {
        const node_type_decl * proto_node_type;
        defs_t defs;
        node_type_decls node_body_repo;
        script_node_types_t script_node_types;

        parse_scope():
            proto_node_type(0)
        {}
    };

    class scope_stack_t : public std::stack<parse_scope> {
    public:
        typedef std::stack<parse_scope>::container_type::iterator
            iterator;
        typedef std::stack<parse_scope>::container_type::reverse_iterator
            reverse_iterator;

        iterator begin()
        {
            return this->c.begin();
        }

        iterator end()
        {
            return this->c.end();
        }

        reverse_iterator rbegin()
        {
            return this->c.rbegin();
        }

        reverse_iterator rend()
        {
            return this->c.rend();
        }
    };

    inline node_type_decls::value_type *
    find_node_type(scope_stack_t & scope_stack,
                   const std::string & node_type_id)
    {
        for (scope_stack_t::reverse_iterator scope = scope_stack.rbegin();
             scope != scope_stack.rend();
             ++scope) {
            const node_type_decls::iterator pos =
                scope->node_body_repo.find(node_type_id);
            if (pos != scope->node_body_repo.end()) {
                return &(*pos);
            }
        }
        return 0;
    }

    inline bool in_proto_def(const scope_stack_t & scope_stack)
    {
        return scope_stack.top().proto_node_type;
    }

    struct sfnode_closure : boost::spirit::closure<sfnode_closure,
                                                   bool> {
        member1 null;
    };

    struct interface_type_closure :
        boost::spirit::closure<interface_type_closure,
                               node_interface::type_id> {
        member1 type;
    };

    struct field_type_closure : boost::spirit::closure<field_type_closure,
                                                       field_value::type_id> {
        member1 type;
    };

    struct interface_decl_closure :
        boost::spirit::closure<interface_decl_closure,
                               node_interface> {
        member1 interface_;
    };

    struct set_node_interface_type_function {
        template <typename NodeInterface, typename NodeInterfaceType>
        struct result {
            typedef void type;
        };

        template <typename NodeInterface, typename NodeInterfaceType>
        void operator()(NodeInterface & interface_,
                        NodeInterfaceType type) const
        {
            interface_.type = type;
        }
    };

    const phoenix::function<set_node_interface_type_function>
        set_node_interface_type;

    struct set_node_interface_field_type_function {
        template <typename NodeInterface, typename FieldType>
        struct result {
            typedef void type;
        };

        template <typename NodeInterface, typename FieldType>
        void operator()(NodeInterface & interface_, FieldType field_type) const
        {
            interface_.field_type = field_type;
        }
    };

    const phoenix::function<set_node_interface_field_type_function>
        set_node_interface_field_type;

    struct set_node_interface_id_function {
        template <typename NodeInterface, typename String>
        struct result {
            typedef void type;
        };

        template <typename NodeInterface, typename String>
        void operator()(NodeInterface & interface_, String id) const
        {
            interface_.id = id;
        }
    };

    const phoenix::function<set_node_interface_id_function>
        set_node_interface_id;

    struct proto_closure : boost::spirit::closure<proto_closure,
                                                  node_type_decl,
                                                  bool> {
        member1 node_type;
        member2 add_node_interface_succeeded;
    };

    struct is_mapping_closure : boost::spirit::closure<is_mapping_closure,
                                                       node_interface,
                                                       node_interface_set,
                                                       node_interface> {
        member1 impl_interface;
        member2 proto_interfaces;
        member3 proto_interface;
    };

    struct externproto_closure :
        boost::spirit::closure<externproto_closure,
                               node_type_decl,
                               std::vector<std::string>,
                               bool> {
        member1 node_type;
        member2 url_list;
        member3 add_node_interface_succeeded;
    };

    struct script_interface_closure :
        boost::spirit::closure<script_interface_closure,
                               node_interface,
                               bool> {
        member1 interface_;
        member2 add_interface_succeeded;
    };

    struct set_node_type_id_function {
        template <typename NodeType, typename String>
        struct result {
            typedef void type;
        };

        template <typename NodeType, typename String>
        void operator()(NodeType & node_type, String id) const
        {
            node_type.first = id;
        }
    };

    const phoenix::function<set_node_type_id_function> set_node_type_id;


    struct add_node_interface_function {
        template <typename NodeType, typename NodeInterface>
        struct result {
            typedef bool type;
        };

        template <typename NodeType, typename NodeInterface>
        bool operator()(NodeType & node_type, NodeInterface interface_) const
        {
            return node_type.second.insert(interface_).second;
        }
    };

    const phoenix::function<add_node_interface_function> add_node_interface;


    struct check_function {
        template <typename Bool>
        struct result {
            typedef Bool type;
        };

        template <typename Bool>
        Bool operator()(Bool val) const
        {
            return val;
        }
    };

    const phoenix::function<check_function> check;


    struct check_valid_proto_interface_function {
        template <typename NodeInterface>
        struct result {
            typedef bool type;
        };

        template <typename NodeInterface>
        bool operator()(NodeInterface interface_) const
        {
            return interface_.type != node_interface::invalid_type_id;
        }
    };

    const phoenix::function<check_valid_proto_interface_function>
        check_valid_proto_interface;


    struct root_node_statement_closure :
        boost::spirit::closure<root_node_statement_closure,
                               std::string,
                               node_type_decls::value_type *> {
        member1 node_name_id;
        member2 node_type;
    };


    struct check_add_node_interface_succeeded {
        explicit check_add_node_interface_succeeded(const bool & succeeded):
            succeeded_(succeeded)
        {}

        template <typename IteratorT>
        void operator()(const IteratorT & first, IteratorT) const
        {
            if (!this->succeeded_) {
                boost::spirit::throw_(first, interface_collision);
            }
        }

    private:
        const bool & succeeded_;
    };


    template <typename String, typename NodeType>
    struct add_def_t {
        add_def_t(scope_stack_t & scope_stack,
                  String & node_name_id,
                  NodeType & node_type):
            scope_stack_(scope_stack),
            node_name_id_(node_name_id),
            node_type_(node_type)
        {}

        template <typename IteratorT>
        void operator()(IteratorT, IteratorT) const
        {
            const std::string node_name_id = this->node_name_id_();
            const node_type_decls::value_type * const node_type =
                this->node_type_();

            if (!node_name_id.empty()) {
                this->scope_stack_.top().defs[node_name_id] = node_type;
            }
        }

    private:
        scope_stack_t & scope_stack_;
        String & node_name_id_;
        NodeType & node_type_;
    };

    template <typename String, typename NodeType>
    const add_def_t<String, NodeType> add_def(scope_stack_t & scope_stack,
                                              String & node_name_id,
                                              NodeType & node_type)
    {
        return add_def_t<String, NodeType>(scope_stack,
                                           node_name_id,
                                           node_type);
    }


    struct route_statement_closure :
        boost::spirit::closure<route_statement_closure,
                               const defs_t::value_type *,
                               const node_interface *,
                               const defs_t::value_type *,
                               const node_interface *> {
        member1 from_node;
        member2 from_node_interface;
        member3 to_node;
        member4 to_node_interface;
    };

    struct get_route_eventout_function {
        template <typename NodeTypeDeclPtr, typename String>
        struct result {
            typedef const node_interface * type;
        };

        template <typename NodeTypeDeclPtr, typename String>
        const node_interface * operator()(NodeTypeDeclPtr node_type_decl,
                                          String eventout_id) const
        {
            using std::find_if;
            using std::bind2nd;
            node_interface_set::const_iterator pos =
                find_if(node_type_decl->second->second.begin(),
                        node_type_decl->second->second.end(),
                        bind2nd(node_interface_matches_eventout(),
                                eventout_id));
            return (pos == node_type_decl->second->second.end())
                ? 0
                : &(*pos);
        }
    };

    const phoenix::function<get_route_eventout_function> get_route_eventout;

    struct get_route_eventin_function {
        template <typename NodeTypeDeclPtr, typename String>
        struct result {
            typedef const node_interface * type;
        };

        template <typename NodeTypeDeclPtr, typename String>
        const node_interface * operator()(NodeTypeDeclPtr node_type_decl,
                                          String eventin_id) const
        {
            using std::find_if;
            using std::bind2nd;
            const node_interface_set::const_iterator pos =
                find_if(node_type_decl->second->second.begin(),
                        node_type_decl->second->second.end(),
                        bind2nd(node_interface_matches_eventin(),
                                eventin_id));
            return (pos == node_type_decl->second->second.end())
                ? 0
                : &(*pos);
        }
    };

    const phoenix::function<get_route_eventin_function> get_route_eventin;

    struct check_consistent_value_type_function {
        template <typename FromNodeInterfacePtr,
                  typename ToNodeInterfacePtr>
        struct result {
            typedef bool type;
        };

        template <typename FromNodeInterfacePtr,
                  typename ToNodeInterfacePtr>
        bool operator()(FromNodeInterfacePtr from_interface,
                        ToNodeInterfacePtr to_interface) const
        {
            return from_interface->field_type == to_interface->field_type;
        }
    };

    const phoenix::function<check_consistent_value_type_function>
        check_consistent_value_type;

    struct is_script_node_function
    {
        template <typename NodeType>
        struct result {
            typedef bool type;
        };

        template <typename NodeType>
        bool operator()(NodeType node_type) const
        {
            return node_type->first == "Script";
        }
    };

    const phoenix::function<is_script_node_function> is_script_node;


    struct null_vrml97_parse_actions {

        struct on_scene_start_t {
            void operator()() const
            {}
        } on_scene_start;

        struct on_scene_finish_t {
            void operator()() const
            {}
        } on_scene_finish;

        struct on_externproto_t {
            void operator()(const std::string & /* node_type_id */,
                            const node_interface_set & /* interfaces */,
                            const std::vector<std::string> & /* uri_list */) const
            {}
        } on_externproto;

        struct on_proto_start_t {
            void operator()(const std::string & /* node_type_id */) const {}
        } on_proto_start;

        struct on_proto_interface_t {
            void operator()(const node_interface & /* interface_ */) const {}
        } on_proto_interface;

        struct on_proto_default_value_start_t {
            void operator()() const {}
        } on_proto_default_value_start;

        struct on_proto_default_value_finish_t {
            void operator()() const {}
        } on_proto_default_value_finish;

        struct on_proto_body_start_t {
            void operator()() const {}
        } on_proto_body_start;

        struct on_proto_finish_t {
            void operator()() const {}
        } on_proto_finish;

        struct on_node_start_t {
            void operator()(const std::string & /* node_name_id */,
                            const std::string & /* node_type_id */) const {}
        } on_node_start;

        struct on_node_finish_t {
            void operator()() const {}
        } on_node_finish;

        struct on_script_interface_decl_t {
            void operator()(const node_interface &) const {}
        } on_script_interface_decl;

        struct on_route_t {
            void operator()(const std::string & /* from_node_name_id */,
                            const node_interface & /* from_node_interface */,
                            const std::string & /* to_node_name_id */,
                            const node_interface & /* to_node_interface */) const
            {}
        } on_route;

        struct on_use_t {
            void operator()(const std::string & /* node_name_id */) const
            {}
        } on_use;

        struct on_is_mapping_t {
            void operator()(const std::string & /* proto_interface_id */) const
            {}
        } on_is_mapping;

        struct on_field_start_t {
            void operator()(const std::string & /* field_name_id */,
                            field_value::type_id /* field_type */) const {}
        } on_field_start;

        struct on_field_finish_t {
            void operator()() const {}
        } on_field_finish;

        struct on_sfnode_t {
            void operator()(bool /* null */) const {}
        } on_sfnode;

        struct on_sfbool_t {
            void operator()(bool) const {}
        } on_sfbool;

        struct on_sfcolor_t {
            void operator()(const color &) const {}
        } on_sfcolor;

        struct on_sffloat_t {
            void operator()(float) const {}
        } on_sffloat;

        struct on_sfimage_t {
            void operator()(const image &) const {}
        } on_sfimage;

        struct on_sfint32_t {
            void operator()(int32) const {}
        } on_sfint32;

        struct on_sfrotation_t {
            void operator()(const rotation &) const {}
        } on_sfrotation;

        struct on_sfstring_t {
            void operator()(const std::string &) const {}
        } on_sfstring;

        struct on_sftime_t {
            void operator()(double) const {}
        } on_sftime;

        struct on_sfvec2f_t {
            void operator()(const vec2f &) const {}
        } on_sfvec2f;

        struct on_sfvec3f_t {
            void operator()(const vec3f &) const {}
        } on_sfvec3f;

        struct on_mfcolor_t {
            void operator()(const std::vector<color> &) const {}
        } on_mfcolor;

        struct on_mffloat_t {
            void operator()(const std::vector<float> &) const {}
        } on_mffloat;

        struct on_mfint32_t {
            void operator()(const std::vector<int32> &) const {}
        } on_mfint32;

        struct on_mfnode_t {
            void operator()() const {}
        } on_mfnode;

        struct on_mfrotation_t {
            void operator()(const std::vector<rotation> &) const {}
        } on_mfrotation;

        struct on_mfstring_t {
            void operator()(const std::vector<std::string> &) const {}
        } on_mfstring;

        struct on_mftime_t {
            void operator()(const std::vector<double> &) const {}
        } on_mftime;

        struct on_mfvec2f_t {
            void operator()(const std::vector<vec2f> &) const {}
        } on_mfvec2f;

        struct on_mfvec3f_t {
            void operator()(const std::vector<vec3f> &) const {}
        } on_mfvec3f;
    };

    template <typename Actions = null_vrml97_parse_actions,
              typename ErrorHandler = vrml97_parse_error_handler>
    struct vrml97_grammar :
        boost::spirit::grammar<vrml97_grammar<Actions, ErrorHandler> > {

        const boost::spirit::functor_parser<rotation_parser<ErrorHandler> >
            rotation_p;

        template <typename ScannerT>
        struct definition {

            typedef boost::spirit::rule<ScannerT> rule_type;

            typedef boost::spirit::rule<ScannerT, sfnode_closure::context_t>
                sfnode_rule_type;

            typedef boost::spirit::rule<ScannerT,
                                        interface_type_closure::context_t>
                interface_type_rule_type;

            typedef boost::spirit::rule<ScannerT,
                                        field_type_closure::context_t>
                field_type_rule_type;

            typedef boost::spirit::rule<ScannerT,
                                        interface_decl_closure::context_t>
                interface_decl_rule_type;

            typedef boost::spirit::rule<ScannerT,
                                        script_interface_closure::context_t>
                script_interface_rule_type;

            typedef boost::spirit::rule<ScannerT, proto_closure::context_t>
                proto_rule_type;

            typedef boost::spirit::rule<ScannerT,
                                        interface_decl_closure::context_t>
                proto_interface_rule_type;

            typedef boost::spirit::rule<ScannerT, is_mapping_closure::context_t>
                is_mapping_rule_type;

            typedef boost::spirit::rule<ScannerT,
                                        externproto_closure::context_t>
                externproto_rule_type;

            typedef boost::spirit::rule<ScannerT,
                                        root_node_statement_closure::context_t>
                root_node_statement_rule_type;

            typedef boost::spirit::rule<ScannerT,
                                        route_statement_closure::context_t>
                route_statement_rule_type;

            typedef boost::spirit::stored_rule<ScannerT> field_value_rule_type;


            template <typename ElementParser>
            struct mftype_parser {
                typedef std::vector<typename ElementParser::template result<ScannerT>::type::attr_t >
                    result_t;

                explicit mftype_parser(const ElementParser & parser):
                    parser_(parser)
                {}

                template <typename MFTypeParserScannerT>
                std::ptrdiff_t operator()(const MFTypeParserScannerT & scan,
                                          result_t & result) const
                {
                    using boost::spirit::ch_p;
                    using boost::spirit::push_back_a;
                    using boost::spirit::match_result;
                    typedef typename boost::spirit::rule<MFTypeParserScannerT>
                        rule_t;
                    typedef typename match_result<MFTypeParserScannerT,
                                                  result_t>::type
                        match_t;

                    static const vrml_parse_assertion
                        expect_element_or_lbracket(
                            get_mftype_parse_error<ElementParser>::element_or_lbracket_value),
                        expect_element_or_rbracket(
                            get_mftype_parse_error<ElementParser>::element_or_rbracket_value);

                    rule_t rule
                        =   this->parser_[push_back_a(result)]
                        |   expect_element_or_lbracket(ch_p('['))
                            >> *(this->parser_[push_back_a(result)])
                            >> expect_element_or_rbracket(ch_p(']'))
                        ;
                    match_t match = rule.parse(scan);
                    return match.length();
                }

            private:
                const ElementParser & parser_;
            };

            static const boost::spirit::functor_parser<
                mftype_parser<
                    boost::spirit::functor_parser<openvrml::color_parser> > >
                mfcolor_p;

            static const boost::spirit::functor_parser<
                mftype_parser<
                    boost::spirit::real_parser<
                        float,
                        boost::spirit::real_parser_policies<float> > > >
                mffloat_p;

            static const boost::spirit::functor_parser<
                mftype_parser<
                    boost::spirit::functor_parser<openvrml::int32_parser> > >
                mfint32_p;

            static const boost::spirit::functor_parser<
                mftype_parser<
                    boost::spirit::functor_parser<openvrml::string_parser> > >
                mfstring_p;

            const boost::spirit::functor_parser<
                mftype_parser<
                    boost::spirit::functor_parser<
                        openvrml::rotation_parser<ErrorHandler> > > >
                mfrotation_p;

            static const boost::spirit::functor_parser<
                mftype_parser<
                    boost::spirit::real_parser<
                        double,
                        boost::spirit::real_parser_policies<double> > > >
                mftime_p;

            static const boost::spirit::functor_parser<
                mftype_parser<
                    boost::spirit::functor_parser<openvrml::vec2f_parser> > >
                mfvec2f_p;

            static const boost::spirit::functor_parser<
                mftype_parser<
                    boost::spirit::functor_parser<openvrml::vec3f_parser> > >
                mfvec3f_p;


            struct mfnode_parser {
                typedef boost::spirit::nil_t result_t;

                explicit mfnode_parser(const rule_type & node_statement):
                    node_statement_(node_statement)
                {}

                std::ptrdiff_t operator()(const ScannerT & scan,
                                          result_t &) const
                {
                    using boost::spirit::ch_p;
                    using boost::spirit::match_result;
                    typedef typename boost::spirit::rule<ScannerT> rule_t;
                    typedef typename match_result<ScannerT, result_t>::type
                        match_t;

                    static const vrml_parse_assertion
                        expect_node_or_lbracket(node_or_lbracket_expected),
                        expect_node_or_rbracket(node_or_rbracket_expected);

                    rule_t rule
                        =   this->node_statement_
                        |   expect_node_or_lbracket(ch_p('['))
                            >> *(this->node_statement_)
                            >> expect_node_or_rbracket(ch_p(']'))
                        ;
                    match_t match = rule.parse(scan);
                    return match.length();
                }

            private:
                const rule_type & node_statement_;
            };

            const boost::spirit::functor_parser<mfnode_parser> mfnode_p;


            scope_stack_t scope_stack;

            virtual const boost::spirit::stored_rule<ScannerT>
            get_field_value_parser(const field_value::type_id field_type) const
            {
                boost::spirit::stored_rule<ScannerT> r;
                switch (field_type) {
                case field_value::sfbool_id:
                    r = expect_bool(bool_p)[this->self.actions.on_sfbool];
                    break;
                case field_value::sfcolor_id:
                    r = expect_color(color_p)[this->self.actions.on_sfcolor];
                    break;
                case field_value::sffloat_id:
                    r = expect_float(float_p)[this->self.actions.on_sffloat];
                    break;
                case field_value::sfimage_id:
                    r = expect_int32(image_p)[this->self.actions.on_sfimage];
                    break;
                case field_value::sfint32_id:
                    r = expect_int32(int32_p)[this->self.actions.on_sfint32];
                    break;
                case field_value::sfnode_id:
                    r = this->sfnode(false)[this->self.actions.on_sfnode];
                    break;
                case field_value::sfrotation_id:
                    r = expect_rotation(this->self.rotation_p)[
                            this->self.actions.on_sfrotation
                        ];
                    break;
                case field_value::sfstring_id:
                    r = expect_string(string_p)[this->self.actions.on_sfstring];
                    break;
                case field_value::sftime_id:
                    r = expect_float(boost::spirit::real_p)[
                            this->self.actions.on_sftime
                        ];
                    break;
                case field_value::sfvec2f_id:
                    r = expect_vec2(vec2f_p)[this->self.actions.on_sfvec2f];
                    break;
                case field_value::sfvec3f_id:
                    r = expect_vec3(vec3f_p)[this->self.actions.on_sfvec3f];
                    break;
                case field_value::mfcolor_id:
                    r = mfcolor_p[this->self.actions.on_mfcolor];
                    break;
                case field_value::mffloat_id:
                    r = mffloat_p[this->self.actions.on_mffloat];
                    break;
                case field_value::mfint32_id:
                    r = mfint32_p[this->self.actions.on_mfint32];
                    break;
                case field_value::mfnode_id:
                    r = mfnode_p[this->on_mfnode];
                    break;
                case field_value::mfrotation_id:
                    r = mfrotation_p[this->self.actions.on_mfrotation];
                    break;
                case field_value::mfstring_id:
                    r = mfstring_p[this->self.actions.on_mfstring];
                    break;
                case field_value::mftime_id:
                    r = mftime_p[this->self.actions.on_mftime];
                    break;
                case field_value::mfvec2f_id:
                    r = mfvec2f_p[this->self.actions.on_mfvec2f];
                    break;
                case field_value::mfvec3f_id:
                    r = mfvec3f_p[this->self.actions.on_mfvec3f];
                    break;
                default:
                    assert(false);
                }
                return r;
            }


            struct add_script_interface_function {
                template <typename NodeType, typename NodeInterface>
                struct result {
                    typedef bool type;
                };

                typedef typename vrml97_grammar<Actions, ErrorHandler>::template definition<ScannerT>
                    vrml97_grammar_def_t;

                explicit add_script_interface_function(vrml97_grammar_def_t & d):
                    vrml97_grammar_def_(d)
                {}

                template <typename NodeType, typename NodeInterface>
                bool operator()(NodeType node_type,
                                NodeInterface interface_) const
                {
                    vrml97_grammar_def_t & d = this->vrml97_grammar_def_;

                    const bool succeeded =
                        node_type->second.insert(interface_).second;
                    if (succeeded) {
                        d.field_value =
                            (interface_.type == node_interface::eventin_id
                             || interface_.type == node_interface::eventout_id)
                            ? boost::spirit::eps_p
                            : d.get_field_value_parser(interface_.field_type);

                        if (in_proto_def(d.scope_stack)) {
                            d.field_value
                                =   d.is_mapping(interface_)
                                |   d.field_value.copy()
                                ;
                        }
                    }
                    return succeeded;
                }

            private:
                vrml97_grammar_def_t &  vrml97_grammar_def_;
            };

            const phoenix::function<add_script_interface_function>
                add_script_interface;


            struct node_name_id_parser {
                typedef const defs_t::value_type * result_t;

                struct get_node_type {
                    get_node_type(
                        const defs_t::value_type * & node_type,
                        const scope_stack_t & scope_stack):
                        node_type_(node_type),
                        scope_stack_(scope_stack)
                    {}

                    template <typename IteratorT>
                    void operator()(const IteratorT & first,
                                    const IteratorT & last) const
                    {
                        const std::string node_name_id(first, last);
                        const defs_t::const_iterator pos =
                            this->scope_stack_.top().defs.find(node_name_id);
                        if (pos == this->scope_stack_.top().defs.end()) {
                            throw_(first, unknown_node_name_id);
                        }
                        this->node_type_ = &(*pos);
                    }

                private:
                    const defs_t::value_type * & node_type_;
                    const scope_stack_t & scope_stack_;
                };

                explicit node_name_id_parser(const scope_stack_t & scope_stack,
                                             rule_type & id):
                    scope_stack_(scope_stack),
                    id_(id)
                {}

                std::ptrdiff_t operator()(const ScannerT & scan,
                                          result_t & result) const
                {
                    using boost::spirit::assertion;
                    using boost::spirit::eps_p;
                    using boost::spirit::rule;
                    using boost::spirit::match_result;
                    typedef rule<ScannerT> rule_t;
                    typedef typename match_result<ScannerT, result_t>::type
                        match_t;

                    assertion<vrml_parse_error> expect_id(id_expected);

                    rule_t node_name_id
                        =   expect_id(id_)[
                                get_node_type(result, this->scope_stack_)
                            ]
                        ;

                    BOOST_SPIRIT_DEBUG_NODE(node_name_id);

                    match_t match = node_name_id.parse(scan);
                    return match.length();
                }

            private:
                const scope_stack_t & scope_stack_;
                rule_type & id_;
            };

            const boost::spirit::functor_parser<node_name_id_parser>
                node_name_id_p;


            struct node_type_id_parser {
                typedef node_type_decls::value_type * result_t;

                struct get_node_type {
                    get_node_type(node_type_decls::value_type * & node_type,
                                  scope_stack_t & scope_stack):
                        node_type_(node_type),
                        scope_stack_(scope_stack)
                    {}

                    template <typename IteratorT>
                    void operator()(const IteratorT & first,
                                    const IteratorT & last) const
                    {
                        const std::string node_type_id(first, last);
                        if (node_type_id == "Script") {
                            static const node_interface
                                script_node_interfaces[3] = {
                                node_interface(node_interface::field_id,
                                               field_value::sfbool_id,
                                               "directOutput"),
                                node_interface(node_interface::field_id,
                                               field_value::sfbool_id,
                                               "mustEvaluate"),
                                node_interface(node_interface::exposedfield_id,
                                               field_value::mfstring_id,
                                               "url")
                            };

                            this->node_type_ =
                                &(*this->scope_stack_.top().script_node_types
                                  .insert(make_pair("Script",
                                                    node_interface_set(script_node_interfaces,
                                                                       script_node_interfaces + 3))));
                            return;
                        }
                        this->node_type_ =
                            find_node_type(this->scope_stack_, node_type_id);
                        if (!this->node_type_) {
                            throw_(first, unknown_node_type_id);
                        }
                    }

                private:
                    node_type_decls::value_type * & node_type_;
                    scope_stack_t & scope_stack_;
                };

                explicit node_type_id_parser(scope_stack_t & scope_stack,
                                             rule_type & id):
                    scope_stack_(scope_stack),
                    id_(id)
                {}

                std::ptrdiff_t operator()(const ScannerT & scan,
                                          result_t & result) const
                {
                    using boost::spirit::eps_p;
                    using boost::spirit::rule;
                    using boost::spirit::match_result;
                    typedef rule<ScannerT> rule_t;
                    typedef typename match_result<ScannerT, result_t>::type
                        match_t;

                    rule_t node_type_id
                        =   id_[get_node_type(result, this->scope_stack_)]
                        ;

                    BOOST_SPIRIT_DEBUG_NODE(node_type_id);

                    match_t match = node_type_id.parse(scan);
                    return match.length();
                }

            private:
                scope_stack_t & scope_stack_;
                rule_type & id_;
            };

            const boost::spirit::functor_parser<node_type_id_parser>
                node_type_id_p;


            struct set_proto_decl_field_value_rule {
                typedef typename vrml97_grammar<Actions, ErrorHandler>::template definition<ScannerT>
                    vrml97_grammar_def_t;

                explicit set_proto_decl_field_value_rule(
                    vrml97_grammar_def_t & grammar_def):
                    vrml97_grammar_def_(grammar_def)
                {}

                void operator()(const node_interface & interface_) const
                {
                    using boost::spirit::eps_p;

                    vrml97_grammar_def_t & d = this->vrml97_grammar_def_;

                    if (interface_.type == node_interface::eventin_id
                        || interface_.type == node_interface::eventout_id) {
                        d.field_value = eps_p;
                    } else {
                        d.field_value
                            =   d.get_field_value_parser(
                                    interface_.field_type)
                            ;
                        d.field_value
                            =   eps_p[d.on_proto_default_value_start]
                                >> d.field_value.copy()
                            ;
                        d.field_value
                            =   d.field_value.copy()
                                >> eps_p[d.on_proto_default_value_finish]
                            ;
                            
                    }
                }

            private:
                vrml97_grammar_def_t & vrml97_grammar_def_;
            };


            struct add_to_node_body_repo {
                add_to_node_body_repo(scope_stack_t & scope_stack,
                                      bool & already_exists):
                    scope_stack_(scope_stack),
                    already_exists_(already_exists)
                {}

                void operator()(const node_type_decl & node_decl) const
                {
                    this->already_exists_ =
                        find_node_type(this->scope_stack_, node_decl.first);

                    if (this->already_exists_) { return; }

                    const bool succeeded =
                        this->scope_stack_.top().node_body_repo
                        .insert(node_decl).second;
                    assert(succeeded);
                }

            private:
                scope_stack_t & scope_stack_;
                bool & already_exists_;
            };

            template <typename NodeType>
            struct set_field_value_rule_t {
                typedef typename vrml97_grammar<Actions, ErrorHandler>::template definition<ScannerT>
                    vrml97_grammar_def_t;

                set_field_value_rule_t(NodeType & node_type,
                                       vrml97_grammar_def_t & grammar_def):
                    node_type_(node_type),
                    vrml97_grammar_def_(grammar_def)
                {}

                template <typename IteratorT>
                void operator()(const IteratorT & first,
                                const IteratorT & last) const
                {
                    using boost::spirit::throw_;

                    vrml97_grammar_def_t & d = this->vrml97_grammar_def_;

                    const std::string interface_id(first, last);

                    const node_type_decls::value_type * const node_type =
                        this->node_type_();
                    assert(node_type);

                    const typename node_interface_set::const_iterator pos =
                        find_interface(node_type->second, interface_id);
                    if (pos == node_type->second.end()) {
                        throw_(first, unknown_node_interface_id);
                    }

                    if (in_proto_def(d.scope_stack)) {
                        using std::equal;
                        static const std::string eventin_prefix = "set_";
                        static const std::string eventout_suffix = "_changed";
                        if (pos->type == node_interface::eventin_id
                            || pos->type == node_interface::eventout_id
                            || (pos->type == node_interface::exposedfield_id
                                && ((interface_id.size() > eventin_prefix.size()
                                     && equal(interface_id.begin(),
                                              interface_id.begin() + eventin_prefix.size(),
                                              eventin_prefix.begin()))
                                    || (interface_id.size() > eventout_suffix.size()
                                        && equal(interface_id.rbegin(),
                                                 interface_id.rbegin() + eventout_suffix.size(),
                                                 eventout_suffix.rbegin()))))) {
                            d.field_value
                                =   expect_is(d.is_mapping(*pos))
                                ;
                        } else {
                            d.field_value
                                =   d.get_field_value_parser(pos->field_type)
                                ;
                            d.field_value
                                =   d.is_mapping(*pos)
                                |   d.field_value.copy()
                                ;
                        }
                    } else {
                        d.field_value
                            =   d.get_field_value_parser(pos->field_type)
                            ;
                    }
                    d.self.actions.on_field_start(interface_id,
                                                   pos->field_type);
                }

            private:
                NodeType & node_type_;
                vrml97_grammar_def_t & vrml97_grammar_def_;
            };

            template <typename NodeType>
            static const set_field_value_rule_t<NodeType>
            set_field_value_rule(
                NodeType & node_type,
                typename set_field_value_rule_t<NodeType>::vrml97_grammar_def_t & grammar_def)
            {
                return set_field_value_rule_t<NodeType>(node_type,
                                                        grammar_def);
            }

            struct push_proto_scope_function {
                template <typename NodeType>
                struct result {
                    typedef void type;
                };

                explicit push_proto_scope_function(scope_stack_t & stack):
                    stack_(stack)
                {}

                template <typename NodeType>
                void operator()(const NodeType & node_type) const
                {
                    parse_scope s;
                    s.proto_node_type = &node_type;
                    this->stack_.push(s);
                }

            private:
                scope_stack_t & stack_;
            };

            const phoenix::function<push_proto_scope_function>
                push_proto_scope;

            struct push_scope {
                explicit push_scope(scope_stack_t & stack):
                    stack_(stack)
                {}

                template <typename IteratorT>
                void operator()(IteratorT, IteratorT) const
                {
                    this->stack_.push(parse_scope());
                }

            private:
                scope_stack_t & stack_;
            };

            struct pop_scope {
                explicit pop_scope(scope_stack_t & stack):
                    stack_(stack)
                {}

                template <typename IteratorT>
                void operator()(IteratorT, IteratorT) const
                {
                    this->stack_.pop();
                }

            private:
                scope_stack_t & stack_;
            };

            struct check_node_type_already_exists {
                explicit check_node_type_already_exists(bool & exists):
                    exists_(exists)
                {}

                template <typename IteratorT>
                void operator()(const IteratorT & first, IteratorT) const
                {
                    if (this->exists_) {
                        throw_(first, openvrml::node_type_already_exists);
                    }
                }

            private:
                bool & exists_;
            };

            struct on_scene_start_t {
                on_scene_start_t(scope_stack_t & scope_stack,
                                 const Actions & actions):
                    scope_stack(scope_stack),
                    actions(actions)
                {}

                template <typename IteratorT>
                void operator()(IteratorT, IteratorT) const
                {
                    std::auto_ptr<node_type_decls> node_types =
                        profile("VRML97");
                    this->scope_stack.top().node_body_repo = *node_types;
                    this->actions.on_scene_start();
                }

            private:
                scope_stack_t & scope_stack;
                const Actions & actions;
            } on_scene_start;

            struct on_scene_finish_t {
                explicit on_scene_finish_t(const Actions & actions):
                    actions(actions)
                {}

                template <typename IteratorT>
                void operator()(IteratorT, IteratorT) const
                {
                    actions.on_scene_finish();
                }

            private:
                const Actions & actions;
            } on_scene_finish;

            struct on_use_t {
                explicit on_use_t(const Actions & actions):
                    actions(actions)
                {}

                template <typename IteratorT>
                void operator()(IteratorT first, IteratorT last) const
                {
                    actions.on_use(std::string(first, last));
                }

            private:
                const Actions & actions;
            } on_use;

            struct on_proto_interface_function {
                template <typename NodeInterface>
                struct result {
                    typedef void type;
                };

                explicit on_proto_interface_function(const Actions & actions):
                    actions(actions)
                {}

                template <typename NodeInterface>
                void operator()(NodeInterface & interface_) const
                {
                    actions.on_proto_interface(interface_);
                }

            private:
                const Actions & actions;
            };

            const phoenix::function<on_proto_interface_function>
                on_proto_interface;


            struct on_proto_default_value_start_t {
                explicit on_proto_default_value_start_t(
                    const Actions & actions):
                    actions(actions)
                {}

                template <typename IteratorT>
                void operator()(IteratorT, IteratorT) const
                {
                    actions.on_proto_default_value_start();
                }

            private:
                const Actions & actions;
            } on_proto_default_value_start;


            struct on_proto_default_value_finish_t {
                explicit on_proto_default_value_finish_t(
                    const Actions & actions):
                    actions(actions)
                {}

                template <typename IteratorT>
                void operator()(IteratorT, IteratorT) const
                {
                    actions.on_proto_default_value_finish();
                }

            private:
                const Actions & actions;
            } on_proto_default_value_finish;


            struct on_proto_finish_t {
                explicit on_proto_finish_t(const Actions & actions):
                    actions(actions)
                {}

                template <typename IteratorT>
                void operator()(IteratorT, IteratorT) const
                {
                    actions.on_proto_finish();
                }

            private:
                const Actions & actions;
            };

            const on_proto_finish_t on_proto_finish;


            struct on_externproto_function {
                template <typename NodeType, typename StringVector>
                struct result {
                    typedef void type;
                };

                explicit on_externproto_function(const Actions & actions):
                    actions(actions)
                {}

                template <typename NodeType, typename StringVector>
                void operator()(NodeType node_type,
                                StringVector uri_list) const
                {
                    actions.on_externproto(node_type.first,
                                           node_type.second,
                                           uri_list);
                }

            private:
                const Actions & actions;
            };

            const phoenix::function<on_externproto_function> on_externproto;


            struct push_field_value_rule {
                typedef typename vrml97_grammar<Actions, ErrorHandler>::template definition<ScannerT>
                    vrml97_grammar_def_t;

                explicit push_field_value_rule(
                    vrml97_grammar_def_t & grammar_def):
                    vrml97_grammar_def_(grammar_def)
                {}

                template <typename IteratorT>
                void operator()(IteratorT, IteratorT) const
                {
                    vrml97_grammar_def_t & d = this->vrml97_grammar_def_;
                    d.field_value_rule_stack.push(d.field_value);
                }

            private:
                vrml97_grammar_def_t & vrml97_grammar_def_;
            };


            struct pop_field_value_rule {
                typedef typename vrml97_grammar<Actions, ErrorHandler>::template definition<ScannerT>
                    vrml97_grammar_def_t;

                explicit pop_field_value_rule(
                    vrml97_grammar_def_t & grammar_def):
                    vrml97_grammar_def_(grammar_def)
                {}

                template <typename IteratorT>
                void operator()(IteratorT, IteratorT) const
                {
                    vrml97_grammar_def_t & d = this->vrml97_grammar_def_;
                    d.field_value = d.field_value_rule_stack.top().copy();
                    d.field_value_rule_stack.pop();
                }

            private:
                vrml97_grammar_def_t & vrml97_grammar_def_;
            };


            struct on_node_start_function {
                template <typename String, typename NodeType>
                struct result {
                    typedef void type;
                };

                explicit on_node_start_function(const Actions & actions):
                    actions(actions)
                {}

                template <typename String, typename NodeType>
                void operator()(String node_name_id, NodeType node_type) const
                {
                    actions.on_node_start(node_name_id, node_type->first);
                }

            private:
                const Actions & actions;
            };

            const phoenix::function<on_node_start_function> on_node_start;

            struct on_node_finish_t {
                explicit on_node_finish_t(const Actions & actions):
                    actions(actions)
                {}

                template <typename IteratorT>
                void operator()(IteratorT, IteratorT) const
                {
                    actions.on_node_finish();
                }

            private:
                const Actions & actions;
            };

            const on_node_finish_t on_node_finish;

            struct on_script_interface_decl_function {
                template <typename NodeInterface>
                struct result {
                    typedef void type;
                };

                explicit on_script_interface_decl_function(
                    const Actions & actions):
                    actions(actions)
                {}

                template <typename NodeInterface>
                void operator()(NodeInterface interface_) const
                {
                    this->actions.on_script_interface_decl(interface_);
                }

            private:
                const Actions & actions;
            };

            const phoenix::function<on_script_interface_decl_function>
                on_script_interface_decl;

            struct on_route_function {
                template <typename FromDefEntryPtr,
                          typename FromNodeInterfacePtr,
                          typename ToDefEntryPtr,
                          typename ToNodeInterfacePtr>
                struct result {
                    typedef void type;
                };

                explicit on_route_function(const Actions & actions):
                    actions(actions)
                {}

                template <typename FromDefEntryPtr,
                          typename FromNodeInterfacePtr,
                          typename ToDefEntryPtr,
                          typename ToNodeInterfacePtr>
                void operator()(FromDefEntryPtr from_node,
                                FromNodeInterfacePtr from_node_interface,
                                ToDefEntryPtr to_node,
                                ToNodeInterfacePtr to_node_interface) const
                {
                    actions.on_route(from_node->first,
                                     *from_node_interface,
                                     to_node->first,
                                     *to_node_interface);
                }

            private:
                const Actions & actions;
            };

            phoenix::function<on_route_function> on_route;

            struct on_proto_start_t {
                explicit on_proto_start_t(const Actions & actions):
                    actions(actions)
                {}

                template <typename IteratorT>
                void operator()(IteratorT first, IteratorT last) const
                {
                    actions.on_proto_start(std::string(first, last));
                }

            private:
                const Actions & actions;
            };

            const on_proto_start_t on_proto_start;

            struct on_proto_body_start_t {
                explicit on_proto_body_start_t(const Actions & actions):
                    actions(actions)
                {}

                template <typename CharT>
                void operator()(CharT) const
                {
                    actions.on_proto_body_start();
                }

            private:
                const Actions & actions;
            };

            const on_proto_body_start_t on_proto_body_start;

            struct get_is_mapping_function {
                template <typename NodeInterface, typename String>
                struct result {
                    typedef node_interface type;
                };

                explicit get_is_mapping_function(scope_stack_t & scope_stack):
                    scope_stack_(scope_stack)
                {}

                template <typename NodeInterface, typename String>
                const node_interface
                operator()(const NodeInterface & impl_interface,
                           const String & proto_interface_id) const
                {
                    const typename node_interface_set::const_iterator pos =
                        find_interface(
                            this->scope_stack_.top().proto_node_type->second,
                            proto_interface_id);

                    //
                    // An exposedField in the PROTO implementation can be IS'd
                    // to any type of interface; otherwise the interface types
                    // must agree.  See 4.8.3; particularly table 4.4.
                    //
                    if (pos != this->scope_stack_.top().proto_node_type->second.end()
                        && pos->field_type == impl_interface.field_type
                        && ((impl_interface.type
                             == node_interface::exposedfield_id)
                            || pos->type == impl_interface.type)) {
                        return *pos;
                    }

                    return node_interface();
                }

            private:
                scope_stack_t & scope_stack_;
            };

            const phoenix::function<get_is_mapping_function> get_is_mapping;


            struct on_is_mapping_function {
                template <typename NodeInterface>
                struct result {
                    typedef void type;
                };

                explicit on_is_mapping_function(const Actions & actions):
                    actions(actions)
                {}

                template <typename NodeInterface>
                void operator()(NodeInterface interface_) const
                {
                    actions.on_is_mapping(interface_.id);
                }

            private:
                const Actions & actions;
            };

            const phoenix::function<on_is_mapping_function> on_is_mapping;

            struct on_mfnode_t {
                explicit on_mfnode_t(const Actions & actions):
                    actions(actions)
                {}

                template <typename IteratorT>
                void operator()(IteratorT, IteratorT) const
                {
                    actions.on_mfnode();
                }

            private:
                const Actions & actions;
            };

            const on_mfnode_t on_mfnode;

            const vrml97_grammar<Actions, ErrorHandler> & self;

            std::string node_type_id;
            bool node_type_already_exists;

            boost::spirit::symbols<typename ScannerT::value_t> keywords;

            rule_type vrml_scene;
            rule_type statement;
            rule_type proto_statement;
            sfnode_rule_type sfnode;
            proto_rule_type proto;
            proto_interface_rule_type proto_interface;
            externproto_rule_type externproto;
            rule_type node_statement;
            root_node_statement_rule_type root_node_statement;
            rule_type node;
            rule_type node_body_element;
            interface_decl_rule_type interface_decl;
            interface_decl_rule_type script_interface_decl;
            script_interface_rule_type script_interface;
            field_value_rule_type field_value;
            std::stack<field_value_rule_type> field_value_rule_stack;
            is_mapping_rule_type is_mapping;
            route_statement_rule_type route_statement;
            rule_type id;
            interface_type_rule_type interface_type;
            interface_type_rule_type script_interface_type;
            field_type_rule_type field_type;
            rule_type eventin;
            rule_type eventout;
            rule_type exposedfield;
            rule_type field;

            typedef boost::spirit::chset<typename ScannerT::value_t> chset;
            chset invalid_id_rest_char, invalid_id_first_char;

            explicit definition(const vrml97_grammar & self);
            virtual ~definition() {}

            const boost::spirit::rule<ScannerT> & start() const;
        };

        explicit vrml97_grammar(
            const Actions & actions = Actions(),
            const ErrorHandler & handler = ErrorHandler()):
            rotation_p(rotation_parser<ErrorHandler>(handler)),
            actions(actions),
            error_handler(handler)
        {}

        const Actions & actions;
        const ErrorHandler & error_handler;
    };

    template <typename Actions, typename ErrorHandler>
    template <typename ScannerT>
    const boost::spirit::functor_parser<
        typename vrml97_grammar<Actions, ErrorHandler>::template definition<ScannerT>::
        template mftype_parser<boost::spirit::functor_parser<openvrml::color_parser> > >
    vrml97_grammar<Actions, ErrorHandler>::definition<ScannerT>::mfcolor_p =
        typename vrml97_grammar<Actions, ErrorHandler>::template definition<ScannerT>::
        template mftype_parser<
            boost::spirit::functor_parser<openvrml::color_parser>
         >(color_p);

    template <typename Actions, typename ErrorHandler>
    template <typename ScannerT>
    const boost::spirit::functor_parser<
        typename vrml97_grammar<Actions, ErrorHandler>::template definition<ScannerT>::
        template mftype_parser<
            boost::spirit::real_parser<
                float,
                boost::spirit::real_parser_policies<float> > > >
    vrml97_grammar<Actions, ErrorHandler>::definition<ScannerT>::mffloat_p =
        typename vrml97_grammar<Actions, ErrorHandler>::template definition<ScannerT>::
        template mftype_parser<
            boost::spirit::real_parser<
               float,
               boost::spirit::real_parser_policies<float>
            >
        >(float_p);

    template <typename Actions, typename ErrorHandler>
    template <typename ScannerT>
    const boost::spirit::functor_parser<
        typename vrml97_grammar<Actions, ErrorHandler>::template definition<ScannerT>::
        template mftype_parser<
            boost::spirit::real_parser<
                double,
                boost::spirit::real_parser_policies<double> > > >
    vrml97_grammar<Actions, ErrorHandler>::definition<ScannerT>::mftime_p =
        typename vrml97_grammar<Actions, ErrorHandler>::template definition<ScannerT>::
        template mftype_parser<
            boost::spirit::real_parser<
               double,
               boost::spirit::real_parser_policies<double>
            >
        >(boost::spirit::real_p);

    template <typename Actions, typename ErrorHandler>
    template <typename ScannerT>
    const boost::spirit::functor_parser<
        typename vrml97_grammar<Actions, ErrorHandler>::template definition<ScannerT>::
        template mftype_parser<boost::spirit::functor_parser<int32_parser> > >
    vrml97_grammar<Actions, ErrorHandler>::definition<ScannerT>::mfint32_p =
        typename vrml97_grammar<Actions, ErrorHandler>::template definition<ScannerT>::
        template mftype_parser< boost::spirit::functor_parser<int32_parser> >(
            int32_p);

    template <typename Actions, typename ErrorHandler>
    template <typename ScannerT>
    const boost::spirit::functor_parser<
        typename vrml97_grammar<Actions, ErrorHandler>::template definition<ScannerT>::
        template mftype_parser<boost::spirit::functor_parser<string_parser> > >
    vrml97_grammar<Actions, ErrorHandler>::definition<ScannerT>::mfstring_p =
        typename vrml97_grammar<Actions, ErrorHandler>::template definition<ScannerT>::
        template mftype_parser<boost::spirit::functor_parser<string_parser> >(
            string_p);

    template <typename Actions, typename ErrorHandler>
    template <typename ScannerT>
    const boost::spirit::functor_parser<
        typename vrml97_grammar<Actions, ErrorHandler>::template definition<ScannerT>::
        template mftype_parser<boost::spirit::functor_parser<vec2f_parser> > >
    vrml97_grammar<Actions, ErrorHandler>::definition<ScannerT>::mfvec2f_p =
        typename vrml97_grammar<Actions, ErrorHandler>::template definition<ScannerT>::
        template mftype_parser<boost::spirit::functor_parser<vec2f_parser> >(
            vec2f_p);

    template <typename Actions, typename ErrorHandler>
    template <typename ScannerT>
    const boost::spirit::functor_parser<
        typename vrml97_grammar<Actions, ErrorHandler>::template definition<ScannerT>::
        template mftype_parser<boost::spirit::functor_parser<vec3f_parser> > >
    vrml97_grammar<Actions, ErrorHandler>::definition<ScannerT>::mfvec3f_p =
        typename vrml97_grammar<Actions, ErrorHandler>::template definition<ScannerT>::
        template mftype_parser<boost::spirit::functor_parser<vec3f_parser> >(
            vec3f_p);

    template <typename Actions, typename ErrorHandler>
    template <typename ScannerT>
    vrml97_grammar<Actions, ErrorHandler>::definition<ScannerT>::
    definition(const vrml97_grammar & self):
        mfrotation_p(
            mftype_parser<
                boost::spirit::functor_parser<
                    openvrml::rotation_parser<ErrorHandler>
                >
            >(self.rotation_p)),
        mfnode_p(mfnode_parser(this->node_statement)),
        add_script_interface(add_script_interface_function(*this)),
        node_name_id_p(node_name_id_parser(this->scope_stack, this->id)),
        node_type_id_p(node_type_id_parser(this->scope_stack, this->id)),
        push_proto_scope(push_proto_scope_function(this->scope_stack)),
        on_scene_start(this->scope_stack, self.actions),
        on_scene_finish(self.actions),
        on_use(self.actions),
        on_proto_interface(on_proto_interface_function(self.actions)),
        on_proto_default_value_start(self.actions),
        on_proto_default_value_finish(self.actions),
        on_proto_finish(self.actions),
        on_externproto(on_externproto_function(self.actions)),
        on_node_start(on_node_start_function(self.actions)),
        on_node_finish(self.actions),
        on_script_interface_decl(
            on_script_interface_decl_function(self.actions)),
        on_route(on_route_function(self.actions)),
        on_proto_start(self.actions),
        on_proto_body_start(self.actions),
        get_is_mapping(get_is_mapping_function(this->scope_stack)),
        on_is_mapping(on_is_mapping_function(self.actions)),
        on_mfnode(self.actions),
        self(self),
        node_type_already_exists(false),
        invalid_id_rest_char("\x01-\x20\x22\x23\x27,.[\\]{}\x7f"),
        invalid_id_first_char(boost::spirit::chset_p("1234567890+-")
                              | invalid_id_rest_char)
    {
        using std::string;
        using namespace boost::spirit;
        using namespace phoenix;
        BOOST_SPIRIT_DEBUG_NODE(vrml_scene);
        BOOST_SPIRIT_DEBUG_NODE(statement);
        BOOST_SPIRIT_DEBUG_NODE(proto_statement);
        BOOST_SPIRIT_DEBUG_NODE(proto);
        BOOST_SPIRIT_DEBUG_NODE(proto_interface);
        BOOST_SPIRIT_DEBUG_NODE(interface_decl);
        BOOST_SPIRIT_DEBUG_NODE(script_interface_decl);
        BOOST_SPIRIT_DEBUG_NODE(externproto);
        BOOST_SPIRIT_DEBUG_NODE(route_statement);
        BOOST_SPIRIT_DEBUG_NODE(sfnode);
        BOOST_SPIRIT_DEBUG_NODE(node_statement);
        BOOST_SPIRIT_DEBUG_NODE(root_node_statement);
        BOOST_SPIRIT_DEBUG_NODE(node);
        BOOST_SPIRIT_DEBUG_NODE(node_body_element);
        BOOST_SPIRIT_DEBUG_NODE(script_interface);
        BOOST_SPIRIT_DEBUG_NODE(is_mapping);
        BOOST_SPIRIT_DEBUG_NODE(field_value);
        BOOST_SPIRIT_DEBUG_NODE(id);

        this->scope_stack.push(parse_scope());

        guard<vrml_parse_error> g;

        keywords =
            "DEF", "EXTERNPROTO", "FALSE", "IS", "NULL", "PROTO",
            "ROUTE", "TO", "TRUE", "USE", "eventIn", "eventOut",
            "field", "exposedField";

        vrml_scene
            =  g(
                    eps_p[on_scene_start] >> *statement
                    >> eps_p[on_scene_finish]
                )[self.error_handler]
            ;

        statement
            =   proto_statement
            |   route_statement
            |   node_statement
            ;

        proto_statement
            =   (   proto[
                        add_to_node_body_repo(this->scope_stack,
                                              this->node_type_already_exists)
                    ]
                    >> eps_p
                )[
                    check_node_type_already_exists(
                        this->node_type_already_exists)
                ]
            |   (   externproto[
                        add_to_node_body_repo(this->scope_stack,
                                              this->node_type_already_exists)
                    ]
                    >> eps_p
                )[
                    check_node_type_already_exists(
                        this->node_type_already_exists)
                ]
            ;

        proto
            =   "PROTO"
                >>  expect_id(id)[
                    set_node_type_id(proto.node_type,
                                     construct_<string>(arg1, arg2))
                ]
                [push_proto_scope(proto.node_type)]
                [on_proto_start]
                >> expect_lbracket(ch_p('['))
                >> eps_p[push_field_value_rule(*this)]
                >> *proto_interface
                >> (']' | expect_interface_type_or_rbracket(nothing_p))
                >> eps_p[pop_field_value_rule(*this)]
                >> expect_lbrace(ch_p('{'))[on_proto_body_start]
                >> *proto_statement
                >> expect_node(root_node_statement)
                >> *statement
                >> '}'
                >> eps_p[pop_scope(this->scope_stack)]
                >> eps_p[on_proto_finish]
            ;

        proto_interface
            =   (   interface_decl[
                        proto_interface.interface_ = arg1
                    ][
                        proto.add_node_interface_succeeded =
                            add_node_interface(proto.node_type, arg1)
                    ][
                        set_proto_decl_field_value_rule(*this)
                    ]
                    >>  expect_unique_node_interface(
                            eps_p(
                                check(proto.add_node_interface_succeeded)
                            )
                        )
                )[on_proto_interface(proto_interface.interface_)]
                 [push_scope(this->scope_stack)]
                >> field_value
                >> eps_p[pop_scope(this->scope_stack)]
            ;

        interface_decl
            =   interface_type[
                    set_node_interface_type(interface_decl.interface_,
                                            arg1)
                ]
                >>  expect_field_type(field_type)[
                    set_node_interface_field_type(interface_decl.interface_,
                                                  arg1)
                ]
                >>  expect_id(id)[
                    set_node_interface_id(interface_decl.interface_,
                                          construct_<string>(arg1, arg2))
                ]
            ;

        script_interface_decl
            =   script_interface_type[
                    set_node_interface_type(script_interface_decl.interface_,
                                            arg1)
                ]
                >>  expect_field_type(field_type)[
                    set_node_interface_field_type(
                        script_interface_decl.interface_,
                        arg1)
                ]
                >>  expect_id(id)[
                    set_node_interface_id(script_interface_decl.interface_,
                                          construct_<string>(arg1, arg2))
                ]
            ;

        externproto
            =   str_p("EXTERNPROTO")
                >>  expect_id(id)[
                    set_node_type_id(externproto.node_type,
                                     construct_<string>(arg1, arg2))
                ]
                >> expect_lbracket(ch_p('['))
                >> *(   interface_decl[
                            externproto.add_node_interface_succeeded =
                                add_node_interface(externproto.node_type, arg1)
                        ]
                        >>  expect_unique_node_interface(
                                eps_p(
                                    check(externproto.add_node_interface_succeeded)
                                )
                            )
                    )
                >> (']' | expect_interface_type_or_rbracket(nothing_p))
                >> mfstring_p[externproto.url_list = arg1]
                >> eps_p[on_externproto(externproto.node_type,
                                        externproto.url_list)]
            ;

        route_statement
            =   "ROUTE"
                >>  node_name_id_p[route_statement.from_node = arg1]
                >>  expect_dot(ch_p('.'))
                >>  expect_id(id)[
                        route_statement.from_node_interface =
                            get_route_eventout(route_statement.from_node,
                                               construct_<string>(arg1, arg2))
                    ]
                >>  expect_eventout_id(
                        eps_p(check(route_statement.from_node_interface))
                    )
                >>  expect_to(str_p("TO"))
                >>  node_name_id_p[route_statement.to_node = arg1]
                >>  expect_dot(ch_p('.'))
                >>  expect_id(id)[
                        route_statement.to_node_interface =
                            get_route_eventin(route_statement.to_node,
                                              construct_<string>(arg1, arg2))
                    ]
                >>  expect_eventin_id(
                        eps_p(check(route_statement.to_node_interface))
                    )
                >>  expect_eventin_value_type_matches_eventout(
                        eps_p(
                            check_consistent_value_type(
                                route_statement.from_node_interface,
                                route_statement.to_node_interface
                            )
                        )
                    )
                >>  eps_p[on_route(route_statement.from_node,
                                   route_statement.from_node_interface,
                                   route_statement.to_node,
                                   route_statement.to_node_interface)]
            ;

        sfnode
            =   node_statement
            |   str_p("NULL")[sfnode.null = true]
            ;

        node_statement
            =   root_node_statement
            |   "USE" >> (node_name_id_p >> eps_p)[on_use]
            ;

        root_node_statement
            =  !(   "DEF" >> expect_id(id)[
                        root_node_statement.node_name_id =
                            construct_<string>(arg1, arg2)
                    ]
                )
                >> node
            ;

        node
            =   (node_type_id_p[root_node_statement.node_type = arg1] >> eps_p)
                [
                    add_def(this->scope_stack,
                            root_node_statement.node_name_id,
                            root_node_statement.node_type)
                ][
                    push_field_value_rule(*this)
                ]
                >> expect_lbrace(ch_p('{'))[
                    on_node_start(root_node_statement.node_name_id,
                                  root_node_statement.node_type)
                ]
                >> *node_body_element
                >>  (   '}'
                    |   eps_p(is_script_node(root_node_statement.node_type))
                        >> expect_script_interface_or_field_or_prototype_or_route_or_rbrace(
                            nothing_p)
                    |   expect_field_or_prototype_or_route_or_rbrace(nothing_p)
                    )
                >> eps_p[pop_field_value_rule(*this)][on_node_finish]
            ;

        node_body_element
            =   eps_p(is_script_node(root_node_statement.node_type))
                >> script_interface
            |   id[set_field_value_rule(root_node_statement.node_type, *this)]
                >> field_value
            |   route_statement
            |   proto_statement
            ;

        script_interface
            =   script_interface_decl[script_interface.interface_ = arg1]
                >>  eps_p[
                        // add_script_interface sets field_value as a
                        // side-effect.
                        script_interface.add_interface_succeeded =
                            add_script_interface(root_node_statement.node_type,
                                                 script_interface.interface_)
                    ]
                >>  eps_p(check(script_interface.add_interface_succeeded))
                >>  eps_p[on_script_interface_decl(script_interface.interface_)]
                >>  field_value
            ;

        is_mapping
            =   "IS"
                >>  expect_id(id)[
                        is_mapping.proto_interface =
                            get_is_mapping(is_mapping.impl_interface,
                                           construct_<string>(arg1, arg2))
                    ]
                >>  expect_compatible_proto_interface(
                         eps_p(
                             check_valid_proto_interface(
                                 is_mapping.proto_interface
                             )
                         )
                    )
                >>  eps_p[on_is_mapping(is_mapping.proto_interface)]
            ;

        id
            =   lexeme_d[
                    ((anychar_p - vrml97_space_p - invalid_id_first_char)
                     >> *(anychar_p - vrml97_space_p - invalid_id_rest_char))
                    - (keywords >> (anychar_p - (anychar_p - vrml97_space_p
                                                 - invalid_id_rest_char)))
                ]
            ;

        interface_type
            =   eventin[interface_type.type = node_interface::eventin_id]
            |   eventout[interface_type.type = node_interface::eventout_id]
            |   exposedfield[
                    interface_type.type = node_interface::exposedfield_id
                ]
            |   field[interface_type.type = node_interface::field_id]
            ;

        script_interface_type
            =   eventin[
                    script_interface_type.type = node_interface::eventin_id
                ]
            |   eventout[
                    script_interface_type.type = node_interface::eventout_id
                ]
            |   field[
                    script_interface_type.type = node_interface::field_id
                ]
            ;

        eventin
            =   str_p("eventIn")
            ;

        eventout
            =   str_p("eventOut")
            ;

        exposedfield
            =   str_p("exposedField")
            ;

        field
            =   str_p("field")
            ;

        field_type
            =   str_p("MFColor")[field_type.type = field_value::mfcolor_id]
            |   str_p("MFFloat")[field_type.type = field_value::mffloat_id]
            |   str_p("MFInt32")[field_type.type = field_value::mfint32_id]
            |   str_p("MFNode")[field_type.type = field_value::mfnode_id]
            |   str_p("MFRotation")[field_type.type = field_value::mfrotation_id]
            |   str_p("MFString")[field_type.type = field_value::mfstring_id]
            |   str_p("MFTime")[field_type.type = field_value::mftime_id]
            |   str_p("MFVec2f")[field_type.type = field_value::mfvec2f_id]
            |   str_p("MFVec3f")[field_type.type = field_value::mfvec3f_id]
            |   str_p("SFBool")[field_type.type = field_value::sfbool_id]
            |   str_p("SFColor")[field_type.type = field_value::sfcolor_id]
            |   str_p("SFFloat")[field_type.type = field_value::sffloat_id]
            |   str_p("SFImage")[field_type.type = field_value::sfimage_id]
            |   str_p("SFInt32")[field_type.type = field_value::sfint32_id]
            |   str_p("SFNode")[field_type.type = field_value::sfnode_id]
            |   str_p("SFRotation")[field_type.type = field_value::sfrotation_id]
            |   str_p("SFString")[field_type.type = field_value::sfstring_id]
            |   str_p("SFTime")[field_type.type = field_value::sftime_id]
            |   str_p("SFVec2f")[field_type.type = field_value::sfvec2f_id]
            |   str_p("SFVec3f")[field_type.type = field_value::sfvec3f_id]
            ;
    }

    template <typename Actions, typename ErrorHandler>
    template <typename ScannerT>
    const boost::spirit::rule<ScannerT> &
    vrml97_grammar<Actions, ErrorHandler>::definition<ScannerT>::start() const
    {
        return this->vrml_scene;
    }
} // namespace openvrml

#   ifdef BOOST_SPIRIT_DEBUG
namespace std {

    template <typename C, typename E>
    std::basic_ostream<C, E> &
    operator<<(std::basic_ostream<C, E> & out,
               const openvrml::node_type_decl & node_type)
    {
        return out << node_type.first;
    }
}
#   endif

# endif // ifndef OPENVRML_VRML97_GRAMMAR_H
