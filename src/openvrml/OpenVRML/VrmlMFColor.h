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

#ifndef VRMLMFCOLOR_H
#define VRMLMFCOLOR_H

#include "VrmlField.h"

class VrmlMFColor : public VrmlField {
public:
    explicit VrmlMFColor(size_t length = 0, float const * colors = 0);
    VrmlMFColor(const VrmlMFColor &source);

    ~VrmlMFColor();

    VrmlMFColor& operator=(const VrmlMFColor & mfColor);

    const float * operator[](size_t index) const;

    const float * get() const;
    void set(size_t length, const float * colors = 0);

    size_t getLength() const;
    void setLength(size_t length);

    //
    // VrmlField implementation
    //
    virtual ostream& print(ostream& os) const;
    virtual VrmlField *clone() const;
    virtual VrmlFieldType fieldType() const;
    virtual const VrmlMFColor* toMFColor() const;
    virtual VrmlMFColor* toMFColor();

private:
    class FData;
    FData *d_data;		// Color data in RGB triples
};

#endif // VRMLMFCOLOR_H
