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

#ifndef VRMLNODE_H
#define VRMLNODE_H

#include <string.h>
#include <list>
#include <iostream.h>
#include "System.h"		// error

class Route;
class Viewer;

class VrmlNamespace;
class VrmlNodeType;
class VrmlField;
class VrmlScene;

#include "VrmlRenderContext.h"
class VrmlBVolume;

// For the safe downcasts
class VrmlNodeAnchor;
class VrmlNodeAppearance;
class VrmlNodeAudioClip;
class VrmlNodeBackground;
class VrmlNodeBox;   //LarryD Mar 08/99
class VrmlNodeChild;
class VrmlNodeColor;
class VrmlNodeCone;   //LarryD Mar 08/99
class VrmlNodeCoordinate;
class VrmlNodeCylinder; //LarryD Mar 08/99
class VrmlNodeDirLight; //LarryD Mar 08/99
class VrmlNodeElevationGrid; //LarryD Mar 09/99
class VrmlNodeExtrusion; //LarryD Mar 09/99
class VrmlNodeFog;
class VrmlNodeFontStyle;
class VrmlNodeGeometry;
class VrmlNodeGroup;
class VrmlNodeIFaceSet;
class VrmlNodeInline;
class VrmlNodeLight;
class VrmlNodeMaterial;
class VrmlNodeMovieTexture;
class VrmlNodeNavigationInfo;
class VrmlNodeNormal;
class VrmlNodePlaneSensor;
class VrmlNodePointLight;
class VrmlNodeScript;
class VrmlNodeShape;
class VrmlNodeSphere; //LarryD Mar 08/99
class VrmlNodeSphereSensor;
class VrmlNodeCylinderSensor;
class VrmlNodeSound;
class VrmlNodeSpotLight;
class VrmlNodeSwitch;
class VrmlNodeTexture;
class VrmlNodeTextureCoordinate;
class VrmlNodeTextureTransform;
class VrmlNodeTimeSensor;
class VrmlNodeTouchSensor;
class VrmlNodeTransform;
class VrmlNodeViewpoint;
class VrmlNodeImageTexture;
class VrmlNodePixelTexture;
class VrmlNodeLOD;
class VrmlNodeScalarInt;
class VrmlNodeOrientationInt;
class VrmlNodePositionInt;

class VrmlNodeProto;

// Global output operator for nodes
class VrmlNode;
ostream& operator<<(ostream& os, const VrmlNode& f);


/**
 * A node in the scene graph.
 */
class VrmlNode {
  friend ostream& operator<<(ostream& os, const VrmlNode& f);

public:

  /**
   * Given a VrmlNodeType, add in the fields, exposedFields, eventIns
   * and eventOuts defined by the particular node implementation.
   * There's a great big method in VrmlNamespace that just calls
   * defineType for every built in node. Nodes that inherit from other
   * nodes (eg VrmlNodeIFaceSet) must call their parent classe's
   * defineType, it doesn't happen automatically. The defineType for
   * VrmlNode doesn't actually do anything, since there (currently)
   * aren't any base events to be defined. It's just here for the sake
   * of symmetry (and makes a nice place to park a comment)
   *
   * @param t to get the predefined cached type, pass a null,
   *          otherwise the fields and events will be added to the
   *          passed in vrmlnodetype object
   *
   * @see VrmlNamesspace::defineBuiltins()
   */
  static VrmlNodeType *defineType(VrmlNodeType *t);

  virtual VrmlNodeType & nodeType() const = 0;

  // VrmlNodes are reference counted, optionally named objects
  // The reference counting is manual (that is, each user of a
  // VrmlNode, such as the VrmlMFNode class, calls reference()
  // and dereference() explicitly). Should make it internal...

  VrmlNode ( VrmlScene *s );
  VrmlNode(  const VrmlNode& );
  virtual ~VrmlNode() = 0;

  // Copy the node, defining its name in the specified scope.
  // Uses the flag to determine whether the node is a USEd node.
  VrmlNode *clone( VrmlNamespace* );
  virtual VrmlNode *cloneMe() const = 0;
  virtual void cloneChildren( VrmlNamespace* );

  // Copy the ROUTEs
  virtual void copyRoutes(VrmlNamespace *ns) const;

  // Add/remove references to a VrmlNode. This is silly, as it
  // requires the users of VrmlNode to do the reference/derefs...
  VrmlNode *reference();
  void dereference();

  // Safe node downcasts. These avoid the dangerous casts of VrmlNode* (esp in
  // presence of protos), but are ugly in that this class must know about all
  // the subclasses. These return 0 if the typecast is invalid.
  // Remember to also add new ones to VrmlNodeProto. Protos should
  // return their first implementation node (except toProto()).
  virtual VrmlNodeAnchor*	toAnchor() const;
  virtual VrmlNodeAppearance*	toAppearance() const;
  virtual VrmlNodeAudioClip*	toAudioClip() const;
  virtual VrmlNodeBackground*	toBackground() const;
  virtual VrmlNodeBox*		toBox() const; //LarryD Mar 08/99
  virtual VrmlNodeChild*	toChild() const;
  virtual VrmlNodeColor*	toColor() const;
  virtual VrmlNodeCone* toCone() const; //LarryD Mar 08/99
  virtual VrmlNodeCoordinate*	toCoordinate() const;
  virtual VrmlNodeCylinder* toCylinder() const; //LarryD Mar 08/99
  virtual VrmlNodeDirLight* toDirLight() const; //LarryD Mar 08/99
  virtual VrmlNodeElevationGrid* toElevationGrid() const; //LarryD Mar 09/99
  virtual VrmlNodeExtrusion*    toExtrusion() const; //LarryD Mar 09/99
  virtual VrmlNodeFog*		toFog() const;
  virtual VrmlNodeFontStyle*	toFontStyle() const;
  virtual VrmlNodeGeometry*	toGeometry() const;
  virtual VrmlNodeGroup*	toGroup() const;
  virtual VrmlNodeIFaceSet*	toIFaceSet() const;
  virtual VrmlNodeImageTexture* toImageTexture() const;
  virtual VrmlNodePixelTexture* toPixelTexture() const;
  virtual VrmlNodeInline*	toInline() const;
  virtual VrmlNodeLight*	toLight() const;
  virtual VrmlNodeMaterial*	toMaterial() const;
  virtual VrmlNodeMovieTexture*	toMovieTexture() const;
  virtual VrmlNodeNavigationInfo*	toNavigationInfo() const;
  virtual VrmlNodeNormal*	toNormal() const;
  virtual VrmlNodePlaneSensor*	toPlaneSensor() const;
  virtual VrmlNodeSphereSensor*	toSphereSensor() const;
  virtual VrmlNodeCylinderSensor* toCylinderSensor() const;
  virtual VrmlNodePointLight*	toPointLight() const;
  virtual VrmlNodeScript*	toScript() const;
  virtual VrmlNodeShape*	toShape() const;
  virtual VrmlNodeSphere* toSphere() const; //LarryD Mar 08/99
  virtual VrmlNodeSound*	toSound() const;
  virtual VrmlNodeSpotLight*	toSpotLight() const;
  virtual VrmlNodeSwitch* toSwitch() const; //LarryD Mar 08/99
  virtual VrmlNodeTexture*	toTexture() const;
  virtual VrmlNodeTextureCoordinate*	toTextureCoordinate() const;
  virtual VrmlNodeTextureTransform* toTextureTransform() const;
  virtual VrmlNodeTimeSensor*	toTimeSensor() const;
  virtual VrmlNodeTouchSensor*	toTouchSensor() const;
  virtual VrmlNodeTransform* toTransform() const;     //LarryD Feb 24/99
  virtual VrmlNodeViewpoint*	toViewpoint() const;

  virtual VrmlNodeLOD* toLOD() const;
  virtual VrmlNodeScalarInt* toScalarInt() const;
  virtual VrmlNodeOrientationInt* toOrientationInt() const;
  virtual VrmlNodePositionInt* toPositionInt() const;

  virtual VrmlNodeProto*	toProto() const;

  // Node DEF/USE/ROUTE name
  void setName(const char *nodeName, VrmlNamespace *ns = 0 );
  const char *name() const;

  // Add to a scene. A node can belong to at most one scene for now.
  // If it doesn't belong to a scene, it can't be rendered.
  virtual void addToScene( VrmlScene *, const char *relativeUrl );

  // Write self
  ostream& print(ostream& os, int indent) const;
  virtual ostream& printFields(ostream& os, int indent);
  static ostream& printField(ostream&, int, const char*, const VrmlField&);

  // Indicate that the node state has changed, need to re-render
  void setModified();
  void clearModified() { d_modified = false; }
  virtual bool isModified() const;
  typedef std::list< VrmlNode* > VrmlNodePath; // duplicate from VrmlScene. argh.


  /**
   * Mark all the nodes in the path as (not) modified. Convenience
   * function used by updateModified.
   */
  static void markPathModified(VrmlNodePath& path, bool mod, int flags = 0x003);

  // do the work of updatemodified. move this to be protected
  //
  virtual void updateModified(VrmlNodePath& path, int flags = 0x003);

  /**
   * Propegate the bvolume dirty flag from children to parents. I
   * don't like this at all, but it's not worth making it pretty
   * because the need for it will go away when parent pointers are
   * implemented.
   *
   * @param path stack of ancestor nodes
   * @param mod set modified flag to this value
   * @param flags 1 indicates normal modified flag, 2 indicates the
   *              bvolume dirty flag, 3 indicates both
   */
  void updateModified(int flags = 0x003);

  /**
   * Get this node's bounding volume. Nodes that have no bounding
   * volume, or have a difficult to calculate bvolume (like, say,
   * extrusion or billboard) can just return an infinite bsphere. Note
   * that returning an infinite bvolume means that all the node's
   * ancestors will also end up with an infinite bvolume, and will
   * never be culled.
   *
   * @return this node's bounding volume
   */
  virtual const VrmlBVolume* getBVolume() const;

  /**
   * Override a node's calculated bounding volume. Not implemented.
   */
  virtual void setBVolume(const VrmlBVolume& v);
  
  /** 
   * Indicate that a node's bounding volume needs to be recalculated
   * (or not). If a node's bvolume is invalid, then the bvolumes of
   * all that node's ancestors are also invalid. Normally, the node
   * itself will determine when its bvolume needs updating.
   */
  virtual void setBVolumeDirty(bool f);

  /**
   * Return true if the node's bounding volume needs to be
   * recalculated.
   */
  virtual bool isBVolumeDirty() const;


  // A generic flag (typically used to find USEd nodes).
  void setFlag() { d_flag = true; }
  virtual void clearFlags();	// Clear childrens flags too.
  bool isFlagSet() { return d_flag; }

  // Add a ROUTE from a field in this node
  void addRoute(const char *fromField, VrmlNode *toNode, const char *toField);

  // Delete a ROUTE from a field in this node
  void deleteRoute(const char *fromField, VrmlNode *toNode, const char *toField);

  // Pass a named event to this node. This method needs to be overridden
  // to support any node-specific eventIns behaviors, but exposedFields
  // (should be) handled here...
  virtual void eventIn(double timeStamp,
		       const char *eventName,
		       const VrmlField *fieldValue);

  // Set a field by name (used by the parser, not for external consumption).
  virtual void setField(const char *fieldName, const VrmlField &fieldValue);

  // Get a field or eventOut by name. getField is used by Script nodes
  // to access exposedFields. It does not allow access to private fields
  // (there tend to be specific access functions for each field for
  // programmatic access).
  virtual const VrmlField *getField(const char *fieldName) const;
  
  // Return an eventOut/exposedField value. Used by the script node
  // to access the node fields. This just strips the _changed suffix
  // and tries to access the field using getField.
  const VrmlField *getEventOut(const char *fieldName) const;


  /**
   * Render this node. Actually, most of the rendering work is
   * delegated to the viewer, but this method is responsible for
   * traversal to the node's renderable children, including
   * culling. Each node class needs to implement this routine
   * appropriately. It's not abstract since it doesn't make sense to
   * call render on some nodes. Alternative would be to break render
   * out into a seperate mixins class, but that's probably overkill.
   *
   * @param v viewer implementation responsible for actually doing the
   *          drawing
   * @param rc generic context argument, holds things like the
   *          accumulated modelview transform.
   */
  virtual void render(Viewer *, VrmlRenderContext rc);


  /**
   * Cache a pointer to one of the parent transforms. The resulting
   * pointer is used by getParentTransform. Grouping nodes need to
   * redefine this, the default implementation does nothing.
   *
   * @param p parent node. can be null.
   *
   * @deprecated This routine will go away once parent pointers
   * are implemented.
   */
  virtual void accumulateTransform(VrmlNode*);

  /**
   * Return the nearest ancestor node that affects the modelview
   * transform. Doesn't work for nodes with more than one parent.
   */
  virtual VrmlNode* getParentTransform();

  /**
   * Compute the inverse of the transform above a viewpoint node. Just
   * like the version that takes a matrix, but instead calls
   * Viewer::setTransform at each level. The idea is to call this
   * routine right before the start of a render traversal.
   *
   * @see getParentTransform
   *
   * @deprecated This method is (gradually) being replaces by
   * inverseTranform(double[4][4]) and should no longer be used.
   */
  virtual void inverseTransform(Viewer *);

  /**
   * Compute the inverse of the transform stack above a Viewpoint
   * node. This is safe since the behavior of multi-parented
   * Viewpoint nodes is undefined. May be called at any time.
   *
   * @param M return the accumulated inverse
   *
   * @see accumulateTransform
   * @see getParentTransform
   */
  virtual void inverseTransform(double [4][4]);

  VrmlScene *scene() { return d_scene; }


protected:

  enum { INDENT_INCREMENT = 4 };

  // Send a named event from this node.
  void eventOut(double timeStamp,
		const char *eventName,
		const VrmlField &fieldValue);

  // Scene this node belongs to
  VrmlScene *d_scene;

  // True if a field changed since last render
  bool d_modified;
  bool d_flag;
  bool d_bvol_dirty;

  // Routes from this node (clean this up, add RouteList ...)
  Route *d_routes;


private:

  int d_refCount;		// Number of active references
  char *d_name;

};


/**
 * Routes
 */
class Route {
public:
  Route(const char *fromEventOut, VrmlNode *toNode, const char *toEventIn);
  Route(const Route&);
  ~Route();

  char *fromEventOut() { return d_fromEventOut; }
  char *toEventIn() { return d_toEventIn; }
  VrmlNode *toNode() { return d_toNode; }

  Route *prev() { return d_prev; }
  Route *next() { return d_next; }
  void setPrev(Route* r) { d_prev = r; }
  void setNext(Route* r) { d_next = r; }
  
private:
  char *d_fromEventOut;
  VrmlNode *d_toNode;
  char *d_toEventIn;
  Route *d_prev, *d_next;
};


// Ugly macro used in printFields
#define PRINT_FIELD(_f) printField(os,indent+INDENT_INCREMENT,#_f,d_##_f)

// Ugly macros used in setField

#define TRY_FIELD(_f,_t) \
(strcmp(fieldName, #_f) == 0) {\
    if ( fieldValue.to##_t() )\
      d_##_f = (Vrml##_t &)fieldValue;\
    else \
      theSystem->error("Invalid type (%s) for %s field of %s node (expected %s).\n",\
	    fieldValue.fieldTypeName(), #_f, nodeType().getName(), #_t);\
  }

// For SFNode fields. Allow un-fetched EXTERNPROTOs to succeed...
#define TRY_SFNODE_FIELD(_f,_n) \
(strcmp(fieldName, #_f) == 0) { \
    VrmlSFNode *x=(VrmlSFNode*)&fieldValue; \
    if (fieldValue.toSFNode() && \
	( (!x->get()) || x->get()->to##_n() || x->get()->toProto() )) \
      d_##_f = (VrmlSFNode &)fieldValue; \
    else \
      theSystem->error("Invalid type (%s) for %s field of %s node (expected %s).\n",\
	    fieldValue.fieldTypeName(), #_f, nodeType().getName(), #_n);\
  }

#define TRY_SFNODE_FIELD2(_f,_n1,_n2) \
(strcmp(fieldName, #_f) == 0) { \
    VrmlSFNode *x=(VrmlSFNode*)&fieldValue; \
    if (fieldValue.toSFNode() && \
	((!x->get()) || x->get()->to##_n1() || x->get()->to##_n2() || \
	 x->get()->toProto() )) \
      d_##_f = (VrmlSFNode &)fieldValue; \
    else \
      theSystem->error("Invalid type (%s) for %s field of %s node (expected %s or %s).\n",\
	    fieldValue.fieldTypeName(), #_f, nodeType().getName(), #_n1, #_n2);\
  }


#endif VRMLNODE_H

