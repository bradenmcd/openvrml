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

#ifndef VRMLMFVEC2F_H
#define VRMLMFVEC2F_H

#include "VrmlField.h"

class VrmlMFVec2f : public VrmlField {
public:
    explicit VrmlMFVec2f(size_t length = 0, const float * vecs = 0);
    VrmlMFVec2f(const VrmlMFVec2f & mfvec2f);

    ~VrmlMFVec2f();

    VrmlMFVec2f & operator=(const VrmlMFVec2f & mfvec2f);

    const float * operator[](size_t index) const;
    float * operator[](size_t index);
    
    const float * get() const;
    void set(size_t length, const float * vecs);

    size_t getLength() const;
    void setLength(size_t length);

    virtual VrmlField *clone() const;
    virtual VrmlFieldType fieldType() const;
    virtual ostream& print(ostream& os) const;
    virtual const VrmlMFVec2f* toMFVec2f() const;
    virtual VrmlMFVec2f* toMFVec2f();

private:
    class FData;
    FData * d_data;
};

#endif // VRMLMFVEC2F_H
