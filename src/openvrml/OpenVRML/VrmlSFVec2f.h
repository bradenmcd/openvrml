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
        /**
         * Construct an SFVec2f with the default values, (0, 0).
         */
        VrmlSFVec2f();
        
        /**
         * Construct an SFVec2f from a 2-element array.
         */
        VrmlSFVec2f(const float vec[2]);
        
        /**
         * Construct an SFVec2f from two floats.
         */
        VrmlSFVec2f(float x, float y);
        
        float operator[](size_t index) const;
        float & operator[](size_t index);
        
        virtual ostream& print(ostream& os) const;
        
        virtual VrmlField *clone() const;
        
        virtual VrmlFieldType fieldType() const;
        virtual const VrmlSFVec2f* toSFVec2f() const;
        virtual VrmlSFVec2f* toSFVec2f();
        
        /**
         * Get the x component.
         */
        float getX() const;
        
        /**
         * Set the x component.
         */
        void setX(float);
        
        /**
         * Get the y component.
         */
        float getY() const;
        
        /**
         * Set the y component.
         */
        void setY(float);
        
        /**
         * Get the value of this vector as a 2-element array.
         */
        const float * get() const;
        
        /**
         * Set the value of this vector using a 2-element array.
         */
        void set(const float vec[2]);

        /**
         * Returns an SFVec2f with a value that is the passed SFVec2f added,
         * componentwise, to the object.
         */
        const VrmlSFVec2f add(const VrmlSFVec2f & vec) const;
        
        /**
         * Returns an SFVec2f with a value that is the object divided by the
         * passed numeric value.
         */
        const VrmlSFVec2f divide(float number) const;
        
        /**
         * Returns the dot product of this vector and vec.
         */
        double dot(const VrmlSFVec2f & vec) const;
        
        /**
         * Returns the geometric length of this vector.
         */
        double length() const;
        
        /**
         * Returns an SFVec2f with a value that is the object multiplied by the
         * passed numeric value.
         */
        const VrmlSFVec2f multiply(float number) const;
        
        const VrmlSFVec2f negate() const;
        const VrmlSFVec2f normalize() const;
        
        void subtract( VrmlSFVec2f * );

private:
  float d_x[2];

};

#endif
