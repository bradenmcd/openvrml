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

#   include <iostream>
#   include <list>
#   include <set>
#   include <stdexcept>
#   include <string>
#   include <typeinfo>
#   include <vector>
#   include "common.h"
#   include "field.h"
#   include "fieldvalueptr.h"
#   include "nodetypeptr.h"
#   include "scopeptr.h"
#   include "VrmlRenderContext.h"

namespace OpenVRML {

    class UnsupportedInterface : public std::invalid_argument {
    public:
        UnsupportedInterface(const std::string & message);
        virtual ~UnsupportedInterface() throw ();
    };
    
    
    struct OPENVRML_SCOPE NodeInterface {
        enum Type { invalidType, eventIn, eventOut, exposedField, field };

        Type type;
        FieldValue::Type fieldType;
        std::string id;
        
        NodeInterface(Type type, FieldValue::Type fieldType,
                      const std::string & id);
    };
    
    bool operator==(const NodeInterface & rhs, const NodeInterface & lhs);
    
    inline bool operator!=(const NodeInterface & rhs,
                           const NodeInterface & lhs) {
        return !(rhs == lhs);
    }
    
    
    class OPENVRML_SCOPE NodeInterfaceSet {
        struct IdLess :
                std::binary_function<NodeInterface, NodeInterface, bool> {
            bool operator()(const NodeInterface & lhs,
                            const NodeInterface & rhs) const {
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
    };
    
    inline NodeInterfaceSet::const_iterator
            NodeInterfaceSet::begin() const throw () {
        return this->nodeInterfaceSet.begin();
    }
    
    inline NodeInterfaceSet::const_iterator
            NodeInterfaceSet::end() const throw () {
        return this->nodeInterfaceSet.end();
    }
    
    
    class Browser;
    
    class OPENVRML_SCOPE NodeClass {
    public:
        Browser & browser;
        
        virtual ~NodeClass() throw () = 0;
        
        virtual const NodeTypePtr
                createType(const std::string & id,
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
        FieldValue::Type hasInterface(const std::string & id) const throw ();
        
        virtual const NodeInterfaceSet & getInterfaces() const throw () = 0;
        virtual const NodePtr createNode(const ScopePtr & scope) const
                throw (std::bad_alloc) = 0;
    
    protected:
        NodeType(NodeClass & nodeClass, const std::string & id)
            throw (std::bad_alloc);
    };


    class Scope;
    class Viewer;
    class NodeType;
    class FieldValue;
    class VrmlMatrix;
    class NodeVisitor;
    class BVolume;
    class ScriptNode;
    class AppearanceNode;
    class ChildNode;
    class ColorNode;
    class CoordinateNode;
    class FontStyleNode;
    class GeometryNode;
    class MaterialNode;
    class NormalNode;
    class SoundSourceNode;
    class TextureNode;
    class TextureCoordinateNode;
    class TextureTransformNode;
    
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

    class Scene;
    
    std::ostream & operator<<(std::ostream & out, const Node & node);

    class OPENVRML_SCOPE Node {
        friend std::ostream & operator<<(std::ostream & out, const Node & node);
    
    public:
        struct Route {
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
        
        const ScopePtr & getScope() const;
        
        Scene * getScene() const;
        
        std::ostream & print(std::ostream & out, size_t indent) const;

        bool accept(NodeVisitor & visitor);
        void resetVisitedFlag();
        
        const MFNode getChildren() const;

        void addEventOutIS(const std::string & eventOut,
                           PolledEventOutValue * eventOutValue)
                throw (UnsupportedInterface, std::bad_alloc);

        void initialize(Scene & scene, double timestamp) throw ();
        
        void setField(const std::string & id, const FieldValue & value)
                throw (UnsupportedInterface, std::bad_cast, std::bad_alloc);
        const FieldValue & getField(const std::string & id) const
                throw (UnsupportedInterface);
        void processEvent(const std::string & id, const FieldValue & value,
                          double timestamp)
                throw (UnsupportedInterface, std::bad_cast, std::bad_alloc);
        const FieldValue & getEventOut(const std::string & id) const
                throw (UnsupportedInterface);
        
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

        // Indicate that the node state has changed, need to re-render
        void setModified();
        void clearModified() { d_modified = false; }
        virtual bool isModified() const;
        typedef std::list< Node* > NodePath;


        static void markPathModified(NodePath& path, bool mod, int flags = 0x003);

        // do the work of updatemodified. move this to be protected
        //
        virtual void updateModified(NodePath& path, int flags = 0x003);

        void updateModified(int flags = 0x003);

        virtual const BVolume * getBVolume() const;

        virtual void setBVolume(const BVolume & v);

        virtual void setBVolumeDirty(bool f);

        virtual bool isBVolumeDirty() const;


        // A generic flag (typically used to find USEd nodes).
        void setFlag() { d_flag = true; }
        virtual void clearFlags();	// Clear childrens flags too.
        bool isFlagSet() { return d_flag; }

        // Add a ROUTE from a field in this node
        void addRoute(const std::string & fromEventOut, const NodePtr & toNode, const std::string & toEventIn);

        // Delete a ROUTE from a field in this node
        void deleteRoute(const std::string & fromEventOut, const NodePtr & toNode, const std::string & toEventIn);

        const RouteList & getRoutes() const;

        virtual void render(Viewer *, VrmlRenderContext rc);


        virtual void accumulateTransform(Node*);

        virtual Node* getParentTransform();

        virtual void inverseTransform(Viewer *);

        virtual void inverseTransform(VrmlMatrix &);

    protected:
        Node(const NodeType & nodeType, const ScopePtr & scope);

        // Send a named event from this node.
        void emitEvent(const std::string & id, const FieldValue & fieldValue,
                       double timestamp)
                throw (std::bad_cast, std::bad_alloc);

        // True if a field changed since last render
        bool d_modified;
        bool d_flag;
        bool d_bvol_dirty;
        bool visited;
    
    private:
        // Not copyable.
        Node(const Node &);
        Node & operator=(const Node &);
        
        virtual void initializeImpl(double timestamp) throw ();
        
        virtual void setFieldImpl(const std::string & id,
                                  const FieldValue & value)
                throw (UnsupportedInterface, std::bad_cast, std::bad_alloc) = 0;
        
        virtual const FieldValue & getFieldImpl(const std::string & id) const
                throw (UnsupportedInterface) = 0;
        
        virtual void processEventImpl(const std::string & id,
                                      const FieldValue & value,
                                      double timestamp)
                throw (UnsupportedInterface, std::bad_cast, std::bad_alloc) = 0;
        
        virtual const FieldValue & getEventOutImpl(const std::string & id) const
                throw (UnsupportedInterface) = 0;
    };

    inline const ScopePtr & Node::getScope() const { return this->scope; }
    
    inline Scene * Node::getScene() const { return this->scene; }
    
    inline bool operator==(const Node::Route & lhs, const Node::Route & rhs) {
        return lhs.fromEventOut == rhs.fromEventOut
            && lhs.toNode == rhs.toNode
            && lhs.toEventIn == rhs.toEventIn;
    }
    
    inline bool operator!=(const Node::Route & lhs, const Node::Route & rhs) {
        return !(lhs == rhs);
    }
    
    
    class OPENVRML_SCOPE AppearanceNode : public virtual Node {
    public:
        virtual ~AppearanceNode() throw () = 0;
    
        virtual const AppearanceNode * toAppearance() const throw ();
        virtual AppearanceNode * toAppearance() throw ();
        
        virtual const SFNode & getMaterial() const throw () = 0;
        virtual const SFNode & getTexture() const throw () = 0;
        virtual const SFNode & getTextureTransform() const throw () = 0;
    
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
        
        virtual const MFColor & getColor() const throw () = 0;
    
    protected:
        ColorNode(const NodeType & nodeType, const ScopePtr & scope);
    };
    
    
    class OPENVRML_SCOPE CoordinateNode : public virtual Node {
    public:
        virtual ~CoordinateNode() throw () = 0;
        
        virtual const CoordinateNode * toCoordinate() const throw ();
        virtual CoordinateNode * toCoordinate() throw ();
        
        virtual const MFVec3f & getPoint() const throw () = 0;
    
    protected:
        CoordinateNode(const NodeType & nodeType, const ScopePtr & scope);
    };
    
    
    class OPENVRML_SCOPE FontStyleNode : public virtual Node {
    public:
        virtual ~FontStyleNode() throw () = 0;
        
        virtual const FontStyleNode * toFontStyle() const throw ();
        virtual FontStyleNode * toFontStyle() throw ();
    
        virtual const MFString & getFamily() const throw () = 0;
        virtual const SFBool & getHorizontal() const throw () = 0;
        virtual const MFString & getJustify() const throw () = 0;
        virtual const SFString & getLanguage() const throw () = 0;
        virtual const SFBool & getLeftToRight() const throw () = 0;
        virtual const SFFloat & getSize() const throw () = 0;
        virtual const SFFloat & getSpacing() const throw () = 0;
        virtual const SFString & getStyle() const throw () = 0;
        virtual const SFBool & getTopToBottom() const throw () = 0;
        
    protected:
        FontStyleNode(const NodeType & nodeType, const ScopePtr & scope);
    };
    
    
    class OPENVRML_SCOPE GeometryNode : public virtual Node {
    public:
        virtual ~GeometryNode() throw () = 0;
        
        virtual const GeometryNode * toGeometry() const throw ();
        virtual GeometryNode * toGeometry() throw ();
        
        virtual const ColorNode * getColor() const throw ();
    
    protected:
        GeometryNode(const NodeType & nodeType, const ScopePtr & scope);
    };
    
    
    class OPENVRML_SCOPE MaterialNode : public virtual Node {
    public:
        virtual ~MaterialNode() throw () = 0;
    
        virtual const MaterialNode * toMaterial() const throw ();
        virtual MaterialNode * toMaterial() throw ();
        
        virtual const SFFloat & getAmbientIntensity() const throw () = 0;
        virtual const SFColor & getDiffuseColor() const throw () = 0;
        virtual const SFColor & getEmissiveColor() const throw () = 0;
        virtual const SFFloat & getShininess() const throw () = 0;
        virtual const SFColor & getSpecularColor() const throw () = 0;
        virtual const SFFloat & getTransparency() const throw () = 0;
    
    protected:
        MaterialNode(const NodeType & nodeType, const ScopePtr & scope);
    };
    
    
    class OPENVRML_SCOPE NormalNode : public virtual Node {
    public:
        virtual ~NormalNode() throw () = 0;
        
        virtual const NormalNode * toNormal() const throw ();
        virtual NormalNode * toNormal() throw ();
        
        virtual const MFVec3f & getVector() const throw () = 0;
    
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
        virtual const SFBool & getRepeatS() const throw () = 0;
        virtual const SFBool & getRepeatT() const throw () = 0;
    
    protected:
        TextureNode(const NodeType & nodeType, const ScopePtr & scope);
    };
    
    
    class OPENVRML_SCOPE TextureCoordinateNode : public virtual Node {
    public:
        virtual ~TextureCoordinateNode() throw () = 0;
        
        virtual const TextureCoordinateNode * toTextureCoordinate() const
                throw ();
        virtual TextureCoordinateNode * toTextureCoordinate() throw ();
        
        virtual const MFVec2f & getPoint() const throw () = 0;
    
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
    
    
    class OPENVRML_SCOPE NodeVisitor {
    public:
        virtual ~NodeVisitor() = 0;
    
        virtual void visit(Node & node) = 0;
    };
}

# endif

