#pragma once

#include <exdisp.h>
#include <exdispid.h>

class ioWebBrowser;

class ioBrowserEvents : public DWebBrowserEvents2
{
protected:
	ioWebBrowser *m_pBrowser;

//IUnknown
public:
	virtual ULONG STDMETHODCALLTYPE AddRef();
	virtual ULONG STDMETHODCALLTYPE Release();
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID, __RPC_FAR void* __RPC_FAR*);

//IDispatch
public:
	virtual HRESULT STDMETHODCALLTYPE GetTypeInfoCount(UINT __RPC_FAR *pctinfo);
	virtual HRESULT STDMETHODCALLTYPE GetTypeInfo(UINT,LCID,ITypeInfo __RPC_FAR *__RPC_FAR*);
	virtual HRESULT STDMETHODCALLTYPE GetIDsOfNames(REFIID,LPOLESTR __RPC_FAR*,UINT,LCID,DISPID __RPC_FAR*);
	virtual HRESULT STDMETHODCALLTYPE Invoke(DISPID,
											 REFIID,
											 LCID,
											 WORD,
											 DISPPARAMS __RPC_FAR *,
											 VARIANT __RPC_FAR *,
											 EXCEPINFO __RPC_FAR *,
											 UINT __RPC_FAR *);

public:
	ioBrowserEvents( ioWebBrowser *pBrowser );
	virtual ~ioBrowserEvents();
};

