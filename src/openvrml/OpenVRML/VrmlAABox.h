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

#ifndef VRMLAABOX_H
#define VRMLAABOX_H

#include "common.h"
#include "VrmlBVolume.h"

namespace OpenVRML {

    class OPENVRML_SCOPE VrmlAABox : public VrmlBVolume {
    public:
        virtual ~VrmlAABox();
        virtual int isectFrustum(const VrmlFrustum & f) const;
        virtual void extend(const VrmlBVolume & b);
        virtual void extend(const float p[3]);
        virtual void extend(const VrmlAABox & b);
        virtual void extend(const VrmlBSphere & b);
        virtual void enclose(const float* p, int n);
        virtual bool isMAX() const;
        virtual void setMAX();
        virtual void orthoTransform(const VrmlMatrix & M);
        virtual void transform(const VrmlMatrix & M);
        virtual ostream & dump(ostream & ostr) const;
    };
}

#endif
