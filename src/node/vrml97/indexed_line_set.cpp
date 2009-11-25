// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML
//
// Copyright 1998  Chris Morley
// Copyright 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2009  Braden McDaniel
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

# include "indexed_line_set.h"
# include "abstract_indexed_set.h"
# include <private.h>
# include <openvrml/viewer.h>
# include <boost/array.hpp>

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

namespace {

    class OPENVRML_LOCAL indexed_line_set_node :
        public openvrml_node_vrml97::abstract_indexed_set_node<indexed_line_set_node> {

        friend class openvrml_node_vrml97::indexed_line_set_metatype;

        exposedfield<openvrml::mfint32> vertex_count_;

    public:
        indexed_line_set_node(
            const openvrml::node_type & type,
            const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~indexed_line_set_node() OPENVRML_NOTHROW;

    private:
        virtual void do_render_geometry(openvrml::viewer & viewer,
                                        openvrml::rendering_context context);
        virtual bool do_emissive() const OPENVRML_NOTHROW;
    };

    /**
     * @class indexed_line_set_node
     *
     * @brief Represents IndexedLineSet node instances.
     */

    /**
     * @var class indexed_line_set_node::indexed_line_set_metatype
     *
     * @brief Class object for IndexedLineSet nodes.
     */

    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with the node.
     * @param scope the scope to which the node belongs.
     */
    indexed_line_set_node::
    indexed_line_set_node(const openvrml::node_type & type,
                          const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        openvrml_node_vrml97::abstract_indexed_set_node<indexed_line_set_node>(
            type, scope),
        vertex_count_(*this)
    {}

    /**
     * @brief Destroy.
     */
    indexed_line_set_node::~indexed_line_set_node() OPENVRML_NOTHROW
    {}

    /**
     * @brief Insert this geometry into @p viewer's display list.
     *
     * @param viewer    a Viewer.
     * @param context   the rendering context.
     *
     * @todo colors
     */
    void
    indexed_line_set_node::
    do_render_geometry(openvrml::viewer & viewer, openvrml::rendering_context)
    {
        using std::vector;
        using openvrml::node_cast;
        using openvrml::vec3f;

        openvrml::coordinate_node * const coordinateNode =
            node_cast<openvrml::coordinate_node *>(
                this->coord_.sfnode::value().get());
        const vector<vec3f> & coord = coordinateNode
            ? coordinateNode->point()
            : vector<vec3f>();

        openvrml::color_node * const colorNode =
            node_cast<openvrml::color_node *>(
                this->color_.sfnode::value().get());
        const vector<openvrml::color> & color = colorNode
            ? colorNode->color()
            : vector<openvrml::color>();

        viewer.insert_line_set(*this,
                               coord, this->coord_index_.value(),
                               this->color_per_vertex_.value(),
                               color, this->color_index_.value());

        if (colorNode) { colorNode->modified(false); }
        if (coordinateNode) { coordinateNode->modified(false); }
    }

    /**
     * @brief Indicate that line sets should be drawn with the emissive color.
     *
     * @return @c true.
     */
    bool indexed_line_set_node::do_emissive() const OPENVRML_NOTHROW
    {
        return true;
    }
}

 
/**
 * @brief @c node_metatype identifier.
 */
const char * const openvrml_node_vrml97::indexed_line_set_metatype::id =
    "urn:X-openvrml:node:IndexedLineSet";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this @c node_metatype object.
 */
openvrml_node_vrml97::indexed_line_set_metatype::
indexed_line_set_metatype(openvrml::browser & browser):
    node_metatype(indexed_line_set_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_vrml97::indexed_line_set_metatype::~indexed_line_set_metatype()
    OPENVRML_NOTHROW
{}

# define INDEXED_LINE_SET_INTERFACE_SEQ                            \
    ((eventin,      mfint32, "set_colorIndex", set_color_index_))  \
    ((eventin,      mfint32, "set_coordIndex", set_coord_index_))  \
    ((exposedfield, sfnode,  "color",          color_))            \
    ((exposedfield, sfnode,  "coord",          coord_))            \
    ((field,        mfint32, "colorIndex",     color_index_))      \
    ((field,        sfbool,  "colorPerVertex", color_per_vertex_)) \
    ((field,        mfint32, "coordIndex",     coord_index_))      \
    ((exposedfield, sfnode,  "metadata",       metadata))          \
    ((exposedfield, mfint32, "vertexCount",    vertex_count_))

OPENVRML_NODE_IMPL_UTIL_DEFINE_DO_CREATE_TYPE(openvrml_node_vrml97,
                                              indexed_line_set_metatype,
                                              indexed_line_set_node,
                                              INDEXED_LINE_SET_INTERFACE_SEQ)
