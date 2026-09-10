
#include "StdAfx.h"

#include "DebugInfoWnd.h"
#include "GlobalWnd.h"

DeveloperInfoWnd::DeveloperInfoWnd()
{
	m_iRoomAverageLevel     = 0;
	m_iRoomLowLevelLimit	= 0;
	m_iRoomHighLevelLimit	= 0;
}

DeveloperInfoWnd::~DeveloperInfoWnd()
{
	m_vUserInfo.clear();
}

void DeveloperInfoWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;
	}
}

void DeveloperInfoWnd::OnDeveloperInfo( SP2Packet &rkPacket )
{
	// 훈련 제거 : Solo 필요없어진 값 2009. 1. 22. KDH
	m_vUserInfo.clear();
	
	int iSize;
	rkPacket >> m_iRoomAverageLevel >> iSize;
	for(int i = 0;i < iSize;i++)
	{
		UserInfo kInfo;
		rkPacket >> kInfo.m_szName >> kInfo.m_iGradeLevel >> kInfo.m_iKillDeathLevel
			     >> kInfo.m_iPartyKO >> kInfo.m_iPartyKOed;
		m_vUserInfo.push_back( kInfo );
	}
	rkPacket >> m_iRoomLowLevelLimit >> m_iRoomHighLevelLimit;
	SetSize( GetWidth(), 105 + ( iSize * 20 ) );
	ShowWnd();
}

void DeveloperInfoWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	
	g_FontMgr.SetTextColor( 255, 0, 0 );
	g_FontMgr.PrintText( iXPos + 10, iYPos + 35, FONT_SIZE_13, STR(1), m_iRoomAverageLevel, m_iRoomLowLevelLimit, m_iRoomHighLevelLimit );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( iXPos + 10, iYPos + 55, FONT_SIZE_13, "Nick" );
	g_FontMgr.PrintText( iXPos + 90, iYPos + 55, FONT_SIZE_13, "GLevel");
	g_FontMgr.PrintText( iXPos + 150, iYPos + 55, FONT_SIZE_13, "K/D Level");
	g_FontMgr.PrintText( iXPos + 230, iYPos + 55, FONT_SIZE_13, "Kill");
	g_FontMgr.PrintText( iXPos + 300, iYPos + 55, FONT_SIZE_13, "Death");

	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	int i = 0;
	int iUserSize = m_vUserInfo.size();
	for(i = 0;i < iUserSize;i++)
	{
		UserInfo &kInfo = m_vUserInfo[i];
		g_FontMgr.PrintTextWidthCut( iXPos + 10, iYPos + 75 + (i*20), FONT_SIZE_13, 80.0f, kInfo.m_szName.c_str() );
		g_FontMgr.PrintText( iXPos + 100, iYPos + 75 + (i*20), FONT_SIZE_13, STR(2), kInfo.m_iGradeLevel );
		g_FontMgr.PrintText( iXPos + 160, iYPos + 75 + (i*20), FONT_SIZE_13, STR(3), kInfo.m_iKillDeathLevel );		
		g_FontMgr.PrintText( iXPos + 230, iYPos + 75 + (i*20), FONT_SIZE_13, STR(4), kInfo.m_iSoloKO, kInfo.m_iPartyKO );
		g_FontMgr.PrintText( iXPos + 310, iYPos + 75 + (i*20), FONT_SIZE_13, STR(5), kInfo.m_iSoloKOed, kInfo.m_iPartyKOed );
	}
}
//////////////////////////////////////////////////////////////////////////
DeveloperP2PWnd::DeveloperP2PWnd()
{

}

DeveloperP2PWnd::~DeveloperP2PWnd()
{
	
}

void DeveloperP2PWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;
	}
}

void DeveloperP2PWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	g_P2PChecker.RenderLog( iXPos + 600, iYPos );
	
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );

	g_FontMgr.PrintText( iXPos + 10, iYPos + 55, FONT_SIZE_13, "SendList" );		
	g_FontMgr.PrintText( iXPos + 200, iYPos + 55, FONT_SIZE_13, "RecvList" );		

	int i = 0;
	for(i = 0;i < ioP2PChecker::MAX_SEND_HIGH_SIZE;i++)
	{
		g_FontMgr.PrintText( iXPos + 10, iYPos + 75 + (i*20), FONT_SIZE_13, g_P2PChecker.SendPacketLog(i).c_str() );		
		g_FontMgr.PrintText( iXPos + 200, iYPos + 75 + (i*20), FONT_SIZE_13, g_P2PChecker.RecvPacketLog(i).c_str() );		
	}	
}

DeveloperFrameWnd::DeveloperFrameWnd()
{
}

DeveloperFrameWnd::~DeveloperFrameWnd()
{
}

void DeveloperFrameWnd::iwm_show()
{
//	g_TimeReport.SetReportOnOff( true, 100 );
}

void DeveloperFrameWnd::iwm_hide()
{
//	g_TimeReport.SetReportOnOff( false, 100 );
}

void DeveloperFrameWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;
	}
}

void DeveloperFrameWnd::OnRender()
{
	ioWnd::OnRender();
/*
	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );

	char szBuf[MAX_PATH*2];
	int iPointCount = g_TimeReport.GetPointCount();
	for( int i=0 ; i<iPointCount ; i++ )
	{
		if( g_TimeReport.GetPointInfo( i, szBuf ) )
		{
			g_FontMgr.PrintText( iXPos + 10, iYPos + 50 + i*25, 0.6f, szBuf );
		}
	}
*/
}
//////////////////////////////////////////////////////////////////////////
FontTextureTestWnd::FontTextureTestWnd()
{
	m_fFontScale = FONT_SIZE_10;
}

FontTextureTestWnd::~FontTextureTestWnd()
{

}

void FontTextureTestWnd::SetFontSize( int iSize )
{
	if( iSize == 0 )
		m_fFontScale = FLOAT1;
	else
		m_fFontScale = (float)iSize / (float)g_FontMgr.GetFontSize();
}

void FontTextureTestWnd::FontTextureRender()
{
	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();
	g_FontMgr.RenderTextTexture( iXPos, iYPos, m_fFontScale );
}

//////////////////////////////////////////////////////////////////////////

CharGameInfoListWnd::CharGameInfoListWnd() : m_pPreEdit( NULL ), m_dwCurrTab( 0 )
{	
}

CharGameInfoListWnd::~CharGameInfoListWnd()
{
}

void CharGameInfoListWnd::iwm_show()
{
	m_pPreEdit = ioEdit::m_pKeyFocusEdit;
	UpdateTab( ID_ITEM_TAB );
}

void CharGameInfoListWnd::UpdateTab( DWORD dwID )
{
	m_dwCurrTab = dwID;
	CheckRadioButton( ID_ITEM_TAB, ID_BUFF_TAB, m_dwCurrTab );

	DisableEditFocus();
	HideChildWnd( ID_LABEL_WEAPON );
	HideChildWnd( ID_LABEL_ARMOR );
	HideChildWnd( ID_LABEL_HELMET );
	HideChildWnd( ID_LABEL_CLOAK );
	HideChildWnd( ID_LABEL_OBJECT );
	HideChildWnd( ID_LABEL_WEAR );
	HideChildWnd( ID_WEAPON_EDIT );
	HideChildWnd( ID_ARMOR_EDIT );
	HideChildWnd( ID_HELMET_EDIT );
	HideChildWnd( ID_CLOAK_EDIT );
	HideChildWnd( ID_OBJECT_EDIT );
	HideChildWnd( ID_WEAR_EDIT );
	HideChildWnd( ID_WEAPON_APPLY );
	HideChildWnd( ID_ARMOR_APPLY );
	HideChildWnd( ID_HELMET_APPLY );
	HideChildWnd( ID_CLOAK_APPLY );
	HideChildWnd( ID_OBJECT_APPLY );
	HideChildWnd( ID_WEAR_APPLY );

	SetFrameElementAlpha( 0.0f );

	switch( m_dwCurrTab )
	{
	case ID_ITEM_TAB:
		{
			ShowChildWnd( ID_LABEL_WEAPON );
			ShowChildWnd( ID_LABEL_ARMOR );
			ShowChildWnd( ID_LABEL_HELMET );
			ShowChildWnd( ID_LABEL_CLOAK );
			ShowChildWnd( ID_LABEL_OBJECT );
			ShowChildWnd( ID_LABEL_WEAR );
			ShowChildWnd( ID_WEAPON_EDIT );
			ShowChildWnd( ID_ARMOR_EDIT );
			ShowChildWnd( ID_HELMET_EDIT );
			ShowChildWnd( ID_CLOAK_EDIT );
			ShowChildWnd( ID_OBJECT_EDIT );
			ShowChildWnd( ID_WEAR_EDIT );
			ShowChildWnd( ID_WEAPON_APPLY );
			ShowChildWnd( ID_ARMOR_APPLY );
			ShowChildWnd( ID_HELMET_APPLY );
			ShowChildWnd( ID_CLOAK_APPLY );
			ShowChildWnd( ID_OBJECT_APPLY );
			ShowChildWnd( ID_WEAR_APPLY );			
			SetFrameElementAlpha( FLOAT1 );
		}
		break;
	case ID_CHAR_INFO_TAB:
		{
		}
		break;
	case ID_BUFF_TAB:
		{

		}
		break;
	}
}

void CharGameInfoListWnd::SetFrameElementAlpha( float fRate )
{
	for( int i = 0; i < (int)m_ElementList.size(); ++i )
	{
		ioUIRenderElement* pElment = m_ElementList[i];
		if( pElment && pElment->GetType() == ioUIRenderElement::UIE_FRAME )
			pElment->SetAlpha( MAX_ALPHA_RATE * fRate );
	}
}

void CharGameInfoListWnd::iwm_hide()
{
	SingleSettingWnd* pWnd = dynamic_cast<SingleSettingWnd*>( g_GUIMgr.FindWnd( SINGLE_SETTING_WND ) );
	if( pWnd )
	{		
		pWnd->ChangeCharacterButtonTitle( "캐릭터 선택" );
	}
	DisableEditFocus();
}

void CharGameInfoListWnd::EnableEditFocus( DWORD dwEidtID )
{
	ioEdit *pEdit = dynamic_cast<ioEdit*>( FindChildWnd( dwEidtID ) );
	if( pEdit && pEdit->IsShow() )
	{
		if( m_pPreEdit != pEdit )
			m_pPreEdit = ioEdit::m_pKeyFocusEdit;
				
		pEdit->SetKeyFocus();
	}
}

void CharGameInfoListWnd::DisableEditFocus()
{
	ioEdit* pEdit = dynamic_cast<ioEdit*>( m_pPreEdit );
	if( pEdit )	
	{
		ioWnd *pParent = pEdit->GetParent();
		if( pParent )
		{
			if( pParent->IsShow() )
			{
				pEdit->SetKeyFocus();
			}
		}
	}
	m_pPreEdit = NULL;
}

void CharGameInfoListWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	ioPlayStage* pStage = g_GUIMgr.GetPlayStage();
	if( pStage )
	{
		ioBaseChar* pChar = pStage->GetBaseChar( m_CharStateInfo.szName );
		if( pChar )
		{
			m_CharStateInfo.fGravity	= pChar->GetGravityAmt();	
			m_CharStateInfo.fForce		= pChar-> GetForceAmt();
			m_CharStateInfo.fMoveSpeed	= pChar->GetCurSpeed();
			m_CharStateInfo.fJumpPower	= pChar->GetCurJumpPowerAmt();

			m_CharStateInfo.BuffList.clear();
			pChar->GetAllBuff( m_CharStateInfo.BuffList );
		}
	}
}

void CharGameInfoListWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;
	case ID_WEAPON_APPLY:
	case ID_ARMOR_APPLY:
	case ID_HELMET_APPLY:
	case ID_CLOAK_APPLY:
	case ID_OBJECT_APPLY:
	case ID_WEAR_APPLY:		
		if( cmd == IOBN_BTNUP )
		{
			DWORD dwID = ID_WEAPON_EDIT + pWnd->GetID() - ID_WEAPON_APPLY;
			EquipItem( dwID );
		}
		break;
	case ID_ITEM_TAB:
	case ID_CHAR_INFO_TAB:
	case ID_BUFF_TAB:
		if( cmd == IOBN_BTNUP )
		{
			UpdateTab( dwID );
		}
		break;
	case ID_WEAPON_EDIT:
	case ID_ARMOR_EDIT:
	case ID_HELMET_EDIT:
	case ID_CLOAK_EDIT:
	case ID_OBJECT_EDIT:
	case ID_WEAR_EDIT:
		if( cmd == IOED_INPUT )
		{
			if( param == 1 )
			{
				EquipItem( pWnd->GetID() );
			}
			else
			{
				DWORD dwNextID = pWnd->GetID() + 1;
				if( !COMPARE( dwNextID, ID_WEAPON_EDIT, ID_WEAR_EDIT + 1 ) )
					dwNextID = ID_WEAPON_EDIT;

				EnableEditFocus( dwNextID );
			}
		}
		break;
	}
}

void CharGameInfoListWnd::EquipItem( DWORD dwID )
{
	ioPlayStage* pStage = g_GUIMgr.GetPlayStage();
	if( pStage == NULL )
		return;

	char szBuf[MAX_PATH];
	ioEdit* pEdit = dynamic_cast<ioEdit*>( FindChildWnd( dwID ) );
	if( pEdit )
	{
		int eSlot = pEdit->GetID() - ID_WEAPON_EDIT;
		if( strlen( pEdit->GetText() ) <= 0 )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "아이템 코드를 입력하세요" );
			wsprintf_e( szBuf, "%d", m_CharStateInfo.iSlot[eSlot] );
			pEdit->SetText( szBuf );
		}

		int nCode = atoi( pEdit->GetText() );
		if( g_ItemMaker.IsItem( nCode ) )
		{
			ioBaseChar* pChar = pStage->GetBaseChar( m_CharStateInfo.szName );					
			ioItem *pItem = pStage->CreateItem( nCode );
			if( pItem )
			{
				if( pItem->GetType() == eSlot + 1 )
				{
					pItem->ReLoadProperty();
					pItem->SetOwnerName( pChar->GetCharName() );
					ioItem* pDeleteItem = pChar->EquipItem( pItem );
					if( pDeleteItem )
						delete pDeleteItem;
					pItem->Initialize( pChar, pStage );

					pEdit->KillKeyFocus();
					DisableEditFocus();
				}
				else
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, "아이템 슬롯에 맞지 않는#아이템 코드 입니다." );
				}
			}
			else
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "아이템 생성에 실패 하였습니다." );
				wsprintf_e( szBuf, "%d", m_CharStateInfo.iSlot[pEdit->GetID() - ID_WEAPON_EDIT] );
				pEdit->SetText( szBuf );
			}
		}
		else
		{
			wsprintf_e( szBuf, "%d", m_CharStateInfo.iSlot[pEdit->GetID() - ID_WEAPON_EDIT] );
			pEdit->SetText( szBuf );
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "잘못된 아이템 코드 입니다." );
		}
	}
}

void CharGameInfoListWnd::SetStateInfoChar( ioBaseChar* pChar )
{
	if( !pChar ) 
		return;

	ioEquipSlot* pSlot = pChar->GetEquipSlot();
	if( !pSlot )
		return;

	DisableEditFocus();
	SetTitleText( pChar->GetCharName().c_str() );

	m_CharStateInfo.Clear();
	const CHARACTER& rkChar = pChar->GetCharacterInfo();

	m_CharStateInfo.szName = pChar->GetCharName();

	char szBuf[MAX_PATH] ="";
	for( int i = 0; i < MAX_EQUIP_SLOT; ++i )
	{
		int iCurrItemCode = 0;
		ioItem* pItem = pSlot->GetEquipedItem( i );
		if( pItem )
		{
			iCurrItemCode = m_CharStateInfo.iSlot[i] = (int)pItem->GetItemCode();
		}

		ioEdit* pEdit = dynamic_cast<ioEdit*>( FindChildWnd( i + ID_WEAPON_EDIT ) );
		if( pEdit )
		{
			wsprintf_e( szBuf, "%d", iCurrItemCode );
			pEdit->SetText( szBuf );
		}
	}	
}

void CharGameInfoListWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	
	switch( m_dwCurrTab )
	{
	case ID_CHAR_INFO_TAB:
		{
			g_FontMgr.PrintText( iXPos + 10, iYPos + 80, FONT_SIZE_11, " Gravity Amt : %f", m_CharStateInfo.fGravity );
			g_FontMgr.PrintText( iXPos + 10, iYPos + 95, FONT_SIZE_11, " Force Amt : %f", m_CharStateInfo.fForce );
			g_FontMgr.PrintText( iXPos + 10, iYPos + 110, FONT_SIZE_11, " Move Speed Amt : %f", m_CharStateInfo.fMoveSpeed );
			g_FontMgr.PrintText( iXPos + 10, iYPos + 125, FONT_SIZE_11, " Jump Power Amt : %f", m_CharStateInfo.fJumpPower );
		}
		break;
	case ID_BUFF_TAB:
		{
			ioHashStringVec::iterator iter = m_CharStateInfo.BuffList.begin();
			for( int i = 0; iter != m_CharStateInfo.BuffList.end(); ++iter, ++i )
			{
				const ioHashString& szName = *iter;
				if( !szName.IsEmpty() )
				{
					g_FontMgr.PrintText( iXPos + 10, iYPos + 80 + i * 15, FONT_SIZE_11, " 버프%d : %s", i, szName.c_str() );
				}
			}
		}
		break;
	}
}

//////////////////////////////////////////////////////////////////////////

bool SingleSettingWnd::m_bSingle	= false;
bool SingleSettingWnd::m_bAutoShow	= false;
bool SingleSettingWnd::m_bPause		= false;

SingleSettingWnd::SingleSettingWnd() : m_fCurrAlpha( ALPHA_MAX )
{
}

SingleSettingWnd::~SingleSettingWnd()
{

}

void SingleSettingWnd::SetSingle( bool bSingle )
{
	m_bSingle = bSingle;
}

void SingleSettingWnd::SetAutoShow( bool bAutoShow )
{
	m_bAutoShow = bAutoShow;
}

void SingleSettingWnd::SetPause( bool bPause )
{
	m_bPause = bPause;
}

bool SingleSettingWnd::IsPause()
{
	return m_bPause;
}

void SingleSettingWnd::iwm_create()
{
}

void SingleSettingWnd::iwm_show()
{
	CreateModeList();

	UpdateMode( ioPlayMode::GetModeType() );
	UpdateModeSub( Setting::GetModeSub() );
	UpdateMapIdx( Setting::GetModeMap() );
	UpdateCheckBox();

	Setting::SetSWndBoundary( false );
	SetPause( false );
	UpdatePauseButton();
	ChangeCharacterButtonTitle( "캐릭터 선택" );

	ParseScroll();

	if( m_bSingle )
	{
		SetChildWndStyleRemove( ID_MODE, IWS_EXACTIVE );
		SetChildWndStyleRemove( ID_MODE_SUB, IWS_EXACTIVE );
		SetChildWndStyleRemove( ID_MAP, IWS_EXACTIVE );
		SetChildWndStyleRemove( ID_MODE_APPLY, IWS_EXACTIVE );
		SetChildWndStyleRemove( ID_PAUSE, IWS_EXACTIVE );
		SetChildWndStyleRemove( ID_CHARACTER, IWS_EXACTIVE );
		SetChildWndStyleRemove( ID_OWER, IWS_EXACTIVE );
	}
	else
	{
		SetChildWndStyleAdd( ID_MODE, IWS_EXACTIVE );
		SetChildWndStyleAdd( ID_MODE_SUB, IWS_EXACTIVE );
		SetChildWndStyleAdd( ID_MAP, IWS_EXACTIVE );
		SetChildWndStyleAdd( ID_MODE_APPLY, IWS_EXACTIVE );
		SetChildWndStyleAdd( ID_PAUSE, IWS_EXACTIVE );
		SetChildWndStyleAdd( ID_CHARACTER, IWS_EXACTIVE );
		SetChildWndStyleAdd( ID_OWER, IWS_EXACTIVE );
	}
}

void SingleSettingWnd::iwm_hide()
{
	Setting::SetSWndBoundary( false );
}

void SingleSettingWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() && m_bSingle && m_bAutoShow )
	{
		if( ioPlayMode::GetModeType() != MT_NONE )
			ShowWnd();
	}

	UpdateMemButtonDsiable();
	UpdteOwner();
}

void SingleSettingWnd::UpdateCheckBox()
{
	ioCheckButton* pBoundButton = dynamic_cast<ioCheckButton*>( FindChildWnd( ID_BOUND ) );
	if( pBoundButton )
		pBoundButton->SetCheck( Setting::ShowBoundBox() );

	ioCheckButton* pSphereButton = dynamic_cast<ioCheckButton*>( FindChildWnd( ID_SPHERE ) );
	if( pSphereButton )
		pSphereButton->SetCheck( Setting::ShowBoundSphere() );
	
	ioCheckButton* pExtraButton = dynamic_cast<ioCheckButton*>( FindChildWnd( ID_EXTRA ) );
	if( pExtraButton )
		pExtraButton->SetCheck( Setting::ShowExtraInfo() );

	ioCheckButton* pMemButton = dynamic_cast<ioCheckButton*>( FindChildWnd( ID_MEMORY ) );
	if( pMemButton )
		pMemButton->SetCheck( Setting::ShowMemoryUsage() );
}

void SingleSettingWnd::UpdatePauseButton()
{
	ioButton* pButton = dynamic_cast<ioButton*>( FindChildWnd( ID_PAUSE ) );
	if( pButton )
	{
		if( IsPause() )
			pButton->SetTitleText( "Play" );
		else
			pButton->SetTitleText( "Pause" );
	}
}

void SingleSettingWnd::UpdateMemButtonDsiable()
{
	SetChildWndStyleRemove( ID_MEMORY, IWS_EXACTIVE );
	ioCheckButton* pExtraButton = dynamic_cast<ioCheckButton*>( FindChildWnd( ID_EXTRA ) );
	if( pExtraButton  && !pExtraButton->IsChecked() )
	{
		ioCheckButton* pMemButton = dynamic_cast<ioCheckButton*>( FindChildWnd( ID_MEMORY ) );
		if( pMemButton )
		{
			SetChildWndStyleAdd( ID_MEMORY, IWS_EXACTIVE );
			pMemButton->SetCheck( false );
			Setting::SetShowMemoryUsage( false );
		}
	}
}

bool SingleSettingWnd::GetModeNameByType( IN int eType, OUT ioHashString &szModeName )
{
	switch( eType )
	{
	case MT_UNDERWEAR:
		szModeName = "팬티몬";
		return true;
	case MT_CBT:
		szModeName = "클베모드";
		return true;
	case MT_CATCH:
		szModeName = "포로탈출";
		return true;
	case MT_SYMBOL:
		szModeName = "파워스톤";
		return true;
	case MT_SURVIVAL:
		szModeName = "데스매치";
		return true;
	case MT_TEAM_SURVIVAL:
		szModeName = "팀데스매치";
		return true;
	case MT_BOSS:
		szModeName = "보스레이드";
		return true;
	case MT_MONSTER_SURVIVAL:
		szModeName = "해골영웅";
		return true;
	case MT_FOOTBALL:
		szModeName = "축구경기";
		return true;
	case MT_ARENA:
		szModeName = "아레나";
		return true;
	case MT_BATTLE:
		szModeName = "배틀모드";
		return true;
	case MT_FARMING:
		szModeName = "파밍모드";
		return true;
	}

	szModeName ="선택없음";
	return false;
}

//-- 모드선택 ------------------------------------------------------------------------------------------------------------

void SingleSettingWnd::ChangeModeButtonTitle( const ioHashString& szName )
{
	ioButton* pButton = dynamic_cast<ioButton*>( FindChildWnd( ID_MODE ) );
	if( pButton )
		pButton->SetTitleText( szName.c_str() );
}

void SingleSettingWnd::CreateModeList()
{
	StringScrollListWnd *pList = dynamic_cast<StringScrollListWnd*>( FindChildWnd( ID_MODE_LIST ) );
	if( pList )
	{
		for( int eMode = MT_NONE; eMode < MAX_MODE_TYPE; ++eMode )
		{
			ioHashString ModeName;
			if( GetModeNameByType( eMode, ModeName ) )
			{
				pList->AddList( ID_MODE_LIST, eMode, ModeName, NULL );
			}
		}
	}
}

void SingleSettingWnd::OpenModekList( ioWnd *pOpenWnd )
{	
	StringScrollListWnd *pList = dynamic_cast<StringScrollListWnd *>( FindChildWnd( ID_MODE_LIST ) );
	if( pList )
	{
		pList->SetList( pOpenWnd, pOpenWnd->GetXPos(), pOpenWnd->GetYPos() + 24 );
		CreateModeList();
		pList->ShowListWnd();
	}
}

void SingleSettingWnd::UpdateMode( int eModeType )
{
	ioHashString ModeName;
	if( GetModeNameByType( eModeType, ModeName ) )
		m_CurrModeType = eModeType;
	else
		m_CurrModeType = MT_NONE;

	ChangeModeButtonTitle( ModeName );
}

//-- 모드 서브인덱스 ------------------------------------------------------------------------------------------------------------

void SingleSettingWnd::ChangeModeSubButtonTitle( const ioHashString& szName )
{
	ioButton* pButton = dynamic_cast<ioButton*>( FindChildWnd( ID_MODE_SUB ) );
	if( pButton )
		pButton->SetTitleText( szName.c_str() );
}

void SingleSettingWnd::CreateModeSubList()
{
	StringScrollListWnd *pList = dynamic_cast<StringScrollListWnd*>( FindChildWnd( ID_MODE_SUB_LIST ) );
	if( pList )
	{		
		int iModeIdx = g_ModeSelectInfo.GetBattleModeIndex( m_CurrModeType );
		g_ModeSelectInfo.GetBattleModeStringList( pList, iModeIdx, ID_MODE_SUB_LIST );
	}
}

void SingleSettingWnd::OpenModeSubList( ioWnd *pOpenWnd )
{	
	StringScrollListWnd *pList = dynamic_cast<StringScrollListWnd *>( FindChildWnd( ID_MODE_SUB_LIST ) );
	if( pList )
	{
		pList->SetList( pOpenWnd, pOpenWnd->GetXPos(), pOpenWnd->GetYPos() + 24 );
		CreateModeSubList();
		pList->ShowListWnd();
	}
}

void SingleSettingWnd::UpdateModeSub( int Param )
{
	char szBuf[MAX_PATH];
	ioHashString szTtitle;
	if( Param == 0  )
	{
		sprintf_s( szBuf, "선택없음" );
		szTtitle = szBuf;
	}
	else
	{
		int nModeIdx = g_ModeSelectInfo.GetBattleModeIndex( m_CurrModeType );
		szTtitle = g_ModeSelectInfo.GetBattleMapTitle( nModeIdx, Param );

	}
	ChangeModeSubButtonTitle( szTtitle );
	m_CurrModeSub = Param;
}

//-- 맵 인덱스 ------------------------------------------------------------------------------------------------------------

void SingleSettingWnd::ChangeMapIdxButtonTitle( const ioHashString& szName )
{
	ioButton* pButton = dynamic_cast<ioButton*>( FindChildWnd( ID_MAP ) );
	if( pButton )
		pButton->SetTitleText( szName.c_str() );
}

void SingleSettingWnd::CreateMapIdxList()
{
	StringScrollListWnd *pList = dynamic_cast<StringScrollListWnd*>( FindChildWnd( ID_MAP_LIST ) );
	if( pList )
	{
		for( int i = 1; i <= SUB_MAX; ++i )
		{
			char szBuf[MAX_PATH];
			sprintf_s( szBuf, "맵인덱스%d", i );
			pList->AddList( ID_MAP_LIST, i, szBuf, NULL );
		}
	}
}

void SingleSettingWnd::OpenMapIdxList( ioWnd *pOpenWnd )
{	
	StringScrollListWnd *pList = dynamic_cast<StringScrollListWnd *>( FindChildWnd( ID_MAP_LIST ) );
	if( pList )
	{
		pList->SetList( pOpenWnd, pOpenWnd->GetXPos(), pOpenWnd->GetYPos() + 24 );
		CreateMapIdxList();
		pList->ShowListWnd();
	}
}

void SingleSettingWnd::UpdateMapIdx( int Param )
{
	char szBuf[MAX_PATH];
	if( Param == 0  )
		sprintf_s( szBuf, "선택없음" );
	else
		sprintf_s( szBuf, "서브인덱스%d", Param );

	ChangeMapIdxButtonTitle( szBuf );
	m_CurrModeMapIndex = Param;
}

//-- 캐릭터 정보 ------------------------------------------------------------------------------------------------------------

void SingleSettingWnd::ChangeCharacterButtonTitle( const ioHashString& szName )
{
	ioButton* pButton = dynamic_cast<ioButton*>( FindChildWnd( ID_CHARACTER ) );
	if( pButton )
		pButton->SetTitleText( szName.c_str() );
}

void SingleSettingWnd::CreateCharacterList()
{
	StringScrollListWnd *pList = dynamic_cast<StringScrollListWnd*>( FindChildWnd( ID_CHARACTER_LIST ) );
	if( pList )
	{
		ioPlayStage* pStage = g_GUIMgr.GetPlayStage();
		if( pStage )
		{
			const BaseCharList& rkList = pStage->GetBaseCharList();
			if( rkList.empty() )
				return;

			for( int i = 0 ; i < (int)rkList.size(); ++i )
			{
				char szBuf[MAX_PATH];
				
				const ioBaseChar* pChar = rkList[i];
				DWORD dwCharIdx = 0;
				if( pChar )
				{					
					sprintf_s( szBuf, "%s", pChar->GetCharName() );
					dwCharIdx = pChar->GetIndex();
				}
				else
				{
					sprintf_s( szBuf, "알수없음" );
				}

				pList->AddList( ID_CHARACTER_LIST, dwCharIdx, szBuf, NULL );
			}
		}
	}
}

void SingleSettingWnd::OpenCharacterList( ioWnd *pOpenWnd )
{	
	StringScrollListWnd *pList = dynamic_cast<StringScrollListWnd *>( FindChildWnd( ID_CHARACTER_LIST ) );
	if( pList )
	{
		pList->SetList( pOpenWnd, pOpenWnd->GetXPos(), pOpenWnd->GetYPos() + 24 );
		CreateCharacterList();
		pList->ShowListWnd();
	}
}

//-- 주 캐릭터 선택 ------------------------------------------------------------------------------------------------------------

void SingleSettingWnd::ChangeOwnerButtonTitle( const ioHashString& szName )
{
	m_CurrOwner = szName;
	ioButton* pButton = dynamic_cast<ioButton*>( FindChildWnd( ID_OWER ) );
	if( pButton )
		pButton->SetTitleText( szName.c_str() );
}

void SingleSettingWnd::CreateOwnerList()
{
	StringScrollListWnd *pList = dynamic_cast<StringScrollListWnd*>( FindChildWnd( ID_OWER_LIST ) );
	if( pList )
	{
		ioPlayStage* pStage = g_GUIMgr.GetPlayStage();
		if( pStage )
		{
			const BaseCharList& rkList = pStage->GetBaseCharList();
			if( rkList.empty() )
				return;

			for( int i = 0 ; i < (int)rkList.size(); ++i )
			{
				char szBuf[MAX_PATH];

				const ioBaseChar* pChar = rkList[i];
				DWORD dwCharIdx = 0;
				if( pChar )
				{					
					sprintf_s( szBuf, "%s", pChar->GetCharName() );
					dwCharIdx = pChar->GetIndex();
				}
				else
				{
					sprintf_s( szBuf, "알수없음" );
				}

				pList->AddList( ID_OWER_LIST, dwCharIdx, szBuf, NULL );
			}
		}
	}
}

void SingleSettingWnd::OpenOwnerList( ioWnd *pOpenWnd )
{	
	StringScrollListWnd *pList = dynamic_cast<StringScrollListWnd *>( FindChildWnd( ID_OWER_LIST ) );
	if( pList )
	{
		pList->SetList( pOpenWnd, pOpenWnd->GetXPos(), pOpenWnd->GetYPos() + 24 );
		CreateOwnerList();
		pList->ShowListWnd();
	}
}

void SingleSettingWnd::UpdteOwner()
{
	ioPlayStage* pStage = g_GUIMgr.GetPlayStage();
	if( pStage && pStage->GetOwnerChar() )
	{
		if( m_CurrOwner != pStage->GetOwnerChar()->GetCharName() )
			ChangeOwnerButtonTitle( pStage->GetOwnerChar()->GetCharName() );
	}
}

//-- 윈도우 알파 조절 ------------------------------------------------------------------------------------------------------------

void SingleSettingWnd::ParseScroll()
{
	ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_SCROLL );
	if( pScroll )
	{
		pScroll->SetScrollRange( 0, ALPHA_MAX );
		pScroll->SetScrollPos( (int)m_fCurrAlpha );
	}
}


void SingleSettingWnd::iwm_wheel( int zDelta )
{
	if( !IsShow() )
	{
		ioWnd::iwm_wheel( zDelta );
	}
	else
	{
		if( g_App.IsMouseBusy() )
			return;

		ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_SCROLL );
		if( pScroll == NULL )
			return;

		if( !pScroll->IsShow() )
			return;

		if( zDelta == WHEEL_DELTA )
			pScroll->SetScrollPos( m_fCurrAlpha - 1 );
		else if( zDelta == -WHEEL_DELTA )
			pScroll->SetScrollPos( m_fCurrAlpha + 1 );
	}

	SetAlpha( m_fCurrAlpha );
}

void SingleSettingWnd::iwm_vscroll( DWORD dwID, int curPos )
{
	ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_SCROLL );
	if( pScroll == NULL )
		return;

	if( !pScroll->IsShow() )
		return;

	m_fCurrAlpha = curPos;
	SetAlpha( m_fCurrAlpha );
}

void SingleSettingWnd::SetAlpha(  float fAlpha )
{
	float fRate = max( ALPHA_MIN, fAlpha );
	DWORD dwRate = MAX_ALPHA_RATE * ( fRate / (float)ALPHA_MAX );
	SetWindowAlpha( dwRate );	
}

//-------------------------------------------------------------------------------------------------------------------------------

void SingleSettingWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
		if( cmd == IOBN_BTNUP )
		{
			m_bAutoShow = false;
			HideWnd();
		}
		break;
	case ID_BOUND:
		if( cmd == IOBN_BTNUP )
		{
			ioCheckButton* pCheck = dynamic_cast<ioCheckButton*>( pWnd );
			if( pCheck )
			{	
				Setting::SetShowBoundBox( pCheck->IsChecked() );
			}
		}
		break;
	case ID_EXTRA:
		if( cmd == IOBN_BTNUP )
		{
			ioCheckButton* pCheck = dynamic_cast<ioCheckButton*>( pWnd );
			if( pCheck )
			{	
				Setting::SetShowExtraInfo( pCheck->IsChecked() );
			}
		}
		break;
	case ID_MEMORY:
		if( cmd == IOBN_BTNUP )
		{
			ioCheckButton* pCheck = dynamic_cast<ioCheckButton*>( pWnd );
			if( pCheck )
			{	
				Setting::SetShowMemoryUsage( pCheck->IsChecked() );
			}
		}
		break;
	case ID_MODE:
		if( cmd == IOBN_BTNUP )
		{
			OpenModekList( pWnd );
		}
		else if( cmd == ID_MODE_LIST )
		{
			UpdateMode( (int)param );
			UpdateModeSub( 0 );
			UpdateMapIdx( 0 );
		}
		else if( cmd == IOEX_BTNDOWN )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "싱글 모드에서만#사용 할 수 있습니다." );
		}
		break;
	case ID_MODE_SUB:
		if( cmd == IOBN_BTNUP )
		{
			OpenModeSubList( pWnd );
		}
		else if( cmd == ID_MODE_SUB_LIST )
		{	
			UpdateModeSub( (int)param );
			UpdateMapIdx( 0 );
		}
		else if( cmd == IOEX_BTNDOWN )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "싱글 모드에서만#사용 할 수 있습니다." );
		}
		break;
	case ID_MAP:
		if( cmd == IOBN_BTNUP )
		{
			OpenMapIdxList( pWnd );
		}
		else if( cmd == ID_MAP_LIST )
		{	
			UpdateMapIdx( (int)param );
		}
		else if( cmd == IOEX_BTNDOWN )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "싱글 모드에서만#사용 할 수 있습니다." );
		}
		break;
	case ID_MODE_APPLY:
		if( cmd == IOBN_BTNUP )
		{
			if( m_CurrModeType == MT_NONE )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "모드를 선택해주세요" );
				return;
			}
			else if( m_CurrModeSub == 0 )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "맵을 선택해주세요" );
				return;
			}
			else if( m_CurrModeMapIndex == 0 )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "맵인덱스를 선택해주세요" );
				return;
			}

			Setting::SetReservedMode( m_CurrModeType );
			Setting::SetModeSub( m_CurrModeSub );
			Setting::SetModeMap( m_CurrModeMapIndex );
			g_App.Reload();
		}
		else if( cmd == IOEX_BTNDOWN )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "싱글 모드에서만#사용 할 수 있습니다." );
		}
	case ID_SPHERE:
		if( cmd == IOBN_BTNUP )
		{
			ioCheckButton* pCheck = dynamic_cast<ioCheckButton*>( pWnd );
			if( pCheck )
			{	
				Setting::SetShowBoundSphere( pCheck->IsChecked() );
			}
		}
		break;
	case ID_PAUSE:
		if( cmd == IOBN_BTNUP )
		{
			SetPause( !IsPause() );
			g_FrameTimer.SetTimerPause( IsPause() );
			UpdatePauseButton();
		}
		else if( cmd == IOEX_BTNDOWN )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "싱글 모드에서만#사용 할 수 있습니다." );
		}
		break;
	case ID_CHARACTER:
		if( cmd == IOBN_BTNUP )
		{
			OpenCharacterList( pWnd );
		}
		else if( cmd == ID_CHARACTER_LIST )
		{			
			ioPlayStage* pStage = g_GUIMgr.GetPlayStage();
			if( pStage )
			{
				ioBaseChar* pChar = pStage->GetBaseChar( (DWORD)param );
				if( pChar )
				{
					CharGameInfoListWnd* pInfoWnd = dynamic_cast<CharGameInfoListWnd*>( g_GUIMgr.FindWnd( CHAR_INFO_LIST_WND ) );
					if( pInfoWnd )
					{
						pInfoWnd->SetWndPos( GetDerivedPosX() - pInfoWnd->GetWidth() - 5, GetDerivedPosY() );
						pInfoWnd->SetStateInfoChar( pChar );
						pInfoWnd->ShowWnd();
					}
					ChangeCharacterButtonTitle( pChar->GetCharName() );
					return;
				}
			}

			g_GUIMgr.HideWnd( CHAR_INFO_LIST_WND );
		}
		else if( cmd == IOEX_BTNDOWN )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "싱글 모드에서만#사용 할 수 있습니다." );
		}
		break;
	case ID_OWER:
		if( cmd == IOBN_BTNUP )
		{
			OpenOwnerList( pWnd );
		}
		else if( cmd == ID_OWER_LIST )
		{
			ioPlayStage* pStage = g_GUIMgr.GetPlayStage();
			if( pStage )
			{
				const ioBaseChar* pChar = pStage->GetBaseChar( (DWORD)param );
				if( pChar )
				{
					ChangeOwnerButtonTitle( pChar->GetCharName() );
					return;
				}
			}
		}
		else if( cmd == IOEX_BTNDOWN )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "싱글 모드에서만#사용 할 수 있습니다." );
		}
		break;
	case ID_WND_BOUNDARY:
		if( cmd == IOBN_BTNUP )
		{
			ioCheckButton* pCheck = dynamic_cast<ioCheckButton*>( pWnd );
			if( pCheck )
			{	
				Setting::SetSWndBoundary( pCheck->IsChecked() );
			}
		}
		break;
	}
}

void SingleSettingWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();
}