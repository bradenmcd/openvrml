
#ifndef VRMLAABOX_H
#define VRMLAABOX_H

/*
 * VrmlBSphere.h
 *
 * Part of the libvrml97 core library.
 * 
 * Copyright (c) 2000 Christopher K. St. John
 * cstjohn@acm.org
 * 
 * see file COPYING for details of license.
 *
 */



#include "VrmlBVolume.h"



/**
 * An axis-aligned bounding box.
 * <p>
 * You might have noticed that this class is currently just a
 * placeholder.
 *
 */
class VrmlAABox : public VrmlBVolume {

 public:

  VrmlBSphere* toBSphere() const;
  VrmlAABox* toAABox() const;

  int isectFrustum(const VrmlFrustum& f) const;

  void extend(const VrmlBVolume& b);
  void extend(const float p[3]);
  void extend(const VrmlAABox& b);
  void extend(const VrmlBSphere& b);
  void enclose(const float* p, int n);

  bool isMAX() const;
  void setMAX();

  void orthoTransform(const double M[4][4]);
  void transform(const double M[4][4]);

  ostream& dump(ostream& ostr) const;
};



#endif // VRMLAABOX_H
