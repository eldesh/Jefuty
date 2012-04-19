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

} // namespace win32
} // namespace jefuty

