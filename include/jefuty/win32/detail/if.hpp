#if !defined JEFUTY_WIN32_DETAIL_IF_INCLUDED
#define      JEFUTY_WIN32_DETAIL_IF_INCLUDED

/****
 * wrapper for network adapters
 *
 ** note
 *     if you get error in "intrin.h" like
 *         "...second C linkage of overloaded function '_interlockedbittestandset' not allowed..."
 *     try comment out 2lines as follors at intrin.h
 *         //__MACHINEI(unsigned char _interlockedbittestandset(long *a, long b))
 *         //__MACHINEI(unsigned char _interlockedbittestandreset(long *a, long b))
 ****/

#include <vector>
#include <jefuty/network/detail/mac_addr.hpp>


namespace jefuty { namespace win32 { namespace network { namespace detail {

	using jefuty::network::mac_address;
	using jefuty::network::address_family;

std::vector<mac_address> mac_addresses (address_family family);


} // namespace detail
} // namespace network
} // namespace win32
} // namespace jefuty

#endif    /* JEFUTY_WIN32_DETAIL_IF_INCLUDED */

