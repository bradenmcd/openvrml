// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML
//
// Copyright 2001  S. K. Bose
// Copyright 2003, 2004, 2005, 2006  Braden McDaniel
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

# include <numeric>
# include <ostream>
# include <boost/spirit.hpp>
# include <boost/spirit/actor.hpp>
# include <boost/spirit/dynamic.hpp>
# include <boost/spirit/phoenix.hpp>
# include <private.h>
# include "basetypes.h"

namespace {

    // Conversion functions between RGB each in [0,1] and HSV with
    // h in [0,360), s,v in [0,1]. From Foley, van Dam p615-616.

    OPENVRML_LOCAL void rgb_to_hsv(const float * const rgb,
                                   float * const hsv)
        OPENVRML_NOTHROW
    {
        assert(rgb[0] >= 0.0 && rgb[0] <= 1.0);
        assert(rgb[1] >= 0.0 && rgb[1] <= 1.0);
        assert(rgb[2] >= 0.0 && rgb[2] <= 1.0);

        const float maxrgb = *std::max_element(rgb, rgb + 3);
        const float minrgb = *std::min_element(rgb, rgb + 3);

        hsv[0] = 0.0;
        hsv[1] = (maxrgb > 0.0) ? ((maxrgb - minrgb) / maxrgb) : 0.0f;
        hsv[2] = maxrgb;

        if (hsv[1] != 0.0) {
            const float rc = (maxrgb - rgb[0]) / (maxrgb - minrgb);
            const float gc = (maxrgb - rgb[1]) / (maxrgb - minrgb);
            const float bc = (maxrgb - rgb[2]) / (maxrgb - minrgb);

            if (rgb[0] == maxrgb) {
                hsv[0] = bc - gc;
            } else if (rgb[1] == maxrgb) {
                hsv[0] = 2 + rc - bc;
            } else {
                hsv[0] = 4 + gc - rc;
            }

            hsv[0] *= 60.0;
            if (hsv[0] < 0.0) { hsv[0] += 360.0; }
        }
    }

    OPENVRML_LOCAL void hsv_to_rgb(float h,
                                   const float s,
                                   const float v,
                                   float * const rgb)
        OPENVRML_NOTHROW
    {
        assert(h >= 0.0 && h <= 360.0);
        assert(s >= 0.0 && s <= 1.0);
        assert(v >= 0.0 && v <= 1.0);

        if (s == 0.0) {
            rgb[0] = rgb[1] = rgb[2] = v;
        } else {
            if (h >= 360.0) { h -= 360.0; }
            h /= 60.0;
            const double i = floor(h);
            const double f = h - i;
            const float p = float(v * (1.0 - s));
            const float q = float(v * (1.0 - s * f));
            const float t = float(v * (1.0 - s * (1.0 - f)));
            switch (int(i)) {
            default:
            case 0: rgb[0] = v; rgb[1] = t; rgb[2] = p; break;
            case 1: rgb[0] = q; rgb[1] = v; rgb[2] = p; break;
            case 2: rgb[0] = p; rgb[1] = v; rgb[2] = t; break;
            case 3: rgb[0] = p; rgb[1] = q; rgb[2] = v; break;
            case 4: rgb[0] = t; rgb[1] = p; rgb[2] = v; break;
            case 5: rgb[0] = v; rgb[1] = p; rgb[2] = q; break;
            }
        }
    }
}

/**
 * @typedef openvrml::int32
 *
 * @brief 32-bit signed integer.
 */

/**
 * @class openvrml::color
 *
 * @brief A color.
 *
 * VRML colors are represented as three single-precision floating point
 * components&mdash;red, green, and blue&mdash;ranging from 0.0 to 1.0.
 */

/**
 * @internal
 *
 * @var float openvrml::color::rgb[3]
 *
 * @brief RGB triplet.
 */

/**
 * @brief Construct.
 */
openvrml::color::color() OPENVRML_NOTHROW
{}

/**
 * @brief Construct from a 3-element float array.
 *
 * @param[in] rgb   an array comprising red, green, and blue components.
 */
openvrml::color::color(const float (&rgb)[3]) OPENVRML_NOTHROW
{
    assert(rgb[0] >= 0.0 && rgb[0] <= 1.0);
    this->rgb[0] = rgb[0];
    assert(rgb[1] >= 0.0 && rgb[1] <= 1.0);
    this->rgb[1] = rgb[1];
    assert(rgb[2] >= 0.0 && rgb[2] <= 1.0);
    this->rgb[2] = rgb[2];
}

/**
 * @brief Construct from red, green, and blue components.
 *
 * @param[in] r red component.
 * @param[in] g green component.
 * @param[in] b blue component.
 */
openvrml::color::color(float r, float g, float b) OPENVRML_NOTHROW
{
    assert(r >= 0.0 && r <= 1.0);
    this->rgb[0] = r;
    assert(g >= 0.0 && g <= 1.0);
    this->rgb[1] = g;
    assert(b >= 0.0 && b <= 1.0);
    this->rgb[2] = b;
}

/**
 * @fn const float & openvrml::color::operator[](const size_t index) const
 *
 * @brief Index-based component access.
 *
 * @param[in] index 0 is the red component, 1 is the green component, and 2 is
 *                  the blue component.
 *
 * @return the component corresponding to @p index.
 */

/**
 * @fn float openvrml::color::r() const
 *
 * @brief Get the red component.
 *
 * @return the red component.
 */

/**
 * @fn float openvrml::color::g() const
 *
 * @brief Get the green component.
 *
 * @return the green component.
 */

/**
 * @fn float openvrml::color::b() const
 *
 * @brief Get the blue component.
 *
 * @return the blue component.
 */

/**
 * @fn void openvrml::color::r(float value)
 *
 * @brief Set the red component.
 *
 * @param[in] value the new component value.
 */

/**
 * @fn void openvrml::color::g(float value)
 *
 * @brief Set the green component.
 *
 * @param[in] value the new component value.
 */

/**
 * @fn void openvrml::color::b(float value)
 *
 * @brief Set the blue component.
 *
 * @param[in] value the new component value.
 */

// Conversion functions between RGB each in [0,1] and HSV with
// h in [0,360), s,v in [0,1]. From Foley, van Dam p615-616.

/**
 * @brief Get the color as hue, saturation, and value.
 *
 * @param[out] result   the hue, saturation, and value.
 */
void openvrml::color::hsv(float (&result)[3]) const OPENVRML_NOTHROW
{
    rgb_to_hsv(&rgb[0], &result[0]);
}

/**
 * @brief Set the color from hue, saturation, and value.
 *
 * @param[in] h hue.
 * @param[in] s saturation.
 * @param[in] v value.
 */
void openvrml::color::hsv(float h, const float s, const float v)
    OPENVRML_NOTHROW
{
    hsv_to_rgb(h, s, v, &rgb[0]);
}

/**
 * @relatesalso openvrml::color
 *
 * @brief Compare for equality.
 *
 * @param[in] lhs   left-hand operand.
 * @param[in] rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs have the same value; @c false
 *         otherwise.
 */
bool openvrml::operator==(const color & lhs, const color & rhs)
    OPENVRML_NOTHROW
{
    using openvrml_::fequal;
    return std::equal(&lhs[0], &lhs[0] + 3, &rhs[0], fequal);
}

/**
 * @relatesalso openvrml::color
 *
 * @brief Compare for inequality.
 *
 * @param[in] lhs   left-hand operand.
 * @param[in] rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs do not have the same value; @c false
 *         otherwise.
 */
bool openvrml::operator!=(const color & lhs, const color & rhs)
    OPENVRML_NOTHROW
{
    return !(lhs == rhs);
}

namespace {
    struct OPENVRML_LOCAL vrml97_space_parser :
        boost::spirit::char_parser<vrml97_space_parser> {

        typedef vrml97_space_parser self_t;

        template <typename CharT>
        bool test(const CharT & c) const
        {
            return isspace(c) || c == ',';
        }
    };

    const vrml97_space_parser vrml97_space_p = vrml97_space_parser();

    typedef std::istream::char_type char_t;
    typedef boost::spirit::multi_pass<std::istreambuf_iterator<char_t> >
        iterator_t;

    typedef boost::spirit::skip_parser_iteration_policy<vrml97_space_parser>
        iter_policy_t;
    typedef boost::spirit::scanner_policies<iter_policy_t> scanner_policies_t;
    typedef boost::spirit::scanner<iterator_t, scanner_policies_t> scanner_t;

    typedef boost::spirit::rule<scanner_t> rule_t;


    struct OPENVRML_LOCAL intensity_parser {

        typedef double result_t;

        struct valid {
            explicit valid(result_t & value):
                value_(value)
            {}

            bool operator()() const
            {
                return this->value_ >= 0.0 && this->value_ <= 1.0;
            }

        private:
            result_t & value_;
        };

        template <typename ScannerT>
        std::ptrdiff_t operator()(const ScannerT & scan,
                                  result_t & result) const
        {
            using boost::spirit::assign_a;
            using boost::spirit::eps_p;
            using boost::spirit::real_p;
            using boost::spirit::match_result;
            typedef typename match_result<ScannerT, result_t>::type match_t;
            typedef typename boost::spirit::rule<ScannerT> rule_t;

            rule_t rule
                =   real_p[assign_a(result)] >> eps_p(valid(result))
                ;
            match_t match = rule.parse(scan);
            return match.length();
        }
    };

    const boost::spirit::functor_parser<intensity_parser> intensity_p;
}

/**
 * @brief Stream input.
 *
 * Consistent with the VRML97 convention, commas (&ldquo;@c ,&rdquo;) in the
 * input are treated as whitespace.
 *
 * If any of the color components are outside the the range [0.0, 1.0], the
 * @c failbit will be set on @p in and @p c will be left in an arbitrary
 * state.
 *
 * @param[in,out] in    input stream.
 * @param[out] c        a @c color.
 *
 * @return @p in.
 */
std::istream & openvrml::operator>>(std::istream & in, color & c)
{
    using std::istreambuf_iterator;
    using boost::spirit::make_multi_pass;
    using boost::spirit::match;
    using boost::spirit::real_p;
    using boost::spirit::assign_a;

    iter_policy_t iter_policy(vrml97_space_p);
    scanner_policies_t policies(iter_policy);
    iterator_t
        first(make_multi_pass(istreambuf_iterator<char_t>(in))),
        last(make_multi_pass(istreambuf_iterator<char_t>()));

    scanner_t scan(first, last, policies);

    rule_t rule
        =   intensity_p[assign_a(c.rgb[0])]
            >> intensity_p[assign_a(c.rgb[1])]
            >> intensity_p[assign_a(c.rgb[2])]
        ;

    match<> m = rule.parse(scan);

    if (!m) {
        in.setstate(std::ios_base::failbit);
        return in;
    }

    return in;
}

/**
 * @relatesalso openvrml::color
 *
 * @brief Stream output.
 *
 * @param[in,out] out   output stream.
 * @param[in] c         a @c color.
 *
 * @return @p out.
 */
std::ostream & openvrml::operator<<(std::ostream & out, const color & c)
{
    return out << c.r() << ' ' << c.g() << ' ' << c.b();
}

/**
 * @class openvrml::color_rgba
 *
 * @brief A color with alpha channel.
 *
 * VRML @c color_rgba%s are represented as four single-precision floating
 * point components&mdash;red, green, blue, and alpha&mdash;ranging from 0.0
 * to 1.0. For the alpha channel, 1.0 is opaque.
 */

/**
 * @internal
 *
 * @var float openvrml::color_rgba::rgba[4]
 *
 * @brief RGB triplet plus alpha.
 */

/**
 * @brief Construct.
 */
openvrml::color_rgba::color_rgba() OPENVRML_NOTHROW
{}

/**
 * @brief Construct from a 4-element float array.
 *
 * @param[in] rgba   an array comprising red, green, and blue components.
 */
openvrml::color_rgba::color_rgba(const float (&rgba)[4]) OPENVRML_NOTHROW
{
    assert(rgba[0] >= 0.0 && rgba[0] <= 1.0);
    this->rgba[0] = rgba[0];
    assert(rgba[1] >= 0.0 && rgba[1] <= 1.0);
    this->rgba[1] = rgba[1];
    assert(rgba[2] >= 0.0 && rgba[2] <= 1.0);
    this->rgba[2] = rgba[2];
    assert(rgba[3] >= 0.0 && rgba[3] <= 1.0);
    this->rgba[3] = rgba[3];
}

/**
 * @brief Construct from red, green, and blue components.
 *
 * @param[in] r red component.
 * @param[in] g green component.
 * @param[in] b blue component.
 * @param[in] a alpha component.
 */
openvrml::color_rgba::color_rgba(float r, float g, float b, float a)
    OPENVRML_NOTHROW
{
    assert(r >= 0.0 && r <= 1.0);
    this->rgba[0] = r;
    assert(g >= 0.0 && g <= 1.0);
    this->rgba[1] = g;
    assert(b >= 0.0 && b <= 1.0);
    this->rgba[2] = b;
    assert(a >= 0.0 && a <= 1.0);
    this->rgba[3] = a;
}

/**
 * @fn const float & openvrml::color_rgba::operator[](const size_t index) const
 *
 * @brief Index-based component access.
 *
 * @param[in] index 0 is the red component, 1 is the green component, 2 is the
 *                  blue component, and 3 is the alpha component.
 *
 * @return the component corresponding to @p index.
 */

/**
 * @fn float openvrml::color_rgba::r() const
 *
 * @brief Get the red component.
 *
 * @return the red component.
 */

/**
 * @fn float openvrml::color_rgba::g() const
 *
 * @brief Get the green component.
 *
 * @return the green component.
 */

/**
 * @fn float openvrml::color_rgba::b() const
 *
 * @brief Get the blue component.
 *
 * @return the blue component.
 */

/**
 * @fn float openvrml::color_rgba::a() const
 *
 * @brief Get the alpha component.
 *
 * @return the alpha component.
 */

/**
 * @fn void openvrml::color_rgba::r(float value)
 *
 * @brief Set the red component.
 *
 * @param[in] value the new component value.
 */

/**
 * @fn void openvrml::color_rgba::g(float value)
 *
 * @brief Set the green component.
 *
 * @param[in] value the new component value.
 */

/**
 * @fn void openvrml::color_rgba::b(float value)
 *
 * @brief Set the blue component.
 *
 * @param[in] value the new component value.
 */

/**
 * @fn void openvrml::color_rgba::a(float value)
 *
 * @brief Set the alpha component.
 *
 * @param[in] value the new component value.
 */

/**
 * @brief Get the color_rgba as hue, saturation, and value.
 *
 * @param[out] result   the hue, saturation, and value.
 */
void openvrml::color_rgba::hsv(float (&result)[4]) const OPENVRML_NOTHROW
{
    rgb_to_hsv(&rgba[0], &result[0]);
    result[3] = rgba[3]; // Copy the alpha.
}

/**
 * @brief Set the color_rgba from hue, saturation, and value.
 *
 * @param[in] h hue.
 * @param[in] s saturation.
 * @param[in] v value.
 * @param[in] a alpha.
 */
void openvrml::color_rgba::hsv(const float h,
                               const float s,
                               const float v,
                               const float a)
    OPENVRML_NOTHROW
{
    hsv_to_rgb(h, s, v, &rgba[0]);
    assert(a >= 0.0 && a <= 1.0);
    rgba[3] = a;
}

/**
 * @relatesalso openvrml::color_rgba
 *
 * @brief Compare for equality.
 *
 * @param[in] lhs   left-hand operand.
 * @param[in] rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs have the same value; @c false
 *         otherwise.
 */
bool openvrml::operator==(const color_rgba & lhs, const color_rgba & rhs)
    OPENVRML_NOTHROW
{
    using openvrml_::fequal;
    return std::equal(&lhs[0], &lhs[0] + 4, &rhs[0], fequal);
}

/**
 * @relatesalso openvrml::color_rgba
 *
 * @brief Compare for inequality.
 *
 * @param[in] lhs   left-hand operand.
 * @param[in] rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs dot not have the same value; @c false
 *         otherwise.
 */
bool openvrml::operator!=(const color_rgba & lhs, const color_rgba & rhs)
    OPENVRML_NOTHROW
{
    return !(lhs == rhs);
}

/**
 * @brief Stream input.
 *
 * Consistent with the VRML97 convention, commas (&ldquo;@c ,&rdquo;) in the
 * input are treated as whitespace.
 *
 * If any of the color components are outside the the range [0.0, 1.0], the
 * @c failbit will be set on @p in and @p c will be left in an arbitrary
 * state.
 *
 * @param[in,out] in    input stream.
 * @param[out] c        a @c color_rgba.
 *
 * @return @p in.
 */
std::istream & openvrml::operator>>(std::istream & in, color_rgba & c)
{
    using std::istreambuf_iterator;
    using boost::spirit::make_multi_pass;
    using boost::spirit::match;
    using boost::spirit::real_p;
    using boost::spirit::assign_a;

    iter_policy_t iter_policy(vrml97_space_p);
    scanner_policies_t policies(iter_policy);
    iterator_t
        first(make_multi_pass(istreambuf_iterator<char_t>(in))),
        last(make_multi_pass(istreambuf_iterator<char_t>()));

    scanner_t scan(first, last, policies);

    rule_t rule
        =   intensity_p[assign_a(c.rgba[0])]
            >> intensity_p[assign_a(c.rgba[1])]
            >> intensity_p[assign_a(c.rgba[2])]
            >> intensity_p[assign_a(c.rgba[3])];

    match<> m = rule.parse(scan);

    if (!m) {
        in.setstate(std::ios_base::failbit);
        return in;
    }

    return in;
}

/**
 * @relatesalso openvrml::color_rgba
 *
 * @brief Stream output.
 *
 * @param[in,out] out   output stream.
 * @param[in] c     a color_rgba.
 *
 * @return @p out.
 */
std::ostream & openvrml::operator<<(std::ostream & out, const color_rgba & c)
{
    return out << c.r() << ' ' << c.g() << ' ' << c.b() << ' ' << c.a();
}


/**
 * @class openvrml::vec2f
 *
 * @brief Two-component single-precision vector.
 */

/**
 * @internal
 *
 * @var float openvrml::vec2f::vec[2]
 *
 * @brief Vector components.
 */

/**
 * @brief Construct.
 */
openvrml::vec2f::vec2f() OPENVRML_NOTHROW
{
    this->vec[0] = 0.0;
    this->vec[1] = 0.0;
}

/**
 * @brief Construct from an array.
 *
 * @pre Elements of @p vec are valid numeric values (i.e., not NaN).
 *
 * @param[in] vec   an array comprising the vector components.
 */
openvrml::vec2f::vec2f(const float (&vec)[2]) OPENVRML_NOTHROW
{
    //
    // Ensure elements of vec are not NaN.
    //
    assert(vec[0] == vec[0]);
    assert(vec[1] == vec[1]);

    this->vec[0] = vec[0];
    this->vec[1] = vec[1];
}

/**
 * @brief Construct from @p x and @p y components.
 *
 * @pre @p x and @p y are valid numeric values (i.e., not NaN).
 *
 * @param[in] x x component.
 * @param[in] y y component.
 */
openvrml::vec2f::vec2f(const float x, const float y) OPENVRML_NOTHROW
{
    //
    // Ensure x and y are not NaN.
    //
    assert(x == x);
    assert(y == y);

    this->vec[0] = x;
    this->vec[1] = y;
}

/**
 * @brief Multiply by a scalar.
 *
 * @param[in] scalar    factor by which to multiply.
 *
 * @return a reference to the object.
 */
openvrml::vec2f & openvrml::vec2f::operator*=(const float scalar)
    OPENVRML_NOTHROW
{
    this->vec[0] *= scalar;
    this->vec[1] *= scalar;
    return *this;
}

/**
 * @relatesalso openvrml::vec2f
 *
 * @brief Multiply a vector by a scalar.
 *
 * @param[in] lhs   left-hand operand (the vector).
 * @param[in] rhs   right-hand operand (the scalar).
 *
 * @return the result vector.
 */
const openvrml::vec2f openvrml::operator*(const vec2f & lhs, const float rhs)
    OPENVRML_NOTHROW
{
    vec2f result(lhs);
    return result *= rhs;
}

/**
 * @relatesalso openvrml::vec2f
 *
 * @brief Multiply a vector by a scalar.
 *
 * @param[in] lhs   left-hand operand (the scalar).
 * @param[in] rhs   right-hand operand (the vector).
 *
 * @return the result vector.
 */
const openvrml::vec2f openvrml::operator*(const float lhs, const vec2f & rhs)
    OPENVRML_NOTHROW
{
    vec2f result(rhs);
    return result *= lhs;
}

/**
 * @brief Divide by a scalar.
 *
 * @pre @p scalar is nonzero.
 *
 * @param[in] scalar    divisor.
 *
 * @return a reference to the object.
 */
openvrml::vec2f & openvrml::vec2f::operator/=(const float scalar)
    OPENVRML_NOTHROW
{
    assert(scalar != 0.0);
    this->vec[0] /= scalar;
    this->vec[1] /= scalar;
    return *this;
}

/**
 * @relatesalso openvrml::vec2f
 *
 * @brief Divide a vector by a scalar.
 *
 * @pre @p rhs is nonzero.
 *
 * @param[in] lhs   left-hand operand (the vector).
 * @param[in] rhs   right-hand operand (the scalar).
 *
 * @return the result vector.
 */
const openvrml::vec2f openvrml::operator/(const vec2f & lhs, const float rhs)
    OPENVRML_NOTHROW
{
    vec2f result(lhs);
    return result /= rhs;
}

/**
 * @brief Add a vector.
 *
 * @param[in] vec   the vector to add.
 *
 * @return a reference to the object.
 */
openvrml::vec2f & openvrml::vec2f::operator+=(const vec2f & vec)
    OPENVRML_NOTHROW
{
    this->vec[0] += vec[0];
    this->vec[1] += vec[1];
    return *this;
}

/**
 * @relatesalso openvrml::vec2f
 *
 * @brief Add two vectors.
 *
 * @param[in] lhs   left-hand operand.
 * @param[in] rhs   right-hand operand.
 *
 * @return the result vector.
 */
const openvrml::vec2f openvrml::operator+(const vec2f & lhs, const vec2f & rhs)
    OPENVRML_NOTHROW
{
    vec2f result(lhs);
    return result += rhs;
}

/**
 * @brief Subtract a vector.
 *
 * @param[in] vec   the vector to subtract.
 *
 * @return a reference to the object.
 */
openvrml::vec2f & openvrml::vec2f::operator-=(const vec2f & vec)
    OPENVRML_NOTHROW
{
    this->vec[0] -= vec[0];
    this->vec[1] -= vec[1];
    return *this;
}

/**
 * @relatesalso openvrml::vec2f
 *
 * @brief Subtract two vectors.
 *
 * @param[in] lhs   left-hand operand.
 * @param[in] rhs   right-hand operand.
 *
 * @return the result vector.
 */
const openvrml::vec2f openvrml::operator-(const vec2f & lhs, const vec2f & rhs)
    OPENVRML_NOTHROW
{
    vec2f result(lhs);
    return result -= rhs;
}

/**
 * @brief Negate.
 *
 * @return the additive inverse of the vector.
 */
const openvrml::vec2f openvrml::vec2f::operator-() const OPENVRML_NOTHROW
{
    return vec2f(-this->vec[0], -this->vec[1]);
}

/**
 * @fn const float & openvrml::vec2f::operator[](const size_t index) const
 *
 * @brief Index-based component access.
 *
 * @param[in] index 0 corresponds to the x component; 1 corresponds to the y
 *                  component.
 *
 * @return a const reference to the component corresponding to @p index.
 *
 * @pre @p index is less than 2.
 */

/**
 * @fn float openvrml::vec2f::x() const
 *
 * @brief Get the x component.
 *
 * @return the x component.
 */

/**
 * @fn float openvrml::vec2f::y() const
 *
 * @brief Get the y component.
 *
 * @return the y component.
 */

/**
 * @fn void openvrml::vec2f::x(float value)
 *
 * @brief Set the x component.
 *
 * @pre @p value is a valid numeric value (i.e., not NaN).
 *
 * @param[in] value new x component value.
 */

/**
 * @fn void openvrml::vec2f::y(float value)
 *
 * @brief Set the y component.
 *
 * @pre @p value is a valid numeric value (i.e., not NaN).
 *
 * @param[in] value new y component value.
 */

/**
 * @brief Dot product.
 *
 * @param[in] vec
 *
 * @return the dot product of the vector and @p vec.
 */
float openvrml::vec2f::dot(const vec2f & vec) const OPENVRML_NOTHROW
{
    return this->vec[0] * vec[0]
        + this->vec[1] * vec[1];
}

/**
 * @brief Geometric length.
 *
 * @return the length of the vector.
 */
float openvrml::vec2f::length() const OPENVRML_NOTHROW
{
    return float(sqrt(this->dot(*this)));
}

/**
 * @brief Normalize.
 *
 * @return the vector normalized.
 */
const openvrml::vec2f openvrml::vec2f::normalize() const OPENVRML_NOTHROW
{
    using openvrml_::fequal;
    const float len = this->length();
    vec2f result(*this);
    if (!fequal(len, 0.0f)) { result /= len; }
    return result;
}

/**
 * @relatesalso openvrml::vec2f
 *
 * @brief Compare for equality.
 *
 * @param[in] lhs   left-hand operand.
 * @param[in] rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs have the same value; @c false
 *         otherwise.
 */
bool openvrml::operator==(const vec2f & lhs, const vec2f & rhs)
    OPENVRML_NOTHROW
{
    using openvrml_::fequal;
    return std::equal(&lhs[0], &lhs[0] + 2, &rhs[0], fequal);
}

/**
 * @relatesalso openvrml::vec2f
 *
 * @brief Compare for inequality.
 *
 * @param[in] lhs   left-hand operand.
 * @param[in] rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs dot not have the same value; @c false
 *         otherwise.
 */
bool openvrml::operator!=(const vec2f & lhs, const vec2f & rhs)
    OPENVRML_NOTHROW
{
    return !(lhs == rhs);
}

/**
 * @brief Stream input.
 *
 * Consistent with the VRML97 convention, commas (&ldquo;@c ,&rdquo;) in the
 * input are treated as whitespace.
 *
 * @param[in,out] in    input stream.
 * @param[out] v        a @c vec2f.
 *
 * @return @p in.
 */
std::istream & openvrml::operator>>(std::istream & in, vec2f & v)
{
    using std::istreambuf_iterator;
    using boost::spirit::make_multi_pass;
    using boost::spirit::match;
    using boost::spirit::real_p;
    using boost::spirit::assign_a;

    iter_policy_t iter_policy(vrml97_space_p);
    scanner_policies_t policies(iter_policy);
    iterator_t
        first(make_multi_pass(istreambuf_iterator<char_t>(in))),
        last(make_multi_pass(istreambuf_iterator<char_t>()));

    scanner_t scan(first, last, policies);

    rule_t r
        =   real_p[assign_a(v.vec[0])] >> real_p[assign_a(v.vec[1])]
        ;

    match<> m = r.parse(scan);

    if (!m) {
        in.setstate(std::ios_base::failbit);
    }

    return in;
}

/**
 * @relatesalso openvrml::vec2f
 *
 * @brief Stream output.
 *
 * @param[in,out] out   output stream.
 * @param[in]     v     a 2-component vector.
 *
 * @return @p out.
 */
std::ostream & openvrml::operator<<(std::ostream & out, const vec2f & v)
{
    return out << v.x() << ' ' << v.y();
}


/**
 * @class openvrml::vec2d
 *
 * @brief Two-component double-precision vector.
 */

/**
 * @internal
 *
 * @var double openvrml::vec2d::vec[2]
 *
 * @brief Vector components.
 */

/**
 * @brief Construct.
 */
openvrml::vec2d::vec2d() OPENVRML_NOTHROW
{
    this->vec[0] = 0.0;
    this->vec[1] = 0.0;
}

/**
 * @brief Construct from an array.
 *
 * @pre Elements of @p vec are valid numeric values (i.e., not NaN).
 *
 * @param[in] vec   an array comprising the vector components.
 */
openvrml::vec2d::vec2d(const double (&vec)[2]) OPENVRML_NOTHROW
{
    //
    // Ensure elements of vec are not NaN.
    //
    assert(vec[0] == vec[0]);
    assert(vec[1] == vec[1]);

    this->vec[0] = vec[0];
    this->vec[1] = vec[1];
}

/**
 * @brief Construct from @p x and @p y components.
 *
 * @pre @p x and @p y are valid numeric values (i.e., not NaN).
 *
 * @param[in] x x component.
 * @param[in] y y component.
 */
openvrml::vec2d::vec2d(const double x, const double y) OPENVRML_NOTHROW
{
    //
    // Ensure x and y are not NaN.
    //
    assert(x == x);
    assert(y == y);

    this->vec[0] = x;
    this->vec[1] = y;
}

/**
 * @brief Multiply by a scalar.
 *
 * @param[in] scalar    factor by which to multiply.
 *
 * @return a reference to the object.
 */
openvrml::vec2d & openvrml::vec2d::operator*=(const double scalar)
    OPENVRML_NOTHROW
{
    this->vec[0] *= scalar;
    this->vec[1] *= scalar;
    return *this;
}

/**
 * @relatesalso openvrml::vec2d
 *
 * @brief Multiply a vector by a scalar.
 *
 * @param[in] lhs   left-hand operand (the vector).
 * @param[in] rhs   right-hand operand (the scalar).
 *
 * @return the result vector.
 */
const openvrml::vec2d openvrml::operator*(const vec2d & lhs, const double rhs)
    OPENVRML_NOTHROW
{
    vec2d result(lhs);
    return result *= rhs;
}

/**
 * @relatesalso openvrml::vec2d
 *
 * @brief Multiply a vector by a scalar.
 *
 * @param[in] lhs   left-hand operand (the scalar).
 * @param[in] rhs   right-hand operand (the vector).
 *
 * @return the result vector.
 */
const openvrml::vec2d openvrml::operator*(const double lhs, const vec2d & rhs)
    OPENVRML_NOTHROW
{
    vec2d result(rhs);
    return result *= lhs;
}

/**
 * @brief Divide by a scalar.
 *
 * @pre @p scalar is nonzero.
 *
 * @param[in] scalar    divisor.
 *
 * @return a reference to the object.
 */
openvrml::vec2d & openvrml::vec2d::operator/=(const double scalar)
    OPENVRML_NOTHROW
{
    assert(scalar != 0.0);
    this->vec[0] /= scalar;
    this->vec[1] /= scalar;
    return *this;
}

/**
 * @relatesalso openvrml::vec2d
 *
 * @brief Divide a vector by a scalar.
 *
 * @pre @p rhs is nonzero.
 *
 * @param[in] lhs   left-hand operand (the vector).
 * @param[in] rhs   right-hand operand (the scalar).
 *
 * @return the result vector.
 */
const openvrml::vec2d openvrml::operator/(const vec2d & lhs, const double rhs)
    OPENVRML_NOTHROW
{
    vec2d result(lhs);
    return result /= rhs;
}

/**
 * @brief Add a vector.
 *
 * @param[in] vec   the vector to add.
 *
 * @return a reference to the object.
 */
openvrml::vec2d & openvrml::vec2d::operator+=(const vec2d & vec)
    OPENVRML_NOTHROW
{
    this->vec[0] += vec[0];
    this->vec[1] += vec[1];
    return *this;
}

/**
 * @relatesalso openvrml::vec2d
 *
 * @brief Add two vectors.
 *
 * @param[in] lhs   left-hand operand.
 * @param[in] rhs   right-hand operand.
 *
 * @return the result vector.
 */
const openvrml::vec2d openvrml::operator+(const vec2d & lhs, const vec2d & rhs)
    OPENVRML_NOTHROW
{
    vec2d result(lhs);
    return result += rhs;
}

/**
 * @brief Subtract a vector.
 *
 * @param[in] vec   the vector to subtract.
 *
 * @return a reference to the object.
 */
openvrml::vec2d & openvrml::vec2d::operator-=(const vec2d & vec)
    OPENVRML_NOTHROW
{
    this->vec[0] -= vec[0];
    this->vec[1] -= vec[1];
    return *this;
}

/**
 * @relatesalso openvrml::vec2d
 *
 * @brief Subtract two vectors.
 *
 * @param[in] lhs   left-hand operand.
 * @param[in] rhs   right-hand operand.
 *
 * @return the result vector.
 */
const openvrml::vec2d openvrml::operator-(const vec2d & lhs, const vec2d & rhs)
    OPENVRML_NOTHROW
{
    vec2d result(lhs);
    return result -= rhs;
}

/**
 * @brief Negate.
 *
 * @return the additive inverse of the vector.
 */
const openvrml::vec2d openvrml::vec2d::operator-() const OPENVRML_NOTHROW
{
    return vec2d(-this->vec[0], -this->vec[1]);
}

/**
 * @fn const double & openvrml::vec2d::operator[](const size_t index) const
 *
 * @brief Index-based component access.
 *
 * @param[in] index 0 corresponds to the x component; 1 corresponds to the y
 *                  component.
 *
 * @return a const reference to the component corresponding to @p index.
 *
 * @pre @p index is less than 2.
 */

/**
 * @fn double openvrml::vec2d::x() const
 *
 * @brief Get the x component.
 *
 * @return the x component.
 */

/**
 * @fn double openvrml::vec2d::y() const
 *
 * @brief Get the y component.
 *
 * @return the y component.
 */

/**
 * @fn void openvrml::vec2d::x(double value)
 *
 * @brief Set the x component.
 *
 * @pre @p value is a valid numeric value (i.e., not NaN).
 *
 * @param[in] value new x component value.
 */

/**
 * @fn void openvrml::vec2d::y(double value)
 *
 * @brief Set the y component.
 *
 * @pre @p value is a valid numeric value (i.e., not NaN).
 *
 * @param[in] value new y component value.
 */

/**
 * @brief Dot product.
 *
 * @param[in] vec
 *
 * @return the dot product of the vector and @p vec.
 */
double openvrml::vec2d::dot(const vec2d & vec) const OPENVRML_NOTHROW
{
    return this->vec[0] * vec[0]
        + this->vec[1] * vec[1];
}

/**
 * @brief Geometric length.
 *
 * @return the length of the vector.
 */
double openvrml::vec2d::length() const OPENVRML_NOTHROW
{
    return double(sqrt(this->dot(*this)));
}

/**
 * @brief Normalize.
 *
 * @return the vector normalized.
 */
const openvrml::vec2d openvrml::vec2d::normalize() const OPENVRML_NOTHROW
{
    using openvrml_::fequal;
    const double len = this->length();
    vec2d result(*this);
    if (!fequal(len, 0.0)) { result /= len; }
    return result;
}

/**
 * @relatesalso openvrml::vec2d
 *
 * @brief Compare for equality.
 *
 * @param[in] lhs   left-hand operand.
 * @param[in] rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs have the same value; @c false
 *         otherwise.
 */
bool openvrml::operator==(const vec2d & lhs, const vec2d & rhs)
    OPENVRML_NOTHROW
{
    using openvrml_::fequal;
    return std::equal(&lhs[0], &lhs[0] + 2, &rhs[0], fequal);
}

/**
 * @relatesalso openvrml::vec2d
 *
 * @brief Compare for inequality.
 *
 * @param[in] lhs   left-hand operand.
 * @param[in] rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs dot not have the same value; @c false
 *         otherwise.
 */
bool openvrml::operator!=(const vec2d & lhs, const vec2d & rhs)
    OPENVRML_NOTHROW
{
    return !(lhs == rhs);
}

/**
 * @brief Stream input.
 *
 * Consistent with the VRML97 convention, commas (&ldquo;@c ,&rdquo;) in the
 * input are treated as whitespace.
 *
 * @param[in,out] in    input stream.
 * @param[out] v        a @c vec2d.
 *
 * @return @p in.
 */
std::istream & openvrml::operator>>(std::istream & in, vec2d & v)
{
    using std::istreambuf_iterator;
    using boost::spirit::make_multi_pass;
    using boost::spirit::match;
    using boost::spirit::real_p;
    using boost::spirit::assign_a;

    iter_policy_t iter_policy(vrml97_space_p);
    scanner_policies_t policies(iter_policy);
    iterator_t
        first(make_multi_pass(istreambuf_iterator<char_t>(in))),
        last(make_multi_pass(istreambuf_iterator<char_t>()));

    scanner_t scan(first, last, policies);

    rule_t r
        =   real_p[assign_a(v.vec[0])] >> real_p[assign_a(v.vec[1])]
        ;

    match<> m = r.parse(scan);

    if (!m) {
        in.setstate(std::ios_base::failbit);
    }

    return in;
}

/**
 * @relatesalso openvrml::vec2d
 *
 * @brief Stream output.
 *
 * @param[in,out] out   output stream.
 * @param[in] v     a 2-component vector.
 *
 * @return @p out.
 */
std::ostream & openvrml::operator<<(std::ostream & out, const vec2d & v)
{
    return out << v.x() << ' ' << v.y();
}


/**
 * @class openvrml::vec3f
 *
 * @brief Three-component single-precision vector.
 */

/**
 * @internal
 *
 * @var float openvrml::vec3f::vec[3]
 *
 * @brief Vector components.
 */

/**
 * @brief Construct.
 */
openvrml::vec3f::vec3f() OPENVRML_NOTHROW
{
    this->vec[0] = 0.0;
    this->vec[1] = 0.0;
    this->vec[2] = 0.0;
}

/**
 * @brief Construct from an array.
 *
 * @pre Elements of @p vec are valid numeric values (i.e., not NaN).
 *
 * @param[in] vec   an array comprising the vector components.
 */
openvrml::vec3f::vec3f(const float (&vec)[3]) OPENVRML_NOTHROW
{
    //
    // Ensure the elements of vec are not NaN.
    //
    assert(vec[0] == vec[0]);
    assert(vec[1] == vec[1]);
    assert(vec[2] == vec[2]);

    this->vec[0] = vec[0];
    this->vec[1] = vec[1];
    this->vec[2] = vec[2];
}

/**
 * @brief Construct from @p x, @p y, and @p z components.
 *
 * @pre @p x, @p y, and @p z are valid numeric values (i.e., not NaN).
 *
 * @param[in] x x component.
 * @param[in] y y component.
 * @param[in] z z component.
 */
openvrml::vec3f::vec3f(const float x, const float y, const float z)
    OPENVRML_NOTHROW
{
    //
    // Ensure x, y, and z are not NaN.
    //
    assert(x == x);
    assert(y == y);
    assert(z == z);

    this->vec[0] = x;
    this->vec[1] = y;
    this->vec[2] = z;
}

/**
 * @brief Cross multiply.
 *
 * @param[in] vec   vector by which to multiply.
 *
 * @return a reference to the object.
 */
openvrml::vec3f & openvrml::vec3f::operator*=(const vec3f & vec)
    OPENVRML_NOTHROW
{
    vec3f temp;
    temp.x(this->y() * vec.z() - this->z() * vec.y());
    temp.y(this->z() * vec.x() - this->x() * vec.z());
    temp.z(this->x() * vec.y() - this->y() * vec.x());
    *this = temp;
    return *this;
}

/**
 * @relatesalso openvrml::vec3f
 *
 * @brief Cross multiply two vectors.
 *
 * @param[in] lhs   left-hand operand.
 * @param[in] rhs   right-hand operand.
 *
 * @return the result vector.
 */
const openvrml::vec3f openvrml::operator*(const vec3f & lhs, const vec3f & rhs)
    OPENVRML_NOTHROW
{
    vec3f result(lhs);
    return result *= rhs;
}

/**
 * @brief Multiply by a matrix.
 *
 * @param[in] mat   matrix by which to multiply.
 *
 * @return a reference to the object.
 */
openvrml::vec3f & openvrml::vec3f::operator*=(const mat4f & mat)
    OPENVRML_NOTHROW
{
    const float x = this->vec[0] * mat[0][0] + this->vec[1] * mat[1][0]
                    + this->vec[2] * mat[2][0] + mat[3][0];
    const float y = this->vec[0] * mat[0][1] + this->vec[1] * mat[1][1]
                    + this->vec[2] * mat[2][1] + mat[3][1];
    const float z = this->vec[0] * mat[0][2] + this->vec[1] * mat[1][2]
                    + this->vec[2] * mat[2][2] + mat[3][2];
    const float w = this->vec[0] * mat[0][3] + this->vec[1] * mat[1][3]
                    + this->vec[2] * mat[2][3] + mat[3][3];
    this->vec[0] = x / w;
    this->vec[1] = y / w;
    this->vec[2] = z / w;
    return *this;
}

/**
 * @relatesalso openvrml::vec3f
 *
 * @brief Multiply a vector by a matrix.
 *
 * @param[in] vec   a vector.
 * @param[in] mat   a matrix.
 *
 * @return the result vector.
 */
const openvrml::vec3f openvrml::operator*(const vec3f & vec, const mat4f & mat)
    OPENVRML_NOTHROW
{
    vec3f result(vec);
    return result *= mat;
}

/**
 * @relatesalso openvrml::vec3f
 *
 * @brief Multiply a matrix by a vector.
 *
 * @param[in] mat   a matrix.
 * @param[in] vec   a 3-component vector.
 *
 * @return the result vector.
 */
const openvrml::vec3f openvrml::operator*(const mat4f & mat, const vec3f & vec)
    OPENVRML_NOTHROW
{
    const float x = mat[0][0] * vec[0] + mat[0][1] * vec[1]
                    + mat[0][2] * vec[2] + mat[0][3];
    const float y = mat[1][0] * vec[0] + mat[1][1] * vec[1]
                    + mat[1][2] * vec[2] + mat[1][3];
    const float z = mat[2][0] * vec[0] + mat[2][1] * vec[1]
                    + mat[2][2] * vec[2] + mat[2][3];
    const float w = mat[3][0] * vec[0] + mat[3][1] * vec[1]
                    + mat[3][2] * vec[2] + mat[3][3];
    return vec3f(x / w, y / w, z / w);
}

/**
 * @brief Multiply by a scalar.
 *
 * @param[in] scalar    factor by which to multiply.
 *
 * @return a reference to the object.
 */
openvrml::vec3f & openvrml::vec3f::operator*=(const float scalar)
    OPENVRML_NOTHROW
{
    this->vec[0] *= scalar;
    this->vec[1] *= scalar;
    this->vec[2] *= scalar;
    return *this;
}

/**
 * @relatesalso openvrml::vec3f
 *
 * @brief Multiply a vector by a scalar.
 *
 * @param[in] lhs   left-hand operand (the vector).
 * @param[in] rhs   right-hand operand (the scalar).
 *
 * @return the result vector.
 */
const openvrml::vec3f openvrml::operator*(const vec3f & lhs, const float rhs)
    OPENVRML_NOTHROW
{
    vec3f result(lhs);
    return result *= rhs;
}

/**
 * @relatesalso openvrml::vec3f
 *
 * @brief Multiply a vector by a scalar.
 *
 * @param[in] lhs   left-hand operand (the scalar).
 * @param[in] rhs   right-hand operand (the vector).
 *
 * @return the result vector.
 */
const openvrml::vec3f openvrml::operator*(const float lhs, const vec3f & rhs)
    OPENVRML_NOTHROW
{
    vec3f result(rhs);
    return result *= lhs;
}

/**
 * @brief Divide by a scalar.
 *
 * @pre @p scalar is nonzero.
 *
 * @param[in] scalar    divisor.
 *
 * @return a reference to the object.
 */
openvrml::vec3f & openvrml::vec3f::operator/=(const float scalar)
    OPENVRML_NOTHROW
{
    assert(scalar != 0.0);
    this->vec[0] /= scalar;
    this->vec[1] /= scalar;
    this->vec[2] /= scalar;
    return *this;
}

/**
 * @relatesalso openvrml::vec3f
 *
 * @brief Divide a vector by a scalar.
 *
 * @pre @p rhs is nonzero.
 *
 * @param[in] lhs   left-hand operand (the vector).
 * @param[in] rhs   right-hand operand (the scalar).
 *
 * @return the result vector.
 */
const openvrml::vec3f openvrml::operator/(const vec3f & lhs, const float rhs)
    OPENVRML_NOTHROW
{
    vec3f result(lhs);
    return result /= rhs;
}

/**
 * @brief Add a vector.
 *
 * @param[in] vec   the vector to add.
 *
 * @return a reference to the object.
 */
openvrml::vec3f & openvrml::vec3f::operator+=(const vec3f & vec)
    OPENVRML_NOTHROW
{
    this->vec[0] += vec[0];
    this->vec[1] += vec[1];
    this->vec[2] += vec[2];
    return *this;
}

/**
 * @relatesalso openvrml::vec3f
 *
 * @brief Add two vectors.
 *
 * @param[in] lhs   left-hand operand.
 * @param[in] rhs   right-hand operand.
 *
 * @return the result vector.
 */
const openvrml::vec3f openvrml::operator+(const vec3f & lhs, const vec3f & rhs)
    OPENVRML_NOTHROW
{
    vec3f result(lhs);
    return result += rhs;
}

/**
 * @brief Subtract a vector.
 *
 * @param[in] vec   the vector to subtract.
 *
 * @return a reference to the object.
 */
openvrml::vec3f & openvrml::vec3f::operator-=(const vec3f & vec)
    OPENVRML_NOTHROW
{
    this->vec[0] -= vec[0];
    this->vec[1] -= vec[1];
    this->vec[2] -= vec[2];
    return *this;
}

/**
 * @relatesalso openvrml::vec3f
 *
 * @brief Subtract two vectors.
 *
 * @param[in] lhs   left-hand operand.
 * @param[in] rhs   right-hand operand.
 *
 * @return the result vector.
 */
const openvrml::vec3f openvrml::operator-(const vec3f & lhs, const vec3f & rhs)
    OPENVRML_NOTHROW
{
    vec3f result(lhs);
    return result -= rhs;
}

/**
 * @brief Negate.
 *
 * @return the additive inverse of the vector.
 */
const openvrml::vec3f openvrml::vec3f::operator-() const OPENVRML_NOTHROW
{
    return vec3f(-this->vec[0], -this->vec[1], -this->vec[2]);
}

/**
 * @fn const float & openvrml::vec3f::operator[](size_t index) const
 *
 * @brief Index-based component access.
 *
 * @param[in] index 0 corresponds to the x component; 1 corresponds to the y
 *              component; 2 corresponds to the z component.
 *
 * @return a const reference to the component corresponding to @p index.
 *
 * @pre @p index is less than 3.
 */

/**
 * @fn float openvrml::vec3f::x() const
 *
 * @brief Get the x component.
 *
 * @return the x component.
 */

/**
 * @fn float openvrml::vec3f::y() const
 *
 * @brief Get the y component.
 *
 * @return the y component.
 */

/**
 * @fn float openvrml::vec3f::z() const
 *
 * @brief Get the z component.
 *
 * @return the z component.
 */

/**
 * @fn void openvrml::vec3f::x(float value)
 *
 * @brief Set the x component.
 *
 * @pre @p value is a valid numeric value (i.e., not NaN).
 *
 * @param[in] value new x component value.
 */

/**
 * @fn void openvrml::vec3f::y(float value)
 *
 * @brief Set the y component.
 *
 * @pre @p value is a valid numeric value (i.e., not NaN).
 *
 * @param[in] value new y component value.
 */

/**
 * @fn void openvrml::vec3f::z(float value)
 *
 * @brief Set the z component.
 *
 * @pre @p value is a valid numeric value (i.e., not NaN).
 *
 * @param[in] value new z component value.
 */

/**
 * @brief Dot product.
 *
 * @param[in] vec
 *
 * @return the dot product of the vector and @p vec.
 */
float openvrml::vec3f::dot(const vec3f & vec) const OPENVRML_NOTHROW
{
    return this->vec[0] * vec[0]
        + this->vec[1] * vec[1]
        + this->vec[2] * vec[2];
}

/**
 * @brief Geometric length.
 *
 * @return the length of the vector.
 */
float openvrml::vec3f::length() const OPENVRML_NOTHROW
{
    return float(sqrt(this->dot(*this)));
}

/**
 * @brief Normalize.
 *
 * @return the vector normalized.
 */
const openvrml::vec3f openvrml::vec3f::normalize() const OPENVRML_NOTHROW
{
    using openvrml_::fequal;
    const float len = this->length();
    vec3f result(*this);
    if (!fequal(len, 0.0f)) { result /= len; }
    return result;
}

/**
 * @relatesalso openvrml::vec3f
 *
 * @brief Compare for equality.
 *
 * @param[in] lhs   left-hand operand.
 * @param[in] rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs have the same value; @c false
 *         otherwise.
 */
bool openvrml::operator==(const vec3f & lhs, const vec3f & rhs)
    OPENVRML_NOTHROW
{
    using openvrml_::fequal;
    return std::equal(&lhs[0], &lhs[0] + 3, &rhs[0], fequal);
}

/**
 * @relatesalso openvrml::vec3f
 *
 * @brief Compare for inequality.
 *
 * @param[in] lhs   left-hand operand.
 * @param[in] rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs dot not have the same value; @c false
 *      otherwise.
 */
bool openvrml::operator!=(const vec3f & lhs, const vec3f & rhs)
    OPENVRML_NOTHROW
{
    return !(lhs == rhs);
}

/**
 * @brief Stream input.
 *
 * Consistent with the VRML97 convention, commas (&ldquo;@c ,&rdquo;) in the
 * input are treated as whitespace.
 *
 * @param[in,out] in    input stream.
 * @param[out] v        a @c vec3f.
 *
 * @return @p in.
 */
std::istream & openvrml::operator>>(std::istream & in, vec3f & v)
{
    using std::istreambuf_iterator;
    using boost::spirit::make_multi_pass;
    using boost::spirit::match;
    using boost::spirit::real_p;
    using boost::spirit::assign_a;

    iter_policy_t iter_policy(vrml97_space_p);
    scanner_policies_t policies(iter_policy);
    iterator_t
        first(make_multi_pass(istreambuf_iterator<char_t>(in))),
        last(make_multi_pass(istreambuf_iterator<char_t>()));

    scanner_t scan(first, last, policies);

    rule_t r
        =   real_p[assign_a(v.vec[0])]
            >> real_p[assign_a(v.vec[1])]
            >> real_p[assign_a(v.vec[2])]
        ;

    match<> m = r.parse(scan);

    if (!m) {
        in.setstate(std::ios_base::failbit);
    }

    return in;
}

/**
 * @relatesalso openvrml::vec3f
 *
 * @brief Stream output.
 *
 * @param[in,out] out   output stream.
 * @param[in] v     a 3-component vector.
 *
 * @return @p out.
 */
std::ostream & openvrml::operator<<(std::ostream & out, const vec3f & v)
{
    return out << v.x() << ' ' << v.y() << ' ' << v.z();
}


/**
 * @class openvrml::vec3d
 *
 * @brief Three-component double-precision vector.
 */

/**
 * @internal
 *
 * @var double openvrml::vec3d::vec[3]
 *
 * @brief Vector components.
 */

/**
 * @brief Construct.
 */
openvrml::vec3d::vec3d() OPENVRML_NOTHROW
{
    this->vec[0] = 0.0;
    this->vec[1] = 0.0;
    this->vec[2] = 0.0;
}

/**
 * @brief Construct from an array.
 *
 * @pre Elements of @p vec are valid numeric values (i.e., not NaN).
 *
 * @param[in] vec   an array comprising the vector components.
 */
openvrml::vec3d::vec3d(const double (&vec)[3]) OPENVRML_NOTHROW
{
    //
    // Ensure the elements of vec are not NaN.
    //
    assert(vec[0] == vec[0]);
    assert(vec[1] == vec[1]);
    assert(vec[2] == vec[2]);

    this->vec[0] = vec[0];
    this->vec[1] = vec[1];
    this->vec[2] = vec[2];
}

/**
 * @brief Construct from @p x, @p y, and @p z components.
 *
 * @pre @p x, @p y, and @p z are valid numeric values (i.e., not NaN).
 *
 * @param[in] x x component.
 * @param[in] y y component.
 * @param[in] z z component.
 */
openvrml::vec3d::vec3d(const double x, const double y, const double z)
    OPENVRML_NOTHROW
{
    //
    // Ensure x, y, and z are not NaN.
    //
    assert(x == x);
    assert(y == y);
    assert(z == z);

    this->vec[0] = x;
    this->vec[1] = y;
    this->vec[2] = z;
}

/**
 * @brief Cross multiply.
 *
 * @param[in] vec   vector by which to multiply.
 *
 * @return a reference to the object.
 */
openvrml::vec3d & openvrml::vec3d::operator*=(const vec3d & vec)
    OPENVRML_NOTHROW
{
    vec3d temp;
    temp.x(this->y() * vec.z() - this->z() * vec.y());
    temp.y(this->z() * vec.x() - this->x() * vec.z());
    temp.z(this->x() * vec.y() - this->y() * vec.x());
    *this = temp;
    return *this;
}

/**
 * @relatesalso openvrml::vec3d
 *
 * @brief Cross multiply two vectors.
 *
 * @param[in] lhs   left-hand operand.
 * @param[in] rhs   right-hand operand.
 *
 * @return the result vector.
 */
const openvrml::vec3d openvrml::operator*(const vec3d & lhs, const vec3d & rhs)
    OPENVRML_NOTHROW
{
    vec3d result(lhs);
    return result *= rhs;
}

/**
 * @brief Multiply by a matrix.
 *
 * @param[in] mat   matrix by which to multiply.
 *
 * @return a reference to the object.
 */
openvrml::vec3d & openvrml::vec3d::operator*=(const mat4f & mat)
    OPENVRML_NOTHROW
{
    const double x = this->vec[0] * mat[0][0] + this->vec[1] * mat[1][0]
                    + this->vec[2] * mat[2][0] + mat[3][0];
    const double y = this->vec[0] * mat[0][1] + this->vec[1] * mat[1][1]
                    + this->vec[2] * mat[2][1] + mat[3][1];
    const double z = this->vec[0] * mat[0][2] + this->vec[1] * mat[1][2]
                    + this->vec[2] * mat[2][2] + mat[3][2];
    const double w = this->vec[0] * mat[0][3] + this->vec[1] * mat[1][3]
                    + this->vec[2] * mat[2][3] + mat[3][3];
    this->vec[0] = x / w;
    this->vec[1] = y / w;
    this->vec[2] = z / w;
    return *this;
}

/**
 * @relatesalso openvrml::vec3d
 *
 * @brief Multiply a vector by a matrix.
 *
 * @param[in] vec   a vector.
 * @param[in] mat   a matrix.
 *
 * @return the result vector.
 */
const openvrml::vec3d openvrml::operator*(const vec3d & vec, const mat4f & mat)
    OPENVRML_NOTHROW
{
    vec3d result(vec);
    return result *= mat;
}

/**
 * @relatesalso openvrml::vec3d
 *
 * @brief Multiply a matrix by a vector.
 *
 * @param[in] mat   a matrix.
 * @param[in] vec   a 3-component vector.
 *
 * @return the result vector.
 */
const openvrml::vec3d openvrml::operator*(const mat4f & mat, const vec3d & vec)
    OPENVRML_NOTHROW
{
    const double x = mat[0][0] * vec[0] + mat[0][1] * vec[1]
                    + mat[0][2] * vec[2] + mat[0][3];
    const double y = mat[1][0] * vec[0] + mat[1][1] * vec[1]
                    + mat[1][2] * vec[2] + mat[1][3];
    const double z = mat[2][0] * vec[0] + mat[2][1] * vec[1]
                    + mat[2][2] * vec[2] + mat[2][3];
    const double w = mat[3][0] * vec[0] + mat[3][1] * vec[1]
                    + mat[3][2] * vec[2] + mat[3][3];
    return vec3d(x / w, y / w, z / w);
}

/**
 * @brief Multiply by a scalar.
 *
 * @param[in] scalar    factor by which to multiply.
 *
 * @return a reference to the object.
 */
openvrml::vec3d & openvrml::vec3d::operator*=(const double scalar)
    OPENVRML_NOTHROW
{
    this->vec[0] *= scalar;
    this->vec[1] *= scalar;
    this->vec[2] *= scalar;
    return *this;
}

/**
 * @relatesalso openvrml::vec3d
 *
 * @brief Multiply a vector by a scalar.
 *
 * @param[in] lhs   left-hand operand (the vector).
 * @param[in] rhs   right-hand operand (the scalar).
 *
 * @return the result vector.
 */
const openvrml::vec3d openvrml::operator*(const vec3d & lhs, const double rhs)
    OPENVRML_NOTHROW
{
    vec3d result(lhs);
    return result *= rhs;
}

/**
 * @relatesalso openvrml::vec3d
 *
 * @brief Multiply a vector by a scalar.
 *
 * @param[in] lhs   left-hand operand (the scalar).
 * @param[in] rhs   right-hand operand (the vector).
 *
 * @return the result vector.
 */
const openvrml::vec3d openvrml::operator*(const double lhs, const vec3d & rhs)
    OPENVRML_NOTHROW
{
    vec3d result(rhs);
    return result *= lhs;
}

/**
 * @brief Divide by a scalar.
 *
 * @pre @p scalar is nonzero.
 *
 * @param[in] scalar    divisor.
 *
 * @return a reference to the object.
 */
openvrml::vec3d & openvrml::vec3d::operator/=(const double scalar)
    OPENVRML_NOTHROW
{
    assert(scalar != 0.0);
    this->vec[0] /= scalar;
    this->vec[1] /= scalar;
    this->vec[2] /= scalar;
    return *this;
}

/**
 * @relatesalso openvrml::vec3d
 *
 * @brief Divide a vector by a scalar.
 *
 * @pre @p rhs is nonzero.
 *
 * @param[in] lhs   left-hand operand (the vector).
 * @param[in] rhs   right-hand operand (the scalar).
 *
 * @return the result vector.
 */
const openvrml::vec3d openvrml::operator/(const vec3d & lhs, const double rhs)
    OPENVRML_NOTHROW
{
    vec3d result(lhs);
    return result /= rhs;
}

/**
 * @brief Add a vector.
 *
 * @param[in] vec   the vector to add.
 *
 * @return a reference to the object.
 */
openvrml::vec3d & openvrml::vec3d::operator+=(const vec3d & vec)
    OPENVRML_NOTHROW
{
    this->vec[0] += vec[0];
    this->vec[1] += vec[1];
    this->vec[2] += vec[2];
    return *this;
}

/**
 * @relatesalso openvrml::vec3d
 *
 * @brief Add two vectors.
 *
 * @param[in] lhs   left-hand operand.
 * @param[in] rhs   right-hand operand.
 *
 * @return the result vector.
 */
const openvrml::vec3d openvrml::operator+(const vec3d & lhs, const vec3d & rhs)
    OPENVRML_NOTHROW
{
    vec3d result(lhs);
    return result += rhs;
}

/**
 * @brief Subtract a vector.
 *
 * @param[in] vec   the vector to subtract.
 *
 * @return a reference to the object.
 */
openvrml::vec3d & openvrml::vec3d::operator-=(const vec3d & vec)
    OPENVRML_NOTHROW
{
    this->vec[0] -= vec[0];
    this->vec[1] -= vec[1];
    this->vec[2] -= vec[2];
    return *this;
}

/**
 * @relatesalso openvrml::vec3d
 *
 * @brief Subtract two vectors.
 *
 * @param[in] lhs   left-hand operand.
 * @param[in] rhs   right-hand operand.
 *
 * @return the result vector.
 */
const openvrml::vec3d openvrml::operator-(const vec3d & lhs, const vec3d & rhs)
    OPENVRML_NOTHROW
{
    vec3d result(lhs);
    return result -= rhs;
}

/**
 * @brief Negate.
 *
 * @return the additive inverse of the vector.
 */
const openvrml::vec3d openvrml::vec3d::operator-() const OPENVRML_NOTHROW
{
    return vec3d(-this->vec[0], -this->vec[1], -this->vec[2]);
}

/**
 * @fn const double & openvrml::vec3d::operator[](size_t index) const
 *
 * @brief Index-based component access.
 *
 * @param[in] index 0 corresponds to the x component; 1 corresponds to the y
 *              component; 2 corresponds to the z component.
 *
 * @return a const reference to the component corresponding to @p index.
 *
 * @pre @p index is less than 3.
 */

/**
 * @fn double openvrml::vec3d::x() const
 *
 * @brief Get the x component.
 *
 * @return the x component.
 */

/**
 * @fn double openvrml::vec3d::y() const
 *
 * @brief Get the y component.
 *
 * @return the y component.
 */

/**
 * @fn double openvrml::vec3d::z() const
 *
 * @brief Get the z component.
 *
 * @return the z component.
 */

/**
 * @fn void openvrml::vec3d::x(double value)
 *
 * @brief Set the x component.
 *
 * @pre @p value is a valid numeric value (i.e., not NaN).
 *
 * @param[in] value new x component value.
 */

/**
 * @fn void openvrml::vec3d::y(double value)
 *
 * @brief Set the y component.
 *
 * @pre @p value is a valid numeric value (i.e., not NaN).
 *
 * @param[in] value new y component value.
 */

/**
 * @fn void openvrml::vec3d::z(double value)
 *
 * @brief Set the z component.
 *
 * @pre @p value is a valid numeric value (i.e., not NaN).
 *
 * @param[in] value new z component value.
 */

/**
 * @brief Dot product.
 *
 * @param[in] vec
 *
 * @return the dot product of the vector and @p vec.
 */
double openvrml::vec3d::dot(const vec3d & vec) const OPENVRML_NOTHROW
{
    return this->vec[0] * vec[0]
        + this->vec[1] * vec[1]
        + this->vec[2] * vec[2];
}

/**
 * @brief Geometric length.
 *
 * @return the length of the vector.
 */
double openvrml::vec3d::length() const OPENVRML_NOTHROW
{
    return double(sqrt(this->dot(*this)));
}

/**
 * @brief Normalize.
 *
 * @return the vector normalized.
 */
const openvrml::vec3d openvrml::vec3d::normalize() const OPENVRML_NOTHROW
{
    using openvrml_::fequal;
    const double len = this->length();
    vec3d result(*this);
    if (!fequal(len, 0.0)) { result /= len; }
    return result;
}

/**
 * @relatesalso openvrml::vec3d
 *
 * @brief Compare for equality.
 *
 * @param[in] lhs   left-hand operand.
 * @param[in] rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs have the same value; @c false
 *         otherwise.
 */
bool openvrml::operator==(const vec3d & lhs, const vec3d & rhs)
    OPENVRML_NOTHROW
{
    using openvrml_::fequal;
    return std::equal(&lhs[0], &lhs[0] + 3, &rhs[0], fequal);
}

/**
 * @relatesalso openvrml::vec3d
 *
 * @brief Compare for inequality.
 *
 * @param[in] lhs   left-hand operand.
 * @param[in] rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs dot not have the same value; @c false
 *      otherwise.
 */
bool openvrml::operator!=(const vec3d & lhs, const vec3d & rhs)
    OPENVRML_NOTHROW
{
    return !(lhs == rhs);
}

/**
 * @brief Stream input.
 *
 * Consistent with the VRML97 convention, commas (&ldquo;@c ,&rdquo;) in the
 * input are treated as whitespace.
 *
 * @param[in,out] in    input stream.
 * @param[out] v        a @c vec3d.
 *
 * @return @p in.
 */
std::istream & openvrml::operator>>(std::istream & in, vec3d & v)
{
    using std::istreambuf_iterator;
    using boost::spirit::make_multi_pass;
    using boost::spirit::match;
    using boost::spirit::real_p;
    using boost::spirit::assign_a;

    iter_policy_t iter_policy(vrml97_space_p);
    scanner_policies_t policies(iter_policy);
    iterator_t
        first(make_multi_pass(istreambuf_iterator<char_t>(in))),
        last(make_multi_pass(istreambuf_iterator<char_t>()));

    scanner_t scan(first, last, policies);

    rule_t r
        =   real_p[assign_a(v.vec[0])]
            >> real_p[assign_a(v.vec[1])]
            >> real_p[assign_a(v.vec[2])]
        ;

    match<> m = r.parse(scan);

    if (!m) {
        in.setstate(std::ios_base::failbit);
    }

    return in;
}

/**
 * @relatesalso openvrml::vec3d
 *
 * @brief Stream output.
 *
 * @param[in,out] out   output stream.
 * @param[in] v     a 3-component vector.
 *
 * @return @p out.
 */
std::ostream & openvrml::operator<<(std::ostream & out, const vec3d & v)
{
    return out << v.x() << ' ' << v.y() << ' ' << v.z();
}


/**
 * @class openvrml::rotation
 *
 * @brief A rotation.
 *
 * VRML rotations are represented with four single-precision floating point
 * components. The first three are an axis of rotation, and the last is
 * rotation in radians.
 */

/**
 * @internal
 *
 * @var float openvrml::rotation::rot[4]
 *
 * @brief Rotation components.
 *
 * The first three components define an axis through the origin. The fourth
 * component is a rotation around the axis.
 */

/**
 * @brief Construct.
 */
openvrml::rotation::rotation() OPENVRML_NOTHROW
{
    this->rot[0] = 0.0;
    this->rot[1] = 0.0;
    this->rot[2] = 1.0;
    this->rot[3] = 0.0;
}

/**
 * @brief Construct from an array.
 *
 * @param[in] rot   an array comprising the rotation components.
 *
 * @pre The vector represented by the first three components of @p rot is
 *      normalized.
 */
openvrml::rotation::rotation(const float (&rot)[4]) OPENVRML_NOTHROW
{
    assert(openvrml_::fequal(1.0f, vec3f(rot[0], rot[1], rot[2]).length()));
    std::copy(rot, rot + 4, this->rot);
}

/**
 * @brief Construct from @p x, @p y, @p z, and @p angle components.
 *
 * @param[in] x     x-component of the rotation axis.
 * @param[in] y     y-component of the rotation axis.
 * @param[in] z     z-component of the rotation axis.
 * @param[in] angle rotation angle.
 *
 * @pre The vector represented by @p x, @p y, and @p z is normalized.
 */
openvrml::rotation::rotation(const float x,
                             const float y,
                             const float z,
                             const float angle)
    OPENVRML_NOTHROW
{
    assert(openvrml_::fequal(1.0f, vec3f(x, y, z).length()));
    this->rot[0] = x;
    this->rot[1] = y;
    this->rot[2] = z;
    this->rot[3] = angle;
}

/**
 * @brief Construct from an axis vector and an angle.
 *
 * @param[in] axis  rotation axis.
 * @param[in] angle rotation angle.
 *
 * @pre @p axis is a normalized vector.
 */
openvrml::rotation::rotation(const vec3f & axis, const float angle)
    OPENVRML_NOTHROW
{
    assert(openvrml_::fequal(1.0f, axis.length()));
    this->axis(axis);
    this->rot[3] = angle;
}

/**
 * @brief Construct a rotation between two vectors.
 *
 * Construct a rotation equal to the rotation between two different vectors.
 *
 * @param[in] from_vec  the starting vector.
 * @param[in] to_vec    the ending vector.
 */
openvrml::rotation::rotation(const vec3f & from_vec, const vec3f & to_vec)
    OPENVRML_NOTHROW
{
    this->axis(from_vec * to_vec);
    this->rot[3] = float(acos(from_vec.dot(to_vec)
                              / (from_vec.length() * to_vec.length())));
}

/**
 * @brief Construct from a quaternion.
 *
 * @param[in] quat  a quaternion.
 */
openvrml::rotation::rotation(const quatf & quat) OPENVRML_NOTHROW
{
    using openvrml_::fequal;

    const float val = float(acos(quat.w()));
    if (fequal(val, 0.0f)) {
        this->rot[0] = 0.0;
        this->rot[1] = 0.0;
        this->rot[2] = 1.0;
        this->rot[3] = 0.0;
    } else {
        const float sin_val = float(sin(val));
        const vec3f axis(quat.x() / sin_val,
                         quat.y() / sin_val,
                         quat.z() / sin_val);
        this->axis(axis.normalize());
        this->rot[3] = 2 * val;
    }
}

/**
 * @brief Multiply rotations.
 *
 * @param[in] rot   the rotation by which to multiply.
 *
 * @return a reference to the object.
 */
openvrml::rotation & openvrml::rotation::operator*=(const rotation & rot)
    OPENVRML_NOTHROW
{
    return *this = rotation(quatf(*this) * quatf(rot));
}

/**
 * @relatesalso openvrml::rotation
 *
 * @brief Multiply rotations.
 *
 * @param[in] lhs   left-hand operand.
 * @param[in] rhs   right-hand operand.
 *
 * @return the product of @p lhs and @p rhs.
 */
const openvrml::rotation openvrml::operator*(const rotation & lhs,
                                             const rotation & rhs)
    OPENVRML_NOTHROW
{
    rotation result(lhs);
    return result *= rhs;
}


/**
 * @fn const float & openvrml::rotation::operator[](const size_t index) const
 *
 * @brief Index-based element access.
 *
 * @param[in] index 0 is the x axis component, 1 is the y axis component, 2 is the
 *              z axis component, and 3 is the angle.
 */

/**
 * @fn float openvrml::rotation::x() const
 *
 * @brief Get the x axis component.
 *
 * @return the x axis component.
 */

/**
 * @fn float openvrml::rotation::y() const
 *
 * @brief Get the y axis component.
 *
 * @return the y axis component.
 */

/**
 * @fn float openvrml::rotation::z() const
 *
 * @brief Get the z axis component.
 *
 * @return the z axis component.
 */

/**
 * @fn const openvrml::vec3f openvrml::rotation::axis() const
 *
 * @brief Get the axis of rotation.
 *
 * @return the axis of rotation.
 */

/**
 * @fn float openvrml::rotation::angle() const
 *
 * @brief Get the rotation angle.
 *
 * @return the rotation angle.
 */

namespace {

    inline double length_(const float vec[3])
    {
        return sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);
    }

    inline void normalize_(float vec[3])
    {
        using openvrml_::fequal;
        const float len = float(length_(vec));
        if (!fequal(len, 0.0f)) {
            vec[0] /= len;
            vec[1] /= len;
            vec[2] /= len;
        }
    }

    void normalize_axis_(float axis[3]) OPENVRML_NOTHROW
    {
        using openvrml_::fequal;
        const float axis_length = float(length_(axis));
        if (fequal(axis_length, 0.0f)) {
            axis[2] = 1.0;
        } else {
            normalize_(axis);
        }
    }
}

/**
 * @brief Set the x axis component.
 *
 * @param[in] value new x axis component value.
 */
void openvrml::rotation::x(const float value) OPENVRML_NOTHROW
{
    this->rot[0] = value;
    normalize_axis_(this->rot);
}

/**
 * @brief Set the y axis component.
 *
 * @param[in] value new y axis component value.
 */
void openvrml::rotation::y(const float value) OPENVRML_NOTHROW
{
    this->rot[1] = value;
    normalize_axis_(this->rot);
}

/**
 * @brief Set the z axis component.
 *
 * @param[in] value new z axis component value.
 */
void openvrml::rotation::z(const float value) OPENVRML_NOTHROW
{
    this->rot[2] = value;
    normalize_axis_(this->rot);
}

/**
 * @brief Set the axis of rotation.
 *
 * @param[in] axis  new axis of rotation.
 */
void openvrml::rotation::axis(const vec3f & axis) OPENVRML_NOTHROW
{
    assert(openvrml_::fequal(1.0f, axis.length()));
    this->rot[0] = axis.x();
    this->rot[1] = axis.y();
    this->rot[2] = axis.z();
}

/**
 * @fn void openvrml::rotation::angle(float value)
 *
 * @brief Set the rotation angle.
 *
 * @param[in] value new rotation angle value.
 */

/**
 * @brief Inverse rotation.
 *
 * @return the inverse rotation.
 */
const openvrml::rotation openvrml::rotation::inverse() const OPENVRML_NOTHROW
{
    rotation result(*this);
    result.rot[3] = -this->rot[3];
    return result;
}

/**
 * @brief Spherical Linear intERPolation.
 *
 * @param[in] dest_rot  destination rotation.
 * @param[in] t         fraction representing the desired position between the
 *                  rotation and @p dest_rot.
 *
 * @return the rotation corresponding to @p t between the rotation and
 *         @p dest_rot.
 */
const openvrml::rotation openvrml::rotation::slerp(const rotation & dest_rot,
                                                   const float t) const
    OPENVRML_NOTHROW
{
    using openvrml_::fequal;

    quatf from_quat(*this), to_quat(dest_rot);

    //
    // Calculate cosine.
    //
    double cosom = std::inner_product(&from_quat[0], &from_quat[0] + 4,
                                      &to_quat[0], 0.0);

    //
    // Adjust signs (if necessary).
    //
    quatf to1;
    if (cosom < 0.0) {
        cosom = -cosom;
        to1 = to_quat * -1;
    } else {
        to1 = to_quat;
    }

    //
    // Calculate coefficients.
    //
    double scale0, scale1;
    if (!fequal(cosom, 0.0)) {
        const double omega = acos(cosom);
        const double sinom = sin(omega);
        scale0 = sin((1.0 - t) * omega) / sinom;
        scale1 = sin(t * omega) / sinom;
    } else {
        //
        // "From" and "to" quaternions are very close, so do linear
        // interpolation.
        //
        scale0 = 1.0 - t;
        scale1 = t;
    }

    //
    // Calculate the final values.
    //
    const quatf result_quat =
        (from_quat * float(scale0)) + (to1 * float(scale1));
    return rotation(result_quat);
}

/**
 * @relatesalso openvrml::rotation
 *
 * @brief Compare for equality.
 *
 * @param[in] lhs   left-hand operand.
 * @param[in] rhs   right-hand operand.
 *
 * @return @c true if @p lhs is equal to @p rhs; @c false otherwise.
 */
bool openvrml::operator==(const rotation & lhs, const rotation & rhs)
    OPENVRML_NOTHROW
{
    using openvrml_::fequal;
    return std::equal(&lhs[0], &lhs[0] + 4, &rhs[0], fequal);
}

/**
 * @relatesalso openvrml::rotation
 *
 * @brief Compare for inequality.
 *
 * @param[in] lhs   left-hand operand.
 * @param[in] rhs   right-hand operand.
 *
 * @return @c true if @p lhs is not equal to @p rhs; @c false otherwise.
 */
bool openvrml::operator!=(const rotation & lhs, const rotation & rhs)
    OPENVRML_NOTHROW
{
    return !(lhs == rhs);
}

namespace {
    struct OPENVRML_LOCAL is_normalized {
        is_normalized(float & x, float & y, float & z):
            x_(x),
            y_(y),
            z_(z)
        {}

        bool operator()() const
        {
            using openvrml::vec3f;
            using openvrml_::fequal;
            return fequal(vec3f(this->x_, this->y_, this->z_).length(), 1.0f);
        }

    private:
        float & x_, & y_, & z_;
    };
}

/**
 * @relatesalso openvrml::rotation
 *
 * @brief Stream input.
 *
 * Consistent with the VRML97 convention, commas (&ldquo;@c ,&rdquo;) in the
 * input are treated as whitespace.
 *
 * If the axis components of the rotation do not represent a normalized
 * vector, the @c failbit will be set on @p in and @p rot will not be
 * modified.
 *
 * @param[in,out] in    input stream.
 * @param[out] rot      a @c rotation.
 *
 * @return @p in.
 */
std::istream & openvrml::operator>>(std::istream & in, rotation & rot)
{
    using std::istreambuf_iterator;
    using boost::spirit::make_multi_pass;
    using boost::spirit::match;
    using boost::spirit::eps_p;
    using boost::spirit::real_p;
    using boost::spirit::assign_a;

    iter_policy_t iter_policy(vrml97_space_p);
    scanner_policies_t policies(iter_policy);
    iterator_t
        first(make_multi_pass(istreambuf_iterator<char_t>(in))),
        last(make_multi_pass(istreambuf_iterator<char_t>()));

    scanner_t scan(first, last, policies);

    float x, y, z, angle;
    rule_t rule
        =   real_p[assign_a(x)] >> real_p[assign_a(y)] >> real_p[assign_a(z)]
            >> eps_p(is_normalized(x, y, z))
            >> real_p[assign_a(angle)]
        ;

    match<> m = rule.parse(scan);

    if (!m) {
        in.setstate(std::ios_base::failbit);
        return in;
    }

    rot.x(x);
    rot.y(y);
    rot.z(z);
    rot.angle(angle);

    return in;
}

/**
 * @relatesalso openvrml::rotation
 *
 * @brief Stream output.
 *
 * @param[in,out] out   output stream.
 * @param[in] r     a rotation.
 *
 * @return @p out.
 */
std::ostream & openvrml::operator<<(std::ostream & out, const rotation & r)
{
    return out << r.x() << ' ' << r.y() << ' ' << r.z() << ' ' << r.angle();
}


/**
 * @class openvrml::mat4f
 *
 * @brief A class for all matrix operations.
 *
 * Matrices are stored in row-major order.
 */

/**
 * @internal
 *
 * @var float openvrml::mat4f::mat[4][4]
 *
 * @brief 4x4 float matrix.
 */

/**
 * @brief Create a rotation matrix.
 *
 * @param[in] rot   a rotation.
 *
 * @return a matrix representation of @p rot.
 */
const openvrml::mat4f openvrml::mat4f::rotation(const openvrml::rotation & rot)
    OPENVRML_NOTHROW
{
    const double s = sin(rot.angle());
    const double c = cos(rot.angle());
    const double t = 1.0 - c;
    const float x = rot.x();
    const float y = rot.y();
    const float z = rot.z();

    return mat4f(float(t * x * x + c),
                 float(t * x * y + s * z),
                 float(t * x * z - s * y),
                 0.0f,
                 float(t * x * y - s * z),
                 float(t * y * y + c),
                 float(t * y * z + s * x),
                 0.0f,
                 float(t * x * z + s * y),
                 float(t * y * z - s * x),
                 float(t * z * z + c),
                 0.0f,
                 0.0f,
                 0.0f,
                 0.0f,
                 1.0f);
}

/**
 * @brief Create a rotation matrix.
 *
 * @param[in] quat  a quaternion.
 *
 * @return a matrix representation of @p quat.
 */
const openvrml::mat4f openvrml::mat4f::rotation(const quatf & quat)
    OPENVRML_NOTHROW
{
    const float x = quat.x();
    const float y = quat.y();
    const float z = quat.z();
    const float w = quat.w();

    return mat4f(float(1.0 - 2.0 * (y * y + z * z)),
                 float(2.0 * (x * y + z * w)),
                 float(2.0 * (z * x - y * w)),
                 0.0f,
                 float(2.0 * (x * y - z * w)),
                 float(1.0 - 2.0 * (z * z + x * x)),
                 float(2.0 * (y * z + x * w)),
                 0.0f,
                 float(2.0 * (z * x + y * w)),
                 float(2.0 * (y * z - x * w)),
                 float(1.0 - 2.0 * (y * y + x * x)),
                 0.0f,
                 0.0f,
                 0.0f,
                 0.0f,
                 1.0f);
}

/**
 * @brief Create a uniform scale matrix.
 *
 * @param[in] s scale factor.
 *
 * @return a uniform scale matrix.
 */
const openvrml::mat4f openvrml::mat4f::scale(const float s) OPENVRML_NOTHROW
{
    return mat4f(s,   0.0, 0.0, 0.0,
                 0.0, s,   0.0, 0.0,
                 0.0, 0.0, s,   0.0,
                 0.0, 0.0, 0.0, 1.0);
}

/**
 * @brief Create a scale matrix.
 *
 * @param[in] s a vector.
 *
 * @return a scale matrix.
 */
const openvrml::mat4f openvrml::mat4f::scale(const vec3f & s) OPENVRML_NOTHROW
{
    return mat4f(s.x(), 0.0,   0.0,   0.0,
                 0.0,   s.y(), 0.0,   0.0,
                 0.0,   0.0,   s.z(), 0.0,
                 0.0,   0.0,   0.0,   1.0);
}

/**
 * @brief Create a translation matrix.
 *
 * @param[in] t translation vector.
 *
 * @return a translation matrix.
 */
const openvrml::mat4f openvrml::mat4f::translation(const vec3f & t)
    OPENVRML_NOTHROW
{
    return mat4f(1.0,   0.0,   0.0,   0.0,
                 0.0,   1.0,   0.0,   0.0,
                 0.0,   0.0,   1.0,   0.0,
                 t.x(), t.y(), t.z(), 1.0);
}

/**
 * @brief Create a transformation matrix from a translation, a rotation,
 *      a scale, a scaleOrientation, and a center.
 *
 * @param[in] t     the translation.
 * @param[in] r     the rotation.
 * @param[in] s     the scale.
 * @param[in] sr    the scale orientation.
 * @param[in] c     the center.
 *
 * @return a transformation matrix.
 */
const openvrml::mat4f
openvrml::mat4f::transformation(const vec3f & t,
                                const openvrml::rotation & r,
                                const vec3f & s,
                                const openvrml::rotation & sr,
                                const vec3f & c)
    OPENVRML_NOTHROW
{
    using openvrml_::fequal;

    mat4f mat;
    if (t != vec3f(0.0, 0.0, 0.0)) {
        mat = translation(t) * mat; // M = T * M   = T
    }
    if (c != vec3f(0.0, 0.0, 0.0)) {
        mat = translation(c) * mat; // M = C * M   = C * T
    }
    if (!fequal(r.angle(), 0.0f)) {
        mat = rotation(r) * mat; // M = R * M    = R * C * T
    }
    if (s != vec3f(1.0, 1.0, 1.0)) {
        if (!fequal(sr.angle(), 0.0f)) {
            mat = rotation(sr) * mat; // M = SR * M    = SR * R * C * T
        }
        mat = scale(s) * mat; // M = S * M     = S * SR * R * C * T
        if (!fequal(sr.angle(), 0.0f)) {
            // M = -SR * M   = -SR * S * SR * R * C * T
            mat = rotation(sr.inverse()) * mat;
        }
    }
    if (c != vec3f(0.0, 0.0, 0.0)) {
        // M = -C * M    =  -C * -SR * S * SR * R * C * T
        mat = translation(-c) * mat;
    }
    return mat;
}

/**
 * @brief Construct.
 *
 * The matrix is initialized to the identity matrix.
 */
openvrml::mat4f::mat4f() OPENVRML_NOTHROW
{
    this->mat[0][0] = 1.0;
    this->mat[0][1] = 0.0;
    this->mat[0][2] = 0.0;
    this->mat[0][3] = 0.0;

    this->mat[1][0] = 0.0;
    this->mat[1][1] = 1.0;
    this->mat[1][2] = 0.0;
    this->mat[1][3] = 0.0;

    this->mat[2][0] = 0.0;
    this->mat[2][1] = 0.0;
    this->mat[2][2] = 1.0;
    this->mat[2][3] = 0.0;

    this->mat[3][0] = 0.0;
    this->mat[3][1] = 0.0;
    this->mat[3][2] = 0.0;
    this->mat[3][3] = 1.0;
}

/**
 * @brief Construct mat4f with given 16 elements in row-major order.
 *
 * \f[ \left[ \begin{array}{cccc}
 *            f_{11} & f_{12} & f_{13} & f_{14} \\
 *            f_{21} & f_{22} & f_{23} & f_{24} \\
 *            f_{31} & f_{32} & f_{33} & f_{34} \\
 *            f_{41} & f_{42} & f_{43} & f_{44} \\
 *            \end{array} \right] \f]
 *
 * @param[in] f11
 * @param[in] f12
 * @param[in] f13
 * @param[in] f14
 * @param[in] f21
 * @param[in] f22
 * @param[in] f23
 * @param[in] f24
 * @param[in] f31
 * @param[in] f32
 * @param[in] f33
 * @param[in] f34
 * @param[in] f41
 * @param[in] f42
 * @param[in] f43
 * @param[in] f44
 */
openvrml::mat4f::mat4f(float f11, float f12, float f13, float f14,
                       float f21, float f22, float f23, float f24,
                       float f31, float f32, float f33, float f34,
                       float f41, float f42, float f43, float f44)
    OPENVRML_NOTHROW
{
    this->mat[0][0] = f11;
    this->mat[0][1] = f12;
    this->mat[0][2] = f13;
    this->mat[0][3] = f14;

    this->mat[1][0] = f21;
    this->mat[1][1] = f22;
    this->mat[1][2] = f23;
    this->mat[1][3] = f24;

    this->mat[2][0] = f31;
    this->mat[2][1] = f32;
    this->mat[2][2] = f33;
    this->mat[2][3] = f34;

    this->mat[3][0] = f41;
    this->mat[3][1] = f42;
    this->mat[3][2] = f43;
    this->mat[3][3] = f44;
}

/**
 * @brief Construct from an array of 16 values.
 *
 * @param[in] mat   an array of values in row-major order.
 */
openvrml::mat4f::mat4f(const float mat[16]) OPENVRML_NOTHROW
{
    std::copy(mat, mat + 16, &this->mat[0][0]);
}

/**
 * @brief Construct from a 4x4 array.
 *
 * @param[in] mat   a 4x4 array of elements in row-major order.
 */
openvrml::mat4f::mat4f(const float (&mat)[4][4]) OPENVRML_NOTHROW
{
    std::copy(&mat[0][0], &mat[0][0] + 16, &this->mat[0][0]);
}

/**
 * @brief Multiply by a scalar.
 *
 * @param[in] scalar    value by which to multiply.
 *
 * @return a reference to the object.
 */
openvrml::mat4f & openvrml::mat4f::operator*=(const float scalar)
    OPENVRML_NOTHROW
{
    this->mat[0][0] *= scalar;
    this->mat[0][1] *= scalar;
    this->mat[0][2] *= scalar;
    this->mat[0][3] *= scalar;
    this->mat[1][0] *= scalar;
    this->mat[1][1] *= scalar;
    this->mat[1][2] *= scalar;
    this->mat[1][3] *= scalar;
    this->mat[2][0] *= scalar;
    this->mat[2][1] *= scalar;
    this->mat[2][2] *= scalar;
    this->mat[2][3] *= scalar;
    this->mat[3][0] *= scalar;
    this->mat[3][1] *= scalar;
    this->mat[3][2] *= scalar;
    this->mat[3][3] *= scalar;
    return *this;
}

/**
 * @relatesalso openvrml::mat4f
 *
 * @brief Multiply a matrix by a scalar value.
 *
 * @param[in] mat       matrix.
 * @param[in] scalar    scalar.
 *
 * @return the result matrix.
 */
const openvrml::mat4f openvrml::operator*(const mat4f & mat,
                                          const float scalar)
    OPENVRML_NOTHROW
{
    mat4f result(mat);
    return result *= scalar;
}

/**
 * @relatesalso openvrml::mat4f
 *
 * @brief Multiply a scalar value by matrix.
 *
 * @param[in] scalar    scalar.
 * @param[in] mat       matrix.
 *
 * @return the result matrix.
 */
const openvrml::mat4f openvrml::operator*(const float scalar,
                                          const mat4f & mat)
    OPENVRML_NOTHROW
{
    mat4f result(mat);
    return result *= scalar;
}

/**
 * @brief Multiply by another matrix.
 *
 * Sets the matrix equal to the matrix multiplied by @p mat.
 *
 * @param[in] mat   the matrix by which to multiply.
 *
 * @return a reference to the object.
 */
openvrml::mat4f & openvrml::mat4f::operator*=(const mat4f & mat)
    OPENVRML_NOTHROW
{
    mat4f temp;

#define POSTMULT(i,j) (this->mat[i][0] * mat.mat[0][j] + \
                       this->mat[i][1] * mat.mat[1][j] + \
                       this->mat[i][2] * mat.mat[2][j] + \
                       this->mat[i][3] * mat.mat[3][j])

    temp[0][0] = POSTMULT(0,0);
    temp[0][1] = POSTMULT(0,1);
    temp[0][2] = POSTMULT(0,2);
    temp[0][3] = POSTMULT(0,3);
    temp[1][0] = POSTMULT(1,0);
    temp[1][1] = POSTMULT(1,1);
    temp[1][2] = POSTMULT(1,2);
    temp[1][3] = POSTMULT(1,3);
    temp[2][0] = POSTMULT(2,0);
    temp[2][1] = POSTMULT(2,1);
    temp[2][2] = POSTMULT(2,2);
    temp[2][3] = POSTMULT(2,3);
    temp[3][0] = POSTMULT(3,0);
    temp[3][1] = POSTMULT(3,1);
    temp[3][2] = POSTMULT(3,2);
    temp[3][3] = POSTMULT(3,3);

#undef POSTMULT

    *this = temp;
    return *this;
}

/**
 * @relatesalso openvrml::mat4f
 *
 * @brief Multiply two matrices.
 *
 * @param[in] lhs   left-hand operand.
 * @param[in] rhs   right-hand operand.
 *
 * @return the result matrix.
 */
const openvrml::mat4f openvrml::operator*(const mat4f & lhs, const mat4f & rhs)
    OPENVRML_NOTHROW
{
    mat4f result(lhs);
    return result *= rhs;
}


namespace {

    float det3(const openvrml::mat4f & mat,
               int r1, int r2, int r3, int c1, int c2, int c3) OPENVRML_NOTHROW
    {
        return mat[r1][c1] * mat[r2][c2] * mat[r3][c3]
             - mat[r1][c1] * mat[r2][c3] * mat[r3][c2]
             + mat[r1][c2] * mat[r2][c3] * mat[r3][c1]
             - mat[r1][c2] * mat[r2][c1] * mat[r3][c3]
             + mat[r1][c3] * mat[r2][c1] * mat[r3][c2]
             - mat[r1][c3] * mat[r2][c2] * mat[r3][c1];
    }
}

/**
 * @brief Get the translation, rotation and scale from the orthogonal
 *        transformation matrix.
 *
 * Any projection or shear information in the matrix is ignored.  The caller
 * is responsible for assuring that the transformation is in fact orthogonal;
 * otherwise the results are undefined.
 *
 * @param[out] t    translation.
 * @param[out] r    rotation.
 * @param[out] s    scale.
 */
void openvrml::mat4f::transformation(vec3f & t,
                                     openvrml::rotation & r,
                                     vec3f & s) const
    OPENVRML_NOTHROW
{
    //
    // Some portions are taken from Graphics Gems 2.
    //
    using openvrml_::fequal;

    //
    // Check if it is singular.
    //
    assert(!fequal(this->mat[3][3], 0.0f));
    if (fequal(det3(*this, 0, 1, 2, 0, 1, 2), 0.0f)){
       OPENVRML_PRINT_MESSAGE_("Warning: matrix is singular.");
       return;
    }

    mat4f tmp_matrix(*this);
    size_t i, j;

    //
    // Ignore perspective.
    //
    tmp_matrix[0][3] = tmp_matrix[1][3] = tmp_matrix[2][3] = 0.0;

    //
    // Normalize the matrix.
    //
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            tmp_matrix[i][j] /= tmp_matrix[3][3];
        }
    }
    t.x(tmp_matrix[3][0]);
    t.y(tmp_matrix[3][1]);
    t.z(tmp_matrix[3][2]);
    tmp_matrix[3][0] = tmp_matrix[3][1] = tmp_matrix[3][2] = 0.0;
    vec3f row_0(tmp_matrix[0][0], tmp_matrix[0][1], tmp_matrix[0][2]);
    vec3f row_1(tmp_matrix[1][0], tmp_matrix[1][1], tmp_matrix[1][2]);
    vec3f row_2(tmp_matrix[2][0], tmp_matrix[2][1], tmp_matrix[2][2]);

    //
    // Compute X scale factor and normalize first row.
    //
    s.x(row_0.length());
    row_0 = row_0.normalize();

    //
    // Compute Y scale factor and normalize second row.
    //
    s.y(row_1.length());
    row_1 = row_1.normalize();

    //
    // Compute Z scale factor and normalize third row.
    //
    s.z(row_2.length());
    row_2 = row_2.normalize();
    for (j = 0; j < 3; j++) {
        tmp_matrix[0][j] = row_0[j];
        tmp_matrix[1][j] = row_1[j];
        tmp_matrix[2][j] = row_2[j];
    }

    //
    // At this point, the matrix (in rows[]) is orthonormal.  That is, each
    // row has length one and are mutually perpendicular.  Is it necessary to
    // calculate the det of this matrix (1 or -1) for checking coordinate
    // system flip?  (According to VRML97, scale > 0.0.)  Calculate quaternion
    // rotation from this matrix.
    //
    quatf quat(tmp_matrix);

    //
    // now convert back to axis/angle.
    //
    r = openvrml::rotation(quat);
}

/**
 * @brief Get the translation, rotation, scale and shear from an affine
 *        transformation matrix.
 *
 * Any projection information in the matrix is ignored.  As VRML allows
 * non-uniform scaling, it is safe to call this routine.  The caller is
 * responsible for assuring that the transformation matrix is in fact affine;
 * otherwise the results are undefined.
 *
 * @param[out] t        translation.
 * @param[out] r        rotation.
 * @param[out] s        scale.
 * @param[out] shear    shear.
 */
void openvrml::mat4f::transformation(vec3f & t,
                                     openvrml::rotation & r,
                                     vec3f & s,
                                     vec3f & shear) const
    OPENVRML_NOTHROW
{
    //
    // Some portions are taken from Graphics Gems 2.
    //
    using openvrml_::fequal;

    //
    // Check if it is singular.
    //
    assert(!fequal(this->mat[3][3], 0.0f));
    if (fequal(det3(*this, 0, 1, 2, 0, 1, 2), 0.0f)){
        OPENVRML_PRINT_MESSAGE_("Warning: matrix is singular.");
        return;
    }

    mat4f tmp_matrix(*this);
    size_t i, j;

    //
    // Ignore perspective.
    //
    tmp_matrix[0][3] = tmp_matrix[1][3] = tmp_matrix[2][3] = 0.0;

    //
    // Normalize the matrix.
    //
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            tmp_matrix[i][j] /= tmp_matrix[3][3];
        }
    }
    t.x(tmp_matrix[3][0]);
    t.y(tmp_matrix[3][1]);
    t.z(tmp_matrix[3][2]);
    tmp_matrix[3][0] = tmp_matrix[3][1] = tmp_matrix[3][2] = 0.0;
    vec3f row_0(tmp_matrix[0][0], tmp_matrix[0][1], tmp_matrix[0][2]);
    vec3f row_1(tmp_matrix[1][0], tmp_matrix[1][1], tmp_matrix[1][2]);
    vec3f row_2(tmp_matrix[2][0], tmp_matrix[2][1], tmp_matrix[2][2]);

    //
    // Compute X scale factor and normalize first row.
    //
    s.x(row_0.length());
    row_0 = row_0.normalize();

    //
    // Compute XY shear factor and make 2nd row orthogonal to 1st.
    //
    float shear_xy = row_0.dot(row_1);
    row_1.x(row_1[0] - row_0[0] * shear_xy);
    row_1.y(row_1[1] - row_0[1] * shear_xy);
    row_1.z(row_1[2] - row_0[2] * shear_xy);

    //
    // Compute Y scale factor and normalize second row.
    //
    s.y(row_1.length());
    row_1 = row_1.normalize();
    shear.x(shear_xy / s.y());

    //
    // Compute XZ and YZ shears, orthogonalize third row.
    //
    float shear_xz = row_0.dot(row_2);
    row_2.x(row_2[0] - row_0[0] * shear_xz);
    row_2.y(row_2[1] - row_0[1] * shear_xz);
    row_2.z(row_2[2] - row_0[2] * shear_xz);
    float shear_yz = row_1.dot(row_2);
    row_2.x(row_2[0] - row_1[0] * shear_yz);
    row_2.y(row_2[1] - row_1[1] * shear_yz);
    row_2.z(row_2[2] - row_1[2] * shear_yz);

    //
    // Compute Z scale factor and normalize third row.
    //
    s.z(row_2.length());
    row_2 = row_2.normalize();
    shear.y(shear_xz / s.z());
    shear.z(shear_yz / s.z());
    for (j = 0; j < 3; j++){
        tmp_matrix[0][j] = row_0[j];
        tmp_matrix[1][j] = row_1[j];
        tmp_matrix[2][j] = row_2[j];
    }

    //
    // At this point, the matrix (in rows[]) is orthonormal.  That is, each
    // row has length one and are mutually perpendicular.  Is it needed to
    // calculate the det of this matrix (1 or -1) for checking coordinate
    // system flip?  (According to VRML standard, scale > 0.0.) Calculate
    // quaternion rotation from this matrix.
    //
    quatf quat(tmp_matrix);

    //
    // Now convert back to axis/angle.
    //
    r = openvrml::rotation(quat);
}

namespace {
    /**
     * This is taken from Graphics Gems 2, Page 603 and it is valid only for
     * an affine matrix with dimension of 4x4.  Since we are using row-major
     * order, this means the last column MUST be [0 0 0 1].
     *
     * By this procedure there is a significant performance improvement over a
     * general procedure that can invert any nonsingular matrix.
     *
     *            -1
     *  -1   |    |      |  -1    |
     * M   = |A  0|  =   | A     0|
     *       |    |      |        |
     *       |    |      |   -1   |
     *       |B  1|      |-BA    1|
     *
     *  where   M is a 4 by 4 matrix,
     *          A is the 3 by 3 upper left submatrix of M,
     *          B is the 1 by 3 lower left submatrix of M.
     *
     * It aborts if input matrix is not affine.  It returns without doing any
     * calculations if the input matrix is singular.
     *
     * @param[in] in   3D affine matrix.
     *
     * @param[out] out  inverse of 3D affine matrix.
     */
    void get_affine_inverse(const openvrml::mat4f & in, openvrml::mat4f & out)
    {
        // Check if it is an affine matrix
        assert(! ( in[0][3] != 0.0 || in[1][3] != 0.0 || in[2][3] != 0.0 ||
               in[3][3] != 1.0 ));

        double det;
        double pos, neg, temp;

        pos = neg = 0.0;
        temp =  in[0][0] * in[1][1] * in[2][2];
        if(temp >= 0.0) pos += temp; else neg += temp;
        temp =  in[0][1] * in[1][2] * in[2][0];
        if(temp >= 0.0) pos += temp; else neg += temp;
        temp =  in[0][2] * in[1][0] * in[2][1];
        if(temp >= 0.0) pos += temp; else neg += temp;
        temp = -in[0][2] * in[1][1] * in[2][0];
        if(temp >= 0.0) pos += temp; else neg += temp;
        temp = -in[0][1] * in[1][0] * in[2][2];
        if(temp >= 0.0) pos += temp; else neg += temp;
        temp = -in[0][0] * in[1][2] * in[2][1];
        if(temp >= 0.0) pos += temp; else neg += temp;
        det = pos + neg;

#define PRECISION_LIMIT (1.0e-25)

        // Is the submatrix A singular?

        if (det * det < PRECISION_LIMIT){
            OPENVRML_PRINT_MESSAGE_("Warning : Matrix is singular");
            return;
        }

        // Calculate inverse(A) = adj(A) / det(A)

        det = 1.0 / det;
        out[0][0] =  float((in[1][1] * in[2][2] - in[1][2] * in[2][1]) * det);
        out[1][0] = -float((in[1][0] * in[2][2] - in[1][2] * in[2][0]) * det);
        out[2][0] =  float((in[1][0] * in[2][1] - in[1][1] * in[2][0]) * det);
        out[0][1] = -float((in[0][1] * in[2][2] - in[0][2] * in[2][1]) * det);
        out[1][1] =  float((in[0][0] * in[2][2] - in[0][2] * in[2][0]) * det);
        out[2][1] = -float((in[0][0] * in[2][1] - in[0][1] * in[2][0]) * det);
        out[0][2] =  float((in[0][1] * in[1][2] - in[0][2] * in[1][1]) * det);
        out[1][2] = -float((in[0][0] * in[1][2] - in[0][2] * in[1][0]) * det);
        out[2][2] =  float((in[0][0] * in[1][1] - in[0][1] * in[1][0]) * det);

        // Calculate -B * inverse(A) Do the translation part

        out[3][0] = -( in[3][0] * out[0][0] +
                       in[3][1] * out[1][0] +
                       in[3][2] * out[2][0] );
        out[3][1] = -( in[3][0] * out[0][1] +
                       in[3][1] * out[1][1] +
                       in[3][2] * out[2][1] );
        out[3][2] = -( in[3][0] * out[0][2] +
                       in[3][1] * out[1][2] +
                       in[3][2] * out[2][2] );

        // Fill in last column
        out[0][3] = out[1][3] = out[2][3] = 0.0;
        out[3][3] = 1.0;

#undef PRECISION_LIMIT

    }
}

/**
 * @brief Get the inverse of an affine matrix.
 *
 * @return the inverse of the matrix.
 *
 * @pre the current matrix must be affine.
 */
const openvrml::mat4f openvrml::mat4f::inverse() const OPENVRML_NOTHROW
{
    mat4f out;
    get_affine_inverse(*this, out);
    return out;
}

/**
 * @brief Get the transposition of the matrix.
 *
 * @return a transposed copy of the matrix.
 */
const openvrml::mat4f openvrml::mat4f::transpose() const OPENVRML_NOTHROW
{
    return mat4f(this->mat[0][0],
                 this->mat[1][0],
                 this->mat[2][0],
                 this->mat[3][0],
                 this->mat[0][1],
                 this->mat[1][1],
                 this->mat[2][1],
                 this->mat[3][1],
                 this->mat[0][2],
                 this->mat[1][2],
                 this->mat[2][2],
                 this->mat[3][2],
                 this->mat[0][3],
                 this->mat[1][3],
                 this->mat[2][3],
                 this->mat[3][3]);
}

/**
 * @brief Determinant.
 *
 * @return the determinant.
 */
float openvrml::mat4f::det() const OPENVRML_NOTHROW
{
    return (  this->mat[0][0] * det3(*this, 1, 2, 3, 1, 2, 3)
            + this->mat[0][1] * det3(*this, 1, 2, 3, 0, 2, 3)
            + this->mat[0][2] * det3(*this, 1, 2, 3, 0, 1, 3)
            + this->mat[0][3] * det3(*this, 1, 2, 3, 0, 1, 2));
}

/**
 * @fn float (&openvrml::mat4f::operator[](size_t index))[4]
 *
 * @brief Row access.
 *
 * @param[in] index row index.
 *
 * @return row @p index.
 */

/**
 * @fn float (&openvrml::mat4f::operator[](size_t index) const)[4]
 *
 * @brief Row access.
 *
 * @param[in] index row index.
 *
 * @return row @p index.
 */

/**
 * @relatesalso openvrml::mat4f
 *
 * @brief Stream input.
 *
 * Consistent with the VRML97 convention, commas (&ldquo;@c ,&rdquo;) in the
 * input are treated as whitespace.
 *
 * Optionally, brackets may be used in the input to group the rows; i.e., the
 * following syntaxes are accepted:
 *
 * - [&nbsp;<var>f<sub>11</sub></var>,&nbsp;... <var>f<sub>14</sub></var>&nbsp;],&nbsp;... [&nbsp;<var>f<sub>41</sub></var>,&nbsp;... <var>f<sub>44</sub></var>&nbsp;]
 * - <var>f<sub>11</sub></var>, <var>f<sub>12</sub></var>,&nbsp;... <var>f<sub>44</sub></var>
 *
 * @param[in,out] in    input stream.
 * @param[out] m        a matrix.
 *
 * @return @p in.
 */
std::istream & openvrml::operator>>(std::istream & in, mat4f & m)
{
    using std::istreambuf_iterator;
    using boost::spirit::make_multi_pass;
    using boost::spirit::ch_p;
    using boost::spirit::real_p;
    using boost::spirit::assign_a;
    using boost::spirit::increment_a;
    using boost::spirit::decrement_a;

    iter_policy_t iter_policy(vrml97_space_p);
    scanner_policies_t policies(iter_policy);
    iterator_t
        first(make_multi_pass(istreambuf_iterator<char_t>(in))),
        last(make_multi_pass(istreambuf_iterator<char_t>()));

    scanner_t scan(first, last, policies);

    size_t row1_bracket_count = 0, row2_bracket_count = 0, row3_bracket_count = 0,
        row4_bracket_count = 0;

    rule_t r
        =   !ch_p('[')[increment_a(row1_bracket_count)]
                >> real_p[assign_a(m[0][0])]
                >> real_p[assign_a(m[0][1])]
                >> real_p[assign_a(m[0][2])]
                >> real_p[assign_a(m[0][3])]
            >> !ch_p(']')[decrement_a(row1_bracket_count)]
            >> !ch_p('[')[increment_a(row2_bracket_count)]
                >> real_p[assign_a(m[1][0])]
                >> real_p[assign_a(m[1][1])]
                >> real_p[assign_a(m[1][2])]
                >> real_p[assign_a(m[1][3])]
            >> !ch_p(']')[decrement_a(row2_bracket_count)]
            >> !ch_p('[')[increment_a(row3_bracket_count)]
                >> real_p[assign_a(m[2][0])]
                >> real_p[assign_a(m[2][1])]
                >> real_p[assign_a(m[2][2])]
                >> real_p[assign_a(m[2][3])]
            >> !ch_p(']')[decrement_a(row3_bracket_count)]
            >> !ch_p('[')[increment_a(row4_bracket_count)]
                >> real_p[assign_a(m[3][0])]
                >> real_p[assign_a(m[3][1])]
                >> real_p[assign_a(m[3][2])]
                >> real_p[assign_a(m[3][3])]
            >> !ch_p(']')[decrement_a(row4_bracket_count)]
        ;

    boost::spirit::match<> match = r.parse(scan);

    if (!match || row1_bracket_count != 0 || row2_bracket_count != 0
        || row3_bracket_count != 0 || row4_bracket_count != 0) {
        in.setstate(std::ios_base::failbit);
    }

    return in;
}

/**
 * @relatesalso openvrml::mat4f
 *
 * @brief Stream output.
 *
 * @param[in,out] out   an output stream.
 * @param[in] mat       a matrix.
 *
 * @return @p out.
 */
std::ostream & openvrml::operator<<(std::ostream & out, const mat4f & mat)
{
    for (size_t i = 0; i < 4; i++) {
        out << '[';
        for (size_t j = 0; j < 4; j++) {
            out << mat[i][j];
            if (j != 3) { out << ", "; }
        }
        out << ']';
        if (i != 3) { out << ", "; }
    }
    return out;
}


/**
 * @relatesalso openvrml::mat4f
 *
 * @brief Equality comparison operator.
 *
 * All componenents must match exactly.
 *
 * @param[in] lhs   a matrix.
 * @param[in] rhs   a matrix.
 *
 * @return @c true if @p lhs and @p rhs are equivalent; @c false otherwise.
 */
bool openvrml::operator==(const mat4f & lhs, const mat4f & rhs)
    OPENVRML_NOTHROW
{
    using openvrml_::fequal;
    return std::equal(&lhs[0][0], &lhs[0][0] + 16, &rhs[0][0], fequal);
}

/**
 * @relatesalso openvrml::mat4f
 *
 * @brief Inequality comparison operator.
 *
 * @param[in] lhs   a matrix.
 * @param[in] rhs   a matrix.
 *
 * @return @c true if @p lhs and @p rhs are not equivalent; @c false otherwise.
 */
bool openvrml::operator!=(const mat4f & lhs, const mat4f & rhs)
    OPENVRML_NOTHROW
{
    return !(lhs == rhs);
}


/**
 * @class openvrml::quatf
 *
 * @brief A quaternion.
 */

/**
 * @internal
 *
 * @var float openvrml::quatf::quat[4]
 *
 * @brief An array comprising the quaternion components.
 */

/**
 * @brief Default constructor.
 */
openvrml::quatf::quatf() OPENVRML_NOTHROW
{
    this->quat[0] = 0.0;
    this->quat[1] = 0.0;
    this->quat[2] = 0.0;
    this->quat[3] = 1.0;
}

/**
 * @brief Construct from four values.
 *
 * @param[in] x the x vector component.
 * @param[in] y the y vector component.
 * @param[in] z the z vector component.
 * @param[in] w the scalar value w.
 */
openvrml::quatf::quatf(const float x,
                       const float y,
                       const float z,
                       const float w)
    OPENVRML_NOTHROW
{
    this->quat[0] = x;
    this->quat[1] = y;
    this->quat[2] = z;
    this->quat[3] = w;
}

/**
 * @brief Construct from an array of four values.
 *
 * @param[in] quat  the first three values in the array are used for the x, y,
 *                  and z vector components, respectively.  The fourth value
 *                  in the array is used for the scalar part of the
 *                  quaternion.
 */
openvrml::quatf::quatf(const float (&quat)[4]) OPENVRML_NOTHROW
{
    this->quat[0] = quat[0];
    this->quat[1] = quat[1];
    this->quat[2] = quat[2];
    this->quat[3] = quat[3];
}

/**
 * @brief Construct from a rotation matrix.
 *
 * @param[in] mat   a rotation matrix.
 */
openvrml::quatf::quatf(const mat4f & mat) OPENVRML_NOTHROW
{
    float diagonal, s;
    diagonal = mat[0][0] + mat[1][1] + mat[2][2];
    // check the diagonal
    if (diagonal > 0.0) {
        s = float(sqrt(diagonal + 1.0));
        this->quat[3] = s / 2.0f;
        s = 0.5f / s;
        this->quat[0] = (mat[1][2] - mat[2][1]) * s;
        this->quat[1] = (mat[2][0] - mat[0][2]) * s;
        this->quat[2] = (mat[0][1] - mat[1][0]) * s;
    } else {
        using openvrml_::fequal;
        size_t i, j, k;
        static const size_t next[3] = { 1, 2, 0 };
        // diagonal is negative
        i = 0;
        if (mat[1][1] > mat[0][0]) { i = 1; }
        if (mat[2][2] > mat[i][i]) { i = 2; }
        j = next[i];
        k = next[j];
        s = float(sqrt((mat[i][i] - (mat[j][j] + mat[k][k])) + 1.0));
        this->quat[i] = s * 0.5f;

        if (!fequal(s, 0.0f)) { s = 0.5f / s; }
        this->quat[3] = (mat[j][k] - mat[k][j]) * s;
        this->quat[j] = (mat[i][j] + mat[j][i]) * s;
        this->quat[k] = (mat[i][k] + mat[k][i]) * s;
    }
}

/**
 * @brief Construct from an rotation.
 *
 * @param[in] rot   a rotation.
 */
openvrml::quatf::quatf(const rotation & rot) OPENVRML_NOTHROW
{
    assert(openvrml_::fequal(rot.axis().length(), 1.0f));

    const float sin_angle = float(sin(rot.angle() / 2.0));
    this->quat[0] = rot.x() * sin_angle;
    this->quat[1] = rot.y() * sin_angle;
    this->quat[2] = rot.z() * sin_angle;
    this->quat[3] = float(cos(rot.angle() / 2.0));
}

/**
 * @brief Multiply by a quaternion.
 *
 * @note \f[
 *       qr = ( \vec{q_v} \times \vec{r_v} + r_w \vec{q_v} + q_w \vec{r_v}, q_w r_w - \vec{q_v} \cdotp \vec{r_v})
 *       \f]
 *
 * @param[in] quat  the quaternion by which to multiply.
 *
 * @return a reference to the object.
 */
openvrml::quatf & openvrml::quatf::operator*=(const quatf & quat)
    OPENVRML_NOTHROW
{
    *this = quatf(this->quat[1] * quat[2] - this->quat[2] * quat[1]
                    + quat[3] * this->quat[0] + this->quat[3] * quat[0],
                  this->quat[2] * quat[0] - this->quat[0] * quat[2]
                    + quat[3] * this->quat[1] + this->quat[3] * quat[1],
                  this->quat[0] * quat[1] - this->quat[1] * quat[0]
                    + quat[3] * this->quat[2] + this->quat[3] * quat[2],
                  this->quat[3] * quat[3]
                    - (this->quat[0] * quat[0] + this->quat[1] * quat[1]
                        + this->quat[2] * quat[2]));
    return *this;
}

/**
 * @relatesalso openvrml::quatf
 *
 * @brief Multiply two quaternions.
 *
 * @param[in] lhs   left-hand operand.
 * @param[in] rhs   right-hand operand.
 *
 * @return the product of @p lhs and @p rhs.
 */
const openvrml::quatf openvrml::operator*(const quatf & lhs, const quatf & rhs)
    OPENVRML_NOTHROW
{
    quatf result(lhs);
    return result *= rhs;
}

/**
 * @brief Multiply by a scalar.
 *
 * @param[in] scalar    value by which to multiply.
 *
 * @return a reference to the object.
 */
openvrml::quatf & openvrml::quatf::operator*=(const float scalar)
    OPENVRML_NOTHROW
{
    this->quat[0] *= scalar;
    this->quat[1] *= scalar;
    this->quat[2] *= scalar;
    this->quat[3] *= scalar;
    return *this;
}

/**
 * @relatesalso openvrml::quatf
 *
 * @brief Multiply a quaternion by a scalar.
 *
 * @param[in] quat      quaternion.
 * @param[in] scalar    scalar.
 *
 * @return the product of @p quat and @p scalar.
 */
const openvrml::quatf openvrml::operator*(const quatf & quat,
                                          const float scalar)
    OPENVRML_NOTHROW
{
    quatf result(quat);
    return result *= scalar;
}

/**
 * @relatesalso openvrml::quatf
 *
 * @brief Multiply a scalar by a quaternion.
 *
 * @param[in] scalar    scalar.
 * @param[in] quat      quaternion.
 *
 * @return the product of @p scalar and @p quat.
 */
const openvrml::quatf openvrml::operator*(const float scalar,
                                          const quatf & quat)
    OPENVRML_NOTHROW
{
    quatf result(quat);
    return result *= scalar;
}

/**
 * @brief Divide by a scalar.
 *
 * @param[in] scalar    value by which to divide.
 *
 * @return a reference to the object.
 */
openvrml::quatf & openvrml::quatf::operator/=(const float scalar)
    OPENVRML_NOTHROW
{
    assert(scalar != 0.0);
    this->quat[0] /= scalar;
    this->quat[1] /= scalar;
    this->quat[2] /= scalar;
    this->quat[3] /= scalar;
    return *this;
}

/**
 * @relatesalso openvrml::quatf
 *
 * @brief Divide a quaternion by a scalar.
 *
 * @param[in] quat      quaternion.
 * @param[in] scalar    scalar.
 *
 * @return the result of dividing @p quat by @p scalar.
 */
const openvrml::quatf openvrml::operator/(const quatf & quat,
                                          const float scalar)
    OPENVRML_NOTHROW
{
    quatf result(quat);
    return result /= scalar;
}

/**
 * @brief Add a quaternion.
 *
 * @param[in] quat  the quaternion to add.
 *
 * @return a reference to the object.
 */
openvrml::quatf & openvrml::quatf::operator+=(const quatf & quat)
    OPENVRML_NOTHROW
{
    this->quat[0] += quat[0];
    this->quat[1] += quat[1];
    this->quat[2] += quat[2];
    this->quat[3] += quat[3];
    return *this;
}

/**
 * @relatesalso openvrml::quatf
 *
 * @brief Add two quaternions.
 *
 * @param[in] lhs   left-hand operand.
 * @param[in] rhs   right-hand operand.
 *
 * @return the sum of @p lhs and @p rhs.
 */
const openvrml::quatf openvrml::operator+(const quatf & lhs, const quatf & rhs)
    OPENVRML_NOTHROW
{
    quatf result(lhs);
    return result += rhs;
}

/**
 * @brief Subtract a quaternion.
 *
 * @param[in] quat  the quaternion to subtract.
 *
 * @return a reference to the object.
 */
openvrml::quatf & openvrml::quatf::operator-=(const quatf & quat)
    OPENVRML_NOTHROW
{
    this->quat[0] -= quat[0];
    this->quat[1] -= quat[1];
    this->quat[2] -= quat[2];
    this->quat[3] -= quat[3];
    return *this;
}

/**
 * @relatesalso openvrml::quatf
 *
 * @brief Take the difference between two quaternions.
 *
 * @param[in] lhs   left-hand operand.
 * @param[in] rhs   right-hand operand.
 *
 * @return the difference between @p lhs and @p rhs.
 */
const openvrml::quatf openvrml::operator-(const quatf & lhs, const quatf & rhs)
    OPENVRML_NOTHROW
{
    quatf result(lhs);
    return result -= rhs;
}

/**
 * @fn float openvrml::quatf::operator[](size_t index) const
 *
 * @brief Array element dereference operator (const version).
 *
 * @param[in] index an index from 0&ndash;3.
 *
 * @pre @p index is not larger than 3.
 */

/**
 * @fn float & openvrml::quatf::operator[](size_t index)
 *
 * @brief Array element dereference operator (non-const version).
 *
 * @param[in] index an index from 0&ndash;3.
 *
 * @pre @p index is not larger than 3.
 */

/**
 * @fn float openvrml::quatf::x() const
 *
 * @brief Get the x component.
 *
 * @return the x component value.
 */

/**
 * @fn void openvrml::quatf::x(float value)
 *
 * @brief Set the x component.
 *
 * @param[in] value x component value.
 */

/**
 * @fn float openvrml::quatf::y() const
 *
 * @brief Get the y component.
 *
 * @return the y component value.
 */

/**
 * @fn void openvrml::quatf::y(float value)
 *
 * @brief Set the y component.
 *
 * @param[in] value y component value.
 */

/**
 * @fn float openvrml::quatf::z() const
 *
 * @brief Get the z component.
 *
 * @return the z component value.
 */

/**
 * @fn void openvrml::quatf::z(float value)
 *
 * @brief Set the z component.
 *
 * @param[in] value z component value.
 */

/**
 * @fn float openvrml::quatf::w() const
 *
 * @brief Get the w component.
 *
 * @return the w component value.
 */

/**
 * @fn void openvrml::quatf::w(float value)
 *
 * @brief Set the w component.
 *
 * @param[in] value w component value.
 */

/**
 * @brief Get the conjugate.
 *
 * @note \f[ q^* = (- \vec{q_v}, q_w) \f]
 *
 * @return the conjugate of the quaternion.
 */
const openvrml::quatf openvrml::quatf::conjugate() const OPENVRML_NOTHROW
{
    const quatf q(-this->quat[0],
                  -this->quat[1],
                  -this->quat[2],
                  this->quat[3]);
    return q;
}

/**
 * @brief Get the inverse.
 *
 * @note \f[ q^{-1} = \frac{q^*}{n(q)} \f]
 *
 * @return the multiplicative inverse.
 */
const openvrml::quatf openvrml::quatf::inverse() const OPENVRML_NOTHROW
{
    return this->conjugate() / this->norm();
}

/**
 * @brief Get the norm.
 *
 * @note \f[ n(q) = q_x^2 + q_y^2 + q_z^2 + q_w^2 \f]
 *
 * @return the norm.
 */
float openvrml::quatf::norm() const OPENVRML_NOTHROW
{
    return this->quat[0] * this->quat[0]
            + this->quat[1] * this->quat[1]
            + this->quat[2] * this->quat[2]
            + this->quat[3] * this->quat[3];
}

/**
 * @brief Normalize the quaternion.
 *
 * @return a unit quaternion derived from the quaternion.
 */
const openvrml::quatf openvrml::quatf::normalize() const OPENVRML_NOTHROW
{
    const float n = this->norm();
    const quatf q(this->quat[0] / n,
                       this->quat[1] / n,
                       this->quat[2] / n,
                       this->quat[3] / n);
    return q;
}

/**
 * @brief Stream input.
 *
 * Consistent with the VRML97 convention, commas (&ldquo;@c ,&rdquo;) in the
 * input are treated as whitespace.
 *
 * @param[in,out] in    input stream.
 * @param[out] q        a @c quatf.
 *
 * @return @p in.
 */
std::istream & openvrml::operator>>(std::istream & in, quatf & q)
{
    using std::istreambuf_iterator;
    using boost::spirit::make_multi_pass;
    using boost::spirit::match;
    using boost::spirit::real_p;
    using boost::spirit::assign_a;

    iter_policy_t iter_policy(vrml97_space_p);
    scanner_policies_t policies(iter_policy);
    iterator_t
        first(make_multi_pass(istreambuf_iterator<char_t>(in))),
        last(make_multi_pass(istreambuf_iterator<char_t>()));

    scanner_t scan(first, last, policies);

    rule_t r
        =   real_p[assign_a(q.quat[0])]
            >> real_p[assign_a(q.quat[1])]
            >> real_p[assign_a(q.quat[2])]
            >> real_p[assign_a(q.quat[3])]
        ;

    match<> m = r.parse(scan);

    if (!m) {
        in.setstate(std::ios_base::failbit);
    }

    return in;
}

/**
 * @relatesalso openvrml::quatf
 *
 * @brief Stream output.
 *
 * @param[in,out] out   an output stream.
 * @param[in] quat      a quaternion.
 *
 * @return @p out.
 */
std::ostream & openvrml::operator<<(std::ostream & out, const quatf & quat)
{
    return out << quat.x() << ' ' << quat.y() << ' ' << quat.z() << ' '
               << quat.w();
}

/**
 * @relatesalso openvrml::quatf
 *
 * @brief Compare for equality.
 *
 * @param[in] lhs   left-hand operand.
 * @param[in] rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs are equal; @c false otherwise.
 */
bool openvrml::operator==(const quatf & lhs, const quatf & rhs)
    OPENVRML_NOTHROW
{
    using openvrml_::fequal;
    return std::equal(&lhs[0], &lhs[0] + 4, &rhs[0], fequal);
}

/**
 * @relatesalso openvrml::quatf
 *
 * @brief Compare for inequality.
 *
 * @param[in] lhs   left-hand operand.
 * @param[in] rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs are not equal; @c false otherwise.
 */
bool openvrml::operator!=(const quatf & lhs, const quatf & rhs)
    OPENVRML_NOTHROW
{
    return !(lhs == rhs);
}


/**
 * @class openvrml::image
 *
 * @brief Pixmap data.
 *
 * The first pixel in the @link image::array array@endlink is the lower left
 * pixel and the last is the upper right pixel.  Pixel values are limited to
 * 256 levels of intensity.  The elements of @link image::array array@endlink
 * are bytes, with one byte per pixel component.  Thus the number of elements
 * in the array is @p x * @p y * @p comp.
 *
 * A one-component image specifies one-byte greyscale values.  A two-component
 * image specifies the intensity in the first byte and the alpha opacity in
 * the second byte.  A three-component image specifies the red component in
 * the first byte, followed by the green and blue components.  Four-component
 * images specify the alpha opacity byte after red/green/blue.
 */

/**
 * @internal
 *
 * @var size_t openvrml::image::x_
 *
 * @brief Pixels in the <var>x</var>-dimension.
 */

/**
 * @internal
 *
 * @var size_t openvrml::image::y_
 *
 * @brief Pixels in the <var>y</var>-dimension.
 */

/**
 * @internal
 *
 * @var size_t openvrml::image::comp_
 *
 * @brief Number of components.
 */

/**
 * @internal
 *
 * @var std::vector<unsigned char> openvrml::image::array_
 *
 * @brief Pixel data.
 */

/**
 * @brief Construct.
 */
openvrml::image::image() OPENVRML_NOTHROW:
    x_(0),
    y_(0),
    comp_(0)
{}

/**
 * @brief Construct.
 *
 * @param[in] x     pixels in the <var>x</var>-direction.
 * @param[in] y     pixels in the <var>y</var>-direction.
 * @param[in] comp  number of components.
 */
openvrml::image::image(const size_t x,
                       const size_t y,
                       const size_t comp)
    OPENVRML_THROW1(std::bad_alloc):
    x_(x),
    y_(y),
    comp_(comp),
    array_(x * y * comp)
{}

/**
 * @brief Construct.
 *
 * @pre @p array.size() <= @p x * @p y * @p comp.
 *
 * @param[in] x     pixels in the <var>x</var>-direction.
 * @param[in] y     pixels in the <var>y</var>-direction.
 * @param[in] comp  number of components.
 * @param[in] array pixel data.
 */
openvrml::image::image(const size_t x,
                       const size_t y,
                       const size_t comp,
                       const std::vector<unsigned char> & array)
    OPENVRML_THROW1(std::bad_alloc):
    x_(x),
    y_(y),
    comp_(comp),
    array_(array)
{
    assert(array.size() <= x * y * comp);
}

/**
 * @fn template <typename InputIterator> openvrml::image::image(size_t x, size_t y, size_t comp, InputIterator array_begin, InputIterator array_end)
 *
 * @brief Construct.
 *
 * @pre std::distance(@p array_begin, @p array_end) <= @p x * @p y * @p comp.
 *
 * @param[in] x             pixels in the <var>x</var>-direction.
 * @param[in] y             pixels in the <var>y</var>-direction.
 * @param[in] comp          number of components.
 * @param[in] array_begin   pixel data begin iterator.
 * @param[in] array_end     pixel data end iterator.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */

/**
 * @brief Swap.
 *
 * @param[in,out] img   an image.
 */
void openvrml::image::swap(image & img) OPENVRML_NOTHROW
{
    using std::swap;
    swap(this->x_, img.x_);
    swap(this->y_, img.y_);
    swap(this->comp_, img.comp_);
    swap(this->array_, img.array_);
}

/**
 * @fn size_t openvrml::image::x() const
 *
 * @brief Pixels in the <var>x</var>-dimension.
 *
 * @return the number of pixels in the <var>x</var>-dimension.
 */

/**
 * @brief Set the pixels in the <var>x</var>-dimension.
 *
 * @param[in] value new value for the <var>x</var>-dimension.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void openvrml::image::x(const size_t value) OPENVRML_THROW1(std::bad_alloc)
{
    //
    // Throws std::bad_alloc.
    //
    this->array_.resize(value * this->y_ * this->comp_);
    this->x_ = value;
}

/**
 * @fn size_t openvrml::image::y() const
 *
 * @brief Pixels in the <var>y</var>-dimension.
 *
 * @return the number of pixels in the <var>y</var>-dimension.
 */

/**
 * @brief Set the pixels in the <var>y</var>-dimension.
 *
 * @param[in] value new value for the <var>x</var>-dimension.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void openvrml::image::y(const size_t value) OPENVRML_THROW1(std::bad_alloc)
{
    //
    // Throws std::bad_alloc.
    //
    this->array_.resize(this->x_ * value * this->comp_);
    this->y_ = value;
}

/**
 * @brief Resize the <var>x</var>- and <var>y</var>-dimensions.
 *
 * @param[in] x <var>x</var>-dimension.
 * @param[in] y <var>y</var>-dimension.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void openvrml::image::resize(const size_t x, const size_t y)
    OPENVRML_THROW1(std::bad_alloc)
{
    this->array_.resize(x * y * this->comp_); // Throws std::bad_alloc.
    this->x_ = x;
    this->y_ = y;
}

/**
 * @fn size_t openvrml::image::comp() const
 *
 * @brief Number of components.
 *
 * @return the number of components.
 */

/**
 * @brief Set the number of components.
 *
 * @pre @p value <= 4
 *
 * @param[in] value number of components.
 */
void openvrml::image::comp(const size_t value) OPENVRML_THROW1(std::bad_alloc)
{
    assert(value <= 4);
    this->array_.resize(this->x_ * this->y_ * value);
    this->comp_ = value;
}


/**
 * @fn const std::vector<unsigned char> & openvrml::image::array() const
 *
 * @brief Pixel value array.
 *
 * @return the array of pixel values.
 */

/**
 * @fn void openvrml::image::array(const std::vector<unsigned char> & value)
 *
 * @brief Set the pixel value array.
 *
 * @pre @p value.size() <= (x() * y() * comp())
 *
 * @param[in] value pixel value array.
 */

/**
 * @fn template <typename InputIterator> void openvrml::image::array(InputIterator begin, InputIterator end)
 *
 * @brief Set the pixel value array.
 *
 * @pre std::distance(@p begin, @p end) <= (x() * y() * comp())
 *
 * @param[in] begin input iterator to the beginning of a sequence.
 * @param[in] end   input iterator to the end of the sequence (one past the last
 *                  element).
 */

/**
 * @fn unsigned long openvrml::image::pixel(size_t index) const
 *
 * @brief Pixel value.
 *
 * @pre @p index < x() * y()
 *
 * @param[in] index pixel value index.
 *
 * @return the pixel value at @p index.
 */

/**
 * @fn void openvrml::image::pixel(size_t index, unsigned long value)
 *
 * @brief Set a pixel value.
 *
 * @pre @p index < x() * y()
 *
 * @param[in] index pixel value index.
 * @param[in] value pixel value.
 */

/**
 * @fn unsigned long openvrml::image::pixel(size_t x, size_t y) const
 *
 * @brief Pixel value.
 *
 * @pre @p x < x(), @p y < y()
 *
 * @param[in] x pixel value <var>x</var>- (column) index.
 * @param[in] y pixel value <var>y</var>- (row) index.
 *
 * @return the pixel value at (@p x, @p y).
 */

/**
 * @fn void openvrml::image::pixel(size_t x, size_t y, unsigned long value)
 *
 * @brief Set a pixel value.
 *
 * @pre @p x < x(), @p y < y()
 *
 * @param[in] x     pixel value <var>x</var>- (column) index.
 * @param[in] y     pixel value <var>y</var>- (row) index.
 * @param[in] value pixel value.
 */

/**
 * @relatesalso openvrml::image
 *
 * @brief Compare for equality.
 *
 * @param[in] lhs   left-hand operand.
 * @param[in] rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs are equal; @c false otherwise.
 */
bool openvrml::operator==(const image & lhs, const image & rhs)
    OPENVRML_NOTHROW
{
    return lhs.x() == rhs.x()
        && lhs.y() == rhs.y()
        && lhs.comp() == rhs.comp()
        && lhs.array() == rhs.array();
}

/**
 * @relatesalso openvrml::image
 *
 * @brief Compare for inequality.
 *
 * @param[in] lhs   left-hand operand.
 * @param[in] rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs are not equal; @c false otherwise.
 */
bool openvrml::operator!=(const image & lhs, const image & rhs)
    OPENVRML_NOTHROW
{
    return !(lhs == rhs);
}

namespace {

    struct OPENVRML_LOCAL set_pixel {
        set_pixel(openvrml::image & img, size_t & index):
            img_(&img),
            index_(&index)
        {}

        template <typename NumT>
        void operator()(NumT val) const
        {
            this->img_->pixel(*this->index_, val);
        }

    private:
        openvrml::image * const img_;
        size_t * const index_;
    };

    struct OPENVRML_LOCAL resize_image {
        resize_image(openvrml::image & img,
                     size_t & x, size_t & y, size_t & comp):
            img_(&img),
            x_(&x),
            y_(&y),
            comp_(&comp)
        {}

        template <typename IteratorT>
        void operator()(IteratorT, IteratorT) const
        {
            openvrml::image temp(*this->x_, *this->y_, *this->comp_);
            this->img_->swap(temp);
        }

    private:
        openvrml::image * const img_;
        size_t * const x_, * const y_, * const comp_;
    };


    struct OPENVRML_LOCAL int32_parser {
        typedef openvrml::int32 result_t;

        template <typename ScannerT>
        std::ptrdiff_t operator()(const ScannerT & scan,
                                  result_t & result) const
        {
            using namespace boost::spirit;
            using namespace phoenix;
            typedef typename match_result<ScannerT, result_t>::type match_t;
            match_t match
                =   (   lexeme_d[
                            ch_p('0') >> chset_p("Xx")
                            >> hex_p[assign_a(result)]
                        ]
                    |   int_p[assign_a(result)]
                    ).parse(scan);
            return match.length();
        }
    };

    const boost::spirit::functor_parser<int32_parser> int32_p = int32_parser();
}

/**
 * @brief Stream input.
 *
 * Consistent with the VRML97 convention, commas (&ldquo;@c ,&rdquo;) in the
 * input are treated as whitespace.
 *
 * @param[in,out] in    input stream.
 * @param[out] img      an @c image.
 *
 * @return @p in.
 */
std::istream & openvrml::operator>>(std::istream & in, image & img)
{
    using std::istreambuf_iterator;
    using boost::ref;
    using boost::spirit::make_multi_pass;
    using boost::spirit::match;
    using boost::spirit::eps_p;
    using boost::spirit::int_p;
    using boost::spirit::repeat_p;
    using boost::spirit::assign_a;
    using boost::spirit::increment_a;
    using phoenix::arg1;
    using phoenix::var;

    iter_policy_t iter_policy(vrml97_space_p);
    scanner_policies_t policies(iter_policy);
    iterator_t
        first(make_multi_pass(istreambuf_iterator<char_t>(in))),
        last(make_multi_pass(istreambuf_iterator<char_t>()));

    scanner_t scan(first, last, policies);

    size_t x = 0, y = 0, comp = 0, pixels = 0, index = 0;
    rule_t r
        =   int32_p[var(pixels) = arg1][assign_a(x)]
            >> int32_p[var(pixels) *= arg1][assign_a(y)]
            >> int32_p[assign_a(comp)]
            // Just resize the image once we have the x, y, and comp values to
            // avoid unnecessary reallocation.
            >> eps_p[resize_image(img, x, y, comp)]
            >> repeat_p(ref(pixels))[
                int32_p[set_pixel(img, index)][increment_a(index)]
            ]
        ;

    match<> m = r.parse(scan);

    if (!m) {
        in.setstate(std::ios_base::failbit);
    }

    return in;
}

/**
 * @relatesalso openvrml::image
 *
 * @brief Stream output.
 *
 * @param[in,out] out   output stream.
 * @param[in] img   image.
 */
std::ostream & openvrml::operator<<(std::ostream & out, const image & img)
{
    using std::vector;
    using std::ios_base;
    using std::dec;
    using std::hex;

    const ios_base::fmtflags save_flags = out.flags();
    out << dec << img.x() << ' ' << img.y() << ' ' << img.comp() << hex;
    for (size_t pixel_index = 0;
         pixel_index < img.x() * img.y();
         ++pixel_index) {
        out << ' ' << "0x" << img.pixel(pixel_index);
    }
    out.flags(save_flags);
    return out;
}

/**
 * @fn template <> void std::swap(openvrml::image & a, openvrml::image & b)
 *
 * @relatesalso openvrml::image
 *
 * @brief Swap the values of @p and @p b.
 *
 * Does not throw.
 *
 * @param[in,out] a
 * @param[in,out] b
 */
