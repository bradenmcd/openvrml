//

//  Vrml 97 library

//  Copyright (C) 1998 Chris Morley

//

//  %W% %G%

//  VrmlNodeWorldInfo.h



#ifndef  _VRMLNODEWORLDINFO_

#define  _VRMLNODEWORLDINFO_



#include "VrmlNodeChild.h"

#include "VrmlMFString.h"

#include "VrmlSFString.h"





class VrmlNodeWorldInfo : public VrmlNodeChild {



public:



  // Define the built in VrmlNodeType:: "WorldInfo"

  static VrmlNodeType *defineType(VrmlNodeType *t = 0);

  virtual VrmlNodeType *nodeType() const;



  VrmlNodeWorldInfo(VrmlScene *);

  virtual ~VrmlNodeWorldInfo();



  virtual VrmlNode *cloneMe() const;



  virtual ostream& printFields(ostream& os, int indent);



  virtual void setField(const char *fieldName, const VrmlField &fieldValue);



  const char* title() { return d_title.get(); }

  const char** info() { return d_info.get(); }

  const int    size() { return d_info.size(); }



private:



  VrmlMFString d_info;

  VrmlSFString d_title;

  

};



#endif _VRMLNODEWORLDINFO_



