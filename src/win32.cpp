#include <windows.h>

#include <jefuty/win32.hpp>
#include <jefuty/win32/string.hpp>
#include <jefuty/win32/window_handle_t.hpp>

namespace jefuty { namespace win32 {


HWND create_window(
	LPCTSTR ClassName,
	LPCTSTR WindowName,
	DWORD Style,
	rect<int> r,
	HWND parent , HMENU menu,
	HINSTANCE handle, LPVOID param)
{
	return CreateWindow(ClassName, WindowName, Style
						, r.origin().x(), r.origin().y(), width(r), height(r)
						, parent, menu, handle, param);
}

HWND create_window(
	DWORD exstyle,
	LPCTSTR ClassName, LPCTSTR WindowName,
	DWORD Style,
	rect<int> r,
	HWND parent , HMENU menu,
	HINSTANCE handle, LPVOID param)
{
	return CreateWindowEx(exstyle, ClassName, WindowName, Style
						, r.origin().x(), r.origin().y(), width(r), height(r)
						, parent, menu, handle, param);
}

HWND create_window(
	DWORD exstyle,
	string ClassName, string WindowName,
	DWORD Style,
	rect<int> r,
	HWND parent , HMENU menu,
	HINSTANCE handle, LPVOID param)
{
	return CreateWindowEx(exstyle, ClassName.c_str(), WindowName.c_str(), Style
						, r.origin().x(), r.origin().y(), width(r), height(r)
						, parent, menu, handle, param);
}


HWND create_window(
	string ClassName, string WindowName,
	DWORD Style,
	rect<int> r,
	HWND parent , HMENU menu,
	HINSTANCE handle, LPVOID param)
{
	return CreateWindow(ClassName.c_str(), WindowName.c_str(), Style
						, r.origin().x(), r.origin().y(), width(r), height(r)
						, parent, menu, handle, param);
}

namespace detail {

WNDCLASSEX make_wnd_class (HINSTANCE instance, jefuty::win32::string const & winclass, WNDPROC proc, UINT style) {
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style          = style;
	wcex.lpfnWndProc    = proc;
	wcex.cbClsExtra     = 0;
	wcex.cbWndExtra     = 0;
	wcex.hInstance      = instance;
	wcex.hIcon          = LoadIcon(instance, MAKEINTRESOURCE(IDI_APPLICATION));
	wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName   = NULL;
	wcex.lpszClassName  = winclass.c_str();
	wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
	return wcex;
}

} // namespace detail

bool register_class (WNDCLASSEX & wnd) {
	return RegisterClassEx(&wnd) != FALSE;
}
bool register_class (window_class & wnd) {
	return RegisterClassEx(&wnd.wnd_class_m) != FALSE;
}


} // namespace win32
} // namespace jefuty

