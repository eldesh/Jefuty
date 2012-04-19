#include <vector>

#if defined WIN32
#  include <WinSock2.h>
#  include <iphlpapi.h>
#else
#  error "unsupported platform..."
#endif

#include <jefuty/network/detail/mac_addr.hpp>
#include <jefuty/win32/detail/if.hpp>

namespace jefuty { namespace win32 { namespace network { namespace detail {

	using jefuty::network::address_family;
	using jefuty::network::byte_t;

std::vector<mac_address> mac_addresses (address_family family) {

#if NTDDI_WINXP <= NTDDI_VERSION
	// XP and later
	// http://msdn.microsoft.com/en-us/library/windows/desktop/aa365915%28v=vs.85%29.aspx
	ULONG const suggested_working_size = 15000; // 15kbyte size is suggested. see Remark section
	ULONG adapters_size = suggested_working_size;
	ULONG const flags = 0
#  if NTDDI_VISTA <= NTDDI_VERSION // vista and later
		//| GAA_FLAG_INCLUDE_ALL_INTERFACES // get adapters not bound to an address family defined in address_family type
#  endif
		;
	namespace net = jefuty::network;
	ULONG family_ = family==net::UNSPEC ? AF_UNSPEC
		          : family==net::IPV4   ? AF_INET
				  : family==net::IPV6   ? AF_INET6
				  : (assert(false), 0);
	std::vector<byte_t> buffer(adapters_size);
	PIP_ADAPTER_ADDRESSES adps = reinterpret_cast<PIP_ADAPTER_ADDRESSES>(buffer.data());
	
	//
	std::vector<mac_address> xs;
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

} // namespace detail
} // namespace network
} // namespace win32
} // namespace jefuty


