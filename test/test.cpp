#include <iostream>
#include <vector>
#include <type_traits>

#include <jefuty/network/network.hpp>
#include <jefuty/type_traits.hpp>

namespace test {

template<typename C, typename V = void>
struct enable_if : std::enable_if<C::value, V>
{ };

template<typename C, typename V = void>
struct disable_if : std::enable_if<!C::value, V>
{ };

};

struct W {
	void width () { }
};
struct H {
	void height () { }
};

template<typename T>
void getw (T const & t, typename test::enable_if<  jefuty::type_traits::has_width<T> >::type* =0) {
	std::cout << "has width()\n";
}
template<typename T>
void getw (T const & t, typename test::disable_if< jefuty::type_traits::has_width<T> >::type* =0) {
	std::cout << "has no width()\n";
}
template<typename T>
void geth (T const & t, typename test::enable_if< jefuty::type_traits::has_height<T> >::type* =0) {
	std::cout << "has height()\n";
}
template<typename T>
void geth (T const & t, typename test::disable_if< jefuty::type_traits::has_height<T> >::type* =0) {
	std::cout << "has no height()\n";
}

int main () {
	using std::cout;
	namespace net = jefuty::network;
	std::vector<net::mac_address> xs = net::mac_addresses();
	for (std::vector<net::mac_address>::const_iterator it=xs.begin(); it!=xs.end(); ++it)
		cout << *it << "\n";

	getw(W());
	geth(W());
	getw(H());
	geth(H());
}


