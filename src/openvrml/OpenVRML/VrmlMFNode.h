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

  VrmlMFNode();
  VrmlMFNode(VrmlNode *);
  VrmlMFNode(size_t n, VrmlNode * const * v);
  VrmlMFNode(VrmlMFNode const &);

  ~VrmlMFNode();

  virtual ostream& print(ostream& os) const;

  // Assignment. Since the nodes themselves are ref counted,
  // I don't bother trying to share the NodeLists.
  VrmlMFNode& operator=(const VrmlMFNode& rhs);

  virtual VrmlField *clone() const;

  virtual VrmlFieldType fieldType() const;
  virtual const VrmlMFNode* toMFNode() const;
  virtual VrmlMFNode* toMFNode();

  size_t size() const			{ return d_size; }
  VrmlNode ** get()			{ return d_v; }
  VrmlNode* get(size_t index)		{ return d_v[index]; }

  // can't use this as lhs for now.
  VrmlNode* operator[](size_t index) const	{ return d_v[index]; }

  bool exists(VrmlNode const * n);

  void addNode(VrmlNode * n);
  void removeNode(VrmlNode * n);


private:

  VrmlNode **d_v;
  size_t d_allocated;
  size_t d_size;

};

# endif
