#if !defined JEFUTY_WIN32_WINDOW_HANDLE_T_INCLUDED
#define      JEFUTY_WIN32_WINDOW_HANDLE_T_INCLUDED

#include <windows.h>

#include <boost/tuple/tuple.hpp>
#include <boost/optional.hpp>

#include "../../jefuty/polygon.hpp"
#include "string.hpp"

namespace jefuty { namespace win32 {

class window_handle_t;

typedef HWND handle_type;

class device_ctx_deleter {
	handle_type & hwnd_m;
public:
	device_ctx_deleter (HWND & hwnd) : hwnd_m(hwnd) { }
	void operator() (HDC * dev) const {
		if (dev)
			::ReleaseDC(hwnd_m, *dev);
		delete dev;
	}
};

class end_paint {
	handle_type & handle_m;
public:
	end_paint(handle_type & handle)
		: handle_m(handle)
	{ }
	void operator() (boost::tuple<HDC, PAINTSTRUCT> * dev) const {
		::EndPaint(handle_m, &dev->get<1>());
		delete dev;
	}
};


void print(HDC hdc, point<int> const & coord, string const & str);
window_handle_t operator<< (window_handle_t hwnd, string      const & str);
window_handle_t operator<< (window_handle_t hwnd, std::string const & str);

window_handle_t find_toplevel_window (string const & class_name, string const & window_name);
window_handle_t find_toplevel_window (string const & window_name);


///
/// window handle wrapper class
///////////////////////////////////////////////////////////////////////////////////////////////

class window_handle_t /*: public boost::noncopyable */{
	handle_type handle_m;
public:
	explicit window_handle_t(handle_type handle)
		: handle_m(handle)
	{ }
	window_handle_t(window_handle_t && h) : handle_m(h.handle_m)
	{ }
	std::unique_ptr<HDC, device_ctx_deleter> device_ctx ()
	{
		return std::unique_ptr<HDC, device_ctx_deleter>
			(new HDC(::GetDC(handle_m)), device_ctx_deleter(handle_m));
	}
private:
	boost::tuple<HDC, PAINTSTRUCT> begin_paint_impl () {
		PAINTSTRUCT p;
		HDC hdc = ::BeginPaint(handle_m, &p);
		return boost::make_tuple(hdc, p);
	}
public:
	std::unique_ptr<boost::tuple<HDC, PAINTSTRUCT>, end_paint>
	begin_paint () {
		return std::unique_ptr<boost::tuple<HDC, PAINTSTRUCT>, end_paint>
			(new boost::tuple<HDC, PAINTSTRUCT>(begin_paint_impl()), end_paint(handle_m));
	}
	window_handle_t find_child_window (string const & class_name, string const & window_name) {
		return window_handle_t(::FindWindowEx(handle_m, nullptr, class_name.c_str(), window_name.c_str()));
	}
	window_handle_t find_child_window (string const & class_name) {
		return window_handle_t(::FindWindowEx(handle_m, nullptr, class_name.c_str(), nullptr));
	}
	boost::optional<string> class_name() const {
		TCHAR ts[256];
		if (::GetClassName(handle_m, ts, sizeof(ts)/sizeof(ts[0])))
			return string(ts, ts+::lstrlen(ts));
		else
			return boost::optional<string>();
	}
	// minimizes (but does not destroy) the specifiec window
	void close (void) {
		if (::CloseWindow(handle_m) == 0)
			std::runtime_error(__FUNCTION__ " fail");
	}
	// TODO: safe_bool
	operator bool () const {
		return handle_m != nullptr;
	}
	// DEPRICATED: for interoperate with raw-WindowsAPI
	operator handle_type () const { return handle_m; }

	static void destroy (window_handle_t h) {
		::DestroyWindow(h);
	}
	//~window_handle_t () { ::DestroyWindow(handle_m); }

	/*
	void for_each_children (boost::function<bool(window_handle_t, LPARAM)> f, LPARAM lp) {
		ignore(::EnumChildWindows(handle_m, [&f,lp](HWND h) -> bool { return f(window_handle_t(h), lp); }, lp));
	}
	*/
	boost::optional<string> get_text (void) const {
		TCHAR ts[256];
		if (int size = ::GetWindowText(handle_m, ts, sizeof(ts)/sizeof(ts[0]))) {
			return string(ts, ts+size);
		} else {
			return boost::optional<string>();
		}
	}
	/// get rectangle area of window on absolute coordinate
	rect<LONG> get_rect () const {
		RECT r;
		if (!::GetWindowRect(handle_m, &r))
			throw std::runtime_error(__FUNCTION__ " fail");
		return rect<LONG>(r.left, r.top, width(r), height(r));
	}
	window_handle_t parent () const {
		return window_handle_t(::GetParent(handle_m));
	}
	/// get relative coordinate from parent window
	point<LONG> relative_coord () const {
		POINT p = {0};
		::MapWindowPoints(handle_m, parent(), &p, 1);
		return point<LONG>(p.x, p.y);
	}
	template<typename T>
	void move_to (rect<T> const & r) {
		point<T> p = r.origin();
		::MoveWindow(handle_m, p.x(), p.y(), width(r), height(r), TRUE);
	}
	template<typename T>
	void move_to (point<T> const & p) {
		rect<LONG> r = get_rect();
		::MoveWindow(handle_m, p.x(), p.y(), width(r), height(r), TRUE);
	}
	/// expand or reduce window to WxH
	void scale_to (LONG w, LONG h) {
		point<LONG> p = relative_coord();
		::MoveWindow(handle_m, p.x(), p.y(), w, h, TRUE);
	}

private:

	template<typename T>
	bool set_window_pos (window_handle_t insert_after, rect<T> const & r, UINT flags) {
		return ::SetWindowPos(handle_m, insert_after.handle_m
							, r.origin().x(), r.origin().y()
							, width(r), height(r), 0) == TRUE;
	}

public:

	/// A handle to the window to precede the positioned window in the Z order
	bool insert_after (window_handle_t hwnd) {
		assert(hwnd);
		return set_window_pos(hwnd, get_rect(), 0);
	}

	/// Places the window above all non-topmost top-most windows.
	/// The window maintains its topmost position even when it is deactivated.
	bool topmost () {
		return set_window_pos(window_handle_t(HWND_TOPMOST)  , get_rect(), 0);
	}
	bool no_topmost () {
		return set_window_pos(window_handle_t(HWND_NOTOPMOST), get_rect(), 0);
	}

	/// Places the window at the top of the Z order.
	bool top () {
		return set_window_pos(window_handle_t(HWND_TOP)      , get_rect(), 0);
	}
	bool bottom () {
		return set_window_pos(window_handle_t(HWND_BOTTOM)   , get_rect(), 0);
	}

	/// activates a window.
	/// return the handle to the window that previously active
	window_handle_t active () {
		return window_handle_t(::SetActiveWindow(handle_m));
	}

	/// sets the keyboard focus to the specified window.
	/// return the handle to the window that previous had keyboard focus
	window_handle_t focus () {
		return window_handle_t(::SetFocus(handle_m));
	}

	friend window_handle_t operator<< (window_handle_t hwnd, string const & str);
};

} // namespace win32
} // namespace jefuty

#endif    // JEFUTY_WIN32_WINDOW_HANDLE_T_INCLUDED

