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

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

# include <boost/array.hpp>
# include <private.h>
# include "cad_layer.h"
# include "grouping_node_base.h"

namespace {

    class OPENVRML_LOCAL cad_layer_node :
        public openvrml_node_vrml97::grouping_node_base<cad_layer_node> {

        friend class openvrml_node_vrml97::cad_layer_metatype;

        class visible_exposedfield : public exposedfield<openvrml::mfbool> {
        public:
            explicit visible_exposedfield(cad_layer_node & node);
            visible_exposedfield(const visible_exposedfield & obj)
                OPENVRML_NOTHROW;
            virtual ~visible_exposedfield() OPENVRML_NOTHROW;

        private:
            virtual std::auto_ptr<openvrml::field_value> do_clone() const
                OPENVRML_THROW1(std::bad_alloc);
            virtual void event_side_effect(const openvrml::mfbool & visible,
                                           double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };
        friend class visible_exposedfield;

        exposedfield<openvrml::sfstring> name_;
        visible_exposedfield visible_;

        //holds all currently visible nodes
        openvrml::mfnode current_children_;

    public:
        cad_layer_node(const openvrml::node_type & type,
                       const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~cad_layer_node() OPENVRML_NOTHROW;

        virtual bool modified() const;

    private:
        virtual
        void do_children_event_side_effect(const openvrml::mfnode & choice,
                                           double timestamp)
            OPENVRML_THROW1(std::bad_alloc);

        virtual void do_render_child(openvrml::viewer & viewer,
                                     openvrml::rendering_context context);
        virtual const std::vector<boost::intrusive_ptr<openvrml::node> >
            do_children() const OPENVRML_THROW1(std::bad_alloc);
        virtual void recalc_bsphere();
    };


    /**
     * @var cad_layer_node::visible_exposedfield cad_layer_node::visible_
     *
     * @brief visible exposedField.
     */

    /**
     * @class cad_layer_node
     *
     * @brief Represents CADLayer node instances.
     */

    /**
     * @var class cad_layer_node::cad_layer_metatype
     *
     * @brief Class object for CADLayer nodes.
     */

    /**
     * @var cad_layer_node::name_t cad_layer_node::name_
     *
     * @brief name exposedField
     */

    /**
     * @var cad_layer_node::visible_exposedfield cad_layer_node::visible_
     *
     * @brief visible exposedField
     */

    /**
     * @brief Process event.
     *
     * @param choice    choice nodes.
     * @param timestamp the current time.
     *
     * @exception std::bad_alloc    if memory allocation fails.
     */
    void cad_layer_node::
    do_children_event_side_effect(const openvrml::mfnode &, double)
        OPENVRML_THROW1(std::bad_alloc)
    {
        try {
            using openvrml::mfnode;
            const std::vector<bool> & visible = this->visible_.mfbool::value();
            const mfnode::value_type & all_children =
                this->children_.mfnode::value();

            mfnode::value_type children(visible.size());

            //add only the visible children
            for (size_t i = 0;
                 i < visible.size() && i < all_children.size();
                 ++i) {
                if (visible[i]) {
                    children.push_back(all_children[i]);
                }
            }

            this->current_children_.value(children);
        } catch (std::bad_cast & ex) {
            OPENVRML_PRINT_EXCEPTION_(ex);
        }
    }

    /**
     * @internal
     *
     * @class cad_layer_node::visible_exposedfield
     *
     * @brief choice exposedField implementation.
     */

    /**
     * @brief Construct.
     *
     * @param node  cad_layer_node.
     */
    cad_layer_node::visible_exposedfield::
    visible_exposedfield(cad_layer_node & node):
        openvrml::node_event_listener(node),
        openvrml::event_emitter(
            static_cast<const openvrml::field_value &>(*this)),
        mfbool_listener(node),
        exposedfield<openvrml::mfbool>(node)
    {}

    /**
     * @brief Construct a copy.
     *
     * @param obj   instance to copy.
     */
    cad_layer_node::visible_exposedfield::
    visible_exposedfield(const visible_exposedfield & obj) OPENVRML_NOTHROW:
        openvrml::event_listener(),
        openvrml::node_event_listener(
            obj.openvrml::node_event_listener::node()),
        openvrml::event_emitter(
            static_cast<const openvrml::field_value &>(*this)),
        mfbool_listener(obj.openvrml::node_event_listener::node()),
        exposedfield<openvrml::mfbool>(obj)
    {}

    /**
     * @brief Destroy.
     */
    cad_layer_node::visible_exposedfield::
    ~visible_exposedfield() OPENVRML_NOTHROW
    {}

    /**
     * @brief Polymorphically construct a copy.
     *
     * @return a copy of the instance.
     *
     * @exception std::bad_alloc    if memory allocation fails.
     */
    std::auto_ptr<openvrml::field_value>
    cad_layer_node::visible_exposedfield::do_clone() const
        OPENVRML_THROW1(std::bad_alloc)
    {
        return std::auto_ptr<openvrml::field_value>(
            new visible_exposedfield(*this));
    }

    /**
     * @brief Process event.
     *
     * @param visible  choice nodes.
     * @param timestamp     the current time.
     *
     * @exception std::bad_alloc    if memory allocation fails.
     */
    void
    cad_layer_node::visible_exposedfield::
    event_side_effect(const mfbool &, double time)
        OPENVRML_THROW1(std::bad_alloc)
    {
        try {
            cad_layer_node & n =
                dynamic_cast<cad_layer_node &>(
                    this->node_event_listener::node());
            n.do_children_event_side_effect(n.children_, time);
        } catch (std::bad_cast & ex) {
            OPENVRML_PRINT_EXCEPTION_(ex);
        }
    }


    /**
     * @brief Determine whether the node has been modified.
     *
     * @return @c true if the node or one of its children has been modified,
     *      @c false otherwise.
     */
    bool cad_layer_node::modified() const
    {
        if (this->node::modified()) { return true; }

        openvrml::mfnode::value_type::const_iterator iter =
            current_children_.value().begin();
        for (; iter != current_children_.value().end(); ++iter)
        {
            if ((*iter)->modified())
                return true;
        }
        return false;
    }

    /**
     * @brief Render the node.
     *
     * The child corresponding to @a whichChoice is rendered. Nothing is
     * rendered if @a whichChoice is -1.
     *
     * @param viewer    a Viewer.
     * @param context   a rendering context.
     */
    void
    cad_layer_node::
    do_render_child(openvrml::viewer & viewer,
                    const openvrml::rendering_context context)
    {
        openvrml::mfnode::value_type::const_iterator iter =
            current_children_.value().begin();
        for (; iter != current_children_.value().end(); ++iter) {
            using openvrml::node_cast;
            openvrml::child_node * const child =
                node_cast<openvrml::child_node *>(iter->get());
            if (child) { child->render_child(viewer, context); }
        }

        this->node::modified(false);
    }

    /**
     * @brief Get the children in the scene graph.
     *
     * @return the child nodes in the scene graph.
     *
     * @exception std::bad_alloc    if memory allocation fails.
     */
    const std::vector<boost::intrusive_ptr<openvrml::node> >
    cad_layer_node::do_children() const OPENVRML_THROW1(std::bad_alloc)
    {
        return this->current_children_.value();
    }

    /**
     * @brief Recalculate the bounding volume.
     */
    void cad_layer_node::recalc_bsphere()
    {
        this->bsphere = openvrml::bounding_sphere();

        openvrml::mfnode::value_type::const_iterator iter =
            current_children_.value().begin();
        for (; iter != current_children_.value().end(); ++iter) {
            using openvrml::node_cast;
            bounded_volume_node * bounded_volume =
                node_cast<bounded_volume_node *>(iter->get());

            if (bounded_volume) {
                const openvrml::bounding_volume & ci_bv =
                    bounded_volume->bounding_volume();
                this->bsphere.extend(ci_bv);
            }
        }
        this->bounding_volume_dirty(false);
    }

    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope     the scope to which the node belongs.
     */
    cad_layer_node::
    cad_layer_node(const openvrml::node_type & type,
                   const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        child_node(type, scope),
        grouping_node(type, scope),
        openvrml_node_vrml97::grouping_node_base<self_t>(type, scope),
        name_(*this),
        visible_(*this)
    {}

    /**
     * @brief Destroy.
     */
    cad_layer_node::~cad_layer_node() OPENVRML_NOTHROW
    {}
}


/**
 * @brief @c node_metatype identifier.
 */
const char * const openvrml_node_vrml97::cad_layer_metatype::id =
    "urn:X-openvrml:node:CADLayer";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this @c cad_layer_metatype.
 */
openvrml_node_vrml97::cad_layer_metatype::
cad_layer_metatype(openvrml::browser & browser):
    node_metatype(cad_layer_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_vrml97::cad_layer_metatype::~cad_layer_metatype()
    OPENVRML_NOTHROW
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a node_type_ptr to a node_type capable of creating CADLayer nodes.
 *
 * @exception openvrml::unsupported_interface   if @p interfaces includes an
 *                                              interface not supported by
 *                                              @c cad_layer_metatype.
 * @exception std::bad_alloc                    if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml_node_vrml97::cad_layer_metatype::
do_create_type(const std::string & id,
               const openvrml::node_interface_set & interfaces) const
    OPENVRML_THROW2(openvrml::unsupported_interface, std::bad_alloc)
{
    using namespace openvrml;
    using namespace openvrml::node_impl_util;

    typedef boost::array<node_interface, 8> supported_interfaces_t;
    static const supported_interfaces_t supported_interfaces = {
        node_interface(node_interface::eventin_id,
                       field_value::mfnode_id,
                       "addChildren"),
        node_interface(node_interface::eventin_id,
                       field_value::mfnode_id,
                       "removeChildren"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfnode_id,
                       "children"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfstring_id,
                       "name"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfbool_id,
                       "visible"),
        node_interface(node_interface::field_id,
                       field_value::sfvec3f_id,
                       "bboxCenter"),
        node_interface(node_interface::field_id,
                       field_value::sfvec3f_id,
                       "bboxSize")
    };
    typedef node_impl_util::node_type_impl<cad_layer_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & the_node_type = static_cast<node_type_t &>(*type);

    for (node_interface_set::const_iterator interface_(interfaces.begin());
         interface_ != interfaces.end();
         ++interface_) {
        supported_interfaces_t::const_iterator supported_interface =
            supported_interfaces.begin() - 1;
        if (*interface_ == *++supported_interface) {
            the_node_type.add_eventin(
                supported_interface->field_type,
                supported_interface->id,
                &cad_layer_node::add_children_listener_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_eventin(
                supported_interface->field_type,
                supported_interface->id,
                &cad_layer_node::remove_children_listener_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &cad_layer_node::children_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &cad_layer_node::metadata);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &cad_layer_node::name_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &cad_layer_node::visible_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &cad_layer_node::bbox_center_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &cad_layer_node::bbox_size_);
        }
    }
    return type;
}
