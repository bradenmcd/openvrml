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

# ifndef OPENVRML_NODE_H
#   define OPENVRML_NODE_H

#   include <string.h>
#   include <iostream.h>
#   include <list>
#   include <string>
#   include <vector>
#   include <iostream.h>
#   include "common.h"
#   include "field.h"
#   include "System.h" // error
#   include "VrmlRenderContext.h"

namespace OpenVRML {

    class Doc2;
    class VrmlNamespace;
    class VrmlScene;

    ostream & operator<<(ostream & os, const Node & f);

    class OPENVRML_SCOPE NodeType {
    public:
        struct NodeFieldRec {
            NodePtr node;
            std::string fieldName;
        };
        typedef std::list<NodeFieldRec *> ISMap;
        struct ProtoField {
            std::string name;
            FieldValue::Type type;
            FieldValue * defaultValue;
            ISMap thisIS;
        };
        typedef std::list<ProtoField *> FieldList;

    private:
        std::string id;
        VrmlNamespace * d_namespace;
        const MFString * d_url; // Where the EXTERNPROTO could be.
        mutable std::string actualUrl;	// The URL actually used.
        Doc2 *d_relative;
        mutable MFNode implNodes;
        // Pointer to function to create instances
        const NodePtr (*d_creator)(VrmlScene *const);	
        // Fields defined for this node type
        mutable FieldList d_eventIns;
        mutable FieldList d_eventOuts;
        mutable FieldList d_fields;
        mutable bool d_fieldsInitialized;

    public:
        NodeType(const std::string & id,
	         const NodePtr (*creator)(VrmlScene * const scene) = 0);
        ~NodeType();

        const NodePtr newNode(VrmlScene * scene = 0) const;

        const std::string & getId() const;
        VrmlNamespace * getScope() const;
        void setScope(VrmlNamespace & scope);

        // Routines for adding/getting eventIns/Outs/fields to this type
        void addEventIn(const std::string & name, FieldValue::Type type);
        void addEventOut(const std::string & name, FieldValue::Type type);
        void addField(const std::string & name, FieldValue::Type type,
		      const FieldValue * defaultVal = 0);
        void addExposedField(const std::string & name,
                             FieldValue::Type type,
		             const FieldValue * defaultVal = 0);

        const FieldValue * fieldDefault(const std::string & name) const;
        void setFieldDefault(const std::string & name, const FieldValue * value);

        FieldValue::Type hasEventIn(const std::string & id) const;
        FieldValue::Type hasEventOut(const std::string & id) const;
        FieldValue::Type hasField(const std::string & id) const;
        FieldValue::Type hasExposedField(const std::string & id) const;
        FieldValue::Type hasInterface(const std::string & id) const;

        void setUrl(const MFString & url, const Doc2 * relative = 0);
        void setActualUrl(const std::string & url);
        const std::string & getActualUrl() const;

        // Add a node to a PROTO implementation
        void addNode(Node & implNode);

        // Add an IS linkage to one of the PROTO interface fields/events.
        void addIS(const std::string & isFieldName, Node & implNode,
	           const std::string & implFieldName);


        const MFNode & getImplementationNodes() const;

        const NodePtr firstNode() const;

        const ISMap * getFieldISMap(const std::string & fieldName) const;

        const FieldList & fields() const { return d_fields; }
        const FieldList & eventIns() const { return d_eventIns; }
        const FieldList & eventOuts() const { return d_eventOuts; }

    private:
        void fetchImplementation() const;
        FieldValue::Type has(const FieldList &, const std::string & id) const;
    };


    class Route;
    class Viewer;
    class NodeType;
    class FieldValue;
    class VrmlMatrix;
    class NodeVisitor;
    class BVolume;
    class ScriptNode;
    class ProtoNode;
    class NodeAnchor;
    class NodeAppearance;
    class NodeAudioClip;
    class NodeBackground;
    class NodeBillboard;
    class NodeBox;
    class NodeChild;
    class NodeCollision;
    class NodeColor;
    class NodeColorInt;
    class NodeCone;
    class NodeCoordinate;
    class NodeCoordinateInt;
    class NodeCylinder;
    class NodeCylinderSensor;
    class NodeDirLight;
    class NodeElevationGrid;
    class NodeExtrusion;
    class NodeFog;
    class NodeFontStyle;
    class NodeGeometry;
    class NodeGroup;
    class NodeIFaceSet;
    class NodeILineSet;
    class NodeInline;
    class NodeImageTexture;
    class NodeLight;
    class NodeLOD;
    class NodeMaterial;
    class NodeMovieTexture;
    class NodeNavigationInfo;
    class NodeNormal;
    class NodeNormalInt;
    class NodeOrientationInt;
    class NodePixelTexture;
    class NodePlaneSensor;
    class NodePointLight;
    class NodePointSet;
    class NodePositionInt;
    class NodeProximitySensor;
    class NodeScalarInt;
    class NodeShape;
    class NodeSphere;
    class NodeSphereSensor;
    class NodeSound;
    class NodeSpotLight;
    class NodeSwitch;
    class NodeText;
    class NodeTexture;
    class NodeTextureCoordinate;
    class NodeTextureTransform;
    class NodeTimeSensor;
    class NodeTouchSensor;
    class NodeTransform;
    class NodeViewpoint;
    class NodeVisibilitySensor;
    class NodeWorldInfo;

    class OPENVRML_SCOPE Node {
        friend ostream & operator<<(ostream & os, const Node & f);

        std::string id;

    public:
        const NodeType & type;
        typedef std::list<Route *> RouteList;

        virtual ~Node() = 0;

        const std::string & getId() const;
        void setId(const std::string & nodeId, VrmlNamespace * ns = 0);

        virtual bool accept(NodeVisitor & visitor) = 0;
        virtual void resetVisitedFlag();
        virtual const FieldValue * getField(const std::string & fieldId) const;
        virtual void setField(const std::string & fieldId,
                              const FieldValue & fieldValue);
        virtual void eventIn(double timeStamp,
		             const std::string & eventName,
		             const FieldValue & fieldValue);
        virtual const MFNode getChildren() const;
        virtual void addToScene(VrmlScene *, const std::string & relativeUrl);

        virtual ScriptNode * toScript() const;
        virtual NodeAnchor * toAnchor() const;
        virtual NodeAppearance * toAppearance() const;
        virtual NodeAudioClip * toAudioClip() const;
        virtual NodeBackground * toBackground() const;
        virtual NodeBillboard * toBillboard() const;
        virtual NodeBox * toBox() const;
        virtual NodeChild * toChild() const;
        virtual NodeCollision * toCollision() const;
        virtual NodeColor * toColor() const;
        virtual NodeCone * toCone() const;
        virtual NodeCoordinate * toCoordinate() const;
        virtual NodeCylinder * toCylinder() const;
        virtual NodeCylinderSensor * toCylinderSensor() const;
        virtual NodeDirLight * toDirLight() const;
        virtual NodeElevationGrid * toElevationGrid() const;
        virtual NodeExtrusion * toExtrusion() const;
        virtual NodeFog * toFog() const;
        virtual NodeFontStyle * toFontStyle() const;
        virtual NodeGeometry * toGeometry() const;
        virtual NodeGroup * toGroup() const;
        virtual NodeIFaceSet * toIFaceSet() const;
        virtual NodeImageTexture * toImageTexture() const;
        virtual NodeInline * toInline() const;
        virtual NodeLOD * toLOD() const;
        virtual NodeLight * toLight() const;
        virtual NodeMaterial * toMaterial() const;
        virtual NodeMovieTexture * toMovieTexture() const;
        virtual NodeNavigationInfo * toNavigationInfo() const;
        virtual NodeNormal * toNormal() const;
        virtual NodeOrientationInt * toOrientationInt() const;
        virtual NodePixelTexture * toPixelTexture() const;
        virtual NodePlaneSensor * toPlaneSensor() const;
        virtual NodePointLight * toPointLight() const;
        virtual NodePointSet * toPointSet() const;
        virtual NodePositionInt * toPositionInt() const;
        virtual NodeScalarInt * toScalarInt() const;
        virtual NodeShape * toShape() const;
        virtual NodeSound * toSound() const;
        virtual NodeSphere * toSphere() const;
        virtual NodeSphereSensor * toSphereSensor() const;
        virtual NodeSpotLight * toSpotLight() const;
        virtual NodeSwitch * toSwitch() const;
        virtual NodeTexture * toTexture() const;
        virtual NodeTextureCoordinate * toTextureCoordinate() const;
        virtual NodeTextureTransform * toTextureTransform() const;
        virtual NodeTimeSensor * toTimeSensor() const;
        virtual NodeTouchSensor * toTouchSensor() const;
        virtual NodeTransform * toTransform() const;
        virtual NodeViewpoint * toViewpoint() const;

        // Write self
        ostream& print(ostream& os, int indent) const;
        virtual ostream& printFields(ostream& os, int indent);
        static ostream& printField(ostream&, int, const char*, const FieldValue &);

        // Indicate that the node state has changed, need to re-render
        void setModified();
        void clearModified() { d_modified = false; }
        virtual bool isModified() const;
        typedef std::list< Node* > NodePath; // duplicate from VrmlScene. argh.


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

        RouteList getRoutes();

        const FieldValue * getEventOut(const std::string & fieldName) const;


        virtual void render(Viewer *, VrmlRenderContext rc);


        virtual void accumulateTransform(Node*);

        virtual Node* getParentTransform();

        virtual void inverseTransform(Viewer *);

        virtual void inverseTransform(VrmlMatrix &);

        VrmlScene *scene() { return d_scene; }

    protected:
        Node(const NodeType & nodeType, VrmlScene * scene);
        Node(const Node &);

        enum { INDENT_INCREMENT = 4 };

        // Send a named event from this node.
        void eventOut(double timeStamp, const std::string & eventName,
		      const FieldValue & fieldValue);

        // Scene this node belongs to
        VrmlScene *d_scene;

        // True if a field changed since last render
        bool d_modified;
        bool d_flag;
        bool d_bvol_dirty;
        bool visited;

        // Routes from this node (clean this up, add RouteList ...)
        RouteList d_routes;
    };


    class OPENVRML_SCOPE NodeVisitor {
    public:
        virtual ~NodeVisitor() = 0;

        virtual void visit(ScriptNode & node) = 0;
        virtual void visit(ProtoNode & node) = 0;
        virtual void visit(NodeAnchor & node) = 0;
        virtual void visit(NodeAppearance & node) = 0;
        virtual void visit(NodeAudioClip & node) = 0;
        virtual void visit(NodeBackground & node) = 0;
        virtual void visit(NodeBillboard & node) = 0;
        virtual void visit(NodeBox & node) = 0;
        virtual void visit(NodeCollision & node) = 0;
        virtual void visit(NodeColor & node) = 0;
        virtual void visit(NodeColorInt & node) = 0;
        virtual void visit(NodeCone & node) = 0;
        virtual void visit(NodeCoordinate & node) = 0;
        virtual void visit(NodeCoordinateInt & node) = 0;
        virtual void visit(NodeCylinder & node) = 0;
        virtual void visit(NodeCylinderSensor & node) = 0;
        virtual void visit(NodeDirLight & node) = 0;
        virtual void visit(NodeElevationGrid & node) = 0;
        virtual void visit(NodeExtrusion & node) = 0;
        virtual void visit(NodeFog & node) = 0;
        virtual void visit(NodeFontStyle & node) = 0;
        virtual void visit(NodeGroup & node) = 0;
        virtual void visit(NodeIFaceSet & node) = 0;
        virtual void visit(NodeILineSet & node) = 0;
        virtual void visit(NodeImageTexture & node) = 0;
        virtual void visit(NodeInline & node) = 0;
        virtual void visit(NodeLOD & node) = 0;
        virtual void visit(NodeMaterial & node) = 0;
        virtual void visit(NodeMovieTexture & node) = 0;
        virtual void visit(NodeNavigationInfo & node) = 0;
        virtual void visit(NodeNormal & node) = 0;
        virtual void visit(NodeNormalInt & node) = 0;
        virtual void visit(NodeOrientationInt & node) = 0;
        virtual void visit(NodePixelTexture & node) = 0;
        virtual void visit(NodePlaneSensor & node) = 0;
        virtual void visit(NodePointLight & node) = 0;
        virtual void visit(NodePointSet & node) = 0;
        virtual void visit(NodePositionInt & node) = 0;
        virtual void visit(NodeProximitySensor & node) = 0;
        virtual void visit(NodeScalarInt & node) = 0;
        virtual void visit(NodeShape & node) = 0;
        virtual void visit(NodeSound & node) = 0;
        virtual void visit(NodeSphere & node) = 0;
        virtual void visit(NodeSphereSensor & node) = 0;
        virtual void visit(NodeSpotLight & node) = 0;
        virtual void visit(NodeSwitch & node) = 0;
        virtual void visit(NodeText & node) = 0;
        virtual void visit(NodeTextureCoordinate & node) = 0;
        virtual void visit(NodeTextureTransform & node) = 0;
        virtual void visit(NodeTimeSensor & node) = 0;
        virtual void visit(NodeTouchSensor & node) = 0;
        virtual void visit(NodeTransform & node) = 0;
        virtual void visit(NodeViewpoint & node) = 0;
        virtual void visit(NodeVisibilitySensor & node) = 0;
        virtual void visit(NodeWorldInfo & node) = 0;
    };
}

// Ugly macro used in printFields
#define PRINT_FIELD(_f) printField(os,indent+INDENT_INCREMENT,#_f,d_##_f)

// Ugly macros used in setField

#define TRY_FIELD(_f,_t) \
(fieldId == #_f) {\
    if ( fieldValue.to##_t() )\
      d_##_f = (_t &)fieldValue;\
    else \
      theSystem->error("Invalid type (%s) for %s field of %s node (expected %s).\n",\
	    fieldValue.typeName(), #_f, this->type.getId().c_str(), #_t);\
  }

// For SFNode fields. Allow un-fetched EXTERNPROTOs to succeed...
#define TRY_SFNODE_FIELD(_f,_n) \
(fieldId == #_f) { \
    SFNode *x=(SFNode*)&fieldValue; \
    if (fieldValue.toSFNode() \
            && (!x->get() || x->get()->to##_n() \
                || dynamic_cast<ProtoNode *>(x->get().get()))) \
        d_##_f = (SFNode &)fieldValue; \
    else \
      theSystem->error("Invalid type (%s) for %s field of %s node (expected %s).\n",\
	    fieldValue.typeName(), #_f, this->type.getId().c_str(), #_n);\
  }

#define TRY_SFNODE_FIELD2(_f,_n1,_n2) \
(fieldId == #_f) { \
    SFNode *x=(SFNode*)&fieldValue; \
    if (fieldValue.toSFNode() && \
	((!x->get()) || x->get()->to##_n1() || x->get()->to##_n2() || \
	 dynamic_cast<ProtoNode *>(x->get().get()))) \
      d_##_f = (SFNode &)fieldValue; \
    else \
      theSystem->error("Invalid type (%s) for %s field of %s node (expected %s or %s).\n",\
	    fieldValue.typeName(), #_f, this->type.getId().c_str(), #_n1, #_n2);\
  }

# endif

