

#include "stdafx.h"

#include "ioRenderOperation.h"
#include "ioLight.h"

#include "ioTexture.h"
#include "ioRenderTexture.h"

#include "ioShader.h"
#include "ioTextureUnitState.h"
#include "ioPass.h"
#include "ioMaterial.h"

#include "ioFontManager.h"
#include "ioShaderManager.h"
#include "ioTextureManager.h"

#include "ioMeshData.h"
#include "ioVertexDeclaration.h"
#include "ioVertexBufferBinder.h"
#include "ioVertexBufferInstance.h"
#include "ioIndexBufferInstance.h"

#include "ioVertexBufferManager.h"
#include "ioIndexBufferManager.h"
#include "ErrorReport.h"

#include "HelpFunc.h"
#include "ioRenderStateDesc.h"
#include "ioEnumDisplayMode.h"
#include "ioCPU.h"

#include "ioRenderSystem.h"

D3DCAPS9 ioRenderSystem::m_D3DCaps;
D3DSURFACE_DESC ioRenderSystem::m_BackSurfaceDesc;
BufferUsage ioRenderSystem::m_NoneAutoBufferUsage = BU_HARDWARE;

static ioRenderStateDesc s_RenderStateDesc;

ioRenderSystem::ioRenderSystem()
{
	m_pD3D = NULL;
	m_pD3DDevice = NULL;
	m_dwDeviceBehaviorFlags = 0;

	m_pRenderTarget = NULL;
	m_pDepthSurface = NULL;
	m_pCaptureTarget = NULL;

	InitializeCriticalSection( &m_CaptureSection );

	m_pEnumDisplay = new ioEnumDisplayMode;

	ZeroMemory( &m_Ambient, sizeof(m_Ambient) );
	m_iUsedLights = 0;

	D3DXMatrixIdentity( &m_matModel );
	D3DXMatrixIdentity( &m_matView );
	D3DXMatrixIdentity( &m_matProj );

	m_hWnd = 0;
	m_uClientWidth  = 0;
	m_uClientHeight = 0;
	
	m_bNowMultiSampleOn = false;

	m_bDeviceLost = false;
	m_bSoftwareProcessing = false;

	m_bFogEnable	= false;
	m_fFogStart		=FLOAT1000;
	m_fFogEnd		= 10000.0f;
	m_fFogConstants = 1.0f / ( m_fFogEnd - m_fFogStart );
	m_dwFogColor	= 0xff7f7f7f;

	ZeroMemory( &m_ViewPort, sizeof(m_ViewPort) );

	ZeroMemory( &m_CurGamma, sizeof(m_CurGamma) );
	m_iCurBright = 0;
	m_iCurContrast = 0;

	m_ShadowMapType = SMT_NOT_ENABLE;

	m_iRenderedFaceCount = 0;
	m_iRenderCallCount = 0;

	m_GlobalFillMode = FM_SOLID;
	m_bSetGlobalFillMode = false;

	m_bDialogBoxMode = false;
	m_bNowDeviceResetted = false;
	m_bCaptureShotSafe = false;
}

ioRenderSystem::~ioRenderSystem()
{
	DeleteCriticalSection( &m_CaptureSection );

	SAFEDELETE(m_pEnumDisplay);

	SAFERELEASE(m_pCaptureTarget);
	SAFERELEASE(m_pD3DDevice);
	SAFERELEASE(m_pD3D);
}

void ioRenderSystem::SetMinDisplayMode( UINT uMinWidth, UINT uMinHeight )
{
	m_pEnumDisplay->SetMinWidthHeight( uMinWidth, uMinHeight );
}

int ioRenderSystem::GetNumEnableDisplayMode() const
{
	return m_pEnumDisplay->GetNumDisplayMode( m_D3DParam.BackBufferFormat );
}

bool ioRenderSystem::GetDisplayResolution( int iIndex, UINT &uWidth, UINT &uHeight ) const
{
	return m_pEnumDisplay->GetResolution( m_D3DParam.BackBufferFormat,
									      iIndex,
										  uWidth,
										  uHeight );
}

bool ioRenderSystem::CreateDirect3D()
{
	SAFERELEASE(m_pD3D);

	if( ::GetFocus() != m_hWnd )
		::SetFocus( m_hWnd );

	m_pD3D = Direct3DCreate9( D3D_SDK_VERSION );
	if( NULL == m_pD3D )
	{
		ErrorReport::SetEnginePos( 5, 12 );
		LOG.PrintTimeAndLog( 0, "ioRenderSystem::CreateDirect3D - Direct3DCreate9 FAILED" );
		return false;
	}

	ZeroMemory( &m_D3DCaps, sizeof(m_D3DCaps) );

	HRESULT hr = m_pD3D->GetDeviceCaps( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &m_D3DCaps );
	if( FAILED(hr) )
	{
		ErrorReport::SetEnginePos( 5, 13 );
		ErrorReport::SetFormat( "%x", hr );
		LOG.PrintTimeAndLog( 0, "ioRenderSystem::Create - GetDeviceCaps FAILED(%x)", hr );

		SAFERELEASE(m_pD3D);
		return false;
	}

	return true;
}

HRESULT ioRenderSystem::CreateD3DDevice( UINT AdapterToUse, D3DDEVTYPE DeviceType )
{
	HRESULT hr;

	if( GetVertexShaderVersion() >= 20 && GetPixelShaderVersion() >= 20 )
	{
		m_dwDeviceBehaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED;
		if( m_D3DCaps.DevCaps & D3DDEVCAPS_PUREDEVICE )
		{
			m_dwDeviceBehaviorFlags |= D3DCREATE_PUREDEVICE;
		}

		hr = m_pD3D->CreateDevice( AdapterToUse,
								   DeviceType,
								   m_hWnd,
								   m_dwDeviceBehaviorFlags,
								   &m_D3DParam,
								   &m_pD3DDevice );
	}
	else
	{
		hr = E_FAIL;	// 바로 MIXED로 직행하기 위함
	}

	if( FAILED( hr ) )
	{
		m_dwDeviceBehaviorFlags = D3DCREATE_MIXED_VERTEXPROCESSING | D3DCREATE_MULTITHREADED;

		hr = m_pD3D->CreateDevice( AdapterToUse,
								   DeviceType,
								   m_hWnd,
								   m_dwDeviceBehaviorFlags,
								   &m_D3DParam,
								   &m_pD3DDevice );

		if( FAILED( hr ) )
		{
			m_dwDeviceBehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED;

			hr = m_pD3D->CreateDevice( AdapterToUse,
									   DeviceType,
									   m_hWnd,
									   m_dwDeviceBehaviorFlags,
									   &m_D3DParam,
									   &m_pD3DDevice );
		}
	}

	return hr;
}

bool ioRenderSystem::Create( HWND hWnd, UINT uWidth, UINT uHeight, bool bSwapCopy, bool bWindow, bool bUse16BitBuffer )
{
	m_hWnd = hWnd;
	m_uClientWidth  = uWidth;
	m_uClientHeight = uHeight;

	if( !CreateDirect3D() )
		return false;

	LOG.PrintTimeAndLog( 0, "VertexShader:%d, PixelShader:%d",
							GetVertexShaderVersion(),
							GetPixelShaderVersion() );

	m_pEnumDisplay->CheckEnableDisplayMode( m_pD3D, D3DFMT_R5G6B5, D3DFMT_X8R8G8B8 );

	ZeroMemory( &m_D3DParam, sizeof(m_D3DParam) );
	m_D3DParam.BackBufferWidth  = uWidth;
	m_D3DParam.BackBufferHeight = uHeight;
	m_D3DParam.BackBufferCount  = 1;

	if( bUse16BitBuffer )
		m_D3DParam.BackBufferFormat = D3DFMT_R5G6B5;
	else
		m_D3DParam.BackBufferFormat = D3DFMT_X8R8G8B8;

	m_D3DParam.hDeviceWindow = hWnd;
	m_D3DParam.EnableAutoDepthStencil = TRUE;

	m_D3DParam.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	m_D3DParam.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

	if( bSwapCopy )
		m_D3DParam.SwapEffect = D3DSWAPEFFECT_COPY;
	else
		m_D3DParam.SwapEffect = D3DSWAPEFFECT_DISCARD;

	UINT uFullScreenRefresh = m_pEnumDisplay->GetRefresh( m_D3DParam.BackBufferFormat,
														  uWidth,
														  uHeight );

	if( bWindow )
		m_D3DParam.Windowed	= TRUE;
	else
		m_D3DParam.Windowed	= FALSE;

	if( bWindow )
		m_D3DParam.FullScreen_RefreshRateInHz = 0;
	else
		m_D3DParam.FullScreen_RefreshRateInHz = uFullScreenRefresh;

	UINT AdapterToUse = D3DADAPTER_DEFAULT;
	D3DDEVTYPE DeviceType = D3DDEVTYPE_HAL;

/////// for NVPerfHUD
#ifndef SHIPPING
	for( UINT Adapter = 0 ; Adapter < m_pD3D->GetAdapterCount() ; Adapter++ )
	{
		D3DADAPTER_IDENTIFIER9 Identifier;
		m_pD3D->GetAdapterIdentifier( Adapter, 0, &Identifier );
		if( strstr( Identifier.Description, "PerfHUD" ) )
		{
			AdapterToUse = Adapter;
			DeviceType = D3DDEVTYPE_REF;
			break;
		}
	}
#endif
////////////////

	HRESULT hr;
	D3DDISPLAYMODE kDispMode;
	hr = m_pD3D->GetAdapterDisplayMode( AdapterToUse, &kDispMode );
	if( FAILED(hr) )
	{
		ErrorReport::SetFormat( "%x", hr );
		ErrorReport::SetEnginePos( 5, 14 );
		LOG.PrintTimeAndLog( 0, "ioRenderSystem::Create - GetAdapterDisplayMode Failed(%x)", hr );

		SAFERELEASE( m_pD3D );
		return false;
	}

	hr = m_pD3D->CheckDepthStencilMatch( AdapterToUse,
										 DeviceType,
										 kDispMode.Format,
										 m_D3DParam.BackBufferFormat,
										 D3DFMT_D24X8 );

	if( SUCCEEDED( hr ) )
		m_D3DParam.AutoDepthStencilFormat = D3DFMT_D24X8;
	else
		m_D3DParam.AutoDepthStencilFormat = D3DFMT_D16;

	hr = CreateD3DDevice( AdapterToUse, DeviceType );

	if( FAILED(hr) )
	{
		for( int i=0 ; i<5 ; i++ )
		{
			::SleepEx( 200, FALSE );

			if( !CreateDirect3D() )
			{
				ErrorReport::SetEnginePos( 5, 15 );
				LOG.PrintTimeAndLog( 0, "ioRenderSystem::Create - Retry(%d) CreateDirect3D Failed", i+1 );
				SAFERELEASE( m_pD3D );
				return false;
			}

			hr = CreateD3DDevice( AdapterToUse, DeviceType );
			if( FAILED(hr) )
			{
				ErrorReport::SetFormat( "%x", hr );
				ErrorReport::SetEnginePos( 5, 16 );
				LOG.PrintTimeAndLog( 0, "ioRenderSystem::Create - CreateDevice Failed(%x)(%d)", hr, i+1 );
			}
			else
			{
				break;
			}
		}

		if( i>= 5 )
		{
			LOG.PrintTimeAndLog( 0, "ioRenderSystem::Create - CreateDevice All Failed" );
			SAFERELEASE( m_pD3D );
			return false;
		}
	}

	CheckMaxMultiSampling( m_D3DParam.BackBufferFormat, m_D3DParam.Windowed );

	if( m_D3DParam.AutoDepthStencilFormat == D3DFMT_D24X8 )
		LOG.PrintTimeAndLog( 0, "ZBuffer - D24X8" );
	else if( m_D3DParam.AutoDepthStencilFormat == D3DFMT_D16 )
		LOG.PrintTimeAndLog( 0, "ZBuffer - D16" );

	if( m_dwDeviceBehaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING )
	{
		if( m_dwDeviceBehaviorFlags & D3DCREATE_PUREDEVICE )
			LOG.PrintTimeAndLog( 0, "ioRenderSystem::Create - Pure Hardware Vertex Processing");
		else
			LOG.PrintTimeAndLog( 0, "ioRenderSystem::Create - Hardware Vertex Processing");

		m_NoneAutoBufferUsage = BU_HARDWARE;
	}
	else if( m_dwDeviceBehaviorFlags & D3DCREATE_MIXED_VERTEXPROCESSING )
	{
		LOG.PrintTimeAndLog( 0, "ioRenderSystem::Create - Mixed Vertex Processing");
		m_NoneAutoBufferUsage = BU_HARDWARE;
	}
	else
	{
		LOG.PrintTimeAndLog( 0, "ioRenderSystem::Create - Soft Vertex Processing");
		m_NoneAutoBufferUsage = BU_SOFTWARE;
	}

	if( m_dwDeviceBehaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING )
	{
		ioVertexBufferManager::SetAutoBufferUsage( BU_HARDWARE );
		ioIndexBufferManager::SetAutoBufferUsage( BU_HARDWARE );
	}
	else
	{
		ioVertexBufferManager::SetAutoBufferUsage( BU_SOFTWARE );
		ioIndexBufferManager::SetAutoBufferUsage( BU_SOFTWARE );

		SetSoftwareVertexProcessing( true );
	}

	InitGamma();
	UpdateSurfaceInfo();
	CheckEnableShadowMapType();

	CreateCaptureTarget( true );

	return true;
}

void ioRenderSystem::SetFixedFVF( DWORD dwFVF )
{
	if( s_RenderStateDesc.m_dwFVF != dwFVF )
	{
		HRESULT hr = m_pD3DDevice->SetFVF( dwFVF );
		if( FAILED(hr) )
		{
			ErrorReport::SetFormat( "SetFixedFVF: %x", hr );
			LOG.PrintTimeAndLog( 0, "ioRenderSystem::SetFixedFVF - FAILED(%x)", hr );
		}

		s_RenderStateDesc.m_dwFVF = dwFVF;
		s_RenderStateDesc.m_pD3DDecl = NULL;
	}
}

void ioRenderSystem::SetSoftwareVertexProcessing( bool bSoftware )
{
	if( m_pD3DDevice == NULL )
		return;

	if( m_dwDeviceBehaviorFlags & D3DCREATE_MIXED_VERTEXPROCESSING )
	{
		if( m_bSoftwareProcessing != bSoftware )
		{
			HRESULT hr = m_pD3DDevice->SetSoftwareVertexProcessing( bSoftware );
			if( FAILED(hr) )
			{
				ErrorReport::SetFormat( "SetSoftwareVertexProcessing: %x", hr );
				LOG.PrintTimeAndLog( 0, "ioRenderSystem::SetSoftwareVertexProcessing - FAILED(%x)", hr );
			}

			m_bSoftwareProcessing = bSoftware;
		}
	}
}

void ioRenderSystem::SetVertexDeclaration( ioVertexDeclaration *pDeclaration )
{
	IDirect3DVertexDeclaration9 *pD3DDecl = NULL;
	if( pDeclaration )
		pD3DDecl = pDeclaration->GetD3DDeclaration();

	if( s_RenderStateDesc.m_pD3DDecl != pD3DDecl )
	{
		HRESULT hr;	
		hr = m_pD3DDevice->SetVertexDeclaration( pD3DDecl );
		if( FAILED(hr) )
		{
			ErrorReport::SetFormat( "SetVertexDeclaration: %x", hr );
			LOG.PrintTimeAndLog( 0, "ioRenderSystem::SetVertexDeclaration - FAILED(%x)", hr );
		}

		s_RenderStateDesc.m_dwFVF = 0;
		s_RenderStateDesc.m_pD3DDecl = pD3DDecl;
	}	
}

void ioRenderSystem::SetVertexBufferBinder( ioVertexBufferBinder *pBinder )
{
	HRESULT hr;

	const ioVertexBufferBinder::VertexBufferBindList &rkBindList = pBinder->GetBufferList();

	ioVertexBufferBinder::VertexBufferBindList::const_iterator iter;
	for( iter=rkBindList.begin() ; iter!=rkBindList.end() ; ++iter )
	{
		const ioVertexBufferBinder::VertexBufferStream &rkStream = *iter;

		if( !s_RenderStateDesc.IsSameStream( rkStream.m_uStream, rkStream.m_pInstance ) )
		{
			hr = m_pD3DDevice->SetStreamSource( rkStream.m_uStream,
												rkStream.m_pInstance->GetD3DBuffer(),
												rkStream.m_pInstance->GetFullOffsetBytes(),
												rkStream.m_pInstance->GetVertexStride() );

			if( FAILED(hr) )
			{
				ErrorReport::SetFormat( "SetVertexBufferBinder1: %x", hr );
				LOG.PrintTimeAndLog( 0, "SetVertexBufferBinder - SetStreamSource Failed(%x)", hr );
			}
			
			s_RenderStateDesc.SetStreamDesc( rkStream.m_uStream, rkStream.m_pInstance );
		}
	}

	int iCurStreamCount = rkBindList.size();
	for( int i=iCurStreamCount ; i<s_RenderStateDesc.m_iLastVertexStreamCount ; i++ )
	{
		hr = m_pD3DDevice->SetStreamSource( i, NULL, 0, 0 );
		if( FAILED(hr) )
		{
			ErrorReport::SetFormat( "SetVertexBufferBinder2: %x", hr );
			LOG.PrintTimeAndLog( 0, "Release StreamSource Failed(%x)", hr );
		}

		s_RenderStateDesc.SetStreamDesc( i, NULL );
	}

	s_RenderStateDesc.m_iLastVertexStreamCount = iCurStreamCount;
}

bool ioRenderSystem::SetIndexBufferInstance( ioIndexBufferInstance *pIndex )
{
	IDirect3DIndexBuffer9 *pD3DBuf = NULL;
	if( pIndex )
		pD3DBuf = pIndex->GetD3DBuffer();

	if( s_RenderStateDesc.m_pIndexBuf != pD3DBuf )
	{
		HRESULT hr;
		hr = m_pD3DDevice->SetIndices( pD3DBuf );
		if( FAILED(hr) )
		{
			ErrorReport::SetFormat( "SetIndexBufferInstance: %x", hr );
			LOG.PrintTimeAndLog( 0, "ioRenderSystem::SetIndexBufferInstance - SetIndices Failed(%x)", hr );
			return false;
		}

		s_RenderStateDesc.m_pIndexBuf = pD3DBuf;
	}

	return true;
}

void ioRenderSystem::DisableShader()
{
	m_pD3DDevice->SetVertexShader( NULL );
	m_pD3DDevice->SetPixelShader( NULL );
}

void ioRenderSystem::EvictManagedResources()
{
	HRESULT hr = m_pD3DDevice->EvictManagedResources();
	if( FAILED( hr ) )
	{
		LOG.PrintTimeAndLog( 0, "ioRenderSystem::EvictManagedResources - Failed(%x)", hr );
	}
}

void ioRenderSystem::SetDialogBoxMode( bool bEnable )
{
	if( m_bDialogBoxMode == bEnable )
		return;

	HRESULT hr;

	if( bEnable )
	{
		bool bParamChanged = false;

		if( m_D3DParam.SwapEffect != D3DSWAPEFFECT_DISCARD )
		{
			m_D3DParam.SwapEffect = D3DSWAPEFFECT_DISCARD;
			bParamChanged = true;
		}

		if( ChangeMultiSamplingType( D3DMULTISAMPLE_NONE ) )
		{
			EnableMultiSampling( false );
			bParamChanged = true;
		}

		if( bParamChanged )
		{
			ApplyChangedPresentParameters();
		}

		hr = m_pD3DDevice->SetDialogBoxMode( TRUE );
	}
	else
	{
		hr = m_pD3DDevice->SetDialogBoxMode( FALSE );
	}

	if( FAILED(hr) )
	{
		ErrorReport::SetFormat( "SetDialogBoxMode: %x", hr );
		LOG.PrintTimeAndLog( 0, "ioRenderSystem::SetDialogBoxMode - %d(%x)", bEnable, hr );
	}
	else
	{
		m_bDialogBoxMode = bEnable;
	}
}

void ioRenderSystem::Render( ioRenderOperation &rkRo, ioShader *pShader )
{
	if( m_bDeviceLost || rkRo.m_iFaceCount <= 0 )
		return;

	SetVertexDeclaration( rkRo.m_pMeshData->m_pVertexDeclaration );
	SetVertexBufferBinder( rkRo.m_pMeshData->m_pVertexBufferBinder );

	HRESULT hr;
	if( rkRo.m_bUseIndex )
	{
		ioIndexBufferInstance *pIndex = rkRo.m_pMeshData->m_pIndexBufferInstance;
		if( pIndex )
		{
			if( !SetIndexBufferInstance( pIndex ) )
				return;

			if( pShader )
			{
				pShader->CommitChanges();
			}

			hr = m_pD3DDevice->DrawIndexedPrimitive( rkRo.m_PrimitiveType,
													 rkRo.m_iBaseVertexIndex,
													 rkRo.m_iMinIndex,
													 rkRo.m_iVertexCount,
													 pIndex->GetOffsetIndex() + rkRo.m_iIndexStart,
													 rkRo.m_iFaceCount );

			if( FAILED(hr) )
			{
				ErrorReport::SetFormat( "ioRenderSystem::Render: %x", hr );
				LOG.PrintTimeAndLog( 0, "ioRenderSystem::Render - DrawIndexedPrimitive Failed(%x)", hr );
			}
		}
		else
		{
			LOG.PrintTimeAndLog( 0, "ioRenderSystem::Render - IndexBuffer Not Exist" );
		}
	}
	else
	{
		if( pShader )
		{
			pShader->CommitChanges();
		}

		hr = m_pD3DDevice->DrawPrimitive( rkRo.m_PrimitiveType,
										  rkRo.m_iBaseVertexIndex,
										  rkRo.m_iFaceCount );

		if( FAILED( hr ) )
		{
			LOG.PrintTimeAndLog( 0, "ioRenderSystem::Render - DrawPrimitive Failed(%x)", hr );
		}
	}

	m_iRenderedFaceCount += rkRo.m_iFaceCount;
	m_iRenderCallCount++;
}

void ioRenderSystem::RenderQuad()
{
	if( m_bDeviceLost )	return;

	ioMeshData *pShareData = g_VtxBufMgr.GetShareVtxTexData();
	if( pShareData )
	{
		SetVertexDeclaration( pShareData->m_pVertexDeclaration );
		SetVertexBufferBinder( pShareData->m_pVertexBufferBinder );
		RenderPrimitive( D3DPT_TRIANGLEFAN, 0, 2 );
	}
}

void ioRenderSystem::RenderPrimitive( D3DPRIMITIVETYPE eType, UINT uStartVertex, UINT uPrimitiveCount )
{
	if( m_bDeviceLost )	return;

	HRESULT hr = m_pD3DDevice->DrawPrimitive( eType, uStartVertex, uPrimitiveCount );
	if( FAILED(hr) )
	{
		ErrorReport::SetFormat( "ioRenderSystem::RenderPrimitive: %x", hr );
		LOG.PrintTimeAndLog( 0, "ioRenderSystem::RenderPrimitive - DrawPrimitive Failed(%x)", hr );
		return;
	}

	m_iRenderedFaceCount += uPrimitiveCount;
	m_iRenderCallCount++;
}

void ioRenderSystem::SetLightEnable( bool bEnable )
{
	if( s_RenderStateDesc.m_iLightEnable != (int)bEnable )
	{
		HRESULT	hr = m_pD3DDevice->SetRenderState( D3DRS_LIGHTING, bEnable );
		if( FAILED(hr) )
		{
			ErrorReport::SetFormat( "ioRenderSystem::SetLightEnable: %x", hr );
			LOG.PrintTimeAndLog( 0, "ioRenderSystem::SetLightEnable FAILED(%X)", hr );
		}

		s_RenderStateDesc.m_iLightEnable = (int)bEnable;
	}
}

void ioRenderSystem::SetAmbientLight( float r, float g, float b )
{
	m_Ambient.r = r;
	m_Ambient.g = g;
	m_Ambient.b = b;
	m_Ambient.a = 1.0f;

	HRESULT hr;
	hr = m_pD3DDevice->SetRenderState( D3DRS_AMBIENT, D3DCOLOR_COLORVALUE( r, g, b, 1.0f ) );
	if( FAILED(hr) )
	{
		ErrorReport::SetFormat( "ioRenderSystem::SetAmbientLight: %x", hr );
		LOG.PrintTimeAndLog( 0, "ioRenderSystem::SetAmbientLight - Failed(%x)", hr );
	}
}

void ioRenderSystem::SetLightsList( const ioLightList &rkLight )
{
	HRESULT hr;

	int iNumLights = rkLight.size();
	for( int i=0 ; i<iNumLights && i<MAX_LIGHTS ; i++ )
	{
		ioLight *pLight = rkLight[i];

		if( s_RenderStateDesc.m_pLights[i] == pLight )
			continue;

		if( pLight->IsVisible() )
		{
			hr = m_pD3DDevice->SetLight( i, pLight->GetD3DLight() );
			if( FAILED(hr) )
			{
				ErrorReport::SetFormat( "ioRenderSystem::SetLightList1: %x", hr );
				LOG.PrintTimeAndLog( 0, "ioRenderSystem::SetLightList - %d SetLight Failed(%x)", i, hr );
			}

			hr = m_pD3DDevice->LightEnable( i, TRUE );
			if( FAILED(hr) )
			{
				ErrorReport::SetFormat( "ioRenderSystem::SetLightList2: %x", hr );
				LOG.PrintTimeAndLog( 0, "ioRenderSystem::SetLightList - %d LightEnable Failed(%x)", i, hr );
			}
		}
		else
		{
			m_pD3DDevice->LightEnable( i, FALSE );
			s_RenderStateDesc.m_pLights[i] = NULL;
		}
	}

	for( ; i<m_iUsedLights ; i++ )
	{
		m_pD3DDevice->LightEnable( i, FALSE );
		s_RenderStateDesc.m_pLights[i] = NULL;
	}

	m_iUsedLights = iNumLights;
}

void ioRenderSystem::SetMaterial( const D3DMATERIAL9 &rkMaterial )
{
	if( memcmp( &s_RenderStateDesc.m_Material, &rkMaterial, sizeof(D3DMATERIAL9) ) )
	{
		HRESULT hr = m_pD3DDevice->SetMaterial( &rkMaterial );
		if( FAILED(hr) )
		{
			ErrorReport::SetFormat( "ioRenderSystem::SetMaterial1: %x", hr );
			LOG.PrintTimeAndLog( 0, "ioRenderSystem::SetMaterial - Failed(%x)", hr );
		}

		s_RenderStateDesc.m_Material = rkMaterial;
	}
}

void ioRenderSystem::SetSceneBlend( D3DBLENDOP eOp, D3DBLEND eSrc, D3DBLEND eDest )
{
	if( s_RenderStateDesc.m_SceneBlend.eBlendOp == eOp &&
		s_RenderStateDesc.m_SceneBlend.eSrc == eSrc &&
		s_RenderStateDesc.m_SceneBlend.eDest == eDest )
		return;

	if( s_RenderStateDesc.m_SceneBlend.eBlendOp != eOp )
	{
		m_pD3DDevice->SetRenderState( D3DRS_BLENDOP, eOp );
		s_RenderStateDesc.m_SceneBlend.eBlendOp = eOp;
	}

	if( s_RenderStateDesc.m_SceneBlend.eSrc != eSrc )
	{
		m_pD3DDevice->SetRenderState( D3DRS_SRCBLEND, eSrc );
		s_RenderStateDesc.m_SceneBlend.eSrc = eSrc;
	}

	if( s_RenderStateDesc.m_SceneBlend.eDest != eDest )
	{
		m_pD3DDevice->SetRenderState( D3DRS_DESTBLEND, eDest );
		s_RenderStateDesc.m_SceneBlend.eDest = eDest;
	}

	DWORD dwAlphaBlendEnable = TRUE;
	if( eSrc == D3DBLEND_ONE && eDest == D3DBLEND_ZERO )
	{
		dwAlphaBlendEnable = FALSE;
	}

	if( s_RenderStateDesc.m_dwAlphaBlendEnable != dwAlphaBlendEnable )
	{
		m_pD3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, dwAlphaBlendEnable );
		s_RenderStateDesc.m_dwAlphaBlendEnable = dwAlphaBlendEnable;
	}
}

void ioRenderSystem::SetShadingType( ShadeType shade )
{
	if( s_RenderStateDesc.m_ShadeType == shade )
		return;

	D3DSHADEMODE d3dShade = D3DSHADE_GOURAUD;
	switch( shade )
	{
	case ST_FLAT:
		d3dShade = D3DSHADE_FLAT;
		break;
	case ST_GOURAUD:
		d3dShade = D3DSHADE_GOURAUD;
		break;
	case ST_PHONG:
		d3dShade = D3DSHADE_PHONG;
		break;
	}

	HRESULT	hr = m_pD3DDevice->SetRenderState( D3DRS_SHADEMODE, d3dShade );
	if( FAILED(hr) )
	{
		ErrorReport::SetFormat( "ioRenderSystem::SetShadingType: %x", hr );
		LOG.PrintTimeAndLog( 0, "ioRenderSystem::SetShadingType FAILED(%X)", hr );
	}

	s_RenderStateDesc.m_ShadeType = shade;
}

void ioRenderSystem::SetCullingMode( CullingMode mode )
{
	if( s_RenderStateDesc.m_CullMode == mode )
		return;

	D3DCULL	d3dCull = D3DCULL_CCW;
	switch( mode )
	{
	case CM_NO:
		d3dCull = D3DCULL_NONE;
		break;
	case CM_CW:
		d3dCull = D3DCULL_CW;
		break;
	case CM_CCW:
		d3dCull = D3DCULL_CCW;
		break;
	}

	HRESULT	hr = m_pD3DDevice->SetRenderState( D3DRS_CULLMODE, d3dCull );
	if( FAILED(hr) )
	{
		ErrorReport::SetFormat( "ioRenderSystem::SetCullingMode: %x", hr );
		LOG.PrintTimeAndLog( 0, "ioRenderSystem::SetCullingMode FAILED(%X)", hr );
	}

	s_RenderStateDesc.m_CullMode = mode;
}

void ioRenderSystem::SetFillMode( FillMode mode )
{
	if( m_bSetGlobalFillMode )	return;
	if( s_RenderStateDesc.m_FillMode == mode )
		return;

	D3DFILLMODE	d3dFill;

	switch( mode )
	{
	case FM_POINT:
		d3dFill = D3DFILL_POINT;
		break;
	case FM_WIREFRAME:
		d3dFill = D3DFILL_WIREFRAME;
		break;
	case FM_SOLID:
		d3dFill = D3DFILL_SOLID;
		break;
	}

	HRESULT	hr = m_pD3DDevice->SetRenderState( D3DRS_FILLMODE, d3dFill );
	if( FAILED(hr) )
	{
		ErrorReport::SetFormat( "ioRenderSystem::SetFillMode: %x", hr );
		LOG.PrintTimeAndLog( 0, "ioRenderSystem::SetFillMode FAILED(%X)", hr );
	}

	s_RenderStateDesc.m_FillMode = mode;
}

void ioRenderSystem::SetGlobalFillMode( bool bUse, FillMode eMode )
{
	m_GlobalFillMode = eMode;
	m_bSetGlobalFillMode = false;
	SetFillMode( eMode );

	m_bSetGlobalFillMode = bUse;
}

void ioRenderSystem::SetColorWriteEnable( DWORD dwColorFlag )
{
	if( !(m_D3DCaps.PrimitiveMiscCaps & D3DPMISCCAPS_COLORWRITEENABLE) )
		return;

	if( s_RenderStateDesc.m_dwTextureStageColorWrite != dwColorFlag )
	{
		HRESULT hr = m_pD3DDevice->SetRenderState( D3DRS_COLORWRITEENABLE, dwColorFlag );
		if( FAILED( hr ) )
		{
			LOG.PrintTimeAndLog( 0, "ioRenderSystem::SetColorWriteEnable - FAILED(%x)", hr );
		}

		s_RenderStateDesc.m_dwTextureStageColorWrite = dwColorFlag;
	}
}

void ioRenderSystem::SetTextureFactor( DWORD dwFactor )
{
	// TFactor는 셰이더쪽에서도 사용하기때문에 여기서만 체크해서는 중복세팅을 확인할수가 없다.
	HRESULT hr = m_pD3DDevice->SetRenderState( D3DRS_TEXTUREFACTOR, dwFactor );
	if( FAILED( hr ) )
	{
		LOG.PrintTimeAndLog( 0, "ioRenderSystem::SetTextureFactor - FAILED(%x)", hr );
	}
}

void ioRenderSystem::SetDepthFunction( D3DCMPFUNC eDepthFunc, bool bWriteEnable, bool bCheckEnable )
{
	HRESULT hr;
	if( s_RenderStateDesc.m_DepthFunc != eDepthFunc )
	{
		hr = m_pD3DDevice->SetRenderState( D3DRS_ZFUNC, eDepthFunc );
		if( FAILED( hr ) )
		{
			ErrorReport::SetFormat( "ioRenderSystem::SetDepthFunction1: %x", hr );
			LOG.PrintTimeAndLog( 0, "ioRenderSystem::SetDepthFunction - ZFunc Failed(%x, %x)",
									hr, eDepthFunc );
		}
		s_RenderStateDesc.m_DepthFunc = eDepthFunc;
	}

	if( s_RenderStateDesc.m_iDepthCheckEnable != (int)bCheckEnable )
	{
		hr = m_pD3DDevice->SetRenderState( D3DRS_ZENABLE, (BOOL)bCheckEnable );
		if( FAILED(hr) )
		{
			ErrorReport::SetFormat( "ioRenderSystem::SetDepthFunction2: %x", hr );
			LOG.PrintTimeAndLog( 0, "ioRenderSystem::SetDepthFunction - ZEnable Failed(%x)", hr );
		}
		s_RenderStateDesc.m_iDepthCheckEnable = (int)bCheckEnable;
	}

	if( s_RenderStateDesc.m_iDepthWriteEnable != (int)bWriteEnable )
	{
		hr = m_pD3DDevice->SetRenderState( D3DRS_ZWRITEENABLE, (BOOL)bWriteEnable );
		if( FAILED(hr) )
		{
			ErrorReport::SetFormat( "ioRenderSystem::SetDepthFunction2: %x", hr );
			LOG.PrintTimeAndLog( 0, "ioRenderSystem::SetDepthFunction - ZWriteEnable Failed(%x)", hr );
		}
		s_RenderStateDesc.m_iDepthWriteEnable = (int)bWriteEnable;
	}
}

void ioRenderSystem::SetDepthBias( float fDepthBias, float fBiasSlope )
{
	HRESULT hr;
	if( m_D3DCaps.RasterCaps & D3DPRASTERCAPS_DEPTHBIAS )
	{
		if( s_RenderStateDesc.m_fDepthBias != fDepthBias )
		{
			hr = m_pD3DDevice->SetRenderState( D3DRS_DEPTHBIAS, *(DWORD*)&fDepthBias );
			if( FAILED(hr) )
			{
				ErrorReport::SetFormat( "ioRenderSystem::SetDepthBias1: %x", hr );
				LOG.PrintTimeAndLog( 0, "ioRenderSystem::SetDepthBias DepthBias FAILED(%x)", hr );
			}
			s_RenderStateDesc.m_fDepthBias = fDepthBias;
		}
	}

	if( m_D3DCaps.RasterCaps & D3DPRASTERCAPS_SLOPESCALEDEPTHBIAS )
	{
		if( s_RenderStateDesc.m_fDepthSlope != fBiasSlope )
		{
			hr = m_pD3DDevice->SetRenderState( D3DRS_SLOPESCALEDEPTHBIAS, *(DWORD*)&fBiasSlope );
			if( FAILED(hr) )
			{
				ErrorReport::SetFormat( "ioRenderSystem::SetDepthBias2: %x", hr );
				LOG.PrintTimeAndLog( 0, "ioRenderSystem::SetDepthBias Slope FAILED(%x)", hr );
			}
			s_RenderStateDesc.m_fDepthSlope = fBiasSlope;
		}
	}
}

void ioRenderSystem::SetAlphaTest( D3DCMPFUNC eTestFunc, int iAlphaRef )
{
	if( s_RenderStateDesc.m_AlphaTextFunc != eTestFunc )
	{
		if( eTestFunc == D3DCMP_NEVER )
		{
			// 이전에 세팅된값이 NEVER가 아니므로 테스트는 TRUE였다.
			m_pD3DDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
		}
		else
		{
			// 이전에 세팅된값이 NEVER였을때만 FALSE였을것이므로..
			if( s_RenderStateDesc.m_AlphaTextFunc == D3DCMP_NEVER )
			{
				m_pD3DDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
			}
		}

		m_pD3DDevice->SetRenderState( D3DRS_ALPHAFUNC, eTestFunc );
		s_RenderStateDesc.m_AlphaTextFunc = eTestFunc;
	}

	if( s_RenderStateDesc.m_iAlphaTestRef != iAlphaRef )
	{
		m_pD3DDevice->SetRenderState( D3DRS_ALPHAREF, iAlphaRef );
		s_RenderStateDesc.m_iAlphaTestRef = iAlphaRef;
	}
}

void ioRenderSystem::SetTextureUnitSetting( int iUnit, ioTextureUnitState *pState )
{
	if( !pState->IsUseLightTexture() )	// LightTexture를 쓰는것은 개별세팅으로 넘긴다.
	{
		SetioTexture( iUnit, pState->GetTexture() );
	}

	SetTextureCoordIndex( iUnit, pState->GetTextureCoordSet() );
	SetTextureAddressMode( iUnit, pState->GetTextureAddressMode() );
	SetTextureFiltering( iUnit, pState->GetTextureFiltering() );
	SetTextureMipMapBias( iUnit, pState->GetTextureMipMapBias() );
	SetTextureMaxAnisotropy( iUnit, pState->GetTextureAnisotropy() );

	SetTextureStageColorBlend( iUnit, pState->GetColorStageBlendOp() );
	SetTextureStageAlphaBlend( iUnit, pState->GetAlphaStageBlendOp() );

	// TextureMatrix의 업데이트를 위해서 GetTextureTransform()는 꼭 호출해줘야한다.
	const D3DXMATRIX &rkMatrix = pState->GetTextureTransform();

	// SetTextureMatrix in Fixed Pipeline
	if( !(m_dwDeviceBehaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING) )
	{
		if( pState->IsIdentityTextureTransform() )
		{
			SetTextureTransformFlag( iUnit, D3DTTFF_DISABLE );
		}
		else
		{
			// 차후 다른 기능이 추가될경우 flag를 구별해주는 기능이 필요
			SetTextureMatrix( iUnit, rkMatrix );
			SetTextureTransformFlag( iUnit, D3DTTFF_COUNT2 );
		}
	}
}

void ioRenderSystem::DisableTextureUnitsFrom( int iUnit )
{
	int iMaxTexture = m_D3DCaps.MaxSimultaneousTextures;
	for( int i=iUnit ; i<iMaxTexture ; i++ )
	{
		SetTexture( i, NULL );
	}

	if( s_RenderStateDesc.m_TexStageDesc[iUnit].ColorBlend.m_StageOp != D3DTOP_DISABLE )
	{
		m_pD3DDevice->SetTextureStageState( iUnit, D3DTSS_COLOROP, D3DTOP_DISABLE );
		s_RenderStateDesc.m_TexStageDesc[iUnit].ColorBlend.m_StageOp = D3DTOP_DISABLE;
	}

	if( s_RenderStateDesc.m_TexStageDesc[iUnit].AlphaBlend.m_StageOp != D3DTOP_DISABLE )
	{
		m_pD3DDevice->SetTextureStageState( iUnit, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
		s_RenderStateDesc.m_TexStageDesc[iUnit].AlphaBlend.m_StageOp = D3DTOP_DISABLE;
	}

	SetTextureTransformFlag( iUnit, D3DTTFF_DISABLE );
}

void ioRenderSystem::SetTextureMaxAnisotropy( int iUnit, int iMaxAniso )
{
	if( s_RenderStateDesc.m_TexStageDesc[iUnit].iMaxAnisotropy != iMaxAniso )
	{
		m_pD3DDevice->SetSamplerState( iUnit, D3DSAMP_MAXANISOTROPY, iMaxAniso );
		s_RenderStateDesc.m_TexStageDesc[iUnit].iMaxAnisotropy = iMaxAniso;
	}
}

void ioRenderSystem::SetTextureStageColorBlend( int iUnit, const StageBlendOperation &rkBlend )
{
	if( s_RenderStateDesc.m_TexStageDesc[iUnit].ColorBlend.m_StageOp != rkBlend.m_StageOp )
	{
		m_pD3DDevice->SetTextureStageState( iUnit, D3DTSS_COLOROP, rkBlend.m_StageOp );
		s_RenderStateDesc.m_TexStageDesc[iUnit].ColorBlend.m_StageOp = rkBlend.m_StageOp;
	}

	if( s_RenderStateDesc.m_TexStageDesc[iUnit].ColorBlend.m_dwSrcFactor != rkBlend.m_dwSrcFactor )
	{
		m_pD3DDevice->SetTextureStageState( iUnit, D3DTSS_COLORARG1, rkBlend.m_dwSrcFactor );
		s_RenderStateDesc.m_TexStageDesc[iUnit].ColorBlend.m_dwSrcFactor = rkBlend.m_dwSrcFactor;
	}	

	if( s_RenderStateDesc.m_TexStageDesc[iUnit].ColorBlend.m_dwDestFactor != rkBlend.m_dwDestFactor )
	{
		m_pD3DDevice->SetTextureStageState( iUnit, D3DTSS_COLORARG2, rkBlend.m_dwDestFactor );
		s_RenderStateDesc.m_TexStageDesc[iUnit].ColorBlend.m_dwDestFactor = rkBlend.m_dwDestFactor;
	}
}

void ioRenderSystem::SetTextureStageColorBlend( int iUnit,
												D3DTEXTUREOP eStageOp,
												DWORD dwSrcFactor,
												DWORD dwDestFactor )
{
	SetTextureStageColorBlend( iUnit, StageBlendOperation( eStageOp, dwSrcFactor, dwDestFactor ) );
}

void ioRenderSystem::SetTextureStageAlphaBlend( int iUnit,
											    D3DTEXTUREOP eStageOp,
												DWORD dwSrcFactor,
												DWORD dwDestFactor )
{
	SetTextureStageAlphaBlend( iUnit, StageBlendOperation( eStageOp, dwSrcFactor, dwDestFactor ) );
}

void ioRenderSystem::SetTextureStageAlphaBlend( int iUnit, const StageBlendOperation &rkBlend )
{
	if( s_RenderStateDesc.m_TexStageDesc[iUnit].AlphaBlend.m_StageOp != rkBlend.m_StageOp )
	{
		m_pD3DDevice->SetTextureStageState( iUnit, D3DTSS_ALPHAOP, rkBlend.m_StageOp );
		s_RenderStateDesc.m_TexStageDesc[iUnit].AlphaBlend.m_StageOp = rkBlend.m_StageOp;
	}

	if( s_RenderStateDesc.m_TexStageDesc[iUnit].AlphaBlend.m_dwSrcFactor != rkBlend.m_dwSrcFactor )
	{
		m_pD3DDevice->SetTextureStageState( iUnit, D3DTSS_ALPHAARG1, rkBlend.m_dwSrcFactor );
		s_RenderStateDesc.m_TexStageDesc[iUnit].AlphaBlend.m_dwSrcFactor = rkBlend.m_dwSrcFactor;
	}

	if( s_RenderStateDesc.m_TexStageDesc[iUnit].AlphaBlend.m_dwDestFactor != rkBlend.m_dwDestFactor )
	{
		m_pD3DDevice->SetTextureStageState( iUnit, D3DTSS_ALPHAARG2, rkBlend.m_dwDestFactor );
		s_RenderStateDesc.m_TexStageDesc[iUnit].AlphaBlend.m_dwDestFactor = rkBlend.m_dwDestFactor;
	}
}

void ioRenderSystem::DisableAlphaBlend()
{
	SetSceneBlend( D3DBLENDOP_ADD, D3DBLEND_ONE, D3DBLEND_ZERO );
}

void ioRenderSystem::DisableAlphaTest()
{
	SetAlphaTest( D3DCMP_NEVER, 0 );
}

void ioRenderSystem::SetTexture( int iStage, IDirect3DTexture9 *pTex )
{
	if( s_RenderStateDesc.m_TexStageDesc[iStage].pTex != pTex )
	{
		HRESULT hr;
		hr = m_pD3DDevice->SetTexture( iStage, pTex );
		if( FAILED(hr) )
		{
			ErrorReport::SetFormat( "ioRenderSystem::SetTexture: %x", hr );
			LOG.PrintTimeAndLog( 0, "ioRenderSytem::SetTexture - %d Stage Failed(%x)", iStage, hr );
		}

		s_RenderStateDesc.m_TexStageDesc[iStage].pTex = pTex;
	}
}

void ioRenderSystem::SetioTexture( int iStage, ioTexture *pTex )
{
	if( pTex )
	{
		SetTexture( iStage, pTex->GetTexture() );
	}
	else
	{
		SetTexture( iStage, NULL );
	}
}

void ioRenderSystem::SetTexture( int iStage, const ioHashString &kName )
{
	ioTexture *pTex = NULL;
	if( !kName.IsEmpty() )
	{
		pTex = g_TextureMgr.GetTextureByName( kName );
	}

	SetioTexture( iStage, pTex );
}

void ioRenderSystem::SetTextureCoordIndex( int iStage, int iIndex )
{
	if( s_RenderStateDesc.m_TexStageDesc[iStage].iTexCoordIndex != iIndex )
	{
		HRESULT hr = m_pD3DDevice->SetTextureStageState( iStage, D3DTSS_TEXCOORDINDEX, iIndex );
		if( FAILED(hr) )
		{
			ErrorReport::SetFormat( "ioRenderSystem::SetTextureCoordIndex: %x", hr );
			LOG.PrintTimeAndLog( 0, "ioRenderSystem::SetTextureCoordIndex - Failed(%x)", hr );
			return;
		}

		s_RenderStateDesc.m_TexStageDesc[iStage].iTexCoordIndex = iIndex;
	}
}

void ioRenderSystem::SetTextureTransformFlag( int iStage, DWORD dwFlags )
{
	if( s_RenderStateDesc.m_TexStageDesc[iStage].m_dwTextureTransformFlags != dwFlags )
	{
		HRESULT hr = m_pD3DDevice->SetTextureStageState( iStage,
														 D3DTSS_TEXTURETRANSFORMFLAGS,
														 dwFlags );

		if( FAILED(hr) )
		{
			ErrorReport::SetFormat( "ioRenderSystem::SetTextureTransformFlag: %x", hr );
			LOG.PrintTimeAndLog( 0, "ioRenderSystem::SetTextureTransformFlag - Failed(%x)", hr );
			return;
		}

		s_RenderStateDesc.m_TexStageDesc[iStage].m_dwTextureTransformFlags = dwFlags;
	}
}

void ioRenderSystem::SetTextureMatrix( int iStage, const D3DXMATRIX &rkMatrix )
{
	HRESULT hr = m_pD3DDevice->SetTransform( (D3DTRANSFORMSTATETYPE)( D3DTS_TEXTURE0 + iStage ), &rkMatrix );
	if( FAILED(hr) )
	{
		ErrorReport::SetFormat( "ioRenderSystem::SetTextureMatrix: %x", hr );
		LOG.PrintTimeAndLog( 0, "ioRenderSystem::SetTextureMatrix - Failed(%x)", hr );
	}
}

void ioRenderSystem::SetTextureFiltering( int iStage, TextureFilterOption filter )
{
	if( s_RenderStateDesc.m_TexStageDesc[iStage].TextureFilter == filter )
		return;

	DWORD dwFilter = GetMagFilter( filter );
	if( s_RenderStateDesc.m_TexStageDesc[iStage].dwMagFilter != dwFilter )
	{
		m_pD3DDevice->SetSamplerState( iStage, D3DSAMP_MAGFILTER, dwFilter );
		s_RenderStateDesc.m_TexStageDesc[iStage].dwMagFilter = dwFilter;
	}

	dwFilter = GetMinFilter( filter );
	if( s_RenderStateDesc.m_TexStageDesc[iStage].dwMinFilter != dwFilter )
	{
		m_pD3DDevice->SetSamplerState( iStage, D3DSAMP_MINFILTER, dwFilter );
		s_RenderStateDesc.m_TexStageDesc[iStage].dwMinFilter = dwFilter;
	}

	dwFilter = GetMipFilter( filter );
	if( s_RenderStateDesc.m_TexStageDesc[iStage].dwMipFilter != dwFilter )
	{
		m_pD3DDevice->SetSamplerState( iStage, D3DSAMP_MIPFILTER, dwFilter );
		s_RenderStateDesc.m_TexStageDesc[iStage].dwMipFilter = dwFilter;
	}

	s_RenderStateDesc.m_TexStageDesc[iStage].TextureFilter = filter;
}

DWORD ioRenderSystem::GetMagFilter( TextureFilterOption filter )
{
	DWORD ret = D3DTEXF_LINEAR;

	// NOTE: Fall through if device doesn't support requested type
	switch( filter )
	{
	case TFO_BILINEAR_NOMIPMAP:
		ret = D3DTEXF_LINEAR;
		break;
	case TFO_ANISOTROPIC:
		if( m_D3DCaps.TextureFilterCaps & D3DPTFILTERCAPS_MAGFANISOTROPIC )
		{
			ret = D3DTEXF_ANISOTROPIC;
			break;
		}
	case TFO_TRILINEAR:
		if( m_D3DCaps.TextureFilterCaps & D3DPTFILTERCAPS_MAGFLINEAR )
		{
			ret = D3DTEXF_LINEAR;
			break;
		}
	case TFO_BILINEAR:
		if( m_D3DCaps.TextureFilterCaps & D3DPTFILTERCAPS_MAGFLINEAR )
		{
			ret = D3DTEXF_LINEAR;
			break;
		}
	case TFO_NEAREST:
		ret = D3DTEXF_POINT;
		break;
	}

	return ret;
}

DWORD ioRenderSystem::GetMinFilter( TextureFilterOption filter )
{
	DWORD ret = D3DTEXF_LINEAR;

	// NOTE: Fall through if device doesn't support requested type
	switch( filter )
	{
	case TFO_BILINEAR_NOMIPMAP:
		ret = D3DTEXF_LINEAR;
		break;
	case TFO_ANISOTROPIC:
		if( m_D3DCaps.TextureFilterCaps & D3DPTFILTERCAPS_MINFANISOTROPIC )
		{
			ret = D3DTEXF_ANISOTROPIC;
			break;
		}
	case TFO_TRILINEAR:
		if( m_D3DCaps.TextureFilterCaps & D3DPTFILTERCAPS_MINFLINEAR )
		{
			ret = D3DTEXF_LINEAR;
			break;
		}
	case TFO_BILINEAR:
		if( m_D3DCaps.TextureFilterCaps & D3DPTFILTERCAPS_MINFLINEAR )
		{
			ret = D3DTEXF_LINEAR;
			break;
		}
	case TFO_NEAREST:
		ret = D3DTEXF_POINT;
		break;
	}

	return ret;
}

DWORD ioRenderSystem::GetMipFilter( TextureFilterOption filter )
{
	DWORD ret = D3DTEXF_POINT;

	// NOTE: Fall through if device doesn't support requested type
	switch( filter )
	{
	case TFO_BILINEAR_NOMIPMAP:
		ret = D3DTEXF_NONE;
		break;
	case TFO_ANISOTROPIC:
		if( m_D3DCaps.TextureFilterCaps & D3DPTFILTERCAPS_MIPFLINEAR )
		{
			ret = D3DTEXF_LINEAR;
			break;
		}
	case TFO_TRILINEAR:
		if( m_D3DCaps.TextureFilterCaps & D3DPTFILTERCAPS_MIPFLINEAR )
		{
			ret = D3DTEXF_LINEAR;
			break;
		}
	case TFO_BILINEAR:
		if( m_D3DCaps.TextureFilterCaps & D3DPTFILTERCAPS_MIPFPOINT )
		{
			ret = D3DTEXF_POINT;
			break;
		}
	case TFO_NEAREST:
		ret = D3DTEXF_NONE;
		break;
	}

	return ret;
}

void ioRenderSystem::SetModelMatrix( const D3DXMATRIX *pMatModel )
{
	if( m_bDeviceLost )	return;

	HRESULT	hr = m_pD3DDevice->SetTransform( D3DTS_WORLD, pMatModel );
	if( FAILED(hr) )
	{
		ErrorReport::SetFormat( "ioRenderSystem::SetModelMatrix: %x", hr );
		LOG.PrintTimeAndLog( 0, "ioRenderSystem::SetModelMatrix - FAILED(%X)", hr );
		return;
	}

	m_matModel = *pMatModel;
}

void ioRenderSystem::SetModelMatrixIdentity()
{
	D3DXMATRIX matIdentity;
	D3DXMatrixIdentity( &matIdentity );
	SetModelMatrix( &matIdentity );
}

void ioRenderSystem::SetViewMatrix( const D3DXMATRIX *pMatView )
{
	if( m_bDeviceLost )	return;

	HRESULT	hr = m_pD3DDevice->SetTransform( D3DTS_VIEW, pMatView );
	if( FAILED(hr) )
	{
		ErrorReport::SetFormat( "ioRenderSystem::SetViewMatrix: %x", hr );
		LOG.PrintTimeAndLog( 0, "ioRenderSystem::SetViewMatrix - FAILED(%X)", hr );
		return;
	}

	m_matView = *pMatView;
}

void ioRenderSystem::SetProjMatrix( const D3DXMATRIX *pMatProj )
{
	if( m_bDeviceLost )	return;

	HRESULT hr = m_pD3DDevice->SetTransform( D3DTS_PROJECTION, pMatProj );
	if( FAILED(hr) )
	{
		ErrorReport::SetFormat( "ioRenderSystem::SetProjMatrix: %x", hr );
		LOG.PrintTimeAndLog( 0, "ioRenderSystem::SetProjMatrix - FAILED(%X)", hr );
		return;
	}

	m_matProj = *pMatProj;
}

void ioRenderSystem::SetViewPort( const D3DVIEWPORT9 &kViewPort )
{
	if( m_bDeviceLost )	return;

	HRESULT hr = m_pD3DDevice->SetViewport( &kViewPort );
	if( FAILED(hr) )
	{
		ErrorReport::SetFormat( "ioRenderSystem::SetViewPort: %x", hr );
		LOG.PrintTimeAndLog( 0, "ioRenderSystem::SetViewPort - FAILED(%x)", hr );
		return;
	}

	m_ViewPort = kViewPort;
}

int ioRenderSystem::GetVertexShaderVersion() const
{
	int iMajor = (m_D3DCaps.VertexShaderVersion & 0xff00)>>8;
	int iMinor = m_D3DCaps.VertexShaderVersion & 0xff;

	return iMajor * 10 + iMinor;
}

int ioRenderSystem::GetPixelShaderVersion() const
{
	int iMajor = (m_D3DCaps.PixelShaderVersion & 0xff00)>>8;
	int iMinor = m_D3DCaps.PixelShaderVersion & 0xff;

	return iMajor * 10 + iMinor;
}

void ioRenderSystem::SetTextureAddressMode( int iStage, TextureAddressMode mode )
{
	if( s_RenderStateDesc.m_TexStageDesc[iStage].TextureAddress == mode )
		return;

	D3DTEXTUREADDRESS d3dType;

	switch( mode )
	{
	case TAM_WRAP:
		d3dType = D3DTADDRESS_WRAP;
		break;
	case TAM_MIRROR:
		d3dType = D3DTADDRESS_MIRROR;
		break;
	case TAM_CLAMP:
		d3dType = D3DTADDRESS_CLAMP;
		break;
	}

	HRESULT	hr;

	hr = m_pD3DDevice->SetSamplerState( iStage, D3DSAMP_ADDRESSU, d3dType );
	if( FAILED(hr) )
	{
		ErrorReport::SetFormat( "SetTextureAddressMode1: %x", hr );
		LOG.PrintTimeAndLog( 0, "ioRenderSystem::SetTextureAddressMode - D3DTSS_ADDRESSU FAILED(%X)", hr );
		return;
	}

	hr = m_pD3DDevice->SetSamplerState( iStage, D3DSAMP_ADDRESSV, d3dType );
	if( FAILED(hr) )
	{
		ErrorReport::SetFormat( "SetTextureAddressMode2: %x", hr );
		LOG.PrintTimeAndLog( 0, "ioRenderSystem::SetTextureAddressMode - D3DTSS_ADDRESSV FAILED(%X)", hr );
		return;
	}

	s_RenderStateDesc.m_TexStageDesc[iStage].TextureAddress = mode;
}

void ioRenderSystem::SetFogFactor( DWORD dwColor, float fStart, float fEnd )
{
	m_dwFogColor = dwColor;
	m_fFogStart	 = fStart;
	m_fFogEnd	 = fEnd;
	m_fFogConstants = 1.0f / ( fEnd - fStart );

	if( m_fFogStart > m_fFogEnd )
	{
		LOG.PrintTimeAndLog( 0, "SetFogFactor - Start(%f) > End(%f)", m_fFogStart, m_fFogEnd );
		m_fFogEnd = m_fFogStart + 1.0f;
	}
}

void ioRenderSystem::SetFogEnable( bool bEnable )
{
	if( m_bFogEnable == bEnable )
		return;

	m_pD3DDevice->SetRenderState( D3DRS_FOGTABLEMODE, D3DFOG_NONE );

	if( !bEnable )
	{
		m_pD3DDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
		m_pD3DDevice->SetRenderState( D3DRS_FOGVERTEXMODE, D3DFOG_NONE );
		m_pD3DDevice->SetRenderState( D3DRS_RANGEFOGENABLE, FALSE );
	}
	else
	{
		m_pD3DDevice->SetRenderState( D3DRS_FOGENABLE, TRUE );
		m_pD3DDevice->SetRenderState( D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR );
		m_pD3DDevice->SetRenderState( D3DRS_FOGCOLOR, m_dwFogColor );
		m_pD3DDevice->SetRenderState( D3DRS_FOGSTART, *((LPDWORD)(&m_fFogStart)) );
		m_pD3DDevice->SetRenderState( D3DRS_FOGEND, *((LPDWORD)(&m_fFogEnd)) );

		if( m_D3DCaps.RasterCaps & D3DPRASTERCAPS_FOGRANGE )
		{
			m_pD3DDevice->SetRenderState( D3DRS_RANGEFOGENABLE, TRUE );
		}
	}

	m_bFogEnable = bEnable;
}

D3DXVECTOR4 ioRenderSystem::GetFogFactor() const
{
	return D3DXVECTOR4( m_fFogEnd, m_fFogConstants, 0.0f, 0.0f );
}

float ioRenderSystem::GetFogStart() const
{
	return m_fFogStart;
}

float ioRenderSystem::GetFogEnd() const
{
	return m_fFogEnd;
}

DWORD ioRenderSystem::GetFogColor() const
{
	return m_dwFogColor;
}

void ioRenderSystem::InitGamma()
{
	WORD wColor = 0;

	for( WORD wIndex = 0 ; wIndex<256 ; wIndex++ )
	{
		wColor = (wIndex<<8) | wIndex;
		wColor = max( 0, min( wColor, 0xffff ) );

		m_CurGamma.red[wIndex]   = wColor;
		m_CurGamma.green[wIndex] = wColor;
		m_CurGamma.blue[wIndex]  = wColor;
	}

	m_iCurBright   = 0;
	m_iCurContrast = 0;
}

void ioRenderSystem::ControlBrightness( int iBright )
{
	if( !(m_D3DCaps.Caps2 & D3DCAPS2_FULLSCREENGAMMA) )
		return;

	int iCurColor = 0;
	int iAddColor = (float)iBright / 255.0f * 0xffff;

	for( int i=0 ; i<256 ; i++ )
	{
		iCurColor = (int)m_CurGamma.red[i] + iAddColor;
		m_CurGamma.red[i] = (WORD)max( 0, min( iCurColor, 0xffff ) );

		iCurColor = (int)m_CurGamma.green[i] + iAddColor;
		m_CurGamma.green[i] = (WORD)max( 0, min( iCurColor, 0xffff ) );

		iCurColor = (int)m_CurGamma.blue[i] + iAddColor;
		m_CurGamma.blue[i] = (WORD)max( 0, min( iCurColor, 0xffff ) );
	}

	m_iCurBright = iBright;
	m_pD3DDevice->SetGammaRamp( 0, D3DSGR_NO_CALIBRATION, &m_CurGamma );
}

void ioRenderSystem::ControlContrast( int iContrast )
{
	if( !(m_D3DCaps.Caps2 & D3DCAPS2_FULLSCREENGAMMA) )
		return;

	int iCurColor = 0;
	float fRate = ( 100 + iContrast ) / 100.0f;

	for( int i=0 ; i<256 ; i++ )
	{
		iCurColor = 0x7f7f + fRate * ( (int)m_CurGamma.red[i] - 0x7f7f );
		m_CurGamma.red[i] = (WORD)max( 0, min( iCurColor, 0xffff ) );

		iCurColor = 0x7f7f + fRate * ( (int)m_CurGamma.green[i] - 0x7f7f );
		m_CurGamma.green[i] = (WORD)max( 0, min( iCurColor, 0xffff ) );

		iCurColor = 0x7f7f + fRate * ( (int)m_CurGamma.blue[i] - 0x7f7f );
		m_CurGamma.blue[i] = (WORD)max( 0, min( iCurColor, 0xffff ) );
	}

	m_iCurContrast = iContrast;
	m_pD3DDevice->SetGammaRamp( 0, D3DSGR_NO_CALIBRATION, &m_CurGamma );
}

void ioRenderSystem::SetRenderTarget( ioRenderTexture *pRTT )
{
	m_pD3DDevice->SetRenderTarget( 0, pRTT->GetTextureSurface() );
	m_pD3DDevice->SetDepthStencilSurface( pRTT->GetDepthSurface() );
}

void ioRenderSystem::RestoreRenderTarget()
{
	if( m_pRenderTarget )
	{
		m_pD3DDevice->SetRenderTarget( 0, m_pRenderTarget );
	}

	if( m_pDepthSurface )
	{
		m_pD3DDevice->SetDepthStencilSurface( m_pDepthSurface );
	}
}

bool ioRenderSystem::CopyRenderTarget( IDirect3DSurface9 *pDest,
									   RECT *pRcDest,
									   D3DTEXTUREFILTERTYPE eFilter )
{
	if( m_pD3DDevice && m_pRenderTarget )
	{
		HRESULT hr = m_pD3DDevice->StretchRect( m_pRenderTarget,
												NULL,
												pDest,
												pRcDest,
												eFilter );
		if( SUCCEEDED(hr) )
			return true;
	}

	return false;
}

void ioRenderSystem::ClearBack( DWORD dwFlags, DWORD dwColor, const D3DRECT *pRect )
{
	if( m_bDeviceLost )
		return;

	HRESULT hr;
	if( !pRect )
		hr = m_pD3DDevice->Clear( 0, NULL, dwFlags, dwColor, 1.0f, 0L );
	else
		hr = m_pD3DDevice->Clear( 1, pRect, dwFlags, dwColor, 1.0f, 0L );

	if( FAILED(hr) )
	{
		ErrorReport::SetFormat( "ClearBack: %x", hr );
		LOG.PrintTimeAndLog( 0, "ioRenderSystem::ClearBack Failed(%x)", hr );
	}
}

bool ioRenderSystem::BeginScene()
{
	if( m_bDeviceLost )
		return false;

	HRESULT hr = m_pD3DDevice->BeginScene();
	if( FAILED( hr ) )
	{
		LOG.PrintTimeAndLog( 0, "ioRenderSystem::BeginScene Failed(%x)", hr );
		return false;
	}

	return true;
}

bool ioRenderSystem::EndScene()
{
	if( m_bDeviceLost )
		return false;

	HRESULT hr = m_pD3DDevice->EndScene();
	if( FAILED(hr) )
	{
		ErrorReport::SetFormat( "EndScene: %x", hr );
		LOG.PrintTimeAndLog( 0, "ioRenderSytem::EndScene Failed(%x)", hr );
		return false;
	}

	return true;
}

bool ioRenderSystem::Present()
{
	if( m_bDeviceLost )
		return true;

	if( m_pD3DDevice == NULL )
		return true;

	HRESULT hr = m_pD3DDevice->TestCooperativeLevel();
	if( hr == D3DERR_DEVICELOST || hr == D3DERR_DEVICENOTRESET )
	{
		m_bDeviceLost = true;
		m_bCaptureShotSafe = false;
		return true;
	}
	else
	{
		hr = m_pD3DDevice->Present( NULL, NULL, NULL,NULL );
		if( hr == D3DERR_DEVICELOST )
		{
			m_bDeviceLost = true;
			m_bCaptureShotSafe = false;
			return true;
		}
		else if( FAILED(hr) )
		{
			ErrorReport::SetFormat( "Present: %x", hr );
			LOG.PrintTimeAndLog( 0, "ioRenderSytem::Present Failed(%x)", hr );
			return false;
		}
	}

	m_bNowDeviceResetted = false;

	return true;
}

bool ioRenderSystem::CheckLostDevice( DWORD dwSleepTime )
{
	if( !m_bDeviceLost )
		return true;

	HRESULT hr = m_pD3DDevice->TestCooperativeLevel();
	if( hr == D3DERR_DEVICELOST )
	{
		//LOG.PrintTimeAndLog( 0, "ioRenderSystem::CheckLostDevice - DeviceLost Sleep(%d)", dwSleepTime );
		::SleepEx( dwSleepTime, FALSE );
		return false;
	}
	else if( hr == D3DERR_DEVICENOTRESET )
	{
		hr = Reset3DEnvironment();
		if( SUCCEEDED(hr) )
			return true;

		if( hr == D3DERR_DEVICELOST )
			m_bDeviceLost = true;

		m_bCaptureShotSafe = false;
	}
	else
	{
		LOG.PrintTimeAndLog( 0, "ioRenderSystem::CheckLostDevice - Exception(%x)", hr );
	}

	return false;
}

void ioRenderSystem::HandlePossibleSizeChange()
{
	if( !m_D3DParam.Windowed )
		return;

	RECT rcClient;
	GetClientRect( m_hWnd, &rcClient );

	int iWidth  = rcClient.right - rcClient.left;
	int iHeight = rcClient.bottom - rcClient.top;

	if( iWidth <= 0 || iHeight <= 0 )
		return;

	if( m_uClientWidth == (UINT)iWidth && m_uClientHeight == (UINT)iHeight )
		return;

	m_uClientWidth  = (UINT)iWidth;
	m_uClientHeight = (UINT)iHeight;

	m_D3DParam.BackBufferWidth  = m_uClientWidth;
	m_D3DParam.BackBufferHeight = m_uClientHeight;

	LOG.PrintTimeAndLog( 0, "ioRenderSystem::HandlePossibleSizeChange - BackBuffer (%d/%d)",
							m_uClientWidth,
							m_uClientHeight );

	ApplyChangedPresentParameters();
}

bool ioRenderSystem::ChangeMultiSamplingType( D3DMULTISAMPLE_TYPE eType )
{
	if( eType != D3DMULTISAMPLE_NONE )
	{
		if( !IsEnableMultiSamplingType( eType ) )
			return false;
	}

	if( m_D3DParam.MultiSampleType == eType )
		return false;

	m_D3DParam.MultiSampleType = eType;
	m_D3DParam.MultiSampleQuality = 0;

	if( eType != D3DMULTISAMPLE_NONE )
		m_D3DParam.Flags &= ~D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
	else
		m_D3DParam.Flags |= D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

	return true;
}

bool ioRenderSystem::ChangeScreenMode( bool bFullScreen )
{
	if( bFullScreen )
		m_D3DParam.Windowed = FALSE;
	else
		m_D3DParam.Windowed = TRUE;

	UINT uFullScreenRefresh = m_pEnumDisplay->GetRefresh( m_D3DParam.BackBufferFormat,
														  m_D3DParam.BackBufferWidth,
														  m_D3DParam.BackBufferHeight );

	if( m_D3DParam.Windowed )
		m_D3DParam.FullScreen_RefreshRateInHz = 0;
	else
		m_D3DParam.FullScreen_RefreshRateInHz = uFullScreenRefresh;

	return true;
}

bool ioRenderSystem::ChangeBackBufferFormat( bool bUse16BitFormat )
{
	D3DFORMAT eNewBackBufferFmt;
	if( bUse16BitFormat )
		eNewBackBufferFmt = D3DFMT_R5G6B5;
	else
		eNewBackBufferFmt = D3DFMT_X8R8G8B8;

	if( eNewBackBufferFmt == m_BackSurfaceDesc.Format )
		return false;

	m_D3DParam.BackBufferFormat = eNewBackBufferFmt;
	return true;
}

bool ioRenderSystem::ChangeBackBufferSize( UINT uWidth, UINT uHeight )
{
	if( m_D3DParam.BackBufferWidth == uWidth && m_D3DParam.BackBufferHeight == uHeight )
		return false;

	m_D3DParam.BackBufferWidth  = uWidth;
	m_D3DParam.BackBufferHeight = uHeight;

	UINT uFullScreenRefresh = m_pEnumDisplay->GetRefresh( m_D3DParam.BackBufferFormat,
														  uWidth,
														  uHeight );

	if( m_D3DParam.Windowed )
		m_D3DParam.FullScreen_RefreshRateInHz = 0;
	else
		m_D3DParam.FullScreen_RefreshRateInHz = uFullScreenRefresh;

	m_uClientWidth  = uWidth;
	m_uClientHeight = uHeight;

	return true;
}

bool ioRenderSystem::ChangeSwapEffect( D3DSWAPEFFECT eSwapEffect )
{
	if( m_D3DParam.SwapEffect == eSwapEffect )
		return false;

	m_D3DParam.SwapEffect = eSwapEffect;
	return true;
}

bool ioRenderSystem::IsEnableMultiSamplingType( D3DMULTISAMPLE_TYPE eType )
{
	if( !IsPossibleMultiSampling() )
		return false;

	int iEnableCount = m_EnableMuntiSampleList.size();
	for( int i=0 ; i<iEnableCount ; i++ )
	{
		if( m_EnableMuntiSampleList[i] == eType )
			return true;
	}

	return false;
}

D3DMULTISAMPLE_TYPE ioRenderSystem::GetEnableMultiSampleType( int iIndex ) const
{
	if( COMPARE( iIndex, 0, GetEnableMultiSampleTypeCount() ) )
		return m_EnableMuntiSampleList[iIndex];

	return D3DMULTISAMPLE_NONE;
}

bool ioRenderSystem::IsPossibleMultiSampling() const
{
	return !m_EnableMuntiSampleList.empty();
}

bool ioRenderSystem::IsNowMultiSamplingOn() const
{
	return m_bNowMultiSampleOn;
}

void ioRenderSystem::EnableMultiSampling( bool bEnable )
{
	if( !m_pD3DDevice )
		return;

	if( m_bNowMultiSampleOn == bEnable )
		return;

	if( !IsPossibleMultiSampling() )
		return;

	HRESULT hr;
	if( bEnable )
	{
		hr = m_pD3DDevice->SetRenderState( D3DRS_MULTISAMPLEANTIALIAS, TRUE );
		if( FAILED(hr) )
		{
			ErrorReport::SetFormat( "EnableMultiSampling1: %x", hr );
			LOG.PrintTimeAndLog( 0, "ioRenderSystem::EnableMultiSampling - On Failed(%x)", hr );
			m_bNowMultiSampleOn = false;
		}
		else
		{
			m_bNowMultiSampleOn = true;
		}
	}
	else
	{
		hr = m_pD3DDevice->SetRenderState( D3DRS_MULTISAMPLEANTIALIAS, FALSE );
		if( FAILED(hr) )
		{
			ErrorReport::SetFormat( "EnableMultiSampling2: %x", hr );
			LOG.PrintTimeAndLog( 0, "ioRenderSystem::EnableMultiSampling - Off Failed(%x)", hr );
		}

		m_bNowMultiSampleOn = false;
	}
}

void ioRenderSystem::SetPresentationInterval()
{
	m_D3DParam.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;

	ApplyChangedPresentParameters();
}

bool ioRenderSystem::ApplyChangedPresentParameters()
{
	if( !m_pD3DDevice )	
		return false;

	HRESULT hr = Reset3DEnvironment();
	if( FAILED(hr) )
	{
		ErrorReport::SetFormat( "ApplyChangedPresentParameters: %x", hr );
		m_bDeviceLost = true;
		m_bCaptureShotSafe = false;
		return false;
	}

	return true;
}

HRESULT ioRenderSystem::Reset3DEnvironment()
{
	LOG.PrintTimeAndLog( 0, "Reset Resource" );

	m_bCaptureShotSafe = false;

	if( ioShaderManager::GetSingletonPtr() )
		ioShaderManager::GetSingleton().OnLostDevice();

	if( ioVertexBufferManager::GetSingletonPtr() )
		ioVertexBufferManager::GetSingleton().ReleaseDefaultPoolResources();

	if( ioIndexBufferManager::GetSingletonPtr() )
		ioIndexBufferManager::GetSingleton().ReleaseDefaultPoolResources();

	if( ioTextureManager::GetSingletonPtr() )
		ioTextureManager::GetSingleton().ReleaseDefaultPoolTextures();
	
	ClearRenderStateDesc();

	EnterCriticalSection( &m_CaptureSection );
	SAFERELEASE(m_pCaptureTarget);
	LeaveCriticalSection( &m_CaptureSection );

	HRESULT hr = m_pD3DDevice->Reset( &m_D3DParam );
	if( FAILED(hr) )
	{
		ErrorReport::SetFormat( "Reset : %x", hr );
		LOG.PrintTimeAndLog( 0, "Reset Failed(%x)", hr );

		if( hr == D3DERR_INVALIDCALL )
		{
			LOG.PrintTimeAndLog( 0, "Reset Failed, BackCnt: %d", m_D3DParam.BackBufferCount );
			LOG.PrintTimeAndLog( 0, "Reset Failed, BackWidth: %d", m_D3DParam.BackBufferWidth );
			LOG.PrintTimeAndLog( 0, "Reset Failed, BackHeight: %d", m_D3DParam.BackBufferHeight );
			LOG.PrintTimeAndLog( 0, "Reset Failed, BackFormat: %d", m_D3DParam.BackBufferFormat );

			LOG.PrintTimeAndLog( 0, "Reset Failed, SampleType: %d", m_D3DParam.MultiSampleType );
			LOG.PrintTimeAndLog( 0, "Reset Failed, SampleQuality: %d", m_D3DParam.MultiSampleQuality );
			
			LOG.PrintTimeAndLog( 0, "Reset Failed, SwapEffect: %d", m_D3DParam.SwapEffect );
			LOG.PrintTimeAndLog( 0, "Reset Failed, Windowed: %d", m_D3DParam.Windowed );
			LOG.PrintTimeAndLog( 0, "Reset Failed, Stencil: %d", m_D3DParam.EnableAutoDepthStencil );
			LOG.PrintTimeAndLog( 0, "Reset Failed, StencilType: %d", m_D3DParam.AutoDepthStencilFormat );

			LOG.PrintTimeAndLog( 0, "Reset Failed, Flags: %d", m_D3DParam.Flags );

			LOG.PrintTimeAndLog( 0, "Reset Failed, FullScreen: %d", m_D3DParam.FullScreen_RefreshRateInHz );
			LOG.PrintTimeAndLog( 0, "Reset Failed, PresentInterval: %d", m_D3DParam.PresentationInterval );
		}

		return hr;
	}

	CreateCaptureTarget( true );

	if( ioTextureManager::GetSingletonPtr() )
		ioTextureManager::GetSingleton().ReCreateDefaultPoolTextures();

	if( ioIndexBufferManager::GetSingletonPtr() )
		ioIndexBufferManager::GetSingleton().ReCreateDefaultPoolResources();

	if( ioVertexBufferManager::GetSingletonPtr() )
		ioVertexBufferManager::GetSingleton().ReCreateDefaultPoolResources();

	if( ioShaderManager::GetSingletonPtr() )
		ioShaderManager::GetSingleton().OnResetDevice();

	if( ioFontManager::GetSingletonPtr() )
		ioFontManager::GetSingleton().Reset();

	m_bDeviceLost = false;
	m_bNowDeviceResetted = true;

	UpdateSurfaceInfo();

	LOG.PrintTimeAndLog( 0, "Reset Done" );

	return S_OK;
}

bool ioRenderSystem::CreateCaptureTarget( bool bReCreateMust )
{
	if( bReCreateMust )
	{
		SAFERELEASE( m_pCaptureTarget );
	}

	if( m_pCaptureTarget )
		return true;

	HRESULT hr;
	hr = m_pD3DDevice->CreateRenderTarget( m_D3DParam.BackBufferWidth,
										   m_D3DParam.BackBufferHeight,
										   m_D3DParam.BackBufferFormat,
										   D3DMULTISAMPLE_NONE,
										   0,
										   TRUE,
										   &m_pCaptureTarget,
										   NULL );

	if( FAILED(hr) )
	{
		ErrorReport::SetFormat( "%x", hr );
		LOG.PrintTimeAndLog( 0, "ioRenderSystem::CreateCaptureTarget - Failed(%x)", hr );
		return false;
	}

	return true;
}

bool ioRenderSystem::PrePareThreadCapture( D3DTEXTUREFILTERTYPE eFilter, float fSizeRate )
{
	m_bCaptureShotSafe = false;

	if( !m_pD3DDevice )
	{
		LOG.PrintTimeAndLog( 0, "ioRenderSystem::PrePareThreadCapture - Initialize First!!" );
		return false;
	}

	if( !CreateCaptureTarget( false ) )
		return false;

	fSizeRate = max( 0.1f, min( fSizeRate, 1.0f ) );

	UINT uWidth, uHeight;
	uWidth  = m_D3DParam.BackBufferWidth * fSizeRate;
	uHeight = m_D3DParam.BackBufferHeight * fSizeRate;

	RECT rcArea;
	SetRect( &rcArea, 0, 0, uWidth, uHeight );

	if( !CopyRenderTarget( m_pCaptureTarget, &rcArea, eFilter ) )
		return false;

	m_bCaptureShotSafe = true;

	return true;
}

IDirect3DSurface9* ioRenderSystem::BeginThreadCapture()
{
	EnterCriticalSection( &m_CaptureSection );
	return m_pCaptureTarget;
}

void ioRenderSystem::EndThreadCapture()
{
	LeaveCriticalSection( &m_CaptureSection );
}

bool ioRenderSystem::CaptureScreen( const char *szFileName )
{
	if( !m_pD3DDevice )
	{
		LOG.PrintTimeAndLog( 0, "ioRenderSystem::CaptureScreen - Initialize First!!" );
		return false;
	}

	HRESULT hr;

	if( m_D3DParam.BackBufferFormat == D3DFMT_R5G6B5 && !m_D3DParam.Windowed )
	{
		IDirect3DSurface9 *pBackSurface = NULL;

		hr = m_pD3DDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackSurface );
		if( FAILED(hr) )
		{
			ErrorReport::SetFormat( "CaptureScreen1: %x", hr );
			LOG.PrintTimeAndLog( 0, "ioRenderSystem::CaptureScreen - GetBackBuffer Failed(%x)", hr );
			return false;
		}

		bool bSource16Bits = false;
		if( m_D3DParam.BackBufferFormat == D3DFMT_R5G6B5 )
			bSource16Bits = true;

		hr = SaveSurfaceToFile( pBackSurface,
								m_D3DParam.BackBufferWidth,
								m_D3DParam.BackBufferHeight,
								bSource16Bits,
								szFileName );

		SAFERELEASE( pBackSurface );
	}
	else
	{
		IDirect3DSurface9 *pTempSurface = NULL;
		hr = m_pD3DDevice->CreateOffscreenPlainSurface( m_D3DParam.BackBufferWidth,
														m_D3DParam.BackBufferHeight,
														D3DFMT_A8R8G8B8,	// Front버퍼는 8888포멧만이 허용
														D3DPOOL_SCRATCH,
														&pTempSurface,
														NULL );

		if( FAILED(hr) )
		{
			ErrorReport::SetFormat( "CaptureScreen2: %x", hr );
			LOG.PrintTimeAndLog( 0, "ioRenderSystem::CaptureScreen - Front Temp Buffer Failed(%x)", hr );
			return false;
		}

		hr = m_pD3DDevice->GetFrontBufferData( 0, pTempSurface );
		if( FAILED(hr) )
		{
			ErrorReport::SetFormat( "CaptureScreen3: %x", hr );
			LOG.PrintTimeAndLog( 0, "ioRenderSystem::CaptureScreen - GetFrontBufferData Failed(%x)", hr );
			SAFERELEASE( pTempSurface );
			return false;
		}

		hr = SaveSurfaceToFile( pTempSurface,
								m_D3DParam.BackBufferWidth,
								m_D3DParam.BackBufferHeight,
								false,
								szFileName );

		SAFERELEASE( pTempSurface );
	}

	if( FAILED(hr) )
	{
		ErrorReport::SetFormat( "CaptureScreen4: %x", hr );
		LOG.PrintTimeAndLog( 0, "ioRenderSystem::CaptureScreen - SaveSurfaceToFile Failed(%x)", hr );
		return false;
	}

	return true;
}

void ioRenderSystem::UpdateSurfaceInfo()
{
	if( !m_pD3DDevice )	return;

	IDirect3DSurface9 *pBackSurface = NULL;
	m_pD3DDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackSurface );
	if( pBackSurface )
	{
		pBackSurface->GetDesc( &m_BackSurfaceDesc );
		pBackSurface->Release();
	}

	m_pRenderTarget = NULL;
	m_pD3DDevice->GetRenderTarget( 0, &m_pRenderTarget );
	if( m_pRenderTarget )
	{
		m_pRenderTarget->Release();
	}

	m_pDepthSurface = NULL;
	m_pD3DDevice->GetDepthStencilSurface( &m_pDepthSurface );
	if( m_pDepthSurface )
	{
		m_pDepthSurface->Release();
	}

	m_pD3DDevice->GetViewport( &m_ViewPort );
}

void ioRenderSystem::CheckMaxMultiSampling( D3DFORMAT eFormat, BOOL bWindow )
{
	if( !m_pD3D )	return;

	HRESULT hr;
	DWORD dwMaxQuality = 0;

	m_EnableMuntiSampleList.clear();

	for( int i=D3DMULTISAMPLE_2_SAMPLES ; i<=D3DMULTISAMPLE_16_SAMPLES ; i+=2 )
	{
		hr = m_pD3D->CheckDeviceMultiSampleType( D3DADAPTER_DEFAULT,
												 D3DDEVTYPE_HAL,
												 eFormat,
												 bWindow,
												 (D3DMULTISAMPLE_TYPE)i,
												 &dwMaxQuality );

		if( SUCCEEDED(hr) )
		{
			m_EnableMuntiSampleList.push_back( (D3DMULTISAMPLE_TYPE)i );
		}
	}

	if( m_EnableMuntiSampleList.empty() )
	{
		LOG.PrintTimeAndLog( 0, "Not Support MultiSampleType" );
	}
	else
	{
		LOG.PrintTimeAndLog( 0, "Support %d MultiSample", m_EnableMuntiSampleList.size() );
	}
}

void ioRenderSystem::ClearRenderStateDesc()
{
	s_RenderStateDesc.InitStateDesc();
}

void ioRenderSystem::ClearStreamDesc()
{
	s_RenderStateDesc.ClearStreamDesc();
}

void ioRenderSystem::ClearRenderedCounts()
{
	m_iRenderedFaceCount = 0;
	m_iRenderCallCount = 0;
}

ShadowMapType ioRenderSystem::CheckEnableShadowMapType()
{
	m_ShadowMapType = SMT_NOT_ENABLE;

	HRESULT hr;
	hr = m_pD3D->CheckDeviceFormat( D3DADAPTER_DEFAULT,
									D3DDEVTYPE_HAL,
									m_BackSurfaceDesc.Format,
									D3DUSAGE_RENDERTARGET,
									D3DRTYPE_TEXTURE,
									D3DFMT_R32F );

	if( SUCCEEDED( hr ) )
	{
		m_ShadowMapType = SMT_R32F_TEX;
		LOG.PrintTimeAndLog( 0, "R32F Texture Support" );
	}
	else
	{
		hr = m_pD3D->CheckDeviceFormat( D3DADAPTER_DEFAULT,
										D3DDEVTYPE_HAL,
										m_BackSurfaceDesc.Format,
										D3DUSAGE_DEPTHSTENCIL,
										D3DRTYPE_TEXTURE,
										D3DFMT_D16 );

		if( SUCCEEDED( hr ) )
		{
			m_ShadowMapType = SMT_DEPTH_BUF;
			LOG.PrintTimeAndLog( 0, "DepthTexture Support" );
		}
		else
		{
			LOG.PrintTimeAndLog( 0, "Unsupport ShadowMap" );
		}
	}

	return m_ShadowMapType;
}

void ioRenderSystem::SetTextureMipMapBias( int iStage, float fBias )
{
	if( s_RenderStateDesc.m_TexStageDesc[iStage].fMipMapBias != fBias )
	{
		if( m_D3DCaps.RasterCaps & D3DPRASTERCAPS_MIPMAPLODBIAS )
		{
			HRESULT hr;
			hr = m_pD3DDevice->SetSamplerState( iStage, D3DSAMP_MIPMAPLODBIAS, *((DWORD*)(&fBias)) );
			if( FAILED(hr) )
			{
				ErrorReport::SetFormat( "SetTextureMipMapBias: %x", hr );
				LOG.PrintTimeAndLog( 0, "ioRenderSystem::SetTextureMipMapBias - Failed(%x)", hr );
			}
		}

		s_RenderStateDesc.m_TexStageDesc[iStage].fMipMapBias = fBias;
	}	
}

ioRenderSystem *pRenderSystem = NULL;

ioRenderSystem& RenderSystem()
{
	if( !pRenderSystem )
	{
		pRenderSystem = new ioRenderSystem;
	}

	return *pRenderSystem;
}

void ReleaseRenderSystem()
{
	SAFEDELETE( pRenderSystem );
}



