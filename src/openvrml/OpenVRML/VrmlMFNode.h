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

# ifndef VRMLMFNODE_H
#   define VRMLMFNODE_H
#   include "VrmlField.h"
#   include <stddef.h>

class VrmlNode;

class VrmlMFNode : public VrmlField {
public:
    explicit VrmlMFNode(size_t length = 0, VrmlNode * const * nodes = 0);
    VrmlMFNode(VrmlMFNode const & mfnode);

    ~VrmlMFNode();

    VrmlMFNode& operator=(const VrmlMFNode & mfnode);

    VrmlNode * operator[](size_t index) const;
    VrmlNode * & operator[](size_t index);

    size_t getLength() const;
    void setLength(size_t length);

    bool exists(const VrmlNode & node) const;

    bool addNode(VrmlNode * node);
    bool removeNode(VrmlNode * node);

    virtual ostream& print(ostream& os) const;
    virtual VrmlField *clone() const;
    virtual VrmlFieldType fieldType() const;
    virtual const VrmlMFNode* toMFNode() const;
    virtual VrmlMFNode* toMFNode();

private:
    void realloc(size_t newSize);
    
    VrmlNode * * d_v;
    size_t d_allocated;
    size_t d_size;
};

# endif
