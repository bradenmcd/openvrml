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

#ifndef  _VRMLNODETIMESENSOR_
#define  _VRMLNODETIMESENSOR_

#include "VrmlNodeChild.h"
#include "VrmlSFBool.h"
#include "VrmlSFFloat.h"
#include "VrmlSFTime.h"

class VrmlScene;

class VrmlNodeTimeSensor : public VrmlNodeChild {

public:

  // Define the fields of TimeSensor nodes
  static VrmlNodeType *defineType(VrmlNodeType *t = 0);
  virtual VrmlNodeType & nodeType() const;

  VrmlNodeTimeSensor( VrmlScene *scene = 0);
  virtual ~VrmlNodeTimeSensor();

  virtual bool accept(VrmlNodeVisitor & visitor);
  
  virtual VrmlNodeTimeSensor* toTimeSensor() const;

  virtual void addToScene( VrmlScene *s, const char* );

  virtual ostream& printFields(ostream& os, int indent);

  void update( VrmlSFTime &now );

  virtual void eventIn(double timeStamp,
		       const char *eventName,
		       const VrmlField *fieldValue);

  virtual const VrmlField *getField(const char *fieldName) const;
  virtual void setField(const char *fieldName, const VrmlField &fieldValue);

  virtual double getCycleInterval(){ return d_cycleInterval.get();} 
  virtual bool getEnabled(){ return d_enabled.get();} 
  virtual bool getLoop(){ return d_loop.get();} 
  virtual double getStartTime(){ return d_startTime.get();} 
  virtual double getStopTime(){ return d_stopTime.get();} 
  const VrmlBVolume* getBVolume() const;


private:

  // Fields
  VrmlSFTime d_cycleInterval;
  VrmlSFBool d_enabled;
  VrmlSFBool d_loop;
  VrmlSFTime d_startTime;
  VrmlSFTime d_stopTime;

  // Internal state
  VrmlSFBool d_isActive;
  double d_lastTime;
};

#endif _VRMLNODETIMESENSOR_

