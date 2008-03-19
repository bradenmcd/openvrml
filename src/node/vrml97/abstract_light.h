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

# ifndef OPENVRML_NODE_VRML97_ABSTRACT_LIGHT_H
#   define OPENVRML_NODE_VRML97_ABSTRACT_LIGHT_H

#   include <openvrml/node_impl_util.h>

namespace openvrml_node_vrml97 {

    /**
     * @brief Base class for all light nodes.
     */
    template <typename Derived>
    class OPENVRML_LOCAL abstract_light_node :
        public openvrml::node_impl_util::abstract_node<Derived>,
        public virtual openvrml::light_node {
    protected:
        typename openvrml::node_impl_util::abstract_node<Derived>::template exposedfield<openvrml::sffloat>
        ambient_intensity_;
        typename openvrml::node_impl_util::abstract_node<Derived>::template exposedfield<openvrml::sfcolor>
        color_;
        typename openvrml::node_impl_util::abstract_node<Derived>::template exposedfield<openvrml::sffloat>
        intensity_;
        typename openvrml::node_impl_util::abstract_node<Derived>::template exposedfield<openvrml::sfbool> on_;

    public:
        virtual ~abstract_light_node() OPENVRML_NOTHROW = 0;

    protected:
        abstract_light_node(
            const openvrml::node_type & type,
            const boost::shared_ptr<openvrml::scope> & scope);

    private:
        virtual float do_ambient_intensity() const OPENVRML_NOTHROW;
        virtual float do_intensity() const OPENVRML_NOTHROW;
        virtual bool do_on() const OPENVRML_NOTHROW;
        virtual const openvrml::color & do_color() const OPENVRML_NOTHROW;
    };

    /**
     * @var openvrml::node_impl_util::abstract_node<Derived>::exposedfield<openvrml::sffloat> abstract_light_node::ambient_intensity_
     *
     * @brief ambientIntensity exposedField.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<Derived>::exposedfield<openvrml::sfcolor> abstract_light_node::color_
     *
     * @brief color exposedField.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<Derived>::exposedfield<openvrml::sffloat> abstract_light_node::intensity_
     *
     * @brief intensity exposedField.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<Derived>::exposedfield<openvrml::sfbool> abstract_light_node::on_
     *
     * @brief on exposedField.
     */

    /**
     * @brief Construct.
     *
     * @param type  the node_type for the node.
     * @param scope     the scope to which the node belongs.
     */
    template <typename Derived>
    abstract_light_node<Derived>::
    abstract_light_node(const openvrml::node_type & type,
                        const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        child_node(type, scope),
        light_node(type, scope),
        openvrml::node_impl_util::abstract_node<Derived>(type, scope),
        ambient_intensity_(*this, 0.0),
        color_(*this, openvrml::make_color(1.0, 1.0, 1.0)),
        intensity_(*this, 1.0),
        on_(*this, true)
    {}

    /**
     * @brief Destroy.
     */
    template <typename Derived>
    abstract_light_node<Derived>::~abstract_light_node() OPENVRML_NOTHROW
    {}

    /**
     * @brief Ambient intensity.
     *
     * @return the ambient intensity.
     */
    template <typename Derived>
    float abstract_light_node<Derived>::do_ambient_intensity() const
        OPENVRML_NOTHROW
    {
        using openvrml::sffloat;
        return this->ambient_intensity_.sffloat::value();
    }

    /**
     * @brief Intensity.
     *
     * @return the intensity.
     */
    template <typename Derived>
    float abstract_light_node<Derived>::do_intensity() const OPENVRML_NOTHROW
    {
        using openvrml::sffloat;
        return this->intensity_.sffloat::value();
    }

    /**
     * @brief Whether the light is on.
     *
     * @return @c true if the light is on; @c false otherwise.
     */
    template <typename Derived>
    bool abstract_light_node<Derived>::do_on() const OPENVRML_NOTHROW
    {
        using openvrml::sfbool;
        return this->on_.sfbool::value();
    }

    /**
     * @brief Light color.
     *
     * @return the light color.
     */
    template <typename Derived>
    const openvrml::color &
    abstract_light_node<Derived>::do_color() const OPENVRML_NOTHROW
    {
        using openvrml::sfcolor;
        return this->color_.sfcolor::value();
    }
}

# endif // ifndef OPENVRML_NODE_VRML97_ABSTRACT_LIGHT_H
