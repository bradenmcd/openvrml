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

# include "externproto.h"
# include "field_value_types.h"
# include "uri.h"
# include <openvrml/browser.h>
# include <openvrml/scene.h>
# include <boost/mpl/for_each.hpp>
# include <boost/multi_index/detail/scope_guard.hpp>

using namespace boost::multi_index::detail;  // for scope_guard

struct OPENVRML_LOCAL openvrml::local::externproto_node_metatype::load_proto {
    load_proto(externproto_node_metatype & externproto_class,
               const openvrml::scene & scene,
               const std::vector<std::string> & alt_uris):
        externproto_node_metatype_(&externproto_class),
        scene_(&scene),
        alt_uris_(alt_uris)
    {}

    void operator()() const OPENVRML_NOTHROW
    {
        try {
            using openvrml::unreachable_url;
            try {
                using namespace openvrml;
                using std::auto_ptr;
                using std::ostringstream;
                using std::string;
                using std::vector;
                using boost::dynamic_pointer_cast;
                using boost::shared_ptr;
                using local::uri;

                scope_guard guard =
                    make_obj_guard(
                        *this->externproto_node_metatype_,
                        &externproto_node_metatype::
                            clear_externproto_node_types);
                boost::ignore_unused_variable_warning(guard);

                auto_ptr<resource_istream> in =
                    this->scene_->get_resource(this->alt_uris_);
                if (!(*in)) { throw unreachable_url(); }

                //
                // We don't actually do anything with these; but the parser
                // wants them.
                //
                vector<boost::intrusive_ptr<node> > nodes;
                std::map<string, string> meta;

                parse_vrml(*in, in->url(), in->type(),
                           *this->scene_, nodes, meta);

                shared_ptr<openvrml::local::proto_node_metatype>
                    proto_node_metatype;
                for (vector<string>::const_iterator alt_uri =
                         this->alt_uris_.begin();
                     (alt_uri != this->alt_uris_.end())
                         && !proto_node_metatype;
                     ++alt_uri) {
                    const uri absolute_uri = !relative(uri(*alt_uri))
                        ? uri(*alt_uri)
                        : this->scene_->url().empty()
                            ? create_file_url(uri(*alt_uri))
                            : resolve_against(uri(*alt_uri), 
                                              uri(this->scene_->url()));

                    const shared_ptr<openvrml::node_metatype> node_metatype =
                        this->scene_->browser().node_metatype(
                            node_metatype_id(absolute_uri));

                    proto_node_metatype =
                        dynamic_pointer_cast<
                            openvrml::local::proto_node_metatype>(
                                node_metatype);
                }

                if (!proto_node_metatype) {
                    ostringstream err_msg;
                    err_msg << "no PROTO definition at <" << in->url()
                            << ">";
                    this->scene_->browser().err(err_msg.str());
                    return;
                }

                this->externproto_node_metatype_
                    ->set_proto_node_metatype(proto_node_metatype);

            } catch (std::exception & ex) {
                this->scene_->browser().err(ex.what());
                throw unreachable_url();
            } catch (...) {
                //
                // The implementation of resource_istream is provided by
                // the user; and unfortunately, operations on it could
                // throw anything.
                //
                throw unreachable_url();
            }
        } catch (std::exception & ex) {
            this->scene_->browser().err(ex.what());
        }
    }

private:
    externproto_node_metatype * externproto_node_metatype_;
    const openvrml::scene * scene_;
    std::vector<std::string> alt_uris_;
};

/**
 * @brief Construct.
 *
 * @param[in] id                            the @c node_metatype_id for the
 *                                          @c EXTERNPROTO.
 * @param[in] scene                         the @c scene in which the
 *                                          @c EXTERNPROTO occurs.
 * @param[in] uris                          the list of alternative
 *                                          implementation identifiers.
 * @param[in,out] load_proto_thread_group   the thread group used for loading
 *                                          @c EXTERNPROTO implementations.
 *
 * @exception boost::thread_resource_error  if a new thread of execution
 *                                          cannot be started.
 * @exception std::bad_alloc                if memory allocation fails.
 */
openvrml::local::externproto_node_metatype::
externproto_node_metatype(const openvrml::node_metatype_id & id,
                          const openvrml::scene & scene,
                          const std::vector<std::string> & uris,
                          boost::thread_group & load_proto_thread_group)
    OPENVRML_THROW2(boost::thread_resource_error, std::bad_alloc):
    node_metatype(id, scene.browser()),
                  externproto_node_types_cleared_(false),
                  load_proto_thread_(
                      load_proto_thread_group.create_thread(
                          boost::function0<void>(
                              load_proto(*this, scene, uris))))
{}

/**
 * @brief Destroy.
 */
openvrml::local::externproto_node_metatype::~externproto_node_metatype()
    OPENVRML_NOTHROW
{}

const boost::shared_ptr<openvrml::node_type>
openvrml::local::externproto_node_metatype::
do_create_type(const std::string & id,
               const openvrml::node_interface_set & interfaces) const
    OPENVRML_THROW2(openvrml::unsupported_interface, std::bad_alloc)
{
    boost::mutex::scoped_lock lock(this->mutex_);

    using boost::shared_ptr;

    shared_ptr<openvrml::local::proto_node_metatype> shared;
    if ((shared = this->proto_node_metatype_.lock())) {
        return shared->create_type(id, interfaces);
    }

    const shared_ptr<externproto_node_type> node_type(
        new externproto_node_type(this->shared_from_this(),
                                  id,
                                  interfaces));

    this->externproto_node_types_.push_back(node_type);

    return node_type;
}

void openvrml::local::externproto_node_metatype::do_shutdown(double)
    OPENVRML_NOTHROW
{
    this->load_proto_thread_->join();
}

void
openvrml::local::externproto_node_metatype::
set_proto_node_metatype(const boost::weak_ptr<proto_node_metatype> & metatype)
    OPENVRML_THROW1(std::bad_alloc)
{
    boost::mutex::scoped_lock lock(this->mutex_);

    using boost::shared_ptr;
    using boost::static_pointer_cast;

    this->proto_node_metatype_ = metatype;

    //
    // Now that we have a proto_node_metatype, we need to tell all the
    // externproto_node_types we've created so that they can in turn
    // tell the externproto_nodes they've created.
    //
    const shared_ptr<proto_node_metatype> shared_proto_node_metatype =
        metatype.lock();
    assert(shared_proto_node_metatype);
    for (externproto_node_types::const_iterator node_type =
             this->externproto_node_types_.begin();
         node_type != this->externproto_node_types_.end();
         ++node_type) {
        assert(!node_type->expired());
        node_type->lock()->set_proto_node_type(*shared_proto_node_metatype);
    }
}

void openvrml::local::externproto_node_metatype::clear_externproto_node_types()
    OPENVRML_NOTHROW
{
    boost::mutex::scoped_lock lock(this->mutex_);

    using boost::shared_ptr;

    for (externproto_node_types::const_iterator node_type =
             this->externproto_node_types_.begin();
         node_type != this->externproto_node_types_.end();
         ++node_type) {
        assert(!node_type->expired());
        node_type->lock()->clear_externproto_nodes();
    }

    this->externproto_node_types_.clear();
    this->externproto_node_types_cleared_ = true;
}


openvrml::local::externproto_node_type::externproto_node_type(
    const boost::shared_ptr<const externproto_node_metatype> & c,
    const std::string & id,
    const openvrml::node_interface_set & interfaces)
    OPENVRML_THROW1(std::bad_alloc):
    node_type(*c, id),
    node_metatype_(c),
    interfaces_(interfaces)
{}

openvrml::local::externproto_node_type::~externproto_node_type()
    OPENVRML_NOTHROW
{}

void
openvrml::local::externproto_node_type::
set_proto_node_type(openvrml::local::proto_node_metatype & proto_node_metatype)
    OPENVRML_THROW1(std::bad_alloc)
{
    boost::mutex::scoped_lock lock(this->mutex_);

    assert(!this->proto_node_type_);

    using boost::static_pointer_cast;

    this->proto_node_type_ =
        proto_node_metatype.create_type(this->id(), this->interfaces_);

    for (externproto_nodes::const_iterator node =
             this->externproto_nodes_.begin();
         node != this->externproto_nodes_.end();
         ++node) {
        (*node)->set_proto_node(*this->proto_node_type_);
    }
}

void openvrml::local::externproto_node_type::clear_externproto_nodes()
    OPENVRML_NOTHROW
{
    boost::mutex::scoped_lock lock(this->mutex_);
    this->externproto_nodes_.clear();
}

const openvrml::node_interface_set &
openvrml::local::externproto_node_type::do_interfaces() const throw()
{
    return this->interfaces_;
}

const boost::intrusive_ptr<openvrml::node>
openvrml::local::externproto_node_type::do_create_node(
    const boost::shared_ptr<openvrml::scope> & scope,
    const openvrml::initial_value_map & initial_values) const
    OPENVRML_THROW3(openvrml::unsupported_interface, std::bad_cast,
                    std::bad_alloc)
{
    //
    // externproto_node_metatype::mutex_ must be locked first.
    // externproto_node_metatype::clear_externproto_node_types must not be
    // initiated while we're doing this.
    // externproto_node_metatype::externproto_node_types_cleared_ is checked
    // later in this function and attempting to lock
    // externproto_node_metatype::mutex_ at that point creates a race
    // condition.
    //
    boost::mutex::scoped_lock
        lock_externproto_node_metatype(
            static_cast<const externproto_node_metatype &>(this->metatype())
            .mutex_),
        lock(this->mutex_);

    if (this->proto_node_type_) {
        return this->proto_node_type_->create_node(scope, initial_values);
    }

    const boost::intrusive_ptr<externproto_node> result(
        new externproto_node(*this, scope, initial_values));

    const externproto_node_metatype & node_metatype =
        static_cast<const externproto_node_metatype &>(this->metatype());
    if (!node_metatype.externproto_node_types_cleared_) {
        this->externproto_nodes_.push_back(result);
    } else {
        assert(this->externproto_nodes_.empty());
    }

    return result;
}


template <typename FieldValue>
openvrml::local::externproto_node::externproto_exposedfield<FieldValue>::
externproto_exposedfield(externproto_node & node,
                         const FieldValue & initial_value) OPENVRML_NOTHROW:
    node_event_listener(node),
    openvrml::event_emitter(static_cast<field_value &>(*this)),
    FieldValue(initial_value),
    proto_eventin<FieldValue>(node),
    proto_eventout<FieldValue>(node, initial_value)
{}

template <typename FieldValue>
openvrml::local::externproto_node::externproto_exposedfield<FieldValue>::
~externproto_exposedfield() OPENVRML_NOTHROW
{}

template <typename FieldValue>
std::auto_ptr<openvrml::field_value>
openvrml::local::externproto_node::externproto_exposedfield<FieldValue>::
do_clone() const
    OPENVRML_THROW1(std::bad_alloc)
{
    using boost::polymorphic_downcast;
    return std::auto_ptr<field_value>(
        new externproto_exposedfield<FieldValue>(
            *polymorphic_downcast<externproto_node *>(&this->node()),
            *this));
}

template <typename FieldValue>
void
openvrml::local::externproto_node::externproto_exposedfield<FieldValue>::
do_process_event(const FieldValue & value, const double timestamp)
    OPENVRML_THROW1(std::bad_alloc)
{
    static_cast<externproto_node &>(this->node()).received_event_.insert(this);
    static_cast<FieldValue &>(*this) = value;
    this->proto_eventin<FieldValue>::do_process_event(value, timestamp);
    this->node().modified(true);
    node::emit_event(*this, timestamp);
}

const boost::shared_ptr<openvrml::field_value>
openvrml::local::externproto_node::
create_exposedfield(externproto_node & node, field_value::type_id type)
    OPENVRML_THROW1(std::bad_alloc)
{
    using boost::mpl::for_each;
    using openvrml::local::field_value_types;

    boost::shared_ptr<field_value> result;
    for_each<field_value_types>(externproto_exposedfield_creator(node,
                                                                 type,
                                                                 result));
    assert(result.get());
    return result;
}

openvrml::local::externproto_node::
externproto_node(const externproto_node_type & type,
                 const boost::shared_ptr<openvrml::scope> & scope,
                 const initial_value_map & initial_values)
    OPENVRML_THROW1(std::bad_alloc):
    abstract_proto_node(type, scope),
    initial_values_(initial_values)
{
    for (node_interface_set::const_iterator interface_ =
             type.interfaces().begin();
         interface_ != type.interfaces().end();
         ++interface_)
    {
        using boost::shared_ptr;
        using boost::dynamic_pointer_cast;

        bool succeeded = false;
        std::auto_ptr<openvrml::field_value> field_auto_ptr;
        shared_ptr<openvrml::event_listener> interface_eventin;
        shared_ptr<openvrml::event_emitter> interface_eventout;
        shared_ptr<openvrml::field_value> interface_field;
        switch (interface_->type) {
        case node_interface::eventin_id:
            succeeded = this->eventin_map
                .insert(make_pair(interface_->id,
                                  create_eventin(interface_->field_type,
                                                 *this)))
                .second;
            break;
        case node_interface::eventout_id:
            succeeded = this->eventout_map
                .insert(make_pair(interface_->id,
                                  create_eventout(interface_->field_type,
                                                  *this)))
                .second;
            break;
        case node_interface::exposedfield_id:
            interface_field = create_exposedfield(*this,
                                                  interface_->field_type);
            succeeded = this->field_map_
                .insert(make_pair(interface_->id, interface_field)).second;
            assert(succeeded);

            interface_eventin =
                dynamic_pointer_cast<openvrml::event_listener>(
                    interface_field);
            succeeded = this->eventin_map
                .insert(make_pair("set_" + interface_->id,
                                  interface_eventin))
                .second;
            assert(succeeded);

            interface_eventout =
                dynamic_pointer_cast<openvrml::event_emitter>(
                    interface_eventin);
            succeeded = this->eventout_map
                .insert(make_pair(interface_->id + "_changed",
                                  interface_eventout))
                .second;
            break;
        case node_interface::field_id:
            field_auto_ptr = field_value::create(interface_->field_type);
            succeeded = this->field_map_
                .insert(
                    make_pair(interface_->id,
                              shared_ptr<field_value>(field_auto_ptr)))
                .second;
            break;
        case node_interface::invalid_type_id:
            assert(false
                   && "got node_interface::invalid_type_id for "
                   "interface_->type");
        }
        assert(succeeded);
    }

    //
    // Set the initial values.
    //
    for (initial_value_map::const_iterator map_entry =
             initial_values.begin();
         map_entry != initial_values.end();
         ++map_entry) {
        const field_map::const_iterator pos =
            this->field_map_.find(map_entry->first);
        if (pos == this->field_map_.end()) {
            throw unsupported_interface(this->type(), map_entry->first);
        }
        pos->second->assign(*map_entry->second);
    }
}

openvrml::local::externproto_node::~externproto_node() OPENVRML_NOTHROW
{}

bool openvrml::local::externproto_node::modified() const
{
    return this->proto_node_
        ? this->proto_node_->modified()
        : false;
}

void
openvrml::local::externproto_node::
set_proto_node(node_type & type)
    OPENVRML_THROW1(std::bad_alloc)
{
    using boost::static_pointer_cast;

    //
    // Any exposedFields that received events add to/override what's in the
    // initial_values_ map.
    //
    for (field_map::const_iterator field = this->field_map_.begin();
         field != this->field_map_.end();
         ++field) {
        std::set<openvrml::field_value *>::const_iterator pos =
            this->received_event_.find(field->second.get());
        if (pos != this->received_event_.end()) {
            this->initial_values_[field->first] = field->second;
        }
    }
    this->received_event_.clear();

    this->proto_node_ = type.create_node(this->scope_, this->initial_values_);
    this->initial_values_.clear(); // No longer need these.

    for (eventin_map_t::const_iterator map_entry =
             this->eventin_map.begin();
         map_entry != this->eventin_map.end();
         ++map_entry) {
        using boost::mpl::for_each;
        using openvrml::local::field_value_types;

        openvrml::event_listener & eventin =
            this->proto_node_->event_listener(map_entry->first);
        for_each<field_value_types>(eventin_is(map_entry->second->type(),
                                               eventin,
                                               *map_entry->second));
    }

    for (eventout_map_t::const_iterator map_entry =
             this->eventout_map.begin();
         map_entry != this->eventout_map.end();
         ++map_entry) {
        using boost::mpl::for_each;
        using openvrml::local::field_value_types;

        openvrml::event_emitter & eventout =
            this->proto_node_->event_emitter(map_entry->first);
        for_each<field_value_types>(
            eventout_is(map_entry->second->value().type(),
                        eventout,
                        *map_entry->second));
    }

    if (this->scene()) {
        this->proto_node_->initialize(*this->scene(),
                                      browser::current_time());
    }
}

const std::vector<boost::intrusive_ptr<openvrml::node> > &
openvrml::local::externproto_node::do_impl_nodes() const OPENVRML_NOTHROW
{
    static const std::vector<boost::intrusive_ptr<node> > empty_vec;
    return this->proto_node_
        ? this->proto_node_->impl_nodes()
        : empty_vec;
}

void openvrml::local::externproto_node::do_initialize(const double timestamp)
    OPENVRML_THROW1(std::bad_alloc)
{
    if (this->proto_node_) {
        this->proto_node_->initialize(*this->scene(), timestamp);
    }
}

const openvrml::field_value &
openvrml::local::externproto_node::do_field(const std::string & id) const
    OPENVRML_THROW1(unsupported_interface)
{
    if (this->proto_node_) {
        return this->proto_node_->do_field(id);
    }

    field_map::const_iterator pos = this->field_map_.find(id);
    if (pos == this->field_map_.end()) {
        throw unsupported_interface(this->type(),
                                    node_interface::field_id,
                                    id);
    }
    assert(pos->second);
    return *pos->second;
}

openvrml::event_listener &
openvrml::local::externproto_node::do_event_listener(const std::string & id)
    OPENVRML_THROW1(unsupported_interface)
{
    if (this->proto_node_) {
        return this->proto_node_->do_event_listener(id);
    }

    eventin_map_t::const_iterator pos = this->eventin_map.find(id);
    if (pos == this->eventin_map.end()) {
        throw unsupported_interface(this->type(),
                                    node_interface::eventin_id,
                                    id);
    }
    assert(pos->second);
    return *pos->second;
}

openvrml::event_emitter &
openvrml::local::externproto_node::do_event_emitter(const std::string & id)
    OPENVRML_THROW1(unsupported_interface)
{
    if (this->proto_node_) {
        return this->proto_node_->do_event_emitter(id);
    }

    eventout_map_t::const_iterator pos = this->eventout_map.find(id);
    if (pos == this->eventout_map.end()) {
        throw unsupported_interface(this->type(),
                                    node_interface::eventout_id,
                                    id);
    }
    assert(pos->second);
    return *pos->second;
}

void openvrml::local::externproto_node::do_shutdown(const double timestamp)
    OPENVRML_NOTHROW
{
    if (this->proto_node_) {
        this->proto_node_->shutdown(timestamp);
    }
}

/**
 * @brief Cast to a @c script_node.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a @c script_node, or 0 otherwise.
 */
openvrml::script_node * openvrml::local::externproto_node::to_script()
    OPENVRML_NOTHROW
{
    return node_cast<script_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to an @c appearance_node.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is an @c appearance_node, or 0 otherwise.
 */
openvrml::appearance_node *
openvrml::local::externproto_node::to_appearance() OPENVRML_NOTHROW
{
    return node_cast<appearance_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to a @c background_node.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a @c background_node, or 0 otherwise.
 */
openvrml::background_node *
openvrml::local::externproto_node::to_background() OPENVRML_NOTHROW
{
    return node_cast<background_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to a @c bounded_volume_node.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a @c bounded_volume_node, or 0 otherwise.
 */
openvrml::bounded_volume_node *
openvrml::local::externproto_node::to_bounded_volume()
    OPENVRML_NOTHROW
{
    return node_cast<bounded_volume_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to a @c child_node.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a @c child_node, or 0 otherwise.
 */
openvrml::child_node *
openvrml::local::externproto_node::to_child() OPENVRML_NOTHROW
{
    return node_cast<child_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to a @c color_node.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a @c color_node, or 0 otherwise.
 */
openvrml::color_node *
openvrml::local::externproto_node::to_color() OPENVRML_NOTHROW
{
    return node_cast<color_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to a @c color_rgba_node.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a @c color_rgba_node, or 0 otherwise.
 */
openvrml::color_rgba_node *
openvrml::local::externproto_node::to_color_rgba()
    OPENVRML_NOTHROW
{
    return node_cast<color_rgba_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to a @c coordinate_node.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a @c coordinate_node, or 0 otherwise.
 */
openvrml::coordinate_node *
openvrml::local::externproto_node::to_coordinate() OPENVRML_NOTHROW
{
    return node_cast<coordinate_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to a @c font_style_node.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a @c font_style_node, or 0 otherwise.
 */
openvrml::font_style_node *
openvrml::local::externproto_node::to_font_style() OPENVRML_NOTHROW
{
    return node_cast<font_style_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to a @c geometry_node.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a @c geometry_node, or 0 otherwise.
 */
openvrml::geometry_node * openvrml::local::externproto_node::to_geometry()
    OPENVRML_NOTHROW
{
    return node_cast<geometry_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to a @c grouping_node.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a @c grouping_node, or 0 otherwise.
 */
openvrml::grouping_node * openvrml::local::externproto_node::to_grouping()
    OPENVRML_NOTHROW
{
    return node_cast<grouping_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to a @c light_node.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a @c light_node, or 0 otherwise.
 */
openvrml::light_node * openvrml::local::externproto_node::to_light()
    OPENVRML_NOTHROW
{
    return node_cast<light_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to a @c material_node.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a @c material_node, or 0 otherwise.
 */
openvrml::material_node * openvrml::local::externproto_node::to_material()
    OPENVRML_NOTHROW
{
    return node_cast<material_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to a @c navigation_info_node.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a @c navigation_info_node, or 0 otherwise.
 */
openvrml::navigation_info_node *
openvrml::local::externproto_node::to_navigation_info()
    OPENVRML_NOTHROW
{
    return node_cast<navigation_info_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to a @c normal_node.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a @c normal_node, or 0 otherwise.
 */
openvrml::normal_node * openvrml::local::externproto_node::to_normal()
    OPENVRML_NOTHROW
{
    return node_cast<normal_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to a @c pointing_device_sensor_node.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a @c pointing_device_sensor_node, or 0 otherwise.
 */
openvrml::pointing_device_sensor_node *
openvrml::local::externproto_node::to_pointing_device_sensor()
    OPENVRML_NOTHROW
{
    return node_cast<pointing_device_sensor_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to a @c scoped_light_node.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a @c scoped_light_node, or 0 otherwise.
 */
openvrml::scoped_light_node *
openvrml::local::externproto_node::to_scoped_light() OPENVRML_NOTHROW
{
    return node_cast<scoped_light_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to a @c sound_source_node.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a @c sound_source_node, or 0 otherwise.
 */
openvrml::sound_source_node *
openvrml::local::externproto_node::to_sound_source() OPENVRML_NOTHROW
{
    return node_cast<sound_source_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to a @c texture_node.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a @c texture_node, or 0 otherwise.
 */
openvrml::texture_node * openvrml::local::externproto_node::to_texture()
    OPENVRML_NOTHROW
{
    return node_cast<texture_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to a @c texture_coordinate_node.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a @c texture_coordinate_node, or 0 otherwise.
 */
openvrml::texture_coordinate_node *
openvrml::local::externproto_node::to_texture_coordinate()
    OPENVRML_NOTHROW
{
    return node_cast<texture_coordinate_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to a @c texture_transform_node.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a @c texture_transform_node, or 0 otherwise.
 */
openvrml::texture_transform_node *
openvrml::local::externproto_node::to_texture_transform()
    OPENVRML_NOTHROW
{
    return node_cast<texture_transform_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to a @c time_dependent_node.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a @c time_dependent_node, or 0 otherwise.
 */
openvrml::time_dependent_node *
openvrml::local::externproto_node::to_time_dependent()
    OPENVRML_NOTHROW
{
    return node_cast<time_dependent_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to a @c transform_node.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a @c transform_node, or 0 otherwise.
 */
openvrml::transform_node * openvrml::local::externproto_node::to_transform()
    OPENVRML_NOTHROW
{
    return node_cast<transform_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to a @c viewpoint_node.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a @c viewpoint_node, or 0 otherwise.
 */
openvrml::viewpoint_node * openvrml::local::externproto_node::to_viewpoint()
    OPENVRML_NOTHROW
{
    return node_cast<viewpoint_node *>(this->proto_node_.get());
}
