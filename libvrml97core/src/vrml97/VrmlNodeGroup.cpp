//
//  Vrml 97 library
//  Copyright (C) 1998 Chris Morley
//  See the file COPYING for license details.
//
//  VrmlNodeGroup.cpp
//

#include "VrmlNodeGroup.h"
#include "VrmlNodeType.h"
#include "VrmlNodeProto.h"
#include "VrmlNodePlaneSensor.h"
#include "VrmlNodeTouchSensor.h"
#include "VrmlMFNode.h"
#include "VrmlSFVec3f.h"


// Return a new VrmlNodeGroup
static VrmlNode *creator( VrmlScene *s ) { return new VrmlNodeGroup(s); }


// Define the built in VrmlNodeType:: "Group" fields

VrmlNodeType *VrmlNodeGroup::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;
      t = st = new VrmlNodeType("Group", creator);
    }

  VrmlNodeChild::defineType(t);	// Parent class
  t->addEventIn("addChildren", VrmlField::MFNODE);
  t->addEventIn("removeChildren", VrmlField::MFNODE);
  t->addExposedField("children", VrmlField::MFNODE);
  t->addField("bboxCenter", VrmlField::SFVEC3F);
  t->addField("bboxSize", VrmlField::SFVEC3F);

  return t;
}


VrmlNodeType & VrmlNodeGroup::nodeType() const
{
    return *defineType(0);
}


VrmlNodeGroup::VrmlNodeGroup(VrmlScene *scene) :
  VrmlNodeChild(scene),
  d_bboxSize(-1.0, -1.0, -1.0),
  d_parentTransform(0),
  d_viewerObject(0)
{
  this->setBVolumeDirty(true);
}

VrmlNodeGroup::~VrmlNodeGroup()
{
  // delete d_viewerObject...
}

// 

VrmlNode *VrmlNodeGroup::cloneMe() const
{
  return new VrmlNodeGroup(*this);
}

void VrmlNodeGroup::cloneChildren(VrmlNamespace *ns)
{
  int n = d_children.size();
  VrmlNode **kids = d_children.get();
  for (int i = 0; i<n; ++i)
    {
      if (! kids[i]) continue;
      VrmlNode *newKid = kids[i]->clone(ns)->reference();
      kids[i]->dereference();
      kids[i] = newKid;
    }
  this->setBVolumeDirty(true);
}


VrmlNodeGroup* VrmlNodeGroup::toGroup() const
{ return (VrmlNodeGroup*) this; }


bool VrmlNodeGroup::isModified() const
{
  if (d_modified) return true;
  
  int n = d_children.size();

  for (int i = 0; i<n; ++i)
    if (d_children[i]->isModified())
      return true;

  return false;
}


void
VrmlNodeGroup::updateModified(VrmlNodePath& path, int flags)
{
  // if the mark_modifed short circuit doesn't
  // pan out, we should be a little smarter here...
  //cout << "VrmlNodeGroup[" << this << "]::updateModified()" << endl;
  if (this->isModified()) markPathModified(path, true, flags);
  path.push_front(this);
  int n = d_children.size();
  //cout << "VrmlNodeGroup[" << this << "]::updateModified():n=" << n << endl;
  for (int i=0; i<n; ++i) {
    //cout << "VrmlNodeGroup[" << this << "]::updateModified():" << d_children[i] << endl;
    d_children[i]->updateModified(path, flags);
  }
  path.pop_front();
}



void VrmlNodeGroup::clearFlags()
{
  VrmlNode::clearFlags();
  int n = d_children.size();
  for (int i=0; i<n; ++i)
    d_children[i]->clearFlags();
}


void VrmlNodeGroup::addToScene(VrmlScene *s, const char *relativeUrl)
{
  d_scene = s;

  theSystem->debug("VrmlNodeGroup::addToScene( %s )\n",
		   relativeUrl ? relativeUrl : "<null>");

  const char *currentRel = d_relative.get();
  if ( ! currentRel || ! relativeUrl || strcmp(currentRel, relativeUrl) != 0)
    d_relative.set( relativeUrl );
  
  int n = d_children.size();

  for (int i = 0; i<n; ++i)
    d_children[i]->addToScene(s, d_relative.get());
}


// Copy the routes to nodes in the given namespace.

void VrmlNodeGroup::copyRoutes( VrmlNamespace *ns ) const
{
  VrmlNode::copyRoutes(ns);  // Copy my routes

  // Copy childrens' routes
  int n = d_children.size();
  for (int i = 0; i<n; ++i)
    d_children[i]->copyRoutes( ns );
}

VrmlNode* VrmlNodeGroup::getParentTransform() { return d_parentTransform; }



ostream& VrmlNodeGroup::printFields(ostream& os, int indent)
{
  if (d_bboxCenter.x() != 0.0 ||
      d_bboxCenter.z() != 0.0 ||
      d_bboxCenter.y() != 0.0)
    PRINT_FIELD(bboxCenter);
  if (d_bboxSize.x() != -1.0 ||
      d_bboxSize.z() != -1.0 ||
      d_bboxSize.y() != -1.0)
    PRINT_FIELD(bboxSize);
  if (d_children.size() > 0)
    PRINT_FIELD(children);

  return os;
}


// Render each of the children

void VrmlNodeGroup::render(Viewer *viewer, VrmlRenderContext rc)
{
  //cout << "VrmlNodeGroup::render()" << endl;

  if (rc.getCullFlag() != VrmlBVolume::BV_INSIDE) {

    const VrmlBSphere* bs = (const VrmlBSphere*)this->getBVolume();
    VrmlBSphere bv_copy(*bs);
    bv_copy.transform(rc.getMatrix());
    int r = viewer->isectViewVolume(bv_copy);
    if (rc.getDrawBSpheres())
      viewer->drawBSphere(*bs, r);
    //bs->dump(cout);

    if (r == VrmlBVolume::BV_OUTSIDE)
      return;
    if (r == VrmlBVolume::BV_INSIDE)
      rc.setCullFlag(VrmlBVolume::BV_INSIDE);

    //rc.setCullFlag(VrmlBVolume::BV_PARTIAL);
  }

  renderNoCull(viewer, rc);
}


// because children will already have done the culling, we don't need
// to repeat it here.
//
void VrmlNodeGroup::renderNoCull(Viewer *viewer, VrmlRenderContext rc)
{
  //cout << "VrmlNodeGroup::renderNoCull()" << endl;

  if ( d_viewerObject && isModified() )
    {
      viewer->removeObject(d_viewerObject);
      d_viewerObject = 0;
    }

  if (d_viewerObject)
    viewer->insertReference(d_viewerObject);

  else if (d_children.size() > 0)
    {
      int i, n = d_children.size();
      int nSensors = 0;

      d_viewerObject = viewer->beginObject(name());

      // Draw nodes that impact their siblings (DirectionalLights,
      // TouchSensors, any others? ...)
      for (i = 0; i<n; ++i)
	{
	  VrmlNode *kid = d_children[i];

	  if ( kid->toLight() && ! (kid->toPointLight() || kid->toSpotLight()) )
	    kid->render(viewer, rc);
	  else if (( kid->toTouchSensor() && kid->toTouchSensor()->isEnabled() ) ||
		   ( kid->toPlaneSensor() && kid->toPlaneSensor()->isEnabled() ))
	    {
	      if (++nSensors == 1)
		viewer->setSensitive( this );
	    }
	}	      

      // Do the rest of the children (except the scene-level lights)
      for (i = 0; i<n; ++i)
	if (! (d_children[i]->toLight() ||
	       d_children[i]->toPlaneSensor() ||
	       d_children[i]->toTouchSensor()) )
	  d_children[i]->render(viewer, rc);

      // Turn off sensitivity
      if (nSensors > 0)
	viewer->setSensitive( 0 );

      viewer->endObject();

    }

  clearModified();
}


// Accumulate transforms
// Cache a pointer to (one of the) parent transforms for proper
// rendering of bindables.

void VrmlNodeGroup::accumulateTransform(VrmlNode *parent)
{
  d_parentTransform = parent;

  int i, n = d_children.size();

  for (i = 0; i<n; ++i)
    {
      VrmlNode *kid = d_children[i];
      kid->accumulateTransform(parent);
    }

}

// Pass on to enabled touchsensor child.

void VrmlNodeGroup::activate( double time,
			      bool isOver, bool isActive,
			      double *p )
{
  int i, n = d_children.size();

  for (i = 0; i<n; ++i)
    {
      VrmlNode *kid = d_children[i];

      if ( kid->toTouchSensor() && kid->toTouchSensor()->isEnabled() )
	{
	  kid->toTouchSensor()->activate( time, isOver, isActive, p );
	  break;
	}
      else if ( kid->toPlaneSensor() && kid->toPlaneSensor()->isEnabled() )
	{
	  kid->toPlaneSensor()->activate( time, isActive, p );
	  break;
	}
    }	      

}


void VrmlNodeGroup::addChildren( const VrmlMFNode &children )
{
  int nNow = d_children.size();
  int n = children.size();

  for (int i=0; i<n; ++i)
    {
      VrmlNode *child = children[i];
      VrmlNodeProto *p = 0;

      // Add legal children and un-instantiated EXTERNPROTOs
      // Is it legal to add null children nodes?
      if ( child == 0 || child->toChild() ||
	   ((p = child->toProto()) != 0 && p->size() == 0) )
	{
	  d_children.addNode(child);
	  if (child)
	    {
	      child->addToScene( d_scene, d_relative.get() );
	      child->accumulateTransform( d_parentTransform );
	    }
	}
      else
	theSystem->error
	  ("Error: Attempt to add a %s node as a child of a %s node.\n",
	      child->nodeType().getName(), nodeType().getName());
    }

  if (nNow != (int) d_children.size())
    {
      //??eventOut( d_scene->timeNow(), "children_changed", d_children );
      setModified();
      this->setBVolumeDirty(true);
    }

}

void VrmlNodeGroup::removeChildren( const VrmlMFNode &children )
{
  int nNow = d_children.size();
  int n = children.size();

  for (int i=0; i<n; ++i)
    d_children.removeNode(children[i]);

  if (nNow != (int) d_children.size())
    {
      //??eventOut( d_scene->timeNow(), "children_changed", d_children );
      setModified();
      this->setBVolumeDirty(true);
    }

}

void VrmlNodeGroup::removeChildren()
{
  int n = d_children.size();

  for (int i=n; i>0; --i)
    d_children.removeNode( d_children[i-1] );

  setModified();
  this->setBVolumeDirty(true);
}


void VrmlNodeGroup::eventIn(double timeStamp,
			    const char *eventName,
			    const VrmlField *fieldValue)
{
  if (! fieldValue) return;

  if (strcmp(eventName, "addChildren") == 0)
    {
      if ( fieldValue->toMFNode() )      // check that fieldValue is MFNode
	addChildren( *(fieldValue->toMFNode()) );
      else
	theSystem->error("VrmlNodeGroup.%s %s eventIn invalid field type.\n",
		      name(), eventName);
    }

  else if (strcmp(eventName, "removeChildren") == 0)
    {
      if ( fieldValue->toMFNode() )      // check that fieldValue is MFNode
	removeChildren( *(fieldValue->toMFNode()) );
      else
	theSystem->error("VrmlNodeGroup.%s %s eventIn invalid field type.\n",
		      name(), eventName);
    }

  else
    {
      VrmlNode::eventIn(timeStamp, eventName, fieldValue);
    }
}

// Get the value of one of the exposedFields or eventOuts.

const VrmlField *VrmlNodeGroup::getField(const char *fieldName) const
{
  if ( strcmp( fieldName, "children" ) == 0 )
    return &d_children;

  return VrmlNodeChild::getField( fieldName );
}

// Set the value of one of the node fields.
void VrmlNodeGroup::setField(const char *fieldName,
			     const VrmlField &fieldValue)
{
  if TRY_FIELD(bboxCenter, SFVec3f)
  else if TRY_FIELD(bboxSize, SFVec3f)
  else if TRY_FIELD(children, MFNode)
  else
    VrmlNodeChild::setField(fieldName, fieldValue);
  this->setBVolumeDirty(true); // overly conservative?
}

int VrmlNodeGroup::size()
{
  return d_children.size();
}

VrmlNode *VrmlNodeGroup::child(int index)
{
  if (index >= 0 && index < (int) d_children.size())
    return d_children[index];

  return 0;
}



const VrmlBVolume* VrmlNodeGroup::getBVolume() const
{
  //cout << "VrmlNodeGroup[" << this << "]::getBVolume() {" << endl;
  if (this->isBVolumeDirty())
    ((VrmlNodeGroup*)this)->recalcBSphere();
  //cout << "}:";
  //d_bsphere.dump(cout) << endl;
  return &d_bsphere;
}



void
VrmlNodeGroup::recalcBSphere()
{
  //cout << "VrmlNodeGroup[" << this << "]::recalcBSphere()" << endl;
  d_bsphere.reset();
  for (int i = 0; i< (int) d_children.size(); ++i) {
    VrmlNode* ci = d_children[i];
    const VrmlBVolume* ci_bv = ci->getBVolume();
    if (ci_bv)
      d_bsphere.extend(*ci_bv);
  }
  //cout << "VrmlNodeGroup[" << this << "]::recalcBSphere()";
  //d_bsphere.dump(cout) << endl;
  this->setBVolumeDirty(false);
}
