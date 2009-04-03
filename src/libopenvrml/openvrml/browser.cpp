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

# include "browser.h"
# include "scene.h"
# include "scope.h"
# include "viewer.h"
# include <openvrml/local/uri.h>
# include <openvrml/local/node_metatype_registry_impl.h>
# include <openvrml/local/component.h>
# include <openvrml/local/parse_vrml.h>
# include <private.h>
# include <boost/bind.hpp>
# include <boost/functional.hpp>
# include <boost/lexical_cast.hpp>
# include <boost/multi_index/detail/scope_guard.hpp>
# include <boost/thread/thread.hpp>
# include <algorithm>
# include <functional>
# include <cerrno>
# ifdef _WIN32
#   include <sys/timeb.h>
#   include <direct.h>
#   include <time.h>
# else
#   include <sys/time.h>
# endif

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

using namespace boost::multi_index::detail;  // for scope_guard

/**
 * @file openvrml/browser.h
 *
 * @brief Classes that manage the scene.
 */

namespace {

    class OPENVRML_LOCAL default_navigation_info :
        public openvrml::navigation_info_node {
    public:
        explicit default_navigation_info(const openvrml::null_node_type & type)
            OPENVRML_NOTHROW;
        virtual ~default_navigation_info() OPENVRML_NOTHROW;

    private:
        virtual void do_field(const std::string & id,
                              const openvrml::field_value & value)
            OPENVRML_NOTHROW;
        virtual const openvrml::field_value &
        do_field(const std::string & id) const
            OPENVRML_NOTHROW;
        virtual void do_process_event(const std::string & id,
                                      const openvrml::field_value & value,
                                      double timestamp)
            OPENVRML_NOTHROW;
        virtual const openvrml::field_value &
        do_eventout(const std::string & id) const
            OPENVRML_NOTHROW;

        virtual openvrml::event_listener &
        do_event_listener(const std::string & id)
            OPENVRML_THROW1(openvrml::unsupported_interface);
        virtual openvrml::event_emitter &
        do_event_emitter(const std::string & id)
            OPENVRML_THROW1(openvrml::unsupported_interface);

        virtual const std::vector<float> & do_avatar_size() const
            OPENVRML_NOTHROW;
        virtual bool do_headlight() const OPENVRML_NOTHROW;
        virtual float do_speed() const OPENVRML_NOTHROW;
        virtual const std::vector<std::string> & do_type() const
            OPENVRML_NOTHROW;
        virtual float do_visibility_limit() const OPENVRML_NOTHROW;
    };

    const boost::shared_ptr<openvrml::scope> null_scope_ptr;

    /**
     * @brief Construct.
     *
     * @param[in] t @c node_type.
     */
    default_navigation_info::
    default_navigation_info(const openvrml::null_node_type & t)
        OPENVRML_NOTHROW:
        openvrml::node(t, null_scope_ptr),
        openvrml::bounded_volume_node(t, null_scope_ptr),
        openvrml::child_node(t, null_scope_ptr),
        openvrml::navigation_info_node(t, null_scope_ptr)
    {}

    /**
     * @brief Destroy
     */
    default_navigation_info::~default_navigation_info() OPENVRML_NOTHROW
    {}

    /**
     * @brief The avatar dimensions.
     *
     * @return [0.25, 1.6, 0.75]
     */
    const std::vector<float> & default_navigation_info::do_avatar_size() const
        OPENVRML_NOTHROW
    {
        static const float array[] = { 0.25f, 1.6f, 0.75f };
        static const std::vector<float> vec(array, array + 3);
        return vec;
    }

    /**
     * @brief The headlight state.
     *
     * @return @c true
     */
    bool default_navigation_info::do_headlight() const OPENVRML_NOTHROW
    {
        return true;
    }

    /**
     * @brief The speed of the user view.
     *
     * @return 1.0
     */
    float default_navigation_info::do_speed() const OPENVRML_NOTHROW
    {
        return 1.0;
    }

    /**
     * @brief The navigation type.
     *
     * @return ["WALK", "ANY"]
     */
    const std::vector<std::string> & default_navigation_info::do_type() const
        OPENVRML_NOTHROW
    {
        static const char * array[] = { "WALK", "ANY" };
        static const std::vector<std::string> vec(array, array + 2);
        return vec;
    }

    /**
     * @brief The visibility limit.
     *
     * @return 0.0
     */
    float default_navigation_info::do_visibility_limit() const OPENVRML_NOTHROW
    {
        return 0.0;
    }

    void default_navigation_info::do_field(const std::string &,
                                           const openvrml::field_value &)
        OPENVRML_NOTHROW
    {
        assert(false);
    }

    const openvrml::field_value &
    default_navigation_info::do_field(const std::string &) const
        OPENVRML_NOTHROW
    {
        assert(false);
        static const openvrml::sfbool value;
        return value;
    }

    void
    default_navigation_info::do_process_event(const std::string &,
                                              const openvrml::field_value &,
                                              double)
        OPENVRML_NOTHROW
    {
        assert(false);
    }

    const openvrml::field_value &
    default_navigation_info::do_eventout(const std::string &) const
        OPENVRML_NOTHROW
    {
        assert(false);
        static const openvrml::sfbool value;
        return value;
    }

    openvrml::event_listener &
    default_navigation_info::do_event_listener(const std::string & id)
        OPENVRML_THROW1(openvrml::unsupported_interface)
    {
        assert(false);
        throw openvrml::unsupported_interface(this->node::type(), id);
        return *static_cast<openvrml::event_listener *>(0);
    }

    openvrml::event_emitter &
    default_navigation_info::do_event_emitter(const std::string & id)
        OPENVRML_THROW1(openvrml::unsupported_interface)
    {
        assert(false);
        throw openvrml::unsupported_interface(this->node::type(), id);
        return *static_cast<openvrml::event_emitter *>(0);
    }


    class OPENVRML_LOCAL default_viewpoint : public openvrml::viewpoint_node {
        openvrml::mat4f userViewTransform;

    public:
        explicit default_viewpoint(const openvrml::null_node_type & type)
            OPENVRML_NOTHROW;
        virtual ~default_viewpoint() OPENVRML_NOTHROW;

    private:
        virtual void do_field(const std::string & id,
                              const openvrml::field_value & value)
            OPENVRML_NOTHROW;
        virtual const openvrml::field_value &
        do_field(const std::string & id) const
            OPENVRML_NOTHROW;
        virtual void do_process_event(const std::string & id,
                                      const openvrml::field_value & value,
                                      double timestamp)
            OPENVRML_NOTHROW;
        virtual const openvrml::field_value &
        do_eventout(const std::string & id) const
            OPENVRML_NOTHROW;

        virtual openvrml::event_listener &
        do_event_listener(const std::string & id)
            OPENVRML_THROW1(openvrml::unsupported_interface);
        virtual openvrml::event_emitter &
        do_event_emitter(const std::string & id)
            OPENVRML_THROW1(openvrml::unsupported_interface);

        virtual const openvrml::mat4f & do_transformation() const
            OPENVRML_NOTHROW;
        virtual const openvrml::mat4f & do_user_view_transform() const
            OPENVRML_NOTHROW;
        virtual void do_user_view_transform(const openvrml::mat4f & transform)
            OPENVRML_NOTHROW;
        virtual const std::string & do_description() const OPENVRML_NOTHROW;
        virtual float do_field_of_view() const OPENVRML_NOTHROW;
    };

    /**
     * @brief Construct.
     *
     * @param[in] type  the <code>browser</code>'s @c null_node_type instance.
     */
    default_viewpoint::default_viewpoint(const openvrml::null_node_type & type)
        OPENVRML_NOTHROW:
        openvrml::node(type, null_scope_ptr),
        openvrml::bounded_volume_node(type, null_scope_ptr),
        openvrml::child_node(type, null_scope_ptr),
        openvrml::viewpoint_node(type, null_scope_ptr),
        userViewTransform(openvrml::make_mat4f())
    {}

    /**
     * @brief Destroy.
     */
    default_viewpoint::~default_viewpoint() OPENVRML_NOTHROW
    {}

    const openvrml::mat4f & default_viewpoint::do_transformation() const
        OPENVRML_NOTHROW
    {
        using openvrml::mat4f;
        using openvrml::rotation;
        using openvrml::make_rotation;
        using openvrml::vec3f;
        using openvrml::make_vec3f;
        static const vec3f position = make_vec3f(0.0, 0.0, 10.0);
        static const rotation orientation = make_rotation();
        static const vec3f scale = make_vec3f(1.0, 1.0, 1.0);
        static const rotation scaleOrientation = make_rotation();
        static const vec3f center = make_vec3f();
        static const mat4f t(make_transformation_mat4f(position,
                                                       orientation,
                                                       scale,
                                                       scaleOrientation,
                                                       center));
        return t;
    }

    const openvrml::mat4f & default_viewpoint::do_user_view_transform() const
        OPENVRML_NOTHROW
    {
        return this->userViewTransform;
    }

    void
    default_viewpoint::do_user_view_transform(const openvrml::mat4f & transform)
        OPENVRML_NOTHROW
    {
        this->userViewTransform = transform;
    }

    const std::string & default_viewpoint::do_description() const
        OPENVRML_NOTHROW
    {
        static const std::string desc;
        return desc;
    }

    float default_viewpoint::do_field_of_view() const OPENVRML_NOTHROW
    {
        static const float fieldOfView = 0.785398f;
        return fieldOfView;
    }

    void default_viewpoint::do_field(const std::string &,
                                     const openvrml::field_value &)
        OPENVRML_NOTHROW
    {
        assert(false);
    }

    const openvrml::field_value &
    default_viewpoint::do_field(const std::string &) const
        OPENVRML_NOTHROW
    {
        assert(false);
        static const openvrml::sfbool value;
        return value;
    }

    void default_viewpoint::do_process_event(const std::string &,
                                             const openvrml::field_value &,
                                             double)
        OPENVRML_NOTHROW
    {
        assert(false);
    }

    const openvrml::field_value &
    default_viewpoint::do_eventout(const std::string &) const OPENVRML_NOTHROW
    {
        assert(false);
        static const openvrml::sfbool value;
        return value;
    }

    openvrml::event_listener &
    default_viewpoint::do_event_listener(const std::string & id)
        OPENVRML_THROW1(openvrml::unsupported_interface)
    {
        assert(false);
        throw openvrml::unsupported_interface(this->type(), id);
        return *static_cast<openvrml::event_listener *>(0);
    }

    openvrml::event_emitter &
    default_viewpoint::do_event_emitter(const std::string & id)
        OPENVRML_THROW1(openvrml::unsupported_interface)
    {
        assert(false);
        throw openvrml::unsupported_interface(this->type(), id);
        return *static_cast<openvrml::event_emitter *>(0);
    }
}

/**
 * @brief Get a @c node_type_decls map corresponding to a particular profile.
 *
 * @param[in] profile_id    a profile identifier.
 *
 * @return a @c node_type_decls map corresponding to @p profile_id.
 *
 * @exception std::invalid_argument if @p profile_id does not correspond to a
 *                                  known profile.
 * @exception std::bad_alloc        if memory allocation fails.
 */
std::auto_ptr<openvrml::node_type_decls>
openvrml::profile(const std::string & profile_id)
    OPENVRML_THROW2(std::invalid_argument, std::bad_alloc)
{
    try {
        return local::profile_registry_.at(profile_id)
            .create_node_type_desc_map();
    } catch (boost::bad_ptr_container_operation &) {
        throw std::invalid_argument("unknown profile identifier \""
                                    + profile_id + '"');
    }
}

/**
 * @brief Add the nodes corresponding to a component level to a
 *        @c node_type_decls map.
 *
 * @param[in,out] node_types    a @c node_type_decls map.
 * @param[in] component_id      a component identifier.
 * @param[in] level             a component level.
 *
 * @exception std::invalid_argument if @p component_id is not a recognized
 *                                  component identifier; or if @p level does
 *                                  not correspond to a supported level of
 *                                  @p component_id.
 * @exception std::bad_alloc        if memory allocation fails.
 */
void openvrml::add_component(node_type_decls & node_types,
                             const std::string & component_id,
                             const size_t level)
    OPENVRML_THROW2(std::invalid_argument, std::bad_alloc)
{
    try {
        const local::component & comp =
            local::component_registry_.at(component_id);
        comp.add_to_node_type_desc_map(node_types, level);
    } catch (boost::bad_ptr_container_operation &) {
        throw std::invalid_argument("unknown component identifier \""
                                    + component_id + '"');
    }
}


// Max time in seconds between updates. Make this user
// setable to balance performance with cpu usage.
#ifndef DEFAULT_DELTA
#define DEFAULT_DELTA 0.5
#endif

/**
 * @namespace openvrml
 *
 * @brief The OpenVRML Runtime Library
 */

/**
 * @brief VRML MIME media type.
 */
const char openvrml::vrml_media_type[11] = "model/vrml";

/**
 * @brief Experimental VRML MIME media type.
 */
const char openvrml::x_vrml_media_type[15] = "x-world/x-vrml";

/**
 * @brief X3D VRML MIME media type.
 */
const char openvrml::x3d_vrml_media_type[15] = "model/x3d-vrml";

/**
 * @class openvrml::resource_istream openvrml/browser.h
 *
 * @brief An abstract input stream for network resources.
 *
 * @c resource_istream inherits @c std::istream, adding functions to get the
 * URI and the MIME content type associated with the stream.  Users of the
 * library must provide an implementation of this class, to be returned from
 * @c openvrml::resource_fetcher::do_get_resource.
 *
 * @sa openvrml::resource_fetcher::do_get_resource(const std::string &)
 */

/**
 * @brief Construct.
 *
 * @param[in] streambuf a stream buffer.
 */
openvrml::resource_istream::resource_istream(std::streambuf * streambuf):
    std::istream(streambuf)
{}

/**
 * @brief Destroy.
 */
openvrml::resource_istream::~resource_istream()
{}

/**
 * @brief Get the URL associated with the stream.
 *
 * This function delegates to @c #do_url.
 *
 * @return the URL associated with the stream.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
const std::string openvrml::resource_istream::url() const
    OPENVRML_THROW1(std::bad_alloc)
{
    return this->do_url();
}

/**
 * @fn const std::string openvrml::resource_istream::do_url() const
 *
 * @brief Get the URL associated with the stream.
 *
 * @return the URL associated with the stream.
 */

/**
 * @brief Get the MIME content type associated with the stream.
 *
 * This function delegates to @c #do_type.
 *
 * @return the MIME content type associated with the stream.
 */
const std::string openvrml::resource_istream::type() const
    OPENVRML_THROW1(std::bad_alloc)
{
    return this->do_type();
}

/**
 * @fn const std::string openvrml::resource_istream::do_type() const
 *
 * @brief Get the MIME content type associated with the stream.
 *
 * @return the MIME content type associated with the stream.
 */

/**
 * @brief Indicates whether data is available to be read from the stream.
 *
 * This function delegates to @c #do_data_available.
 *
 * @return @c true if there is data in the stream buffer to be read; @c false
 *         otherwise.
 */
bool openvrml::resource_istream::data_available() const OPENVRML_NOTHROW
{
    return this->do_data_available();
}

/**
 * @fn bool openvrml::resource_istream::do_data_available() const
 *
 * @brief Indicates whether data is available to be read from the stream.
 *
 * @return @c true if there is data in the stream buffer to be read; @c false
 *         otherwise.
 */


/**
 * @class openvrml::resource_fetcher
 *
 * @brief An abstract factory for @c resource_istream%s.
 *
 * A concrete implementation of this interface must be passed to
 * <code>browser</code>'s constructor.
 *
 * @sa openvrml::browser
 */

/**
 * @brief Destroy.
 */
openvrml::resource_fetcher::~resource_fetcher() OPENVRML_NOTHROW
{}

/**
 * @brief Fetch a network resource.
 *
 * @param[in] uri   a Uniform Resource Identifier.
 *
 * This function delegates to @c resource_fetcher::do_get_resource.
 *
 * @return the requested resource as a stream.
 *
 * @exception std::invalid_argument if @p uri is malformed or in an
 *                                  unsupported format.
 */
std::auto_ptr<openvrml::resource_istream>
openvrml::resource_fetcher::get_resource(const std::string & uri)
{
    return this->do_get_resource(uri);
}

/**
 * @fn std::auto_ptr<openvrml::resource_istream> openvrml::resource_fetcher::do_get_resource(const std::string & uri)
 *
 * @brief Fetch a network resource.
 *
 * Called by @c #get_resource, clients of OpenVRML are required to provide an
 * implementation for this function.  OpenVRML depends on the implementation
 * of this function for all of its input needs.  As such, what kinds of
 * resources OpenVRML is capable of resolving are entirely dependent on code
 * provided by the application.
 *
 * Implementations should throw @c std::invalid_argument if @p uri is
 * malformed or in an format that is not supported by the implementation.
 *
 * A trivial implementation designed to handle only @c file resources can use
 * @c std::filebuf:
 *
 * @code
 * std::auto_ptr<openvrml::resource_istream>
 * my_resource_fetcher::do_get_resource(const std::string & uri)
 * {
 *     using std::auto_ptr;
 *     using std::invalid_argument;
 *     using std::string;
 *     using openvrml::resource_istream;
 *
 *     class file_resource_istream : public resource_istream {
 *         std::string url_;
 *         std::filebuf buf_;
 *
 *     public:
 *         explicit file_resource_istream(const std::string & path):
 *             resource_istream(&this->buf_)
 *         {
 *             //
 *             // Note that the failbit is set in the constructor if no data
 *             // can be read from the stream.  This is important.  If the
 *             // failbit is not set on such a stream, OpenVRML will attempt
 *             // to read data from a stream that cannot provide it.
 *             //
 *             if (!this->buf_.open(path.c_str(),
 *                                  ios_base::in | ios_base::binary)) {
 *                 this->setstate(ios_base::badbit);
 *             }
 *         }
 *
 *         void url(const std::string & str)
 *         {
 *             this->url_ = str;
 *         }
 *
 *     private:
 *         virtual const std::string url() const
 *         {
 *             return this->url_;
 *         }
 *
 *         virtual const std::string type() const
 *         {
 *             //
 *             // A real application should use OS facilities for this;
 *             // however, that is beyond the scope of this example (which
 *             // is intended to be portable and stupid).
 *             //
 *             using std::find;
 *             using std::string;
 *             using boost::algorithm::iequals;
 *             using boost::next;
 *             string media_type = "application/octet-stream";
 *             const string::const_reverse_iterator dot_pos =
 *                 find(this->url_.rbegin(), this->url_.rend(), '.');
 *             if (dot_pos == this->url_.rend()
 *                 || next(dot_pos.base()) == this->url_.end()) {
 *                 return media_type;
 *             }
 *             const string::const_iterator hash_pos =
 *                 find(next(dot_pos.base()), this->url_.end(), '#');
 *             const string ext(dot_pos.base(), hash_pos);
 *             if (iequals(ext, "wrl")) {
 *                 media_type = "model/vrml";
 *             } else if (iequals(ext, "png")) {
 *                 media_type = "image/png";
 *             } else if (iequals(ext, "jpg") || iequals(ext, "jpeg")) {
 *                 media_type = "image/jpeg";
 *             }
 *             return media_type;
 *         }
 *
 *         virtual bool data_available() const
 *         {
 *             return !!(*this);
 *         }
 *     };
 *
 *     const string scheme = uri.substr(0, uri.find_first_of(':'));
 *     if (scheme != "file") {
 *         throw invalid_argument('\"' + scheme + "\" URI scheme not "
 *                                "supported");
 *     }
 *     //
 *     // file://
 *     //        ^
 *     // 01234567
 *     //
 *     string path = uri.substr(uri.find_first_of('/', 7));
 *
 *     auto_ptr<resource_istream> in(new file_resource_istream(path));
 *     static_cast<file_resource_istream *>(in.get())->url(uri);
 *
 *     return in;
 * }
 * @endcode
 *
 * The @p uri parameter is provided by OpenVRML and can be assumed to be an
 * absolute URI.  As such, it will always have a scheme through which the
 * client code can choose a resolution mechanism.  For more information on URI
 * syntax, see <a
 * href="ftp://ftp.rfc-editor.org/in-notes/std/std66.txt">Internet
 * STD&nbsp;66</a>.
 *
 * @param[in] uri   an absolute Uniform Resource Identifier.
 *
 * @return the requested resource as a stream.
 *
 * @exception std::invalid_argument if @p uri is malformed or in an
 *                                  unsupported format.
 *
 * @sa ftp://ftp.rfc-editor.org/in-notes/std/std66.txt
 */


/**
 * @class openvrml::stream_listener openvrml/browser.h
 *
 * @brief An interface to simplify asynchronously reading a
 *        @c resource_istream.
 */

/**
 * @brief Destroy.
 */
openvrml::stream_listener::~stream_listener() OPENVRML_NOTHROW
{}

/**
 * @brief Called once the stream is available for use.
 *
 * This function calls @c #do_stream_available.
 *
 * @param[in] uri           the URI associated with the stream.
 * @param[in] media_type    the MIME media type for the stream.
 */
void
openvrml::stream_listener::stream_available(const std::string & uri,
                                            const std::string & media_type)
{
    this->do_stream_available(uri, media_type);
}

/**
 * @fn void openvrml::stream_listener::do_stream_available(const std::string & uri, const std::string & media_type)
 *
 * @brief Called by @c #stream_available.
 *
 * Concrete @c stream_listener%s must override this function.
 *
 * @param[in] uri           the URI associated with the stream.
 * @param[in] media_type    the MIME media type for the stream.
 */

/**
 * @brief Called when data is available.
 *
 * This function calls @c #do_data_available.
 *
 * @param[in] data  the data.
 */
void
openvrml::stream_listener::
data_available(const std::vector<unsigned char> & data)
{
    this->do_data_available(data);
}

/**
 * @fn void openvrml::stream_listener::do_data_available(const std::vector<unsigned char> & data)
 *
 * @brief Called by @c #data_available.
 *
 * @param[in] data  the data.
 */

/**
 * @class openvrml::invalid_vrml openvrml/browser.h
 *
 * @brief Exception thrown when the parser fails due to errors in the VRML
 *        input.
 */

/**
 * @var const std::string openvrml::invalid_vrml::url
 *
 * @brief Resource identifier.
 */

/**
 * @var size_t openvrml::invalid_vrml::line
 *
 * @brief Line number.
 */

/**
 * @var size_t openvrml::invalid_vrml::column
 *
 * @brief Column number.
 */

/**
 * @brief Construct.
 *
 * @param[in] url       resource identifier of the stream.
 * @param[in] line      line number where the error was detected.
 * @param[in] column    column number where the error was detected.
 * @param[in] message   description of the error.
 */
openvrml::invalid_vrml::invalid_vrml(const std::string & url,
                                     const size_t line,
                                     const size_t column,
                                     const std::string & message):
    std::runtime_error(message),
    url(url),
    line(line),
    column(column)
{}

/**
 * @brief Destroy.
 */
openvrml::invalid_vrml::~invalid_vrml() throw ()
{}


/**
 * @class openvrml::viewer_in_use openvrml/browser.h
 *
 * @brief Exception thrown when attempting to associate a @c viewer
 *        with a @c browser when the @c viewer is already
 *        associated with a @c browser.
 */

/**
 * @brief Construct.
 */
openvrml::viewer_in_use::viewer_in_use():
    std::invalid_argument("viewer in use")
{}

/**
 * @brief Destroy.
 */
openvrml::viewer_in_use::~viewer_in_use() throw ()
{}


/**
 * @class openvrml::browser_event openvrml/browser.h
 *
 * @brief A <code>browser</code>-wide event.
 */

/**
 * @var openvrml::browser_event::browser
 *
 * @brief Only @c browser%s can construct @c browser_event%s.
 */

/**
 * @enum openvrml::browser_event::type_id
 *
 * @brief @c browser_event type identifier.
 */

/**
 * @var openvrml::browser_event::initialized
 *
 * @brief An @c initialized event is sent once the world has loaded and all
 *        nodes in the initial scene have been initialized.
 */

/**
 * @var openvrml::browser_event::shutdown
 *
 * @brief A @c shutdown event is sent once all of the nodes in the scene have
 *        been shut down.
 */

/**
 * @internal
 *
 * @var openvrml::browser * openvrml::browser_event::source_
 *
 * @brief The @c browser from which the event originated.
 */

/**
 * @internal
 *
 * @var openvrml::browser_event::type_id openvrml::browser_event::id_
 *
 * @brief Event type identifier.
 */

/**
 * @internal
 *
 * @brief Construct.
 *
 * @param[in] b     the @c browser from which the event originated.
 * @param[in] id    the event type.
 */
openvrml::browser_event::browser_event(browser & b, type_id id)
    OPENVRML_NOTHROW:
    source_(&b),
    id_(id)
{}

/**
 * @brief Event type identifier.
 *
 * @return the event type identifier.
 */
openvrml::browser_event::type_id openvrml::browser_event::id() const
    OPENVRML_NOTHROW
{
    return this->id_;
}

/**
 * @brief The @c browser that emitted the event.
 *
 * @return the @c browser that emitted the event.
 */
openvrml::browser & openvrml::browser_event::source() const OPENVRML_NOTHROW
{
    return *this->source_;
}


/**
 * @class openvrml::browser_listener openvrml/browser.h
 *
 * @brief This class should be inherited by classes that want to listen for
 *        @c browser_event%s.
 */

/**
 * @internal
 *
 * @var openvrml::browser_listener::browser
 *
 * @brief @c browser instances need to call @c #browser_changed.
 */

/**
 * @brief Destroy.
 */
openvrml::browser_listener::~browser_listener() OPENVRML_NOTHROW
{}

/**
 * @internal
 *
 * @brief Send a @c browser_event.
 *
 * This function delegates to @c #do_browser_changed.
 *
 * @param[in] event the @c browser_event to send.
 */
void openvrml::browser_listener::browser_changed(const browser_event & event)
{
    this->do_browser_changed(event);
}

/**
 * @fn void openvrml::browser_listener::do_browser_changed(const browser_event & event)
 *
 * @param[in] event the @c browser_event to send.
 */


/**
 * @class openvrml::node_metatype_registry openvrml/browser.h
 *
 * @brief Registry for @c node_metatype%s.
 */

/**
 * @var boost::scoped_ptr<openvrml::local::node_metatype_registry_impl> openvrml::node_metatype_registry::impl_
 *
 * @brief A pointer to the implementation object.
 */

/**
 * @brief Construct.
 *
 * @param[in] b a @c browser.
 */
openvrml::node_metatype_registry::node_metatype_registry(openvrml::browser & b):
    impl_(new local::node_metatype_registry_impl(b))
{
    this->impl_->register_node_metatypes(*this);
}

/**
 * @brief Destroy.
 */
openvrml::node_metatype_registry::~node_metatype_registry() OPENVRML_NOTHROW
{}

/**
 * @brief The @c browser.
 *
 * @return the @c browser.
 */
openvrml::browser & openvrml::node_metatype_registry::browser() const
    OPENVRML_NOTHROW
{
    return this->impl_->browser();
}

/**
 * @brief Register a @c node_metatype.
 *
 * This operation is destructive; that is, if a @c node_metatype is already
 * registered under @p id, @p metatype will replace it in the registry.
 *
 * @param[in] id        the string identifier associated with @p metatype.
 * @param[in] metatype  a @c node_metatype.
 *
 * @exception std::invalid_argument if @p metatype is null.
 * @exception std::bad_alloc        if memory allocation fails.
 */
void
openvrml::node_metatype_registry::register_node_metatype(
    const std::string & id,
    const boost::shared_ptr<node_metatype> & metatype)
    OPENVRML_THROW2(std::invalid_argument, std::bad_alloc)
{
    this->impl_->register_node_metatype(id, metatype);
}

/**
 * @typedef void (*openvrml::register_node_metatypes_func)(openvrml::node_metatype_registry &)
 *
 * @brief The signature of the node module entry point.
 *
 * A node module should have a function with this signature named
 * @c openvrml_register_node_metatypes.
 */

/**
 * @class openvrml::browser openvrml/browser.h
 *
 * @brief Encapsulates a VRML browser.
 *
 * @c browser is the foundation of the OpenVRML runtime.  @c browser is
 * instantiated with an implementation of @c resource_fetcher, which is
 * provided by application code.  The @c resource_fetcher instance must have a
 * longer lifetime than the @c browser instance, since the @c resource_fetcher
 * instance could be used during destruction of the @c browser.  Note,
 * however, that <code>browser</code>'s destructor will block until all
 * threads that may use the @c resource_fetcher have completed.  So it is
 * sufficient to have the @c browser and the @c resource_fetcher destroyed
 * sequentially in the same thread.
 *
 * @sa openvrml::resource_fetcher
 */

/**
 * @example sdl_viewer.cpp
 *
 * Basic example using @c openvrml::browser.
 */

/**
 * @var class openvrml::browser::scene
 *
 * @brief The scene.
 */

/**
 * @internal
 *
 * @var std::auto_ptr<openvrml::null_node_metatype> openvrml::browser::null_node_metatype_
 *
 * @brief &ldquo;Null&rdquo; class object for default nodes (e.g.,
 *        @c default_viewpoint).
 */

/**
 * @internal
 *
 * @var std::auto_ptr<openvrml::null_node_type> openvrml::browser::null_node_type_
 *
 * @brief &ldquo;Null&rdquo; type object for default nodes (e.g., @c
 *        default_viewpoint).
 */

/**
 * @internal
 *
 * @var openvrml::read_write_mutex openvrml::browser::load_root_scene_thread_mutex_
 *
 * @brief Mutex protecting @c #load_root_scene_thread_.
 */

/**
 * @internal
 *
 * @var boost::scoped_ptr<boost::thread> openvrml::browser::load_root_scene_thread_
 *
 * @brief The thread that loads the root scene.
 *
 * This thread is spawned by @c #load_url. It is joined there (in a subsequent
 * call to @c #load_url) or when the @c browser is destroyed.
 */

/**
 * @internal
 *
 * @var boost::thread_group openvrml::browser::load_proto_thread_group_
 *
 * @brief The threads that load @c EXTERNPROTO implementations.
 *
 * These threads @b must be joined by the @c browser before it is destroyed.
 */

/**
 * @internal
 *
 * @var openvrml::script_node_metatype openvrml::browser::script_node_metatype_
 *
 * @brief @c node_metatype for Script @c node%s in the @c browser.
 */

/**
 * @internal
 *
 * @var openvrml::resource_fetcher & openvrml::browser::fetcher_
 *
 * @brief A reference to the @c resource_fetcher associated with the
 *        @c browser.
 */

/**
 * @internal
 *
 * @var openvrml::read_write_mutex openvrml::browser::scene_mutex_
 *
 * @brief Mutex protecting @c #scene_.
 */

/**
 * @internal
 *
 * @var openvrml::scene * openvrml::browser::scene_
 *
 * @brief Pointer to the root @c scene.
 */

/**
 * @internal
 *
 * @var boost::intrusive_ptr<openvrml::node> openvrml::browser::default_viewpoint_
 *
 * @brief The &ldquo;default&rdquo; @c viewpoint_node used when no
 *        @c viewpoint_node in the scene is bound.
 */

/**
 * @internal
 *
 * @var openvrml::read_write_mutex openvrml::browser::active_viewpoint_mutex_
 *
 * @brief Mutex protecting @c #active_viewpoint_.
 */

/**
 * @internal
 *
 * @var openvrml::viewpoint_node * openvrml::browser::active_viewpoint_
 *
 * @brief The currently &ldquo;active&rdquo; @c viewpoint_node.
 */

/**
 * @internal
 *
 * @var boost::intrusive_ptr<openvrml::node> openvrml::browser::default_navigation_info_
 *
 * @brief The &ldquo;default&rdquo; @c navigation_info_node used when no @c
 *        navigation_info_node in the scene is bound.
 */

/**
 * @internal
 *
 * @var openvrml::read_write_mutex openvrml::browser::active_navigation_info_mutex_
 *
 * @brief Mutex protecting @c #active_navigation_info_.
 */

/**
 * @internal
 *
 * @var openvrml::navigation_info_node * openvrml::browser::active_navigation_info_
 *
 * @brief The currently &ldquo;active&rdquo; @c navigation_info_node.
 */

/**
 * @internal
 *
 * @var openvrml::read_write_mutex openvrml::browser::viewpoint_list_mutex_
 *
 * @brief Mutex protecting @c #viewpoint_list_.
 */

/**
 * @internal
 *
 * @var std::list<openvrml::viewpoint_node *> openvrml::browser::viewpoint_list_
 *
 * @brief A list of all the Viewpoint @c node%s in the @c browser.
 */

/**
 * @internal
 *
 * @var openvrml::read_write_mutex openvrml::browser::scoped_lights_mutex_
 *
 * @brief Mutex protecting @c #scoped_lights_.
 */

/**
 * @internal
 *
 * @var std::list<openvrml::node *> openvrml::browser::scoped_lights_
 *
 * @brief A list of all the scoped light @c node%s in the @c browser.
 */

/**
 * @internal
 *
 * @var openvrml::read_write_mutex openvrml::browser::scripts_mutex_
 *
 * @brief Mutex protecting @c #scripts_.
 */

/**
 * @internal
 *
 * @var std::list<openvrml::script_node *> openvrml::browser::scripts_
 *
 * @brief A list of all the Script @c node%s in the @c browser.
 */

/**
 * @internal
 *
 * @var openvrml::read_write_mutex openvrml::browser::timers_mutex_
 *
 * @brief Mutex protecting @c #timers_.
 */

/**
 * @internal
 *
 * @var std::list<openvrml::time_dependent_node *> openvrml::browser::timers_
 *
 * @brief A list of all the TimeSensor @c node%s in the @c browser.
 */

/**
 * @internal
 *
 * @var boost::mutex openvrml::browser::listeners_mutex_
 *
 * @brief Mutex to guard @c #listeners_.
 */

/**
 * @internal
 *
 * @var std::set<openvrml::browser_listener *> openvrml::browser::listeners_
 *
 * @brief The set of @c browser_listener%s that will receive
 *        @c browser_event%s.
 *
 * @sa #add_listener
 * @sa #remove_listener
 */

/**
 * @var bool openvrml::browser::modified_
 *
 * @brief Flag to indicate whether the @c browser has been modified.
 */

/**
 * @var openvrml::read_write_mutex openvrml::browser::modified_mutex_
 *
 * @brief Mutex protecting @c #modified_.
 */

/**
 * @internal
 *
 * @var bool openvrml::browser::new_view
 *
 * @brief Flag to indicate if the user has changed to a new view.
 */

/**
 * @internal
 *
 * @var openvrml::read_write_mutex openvrml::browser::delta_time_mutex_
 *
 * @brief Mutex protecting @c #delta_time.
 */

/**
 * @internal
 *
 * @var double openvrml::browser::delta_time
 *
 * @brief Time elapsed since the last update.
 */

/**
 * @internal
 *
 * @var openvrml::read_write_mutex openvrml::browser::viewer_mutex_
 *
 * @brief Mutex protecting @c #viewer_.
 */

/**
 * @internal
 *
 * @var openvrml::openvrml::viewer * openvrml::browser::viewer_
 *
 * @brief The current @c viewer.
 */

/**
 * @internal
 *
 * @var openvrml::read_write_mutex openvrml::browser::frame_rate_mutex_
 *
 * @brief Mutex protecting @c #frame_rate_.
 */

/**
 * @internal
 *
 * @var double openvrml::browser::frame_rate_
 *
 * @brief Frame rate.
 */

/**
 * @brief Get the current time.
 */
double openvrml::browser::current_time() OPENVRML_NOTHROW
{
# ifdef _WIN32
    _timeb timebuffer;
#   if defined(_MSC_VER) && (_MSC_VER < 1400)
    _ftime(&timebuffer);
#   else
    const errno_t err = _ftime_s(&timebuffer);
    assert(err == 0);
#   endif
    return double(timebuffer.time) + 1.0e-3 * double(timebuffer.millitm);
# else
    timeval tv;
    const int result = gettimeofday(&tv, 0);
    assert(result == 0);

    return double(tv.tv_sec) + 1.0e-6 * double(tv.tv_usec);
# endif
}

/**
 * @var boost::mutex openvrml::browser::out_mutex_
 *
 * @brief Mutex guarding @c #out_.
 */

/**
 * @var std::ostream & openvrml::browser::out_
 *
 * @brief Output stream, generally for console output.
 */

/**
 * @var boost::mutex openvrml::browser::err_mutex_
 *
 * @brief Mutex guarding @c #err_.
 */

/**
 * @var std::ostream & openvrml::browser::err_
 *
 * @brief Error output stream.
 */

/**
 * @var bool openvrml::browser::flags_need_updating
 *
 * @brief Set by @c node::bounding_volume_dirty on any node in this browser
 *        graph, cleared by @c #update_flags.
 *
 * @c true if the bvolume dirty flag has been set on a @c node in the
 * @c browser graph, but has not yet been propagated to that
 * <code>node</code>'s ancestors.
 */

/**
 * @brief Constructor.
 *
 * @param[in] fetcher   a @c resource_fetcher implementation.
 * @param[in] out       output stream for console output.
 * @param[in] err       output stream for error console output.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
openvrml::browser::browser(resource_fetcher & fetcher,
                           std::ostream & out,
                           std::ostream & err)
    OPENVRML_THROW1(std::bad_alloc):
    node_metatype_registry_(new node_metatype_registry(*this)),
    null_node_metatype_(new null_node_metatype(*this)),
    null_node_type_(new null_node_type(*null_node_metatype_)),
    script_node_metatype_(*this),
    fetcher_(fetcher),
    scene_(new scene(*this)),
    default_viewpoint_(new default_viewpoint(*null_node_type_)),
    active_viewpoint_(node_cast<viewpoint_node *>(default_viewpoint_.get())),
    default_navigation_info_(new default_navigation_info(*null_node_type_)),
    active_navigation_info_(
        node_cast<navigation_info_node *>(default_navigation_info_.get())),
    new_view(false),
    delta_time(DEFAULT_DELTA),
    viewer_(0),
    modified_(false),
    frame_rate_(0.0),
    out_(&out),
    err_(&err),
    flags_need_updating(false)
{
    assert(this->active_viewpoint_);
    assert(this->active_navigation_info_);
}

/**
 * @brief Destructor.
 */
openvrml::browser::~browser() OPENVRML_NOTHROW
{
    read_write_mutex::scoped_read_lock
        lock(this->load_root_scene_thread_mutex_);
    if (this->load_root_scene_thread_) {
        this->load_root_scene_thread_->join();
    }

    this->load_proto_thread_group_.join_all();

    const double now = browser::current_time();

    read_write_mutex::scoped_read_lock scene_lock(this->scene_mutex_);
    if (this->scene_) { this->scene_->shutdown(now); }

    this->node_metatype_registry_->impl_->shutdown(now);
    assert(this->viewpoint_list_.empty());
    assert(this->scoped_lights_.empty());
    assert(this->scripts_.empty());
    assert(this->timers_.empty());
}

/**
 * @brief Add a @c node_metatype.
 *
 * If a @c node_metatype identified by @p id has already been added to
 * the browser, it will be replaced.
 *
 * @warning If @c std::bad_alloc is thrown here, the <code>browser</code>'s
 *          @c node_metatype map is left in an unknown state.  In all
 *          likelihood any preexisting entry in the map with the same
 *          implementation identifier as @p id will have been removed.
 *
 * @param[in] id    a @c node_metatype identifier.
 * @param[in] nc    a @c boost::shared_ptr to a @c node_metatype
 *
 * @exception std::invalid_argument if @p nc is null.
 * @exception std::bad_alloc        if memory allocation fails.
 */
void
openvrml::browser::
add_node_metatype(const node_metatype_id & id,
                  const boost::shared_ptr<openvrml::node_metatype> & metatype)
    OPENVRML_THROW2(std::invalid_argument, std::bad_alloc)
{
    this->node_metatype_registry_->impl_->register_node_metatype(id, metatype);
}

/**
 * @brief Get the @c node_metatype corresponding to @p id.
 *
 * @param[in] id    a @c node_metatype identifier.
 *
 * @return the @c node_metatype corresponding to @p id; or a null pointer if
 *         no such @c node_metatype exists.
 */
const boost::shared_ptr<openvrml::node_metatype>
openvrml::browser::node_metatype(const node_metatype_id & id) const
    OPENVRML_NOTHROW
{
    return this->node_metatype_registry_->impl_->find(id);
}

/**
 * @brief Get the root @c scene.
 *
 * @return the root @c scene, or 0 if no @c scene is loaded.
 */
openvrml::scene * openvrml::browser::root_scene() const OPENVRML_NOTHROW
{
    read_write_mutex::scoped_read_lock lock(this->scene_mutex_);
    return this->scene_.get();
}

/**
 * @brief Get the path to a @c node in the scene graph.
 *
 * @param[in] n  the objective @c node.
 *
 * @return the path to @p node, starting with a root @c node, and ending with
 *         @p node. If @p node is not in the scene graph, the returned
 *         @c node_path is empty.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
const openvrml::node_path openvrml::browser::find_node(const node & n) const
    OPENVRML_THROW1(std::bad_alloc)
{
    read_write_mutex::scoped_read_lock lock(this->scene_mutex_);
    assert(this->scene_);

    class FindNodeTraverser : public node_traverser {
        const node & objectiveNode;
        node_path & nodePath;

    public:
        FindNodeTraverser(const node & objectiveNode, node_path & nodePath)
            OPENVRML_NOTHROW:
            objectiveNode(objectiveNode),
            nodePath(nodePath)
        {}

        virtual ~FindNodeTraverser() throw()
        {}

    private:
        virtual void on_entering(node & n) OPENVRML_THROW1(std::bad_alloc)
        {
            this->nodePath.push_back(&n);
            if (&n == &this->objectiveNode) { this->halt_traversal(); }
        }

        virtual void on_leaving(node &) OPENVRML_NOTHROW
        {
            if (!this->halted()) { this->nodePath.pop_back(); }
        }
    };

    node_path nodePath;
    FindNodeTraverser(n, nodePath).traverse(this->scene_->nodes());
    return nodePath;
}

/**
 * @brief Get the active @c viewpoint_node.
 *
 * The active @c viewpoint_node is the one currently associated with the user
 * view.
 *
 * @return the active @c viewpoint_node.
 */
openvrml::viewpoint_node & openvrml::browser::active_viewpoint() const
    OPENVRML_NOTHROW
{
    read_write_mutex::scoped_read_lock lock(this->active_viewpoint_mutex_);
    return *this->active_viewpoint_;
}

/**
 * @brief Set the active @c viewpoint_node.
 *
 * @param[in] viewpoint a @c viewpoint_node.
 *
 * @pre @p viewpoint.scene() == @c this->root_scene()
 */
void openvrml::browser::active_viewpoint(viewpoint_node & viewpoint)
    OPENVRML_NOTHROW
{
    read_write_mutex::scoped_read_lock scene_lock(this->scene_mutex_);
    read_write_mutex::scoped_write_lock
        active_viewpoint_lock(this->active_viewpoint_mutex_);
# ifndef NDEBUG
    scene * root_scene = 0;
    for (root_scene = viewpoint.scene();
         root_scene->parent();
         root_scene = root_scene->parent())
    {}
# endif
    assert(root_scene == this->scene_.get());
    this->active_viewpoint_ = &viewpoint;
}

/**
 * @brief Reset the active @c viewpoint_node to the default.
 */
void openvrml::browser::reset_default_viewpoint() OPENVRML_NOTHROW
{
    read_write_mutex::scoped_write_lock lock(this->active_viewpoint_mutex_);
    assert(this->default_viewpoint_);
    this->active_viewpoint_ =
        node_cast<viewpoint_node *>(this->default_viewpoint_.get());
    assert(this->active_viewpoint_);
}

/**
 * @brief Get the active @c navigation_info_node.
 *
 * The active @c navigation_info_node is the one currently associated with the
 * user view.
 *
 * @return the active @c navigation_info_node.
 */
openvrml::navigation_info_node &
openvrml::browser::active_navigation_info() const OPENVRML_NOTHROW
{
    read_write_mutex::scoped_read_lock
        lock(this->active_navigation_info_mutex_);
    return *this->active_navigation_info_;
}

/**
 * @brief Set the active @c navigation_info_node.
 *
 * @param[in] nav_info a @c navigation_info_node.
 *
 * @pre @p viewpoint.scene() == @c this->root_scene()
 */
void openvrml::browser::active_navigation_info(navigation_info_node & nav_info)
    OPENVRML_NOTHROW
{
    read_write_mutex::scoped_read_lock scene_lock(this->scene_mutex_);
    read_write_mutex::scoped_write_lock
        active_navigation_info_lock(this->active_navigation_info_mutex_);
# ifndef NDEBUG
    scene * root_scene = 0;
    for (root_scene = nav_info.scene();
         root_scene->parent();
         root_scene = root_scene->parent())
    {}
# endif
    assert(root_scene == this->scene_.get());
    this->active_navigation_info_ = &nav_info;
}

/**
 * @brief Reset the active @c navigation_info_node to the default.
 */
void openvrml::browser::reset_default_navigation_info() OPENVRML_NOTHROW
{
    read_write_mutex::scoped_write_lock
        lock(this->active_navigation_info_mutex_);
    assert(this->default_navigation_info_);
    this->active_navigation_info_ =
        node_cast<navigation_info_node *>(
            this->default_navigation_info_.get());
    assert(this->active_navigation_info_);
}

/**
 * @brief Add a @c viewpoint_node to the list of @c viewpoint_node%s for the
 *        @c browser.
 *
 * @param[in] viewpoint a @c viewpoint_node.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 *
 * @pre @p viewpoint is not in the list of @c viewpoint_node%s for the @c
 *      browser.
 */
void openvrml::browser::add_viewpoint(viewpoint_node & viewpoint)
    OPENVRML_THROW1(std::bad_alloc)
{
    read_write_mutex::scoped_write_lock lock(this->viewpoint_list_mutex_);
    assert(std::find(this->viewpoint_list_.begin(), this->viewpoint_list_.end(),
                     &viewpoint) == this->viewpoint_list_.end());
    this->viewpoint_list_.push_back(&viewpoint);
}

/**
 * @brief Remove a @c viewpoint_node from the list of @c viewpoint_node%s for
 *        the @c browser.
 *
 * @param[in] viewpoint a @c viewpoint_node.
 *
 * @pre @p viewpoint is in the list of @c viewpoint_node%s for the @c browser.
 */
void openvrml::browser::remove_viewpoint(viewpoint_node & viewpoint)
    OPENVRML_NOTHROW
{
    read_write_mutex::scoped_write_lock lock(this->viewpoint_list_mutex_);
    assert(!this->viewpoint_list_.empty());
    typedef std::list<viewpoint_node *> viewpoint_list_t;
    const viewpoint_list_t::iterator end = this->viewpoint_list_.end();
    const viewpoint_list_t::iterator pos =
            std::find(this->viewpoint_list_.begin(), end, &viewpoint);
    assert(pos != end);
    this->viewpoint_list_.erase(pos);
}

/**
 * @brief Get the list of @c viewpoint_node%s for the world.
 *
 * @return the list of @c viewpoint_node%s for the world.
 */
const std::list<openvrml::viewpoint_node *>
openvrml::browser::viewpoints() const OPENVRML_NOTHROW
{
    read_write_mutex::scoped_read_lock lock(this->viewpoint_list_mutex_);
    return this->viewpoint_list_;
}

/**
 * @brief Set the current @c viewer.
 *
 * @param[in] v @c viewer.
 *
 * @exception viewer_in_use if @p v is already associated with a
 *                          @c browser.
 */
void openvrml::browser::viewer(openvrml::viewer * v)
    OPENVRML_THROW1(viewer_in_use)
{
    read_write_mutex::scoped_write_lock lock(this->viewer_mutex_);
    if (v && v->browser_) { throw viewer_in_use(); }
    if (this->viewer_) { this->viewer_->browser_ = 0; }
    this->viewer_ = v;
    if (v) { v->browser_ = this; }
}

/**
 * @brief The current @c viewer.
 *
 * @return the current @c viewer.
 */
openvrml::viewer * openvrml::browser::viewer() const OPENVRML_NOTHROW
{
    read_write_mutex::scoped_read_lock lock(this->viewer_mutex_);
    return this->viewer_;
}

/**
 * @brief Get the browser name.
 *
 * @return "OpenVRML"
 *
 * Specific browsers may wish to override this method.
 */
const char * openvrml::browser::name() const OPENVRML_NOTHROW
{
    return "OpenVRML";
}

/**
 * @brief Get the @c browser version.
 *
 * @return the version of OpenVRML.
 *
 * Specific browsers may wish to override this method.
 */
const char * openvrml::browser::version() const OPENVRML_NOTHROW
{
    return PACKAGE_VERSION;
}

/**
 * @brief Get the average navigation speed in meters per second.
 *
 * @return the average navigation speed.
 */
float openvrml::browser::current_speed()
{
    read_write_mutex::scoped_read_lock
        lock(this->active_navigation_info_mutex_);
    navigation_info_node & nav_info = this->active_navigation_info();
    return nav_info.speed();
}

/**
 * @brief Get the URI for the world.
 *
 * @return the URI for the world.
 */
const std::string openvrml::browser::world_url() const
    OPENVRML_THROW1(std::bad_alloc)
{
    read_write_mutex::scoped_read_lock lock(this->scene_mutex_);
    assert(this->scene_);
    return this->scene_->url(); // Throws std::bad_alloc.
}

/**
 * @brief Set the world from a stream.
 *
 * @param[in,out] in    an input stream.
 *
 * @exception bad_media_type    if @p in.type() is not @c model/vrml,
 *                              @c x-world/x-vrml, or @c model/x3d-vrml.
 * @exception invalid_vrml      if @p in has invalid syntax.
 */
void openvrml::browser::set_world(resource_istream & in)
{
    using std::for_each;
    {
        read_write_mutex::scoped_read_write_lock scene_lock(this->scene_mutex_);

        using std::string;
        using local::uri;

        //
        // Clear out the current scene.
        //
        this->load_proto_thread_group_.join_all();
        double now = browser::current_time();
        if (this->scene_) { this->scene_->shutdown(now); }
        this->node_metatype_registry_->impl_->shutdown(now);
        read_write_mutex::scoped_read_lock
            listeners_lock(this->listeners_mutex_);
        for_each(this->listeners_.begin(), this->listeners_.end(),
                 boost::bind2nd(
                     boost::mem_fun(&browser_listener::browser_changed),
                     browser_event(*this, browser_event::shutdown)));

        scene_lock.promote();

        this->scene_.reset();
        assert(this->viewpoint_list_.empty());
        assert(this->scoped_lights_.empty());
        assert(this->scripts_.empty());
        assert(this->timers_.empty());

        //
        // Create the new scene.
        //
        this->node_metatype_registry_.reset(new node_metatype_registry(*this));
        this->scene_.reset(new scene(*this));

        scene_lock.demote();

        this->scene_->load(in);

        //
        // Initialize.
        //
        now = browser::current_time();
        this->scene_->initialize(now);

        //
        // Get the initial viewpoint_node, if any was specified.
        //
        viewpoint_node * initial_viewpoint = 0;
        const string viewpoint_node_id = uri(this->scene_->url()).fragment();
        if (!viewpoint_node_id.empty()) {
            if (!this->scene_->nodes().empty()) {
                using boost::intrusive_ptr;
                const intrusive_ptr<node> & n = this->scene_->nodes().front();
                if (n) {
                    node * const vp = n->scope().find_node(viewpoint_node_id);
                    initial_viewpoint = dynamic_cast<viewpoint_node *>(vp);
                }
            }
        }

        //
        // Initialize the node_metatypes.
        //
        this->node_metatype_registry_->impl_->init(initial_viewpoint, now);

        if (!this->active_viewpoint_) {
            read_write_mutex::scoped_write_lock
                lock(this->active_viewpoint_mutex_);
            this->active_viewpoint_ = this->default_viewpoint_.get();
        }

        this->modified(true);
        this->new_view = true; // Force resetUserNav
    } // unlock this->scene_mutex_, this->active_viewpoint_mutex_

    read_write_mutex::scoped_read_lock
        listeners_lock(this->listeners_mutex_);
    for_each(this->listeners_.begin(), this->listeners_.end(),
             boost::bind2nd(boost::mem_fun(&browser_listener::browser_changed),
                            browser_event(*this, browser_event::initialized)));
}

/**
 * @brief Replace the root nodes of the world.
 *
 * @param[in] nodes new root nodes for the world.
 */
void
openvrml::browser::replace_world(
    const std::vector<boost::intrusive_ptr<node> > & nodes)
{
    read_write_mutex::scoped_read_lock lock(this->scene_mutex_);
    const double now = browser::current_time();
    this->scene_->nodes(nodes);
    this->scene_->initialize(now);
    //
    // Initialize the node_metatypes.
    //
    static viewpoint_node * const initial_viewpoint = 0;
    this->node_metatype_registry_->impl_->init(initial_viewpoint, now);
    this->modified(true);
    this->new_view = true; // Force resetUserNav
}

struct OPENVRML_LOCAL openvrml::browser::root_scene_loader {
    root_scene_loader(browser & b, const std::vector<std::string> & url)
        OPENVRML_THROW1(std::bad_alloc):
        browser_(&b),
        url_(url)
    {}

    void operator()() const OPENVRML_NOTHROW
    {
        try {
            using std::endl;

            openvrml::browser & browser = *this->browser_;

            try {
                std::auto_ptr<resource_istream> in;
                {
                    read_write_mutex::scoped_read_lock
                        lock(browser.scene_mutex_);
                    in = browser.scene_->get_resource(this->url_);
                }
                if (!(*in)) { throw unreachable_url(); }
                browser.set_world(*in);
            } catch (invalid_vrml & ex) {
                std::ostringstream out;
                out << ex.url << ':' << ex.line << ':' << ex.column
                    << ": error: " << ex.what();
                browser.err(out.str());
            } catch (unreachable_url &) {
                throw;
            } catch (std::exception & ex) {
                browser.err(ex.what());
            } catch (...) {
                throw unreachable_url();
            }
        } catch (unreachable_url & ex) {
            this->browser_->err(ex.what());
        }
    }

private:
    browser * browser_;
    const std::vector<std::string> url_;
};

/**
 * @brief Asynchronously load a VRML world into the @c browser.
 *
 * This function takes an alternative URI list consistent with such lists as
 * they appear in VRML nodes.  @c openvrml::browser_event::initialized will be
 * emitted once the world has been loaded and initialized.
 *
 * For synchronously loading a world, see @c #set_world.
 *
 * @param[in] url       a URI.
 * @param[in] parameter parameters for @p url.
 *
 * @exception std::bad_alloc                if memory allocation fails.
 * @exception boost::thread_resource_error  if thread creation fails.
 *
 * @sa #set_world
 */
void openvrml::browser::load_url(const std::vector<std::string> & url,
                                 const std::vector<std::string> &)
    OPENVRML_THROW2(std::bad_alloc, boost::thread_resource_error)
{
    {
        read_write_mutex::scoped_read_lock
            lock(this->load_root_scene_thread_mutex_);
        if (this->load_root_scene_thread_) {
            this->load_root_scene_thread_->join();
        }
    }

    boost::function0<void> f = root_scene_loader(*this, url);

    read_write_mutex::scoped_write_lock
        lock(this->load_root_scene_thread_mutex_);
    this->load_root_scene_thread_.reset(new boost::thread(f));
}

/**
 * @brief Send a string to the user interface.
 *
 * The default implementation of this method simply prints @p description to
 * @a out.  Subclasses can override this method to direct messages to an
 * application's UI; for instance, a status bar.
 *
 * @param[in] description   a string.
 */
void openvrml::browser::description(const std::string & description)
{
    this->out(description);
}


//
// stream_id_index_ is used to construct the URI for the stream; this is used
// to identify any PROTOs in the stream in the browser's node_metatype map.  A
// side-effect of this approach is that the node_metatype map will keep growing,
// even if identical streams are repeatedly loaded.  For this reason it is
// preferable to use an EXTERNPROTO in the stream.
//
namespace {
    OPENVRML_LOCAL size_t stream_id_index_ = 0;
    OPENVRML_LOCAL boost::mutex stream_id_index_mutex_;
}

/**
 * @brief Generate nodes from a stream of VRML syntax.
 *
 * In addition to the exceptions listed, this method may throw any
 * exception that may result from reading the input stream.
 *
 * @param[in,out] in    an input stream.
 * @param[in]     type  MIME content type of @p in.
 *
 * @return the root nodes generated from @p in.
 *
 * @exception invalid_vrml          if @p in has invalid VRML syntax.
 * @exception std::invalid_argument if @p type refers to an unsupported content
 *                                  type.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const std::vector<boost::intrusive_ptr<openvrml::node> >
openvrml::browser::create_vrml_from_stream(std::istream & in,
                                           const std::string & type)
{
    using std::ostringstream;
    using std::string;
    using std::vector;
    using boost::lexical_cast;

    {
        boost::mutex::scoped_lock lock(stream_id_index_mutex_);
        ++stream_id_index_;
    }

    ostringstream stream_id;
    stream_id << local::anonymous_stream_id_prefix
              << lexical_cast<string>(stream_id_index_);

    std::vector<boost::intrusive_ptr<node> > nodes;
    try {
        read_write_mutex::scoped_read_lock lock(this->scene_mutex_);
        assert(this->scene_);
        std::map<string, string> meta;
        local::parse_vrml(in, stream_id.str(), type,
                          *this->scene_, nodes, meta);
    } catch (openvrml::bad_media_type & ex) {
        //
        // bad_media_type is a std::runtime_error.  However, here we're using
        // the media type as an argument.
        //
        throw std::invalid_argument(ex.what());
    }
    return nodes;
}

/**
 * @brief Create nodes from a URI.
 *
 * This function executes asynchronously. When the nodes have been completely
 * loaded, they are sent to the @p event MFNode eventIn of @p node.
 *
 * @param[in] url       an alternative URI list.
 * @param[in] node      the node to which the nodes loaded from @p url should be
 *                      sent as an event.
 * @param[in] event     the event of @p node to which the new nodes will be sent.
 *
 * @exception unsupported_interface         if @p node has no eventIn @p event.
 * @exception std::bad_cast                 if the @p event eventIn of @p node
 *                                          is not an MFNode.
 * @exception boost::thread_resource_error  if thread creation fails.
 */
void
openvrml::browser::
create_vrml_from_url(const std::vector<std::string> & url,
                     const boost::intrusive_ptr<node> & node,
                     const std::string & event)
    OPENVRML_THROW3(unsupported_interface, std::bad_cast,
                    boost::thread_resource_error)
{
    read_write_mutex::scoped_read_lock lock(this->scene_mutex_);
    assert(this->scene_);
    this->scene_->create_vrml_from_url(url, node, event);
}

/**
 * @brief Add a listener for @c browser_event%s.
 *
 * @param[in] listener  a @c browser_listener.
 *
 * @return @c true if @p listener is added successfully; @c false otherwise (if
 *         @p listener is already listening for events from the
 *         @c browser).
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
bool openvrml::browser::add_listener(browser_listener & listener)
    OPENVRML_THROW1(std::bad_alloc)
{
    read_write_mutex::scoped_write_lock lock(this->listeners_mutex_);
    return this->listeners_.insert(&listener).second;
}

/**
 * @brief Remove a listener for @c browser_event%s.
 *
 * @param[in] listener  a @c browser_listener.
 *
 * @return @c true if @p listener is removed successfully; @c false otherwise
 *         (if @p listener is not listening for events from the
 *         @c browser).
 */
bool openvrml::browser::remove_listener(browser_listener & listener)
    OPENVRML_NOTHROW
{
    read_write_mutex::scoped_write_lock lock(this->listeners_mutex_);
    return this->listeners_.erase(&listener) > 0;
}

/**
 * @brief Get the current frame rate.
 *
 * @return the current frame rate.
 */
double openvrml::browser::frame_rate() const
{
    read_write_mutex::scoped_read_lock lock(this->frame_rate_mutex_);
    return this->frame_rate_;
}

/**
 * Called by the @c viewer when the cursor passes over, clicks, drags, or
 * releases a sensitive object (an Anchor or another grouping node with
 * an enabled TouchSensor child).
 */
void openvrml::browser::sensitive_event(node * const n,
                                        const double timestamp,
                                        const bool is_over,
                                        const bool is_active,
                                        const double (&point)[3])
{
    if (n) {
        if (pointing_device_sensor_node * pointing_device_sensor =
            node_cast<pointing_device_sensor_node *>(n)) {
            pointing_device_sensor->activate(timestamp,
                                             is_over,
                                             is_active,
                                             point);
        } else if (grouping_node * g = node_cast<grouping_node *>(n)) {
            //
            // The parent grouping node is registered for Touch/Drag Sensors.
            //
            g->activate_pointing_device_sensors(timestamp,
                                                is_over,
                                                is_active,
                                                point);
            this->modified(true);
        }
    }
}

/**
 * @brief Process events (update the @c browser).
 *
 * This method should be called after each frame is rendered.
 *
 * @return @c true if the @c browser needs to be rerendered, @c false otherwise.
 */
bool openvrml::browser::update(double current_time)
{
    using std::for_each;

    read_write_mutex::scoped_read_lock
        timers_lock(this->timers_mutex_),
        scripts_lock(this->scripts_mutex_);

    if (current_time <= 0.0) { current_time = browser::current_time(); }

    this->delta_time = DEFAULT_DELTA;

    //
    // Update each of the timers.
    //
    for_each(this->timers_.begin(), this->timers_.end(),
             boost::bind2nd(boost::mem_fun(&time_dependent_node::update),
                            current_time));

    //
    // Update each of the scripts.
    //
    for_each(this->scripts_.begin(), this->scripts_.end(),
             boost::bind2nd(boost::mem_fun(&script_node::update),
                            current_time));

    // Signal a redisplay if necessary
    return this->modified();
}

/**
 * @brief Indicate whether the headlight is on.
 *
 * @return @c true if the headlight is on; @c false otherwise.
 */
bool openvrml::browser::headlight_on()
{
    navigation_info_node & nav_info = this->active_navigation_info();
    return nav_info.headlight();
}

/**
 * @brief Draw this @c browser into the specified @c viewer
 */
void openvrml::browser::render()
{
    read_write_mutex::scoped_read_lock
        scene_lock(this->scene_mutex_),
        active_viewpoint_lock_(this->active_viewpoint_mutex_);

    if (!this->viewer_) { return; }

    if (this->new_view) {
        this->viewer_->reset_user_navigation();
        this->new_view = false;
    }
    navigation_info_node & nav_info = this->active_navigation_info();
    const float avatarSize = nav_info.avatar_size().empty()
        ? 0.25f
        : nav_info.avatar_size()[0];
    const float visibilityLimit = nav_info.visibility_limit();

    this->viewer_->set_frustum(this->active_viewpoint_->field_of_view(),
                               avatarSize,
                               visibilityLimit);

    //
    // Per-node_metatype rendering happens before viewer::set_viewpoint is
    // called.  This is important for things like background rendering, since
    // viewer::insert_background must be called before viewer::set_viewpoint.
    //
    this->node_metatype_registry_->impl_->render(*this->viewer_);

    // Activate the headlight.
    // ambient is supposed to be 0 according to the spec...
    if (this->headlight_on()) {
        static const color color = make_color(1.0, 1.0, 1.0);
        static const vec3f direction = make_vec3f(0.0, 0.0, -1.0);
        static const float ambientIntensity = 0.3f;
        static const float intensity = 1.0;

        this->viewer_->insert_dir_light(ambientIntensity,
                                        intensity,
                                        color,
                                        direction);
    }

    // sets the viewpoint transformation
    //
    const mat4f t = this->active_viewpoint_->user_view_transform()
                    * this->active_viewpoint_->transformation();
    vec3f position, scale;
    rotation orientation;
    t.transformation(position, orientation, scale);
    this->viewer_->set_viewpoint(position,
                                 orientation,
                                 avatarSize,
                                 visibilityLimit);

    // Top level object

    this->viewer_->begin_object(0);
    mat4f modelview = t.inverse();
    rendering_context rc(bounding_volume::partial, modelview);
    rc.draw_bounding_spheres = true;

    // Do the browser-level lights (Points and Spots)
    {
        using std::for_each;
        read_write_mutex::scoped_read_lock
            scoped_lights_lock(this->scoped_lights_mutex_);
        for_each(this->scoped_lights_.begin(), this->scoped_lights_.end(),
                 boost::bind2nd(
                     boost::mem_fun(&scoped_light_node::render_scoped_light),
                     *this->viewer_));
    }

    //
    // Render the nodes.  scene_ may be 0 if the world failed to load.
    //
    if (this->scene_) {
        this->scene_->render(*this->viewer_, rc);
    }

    this->viewer_->end_object();

    // This is actually one frame late...
    read_write_mutex::scoped_write_lock
        frame_rate_lock(this->frame_rate_mutex_);
    this->frame_rate_ = this->viewer_->frame_rate();

    this->modified(false);
}

/**
 * @brief Indicate whether rendering is necessary.
 *
 * @param[in] value @c true to indicate that the browser state has changed and
 *                  rerendering is necessary; @c false once rendering has
 *                  occurred.
 */
void openvrml::browser::modified(const bool value)
{
    read_write_mutex::scoped_write_lock lock(this->modified_mutex_);
    this->modified_ = value;
}

/**
 * @brief Check if the browser has been modified.
 *
 * @return @c true if the browser has been modified, @c false otherwise.
 */
bool openvrml::browser::modified() const
{
    read_write_mutex::scoped_read_lock lock(this->modified_mutex_);
    return this->modified_;
}

/**
 * @brief Set the time until the next update is needed.
 *
 * @param[in] d a time interval.
 */
void openvrml::browser::delta(const double d)
{
    read_write_mutex::scoped_write_lock lock(this->delta_time_mutex_);
    if (d < this->delta_time) { this->delta_time = d; }
}

/**
 * @brief Get the time interval between @c browser updates.
 *
 * @return the time interval between @c browser updates.
 */
double openvrml::browser::delta() const
{
    read_write_mutex::scoped_read_lock lock(this->delta_time_mutex_);
    return this->delta_time;
}

/**
 * @brief Add a scoped light node to the @c browser.
 *
 * @param[in] light a light node.
 *
 * @pre @p light is not in the list of light nodes for the @c browser.
 */
void
openvrml::browser::add_scoped_light(scoped_light_node & light)
{
    read_write_mutex::scoped_write_lock lock(this->scoped_lights_mutex_);
    assert(std::find(this->scoped_lights_.begin(), this->scoped_lights_.end(),
                     &light) == this->scoped_lights_.end());
    this->scoped_lights_.push_back(&light);
}

/**
 * @brief Remove a scoped light node from the @c browser.
 *
 * @param[in] light the light node to remove.
 *
 * @pre @p light is in the list of light nodes for the @c browser.
 */
void
openvrml::browser::remove_scoped_light(scoped_light_node & light)
{
    read_write_mutex::scoped_write_lock lock(this->scoped_lights_mutex_);
    assert(!this->scoped_lights_.empty());
    const std::list<scoped_light_node *>::iterator end =
        this->scoped_lights_.end();
    const std::list<scoped_light_node *>::iterator pos =
            std::find(this->scoped_lights_.begin(), end, &light);
    assert(pos != end);
    this->scoped_lights_.erase(pos);
}

/**
 * @brief Add a Script node to the browser.
 *
 * @param[in] script    a Script node.
 *
 * @pre @p script is not in the list of Script nodes for the @c browser.
 */
void openvrml::browser::add_script(script_node & script)
{
    read_write_mutex::scoped_write_lock lock(this->scripts_mutex_);
    assert(std::find(this->scripts_.begin(), this->scripts_.end(), &script)
            == this->scripts_.end());
    this->scripts_.push_back(&script);
}

/**
 * @brief Remove a Script node from the browser.
 *
 * @param[in] script    the Script node to remove.
 *
 * @pre @p script is in the list of Script nodes for the @c browser.
 */
void openvrml::browser::remove_script(script_node & script)
{
    read_write_mutex::scoped_write_lock lock(this->scripts_mutex_);
    assert(!this->scripts_.empty());
    typedef std::list<script_node *> script_node_list_t;
    const script_node_list_t::iterator end = this->scripts_.end();
    const script_node_list_t::iterator pos =
            std::find(this->scripts_.begin(), end, &script);
    assert(pos != end);
    this->scripts_.erase(pos);
}

/**
 * @brief Add a time-dependent node to the browser.
 *
 * @param[in] n a @c time_dependent_node.
 *
 * @pre @p n is not in the list of @c time_dependent_node%s for the @c browser.
 */
void openvrml::browser::add_time_dependent(time_dependent_node & n)
{
    read_write_mutex::scoped_write_lock lock(this->timers_mutex_);
    assert(std::find(this->timers_.begin(), this->timers_.end(), &n)
           == this->timers_.end());
    this->timers_.push_back(&n);
}

/**
 * @brief Remove a time-dependent node from the browser.
 *
 * @param[in] n the @c time_dependent_node to remove.
 *
 * @pre @p n is in the list of @c time_dependent_node%s for the @c browser.
 */
void
openvrml::browser::remove_time_dependent(time_dependent_node & n)
{
    read_write_mutex::scoped_write_lock lock(this->timers_mutex_);
    assert(!this->timers_.empty());
    const std::list<time_dependent_node *>::iterator end = this->timers_.end();
    const std::list<time_dependent_node *>::iterator pos =
            std::find(this->timers_.begin(), end, &n);
    assert(pos != end);
    this->timers_.erase(pos);
}

/**
 * @brief Propagate the bvolume dirty flag from children to ancestors.
 *
 * The invariant is that if a <code>node</code>'s bounding volume is out of
 * date, then the bounding volumes of all that <code>node</code>'s ancestors
 * must be out of date.  However, @c node does not maintain a parent pointer.
 * So we must do a traversal of the entire browser graph to do the
 * propagation.
 *
 * @see node::setBVolumeDirty
 * @see node::isBVolumeDirty
 */
void openvrml::browser::update_flags()
{
//  Node* root = this->getRoot();
//  root->updateModified(0x002);
}

/**
 * @brief Print a message to the output stream.
 *
 * @param[in] str   a string.
 */
void openvrml::browser::out(const std::string & str) const
{
    boost::mutex::scoped_lock(this->out_mutex_);
    *this->out_ << str << std::endl;
}

/**
 * @brief Print a message to the error stream.
 *
 * @param[in] str   a string.
 */
void openvrml::browser::err(const std::string & str) const
{
    boost::mutex::scoped_lock(this->err_mutex_);
    *this->err_ << str << std::endl;
}
