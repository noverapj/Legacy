

#include "stdafx.h"

#include <oleauto.h>

#include "WellKnownURL.h"
#include "ioWebBrowser.h"
#include "ioBrowserEvents.h"

ioBrowserEvents::ioBrowserEvents( ioWebBrowser *pBrowser )
{
	m_pBrowser = pBrowser;
}

ioBrowserEvents::~ioBrowserEvents()
{
	m_pBrowser = NULL;
}

ULONG STDMETHODCALLTYPE ioBrowserEvents::AddRef()
{
	return 1;
}

ULONG STDMETHODCALLTYPE ioBrowserEvents::Release()
{
	return 0;
}

HRESULT STDMETHODCALLTYPE ioBrowserEvents::QueryInterface(REFIID riid,void __RPC_FAR *__RPC_FAR *ppvObject)
{
	*ppvObject = NULL;

	if( IID_IUnknown == riid )
	{ 
		*ppvObject = (IUnknown*)(DWebBrowserEvents2*)this;
	}
	else if( DIID_DWebBrowserEvents2 == riid )
	{ 
		*ppvObject = (DWebBrowserEvents2*)this; 
	}
	else if( IID_IDispatch == riid )
	{
		*ppvObject = (IDispatch*)this;
	}

	if( *ppvObject )
		return S_OK;

	return E_NOINTERFACE;
}

HRESULT STDMETHODCALLTYPE ioBrowserEvents::GetTypeInfoCount(UINT __RPC_FAR *pctinfo)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE ioBrowserEvents::GetTypeInfo(UINT,LCID,ITypeInfo __RPC_FAR *__RPC_FAR*)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE ioBrowserEvents::GetIDsOfNames(REFIID,LPOLESTR __RPC_FAR*,UINT,LCID,DISPID __RPC_FAR*)
{
	return E_NOTIMPL;
}

HRESULT ioBrowserEvents::Invoke( DISPID dispIdMember,
								 REFIID riid,
								 LCID lcid,
								 WORD wFlags,
								 DISPPARAMS __RPC_FAR *pDispParams,
								 VARIANT __RPC_FAR *pVarResult,
								 EXCEPINFO __RPC_FAR *pExcepInfo,
								 UINT __RPC_FAR *puArgErr)
{
	if( !m_pBrowser )
		return S_FALSE;
	if( !pDispParams )
		return S_FALSE;

	switch( dispIdMember )
	{
	case DISPID_BEFORENAVIGATE2:
		ErrorReport::SetPosition( 7000, 1 );
		m_pBrowser->OnBeforeNavigate2( pDispParams->rgvarg[6].pdispVal,    // pDisp
									   pDispParams->rgvarg[5].pvarVal,     // url
									   pDispParams->rgvarg[4].pvarVal,     // Flags
									   pDispParams->rgvarg[3].pvarVal,     // TargetFrameName
									   pDispParams->rgvarg[2].pvarVal,     // PostData
									   pDispParams->rgvarg[1].pvarVal,     // Headers
									   pDispParams->rgvarg[0].pboolVal );  // Cancel
		ErrorReport::SetPosition( 7000, 2 );
		break;
	case DISPID_NAVIGATECOMPLETE2:
		ErrorReport::SetPosition( 7000, 3 );
		m_pBrowser->OnNavigateComplete2(pDispParams->rgvarg[1].pdispVal, pDispParams->rgvarg[0].pvarVal);
		ErrorReport::SetPosition( 7000, 4 );
		break;
	case DISPID_TITLECHANGE:
		ErrorReport::SetPosition( 7000, 5 );
		m_pBrowser->OnTitleChange( pDispParams->rgvarg[0].bstrVal );
		ErrorReport::SetPosition( 7000, 6 );
		break;
	case DISPID_CLIENTTOHOSTWINDOW:
		ErrorReport::SetPosition( 7000, 7 );
		m_pBrowser->OnClientToHostWindow(pDispParams->rgvarg[1].plVal, pDispParams->rgvarg[0].plVal);
		ErrorReport::SetPosition( 7000, 8 );
		break;
	case DISPID_DOCUMENTCOMPLETE:
		ErrorReport::SetPosition( 7000, 9 );
		m_pBrowser->OnDocumentComplete(pDispParams->rgvarg[1].pdispVal, pDispParams->rgvarg[0].pvarVal);
		ErrorReport::SetPosition( 7000, 10 );
		break;
	case DISPID_NEWWINDOW2:
		ErrorReport::SetPosition( 7000, 11 );
		m_pBrowser->OnNewWindow( pDispParams->rgvarg[1].ppdispVal, pDispParams->rgvarg[0].pboolVal );
		ErrorReport::SetPosition( 7000, 12 );
		break;
	case DISPID_WINDOWCLOSING:
		ErrorReport::SetPosition( 7000, 13 );
		m_pBrowser->OnWindowClosing( pDispParams->rgvarg[1].boolVal, pDispParams->rgvarg[0].pboolVal );
		ErrorReport::SetPosition( 7000, 14 );
		break;
	case DISPID_ONQUIT:
	case DISPID_COMMANDSTATECHANGE:
	case DISPID_DOWNLOADBEGIN:
	case DISPID_DOWNLOADCOMPLETE:
	case DISPID_FILEDOWNLOAD:
	case DISPID_NAVIGATEERROR:
		break;
	}

	return S_OK;
}
