//
//  Vrml 97 library
//  Copyright (C) 1998 Chris Morley
//
# ifndef VRMLSFNODE_H
# define VRMLSFNODE_H

# include "VrmlField.h"

class VrmlNode;

class VrmlSFNode : public VrmlField {
public:

  VrmlSFNode(VrmlNode *value = 0);
  VrmlSFNode(const VrmlSFNode &n);
  ~VrmlSFNode();

  // Assignment.
  VrmlSFNode& operator=(const VrmlSFNode& rhs);

  virtual ostream& print(ostream& os) const;

  virtual VrmlField *clone() const;

  virtual VrmlFieldType fieldType() const;
  virtual const VrmlSFNode* toSFNode() const;
  virtual VrmlSFNode* toSFNode();

  VrmlNode *get(void) const		{ return d_value; }

  void set(VrmlNode *value);

private:
  VrmlNode *d_value;

};

# endif
