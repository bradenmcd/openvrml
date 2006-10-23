// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; -*-
//
// OpenVRML
//
// Copyright 2001, 2002, 2003, 2004  Braden McDaniel
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

# ifndef OPENVRML_PRIVATE_H
#   define OPENVRML_PRIVATE_H

#   include <openvrml-common.h>
#   include <boost/concept_check.hpp>
#   include <boost/spirit.hpp>
#   include <boost/spirit/phoenix.hpp>

#   ifndef NDEBUG
#     include <iostream>
#   endif

#   ifdef NDEBUG
#     define OPENVRML_VERIFY_(condition_)
#   else
#     define OPENVRML_VERIFY_(condition_) \
            do { \
                if (!(condition_)) { \
                    std::cerr << __FILE__ << ":" << __LINE__ \
                              << ": verification (" #condition_ ") failed" \
                              << std::endl; \
                } \
            } while (false)
#   endif

#   ifdef NDEBUG
#     define OPENVRML_PRINT_EXCEPTION_(ex_) \
            boost::ignore_unused_variable_warning(ex_)
#   else
#     define OPENVRML_PRINT_EXCEPTION_(ex_) \
            std::cerr << __FILE__ << ":" << __LINE__ << ": " \
                      << ex_.what() << std::endl
#   endif

#   ifdef NDEBUG
#     define OPENVRML_PRINT_MESSAGE_(message_)
#   else
#     define OPENVRML_PRINT_MESSAGE_(message_) \
            std::cerr << __FILE__ << ":" << __LINE__ << ": " \
                      << message_ << std::endl
#   endif

#   include <cassert>
#   include <cmath>
#   include <cstddef>
#   include <functional>
#   include <limits>
#   include <boost/concept_check.hpp>
#   include <boost/mpl/vector.hpp>

namespace openvrml {
    class sfbool;
    class sfcolor;
    class sfcolorrgba;
    class sfdouble;
    class sffloat;
    class sfimage;
    class sfint32;
    class sfnode;
    class sfrotation;
    class sfstring;
    class sftime;
    class sfvec2d;
    class sfvec2f;
    class sfvec3d;
    class sfvec3f;
    class mfbool;
    class mfcolor;
    class mfcolorrgba;
    class mfdouble;
    class mffloat;
    class mfimage;
    class mfint32;
    class mfnode;
    class mfrotation;
    class mfstring;
    class mftime;
    class mfvec2d;
    class mfvec2f;
    class mfvec3d;
    class mfvec3f;
}

namespace {
    namespace openvrml_ {

        const double pi     = 3.14159265358979323846;
        const double pi_2   = 1.57079632679489661923;
        const double pi_4   = 0.78539816339744830962;
        const double inv_pi = 0.31830988618379067154;

        template <typename Float>
        OPENVRML_LOCAL inline Float fabs(const Float f)
        {
            return f < 0.0 ? -f : f;
        }

        struct OPENVRML_LOCAL fequal_t {
            template <typename Float>
            bool operator()(Float a, Float b) const
            {
                const Float diff = fabs(a - b);
                if (diff == 0.0) { return true; }
                static const int rounding_errors = 10;
                static const Float e = std::numeric_limits<Float>::epsilon();
                static const Float tolerance = e * rounding_errors / 2;
                return diff / fabs(a) <= tolerance
                    && diff / fabs(b) <= tolerance;
            }
        };

        const fequal_t fequal = fequal_t();

        struct OPENVRML_LOCAL fless_equal_t {
            template <typename Float>
            bool operator()(Float a, Float b) const
            {
                return a < b || fequal(a, b);
            }
        };

        const fless_equal_t fless_equal = fless_equal_t();

        struct OPENVRML_LOCAL fgreater_equal_t {
            template <typename Float>
            bool operator()(Float a, Float b) const
            {
                return a > b || fequal(a, b);
            }
        };

        const fgreater_equal_t fgreater_equal = fgreater_equal_t();


        class OPENVRML_LOCAL scope_guard_impl_base {
        protected:
            mutable bool dismissed;

        public:
            void dismiss() const throw ();

        protected:
            scope_guard_impl_base() throw ();
            scope_guard_impl_base(const scope_guard_impl_base &) throw ();

        private:
            // Disable assignment.
            scope_guard_impl_base & operator=(const scope_guard_impl_base &);
        };

        typedef const scope_guard_impl_base & scope_guard;

        inline scope_guard_impl_base::scope_guard_impl_base() throw ():
            dismissed(false)
        {}

        inline scope_guard_impl_base::scope_guard_impl_base(
            const scope_guard_impl_base & scope_guard) throw ():
            dismissed(scope_guard.dismissed)
        {
            scope_guard.dismiss();
        }

        inline void scope_guard_impl_base::dismiss() const throw ()
        {
            this->dismissed = true;
        }

        template <typename Function, typename Param>
        class OPENVRML_LOCAL scope_guard_impl1 : public scope_guard_impl_base {
            Function function;
            const Param param;

        public:
            scope_guard_impl1(const Function & function, const Param & param);
            ~scope_guard_impl1();
        };

        template <typename Function, typename Param>
        scope_guard_impl1<Function, Param>::
        scope_guard_impl1(const Function & function, const Param & param):
            function(function),
            param(param)
        {}

        template <typename Function, typename Param>
        scope_guard_impl1<Function, Param>::~scope_guard_impl1()
        {
            if (!this->dismissed) { this->function(this->param); }
        }

        template <typename Function, typename Param>
        OPENVRML_LOCAL scope_guard_impl1<Function, Param>
        make_guard(const Function & function, const Param & param)
        {
            return scope_guard_impl1<Function, Param>(function, param);
        }

        template <typename Function,
                  typename Param1,
                  typename Param2,
                  typename Param3>
        class OPENVRML_LOCAL scope_guard_impl3 : public scope_guard_impl_base {
            Function function;
            const Param1 param1;
            const Param2 param2;
            const Param3 param3;

        public:
            scope_guard_impl3(const Function & function,
                              const Param1 & param1,
                              const Param2 & param2,
                              const Param3 & param3);
            ~scope_guard_impl3();
        };

        template <typename Function,
                  typename Param1,
                  typename Param2,
                  typename Param3>
        scope_guard_impl3<Function, Param1, Param2, Param3>::
        scope_guard_impl3(const Function & function,
                          const Param1 & param1,
                          const Param2 & param2,
                          const Param3 & param3):
            function(function),
            param1(param1),
            param2(param2),
            param3(param3)
        {}

        template <typename Function,
                  typename Param1,
                  typename Param2,
                  typename Param3>
        scope_guard_impl3<Function, Param1, Param2, Param3>::
        ~scope_guard_impl3()
        {
            if (!this->dismissed) {
                this->function(this->param1, this->param2, this->param3);
            }
        }

        template <typename Function,
                  typename Param1,
                  typename Param2,
                  typename Param3>
        OPENVRML_LOCAL scope_guard_impl3<Function, Param1, Param2, Param3>
        make_guard(const Function & function,
                   const Param1 & param1,
                   const Param2 & param2,
                   const Param3 & param3)
        {
            return scope_guard_impl3<Function, Param1, Param2, Param3>(
                function, param1, param2, param3);
        }

        template <typename Object, typename MemberFunction>
        class OPENVRML_LOCAL obj_scope_guard_impl0 :
            public scope_guard_impl_base {
            Object & obj;
            MemberFunction mem_fun;

        public:
            obj_scope_guard_impl0(Object & obj, MemberFunction mem_fun);
            ~obj_scope_guard_impl0();
        };

        template <typename Object, typename MemberFunction>
        inline
        obj_scope_guard_impl0<Object, MemberFunction>::
        obj_scope_guard_impl0(Object & obj, MemberFunction mem_fun):
            obj(obj),
            mem_fun(mem_fun)
        {}

        template <typename Object, typename MemberFunction>
        inline
        obj_scope_guard_impl0<Object, MemberFunction>::~obj_scope_guard_impl0()
        {
            if (!this->dismissed) { (this->obj.*this->mem_fun)(); }
        }

        template <typename Object, typename MemberFunction>
        OPENVRML_LOCAL obj_scope_guard_impl0<Object, MemberFunction>
        make_obj_guard(Object & obj, MemberFunction mem_fun)
        {
            return obj_scope_guard_impl0<Object, MemberFunction>(obj, mem_fun);
        }

        template <typename Object, typename MemberFunction, typename Param>
        class OPENVRML_LOCAL obj_scope_guard_impl1 :
            public scope_guard_impl_base {
            Object & obj;
            MemberFunction mem_fun;
            const Param param;

        public:
            obj_scope_guard_impl1(Object & obj,
                                  MemberFunction mem_fun,
                                  const Param & param) throw ();
            ~obj_scope_guard_impl1() throw ();
        };

        template <typename Object, typename MemberFunction, typename Param>
        inline
        obj_scope_guard_impl1<Object, MemberFunction, Param>::
        obj_scope_guard_impl1(Object & obj,
                              MemberFunction mem_fun,
                              const Param & param)
            throw ():
            obj(obj),
            mem_fun(mem_fun),
            param(param)
        {}

        template <typename Object, typename MemberFunction, typename Param>
        inline
        obj_scope_guard_impl1<Object, MemberFunction, Param>::
        ~obj_scope_guard_impl1() throw ()
        {
            if (!this->dismissed) { (this->obj.*this->mem_fun)(this->param); }
        }

        template <typename Object, typename MemberFunction, typename Param>
        OPENVRML_LOCAL obj_scope_guard_impl1<Object, MemberFunction, Param>
        make_obj_guard(Object & obj,
                       MemberFunction mem_fun,
                       const Param & param)
        {
            return obj_scope_guard_impl1<Object, MemberFunction, Param>(
                    obj, mem_fun, param);
        }


        typedef boost::mpl::vector<openvrml::sfbool,
                                   openvrml::sfcolor,
                                   openvrml::sfcolorrgba,
                                   openvrml::sfdouble,
                                   openvrml::sffloat,
                                   openvrml::sfimage,
                                   openvrml::sfint32,
                                   openvrml::sfnode,
                                   openvrml::sfrotation,
                                   openvrml::sfstring,
                                   openvrml::sftime,
                                   openvrml::sfvec2d,
                                   openvrml::sfvec2f,
                                   openvrml::sfvec3d,
                                   openvrml::sfvec3f,
                                   openvrml::mfbool,
                                   openvrml::mfcolor,
                                   openvrml::mfcolorrgba,
                                   openvrml::mfdouble,
                                   openvrml::mffloat,
                                   openvrml::mfimage,
                                   openvrml::mfint32,
                                   openvrml::mfnode,
                                   openvrml::mfrotation,
                                   openvrml::mfstring,
                                   openvrml::mftime,
                                   openvrml::mfvec2d,
                                   openvrml::mfvec2f,
                                   openvrml::mfvec3d,
                                   openvrml::mfvec3f>
        field_value_types;


        template <typename SpiritActor, typename Iterator1, typename Iterator2>
        class assign_iterators_base {
            SpiritActor actor_;
            Iterator1 begin_;
            Iterator2 end_;

        public:
            typedef assign_iterators_base<SpiritActor, Iterator1, Iterator2>
            this_type;

            assign_iterators_base(const SpiritActor & actor,
                                  const Iterator1 & begin,
                                  const Iterator2 & end):
                actor_(actor),
                begin_(begin),
                end_(end)
                {}

            template <typename T>
            struct result {
                typedef void type;
            };

            template <typename Tuple>
            typename phoenix::actor_result<this_type, Tuple>::type
            eval(Tuple) const
                {
                    this->actor_(this->begin_(), this->end_());
                }
        };

        template <typename SpiritActor, typename Iterator1, typename Iterator2>
        phoenix::actor<assign_iterators_base<SpiritActor, Iterator1, Iterator2> >
        assign_iterators(const SpiritActor & actor,
                         const Iterator1 & begin,
                         const Iterator2 & end)
        {
            return assign_iterators_base<SpiritActor, Iterator1, Iterator2>(actor,
                                                                            begin,
                                                                            end);
        }

        class OPENVRML_LOCAL null_actions {
        public:
            struct null_action {
                template <typename Iterator>
                void operator()(const Iterator &, const Iterator &) const
                    {}
            };

            null_action scheme, scheme_specific_part, userinfo, host, port,
                authority, path, query, fragment;
        };


        struct OPENVRML_LOCAL uri_reserved_parser :
            public boost::spirit::char_parser<uri_reserved_parser> {

            typedef uri_reserved_parser self_t;

            template <typename CharT>
                bool test(CharT ch) const
            {
                return ch == ';'
                    || ch == '/'
                    || ch == '?'
                    || ch == ':'
                    || ch == '@'
                    || ch == '&'
                    || ch == '='
                    || ch == '+'
                    || ch == '$'
                    || ch == ',';
            }
        };

        const uri_reserved_parser uri_reserved_p = uri_reserved_parser();


        struct OPENVRML_LOCAL uri_unreserved_parser :
            public boost::spirit::char_parser<uri_unreserved_parser> {

            typedef uri_unreserved_parser self_t;

            template <typename CharT>
            bool test(CharT ch) const
            {
                using namespace std;
                return isalnum(char_traits<CharT>::to_int_type(ch))
                    || ch == '-'
                    || ch == '_'
                    || ch == '.'
                    || ch == '!'
                    || ch == '~'
                    || ch == '*'
                    || ch == '\''
                    || ch == '('
                    || ch == ')';
            }
        };

        const uri_unreserved_parser uri_unreserved_p = uri_unreserved_parser();


        struct OPENVRML_LOCAL uric_grammar :
            public boost::spirit::grammar<uric_grammar> {

            template <typename ScannerT>
                struct definition {
                typedef boost::spirit::rule<ScannerT> rule_type;

                rule_type uric, escaped;

                definition(const uric_grammar & self);

                const boost::spirit::rule<ScannerT> & start() const;
            };
        };

        template <typename ScannerT>
        uric_grammar::definition<ScannerT>::definition(const uric_grammar &)
        {
            using namespace boost::spirit;

            uric
                =   uri_reserved_p
                |   uri_unreserved_p
                |   escaped
                ;

            escaped
                =   '%' >> xdigit_p >> xdigit_p
                ;
        }

        template <typename ScannerT>
        const boost::spirit::rule<ScannerT> &
        uric_grammar::definition<ScannerT>::start() const
        {
            return this->uric;
        }


        template <typename Actions = null_actions>
        struct uri_authority_grammar :
            public boost::spirit::grammar<uri_authority_grammar<Actions> > {

            template <typename ScannerT>
            struct definition {
                struct server_closure :
                    boost::spirit::closure<server_closure,
                                           typename ScannerT::iterator_t,
                                           typename ScannerT::iterator_t> {
                    typename server_closure::member1 userinfo_begin;
                    typename server_closure::member2 userinfo_end;
                };

                typedef boost::spirit::rule<ScannerT> rule_type;
                typedef boost::spirit::rule<ScannerT,
                                            typename server_closure::context_t>
                server_rule_type;

                rule_type authority;
                rule_type reg_name;
                server_rule_type server;
                rule_type userinfo;
                rule_type hostport;
                rule_type host;
                rule_type hostname;
                rule_type domainlabel;
                rule_type toplabel;
                rule_type ipv4address;
                rule_type port;
                rule_type escaped;

                explicit definition(const uri_authority_grammar & self);

                const boost::spirit::rule<ScannerT> & start() const;
            };

            const Actions & actions;

            explicit uri_authority_grammar(const Actions & actions = Actions());
        };

        template <typename Actions>
        template <typename ScannerT>
        uri_authority_grammar<Actions>::definition<ScannerT>::
        definition(const uri_authority_grammar & self)
        {
            using namespace boost::spirit;
            using namespace phoenix;

            authority
                =   (server | reg_name)[ self.actions.authority ]
                ;

            reg_name
                =  +(   uri_unreserved_p
                        |   escaped
                        |   '$'
                        |   ','
                        |   ';'
                        |   ':'
                        |   '@'
                        |   '&'
                        |   '='
                        |   '+'
                    )
                ;

            server
                =  !(
                    !(
                        userinfo[
                            server.userinfo_begin = arg1,
                            server.userinfo_end = arg2
                            ] >> '@'
                        )[
                            assign_iterators(self.actions.userinfo,
                                             server.userinfo_begin,
                                             server.userinfo_end)
                            ]
                    >> hostport
                    )
                ;

            userinfo
                =  *(   uri_unreserved_p
                        |   escaped
                        |   ';'
                        |   ':'
                        |   '&'
                        |   '='
                        |   '+'
                        |   '$'
                        |   ','
                    )
                ;

            hostport
                =   host >> !(':' >> port)
                ;

            host
                =   (hostname | ipv4address)[ self.actions.host ]
                ;

            hostname
                =   *(domainlabel >> '.') >> toplabel >> !ch_p('.')
                ;

            domainlabel
                =   alnum_p >> *(*ch_p('-') >> alnum_p)
                ;

            toplabel
                =   alpha_p >> *(*ch_p('-') >> alnum_p)
                ;

            ipv4address
                =   +digit_p >> '.' >> +digit_p >> '.' >> +digit_p >> '.'
                             >> +digit_p
                ;

            port
                =   (*digit_p)[ self.actions.port ]
                ;

            escaped
                =   '%' >> xdigit_p >> xdigit_p
                ;
        }

        template <typename Actions>
        template <typename ScannerT>
        const boost::spirit::rule<ScannerT> &
        uri_authority_grammar<Actions>::definition<ScannerT>::start() const
        {
            return this->authority;
        }

        template <typename Actions>
        uri_authority_grammar<Actions>::
        uri_authority_grammar(const Actions & actions):
            actions(actions)
        {}

        template <typename Actions>
        struct uri_abs_path_grammar :
            public boost::spirit::grammar<uri_abs_path_grammar<Actions> > {

            template <typename ScannerT>
            struct definition {
                typedef boost::spirit::rule<ScannerT> rule_type;

                rule_type abs_path;
                rule_type path_segments;
                rule_type segment;
                rule_type param;
                rule_type pchar;
                rule_type query;
                rule_type escaped;

                explicit definition(const uri_abs_path_grammar & self);

                const boost::spirit::rule<ScannerT> & start() const;
            };

            const Actions & actions;

            explicit uri_abs_path_grammar(const Actions & actions = Actions());
        };

        template <typename Actions>
        template <typename ScannerT>
        uri_abs_path_grammar<Actions>::definition<ScannerT>::
        definition(const uri_abs_path_grammar & self)
        {
            using namespace boost::spirit;
            using namespace phoenix;

            abs_path
                =   ('/' >> path_segments)[ self.actions.path ]
                ;

            path_segments
                =   segment >> *('/' >> segment)
                ;

            segment
                =   *pchar >> *(';' >> param)
                ;

            param
                =   *pchar
                ;

            pchar
                =   uri_unreserved_p
                |   escaped
                |   ':'
                |   '@'
                |   '&'
                |   '='
                |   '+'
                |   '$'
                |   ','
                ;

            escaped
                =   '%' >> xdigit_p >> xdigit_p
                ;
        }

        template <typename Actions>
        template <typename ScannerT>
        const boost::spirit::rule<ScannerT> &
        uri_abs_path_grammar<Actions>::definition<ScannerT>::start() const
        {
            return this->abs_path;
        }

        template <typename Actions>
        uri_abs_path_grammar<Actions>::
        uri_abs_path_grammar(const Actions & actions):
            actions(actions)
        {}


        template <typename Actions = null_actions>
        struct absolute_uri_grammar :
            public boost::spirit::grammar<absolute_uri_grammar<Actions> > {

            template <typename ScannerT>
            struct definition {
                struct absolute_uri_closure :
                    boost::spirit::closure<absolute_uri_closure,
                                           typename ScannerT::iterator_t,
                                           typename ScannerT::iterator_t> {
                    typename absolute_uri_closure::member1 scheme_begin;
                    typename absolute_uri_closure::member2 scheme_end;
                };

                struct server_closure :
                    boost::spirit::closure<server_closure,
                                           typename ScannerT::iterator_t,
                                           typename ScannerT::iterator_t> {
                    typename server_closure::member1 userinfo_begin;
                    typename server_closure::member2 userinfo_end;
                };

                typedef boost::spirit::rule<ScannerT> rule_type;
                typedef boost::spirit::rule<
                    ScannerT,
                    typename absolute_uri_closure::context_t>
                absolute_uri_rule_type;
                typedef boost::spirit::rule<ScannerT,
                                            typename server_closure::context_t>
                server_rule_type;

                absolute_uri_rule_type absolute_uri;
                rule_type scheme;
                rule_type hier_part;
                rule_type opaque_part;
                rule_type net_path;
                uri_abs_path_grammar<Actions> abs_path;
                uri_authority_grammar<Actions> authority;
                rule_type query;
                rule_type uric_no_slash;
                uric_grammar uric;

                explicit definition(const absolute_uri_grammar & self);

                const absolute_uri_rule_type & start() const;
            };

            const Actions & actions;

            explicit absolute_uri_grammar(const Actions & actions = Actions());
        };

        template <typename Actions>
        template <typename ScannerT>
        absolute_uri_grammar<Actions>::definition<ScannerT>::
        definition(const absolute_uri_grammar & self):
            abs_path(self.actions),
            authority(self.actions)
        {
            using namespace boost::spirit;
            using namespace phoenix;

            absolute_uri
                =   (
                    scheme[
                        absolute_uri.scheme_begin = arg1,
                        absolute_uri.scheme_end = arg2
                        ] >> ':'
                    )[
                        assign_iterators(self.actions.scheme,
                                         absolute_uri.scheme_begin,
                                         absolute_uri.scheme_end)
                        ] >> (hier_part | opaque_part)[
                            self.actions.scheme_specific_part
                            ]
                ;

            scheme
                =   (alpha_p >> *(alpha_p | digit_p | '+' | '-' | '.'))
                ;

            hier_part
                =   (net_path | abs_path) >> !('?' >> query)
                ;

            opaque_part
                =   uric_no_slash >> *uric
                ;

            uric_no_slash
                =   uric - '/'
                ;

            net_path
                =   "//" >> authority >> !abs_path
                ;

            query
                =   (*uric)[ self.actions.query ]
                ;

        }

        template <typename Actions>
        template <typename ScannerT>
        const typename absolute_uri_grammar<Actions>::
        template definition<ScannerT>::absolute_uri_rule_type &
        absolute_uri_grammar<Actions>::definition<ScannerT>::start() const
        {
            return this->absolute_uri;
        }

        template <typename Actions>
        absolute_uri_grammar<Actions>::
        absolute_uri_grammar(const Actions & actions):
            actions(actions)
        {}


        template <typename Actions = null_actions>
        struct uri_grammar : public boost::spirit::grammar<uri_grammar<Actions> > {

            template <typename ScannerT>
            struct definition {
                typedef boost::spirit::rule<ScannerT> rule_type;

                rule_type uri_reference;
                absolute_uri_grammar<Actions> absolute_uri;
                rule_type relative_uri;
                rule_type net_path;
                uri_abs_path_grammar<Actions> abs_path;
                rule_type rel_path;
                rule_type rel_segment;
                uri_authority_grammar<Actions> authority;
                rule_type query;
                rule_type fragment;
                uric_grammar uric;
                rule_type escaped;

                explicit definition(const uri_grammar & self);

                const boost::spirit::rule<ScannerT> & start() const;
            };

            const Actions & actions;

            explicit uri_grammar(const Actions & actions = Actions());
        };


        template <typename Actions>
        uri_grammar<Actions>::uri_grammar(const Actions & actions):
            actions(actions)
        {}

        template <typename Actions>
        template <typename ScannerT>
        uri_grammar<Actions>::definition<ScannerT>::
        definition(const uri_grammar & self):
            absolute_uri(self.actions),
            abs_path(self.actions),
            authority(self.actions)
        {
            using namespace boost::spirit;
            using namespace phoenix;

            BOOST_SPIRIT_DEBUG_NODE(uri_reference);
            BOOST_SPIRIT_DEBUG_NODE(absolute_uri);
            BOOST_SPIRIT_DEBUG_NODE(net_path);
            BOOST_SPIRIT_DEBUG_NODE(abs_path);
            BOOST_SPIRIT_DEBUG_NODE(rel_path);
            BOOST_SPIRIT_DEBUG_NODE(rel_segment);
            BOOST_SPIRIT_DEBUG_NODE(authority);
            BOOST_SPIRIT_DEBUG_NODE(query);
            BOOST_SPIRIT_DEBUG_NODE(fragment);
            BOOST_SPIRIT_DEBUG_NODE(uric);
            BOOST_SPIRIT_DEBUG_NODE(escaped);

            uri_reference
                =   !(absolute_uri | relative_uri) >> !('#' >> fragment)
                ;
            relative_uri
                =   (net_path | abs_path | rel_path) >> !('?' >> query)
                ;

            net_path
                =   "//" >> authority >> !abs_path
                ;

            rel_path
                =   (rel_segment >> !abs_path)[ self.actions.path ]
                ;

            rel_segment
                =  +(   uri_unreserved_p
                        |   escaped
                        |   ';'
                        |   '@'
                        |   '&'
                        |   '='
                        |   '+'
                        |   '$'
                        |   ','
                    )
                ;

            query
                =   (*uric)[ self.actions.query ]
                ;

            fragment
                =   (*uric)[ self.actions.fragment ]
                ;

            escaped
                =   '%' >> xdigit_p >> xdigit_p
                ;
        }

        template <typename Actions>
        template <typename ScannerT>
        const boost::spirit::rule<ScannerT> &
        uri_grammar<Actions>::definition<ScannerT>::start() const
        {
            return this->uri_reference;
        }
    }
}

# endif
