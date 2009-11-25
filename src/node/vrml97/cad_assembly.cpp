// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML
//
// Copyright 2006, 2007, 2008, 2009  Braden McDaniel
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
# include "cad_assembly.h"
# include "grouping_node_base.h"

namespace {

    class OPENVRML_LOCAL cad_assembly_node :
        public openvrml_node_vrml97::grouping_node_base<cad_assembly_node> {

        friend class openvrml_node_vrml97::cad_assembly_metatype;

        //for CADAssembly node
        exposedfield<openvrml::sfstring> name_;

    public:
        cad_assembly_node(const openvrml::node_type & type,
                   const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~cad_assembly_node() OPENVRML_NOTHROW;
    };

    /**
     * @class cad_assembly_node
     *
     * @brief Represents Group node instances.
     */

    /**
     * @var class cad_assembly_node::cad_assembly_metatype
     *
     * @brief Class object for Group nodes.
     */

    /**
     * @brief Construct.
     */
    cad_assembly_node::
    cad_assembly_node(const openvrml::node_type & type,
                      const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        child_node(type, scope),
        grouping_node(type, scope),
        openvrml_node_vrml97::grouping_node_base<cad_assembly_node>(type,
                                                                    scope),
        name_(*this)
    {}

    /**
     * @brief Destroy.
     */
    cad_assembly_node::~cad_assembly_node() OPENVRML_NOTHROW
    {}
}


/**
 * @brief @c node_metatype identifier.
 */
const char * const openvrml_node_vrml97::cad_assembly_metatype::id =
    "urn:X-openvrml:node:CADAssembly";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this @c node_metatype object.
 */
openvrml_node_vrml97::cad_assembly_metatype::
cad_assembly_metatype(openvrml::browser & browser):
    node_metatype(cad_assembly_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_vrml97::cad_assembly_metatype::~cad_assembly_metatype()
    OPENVRML_NOTHROW
{}

# define CAD_ASSEMBLY_INTERFACE_SEQ                                     \
    ((eventin,      mfnode,   "addChildren",    add_children_listener_)) \
    ((eventin,      mfnode,   "removeChildren", remove_children_listener_)) \
    ((exposedfield, mfnode,   "children",       children_))             \
    ((field,        sfvec3f,  "bboxCenter",     bbox_center_))          \
    ((field,        sfvec3f,  "bboxSize",       bbox_size_))            \
    ((exposedfield, sfnode,   "metadata",       metadata))              \
    ((exposedfield, sfstring, "name",           name_))

OPENVRML_NODE_IMPL_UTIL_DEFINE_DO_CREATE_TYPE(openvrml_node_vrml97,
                                              cad_assembly_metatype,
                                              cad_assembly_node,
                                              CAD_ASSEMBLY_INTERFACE_SEQ)
