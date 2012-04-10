#if !defined JEFUTY_POLYGON_INCLUDED
#define      JEFUTY_POLYGON_INCLUDED

#include <boost/range.hpp>

namespace jefuty {

using boost::begin;
using boost::end;

// msvc2010 workaround
template<typename T> T declval (void);

template<typename T> void ignore (T) { }

template<typename T> class point {
	boost::tuple<T,T> coord_m;
public:
	point(T x, T y) : coord_m(x,y) { }
	T x (void) const { return boost::get<0>(coord_m); }
	T y (void) const { return boost::get<1>(coord_m); }
};

template<typename Point> point<LONG> point_of(Point p) {
	return point<decltype(p.x)>(p.x, p.y);
}

template<typename T> point<T> make_point (T x, T y) {
	return point<T>(x, y);
}

template<typename T> struct rect {
	point<T> origin_m;
	boost::tuple<T,T> size_m;
public:
	rect (T x, T y, T w, T h) : origin_m(x,y), size_m(w,h) { }
	point<T> origin (void) const { return origin_m; }
	T width  (void) const { return boost::get<0>(size_m); }
	T height (void) const { return boost::get<1>(size_m); }
};

template<typename T>
std::ostream & operator<< (std::ostream & os, point<T> const & p) {
	return os << "(" << p.x() << "," << p.y() << ")";
}

template<typename T>
std::ostream & operator<< (std::ostream & os, rect<T> const & r) {
	point<T> p = r.origin();
	return os << "(" << p.x()     << "," << p.y()     << ","
					 << width(r)  << "," << height(r) << ")";
}

template<typename T>
rect<T> make_rect(T x, T y, T w, T h) {
	return rect<T>(x, y, w, h);
}

template<typename T>
class has_width {
	template<typename U>
	static auto check (U u) -> decltype(u.width(), std::true_type());
	static std::false_type check (...);
public:
	static const bool value = std::identity<decltype(check(declval<T>()))>::type::value;
};

template<typename T>
class has_height {
	template<typename U>
	static auto check (U u) -> decltype(u.height(), std::true_type());
	static std::false_type check (...);
public:
	static const bool value = std::identity<decltype(check(declval<T>()))>::type::value;
};

//template<typename T, typename Result = typename std::enable_if<has_width<T>::value >::type >
//auto width  (T const & t) -> decltype(t.width())  { return t.width(); }
template<typename T>
auto width  (T const & t) -> decltype(t.width()) { return t.width(); }

template<typename T>
auto height (T const & t) -> decltype(t.height()) { return t.height(); }


// REVISIT: to use default template argument for SFINAE
//          (but msvc2010 can't accept default template function parameters)

// specific to RECT (@WinAPI)
template<typename T>
auto width (T const & r, typename std::enable_if< !has_width<T>::value >::type * =0) -> decltype(r.right)
{ return r.right - r.left; }

// specific to RECT (@WinAPI)
template<typename T>
auto height (T const & r, typename std::enable_if< !has_height<T>::value >::type * =0) -> decltype(r.bottom)
{ return r.bottom - r.top; }


} // namespace jefuty

#endif    /* JEFUTY_POLYGON_INCLUDED */

