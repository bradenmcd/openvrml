// -*- mode: c++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML
//
// Copyright 2008  Braden McDaniel
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

# ifndef OPENVRML_LOCAL_URI_H
#   define OPENVRML_LOCAL_URI_H

#   include <openvrml/bad_url.h>
#   include <boost/spirit/include/classic.hpp>
#   include <boost/spirit/include/phoenix1.hpp>

namespace openvrml {

    namespace local {

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
        phoenix::actor<assign_iterators_base<SpiritActor,
                                             Iterator1,
                                             Iterator2> >
        assign_iterators(const SpiritActor & actor,
                         const Iterator1 & begin,
                         const Iterator2 & end)
        {
            return assign_iterators_base<SpiritActor, Iterator1, Iterator2>(
                actor,
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
            public boost::spirit::classic::char_parser<uri_reserved_parser> {

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
            public boost::spirit::classic::char_parser<uri_unreserved_parser> {

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
            public boost::spirit::classic::grammar<uric_grammar> {

            template <typename ScannerT>
                struct definition {
                typedef boost::spirit::classic::rule<ScannerT> rule_type;

                rule_type uric, escaped;

                definition(const uric_grammar & self);

                const boost::spirit::classic::rule<ScannerT> & start() const;
            };
        };

        template <typename ScannerT>
        uric_grammar::definition<ScannerT>::definition(const uric_grammar &)
        {
            using namespace boost::spirit::classic;

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
        const boost::spirit::classic::rule<ScannerT> &
        uric_grammar::definition<ScannerT>::start() const
        {
            return this->uric;
        }


        template <typename Actions = null_actions>
        struct uri_authority_grammar :
            public boost::spirit::classic::grammar<uri_authority_grammar<Actions> > {

            template <typename ScannerT>
            struct definition {
                struct server_closure :
                    boost::spirit::classic::closure<server_closure,
                                           typename ScannerT::iterator_t,
                                           typename ScannerT::iterator_t> {
                    typename server_closure::member1 userinfo_begin;
                    typename server_closure::member2 userinfo_end;
                };

                typedef boost::spirit::classic::rule<ScannerT> rule_type;
                typedef boost::spirit::classic::rule<ScannerT,
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

                const boost::spirit::classic::rule<ScannerT> & start() const;
            };

            const Actions & actions;

            explicit uri_authority_grammar(const Actions & actions = Actions());
        };

        template <typename Actions>
        template <typename ScannerT>
        uri_authority_grammar<Actions>::definition<ScannerT>::
        definition(const uri_authority_grammar & self)
        {
            using namespace boost::spirit::classic;
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
        const boost::spirit::classic::rule<ScannerT> &
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
            public boost::spirit::classic::grammar<uri_abs_path_grammar<Actions> > {

            template <typename ScannerT>
            struct definition {
                typedef boost::spirit::classic::rule<ScannerT> rule_type;

                rule_type abs_path;
                rule_type path_segments;
                rule_type segment;
                rule_type param;
                rule_type pchar;
                rule_type query;
                rule_type escaped;

                explicit definition(const uri_abs_path_grammar & self);

                const boost::spirit::classic::rule<ScannerT> & start() const;
            };

            const Actions & actions;

            explicit uri_abs_path_grammar(const Actions & actions = Actions());
        };

        template <typename Actions>
        template <typename ScannerT>
        uri_abs_path_grammar<Actions>::definition<ScannerT>::
        definition(const uri_abs_path_grammar & self)
        {
            using namespace boost::spirit::classic;
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
        const boost::spirit::classic::rule<ScannerT> &
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
            public boost::spirit::classic::grammar<absolute_uri_grammar<Actions> > {

            template <typename ScannerT>
            struct definition {
                struct absolute_uri_closure :
                    boost::spirit::classic::closure<absolute_uri_closure,
                                           typename ScannerT::iterator_t,
                                           typename ScannerT::iterator_t> {
                    typename absolute_uri_closure::member1 scheme_begin;
                    typename absolute_uri_closure::member2 scheme_end;
                };

                struct server_closure :
                    boost::spirit::classic::closure<server_closure,
                                           typename ScannerT::iterator_t,
                                           typename ScannerT::iterator_t> {
                    typename server_closure::member1 userinfo_begin;
                    typename server_closure::member2 userinfo_end;
                };

                typedef boost::spirit::classic::rule<ScannerT> rule_type;
                typedef boost::spirit::classic::rule<
                    ScannerT,
                    typename absolute_uri_closure::context_t>
                absolute_uri_rule_type;
                typedef boost::spirit::classic::rule<ScannerT,
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
            using namespace boost::spirit::classic;
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
        struct uri_grammar :
            public boost::spirit::classic::grammar<uri_grammar<Actions> > {

            template <typename ScannerT>
            struct definition {
                typedef boost::spirit::classic::rule<ScannerT> rule_type;

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

                const boost::spirit::classic::rule<ScannerT> & start() const;
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
            using namespace boost::spirit::classic;
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
        const boost::spirit::classic::rule<ScannerT> &
        uri_grammar<Actions>::definition<ScannerT>::start() const
        {
            return this->uri_reference;
        }


        class OPENVRML_LOCAL uri {
            class actions {
                uri * uri_;

            public:
                explicit actions(uri & uri_):
                    uri_(&uri_),
                    scheme(*this),
                    scheme_specific_part(*this),
                    authority(*this),
                    userinfo(*this),
                    host(*this),
                    port(*this),
                    path(*this),
                    query(*this),
                    fragment(*this)
                {}

                struct scheme_action {
                    explicit scheme_action(actions & actions_):
                        actions_(&actions_)
                    {}

                    template <typename Iterator>
                    void operator()(const Iterator & first,
                                    const Iterator & last) const
                    {
                        this->actions_->uri_->scheme_begin = first;
                        this->actions_->uri_->scheme_end = last;
                    }

                private:
                    actions * actions_;
                } scheme;

                struct scheme_specific_part_action {
                    explicit scheme_specific_part_action(actions & actions_):
                        actions_(&actions_)
                    {}

                    template <typename Iterator>
                    void operator()(const Iterator & first,
                                    const Iterator & last) const
                    {
                        this->actions_->uri_->scheme_specific_part_begin =
                            first;
                        this->actions_->uri_->scheme_specific_part_end = last;
                    }

                private:
                    actions * actions_;
                } scheme_specific_part;

                struct authority_action {
                    explicit authority_action(actions & actions_):
                        actions_(&actions_)
                    {}

                    template <typename Iterator>
                    void operator()(const Iterator & first,
                                    const Iterator & last) const
                    {
                        this->actions_->uri_->authority_begin = first;
                        this->actions_->uri_->authority_end = last;
                    }

                private:
                    actions * actions_;
                } authority;

                struct userinfo_action {
                    explicit userinfo_action(actions & actions_):
                        actions_(&actions_)
                    {}

                    template <typename Iterator>
                    void operator()(const Iterator & first,
                                    const Iterator & last) const
                    {
                        this->actions_->uri_->userinfo_begin = first;
                        this->actions_->uri_->userinfo_end = last;
                    }

                private:
                    actions * actions_;
                } userinfo;

                struct host_action {
                    explicit host_action(actions & actions_):
                        actions_(&actions_)
                    {}

                    template <typename Iterator>
                    void operator()(const Iterator & first,
                                    const Iterator & last) const
                    {
                        this->actions_->uri_->host_begin = first;
                        this->actions_->uri_->host_end = last;
                    }

                private:
                    actions * actions_;
                } host;

                struct port_action {
                    explicit port_action(actions & actions_):
                        actions_(&actions_)
                    {}

                    template <typename Iterator>
                    void operator()(const Iterator & first,
                                    const Iterator & last) const
                    {
                        this->actions_->uri_->port_begin = first;
                        this->actions_->uri_->port_end = last;
                    }

                private:
                    actions * actions_;
                } port;

                struct path_action {
                    explicit path_action(actions & actions_):
                        actions_(&actions_)
                    {}

                    template <typename Iterator>
                    void operator()(const Iterator & first,
                                    const Iterator & last) const
                    {
                        this->actions_->uri_->path_begin = first;
                        this->actions_->uri_->path_end = last;
                    }

                private:
                    actions * actions_;
                } path;

                struct query_action {
                    explicit query_action(actions & actions_):
                        actions_(&actions_)
                    {}

                    template <typename Iterator>
                    void operator()(const Iterator & first,
                                    const Iterator & last) const
                    {
                        this->actions_->uri_->query_begin = first;
                        this->actions_->uri_->query_end = last;
                    }

                private:
                    actions * actions_;
                } query;

                struct fragment_action {
                    explicit fragment_action(actions & actions_):
                        actions_(&actions_)
                    {}

                    template <typename Iterator>
                    void operator()(const Iterator & first,
                                    const Iterator & last) const
                    {
                        this->actions_->uri_->fragment_begin = first;
                        this->actions_->uri_->fragment_end = last;
                    }

                private:
                    actions * actions_;
                } fragment;
            };

            std::string str_;
            std::string::const_iterator scheme_begin, scheme_end;
            std::string::const_iterator scheme_specific_part_begin,
                scheme_specific_part_end;
            std::string::const_iterator authority_begin, authority_end;
            std::string::const_iterator userinfo_begin, userinfo_end;
            std::string::const_iterator host_begin, host_end;
            std::string::const_iterator port_begin, port_end;
            std::string::const_iterator path_begin, path_end;
            std::string::const_iterator query_begin, query_end;
            std::string::const_iterator fragment_begin, fragment_end;

        public:
            uri() OPENVRML_THROW1(std::bad_alloc);
            explicit uri(const std::string & str)
                OPENVRML_THROW2(openvrml::invalid_url, std::bad_alloc);
            uri(const uri & id) OPENVRML_THROW1(std::bad_alloc);

            uri & operator=(const uri & id) OPENVRML_THROW1(std::bad_alloc);

            operator const std::string() const OPENVRML_THROW1(std::bad_alloc);

            void swap(uri & id) OPENVRML_NOTHROW;

            const std::string scheme() const OPENVRML_THROW1(std::bad_alloc);
            const std::string scheme_specific_part() const
                OPENVRML_THROW1(std::bad_alloc);
            const std::string authority() const OPENVRML_THROW1(std::bad_alloc);
            const std::string userinfo() const OPENVRML_THROW1(std::bad_alloc);
            const std::string host() const OPENVRML_THROW1(std::bad_alloc);
            const std::string port() const OPENVRML_THROW1(std::bad_alloc);
            const std::string path() const OPENVRML_THROW1(std::bad_alloc);
            const std::string query() const OPENVRML_THROW1(std::bad_alloc);
            const std::string fragment() const OPENVRML_THROW1(std::bad_alloc);
        };

        OPENVRML_LOCAL const uri resolve_against(const uri & relative_uri,
                                                 const uri & absolute_uri)
            OPENVRML_THROW1(std::bad_alloc);

        OPENVRML_LOCAL inline bool relative(const uri & id)
        {
            return id.scheme().empty();
        }

        OPENVRML_LOCAL const uri create_file_url(const uri & relative_uri)
            OPENVRML_THROW2(std::runtime_error, std::bad_alloc);

        OPENVRML_LOCAL const std::string getcwd()
            OPENVRML_THROW2(std::runtime_error, std::bad_alloc);
    }
}

#   ifdef BOOST_SPIRIT_DEBUG
namespace std {

    template <typename C, typename E, typename IteratorT>
    std::basic_ostream<C, E> &
    operator<<(std::basic_ostream<C, E> & out, IteratorT)
    {
        return out;
    }
}
#   endif

# endif // ifndef OPENVRML_LOCAL_URI_H
