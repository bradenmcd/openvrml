//
//  Vrml 97 library
//  Copyright (C) 1998 Chris Morley
//  See the file COPYING for license details.
//
# ifndef VRMLMFNODE_H
#   define VRMLMFNODE_H

#   include "VrmlField.h"
#   include <stddef.h>

class VrmlNode;

class VrmlMFNode : public VrmlMField {
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
