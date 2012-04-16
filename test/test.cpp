#include <iostream>
#include <vector>
#include <jefuty/network/network.hpp>

int main () {
	using std::cout;
	namespace net = jefuty::network;
	std::vector<net::mac_address> xs = net::mac_addresses();
	for (std::vector<net::mac_address>::const_iterator it=xs.begin(); it!=xs.end(); ++it)
		cout << *it << "\n";

}
