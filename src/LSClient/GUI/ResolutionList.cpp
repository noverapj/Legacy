

#include "StdAfx.h"

#include "ResolutionList.h"
#include "PartyVersusWnd.h"

ResolutionList::ResolutionList()
{
	m_pDarkLine = NULL;
	m_pLightLine= NULL;
	m_pOverLine = NULL;

	m_pOpenBtn = NULL;
	m_iOverListIndex = -1;
}

ResolutionList::~ResolutionList()
{
	SAFEDELETE( m_pDarkLine );
	SAFEDELETE( m_pLightLine );
	SAFEDELETE( m_pOverLine );
}

void ResolutionList::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "DarkLine" )
	{
		SAFEDELETE( m_pDarkLine );
		m_pDarkLine = pImage;
	}
	else if( szType == "LightLine" )
	{
		SAFEDELETE( m_pLightLine );
		m_pLightLine = pImage;
	}
	else if( szType == "OverLine" )
	{
		SAFEDELETE( m_pOverLine );
		m_pOverLine = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void ResolutionList::iwm_show()
{
	SetCheckParentOutSide( true );
}

void ResolutionList::iwm_hide()
{
	SetCheckParentOutSide( false );
}

void ResolutionList::OpenList( ioWnd *pOpenBtn, int iCurWidth, int iCurHeight )
{
	if( IsShow() )
	{
		HideWnd();
		return;
	}

	m_pOpenBtn = pOpenBtn;
	m_iOverListIndex = -1;

	int iCount = Setting::GetResolutionCount();

	m_ResolutionList.clear();
	m_ResolutionList.reserve( iCount );

	for( int i=0 ; i<iCount ; i++ )
	{
		Resolution kSolution;

		Setting::GetResolution( i, kSolution.iWidth, kSolution.iHeight );

		if( kSolution.iWidth == iCurWidth && kSolution.iHeight == iCurHeight )
		{
			kSolution.bCheck = true;
		}
		else
		{
			kSolution.bCheck = false;
		}

		m_ResolutionList.push_back( kSolution );
	}

	SetSize( GetWidth(), m_ResolutionList.size() * 21 + 14 );

	ShowWnd();
}

void ResolutionList::OnProcess( float fTimePerSec )
{
	if( !IsShow() )	return;

	m_iOverListIndex = -1;

	// 다른 곳에서 클릭이 일어났는지 확인
	ioWnd *pPreClick = g_GUIMgr.GetPreClickWnd();
	if( pPreClick )
	{
		if( pPreClick != m_pOpenBtn && pPreClick != this )
		{
			HideWnd();
			return;
		}
	}

	int iXPos, iYPos;
	iXPos = GetDerivedPosX() + 7;
	iYPos = GetDerivedPosY() + 7;

	ioLSMouse *pMouse = g_App.GetMouse();

	int iListSize = m_ResolutionList.size();
	for( int i=0 ; i<iListSize ; i++ )
	{
		RECT rcNameRect;
		rcNameRect.left = iXPos;
		rcNameRect.top  = iYPos + i * 21;
		rcNameRect.right  = rcNameRect.left + GetWidth();
		rcNameRect.bottom = rcNameRect.top + 21;

		if( ::PtInRect( &rcNameRect, pMouse->GetMousePos() ) )
		{
			m_iOverListIndex = i;

			if( pMouse->IsLBtnUp() )
			{
				GetParent()->iwm_command( this, IO_RESOLUTION_CHANGE, i );
			}
			break;
		}
	}
}

void ResolutionList::OnRender()
{
	ioWnd::OnRender();

	if( !m_pDarkLine || !m_pLightLine || !m_pOverLine ) 
		return;

	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	bool bDarkRender = false;
	int iListSize = m_ResolutionList.size();
	for( int i=0 ; i<iListSize ; i++ )
	{
		const Resolution &rkResolution = m_ResolutionList[i];
		bDarkRender = !bDarkRender;
		if( m_iOverListIndex == i )
		{
			m_pOverLine->Render( iXPos, iYPos + ( i * 21 ) );
			g_FontMgr.SetTextColor( 255, 255, 255 );
		}
		else if( bDarkRender )
		{
			m_pDarkLine->Render( iXPos, iYPos + ( i * 21 ) );
			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		}
		else
		{
			m_pLightLine->Render( iXPos, iYPos + ( i * 21 ) );
			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		}

		g_FontMgr.PrintText( iXPos + 12, iYPos + 10 + ( i * 21 ), FONT_SIZE_12, STR(1), rkResolution.iWidth, rkResolution.iHeight );
	}
}

//////////////////////////////////////////////////////////////////////////

MovieResolutionList::MovieResolutionList()
{
	m_pDarkLine = NULL;
	m_pLightLine= NULL;
	m_pOverLine = NULL;

	m_pOpenBtn = NULL;
	m_iOverListIndex = -1;
	m_iOptionType = 0;
}

MovieResolutionList::~MovieResolutionList()
{
	SAFEDELETE( m_pDarkLine );
	SAFEDELETE( m_pLightLine );
	SAFEDELETE( m_pOverLine );
}

void MovieResolutionList::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "DarkLine" )
	{
		SAFEDELETE( m_pDarkLine );
		m_pDarkLine = pImage;
	}
	else if( szType == "LightLine" )
	{
		SAFEDELETE( m_pLightLine );
		m_pLightLine = pImage;
	}
	else if( szType == "OverLine" )
	{
		SAFEDELETE( m_pOverLine );
		m_pOverLine = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void MovieResolutionList::iwm_show()
{
	SetCheckParentOutSide( true );
}

void MovieResolutionList::iwm_hide()
{
	SetCheckParentOutSide( false );
}

void MovieResolutionList::OpenList( ioWnd *pOpenBtn, int iType, int iCurValue )
{
	if( IsShow() )
	{
		if( pOpenBtn == m_pOpenBtn )
		{
			HideWnd();
			return;
		}
	}

	if( pOpenBtn == NULL ) return;

	m_pOpenBtn = pOpenBtn;
	m_iOverListIndex = -1;
	m_iOptionType = iType;

	SetWndPos( m_pOpenBtn->GetXPos(), m_pOpenBtn->GetYPos() + 24 );
	
	int iCount = g_MovieMaker.GetMaxMovieOption( m_iOptionType );

	m_ResolutionList.clear();
	m_ResolutionList.reserve( iCount );

	for(int i = 0;i < iCount;i++)
	{
		Resolution kSolution;
		kSolution.iValue = i + 1;
		if( iCurValue == kSolution.iValue )
			kSolution.bCheck = true;
		else
			kSolution.bCheck = false;
		m_ResolutionList.push_back( kSolution );
	}

	SetSize( GetWidth(), m_ResolutionList.size() * 21 + 14 );

	ShowWnd();
}

void MovieResolutionList::OnProcess( float fTimePerSec )
{
	if( !IsShow() )	return;

	m_iOverListIndex = -1;

	// 다른 곳에서 클릭이 일어났는지 확인
	ioWnd *pPreClick = g_GUIMgr.GetPreClickWnd();
	if( pPreClick )
	{
		if( pPreClick != m_pOpenBtn && pPreClick != this )
		{
			HideWnd();
			return;
		}
	}

	int iXPos, iYPos;
	iXPos = GetDerivedPosX() + 7;
	iYPos = GetDerivedPosY() + 7;

	ioLSMouse *pMouse = g_App.GetMouse();

	int iListSize = m_ResolutionList.size();
	for( int i=0 ; i<iListSize ; i++ )
	{
		RECT rcNameRect;
		rcNameRect.left = iXPos;
		rcNameRect.top  = iYPos + i * 21;
		rcNameRect.right  = rcNameRect.left + GetWidth();
		rcNameRect.bottom = rcNameRect.top + 21;

		if( ::PtInRect( &rcNameRect, pMouse->GetMousePos() ) )
		{
			m_iOverListIndex = i;

			if( pMouse->IsLBtnUp() )
			{
				const Resolution &rkResolution = m_ResolutionList[i];
				GetParent()->iwm_command( m_pOpenBtn, IO_RESOLUTION_CHANGE, rkResolution.iValue );
				HideWnd();
			}
			break;
		}
	}
}

void MovieResolutionList::OnRender()
{
	ioWnd::OnRender();

	if( !m_pDarkLine || !m_pLightLine || !m_pOverLine ) 
		return;

	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	bool bDarkRender = false;
	int iListSize = m_ResolutionList.size();
	for( int i=0 ; i<iListSize ; i++ )
	{
		const Resolution &rkResolution = m_ResolutionList[i];
		bDarkRender = !bDarkRender;
		if( m_iOverListIndex == i )
		{
			m_pOverLine->Render( iXPos, iYPos + ( i * 21 ) );
			g_FontMgr.SetTextColor( 255, 255, 255 );
		}
		else if( bDarkRender )
		{
			m_pDarkLine->Render( iXPos, iYPos + ( i * 21 ) );
			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		}
		else
		{
			m_pLightLine->Render( iXPos, iYPos + ( i * 21 ) );
			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		}

		char szTitle[MAX_PATH] = "";
		switch( m_iOptionType )
		{
		case ioMovieMaker::MO_SIZE:			
			sprintf( szTitle, g_MovieMaker.GetSizeTypeText( rkResolution.iValue ).c_str() );
			break;
		case ioMovieMaker::MO_FRAME:
			sprintf( szTitle, g_MovieMaker.GetFrameTypeText( rkResolution.iValue ).c_str() );
			break;
		case ioMovieMaker::MO_QUALITY:
			sprintf( szTitle, g_MovieMaker.GetQualityTypeText( rkResolution.iValue ).c_str() );
			break;
		case ioMovieMaker::MO_SOUND:
			sprintf( szTitle, g_MovieMaker.GetSoundTypeText( rkResolution.iValue ).c_str() );
			break;
		case ioMovieMaker::MO_CODEC:
			sprintf( szTitle, g_MovieMaker.GetCodecTypeText( rkResolution.iValue ).c_str() );
			break;
		}		
		g_FontMgr.PrintText( iXPos + 12, iYPos + 10 + ( i * 21 ), FONT_SIZE_12, szTitle );
	}
}

//------------------------------------------------------------------------------------------------

StringSelectList::StringSelectList()
{
	m_pDarkLine = NULL;
	m_pLightLine= NULL;
	m_pOverLine = NULL;

	m_pNotifier = NULL;
	m_iOverListIndex  = -1;
	m_iPreSelectIndex = -1;
	m_dwNotifyCommand = 0;
}

StringSelectList::~StringSelectList()
{
	SAFEDELETE( m_pDarkLine );
	SAFEDELETE( m_pLightLine );
	SAFEDELETE( m_pOverLine );
}

void StringSelectList::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "DarkLine" )
	{
		SAFEDELETE( m_pDarkLine );
		m_pDarkLine = pImage;
	}
	else if( szType == "LightLine" )
	{
		SAFEDELETE( m_pLightLine );
		m_pLightLine = pImage;
	}
	else if( szType == "OverLine" )
	{
		SAFEDELETE( m_pOverLine );
		m_pOverLine = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void StringSelectList::iwm_show()
{
	SetCheckParentOutSide( true );
}

void StringSelectList::iwm_hide()
{
	SetCheckParentOutSide( false );

	m_pNotifier = NULL;
	m_iOverListIndex  = -1;
	m_iPreSelectIndex = -1;
}

void StringSelectList::OpenList( ioWnd *pNotifier,
								 StringVector &vNameList,
								 int iCurSelectIndex,
								 DWORD dwNotifyCommand )
{
	if( IsShow() || vNameList.empty() )
	{
		HideWnd();
		return;
	}

	m_pNotifier = pNotifier;
	m_iOverListIndex  = -1;
	m_iPreSelectIndex = iCurSelectIndex;
	m_dwNotifyCommand = dwNotifyCommand;

	m_vNameList = vNameList;

	SetSize( GetWidth(), m_vNameList.size() * 21 + 14 );
	ShowWnd();
}

std::string StringSelectList::GetName( int iIndex ) const
{
	int iListCount = m_vNameList.size();
	if( COMPARE( iIndex, 0, iListCount ) )
		return m_vNameList[iIndex];

	return std::string();
}

void StringSelectList::OnProcess( float fTimePerSec )
{
	if( !IsShow() )	return;

	m_iOverListIndex = -1;

	// 다른 곳에서 클릭이 일어났는지 확인
	ioWnd *pPreClick = g_GUIMgr.GetPreClickWnd();
	if( pPreClick )
	{
		if( pPreClick != m_pNotifier && pPreClick != this )
		{
			HideWnd();
			return;
		}
	}

	int iXPos, iYPos;
	iXPos = GetDerivedPosX() + 7;
	iYPos = GetDerivedPosY() + 7;

	ioLSMouse *pMouse = g_App.GetMouse();

	int iListSize = m_vNameList.size();
	for( int i=0 ; i<iListSize ; i++ )
	{
		RECT rcNameRect;
		rcNameRect.left = iXPos;
		rcNameRect.top  = iYPos + i * 21;
		rcNameRect.right  = rcNameRect.left + GetWidth();
		rcNameRect.bottom = rcNameRect.top + 21;

		if( ::PtInRect( &rcNameRect, pMouse->GetMousePos() ) )
		{
			m_iOverListIndex = i;

			if( pMouse->IsLBtnUp() )
			{
				GetParent()->iwm_command( this, m_dwNotifyCommand, i );
			}
			break;
		}
	}
}

void StringSelectList::OnRender()
{
	ioWnd::OnRender();

	if( !m_pDarkLine || !m_pLightLine || !m_pOverLine ) 
		return;

	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	bool bDarkRender = false;
	int iListSize = m_vNameList.size();
	for( int i=0 ; i<iListSize ; i++ )
	{
		bDarkRender = !bDarkRender;
		if( m_iOverListIndex == i )
		{
			m_pOverLine->Render( iXPos, iYPos + ( i * 21 ) );
			g_FontMgr.SetTextColor( 255, 255, 255 );
		}
		else if( bDarkRender )
		{
			m_pDarkLine->Render( iXPos, iYPos + ( i * 21 ) );
			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		}
		else
		{
			m_pLightLine->Render( iXPos, iYPos + ( i * 21 ) );
			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		}

		g_FontMgr.PrintText( iXPos + 12, iYPos + 10 + ( i * 21 ), FONT_SIZE_12, m_vNameList[i].c_str() );
	}
}

//////////////////////////////////////////////////////////////////////////

ResolutionConfirm::ResolutionConfirm()
{
	m_dwWaitDuration = 10000;
	m_dwStartTime = 0;

	m_pNotifier = NULL;
}

ResolutionConfirm::~ResolutionConfirm()
{
}

void ResolutionConfirm::ParseExtraInfo( ioXMLElement &xElement )
{
	m_dwWaitDuration = xElement.GetIntAttribute_e( "WaitDuration" );
	if( m_dwWaitDuration <= 0 )
	{
		m_dwWaitDuration = 10000; 
	}
}

void ResolutionConfirm::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_CONFIRM:
		if( cmd == IOBN_BTNUP )
		{
			NotifyConfirm();
		}
		break;
	case ID_RESTORE:
	case ID_EXIT:
		if( cmd == IOBN_BTNUP )
		{
			NotifyRestore();
		}
		break;
	}
}

bool ResolutionConfirm::iwm_esc()
{
	if( ioWnd::iwm_esc() )
	{
		NotifyRestore();
		return true;
	}

	return false;
}

void ResolutionConfirm::OpenConfirm( ioWnd *pWnd, int iWidth, int iHeight )
{
	if( !pWnd )	return;

	m_dwStartTime = REALGETTIME();
	m_pNotifier = pWnd;

	SetWndPos( (iWidth - GetWidth() ) / 2, (iHeight - GetHeight() ) / 2 );

	ShowWnd();
}

void ResolutionConfirm::NotifyConfirm()
{
	HideWnd();

	if( m_pNotifier )
	{
		m_pNotifier->iwm_command( this, IO_RESOLUTION_CONFIRM, 1 );
	}
}

void ResolutionConfirm::NotifyRestore()
{
	HideWnd();

	if( m_pNotifier )
	{
		m_pNotifier->iwm_command( this, IO_RESOLUTION_CONFIRM, 0 );
	}
}

void ResolutionConfirm::OnProcess( float fTimePerSec )
{
	if( !IsShow() )	return;

	if( m_dwStartTime + m_dwWaitDuration < REALGETTIME() )
	{
		NotifyRestore();
	}
}

void ResolutionConfirm::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );

	int iHalfWidth = GetWidth() * FLOAT05;

	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + iHalfWidth, iYPos + 144, FONT_SIZE_13, STR(1) );

	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( iXPos + iHalfWidth, iYPos + 164, FONT_SIZE_13, STR(2) );

	int iSecond = 1;
	DWORD dwPastTime = REALGETTIME() - m_dwStartTime;
	if( dwPastTime <= m_dwWaitDuration )
	{
		iSecond = ( m_dwWaitDuration - dwPastTime + 999 ) / 1000;
	}

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );

	kPrinter.SetTextColor( TCT_DEFAULT_RED );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(3), iSecond );

	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(4) );

	kPrinter.PrintFullText( iXPos+iHalfWidth, iYPos+184, TAT_CENTER );
}

//////////////////////////////////////////////////////////////////////////

RoomOptionTypeList::RoomOptionTypeList()
{
	m_pDarkLine = NULL;
	m_pLightLine= NULL;
	m_pOverLine = NULL;

	m_pOpenBtn = NULL;
	m_iOverListIndex = -1;
	m_iOptionType = 0;
}

RoomOptionTypeList::~RoomOptionTypeList()
{
	SAFEDELETE( m_pDarkLine );
	SAFEDELETE( m_pLightLine );
	SAFEDELETE( m_pOverLine );
}

void RoomOptionTypeList::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "DarkLine" )
	{
		SAFEDELETE( m_pDarkLine );
		m_pDarkLine = pImage;
	}
	else if( szType == "LightLine" )
	{
		SAFEDELETE( m_pLightLine );
		m_pLightLine = pImage;
	}
	else if( szType == "OverLine" )
	{
		SAFEDELETE( m_pOverLine );
		m_pOverLine = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void RoomOptionTypeList::iwm_show()
{
	SetCheckParentOutSide( true );
}

void RoomOptionTypeList::iwm_hide()
{
	SetCheckParentOutSide( false );
}

void RoomOptionTypeList::OpenList( int iXPos, int iYPos, ioWnd *pOpenBtn, int iType, int iCurValue )
{
	if( IsShow() )
	{
		if( pOpenBtn == m_pOpenBtn )
		{
			HideWnd();
			return;
		}
	}

	if( pOpenBtn == NULL ) return;

	m_pOpenBtn = pOpenBtn;
	m_iOverListIndex = -1;
	m_iOptionType = iType;

	int iCount = g_RoomOptionMgr.GetOptionTypeCnt( m_iOptionType );

	m_ResolutionList.clear();
	m_ResolutionList.reserve( iCount );

	if( PartyOptionWnd::ID_PRESET_BTN == m_iOptionType )
	{
		for(int i = 0;i < iCount;i++)
		{
			if( i == 7 || i == 8 )   // jal   7 이면 팬티몬 모드 8 이면 클베모드 (프리셋 리스트에서는 뺀다)
			{
				continue;
			}

			Resolution kSolution;
			kSolution.iValue = i;
			if( iCurValue == kSolution.iValue )
				kSolution.bCheck = true;
			else
				kSolution.bCheck = false;
			m_ResolutionList.push_back( kSolution );
		}
	}
	else
	{
		for(int i = 0;i < iCount;i++)
		{
			Resolution kSolution;
			kSolution.iValue = i;
			if( iCurValue == kSolution.iValue )
				kSolution.bCheck = true;
			else
				kSolution.bCheck = false;
			m_ResolutionList.push_back( kSolution );
		}
	}

	SetSize( GetWidth(), m_ResolutionList.size() * 21 + 14 );
	SetWndPos( iXPos, iYPos );

	ShowWnd();
}

void RoomOptionTypeList::OnProcess( float fTimePerSec )
{
	if( !IsShow() )	return;

	m_iOverListIndex = -1;

	// 다른 곳에서 클릭이 일어났는지 확인
	ioWnd *pPreClick = g_GUIMgr.GetPreClickWnd();
	if( pPreClick )
	{
		if( pPreClick != m_pOpenBtn && pPreClick != this )
		{
			HideWnd();
			return;
		}
	}

	int iXPos, iYPos;
	iXPos = GetDerivedPosX() + 7;
	iYPos = GetDerivedPosY() + 7;

	ioLSMouse *pMouse = g_App.GetMouse();

	int iListSize = m_ResolutionList.size();
	for( int i=0 ; i<iListSize ; i++ )
	{
		RECT rcNameRect;
		rcNameRect.left = iXPos;
		rcNameRect.top  = iYPos + i * 21;
		rcNameRect.right  = rcNameRect.left + GetWidth();
		rcNameRect.bottom = rcNameRect.top + 21;

		if( ::PtInRect( &rcNameRect, pMouse->GetMousePos() ) )
		{
			m_iOverListIndex = i;

			if( pMouse->IsLBtnUp() )
			{
				const Resolution &rkResolution = m_ResolutionList[i];
				GetParent()->iwm_command( m_pOpenBtn, IO_RESOLUTION_CHANGE, rkResolution.iValue );
				HideWnd();
			}
			break;
		}
	}
}

void RoomOptionTypeList::OnRender()
{
	ioWnd::OnRender();

	if( !m_pDarkLine || !m_pLightLine || !m_pOverLine ) 
		return;

	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();
	bool bDarkRender = false;
	int iListSize = m_ResolutionList.size();
	for( int i=0 ; i<iListSize ; i++ )
	{
		bDarkRender = !bDarkRender;
		if( m_iOverListIndex == i )
		{
			m_pOverLine->Render( iXPos, iYPos + ( i * 21 ) );
			g_FontMgr.SetTextColor( 255, 255, 255 );
		}
		else if( bDarkRender )
		{
			m_pDarkLine->Render( iXPos, iYPos + ( i * 21 ) );
			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		}
		else
		{
			m_pLightLine->Render( iXPos, iYPos + ( i * 21 ) );
			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		}

		const Resolution &rkResolution = m_ResolutionList[i];
		ioHashString szDesc;
		switch( m_iOptionType )
		{
		case PartyOptionWnd::ID_PRESET_BTN:
			szDesc = g_RoomOptionMgr.GetPreSetDesc(rkResolution.iValue);
			break;
		case PartyOptionWnd::ID_CHANGE_CHAR_TYPE:
			szDesc = g_RoomOptionMgr.GetChangeCharDesc(rkResolution.iValue);
			break;
		case PartyOptionWnd::ID_COOLTIME_TYPE:
			szDesc = g_RoomOptionMgr.GetCoolTimeDesc(rkResolution.iValue);
			break;
		case PartyOptionWnd::ID_DROP_DAMAGE_TYPE:
			szDesc = g_RoomOptionMgr.GetDropDamageDesc(rkResolution.iValue);
			break;
		case PartyOptionWnd::ID_GRAVITY_TYPE:
			szDesc = g_RoomOptionMgr.GetGravityDesc(rkResolution.iValue);
			break;
		case PartyOptionWnd::ID_RED_HP_TYPE:
			szDesc = g_RoomOptionMgr.GetRedHPDesc(rkResolution.iValue);
			break;
		case PartyOptionWnd::ID_BLUE_HP_TYPE:
			szDesc = g_RoomOptionMgr.GetBlueHPDesc(rkResolution.iValue);
			break;
		case PartyOptionWnd::ID_TEAM_ATTACK_TYPE:
			szDesc = g_RoomOptionMgr.GetTeamAttackDesc(rkResolution.iValue);
			break;
		case PartyOptionWnd::ID_GETUP_TYPE:
			szDesc = g_RoomOptionMgr.GetGetUpDesc(rkResolution.iValue);
			break;
		case PartyOptionWnd::ID_KO_TYPE:
			szDesc = g_RoomOptionMgr.GetKODesc(rkResolution.iValue);
			break;
		case PartyOptionWnd::ID_KO_EFFECT_TYPE:
			szDesc = g_RoomOptionMgr.GetKOEffectDesc(rkResolution.iValue);
			break;
		case PartyOptionWnd::ID_RED_BLOW_TYPE:
			szDesc = g_RoomOptionMgr.GetRedBlowDesc(rkResolution.iValue);
			break;
		case PartyOptionWnd::ID_BLUE_BLOW_TYPE:
			szDesc = g_RoomOptionMgr.GetBlueBlowDesc(rkResolution.iValue);
			break;
		case PartyOptionWnd::ID_RED_MOVE_SPEED_TYPE:
			szDesc = g_RoomOptionMgr.GetRedMoveSpeedDesc(rkResolution.iValue);
			break;
		case PartyOptionWnd::ID_BLUE_MOVE_SPEED_TYPE:
			szDesc = g_RoomOptionMgr.GetBlueMoveSpeedDesc(rkResolution.iValue);
			break;
		case PartyOptionWnd::ID_RED_EQUIP_TYPE:
			szDesc = g_RoomOptionMgr.GetRedEquipDesc(rkResolution.iValue);
			break;
		case PartyOptionWnd::ID_BLUE_EQUIP_TYPE:
			szDesc = g_RoomOptionMgr.GetBlueEquipDesc(rkResolution.iValue);
			break;
		case PartyOptionWnd::ID_ROUND_TYPE:
			szDesc = g_RoomOptionMgr.GetRoundTypeDesc(rkResolution.iValue);
			break;
		case PartyOptionWnd::ID_ROUND_TIME_TYPE:
			szDesc = g_RoomOptionMgr.GetRoundTimeTypeDesc(rkResolution.iValue);
			break;
		case PartyOptionWnd::ID_GROWTH_USE_TYPE:
			szDesc = g_RoomOptionMgr.GetGrowthUseDesc(rkResolution.iValue);
			break;
		case PartyOptionWnd::ID_EXTRAITEM_USE_TYPE:
			szDesc = g_RoomOptionMgr.GetExtraItemUseDesc(rkResolution.iValue);
			break;
		}

		g_FontMgr.PrintText( iXPos + 12, iYPos + 10 + ( i * 21 ), FONT_SIZE_13, szDesc.c_str() );
	}
}

