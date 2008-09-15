// -*- mode: c++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML
//
// Copyright 2006, 2007  Braden McDaniel
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
# include "browser.h"
# include "node_impl_util.h"
# include "x3d_shape.h"

using namespace openvrml;
using namespace openvrml::node_impl_util;
using namespace std;

namespace {

    /**
     * @brief Class object for FillProperties nodes.
     */
    class OPENVRML_LOCAL fill_properties_metatype : public node_metatype {
    public:
        static const char * const id;

        explicit fill_properties_metatype(openvrml::browser & browser);
        virtual ~fill_properties_metatype() OPENVRML_NOTHROW;

    private:
        virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
            OPENVRML_THROW2(unsupported_interface, std::bad_alloc);
    };


    /**
     * @brief Class object for LineProperties nodes.
     */
    class OPENVRML_LOCAL line_properties_metatype : public node_metatype {
    public:
        static const char * const id;

        explicit line_properties_metatype(openvrml::browser & browser);
        virtual ~line_properties_metatype() OPENVRML_NOTHROW;

    private:
        virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
            OPENVRML_THROW2(unsupported_interface, std::bad_alloc);
    };
}

void register_shape_node_metatypes(openvrml::browser & b)
{
    using boost::shared_ptr;
    using openvrml::node_metatype;
    b.add_node_metatype(fill_properties_metatype::id,
                        shared_ptr<node_metatype>(
                            new fill_properties_metatype(b)));
    b.add_node_metatype(line_properties_metatype::id,
                        shared_ptr<node_metatype>(
                            new line_properties_metatype(b)));
}

namespace {

    using namespace openvrml_;

    class OPENVRML_LOCAL fill_properties_node :
        public abstract_node<fill_properties_node> {

        friend class fill_properties_metatype;

        exposedfield<sfbool> filled_;
        exposedfield<sfcolor> hatch_color_;
        exposedfield<sfbool> hatched_;
        exposedfield<sfint32> hatch_style_;

    public:
        fill_properties_node(const node_type & type, 
                             const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~fill_properties_node() OPENVRML_NOTHROW;
    };


    class OPENVRML_LOCAL line_properties_node :
        public abstract_node<line_properties_node> {

        friend class line_properties_metatype;

        exposedfield<sfbool> applied_;
        exposedfield<sfint32> line_type_;
        exposedfield<sffloat> linewidth_scale_factor_;

    public:
        line_properties_node(const node_type & type, 
                             const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~line_properties_node() OPENVRML_NOTHROW;
    };


    /**
     * @brief @c node_metatype identifier.
     */
    const char * const fill_properties_metatype::id =
        "urn:X-openvrml:node:FillProperties";

    /**
     * @brief Construct.
     *
     * @param browser the browser associated with this fill_properties_metatype.
     */
    fill_properties_metatype::fill_properties_metatype(openvrml::browser & browser):
        node_metatype(fill_properties_metatype::id, browser)
    {}

    /**
     * @brief Destroy.
     */
    fill_properties_metatype::~fill_properties_metatype() OPENVRML_NOTHROW
    {}

    /**
     * @brief Create a node_type.
     *
     * @param id            the name for the new node_type.
     * @param interfaces    the interfaces for the new node_type.
     *
     * @return a node_type_ptr to a node_type capable of creating FillProperties nodes.
     *
     * @exception unsupported_interface if @p interfaces includes an interface
     *                                  not supported by fill_properties_metatype.
     * @exception std::bad_alloc        if memory allocation fails.
     */
    const boost::shared_ptr<openvrml::node_type>
    fill_properties_metatype::
    do_create_type(const std::string & id,
                   const node_interface_set & interfaces) const
        OPENVRML_THROW2(unsupported_interface, std::bad_alloc)
    {
        typedef boost::array<node_interface, 5> supported_interfaces_t;
        static const supported_interfaces_t supported_interfaces = {
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "metadata"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfbool_id,
                           "filled"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfcolor_id,
                           "hatchColor"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfbool_id,
                           "hatched"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfint32_id,
                           "hatchStyle")
        };    
        typedef node_type_impl<fill_properties_node> node_type_t;

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
                    &fill_properties_node::metadata);
            } else if (*interface_ == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    &fill_properties_node::filled_);
            } else if (*interface_ == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    &fill_properties_node::hatch_color_);
            } else if (*interface_ == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    &fill_properties_node::hatched_);
            } else if (*interface_ == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    &fill_properties_node::hatch_style_);
            } else {
                throw unsupported_interface(*interface_);
            }
        }        
        return type;
    }


    /**
     * @class fill_properties_node
     *
     * @brief Represents FillProperties node instances.
     */

    /**
     * @var fill_properties_node::fill_properties_metatype
     *
     * @brief Class object for FillProperties nodes.
     */

    /**
     * @var fill_properties_node::filled_
     *
     * @brief filled exposedField
     */

    /**
     * @var fill_properties_node::hatch_color_
     *
     * @brief hatch_color exposedField
     */

    /**
     * @var fill_properties_node::hatched_
     *
     * @brief hatched exposedField
     */

    /**
     * @var fill_properties_node::hatch_style_
     *
     * @brief hatch_style exposedField
     */

    
    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope     the scope to which the node belongs.
     */
    fill_properties_node::
    fill_properties_node(const node_type & type,
                         const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        abstract_node<self_t>(type, scope),
        filled_(*this, true),
        hatch_color_(*this, make_color(1.0, 1.0, 1.0)),
        hatched_(*this, true),
        hatch_style_(*this, 1)
    {}

    /**
     * @brief Destroy.
     */
    fill_properties_node::~fill_properties_node() OPENVRML_NOTHROW
    {}


    /**
     * @brief @c node_metatype identifier.
     */
    const char * const line_properties_metatype::id =
        "urn:X-openvrml:node:LineProperties";

    /**
     * @brief Construct.
     *
     * @param browser the @c browser associated with this
     *                @c line_properties_metatype.
     */
    line_properties_metatype::line_properties_metatype(openvrml::browser & browser):
        node_metatype(line_properties_metatype::id, browser)
    {}

    /**
     * @brief Destroy.
     */
    line_properties_metatype::~line_properties_metatype() OPENVRML_NOTHROW
    {}

    /**
     * @brief Create a node_type.
     *
     * @param id            the name for the new node_type.
     * @param interfaces    the interfaces for the new node_type.
     *
     * @return a node_type_ptr to a node_type capable of creating LineProperties nodes.
     *
     * @exception unsupported_interface if @p interfaces includes an interface
     *                                  not supported by line_properties_metatype.
     * @exception std::bad_alloc        if memory allocation fails.
     */
    const boost::shared_ptr<openvrml::node_type>
    line_properties_metatype::
    do_create_type(const std::string & id,
                   const node_interface_set & interfaces) const
        OPENVRML_THROW2(unsupported_interface, std::bad_alloc)
    {
        typedef boost::array<node_interface, 4> supported_interfaces_t;
        static const supported_interfaces_t supported_interfaces = {
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "metadata"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfbool_id,
                           "applied"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfint32_id,
                           "lineType"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sffloat_id,
                           "linewidthScaleFactor")
        };
        typedef node_type_impl<line_properties_node> node_type_t;

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
                    &line_properties_node::metadata);
            } else if (*interface_ == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    &line_properties_node::applied_);
            } else if (*interface_ == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    &line_properties_node::line_type_);
            } else if (*interface_ == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    &line_properties_node::linewidth_scale_factor_);
            } else {
                throw unsupported_interface(*interface_);
            }
        }
        return type;
    }


    /**
     * @class line_properties_node
     *
     * @brief Represents LineProperties node instances.
     */

    /**
     * @var line_properties_node::line_properties_metatype
     *
     * @brief Class object for LineProperties nodes.
     */

    /**
     * @var line_properties_node::applied_
     *
     * @brief applied exposedField
     */

    /**
     * @var line_properties_node::line_type_
     *
     * @brief line_type exposedField
     */

    /**
     * @var line_properties_node::linewidth_scale_factor_
     *
     * @brief linewidth_scale_factor exposedField
     */


    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope the scope to which the node belongs.
     */
    line_properties_node::
    line_properties_node(const node_type & type,
                         const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        abstract_node<self_t>(type, scope),
        applied_(*this, true),
        line_type_(*this, 1),
        linewidth_scale_factor_(*this)
    {}

    /**
     * @brief Destroy.
     */
    line_properties_node::~line_properties_node() OPENVRML_NOTHROW
    {}
}
