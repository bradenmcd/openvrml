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

#   include <iostream>
#   include <memory>
#   include <string>
#   include <typeinfo>
#   include <vector>
#   include "common.h"
#   include "nodeptr.h"

namespace OpenVRML {
    class FieldValue;

    std::ostream & operator<<(std::ostream & out,
                              const FieldValue & fieldValue);

    class OPENVRML_SCOPE FieldValue {
        friend std::ostream & operator<<(std::ostream & out,
                                         const FieldValue & fieldValue);

    public:
        enum Type {
            invalidType, sfbool, sfcolor, sffloat, sfimage, sfint32, sfnode,
            sfrotation, sfstring, sftime, sfvec2f, sfvec3f, mfcolor, mffloat,
            mfint32, mfnode, mfrotation, mfstring, mftime, mfvec2f, mfvec3f
        };

        virtual ~FieldValue() throw () = 0;
        
        virtual std::auto_ptr<FieldValue> clone() const
            throw (std::bad_alloc) = 0;
        virtual FieldValue & assign(const FieldValue & value)
            throw (std::bad_cast, std::bad_alloc) = 0;
        virtual Type type() const throw () = 0;
    
    private:
        virtual void print(std::ostream & out) const = 0;
    };

    std::ostream & operator<<(std::ostream & out, FieldValue::Type type);
    std::istream & operator>>(std::istream & out, FieldValue::Type & type);


    class OPENVRML_SCOPE SFBool : public FieldValue {
        bool value;

    public:
        explicit SFBool(bool value = false) throw ();
        virtual ~SFBool() throw ();

        // Use compiler-defined copy ctor and operator=.

        bool get() const throw ();
        void set(bool value) throw ();

        virtual std::auto_ptr<FieldValue> clone() const throw (std::bad_alloc);
        virtual FieldValue & assign(const FieldValue & value)
            throw (std::bad_cast);
        virtual Type type() const throw ();
    
    private:
        virtual void print(std::ostream & out) const;
    };


    class OPENVRML_SCOPE SFColor : public FieldValue {
        float value[3];

    public:
        typedef float (&ArrayReference)[3];
        typedef const float (&ConstArrayReference)[3];
        typedef float (*ArrayPointer)[3];
        typedef const float (*ConstArrayPointer)[3];
        
        static void HSVtoRGB(ConstArrayReference hsv, ArrayReference rgb)
                throw ();
        static void RGBtoHSV(ConstArrayReference rgb, ArrayReference hsv)
                throw ();

        SFColor() throw ();
        explicit SFColor(ConstArrayReference rgb) throw ();
        SFColor(float r, float g, float b) throw ();
        virtual ~SFColor() throw ();

        // Use compiler-defined copy ctor and operator=.

        float operator[](size_t index) const throw ();
        float & operator[](size_t index) throw ();
        float getR() const throw ();
        float getG() const throw ();
        float getB() const throw ();
        ConstArrayReference get() const throw ();
        void set(ConstArrayReference rgb) throw ();
        void setHSV(float h, float s, float v) throw ();
        void getHSV(ArrayReference hsv) const throw ();

        virtual std::auto_ptr<FieldValue> clone() const throw (std::bad_alloc);
        virtual FieldValue & assign(const FieldValue & value)
            throw (std::bad_cast);
        virtual Type type() const throw ();
    
    private:
        virtual void print(std::ostream & out) const;
    };


    class OPENVRML_SCOPE SFFloat : public FieldValue {
        float value;

    public:
        explicit SFFloat(float value = 0.0) throw ();
        virtual ~SFFloat() throw ();

        // Use compiler-defined copy ctor and operator=.

        float get() const throw ();
        void set(float value) throw ();

        virtual std::auto_ptr<FieldValue> clone() const throw (std::bad_alloc);
        virtual FieldValue & assign(const FieldValue & value)
            throw (std::bad_cast);
        virtual Type type() const throw ();
    
    private:
        virtual void print(std::ostream & out) const;
    };


    class OPENVRML_SCOPE SFImage : public FieldValue {
        size_t d_w, d_h, d_nc;
        unsigned char * d_pixels;	// nc bytes/pixel, lower left to upper right

    public:
        SFImage() throw ();
        SFImage(size_t w, size_t h, size_t nc, const unsigned char * pixels = 0)
                throw (std::bad_alloc);
        SFImage(const SFImage &) throw (std::bad_alloc);
        virtual ~SFImage() throw ();

        SFImage & operator=(const SFImage & rhs) throw (std::bad_alloc);

        size_t getWidth() const throw ();
        size_t getHeight() const throw ();
        size_t getComponents() const throw ();
        const unsigned char * getPixels() const throw ();
        void set(size_t width, size_t height, size_t components,
                 const unsigned char * pixels) throw (std::bad_alloc);

        virtual std::auto_ptr<FieldValue> clone() const throw (std::bad_alloc);
        virtual FieldValue & assign(const FieldValue & value)
            throw (std::bad_cast, std::bad_alloc);
        virtual Type type() const throw ();
    
    private:
        virtual void print(std::ostream & out) const;
    };


    class OPENVRML_SCOPE SFInt32 : public FieldValue {
        long value;

    public:
        explicit SFInt32(long = 0) throw ();
        virtual ~SFInt32() throw ();

        // Use compiler-defined copy ctor and operator=.

        long get() const throw ();
        void set(long) throw ();

        virtual std::auto_ptr<FieldValue> clone() const throw (std::bad_alloc);
        virtual FieldValue & assign(const FieldValue & value)
            throw (std::bad_cast);
        virtual Type type() const throw ();
    
    private:
        virtual void print(std::ostream &) const;
    };


    class OPENVRML_SCOPE SFNode : public FieldValue {
        NodePtr node;

    public:
        explicit SFNode(const NodePtr & node = NodePtr(0)) throw ();
        virtual ~SFNode() throw ();

        // Use compiler-defined copy ctor and operator=.

        const NodePtr & get() const throw ();
        void set(const NodePtr & node) throw ();

        virtual std::auto_ptr<FieldValue> clone() const throw (std::bad_alloc);
        virtual FieldValue & assign(const FieldValue & value)
            throw (std::bad_cast);
        virtual Type type() const throw ();
    
    private:
        virtual void print(std::ostream &) const;
    };


    class SFVec3f;
    
    class OPENVRML_SCOPE SFRotation : public FieldValue {
        float value[4];

    public:
        typedef float (&ArrayReference)[4];
        typedef const float (&ConstArrayReference)[4];
        typedef float (*ArrayPointer)[4];
        typedef const float (*ConstArrayPointer)[4];
        
        SFRotation() throw ();
        explicit SFRotation(ConstArrayReference rot) throw ();
        SFRotation(float x, float y, float z, float angle) throw ();
        SFRotation(const SFVec3f & axis, float angle) throw ();
        SFRotation(const SFVec3f & fromVec, const SFVec3f & toVec) throw ();
        virtual ~SFRotation() throw ();

        // Use compiler-defined copy ctor and operator=.

        ConstArrayReference get() const throw ();
        void set(ConstArrayReference rot) throw ();
        float getX() const throw ();
        void setX(float) throw ();
        float getY() const throw ();
        void setY(float) throw ();
        float getZ() const throw ();
        void setZ(float) throw ();
        float getAngle() const throw ();
        void setAngle(float) throw ();
        const SFVec3f getAxis() const throw ();
        void setAxis(const SFVec3f & vec) throw ();
        const SFRotation inverse() const throw ();
        const SFRotation multiply(const SFRotation & rot) const throw ();
        const SFVec3f multVec(const SFVec3f & vec) const;
        const SFRotation slerp(const SFRotation & destRot, float t) const
                throw ();  

        virtual std::auto_ptr<FieldValue> clone() const throw (std::bad_alloc);
        virtual FieldValue & assign(const FieldValue & value)
            throw (std::bad_cast);
        virtual Type type() const throw ();

    private:
        virtual void print(std::ostream & os) const;
    };


    class OPENVRML_SCOPE SFString : public FieldValue {
        std::string value;

    public:
        explicit SFString(const std::string & value = std::string())
                throw (std::bad_alloc);
        virtual ~SFString() throw ();

        // Use compiler-defined copy ctor and operator=.

        const std::string & get() const throw ();
        void set(const std::string & value) throw (std::bad_alloc);

        virtual std::auto_ptr<FieldValue> clone() const throw (std::bad_alloc);
        virtual FieldValue & assign(const FieldValue & value)
            throw (std::bad_cast, std::bad_alloc);
        virtual Type type() const throw ();
    
    private:
        virtual void print(std::ostream &) const;
    };


    class OPENVRML_SCOPE SFTime : public FieldValue {
        double value;

    public:
        explicit SFTime(double value = 0.0) throw ();
        virtual ~SFTime() throw ();

        // Use compiler-defined copy ctor and operator=.

        double get() const throw ();
        void set(double value) throw ();

        virtual std::auto_ptr<FieldValue> clone() const throw (std::bad_alloc);
        virtual FieldValue & assign(const FieldValue & value)
            throw (std::bad_cast);
        virtual Type type() const throw ();
    
    private:
        virtual void print(std::ostream &) const;
    };


    class OPENVRML_SCOPE SFVec2f : public FieldValue {
        float value[2];

    public:
        typedef float (&ArrayReference)[2];
        typedef const float (&ConstArrayReference)[2];
        typedef float (*ArrayPointer)[2];
        typedef const float (*ConstArrayPointer)[2];
        
        SFVec2f() throw ();
        explicit SFVec2f(ConstArrayReference vec) throw ();
        SFVec2f(float x, float y) throw ();
        virtual ~SFVec2f() throw ();

        // Use compiler-defined copy ctor and operator=.

        float operator[](size_t index) const throw ();
        float & operator[](size_t index) throw ();
        float getX() const throw ();
        void setX(float) throw ();
        float getY() const throw ();
        void setY(float) throw ();
        ConstArrayReference get() const throw ();
        void set(ConstArrayReference vec) throw ();
        const SFVec2f add(const SFVec2f & vec) const throw ();
        const SFVec2f divide(float number) const throw ();
        double dot(const SFVec2f & vec) const throw ();
        double length() const throw ();
        const SFVec2f multiply(float number) const throw ();
        const SFVec2f negate() const throw ();
        const SFVec2f normalize() const throw ();
        const SFVec2f subtract(const SFVec2f & vec) const throw ();

        virtual std::auto_ptr<FieldValue> clone() const throw (std::bad_alloc);
        virtual FieldValue & assign(const FieldValue & value)
            throw (std::bad_cast);
        virtual Type type() const throw ();
    
    private:
        virtual void print(std::ostream &) const;
    };


    class OPENVRML_SCOPE SFVec3f : public FieldValue {
        float value[3];

    public:
        typedef float (&ArrayReference)[3];
        typedef const float (&ConstArrayReference)[3];
        typedef float (*ArrayPointer)[3];
        typedef const float (*ConstArrayPointer)[3];
        
        SFVec3f() throw ();
        explicit SFVec3f(ConstArrayReference vec) throw ();
        SFVec3f(float x, float y, float z) throw ();
        virtual ~SFVec3f() throw ();

        // Use compiler-defined copy ctor and operator=.

        float operator[](size_t index) const throw ();
        float & operator[](size_t index) throw ();
        float getX() const throw ();
        void setX(float) throw ();
        float getY() const throw ();
        void setY(float) throw ();
        float getZ() const throw ();
        void setZ(float) throw ();
        ConstArrayReference get() const throw ();
        void set(ConstArrayReference vec) throw ();
        const SFVec3f add(const SFVec3f & vec) const throw ();
        const SFVec3f cross(const SFVec3f & vec) const throw ();
        const SFVec3f divide(float number) const throw ();
        double dot(const SFVec3f & vec) const throw ();
        double length() const throw ();
        const SFVec3f multiply(float number) const throw ();
        const SFVec3f negate() const throw ();
        const SFVec3f normalize() const throw ();
        const SFVec3f subtract(const SFVec3f & vec) const throw ();

        virtual std::auto_ptr<FieldValue> clone() const throw (std::bad_alloc);
        virtual FieldValue & assign(const FieldValue & value)
            throw (std::bad_cast);
        virtual Type type() const throw ();
    
    private:
        virtual void print(std::ostream &) const;
    };


    class OPENVRML_SCOPE MFColor : public FieldValue {
        void * values;

    public:
        explicit MFColor(size_t length = 0,
                         SFColor::ConstArrayPointer values = 0)
            throw (std::bad_alloc);
        MFColor(const MFColor & mfcolor) throw (std::bad_alloc);
        virtual ~MFColor() throw ();

        MFColor & operator=(const MFColor & mfcolor) throw (std::bad_alloc);

        SFColor::ConstArrayReference getElement(size_t index) const throw ();
        void setElement(size_t index, SFColor::ConstArrayReference value)
            throw ();
        size_t getLength() const throw ();
        void setLength(size_t length) throw (std::bad_alloc);
        void insertElement(size_t index, SFColor::ConstArrayReference value)
            throw (std::bad_alloc);
        void removeElement(size_t index) throw ();

        virtual std::auto_ptr<FieldValue> clone() const throw (std::bad_alloc);
        virtual FieldValue & assign(const FieldValue & value)
            throw (std::bad_cast, std::bad_alloc);
        virtual Type type() const throw ();
    
    private:
        virtual void print(std::ostream &) const;
    };


    class OPENVRML_SCOPE MFFloat : public FieldValue {
        std::vector<float> values;

    public:
        explicit MFFloat(size_t length = 0, float const * numbers = 0)
            throw (std::bad_alloc);
        virtual ~MFFloat() throw ();

        // Use compiler-generated copy constructor and operator=.
        
        const float & getElement(size_t index) const throw ();
        void setElement(size_t index, float value) throw ();
        size_t getLength() const throw ();
        void setLength(size_t length) throw (std::bad_alloc);
        void insertElement(size_t index, float value) throw (std::bad_alloc);
        void removeElement(size_t index) throw ();

        virtual std::auto_ptr<FieldValue> clone() const throw (std::bad_alloc);
        virtual FieldValue & assign(const FieldValue & value)
            throw (std::bad_cast, std::bad_alloc);
        virtual Type type() const throw ();
    
    private:
        virtual void print(std::ostream &) const;
    };


    class OPENVRML_SCOPE MFInt32 : public FieldValue {
        std::vector<long> values;

    public:
        explicit MFInt32(size_t length = 0, const long * numbers = 0)
            throw (std::bad_alloc);
        virtual ~MFInt32() throw ();

        // Use compiler-defined copy constructor and operator=.
        
        const long & getElement(size_t index) const throw ();
        void setElement(size_t index, long value) throw ();
        size_t getLength() const throw ();
        void setLength(size_t length) throw (std::bad_alloc);
        void insertElement(size_t index, long value) throw (std::bad_alloc);
        void removeElement(size_t index) throw ();

        virtual std::auto_ptr<FieldValue> clone() const throw (std::bad_alloc);
        virtual FieldValue & assign(const FieldValue & value)
            throw (std::bad_cast, std::bad_alloc);
        virtual Type type() const throw ();
    
    private:
        virtual void print(std::ostream &) const;
    };


    class OPENVRML_SCOPE MFNode : public FieldValue {
        std::vector<NodePtr> nodes;

    public:
        explicit MFNode(size_t length = 0, const NodePtr * nodes = 0)
            throw (std::bad_alloc);
        virtual ~MFNode() throw ();

        // Use compiler-defined copy ctor and operator=

        const NodePtr & getElement(size_t index) const throw ();
        void setElement(size_t index, const NodePtr & node) throw ();
        size_t getLength() const throw ();
        void setLength(size_t length) throw (std::bad_alloc);
        bool exists(const Node & node) const;
        bool addNode(const NodePtr & node);
        bool removeNode(const Node & node);
        void insertElement(size_t index, const NodePtr & node)
            throw (std::bad_alloc);
        void removeElement(size_t index) throw ();
        void clear() throw ();

        virtual std::auto_ptr<FieldValue> clone() const throw (std::bad_alloc);
        virtual FieldValue & assign(const FieldValue & value)
            throw (std::bad_cast, std::bad_alloc);
        virtual Type type() const throw ();
    
    private:
        virtual void print(std::ostream &) const;
    };


    class OPENVRML_SCOPE MFRotation : public FieldValue {
        void * values;

    public:
        explicit MFRotation(size_t length = 0,
                            SFRotation::ConstArrayPointer values = 0)
            throw (std::bad_alloc);
        MFRotation(const MFRotation & mfrotation) throw (std::bad_alloc);
        virtual ~MFRotation() throw ();

        MFRotation & operator=(const MFRotation & mfrotation)
            throw (std::bad_alloc);

        SFRotation::ConstArrayReference getElement(size_t index) const throw ();
        void setElement(size_t index, SFRotation::ConstArrayReference value)
            throw ();
        size_t getLength() const throw ();
        void setLength(size_t length) throw (std::bad_alloc);
        void insertElement(size_t index, SFRotation::ConstArrayReference value)
            throw (std::bad_alloc);
        void removeElement(size_t index) throw ();

        virtual std::auto_ptr<FieldValue> clone() const throw (std::bad_alloc);
        virtual FieldValue & assign(const FieldValue & value)
            throw (std::bad_cast, std::bad_alloc);
        virtual Type type() const throw ();
    
    private:
        virtual void print(std::ostream &) const;
    };


    class OPENVRML_SCOPE MFString : public FieldValue {
        std::vector<std::string> values;

    public:
        explicit MFString(size_t length = 0, const std::string * values = 0)
                throw (std::bad_alloc);
        MFString(const MFString & mfstring) throw (std::bad_alloc);
        virtual ~MFString() throw ();

        MFString & operator=(const MFString & mfstring) throw (std::bad_alloc);

        const std::string & getElement(size_t index) const throw ();
        void setElement(size_t index, const std::string & value)
            throw (std::bad_alloc);
        size_t getLength() const throw ();
        void setLength(const size_t length) throw (std::bad_alloc);
        void insertElement(size_t index, const std::string & value)
            throw (std::bad_alloc);
        void removeElement(size_t index) throw ();

        virtual std::auto_ptr<FieldValue> clone() const throw (std::bad_alloc);
        virtual FieldValue & assign(const FieldValue & value)
            throw (std::bad_cast, std::bad_alloc);
        virtual Type type() const throw ();
    
    private:
        virtual void print(std::ostream &) const;
    };


    class OPENVRML_SCOPE MFTime : public FieldValue {
        std::vector<double> values;

    public:
        explicit MFTime(size_t length = 0, const double * times = 0)
            throw (std::bad_alloc);
        virtual ~MFTime() throw ();

        // Use compiler-defined copy constructor and operator=.
        
        const double & getElement(size_t index) const throw ();
        void setElement(size_t index, double value) throw ();
        size_t getLength() const throw ();
        void setLength(size_t length) throw (std::bad_alloc);
        void insertElement(size_t index, double value) throw (std::bad_alloc);
        void removeElement(size_t index) throw ();

        virtual std::auto_ptr<FieldValue> clone() const throw (std::bad_alloc);
        virtual FieldValue & assign(const FieldValue & value)
            throw (std::bad_cast, std::bad_alloc);
        virtual Type type() const throw ();
    
    private:
        virtual void print(std::ostream &) const;
    };


    class OPENVRML_SCOPE MFVec2f : public FieldValue {
        void * values;

    public:
        explicit MFVec2f(size_t length = 0,
                         SFVec2f::ConstArrayPointer values = 0)
            throw (std::bad_alloc);
        MFVec2f(const MFVec2f & mfvec2f) throw (std::bad_alloc);
        virtual ~MFVec2f() throw ();

        MFVec2f & operator=(const MFVec2f & mfvec2f) throw (std::bad_alloc);

        SFVec2f::ConstArrayReference getElement(size_t index) const throw ();
        void setElement(size_t index, SFVec2f::ConstArrayReference value)
            throw ();
        size_t getLength() const throw ();
        void setLength(size_t length) throw (std::bad_alloc);
        void insertElement(size_t index, SFVec2f::ConstArrayReference value)
            throw (std::bad_alloc);
        void removeElement(size_t index) throw ();

        virtual std::auto_ptr<FieldValue> clone() const throw (std::bad_alloc);
        virtual FieldValue & assign(const FieldValue & value)
            throw (std::bad_cast, std::bad_alloc);
        virtual Type type() const throw ();
    
    private:
        virtual void print(std::ostream &) const;
    };


    class OPENVRML_SCOPE MFVec3f : public FieldValue {
        void * values;

    public:
        explicit MFVec3f(size_t length = 0,
                         SFVec3f::ConstArrayPointer values = 0)
            throw (std::bad_alloc);
        MFVec3f(const MFVec3f & mfvec3f) throw (std::bad_alloc);
        virtual ~MFVec3f() throw ();

        MFVec3f & operator=(const MFVec3f & mfvec3f) throw (std::bad_alloc);

        SFVec3f::ConstArrayReference getElement(size_t index) const throw ();
        void setElement(size_t index, SFVec3f::ConstArrayReference value)
            throw ();
        size_t getLength() const throw ();
        void setLength(size_t length) throw (std::bad_alloc);
        void insertElement(size_t index, SFVec3f::ConstArrayReference data)
            throw (std::bad_alloc);
        void removeElement(size_t index) throw ();

        virtual std::auto_ptr<FieldValue> clone() const throw (std::bad_alloc);
        virtual FieldValue & assign(const FieldValue & value)
            throw (std::bad_cast, std::bad_alloc);
        virtual Type type() const throw ();
    
    private:
        virtual void print(std::ostream &) const;
    };
}

# endif
