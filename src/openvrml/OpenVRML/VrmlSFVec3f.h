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

#ifndef VRMLSFVEC3F_H
#define VRMLSFVEC3F_H

#include "VrmlField.h"

class VrmlSFVec3f : public VrmlField {
    public:
        VrmlSFVec3f();
        VrmlSFVec3f(const float vec[3]);
        VrmlSFVec3f(float x, float y, float z);
        
        float operator[](size_t index) const;
        float & operator[](size_t index);
        
        virtual ostream& print(ostream& os) const;
        
        virtual VrmlField * clone() const;
        
        virtual VrmlFieldType fieldType() const;
        virtual const VrmlSFVec3f* toSFVec3f() const;
        virtual VrmlSFVec3f* toSFVec3f();
        
        float getX() const;
        void setX(float);
        
        float getY() const;
        void setY(float);
        
        float getZ() const;
        void setZ(float);
        
        const float * get() const;
        void set(const float vec[3]);
        
        const VrmlSFVec3f add(const VrmlSFVec3f & vec) const;
        
        const VrmlSFVec3f cross(const VrmlSFVec3f & vec) const;
        
        const VrmlSFVec3f divide(float number) const;
        
        double dot(const VrmlSFVec3f & vec) const;
        
        double length() const;
        
        const VrmlSFVec3f multiply(float number) const;
        
        const VrmlSFVec3f negate() const;
        
        const VrmlSFVec3f normalize() const;
        
        const VrmlSFVec3f subtract(const VrmlSFVec3f & vec) const;
        
    private:
        float d_x[3];
};

#endif
