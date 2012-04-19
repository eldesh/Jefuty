#if !defined JEFUTY_WIN32_WINDOW_CLASS_INCLUDED
#define      JEFUTY_WIN32_WINDOW_CLASS_INCLUDED

#include <windows.h>
#include <boost/noncopyable.hpp>
#include "string.hpp"

namespace jefuty { namespace win32 {

class window_class : public boost::noncopyable {
	typedef WNDPROC proc_type;
	WNDCLASSEX wnd_m;
public:
	window_class::window_class(WNDCLASSEX wnd) : wnd_m(wnd)
	{
	}
	window_class::window_class(
		UINT      style,
		WNDPROC   proc,
		HINSTANCE instance,
		HICON     icon,
		HCURSOR   cursor,
		HBRUSH    background,
		string const & menu_name,
		string const & class_name,
		HICON          iconsm)
	{
		wnd_m.cbSize = sizeof(WNDCLASSEX);
		wnd_m.style          = style;
		wnd_m.lpfnWndProc    = proc;
		wnd_m.cbClsExtra     = 0;
		wnd_m.cbWndExtra     = sizeof(void*); // sizeof 'this' pointer
		wnd_m.hInstance      = instance;
		wnd_m.hIcon          = LoadIcon(instance, MAKEINTRESOURCE(IDI_APPLICATION));
		wnd_m.hCursor        = LoadCursor(nullptr, IDC_ARROW);
		wnd_m.hbrBackground  = reinterpret_cast<HBRUSH>(COLOR_WINDOW+1);
		wnd_m.lpszMenuName   = menu_name.c_str();
		wnd_m.lpszClassName  = class_name.c_str();
		wnd_m.hIconSm        = LoadIcon(wnd_m.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
	}
	friend bool register_class(window_class &);
};

bool register_class (window_class & wnd);


} // namespace win32
} // namespace jefuty

#endif    /* JEFUTY_WIN32_WINDOW_CLASS_INCLUDED */
