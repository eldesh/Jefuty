#if !defined JEFUTY_TYPE_TRAITS_INCLUDED
#define      JEFUTY_TYPE_TRAITS_INCLUDED

#include <utility>
#include <type_traits>
#include <boost/mpl/bool.hpp>

namespace jefuty { namespace type_traits {

namespace detail {

// workaround for MSVC2010
template<typename T> T && declval ();

template<typename T>
struct has_width_impl {
	template<typename U>
	static auto check (U u) -> decltype(u.width(), std::true_type());
	static std::false_type check (...) ;

	static bool const value = std::identity<decltype(check(declval<T>()))>::type::value;
};

template<typename T>
struct has_height_impl {
	template<typename U>
	static auto check (U u) -> decltype(u.height(), std::true_type());
	static std::false_type check (...) ;

	static bool const value = std::identity<decltype(check(declval<T>()))>::type::value;
};

} // namespace detail

template<typename T>
struct has_width : boost::mpl::bool_<detail::has_width_impl<T>::value>
{};

template<typename T>
struct has_height : boost::mpl::bool_<detail::has_height_impl<T>::value>
{};


} // namespace type_traits
} // namespace jefuty

#endif    /* JEFUTY_TYPE_TRAITS_INCLUDED */

