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
#ifndef VRMLSFCOLOR_H
#define VRMLSFCOLOR_H

#include "VrmlField.h"


class VrmlSFColor : public VrmlField {
public:

  VrmlSFColor(float r = 1.0, float g = 1.0, float b = 1.0);

  virtual ostream& print(ostream& os) const;

  virtual VrmlField *clone() const;

  virtual VrmlFieldType fieldType() const;
  virtual const VrmlSFColor* toSFColor() const;
  virtual VrmlSFColor* toSFColor();

  float r(void)			{ return d_rgb[0]; }
  float g(void)			{ return d_rgb[1]; }
  float b(void)			{ return d_rgb[2]; }
  float *get()			{ return &d_rgb[0]; }
  void set(float r, float g, float b)
    { d_rgb[0] = r; d_rgb[1] = g; d_rgb[2] = b; }

  static void HSVtoRGB( float h, float s, float v,
			float &r, float &g, float &b);
  static void RGBtoHSV( float r, float g, float b,
			float &h, float &s, float &v);

  void setHSV(float h, float s, float v);
  void getHSV(float &h, float &s, float &v);

private:
  float d_rgb[3];

};

#endif
