#if !defined JEFUTY_WIN32_INCLUDED
#define      JEFUTY_WIN32_INCLUDED


/****
 *
 * Jefuty >> Win32
 *
 *     windows API wrapper library
 *
 *
 ** functionalities
 *     provide abstractions are...
 *
 *         - window handle and related messaging system
 *         - network interface
 *     future plan
 *         - interoperable Unicode(and other encodings) string with standard string
 *
 ** reference
 *     windows version
 *         http://msdn.microsoft.com/en-us/library/aa383745%28v=vs.85%29.aspx
 *
 *     predefined macros
 *         http://msdn.microsoft.com/en-us/library/b0084kay%28v=vs.100%29.aspx
 *
 *     windows data types
 *         http://msdn.microsoft.com/en-us/library/aa383751%28v=VS.85%29.aspx
 *
 ****/


/// win32
#if defined _WIN32
#  include <windows.h>
#  include <commctrl.h>
#  include <tchar.h>
#else
#  error "jefuty/win32 is windows specific wrapper library"
#endif

/// std
#include <string>
#include <vector>
#include <map>
#include <numeric>
#include <iostream>

/// boost
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/range.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/noncopyable.hpp>
#include <boost/pool/singleton_pool.hpp>
#include <boost/thread.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>

#include "../jefuty/polygon.hpp"
#include "win32/string.hpp"
#include "win32/window_handle_t.hpp"
#include "win32/window_class.hpp"

namespace jefuty {


namespace win32 {

static const int default_size = CW_USEDEFAULT;

//// TODO: return window_handle from create_window functions

HWND create_window(
	LPCTSTR ClassName,
	LPCTSTR WindowName,
	DWORD Style,
	rect<int> r,
	HWND parent , HMENU menu,
	HINSTANCE handle, LPVOID param);

HWND create_window(
	DWORD exstyle,
	LPCTSTR ClassName, LPCTSTR WindowName,
	DWORD Style,
	rect<int> r,
	HWND parent , HMENU menu,
	HINSTANCE handle, LPVOID param);

HWND create_window(
	DWORD exstyle,
	string ClassName, string WindowName,
	DWORD Style,
	rect<int> r,
	HWND parent , HMENU menu,
	HINSTANCE handle, LPVOID param);

HWND create_window(
	string ClassName, string WindowName,
	DWORD Style,
	rect<int> r,
	HWND parent , HMENU menu,
	HINSTANCE handle, LPVOID param);


} // namespace win32
} // namespace jefuty

#endif    // JEFUTY_WIN32_INCLUDED



