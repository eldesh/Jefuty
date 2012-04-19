#if !defined JEFUTY_NETWORK_DETAIL_MACADDR_INCLUDED
#define      JEFUTY_NETWORK_DETAIL_MACADDR_INCLUDED

#include <vector>
#include <array>
#include <string>
#include <utility>
#include <type_traits>
#include <cassert>

#if defined _WIN32
#  include <WinSock2.h>
#  include <iphlpapi.h>
#elif defined __CYGWIN__
#  define nullptr NULL
#else
#  error "unsupported platform..."
#endif

#include <jefuty/array.hpp>

namespace jefuty { namespace network {

#if defined _WIN32
	typedef BYTE          byte_t;
#elif defined __CYGWIN__
	typedef unsigned char byte_t;
#endif

enum mac_address_type {
	MAC48,
	EUI48,
	EUI64
};

struct mac_address {
#if defined WIN32
	static const std::size_t max_length = MAX_ADAPTER_ADDRESS_LENGTH;
#else
	static const std::size_t max_length = 8;
#endif
private:
	typedef std::array<byte_t, max_length> address_type;
	typedef address_type::const_iterator const_iterator;

	address_type address_m;
	size_t size_m; // actual number of octed specify address
	//mac_address_type tag_m;

public:

	// REVISIT:
	//     default constructable is required by std::vector, but it is invalid value
	mac_address() : address_m(), size_m()
	{
	}

	template<typename T, size_t N>
	mac_address(std::array<T,N> const & xs) : address_m(jefuty::expand_to<max_length>()(xs)), size_m(N)
	{
		//static_assert(N<=max_length);
	}

	template<typename T, size_t N>
	mac_address(T (&xs)[N], size_t size, typename std::enable_if<(N==max_length)||(N==6)>::type* =nullptr)
		: address_m(jefuty::to_array(xs)), size_m(size)
	{
		assert(size <= N);
	}
	
	template<typename T, size_t N>
	mac_address(T (&xs)[N], typename std::enable_if<(N==max_length)||(N==6)>::type* =nullptr)
		: address_m(jefuty::to_array(xs)), size_m(N)
	{
		/* REVISIT: how specify kind of address
		     if (size_m==6) tag_m = MAC48; // or EUI48
		else if (size_m==8) tag_m = EUI64;
		*/
	}

	size_t size () const { return size_m; }

	// don't publish mutable iterators because mac-address is immutable value
	const_iterator begin () const { return address_m.begin(); }
	const_iterator end   () const { return address_m.end(); }

	byte_t operator[](int idx) const {
		return address_m[idx];
	}

	template<size_t I> byte_t get() const { return std::get<I>(address_m); }

	// convert to human friendly string
	std::string to_string ()         const ;
	std::string to_string (char sep) const ;
};

enum address_family {
	UNSPEC, // both IPv4,IPv6
	IPV4,
	IPV6,
};

// all MAC-addresses of system (get informations except for loopback interface)
std::vector<mac_address> mac_addresses (void);
std::vector<mac_address> mac_addresses (address_family);

// The standard (IEEE 802) format for printing MAC-48 addresses in human-friendly form
std::ostream & operator<< (std::ostream & os, mac_address const & addr);

} // namespace network
} // namespace jefuty

#endif    /* JEFUTY_NETWORK_DETAIL_MACADDR_INCLUDED */

