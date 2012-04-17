#if !defined JEFUTY_ARRAY_INCLUDED
#define      JEFUTY_ARRAY_INCLUDED

namespace jefuty {


// [#a] -> array<a>
template<typename T, size_t N>
std::array<T,N> to_array (T const (&xs)[N]) {
	std::array<T,N> ys;
	for (size_t i=0; i!=N; ++i)
		ys[i] = xs[i];
	return ys;
}

template<size_t To>
struct expand_to {
	template<typename T, size_t From>
	std::array<T,To> operator() (std::array<T,From> const & xs) const {
		//static_assert(From <= To);
		std::array<T,To> ys;
		for (size_t i=0; i<From; ++i)
			ys[i] = xs[i];
		return ys;
	}
};

template<size_t To>
struct truncate_to {
	template<typename T, size_t From>
	std::array<T,To> operator() (std::array<T,From> const & xs) const {
		//static_assert(From <= To);
		std::array<T,To> ys;
		for (size_t i=0; i<To; ++i)
			ys[i] = xs[i];
		return ys;
	}
};


} // namespace jefuty

#endif    /* JEFUTY_ARRAY_INCLUDED */

