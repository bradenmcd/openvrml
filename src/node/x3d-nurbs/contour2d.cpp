// -*- mode: c++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
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

# include "contour2d.h"
# include <openvrml/node_impl_util.h>
# include <boost/array.hpp>

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

using namespace openvrml;
using namespace openvrml::node_impl_util;
using namespace std;

namespace {

    /**
     * @brief Represents Contour2D node instances.
     */
    class OPENVRML_LOCAL contour2d_node :
        public abstract_node<contour2d_node> {

        friend class openvrml_node_x3d_nurbs::contour2d_metatype;

        class add_children_listener : public event_listener_base<self_t>,
                                      public mfnode_listener {
        public:
            explicit add_children_listener(self_t & node);
            virtual ~add_children_listener() OPENVRML_NOTHROW;

        private:
            virtual void do_process_event(const mfnode & fraction,
                                          double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        class remove_children_listener : public event_listener_base<self_t>,
                                         public mfnode_listener {
        public:
            explicit remove_children_listener(self_t & node);
            virtual ~remove_children_listener() OPENVRML_NOTHROW;

        private:
            virtual void do_process_event(const mfnode & fraction,
                                          double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        add_children_listener add_children_listener_;
        remove_children_listener remove_children_listener_;
        exposedfield<mfnode> children_;

    public:
        contour2d_node(const node_type & type,
                       const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~contour2d_node() OPENVRML_NOTHROW;
    };


    /**
     * @var contour2d_node::contour2d_metatype
     *
     * @brief Class object for Contour2D nodes.
     */

    /**
     * @var contour2d_node::add_children_listener contour2d_node::add_children_listener_
     *
     * @brief add_children eventIn listener.
     */

    /**
     * @var contour2d_node::remove_children_listener contour2d_node::remove_children_listener_
     *
     * @brief remove_children eventIn listener.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<contour2d_node>::exposedfield<openvrml::mfnode> contour2d_node::children_
     *
     * @brief children exposedField
     */

    contour2d_node::add_children_listener::
    add_children_listener(self_t & node):
        node_event_listener(node),
        event_listener_base<self_t>(node),
        mfnode_listener(node)
    {}

    contour2d_node::add_children_listener::
    ~add_children_listener() OPENVRML_NOTHROW
    {}

    void contour2d_node::add_children_listener::
    do_process_event(const mfnode & /* fraction */, double /* timestamp */)
        OPENVRML_THROW1(std::bad_alloc)
    {
        //TODO: add logic here
    }

    contour2d_node::remove_children_listener::
    remove_children_listener(self_t & node):
        node_event_listener(node),
        event_listener_base<self_t>(node),
        mfnode_listener(node)
    {}

    contour2d_node::remove_children_listener::
    ~remove_children_listener() OPENVRML_NOTHROW
    {}

    void contour2d_node::remove_children_listener::
    do_process_event(const mfnode & /* fraction */, double /* timestamp */)
        OPENVRML_THROW1(std::bad_alloc)
    {
        //TODO: add logic here
    }


    /**
     * @brief Construct.
     *
     * @param type  the @c node_type associated with this node.
     * @param scope the @c scope to which the node belongs.
     */
    contour2d_node::
    contour2d_node(const node_type & type,
                   const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        abstract_node<self_t>(type, scope),
        add_children_listener_(*this),
        remove_children_listener_(*this),
        children_(*this)
    {}

    /**
     * @brief Destroy.
     */
    contour2d_node::~contour2d_node() OPENVRML_NOTHROW
    {}
}


/**
 * @brief @c node_metatype identifier.
 */
const char * const openvrml_node_x3d_nurbs::contour2d_metatype::id =
    "urn:X-openvrml:node:Contour2D";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this @c contour2d_metatype.
 */
openvrml_node_x3d_nurbs::contour2d_metatype::
contour2d_metatype(openvrml::browser & browser):
    node_metatype(contour2d_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_x3d_nurbs::contour2d_metatype::~contour2d_metatype()
    OPENVRML_NOTHROW
{}

/**
 * @brief Create a @c node_type.
 *
 * @param id            the name for the new @c node_type.
 * @param interfaces    the interfaces for the new @c node_type.
 *
 * @return a @c node_type capable of creating Contour2D nodes.
 *
 * @exception unsupported_interface if @p interfaces includes an interface
 *                                  not supported by @c contour2d_metatype.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml_node_x3d_nurbs::contour2d_metatype::
do_create_type(const std::string & id,
               const node_interface_set & interfaces) const
    OPENVRML_THROW2(unsupported_interface, std::bad_alloc)
{
    typedef boost::array<node_interface, 4> supported_interfaces_t;
    static const supported_interfaces_t supported_interfaces = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata"),
        node_interface(node_interface::eventin_id,
                       field_value::mfnode_id,
                       "addChildren"),
        node_interface(node_interface::eventin_id,
                       field_value::mfnode_id,
                       "removeChildren"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfnode_id,
                       "children")
    };
    typedef node_type_impl<contour2d_node> node_type_t;

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
                &contour2d_node::metadata);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_eventin(
                supported_interface->field_type,
                supported_interface->id,
                &contour2d_node::add_children_listener_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_eventin(
                supported_interface->field_type,
                supported_interface->id,
                &contour2d_node::remove_children_listener_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &contour2d_node::children_);
        } else {
            throw unsupported_interface(*interface_);
        }
    }
    return type;
}
