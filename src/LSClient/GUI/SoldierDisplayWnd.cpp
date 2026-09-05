

#include "StdAfx.h"
#include "SoldierDisplayWnd.h"

SoldierDisplayChangeWnd::SoldierDisplayChangeWnd()
{
	m_pPlayStage = NULL;
	m_iClassType = 0;
}

SoldierDisplayChangeWnd::~SoldierDisplayChangeWnd()
{
}

void SoldierDisplayChangeWnd::SetPlayStage( ioPlayStage *pPlayStage )
{
	m_pPlayStage = pPlayStage;
}

void SoldierDisplayChangeWnd::iwm_hide()
{
	if( !g_GUIMgr.IsShow( SOLDIER_DISPLAY_SELECT_WND ) )
		g_GUIMgr.ShowWnd( SOLDIER_DISPLAY_SELECT_WND );
}

bool SoldierDisplayChangeWnd::iwm_spacebar()
{
	ioWnd *pOkBtn = FindChildWnd( ID_CHANGE_OK );
	if( !pOkBtn ) return false;
	if( !pOkBtn->IsShow() ) return false;

	if( pOkBtn->HasWndStyle( IWS_INACTIVE ) ) return false;

	iwm_command( pOkBtn, IOBN_BTNUP, 0 );
	return true;
}

void SoldierDisplayChangeWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
	case ID_CHANGE_CANCEL:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;
	case ID_CHANGE_OK:
		if( cmd == IOBN_BTNUP )
		{
			if( m_pPlayStage )
			{
				ioBaseChar *pOwner = m_pPlayStage->GetOwnerChar();
				if( pOwner )
				{
					if( pOwner->GetState() != CS_DELAY && pOwner->GetState() != CS_SPECIAL_MOTION && pOwner->GetState() != CS_ETCITEM_MOTION )
					{
						g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
						return;
					}

					g_MyInfo.SetDisplaySoldier( m_iClassType, (int)pOwner->GetWorldPosition().x, (int)pOwner->GetWorldPosition().z, false );
				}
			}

			HideWnd();
		}
		break;
	}
}

void SoldierDisplayChangeWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	ioMovingWnd::OnProcess( fTimePerSec );

	if( m_pPlayStage )
	{
		ioBaseChar *pOwner = m_pPlayStage->GetOwnerChar();
		if( pOwner && pOwner->GetState() != CS_CHANGE_WAIT )
		{
			if( m_iClassType != g_MyInfo.GetClassType() )
			{
				HideWnd();
			}		
		}
	}	
}

void SoldierDisplayChangeWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, STR(1) );
	kPrinter.PrintFullText( iXPos + 177, iYPos + 41, TAT_CENTER );
	kPrinter.ClearList();
}
//////////////////////////////////////////////////////////////////////////
SoldierDisplaySelectBtn::SoldierDisplaySelectBtn()
{
	m_pCustomNormal = NULL;
	m_pCustomOver	= NULL;
	m_pNone			= NULL;
	m_pIconBack		= NULL;
	m_pSoldierIcon  = NULL;
	m_pSoldierSubIcon = NULL;
	m_pDisplayIcon  = NULL;

	m_iClassType = -1;
	m_bHaveClass = false;

	m_nGradeType = 0;
}

SoldierDisplaySelectBtn::~SoldierDisplaySelectBtn()
{
	SAFEDELETE( m_pCustomNormal );
	SAFEDELETE( m_pCustomOver );
	SAFEDELETE( m_pNone );
	SAFEDELETE( m_pIconBack );
	SAFEDELETE( m_pSoldierIcon );
	SAFEDELETE( m_pSoldierSubIcon );
	SAFEDELETE( m_pDisplayIcon );
}

void SoldierDisplaySelectBtn::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "CustomNormal" )
	{
		SAFEDELETE( m_pCustomNormal );
		m_pCustomNormal = pFrame;
	}
	else if( szType == "CustomOver" )
	{
		SAFEDELETE( m_pCustomOver );
		m_pCustomOver = pFrame;
	}
	else
	{
		ioButton::AddRenderFrame( szType, pFrame );
	}
}

void SoldierDisplaySelectBtn::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "None" )
	{
		SAFEDELETE( m_pNone );
		m_pNone = pImage;
	}
	else if( szType == "IconBack" )
	{
		SAFEDELETE( m_pIconBack );
		m_pIconBack = pImage;
	}
	else if( szType == "DisplayIcon" )
	{
		SAFEDELETE( m_pDisplayIcon );
		m_pDisplayIcon = pImage;
	}
	else 
	{
		ioButton::AddRenderImage( szType, pImage );
	}
}

void SoldierDisplaySelectBtn::SetClassType( int iClassType )
{
	m_iClassType = iClassType;
	m_nGradeType = 0;

	if( g_MyInfo.GetCharIndexByClassType( iClassType ) == 0 )
	{		
		m_bHaveClass = false;
	}
	else
	{
		m_bHaveClass = true;
	}

	SAFEDELETE( m_pSoldierIcon );
	SAFEDELETE( m_pSoldierSubIcon );
	if( m_iClassType != -1 )
	{
		SetActive();
		m_pSoldierIcon = g_MyInfo.GetMySoldierIcon( m_iClassType );
		m_pSoldierSubIcon = g_MyInfo.GetSoldierSubIcon( m_iClassType );
		m_nGradeType = g_UIImageRenderMgr.GetPowerUpGradeType( m_iClassType );
	}
	else
	{
		SetInActive();
	}
}

void SoldierDisplaySelectBtn::OnRender()
{
	ioButton::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();
	if( IsDisabled() )
	{
		if( m_pCustomNormal )
		{
			m_pCustomNormal->SetAlpha( (float)MAX_ALPHA_RATE * 0.60f );
			m_pCustomNormal->Render( iXPos, iYPos );
		}

		if( m_pNone )
		{
			m_pNone->SetScale( 0.60f );
			m_pNone->Render( iXPos + 29, iYPos + 29, UI_RENDER_MULTIPLY, TFO_BILINEAR );
		}
	}
	else
	{
		if( m_bOver || m_bClicked )
		{
			if( m_pCustomOver )
			{
				m_pCustomOver->Render( iXPos, iYPos );
			}
		}
		else if( m_pCustomNormal )
		{
			m_pCustomNormal->SetAlpha( (float)MAX_ALPHA_RATE * 0.60f );
			m_pCustomNormal->Render( iXPos, iYPos );
		}

		if( m_pIconBack )
		{
			m_pIconBack->SetScale( 0.60f );
			m_pIconBack->Render( iXPos + 29, iYPos + 29, UI_RENDER_MULTIPLY, TFO_BILINEAR );

			if ( m_bHaveClass )
				g_UIImageRenderMgr.RenderGradeIconBack( m_nGradeType, iXPos + 29, iYPos + 29, 0.60f );
			else
				g_UIImageRenderMgr.RenderGradeIconBack( m_nGradeType, iXPos + 29, iYPos + 29, 0.60f, UI_RENDER_GRAY );
		}

		if( m_pSoldierIcon )
		{
			m_pSoldierIcon->SetScale( 0.60f );
			if( m_bHaveClass )
				m_pSoldierIcon->Render( iXPos + 29, iYPos + 29, UI_RENDER_NORMAL, TFO_BILINEAR );
			else
				m_pSoldierIcon->Render( iXPos + 29, iYPos + 29, UI_RENDER_GRAY, TFO_BILINEAR );
		}
		if( m_pSoldierSubIcon )
		{
			m_pSoldierSubIcon->SetScale( 0.60f );
			if( m_bHaveClass )
				m_pSoldierSubIcon->Render( iXPos + 29, iYPos + 29, UI_RENDER_NORMAL, TFO_BILINEAR );
			else
				m_pSoldierSubIcon->Render( iXPos + 29, iYPos + 29, UI_RENDER_GRAY, TFO_BILINEAR );
		}
		if( g_MyInfo.IsDisplaySoldier( m_iClassType ) )
		{
			if( m_pDisplayIcon )
				m_pDisplayIcon->Render( iXPos + 6, iYPos + 5 );
		}
	}
}
//////////////////////////////////////////////////////////////////////////
SoldierDisplaySelectWnd::SoldierDisplaySelectWnd()
{
	m_pPlayStage = NULL;
	m_iCurPage = 0;
	m_iMaxPage = 0;
}

SoldierDisplaySelectWnd::~SoldierDisplaySelectWnd()
{
}

void SoldierDisplaySelectWnd::SetPlayStage( ioPlayStage *pPlayStage )
{
	m_pPlayStage = pPlayStage;
}

void SoldierDisplaySelectWnd::SetBtnPos()
{
	int iRealXPos = 11;
	int iRealYPos = 72;
	int iMaxLoop  = MAX_HORZ * MAX_VERT;
	for(int i = 0;i < iMaxLoop;i++)
	{
		SoldierDisplaySelectBtn *pSoldier = dynamic_cast<SoldierDisplaySelectBtn*>(FindChildWnd( ID_SOLDIER_BTN1 + i ));
		if( pSoldier )
		{
			pSoldier->SetWndPos( iRealXPos, iRealYPos );
		}
		if( 0 == ( i + 1 ) % MAX_HORZ )
		{
			iRealXPos  = 11;
			iRealYPos  += 61;
		}
		else
		{
			iRealXPos += 61;
		}
	}
}

void SoldierDisplaySelectWnd::UpdateSetPage( int iCurPage )
{
	m_iCurPage = iCurPage;

	int iListSIze = m_CharList.size();
	int iStartPos = iCurPage * MAX_SOLDIER_BTN;
	for(int i = ID_SOLDIER_BTN1;i < ID_SOLDIER_BTN30 + 1;i++,iStartPos++)
	{
		SoldierDisplaySelectBtn *pBtn = dynamic_cast<SoldierDisplaySelectBtn*>( FindChildWnd( i ) );
		if( pBtn )
		{
			if( iStartPos < iListSIze )
			{
				pBtn->SetClassType( m_CharList[iStartPos] );
			}
			else
			{
				pBtn->SetClassType( -1 );
			}
		}
	}	
}

void SoldierDisplaySelectWnd::iwm_show()
{
	SetBtnPos();
	
	m_CharList.clear();    
	IntVec HaveList;
	int i = 0;
	int iMax = g_ClassPrice.MaxClassPrice();
	for(i = 0; i <  iMax; i++)
	{
		if( !g_ClassPrice.IsActiveClass( i ) )
		{
			// inactive 용병이지만 표시해야 하는 경우가 있다 ( 해외 가챠 캡슐용병등 )
			bool bContinue = true;
			int iClassType = g_ClassPrice.GetClassType( i );
			int iSize = m_vExtraClassVec.size();
			for (int j = 0; j < iSize ; j++)
			{
				int &rClassType = m_vExtraClassVec[j];
				if( iClassType == rClassType )
				{
					bContinue = false;
					break;
				}
			}

			if( bContinue )
				continue;
		}

		int iSetIdx = g_ClassPrice.ArrayToSetItemIdx( i );
		const ioSetItemInfo *pInfo = g_SetItemInfoMgr.GetSetInfoByIdx( iSetIdx );
		if( !pInfo ) continue;

		int iClassType = pInfo->GetSetCode() - SET_ITEM_CODE;
		if( g_MyInfo.GetCharIndexByClassType( iClassType ) != 0 )
		{
			HaveList.insert( HaveList.begin(), iClassType );
			continue;
		}
		m_CharList.push_back( iClassType );
	}

	for(i = 0;i < (int)HaveList.size();i++)
	{
		m_CharList.insert( m_CharList.begin(), HaveList[i] );
	}
	HaveList.clear();

	m_iMaxPage = max( 0, (int)m_CharList.size() - 1 ) / MAX_SOLDIER_BTN;
	UpdateSetPage( 0 );

	g_GUIMgr.HideWnd( SOLDIER_DISPLAY_CHANGE_WND );
}

bool SoldierDisplaySelectWnd::iwm_esc()
{
	ioWnd *pExitBtn = FindChildWnd( ID_EXIT );
	if( !pExitBtn ) return false;
	if( !pExitBtn->IsShow() ) return false;

	iwm_command( pExitBtn, IOBN_BTNUP, 0 );
	return true;
}

void SoldierDisplaySelectWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
		if( cmd == IOBN_BTNUP )
		{
			if( m_pPlayStage )
			{
				ioHeadquartersMode *pHeadquartersMode = ToHeadquartersMode( m_pPlayStage->GetPlayMode() );
				if( pHeadquartersMode &&
					pHeadquartersMode->GetCharState() == ioHeadquartersMode::STATE_UNITE_CHAR &&
					pHeadquartersMode->GetRecordPlayUserCnt() == 1 )
				{
					SP2Packet kPacket( CTPK_HEADQUARTERS_STATE_CHANGE );
					kPacket << (int)ioHeadquartersMode::STATE_DISPLAY_CHAR;
					TCPNetwork::SendToServer( kPacket );
					TCPNetwork::MouseBusy( true );
				}
			}
			HideWnd();
		}
		break;
	case ID_LEFT_PAGE:
		if( cmd == IOBN_BTNUP )
		{
			UpdateSetPage( max( 0, m_iCurPage - 1 ) );
		}
		break;
	case ID_RIGHT_PAGE:
		if( cmd == IOBN_BTNUP )
		{
			UpdateSetPage( min( m_iMaxPage, m_iCurPage + 1 ) );
		}
		break;
	}

	if( COMPARE( dwID, ID_SOLDIER_BTN1, ID_SOLDIER_BTN30 + 1 ) )
	{
		if( cmd == IOBN_BTNUP )
		{
			ioHeadquartersMode *pHeadquartersMode = ToHeadquartersMode( m_pPlayStage->GetPlayMode() );
			if( pHeadquartersMode )
			{
				if( pHeadquartersMode->GetRecordPlayUserCnt() > 1 )
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
					return;
				}
			}

			SoldierDisplaySelectBtn *pSoldier = dynamic_cast<SoldierDisplaySelectBtn*>(pWnd);
			int iClassType = pSoldier->GetClassType();
			if( iClassType == -1 ) return;

			if( g_MyInfo.GetCharIndexByClassType( iClassType ) == 0 )
			{
				// 용병 구매 팝업
				SoldierDisplayAlarmWnd *pAlarmWnd = dynamic_cast<SoldierDisplayAlarmWnd*>( g_GUIMgr.FindWnd(SOLDIER_DISPLAY_ALARM_WND) );
				if( pAlarmWnd )
					pAlarmWnd->SetClassType( iClassType, false );
				HideWnd();
			}
			else
			{
				if( g_MyInfo.IsDisplaySoldier( iClassType ) )
				{
					// 진열 해제
					g_MyInfo.SetDisplaySoldier( iClassType, 0, 0, true );
				}
				else
				{
					if( g_MyInfo.GetDisplaySoldierCount() >= MAX_DISPLAY_CNT )
					{
						g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2), MAX_DISPLAY_CNT );
						return;
					}
					if( m_pPlayStage )
					{
						ioBaseChar *pOwner = m_pPlayStage->GetOwnerChar();
						if( !pOwner )	return;

						ioHeadquartersMode *pHeadquartersMode = ToHeadquartersMode( m_pPlayStage->GetPlayMode() );
						if( pHeadquartersMode->GetCharState() != ioHeadquartersMode::STATE_UNITE_CHAR )
						{
							SP2Packet kPacket( CTPK_HEADQUARTERS_STATE_CHANGE );
							kPacket << (int)ioHeadquartersMode::STATE_UNITE_CHAR;
							TCPNetwork::SendToServer( kPacket );
							TCPNetwork::MouseBusy( true );
						}

						int iCharArray = g_MyInfo.GetClassArray( iClassType );
						if( iCharArray != pOwner->GetSelectCharArray() )
						{
							pOwner->SetChangeWaitState( iCharArray, true );
						}

						SoldierDisplayChangeWnd *pDisplayChangeWnd = dynamic_cast<SoldierDisplayChangeWnd*>(g_GUIMgr.FindWnd( SOLDIER_DISPLAY_CHANGE_WND ));
						if( pDisplayChangeWnd )
						{
							pDisplayChangeWnd->SetClassType( iClassType );
							pDisplayChangeWnd->ShowWnd();
						}
					}
					HideWnd();
				}
			}
		}
	}
}

void SoldierDisplaySelectWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	ioMovingWnd::OnProcess( fTimePerSec );
}

void SoldierDisplaySelectWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(1), MAX_DISPLAY_CNT );
	kPrinter.PrintFullText( iXPos + 193, iYPos + 48, TAT_CENTER );
	kPrinter.ClearList();

	{
		kPrinter.SetTextStyle( TS_NORMAL );
		kPrinter.SetBkColor( 0, 0, 0 );
		kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter.AddTextPiece( FONT_SIZE_12, STR(2), m_iCurPage + 1 );

		kPrinter.SetTextStyle( TS_NORMAL );
		kPrinter.SetBkColor( 0, 0, 0 );
		kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
		kPrinter.AddTextPiece( FONT_SIZE_12, STR(3), m_iMaxPage + 1 );
		kPrinter.PrintFullText( iXPos + 192, iYPos + 382, TAT_CENTER );
	}
}

bool SoldierDisplaySelectWnd::QuestGuide( int iClassType, float &rfXPos, float &rfYPos )
{
	int i = 0;
	for(i = ID_SOLDIER_BTN1;i < ID_SOLDIER_BTN30 + 1;i++)
	{
		SoldierDisplaySelectBtn *pSelectBtn = dynamic_cast<SoldierDisplaySelectBtn*>( FindChildWnd( i ) );
		if( pSelectBtn )
		{
			if( pSelectBtn->GetClassType() == iClassType )
			{
				rfXPos = pSelectBtn->GetDerivedPosX() + ( pSelectBtn->GetWidth() / 2 );
				rfYPos = pSelectBtn->GetDerivedPosY();
				return true;
			}
		}
	}	

	// 리스트에 없으면..
	int iClassPos = 0;
	for(iClassPos = 0;iClassPos < (int)m_CharList.size();iClassPos++)
	{
		if( m_CharList[iClassPos] == iClassType )
			break;
	}

	if( m_iCurPage < (iClassPos / MAX_SOLDIER_BTN) )
	{
		// Right Btn
		ioWnd *pRightBtn = FindChildWnd( ID_RIGHT_PAGE );
		if( pRightBtn )
		{
			rfXPos = pRightBtn->GetDerivedPosX() + ( pRightBtn->GetWidth() / 2 );
			rfYPos = pRightBtn->GetDerivedPosY();
			return true;
		}
	}
	else
	{
		// Left Btn
		ioWnd *pLeftBtn = FindChildWnd( ID_LEFT_PAGE );
		if( pLeftBtn )
		{
			rfXPos = pLeftBtn->GetDerivedPosX() + ( pLeftBtn->GetWidth() / 2 );
			rfYPos = pLeftBtn->GetDerivedPosY();
			return true;
		}
	}

	return false;
}

void SoldierDisplaySelectWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_vExtraClassVec.clear();
	m_vExtraClassVec.reserve(10);
	int iMax = xElement.GetIntAttribute_e( "MaxClass" );
	for (int i = 0; i < iMax ; i++)
	{
		char szName[MAX_PATH]="";
		StringCbPrintf_e( szName, sizeof( szName ), "ClassType%d", i+1 );
		int iClassType = xElement.GetIntAttribute( szName );
		if( iClassType  <= 0 )
			continue;
		m_vExtraClassVec.push_back( iClassType );
	}
}
//////////////////////////////////////////////////////////////////////////
SoldierDisplayAlarmWnd::SoldierDisplayAlarmWnd()
{
	m_pSoldierIcon = NULL;
	m_pSoldierSubIcon = NULL;
	m_iClassType = 0;
	m_bHaveClass = false;
}

SoldierDisplayAlarmWnd::~SoldierDisplayAlarmWnd()
{
	SAFEDELETE( m_pSoldierIcon );
	SAFEDELETE( m_pSoldierSubIcon );
}

void SoldierDisplayAlarmWnd::SetClassType( int iClassType, bool bHaveClass )
{
	m_iClassType = iClassType;
	m_bHaveClass = bHaveClass;

	SAFEDELETE( m_pSoldierIcon );
	SAFEDELETE( m_pSoldierSubIcon );
	m_pSoldierIcon = g_MyInfo.GetMySoldierIcon( iClassType );
	m_pSoldierSubIcon = g_MyInfo.GetSoldierSubIcon( iClassType );

	char szTitle[MAX_PATH] = "";
	if( m_bHaveClass )
		SafeSprintf( szTitle, sizeof( szTitle ), STR(1), g_MyInfo.GetClassName( m_iClassType, g_MyInfo.GetCharPowerUpGradeByClassType( m_iClassType ) ) );
	else
		SafeSprintf( szTitle, sizeof( szTitle ), STR(1), g_MyInfo.GetClassName( m_iClassType ) );

	SetTitleText( szTitle );

	ShowWnd();
}

void SoldierDisplayAlarmWnd::iwm_hide()
{
	if( !g_GUIMgr.IsShow( NEW_SHOP_WND ) )
	{
		g_GUIMgr.ShowWnd( SOLDIER_DISPLAY_SELECT_WND );
	}
}

void SoldierDisplayAlarmWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
	case ID_CLOSE:
		if( cmd == IOBN_BTNUP )
			HideWnd();
		break;
	case ID_SHOP:
		if( cmd == IOBN_BTNUP )
		{	
			g_GUIMgr.ShowWnd( NEW_SHOP_WND );
			HideWnd();
		}
		break;
	}
}

void SoldierDisplayAlarmWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_pSoldierIcon )
		m_pSoldierIcon->Render( iXPos + 151, iYPos + 98 );
	if( m_pSoldierSubIcon )
		m_pSoldierSubIcon->Render( iXPos + 151, iYPos + 98 );

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	if( m_bHaveClass )
		kPrinter.AddTextPiece( FONT_SIZE_13, STR(1), g_MyInfo.GetClassName( m_iClassType, g_MyInfo.GetCharPowerUpGradeByClassType( m_iClassType ) ) );
	else
		kPrinter.AddTextPiece( FONT_SIZE_13, STR(1), g_MyInfo.GetClassName( m_iClassType ) );
	kPrinter.PrintFullText( iXPos + 151, iYPos + 144, TAT_CENTER );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(2) );
	kPrinter.PrintFullText( iXPos + 151, iYPos + 164, TAT_CENTER );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(3) );
	kPrinter.PrintFullText( iXPos + 151, iYPos + 184, TAT_CENTER );
	kPrinter.ClearList();
}