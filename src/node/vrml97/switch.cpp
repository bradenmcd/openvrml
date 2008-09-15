// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML
//
// Copyright 1998  Chris Morley
// Copyright 2001, 2002, 2003, 2004, 2005, 2006, 2007  Braden McDaniel
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
# include "switch.h"
# include "grouping_node_base.h"

namespace {

    class OPENVRML_LOCAL switch_node :
        public openvrml_node_vrml97::grouping_node_base<switch_node> {

        friend class openvrml_node_vrml97::switch_metatype;

        class which_choice_exposedfield : public exposedfield<openvrml::sfint32> {
        public:
            explicit which_choice_exposedfield(switch_node & node);
            which_choice_exposedfield(const which_choice_exposedfield & obj)
                OPENVRML_NOTHROW;
            virtual ~which_choice_exposedfield() OPENVRML_NOTHROW;

        private:
            virtual std::auto_ptr<openvrml::field_value> do_clone() const
                OPENVRML_THROW1(std::bad_alloc);
            virtual void event_side_effect(const openvrml::sfint32 & which_choice,
                                           double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        which_choice_exposedfield which_choice_;
        openvrml::mfnode current_children_;

    public:
        switch_node(const openvrml::node_type & type,
                    const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~switch_node() OPENVRML_NOTHROW;

        virtual bool modified() const;

    private:
        virtual void do_children_event_side_effect(const openvrml::mfnode & choice,
                                                   double timestamp)
            OPENVRML_THROW1(std::bad_alloc);

        virtual void do_render_child(openvrml::viewer & viewer,
                                     openvrml::rendering_context context);
        virtual const std::vector<boost::intrusive_ptr<openvrml::node> >
            do_children() const OPENVRML_THROW1(std::bad_alloc);
        virtual void recalc_bsphere();
    };

    /**
     * @class switch_node
     *
     * @brief Switch node instances.
     */

    /**
     * @var class switch_node::switch_metatype
     *
     * @brief Class object for Switch nodes.
     */

    /**
     * @brief Process event.
     *
     * @param choice    choice nodes.
     * @param timestamp the current time.
     *
     * @exception std::bad_alloc    if memory allocation fails.
     */
    void switch_node::do_children_event_side_effect(const openvrml::mfnode &, double)
        OPENVRML_THROW1(std::bad_alloc)
    {
        try {
            using openvrml::int32;
            const int32 which_choice = this->which_choice_.sfint32::value();
            assert(!this->children_.mfnode::value().empty());
            typedef std::vector<boost::intrusive_ptr<openvrml::node> >
                children_t;
            children_t children = this->current_children_.value();
            children[0] =
                (which_choice >= 0
                 && which_choice < int32(this->children_.mfnode::value().size()))
                ? this->children_.mfnode::value()[which_choice]
                : children_t::value_type(0);
            this->current_children_.value(children);
        } catch (std::bad_cast & ex) {
            OPENVRML_PRINT_EXCEPTION_(ex);
        }
    }

    /**
     * @internal
     *
     * @class switch_node::which_choice_exposedfield
     *
     * @brief choice exposedField implementation.
     */

    /**
     * @brief Construct.
     *
     * @param node  switch_node.
     */
    switch_node::which_choice_exposedfield::
    which_choice_exposedfield(switch_node & node):
        openvrml::node_event_listener(node),
        openvrml::event_emitter(static_cast<const openvrml::field_value &>(*this)),
        sfint32_listener(node),
        exposedfield<openvrml::sfint32>(node, -1)
    {}

    /**
     * @brief Construct a copy.
     *
     * @param obj   instance to copy.
     */
    switch_node::which_choice_exposedfield::
    which_choice_exposedfield(const which_choice_exposedfield & obj) OPENVRML_NOTHROW:
        openvrml::event_listener(),
        openvrml::node_event_listener(obj.openvrml::node_event_listener::node()),
        openvrml::event_emitter(static_cast<const openvrml::field_value &>(*this)),
        sfint32_listener(obj.openvrml::node_event_listener::node()),
        exposedfield<openvrml::sfint32>(obj)
    {}

    /**
     * @brief Destroy.
     */
    switch_node::which_choice_exposedfield::
    ~which_choice_exposedfield() OPENVRML_NOTHROW
    {}

    /**
     * @brief Polymorphically construct a copy.
     *
     * @return a copy of the instance.
     *
     * @exception std::bad_alloc    if memory allocation fails.
     */
    std::auto_ptr<openvrml::field_value>
    switch_node::which_choice_exposedfield::do_clone() const
        OPENVRML_THROW1(std::bad_alloc)
    {
        return std::auto_ptr<openvrml::field_value>(
            new which_choice_exposedfield(*this));
    }

    /**
     * @brief Process event.
     *
     * @param which_choice  choice nodes.
     * @param timestamp     the current time.
     *
     * @exception std::bad_alloc    if memory allocation fails.
     */
    void
    switch_node::which_choice_exposedfield::
    event_side_effect(const openvrml::sfint32 & which_choice, double)
        OPENVRML_THROW1(std::bad_alloc)
    {
        try {
            switch_node & n =
                dynamic_cast<switch_node &>(this->node_event_listener::node());

            assert(!n.current_children_.mfnode::value().empty());
            typedef std::vector<boost::intrusive_ptr<openvrml::node> >
                children_t;
            children_t children = n.current_children_.value();
            children[0] =
                ((which_choice.value() >= 0)
                 && (which_choice.value()
                     < openvrml::int32(n.children_.mfnode::value().size())))
                ? n.children_.mfnode::value()[which_choice.value()]
                : children_t::value_type(0);
            n.current_children_.mfnode::value(children);
        } catch (std::bad_cast & ex) {
            OPENVRML_PRINT_EXCEPTION_(ex);
        }
    }

    /**
     * @var switch_node::which_choice_exposedfield switch_node::which_choice_
     *
     * @brief whichChoice exposedField.
     */

    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with the node instance.
     * @param scope     the scope to which the node belongs.
     */
    switch_node::
    switch_node(const openvrml::node_type & type,
                const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        child_node(type, scope),
        grouping_node(type, scope),
        openvrml_node_vrml97::grouping_node_base<switch_node>(type, scope),
        which_choice_(*this),
        current_children_(1)
    {
        this->bounding_volume_dirty(true);
    }

    /**
     * @brief Destroy.
     */
    switch_node::~switch_node() OPENVRML_NOTHROW
    {}

    /**
     * @brief Determine whether the node has been modified.
     *
     * @return @c true if the node or one of its children has been modified,
     *      @c false otherwise.
     */
    bool switch_node::modified() const
    {
        if (this->node::modified()) { return true; }

        long w = this->which_choice_.sfint32::value();

        return w >= 0 && size_t(w) < this->children_.mfnode::value().size()
            && this->children_.mfnode::value()[w]->modified();
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
    switch_node::
    do_render_child(openvrml::viewer & viewer,
                    const openvrml::rendering_context context)
    {
        assert(!this->current_children_.mfnode::value().empty());
        child_node * const child =
            openvrml::node_cast<child_node *>(
                this->current_children_.value()[0].get());
        if (child) { child->render_child(viewer, context); }
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
    switch_node::do_children() const OPENVRML_THROW1(std::bad_alloc)
    {
        return this->current_children_.value();
    }

    /**
     * @brief Recalculate the bounding volume.
     */
    void switch_node::recalc_bsphere()
    {
        this->bsphere = openvrml::bounding_sphere();
        openvrml::int32 w = this->which_choice_.sfint32::value();
        if (w >= 0 && size_t(w) < this->children_.mfnode::value().size()) {
            using openvrml::node_cast;
            const boost::intrusive_ptr<node> & node =
                this->children_.mfnode::value()[w];
            bounded_volume_node * bounded_volume =
                node_cast<bounded_volume_node *>(node.get());
            if (bounded_volume) {
                const openvrml::bounding_volume & ci_bv =
                    bounded_volume->bounding_volume();
                this->bsphere.extend(ci_bv);
            }
        }
        this->bounding_volume_dirty(false);
    }
}


/**
 * @brief @c node_metatype identifier.
 */
const char * const openvrml_node_vrml97::switch_metatype::id =
    "urn:X-openvrml:node:Switch";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this @c node_metatype.
 */
openvrml_node_vrml97::switch_metatype::
switch_metatype(openvrml::browser & browser):
    node_metatype(switch_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_vrml97::switch_metatype::~switch_metatype()
    OPENVRML_NOTHROW
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a boost::shared_ptr<node_type> to a node_type capable of
 *         creating Switch nodes.
 *
 * @exception openvrml::unsupported_interface if @p interfaces includes an interface
 *                                  not supported by switch_metatype.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml_node_vrml97::switch_metatype::
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
                       field_value::mfnode_id,
                       "choice"),
        node_interface(node_interface::field_id,
                       field_value::sfvec3f_id,
                       "bboxCenter"),
        node_interface(node_interface::field_id,
                       field_value::sfvec3f_id,
                       "bboxSize"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfint32_id,
                       "whichChoice"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata")
    };

    typedef node_impl_util::node_type_impl<switch_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & switchNodeType = static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface_(interfaces.begin());
         interface_ != interfaces.end();
         ++interface_) {
        supported_interfaces_t::const_iterator supported_interface =
            supported_interfaces.begin() - 1;
        if (*interface_ == *++supported_interface) {
            switchNodeType.add_eventin(
                supported_interface->field_type,
                supported_interface->id,
                &switch_node::add_children_listener_);
        } else if (*interface_ == *++supported_interface) {
            switchNodeType.add_eventin(
                supported_interface->field_type,
                supported_interface->id,
                &switch_node::remove_children_listener_);
        } else if (*interface_ == *++supported_interface) {
            switchNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &switch_node::children_);
        } else if (*interface_ == *++supported_interface) {
            switchNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &switch_node::children_);
        } else if (*interface_ == *++supported_interface) {
            switchNodeType.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &switch_node::bbox_center_);
        } else if (*interface_ == *++supported_interface) {
            switchNodeType.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &switch_node::bbox_size_);
        } else if (*interface_ == *++supported_interface) {
            switchNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &switch_node::which_choice_);
        } else if (*interface_ == *++supported_interface) {
            switchNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &switch_node::metadata);
        } else {
            throw unsupported_interface(*interface_);
        }
    }
    return type;
}
