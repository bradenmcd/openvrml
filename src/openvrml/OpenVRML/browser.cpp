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
# include "Viewer.h"
# include "scope.h"
# include "VrmlRenderContext.h"
# include "script.h"
# include "vrml97node.h"

namespace OpenVRML {

    class NodeInterfaceTypeMismatch : public std::runtime_error {
    public:
        NodeInterfaceTypeMismatch(NodeInterface::Type lhs,
                                  NodeInterface::Type rhs);
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

    class ProtoNode : public Node {
        friend class ProtoNodeClass;
        friend class Vrml97Parser;

    public:
        struct ImplNodeInterface {
            Node & node;
            std::string interfaceId;

            ImplNodeInterface(Node & node, const std::string & interfaceId);
        };
        typedef std::multimap<std::string, ImplNodeInterface> ISMap;
        typedef std::map<std::string, FieldValuePtr> FieldValueMap;
        typedef std::map<std::string, PolledEventOutValue> EventOutValueMap;

        ISMap isMap;

    private:
        FieldValueMap unISdFieldValueMap;
        EventOutValueMap eventOutValueMap;
        std::vector<NodePtr> implNodes;

    public:
        explicit ProtoNode(const NodeType & nodeType) throw (std::bad_alloc);
        ProtoNode(const NodeType & nodeType,
                  const ScopePtr & scope,
                  const ProtoNode & node) throw (std::bad_alloc);
        virtual ~ProtoNode() throw ();

        const std::vector<NodePtr> & getImplNodes() const throw ();

        void addRootNode(const NodePtr & node) throw (std::bad_alloc);
        void addIS(Node & implNode,
                   const std::string & implNodeInterfaceId,
                   const std::string & protoInterfaceId)
            throw (UnsupportedInterface, NodeInterfaceTypeMismatch,
                   FieldValueTypeMismatch, std::bad_alloc);

        void update(double time);

        virtual const ScriptNode * toScript() const throw ();
        virtual ScriptNode * toScript() throw ();
        virtual const AppearanceNode * toAppearance() const throw ();
        virtual AppearanceNode * toAppearance() throw ();
        virtual const ChildNode * toChild() const throw ();
        virtual ChildNode * toChild() throw ();
        virtual const ColorNode * toColor() const throw ();
        virtual ColorNode * toColor() throw ();
        virtual const CoordinateNode * toCoordinate() const throw ();
        virtual CoordinateNode * toCoordinate() throw ();
        virtual const FontStyleNode * toFontStyle() const throw ();
        virtual FontStyleNode * toFontStyle() throw () ;
        virtual const GeometryNode * toGeometry() const throw ();
        virtual GeometryNode * toGeometry() throw ();
        virtual const MaterialNode * toMaterial() const throw ();
        virtual MaterialNode * toMaterial() throw ();
        virtual const NormalNode * toNormal() const throw ();
        virtual NormalNode * toNormal() throw ();
        virtual const SoundSourceNode * toSoundSource() const throw ();
        virtual SoundSourceNode * toSoundSource() throw ();
        virtual const TextureNode * toTexture() const throw ();
        virtual TextureNode * toTexture() throw ();
        virtual const TextureCoordinateNode * toTextureCoordinate() const
                throw ();
        virtual TextureCoordinateNode * toTextureCoordinate() throw ();
        virtual const TextureTransformNode * toTextureTransform() const
                throw ();
        virtual TextureTransformNode * toTextureTransform() throw ();
        virtual const TransformNode * toTransform() const throw ();
        virtual TransformNode * toTransform() throw ();
        virtual const ViewpointNode * toViewpoint() const throw ();
        virtual ViewpointNode * toViewpoint() throw ();

        virtual Vrml97Node::Anchor * toAnchor() const;
        virtual Vrml97Node::AudioClip * toAudioClip() const;
        virtual Vrml97Node::CylinderSensor * toCylinderSensor() const;
        virtual Vrml97Node::AbstractLight * toLight() const;
        virtual Vrml97Node::MovieTexture * toMovieTexture() const;
        virtual Vrml97Node::NavigationInfo * toNavigationInfo() const;
        virtual Vrml97Node::PlaneSensor * toPlaneSensor() const;
        virtual Vrml97Node::PointLight * toPointLight() const;
        virtual Vrml97Node::SphereSensor * toSphereSensor() const;
        virtual Vrml97Node::SpotLight * toSpotLight() const;
        virtual Vrml97Node::TimeSensor * toTimeSensor() const;
        virtual Vrml97Node::TouchSensor * toTouchSensor() const;

        virtual void render(Viewer & viewer, VrmlRenderContext context);

    private:
        // Not copyable.
        ProtoNode(const ProtoNode &);
        ProtoNode & operator=(const ProtoNode &);

        virtual void do_initialize(double timestamp) throw (std::bad_alloc);
        virtual void do_setField(const std::string & id,
                                 const FieldValue & value)
                throw (UnsupportedInterface, std::bad_cast, std::bad_alloc);
        virtual const FieldValue & do_getField(const std::string & id) const
                throw (UnsupportedInterface);
        virtual void do_processEvent(const std::string & id,
                                      const FieldValue & value,
                                      double timestamp)
                throw (UnsupportedInterface, std::bad_cast, std::bad_alloc);
        virtual const FieldValue & do_getEventOut(const std::string & id) const
                throw (UnsupportedInterface);
        virtual void do_shutdown(double timestamp) throw ();
    };


    class ProtoNodeClass : public NodeClass {
        friend class ProtoNode;
        friend class Vrml97Parser;

    public:
        class ProtoNodeType : public NodeType {
            NodeInterfaceSet nodeInterfaces;

        public:
            ProtoNodeType(ProtoNodeClass & nodeClass, const std::string & id)
                throw (UnsupportedInterface, std::bad_alloc);
            virtual ~ProtoNodeType() throw ();

            virtual const NodeInterfaceSet & getInterfaces() const throw ();
            virtual const NodePtr createNode(const ScopePtr & scope) const
                throw (std::bad_alloc);

            void addInterface(const NodeInterface & interface)
                throw (std::invalid_argument, std::bad_alloc);
        };

        friend class ProtoNodeType;

    private:
        typedef std::map<std::string, FieldValuePtr> DefaultValueMap;

        ProtoNodeType protoNodeType;
        DefaultValueMap defaultValueMap;
        ProtoNode protoNode;

    public:
        explicit ProtoNodeClass(Browser & browser) throw ();
        virtual ~ProtoNodeClass() throw ();

        void addEventIn(FieldValue::Type, const std::string & id)
            throw (std::invalid_argument, std::bad_alloc);
        void addEventOut(FieldValue::Type, const std::string & id)
            throw (std::invalid_argument, std::bad_alloc);
        void addExposedField(const std::string & id,
                             const FieldValuePtr & defaultValue)
            throw (std::invalid_argument, std::bad_alloc);
        void addField(const std::string & id,
                      const FieldValuePtr & defaultValue)
            throw (std::invalid_argument, std::bad_alloc);
        void addRootNode(const NodePtr & node) throw (std::bad_alloc);
        void addIS(Node & implNode,
                   const std::string & implNodeInterfaceId,
                   const std::string & protoInterfaceId)
            throw (UnsupportedInterface, NodeInterfaceTypeMismatch,
                   FieldValueTypeMismatch, std::bad_alloc);

        virtual const NodeTypePtr createType(const std::string & id,
                                             const NodeInterfaceSet &)
            throw (UnsupportedInterface, std::bad_alloc);
    };

    class Vrml97RootScope : public Scope {
    public:
        Vrml97RootScope(const Browser & browser,
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

class NullNodeClass : public NodeClass {
public:
    explicit NullNodeClass(Browser & browser) throw ();
    virtual ~NullNodeClass() throw ();

    virtual const NodeTypePtr createType(const std::string & id,
                                         const NodeInterfaceSet & interfaces)
        throw ();
};


class NullNodeType : public NodeType {
public:
    explicit NullNodeType(NullNodeClass & nodeClass) throw ();
    virtual ~NullNodeType() throw ();

    virtual const NodeInterfaceSet & getInterfaces() const throw ();
    virtual const NodePtr createNode(const ScopePtr & scope) const throw ();
};


class DefaultViewpoint : public ViewpointNode {
    mat4f userViewTransform;

public:
    explicit DefaultViewpoint(const NullNodeType & nodeType) throw ();
    virtual ~DefaultViewpoint() throw ();

    virtual const mat4f & getTransformation() const throw ();
    virtual const mat4f & getUserViewTransform() const throw ();
    virtual void setUserViewTransform(const mat4f & transform) throw ();
    virtual const std::string & getDescription() const throw ();
    virtual float getFieldOfView() const throw ();

private:
    virtual void do_setField(const std::string & id, const FieldValue & value)
        throw ();
    virtual const FieldValue & do_getField(const std::string & id) const
        throw ();
    virtual void do_processEvent(const std::string & id,
                                 const FieldValue & value,
                                 double timestamp)
        throw ();
    virtual const FieldValue & do_getEventOut(const std::string & id) const
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
 * @class InvalidVrml
 *
 * @brief Exception thrown when the parser fails due to errors in the VRML
 *      input.
 */

/**
 * @brief Constructor.
 */
InvalidVrml::InvalidVrml():
    std::runtime_error("Invalid VRML.")
{}

/**
 * @brief Destructor.
 */
InvalidVrml::~InvalidVrml() throw ()
{}


/**
 * @class Browser
 *
 * @brief Encapsulates a VRML browser.
 */

/**
 * @var Browser::Vrml97Parser
 *
 * @brief VRML97 parser generated by ANTLR.
 */

/**
 * @var Browser::ProtoNodeClass
 *
 * @brief Class object for @link ProtoNode ProtoNodes@endlink.
 */

/**
 * @enum Browser::CBReason
 *
 * @brief Valid reasons for browser callback.
 */

/**
 * @var Browser::CBReason Browser::DESTROY_WORLD
 *
 * @brief Destroy the world.
 */

/**
 * @var Browser::CBReason Browser::REPLACE_WORLD
 *
 * @brief Replace the world.
 */

/**
 * @typedef Browser::SceneCB
 *
 * @brief A pointer to a browser callback function.
 *
 * The callback function provoides a way to let the app know when a world is
 * loaded, changed, etc.
 */

/**
 * @typedef Browser::NodeClassMap
 *
 * @brief Maps URIs to @link NodeClass NodeClasses@endlink.
 */

/**
 * @var Browser::NodeClassMap Browser::nodeClassMap
 *
 * @brief A map of URIs to node implementations.
 */

/**
 * @var ScriptNodeClass Browser::scriptNodeClass
 *
 * @brief NodeClass for Script nodes in the browser.
 */

/**
 * @var Scene * Browser::scene
 *
 * @brief Pointer to the root Scene.
 */

/**
 * @typedef Browser::BindStack
 *
 * @brief A list of bound nodes.
 */

/**
 * @var Browser::BindStack Browser::d_navigationInfoStack
 *
 * @brief The stack of bound NavigationInfo nodes.
 */

/**
 * @var Browser::BindStack Browser::d_viewpointStack
 *
 * @brief The stack of bound Viewpoint nodes.
 */

/**
 * @var std::list<Node *> Browser::d_navigationInfos
 *
 * @brief A list of all the NavigationInfo nodes in the browser.
 */

/**
 * @var std::list<Node *> Browser::d_viewpoints
 *
 * @brief A list of all the Viewpoint nodes in the browser.
 */

/**
 * @var std::list<Node *> Browser::d_scopedLights
 *
 * @brief A list of all the scoped light nodes in the browser.
 */

/**
 * @var std::list<ScriptNode *> Browser::d_scripts
 *
 * @brief A list of all the Script nodes in the browser.
 */

/**
 * @var std::list<Node *> Browser::d_timers
 *
 * @brief A list of all the TimeSensor nodes in the browser.
 */

/**
 * @var std::list<Node *> Browser::d_audioClips
 *
 * @brief A list of all the AudioClip nodes in the browser.
 */

/**
 * @var std::list<Node *> Browser::d_movies
 *
 * @brief A list of all the MovieTexture nodes in the browser.
 */

/**
 * @var std::list<Node *> Browser::protoNodeList
 *
 * @brief A list of all the prototype nodes in the browser.
 */

/**
 * @var bool Browser::d_modified
 *
 * @brief Flag to indicate whether the browser has been modified.
 */

/**
 * @var bool Browser::d_newView
 *
 * @brief Flag to indicate if the user has changed to a new view.
 */

/**
 * @var double Browser::d_deltaTime
 *
 * @brief Time elapsed since the last update.
 */

/**
 * @typedef Browser::SceneCBList
 *
 * @brief List of functions to call when the world is changed.
 */

/**
 * @struct Browser::Event
 *
 * @brief An event.
 *
 * An event has a value and a destination, and is associated with a time.
 */

/**
 * @var double Browser::Event::timeStamp
 *
 * @brief The timestamp of the event.
 */

/**
 * @var FieldValue * Browser::Event::value
 *
 * @brief The value associated with the event.
 */

/**
 * @var NodePtr Browser::Event::toNode
 *
 * @brief The Node the event is going to.
 */

/**
 * @var std::string Browser::Event::toEventIn
 *
 * @brief The eventIn of @a toNode the event is going to.
 */

/**
 * @var Browser::SceneCBList Browser::d_sceneCallbacks
 *
 * @brief List of functions to call when the world is changed.
 */

/**
 * @var double Browser::d_frameRate
 *
 * @brief Frame rate.
 */

/**
 * @var Browser::MAXEVENTS
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
 * @var Browser::Event Browser::d_eventMem
 *
 * @brief The event queue.
 *
 * @todo The event queue ought to be sorted by timeStamp.
 */

/**
 * @brief Get the current time.
 */
double Browser::getCurrentTime() throw ()
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
 * @var std::ostream & Browser::out
 *
 * @brief Output stream, generally for console output.
 */

/**
 * @var std::ostream & Browser::err
 *
 * @brief Error output stream.
 */

/**
 * @var bool Browser::d_flags_need_updating
 *
 * @brief Set by Node::setBVolumeDirty on any node in this browser graph,
 *      cleared by updateFlags.
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
Browser::Browser(std::ostream & out, std::ostream & err) throw (std::bad_alloc):
    nullNodeClass(new NullNodeClass(*this)),
    nullNodeType(new NullNodeType(*nullNodeClass)),
    scriptNodeClass(*this),
    scene(0),
    defaultViewpoint(new DefaultViewpoint(*nullNodeType)),
    activeViewpoint(defaultViewpoint->toViewpoint()),
    d_modified(false),
    d_newView(false),
    d_deltaTime(DEFAULT_DELTA),
    d_frameRate(0.0),
    d_firstEvent(0),
    d_lastEvent(0),
    out(out),
    err(err),
    d_flags_need_updating(false)
{
    using std::string;
    using std::vector;
    assert(this->activeViewpoint);
    this->Browser::loadURI(vector<string>(), vector<string>());
}

/**
 * @brief Destructor.
 */
Browser::~Browser() throw ()
{
    const double now = Browser::getCurrentTime();

    if (this->scene) { this->scene->shutdown(now); }
    delete this->scene;
    this->scene = 0;
    this->d_navigationInfoStack.clear();
    assert(this->viewpointList.empty());
    assert(this->d_navigationInfos.empty());
    assert(this->d_scopedLights.empty());
    assert(this->d_scripts.empty());
    assert(this->d_timers.empty());
    assert(this->d_audioClips.empty());
    assert(this->d_movies.empty());
    assert(this->protoNodeList.empty());
    this->nodeClassMap.clear();
}

/**
 * @brief Get the root nodes for the browser.
 *
 * @return the root nodes for the browser.
 */
const std::vector<NodePtr> & Browser::getRootNodes() const throw ()
{
    assert(this->scene);
    return this->scene->getNodes();
}

/**
 * @brief Get the path to a Node in the scene graph.
 *
 * @param node  the objective Node.
 *
 * @return the path to @p node, starting with a root Node, and ending with
 *      @p node. If @p node is not in the scene graph, the returned NodePath is
 *      empty.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
const NodePath Browser::findNode(const Node & node) const throw (std::bad_alloc)
{
    assert(this->scene);

    class FindNodeTraverser : public NodeTraverser {
        const Node & objectiveNode;
        NodePath & nodePath;

    public:
        explicit FindNodeTraverser(const Node & objectiveNode,
                                   NodePath & nodePath) throw ():
            objectiveNode(objectiveNode),
            nodePath(nodePath)
        {}

        virtual ~FindNodeTraverser() throw()
        {}

    private:
        virtual void onEntering(Node & node) throw (std::bad_alloc)
        {
            this->nodePath.push_back(&node);
            if (&node == &this->objectiveNode) { this->haltTraversal(); }
        }

        virtual void onLeaving(Node & node) throw ()
        {
            if (&node != &this->objectiveNode) { this->nodePath.pop_back(); }
        }
    };

    NodePath nodePath;
    FindNodeTraverser(node, nodePath).traverse(this->scene->getNodes());
    return nodePath;
}

/**
 * @brief Get the active ViewpointNode.
 *
 * The active ViewpointNode is the one currently associated with the user view.
 *
 * @return the active ViewpointNode.
 */
ViewpointNode & Browser::getActiveViewpoint() const throw ()
{
    return *this->activeViewpoint;
}

/**
 * @brief Set the active ViewpointNode.
 *
 * @param viewpoint a ViewpointNode.
 */
void Browser::setActiveViewpoint(ViewpointNode & viewpoint) throw ()
{
    this->activeViewpoint = &viewpoint;
}

/**
 * @brief Reset the active ViewpointNode to the default.
 */
void Browser::resetDefaultViewpoint() throw ()
{
    assert(this->defaultViewpoint);
    assert(this->defaultViewpoint->toViewpoint());
    this->activeViewpoint = this->defaultViewpoint->toViewpoint();
}

/**
 * @brief Add a ViewpointNode to the list of ViewpointNodes for the Browser.
 *
 * @param viewpoint a ViewpointNode.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 *
 * @pre @p viewpoint is not in the list of ViewpointNodes for the Browser.
 */
void Browser::addViewpoint(ViewpointNode & viewpoint) throw (std::bad_alloc)
{
    assert(std::find(this->viewpointList.begin(), this->viewpointList.end(),
                     &viewpoint) == this->viewpointList.end());
    this->viewpointList.push_back(&viewpoint);
}

/**
 * @brief Remove a ViewpointNode from the list of ViewpointNodes for the
 *      Browser.
 *
 * @param viewpoint a ViewpointNode.
 *
 * @pre @p viewpoint is in the list of ViewpointNodes for the Browser.
 */
void Browser::removeViewpoint(ViewpointNode & viewpoint) throw ()
{
    assert(!this->viewpointList.empty());
    typedef std::list<ViewpointNode *> ViewpointList;
    const ViewpointList::iterator end = this->viewpointList.end();
    const ViewpointList::iterator pos =
            std::find(this->viewpointList.begin(), end, &viewpoint);
    assert(pos != end);
    this->viewpointList.erase(pos);
}

/**
 * @brief Get the list of @link ViewpointNode ViewpointNodes@endlink for the
 *      world.
 *
 * @return the list of @link ViewpointNode ViewpointNodes@endlink for the
 *      world.
 */
const std::list<ViewpointNode *> & Browser::getViewpoints() const throw ()
{
    return this->viewpointList;
}

/**
 * @brief Get the browser name.
 *
 * @return "%OpenVRML"
 *
 * Specific browsers may wish to override this method.
 */
const char * Browser::getName() const throw () { return "OpenVRML"; }

/**
 * @brief Get the browser version.
 *
 * @return the version of OpenVRML.
 *
 * Specific browsers may wish to override this method.
 */
const char * Browser::getVersion() const throw () { return PACKAGE_VERSION; }

/**
 * @brief Get the average navigation speed in meters per second.
 *
 * @return the average navigation speed.
 */
float Browser::getCurrentSpeed() {
    Vrml97Node::NavigationInfo * const navInfo = bindableNavigationInfoTop();
    if (navInfo) { return navInfo->getSpeed(); }
    return 0.0f;
}

/**
 * @brief Get the URI for the world.
 *
 * @return the URI for the world.
 */
const std::string Browser::getWorldURI() const throw (std::bad_alloc) {
    return this->scene
            ? this->scene->getURI()
            : std::string();
}

/**
 * @todo Implement me!
 */
void Browser::replaceWorld(const std::vector<NodePtr> & nodes) {}

namespace {
    typedef std::map<std::string, NodeClassPtr> NodeClassMap;

    struct InitNodeClass : std::unary_function<void, NodeClassMap::value_type> {
        explicit InitNodeClass(ViewpointNode * initialViewpoint,
                               const double time)
            throw ():
            initialViewpoint(initialViewpoint),
            time(time)
        {}

        void operator()(const NodeClassMap::value_type & value) const throw ()
        {
            assert(value.second);
            value.second->initialize(this->initialViewpoint, this->time);
        }

    private:
        ViewpointNode * initialViewpoint;
        double time;
    };


    struct HasId_ : std::unary_function<bool, ViewpointNode *> {
        explicit HasId_(const std::string & nodeId) throw ():
            nodeId(&nodeId)
        {}

        bool operator()(ViewpointNode * const node) const throw ()
        {
            return node->getId() == *this->nodeId;
        }

    private:
        const std::string * nodeId;
    };


    class URI {
        std::string str;
        enum { nmatch = 11 };
        regmatch_t regmatch[nmatch];

    public:
        URI(const std::string & str) throw (InvalidURI, std::bad_alloc);

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
 * into the Browser.  If the resource at @p uri is not a VRML world, this
 * method as no effect.
 *
 * Implementations should override this method to handle non-VRML resources,
 * handing them off to a Web browser or other appropriate handler.
 *
 * @param uri       a URI.
 * @param parameter paramaters for @p uri.
 *
 * @todo Make this asynchronous.
 */
void Browser::loadURI(const std::vector<std::string> & uri,
                      const std::vector<std::string> & parameter)
    throw (std::bad_alloc)
{
    using std::for_each;
    using std::list;
    using std::string;

    const double now = Browser::getCurrentTime();

    //
    // Clear out the current Scene.
    //
    if (this->scene) { this->scene->shutdown(now); }
    delete this->scene;
    this->scene = 0;
    this->d_navigationInfoStack.clear();
    assert(this->viewpointList.empty());
    assert(this->d_navigationInfos.empty());
    assert(this->d_scopedLights.empty());
    assert(this->d_scripts.empty());
    assert(this->d_timers.empty());
    assert(this->d_audioClips.empty());
    assert(this->d_movies.empty());
    assert(this->protoNodeList.empty());
    this->nodeClassMap.clear();

    //
    // Create the new Scene.
    //
    this->initNodeClassMap();
    this->scene = new Scene(*this, uri);
    this->scene->initialize(now);

    //
    // Get the initial ViewpointNode, if any was specified.
    //
    ViewpointNode * initialViewpoint = 0;
    const string viewpointNodeId = URI(this->scene->getURI()).getFragment();
    if (!viewpointNodeId.empty()) {
        if (!this->scene->getNodes().empty()) {
            const NodePtr & node = this->scene->getNodes()[0];
            if (node) {
                Node * const vp = node->getScope()->findNode(viewpointNodeId);
                initialViewpoint = dynamic_cast<ViewpointNode *>(vp);
            }
        }
    }

    //
    // Initialize the NodeClasses.
    //
    for_each(this->nodeClassMap.begin(), this->nodeClassMap.end(),
             InitNodeClass(initialViewpoint, now));

    //
    // Send initial bind events to bindable nodes.
    //
    if (!this->d_navigationInfos.empty()) {
        assert(this->d_navigationInfos.front());
        this->d_navigationInfos.front()
                ->processEvent("set_bind", SFBool(true), now);
    }

    if (this->activeViewpoint != this->defaultViewpoint->toViewpoint()) {
        this->activeViewpoint->processEvent("set_bind", SFBool(true), now);
    }

    this->setModified();
    this->d_newView = true;		// Force resetUserNav
}

/**
 * @brief Send a string to the user interface.
 *
 * The default implementation of this method simply prints @p description to
 * @a out.  Subclasses can override this method to direct messages to an
 * application's UI; for instance, a status bar.
 *
 * @param description   a string.
 */
void Browser::setDescription(const std::string & description)
{
    this->out << description << std::endl;
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
 * @exception InvalidVrml       if @p in has invalid VRML syntax.
 * @exception std::bad_alloc    if memory allocation fails.
 */
const std::vector<NodePtr> Browser::createVrmlFromStream(std::istream & in) {
    std::vector<NodePtr> nodes;
    try {
        Vrml97Scanner scanner(in);
        Vrml97Parser parser(scanner, "");
        parser.vrmlScene(*this, nodes);
    } catch (antlr::RecognitionException & ex) {
        throw InvalidVrml();
    }
    return nodes;
}

/**
 * @todo Implement me!
 */
void Browser::createVrmlFromURI(const std::vector<std::string> & uri,
                                const NodePtr & node,
                                const std::string & event) {}

/**
 * @brief Initialize the NodeClass map with the available node implementations.
 */
void Browser::initNodeClassMap() {
    using namespace Vrml97Node;
    this->nodeClassMap["urn:X-openvrml:node:Anchor"] =
            NodeClassPtr(new AnchorClass(*this));
    this->nodeClassMap["urn:X-openvrml:node:Appearance"] =
            NodeClassPtr(new AppearanceClass(*this));
    this->nodeClassMap["urn:X-openvrml:node:AudioClip"] =
            NodeClassPtr(new AudioClipClass(*this));
    this->nodeClassMap["urn:X-openvrml:node:Background"] =
            NodeClassPtr(new BackgroundClass(*this));
    this->nodeClassMap["urn:X-openvrml:node:Billboard"] =
            NodeClassPtr(new BillboardClass(*this));
    this->nodeClassMap["urn:X-openvrml:node:Box"] =
            NodeClassPtr(new BoxClass(*this));
    this->nodeClassMap["urn:X-openvrml:node:Collision"] =
            NodeClassPtr(new CollisionClass(*this));
    this->nodeClassMap["urn:X-openvrml:node:Color"] =
            NodeClassPtr(new ColorClass(*this));
    this->nodeClassMap["urn:X-openvrml:node:ColorInterpolator"] =
            NodeClassPtr(new ColorInterpolatorClass(*this));
    this->nodeClassMap["urn:X-openvrml:node:Cone"] =
            NodeClassPtr(new ConeClass(*this));
    this->nodeClassMap["urn:X-openvrml:node:Coordinate"] =
            NodeClassPtr(new CoordinateClass(*this));
    this->nodeClassMap["urn:X-openvrml:node:CoordinateInterpolator"] =
            NodeClassPtr(new CoordinateInterpolatorClass(*this));
    this->nodeClassMap["urn:X-openvrml:node:Cylinder"] =
            NodeClassPtr(new CylinderClass(*this));
    this->nodeClassMap["urn:X-openvrml:node:CylinderSensor"] =
            NodeClassPtr(new CylinderSensorClass(*this));
    this->nodeClassMap["urn:X-openvrml:node:DirectionalLight"] =
            NodeClassPtr(new DirectionalLightClass(*this));
    this->nodeClassMap["urn:X-openvrml:node:ElevationGrid"] =
            NodeClassPtr(new ElevationGridClass(*this));
    this->nodeClassMap["urn:X-openvrml:node:Extrusion"] =
            NodeClassPtr(new ExtrusionClass(*this));
    this->nodeClassMap["urn:X-openvrml:node:Fog"] =
            NodeClassPtr(new FogClass(*this));
    this->nodeClassMap["urn:X-openvrml:node:FontStyle"] =
            NodeClassPtr(new FontStyleClass(*this));
    this->nodeClassMap["urn:X-openvrml:node:Group"] =
            NodeClassPtr(new GroupClass(*this));
    this->nodeClassMap["urn:X-openvrml:node:ImageTexture"] =
            NodeClassPtr(new ImageTextureClass(*this));
    this->nodeClassMap["urn:X-openvrml:node:IndexedFaceSet"] =
            NodeClassPtr(new IndexedFaceSetClass(*this));
    this->nodeClassMap["urn:X-openvrml:node:IndexedLineSet"] =
            NodeClassPtr(new IndexedLineSetClass(*this));
    this->nodeClassMap["urn:X-openvrml:node:Inline"] =
            NodeClassPtr(new InlineClass(*this));
    this->nodeClassMap["urn:X-openvrml:node:LOD"] =
            NodeClassPtr(new LODClass(*this));
    this->nodeClassMap["urn:X-openvrml:node:Material"] =
            NodeClassPtr(new MaterialClass(*this));
    this->nodeClassMap["urn:X-openvrml:node:MovieTexture"] =
            NodeClassPtr(new MovieTextureClass(*this));
    this->nodeClassMap["urn:X-openvrml:node:NavigationInfo"] =
            NodeClassPtr(new NavigationInfoClass(*this));
    this->nodeClassMap["urn:X-openvrml:node:Normal"] =
            NodeClassPtr(new NormalClass(*this));
    this->nodeClassMap["urn:X-openvrml:node:NormalInterpolator"] =
            NodeClassPtr(new NormalInterpolatorClass(*this));
    this->nodeClassMap["urn:X-openvrml:node:OrientationInterpolator"] =
            NodeClassPtr(new OrientationInterpolatorClass(*this));
    this->nodeClassMap["urn:X-openvrml:node:PixelTexture"] =
            NodeClassPtr(new PixelTextureClass(*this));
    this->nodeClassMap["urn:X-openvrml:node:PlaneSensor"] =
            NodeClassPtr(new PlaneSensorClass(*this));
    this->nodeClassMap["urn:X-openvrml:node:PointLight"] =
            NodeClassPtr(new PointLightClass(*this));
    this->nodeClassMap["urn:X-openvrml:node:PointSet"] =
            NodeClassPtr(new PointSetClass(*this));
    this->nodeClassMap["urn:X-openvrml:node:PositionInterpolator"] =
            NodeClassPtr(new PositionInterpolatorClass(*this));
    this->nodeClassMap["urn:X-openvrml:node:ProximitySensor"] =
            NodeClassPtr(new ProximitySensorClass(*this));
    this->nodeClassMap["urn:X-openvrml:node:ScalarInterpolator"] =
            NodeClassPtr(new ScalarInterpolatorClass(*this));
    this->nodeClassMap["urn:X-openvrml:node:Shape"] =
            NodeClassPtr(new ShapeClass(*this));
    this->nodeClassMap["urn:X-openvrml:node:Sound"] =
            NodeClassPtr(new SoundClass(*this));
    this->nodeClassMap["urn:X-openvrml:node:Sphere"] =
            NodeClassPtr(new SphereClass(*this));
    this->nodeClassMap["urn:X-openvrml:node:SphereSensor"] =
            NodeClassPtr(new SphereSensorClass(*this));
    this->nodeClassMap["urn:X-openvrml:node:SpotLight"] =
            NodeClassPtr(new SpotLightClass(*this));
    this->nodeClassMap["urn:X-openvrml:node:Switch"] =
            NodeClassPtr(new SwitchClass(*this));
    this->nodeClassMap["urn:X-openvrml:node:Text"] =
            NodeClassPtr(new TextClass(*this));
    this->nodeClassMap["urn:X-openvrml:node:TextureCoordinate"] =
            NodeClassPtr(new TextureCoordinateClass(*this));
    this->nodeClassMap["urn:X-openvrml:node:TextureTransform"] =
            NodeClassPtr(new TextureTransformClass(*this));
    this->nodeClassMap["urn:X-openvrml:node:TimeSensor"] =
            NodeClassPtr(new TimeSensorClass(*this));
    this->nodeClassMap["urn:X-openvrml:node:TouchSensor"] =
            NodeClassPtr(new TouchSensorClass(*this));
    this->nodeClassMap["urn:X-openvrml:node:Transform"] =
            NodeClassPtr(new TransformClass(*this));
    this->nodeClassMap["urn:X-openvrml:node:Viewpoint"] =
            NodeClassPtr(new ViewpointClass(*this));
    this->nodeClassMap["urn:X-openvrml:node:VisibilitySensor"] =
            NodeClassPtr(new VisibilitySensorClass(*this));
    this->nodeClassMap["urn:X-openvrml:node:WorldInfo"] =
            NodeClassPtr(new WorldInfoClass(*this));
}

/**
 * @brief Execute browser callback functions.
 *
 * @param reason    the CBReason to pass to the callback functions.
 */
void Browser::doCallbacks(const CBReason reason) {
  SceneCBList::iterator cb, cbend = d_sceneCallbacks.end();
  for (cb = d_sceneCallbacks.begin(); cb != cbend; ++cb)
    (*cb)( reason );

}

/**
 * @brief Add a callback function to be called when the world changes.
 *
 * @param cb    a browser callback function.
 */
void Browser::addWorldChangedCallback(const SceneCB cb) {
    this->d_sceneCallbacks.push_front(cb);
}

/**
 * @brief Get the current frame rate.
 *
 * @return the current frame rate.
 */
double Browser::getFrameRate() const { return this->d_frameRate; }

/**
 * @brief Queue an event for a node.
 *
 * Current events are in the array @a d_eventMem[d_firstEvent,d_lastEvent). If
 * @a d_firstEvent == @a d_lastEvent, the queue is empty. There is a fixed
 * maximum number of events. If we are so far behind that the queue is filled,
 * the oldest events get overwritten.
 */
void Browser::queueEvent(double timeStamp, FieldValue * value,
			   const NodePtr & toNode,
			   const std::string & toEventIn) {
    Event * e = &this->d_eventMem[this->d_lastEvent];
    e->timeStamp = timeStamp;
    e->value = value;
    e->toNode = toNode;
    e->toEventIn = toEventIn;
    d_lastEvent = (d_lastEvent+1) % MAXEVENTS;

    // If the event queue is full, discard the oldest (in terms of when it
    // was put on the queue, not necessarily in terms of earliest timestamp).
    if (d_lastEvent == d_firstEvent) {
        e = &d_eventMem[d_lastEvent];
        delete e->value;
        d_firstEvent = (d_firstEvent+1) % MAXEVENTS;
    }
}

/**
 * @brief Check if any events are waiting to be distributed.
 *
 * @return @c true if there are pending events, @c false otherwise.
 */
bool Browser::eventsPending() {
    return this->d_firstEvent != this->d_lastEvent;
}


/**
 * @brief Discard all pending events
 */
void Browser::flushEvents()
{
  while (d_firstEvent != d_lastEvent)
    {
      Event *e = &d_eventMem[d_firstEvent];
      d_firstEvent = (d_firstEvent+1) % MAXEVENTS;
      delete e->value;
    }
}

/**
 * Called by the viewer when the cursor passes over, clicks, drags, or
 * releases a sensitive object (an Anchor or another grouping node with
 * an enabled TouchSensor child).
 */
void Browser::sensitiveEvent(Node * const n,
                             const double timeStamp,
                             const bool isOver, const bool isActive,
                             double * const point) {
    if (n) {
        Vrml97Node::Anchor * a = n->toAnchor();
        if (a) {
            //
            // This should really be (isOver && !isActive && n->wasActive)
            // (ie, button up over the anchor after button down over the anchor)
            //
            if (isActive && isOver) {
                a->activate();
            }
        } else {
            //
            // The parent grouping node is registered for Touch/Drag Sensors.
            //
            GroupingNode * const g = n->toGrouping();
            if (g) {
                g->activate(timeStamp, isOver, isActive, point);
                setModified();
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
bool Browser::update(double currentTime) {
    if (currentTime <= 0.0) { currentTime = Browser::getCurrentTime(); }

    d_deltaTime = DEFAULT_DELTA;

    // Update each of the timers.
    std::list<Node *>::iterator i, end = this->d_timers.end();
    for (i = this->d_timers.begin(); i != end; ++i) {
        Vrml97Node::TimeSensor * t = (*i)->toTimeSensor();
        if (t) { t->update(currentTime); }
    }

    // Update each of the clips.
    end = this->d_audioClips.end();
    for (i = this->d_audioClips.begin(); i != end; ++i) {
        Vrml97Node::AudioClip * c = (*i)->toAudioClip();
        if (c) { c->update(currentTime); }
    }

    // Update each of the movies.
    end = this->d_movies.end();
    for (i = this->d_movies.begin(); i != end; ++i) {
        Vrml97Node::MovieTexture * m = (*i)->toMovieTexture();
        if (m) { m->update(currentTime); }
    }

    //
    // Update each of the scripts.
    //
    std::for_each(this->d_scripts.begin(), this->d_scripts.end(),
                  UpdatePolledNode_<ScriptNode *>(currentTime));

    //
    // Update each of the prototype instances.
    //
    std::for_each(this->protoNodeList.begin(), this->protoNodeList.end(),
                  UpdatePolledNode_<ProtoNode *>(currentTime));

    // Pass along events to their destinations
    while (this->d_firstEvent != this->d_lastEvent) {
        Event * const e = &this->d_eventMem[this->d_firstEvent];
        this->d_firstEvent = (this->d_firstEvent + 1) % MAXEVENTS;

        e->toNode->processEvent(e->toEventIn, *e->value, e->timeStamp);

        // this needs to change if event values are shared...
        delete e->value;
    }

    // Signal a redisplay if necessary
    return isModified();
}

bool Browser::headlightOn() {
    Vrml97Node::NavigationInfo * const navInfo = bindableNavigationInfoTop();
    if (navInfo) { return navInfo->getHeadlightOn(); }
    return true;
}

namespace {

    struct RenderNodeClass :
            std::unary_function<void, NodeClassMap::value_type> {
        explicit RenderNodeClass(Viewer & viewer):
            viewer(&viewer)
        {}

        void operator()(const NodeClassMap::value_type & value) const
        {
            value.second->render(*this->viewer);
        }

    private:
        Viewer * viewer;
    };
}

/**
 * @brief Draw this browser into the specified viewer
 */
void Browser::render(Viewer & viewer)
{
    if (d_newView) {
        viewer.resetUserNavigation();
        d_newView = false;
    }
    float avatarSize = 0.25;
    float visibilityLimit = 0.0;
    Vrml97Node::NavigationInfo * ni = bindableNavigationInfoTop();
    if (ni) {
        avatarSize = ni->getAvatarSize()[0];
        visibilityLimit = ni->getVisibilityLimit();
    }

    // Activate the headlight.
    // ambient is supposed to be 0 according to the spec...
    if (headlightOn()) {
        const color color(1.0, 1.0, 1.0);
        const vec3f direction(0.0, 0.0, -1.0);
        const float ambientIntensity = 0.3;
        const float intensity = 1.0;

        viewer.insertDirLight(ambientIntensity, intensity, color, direction);
    }

    // sets the viewpoint transformation
    //
    const mat4f t = this->activeViewpoint->getUserViewTransform()
                    * this->activeViewpoint->getTransformation();
    vec3f position, scale;
    rotation orientation;
    t.transformation(position, orientation, scale);
    viewer.setViewpoint(position,
                        orientation,
                        activeViewpoint->getFieldOfView(),
                        avatarSize,
                        visibilityLimit);

    std::for_each(this->nodeClassMap.begin(), this->nodeClassMap.end(),
                  RenderNodeClass(viewer));

    // Top level object

    viewer.beginObject(0);
    mat4f modelview = t.inverse();
    VrmlRenderContext rc(BVolume::partial, modelview);
    rc.setDrawBSpheres(true);

    // Do the browser-level lights (Points and Spots)
    std::list<Node *>::iterator li, end = this->d_scopedLights.end();
    for (li = this->d_scopedLights.begin(); li != end; ++li) {
        Vrml97Node::AbstractLight * x = (*li)->toLight();
        if (x) { x->renderScoped(viewer); }
    }

    // Render the nodes
    assert(this->scene);
    this->scene->render(viewer, rc);

    viewer.endObject();

    // This is actually one frame late...
    d_frameRate = viewer.getFrameRate();

    clearModified();

    // If any events were generated during render (ugly...) do an update
    if (eventsPending()) { setDelta(0.0); }
}

/**
 * @brief Indicate that the browser state has changed; need to rerender.
 */
void Browser::setModified() { this->d_modified = true; }

/**
 * @brief Clear the "modified" flag for the browser.
 */
void Browser::clearModified() { this->d_modified = false; }

/**
 * @brief Check if the browser has been modified.
 *
 * @return @c true if the browser has been modified, @c false otherwise.
 */
bool Browser::isModified() const { return this->d_modified; }

/**
 * @brief Set the time until the next update is needed.
 *
 * @param d a time interval.
 */
void Browser::setDelta(const double d) {
    if (d < this->d_deltaTime) { this->d_deltaTime = d; }
}

/**
 * @brief Get the time interval between browser updates.
 *
 * @return the time interval between browser updates.
 */
double Browser::getDelta() const { return this->d_deltaTime; }

/**
 * @brief Get the top node of a BindStack.
 *
 * @return the top node of @p stack.
 */
const NodePtr Browser::bindableTop(const BindStack & stack) {
    return stack.empty() ? NodePtr(0) : stack.front();
}

/**
 * @brief Push a node onto a BindStack.
 *
 * @param stack the BindStack onto which to push @p node.
 * @param node  the Node to push onto @p stack.
 */
void Browser::bindablePush(BindStack & stack, const NodePtr & node) {
    bindableRemove( stack, node ); // Remove any existing reference
    stack.push_front(node);
    setModified();
}

/**
 * @brief Remove a node from a BindStack.
 *
 * @param stack the BindStack from which to remove @p node.
 * @param node  the Node to remove from @p stack.
 */
void Browser::bindableRemove(BindStack & stack, const NodePtr & node) {
    const BindStack::iterator pos = std::find(stack.begin(), stack.end(), node);
    if (pos != stack.end()) {
        stack.erase(pos);
        this->setModified();
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
void Browser::addNavigationInfo(Vrml97Node::NavigationInfo & node) {
    assert(std::find(this->d_navigationInfos.begin(),
                     this->d_navigationInfos.end(), &node)
            == this->d_navigationInfos.end());
    this->d_navigationInfos.push_back(&node);
}

/**
 * @brief Remove a NavigationInfo node from the list of NavigationInfo nodes
 *      for the browser.
 *
 * @param node  a NavigationInfo node.
 *
 * @pre @p node is in the list of NavigationInfo nodes for the browser.
 */
void Browser::removeNavigationInfo(Vrml97Node::NavigationInfo & node) {
    assert(!this->d_navigationInfos.empty());
    const std::list<Node *>::iterator end = this->d_navigationInfos.end();
    const std::list<Node *>::iterator pos =
            std::find(this->d_navigationInfos.begin(), end, &node);
    assert(pos != end);
    this->d_navigationInfos.erase(pos);
}

/**
 * @brief Get the active node on the bound NavigationInfo stack.
 *
 * @return the active node on the bound NavigationInfo stack.
 */
Vrml97Node::NavigationInfo * Browser::bindableNavigationInfoTop() {
    Node * const n = bindableTop(d_navigationInfoStack).get();
    return n ? n->toNavigationInfo() : 0;
}

/**
 * @brief Push a NavigationInfo node onto the bound NavigationInfo node stack.
 *
 * @param n a NavigationInfo node.
 */
void Browser::bindablePush(Vrml97Node::NavigationInfo * n) {
    bindablePush(d_navigationInfoStack, NodePtr(n));
}

/**
 * @brief Remove a NavigationInfo node from the bound NavigationInfo node stack.
 *
 * @param n a NavigationInfo node.
 */
void Browser::bindableRemove(Vrml97Node::NavigationInfo * n) {
    bindableRemove(d_navigationInfoStack, NodePtr(n));
}

/**
 * @brief Add a scoped light node to the browser.
 *
 * @param light a light node.
 *
 * @pre @p light is not in the list of light nodes for the browser.
 */
void Browser::addScopedLight(Vrml97Node::AbstractLight & light) {
    assert(std::find(this->d_scopedLights.begin(), this->d_scopedLights.end(),
                     &light) == this->d_scopedLights.end());
    this->d_scopedLights.push_back(&light);
}

/**
 * @brief Remove a scoped light node from the browser.
 *
 * @param light the light node to remove.
 *
 * @pre @p light is in the list of light nodes for the browser.
 */
void Browser::removeScopedLight(Vrml97Node::AbstractLight & light) {
    assert(!this->d_scopedLights.empty());
    const std::list<Node *>::iterator end = this->d_scopedLights.end();
    const std::list<Node *>::iterator pos =
            std::find(this->d_scopedLights.begin(), end, &light);
    assert(pos != end);
    this->d_scopedLights.erase(pos);
}

/**
 * @brief Add a MovieTexture node to the browser.
 *
 * @param movie a MovieTexture node.
 *
 * @pre @p movie is not in the list of MovieTexture nodes for the browser.
 */
void Browser::addMovie(Vrml97Node::MovieTexture & movie) {
    assert(std::find(this->d_movies.begin(), this->d_movies.end(), &movie)
            == this->d_movies.end());
    this->d_movies.push_back(&movie);
}

/**
 * @brief Remove a MovieTexture node from the browser.
 *
 * @param movie the MovieTexture node to remove.
 *
 * @pre @p movie is in the list of MovieTexture nodes for the browser.
 */
void Browser::removeMovie(Vrml97Node::MovieTexture & movie) {
    assert(!this->d_movies.empty());
    const std::list<Node *>::iterator end = this->d_movies.end();
    const std::list<Node *>::iterator pos =
            std::find(this->d_movies.begin(), end, &movie);
    assert(pos != end);
    this->d_movies.erase(pos);
}

/**
 * @brief Add a Script node to the browser.
 *
 * @param script    a Script node.
 *
 * @pre @p script is not in the list of Script nodes for the browser.
 */
void Browser::addScript(ScriptNode & script) {
    assert(std::find(this->d_scripts.begin(), this->d_scripts.end(), &script)
            == this->d_scripts.end());
    this->d_scripts.push_back(&script);
}

/**
 * @brief Remove a Script node from the browser.
 *
 * @param script    the Script node to remove.
 *
 * @pre @p script is in the list of Script nodes for the browser.
 */
void Browser::removeScript(ScriptNode & script) {
    assert(!this->d_scripts.empty());
    typedef std::list<ScriptNode *> ScriptNodeList;
    const ScriptNodeList::iterator end = this->d_scripts.end();
    const ScriptNodeList::iterator pos =
            std::find(this->d_scripts.begin(), end, &script);
    assert(pos != end);
    this->d_scripts.erase(pos);
}

/**
 * @brief Add a PROTO instance to the browser.
 *
 * @param node  a PROTO instance.
 *
 * @pre @p node is not in the list of prototype instances for the browser.
 */
void Browser::addProto(ProtoNode & node) {
    assert(std::find(this->protoNodeList.begin(), this->protoNodeList.end(),
                     &node) == this->protoNodeList.end());
    this->protoNodeList.push_back(&node);
}

/**
 * @brief Remove a PROTO instance from the browser.
 *
 * @param node  the PROTO instance to remove.
 *
 * @pre @p node exists in the browser's list of prototype instances.
 */
void Browser::removeProto(ProtoNode & node) {
    assert(!this->protoNodeList.empty());
    typedef std::list<ProtoNode *> ProtoNodeList;
    const ProtoNodeList::iterator end = this->protoNodeList.end();
    const ProtoNodeList::iterator pos =
            std::find(this->protoNodeList.begin(), end, &node);
    assert(pos != end);
    this->protoNodeList.erase(pos);
}

/**
 * @brief Add a TimeSensor node to the browser.
 *
 * @param timer a TimeSensor node.
 *
 * @pre @p timer is not in the list of TimeSensor nodes for the browser.
 */
void Browser::addTimeSensor(Vrml97Node::TimeSensor & timer) {
    assert(std::find(this->d_timers.begin(), this->d_timers.end(), &timer)
            == this->d_timers.end());
    this->d_timers.push_back(&timer);
}

/**
 * @brief Remove a TimeSensor node from the browser.
 *
 * @param timer the TimeSensor node to remove.
 *
 * @pre @p timer is in the list of TimeSensor nodes for the browser.
 */
void Browser::removeTimeSensor(Vrml97Node::TimeSensor & timer) {
    assert(!this->d_timers.empty());
    const std::list<Node *>::iterator end = this->d_timers.end();
    const std::list<Node *>::iterator pos =
            std::find(this->d_timers.begin(), end, &timer);
    assert(pos != end);
    this->d_timers.erase(pos);
}


/**
 * @brief Add an AudioClip node to the browser.
 *
 * @param audio_clip    an AudioClip node.
 *
 * @pre @p audio_clip is not in the list of AudioClip nodes for the browser.
 */
void Browser::addAudioClip(Vrml97Node::AudioClip & audio_clip) {
    assert(std::find(this->d_audioClips.begin(), this->d_audioClips.end(),
                     &audio_clip) == this->d_audioClips.end());
    this->d_audioClips.push_back(&audio_clip);
}

/**
 * @brief Remove an AudioClip node from the browser.
 *
 * @param audio_clip    the AudioClip node to remove.
 *
 * @pre @p audio_clip is in the list of AudioClip nodes for the browser.
 */
void Browser::removeAudioClip(Vrml97Node::AudioClip & audio_clip) {
    assert(!this->d_audioClips.empty());
    const std::list<Node *>::iterator end = this->d_audioClips.end();
    const std::list<Node *>::iterator pos =
            std::find(this->d_audioClips.begin(), end, &audio_clip);
    assert(pos != end);
    this->d_audioClips.erase(pos);
}


/**
 * @brief Propagate the bvolume dirty flag from children to ancestors.
 *
 * The invariant is that if a node's bounding volume is out of date,
 * then the bounding volumes of all that nodes's ancestors must be
 * out of date. However, Node does not maintain a parent pointer. So
 * we must do a traversal of the entire browser graph to do the propagation.
 *
 * @see Node::setBVolumeDirty
 * @see Node::isBVolumeDirty
 */
void Browser::updateFlags()
{
//  Node* root = this->getRoot();
//  root->updateModified(0x002);
}


/**
 * @class BadURI
 *
 * @brief Thrown when there is a problem resolving a URI.
 */

/**
 * @brief Constructor.
 *
 * @param message   Informative text.
 */
BadURI::BadURI(const std::string & message): std::runtime_error(message) {}

/**
 * @brief Destructor.
 */
BadURI::~BadURI() throw () {}


/**
 * @class InvalidURI
 *
 * @brief Thrown when parsing a URI fails.
 */

/**
 * @brief Constructor.
 */
InvalidURI::InvalidURI(): BadURI("Invalid URI.") {}

/**
 * @brief Destructor.
 */
InvalidURI::~InvalidURI() throw () {}


/**
 * @class UnreachableURI
 *
 * @brief Thrown when a URI cannot be reached.
 */

/**
 * @brief Constructor.
 */
UnreachableURI::UnreachableURI(): BadURI("Unreachable URI.") {}

/**
 * @brief Destructor.
 */
UnreachableURI::~UnreachableURI() throw () {}


/**
 * @class Scene
 *
 * @brief A scene in the VRML world.
 */

/**
 * @var MFNode Scene::nodes
 *
 * @brief The nodes for the scene.
 */

/**
 * @var const std::string Scene::uri
 *
 * @brief The URI for the scene.
 *
 * @a uri may be a relative or an absolute reference.
 */

/**
 * @var Browser & Scene::browser
 *
 * @brief A reference to the Browser associated with the Scene.
 */

/**
 * @var Scene * const Scene::parent
 *
 * @brief A pointer to the parent scene.
 *
 * If the Scene is the root Scene, @a parent will be 0.
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
 * @brief Construct a Scene from a URI.
 *
 * @param browser   the Browser associated with the Scene.
 * @param uri       the URI for the Scene.
 * @param parent    the parent Scene.
 *
 * @exception InvalidVrml       if there is a syntax error in the VRML input.
 * @exception std::bad_alloc    if memory allocation fails.
 */
Scene::Scene(Browser & browser,
             const std::vector<std::string> & uri,
             Scene * parent)
    throw (InvalidVrml, std::bad_alloc):
    browser(browser),
    parent(parent)
{
    std::string absoluteURI;
    for (size_t i = 0; i < uri.size(); ++i) {
        try {
            //
            // Throw InvalidURI if it isn't a valid URI.
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
                absoluteURI = testURI.resolveAgainst(URI(parent->getURI()));
            }

            Doc2 doc(absoluteURI);
            std::istream & in = doc.inputStream();
            if (!in) { throw UnreachableURI(); }
            try {
                Vrml97Scanner scanner(in);
                Vrml97Parser parser(scanner, this->getURI());
                parser.vrmlScene(browser, this->nodes);
            } catch (antlr::RecognitionException & ex) {
                throw InvalidVrml();
            } catch (std::bad_alloc &) {
                throw;
            } catch (...) {
                throw UnreachableURI();
            }
        } catch (BadURI & ex) {
            browser.err << ex.what() << std::endl;
            continue;
        }
        //
        // If this is the root scene (that is, the parent is null), then
        // this->uri must be the absolute URI.
        //
        this->uri = parent
                  ? uri[i]
                  : absoluteURI;
        break;
    }
}

/**
 * @brief Get the absolute URI for the Scene.
 *
 * @return the absolute URI for the Scene.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
const std::string Scene::getURI() const throw (std::bad_alloc) {
    using std::string;
    return this->parent
            ? string(URI(this->uri).resolveAgainst(URI(this->parent->getURI())))
            : this->uri;
}

/**
 * @brief Initialize the scene.
 *
 * @param timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void Scene::initialize(const double timestamp) throw (std::bad_alloc)
{
    for (std::vector<NodePtr>::iterator node(this->nodes.begin());
            node != this->nodes.end(); ++node) {
        assert(*node);
        (*node)->initialize(*this, timestamp);
        (*node)->relocate();
    }
}

/**
 * @brief Render the Scene.
 *
 * @param viewer    a Viewer to render to.
 * @param context   a VrmlRenderContext.
 */
void Scene::render(Viewer & viewer, VrmlRenderContext context) {
    for (std::vector<NodePtr>::iterator node(this->nodes.begin());
            node != this->nodes.end(); ++node) {
        assert(*node);
        (*node)->render(viewer, context);
    }
}

/**
 * @brief Load a resource into @a browser.
 *
 * This method simply resolves any relative references in @p uri and calls
 * Browser::loadURI.
 *
 * @note There are a couple of edge cases here where we are probably doing the
 *      wrong thing:
 *       - If there is a URI in the list of the form "#NodeId" and it is not the
 *         first URI in the list, this URI will be loaded as if it were a new
 *         world rather than as a Viewpoint that should simply be bound.
 *       - If the first URI in the list is of the form "#NodeId" and no
 *         Viewpoint named "NodeId" exists in the scene, this method will not
 *         try any subsequent URIs in the list.
 *
 * @param uri       an array of URIs. Per VRML97 convention, the first resource
 *                  in the sequence that can be reached will be loaded into the
 *                  Browser.
 * @param parameter an array of parameters to be associated with the URIs in
 *                  @p uri.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 *
 * @todo This method currently fails silently if any of the URIs in @p uri is
 *      invalid. Should this throw InvalidURI?
 */
void Scene::loadURI(const std::vector<std::string> & uri,
                    const std::vector<std::string> & parameter)
    throw (std::bad_alloc)
{
    using std::string;

    if (!uri.empty()) {
        if (uri[0][0] == '#') {
# if 0
            //
            // If the first element in uri is a Viewpoint name, bind the
            // Viewpoint.
            //
            this->browser.setViewpoint(uri[0].substr(1));
# endif
        } else {
            std::vector<std::string> absoluteURIs(uri.size());
            for (size_t i = 0; i < absoluteURIs.size(); ++i) {
                try {
                    const URI uriElement(uri[i]);
                    const string value =
                        uriElement.getScheme().empty()
                            ? uriElement.resolveAgainst(URI(this->getURI()))
                            : uriElement;
                    absoluteURIs[i] = value;
                } catch (InvalidURI & ex) {
                    OPENVRML_PRINT_EXCEPTION_(ex);
                }
            }
            this->browser.loadURI(absoluteURIs, parameter);
        }
    }
}

/**
 * @brief Shut down the nodes in the Scene.
 *
 * This function @b must be called before the Scene is destroyed.
 *
 * @param timestamp the current time.
 */
void Scene::shutdown(const double timestamp) throw ()
{
    for (std::vector<NodePtr>::iterator node(this->nodes.begin());
            node != this->nodes.end(); ++node) {
        if (*node) { (*node)->shutdown(timestamp); }
    }
}


namespace {
    const std::string toString(const NodeInterface::Type interfaceType)
    {
        std::ostringstream out;
        out << interfaceType;
        return out.str();
    }
}

NodeInterfaceTypeMismatch::NodeInterfaceTypeMismatch(
        const NodeInterface::Type lhs, const NodeInterface::Type rhs):
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
ProtoNode::ImplNodeInterface::ImplNodeInterface(Node & node,
                                                const std::string & interfaceId):
        node(node), interfaceId(interfaceId) {}

namespace {
    const FieldValuePtr defaultFieldValue(const FieldValue::Type fieldType) {
        switch (fieldType) {
        case FieldValue::sfbool:        return FieldValuePtr(new SFBool);
        case FieldValue::sfcolor:       return FieldValuePtr(new SFColor);
        case FieldValue::sffloat:       return FieldValuePtr(new SFFloat);
        case FieldValue::sfimage:       return FieldValuePtr(new SFImage);
        case FieldValue::sfint32:       return FieldValuePtr(new SFInt32);
        case FieldValue::sfnode:        return FieldValuePtr(new SFNode);
        case FieldValue::sfrotation:    return FieldValuePtr(new SFRotation);
        case FieldValue::sfstring:      return FieldValuePtr(new SFString);
        case FieldValue::sftime:        return FieldValuePtr(new SFTime);
        case FieldValue::sfvec2f:       return FieldValuePtr(new SFVec2f);
        case FieldValue::sfvec3f:       return FieldValuePtr(new SFVec3f);
        case FieldValue::mfcolor:       return FieldValuePtr(new MFColor);
        case FieldValue::mffloat:       return FieldValuePtr(new MFFloat);
        case FieldValue::mfint32:       return FieldValuePtr(new MFInt32);
        case FieldValue::mfnode:        return FieldValuePtr(new MFNode);
        case FieldValue::mfrotation:    return FieldValuePtr(new MFRotation);
        case FieldValue::mfstring:      return FieldValuePtr(new MFString);
        case FieldValue::mftime:        return FieldValuePtr(new MFTime);
        case FieldValue::mfvec2f:       return FieldValuePtr(new MFVec2f);
        case FieldValue::mfvec3f:       return FieldValuePtr(new MFVec3f);
        default: assert(false);
        }
        return FieldValuePtr(0);
    }

    struct AddEventOutValue_ : std::unary_function<NodeInterface, void> {
        AddEventOutValue_(ProtoNode::EventOutValueMap & eventOutValueMap):
                eventOutValueMap(&eventOutValueMap) {}

        void operator()(const NodeInterface & interface) const {
            if (interface.type == NodeInterface::eventOut) {
                const Node::PolledEventOutValue
                    eventOutValue(defaultFieldValue(interface.fieldType), 0.0);
                const ProtoNode::EventOutValueMap::value_type
                        value(interface.id, eventOutValue);
                const bool succeeded =
                        this->eventOutValueMap->insert(value).second;
                assert(succeeded);
            } else if (interface.type == NodeInterface::exposedField) {
                const Node::PolledEventOutValue
                    eventOutValue(defaultFieldValue(interface.fieldType), 0.0);
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
ProtoNode::ProtoNode(const NodeType & nodeType) throw (std::bad_alloc):
    Node(nodeType, ScopePtr(0))
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
ProtoNode::ProtoNode(const NodeType & nodeType,
                     const ScopePtr & scope,
                     const ProtoNode & node) throw (std::bad_alloc):
    Node(nodeType, scope)
{
    assert(!node.implNodes.empty());
    assert(node.implNodes[0]);

    //
    // For each exposedField and eventOut in the prototype interface, add
    // a value to the eventOutValueMap.
    //
    // Note: We don't want to copy node's EventOutValueMap, since the values
    // in that map should be per-instance.
    //
    const NodeInterfaceSet & interfaces = this->nodeType.getInterfaces();
    std::for_each(interfaces.begin(), interfaces.end(),
                  AddEventOutValue_(this->eventOutValueMap));

    //
    // Cloning the nodes is a two-step process. First, we clone the actual
    // node instances. Second, we traverse the node tree again cloning the
    // routes.
    //
    class ProtoImplCloner {
        std::set<Node *> traversedNodes;
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
            const std::vector<NodePtr> & protoDefImplNodes =
                    protoDef.getImplNodes();
            for (std::vector<NodePtr>::const_iterator node =
                    protoDefImplNodes.begin();
                    node != protoDefImplNodes.end(); ++node) {
                const NodePtr newNode =
                        this->cloneNode(*node, protoInstance.getScope());
                assert(newNode);
                protoInstance.addRootNode(newNode);
            }
            this->protoDef = 0;
            this->protoInstance = 0;
            this->traversedNodes.clear();
        }

    private:
        const SFNode cloneFieldValue(const SFNode & node,
                                     const ScopePtr & targetScope)
            throw (std::bad_alloc)
        {
            return SFNode(this->cloneNode(node.value, targetScope));
        }

        const MFNode cloneFieldValue(const MFNode & nodes,
                                     const ScopePtr & targetScope)
            throw (std::bad_alloc)
        {
            MFNode result(nodes.value.size());
            for (size_t i = 0; i < nodes.value.size(); ++i) {
                result.value[i] = this->cloneNode(nodes.value[i], targetScope);
            }
            return result;
        }

        const NodePtr cloneNode(const NodePtr & node,
                                const ScopePtr & targetScope)
            throw (std::bad_alloc)
        {
            assert(targetScope);

            NodePtr result(0);

            if (!node) { return result; }

            std::set<Node *>::iterator pos =
                    this->traversedNodes.find(node.get());
            const bool alreadyTraversed = (pos != this->traversedNodes.end());

            if (alreadyTraversed) {
                result.reset(targetScope->findNode(node->getId()));
                assert(result);
            } else {
                result = node->nodeType.createNode(targetScope);
                if (!node->getId().empty()) { result->setId(node->getId()); }

                //
                // Any IS mappings for this node?
                //
                for (ProtoNode::ISMap::const_iterator isMapEntry =
                        this->protoDef->isMap.begin();
                        isMapEntry != this->protoDef->isMap.end();
                        ++isMapEntry) {
                    if (&isMapEntry->second.node == node.get()) {
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

                const NodeInterfaceSet & interfaces =
                        node->nodeType.getInterfaces();
                for (NodeInterfaceSet::const_iterator interface =
                        interfaces.begin();
                        interface != interfaces.end();
                        ++interface) {
                    try {
                        if (interface->type == NodeInterface::exposedField
                                || interface->type == NodeInterface::field) {
                            if (interface->fieldType == FieldValue::sfnode) {
                                const SFNode & value =
                                        static_cast<const SFNode &>
                                            (node->getField(interface->id));
                                result->setField(interface->id,
                                    this->cloneFieldValue(value, targetScope));
                            } else if (interface->fieldType
                                    == FieldValue::mfnode) {
                                const MFNode & value =
                                        static_cast<const MFNode &>
                                            (node->getField(interface->id));
                                result->setField(interface->id,
                                    this->cloneFieldValue(value, targetScope));
                            } else {
                                result->setField(interface->id,
                                                 node->getField(interface->id));
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

    protoImplCloner.clone(node, *this);

    class RouteCopyTraverser : public NodeTraverser {
        const Scope & targetScope;

    public:
        RouteCopyTraverser(const Scope & targetScope):
            targetScope(targetScope)
        {}

        virtual ~RouteCopyTraverser() throw ()
        {}

    private:
        virtual void onEntering(Node & node)
        {
            const std::string & fromNodeId = node.getId();
            if (!fromNodeId.empty()) {
                Node * const fromNode = this->targetScope.findNode(fromNodeId);
                assert(fromNode);
                const Node::RouteList & routes = node.getRoutes();
                for (Node::RouteList::const_iterator route = routes.begin();
                        route != routes.end(); ++route) {
                    const std::string & toNodeId = route->toNode->getId();
                    const NodePtr toNode(this->targetScope.findNode(toNodeId));
                    assert(toNode);
                    fromNode->addRoute(route->fromEventOut,
                                       toNode,
                                       route->toEventIn);
                }
            }
        }
    };

    RouteCopyTraverser(*scope).traverse(node.getImplNodes());

    //
    // Finally, we initialize the implementation using the PROTO's default
    // field values.
    //
    class NodeFieldCloner {
        std::set<Node *> traversedNodes;

    public:
        const SFNode clone(const SFNode & node,
                           const ScopePtr & targetScope)
            throw (std::bad_alloc)
        {
            assert(this->traversedNodes.empty());
            const SFNode result = this->cloneFieldValue(node, targetScope);
            this->traversedNodes.clear();
            return result;
        }

        const MFNode clone(const MFNode & nodes,
                           const ScopePtr & targetScope)
            throw (std::bad_alloc)
        {
            assert(this->traversedNodes.empty());
            const MFNode result = this->cloneFieldValue(nodes, targetScope);
            this->traversedNodes.clear();
            return result;
        }

    private:
        const SFNode cloneFieldValue(const SFNode & node,
                                     const ScopePtr & targetScope)
            throw (std::bad_alloc)
        {
            return SFNode(this->cloneNode(node.value, targetScope));
        }

        const MFNode cloneFieldValue(const MFNode & nodes,
                                     const ScopePtr & targetScope)
            throw (std::bad_alloc)
        {
            MFNode result(nodes.value.size());
            for (size_t i = 0; i < nodes.value.size(); ++i) {
                result.value[i] = this->cloneNode(nodes.value[i], targetScope);
            }
            return result;
        }

        const NodePtr cloneNode(const NodePtr & node,
                                const ScopePtr & targetScope)
            throw (std::bad_alloc)
        {
            assert(targetScope);

            NodePtr result(0);

            if (!node) { return result; }

            std::set<Node *>::iterator pos =
                    this->traversedNodes.find(node.get());
            const bool alreadyTraversed = (pos != this->traversedNodes.end());

            if (alreadyTraversed) {
                result.reset(targetScope->findNode(node->getId()));
                assert(result);
            } else {
                result = node->nodeType.createNode(targetScope);
                if (!node->getId().empty()) { result->setId(node->getId()); }

                const NodeInterfaceSet & interfaces =
                        node->nodeType.getInterfaces();
                for (NodeInterfaceSet::const_iterator interface =
                        interfaces.begin();
                        interface != interfaces.end();
                        ++interface) {
                    try {
                        if (interface->type == NodeInterface::exposedField
                                || interface->type == NodeInterface::field) {
                            if (interface->fieldType == FieldValue::sfnode) {
                                const SFNode & value =
                                        static_cast<const SFNode &>
                                            (node->getField(interface->id));
                                result->setField(interface->id,
                                    this->cloneFieldValue(value, targetScope));
                            } else if (interface->fieldType
                                    == FieldValue::mfnode) {
                                const MFNode & value =
                                        static_cast<const MFNode &>
                                            (node->getField(interface->id));
                                result->setField(interface->id,
                                    this->cloneFieldValue(value, targetScope));
                            } else {
                                result->setField(interface->id,
                                                 node->getField(interface->id));
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
            static_cast<ProtoNodeClass &>(nodeType.nodeClass).defaultValueMap;
    const ScopePtr & protoScope = this->implNodes[0]->getScope();

    for (DefaultValueMap::const_iterator i(defaultValueMap.begin());
            i != defaultValueMap.end(); ++i) {
        //
        // Node field values need to be cloned; everything else we can just
        // copy.
        //
        FieldValuePtr fieldValue;
        const FieldValue::Type type = i->second->type();
        if (type == FieldValue::sfnode) {
            const SFNode & node = static_cast<const SFNode &>(*i->second);
            fieldValue.reset(new SFNode(nodeCloner.clone(node, protoScope)));
        } else if (type == FieldValue::mfnode) {
            const MFNode & nodes = static_cast<const MFNode &>(*i->second);
            fieldValue.reset(new MFNode(nodeCloner.clone(nodes, protoScope)));
        } else {
            fieldValue = i->second;
        }

        const std::pair<ISMap::iterator, ISMap::iterator> rangeItrs =
                this->isMap.equal_range(i->first);
        for (ISMap::iterator j(rangeItrs.first); j != rangeItrs.second; ++j) {
            j->second.node.setField(j->second.interfaceId, *fieldValue);
        }

        //
        // If we have a field that isn't IS'd to anything in the
        // implementation, we need to store it's value specially.
        //
        if (this->nodeType.hasField(i->first)
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
 * @brief Cast to a ScriptNode.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a ScriptNode, or 0 otherwise.
 */
const ScriptNode * ProtoNode::toScript() const throw ()
{
    assert(!this->implNodes.empty());
    assert(this->implNodes[0]);
    return this->implNodes[0]->toScript();
}

/**
 * @brief Cast to a ScriptNode.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a ScriptNode, or 0 otherwise.
 */
ScriptNode * ProtoNode::toScript() throw ()
{
    assert(!this->implNodes.empty());
    assert(this->implNodes[0]);
    return this->implNodes[0]->toScript();
}

/**
 * @brief Cast to an AppearanceNode.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      an AppearanceNode, or 0 otherwise.
 */
const AppearanceNode * ProtoNode::toAppearance() const throw ()
{
    assert(!this->implNodes.empty());
    assert(this->implNodes[0]);
    return this->implNodes[0]->toAppearance();
}

/**
 * @brief Cast to an AppearanceNode.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      an AppearanceNode, or 0 otherwise.
 */
AppearanceNode * ProtoNode::toAppearance() throw ()
{
    assert(!this->implNodes.empty());
    assert(this->implNodes[0]);
    return this->implNodes[0]->toAppearance();
}

/**
 * @brief Cast to a ChildNode.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a ChildNode, or 0 otherwise.
 */
const ChildNode * ProtoNode::toChild() const throw ()
{
    assert(!this->implNodes.empty());
    assert(this->implNodes[0]);
    return this->implNodes[0]->toChild();
}

/**
 * @brief Cast to a ChildNode.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a ChildNode, or 0 otherwise.
 */
ChildNode * ProtoNode::toChild() throw ()
{
    assert(!this->implNodes.empty());
    assert(this->implNodes[0]);
    return this->implNodes[0]->toChild();
}

/**
 * @brief Cast to a ColorNode.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a ColorNode, or 0 otherwise.
 */
const ColorNode * ProtoNode::toColor() const throw ()
{
    assert(!this->implNodes.empty());
    assert(this->implNodes[0]);
    return this->implNodes[0]->toColor();
}

/**
 * @brief Cast to a ColorNode.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a ColorNode, or 0 otherwise.
 */
ColorNode * ProtoNode::toColor() throw ()
{
    assert(!this->implNodes.empty());
    assert(this->implNodes[0]);
    return this->implNodes[0]->toColor();
}

/**
 * @brief Cast to a CoordinateNode.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a CoordinateNode, or 0 otherwise.
 */
const CoordinateNode * ProtoNode::toCoordinate() const throw ()
{
    assert(!this->implNodes.empty());
    assert(this->implNodes[0]);
    return this->implNodes[0]->toCoordinate();
}

/**
 * @brief Cast to a CoordinateNode.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a CoordinateNode, or 0 otherwise.
 */
CoordinateNode * ProtoNode::toCoordinate() throw ()
{
    assert(!this->implNodes.empty());
    assert(this->implNodes[0]);
    return this->implNodes[0]->toCoordinate();
}

/**
 * @brief Cast to a FontStyleNode.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a FontStyleNode, or 0 otherwise.
 */
const FontStyleNode * ProtoNode::toFontStyle() const throw ()
{
    assert(!this->implNodes.empty());
    assert(this->implNodes[0]);
    return this->implNodes[0]->toFontStyle();
}

/**
 * @brief Cast to a FontStyleNode.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a FontStyleNode, or 0 otherwise.
 */
FontStyleNode * ProtoNode::toFontStyle() throw ()
{
    assert(!this->implNodes.empty());
    assert(this->implNodes[0]);
    return this->implNodes[0]->toFontStyle();
}

/**
 * @brief Cast to a GeometryNode.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a GeometryNode, or 0 otherwise.
 */
const GeometryNode * ProtoNode::toGeometry() const throw ()
{
    assert(!this->implNodes.empty());
    assert(this->implNodes[0]);
    return this->implNodes[0]->toGeometry();
}

/**
 * @brief Cast to a GeometryNode.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a GeometryNode, or 0 otherwise.
 */
GeometryNode * ProtoNode::toGeometry() throw ()
{
    assert(!this->implNodes.empty());
    assert(this->implNodes[0]);
    return this->implNodes[0]->toGeometry();
}

/**
 * @brief Cast to a MaterialNode.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a MaterialNode, or 0 otherwise.
 */
const MaterialNode * ProtoNode::toMaterial() const throw ()
{
    assert(!this->implNodes.empty());
    assert(this->implNodes[0]);
    return this->implNodes[0]->toMaterial();
}

/**
 * @brief Cast to a MaterialNode.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a MaterialNode, or 0 otherwise.
 */
MaterialNode * ProtoNode::toMaterial() throw ()
{
    assert(!this->implNodes.empty());
    assert(this->implNodes[0]);
    return this->implNodes[0]->toMaterial();
}

/**
 * @brief Cast to a NormalNode.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a NormalNode, or 0 otherwise.
 */
const NormalNode * ProtoNode::toNormal() const throw ()
{
    assert(!this->implNodes.empty());
    assert(this->implNodes[0]);
    return this->implNodes[0]->toNormal();
}

/**
 * @brief Cast to a NormalNode.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a NormalNode, or 0 otherwise.
 */
NormalNode * ProtoNode::toNormal() throw ()
{
    assert(!this->implNodes.empty());
    assert(this->implNodes[0]);
    return this->implNodes[0]->toNormal();
}

/**
 * @brief Cast to a SoundSourceNode.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a SoundSourceNode, or 0 otherwise.
 */
const SoundSourceNode * ProtoNode::toSoundSource() const throw ()
{
    assert(!this->implNodes.empty());
    assert(this->implNodes[0]);
    return this->implNodes[0]->toSoundSource();
}

/**
 * @brief Cast to a SoundSourceNode.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a SoundSourceNode, or 0 otherwise.
 */
SoundSourceNode * ProtoNode::toSoundSource() throw ()
{
    assert(!this->implNodes.empty());
    assert(this->implNodes[0]);
    return this->implNodes[0]->toSoundSource();
}

/**
 * @brief Cast to a TextureNode.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a TextureNode, or 0 otherwise.
 */
const TextureNode * ProtoNode::toTexture() const throw ()
{
    assert(!this->implNodes.empty());
    assert(this->implNodes[0]);
    return this->implNodes[0]->toTexture();
}

/**
 * @brief Cast to a TextureNode.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a TextureNode, or 0 otherwise.
 */
TextureNode * ProtoNode::toTexture() throw ()
{
    assert(!this->implNodes.empty());
    assert(this->implNodes[0]);
    return this->implNodes[0]->toTexture();
}

/**
 * @brief Cast to a TextureCoordinateNode.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a TextureCoordinateNode, or 0 otherwise.
 */
const TextureCoordinateNode * ProtoNode::toTextureCoordinate() const throw ()
{
    assert(!this->implNodes.empty());
    assert(this->implNodes[0]);
    return this->implNodes[0]->toTextureCoordinate();
}

/**
 * @brief Cast to a TextureCoordinateNode.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a TextureCoordinateNode, or 0 otherwise.
 */
TextureCoordinateNode * ProtoNode::toTextureCoordinate() throw ()
{
    assert(!this->implNodes.empty());
    assert(this->implNodes[0]);
    return this->implNodes[0]->toTextureCoordinate();
}

/**
 * @brief Cast to a TextureTransformNode.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a TextureTransformNode, or 0 otherwise.
 */
const TextureTransformNode * ProtoNode::toTextureTransform() const throw ()
{
    assert(!this->implNodes.empty());
    assert(this->implNodes[0]);
    return this->implNodes[0]->toTextureTransform();
}

/**
 * @brief Cast to a TextureTransformNode.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a TextureTransformNode, or 0 otherwise.
 */
TextureTransformNode * ProtoNode::toTextureTransform() throw ()
{
    assert(!this->implNodes.empty());
    assert(this->implNodes[0]);
    return this->implNodes[0]->toTextureTransform();
}

/**
 * @brief Cast to a TransformNode.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a TransformNode, or 0 otherwise.
 */
const TransformNode * ProtoNode::toTransform() const throw ()
{
    assert(!this->implNodes.empty());
    assert(this->implNodes[0]);
    return this->implNodes[0]->toTransform();
}

/**
 * @brief Cast to a TransformNode.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a TransformNode, or 0 otherwise.
 */
TransformNode * ProtoNode::toTransform() throw ()
{
    assert(!this->implNodes.empty());
    assert(this->implNodes[0]);
    return this->implNodes[0]->toTransform();
}

/**
 * @brief Cast to a ViewpointNode.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a ViewpointNode, or 0 otherwise.
 */
const ViewpointNode * ProtoNode::toViewpoint() const throw ()
{
    assert(!this->implNodes.empty());
    assert(this->implNodes[0]);
    return this->implNodes[0]->toViewpoint();
}

/**
 * @brief Cast to a ViewpointNode.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a ViewpointNode, or 0 otherwise.
 */
ViewpointNode * ProtoNode::toViewpoint() throw ()
{
    assert(!this->implNodes.empty());
    assert(this->implNodes[0]);
    return this->implNodes[0]->toViewpoint();
}

/**
 * @brief Get the implementation nodes.
 *
 * @return the implementation Nodes
 */
const std::vector<NodePtr> & ProtoNode::getImplNodes() const throw ()
{
    return this->implNodes;
}

/**
 * @brief Add a root node to the prototype definition.
 */
void ProtoNode::addRootNode(const NodePtr & node) throw (std::bad_alloc)
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
 * @exception UnsupportedInterface      if @p implNode has no interface
 *                                      @p implNodeInterfaceId, or if the
 *                                      prototype has no interface
 *                                      @p protoInterfaceId.
 * @exception NodeInterfaceTypeMismatch if the two interface types are
 *                                      incompatible.
 * @exception FieldValueTypeMismatch    if the field types of the interfaces are
 *                                      not identical.
 * @exception std::bad_alloc            if memory allocation fails.
 *
 * @see http://www.web3d.org/technicalinfo/specifications/vrml97/part1/concepts.html#Table4.4
 *
 * @todo Rewrite this method. Check for interface type, field type, agreement
 *      here and throw exceptions on failure.
 */
void ProtoNode::addIS(Node & implNode,
                      const std::string & implNodeInterfaceId,
                      const std::string & protoInterfaceId)
    throw (UnsupportedInterface, NodeInterfaceTypeMismatch,
           FieldValueTypeMismatch, std::bad_alloc)
{
    using std::find_if;
    using std::pair;

    //
    // Make sure the IS is legitimate. First, get the interface type of the
    // implementation node's interface.
    //
    const NodeInterfaceSet::const_iterator implNodeInterface =
        implNode.nodeType.getInterfaces().findInterface(implNodeInterfaceId);
    if (implNodeInterface == implNode.nodeType.getInterfaces().end()) {
        throw UnsupportedInterface(implNode.nodeType, implNodeInterfaceId);
    }

    //
    // The rhs of the IS mapping must be an *exact* match for one of the
    // PROTO's interfaces; so, we don't use NodeInterfaceSet::findInterface.
    //
    const NodeInterfaceSet & protoInterfaces = this->nodeType.getInterfaces();
    const NodeInterfaceSet::const_iterator protoInterface =
            find_if(protoInterfaces.begin(), protoInterfaces.end(),
                    interface_id_equals_(protoInterfaceId));
    if (protoInterface == protoInterfaces.end()) {
        throw UnsupportedInterface(this->nodeType, protoInterfaceId);
    }

    //
    // Make sure the interface types agree.
    //
    if (implNodeInterface->type != NodeInterface::exposedField
            && implNodeInterface->type != protoInterface->type) {
        throw NodeInterfaceTypeMismatch(implNodeInterface->type,
                                        protoInterface->type);
    }

    //
    // Make sure the field value types agree.
    //
    if (implNodeInterface->fieldType != protoInterface->fieldType) {
        throw FieldValueTypeMismatch();
    }

    //
    // Add the IS.
    //
    const ImplNodeInterface implNodeInterfaceRef(implNode, implNodeInterfaceId);
    const ISMap::value_type value(protoInterfaceId, implNodeInterfaceRef);
    this->isMap.insert(value);

    if (protoInterface->type == NodeInterface::eventOut) {
        EventOutValueMap::iterator pos =
                this->eventOutValueMap.find(protoInterfaceId);
        if (pos == this->eventOutValueMap.end()) {
            pos = this->eventOutValueMap.find(protoInterfaceId + "_changed");
        }
        assert(pos != this->eventOutValueMap.end());
        implNode.addEventOutIS(implNodeInterfaceId, pos->second);
    }
}

void ProtoNode::update(const double currentTime) {
    //
    // For each modified eventOut, send an event.
    //
    for (EventOutValueMap::iterator itr = eventOutValueMap.begin();
            itr != eventOutValueMap.end(); ++itr) {
        if (itr->second.modified) {
            this->emitEvent(itr->first, *itr->second.value, currentTime);
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
    assert(this->getScene());

    for (size_t i = 0; i < this->implNodes.size(); ++i) {
        assert(this->implNodes[i]);
        this->implNodes[i]->initialize(*this->getScene(), timestamp);
    }

    this->getScene()->browser.addProto(*this);
}

void ProtoNode::do_setField(const std::string & id,
                            const FieldValue & value)
    throw (UnsupportedInterface, std::bad_cast, std::bad_alloc)
{
    const std::pair<ISMap::iterator, ISMap::iterator> rangeItrs =
            this->isMap.equal_range(id);
    for (ISMap::iterator itr(rangeItrs.first); itr != rangeItrs.second; ++itr) {
        itr->second.node.setField(itr->second.interfaceId, value);
    }

    if (rangeItrs.second == rangeItrs.first) {
        //
        // The field hasn't been IS'd to anything.  If it's an exposedField,
        // the value is in eventOutValueMap; if it's a field, the value is in
        // unISdFieldValueMap.
        //
        const NodeInterfaceSet & interfaces = this->nodeType.getInterfaces();
        const NodeInterfaceSet::const_iterator interface =
                interfaces.findInterface(id);
        if (interface == interfaces.end()) {
            throw UnsupportedInterface(this->nodeType,
                                       NodeInterface::field,
                                       id);
        }
        if (interface->type == NodeInterface::exposedField) {
            assert(this->eventOutValueMap.find(id + "_changed")
                    != this->eventOutValueMap.end());
            this->eventOutValueMap[id + "_changed"].value->assign(value);
        } else if (interface->type == NodeInterface::field) {
            assert(this->unISdFieldValueMap.find(id)
                    != this->unISdFieldValueMap.end());
            this->unISdFieldValueMap[id]->assign(value);
        } else {
            throw UnsupportedInterface(this->nodeType,
                                       NodeInterface::field,
                                       id);
        }
    }
}

const FieldValue & ProtoNode::do_getField(const std::string & id) const
    throw (UnsupportedInterface)
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
        if (this->nodeType.hasExposedField(id)) {
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
                throw UnsupportedInterface(this->nodeType.id
                                       + " node has no field \"" + id + "\".");
            }
            assert(pos->second);
            return *pos->second;
        }
    }
    return pos->second.node.getField(pos->second.interfaceId);
}

namespace {

    /**
     * @internal
     */
    struct DispatchEvent_ :
            std::unary_function<ProtoNode::ISMap::value_type, void> {
        DispatchEvent_(const FieldValue & value, const double timestamp):
            value(&value),
            timestamp(timestamp)
        {}

        void operator()(const ProtoNode::ISMap::value_type & value) const
        {
            value.second.node.processEvent(value.second.interfaceId,
                                           *this->value, this->timestamp);
        }

    private:
        const FieldValue * value;
        double timestamp;
    };
}

void ProtoNode::do_processEvent(const std::string & id,
                                const FieldValue & value,
                                const double timestamp)
    throw (UnsupportedInterface, std::bad_cast, std::bad_alloc)
{
    const std::pair<ISMap::iterator, ISMap::iterator> rangeItrs =
            this->isMap.equal_range(id);
    if (rangeItrs.first == this->isMap.end()) {
        throw UnsupportedInterface(this->nodeType, NodeInterface::eventIn, id);
    }
    std::for_each(rangeItrs.first, rangeItrs.second,
                  DispatchEvent_(value, timestamp));

    //
    // Emit events.
    //
    for (EventOutValueMap::iterator itr(this->eventOutValueMap.begin());
            itr != this->eventOutValueMap.end(); ++itr) {
        if (itr->second.modified) {
            this->emitEvent(itr->first, *itr->second.value, timestamp);
            itr->second.modified = false;
        }
    }
}

const FieldValue & ProtoNode::do_getEventOut(const std::string & id) const
        throw (UnsupportedInterface) {
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
            throw UnsupportedInterface(this->nodeType.id + " node has no "
                                       "eventOut \"" + id + "\".");
        }
        return pos->second.node.getEventOut(id);
    }
}

/**
 * @brief Shut down.
 *
 * @param timestamp the current time.
 */
void ProtoNode::do_shutdown(const double timestamp) throw ()
{
    assert(this->getScene());
    this->getScene()->browser.removeProto(*this);

    for (size_t i = 0; i < this->implNodes.size(); ++i) {
        assert(this->implNodes[i]);
        this->implNodes[i]->shutdown(timestamp);
    }
}

Vrml97Node::Anchor * ProtoNode::toAnchor() const {
    return this->implNodes[0]->toAnchor();
}

Vrml97Node::AudioClip * ProtoNode::toAudioClip() const {
    return this->implNodes[0]->toAudioClip();
}

Vrml97Node::CylinderSensor * ProtoNode::toCylinderSensor() const {
    return this->implNodes[0]->toCylinderSensor();
}

Vrml97Node::AbstractLight * ProtoNode::toLight() const {
    return this->implNodes[0]->toLight();
}

Vrml97Node::MovieTexture * ProtoNode::toMovieTexture() const {
    return this->implNodes[0]->toMovieTexture();
}

Vrml97Node::NavigationInfo * ProtoNode::toNavigationInfo() const {
    return this->implNodes[0]->toNavigationInfo();
}

Vrml97Node::PlaneSensor * ProtoNode::toPlaneSensor() const {
    return this->implNodes[0]->toPlaneSensor();
}

Vrml97Node::PointLight * ProtoNode::toPointLight() const {
    return this->implNodes[0]->toPointLight();
}

Vrml97Node::SphereSensor * ProtoNode::toSphereSensor() const {
    return this->implNodes[0]->toSphereSensor();
}

Vrml97Node::SpotLight * ProtoNode::toSpotLight() const {
    return this->implNodes[0]->toSpotLight();
}

Vrml97Node::TimeSensor * ProtoNode::toTimeSensor() const {
    return this->implNodes[0]->toTimeSensor();
}

Vrml97Node::TouchSensor * ProtoNode::toTouchSensor() const {
    return this->implNodes[0]->toTouchSensor();
}

void ProtoNode::render(Viewer & viewer, const VrmlRenderContext context) {
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
        throw (UnsupportedInterface, std::bad_alloc): NodeType(nodeClass, id) {}

/**
 * @brief Destructor.
 */
ProtoNodeClass::ProtoNodeType::~ProtoNodeType() throw () {}

/**
 * @brief Get the list of interfaces.
 *
 * @return the list of interfaces for nodes of this type.
 */
const NodeInterfaceSet & ProtoNodeClass::ProtoNodeType::getInterfaces() const
        throw () {
    return this->nodeInterfaces;
}

/**
 * @brief Create a Node of this type.
 *
 * @return a NodePtr to a new Node.
 */
const NodePtr
        ProtoNodeClass::ProtoNodeType::createNode(const ScopePtr & scope) const
        throw (std::bad_alloc) {
    return NodePtr(new ProtoNode(*this, scope,
                                 static_cast<ProtoNodeClass &>(this->nodeClass)
                                    .protoNode));
}

/**
 * @brief Add an interface.
 */
void ProtoNodeClass::ProtoNodeType::addInterface(const NodeInterface & interface)
        throw (std::invalid_argument, std::bad_alloc) {
    this->nodeInterfaces.add(interface);
}

/**
 * @var ProtoNodeClass::ProtoNodeType ProtoNodeClass::protoNodeType
 *
 * @brief This NodeType includes the full set of
 *      @link NodeInterface NodeInterfaces@endlink supported by the
 *      ProtoNodeClass.
 *
 * A ProtoNodeType instance is used for this instead of a NodeInterfaceSet as
 * a matter of convenience: we need to give @a protoNode a NodeType object.
 */

/**
 * @var std::map<std::string, FieldValuePtr> ProtoNodeClass::defaultValueMap
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
 * @param browser the Browser associated with this NodeClass.
 */
ProtoNodeClass::ProtoNodeClass(Browser & browser) throw ():
        NodeClass(browser), protoNodeType(*this, ""),
        protoNode(protoNodeType) {}

/**
 * @brief Destructor.
 */
ProtoNodeClass::~ProtoNodeClass() throw () {}

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
void ProtoNodeClass::addEventIn(const FieldValue::Type type,
                                const std::string & id)
        throw (std::invalid_argument, std::bad_alloc) {
    const NodeInterface interface(NodeInterface::eventIn, type, id);
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
void ProtoNodeClass::addEventOut(const FieldValue::Type type,
                                 const std::string & id)
        throw (std::invalid_argument, std::bad_alloc) {
    const NodeInterface interface(NodeInterface::eventOut, type, id);
    this->protoNodeType.addInterface(interface);

    //
    // Add a value to the ProtoNode's eventOutValueMap.
    //
    const Node::PolledEventOutValue
            eventOutValue(defaultFieldValue(interface.fieldType), false);
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
                                     const FieldValuePtr & defaultValue)
        throw (std::invalid_argument, std::bad_alloc) {
    const NodeInterface
            interface(NodeInterface::exposedField, defaultValue->type(), id);
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
        const Node::PolledEventOutValue
                eventOutValue(defaultFieldValue(interface.fieldType), 0.0);
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
                              const FieldValuePtr & defaultValue)
        throw (std::invalid_argument, std::bad_alloc) {
    const NodeInterface
            interface(NodeInterface::field, defaultValue->type(), id);
    this->protoNodeType.addInterface(interface);
    const DefaultValueMap::value_type value(id, defaultValue);
    const bool succeeded = this->defaultValueMap.insert(value).second;
    assert(succeeded);
}

/**
 * @brief Add a root node to the prototype definition.
 *
 * @param node  a NodePtr to a non-NULL node.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void ProtoNodeClass::addRootNode(const NodePtr & node) throw (std::bad_alloc) {
    this->protoNode.addRootNode(node);
}

/**
 * @brief Add an IS mapping to the prototype definition.
 *
 * @param implNode              a node in the protoype implementation.
 * @param implNodeInterfaceId   an interface of @p implNode.
 * @param protoInterfaceId      an interface of the prototype.
 *
 * @exception UnsupportedInterface      if @p implNode has no interface
 *                                      @p implNodeInterfaceId, or if the
 *                                      prototype has no interface
 *                                      @p protoInterfaceId.
 * @exception NodeInterfaceTypeMismatch if the two interface types are
 *                                      incompatible.
 * @exception FieldValueTypeMismatch    if the field types of the interfaces are
 *                                      not identical.
 * @exception std::bad_alloc            if memory allocation fails.
 */
void ProtoNodeClass::addIS(Node & implNode,
                           const std::string & implNodeInterfaceId,
                           const std::string & protoInterfaceId)
    throw (UnsupportedInterface, NodeInterfaceTypeMismatch,
           FieldValueTypeMismatch, std::bad_alloc)
{
    this->protoNode.addIS(implNode, implNodeInterfaceId, protoInterfaceId);
}

namespace {
    struct AddInterface_ : std::unary_function<NodeInterface, void> {
        AddInterface_(ProtoNodeClass::ProtoNodeType & protoNodeType):
                protoNodeType(&protoNodeType) {}

        void operator()(const NodeInterface & interface) const {
            protoNodeType->addInterface(interface);
        }

    private:
        ProtoNodeClass::ProtoNodeType * protoNodeType;
    };
}
/**
 * @brief Create a new NodeType.
 */
const NodeTypePtr
        ProtoNodeClass::createType(const std::string & id,
                                   const NodeInterfaceSet & interfaces)
        throw (UnsupportedInterface, std::bad_alloc) {
    const NodeTypePtr nodeType(new ProtoNodeType(*this, id));
    try {
        std::for_each(interfaces.begin(), interfaces.end(),
                      AddInterface_(static_cast<ProtoNodeType &>(*nodeType)));
    } catch (UnsupportedInterface &) {
        throw;
    } catch (std::invalid_argument & ex) {
        throw UnsupportedInterface(ex.what());
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
    class AddInterfaceToSet_ : std::unary_function<NodeInterface, void> {
        NodeInterfaceSet & nodeInterfaceSet;

    public:
        AddInterfaceToSet_(NodeInterfaceSet & nodeInterfaceSet):
                nodeInterfaceSet(nodeInterfaceSet) {}

        void operator()(const NodeInterface & nodeInterface) const
                throw (std::invalid_argument, std::bad_alloc) {
            this->nodeInterfaceSet.add(nodeInterface);
        }
    };

    /**
     * @internal
     */
    class Vrml97NodeInterfaceSet_ : public NodeInterfaceSet {
    public:
        Vrml97NodeInterfaceSet_(const NodeInterface * const begin,
                                const NodeInterface * const end) {
            std::for_each(begin, end, AddInterfaceToSet_(*this));
        }
    };
}

/**
 * @brief Constructor.
 *
 * @param browser   the Browser object.
 * @param uri       the URI associated with the stream being read.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
Vrml97RootScope::Vrml97RootScope(const Browser & browser,
                                 const std::string & uri)
        throw (std::bad_alloc):
        Scope(uri) {
    const Browser::NodeClassMap & nodeClassMap = browser.nodeClassMap;
    Browser::NodeClassMap::const_iterator pos;

    //
    // Anchor node
    //
    static const NodeInterface anchorInterfaces[] = {
        NodeInterface(NodeInterface::eventIn, FieldValue::mfnode, "addChildren"),
        NodeInterface(NodeInterface::eventIn, FieldValue::mfnode, "removeChildren"),
        NodeInterface(NodeInterface::exposedField, FieldValue::mfnode, "children"),
        NodeInterface(NodeInterface::exposedField, FieldValue::sfstring, "description"),
        NodeInterface(NodeInterface::exposedField, FieldValue::mfstring, "parameter"),
        NodeInterface(NodeInterface::exposedField, FieldValue::mfstring, "url"),
        NodeInterface(NodeInterface::field, FieldValue::sfvec3f, "bboxCenter"),
        NodeInterface(NodeInterface::field, FieldValue::sfvec3f, "bboxSize")
    };
    static const Vrml97NodeInterfaceSet_
            anchorInterfaceSet(anchorInterfaces, anchorInterfaces + 8);
    pos = nodeClassMap.find("urn:X-openvrml:node:Anchor");
    assert(pos != nodeClassMap.end());
    this->addNodeType(pos->second->createType("Anchor", anchorInterfaceSet));

    //
    // Appearance node
    //
    static const NodeInterface appearanceInterfaces[] = {
        NodeInterface(NodeInterface::exposedField, FieldValue::sfnode, "material"),
        NodeInterface(NodeInterface::exposedField, FieldValue::sfnode, "texture"),
        NodeInterface(NodeInterface::exposedField, FieldValue::sfnode, "textureTransform")
    };
    static const Vrml97NodeInterfaceSet_
            appearanceInterfaceSet(appearanceInterfaces,
                                   appearanceInterfaces + 3);
    pos = nodeClassMap.find("urn:X-openvrml:node:Appearance");
    assert(pos != nodeClassMap.end());
    this->addNodeType(pos->second->createType("Appearance",
                                              appearanceInterfaceSet));

    //
    // AudioClip node
    //
    static const NodeInterface audioClipInterfaces[] = {
        NodeInterface(NodeInterface::exposedField, FieldValue::sfstring, "description"),
        NodeInterface(NodeInterface::exposedField, FieldValue::sfbool, "loop"),
        NodeInterface(NodeInterface::exposedField, FieldValue::sffloat, "pitch"),
        NodeInterface(NodeInterface::exposedField, FieldValue::sftime, "startTime"),
        NodeInterface(NodeInterface::exposedField, FieldValue::sftime, "stopTime"),
        NodeInterface(NodeInterface::exposedField, FieldValue::mfstring, "url"),
        NodeInterface(NodeInterface::eventOut, FieldValue::sftime, "duration_changed"),
        NodeInterface(NodeInterface::eventOut, FieldValue::sfbool, "isActive")
    };
    static const Vrml97NodeInterfaceSet_
            audioClipInterfaceSet(audioClipInterfaces,
                                  audioClipInterfaces + 8);
    pos = nodeClassMap.find("urn:X-openvrml:node:AudioClip");
    assert(pos != nodeClassMap.end());
    this->addNodeType(pos->second->createType("AudioClip",
                                              audioClipInterfaceSet));

    //
    // Background node
    //
    static const NodeInterface backgroundInterfaces[] = {
        NodeInterface(NodeInterface::eventIn, FieldValue::sfbool, "set_bind"),
        NodeInterface(NodeInterface::exposedField, FieldValue::mffloat, "groundAngle"),
        NodeInterface(NodeInterface::exposedField, FieldValue::mfcolor, "groundColor"),
        NodeInterface(NodeInterface::exposedField, FieldValue::mfstring, "backUrl"),
        NodeInterface(NodeInterface::exposedField, FieldValue::mfstring, "bottomUrl"),
        NodeInterface(NodeInterface::exposedField, FieldValue::mfstring, "frontUrl"),
        NodeInterface(NodeInterface::exposedField, FieldValue::mfstring, "leftUrl"),
        NodeInterface(NodeInterface::exposedField, FieldValue::mfstring, "rightUrl"),
        NodeInterface(NodeInterface::exposedField, FieldValue::mfstring, "topUrl"),
        NodeInterface(NodeInterface::exposedField, FieldValue::mffloat, "skyAngle"),
        NodeInterface(NodeInterface::exposedField, FieldValue::mfcolor, "skyColor"),
        NodeInterface(NodeInterface::eventOut, FieldValue::sfbool, "isBound")
    };
    static const Vrml97NodeInterfaceSet_
            backgroundInterfaceSet(backgroundInterfaces,
                                  backgroundInterfaces + 12);
    pos = nodeClassMap.find("urn:X-openvrml:node:Background");
    assert(pos != nodeClassMap.end());
    this->addNodeType(pos->second->createType("Background",
                                              backgroundInterfaceSet));

    //
    // Billboard node
    //
    static const NodeInterface billboardInterfaces[] = {
        NodeInterface(NodeInterface::eventIn, FieldValue::mfnode, "addChildren"),
        NodeInterface(NodeInterface::eventIn, FieldValue::mfnode, "removeChildren"),
        NodeInterface(NodeInterface::exposedField, FieldValue::sfvec3f, "axisOfRotation"),
        NodeInterface(NodeInterface::exposedField, FieldValue::mfnode, "children"),
        NodeInterface(NodeInterface::field, FieldValue::sfvec3f, "bboxCenter"),
        NodeInterface(NodeInterface::field, FieldValue::sfvec3f, "bboxSize")
    };
    static const Vrml97NodeInterfaceSet_
            billboardInterfaceSet(billboardInterfaces,
                                  billboardInterfaces + 6);
    pos = nodeClassMap.find("urn:X-openvrml:node:Billboard");
    assert(pos != nodeClassMap.end());
    this->addNodeType(pos->second->createType("Billboard",
                                              billboardInterfaceSet));

    //
    // Box node
    //
    static const NodeInterface boxInterface =
            NodeInterface(NodeInterface::field, FieldValue::sfvec3f, "size");
    static const Vrml97NodeInterfaceSet_
            boxInterfaceSet(&boxInterface, &boxInterface + 1);
    pos = nodeClassMap.find("urn:X-openvrml:node:Box");
    assert(pos != nodeClassMap.end());
    this->addNodeType(pos->second->createType("Box", boxInterfaceSet));

    //
    // Collision node
    //
    static const NodeInterface collisionInterfaces[] = {
        NodeInterface(NodeInterface::eventIn, FieldValue::mfnode, "addChildren"),
        NodeInterface(NodeInterface::eventIn, FieldValue::mfnode, "removeChildren"),
        NodeInterface(NodeInterface::exposedField, FieldValue::mfnode, "children"),
        NodeInterface(NodeInterface::exposedField, FieldValue::sfbool, "collide"),
        NodeInterface(NodeInterface::field, FieldValue::sfvec3f, "bboxCenter"),
        NodeInterface(NodeInterface::field, FieldValue::sfvec3f, "bboxSize"),
        NodeInterface(NodeInterface::field, FieldValue::sfnode, "proxy"),
        NodeInterface(NodeInterface::eventOut, FieldValue::sftime, "collideTime")
    };
    static const Vrml97NodeInterfaceSet_
            collisionInterfaceSet(collisionInterfaces, collisionInterfaces + 8);
    pos = nodeClassMap.find("urn:X-openvrml:node:Collision");
    assert(pos != nodeClassMap.end());
    this->addNodeType(pos->second->createType("Collision",
                                              collisionInterfaceSet));

    //
    // Color node
    //
    static const NodeInterface colorInterface =
            NodeInterface(NodeInterface::exposedField, FieldValue::mfcolor, "color");
    static const Vrml97NodeInterfaceSet_
            colorInterfaceSet(&colorInterface, &colorInterface + 1);
    pos = nodeClassMap.find("urn:X-openvrml:node:Color");
    assert(pos != nodeClassMap.end());
    this->addNodeType(pos->second->createType("Color", colorInterfaceSet));

    //
    // ColorInterpolator node
    //
    static const NodeInterface colorInterpolatorInterfaces[] = {
        NodeInterface(NodeInterface::eventIn, FieldValue::sffloat, "set_fraction"),
        NodeInterface(NodeInterface::exposedField, FieldValue::mffloat, "key"),
        NodeInterface(NodeInterface::exposedField, FieldValue::mfcolor, "keyValue"),
        NodeInterface(NodeInterface::eventOut, FieldValue::sfcolor, "value_changed")
    };
    static const Vrml97NodeInterfaceSet_
            colorInterpolatorInterfaceSet(colorInterpolatorInterfaces,
                                          colorInterpolatorInterfaces + 4);
    pos = nodeClassMap.find("urn:X-openvrml:node:ColorInterpolator");
    assert(pos != nodeClassMap.end());
    this->addNodeType(pos->second->createType("ColorInterpolator",
                                              colorInterpolatorInterfaceSet));

    //
    // Cone node
    //
    static const NodeInterface coneInterfaces[] = {
        NodeInterface(NodeInterface::field, FieldValue::sffloat, "bottomRadius"),
        NodeInterface(NodeInterface::field, FieldValue::sffloat, "height"),
        NodeInterface(NodeInterface::field, FieldValue::sfbool, "side"),
        NodeInterface(NodeInterface::field, FieldValue::sfbool, "bottom")
    };
    static const Vrml97NodeInterfaceSet_
            coneInterfaceSet(coneInterfaces, coneInterfaces + 4);
    pos = nodeClassMap.find("urn:X-openvrml:node:Cone");
    assert(pos != nodeClassMap.end());
    this->addNodeType(pos->second->createType("Cone", coneInterfaceSet));

    //
    // Coordinate node
    //
    static const NodeInterface coordinateInterface =
            NodeInterface(NodeInterface::exposedField, FieldValue::mfvec3f, "point");
    static const Vrml97NodeInterfaceSet_
            coordinateInterfaceSet(&coordinateInterface,
                                   &coordinateInterface + 1);
    pos = nodeClassMap.find("urn:X-openvrml:node:Coordinate");
    assert(pos != nodeClassMap.end());
    this->addNodeType(pos->second->createType("Coordinate",
                                              coordinateInterfaceSet));

    //
    // CoordinateInterpolator node
    //
    static const NodeInterface coordinateInterpolatorInterfaces[] = {
        NodeInterface(NodeInterface::eventIn, FieldValue::sffloat, "set_fraction"),
        NodeInterface(NodeInterface::exposedField, FieldValue::mffloat, "key"),
        NodeInterface(NodeInterface::exposedField, FieldValue::mfvec3f, "keyValue"),
        NodeInterface(NodeInterface::eventOut, FieldValue::mfvec3f, "value_changed")
    };
    static const Vrml97NodeInterfaceSet_
            coordinateInterpolatorInterfaceSet(coordinateInterpolatorInterfaces,
                                               coordinateInterpolatorInterfaces + 4);
    pos = nodeClassMap.find("urn:X-openvrml:node:CoordinateInterpolator");
    assert(pos != nodeClassMap.end());
    this->addNodeType(pos->second->createType("CoordinateInterpolator",
                                              coordinateInterpolatorInterfaceSet));

    //
    // Cylinder node
    //
    static const NodeInterface cylinderInterfaces[] = {
        NodeInterface(NodeInterface::field, FieldValue::sfbool, "bottom"),
        NodeInterface(NodeInterface::field, FieldValue::sffloat, "height"),
        NodeInterface(NodeInterface::field, FieldValue::sffloat, "radius"),
        NodeInterface(NodeInterface::field, FieldValue::sfbool, "side"),
        NodeInterface(NodeInterface::field, FieldValue::sfbool, "top")
    };
    static const Vrml97NodeInterfaceSet_
            cylinderInterfaceSet(cylinderInterfaces, cylinderInterfaces + 5);
    pos = nodeClassMap.find("urn:X-openvrml:node:Cylinder");
    assert(pos != nodeClassMap.end());
    this->addNodeType(pos->second->createType("Cylinder",
                                              cylinderInterfaceSet));

    //
    // CylinderSensor node
    //
    static const NodeInterface cylinderSensorInterfaces[] = {
        NodeInterface(NodeInterface::exposedField, FieldValue::sfbool, "autoOffset"),
        NodeInterface(NodeInterface::exposedField, FieldValue::sffloat, "diskAngle"),
        NodeInterface(NodeInterface::exposedField, FieldValue::sfbool, "enabled"),
        NodeInterface(NodeInterface::exposedField, FieldValue::sffloat, "maxAngle"),
        NodeInterface(NodeInterface::exposedField, FieldValue::sffloat, "minAngle"),
        NodeInterface(NodeInterface::exposedField, FieldValue::sffloat, "offset"),
        NodeInterface(NodeInterface::eventOut, FieldValue::sfbool, "isActive"),
        NodeInterface(NodeInterface::eventOut, FieldValue::sfrotation, "rotation_changed"),
        NodeInterface(NodeInterface::eventOut, FieldValue::sfvec3f, "trackPoint_changed")
    };
    static const Vrml97NodeInterfaceSet_
            cylinderSensorInterfaceSet(cylinderSensorInterfaces,
                                       cylinderSensorInterfaces + 9);
    pos = nodeClassMap.find("urn:X-openvrml:node:CylinderSensor");
    assert(pos != nodeClassMap.end());
    this->addNodeType(pos->second->createType("CylinderSensor",
                                              cylinderSensorInterfaceSet));

    //
    // DirectionalLight node
    //
    static const NodeInterface directionalLightInterfaces[] = {
        NodeInterface(NodeInterface::exposedField, FieldValue::sffloat, "ambientIntensity"),
        NodeInterface(NodeInterface::exposedField, FieldValue::sfcolor, "color"),
        NodeInterface(NodeInterface::exposedField, FieldValue::sfvec3f, "direction"),
        NodeInterface(NodeInterface::exposedField, FieldValue::sffloat, "intensity"),
        NodeInterface(NodeInterface::exposedField, FieldValue::sfbool, "on")
    };
    static const Vrml97NodeInterfaceSet_
            directionalLightInterfaceSet(directionalLightInterfaces,
                                         directionalLightInterfaces + 5);
    pos = nodeClassMap.find("urn:X-openvrml:node:DirectionalLight");
    assert(pos != nodeClassMap.end());
    this->addNodeType(pos->second->createType("DirectionalLight",
                                              directionalLightInterfaceSet));

    //
    // ElevationGrid node
    //
    static const NodeInterface elevationGridInterfaces[] = {
        NodeInterface(NodeInterface::eventIn, FieldValue::mffloat, "set_height"),
        NodeInterface(NodeInterface::exposedField, FieldValue::sfnode, "color"),
        NodeInterface(NodeInterface::exposedField, FieldValue::sfnode, "normal"),
        NodeInterface(NodeInterface::exposedField, FieldValue::sfnode, "texCoord"),
        NodeInterface(NodeInterface::field, FieldValue::mffloat, "height"),
        NodeInterface(NodeInterface::field, FieldValue::sfbool, "ccw"),
        NodeInterface(NodeInterface::field, FieldValue::sfbool, "colorPerVertex"),
        NodeInterface(NodeInterface::field, FieldValue::sffloat, "creaseAngle"),
        NodeInterface(NodeInterface::field, FieldValue::sfbool, "normalPerVertex"),
        NodeInterface(NodeInterface::field, FieldValue::sfbool, "solid"),
        NodeInterface(NodeInterface::field, FieldValue::sfint32, "xDimension"),
        NodeInterface(NodeInterface::field, FieldValue::sffloat, "xSpacing"),
        NodeInterface(NodeInterface::field, FieldValue::sfint32, "zDimension"),
        NodeInterface(NodeInterface::field, FieldValue::sffloat, "zSpacing")
    };
    static const Vrml97NodeInterfaceSet_
            elevationGridInterfaceSet(elevationGridInterfaces,
                                         elevationGridInterfaces + 14);
    pos = nodeClassMap.find("urn:X-openvrml:node:ElevationGrid");
    assert(pos != nodeClassMap.end());
    this->addNodeType(pos->second->createType("ElevationGrid",
                                              elevationGridInterfaceSet));

    //
    // Extrusion node
    //
    {
        static const NodeInterface nodeInterfaces[] = {
            NodeInterface(NodeInterface::eventIn, FieldValue::mfvec2f, "set_crossSection"),
            NodeInterface(NodeInterface::eventIn, FieldValue::mfrotation, "set_orientation"),
            NodeInterface(NodeInterface::eventIn, FieldValue::mfvec2f, "set_scale"),
            NodeInterface(NodeInterface::eventIn, FieldValue::mfvec3f, "set_spine"),
            NodeInterface(NodeInterface::field, FieldValue::sfbool, "beginCap"),
            NodeInterface(NodeInterface::field, FieldValue::sfbool, "ccw"),
            NodeInterface(NodeInterface::field, FieldValue::sfbool, "convex"),
            NodeInterface(NodeInterface::field, FieldValue::sffloat, "creaseAngle"),
            NodeInterface(NodeInterface::field, FieldValue::mfvec2f, "crossSection"),
            NodeInterface(NodeInterface::field, FieldValue::sfbool, "endCap"),
            NodeInterface(NodeInterface::field, FieldValue::mfrotation, "orientation"),
            NodeInterface(NodeInterface::field, FieldValue::mfvec2f, "scale"),
            NodeInterface(NodeInterface::field, FieldValue::sfbool, "solid"),
            NodeInterface(NodeInterface::field, FieldValue::mfvec3f, "spine")
        };
        static const Vrml97NodeInterfaceSet_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 14);
        pos = nodeClassMap.find("urn:X-openvrml:node:Extrusion");
        assert(pos != nodeClassMap.end());
        this->addNodeType(pos->second->createType("Extrusion",
                                                  nodeInterfaceSet));
    }

    //
    // Fog node
    //
    {
        static const NodeInterface nodeInterfaces[] = {
            NodeInterface(NodeInterface::eventIn, FieldValue::sfbool, "set_bind"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfcolor, "color"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfstring, "fogType"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sffloat, "visibilityRange"),
            NodeInterface(NodeInterface::eventOut, FieldValue::sfbool, "isBound")
        };
        static const Vrml97NodeInterfaceSet_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 5);
        pos = nodeClassMap.find("urn:X-openvrml:node:Fog");
        assert(pos != nodeClassMap.end());
        this->addNodeType(pos->second->createType("Fog", nodeInterfaceSet));
    }

    //
    // FontStyle node
    //
    {
        static const NodeInterface nodeInterfaces[] = {
            NodeInterface(NodeInterface::field, FieldValue::mfstring, "family"),
            NodeInterface(NodeInterface::field, FieldValue::sfbool, "horizontal"),
            NodeInterface(NodeInterface::field, FieldValue::mfstring, "justify"),
            NodeInterface(NodeInterface::field, FieldValue::sfstring, "language"),
            NodeInterface(NodeInterface::field, FieldValue::sfbool, "leftToRight"),
            NodeInterface(NodeInterface::field, FieldValue::sffloat, "size"),
            NodeInterface(NodeInterface::field, FieldValue::sffloat, "spacing"),
            NodeInterface(NodeInterface::field, FieldValue::sfstring, "style"),
            NodeInterface(NodeInterface::field, FieldValue::sfbool, "topToBottom")
        };
        static const Vrml97NodeInterfaceSet_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 9);
        pos = nodeClassMap.find("urn:X-openvrml:node:FontStyle");
        assert(pos != nodeClassMap.end());
        this->addNodeType(pos->second->createType("FontStyle",
                                                  nodeInterfaceSet));
    }

    //
    // Group node
    //
    {
        static const NodeInterface nodeInterfaces[] = {
            NodeInterface(NodeInterface::eventIn, FieldValue::mfnode, "addChildren"),
            NodeInterface(NodeInterface::eventIn, FieldValue::mfnode, "removeChildren"),
            NodeInterface(NodeInterface::exposedField, FieldValue::mfnode, "children"),
            NodeInterface(NodeInterface::field, FieldValue::sfvec3f, "bboxCenter"),
            NodeInterface(NodeInterface::field, FieldValue::sfvec3f, "bboxSize")
        };
        static const Vrml97NodeInterfaceSet_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 5);
        pos = nodeClassMap.find("urn:X-openvrml:node:Group");
        assert(pos != nodeClassMap.end());
        this->addNodeType(pos->second->createType("Group",
                                                  nodeInterfaceSet));
    }

    //
    // ImageTexture node
    //
    {
        static const NodeInterface nodeInterfaces[] = {
            NodeInterface(NodeInterface::exposedField, FieldValue::mfstring, "url"),
            NodeInterface(NodeInterface::field, FieldValue::sfbool, "repeatS"),
            NodeInterface(NodeInterface::field, FieldValue::sfbool, "repeatT")
        };
        static const Vrml97NodeInterfaceSet_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 3);
        pos = nodeClassMap.find("urn:X-openvrml:node:ImageTexture");
        assert(pos != nodeClassMap.end());
        this->addNodeType(pos->second->createType("ImageTexture",
                                                  nodeInterfaceSet));
    }

    //
    // IndexedFaceSet node
    //
    {
        static const NodeInterface nodeInterfaces[] = {
            NodeInterface(NodeInterface::eventIn, FieldValue::mfint32, "set_colorIndex"),
            NodeInterface(NodeInterface::eventIn, FieldValue::mfint32, "set_coordIndex"),
            NodeInterface(NodeInterface::eventIn, FieldValue::mfint32, "set_normalIndex"),
            NodeInterface(NodeInterface::eventIn, FieldValue::mfint32, "set_texCoordIndex"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfnode, "color"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfnode, "coord"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfnode, "normal"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfnode, "texCoord"),
            NodeInterface(NodeInterface::field, FieldValue::sfbool, "ccw"),
            NodeInterface(NodeInterface::field, FieldValue::mfint32, "colorIndex"),
            NodeInterface(NodeInterface::field, FieldValue::sfbool, "colorPerVertex"),
            NodeInterface(NodeInterface::field, FieldValue::sfbool, "convex"),
            NodeInterface(NodeInterface::field, FieldValue::mfint32, "coordIndex"),
            NodeInterface(NodeInterface::field, FieldValue::sffloat, "creaseAngle"),
            NodeInterface(NodeInterface::field, FieldValue::mfint32, "normalIndex"),
            NodeInterface(NodeInterface::field, FieldValue::sfbool, "normalPerVertex"),
            NodeInterface(NodeInterface::field, FieldValue::sfbool, "solid"),
            NodeInterface(NodeInterface::field, FieldValue::mfint32, "texCoordIndex")
        };
        static const Vrml97NodeInterfaceSet_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 18);
        pos = nodeClassMap.find("urn:X-openvrml:node:IndexedFaceSet");
        assert(pos != nodeClassMap.end());
        this->addNodeType(pos->second->createType("IndexedFaceSet",
                                                  nodeInterfaceSet));
    }

    //
    // IndexedLineSet node
    //
    {
        static const NodeInterface nodeInterfaces[] = {
            NodeInterface(NodeInterface::eventIn, FieldValue::mfint32, "set_colorIndex"),
            NodeInterface(NodeInterface::eventIn, FieldValue::mfint32, "set_coordIndex"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfnode, "color"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfnode, "coord"),
            NodeInterface(NodeInterface::field, FieldValue::mfint32, "colorIndex"),
            NodeInterface(NodeInterface::field, FieldValue::sfbool, "colorPerVertex"),
            NodeInterface(NodeInterface::field, FieldValue::mfint32, "coordIndex")
        };
        static const Vrml97NodeInterfaceSet_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 7);
        pos = nodeClassMap.find("urn:X-openvrml:node:IndexedLineSet");
        assert(pos != nodeClassMap.end());
        this->addNodeType(pos->second->createType("IndexedLineSet",
                                                  nodeInterfaceSet));
    }

    //
    // Inline node
    //
    {
        static const NodeInterface nodeInterfaces[] = {
            NodeInterface(NodeInterface::exposedField, FieldValue::mfstring, "url"),
            NodeInterface(NodeInterface::field, FieldValue::sfvec3f, "bboxCenter"),
            NodeInterface(NodeInterface::field, FieldValue::sfvec3f, "bboxSize")
        };
        static const Vrml97NodeInterfaceSet_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 3);
        pos = nodeClassMap.find("urn:X-openvrml:node:Inline");
        assert(pos != nodeClassMap.end());
        this->addNodeType(pos->second->createType("Inline", nodeInterfaceSet));
    }

    //
    // LOD node
    //
    {
        static const NodeInterface nodeInterfaces[] = {
            NodeInterface(NodeInterface::exposedField, FieldValue::mfnode, "level"),
            NodeInterface(NodeInterface::field, FieldValue::sfvec3f, "center"),
            NodeInterface(NodeInterface::field, FieldValue::mffloat, "range")
        };
        static const Vrml97NodeInterfaceSet_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 3);
        pos = nodeClassMap.find("urn:X-openvrml:node:LOD");
        assert(pos != nodeClassMap.end());
        this->addNodeType(pos->second->createType("LOD", nodeInterfaceSet));
    }

    //
    // Material node
    //
    {
        static const NodeInterface nodeInterfaces[] = {
            NodeInterface(NodeInterface::exposedField, FieldValue::sffloat, "ambientIntensity"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfcolor, "diffuseColor"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfcolor, "emissiveColor"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sffloat, "shininess"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfcolor, "specularColor"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sffloat, "transparency")
        };
        static const Vrml97NodeInterfaceSet_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 6);
        pos = nodeClassMap.find("urn:X-openvrml:node:Material");
        assert(pos != nodeClassMap.end());
        this->addNodeType(pos->second->createType("Material",
                                                  nodeInterfaceSet));
    }

    //
    // MovieTexture node
    //
    {
        static const NodeInterface nodeInterfaces[] = {
            NodeInterface(NodeInterface::exposedField, FieldValue::sfbool, "loop"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sffloat, "speed"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sftime, "startTime"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sftime, "stopTime"),
            NodeInterface(NodeInterface::exposedField, FieldValue::mfstring, "url"),
            NodeInterface(NodeInterface::field, FieldValue::sfbool, "repeatS"),
            NodeInterface(NodeInterface::field, FieldValue::sfbool, "repeatT"),
            NodeInterface(NodeInterface::eventOut, FieldValue::sftime, "duration_changed"),
            NodeInterface(NodeInterface::eventOut, FieldValue::sfbool, "isActive")
        };
        static const Vrml97NodeInterfaceSet_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 9);
        pos = nodeClassMap.find("urn:X-openvrml:node:MovieTexture");
        assert(pos != nodeClassMap.end());
        this->addNodeType(pos->second->createType("MovieTexture",
                                                  nodeInterfaceSet));
    }

    //
    // NavigationInfo node
    //
    {
        static const NodeInterface nodeInterfaces[] = {
            NodeInterface(NodeInterface::eventIn, FieldValue::sfbool, "set_bind"),
            NodeInterface(NodeInterface::exposedField, FieldValue::mffloat, "avatarSize"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfbool, "headlight"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sffloat, "speed"),
            NodeInterface(NodeInterface::exposedField, FieldValue::mfstring, "type"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sffloat, "visibilityLimit"),
            NodeInterface(NodeInterface::eventOut, FieldValue::sfbool, "isBound")
        };
        static const Vrml97NodeInterfaceSet_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 7);
        pos = nodeClassMap.find("urn:X-openvrml:node:NavigationInfo");
        assert(pos != nodeClassMap.end());
        this->addNodeType(pos->second->createType("NavigationInfo",
                                                  nodeInterfaceSet));
    }

    //
    // Normal node
    //
    {
        static const NodeInterface nodeInterface =
                NodeInterface(NodeInterface::exposedField, FieldValue::mfvec3f, "vector");
        static const Vrml97NodeInterfaceSet_
                nodeInterfaceSet(&nodeInterface, &nodeInterface + 1);
        pos = nodeClassMap.find("urn:X-openvrml:node:Normal");
        assert(pos != nodeClassMap.end());
        this->addNodeType(pos->second->createType("Normal",
                                                  nodeInterfaceSet));
    }

    //
    // NormalInterpolator node
    //
    {
        static const NodeInterface nodeInterfaces[] = {
            NodeInterface(NodeInterface::eventIn, FieldValue::sffloat, "set_fraction"),
            NodeInterface(NodeInterface::exposedField, FieldValue::mffloat, "key"),
            NodeInterface(NodeInterface::exposedField, FieldValue::mfvec3f, "keyValue"),
            NodeInterface(NodeInterface::eventOut, FieldValue::mfvec3f, "value_changed")
        };
        static const Vrml97NodeInterfaceSet_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 4);
        pos = nodeClassMap.find("urn:X-openvrml:node:NormalInterpolator");
        assert(pos != nodeClassMap.end());
        this->addNodeType(pos->second->createType("NormalInterpolator",
                                                  nodeInterfaceSet));
    }

    //
    // OrientationInterpolator node
    //
    {
        static const NodeInterface nodeInterfaces[] = {
            NodeInterface(NodeInterface::eventIn, FieldValue::sffloat, "set_fraction"),
            NodeInterface(NodeInterface::exposedField, FieldValue::mffloat, "key"),
            NodeInterface(NodeInterface::exposedField, FieldValue::mfrotation, "keyValue"),
            NodeInterface(NodeInterface::eventOut, FieldValue::sfrotation, "value_changed")
        };
        static const Vrml97NodeInterfaceSet_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 4);
        pos = nodeClassMap.find("urn:X-openvrml:node:OrientationInterpolator");
        assert(pos != nodeClassMap.end());
        this->addNodeType(pos->second->createType("OrientationInterpolator",
                                                  nodeInterfaceSet));
    }

    //
    // PixelTexture node
    //
    {
        static const NodeInterface nodeInterfaces[] = {
            NodeInterface(NodeInterface::exposedField, FieldValue::sfimage, "image"),
            NodeInterface(NodeInterface::field, FieldValue::sfbool, "repeatS"),
            NodeInterface(NodeInterface::field, FieldValue::sfbool, "repeatT")
        };
        static const Vrml97NodeInterfaceSet_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 3);
        pos = nodeClassMap.find("urn:X-openvrml:node:PixelTexture");
        assert(pos != nodeClassMap.end());
        this->addNodeType(pos->second->createType("PixelTexture",
                                                  nodeInterfaceSet));
    }

    //
    // PlaneSensor node
    //
    {
        static const NodeInterface nodeInterfaces[] = {
            NodeInterface(NodeInterface::exposedField, FieldValue::sfbool, "autoOffset"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfbool, "enabled"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfvec2f, "maxPosition"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfvec2f, "minPosition"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfvec3f, "offset"),
            NodeInterface(NodeInterface::eventOut, FieldValue::sfbool, "isActive"),
            NodeInterface(NodeInterface::eventOut, FieldValue::sfvec3f, "trackPoint_changed"),
            NodeInterface(NodeInterface::eventOut, FieldValue::sfvec3f, "translation_changed")
        };
        static const Vrml97NodeInterfaceSet_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 8);
        pos = nodeClassMap.find("urn:X-openvrml:node:PlaneSensor");
        assert(pos != nodeClassMap.end());
        this->addNodeType(pos->second->createType("PlaneSensor",
                                                  nodeInterfaceSet));
    }

    //
    // PointLight node
    //
    {
        static const NodeInterface nodeInterfaces[] = {
            NodeInterface(NodeInterface::exposedField, FieldValue::sffloat, "ambientIntensity"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfvec3f, "attenuation"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfcolor, "color"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sffloat, "intensity"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfvec3f, "location"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfbool, "on"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sffloat, "radius")
        };
        static const Vrml97NodeInterfaceSet_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 7);
        pos = nodeClassMap.find("urn:X-openvrml:node:PointLight");
        assert(pos != nodeClassMap.end());
        this->addNodeType(pos->second->createType("PointLight",
                                                  nodeInterfaceSet));
    }

    //
    // PointSet node
    //
    {
        static const NodeInterface nodeInterfaces[] = {
            NodeInterface(NodeInterface::exposedField, FieldValue::sfnode, "color"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfnode, "coord")
        };
        static const Vrml97NodeInterfaceSet_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 2);
        pos = nodeClassMap.find("urn:X-openvrml:node:PointSet");
        assert(pos != nodeClassMap.end());
        this->addNodeType(pos->second->createType("PointSet",
                                                  nodeInterfaceSet));
    }

    //
    // PositionInterpolator node
    //
    {
        static const NodeInterface nodeInterfaces[] = {
            NodeInterface(NodeInterface::eventIn, FieldValue::sffloat, "set_fraction"),
            NodeInterface(NodeInterface::exposedField, FieldValue::mffloat, "key"),
            NodeInterface(NodeInterface::exposedField, FieldValue::mfvec3f, "keyValue"),
            NodeInterface(NodeInterface::eventOut, FieldValue::sfvec3f, "value_changed")
        };
        static const Vrml97NodeInterfaceSet_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 4);
        pos = nodeClassMap.find("urn:X-openvrml:node:PositionInterpolator");
        assert(pos != nodeClassMap.end());
        this->addNodeType(pos->second->createType("PositionInterpolator",
                                                  nodeInterfaceSet));
    }

    //
    // ProximitySensor node
    //
    {
        static const NodeInterface nodeInterfaces[] = {
            NodeInterface(NodeInterface::exposedField, FieldValue::sfvec3f, "center"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfvec3f, "size"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfbool, "enabled"),
            NodeInterface(NodeInterface::eventOut, FieldValue::sfbool, "isActive"),
            NodeInterface(NodeInterface::eventOut, FieldValue::sfvec3f, "position_changed"),
            NodeInterface(NodeInterface::eventOut, FieldValue::sfrotation, "orientation_changed"),
            NodeInterface(NodeInterface::eventOut, FieldValue::sftime, "enterTime"),
            NodeInterface(NodeInterface::eventOut, FieldValue::sftime, "exitTime")
        };
        static const Vrml97NodeInterfaceSet_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 8);
        pos = nodeClassMap.find("urn:X-openvrml:node:ProximitySensor");
        assert(pos != nodeClassMap.end());
        this->addNodeType(pos->second->createType("ProximitySensor",
                                                  nodeInterfaceSet));
    }

    //
    // ScalarInterpolator node
    //
    {
        static const NodeInterface nodeInterfaces[] = {
            NodeInterface(NodeInterface::eventIn, FieldValue::sffloat, "set_fraction"),
            NodeInterface(NodeInterface::exposedField, FieldValue::mffloat, "key"),
            NodeInterface(NodeInterface::exposedField, FieldValue::mffloat, "keyValue"),
            NodeInterface(NodeInterface::eventOut, FieldValue::sffloat, "value_changed")
        };
        static const Vrml97NodeInterfaceSet_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 4);
        pos = nodeClassMap.find("urn:X-openvrml:node:ScalarInterpolator");
        assert(pos != nodeClassMap.end());
        this->addNodeType(pos->second->createType("ScalarInterpolator",
                                                  nodeInterfaceSet));
    }

    //
    // Shape node
    //
    {
        static const NodeInterface nodeInterfaces[] = {
            NodeInterface(NodeInterface::exposedField, FieldValue::sfnode, "appearance"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfnode, "geometry")
        };
        static const Vrml97NodeInterfaceSet_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 2);
        pos = nodeClassMap.find("urn:X-openvrml:node:Shape");
        assert(pos != nodeClassMap.end());
        this->addNodeType(pos->second->createType("Shape", nodeInterfaceSet));
    }

    //
    // Sound node
    //
    {
        static const NodeInterface nodeInterfaces[] = {
            NodeInterface(NodeInterface::exposedField, FieldValue::sfvec3f, "direction"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sffloat, "intensity"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfvec3f, "location"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sffloat, "maxBack"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sffloat, "maxFront"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sffloat, "minBack"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sffloat, "minFront"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sffloat, "priority"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfnode, "source"),
            NodeInterface(NodeInterface::field, FieldValue::sfbool, "spatialize")
        };
        static const Vrml97NodeInterfaceSet_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 10);
        pos = nodeClassMap.find("urn:X-openvrml:node:Sound");
        assert(pos != nodeClassMap.end());
        this->addNodeType(pos->second->createType("Sound", nodeInterfaceSet));
    }

    //
    // Sphere node
    //
    {
        static const NodeInterface nodeInterface =
                NodeInterface(NodeInterface::field, FieldValue::sffloat, "radius");
        static const Vrml97NodeInterfaceSet_
                nodeInterfaceSet(&nodeInterface, &nodeInterface + 1);
        pos = nodeClassMap.find("urn:X-openvrml:node:Sphere");
        assert(pos != nodeClassMap.end());
        this->addNodeType(pos->second->createType("Sphere", nodeInterfaceSet));
    }

    //
    // SphereSensor node
    //
    {
        static const NodeInterface nodeInterfaces[] = {
            NodeInterface(NodeInterface::exposedField, FieldValue::sfbool, "autoOffset"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfbool, "enabled"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sffloat, "offset"),
            NodeInterface(NodeInterface::eventOut, FieldValue::sfbool, "isActive"),
            NodeInterface(NodeInterface::eventOut, FieldValue::sfrotation, "rotation_changed"),
            NodeInterface(NodeInterface::eventOut, FieldValue::sfvec3f, "trackPoint_changed")
        };
        static const Vrml97NodeInterfaceSet_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 6);
        pos = nodeClassMap.find("urn:X-openvrml:node:SphereSensor");
        assert(pos != nodeClassMap.end());
        this->addNodeType(pos->second->createType("SphereSensor",
                                                  nodeInterfaceSet));
    }

    //
    // SpotLight node
    //
    {
        static const NodeInterface nodeInterfaces[] = {
            NodeInterface(NodeInterface::exposedField, FieldValue::sffloat, "ambientIntensity"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfvec3f, "attenuation"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sffloat, "beamWidth"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfcolor, "color"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sffloat, "cutOffAngle"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfvec3f, "direction"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sffloat, "intensity"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfvec3f, "location"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfbool, "on"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sffloat, "radius")
        };
        static const Vrml97NodeInterfaceSet_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 10);
        pos = nodeClassMap.find("urn:X-openvrml:node:SpotLight");
        assert(pos != nodeClassMap.end());
        this->addNodeType(pos->second->createType("SpotLight",
                                                  nodeInterfaceSet));
    }

    //
    // Switch node
    //
    {
        static const NodeInterface nodeInterfaces[] = {
            NodeInterface(NodeInterface::exposedField, FieldValue::mfnode, "choice"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfint32, "whichChoice")
        };
        static const Vrml97NodeInterfaceSet_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 2);
        pos = nodeClassMap.find("urn:X-openvrml:node:Switch");
        assert(pos != nodeClassMap.end());
        this->addNodeType(pos->second->createType("Switch", nodeInterfaceSet));
    }

    //
    // Text node
    //
    {
        static const NodeInterface nodeInterfaces[] = {
            NodeInterface(NodeInterface::exposedField, FieldValue::mfstring, "string"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfnode, "fontStyle"),
            NodeInterface(NodeInterface::exposedField, FieldValue::mffloat, "length"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sffloat, "maxExtent")
        };
        static const Vrml97NodeInterfaceSet_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 4);
        pos = nodeClassMap.find("urn:X-openvrml:node:Text");
        assert(pos != nodeClassMap.end());
        this->addNodeType(pos->second->createType("Text", nodeInterfaceSet));
    }

    //
    // TextureCoordinate node
    //
    {
        static const NodeInterface nodeInterface =
                NodeInterface(NodeInterface::exposedField, FieldValue::mfvec2f, "point");
        static const Vrml97NodeInterfaceSet_
                nodeInterfaceSet(&nodeInterface, &nodeInterface + 1);
        pos = nodeClassMap.find("urn:X-openvrml:node:TextureCoordinate");
        assert(pos != nodeClassMap.end());
        this->addNodeType(pos->second->createType("TextureCoordinate",
                                                  nodeInterfaceSet));
    }

    //
    // TextureTransform node
    //
    {
        static const NodeInterface nodeInterfaces[] = {
            NodeInterface(NodeInterface::exposedField, FieldValue::sfvec2f, "center"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sffloat, "rotation"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfvec2f, "scale"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfvec2f, "translation")
        };
        static const Vrml97NodeInterfaceSet_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 4);
        pos = nodeClassMap.find("urn:X-openvrml:node:TextureTransform");
        assert(pos != nodeClassMap.end());
        this->addNodeType(pos->second->createType("TextureTransform",
                                                  nodeInterfaceSet));
    }

    //
    // TimeSensor node
    //
    {
        static const NodeInterface nodeInterfaces[] = {
            NodeInterface(NodeInterface::exposedField, FieldValue::sftime, "cycleInterval"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfbool, "enabled"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfbool, "loop"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sftime, "startTime"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sftime, "stopTime"),
            NodeInterface(NodeInterface::eventOut, FieldValue::sftime, "cycleTime"),
            NodeInterface(NodeInterface::eventOut, FieldValue::sffloat, "fraction_changed"),
            NodeInterface(NodeInterface::eventOut, FieldValue::sfbool, "isActive"),
            NodeInterface(NodeInterface::eventOut, FieldValue::sftime, "time")
        };
        static const Vrml97NodeInterfaceSet_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 9);
        pos = nodeClassMap.find("urn:X-openvrml:node:TimeSensor");
        assert(pos != nodeClassMap.end());
        this->addNodeType(pos->second->createType("TimeSensor",
                                                  nodeInterfaceSet));
    }

    //
    // TouchSensor node
    //
    {
        static const NodeInterface nodeInterfaces[] = {
            NodeInterface(NodeInterface::exposedField, FieldValue::sfbool, "enabled"),
            NodeInterface(NodeInterface::eventOut, FieldValue::sfvec3f, "hitNormal_changed"),
            NodeInterface(NodeInterface::eventOut, FieldValue::sfvec3f, "hitPoint_changed"),
            NodeInterface(NodeInterface::eventOut, FieldValue::sfvec2f, "hitTexCoord_changed"),
            NodeInterface(NodeInterface::eventOut, FieldValue::sfbool, "isActive"),
            NodeInterface(NodeInterface::eventOut, FieldValue::sfbool, "isOver"),
            NodeInterface(NodeInterface::eventOut, FieldValue::sftime, "touchTime")
        };
        static const Vrml97NodeInterfaceSet_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 7);
        pos = nodeClassMap.find("urn:X-openvrml:node:TouchSensor");
        assert(pos != nodeClassMap.end());
        this->addNodeType(pos->second->createType("TouchSensor",
                                                  nodeInterfaceSet));
    }

    //
    // Transform node
    //
    {
        static const NodeInterface nodeInterfaces[] = {
            NodeInterface(NodeInterface::eventIn, FieldValue::mfnode, "addChildren"),
            NodeInterface(NodeInterface::eventIn, FieldValue::mfnode, "removeChildren"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfvec3f, "center"),
            NodeInterface(NodeInterface::exposedField, FieldValue::mfnode, "children"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfrotation, "rotation"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfvec3f, "scale"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfrotation, "scaleOrientation"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfvec3f, "translation"),
            NodeInterface(NodeInterface::field, FieldValue::sfvec3f, "bboxCenter"),
            NodeInterface(NodeInterface::field, FieldValue::sfvec3f, "bboxSize")
        };
        static const Vrml97NodeInterfaceSet_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 10);
        pos = nodeClassMap.find("urn:X-openvrml:node:Transform");
        assert(pos != nodeClassMap.end());
        this->addNodeType(pos->second->createType("Transform",
                                                  nodeInterfaceSet));
    }

    //
    // Viewpoint node
    //
    {
        static const NodeInterface nodeInterfaces[] = {
            NodeInterface(NodeInterface::eventIn, FieldValue::sfbool, "set_bind"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sffloat, "fieldOfView"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfbool, "jump"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfrotation, "orientation"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfvec3f, "position"),
            NodeInterface(NodeInterface::field, FieldValue::sfstring, "description"),
            NodeInterface(NodeInterface::eventOut, FieldValue::sftime, "bindTime"),
            NodeInterface(NodeInterface::eventOut, FieldValue::sfbool, "isBound")
        };
        static const Vrml97NodeInterfaceSet_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 8);
        pos = nodeClassMap.find("urn:X-openvrml:node:Viewpoint");
        assert(pos != nodeClassMap.end());
        this->addNodeType(pos->second->createType("Viewpoint",
                                                  nodeInterfaceSet));
    }

    //
    // VisibilitySensor node
    //
    {
        static const NodeInterface nodeInterfaces[] = {
            NodeInterface(NodeInterface::exposedField, FieldValue::sfvec3f, "center"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfbool, "enabled"),
            NodeInterface(NodeInterface::exposedField, FieldValue::sfvec3f, "size"),
            NodeInterface(NodeInterface::eventOut, FieldValue::sftime, "enterTime"),
            NodeInterface(NodeInterface::eventOut, FieldValue::sftime, "exitTime"),
            NodeInterface(NodeInterface::eventOut, FieldValue::sfbool, "isActive")
        };
        static const Vrml97NodeInterfaceSet_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 6);
        pos = nodeClassMap.find("urn:X-openvrml:node:VisibilitySensor");
        assert(pos != nodeClassMap.end());
        this->addNodeType(pos->second->createType("VisibilitySensor",
                                                  nodeInterfaceSet));
    }

    //
    // WorldInfo node
    //
    {
        static const NodeInterface nodeInterfaces[] = {
            NodeInterface(NodeInterface::field, FieldValue::mfstring, "info"),
            NodeInterface(NodeInterface::field, FieldValue::sfstring, "title")
        };
        static const Vrml97NodeInterfaceSet_
                nodeInterfaceSet(nodeInterfaces, nodeInterfaces + 2);
        pos = nodeClassMap.find("urn:X-openvrml:node:WorldInfo");
        assert(pos != nodeClassMap.end());
        this->addNodeType(pos->second->createType("WorldInfo",
                                                  nodeInterfaceSet));
    }
}

Vrml97RootScope::~Vrml97RootScope() throw () {}


NullNodeClass::NullNodeClass(Browser & browser) throw ():
    NodeClass(browser)
{}

NullNodeClass::~NullNodeClass() throw ()
{}

const NodeTypePtr NullNodeClass::createType(const std::string & id,
                                            const NodeInterfaceSet & interfaces)
    throw ()
{
    assert(false);
    static const NodeTypePtr nodeType;
    return nodeType;
}


NullNodeType::NullNodeType(NullNodeClass & nodeClass) throw ():
    NodeType(nodeClass, std::string())
{}

NullNodeType::~NullNodeType() throw ()
{}

const NodeInterfaceSet & NullNodeType::getInterfaces() const throw ()
{
    assert(false);
    static const NodeInterfaceSet interfaces;
    return interfaces;
}

const NodePtr NullNodeType::createNode(const ScopePtr & scope) const throw ()
{
    assert(false);
    static const NodePtr node;
    return node;
}


/**
 * @brief Constructor.
 *
 * @param nodeType  the Browser's NullNodeType instance.
 */
DefaultViewpoint::DefaultViewpoint(const NullNodeType & nodeType) throw ():
    Node(nodeType, ScopePtr()),
    ChildNode(nodeType, ScopePtr()),
    ViewpointNode(nodeType, ScopePtr())
{}

/**
 * @brief Destructor.
 */
DefaultViewpoint::~DefaultViewpoint() throw ()
{}

const mat4f & DefaultViewpoint::getTransformation() const throw ()
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

const mat4f & DefaultViewpoint::getUserViewTransform() const throw ()
{
    return this->userViewTransform;
}

void DefaultViewpoint::setUserViewTransform(const mat4f & transform)
    throw ()
{
    this->userViewTransform = transform;
}

const std::string & DefaultViewpoint::getDescription() const throw ()
{
    static const std::string description;
    return description;
}

float DefaultViewpoint::getFieldOfView() const throw ()
{
    static const float fieldOfView = 0.785398;
    return fieldOfView;
}

void DefaultViewpoint::do_setField(const std::string & id,
                                   const FieldValue & value)
    throw ()
{
    assert(false);
}

const FieldValue & DefaultViewpoint::do_getField(const std::string & id) const
    throw ()
{
    assert(false);
    static const SFBool value;
    return value;
}

void DefaultViewpoint::do_processEvent(const std::string & id,
                                       const FieldValue & value,
                                       double timestamp)
    throw ()
{
    assert(false);
}

const FieldValue &
DefaultViewpoint::do_getEventOut(const std::string & id) const throw ()
{
    assert(false);
    static const SFBool value;
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

    URI::URI(const std::string & str) throw (InvalidURI, std::bad_alloc):
            str(str) {
        int err = uriRegex.exec(str.c_str(), URI::nmatch, this->regmatch, 0);
        if (err != 0) { throw InvalidURI(); }
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
