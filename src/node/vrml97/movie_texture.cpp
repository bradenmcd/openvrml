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

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

# include <boost/array.hpp>
# include <openvrml/browser.h>
# include <private.h>
# include "movie_texture.h"
# include "abstract_texture.h"

namespace {

    class OPENVRML_LOCAL movie_texture_node :
        public openvrml_node_vrml97::abstract_texture_node<movie_texture_node>,
        public openvrml::time_dependent_node {

        friend class openvrml_node_vrml97::movie_texture_metatype;

        class set_speed_listener : public openvrml::node_impl_util::event_listener_base<self_t>,
                                   public sffloat_listener {
        public:
            explicit set_speed_listener(movie_texture_node & node);
            virtual ~set_speed_listener() OPENVRML_NOTHROW;

        private:
            virtual void do_process_event(const openvrml::sffloat & speed,
                                          double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        exposedfield<openvrml::sfbool> loop_;
        set_speed_listener set_speed_;
        openvrml::sffloat speed_;
        sffloat_emitter speed_changed_;
        exposedfield<openvrml::sftime> start_time_;
        exposedfield<openvrml::sftime> stop_time_;
        exposedfield<openvrml::mfstring> url_;
        exposedfield<openvrml::sftime> pause_time_;
        exposedfield<openvrml::sftime> resume_time_;
        openvrml::sftime duration_;
        sftime_emitter duration_changed_;
        openvrml::sfbool active_;
        sfbool_emitter is_active_;
        openvrml::sftime elapsed_time_;
        sftime_emitter elapsed_time_emitter_;
        openvrml::sfbool is_paused_;
        sfbool_emitter is_paused_emitter_;

        openvrml::image image_;

    public:
        movie_texture_node(const openvrml::node_type & type,
                           const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~movie_texture_node() OPENVRML_NOTHROW;

        virtual const openvrml::image & image() const OPENVRML_NOTHROW;

    private:
        virtual void do_initialize(double timestamp) OPENVRML_THROW1(std::bad_alloc);
        virtual void do_shutdown(double timestamp) OPENVRML_NOTHROW;
        virtual openvrml::viewer::texture_object_t
            do_render_texture(openvrml::viewer & v);
        virtual void do_update(double time);
    };

    /**
     * @class movie_texture_node
     *
     * @brief MovieTexture node instances.
     */

    /**
     * @var class movie_texture_node::movie_texture_metatype
     *
     * @brief Class object for MovieTexture nodes.
     */

    /**
     * @internal
     *
     * @class movie_texture_node::set_speed_listener
     *
     * @brief set_speed eventIn handler.
     *
     * Unlike most exposedFields which are implemented either with an instance
     * of the exposedfield template or a class derived from it, MovieTexture's
     * speed exposedField is implemented using the listener and emitter
     * primitives. This is because unlike most exposedFields, changes to the
     * speed field (that is, set_speed events) are ignored if the node is not
     * active.
     */

    /**
     * @brief Construct.
     *
     * @param node  movie_texture_node.
     */
    movie_texture_node::set_speed_listener::
    set_speed_listener(movie_texture_node & node):
        openvrml::node_event_listener(node),
        openvrml::node_impl_util::event_listener_base<movie_texture_node>(node),
        sffloat_listener(node)
    {}

    /**
     * @brief Destroy.
     */
    movie_texture_node::set_speed_listener::
    ~set_speed_listener() OPENVRML_NOTHROW
    {}

    /**
     * @brief Process event.
     *
     * @param speed     speed value.
     * @param timestamp the current time.
     *
     * @exception std::bad_alloc    if memory allocation fails.
     */
    void
    movie_texture_node::set_speed_listener::
    do_process_event(const openvrml::sffloat & speed, const double timestamp)
        OPENVRML_THROW1(std::bad_alloc)
    {
        try {
            movie_texture_node & node =
                dynamic_cast<movie_texture_node &>(this->node());

            //
            // set_speed is ignored if the MovieTexture is active.
            //
            if (!node.active_.value()) {
                node.speed_ = speed;
                node.node::modified(true);
                node::emit_event(node.speed_changed_, timestamp);
            }
        } catch (std::bad_alloc & ex) {
            OPENVRML_PRINT_EXCEPTION_(ex);
        }
    }

    /**
     * @var openvrml::node_impl_util::abstract_node<movie_texture_node>::exposedfield<openvrml::sfbool> movie_texture_node::loop_
     *
     * @brief loop exposedField.
     */

    /**
     * @var movie_texture_node::set_speed_listener movie_texture_node::set_speed_
     *
     * @brief set_speed event handler for the speed exposedField.
     */

    /**
     * @var openvrml::sffloat movie_texture_node::speed_
     *
     * @brief Value for speed exposedField.
     */

    /**
     * @var openvrml::sffloat_emitter movie_texture_node::speed_changed_
     *
     * @brief speed_changed event emitter for the speed exposedField.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<movie_texture_node>::exposedfield<openvrml::sftime> movie_texture_node::start_time_
     *
     * @brief startTime exposedField.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<movie_texture_node>::exposedfield<openvrml::sftime> movie_texture_node::stop_time_
     *
     * @brief stopTime exposedField.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<movie_texture_node>::exposedfield<openvrml::mfstring> movie_texture_node::url_
     *
     * @brief url exposedField.
     */

    /**
     * @var openvrml::sftime movie_texture_node::duration_
     *
     * @brief duration value.
     */

    /**
     * @var openvrml::sftime_emitter movie_texture_node::duration_changed_
     *
     * @brief duration_changed eventOut
     */

    /**
     * @var openvrml::sfbool movie_texture_node::active_
     *
     * @brief active state.
     */

    /**
     * @var openvrml::sfbool_emitter movie_texture_node::is_active_
     *
     * @brief isActive eventOut.
     */

    /**
     * @var openvrml::image movie_texture_node::image_
     *
     * @brief Frame data.
     */

    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with the node instance.
     * @param scope the scope to which the node belongs.
     */
    movie_texture_node::
    movie_texture_node(const openvrml::node_type & type,
                       const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        openvrml_node_vrml97::abstract_texture_node<movie_texture_node>(
            type, scope),
        time_dependent_node(type, scope),
        loop_(*this, false),
        set_speed_(*this),
        speed_(1.0),
        speed_changed_(*this, this->speed_),
        start_time_(*this),
        stop_time_(*this),
        url_(*this),
        pause_time_(*this),
        resume_time_(*this),
        duration_changed_(*this, this->duration_),
        is_active_(*this, this->active_),
        elapsed_time_emitter_(*this, this->elapsed_time_),
        is_paused_emitter_(*this, this->is_paused_)
    {}

    /**
     * @brief Destroy.
     */
    movie_texture_node::~movie_texture_node() OPENVRML_NOTHROW
    {}

    /**
     * @brief Update the node for the current timestamp.
     *
     * @param time  the current time.
     */
    void movie_texture_node::do_update(double /* time */)
    {
# if 0
        if (this->modified()) {
            if (this->img_) {
                const char * imageUrl = this->img_->url();
                size_t imageLen = strlen(imageUrl);
                size_t i, nUrls = this->url_.mfstring::value.size();
                for (i = 0; i < nUrls; ++i) {
                    size_t len = this->url_.mfstring::value[i].length();

                    if (this->url_.mfstring::value[i] == imageUrl
                        || ((imageLen > len)
                            && (this->url_.mfstring::value[i]
                                == (imageUrl + imageLen - len)))) {
                        break;
                    }
                }

                // if (d_image->url() not in d_url list) ...
                if (i == nUrls) {
                    delete this->img_;
                    this->img_ = 0;
                }
            }
        }

        // Load the movie if needed (should check startTime...)
        if (!this->img_ && !this->url_.mfstring::value.empty()) {
            doc2 baseDoc(this->scene()->url());
            this->img_ = new img;
            if (!this->img_->try_urls(this->url_.mfstring::value, &baseDoc)) {
                std::cerr << "Error: couldn't read MovieTexture from URL "
                          << this->url_ << std::endl;
            }

            const size_t nFrames = this->img_->nframes();
            this->duration_.value = (nFrames >= 0)
                ? double(nFrames)
                : -1.0;
            node::emit_event(this->duration_changed_, time);
            this->frame = int((this->speed_.value >= 0)
                              ? 0
                              : nFrames - 1);
            // Set the last frame equal to the start time.
            // This is needed to properly handle the case where the startTime
            // and stopTime are set at runtime to the same value (spec says
            // that a single loop should occur in this case...)
            this->lastFrameTime = this->start_time_.sftime::value;
        }

        // No pictures to show
        if (!this->img_ || this->img_->nframes() == 0) { return; }

        // See section 4.6.9 of the VRML97 spec for a detailed explanation
        // of the logic here.
        if (!this->active_.value) {
            if (time >= this->start_time_.sftime::value) {
                if (time >= this->stop_time_.sftime::value) {
                    if (this->start_time_.sftime::value
                        >= this->stop_time_.sftime::value) {
                        if (this->loop_.sfbool::value) {
                            this->active_.value = true;
                            node::emit_event(this->is_active_, time);
                            this->lastFrameTime = time;
                            this->frame = int((this->speed_.value >= 0)
                                              ? 0
                                              : this->img_->nframes() - 1);
                            this->modified(true);
                        } else if (this->start_time_.sftime::value
                                   > this->lastFrameTime) {
                            this->active_.value = true;
                            node::emit_event(this->is_active_, time);
                            this->lastFrameTime = time;
                            this->frame = int((this->speed_.value >= 0)
                                              ? 0
                                              : this->img_->nframes() - 1);
                            this->modified(true);
                        }
                    }
                } else if (this->stop_time_.sftime::value > time) {
                    this->active_.value = true;
                    node::emit_event(this->is_active_, time);
                    this->lastFrameTime = time;
                    this->frame = int((this->speed_.value >= 0)
                                      ? 0
                                      : this->img_->nframes() - 1);
                    this->modified(true);
                }
            }
        }
        // Check whether stopTime has passed
        else if (this->active_.value
                 && ((this->stop_time_.sftime::value
                      > this->start_time_.sftime::value
                      && this->stop_time_.sftime::value <= time))
                 || ((this->frame < 0) && !this->loop_.sfbool::value)) {
            this->active_.value = false;
            node::emit_event(this->is_active_, time);
            this->modified(true);
        } else if (this->frame < 0 && this->loop_.sfbool::value) {
            // Reset frame to 0 to begin loop again.
            this->frame = 0;
        }

        // Check whether the frame should be advanced
        else if (this->active_.value
                 && this->lastFrameTime + fabs(1 / this->speed_.value) <= time) {
            this->frame = (this->speed_.value >= 0)
                ? int(time - this->start_time_.sftime::value)
                % int(this->duration_.value / this->speed_.value)
                : -(int(time - this->start_time_.sftime::value)
                    % int(fabs(this->duration_.value / this->speed_.value)));
            assert(this->frame >= 0);

            this->lastFrameTime = time;
            this->modified(true);
        }

        const size_t frame_bytes =
            this->img_->w() * this->img_->h() * this->img_->nc();
        this->image_ =
            openvrml::image(this->img_->w(),
                            this->img_->h(),
                            this->img_->nc(),
                            this->img_->pixels(this->frame),
                            this->img_->pixels(this->frame) + frame_bytes);

        // Tell the scene when the next update is needed.
        if (this->active_.value) {
            double d = this->lastFrameTime + fabs(1 / this->speed_.value) - time;
            this->type().metatype().browser().delta(0.9 * d);
        }
# endif
    }

    /**
     * @brief The image.
     *
     * @return the image.
     */
    const openvrml::image &
    movie_texture_node::image() const OPENVRML_NOTHROW
    {
        return this->image_;
    }

    /**
     * @brief Initialize.
     *
     * @param timestamp the current time.
     *
     * @exception std::bad_alloc    if memory allocation fails.
     */
    void
    movie_texture_node::do_initialize(double)
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
    movie_texture_node::do_shutdown(double)
        OPENVRML_NOTHROW
    {
        assert(this->scene());
        this->scene()->browser().remove_time_dependent(*this);
    }

    /**
     * @brief render_texture implementation.
     *
     * @param v viewer.
     *
     * @return object identifier for the inserted texture.
     */
    openvrml::viewer::texture_object_t
    movie_texture_node::do_render_texture(openvrml::viewer & /* v */)
    {
# if 0
        if (!this->img_ || this->frame < 0) { return 0; }

        viewer::texture_object_t texture_object = 0;

        if (!this->img_->pixels(this->frame)) {
            this->frame = -1;
        } else {
            texture_object = v.insert_texture(this->image_,
                                              this->repeat_s_.value,
                                              this->repeat_t_.value,
                                              !this->active_.value);
        }

        this->lastFrame = this->frame;
        return texture_object;
# endif
        return 0;
    }
}


/**
 * @brief @c node_metatype identifier.
 */
const char * const openvrml_node_vrml97::movie_texture_metatype::id =
    "urn:X-openvrml:node:MovieTexture";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this class object.
 */
openvrml_node_vrml97::movie_texture_metatype::
movie_texture_metatype(openvrml::browser & browser):
    node_metatype(movie_texture_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_vrml97::movie_texture_metatype::~movie_texture_metatype()
    OPENVRML_NOTHROW
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a boost::shared_ptr<node_type> to a node_type capable of
 *         creating MovieTexture nodes.
 *
 * @exception openvrml::unsupported_interface if @p interfaces includes an interface
 *                                  not supported by movie_texture_metatype.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml_node_vrml97::movie_texture_metatype::
do_create_type(const std::string & id,
               const openvrml::node_interface_set & interfaces) const
    OPENVRML_THROW2(openvrml::unsupported_interface, std::bad_alloc)
{
    using namespace openvrml;
    using namespace openvrml::node_impl_util;

    typedef boost::array<node_interface, 14> supported_interfaces_t;
    static const supported_interfaces_t supported_interfaces = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sfbool_id,
                       "loop"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sffloat_id,
                       "speed"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sftime_id,
                       "startTime"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sftime_id,
                       "stopTime"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfstring_id,
                       "url"),
        node_interface(node_interface::field_id,
                       field_value::sfbool_id,
                       "repeatS"),
        node_interface(node_interface::field_id,
                       field_value::sfbool_id,
                       "repeatT"),
        node_interface(node_interface::eventout_id,
                       field_value::sftime_id,
                       "duration_changed"),
        node_interface(node_interface::eventout_id,
                       field_value::sfbool_id,
                       "isActive"),
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

    typedef node_impl_util::node_type_impl<movie_texture_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & movieTextureNodeType = static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface_(interfaces.begin());
         interface_ != interfaces.end();
         ++interface_) {
        supported_interfaces_t::const_iterator supported_interface =
            supported_interfaces.begin() - 1;
        if (*interface_ == *++supported_interface) {
            movieTextureNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &movie_texture_node::loop_);
        } else if (*interface_ == *++supported_interface) {
            movieTextureNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &movie_texture_node::set_speed_,
                &movie_texture_node::speed_,
                &movie_texture_node::speed_changed_);
        } else if (*interface_ == *++supported_interface) {
            movieTextureNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &movie_texture_node::start_time_);
        } else if (*interface_ == *++supported_interface) {
            movieTextureNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &movie_texture_node::stop_time_);
        } else if (*interface_ == *++supported_interface) {
            movieTextureNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &movie_texture_node::url_);
        } else if (*interface_ == *++supported_interface) {
            movieTextureNodeType.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &movie_texture_node::repeat_s_);
        } else if (*interface_ == *++supported_interface) {
            movieTextureNodeType.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &movie_texture_node::repeat_t_);
        } else if (*interface_ == *++supported_interface) {
            movieTextureNodeType.add_eventout(
                supported_interface->field_type,
                supported_interface->id,
                &movie_texture_node::duration_changed_);
        } else if (*interface_ == *++supported_interface) {
            movieTextureNodeType.add_eventout(
                supported_interface->field_type,
                supported_interface->id,
                &movie_texture_node::is_active_);
        } else if (*interface_ == *++supported_interface) {
            movieTextureNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &movie_texture_node::metadata);
        } else if (*interface_ == *++supported_interface) {
            movieTextureNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &movie_texture_node::pause_time_);
        } else if (*interface_ == *++supported_interface) {
            movieTextureNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &movie_texture_node::resume_time_);
        } else if (*interface_ == *++supported_interface) {
            movieTextureNodeType.add_eventout(
                supported_interface->field_type,
                supported_interface->id,
                &movie_texture_node::elapsed_time_emitter_);
        } else if (*interface_ == *++supported_interface) {
            movieTextureNodeType.add_eventout(
                supported_interface->field_type,
                supported_interface->id,
                &movie_texture_node::is_paused_emitter_);
        } else {
            throw unsupported_interface(*interface_);
        }
    }
    return type;
}
