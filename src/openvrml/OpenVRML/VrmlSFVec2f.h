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

#ifndef VRMLSFVEC2F
#define VRMLSFVEC2F

#include "VrmlField.h"

class VrmlSFVec2f : public VrmlField {
    public:
        VrmlSFVec2f();
        VrmlSFVec2f(const float vec[2]);
        VrmlSFVec2f(float x, float y);
        
        float operator[](size_t index) const;
        float & operator[](size_t index);
        
        virtual ostream& print(ostream& os) const;
        
        virtual VrmlField *clone() const;
        
        virtual VrmlFieldType fieldType() const;
        virtual const VrmlSFVec2f* toSFVec2f() const;
        virtual VrmlSFVec2f* toSFVec2f();
        
        float getX() const;
        void setX(float);
        
        float getY() const;
        void setY(float);
        
        const float * get() const;
        void set(const float vec[2]);

        const VrmlSFVec2f add(const VrmlSFVec2f & vec) const;
        
        const VrmlSFVec2f divide(float number) const;
        
        double dot(const VrmlSFVec2f & vec) const;
        
        double length() const;
        
        const VrmlSFVec2f multiply(float number) const;
        
        const VrmlSFVec2f negate() const;
        
        const VrmlSFVec2f normalize() const;
        
        const VrmlSFVec2f subtract(const VrmlSFVec2f & vec) const;

    private:
        float d_x[2];
};

#endif
