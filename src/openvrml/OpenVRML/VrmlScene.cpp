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

# if defined(_WIN32) && !defined(__CYGWIN__)
#   include <winconfig.h>
# endif

# include <errno.h>
# include <stdio.h>
# if defined(_WIN32) && !defined(__CYGWIN__)
#   include <strstrea.h>
# else
#   include <strstream.h>
# endif
# include <algorithm>
# include <stack>
# include "VrmlScene.h"
# include "doc2.hpp"
# include "Viewer.h"
# include "System.h"
# include "MathUtils.h"
# include "VrmlNamespace.h"
# include "VrmlRenderContext.h"
# include "script.h"

namespace OpenVRML {

    namespace Vrml97Node {
        class Anchor;
        class AudioClip;
        class Background;
        class CylinderSensor;
        class Fog;
        class Group;
        class Inline;
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
        
        class NodeCloneVisitor : public NodeVisitor {
            std::stack<NodePtr> rootNodeStack;

            const ProtoNode & fromProtoNode;
            ProtoNode & toProtoNode;

        public:
            NodeCloneVisitor(const ProtoNode & fromProtoNode,
                             ProtoNode & toProtoNode);
            virtual ~NodeCloneVisitor();

            void clone();

            virtual void visit(Node & node);

        private:
            // Not copyable.
            NodeCloneVisitor(const NodeCloneVisitor &);
            NodeCloneVisitor & operator=(const NodeCloneVisitor &);
        };
        
        friend class NodeCloneVisitor;
        
        class RouteCopyVisitor : public NodeVisitor {
            const ProtoNode & fromProtoNode;
            ProtoNode & toProtoNode;

        public:
            RouteCopyVisitor(const ProtoNode & fromProtoNode,
                             ProtoNode & toProtoNode);
            virtual ~RouteCopyVisitor() {}

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
        typedef std::map<std::string, PolledEventOutValue> EventOutValueMap;

    private:
        ISMap isMap;
        EventOutValueMap eventOutValueMap;
        VrmlNamespace scope;
        MFNode implNodes;
        
    public:
        ProtoNode(const NodeType & nodeType, VrmlNamespace & parentScope);
        ProtoNode(const NodeType & nodeType, const ProtoNode & node);
        virtual ~ProtoNode() throw ();
        
        void addRootNode(const NodePtr & node) throw (std::bad_alloc);
        void addIS(Node & implNode,
                   const std::string & implNodeInterfaceId,
                   const std::string & protoInterfaceId)
                throw (std::invalid_argument, std::bad_alloc);
        
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
        virtual Vrml97Node::Background * toBackground() const;
        virtual Vrml97Node::CylinderSensor * toCylinderSensor() const;
        virtual Vrml97Node::Fog * toFog() const;
        virtual Vrml97Node::Group * toGroup() const;
        virtual Vrml97Node::Inline * toInline() const;
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
        
        virtual void render(Viewer *, VrmlRenderContext rc);

    private:
        // Not copyable.
        ProtoNode(const ProtoNode &);
        ProtoNode & operator=(const ProtoNode &);
        
        virtual void setFieldImpl(const std::string & id,
                                  const FieldValue & value)
                throw (UnsupportedInterface, std::bad_cast, std::bad_alloc);
        
        virtual const FieldValue & getFieldImpl(const std::string & id) const
                throw (UnsupportedInterface);
        
        virtual void processEventImpl(const std::string & id,
                                      const FieldValue & value,
                                      double timestamp)
                throw (UnsupportedInterface, std::bad_cast, std::bad_alloc);
        
        virtual const FieldValue & getEventOutImpl(const std::string & id) const
                throw (UnsupportedInterface);
    };
    
    
    class ProtoNodeClass : public NodeClass {
        
        friend class Vrml97Parser;
        
    public:
        friend class ProtoNodeType : public NodeType {
            NodeInterfaceSet nodeInterfaces;

        public:
            ProtoNodeType(ProtoNodeClass & nodeClass, const std::string & id)
                    throw (UnsupportedInterface, std::bad_alloc);
            virtual ~ProtoNodeType() throw ();

            virtual const NodeInterfaceSet & getInterfaces() const throw ();
            virtual const NodePtr createNode() const throw (std::bad_alloc);
            
            void addInterface(const NodeInterface & interface)
                    throw (std::invalid_argument, std::bad_alloc);
        };

    private:
        typedef std::map<std::string, FieldValuePtr> DefaultValueMap;
        
        ProtoNodeType protoNodeType;
        DefaultValueMap defaultValueMap;
        ProtoNode protoNode;

    public:
        ProtoNodeClass(VrmlScene & scene) throw ();
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
                throw (std::invalid_argument, std::bad_alloc);
        
        virtual const NodeTypePtr createType(const std::string & id,
                                             const NodeInterfaceSet &)
                throw (UnsupportedInterface, std::bad_alloc);
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
 * @typedef NodeClassMap
 *
 * @brief Maps URIs to @link NodeClass NodeClasses@endlink.
 */


/**
 * @class VrmlScene
 *
 * @brief Encapsulates a VRML scene.
 */

/**
 * @var VrmlScene::Vrml97Parser
 *
 * @brief VRML97 parser generated by ANTLR.
 */

/**
 * @var VrmlScene::ProtoNodeClass
 *
 * @brief Class object for @link ProtoNodes@endlink.
 */

/**
 * @var VrmlScene::Vrml97Node::Inline
 *
 * @brief VRML97 Inline node.
 *
 * @todo Fix Inline nodes.
 */

/**
 * @enum VrmlScene::CBReason
 *
 * @brief Valid reasons for scene callback.
 */

/**
 * @var VrmlScene::CBReason VrmlScene::DESTROY_WORLD
 *
 * @brief Destroy the world.
 */

/**
 * @var VrmlScene::CBReason VrmlScene::REPLACE_WORLD
 *
 * @brief Replace the world.
 */

/**
 * @typedef VrmlScene::SceneCB
 *
 * @brief A pointer to a scene callback function.
 *
 * The callback function provoides a way to let the app know when a world is
 * loaded, changed, etc.
 */

/**
 * @var NodeClassMap VrmlScene::nodeClassMap
 *
 * @brief A map of URIs to node implementations.
 */

/**
 * @var ScriptNodeClass VrmlScene::scriptNodeClass
 *
 * @brief NodeClass for Script nodes in the scene.
 */

/**
 * @var Doc2 * VrmlScene::d_url
 *
 * @brief Doc2 object associated with the scene.
 */

/**
 * @var MFNode VrmlScene::nodes
 *
 * @brief The root nodes for the scene.
 */

/**
 * @var VrmlNamespace * VrmlScene::scope
 *
 * @brief The root scope for the scene.
 */

/**
 * @typedef VrmlScene::BindStack
 *
 * @brief A list of bound nodes.
 */

/**
 * @var VrmlScene::BindStack VrmlScene::d_backgroundStack
 *
 * @brief The stack of bound Background nodes.
 */

/**
 * @var VrmlScene::BindStack VrmlScene::d_fogStack
 *
 * @brief The stack of bound Fog nodes.
 */

/**
 * @var VrmlScene::BindStack VrmlScene::d_navigationInfoStack
 *
 * @brief The stack of bound NavigationInfo nodes.
 */

/**
 * @var VrmlScene::BindStack VrmlScene::d_viewpointStack
 *
 * @brief The stack of bound Viewpoint nodes.
 */

/**
 * @var std::list<Node *> VrmlScene::d_backgrounds
 *
 * @brief A list of all the Background nodes in the scene.
 */

/**
 * @var std::list<Node *> VrmlScene::d_fogs
 *
 * @brief A list of all the Fog nodes in the scene.
 */

/**
 * @var std::list<Node *> VrmlScene::d_navigationInfos
 *
 * @brief A list of all the NavigationInfo nodes in the scene.
 */

/**
 * @var std::list<Node *> VrmlScene::d_viewpoints
 *
 * @brief A list of all the Viewpoint nodes in the scene.
 */

/**
 * @var std::list<Node *> VrmlScene::d_scopedLights
 *
 * @brief A list of all the scoped light nodes in the scene.
 */

/**
 * @var std::list<ScriptNode *> VrmlScene::d_scripts
 *
 * @brief A list of all the Script nodes in the scene.
 */

/**
 * @var std::list<Node *> VrmlScene::d_timers
 *
 * @brief A list of all the TimeSensor nodes in the scene.
 */

/**
 * @var std::list<Node *> VrmlScene::d_audioClips
 *
 * @brief A list of all the AudioClip nodes in the scene.
 */

/**
 * @var std::list<Node *> VrmlScene::d_movies
 *
 * @brief A list of all the MovieTexture nodes in the scene.
 */

/**
 * @var std::list<Node *> VrmlScene::protoNodeList
 *
 * @brief A list of all the prototype nodes in the scene.
 */

/**
 * @var bool VrmlScene::d_modified
 *
 * @brief Flag to indicate whether the scene has been modified.
 */

/**
 * @var bool VrmlScene::d_newView
 *
 * @brief Flag to indicate if the user has changed to a new view.
 */

/**
 * @var double VrmlScene::d_deltaTime
 *
 * @brief Time elapsed since the last update.
 */

/**
 * @typedef VrmlScene::SceneCBList
 *
 * @brief List of functions to call when the world is changed.
 */

/**
 * @struct VrmlScene::Event
 *
 * @brief An event.
 *
 * An event has a value and a destination, and is associated with a time.
 */

/**
 * @var double VrmlScene::Event::timeStamp
 *
 * @brief The timestamp of the event.
 */

/**
 * @var FieldValue * VrmlScene::Event::value
 *
 * @brief The value associated with the event.
 */

/**
 * @var NodePtr VrmlScene::Event::toNode
 *
 * @brief The Node the event is going to.
 */

/**
 * @var std::string VrmlScene::Event::toEventIn
 *
 * @brief The eventIn of @a toNode the event is going to.
 */

/**
 * @var MFString * VrmlScene::d_pendingUrl
 *
 * @brief Allow requests to load URLs; nodes to wait until all events are
 *      processed.
 */

/**
 * @var MFString * VrmlScene::d_pendingParameters
 *
 * @brief Parameters associated with @a d_pendingNodes.
 */

/**
 * @var VrmlScene::SceneCBList VrmlScene::d_sceneCallbacks
 *
 * @brief List of functions to call when the world is changed.
 */

/**
 * @var double VrmlScene::d_frameRate
 *
 * @brief Frame rate.
 */

/**
 * @var VrmlScene::MAXEVENTS
 *
 * @brief The maximum number of events which may be queued.
 *
 * Each scene can have a limited number of pending events.
 * Repeatedly allocating/freeing events is slow (it would be
 * nice to get rid of the field cloning, too), and if there are
 * so many events pending, we are probably running too slow to
 * handle them effectively anyway.
 */

/**
 * @var VrmlScene::Event VrmlScene::d_eventMem
 *
 * @brief The event queue.
 *
 * @todo The event queue ought to be sorted by timeStamp.
 */

/**
 * @var bool VrmlScene::d_flags_need_updating
 *
 * @brief Set by Node::setBVolumeDirty on any node in this scene graph,
 *      cleared by updateFlags.
 *
 * @c true if the bvolume dirty flag has been set on a node in the
 * scene graph, but has not yet been propegated to that node's
 * ancestors. 
 */

/**
 * @brief Create a VrmlScene from a URI.
 *
 * @param url   a URI.
 */
VrmlScene::VrmlScene(const std::string & url):
        scriptNodeClass(*this), d_flags_need_updating(false),
        d_url(new Doc2(url)), scope(0), d_modified(false), d_newView(false),
        d_deltaTime(DEFAULT_DELTA), d_pendingUrl(0), d_pendingParameters(0),
        d_pendingNodes(0), d_pendingScope(0), d_frameRate(0.0), d_firstEvent(0),
        d_lastEvent(0) {
    this->initNodeClassMap();
    this->scope = new Vrml97RootNamespace(this->nodeClassMap);

    MFNode * newNodes = this->readWrl(this->d_url, this->scope);
    if (newNodes) {
        this->nodes = *newNodes;
        
        for (size_t i = 0; i < this->nodes.getLength(); ++i) {
            this->nodes.getElement(i)->accumulateTransform(0);
        }
    }
    delete newNodes;

    const double timeNow = theSystem->time();
    
    //
    // Initialize Script nodes.
    //
    std::for_each(this->d_scripts.begin(), this->d_scripts.end(),
                  std::bind2nd(std::mem_fun(&ScriptNode::initialize), timeNow));
    
    //
    // Send initial bind events to bindable nodes.
    //
    if (!this->d_backgrounds.empty()) {
        assert(this->d_backgrounds.front());
        this->d_backgrounds.front()
                ->processEvent("set_bind", SFBool(true), timeNow);
    }
    
    if (!this->d_fogs.empty()) {
        assert(this->d_fogs.front());
        this->d_fogs.front()->processEvent("set_bind", SFBool(true), timeNow);
    }
    
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
 * @brief Destructor.
 */
VrmlScene::~VrmlScene() {
    delete d_url;

    delete d_pendingUrl;
    delete d_pendingParameters;
    delete d_pendingNodes;
    delete d_pendingScope;

    delete this->scope;
}

/**
 * @brief Get the browser name.
 *
 * @return "%OpenVRML"
 *
 * Specific browsers may wish to override this method.
 */
const char * VrmlScene::getName() { return "OpenVRML"; }

/**
 * @brief Get the browser version.
 *
 * @return the version of OpenVRML.
 *
 * Specific browsers may wish to override this method.
 */
const char * VrmlScene::getVersion() { return PACKAGE_VERSION; }

/**
 * @brief Initialize the NodeClass map with the available node implementations.
 */
void VrmlScene::initNodeClassMap() {
    this->nodeClassMap["urn:openvrml:node:Script"] =
            NodeClassPtr(new ScriptNodeClass(*this));
    
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
 * @brief Get the root nodes for the scene.
 *
 * @return the root nodes for the scene.
 */
const MFNode & VrmlScene::getRootNodes() const throw () { return this->nodes; }

/**
 * @brief Load a (possibly non-VRML) file...
 */
bool VrmlScene::loadUrl(const MFString & url,
                        const MFString & parameters) {
    
    size_t np = parameters.getLength();
    
    // try each url until we find one we can handle
    size_t i(0);
    for (; i < url.getLength(); ++i) {
        using std::string;
        const string & currentUrl = url.getElement(i);
        if (currentUrl.length() == 0) {
            continue;
        }
        
# if 0
        // #Viewpoint
        if (currentUrl[0] == '#') {
            if (load(url.getElement(i))) {
                break;
            }
        }
# endif
        
        // Load .wrl's, or pass off to system
        else {
            // Check mime type...
            string::size_type tailPos = currentUrl.find_last_of('/');
            if (tailPos == string::npos) {
                tailPos = 0;
            }
            string::size_type modPos = currentUrl.find_first_of('#', tailPos);
            if (modPos == string::npos) {
                modPos = currentUrl.length();
            }
            if (modPos - tailPos > 4
                    && (currentUrl.substr(modPos - 4, 4) == ".wrl"
                        || currentUrl.substr(modPos - 4, 4) == ".wrz"
                        || currentUrl.substr(modPos - 4, 4) == ".WRL"
                        || currentUrl.substr(modPos - 4, 4) == ".WRZ"
                        || (modPos - tailPos > 7 && currentUrl.substr(modPos - 7, 7) == ".wrl.gz")))
            {
# if 0
                if (load(currentUrl)) {
                    break;
                }
# endif
	    } else {
                if (theSystem->loadUrl(currentUrl, parameters)) {
                    break;
                }
            }
        }
    }
    
    return (i != url.getLength());  // true if we found a url that loaded
}

/**
 * @brief Execute scene callback functions.
 *
 * @param reason    the CBReason to pass to the callback functions.
 */
void VrmlScene::doCallbacks(const CBReason reason) {
  SceneCBList::iterator cb, cbend = d_sceneCallbacks.end();
  for (cb = d_sceneCallbacks.begin(); cb != cbend; ++cb)
    (*cb)( reason );

}

/**
 * @brief Add a callback function to be called when the world changes.
 *
 * @param cb    a scene callback function.
 */
void VrmlScene::addWorldChangedCallback(const SceneCB cb) {
    this->d_sceneCallbacks.push_front(cb);
}

/**
 * @brief Read a VRML file from one of the urls.
 */
MFNode * VrmlScene::readWrl(const MFString & urls, Doc2 * relative,
                            VrmlNamespace * ns) {
    Doc2 url;
    const size_t n = urls.getLength();
    for (size_t i = 0; i < n; ++i) {
        //theSystem->debug("Trying to read url '%s'\n", urls->get(i));
        url.seturl(urls.getElement(i).c_str(), relative );
        MFNode * kids = this->readWrl(&url, ns);
        if (kids) {
            return kids;
        } else if ((i < n - 1)
                && strncmp(urls.getElement(i).c_str(), "urn:", 4)) {
            theSystem->warn("Couldn't read url '%s': %s\n",
                            urls.getElement(i).c_str(), strerror(errno));
        }
    }
    
    return 0;
}


/**
 * @brief Read a VRML file and return the (valid) nodes.
 */
MFNode * VrmlScene::readWrl(Doc2 * tryUrl, VrmlNamespace * ns) {
    MFNode * result = 0;
    
    // Should verify MIME type...
    istream & istm = tryUrl->inputStream();
    if (istm) {
        
        Vrml97Scanner scanner(istm);
        Vrml97Parser parser(scanner);
        
        //
        // If the caller is not interested in PROTO defs, use a local namespace.
        //
        // Note: Can we displace the responsibility for this to the caller? Here
        // we instantiate a VrmlNamespace whether or not we need it. That could
        // be avoided with new/delete, but the resulting code would be awkward
        // and hard to follow. Why not instead have the caller pass in a
        // reference to a namespace, and let the caller decide whether or not a
        // new namespace should be instantiated?
        // -- Braden McDaniel <braden@endoframe.com>, 30 Mar, 2000
        //
        VrmlNamespace nodeDefs;
        VrmlNamespace * rootNamespace = ns ? ns : &nodeDefs;
        
        result = new MFNode();
        parser.vrmlScene(*this, *result, *rootNamespace, tryUrl);
    }
    
    return result;
}

/**
 * @brief Read VRML from a string and return the (valid) nodes.
 */
const MFNode VrmlScene::readString(char const * vrmlString, VrmlNamespace * ns)
{
    //
    // Hmm. It looks like passing a zero pointer for the namespace argument of
    // this function is not an option. Perhaps it should be a reference rather
    // than a pointer?
    // -- Braden McDaniel <braden@endoframe.com> 1 Apr, 2000
    //
    
    MFNode result;
    
    if (vrmlString) {
        istrstream istrstm(
#ifdef _WIN32
                           const_cast<char *>(vrmlString)
#else
                           vrmlString
#endif
                           );
        Vrml97Scanner scanner(istrstm);
        Vrml97Parser parser(scanner);
        parser.vrmlScene(*this, result, *ns, 0);
    }
    
    return result;
}

/**
 * @brief Write the current scene to a file.
 *
 * @todo Need to save the PROTOs/EXTERNPROTOs too...
 */
bool VrmlScene::save(const char *url)
{
  bool success = false;
  Doc2 save(url);
  ostream &os = save.outputStream();

  if (os)
    {
      os << "#VRML V2.0 utf8\n";
      os << this->nodes;
      success = true;
    }

  return success;
}

/**
 * @brief Get the URI for the current scene.
 *
 * @return the Doc2 associated with the current scene.
 */
Doc2 * VrmlScene::urlDoc() const { return this->d_url; }

/**
 * @brief Get the root scope for the scene.
 *
 * @return the root scope for the scene.
 */
VrmlNamespace * VrmlScene::getScope() const { return this->scope; }

/**
 * @brief Get the current frame rate.
 *
 * @return the current frame rate.
 */
double VrmlScene::getFrameRate() const { return this->d_frameRate; }

/**
 * @brief Queue an event to load a URL (async so it can be called from a node)
 *
 * @param url           a URI.
 * @param parameters    parameters associated with @a url.
 */
void VrmlScene::queueLoadUrl(const MFString & url,
                             const MFString & parameters) {
    if (!this->d_pendingNodes && !this->d_pendingUrl) {
        this->d_pendingUrl = new MFString(url);
        this->d_pendingParameters = new MFString(parameters);
    }
}

/**
 * @brief Queue an event to replace the node for the world.
 *
 * @param nodes the new nodes.
 * @param ns    the scope associated with @a nodes.
 */
void VrmlScene::queueReplaceNodes(const MFNode & nodes, VrmlNamespace & ns) {
    if (!this->d_pendingNodes && !this->d_pendingUrl) {
        this->d_pendingNodes = new MFNode(nodes);
        d_pendingScope = &ns;
    }
}

/**
 * @brief Queue an event for a node.
 *
 * Current events are in the array @a d_eventMem[d_firstEvent,d_lastEvent). If
 * @a d_firstEvent == @a d_lastEvent, the queue is empty. There is a fixed
 * maximum number of events. If we are so far behind that the queue is filled,
 * the oldest events get overwritten.
 */
void VrmlScene::queueEvent(double timeStamp, FieldValue * value,
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
 * @return @true if there are pending events, @c false otherwise.
 */
bool VrmlScene::eventsPending() {
    return this->d_firstEvent != this->d_lastEvent;
}


/**
 * @brief Discard all pending events
 */
void VrmlScene::flushEvents()
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
void VrmlScene::sensitiveEvent(Node * const n,
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
            } else if (isOver) {
                const std::string & description = a->getDescription();
                const std::string & url = a->getUrl();
                if (!description.empty() && !url.empty()) {
                    theSystem->inform("%s (%s)", description.c_str(),
                                      url.c_str());
                } else if (!description.empty() || !url.empty()) {
                    theSystem->inform("%s", description.length() > 0 ? description.c_str() : url.c_str());
                }
            }
        } else {
            //
            // The parent grouping node is registered for Touch/Drag Sensors.
            //
            Vrml97Node::Group * g = n->toGroup();
            if (g) {
                g->activate(timeStamp, isOver, isActive, point);
                setModified();
            }
        }
    }
}

/**
 * @brief Process events (update the scene).
 *
 * This method should be called after each frame is rendered.
 *
 * @return @c true if the scene needs to be rerendered, @c false otherwise.
 */
bool VrmlScene::update(double currentTime) {
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
                  std::bind2nd(std::mem_fun(&ScriptNode::update), currentTime));

    //
    // Update each of the prototype instances.
    //
    std::for_each(this->protoNodeList.begin(), this->protoNodeList.end(),
                  std::bind2nd(std::mem_fun(&ProtoNode::update), currentTime));
    
    // Pass along events to their destinations
    while (this->d_firstEvent != this->d_lastEvent
            && !this->d_pendingUrl && !this->d_pendingNodes) {
        Event * const e = &this->d_eventMem[this->d_firstEvent];
        this->d_firstEvent = (this->d_firstEvent + 1) % MAXEVENTS;
        
        e->toNode->processEvent(e->toEventIn, *e->value, e->timeStamp);
        
        // this needs to change if event values are shared...
        delete e->value;
    }

# if 0
    if (d_pendingNodes) {
        replaceWorld(*d_pendingNodes, d_pendingScope);
        delete d_pendingNodes;
        d_pendingNodes = 0;
        d_pendingScope = 0;
    } else if (d_pendingUrl) {
        loadUrl(*d_pendingUrl, *d_pendingParameters);
        delete d_pendingUrl;
        delete d_pendingParameters;
        d_pendingUrl = 0;
        d_pendingParameters = 0;
    }
# endif

    // Signal a redisplay if necessary
    return isModified();
}

bool VrmlScene::headlightOn() {
    Vrml97Node::NavigationInfo * const navInfo = bindableNavigationInfoTop();
    if (navInfo) { return navInfo->getHeadlightOn(); }
    return true;
}

/**
 * @brief Draw this scene into the specified viewer
 */
void VrmlScene::render(Viewer * viewer) {
    if (d_newView) {
        viewer->resetUserNavigation();
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

        viewer->insertDirLight(ambient, 1.0, rgb, xyz);
    }                         

    // sets the viewpoint transformation
    //
    viewer->setViewpoint(position, orientation, field, avatarSize, visibilityLimit);
    
    // Set background.
    
    Vrml97Node::Background * bg = bindableBackgroundTop();
    if (bg) {
        // Should be transformed by the accumulated rotations above ...
        bg->renderBindable(viewer);
    } else {
        viewer->insertBackground(); // Default background
    }
    
    // Fog
    Vrml97Node::Fog * f = bindableFogTop();
    if (f) {
        viewer->setFog(f->getColor(), f->getVisibilityRange(), f->getFogType().c_str());
    }
    
    // Top level object
    
    viewer->beginObject(0);
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
        viewer->MatrixMultiply(IM.get());
        vp->getInverseMatrix(MV);
        viewer->getUserNavigation(NMAT);
        MV = MV.multLeft(NMAT);
        MV = MV.multLeft(IM);
    } else {
        // if there's no viewpoint, then set us up arbitrarily at 0,0,-10,
        // as indicated in the vrml spec (section 6.53 Viewpoint).
        //
        float t[3] = { 0.0f, 0.0f, -10.0f };
        VrmlMatrix NMAT;
        MV.setTranslate(t);
        viewer->getUserNavigation(NMAT);
        MV = MV.multLeft(NMAT);
    }     
    
    VrmlRenderContext rc(BVolume::BV_PARTIAL, MV);
    rc.setDrawBSpheres(true);

    // Do the scene-level lights (Points and Spots)
    std::list<Node *>::iterator li, end = this->d_scopedLights.end();
    for (li = this->d_scopedLights.begin(); li != end; ++li) {
        Vrml97Node::AbstractLight * x = (*li)->toLight();
        if (x) { x->renderScoped(viewer); }
    }
    
    // Render the nodes
    for (size_t i = 0; i < this->nodes.getLength(); ++i) {
        this->nodes.getElement(i)->render(viewer, rc);
    }
    
    viewer->endObject();
    
    // This is actually one frame late...
    d_frameRate = viewer->getFrameRate();
    
    clearModified();

    // If any events were generated during render (ugly...) do an update
    if (eventsPending()) { setDelta(0.0); }
}

/**
 * @brief Indicate that the scene state has changed; need to rerender.
 */
void VrmlScene::setModified() { this->d_modified = true; }

/**
 * @brief Clear the "modified" flag for the scene.
 */
void VrmlScene::clearModified() { this->d_modified = false; }

/**
 * @brief Check if the scene has been modified.
 *
 * @return @c true if the scene has been modified, @c false otherwise.
 */
bool VrmlScene::isModified() const { return this->d_modified; }

/**
 * @brief Set the time until the next update is needed.
 *
 * @param d a time interval.
 */
void VrmlScene::setDelta(const double d) {
    if (d < this->d_deltaTime) { this->d_deltaTime = d; }
}

/**
 * @brief Get the time interval between scene updates.
 *
 * @return the time interval between scene updates.
 */
double VrmlScene::getDelta() const { return this->d_deltaTime; }

/**
 * @brief Get the top node of a BindStack.
 *
 * @return the top node of @p stack.
 */
const NodePtr VrmlScene::bindableTop(const BindStack & stack) {
    return stack.empty() ? NodePtr(0) : stack.front();
}

/**
 * @brief Push a node onto a BindStack.
 *
 * @param stack the BindStack onto which to push @p node.
 * @param node  the Node to push onto @p stack.
 */
void VrmlScene::bindablePush(BindStack & stack, const NodePtr & node) {
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
void VrmlScene::bindableRemove(BindStack & stack, const NodePtr & node) {
    const BindStack::iterator pos = std::find(stack.begin(), stack.end(), node);
    if (pos != stack.end()) {
        stack.erase(pos);
        this->setModified();
    }
}

/**
 * @brief Add a Background node to the list of Background nodes for the scene.
 *
 * @param node  a Background node.
 */
void VrmlScene::addBackground(Vrml97Node::Background & node) {
    this->d_backgrounds.push_back(&node);
}

/**
 * @brief Remove a Background node from the list of Background nodes for the
 *      scene.
 *
 * @param node  a Background node.
 *
 * This method has no effect if @p node is not in the list of Background nodes
 * for the scene.
 */
void VrmlScene::removeBackground(Vrml97Node::Background & node) {
    const std::list<Node *>::iterator end = this->d_backgrounds.end();
    const std::list<Node *>::iterator pos =
            std::find(this->d_backgrounds.begin(), end, &node);
    if (pos != end) { this->d_backgrounds.erase(pos); }
}

/**
 * @brief Get the active node on the bound Background stack.
 *
 * @return the active node on the bound Background stack.
 */
Vrml97Node::Background * VrmlScene::bindableBackgroundTop() {
    Node * const b = bindableTop(d_backgroundStack).get();
    return b ? b->toBackground() : 0;
}

/**
 * @brief Push a Background node onto the bound Background node stack.
 *
 * @param n a Background node.
 */
void VrmlScene::bindablePush(Vrml97Node::Background * n) {
    bindablePush(d_backgroundStack, NodePtr(n));
}

/**
 * @brief Remove a Background node from the bound Background node stack.
 *
 * @param n a Background node.
 */
void VrmlScene::bindableRemove(Vrml97Node::Background * n) {
    bindableRemove(d_backgroundStack, NodePtr(n));
}

/**
 * @brief Add a Fog node to the list of Fog nodes for the scene.
 *
 * @param n a Fog node.
 */
void VrmlScene::addFog(Vrml97Node::Fog & n) { this->d_fogs.push_back(&n); }

/**
 * @brief Remove a Fog node from the list of Fog nodes for the scene.
 *
 * @param node  a Fog node.
 *
 * This method has no effect if @p node is not in the list of Fog nodes for
 * the scene.
 */
void VrmlScene::removeFog(Vrml97Node::Fog & node) {
    const std::list<Node *>::iterator end = this->d_fogs.end();
    const std::list<Node *>::iterator pos =
            std::find(this->d_fogs.begin(), end, &node);
    if (pos != end) { this->d_fogs.erase(pos); }
}

/**
 * @brief Get the active node on the bound Fog stack.
 *
 * @return the active node on the bound Fog stack.
 */
Vrml97Node::Fog * VrmlScene::bindableFogTop() {
    Node * const f = bindableTop(d_fogStack).get();
    return f ? f->toFog() : 0;
}

/**
 * @brief Push a Fog node onto the bound Fog node stack.
 *
 * @param n a Fog node.
 */
void VrmlScene::bindablePush(Vrml97Node::Fog * n) {
    bindablePush(d_fogStack, NodePtr(n));
}

/**
 * @brief Remove a Fog node from the bound Fog node stack.
 *
 * @param n a Fog node.
 */
void VrmlScene::bindableRemove(Vrml97Node::Fog * n) {
    bindableRemove(d_fogStack, NodePtr(n));
}

/**
 * @brief Add a NavigationInfo node to the list of NavigationInfo nodes for the
 *      scene.
 *
 * @param n a NavigationInfo node.
 */
void VrmlScene::addNavigationInfo(Vrml97Node::NavigationInfo & n) {
    this->d_navigationInfos.push_back(&n);
}

/**
 * @brief Remove a NavigationInfo node from the list of NavigationInfo nodes
 *      for the scene.
 *
 * @param node  a NavigationInfo node.
 *
 * This method has no effect if @p node is not in the list of NavigationInfo
 * nodes for the scene.
 */
void VrmlScene::removeNavigationInfo(Vrml97Node::NavigationInfo & node) {
    const std::list<Node *>::iterator end = this->d_navigationInfos.end();
    const std::list<Node *>::iterator pos =
            std::find(this->d_navigationInfos.begin(), end, &node);
    if (pos != end) { this->d_navigationInfos.erase(pos); }
}

/**
 * @brief Get the active node on the bound NavigationInfo stack.
 *
 * @return the active node on the bound NavigationInfo stack.
 */
Vrml97Node::NavigationInfo * VrmlScene::bindableNavigationInfoTop() {
    Node * const n = bindableTop(d_navigationInfoStack).get();
    return n ? n->toNavigationInfo() : 0;
}

/**
 * @brief Push a NavigationInfo node onto the bound NavigationInfo node stack.
 *
 * @param n a NavigationInfo node.
 */
void VrmlScene::bindablePush(Vrml97Node::NavigationInfo * n) {
    bindablePush(d_navigationInfoStack, NodePtr(n));
}

/**
 * @brief Remove a NavigationInfo node from the bound NavigationInfo node stack.
 *
 * @param n a NavigationInfo node.
 */
void VrmlScene::bindableRemove(Vrml97Node::NavigationInfo * n) {
    bindableRemove(d_navigationInfoStack, NodePtr(n));
}

/**
 * @brief Add a Viewpoint node to the list of Viewpoint nodes for the scene.
 *
 * @param n a Viewpoint node.
 */
void VrmlScene::addViewpoint(Vrml97Node::Viewpoint & n) {
    this->d_viewpoints.push_back(&n);
}

/**
 * @brief Remove a Viewpoint node from the list of Viewpoint nodes for the
 *      scene.
 *
 * @param node  a Viewpoint node.
 *
 * This method has no effect if @p node is not in the list of Viewpoint
 * nodes for the scene.
 */
void VrmlScene::removeViewpoint(Vrml97Node::Viewpoint & node) {
    const std::list<Node *>::iterator end = this->d_viewpoints.end();
    const std::list<Node *>::iterator pos =
            std::find(this->d_viewpoints.begin(), end, &node);
    if (pos != end) { this->d_viewpoints.erase(pos); }
}

/**
 * @brief Get the active node on the bound Viewpoint stack.
 *
 * @return the active node on the bound Viewpoint stack.
 */
Vrml97Node::Viewpoint * VrmlScene::bindableViewpointTop() {
    Node * const t = bindableTop(d_viewpointStack).get();
    return t ? t->toViewpoint() : 0;
}

/**
 * @brief Push a Viewpoint node onto the bound Viewpoint node stack.
 *
 * @param n a Viewpoint node.
 */
void VrmlScene::bindablePush(Vrml97Node::Viewpoint * n) {
    bindablePush(d_viewpointStack, NodePtr(n));
    d_newView = true;
}

/**
 * @brief Remove a Viewpoint node from the bound Viewpoint node stack.
 *
 * @param n a Viewpoint node.
 */
void VrmlScene::bindableRemove(Vrml97Node::Viewpoint * n) {
    bindableRemove(d_viewpointStack, NodePtr(n));
    d_newView = true;
}

/**
 * @brief Bind to the next Viewpoint in the list.
 */
void VrmlScene::nextViewpoint() {
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
void VrmlScene::prevViewpoint() {
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
 * @brief Get the number of Viewpoint nodes in the scene.
 *
 * @return the number of Viewpoint nodes in the scene.
 */
size_t VrmlScene::nViewpoints() { return d_viewpoints.size(); }

/**
 * @brief Get the name and description of a Viewpoint.
 *
 * @param nvp           the index of the Viewpoint.
 * @retval name         the name of the Viewpoint
 * @retval description  the description of the Viewpoint.
 */
void VrmlScene::getViewpoint(const size_t nvp, std::string & name,
                             std::string & description) {
    std::list<Node *>::const_iterator i = this->d_viewpoints.begin();
    size_t n = 0;
    for (; i != this->d_viewpoints.end(); ++i, ++n ) {
        if (n == nvp) {
            name = (*i)->getId();
            description = (*i)->toViewpoint()->getDescription().get();
            return;
        }
    }
}

/**
 * @brief Bind to a specific Viewpoint.
 *
 * @param name          the name of the Viewpoint to bind to.
 * @param description   the description of the Viewpoint to bind to.
 *
 * This method will only bind to a Viewpoint if both @p name and @p description
 * match the name and description of the Viewpoint. If no such Viewpoint is
 * available, this method has no effect.
 */
void VrmlScene::setViewpoint(const std::string & name,
                             const std::string & description) {
    std::list<Node *>::iterator i = this->d_viewpoints.begin();
    for (; i != this->d_viewpoints.end(); ++i) {
        if (name == (*i)->getId()
                && description == (*i)->toViewpoint()->getDescription().get()) {
            Vrml97Node::Viewpoint * const vp = (*i)->toViewpoint();
            if (vp) {
                vp->processEvent("set_bind", SFBool(true), theSystem->time());
            }
            return;
        }
    }
}

/**
 * @brief Bind to a Viewpoint with a particular index.
 *
 * @param nvp   the index of the Viewpoint to bind to.
 *
 * If @p nvp is not a valid index, this method has no effect.
 */
void VrmlScene::setViewpoint(size_t nvp) {
    std::list<Node *>::iterator i;
    int j = 0;
    
    for (i = d_viewpoints.begin(); i != d_viewpoints.end(); ++i) {
        if (j == nvp) {
            Vrml97Node::Viewpoint * const vp = (*i)->toViewpoint();
            if (vp) {
                vp->processEvent("set_bind", SFBool(true), theSystem->time());
            }
	    return;
        }
        ++j;
    }
}

/**
 * @brief Add a scoped light node to the scene.
 *
 * @param light a light node.
 */
void VrmlScene::addScopedLight(Vrml97Node::AbstractLight & light) {
    this->d_scopedLights.push_back(&light);
}

/**
 * @brief Remove a scoped light node from the scene.
 *
 * @param light the light node to remove.
 *
 * This method has no effect if @p light is not in the list of light nodes for
 * the scene.
 */
void VrmlScene::removeScopedLight(Vrml97Node::AbstractLight & light) {
    this->d_scopedLights.remove(&light);
}

/**
 * @brief Add a MovieTexture node to the scene.
 *
 * @param movie a MovieTexture node.
 */
void VrmlScene::addMovie(Vrml97Node::MovieTexture & movie) {
    this->d_movies.push_back(&movie);
}

/**
 * @brief Remove a MovieTexture node from the scene.
 *
 * @param movie the MovieTexture node to remove.
 *
 * This method has no effect if @p movie is not in the list of MovieTexture
 * nodes for the scene.
 */
void VrmlScene::removeMovie(Vrml97Node::MovieTexture & movie) {
    this->d_movies.remove(&movie);
}

/**
 * @brief Add a Script node to the scene.
 *
 * @param script    a Script node.
 */
void VrmlScene::addScript(ScriptNode & script) {
    this->d_scripts.push_back(&script);
}

/**
 * @brief Remove a Script node from the scene.
 *
 * @param script    the Script node to remove.
 *
 * This method has no effect if @p script is not in the list of Script nodes
 * for the scene.
 */
void VrmlScene::removeScript(ScriptNode & script) {
    this->d_scripts.remove(&script);
}

/**
 * @brief Add a PROTO instance to the scene.
 *
 * @param node  a PROTO instance.
 */
void VrmlScene::addProto(ProtoNode & node) {
    this->protoNodeList.push_back(&node);
}

/**
 * @brief Remove a PROTO instance from the scene.
 *
 * @param node  the PROTO instance to remove.
 *
 * This method has no effect if @p node is not in the list of PROTO instances
 * for the scene.
 */
void VrmlScene::removeProto(ProtoNode & node) {
    this->protoNodeList.remove(&node);
}

/**
 * @brief Add a TimeSensor node to the scene.
 *
 * @param timer a TimeSensor node.
 */
void VrmlScene::addTimeSensor(Vrml97Node::TimeSensor & timer) {
    this->d_timers.push_back(&timer);
}

/**
 * @brief Remove a TimeSensor node from the scene.
 *
 * @param timer the TimeSensor node to remove.
 *
 * This method has no effect if @p timer is not in the list of TimeSensor nodes
 * for the scene.
 */
void VrmlScene::removeTimeSensor(Vrml97Node::TimeSensor & timer) {
    this->d_timers.remove(&timer);
}


/**
 * @brief Add an AudioClip node to the scene.
 *
 * @param audio_clip    an AudioClip node.
 */
void VrmlScene::addAudioClip(Vrml97Node::AudioClip & audio_clip) {
    this->d_audioClips.push_back(&audio_clip);
}

/**
 * @brief Remove an AudioClip node from the scene.
 *
 * @param audio_clip    the AudioClip node to remove.
 *
 * This method has no effect if @p audio_clip is not in the list of AudioClip
 * nodes for the scene.
 */
void VrmlScene::removeAudioClip(Vrml97Node::AudioClip & audio_clip) {
    this->d_audioClips.remove(&audio_clip);
}


/**
 * @brief Propagate the bvolume dirty flag from children to ancestors.
 *
 * The invariant is that if a node's bounding volume is out of date,
 * then the bounding volumes of all that nodes's ancestors must be
 * out of date. However, Node does not maintain a parent pointer. So
 * we must do a traversal of the entire scene graph to do the propagation.
 *
 * @see Node::setBVolumeDirty
 * @see Node::isBVolumeDirty
 */
void VrmlScene::updateFlags()
{
//  Node* root = this->getRoot();
//  root->updateModified(0x002);
}


/**
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
 * @class ProtoNode::NodeCloneVisitor
 *
 * @brief A NodeVisitor that makes a deep copy of the node tree for creating
 *      a new prototype node instance.
 */

/**
 * @var std::stack<NodePtr> ProtoNode::NodeCloneVisitor::rootNodeStack
 *
 * @brief A stack for node clones.
 *
 * As the NodeVisitor traverses the node tree, it pushes the newly-created
 * cloned node onto the stack. The node at the top of the stack is then the
 * result of cloning any given subtree in the source node tree.
 */

/**
 * @var const ProtoNode & ProtoNode::NodeCloneVisitor::fromProtoNode
 *
 * @brief A reference to the "source" node.
 */

/**
 * @var const ProtoNode & ProtoNode::NodeCloneVisitor::toProtoNode
 *
 * @brief A reference to the "destination" node.
 */

/**
 * @brief Constructor.
 *
 * @param fromNode  a reference to the "source" node.
 * @param toNode    a reference to the "destination" node.
 */
ProtoNode::NodeCloneVisitor::NodeCloneVisitor(const ProtoNode & fromProtoNode,
                                              ProtoNode & toProtoNode):
        fromProtoNode(fromProtoNode), toProtoNode(toProtoNode) {}

/**
 * @brief Destructor.
 */
ProtoNode::NodeCloneVisitor::~NodeCloneVisitor() {}

/**
 * @brief Clone the nodes.
 */
void ProtoNode::NodeCloneVisitor::clone() {
    assert(this->fromProtoNode.implNodes.getElement(0));
    this->toProtoNode.implNodes.setLength(this->fromProtoNode.implNodes.getLength());
    for (size_t i = 0; i < this->toProtoNode.implNodes.getLength(); ++i) {
        if (this->fromProtoNode.implNodes.getElement(i)) {
            Node & childNode(*this->fromProtoNode.implNodes.getElement(i));
            if (!childNode.accept(*this)) {
                assert(this->toProtoNode.scope.findNode(childNode.getId()));
                this->rootNodeStack
                        .push(NodePtr(this->toProtoNode.scope.findNode(childNode.getId())));
            }
            assert(this->rootNodeStack.top());
            this->toProtoNode.implNodes.setElement(i, this->rootNodeStack.top());
            this->rootNodeStack.pop();
        }
    }
    assert(this->rootNodeStack.size() == 0);

    for (size_t i = 0; i < this->fromProtoNode.implNodes.getLength(); ++i) {
        assert(this->fromProtoNode.implNodes.getElement(i));
        this->fromProtoNode.implNodes.getElement(i)->resetVisitedFlag();
    }
}

namespace {

    struct CloneFieldValue_ : std::unary_function<NodeInterface, void> {
        CloneFieldValue_(NodeVisitor & visitor,
                         std::stack<NodePtr> & rootNodeStack,
                         VrmlNamespace & scope, Node & fromNode, Node & toNode):
                visitor(&visitor), rootNodeStack(&rootNodeStack), scope(&scope),
                fromNode(&fromNode), toNode(&toNode) {}
    
        void operator()(const NodeInterface & interface) const {
            if (interface.type == NodeInterface::field
                    || interface.type == NodeInterface::exposedField) {
                if (interface.fieldType == FieldValue::sfnode) {
                    const SFNode & value =
                            static_cast<const SFNode &>
                                (this->fromNode->getField(interface.id));
                    if (value.get()) {
                        Node & childNode(*value.get());
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
                            Node & childNode(*children.getElement(i));
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
        VrmlNamespace * scope;
        Node * fromNode;
        Node * toNode;
    };
}

/**
 * @brief Visit a node.
 *
 * @param node  a Node.
 */
void ProtoNode::NodeCloneVisitor::visit(Node & node) {
    //
    // Create a new node of the same type.
    //
    const NodePtr newNode(node.nodeType.createNode());
    this->rootNodeStack.push(newNode);

    //
    // If the node has a name, give it to the new node.
    //
    if (!node.getId().empty()) {
        newNode->setId(node.getId(), &this->toProtoNode.scope);
    }

    //
    // Any IS mappings for this node?
    //
    for (ISMap::const_iterator itr(this->fromProtoNode.isMap.begin());
            itr != this->fromProtoNode.isMap.end(); ++itr) {
        if (&itr->second.node == &node) {
            this->toProtoNode.addIS(*newNode, itr->second.interfaceId,
                                    itr->first);
        }
    }

    //
    // Copy the field values.
    //
    const NodeInterfaceSet & interfaces(node.nodeType.getInterfaces());
    std::for_each(interfaces.begin(), interfaces.end(),
                  CloneFieldValue_(*this, this->rootNodeStack,
                                   this->toProtoNode.scope, node, *newNode));
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
                                              ProtoNode & toProtoNode):
        fromProtoNode(fromProtoNode), toProtoNode(toProtoNode) {}

/**
 * @brief Copy the ROUTEs.
 */
void ProtoNode::RouteCopyVisitor::copyRoutes() {
    for (size_t i = 0; i < this->fromProtoNode.implNodes.getLength(); ++i) {
        if (this->fromProtoNode.implNodes.getElement(i)) {
            this->fromProtoNode.implNodes.getElement(i)->accept(*this);
        }
    }

    for (size_t i = 0; i < this->fromProtoNode.implNodes.getLength(); ++i) {
        this->fromProtoNode.implNodes.getElement(i)->resetVisitedFlag();
    }
}

namespace {
    
    struct AddRoute_ : std::unary_function<Node::Route, void> {
        AddRoute_(VrmlNamespace & scope, Node & fromNode):
                  scope(&scope), fromNode(&fromNode) {}

        void operator()(const Node::Route & route) const {
            const std::string & toNodeId(route.toNode->getId());
            assert(this->scope->findNode(toNodeId));
            fromNode->addRoute(route.fromEventOut,
                               NodePtr(this->scope->findNode(toNodeId)),
                               route.toEventIn);
        }

    private:
        VrmlNamespace * scope;
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
        Node * const fromNode = this->toProtoNode.scope.findNode(fromNodeId);
        assert(fromNode);
        std::for_each(node.getRoutes().begin(), node.getRoutes().end(),
                      AddRoute_(this->toProtoNode.scope, *fromNode));
    }

    //
    // Visit the children.
    //
    const MFNode & children(node.getChildren());
    for (size_t i = 0; i < children.getLength(); ++i) {
        if (children.getElement(i)) {
            children.getElement(i)->accept(*this);
        }
    }
}

/**
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
 * @param nodeType      the NodeType associated with the node.
 * @param parentScope   the parent scope.
 */
ProtoNode::ProtoNode(const NodeType & nodeType, VrmlNamespace & parentScope):
        Node(nodeType), scope(&parentScope) {
    //
    // For each exposedField and eventOut in the prototype interface, add
    // a value to the eventOutValueMap.
    //
//    const NodeInterfaceSet & interfaces(this->nodeType.getInterfaces());
//    std::for_each(interfaces.begin(), interfaces.end(),
//                  AddEventOutValue_(this->eventOutValueMap));
}

/**
 * @brief Construct a prototype instance.
 *
 * @param nodeType  the type object for the new ProtoNode instance.
 * @param node      the ProtoNode to clone when creating the instance.
 */
ProtoNode::ProtoNode(const NodeType & nodeType, const ProtoNode & node):
        Node(nodeType), scope(node.scope.parent) {
    assert(node.implNodes.getLength() > 0);
    assert(node.implNodes.getElement(0));
    
    //
    // For each exposedField and eventOut in the prototype interface, add
    // a value to the eventOutValueMap.
    //
    // Note: We don't want to copy node's EventOutValueMap, since the values
    // in that map should be per-instance.
    //
    const NodeInterfaceSet & interfaces(this->nodeType.getInterfaces());
    std::for_each(interfaces.begin(), interfaces.end(),
                  AddEventOutValue_(this->eventOutValueMap));
    
    //
    // Cloning the nodes is a two-step process. First, we clone the actual
    // node instances. Second, we traverse the node tree again cloning the
    // routes.
    //
    NodeCloneVisitor(node, *this).clone();
    RouteCopyVisitor(node, *this).copyRoutes();
    
    //
    // Add to the scene.
    //
    this->nodeType.nodeClass.scene.addProto(*this);
}

/**
 * @brief Destructor.
 */
ProtoNode::~ProtoNode() throw () {
    //
    // Remove from the scene.
    //
    this->nodeType.nodeClass.scene.removeProto(*this);
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
 * @exception std::invalid_argument
 */
void ProtoNode::addIS(Node & implNode,
                      const std::string & implNodeInterfaceId,
                      const std::string & protoInterfaceId)
        throw (std::invalid_argument, std::bad_alloc) {
    const ImplNodeInterface implNodeInterface(implNode, implNodeInterfaceId);
    const ISMap::value_type value(protoInterfaceId, implNodeInterface);
    this->isMap.insert(value);
    
    if (this->nodeType.hasEventOut(protoInterfaceId)) {
        EventOutValueMap::iterator pos =
                this->eventOutValueMap.find(protoInterfaceId);
        if (pos == this->eventOutValueMap.end()) {
            pos = this->eventOutValueMap.find(protoInterfaceId + "_changed");
        }
        assert(pos != this->eventOutValueMap.end());
        implNode.addEventOutIS(implNodeInterfaceId, &pos->second);
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

void ProtoNode::setFieldImpl(const std::string & id,
                             const FieldValue & value)
        throw (UnsupportedInterface, std::bad_cast, std::bad_alloc) {
    const std::pair<ISMap::iterator, ISMap::iterator> rangeItrs =
            this->isMap.equal_range(id);
    for (ISMap::iterator itr(rangeItrs.first); itr != rangeItrs.second; ++itr) {
        itr->second.node.setField(itr->second.interfaceId, value);
    }
}

const FieldValue & ProtoNode::getFieldImpl(const std::string & id) const
        throw (UnsupportedInterface) {
    //
    // This is a little wierd: what should getField mean for a PROTO-based
    // node? Here, we just pick the first node in the IS-map and call getField
    // on it.
    //
    ISMap::const_iterator pos = this->isMap.find(id);
    if (pos == this->isMap.end()) {
        throw UnsupportedInterface(this->nodeType.id + " node has no field \""
                                   + id + "\".");
    }
    return pos->second.node.getField(pos->second.interfaceId);
}

namespace {
    
    struct DispatchEvent_ :
            std::unary_function<ProtoNode::ISMap::value_type, void> {
        DispatchEvent_(const FieldValue & value, const double timestamp):
                value(&value), timestamp(timestamp) {}

        void operator()(const ProtoNode::ISMap::value_type & value) const {
            value.second.node.processEvent(value.second.interfaceId,
                                           *this->value, this->timestamp);
        }

    private:
        const FieldValue * value;
        double timestamp;
    };
}

void ProtoNode::processEventImpl(const std::string & id,
                                 const FieldValue & value,
                                 const double timestamp)
        throw (UnsupportedInterface, std::bad_cast, std::bad_alloc) {
    const std::pair<ISMap::const_iterator, ISMap::const_iterator> rangeItrs =
            this->isMap.equal_range(id);
    if (rangeItrs.first == this->isMap.end()) {
        throw UnsupportedInterface(this->nodeType.id + " node has no eventIn "
                                   + id);
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

const FieldValue & ProtoNode::getEventOutImpl(const std::string & id) const
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
        if (pos != this->isMap.end()) {
            return pos->second.node.getEventOut(id);
        }
    }
}

Vrml97Node::Anchor * ProtoNode::toAnchor() const {
    return this->implNodes.getElement(0)->toAnchor();
}

Vrml97Node::AudioClip * ProtoNode::toAudioClip() const {
    return this->implNodes.getElement(0)->toAudioClip();
}

Vrml97Node::Background * ProtoNode::toBackground() const {
    return this->implNodes.getElement(0)->toBackground();
}

Vrml97Node::CylinderSensor * ProtoNode::toCylinderSensor() const {
    return this->implNodes.getElement(0)->toCylinderSensor();
}

Vrml97Node::Fog * ProtoNode::toFog() const {
    return this->implNodes.getElement(0)->toFog();
}

Vrml97Node::Group * ProtoNode::toGroup() const {
    return this->implNodes.getElement(0)->toGroup();
}

Vrml97Node::Inline * ProtoNode::toInline() const {
    return this->implNodes.getElement(0)->toInline();
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

void ProtoNode::render(Viewer * const viewer, const VrmlRenderContext rc) {
    assert(this->implNodes.getElement(0));
    this->implNodes.getElement(0)->render(viewer, rc);
}


/**
 * @class ProtoNodeClass
 *
 * @brief Class object for ProtoNode instances.
 *
 * ProtoNodeClass instances are created and initialized when a PROTO definition
 * is read by the parser. Initialization consists of various calls to
 * addEventIn, addEventOut, addExposedField, addField, addRootNode, and addIS,
 * after which the ProtoNodeClass instance is added to the implementation
 * repository. Once the scene has started running, it is not appropriate to
 * call those methods.
 */

/**
 * @class ProtoNodeClass::ProtoNodeType
 *
 * @brief Type information object for ProtoNode instances.
 */

/**
 * @var ProtoNodeClass:ProtoNodeType::nodeInterfaces
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
const NodePtr ProtoNodeClass::ProtoNodeType::createNode() const
        throw (std::bad_alloc) {
    return NodePtr(new ProtoNode(*this,
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
 * @param scene the VrmlScene associated with this NodeClass.
 */
ProtoNodeClass::ProtoNodeClass(VrmlScene & scene) throw ():
        NodeClass(scene), protoNodeType(*this, ""),
        protoNode(protoNodeType, *scene.scope) {}

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
 * @exception std::invalid_argument
 */
void ProtoNodeClass::addIS(Node & implNode,
                           const std::string & implNodeInterfaceId,
                           const std::string & protoInterfaceId)
        throw (std::invalid_argument, std::bad_alloc) {
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

} // namespace OpenVRML
