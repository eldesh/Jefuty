#include <vector>
#include <memory>
#include <boost/optional.hpp>

#if defined __CYGWIN__
#  include <fcntl.h>
#  include <stdlib.h>
#  include <string.h>
#  include <unistd.h>
#  include <sys/ioctl.h>
#  include <sys/types.h>
#  include <sys/socket.h>
#  include <netinet/in.h>
#else
#  error "unsupported platform..."
#endif

#include <jefuty/network/detail/mac_addr.hpp>
#include <jefuty/network/detail/if.hpp>

namespace jefuty { namespace network { namespace detail {

// network devices (only bound to IPv4 devices)
std::vector<ifreq> network_devices (int sock) {
	std::vector<ifreq> xs;
	std::vector<char> buff;
	ifconf ifc;
	int N = 0;
	do {
		++N;
		buff = std::vector<char>(sizeof(ifreq)*N);
		ifc.ifc_len = sizeof(ifreq)*N;
		ifc.ifc_buf = buff.data();
		if (ioctl(sock, SIOCGIFCONF, &ifc)==-1) { // receive array of ifreq
			return std::vector<ifreq>();
		}
	} while (static_cast<int>(sizeof(ifreq)*N) <= ifc.ifc_len);
	return std::vector<ifreq>(ifc.ifc_req, &ifc.ifc_req[ifc.ifc_len/sizeof(ifreq)]);
}

struct socket_closer {
	void operator()(int * s) const {
		if (s && *s==-1)
			close(*s);
		free(s);
	}
};

template<typename F, typename T, size_t N>
auto map_to(F const & f, std::array<T,N> const & xs) -> std::array<decltype(f(std::declval<T>())), N>
{
	std::array<decltype(f(std::declval<T>())), N> ys;
	for (typename std::array<T,N>::size_type i=0; i<xs.size(); ++i)
		ys[i] = f(xs[i]);
	return ys;
}

boost::optional<mac_address> get_mac_address(int sock, ifreq & ifr) {
	if (ioctl(sock, SIOCGIFHWADDR, &ifr) == 0) {
		auto addr = map_to([](char c) -> byte_t { return static_cast<byte_t>(c); }, jefuty::to_array(ifr.ifr_hwaddr.sa_data));
		return boost::optional<mac_address>(jefuty::truncate_to<6>()(addr)); // specify to size of IPv4 address
	} else
		return boost::optional<mac_address>();
}

std::vector<mac_address> mac_addresses (address_family family) {
	// http://linuxjm.sourceforge.jp/html/LDP_man-pages/man7/netdevice.7.html
	using std::unique_ptr;
	using std::vector;
	using boost::optional;
	vector<mac_address> xs;
	unique_ptr<int, socket_closer> sock
		= unique_ptr<int, socket_closer>(new int(socket(AF_INET, SOCK_DGRAM, 0)));
	if (*sock==-1)
		return xs;
	vector<ifreq> ifreqs = network_devices(*sock);
	for (auto it=ifreqs.begin(); it!=ifreqs.end(); ++it) {
		if (ioctl(*sock, SIOCGIFFLAGS, &*it)==0) {
			if (! (it->ifr_flags & IFF_LOOPBACK)) { // except loop back IF
				if (optional<mac_address> addr = get_mac_address(*sock, *it)) {
					xs.push_back(*addr);
				}
			}
		}
	}
	return xs;
}

} // namespace detail
} // namespace network
} // namespace jefuty


