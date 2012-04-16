#if !defined JEFUTY_NETWORK_NETWORK_INCLUDED
#define      JEFUTY_NETWORK_NETWORK_INCLUDED

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

#include "detail/mac_addr.hpp"

#if defined _WIN32
#  include <WinSock2.h>
#  include <iphlpapi.h>
#  include <jefuty/win32/detail/if.hpp>
#elif defined __CYGWIN__
#  include "detail/if.hpp"
#  define nullptr NULL
#else
#  error "unsupported platform..."
#endif


namespace jefuty { namespace network {

// all MAC-addresses of system (get informations except for loopback interface)
std::vector<mac_address> mac_addresses (void);
std::vector<mac_address> mac_addresses (address_family);

} // namespace network
} // namespace jefuty

#endif    /* JEFUTY_NETWORK_NETWORK_INCLUDED */

