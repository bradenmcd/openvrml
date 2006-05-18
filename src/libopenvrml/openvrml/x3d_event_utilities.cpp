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
# include <openvrml/browser.h>
# include "node_impl_util.h"
# include "x3d_event_utilities.h"

using namespace openvrml;
using namespace openvrml::node_impl_util;
using namespace std;

namespace {

    /**
     * @brief Class object for BooleanFilter nodes.
     */
    class OPENVRML_LOCAL boolean_filter_metatype : public node_metatype {
    public:
        static const char * const id;

        explicit boolean_filter_metatype(openvrml::browser & browser);
        virtual ~boolean_filter_metatype() OPENVRML_NOTHROW;

    private:
        virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
            OPENVRML_THROW2(unsupported_interface, std::bad_alloc);
    };


    /**
     * @brief Class object for BooleanSequencer nodes.
     */
    class OPENVRML_LOCAL boolean_sequencer_metatype : public node_metatype {
    public:
        static const char * const id;

        explicit boolean_sequencer_metatype(openvrml::browser & browser);
        virtual ~boolean_sequencer_metatype() OPENVRML_NOTHROW;

    private:
        virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
            OPENVRML_THROW2(unsupported_interface, std::bad_alloc);
    };


    /**
     * @brief Class object for BooleanToggle nodes.
     */
    class OPENVRML_LOCAL boolean_toggle_metatype : public node_metatype {
    public:
        static const char * const id;

        explicit boolean_toggle_metatype(openvrml::browser & browser);
        virtual ~boolean_toggle_metatype() OPENVRML_NOTHROW;

    private:
        virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
            OPENVRML_THROW2(unsupported_interface, std::bad_alloc);
    };


    /**
     * @brief Class object for BooleanTrigger nodes.
     */
    class OPENVRML_LOCAL boolean_trigger_metatype : public node_metatype {
    public:
        static const char * const id;

        explicit boolean_trigger_metatype(openvrml::browser & browser);
        virtual ~boolean_trigger_metatype() OPENVRML_NOTHROW;

    private:
        virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
            OPENVRML_THROW2(unsupported_interface, std::bad_alloc);
    };


    /**
     * @brief Class object for IntegerSequencer nodes.
     */
    class OPENVRML_LOCAL integer_sequencer_metatype : public node_metatype {
    public:
        static const char * const id;

        explicit integer_sequencer_metatype(openvrml::browser & browser);
        virtual ~integer_sequencer_metatype() OPENVRML_NOTHROW;

    private:
        virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
            OPENVRML_THROW2(unsupported_interface, std::bad_alloc);
    };


    /**
     * @brief Class object for IntegerTrigger nodes.
     */
    class OPENVRML_LOCAL integer_trigger_metatype : public node_metatype {
    public:
        static const char * const id;

        explicit integer_trigger_metatype(openvrml::browser & browser);
        virtual ~integer_trigger_metatype() OPENVRML_NOTHROW;

    private:
        virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
            OPENVRML_THROW2(unsupported_interface, std::bad_alloc);
    };


    /**
     * @brief Class object for TimeTrigger nodes.
     */
    class OPENVRML_LOCAL time_trigger_metatype : public node_metatype {
    public:
        static const char * const id;

        explicit time_trigger_metatype(openvrml::browser & browser);
        virtual ~time_trigger_metatype() OPENVRML_NOTHROW;

    private:
        virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
            OPENVRML_THROW2(unsupported_interface, std::bad_alloc);
    };
}

void register_event_utilities_node_metatypes(openvrml::browser & b)
{
    using boost::shared_ptr;
    using openvrml::node_metatype;
    b.add_node_metatype(boolean_filter_metatype::id,
                     shared_ptr<node_metatype>(new boolean_filter_metatype(b)));
    b.add_node_metatype(boolean_sequencer_metatype::id,
                     shared_ptr<node_metatype>(new boolean_sequencer_metatype(b)));
    b.add_node_metatype(boolean_toggle_metatype::id,
                     shared_ptr<node_metatype>(new boolean_toggle_metatype(b)));
    b.add_node_metatype(boolean_trigger_metatype::id,
                     shared_ptr<node_metatype>(new boolean_trigger_metatype(b)));
    b.add_node_metatype(integer_sequencer_metatype::id,
                     shared_ptr<node_metatype>(new integer_sequencer_metatype(b)));
    b.add_node_metatype(integer_trigger_metatype::id,
                     shared_ptr<node_metatype>(new integer_trigger_metatype(b)));
    b.add_node_metatype(time_trigger_metatype::id,
                     shared_ptr<node_metatype>(new time_trigger_metatype(b)));
}

namespace {

    using namespace openvrml_;

    class OPENVRML_LOCAL boolean_filter_node : public abstract_node<boolean_filter_node>,
                                               public child_node {
        friend class boolean_filter_metatype;

        class set_boolean_listener : public event_listener_base<self_t>,
                                     public sfbool_listener {
        public:
            explicit set_boolean_listener(self_t & node);
            virtual ~set_boolean_listener() OPENVRML_NOTHROW;

        private:
            virtual void do_process_event(const sfbool & fraction,
                                          double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        set_boolean_listener set_boolean_listener_;
        sfbool input_false_;
        sfbool_emitter input_false_emitter_;
        sfbool input_negate_;
        sfbool_emitter input_negate_emitter_;
        sfbool input_true_;
        sfbool_emitter input_true_emitter_;

    public:
        boolean_filter_node(const node_type & type,
                            const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~boolean_filter_node() OPENVRML_NOTHROW;
    };

    class OPENVRML_LOCAL boolean_sequencer_node :
        public abstract_node<boolean_sequencer_node>,
        public child_node {

        friend class boolean_sequencer_metatype;

        class next_listener : public event_listener_base<self_t>,
                              public sfbool_listener {
        public:
            explicit next_listener(self_t & node);
            virtual ~next_listener() OPENVRML_NOTHROW;

        private:
            virtual void do_process_event(const sfbool & fraction,
                                          double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        class previous_listener : public event_listener_base<self_t>,
                                  public sfbool_listener {
        public:
            explicit previous_listener(self_t & node);
            virtual ~previous_listener() OPENVRML_NOTHROW;

        private:
            virtual void do_process_event(const sfbool & fraction,
                                          double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        class set_fraction_listener : public event_listener_base<self_t>,
                                      public sffloat_listener {
        public:
            explicit set_fraction_listener(self_t & node);
            virtual ~set_fraction_listener() OPENVRML_NOTHROW;

        private:
            virtual void do_process_event(const sffloat & fraction,
                                          double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        next_listener next_listener_;
        previous_listener previous_listener_;
        set_fraction_listener set_fraction_listener_;
        exposedfield<mffloat> key_;
        exposedfield<mfbool> key_value_;
        sfbool value_changed_;
        sfbool_emitter value_changed_emitter_;

    public:
        boolean_sequencer_node(const node_type & type,
                               const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~boolean_sequencer_node() OPENVRML_NOTHROW;
    };


    class OPENVRML_LOCAL boolean_toggle_node :
        public abstract_node<boolean_toggle_node>,
        public child_node {

        friend class boolean_toggle_metatype;

        class set_boolean_listener : public event_listener_base<self_t>,
                                     public sfbool_listener {
        public:
            explicit set_boolean_listener(self_t & node);
            virtual ~set_boolean_listener() OPENVRML_NOTHROW;

        private:
            virtual void do_process_event(const sfbool & fraction,
                                          double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        set_boolean_listener set_boolean_listener_;
        exposedfield<sfbool> toggle_;

    public:
        boolean_toggle_node(const node_type & type,
                            const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~boolean_toggle_node() OPENVRML_NOTHROW;
    };


    class OPENVRML_LOCAL boolean_trigger_node :
        public abstract_node<boolean_trigger_node>,
        public child_node {

        friend class boolean_trigger_metatype;

        class set_trigger_time_listener : public event_listener_base<self_t>,
                                          public sftime_listener {
        public:
            explicit set_trigger_time_listener(self_t & node);
            virtual ~set_trigger_time_listener() OPENVRML_NOTHROW;

        private:
            virtual void do_process_event(const sftime & fraction,
                                          double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        set_trigger_time_listener set_trigger_time_listener_;
        sfbool trigger_true_;
        sfbool_emitter trigger_true_emitter_;

    public:
        boolean_trigger_node(const node_type & type,
                             const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~boolean_trigger_node() OPENVRML_NOTHROW;
    };


    class OPENVRML_LOCAL integer_sequencer_node :
        public abstract_node<integer_sequencer_node>,
        public child_node {

        friend class integer_sequencer_metatype;

        class next_listener : public event_listener_base<self_t>,
                              public sfbool_listener {
        public:
            explicit next_listener(self_t & node);
            virtual ~next_listener() OPENVRML_NOTHROW;

        private:
            virtual void do_process_event(const sfbool & fraction,
                                          double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        class previous_listener : public event_listener_base<self_t>,
                                  public sfbool_listener {
        public:
            explicit previous_listener(self_t & node);
            virtual ~previous_listener() OPENVRML_NOTHROW;

        private:
            virtual void do_process_event(const sfbool & fraction,
                                          double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        class set_fraction_listener : public event_listener_base<self_t>,
                                      public sffloat_listener {
        public:
            explicit set_fraction_listener(self_t & node);
            virtual ~set_fraction_listener() OPENVRML_NOTHROW;

        private:
            virtual void do_process_event(const sffloat & fraction,
                                          double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        next_listener next_listener_;
        previous_listener previous_listener_;
        set_fraction_listener set_fraction_listener_;
        exposedfield<mffloat> key_;
        exposedfield<mfint32> key_value_;
        sfint32 value_changed_;
        sfint32_emitter value_changed_emitter_;

    public:
        integer_sequencer_node(
            const node_type & type,
            const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~integer_sequencer_node() OPENVRML_NOTHROW;
    };


    class OPENVRML_LOCAL integer_trigger_node :
        public abstract_node<integer_trigger_node>,
        public child_node {

        friend class integer_trigger_metatype;

        class set_boolean_listener : public event_listener_base<self_t>,
                                     public sfbool_listener {
        public:
            explicit set_boolean_listener(self_t & node);
            virtual ~set_boolean_listener() OPENVRML_NOTHROW;

        private:
            virtual void do_process_event(const sfbool & fraction,
                                          double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        set_boolean_listener set_boolean_listener_;
        exposedfield<mfint32> integer_key_;
        sfint32 trigger_value_;
        sfint32_emitter trigger_value_emitter_;

    public:
        integer_trigger_node(const node_type & type,
                             const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~integer_trigger_node() OPENVRML_NOTHROW;
    };


    class OPENVRML_LOCAL time_trigger_node :
        public abstract_node<time_trigger_node>,
        public child_node {

        friend class time_trigger_metatype;

        class set_boolean_listener : public event_listener_base<self_t>,
                                     public sfbool_listener {
        public:
            explicit set_boolean_listener(self_t & node);
            virtual ~set_boolean_listener() OPENVRML_NOTHROW;

        private:
            virtual void do_process_event(const sfbool & fraction,
                                          double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        set_boolean_listener set_boolean_listener_;
        sftime trigger_time_;
        sftime_emitter trigger_time_emitter_;

    public:
        time_trigger_node(const node_type & type,
                          const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~time_trigger_node() OPENVRML_NOTHROW;
    };


    /**
     * @brief @c node_metatype identifier.
     */
    const char * const boolean_filter_metatype::id =
        "urn:X-openvrml:node:BooleanFilter";

    /**
     * @brief Construct.
     *
     * @param browser the @c browser associated with this
     *                @c boolean_filter_metatype.
     */
    boolean_filter_metatype::boolean_filter_metatype(openvrml::browser & browser):
        node_metatype(boolean_filter_metatype::id, browser)
    {}

    /**
     * @brief Destroy.
     */
    boolean_filter_metatype::~boolean_filter_metatype() OPENVRML_NOTHROW
    {}

    /**
     * @brief Create a node_type.
     *
     * @param id            the name for the new node_type.
     * @param interfaces    the interfaces for the new node_type.
     *
     * @return a node_type_ptr to a node_type capable of creating BooleanFilter nodes.
     *
     * @exception unsupported_interface if @p interfaces includes an interface
     *                                  not supported by boolean_filter_metatype.
     * @exception std::bad_alloc        if memory allocation fails.
     */
    const boost::shared_ptr<openvrml::node_type>
    boolean_filter_metatype::
    do_create_type(const std::string & id,
                   const node_interface_set & interfaces) const
        OPENVRML_THROW2(unsupported_interface, std::bad_alloc)
    {
        typedef boost::array<node_interface, 5> supported_interfaces_t;
        static const supported_interfaces_t supported_interfaces = {
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "metadata"),
            node_interface(node_interface::eventin_id,
                           field_value::sfbool_id,
                           "set_boolean"),
            node_interface(node_interface::eventout_id,
                           field_value::sfbool_id,
                           "inputFalse"),
            node_interface(node_interface::eventout_id,
                           field_value::sfbool_id,
                           "inputNegate"),
            node_interface(node_interface::eventout_id,
                           field_value::sfbool_id,
                           "inputTrue")
        };
        typedef node_type_impl<boolean_filter_node> node_type_t;

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
                        abstract_node<boolean_filter_node>::exposedfield<sfnode> >(
                            &boolean_filter_node::metadata)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<boolean_filter_node>::exposedfield<sfnode> >(
                            &boolean_filter_node::metadata)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<boolean_filter_node>::exposedfield<sfnode> >(
                            &boolean_filter_node::metadata)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventin(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        boolean_filter_node::set_boolean_listener>(
                            &boolean_filter_node::set_boolean_listener_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventout(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        boolean_filter_node::sfbool_emitter>(
                            &boolean_filter_node::input_false_emitter_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventout(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        boolean_filter_node::sfbool_emitter>(
                            &boolean_filter_node::input_negate_emitter_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventout(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        boolean_filter_node::sfbool_emitter>(
                            &boolean_filter_node::input_true_emitter_)));
            } else {
                throw unsupported_interface(*interface);
            }
        }
        return type;
    }


    /**
     * @brief @c node_metatype identifier.
     */
    const char * const boolean_sequencer_metatype::id =
        "urn:X-openvrml:node:BooleanSequencer";

    /**
     * @brief Construct.
     *
     * @param browser the @c browser associated with this
     *                @c boolean_sequencer_metatype.
     */
    boolean_sequencer_metatype::boolean_sequencer_metatype(openvrml::browser & browser):
        node_metatype(boolean_sequencer_metatype::id, browser)
    {}

    /**
     * @brief Destroy.
     */
    boolean_sequencer_metatype::~boolean_sequencer_metatype() OPENVRML_NOTHROW
    {}

    /**
     * @brief Create a node_type.
     *
     * @param id            the name for the new node_type.
     * @param interfaces    the interfaces for the new node_type.
     *
     * @return a node_type_ptr to a node_type capable of creating BooleanSequencer nodes.
     *
     * @exception unsupported_interface if @p interfaces includes an interface
     *                                  not supported by boolean_sequencer_metatype.
     * @exception std::bad_alloc        if memory allocation fails.
     */
    const boost::shared_ptr<openvrml::node_type>
    boolean_sequencer_metatype::
    do_create_type(const std::string & id,
                   const node_interface_set & interfaces) const
        OPENVRML_THROW2(unsupported_interface, std::bad_alloc)
    {
        typedef boost::array<node_interface, 7> supported_interfaces_t;
        static const supported_interfaces_t supported_interfaces = {
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "metadata"),
            node_interface(node_interface::eventin_id,
                           field_value::sfbool_id,
                           "next"),
            node_interface(node_interface::eventin_id,
                           field_value::sfbool_id,
                           "previous"),
            node_interface(node_interface::eventin_id,
                           field_value::sffloat_id,
                           "set_fraction"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mffloat_id,
                           "key"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mfbool_id,
                           "keyValue"),
            node_interface(node_interface::eventout_id,
                           field_value::sfbool_id,
                           "value_changed")
        };
        typedef node_type_impl<boolean_sequencer_node> node_type_t;

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
                        abstract_node<boolean_sequencer_node>::exposedfield<sfnode> >(
                            &boolean_sequencer_node::metadata)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<boolean_sequencer_node>::exposedfield<sfnode> >(
                            &boolean_sequencer_node::metadata)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<boolean_sequencer_node>::exposedfield<sfnode> >(
                            &boolean_sequencer_node::metadata)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventin(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        boolean_sequencer_node::next_listener>(
                            &boolean_sequencer_node::next_listener_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventin(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        boolean_sequencer_node::previous_listener>(
                            &boolean_sequencer_node::previous_listener_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventin(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        boolean_sequencer_node::set_fraction_listener>(
                            &boolean_sequencer_node::set_fraction_listener_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<boolean_sequencer_node>::exposedfield<mffloat> >(
                            &boolean_sequencer_node::key_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<boolean_sequencer_node>::exposedfield<mffloat> >(
                            &boolean_sequencer_node::key_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<boolean_sequencer_node>::exposedfield<mffloat> >(
                            &boolean_sequencer_node::key_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<boolean_sequencer_node>::exposedfield<mfbool> >(
                            &boolean_sequencer_node::key_value_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<boolean_sequencer_node>::exposedfield<mfbool> >(
                            &boolean_sequencer_node::key_value_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<boolean_sequencer_node>::exposedfield<mfbool> >(
                            &boolean_sequencer_node::key_value_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventout(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        boolean_sequencer_node::sfbool_emitter>(
                            &boolean_sequencer_node::value_changed_emitter_)));
            } else {
                throw unsupported_interface(*interface);
            }
        }
        return type;
    }


    /**
     * @brief @c node_metatype identifier.
     */
    const char * const boolean_toggle_metatype::id =
        "urn:X-openvrml:node:BooleanToggle";

    /**
     * @brief Construct.
     *
     * @param browser the @c browser associated with
     *                @c this boolean_toggle_metatype.
     */
    boolean_toggle_metatype::boolean_toggle_metatype(openvrml::browser & browser):
        node_metatype(boolean_toggle_metatype::id, browser)
    {}

    /**
     * @brief Destroy.
     */
    boolean_toggle_metatype::~boolean_toggle_metatype() OPENVRML_NOTHROW
    {}

    /**
     * @brief Create a node_type.
     *
     * @param id            the name for the new node_type.
     * @param interfaces    the interfaces for the new node_type.
     *
     * @return a node_type_ptr to a node_type capable of creating BooleanToggle nodes.
     *
     * @exception unsupported_interface if @p interfaces includes an interface
     *                                  not supported by boolean_toggle_metatype.
     * @exception std::bad_alloc        if memory allocation fails.
     */
    const boost::shared_ptr<openvrml::node_type>
    boolean_toggle_metatype::
    do_create_type(const std::string & id,
                   const node_interface_set & interfaces) const
        OPENVRML_THROW2(unsupported_interface, std::bad_alloc)
    {
        typedef boost::array<node_interface, 3> supported_interfaces_t;
        static const supported_interfaces_t supported_interfaces = {
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "metadata"),
            node_interface(node_interface::eventin_id,
                           field_value::sfbool_id,
                           "set_boolean"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfbool_id,
                           "toggle")
        };
        typedef node_type_impl<boolean_toggle_node> node_type_t;

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
                        abstract_node<boolean_toggle_node>::exposedfield<sfnode> >(
                            &boolean_toggle_node::metadata)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<boolean_toggle_node>::exposedfield<sfnode> >(
                            &boolean_toggle_node::metadata)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<boolean_toggle_node>::exposedfield<sfnode> >(
                            &boolean_toggle_node::metadata)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventin(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        boolean_toggle_node::set_boolean_listener>(
                            &boolean_toggle_node::set_boolean_listener_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<boolean_toggle_node>::exposedfield<sfbool> >(
                            &boolean_toggle_node::toggle_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<boolean_toggle_node>::exposedfield<sfbool> >(
                            &boolean_toggle_node::toggle_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<boolean_toggle_node>::exposedfield<sfbool> >(
                            &boolean_toggle_node::toggle_)));
            } else {
                throw unsupported_interface(*interface);
            }
        }
        return type;
    }


    /**
     * @brief @c node_metatype identifier.
     */
    const char * const boolean_trigger_metatype::id =
        "urn:X-openvrml:node:BooleanTrigger";

    /**
     * @brief Construct.
     *
     * @param browser the @c browser associated with this
     *                @c boolean_trigger_metatype.
     */
    boolean_trigger_metatype::boolean_trigger_metatype(openvrml::browser & browser):
        node_metatype(boolean_trigger_metatype::id, browser)
    {}

    /**
     * @brief Destroy.
     */
    boolean_trigger_metatype::~boolean_trigger_metatype() OPENVRML_NOTHROW
    {}

    /**
     * @brief Create a node_type.
     *
     * @param id            the name for the new node_type.
     * @param interfaces    the interfaces for the new node_type.
     *
     * @return a node_type_ptr to a node_type capable of creating BooleanTrigger nodes.
     *
     * @exception unsupported_interface if @p interfaces includes an interface
     *                                  not supported by boolean_trigger_metatype.
     * @exception std::bad_alloc        if memory allocation fails.
     */
    const boost::shared_ptr<openvrml::node_type>
    boolean_trigger_metatype::
    do_create_type(const std::string & id,
                   const node_interface_set & interfaces) const
        OPENVRML_THROW2(unsupported_interface, std::bad_alloc)
    {
        typedef boost::array<node_interface, 3> supported_interfaces_t;
        static const supported_interfaces_t supported_interfaces = {
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "metadata"),
            node_interface(node_interface::eventin_id,
                           field_value::sftime_id,
                           "set_triggerTime"),
            node_interface(node_interface::eventout_id,
                           field_value::sfbool_id,
                           "triggerTrue")
        };
        typedef node_type_impl<boolean_trigger_node> node_type_t;

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
                        abstract_node<boolean_trigger_node>::exposedfield<sfnode> >(
                            &boolean_trigger_node::metadata)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<boolean_trigger_node>::exposedfield<sfnode> >(
                            &boolean_trigger_node::metadata)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<boolean_trigger_node>::exposedfield<sfnode> >(
                            &boolean_trigger_node::metadata)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventin(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        boolean_trigger_node::set_trigger_time_listener>(
                            &boolean_trigger_node::set_trigger_time_listener_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventout(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        boolean_trigger_node::sfbool_emitter>(
                            &boolean_trigger_node::trigger_true_emitter_)));
            } else {
                throw unsupported_interface(*interface);
            }
        }
        return type;
    }


    /**
     * @brief @c node_metatype identifier.
     */
    const char * const integer_sequencer_metatype::id =
        "urn:X-openvrml:node:IntegerSequencer";

    /**
     * @brief Construct.
     *
     * @param browser the @c browser associated with this
     *                @c integer_sequencer_metatype.
     */
    integer_sequencer_metatype::
    integer_sequencer_metatype(openvrml::browser & browser):
        node_metatype(integer_sequencer_metatype::id, browser)
    {}

    /**
     * @brief Destroy.
     */
    integer_sequencer_metatype::~integer_sequencer_metatype() OPENVRML_NOTHROW
    {}

    /**
     * @brief Create a node_type.
     *
     * @param id            the name for the new node_type.
     * @param interfaces    the interfaces for the new node_type.
     *
     * @return a node_type_ptr to a node_type capable of creating IntegerSequencer nodes.
     *
     * @exception unsupported_interface if @p interfaces includes an interface
     *                                  not supported by integer_sequencer_metatype.
     * @exception std::bad_alloc        if memory allocation fails.
     */
    const boost::shared_ptr<openvrml::node_type>
    integer_sequencer_metatype::
    do_create_type(const std::string & id,
                   const node_interface_set & interfaces) const
        OPENVRML_THROW2(unsupported_interface, std::bad_alloc)
    {
        typedef boost::array<node_interface, 7> supported_interfaces_t;
        static const supported_interfaces_t supported_interfaces = {
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "metadata"),
            node_interface(node_interface::eventin_id,
                           field_value::sfbool_id,
                           "next"),
            node_interface(node_interface::eventin_id,
                           field_value::sfbool_id,
                           "previous"),
            node_interface(node_interface::eventin_id,
                           field_value::sffloat_id,
                           "set_fraction"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mffloat_id,
                           "key"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mfint32_id,
                           "keyValue"),
            node_interface(node_interface::eventout_id,
                           field_value::sfint32_id,
                           "value_changed")
        };
        typedef node_type_impl<integer_sequencer_node> node_type_t;

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
                        abstract_node<integer_sequencer_node>::exposedfield<sfnode> >(
                            &integer_sequencer_node::metadata)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<integer_sequencer_node>::exposedfield<sfnode> >(
                            &integer_sequencer_node::metadata)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<integer_sequencer_node>::exposedfield<sfnode> >(
                            &integer_sequencer_node::metadata)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventin(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        integer_sequencer_node::next_listener>(
                            &integer_sequencer_node::next_listener_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventin(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        integer_sequencer_node::previous_listener>(
                            &integer_sequencer_node::previous_listener_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventin(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        integer_sequencer_node::set_fraction_listener>(
                            &integer_sequencer_node::set_fraction_listener_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<integer_sequencer_node>::exposedfield<mffloat> >(
                            &integer_sequencer_node::key_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<integer_sequencer_node>::exposedfield<mffloat> >(
                            &integer_sequencer_node::key_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<integer_sequencer_node>::exposedfield<mffloat> >(
                            &integer_sequencer_node::key_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<integer_sequencer_node>::exposedfield<mfint32> >(
                            &integer_sequencer_node::key_value_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<integer_sequencer_node>::exposedfield<mfint32> >(
                            &integer_sequencer_node::key_value_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<integer_sequencer_node>::exposedfield<mfint32> >(
                            &integer_sequencer_node::key_value_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventout(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        integer_sequencer_node::sfint32_emitter>(
                            &integer_sequencer_node::value_changed_emitter_)));
            } else {
                throw unsupported_interface(*interface);
            }
        }
        return type;
    }


    /**
     * @brief @c node_metatype identifier.
     */
    const char * const integer_trigger_metatype::id =
        "urn:X-openvrml:node:IntegerTrigger";

    /**
     * @brief Construct.
     *
     * @param browser the @c browser associated with this
     *                @c integer_trigger_metatype.
     */
    integer_trigger_metatype::integer_trigger_metatype(openvrml::browser & browser):
        node_metatype(integer_trigger_metatype::id, browser)
    {}

    /**
     * @brief Destroy.
     */
    integer_trigger_metatype::~integer_trigger_metatype() OPENVRML_NOTHROW
    {}

    /**
     * @brief Create a node_type.
     *
     * @param id            the name for the new node_type.
     * @param interfaces    the interfaces for the new node_type.
     *
     * @return a node_type_ptr to a node_type capable of creating IntegerTrigger nodes.
     *
     * @exception unsupported_interface if @p interfaces includes an interface
     *                                  not supported by integer_trigger_metatype.
     * @exception std::bad_alloc        if memory allocation fails.
     */
    const boost::shared_ptr<openvrml::node_type>
    integer_trigger_metatype::
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
                           field_value::sfbool_id,
                           "set_boolean"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mfint32_id,
                           "integerKey"),
            node_interface(node_interface::eventout_id,
                           field_value::sfint32_id,
                           "triggerValue")
        };
        typedef node_type_impl<integer_trigger_node> node_type_t;

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
                        abstract_node<integer_trigger_node>::exposedfield<sfnode> >(
                            &integer_trigger_node::metadata)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<integer_trigger_node>::exposedfield<sfnode> >(
                            &integer_trigger_node::metadata)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<integer_trigger_node>::exposedfield<sfnode> >(
                            &integer_trigger_node::metadata)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventin(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        integer_trigger_node::set_boolean_listener>(
                            &integer_trigger_node::set_boolean_listener_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<integer_trigger_node>::exposedfield<mfint32> >(
                            &integer_trigger_node::integer_key_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<integer_trigger_node>::exposedfield<mfint32> >(
                            &integer_trigger_node::integer_key_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<integer_trigger_node>::exposedfield<mfint32> >(
                            &integer_trigger_node::integer_key_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventout(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        integer_trigger_node::sfint32_emitter>(
                            &integer_trigger_node::trigger_value_emitter_)));
            } else {
                throw unsupported_interface(*interface);
            }
        }
        return type;
    }

    /**
     * @brief @c node_metatype identifier.
     */
    const char * const time_trigger_metatype::id =
        "urn:X-openvrml:node:TimeTrigger";

    /**
     * @brief Construct.
     *
     * @param browser the @c browser associated with this
     *                @c time_trigger_metatype.
     */
    time_trigger_metatype::time_trigger_metatype(openvrml::browser & browser):
        node_metatype(time_trigger_metatype::id, browser)
    {}

    /**
     * @brief Destroy.
     */
    time_trigger_metatype::~time_trigger_metatype() OPENVRML_NOTHROW
    {}

    /**
     * @brief Create a node_type.
     *
     * @param id            the name for the new node_type.
     * @param interfaces    the interfaces for the new node_type.
     *
     * @return a node_type_ptr to a node_type capable of creating TimeTrigger nodes.
     *
     * @exception unsupported_interface if @p interfaces includes an interface
     *                                  not supported by time_trigger_metatype.
     * @exception std::bad_alloc        if memory allocation fails.
     */
    const boost::shared_ptr<openvrml::node_type>
    time_trigger_metatype::
    do_create_type(const std::string & id,
                   const node_interface_set & interfaces) const
        OPENVRML_THROW2(unsupported_interface, std::bad_alloc)
    {
        typedef boost::array<node_interface, 3> supported_interfaces_t;
        static const supported_interfaces_t supported_interfaces = {
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "metadata"),
            node_interface(node_interface::eventin_id,
                           field_value::sfbool_id,
                           "set_boolean"),
            node_interface(node_interface::eventout_id,
                           field_value::sftime_id,
                           "triggerTime")
        };
        typedef node_type_impl<time_trigger_node> node_type_t;

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
                        abstract_node<time_trigger_node>::exposedfield<sfnode> >(
                            &time_trigger_node::metadata)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<time_trigger_node>::exposedfield<sfnode> >(
                            &time_trigger_node::metadata)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<time_trigger_node>::exposedfield<sfnode> >(
                            &time_trigger_node::metadata)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventin(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        time_trigger_node::set_boolean_listener>(
                            &time_trigger_node::set_boolean_listener_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventout(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        time_trigger_node::sftime_emitter>(
                            &time_trigger_node::trigger_time_emitter_)));
            } else {
                throw unsupported_interface(*interface);
            }
        }
        return type;
    }


    /**
     * @class boolean_filter_node
     *
     * @brief Represents BooleanFilter node instances.
     */

    /**
     * @var boolean_filter_node::boolean_filter_metatype
     *
     * @brief Class object for BooleanFilter nodes.
     */

    /**
     * @var boolean_filter_node::set_boolean_
     *
     * @brief set_boolean eventIn
     */

    /**
     * @var boolean_filter_node::input_false_
     *
     * @brief input_false eventOut
     */

    /**
     * @var boolean_filter_node::input_negate_
     *
     * @brief input_negate eventOut
     */

    /**
     * @var boolean_filter_node::input_true_
     *
     * @brief input_true eventOut
     */

    boolean_filter_node::set_boolean_listener::
    set_boolean_listener(self_t & node):
        node_event_listener(node),
        event_listener_base<self_t>(node),
        sfbool_listener(node)
    {}

    boolean_filter_node::set_boolean_listener::
    ~set_boolean_listener() OPENVRML_NOTHROW
    {}

    void boolean_filter_node::set_boolean_listener::
    do_process_event(const sfbool & /* fraction */, const double /* timestamp */)
        OPENVRML_THROW1(std::bad_alloc)
    {
        //TODO: add logic here
    }


    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope the scope to which the node belongs.
     */
    boolean_filter_node::
    boolean_filter_node(const node_type & type,
                        const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        abstract_node<self_t>(type, scope),
        child_node(type, scope),
        set_boolean_listener_(*this),
        input_false_emitter_(*this, this->input_false_),
        input_negate_emitter_(*this, this->input_negate_),
        input_true_emitter_(*this, this->input_true_)
    {}

    /**
     * @brief Destroy.
     */
    boolean_filter_node::~boolean_filter_node() OPENVRML_NOTHROW
    {}


    /**
     * @class boolean_sequencer_node
     *
     * @brief Represents BooleanSequencer node instances.
     */

    /**
     * @var boolean_sequencer_node::boolean_sequencer_metatype
     *
     * @brief Class object for BooleanSequencer nodes.
     */

    /**
     * @var boolean_sequencer_node::next_
     *
     * @brief next eventIn
     */

    /**
     * @var boolean_sequencer_node::previous_
     *
     * @brief previous eventIn
     */

    /**
     * @var boolean_sequencer_node::set_fraction_
     *
     * @brief set_fraction eventIn
     */

    /**
     * @var boolean_sequencer_node::key_
     *
     * @brief key exposedField
     */

    /**
     * @var boolean_sequencer_node::key_value_
     *
     * @brief key_value exposedField
     */

    /**
     * @var boolean_sequencer_node::value_changed_
     *
     * @brief value_changed eventOut
     */

    boolean_sequencer_node::next_listener::
    next_listener(self_t & node):
        node_event_listener(node),
        event_listener_base<self_t>(node),
        sfbool_listener(node)
    {}

    boolean_sequencer_node::next_listener::
    ~next_listener() OPENVRML_NOTHROW
    {}

    void boolean_sequencer_node::next_listener::
    do_process_event(const sfbool & /* fraction */, const double /* timestamp */)
        OPENVRML_THROW1(std::bad_alloc)
    {
        //TODO: add logic here
    }

    boolean_sequencer_node::previous_listener::
    previous_listener(self_t & node):
        node_event_listener(node),
        event_listener_base<self_t>(node),
        sfbool_listener(node)
    {}

    boolean_sequencer_node::previous_listener::
    ~previous_listener() OPENVRML_NOTHROW
    {}

    void boolean_sequencer_node::previous_listener::
    do_process_event(const sfbool & /* fraction */, const double /* timestamp */)
        OPENVRML_THROW1(std::bad_alloc)
    {
        //TODO: add logic here
    }

    boolean_sequencer_node::set_fraction_listener::
    set_fraction_listener(self_t & node):
        node_event_listener(node),
        event_listener_base<self_t>(node),
        sffloat_listener(node)
    {}

    boolean_sequencer_node::set_fraction_listener::
    ~set_fraction_listener() OPENVRML_NOTHROW
    {}

    void boolean_sequencer_node::set_fraction_listener::
    do_process_event(const sffloat & /* fraction */, const double /* timestamp */)
        OPENVRML_THROW1(std::bad_alloc)
    {
        //TODO: add logic here
    }


    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope the scope to which the node belongs.
     */
    boolean_sequencer_node::
    boolean_sequencer_node(const node_type & type,
                           const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        abstract_node<self_t>(type, scope),
        child_node(type, scope),
        next_listener_(*this),
        previous_listener_(*this),
        set_fraction_listener_(*this),
        key_(*this),
        key_value_(*this),
        value_changed_emitter_(*this, this->value_changed_)
    {}

    /**
     * @brief Destroy.
     */
    boolean_sequencer_node::~boolean_sequencer_node() OPENVRML_NOTHROW
    {}


    /**
     * @class boolean_toggle_node
     *
     * @brief Represents BooleanToggle node instances.
     */

    /**
     * @var boolean_toggle_node::BooleanToggle_metatype
     *
     * @brief Class object for BooleanToggle nodes.
     */

    /**
     * @var boolean_toggle_node::set_boolean_
     *
     * @brief set_boolean eventIn
     */

    /**
     * @var boolean_toggle_node::toggle_
     *
     * @brief toggle exposedField
     */

    boolean_toggle_node::set_boolean_listener::
    set_boolean_listener(self_t & node):
        node_event_listener(node),
        event_listener_base<self_t>(node),
        sfbool_listener(node)
    {}

    boolean_toggle_node::set_boolean_listener::
    ~set_boolean_listener() OPENVRML_NOTHROW
    {}

    void boolean_toggle_node::set_boolean_listener::
    do_process_event(const sfbool & /* fraction */, const double /* timestamp */)
        OPENVRML_THROW1(std::bad_alloc)
    {
        //TODO: add logic here
    }


    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope     the scope to which the node belongs.
     */
    boolean_toggle_node::
    boolean_toggle_node(const node_type & type,
                        const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        abstract_node<self_t>(type, scope),
        child_node(type, scope),
        set_boolean_listener_(*this),
        toggle_(*this)
    {}

    /**
     * @brief Destroy.
     */
    boolean_toggle_node::~boolean_toggle_node() OPENVRML_NOTHROW
    {}


    /**
     * @class boolean_trigger_node
     *
     * @brief Represents BooleanTrigger node instances.
     */

    /**
     * @var boolean_trigger_node::boolean_trigger_metatype
     *
     * @brief Class object for BooleanTrigger nodes.
     */

    /**
     * @var boolean_trigger_node::set_trigger_time_
     *
     * @brief set_trigger_time eventIn
     */

    /**
     * @var boolean_trigger_node::trigger_true_
     *
     * @brief trigger_true eventOut
     */

    boolean_trigger_node::set_trigger_time_listener::
    set_trigger_time_listener(self_t & node):
        node_event_listener(node),
        event_listener_base<self_t>(node),
        sftime_listener(node)
    {}

    boolean_trigger_node::set_trigger_time_listener::
    ~set_trigger_time_listener() OPENVRML_NOTHROW
    {}

    void boolean_trigger_node::set_trigger_time_listener::
    do_process_event(const sftime & /* fraction */, const double /* timestamp */)
        OPENVRML_THROW1(std::bad_alloc)
    {
        //TODO: add logic here
    }


    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope the scope to which the node belongs.
     */
    boolean_trigger_node::
    boolean_trigger_node(const node_type & type,
                         const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        abstract_node<self_t>(type, scope),
        child_node(type, scope),
        set_trigger_time_listener_(*this),
        trigger_true_emitter_(*this, this->trigger_true_)
    {}

    /**
     * @brief Destroy.
     */
    boolean_trigger_node::~boolean_trigger_node() OPENVRML_NOTHROW
    {}


    /**
     * @class integer_sequencer_node
     *
     * @brief Represents IntegerSequencer node instances.
     */

    /**
     * @var integer_sequencer_node::integer_sequencer_metatype
     *
     * @brief Class object for IntegerSequencer nodes.
     */

    /**
     * @var integer_sequencer_node::next_
     *
     * @brief next eventIn
     */

    /**
     * @var integer_sequencer_node::previous_
     *
     * @brief previous eventIn
     */

    /**
     * @var integer_sequencer_node::set_fraction_
     *
     * @brief set_fraction eventIn
     */

    /**
     * @var integer_sequencer_node::key_
     *
     * @brief key exposedField
     */

    /**
     * @var integer_sequencer_node::key_value_
     *
     * @brief key_value exposedField
     */

    /**
     * @var integer_sequencer_node::value_changed_
     *
     * @brief value_changed eventOut
     */

    integer_sequencer_node::next_listener::
    next_listener(self_t & node):
        node_event_listener(node),
        event_listener_base<self_t>(node),
        sfbool_listener(node)
    {}

    integer_sequencer_node::next_listener::
    ~next_listener() OPENVRML_NOTHROW
    {}

    void integer_sequencer_node::next_listener::
    do_process_event(const sfbool & /* fraction */, const double /* timestamp */)
        OPENVRML_THROW1(std::bad_alloc)
    {
        //TODO: add logic here
    }

    integer_sequencer_node::previous_listener::
    previous_listener(self_t & node):
        node_event_listener(node),
        event_listener_base<self_t>(node),
        sfbool_listener(node)
    {}

    integer_sequencer_node::previous_listener::
    ~previous_listener() OPENVRML_NOTHROW
    {}

    void integer_sequencer_node::previous_listener::
    do_process_event(const sfbool & /* fraction */, const double /* timestamp */)
        OPENVRML_THROW1(std::bad_alloc)
    {
        //TODO: add logic here
    }

    integer_sequencer_node::set_fraction_listener::
    set_fraction_listener(self_t & node):
        node_event_listener(node),
        event_listener_base<self_t>(node),
        sffloat_listener(node)
    {}

    integer_sequencer_node::set_fraction_listener::
    ~set_fraction_listener() OPENVRML_NOTHROW
    {}

    void integer_sequencer_node::set_fraction_listener::
    do_process_event(const sffloat & /* fraction */, const double /* timestamp */)
        OPENVRML_THROW1(std::bad_alloc)
    {
        //TODO: add logic here
    }


    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope the scope to which the node belongs.
     */
    integer_sequencer_node::
    integer_sequencer_node(const node_type & type,
                           const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        abstract_node<self_t>(type, scope),
        child_node(type, scope),
        next_listener_(*this),
        previous_listener_(*this),
        set_fraction_listener_(*this),
        key_(*this),
        key_value_(*this),
        value_changed_emitter_(*this, this->value_changed_)
    {}

    /**
     * @brief Destroy.
     */
    integer_sequencer_node::~integer_sequencer_node() OPENVRML_NOTHROW
    {}


    /**
     * @class integer_trigger_node
     *
     * @brief Represents IntegerTrigger node instances.
     */

    /**
     * @var integer_trigger_node::integer_trigger_metatype
     *
     * @brief Class object for IntegerTrigger nodes.
     */

    /**
     * @var integer_trigger_node::set_boolean_
     *
     * @brief set_boolean eventIn
     */

    /**
     * @var integer_trigger_node::integer_key_
     *
     * @brief integer_key exposedField
     */

    /**
     * @var integer_trigger_node::trigger_value_
     *
     * @brief trigger_value eventOut
     */

    integer_trigger_node::set_boolean_listener::
    set_boolean_listener(self_t & node):
        node_event_listener(node),
        event_listener_base<self_t>(node),
        sfbool_listener(node)
    {}

    integer_trigger_node::set_boolean_listener::
    ~set_boolean_listener() OPENVRML_NOTHROW
    {}

    void integer_trigger_node::set_boolean_listener::
    do_process_event(const sfbool & /* fraction */, const double /* timestamp */)
        OPENVRML_THROW1(std::bad_alloc)
    {
        //TODO: add logic here
    }


    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope     the scope to which the node belongs.
     */
    integer_trigger_node::
    integer_trigger_node(const node_type & type,
                         const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        abstract_node<self_t>(type, scope),
        child_node(type, scope),
        set_boolean_listener_(*this),
        integer_key_(*this),
        trigger_value_emitter_(*this, this->trigger_value_)
    {}

    /**
     * @brief Destroy.
     */
    integer_trigger_node::~integer_trigger_node() OPENVRML_NOTHROW
    {}


    /**
     * @class time_trigger_node
     *
     * @brief Represents TimeTrigger node instances.
     */

    /**
     * @var time_trigger_node::time_trigger_metatype
     *
     * @brief Class object for TimeTrigger nodes.
     */

    /**
     * @var time_trigger_node::set_boolean_
     *
     * @brief set_boolean eventIn
     */

    /**
     * @var time_trigger_node::trigger_time_
     *
     * @brief trigger_time eventOut
     */

    time_trigger_node::set_boolean_listener::
    set_boolean_listener(self_t & node):
        node_event_listener(node),
        event_listener_base<self_t>(node),
        sfbool_listener(node)
    {}

    time_trigger_node::set_boolean_listener::
    ~set_boolean_listener() OPENVRML_NOTHROW
    {}

    void time_trigger_node::set_boolean_listener::
    do_process_event(const sfbool & /* fraction */, const double /* timestamp */)
        OPENVRML_THROW1(std::bad_alloc)
    {
        //TODO: add logic here
    }


    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope     the scope to which the node belongs.
     */
    time_trigger_node::
    time_trigger_node(const node_type & type,
                      const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        abstract_node<self_t>(type, scope),
        child_node(type, scope),
        set_boolean_listener_(*this),
        trigger_time_emitter_(*this, this->trigger_time_)
    {}

    /**
     * @brief Destroy.
     */
    time_trigger_node::~time_trigger_node() OPENVRML_NOTHROW
    {}
}
