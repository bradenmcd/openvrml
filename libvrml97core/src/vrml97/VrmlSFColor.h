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

#ifndef VRMLSFCOLOR_H
#define VRMLSFCOLOR_H

#include "VrmlField.h"

class VrmlSFColor : public VrmlField {
    public:
        VrmlSFColor();
        VrmlSFColor(const float rgb[3]);
        VrmlSFColor(float r, float g, float b);
        
        float operator[](size_t index) const;
        float & operator[](size_t index);
        
        virtual ostream& print(ostream& os) const;
        
        virtual VrmlField *clone() const;
        
        virtual VrmlFieldType fieldType() const;
        virtual const VrmlSFColor* toSFColor() const;
        virtual VrmlSFColor* toSFColor();
        
        float getR() const;
        float getG() const;
        float getB() const;
        const float * get() const;
        void set(const float rgb[3]);
        
        static void HSVtoRGB(const float hsv[3], float rgb[3]);
        static void RGBtoHSV(const float rgb[3], float hsv[3]);
        
        void setHSV(float h, float s, float v);
        void getHSV(float hsv[3]) const;
        
    private:
        float d_rgb[3];
};

#endif
