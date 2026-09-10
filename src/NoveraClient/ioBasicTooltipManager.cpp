#include "stdafx.h"
#include "ioBasicToolTipManager.h"
#include "GUI/ioBasicToolTip.h"

template<> ioBasicTooltipManager* Singleton< ioBasicTooltipManager >::ms_Singleton = 0;
ioBasicTooltipManager& ioBasicTooltipManager::GetSingleton()
{
	return Singleton< ioBasicTooltipManager >::GetSingleton();
}

ioBasicTooltipManager::ioBasicTooltipManager()
{
	m_pToolTipWnd = NULL;
}

ioBasicTooltipManager::~ioBasicTooltipManager()
{
	ClearAllGUI();
}

void ioBasicTooltipManager::Initialize()
{
}

void ioBasicTooltipManager::ClearAllGUI()
{	
}

void ioBasicTooltipManager::InitDeskTop( int iWidth, int iHeight )
{
	RECT rcRect;
	SetRect( &rcRect, 0, 0, iWidth, iHeight );
	
	m_pToolTipWnd = new ioWnd;
	m_pToolTipWnd->SetID( BASIC_TOOLTIP_MGR );
	m_pToolTipWnd->SetWndStyle( IWS_DRAG_DROP );
	m_pToolTipWnd->iwm_create();
	m_pToolTipWnd->SetWndRect( rcRect );	

	ExcuteReAlloc();
	ExcuteReserveAlloc();

	//GUIMananger에서 Init이 호출 될때마다 GUIMananger의 차일드가 모두 제거 되기 때문에 다시 등록
	g_GUIMgr.AddDeskTopChild( m_pToolTipWnd );
}

DWORD ioBasicTooltipManager::NewToolTip( const std::string& szTooltipXmlName )
{
	if( szTooltipXmlName.empty() )
		return 0;

	char szFileName[MAX_PATH];
	sprintf_s( szFileName, "XML/tooltip/%s", szTooltipXmlName.c_str() );

	ioBasicToolTip* pToolTip = dynamic_cast<ioBasicToolTip*>( g_GUIMgr.AddWnd( szFileName, m_pToolTipWnd ) );
	if( pToolTip )
	{
		DWORD dwID = static_cast<DWORD>( m_TooltipAllocMap.size() ) + 1;

		TooltipAlloc Alloc;
		Alloc.szTooltipXmlName	= szTooltipXmlName;
		Alloc.dwUniqueIdx		= dwID;

		pToolTip->SetID( dwID );
		m_TooltipAllocMap.insert( TooltipAllocMap::value_type( dwID, Alloc ) );

		return dwID;
	}

	return 0;
}

void ioBasicTooltipManager::NewReserveToolTip( const std::string& szTooltipXmlName )
{
	if( szTooltipXmlName.empty() )
		return;

	TooltipAllocVector::iterator iter = m_ReserveAlloc.begin();
	for( ; iter != m_ReserveAlloc.end(); ++iter )
	{
		const TooltipAlloc& Alloc = *iter;
		if( Alloc.szTooltipXmlName == szTooltipXmlName )
			return;
	}

	TooltipAlloc Alloc;
	Alloc.szTooltipXmlName	= szTooltipXmlName;	
	m_ReserveAlloc.push_back( Alloc );
}

void ioBasicTooltipManager::ExcuteReserveAlloc()
{
	TooltipAllocVector::iterator iter = m_ReserveAlloc.begin();
	for( ; iter != m_ReserveAlloc.end(); ++iter )
	{
		const TooltipAlloc& Alloc = *iter;
		NewToolTip( Alloc.szTooltipXmlName );
	}

	m_ReserveAlloc.clear();
}

void ioBasicTooltipManager::ExcuteReAlloc()
{
	char szFileName[MAX_PATH];

	TooltipAllocMap::iterator iter = m_TooltipAllocMap.begin();
	for( ; iter != m_TooltipAllocMap.end(); ++iter )
	{
		const TooltipAlloc& Alloc = iter->second;
		if( Alloc.szTooltipXmlName.empty() )
			continue;
				
		sprintf_s( szFileName, "XML/tooltip/%s", Alloc.szTooltipXmlName.c_str() );
		ioBasicToolTip* pToolTip = dynamic_cast<ioBasicToolTip*>( g_GUIMgr.AddWnd( szFileName, m_pToolTipWnd ) );
		if( pToolTip )
		{
			pToolTip->SetID( Alloc.dwUniqueIdx );
		}
	}
}

ioWnd* ioBasicTooltipManager::FindToolTip( const std::string& szTooltipXmlName )
{
	TooltipAllocMap::iterator iter = m_TooltipAllocMap.begin();
	for( ; iter != m_TooltipAllocMap.end(); ++iter )
	{
		const TooltipAlloc& Alloc = iter->second;
		if( Alloc.szTooltipXmlName == szTooltipXmlName )
		{
			if(m_pToolTipWnd)
				return m_pToolTipWnd->FindChildWnd( Alloc.dwUniqueIdx );
		}
	}

	return NULL;
}

ioWnd* ioBasicTooltipManager::FindToolTip( DWORD dwID )
{	
	if( m_pToolTipWnd )
	{
		return m_pToolTipWnd->FindChildWnd( dwID );
	}

	return NULL;
}

int ioBasicTooltipManager::GetToolTipID( const std::string& szTooltipXmlName )
{
	ioWnd* pWnd = FindToolTip( szTooltipXmlName );
	if( pWnd )
	{
		return pWnd->GetID();
	}

	return 0;
}

void ioBasicTooltipManager::ShowToolTip( DWORD dwID )
{
	if( m_pToolTipWnd )
	{
		m_pToolTipWnd->ShowWnd();
		m_pToolTipWnd->ShowChildWnd( dwID, true );
	}
}

void ioBasicTooltipManager::ShowToolTip( const std::string& szTooltipXmlName )
{
	if( m_pToolTipWnd )
	{
		m_pToolTipWnd->ShowWnd();

		ioWnd* pWnd = FindToolTip( szTooltipXmlName );
		if( pWnd )			
			pWnd->ShowWnd();		
	}
}

void ioBasicTooltipManager::HideToolTip( DWORD dwID )
{
	if( m_pToolTipWnd )
	{
		m_pToolTipWnd->HideWnd();
		m_pToolTipWnd->HideChildWnd( dwID );
	}
}

void ioBasicTooltipManager::HideToolTip( const std::string& szTooltipXmlName )
{
	if( m_pToolTipWnd )
	{
		m_pToolTipWnd->HideWnd();

		ioWnd* pWnd = FindToolTip( szTooltipXmlName );
		if( pWnd )			
			pWnd->HideWnd();
	}
}