#if !defined JEFUTY_NETWORK_DETAIL_IF_INCLUDED
#define      JEFUTY_NETWORK_DETAIL_IF_INCLUDED

#include <vector>
#include "mac_addr.hpp"

namespace jefuty { namespace network { namespace detail {


std::vector<mac_address> mac_addresses (address_family family);


} // namespace detail
} // namespace network
} // namespace jefuty

#endif    /* JEFUTY_NETWORK_DETAIL_IF_INCLUDED */

