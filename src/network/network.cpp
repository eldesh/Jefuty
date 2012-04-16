#include <vector>

#include <boost/optional.hpp>
#include <boost/io/ios_state.hpp>

#if defined _WIN32
#  include <jefuty/win32/detail/if.hpp>
#elif defined __CYGWIN__
#  include <jefuty/network/network.hpp>
#else
#  error "unsupported platform..."
#endif

namespace jefuty { namespace network {

std::vector<mac_address> mac_addresses () {
	return mac_addresses(UNSPEC);
}

std::vector<mac_address> mac_addresses (address_family family) {
#if defined WIN32
	// TODO: publish mac_addresses at win32::network with `using`
	return jefuty::win32::network::detail::mac_addresses(family);
#elif defined __CYGWIN__
	return detail::mac_addresses(family);
#else
#  error "unsupported platform"
#endif
}

} // namespace network
} // namespace jefuty

