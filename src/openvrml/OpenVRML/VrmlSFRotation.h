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

#ifndef VRMLSFROTATION_H
#define VRMLSFROTATION_H

#include "VrmlField.h"
#include "VrmlSFVec3f.h"

class VrmlSFRotation : public VrmlField {
    public:
        VrmlSFRotation();
        VrmlSFRotation(const float rotation[4]);
        VrmlSFRotation(float x, float y, float z, float angle);
        VrmlSFRotation(const VrmlSFVec3f & axis, float angle);
        VrmlSFRotation(const VrmlSFVec3f & fromVector,
                       const VrmlSFVec3f & toVector);
        
        float operator[](size_t index) const;
        float & operator[](size_t index);
        
        virtual ostream& print(ostream& os) const;
        
        virtual VrmlField *clone() const;
        
        virtual VrmlFieldType fieldType() const;
        virtual const VrmlSFRotation* toSFRotation() const;
        virtual VrmlSFRotation* toSFRotation();
        
        const float * get() const;
        void set(const float rotation[4]);
        
        float getX() const;
        void setX(float);
        
        float getY() const;
        void setY(float);
        
        float getZ() const;
        void setZ(float);
        
        float getAngle() const;
        void setAngle(float);
        
        /**
         * Get the axis of rotation.
         */
        const VrmlSFVec3f getAxis() const;
        
        /**
         * Get the inverse of this rotation.
         */
        const VrmlSFRotation inverse() const;
        
        /**
         * Return the result of multiplying this rotation by rotation.
         */
        const VrmlSFRotation multiply(const VrmlSFRotation & rotation) const;
        
        /**
         * Returns a vector that is the result of multiplying vec by the matrix
         * corresponding to this rotation.
         */
        const VrmlSFVec3f multVec(const VrmlSFVec3f & vec) const;
        
        /**
         * Set the axis of rotation.
         */
        void setAxis(const VrmlSFVec3f & vec);
        
        /**
         * Perform a Spherical Linear IntERPolation.
         * @param destRotation the destination rotation
         * @param t the interval fraction
         */
        const VrmlSFRotation slerp(const VrmlSFRotation & destRotation,
                                   float t) const;  
        
    private:
        void toQuaternion(float theQuat[4]) const;
        void fromQuaternion(const float theQuat[4]);
        
        float d_x[4];
};

#endif
