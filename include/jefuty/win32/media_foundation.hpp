/****
 *
 *  Wrapper library for audio/video operations on Windows (Vista or later)
 *
 *
 *  Microsoft Media Foundation
 *      http://msdn.microsoft.com/en-us/library/windows/desktop/ms694197%28v=vs.85%29.aspx
 *
 ****/

#if !defined JEFUTY_MEDIAFOUNDATION_INCLUDED
#define      JEFUTY_MEDIAFOUNDATION_INCLUDED

#include <iostream>
#include <string>
#include <vector>
#include <memory>

#include <windows.h>
#include <shobjidl.h> 
#include <shlwapi.h>
#include <assert.h>
#include <strsafe.h>

// Media Foundation headers
#include <mfapi.h>
#include <mfidl.h>
#include <mferror.h>
#include <evr.h>

#include <boost/smart_ptr/intrusive_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <boost/optional.hpp>

namespace {
	// used for managing reference count of COM object by boost/intrusive_ptr
	void intrusive_ptr_add_ref(IUnknown * p) { p->AddRef() ; }
	void intrusive_ptr_release(IUnknown * p) { p->Release(); }
} // namespace `anonymouse`

namespace std {
	template<>
	struct default_delete<IUnknown>
	{
   		void operator() (IUnknown * p) const {
			if (p) p->Release();
 		}
 	};
} // namespace std

namespace jefuty { namespace win32 {

class prop_variant {
public:
	PROPVARIANT var_m;
	prop_variant () {
		// http://msdn.microsoft.com/en-us/library/windows/desktop/aa380293%28v=vs.85%29.aspx
		::PropVariantInit(&var_m); // Initializes a PROPVARIANT structure, and sets its type to VT_EMPTY
	}
	~prop_variant () {
		::PropVariantClear(&var_m);
	}
};


namespace media {

struct MF : private boost::noncopyable {
private:
	MF() {
		// Start up Media Foundation platform.
		if (FAILED(::MFStartup(MF_VERSION)))
			throw std::runtime_error("initialize media foundation failed");
	}
	~MF() {
		MFShutdown();
	}
	static MF mf;
public:
	static MF & getInstance() {
		static MF mf;
		return mf;
	}
};
MF MF::mf; // = MF::getInstance();

template<typename T>
class mf_wrapper {
public:
	typedef T wrapped_type;
protected:
	boost::intrusive_ptr<wrapped_type> impl_m; // don't publish from derived class
	mf_wrapper (wrapped_type * p) : impl_m(p)
	{
	}
public:
	// publish the raw pointer for interoperability
	wrapped_type const * get () const { return impl_m.get(); }
	wrapped_type       * get ()       { return impl_m.get(); }
};

class stream_descriptor ;

// http://msdn.microsoft.com/en-us/library/windows/desktop/ms698961%28v=vs.85%29.aspx
class presentation_descriptor : public mf_wrapper<IMFPresentationDescriptor> {
public:
	typedef mf_wrapper<IMFPresentationDescriptor> base_type;
	presentation_descriptor (wrapped_type * p) : base_type(p)
	{
	}
	DWORD stream_count () const {
		DWORD n;
		wrapped_type * p = const_cast<wrapped_type*>(get());
		if (FAILED(p->GetStreamDescriptorCount(&n)))
			throw std::runtime_error(__FUNCTION__ " fail");
		return n;
	}
	// REVISIT: how to get selected or not
	//stream_descriptor operator[] (DWORD idx) const {	}
};

class media_type_handler : public mf_wrapper<IMFMediaTypeHandler> {
public:
	typedef mf_wrapper<IMFMediaTypeHandler> base_type;

	GUID major_type () const {
		GUID t;
		if (FAILED(const_cast<wrapped_type*>(get())->GetMajorType(&t)))
			throw std::runtime_error("get major type fail");
		return t;
	}
private:
	media_type_handler (wrapped_type * handler) : base_type(handler)
	{
	}
	friend stream_descriptor; // only allow stream_desc to construct
};

class source_resolver : public mf_wrapper<IMFSourceResolver> {
public:
	typedef mf_wrapper<IMFSourceResolver> base_type;
	source_resolver() : base_type(nullptr) {
		wrapped_type * p = nullptr;
		if (FAILED(::MFCreateSourceResolver(&p)))
			throw std::runtime_error("create source resolver fail");
		impl_m.reset(p);
	}
};

class source : public mf_wrapper<IMFMediaSource> {
public:
	typedef mf_wrapper<IMFMediaSource> base_type;
private:
	source (wrapped_type * p) : base_type(p)
	{
	}
public:
	presentation_descriptor create_presentation_descriptor () {
		IMFPresentationDescriptor * desc=nullptr;
		// http://msdn.microsoft.com/en-us/library/windows/desktop/ms702261%28v=vs.85%29.aspx
		// ...The caller must release the interface.
		get()->CreatePresentationDescriptor(&desc);
		return presentation_descriptor(desc);
	}
	static boost::optional<source> create(PCWSTR path) {
		source_resolver res;
		MF_OBJECT_TYPE type = MF_OBJECT_INVALID;
		IUnknown * src_ = nullptr;
		if (SUCCEEDED(res.get()->CreateObjectFromURL(path, MF_RESOLUTION_MEDIASOURCE, NULL, &type, &src_))) {
			std::unique_ptr<IUnknown> src(src_);
			IMFMediaSource * p = nullptr;
			if (SUCCEEDED(src->QueryInterface(IID_PPV_ARGS(&p))))
				return boost::optional<source>(source(p));
		}
		return boost::none;
	}
};

class stream_descriptor : public mf_wrapper<IMFStreamDescriptor> {
public:
	typedef mf_wrapper<IMFStreamDescriptor> base_type;
	stream_descriptor (wrapped_type * desc) : base_type(desc)
	{ }
	media_type_handler get_media_type_handler () const {
		IMFMediaTypeHandler * handler;
		if (FAILED(const_cast<wrapped_type*>(get())->GetMediaTypeHandler(&handler)))
			throw std::runtime_error(__FUNCTION__ " fail");
		return media_type_handler(handler);
	}
};


class activate : public mf_wrapper<IMFActivate> {
public:
	typedef mf_wrapper<IMFActivate> base_type;
	activate (wrapped_type * act) : base_type(act)
	{
	}
	static activate create_audio_renderer() {
		wrapped_type * p(nullptr);
		::MFCreateAudioRendererActivate(&p);
		return activate(p);
	}
	static activate create_video_renderer(HWND hwnd) { // TODO: take wrapped handler
		wrapped_type * p(nullptr);
		::MFCreateVideoRendererActivate(hwnd, &p);
		return activate(p);
	}
};

class topology_node : public mf_wrapper<IMFTopologyNode> {
public:
	typedef mf_wrapper<IMFTopologyNode> base_type;
	topology_node(wrapped_type * node) : base_type(node)
	{
	}
	topology_node(MF_TOPOLOGY_TYPE type) : base_type(nullptr)
	{
		IMFTopologyNode * p=nullptr;
		if (FAILED(::MFCreateTopologyNode(type, &p)))
			throw std::runtime_error("create topology fail");
		impl_m.reset(p);
	}
	// Creating Source Nodes
	// http://msdn.microsoft.com/en-us/library/windows/desktop/bb250373%28v=vs.85%29.aspx
	static topology_node make_src_node(source & src, presentation_descriptor & pd, stream_descriptor & sd)
	{
		topology_node node = topology_node(MF_TOPOLOGY_SOURCESTREAM_NODE);
		/// set nodes with attributes
		node.get()->SetUnknown(MF_TOPONODE_SOURCE                 , src.get());
		node.get()->SetUnknown(MF_TOPONODE_PRESENTATION_DESCRIPTOR,  pd.get());
		node.get()->SetUnknown(MF_TOPONODE_STREAM_DESCRIPTOR      ,  sd.get());
		return node;
	}
	// Creating Output Nodes
	//   - Creating an Output Node from an Activation Object
	//     you must use this way to load the topology inside the PMP
	//     where
	//         PMP = Protected Media Path(=encrypted)
	// http://msdn.microsoft.com/en-us/library/windows/desktop/bb250377%28v=vs.85%29.aspx
	static topology_node make_out_node(activate & act, DWORD id) {
		topology_node tnode(MF_TOPOLOGY_OUTPUT_NODE);
		tnode.get()->SetObject(act.get());
		tnode.get()->SetUINT32(MF_TOPONODE_STREAMID, id);
		tnode.get()->SetUINT32(MF_TOPONODE_NOSHUTDOWN_ON_REMOVE, FALSE);
		return tnode;
	}
};

// http://msdn.microsoft.com/en-us/library/windows/desktop/ms695284%28v=vs.85%29.aspx
// connect source topology-node to output topology-node (use output/input streams specified index 0)
HRESULT operator>> (topology_node & src, topology_node & out) {
	return src.get()->ConnectOutput(0, out.get(), 0);
}

// http://msdn.microsoft.com/en-us/library/windows/desktop/ms698943%28v=vs.85%29.aspx
// A topology is an object that represents how data flows in the pipeline.
class topology : public mf_wrapper<IMFTopology> {
public:
	typedef mf_wrapper<IMFTopology> base_type;
	topology() : base_type(nullptr) {
		IMFTopology * p=nullptr;
		if (FAILED(::MFCreateTopology(&p)))
			throw std::runtime_error("create topology fail");
		impl_m.reset(p);
	}
	bool add_node(topology_node && node) {
		return SUCCEEDED(get()->AddNode(node.get()));
	}
};

// Media Foundation Attributes
// http://msdn.microsoft.com/en-us/library/windows/desktop/ms696989%28v=vs.85%29.aspx
class attributes : public mf_wrapper<IMFAttributes> {
public:
	typedef mf_wrapper<IMFAttributes> base_type;
	attributes (wrapped_type * p) : base_type(p)
	{ }
};

class session : public mf_wrapper<IMFMediaSession> {
	session () ;
public:
	typedef mf_wrapper<IMFMediaSession> base_type;
	session (topology & topo, attributes const & conf) : base_type(nullptr) {
		IMFMediaSession * p=nullptr;
		if (::MFCreateMediaSession(const_cast<attributes::wrapped_type*>(conf.get()), &p))
			throw std::runtime_error("create media session fail");
		impl_m.reset(p);
		impl_m->SetTopology(0, topo.get());
	}
	session (topology & topo) : base_type(nullptr) {
		IMFMediaSession * p=nullptr;
		if (::MFCreateMediaSession(nullptr, &p))
			throw std::runtime_error("create media session fail");
		impl_m.reset(p);
		impl_m->SetTopology(0, topo.get());
	}

	// starts the media session
	HRESULT start (GUID const * timefmt, prop_variant const & var) {
		return impl_m->Start(timefmt, &var.var_m);
	}
	HRESULT start (prop_variant const & var) {
		return start(nullptr, var);
	}
	void stop  () { impl_m->Stop();	 }
	void pause () { impl_m->Pause(); }
	// ~session () { impl_m->Shutdown(); }
};

class video_disp_ctrl {
public:
	IMFVideoDisplayControl * ctrl_m;
//	video_disp_ctrl(HWND hwnd) 
};

} // namespace media

} // namespace win32
} // namespace jefuty

#endif    /* JEFUTY_MEDIAFOUNDATION_INCLUDED */
