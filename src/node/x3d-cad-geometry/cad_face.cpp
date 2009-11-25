// -*- mode: c++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML
//
// Copyright 2006, 2007, 2009  Braden McDaniel
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

# include "cad_face.h"
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
     * @brief Represents CADFace node instances.
     */
    class OPENVRML_LOCAL cad_face_node : public abstract_node<cad_face_node>,
                                         public grouping_node {
        friend class openvrml_node_x3d_cad_geometry::cad_face_metatype;

        exposedfield<sfstring> name_;
        exposedfield<sfnode> shape_;

        //used by do_children to return the shape
        std::vector<boost::intrusive_ptr<node> > children_;

    public:
        cad_face_node(const node_type & type,
                      const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~cad_face_node() OPENVRML_NOTHROW;

    protected:
        virtual bool do_modified() const
            OPENVRML_THROW1(boost::thread_resource_error);

        virtual const openvrml::bounding_volume & do_bounding_volume() const;
        virtual const std::vector<boost::intrusive_ptr<node> >
            do_children() const OPENVRML_THROW1(std::bad_alloc);
    };


    /**
     * @var class cad_face_node::cad_face_metatype
     *
     * @brief Class object for CADFace nodes.
     */

    /**
     * @var abstract_node<self_t>::exposedfield<sfstring> cad_face_node::name_
     *
     * @brief name exposedField
     */

    /**
     * @var abstract_node<self_t>::exposedfield<sfnode> cad_face_node::shape_
     *
     * @brief shape exposedField
     */

    /**
     * @brief Get the children in the scene graph.
     *
     * @return the child nodes in the scene graph.
     *
     * @exception std::bad_alloc    if memory allocation fails.
     */
    const std::vector<boost::intrusive_ptr<node> >
    cad_face_node::do_children() const OPENVRML_THROW1(std::bad_alloc)
    {
        cad_face_node * me = const_cast<cad_face_node *>(this);
        // if there is no child
        if (!shape_.sfnode::value().get()) {
            me->children_.clear();
        } else if (this->children_.empty()
                   || this->children_[0] != shape_.sfnode::value()) {
            // if we have not set the child, or the child has changed.
            me->children_.assign(1, shape_.sfnode::value());
        }
        return children_;
    }

    /**
     * @brief Determine whether the node has been modified.
     *
     * @return @c true if the node or it child has been modified
     *      @c false otherwise.
     */
    bool cad_face_node::do_modified() const
        OPENVRML_THROW1(boost::thread_resource_error)
    {
        return this->shape_.value() && this->shape_.value()->modified();
    }

    /**
     * @brief Get the bounding volume.
     *
     * @return the bounding volume associated with the node.
     */
    const openvrml::bounding_volume &
    cad_face_node::do_bounding_volume() const
    {
        static bounding_sphere empty_volume;

        //get the child as a bounded_volume_node
        bounded_volume_node * node =
            node_cast<bounded_volume_node *>(shape_.sfnode::value().get());

        //return empty if there is not child
        if (node == NULL)
            return empty_volume;

        //return the child's bounded volume
        return node->bounding_volume();
    }

    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope the scope to which the node belongs.
     */
    cad_face_node::
    cad_face_node(const node_type & type,
                  const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        child_node(type, scope),
        abstract_node<self_t>(type, scope),
        grouping_node(type, scope),
        name_(*this),
        shape_(*this)
    {}

    /**
     * @brief Destroy.
     */
    cad_face_node::~cad_face_node() OPENVRML_NOTHROW
    {}
}


/**
 * @brief @c node_metatype identifier.
 */
const char * const
openvrml_node_x3d_cad_geometry::cad_face_metatype::id = "urn:X-openvrml:node:CADFace";

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this cad_face_metatype.
 */
openvrml_node_x3d_cad_geometry::cad_face_metatype::
cad_face_metatype(openvrml::browser & browser):
    node_metatype(cad_face_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_x3d_cad_geometry::cad_face_metatype::~cad_face_metatype()
    OPENVRML_NOTHROW
{}

# define CAD_FACE_INTERFACE_SEQ \
    ((exposedfield, sfnode,   "metadata", metadata)) \
    ((exposedfield, sfstring, "name",     name_)) \
    ((exposedfield, sfnode,   "shape",    shape_))

OPENVRML_NODE_IMPL_UTIL_DEFINE_DO_CREATE_TYPE(openvrml_node_x3d_cad_geometry,
                                              cad_face_metatype,
                                              cad_face_node,
                                              CAD_FACE_INTERFACE_SEQ)
