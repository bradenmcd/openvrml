//
// OpenVRML
//
// Copyright (C) 1998  Chris Morley
// Copyright (C) 2002  Braden McDaniel
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

# ifndef OPENVRML_NODE_H
#   define OPENVRML_NODE_H

#   include <deque>
#   include <iostream>
#   include <list>
#   include <set>
#   include <stdexcept>
#   include <utility>
#   include <OpenVRML/field.h>
#   include <OpenVRML/fieldvalueptr.h>
#   include <OpenVRML/nodetypeptr.h>
#   include <OpenVRML/scopeptr.h>
#   include <OpenVRML/Viewer.h>
#   include <OpenVRML/VrmlRenderContext.h>

namespace OpenVRML {

    class OPENVRML_SCOPE NodeInterface {
    public:
        enum Type {
            invalidType,
            eventIn,
            eventOut,
            exposedField,
            field
        };

        Type type;
        FieldValue::Type fieldType;
        std::string id;

        NodeInterface(Type type,
                      FieldValue::Type fieldType,
                      const std::string & id);
    };

    std::ostream & OPENVRML_SCOPE operator<<(std::ostream & out,
                                             NodeInterface::Type type);
    std::istream & OPENVRML_SCOPE operator>>(std::istream & in,
                                             NodeInterface::Type & type);

    inline bool operator==(const NodeInterface & lhs, const NodeInterface & rhs)
        throw ()
    {
        return lhs.type == rhs.type
                && lhs.fieldType == rhs.fieldType
                && lhs.id == rhs.id;
    }

    inline bool operator!=(const NodeInterface & lhs, const NodeInterface & rhs)
        throw ()
    {
        return !(lhs == rhs);
    }

    class NodeType;

    class OPENVRML_SCOPE UnsupportedInterface : public std::runtime_error {
    public:
        explicit UnsupportedInterface(const std::string & message);
        UnsupportedInterface(const NodeType & nodeType,
                             const std::string & interfaceId);
        UnsupportedInterface(const NodeType & nodeType,
                             NodeInterface::Type interfaceType,
                             const std::string & interfaceId);
        virtual ~UnsupportedInterface() throw ();
    };


    class OPENVRML_SCOPE NodeInterfaceSet {
        struct IdLess :
                std::binary_function<NodeInterface, NodeInterface, bool> {
            bool operator()(const NodeInterface & lhs,
                            const NodeInterface & rhs) const
            {
                return lhs.id < rhs.id;
            }
        };

        std::set<NodeInterface, IdLess> nodeInterfaceSet;

    public:
        typedef std::set<NodeInterface, IdLess>::const_iterator const_iterator;

        void add(const NodeInterface & nodeInterface)
                throw (std::invalid_argument, std::bad_alloc);
        const_iterator begin() const throw ();
        const_iterator end() const throw ();
        const_iterator findInterface(const std::string & id) const throw ();
    };

    inline NodeInterfaceSet::const_iterator NodeInterfaceSet::begin() const
        throw ()
    {
        return this->nodeInterfaceSet.begin();
    }

    inline NodeInterfaceSet::const_iterator NodeInterfaceSet::end() const
        throw ()
    {
        return this->nodeInterfaceSet.end();
    }


    class Browser;
    class Viewer;
    class ViewpointNode;

    class OPENVRML_SCOPE NodeClass {
    public:
        Browser & browser;

        virtual ~NodeClass() throw () = 0;
        virtual void initialize(ViewpointNode * initialViewpoint,
                                double time) throw ();
        virtual void render(Viewer & viewer) throw ();
        virtual const NodeTypePtr createType(const std::string & id,
                                             const NodeInterfaceSet & interfaces)
            throw (UnsupportedInterface, std::bad_alloc) = 0;

    protected:
        explicit NodeClass(Browser & browser) throw ();
    };

    class OPENVRML_SCOPE NodeType {
    public:
        NodeClass & nodeClass;
        const std::string id;

        virtual ~NodeType() throw () = 0;

        FieldValue::Type hasEventIn(const std::string & id) const throw ();
        FieldValue::Type hasEventOut(const std::string & id) const throw ();
        FieldValue::Type hasField(const std::string & id) const throw ();
        FieldValue::Type hasExposedField(const std::string & id) const throw ();

        virtual const NodeInterfaceSet & getInterfaces() const throw () = 0;
        virtual const NodePtr createNode(const ScopePtr & scope) const
            throw (std::bad_alloc) = 0;

    protected:
        NodeType(NodeClass & nodeClass, const std::string & id)
            throw (std::bad_alloc);
    };


    class OPENVRML_SCOPE FieldValueTypeMismatch : public std::runtime_error {
    public:
        FieldValueTypeMismatch();
        virtual ~FieldValueTypeMismatch() throw ();
    };


    typedef std::deque<Node *> NodePath;


    class Scope;
    class BVolume;
    class ScriptNode;
    class AppearanceNode;
    class ChildNode;
    class ColorNode;
    class CoordinateNode;
    class FontStyleNode;
    class GeometryNode;
    class GroupingNode;
    class MaterialNode;
    class NormalNode;
    class SoundSourceNode;
    class TextureNode;
    class TextureCoordinateNode;
    class TextureTransformNode;
    class TransformNode;
    class ViewpointNode;

    namespace Vrml97Node {
        class Anchor;
        class AudioClip;
        class CylinderSensor;
        class AbstractLight;
        class MovieTexture;
        class NavigationInfo;
        class PlaneSensor;
        class PointLight;
        class SphereSensor;
        class SpotLight;
        class TimeSensor;
        class TouchSensor;
    }

    class Scene;

    std::ostream & OPENVRML_SCOPE operator<<(std::ostream & out,
                                             const Node & node);

    class OPENVRML_SCOPE Node {
        friend std::ostream & operator<<(std::ostream & out, const Node & node);

    public:
        class Route {
        public:
            const std::string fromEventOut;
            const NodePtr toNode;
            const std::string toEventIn;

            Route(const std::string & fromEventOut, const NodePtr & toNode,
                  const std::string & toEventIn);
            Route(const Route & route);
        };

        typedef std::list<Route> RouteList;

        struct PolledEventOutValue {
            const FieldValuePtr value;
            bool modified;

            PolledEventOutValue();
            PolledEventOutValue(const FieldValuePtr & value, bool modified);
        };

    private:
        ScopePtr scope;
        Scene * scene;
        RouteList routes;

        typedef std::map<std::string, PolledEventOutValue *> EventOutISMap;
        EventOutISMap eventOutISMap;

    public:
        const NodeType & nodeType;

        virtual ~Node() throw () = 0;

        const std::string getId() const;
        void setId(const std::string & nodeId);

        const ScopePtr & getScope() const throw ();

        Scene * getScene() const throw ();

        std::ostream & print(std::ostream & out, size_t indent) const;

        void addEventOutIS(const std::string & eventOut,
                           PolledEventOutValue & eventOutValue)
            throw (UnsupportedInterface, std::bad_alloc);

        void initialize(Scene & scene, double timestamp) throw (std::bad_alloc);
        void relocate() throw (std::bad_alloc);

        void setField(const std::string & id, const FieldValue & value)
            throw (UnsupportedInterface, std::bad_cast, std::bad_alloc);
        const FieldValue & getField(const std::string & id) const
            throw (UnsupportedInterface);
        void processEvent(const std::string & id, const FieldValue & value,
                          double timestamp)
            throw (UnsupportedInterface, std::bad_cast, std::bad_alloc);
        const FieldValue & getEventOut(const std::string & id) const
            throw (UnsupportedInterface);

        void shutdown(double timestamp) throw ();

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
        virtual const GroupingNode * toGrouping() const throw ();
        virtual GroupingNode * toGrouping() throw ();
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

        // Indicate that the node state has changed, need to re-render
        void setModified();
        void clearModified() { d_modified = false; }
        virtual bool isModified() const;

        static void markPathModified(NodePath& path, bool mod, int flags = 0x003);

        // do the work of updatemodified. move this to be protected
        //
        virtual void updateModified(NodePath& path, int flags = 0x003);

        void updateModified(int flags = 0x003);

        virtual const BVolume * getBVolume() const;

        virtual void setBVolume(const BVolume & v);

        virtual void setBVolumeDirty(bool f);

        virtual bool isBVolumeDirty() const;

        // Add a ROUTE from a field in this node
        void addRoute(const std::string & fromEventOut,
                      const NodePtr & toNode, const std::string & toEventIn)
            throw (UnsupportedInterface, FieldValueTypeMismatch);

        // Delete a ROUTE from a field in this node
        void deleteRoute(const std::string & fromEventOut,
                         const NodePtr & toNode, const std::string & toEventIn)
            throw ();

        const RouteList & getRoutes() const;

        virtual void render(Viewer & viewer, VrmlRenderContext context);

    protected:
        Node(const NodeType & nodeType, const ScopePtr & scope);

        // Send a named event from this node.
        void emitEvent(const std::string & id, const FieldValue & fieldValue,
                       double timestamp)
            throw (std::bad_cast, std::bad_alloc);

        bool d_modified;
        bool d_bvol_dirty;

    private:
        // Not copyable.
        Node(const Node &);
        Node & operator=(const Node &);

        virtual void do_initialize(double timestamp) throw (std::bad_alloc);
        virtual void do_relocate() throw (std::bad_alloc);

        virtual void do_setField(const std::string & id,
                                 const FieldValue & value)
            throw (UnsupportedInterface, std::bad_cast, std::bad_alloc) = 0;
        virtual const FieldValue & do_getField(const std::string & id) const
            throw (UnsupportedInterface) = 0;
        virtual void do_processEvent(const std::string & id,
                                     const FieldValue & value,
                                     double timestamp)
            throw (UnsupportedInterface, std::bad_cast, std::bad_alloc) = 0;
        virtual const FieldValue & do_getEventOut(const std::string & id) const
            throw (UnsupportedInterface) = 0;
        virtual void do_shutdown(double timestamp) throw ();
    };

    inline const ScopePtr & Node::getScope() const throw ()
    {
        return this->scope;
    }

    inline Scene * Node::getScene() const throw ()
    {
        return this->scene;
    }

    inline bool operator==(const Node::Route & lhs, const Node::Route & rhs)
        throw ()
    {
        return lhs.fromEventOut == rhs.fromEventOut
            && lhs.toNode == rhs.toNode
            && lhs.toEventIn == rhs.toEventIn;
    }

    inline bool operator!=(const Node::Route & lhs, const Node::Route & rhs)
        throw ()
    {
        return !(lhs == rhs);
    }


    class OPENVRML_SCOPE AppearanceNode : public virtual Node {
    public:
        virtual ~AppearanceNode() throw () = 0;

        virtual const AppearanceNode * toAppearance() const throw ();
        virtual AppearanceNode * toAppearance() throw ();

        virtual const NodePtr & getMaterial() const throw () = 0;
        virtual const NodePtr & getTexture() const throw () = 0;
        virtual const NodePtr & getTextureTransform() const throw () = 0;

    protected:
        AppearanceNode(const NodeType & nodeType, const ScopePtr & scope);
    };


    class OPENVRML_SCOPE ChildNode : public virtual Node {
    public:
        virtual ~ChildNode() throw () = 0;

        virtual const ChildNode * toChild() const throw ();
        virtual ChildNode * toChild() throw ();

    protected:
        ChildNode(const NodeType & nodeType, const ScopePtr & scope);
    };


    class OPENVRML_SCOPE ColorNode : public virtual Node {
    public:
        virtual ~ColorNode() throw () = 0;

        virtual const ColorNode * toColor() const throw ();
        virtual ColorNode * toColor() throw ();

        virtual const std::vector<color> & getColor() const throw () = 0;

    protected:
        ColorNode(const NodeType & nodeType, const ScopePtr & scope);
    };


    class OPENVRML_SCOPE CoordinateNode : public virtual Node {
    public:
        virtual ~CoordinateNode() throw () = 0;

        virtual const CoordinateNode * toCoordinate() const throw ();
        virtual CoordinateNode * toCoordinate() throw ();

        virtual const std::vector<vec3f> & getPoint() const throw () = 0;

    protected:
        CoordinateNode(const NodeType & nodeType, const ScopePtr & scope);
    };


    class OPENVRML_SCOPE FontStyleNode : public virtual Node {
    public:
        virtual ~FontStyleNode() throw () = 0;

        virtual const FontStyleNode * toFontStyle() const throw ();
        virtual FontStyleNode * toFontStyle() throw ();

        virtual const std::vector<std::string> & getFamily() const throw () = 0;
        virtual bool getHorizontal() const throw () = 0;
        virtual const std::vector<std::string> & getJustify() const throw () = 0;
        virtual const std::string & getLanguage() const throw () = 0;
        virtual bool getLeftToRight() const throw () = 0;
        virtual float getSize() const throw () = 0;
        virtual float getSpacing() const throw () = 0;
        virtual const std::string & getStyle() const throw () = 0;
        virtual bool getTopToBottom() const throw () = 0;

    protected:
        FontStyleNode(const NodeType & nodeType, const ScopePtr & scope);
    };


    class OPENVRML_SCOPE GeometryNode : public virtual Node {
    public:
        virtual ~GeometryNode() throw () = 0;

        virtual const GeometryNode * toGeometry() const throw ();
        virtual GeometryNode * toGeometry() throw ();

        virtual Viewer::Object insertGeometry(Viewer & viewer,
                                              VrmlRenderContext context) = 0;
        virtual const ColorNode * getColor() const throw ();

    protected:
        GeometryNode(const NodeType & nodeType, const ScopePtr & scope);
    };


    class OPENVRML_SCOPE GroupingNode : public virtual ChildNode {
    public:
        virtual ~GroupingNode() throw () = 0;

        virtual const GroupingNode * toGrouping() const throw ();
        virtual GroupingNode * toGrouping() throw ();

        virtual const std::vector<NodePtr> & getChildren() const throw () = 0;
        virtual void activate(double timestamp, bool over, bool active,
                              double *p) = 0;

    protected:
        GroupingNode(const NodeType & nodeType, const ScopePtr & scope);
    };


    class OPENVRML_SCOPE MaterialNode : public virtual Node {
    public:
        virtual ~MaterialNode() throw () = 0;

        virtual const MaterialNode * toMaterial() const throw ();
        virtual MaterialNode * toMaterial() throw ();

        virtual float getAmbientIntensity() const throw () = 0;
        virtual const color & getDiffuseColor() const throw () = 0;
        virtual const color & getEmissiveColor() const throw () = 0;
        virtual float getShininess() const throw () = 0;
        virtual const color & getSpecularColor() const throw () = 0;
        virtual float getTransparency() const throw () = 0;

    protected:
        MaterialNode(const NodeType & nodeType, const ScopePtr & scope);
    };


    class OPENVRML_SCOPE NormalNode : public virtual Node {
    public:
        virtual ~NormalNode() throw () = 0;

        virtual const NormalNode * toNormal() const throw ();
        virtual NormalNode * toNormal() throw ();

        virtual const std::vector<vec3f> & getVector() const throw () = 0;

    protected:
        NormalNode(const NodeType & nodeType, const ScopePtr & scope);
    };


    class OPENVRML_SCOPE SoundSourceNode : public virtual Node {
    public:
        virtual ~SoundSourceNode() throw () = 0;
        virtual const SoundSourceNode * toSoundSource() const throw ();
        virtual SoundSourceNode * toSoundSource() throw ();

    protected:
        SoundSourceNode(const NodeType & nodeType, const ScopePtr & scope);
    };


    class OPENVRML_SCOPE TextureNode : public virtual Node {
    public:
        virtual ~TextureNode() throw () = 0;

        virtual const TextureNode * toTexture() const throw ();
        virtual TextureNode * toTexture() throw ();

        virtual size_t nComponents() const throw () = 0;
        virtual size_t width() const throw () = 0;
        virtual size_t height() const throw () = 0;
        virtual size_t nFrames() const throw () = 0;
        virtual const unsigned char * pixels() const throw () = 0;
        virtual bool getRepeatS() const throw () = 0;
        virtual bool getRepeatT() const throw () = 0;

    protected:
        TextureNode(const NodeType & nodeType, const ScopePtr & scope);
    };


    class OPENVRML_SCOPE TextureCoordinateNode : public virtual Node {
    public:
        virtual ~TextureCoordinateNode() throw () = 0;

        virtual const TextureCoordinateNode * toTextureCoordinate() const
            throw ();
        virtual TextureCoordinateNode * toTextureCoordinate() throw ();

        virtual const std::vector<vec2f> & getPoint() const throw () = 0;

    protected:
        TextureCoordinateNode(const NodeType & nodeType,
                              const ScopePtr & scope);
    };


    class OPENVRML_SCOPE TextureTransformNode : public virtual Node {
    public:
        virtual ~TextureTransformNode() throw () = 0;

        virtual const TextureTransformNode * toTextureTransform() const throw ();
        virtual TextureTransformNode * toTextureTransform() throw ();

    protected:
        TextureTransformNode(const NodeType & nodeType, const ScopePtr & scope);
    };


    class OPENVRML_SCOPE TransformNode : public virtual GroupingNode {
    public:
        virtual ~TransformNode() throw () = 0;

        virtual const TransformNode * toTransform() const throw ();
        virtual TransformNode * toTransform() throw ();

        virtual const mat4f & getTransform() const throw () = 0;

    protected:
        TransformNode(const NodeType & nodeType, const ScopePtr & scope);
    };


    class OPENVRML_SCOPE ViewpointNode : public virtual ChildNode {
    public:
        virtual ~ViewpointNode() throw () = 0;

        virtual const ViewpointNode * toViewpoint() const throw ();
        virtual ViewpointNode * toViewpoint() throw ();

        virtual const mat4f & getTransformation() const throw () = 0;
        virtual const mat4f & getUserViewTransform() const throw () = 0;
        virtual void setUserViewTransform(const mat4f & transform)
            throw () = 0;
        virtual const std::string & getDescription() const throw () = 0;
        virtual float getFieldOfView() const throw () = 0;

    protected:
        ViewpointNode(const NodeType & nodeType, const ScopePtr & scope);
    };


    class OPENVRML_SCOPE NodeTraverser {
        std::set<Node *> traversedNodes;
        bool halt;

    public:
        NodeTraverser() throw (std::bad_alloc);
        virtual ~NodeTraverser() throw () = 0;

        void traverse(Node & node);
        void traverse(const NodePtr & node);
        void traverse(const std::vector<NodePtr> & nodes);

    protected:
        void haltTraversal() throw ();

    private:
        // Noncopyable.
        NodeTraverser(const NodeTraverser &);
        NodeTraverser & operator=(const NodeTraverser &);

        virtual void onEntering(Node & node);
        virtual void onLeaving(Node & node);

        void do_traversal(Node & node);
    };
}

# endif
