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

# ifndef OPENVRML_BROWSER_H
#   define OPENVRML_BROWSER_H

#   include <stddef.h>
#   include <list>
#   include <map>
#   include <OpenVRML/common.h>
#   include <OpenVRML/node_class_ptr.h>
#   include <OpenVRML/script.h>

namespace OpenVRML {

    class OPENVRML_SCOPE InvalidVrml : public std::runtime_error {
    public:
        InvalidVrml();
        virtual ~InvalidVrml() throw ();
    };

    class Doc2;
    class Viewer;
    class ProtoNode;
    class Scene;
    class Vrml97RootScope;
    class NullNodeClass;
    class NullNodeType;

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
        std::auto_ptr<NullNodeClass> nullNodeClass;
        std::auto_ptr<NullNodeType> nullNodeType;
        typedef std::map<std::string, node_class_ptr> NodeClassMap;
        NodeClassMap nodeClassMap;
        ScriptNodeClass scriptNodeClass;
        Scene * scene;
        NodePtr defaultViewpoint;
        viewpoint_node * activeViewpoint;
        std::list<viewpoint_node *> viewpointList;
        typedef std::list<NodePtr> BindStack;
        BindStack d_navigationInfoStack;
        std::list<node *> d_navigationInfos;
        std::list<node *> d_scopedLights;
        std::list<ScriptNode *> d_scripts;
        std::list<node *> d_timers;
        std::list<node *> d_audioClips;
        std::list<node *> d_movies;
        std::list<ProtoNode *> protoNodeList;
        bool d_modified;
        bool d_newView;
        double d_deltaTime;

    protected:
        typedef std::list < SceneCB > SceneCBList;

        struct Event {
            double timeStamp;
            field_value * value;
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
        static double getCurrentTime() throw ();

        std::ostream & out;
        std::ostream & err;
        bool d_flags_need_updating;

        Browser(std::ostream & out, std::ostream & err) throw (std::bad_alloc);
        virtual ~Browser() throw ();

        const std::vector<NodePtr> & getRootNodes() const throw ();
        const node_path findNode(const node & node) const
            throw (std::bad_alloc);
        viewpoint_node & getActiveViewpoint() const throw ();
        void setActiveViewpoint(viewpoint_node & viewpoint) throw ();
        void resetDefaultViewpoint() throw ();
        void addViewpoint(viewpoint_node & viewpoint) throw (std::bad_alloc);
        void removeViewpoint(viewpoint_node & viewpoint) throw ();
        const std::list<viewpoint_node *> & getViewpoints() const throw ();

        virtual const char * getName() const throw ();
        virtual const char * getVersion() const throw ();
        float getCurrentSpeed();
        const std::string getWorldURI() const throw (std::bad_alloc);
        void replaceWorld(const std::vector<NodePtr> & nodes);
        virtual void loadURI(const std::vector<std::string> & uri,
                             const std::vector<std::string> & parameter)
            throw (std::bad_alloc);
        virtual void setDescription(const std::string & description);
        const std::vector<NodePtr> createVrmlFromStream(std::istream & in);
        void createVrmlFromURI(const std::vector<std::string> & uri,
                               const NodePtr & node,
                               const std::string & event);

        void addWorldChangedCallback(SceneCB);

        void sensitiveEvent(node * object, double timeStamp,
		            bool isOver, bool isActive, double *point );

        void queueEvent(double timeStamp, field_value * value,
                        const NodePtr & toNode, const std::string & toEventIn);

        bool eventsPending();

        void flushEvents();

        double getFrameRate() const;

        bool update(double currentTime = -1.0);

        void render(Viewer & viewer);

        void setModified();
        void clearModified();
        bool isModified() const;

        void setDelta(double d);
        double getDelta() const;

        void addNavigationInfo(Vrml97Node::NavigationInfo &);
        void removeNavigationInfo(Vrml97Node::NavigationInfo &);
        Vrml97Node::NavigationInfo *bindableNavigationInfoTop();
        void bindablePush(Vrml97Node::NavigationInfo *);
        void bindableRemove(Vrml97Node::NavigationInfo *);

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
        virtual ~BadURI() throw ();
    };


    class OPENVRML_SCOPE InvalidURI : public BadURI {
    public:
        InvalidURI();
        virtual ~InvalidURI() throw ();
    };


    class OPENVRML_SCOPE UnreachableURI : public BadURI {
    public:
        UnreachableURI();
        virtual ~UnreachableURI() throw ();
    };


    class OPENVRML_SCOPE Scene {
        std::vector<NodePtr> nodes;
        std::string uri;

    public:
        Browser & browser;
        Scene * const parent;

        Scene(Browser & browser, const std::vector<std::string> & uri,
              Scene * parent = 0)
            throw (InvalidVrml, std::bad_alloc);

        void initialize(double timestamp) throw (std::bad_alloc);
        const std::vector<NodePtr> & getNodes() const throw ();
        const std::string getURI() const throw (std::bad_alloc);
        void render(Viewer & viewer, VrmlRenderContext context);
        void loadURI(const std::vector<std::string> & uri,
                     const std::vector<std::string> & parameter)
                throw (std::bad_alloc);
        void shutdown(double timestamp) throw ();

    private:
        // Noncopyable.
        Scene(const Scene &);
        Scene & operator=(const Scene &);
    };

    inline const std::vector<NodePtr> & Scene::getNodes() const throw()
    {
        return this->nodes;
    }
}

# endif
