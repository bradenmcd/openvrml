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

# ifndef OPENVRML_BROWSER_H
#   define OPENVRML_BROWSER_H

#   include <algorithm>
#   include <list>
#   include <map>
#   include "common.h"
#   include "nodeclassptr.h"
#   include "script.h"
#   include "vrml97node.h"

namespace OpenVRML {

    class Doc2;
    class Viewer;
    class ProtoNode;
    class Scene;
    class Vrml97RootScope;

    class OPENVRML_SCOPE Browser {
        friend class Vrml97Parser;
        friend class ProtoNodeClass;
        friend class Vrml97RootScope;
        
    public:
        enum CBReason {
            DESTROY_WORLD,
            REPLACE_WORLD
        };

        typedef void (*SceneCB)(CBReason reason);

    private:
        typedef std::map<std::string, NodeClassPtr> NodeClassMap;
        NodeClassMap nodeClassMap;
        ScriptNodeClass scriptNodeClass;
        Scene * scene;
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

    protected:
        typedef std::list < SceneCB > SceneCBList;
    
        struct Event {
            double timeStamp;
            FieldValue * value;
            NodePtr toNode;
            std::string toEventIn;
        };

        SceneCBList d_sceneCallbacks;

        double d_frameRate;

        enum { MAXEVENTS = 400 };
        Event d_eventMem[MAXEVENTS];
        size_t d_firstEvent;
        size_t d_lastEvent;
        
    public:
        std::ostream & out;
        std::ostream & err;
        bool d_flags_need_updating;

        Browser(std::ostream & out, std::ostream & err) throw (std::bad_alloc);
        virtual ~Browser() throw ();

        const MFNode & getRootNodes() const throw ();

        virtual const char * getName() const throw ();
        virtual const char * getVersion() const throw ();
        const std::string getWorldURI() const throw (std::bad_alloc);
        void replaceWorld(const MFNode & nodes);
        virtual void loadURI(const MFString & uri, const MFString & parameter)
                throw (std::bad_alloc);
        virtual void setDescription(const std::string & description);
        const MFNode createVrmlFromStream(std::istream & in);
        void createVrmlFromURI(const MFString & uri,
                               const NodePtr & node,
                               const std::string & event);

        void addWorldChangedCallback(SceneCB);
        
        void sensitiveEvent(Node * object, double timeStamp,
		            bool isOver, bool isActive, double *point );

        void queueEvent(double timeStamp, FieldValue * value,
                        const NodePtr & toNode, const std::string & toEventIn);

        bool eventsPending();

        void flushEvents();

        double getFrameRate() const;

        bool update(double currentTime = -1.0);

        void render(Viewer *);

        void setModified();
        void clearModified();
        bool isModified() const;

        void setDelta(double d);
        double getDelta() const;

        void addBackground(Vrml97Node::Background &);
        void removeBackground(Vrml97Node::Background &);
        Vrml97Node::Background * bindableBackgroundTop();
        void bindablePush(Vrml97Node::Background *);
        void bindableRemove(Vrml97Node::Background *);

        void addFog(Vrml97Node::Fog &);
        void removeFog(Vrml97Node::Fog &);
        Vrml97Node::Fog *bindableFogTop();
        void bindablePush( Vrml97Node::Fog * );
        void bindableRemove( Vrml97Node::Fog * );

        void addNavigationInfo(Vrml97Node::NavigationInfo &);
        void removeNavigationInfo(Vrml97Node::NavigationInfo &);
        Vrml97Node::NavigationInfo *bindableNavigationInfoTop();
        void bindablePush(Vrml97Node::NavigationInfo *);
        void bindableRemove(Vrml97Node::NavigationInfo *);

        void addViewpoint(Vrml97Node::Viewpoint &);
        void removeViewpoint(Vrml97Node::Viewpoint &);
        Vrml97Node::Viewpoint *bindableViewpointTop();
        void bindablePush(Vrml97Node::Viewpoint *);
        void bindableRemove(Vrml97Node::Viewpoint *);

        void nextViewpoint();
        void prevViewpoint();  
        size_t nViewpoints();
        void getViewpoint(size_t index,
                          std::string & name, std::string & description);
        void setViewpoint(const std::string & name,
                          const std::string & description);
        void setViewpoint(size_t);

        void addScopedLight(Vrml97Node::AbstractLight &);
        void removeScopedLight(Vrml97Node::AbstractLight &);

        void addTimeSensor(Vrml97Node::TimeSensor &);
        void removeTimeSensor(Vrml97Node::TimeSensor &);

        void addAudioClip(Vrml97Node::AudioClip &);
        void removeAudioClip(Vrml97Node::AudioClip &);

        void addMovie(Vrml97Node::MovieTexture &);
        void removeMovie(Vrml97Node::MovieTexture &);
        
        void addProto(ProtoNode & node);
        void removeProto(ProtoNode & node);
        void addScript(ScriptNode &);
        void removeScript(ScriptNode &);

        void updateFlags();

    protected:
        bool headlightOn();
        void doCallbacks(CBReason reason);

        const NodePtr bindableTop(const BindStack & stack);
        void bindablePush(BindStack & stack, const NodePtr & node);
        void bindableRemove(BindStack & stack, const NodePtr & node);

    private:
        // Not copyable.
        Browser(const Browser &);
        Browser & operator=(const Browser &);
        
        void initNodeClassMap();
    };


    class OPENVRML_SCOPE BadURI : public std::runtime_error {
    public:
        BadURI(const std::string & message);
        virtual ~BadURI();
    };
    
    
    class OPENVRML_SCOPE InvalidURI : public BadURI {
    public:
        InvalidURI();
        virtual ~InvalidURI();
    };
    
    
    class OPENVRML_SCOPE UnreachableURI : public BadURI {
    public:
        UnreachableURI();
        virtual ~UnreachableURI();
    };
    
    
    class OPENVRML_SCOPE Scene {
        MFNode nodes;
        std::string uri;
        
    public:
        Browser & browser;
        Scene * const parent;
    
        Scene(Browser & browser, const MFString & uri, Scene * parent = 0)
                throw (std::bad_alloc);

        const MFNode & getNodes() const throw ();
        const std::string getURI() const throw (std::bad_alloc);
        
        void initialize(double timestamp);
        void render(Viewer & viewer, VrmlRenderContext context);
        
        void loadURI(const MFString & uri, const MFString & parameter)
                throw (std::bad_alloc);
    
    private:
        // Noncopyable.
        Scene(const Scene &);
        Scene & operator=(const Scene &);
    };
    
    inline const MFNode & Scene::getNodes() const throw() {
        return this->nodes;
    }
}

# endif
