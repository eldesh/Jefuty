#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <iomanip>
#include <vector>
#include <array>
#include <sstream>
#include <cstdint>

#include <boost/optional.hpp>
#include <boost/io/ios_state.hpp>

#include <jefuty/network/mac_addr.hpp>

#if defined _WIN32
#  include <sdkddkver.h> // WINVER
#elif defined __CYGWIN__
#  include <fcntl.h>
#  include <stdlib.h>
#  include <string.h>
#  include <unistd.h>
#  include <sys/ioctl.h>
#  include <sys/types.h>
#  include <sys/socket.h>
#  include <netinet/in.h>
typedef unsigned char BYTE;
#else
#  error "unsupported platform..."
#endif

namespace jefuty { namespace network {

std::vector<mac_address> mac_addresses () {
	return mac_addresses(UNSPEC);
}

std::vector<mac_address> mac_addresses (address_family family) {
	typedef std::vector<mac_address> result_type;

#if NTDDI_WINXP <= NTDDI_VERSION
	// XP and later
	// http://msdn.microsoft.com/en-us/library/windows/desktop/aa365915%28v=vs.85%29.aspx
	ULONG family_ = family==UNSPEC ? AF_UNSPEC
		          : family==IPV4   ? AF_INET
				  : family==IPV6   ? AF_INET6
				  : (assert(false), 0);

	ULONG const suggested_working_size = 15000; // 15kbyte size is suggested. see Remark section
	ULONG adapters_size = suggested_working_size;
	ULONG const flags = 0
#  if NTDDI_VISTA <= NTDDI_VERSION // vista and later
		//| GAA_FLAG_INCLUDE_ALL_INTERFACES // get adapters not bound to an address family defined in address_family type
#  endif
		;

	std::vector<byte_t> buffer(adapters_size);
	PIP_ADAPTER_ADDRESSES adps = reinterpret_cast<PIP_ADAPTER_ADDRESSES>(buffer.data());
	
	result_type xs;
	if (::GetAdaptersAddresses(family_, flags, nullptr, adps, &adapters_size) == NO_ERROR) {
		for (; adps; adps=adps->Next) {
			if (adps->IfType != IF_TYPE_SOFTWARE_LOOPBACK) // loopback
				xs.push_back(mac_address(adps->PhysicalAddress, adps->PhysicalAddressLength));
		}
	}
	return xs;
#elif NTDDI_VERSION < NTDDI_WINXP
	// before XP
	// TODO: return adapters that not bound to IPv4 address
	ULONG n=0;
	::GetAdaptersInfo(NULL,&n);
	std::vector<byte_t> buff(n);
	PIP_ADAPTER_INFO adps = reinterpret_cast<PIP_ADAPTER_INFO>(buff.data());
	std::vector<mac_address> xs;
	if (::GetAdaptersInfo(adps,&n) == NO_ERROR) {
		for (; adps; adps=adps->Next) {
			xs.push_back(mac_address(adps->Address, adps->AddressLength));
		}
	}
	return xs;
#else
#  error "unsupported platform"
#endif
}

std::string mac_address::to_string () const {
	return to_string('-');
}

std::string mac_address::to_string (char sep) const {
	std::stringstream ss;
	ss << std::setfill ('0') << std::hex;
	if (size()) {
		ss << std::setw(2) << static_cast<int>(address_m[0]);
		for (size_t i=1; i<size(); ++i)
			ss << sep << std::setw(2) << static_cast<int>(address_m[i]);
	}
	return ss.str();
}


std::ostream & operator<< (std::ostream & os, mac_address const & addr) {
	return os << addr.to_string('-');
}

} // namespace network
} // namespace jefuty
