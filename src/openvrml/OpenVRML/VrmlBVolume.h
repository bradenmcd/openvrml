//
// OpenVRML
//
// Copyright (C) 2000  Christopher K. St. John
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

#ifndef VRMLBVOLUME_H
#define VRMLBVOLUME_H

#include <iostream.h>
#include "common.h"

class VrmlFrustum;
class VrmlAABox;
class VrmlBSphere;
class VrmlMatrix;

class VrmlBVolume {
public:
    enum { BV_INSIDE = 1, BV_OUTSIDE = -1, BV_PARTIAL = 0 };

    virtual ~VrmlBVolume() = 0;
    virtual bool isMAX() const = 0;
    virtual void setMAX() = 0;
    virtual int isectFrustum(const VrmlFrustum & f) const = 0;
    virtual void extend(const VrmlBVolume & b) =0;
    virtual void extend(const float p[3]) = 0;
    virtual void extend(const VrmlAABox & b) = 0;
    virtual void extend(const VrmlBSphere & b) = 0;
    virtual void enclose(const float* p, int n) = 0;
    virtual void orthoTransform(const VrmlMatrix & M) = 0;
    virtual void transform(const VrmlMatrix & M) = 0;
    virtual ostream & dump(ostream & ostr) const = 0;
};

#endif // VRMLBVOLUME_H
