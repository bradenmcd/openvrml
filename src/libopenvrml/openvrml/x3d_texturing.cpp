// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; -*-
//
// OpenVRML
//
// Copyright 2006  Braden McDaniel
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

# include <boost/array.hpp>
# include <private.h>
# include "browser.h"
# include "node_impl_util.h"
# include "x3d_texturing.h"

using namespace openvrml;
using namespace openvrml::node_impl_util;
using namespace std;

namespace {

    /**
     * @brief Class object for MultiTexture nodes.
     */
    class OPENVRML_LOCAL multi_texture_metatype : public node_metatype {
    public:
        static const char * const id;

        explicit multi_texture_metatype(openvrml::browser & browser);
        virtual ~multi_texture_metatype() throw ();

    private:
        virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
            throw (unsupported_interface, std::bad_alloc);
    };


    /**
     * @brief Class object for MultiTextureCoordinate nodes.
     */
    class OPENVRML_LOCAL multi_texture_coordinate_metatype : public node_metatype {
    public:
        static const char * const id;

        explicit multi_texture_coordinate_metatype(openvrml::browser & browser);
        virtual ~multi_texture_coordinate_metatype() throw ();

    private:
        virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
            throw (unsupported_interface, std::bad_alloc);
    };


    /**
     * @brief Class object for MultiTextureTransform nodes.
     */
    class OPENVRML_LOCAL multi_texture_transform_metatype : public node_metatype {
    public:
        static const char * const id;

        explicit multi_texture_transform_metatype(openvrml::browser & browser);
        virtual ~multi_texture_transform_metatype() throw ();

    private:
        virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
            throw (unsupported_interface, std::bad_alloc);
    };


    /**
     * @brief Class object for TextureCoordinateGenerator nodes.
     */
    class OPENVRML_LOCAL texture_coordinate_generator_metatype :
        public node_metatype {
    public:
        static const char * const id;

        explicit texture_coordinate_generator_metatype(
            openvrml::browser & browser);
        virtual ~texture_coordinate_generator_metatype() throw ();

    private:
        virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
            throw (unsupported_interface, std::bad_alloc);
    };
}

void register_texturing_node_metatypes(openvrml::browser & b)
{
    using boost::shared_ptr;
    using openvrml::node_metatype;
    b.add_node_metatype(multi_texture_metatype::id,
                        shared_ptr<node_metatype>(new multi_texture_metatype(b)));
    b.add_node_metatype(
        multi_texture_coordinate_metatype::id,
        shared_ptr<node_metatype>(new multi_texture_coordinate_metatype(b)));
    b.add_node_metatype(
        multi_texture_transform_metatype::id,
        shared_ptr<node_metatype>(new multi_texture_transform_metatype(b)));
    b.add_node_metatype(
        texture_coordinate_generator_metatype::id,
        shared_ptr<node_metatype>(new texture_coordinate_generator_metatype(b)));
}

namespace {

    using namespace openvrml_;

    class OPENVRML_LOCAL multi_texture_node :
        public abstract_node<multi_texture_node> {

        friend class multi_texture_metatype;

        exposedfield<sffloat> alpha_;
        exposedfield<sfcolor> color_;
        exposedfield<mfstring> function_;
        exposedfield<mfstring> mode_;
        exposedfield<mfstring> source_;
        exposedfield<mfnode> texture_;

    public:
        multi_texture_node(const node_type & type,
                           const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~multi_texture_node() throw ();
    };


    class OPENVRML_LOCAL multi_texture_coordinate_node :
        public abstract_node<multi_texture_coordinate_node> {

        friend class multi_texture_coordinate_metatype;

        exposedfield<mfnode> tex_coord_;

    public:
        multi_texture_coordinate_node(const node_type & type,
                                      const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~multi_texture_coordinate_node() throw ();
    };


    class OPENVRML_LOCAL multi_texture_transform_node :
        public abstract_node<multi_texture_transform_node> {
        friend class multi_texture_transform_metatype;

        exposedfield<mfnode> texture_transform_;

    public:
        multi_texture_transform_node(const node_type & type,
                                     const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~multi_texture_transform_node() throw ();
    };


    class OPENVRML_LOCAL texture_coordinate_generator_node :
        public abstract_node<texture_coordinate_generator_node> {
        friend class texture_coordinate_generator_metatype;

        exposedfield<sfstring> mode_;
        exposedfield<mffloat> parameter_;

    public:
        texture_coordinate_generator_node(const node_type & type,
                                          const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~texture_coordinate_generator_node() throw ();
    };


    /**
     * @brief @c node_metatype identifier.
     */
    const char * const multi_texture_metatype::id =
        "urn:X-openvrml:node:MultiTexture";

    /**
     * @brief Construct.
     *
     * @param browser the @c browser associated with this
     *                @c multi_texture_metatype.
     */
    multi_texture_metatype::multi_texture_metatype(openvrml::browser & browser):
        node_metatype(multi_texture_metatype::id, browser)
    {}

    /**
     * @brief Destroy.
     */
    multi_texture_metatype::~multi_texture_metatype() throw ()
    {}

    /**
     * @brief Create a node_type.
     *
     * @param id            the name for the new node_type.
     * @param interfaces    the interfaces for the new node_type.
     *
     * @return a @c node_type capable of creating MultiTexture nodes.
     *
     * @exception unsupported_interface if @p interfaces includes an interface
     *                                  not supported by multi_texture_metatype.
     * @exception std::bad_alloc        if memory allocation fails.
     */
    const boost::shared_ptr<openvrml::node_type>
    multi_texture_metatype::
    do_create_type(const std::string & id,
                   const node_interface_set & interfaces) const
        throw (unsupported_interface, std::bad_alloc)
    {
        typedef boost::array<node_interface, 7> supported_interfaces_t;
        static const supported_interfaces_t supported_interfaces = {
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "metadata"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sffloat_id,
                           "alpha"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfcolor_id,
                           "color"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mfstring_id,
                           "function"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mfstring_id,
                           "mode"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mfstring_id,
                           "source"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mfnode_id,
                           "texture")
        };
        typedef node_type_impl<multi_texture_node> node_type_t;

        const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
        node_type_t & the_node_type = static_cast<node_type_t &>(*type);

        for (node_interface_set::const_iterator interface(interfaces.begin());
             interface != interfaces.end();
             ++interface) {
            supported_interfaces_t::const_iterator supported_interface =
                supported_interfaces.begin() - 1;
            if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<multi_texture_node>::exposedfield<sfnode> >(
                            &multi_texture_node::metadata)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<multi_texture_node>::exposedfield<sfnode> >(
                            &multi_texture_node::metadata)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<multi_texture_node>::exposedfield<sfnode> >(
                            &multi_texture_node::metadata)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<multi_texture_node>::exposedfield<sffloat> >(
                            &multi_texture_node::alpha_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<multi_texture_node>::exposedfield<sffloat> >(
                            &multi_texture_node::alpha_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<multi_texture_node>::exposedfield<sffloat> >(
                            &multi_texture_node::alpha_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<multi_texture_node>::exposedfield<sfcolor> >(
                            &multi_texture_node::color_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<multi_texture_node>::exposedfield<sfcolor> >(
                            &multi_texture_node::color_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<multi_texture_node>::exposedfield<sfcolor> >(
                            &multi_texture_node::color_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<multi_texture_node>::exposedfield<mfstring> >(
                            &multi_texture_node::function_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<multi_texture_node>::exposedfield<mfstring> >(
                            &multi_texture_node::function_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<multi_texture_node>::exposedfield<mfstring> >(
                            &multi_texture_node::function_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<multi_texture_node>::exposedfield<mfstring> >(
                            &multi_texture_node::mode_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<multi_texture_node>::exposedfield<mfstring> >(
                            &multi_texture_node::mode_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<multi_texture_node>::exposedfield<mfstring> >(
                            &multi_texture_node::mode_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<multi_texture_node>::exposedfield<mfstring> >(
                            &multi_texture_node::source_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<multi_texture_node>::exposedfield<mfstring> >(
                            &multi_texture_node::source_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<multi_texture_node>::exposedfield<mfstring> >(
                            &multi_texture_node::source_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<multi_texture_node>::exposedfield<mfnode> >(
                            &multi_texture_node::texture_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<multi_texture_node>::exposedfield<mfnode> >(
                            &multi_texture_node::texture_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<multi_texture_node>::exposedfield<mfnode> >(
                            &multi_texture_node::texture_)));
            } else {
                throw unsupported_interface(*interface);
            }
        }
        return type;
    }


    /**
     * @brief @c node_metatype identifier.
     */
    const char * const multi_texture_coordinate_metatype::id =
        "urn:X-openvrml:node:MultiTextureCoordinate";

    /**
     * @brief Construct.
     *
     * @param browser the @c browser associated with this
     *                @c multi_texture_coordinate_metatype.
     */
    multi_texture_coordinate_metatype::
    multi_texture_coordinate_metatype(openvrml::browser & browser):
        node_metatype(multi_texture_coordinate_metatype::id, browser)
    {}

    /**
     * @brief Destroy.
     */
    multi_texture_coordinate_metatype::~multi_texture_coordinate_metatype() throw ()
    {}

    /**
     * @brief Create a node_type.
     *
     * @param id            the name for the new node_type.
     * @param interfaces    the interfaces for the new node_type.
     *
     * @return a @c node_type capable of creating MultiTextureCoordinate nodes.
     *
     * @exception unsupported_interface if @p interfaces includes an interface
     *                                  not supported by multi_texture_coordinate_metatype.
     * @exception std::bad_alloc        if memory allocation fails.
     */
    const boost::shared_ptr<openvrml::node_type>
    multi_texture_coordinate_metatype::
    do_create_type(const std::string & id,
                   const node_interface_set & interfaces) const
        throw (unsupported_interface, std::bad_alloc)
    {
        typedef boost::array<node_interface, 2> supported_interfaces_t;
        static const supported_interfaces_t supported_interfaces = {
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "metadata"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mfnode_id,
                           "texCoord")
        };
        typedef node_type_impl<multi_texture_coordinate_node> node_type_t;

        const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
        node_type_t & the_node_type = static_cast<node_type_t &>(*type);

        for (node_interface_set::const_iterator interface(interfaces.begin());
             interface != interfaces.end();
             ++interface) {
            supported_interfaces_t::const_iterator supported_interface =
                supported_interfaces.begin() - 1;
            if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<multi_texture_coordinate_node>::exposedfield<sfnode> >(
                            &multi_texture_coordinate_node::metadata)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<multi_texture_coordinate_node>::exposedfield<sfnode> >(
                            &multi_texture_coordinate_node::metadata)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<multi_texture_coordinate_node>::exposedfield<sfnode> >(
                            &multi_texture_coordinate_node::metadata)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<multi_texture_coordinate_node>::exposedfield<mfnode> >(
                            &multi_texture_coordinate_node::tex_coord_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<multi_texture_coordinate_node>::exposedfield<mfnode> >(
                            &multi_texture_coordinate_node::tex_coord_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<multi_texture_coordinate_node>::exposedfield<mfnode> >(
                            &multi_texture_coordinate_node::tex_coord_)));
            } else {
                throw unsupported_interface(*interface);
            }
        }
        return type;
    }


    /**
     * @brief @c node_metatype identifier.
     */
    const char * const multi_texture_transform_metatype::id =
        "urn:X-openvrml:node:MultiTextureTransform";

    /**
     * @brief Construct.
     *
     * @param browser the @c browser associated with this
     *                @c multi_texture_transform_metatype.
     */
    multi_texture_transform_metatype::
    multi_texture_transform_metatype(openvrml::browser & browser):
        node_metatype(multi_texture_transform_metatype::id, browser)
    {}

    /**
     * @brief Destroy.
     */
    multi_texture_transform_metatype::~multi_texture_transform_metatype() throw ()
    {}

    /**
     * @brief Create a node_type.
     *
     * @param id            the name for the new node_type.
     * @param interfaces    the interfaces for the new node_type.
     *
     * @return a @c node_type capable of creating MultiTextureTransform nodes.
     *
     * @exception unsupported_interface if @p interfaces includes an interface
     *                                  not supported by multi_texture_transform_metatype.
     * @exception std::bad_alloc        if memory allocation fails.
     */
    const boost::shared_ptr<openvrml::node_type>
    multi_texture_transform_metatype::
    do_create_type(const std::string & id,
                   const node_interface_set & interfaces) const
        throw (unsupported_interface, std::bad_alloc)
    {
        typedef boost::array<node_interface, 2> supported_interfaces_t;
        static const supported_interfaces_t supported_interfaces = {
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "metadata"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mfnode_id,
                           "textureTransform")
        };
        typedef node_type_impl<multi_texture_transform_node> node_type_t;

        const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
        node_type_t & the_node_type = static_cast<node_type_t &>(*type);

        for (node_interface_set::const_iterator interface(interfaces.begin());
             interface != interfaces.end();
             ++interface) {
            supported_interfaces_t::const_iterator supported_interface =
                supported_interfaces.begin() - 1;
            if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<multi_texture_transform_node>::exposedfield<sfnode> >(
                            &multi_texture_transform_node::metadata)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<multi_texture_transform_node>::exposedfield<sfnode> >(
                            &multi_texture_transform_node::metadata)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<multi_texture_transform_node>::exposedfield<sfnode> >(
                            &multi_texture_transform_node::metadata)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<multi_texture_transform_node>::exposedfield<mfnode> >(
                            &multi_texture_transform_node::texture_transform_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<multi_texture_transform_node>::exposedfield<mfnode> >(
                            &multi_texture_transform_node::texture_transform_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<multi_texture_transform_node>::exposedfield<mfnode> >(
                            &multi_texture_transform_node::texture_transform_)));
            } else {
                throw unsupported_interface(*interface);
            }
        }
        return type;
    }


    /**
     * @brief @c node_metatype identifier.
     */
    const char * const texture_coordinate_generator_metatype::id =
        "urn:X-openvrml:node:TextureCoordinateGenerator";

    /**
     * @brief Construct.
     *
     * @param browser the @c browser associated with this
     *                @c texture_coordinate_generator_metatype.
     */
    texture_coordinate_generator_metatype::
    texture_coordinate_generator_metatype(openvrml::browser & browser):
        node_metatype(texture_coordinate_generator_metatype::id, browser)
    {}

    /**
     * @brief Destroy.
     */
    texture_coordinate_generator_metatype::~texture_coordinate_generator_metatype() throw ()
    {}

    /**
     * @brief Create a node_type.
     *
     * @param id            the name for the new node_type.
     * @param interfaces    the interfaces for the new node_type.
     *
     * @return a node_type_ptr to a node_type capable of creating TextureCoordinateGenerator nodes.
     *
     * @exception unsupported_interface if @p interfaces includes an interface
     *                                  not supported by texture_coordinate_generator_metatype.
     * @exception std::bad_alloc        if memory allocation fails.
     */
    const boost::shared_ptr<openvrml::node_type>
    texture_coordinate_generator_metatype::
    do_create_type(const std::string & id,
                   const node_interface_set & interfaces) const
        throw (unsupported_interface, std::bad_alloc)
    {
        typedef boost::array<node_interface, 3> supported_interfaces_t;
        static const supported_interfaces_t supported_interfaces = {
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "metadata"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfstring_id,
                           "mode"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mffloat_id,
                           "parameter")
        };
        typedef node_type_impl<texture_coordinate_generator_node> node_type_t;

        const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
        node_type_t & the_node_type = static_cast<node_type_t &>(*type);

        for (node_interface_set::const_iterator interface(interfaces.begin());
             interface != interfaces.end();
             ++interface) {
            supported_interfaces_t::const_iterator supported_interface =
                supported_interfaces.begin() - 1;
            if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<texture_coordinate_generator_node>::exposedfield<sfnode> >(
                            &texture_coordinate_generator_node::metadata)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<texture_coordinate_generator_node>::exposedfield<sfnode> >(
                            &texture_coordinate_generator_node::metadata)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<texture_coordinate_generator_node>::exposedfield<sfnode> >(
                            &texture_coordinate_generator_node::metadata)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<texture_coordinate_generator_node>::exposedfield<sfstring> >(
                            &texture_coordinate_generator_node::mode_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<texture_coordinate_generator_node>::exposedfield<sfstring> >(
                            &texture_coordinate_generator_node::mode_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<texture_coordinate_generator_node>::exposedfield<sfstring> >(
                            &texture_coordinate_generator_node::mode_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<texture_coordinate_generator_node>::exposedfield<mffloat> >(
                            &texture_coordinate_generator_node::parameter_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<texture_coordinate_generator_node>::exposedfield<mffloat> >(
                            &texture_coordinate_generator_node::parameter_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<texture_coordinate_generator_node>::exposedfield<mffloat> >(
                            &texture_coordinate_generator_node::parameter_)));
            } else {
                throw unsupported_interface(*interface);
            }
        }
        return type;
    }


    /**
     * @class multi_texture_node
     *
     * @brief Represents MultiTexture node instances.
     */

    /**
     * @var multi_texture_node::MultiTexture_metatype
     *
     * @brief Class object for MultiTexture nodes.
     */

    /**
     * @var multi_texture_node::alpha_
     *
     * @brief alpha exposedField
     */

    /**
     * @var multi_texture_node::color_
     *
     * @brief color exposedField
     */

    /**
     * @var multi_texture_node::function_
     *
     * @brief function exposedField
     */

    /**
     * @var multi_texture_node::mode_
     *
     * @brief mode exposedField
     */

    /**
     * @var multi_texture_node::source_
     *
     * @brief source exposedField
     */

    /**
     * @var multi_texture_node::texture_
     *
     * @brief texture exposedField
     */


    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope     the scope to which the node belongs.
     */
    multi_texture_node::
    multi_texture_node(const node_type & type,
                       const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        abstract_node<self_t>(type, scope),
        alpha_(*this, 1),
        color_(*this, color(1,1,1)),
        function_(*this),
        mode_(*this),
        source_(*this),
        texture_(*this)
    {}

    /**
     * @brief Destroy.
     */
    multi_texture_node::~multi_texture_node() throw ()
    {}


    /**
     * @class multi_texture_coordinate_node
     *
     * @brief Represents MultiTextureCoordinate node instances.
     */

    /**
     * @var multi_texture_coordinate_node::multi_texture_coordinate_metatype
     *
     * @brief Class object for MultiTextureCoordinate nodes.
     */

    /**
     * @var multi_texture_coordinate_node::tex_coord_
     *
     * @brief tex_coord exposedField
     */


    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope the scope to which the node belongs.
     */
    multi_texture_coordinate_node::
    multi_texture_coordinate_node(
        const node_type & type,
        const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        abstract_node<self_t>(type, scope),
        tex_coord_(*this)
    {}

    /**
     * @brief Destroy.
     */
    multi_texture_coordinate_node::~multi_texture_coordinate_node() throw ()
    {}


    /**
     * @class multi_texture_transform_node
     *
     * @brief Represents MultiTextureTransform node instances.
     */

    /**
     * @var multi_texture_transform_node::multi_texture_transform_metatype
     *
     * @brief Class object for MultiTextureTransform nodes.
     */

    /**
     * @var multi_texture_transform_node::texture_transform_
     *
     * @brief texture_transform exposedField
     */


    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope     the scope to which the node belongs.
     */
    multi_texture_transform_node::
    multi_texture_transform_node(
        const node_type & type,
        const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        abstract_node<self_t>(type, scope),
        texture_transform_(*this)
    {}

    /**
     * @brief Destroy.
     */
    multi_texture_transform_node::~multi_texture_transform_node() throw ()
    {}


    /**
     * @class texture_coordinate_generator_node
     *
     * @brief Represents TextureCoordinateGenerator node instances.
     */

    /**
     * @var texture_coordinate_generator_node::texture_coordinate_generator_metatype
     *
     * @brief Class object for TextureCoordinateGenerator nodes.
     */

    /**
     * @var texture_coordinate_generator_node::mode_
     *
     * @brief mode exposedField
     */

    /**
     * @var texture_coordinate_generator_node::parameter_
     *
     * @brief parameter exposedField
     */


    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope     the scope to which the node belongs.
     */
    texture_coordinate_generator_node::
    texture_coordinate_generator_node(
        const node_type & type,
        const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        abstract_node<self_t>(type, scope),
        mode_(*this, "SPHERE"),
        parameter_(*this)
    {}

    /**
     * @brief Destroy.
     */
    texture_coordinate_generator_node::~texture_coordinate_generator_node() throw ()
    {}
}
