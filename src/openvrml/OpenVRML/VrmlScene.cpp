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

#include "VrmlScene.h"

#include <errno.h>
#include <stdio.h>

#ifdef _WIN32
#include <strstrea.h>
#else
#include <strstream.h>
#endif

#include "doc2.hpp"
#include "Viewer.h"
#include "System.h"
#include "MathUtils.h"
#include "VrmlNamespace.h"
#include "nodetype.h"
#include "VrmlRenderContext.h"

#include "Vrml97Parser.cpp"

// List of Scripts in the scene
#include "VrmlNodeScript.h"

// Max time in seconds between updates. Make this user
// setable to balance performance with cpu usage.
#ifndef DEFAULT_DELTA
#define DEFAULT_DELTA 0.5
#endif


//
// Create a VrmlScene from a URL (optionally loading from a local copy,
// so I can run as a netscape helper but still retrieve embedded urls).
//

VrmlScene::VrmlScene(const std::string & sceneUrl,
                     const std::string & localCopy):
        d_flags_need_updating(false), d_url(0), d_urlLocal(0), d_namespace(0),
        d_modified(false), d_newView(false), d_deltaTime(DEFAULT_DELTA),
        d_pendingUrl(0), d_pendingParameters(0), d_pendingNodes(0),
        d_pendingScope(0), d_frameRate(0.0), d_firstEvent(0), d_lastEvent(0) {
    for (size_t i = 0; i < this->nodes.getLength(); ++i) {
        this->nodes.getElement(i)->addToScene(this, sceneUrl);
        this->nodes.getElement(i)->accumulateTransform(0);
    }

    if (sceneUrl.length() > 0)
      if (!this->load(sceneUrl, localCopy))
        theSystem->error("Couldn't load '%s'.\n", sceneUrl.c_str());
}

VrmlScene::~VrmlScene() {
    delete d_url;
    delete d_urlLocal;

    delete d_pendingUrl;
    delete d_pendingParameters;
    delete d_pendingNodes;
    delete d_pendingScope;

    delete d_namespace;
}

const VrmlMFNode & VrmlScene::getRootNodes() const throw () {
    return this->nodes;
}

// Load a (possibly non-VRML) file...

bool VrmlScene::loadUrl(const VrmlMFString & url,
                        const VrmlMFString & parameters) {
    
    size_t np = parameters.getLength();
    
    // try each url until we find one we can handle
    size_t i(0);
    for (; i < url.getLength(); ++i) {
        using std::string;
        const string & currentUrl = url.getElement(i);
        if (currentUrl.length() == 0) {
            continue;
        }
        
        // #Viewpoint
        if (currentUrl[0] == '#') {
            if (load(url.getElement(i))) {
                break;
            }
        }
        
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
                if (load(currentUrl)) {
                    break;
                }
	    } else {
                if (theSystem->loadUrl(currentUrl, parameters)) {
                    break;
                }
            }
        }
    }
    
    return (i != url.getLength());  // true if we found a url that loaded
}

// Called by viewer when a destroy request is received. The request
// is just passed on to the client via the worldChanged CB.

void VrmlScene::destroyWorld() {
    doCallbacks( DESTROY_WORLD );
}


// Replace nodes

void VrmlScene::replaceWorld(VrmlMFNode & nodes, VrmlNamespace * ns,
			     Doc2 * url, Doc2 * urlLocal) {
    delete d_namespace;
    delete d_url;
    delete d_urlLocal;
    
    d_namespace = ns;
    d_url = url;
    d_urlLocal = urlLocal;
    
    // Clear bindable stacks.
    this->d_backgroundStack.clear();
    this->d_fogStack.clear(); 
    this->d_navigationInfoStack.clear(); 
    this->d_viewpointStack.clear();
    
    // Get rid of current world: pending events, nodes.
    flushEvents();
    
    this->nodes = nodes;

  
    // Do this to set the relative URL
    for (size_t i = 0; i < this->nodes.getLength(); ++i) {
        nodes.getElement(i)->addToScene(this, this->d_url ? this->d_url->url() : 0);
	    nodes.getElement(i)->accumulateTransform(0);
    }
    
    // Send initial set_binds to bindable nodes
    double timeNow = theSystem->time();
    VrmlSFBool flag(true);
    
    if (!this->d_backgrounds.empty()) {
        assert(this->d_backgrounds.front());
        this->d_backgrounds.front()->eventIn(timeNow, "set_bind", flag);
    }
    
    if (!this->d_fogs.empty()) {
        assert(this->d_fogs.front());
        this->d_fogs.front()->eventIn(timeNow, "set_bind", flag);
    }
    
    if (!this->d_navigationInfos.empty()) {
        assert(this->d_navigationInfos.front());
        this->d_navigationInfos.front()->eventIn(timeNow, "set_bind", flag);
    }
    
    if (!this->d_viewpoints.empty()) {
        assert(this->d_viewpoints.front());
        this->d_viewpoints.front()->eventIn(timeNow, "set_bind", flag);
    }
    
    // Notify anyone interested that the world has changed
    doCallbacks( REPLACE_WORLD );
    
    setModified();
    d_newView = true;		// Force resetUserNav
}

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

// Read a VRML97 file.
// This is only for [*.wrl][#viewpoint] url loading (no parameters).

bool VrmlScene::load(const std::string & url, const std::string & localCopy) {
    // Look for '#Viewpoint' syntax. There ought to be a current
    // scene if this format is used.
    VrmlSFBool flag(true);
    if (url[0] == '#') {
        const VrmlNodePtr & vp = d_namespace
                                    ? d_namespace->findNode(url.substr(1))
                                    : VrmlNodePtr(0);
        
        // spec: ignore if named viewpoint not found
        if (vp) {
	    vp->eventIn(theSystem->time(), "set_bind", flag);
	    setModified();
	}
        
        return true;
    }
    
    // Try to load a file. Prefer a local copy if available.
    Doc2 * tryUrl = 0;
    if (localCopy.length() > 0) {
        tryUrl = new Doc2(localCopy, 0);
    } else {
        tryUrl = new Doc2(url, d_url);
    }
    
    VrmlNamespace * newScope = new VrmlNamespace();
    VrmlMFNode * newNodes = readWrl(tryUrl, newScope);
    
    if (newNodes) {
        Doc2 * sourceUrl = tryUrl;
        Doc2 * urlLocal = 0;
        if (localCopy.length() > 0) {
            sourceUrl = new Doc2( url );
            urlLocal = tryUrl;
        }
        
        replaceWorld(*newNodes, newScope, sourceUrl, urlLocal);
        delete newNodes;
        
        // Look for '#Viewpoint' syntax
        if (sourceUrl->urlModifier()) {
            const VrmlNodePtr & vp = d_namespace->findNode(sourceUrl->urlModifier() + 1);
            double timeNow = theSystem->time();
            if (vp) {
                vp->eventIn(timeNow, "set_bind", flag);
            }
	}
        
        return true;      // Success.
    }
    
    delete tryUrl;
    return false;
}


// Read a VRML file from one of the urls.

VrmlMFNode * VrmlScene::readWrl(const VrmlMFString & urls, Doc2 * relative,
                                VrmlNamespace * ns)
{
    Doc2 url;
    const size_t n = urls.getLength();
    for (size_t i = 0; i < n; ++i) {
        //theSystem->debug("Trying to read url '%s'\n", urls->get(i));
        url.seturl(urls.getElement(i).c_str(), relative );
        VrmlMFNode * kids = VrmlScene::readWrl(&url, ns);
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


// Read a VRML file and return the (valid) nodes.
VrmlMFNode * VrmlScene::readWrl(Doc2 * tryUrl, VrmlNamespace * ns)
{
    VrmlMFNode * result = 0;
    
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
        
        result = new VrmlMFNode();
        try {
            parser.vrmlScene(*result, *rootNamespace, tryUrl);
        } catch (std::exception & ex) {
            cerr << ex.what() << endl;
        }
    }
    
    return result;
}

bool VrmlScene::loadFromString( const char *vrmlString )
{
    VrmlNamespace * newScope = new VrmlNamespace();
    VrmlMFNode newNodes = readString(vrmlString, newScope);
    if (newNodes.getLength() > 0) {
        replaceWorld(newNodes, newScope, 0, 0);
        return true;
    }
    return false;
}

// Read VRML from a string and return the (valid) nodes.

const VrmlMFNode VrmlScene::readString(char const * vrmlString, VrmlNamespace * ns)
{
    //
    // Hmm. It looks like passing a zero pointer for the namespace argument of
    // this function is not an option. Perhaps it should be a reference rather
    // than a pointer?
    // -- Braden McDaniel <braden@endoframe.com> 1 Apr, 2000
    //
    
    VrmlMFNode result;
    
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
        parser.vrmlScene(result, *ns, 0);
    }
    
    return result;
}


# if 0
// Load VRML from an application-provided callback function

bool VrmlScene::loadFromFunction(LoadCB cb, char const * url)
{
    Doc2 * doc = url ? new Doc2(url, 0) : 0;
    VrmlNamespace * ns = new VrmlNamespace();
    VrmlMFNode * newNodes = readFunction(cb, doc, ns);
    
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

VrmlMFNode * VrmlScene::readFunction(LoadCB cb, Doc2 * url, VrmlNamespace * ns)
{
    VrmlMFNode * result = 0;
    
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
# endif


// Read a PROTO from a URL to get the implementation of an EXTERNPROTO.
// This should read only PROTOs and return when the first/specified PROTO
// is read...

const NodeTypePtr VrmlScene::readPROTO(const VrmlMFString & urls,
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
        VrmlMFNode * kids = VrmlScene::readWrl(&urlDoc, protos);
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

// Write the current scene to a file.
// Need to save the PROTOs/EXTERNPROTOs too...

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

// Queue an event to load URL/nodes (async so it can be called from a node)

void VrmlScene::queueLoadUrl( VrmlMFString *url, VrmlMFString *parameters )
{
  if (! d_pendingNodes && ! d_pendingUrl)
    {
      d_pendingUrl = url->clone()->toMFString();
      d_pendingParameters = parameters->clone()->toMFString();
    }
}

void VrmlScene::queueReplaceNodes( VrmlMFNode *nodes, VrmlNamespace *ns )
{
  if (! d_pendingNodes && ! d_pendingUrl)
    {
      d_pendingNodes = nodes->clone()->toMFNode();
      d_pendingScope = ns;
    }
}

// Event processing. Current events are in the array 
// d_eventMem[d_firstEvent,d_lastEvent). If d_firstEvent == d_lastEvent,
// the queue is empty. There is a fixed maximum number of events. If we
// are so far behind that the queue is filled, the oldest events get
// overwritten.

void VrmlScene::queueEvent(double timeStamp, VrmlField * value,
			   const VrmlNodePtr & toNode,
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

// Any events waiting to be distributed?

bool VrmlScene::eventsPending() 
{ return d_firstEvent != d_lastEvent; }


// Discard all pending events

void VrmlScene::flushEvents()
{
  while (d_firstEvent != d_lastEvent)
    {
      Event *e = &d_eventMem[d_firstEvent];
      d_firstEvent = (d_firstEvent+1) % MAXEVENTS;
      delete e->value;
    }
}

// Called by the viewer when the cursor passes over, clicks, drags, or
// releases a sensitive object (an Anchor or another grouping node with 
// an enabled TouchSensor child).

void VrmlScene::sensitiveEvent( void *object,
				double timeStamp,
				bool isOver, bool isActive,
				double *point )
{
  VrmlNode * const n = reinterpret_cast<VrmlNode *>(object);

  if (n)
    {
      VrmlNodeAnchor *a = n->toAnchor();
      if ( a )
	{
	  // This should really be (isOver && !isActive && n->wasActive)
	  // (ie, button up over the anchor after button down over the anchor)
	  if (isActive && isOver)
	    {
	      a->activate();
	      //theSystem->inform("");
	    }
	  else if (isOver)
	    {
	      const std::string & description = a->description();
	      const std::string & url = a->url();
	      if (description.length() > 0 && url.length() > 0)
		theSystem->inform("%s (%s)", description.c_str(), url.c_str());
	      else if (description.length() > 0 || url.length() > 0)
		theSystem->inform("%s", description.length() > 0 ? description.c_str() : url.c_str());
	      //else
	      //theSystem->inform("");
	    }
	  //else
	  //theSystem->inform("");
	}

      // The parent grouping node is registered for Touch/Drag Sensors
      else
	{
	  VrmlNodeGroup *g = n->toGroup();
	  if (g)
	    {
	      //theSystem->inform("");
	      g->activate( timeStamp, isOver, isActive, point );
	      setModified();
	    }
	}
      
    }

  //else
  //theSystem->inform("");
}

//
// The update method is where the events are processed. It should be
// called after each frame is rendered.
//
bool VrmlScene::update( double timeStamp )
{
  if (timeStamp <= 0.0) timeStamp = theSystem->time();
  VrmlSFTime now( timeStamp );

  d_deltaTime = DEFAULT_DELTA;

  // Update each of the timers.
  std::list<VrmlNode *>::iterator i, end = this->d_timers.end();
  for (i = this->d_timers.begin(); i != end; ++i)
    {
      VrmlNodeTimeSensor *t = (*i)->toTimeSensor();
      if (t) t->update( now );
    }

  // Update each of the clips.
  end = this->d_audioClips.end();
  for (i = this->d_audioClips.begin(); i != end; ++i)
    {
      VrmlNodeAudioClip *c = (*i)->toAudioClip();
      if (c) c->update( now );
    }

  // Update each of the scripts.
  end = this->d_scripts.end();
  for (i = this->d_scripts.begin(); i != end; ++i)
    {
      VrmlNodeScript *s = (*i)->toScript();
      if (s) s->update( now );
    }

  // Update each of the movies.
  end = this->d_movies.end();
  for (i = this->d_movies.begin(); i != end; ++i)
    {
      VrmlNodeMovieTexture *m =  (*i)->toMovieTexture();
      if (m) m->update( now );
    }


  // Pass along events to their destinations
  while (this->d_firstEvent != this->d_lastEvent &&
	 ! this->d_pendingUrl && ! this->d_pendingNodes)
    {
      Event *e = &d_eventMem[d_firstEvent];
      d_firstEvent = (d_firstEvent+1) % MAXEVENTS;

      // Ensure that the node is in the scene graph
      const VrmlNodePtr & n = e->toNode;
      if (this != n->scene())
	{
	  theSystem->debug("VrmlScene::update: %s::%s is not in the scene graph yet.\n",
                           n->type.getId().c_str(), n->getId().c_str());
	  n->addToScene((VrmlScene*)this, urlDoc()->url() );
	}
      n->eventIn(e->timeStamp, e->toEventIn, *e->value);
      // this needs to change if event values are shared...
      delete e->value;
    }

  if (d_pendingNodes)
    {
      replaceWorld( *d_pendingNodes, d_pendingScope );
      delete d_pendingNodes;
      d_pendingNodes = 0;
      d_pendingScope = 0;
    }
  else if (d_pendingUrl)
    {
      loadUrl(*d_pendingUrl, *d_pendingParameters);
      delete d_pendingUrl;
      delete d_pendingParameters;
      d_pendingUrl = 0;
      d_pendingParameters = 0;
    }

  // Signal a redisplay if necessary
  return isModified();
}




bool VrmlScene::headlightOn()
{
  VrmlNodeNavigationInfo *navInfo = bindableNavigationInfoTop();
  if (navInfo)
    return navInfo->headlightOn();
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

// Draw this scene into the specified viewer

void VrmlScene::render(Viewer *viewer)
{
  if (d_newView)
    {
      viewer->resetUserNavigation();
      d_newView = false;
    }
      
  // Default viewpoint parameters
  float position[3] = { 0.0, 0.0, 10.0 };
  float orientation[4] = { 0.0, 0.0, 1.0, 0.0 };
  float field = 0.785398;
  float avatarSize = 0.25;
  float visibilityLimit = 0.0;

  VrmlNodeViewpoint *vp = bindableViewpointTop();
  if (vp)
    {
      std::copy(vp->getPosition().get(), vp->getPosition().get() + 3, position);
      std::copy(vp->getOrientation().get(), vp->getOrientation().get() + 4,
                orientation);
      field = vp->getFieldOfView().get();
    }

  VrmlNodeNavigationInfo *ni = bindableNavigationInfoTop();
  if (ni)
    {
      avatarSize = ni->avatarSize()[0];
      visibilityLimit = ni->visibilityLimit();
    }

    // Activate the headlight.
  // ambient is supposed to be 0 according to the spec...
  if ( headlightOn() )
  {
    float rgb[3] = { 1.0, 1.0, 1.0 };
    float xyz[3] = { 0.0, 0.0, -1.0 };
    float ambient = 0.3;
 
    viewer->insertDirLight( ambient, 1.0, rgb, xyz );
  }                         

  // sets the viewpoint transformation
  //
  viewer->setViewpoint(position, orientation, field, avatarSize, visibilityLimit);


  // Set background.

  VrmlNodeBackground *bg = bindableBackgroundTop();
  if (bg)
    { // Should be transformed by the accumulated rotations above ...
      bg->renderBindable(viewer);
    }
  else
    viewer->insertBackground();	// Default background

  // Fog
  VrmlNodeFog *f = bindableFogTop();
  if (f)
    {
      viewer->setFog(f->color(), f->visibilityRange(), f->fogType().c_str());
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
  if(vp)
  {
   VrmlMatrix IM,NMAT;
   vp->inverseTransform(IM);   // put back nested viewpoint. skb
   viewer->MatrixMultiply(IM.get());
   vp->getInverseMatrix(MV);
   viewer->getUserNavigation(NMAT);
   MV = MV.multLeft(NMAT);
   MV = MV.multLeft(IM);
  }
  else
  {
    // if there's no viewpoint, then set us up arbitrarily at 0,0,-10,
    // as indicated in the vrml spec (section 6.53 Viewpoint).
    //
    float t[3] = { 0.0f, 0.0f, -10.0f };
    VrmlMatrix NMAT;
    MV.setTranslate(t);
    viewer->getUserNavigation(NMAT);
    MV = MV.multLeft(NMAT);
  }     

  VrmlRenderContext rc(VrmlBVolume::BV_PARTIAL, MV);
  rc.setDrawBSpheres(true);

  // Do the scene-level lights (Points and Spots)
  std::list<VrmlNode *>::iterator li, end = this->d_scopedLights.end();
  for (li = this->d_scopedLights.begin(); li != end; ++li)
    {
      VrmlNodeLight* x = (*li)->toLight();
      if (x) x->renderScoped( viewer );
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
  if (eventsPending())
    setDelta( 0.0 );
    
}

//
//  Bindable children node stacks. For the CS purists out there, these 
//  aren't really stacks as they allow arbitrary elements to be removed
//  (not just the top).
//

const VrmlNodePtr VrmlScene::bindableTop(const BindStack & stack) {
    return stack.empty() ? VrmlNodePtr(0) : stack.front();
}

void VrmlScene::bindablePush(BindStack & stack, const VrmlNodePtr & node) {
    bindableRemove( stack, node ); // Remove any existing reference
    stack.push_front(node);
    setModified();
}

void VrmlScene::bindableRemove(BindStack & stack, const VrmlNodePtr & node) {
    for (std::list<VrmlNodePtr>::iterator i(stack.begin());
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
//    VrmlNodeType *bindableTypeTop();
//    void bindablePush(VrmlNodeType *);
//    void bindableRemove(VrmlNodeType *);

// Background

void VrmlScene::addBackground(VrmlNodeBackground & node) {
    this->d_backgrounds.push_back(&node);
}

void VrmlScene::removeBackground(VrmlNodeBackground & node) {
    this->d_backgrounds.remove(&node);
}

VrmlNodeBackground *VrmlScene::bindableBackgroundTop() {
    VrmlNode * const b = bindableTop(d_backgroundStack).get();
    return b ? b->toBackground() : 0;
}

void VrmlScene::bindablePush(VrmlNodeBackground * n) {
    bindablePush(d_backgroundStack, VrmlNodePtr(n));
}

void VrmlScene::bindableRemove( VrmlNodeBackground *n )
{
    bindableRemove(d_backgroundStack, VrmlNodePtr(n));
}

// Fog

void VrmlScene::addFog(VrmlNodeFog & n) {
    this->d_fogs.push_back(&n);
}

void VrmlScene::removeFog(VrmlNodeFog & n) {
    this->d_fogs.remove(&n);
}

VrmlNodeFog *VrmlScene::bindableFogTop()
{
  VrmlNode * const f = bindableTop(d_fogStack).get();
  return f ? f->toFog() : 0;
}

void VrmlScene::bindablePush( VrmlNodeFog *n )  
{
    bindablePush(d_fogStack, VrmlNodePtr(n));
}

void VrmlScene::bindableRemove( VrmlNodeFog *n )  
{
    bindableRemove(d_fogStack, VrmlNodePtr(n));
}

// NavigationInfo
void VrmlScene::addNavigationInfo(VrmlNodeNavigationInfo & n) {
    this->d_navigationInfos.push_back(&n);
}

void VrmlScene::removeNavigationInfo(VrmlNodeNavigationInfo & n) {
    this->d_navigationInfos.remove(&n);
}

VrmlNodeNavigationInfo *VrmlScene::bindableNavigationInfoTop()
{
  VrmlNode * const n = bindableTop(d_navigationInfoStack).get();
  return n ? n->toNavigationInfo() : 0;
}

void VrmlScene::bindablePush( VrmlNodeNavigationInfo *n )
{
    bindablePush(d_navigationInfoStack, VrmlNodePtr(n));
}

void VrmlScene::bindableRemove( VrmlNodeNavigationInfo *n )
{
    bindableRemove(d_navigationInfoStack, VrmlNodePtr(n));
}

// Viewpoint
void VrmlScene::addViewpoint(VrmlNodeViewpoint & n) {
    this->d_viewpoints.push_back(&n);
}

void VrmlScene::removeViewpoint(VrmlNodeViewpoint & n) {
    this->d_viewpoints.remove(&n);
}

VrmlNodeViewpoint *VrmlScene::bindableViewpointTop()
{
  VrmlNode * const t = bindableTop(d_viewpointStack).get();
  return t ? t->toViewpoint() : 0;
}

void VrmlScene::bindablePush( VrmlNodeViewpoint *n )
{
  bindablePush(d_viewpointStack, VrmlNodePtr(n));
  d_newView = true;
}

void VrmlScene::bindableRemove( VrmlNodeViewpoint *n )
{
  bindableRemove(d_viewpointStack, VrmlNodePtr(n));
  d_newView = true;
}

// Bind to the next viewpoint in the list

void VrmlScene::nextViewpoint()
{
  VrmlNodeViewpoint *vp = bindableViewpointTop();
  std::list<VrmlNode *>::iterator i;

  for (i = d_viewpoints.begin(); i != d_viewpoints.end(); ++i )
    if (*i == vp)
      {
	if (++i == d_viewpoints.end())
	  i = d_viewpoints.begin();

	VrmlSFBool flag(true);
	if ((*i) && (vp = (*i)->toViewpoint()) != 0)
	  vp->eventIn(theSystem->time(), "set_bind", flag);

	return;
      }
}
  

void VrmlScene::prevViewpoint()
{
  VrmlNodeViewpoint *vp = bindableViewpointTop();
  std::list<VrmlNode *>::iterator i;

  for (i = d_viewpoints.begin(); i != d_viewpoints.end(); ++i )
    if (*i == vp)
      {
	if (i == d_viewpoints.begin())
	  i = d_viewpoints.end();

	VrmlSFBool flag(true);
	if ( *(--i) && (vp = (*i)->toViewpoint()) != 0 )
	  vp->eventIn(theSystem->time(), "set_bind", flag);

	return;
      }
}

int VrmlScene::nViewpoints() { return d_viewpoints.size(); }

void VrmlScene::getViewpoint(const size_t nvp, std::string & name,
                             std::string & description) {
    std::list<VrmlNode *>::const_iterator i = this->d_viewpoints.begin();
    size_t n = 0;
    for (; i != this->d_viewpoints.end(); ++i, ++n ) {
        if (n == nvp) {
            name = (*i)->getId();
            description = (*i)->toViewpoint()->description();
            return;
        }
    }
}

void VrmlScene::setViewpoint(const std::string & name,
                             const std::string & description) {
    std::list<VrmlNode *>::iterator i = this->d_viewpoints.begin();
    for (; i != this->d_viewpoints.end(); ++i) {
        if (name == (*i)->getId()
                && description == (*i)->toViewpoint()->description()) {
            VrmlNodeViewpoint * vp;
            VrmlSFBool flag(true);
            if ((vp = (*i)->toViewpoint()) != 0) {
                vp->eventIn(theSystem->time(), "set_bind", flag);
            }
            return;
        }
    }
}

void VrmlScene::setViewpoint(int nvp)
{
  std::list<VrmlNode *>::iterator i;
  int j = 0;

  for (i = d_viewpoints.begin(); i != d_viewpoints.end(); ++i) {
    if (j == nvp)
      {
	VrmlNodeViewpoint *vp;
	VrmlSFBool flag(true);
	if ((vp = (*i)->toViewpoint()) != 0)
	  vp->eventIn(theSystem->time(), "set_bind", flag);
	return;
      }
    ++j;
  }
}

// The nodes in these lists are not ref'd/deref'd because they
// are only added to and removed from the lists in their constructors
// and destructors.


// Scene-level distance-scoped lights

void VrmlScene::addScopedLight(VrmlNodeLight & light) {
    this->d_scopedLights.push_back(&light);
}

void VrmlScene::removeScopedLight(VrmlNodeLight & light) {
    this->d_scopedLights.remove(&light);
}


// Movies

void VrmlScene::addMovie(VrmlNodeMovieTexture & movie) {
    this->d_movies.push_back(&movie);
}

void VrmlScene::removeMovie(VrmlNodeMovieTexture & movie) {
    this->d_movies.remove(&movie);
}

// Scripts

void VrmlScene::addScript(VrmlNodeScript & script) {
    this->d_scripts.push_back(&script);
}

void VrmlScene::removeScript(VrmlNodeScript & script) {
    this->d_scripts.remove(&script);
}

// TimeSensors

void VrmlScene::addTimeSensor(VrmlNodeTimeSensor & timer) {
    this->d_timers.push_back(&timer);
}

void VrmlScene::removeTimeSensor(VrmlNodeTimeSensor & timer) {
    this->d_timers.remove(&timer);
}


// AudioClips

void VrmlScene::addAudioClip(VrmlNodeAudioClip & audio_clip) {
    this->d_audioClips.push_back(&audio_clip);
}

void VrmlScene::removeAudioClip(VrmlNodeAudioClip & audio_clip) {
    this->d_audioClips.remove(&audio_clip);
}


/**
 * Propagate the bvolume dirty flag from children to ancestors. The
 * invariant is that if a node's bounding volume is out of date,
 * then the bounding volumes of all that nodes's ancestors must be
 * out of date. However, VrmlNode does not maintain a parent
 * pointer. So we must do a traversal of the entire scene graph to
 * do the propagation.
 *
 * @see VrmlNode::setBVolumeDirty
 * @see VrmlNode::isBVolumeDirty
 */
void VrmlScene::updateFlags()
{
//  VrmlNode* root = this->getRoot();
//  root->updateModified(0x002);
}
