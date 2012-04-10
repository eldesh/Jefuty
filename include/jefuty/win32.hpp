#if !defined JEFUTY_WIN32_INCLUDED
#define      JEFUTY_WIN32_INCLUDED

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

namespace detail {

WNDCLASSEX make_wnd_class (HINSTANCE instance, jefuty::win32::string const & winclass, WNDPROC proc, UINT style) ;

} // namespace detail

using detail::make_wnd_class;

class window_class : public boost::noncopyable {
	//typedef LRESULT (* proc_type)(HWND, UINT, WPARAM, LPARAM);
	typedef WNDPROC proc_type;
	WNDCLASSEX wnd_class_m;
public:
	window_class::window_class(WNDCLASSEX wnd)
		: wnd_class_m(wnd)
	{
	}
	window_class::window_class(HINSTANCE instance, string const & winclass, proc_type proc, UINT style)
		: wnd_class_m(detail::make_wnd_class(instance, winclass, proc, style))
	{
	}
	friend bool register_class(window_class &);
};

bool register_class (WNDCLASSEX & wnd);
bool register_class (window_class & wnd);

} // namespace win32
} // namespace jefuty

#endif    // JEFUTY_WIN32_INCLUDED



