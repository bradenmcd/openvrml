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

# ifndef OPENVRML_NODE_VRML97_ABSTRACT_TEXTURE_H
#   define OPENVRML_NODE_VRML97_ABSTRACT_TEXTURE_H

#   include <openvrml/node_impl_util.h>

namespace openvrml_node_vrml97 {

    /**
     * @brief Abstract base class for VRML97 texture nodes.
     */
    template <typename Derived>
    class abstract_texture_node :
        public openvrml::node_impl_util::abstract_node<Derived>,
        public openvrml::texture_node {
    protected:
        openvrml::sfbool repeat_s_;
        openvrml::sfbool repeat_t_;

    public:
        virtual ~abstract_texture_node() OPENVRML_NOTHROW = 0;

    protected:
        abstract_texture_node(
            const openvrml::node_type & type,
            const boost::shared_ptr<openvrml::scope> & scope);

    private:
        //
        // texture_node implementation.
        //
        virtual bool do_repeat_s() const OPENVRML_NOTHROW;
        virtual bool do_repeat_t() const OPENVRML_NOTHROW;
    };

    /**
     * @var openvrml::sfbool abstract_texture_node::repeat_s_
     *
     * @brief repeatS field.
     */

    /**
     * @var openvrml::sfbool abstract_texture_node::repeat_t_
     *
     * @brief repeatT field.
     */

    /**
     * @brief Construct.
     *
     * @param type  the node_type for the node instance.
     * @param scope the scope to which the node belongs.
     */
    template <typename Derived>
    abstract_texture_node<Derived>::
    abstract_texture_node(
        const openvrml::node_type & type,
        const boost::shared_ptr<openvrml::scope> & scope):
        openvrml::node(type, scope),
        openvrml::node_impl_util::abstract_node<Derived>(type, scope),
        openvrml::texture_node(type, scope),
        repeat_s_(true),
        repeat_t_(true)
    {}

    /**
     * @brief Destroy.
     */
    template <typename Derived>
    abstract_texture_node<Derived>::~abstract_texture_node() OPENVRML_NOTHROW
    {}

    /**
     * @brief Get the repeatS flag.
     *
     * @return @c true if the texture should repeat in the <var>S</var>
     *         direction, @c false otherwise.
     */
    template <typename Derived>
    bool abstract_texture_node<Derived>::do_repeat_s() const OPENVRML_NOTHROW
    {
        return this->repeat_s_.value();
    }

    /**
     * @brief Get the repeatT flag.
     *
     * @return @c true if the texture should repeat in the <var>T</var>
     *         direction, @c false otherwise.
     */
    template <typename Derived>
    bool abstract_texture_node<Derived>::do_repeat_t() const OPENVRML_NOTHROW
    {
        return this->repeat_t_.value();
    }
}

# endif // ifndef OPENVRML_NODE_VRML97_ABSTRACT_TEXTURE_H
