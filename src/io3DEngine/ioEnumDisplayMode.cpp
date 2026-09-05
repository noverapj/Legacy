

#include "stdafx.h"

#include "ioEnumDisplayMode.h"
#include "HelpFunc.h"

struct MonitorModeLess
{
	bool operator()( const ioEnumDisplayMode::MonitorMode *pA,
					 const ioEnumDisplayMode::MonitorMode *pB ) const
	{
		if( pA->uWidth < pB->uWidth )
			return true;
		else if( pA->uWidth == pB->uWidth )
		{
			if( pA->uHeight < pB->uHeight )
				return true;
			else if( pA->uHeight == pB->uHeight )
			{
				if( pA->uPixel < pB->uPixel )
					return true;
			}
		}

		return false;
	}
};

ioEnumDisplayMode::ioEnumDisplayMode()
{
	m_uMinWidth  = MIN_WIDTH;
	m_uMinHeight = MIN_HEIGHT;
	m_uMaxWidth  = MIN_WIDTH;
	m_uMaxHeight = MIN_HEIGHT;

	m_uStartRefresh = SAFE_REFRESH;
}

ioEnumDisplayMode::~ioEnumDisplayMode()
{
	ClearAllEnumInfo();
}

void ioEnumDisplayMode::SetMinWidthHeight( UINT uMinWidth, UINT uMinHeight )
{
	m_uMinWidth  = max( MIN_WIDTH, uMinWidth );
	m_uMinHeight = max( MIN_HEIGHT, uMinHeight );
}

void ioEnumDisplayMode::ClearAllEnumInfo()
{
	MonitorModeList::iterator iter = m_MonitorModeList.begin();
	for( ; iter!=m_MonitorModeList.end() ; ++iter )
	{
		delete *iter;
	}
	m_MonitorModeList.clear();

	m_16BitsAdapters.clear();
	m_32BitsAtapters.clear();
}

void ioEnumDisplayMode::CheckEnableDisplayMode( IDirect3D9 *pD3D, D3DFORMAT e16Bit, D3DFORMAT e32Bit )
{
	m_uMaxWidth  = GetSystemMetrics(SM_CXSCREEN);
	m_uMaxHeight = GetSystemMetrics(SM_CYSCREEN);

	ClearAllEnumInfo();

	CheckCurrentDisplayMode();
	CheckMonitorDisplayMode();

	CheckAdapterDisplayMode( pD3D, e16Bit );
	CheckAdapterDisplayMode( pD3D, e32Bit );
}

void ioEnumDisplayMode::CheckCurrentDisplayMode()
{
	//dmDriverExtra변수를 0으로 세팅해야함 ZeroMemory가 대신 0으로 세팅
	DEVMODE kDevMode;
	ZeroMemory( &kDevMode, sizeof(kDevMode) );
	kDevMode.dmSize = sizeof(DEVMODE);

	if( EnumDisplaySettingsEx( NULL, ENUM_CURRENT_SETTINGS, &kDevMode, 0 ) )
	{
		m_uStartRefresh = kDevMode.dmDisplayFrequency;
	}
	else
	{
		// 실패시 최대한 보수적인 수치로 설정
		m_uStartRefresh = SAFE_REFRESH;
	}
}

void ioEnumDisplayMode::CheckMonitorDisplayMode()
{
	for( DWORD dwMode = 0 ; true ; dwMode++ )
	{
		//dmDriverExtra변수를 0으로 세팅해야함 ZeroMemory가 대신 0으로 세팅
		DEVMODE kDevMode;
		ZeroMemory( &kDevMode, sizeof(kDevMode) );
		kDevMode.dmSize = sizeof(DEVMODE);

		if( !EnumDisplaySettingsEx( NULL, dwMode, &kDevMode, 0 ) )
			break;

		if( kDevMode.dmBitsPerPel != 16 && kDevMode.dmBitsPerPel != 32 )
			continue;

		if( kDevMode.dmPelsWidth < m_uMinWidth || kDevMode.dmPelsHeight < m_uMinHeight )
			continue;

		if( kDevMode.dmPelsWidth > m_uMaxWidth || kDevMode.dmPelsHeight > m_uMaxHeight )
			continue;

		InsertMonitorMode( kDevMode );
	}

	std::sort( m_MonitorModeList.begin(), m_MonitorModeList.end(), MonitorModeLess() );
}

void ioEnumDisplayMode::InsertMonitorMode( const DEVMODE &rkMode )
{
	UINT uWidth   = rkMode.dmPelsWidth;
	UINT uHeight  = rkMode.dmPelsHeight;
	UINT uPixel   = rkMode.dmBitsPerPel;
	UINT uRefresh = rkMode.dmDisplayFrequency;

	MonitorModeList::iterator iter = m_MonitorModeList.begin();
	for( ; iter!=m_MonitorModeList.end() ; ++iter )
	{
		MonitorMode *pMode = *iter;

		if( pMode->uWidth == uWidth &&
			pMode->uHeight == uHeight &&
			pMode->uPixel  == uPixel )
		{
			MonitorMode::RefreshList::iterator iRefresh = pMode->vRefresh.begin();
			for( ; iRefresh != pMode->vRefresh.end() ; ++iRefresh )
			{
				if( *iRefresh == uRefresh )	// 이미 같은게 존재한다.
					return;
			}

			pMode->vRefresh.push_back( uRefresh );
			return;
		}
	}

	MonitorMode *pNewMode = new MonitorMode;
	pNewMode->uWidth  = uWidth;
	pNewMode->uHeight = uHeight;
	pNewMode->uPixel  = uPixel;
	pNewMode->vRefresh.push_back( uRefresh );

	m_MonitorModeList.push_back( pNewMode );
}

bool ioEnumDisplayMode::IsMonitorEnableMode( const D3DDISPLAYMODE &rkMode ) const
{
	UINT uWidth   = rkMode.Width;
	UINT uHeight  = rkMode.Height;
	UINT uPixel   = GetPixelWidth( rkMode.Format );
	UINT uRefresh = rkMode.RefreshRate;

	MonitorModeList::const_iterator iter = m_MonitorModeList.begin();
	for( ; iter!=m_MonitorModeList.end() ; ++iter )
	{
		const MonitorMode *pMode = *iter;

		if( pMode->uWidth == uWidth &&
			pMode->uHeight == uHeight &&
			pMode->uPixel == uPixel )
		{
			MonitorMode::RefreshList::const_iterator iRefresh;
			iRefresh = std::find( pMode->vRefresh.begin(), pMode->vRefresh.end(), uRefresh );

			if( iRefresh != pMode->vRefresh.end() )
				return true;

			return false;
		}
	}

	return false;
}

ioEnumDisplayMode::AdapterModeList* ioEnumDisplayMode::GetAdapterModeListByFormat( D3DFORMAT eFormat )
{
	int iPixel = GetPixelWidth( eFormat );
	if( iPixel == 16 )
		return &m_16BitsAdapters;
	else if( iPixel == 32 )
		return &m_32BitsAtapters;

	return NULL;
}

void ioEnumDisplayMode::CheckAdapterDisplayMode( IDirect3D9 *pD3D, D3DFORMAT eFormat )
{
	AdapterModeList *pModeList = GetAdapterModeListByFormat( eFormat );
	if( !pModeList )	return;

	UINT uNumAdapterModes = pD3D->GetAdapterModeCount( D3DADAPTER_DEFAULT, eFormat );
	pModeList->clear();
	pModeList->reserve( uNumAdapterModes );

	D3DDISPLAYMODE kDisplayMode;
	for( UINT uMode=0 ; uMode<uNumAdapterModes ; uMode++ )
	{
		pD3D->EnumAdapterModes( D3DADAPTER_DEFAULT, eFormat, uMode, &kDisplayMode );

		if( IsMonitorEnableMode( kDisplayMode ) )
		{
			AdapterMode kMode;
			kMode.uWidth  = kDisplayMode.Width;
			kMode.uHeight = kDisplayMode.Height;
			pModeList->push_back( kMode );
		}
	}

	if( pModeList->size() > 1 )
	{
		std::sort( pModeList->begin(), pModeList->end() );
	}
}

int ioEnumDisplayMode::GetNumDisplayMode( D3DFORMAT eFormat )
{
	const AdapterModeList *pModeList = GetAdapterModeListByFormat( eFormat );
	if( pModeList )
		return pModeList->size();

	return 0;
}

bool ioEnumDisplayMode::GetResolution( D3DFORMAT eFormat, int iIndex, UINT &uWidth, UINT &uHeight )
{
	const AdapterModeList *pModeList = GetAdapterModeListByFormat( eFormat );
	if( pModeList )
	{
		int iModeCount = pModeList->size();
		if( COMPARE( iIndex, 0, iModeCount ) )
		{
			const AdapterMode &rkMode = (*pModeList)[iIndex];
			uWidth  = rkMode.uWidth;
			uHeight = rkMode.uHeight;
			return true;
		}
	}

	return false;
}

UINT ioEnumDisplayMode::GetRefresh( D3DFORMAT eFormat, UINT uWidth, UINT uHeight )
{
	const AdapterModeList *pModeList = GetAdapterModeListByFormat( eFormat );
	if( !pModeList )	return SAFE_REFRESH;

	UINT uPixel = (UINT)GetPixelWidth( eFormat );

	MonitorModeList::iterator iter = m_MonitorModeList.begin();
	for( ; iter!=m_MonitorModeList.end() ; ++iter )
	{
		MonitorMode *pMode = *iter;

		if( pMode->uWidth != uWidth || pMode->uHeight != uHeight || pMode->uPixel != uPixel )
			continue;

		MonitorMode::RefreshList::iterator iRefresh;
		iRefresh = std::find( pMode->vRefresh.begin(), pMode->vRefresh.end(), m_uStartRefresh );
		if( iRefresh != pMode->vRefresh.end() )
			return m_uStartRefresh;

		// 최초 사용중인 Hz가 이 해상도에서 지원되지 않는경우
		// 다른 Hz를 찾아볼수도 있지만 이마저도 지원 안될수가 있다. ( 특히 CRT )
		// 윈도우 바탕화면에서 설정가능한 hz의 경우도 가능목록에 있지만 막상해보면 실패 ( 1280 * 960, 72hz )
		
		return SAFE_REFRESH;	// 결국 안전한 60Hz를 리턴..
	}

	return SAFE_REFRESH;
}
