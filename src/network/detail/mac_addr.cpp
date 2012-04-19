#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <iomanip>
#include <vector>
#include <vector>
#include <sstream>

#include <boost/io/ios_state.hpp>

#include <jefuty/network/detail/mac_addr.hpp>

namespace jefuty { namespace network {

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

