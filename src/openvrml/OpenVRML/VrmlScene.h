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

#ifndef VRMLSCENE_H
#define VRMLSCENE_H

#include <list>
#include "common.h"
#include "vrml97node.h"

class VrmlNode;

// List of nodes
//class VrmlNodeList; Why doesn't this work? 
// I would rather not include <list> in here.
typedef std::list<VrmlNodePtr> VrmlNodeList;


class Doc2;
class Viewer;

// PROTO definitions
class VrmlNamespace;
class VrmlNodeType;

OPENVRML_SCOPE class VrmlScene {

public:

  // These are available without a scene object
  static VrmlMFNode * readWrl(VrmlMFString * url, Doc2 * relative,
                              VrmlNamespace * ns);
  static VrmlMFNode * readWrl(Doc2 * url, VrmlNamespace * ns );
  static const VrmlMFNode readString(char const * vrmlString,
                                     VrmlNamespace * ns);

//
// Disabling the LoadCB stuff for now. We probably want to replace this with
// functions that take an istream instead of a function pointer.
// -- Braden McDaniel <braden@endoframe.com>, 2 Apr, 2000
//
#if 0
  typedef int (*LoadCB)(char *buf, int bufSize);
  static VrmlMFNode * readFunction(LoadCB cb, Doc *url, VrmlNamespace *ns);
#endif

  static VrmlNodeType * readPROTO(const VrmlMFString & url,
                                  const Doc2 * relative = 0);

  //
  VrmlScene( const char *url = 0, const char *localCopy = 0);
  virtual ~VrmlScene();		// Allow overriding of Script API methods

  // Destroy world (just passes destroy request up to client)
  void destroyWorld();

  // Replace world with nodes, recording url as the source URL.
  void replaceWorld(VrmlMFNode & nodes, VrmlNamespace * ns,
		    Doc2 * url=0, Doc2 * urlLocal=0);

  // A way to let the app know when a world is loaded, changed, etc.
  typedef void (*SceneCB)( int reason );

  // Valid reasons for scene callback (need more...)
  enum {
    DESTROY_WORLD,
    REPLACE_WORLD
  };

  void addWorldChangedCallback( SceneCB );

  // Load a generic file (possibly non-VRML)
  bool loadUrl(VrmlMFString const * url, VrmlMFString const * parameters = 0 );

  // Load a VRML file
  bool load(const char *url, const char *localCopy = 0);

  // Load a VRML string
  bool loadFromString(const char *string);

#if 0
  // Load VRML from an application-provided callback function
  bool loadFromFunction( LoadCB, const char * url = 0 );
#endif

  // Save the scene to a file
  bool save(const char *url);

  // URL the current scene was loaded from
  Doc2 * urlDoc() { return d_url; }

  // Types and node names defined in this scope
  VrmlNamespace *scope() { return d_namespace; }

  // Queue an event to load URL/nodes (async so it can be called from a node)
  void queueLoadUrl( VrmlMFString *url, VrmlMFString *parameters );
  void queueReplaceNodes( VrmlMFNode *nodes, VrmlNamespace *ns );

  void sensitiveEvent( void *object, double timeStamp,
		       bool isOver, bool isActive, double *point );

  // Queue an event for a given node
  void queueEvent(double timeStamp,
		  VrmlField *value,
		  const VrmlNodePtr & toNode, const char *toEventIn);

  bool eventsPending();

  void flushEvents();

  // Script node API support functions. Can be overridden if desired.
  virtual const char *getName();
  virtual const char *getVersion();
  double getFrameRate();

  // Returns true if scene needs to be re-rendered
  bool update( double timeStamp = -1.0 );

  void render(Viewer *);

  // Indicate that the scene state has changed, need to re-render
  void setModified() { d_modified = true; }
  void clearModified() { d_modified = false; }
  bool isModified() const { return d_modified; }

  // Time until next update needed
  void setDelta(double d) { if (d < d_deltaTime) d_deltaTime = d; }
  double getDelta()       { return d_deltaTime; }

  // Bindable children nodes can be referenced via a list or bindable stacks.
  // Define for each bindableType:
  //    addBindableType(bindableType *);
  //    removeBindableType(bindableType *);
  //    bindableType *bindableTypeTop();
  //    void bindablePush(VrmlNodeType *);
  //    void bindableRemove(VrmlNodeType *);

  // Background
  void addBackground( VrmlNodeBackground * );
  void removeBackground( VrmlNodeBackground * );
  VrmlNodeBackground *bindableBackgroundTop();
  void bindablePush( VrmlNodeBackground * );
  void bindableRemove( VrmlNodeBackground * );

  // Fog
  void addFog( VrmlNodeFog * );
  void removeFog( VrmlNodeFog * );
  VrmlNodeFog *bindableFogTop();
  void bindablePush( VrmlNodeFog * );
  void bindableRemove( VrmlNodeFog * );

  // NavigationInfo
  void addNavigationInfo( VrmlNodeNavigationInfo * );
  void removeNavigationInfo( VrmlNodeNavigationInfo * );
  VrmlNodeNavigationInfo *bindableNavigationInfoTop();
  void bindablePush( VrmlNodeNavigationInfo * );
  void bindableRemove( VrmlNodeNavigationInfo * );

  // Viewpoint
  void addViewpoint( VrmlNodeViewpoint * );
  void removeViewpoint( VrmlNodeViewpoint * );
  VrmlNodeViewpoint *bindableViewpointTop();
  void bindablePush( VrmlNodeViewpoint * );
  void bindableRemove( VrmlNodeViewpoint * );

  // Viewpoint navigation
  void nextViewpoint();
  void prevViewpoint();  
  int nViewpoints();
  void getViewpoint(int, const char **, const char **);
  void setViewpoint(const char *, const char *);
  void setViewpoint(int);

  // Other (non-bindable) node types that the scene needs access to:

  // Scene-scoped lights
  void addScopedLight( VrmlNodeLight * );
  void removeScopedLight( VrmlNodeLight * );

  // Scripts
  void addScript( VrmlNodeScript * );
  void removeScript( VrmlNodeScript * );

  // TimeSensors
  void addTimeSensor( VrmlNodeTimeSensor * );
  void removeTimeSensor( VrmlNodeTimeSensor * );

  // AudioClips
  void addAudioClip( VrmlNodeAudioClip * );
  void removeAudioClip( VrmlNodeAudioClip * );

  // MovieTextures
  void addMovie( VrmlNodeMovieTexture * );
  void removeMovie( VrmlNodeMovieTexture * );


  VrmlNode* getRoot();

  /**
   * True if the bvolume dirty flag has been set on a node in the
   * scene graph, but has not yet been propegated to that node's
   * ancestors. Set by VrmlNode::setBVolumeDirty on any node in this
   * scene graph, cleared by updateFlags()
   */
  bool d_flags_need_updating;

  /**
   * Propegate the bvolume dirty flag from children to ancestors. The
   * invariant is that if a node's bounding volume is out of date,
   * then the bounding volumes of all that nodes's ancestors must be
   * out of date. However, VrmlNode does not maintain a parent
   * pointer. So we must do a traversal of the entire scene graph to
   * do the propegation.
   *
   * @see VrmlNode::setBVolumeDirty
   * @see VrmlNode::isBVolumeDirty
   */
  void updateFlags();


protected:

  bool headlightOn();
  void doCallbacks( int reason );

  // Document URL
  Doc2 * d_url;
  Doc2 * d_urlLocal;

  // Scene graph
  VrmlNodeGroup d_nodes;

  // Nodes and node types defined in this scope
  VrmlNamespace *d_namespace;

  // Need render
  bool d_modified;

  // New viewpoint has been bound
  bool d_newView;

  // Time until next update
  double d_deltaTime;

  // Allow requests to load urls, nodes to wait until events are processed
  VrmlMFString *d_pendingUrl;
  VrmlMFString *d_pendingParameters;

  VrmlMFNode *d_pendingNodes;
  VrmlNamespace *d_pendingScope;

  // Functions to call when world is changed
  typedef std::list < SceneCB > SceneCBList;
  SceneCBList d_sceneCallbacks;

  // frame rate
  double d_frameRate;

  // Bindable children nodes (pseudo-stacks - allow random access deletion).
  typedef VrmlNodeList* BindStack;

  // Generic bindable children stack operations
  const VrmlNodePtr bindableTop(BindStack);
  void bindablePush(BindStack, const VrmlNodePtr &);
  void bindableRemove(BindStack, const VrmlNodePtr &);
  void bindableRemoveAll( BindStack );

  //   Background
  VrmlNodeList* d_backgrounds;		// All backgrounds
  BindStack d_backgroundStack;		// Background stack

  //   Fog
  VrmlNodeList* d_fogs;			// All fog nodes
  BindStack d_fogStack;			// Fog stack

  //   NavigationInfo
  VrmlNodeList* d_navigationInfos;	// All navigation info nodes
  BindStack d_navigationInfoStack;	// Navigation info stack

  //   Viewpoint
  VrmlNodeList* d_viewpoints;		// All viewpoint nodes
  BindStack d_viewpointStack;		// Viewpoint stack

  // An event has a value and a destination, and is associated with a time
  struct Event{
    double timeStamp;
    VrmlField *value;
    VrmlNodePtr toNode;
    const char *toEventIn;
  };

  // For each scene can have a limited number of pending events.
  // Repeatedly allocating/freeing events is slow (it would be
  // nice to get rid of the field cloning, too), and if there are
  // so many events pending, we are probably running too slow to
  // handle them effectively anyway.
  // The event queue ought to be sorted by timeStamp...
  //static const int MAXEVENTS = 400; MSVC++5 doesn't like this.
  enum { MAXEVENTS = 400 };
  Event d_eventMem[MAXEVENTS];
  int d_firstEvent, d_lastEvent;

  // Scene-scoped lights (PointLights and SpotLights)
  VrmlNodeList* d_scopedLights;

  // Scripts in this scene
  VrmlNodeList* d_scripts;

  // Time sensors in this scene
  VrmlNodeList* d_timers;

  // Audio clips in this scene
  VrmlNodeList* d_audioClips;

  // Movies in this scene
  VrmlNodeList* d_movies;

};

#endif // VRMLSCENE_H

