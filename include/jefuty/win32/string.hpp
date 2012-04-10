#if !defined JEFUTY_WIN32_STRING_INCLUDED
#define      JEFUTY_WIN32_STRING_INCLUDED

#include <string>
#include <tchar.h>

namespace jefuty { namespace win32 {

typedef TCHAR char_t;
typedef std::basic_string<TCHAR> string;


string operator+ (string const & lhs, string const & rhs);

string of_string(std::string const & str);

template<size_t N> string of_string(TCHAR const (&ss)[N]) {
	return string(ss, ss+N);
}

} // namespace win32
} // namespace jefuty

#endif    // JEFUTY_WIN32_STRING_INCLUDED

