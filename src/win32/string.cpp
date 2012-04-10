#include <vector>
#include <cstdlib>

#include <windows.h>
#include <tchar.h>

#include <jefuty/win32/string.hpp>

namespace jefuty { namespace win32 {

string operator+ (string const & lhs, string const & rhs) {
	int len = ::lstrlen(lhs.c_str()) + ::lstrlen(rhs.c_str());
	std::vector<TCHAR> ss(len);
	::lstrcat(&ss[0], lhs.c_str());
	::lstrcat(&ss[0], rhs.c_str());
	return string(ss.begin(), ss.end());
}

#pragma warning( push )
#pragma warning( disable: 4996 )
string of_string(std::string const & str) {
	std::vector<TCHAR> ss(str.length());
	::mbstowcs(&ss[0], str.c_str(), ss.size());
	return string(ss.begin(), ss.end());
}
#pragma warning( pop )

} // namespace win32
} // namespace jefuty


