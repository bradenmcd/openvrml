//
// OpenVRML
//
// Copyright (C) 1998  Chris Morley
// Copyright (C) 2001  Braden McDaniel
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

# ifndef OPENVRML_FIELD_H
#   define OPENVRML_FIELD_H

#   include <string>
#   include <vector>
#   include <iostream.h>
#   include "common.h"
#   include "VrmlNodePtr.h"

class VrmlSFBool;
class VrmlSFColor;
class VrmlSFFloat;
class VrmlSFImage;
class VrmlSFInt32;
class VrmlSFNode;
class VrmlSFRotation;
class VrmlSFString;
class VrmlSFTime;
class VrmlSFVec2f;
class VrmlSFVec3f;
class VrmlMFColor;
class VrmlMFFloat;
class VrmlMFInt32;
class VrmlMFNode;
class VrmlMFRotation;
class VrmlMFString;
class VrmlMFVec2f;
class VrmlMFVec3f;

class OPENVRML_SCOPE VrmlField {
    friend ostream & operator<<(ostream &, const VrmlField &);
public:
    enum VrmlFieldType {
        NO_FIELD, SFBOOL, SFCOLOR, SFFLOAT, SFIMAGE, SFINT32, SFNODE,
        SFROTATION, SFSTRING, SFTIME, SFVEC2F, SFVEC3F, MFCOLOR, MFFLOAT,
        MFINT32, MFNODE, MFROTATION, MFSTRING, MFTIME, MFVEC2F, MFVEC3F
    };

    static VrmlFieldType fieldType(const char * fieldTypeId);
    
    const char * fieldTypeName() const;
    
    virtual ~VrmlField() = 0;
    virtual VrmlField * clone() const = 0;
    virtual ostream& print(ostream& os) const = 0;
    virtual VrmlFieldType fieldType() const = 0;

    // safe downcasts, const and non-const versions.
    // These avoid casts of VrmlField* but are ugly in that this class
    // must know of the existence of all of its subclasses...
    virtual const VrmlSFBool *toSFBool() const;
    virtual const VrmlSFColor *toSFColor() const;
    virtual const VrmlSFFloat *toSFFloat() const;
    virtual const VrmlSFImage *toSFImage() const;
    virtual const VrmlSFInt32 *toSFInt32() const;
    virtual const VrmlSFNode *toSFNode() const;
    virtual const VrmlSFRotation *toSFRotation() const;
    virtual const VrmlSFString *toSFString() const;
    virtual const VrmlSFTime *toSFTime() const;
    virtual const VrmlSFVec2f *toSFVec2f() const;
    virtual const VrmlSFVec3f *toSFVec3f() const;
    virtual const VrmlMFColor *toMFColor() const;
    virtual const VrmlMFFloat *toMFFloat() const;
    virtual const VrmlMFInt32 *toMFInt32() const;
    virtual const VrmlMFNode *toMFNode() const;
    virtual const VrmlMFRotation *toMFRotation() const;
    virtual const VrmlMFString *toMFString() const;
    virtual const VrmlMFVec2f *toMFVec2f() const;
    virtual const VrmlMFVec3f *toMFVec3f() const;
    virtual VrmlSFBool *toSFBool();
    virtual VrmlSFColor *toSFColor();
    virtual VrmlSFFloat *toSFFloat();
    virtual VrmlSFImage *toSFImage();
    virtual VrmlSFInt32 *toSFInt32();
    virtual VrmlSFNode *toSFNode();
    virtual VrmlSFRotation *toSFRotation();
    virtual VrmlSFString *toSFString();
    virtual VrmlSFTime *toSFTime();
    virtual VrmlSFVec2f *toSFVec2f();
    virtual VrmlSFVec3f *toSFVec3f();
    virtual VrmlMFColor *toMFColor();
    virtual VrmlMFFloat *toMFFloat();
    virtual VrmlMFInt32 *toMFInt32();
    virtual VrmlMFNode *toMFNode();
    virtual VrmlMFRotation *toMFRotation();
    virtual VrmlMFString *toMFString();
    virtual VrmlMFVec2f *toMFVec2f();
    virtual VrmlMFVec3f *toMFVec3f();
};


class OPENVRML_SCOPE VrmlSFBool : public VrmlField {
    bool d_value;
public:
    explicit VrmlSFBool(bool value = false);
    virtual ~VrmlSFBool();

    bool get() const;
    void set(bool value);

    virtual ostream& print(ostream& os) const;
    virtual VrmlField *clone() const;
    virtual VrmlFieldType fieldType() const;
    virtual const VrmlSFBool* toSFBool() const;
    virtual VrmlSFBool* toSFBool();
};


class OPENVRML_SCOPE VrmlSFColor : public VrmlField {
    float d_rgb[3];
public:
    static void HSVtoRGB(const float hsv[3], float rgb[3]);
    static void RGBtoHSV(const float rgb[3], float hsv[3]);

    VrmlSFColor();
    explicit VrmlSFColor(const float rgb[3]);
    VrmlSFColor(float r, float g, float b);
    virtual ~VrmlSFColor();

    float operator[](size_t index) const;
    float & operator[](size_t index);
    float getR() const;
    float getG() const;
    float getB() const;
    const float (&get() const)[3];
    void set(const float rgb[3]);
    void setHSV(float h, float s, float v);
    void getHSV(float hsv[3]) const;

    virtual ostream& print(ostream& os) const;
    virtual VrmlField *clone() const;
    virtual VrmlFieldType fieldType() const;
    virtual const VrmlSFColor* toSFColor() const;
    virtual VrmlSFColor* toSFColor();
};


class OPENVRML_SCOPE VrmlSFFloat : public VrmlField {
    float d_value;
public:
    explicit VrmlSFFloat(float value = 0.0);
    virtual ~VrmlSFFloat();

    float get() const;
    void set(float value);

    virtual ostream& print(ostream& os) const;
    virtual VrmlField *clone() const;
    virtual VrmlFieldType fieldType() const;
    virtual const VrmlSFFloat* toSFFloat() const;
    virtual VrmlSFFloat* toSFFloat();
};


class OPENVRML_SCOPE VrmlSFImage : public VrmlField {
    size_t d_w, d_h, d_nc;
    unsigned char * d_pixels;	// nc bytes/pixel, lower left to upper right
public:
    VrmlSFImage();
    VrmlSFImage(size_t w, size_t h, size_t nc, const unsigned char * pixels = 0);
    VrmlSFImage(const VrmlSFImage&);
    virtual ~VrmlSFImage();

    VrmlSFImage & operator=(const VrmlSFImage & rhs);

    size_t getWidth() const;
    size_t getHeight() const;
    size_t getComponents() const;
    const unsigned char * getPixels() const;
    void set(size_t width, size_t height, size_t components,
             const unsigned char * pixels);

    virtual ostream& print(ostream& os) const;
    virtual VrmlField *clone() const;
    virtual VrmlFieldType fieldType() const;
    virtual const VrmlSFImage* toSFImage() const;
    virtual VrmlSFImage* toSFImage();
};


class OPENVRML_SCOPE VrmlSFInt32 : public VrmlField {
    long d_value;
public:
    explicit VrmlSFInt32(long = 0);
    virtual ~VrmlSFInt32();

    long get() const;
    void set(long);

    virtual ostream& print(ostream& os) const;
    virtual VrmlField *clone() const;
    virtual VrmlFieldType fieldType() const;
    virtual const VrmlSFInt32* toSFInt32() const;
    virtual VrmlSFInt32* toSFInt32();
};


class OPENVRML_SCOPE VrmlSFNode : public VrmlField {
    VrmlNodePtr node;
public:
    explicit VrmlSFNode(const VrmlNodePtr & node = VrmlNodePtr(0));
    VrmlSFNode(const VrmlSFNode & sfnode);
    virtual ~VrmlSFNode();

    VrmlSFNode & operator=(const VrmlSFNode & sfnode);

    const VrmlNodePtr & get() const;
    void set(const VrmlNodePtr & node);

    virtual ostream& print(ostream& os) const;
    virtual VrmlField *clone() const;
    virtual VrmlFieldType fieldType() const;
    virtual const VrmlSFNode* toSFNode() const;
    virtual VrmlSFNode* toSFNode();
};


class OPENVRML_SCOPE VrmlSFRotation : public VrmlField {
    float d_x[4];

public:
    VrmlSFRotation();
    explicit VrmlSFRotation(const float rot[4]);
    VrmlSFRotation(float x, float y, float z, float angle);
    VrmlSFRotation(const VrmlSFVec3f & axis, float angle);
    VrmlSFRotation(const VrmlSFVec3f & fromVec, const VrmlSFVec3f & toVec);
    virtual ~VrmlSFRotation();

//    float operator[](size_t index) const;
//    float & operator[](size_t index);
    const float (&get() const)[4];
    void set(const float rot[4]);
    float getX() const;
    void setX(float);
    float getY() const;
    void setY(float);
    float getZ() const;
    void setZ(float);
    float getAngle() const;
    void setAngle(float);
    const VrmlSFVec3f getAxis() const;
    void setAxis(const VrmlSFVec3f & vec);
    const VrmlSFRotation inverse() const;
    const VrmlSFRotation multiply(const VrmlSFRotation & rot) const;
    const VrmlSFVec3f multVec(const VrmlSFVec3f & vec) const;
    const VrmlSFRotation slerp(const VrmlSFRotation & destRot,
                               float t) const;  

    virtual ostream& print(ostream& os) const;
    virtual VrmlField *clone() const;
    virtual VrmlFieldType fieldType() const;
    virtual const VrmlSFRotation* toSFRotation() const;
    virtual VrmlSFRotation* toSFRotation();

private:
    void toQuaternion(float theQuat[4]) const;
    void fromQuaternion(const float theQuat[4]);
};


class OPENVRML_SCOPE VrmlSFString : public VrmlField {
    std::string value;

public:
    explicit VrmlSFString(const std::string & value = std::string());
    virtual ~VrmlSFString();

    // Use compiler-defined copy ctor and operator=.

    const std::string & get() const;
    void set(const std::string & value);

    virtual ostream& print(ostream& os) const;
    virtual VrmlField *clone() const;
    virtual VrmlFieldType fieldType() const;
    virtual const VrmlSFString* toSFString() const;
    virtual VrmlSFString* toSFString();
};


class OPENVRML_SCOPE VrmlSFTime : public VrmlField {
    double d_value;
public:
    explicit VrmlSFTime(double value = 0.0);
    virtual ~VrmlSFTime();

    double get() const;
    void set(double value);

    virtual ostream& print(ostream& os) const;
    virtual VrmlField *clone() const;
    virtual VrmlFieldType fieldType() const;
    virtual const VrmlSFTime* toSFTime() const;
    virtual VrmlSFTime* toSFTime();
};


class OPENVRML_SCOPE VrmlSFVec2f : public VrmlField {
    float d_x[2];
public:
    VrmlSFVec2f();
    explicit VrmlSFVec2f(const float vec[2]);
    VrmlSFVec2f(float x, float y);
    virtual ~VrmlSFVec2f();

    float operator[](size_t index) const;
    float & operator[](size_t index);
    float getX() const;
    void setX(float);
    float getY() const;
    void setY(float);
    const float (&get() const)[2];
    void set(const float vec[2]);
    const VrmlSFVec2f add(const VrmlSFVec2f & vec) const;
    const VrmlSFVec2f divide(float number) const;
    double dot(const VrmlSFVec2f & vec) const;
    double length() const;
    const VrmlSFVec2f multiply(float number) const;
    const VrmlSFVec2f negate() const;
    const VrmlSFVec2f normalize() const;
    const VrmlSFVec2f subtract(const VrmlSFVec2f & vec) const;

    virtual ostream& print(ostream& os) const;
    virtual VrmlField *clone() const;
    virtual VrmlFieldType fieldType() const;
    virtual const VrmlSFVec2f* toSFVec2f() const;
    virtual VrmlSFVec2f* toSFVec2f();
};


class OPENVRML_SCOPE VrmlSFVec3f : public VrmlField {
    float d_x[3];
public:
    VrmlSFVec3f();
    explicit VrmlSFVec3f(const float vec[3]);
    VrmlSFVec3f(float x, float y, float z);
    virtual ~VrmlSFVec3f();

    float operator[](size_t index) const;
    float & operator[](size_t index);
    float getX() const;
    void setX(float);
    float getY() const;
    void setY(float);
    float getZ() const;
    void setZ(float);
    const float (&get() const)[3];
    void set(const float vec[3]);
    const VrmlSFVec3f add(const VrmlSFVec3f & vec) const;
    const VrmlSFVec3f cross(const VrmlSFVec3f & vec) const;
    const VrmlSFVec3f divide(float number) const;
    double dot(const VrmlSFVec3f & vec) const;
    double length() const;
    const VrmlSFVec3f multiply(float number) const;
    const VrmlSFVec3f negate() const;
    const VrmlSFVec3f normalize() const;
    const VrmlSFVec3f subtract(const VrmlSFVec3f & vec) const;

    virtual ostream& print(ostream& os) const;
    virtual VrmlField * clone() const;
    virtual VrmlFieldType fieldType() const;
    virtual const VrmlSFVec3f* toSFVec3f() const;
    virtual VrmlSFVec3f* toSFVec3f();
};


class OPENVRML_SCOPE VrmlMFColor : public VrmlField {
    class FData;
    FData * d_data;
public:
    explicit VrmlMFColor(size_t length = 0, float const * colors = 0);
    VrmlMFColor(const VrmlMFColor &source);
    virtual ~VrmlMFColor();

    VrmlMFColor & operator=(const VrmlMFColor & mfColor);

    const float * get() const;
    void set(size_t length, const float * colors = 0);
    const float * getElement(size_t index) const;
    void setElement(size_t index, const float value[3]);
    size_t getLength() const;
    void setLength(size_t length);
    void insertElement(size_t index, const float value[3]);
    void removeElement(size_t index);

    //
    // VrmlField implementation
    //
    virtual ostream& print(ostream& os) const;
    virtual VrmlField *clone() const;
    virtual VrmlFieldType fieldType() const;
    virtual const VrmlMFColor* toMFColor() const;
    virtual VrmlMFColor* toMFColor();
};


class OPENVRML_SCOPE VrmlMFFloat : public VrmlField {
    class FData;
    FData * d_data;
public:
    explicit VrmlMFFloat(size_t length = 0, float const * numbers = 0);
    VrmlMFFloat(const VrmlMFFloat & mfFloat);
    virtual ~VrmlMFFloat();

    VrmlMFFloat& operator=(const VrmlMFFloat & mfFloat);

    const float * get() const;
    void set(size_t length, const float * numbers);
    float getElement(size_t index) const;
    void setElement(size_t index, float value);
    size_t getLength() const;
    void setLength(size_t length);
    void insertElement(size_t index, float value);
    void removeElement(size_t index);

    //
    // VrmlField implementation
    //
    virtual ostream& print(ostream& os) const;
    virtual VrmlField *clone() const;
    virtual VrmlFieldType fieldType() const;
    virtual const VrmlMFFloat* toMFFloat() const;
    virtual VrmlMFFloat* toMFFloat();
};


class OPENVRML_SCOPE VrmlMFInt32 : public VrmlField {
    class IData;
    IData *d_data;
public:
    explicit VrmlMFInt32(size_t length = 0, const long * numbers = 0);
    VrmlMFInt32(const VrmlMFInt32 & mfInt32);
    virtual ~VrmlMFInt32();

    VrmlMFInt32 & operator=(const VrmlMFInt32 & mfInt32);

    const long * get() const;
    void set(size_t length, const long * numbers);
    long getElement(size_t index) const;
    void setElement(size_t index, long value);
    size_t getLength() const;
    void setLength(size_t length);
    void insertElement(size_t index, long value);
    void removeElement(size_t index);

    //
    // VrmlField implementation
    //
    virtual ostream& print(ostream& os) const;
    virtual VrmlField *clone() const;
    virtual VrmlFieldType fieldType() const;
    virtual const VrmlMFInt32 * toMFInt32() const;
    virtual VrmlMFInt32 * toMFInt32();
};


class OPENVRML_SCOPE VrmlMFNode : public VrmlField {
    std::vector<VrmlNodePtr> nodes;

public:
    explicit VrmlMFNode(size_t length = 0, const VrmlNodePtr * nodes = 0);
    virtual ~VrmlMFNode();

    // Use compiler-defined copy ctor and operator=

    const VrmlNodePtr & getElement(size_t index) const throw ();
    void setElement(size_t index, const VrmlNodePtr & node);
    size_t getLength() const throw ();
    void setLength(size_t length);
    bool exists(const VrmlNode & node) const;
    bool addNode(VrmlNode & node);
    bool removeNode(const VrmlNode & node);
    void insertElement(size_t index, const VrmlNodePtr & node);
    void removeElement(size_t index);
    void clear();

    virtual ostream& print(ostream& os) const;
    virtual VrmlField *clone() const;
    virtual VrmlFieldType fieldType() const;
    virtual const VrmlMFNode* toMFNode() const;
    virtual VrmlMFNode* toMFNode();
};


class OPENVRML_SCOPE VrmlMFRotation : public VrmlField {
    class FData;
    FData *d_data;
public:
    explicit VrmlMFRotation(size_t length = 0, const float * rotations = 0);
    VrmlMFRotation(const VrmlMFRotation & mfrotation);
    virtual ~VrmlMFRotation();

    VrmlMFRotation & operator=(const VrmlMFRotation & mfrotation);

    const float * get() const;
    void set(size_t length, const float * rotations);
    const float * getElement(size_t index) const;
    void setElement(size_t, const float value[4]);
    size_t getLength() const;
    void setLength(size_t length);
    void insertElement(size_t index, const float value[3]);
    void removeElement(size_t index);

    virtual ostream& print(ostream& os) const;
    virtual VrmlField *clone() const;
    virtual VrmlFieldType fieldType() const;
    virtual const VrmlMFRotation* toMFRotation() const;
    virtual VrmlMFRotation* toMFRotation();
};


class OPENVRML_SCOPE VrmlMFString : public VrmlField {
    std::vector<std::string> values;

public:
    explicit VrmlMFString(size_t length = 0, const std::string * values = 0);
    virtual ~VrmlMFString();

    // Use compiler-defined copy ctor and operator=.

    const std::string & getElement(size_t index) const;
    void setElement(size_t index, const std::string & value);
    size_t getLength() const;
    void insertElement(size_t index, const std::string & value);
    void removeElement(size_t index);

    virtual VrmlField *clone() const;
    virtual VrmlFieldType fieldType() const;
    virtual const VrmlMFString* toMFString() const;
    virtual VrmlMFString* toMFString();
    virtual ostream& print(ostream& os) const;
};


class OPENVRML_SCOPE VrmlMFTime : public VrmlField {
    class DData;
    DData * d_data;
public:
    explicit VrmlMFTime(size_t length = 0, const double * times = 0);
    VrmlMFTime(const VrmlMFTime & mftime);
    virtual ~VrmlMFTime();

    VrmlMFTime & operator=(const VrmlMFTime & mftime);

    const double * get() const;
    void set(size_t length, const double * times);
    double getElement(size_t index) const;
    void setElement(size_t index, double value);
    size_t getLength() const;
    void setLength(size_t length);
    void insertElement(size_t index, double value);
    void removeElement(size_t index);

    //
    // Override from VrmlField
    //
    virtual ostream & print(ostream &) const;
    virtual VrmlField * clone() const;
    virtual VrmlFieldType fieldType() const;
};


class OPENVRML_SCOPE VrmlMFVec2f : public VrmlField {
    class FData;
    FData * d_data;
public:
    explicit VrmlMFVec2f(size_t length = 0, const float * vecs = 0);
    VrmlMFVec2f(const VrmlMFVec2f & mfvec2f);
    virtual ~VrmlMFVec2f();

    VrmlMFVec2f & operator=(const VrmlMFVec2f & mfvec2f);

    const float * get() const;
    void set(size_t length, const float * vecs);
    const float * getElement(size_t index) const;
    void setElement(size_t, const float value[2]);
    size_t getLength() const;
    void setLength(size_t length);
    void insertElement(size_t index, const float value[2]);
    void removeElement(size_t index);

    virtual VrmlField *clone() const;
    virtual VrmlFieldType fieldType() const;
    virtual ostream& print(ostream& os) const;
    virtual const VrmlMFVec2f* toMFVec2f() const;
    virtual VrmlMFVec2f* toMFVec2f();
};


class OPENVRML_SCOPE VrmlMFVec3f : public VrmlField {
    class FData;
    FData * d_data;
public:
    explicit VrmlMFVec3f(size_t length = 0, const float * vecs = 0);
    VrmlMFVec3f(const VrmlMFVec3f & mfvec3f);
    virtual ~VrmlMFVec3f();

    VrmlMFVec3f & operator=(const VrmlMFVec3f & mfvec3f);

    const float * get() const;
    void set(size_t length, const float * vecs);
    const float * getElement(size_t index) const;
    void setElement(size_t, const float value[3]);
    size_t getLength() const;
    void setLength(size_t length);
    void insertElement(size_t index, const float data[3]);
    void removeElement(size_t index);

    virtual VrmlField *clone() const;
    virtual VrmlFieldType fieldType() const;
    virtual ostream& print(ostream& os) const;
    virtual const VrmlMFVec3f* toMFVec3f() const;
    virtual VrmlMFVec3f* toMFVec3f();
};

# endif
