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
# include "browser.h"
# include "node_impl_util.h"
# include "x3d_core.h"
    
using namespace openvrml;
using namespace openvrml::node_impl_util;
using namespace std;


namespace {

    /**
     * @brief Class object for MetadataDouble nodes.
     */
    class OPENVRML_LOCAL metadata_double_metatype : public node_metatype {
    public:
        static const char * const id;

        explicit metadata_double_metatype(openvrml::browser & browser);
        virtual ~metadata_double_metatype() OPENVRML_NOTHROW;

    private:
        virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
            OPENVRML_THROW2(unsupported_interface, std::bad_alloc);
    };


    /**
     * @brief Class object for MetadataFloat nodes.
     */
    class OPENVRML_LOCAL metadata_float_metatype : public node_metatype {
    public:
        static const char * const id;

        explicit metadata_float_metatype(openvrml::browser & browser);
        virtual ~metadata_float_metatype() OPENVRML_NOTHROW;

    private:
        virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
            OPENVRML_THROW2(unsupported_interface, std::bad_alloc);
    };


    /**
     * @brief Class object for MetadataInteger nodes.
     */
    class OPENVRML_LOCAL metadata_integer_metatype : public node_metatype {
    public:
        static const char * const id;

        explicit metadata_integer_metatype(openvrml::browser & browser);
        virtual ~metadata_integer_metatype() OPENVRML_NOTHROW;

    private:
        virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
            OPENVRML_THROW2(unsupported_interface, std::bad_alloc);
    };


    /**
     * @brief Class object for MetadataSet nodes.
     */
    class OPENVRML_LOCAL metadata_set_metatype : public node_metatype {
    public:
        static const char * const id;

        explicit metadata_set_metatype(openvrml::browser & browser);
        virtual ~metadata_set_metatype() OPENVRML_NOTHROW;

    private:
        virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
            OPENVRML_THROW2(unsupported_interface, std::bad_alloc);
    };


    /**
     * @brief Class object for MetadataString nodes.
     */
    class OPENVRML_LOCAL metadata_string_metatype : public node_metatype {
    public:
        static const char * const id;

        explicit metadata_string_metatype(openvrml::browser & browser);
        virtual ~metadata_string_metatype() OPENVRML_NOTHROW;

    private:
        virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
            OPENVRML_THROW2(unsupported_interface, std::bad_alloc);
    };
}

void register_core_node_metatypes(openvrml::browser & b)
{
    using boost::shared_ptr;
    using openvrml::node_metatype;
    b.add_node_metatype(metadata_double_metatype::id,
                        shared_ptr<node_metatype>(
                            new metadata_double_metatype(b)));
    b.add_node_metatype(metadata_float_metatype::id,
                        shared_ptr<node_metatype>(
                            new metadata_float_metatype(b)));
    b.add_node_metatype(metadata_integer_metatype::id,
                        shared_ptr<node_metatype>(
                            new metadata_integer_metatype(b)));
    b.add_node_metatype(metadata_set_metatype::id,
                        shared_ptr<node_metatype>(
                            new metadata_set_metatype(b)));
    b.add_node_metatype(metadata_string_metatype::id,
                        shared_ptr<node_metatype>(
                            new metadata_string_metatype(b)));
}

namespace {

    class OPENVRML_LOCAL metadata_double_node :
        public abstract_node<metadata_double_node> {

        friend class metadata_double_metatype;

        exposedfield<sfstring> name_;
        exposedfield<sfstring> reference_;
        exposedfield<mfdouble> value_;

    public:
        metadata_double_node(const node_type & type, 
                             const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~metadata_double_node() OPENVRML_NOTHROW;
    };

    class OPENVRML_LOCAL metadata_float_node :
        public abstract_node<metadata_float_node> {

        friend class metadata_float_metatype;

        exposedfield<sfstring> name_;
        exposedfield<sfstring> reference_;
        exposedfield<mffloat> value_;

    public:
        metadata_float_node(const node_type & type, 
                            const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~metadata_float_node() OPENVRML_NOTHROW;
    };

    class OPENVRML_LOCAL metadata_integer_node :
        public abstract_node<metadata_integer_node> {

        friend class metadata_integer_metatype;

        exposedfield<sfstring> name_;
        exposedfield<sfstring> reference_;
        exposedfield<mfint32> value_;

    public:
        metadata_integer_node(const node_type & type, 
                              const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~metadata_integer_node() OPENVRML_NOTHROW;
    };

    class OPENVRML_LOCAL metadata_set_node :
        public abstract_node<metadata_set_node> {

        friend class metadata_set_metatype;

        exposedfield<sfstring> name_;
        exposedfield<sfstring> reference_;
        exposedfield<mfnode> value_;

    public:
        metadata_set_node(const node_type & type, 
                          const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~metadata_set_node() OPENVRML_NOTHROW;
    };

    class OPENVRML_LOCAL metadata_string_node :
        public abstract_node<metadata_string_node> {

        friend class metadata_string_metatype;

        exposedfield<sfstring> name_;
        exposedfield<sfstring> reference_;
        exposedfield<mfstring> value_;

    public:
        metadata_string_node(const node_type & type, 
                             const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~metadata_string_node() OPENVRML_NOTHROW;
    };


    /**
     * @brief @c node_metatype identifier.
     */
    const char * const metadata_double_metatype::id =
        "urn:X-openvrml:node:MetadataDouble";

    /**
     * @brief Construct.
     *
     * @param browser the @c browser associated with this
     *                @c metadata_double_metatype.
     */
    metadata_double_metatype::metadata_double_metatype(openvrml::browser & browser):
        node_metatype(metadata_double_metatype::id, browser)
    {}

    /**
     * @brief Destroy.
     */
    metadata_double_metatype::~metadata_double_metatype() OPENVRML_NOTHROW
    {}

    /**
     * @brief Create a @c node_type.
     *
     * @param id            the name for the new @c node_type.
     * @param interfaces    the interfaces for the new @c node_type.
     *
     * @return a @c node_type capable of creating MetadataDouble nodes.
     *
     * @exception unsupported_interface if @p interfaces includes an interface
     *                                  not supported by
     *                                  @c metadata_double_metatype.
     * @exception std::bad_alloc        if memory allocation fails.
     */
    const boost::shared_ptr<openvrml::node_type>
    metadata_double_metatype::
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
                           field_value::sfstring_id,
                           "name"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfstring_id,
                           "reference"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mfdouble_id,
                           "value")
        };    
        typedef node_type_impl<metadata_double_node> node_type_t;

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
                        abstract_node<metadata_double_node>::exposedfield<sfnode> >(
                            &metadata_double_node::metadata)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<metadata_double_node>::exposedfield<sfnode> >(
                            &metadata_double_node::metadata)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<metadata_double_node>::exposedfield<sfnode> >(
                            &metadata_double_node::metadata)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<metadata_double_node>::exposedfield<sfstring> >(
                            &metadata_double_node::name_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<metadata_double_node>::exposedfield<sfstring> >(
                            &metadata_double_node::name_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<metadata_double_node>::exposedfield<sfstring> >(
                            &metadata_double_node::name_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<metadata_double_node>::exposedfield<sfstring> >(
                            &metadata_double_node::reference_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<metadata_double_node>::exposedfield<sfstring> >(
                            &metadata_double_node::reference_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<metadata_double_node>::exposedfield<sfstring> >(
                            &metadata_double_node::reference_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<metadata_double_node>::exposedfield<mfdouble> >(
                            &metadata_double_node::value_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<metadata_double_node>::exposedfield<mfdouble> >(
                            &metadata_double_node::value_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<metadata_double_node>::exposedfield<mfdouble> >(
                            &metadata_double_node::value_)));
            } else {
                throw unsupported_interface(*interface);
            }
        }        
        return type;
    }


    /**
     * @brief @c node_metatype identifier.
     */
    const char * const metadata_float_metatype::id =
        "urn:X-openvrml:node:MetadataFloat";

    /**
     * @brief Construct.
     *
     * @param browser the @c browser associated with this
     *                @c metadata_float_metatype.
     */
    metadata_float_metatype::metadata_float_metatype(openvrml::browser & browser):
        node_metatype(metadata_float_metatype::id, browser)
    {}

    /**
     * @brief Destroy.
     */
    metadata_float_metatype::~metadata_float_metatype() OPENVRML_NOTHROW
    {}

    /**
     * @brief Create a node_type.
     *
     * @param id            the name for the new node_type.
     * @param interfaces    the interfaces for the new node_type.
     *
     * @return a node_type_ptr to a node_type capable of creating MetadataFloat nodes.
     *
     * @exception unsupported_interface if @p interfaces includes an interface
     *                                  not supported by metadata_float_metatype.
     * @exception std::bad_alloc        if memory allocation fails.
     */
    const boost::shared_ptr<openvrml::node_type>
    metadata_float_metatype::do_create_type(const std::string & id,
                                         const node_interface_set & interfaces) const
        OPENVRML_THROW2(unsupported_interface, std::bad_alloc)
    {
        typedef boost::array<node_interface, 4> supported_interfaces_t;
        static const supported_interfaces_t supported_interfaces = {
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "metadata"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfstring_id,
                           "name"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfstring_id,
                           "reference"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mffloat_id,
                           "value")
        };    
        typedef node_type_impl<metadata_float_node> node_type_t;

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
                        abstract_node<metadata_float_node>::exposedfield<sfnode> >(
                            &metadata_float_node::metadata)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<metadata_float_node>::exposedfield<sfnode> >(
                            &metadata_float_node::metadata)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<metadata_float_node>::exposedfield<sfnode> >(
                            &metadata_float_node::metadata)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<metadata_float_node>::exposedfield<sfstring> >(
                            &metadata_float_node::name_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<metadata_float_node>::exposedfield<sfstring> >(
                            &metadata_float_node::name_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<metadata_float_node>::exposedfield<sfstring> >(
                            &metadata_float_node::name_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<metadata_float_node>::exposedfield<sfstring> >(
                            &metadata_float_node::reference_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<metadata_float_node>::exposedfield<sfstring> >(
                            &metadata_float_node::reference_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<metadata_float_node>::exposedfield<sfstring> >(
                            &metadata_float_node::reference_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<metadata_float_node>::exposedfield<mffloat> >(
                            &metadata_float_node::value_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<metadata_float_node>::exposedfield<mffloat> >(
                            &metadata_float_node::value_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<metadata_float_node>::exposedfield<mffloat> >(
                            &metadata_float_node::value_)));
            } else {
                throw unsupported_interface(*interface);
            }
        }        
        return type;
    }


    /**
     * @brief @c node_metatype identifier.
     */
    const char * const metadata_integer_metatype::id =
        "urn:X-openvrml:node:MetadataInteger";

    /**
     * @brief Construct.
     *
     * @param browser the @c browser associated with this
     *                @c metadata_integer_metatype.
     */
    metadata_integer_metatype::
    metadata_integer_metatype(openvrml::browser & browser):
        node_metatype(metadata_integer_metatype::id, browser)
    {}

    /**
     * @brief Destroy.
     */
    metadata_integer_metatype::~metadata_integer_metatype() OPENVRML_NOTHROW
    {}

    /**
     * @brief Create a node_type.
     *
     * @param id            the name for the new node_type.
     * @param interfaces    the interfaces for the new node_type.
     *
     * @return a node_type_ptr to a node_type capable of creating MetadataInteger nodes.
     *
     * @exception unsupported_interface if @p interfaces includes an interface
     *                                  not supported by metadata_integer_metatype.
     * @exception std::bad_alloc        if memory allocation fails.
     */
    const boost::shared_ptr<openvrml::node_type>
    metadata_integer_metatype::
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
                           field_value::sfstring_id,
                           "name"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfstring_id,
                           "reference"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mfint32_id,
                           "value")
        };    
        typedef node_type_impl<metadata_integer_node> node_type_t;

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
                        abstract_node<metadata_integer_node>::exposedfield<sfnode> >(
                            &metadata_integer_node::metadata)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<metadata_integer_node>::exposedfield<sfnode> >(
                            &metadata_integer_node::metadata)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<metadata_integer_node>::exposedfield<sfnode> >(
                            &metadata_integer_node::metadata)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<metadata_integer_node>::exposedfield<sfstring> >(
                            &metadata_integer_node::name_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<metadata_integer_node>::exposedfield<sfstring> >(
                            &metadata_integer_node::name_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<metadata_integer_node>::exposedfield<sfstring> >(
                            &metadata_integer_node::name_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<metadata_integer_node>::exposedfield<sfstring> >(
                            &metadata_integer_node::reference_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<metadata_integer_node>::exposedfield<sfstring> >(
                            &metadata_integer_node::reference_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<metadata_integer_node>::exposedfield<sfstring> >(
                            &metadata_integer_node::reference_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<metadata_integer_node>::exposedfield<mfint32> >(
                            &metadata_integer_node::value_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<metadata_integer_node>::exposedfield<mfint32> >(
                            &metadata_integer_node::value_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<metadata_integer_node>::exposedfield<mfint32> >(
                            &metadata_integer_node::value_)));
            } else {
                throw unsupported_interface(*interface);
            }
        }        
        return type;
    }

    /**
     * @brief @c node_metatype identifier.
     */
    const char * const metadata_set_metatype::id =
        "urn:X-openvrml:node:MetadataSet";

    /**
     * @brief Construct.
     *
     * @param browser the @c browser associated with this
     *                @c metadata_set_metatype.
     */
    metadata_set_metatype::metadata_set_metatype(openvrml::browser & browser):
        node_metatype(metadata_set_metatype::id, browser)
    {}

    /**
     * @brief Destroy.
     */
    metadata_set_metatype::~metadata_set_metatype() OPENVRML_NOTHROW
    {}

    /**
     * @brief Create a node_type.
     *
     * @param id            the name for the new node_type.
     * @param interfaces    the interfaces for the new node_type.
     *
     * @return a node_type_ptr to a node_type capable of creating MetadataSet nodes.
     *
     * @exception unsupported_interface if @p interfaces includes an interface
     *                                  not supported by metadata_set_metatype.
     * @exception std::bad_alloc        if memory allocation fails.
     */
    const boost::shared_ptr<openvrml::node_type>
    metadata_set_metatype::
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
                           field_value::sfstring_id,
                           "name"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfstring_id,
                           "reference"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mfnode_id,
                           "value")
        };    
        typedef node_type_impl<metadata_set_node> node_type_t;

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
                        abstract_node<metadata_set_node>::exposedfield<sfnode> >(
                            &metadata_set_node::metadata)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<metadata_set_node>::exposedfield<sfnode> >(
                            &metadata_set_node::metadata)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<metadata_set_node>::exposedfield<sfnode> >(
                            &metadata_set_node::metadata)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<metadata_set_node>::exposedfield<sfstring> >(
                            &metadata_set_node::name_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<metadata_set_node>::exposedfield<sfstring> >(
                            &metadata_set_node::name_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<metadata_set_node>::exposedfield<sfstring> >(
                            &metadata_set_node::name_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<metadata_set_node>::exposedfield<sfstring> >(
                            &metadata_set_node::reference_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<metadata_set_node>::exposedfield<sfstring> >(
                            &metadata_set_node::reference_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<metadata_set_node>::exposedfield<sfstring> >(
                            &metadata_set_node::reference_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<metadata_set_node>::exposedfield<mfnode> >(
                            &metadata_set_node::value_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<metadata_set_node>::exposedfield<mfnode> >(
                            &metadata_set_node::value_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<metadata_set_node>::exposedfield<mfnode> >(
                            &metadata_set_node::value_)));
            } else {
                throw unsupported_interface(*interface);
            }
        }        
        return type;
    }


    /**
     * @brief @c node_metatype identifier.
     */
    const char * const metadata_string_metatype::id =
        "urn:X-openvrml:node:MetadataString";

    /**
     * @brief Construct.
     *
     * @param browser the @c browser associated with this
     *                @c metadata_string_metatype.
     */
    metadata_string_metatype::metadata_string_metatype(openvrml::browser & browser):
        node_metatype(metadata_string_metatype::id, browser)
    {}

    /**
     * @brief Destroy.
     */
    metadata_string_metatype::~metadata_string_metatype() OPENVRML_NOTHROW
    {}

    /**
     * @brief Create a node_type.
     *
     * @param id            the name for the new node_type.
     * @param interfaces    the interfaces for the new node_type.
     *
     * @return a node_type_ptr to a node_type capable of creating MetadataString nodes.
     *
     * @exception unsupported_interface if @p interfaces includes an interface
     *                                  not supported by metadata_string_metatype.
     * @exception std::bad_alloc        if memory allocation fails.
     */
    const boost::shared_ptr<openvrml::node_type>
    metadata_string_metatype::
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
                           field_value::sfstring_id,
                           "name"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfstring_id,
                           "reference"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mfstring_id,
                           "value")
        };    
        typedef node_type_impl<metadata_string_node> node_type_t;

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
                        abstract_node<metadata_string_node>::exposedfield<sfnode> >(
                            &metadata_string_node::metadata)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<metadata_string_node>::exposedfield<sfnode> >(
                            &metadata_string_node::metadata)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<metadata_string_node>::exposedfield<sfnode> >(
                            &metadata_string_node::metadata)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<metadata_string_node>::exposedfield<sfstring> >(
                            &metadata_string_node::name_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<metadata_string_node>::exposedfield<sfstring> >(
                            &metadata_string_node::name_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<metadata_string_node>::exposedfield<sfstring> >(
                            &metadata_string_node::name_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<metadata_string_node>::exposedfield<sfstring> >(
                            &metadata_string_node::reference_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<metadata_string_node>::exposedfield<sfstring> >(
                            &metadata_string_node::reference_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<metadata_string_node>::exposedfield<sfstring> >(
                            &metadata_string_node::reference_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<metadata_string_node>::exposedfield<mfstring> >(
                            &metadata_string_node::value_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<metadata_string_node>::exposedfield<mfstring> >(
                            &metadata_string_node::value_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<metadata_string_node>::exposedfield<mfstring> >(
                            &metadata_string_node::value_)));
            } else {
                throw unsupported_interface(*interface);
            }
        }        
        return type;
    }


    /**
     * @class metadata_double_node
     *
     * @brief Represents MetadataDouble node instances.
     */

    /**
     * @var metadata_double_node::metadata_double_metatype
     *
     * @brief Class object for MetadataDouble nodes.
     */

    /**
     * @var metadata_double_node::name_
     *
     * @brief name exposedField
     */

    /**
     * @var metadata_double_node::reference_
     *
     * @brief reference exposedField
     */

    /**
     * @var metadata_double_node::value_
     *
     * @brief value exposedField
     */

    
    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope     the scope to which the node belongs.
     */
    metadata_double_node::
    metadata_double_node(const node_type & type,
                         const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        abstract_node<self_t>(type, scope),
        name_(*this),
        reference_(*this),
        value_(*this)
    {}

    /**
     * @brief Destroy.
     */
    metadata_double_node::~metadata_double_node() OPENVRML_NOTHROW
    {}


    /**
     * @class metadata_float_node
     *
     * @brief Represents MetadataFloat node instances.
     */

    /**
     * @var metadata_float_node::metadata_float_metatype
     *
     * @brief Class object for MetadataFloat nodes.
     */

    /**
     * @var metadata_float_node::name_
     *
     * @brief name exposedField
     */

    /**
     * @var metadata_float_node::reference_
     *
     * @brief reference exposedField
     */

    /**
     * @var metadata_float_node::value_
     *
     * @brief value exposedField
     */

    
    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope     the scope to which the node belongs.
     */
    metadata_float_node::
    metadata_float_node(const node_type & type,
                        const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        abstract_node<self_t>(type, scope),
        name_(*this),
        reference_(*this),
        value_(*this)
    {}

    /**
     * @brief Destroy.
     */
    metadata_float_node::~metadata_float_node() OPENVRML_NOTHROW
    {}


    /**
     * @class metadata_integer_node
     *
     * @brief Represents MetadataInteger node instances.
     */

    /**
     * @var metadata_integer_node::metadata_integer_metatype
     *
     * @brief Class object for MetadataInteger nodes.
     */

    /**
     * @var metadata_integer_node::name_
     *
     * @brief name exposedField
     */

    /**
     * @var metadata_integer_node::reference_
     *
     * @brief reference exposedField
     */

    /**
     * @var metadata_integer_node::value_
     *
     * @brief value exposedField
     */

    
    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope the scope to which the node belongs.
     */
    metadata_integer_node::
    metadata_integer_node(const node_type & type,
                          const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        abstract_node<self_t>(type, scope),
        name_(*this),
        reference_(*this),
        value_(*this)
    {}

    /**
     * @brief Destroy.
     */
    metadata_integer_node::~metadata_integer_node() OPENVRML_NOTHROW
    {}


    /**
     * @class metadata_set_node
     *
     * @brief Represents MetadataSet node instances.
     */

    /**
     * @var metadata_set_node::metadata_set_metatype
     *
     * @brief Class object for MetadataSet nodes.
     */

    /**
     * @var metadata_set_node::name_
     *
     * @brief name exposedField
     */

    /**
     * @var metadata_set_node::reference_
     *
     * @brief reference exposedField
     */

    /**
     * @var metadata_set_node::value_
     *
     * @brief value exposedField
     */

    
    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope     the scope to which the node belongs.
     */
    metadata_set_node::
    metadata_set_node(const node_type & type,
                      const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        abstract_node<self_t>(type, scope),
        name_(*this),
        reference_(*this),
        value_(*this)
    {}

    /**
     * @brief Destroy.
     */
    metadata_set_node::~metadata_set_node() OPENVRML_NOTHROW
    {}


    /**
     * @class metadata_string_node
     *
     * @brief Represents MetadataString node instances.
     */

    /**
     * @var metadata_string_node::metadata_string_metatype
     *
     * @brief Class object for MetadataString nodes.
     */

    /**
     * @var metadata_string_node::name_
     *
     * @brief name exposedField
     */

    /**
     * @var metadata_string_node::reference_
     *
     * @brief reference exposedField
     */

    /**
     * @var metadata_string_node::value_
     *
     * @brief value exposedField
     */

    
    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope     the scope to which the node belongs.
     */
    metadata_string_node::
    metadata_string_node(const node_type & type,
                         const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        abstract_node<self_t>(type, scope),
        name_(*this),
        reference_(*this),
        value_(*this)
    {}

    /**
     * @brief Destroy.
     */
    metadata_string_node::~metadata_string_node() OPENVRML_NOTHROW
    {}
}
