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

# include "VrmlField.h"

class VrmlNode;

class VrmlSFNode : public VrmlField {
public:

  VrmlSFNode(VrmlNode * node = 0);
  VrmlSFNode(const VrmlSFNode &n);
  ~VrmlSFNode();

  // Assignment.
  VrmlSFNode& operator=(const VrmlSFNode& rhs);

  virtual ostream& print(ostream& os) const;

  virtual VrmlField *clone() const;

  virtual VrmlFieldType fieldType() const;
  virtual const VrmlSFNode* toSFNode() const;
  virtual VrmlSFNode* toSFNode();

  VrmlNode * get() const;

  void set(VrmlNode * node);

private:
  VrmlNode *d_value;

};

# endif
