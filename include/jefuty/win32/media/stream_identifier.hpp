#if !defined JEFUTY_MF_STREAM_IDENTIFIER_INCLUDED
#define      JEFUTY_MF_STREAM_IDENTIFIER_INCLUDED

#include <iostream>
#include <WinDef.h>

namespace jefuty { namespace win32 { namespace media {

// http://msdn.microsoft.com/en-us/library/windows/desktop/ms703852%28v=vs.85%29.aspx
// The stream identifier uniquely identifies a stream within a presentation.
// It does not change throughout the lifetime of the stream.
// stream identifier *IS NOT* the index number of stream.s
class stream_identifier {
private:
	DWORD id_m;
	stream_identifier ();
	stream_identifier (DWORD id) : id_m(id) // construct only from stream-descriptor
	{
	}
	friend class stream_descriptor;
	friend bool operator< (stream_identifier const & lhs, stream_identifier const & rhs);
};

bool operator< (stream_identifier const & lhs, stream_identifier const & rhs) {
	return lhs.id_m < rhs.id_m;
}
bool operator> (stream_identifier const & lhs, stream_identifier const & rhs) {
	return rhs < lhs;
}
bool operator<=(stream_identifier const & lhs, stream_identifier const & rhs) {
	return !(lhs > rhs);
}
bool operator>=(stream_identifier const & lhs, stream_identifier const & rhs) {
	return !(lhs < rhs);
}
bool operator==(stream_identifier const & lhs, stream_identifier const & rhs) {
	return !(lhs < rhs) && !(rhs < lhs);
}
bool operator!=(stream_identifier const & lhs, stream_identifier const & rhs) {
	return !(lhs == rhs);
}

} // namespace media
} // namespace win32
} // namespace jefuty

#endif    /* JEFUTY_MF_STREAM_IDENTIFIER_INCLUDED */
