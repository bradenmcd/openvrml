// -*- mode: c++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML
//
// Copyright 2009, 2010  Braden McDaniel
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

# ifndef OPENVRML_LOCAL_PARSE_VRML_H
#   define OPENVRML_LOCAL_PARSE_VRML_H

#   include <openvrml/local/externproto.h>
#   include <openvrml/local/component.h>
#   include <openvrml/local/uri.h>
#   include <openvrml/browser.h>
#   include <openvrml/scene.h>
#   include <openvrml/scope.h>
#   include <stack>

namespace openvrml {

    namespace local {

        const std::string anonymous_stream_id_prefix =
            "urn:X-openvrml:stream:";

        OPENVRML_LOCAL
        bool anonymous_stream_id(const openvrml::local::uri & id);

        struct OPENVRML_LOCAL vrml97_parse_actions {
            vrml97_parse_actions(
                const std::string & uri,
                const openvrml::scene & scene,
                std::vector<boost::intrusive_ptr<openvrml::node> > & nodes):
                on_scene_start(*this),
                on_scene_finish(*this),
                on_externproto(*this),
                on_proto_start(*this),
                on_proto_interface(*this),
                on_proto_default_value_start(*this),
                on_proto_default_value_finish(*this),
                on_proto_body_start(*this),
                on_proto_finish(*this),
                on_node_start(*this),
                on_node_finish(*this),
                on_script_interface_decl(*this),
                on_route(*this),
                on_use(*this),
                on_is_mapping(*this),
                on_field_start(*this),
                on_sfnode(*this),
                on_mfnode(*this),
                on_sfbool(*this),
                on_sfcolor(*this),
                on_mfcolor(*this),
                on_sffloat(*this),
                on_mffloat(*this),
                on_sfimage(*this),
                on_sfint32(*this),
                on_mfint32(*this),
                on_sfrotation(*this),
                on_mfrotation(*this),
                on_sfstring(*this),
                on_mfstring(*this),
                on_sftime(*this),
                on_mftime(*this),
                on_sfvec2f(*this),
                on_mfvec2f(*this),
                on_sfvec3f(*this),
                on_mfvec3f(*this),
                uri_(uri),
                scene_(scene),
                nodes_(nodes)
            {}

            ~vrml97_parse_actions()
            {
                //
                // If the parse scope stack is not empty, we must have hit a
                // parse error.  Since all inner scopes are associated with
                // PROTOs, we'll just drop them.  Back up to the top-level
                // parse scope and use whatever nodes we have there.
                //
                if (!this->ps.empty()) {
                    while (this->ps.size() > 1) { this->ps.pop(); }

                    parse_scope & ps = this->ps.top();

                    //
                    // When we start out, the children stack is one bigger
                    // than the node_data stack.  The node_data stack doesn't
                    // get pushed until we start parsing a node; and it gets
                    // popped when we finish parsing a node.  Once we start
                    // parsing a contained node, children gets pushed again
                    // and it's again one bigger than node_data.
                    //
                    // So, if the node_data and children stacks are the same
                    // size, that means we started parsing a node, but haven't
                    // gotten as far as parsing any contained nodes.  In this
                    // case, just go ahead and call on_node_finish to create
                    // the "current" node and pop the node_data stack.
                    //
                    if (ps.node_data_.size() == ps.children.size()) {
                        this->on_node_finish();
                    }

                    try {
                        while (ps.children.size() > 1) {
                            assert(!ps.node_data_.empty());
                            node_data & nd = ps.node_data_.top();
                            assert(nd.current_field_value);
                            const field_value::type_id current_field_type =
                                nd.current_field_value->second->type();
                            if (current_field_type == field_value::sfnode_id) {
                                nd.current_field_value->second->assign(
                                    sfnode(ps.children.top().front()));
                            } else if (current_field_type
                                       == field_value::mfnode_id) {
                                nd.current_field_value->second->assign(
                                    mfnode(ps.children.top()));
                            } else {
                                assert(false);
                            }
                            ps.children.pop();

                            this->on_node_finish();
                        }

                        this->on_scene_finish();

                    } catch (std::exception & ex) {
                        //
                        // Known possible exceptions here are std::bad_alloc,
                        // std::bad_cast, and openvrml::unsupported_interface.
                        // We're in a destructor here, so we don't want to
                        // throw.
                        //
                        this->scene_.browser().err(
                            "caught exception while constructing scene from "
                            "invalid data: " + std::string(ex.what()));
                    }
                }
            }

            struct on_scene_start_t {
                explicit on_scene_start_t(vrml97_parse_actions & actions):
                    actions_(actions)
                {}

                void operator()() const
                {
                    using std::vector;
                    using boost::intrusive_ptr;

                    this->actions_.ps.push(parse_scope());

                    const profile & p =
                        local::profile_registry_.at(vrml97_profile::id);
                    std::auto_ptr<scope>
                        root_scope(
                            p.create_root_scope(this->actions_.scene_.browser(),
                                                this->actions_.uri_));
                    this->actions_.ps.top().scope = root_scope;
                    this->actions_.ps.top().children.push(
                        parse_scope::children_t());
                }

            private:
                vrml97_parse_actions & actions_;
            } on_scene_start;

            struct on_scene_finish_t {
                explicit on_scene_finish_t(vrml97_parse_actions & actions):
                    actions_(actions)
                {}

                void operator()() const
                {
                    using std::vector;

                    assert(this->actions_.ps.top().children.size() == 1);
                    this->actions_.nodes_ =
                        this->actions_.ps.top().children.top();
                    this->actions_.ps.top().children.pop();

                    assert(this->actions_.ps.size() == 1);

                    parse_scope & ps = this->actions_.ps.top();

                    for (vector<route>::const_iterator r = ps.routes.begin();
                         r != ps.routes.end();
                         ++r) {
                        node * const from = ps.scope->find_node(r->from);
                        assert(from);
                        node * const to = ps.scope->find_node(r->to);
                        assert(to);
                        //
                        // Redundant routing is ignored (4.10.2), so we don't
                        // care if add_route returns false.
                        //
                        add_route(*from, r->eventout, *to, r->eventin);
                    }
                    this->actions_.ps.pop();
                    assert(this->actions_.ps.empty());
                }

            private:
                vrml97_parse_actions & actions_;
            } on_scene_finish;

            static boost::thread_group & get_load_proto_thread_group(browser & b)
            {
                return b.load_proto_thread_group_;
            }

            struct on_externproto_t {
                explicit on_externproto_t(vrml97_parse_actions & actions):
                    actions_(actions)
                {}

                void operator()(const std::string & node_type_id,
                                const node_interface_set & interfaces,
                                const std::vector<std::string> & uri_list) const
                {
                    using std::string;
                    using std::vector;
                    using boost::shared_ptr;
                    using local::uri;

                    //
                    // If we are parsing an "anonymous" stream--that is, one
                    // passed to browser::create_vrml_from_stream--we cannot
                    // use the URI associated with the stream to resolve
                    // relative URIs.  So in this instance, we get the base
                    // URI from browser::world_url.  If browser::world_url is
                    // an empty string, we call create_file_url with an empty
                    // (relative) uri.
                    //
                    const uri base_uri =
                        anonymous_stream_id(uri(this->actions_.uri_))
                        ? this->actions_.scene_.browser().world_url().empty()
                            ? create_file_url(uri())
                            : uri(this->actions_.scene_.browser().world_url())
                        : uri(this->actions_.uri_);

                    shared_ptr<node_type> node_type;
                    for (vector<string>::const_iterator resource_id =
                             uri_list.begin();
                         resource_id != uri_list.end();
                         ++resource_id) try {
                        const uri absolute_uri = relative(uri(*resource_id))
                            ? resolve_against(uri(*resource_id), base_uri)
                            : uri(*resource_id);
                        const shared_ptr<openvrml::node_metatype>
                            node_metatype =
                            this->actions_.scene_.browser().node_metatype(
                                node_metatype_id(absolute_uri));
                        if (node_metatype) {
                            node_type = node_metatype->create_type(node_type_id,
                                                                   interfaces);
                            break;
                        }
                    } catch (invalid_url &) {
                        // If a URI is bogus, ignore it and move on.
                    }

                    if (!node_type) {
                        const std::string metatype_id =
                            path(*this->actions_.ps.top().scope) + '#'
                            + node_type_id;
                        const shared_ptr<openvrml::node_metatype>
                            externproto_class(
                                new externproto_node_metatype(
                                    metatype_id,
                                    this->actions_.scene_,
                                    uri_list,
                                    get_load_proto_thread_group(
                                        this->actions_.scene_.browser())));

                        this->actions_.scene_.browser().add_node_metatype(
                            externproto_class->id(),
                            externproto_class);
                        for (vector<string>::const_iterator resource_id =
                                 uri_list.begin();
                             resource_id != uri_list.end();
                             ++resource_id) try {
                            const uri absolute_uri = relative(uri(*resource_id))
                                ? resolve_against(uri(*resource_id), base_uri)
                                : uri(*resource_id);
                            this->actions_.scene_.browser()
                                .add_node_metatype(
                                    node_metatype_id(absolute_uri),
                                    externproto_class);
                        } catch (invalid_url &) {
                            // We don't want to register node_types under bogus
                            // URIs; ignore the bad ones and move on.
                        }

                        node_type = externproto_class->create_type(node_type_id,
                                                                   interfaces);
                    }

                    assert(node_type);

                    const bool succeeded =
                        this->actions_.ps.top().scope->add_type(node_type)
                        .second;
                    assert(succeeded);
                }

            private:
                vrml97_parse_actions & actions_;
            } on_externproto;

            struct on_proto_start_t {
                explicit on_proto_start_t(vrml97_parse_actions & actions):
                    actions_(actions)
                {}

                void operator()(const std::string & node_type_id) const
                {
                    const boost::shared_ptr<scope> outer_scope =
                        this->actions_.ps.top().scope;
                    assert(outer_scope);

                    this->actions_.ps.push(parse_scope());

                    parse_scope & ps = this->actions_.ps.top();

                    ps.proto_node_type_id = node_type_id;

                    //
                    // We don't want to create the new scope until we start
                    // the PROTO body.  So, for now we set the
                    // parse_scope::scope to the outer scope.
                    //
                    ps.scope = outer_scope;

                    //
                    // We push a node_data onto the stack to hold any field
                    // default values.
                    //
                    ps.node_data_.push(node_data());
                }

            private:
                vrml97_parse_actions & actions_;
            } on_proto_start;

            struct on_proto_interface_t {
                explicit on_proto_interface_t(vrml97_parse_actions & actions):
                    actions_(actions)
                {}

                void operator()(const node_interface & interface_) const
                {
                    parse_scope & ps = this->actions_.ps.top();

                    const bool succeeded =
                        ps.proto_interfaces.insert(interface_).second;
                    assert(succeeded);

                    assert(!ps.node_data_.empty());
                    node_data & nd = ps.node_data_.top();

                    if (interface_.type == node_interface::field_id
                        || interface_.type == node_interface::exposedfield_id) {
                        using std::auto_ptr;
                        using std::pair;
                        auto_ptr<field_value> value =
                            field_value::create(interface_.field_type);
                        pair<initial_value_map::iterator, bool> insert_result =
                            nd.initial_values.insert(
                                make_pair(
                                    interface_.id,
                                    boost::shared_ptr<field_value>(value)));
                        assert(insert_result.second);
                        nd.current_field_value = &(*insert_result.first);
                    }
                }

            private:
                vrml97_parse_actions & actions_;
            } on_proto_interface;

            struct on_proto_default_value_start_t {
                explicit on_proto_default_value_start_t(
                    vrml97_parse_actions & actions):
                    actions_(actions)
                {}

                void operator()() const
                {
                    parse_scope & ps = this->actions_.ps.top();

                    boost::shared_ptr<scope> outer_scope = ps.scope;
                    const std::string & proto_node_type_id =
                        ps.proto_node_type_id;
                    assert(!proto_node_type_id.empty());
                    const std::string & field_id =
                        ps.node_data_.top().current_field_value->first;
                    const field_value::type_id field_type =
                        ps.node_data_.top().current_field_value->second->type();

                    if (field_type == field_value::sfnode_id
                        || field_type == field_value::mfnode_id) {
                        using std::vector;
                        using boost::intrusive_ptr;

                        const node_data old_nd = ps.node_data_.top();

                        const std::string scope_id =
                            proto_node_type_id + '.' + field_id;
                        actions_.ps.push(parse_scope());

                        parse_scope & new_ps = this->actions_.ps.top();

                        new_ps.scope.reset(new scope(scope_id, outer_scope));
                        new_ps.node_data_.push(old_nd);
                        new_ps.children.push(parse_scope::children_t());
                    }
                }

            private:
                vrml97_parse_actions & actions_;
            } on_proto_default_value_start;

            struct on_proto_default_value_finish_t {
                explicit on_proto_default_value_finish_t(
                    vrml97_parse_actions & actions):
                    actions_(actions)
                {}

                void operator()() const
                {
                    assert(!actions_.ps.empty());

                    parse_scope & ps = this->actions_.ps.top();

                    const field_value::type_id field_type =
                        ps.node_data_.top().current_field_value->second->type();

                    if (field_type == field_value::sfnode_id
                        || field_type == field_value::mfnode_id) {
                        using std::vector;

                        assert(actions_.ps.size() > 1);

                        for (vector<route>::const_iterator r =
                                 ps.routes.begin();
                             r != ps.routes.end();
                             ++r) {
                            node * const from = ps.scope->find_node(r->from);
                            assert(from);
                            node * const to = ps.scope->find_node(r->to);
                            assert(to);
                            //
                            // Redundant routing is ignored (4.10.2), so we
                            // don't care if add_route returns false.
                            //
                            add_route(*from, r->eventout, *to, r->eventin);
                        }

                        parse_scope old_ps = ps;
                        this->actions_.ps.pop();
                        parse_scope & new_ps = this->actions_.ps.top();
                        new_ps.node_data_.top().current_field_value->second
                            ->assign(
                                *old_ps.node_data_.top().current_field_value->second);
                    }
                }

            private:
                vrml97_parse_actions & actions_;
            } on_proto_default_value_finish;

            struct on_proto_body_start_t {
                explicit on_proto_body_start_t(vrml97_parse_actions & actions):
                    actions_(actions)
                {}

                void operator()() const
                {
                    assert(!actions_.ps.empty());
                    const boost::shared_ptr<scope> parent =
                        actions_.ps.top().scope;
                    actions_.ps.top().scope.reset(
                        new scope(actions_.ps.top().proto_node_type_id,
                                  parent));
                    actions_.ps.top().proto_default_values =
                        actions_.ps.top().node_data_.top().initial_values;
                    actions_.ps.top().node_data_.pop();
                    assert(actions_.ps.top().children.empty());
                    actions_.ps.top().children.push(
                        parse_scope::children_t());
                }

            private:
                vrml97_parse_actions & actions_;
            } on_proto_body_start;

            struct on_proto_finish_t {
                explicit on_proto_finish_t(vrml97_parse_actions & actions):
                    actions_(actions)
                {}

                void operator()() const
                {
                    using std::vector;
                    using boost::shared_ptr;
                    using boost::dynamic_pointer_cast;
                    using local::proto_node_metatype;
                    parse_scope & ps = this->actions_.ps.top();
                    assert(ps.children.size() == 1);
                    assert(ps.node_data_.empty());
                    proto_node_metatype::routes_t proto_routes;
                    for (vector<route>::const_iterator r = ps.routes.begin();
                         r != ps.routes.end();
                         ++r) {
                        node * const from = ps.scope->find_node(r->from);
                        assert(from);
                        node * const to = ps.scope->find_node(r->to);
                        assert(to);
                        proto_routes.push_back(
                            proto_node_metatype::route(*from, r->eventout,
                                                       *to, r->eventin));
                    }
                    const shared_ptr<openvrml::node_metatype> node_metatype(
                        new proto_node_metatype(
                            path(*ps.scope),
                            this->actions_.scene_.browser(),
                            ps.proto_interfaces,
                            ps.proto_default_values,
                            ps.children.top(),
                            ps.proto_is_map,
                            proto_routes));

                    //
                    // Add the new node_metatype (prototype definition) to the
                    // browser's node_metatype_registry.
                    //
                    this->actions_.scene_.browser()
                        .add_node_metatype(node_metatype->id(), node_metatype);

                    if (!dynamic_pointer_cast<proto_node_metatype>(
                            this->actions_.scene_.browser().node_metatype(
                                node_metatype_id(this->actions_.uri_)))) {
                        this->actions_.scene_.browser()
                            .add_node_metatype(
                                node_metatype_id(this->actions_.uri_),
                                node_metatype);
                    }

                    //
                    // PROTOs implicitly introduce a new node type as well.
                    //
                    const shared_ptr<node_type> node_type =
                        node_metatype->create_type(
                            ps.proto_node_type_id,
                            this->actions_.ps.top().proto_interfaces);
                    assert(node_type);

                    //
                    // We want to add the type to the PROTO's enclosing scope,
                    // so pop off the current scope.
                    //
                    assert(this->actions_.ps.size() > 1);
                    this->actions_.ps.pop();

                    const bool succeeded =
                        this->actions_.ps.top().scope->add_type(node_type)
                        .second;
                    assert(succeeded);
                }

            private:
                vrml97_parse_actions & actions_;
            } on_proto_finish;

            struct on_node_start_t {
                explicit on_node_start_t(vrml97_parse_actions & actions):
                    actions_(actions)
                {}

                void operator()(const std::string & node_name_id,
                                const std::string & node_type_id) const
                {
                    parse_scope & ps = this->actions_.ps.top();

                    ps.node_data_.push(node_data());
                    ps.node_data_.top().node_name_id = node_name_id;

                    assert(ps.scope);
                    if (node_type_id != "Script") {
                        ps.node_data_.top().type =
                            ps.scope->find_type(node_type_id);
                        assert(ps.node_data_.top().type);
                    }
                }

            private:
                vrml97_parse_actions & actions_;
            } on_node_start;

            static script_node_metatype & get_script_node_metatype(browser & b)
            {
                return b.script_node_metatype_;
            }

            struct on_node_finish_t {
                explicit on_node_finish_t(vrml97_parse_actions & actions):
                    actions_(actions)
                {}

                void operator()() const
                {
                    using boost::intrusive_ptr;

                    parse_scope & ps = this->actions_.ps.top();

                    assert(!ps.children.empty());
                    assert(!ps.node_data_.empty());

                    const node_data & nd = ps.node_data_.top();

                    const intrusive_ptr<node> n = nd.type
                        ? nd.type->create_node(ps.scope, nd.initial_values)
                        : intrusive_ptr<node>(
                            new script_node(
                                get_script_node_metatype(
                                    this->actions_.scene_.browser()),
                                ps.scope,
                                nd.script_interfaces,
                                nd.initial_values));

                    if (!nd.node_name_id.empty()) { n->id(nd.node_name_id); }

                    for (node_data::is_map_t::const_iterator entry =
                             nd.is_map.begin();
                         entry != nd.is_map.end();
                         ++entry) {
                        ps.proto_is_map.insert(
                            make_pair(entry->second,
                                      local::proto_node_metatype::is_target(
                                          *n, entry->first)));
                    }

                    ps.children.top().push_back(n);
                    ps.node_data_.pop();
                }

            private:
                vrml97_parse_actions & actions_;
            } on_node_finish;

            struct on_script_interface_decl_t {
                explicit on_script_interface_decl_t(
                    vrml97_parse_actions & actions):
                    actions_(actions)
                {}

                void operator()(const node_interface & interface_) const
                {
                    parse_scope & ps = this->actions_.ps.top();

                    assert(!ps.node_data_.empty());

                    node_data & nd = ps.node_data_.top();

                    const bool succeeded =
                        nd.script_interfaces.insert(interface_).second;
                    assert(succeeded);

                    this->actions_.on_field_start(interface_.id,
                                                  interface_.field_type);
                    if ((interface_.type == node_interface::eventin_id
                         || interface_.type == node_interface::eventout_id)
                        && (interface_.field_type == field_value::sfnode_id
                            || interface_.field_type == field_value::mfnode_id))
                    {
                        ps.children.pop();
                    }
                }

            private:
                vrml97_parse_actions & actions_;
            } on_script_interface_decl;

            struct on_route_t {
                explicit on_route_t(vrml97_parse_actions & actions):
                    actions_(actions)
                {}

                void operator()(const std::string & from_node_name_id,
                                const node_interface & from_node_interface,
                                const std::string & to_node_name_id,
                                const node_interface & to_node_interface) const
                {
                    parse_scope & ps = this->actions_.ps.top();
                    const route r(from_node_name_id, from_node_interface.id,
                                  to_node_name_id, to_node_interface.id);
                    ps.routes.push_back(r);
                }

            private:
                vrml97_parse_actions & actions_;
            } on_route;

            struct on_use_t {
                explicit on_use_t(vrml97_parse_actions & actions):
                    actions_(actions)
                {}

                void operator()(const std::string & node_name_id) const
                {
                    using boost::intrusive_ptr;
                    parse_scope & ps = this->actions_.ps.top();
                    node_data & nd = ps.node_data_.top();

                    //
                    // Check for self-reference.  This is legitimate in Script
                    // nodes.
                    //
                    node * const n = (node_name_id == nd.node_name_id)
                        ? node::self_tag.get()
                        : ps.scope->find_node(node_name_id);
                    assert(n);
                    assert(!ps.children.empty());
                    ps.children.top().push_back(intrusive_ptr<node>(n));
                }

            private:
                vrml97_parse_actions & actions_;
            } on_use;

            struct on_is_mapping_t {
                explicit on_is_mapping_t(vrml97_parse_actions & actions):
                    actions_(actions)
                {}

                void operator()(const std::string & proto_interface_id) const
                {
                    node_data & nd =
                        this->actions_.ps.top().node_data_.top();
                    assert(nd.current_field_value);
                    nd.is_map.insert(make_pair(nd.current_field_value->first,
                                               proto_interface_id));

                    const field_value::type_id field_type =
                        nd.current_field_value->second->type();
                    if (field_type == field_value::sfnode_id
                        || field_type == field_value::mfnode_id) {
                        this->actions_.ps.top().children.pop();
                    }

                    //
                    // We need to leave this around for Script nodes.
                    //
                    if (nd.type) {
                        const initial_value_map::size_type erased =
                            nd.initial_values.erase(
                                nd.current_field_value->first);
                        assert(erased != 0);
                    }
                    nd.current_field_value = 0;
                }

            private:
                vrml97_parse_actions & actions_;
            } on_is_mapping;

            struct on_field_start_t {
                explicit on_field_start_t(vrml97_parse_actions & actions):
                    actions_(actions)
                {}

                void operator()(const std::string & field_name_id,
                                const field_value::type_id field_type) const
                {
                    parse_scope & ps = this->actions_.ps.top();
                    node_data & nd = ps.node_data_.top();

                    using boost::shared_ptr;
                    using std::pair;
                    std::auto_ptr<field_value> value =
                        field_value::create(field_type);
                    pair<initial_value_map::iterator, bool> insert_result =
                        nd.initial_values.insert(
                            make_pair(field_name_id,
                                      shared_ptr<field_value>(value)));
                    assert(insert_result.second);
                    nd.current_field_value = &(*insert_result.first);

                    if (field_type == field_value::sfnode_id
                        || field_type == field_value::mfnode_id) {
                        ps.children.push(parse_scope::children_t());
                    }
                }

            private:
                vrml97_parse_actions & actions_;
            } on_field_start;

            struct on_sfnode_t {
                explicit on_sfnode_t(vrml97_parse_actions & actions):
                    actions_(actions)
                {}

                void operator()(bool null) const
                {
                    assert(!actions_.ps.empty());
                    parse_scope & ps = this->actions_.ps.top();

                    if (!null) {
                        assert(!ps.node_data_.empty());
                        assert(!ps.children.top().empty());
                        ps.node_data_.top()
                            .current_field_value->second->assign(
                                sfnode(ps.children.top().front()));
                    }
                    ps.children.pop();
                }

            private:
                vrml97_parse_actions & actions_;
            } on_sfnode;

            struct on_mfnode_t {
                explicit on_mfnode_t(vrml97_parse_actions & actions):
                    actions_(actions)
                {}

                void operator()() const
                {
                    assert(!this->actions_.ps.empty());
                    parse_scope & ps = this->actions_.ps.top();

                    assert(!ps.node_data_.empty());
                    assert(!ps.children.empty());
                    ps.node_data_.top().current_field_value->second
                        ->assign(mfnode(ps.children.top()));
                    ps.children.pop();
                }

            private:
                vrml97_parse_actions & actions_;
            } on_mfnode;

            struct on_sfbool_t {
                explicit on_sfbool_t(vrml97_parse_actions & actions):
                    actions_(actions)
                {}

                void operator()(const bool val) const
                {
                    assert(!actions_.ps.empty());
                    assert(!actions_.ps.top().node_data_.empty());
                    actions_.ps.top().node_data_.top()
                        .current_field_value->second->assign(sfbool(val));
                }

            private:
                vrml97_parse_actions & actions_;
            } on_sfbool;

            struct on_sfcolor_t {
                explicit on_sfcolor_t(vrml97_parse_actions & actions):
                    actions_(actions)
                {}

                void operator()(const color & val) const
                {
                    assert(!actions_.ps.empty());
                    assert(!actions_.ps.top().node_data_.empty());
                    actions_.ps.top().node_data_.top()
                        .current_field_value->second->assign(sfcolor(val));
                }

            private:
                vrml97_parse_actions & actions_;
            } on_sfcolor;

            struct on_mfcolor_t {
                explicit on_mfcolor_t(vrml97_parse_actions & actions):
                    actions_(actions)
                {}

                void operator()(const std::vector<color> & val) const
                {
                    assert(!actions_.ps.empty());
                    assert(!actions_.ps.top().node_data_.empty());
                    actions_.ps.top().node_data_.top()
                        .current_field_value->second->assign(mfcolor(val));
                }

            private:
                vrml97_parse_actions & actions_;
            } on_mfcolor;

            struct on_sffloat_t {
                explicit on_sffloat_t(vrml97_parse_actions & actions):
                    actions_(actions)
                {}

                void operator()(const float val) const
                {
                    assert(!actions_.ps.empty());
                    assert(!actions_.ps.top().node_data_.empty());
                    actions_.ps.top().node_data_.top()
                        .current_field_value->second->assign(sffloat(val));
                }

            private:
                vrml97_parse_actions & actions_;
            } on_sffloat;

            struct on_mffloat_t {
                explicit on_mffloat_t(vrml97_parse_actions & actions):
                    actions_(actions)
                {}

                void operator()(const std::vector<float> & val) const
                {
                    assert(!actions_.ps.empty());
                    assert(!actions_.ps.top().node_data_.empty());
                    actions_.ps.top().node_data_.top()
                        .current_field_value->second->assign(mffloat(val));
                }

            private:
                vrml97_parse_actions & actions_;
            } on_mffloat;

            struct on_sfimage_t {
                explicit on_sfimage_t(vrml97_parse_actions & actions):
                    actions_(actions)
                {}

                void operator()(const image & val) const
                {
                    assert(!actions_.ps.empty());
                    assert(!actions_.ps.top().node_data_.empty());
                    actions_.ps.top().node_data_.top()
                        .current_field_value->second->assign(sfimage(val));
                }

            private:
                vrml97_parse_actions & actions_;
            } on_sfimage;

            struct on_sfint32_t {
                explicit on_sfint32_t(vrml97_parse_actions & actions):
                    actions_(actions)
                {}

                void operator()(const int32 val) const
                {
                    assert(!actions_.ps.empty());
                    assert(!actions_.ps.top().node_data_.empty());
                    actions_.ps.top().node_data_.top()
                        .current_field_value->second->assign(sfint32(val));
                }

            private:
                vrml97_parse_actions & actions_;
            } on_sfint32;

            struct on_mfint32_t {
                explicit on_mfint32_t(vrml97_parse_actions & actions):
                    actions_(actions)
                {}

                void operator()(const std::vector<int32> & val) const
                {
                    assert(!actions_.ps.empty());
                    assert(!actions_.ps.top().node_data_.empty());
                    actions_.ps.top().node_data_.top()
                        .current_field_value->second->assign(mfint32(val));
                }

            private:
                vrml97_parse_actions & actions_;
            } on_mfint32;

            struct on_sfrotation_t {
                explicit on_sfrotation_t(vrml97_parse_actions & actions):
                    actions_(actions)
                {}

                void operator()(const rotation & val) const
                {
                    assert(!actions_.ps.empty());
                    assert(!actions_.ps.top().node_data_.empty());
                    actions_.ps.top().node_data_.top()
                        .current_field_value->second->assign(sfrotation(val));
                }

            private:
                vrml97_parse_actions & actions_;
            } on_sfrotation;

            struct on_mfrotation_t {
                explicit on_mfrotation_t(vrml97_parse_actions & actions):
                    actions_(actions)
                {}

                void operator()(const std::vector<rotation> & val) const
                {
                    assert(!actions_.ps.empty());
                    assert(!actions_.ps.top().node_data_.empty());
                    actions_.ps.top().node_data_.top()
                        .current_field_value->second->assign(mfrotation(val));
                }

            private:
                vrml97_parse_actions & actions_;
            } on_mfrotation;

            struct on_sfstring_t {
                explicit on_sfstring_t(vrml97_parse_actions & actions):
                    actions_(actions)
                {}

                void operator()(const std::string & val) const
                {
                    assert(!actions_.ps.empty());
                    assert(!actions_.ps.top().node_data_.empty());
                    actions_.ps.top().node_data_.top()
                        .current_field_value->second->assign(sfstring(val));
                }

            private:
                vrml97_parse_actions & actions_;
            } on_sfstring;

            struct on_mfstring_t {
                explicit on_mfstring_t(vrml97_parse_actions & actions):
                    actions_(actions)
                {}

                void operator()(const std::vector<std::string> & val) const
                {
                    assert(!actions_.ps.empty());
                    assert(!actions_.ps.top().node_data_.empty());
                    actions_.ps.top().node_data_.top()
                        .current_field_value->second->assign(mfstring(val));
                }

            private:
                vrml97_parse_actions & actions_;
            } on_mfstring;

            struct on_sftime_t {
                explicit on_sftime_t(vrml97_parse_actions & actions):
                    actions_(actions)
                {}

                void operator()(const double val) const
                {
                    assert(!actions_.ps.empty());
                    assert(!actions_.ps.top().node_data_.empty());
                    actions_.ps.top().node_data_.top()
                        .current_field_value->second->assign(sftime(val));
                }

            private:
                vrml97_parse_actions & actions_;
            } on_sftime;

            struct on_mftime_t {
                explicit on_mftime_t(vrml97_parse_actions & actions):
                    actions_(actions)
                {}

                void operator()(const std::vector<double> & val) const
                {
                    assert(!actions_.ps.empty());
                    assert(!actions_.ps.top().node_data_.empty());
                    actions_.ps.top().node_data_.top()
                        .current_field_value->second->assign(mftime(val));
                }

            private:
                vrml97_parse_actions & actions_;
            } on_mftime;

            struct on_sfvec2f_t {
                explicit on_sfvec2f_t(vrml97_parse_actions & actions):
                    actions_(actions)
                {}

                void operator()(const vec2f & val) const
                {
                    assert(!actions_.ps.empty());
                    assert(!actions_.ps.top().node_data_.empty());
                    actions_.ps.top().node_data_.top()
                        .current_field_value->second->assign(sfvec2f(val));
                }

            private:
                vrml97_parse_actions & actions_;
            } on_sfvec2f;

            struct on_mfvec2f_t {
                explicit on_mfvec2f_t(vrml97_parse_actions & actions):
                    actions_(actions)
                {}

                void operator()(const std::vector<vec2f> & val) const
                {
                    assert(!actions_.ps.empty());
                    assert(!actions_.ps.top().node_data_.empty());
                    actions_.ps.top().node_data_.top()
                        .current_field_value->second->assign(mfvec2f(val));
                }

            private:
                vrml97_parse_actions & actions_;
            } on_mfvec2f;

            struct on_sfvec3f_t {
                explicit on_sfvec3f_t(vrml97_parse_actions & actions):
                    actions_(actions)
                {}

                void operator()(const vec3f & val) const
                {
                    assert(!actions_.ps.empty());
                    assert(!actions_.ps.top().node_data_.empty());
                    actions_.ps.top().node_data_.top()
                        .current_field_value->second->assign(sfvec3f(val));
                }

            private:
                vrml97_parse_actions & actions_;
            } on_sfvec3f;

            struct on_mfvec3f_t {
                explicit on_mfvec3f_t(vrml97_parse_actions & actions):
                    actions_(actions)
                {}

                void operator()(const std::vector<vec3f> & val) const
                {
                    assert(!actions_.ps.empty());
                    assert(!actions_.ps.top().node_data_.empty());
                    actions_.ps.top().node_data_.top()
                        .current_field_value->second->assign(mfvec3f(val));
                }

            private:
                vrml97_parse_actions & actions_;
            } on_mfvec3f;

            struct route {
                std::string from;
                std::string eventout;
                std::string to;
                std::string eventin;

                route(const std::string & from, const std::string & eventout,
                      const std::string & to, const std::string & eventin):
                    from(from),
                    eventout(eventout),
                    to(to),
                    eventin(eventin)
                {}
            };

            struct node_data {
                //
                // This is a multimap because an exposedField might have
                // multiple entries (i.e., for the corresponding eventIn,
                // field, and eventOut).
                //
                typedef std::multimap<std::string, std::string> is_map_t;

                boost::shared_ptr<node_type> type;
                std::string node_name_id;
                initial_value_map initial_values;
                initial_value_map::value_type * current_field_value;
                node_interface_set script_interfaces;
                is_map_t is_map;

                node_data():
                    current_field_value(0)
                {}

                node_data(const node_data & nd):
                    type(nd.type),
                    node_name_id(nd.node_name_id),
                    initial_values(nd.initial_values),
                    current_field_value(0),
                    script_interfaces(nd.script_interfaces),
                    is_map(nd.is_map)
                {
                    if (nd.current_field_value) {
                        initial_value_map::iterator pos =
                            this->initial_values.find(
                                nd.current_field_value->first);
                        assert(pos != this->initial_values.end());
                        this->current_field_value = &(*pos);
                    }
                }

                node_data & operator=(const node_data & nd)
                {
                    node_data temp(nd);
                    this->swap(temp);
                    return *this;
                }

                void swap(node_data & nd) OPENVRML_NOTHROW
                {
                    this->type.swap(nd.type);
                    this->node_name_id.swap(nd.node_name_id);
                    this->initial_values.swap(nd.initial_values);
                    std::swap(this->current_field_value,
                              nd.current_field_value);
                    this->script_interfaces.swap(nd.script_interfaces);
                    this->is_map.swap(nd.is_map);
                }
            };

            struct parse_scope {
                typedef std::vector<boost::intrusive_ptr<node> > children_t;

                std::string proto_node_type_id;
                boost::shared_ptr<openvrml::scope> scope;
                node_interface_set proto_interfaces;
                local::proto_node_metatype::default_value_map_t proto_default_values;
                local::proto_node_metatype::is_map_t proto_is_map;
                std::vector<route> routes;
                std::stack<children_t> children;
                std::stack<node_data> node_data_;
            };

            //
            // We push a parse_scope onto the stack
            // * at the scene root
            // * when starting a PROTO definition
            // * when starting an SFNode or MFNode PROTO default value
            //
            std::stack<parse_scope> ps;

        private:
            const std::string uri_;
            const openvrml::scene & scene_;
            std::vector<boost::intrusive_ptr<openvrml::node> > & nodes_;
        };

        struct OPENVRML_LOCAL x3d_vrml_parse_actions : vrml97_parse_actions {

            x3d_vrml_parse_actions(
                const std::string & uri,
                const openvrml::scene & scene,
                std::vector<boost::intrusive_ptr<openvrml::node> > & nodes,
                std::map<std::string, std::string> & meta):
                local::vrml97_parse_actions(uri, scene, nodes),
                on_profile_statement(*this),
                on_component_statement(*this),
                on_meta_statement(*this),
                on_sfcolorrgba(*this),
                on_sfdouble(*this),
                on_sfvec2d(*this),
                on_sfvec3d(*this),
                on_mfbool(*this),
                on_mfcolorrgba(*this),
                on_mfdouble(*this),
                on_mfimage(*this),
                on_mfvec2d(*this),
                on_mfvec3d(*this),
                meta_(meta)
            {}

            struct on_profile_statement_t {
                explicit on_profile_statement_t(
                    x3d_vrml_parse_actions & actions):
                    actions_(actions)
                {}

                void operator()(const std::string & /* profile_id */) const
                {}

            private:
                x3d_vrml_parse_actions & actions_;
            } on_profile_statement;

            struct on_component_statement_t {
                explicit on_component_statement_t(
                    x3d_vrml_parse_actions & actions):
                    actions_(actions)
                {}

                void operator()(const std::string & /* component_id */,
                                int32 /* level */) const
                {}

            private:
                x3d_vrml_parse_actions & actions_;
            } on_component_statement;

            struct on_meta_statement_t {
                explicit on_meta_statement_t(x3d_vrml_parse_actions & actions):
                    actions_(actions)
                {}

                void operator()(const std::string & name,
                                const std::string & value) const
                {
                    this->actions_.meta_[name] = value;
                }

            private:
                x3d_vrml_parse_actions & actions_;
            } on_meta_statement;

            struct on_import_statement_t {
                void
                operator()(const std::string & /* inline_node_name_id */,
                           const std::string & /* exported_node_name_id */,
                           const std::string & /* imported_node_name_id */) const
                {}
            } on_import_statement;

            struct on_export_statement_t {
                void
                operator()(const std::string & /* node_name_id */,
                           const std::string & /* exported_node_name_id */) const
                {}
            } on_export_statement;

            struct on_sfcolorrgba_t {
                explicit on_sfcolorrgba_t(vrml97_parse_actions & actions):
                    actions_(actions)
                {}

                void operator()(const color_rgba & val) const
                {
                    assert(!actions_.ps.empty());
                    assert(!actions_.ps.top().node_data_.empty());
                    actions_.ps.top().node_data_.top()
                        .current_field_value->second->assign(sfcolorrgba(val));
                }

            private:
                vrml97_parse_actions & actions_;
            } on_sfcolorrgba;

            struct on_sfdouble_t {
                explicit on_sfdouble_t(vrml97_parse_actions & actions):
                    actions_(actions)
                {}

                void operator()(const double val) const
                {
                    assert(!actions_.ps.empty());
                    assert(!actions_.ps.top().node_data_.empty());
                    actions_.ps.top().node_data_.top()
                        .current_field_value->second->assign(sfdouble(val));
                }

            private:
                vrml97_parse_actions & actions_;
            } on_sfdouble;

            struct on_sfvec2d_t {
                explicit on_sfvec2d_t(vrml97_parse_actions & actions):
                    actions_(actions)
                {}

                void operator()(const vec2d & val) const
                {
                    assert(!actions_.ps.empty());
                    assert(!actions_.ps.top().node_data_.empty());
                    actions_.ps.top().node_data_.top()
                        .current_field_value->second->assign(sfvec2d(val));
                }

            private:
                vrml97_parse_actions & actions_;
            } on_sfvec2d;

            struct on_sfvec3d_t {
                explicit on_sfvec3d_t(vrml97_parse_actions & actions):
                    actions_(actions)
                {}

                void operator()(const vec3d & val) const
                {
                    assert(!actions_.ps.empty());
                    assert(!actions_.ps.top().node_data_.empty());
                    actions_.ps.top().node_data_.top()
                        .current_field_value->second->assign(sfvec3d(val));
                }

            private:
                vrml97_parse_actions & actions_;
            } on_sfvec3d;

            struct on_mfbool_t {
                explicit on_mfbool_t(vrml97_parse_actions & actions):
                    actions_(actions)
                {}

                void operator()(const std::vector<bool> & val) const
                {
                    assert(!actions_.ps.empty());
                    assert(!actions_.ps.top().node_data_.empty());
                    actions_.ps.top().node_data_.top()
                        .current_field_value->second->assign(mfbool(val));
                }

            private:
                vrml97_parse_actions & actions_;
            } on_mfbool;

            struct on_mfcolorrgba_t {
                explicit on_mfcolorrgba_t(vrml97_parse_actions & actions):
                    actions_(actions)
                {}

                void operator()(const std::vector<color_rgba> & val) const
                {
                    assert(!actions_.ps.empty());
                    assert(!actions_.ps.top().node_data_.empty());
                    actions_.ps.top().node_data_.top()
                        .current_field_value->second->assign(mfcolorrgba(val));
                }

            private:
                vrml97_parse_actions & actions_;
            } on_mfcolorrgba;

            struct on_mfdouble_t {
                explicit on_mfdouble_t(vrml97_parse_actions & actions):
                    actions_(actions)
                {}

                void operator()(const std::vector<double> & val) const
                {
                    assert(!actions_.ps.empty());
                    assert(!actions_.ps.top().node_data_.empty());
                    actions_.ps.top().node_data_.top()
                        .current_field_value->second->assign(mfdouble(val));
                }

            private:
                vrml97_parse_actions & actions_;
            } on_mfdouble;

            struct on_mfimage_t {
                explicit on_mfimage_t(vrml97_parse_actions & actions):
                    actions_(actions)
                {}

                void operator()(const std::vector<image> & val) const
                {
                    assert(!actions_.ps.empty());
                    assert(!actions_.ps.top().node_data_.empty());
                    actions_.ps.top().node_data_.top()
                        .current_field_value->second->assign(mfimage(val));
                }

            private:
                vrml97_parse_actions & actions_;
            } on_mfimage;

            struct on_mfvec2d_t {
                explicit on_mfvec2d_t(vrml97_parse_actions & actions):
                    actions_(actions)
                {}

                void operator()(const std::vector<vec2d> & val) const
                {
                    assert(!actions_.ps.empty());
                    assert(!actions_.ps.top().node_data_.empty());
                    actions_.ps.top().node_data_.top()
                        .current_field_value->second->assign(mfvec2d(val));
                }

            private:
                vrml97_parse_actions & actions_;
            } on_mfvec2d;

            struct on_mfvec3d_t {
                explicit on_mfvec3d_t(vrml97_parse_actions & actions):
                    actions_(actions)
                {}

                void operator()(const std::vector<vec3d> & val) const
                {
                    assert(!actions_.ps.empty());
                    assert(!actions_.ps.top().node_data_.empty());
                    actions_.ps.top().node_data_.top()
                        .current_field_value->second->assign(mfvec3d(val));
                }

            private:
                vrml97_parse_actions & actions_;
            } on_mfvec3d;

        private:
            std::map<std::string, std::string> & meta_;
        };

        OPENVRML_LOCAL
        void parse_vrml(std::istream & in,
                        const std::string & uri,
                        const std::string & type,
                        const openvrml::scene & scene,
                        std::vector<boost::intrusive_ptr<node> > & nodes,
                        std::map<std::string, std::string> & meta);
    }
}

# endif // ifndef OPENVRML_LOCAL_PARSE_VRML_H
