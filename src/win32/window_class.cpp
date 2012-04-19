#include <jefuty/win32/window_class.hpp>


namespace jefuty { namespace win32 {

bool register_class (window_class & wnd) {
	return RegisterClassEx(&wnd.wnd_m) != FALSE;
}

} // namespace win32
} // namespace jefuty
