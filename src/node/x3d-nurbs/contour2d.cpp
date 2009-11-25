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

# define CONTOUR2D_INTERFACE_SEQ                                        \
    ((exposedfield, sfnode,  "metadata",       metadata))               \
    ((eventin,      mfnode,  "addChildren",    add_children_listener_)) \
    ((eventin,      mfnode,  "removeChildren", remove_children_listener_)) \
    ((exposedfield, mfnode,  "children",       children_))

OPENVRML_NODE_IMPL_UTIL_DEFINE_DO_CREATE_TYPE(openvrml_node_x3d_nurbs,
                                              contour2d_metatype,
                                              contour2d_node,
                                              CONTOUR2D_INTERFACE_SEQ)
