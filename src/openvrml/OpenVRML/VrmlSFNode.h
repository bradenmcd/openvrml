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

# ifndef VRMLSFNODE_H
# define VRMLSFNODE_H
#   include "VrmlField.h"
#   include "VrmlNodePtr.h"

class VrmlNode;

class VrmlSFNode : public VrmlField {
    
    VrmlNodePtr node;

public:
    VrmlSFNode(const VrmlNodePtr & node = VrmlNodePtr(0));
    VrmlSFNode(const VrmlSFNode & sfnode);
    ~VrmlSFNode();

    VrmlSFNode & operator=(const VrmlSFNode & sfnode);

    const VrmlNodePtr & get() const;
    void set(const VrmlNodePtr & node);

    virtual ostream& print(ostream& os) const;
    virtual VrmlField *clone() const;
    virtual VrmlFieldType fieldType() const;
    virtual const VrmlSFNode* toSFNode() const;
    virtual VrmlSFNode* toSFNode();
};

# endif
