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

# ifndef OPENVRML_VRMLSCENE_H
#   define OPENVRML_VRMLSCENE_H

#   include <list>
#   include <map>
#   include "common.h"
#   include "nodeclassptr.h"
#   include "script.h"
#   include "VrmlNamespace.h"
#   include "vrml97node.h"

namespace OpenVRML {

    class Doc2;
    class Viewer;
    class ProtoNode;

    typedef std::map<std::string, NodeClassPtr> NodeClassMap;
        
    class OPENVRML_SCOPE VrmlScene {
        friend class Vrml97Parser;
        friend class ProtoNodeClass;
        friend class Vrml97Node::Inline;
        
        NodeClassMap nodeClassMap;
        ScriptNodeClass scriptNodeClass;
        Doc2 * d_url;
        MFNode nodes;
        VrmlNamespace * scope;
        typedef std::list<NodePtr> BindStack;
        BindStack d_backgroundStack;
        BindStack d_fogStack;
        BindStack d_navigationInfoStack;
        BindStack d_viewpointStack;
        std::list<Node *> d_backgrounds;
        std::list<Node *> d_fogs;
        std::list<Node *> d_navigationInfos;
        std::list<Node *> d_viewpoints;
        std::list<Node *> d_scopedLights;
        std::list<ScriptNode *> d_scripts;
        std::list<Node *> d_timers;
        std::list<Node *> d_audioClips;
        std::list<Node *> d_movies;
        std::list<ProtoNode *> protoNodeList;
        bool d_modified;
        bool d_newView;
        double d_deltaTime;

    public:
# if 0
        static const NodeTypePtr readPROTO(const MFString & url,
                                           const Doc2 * relative = 0);
# endif

        explicit VrmlScene(const std::string & url);
        virtual ~VrmlScene();

        MFNode * readWrl(const MFString & urls, Doc2 * relative,
                         VrmlNamespace * ns);
        MFNode * readWrl(Doc2 * url, VrmlNamespace * ns );
        const MFNode readString(char const * vrmlString, VrmlNamespace * ns);

        const MFNode & getRootNodes() const throw ();

# if 0
        // Destroy world (just passes destroy request up to client)
        void destroyWorld();

        // Replace world with nodes, recording url as the source URL.
        void replaceWorld(MFNode & nodes, VrmlNamespace * ns, Doc2 * url = 0);
# endif

        // A way to let the app know when a world is loaded, changed, etc.
        typedef void (*SceneCB)( int reason );

        // Valid reasons for scene callback (need more...)
        enum {
            DESTROY_WORLD,
            REPLACE_WORLD
        };

        void addWorldChangedCallback( SceneCB );

        // Load a generic file (possibly non-VRML)
        bool loadUrl(const MFString & url, const MFString & parameters = MFString());

# if 0
        // Load a VRML file
        bool load(const std::string & url);

        // Load a VRML string
        bool loadFromString(const char *string);
# endif

        // Save the scene to a file
        bool save(const char *url);

        // URL the current scene was loaded from
        Doc2 * urlDoc() { return d_url; }

        // Types and node names defined in this scope
        VrmlNamespace * getScope() { return this->scope; }

        // Queue an event to load URL/nodes (async so it can be called from a node)
        void queueLoadUrl(const MFString & url, const MFString & parameters );
        void queueReplaceNodes(const MFNode & nodes, VrmlNamespace & ns);

        void sensitiveEvent(Node * object, double timeStamp,
		            bool isOver, bool isActive, double *point );

        // Queue an event for a given node
        void queueEvent(double timeStamp, FieldValue * value,
                        const NodePtr & toNode, const std::string & toEventIn);

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
        //    void bindablePush(NodeType *);
        //    void bindableRemove(NodeType *);

        // Background
        void addBackground(Vrml97Node::Background &);
        void removeBackground(Vrml97Node::Background &);
        Vrml97Node::Background * bindableBackgroundTop();
        void bindablePush(Vrml97Node::Background *);
        void bindableRemove(Vrml97Node::Background *);

        // Fog
        void addFog(Vrml97Node::Fog &);
        void removeFog(Vrml97Node::Fog &);
        Vrml97Node::Fog *bindableFogTop();
        void bindablePush( Vrml97Node::Fog * );
        void bindableRemove( Vrml97Node::Fog * );

        // NavigationInfo
        void addNavigationInfo(Vrml97Node::NavigationInfo &);
        void removeNavigationInfo(Vrml97Node::NavigationInfo &);
        Vrml97Node::NavigationInfo *bindableNavigationInfoTop();
        void bindablePush(Vrml97Node::NavigationInfo *);
        void bindableRemove(Vrml97Node::NavigationInfo *);

        // Viewpoint
        void addViewpoint(Vrml97Node::Viewpoint &);
        void removeViewpoint(Vrml97Node::Viewpoint &);
        Vrml97Node::Viewpoint *bindableViewpointTop();
        void bindablePush(Vrml97Node::Viewpoint *);
        void bindableRemove(Vrml97Node::Viewpoint *);

        // Viewpoint navigation
        void nextViewpoint();
        void prevViewpoint();  
        int nViewpoints();
        void getViewpoint(size_t index, std::string & name, std::string & description);
        void setViewpoint(const std::string & name, const std::string & description);
        void setViewpoint(int);

        // Other (non-bindable) node types that the scene needs access to:

        // Scene-scoped lights
        void addScopedLight(Vrml97Node::AbstractLight &);
        void removeScopedLight(Vrml97Node::AbstractLight &);

        // TimeSensors
        void addTimeSensor(Vrml97Node::TimeSensor &);
        void removeTimeSensor(Vrml97Node::TimeSensor &);

        // AudioClips
        void addAudioClip(Vrml97Node::AudioClip &);
        void removeAudioClip(Vrml97Node::AudioClip &);

        // MovieTextures
        void addMovie(Vrml97Node::MovieTexture &);
        void removeMovie(Vrml97Node::MovieTexture &);
        
        void addProto(ProtoNode & node);
        void removeProto(ProtoNode & node);
        void addScript(ScriptNode &);
        void removeScript(ScriptNode &);

        /**
         * True if the bvolume dirty flag has been set on a node in the
         * scene graph, but has not yet been propegated to that node's
         * ancestors. Set by Node::setBVolumeDirty on any node in this
         * scene graph, cleared by updateFlags()
         */
        bool d_flags_need_updating;

        void updateFlags();

    protected:
        bool headlightOn();
        void doCallbacks(int reason);

        // Allow requests to load urls, nodes to wait until events are processed
        MFString *d_pendingUrl;
        MFString *d_pendingParameters;

        MFNode *d_pendingNodes;
        VrmlNamespace *d_pendingScope;

        // Functions to call when world is changed
        typedef std::list < SceneCB > SceneCBList;
        SceneCBList d_sceneCallbacks;

        // frame rate
        double d_frameRate;

        // Generic bindable children stack operations
        const NodePtr bindableTop(const BindStack & stack);
        void bindablePush(BindStack & stack, const NodePtr & node);
        void bindableRemove(BindStack & stack, const NodePtr & node);

        // An event has a value and a destination, and is associated with a time
        struct Event{
          double timeStamp;
          FieldValue * value;
          NodePtr toNode;
          std::string toEventIn;
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
        size_t d_firstEvent;
        size_t d_lastEvent;
    
    private:
        // Not copyable.
        VrmlScene(const VrmlScene &);
        VrmlScene & operator=(const VrmlScene &);
        
        void initNodeClassMap();
        void initScope();
    };
}

# endif
