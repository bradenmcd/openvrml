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

#ifndef VRMLMFROTATION_H
#define VRMLMFROTATION_H

#include "VrmlField.h"

class VrmlMFRotation : public VrmlField {
public:
    explicit VrmlMFRotation(size_t length = 0, const float * rotations = 0);
    VrmlMFRotation(const VrmlMFRotation & mfrotation);
    
    ~VrmlMFRotation();

    VrmlMFRotation & operator=(const VrmlMFRotation & mfrotation);

    const float * operator[](size_t index) const;
    
    const float * get() const;
    void set(size_t length, const float * rotations);

    size_t getLength() const;
    void setLength(size_t length);

    virtual ostream& print(ostream& os) const;
    virtual VrmlField *clone() const;
    virtual VrmlFieldType fieldType() const;
    virtual const VrmlMFRotation* toMFRotation() const;
    virtual VrmlMFRotation* toMFRotation();
    
private:
    class FData;
    FData *d_data;
};

#endif // VRMLMFROTATION_H
