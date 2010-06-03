// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML
//
// Copyright 2006, 2007, 2008  Braden McDaniel
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

# include "texture_background.h"
# include <openvrml/node_impl_util.h>
# include <boost/array.hpp>

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

using namespace openvrml;
using namespace openvrml::node_impl_util;
using namespace std;

namespace {

    class OPENVRML_LOCAL texture_background_node :
        public abstract_node<texture_background_node>,
        public background_node {

        friend
        class openvrml_node_x3d_environmental_effects::texture_background_metatype;

        class set_bind_listener : public event_listener_base<self_t>,
                                  public sfbool_listener {
        public:
            explicit set_bind_listener(self_t & node);
            virtual ~set_bind_listener() OPENVRML_NOTHROW;

        private:
            virtual void do_process_event(const sfbool & fraction,
                                          double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        set_bind_listener set_bind_listener_;
        exposedfield<mffloat> ground_angle_;
        exposedfield<mfcolor> ground_color_;
        exposedfield<sfnode> back_texture_;
        exposedfield<sfnode> bottom_texture_;
        exposedfield<sfnode> front_texture_;
        exposedfield<sfnode> left_texture_;
        exposedfield<sfnode> right_texture_;
        exposedfield<sfnode> top_texture_;
        exposedfield<mffloat> sky_angle_;
        exposedfield<mfcolor> sky_color_;
        exposedfield<mffloat> transparency_;
        sftime bind_time_;
        sftime_emitter bind_time_emitter_;
        sfbool is_bound_;
        sfbool_emitter is_bound_emitter_;

    public:
        texture_background_node(
            const node_type & type,
            const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~texture_background_node() OPENVRML_NOTHROW;

    private:
        virtual bool do_modified() const
            OPENVRML_THROW1(boost::thread_resource_error);

        virtual const std::vector<float> & do_ground_angle() const
            OPENVRML_NOTHROW;
        virtual const std::vector<openvrml::color> & do_ground_color() const
            OPENVRML_NOTHROW;
        virtual const std::vector<float> & do_sky_angle() const
            OPENVRML_NOTHROW;
        virtual const std::vector<openvrml::color> & do_sky_color() const
            OPENVRML_NOTHROW;
        virtual openvrml::texture_node * do_front() const OPENVRML_NOTHROW;
        virtual openvrml::texture_node * do_back() const OPENVRML_NOTHROW;
        virtual openvrml::texture_node * do_left() const OPENVRML_NOTHROW;
        virtual openvrml::texture_node * do_right() const OPENVRML_NOTHROW;
        virtual openvrml::texture_node * do_top() const OPENVRML_NOTHROW;
        virtual openvrml::texture_node * do_bottom() const OPENVRML_NOTHROW;
    };


    /**
     * @class texture_background_node
     *
     * @brief Represents TextureBackground node instances.
     */

    /**
     * @var texture_background_node::texture_background_metatype
     *
     * @brief Class object for TextureBackground nodes.
     */

    /**
     * @var texture_background_node::set_bind_listener texture_background_node::set_bind_listener_
     *
     * @brief set_bind eventIn
     */

    /**
     * @var exposedfield<mffloat> texture_background_node::ground_angle_
     *
     * @brief ground_angle exposedField
     */

    /**
     * @var exposedfield<mfcolor> texture_background_node::ground_color_
     *
     * @brief ground_color exposedField
     */

    /**
     * @var exposedfield<sfnode> texture_background_node::back_texture_
     *
     * @brief back_texture exposedField
     */

    /**
     * @var exposedfield<sfnode> texture_background_node::bottom_texture_
     *
     * @brief bottom_texture exposedField
     */

    /**
     * @var exposedfield<sfnode> texture_background_node::front_texture_
     *
     * @brief front_texture exposedField
     */

    /**
     * @var exposedfield<sfnode> texture_background_node::left_texture_
     *
     * @brief left_texture exposedField
     */

    /**
     * @var exposedfield<sfnode> texture_background_node::right_texture_
     *
     * @brief right_texture exposedField
     */

    /**
     * @var exposedfield<sfnode> texture_background_node::top_texture_
     *
     * @brief top_texture exposedField
     */

    /**
     * @var exposedfield<mffloat> texture_background_node::sky_angle_
     *
     * @brief sky_angle exposedField
     */

    /**
     * @var exposedfield<mfcolor> texture_background_node::sky_color_
     *
     * @brief sky_color exposedField
     */

    /**
     * @var exposedfield<mffloat> texture_background_node::transparency_
     *
     * @brief transparency exposedField
     */

    /**
     * @var sftime texture_background_node::bind_time_
     *
     * @brief bind_time eventOut.
     */

    /**
     * @var sftime_emitter texture_background_node::bind_time_emitter_
     *
     * @brief bind_time eventOut emitter.
     */

    /**
     * @var sfbool texture_background_node::is_bound_
     *
     * @brief is_bound eventOut.
     */

    /**
     * @var sfbool_emitter texture_background_node::is_bound_emitter_
     *
     * @brief is_bound eventOut emitter.
     */

    texture_background_node::set_bind_listener::
    set_bind_listener(self_t & node):
        node_event_listener(node),
        event_listener_base<self_t>(node),
        sfbool_listener(node)
    {}

    texture_background_node::set_bind_listener::
    ~set_bind_listener() OPENVRML_NOTHROW
    {}

    void texture_background_node::set_bind_listener::
    do_process_event(const sfbool & /* fraction */, double /* timestamp */)
        OPENVRML_THROW1(std::bad_alloc)
    {
        //TODO: add logic here
    }


    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope     the scope to which the node belongs.
     */
    texture_background_node::
    texture_background_node(const node_type & type,
                            const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        child_node(type, scope),
        abstract_node<self_t>(type, scope),
        background_node(type, scope),
        set_bind_listener_(*this),
        ground_angle_(*this),
        ground_color_(*this),
        back_texture_(*this),
        bottom_texture_(*this),
        front_texture_(*this),
        left_texture_(*this),
        right_texture_(*this),
        top_texture_(*this),
        sky_angle_(*this),
        sky_color_(*this),
        transparency_(*this),
        bind_time_emitter_(*this, this->bind_time_),
        is_bound_emitter_(*this, this->is_bound_)
    {}

    /**
     * @brief Destroy.
     */
    texture_background_node::~texture_background_node() OPENVRML_NOTHROW
    {}

    bool texture_background_node::do_modified() const
        OPENVRML_THROW1(boost::thread_resource_error)
    {
        return (this->front_texture_.value()
                && this->front_texture_.value()->modified())
            || (this->back_texture_.value()
                && this->back_texture_.value()->modified())
            || (this->left_texture_.value()
                && this->left_texture_.value()->modified())
            || (this->right_texture_.value()
                && this->right_texture_.value()->modified())
            || (this->top_texture_.value()
                && this->top_texture_.value()->modified())
            || (this->bottom_texture_.value()
                && this->bottom_texture_.value()->modified());
    }

    /**
     * @brief Ground angles.
     *
     * @return a vector of angles corresponding to ground colors.
     */
    const std::vector<float> &
    texture_background_node::do_ground_angle() const OPENVRML_NOTHROW
    {
        return this->ground_angle_.value();
    }

    /**
     * @brief Ground colors.
     *
     * @return a vector of ground colors.
     */
    const std::vector<openvrml::color> &
    texture_background_node::do_ground_color() const OPENVRML_NOTHROW
    {
        return this->ground_color_.value();
    }

    /**
     * @brief Sky angles.
     *
     * @return a vector of angles corresponding to sky colors.
     */
    const std::vector<float> &
    texture_background_node::do_sky_angle() const OPENVRML_NOTHROW
    {
        return this->sky_angle_.value();
    }

    /**
     * @brief Sky colors.
     *
     * @return a vector of sky colors.
     */
    const std::vector<openvrml::color> &
    texture_background_node::do_sky_color() const OPENVRML_NOTHROW
    {
        return this->sky_color_.value();
    }

    /**
     * @brief Front texture.
     *
     * @return the front texture.
     */
    openvrml::texture_node * texture_background_node::do_front() const
        OPENVRML_NOTHROW
    {
        return dynamic_cast<texture_node *>(this->front_texture_.value().get());
    }

    /**
     * @brief Back texture.
     *
     * @return the back texture.
     */
    openvrml::texture_node * texture_background_node::do_back() const
        OPENVRML_NOTHROW
    {
        return dynamic_cast<texture_node *>(this->back_texture_.value().get());
    }

    /**
     * @brief Left texture.
     *
     * @return the left texture.
     */
    openvrml::texture_node * texture_background_node::do_left() const
        OPENVRML_NOTHROW
    {
        return dynamic_cast<texture_node *>(this->left_texture_.value().get());
    }

    /**
     * @brief Right texture.
     *
     * @return the right texture.
     */
    openvrml::texture_node * texture_background_node::do_right() const
        OPENVRML_NOTHROW
    {
        return dynamic_cast<texture_node *>(this->right_texture_.value().get());
    }

    /**
     * @brief Top texture.
     *
     * @return the top texture.
     */
    openvrml::texture_node * texture_background_node::do_top() const
        OPENVRML_NOTHROW
    {
        return dynamic_cast<texture_node *>(this->top_texture_.value().get());
    }

    /**
     * @brief Bottom texture.
     *
     * @return the bottom texture.
     */
    openvrml::texture_node *
    texture_background_node::do_bottom() const OPENVRML_NOTHROW
    {
        return dynamic_cast<texture_node *>(this->bottom_texture_.value().get());
    }
}


/**
 * @brief @c node_metatype identifier.
 */
const char * const
openvrml_node_x3d_environmental_effects::texture_background_metatype::id =
    "urn:X-openvrml:node:TextureBackground";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this
 *                    @c texture_background_metatype.
 */
openvrml_node_x3d_environmental_effects::texture_background_metatype::
texture_background_metatype(openvrml::browser & browser):
    node_metatype(texture_background_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_x3d_environmental_effects::texture_background_metatype::
~texture_background_metatype()
    OPENVRML_NOTHROW
{}

/**
 * @brief Create a @c node_type.
 *
 * @param id            the name for the new @c node_type.
 * @param interfaces    the interfaces for the new @c node_type.
 *
 * @return a @c node_type capable of creating TextureBackground nodes.
 *
 * @exception unsupported_interface if @p interfaces includes an interface
 *                                  not supported by
 *                                  @c texture_background_metatype.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml_node_x3d_environmental_effects::texture_background_metatype::
do_create_type(const std::string & id,
               const node_interface_set & interfaces) const
    OPENVRML_THROW2(unsupported_interface, std::bad_alloc)
{
    typedef boost::array<node_interface, 15> supported_interfaces_t;
    static const supported_interfaces_t supported_interfaces = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata"),
        node_interface(node_interface::eventin_id,
                       field_value::sfbool_id,
                       "set_bind"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mffloat_id,
                       "groundAngle"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfcolor_id,
                       "groundColor"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "backTexture"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "bottomTexture"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "frontTexture"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "leftTexture"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "rightTexture"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "topTexture"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mffloat_id,
                       "skyAngle"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfcolor_id,
                       "skyColor"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mffloat_id,
                       "transparency"),
        node_interface(node_interface::eventout_id,
                       field_value::sftime_id,
                       "bindTime"),
        node_interface(node_interface::eventout_id,
                       field_value::sfbool_id,
                       "isBound")
    };
    typedef node_type_impl<texture_background_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & the_node_type = static_cast<node_type_t &>(*type);

    for (node_interface_set::const_iterator interface_(interfaces.begin());
         interface_ != interfaces.end();
         ++interface_) {
        supported_interfaces_t::const_iterator supported_interface =
            supported_interfaces.begin() - 1;
        if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &texture_background_node::metadata);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_eventin(
                supported_interface->field_type,
                supported_interface->id,
                &texture_background_node::set_bind_listener_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &texture_background_node::ground_angle_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &texture_background_node::ground_color_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &texture_background_node::back_texture_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &texture_background_node::bottom_texture_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &texture_background_node::front_texture_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &texture_background_node::left_texture_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &texture_background_node::right_texture_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &texture_background_node::top_texture_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &texture_background_node::sky_angle_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &texture_background_node::sky_color_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &texture_background_node::transparency_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_eventout(
                supported_interface->field_type,
                supported_interface->id,
                &texture_background_node::bind_time_emitter_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_eventout(
                supported_interface->field_type,
                supported_interface->id,
                &texture_background_node::is_bound_emitter_);
        } else {
            throw unsupported_interface(*interface_);
        }
    }
    return type;
}
