//
//  Vrml 97 library
//  Copyright (C) 1998 Chris Morley
//
//  VrmlNodeTimeSensor.cpp

#include "VrmlNodeTimeSensor.h"
#include "VrmlNodeType.h"
#include "MathUtils.h"
#include "VrmlScene.h"

// TimeSensor factory. Add each TimeSensor to the scene for fast access.

static VrmlNode *creator( VrmlScene *scene ) 
{
  return new VrmlNodeTimeSensor(scene);
}


// Define the built in VrmlNodeType:: "TimeSensor" fields

VrmlNodeType *VrmlNodeTimeSensor::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;		// Only define the type once.
      t = st = new VrmlNodeType("TimeSensor", creator);
    }

  VrmlNodeChild::defineType(t);	// Parent class
  t->addExposedField("cycleInterval", VrmlField::SFTIME);
  t->addExposedField("enabled", VrmlField::SFBOOL);
  t->addExposedField("loop", VrmlField::SFBOOL);
  t->addExposedField("startTime", VrmlField::SFTIME);
  t->addExposedField("stopTime", VrmlField::SFTIME);
  t->addEventOut("cycleTime", VrmlField::SFTIME);
  t->addEventOut("fraction_changed", VrmlField::SFFLOAT);
  t->addEventOut("isActive", VrmlField::SFBOOL);
  t->addEventOut("time", VrmlField::SFTIME);

  return t;
}


VrmlNodeType & VrmlNodeTimeSensor::nodeType() const
{
    return *defineType(0);
}


VrmlNodeTimeSensor::VrmlNodeTimeSensor( VrmlScene *scene ) :
  VrmlNodeChild(scene),
  d_cycleInterval(1.0),
  d_enabled(true),
  d_loop(false),
  d_startTime(0.0),
  d_stopTime(0.0),
  d_isActive(false),
  d_lastTime(-1.0)
{
  if (d_scene) d_scene->addTimeSensor(this);
}

VrmlNodeTimeSensor::~VrmlNodeTimeSensor()
{
  if (d_scene) d_scene->removeTimeSensor(this);
}

VrmlNode *VrmlNodeTimeSensor::cloneMe() const
{
  return new VrmlNodeTimeSensor(*this);
}


VrmlNodeTimeSensor* VrmlNodeTimeSensor::toTimeSensor() const
{
  return (VrmlNodeTimeSensor*) this;
}

void VrmlNodeTimeSensor::addToScene(VrmlScene *s, const char*)
{ if (d_scene != s && (d_scene = s) != 0) d_scene->addTimeSensor(this); }


ostream& VrmlNodeTimeSensor::printFields(ostream& os, int indent)
{
  if (! FPEQUAL(d_cycleInterval.get(), 1.0))
    PRINT_FIELD(cycleInterval);
  if (! d_enabled.get()) PRINT_FIELD(enabled);
  if (d_loop.get()) PRINT_FIELD(loop);
  if (! FPZERO(d_startTime.get())) PRINT_FIELD(startTime);
  if (! FPZERO(d_stopTime.get())) PRINT_FIELD(stopTime);

  return os;
}


//
// Generate timer events. If necessary, events prior to the timestamp (inTime)
// are generated to respect stopTimes and cycleIntervals. The timestamp
// should never be increased. This assumes the event loop delivers pending
// events in order (ascending time stamps). Should inTime be modified?
// Should ensure continuous events are delivered before discrete ones
// (such as cycleTime, isActive).

void VrmlNodeTimeSensor::update( VrmlSFTime &inTime )
{
  VrmlSFTime timeNow( inTime );

  //printf("VrmlNodeTimeSensor %s ::update( enabled %d, active %d, loop %d )\n",
  //name(), d_enabled.get(), d_isActive.get(), d_loop.get() );

  if (d_enabled.get())
    {
      if ( d_lastTime > inTime.get() )
	d_lastTime = inTime.get();

      // print the results of the tests for activation
      //printf(" startTime <= timeNow: %d\n", d_startTime.get() <= timeNow.get() );
      //printf(" startTime >= d_lastTime: %d\n", d_startTime.get() >= d_lastTime );
      //printf(" stopTime < startTime: %d\n", d_stopTime.get() < d_startTime.get() );
      //printf(" stopTime > timeNow: %d\n", d_stopTime.get() > timeNow.get() );

      // Become active at startTime if either the valid stopTime hasn't
      // passed or we are looping.
      if (! d_isActive.get() &&
	  d_startTime.get() <= timeNow.get() &&
	  d_startTime.get() >= d_lastTime &&
	  ( (d_stopTime.get() < d_startTime.get() ||
	     d_stopTime.get() > timeNow.get()) ||
	    d_loop.get() ))
	{
	  d_isActive.set(true);
	  theSystem->debug("TimeSensor.%s isActive TRUE\n", name());

	  // Start at first tick >= startTime
	  eventOut( timeNow.get(), "isActive", d_isActive );

	  eventOut( timeNow.get(), "time", timeNow );
	  eventOut( timeNow.get(), "fraction_changed", VrmlSFFloat(0.0) );
	  eventOut( timeNow.get(), "cycleTime", timeNow );

	}

      // Running (active and enabled)
      else if ( d_isActive.get() )
	{
	  double f, cycleInt = d_cycleInterval.get();
	  bool deactivate = false;

	  // Are we done? Choose min of stopTime or start + single cycle.
	  if ( (d_stopTime.get() > d_startTime.get() &&
		d_stopTime.get() <= timeNow.get()) ||
	       ( (! d_loop.get()) &&
		 d_startTime.get() + cycleInt <= timeNow.get()) )
	    {
	      d_isActive.set(false);

	      // Must respect stopTime/cycleInterval exactly
	      if (d_startTime.get() + cycleInt < d_stopTime.get())
		timeNow = d_startTime.get() + cycleInt;
	      else
		timeNow = d_stopTime;

	      deactivate = true;
	    }

	  if (cycleInt > 0.0 && timeNow.get() > d_startTime.get())
	    f = fmod( timeNow.get() - d_startTime.get(), cycleInt );
	  else
	    f = 0.0;

	  // Fraction of cycle message
	  VrmlSFFloat fraction_changed( FPZERO(f) ? 1.0 : (f / cycleInt) );
	  eventOut( timeNow.get(), "fraction_changed", fraction_changed );

	  // Current time message
	  eventOut( timeNow.get(), "time", timeNow );

	  // End of cycle message (this may miss cycles...)
	  if ( FPEQUAL(fraction_changed.get(), 1.0) )
	    eventOut( timeNow.get(), "cycleTime", timeNow );

	  if (deactivate)
	    eventOut( timeNow.get(), "isActive", d_isActive );
	}

      // Tell the scene this node needs quick updates while it is active.
      // Should check whether time, fraction_changed eventOuts are
      // being used, and set delta to cycleTime if not...
#ifdef macintosh
      if (d_isActive.get()) d_scene->setDelta( 0.001 ); //0.0 is too fast(!)
#else
      if (d_isActive.get()) d_scene->setDelta( 0.0 );
#endif
      d_lastTime = inTime.get();

    }
}

// Ignore set_cycleInterval & set_startTime when active, deactivate
// if set_enabled FALSE is received when active.

void VrmlNodeTimeSensor::eventIn(double timeStamp,
				 const char *eventName,
				 const VrmlField *fieldValue)
{
  const char *origEventName = eventName;
  if ( strncmp(eventName, "set_", 4) == 0 )
    eventName += 4;

  //theSystem->debug("TimeSensor.%s eventIn %s\n", name(), origEventName);

  // Ignore set_cycleInterval & set_startTime when active
  if ( strcmp(eventName,"cycleInterval") == 0 ||
       strcmp(eventName,"startTime") == 0 )
    {
      if (! d_isActive.get())
	{
	  d_lastTime = timeStamp;
	  setField(eventName, *fieldValue);
	  char eventOutName[256];
	  strcpy(eventOutName, eventName);
	  strcat(eventOutName, "_changed");
	  eventOut(timeStamp, eventOutName, *fieldValue);
	}
    }

  // Shutdown if set_enabled FALSE is received when active
  else if ( strcmp(eventName, "enabled") == 0 )
    {
      setField(eventName, *fieldValue);
      if ( d_isActive.get() && ! d_enabled.get() )
	{
	  d_isActive.set(false);

	  // Send relevant eventOuts (continuous ones first)
	  VrmlSFTime timeNow( timeStamp );
	  eventOut(timeStamp, "time", timeNow);

	  double f, cycleInt = d_cycleInterval.get();
	  if (cycleInt > 0.0)
	    f = fmod( timeNow.get() - d_startTime.get(), cycleInt );
	  else
	    f = 0.0;

	  // Fraction of cycle message
	  VrmlSFFloat fraction_changed( FPZERO(f) ? 1.0 : (f / cycleInt) );

	  eventOut(timeStamp, "fraction_changed", fraction_changed);
	  eventOut(timeStamp, "isActive", d_isActive);
	}

      eventOut(timeStamp, "enabled_changed", *fieldValue);

    }

  // Let the generic code handle the rest.
  else
    VrmlNode::eventIn( timeStamp, origEventName, fieldValue );

  // TimeSensors shouldn't generate redraws.
  clearModified();
}

// Get the value of a field or eventOut.

const VrmlField *VrmlNodeTimeSensor::getField(const char *fieldName) const
{
  // exposedFields
  if ( strcmp( fieldName, "cycleInterval" ) == 0 )
    return &d_cycleInterval;
  else if ( strcmp( fieldName, "enabled" ) == 0 )
    return &d_enabled;
  else if ( strcmp( fieldName, "loop" ) == 0 )
    return &d_loop;
  else if ( strcmp( fieldName, "startTime" ) == 0 )
    return &d_startTime;
  else if ( strcmp( fieldName, "stopTime" ) == 0 )
    return &d_stopTime;

  // eventOuts
  else if ( strcmp( fieldName, "isActive" ) == 0 )
    return &d_isActive;
#if 0				// not implemented yet...
  else if ( strcmp( fieldName, "cycleTime" ) == 0 )
    return &d_cycleTime;
  else if ( strcmp( fieldName, "fraction" ) == 0 )
    return &d_fraction;
  else if ( strcmp( fieldName, "time" ) == 0 )
    return &d_time;
#endif

  return VrmlNodeChild::getField(fieldName); // Parent class
}

// Set the value of one of the node fields.

void VrmlNodeTimeSensor::setField(const char *fieldName,
				  const VrmlField &fieldValue)
{
  if TRY_FIELD(cycleInterval, SFTime)
  else if TRY_FIELD(enabled, SFBool)
  else if TRY_FIELD(loop, SFBool)
  else if TRY_FIELD(startTime, SFTime)
  else if TRY_FIELD(stopTime, SFTime)
  else
    VrmlNodeChild::setField(fieldName, fieldValue);
}


const VrmlBVolume* VrmlNodeTimeSensor::getBVolume() const
{
  //cout << "VrmlNodeTimeSensor::getBVolume():unset" << endl;
  static VrmlBSphere* inf_bsphere = (VrmlBSphere*)0;
  if (!inf_bsphere) {
    inf_bsphere = new VrmlBSphere();
  }
  return inf_bsphere;
}
