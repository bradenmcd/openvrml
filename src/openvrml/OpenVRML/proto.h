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

# ifndef OPENVRML_PROTO_H
#   define OPENVRML_PROTO_H

#   include <stack>
#   include "common.h"
#   include "node.h"
#   include "fieldvalueptr.h"
#   include "Viewer.h"
#   include "VrmlNamespace.h"

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

    class OPENVRML_SCOPE ProtoNode : public Node {
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
    
    
    class OPENVRML_SCOPE ProtoNodeClass : public NodeClass {
        
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

# endif
