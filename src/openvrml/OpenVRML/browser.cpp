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
# include "private.h"
# include "browser.h"
# include "doc2.hpp"
# include "Viewer.h"
# include "System.h"
# include "MathUtils.h"
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

        class NodeFieldCloneVisitor : public NodeVisitor {
            const ScopePtr & toScope;
            std::stack<NodePtr> rootNodeStack;
        
        public:
            explicit NodeFieldCloneVisitor(const ScopePtr & toScope)
                throw (std::bad_alloc);
            virtual ~NodeFieldCloneVisitor() throw ();
            
            const SFNode clone(const SFNode & sfnode);
            const MFNode clone(const MFNode & mfnode);

            virtual void visit(Node & node);

        private:
            // Not copyable.
            NodeFieldCloneVisitor(const NodeFieldCloneVisitor &);
            NodeFieldCloneVisitor & operator=(const NodeFieldCloneVisitor &);
        };
        
        class ProtoImplCloneVisitor : public NodeVisitor {
            std::stack<NodePtr> rootNodeStack;

            const ProtoNode & fromProtoNode;
            ProtoNode & toProtoNode;

        public:
            ProtoImplCloneVisitor(const ProtoNode & fromProtoNode,
                                  ProtoNode & toProtoNode)
                throw (std::bad_alloc);
            virtual ~ProtoImplCloneVisitor() throw ();

            void clone() throw (std::bad_alloc);

            virtual void visit(Node & node) throw (std::bad_alloc);

        private:
            // Not copyable.
            ProtoImplCloneVisitor(const ProtoImplCloneVisitor &);
            ProtoImplCloneVisitor & operator=(const ProtoImplCloneVisitor &);
        };

        friend class ProtoImplCloneVisitor;

        class RouteCopyVisitor : public NodeVisitor {
            const ProtoNode & fromProtoNode;
            ProtoNode & toProtoNode;

        public:
            RouteCopyVisitor(const ProtoNode & fromProtoNode,
                             ProtoNode & toProtoNode) throw ();
            virtual ~RouteCopyVisitor() throw ();

            void copyRoutes();

            virtual void visit(Node & node);

        private:
            // Not copyable.
            RouteCopyVisitor(const RouteCopyVisitor &);
            RouteCopyVisitor & operator=(const RouteCopyVisitor &);
        };

        friend class RouteCopyVisitor;

    public:
        struct ImplNodeInterface {
            Node & node;
            std::string interfaceId;

            ImplNodeInterface(Node & node, const std::string & interfaceId);
        };
        typedef std::multimap<std::string, ImplNodeInterface> ISMap;
        typedef std::map<std::string, FieldValuePtr> FieldValueMap;
        typedef std::map<std::string, PolledEventOutValue> EventOutValueMap;

    private:
        ISMap isMap;
        FieldValueMap unISdFieldValueMap;
        EventOutValueMap eventOutValueMap;
        MFNode implNodes;

    public:
        explicit ProtoNode(const NodeType & nodeType) throw (std::bad_alloc);
        ProtoNode(const NodeType & nodeType,
                  const ScopePtr & scope,
                  const ProtoNode & node) throw (std::bad_alloc);
        virtual ~ProtoNode() throw ();

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
        virtual Vrml97Node::Viewpoint * toViewpoint() const;

        virtual void render(Viewer & viewer, VrmlRenderContext context);

    private:
        // Not copyable.
        ProtoNode(const ProtoNode &);
        ProtoNode & operator=(const ProtoNode &);

        virtual void do_initialize(double timestamp) throw ();
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

namespace {

    struct InitializeScript_ : std::unary_function<ScriptNode *, void> {
        InitializeScript_(double time): time(time) {}

        void operator()(ScriptNode * const scriptNode) const {
            scriptNode->initialize(time);
        }

    private:
        double time;
    };
}

/**
 * @brief Constructor.
 *
 * @param out   output stream for console output.
 * @param err   output stream for error console output.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
Browser::Browser(std::ostream & out, std::ostream & err) throw (std::bad_alloc):
        scriptNodeClass(*this),
        scene(0),
        d_modified(false),
        d_newView(false),
        d_deltaTime(DEFAULT_DELTA),
        d_frameRate(0.0),
        d_firstEvent(0),
        d_lastEvent(0),
        out(out),
        err(err),
        d_flags_need_updating(false) {
    this->Browser::loadURI(MFString(), MFString());
}

/**
 * @brief Destructor.
 */
Browser::~Browser() throw () {}

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
void Browser::replaceWorld(const MFNode & nodes) {}

namespace {
    typedef std::map<std::string, NodeClassPtr> NodeClassMap;
    
    struct InitNodeClass : std::unary_function<void, NodeClassMap::value_type> {
        explicit InitNodeClass(const double time):
            time(time)
        {}
        
        void operator()(const NodeClassMap::value_type & value) const
        {
            assert(value.second);
            value.second->initialize(this->time);
        }

    private:
        double time;
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
void Browser::loadURI(const MFString & uri, const MFString & parameter)
        throw (std::bad_alloc) {
    //
    // Clear out the current Scene.
    //
    delete this->scene;
    this->scene = 0;
    this->d_navigationInfoStack.clear();
    this->d_viewpointStack.clear();
    assert(this->d_navigationInfos.empty());
    assert(this->d_viewpoints.empty());
    this->nodeClassMap.clear();
    
    //
    // Create the new Scene.
    //
    this->initNodeClassMap();
    this->scene = new Scene(*this, uri);
    
    const double timeNow = theSystem->time();

    this->scene->initialize(timeNow);
    std::for_each(this->nodeClassMap.begin(), this->nodeClassMap.end(),
                  InitNodeClass(timeNow));

    //
    // Send initial bind events to bindable nodes.
    //
    if (!this->d_navigationInfos.empty()) {
        assert(this->d_navigationInfos.front());
        this->d_navigationInfos.front()
                ->processEvent("set_bind", SFBool(true), timeNow);
    }

    if (!this->d_viewpoints.empty()) {
        assert(this->d_viewpoints.front());
        this->d_viewpoints.front()
                ->processEvent("set_bind", SFBool(true), timeNow);
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
const MFNode Browser::createVrmlFromStream(std::istream & in) {
    MFNode nodes;
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
void Browser::createVrmlFromURI(const MFString & uri,
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
 * @brief Get the root nodes for the browser.
 *
 * @return the root nodes for the browser.
 */
const MFNode & Browser::getRootNodes() const throw () {
    assert(this->scene);
    return this->scene->getNodes();
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
    if (currentTime <= 0.0) { currentTime = theSystem->time(); }

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
void Browser::render(Viewer & viewer) {
    if (d_newView) {
        viewer.resetUserNavigation();
        d_newView = false;
    }

    // Default viewpoint parameters
    float position[3] = { 0.0, 0.0, 10.0 };
    float orientation[4] = { 0.0, 0.0, 1.0, 0.0 };
    float field = 0.785398;
    float avatarSize = 0.25;
    float visibilityLimit = 0.0;

    Vrml97Node::Viewpoint * vp = bindableViewpointTop();
    if (vp) {
        std::copy(vp->getPosition().get(), vp->getPosition().get() + 3, position);
        std::copy(vp->getOrientation().get(), vp->getOrientation().get() + 4,
                  orientation);
        field = vp->getFieldOfView().get();
    }

    Vrml97Node::NavigationInfo * ni = bindableNavigationInfoTop();
    if (ni) {
        avatarSize = ni->getAvatarSize()[0];
        visibilityLimit = ni->getVisibilityLimit();
    }

    // Activate the headlight.
    // ambient is supposed to be 0 according to the spec...
    if (headlightOn()) {
        float rgb[3] = { 1.0, 1.0, 1.0 };
        float xyz[3] = { 0.0, 0.0, -1.0 };
        float ambient = 0.3;

        viewer.insertDirLight(ambient, 1.0, rgb, xyz);
    }

    // sets the viewpoint transformation
    //
    viewer.setViewpoint(position, orientation, field, avatarSize, visibilityLimit);
    
    std::for_each(this->nodeClassMap.begin(), this->nodeClassMap.end(),
                  RenderNodeClass(viewer));

    // Top level object

    viewer.beginObject(0);
    //
    // Hack alert: Right now the rendering code uses the old-style
    // set/unset Transform code, but the culling code accumulates the
    // modelview matrix on the core side using modifications to
    // VrmlTransform and the new VrmlRenderContext class.
    //
    // However, that means we need to jump through some hoops to make
    // sure that the new modelview transform code exactly shadows the
    // old code.
    //
    VrmlMatrix MV; // the modelview transform
    if (vp) {
        VrmlMatrix IM,NMAT;
        vp->inverseTransform(IM);   // put back nested viewpoint. skb
        viewer.transform(IM);
        vp->getInverseMatrix(MV);
        viewer.getUserNavigation(NMAT);
        MV = MV.multLeft(NMAT);
        MV = MV.multLeft(IM);
    } else {
        // if there's no viewpoint, then set us up arbitrarily at 0,0,-10,
        // as indicated in the vrml spec (section 6.53 Viewpoint).
        //
        float t[3] = { 0.0f, 0.0f, -10.0f };
        VrmlMatrix NMAT;
        MV.setTranslate(t);
        viewer.getUserNavigation(NMAT);
        MV = MV.multLeft(NMAT);
    }

    VrmlRenderContext rc(BVolume::partial, MV);
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
 * @brief Add a Viewpoint node to the list of Viewpoint nodes for the browser.
 *
 * @param node a Viewpoint node.
 *
 * @pre @p node is not in the list of Viewpoint nodes for the browser.
 */
void Browser::addViewpoint(Vrml97Node::Viewpoint & node) {
    assert(std::find(this->d_viewpoints.begin(), this->d_viewpoints.end(),
                     &node) == this->d_viewpoints.end());
    this->d_viewpoints.push_back(&node);
}

/**
 * @brief Remove a Viewpoint node from the list of Viewpoint nodes for the
 *      browser.
 *
 * @param node  a Viewpoint node.
 *
 * @pre @p node is in the list of Viewpoint nodes for the browser.
 */
void Browser::removeViewpoint(Vrml97Node::Viewpoint & node) {
    assert(!this->d_viewpoints.empty());
    const std::list<Node *>::iterator end = this->d_viewpoints.end();
    const std::list<Node *>::iterator pos =
            std::find(this->d_viewpoints.begin(), end, &node);
    assert(pos != end);
    this->d_viewpoints.erase(pos);
}

/**
 * @brief Get the active node on the bound Viewpoint stack.
 *
 * @return the active node on the bound Viewpoint stack.
 */
Vrml97Node::Viewpoint * Browser::bindableViewpointTop() {
    Node * const t = bindableTop(d_viewpointStack).get();
    return t ? t->toViewpoint() : 0;
}

/**
 * @brief Push a Viewpoint node onto the bound Viewpoint node stack.
 *
 * @param n a Viewpoint node.
 */
void Browser::bindablePush(Vrml97Node::Viewpoint * n) {
    bindablePush(d_viewpointStack, NodePtr(n));
    d_newView = true;
}

/**
 * @brief Remove a Viewpoint node from the bound Viewpoint node stack.
 *
 * @param n a Viewpoint node.
 */
void Browser::bindableRemove(Vrml97Node::Viewpoint * n) {
    bindableRemove(d_viewpointStack, NodePtr(n));
    d_newView = true;
}

/**
 * @brief Bind to the next Viewpoint in the list.
 */
void Browser::nextViewpoint() {
    Vrml97Node::Viewpoint *vp = bindableViewpointTop();
    std::list<Node *>::iterator i;

    for (i = d_viewpoints.begin(); i != d_viewpoints.end(); ++i ) {
        if (*i == vp) {
            if (++i == d_viewpoints.end())
              i = d_viewpoints.begin();

            if (*i && (vp = (*i)->toViewpoint())) {
                vp->processEvent("set_bind", SFBool(true), theSystem->time());
            }
            return;
        }
    }
}

/**
 * @brief Bind to the previous Viewpoint in the list.
 */
void Browser::prevViewpoint() {
    Vrml97Node::Viewpoint *vp = bindableViewpointTop();
    std::list<Node *>::iterator i;

    for (i = d_viewpoints.begin(); i != d_viewpoints.end(); ++i) {
        if (*i == vp) {
            if (i == d_viewpoints.begin()) {
                i = d_viewpoints.end();
            }
            if (*(--i) && (vp = (*i)->toViewpoint())) {
                vp->processEvent("set_bind", SFBool(true), theSystem->time());
            }
            return;
        }
    }
}

/**
 * @brief Get the number of Viewpoint nodes in the browser.
 *
 * @return the number of Viewpoint nodes in the browser.
 */
size_t Browser::nViewpoints() { return d_viewpoints.size(); }

/**
 * @brief Get the name and description of a Viewpoint.
 *
 * @param index         the index of the Viewpoint.
 * @retval name         the name of the Viewpoint.
 * @retval description  the description of the Viewpoint.
 */
void Browser::getViewpoint(const size_t index,
                           std::string & name,
                           std::string & description)
{
    size_t n = 0;
    for (std::list<Node *>::const_iterator i = this->d_viewpoints.begin();
            i != this->d_viewpoints.end(); ++i, ++n ) {
        if (n == index) {
            name = (*i)->getId();
            description = (*i)->toViewpoint()->getDescription().get();
            return;
        }
    }
}

/**
 * @brief Bind to a specific Viewpoint.
 *
 * @param name  the nodeId of the Viewpoint to bind to.
 *
 * This method will bind to a Viewpoint with the nodeId @p name. If no such
 * Viewpoint is available, this method has no effect.
 */
void Browser::setViewpoint(const std::string & name)
{
    for (std::list<Node *>::iterator i = this->d_viewpoints.begin();
            i != this->d_viewpoints.end(); ++i) {
        if (name == (*i)->getId()) {
            assert((*i)->toViewpoint());
            (*i)->processEvent("set_bind", SFBool(true), theSystem->time());
            return;
        }
    }
}

/**
 * @brief Bind to a Viewpoint with a particular index.
 *
 * @param index the index of the Viewpoint to bind to.
 *
 * If @p index is not a valid index, this method has no effect.
 */
void Browser::setViewpoint(const size_t index) {
    size_t j = 0;
    for (std::list<Node *>::iterator i = this->d_viewpoints.begin();
            i != this->d_viewpoints.end(); ++i, ++j) {
        if (j == index) {
            assert((*i)->toViewpoint());
            (*i)->processEvent("set_bind", SFBool(true), theSystem->time());
	    return;
        }
    }
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


namespace {
    
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

    const URI createFileURL(const URI & uri) throw ()
    {
        assert(uri.getScheme().empty());
        
        using std::string;
        
        string result = "file://";
        
# ifdef _WIN32
        char buffer[_MAX_PATH];
        const char * resolvedPath =
                _fullpath(buffer, uri.getPath().c_str(), _MAX_PATH);
# else
        char buffer[PATH_MAX];
        const char * resolvedPath = realpath(uri.getPath().c_str(), buffer);
# endif
        if (!resolvedPath) {
            //
            // XXX Failed; need to check errno to see what we should throw.
            //
            return result;
        }
        
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
Scene::Scene(Browser & browser, const MFString & uri, Scene * parent)
    throw (InvalidVrml, std::bad_alloc):
    browser(browser),
    parent(parent)
{
    std::string absoluteURI;
    for (size_t i = 0; i < uri.getLength(); ++i) {
        try {
            //
            // Throw InvalidURI if it isn't a valid URI.
            //
            URI testURI(uri.getElement(i));

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
                  ? uri.getElement(i)
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
 */
void Scene::initialize(const double timestamp) {
    for (size_t i = 0; i < this->nodes.getLength(); ++i) {
        assert(this->nodes.getElement(i));
        this->nodes.getElement(i)->initialize(*this, timestamp);
    }
}

/**
 * @brief Render the Scene.
 *
 * @param viewer    a Viewer to render to.
 * @param context   a VrmlRenderContext.
 */
void Scene::render(Viewer & viewer, VrmlRenderContext context) {
    for (size_t i = 0; i < this->nodes.getLength(); ++i) {
        this->nodes.getElement(i)->render(viewer, context);
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
void Scene::loadURI(const MFString & uri, const MFString & parameter)
    throw (std::bad_alloc)
{
    if (uri.getLength() > 0) {
        if (uri.getElement(0)[0] == '#') {
            //
            // If the first element in uri is a Viewpoint name, bind the
            // Viewpoint.
            //
            this->browser.setViewpoint(uri.getElement(0).substr(1));
        } else {
            MFString absoluteURIs(uri.getLength());
            for (size_t i = 0; i < absoluteURIs.getLength(); ++i) {
                try {
                    const URI uriElement(uri.getElement(i));
                    const std::string value =
                        uriElement.getScheme().empty()
                            ? uriElement.resolveAgainst(URI(this->getURI()))
                            : uriElement;
                    absoluteURIs.setElement(i, value);
                } catch (InvalidURI & ex) {
                    OPENVRML_PRINT_EXCEPTION_(ex);
                }
            }
            this->browser.loadURI(absoluteURIs, parameter);
        }
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
 * @class ProtoNode::NodeFieldCloneVisitor
 *
 * @brief A NodeVisitor that makes a deep copy of an SFNode or MFNode field
 *      value.
 *
 * SFNode and MFNode default field values must be cloned in order to be
 * propagated to a new PROTO instance.
 */

/**
 * @var const ScopePtr & ProtoNode::NodeFieldCloneVisitor::toScope
 *
 * @brief The Scope to which the clones should belong.
 */

/**
 * @var std::stack<NodePtr> ProtoNode::NodeFieldCloneVisitor::rootNodeStack
 *
 * @brief A stack for node clones.
 *
 * As the NodeVisitor traverses the node tree, it pushes the newly-created
 * cloned node onto the stack. The node at the top of the stack is then the
 * result of cloning any given subtree in the source node tree.
 */

/**
 * @brief Constructor.
 *
 * @param fromNode  a reference to the "source" node.
 * @param toNode    a reference to the "destination" node.
 */
ProtoNode::NodeFieldCloneVisitor::NodeFieldCloneVisitor(
        const ScopePtr & toScope)
    throw (std::bad_alloc):
    toScope(toScope)
{}

/**
 * @brief Destructor.
 */
ProtoNode::NodeFieldCloneVisitor::~NodeFieldCloneVisitor() throw ()
{}

/**
 * @brief Clone an SFNode field value.
 *
 * @param sfnode    the SFNode to clone.
 *
 * @return the clone.
 */
const SFNode ProtoNode::NodeFieldCloneVisitor::clone(const SFNode & sfnode)
{
    SFNode result;
    if (sfnode.get()) {
        Node & node = *sfnode.get();
        if (!node.accept(*this)) {
            assert(this->toScope->findNode(node.getId()));
            this->rootNodeStack
                    .push(NodePtr(this->toScope->findNode(node.getId())));
        }
        assert(this->rootNodeStack.top());
        result.set(this->rootNodeStack.top());
        this->rootNodeStack.pop();
        assert(this->rootNodeStack.empty());

        sfnode.get()->resetVisitedFlag();
    }
    return result;
}

/**
 * @brief Clone an MFNode field value.
 *
 * @param mfnode    the MFNode to clone.
 *
 * @return the clones.
 */
const MFNode ProtoNode::NodeFieldCloneVisitor::clone(const MFNode & mfnode)
{
    MFNode result(mfnode.getLength());
    size_t i;
    for (i = 0; i < result.getLength(); ++i) {
        if (mfnode.getElement(i)) {
            Node & node = *mfnode.getElement(i);
            if (!node.accept(*this)) {
                assert(this->toScope->findNode(node.getId()));
                this->rootNodeStack
                        .push(NodePtr(this->toScope->findNode(node.getId())));
            }
            assert(this->rootNodeStack.top());
            result.setElement(i, this->rootNodeStack.top());
            this->rootNodeStack.pop();
        }
    }
    assert(this->rootNodeStack.empty());

    for (i = 0; i < mfnode.getLength(); ++i) {
        if (mfnode.getElement(i)) {
            mfnode.getElement(i)->resetVisitedFlag();
        }
    }
    return result;
}

namespace {

    /**
     * @internal
     */
    struct CloneFieldValue_ : std::unary_function<NodeInterface, void> {
        CloneFieldValue_(NodeVisitor & visitor,
                         std::stack<NodePtr> & rootNodeStack,
                         Scope & scope,
                         Node & fromNode, Node & toNode):
            visitor(&visitor),
            rootNodeStack(&rootNodeStack),
            scope(&scope),
            fromNode(&fromNode),
            toNode(&toNode)
        {}

        void operator()(const NodeInterface & interface) const
        {
            if (interface.type == NodeInterface::field
                    || interface.type == NodeInterface::exposedField) {
                if (interface.fieldType == FieldValue::sfnode) {
                    const SFNode & value =
                            static_cast<const SFNode &>
                                (this->fromNode->getField(interface.id));
                    if (value.get()) {
                        Node & childNode = *value.get();
                        if (!childNode.accept(*this->visitor)) {
                            assert(this->scope->findNode(childNode.getId()));
                            this->rootNodeStack
                                    ->push(NodePtr(this->scope->findNode(childNode.getId())));
                        }
                        assert(this->rootNodeStack->top());
                        this->toNode->setField(interface.id,
                                               SFNode(this->rootNodeStack->top()));
                        this->rootNodeStack->pop();
                    }
                } else if (interface.fieldType == FieldValue::mfnode) {
                    const MFNode & children =
                            static_cast<const MFNode &>
                                (this->fromNode->getField(interface.id));
                    MFNode clonedChildren(children.getLength());
                    for (size_t i = 0; i < clonedChildren.getLength(); ++i) {
                        if (children.getElement(i)) {
                            Node & childNode = *children.getElement(i);
                            if (!childNode.accept(*this->visitor)) {
                                assert(this->scope->findNode(childNode.getId()));
                                this->rootNodeStack
                                        ->push(NodePtr(this->scope->findNode(childNode.getId())));
                            }
                            assert(this->rootNodeStack->top());
                            clonedChildren.setElement(i, this->rootNodeStack->top());
                            this->rootNodeStack->pop();
                        }
                    }
                    this->toNode->setField(interface.id, clonedChildren);
                } else {
                    this->toNode->setField(interface.id,
                                           this->fromNode->getField(interface.id));
                }
            }
        }

    private:
        NodeVisitor * visitor;
        std::stack<NodePtr> * rootNodeStack;
        Scope * scope;
        Node * fromNode;
        Node * toNode;
    };
}

/**
 * @brief Visit a Node.
 *
 * @param node  a Node.
 */
void ProtoNode::NodeFieldCloneVisitor::visit(Node & node)
{
    //
    // Create a new node of the same type.
    //
    const NodePtr newNode(node.nodeType.createNode(this->toScope));
    this->rootNodeStack.push(newNode);

    //
    // If the node has a name, give it to the new node.
    //
    if (!node.getId().empty()) { newNode->setId(node.getId()); }

    //
    // Copy the field values.
    //
    const NodeInterfaceSet & interfaces = node.nodeType.getInterfaces();
    std::for_each(interfaces.begin(), interfaces.end(),
                  CloneFieldValue_(*this, this->rootNodeStack, *this->toScope,
                                   node, *newNode));
}


/**
 * @internal
 *
 * @class ProtoNode::ProtoImplCloneVisitor
 *
 * @brief A NodeVisitor that makes a deep copy of the PROTO implementation.
 *
 * The PROTO implementation nodes must be cloned when creating a new instance.
 */

/**
 * @var std::stack<NodePtr> ProtoNode::ProtoImplCloneVisitor::rootNodeStack
 *
 * @brief A stack for node clones.
 *
 * As the NodeVisitor traverses the node tree, it pushes the newly-created
 * cloned node onto the stack. The node at the top of the stack is then the
 * result of cloning any given subtree in the source node tree.
 */

/**
 * @var const ProtoNode & ProtoNode::ProtoImplCloneVisitor::fromProtoNode
 *
 * @brief A reference to the "source" node.
 */

/**
 * @var const ProtoNode & ProtoNode::ProtoImplCloneVisitor::toProtoNode
 *
 * @brief A reference to the "destination" node.
 */

/**
 * @brief Constructor.
 *
 * @param fromNode  a reference to the "source" node.
 * @param toNode    a reference to the "destination" node.
 */
ProtoNode::ProtoImplCloneVisitor::ProtoImplCloneVisitor(
        const ProtoNode & fromProtoNode,
        ProtoNode & toProtoNode)
    throw (std::bad_alloc):
    fromProtoNode(fromProtoNode),
    toProtoNode(toProtoNode)
{}

/**
 * @brief Destructor.
 */
ProtoNode::ProtoImplCloneVisitor::~ProtoImplCloneVisitor() throw ()
{}

/**
 * @brief Clone the nodes.
 */
void ProtoNode::ProtoImplCloneVisitor::clone() throw (std::bad_alloc)
{
    assert(this->fromProtoNode.implNodes.getElement(0));
    this->toProtoNode.implNodes
            .setLength(this->fromProtoNode.implNodes.getLength());
    size_t i;
    for (i = 0; i < this->toProtoNode.implNodes.getLength(); ++i) {
        if (this->fromProtoNode.implNodes.getElement(i)) {
            Node & childNode = *this->fromProtoNode.implNodes.getElement(i);
            if (!childNode.accept(*this)) {
                assert(this->toProtoNode.getScope()->findNode(childNode.getId()));
                this->rootNodeStack
                    .push(NodePtr(this->toProtoNode.getScope()->findNode(childNode.getId())));
            }
            assert(this->rootNodeStack.top());
            this->toProtoNode.implNodes.setElement(i, this->rootNodeStack.top());
            this->rootNodeStack.pop();
        }
    }
    assert(this->rootNodeStack.empty());

    for (i = 0; i < this->fromProtoNode.implNodes.getLength(); ++i) {
        assert(this->fromProtoNode.implNodes.getElement(i));
        this->fromProtoNode.implNodes.getElement(i)->resetVisitedFlag();
    }
}

/**
 * @brief Visit a Node.
 *
 * @param node  a Node.
 */
void ProtoNode::ProtoImplCloneVisitor::visit(Node & node) throw (std::bad_alloc)
{
    //
    // Create a new node of the same type.
    //
    const NodePtr newNode(node.nodeType.createNode(this->toProtoNode.getScope()));
    this->rootNodeStack.push(newNode);

    //
    // If the node has a name, give it to the new node.
    //
    if (!node.getId().empty()) { newNode->setId(node.getId()); }

    //
    // Any IS mappings for this node?
    //
    for (ISMap::const_iterator itr(this->fromProtoNode.isMap.begin());
            itr != this->fromProtoNode.isMap.end(); ++itr) {
        if (&itr->second.node == &node) {
            try {
                this->toProtoNode.addIS(*newNode, itr->second.interfaceId,
                                        itr->first);
            } catch (std::bad_alloc &) {
                throw;
            } catch (std::runtime_error & ex) {
                OPENVRML_PRINT_EXCEPTION_(ex);
                assert(false);
            }
        }
    }

    //
    // Copy the field values.
    //
    const NodeInterfaceSet & interfaces = node.nodeType.getInterfaces();
    std::for_each(interfaces.begin(), interfaces.end(),
                  CloneFieldValue_(*this, this->rootNodeStack,
                                   *this->toProtoNode.getScope(),
                                   node, *newNode));
}

/**
 * @internal
 *
 * @class ProtoNode::RouteCopyVisitor
 *
 * @brief Copy the routes.
 *
 * Given two topologically identical node trees, copy the routes in the first
 * tree to analogous routes in the second tree.
 */

/**
 * @brief Constructor.
 *
 * @param fromProtoNode a reference to the "source" node.
 * @param toProtoNode   a reference to the "destination" node.
 */
ProtoNode::RouteCopyVisitor::RouteCopyVisitor(const ProtoNode & fromProtoNode,
                                              ProtoNode & toProtoNode)
    throw ():
    fromProtoNode(fromProtoNode),
    toProtoNode(toProtoNode)
{}

/**
 * @brief Destructor.
 */
ProtoNode::RouteCopyVisitor::~RouteCopyVisitor() throw ()
{}

/**
 * @brief Copy the ROUTEs.
 */
void ProtoNode::RouteCopyVisitor::copyRoutes() {
	size_t i;
    for (i = 0; i < this->fromProtoNode.implNodes.getLength(); ++i) {
        if (this->fromProtoNode.implNodes.getElement(i)) {
            this->fromProtoNode.implNodes.getElement(i)->accept(*this);
        }
    }

    for (i = 0; i < this->fromProtoNode.implNodes.getLength(); ++i) {
        this->fromProtoNode.implNodes.getElement(i)->resetVisitedFlag();
    }
}

namespace {

    struct AddRoute_ : std::unary_function<Node::Route, void> {
        AddRoute_(Scope & scope, Node & fromNode):
                  scope(&scope), fromNode(&fromNode) {}

        void operator()(const Node::Route & route) const {
            const std::string & toNodeId = route.toNode->getId();
            assert(this->scope->findNode(toNodeId));
            fromNode->addRoute(route.fromEventOut,
                               NodePtr(this->scope->findNode(toNodeId)),
                               route.toEventIn);
        }

    private:
        Scope * scope;
        Node * fromNode;
    };
}

/**
 * @brief Visit a node.
 *
 * @param node  a Node.
 */
void ProtoNode::RouteCopyVisitor::visit(Node & node) {
    const std::string & fromNodeId = node.getId();
    if (!fromNodeId.empty()) {
        Node * const fromNode =
                this->toProtoNode.getScope()->findNode(fromNodeId);
        assert(fromNode);
        std::for_each(node.getRoutes().begin(), node.getRoutes().end(),
                      AddRoute_(*this->toProtoNode.getScope(), *fromNode));
    }

    //
    // Visit the children.
    //
    const NodeInterfaceSet & interfaces = node.nodeType.getInterfaces();
    for (NodeInterfaceSet::const_iterator interface(interfaces.begin());
            interface != interfaces.end(); ++interface) {
        if (interface->type == NodeInterface::exposedField
                || interface->type == NodeInterface::field) {
            if (interface->fieldType == FieldValue::sfnode) {
                assert(dynamic_cast<const SFNode *>
                       (&node.getField(interface->id)));
                const SFNode & sfnode =
                    static_cast<const SFNode &>(node.getField(interface->id));
                if (sfnode.get()) { sfnode.get()->accept(*this); }
            } else if (interface->fieldType == FieldValue::mfnode) {
                assert(dynamic_cast<const MFNode *>
                       (&node.getField(interface->id)));
                const MFNode & mfnode =
                    static_cast<const MFNode &>(node.getField(interface->id));
                for (size_t i = 0; i < mfnode.getLength(); ++i) {
                    if (mfnode.getElement(i)) {
                        mfnode.getElement(i)->accept(*this);
                    }
                }
            }
        }
    }
}

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
    assert(node.implNodes.getLength() > 0);
    assert(node.implNodes.getElement(0));

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
    ProtoImplCloneVisitor(node, *this).clone();
    RouteCopyVisitor(node, *this).copyRoutes();
    
    //
    // Finally, we initialize the implementation using the PROTO's default
    // field values.
    //
    typedef ProtoNodeClass::DefaultValueMap DefaultValueMap;
    DefaultValueMap & defaultValueMap =
            static_cast<ProtoNodeClass &>(nodeType.nodeClass).defaultValueMap;
    const ScopePtr & protoScope = this->implNodes.getElement(0)->getScope();
    NodeFieldCloneVisitor nodeFieldCloneVisitor(protoScope);
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
            fieldValue.reset(new SFNode(nodeFieldCloneVisitor.clone(node)));
        } else if (type == FieldValue::mfnode) {
            const MFNode & nodes = static_cast<const MFNode &>(*i->second);
            fieldValue.reset(new MFNode(nodeFieldCloneVisitor.clone(nodes)));
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
ProtoNode::~ProtoNode() throw () {
    //
    // Remove from the browser.
    //
    if (this->getScene()) {
        this->getScene()->browser.removeProto(*this);
    }
}

/**
 * @brief Cast to a ScriptNode.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a ScriptNode, or 0 otherwise.
 */
const ScriptNode * ProtoNode::toScript() const throw () {
    assert(this->implNodes.getLength() != 0);
    assert(this->implNodes.getElement(0).get());
    return this->implNodes.getElement(0)->toScript();
}

/**
 * @brief Cast to a ScriptNode.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a ScriptNode, or 0 otherwise.
 */
ScriptNode * ProtoNode::toScript() throw () {
    assert(this->implNodes.getLength() != 0);
    assert(this->implNodes.getElement(0).get());
    return this->implNodes.getElement(0)->toScript();
}

/**
 * @brief Cast to an AppearanceNode.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      an AppearanceNode, or 0 otherwise.
 */
const AppearanceNode * ProtoNode::toAppearance() const throw () {
    assert(this->implNodes.getLength() != 0);
    assert(this->implNodes.getElement(0).get());
    return this->implNodes.getElement(0)->toAppearance();
}

/**
 * @brief Cast to an AppearanceNode.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      an AppearanceNode, or 0 otherwise.
 */
AppearanceNode * ProtoNode::toAppearance() throw () {
    assert(this->implNodes.getLength() != 0);
    assert(this->implNodes.getElement(0).get());
    return this->implNodes.getElement(0)->toAppearance();
}

/**
 * @brief Cast to a ChildNode.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a ChildNode, or 0 otherwise.
 */
const ChildNode * ProtoNode::toChild() const throw () {
    assert(this->implNodes.getLength() != 0);
    assert(this->implNodes.getElement(0).get());
    return this->implNodes.getElement(0)->toChild();
}

/**
 * @brief Cast to a ChildNode.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a ChildNode, or 0 otherwise.
 */
ChildNode * ProtoNode::toChild() throw () {
    assert(this->implNodes.getLength() != 0);
    assert(this->implNodes.getElement(0).get());
    return this->implNodes.getElement(0)->toChild();
}

/**
 * @brief Cast to a ColorNode.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a ColorNode, or 0 otherwise.
 */
const ColorNode * ProtoNode::toColor() const throw () {
    assert(this->implNodes.getLength() != 0);
    assert(this->implNodes.getElement(0).get());
    return this->implNodes.getElement(0)->toColor();
}

/**
 * @brief Cast to a ColorNode.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a ColorNode, or 0 otherwise.
 */
ColorNode * ProtoNode::toColor() throw () {
    assert(this->implNodes.getLength() != 0);
    assert(this->implNodes.getElement(0).get());
    return this->implNodes.getElement(0)->toColor();
}

/**
 * @brief Cast to a CoordinateNode.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a CoordinateNode, or 0 otherwise.
 */
const CoordinateNode * ProtoNode::toCoordinate() const throw () {
    assert(this->implNodes.getLength() != 0);
    assert(this->implNodes.getElement(0).get());
    return this->implNodes.getElement(0)->toCoordinate();
}

/**
 * @brief Cast to a CoordinateNode.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a CoordinateNode, or 0 otherwise.
 */
CoordinateNode * ProtoNode::toCoordinate() throw () {
    assert(this->implNodes.getLength() != 0);
    assert(this->implNodes.getElement(0).get());
    return this->implNodes.getElement(0)->toCoordinate();
}

/**
 * @brief Cast to a FontStyleNode.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a FontStyleNode, or 0 otherwise.
 */
const FontStyleNode * ProtoNode::toFontStyle() const throw () {
    assert(this->implNodes.getLength() != 0);
    assert(this->implNodes.getElement(0).get());
    return this->implNodes.getElement(0)->toFontStyle();
}

/**
 * @brief Cast to a FontStyleNode.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a FontStyleNode, or 0 otherwise.
 */
FontStyleNode * ProtoNode::toFontStyle() throw () {
    assert(this->implNodes.getLength() != 0);
    assert(this->implNodes.getElement(0).get());
    return this->implNodes.getElement(0)->toFontStyle();
}

/**
 * @brief Cast to a GeometryNode.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a GeometryNode, or 0 otherwise.
 */
const GeometryNode * ProtoNode::toGeometry() const throw () {
    assert(this->implNodes.getLength() != 0);
    assert(this->implNodes.getElement(0).get());
    return this->implNodes.getElement(0)->toGeometry();
}

/**
 * @brief Cast to a GeometryNode.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a GeometryNode, or 0 otherwise.
 */
GeometryNode * ProtoNode::toGeometry() throw () {
    assert(this->implNodes.getLength() != 0);
    assert(this->implNodes.getElement(0).get());
    return this->implNodes.getElement(0)->toGeometry();
}

/**
 * @brief Cast to a MaterialNode.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a MaterialNode, or 0 otherwise.
 */
const MaterialNode * ProtoNode::toMaterial() const throw () {
    assert(this->implNodes.getLength() != 0);
    assert(this->implNodes.getElement(0).get());
    return this->implNodes.getElement(0)->toMaterial();
}

/**
 * @brief Cast to a MaterialNode.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a MaterialNode, or 0 otherwise.
 */
MaterialNode * ProtoNode::toMaterial() throw () {
    assert(this->implNodes.getLength() != 0);
    assert(this->implNodes.getElement(0).get());
    return this->implNodes.getElement(0)->toMaterial();
}

/**
 * @brief Cast to a NormalNode.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a NormalNode, or 0 otherwise.
 */
const NormalNode * ProtoNode::toNormal() const throw () {
    assert(this->implNodes.getLength() != 0);
    assert(this->implNodes.getElement(0).get());
    return this->implNodes.getElement(0)->toNormal();
}

/**
 * @brief Cast to a NormalNode.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a NormalNode, or 0 otherwise.
 */
NormalNode * ProtoNode::toNormal() throw () {
    assert(this->implNodes.getLength() != 0);
    assert(this->implNodes.getElement(0).get());
    return this->implNodes.getElement(0)->toNormal();
}

/**
 * @brief Cast to a SoundSourceNode.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a SoundSourceNode, or 0 otherwise.
 */
const SoundSourceNode * ProtoNode::toSoundSource() const throw () {
    assert(this->implNodes.getLength() != 0);
    assert(this->implNodes.getElement(0).get());
    return this->implNodes.getElement(0)->toSoundSource();
}

/**
 * @brief Cast to a SoundSourceNode.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a SoundSourceNode, or 0 otherwise.
 */
SoundSourceNode * ProtoNode::toSoundSource() throw () {
    assert(this->implNodes.getLength() != 0);
    assert(this->implNodes.getElement(0).get());
    return this->implNodes.getElement(0)->toSoundSource();
}

/**
 * @brief Cast to a TextureNode.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a TextureNode, or 0 otherwise.
 */
const TextureNode * ProtoNode::toTexture() const throw () {
    assert(this->implNodes.getLength() != 0);
    assert(this->implNodes.getElement(0).get());
    return this->implNodes.getElement(0)->toTexture();
}

/**
 * @brief Cast to a TextureNode.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a TextureNode, or 0 otherwise.
 */
TextureNode * ProtoNode::toTexture() throw () {
    assert(this->implNodes.getLength() != 0);
    assert(this->implNodes.getElement(0).get());
    return this->implNodes.getElement(0)->toTexture();
}

/**
 * @brief Cast to a TextureCoordinateNode.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a TextureCoordinateNode, or 0 otherwise.
 */
const TextureCoordinateNode * ProtoNode::toTextureCoordinate() const throw () {
    assert(this->implNodes.getLength() != 0);
    assert(this->implNodes.getElement(0).get());
    return this->implNodes.getElement(0)->toTextureCoordinate();
}

/**
 * @brief Cast to a TextureCoordinateNode.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a TextureCoordinateNode, or 0 otherwise.
 */
TextureCoordinateNode * ProtoNode::toTextureCoordinate() throw () {
    assert(this->implNodes.getLength() != 0);
    assert(this->implNodes.getElement(0).get());
    return this->implNodes.getElement(0)->toTextureCoordinate();
}

/**
 * @brief Cast to a TextureTransformNode.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a TextureTransformNode, or 0 otherwise.
 */
const TextureTransformNode * ProtoNode::toTextureTransform() const throw () {
    assert(this->implNodes.getLength() != 0);
    assert(this->implNodes.getElement(0).get());
    return this->implNodes.getElement(0)->toTextureTransform();
}

/**
 * @brief Cast to a TextureTransformNode.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a TextureTransformNode, or 0 otherwise.
 */
TextureTransformNode * ProtoNode::toTextureTransform() throw () {
    assert(this->implNodes.getLength() != 0);
    assert(this->implNodes.getElement(0).get());
    return this->implNodes.getElement(0)->toTextureTransform();
}

/**
 * @brief Add a root node to the prototype definition.
 */
void ProtoNode::addRootNode(const NodePtr & node) throw (std::bad_alloc) {
    assert(node);
    this->implNodes.addNode(node);
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
                    InterfaceIdEquals_(protoInterfaceId));
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
 */
void ProtoNode::do_initialize(const double timestamp) throw ()
{
    assert(this->getScene());
    
    for (size_t i = 0; i < this->implNodes.getLength(); ++i) {
        assert(this->implNodes.getElement(i));
        this->implNodes.getElement(i)->initialize(*this->getScene(), timestamp);
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

Vrml97Node::Anchor * ProtoNode::toAnchor() const {
    return this->implNodes.getElement(0)->toAnchor();
}

Vrml97Node::AudioClip * ProtoNode::toAudioClip() const {
    return this->implNodes.getElement(0)->toAudioClip();
}

Vrml97Node::CylinderSensor * ProtoNode::toCylinderSensor() const {
    return this->implNodes.getElement(0)->toCylinderSensor();
}

Vrml97Node::AbstractLight * ProtoNode::toLight() const {
    return this->implNodes.getElement(0)->toLight();
}

Vrml97Node::MovieTexture * ProtoNode::toMovieTexture() const {
    return this->implNodes.getElement(0)->toMovieTexture();
}

Vrml97Node::NavigationInfo * ProtoNode::toNavigationInfo() const {
    return this->implNodes.getElement(0)->toNavigationInfo();
}

Vrml97Node::PlaneSensor * ProtoNode::toPlaneSensor() const {
    return this->implNodes.getElement(0)->toPlaneSensor();
}

Vrml97Node::PointLight * ProtoNode::toPointLight() const {
    return this->implNodes.getElement(0)->toPointLight();
}

Vrml97Node::SphereSensor * ProtoNode::toSphereSensor() const {
    return this->implNodes.getElement(0)->toSphereSensor();
}

Vrml97Node::SpotLight * ProtoNode::toSpotLight() const {
    return this->implNodes.getElement(0)->toSpotLight();
}

Vrml97Node::TimeSensor * ProtoNode::toTimeSensor() const {
    return this->implNodes.getElement(0)->toTimeSensor();
}

Vrml97Node::TouchSensor * ProtoNode::toTouchSensor() const {
    return this->implNodes.getElement(0)->toTouchSensor();
}

Vrml97Node::Viewpoint * ProtoNode::toViewpoint() const {
    return this->implNodes.getElement(0)->toViewpoint();
}

void ProtoNode::render(Viewer & viewer, const VrmlRenderContext context) {
    assert(this->implNodes.getElement(0));
    this->implNodes.getElement(0)->render(viewer, context);
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
        const std::list<string>::iterator begin(pathSegments.begin());
        std::list<string>::iterator pos;
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
        result += this->getQuery();
        result += this->getFragment();

        return result;
    }
    
} // namespace

} // namespace OpenVRML
