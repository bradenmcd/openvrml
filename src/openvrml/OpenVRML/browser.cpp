// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; -*-
//
// OpenVRML
//
// Copyright (C) 1998  Chris Morley
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

# include <algorithm>
# include <sstream>
# include <stack>
# include <regex.h>
# ifdef _WIN32
#   include <sys/timeb.h>
#   include <time.h>
# else
#   include <sys/time.h>
# endif
# include "private.h"
# include "browser.h"
# include "doc2.hpp"
# include "viewer.h"
# include "scope.h"
# include "VrmlRenderContext.h"
# include "script.h"
# include "vrml97node.h"

namespace OpenVRML {

    class NodeInterfaceTypeMismatch : public std::runtime_error {
    public:
        NodeInterfaceTypeMismatch(node_interface::type_id lhs,
                                  node_interface::type_id rhs);
        virtual ~NodeInterfaceTypeMismatch() throw ();
    };

    namespace Vrml97Node {
        class Anchor;
        class AudioClip;
        class Background;
        class CylinderSensor;
        class Fog;
        class Group;
        class AbstractLight;
        class MovieTexture;
        class NavigationInfo;
        class PlaneSensor;
        class PointLight;
        class SphereSensor;
        class SpotLight;
        class TimeSensor;
        class TouchSensor;
        class Viewpoint;
    }

    class ProtoNode : public node {
        friend class ProtoNodeClass;
        friend class Vrml97Parser;

    public:
        struct ImplNodeInterface {
            OpenVRML::node & node;
            std::string interfaceId;

            ImplNodeInterface(OpenVRML::node & node,
                              const std::string & interfaceId);
        };
        typedef std::multimap<std::string, ImplNodeInterface> ISMap;
        typedef std::map<std::string, field_value_ptr> FieldValueMap;
        typedef std::map<std::string, polled_eventout_value> EventOutValueMap;

        ISMap isMap;

    private:
        FieldValueMap unISdFieldValueMap;
        EventOutValueMap eventOutValueMap;
        std::vector<node_ptr> implNodes;

    public:
        explicit ProtoNode(const node_type & nodeType) throw (std::bad_alloc);
        ProtoNode(const node_type & nodeType,
                  const scope_ptr & scope,
                  const ProtoNode & node) throw (std::bad_alloc);
        virtual ~ProtoNode() throw ();

        const std::vector<node_ptr> & getImplNodes() const throw ();

        void addRootNode(const node_ptr & node) throw (std::bad_alloc);
        void addIS(node & implNode,
                   const std::string & implNodeInterfaceId,
                   const std::string & protoInterfaceId)
            throw (unsupported_interface, NodeInterfaceTypeMismatch,
                   field_value_type_mismatch, std::bad_alloc);

        void update(double time);

        virtual const script_node * to_script() const throw ();
        virtual script_node * to_script() throw ();
        virtual const appearance_node * to_appearance() const throw ();
        virtual appearance_node * to_appearance() throw ();
        virtual const child_node * to_child() const throw ();
        virtual child_node * to_child() throw ();
        virtual const color_node * to_color() const throw ();
        virtual color_node * to_color() throw ();
        virtual const coordinate_node * to_coordinate() const throw ();
        virtual coordinate_node * to_coordinate() throw ();
        virtual const font_style_node * to_font_style() const throw ();
        virtual font_style_node * to_font_style() throw () ;
        virtual const geometry_node * to_geometry() const throw ();
        virtual geometry_node * to_geometry() throw ();
        virtual const material_node * to_material() const throw ();
        virtual material_node * to_material() throw ();
        virtual const normal_node * to_normal() const throw ();
        virtual normal_node * to_normal() throw ();
        virtual const sound_source_node * to_sound_source() const throw ();
        virtual sound_source_node * to_sound_source() throw ();
        virtual const texture_node * to_texture() const throw ();
        virtual texture_node * to_texture() throw ();
        virtual const texture_coordinate_node * to_texture_coordinate() const
                throw ();
        virtual texture_coordinate_node * to_texture_coordinate() throw ();
        virtual const texture_transform_node * to_texture_transform() const
                throw ();
        virtual texture_transform_node * to_texture_transform() throw ();
        virtual const transform_node * to_transform() const throw ();
        virtual transform_node * to_transform() throw ();
        virtual const viewpoint_node * to_viewpoint() const throw ();
        virtual viewpoint_node * to_viewpoint() throw ();

        virtual Vrml97Node::Anchor * to_anchor() const;
        virtual Vrml97Node::AudioClip * to_audio_clip() const;
        virtual Vrml97Node::CylinderSensor * to_cylinder_sensor() const;
        virtual Vrml97Node::AbstractLight * to_light() const;
        virtual Vrml97Node::MovieTexture * to_movie_texture() const;
        virtual Vrml97Node::NavigationInfo * to_navigation_info() const;
        virtual Vrml97Node::PlaneSensor * to_plane_sensor() const;
        virtual Vrml97Node::PointLight * to_point_light() const;
        virtual Vrml97Node::SphereSensor * to_sphere_sensor() const;
        virtual Vrml97Node::SpotLight * to_spot_light() const;
        virtual Vrml97Node::TimeSensor * to_time_sensor() const;
        virtual Vrml97Node::TouchSensor * to_touch_sensor() const;

        virtual void render(OpenVRML::viewer & viewer,
                            VrmlRenderContext context);

    private:
        // Not copyable.
        ProtoNode(const ProtoNode &);
        ProtoNode & operator=(const ProtoNode &);

        virtual void do_initialize(double timestamp) throw (std::bad_alloc);
        virtual void do_field(const std::string & id,
                                 const field_value & value)
                throw (unsupported_interface, std::bad_cast, std::bad_alloc);
        virtual const field_value & do_field(const std::string & id) const
                throw (unsupported_interface);
        virtual void do_process_event(const std::string & id,
                                      const field_value & value,
                                      double timestamp)
                throw (unsupported_interface, std::bad_cast, std::bad_alloc);
        virtual const field_value &
        do_eventout(const std::string & id) const
            throw (unsupported_interface);
        virtual void do_shutdown(double timestamp) throw ();
    };


    class ProtoNodeClass : public node_class {
        friend class ProtoNode;
        friend class Vrml97Parser;

    public:
        class ProtoNodeType : public node_type {
            node_interface_set nodeInterfaces;

        public:
            ProtoNodeType(ProtoNodeClass & nodeClass, const std::string & id)
                throw (unsupported_interface, std::bad_alloc);
            virtual ~ProtoNodeType() throw ();

            virtual const node_interface_set & interfaces() const throw ();
            virtual const node_ptr create_node(const scope_ptr & scope) const
                throw (std::bad_alloc);

            void addInterface(const node_interface & interface)
                throw (std::invalid_argument, std::bad_alloc);
        };

        friend class ProtoNodeType;

    private:
        typedef std::map<std::string, field_value_ptr> DefaultValueMap;

        ProtoNodeType protoNodeType;
        DefaultValueMap defaultValueMap;
        ProtoNode protoNode;

    public:
        explicit ProtoNodeClass(OpenVRML::browser & browser) throw ();
        virtual ~ProtoNodeClass() throw ();

        void addEventIn(field_value::type_id, const std::string & id)
            throw (std::invalid_argument, std::bad_alloc);
        void addEventOut(field_value::type_id, const std::string & id)
            throw (std::invalid_argument, std::bad_alloc);
        void addExposedField(const std::string & id,
                             const field_value_ptr & defaultValue)
            throw (std::invalid_argument, std::bad_alloc);
        void addField(const std::string & id,
                      const field_value_ptr & defaultValue)
            throw (std::invalid_argument, std::bad_alloc);
        void addRootNode(const node_ptr & node) throw (std::bad_alloc);
        void addIS(node & implNode,
                   const std::string & implNodeInterfaceId,
                   const std::string & protoInterfaceId)
            throw (unsupported_interface, NodeInterfaceTypeMismatch,
                   field_value_type_mismatch, std::bad_alloc);

        virtual const node_type_ptr create_type(const std::string & id,
                                              const node_interface_set &)
            throw (unsupported_interface, std::bad_alloc);
    };

    class Vrml97RootScope : public scope {
    public:
        Vrml97RootScope(const browser & browser,
                        const std::string & uri = std::string())
            throw (std::bad_alloc);
        virtual ~Vrml97RootScope() throw ();
    };
}

//
// Including a .cpp file is strange, but it's exactly what we want to do here.
// This usage lets us put the ANTLR parser in an unnamed namespace.
//
#include "Vrml97Parser.cpp"
} // Close "namespace OpenVRML", opened in Vrml97Parser.cpp.

// Max time in seconds between updates. Make this user
// setable to balance performance with cpu usage.
#ifndef DEFAULT_DELTA
#define DEFAULT_DELTA 0.5
#endif

/**
 * @namespace OpenVRML
 *
 * @brief The OpenVRML Runtime Library
 */
namespace OpenVRML {

class null_node_class : public node_class {
public:
    explicit null_node_class(OpenVRML::browser & browser) throw ();
    virtual ~null_node_class() throw ();

    virtual const node_type_ptr
    create_type(const std::string & id,
                const node_interface_set & interfaces)
        throw ();
};


class null_node_type : public node_type {
public:
    explicit null_node_type(null_node_class & nodeClass) throw ();
    virtual ~null_node_type() throw ();

    virtual const node_interface_set & interfaces() const throw ();
    virtual const node_ptr create_node(const scope_ptr & scope) const throw ();
};


class DefaultViewpoint : public viewpoint_node {
    mat4f userViewTransform;

public:
    explicit DefaultViewpoint(const null_node_type & nodeType) throw ();
    virtual ~DefaultViewpoint() throw ();

    virtual const mat4f & transformation() const throw ();
    virtual const mat4f & user_view_transform() const throw ();
    virtual void user_view_transform(const mat4f & transform) throw ();
    virtual const std::string & description() const throw ();
    virtual float field_of_view() const throw ();

private:
    virtual void do_field(const std::string & id, const field_value & value)
        throw ();
    virtual const field_value & do_field(const std::string & id) const
        throw ();
    virtual void do_process_event(const std::string & id,
                                  const field_value & value,
                                  double timestamp)
        throw ();
    virtual const field_value & do_eventout(const std::string & id) const
        throw ();
};


/**
 * @var const double pi
 *
 * @brief pi
 */

/**
 * @var const double pi_2
 *
 * @brief pi/2
 */

/**
 * @var const double pi_4
 *
 * @brief pi/4
 */

/**
 * @var const double inv_pi
 *
 * @brief 1/pi
 */

/**
 * @class invalid_vrml
 *
 * @brief Exception thrown when the parser fails due to errors in the VRML
 *      input.
 */

/**
 * @brief Constructor.
 */
invalid_vrml::invalid_vrml():
    std::runtime_error("Invalid VRML.")
{}

/**
 * @brief Destructor.
 */
invalid_vrml::~invalid_vrml() throw ()
{}


/**
 * @class browser
 *
 * @brief Encapsulates a VRML browser.
 */

/**
 * @var browser::Vrml97Parser
 *
 * @brief VRML97 parser generated by ANTLR.
 */

/**
 * @var browser::ProtoNodeClass
 *
 * @brief Class object for @link ProtoNode ProtoNodes@endlink.
 */

/**
 * @enum browser::CBReason
 *
 * @brief Valid reasons for browser callback.
 */

/**
 * @var browser::CBReason browser::DESTROY_WORLD
 *
 * @brief Destroy the world.
 */

/**
 * @var browser::CBReason browser::REPLACE_WORLD
 *
 * @brief Replace the world.
 */

/**
 * @typedef browser::SceneCB
 *
 * @brief A pointer to a browser callback function.
 *
 * The callback function provoides a way to let the app know when a world is
 * loaded, changed, etc.
 */

/**
 * @typedef browser::NodeClassMap
 *
 * @brief Maps URIs to @link node_class node_classes@endlink.
 */

/**
 * @var browser::NodeClassMap browser::nodeClassMap
 *
 * @brief A map of URIs to node implementations.
 */

/**
 * @var script_node_class browser::scriptNodeClass
 *
 * @brief node_class for Script nodes in the browser.
 */

/**
 * @var scene * browser::scene
 *
 * @brief Pointer to the root scene.
 */

/**
 * @typedef browser::BindStack
 *
 * @brief A list of bound nodes.
 */

/**
 * @var browser::BindStack browser::d_navigationInfoStack
 *
 * @brief The stack of bound NavigationInfo nodes.
 */

/**
 * @var browser::BindStack browser::d_viewpointStack
 *
 * @brief The stack of bound Viewpoint nodes.
 */

/**
 * @var std::list<Node *> browser::d_navigationInfos
 *
 * @brief A list of all the NavigationInfo nodes in the browser.
 */

/**
 * @var std::list<Node *> browser::d_viewpoints
 *
 * @brief A list of all the Viewpoint nodes in the browser.
 */

/**
 * @var std::list<Node *> browser::d_scopedLights
 *
 * @brief A list of all the scoped light nodes in the browser.
 */

/**
 * @var std::list<script_node *> browser::d_scripts
 *
 * @brief A list of all the Script nodes in the browser.
 */

/**
 * @var std::list<Node *> browser::d_timers
 *
 * @brief A list of all the TimeSensor nodes in the browser.
 */

/**
 * @var std::list<Node *> browser::d_audioClips
 *
 * @brief A list of all the AudioClip nodes in the browser.
 */

/**
 * @var std::list<Node *> browser::d_movies
 *
 * @brief A list of all the MovieTexture nodes in the browser.
 */

/**
 * @var std::list<Node *> browser::protoNodeList
 *
 * @brief A list of all the prototype nodes in the browser.
 */

/**
 * @var bool browser::d_modified
 *
 * @brief Flag to indicate whether the browser has been modified.
 */

/**
 * @var bool browser::d_newView
 *
 * @brief Flag to indicate if the user has changed to a new view.
 */

/**
 * @var double browser::d_deltaTime
 *
 * @brief Time elapsed since the last update.
 */

/**
 * @typedef browser::SceneCBList
 *
 * @brief List of functions to call when the world is changed.
 */

/**
 * @struct browser::event
 *
 * @brief An event.
 *
 * An event has a value and a destination, and is associated with a time.
 */

/**
 * @var double browser::event::timestamp
 *
 * @brief The timestamp of the event.
 */

/**
 * @var field_value * browser::event::value
 *
 * @brief The value associated with the event.
 */

/**
 * @var node_ptr browser::event::to_node
 *
 * @brief The node the event is going to.
 */

/**
 * @var std::string browser::event::to_eventin
 *
 * @brief The eventIn of @a to_node the event is going to.
 */

/**
 * @var browser::SceneCBList browser::d_sceneCallbacks
 *
 * @brief List of functions to call when the world is changed.
 */

/**
 * @var double browser::d_frameRate
 *
 * @brief Frame rate.
 */

/**
 * @var browser::MAXEVENTS
 *
 * @brief The maximum number of events which may be queued.
 *
 * Each browser can have a limited number of pending events.
 * Repeatedly allocating/freeing events is slow (it would be
 * nice to get rid of the field cloning, too), and if there are
 * so many events pending, we are probably running too slow to
 * handle them effectively anyway.
 */

/**
 * @var browser::event browser::d_eventMem
 *
 * @brief The event queue.
 *
 * @todo The event queue ought to be sorted by timeStamp.
 */

/**
 * @brief Get the current time.
 */
double browser::current_time() throw ()
{
    double currentTime;
# ifdef _WIN32
    _timeb timebuffer;
    _ftime(&timebuffer);
    currentTime = double(timebuffer.time) + 1.0e-3 * double(timebuffer.millitm);
# else
    timeval tv;
    const int result = gettimeofday(&tv, 0);
    assert(result == 0);

    currentTime = double(tv.tv_sec) + 1.0e-6 * double(tv.tv_usec);
# endif
    return currentTime;
}

/**
 * @var std::ostream & browser::out
 *
 * @brief Output stream, generally for console output.
 */

/**
 * @var std::ostream & browser::err
 *
 * @brief Error output stream.
 */

/**
 * @var bool browser::flags_need_updating
 *
 * @brief Set by node::setBVolumeDirty on any node in this browser graph,
 *      cleared by update_flags.
 *
 * @c true if the bvolume dirty flag has been set on a node in the
 * browser graph, but has not yet been propegated to that node's
 * ancestors.
 */

/**
 * @brief Constructor.
 *
 * @param out   output stream for console output.
 * @param err   output stream for error console output.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
browser::browser(std::ostream & out, std::ostream & err) throw (std::bad_alloc):
    null_node_class_(new null_node_class(*this)),
    null_node_type_(new null_node_type(*null_node_class_)),
    script_node_class_(*this),
    scene_(0),
    default_viewpoint(new DefaultViewpoint(*null_node_type_)),
    active_viewpoint_(default_viewpoint->to_viewpoint()),
    modified_(false),
    new_view(false),
    delta_time(DEFAULT_DELTA),
    frame_rate_(0.0),
    first_event(0),
    last_event(0),
    out(out),
    err(err),
    flags_need_updating(false)
{
    using std::string;
    using std::vector;
    assert(this->active_viewpoint_);
    this->browser::load_url(vector<string>(), vector<string>());
}

/**
 * @brief Destructor.
 */
browser::~browser() throw ()
{
    const double now = browser::current_time();

    if (this->scene_) { this->scene_->shutdown(now); }
    delete this->scene_;
    this->scene_ = 0;
    this->navigation_info_stack.clear();
    assert(this->viewpoint_list.empty());
    assert(this->navigation_infos.empty());
    assert(this->scoped_lights.empty());
    assert(this->scripts.empty());
    assert(this->timers.empty());
    assert(this->audio_clips.empty());
    assert(this->movies.empty());
    assert(this->proto_node_list.empty());
    this->node_class_map.clear();
}

/**
 * @brief Get the root nodes for the browser.
 *
 * @return the root nodes for the browser.
 */
const std::vector<node_ptr> & browser::root_nodes() const throw ()
{
    assert(this->scene_);
    return this->scene_->nodes();
}

/**
 * @brief Get the path to a node in the scene graph.
 *
 * @param n  the objective node.
 *
 * @return the path to @p node, starting with a root node, and ending with
 *         @p node. If @p node is not in the scene graph, the returned
 *         node_path is empty.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
const node_path browser::find_node(const node & n) const
    throw (std::bad_alloc)
{
    assert(this->scene_);

    class FindNodeTraverser : public node_traverser {
        const node & objectiveNode;
        node_path & nodePath;

    public:
        explicit FindNodeTraverser(const node & objectiveNode,
                                   node_path & nodePath) throw ():
            objectiveNode(objectiveNode),
            nodePath(nodePath)
        {}

        virtual ~FindNodeTraverser() throw()
        {}

    private:
        virtual void on_entering(node & n) throw (std::bad_alloc)
        {
            this->nodePath.push_back(&n);
            if (&n == &this->objectiveNode) { this->halt_traversal(); }
        }

        virtual void on_leaving(node & n) throw ()
        {
            if (&n != &this->objectiveNode) { this->nodePath.pop_back(); }
        }
    };

    node_path nodePath;
    FindNodeTraverser(n, nodePath).traverse(this->scene_->nodes());
    return nodePath;
}

/**
 * @brief Get the active viewpoint_node.
 *
 * The active viewpoint_node is the one currently associated with the user
 * view.
 *
 * @return the active viewpoint_node.
 */
viewpoint_node & browser::active_viewpoint() const throw ()
{
    return *this->active_viewpoint_;
}

/**
 * @brief Set the active viewpoint_node.
 *
 * @param viewpoint a viewpoint_node.
 */
void browser::active_viewpoint(viewpoint_node & viewpoint) throw ()
{
    this->active_viewpoint_ = &viewpoint;
}

/**
 * @brief Reset the active viewpoint_node to the default.
 */
void browser::reset_default_viewpoint() throw ()
{
    assert(this->default_viewpoint);
    assert(this->default_viewpoint->to_viewpoint());
    this->active_viewpoint_ = this->default_viewpoint->to_viewpoint();
}

/**
 * @brief Add a viewpoint_node to the list of viewpoint_nodes for the browser.
 *
 * @param viewpoint a viewpoint_node.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 *
 * @pre @p viewpoint is not in the list of viewpoint_nodes for the browser.
 */
void browser::add_viewpoint(viewpoint_node & viewpoint) throw (std::bad_alloc)
{
    assert(std::find(this->viewpoint_list.begin(), this->viewpoint_list.end(),
                     &viewpoint) == this->viewpoint_list.end());
    this->viewpoint_list.push_back(&viewpoint);
}

/**
 * @brief Remove a viewpoint_node from the list of viewpoint_nodes for the
 *      browser.
 *
 * @param viewpoint a viewpoint_node.
 *
 * @pre @p viewpoint is in the list of viewpoint_nodes for the browser.
 */
void browser::remove_viewpoint(viewpoint_node & viewpoint) throw ()
{
    assert(!this->viewpoint_list.empty());
    typedef std::list<viewpoint_node *> viewpoint_list_t;
    const viewpoint_list_t::iterator end = this->viewpoint_list.end();
    const viewpoint_list_t::iterator pos =
            std::find(this->viewpoint_list.begin(), end, &viewpoint);
    assert(pos != end);
    this->viewpoint_list.erase(pos);
}

/**
 * @brief Get the list of @link viewpoint_node viewpoint_nodes@endlink for the
 *      world.
 *
 * @return the list of @link viewpoint_node viewpoint_nodes@endlink for the
 *      world.
 */
const std::list<viewpoint_node *> & browser::viewpoints() const throw ()
{
    return this->viewpoint_list;
}

/**
 * @brief Get the browser name.
 *
 * @return "%OpenVRML"
 *
 * Specific browsers may wish to override this method.
 */
const char * browser::name() const throw ()
{
    return "OpenVRML";
}

/**
 * @brief Get the browser version.
 *
 * @return the version of OpenVRML.
 *
 * Specific browsers may wish to override this method.
 */
const char * browser::version() const throw ()
{
    return PACKAGE_VERSION;
}

/**
 * @brief Get the average navigation speed in meters per second.
 *
 * @return the average navigation speed.
 */
float browser::current_speed()
{
    Vrml97Node::NavigationInfo * const navInfo =
        bindable_navigation_info_top();
    if (navInfo) { return navInfo->getSpeed(); }
    return 0.0f;
}

/**
 * @brief Get the URI for the world.
 *
 * @return the URI for the world.
 */
const std::string  browser::world_url() const throw (std::bad_alloc)
{
    static const std::string empty_string;
    return this->scene_
        ? this->scene_->url() // Throws std::bad_alloc.
        : empty_string;
}

/**
 * @todo Implement me!
 */
void browser::replace_world(const std::vector<node_ptr> & nodes)
{}

namespace {
    typedef std::map<std::string, node_class_ptr> node_class_map_t;

    struct InitNodeClass : std::unary_function<void,
                                               node_class_map_t::value_type>
    {
        explicit InitNodeClass(viewpoint_node * initialViewpoint,
                               const double time)
            throw ():
            initialViewpoint(initialViewpoint),
            time(time)
        {}

        void operator()(const node_class_map_t::value_type & value) const
            throw ()
        {
            assert(value.second);
            value.second->initialize(this->initialViewpoint, this->time);
        }

    private:
        viewpoint_node * initialViewpoint;
        double time;
    };


    struct HasId_ : std::unary_function<bool, viewpoint_node *> {
        explicit HasId_(const std::string & nodeId) throw ():
            nodeId(&nodeId)
        {}

        bool operator()(viewpoint_node * const node) const throw ()
        {
            return node->id() == *this->nodeId;
        }

    private:
        const std::string * nodeId;
    };


    class URI {
        std::string str;
        enum { nmatch = 11 };
        regmatch_t regmatch[nmatch];

    public:
        URI(const std::string & str) throw (invalid_url, std::bad_alloc);

        operator std::string() const throw (std::bad_alloc);

        const std::string getScheme() const throw (std::bad_alloc);
        const std::string getSchemeSpecificPart() const throw (std::bad_alloc);
        const std::string getAuthority() const throw (std::bad_alloc);
        const std::string getUserinfo() const throw (std::bad_alloc);
        const std::string getHost() const throw (std::bad_alloc);
        const std::string getPort() const throw (std::bad_alloc);
        const std::string getPath() const throw (std::bad_alloc);
        const std::string getQuery() const throw (std::bad_alloc);
        const std::string getFragment() const throw (std::bad_alloc);

        const URI resolveAgainst(const URI & absoluteURI) const
                throw (std::bad_alloc);
    };
}

/**
 * @brief Load a resource into the browser.
 *
 * The default implementation of this method simply loads a VRML world
 * into the browser.  If the resource at @p url is not a VRML world, this
 * method as no effect.
 *
 * Implementations should override this method to handle non-VRML resources,
 * handing them off to a Web browser or other appropriate handler.
 *
 * @param url       a URI.
 * @param parameter parameters for @p url.
 *
 * @todo Make this asynchronous.
 */
void browser::load_url(const std::vector<std::string> & url,
                       const std::vector<std::string> & parameter)
    throw (std::bad_alloc)
{
    using std::for_each;
    using std::list;
    using std::string;

    const double now = browser::current_time();

    //
    // Clear out the current scene.
    //
    if (this->scene_) { this->scene_->shutdown(now); }
    delete this->scene_;
    this->scene_ = 0;
    this->navigation_info_stack.clear();
    assert(this->viewpoint_list.empty());
    assert(this->navigation_infos.empty());
    assert(this->scoped_lights.empty());
    assert(this->scripts.empty());
    assert(this->timers.empty());
    assert(this->audio_clips.empty());
    assert(this->movies.empty());
    assert(this->proto_node_list.empty());
    this->node_class_map.clear();

    //
    // Create the new scene.
    //
    this->init_node_class_map();
    this->scene_ = new scene(*this, url);
    this->scene_->initialize(now);

    //
    // Get the initial viewpoint_node, if any was specified.
    //
    viewpoint_node * initialViewpoint = 0;
    const string viewpointNodeId = URI(this->scene_->url()).getFragment();
    if (!viewpointNodeId.empty()) {
        if (!this->scene_->nodes().empty()) {
            const node_ptr & n = this->scene_->nodes()[0];
            if (n) {
                node * const vp = n->scope()->find_node(viewpointNodeId);
                initialViewpoint = dynamic_cast<viewpoint_node *>(vp);
            }
        }
    }

    //
    // Initialize the node_classes.
    //
    for_each(this->node_class_map.begin(), this->node_class_map.end(),
             InitNodeClass(initialViewpoint, now));

    //
    // Send initial bind events to bindable nodes.
    //
    if (!this->navigation_infos.empty()) {
        assert(this->navigation_infos.front());
        this->navigation_infos.front()
                ->process_event("set_bind", sfbool(true), now);
    }

    if (this->active_viewpoint_ != this->default_viewpoint->to_viewpoint()) {
        this->active_viewpoint_->process_event("set_bind", sfbool(true), now);
    }

    this->modified(true);
    this->new_view = true;		// Force resetUserNav
}

/**
 * @brief Send a string to the user interface.
 *
 * The default implementation of this method simply prints @p description to
 * @a out.  Subclasses can override this method to direct messages to an
 * application's UI; for instance, a status bar.
 *
 * @param desc   a string.
 */
void browser::description(const std::string & desc)
{
    this->out << desc << std::endl;
}

/**
 * @brief Generate nodes from a stream of VRML syntax.
 *
 * In addition to the exceptions listed, this method may throw any
 * exception that may result from reading the input stream.
 *
 * @param in    an input stream.
 *
 * @return the root nodes generated from @p in.
 *
 * @exception invalid_vrml       if @p in has invalid VRML syntax.
 * @exception std::bad_alloc    if memory allocation fails.
 */
const std::vector<node_ptr> browser::create_vrml_from_stream(std::istream & in)
{
    std::vector<node_ptr> nodes;
    try {
        Vrml97Scanner scanner(in);
        Vrml97Parser parser(scanner, "");
        parser.vrmlScene(*this, nodes);
    } catch (antlr::RecognitionException & ex) {
        throw invalid_vrml();
    }
    return nodes;
}

/**
 * @todo Implement me!
 */
void browser::create_vrml_from_url(const std::vector<std::string> & url,
                                   const node_ptr & node,
                                   const std::string & event)
{}

/**
 * @brief Initialize the node_class map with the available node
 *        implementations.
 */
void browser::init_node_class_map() {
    using namespace Vrml97Node;
    this->node_class_map["urn:X-openvrml:node:Anchor"] =
            node_class_ptr(new AnchorClass(*this));
    this->node_class_map["urn:X-openvrml:node:Appearance"] =
            node_class_ptr(new AppearanceClass(*this));
    this->node_class_map["urn:X-openvrml:node:AudioClip"] =
            node_class_ptr(new AudioClipClass(*this));
    this->node_class_map["urn:X-openvrml:node:Background"] =
            node_class_ptr(new BackgroundClass(*this));
    this->node_class_map["urn:X-openvrml:node:Billboard"] =
            node_class_ptr(new BillboardClass(*this));
    this->node_class_map["urn:X-openvrml:node:Box"] =
            node_class_ptr(new BoxClass(*this));
    this->node_class_map["urn:X-openvrml:node:Collision"] =
            node_class_ptr(new CollisionClass(*this));
    this->node_class_map["urn:X-openvrml:node:Color"] =
            node_class_ptr(new ColorClass(*this));
    this->node_class_map["urn:X-openvrml:node:ColorInterpolator"] =
            node_class_ptr(new ColorInterpolatorClass(*this));
    this->node_class_map["urn:X-openvrml:node:Cone"] =
            node_class_ptr(new ConeClass(*this));
    this->node_class_map["urn:X-openvrml:node:Coordinate"] =
            node_class_ptr(new CoordinateClass(*this));
    this->node_class_map["urn:X-openvrml:node:CoordinateInterpolator"] =
            node_class_ptr(new CoordinateInterpolatorClass(*this));
    this->node_class_map["urn:X-openvrml:node:Cylinder"] =
            node_class_ptr(new CylinderClass(*this));
    this->node_class_map["urn:X-openvrml:node:CylinderSensor"] =
            node_class_ptr(new CylinderSensorClass(*this));
    this->node_class_map["urn:X-openvrml:node:DirectionalLight"] =
            node_class_ptr(new DirectionalLightClass(*this));
    this->node_class_map["urn:X-openvrml:node:ElevationGrid"] =
            node_class_ptr(new ElevationGridClass(*this));
    this->node_class_map["urn:X-openvrml:node:Extrusion"] =
            node_class_ptr(new ExtrusionClass(*this));
    this->node_class_map["urn:X-openvrml:node:Fog"] =
            node_class_ptr(new FogClass(*this));
    this->node_class_map["urn:X-openvrml:node:FontStyle"] =
            node_class_ptr(new FontStyleClass(*this));
    this->node_class_map["urn:X-openvrml:node:Group"] =
            node_class_ptr(new GroupClass(*this));
    this->node_class_map["urn:X-openvrml:node:ImageTexture"] =
            node_class_ptr(new ImageTextureClass(*this));
    this->node_class_map["urn:X-openvrml:node:IndexedFaceSet"] =
            node_class_ptr(new IndexedFaceSetClass(*this));
    this->node_class_map["urn:X-openvrml:node:IndexedLineSet"] =
            node_class_ptr(new IndexedLineSetClass(*this));
    this->node_class_map["urn:X-openvrml:node:Inline"] =
            node_class_ptr(new InlineClass(*this));
    this->node_class_map["urn:X-openvrml:node:LOD"] =
            node_class_ptr(new LODClass(*this));
    this->node_class_map["urn:X-openvrml:node:Material"] =
            node_class_ptr(new MaterialClass(*this));
    this->node_class_map["urn:X-openvrml:node:MovieTexture"] =
            node_class_ptr(new MovieTextureClass(*this));
    this->node_class_map["urn:X-openvrml:node:NavigationInfo"] =
            node_class_ptr(new NavigationInfoClass(*this));
    this->node_class_map["urn:X-openvrml:node:Normal"] =
            node_class_ptr(new NormalClass(*this));
    this->node_class_map["urn:X-openvrml:node:NormalInterpolator"] =
            node_class_ptr(new NormalInterpolatorClass(*this));
    this->node_class_map["urn:X-openvrml:node:OrientationInterpolator"] =
            node_class_ptr(new OrientationInterpolatorClass(*this));
    this->node_class_map["urn:X-openvrml:node:PixelTexture"] =
            node_class_ptr(new PixelTextureClass(*this));
    this->node_class_map["urn:X-openvrml:node:PlaneSensor"] =
            node_class_ptr(new PlaneSensorClass(*this));
    this->node_class_map["urn:X-openvrml:node:PointLight"] =
            node_class_ptr(new PointLightClass(*this));
    this->node_class_map["urn:X-openvrml:node:PointSet"] =
            node_class_ptr(new PointSetClass(*this));
    this->node_class_map["urn:X-openvrml:node:PositionInterpolator"] =
            node_class_ptr(new PositionInterpolatorClass(*this));
    this->node_class_map["urn:X-openvrml:node:ProximitySensor"] =
            node_class_ptr(new ProximitySensorClass(*this));
    this->node_class_map["urn:X-openvrml:node:ScalarInterpolator"] =
            node_class_ptr(new ScalarInterpolatorClass(*this));
    this->node_class_map["urn:X-openvrml:node:Shape"] =
            node_class_ptr(new ShapeClass(*this));
    this->node_class_map["urn:X-openvrml:node:Sound"] =
            node_class_ptr(new SoundClass(*this));
    this->node_class_map["urn:X-openvrml:node:Sphere"] =
            node_class_ptr(new SphereClass(*this));
    this->node_class_map["urn:X-openvrml:node:SphereSensor"] =
            node_class_ptr(new SphereSensorClass(*this));
    this->node_class_map["urn:X-openvrml:node:SpotLight"] =
            node_class_ptr(new SpotLightClass(*this));
    this->node_class_map["urn:X-openvrml:node:Switch"] =
            node_class_ptr(new SwitchClass(*this));
    this->node_class_map["urn:X-openvrml:node:Text"] =
            node_class_ptr(new TextClass(*this));
    this->node_class_map["urn:X-openvrml:node:TextureCoordinate"] =
            node_class_ptr(new TextureCoordinateClass(*this));
    this->node_class_map["urn:X-openvrml:node:TextureTransform"] =
            node_class_ptr(new TextureTransformClass(*this));
    this->node_class_map["urn:X-openvrml:node:TimeSensor"] =
            node_class_ptr(new TimeSensorClass(*this));
    this->node_class_map["urn:X-openvrml:node:TouchSensor"] =
            node_class_ptr(new TouchSensorClass(*this));
    this->node_class_map["urn:X-openvrml:node:Transform"] =
            node_class_ptr(new TransformClass(*this));
    this->node_class_map["urn:X-openvrml:node:Viewpoint"] =
            node_class_ptr(new ViewpointClass(*this));
    this->node_class_map["urn:X-openvrml:node:VisibilitySensor"] =
            node_class_ptr(new VisibilitySensorClass(*this));
    this->node_class_map["urn:X-openvrml:node:WorldInfo"] =
            node_class_ptr(new WorldInfoClass(*this));
}

/**
 * @brief Execute browser callback functions.
 *
 * @param reason    the CBReason to pass to the callback functions.
 */
void browser::do_callbacks(const cb_reason reason)
{
    scene_cb_list_t::iterator cb, cbend = this->scene_callbacks.end();
    for (cb = this->scene_callbacks.begin(); cb != cbend; ++cb) {
        (*cb)(reason);
    }
}

/**
 * @brief Add a callback function to be called when the world changes.
 *
 * @param cb    a browser callback function.
 */
void browser::add_world_changed_callback(const scene_cb cb)
{
    this->scene_callbacks.push_front(cb);
}

/**
 * @brief Get the current frame rate.
 *
 * @return the current frame rate.
 */
double browser::frame_rate() const
{
    return this->frame_rate_;
}

/**
 * @brief Queue an event for a node.
 *
 * Current events are in the array @a event_mem[first_event, last_event). If
 * @a first_event == @a last_event, the queue is empty. There is a fixed
 * maximum number of events. If we are so far behind that the queue is filled,
 * the oldest events get overwritten.
 */
void browser::queue_event(double timestamp,
                          field_value * value,
                          const node_ptr & to_node,
                          const std::string & to_eventin)
{
    event * e = &this->event_mem[this->last_event];
    e->timestamp = timestamp;
    e->value = value;
    e->to_node = to_node;
    e->to_eventin = to_eventin;
    this->last_event = (this->last_event + 1) % max_events;

    // If the event queue is full, discard the oldest (in terms of when it
    // was put on the queue, not necessarily in terms of earliest timestamp).
    if (this->last_event == this->first_event) {
        e = &this->event_mem[this->last_event];
        delete e->value;
        this->first_event = (this->first_event + 1) % max_events;
    }
}

/**
 * @brief Check if any events are waiting to be distributed.
 *
 * @return @c true if there are pending events, @c false otherwise.
 */
bool browser::events_pending()
{
    return this->first_event != this->last_event;
}


/**
 * @brief Discard all pending events
 */
void browser::flush_events()
{
    while (this->first_event != this->last_event) {
        event *e = &this->event_mem[this->first_event];
        this->first_event = (this->first_event + 1) % max_events;
        delete e->value;
    }
}

/**
 * Called by the viewer when the cursor passes over, clicks, drags, or
 * releases a sensitive object (an Anchor or another grouping node with
 * an enabled TouchSensor child).
 */
void browser::sensitive_event(node * const n,
                              const double timestamp,
                              const bool is_over, const bool is_active,
                              double * const point)
{
    if (n) {
        Vrml97Node::Anchor * a = n->to_anchor();
        if (a) {
            //
            // This should really be (isOver && !isActive && n->wasActive)
            // (ie, button up over the anchor after button down over the
            // anchor)
            //
            if (is_active && is_over) { a->activate(); }
        } else {
            //
            // The parent grouping node is registered for Touch/Drag Sensors.
            //
            grouping_node * const g = n->to_grouping();
            if (g) {
                g->activate(timestamp, is_over, is_active, point);
                this->modified(true);
            }
        }
    }
}

namespace {
    template <typename T>
    struct UpdatePolledNode_ : std::unary_function<T, void> {
        explicit UpdatePolledNode_(double time): time(time) {}

        void operator()(T node) const { node->update(time); }

    private:
        double time;
    };
}

/**
 * @brief Process events (update the browser).
 *
 * This method should be called after each frame is rendered.
 *
 * @return @c true if the browser needs to be rerendered, @c false otherwise.
 */
bool browser::update(double current_time)
{
    if (current_time <= 0.0) { current_time = browser::current_time(); }

    this->delta_time = DEFAULT_DELTA;

    // Update each of the timers.
    std::list<node *>::iterator i, end = this->timers.end();
    for (i = this->timers.begin(); i != end; ++i) {
        Vrml97Node::TimeSensor * t = (*i)->to_time_sensor();
        if (t) { t->update(current_time); }
    }

    // Update each of the clips.
    end = this->audio_clips.end();
    for (i = this->audio_clips.begin(); i != end; ++i) {
        Vrml97Node::AudioClip * c = (*i)->to_audio_clip();
        if (c) { c->update(current_time); }
    }

    // Update each of the movies.
    end = this->movies.end();
    for (i = this->movies.begin(); i != end; ++i) {
        Vrml97Node::MovieTexture * m = (*i)->to_movie_texture();
        if (m) { m->update(current_time); }
    }

    //
    // Update each of the scripts.
    //
    std::for_each(this->scripts.begin(), this->scripts.end(),
                  UpdatePolledNode_<script_node *>(current_time));

    //
    // Update each of the prototype instances.
    //
    std::for_each(this->proto_node_list.begin(), this->proto_node_list.end(),
                  UpdatePolledNode_<ProtoNode *>(current_time));

    // Pass along events to their destinations
    while (this->first_event != this->last_event) {
        event * const e = &this->event_mem[this->first_event];
        this->first_event = (this->first_event + 1) % max_events;

        e->to_node->process_event(e->to_eventin, *e->value, e->timestamp);

        // this needs to change if event values are shared...
        delete e->value;
    }

    // Signal a redisplay if necessary
    return this->modified();
}

bool browser::headlight_on()
{
    Vrml97Node::NavigationInfo * const navInfo =
        this->bindable_navigation_info_top();
    if (navInfo) { return navInfo->getHeadlightOn(); }
    return true;
}

namespace {

    struct RenderNodeClass :
            std::unary_function<void, node_class_map_t::value_type> {
        explicit RenderNodeClass(OpenVRML::viewer & viewer):
            viewer(&viewer)
        {}

        void operator()(const node_class_map_t::value_type & value) const
        {
            value.second->render(*this->viewer);
        }

    private:
        OpenVRML::viewer * viewer;
    };
}

/**
 * @brief Draw this browser into the specified viewer
 */
void browser::render(OpenVRML::viewer & viewer)
{
    if (this->new_view) {
        viewer.reset_user_navigation();
        this->new_view = false;
    }
    float avatarSize = 0.25;
    float visibilityLimit = 0.0;
    Vrml97Node::NavigationInfo * ni = this->bindable_navigation_info_top();
    if (ni) {
        avatarSize = ni->getAvatarSize()[0];
        visibilityLimit = ni->getVisibilityLimit();
    }

    // Activate the headlight.
    // ambient is supposed to be 0 according to the spec...
    if (this->headlight_on()) {
        const color color(1.0, 1.0, 1.0);
        const vec3f direction(0.0, 0.0, -1.0);
        const float ambientIntensity = 0.3;
        const float intensity = 1.0;

        viewer.insert_dir_light(ambientIntensity,
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
    viewer.set_viewpoint(position,
                         orientation,
                         this->active_viewpoint_->field_of_view(),
                         avatarSize,
                         visibilityLimit);

    std::for_each(this->node_class_map.begin(), this->node_class_map.end(),
                  RenderNodeClass(viewer));

    // Top level object

    viewer.begin_object(0);
    mat4f modelview = t.inverse();
    VrmlRenderContext rc(bounding_volume::partial, modelview);
    rc.setDrawBSpheres(true);

    // Do the browser-level lights (Points and Spots)
    std::list<node *>::iterator li, end = this->scoped_lights.end();
    for (li = this->scoped_lights.begin(); li != end; ++li) {
        Vrml97Node::AbstractLight * x = (*li)->to_light();
        if (x) { x->renderScoped(viewer); }
    }

    // Render the nodes
    assert(this->scene_);
    this->scene_->render(viewer, rc);

    viewer.end_object();

    // This is actually one frame late...
    this->frame_rate_ = viewer.frame_rate();

    this->modified(false);

    // If any events were generated during render (ugly...) do an update
    if (this->events_pending()) { this->delta(0.0); }
}

/**
 * @brief Indicate whether rendering is necessary.
 *
 * @param value @c true to indicate that the browser state has changed and
 *              rerendering is necessary; @c false once rendering has occurred.
 */
void browser::modified(const bool value)
{
    this->modified_ = value;
}

/**
 * @brief Check if the browser has been modified.
 *
 * @return @c true if the browser has been modified, @c false otherwise.
 */
bool browser::modified() const
{
    return this->modified_;
}

/**
 * @brief Set the time until the next update is needed.
 *
 * @param d a time interval.
 */
void browser::delta(const double d)
{
    if (d < this->delta_time) { this->delta_time = d; }
}

/**
 * @brief Get the time interval between browser updates.
 *
 * @return the time interval between browser updates.
 */
double browser::delta() const
{
    return this->delta_time;
}

/**
 * @brief Get the top node of a bind_stack_t.
 *
 * @return the top node of @p stack.
 */
const node_ptr browser::bindable_top(const bind_stack_t & stack)
{
    return stack.empty() ? node_ptr(0) : stack.front();
}

/**
 * @brief Push a node onto a bind_stack_t.
 *
 * @param stack the bind_stack_t onto which to push @p node.
 * @param node  the node to push onto @p stack.
 */
void browser::bindable_push(bind_stack_t & stack, const node_ptr & node)
{
    this->bindable_remove(stack, node); // Remove any existing reference
    stack.push_front(node);
    this->modified(true);
}

/**
 * @brief Remove a node from a bind_stack_t.
 *
 * @param stack the bind_stack_t from which to remove @p node.
 * @param node  the node to remove from @p stack.
 */
void browser::bindable_remove(bind_stack_t & stack, const node_ptr & node)
{
    const bind_stack_t::iterator pos =
        std::find(stack.begin(), stack.end(), node);
    if (pos != stack.end()) {
        stack.erase(pos);
        this->modified(true);
    }
}

/**
 * @brief Add a NavigationInfo node to the list of NavigationInfo nodes for the
 *      browser.
 *
 * @param node a NavigationInfo node.
 *
 * @pre @p node is not in the list of NavigationInfo nodes for the browser.
 */
void browser::add_navigation_info(Vrml97Node::NavigationInfo & node)
{
    assert(std::find(this->navigation_infos.begin(),
                     this->navigation_infos.end(), &node)
            == this->navigation_infos.end());
    this->navigation_infos.push_back(&node);
}

/**
 * @brief Remove a NavigationInfo node from the list of NavigationInfo nodes
 *      for the browser.
 *
 * @param n  a NavigationInfo node.
 *
 * @pre @p n is in the list of NavigationInfo nodes for the browser.
 */
void browser::remove_navigation_info(Vrml97Node::NavigationInfo & n)
{
    assert(!this->navigation_infos.empty());
    const std::list<node *>::iterator end = this->navigation_infos.end();
    const std::list<node *>::iterator pos =
            std::find(this->navigation_infos.begin(), end, &n);
    assert(pos != end);
    this->navigation_infos.erase(pos);
}

/**
 * @brief Get the active node on the bound NavigationInfo stack.
 *
 * @return the active node on the bound NavigationInfo stack.
 */
Vrml97Node::NavigationInfo * browser::bindable_navigation_info_top()
{
    node * const n = this->bindable_top(this->navigation_info_stack).get();
    return n ? n->to_navigation_info() : 0;
}

/**
 * @brief Push a NavigationInfo node onto the bound NavigationInfo node stack.
 *
 * @param n a NavigationInfo node.
 */
void browser::bindable_push(Vrml97Node::NavigationInfo * n)
{
    this->bindable_push(this->navigation_info_stack, node_ptr(n));
}

/**
 * @brief Remove a NavigationInfo node from the bound NavigationInfo node
 *        stack.
 *
 * @param n a NavigationInfo node.
 */
void browser::bindable_remove(Vrml97Node::NavigationInfo * n)
{
    this->bindable_remove(this->navigation_info_stack, node_ptr(n));
}

/**
 * @brief Add a scoped light node to the browser.
 *
 * @param light a light node.
 *
 * @pre @p light is not in the list of light nodes for the browser.
 */
void browser::add_scoped_light(Vrml97Node::AbstractLight & light) {
    assert(std::find(this->scoped_lights.begin(), this->scoped_lights.end(),
                     &light) == this->scoped_lights.end());
    this->scoped_lights.push_back(&light);
}

/**
 * @brief Remove a scoped light node from the browser.
 *
 * @param light the light node to remove.
 *
 * @pre @p light is in the list of light nodes for the browser.
 */
void browser::remove_scoped_light(Vrml97Node::AbstractLight & light)
{
    assert(!this->scoped_lights.empty());
    const std::list<node *>::iterator end = this->scoped_lights.end();
    const std::list<node *>::iterator pos =
            std::find(this->scoped_lights.begin(), end, &light);
    assert(pos != end);
    this->scoped_lights.erase(pos);
}

/**
 * @brief Add a MovieTexture node to the browser.
 *
 * @param movie a MovieTexture node.
 *
 * @pre @p movie is not in the list of MovieTexture nodes for the browser.
 */
void browser::add_movie(Vrml97Node::MovieTexture & movie) {
    assert(std::find(this->movies.begin(), this->movies.end(), &movie)
            == this->movies.end());
    this->movies.push_back(&movie);
}

/**
 * @brief Remove a MovieTexture node from the browser.
 *
 * @param movie the MovieTexture node to remove.
 *
 * @pre @p movie is in the list of MovieTexture nodes for the browser.
 */
void browser::remove_movie(Vrml97Node::MovieTexture & movie)
{
    assert(!this->movies.empty());
    const std::list<node *>::iterator end = this->movies.end();
    const std::list<node *>::iterator pos =
            std::find(this->movies.begin(), end, &movie);
    assert(pos != end);
    this->movies.erase(pos);
}

/**
 * @brief Add a Script node to the browser.
 *
 * @param script    a Script node.
 *
 * @pre @p script is not in the list of Script nodes for the browser.
 */
void browser::add_script(script_node & script) {
    assert(std::find(this->scripts.begin(), this->scripts.end(), &script)
            == this->scripts.end());
    this->scripts.push_back(&script);
}

/**
 * @brief Remove a Script node from the browser.
 *
 * @param script    the Script node to remove.
 *
 * @pre @p script is in the list of Script nodes for the browser.
 */
void browser::remove_script(script_node & script) {
    assert(!this->scripts.empty());
    typedef std::list<script_node *> script_node_list_t;
    const script_node_list_t::iterator end = this->scripts.end();
    const script_node_list_t::iterator pos =
            std::find(this->scripts.begin(), end, &script);
    assert(pos != end);
    this->scripts.erase(pos);
}

/**
 * @brief Add a PROTO instance to the browser.
 *
 * @param node  a PROTO instance.
 *
 * @pre @p node is not in the list of prototype instances for the browser.
 */
void browser::add_proto(ProtoNode & node) {
    using std::find;
    assert(find(this->proto_node_list.begin(), this->proto_node_list.end(),
                &node)
           == this->proto_node_list.end());
    this->proto_node_list.push_back(&node);
}

/**
 * @brief Remove a PROTO instance from the browser.
 *
 * @param node  the PROTO instance to remove.
 *
 * @pre @p node exists in the browser's list of prototype instances.
 */
void browser::remove_proto(ProtoNode & node) {
    assert(!this->proto_node_list.empty());
    typedef std::list<ProtoNode *> proto_node_list_t;
    const proto_node_list_t::iterator end = this->proto_node_list.end();
    const proto_node_list_t::iterator pos =
            std::find(this->proto_node_list.begin(), end, &node);
    assert(pos != end);
    this->proto_node_list.erase(pos);
}

/**
 * @brief Add a TimeSensor node to the browser.
 *
 * @param timer a TimeSensor node.
 *
 * @pre @p timer is not in the list of TimeSensor nodes for the browser.
 */
void browser::add_time_sensor(Vrml97Node::TimeSensor & timer) {
    assert(std::find(this->timers.begin(), this->timers.end(), &timer)
           == this->timers.end());
    this->timers.push_back(&timer);
}

/**
 * @brief Remove a TimeSensor node from the browser.
 *
 * @param timer the TimeSensor node to remove.
 *
 * @pre @p timer is in the list of TimeSensor nodes for the browser.
 */
void browser::remove_time_sensor(Vrml97Node::TimeSensor & timer)
{
    assert(!this->timers.empty());
    const std::list<node *>::iterator end = this->timers.end();
    const std::list<node *>::iterator pos =
            std::find(this->timers.begin(), end, &timer);
    assert(pos != end);
    this->timers.erase(pos);
}


/**
 * @brief Add an AudioClip node to the browser.
 *
 * @param audio_clip    an AudioClip node.
 *
 * @pre @p audio_clip is not in the list of AudioClip nodes for the browser.
 */
void browser::add_audio_clip(Vrml97Node::AudioClip & audio_clip) {
    assert(std::find(this->audio_clips.begin(), this->audio_clips.end(),
                     &audio_clip) == this->audio_clips.end());
    this->audio_clips.push_back(&audio_clip);
}

/**
 * @brief Remove an AudioClip node from the browser.
 *
 * @param audio_clip    the AudioClip node to remove.
 *
 * @pre @p audio_clip is in the list of AudioClip nodes for the browser.
 */
void browser::remove_audio_clip(Vrml97Node::AudioClip & audio_clip)
{
    assert(!this->audio_clips.empty());
    const std::list<node *>::iterator end = this->audio_clips.end();
    const std::list<node *>::iterator pos =
            std::find(this->audio_clips.begin(), end, &audio_clip);
    assert(pos != end);
    this->audio_clips.erase(pos);
}


/**
 * @brief Propagate the bvolume dirty flag from children to ancestors.
 *
 * The invariant is that if a node's bounding volume is out of date,
 * then the bounding volumes of all that nodes's ancestors must be
 * out of date. However, Node does not maintain a parent pointer. So
 * we must do a traversal of the entire browser graph to do the propagation.
 *
 * @see node::setBVolumeDirty
 * @see node::isBVolumeDirty
 */
void browser::update_flags()
{
//  Node* root = this->getRoot();
//  root->updateModified(0x002);
}


/**
 * @class bad_url
 *
 * @brief Thrown when there is a problem resolving a URI.
 */

/**
 * @brief Constructor.
 *
 * @param message   Informative text.
 */
bad_url::bad_url(const std::string & message): std::runtime_error(message) {}

/**
 * @brief Destructor.
 */
bad_url::~bad_url() throw () {}


/**
 * @class invalid_url
 *
 * @brief Thrown when parsing a URI fails.
 */

/**
 * @brief Constructor.
 */
invalid_url::invalid_url(): bad_url("Invalid URI.") {}

/**
 * @brief Destructor.
 */
invalid_url::~invalid_url() throw () {}


/**
 * @class unreachable_url
 *
 * @brief Thrown when a URI cannot be reached.
 */

/**
 * @brief Constructor.
 */
unreachable_url::unreachable_url(): bad_url("Unreachable URI.") {}

/**
 * @brief Destructor.
 */
unreachable_url::~unreachable_url() throw () {}


/**
 * @class scene
 *
 * @brief A scene in the VRML world.
 */

/**
 * @var mfnode scene::nodes
 *
 * @brief The nodes for the scene.
 */

/**
 * @var const std::string scene::uri
 *
 * @brief The URI for the scene.
 *
 * @a uri may be a relative or an absolute reference.
 */

/**
 * @var browser & scene::browser
 *
 * @brief A reference to the browser associated with the scene.
 */

/**
 * @var scene * const scene::parent
 *
 * @brief A pointer to the parent scene.
 *
 * If the scene is the root scene, @a parent will be 0.
 */

namespace {

# ifdef _WIN32
    struct IsBackslash : std::unary_function<char, bool> {
        bool operator()(const char c) const throw ()
        {
            return c == '\\';
        }
    };
# endif

    const URI createFileURL(const URI & uri) throw ()
    {
        assert(uri.getScheme().empty());

        using std::string;

        string result = "file://";

# ifdef _WIN32
        //
        // _fullpath returns a string starting with the drive letter; for the
        // URL, the path must begin with a '/'. So we simply put one at the
        // beginning of the buffer.
        //
        char buffer[_MAX_PATH] = { '/' };
        char * resolvedPath =
                _fullpath(buffer + 1, uri.getPath().c_str(), _MAX_PATH);
        if (!resolvedPath) {
            //
            // XXX Failed; need to check errno to see what we should throw.
            //
            return result;
        }
        std::replace_if(resolvedPath, resolvedPath + strlen(resolvedPath) + 1,
                        IsBackslash(), '/');
        --resolvedPath;
        assert(resolvedPath == buffer);
# else
        char buffer[PATH_MAX];
        const char * resolvedPath = realpath(uri.getPath().c_str(), buffer);
        if (!resolvedPath) {
            //
            // XXX Failed; need to check errno to see what we should throw.
            //
            return result;
        }
# endif

        result += resolvedPath;

        string query = uri.getQuery();
        if (!query.empty()) { result += '?' + query; }

        string fragment = uri.getFragment();
        if (!fragment.empty()) { result += '#' + fragment; }

        return result;
    }
}

/**
 * @brief Construct a scene from a URI.
 *
 * @param browser   the browser associated with the scene.
 * @param uri       the URI for the scene.
 * @param parent    the parent scene.
 *
 * @exception invalid_vrml       if there is a syntax error in the VRML input.
 * @exception std::bad_alloc    if memory allocation fails.
 */
scene::scene(OpenVRML::browser & browser,
             const std::vector<std::string> & uri,
             scene * parent)
    throw (invalid_vrml, std::bad_alloc):
    browser(browser),
    parent(parent)
{
    std::string absoluteURI;
    for (size_t i = 0; i < uri.size(); ++i) {
        try {
            //
            // Throw invalid_url if it isn't a valid URI.
            //
            URI testURI(uri[i]);

            const bool absolute = !testURI.getScheme().empty();
            if (absolute) {
                absoluteURI = testURI;
            } else if (!parent) {
                //
                // If we have a relative reference and the parent is null, then
                // assume the reference is to the local file system: convert
                // the relative reference to a file URL.
                //
                absoluteURI = createFileURL(testURI);
            } else {
                //
                // If we have a relative URI and parent is not null, try to
                // resolve the relative reference against the parent's URI.
                //
                absoluteURI = testURI.resolveAgainst(URI(parent->url()));
            }

            Doc2 doc(absoluteURI);
            std::istream & in = doc.inputStream();
            if (!in) { throw unreachable_url(); }
            try {
                Vrml97Scanner scanner(in);
                Vrml97Parser parser(scanner, this->url());
                parser.vrmlScene(browser, this->nodes_);
            } catch (antlr::RecognitionException & ex) {
                throw invalid_vrml();
            } catch (std::bad_alloc &) {
                throw;
            } catch (...) {
                throw unreachable_url();
            }
        } catch (bad_url & ex) {
            browser.err << ex.what() << std::endl;
            continue;
        }
        //
        // If this is the root scene (that is, the parent is null), then
        // this->uri must be the absolute URI.
        //
        this->url_ = parent
            ? uri[i]
            : absoluteURI;
        break;
    }
}

/**
 * @brief Get the absolute URI for the scene.
 *
 * @return the absolute URI for the scene.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
const std::string scene::url() const throw (std::bad_alloc) {
    using std::string;
    return this->parent
            ? string(URI(this->url_).resolveAgainst(URI(this->parent->url())))
            : this->url_;
}

/**
 * @brief Initialize the scene.
 *
 * @param timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void scene::initialize(const double timestamp) throw (std::bad_alloc)
{
    for (std::vector<node_ptr>::iterator node(this->nodes_.begin());
         node != this->nodes_.end();
         ++node) {
        assert(*node);
        (*node)->initialize(*this, timestamp);
        (*node)->relocate();
    }
}

/**
 * @brief Render the scene.
 *
 * @param viewer    a Viewer to render to.
 * @param context   a VrmlRenderContext.
 */
void scene::render(OpenVRML::viewer & viewer, VrmlRenderContext context) {
    for (std::vector<node_ptr>::iterator node(this->nodes_.begin());
         node != this->nodes_.end();
         ++node) {
        assert(*node);
        (*node)->render(viewer, context);
    }
}

/**
 * @brief Load a resource into @a browser.
 *
 * This method simply resolves any relative references in @p uri and calls
 * browser::load_url.
 *
 * @note There are a couple of edge cases here where we are probably doing the
 *      wrong thing:
 *       - If there is a URI in the list of the form "#NodeId" and it is not
 *         the first URI in the list, this URI will be loaded as if it were a
 *         new world rather than as a Viewpoint that should simply be bound.
 *       - If the first URI in the list is of the form "#NodeId" and no
 *         Viewpoint named "NodeId" exists in the scene, this method will not
 *         try any subsequent URIs in the list.
 *
 * @param url       an array of URIs. Per VRML97 convention, the first resource
 *                  in the sequence that can be reached will be loaded into the
 *                  browser.
 * @param parameter an array of parameters to be associated with the URIs in
 *                  @p uri.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 *
 * @todo This method currently fails silently if any of the URIs in @p url is
 *      invalid. Should this throw invalid_url?
 */
void scene::load_url(const std::vector<std::string> & url,
                     const std::vector<std::string> & parameter)
    throw (std::bad_alloc)
{
    using std::string;

    if (!url.empty()) {
        if (url[0][0] == '#') {
# if 0
            //
            // If the first element in uri is a Viewpoint name, bind the
            // Viewpoint.
            //
            this->browser.setViewpoint(uri[0].substr(1));
# endif
        } else {
            std::vector<std::string> absoluteURIs(url.size());
            for (size_t i = 0; i < absoluteURIs.size(); ++i) {
                try {
                    const URI urlElement(url[i]);
                    const string value =
                        urlElement.getScheme().empty()
                            ? urlElement.resolveAgainst(URI(this->url()))
                            : urlElement;
                    absoluteURIs[i] = value;
                } catch (invalid_url & ex) {
                    OPENVRML_PRINT_EXCEPTION_(ex);
                }
            }
            this->browser.load_url(absoluteURIs, parameter);
        }
    }
}

/**
 * @brief Shut down the nodes in the scene.
 *
 * This function @b must be called before the scene is destroyed.
 *
 * @param timestamp the current time.
 */
void scene::shutdown(const double timestamp) throw ()
{
    for (std::vector<node_ptr>::iterator node(this->nodes_.begin());
         node != this->nodes_.end();
         ++node) {
        if (*node) { (*node)->shutdown(timestamp); }
    }
}


namespace {
    const std::string toString(const node_interface::type_id interfaceType)
    {
        std::ostringstream out;
        out << interfaceType;
        return out.str();
    }
}

NodeInterfaceTypeMismatch::NodeInterfaceTypeMismatch(
        const node_interface::type_id lhs, const node_interface::type_id rhs):
    std::runtime_error(toString(lhs) + " cannot be mapped to " + toString(rhs))
{}

NodeInterfaceTypeMismatch::~NodeInterfaceTypeMismatch() throw ()
{}


/**
 * @internal
 *
 * @class ProtoNode
 *
 * @brief A prototype node instance.
 *
 * An archetypal ProtoNode is constructed in the process of parsing a @c PROTO,
 * and stored in the ProtoNodeClass. Calls to @c ProtoNodeType::createNode
 * return a clone of the archetypal instance.
 */

/**
 * @var ProtoNode::ProtoNodeClass
 *
 * @brief Class object for ProtoNode instances.
 */

/**
 * @var ProtoNode::Vrml97Parser
 *
 * @brief VRML97 parser (generated by ANTLR).
 */

/**
 * @internal
 *
 * @struct ProtoNode::ImplNodeInterface
 *
 * @brief Used for @c IS event propagation.
 */

/**
 * @var Node & ProtoNode::ImplNodeInterface::node
 *
 * @brief A reference to a node in the prototype implementation.
 */

/**
 * @var std::string ProtoNode::ImplNodeInterface::interfaceId
 *
 * @brief An interface of @a node.
 */

/**
 * @brief Constructor.
 *
 * @param node          a reference to a node in the prototype implementation.
 * @param interfaceId   an interface of @p node.
 */
ProtoNode::
ImplNodeInterface::ImplNodeInterface(OpenVRML::node & node,
                                     const std::string & interfaceId):
    node(node),
    interfaceId(interfaceId)
{}

namespace {
    const field_value_ptr
    default_field_value(const field_value::type_id field_type)
    {
        switch (field_type) {
        case field_value::sfbool_id:     return field_value_ptr(new sfbool);
        case field_value::sfcolor_id:    return field_value_ptr(new sfcolor);
        case field_value::sffloat_id:    return field_value_ptr(new sffloat);
        case field_value::sfimage_id:    return field_value_ptr(new sfimage);
        case field_value::sfint32_id:    return field_value_ptr(new sfint32);
        case field_value::sfnode_id:     return field_value_ptr(new sfnode);
        case field_value::sfrotation_id: return field_value_ptr(new sfrotation);
        case field_value::sfstring_id:   return field_value_ptr(new sfstring);
        case field_value::sftime_id:     return field_value_ptr(new sftime);
        case field_value::sfvec2f_id:    return field_value_ptr(new sfvec2f);
        case field_value::sfvec3f_id:    return field_value_ptr(new sfvec3f);
        case field_value::mfcolor_id:    return field_value_ptr(new mfcolor);
        case field_value::mffloat_id:    return field_value_ptr(new mffloat);
        case field_value::mfint32_id:    return field_value_ptr(new mfint32);
        case field_value::mfnode_id:     return field_value_ptr(new mfnode);
        case field_value::mfrotation_id: return field_value_ptr(new mfrotation);
        case field_value::mfstring_id:   return field_value_ptr(new mfstring);
        case field_value::mftime_id:     return field_value_ptr(new mftime);
        case field_value::mfvec2f_id:    return field_value_ptr(new mfvec2f);
        case field_value::mfvec3f_id:    return field_value_ptr(new mfvec3f);
        default: assert(false);
        }
        return field_value_ptr(0);
    }

    struct add_eventout_value_ : std::unary_function<node_interface, void> {
        add_eventout_value_(ProtoNode::EventOutValueMap & eventOutValueMap):
                eventOutValueMap(&eventOutValueMap) {}

        void operator()(const node_interface & interface) const {
            if (interface.type == node_interface::eventout_id) {
                const node::polled_eventout_value
                    eventOutValue(default_field_value(interface.field_type),
                                  0.0);
                const ProtoNode::EventOutValueMap::value_type
                        value(interface.id, eventOutValue);
                const bool succeeded =
                        this->eventOutValueMap->insert(value).second;
                assert(succeeded);
            } else if (interface.type == node_interface::exposedfield_id) {
                const node::polled_eventout_value
                    eventOutValue(default_field_value(interface.field_type),
                                  0.0);
                const ProtoNode::EventOutValueMap::value_type
                        value(interface.id + "_changed", eventOutValue);
                const bool succeeded =
                        this->eventOutValueMap->insert(value).second;
                assert(succeeded);
            }
        }

    private:
        ProtoNode::EventOutValueMap * eventOutValueMap;
    };
}

/**
 * @brief Constructor.
 *
 * @param nodeType  the NodeType associated with the node.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
ProtoNode::ProtoNode(const node_type & nodeType) throw (std::bad_alloc):
    node(nodeType, scope_ptr(0))
{}

/**
 * @brief Construct a prototype instance.
 *
 * @param nodeType  the type object for the new ProtoNode instance.
 * @param scope     the scope the new node belongs to.
 * @param node      the ProtoNode to clone when creating the instance.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
ProtoNode::ProtoNode(const node_type & nodeType,
                     const scope_ptr & scope,
                     const ProtoNode & n) throw (std::bad_alloc):
    node(nodeType, scope)
{
    assert(!n.implNodes.empty());
    assert(n.implNodes[0]);

    //
    // For each exposedField and eventOut in the prototype interface, add
    // a value to the eventOutValueMap.
    //
    // Note: We don't want to copy node's EventOutValueMap, since the values
    // in that map should be per-instance.
    //
    const node_interface_set & interfaces = this->type.interfaces();
    std::for_each(interfaces.begin(), interfaces.end(),
                  add_eventout_value_(this->eventOutValueMap));

    //
    // Cloning the nodes is a two-step process. First, we clone the actual
    // node instances. Second, we traverse the node tree again cloning the
    // routes.
    //
    class ProtoImplCloner {
        std::set<node *> traversedNodes;
        const ProtoNode * protoDef;
        ProtoNode * protoInstance;

    public:
        ProtoImplCloner():
            protoDef(0),
            protoInstance(0)
        {}

        void clone(const ProtoNode & protoDef,
                   ProtoNode & protoInstance)
            throw (std::bad_alloc)
        {
            assert(this->traversedNodes.empty());
            assert(!this->protoDef);
            assert(!this->protoInstance);

            this->protoDef = &protoDef;
            this->protoInstance = &protoInstance;
            const std::vector<node_ptr> & protoDefImplNodes =
                    protoDef.getImplNodes();
            for (std::vector<node_ptr>::const_iterator node =
                    protoDefImplNodes.begin();
                    node != protoDefImplNodes.end(); ++node) {
                const node_ptr newNode =
                        this->cloneNode(*node, protoInstance.scope());
                assert(newNode);
                protoInstance.addRootNode(newNode);
            }
            this->protoDef = 0;
            this->protoInstance = 0;
            this->traversedNodes.clear();
        }

    private:
        const sfnode cloneFieldValue(const sfnode & node,
                                     const scope_ptr & targetScope)
            throw (std::bad_alloc)
        {
            return sfnode(this->cloneNode(node.value, targetScope));
        }

        const mfnode cloneFieldValue(const mfnode & nodes,
                                     const scope_ptr & targetScope)
            throw (std::bad_alloc)
        {
            mfnode result(nodes.value.size());
            for (size_t i = 0; i < nodes.value.size(); ++i) {
                result.value[i] = this->cloneNode(nodes.value[i], targetScope);
            }
            return result;
        }

        const node_ptr cloneNode(const node_ptr & n,
                                const scope_ptr & targetScope)
            throw (std::bad_alloc)
        {
            assert(targetScope);

            node_ptr result(0);

            if (!n) { return result; }

            std::set<node *>::iterator pos =
                this->traversedNodes.find(n.get());
            const bool alreadyTraversed = (pos != this->traversedNodes.end());

            if (alreadyTraversed) {
                result.reset(targetScope->find_node(n->id()));
                assert(result);
            } else {
                result = n->type.create_node(targetScope);
                if (!n->id().empty()) { result->id(n->id()); }

                //
                // Any IS mappings for this node?
                //
                for (ProtoNode::ISMap::const_iterator isMapEntry =
                        this->protoDef->isMap.begin();
                        isMapEntry != this->protoDef->isMap.end();
                        ++isMapEntry) {
                    if (&isMapEntry->second.node == n.get()) {
                        try {
                            this->protoInstance
                                    ->addIS(*result,
                                            isMapEntry->second.interfaceId,
                                            isMapEntry->first);
                        } catch (std::bad_alloc &) {
                            throw;
                        } catch (std::runtime_error & ex) {
                            OPENVRML_PRINT_EXCEPTION_(ex);
                            assert(false);
                        }
                    }
                }

                const node_interface_set & interfaces = n->type.interfaces();
                for (node_interface_set::const_iterator interface =
                        interfaces.begin();
                        interface != interfaces.end();
                        ++interface) {
                    try {
                        if (interface->type == node_interface::exposedfield_id
                                || interface->type
                                    == node_interface::field_id) {
                            if (interface->field_type
                                    == field_value::sfnode_id) {
                                const sfnode & value =
                                        static_cast<const sfnode &>
                                            (n->field(interface->id));
                                result->field(interface->id,
                                              this->cloneFieldValue(value,
                                                                 targetScope));
                            } else if (interface->field_type
                                    == field_value::mfnode_id) {
                                const mfnode & value =
                                    static_cast<const mfnode &>
                                        (n->field(interface->id));
                                result->field(interface->id,
                                              this->cloneFieldValue(value,
                                                                 targetScope));
                            } else {
                                result->field(interface->id,
                                              n->field(interface->id));
                            }
                        }
                    } catch (std::bad_alloc) {
                        throw;
                    } catch (std::runtime_error & ex) {
                        OPENVRML_PRINT_EXCEPTION_(ex);
                    }
                }
            }
            return result;
        }
    } protoImplCloner;

    protoImplCloner.clone(n, *this);

    class RouteCopyTraverser : public node_traverser {
        const OpenVRML::scope & targetScope;

    public:
        RouteCopyTraverser(const OpenVRML::scope & targetScope):
            targetScope(targetScope)
        {}

        virtual ~RouteCopyTraverser() throw ()
        {}

    private:
        virtual void on_entering(node & n)
        {
            const std::string & fromNodeId = n.id();
            if (!fromNodeId.empty()) {
                node * const fromNode =
                    this->targetScope.find_node(fromNodeId);
                assert(fromNode);
                const node::routes_t & routes = n.routes();
                for (node::routes_t::const_iterator route = routes.begin();
                        route != routes.end(); ++route) {
                    const std::string & toNodeId = route->to_node->id();
                    const node_ptr
                        toNode(this->targetScope.find_node(toNodeId));
                    assert(toNode);
                    fromNode->add_route(route->from_eventout,
                                        toNode,
                                        route->to_eventin);
                }
            }
        }
    };

    RouteCopyTraverser(*scope).traverse(n.getImplNodes());

    //
    // Finally, we initialize the implementation using the PROTO's default
    // field values.
    //
    class NodeFieldCloner {
        std::set<node *> traversedNodes;

    public:
        const sfnode clone(const sfnode & node,
                           const scope_ptr & targetScope)
            throw (std::bad_alloc)
        {
            assert(this->traversedNodes.empty());
            const sfnode result = this->cloneFieldValue(node, targetScope);
            this->traversedNodes.clear();
            return result;
        }

        const mfnode clone(const mfnode & nodes,
                           const scope_ptr & targetScope)
            throw (std::bad_alloc)
        {
            assert(this->traversedNodes.empty());
            const mfnode result = this->cloneFieldValue(nodes, targetScope);
            this->traversedNodes.clear();
            return result;
        }

    private:
        const sfnode cloneFieldValue(const sfnode & node,
                                     const scope_ptr & targetScope)
            throw (std::bad_alloc)
        {
            return sfnode(this->cloneNode(node.value, targetScope));
        }

        const mfnode cloneFieldValue(const mfnode & nodes,
                                     const scope_ptr & targetScope)
            throw (std::bad_alloc)
        {
            mfnode result(nodes.value.size());
            for (size_t i = 0; i < nodes.value.size(); ++i) {
                result.value[i] = this->cloneNode(nodes.value[i], targetScope);
            }
            return result;
        }

        const node_ptr cloneNode(const node_ptr & node,
                                const scope_ptr & targetScope)
            throw (std::bad_alloc)
        {
            assert(targetScope);

            node_ptr result(0);

            if (!node) { return result; }

            std::set<OpenVRML::node *>::iterator pos =
                    this->traversedNodes.find(node.get());
            const bool alreadyTraversed = (pos != this->traversedNodes.end());

            if (alreadyTraversed) {
                result.reset(targetScope->find_node(node->id()));
                assert(result);
            } else {
                result = node->type.create_node(targetScope);
                if (!node->id().empty()) { result->id(node->id()); }

                const node_interface_set & interfaces =
                    node->type.interfaces();
                for (node_interface_set::const_iterator interface =
                        interfaces.begin();
                        interface != interfaces.end();
                        ++interface) {
                    try {
                        if (interface->type == node_interface::exposedfield_id
                                || interface->type
                                    == node_interface::field_id) {
                            if (interface->field_type
                                    == field_value::sfnode_id) {
                                const sfnode & value =
                                    static_cast<const sfnode &>
                                        (node->field(interface->id));
                                result->field(interface->id,
                                              this->cloneFieldValue(value,
                                                                 targetScope));
                            } else if (interface->field_type
                                    == field_value::mfnode_id) {
                                const mfnode & value =
                                    static_cast<const mfnode &>
                                        (node->field(interface->id));
                                result->field(interface->id,
                                              this->cloneFieldValue(value,
                                                                 targetScope));
                            } else {
                                result->field(interface->id,
                                              node->field(interface->id));
                            }
                        }
                    } catch (std::bad_alloc) {
                        throw;
                    } catch (std::runtime_error & ex) {
                        OPENVRML_PRINT_EXCEPTION_(ex);
                    }
                }
            }

            return result;
        }
    } nodeCloner;

    typedef ProtoNodeClass::DefaultValueMap DefaultValueMap;
    DefaultValueMap & defaultValueMap =
        static_cast<ProtoNodeClass &>(nodeType._class).defaultValueMap;
    const scope_ptr & protoScope = this->implNodes[0]->scope();

    for (DefaultValueMap::const_iterator i(defaultValueMap.begin());
            i != defaultValueMap.end(); ++i) {
        //
        // Node field values need to be cloned; everything else we can just
        // copy.
        //
        field_value_ptr fieldValue;
        const field_value::type_id type = i->second->type();
        if (type == field_value::sfnode_id) {
            const sfnode & node = static_cast<const sfnode &>(*i->second);
            fieldValue.reset(new sfnode(nodeCloner.clone(node, protoScope)));
        } else if (type == field_value::mfnode_id) {
            const mfnode & nodes = static_cast<const mfnode &>(*i->second);
            fieldValue.reset(new mfnode(nodeCloner.clone(nodes, protoScope)));
        } else {
            fieldValue = i->second;
        }

        const std::pair<ISMap::iterator, ISMap::iterator> rangeItrs =
                this->isMap.equal_range(i->first);
        for (ISMap::iterator j(rangeItrs.first); j != rangeItrs.second; ++j) {
            j->second.node.field(j->second.interfaceId, *fieldValue);
        }

        //
        // If we have a field that isn't IS'd to anything in the
        // implementation, we need to store it's value specially.
        //
        if (this->type.has_field(i->first)
                && rangeItrs.second == rangeItrs.first) {
            FieldValueMap::value_type value(i->first, fieldValue);
            const bool succeeded =
                    this->unISdFieldValueMap.insert(value).second;
            assert(succeeded);
        }
    }
}

/**
 * @brief Destructor.
 */
ProtoNode::~ProtoNode() throw ()
{}

/**
 * @brief Cast to a script_node.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a script_node, or 0 otherwise.
 */
const script_node * ProtoNode::to_script() const throw ()
{
    assert(!this->implNodes.empty());
    assert(this->implNodes[0]);
    return this->implNodes[0]->to_script();
}

/**
 * @brief Cast to a script_node.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a script_node, or 0 otherwise.
 */
script_node * ProtoNode::to_script() throw ()
{
    assert(!this->implNodes.empty());
    assert(this->implNodes[0]);
    return this->implNodes[0]->to_script();
}

/**
 * @brief Cast to an appearance_node.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      an appearance_node, or 0 otherwise.
 */
const appearance_node * ProtoNode::to_appearance() const throw ()
{
    assert(!this->implNodes.empty());
    assert(this->implNodes[0]);
    return this->implNodes[0]->to_appearance();
}

/**
 * @brief Cast to an appearance_node.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      an appearance_node, or 0 otherwise.
 */
appearance_node * ProtoNode::to_appearance() throw ()
{
    assert(!this->implNodes.empty());
    assert(this->implNodes[0]);
    return this->implNodes[0]->to_appearance();
}

/**
 * @brief Cast to a child_node.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a child_node, or 0 otherwise.
 */
const child_node * ProtoNode::to_child() const throw ()
{
    assert(!this->implNodes.empty());
    assert(this->implNodes[0]);
    return this->implNodes[0]->to_child();
}

/**
 * @brief Cast to a child_node.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a child_node, or 0 otherwise.
 */
child_node * ProtoNode::to_child() throw ()
{
    assert(!this->implNodes.empty());
    assert(this->implNodes[0]);
    return this->implNodes[0]->to_child();
}

/**
 * @brief Cast to a color_node.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a color_node, or 0 otherwise.
 */
const color_node * ProtoNode::to_color() const throw ()
{
    assert(!this->implNodes.empty());
    assert(this->implNodes[0]);
    return this->implNodes[0]->to_color();
}

/**
 * @brief Cast to a color_node.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a color_node, or 0 otherwise.
 */
color_node * ProtoNode::to_color() throw ()
{
    assert(!this->implNodes.empty());
    assert(this->implNodes[0]);
    return this->implNodes[0]->to_color();
}

/**
 * @brief Cast to a coordinate_node.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a coordinate_node, or 0 otherwise.
 */
const coordinate_node * ProtoNode::to_coordinate() const throw ()
{
    assert(!this->implNodes.empty());
    assert(this->implNodes[0]);
    return this->implNodes[0]->to_coordinate();
}

/**
 * @brief Cast to a coordinate_node.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a coordinate_node, or 0 otherwise.
 */
coordinate_node * ProtoNode::to_coordinate() throw ()
{
    assert(!this->implNodes.empty());
    assert(this->implNodes[0]);
    return this->implNodes[0]->to_coordinate();
}

/**
 * @brief Cast to a font_style_node.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a font_style_node, or 0 otherwise.
 */
const font_style_node * ProtoNode::to_font_style() const throw ()
{
    assert(!this->implNodes.empty());
    assert(this->implNodes[0]);
    return this->implNodes[0]->to_font_style();
}

/**
 * @brief Cast to a font_style_node.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a font_style_node, or 0 otherwise.
 */
font_style_node * ProtoNode::to_font_style() throw ()
{
    assert(!this->implNodes.empty());
    assert(this->implNodes[0]);
    return this->implNodes[0]->to_font_style();
}

/**
 * @brief Cast to a geometry_node.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a geometry_node, or 0 otherwise.
 */
const geometry_node * ProtoNode::to_geometry() const throw ()
{
    assert(!this->implNodes.empty());
    assert(this->implNodes[0]);
    return this->implNodes[0]->to_geometry();
}

/**
 * @brief Cast to a geometry_node.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a geometry_node, or 0 otherwise.
 */
geometry_node * ProtoNode::to_geometry() throw ()
{
    assert(!this->implNodes.empty());
    assert(this->implNodes[0]);
    return this->implNodes[0]->to_geometry();
}

/**
 * @brief Cast to a material_node.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a material_node, or 0 otherwise.
 */
const material_node * ProtoNode::to_material() const throw ()
{
    assert(!this->implNodes.empty());
    assert(this->implNodes[0]);
    return this->implNodes[0]->to_material();
}

/**
 * @brief Cast to a material_node.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a material_node, or 0 otherwise.
 */
material_node * ProtoNode::to_material() throw ()
{
    assert(!this->implNodes.empty());
    assert(this->implNodes[0]);
    return this->implNodes[0]->to_material();
}

/**
 * @brief Cast to a normal_node.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a normal_node, or 0 otherwise.
 */
const normal_node * ProtoNode::to_normal() const throw ()
{
    assert(!this->implNodes.empty());
    assert(this->implNodes[0]);
    return this->implNodes[0]->to_normal();
}

/**
 * @brief Cast to a normal_node.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a normal_node, or 0 otherwise.
 */
normal_node * ProtoNode::to_normal() throw ()
{
    assert(!this->implNodes.empty());
    assert(this->implNodes[0]);
    return this->implNodes[0]->to_normal();
}

/**
 * @brief Cast to a sound_source_node.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a sound_source_node, or 0 otherwise.
 */
const sound_source_node * ProtoNode::to_sound_source() const throw ()
{
    assert(!this->implNodes.empty());
    assert(this->implNodes[0]);
    return this->implNodes[0]->to_sound_source();
}

/**
 * @brief Cast to a sound_source_node.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a sound_source_node, or 0 otherwise.
 */
sound_source_node * ProtoNode::to_sound_source() throw ()
{
    assert(!this->implNodes.empty());
    assert(this->implNodes[0]);
    return this->implNodes[0]->to_sound_source();
}

/**
 * @brief Cast to a texture_node.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a texture_node, or 0 otherwise.
 */
const texture_node * ProtoNode::to_texture() const throw ()
{
    assert(!this->implNodes.empty());
    assert(this->implNodes[0]);
    return this->implNodes[0]->to_texture();
}

/**
 * @brief Cast to a texture_node.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a texture_node, or 0 otherwise.
 */
texture_node * ProtoNode::to_texture() throw ()
{
    assert(!this->implNodes.empty());
    assert(this->implNodes[0]);
    return this->implNodes[0]->to_texture();
}

/**
 * @brief Cast to a texture_coordinate_node.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a texture_coordinate_node, or 0 otherwise.
 */
const texture_coordinate_node * ProtoNode::to_texture_coordinate() const throw ()
{
    assert(!this->implNodes.empty());
    assert(this->implNodes[0]);
    return this->implNodes[0]->to_texture_coordinate();
}

/**
 * @brief Cast to a texture_coordinate_node.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a texture_coordinate_node, or 0 otherwise.
 */
texture_coordinate_node * ProtoNode::to_texture_coordinate() throw ()
{
    assert(!this->implNodes.empty());
    assert(this->implNodes[0]);
    return this->implNodes[0]->to_texture_coordinate();
}

/**
 * @brief Cast to a texture_transform_node.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a texture_transform_node, or 0 otherwise.
 */
const texture_transform_node * ProtoNode::to_texture_transform() const throw ()
{
    assert(!this->implNodes.empty());
    assert(this->implNodes[0]);
    return this->implNodes[0]->to_texture_transform();
}

/**
 * @brief Cast to a texture_transform_node.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a texture_transform_node, or 0 otherwise.
 */
texture_transform_node * ProtoNode::to_texture_transform() throw ()
{
    assert(!this->implNodes.empty());
    assert(this->implNodes[0]);
    return this->implNodes[0]->to_texture_transform();
}

/**
 * @brief Cast to a transform_node.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a transform_node, or 0 otherwise.
 */
const transform_node * ProtoNode::to_transform() const throw ()
{
    assert(!this->implNodes.empty());
    assert(this->implNodes[0]);
    return this->implNodes[0]->to_transform();
}

/**
 * @brief Cast to a transform_node.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a transform_node, or 0 otherwise.
 */
transform_node * ProtoNode::to_transform() throw ()
{
    assert(!this->implNodes.empty());
    assert(this->implNodes[0]);
    return this->implNodes[0]->to_transform();
}

/**
 * @brief Cast to a viewpoint_node.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a viewpoint_node, or 0 otherwise.
 */
const viewpoint_node * ProtoNode::to_viewpoint() const throw ()
{
    assert(!this->implNodes.empty());
    assert(this->implNodes[0]);
    return this->implNodes[0]->to_viewpoint();
}

/**
 * @brief Cast to a viewpoint_node.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a viewpoint_node, or 0 otherwise.
 */
viewpoint_node * ProtoNode::to_viewpoint() throw ()
{
    assert(!this->implNodes.empty());
    assert(this->implNodes[0]);
    return this->implNodes[0]->to_viewpoint();
}

/**
 * @brief Get the implementation nodes.
 *
 * @return the implementation Nodes
 */
const std::vector<node_ptr> & ProtoNode::getImplNodes() const throw ()
{
    return this->implNodes;
}

/**
 * @brief Add a root node to the prototype definition.
 */
void ProtoNode::addRootNode(const node_ptr & node) throw (std::bad_alloc)
{
    assert(node);
    this->implNodes.push_back(node);
}

/**
 * @brief Add an IS mapping to the prototype definition.
 *
 * @param implNode              a node in the protoype implementation.
 * @param implNodeInterfaceId   an interface of @p implNode.
 * @param protoInterfaceId      an interface of the prototype.
 *
 * @exception unsupported_interface      if @p implNode has no interface
 *                                      @p implNodeInterfaceId, or if the
 *                                      prototype has no interface
 *                                      @p protoInterfaceId.
 * @exception NodeInterfaceTypeMismatch if the two interface types are
 *                                      incompatible.
 * @exception field_value_type_mismatch    if the field types of the interfaces are
 *                                      not identical.
 * @exception std::bad_alloc            if memory allocation fails.
 *
 * @see http://www.web3d.org/technicalinfo/specifications/vrml97/part1/concepts.html#Table4.4
 *
 * @todo Rewrite this method. Check for interface type, field type, agreement
 *      here and throw exceptions on failure.
 */
void ProtoNode::addIS(node & implNode,
                      const std::string & implNodeInterfaceId,
                      const std::string & protoInterfaceId)
    throw (unsupported_interface, NodeInterfaceTypeMismatch,
           field_value_type_mismatch, std::bad_alloc)
{
    using std::find_if;
    using std::pair;

    //
    // Make sure the IS is legitimate. First, get the interface type of the
    // implementation node's interface.
    //
    const node_interface_set::const_iterator implNodeInterface =
        implNode.type.interfaces().find(implNodeInterfaceId);
    if (implNodeInterface == implNode.type.interfaces().end()) {
        throw unsupported_interface(implNode.type, implNodeInterfaceId);
    }

    //
    // The rhs of the IS mapping must be an *exact* match for one of the
    // PROTO's interfaces; so, we don't use node_interface_set::findInterface.
    //
    const node_interface_set & protoInterfaces = this->type.interfaces();
    const node_interface_set::const_iterator protoInterface =
            find_if(protoInterfaces.begin(), protoInterfaces.end(),
                    interface_id_equals_(protoInterfaceId));
    if (protoInterface == protoInterfaces.end()) {
        throw unsupported_interface(this->type, protoInterfaceId);
    }

    //
    // Make sure the interface types agree.
    //
    if (implNodeInterface->type != node_interface::exposedfield_id
            && implNodeInterface->type != protoInterface->type) {
        throw NodeInterfaceTypeMismatch(implNodeInterface->type,
                                        protoInterface->type);
    }

    //
    // Make sure the field value types agree.
    //
    if (implNodeInterface->field_type != protoInterface->field_type) {
        throw field_value_type_mismatch();
    }

    //
    // Add the IS.
    //
    const ImplNodeInterface implNodeInterfaceRef(implNode, implNodeInterfaceId);
    const ISMap::value_type value(protoInterfaceId, implNodeInterfaceRef);
    this->isMap.insert(value);

    if (protoInterface->type == node_interface::eventout_id) {
        EventOutValueMap::iterator pos =
                this->eventOutValueMap.find(protoInterfaceId);
        if (pos == this->eventOutValueMap.end()) {
            pos = this->eventOutValueMap.find(protoInterfaceId + "_changed");
        }
        assert(pos != this->eventOutValueMap.end());
        implNode.add_eventout_is(implNodeInterfaceId, pos->second);
    }
}

void ProtoNode::update(const double currentTime) {
    //
    // For each modified eventOut, send an event.
    //
    for (EventOutValueMap::iterator itr = eventOutValueMap.begin();
            itr != eventOutValueMap.end(); ++itr) {
        if (itr->second.modified) {
            this->emit_event(itr->first, *itr->second.value, currentTime);
            itr->second.modified = false;
        }
    }
}

/**
 * @brief Initialize.
 *
 * @param timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void ProtoNode::do_initialize(const double timestamp) throw (std::bad_alloc)
{
    assert(this->scene());

    for (size_t i = 0; i < this->implNodes.size(); ++i) {
        assert(this->implNodes[i]);
        this->implNodes[i]->initialize(*this->scene(), timestamp);
    }

    this->scene()->browser.add_proto(*this);
}

void ProtoNode::do_field(const std::string & id, const field_value & value)
    throw (unsupported_interface, std::bad_cast, std::bad_alloc)
{
    const std::pair<ISMap::iterator, ISMap::iterator> rangeItrs =
            this->isMap.equal_range(id);
    for (ISMap::iterator itr(rangeItrs.first);
         itr != rangeItrs.second;
         ++itr) {
        itr->second.node.field(itr->second.interfaceId, value);
    }

    if (rangeItrs.second == rangeItrs.first) {
        //
        // The field hasn't been IS'd to anything.  If it's an exposedField,
        // the value is in eventOutValueMap; if it's a field, the value is in
        // unISdFieldValueMap.
        //
        const node_interface_set & interfaces = this->type.interfaces();
        const node_interface_set::const_iterator interface =
            interfaces.find(id);
        if (interface == interfaces.end()) {
            throw unsupported_interface(this->type,
                                        node_interface::field_id,
                                        id);
        }
        if (interface->type == node_interface::exposedfield_id) {
            assert(this->eventOutValueMap.find(id + "_changed")
                    != this->eventOutValueMap.end());
            this->eventOutValueMap[id + "_changed"].value->assign(value);
        } else if (interface->type == node_interface::field_id) {
            assert(this->unISdFieldValueMap.find(id)
                    != this->unISdFieldValueMap.end());
            this->unISdFieldValueMap[id]->assign(value);
        } else {
            throw unsupported_interface(this->type,
                                        node_interface::field_id,
                                        id);
        }
    }
}

const field_value & ProtoNode::do_field(const std::string & id) const
    throw (unsupported_interface)
{
    //
    // This is a little wierd: what should getField mean for a PROTO-based
    // node? We pick the first node in the IS-map and call getField on it.
    //
    const ISMap::const_iterator pos = this->isMap.find(id);
    if (pos == this->isMap.end()) {
        //
        // The field may exist for the node, but not be IS'd to anything.
        //
        // There are two possibilities...
        //
        if (this->type.has_exposedfield(id)) {
            //
            // If the field is really an exposedField then we need to grab the
            // value from the eventOutValueMap.
            //
            const EventOutValueMap::const_iterator pos =
                    this->eventOutValueMap.find(id + "_changed");
            assert(pos != this->eventOutValueMap.end());
            assert(pos->second.value);
            return *pos->second.value;
        } else {
            //
            // Otherwise, if we are dealing with a non-exposed field...
            //
            const FieldValueMap::const_iterator pos =
                    this->unISdFieldValueMap.find(id);
            if (pos == this->unISdFieldValueMap.end()) {
                throw unsupported_interface(this->type.id
                                            + " node has no field \"" + id
                                            + "\".");
            }
            assert(pos->second);
            return *pos->second;
        }
    }
    return pos->second.node.field(pos->second.interfaceId);
}

namespace {

    /**
     * @internal
     */
    struct DispatchEvent_ :
            std::unary_function<ProtoNode::ISMap::value_type, void> {
        DispatchEvent_(const field_value & value, const double timestamp):
            value(&value),
            timestamp(timestamp)
        {}

        void operator()(const ProtoNode::ISMap::value_type & value) const
        {
            value.second.node.process_event(value.second.interfaceId,
                                            *this->value,
                                            this->timestamp);
        }

    private:
        const field_value * value;
        double timestamp;
    };
}

void ProtoNode::do_process_event(const std::string & id,
                                 const field_value & value,
                                 const double timestamp)
    throw (unsupported_interface, std::bad_cast, std::bad_alloc)
{
    const std::pair<ISMap::iterator, ISMap::iterator> rangeItrs =
            this->isMap.equal_range(id);
    if (rangeItrs.first == this->isMap.end()) {
        throw unsupported_interface(this->type,
                                    node_interface::eventin_id,
                                    id);
    }
    std::for_each(rangeItrs.first, rangeItrs.second,
                  DispatchEvent_(value, timestamp));

    //
    // Emit events.
    //
    for (EventOutValueMap::iterator itr(this->eventOutValueMap.begin());
            itr != this->eventOutValueMap.end(); ++itr) {
        if (itr->second.modified) {
            this->emit_event(itr->first, *itr->second.value, timestamp);
            itr->second.modified = false;
        }
    }
}

const field_value & ProtoNode::do_eventout(const std::string & id) const
        throw (unsupported_interface) {
    //
    // If we have a real eventOut (not an exposedField) ...
    //
    {
        const EventOutValueMap::const_iterator pos =
                this->eventOutValueMap.find(id);
        if (pos != this->eventOutValueMap.end()) { return *pos->second.value; }
    }

    //
    // If the above code doesn't find anything, see if we have an exposedField.
    //
    // XXX I don't think this covers the case where more than one exposedField
    // XXX in the implementation is IS'd to the same exposedField in the
    // XXX interface.
    //
    {
        const ISMap::const_iterator pos = this->isMap.find(id);
        if (pos == this->isMap.end()) {
            throw unsupported_interface(this->type.id + " node has no "
                                        + "eventOut \"" + id + "\".");
        }
        return pos->second.node.eventout(id);
    }
}

/**
 * @brief Shut down.
 *
 * @param timestamp the current time.
 */
void ProtoNode::do_shutdown(const double timestamp) throw ()
{
    assert(this->scene());
    this->scene()->browser.remove_proto(*this);

    for (size_t i = 0; i < this->implNodes.size(); ++i) {
        assert(this->implNodes[i]);
        this->implNodes[i]->shutdown(timestamp);
    }
}

Vrml97Node::Anchor * ProtoNode::to_anchor() const
{
    return this->implNodes[0]->to_anchor();
}

Vrml97Node::AudioClip * ProtoNode::to_audio_clip() const
{
    return this->implNodes[0]->to_audio_clip();
}

Vrml97Node::CylinderSensor * ProtoNode::to_cylinder_sensor() const
{
    return this->implNodes[0]->to_cylinder_sensor();
}

Vrml97Node::AbstractLight * ProtoNode::to_light() const
{
    return this->implNodes[0]->to_light();
}

Vrml97Node::MovieTexture * ProtoNode::to_movie_texture() const
{
    return this->implNodes[0]->to_movie_texture();
}

Vrml97Node::NavigationInfo * ProtoNode::to_navigation_info() const
{
    return this->implNodes[0]->to_navigation_info();
}

Vrml97Node::PlaneSensor * ProtoNode::to_plane_sensor() const
{
    return this->implNodes[0]->to_plane_sensor();
}

Vrml97Node::PointLight * ProtoNode::to_point_light() const
{
    return this->implNodes[0]->to_point_light();
}

Vrml97Node::SphereSensor * ProtoNode::to_sphere_sensor() const
{
    return this->implNodes[0]->to_sphere_sensor();
}

Vrml97Node::SpotLight * ProtoNode::to_spot_light() const
{
    return this->implNodes[0]->to_spot_light();
}

Vrml97Node::TimeSensor * ProtoNode::to_time_sensor() const
{
    return this->implNodes[0]->to_time_sensor();
}

Vrml97Node::TouchSensor * ProtoNode::to_touch_sensor() const
{
    return this->implNodes[0]->to_touch_sensor();
}

void ProtoNode::render(OpenVRML::viewer & viewer,
                       const VrmlRenderContext context)
{
    assert(this->implNodes[0]);
    this->implNodes[0]->render(viewer, context);
}


/**
 * @internal
 *
 * @class ProtoNodeClass
 *
 * @brief Class object for ProtoNode instances.
 *
 * ProtoNodeClass instances are created and initialized when a PROTO definition
 * is read by the parser. Initialization consists of various calls to
 * addEventIn, addEventOut, addExposedField, addField, addRootNode, and addIS,
 * after which the ProtoNodeClass instance is added to the implementation
 * repository. Once the browser has started running, it is not appropriate to
 * call those methods.
 */

/**
 * @internal
 *
 * @class ProtoNodeClass::ProtoNodeType
 *
 * @brief Type information object for ProtoNode instances.
 */

/**
 * @var ProtoNodeClass::ProtoNodeType::nodeInterfaces
 *
 * @brief The list of interfaces supported by a node of this type.
 */

/**
 * @brief Constructor.
 */
ProtoNodeClass::ProtoNodeType::ProtoNodeType(ProtoNodeClass & nodeClass,
                                             const std::string & id)
    throw (unsupported_interface, std::bad_alloc):
    node_type(nodeClass, id)
{}

/**
 * @brief Destructor.
 */
ProtoNodeClass::ProtoNodeType::~ProtoNodeType() throw ()
{}

/**
 * @brief Get the list of interfaces.
 *
 * @return the list of interfaces for nodes of this type.
 */
const node_interface_set & ProtoNodeClass::ProtoNodeType::interfaces() const
    throw ()
{
    return this->nodeInterfaces;
}

/**
 * @brief Create a Node of this type.
 *
 * @return a node_ptr to a new Node.
 */
const node_ptr
ProtoNodeClass::ProtoNodeType::create_node(const scope_ptr & scope) const
    throw (std::bad_alloc)
{
    return node_ptr(new ProtoNode(*this, scope,
                                 static_cast<ProtoNodeClass &>(this->_class)
                                    .protoNode));
}

/**
 * @brief Add an interface.
 */
void
ProtoNodeClass::ProtoNodeType::addInterface(const node_interface & interface)
    throw (std::invalid_argument, std::bad_alloc)
{
    this->nodeInterfaces.add(interface);
}

/**
 * @var ProtoNodeClass::ProtoNodeType ProtoNodeClass::protoNodeType
 *
 * @brief This NodeType includes the full set of
 *      @link node_interface node_interfaces@endlink supported by the
 *      ProtoNodeClass.
 *
 * A ProtoNodeType instance is used for this instead of a node_interface_set as
 * a matter of convenience: we need to give @a protoNode a NodeType object.
 */

/**
 * @var std::map<std::string, field_value_ptr> ProtoNodeClass::defaultValueMap
 *
 * @brief A map containing the default values for fields and exposedFields for
 *      the PROTO.
 */

/**
 * @var ProtoNode ProtoNodeClass::protoNode
 *
 * @brief The prototype object. New nodes are created by copying this object.
 */

/**
 * @brief Constructor.
 *
 * @param browser the browser associated with this node_class.
 */
ProtoNodeClass::ProtoNodeClass(OpenVRML::browser & browser) throw ():
    node_class(browser),
    protoNodeType(*this, ""),
    protoNode(protoNodeType)
{}

/**
 * @brief Destructor.
 */
ProtoNodeClass::~ProtoNodeClass() throw ()
{}

/**
 * @brief Add an eventIn.
 *
 * @param type  the data type for the eventIn.
 * @param id    the name of the eventIn.
 *
 * @exception std::invalid_argument if an interface named @p id is already
 *                                  defined for the prototype.
 * @exception std::bad_alloc        if memory allocation fails.
 */
void ProtoNodeClass::addEventIn(const field_value::type_id type,
                                const std::string & id)
        throw (std::invalid_argument, std::bad_alloc) {
    const node_interface interface(node_interface::eventin_id, type, id);
    this->protoNodeType.addInterface(interface);
}

/**
 * @brief Add an eventOut.
 *
 * @param type  the data type for the eventOut.
 * @param id    the name of the eventOut.
 *
 * @exception std::invalid_argument if an interface named @p id is already
 *                                  defined for the prototype.
 * @exception std::bad_alloc        if memory allocation fails.
 */
void ProtoNodeClass::addEventOut(const field_value::type_id type,
                                 const std::string & id)
        throw (std::invalid_argument, std::bad_alloc) {
    const node_interface interface(node_interface::eventout_id, type, id);
    this->protoNodeType.addInterface(interface);

    //
    // Add a value to the ProtoNode's eventOutValueMap.
    //
    const node::polled_eventout_value
            eventOutValue(default_field_value(interface.field_type), false);
    const ProtoNode::EventOutValueMap::value_type
            value(interface.id, eventOutValue);
    const bool succeeded =
            this->protoNode.eventOutValueMap.insert(value).second;
    assert(succeeded);
}

/**
 * @brief Add an exposedField.
 *
 * @param id            the name of the exposedField.
 * @param defaultValue  the default value for the exposedField.
 *
 * @exception std::invalid_argument if an interface named @p id is already
 *                                  defined for the prototype.
 * @exception std::bad_alloc        if memory allocation fails.
 */
void ProtoNodeClass::addExposedField(const std::string & id,
                                     const field_value_ptr & defaultValue)
        throw (std::invalid_argument, std::bad_alloc) {
    const node_interface
        interface(node_interface::exposedfield_id, defaultValue->type(), id);
    this->protoNodeType.addInterface(interface);
    {
        const DefaultValueMap::value_type value(id, defaultValue);
        const bool succeeded = this->defaultValueMap.insert(value).second;
        assert(succeeded);
    }

    //
    // Add a value to the ProtoNode's eventOutValueMap.
    //
    {
        const node::polled_eventout_value
                eventOutValue(default_field_value(interface.field_type), 0.0);
        const ProtoNode::EventOutValueMap::value_type
                value(interface.id + "_changed", eventOutValue);
        const bool succeeded =
                this->protoNode.eventOutValueMap.insert(value).second;
        assert(succeeded);
    }
}

/**
 * @brief Add a field.
 *
 * @param id            the name of the field.
 * @param defaultValue  the default value for the field.
 *
 * @exception std::invalid_argument if an interface named @p id is already
 *                                  defined for the prototype.
 * @exception std::bad_alloc        if memory allocation fails.
 */
void ProtoNodeClass::addField(const std::string & id,
                              const field_value_ptr & defaultValue)
    throw (std::invalid_argument, std::bad_alloc)
{
    const node_interface
            interface(node_interface::field_id, defaultValue->type(), id);
    this->protoNodeType.addInterface(interface);
    const DefaultValueMap::value_type value(id, defaultValue);
    const bool succeeded = this->defaultValueMap.insert(value).second;
    assert(succeeded);
}

/**
 * @brief Add a root node to the prototype definition.
 *
 * @param node  a node_ptr to a non-NULL node.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void ProtoNodeClass::addRootNode(const node_ptr & node) throw (std::bad_alloc) {
    this->protoNode.addRootNode(node);
}

/**
 * @brief Add an IS mapping to the prototype definition.
 *
 * @param implNode              a node in the protoype implementation.
 * @param implNodeInterfaceId   an interface of @p implNode.
 * @param protoInterfaceId      an interface of the prototype.
 *
 * @exception unsupported_interface     if @p implNode has no interface
 *                                      @p implNodeInterfaceId, or if the
 *                                      prototype has no interface
 *                                      @p protoInterfaceId.
 * @exception NodeInterfaceTypeMismatch if the two interface types are
 *                                      incompatible.
 * @exception field_value_type_mismatch if the field types of the interfaces
 *                                      are not identical.
 * @exception std::bad_alloc            if memory allocation fails.
 */
void ProtoNodeClass::addIS(node & implNode,
                           const std::string & implNodeInterfaceId,
                           const std::string & protoInterfaceId)
    throw (unsupported_interface, NodeInterfaceTypeMismatch,
           field_value_type_mismatch, std::bad_alloc)
{
    this->protoNode.addIS(implNode, implNodeInterfaceId, protoInterfaceId);
}

namespace {
    struct AddInterface_ : std::unary_function<node_interface, void> {
        AddInterface_(ProtoNodeClass::ProtoNodeType & protoNodeType):
                protoNodeType(&protoNodeType) {}

        void operator()(const node_interface & interface) const {
            protoNodeType->addInterface(interface);
        }

    private:
        ProtoNodeClass::ProtoNodeType * protoNodeType;
    };
}
/**
 * @brief Create a new NodeType.
 */
const node_type_ptr
ProtoNodeClass::create_type(const std::string & id,
                            const node_interface_set & interfaces)
    throw (unsupported_interface, std::bad_alloc)
{
    const node_type_ptr nodeType(new ProtoNodeType(*this, id));
    try {
        std::for_each(interfaces.begin(), interfaces.end(),
                      AddInterface_(static_cast<ProtoNodeType &>(*nodeType)));
    } catch (unsupported_interface &) {
        throw;
    } catch (std::invalid_argument & ex) {
        throw unsupported_interface(ex.what());
    }
    return nodeType;
}


/**
 * @internal
 *
 * @class Vrml97RootScope
 *
 * @brief Root namespace for VRML97 browsers.
 *
 * Vrml97RootScope is initialized with the VRML97 node types.
 */

namespace {
    class AddInterfaceToSet_ : std::unary_function<node_interface, void> {
        node_interface_set & nodeInterfaceSet;

    public:
        AddInterfaceToSet_(node_interface_set & nodeInterfaceSet):
                nodeInterfaceSet(nodeInterfaceSet) {}

        void operator()(const node_interface & nodeInterface) const
                throw (std::invalid_argument, std::bad_alloc) {
            this->nodeInterfaceSet.add(nodeInterface);
        }
    };

    /**
     * @internal
     */
    class vrml97_node_interface_set_ : public node_interface_set {
    public:
        vrml97_node_interface_set_(const node_interface * const begin,
                                const node_interface * const end) {
            std::for_each(begin, end, AddInterfaceToSet_(*this));
        }
    };
}

/**
 * @brief Constructor.
 *
 * @param browser   the browser object.
 * @param uri       the URI associated with the stream being read.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
Vrml97RootScope::Vrml97RootScope(const browser & browser,
                                 const std::string & uri)
    throw (std::bad_alloc):
    scope(uri)
{
    const browser::node_class_map_t & nodeClassMap = browser.node_class_map;
    browser::node_class_map_t::const_iterator pos;

    //
    // Anchor node
    //
    static const node_interface anchorInterfaces[] = {
        node_interface(node_interface::eventin_id,
                       field_value::mfnode_id,
                       "addChildren"),
        node_interface(node_interface::eventin_id,
                       field_value::mfnode_id,
                       "removeChildren"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfnode_id,
                       "children"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfstring_id,
                       "description"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfstring_id,
                       "parameter"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfstring_id,
                       "url"),
        node_interface(node_interface::field_id,
                       field_value::sfvec3f_id,
                       "bboxCenter"),
        node_interface(node_interface::field_id,
                       field_value::sfvec3f_id,
                       "bboxSize")
    };
    static const vrml97_node_interface_set_
            anchorInterfaceSet(anchorInterfaces, anchorInterfaces + 8);
    pos = nodeClassMap.find("urn:X-openvrml:node:Anchor");
    assert(pos != nodeClassMap.end());
    this->add_type(pos->second->create_type("Anchor", anchorInterfaceSet));

    //
    // Appearance node
    //
    static const node_interface appearanceInterfaces[] = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "material"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "texture"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "textureTransform")
    };
    static const vrml97_node_interface_set_
            appearanceInterfaceSet(appearanceInterfaces,
                                   appearanceInterfaces + 3);
    pos = nodeClassMap.find("urn:X-openvrml:node:Appearance");
    assert(pos != nodeClassMap.end());
    this->add_type(pos->second->create_type("Appearance",
                                            appearanceInterfaceSet));

    //
    // AudioClip node
    //
    static const node_interface audioClipInterfaces[] = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sfstring_id,
                       "description"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfbool_id,
                       "loop"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sffloat_id,
                       "pitch"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sftime_id,
                       "startTime"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sftime_id,
                       "stopTime"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfstring_id,
                       "url"),
        node_interface(node_interface::eventout_id,
                       field_value::sftime_id,
                       "duration_changed"),
        node_interface(node_interface::eventout_id,
                       field_value::sfbool_id,
                       "isActive")
    };
    static const vrml97_node_interface_set_
            audioClipInterfaceSet(audioClipInterfaces,
                                  audioClipInterfaces + 8);
    pos = nodeClassMap.find("urn:X-openvrml:node:AudioClip");
    assert(pos != nodeClassMap.end());
    this->add_type(pos->second->create_type("AudioClip",
                                            audioClipInterfaceSet));

    //
    // Background node
    //
    static const node_interface backgroundInterfaces[] = {
        node_interface(node_interface::eventin_id, field_value::sfbool_id, "set_bind"),
        node_interface(node_interface::exposedfield_id, field_value::mffloat_id, "groundAngle"),
        node_interface(node_interface::exposedfield_id, field_value::mfcolor_id, "groundColor"),
        node_interface(node_interface::exposedfield_id, field_value::mfstring_id, "backUrl"),
        node_interface(node_interface::exposedfield_id, field_value::mfstring_id, "bottomUrl"),
        node_interface(node_interface::exposedfield_id, field_value::mfstring_id, "frontUrl"),
        node_interface(node_interface::exposedfield_id, field_value::mfstring_id, "leftUrl"),
        node_interface(node_interface::exposedfield_id, field_value::mfstring_id, "rightUrl"),
        node_interface(node_interface::exposedfield_id, field_value::mfstring_id, "topUrl"),
        node_interface(node_interface::exposedfield_id, field_value::mffloat_id, "skyAngle"),
        node_interface(node_interface::exposedfield_id, field_value::mfcolor_id, "skyColor"),
        node_interface(node_interface::eventout_id, field_value::sfbool_id, "isBound")
    };
    static const vrml97_node_interface_set_
            backgroundInterfaceSet(backgroundInterfaces,
                                  backgroundInterfaces + 12);
    pos = nodeClassMap.find("urn:X-openvrml:node:Background");
    assert(pos != nodeClassMap.end());
    this->add_type(pos->second->create_type("Background",
                                              backgroundInterfaceSet));

    //
    // Billboard node
    //
    static const node_interface billboardInterfaces[] = {
        node_interface(node_interface::eventin_id, field_value::mfnode_id, "addChildren"),
        node_interface(node_interface::eventin_id, field_value::mfnode_id, "removeChildren"),
        node_interface(node_interface::exposedfield_id, field_value::sfvec3f_id, "axisOfRotation"),
        node_interface(node_interface::exposedfield_id, field_value::mfnode_id, "children"),
        node_interface(node_interface::field_id, field_value::sfvec3f_id, "bboxCenter"),
        node_interface(node_interface::field_id, field_value::sfvec3f_id, "bboxSize")
    };
    static const vrml97_node_interface_set_
            billboardInterfaceSet(billboardInterfaces,
                                  billboardInterfaces + 6);
    pos = nodeClassMap.find("urn:X-openvrml:node:Billboard");
    assert(pos != nodeClassMap.end());
    this->add_type(pos->second->create_type("Billboard",
                                              billboardInterfaceSet));

    //
    // Box node
    //
    static const node_interface boxInterface =
            node_interface(node_interface::field_id, field_value::sfvec3f_id, "size");
    static const vrml97_node_interface_set_
            boxInterfaceSet(&boxInterface, &boxInterface + 1);
    pos = nodeClassMap.find("urn:X-openvrml:node:Box");
    assert(pos != nodeClassMap.end());
    this->add_type(pos->second->create_type("Box", boxInterfaceSet));

    //
    // Collision node
    //
    static const node_interface collisionInterfaces[] = {
        node_interface(node_interface::eventin_id, field_value::mfnode_id, "addChildren"),
        node_interface(node_interface::eventin_id, field_value::mfnode_id, "removeChildren"),
        node_interface(node_interface::exposedfield_id, field_value::mfnode_id, "children"),
        node_interface(node_interface::exposedfield_id, field_value::sfbool_id, "collide"),
        node_interface(node_interface::field_id, field_value::sfvec3f_id, "bboxCenter"),
        node_interface(node_interface::field_id, field_value::sfvec3f_id, "bboxSize"),
        node_interface(node_interface::field_id, field_value::sfnode_id, "proxy"),
        node_interface(node_interface::eventout_id, field_value::sftime_id, "collideTime")
    };
    static const vrml97_node_interface_set_
        collisionInterfaceSet(collisionInterfaces, collisionInterfaces + 8);
    pos = nodeClassMap.find("urn:X-openvrml:node:Collision");
    assert(pos != nodeClassMap.end());
    this->add_type(pos->second->create_type("Collision",
                                              collisionInterfaceSet));

    //
    // Color node
    //
    static const node_interface colorInterface =
        node_interface(node_interface::exposedfield_id,
                      field_value::mfcolor_id,
                      "color");
    static const vrml97_node_interface_set_
            colorInterfaceSet(&colorInterface, &colorInterface + 1);
    pos = nodeClassMap.find("urn:X-openvrml:node:Color");
    assert(pos != nodeClassMap.end());
    this->add_type(pos->second->create_type("Color", colorInterfaceSet));

    //
    // ColorInterpolator node
    //
    static const node_interface colorInterpolatorInterfaces[] = {
        node_interface(node_interface::eventin_id,
                      field_value::sffloat_id,
                      "set_fraction"),
        node_interface(node_interface::exposedfield_id,
                      field_value::mffloat_id,
                      "key"),
        node_interface(node_interface::exposedfield_id,
                      field_value::mfcolor_id,
                      "keyValue"),
        node_interface(node_interface::eventout_id,
                      field_value::sfcolor_id,
                      "value_changed")
    };
    static const vrml97_node_interface_set_
            colorInterpolatorInterfaceSet(colorInterpolatorInterfaces,
                                          colorInterpolatorInterfaces + 4);
    pos = nodeClassMap.find("urn:X-openvrml:node:ColorInterpolator");
    assert(pos != nodeClassMap.end());
    this->add_type(pos->second->create_type("ColorInterpolator",
                                              colorInterpolatorInterfaceSet));

    //
    // Cone node
    //
    static const node_interface coneInterfaces[] = {
        node_interface(node_interface::field_id, field_value::sffloat_id, "bottomRadius"),
        node_interface(node_interface::field_id, field_value::sffloat_id, "height"),
        node_interface(node_interface::field_id, field_value::sfbool_id, "side"),
        node_interface(node_interface::field_id, field_value::sfbool_id, "bottom")
    };
    static const vrml97_node_interface_set_
            coneInterfaceSet(coneInterfaces, coneInterfaces + 4);
    pos = nodeClassMap.find("urn:X-openvrml:node:Cone");
    assert(pos != nodeClassMap.end());
    this->add_type(pos->second->create_type("Cone", coneInterfaceSet));

    //
    // Coordinate node
    //
    static const node_interface coordinateInterface =
            node_interface(node_interface::exposedfield_id, field_value::mfvec3f_id, "point");
    static const vrml97_node_interface_set_
            coordinateInterfaceSet(&coordinateInterface,
                                   &coordinateInterface + 1);
    pos = nodeClassMap.find("urn:X-openvrml:node:Coordinate");
    assert(pos != nodeClassMap.end());
    this->add_type(pos->second->create_type("Coordinate",
                                              coordinateInterfaceSet));

    //
    // CoordinateInterpolator node
    //
    static const node_interface coordinateInterpolatorInterfaces[] = {
        node_interface(node_interface::eventin_id, field_value::sffloat_id, "set_fraction"),
        node_interface(node_interface::exposedfield_id, field_value::mffloat_id, "key"),
        node_interface(node_interface::exposedfield_id, field_value::mfvec3f_id, "keyValue"),
        node_interface(node_interface::eventout_id, field_value::mfvec3f_id, "value_changed")
    };
    static const vrml97_node_interface_set_
            coordinateInterpolatorInterfaceSet(coordinateInterpolatorInterfaces,
                                               coordinateInterpolatorInterfaces + 4);
    pos = nodeClassMap.find("urn:X-openvrml:node:CoordinateInterpolator");
    assert(pos != nodeClassMap.end());
    this->add_type(pos->second->create_type("CoordinateInterpolator",
                                              coordinateInterpolatorInterfaceSet));

    //
    // Cylinder node
    //
    static const node_interface cylinderInterfaces[] = {
        node_interface(node_interface::field_id, field_value::sfbool_id, "bottom"),
        node_interface(node_interface::field_id, field_value::sffloat_id, "height"),
        node_interface(node_interface::field_id, field_value::sffloat_id, "radius"),
        node_interface(node_interface::field_id, field_value::sfbool_id, "side"),
        node_interface(node_interface::field_id, field_value::sfbool_id, "top")
    };
    static const vrml97_node_interface_set_
            cylinderInterfaceSet(cylinderInterfaces, cylinderInterfaces + 5);
    pos = nodeClassMap.find("urn:X-openvrml:node:Cylinder");
    assert(pos != nodeClassMap.end());
    this->add_type(pos->second->create_type("Cylinder",
                                              cylinderInterfaceSet));

    //
    // CylinderSensor node
    //
    static const node_interface cylinderSensorInterfaces[] = {
        node_interface(node_interface::exposedfield_id, field_value::sfbool_id, "autoOffset"),
        node_interface(node_interface::exposedfield_id, field_value::sffloat_id, "diskAngle"),
        node_interface(node_interface::exposedfield_id, field_value::sfbool_id, "enabled"),
        node_interface(node_interface::exposedfield_id, field_value::sffloat_id, "maxAngle"),
        node_interface(node_interface::exposedfield_id, field_value::sffloat_id, "minAngle"),
        node_interface(node_interface::exposedfield_id, field_value::sffloat_id, "offset"),
        node_interface(node_interface::eventout_id, field_value::sfbool_id, "isActive"),
        node_interface(node_interface::eventout_id, field_value::sfrotation_id, "rotation_changed"),
        node_interface(node_interface::eventout_id, field_value::sfvec3f_id, "trackPoint_changed")
    };
    static const vrml97_node_interface_set_
            cylinderSensorInterfaceSet(cylinderSensorInterfaces,
                                       cylinderSensorInterfaces + 9);
    pos = nodeClassMap.find("urn:X-openvrml:node:CylinderSensor");
    assert(pos != nodeClassMap.end());
    this->add_type(pos->second->create_type("CylinderSensor",
                                              cylinderSensorInterfaceSet));

    //
    // DirectionalLight node
    //
    static const node_interface directionalLightInterfaces[] = {
        node_interface(node_interface::exposedfield_id, field_value::sffloat_id, "ambientIntensity"),
        node_interface(node_interface::exposedfield_id, field_value::sfcolor_id, "color"),
        node_interface(node_interface::exposedfield_id, field_value::sfvec3f_id, "direction"),
        node_interface(node_interface::exposedfield_id, field_value::sffloat_id, "intensity"),
        node_interface(node_interface::exposedfield_id, field_value::sfbool_id, "on")
    };
    static const vrml97_node_interface_set_
            directionalLightInterfaceSet(directionalLightInterfaces,
                                         directionalLightInterfaces + 5);
    pos = nodeClassMap.find("urn:X-openvrml:node:DirectionalLight");
    assert(pos != nodeClassMap.end());
    this->add_type(pos->second->create_type("DirectionalLight",
                                              directionalLightInterfaceSet));

    //
    // ElevationGrid node
    //
    static const node_interface elevationGridInterfaces[] = {
        node_interface(node_interface::eventin_id, field_value::mffloat_id, "set_height"),
        node_interface(node_interface::exposedfield_id, field_value::sfnode_id, "color"),
        node_interface(node_interface::exposedfield_id, field_value::sfnode_id, "normal"),
        node_interface(node_interface::exposedfield_id, field_value::sfnode_id, "texCoord"),
        node_interface(node_interface::field_id, field_value::mffloat_id, "height"),
        node_interface(node_interface::field_id, field_value::sfbool_id, "ccw"),
        node_interface(node_interface::field_id, field_value::sfbool_id, "colorPerVertex"),
        node_interface(node_interface::field_id, field_value::sffloat_id, "creaseAngle"),
        node_interface(node_interface::field_id, field_value::sfbool_id, "normalPerVertex"),
        node_interface(node_interface::field_id, field_value::sfbool_id, "solid"),
        node_interface(node_interface::field_id, field_value::sfint32_id, "xDimension"),
        node_interface(node_interface::field_id, field_value::sffloat_id, "xSpacing"),
        node_interface(node_interface::field_id, field_value::sfint32_id, "zDimension"),
        node_interface(node_interface::field_id, field_value::sffloat_id, "zSpacing")
    };
    static const vrml97_node_interface_set_
            elevationGridInterfaceSet(elevationGridInterfaces,
                                         elevationGridInterfaces + 14);
    pos = nodeClassMap.find("urn:X-openvrml:node:ElevationGrid");
    assert(pos != nodeClassMap.end());
    this->add_type(pos->second->create_type("ElevationGrid",
                                              elevationGridInterfaceSet));

    //
    // Extrusion node
    //
    {
        static const node_interface nodeInterfaces[] = {
            node_interface(node_interface::eventin_id, field_value::mfvec2f_id, "set_crossSection"),
            node_interface(node_interface::eventin_id, field_value::mfrotation_id, "set_orientation"),
            node_interface(node_interface::eventin_id, field_value::mfvec2f_id, "set_scale"),
            node_interface(node_interface::eventin_id, field_value::mfvec3f_id, "set_spine"),
            node_interface(node_interface::field_id, field_value::sfbool_id, "beginCap"),
            node_interface(node_interface::field_id, field_value::sfbool_id, "ccw"),
            node_interface(node_interface::field_id, field_value::sfbool_id, "convex"),
            node_interface(node_interface::field_id, field_value::sffloat_id, "creaseAngle"),
            node_interface(node_interface::field_id, field_value::mfvec2f_id, "crossSection"),
            node_interface(node_interface::field_id, field_value::sfbool_id, "endCap"),
            node_interface(node_interface::field_id, field_value::mfrotation_id, "orientation"),
            node_interface(node_interface::field_id, field_value::mfvec2f_id, "scale"),
            node_interface(node_interface::field_id, field_value::sfbool_id, "solid"),
            node_interface(node_interface::field_id, field_value::mfvec3f_id, "spine")
        };
        static const vrml97_node_interface_set_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 14);
        pos = nodeClassMap.find("urn:X-openvrml:node:Extrusion");
        assert(pos != nodeClassMap.end());
        this->add_type(pos->second->create_type("Extrusion",
                                                  nodeInterfaceSet));
    }

    //
    // Fog node
    //
    {
        static const node_interface nodeInterfaces[] = {
            node_interface(node_interface::eventin_id, field_value::sfbool_id, "set_bind"),
            node_interface(node_interface::exposedfield_id, field_value::sfcolor_id, "color"),
            node_interface(node_interface::exposedfield_id, field_value::sfstring_id, "fogType"),
            node_interface(node_interface::exposedfield_id, field_value::sffloat_id, "visibilityRange"),
            node_interface(node_interface::eventout_id, field_value::sfbool_id, "isBound")
        };
        static const vrml97_node_interface_set_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 5);
        pos = nodeClassMap.find("urn:X-openvrml:node:Fog");
        assert(pos != nodeClassMap.end());
        this->add_type(pos->second->create_type("Fog", nodeInterfaceSet));
    }

    //
    // FontStyle node
    //
    {
        static const node_interface nodeInterfaces[] = {
            node_interface(node_interface::field_id, field_value::mfstring_id, "family"),
            node_interface(node_interface::field_id, field_value::sfbool_id, "horizontal"),
            node_interface(node_interface::field_id, field_value::mfstring_id, "justify"),
            node_interface(node_interface::field_id, field_value::sfstring_id, "language"),
            node_interface(node_interface::field_id, field_value::sfbool_id, "leftToRight"),
            node_interface(node_interface::field_id, field_value::sffloat_id, "size"),
            node_interface(node_interface::field_id, field_value::sffloat_id, "spacing"),
            node_interface(node_interface::field_id, field_value::sfstring_id, "style"),
            node_interface(node_interface::field_id, field_value::sfbool_id, "topToBottom")
        };
        static const vrml97_node_interface_set_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 9);
        pos = nodeClassMap.find("urn:X-openvrml:node:FontStyle");
        assert(pos != nodeClassMap.end());
        this->add_type(pos->second->create_type("FontStyle",
                                                  nodeInterfaceSet));
    }

    //
    // Group node
    //
    {
        static const node_interface nodeInterfaces[] = {
            node_interface(node_interface::eventin_id, field_value::mfnode_id, "addChildren"),
            node_interface(node_interface::eventin_id, field_value::mfnode_id, "removeChildren"),
            node_interface(node_interface::exposedfield_id, field_value::mfnode_id, "children"),
            node_interface(node_interface::field_id, field_value::sfvec3f_id, "bboxCenter"),
            node_interface(node_interface::field_id, field_value::sfvec3f_id, "bboxSize")
        };
        static const vrml97_node_interface_set_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 5);
        pos = nodeClassMap.find("urn:X-openvrml:node:Group");
        assert(pos != nodeClassMap.end());
        this->add_type(pos->second->create_type("Group",
                                                  nodeInterfaceSet));
    }

    //
    // ImageTexture node
    //
    {
        static const node_interface nodeInterfaces[] = {
            node_interface(node_interface::exposedfield_id, field_value::mfstring_id, "url"),
            node_interface(node_interface::field_id, field_value::sfbool_id, "repeatS"),
            node_interface(node_interface::field_id, field_value::sfbool_id, "repeatT")
        };
        static const vrml97_node_interface_set_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 3);
        pos = nodeClassMap.find("urn:X-openvrml:node:ImageTexture");
        assert(pos != nodeClassMap.end());
        this->add_type(pos->second->create_type("ImageTexture",
                                                  nodeInterfaceSet));
    }

    //
    // IndexedFaceSet node
    //
    {
        static const node_interface nodeInterfaces[] = {
            node_interface(node_interface::eventin_id, field_value::mfint32_id, "set_colorIndex"),
            node_interface(node_interface::eventin_id, field_value::mfint32_id, "set_coordIndex"),
            node_interface(node_interface::eventin_id, field_value::mfint32_id, "set_normalIndex"),
            node_interface(node_interface::eventin_id, field_value::mfint32_id, "set_texCoordIndex"),
            node_interface(node_interface::exposedfield_id, field_value::sfnode_id, "color"),
            node_interface(node_interface::exposedfield_id, field_value::sfnode_id, "coord"),
            node_interface(node_interface::exposedfield_id, field_value::sfnode_id, "normal"),
            node_interface(node_interface::exposedfield_id, field_value::sfnode_id, "texCoord"),
            node_interface(node_interface::field_id, field_value::sfbool_id, "ccw"),
            node_interface(node_interface::field_id, field_value::mfint32_id, "colorIndex"),
            node_interface(node_interface::field_id, field_value::sfbool_id, "colorPerVertex"),
            node_interface(node_interface::field_id, field_value::sfbool_id, "convex"),
            node_interface(node_interface::field_id, field_value::mfint32_id, "coordIndex"),
            node_interface(node_interface::field_id, field_value::sffloat_id, "creaseAngle"),
            node_interface(node_interface::field_id, field_value::mfint32_id, "normalIndex"),
            node_interface(node_interface::field_id, field_value::sfbool_id, "normalPerVertex"),
            node_interface(node_interface::field_id, field_value::sfbool_id, "solid"),
            node_interface(node_interface::field_id, field_value::mfint32_id, "texCoordIndex")
        };
        static const vrml97_node_interface_set_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 18);
        pos = nodeClassMap.find("urn:X-openvrml:node:IndexedFaceSet");
        assert(pos != nodeClassMap.end());
        this->add_type(pos->second->create_type("IndexedFaceSet",
                                                  nodeInterfaceSet));
    }

    //
    // IndexedLineSet node
    //
    {
        static const node_interface nodeInterfaces[] = {
            node_interface(node_interface::eventin_id,
                          field_value::mfint32_id,
                          "set_colorIndex"),
            node_interface(node_interface::eventin_id,
                          field_value::mfint32_id,
                          "set_coordIndex"),
            node_interface(node_interface::exposedfield_id,
                          field_value::sfnode_id,
                          "color"),
            node_interface(node_interface::exposedfield_id,
                          field_value::sfnode_id,
                          "coord"),
            node_interface(node_interface::field_id,
                          field_value::mfint32_id,
                          "colorIndex"),
            node_interface(node_interface::field_id,
                          field_value::sfbool_id,
                          "colorPerVertex"),
            node_interface(node_interface::field_id,
                          field_value::mfint32_id,
                          "coordIndex")
        };
        static const vrml97_node_interface_set_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 7);
        pos = nodeClassMap.find("urn:X-openvrml:node:IndexedLineSet");
        assert(pos != nodeClassMap.end());
        this->add_type(pos->second->create_type("IndexedLineSet",
                                                  nodeInterfaceSet));
    }

    //
    // Inline node
    //
    {
        static const node_interface nodeInterfaces[] = {
            node_interface(node_interface::exposedfield_id, field_value::mfstring_id, "url"),
            node_interface(node_interface::field_id, field_value::sfvec3f_id, "bboxCenter"),
            node_interface(node_interface::field_id, field_value::sfvec3f_id, "bboxSize")
        };
        static const vrml97_node_interface_set_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 3);
        pos = nodeClassMap.find("urn:X-openvrml:node:Inline");
        assert(pos != nodeClassMap.end());
        this->add_type(pos->second->create_type("Inline", nodeInterfaceSet));
    }

    //
    // LOD node
    //
    {
        static const node_interface nodeInterfaces[] = {
            node_interface(node_interface::exposedfield_id, field_value::mfnode_id, "level"),
            node_interface(node_interface::field_id, field_value::sfvec3f_id, "center"),
            node_interface(node_interface::field_id, field_value::mffloat_id, "range")
        };
        static const vrml97_node_interface_set_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 3);
        pos = nodeClassMap.find("urn:X-openvrml:node:LOD");
        assert(pos != nodeClassMap.end());
        this->add_type(pos->second->create_type("LOD", nodeInterfaceSet));
    }

    //
    // Material node
    //
    {
        static const node_interface nodeInterfaces[] = {
            node_interface(node_interface::exposedfield_id, field_value::sffloat_id, "ambientIntensity"),
            node_interface(node_interface::exposedfield_id, field_value::sfcolor_id, "diffuseColor"),
            node_interface(node_interface::exposedfield_id, field_value::sfcolor_id, "emissiveColor"),
            node_interface(node_interface::exposedfield_id, field_value::sffloat_id, "shininess"),
            node_interface(node_interface::exposedfield_id, field_value::sfcolor_id, "specularColor"),
            node_interface(node_interface::exposedfield_id, field_value::sffloat_id, "transparency")
        };
        static const vrml97_node_interface_set_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 6);
        pos = nodeClassMap.find("urn:X-openvrml:node:Material");
        assert(pos != nodeClassMap.end());
        this->add_type(pos->second->create_type("Material",
                                                  nodeInterfaceSet));
    }

    //
    // MovieTexture node
    //
    {
        static const node_interface nodeInterfaces[] = {
            node_interface(node_interface::exposedfield_id, field_value::sfbool_id, "loop"),
            node_interface(node_interface::exposedfield_id, field_value::sffloat_id, "speed"),
            node_interface(node_interface::exposedfield_id, field_value::sftime_id, "startTime"),
            node_interface(node_interface::exposedfield_id, field_value::sftime_id, "stopTime"),
            node_interface(node_interface::exposedfield_id, field_value::mfstring_id, "url"),
            node_interface(node_interface::field_id, field_value::sfbool_id, "repeatS"),
            node_interface(node_interface::field_id, field_value::sfbool_id, "repeatT"),
            node_interface(node_interface::eventout_id, field_value::sftime_id, "duration_changed"),
            node_interface(node_interface::eventout_id, field_value::sfbool_id, "isActive")
        };
        static const vrml97_node_interface_set_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 9);
        pos = nodeClassMap.find("urn:X-openvrml:node:MovieTexture");
        assert(pos != nodeClassMap.end());
        this->add_type(pos->second->create_type("MovieTexture",
                                                  nodeInterfaceSet));
    }

    //
    // NavigationInfo node
    //
    {
        static const node_interface nodeInterfaces[] = {
            node_interface(node_interface::eventin_id, field_value::sfbool_id, "set_bind"),
            node_interface(node_interface::exposedfield_id, field_value::mffloat_id, "avatarSize"),
            node_interface(node_interface::exposedfield_id, field_value::sfbool_id, "headlight"),
            node_interface(node_interface::exposedfield_id, field_value::sffloat_id, "speed"),
            node_interface(node_interface::exposedfield_id, field_value::mfstring_id, "type"),
            node_interface(node_interface::exposedfield_id, field_value::sffloat_id, "visibilityLimit"),
            node_interface(node_interface::eventout_id, field_value::sfbool_id, "isBound")
        };
        static const vrml97_node_interface_set_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 7);
        pos = nodeClassMap.find("urn:X-openvrml:node:NavigationInfo");
        assert(pos != nodeClassMap.end());
        this->add_type(pos->second->create_type("NavigationInfo",
                                                  nodeInterfaceSet));
    }

    //
    // Normal node
    //
    {
        static const node_interface nodeInterface =
                node_interface(node_interface::exposedfield_id, field_value::mfvec3f_id, "vector");
        static const vrml97_node_interface_set_
                nodeInterfaceSet(&nodeInterface, &nodeInterface + 1);
        pos = nodeClassMap.find("urn:X-openvrml:node:Normal");
        assert(pos != nodeClassMap.end());
        this->add_type(pos->second->create_type("Normal",
                                                  nodeInterfaceSet));
    }

    //
    // NormalInterpolator node
    //
    {
        static const node_interface nodeInterfaces[] = {
            node_interface(node_interface::eventin_id, field_value::sffloat_id, "set_fraction"),
            node_interface(node_interface::exposedfield_id, field_value::mffloat_id, "key"),
            node_interface(node_interface::exposedfield_id, field_value::mfvec3f_id, "keyValue"),
            node_interface(node_interface::eventout_id, field_value::mfvec3f_id, "value_changed")
        };
        static const vrml97_node_interface_set_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 4);
        pos = nodeClassMap.find("urn:X-openvrml:node:NormalInterpolator");
        assert(pos != nodeClassMap.end());
        this->add_type(pos->second->create_type("NormalInterpolator",
                                                  nodeInterfaceSet));
    }

    //
    // OrientationInterpolator node
    //
    {
        static const node_interface nodeInterfaces[] = {
            node_interface(node_interface::eventin_id,
                          field_value::sffloat_id,
                          "set_fraction"),
            node_interface(node_interface::exposedfield_id,
                          field_value::mffloat_id,
                          "key"),
            node_interface(node_interface::exposedfield_id,
                          field_value::mfrotation_id,
                          "keyValue"),
            node_interface(node_interface::eventout_id,
                          field_value::sfrotation_id,
                          "value_changed")
        };
        static const vrml97_node_interface_set_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 4);
        pos = nodeClassMap.find("urn:X-openvrml:node:OrientationInterpolator");
        assert(pos != nodeClassMap.end());
        this->add_type(pos->second->create_type("OrientationInterpolator",
                                                  nodeInterfaceSet));
    }

    //
    // PixelTexture node
    //
    {
        static const node_interface nodeInterfaces[] = {
            node_interface(node_interface::exposedfield_id,
                          field_value::sfimage_id,
                          "image"),
            node_interface(node_interface::field_id,
                          field_value::sfbool_id,
                          "repeatS"),
            node_interface(node_interface::field_id,
                          field_value::sfbool_id,
                          "repeatT")
        };
        static const vrml97_node_interface_set_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 3);
        pos = nodeClassMap.find("urn:X-openvrml:node:PixelTexture");
        assert(pos != nodeClassMap.end());
        this->add_type(pos->second->create_type("PixelTexture",
                                                  nodeInterfaceSet));
    }

    //
    // PlaneSensor node
    //
    {
        static const node_interface nodeInterfaces[] = {
            node_interface(node_interface::exposedfield_id, field_value::sfbool_id, "autoOffset"),
            node_interface(node_interface::exposedfield_id, field_value::sfbool_id, "enabled"),
            node_interface(node_interface::exposedfield_id, field_value::sfvec2f_id, "maxPosition"),
            node_interface(node_interface::exposedfield_id, field_value::sfvec2f_id, "minPosition"),
            node_interface(node_interface::exposedfield_id, field_value::sfvec3f_id, "offset"),
            node_interface(node_interface::eventout_id, field_value::sfbool_id, "isActive"),
            node_interface(node_interface::eventout_id, field_value::sfvec3f_id, "trackPoint_changed"),
            node_interface(node_interface::eventout_id, field_value::sfvec3f_id, "translation_changed")
        };
        static const vrml97_node_interface_set_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 8);
        pos = nodeClassMap.find("urn:X-openvrml:node:PlaneSensor");
        assert(pos != nodeClassMap.end());
        this->add_type(pos->second->create_type("PlaneSensor",
                                                  nodeInterfaceSet));
    }

    //
    // PointLight node
    //
    {
        static const node_interface nodeInterfaces[] = {
            node_interface(node_interface::exposedfield_id, field_value::sffloat_id, "ambientIntensity"),
            node_interface(node_interface::exposedfield_id, field_value::sfvec3f_id, "attenuation"),
            node_interface(node_interface::exposedfield_id, field_value::sfcolor_id, "color"),
            node_interface(node_interface::exposedfield_id, field_value::sffloat_id, "intensity"),
            node_interface(node_interface::exposedfield_id, field_value::sfvec3f_id, "location"),
            node_interface(node_interface::exposedfield_id, field_value::sfbool_id, "on"),
            node_interface(node_interface::exposedfield_id, field_value::sffloat_id, "radius")
        };
        static const vrml97_node_interface_set_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 7);
        pos = nodeClassMap.find("urn:X-openvrml:node:PointLight");
        assert(pos != nodeClassMap.end());
        this->add_type(pos->second->create_type("PointLight",
                                                  nodeInterfaceSet));
    }

    //
    // PointSet node
    //
    {
        static const node_interface nodeInterfaces[] = {
            node_interface(node_interface::exposedfield_id, field_value::sfnode_id, "color"),
            node_interface(node_interface::exposedfield_id, field_value::sfnode_id, "coord")
        };
        static const vrml97_node_interface_set_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 2);
        pos = nodeClassMap.find("urn:X-openvrml:node:PointSet");
        assert(pos != nodeClassMap.end());
        this->add_type(pos->second->create_type("PointSet",
                                                  nodeInterfaceSet));
    }

    //
    // PositionInterpolator node
    //
    {
        static const node_interface nodeInterfaces[] = {
            node_interface(node_interface::eventin_id,
                          field_value::sffloat_id,
                          "set_fraction"),
            node_interface(node_interface::exposedfield_id,
                          field_value::mffloat_id,
                          "key"),
            node_interface(node_interface::exposedfield_id,
                          field_value::mfvec3f_id,
                          "keyValue"),
            node_interface(node_interface::eventout_id,
                          field_value::sfvec3f_id,
                          "value_changed")
        };
        static const vrml97_node_interface_set_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 4);
        pos = nodeClassMap.find("urn:X-openvrml:node:PositionInterpolator");
        assert(pos != nodeClassMap.end());
        this->add_type(pos->second->create_type("PositionInterpolator",
                                                  nodeInterfaceSet));
    }

    //
    // ProximitySensor node
    //
    {
        static const node_interface nodeInterfaces[] = {
            node_interface(node_interface::exposedfield_id, field_value::sfvec3f_id, "center"),
            node_interface(node_interface::exposedfield_id, field_value::sfvec3f_id, "size"),
            node_interface(node_interface::exposedfield_id, field_value::sfbool_id, "enabled"),
            node_interface(node_interface::eventout_id, field_value::sfbool_id, "isActive"),
            node_interface(node_interface::eventout_id, field_value::sfvec3f_id, "position_changed"),
            node_interface(node_interface::eventout_id, field_value::sfrotation_id, "orientation_changed"),
            node_interface(node_interface::eventout_id, field_value::sftime_id, "enterTime"),
            node_interface(node_interface::eventout_id, field_value::sftime_id, "exitTime")
        };
        static const vrml97_node_interface_set_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 8);
        pos = nodeClassMap.find("urn:X-openvrml:node:ProximitySensor");
        assert(pos != nodeClassMap.end());
        this->add_type(pos->second->create_type("ProximitySensor",
                                                nodeInterfaceSet));
    }

    //
    // ScalarInterpolator node
    //
    {
        static const node_interface nodeInterfaces[] = {
            node_interface(node_interface::eventin_id,
                           field_value::sffloat_id,
                           "set_fraction"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mffloat_id,
                           "key"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mffloat_id,
                           "keyValue"),
            node_interface(node_interface::eventout_id,
                           field_value::sffloat_id,
                           "value_changed")
        };
        static const vrml97_node_interface_set_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 4);
        pos = nodeClassMap.find("urn:X-openvrml:node:ScalarInterpolator");
        assert(pos != nodeClassMap.end());
        this->add_type(pos->second->create_type("ScalarInterpolator",
                                                nodeInterfaceSet));
    }

    //
    // Shape node
    //
    {
        static const node_interface nodeInterfaces[] = {
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "appearance"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "geometry")
        };
        static const vrml97_node_interface_set_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 2);
        pos = nodeClassMap.find("urn:X-openvrml:node:Shape");
        assert(pos != nodeClassMap.end());
        this->add_type(pos->second->create_type("Shape", nodeInterfaceSet));
    }

    //
    // Sound node
    //
    {
        static const node_interface nodeInterfaces[] = {
            node_interface(node_interface::exposedfield_id,
                           field_value::sfvec3f_id,
                           "direction"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sffloat_id,
                           "intensity"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfvec3f_id,
                           "location"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sffloat_id,
                           "maxBack"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sffloat_id,
                           "maxFront"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sffloat_id,
                           "minBack"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sffloat_id,
                           "minFront"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sffloat_id,
                           "priority"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "source"),
            node_interface(node_interface::field_id,
                           field_value::sfbool_id,
                           "spatialize")
        };
        static const vrml97_node_interface_set_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 10);
        pos = nodeClassMap.find("urn:X-openvrml:node:Sound");
        assert(pos != nodeClassMap.end());
        this->add_type(pos->second->create_type("Sound", nodeInterfaceSet));
    }

    //
    // Sphere node
    //
    {
        static const node_interface nodeInterface =
                node_interface(node_interface::field_id,
                               field_value::sffloat_id,
                               "radius");
        static const vrml97_node_interface_set_
                nodeInterfaceSet(&nodeInterface, &nodeInterface + 1);
        pos = nodeClassMap.find("urn:X-openvrml:node:Sphere");
        assert(pos != nodeClassMap.end());
        this->add_type(pos->second->create_type("Sphere", nodeInterfaceSet));
    }

    //
    // SphereSensor node
    //
    {
        static const node_interface nodeInterfaces[] = {
            node_interface(node_interface::exposedfield_id,
                           field_value::sfbool_id,
                           "autoOffset"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfbool_id,
                           "enabled"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sffloat_id,
                           "offset"),
            node_interface(node_interface::eventout_id,
                           field_value::sfbool_id,
                           "isActive"),
            node_interface(node_interface::eventout_id,
                           field_value::sfrotation_id,
                           "rotation_changed"),
            node_interface(node_interface::eventout_id,
                           field_value::sfvec3f_id,
                           "trackPoint_changed")
        };
        static const vrml97_node_interface_set_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 6);
        pos = nodeClassMap.find("urn:X-openvrml:node:SphereSensor");
        assert(pos != nodeClassMap.end());
        this->add_type(pos->second->create_type("SphereSensor",
                                                nodeInterfaceSet));
    }

    //
    // SpotLight node
    //
    {
        static const node_interface nodeInterfaces[] = {
            node_interface(node_interface::exposedfield_id,
                          field_value::sffloat_id,
                          "ambientIntensity"),
            node_interface(node_interface::exposedfield_id,
                          field_value::sfvec3f_id,
                          "attenuation"),
            node_interface(node_interface::exposedfield_id,
                          field_value::sffloat_id,
                          "beamWidth"),
            node_interface(node_interface::exposedfield_id,
                          field_value::sfcolor_id,
                          "color"),
            node_interface(node_interface::exposedfield_id,
                          field_value::sffloat_id,
                          "cutOffAngle"),
            node_interface(node_interface::exposedfield_id,
                          field_value::sfvec3f_id,
                          "direction"),
            node_interface(node_interface::exposedfield_id,
                          field_value::sffloat_id,
                          "intensity"),
            node_interface(node_interface::exposedfield_id,
                          field_value::sfvec3f_id,
                          "location"),
            node_interface(node_interface::exposedfield_id,
                          field_value::sfbool_id,
                          "on"),
            node_interface(node_interface::exposedfield_id,
                          field_value::sffloat_id,
                          "radius")
        };
        static const vrml97_node_interface_set_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 10);
        pos = nodeClassMap.find("urn:X-openvrml:node:SpotLight");
        assert(pos != nodeClassMap.end());
        this->add_type(pos->second->create_type("SpotLight",
                                                nodeInterfaceSet));
    }

    //
    // Switch node
    //
    {
        static const node_interface nodeInterfaces[] = {
            node_interface(node_interface::exposedfield_id,
                          field_value::mfnode_id,
                          "choice"),
            node_interface(node_interface::exposedfield_id,
                          field_value::sfint32_id,
                          "whichChoice")
        };
        static const vrml97_node_interface_set_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 2);
        pos = nodeClassMap.find("urn:X-openvrml:node:Switch");
        assert(pos != nodeClassMap.end());
        this->add_type(pos->second->create_type("Switch", nodeInterfaceSet));
    }

    //
    // Text node
    //
    {
        static const node_interface nodeInterfaces[] = {
            node_interface(node_interface::exposedfield_id,
                          field_value::mfstring_id,
                          "string"),
            node_interface(node_interface::exposedfield_id,
                          field_value::sfnode_id,
                          "fontStyle"),
            node_interface(node_interface::exposedfield_id,
                          field_value::mffloat_id,
                          "length"),
            node_interface(node_interface::exposedfield_id,
                          field_value::sffloat_id,
                          "maxExtent")
        };
        static const vrml97_node_interface_set_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 4);
        pos = nodeClassMap.find("urn:X-openvrml:node:Text");
        assert(pos != nodeClassMap.end());
        this->add_type(pos->second->create_type("Text", nodeInterfaceSet));
    }

    //
    // TextureCoordinate node
    //
    {
        static const node_interface nodeInterface =
                node_interface(node_interface::exposedfield_id,
                              field_value::mfvec2f_id,
                              "point");
        static const vrml97_node_interface_set_
                nodeInterfaceSet(&nodeInterface, &nodeInterface + 1);
        pos = nodeClassMap.find("urn:X-openvrml:node:TextureCoordinate");
        assert(pos != nodeClassMap.end());
        this->add_type(pos->second->create_type("TextureCoordinate",
                                                nodeInterfaceSet));
    }

    //
    // TextureTransform node
    //
    {
        static const node_interface nodeInterfaces[] = {
            node_interface(node_interface::exposedfield_id,
                           field_value::sfvec2f_id,
                           "center"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sffloat_id,
                           "rotation"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfvec2f_id,
                           "scale"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfvec2f_id,
                           "translation")
        };
        static const vrml97_node_interface_set_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 4);
        pos = nodeClassMap.find("urn:X-openvrml:node:TextureTransform");
        assert(pos != nodeClassMap.end());
        this->add_type(pos->second->create_type("TextureTransform",
                                                nodeInterfaceSet));
    }

    //
    // TimeSensor node
    //
    {
        static const node_interface nodeInterfaces[] = {
            node_interface(node_interface::exposedfield_id, field_value::sftime_id, "cycleInterval"),
            node_interface(node_interface::exposedfield_id, field_value::sfbool_id, "enabled"),
            node_interface(node_interface::exposedfield_id, field_value::sfbool_id, "loop"),
            node_interface(node_interface::exposedfield_id, field_value::sftime_id, "startTime"),
            node_interface(node_interface::exposedfield_id, field_value::sftime_id, "stopTime"),
            node_interface(node_interface::eventout_id, field_value::sftime_id, "cycleTime"),
            node_interface(node_interface::eventout_id, field_value::sffloat_id, "fraction_changed"),
            node_interface(node_interface::eventout_id, field_value::sfbool_id, "isActive"),
            node_interface(node_interface::eventout_id, field_value::sftime_id, "time")
        };
        static const vrml97_node_interface_set_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 9);
        pos = nodeClassMap.find("urn:X-openvrml:node:TimeSensor");
        assert(pos != nodeClassMap.end());
        this->add_type(pos->second->create_type("TimeSensor",
                                                  nodeInterfaceSet));
    }

    //
    // TouchSensor node
    //
    {
        static const node_interface nodeInterfaces[] = {
            node_interface(node_interface::exposedfield_id,
                          field_value::sfbool_id,
                          "enabled"),
            node_interface(node_interface::eventout_id,
                          field_value::sfvec3f_id,
                          "hitNormal_changed"),
            node_interface(node_interface::eventout_id,
                          field_value::sfvec3f_id,
                          "hitPoint_changed"),
            node_interface(node_interface::eventout_id,
                          field_value::sfvec2f_id,
                          "hitTexCoord_changed"),
            node_interface(node_interface::eventout_id,
                          field_value::sfbool_id,
                          "isActive"),
            node_interface(node_interface::eventout_id,
                          field_value::sfbool_id,
                          "isOver"),
            node_interface(node_interface::eventout_id,
                          field_value::sftime_id,
                          "touchTime")
        };
        static const vrml97_node_interface_set_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 7);
        pos = nodeClassMap.find("urn:X-openvrml:node:TouchSensor");
        assert(pos != nodeClassMap.end());
        this->add_type(pos->second->create_type("TouchSensor",
                                                  nodeInterfaceSet));
    }

    //
    // Transform node
    //
    {
        static const node_interface nodeInterfaces[] = {
            node_interface(node_interface::eventin_id,
                           field_value::mfnode_id,
                           "addChildren"),
            node_interface(node_interface::eventin_id,
                           field_value::mfnode_id,
                           "removeChildren"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfvec3f_id,
                           "center"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mfnode_id,
                           "children"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfrotation_id,
                           "rotation"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfvec3f_id,
                           "scale"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfrotation_id,
                           "scaleOrientation"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfvec3f_id,
                           "translation"),
            node_interface(node_interface::field_id,
                           field_value::sfvec3f_id,
                           "bboxCenter"),
            node_interface(node_interface::field_id,
                           field_value::sfvec3f_id,
                           "bboxSize")
        };
        static const vrml97_node_interface_set_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 10);
        pos = nodeClassMap.find("urn:X-openvrml:node:Transform");
        assert(pos != nodeClassMap.end());
        this->add_type(pos->second->create_type("Transform",
                                                  nodeInterfaceSet));
    }

    //
    // Viewpoint node
    //
    {
        static const node_interface nodeInterfaces[] = {
            node_interface(node_interface::eventin_id,
                          field_value::sfbool_id,
                          "set_bind"),
            node_interface(node_interface::exposedfield_id,
                          field_value::sffloat_id,
                          "fieldOfView"),
            node_interface(node_interface::exposedfield_id,
                          field_value::sfbool_id, "jump"),
            node_interface(node_interface::exposedfield_id,
                          field_value::sfrotation_id,
                          "orientation"),
            node_interface(node_interface::exposedfield_id,
                          field_value::sfvec3f_id,
                          "position"),
            node_interface(node_interface::field_id,
                          field_value::sfstring_id,
                          "description"),
            node_interface(node_interface::eventout_id,
                          field_value::sftime_id,
                          "bindTime"),
            node_interface(node_interface::eventout_id,
                          field_value::sfbool_id,
                          "isBound")
        };
        static const vrml97_node_interface_set_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 8);
        pos = nodeClassMap.find("urn:X-openvrml:node:Viewpoint");
        assert(pos != nodeClassMap.end());
        this->add_type(pos->second->create_type("Viewpoint",
                                                  nodeInterfaceSet));
    }

    //
    // VisibilitySensor node
    //
    {
        static const node_interface nodeInterfaces[] = {
            node_interface(node_interface::exposedfield_id,
                          field_value::sfvec3f_id,
                          "center"),
            node_interface(node_interface::exposedfield_id,
                          field_value::sfbool_id,
                          "enabled"),
            node_interface(node_interface::exposedfield_id,
                          field_value::sfvec3f_id,
                          "size"),
            node_interface(node_interface::eventout_id,
                          field_value::sftime_id,
                          "enterTime"),
            node_interface(node_interface::eventout_id,
                          field_value::sftime_id,
                          "exitTime"),
            node_interface(node_interface::eventout_id,
                          field_value::sfbool_id,
                          "isActive")
        };
        static const vrml97_node_interface_set_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 6);
        pos = nodeClassMap.find("urn:X-openvrml:node:VisibilitySensor");
        assert(pos != nodeClassMap.end());
        this->add_type(pos->second->create_type("VisibilitySensor",
                                                nodeInterfaceSet));
    }

    //
    // WorldInfo node
    //
    {
        static const node_interface nodeInterfaces[] = {
            node_interface(node_interface::field_id,
                           field_value::mfstring_id,
                           "info"),
            node_interface(node_interface::field_id,
                           field_value::sfstring_id,
                           "title")
        };
        static const vrml97_node_interface_set_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 2);
        pos = nodeClassMap.find("urn:X-openvrml:node:WorldInfo");
        assert(pos != nodeClassMap.end());
        this->add_type(pos->second->create_type("WorldInfo",
                                                nodeInterfaceSet));
    }
}

Vrml97RootScope::~Vrml97RootScope() throw () {}


null_node_class::null_node_class(OpenVRML::browser & browser) throw ():
    node_class(browser)
{}

null_node_class::~null_node_class() throw ()
{}

const node_type_ptr
null_node_class::create_type(const std::string & id,
                             const node_interface_set & interfaces)
    throw ()
{
    assert(false);
    static const node_type_ptr nodeType;
    return nodeType;
}


null_node_type::null_node_type(null_node_class & nodeClass) throw ():
    node_type(nodeClass, std::string())
{}

null_node_type::~null_node_type() throw ()
{}

const node_interface_set & null_node_type::interfaces() const throw ()
{
    assert(false);
    static const node_interface_set interfaces;
    return interfaces;
}

const node_ptr null_node_type::create_node(const scope_ptr & scope) const throw ()
{
    assert(false);
    static const node_ptr node;
    return node;
}


/**
 * @brief Constructor.
 *
 * @param nodeType  the browser's null_node_type instance.
 */
DefaultViewpoint::DefaultViewpoint(const null_node_type & nodeType) throw ():
    node(nodeType, scope_ptr()),
    child_node(nodeType, scope_ptr()),
    viewpoint_node(nodeType, scope_ptr())
{}

/**
 * @brief Destructor.
 */
DefaultViewpoint::~DefaultViewpoint() throw ()
{}

const mat4f & DefaultViewpoint::transformation() const throw ()
{
    static const vec3f position(0.0, 0.0, 10.0);
    static const rotation orientation;
    static const vec3f scale(1.0, 1.0, 1.0);
    static const rotation scaleOrientation;
    static const vec3f center;
    static const mat4f t(mat4f::transformation(position,
                                               orientation,
                                               scale,
                                               scaleOrientation,
                                               center));
    return t;
}

const mat4f & DefaultViewpoint::user_view_transform() const throw ()
{
    return this->userViewTransform;
}

void DefaultViewpoint::user_view_transform(const mat4f & transform)
    throw ()
{
    this->userViewTransform = transform;
}

const std::string & DefaultViewpoint::description() const throw ()
{
    static const std::string desc;
    return desc;
}

float DefaultViewpoint::field_of_view() const throw ()
{
    static const float fieldOfView = 0.785398;
    return fieldOfView;
}

void DefaultViewpoint::do_field(const std::string & id,
                                const field_value & value)
    throw ()
{
    assert(false);
}

const field_value & DefaultViewpoint::do_field(const std::string & id) const
    throw ()
{
    assert(false);
    static const sfbool value;
    return value;
}

void DefaultViewpoint::do_process_event(const std::string & id,
                                        const field_value & value,
                                        double timestamp)
    throw ()
{
    assert(false);
}

const field_value &
DefaultViewpoint::do_eventout(const std::string & id) const throw ()
{
    assert(false);
    static const sfbool value;
    return value;
}


namespace {

    const char * const expression =
            "^(([^:/?#]+):)?((//([^/?#]*))?([^?#]*)([?]([^#]*))?(#(.*))?)";
    //        |+- scheme    ||  |          |       |   |        | +- fragment-id
    //        +- scheme ':' ||  |          |       |   |        +- '#' fragment-id
    //                      ||  |          |       |   +- query
    //                      ||  |          |       +- '?' query
    //                      ||  |          +- path
    //                      ||  +- authority
    //                      |+- "//" authority
    //                      +- scheme-specific-part

    class URIRegex {
        regex_t regex;

    public:
        URIRegex() throw (std::bad_alloc);
        ~URIRegex() throw ();

        int exec(const char * str, size_t nmatch, regmatch_t pmatch[],
                 int eflags);
    };

    URIRegex::URIRegex() throw (std::bad_alloc) {
        int err = regcomp(&this->regex, expression, REG_EXTENDED);
        if (err == REG_ESPACE) { throw std::bad_alloc(); }
        assert(err == 0);
    }

    URIRegex::~URIRegex() throw () { regfree(&this->regex); }

    int URIRegex::exec(const char * str, size_t nmatch, regmatch_t pmatch[],
                       int eflags) {
        return regexec(&this->regex, str, nmatch, pmatch, eflags);
    }

    URIRegex uriRegex;

    URI::URI(const std::string & str) throw (invalid_url, std::bad_alloc):
            str(str) {
        int err = uriRegex.exec(str.c_str(), URI::nmatch, this->regmatch, 0);
        if (err != 0) { throw invalid_url(); }
    }

    URI::operator std::string() const throw (std::bad_alloc) {
        return this->str;
    }

    const std::string URI::getScheme() const throw (std::bad_alloc) {
        return (this->regmatch[2].rm_so > -1)
                ? this->str.substr(this->regmatch[2].rm_so,
                                   this->regmatch[2].rm_eo - this->regmatch[2].rm_so)
                : std::string();
    }

    const std::string URI::getSchemeSpecificPart() const
            throw (std::bad_alloc) {
        return (this->regmatch[3].rm_so > -1)
                ? this->str.substr(this->regmatch[3].rm_so,
                                   this->regmatch[3].rm_eo - this->regmatch[3].rm_so)
                : std::string();
    }

    const std::string URI::getAuthority() const throw (std::bad_alloc) {
        return (this->regmatch[5].rm_so > -1)
                ? this->str.substr(this->regmatch[5].rm_so,
                                   this->regmatch[5].rm_eo - this->regmatch[5].rm_so)
                : std::string();
    }

    const std::string URI::getUserinfo() const throw (std::bad_alloc) {
        return std::string();
    }

    const std::string URI::getHost() const throw (std::bad_alloc) {
        return std::string();
    }

    const std::string URI::getPort() const throw (std::bad_alloc) {
        return std::string();
    }

    const std::string URI::getPath() const throw (std::bad_alloc) {
        return (this->regmatch[6].rm_so > -1)
                ? this->str.substr(this->regmatch[6].rm_so,
                                   this->regmatch[6].rm_eo - this->regmatch[6].rm_so)
                : std::string();
    }

    const std::string URI::getQuery() const throw (std::bad_alloc) {
        return (this->regmatch[7].rm_so > -1)
                ? this->str.substr(this->regmatch[7].rm_so,
                                   this->regmatch[7].rm_eo - this->regmatch[7].rm_so)
                : std::string();
    }

    const std::string URI::getFragment() const throw (std::bad_alloc) {
        return (this->regmatch[10].rm_so > -1)
                ? this->str.substr(this->regmatch[10].rm_so,
                                   this->regmatch[10].rm_eo - this->regmatch[10].rm_so)
                : std::string();
    }

    const URI URI::resolveAgainst(const URI & absoluteURI) const
            throw (std::bad_alloc) {
        using std::list;
        using std::string;

        assert(this->getScheme().empty());
        assert(!absoluteURI.getScheme().empty());

        string result = absoluteURI.getScheme() + ':';

        if (!this->getAuthority().empty()) {
            return result + this->getSchemeSpecificPart();
        } else {
            result += "//" + absoluteURI.getAuthority();
        }

        string path = absoluteURI.getPath();
        const string::size_type lastSlashIndex = path.find_last_of('/');

        //
        // Chop off the leading slash and the last path segment (typically a
        // file name).
        //
        path = path.substr(1, lastSlashIndex);

        //
        // Append the relative path.
        //
        path += this->getPath();

        //
        // Put the path segments in a list to process them.
        //
        list<string> pathSegments;
        string::size_type slashIndex = 0;
        string::size_type segmentStartIndex = 0;
        do {
            slashIndex = path.find('/', segmentStartIndex);
            string segment = path.substr(segmentStartIndex,
                                         slashIndex - segmentStartIndex);
            if (!segment.empty()) {
                pathSegments.push_back(segment);
            }
            segmentStartIndex = slashIndex + 1;
        } while (slashIndex != string::npos);

        //
        // Remove any "." segments.
        //
        pathSegments.remove(".");

        //
        // Remove any ".." segments along with the segment that precedes them.
        //
        const list<string>::iterator begin(pathSegments.begin());
        list<string>::iterator pos;
        for (pos = begin; pos != pathSegments.end(); ++pos) {
            if (pos != begin && *pos == "..") {
                --(pos = pathSegments.erase(pos));
                --(pos = pathSegments.erase(pos));
            }
        }

        //
        // Reconstruct the path.
        //
        path = string();
        for (pos = pathSegments.begin(); pos != pathSegments.end(); ++pos) {
            path += '/' + *pos;
        }

        //
        // End in a slash?
        //
        if (*(this->getPath().end() - 1) == '/') { path += '/'; }

        result += path;

        const string query = this->getQuery();
        if (!query.empty()) { result += '?' + query; }

        const string fragment = this->getFragment();
        if (!fragment.empty()) { result += '#' + fragment; }

        return result;
    }

} // namespace

} // namespace OpenVRML
