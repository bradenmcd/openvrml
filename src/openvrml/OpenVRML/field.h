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
#   include <typeinfo>
#   include <vector>
#   include <iostream.h>
#   include "common.h"
#   include "nodeptr.h"

namespace OpenVRML {
    class FieldValue;
}

ostream & operator<<(ostream &, const OpenVRML::FieldValue &);

namespace OpenVRML {

    class SFBool;
    class SFColor;
    class SFFloat;
    class SFImage;
    class SFInt32;
    class SFNode;
    class SFRotation;
    class SFString;
    class SFTime;
    class SFVec2f;
    class SFVec3f;
    class MFColor;
    class MFFloat;
    class MFInt32;
    class MFNode;
    class MFRotation;
    class MFString;
    class MFVec2f;
    class MFVec3f;

    class OPENVRML_SCOPE FieldValue {
        friend ostream & ::operator<<(ostream &, const FieldValue &);

    public:
        enum Type {
            invalidType, sfbool, sfcolor, sffloat, sfimage, sfint32, sfnode,
            sfrotation, sfstring, sftime, sfvec2f, sfvec3f, mfcolor, mffloat,
            mfint32, mfnode, mfrotation, mfstring, mftime, mfvec2f, mfvec3f
        };

        static Type type(const char * typeId);
        static const char * getFieldName(const Type type);

        virtual ~FieldValue() = 0;
        
        const char * typeName() const;

        virtual FieldValue * clone() const throw (std::bad_alloc) = 0;
        virtual FieldValue & assign(const FieldValue &)
                throw (std::bad_cast) = 0;
        virtual ostream& print(ostream& os) const = 0;
        virtual Type type() const = 0;

        // safe downcasts, const and non-const versions.
        // These avoid casts of VrmlField* but are ugly in that this class
        // must know of the existence of all of its subclasses...
        virtual const SFBool *toSFBool() const;
        virtual const SFColor *toSFColor() const;
        virtual const SFFloat *toSFFloat() const;
        virtual const SFImage *toSFImage() const;
        virtual const SFInt32 *toSFInt32() const;
        virtual const SFNode *toSFNode() const;
        virtual const SFRotation *toSFRotation() const;
        virtual const SFString *toSFString() const;
        virtual const SFTime *toSFTime() const;
        virtual const SFVec2f *toSFVec2f() const;
        virtual const SFVec3f *toSFVec3f() const;
        virtual const MFColor *toMFColor() const;
        virtual const MFFloat *toMFFloat() const;
        virtual const MFInt32 *toMFInt32() const;
        virtual const MFNode *toMFNode() const;
        virtual const MFRotation *toMFRotation() const;
        virtual const MFString *toMFString() const;
        virtual const MFVec2f *toMFVec2f() const;
        virtual const MFVec3f *toMFVec3f() const;
        virtual SFBool *toSFBool();
        virtual SFColor *toSFColor();
        virtual SFFloat *toSFFloat();
        virtual SFImage *toSFImage();
        virtual SFInt32 *toSFInt32();
        virtual SFNode *toSFNode();
        virtual SFRotation *toSFRotation();
        virtual SFString *toSFString();
        virtual SFTime *toSFTime();
        virtual SFVec2f *toSFVec2f();
        virtual SFVec3f *toSFVec3f();
        virtual MFColor *toMFColor();
        virtual MFFloat *toMFFloat();
        virtual MFInt32 *toMFInt32();
        virtual MFNode *toMFNode();
        virtual MFRotation *toMFRotation();
        virtual MFString *toMFString();
        virtual MFVec2f *toMFVec2f();
        virtual MFVec3f *toMFVec3f();
    };


    class OPENVRML_SCOPE SFBool : public FieldValue {
        bool d_value;

    public:
        explicit SFBool(bool value = false);
        virtual ~SFBool();

        // Use compiler-defined copy ctor and operator=.

        bool get() const;
        void set(bool value);

        virtual FieldValue * clone() const throw (std::bad_alloc);
        virtual FieldValue & assign(const FieldValue &) throw (std::bad_cast);
        virtual ostream& print(ostream& os) const;
        virtual Type type() const;
        virtual const SFBool * toSFBool() const;
        virtual SFBool * toSFBool();
    };


    class OPENVRML_SCOPE SFColor : public FieldValue {
        float d_rgb[3];

    public:
        static void HSVtoRGB(const float hsv[3], float rgb[3]);
        static void RGBtoHSV(const float rgb[3], float hsv[3]);

        SFColor();
        explicit SFColor(const float rgb[3]);
        SFColor(float r, float g, float b);
        virtual ~SFColor();

        // Use compiler-defined copy ctor and operator=.

        float operator[](size_t index) const;
        float & operator[](size_t index);
        float getR() const;
        float getG() const;
        float getB() const;
        const float (&get() const)[3];
        void set(const float rgb[3]);
        void setHSV(float h, float s, float v);
        void getHSV(float hsv[3]) const;

        virtual FieldValue * clone() const throw (std::bad_alloc);
        virtual FieldValue & assign(const FieldValue &) throw (std::bad_cast);
        virtual ostream& print(ostream& os) const;
        virtual Type type() const;
        virtual const SFColor * toSFColor() const;
        virtual SFColor * toSFColor();
    };


    class OPENVRML_SCOPE SFFloat : public FieldValue {
        float d_value;

    public:
        explicit SFFloat(float value = 0.0);
        virtual ~SFFloat();

        // Use compiler-defined copy ctor and operator=.

        float get() const;
        void set(float value);

        virtual FieldValue * clone() const throw (std::bad_alloc);
        virtual FieldValue & assign(const FieldValue &) throw (std::bad_cast);
        virtual ostream& print(ostream& os) const;
        virtual Type type() const;
        virtual const SFFloat * toSFFloat() const;
        virtual SFFloat * toSFFloat();
    };


    class OPENVRML_SCOPE SFImage : public FieldValue {
        size_t d_w, d_h, d_nc;
        unsigned char * d_pixels;	// nc bytes/pixel, lower left to upper right

    public:
        SFImage();
        SFImage(size_t w, size_t h, size_t nc, const unsigned char * pixels = 0);
        SFImage(const SFImage &);
        virtual ~SFImage();

        SFImage & operator=(const SFImage & rhs);

        size_t getWidth() const;
        size_t getHeight() const;
        size_t getComponents() const;
        const unsigned char * getPixels() const;
        void set(size_t width, size_t height, size_t components,
                 const unsigned char * pixels);

        virtual FieldValue * clone() const throw (std::bad_alloc);
        virtual FieldValue & assign(const FieldValue &) throw (std::bad_cast);
        virtual ostream& print(ostream& os) const;
        virtual Type type() const;
        virtual const SFImage * toSFImage() const;
        virtual SFImage * toSFImage();
    };


    class OPENVRML_SCOPE SFInt32 : public FieldValue {
        long d_value;

    public:
        explicit SFInt32(long = 0);
        virtual ~SFInt32();

        // Use compiler-defined copy ctor and operator=.

        long get() const;
        void set(long);

        virtual FieldValue * clone() const throw (std::bad_alloc);
        virtual FieldValue & assign(const FieldValue &) throw (std::bad_cast);
        virtual ostream& print(ostream& os) const;
        virtual Type type() const;
        virtual const SFInt32 * toSFInt32() const;
        virtual SFInt32 * toSFInt32();
    };


    class OPENVRML_SCOPE SFNode : public FieldValue {
        NodePtr node;

    public:
        explicit SFNode(const NodePtr & node = NodePtr(0));
        virtual ~SFNode();

        // Use compiler-defined copy ctor and operator=.

        const NodePtr & get() const;
        void set(const NodePtr & node);

        virtual FieldValue * clone() const throw (std::bad_alloc);
        virtual FieldValue & assign(const FieldValue &) throw (std::bad_cast);
        virtual ostream& print(ostream& os) const;
        virtual Type type() const;
        virtual const SFNode * toSFNode() const;
        virtual SFNode * toSFNode();
    };


    class OPENVRML_SCOPE SFRotation : public FieldValue {
        float d_x[4];

    public:
        SFRotation();
        explicit SFRotation(const float rot[4]);
        SFRotation(float x, float y, float z, float angle);
        SFRotation(const SFVec3f & axis, float angle);
        SFRotation(const SFVec3f & fromVec, const SFVec3f & toVec);
        virtual ~SFRotation();

        // Use compiler-defined copy ctor and operator=.

//        float operator[](size_t index) const;
//        float & operator[](size_t index);
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
        const SFVec3f getAxis() const;
        void setAxis(const SFVec3f & vec);
        const SFRotation inverse() const;
        const SFRotation multiply(const SFRotation & rot) const;
        const SFVec3f multVec(const SFVec3f & vec) const;
        const SFRotation slerp(const SFRotation & destRot, float t) const;  

        virtual FieldValue * clone() const throw (std::bad_alloc);
        virtual FieldValue & assign(const FieldValue &) throw (std::bad_cast);
        virtual ostream& print(ostream& os) const;
        virtual Type type() const;
        virtual const SFRotation * toSFRotation() const;
        virtual SFRotation * toSFRotation();

    private:
        void toQuaternion(float theQuat[4]) const;
        void fromQuaternion(const float theQuat[4]);
    };


    class OPENVRML_SCOPE SFString : public FieldValue {
        std::string value;

    public:
        explicit SFString(const std::string & value = std::string());
        virtual ~SFString();

        // Use compiler-defined copy ctor and operator=.

        const std::string & get() const;
        void set(const std::string & value);

        virtual FieldValue * clone() const throw (std::bad_alloc);
        virtual FieldValue & assign(const FieldValue &) throw (std::bad_cast);
        virtual ostream& print(ostream& os) const;
        virtual Type type() const;
        virtual const SFString * toSFString() const;
        virtual SFString * toSFString();
    };


    class OPENVRML_SCOPE SFTime : public FieldValue {
        double d_value;

    public:
        explicit SFTime(double value = 0.0);
        virtual ~SFTime();

        // Use compiler-defined copy ctor and operator=.

        double get() const;
        void set(double value);

        virtual FieldValue * clone() const throw (std::bad_alloc);
        virtual FieldValue & assign(const FieldValue &) throw (std::bad_cast);
        virtual ostream& print(ostream& os) const;
        virtual Type type() const;
        virtual const SFTime * toSFTime() const;
        virtual SFTime * toSFTime();
    };


    class OPENVRML_SCOPE SFVec2f : public FieldValue {
        float d_x[2];

    public:
        SFVec2f();
        explicit SFVec2f(const float vec[2]);
        SFVec2f(float x, float y);
        virtual ~SFVec2f();

        // Use compiler-defined copy ctor and operator=.

        float operator[](size_t index) const;
        float & operator[](size_t index);
        float getX() const;
        void setX(float);
        float getY() const;
        void setY(float);
        const float (&get() const)[2];
        void set(const float vec[2]);
        const SFVec2f add(const SFVec2f & vec) const;
        const SFVec2f divide(float number) const;
        double dot(const SFVec2f & vec) const;
        double length() const;
        const SFVec2f multiply(float number) const;
        const SFVec2f negate() const;
        const SFVec2f normalize() const;
        const SFVec2f subtract(const SFVec2f & vec) const;

        virtual FieldValue * clone() const throw (std::bad_alloc);
        virtual FieldValue & assign(const FieldValue &) throw (std::bad_cast);
        virtual ostream& print(ostream& os) const;
        virtual Type type() const;
        virtual const SFVec2f * toSFVec2f() const;
        virtual SFVec2f * toSFVec2f();
    };


    class OPENVRML_SCOPE SFVec3f : public FieldValue {
        float d_x[3];

    public:
        SFVec3f();
        explicit SFVec3f(const float vec[3]);
        SFVec3f(float x, float y, float z);
        virtual ~SFVec3f();

        // Use compiler-defined copy ctor and operator=.

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
        const SFVec3f add(const SFVec3f & vec) const;
        const SFVec3f cross(const SFVec3f & vec) const;
        const SFVec3f divide(float number) const;
        double dot(const SFVec3f & vec) const;
        double length() const;
        const SFVec3f multiply(float number) const;
        const SFVec3f negate() const;
        const SFVec3f normalize() const;
        const SFVec3f subtract(const SFVec3f & vec) const;

        virtual FieldValue * clone() const throw (std::bad_alloc);
        virtual FieldValue & assign(const FieldValue &) throw (std::bad_cast);
        virtual ostream& print(ostream& os) const;
        virtual Type type() const;
        virtual const SFVec3f * toSFVec3f() const;
        virtual SFVec3f * toSFVec3f();
    };


    class OPENVRML_SCOPE MFColor : public FieldValue {
        class FData;
        FData * d_data;

    public:
        explicit MFColor(size_t length = 0, float const * colors = 0);
        MFColor(const MFColor & mfcolor);
        virtual ~MFColor();

        MFColor & operator=(const MFColor & mfcolor);

        const float * get() const;
        void set(size_t length, const float * colors = 0);
        const float * getElement(size_t index) const;
        void setElement(size_t index, const float value[3]);
        size_t getLength() const;
        void setLength(size_t length);
        void insertElement(size_t index, const float value[3]);
        void removeElement(size_t index);

        virtual FieldValue * clone() const throw (std::bad_alloc);
        virtual FieldValue & assign(const FieldValue &) throw (std::bad_cast);
        virtual ostream& print(ostream& os) const;
        virtual Type type() const;
        virtual const MFColor * toMFColor() const;
        virtual MFColor * toMFColor();
    };


    class OPENVRML_SCOPE MFFloat : public FieldValue {
        class FData;
        FData * d_data;

    public:
        explicit MFFloat(size_t length = 0, float const * numbers = 0);
        MFFloat(const MFFloat & mffloat);
        virtual ~MFFloat();

        MFFloat & operator=(const MFFloat & mffloat);

        const float * get() const;
        void set(size_t length, const float * numbers);
        float getElement(size_t index) const;
        void setElement(size_t index, float value);
        size_t getLength() const;
        void setLength(size_t length);
        void insertElement(size_t index, float value);
        void removeElement(size_t index);

        virtual FieldValue * clone() const throw (std::bad_alloc);
        virtual FieldValue & assign(const FieldValue &) throw (std::bad_cast);
        virtual ostream& print(ostream& os) const;
        virtual Type type() const;
        virtual const MFFloat * toMFFloat() const;
        virtual MFFloat * toMFFloat();
    };


    class OPENVRML_SCOPE MFInt32 : public FieldValue {
        class IData;
        IData *d_data;

    public:
        explicit MFInt32(size_t length = 0, const long * numbers = 0);
        MFInt32(const MFInt32 & mfint32);
        virtual ~MFInt32();

        MFInt32 & operator=(const MFInt32 & mfint32);

        const long * get() const;
        void set(size_t length, const long * numbers);
        long getElement(size_t index) const;
        void setElement(size_t index, long value);
        size_t getLength() const;
        void setLength(size_t length);
        void insertElement(size_t index, long value);
        void removeElement(size_t index);

        virtual FieldValue * clone() const throw (std::bad_alloc);
        virtual FieldValue & assign(const FieldValue &) throw (std::bad_cast);
        virtual ostream& print(ostream& os) const;
        virtual Type type() const;
        virtual const MFInt32 * toMFInt32() const;
        virtual MFInt32 * toMFInt32();
    };


    class OPENVRML_SCOPE MFNode : public FieldValue {
        std::vector<NodePtr> nodes;

    public:
        explicit MFNode(size_t length = 0, const NodePtr * nodes = 0);
        virtual ~MFNode();

        // Use compiler-defined copy ctor and operator=

        const NodePtr & getElement(size_t index) const throw ();
        void setElement(size_t index, const NodePtr & node);
        size_t getLength() const throw ();
        void setLength(size_t length);
        bool exists(const Node & node) const;
        bool addNode(Node & node);
        bool removeNode(const Node & node);
        void insertElement(size_t index, const NodePtr & node);
        void removeElement(size_t index);
        void clear();

        virtual FieldValue * clone() const throw (std::bad_alloc);
        virtual FieldValue & assign(const FieldValue &) throw (std::bad_cast);
        virtual ostream& print(ostream& os) const;
        virtual Type type() const;
        virtual const MFNode* toMFNode() const;
        virtual MFNode* toMFNode();
    };


    class OPENVRML_SCOPE MFRotation : public FieldValue {
        class FData;
        FData *d_data;

    public:
        explicit MFRotation(size_t length = 0, const float * rotations = 0);
        MFRotation(const MFRotation & mfrotation);
        virtual ~MFRotation();

        MFRotation & operator=(const MFRotation & mfrotation);

        const float * get() const;
        void set(size_t length, const float * rotations);
        const float * getElement(size_t index) const;
        void setElement(size_t, const float value[4]);
        size_t getLength() const;
        void setLength(size_t length);
        void insertElement(size_t index, const float value[3]);
        void removeElement(size_t index);

        virtual FieldValue * clone() const throw (std::bad_alloc);
        virtual FieldValue & assign(const FieldValue &) throw (std::bad_cast);
        virtual ostream& print(ostream& os) const;
        virtual Type type() const;
        virtual const MFRotation * toMFRotation() const;
        virtual MFRotation * toMFRotation();
    };


    class OPENVRML_SCOPE MFString : public FieldValue {
        std::vector<std::string> values;

    public:
        explicit MFString(size_t length = 0, const std::string * values = 0);
        virtual ~MFString();

        // Use compiler-defined copy ctor and operator=.

        const std::string & getElement(size_t index) const;
        void setElement(size_t index, const std::string & value);
        size_t getLength() const;
        void setLength(const size_t length);
        void insertElement(size_t index, const std::string & value);
        void removeElement(size_t index);

        virtual FieldValue * clone() const throw (std::bad_alloc);
        virtual FieldValue & assign(const FieldValue &) throw (std::bad_cast);
        virtual Type type() const;
        virtual const MFString * toMFString() const;
        virtual MFString * toMFString();
        virtual ostream& print(ostream& os) const;
    };


    class OPENVRML_SCOPE MFTime : public FieldValue {
        class DData;
        DData * d_data;

    public:
        explicit MFTime(size_t length = 0, const double * times = 0);
        MFTime(const MFTime & mftime);
        virtual ~MFTime();

        MFTime & operator=(const MFTime & mftime);

        const double * get() const;
        void set(size_t length, const double * times);
        double getElement(size_t index) const;
        void setElement(size_t index, double value);
        size_t getLength() const;
        void setLength(size_t length);
        void insertElement(size_t index, double value);
        void removeElement(size_t index);

        virtual FieldValue * clone() const throw (std::bad_alloc);
        virtual FieldValue & assign(const FieldValue &) throw (std::bad_cast);
        virtual ostream & print(ostream &) const;
        virtual Type type() const;
    };


    class OPENVRML_SCOPE MFVec2f : public FieldValue {
        class FData;
        FData * d_data;

    public:
        explicit MFVec2f(size_t length = 0, const float * vecs = 0);
        MFVec2f(const MFVec2f & mfvec2f);
        virtual ~MFVec2f();

        MFVec2f & operator=(const MFVec2f & mfvec2f);

        const float * get() const;
        void set(size_t length, const float * vecs);
        const float * getElement(size_t index) const;
        void setElement(size_t, const float value[2]);
        size_t getLength() const;
        void setLength(size_t length);
        void insertElement(size_t index, const float value[2]);
        void removeElement(size_t index);

        virtual FieldValue * clone() const throw (std::bad_alloc);
        virtual FieldValue & assign(const FieldValue &) throw (std::bad_cast);
        virtual Type type() const;
        virtual ostream& print(ostream& os) const;
        virtual const MFVec2f * toMFVec2f() const;
        virtual MFVec2f * toMFVec2f();
    };


    class OPENVRML_SCOPE MFVec3f : public FieldValue {
        class FData;
        FData * d_data;

    public:
        explicit MFVec3f(size_t length = 0, const float * vecs = 0);
        MFVec3f(const MFVec3f & mfvec3f);
        virtual ~MFVec3f();

        MFVec3f & operator=(const MFVec3f & mfvec3f);

        const float * get() const;
        void set(size_t length, const float * vecs);
        const float * getElement(size_t index) const;
        void setElement(size_t, const float value[3]);
        size_t getLength() const;
        void setLength(size_t length);
        void insertElement(size_t index, const float data[3]);
        void removeElement(size_t index);

        virtual FieldValue * clone() const throw (std::bad_alloc);
        virtual FieldValue & assign(const FieldValue &) throw (std::bad_cast);
        virtual Type type() const;
        virtual ostream& print(ostream& os) const;
        virtual const MFVec3f * toMFVec3f() const;
        virtual MFVec3f * toMFVec3f();
    };
}

# endif
