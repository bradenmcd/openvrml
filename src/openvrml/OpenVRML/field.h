// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; -*-
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

#   include <iosfwd>
#   include <memory>
#   include <stdexcept>
#   include <string>
#   include <typeinfo>
#   include <vector>
#   include <OpenVRML/basetypes.h>
#   include <OpenVRML/nodeptr.h>

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
    
    protected:
        FieldValue() throw ();
        FieldValue(const FieldValue & value) throw ();
        FieldValue & operator=(const FieldValue & value) throw ();
    
    private:
        virtual void print(std::ostream & out) const = 0;
    };

    std::ostream & operator<<(std::ostream & out, FieldValue::Type type);
    std::istream & operator>>(std::istream & out, FieldValue::Type & type);


    class OPENVRML_SCOPE SFBool : public FieldValue {
    public:
        bool value;

        explicit SFBool(bool value = false) throw ();
        virtual ~SFBool() throw ();

        // Use compiler-defined copy ctor and operator=.

        virtual std::auto_ptr<FieldValue> clone() const throw (std::bad_alloc);
        virtual FieldValue & assign(const FieldValue & value)
            throw (std::bad_cast);
        virtual Type type() const throw ();
    
    private:
        virtual void print(std::ostream & out) const;
    };

    inline bool operator==(const SFBool & lhs, const SFBool & rhs) throw ()
    {
        return lhs.value == rhs.value;
    }

    inline bool operator!=(const SFBool & lhs, const SFBool & rhs) throw ()
    {
        return !(lhs == rhs);
    }


    class OPENVRML_SCOPE SFColor : public FieldValue {
    public:
        color value;

        explicit SFColor(const color & value = color()) throw ();
        virtual ~SFColor() throw ();

        // Use compiler-defined copy ctor and operator=.

        virtual std::auto_ptr<FieldValue> clone() const throw (std::bad_alloc);
        virtual FieldValue & assign(const FieldValue & value)
            throw (std::bad_cast);
        virtual Type type() const throw ();
    
    private:
        virtual void print(std::ostream & out) const;
    };

    inline bool operator==(const SFColor & lhs, const SFColor & rhs) throw ()
    {
        return lhs.value == rhs.value;
    }

    inline bool operator!=(const SFColor & lhs, const SFColor & rhs) throw ()
    {
        return !(lhs == rhs);
    }


    class OPENVRML_SCOPE SFFloat : public FieldValue {
    public:
        float value;

        explicit SFFloat(float value = 0.0) throw ();
        virtual ~SFFloat() throw ();

        // Use compiler-defined copy ctor and operator=.

        virtual std::auto_ptr<FieldValue> clone() const throw (std::bad_alloc);
        virtual FieldValue & assign(const FieldValue & value)
            throw (std::bad_cast);
        virtual Type type() const throw ();
    
    private:
        virtual void print(std::ostream & out) const;
    };

    inline bool operator==(const SFFloat & lhs, const SFFloat & rhs) throw ()
    {
        return lhs.value == rhs.value;
    }

    inline bool operator!=(const SFFloat & lhs, const SFFloat & rhs) throw ()
    {
        return !(lhs == rhs);
    }


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
    public:
        int32 value;

        explicit SFInt32(int32 value = 0) throw ();
        virtual ~SFInt32() throw ();

        // Use compiler-defined copy ctor and operator=.

        virtual std::auto_ptr<FieldValue> clone() const throw (std::bad_alloc);
        virtual FieldValue & assign(const FieldValue & value)
            throw (std::bad_cast);
        virtual Type type() const throw ();
    
    private:
        virtual void print(std::ostream &) const;
    };

    inline bool operator==(const SFInt32 & lhs, const SFInt32 & rhs) throw ()
    {
        return lhs.value == rhs.value;
    }

    inline bool operator!=(const SFInt32 & lhs, const SFInt32 & rhs) throw ()
    {
        return !(lhs == rhs);
    }


    class OPENVRML_SCOPE SFNode : public FieldValue {
    public:
        NodePtr value;

        explicit SFNode(const NodePtr & node = NodePtr(0)) throw ();
        virtual ~SFNode() throw ();

        // Use compiler-defined copy ctor and operator=.

        virtual std::auto_ptr<FieldValue> clone() const throw (std::bad_alloc);
        virtual FieldValue & assign(const FieldValue & value)
            throw (std::bad_cast);
        virtual Type type() const throw ();
    
    private:
        virtual void print(std::ostream &) const;
    };

    inline bool operator==(const SFNode & lhs, const SFNode & rhs) throw ()
    {
        return lhs.value == rhs.value;
    }

    inline bool operator!=(const SFNode & lhs, const SFNode & rhs) throw ()
    {
        return !(lhs == rhs);
    }


    class OPENVRML_SCOPE SFRotation : public FieldValue {
    public:
        rotation value;

        explicit SFRotation(const rotation & rot = rotation()) throw ();
        virtual ~SFRotation() throw ();

        // Use compiler-defined copy ctor and operator=.

        virtual std::auto_ptr<FieldValue> clone() const throw (std::bad_alloc);
        virtual FieldValue & assign(const FieldValue & value)
            throw (std::bad_cast);
        virtual Type type() const throw ();

    private:
        virtual void print(std::ostream & os) const;
    };

    inline bool operator==(const SFRotation & lhs, const SFRotation & rhs)
        throw ()
    {
        return lhs.value == rhs.value;
    }

    inline bool operator!=(const SFRotation & lhs, const SFRotation & rhs)
        throw ()
    {
        return !(lhs == rhs);
    }


    class OPENVRML_SCOPE SFString : public FieldValue {
    public:
        std::string value;

        explicit SFString(const std::string & value = std::string())
                throw (std::bad_alloc);
        virtual ~SFString() throw ();

        // Use compiler-defined copy ctor and operator=.

        virtual std::auto_ptr<FieldValue> clone() const throw (std::bad_alloc);
        virtual FieldValue & assign(const FieldValue & value)
            throw (std::bad_cast, std::bad_alloc);
        virtual Type type() const throw ();
    
    private:
        virtual void print(std::ostream &) const;
    };

    inline bool operator==(const SFString & lhs, const SFString & rhs) throw ()
    {
        return lhs.value == rhs.value;
    }

    inline bool operator!=(const SFString & lhs, const SFString & rhs) throw ()
    {
        return !(lhs == rhs);
    }


    class OPENVRML_SCOPE SFTime : public FieldValue {
    public:
        double value;

        explicit SFTime(double value = 0.0) throw ();
        virtual ~SFTime() throw ();

        // Use compiler-defined copy ctor and operator=.

        virtual std::auto_ptr<FieldValue> clone() const throw (std::bad_alloc);
        virtual FieldValue & assign(const FieldValue & value)
            throw (std::bad_cast);
        virtual Type type() const throw ();
    
    private:
        virtual void print(std::ostream &) const;
    };

    inline bool operator==(const SFTime & lhs, const SFTime & rhs) throw ()
    {
        return lhs.value == rhs.value;
    }

    inline bool operator!=(const SFTime & lhs, const SFTime & rhs) throw ()
    {
        return !(lhs == rhs);
    }


    class OPENVRML_SCOPE SFVec2f : public FieldValue {
    public:
        vec2f value;

        explicit SFVec2f(const vec2f & vec = vec2f()) throw ();
        virtual ~SFVec2f() throw ();

        // Use compiler-defined copy ctor and operator=.

        virtual std::auto_ptr<FieldValue> clone() const throw (std::bad_alloc);
        virtual FieldValue & assign(const FieldValue & value)
            throw (std::bad_cast);
        virtual Type type() const throw ();
    
    private:
        virtual void print(std::ostream &) const;
    };

    inline bool operator==(const SFVec2f & lhs, const SFVec2f & rhs) throw ()
    {
        return lhs.value == rhs.value;
    }

    inline bool operator!=(const SFVec2f & lhs, const SFVec2f & rhs) throw ()
    {
        return !(lhs == rhs);
    }


    class OPENVRML_SCOPE SFVec3f : public FieldValue {
    public:
        vec3f value;

        explicit SFVec3f(const vec3f & vec = vec3f()) throw ();
        virtual ~SFVec3f() throw ();

        // Use compiler-defined copy constructor and operator=.

        virtual std::auto_ptr<FieldValue> clone() const throw (std::bad_alloc);
        virtual FieldValue & assign(const FieldValue & value)
            throw (std::bad_cast);
        virtual Type type() const throw ();
    
    private:
        virtual void print(std::ostream &) const;
    };

    inline bool operator==(const SFVec3f & lhs, const SFVec3f & rhs) throw ()
    {
        return lhs.value == rhs.value;
    }

    inline bool operator!=(const SFVec3f & lhs, const SFVec3f & rhs) throw ()
    {
        return !(lhs == rhs);
    }


    class OPENVRML_SCOPE MFColor : public FieldValue {
    public:
        std::vector<color> value;

        explicit MFColor(std::vector<color>::size_type n = 0,
                         const color & value = color())
            throw (std::bad_alloc);
        template <typename InputIterator>
        MFColor(InputIterator first, InputIterator last);
        virtual ~MFColor() throw ();

        // Use compiler-defined copy constructor and operator=.

        virtual std::auto_ptr<FieldValue> clone() const throw (std::bad_alloc);
        virtual FieldValue & assign(const FieldValue & value)
            throw (std::bad_cast, std::bad_alloc);
        virtual Type type() const throw ();
    
    private:
        virtual void print(std::ostream &) const;
    };

    template <typename InputIterator>
    MFColor::MFColor(InputIterator first, InputIterator last):
        value(first, last)
    {}

    inline bool operator==(const MFColor & lhs, const MFColor & rhs) throw ()
    {
        return lhs.value == rhs.value;
    }

    inline bool operator!=(const MFColor & lhs, const MFColor & rhs) throw ()
    {
        return lhs.value != rhs.value;
    }


    class OPENVRML_SCOPE MFFloat : public FieldValue {
    public:
        std::vector<float> value;

        explicit MFFloat(std::vector<float>::size_type n = 0,
                         float value = 0.0f)
            throw (std::bad_alloc);
        template <typename InputIterator>
        MFFloat(InputIterator first, InputIterator last);
        virtual ~MFFloat() throw ();

        // Use compiler-defined copy constructor and operator=.

        virtual std::auto_ptr<FieldValue> clone() const throw (std::bad_alloc);
        virtual FieldValue & assign(const FieldValue & value)
            throw (std::bad_cast, std::bad_alloc);
        virtual Type type() const throw ();
    
    private:
        virtual void print(std::ostream &) const;
    };

    template <typename InputIterator>
    MFFloat::MFFloat(InputIterator first, InputIterator last):
        value(first, last)
    {}

    inline bool operator==(const MFFloat & lhs, const MFFloat & rhs) throw ()
    {
        return lhs.value == rhs.value;
    }

    inline bool operator!=(const MFFloat & lhs, const MFFloat & rhs) throw ()
    {
        return lhs.value != rhs.value;
    }


    class OPENVRML_SCOPE MFInt32 : public FieldValue {
    public:
        std::vector<int32> value;

        explicit MFInt32(std::vector<int32>::size_type n = 0, int32 value = 0)
            throw (std::bad_alloc);
        template <typename InputIterator>
        MFInt32(InputIterator first, InputIterator last);
        virtual ~MFInt32() throw ();

        // Use compiler-defined copy constructor and operator=.

        virtual std::auto_ptr<FieldValue> clone() const throw (std::bad_alloc);
        virtual FieldValue & assign(const FieldValue & value)
            throw (std::bad_cast, std::bad_alloc);
        virtual Type type() const throw ();
    
    private:
        virtual void print(std::ostream &) const;
    };

    template <typename InputIterator>
    MFInt32::MFInt32(InputIterator first, InputIterator last):
        value(first, last)
    {}

    inline bool operator==(const MFInt32 & lhs, const MFInt32 & rhs) throw ()
    {
        return lhs.value == rhs.value;
    }

    inline bool operator!=(const MFInt32 & lhs, const MFInt32 & rhs) throw ()
    {
        return lhs.value != rhs.value;
    }


    class OPENVRML_SCOPE MFNode : public FieldValue {
    public:
        std::vector<NodePtr> value;

        explicit MFNode(std::vector<NodePtr>::size_type n = 0,
                        const NodePtr & value = NodePtr())
            throw (std::bad_alloc);
        template <typename InputIterator>
        MFNode(InputIterator first, InputIterator last);
        virtual ~MFNode() throw ();

        // Use compiler-defined copy constructor and operator=.

        virtual std::auto_ptr<FieldValue> clone() const throw (std::bad_alloc);
        virtual FieldValue & assign(const FieldValue & value)
            throw (std::bad_cast, std::bad_alloc);
        virtual Type type() const throw ();
    
    private:
        virtual void print(std::ostream &) const;
    };

    template <typename InputIterator>
    MFNode::MFNode(InputIterator first, InputIterator last):
        value(first, last)
    {}

    inline bool operator==(const MFNode & lhs, const MFNode & rhs) throw ()
    {
        return lhs.value == rhs.value;
    }

    inline bool operator!=(const MFNode & lhs, const MFNode & rhs) throw ()
    {
        return lhs.value != rhs.value;
    }


    class OPENVRML_SCOPE MFRotation : public FieldValue {
    public:
        std::vector<rotation> value;

        explicit MFRotation(std::vector<rotation>::size_type n = 0,
                            const rotation & value = rotation())
            throw (std::bad_alloc);
        template <typename InputIterator>
        MFRotation(InputIterator first, InputIterator last);
        virtual ~MFRotation() throw ();

        // Use compiler-defined copy constructor and operator=.

        virtual std::auto_ptr<FieldValue> clone() const throw (std::bad_alloc);
        virtual FieldValue & assign(const FieldValue & value)
            throw (std::bad_cast, std::bad_alloc);
        virtual Type type() const throw ();
    
    private:
        virtual void print(std::ostream &) const;
    };

    template <typename InputIterator>
    MFRotation::MFRotation(InputIterator first, InputIterator last):
        value(first, last)
    {}

    inline bool operator==(const MFRotation & lhs, const MFRotation & rhs)
        throw ()
    {
        return lhs.value == rhs.value;
    }

    inline bool operator!=(const MFRotation & lhs, const MFRotation & rhs)
        throw ()
    {
        return lhs.value != rhs.value;
    }


    class OPENVRML_SCOPE MFString : public FieldValue {
    public:
        std::vector<std::string> value;

        explicit MFString(std::vector<std::string>::size_type n = 0,
                          const std::string & value = std::string())
            throw (std::bad_alloc);
        template <typename InputIterator>
        MFString(InputIterator first, InputIterator last);
        virtual ~MFString() throw ();

        // Use compiler-defined copy constructor and operator=.

        virtual std::auto_ptr<FieldValue> clone() const throw (std::bad_alloc);
        virtual FieldValue & assign(const FieldValue & value)
            throw (std::bad_cast, std::bad_alloc);
        virtual Type type() const throw ();
    
    private:
        virtual void print(std::ostream &) const;
    };

    template <typename InputIterator>
    MFString::MFString(InputIterator first, InputIterator last):
        value(first, last)
    {}

    inline bool operator==(const MFString & lhs, const MFString & rhs) throw ()
    {
        return lhs.value == rhs.value;
    }

    inline bool operator!=(const MFString & lhs, const MFString & rhs) throw ()
    {
        return lhs.value != rhs.value;
    }


    class OPENVRML_SCOPE MFTime : public FieldValue {
    public:
        std::vector<double> value;

        explicit MFTime(std::vector<double>::size_type n = 0,
                        double value = 0.0)
            throw (std::bad_alloc);
        template <typename InputIterator>
        MFTime(InputIterator first, InputIterator last);
        virtual ~MFTime() throw ();

        // Use compiler-defined copy constructor and operator=.

        virtual std::auto_ptr<FieldValue> clone() const throw (std::bad_alloc);
        virtual FieldValue & assign(const FieldValue & value)
            throw (std::bad_cast, std::bad_alloc);
        virtual Type type() const throw ();
    
    private:
        virtual void print(std::ostream &) const;
    };

    template <typename InputIterator>
    MFTime::MFTime(InputIterator first, InputIterator last):
        value(first, last)
    {}

    inline bool operator==(const MFTime & lhs, const MFTime & rhs) throw ()
    {
        return lhs.value == rhs.value;
    }

    inline bool operator!=(const MFTime & lhs, const MFTime & rhs) throw ()
    {
        return lhs.value != rhs.value;
    }


    class OPENVRML_SCOPE MFVec2f : public FieldValue {
    public:
        std::vector<vec2f> value;

        explicit MFVec2f(std::vector<vec2f>::size_type n = 0,
                         const vec2f & value = vec2f())
            throw (std::bad_alloc);
        template <typename InputIterator>
        MFVec2f(InputIterator first, InputIterator last);
        virtual ~MFVec2f() throw ();

        // Use compiler-defined copy constructor and operator=.

        virtual std::auto_ptr<FieldValue> clone() const throw (std::bad_alloc);
        virtual FieldValue & assign(const FieldValue & value)
            throw (std::bad_cast, std::bad_alloc);
        virtual Type type() const throw ();
    
    private:
        virtual void print(std::ostream &) const;
    };

    template <typename InputIterator>
    MFVec2f::MFVec2f(InputIterator first, InputIterator last):
        value(first, last)
    {}

    inline bool operator==(const MFVec2f & lhs, const MFVec2f & rhs) throw ()
    {
        return lhs.value == rhs.value;
    }

    inline bool operator!=(const MFVec2f & lhs, const MFVec2f & rhs) throw ()
    {
        return lhs.value != rhs.value;
    }


    class OPENVRML_SCOPE MFVec3f : public FieldValue {
    public:
        std::vector<vec3f> value;

        explicit MFVec3f(std::vector<vec3f>::size_type n = 0,
                         const vec3f & value = vec3f())
            throw (std::bad_alloc);
        template <typename InputIterator>
        MFVec3f(InputIterator first, InputIterator last);
        virtual ~MFVec3f() throw ();

        // Use compiler-defined copy constructor and operator=.

        virtual std::auto_ptr<FieldValue> clone() const throw (std::bad_alloc);
        virtual FieldValue & assign(const FieldValue & value)
            throw (std::bad_cast, std::bad_alloc);
        virtual Type type() const throw ();
    
    private:
        virtual void print(std::ostream &) const;
    };

    template <typename InputIterator>
    MFVec3f::MFVec3f(InputIterator first, InputIterator last):
        value(first, last)
    {}

    inline bool operator==(const MFVec3f & lhs, const MFVec3f & rhs) throw ()
    {
        return lhs.value == rhs.value;
    }

    inline bool operator!=(const MFVec3f & lhs, const MFVec3f & rhs) throw ()
    {
        return lhs.value != rhs.value;
    }
}

namespace std {

    template <>
    inline void swap(OpenVRML::MFColor & a, OpenVRML::MFColor & b)
    {
        a.value.swap(b.value);
    }

    template <>
    inline void swap(OpenVRML::MFFloat & a, OpenVRML::MFFloat & b)
    {
        a.value.swap(b.value);
    }

    template <>
    inline void swap(OpenVRML::MFInt32 & a, OpenVRML::MFInt32 & b)
    {
        a.value.swap(b.value);
    }

    template <>
    inline void swap(OpenVRML::MFNode & a, OpenVRML::MFNode & b)
    {
        a.value.swap(b.value);
    }

    template <>
    inline void swap(OpenVRML::MFRotation & a, OpenVRML::MFRotation & b)
    {
        a.value.swap(b.value);
    }

    template <>
    inline void swap(OpenVRML::MFString & a, OpenVRML::MFString & b)
    {
        a.value.swap(b.value);
    }

    template <>
    inline void swap(OpenVRML::MFTime & a, OpenVRML::MFTime & b)
    {
        a.value.swap(b.value);
    }

    template <>
    inline void swap(OpenVRML::MFVec2f & a, OpenVRML::MFVec2f & b)
    {
        a.value.swap(b.value);
    }

    template <>
    inline void swap(OpenVRML::MFVec3f & a, OpenVRML::MFVec3f & b)
    {
        a.value.swap(b.value);
    }
}

# endif
