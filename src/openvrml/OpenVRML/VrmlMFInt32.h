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

#ifndef VRMLMFINT_H
#define VRMLMFINT_H

#include "VrmlField.h"

class VrmlMFInt32 : public VrmlField {
public:
    explicit VrmlMFInt32(size_t length = 0, const long * numbers = 0);
    VrmlMFInt32(const VrmlMFInt32 & mfInt32);

    ~VrmlMFInt32();

    VrmlMFInt32 & operator=(const VrmlMFInt32 & mfInt32);

    long operator[](size_t) const;

    const long * get() const;
    void set(size_t length, const long * numbers);

    size_t getLength() const;
    void setLength(size_t length);

    virtual ostream& print(ostream& os) const;
    virtual VrmlField *clone() const;
    virtual VrmlFieldType fieldType() const;
    virtual const VrmlMFInt32 * toMFInt32() const;
    virtual VrmlMFInt32 * toMFInt32();


private:
    class IData;
    IData *d_data;
};

#endif // VRMLMFINT_H
