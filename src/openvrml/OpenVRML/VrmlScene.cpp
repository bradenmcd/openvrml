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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if defined(_WIN32) && !defined(__CYGWIN__)
#include <winconfig.h>
#endif

#include <algorithm>
#include <errno.h>
#include <stdio.h>
#if defined(_WIN32) && !defined(__CYGWIN__)
#include <strstrea.h>
#else
#include <strstream.h>
#endif
#include "VrmlScene.h"
#include "doc2.hpp"
#include "Viewer.h"
#include "System.h"
#include "MathUtils.h"
#include "VrmlNamespace.h"
#include "VrmlRenderContext.h"
#include "script.h"

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
 * @class VrmlScene
 *
 * @brief Encapsulates a VRML scene.
 */

/**
 * Create a VrmlScene from a URL.
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

VrmlScene::~VrmlScene() {
    delete d_url;

    delete d_pendingUrl;
    delete d_pendingParameters;
    delete d_pendingNodes;
    delete d_pendingScope;

    delete this->scope;
}

const MFNode & VrmlScene::getRootNodes() const throw () {
    return this->nodes;
}

/**
 * Load a (possibly non-VRML) file...
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

# if 0
/**
 * Called by viewer when a destroy request is received. The request
 * is just passed on to the client via the worldChanged CB.
 */
void VrmlScene::destroyWorld() {
    doCallbacks( DESTROY_WORLD );
}

/**
 * Replace nodes
 */
void VrmlScene::replaceWorld(MFNode & nodes, VrmlNamespace * ns, Doc2 * url) {
    delete this->scope;
    delete d_url;
    
    this->scope = ns;
    d_url = url;
    
    // Clear bindable stacks.
    this->d_backgroundStack.clear();
    this->d_fogStack.clear(); 
    this->d_navigationInfoStack.clear(); 
    this->d_viewpointStack.clear();
    
    // Get rid of current world: pending events, nodes.
    flushEvents();
    
    this->nodes = nodes;

  
    for (size_t i = 0; i < this->nodes.getLength(); ++i) {
        nodes.getElement(i)->accumulateTransform(0);
    }
    
    // Send initial set_binds to bindable nodes
    double timeNow = theSystem->time();
    SFBool flag(true);
    
    if (!this->d_backgrounds.empty()) {
        assert(this->d_backgrounds.front());
        this->d_backgrounds.front()->processEvent("set_bind", flag, timeNow);
    }
    
    if (!this->d_fogs.empty()) {
        assert(this->d_fogs.front());
        this->d_fogs.front()->processEvent("set_bind", flag, timeNow);
    }
    
    if (!this->d_navigationInfos.empty()) {
        assert(this->d_navigationInfos.front());
        this->d_navigationInfos.front()
                ->processEvent("set_bind", flag, timeNow);
    }
    
    if (!this->d_viewpoints.empty()) {
        assert(this->d_viewpoints.front());
        this->d_viewpoints.front()->processEvent("set_bind", flag, timeNow);
    }
    
    // Notify anyone interested that the world has changed
    doCallbacks( REPLACE_WORLD );
    
    setModified();
    d_newView = true;		// Force resetUserNav
}
# endif

void VrmlScene::doCallbacks( int reason )
{
  SceneCBList::iterator cb, cbend = d_sceneCallbacks.end();
  for (cb = d_sceneCallbacks.begin(); cb != cbend; ++cb)
    (*cb)( reason );

}

void VrmlScene::addWorldChangedCallback( SceneCB cb )
{
  d_sceneCallbacks.push_front( cb );
}

# if 0
/**
 * Read a VRML97 file.
 * This is only for [*.wrl][#viewpoint] url loading (no parameters).
 */
bool VrmlScene::load(const std::string & url) {
    // Look for '#Viewpoint' syntax. There ought to be a current
    // scene if this format is used.
    SFBool flag(true);
    if (url[0] == '#') {
        Node * const vp = this->scope
                              ? this->scope->findNode(url.substr(1))
                              : 0;
        
        // spec: ignore if named viewpoint not found
        if (vp) {
	    vp->processEvent("set_bind", flag, theSystem->time());
	    setModified();
	}
        
        return true;
    }
    
    Doc2 * tryUrl = new Doc2(url, this->d_url);
    
    MFNode * newNodes = this->readWrl(tryUrl, this->scope);
    
    if (newNodes) {
        Doc2 * sourceUrl = tryUrl;
        
        this->replaceWorld(*newNodes, this->scope, sourceUrl);
        delete newNodes;
        
        // Look for '#Viewpoint' syntax
        if (sourceUrl->urlModifier()) {
            Node * const vp =
                    this->scope->findNode(sourceUrl->urlModifier() + 1);
            double timeNow = theSystem->time();
            if (vp) {
                vp->processEvent("set_bind", flag, timeNow);
            }
	}
        
        return true;      // Success.
    }
    
    delete tryUrl;
    return false;
}
# endif

/**
 * Read a VRML file from one of the urls.
 */
MFNode * VrmlScene::readWrl(const MFString & urls, Doc2 * relative,
                                VrmlNamespace * ns)
{
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
 * Read a VRML file and return the (valid) nodes.
 */
MFNode * VrmlScene::readWrl(Doc2 * tryUrl, VrmlNamespace * ns)
{
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
//        try {
            parser.vrmlScene(*this, *result, *rootNamespace, tryUrl);
//        } catch (std::exception & ex) {
//            cerr << ex.what() << endl;
//        }
    }
    
    return result;
}

# if 0
bool VrmlScene::loadFromString( const char *vrmlString )
{
    VrmlNamespace * newScope = new VrmlNamespace();
    MFNode newNodes = readString(vrmlString, newScope);
    if (newNodes.getLength() > 0) {
        replaceWorld(newNodes, newScope, 0);
        return true;
    }
    return false;
}
# endif

/**
 * Read VRML from a string and return the (valid) nodes.
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


# if 0
// Load VRML from an application-provided callback function

bool VrmlScene::loadFromFunction(LoadCB cb, char const * url)
{
    Doc2 * doc = url ? new Doc2(url, 0) : 0;
    VrmlNamespace * ns = new VrmlNamespace();
    MFNode * newNodes = readFunction(cb, doc, ns);
    
    if (newNodes) {
        replaceWorld( *newNodes, ns, doc, 0 );
        delete newNodes;
        return true;
    }
    
    if (doc) {
        delete doc;
    }
    
    return false;
}

// Read VRML from a cb and return the (valid) nodes.

MFNode * VrmlScene::readFunction(LoadCB cb, Doc2 * url, VrmlNamespace * ns)
{
    MFNode * result = 0;
    
    if (cb != 0) {
        yyNodeTypes = ns;
        yyParsedNodes = 0;
        yyDocument = url;
        
        // set input to be from cb
        yyfunction(cb);
        
        yyparse();
        
        yyDocument = 0;
        yyNodeTypes = 0;
        result = yyParsedNodes;
        yyParsedNodes = 0;
    }
    
    return result;
}


// Read a PROTO from a URL to get the implementation of an EXTERNPROTO.
// This should read only PROTOs and return when the first/specified PROTO
// is read...

const NodeTypePtr VrmlScene::readPROTO(const MFString & urls,
                                       const Doc2 * relative) {
    // This is a problem. The nodeType of the EXTERNPROTO has a namespace
    // that refers back to this namespace (protos), which will be invalid
    // after we exit this function. I guess it needs to be allocated and
    // ref counted too...
    //VrmlNamespace protos;
    VrmlNamespace * protos = new VrmlNamespace();  // leak...
    Doc2 urlDoc;
    NodeTypePtr def(0);
    
    for (size_t i(0); i < urls.getLength(); ++i) {
        theSystem->inform("Trying to read EXTERNPROTO from url '%s'\n",
                          urls.getElement(i).c_str());
        urlDoc.seturl(urls.getElement(i).c_str(), relative);
        MFNode * kids = VrmlScene::readWrl(&urlDoc, protos);
        delete kids;
        
        // Grab the specified PROTO, or the first one.
        const char * whichProto = urlDoc.urlModifier();
        if (whichProto && *whichProto) {
            def = protos->findType( whichProto+1 );
        } else {
            def = protos->firstType();
        }
        
        if (def) {
            def->setActualUrl( urlDoc.url() );
            break;
	} else if ((i < (urls.getLength() - 1))
                && strncmp(urls.getElement(i).c_str(), "urn:", 4)) {
            theSystem->warn("Couldn't read EXTERNPROTO url '%s': %s\n",
                            urls.getElement(i).c_str(), strerror(errno));
        }
    }
    
    return def;
}
# endif

/**
 * Write the current scene to a file.
 * Need to save the PROTOs/EXTERNPROTOs too...
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


//
// Script node API functions
//
const char *VrmlScene::getName() { return "OpenVRML"; }

const char *VrmlScene::getVersion() {
  static char vs[32];
  sprintf(vs, "%d.%d.%d", OPENVRML_MAJOR_VERSION, OPENVRML_MINOR_VERSION, OPENVRML_MICRO_VERSION);
  return vs;
}

double VrmlScene::getFrameRate() { return d_frameRate; }

/**
 * Queue an event to load URL/nodes (async so it can be called from a node)
 */
void VrmlScene::queueLoadUrl(const MFString & url,
                             const MFString & parameters) {
    if (!this->d_pendingNodes && !this->d_pendingUrl) {
        this->d_pendingUrl = new MFString(url);
        this->d_pendingParameters = new MFString(parameters);
    }
}

void VrmlScene::queueReplaceNodes(const MFNode & nodes, VrmlNamespace & ns) {
    if (!this->d_pendingNodes && !this->d_pendingUrl) {
        this->d_pendingNodes = new MFNode(nodes);
        d_pendingScope = &ns;
    }
}

/**
 * Event processing. Current events are in the array 
 * d_eventMem[d_firstEvent,d_lastEvent). If d_firstEvent == d_lastEvent,
 * the queue is empty. There is a fixed maximum number of events. If we
 * are so far behind that the queue is filled, the oldest events get
 * overwritten.
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
 * Any events waiting to be distributed?
 */
bool VrmlScene::eventsPending() 
{ return d_firstEvent != d_lastEvent; }


/**
 * Discard all pending events
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
 * The update method is where the events are processed. It should be
 * called after each frame is rendered.
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

#if 0
static void
matrix_to_glmatrix(double M[4][4], float GLM[16])
{
  GLM[0]  = M[0][0];
  GLM[1]  = M[1][0];
  GLM[2]  = M[2][0];
  GLM[3]  = M[3][0];
  GLM[4]  = M[0][1];
  GLM[5]  = M[1][1];
  GLM[6]  = M[2][1];
  GLM[7]  = M[3][1];
  GLM[8]  = M[0][2];
  GLM[9]  = M[1][2];
  GLM[10] = M[2][2];
  GLM[11] = M[3][2];
  GLM[12] = M[0][3];
  GLM[13] = M[1][3];
  GLM[14] = M[2][3];
  GLM[15] = M[3][3];
}
#endif

/**
 * Draw this scene into the specified viewer
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

//
//  Bindable children node stacks. For the CS purists out there, these 
//  aren't really stacks as they allow arbitrary elements to be removed
//  (not just the top).
//

const NodePtr VrmlScene::bindableTop(const BindStack & stack) {
    return stack.empty() ? NodePtr(0) : stack.front();
}

void VrmlScene::bindablePush(BindStack & stack, const NodePtr & node) {
    bindableRemove( stack, node ); // Remove any existing reference
    stack.push_front(node);
    setModified();
}

void VrmlScene::bindableRemove(BindStack & stack, const NodePtr & node) {
    for (std::list<NodePtr>::iterator i(stack.begin());
            i != stack.end(); ++i) {
        if ( *i == node ) {
            stack.erase( i );
            setModified();
            break;
        }
    }
}


// The nodes in the "set of all nodes of this type" lists are not 
// ref'd/deref'd because they are only added to and removed from 
// the lists in their constructors and destructors.


// Bindable children nodes (stacks)
// Define for each Type:
//    add/remove to complete list of nodes of this type
//    NodeType *bindableTypeTop();
//    void bindablePush(NodeType *);
//    void bindableRemove(NodeType *);

// Background

void VrmlScene::addBackground(Vrml97Node::Background & node) {
    this->d_backgrounds.push_back(&node);
}

void VrmlScene::removeBackground(Vrml97Node::Background & node) {
    const std::list<Node *>::iterator end = this->d_backgrounds.end();
    const std::list<Node *>::iterator pos =
            std::find(this->d_backgrounds.begin(), end, &node);
    if (pos != end) {
        this->d_backgrounds.erase(pos);
    }
}

Vrml97Node::Background * VrmlScene::bindableBackgroundTop() {
    Node * const b = bindableTop(d_backgroundStack).get();
    return b ? b->toBackground() : 0;
}

void VrmlScene::bindablePush(Vrml97Node::Background * n) {
    bindablePush(d_backgroundStack, NodePtr(n));
}

void VrmlScene::bindableRemove(Vrml97Node::Background * n) {
    bindableRemove(d_backgroundStack, NodePtr(n));
}

// Fog

void VrmlScene::addFog(Vrml97Node::Fog & n) {
    this->d_fogs.push_back(&n);
}

void VrmlScene::removeFog(Vrml97Node::Fog & node) {
    const std::list<Node *>::iterator end = this->d_fogs.end();
    const std::list<Node *>::iterator pos =
            std::find(this->d_fogs.begin(), end, &node);
    if (pos != end) {
        this->d_fogs.erase(pos);
    }
}

Vrml97Node::Fog * VrmlScene::bindableFogTop() {
    Node * const f = bindableTop(d_fogStack).get();
    return f ? f->toFog() : 0;
}

void VrmlScene::bindablePush(Vrml97Node::Fog * n) {
    bindablePush(d_fogStack, NodePtr(n));
}

void VrmlScene::bindableRemove(Vrml97Node::Fog * n) {
    bindableRemove(d_fogStack, NodePtr(n));
}

// NavigationInfo
void VrmlScene::addNavigationInfo(Vrml97Node::NavigationInfo & n) {
    this->d_navigationInfos.push_back(&n);
}

void VrmlScene::removeNavigationInfo(Vrml97Node::NavigationInfo & node) {
    const std::list<Node *>::iterator end = this->d_navigationInfos.end();
    const std::list<Node *>::iterator pos =
            std::find(this->d_navigationInfos.begin(), end, &node);
    if (pos != end) {
        this->d_navigationInfos.erase(pos);
    }
}

Vrml97Node::NavigationInfo * VrmlScene::bindableNavigationInfoTop() {
    Node * const n = bindableTop(d_navigationInfoStack).get();
    return n ? n->toNavigationInfo() : 0;
}

void VrmlScene::bindablePush(Vrml97Node::NavigationInfo * n) {
    bindablePush(d_navigationInfoStack, NodePtr(n));
}

void VrmlScene::bindableRemove(Vrml97Node::NavigationInfo * n) {
    bindableRemove(d_navigationInfoStack, NodePtr(n));
}

// Viewpoint
void VrmlScene::addViewpoint(Vrml97Node::Viewpoint & n) {
    this->d_viewpoints.push_back(&n);
}

void VrmlScene::removeViewpoint(Vrml97Node::Viewpoint & node) {
    const std::list<Node *>::iterator end = this->d_viewpoints.end();
    const std::list<Node *>::iterator pos =
            std::find(this->d_viewpoints.begin(), end, &node);
    if (pos != end) {
        this->d_viewpoints.erase(pos);
    }
}

Vrml97Node::Viewpoint * VrmlScene::bindableViewpointTop() {
    Node * const t = bindableTop(d_viewpointStack).get();
    return t ? t->toViewpoint() : 0;
}

void VrmlScene::bindablePush(Vrml97Node::Viewpoint * n) {
    bindablePush(d_viewpointStack, NodePtr(n));
    d_newView = true;
}

void VrmlScene::bindableRemove(Vrml97Node::Viewpoint * n) {
    bindableRemove(d_viewpointStack, NodePtr(n));
    d_newView = true;
}

/**
 * Bind to the next viewpoint in the list.
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

int VrmlScene::nViewpoints() { return d_viewpoints.size(); }

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

void VrmlScene::setViewpoint(int nvp) {
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

// The nodes in these lists are not ref'd/deref'd because they
// are only added to and removed from the lists in their constructors
// and destructors.


// Scene-level distance-scoped lights

void VrmlScene::addScopedLight(Vrml97Node::AbstractLight & light) {
    this->d_scopedLights.push_back(&light);
}

void VrmlScene::removeScopedLight(Vrml97Node::AbstractLight & light) {
    this->d_scopedLights.remove(&light);
}


// Movies

void VrmlScene::addMovie(Vrml97Node::MovieTexture & movie) {
    this->d_movies.push_back(&movie);
}

void VrmlScene::removeMovie(Vrml97Node::MovieTexture & movie) {
    this->d_movies.remove(&movie);
}

// Scripts

void VrmlScene::addScript(ScriptNode & script) {
    this->d_scripts.push_back(&script);
}

void VrmlScene::removeScript(ScriptNode & script) {
    this->d_scripts.remove(&script);
}

void VrmlScene::addProto(ProtoNode & node) {
    this->protoNodeList.push_back(&node);
}

void VrmlScene::removeProto(ProtoNode & node) {
    this->protoNodeList.remove(&node);
}

// TimeSensors

void VrmlScene::addTimeSensor(Vrml97Node::TimeSensor & timer) {
    this->d_timers.push_back(&timer);
}

void VrmlScene::removeTimeSensor(Vrml97Node::TimeSensor & timer) {
    this->d_timers.remove(&timer);
}


// AudioClips

void VrmlScene::addAudioClip(Vrml97Node::AudioClip & audio_clip) {
    this->d_audioClips.push_back(&audio_clip);
}

void VrmlScene::removeAudioClip(Vrml97Node::AudioClip & audio_clip) {
    this->d_audioClips.remove(&audio_clip);
}


/**
 * Propagate the bvolume dirty flag from children to ancestors. The
 * invariant is that if a node's bounding volume is out of date,
 * then the bounding volumes of all that nodes's ancestors must be
 * out of date. However, Node does not maintain a parent
 * pointer. So we must do a traversal of the entire scene graph to
 * do the propagation.
 *
 * @see Node::setBVolumeDirty
 * @see Node::isBVolumeDirty
 */
void VrmlScene::updateFlags()
{
//  Node* root = this->getRoot();
//  root->updateModified(0x002);
}

} // namespace OpenVRML
