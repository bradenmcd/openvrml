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

# ifndef OPENVRML_BVOLUME_H
#   define OPENVRML_BVOLUME_H

#   include <iostream.h>
#   include "common.h"

namespace OpenVRML {

    class VrmlFrustum;
    class AABox;
    class BSphere;
    class VrmlMatrix;

    class OPENVRML_SCOPE BVolume {
    public:
        enum { BV_INSIDE = 1, BV_OUTSIDE = -1, BV_PARTIAL = 0 };

        virtual ~BVolume() = 0;
        virtual bool isMAX() const = 0;
        virtual void setMAX() = 0;
        virtual int isectFrustum(const VrmlFrustum & f) const = 0;
        virtual void extend(const BVolume & b) = 0;
        virtual void extend(const float p[3]) = 0;
        virtual void extend(const AABox & b) = 0;
        virtual void extend(const BSphere & b) = 0;
        virtual void enclose(const float* p, int n) = 0;
        virtual void orthoTransform(const VrmlMatrix & M) = 0;
        virtual void transform(const VrmlMatrix & M) = 0;
        virtual ostream & dump(ostream & ostr) const = 0;
    };


    class SFVec3f;

    class OPENVRML_SCOPE BSphere : public BVolume {
        float c[3];
        float r;

    public:
        BSphere();
        BSphere(const BSphere & rhs);
        virtual ~BSphere();

        void reset();

        virtual int isectFrustum(const VrmlFrustum & f) const;

        virtual void extend(const BVolume & b);
        virtual void extend(const float p[3]) ;
        virtual void extend(const AABox & b);
        virtual void extend(const BSphere & b);
        virtual void enclose(const float* p, int n);

        virtual bool isMAX() const;
        virtual void setMAX();

        virtual void orthoTransform(const VrmlMatrix & M);
        virtual void transform(const VrmlMatrix & M);

        const float* getCenter() const;
        void setCenter(const SFVec3f & c);
        void setCenter(float x, float y, float z);
        void setCenter(const float ac[3]);

        float getRadius() const;
        void setRadius(float r);

        virtual ostream & dump(ostream & ostr) const;
    };


    class OPENVRML_SCOPE AABox : public BVolume {
    public:
        virtual ~AABox();
        virtual int isectFrustum(const VrmlFrustum & f) const;
        virtual void extend(const BVolume & b);
        virtual void extend(const float p[3]);
        virtual void extend(const AABox & b);
        virtual void extend(const BSphere & b);
        virtual void enclose(const float* p, int n);
        virtual bool isMAX() const;
        virtual void setMAX();
        virtual void orthoTransform(const VrmlMatrix & M);
        virtual void transform(const VrmlMatrix & M);
        virtual ostream & dump(ostream & ostr) const;
    };
}

#endif
