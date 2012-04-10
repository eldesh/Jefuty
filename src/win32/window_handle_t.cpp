#include <windows.h>

#include <jefuty/win32/string.hpp>
#include <jefuty/win32/window_handle_t.hpp>

namespace jefuty { namespace win32 {

void print(HDC hdc, point<int> const & coord, string const & str) {
	::TextOut(hdc, coord.x(), coord.y(), str.c_str(), ::_tcslen(str.c_str()));
}

window_handle_t operator<< (window_handle_t hwnd, string const & str) {
	::SetWindowText(hwnd.handle_m, str.c_str());
	return hwnd;
}
window_handle_t operator<< (window_handle_t hwnd, std::string const & str) {
	return hwnd << of_string(str.c_str());
}


window_handle_t find_toplevel_window (string const & class_name, string const & window_name) {
	return window_handle_t(::FindWindow(class_name.c_str(), window_name.c_str()));
}
window_handle_t find_toplevel_window (string const & window_name) {
	return window_handle_t(::FindWindow(nullptr, window_name.c_str()));
}


} // namespace win32
} // namespace jefuty

