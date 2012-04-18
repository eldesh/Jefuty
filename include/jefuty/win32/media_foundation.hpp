
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

struct MF {
private:
	MF() {
		// Start up Media Foundation platform.
		if (FAILED(::MFStartup(MF_VERSION)))
			throw std::runtime_error("initialize media foundation failed");
	}
	static MF mf;
public:
	static MF & getInstance() {
		static MF mf;
		return mf;
	}
};
MF MF::mf = MF::getInstance();

template <class T> void SafeRelease(T **ppT)
{
    if (*ppT)
    {
        (*ppT)->Release();
        *ppT = NULL;
    }
}


class stream_descriptor ;

// http://msdn.microsoft.com/en-us/library/windows/desktop/ms698961%28v=vs.85%29.aspx
class presentation_descriptor {
public:	
	IMFPresentationDescriptor * desc_m;
	presentation_descriptor (IMFPresentationDescriptor * desc)
		: desc_m(desc)
	{
	}
	DWORD stream_count () const {
		DWORD n;
		if (FAILED(desc_m->GetStreamDescriptorCount(&n)))
			throw std::runtime_error(__FUNCTION__ " fail");
		return n;
	}
	// REVISIT: how to get selected or not
	//stream_descriptor operator[] (DWORD idx) const {	}
};

class media_type_handler {
public:
	IMFMediaTypeHandler * handler_m;
private:
	media_type_handler (IMFMediaTypeHandler * handler) : handler_m(handler)
	{ }
	friend stream_descriptor;
};

class source_resolver {
public:
	IMFSourceResolver * res_m;
	source_resolver() : res_m(nullptr) {
		if (FAILED(::MFCreateSourceResolver(&res_m)))
			throw std::runtime_error("create source resolver fail");
	}
};

class source {
public:
	IMFMediaSource * source_m;
	source (PCWSTR path) : source_m(nullptr) {
		source_resolver res;
		MF_OBJECT_TYPE type = MF_OBJECT_INVALID;
		IUnknown * pSource = nullptr;
		if (FAILED(res.res_m->CreateObjectFromURL(path, MF_RESOLUTION_MEDIASOURCE, NULL, &type, &pSource))) {
			throw std::runtime_error("create obj from url fail");
		}
		if (FAILED(pSource->QueryInterface(IID_PPV_ARGS(&source_m)))) {
			throw std::runtime_error("QueryInterface fail");
		}
		SafeRelease(&res.res_m);
		SafeRelease(&pSource);
	}
	presentation_descriptor create_presentation_descriptor () {
		IMFPresentationDescriptor * desc=nullptr;
		// http://msdn.microsoft.com/en-us/library/windows/desktop/ms702261%28v=vs.85%29.aspx
		// ...The caller must release the interface.
		source_m->CreatePresentationDescriptor(&desc);
		return presentation_descriptor(desc);
	}
};

class stream_descriptor {
public:
	IMFStreamDescriptor * desc_m;
	stream_descriptor (IMFStreamDescriptor * desc) : desc_m(desc) { }
	media_type_handler get_media_type_handler () const {
		IMFMediaTypeHandler * handler;
		if (FAILED(desc_m->GetMediaTypeHandler(&handler)))
			throw std::runtime_error(__FUNCTION__ " fail");
		return media_type_handler(handler);
	}
};


class activate {
public:
	IMFActivate * act_m;
	activate (IMFActivate * act) : act_m(act) { }
};

class topology_node {
public:
	IMFTopologyNode * node_m;
	topology_node(IMFTopologyNode * node) : node_m(node) { }
	topology_node(MF_TOPOLOGY_TYPE type) : node_m(nullptr)
	{
		if (FAILED(::MFCreateTopologyNode(type, &node_m)))
			throw std::runtime_error("create topology fail");
	}
	// Creating Source Nodes
	// http://msdn.microsoft.com/en-us/library/windows/desktop/bb250373%28v=vs.85%29.aspx
	static topology_node make_src_node(source & src, presentation_descriptor & pd, stream_descriptor & sd)
	{
		topology_node node = topology_node(MF_TOPOLOGY_SOURCESTREAM_NODE);
		/// set nodes with attributes
		node.node_m->SetUnknown(MF_TOPONODE_SOURCE                 , src.source_m);
		node.node_m->SetUnknown(MF_TOPONODE_PRESENTATION_DESCRIPTOR, pd.desc_m);
		node.node_m->SetUnknown(MF_TOPONODE_STREAM_DESCRIPTOR      , sd.desc_m);
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
		tnode.node_m->SetObject(act.act_m);
		tnode.node_m->SetUINT32(MF_TOPONODE_STREAMID, id);
		tnode.node_m->SetUINT32(MF_TOPONODE_NOSHUTDOWN_ON_REMOVE, FALSE);
		return tnode;
	}
};

// http://msdn.microsoft.com/en-us/library/windows/desktop/ms695284%28v=vs.85%29.aspx
// connect source topology-node to output topology-node (use output/input streams specified index 0)
HRESULT operator>> (topology_node & src, topology_node & out) {
	return src.node_m->ConnectOutput(0, out.node_m, 0);
}

// http://msdn.microsoft.com/en-us/library/windows/desktop/ms698943%28v=vs.85%29.aspx
// A topology is an object that represents how data flows in the pipeline.
class topology {
public:
	IMFTopology * topology_m;
	topology() : topology_m(nullptr) {
		if (FAILED(::MFCreateTopology(&topology_m)))
			throw std::runtime_error("create topology fail");
	}
	bool add_node(topology_node && node) {
		return SUCCEEDED(topology_m->AddNode(node.node_m));
	}
};

// Media Foundation Attributes
// http://msdn.microsoft.com/en-us/library/windows/desktop/ms696989%28v=vs.85%29.aspx
class attributes {
public:
	IMFAttributes * attr_m;
	attributes (IMFAttributes * attr) : attr_m(attr) { }
};

class session {
	session () ;
public:
	IMFMediaSession * session_m;
	session (topology & topo, attributes const & conf) : session_m(nullptr) {
		if (::MFCreateMediaSession(conf.attr_m, &session_m))
			throw std::runtime_error("create media session fail");
		session_m->SetTopology(0, topo.topology_m);
	}
	session (topology & topo) : session_m(nullptr) {
		if (::MFCreateMediaSession(nullptr, &session_m))
			throw std::runtime_error("create media session fail");
		session_m->SetTopology(0, topo.topology_m);
	}

	// starts the media session
	HRESULT start (GUID const * timefmt, prop_variant const & var) {
		return session_m->Start(timefmt, &var.var_m);
	}
	HRESULT start (prop_variant const & var) {
		return start(nullptr, var);
	}
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
