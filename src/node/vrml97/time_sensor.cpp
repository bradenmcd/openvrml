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

# include "time_sensor.h"
# include <openvrml/local/float.h>
# include <private.h>
# include <openvrml/browser.h>
# include <openvrml/node_impl_util.h>
# include <openvrml/scene.h>
# include <boost/array.hpp>

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

namespace {

    class OPENVRML_LOCAL time_sensor_node :
        public openvrml::node_impl_util::abstract_node<time_sensor_node>,
        public openvrml::time_dependent_node,
        public openvrml::child_node {

        friend class openvrml_node_vrml97::time_sensor_metatype;

        class set_cycle_interval_listener :
            public openvrml::node_impl_util::event_listener_base<self_t>,
            public sftime_listener {
        public:
            explicit set_cycle_interval_listener(time_sensor_node & node);
            virtual ~set_cycle_interval_listener() OPENVRML_NOTHROW;

        private:
            virtual
            void do_process_event(const openvrml::sftime & cycle_interval,
                                  double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        class enabled_exposedfield : public exposedfield<openvrml::sfbool> {
        public:
            explicit enabled_exposedfield(time_sensor_node & node);
            enabled_exposedfield(const enabled_exposedfield & obj)
                OPENVRML_NOTHROW;
            virtual ~enabled_exposedfield() OPENVRML_NOTHROW;

        private:
            virtual std::auto_ptr<openvrml::field_value> do_clone() const
                OPENVRML_THROW1(std::bad_alloc);
            virtual void event_side_effect(const openvrml::sfbool & enabled,
                                           double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        class set_start_time_listener :
            public openvrml::node_impl_util::event_listener_base<self_t>,
            public sftime_listener {
        public:
            explicit set_start_time_listener(time_sensor_node & node);
            virtual ~set_start_time_listener() OPENVRML_NOTHROW;

        private:
            virtual void do_process_event(const openvrml::sftime & start_time,
                                          double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        set_cycle_interval_listener set_cycle_interval_listener_;
        openvrml::sftime cycle_interval_;
        sftime_emitter cycle_interval_changed_emitter_;
        enabled_exposedfield enabled_;
        exposedfield<openvrml::sfbool> loop_;
        set_start_time_listener set_start_time_listener_;
        openvrml::sftime start_time_;
        sftime_emitter start_time_changed_emitter_;
        exposedfield<openvrml::sftime> stop_time_;
        exposedfield<openvrml::sftime> pause_time_;
        exposedfield<openvrml::sftime> resume_time_;
        openvrml::sftime cycle_time_;
        sftime_emitter cycle_time_emitter_;
        openvrml::sffloat fraction_changed_;
        sffloat_emitter fraction_changed_emitter_;
        openvrml::sfbool is_active_;
        sfbool_emitter is_active_emitter_;
        openvrml::sftime time_;
        sftime_emitter time_emitter_;
        openvrml::sftime elapsed_time_;
        sftime_emitter elapsed_time_emitter_;
        openvrml::sfbool is_paused_;
        sfbool_emitter is_paused_emitter_;

        double lastTime;

    public:
        time_sensor_node(const openvrml::node_type & type,
                         const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~time_sensor_node() OPENVRML_NOTHROW;

    private:
        virtual void do_initialize(double timestamp)
            OPENVRML_THROW1(std::bad_alloc);
        virtual void do_shutdown(double timestamp) OPENVRML_NOTHROW;
        virtual void do_update(double time);
    };

    /**
     * @class time_sensor_node
     *
     * @brief TimeSensor node instances.
     */

    /**
     * @var class time_sensor_node::time_sensor_metatype
     *
     * @brief Class object for TimeSensor nodes.
     */

    /**
     * @internal
     *
     * @class time_sensor_node::set_cycle_interval_listener
     *
     * @brief set_cycleInterval eventIn handler.
     */

    /**
     * @brief Construct.
     *
     * @param node  time_sensor_node.
     */
    time_sensor_node::set_cycle_interval_listener::
    set_cycle_interval_listener(time_sensor_node & node):
        openvrml::node_event_listener(node),
        openvrml::node_impl_util::event_listener_base<time_sensor_node>(node),
        sftime_listener(node)
    {}

    /**
     * @brief Destroy.
     */
    time_sensor_node::set_cycle_interval_listener::
    ~set_cycle_interval_listener()
        OPENVRML_NOTHROW
    {}

    /**
     * @brief Process event.
     *
     * @param cycle_interval    cycleInterval.
     * @param timestamp         the current time.
     *
     * @exception std::bad_alloc    if memory allocation fails.
     */
    void
    time_sensor_node::set_cycle_interval_listener::
    do_process_event(const openvrml::sftime & cycle_interval, const double timestamp)
        OPENVRML_THROW1(std::bad_alloc)
    {
        try {
            time_sensor_node & node =
                dynamic_cast<time_sensor_node &>(this->node());

            if (!node.is_active_.value()) {
                node.cycle_interval_ = cycle_interval;
                node.lastTime = timestamp;
                node::emit_event(node.cycle_interval_changed_emitter_,
                                 timestamp);
            }
        } catch (std::bad_cast & ex) {
            OPENVRML_PRINT_EXCEPTION_(ex);
        }
    }

   /**
    * @internal
    *
    * @class time_sensor_node::enabled_exposedfield
    *
    * @brief enabled exposedField implementation.
    */

    /**
     * @brief Construct.
     *
     * @param node  time_sensor_node.
     */
    time_sensor_node::enabled_exposedfield::
    enabled_exposedfield(time_sensor_node & node):
        openvrml::node_event_listener(node),
        openvrml::event_emitter(static_cast<const openvrml::field_value &>(*this)),
        sfbool_listener(node),
        exposedfield<openvrml::sfbool>(node, true)
    {}

    /**
     * @brief Construct a copy.
     *
     * @param obj   instance to copy.
     */
    time_sensor_node::enabled_exposedfield::
    enabled_exposedfield(const enabled_exposedfield & obj) OPENVRML_NOTHROW:
        openvrml::event_listener(),
        openvrml::node_event_listener(obj.openvrml::node_event_listener::node()),
        openvrml::event_emitter(static_cast<const openvrml::field_value &>(*this)),
        sfbool_listener(obj.openvrml::node_event_listener::node()),
        exposedfield<openvrml::sfbool>(obj)
    {}

    /**
     * @brief Destroy.
     */
    time_sensor_node::enabled_exposedfield::
    ~enabled_exposedfield()
        OPENVRML_NOTHROW
    {}

    /**
     * @brief Polymorphically construct a copy.
     *
     * @return a copy of the instance.
     *
     * @exception std::bad_alloc    if memory allocation fails.
     */
    std::auto_ptr<openvrml::field_value>
    time_sensor_node::enabled_exposedfield::do_clone() const
        OPENVRML_THROW1(std::bad_alloc)
    {
        return std::auto_ptr<openvrml::field_value>(
            new enabled_exposedfield(*this));
    }

    /**
     * @brief Process event.
     *
     * @param enabled   enabled state.
     * @param timestamp the current time.
     *
     * @exception std::bad_alloc    if memory allocation fails.
     */
    void
    time_sensor_node::enabled_exposedfield::
    event_side_effect(const openvrml::sfbool & enabled, const double timestamp)
        OPENVRML_THROW1(std::bad_alloc)
    {
        try {
            time_sensor_node & node =
                dynamic_cast<time_sensor_node &>(this->node_event_listener::node());

            if (enabled.value() != node.is_active_.value()) {
                if (node.is_active_.value()) {
                    using openvrml::local::fequal;

                    //
                    // Was active; shutdown.
                    //
                    double cycleInt = node.cycle_interval_.value();
                    double f = (cycleInt > 0.0)
                        ? fmod(node.time_.value() - node.start_time_.value(),
                               cycleInt)
                        : 0.0;

                    // Fraction of cycle message
                    node.fraction_changed_.value(float(fequal(f, 0.0)
                                                       ? 1.0
                                                       : (f / cycleInt)));
                } else {
                    //
                    // Was inactive; startup.
                    //
                    node.cycle_time_.value(timestamp);
                    node::emit_event(node.cycle_time_emitter_, timestamp);

                    // Fraction of cycle message
                    node.fraction_changed_.value(0.0);
                }
                node.time_.value(timestamp);
                node::emit_event(node.time_emitter_, timestamp);
                node::emit_event(node.fraction_changed_emitter_, timestamp);
                node.is_active_ = enabled;
                node::emit_event(node.is_active_emitter_, timestamp);
            }
        } catch (std::bad_cast & ex) {
            OPENVRML_PRINT_EXCEPTION_(ex);
        }
    }

    /**
     * @internal
     *
     * @class time_sensor_node::set_start_time_listener
     *
     * @brief set_startTime eventIn handler.
     */

    /**
     * @brief Construct.
     *
     * @param node  time_sensor_node.
     */
    time_sensor_node::set_start_time_listener::
    set_start_time_listener(time_sensor_node & node):
        openvrml::node_event_listener(node),
        openvrml::node_impl_util::event_listener_base<time_sensor_node>(node),
        sftime_listener(node)
    {}

    /**
     * @brief Destroy.
     */
    time_sensor_node::set_start_time_listener::
    ~set_start_time_listener()
        OPENVRML_NOTHROW
    {}

    /**
     * @brief Process event.
     *
     * @param start_time    startTime.
     * @param timestamp     the current time.
     *
     * @exception std::bad_alloc    if memory allocation fails.
     */
    void
    time_sensor_node::set_start_time_listener::
    do_process_event(const openvrml::sftime & start_time, const double timestamp)
        OPENVRML_THROW1(std::bad_alloc)
    {
        try {
            time_sensor_node & node =
                dynamic_cast<time_sensor_node &>(this->node());

            if (!node.is_active_.value()) {
                node.start_time_ = start_time;
                node.lastTime = timestamp;
                node::emit_event(node.start_time_changed_emitter_, timestamp);
            }
        } catch (std::bad_cast & ex) {
            OPENVRML_PRINT_EXCEPTION_(ex);
        }
    }

    /**
     * @var time_sensor_node::set_cycle_interval_listener time_sensor_node::set_cycle_interval_listener_
     *
     * @brief set_cycleInterval eventIn handler.
     */

    /**
     * @var openvrml::sftime time_sensor_node::cycle_interval_
     *
     * @brief cycleInterval exposedField value.
     */

    /**
     * @var openvrml::sftime_emitter time_sensor_node::cycle_interval_changed_emitter_
     *
     * @brief cycleInterval_changed event emitter.
     */

    /**
     * @var time_sensor_node::enabled_exposedfield time_sensor_node::enabled_
     *
     * @brief enabled exposedField.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<time_sensor_node>::exposedfield<openvrml::sfbool> time_sensor_node::loop_
     *
     * @brief loop exposedField.
     */

    /**
     * @var time_sensor_node::set_start_time_listener time_sensor_node::set_start_time_listener_
     *
     * @brief set_startTime event handler.
     */

    /**
     * @var openvrml::sftime time_sensor_node::start_time_
     *
     * @brief startTime exposedField value.
     */

    /**
     * @var openvrml::sftime_emitter time_sensor_node::start_time_changed_emitter_
     *
     * @brief startTime_changed event emitter.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<time_sensor_node>::exposedfield<openvrml::sftime> time_sensor_node::stop_time_
     *
     * @brief stopTime exposedField.
     */

    /**
     * @var openvrml::sftime time_sensor_node::cycle_time_
     *
     * @brief cycleTime eventOut value.
     */

    /**
     * @var openvrml::sftime_emitter time_sensor_node::cycle_time_emitter_
     *
     * @brief cycleTime eventOut emitter.
     */

    /**
     * @var openvrml::sffloat time_sensor_node::fraction_changed_
     *
     * @brief fraction_changed eventOut value.
     */

    /**
     * @var openvrml::sffloat_emitter time_sensor_node::fraction_changed_emitter_
     *
     * @brief fraction_changed eventOut emitter.
     */

    /**
     * @var openvrml::sfbool time_sensor_node::is_active_
     *
     * @brief isActive eventOut value.
     */

    /**
     * @var openvrml::sfbool_emitter time_sensor_node::is_active_emitter_
     *
     * @brief isActive eventOut emitter.
     */

    /**
     * @var openvrml::sftime time_sensor_node::time_
     *
     * @brief time eventOut value.
     */

    /**
     * @var openvrml::sftime_emitter time_sensor_node::time_emitter_
     *
     * @brief time eventOut emitter.
     */

    /**
     * @var double time_sensor_node::lastTime
     *
     * @brief The timestamp previously received.
     */

    /**
     * @brief Construct.
     *
     * @param type      the @c node_type associated with the instance.
     * @param scope     the @c scope that the new node will belong to.
     */
    time_sensor_node::
    time_sensor_node(const openvrml::node_type & type,
                     const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        openvrml::node_impl_util::abstract_node<time_sensor_node>(type, scope),
        time_dependent_node(type, scope),
        child_node(type, scope),
        set_cycle_interval_listener_(*this),
        cycle_interval_(1.0),
        cycle_interval_changed_emitter_(*this, this->cycle_interval_),
        enabled_(*this),
        loop_(*this, false),
        set_start_time_listener_(*this),
        start_time_(0.0),
        start_time_changed_emitter_(*this, this->start_time_),
        stop_time_(*this, 0.0),
        pause_time_(*this),
        resume_time_(*this),
        cycle_time_emitter_(*this, this->cycle_time_),
        fraction_changed_emitter_(*this, this->fraction_changed_),
        is_active_(false),
        is_active_emitter_(*this, this->is_active_),
        time_emitter_(*this, this->time_),
        elapsed_time_emitter_(*this, this->elapsed_time_),
        is_paused_emitter_(*this, this->is_paused_),
        lastTime(-1.0)
    {}

    /**
     * @brief Destroy.
     */
    time_sensor_node::~time_sensor_node() OPENVRML_NOTHROW
    {}

    /**
     * Generate timer events. If necessary, events prior to the timestamp
     * (inTime) are generated to respect stopTimes and cycleIntervals. The
     * timestamp should never be increased. This assumes the event loop
     * delivers pending events in order (ascending time stamps). Should inTime
     * be modified?  Should ensure continuous events are delivered before
     * discrete ones (such as cycleTime, isActive).
     */
    void
    time_sensor_node::do_update(const double currentTime)
    {
        openvrml::sftime timeNow(currentTime);

        if (this->enabled_.sfbool::value()) {
            if (this->lastTime > timeNow.value()) {
                this->lastTime = timeNow.value();
            }

            // Become active at startTime if either the valid stopTime hasn't
            // passed or we are looping.
            if (!this->is_active_.value()
                && this->start_time_.value() <= timeNow.value()
                && this->start_time_.value() >= this->lastTime
                && ((this->stop_time_.sftime::value() < this->start_time_.value()
                     || this->stop_time_.sftime::value() > timeNow.value())
                    || this->loop_.sfbool::value())) {

                // Start at first tick >= startTime
                this->is_active_.value(true);
                node::emit_event(this->is_active_emitter_, timeNow.value());
                this->time_.value(timeNow.value());
                node::emit_event(this->time_emitter_, timeNow.value());
                this->fraction_changed_.value(0.0);
                node::emit_event(this->fraction_changed_emitter_,
                                 timeNow.value());
                this->cycle_time_.value(timeNow.value());
                node::emit_event(this->cycle_time_emitter_, timeNow.value());
            }

            // Running (active and enabled)
            else if (this->is_active_.value()) {
                using openvrml::local::fequal;
                using openvrml::local::fless_equal;

                double f, cycleInt = this->cycle_interval_.value();
                bool deactivate = false;

                // Are we done? Choose min of stopTime or start + single cycle.
                if ((this->stop_time_.sftime::value() > this->start_time_.value()
                     && fless_equal(this->stop_time_.sftime::value(),
                                    timeNow.value()))
                    || (!this->loop_.sfbool::value()
                        && fless_equal(this->start_time_.value() + cycleInt,
                                       timeNow.value()))) {
                    this->is_active_.value(false);

                    // Must respect stopTime/cycleInterval exactly
                    if (this->start_time_.value() + cycleInt
                        < this->stop_time_.sftime::value()) {
                        timeNow = openvrml::sftime(this->start_time_.value()
                                                   + cycleInt);
                    } else {
                        timeNow = this->stop_time_;
                    }

                    deactivate = true;
                }

                f = (cycleInt > 0.0 && timeNow.value() > this->start_time_.value())
                    ? fmod(timeNow.value() - this->start_time_.value(),
                           cycleInt)
                    : 0.0;

                // Fraction of cycle message
                this->fraction_changed_.value(fequal(f, 0.0)
                                              ? 1.0f
                                              : float(f / cycleInt));
                node::emit_event(this->fraction_changed_emitter_,
                                 timeNow.value());

                // Current time message
                this->time_.value(timeNow.value());
                node::emit_event(this->time_emitter_, timeNow.value());

                // End of cycle message (this may miss cycles...)
                if (fequal(this->fraction_changed_.value(), 1.0f)) {
                    this->cycle_time_.value(timeNow.value());
                    node::emit_event(this->cycle_time_emitter_,
                                     timeNow.value());
                }

                if (deactivate) {
                    node::emit_event(this->is_active_emitter_,
                                     timeNow.value());
                }
            }

            // Tell the scene this node needs quick updates while it is active.
            // Should check whether time, fraction_changed eventOuts are
            // being used, and set delta to cycleTime if not...
            if (this->is_active_.value()) {
                this->type().metatype().browser().delta(0.0);
            }
            this->lastTime = currentTime;
        }
    }

    /**
     * @brief Initialize.
     *
     * @param timestamp the current time.
     *
     * @exception std::bad_alloc    if memory allocation fails.
     */
    void
    time_sensor_node::do_initialize(double)
        OPENVRML_THROW1(std::bad_alloc)
    {
        assert(this->scene());
        this->scene()->browser().add_time_dependent(*this);
    }

    /**
     * @brief Shut down.
     *
     * @param timestamp the current time.
     */
    void
    time_sensor_node::do_shutdown(double)
        OPENVRML_NOTHROW
    {
        assert(this->scene());
        this->scene()->browser().remove_time_dependent(*this);
    }
}


/**
 * @brief @c node_metatype identifier.
 */
const char * const openvrml_node_vrml97::time_sensor_metatype::id =
    "urn:X-openvrml:node:TimeSensor";

/**
 * @brief Construct.
 *
 * @param browser   the @c browser associated with this @c node_metatype.
 */
openvrml_node_vrml97::time_sensor_metatype::
time_sensor_metatype(openvrml::browser & browser):
    node_metatype(time_sensor_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_vrml97::time_sensor_metatype::~time_sensor_metatype()
    OPENVRML_NOTHROW
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a boost::shared_ptr<node_type> to a node_type capable of
 *         creating TimeSensor nodes.
 *
 * @exception openvrml::unsupported_interface if @p interfaces includes an interface
 *                                  not supported by time_sensor_metatype.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml_node_vrml97::time_sensor_metatype::
do_create_type(const std::string & id,
               const openvrml::node_interface_set & interfaces) const
    OPENVRML_THROW2(openvrml::unsupported_interface, std::bad_alloc)
{
    using namespace openvrml;
    using namespace openvrml::node_impl_util;

    typedef boost::array<node_interface, 14> supported_interfaces_t;
    static const supported_interfaces_t supported_interfaces = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sftime_id,
                       "cycleInterval"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfbool_id,
                       "enabled"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfbool_id,
                       "loop"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sftime_id,
                       "startTime"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sftime_id,
                       "stopTime"),
        node_interface(node_interface::eventout_id,
                       field_value::sftime_id,
                       "cycleTime"),
        node_interface(node_interface::eventout_id,
                       field_value::sffloat_id,
                       "fraction_changed"),
        node_interface(node_interface::eventout_id,
                       field_value::sfbool_id,
                       "isActive"),
        node_interface(node_interface::eventout_id,
                       field_value::sftime_id,
                       "time"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sftime_id,
                       "pauseTime"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sftime_id,
                       "resumeTime"),
        node_interface(node_interface::eventout_id,
                       field_value::sftime_id,
                       "elapsedTime"),
        node_interface(node_interface::eventout_id,
                       field_value::sfbool_id,
                       "isPaused")
    };

    typedef node_impl_util::node_type_impl<time_sensor_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & timeSensorNodeType = static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface_(interfaces.begin());
         interface_ != interfaces.end();
         ++interface_) {
        supported_interfaces_t::const_iterator supported_interface =
            supported_interfaces.begin() - 1;
        if (*interface_ == *++supported_interface) {
            timeSensorNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &time_sensor_node::set_cycle_interval_listener_,
                &time_sensor_node::cycle_interval_,
                &time_sensor_node::cycle_interval_changed_emitter_);
        } else if (*interface_ == *++supported_interface) {
            timeSensorNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &time_sensor_node::enabled_);
        } else if (*interface_ == *++supported_interface) {
            timeSensorNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &time_sensor_node::loop_);
        } else if (*interface_ == *++supported_interface) {
            timeSensorNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &time_sensor_node::set_start_time_listener_,
                &time_sensor_node::start_time_,
                &time_sensor_node::start_time_changed_emitter_);
        } else if (*interface_ == *++supported_interface) {
            timeSensorNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &time_sensor_node::stop_time_);
        } else if (*interface_ == *++supported_interface) {
            timeSensorNodeType.add_eventout(
                supported_interface->field_type,
                supported_interface->id,
                &time_sensor_node::cycle_time_emitter_);
        } else if (*interface_ == *++supported_interface) {
            timeSensorNodeType.add_eventout(
                supported_interface->field_type,
                supported_interface->id,
                &time_sensor_node::fraction_changed_emitter_);
        } else if (*interface_ == *++supported_interface) {
            timeSensorNodeType.add_eventout(
                supported_interface->field_type,
                supported_interface->id,
                &time_sensor_node::is_active_emitter_);
        } else if (*interface_ == *++supported_interface) {
            timeSensorNodeType.add_eventout(
                supported_interface->field_type,
                supported_interface->id,
                &time_sensor_node::time_emitter_);
        } else if (*interface_ == *++supported_interface) {
            timeSensorNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &time_sensor_node::metadata);
        } else if (*interface_ == *++supported_interface) {
            timeSensorNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &time_sensor_node::pause_time_);
        } else if (*interface_ == *++supported_interface) {
            timeSensorNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &time_sensor_node::resume_time_);
        } else if (*interface_ == *++supported_interface) {
            timeSensorNodeType.add_eventout(
                supported_interface->field_type,
                supported_interface->id,
                &time_sensor_node::elapsed_time_emitter_);
        } else if (*interface_ == *++supported_interface) {
            timeSensorNodeType.add_eventout(
                supported_interface->field_type,
                supported_interface->id,
                &time_sensor_node::is_paused_emitter_);
        } else {
            throw unsupported_interface(*interface_);
        }
    }
    return type;
}
