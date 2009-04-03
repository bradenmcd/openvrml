// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML
//
// Copyright 1998  Chris Morley
// Copyright 2001, 2002, 2003, 2004, 2005, 2006, 2007  Braden McDaniel
// Copyright 2002  S. K. Bose
//
// This library is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation; either version 3 of the License, or (at your
// option) any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
// License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, see <http://www.gnu.org/licenses/>.
//

# include "text.h"
# include <private.h>
# include <openvrml/node_impl_util.h>
# include <openvrml/browser.h>
# include <openvrml/viewer.h>
# ifdef OPENVRML_ENABLE_RENDER_TEXT_NODE
#   include <ft2build.h>
#   include FT_FREETYPE_H
#   include FT_GLYPH_H
#   include FT_OUTLINE_H
#   ifdef _WIN32
#     include <windows.h>
#     include <strsafe.h>
#     include <shlobj.h>
#     undef interface
#   else
#     include <fontconfig/fontconfig.h>
extern "C" {
#     include <fontconfig/fcfreetype.h>
}
#   endif
# endif
# include <boost/array.hpp>
# include <boost/multi_index/detail/scope_guard.hpp>

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

namespace {

# ifdef OPENVRML_ENABLE_RENDER_TEXT_NODE
    typedef boost::mpl::if_c<sizeof (wchar_t) == 4,
                             wchar_t,
                             openvrml::int32>::type
        char32_t;
# endif

    class OPENVRML_LOCAL text_node :
        public openvrml::node_impl_util::abstract_node<text_node>,
        public openvrml::geometry_node {

        friend class openvrml_node_vrml97::text_metatype;

        class string_exposedfield : public exposedfield<openvrml::mfstring> {
        public:
            explicit string_exposedfield(text_node & node);
            string_exposedfield(const string_exposedfield & obj)
                OPENVRML_NOTHROW;
            virtual ~string_exposedfield() OPENVRML_NOTHROW;

        private:
            virtual std::auto_ptr<openvrml::field_value> do_clone() const
                OPENVRML_THROW1(std::bad_alloc);
            virtual void event_side_effect(const openvrml::mfstring & string,
                                           double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        class font_style_exposedfield : public exposedfield<openvrml::sfnode> {
        public:
            explicit font_style_exposedfield(text_node & node);
            font_style_exposedfield(const font_style_exposedfield & obj)
                OPENVRML_NOTHROW;
            virtual ~font_style_exposedfield() OPENVRML_NOTHROW;

        private:
            virtual std::auto_ptr<openvrml::field_value> do_clone() const
                OPENVRML_THROW1(std::bad_alloc);
            virtual void event_side_effect(const openvrml::sfnode & font_style,
                                           double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        class length_exposedfield : public exposedfield<openvrml::mffloat> {
        public:
            explicit length_exposedfield(text_node & node);
            length_exposedfield(const length_exposedfield & obj) OPENVRML_NOTHROW;
            virtual ~length_exposedfield() OPENVRML_NOTHROW;

        private:
            virtual std::auto_ptr<openvrml::field_value> do_clone() const
                OPENVRML_THROW1(std::bad_alloc);
            virtual void event_side_effect(const openvrml::mffloat & length,
                                           double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        class max_extent_exposedfield : public exposedfield<openvrml::sffloat> {
        public:
            explicit max_extent_exposedfield(text_node & node);
            max_extent_exposedfield(const max_extent_exposedfield & obj)
                OPENVRML_NOTHROW;
            virtual ~max_extent_exposedfield() OPENVRML_NOTHROW;

        private:
            virtual std::auto_ptr<openvrml::field_value> do_clone() const
                OPENVRML_THROW1(std::bad_alloc);
            virtual void event_side_effect(const openvrml::sffloat & max_extent,
                                           double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        string_exposedfield string_;
        font_style_exposedfield font_style_;
        length_exposedfield length_;
        max_extent_exposedfield max_extent_;
        openvrml::sfbool solid_;

        struct glyph_geometry {
            std::vector<openvrml::vec2f> coord;
            std::vector<openvrml::int32> coord_index;
            float advance_width;
            float advance_height;

            glyph_geometry(const std::vector<std::vector<openvrml::vec2f> > & contours,
                           float advance_width,
                           float advance_height)
                OPENVRML_THROW1(std::bad_alloc);
        };

        struct text_geometry {
            std::vector<openvrml::vec3f> coord;
            std::vector<openvrml::int32> coord_index;
            std::vector<openvrml::vec3f> normal;
            std::vector<openvrml::vec2f> tex_coord;
        };

# ifdef OPENVRML_ENABLE_RENDER_TEXT_NODE
        typedef std::vector<std::vector<char32_t> > ucs4_string_t;
        typedef std::map<FT_UInt, glyph_geometry> glyph_geometry_map_t;

        ucs4_string_t ucs4_string;
        FT_Face face;
        glyph_geometry_map_t glyph_geometry_map;
# endif
        text_geometry text_geometry_;

    public:
        text_node(const openvrml::node_type & type,
                  const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~text_node() OPENVRML_NOTHROW;

        virtual bool modified() const;

    private:
        virtual void do_render_geometry(openvrml::viewer & viewer,
                                        openvrml::rendering_context context);

        virtual void do_initialize(double timestamp)
            OPENVRML_THROW1(std::bad_alloc);
        virtual void do_shutdown(double timestamp) OPENVRML_NOTHROW;

        void update_ucs4() OPENVRML_THROW1(std::bad_alloc);
        void update_face() OPENVRML_THROW1(std::bad_alloc);
        void update_geometry() OPENVRML_THROW1(std::bad_alloc);
    };

    /**
     * @class text_node
     *
     * @brief Text node instances.
     */

    /**
     * @var class text_node::text_metatype
     *
     * @brief Class object for Text nodes.
     */

    /**
     * @internal
     *
     * @class text_node::string_exposedfield
     *
     * @brief string exposedField implementation.
     */

    /**
     * @brief Construct.
     *
     * @param node  text_node.
     */
    text_node::string_exposedfield::
    string_exposedfield(text_node & node):
        openvrml::node_event_listener(node),
        openvrml::event_emitter(static_cast<const openvrml::field_value &>(*this)),
        mfstring_listener(node),
        exposedfield<openvrml::mfstring>(node)
    {}

    /**
     * @brief Construct a copy.
     *
     * @param obj   instance to copy.
     */
    text_node::string_exposedfield::
    string_exposedfield(const string_exposedfield & obj) OPENVRML_NOTHROW:
        openvrml::event_listener(),
        openvrml::node_event_listener(obj.openvrml::node_event_listener::node()),
        openvrml::event_emitter(static_cast<const openvrml::field_value &>(*this)),
        mfstring_listener(obj.openvrml::node_event_listener::node()),
        exposedfield<openvrml::mfstring>(obj)
    {}

    /**
     * @brief Destroy.
     */
    text_node::string_exposedfield::
    ~string_exposedfield() OPENVRML_NOTHROW
    {}

    /**
     * @brief Polymorphically construct a copy.
     *
     * @return a copy of the instance.
     *
     * @exception std::bad_alloc    if memory allocation fails.
     */
    std::auto_ptr<openvrml::field_value>
    text_node::string_exposedfield::do_clone() const
        OPENVRML_THROW1(std::bad_alloc)
    {
        return std::auto_ptr<openvrml::field_value>(
            new string_exposedfield(*this));
    }

    /**
     * @brief Process event.
     *
     * @param string    text strings.
     * @param timestamp the current time.
     *
     * @exception std::bad_alloc    if memory allocation fails.
     */
    void
    text_node::string_exposedfield::
    event_side_effect(const openvrml::mfstring &, double)
        OPENVRML_THROW1(std::bad_alloc)
    {
        try {
            text_node & node =
                dynamic_cast<text_node &>(this->node_event_listener::node());
            node.update_ucs4();
            node.update_geometry();
        } catch (std::bad_cast & ex) {
            OPENVRML_PRINT_EXCEPTION_(ex);
        }
    }

    /**
     * @internal
     *
     * @class text_node::font_style_exposedfield
     *
     * @brief fontStyle exposedField implementation.
     */

    /**
     * @brief Construct.
     *
     * @param node  text_node.
     */
    text_node::font_style_exposedfield::
    font_style_exposedfield(text_node & node):
        openvrml::node_event_listener(node),
        openvrml::event_emitter(static_cast<const openvrml::field_value &>(*this)),
        sfnode_listener(node),
        exposedfield<openvrml::sfnode>(node)
    {}

    /**
     * @brief Construct a copy.
     *
     * @param obj   instance to copy.
     */
    text_node::font_style_exposedfield::
    font_style_exposedfield(const font_style_exposedfield & obj) OPENVRML_NOTHROW:
        openvrml::event_listener(),
        openvrml::node_event_listener(obj.openvrml::node_event_listener::node()),
        openvrml::event_emitter(static_cast<const openvrml::field_value &>(*this)),
        sfnode_listener(obj.openvrml::node_event_listener::node()),
        exposedfield<openvrml::sfnode>(obj)
    {}

    /**
     * @brief Destroy.
     */
    text_node::font_style_exposedfield::~font_style_exposedfield() OPENVRML_NOTHROW
    {}

    /**
     * @brief Polymorphically construct a copy.
     *
     * @return a copy of the instance.
     *
     * @exception std::bad_alloc    if memory allocation fails.
     */
    std::auto_ptr<openvrml::field_value>
    text_node::font_style_exposedfield::do_clone() const
        OPENVRML_THROW1(std::bad_alloc)
    {
        return std::auto_ptr<openvrml::field_value>(
            new font_style_exposedfield(*this));
    }

    /**
     * @brief Process event.
     *
     * @param font_style    text strings.
     * @param timestamp     the current time.
     *
     * @exception std::bad_alloc    if memory allocation fails.
     */
    void
    text_node::font_style_exposedfield::
    event_side_effect(const openvrml::sfnode &, double)
        OPENVRML_THROW1(std::bad_alloc)
    {
        try {
            text_node & node =
                dynamic_cast<text_node &>(this->node_event_listener::node());
            node.update_ucs4();
            node.update_geometry();
        } catch (std::bad_cast & ex) {
            OPENVRML_PRINT_EXCEPTION_(ex);
        }
    }

    /**
     * @internal
     *
     * @class text_node::length_exposedfield
     *
     * @brief length exposedField implementation.
     */

    /**
     * @brief Construct.
     *
     * @param node  text_node.
     */
    text_node::length_exposedfield::
    length_exposedfield(text_node & node):
        openvrml::node_event_listener(node),
        openvrml::event_emitter(static_cast<const openvrml::field_value &>(*this)),
        mffloat_listener(node),
        exposedfield<openvrml::mffloat>(node)
    {}

    /**
     * @brief Construct a copy.
     *
     * @param obj   instance to copy.
     */
    text_node::length_exposedfield::
    length_exposedfield(const length_exposedfield & obj) OPENVRML_NOTHROW:
        openvrml::event_listener(),
        openvrml::node_event_listener(obj.openvrml::node_event_listener::node()),
        openvrml::event_emitter(static_cast<const openvrml::field_value &>(*this)),
        mffloat_listener(obj.openvrml::node_event_listener::node()),
        exposedfield<openvrml::mffloat>(obj)
    {}

    /**
     * @brief Destroy.
     */
    text_node::length_exposedfield::~length_exposedfield()
        OPENVRML_NOTHROW
    {}

    /**
     * @brief Polymorphically construct a copy.
     *
     * @return a copy of the instance.
     *
     * @exception std::bad_alloc    if memory allocation fails.
     */
    std::auto_ptr<openvrml::field_value>
    text_node::length_exposedfield::do_clone() const
        OPENVRML_THROW1(std::bad_alloc)
    {
        return std::auto_ptr<openvrml::field_value>(
            new length_exposedfield(*this));
    }

    /**
     * @brief Process event.
     *
     * @param length    length of the text strings.
     * @param timestamp the current time.
     *
     * @exception std::bad_alloc    if memory allocation fails.
     */
    void
    text_node::length_exposedfield::
    event_side_effect(const openvrml::mffloat &, double)
        OPENVRML_THROW1(std::bad_alloc)
    {
        try {
            text_node & node =
                dynamic_cast<text_node &>(this->node_event_listener::node());
            node.update_geometry();
        } catch (std::bad_cast & ex) {
            OPENVRML_PRINT_EXCEPTION_(ex);
        }
    }

    /**
     * @internal
     *
     * @class text_node::max_extent_exposedfield
     *
     * @brief maxExtent exposedField implementation.
     */

    /**
     * @brief Construct.
     *
     * @param node  text_node.
     */
    text_node::max_extent_exposedfield::
    max_extent_exposedfield(text_node & node):
        openvrml::node_event_listener(node),
        openvrml::event_emitter(static_cast<const openvrml::field_value &>(*this)),
        sffloat_listener(node),
        exposedfield<openvrml::sffloat>(node)
    {}

    /**
     * @brief Construct a copy.
     *
     * @param obj   instance to copy.
     */
    text_node::max_extent_exposedfield::
    max_extent_exposedfield(const max_extent_exposedfield & obj) OPENVRML_NOTHROW:
        openvrml::event_listener(),
        openvrml::node_event_listener(obj.openvrml::node_event_listener::node()),
        openvrml::event_emitter(static_cast<const openvrml::field_value &>(*this)),
        sffloat_listener(obj.openvrml::node_event_listener::node()),
        exposedfield<openvrml::sffloat>(obj)
    {}

    /**
     * @brief Destroy.
     */
    text_node::max_extent_exposedfield::
    ~max_extent_exposedfield() OPENVRML_NOTHROW
    {}

    /**
     * @brief Polymorphically construct a copy.
     *
     * @return a copy of the instance.
     *
     * @exception std::bad_alloc    if memory allocation fails.
     */
    std::auto_ptr<openvrml::field_value>
    text_node::max_extent_exposedfield::do_clone() const
        OPENVRML_THROW1(std::bad_alloc)
    {
        return std::auto_ptr<openvrml::field_value>(
            new max_extent_exposedfield(*this));
    }

    /**
     * @brief Process event.
     *
     * @param max_extent    maximum extent of the text strings.
     * @param timestamp     the current time.
     *
     * @exception std::bad_alloc    if memory allocation fails.
     */
    void
    text_node::max_extent_exposedfield::
    event_side_effect(const openvrml::sffloat &, double)
        OPENVRML_THROW1(std::bad_alloc)
    {
        try {
            text_node & node =
                dynamic_cast<text_node &>(this->node_event_listener::node());
            node.update_geometry();
        } catch (std::bad_cast & ex) {
            OPENVRML_PRINT_EXCEPTION_(ex);
        }
    }

    /**
     * @var text_node::string_exposedfield text_node::string_
     *
     * @brief string exposedField.
     */

    /**
     * @var text_node::font_style_exposedfield text_node::font_style_
     *
     * @brief fontStyle exposedField.
     */

    /**
     * @var text_node::length_exposedfield text_node::length_
     *
     * @brief length exposedField.
     */

    /**
     * @var text_node::max_extent_exposedfield text_node::max_extent_
     *
     * @brief maxExtent exposedField.
     */

    /**
     * @internal
     *
     * @struct text_node::glyph_geometry
     *
     * @brief Used to hold the geometry of individual glyphs.
     */

    /**
     * @var std::vector<openvrml::vec2f> text_node::glyph_geometry::coord
     *
     * @brief Glyph coordinates.
     */

    /**
     * @var std::vector<openvrml::int32> text_node::glyph_geometry::coord_index
     *
     * @brief Glyph coordinate indices.
     */

    /**
     * @var float text_node::glyph_geometry::advance_width
     *
     * @brief The distance the pen should advance horizontally after drawing
     *        the glyph.
     */

    /**
     * @var float text_node::glyph_geometry::advance_height
     *
     * @brief The distance the pen should advance vertically after drawing the
     *      glyph.
     */

# ifdef OPENVRML_ENABLE_RENDER_TEXT_NODE

    /**
     * @internal
     *
     * @brief Determine whether three vertices are ordered counter-clockwise.
     *
     * Does not throw.
     *
     * @param p0 first vertex.
     * @param p1 second vertex.
     * @param p2 third vertex.
     *
     * @return 1 if the vertices are counter-clockwise, -1 if the vertices are
     *         clockwise, or 0 if the vertices are neither.
     */
    OPENVRML_LOCAL int ccw_(const openvrml::vec2f & p0,
                            const openvrml::vec2f & p1,
                            const openvrml::vec2f & p2)
        OPENVRML_NOTHROW
    {
        const float dx1 = p1.x() - p0.x();
        const float dy1 = p1.y() - p0.y();
        const float dx2 = p2.x() - p0.x();
        const float dy2 = p2.y() - p0.y();

        if (dx1 * dy2 > dy1 * dx2) { return 1; }
        if (dx1 * dy2 < dy1 * dx2) { return -1; }
        if ((dx1 * dx2 < 0) || (dy1 * dy2 < 0)) return -1;
        if ((dx1 * dx1 + dy1 * dy1) < (dx2 * dx2 + dy2 * dy2)) { return 1; }
        return 0;
    }

    /**
     * @internal
     *
     * @brief Determine whether two line segments intersect.
     *
     * Does not throw.
     *
     * @param l0p0 first point of the first line.
     * @param l0p1 second point of the first line.
     * @param l1p0 first point of the second line.
     * @param l1p1 second point of the second line.
     *
     * @return @c true if the line segments intersect; @c false otherwise.
     */
    OPENVRML_LOCAL bool intersect_(const openvrml::vec2f & l0p0,
                                   const openvrml::vec2f & l0p1,
                                   const openvrml::vec2f & l1p0,
                                   const openvrml::vec2f & l1p1)
        OPENVRML_NOTHROW
    {
        return ccw_(l0p0, l0p1, l1p0) * ccw_(l0p0, l0p1, l1p1) <= 0
            && ccw_(l1p0, l1p1, l0p0) * ccw_(l1p0, l1p1, l0p1) <= 0;
    }

    /**
     * @brief Determine whether a line segment intersects any line segments
     *        in a contour.
     *
     * Does not throw.
     *
     * @param v0      initial vertex of the line segment.
     * @param v1      final vertex of the line segment.
     * @param contour a contour (a series of line segments).
     *
     * @return @c true if the line segment defined by (@p v0, @p v1)
     *         intersects any line segment in @p contour; @c false otherwise.
     */
    OPENVRML_LOCAL bool
    intersects_segment_in_contour(const openvrml::vec2f & v0,
                                  const openvrml::vec2f & v1,
                                  const std::vector<openvrml::vec2f> & contour)
        OPENVRML_NOTHROW
    {
        for (size_t j = 0; j < contour.size() - 1; ++j) {
            using openvrml::vec2f;
            //
            // Endpoints of the segment to test for intersection.
            //
            const vec2f & contour_v0 = contour[j];
            const vec2f & contour_v1 = contour[j + 1];
            //
            // We don't care if the endpoints match (and the intersection
            // test will treat this as an intersection).
            //
            if (contour_v0 == v0 || contour_v0 == v1
                || contour_v1 == v0 || contour_v1 == v1) { continue; }
            if (intersect_(v0, v1, contour_v0, contour_v1)) { return true; }
        }
        return false;
    }

    /**
     * @internal
     *
     * @brief Get the exterior vertext that should be used to connect to the
     *      interior contour.
     *
     * Finds the first vertex in @p exteriorContour such that a line segment
     * from the interior contour vertex at @p interiorIndex through the
     * exterior contour vertex does not cross @p interiorContour.
     *
     * Does not throw.
     *
     * @param exterior_contour the exterior contour.
     * @param interior_contour the interior contour.
     * @param interior_index   the index of a vertex in @p interiorContour to
     *                         be used as the interior connecting vertex.
     *
     * @return the index of a vertex in @p exteriorContour that is usable as
     *         the exterior connecting vertex, or -1 if no such vertex is
     *         found.
     */
    OPENVRML_LOCAL long get_exterior_connecting_vertex_index_(
        const std::vector<openvrml::vec2f> & exterior_contour,
        const std::vector<const std::vector<openvrml::vec2f> *> &
        interior_contours,
        const openvrml::vec2f & interior_vertex)
        OPENVRML_NOTHROW
    {
        assert(exterior_contour.size() > 1);
        assert(!interior_contours.empty());

        using openvrml::vec2f;

        typedef std::vector<const std::vector<vec2f> *> interior_contours_type;

        for (size_t i = 0; i < exterior_contour.size(); ++i) {
            const vec2f & exterior_vertex = exterior_contour[i];
            bool intersects_interior = false;
            for (interior_contours_type::const_iterator interior_contour =
                     interior_contours.begin();
                 interior_contour != interior_contours.end()
                     && !intersects_interior;
                 ++interior_contour) {
                assert(*interior_contour);
                if (intersects_segment_in_contour(interior_vertex,
                                                  exterior_vertex,
                                                  **interior_contour)) {
                    intersects_interior = true;
                }
            }
            if (!intersects_interior
                && !intersects_segment_in_contour(interior_vertex,
                                                  exterior_vertex,
                                                  exterior_contour)) {
                return long(i);
            }
        }
        return -1;
    }

    OPENVRML_LOCAL bool
    inside_contour_(const std::vector<openvrml::vec2f> & contour,
                    const openvrml::vec2f & point)
        OPENVRML_NOTHROW
    {
        using openvrml::vec2f;

        bool result = false;
        const size_t nvert = contour.size();
        for (size_t i = 0, j = nvert - 1; i < nvert; j = i++) {
            const vec2f & vi = contour[i];
            const vec2f & vj = contour[j];
            if ((((vi.y() <= point.y()) && (point.y() < vj.y()))
                 || ((vj.y() <= point.y()) && (point.y() < vi.y())))
                && (point.x() < (vj.x() - vi.x())
                    * (point.y() - vi.y()) / (vj.y() - vi.y()) + vi.x())) {
                result = !result;
            }
        }
        return result;
    }

    enum contour_type_ { exterior_, interior_ };

    OPENVRML_LOCAL contour_type_
    get_type_(const std::vector<openvrml::vec2f> & contour,
              const std::vector<std::vector<openvrml::vec2f> > & contours)
        OPENVRML_NOTHROW
    {
        using std::vector;

        assert(!contour.empty());
        const openvrml::vec2f & vertex = contour[0];

        bool is_interior = false;
        for (vector<vector<openvrml::vec2f> >::const_iterator test_contour =
                 contours.begin();
             test_contour != contours.end();
             ++test_contour) {
            if (&*test_contour == &contour) { continue; }
            if (inside_contour_(*test_contour, vertex)) {
                is_interior = !is_interior;
            }
        }
        return is_interior
            ? interior_
            : exterior_;
    }

    struct OPENVRML_LOCAL polygon_ {
        const std::vector<openvrml::vec2f> * exterior;
        std::vector<const std::vector<openvrml::vec2f> *> interiors;
    };

    struct OPENVRML_LOCAL inside_ :
        std::binary_function<const std::vector<openvrml::vec2f> *,
                             const std::vector<openvrml::vec2f> *,
                             bool> {
        bool operator()(const std::vector<openvrml::vec2f> * const lhs,
                        const std::vector<openvrml::vec2f> * const rhs) const
            {
                assert(lhs);
                assert(rhs);
                assert(!lhs->empty());
                //
                // Assume contours don't intersect. So if one point on lhs is
                // inside rhs, then assume all of lhs is inside rhs.
                //
                return inside_contour_(*rhs, lhs->front());
            }
    };

    OPENVRML_LOCAL const std::vector<polygon_>
    get_polygons_(const std::vector<std::vector<openvrml::vec2f> > & contours)
        OPENVRML_THROW1(std::bad_alloc)
    {
        using std::vector;
        using openvrml::vec2f;
        typedef std::multiset<const vector<vec2f> *, inside_>
            segregated_contours;

        //
        // First, divide the contours into interior and exterior contours.
        //
        segregated_contours interiors, exteriors;
        for (vector<vector<vec2f> >::const_iterator contour = contours.begin();
             contour != contours.end();
             ++contour) {
            switch (get_type_(*contour, contours)) {
            case interior_:
                interiors.insert(&*contour);
                break;
            case exterior_:
                exteriors.insert(&*contour);
                break;
            default:
                assert(false);
            }
        }

        //
        // For each exterior, find its associated interiors and group them in
        // a Polygon_.
        //
        vector<polygon_> polygons;
        while (!exteriors.empty()) {
            polygon_ polygon;
            polygon.exterior = *exteriors.begin();
            segregated_contours::iterator interior = interiors.begin();
            while (interior != interiors.end()) {
                assert(!(*interior)->empty());
                if (inside_contour_(*polygon.exterior, (*interior)->front())) {
                    polygon.interiors.push_back(*interior);
                    segregated_contours::iterator next = interior;
                    ++next;
                    interiors.erase(interior);
                    interior = next;
                } else {
                    ++interior;
                }
            }
            polygons.push_back(polygon);
            exteriors.erase(exteriors.begin());
        }
        return polygons;
    }

    OPENVRML_LOCAL long
    get_vertex_index_(const std::vector<openvrml::vec2f> & vertices,
                      const openvrml::vec2f & vertex)
        OPENVRML_NOTHROW
    {
        for (size_t i = 0; i < vertices.size(); ++i) {
            if (vertices[i] == vertex) { return long(i); }
        }
        return -1;
    }

    typedef std::multimap<const openvrml::vec2f *,
                          const std::vector<openvrml::vec2f> *>
        connection_map_t;

    //
    // Fill connection_map. For each interior contour, find the exterior
    // vertex that is closest to the first vertex in the interior contour, and
    // the put the pair in the map.
    //
    OPENVRML_LOCAL std::auto_ptr<connection_map_t>
    get_connection_map(const polygon_ & p)
    {
        using std::vector;
        using openvrml::vec2f;
        std::auto_ptr<connection_map_t> connection_map(new connection_map_t);
        for (vector<const vector<vec2f> *>::const_iterator interior =
                 p.interiors.begin();
             interior != p.interiors.end();
             ++interior) {
            assert(*interior);
            assert(!(*interior)->empty());
            long exterior_vertex_index =
                get_exterior_connecting_vertex_index_(*p.exterior,
                                                      p.interiors,
                                                      (*interior)->front());
            assert(exterior_vertex_index > -1);
            const vec2f * const exterior_vertex =
                &(*p.exterior)[exterior_vertex_index];
            assert(exterior_vertex);
            const connection_map_t::value_type value(exterior_vertex,
                                                     *interior);
            connection_map->insert(value);
        }
        return connection_map;
    }

    struct OPENVRML_LOCAL draw_glyph_polygon {
        draw_glyph_polygon(std::vector<openvrml::vec2f> & coord,
                           std::vector<openvrml::int32> & coord_index):
            coord(coord),
            coord_index(coord_index)
        {}

        void operator()(const polygon_ & p) const
        {
            using openvrml::vec2f;

            //
            // connectionMap is keyed on a pointer to a vertex on the exterior
            // contour, and maps to a pointer to the interior contour whose
            // first vertex is closest to the exterior vertex.
            //
            std::auto_ptr<connection_map_t> connection_map(get_connection_map(p));

            assert(!p.exterior->empty());
            for (size_t i = 0; i < p.exterior->size(); ++i) {
                const vec2f & exterior_vertex = (*p.exterior)[i];
                long exterior_index = get_vertex_index_(this->coord, exterior_vertex);
                if (exterior_index > -1) {
                    this->coord_index.push_back(exterior_index);
                } else {
                    this->coord.push_back(exterior_vertex);
                    assert(!this->coord.empty());
                    exterior_index = long(this->coord.size() - 1);
                    coord_index.push_back(exterior_index);
                }
                connection_map_t::iterator pos;
                while ((pos = connection_map->find(&exterior_vertex))
                       != connection_map->end()) {
                    for (int i = int(pos->second->size() - 1); i > -1; --i) {
                        const vec2f & interior_vertex = (*pos->second)[i];
                        const long interior_index =
                            get_vertex_index_(this->coord, interior_vertex);
                        if (interior_index > -1) {
                            this->coord_index.push_back(interior_index);
                        } else {
                            using openvrml::int32;
                            this->coord.push_back(interior_vertex);
                            assert(!this->coord.empty());
                            this->coord_index.push_back(int32(this->coord.size() - 1));
                        }
                    }
                    this->coord_index.push_back(exterior_index);
                    connection_map->erase(pos);
                }
            }
            assert(connection_map->empty());
            this->coord_index.push_back(-1);
        }
    private:
        std::vector<openvrml::vec2f> & coord;
        std::vector<openvrml::int32> & coord_index;
    };
# endif // OPENVRML_ENABLE_RENDER_TEXT_NODE

    /**
     * @brief Construct from a set of contours.
     *
     * @param contours          a vector of closed contours that make up the
     *                          glyph's outline.
     * @param advance_width     the distance the pen should advance
     *                          horizontally after drawing the glyph.
     * @param advance_height    the distance the pen should advance vertically
     *                          after drawing the glyph.
     *
     * @exception std::bad_alloc    if memory allocation fails.
     */
    text_node::glyph_geometry::
    glyph_geometry(const std::vector<std::vector<openvrml::vec2f> > & contours,
                   const float advance_width,
                   const float advance_height)
        OPENVRML_THROW1(std::bad_alloc):
        advance_width(advance_width),
        advance_height(advance_height)
    {
# ifdef OPENVRML_ENABLE_RENDER_TEXT_NODE
        using std::vector;

        const vector<polygon_> & polygons = get_polygons_(contours);
        std::for_each(polygons.begin(), polygons.end(),
                      draw_glyph_polygon(this->coord, this->coord_index));
# endif // OPENVRML_ENABLE_RENDER_TEXT_NODE
    }

    /**
     * @internal
     *
     * @struct text_node::text_geometry
     *
     * @brief Holds the text geometry.
     */

    /**
     * @var std::vector<openvrml::vec3f> text_node::text_geometry::coord
     *
     * @brief Text geometry coordinates.
     */

    /**
     * @var std::vector<openvrml::int32> text_node::text_geometry::coord_index
     *
     * @brief Text geometry coordinate indices.
     */

    /**
     * @var std::vector<openvrml::vec3f> text_node::text_geometry::normal
     *
     * @brief Text geometry normals.
     */

    /**
     * @var std::vector<openvrml::vec2f> text_node::text_geometry::tex_coord
     *
     * @brief Text geometry texture coordinates.
     */

    /**
     * @typedef text_node::ucs4_string_t
     *
     * @brief A vector of FcChar32 vectors.
     */

    /**
     * @typedef text_node::glyph_geometry_map_t
     *
     * @brief Maps FT_UInts to glyph_geometry.
     *
     * @see http://freetype.org/freetype2/docs/reference/ft2-basic_types.html#FT_UInt
     */

    /**
     * @var text_node::ucs4_string_t text_node::ucs4_string
     *
     * @brief UCS-4 equivalent of the (UTF-8) data in @a string.
     */

    /**
     * @var FT_Face text_node::face
     *
     * @brief Handle to the font face.
     *
     * @see http://freetype.org/freetype2/docs/reference/ft2-base_interface.html#FT_Face
     */

    /**
     * @var text_node::glyph_geometry_map_t text_node::glyph_geometry_map
     *
     * @brief Map of glyph indices to glyph_geometry.
     *
     * glyph_geometry instances are created as needed as new glyphs are
     * encountered. Once they are created, they are cached in the
     * glyph_geometry_map for rapid retrieval the next time the glyph is
     * encountered.
     */

    /**
     * @var text_node::text_geometry text_node::text_geometry_
     *
     * @brief The text geometry.
     */

    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with the instance.
     * @param scope the scope that the new node will belong to.
     */
    text_node::
    text_node(const openvrml::node_type & type,
              const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        openvrml::node_impl_util::abstract_node<text_node>(type, scope),
        geometry_node(type, scope),
        string_(*this),
        font_style_(*this),
        length_(*this),
        max_extent_(*this),
        solid_(true)
# ifdef OPENVRML_ENABLE_RENDER_TEXT_NODE
        ,face(0)
# endif
    {}

    /**
     * @brief Destroy.
     */
    text_node::~text_node() OPENVRML_NOTHROW
    {}

    /**
     * @brief Determine whether the node has been modified.
     *
     * @return @c true if the node or one of its children has been modified,
     *      @c false otherwise.
     */
    bool text_node::modified() const
    {
        return this->node::modified()
            || (this->font_style_.sfnode::value()
                && this->font_style_.sfnode::value()->modified());
    }

    /**
     * @brief Insert this geometry into @p v's display list.
     *
     * @param v         a viewer.
     * @param context   the rendering context.
     */
    void text_node::do_render_geometry(openvrml::viewer & v,
                                       openvrml::rendering_context)
    {
        using openvrml::int32;
        v.insert_shell(*this,
                       openvrml::viewer::mask_ccw,
                       this->text_geometry_.coord,
                       this->text_geometry_.coord_index,
                       std::vector<openvrml::color>(), // color
                       std::vector<int32>(), // colorIndex
                       this->text_geometry_.normal,
                       std::vector<int32>(), // normalIndex
                       this->text_geometry_.tex_coord,
                       std::vector<int32>()); // texCoordIndex
        if (this->font_style_.sfnode::value()) {
            this->font_style_.sfnode::value()->modified(false);
        }
    }

    /**
     * @brief Initialize.
     *
     * @param timestamp the current time.
     *
     * @exception std::bad_alloc    if memory allocation fails.
     */
    void text_node::do_initialize(double)
        OPENVRML_THROW1(std::bad_alloc)
    {
        this->update_ucs4();
        this->update_face();
        this->update_geometry();
    }

    /**
     * @brief Shut down.
     *
     * @param timestamp the current time.
     */
    void text_node::do_shutdown(double)
        OPENVRML_NOTHROW
    {
# ifdef OPENVRML_ENABLE_RENDER_TEXT_NODE
        if (this->face) {
            FT_Error ftError = FT_Done_Face(this->face);
            assert(ftError == FT_Err_Ok); // Surely this can't fail.
        }
# endif // OPENVRML_ENABLE_RENDER_TEXT_NODE
    }

# ifdef OPENVRML_ENABLE_RENDER_TEXT_NODE

    OPENVRML_LOCAL ptrdiff_t utf8_to_ucs4_(const unsigned char * src_orig,
                                           char32_t & dst,
                                           size_t len)
    {
        const unsigned char *src = src_orig;

        if (len == 0) { return 0; }

        unsigned char s = *src++;
        len--;

        char32_t result;
        size_t extra;
        if (!(s & 0x80)) {
            result = s;
            extra = 0;
        } else if (!(s & 0x40)) {
            return -1;
        } else if (!(s & 0x20)) {
            result = s & 0x1f;
            extra = 1;
        } else if (!(s & 0x10)) {
            result = s & 0xf;
            extra = 2;
        } else if (!(s & 0x08)) {
            result = s & 0x07;
            extra = 3;
        } else if (!(s & 0x04)) {
            result = s & 0x03;
            extra = 4;
        } else if ( ! (s & 0x02)) {
            result = s & 0x01;
            extra = 5;
        } else {
            return -1;
        }
        if (extra > len) { return -1; }

        while (extra--) {
            result <<= 6;
            s = *src++;

            if ((s & 0xc0) != 0x80) { return -1; }

            result |= s & 0x3f;
        }
        dst = result;
        return src - src_orig;
    }

    OPENVRML_LOCAL bool utf8_len_(const unsigned char * utf8_str,
                                  size_t len,
                                  size_t & chars,
                                  size_t & max_char_width)
    {

        int n = 0;
        char32_t max = 0;
        while (len) {
            char32_t c;
            const ptrdiff_t clen = utf8_to_ucs4_(utf8_str, c, len);
            if (clen <= 0) {
                // Malformed UTF8 string.
                return false;
            }
            if (c > max) {
                max = c;
            }
            utf8_str += clen;
            len -= clen;
            n++;
        }
        chars = n;
        if (max >= 0x10000) {
            max_char_width = 4;
        } else if (max > 0x100) {
            max_char_width = 2;
        } else {
            max_char_width = 1;
        }
        return true;
    }
# endif // ifdef OPENVRML_ENABLE_RENDER_TEXT_NODE

    /**
     * @brief Called when @a string changes to update the UCS-4 text.
     *
     * @exception std::bad_alloc    if memory allocation fails.
     */
    void text_node::update_ucs4() OPENVRML_THROW1(std::bad_alloc)
    {
# ifdef OPENVRML_ENABLE_RENDER_TEXT_NODE
        this->ucs4_string.clear();
        this->ucs4_string.resize(this->string_.mfstring::value().size());

        for (size_t i = 0; i < this->string_.mfstring::value().size(); ++i) {
            using std::string;
            using std::vector;

            const string & element = this->string_.mfstring::value()[i];

            vector<char32_t> & ucs4Element = this->ucs4_string[i];

            //
            // First, we need to convert the characters from UTF-8 to UCS-4.
            //
            vector<unsigned char> utf8String(element.begin(), element.end());
            size_t chars = 0, max_char_width = 0;
            const bool well_formed = utf8_len_(&utf8String[0],
                                               utf8String.size(),
                                               chars,
                                               max_char_width);
            if (well_formed) {
                ucs4Element.resize(chars);
                vector<unsigned char>::iterator utf8interface =
                    utf8String.begin();
                vector<char32_t>::iterator ucs4interface = ucs4Element.begin();
                while (utf8interface != utf8String.end()) {
                    const ptrdiff_t utf8bytes =
                        utf8_to_ucs4_(&*utf8interface,
                                      *ucs4interface,
                                      utf8String.end() - utf8interface);
                    utf8interface += utf8bytes;
                    ucs4interface++;
                }
            }
        }
# endif // OPENVRML_ENABLE_RENDER_TEXT_NODE
    }

# ifdef OPENVRML_ENABLE_RENDER_TEXT_NODE

    //
    // unsigned_char_traits is a model of the standard library Character Traits
    // concept.
    //
    struct OPENVRML_LOCAL unsigned_char_traits {
        typedef unsigned char char_type;
        typedef int int_type;
        typedef std::streampos pos_type;
        typedef std::streamoff off_type;
        typedef mbstate_t state_type;

        static void assign(char_type & c1, const char_type & c2);
        static bool eq(const char_type & c1, const char_type & c2);
        static bool lt(const char_type & c1, const char_type & c2);
        static int compare(const char_type * s1, const char_type * s2,
                           size_t n);
        static size_t length(const char_type * s);
        static const char_type * find(const char_type * s, size_t n,
                                      const char_type & a);
        static char_type * move(char_type * s1, const char_type * s2,
                                size_t n);
        static char_type * copy(char_type * s1, const char_type * s2,
                                size_t n);
        static char_type * assign(char_type * s, size_t n, char_type a);
        static int_type eof();
        static int_type not_eof(const int_type & c);
        static char_type to_char_type(const int_type & e);
        static int_type to_int_type(const char_type & c);
        static bool eq_int_type(const int_type & e1, const int_type & e2);
    };

    inline void unsigned_char_traits::assign(char_type & c1,
                                             const char_type & c2)
    {
        c1 = c2;
    }

    inline bool unsigned_char_traits::eq(const char_type & c1,
                                         const char_type & c2)
    {
        return c1 == c2;
    }

    inline bool unsigned_char_traits::lt(const char_type & c1,
                                         const char_type & c2)
    {
        return c1 < c2;
    }

    inline int unsigned_char_traits::compare(const char_type * s1,
                                             const char_type * s2,
                                             size_t n)
    {
        for (size_t i = 0; i < n; ++i) {
            if (!eq(s1[i], s2[i])) { return lt(s1[i], s2[i]) ? -1 : 1; }
        }
        return 0;
    }

    inline size_t unsigned_char_traits::length(const char_type * s)
    {
        const char_type * p = s;
        while (*p) { ++p; }
        return (p - s);
    }

    inline unsigned_char_traits::char_type *
    unsigned_char_traits::move(char_type * s1, const char_type * s2, size_t n)
    {
        return reinterpret_cast<char_type *>(
            memmove(s1, s2, n * sizeof(char_type)));
    }

    inline const unsigned_char_traits::char_type *
    unsigned_char_traits::find(const char_type * s,
                               size_t n,
                               const char_type & a)
    {
        for (const char_type * p = s; size_t(p - s) < n; ++p) {
            if (*p == a) { return p; }
        }
        return 0;
    }

    inline unsigned_char_traits::char_type *
    unsigned_char_traits::copy(char_type * s1, const char_type * s2, size_t n)
    {
        return reinterpret_cast<char_type *>(
            memcpy(s1, s2, n * sizeof(char_type)));
    }

    inline unsigned_char_traits::char_type *
    unsigned_char_traits::assign(char_type * s, size_t n, char_type a)
    {
        for (char_type * p = s; p < s + n; ++p) { assign(*p, a); }
        return s;
    }

    inline unsigned_char_traits::int_type unsigned_char_traits::eof()
    {
        return static_cast<int_type>(-1);
    }

    inline unsigned_char_traits::int_type
    unsigned_char_traits::not_eof(const int_type & c)
    {
        return eq_int_type(c, eof()) ? int_type(0) : c;
    }

    inline unsigned_char_traits::char_type
    unsigned_char_traits::to_char_type(const int_type & e)
    {
        return char_type(e);
    }

    inline unsigned_char_traits::int_type
    unsigned_char_traits::to_int_type(const char_type & c)
    {
        return int_type(c);
    }

    inline bool unsigned_char_traits::eq_int_type(const int_type & e1,
                                            const int_type & e2)
    {
        return e1 == e2;
    }

#   ifndef _WIN32
    const char * const fcResultMessage[] = { "match",
                                             "no match",
                                             "type mismatch",
                                             "no id" };

    //
    // FontconfigError and FreeTypeError are never thrown out of the library.
    //

    class OPENVRML_LOCAL FontconfigError : public std::runtime_error {
    public:
        explicit FontconfigError(const FcResult result):
            std::runtime_error(fcResultMessage[result])
        {}

        virtual ~FontconfigError() OPENVRML_NOTHROW
        {}
    };
#   endif

    class OPENVRML_LOCAL FreeTypeError : public std::runtime_error {
    public:
        //
        // The normal build of FreeType doesn't include a means of mapping
        // error codes to human-readable strings.  There's a means of
        // letting client apps do this by defining some macros, but that's
        // too much trouble for now.
        //
        explicit FreeTypeError(FT_Error):
            std::runtime_error("FreeType error")
        {}

        virtual ~FreeTypeError() OPENVRML_NOTHROW
        {}
    };

    typedef std::basic_string<unsigned char, unsigned_char_traits>
        unsigned_char_string;

    OPENVRML_LOCAL void
    get_font_filename(const std::vector<std::string> & family,
                      const std::string & style,
                      const unsigned_char_string & language,
                      std::vector<char> & filename,
                      FT_Long & face_index)
    {
        using std::vector;
# ifdef _WIN32
        using namespace boost::multi_index::detail;  // for scope_guard

        LOGFONT lf;
        lf.lfHeight =         0;
        lf.lfWidth =          0;
        lf.lfEscapement =     0;
        lf.lfOrientation =    0;
        lf.lfWeight =         FW_MEDIUM;
        lf.lfItalic =         FALSE;
        lf.lfUnderline =      FALSE;
        lf.lfStrikeOut =      FALSE;
        lf.lfCharSet =        DEFAULT_CHARSET;
        lf.lfOutPrecision =   OUT_TT_ONLY_PRECIS;
        lf.lfClipPrecision =  CLIP_DEFAULT_PRECIS;
        lf.lfQuality =        DEFAULT_QUALITY;
        lf.lfPitchAndFamily = VARIABLE_PITCH | FF_ROMAN;

        HDC hdc = CreateCompatibleDC(0);
        scope_guard hdc_guard = make_guard(&DeleteDC, hdc);
        HFONT hfont = CreateFontIndirect(&lf);
        SelectObject(hdc, hfont);
        TCHAR faceName[256] = {};
        GetTextFace(hdc, sizeof faceName / sizeof (TCHAR), faceName);
        const int faceNameLen = lstrlen(faceName);

        //
        // Get the fonts folder.
        //
        TCHAR fontsPath[MAX_PATH];
        HRESULT status =
            SHGetFolderPath(NULL, CSIDL_FONTS, NULL, SHGFP_TYPE_CURRENT,
                            fontsPath);
        if (FAILED(status)) { /* bail */ }

        //
        // Enumerate the fonts in the registry and pick one that matches.
        //
        HKEY fontsKey;
        LONG result =
            RegOpenKeyEx(
                HKEY_LOCAL_MACHINE,
                "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Fonts",
                0,
                KEY_READ,
                &fontsKey);
        if (result != ERROR_SUCCESS) { /* bail */ }
        scope_guard fontsKey_guard = make_guard(&RegCloseKey, fontsKey);

        DWORD maxValueNameLen, maxValueLen;
        result = RegQueryInfoKey(fontsKey,
                                 NULL,  // lpClass
                                 NULL,  // lpcClass
                                 NULL,  // lpReserved
                                 NULL,  // lpcSubKeys
                                 NULL,  // lpcMaxSubKeyLen
                                 NULL,  // lpcMaxClassLen
                                 NULL,  // lpcValues
                                 &maxValueNameLen,
                                 &maxValueLen,
                                 NULL,  // lpcbSecurityDescriptor
                                 NULL); // lpftLastWriteTime

        DWORD index = 0;
        vector<TCHAR> valueName(maxValueNameLen + 1);
        DWORD type;
        vector<BYTE> data(maxValueLen);
        TCHAR fontPath[MAX_PATH] = {};
        result = ERROR_SUCCESS;
        while (result != ERROR_NO_MORE_ITEMS) {
            DWORD dataLength = DWORD(data.size());
            DWORD valueNameLength = DWORD(valueName.size());
            result = RegEnumValue(fontsKey,
                                  index,
                                  &valueName.front(),
                                  &valueNameLength,
                                  NULL,
                                  &type,
                                  &data.front(),
                                  &dataLength);
            if (result == ERROR_MORE_DATA) {
                data.resize(dataLength);
                continue;
            }
            if (result == ERROR_SUCCESS) {
                if (DWORD(faceNameLen + 1) <= valueNameLength
                    && std::equal(faceName, faceName + faceNameLen,
                                  valueName.begin())) {
                    HRESULT strcat_result = StringCchCat(fontPath,
                                                         MAX_PATH,
                                                         fontsPath);
                    assert(SUCCEEDED(strcat_result));
                    strcat_result = StringCchCat(fontPath, MAX_PATH, "\\");
                    assert(SUCCEEDED(strcat_result));
                    strcat_result =
                        StringCchCat(fontPath,
                                     MAX_PATH,
                                     reinterpret_cast<STRSAFE_LPCSTR>(
                                         &data.front()));
                    assert(SUCCEEDED(strcat_result));
                    break;
                }
                ++index;
            }
        }

        const size_t fontPathLen = lstrlen(fontPath);
        assert(fontPathLen != 0);
        filename.assign(fontPath, fontPath + fontPathLen + 1);
        face_index = 0;
# else
        using std::string;
        using namespace boost::multi_index::detail;  // for scope_guard

        string fontName;
        //
        // Set the family.
        //
        for (size_t i = 0; i < family.size(); ++i) {
            const string & element = family[i];
            if (element == "SERIF") {
                fontName += "serif";
            } else if (element == "SANS") {
                fontName += "sans";
            } else if (element == "TYPEWRITER") {
                fontName += "monospace";
            } else {
                fontName += element;
            }
            if (i + 1 < family.size()) { fontName += ", "; }
        }

        //
        // Set the weight.
        //
        if (style.find("BOLD") != string::npos) {
            fontName += ":bold";
        }

        //
        // Set the slant.
        //
        if (style.find("ITALIC") != string::npos) {
            fontName += ":italic";
        }

        //
        // For now, at least, we only want outline fonts.
        //
        fontName += ":outline=True";

        FcPattern * const initialPattern =
            FcNameParse(unsigned_char_string(fontName.begin(),
                                             fontName.end()).c_str());
        if (!initialPattern) { throw std::bad_alloc(); }
        scope_guard initialPattern_guard =
            make_guard(&FcPatternDestroy, initialPattern);
        boost::ignore_unused_variable_warning(initialPattern_guard);

        //
        // Set the language.
        //
        if (!language.empty()) {
            FcPatternAddString(initialPattern,
                               FC_LANG,
                               language.c_str());
        }

        FcConfigSubstitute(0, initialPattern, FcMatchPattern);
        FcDefaultSubstitute(initialPattern);

        FcResult result = FcResultMatch;
        FcPattern * const matchedPattern =
            FcFontMatch(0, initialPattern, &result);
        if (result != FcResultMatch) { throw FontconfigError(result); }
        assert(matchedPattern);
        scope_guard matchedPattern_guard =
            make_guard(&FcPatternDestroy, matchedPattern);
        boost::ignore_unused_variable_warning(matchedPattern_guard);

        FcChar8 * filename_c_str = 0;
        result = FcPatternGetString(matchedPattern,
                                    FC_FILE,
                                    0,
                                    &filename_c_str);
        if (result != FcResultMatch) { throw FontconfigError(result); }

        size_t filenameLen = 0;
        for (; filename_c_str[filenameLen]; ++filenameLen) {}

        filename.assign(filename_c_str, filename_c_str + filenameLen + 1);

        int face_index_int = 0;
        result = FcPatternGetInteger(matchedPattern, FC_INDEX, 0,
                                     &face_index_int);
        if (result != FcResultMatch) { throw FontconfigError(result); }
        face_index = FT_Long(face_index_int);
# endif
    }

# endif // OPENVRML_ENABLE_RENDER_TEXT_NODE

    /**
     * @brief Called when @a fontStyle changes to update the font face.
     *
     * @exception std::bad_alloc    if memory allocation fails.
     */
    void text_node::update_face() OPENVRML_THROW1(std::bad_alloc)
    {
# ifdef OPENVRML_ENABLE_RENDER_TEXT_NODE
        using std::string;
        using openvrml::node_cast;
        using openvrml_node_vrml97::text_metatype;

        unsigned_char_string language;

        std::vector<string> family;
        family.push_back("SERIF");

        string style;

        openvrml::font_style_node * const fontStyle =
            node_cast<openvrml::font_style_node *>(
                this->font_style_.sfnode::value().get());
        if (fontStyle) {
            if (!fontStyle->family().empty()) {
                family = fontStyle->family();
                style = fontStyle->style();
                language.assign(fontStyle->language().begin(),
                                fontStyle->language().end());
            }
        }

        try {
            std::vector<char> ftFilename;
            FT_Long face_index;
            get_font_filename(family, style, language, ftFilename, face_index);

            text_metatype & nodeClass =
                const_cast<text_metatype &>(
                    static_cast<const text_metatype &>(
                        this->type().metatype()));

            FT_Face newFace = 0;
            FT_Error ftError = FT_Err_Ok;
            ftError = FT_New_Face(nodeClass.freeTypeLibrary,
                                  &ftFilename[0], FT_Long(face_index),
                                  &newFace);
            if (ftError) { throw FreeTypeError(ftError); }

            if (this->face) {
                ftError = FT_Done_Face(this->face);
                assert(ftError == FT_Err_Ok); // Surely this can't fail.
            }

            this->face = newFace;
            this->glyph_geometry_map.clear();
        } catch (std::runtime_error & ex) {
            OPENVRML_PRINT_EXCEPTION_(ex);
        }
# endif // OPENVRML_ENABLE_RENDER_TEXT_NODE
    }

# ifdef OPENVRML_ENABLE_RENDER_TEXT_NODE
    struct OPENVRML_LOCAL GlyphContours_ {
        const float scale;
        std::vector<std::vector<openvrml::vec2f> > contours;

        explicit GlyphContours_(float scale);
    };

    GlyphContours_::GlyphContours_(const float scale):
        scale(scale)
    {}

    const float stepSize_ = 0.2f;

    extern "C" int
    moveTo_(OPENVRML_FT_CONST FT_Vector * const to,
            void * const user)
    {
        using std::vector;
        using openvrml::vec2f;
        using openvrml::make_vec2f;

        assert(user);
        GlyphContours_ & c = *static_cast<GlyphContours_ *>(user);
        try {
            c.contours.push_back(vector<vec2f>(1));
        } catch (std::bad_alloc & ex) {
            OPENVRML_PRINT_EXCEPTION_(ex);
            return FT_Err_Out_Of_Memory;
        }
        const vec2f vertex = make_vec2f(to->x * c.scale, to->y * c.scale);
        c.contours.back().front() = vertex;
        return 0;
    }

    extern "C" int
    lineTo_(OPENVRML_FT_CONST FT_Vector * const to,
            void * const user)
    {
        using openvrml::make_vec2f;

        assert(user);
        GlyphContours_ & c = *static_cast<GlyphContours_ *>(user);
        const openvrml::vec2f vertex = make_vec2f(to->x * c.scale,
                                                  to->y * c.scale);
        try {
            c.contours.back().push_back(vertex);
        } catch (std::bad_alloc & ex) {
            OPENVRML_PRINT_EXCEPTION_(ex);
            return FT_Err_Out_Of_Memory;
        }
        return 0;
    }

    /**
     * @brief de Casteljau's algorithm.
     *
     * This is a nice recursive algorithm defined by de-Casteljau which
     * calculates for a given control polygon the point that lies on the bezier
     * curve for any value of t, and can be used to evaluate and draw the
     * Bezier spline without using the Bernstein polynomials.
     *
     * The algorithm advances by creating in each step a polygons of degree one
     * less than the one created in the previous step until there is only one
     * point left, which is the point on the curve. The polygon vertices for
     * each step are defined by linear interpolation of two consecutive
     * vertices of the polygon from the previous step with a value of t (the
     * parameter):
     *
     * @param buffer    an array including the control points for the curve in
     *                  the first @p npoints elements. The total size of the
     *                  array must be @p npoints * @p npoints. The remaining
     *                  elements of the array will be used by the algorithm to
     *                  store temporary values.
     * @param npoints   the number of control points.
     * @param contour   the points on the curve are added to this array.
     *
     * @exception std::bad_alloc    if memory allocation fails.
     */
    OPENVRML_LOCAL void evaluateCurve_(openvrml::vec2f * const buffer,
                                       const size_t npoints,
                                       std::vector<openvrml::vec2f> & contour)
        OPENVRML_THROW1(std::bad_alloc)
    {
        for (size_t i = 1; i <= (1 / stepSize_); i++){
            const float t = i * stepSize_; // Parametric points 0 <= t <= 1
            for (size_t j = 1; j < npoints; j++) {
                for (size_t k = 0; k < (npoints - j); k++) {
                    openvrml::vec2f & element = buffer[j * npoints + k];
                    element.x((1 - t) * buffer[(j - 1) * npoints + k][0]
                              + t * buffer[(j - 1) * npoints + k + 1][0]);
                    element.y((1 - t) * buffer[(j - 1) * npoints + k][1]
                              + t * buffer[(j - 1) * npoints + k + 1][1]);
                }
            }
            //
            // Specify next vertex to be included on curve
            //
            contour.push_back(buffer[(npoints - 1) * npoints]); // throws std::bad_alloc
        }
    }

    extern "C" int
    conicTo_(OPENVRML_FT_CONST FT_Vector * const control,
             OPENVRML_FT_CONST FT_Vector * const to,
             void * const user)
    {
        using std::vector;
        using openvrml::vec2f;
        using openvrml::make_vec2f;

        assert(control);
        assert(to);
        assert(user);

        GlyphContours_ & c = *static_cast<GlyphContours_ *>(user);

        assert(!c.contours.empty());
        vector<vec2f> & contour = c.contours.back();
        const vec2f & lastVertex = contour[contour.size() - 1];

        assert(!contour.empty());
        const size_t npoints = 3;
        vec2f buffer[npoints * npoints] = {
            make_vec2f(lastVertex[0], lastVertex[1]),
            make_vec2f(control->x * c.scale, control->y * c.scale),
            make_vec2f(to->x * c.scale, to->y * c.scale)
        };

        try {
            evaluateCurve_(buffer, npoints, contour);
        } catch (std::bad_alloc & ex) {
            OPENVRML_PRINT_EXCEPTION_(ex);
            return FT_Err_Out_Of_Memory;
        }
        return 0;
    }

    extern "C" int
    cubicTo_(OPENVRML_FT_CONST FT_Vector * const control1,
             OPENVRML_FT_CONST FT_Vector * const control2,
             OPENVRML_FT_CONST FT_Vector * const to,
             void * const user)
    {
        using std::vector;
        using openvrml::vec2f;
        using openvrml::make_vec2f;

        assert(control1);
        assert(control2);
        assert(to);
        assert(user);

        GlyphContours_ & c = *static_cast<GlyphContours_ *>(user);

        assert(!c.contours.empty());
        vector<vec2f> & contour = c.contours.back();
        assert(!contour.empty());
        const vec2f & lastVertex = contour.back();

        static const size_t npoints = 4;
        vec2f buffer[npoints * npoints] = {
            make_vec2f(lastVertex[0], lastVertex[1]),
            make_vec2f(control1->x * c.scale, control1->y * c.scale),
            make_vec2f(control2->x * c.scale, control2->y * c.scale),
            make_vec2f(to->x * c.scale, to->y * c.scale)
        };

        try {
            evaluateCurve_(buffer, npoints, contour);
        } catch (std::bad_alloc & ex) {
            OPENVRML_PRINT_EXCEPTION_(ex);
            return FT_Err_Out_Of_Memory;
        }
        return 0;
    }
# endif // OPENVRML_ENABLE_RENDER_TEXT_NODE

    /**
     * @brief Called to update @a text_geometry.
     *
     * @exception std::bad_alloc    if memory allocation fails.
     */
    void text_node::update_geometry() OPENVRML_THROW1(std::bad_alloc)
    {
# ifdef OPENVRML_ENABLE_RENDER_TEXT_NODE
        using std::pair;
        using std::string;
        using std::vector;
        using openvrml::node_cast;
        using openvrml::vec2f;
        using openvrml::make_vec2f;
        using openvrml::vec3f;
        using openvrml::make_vec3f;

        bool horizontal = true;
        string justify[2] = { "BEGIN", "FIRST" };
        bool leftToRight = true;
        bool topToBottom = true;
        float size = 1.0;
        float spacing = 1.0;
        openvrml::font_style_node * fontStyle =
            node_cast<openvrml::font_style_node *>(
                this->font_style_.sfnode::value().get());
        if (fontStyle) {
            horizontal = fontStyle->horizontal();
            if (!fontStyle->justify().empty()) {
                justify[0] = fontStyle->justify()[0];
            }
            if (fontStyle->justify().size() > 1) {
                justify[1] = fontStyle->justify()[1];
            }
            leftToRight = fontStyle->left_to_right();
            topToBottom = fontStyle->top_to_bottom();
            size = fontStyle->size();
            spacing = fontStyle->spacing();
        }

        text_geometry newGeometry;
        float geometryXMin = 0.0, geometryXMax = 0.0;
        float geometryYMin = 0.0, geometryYMax = 0.0;
        size_t npolygons = 0;
        const ucs4_string_t::const_iterator stringBegin =
            this->ucs4_string.begin();
        for (ucs4_string_t::const_iterator string = stringBegin;
             string != this->ucs4_string.end();
             ++string) {
            float penPos[2] = { 0.0, 0.0 };
            const size_t line = std::distance(stringBegin, string);
            const float lineAdvance = size * spacing * line;
            if (horizontal) {
                if (topToBottom) {
                    penPos[1] -= lineAdvance;
                } else {
                    penPos[1] += lineAdvance;
                }
            } else {
                if (leftToRight) {
                    penPos[0] += lineAdvance;
                } else {
                    penPos[0] -= lineAdvance;
                }
            }

            using openvrml::int32;

            struct LineGeometry {
                vector<vec2f> coord;
                vector<int32> coordIndex;
                float xMin, xMax;
                float yMin, yMax;

                LineGeometry(): xMin(0.0), xMax(0.0), yMin(0.0), yMax(0.0)
                {}
            };

            LineGeometry lineGeometry;
            for (vector<char32_t>::const_iterator character = string->begin();
                 character != string->end(); ++character) {
                assert(this->face);
                const FT_UInt glyphIndex =
#   ifdef _WIN32
                    FT_Get_Char_Index(this->face, *character);
#   else
                    FcFreeTypeCharIndex(this->face, *character);
#   endif

                const glyph_geometry * glyphGeometry = 0;
                const glyph_geometry_map_t::iterator pos =
                    this->glyph_geometry_map.find(glyphIndex);
                if (pos != this->glyph_geometry_map.end()) {
                    glyphGeometry = &pos->second;
                } else {
                    FT_Error error = FT_Err_Ok;
                    error = FT_Load_Glyph(this->face,
                                          glyphIndex,
                                          FT_LOAD_NO_SCALE);
                    assert(error == FT_Err_Ok);
                    FT_Glyph glyph;
                    error = FT_Get_Glyph(this->face->glyph, &glyph);
                    assert(error == FT_Err_Ok);
                    static FT_Outline_Funcs outlineFuncs = { moveTo_,
                                                             lineTo_,
                                                             conicTo_,
                                                             cubicTo_,
                                                             0,
                                                             0 };
                    const float glyphScale = (this->face->bbox.yMax > 0.0)
                        ? size / this->face->bbox.yMax
                        : size;
                    GlyphContours_ glyphContours(glyphScale);
                    assert(glyph->format == ft_glyph_format_outline);
                    const FT_OutlineGlyph outlineGlyph =
                        reinterpret_cast<FT_OutlineGlyph>(glyph);
                    error = FT_Outline_Decompose(&outlineGlyph->outline,
                                                 &outlineFuncs,
                                                 &glyphContours);
                    assert(error == FT_Err_Ok);

                    assert(this->face->glyph);
                    const float advanceWidth =
                        FT_HAS_HORIZONTAL(this->face)
                        ? this->face->glyph->metrics.horiAdvance * glyphScale
                        : 0.0f;
                    const float advanceHeight =
                        FT_HAS_VERTICAL(this->face)
                        ? this->face->glyph->metrics.vertAdvance * glyphScale
                        : 0.0f;

                    const glyph_geometry_map_t::value_type
                        value(glyphIndex,
                              glyph_geometry(glyphContours.contours,
                                             advanceWidth,
                                             advanceHeight));
                    const pair<glyph_geometry_map_t::iterator, bool> result =
                        this->glyph_geometry_map.insert(value);
                    assert(result.second);
                    glyphGeometry = &result.first->second;
                }

                for (size_t i = 0; i < glyphGeometry->coord.size(); ++i) {
                    const vec2f & glyphVertex = glyphGeometry->coord[i];
                    const vec2f textVertex =
                        make_vec2f(glyphVertex[0] + penPos[0],
                                   glyphVertex[1] + penPos[1]);
                    lineGeometry.coord.push_back(textVertex);
                    lineGeometry.xMin = (lineGeometry.xMin < textVertex[0])
                        ? lineGeometry.xMin
                        : textVertex[0];
                    lineGeometry.xMax = (lineGeometry.xMax > textVertex[0])
                        ? lineGeometry.xMax
                        : textVertex[0];
                    lineGeometry.yMin = (lineGeometry.yMin < textVertex[1])
                        ? lineGeometry.yMin
                        : textVertex[1];
                    lineGeometry.yMax = (lineGeometry.yMax > textVertex[1])
                        ? lineGeometry.yMax
                        : textVertex[1];
                }

                for (size_t i = 0; i < glyphGeometry->coord_index.size(); ++i)
                {
                    const long index = glyphGeometry->coord_index[i];
                    if (index > -1) {
                        const size_t offset = lineGeometry.coord.size()
                            - glyphGeometry->coord.size();
                        lineGeometry.coordIndex.push_back(int32(offset + index));
                    } else {
                        lineGeometry.coordIndex.push_back(-1);
                        ++npolygons;
                    }
                }
                if (horizontal) {
                    const float xAdvance = glyphGeometry->advance_width;
                    if (leftToRight) {
                        penPos[0] += xAdvance;
                    } else {
                        penPos[0] -= xAdvance;
                    }
                } else {
                    const float yAdvance = glyphGeometry->advance_height;
                    if (topToBottom) {
                        penPos[1] -= yAdvance;
                    } else {
                        penPos[1] += yAdvance;
                    }
                }
            }

            //
            // Scale to length.
            //
            const float length =
                (line < this->length_.mffloat::value().size())
                ? this->length_.mffloat::value()[line]
                : 0.0f;
            if (length > 0.0) {
                const float currentLength =
                    lineGeometry.xMax - lineGeometry.xMin;
                for (size_t i = 0; i < lineGeometry.coord.size(); ++i) {
                    const vec2f & vertex = lineGeometry.coord[i];
                    const vec2f scaledVertex =
                        openvrml::make_vec2f(vertex[0] / currentLength * length,
                                             vertex[1]);
                    lineGeometry.coord[i] = scaledVertex;
                }
            }

            //
            // Add the line to the text geometry.  We need to adjust for the
            // major alignment.
            //
            float xOffset = 0.0f, yOffset = 0.0f;
            //
            // Offset is 0 for "BEGIN" or "FIRST" (or anything else, in our
            // case).
            //
            if (justify[0] == "MIDDLE") {
                if (horizontal) {
                    xOffset =
                        -((lineGeometry.xMax - lineGeometry.xMin) / 2.0f);
                } else {
                    yOffset = (lineGeometry.yMax - lineGeometry.yMin) / 2.0f;
                }
            } else if (justify[0] == "END") {
                if (horizontal) {
                    xOffset = -(lineGeometry.xMax - lineGeometry.xMin);
                } else {
                    yOffset = lineGeometry.yMax - lineGeometry.yMin;
                }
            }
            for (size_t i = 0; i < lineGeometry.coordIndex.size(); ++i) {
                const long index = lineGeometry.coordIndex[i];
                if (index > -1) {
                    const vec2f & lineVertex = lineGeometry.coord[index];
                    const openvrml::vec3f textVertex =
                        openvrml::make_vec3f(lineVertex.x() + xOffset,
                                             lineVertex.y() + yOffset,
                                             0.0f);
                    newGeometry.coord.push_back(textVertex);
                    newGeometry.coord_index
                        .push_back(int32(newGeometry.coord.size() - 1));
                    geometryXMin = (geometryXMin < textVertex.x())
                        ? geometryXMin
                        : textVertex.x();
                    geometryXMax = (geometryXMax > textVertex.x())
                        ? geometryXMax
                        : textVertex.x();
                    geometryYMin = (geometryYMin < textVertex.y())
                        ? geometryYMin
                        : textVertex.y();
                    geometryYMax = (geometryYMax > textVertex.y())
                        ? geometryYMax
                        : textVertex.y();
                } else {
                    newGeometry.coord_index.push_back(-1);
                }
            }
        }

        //
        // Scale to maxExtent.
        //
        const float maxExtent =
            (this->max_extent_.sffloat::value() > 0.0)
            ? this->max_extent_.sffloat::value()
            : 0.0f;
        if (maxExtent > 0.0) {
            const float currentMaxExtent = geometryXMax - geometryXMin;
            if (currentMaxExtent > maxExtent) {
                for (size_t i = 0; i < newGeometry.coord.size(); ++i) {
                    const vec3f & vertex = newGeometry.coord[i];
                    const vec3f scaledVertex =
                        make_vec3f(vertex.x() / currentMaxExtent * maxExtent,
                                   vertex.y(),
                                   vertex.z());
                    newGeometry.coord[i] = scaledVertex;
                }
            }
        }

        //
        // Adjust for the minor alignment.
        //
        float xOffset = 0.0f, yOffset = 0.0f;
        if (justify[1] == "FIRST" || justify[1] == "") {
        } else if (justify[1] == "BEGIN") {
            if (horizontal) {
                yOffset = -(size * spacing);
            } else {
                xOffset = 0.0f;
            }
        } else if (justify[1] == "MIDDLE") {
            if (horizontal) {
                yOffset = ((size * spacing
                            * this->string_.mfstring::value().size()) / 2.0f)
                    - (size * spacing);
            } else {
                xOffset = ((size * spacing
                            * this->string_.mfstring::value().size()) / 2.0f)
                    - (size * spacing);
            }
        } else if (justify[1] == "END") {
            if (horizontal) {
                yOffset = size * spacing
                    * (this->string_.mfstring::value().size() - 1);
            } else {
                xOffset = size * spacing
                    * (this->string_.mfstring::value().size() - 1);
            }
        }
        for (size_t i = 0; i < newGeometry.coord.size(); ++i) {
            const vec3f & vertex = newGeometry.coord[i];
            const vec3f adjustedVertex = make_vec3f(vertex.x() + xOffset,
                                                    vertex.y() + yOffset,
                                                    vertex.z());
            newGeometry.coord[i] = adjustedVertex;
        }

        //
        // Create the normals.
        //
        newGeometry.normal.resize(npolygons); // Throws std::bad_alloc.
        for (size_t i = 0; i < newGeometry.normal.size(); ++i) {
            static const vec3f normal = make_vec3f(0.0, 0.0, 1.0);
            newGeometry.normal[i] = normal;
        }

        //
        // Create the texture coordinates.
        //
        newGeometry.tex_coord.resize(newGeometry.coord.size()); // std::bad_alloc
        for (size_t i = 0; i < newGeometry.tex_coord.size(); ++i) {
            const vec3f & vertex = newGeometry.coord[i];
            newGeometry.tex_coord[i] = make_vec2f(vertex.x() / size,
                                                  vertex.y() / size);
        }

        this->text_geometry_ = newGeometry;
# endif // OPENVRML_ENABLE_RENDER_TEXT_NODE
    }
}


/**
 * @brief @c node_metatype identifier.
 */
const char * const openvrml_node_vrml97::text_metatype::id =
    "urn:X-openvrml:node:Text";

/**
 * @var FT_Library text_metatype::freeTypeLibrary
 *
 * @brief FreeType library handle.
 *
 * @see http://freetype.org/freetype2/docs/reference/ft2-base_interface.html#FT_Library
 */

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this @c node_metatype.
 */
openvrml_node_vrml97::text_metatype::
text_metatype(openvrml::browser & browser):
    node_metatype(text_metatype::id, browser)
{
# ifdef OPENVRML_ENABLE_RENDER_TEXT_NODE
    FT_Error error = 0;
    error = FT_Init_FreeType(&this->freeTypeLibrary);
    if (error) {
        browser.err("error initializing FreeType library");
    }
# endif // OPENVRML_ENABLE_RENDER_TEXT_NODE
}

/**
 * @brief Destroy.
 */
openvrml_node_vrml97::text_metatype::~text_metatype() OPENVRML_NOTHROW
{
# ifdef OPENVRML_ENABLE_RENDER_TEXT_NODE
    FT_Error error = 0;
    error = FT_Done_FreeType(this->freeTypeLibrary);
    if (error) {
        this->browser().err("error shutting down FreeType library");
    }
# endif // OPENVRML_ENABLE_RENDER_TEXT_NODE
}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a boost::shared_ptr<node_type> to a node_type capable of
 *         creating Text nodes.
 *
 * @exception openvrml::unsupported_interface if @p interfaces includes an interface
 *                                  not supported by text_metatype.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml_node_vrml97::text_metatype::
do_create_type(const std::string & id,
               const openvrml::node_interface_set & interfaces) const
    OPENVRML_THROW2(openvrml::unsupported_interface, std::bad_alloc)
{
    using namespace openvrml;
    using namespace openvrml::node_impl_util;

    typedef boost::array<node_interface, 6> supported_interfaces_t;
    static const supported_interfaces_t supported_interfaces = {
        node_interface(node_interface::exposedfield_id,
                       field_value::mfstring_id,
                       "string"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "fontStyle"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mffloat_id,
                       "length"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sffloat_id,
                       "maxExtent"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata"),
        node_interface(node_interface::field_id,
                       field_value::sfbool_id,
                       "solid")
    };

    typedef node_impl_util::node_type_impl<text_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & textNodeType = static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface(interfaces.begin());
         interface != interfaces.end();
         ++interface) {
        supported_interfaces_t::const_iterator supported_interface =
            supported_interfaces.begin() - 1;
        if (*interface == *++supported_interface) {
            textNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &text_node::string_);
        } else if (*interface == *++supported_interface) {
            textNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &text_node::font_style_);
        } else if (*interface == *++supported_interface) {
            textNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &text_node::length_);
        } else if (*interface == *++supported_interface) {
            textNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &text_node::max_extent_);
        } else if (*interface == *++supported_interface) {
            textNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &text_node::metadata);
        } else if (*interface == *++supported_interface) {
            textNodeType.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &text_node::solid_);
        } else {
            throw unsupported_interface(*interface);
        }
    }
    return type;
}
