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

#include "VrmlNodeTransform.h"
#include "MathUtils.h"
#include "VrmlNodeType.h"
#include "VrmlRenderContext.h"


static VrmlNode *creator(VrmlScene *s) { return new VrmlNodeTransform(s); }


// Define the built in VrmlNodeType:: "Transform" fields

VrmlNodeType *VrmlNodeTransform::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;
      t = st = new VrmlNodeType("Transform", creator);
      t->reference();
    }

  VrmlNodeGroup::defineType(t);	// Parent class
  t->addExposedField("center", VrmlField::SFVEC3F);
  t->addExposedField("rotation", VrmlField::SFROTATION);
  t->addExposedField("scale", VrmlField::SFVEC3F);
  t->addExposedField("scaleOrientation", VrmlField::SFROTATION);
  t->addExposedField("translation", VrmlField::SFVEC3F);

  return t;
}

VrmlNodeType & VrmlNodeTransform::nodeType() const
{
    return *defineType(0);
}


VrmlNodeTransform::VrmlNodeTransform(VrmlScene *scene) :
  VrmlNodeGroup(scene),
  d_center(0.0, 0.0, 0.0),
  d_rotation(0.0, 0.0, 1.0, 0.0),
  d_scale(1.0, 1.0, 1.0),
  d_scaleOrientation(0.0, 0.0, 1.0, 0.0),
  d_translation(0.0, 0.0, 0.0),
  d_xformObject(0)
{
  M_dirty = true;
  synch_cached_matrix();
  this->setBVolumeDirty(true);
}

VrmlNodeTransform::~VrmlNodeTransform()
{
  // delete d_xformObject...
}

VrmlNodeTransform* VrmlNodeTransform::toTransform() const //LarryD Feb24/99
{ return (VrmlNodeTransform*) this; }

const VrmlSFVec3f& VrmlNodeTransform::getCenter() const   // LarryD Feb 18/99
{  return d_center; }

const VrmlSFRotation& VrmlNodeTransform::getRotation() const  //LarryD Feb 24/99
{ return d_rotation; }

const VrmlSFVec3f& VrmlNodeTransform::getScale() const //LarryD Feb 24/99
{ return d_scale ;}

const VrmlSFRotation& VrmlNodeTransform::getScaleOrientation() const //LarryD Feb 24/99
{ return d_scaleOrientation; }

const VrmlSFVec3f& VrmlNodeTransform::getTranslation() const  //LarryD Feb 24/99
{ return d_translation; }

VrmlNode *VrmlNodeTransform::cloneMe() const
{
  return new VrmlNodeTransform(*this);
}


ostream& VrmlNodeTransform::printFields(ostream& os, int indent)
{
  if (! FPZERO(d_center.x()) ||
      ! FPZERO(d_center.y()) ||
      ! FPZERO(d_center.z()))
    PRINT_FIELD(center);
  if (! FPZERO(d_rotation.x()) ||
      ! FPZERO(d_rotation.y()) ||
      ! FPEQUAL(d_rotation.z(), 1.0) ||
      ! FPZERO(d_rotation.r()))
    PRINT_FIELD(rotation);
  if (! FPEQUAL(d_scale.x(), 1.0) ||
      ! FPEQUAL(d_scale.y(), 1.0) ||
      ! FPEQUAL(d_scale.z(), 1.0))
    PRINT_FIELD(scale);
  if (! FPZERO(d_scaleOrientation.x()) ||
      ! FPZERO(d_scaleOrientation.y()) ||
      ! FPEQUAL(d_scaleOrientation.z(), 1.0) ||
      ! FPZERO(d_scaleOrientation.r()))
    PRINT_FIELD(scaleOrientation);
  if (! FPZERO(d_translation.x()) ||
      ! FPZERO(d_translation.y()) ||
      ! FPZERO(d_translation.z()))
    PRINT_FIELD(translation);

  VrmlNodeGroup::printFields(os, indent);
  return os;
}


void VrmlNodeTransform::render(Viewer *viewer, VrmlRenderContext rc)
{
  //cout << "VrmlNodeTransform::render()" << endl;
  
  if (rc.getCullFlag() != VrmlBVolume::BV_INSIDE) {

    const VrmlBSphere* bs = (VrmlBSphere*)this->getBVolume();
    VrmlBSphere bv_copy(*bs);
    bv_copy.transform(rc.getMatrix());
    int r = viewer->isectViewVolume(bv_copy);
    if (rc.getDrawBSpheres())
      viewer->drawBSphere(*bs, r);

    if (r == VrmlBVolume::BV_OUTSIDE)
      return;
    if (r == VrmlBVolume::BV_INSIDE)
      rc.setCullFlag(VrmlBVolume::BV_INSIDE);

    //rc.setCullFlag(VrmlBVolume::BV_PARTIAL);
  }

  double LM[4][4];
  synch_cached_matrix();
  this->getMatrix(LM);
  double new_LM[4][4];
  Mcopy(new_LM, rc.getMatrix());
  MM(new_LM, LM);
  rc.setMatrix(new_LM);

  if ( d_xformObject && isModified() )
    {
      viewer->removeObject(d_xformObject);
      d_xformObject = 0;
    }

  if (d_xformObject)
    viewer->insertReference(d_xformObject);

  else if (d_children.size() > 0)
    {
      d_xformObject = viewer->beginObject(name());

      // Apply transforms
      viewer->setTransform(d_center.get(),
			   d_rotation.get(),
			   d_scale.get(),
			   d_scaleOrientation.get(),
			   d_translation.get());

      // Render children
      VrmlNodeGroup::renderNoCull(viewer, rc);

      // Reverse transforms (for immediate mode/no matrix stack renderer)
      viewer->unsetTransform(d_center.get(),
			     d_rotation.get(),
			     d_scale.get(),
			     d_scaleOrientation.get(),
			     d_translation.get());
      viewer->endObject();
    }

  clearModified();
}


// Get the value of a field or eventOut.

const VrmlField *VrmlNodeTransform::getField(const char *fieldName) const
{
  // exposedFields
  if ( strcmp( fieldName, "center" ) == 0 )
    return &d_center;
  else if ( strcmp( fieldName, "rotation" ) == 0 )
    return &d_rotation;
  else if ( strcmp( fieldName, "scale" ) == 0 )
    return &d_scale;
  else if ( strcmp( fieldName, "scaleOrientation" ) == 0 )
    return &d_scaleOrientation;
  else if ( strcmp( fieldName, "translation" ) == 0 )
    return &d_translation;

  return VrmlNodeGroup::getField( fieldName );
}

// Set the value of one of the node fields.

void VrmlNodeTransform::setField(const char *fieldName,
				 const VrmlField &fieldValue)
{
  if TRY_FIELD(center, SFVec3f)
  else if TRY_FIELD(rotation, SFRotation)
  else if TRY_FIELD(scale, SFVec3f)
  else if TRY_FIELD(scaleOrientation, SFRotation)
  else if TRY_FIELD(translation, SFVec3f)
  else
    VrmlNodeGroup::setField(fieldName, fieldValue);
  this->setBVolumeDirty(true);
  M_dirty = true; // d_modified? see M_dirty comments in .h
}


// Cache a pointer to (one of the) parent transforms for proper
// rendering of bindables.

void VrmlNodeTransform::accumulateTransform( VrmlNode *parent )
{
  d_parentTransform = parent;

  int i, n = d_children.size();

  for (i = 0; i<n; ++i)
    {
      VrmlNode *kid = d_children[i];
      kid->accumulateTransform( this );
    }
}


void VrmlNodeTransform::inverseTransform(Viewer *viewer)
{
  VrmlNode *parentTransform = getParentTransform();
  if (parentTransform)
    parentTransform->inverseTransform(viewer);

  // Build the inverse
  float trans[3] = { - d_translation.x(),
		     - d_translation.y(),
		     - d_translation.z() };
  float rot[4] = { d_rotation.x(),
		   d_rotation.y(),
		   d_rotation.z(),
		   -  d_rotation.r() };

  // Invert scale (1/x)
  float scale[3] = { d_scale.x(), d_scale.y(), d_scale.z() };
  if (! FPZERO(scale[0])) scale[0] = 1.0 / scale[0];
  if (! FPZERO(scale[1])) scale[1] = 1.0 / scale[1];
  if (! FPZERO(scale[2])) scale[2] = 1.0 / scale[2];

  // Apply transforms (this may need to be broken into separate
  // calls to perform the ops in reverse order...)
  viewer->setTransform( d_center.get(),
			rot,
			scale,
			d_scaleOrientation.get(),
			trans );
}

void VrmlNodeTransform::inverseTransform(double m[4][4])
{
  //cout << "VrmlNodeTransform::inverseTransform()" << endl;
  VrmlNode *parentTransform = getParentTransform();
  if (parentTransform)
    parentTransform->inverseTransform(m);
  else
    Midentity(m);

  //// Invert this transform
  //float rot[4] = { d_rotation.x(),
  //d_rotation.y(),
  //d_rotation.z(),
  //- d_rotation.r() };
  //double M[4][4];
  //Mrotation( M, rot );
  //MM( m, M );
  //// Invert scale (1/x)
  //float scale[3] = { d_scale.x(), d_scale.y(), d_scale.z() };
  //if (! FPZERO(scale[0])) scale[0] = 1.0 / scale[0];
  //if (! FPZERO(scale[1])) scale[1] = 1.0 / scale[1];
  //if (! FPZERO(scale[2])) scale[2] = 1.0 / scale[2];
  //Mscale( M, scale );

  double IM[4][4];
  synch_cached_matrix();
  transform_to_matrix(this, 1, IM);
  MM( m, IM );
}


const VrmlBVolume*
VrmlNodeTransform::getBVolume() const
{
  //cout << "VrmlNodeTransform[" << this << "]::getBVolume() {" << endl;
  if (this->isBVolumeDirty())
    ((VrmlNodeTransform*)this)->recalcBSphere();
  //cout << "}:";
  //d_bsphere.dump(cout) << endl;
  return &d_bsphere;
}



void
VrmlNodeTransform::recalcBSphere()
{
  //cout << "VrmlNodeTransform[" << this << "]::recalcBSphere()" << endl;
  d_bsphere.reset();
  for (size_t i=0; i<d_children.size(); ++i) {
    const VrmlBVolume* ci_bv = d_children[i]->getBVolume();
    if (ci_bv)
      d_bsphere.extend(*ci_bv);
  }
  synch_cached_matrix();

  //d_bsphere.orthoTransform(M);
  d_bsphere.transform(M);

  this->setBVolumeDirty(false);
}



#if 0
void
VrmlNodeTransform::recalcBSphere()
{
  cout << "VrmlNodeTransform[" << this << "]::recalcBSphere()" << endl;
  synch_cached_matrix();
  d_bsphere.reset();
  for (int i = 0; i<d_children.size(); ++i) {
    VrmlNode* ci = d_children[i];
    const VrmlBVolume* ci_bv = ci->getBVolume();
    if (ci_bv) { // shouldn't happen...
      VrmlBSphere* bs = (VrmlBSphere*)ci_bv;
      VrmlBSphere tmp(*bs);
      tmp.transform(M);
      d_bsphere.extend(tmp);
    }
  }
  this->setBVolumeDirty(false);
}
#endif


// flag==0 is normal matrix
// flag==1 is inverse matrix
//
void
VrmlNodeTransform::transform_to_matrix(const VrmlNodeTransform* t_arg, int flag, double M[4][4])
{
  // lots and lots of fat here, but let's face it: this probably isn't
  // a bottleneck, and until it is, there's no sense in obfuscating
  // the code. so: if you feel tempted to make this "more efficient",
  // do us all a favor and run some performance tests before you commit
  // your changes.
  //
  // judicious reuse of temporaries is a goal to aim for. plus, we
  // could get tricky and reuse the inverses since they have a simple
  // relationship to the original. but see above.
  //
  Midentity(M);     // M = I

  VrmlNodeTransform* t = (VrmlNodeTransform*)t_arg; // argh.

  double NC[4][4];
  float nc[3];
  Vset(nc, t->d_center.get());
  Vscale(nc, -1);
  Mtranslation(NC, nc);
  //Mdump(cout, NC) << endl;

  double NSR[4][4];
  float nsr[4]; // just reverse the rotation, right?
  nsr[0] = t->d_scaleOrientation.x();
  nsr[1] = t->d_scaleOrientation.y();
  nsr[2] = t->d_scaleOrientation.z();
  nsr[3] = -t->d_scaleOrientation.r();
  Mrotation(NSR, nsr);
  //Mdump(cout, NSR) << endl;

  double SR[4][4];
  Mrotation(SR, t->d_scaleOrientation.get());
  //Mdump(cout, SR) << endl;

  double C[4][4];
  Mtranslation(C, t->d_center.get());
  //Mdump(cout, C) << endl;


  if (flag == 0) {
    //
    // normal matrix
    //

    double S[4][4];
    Mscale(S, t->d_scale.get());
    //Mdump(cout, S) << endl;

    double R[4][4];
    Mrotation(R, t->d_rotation.get());
    //Mdump(cout, R) << endl;

    double C[4][4];
    Mtranslation(C, t->d_center.get());
    //Mdump(cout, C) << endl;

    double T[4][4];
    Mtranslation(T, t->d_translation.get());
    //Mdump(cout, T) << endl;

    MM(M, T);         // M = M * T   = T
    MM(M, C);         // M = M * C   = T * C
    MM(M, R);         // M = M * R   = T * C * R
    MM(M, SR);        // M = M * SR  = T * C * R * SR
    MM(M, S);         // M = M * S   = T * C * R * SR * S
    MM(M, NSR);       // M = M * -SR = T * C * R * SR * S * -SR
    MM(M, NC);        // M = M * -C  = T * C * R * SR * S * -SR * -C

  } else {

    //
    // inverse matrix
    //

    double NS[4][4];
    float ns[3];
    Vset(ns, t->d_scale.get());
    ns[0] = 1.0f/ns[0];
    ns[1] = 1.0f/ns[1];
    ns[2] = 1.0f/ns[2];
    Mscale(NS, ns);
    //Mdump(cout, S) << endl;

    double NR[4][4];
    float nr[4];
    nr[0] = t->d_rotation.x();
    nr[1] = t->d_rotation.y();
    nr[2] = t->d_rotation.z();
    nr[3] = -t->d_rotation.r();
    Mrotation(NR, nr);
    
    double NT[4][4];
    float nt[3];
    Vset(nt, t->d_translation.get());
    Vscale(nt, -1);
    Mtranslation(NT, nt);

    MM(M, C);
    MM(M, SR);
    MM(M, NS);
    MM(M, NSR);
    MM(M, NR);
    MM(M, NC);
    MM(M, NT);
  }

}


// P' = T × C × R × SR × S × -SR × -C × P
//
void
VrmlNodeTransform::synch_cached_matrix()
{
  if (M_dirty) {
    transform_to_matrix(this, 0, M);
    //Mdump(cout, M);
    M_dirty = false;
  }
}


void
VrmlNodeTransform::getMatrix(double M_out[4][4]) const
{
  // we're logically, but not physically const
  ((VrmlNodeTransform*)this)->synch_cached_matrix();
  Mcopy(M_out, M);
}
