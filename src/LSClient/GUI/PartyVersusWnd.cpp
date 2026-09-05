

#include "StdAfx.h"

#include "../ioGuildMarkMgr.h"
#include "../ioTournamentManager.h"
#include "../ioTournamentTeamManager.h"
#include "../ioRankBattleManager.h"

#include "ioInterfaceBackground.h"
#include "ChatMsgWnd.h"
#include "PartyVersusWnd.h"
#include "ModeMapSelectList.h"
#include "MonsterCoinWnd.h"
#include "ResolutionList.h"
#include "UserInfoToolTip.h"
#include "TimeGateSearch.h"


ActiveScreenBtn::ActiveScreenBtn()
{
	m_bScreen = false;
	m_dwScreenTimer = FRAMEGETTIME();
}

ActiveScreenBtn::~ActiveScreenBtn()
{

}

void ActiveScreenBtn::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;
	ioButton::OnProcess( fTimePerSec );

	if( FRAMEGETTIME() - m_dwScreenTimer > 500 )
	{
		m_dwScreenTimer = FRAMEGETTIME();
		m_bScreen = !m_bScreen;
	}
}

void ActiveScreenBtn::OnDrawNormal( int iXPos, int iYPos )
{
	if( m_bScreen )
	{
		ioButton::OnDrawOvered( iXPos, iYPos );
	}
	else
	{
		ioButton::OnDrawNormal( iXPos, iYPos );
	}
}
//////////////////////////////////////////////////////////////////////////
PartyMemberList::PartyMemberList()
{
	m_pBlueNor		= NULL;
	m_pBlueOver		= NULL;
	m_pRedNor		= NULL;
	m_pRedOver		= NULL;
	m_pCaptainBack	= NULL;

	m_pBlueNorGra		= NULL;
	m_pBlueOverGra		= NULL;
	m_pRedNorGra		= NULL;
	m_pRedOverGra		= NULL;
	m_pGuildMarkLine	= NULL;
	m_pGoldMonsterCoin	= NULL;
	m_pMonsterCoin		= NULL;
	m_pRaidCoin			= NULL;
	m_pCoinNumber		= NULL;
	m_pCoinUnlimited	= NULL;
	m_pBlueEmpty		= NULL;
	m_pRedEmpty			= NULL;

	m_pCurrentNor		= NULL;
	m_pCurrentOver		= NULL;
	m_pCurrentNorGra	= NULL;
	m_pCurrentOverGra	= NULL;
	m_pCurrentEmptyX    = NULL;

	m_pLoadCharBack     = NULL;
	for(int i = 0;i < MAX_CHAR_LODING;i++)
		m_pCharLoading[i] = NULL;

	m_iGradeLevel = 0;
	m_eUITeam     = TEAM_NONE;
	m_ePrivateTeam= TEAM_NONE;
	m_bRenderCoin = false;
	m_dwCharLoadTime = 0;
	m_iCharLoadArray = 0;
	m_dwCharIndex    = 0;
	m_dwCharMotionItem = 0;

	m_pUI3DRender    = NULL;
	m_pUI3DRender_BattleMode = NULL;
	m_aMyChar	     = NULL;
	m_pAIBG = NULL;
	m_bAI = false;
}

PartyMemberList::~PartyMemberList()
{
	SAFEDELETE( m_pBlueNor );
	SAFEDELETE( m_pBlueOver );
	SAFEDELETE( m_pRedNor );
	SAFEDELETE( m_pRedOver );
	SAFEDELETE( m_pCaptainBack );

	SAFEDELETE( m_pBlueNorGra );
	SAFEDELETE( m_pBlueOverGra );
	SAFEDELETE( m_pRedNorGra );
	SAFEDELETE( m_pRedOverGra );
	SAFEDELETE( m_pGuildMarkLine );
	SAFEDELETE( m_pGoldMonsterCoin );
	SAFEDELETE( m_pMonsterCoin );
	SAFEDELETE( m_pRaidCoin );
	SAFEDELETE( m_pCoinNumber );
	SAFEDELETE( m_pCoinUnlimited );
	SAFEDELETE( m_pBlueEmpty );
	SAFEDELETE( m_pRedEmpty );

	SAFEDELETE( m_pLoadCharBack );
	for(int i = 0;i < MAX_CHAR_LODING;i++)
		SAFEDELETE( m_pCharLoading[i] );

	SAFEDELETE( m_aMyChar );
	SAFEDELETE( m_pUI3DRender );
	SAFEDELETE( m_pUI3DRender_BattleMode );
	SAFEDELETE( m_pAIBG );
}

void PartyMemberList::iwm_create()
{
	SAFEDELETE( m_pUI3DRender );
	SAFEDELETE( m_pUI3DRender_BattleMode );

	m_pUI3DRender = new ioUI3DRender;
	m_pUI3DRender->Initialize( "config/sp2_stage_charselect_map.ini", "battle_cam" );
	m_pUI3DRender_BattleMode = new ioUI3DRender;
	m_pUI3DRender_BattleMode->Initialize( "config/sp2_stage_charselect_map.ini", "ladder_cam" );

	g_GUIMgr.AddWnd( "XML/chatbubblewnd.xml", this );

	m_PingStep.Create();
}

void PartyMemberList::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "BlueNorGra" )
	{
		SAFEDELETE( m_pBlueNorGra );
		m_pBlueNorGra = pImage;
	}
	else if( szType == "BlueOverGra" )
	{
		SAFEDELETE( m_pBlueOverGra );
		m_pBlueOverGra = pImage;
	}
	else if( szType == "RedNorGra" )
	{
		SAFEDELETE( m_pRedNorGra );
		m_pRedNorGra = pImage;
	}
	else if( szType == "RedOverGra" )
	{
		SAFEDELETE( m_pRedOverGra );
		m_pRedOverGra = pImage;
	}
	else if( szType == "GuildMarkLine" )
	{
		SAFEDELETE( m_pGuildMarkLine );
		m_pGuildMarkLine = pImage;
	}
	else if( szType == "GoldMonsterCoin" )
	{
		SAFEDELETE( m_pGoldMonsterCoin );
		m_pGoldMonsterCoin = pImage;
	}
	else if( szType == "MonsterCoin" )
	{
		SAFEDELETE( m_pMonsterCoin );
		m_pMonsterCoin = pImage;
	}
	else if( szType == "RaidCoin" )
	{
		SAFEDELETE( m_pRaidCoin );
		m_pRaidCoin = pImage;
	}
	else if( szType == "CoinNumber" )
	{
		SAFEDELETE( m_pCoinNumber );
		m_pCoinNumber = pImage;
	}
	else if( szType == "CoinUnlimited" )
	{
		SAFEDELETE( m_pCoinUnlimited );
		m_pCoinUnlimited = pImage;
	}
	else if( szType == "BlueEmpty" )
	{
		SAFEDELETE( m_pBlueEmpty );
		m_pBlueEmpty = pImage;
	}
	else if( szType == "RedEmpty" )
	{
		SAFEDELETE( m_pRedEmpty );
		m_pRedEmpty = pImage;
	}
	else if( szType == "LoadCharBack" )
	{
		SAFEDELETE( m_pLoadCharBack );
		m_pLoadCharBack = pImage;
	}
	else if( szType == "CharLoading1" )
	{
		SAFEDELETE( m_pCharLoading[0] );
		m_pCharLoading[0] = pImage;
	}
	else if( szType == "CharLoading2" )
	{
		SAFEDELETE( m_pCharLoading[1] );
		m_pCharLoading[1] = pImage;
	}
	else if( szType == "CharLoading3" )
	{
		SAFEDELETE( m_pCharLoading[2] );
		m_pCharLoading[2] = pImage;
	}
	else if( szType == "CharLoading4" )
	{
		SAFEDELETE( m_pCharLoading[3] );
		m_pCharLoading[3] = pImage;
	}
	else if( szType == "CharLoading5" )
	{
		SAFEDELETE( m_pCharLoading[4] );
		m_pCharLoading[4] = pImage;
	}
	else if( szType == "CharLoading6" )
	{
		SAFEDELETE( m_pCharLoading[5] );
		m_pCharLoading[5] = pImage;
	}
	else if( szType == "CharLoading7" )
	{
		SAFEDELETE( m_pCharLoading[6] );
		m_pCharLoading[6] = pImage;
	}
	else if( szType == "CharLoading8" )
	{
		SAFEDELETE( m_pCharLoading[7] );
		m_pCharLoading[7] = pImage;
	}
	else if( szType == "AIBG" )
	{
		SAFEDELETE( m_pAIBG );
		m_pAIBG = pImage;
	}
	else
		ioButton::AddRenderImage( szType, pImage );
}

void PartyMemberList::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "BlueNor" )
	{
		SAFEDELETE( m_pBlueNor );
		m_pBlueNor = pFrame;
	}
	else if( szType == "BlueOver" )
	{
		SAFEDELETE( m_pBlueOver );
		m_pBlueOver = pFrame;
	}
	else if( szType == "RedNor" )
	{
		SAFEDELETE( m_pRedNor );
		m_pRedNor = pFrame;
	}
	else if( szType == "RedOver" )
	{
		SAFEDELETE( m_pRedOver );
		m_pRedOver = pFrame;
	}
	else if( szType == "CaptainBack" )
	{
		SAFEDELETE( m_pCaptainBack );
		m_pCaptainBack = pFrame;
	}
	else
		ioButton::AddRenderFrame( szType, pFrame );
}

void PartyMemberList::StartChatBubble( const char *szID, const char *szChat, bool bWholeChat )
{
	ChatBubbleWnd *pChatBubbleWnd = dynamic_cast<ChatBubbleWnd*>(FindChildWnd( ID_CHAT_BUBBLE ));
	if( pChatBubbleWnd )
		pChatBubbleWnd->StartChatBubble( szID, szChat, bWholeChat );
}

bool PartyMemberList::IsChangeChatBubble()
{
	ChatBubbleWnd *pChatBubbleWnd = dynamic_cast<ChatBubbleWnd*>(FindChildWnd( ID_CHAT_BUBBLE ));
	if( pChatBubbleWnd && pChatBubbleWnd->IsShow() )
	{
		if( GetListName() != pChatBubbleWnd->GetBubbleName() )
			return true;
	}
	return false;    	
}

void PartyMemberList::ClearChatBubble()
{
	ChatBubbleWnd *pChatBubbleWnd = dynamic_cast<ChatBubbleWnd*>(FindChildWnd( ID_CHAT_BUBBLE ));
	if( pChatBubbleWnd && pChatBubbleWnd->IsShow() )
	{
		if( GetListName() != pChatBubbleWnd->GetBubbleName() )
		{	
			pChatBubbleWnd->HideWnd();
		}
	}
}

void PartyMemberList::ChangeChatBubble( PartyMemberList *pMemberList )
{
	if( !pMemberList ) return;

	ChatBubbleWnd *pChatBubbleWnd = dynamic_cast<ChatBubbleWnd*>(FindChildWnd( ID_CHAT_BUBBLE ));
	if( pChatBubbleWnd )
	{
		ChatBubbleWnd *pTempChatBubble = dynamic_cast<ChatBubbleWnd*>(pMemberList->FindChildWnd( ID_CHAT_BUBBLE ));
		if( pTempChatBubble )
		{
			if( GetListName() == pTempChatBubble->GetBubbleName() )
				pChatBubbleWnd->ChangeChatBubble( pTempChatBubble );
		}
	}
}

void PartyMemberList::CreateCharacter()
{
	SAFEDELETE( m_aMyChar );

	m_CharInfo = g_GlobalSoldierInfo.GetSoldierData( m_szName, m_dwCharIndex );
	if( m_CharInfo.m_class_type == 0 )
		return;

	float fView_Height = VIEW_HEIGHT;
	float fCharYPos = -120.0f;
	ioUI3DRender* pUI3DRender = m_pUI3DRender;
	ModeType eModeType = (ModeType)g_ModeSelectInfo.GetBattleMode(g_BattleRoomMgr.GetSelectModeIndex());
	if(eModeType == MT_BATTLE)
	{
		fView_Height = VIEW_BATTLE_MODE_HEIGHT;
		fCharYPos = -FLOAT90;
		pUI3DRender = m_pUI3DRender_BattleMode;
	}

	ioCreateChar *pChar = pUI3DRender->CreateChar( m_CharInfo, "ResultChar" );
	if( !pChar )	return;

	pChar->SetPosition( 0.0f, fCharYPos, 900.0f );
	pChar->SetYaw( FLOAT10 );
	pUI3DRender->SetLookAt( 0.0f, 0.0f, 900.0f );

	float fAspect = (float)VIEW_WIDTH/fView_Height;
	pUI3DRender->SetProjection( FLOAT1, 600.0f, 45.0f, fAspect );

	m_aMyChar = pChar;

	// 코스튬 관련 (전투방 대기시)
	int TempCostume[MAX_INVENTORY];
	for ( int i=0; i<MAX_INVENTORY; i++ )
	{
		TempCostume[i] = m_CharInfo.m_EquipCostume[i].m_CostumeCode;
		
		// 코스튬 관련 (자신만 적용)
		if ( !Setting::IsCostumeShowChange() && !g_CostumeInfoMgr.IsEnableCostumeMode() && m_szName != g_MyInfo.GetPublicID() )
			TempCostume[i] = 0;
	}

	const ioSetItemInfo *pSetInfo = g_SetItemInfoMgr.GetSetInfoByIdx( m_CharInfo.m_class_type - 1 );
	if( pSetInfo )
	{
		ConstItemVector rkItemList = pSetInfo->GetSetItemList();
		int iMaxItemCnt = min( rkItemList.size(), MAX_INVENTORY );
		for(int i = 0;i < iMaxItemCnt;i++)
		{			
			DWORD dwItemCode = g_GlobalSoldierInfo.GetSoldierItemCode( m_szName, m_dwCharIndex, i );
			DWORD dwCostumeMaleCustom = m_CharInfo.m_EquipCostume[i].m_Costume_Male_Custom;
			DWORD dwCostumeFemaleCustom = m_CharInfo.m_EquipCostume[i].m_Costume_Female_Custom;

			if( dwItemCode == 0 )
			{
				if ( m_CharInfo.m_EquipCostume[i].m_CostumeCode <= 0 )
				{
					dwCostumeMaleCustom = 0;
					dwCostumeFemaleCustom = 0;
				}
				pChar->EquipItem( rkItemList[i]->GetItemCode(), dwCostumeMaleCustom, dwCostumeFemaleCustom, TempCostume[i] );
			}
			else
			{
				if ( m_CharInfo.m_EquipCostume[i].m_CostumeCode <= 0 )
				{
					dwCostumeMaleCustom = g_GlobalSoldierInfo.GetSoldierItemMaleCustom( m_szName, m_dwCharIndex, i );
					dwCostumeFemaleCustom = g_GlobalSoldierInfo.GetSoldierItemFemaleCustom( m_szName, m_dwCharIndex, i );
				}
				pChar->EquipItem( dwItemCode, dwCostumeMaleCustom, dwCostumeFemaleCustom, TempCostume[i] );
			}
		}
	}

	m_dwCharMotionItem = g_BattleRoomMgr.GetUserCharMotion( m_szName );
	ioHashString kMotionAni;
	ioEtcItemMotion *pMotionItem = dynamic_cast< ioEtcItemMotion * > ( g_EtcItemMgr.FindEtcItem( m_dwCharMotionItem ) );
	if( pMotionItem )
	{
		if( m_aMyChar->IsMale() )
			kMotionAni = pMotionItem->GetHumanManMotion();
		else
			kMotionAni = pMotionItem->GetHumanWomanMotion();
	}

	if( kMotionAni.IsEmpty() )
	{
		if( !m_aMyChar->SetWeaponShopMotion() )
		{
			ioHashString szUIAnimation = pUI3DRender->GetModelAnimation( 0, m_aMyChar->IsMale() );
			if( !szUIAnimation.IsEmpty() )
			{
				m_aMyChar->SetLoopAni( szUIAnimation );
			}
		}
	}
	else
	{
		m_aMyChar->SetMotionItemAni( kMotionAni );
	}
}

void PartyMemberList::MemberInfo( TeamType eUITeam, int iListArray, int iGradeLevel, ioHashString szName, int iMonsterCoin, int iGoldMonsterCoin, int iRaidCoin, bool bAI )
{
	m_eUITeam           = eUITeam;
	m_iGradeLevel		= iGradeLevel;
	m_iMonsterCoin		= iMonsterCoin;
	m_iGoldMonsterCoin	= iGoldMonsterCoin;
	m_iRaidCoin			= iRaidCoin;
	m_szName			= szName;	
	m_iListArray        = iListArray;
	m_bRenderCoin       = true;
	m_bAI				= bAI;

	if( m_iMonsterCoin == -1 && m_iGoldMonsterCoin == -1 && m_iRaidCoin == -1 )
	{
		m_bRenderCoin = false;
	}

	if( m_eUITeam == TEAM_BLUE )
	{
		m_pCurrentNor		= m_pBlueNor;
		m_pCurrentOver		= m_pBlueOver;
		m_pCurrentNorGra	= m_pBlueNorGra;
		m_pCurrentOverGra	= m_pBlueOverGra;
		m_pCurrentEmptyX    = m_pBlueEmpty;
	}
	else 
	{
		m_pCurrentNor		= m_pRedNor;
		m_pCurrentOver		= m_pRedOver;
		m_pCurrentNorGra	= m_pRedNorGra;
		m_pCurrentOverGra	= m_pRedOverGra;
		m_pCurrentEmptyX    = m_pRedEmpty;
	}

	if( COMPARE( GetID(), PartyShamBattleWnd::ID_BLUE_USER_1, PartyShamBattleWnd::ID_BLUE_BATTLE_USER_4 + 1 ) )
		m_ePrivateTeam = TEAM_BLUE;
	else
		m_ePrivateTeam = TEAM_RED;

	m_dwCharIndex  = g_BattleRoomMgr.GetUserCharIndex( m_szName );
	if( m_CharInfo != g_GlobalSoldierInfo.GetSoldierData( m_szName, m_dwCharIndex ) ||
		m_dwCharMotionItem != g_BattleRoomMgr.GetUserCharMotion( m_szName ) )
	{
		CreateCharacter();
	}
}

void PartyMemberList::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;
	if( m_szName.IsEmpty() ) return;
	if( !m_bAI && m_bOver && !m_bClicked )
	{
		ioLSMouse *pMouse = g_App.GetMouse();
		if( pMouse && pMouse->IsMouseShowing() )
		{
			// 상태 캐릭터가 보이므로 실시간 동기화
			CHARACTER kChar         = g_GlobalSoldierInfo.GetSoldierData( m_szName, m_dwCharIndex );
			UserInfoNode *pUserInfo = g_UserInfoMgr.GetUserInfoNode( m_szName );
			if( pUserInfo )
			{
				if( pUserInfo->GetClassType() != kChar.m_class_type )
					pUserInfo->SetForceUDPSync( true );
			}
			pMouse->SetToolTipID( m_szName, UserInfoToolTip::TT_GLOBAL );
		}
	}	

	if( FRAMEGETTIME() - m_dwCharLoadTime > 100 )
	{
		m_dwCharLoadTime = FRAMEGETTIME();
		m_iCharLoadArray++;
		if( m_iCharLoadArray >= MAX_CHAR_LODING )
			m_iCharLoadArray = 0;
	}

	if( m_CharInfo != g_GlobalSoldierInfo.GetSoldierData( m_szName, m_dwCharIndex ) )
	{
		CreateCharacter();
	}

	if( m_aMyChar )
		m_aMyChar->Update( fTimePerSec );
}

void PartyMemberList::OnRenderSoldier( int iXPos, int iYPos )
{
	if( m_szName.IsEmpty() ) return;
	if( !m_pUI3DRender || !m_pUI3DRender_BattleMode || !m_aMyChar ) return;

	D3DRECT rcD3D;
	rcD3D.x1 = iXPos;
	rcD3D.y1 = iYPos;
	rcD3D.x2 = rcD3D.x1 + VIEW_WIDTH;
	rcD3D.y2 = rcD3D.y1 + VIEW_HEIGHT;
	ModeType eModeType = (ModeType)g_ModeSelectInfo.GetBattleMode(g_BattleRoomMgr.GetSelectModeIndex());
	if(eModeType == MT_BATTLE)
	{
		rcD3D.y2 = rcD3D.y1 + VIEW_BATTLE_MODE_HEIGHT;
		m_pUI3DRender_BattleMode->RenderViewPort( &rcD3D, true );
	}
	else
		m_pUI3DRender->RenderViewPort( &rcD3D, true );
}

void PartyMemberList::OnRenderFrame( int iXPos, int iYPos )
{
	if( !m_pCurrentNor || !m_pCurrentOver )
		return;

	// 배틀모드일대만 Y값 변경
	int iWeightY = 93;
	ModifyUIYPosByMode(iWeightY, true);

	if( m_bOver || m_bClicked || g_MyInfo.GetPublicID() == m_szName )
	{
		m_pCurrentOver->Render( iXPos, iYPos );

		if( m_aMyChar == NULL )
			OnRenderLoadingChar( iXPos, iYPos );
		else
			OnRenderSoldier( iXPos + 3, iYPos + 3 );

		m_pCurrentOverGra->SetSize( 82, 40 );
		m_pCurrentOverGra->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
		m_pCurrentOverGra->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
		m_pCurrentOverGra->Render( iXPos + 3, iYPos + iWeightY );
	}	
	else
	{
		m_pCurrentNor->Render( iXPos, iYPos );

		if( m_aMyChar == NULL )
			OnRenderLoadingChar( iXPos, iYPos );
		else
			OnRenderSoldier( iXPos + 3, iYPos + 3 );

		m_pCurrentNorGra->SetSize( 82, 40 );
		m_pCurrentNorGra->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
		m_pCurrentNorGra->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
		m_pCurrentNorGra->Render( iXPos + 3, iYPos + iWeightY );
	}
}

void PartyMemberList::OnRenderLoadingChar( int iXPos, int iYPos )
{
	if( m_szName.IsEmpty() ) return;
	if( !COMPARE( m_iCharLoadArray, 0, MAX_CHAR_LODING ) )
		return;
	if( m_bAI )
	{
		if( m_pAIBG )
			m_pAIBG->Render( iXPos, iYPos );
	}
	else
	{
		if( m_pLoadCharBack )
			m_pLoadCharBack->Render( iXPos, iYPos );

		if( m_pCharLoading[m_iCharLoadArray] )
			m_pCharLoading[m_iCharLoadArray]->Render( iXPos + 32, iYPos + 54 );
	}	
}

void PartyMemberList::OnRender()
{
	ioButton::OnRender();

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	OnRenderFrame( iXPos, iYPos );

	int iWeightY = 0;
	ModeType eModeType = ModifyUIYPosByMode(iYPos, false);
	if(eModeType == MT_BATTLE)
		iWeightY = 110;
	
	int iMaxPlayer = g_BattleRoomMgr.GetMaxPlayerBlue();
	if( m_ePrivateTeam == TEAM_RED )
		iMaxPlayer = g_BattleRoomMgr.GetMaxPlayerRed();
	if(eModeType == MT_BATTLE)
		iMaxPlayer += 1;
	if( !m_bAI && (m_iListArray > iMaxPlayer) )
	{
		if( m_pCurrentEmptyX )
			m_pCurrentEmptyX->Render( iXPos + 18, iYPos + 40, UI_RENDER_MULTIPLY );
	}
	else if( m_szName.IsEmpty() )
	{
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		if( m_eUITeam == TEAM_RED )
		{
			g_FontMgr.SetTextColor( 204, 69, 52 );
		}
		else
		{
			g_FontMgr.SetTextColor( 60, 126, 207 );
		}
		int iEmptyTextYPos = iYPos;
		if(eModeType == MT_BATTLE)
			iEmptyTextYPos += 35;

		g_FontMgr.PrintText( iXPos + 44, iEmptyTextYPos + 112, FONT_SIZE_11, STR(1), m_iListArray );
	}
	else
	{
		if( g_BattleRoomMgr.GetOwnerName() == m_szName && !g_BattleRoomMgr.IsTournamentModeType() )
		{	
			int iRoomOwnerTextYPos = iYPos;
			if(eModeType == MT_BATTLE)
				iRoomOwnerTextYPos += 35;

			if( m_pCaptainBack )
				m_pCaptainBack->Render( iXPos, iRoomOwnerTextYPos - iWeightY );

			g_FontMgr.SetTextStyle( TS_NORMAL );
			g_FontMgr.SetAlignType( TAT_CENTER );
			g_FontMgr.SetBkColor( 0, 0, 0 );
			g_FontMgr.SetTextColor( 255, 255, 255 );
			g_FontMgr.PrintText( iXPos + 26, iRoomOwnerTextYPos - iWeightY + 8, FONT_SIZE_11, STR(2) );
		}			
		
		enum{ GRADE_GAP = 17, GUILD_MARK_GAP = 17, GOLD_COIN_GAP = 18, COIN_GAP = 18, };
		DWORD dwGuildIndex, dwGuildMark;
		g_UserInfoMgr.GetGuildInfo( m_szName, dwGuildIndex, dwGuildMark );
		int iGradeLineSize = GRADE_GAP;
		if( dwGuildIndex > 0 )           // 길드가 있으면
		{
			iGradeLineSize += GUILD_MARK_GAP;
		}		
		if( m_bRenderCoin )
		{
			if(Help::IsMonsterDungeonMode((ModeType)g_ModeSelectInfo.GetBattleMode(g_BattleRoomMgr.GetSelectModeIndex())))
				iGradeLineSize += COIN_GAP;
			else
				iGradeLineSize += GOLD_COIN_GAP + COIN_GAP;
		}

		int iStartXPos = 44 - iGradeLineSize / 2;
		g_LevelMgr.RenderGrade( m_szName, iXPos + iStartXPos, iYPos + 93, m_iGradeLevel, TEAM_PRIVATE_2 );
		iStartXPos += GRADE_GAP;
		if( dwGuildIndex > 0 )   
		{
			if( m_pGuildMarkLine )
				m_pGuildMarkLine->Render( iXPos + iStartXPos, iYPos + 93 );
			g_GuildMarkMgr.RenderSmallMark( dwGuildIndex, dwGuildMark, iXPos + iStartXPos + 2, iYPos + 95 );			
			iStartXPos += GUILD_MARK_GAP;
		}
		if( m_bRenderCoin && m_pGoldMonsterCoin && m_pMonsterCoin && m_pRaidCoin && m_pCoinNumber && m_pCoinUnlimited )
		{
			ModeType eModeType = (ModeType)g_ModeSelectInfo.GetBattleMode(g_BattleRoomMgr.GetSelectModeIndex());
			if(Help::IsMonsterDungeonMode(eModeType))
			{
				m_pCoinNumber->SetHorzFormatting( ioUIRenderImage::HF_CENTER );
				// 몬스터 코인 표시
				m_pMonsterCoin->Render( iXPos + iStartXPos, iYPos + 93 );
				if( m_iMonsterCoin >= 100 )
					m_pCoinUnlimited->Render( iXPos + iStartXPos + 4, iYPos + 99 );
				else
					m_pCoinNumber->RenderNum( iXPos + iStartXPos + 9, iYPos + 99, m_iMonsterCoin );
				iStartXPos += COIN_GAP;
			}
			else if( eModeType == MT_RAID )
			{
				// 몬스터 코인 표시
				m_pRaidCoin->Render( iXPos + iStartXPos, iYPos + 94 );
				if( m_iRaidCoin >= 100 )
					m_pCoinUnlimited->Render( iXPos + iStartXPos + 8, iYPos + 98 );
				else if ( m_iRaidCoin >= 10 )
					m_pCoinNumber->RenderNum( iXPos + iStartXPos + 7, iYPos + 98, m_iRaidCoin );
				else
					m_pCoinNumber->RenderNum( iXPos + iStartXPos + 10, iYPos + 98, m_iRaidCoin );
				iStartXPos += COIN_GAP;
			}
			else
			{
				m_pCoinNumber->SetHorzFormatting( ioUIRenderImage::HF_CENTER );
				// 몬스터 코인 표시
				m_pMonsterCoin->Render( iXPos + iStartXPos, iYPos + 93 );
				if( m_iMonsterCoin >= 100 )
					m_pCoinUnlimited->Render( iXPos + iStartXPos + 4, iYPos + 99 );
				else
					m_pCoinNumber->RenderNum( iXPos + iStartXPos + 9, iYPos + 99, m_iMonsterCoin );
				iStartXPos += COIN_GAP;

				// 골드 몬스터 코인 표시
				m_pGoldMonsterCoin->Render( iXPos + iStartXPos, iYPos + 93 );
				if( m_iGoldMonsterCoin >= 100 )
					m_pCoinUnlimited->Render( iXPos + iStartXPos + 4, iYPos + 99 );
				else
					m_pCoinNumber->RenderNum( iXPos + iStartXPos + 9, iYPos + 99, m_iGoldMonsterCoin );
				iStartXPos += GOLD_COIN_GAP;
			}
		}

		//닉네임
		g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_3X );
		g_FontMgr.SetAlignType( TAT_CENTER );
		if( m_eUITeam == TEAM_BLUE )
		{
			if( m_bClicked || m_bOver )
				g_FontMgr.SetBkColor( 50, 118, 176 );
			else
				g_FontMgr.SetBkColor( 27, 71, 128 );
		}
		else
		{
			if( m_bClicked || m_bOver )
				g_FontMgr.SetBkColor( 172, 62, 42 );
			else
				g_FontMgr.SetBkColor( 126, 34, 22 );
		}
		g_FontMgr.SetTextColor( 255, 255, 255 );
		g_FontMgr.PrintTextWidthCut( iXPos + 44, iYPos + 110, FONT_SIZE_11, 80.0f, m_szName.c_str() );
		if( !m_bAI )
		{
			int iPingYPos = iYPos;
			if(eModeType == MT_BATTLE)
				iPingYPos += 35;
			OnRenderPing( iXPos + 59, iPingYPos - iWeightY + 5 );
		}
	}
}

void PartyMemberList::OnRenderPing( int iXPos, int iYPos )
{
	bool bServerRelay = false;
	int  iPingStep = 0;
	if( m_szName == g_MyInfo.GetPublicID() )
	{
		iPingStep = g_GlobalTimer.GetServerDelayMS() / PingStepWnd::PING_MS_VALUE;	
	}			
	else
	{
		iPingStep = abs(  P2PNetwork::GetP2pPingTime( m_szName ) ) / PingStepWnd::PING_MS_VALUE;
		if( P2PNetwork::IsServerRelay( m_szName ) )
			bServerRelay = true;
	}
	m_PingStep.RenderPing( iXPos, iYPos, iPingStep, bServerRelay );
}

ModeType PartyMemberList::ModifyUIYPosByMode(int& iYPos, bool bFrame)
{
	ModeType eModeType = (ModeType)g_ModeSelectInfo.GetBattleMode(g_BattleRoomMgr.GetSelectModeIndex());
	if(eModeType == MT_BATTLE)
	{
		if( bFrame )
			iYPos += 110;
		else
			iYPos += 75;
	}
	return eModeType;
}
//////////////////////////////////////////////////////////////////////////
PartyModeNMapBtn::PartyModeNMapBtn()
{
	m_dwUserIndex = 0;
}

PartyModeNMapBtn::~PartyModeNMapBtn()
{
}

void PartyModeNMapBtn::OnRender()
{
	ioButton::OnRender();

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();
	
	switch( GetID() )
	{
	case PartyShamBattleWnd::ID_CHANGE_MODE:
		OnModeRender( iXPos, iYPos );
		break;
	case PartyShamBattleWnd::ID_CHANGE_MAP:
		OnMapRender( iXPos, iYPos );
		break;
	case PartyShamBattleWnd::ID_CHANGE_AI_LEVEL:
		OnAiRender( iXPos, iYPos );
		break;
	case PartyShamBattleWnd::ID_EXHIBITION_BLUE_ORDER_1:
	case PartyShamBattleWnd::ID_EXHIBITION_BLUE_ORDER_2:
	case PartyShamBattleWnd::ID_EXHIBITION_BLUE_ORDER_3:
	case PartyShamBattleWnd::ID_EXHIBITION_RED_ORDER_1:
	case PartyShamBattleWnd::ID_EXHIBITION_RED_ORDER_2:
	case PartyShamBattleWnd::ID_EXHIBITION_RED_ORDER_3:
		OnBattleOrderRender( iXPos, iYPos );
		break;
	}

}

void PartyModeNMapBtn::OnModeRender( int iXPos, int iYPos )
{
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 26, iYPos + 8, FONT_SIZE_12, STR(1) );

	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( iXPos + 46, iYPos + 8, FONT_SIZE_12, "|" );

	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );

	ioHashString kModeTitle = g_ModeSelectInfo.GetBattleModeTitle( g_BattleRoomMgr.GetSelectModeIndex() );
	g_FontMgr.PrintTextWidthCut( iXPos + 54, iYPos + 8, FONT_SIZE_12,FLOAT90, kModeTitle.c_str() );

	ioUIRenderImage *pOrderMark = g_ModeSelectInfo.GetBattleModeOrderMarkSmall( g_BattleRoomMgr.GetSelectModeIndex() );
	if( pOrderMark )
	{
		int iTitleSize = g_FontMgr.GetTextWidthCutSize( kModeTitle.c_str(), TS_NORMAL, FONT_SIZE_12,FLOAT90 );
		pOrderMark->Render( iXPos + 56 + iTitleSize, iYPos + 10 );
	}
}

void PartyModeNMapBtn::OnMapRender( int iXPos, int iYPos )
{
	bool bUseMonsterCoin = g_ModeSelectInfo.IsCoinVisible( g_BattleRoomMgr.GetSelectModeIndex() );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	if( bUseMonsterCoin )
	{
		ModeType eModeType = (ModeType)g_ModeSelectInfo.GetBattleMode(g_BattleRoomMgr.GetSelectModeIndex());

		if (eModeType == MT_RAID)
			g_FontMgr.PrintText( iXPos + 26, iYPos + 8, FONT_SIZE_12, STR(2) );
		else
			g_FontMgr.PrintText( iXPos + 26, iYPos + 8, FONT_SIZE_12, STR(1) );

		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
		g_FontMgr.PrintText( iXPos + 46, iYPos + 8, FONT_SIZE_12, "|" );

		g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );

		ioHashString kMapTitle = g_ModeSelectInfo.GetBattleMapTitle( g_BattleRoomMgr.GetSelectModeIndex(), g_BattleRoomMgr.GetSelectMapIndex() );
		g_FontMgr.PrintTextWidthCut( iXPos + 54, iYPos + 8, FONT_SIZE_12,FLOAT90, kMapTitle.c_str() );

		ioUIRenderImage *pOrderMark = g_ModeSelectInfo.GetBattleMapOrderMarkSmall( g_BattleRoomMgr.GetSelectModeIndex(), g_BattleRoomMgr.GetSelectMapIndex() );
		if( pOrderMark )
		{
			int iTitleSize = g_FontMgr.GetTextWidthCutSize( kMapTitle.c_str(), TS_NORMAL, FONT_SIZE_12,FLOAT90 );
			pOrderMark->Render( iXPos + 56 + iTitleSize, iYPos + 10 );
		}
	}
	else
	{
		bool bAIMode = g_ModeSelectInfo.IsAIMode( g_BattleRoomMgr.GetSelectModeIndex() );
		if( bAIMode )
		{
			g_FontMgr.SetAlignType( TAT_LEFT );
			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );

			ioHashString kMapTitle = g_ModeSelectInfo.GetBattleMapTitle( g_BattleRoomMgr.GetSelectModeIndex(), g_BattleRoomMgr.GetSelectMapIndex() );
			g_FontMgr.PrintTextWidthCut( iXPos + 11, iYPos + 8, FONT_SIZE_12,FLOAT90, kMapTitle.c_str() );
		}
		else
		{
			g_FontMgr.PrintText( iXPos + 26, iYPos + 8, FONT_SIZE_12, STR(2) );

			g_FontMgr.SetAlignType( TAT_LEFT );
			g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
			g_FontMgr.PrintText( iXPos + 46, iYPos + 8, FONT_SIZE_12, "|" );

			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );

			ioHashString kMapTitle = g_ModeSelectInfo.GetBattleMapTitle( g_BattleRoomMgr.GetSelectModeIndex(), g_BattleRoomMgr.GetSelectMapIndex() );
			g_FontMgr.PrintTextWidthCut( iXPos + 54, iYPos + 8, FONT_SIZE_12,FLOAT90, kMapTitle.c_str() );

			ioUIRenderImage *pOrderMark = g_ModeSelectInfo.GetBattleMapOrderMarkSmall( g_BattleRoomMgr.GetSelectModeIndex(), g_BattleRoomMgr.GetSelectMapIndex() );
			if( pOrderMark )
			{
				int iTitleSize = g_FontMgr.GetTextWidthCutSize( kMapTitle.c_str(), TS_NORMAL, FONT_SIZE_12,FLOAT90 );
				pOrderMark->Render( iXPos + 56 + iTitleSize, iYPos + 10 );
			}
		}
	}
}

void PartyModeNMapBtn::OnBattleOrderRender( int iXPos, int iYPos )
{
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	if( IsDisabled() )
		g_FontMgr.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
	else
		g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	ioHashString szBattleOrder = g_BattleRoomMgr.GetBattleOrder(m_dwUserIndex);
	g_FontMgr.PrintText( iXPos + 10, iYPos + 8, FONT_SIZE_10, szBattleOrder.c_str() );
}

void PartyModeNMapBtn::CheckFrameReSize( int iWidth, int iHeight, int iPrevWidth, int iPrevHeight )
{
	if( m_pNormalFrm && m_pNormalFrm->IsAutoReSize() )
		m_pNormalFrm->SetSize( iWidth, iHeight );

	if( m_pOverFrm && m_pOverFrm->IsAutoReSize() )
		m_pOverFrm->SetSize( iWidth, iHeight );

	if( m_pOverAddFrm && m_pOverAddFrm->IsAutoReSize() )
		m_pOverAddFrm->SetSize( iWidth, iHeight );

	if( m_pPushFrm && m_pPushFrm->IsAutoReSize() )
		m_pPushFrm->SetSize( iWidth, iHeight );
}

void PartyModeNMapBtn::OnAiMode( bool bSet )
{
	if( bSet )
	{
		ioButton::SetWidth( 95 );
		ioUIRenderElement* pEle = GetRenderElement( 0 );
		if( pEle )
			pEle->SetPosition( 80, pEle->GetPosY() );
	}
	else
	{
		ioButton::SetWidth( 163 );

		ioUIRenderElement* pEle = GetRenderElement( 0 );
		if( pEle )
			pEle->SetPosition( 148, pEle->GetPosY() );
	}
}

void PartyModeNMapBtn::OnAiRender( int iXPos, int iYPos )
{
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );

	ioHashString szLevel;
	int iAiLevel = g_BattleRoomMgr.GetAiLevel();
	if( iAiLevel == 0 )
		szLevel = "쉬움";
	else
		szLevel = "보통";
	g_FontMgr.PrintTextWidthCut( iXPos + 10, iYPos + 8, FONT_SIZE_12,FLOAT90, szLevel.c_str() );
}

//////////////////////////////////////////////////////////////////////////
BattleChatWnd::BattleChatWnd()
{
}

BattleChatWnd::~BattleChatWnd()
{
}

void BattleChatWnd::SetPlayStage( ioPlayStage *pPlayStage )
{
	GameChatInput *pGameChatInput = dynamic_cast<GameChatInput*>(FindChildWnd( ID_CHAT_INPUT_WND ));
	if( pGameChatInput )
	{
		pGameChatInput->SetPlayStage( pPlayStage );
		pGameChatInput->SetChatTypeToBtn( CT_PARTY );
	}
}

void BattleChatWnd::iwm_create()
{
}

void BattleChatWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_szLoadInfo = xElement.GetStringAttribute_e( "LoadInfo" );	
}

void BattleChatWnd::iwm_show()
{
	g_GUIMgr.HideWnd( GLOBAL_CHAT_WND );

	g_ChatMgr.LoadChatInfo( m_szLoadInfo );

	ShowChildWnd( ID_CHAT_MSG_WND );
	ShowChildWnd( ID_CHAT_INPUT_WND );

	GameChatInput *pGameChatInput = dynamic_cast<GameChatInput*>(FindChildWnd( ID_CHAT_INPUT_WND ));
	if( pGameChatInput )
	{
		pGameChatInput->SetFirstChatTypeToBtn( CT_PARTY );
	}

	g_ChatMgr.SetMsgWndFeature( MSF_NORMAL_CHAT | MSF_SYSTEM_CHAT | MSF_WHOLE_CHAT );
}

void BattleChatWnd::iwm_hide()
{
	HideChildWnd( ID_CHAT_MSG_WND );
	HideChildWnd( ID_CHAT_INPUT_WND );
}

bool BattleChatWnd::iwm_esc()
{
	GameChatInput *pGameChatInput = dynamic_cast<GameChatInput*>(FindChildWnd( ID_CHAT_INPUT_WND ));
	if( pGameChatInput )
		return pGameChatInput->iwm_esc();
	return false;
}

void BattleChatWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_CHAT_INPUT_WND:
		if( cmd == IOBN_BTNUP )
		{
			ChatMsgWnd *pChatWnd = dynamic_cast<ChatMsgWnd*>( FindChildWnd( ID_CHAT_MSG_WND ) );
			if( pChatWnd )
			{
				pChatWnd->InitScrollPos();
			}
		}
		break;
	}
}
//////////////////////////////////////////////////////////////////////////
BattleUserOptionWnd::BattleUserOptionWnd()
{
	m_pDarkLine	= NULL;
	m_pLightLine= NULL;
	m_pOverLine	= NULL;

	m_iAddWidth	= 0;
	m_iAddHeight= 0;
	m_iListXPos = 0;
	m_iListYPos = 0;
	m_iStartXPos= 0;
	m_iStartYPos= 0;
	m_iHeightGap= 0;
	m_iOverListIndex = -1;
	m_eTeamType = TEAM_BLUE;
	m_iSlotArray= 0;
}

BattleUserOptionWnd::~BattleUserOptionWnd()
{
	SAFEDELETE( m_pDarkLine	);
	SAFEDELETE( m_pLightLine );
	SAFEDELETE( m_pOverLine	);
}

void BattleUserOptionWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_iAddWidth	= xElement.GetIntAttribute_e( "AddWidth" );
	m_iAddHeight= xElement.GetIntAttribute_e( "AddHeight" );
	m_iListXPos = xElement.GetIntAttribute_e( "ListX" );
	m_iListYPos = xElement.GetIntAttribute_e( "ListY" );
	m_iHeightGap= xElement.GetIntAttribute_e( "HeightGap" );
}

void BattleUserOptionWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
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

void BattleUserOptionWnd::CommandOption( DWORD dwCmd )
{
	PartyShamBattleWnd *pParent = dynamic_cast<PartyShamBattleWnd*>( g_GUIMgr.FindWnd( PARTY_SHAM_BATTLE_WND ) );
	if( !pParent || !pParent->IsShow() )
		return;

	switch( dwCmd )
	{
	case OPTION_INFO:
		{
			g_App.OpenMannerWnd( m_szName.c_str(), true );
		}
		break;
	case OPTION_KICK:
		{
			if( g_BattleRoomMgr.GetOwnerName() != g_MyInfo.GetPublicID() )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );			
				return;
			}

			if( g_MyInfo.GetPublicID() == m_szName )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );			
				return;
			}

			if( g_App.IsAdminID( m_szName.c_str() ) )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );			
				return;
			}
			g_GUIMgr.SetMsgBox( MB_YESNO, this, STR(4), m_szName.c_str() );	
		}
		break;
	case OPTION_CLOSE:
		{
			if( g_BattleRoomMgr.GetOwnerName() != g_MyInfo.GetPublicID() )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(5) );			
				return;
			}

			int iMaxPlayerTeam = 0;
			int iMaxPlayer	   = m_iSlotArray - 1;
			if( m_eTeamType == TEAM_BLUE )
			{
				iMaxPlayerTeam = g_BattleRoomMgr.GetMaxPlayerBlue();
			}
			else if( m_eTeamType == TEAM_RED )
			{
				iMaxPlayerTeam = g_BattleRoomMgr.GetMaxPlayerRed();
			}

			if( iMaxPlayer > iMaxPlayerTeam )
				return;

			if( COMPARE( iMaxPlayer, 1, ( MAX_PARTY_PLAYER / 2 ) + 1 ) )
			{
				if( iMaxPlayer >= g_BattleRoomMgr.GetBattleTeamUserCount( m_eTeamType ) )
				{
					if( m_eTeamType == TEAM_BLUE )
					{
						if( !pParent->IsMaxPlayerLimit( iMaxPlayer + g_BattleRoomMgr.GetMaxPlayerRed() ) )
							return;
						pParent->SendMaxPlayer( iMaxPlayer, g_BattleRoomMgr.GetMaxPlayerRed(), g_BattleRoomMgr.GetMaxObserver() );	
					}
					else if( m_eTeamType == TEAM_RED )
					{
						if( !pParent->IsMaxPlayerLimit( iMaxPlayer + g_BattleRoomMgr.GetMaxPlayerBlue() ) )
							return;
						pParent->SendMaxPlayer( g_BattleRoomMgr.GetMaxPlayerBlue(), iMaxPlayer, g_BattleRoomMgr.GetMaxObserver() );	
					}				
				}
			}		
		}
		break;
	case OPTION_OPEN:
		{
			if( g_BattleRoomMgr.GetOwnerName() != g_MyInfo.GetPublicID() )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(6) );			
				return;
			}

			int iMaxPlayerTeam = 0;
			int iMaxPlayer     = m_iSlotArray;
			if( m_eTeamType == TEAM_BLUE )
			{
				iMaxPlayerTeam = g_BattleRoomMgr.GetMaxPlayerBlue();
			}
			else if( m_eTeamType == TEAM_RED )
			{
				iMaxPlayerTeam = g_BattleRoomMgr.GetMaxPlayerRed();
			}

			if( iMaxPlayer <= iMaxPlayerTeam )
				return;

			if( COMPARE( iMaxPlayer, 1, ( MAX_PARTY_PLAYER / 2 ) + 1 ) )
			{						
				if( iMaxPlayer >= g_BattleRoomMgr.GetBattleTeamUserCount( m_eTeamType ) )
				{
					if( m_eTeamType == TEAM_BLUE )
					{
						if( !pParent->IsMaxPlayerLimit( iMaxPlayer + g_BattleRoomMgr.GetMaxPlayerRed() ) )
							return;
						pParent->SendMaxPlayer( iMaxPlayer, g_BattleRoomMgr.GetMaxPlayerRed(), g_BattleRoomMgr.GetMaxObserver() );	
					}
					else if( m_eTeamType == TEAM_RED )
					{
						if( !pParent->IsMaxPlayerLimit( iMaxPlayer + g_BattleRoomMgr.GetMaxPlayerBlue() ) )
							return;
						pParent->SendMaxPlayer( g_BattleRoomMgr.GetMaxPlayerBlue(), iMaxPlayer, g_BattleRoomMgr.GetMaxObserver() );	
					}				
				}
			}	
		}
		break;
	case OPTION_CHANGE_TEAM:
		{			
			if( g_BattleRoomMgr.GetBattleUserTeam( g_MyInfo.GetPublicID() ) != m_eTeamType )
			{
				if( !pParent->IsModeStart() )
				{
					SP2Packet kPacket( CTPK_BATTLEROOM_COMMAND );
					kPacket << BATTLEROOM_TEAM_CHANGE << m_eTeamType;
					// 상대팀 대기 슬롯을 클릭하여 팀을 이동하겠다는 플래그 추가 전달하고 서버는 다른 클라(유저)들에게 다시 전달함
					//kPacket << bMoveOtherTeamSubSlot;
					TCPNetwork::SendToServer( kPacket );
					TCPNetwork::MouseBusy( true );
				}
			}			
		}
		break;
	}
}

void BattleUserOptionWnd::SetOptionInfo( const ioHashString &rkName, TeamType eTeam, int iSlotArray )
{
	HideWnd();
	/*
		-내가 방장일 경우
		  ㄴ 자기클릭 - 자기정보
      	  ㄴ 사람클릭 - 사람정보 / 강퇴
	      ㄴ 빈곳클릭 - 닫기 / 팀변경
		  ㄴ 엑스클릭 - 열기 or '해골영웅 1층(쉬움)은 2명까지만 함께 플레이가 가능합니다.' 팝업 띄우기

	    -내가 일반일 경우
		  ㄴ 자기클릭 - 자기정보
          ㄴ 사람클릭 - 사람정보
          ㄴ 빈곳클릭 - 팀변경
          ㄴ 엑스클릭 - 동작X - 팝업 띄우기

		-명칭
		  [누구누구 정보]
		  [강제퇴장 시키기]
		  [슬롯 닫기]
		  [레드(블루)팀으로 변경]
		  [슬롯 열기]

	    -옵션 명령이 하나밖에 없으면 즉시 실행한다.
	*/
	m_OptionMap.clear();

	m_szName	= rkName;
	m_eTeamType = eTeam;
	m_iSlotArray= iSlotArray;

	int iMaxPlayer = g_BattleRoomMgr.GetMaxPlayerBlue();
	if( m_eTeamType == TEAM_RED )
		iMaxPlayer = g_BattleRoomMgr.GetMaxPlayerRed();
	ModeType eModeType = (ModeType)g_ModeSelectInfo.GetBattleMode(g_BattleRoomMgr.GetSelectModeIndex());
	
	bool bFreeForAllMode = g_ModeSelectInfo.IsFreeForAllMode( g_BattleRoomMgr.GetSelectModeIndex() );
	// 방장
	if( g_BattleRoomMgr.GetOwnerName() == g_MyInfo.GetPublicID() )
	{
		if( rkName.IsEmpty() )
		{
			if( m_iSlotArray > iMaxPlayer && eModeType != MT_BATTLE )
			{
				m_OptionMap.insert( OptionMap::value_type( OPTION_OPEN, STR(1) ) );
			}
			else
			{
				// 닫기
				if( m_iSlotArray > 1 && eModeType != MT_BATTLE )
				{
					m_OptionMap.insert( OptionMap::value_type( OPTION_CLOSE, STR(2) ) );
				}
			
				if( ( !bFreeForAllMode && g_BattleRoomMgr.GetBattleUserTeam( g_MyInfo.GetPublicID() ) != m_eTeamType ) ||
					 g_BattleRoomMgr.IsObserverUser( g_MyInfo.GetPublicID() ) )
				{
					// 팀변경
					char szTitle[MAX_PATH] = "";
					if( g_BattleRoomMgr.IsObserverUser( g_MyInfo.GetPublicID() ) )
						SafeSprintf( szTitle, sizeof( szTitle ), STR(3) );
					else if( m_eTeamType == TEAM_BLUE )
						SafeSprintf( szTitle, sizeof( szTitle ), STR(4) );
					else
						SafeSprintf( szTitle, sizeof( szTitle ), STR(5) );
					m_OptionMap.insert( OptionMap::value_type( OPTION_CHANGE_TEAM, szTitle ) );
				}
			}
		}
		else if( rkName == g_MyInfo.GetPublicID() )
		{
			m_OptionMap.insert( OptionMap::value_type( OPTION_INFO, STR(6) ) );
		}
		else
		{
			char szTitle[MAX_PATH] = "";
			SafeSprintf( szTitle, sizeof( szTitle ), STR(7), m_szName.c_str() );
			m_OptionMap.insert( OptionMap::value_type( OPTION_INFO, szTitle ) );

			//강퇴
			if( !g_App.IsAdminID( m_szName.c_str() ) )
			{
				m_OptionMap.insert( OptionMap::value_type( OPTION_KICK, STR(8) ) );
			}
		}
	}
	else    //일반 - 즉시 실행되므로 타이틀에 신경쓰지 말자.
	{
		if( rkName.IsEmpty() )
		{	
			// 배틀모드는 팀 변경을 위한 여유 슬롯이 하나 더 있기 때문에...
			if( eModeType == MT_BATTLE )
				iMaxPlayer += 1;
			if( !bFreeForAllMode && m_iSlotArray <= iMaxPlayer && g_BattleRoomMgr.GetBattleUserTeam( g_MyInfo.GetPublicID() ) != m_eTeamType )
				m_OptionMap.insert( OptionMap::value_type( OPTION_CHANGE_TEAM, STR(9) ) );
			else if( m_iSlotArray <= iMaxPlayer && g_BattleRoomMgr.IsObserverUser( g_MyInfo.GetPublicID() ) )
				m_OptionMap.insert( OptionMap::value_type( OPTION_CHANGE_TEAM, STR(10) ) );
			else
				m_OptionMap.insert( OptionMap::value_type( OPTION_OPEN, STR(11) ) );
		}
		else
		{
			m_OptionMap.insert( OptionMap::value_type( OPTION_INFO, STR(12) ) );
		}
	}

	// 배틀 모드일때는 같은 팀이 아닐때만...
	if( m_OptionMap.empty() )
	{
		return;
	}

	if( m_OptionMap.size() == 1 )
	{
		//즉시 실행
		OptionMap::iterator iCreate = m_OptionMap.begin();
		if( GetParent() )
		{
			CommandOption( iCreate->first );
		}
	}
	else
	{
		// 윈도우 사이즈 결정
		int iListWidth = 0;
		OptionMap::iterator iter;
		for( iter = m_OptionMap.begin() ; iter != m_OptionMap.end() ; ++iter )
		{
			int iTextWidth = g_FontMgr.GetTextWidth( iter->second.c_str(), TS_NORMAL, FONT_SIZE_12 );
			if( iListWidth < iTextWidth )
				iListWidth = iTextWidth;
		}
		SetSize( iListWidth + m_iAddWidth + 10, (m_OptionMap.size() * m_iHeightGap) + m_iAddHeight );

		ioLSMouse *pMouse = g_App.GetMouse();
		if( pMouse && GetParent() )
		{
			int iOffsetX = GetParent()->GetXPos();
			int iOffsetY = GetParent()->GetYPos();
			if( pMouse->GetMousePos().x + GetWidth() > Setting::Width() )
				SetWndPos( ( pMouse->GetMousePos().x - GetWidth() ) - iOffsetX, pMouse->GetMousePos().y - iOffsetY );
			else
				SetWndPos( pMouse->GetMousePos().x - iOffsetX, pMouse->GetMousePos().y - iOffsetY );
		}
		ShowWnd();
	}
}

void BattleUserOptionWnd::iwm_show()
{
	SetCheckParentOutSide( true );
}

void BattleUserOptionWnd::iwm_hide()
{
	SetCheckParentOutSide( false );
}

void BattleUserOptionWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case MESSAGE_BOX:
		if( cmd == IOBN_BTNUP )
		{
			if( param == IDYES )
			{
				if( g_MyInfo.GetPublicID() == m_szName )
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );			
					return;
				}

				if( g_App.IsAdminID( m_szName.c_str() ) )
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );			
					return;
				}		

				if( g_BattleRoomMgr.GetOwnerName() == g_MyInfo.GetPublicID() )
				{
					SP2Packet kPacket( CTPK_MACRO_COMMAND );
					kPacket << MACRO_KICK_OUT << m_szName;
					TCPNetwork::SendToServer( kPacket );
				}
			}
		}
		break;
	}
}

void BattleUserOptionWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )	return;

	m_iOverListIndex = -1;

	// 다른 곳에서 클릭이 일어났는지 확인
	ioWnd *pPreClick = g_GUIMgr.GetPreClickWnd();
	if( pPreClick )
	{
		if( pPreClick != this )
		{
			HideWnd();
			return;
		}
	}

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	ioLSMouse *pMouse = g_App.GetMouse();

	OptionMap::iterator iter = m_OptionMap.begin();
	for(int iIndex = 0 ; iter != m_OptionMap.end() ; ++iter, ++iIndex )
	{
		RECT rcNameRect;
		rcNameRect.left = iXPos + m_iListXPos;
		rcNameRect.top  = iYPos + m_iListYPos + iIndex * m_iHeightGap;
		rcNameRect.right  = rcNameRect.left + ( GetWidth() - m_iAddWidth );
		rcNameRect.bottom = rcNameRect.top + m_iHeightGap;

		if( ::PtInRect( &rcNameRect, pMouse->GetMousePos() ) )
		{
			m_iOverListIndex  = iIndex;
			if( pMouse->IsLBtnUp() )
			{
				CommandOption( iter->first );
				HideWnd();
			}
			else if( pMouse->IsMouseShowing() )
				pMouse->SetToolTipID( m_szName, UserInfoToolTip::TT_GLOBAL );
			break;
		}
	}
}

void BattleUserOptionWnd::OnRender()
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
	OptionMap::iterator iter = m_OptionMap.begin();
	for( int iIndex = 0; iter != m_OptionMap.end() ; ++iter, ++iIndex )
	{
		bDarkRender = !bDarkRender;

		if( m_iOverListIndex == iIndex )
		{
			m_pOverLine->SetSize( GetWidth() - m_iAddWidth, 21 );
			m_pOverLine->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
			m_pOverLine->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
			m_pOverLine->Render( iXPos + m_iListXPos, iYPos + m_iListYPos + ( iIndex * m_iHeightGap ) );
			g_FontMgr.SetTextColor( 255, 255, 255 );
		}
		else if( bDarkRender )
		{
			m_pDarkLine->SetSize( GetWidth() - m_iAddWidth, 21 );
			m_pDarkLine->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
			m_pDarkLine->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
			m_pDarkLine->Render( iXPos + m_iListXPos, iYPos + m_iListYPos + ( iIndex * m_iHeightGap ) );
			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		}
		else
		{
			m_pLightLine->SetSize( GetWidth() - m_iAddWidth, 21 );
			m_pLightLine->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
			m_pLightLine->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
			m_pLightLine->Render( iXPos + m_iListXPos, iYPos + m_iListYPos + ( iIndex * m_iHeightGap ) );
			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		}

		g_FontMgr.PrintText( iXPos + m_iListXPos + 5, iYPos + m_iListYPos + ( iIndex * m_iHeightGap ) + 3, FONT_SIZE_12, iter->second.c_str() );
	}
}
//////////////////////////////////////////////////////////////////////////
PartyShamBattleWnd::PartyShamBattleWnd() : m_pPlayStage( NULL )
{
	m_bOwner		  = false;
	m_bFreeForAllMode = false;	
	m_bStartEnterXMode= false;
	m_mode_start_time = 0;
	m_mode_cur_sec	  = -1;

    m_pModeNMapGrayFrm = NULL;
	m_pTournamentStartGrayFrm = NULL;

	for( int i=0; i < MAX_WAR_COUNT; ++i )
	{
		m_pWarCount[i] = NULL;
	}

	m_pWarCountCircle = NULL;
	m_pManualMark     = NULL;

	m_dwChangeCountTime = 0;

	m_dwAutoStartCheckTime = 0;
	m_dwAutoStartCurrentTime = 0;
	m_iAutoStartSecond = -1;

	m_iDefaultX   = 0;
	m_iDefaultY   = 0;

	m_pModeHelpTooltip = NULL;
	m_bModeTooltipOver = false;

	m_pTournamentEntryArmFrm = NULL;

	m_SearchEntryTeamType = TEAM_NONE;
	m_bSearchEntry = false;
	m_bCheckSlotByMode = true;
	m_bBattleOrderControl = true;
	m_bDoNotSendPacket = false;
}

PartyShamBattleWnd::~PartyShamBattleWnd()
{
	SAFEDELETE( m_pModeNMapGrayFrm );
	SAFEDELETE( m_pTournamentStartGrayFrm );
	SAFEDELETE( m_pManualMark );
	SAFEDELETE( m_pTournamentEntryArmFrm );

	ClearCountImage();

	g_SoundMgr.StopStream();

	if( !COMPARE( m_iDefaultX, 0, Setting::Width() ) || 
		!COMPARE( m_iDefaultY, 0, Setting::Height() ) )
		return;

	g_GUIMgr.GUIPosSaveInt( "PartyShamBattleWnd", "XPos", m_iDefaultX ,POS_SAVE_VERSION );
	g_GUIMgr.GUIPosSaveInt( "PartyShamBattleWnd", "YPos", m_iDefaultY ,POS_SAVE_VERSION );
}

void PartyShamBattleWnd::ClearCountImage()
{
	for( int i=0; i < MAX_WAR_COUNT; ++i )
	{
		SAFEDELETE( m_pWarCount[i] );
	}

	SAFEDELETE( m_pWarCountCircle );
}

void PartyShamBattleWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_szBackOgg = xElement.GetStringAttribute_e( "BackOGG" );

	m_dwAutoStartCheckTime = xElement.GetIntAttribute_e( "AutoStartCheckTime" );
}

void PartyShamBattleWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "ModeNMapGray" )
	{
		SAFEDELETE( m_pModeNMapGrayFrm );
		m_pModeNMapGrayFrm = pFrame;
	}
	else if( szType == "TournamentStartGray" )
	{
		SAFEDELETE( m_pTournamentStartGrayFrm );
		m_pTournamentStartGrayFrm = pFrame;
	}
	else if( szType == "TournamentEntryArmFrm" )
	{
		SAFEDELETE( m_pTournamentEntryArmFrm );
		m_pTournamentEntryArmFrm = pFrame;
	}
	else
	{
		ioWnd::AddRenderFrame( szType, pFrame );
	}
}

void PartyShamBattleWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "count_5" )
	{
		SAFEDELETE( m_pWarCount[5] );
		m_pWarCount[5] = pImage;
	}
	else if( szType == "count_4" )
	{
		SAFEDELETE( m_pWarCount[4] );
		m_pWarCount[4] = pImage;
	}
	else if( szType == "count_3" )
	{
		SAFEDELETE( m_pWarCount[3] );
		m_pWarCount[3] = pImage;
	}
	else if( szType == "count_2" )
	{
		SAFEDELETE( m_pWarCount[2] );
		m_pWarCount[2] = pImage;
	}
	else if( szType == "count_1" )
	{
		SAFEDELETE( m_pWarCount[1] );
		m_pWarCount[1] = pImage;
	}
	else if( szType == "count_0" )
	{
		SAFEDELETE( m_pWarCount[0] );
		m_pWarCount[0] = pImage;
	}
	else if( szType == "count_circle" )
	{
		SAFEDELETE( m_pWarCountCircle );
		m_pWarCountCircle = pImage;
	}
	else if( szType == "ManualMark" )
	{
		SAFEDELETE( m_pManualMark );
		m_pManualMark = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}

void PartyShamBattleWnd::iwm_create()
{
	g_GUIMgr.AddWnd( "XML/battlechatwnd.xml", this );

	int iXPos = g_GUIMgr.GUIPosLoadInt( "PartyShamBattleWnd", "XPos", GetXPos() ,POS_SAVE_VERSION );
	int iYPos = g_GUIMgr.GUIPosLoadInt( "PartyShamBattleWnd", "YPos", GetYPos() ,POS_SAVE_VERSION );

	if( COMPARE( iXPos, 0, Setting::Width() ) && COMPARE( iYPos, 0, Setting::Height() ) )
	{
		SetWndPos( iXPos, iYPos );
	}
	m_iDefaultX = GetXPos();
	m_iDefaultY = GetYPos();
}

void PartyShamBattleWnd::SetPlayStage( ioPlayStage *pPlayStage )
{
	BattleChatWnd *pBattleChat = dynamic_cast<BattleChatWnd*>( FindChildWnd( ID_BATTLE_CHAT_WND ) );
	if( pBattleChat )
		pBattleChat->SetPlayStage( pPlayStage );
	m_pPlayStage = pPlayStage;
}

void PartyShamBattleWnd::InitAutoStart()
{
	m_dwAutoStartCurrentTime = FRAMEGETTIME();
	m_iAutoStartSecond       = -1;
	m_bTournamentStart		 = false;
	m_bTournamentStartLog	 = false;
}

void PartyShamBattleWnd::iwm_show()
{
	m_dwChangeCountTime = 0;
	
	{   // 결과 종료시 나오면 안되는 UI
		g_GUIMgr.HideWnd( TOTAL_RESULT_MAIN_WND );
		g_GUIMgr.HideWnd( MINIMAP_WND );
		g_GUIMgr.HideWnd( TIME_SCORE_WND );
		g_GUIMgr.HideWnd( SKILL_LIST_WND );
		g_GUIMgr.HideWnd( ACCESSORY_SKILL_LIST_WND );
		g_GUIMgr.HideWnd( CENTER_INFO_WND );
		g_GUIMgr.HideWnd( MONSTER_BOSS_GAUGE_WND );
		g_GUIMgr.HideWnd( MODE_RANK_WND );
		g_GUIMgr.HideWnd( GLOBAL_CHAT_WND );
		g_GUIMgr.HideWnd( KO_CHAT_MSG_WND );
		g_GUIMgr.HideWnd( FIGHTCLUB_TIME_GAUGE_WND );
		g_GUIMgr.HideWnd( FIGHTCLUB_ROUND_END_WND );
		g_GUIMgr.HideWnd( FIGHTCLUB_CHALLENGER_WND );
		g_GUIMgr.HideWnd( TOWERDEF_USER_GAUGE_WND );
		g_GUIMgr.HideWnd( MINI_SOLDIER_SELECT_WND );
		g_GUIMgr.HideWnd( TUTORIAL_KEYBOARD_WND );
		g_GUIMgr.HideWnd( FIGHTCLUB_CHALLENGER_WND );
		g_GUIMgr.HideWnd( FIGHTCLUB_CONTINUE_WND );
		g_GUIMgr.HideWnd( TRAINING_FUNC_MENU );
		g_GUIMgr.HideWnd( BATTLE_MODE_TIME_GAUGE_WND );
		g_GUIMgr.HideWnd( BATTLE_MODE_FUNCTION_WND );
		g_GUIMgr.HideWnd( BATTLE_MODE_BLUE_USER_INFO_WND );
		g_GUIMgr.HideWnd( BATTLE_MODE_RED_USER_INFO_WND );
		g_ModeHelpMgr.HideHelpDirectly();
	}

	ShowChildWnd( ID_BATTLE_CHAT_WND );

	HideChildWnd( ID_MODE_MAP_LIST );
	ShowChildWnd( ID_BATTLE_START );
	HideChildWnd( ID_BATTLE_END );	

	SetChildActive( ID_CHANGE_MODE );
	SetChildActive( ID_CHANGE_MAP );

	ChangeModeType();

	UpdateShamBattle();

	g_BackgroundMgr.SetBackground( BGT_BATTLE );

	if( !m_szBackOgg.IsEmpty() )
	{
		g_SoundMgr.StopStream();

		char szName[MAX_PATH];
		sprintf( szName, "Resource/wave/%s", m_szBackOgg.c_str() );
		g_SoundMgr.OpenOggStream( szName );
		g_SoundMgr.PlayStream( true );
	}

	if( g_BattleRoomMgr.ChangeMeHostAlarm() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
	}	

	InitAutoStart();
	g_GUIMgr.ShowReserveMsgBox();
}

void PartyShamBattleWnd::iwm_hide()
{
	HideChildWnd( ID_BATTLE_CHAT_WND );
	g_GUIMgr.HideWnd( PARTY_INVITE_LIST_WND );
	
	g_GUIMgr.HideWnd( PARTY_OPTION_WND );

	ModeHelpTooltip *pModeHelpTooltip = dynamic_cast< ModeHelpTooltip * >( m_pModeHelpTooltip );	
	if( pModeHelpTooltip )
	{
		pModeHelpTooltip->SetModeHelpTooltip( NULL, "", -1 );
	}
}

bool PartyShamBattleWnd::iwm_esc()
{
	BattleChatWnd *pBattleChat = dynamic_cast<BattleChatWnd*>( FindChildWnd( ID_BATTLE_CHAT_WND ) );
	if( pBattleChat && pBattleChat->IsShow() )
		return pBattleChat->iwm_esc();
	return false;
}

void PartyShamBattleWnd::SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate )
{
	ioMovingWnd::SetPositionToDefault( iParentWidth, iParentHeight, bChildMove, bCreate );

	OnChangeAILevel();

	if( !bCreate )
	{
		m_iDefaultX = GetXPos();
		m_iDefaultY = GetYPos();
	}
}

void PartyShamBattleWnd::OnChangeTeam()
{
	if( g_BattleRoomMgr.IsObserverUser( g_MyInfo.GetPublicID() ) )
	{
		if( m_mode_start_time == 0 )
		{
			// 블루팀 & 레드팀중 인원이 적은팀으로 이동
			TeamType eChangeTeam = TEAM_NONE;
			int iEmptyBlueUserCnt = max( 0, g_BattleRoomMgr.GetMaxPlayerBlue() - g_BattleRoomMgr.GetBattleTeamUserCount( TEAM_BLUE ) );
			int iEmptyRedUserCnt  = max( 0, g_BattleRoomMgr.GetMaxPlayerRed() - g_BattleRoomMgr.GetBattleTeamUserCount( TEAM_RED ) );
			if( iEmptyBlueUserCnt + iEmptyRedUserCnt > 0 )
			{
				if( iEmptyBlueUserCnt >= iEmptyRedUserCnt )
					eChangeTeam = TEAM_BLUE;
				else
					eChangeTeam = TEAM_RED;
			}

			if( eChangeTeam == TEAM_NONE )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
			}
			else
			{
				SP2Packet kPacket( CTPK_BATTLEROOM_COMMAND );
				kPacket << BATTLEROOM_TEAM_CHANGE << eChangeTeam;
				TCPNetwork::SendToServer( kPacket );
				TCPNetwork::MouseBusy( true );
			}
		}
		else
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
	}
	else if( g_BattleRoomMgr.GetBattleUserTeam( g_MyInfo.GetPublicID() ) != TEAM_BLUE )
	{
		if( m_mode_start_time == 0 )
		{
			SP2Packet kPacket( CTPK_BATTLEROOM_COMMAND );
			kPacket << BATTLEROOM_TEAM_CHANGE << TEAM_BLUE;
			TCPNetwork::SendToServer( kPacket );
			TCPNetwork::MouseBusy( true );
		}
		else
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
	}					
	else if( g_BattleRoomMgr.GetBattleUserTeam( g_MyInfo.GetPublicID() ) != TEAM_RED )
	{
		if( m_mode_start_time == 0 )
		{
			SP2Packet kPacket( CTPK_BATTLEROOM_COMMAND );
			kPacket << BATTLEROOM_TEAM_CHANGE << TEAM_RED;
			TCPNetwork::SendToServer( kPacket );
			TCPNetwork::MouseBusy( true );
		}
		else
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4) );
	}
}

void PartyShamBattleWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_BATTLE_INVITE:
		if( cmd == IOBN_BTNUP || cmd == IOEX_BTNUP )
		{
			if( g_BattleRoomMgr.IsSafetyLevelRoom() )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
			}
			else if( !g_BattleRoomMgr.IsBroadcastModeType() && g_BattleRoomMgr.IsObserverUser( g_MyInfo.GetPublicID() ) )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
			}
			else if( g_BattleRoomMgr.IsBattleModePlaying() && g_BattleRoomMgr.IsStartRoomEnterX() )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
			}
			else if( g_BattleRoomMgr.IsBroadcastModeType() && g_MyInfo.GetUserEventType() != USER_TYPE_BROADCAST_MBC )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(38) );
			}
			else if( g_BattleRoomMgr.IsTournamentModeType() )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "대회방에는 초대가 되지 않습니다." );
			}
			else if( g_RankBattleMgr.IsRankBattlePlaying() )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "랭킹전 중에는 초대가 되지 않습니다." );
			}
			else
			{
				if( g_GUIMgr.IsShow( PARTY_INVITE_LIST_WND ) )
					g_GUIMgr.HideWnd( PARTY_INVITE_LIST_WND );
				else
					g_GUIMgr.ShowWnd( PARTY_INVITE_LIST_WND );
			}
		}
		break;
	case ID_BLUE_USER_1:
	case ID_BLUE_USER_2:
	case ID_BLUE_USER_3:
	case ID_BLUE_USER_4:
	case ID_BLUE_USER_5:
	case ID_BLUE_USER_6:
	case ID_BLUE_USER_7:
	case ID_BLUE_USER_8:
	case ID_BLUE_BATTLE_USER_1:
	case ID_BLUE_BATTLE_USER_2:
	case ID_BLUE_BATTLE_USER_3:
	case ID_BLUE_BATTLE_USER_4:
	case ID_RED_USER_1:
	case ID_RED_USER_2:
	case ID_RED_USER_3:
	case ID_RED_USER_4:
	case ID_RED_USER_5:
	case ID_RED_USER_6:
	case ID_RED_USER_7:
	case ID_RED_USER_8:
	case ID_RED_BATTLE_USER_1:
	case ID_RED_BATTLE_USER_2:
	case ID_RED_BATTLE_USER_3:
	case ID_RED_BATTLE_USER_4:
		if( cmd == IOBN_BTNUP )
		{
			PartyMemberList *pSlotWnd = dynamic_cast<PartyMemberList*>(pWnd);
			if( pSlotWnd )
			{
				if( g_BattleRoomMgr.IsTournamentModeType() )
				{
					if( !pSlotWnd->GetListName().IsEmpty() )
						g_App.OpenMannerWnd( pSlotWnd->GetListName().c_str(), true );
				}
				else
				{
					//ai면 스킵
					bool bAIMode = g_ModeSelectInfo.IsAIMode( g_BattleRoomMgr.GetSelectModeIndex() );
					if( bAIMode )
					{
						if( pSlotWnd->GetListName() == "AI" )
							return;

						BattleUserOptionWnd *pBattleUserOption = dynamic_cast<BattleUserOptionWnd*>( FindChildWnd( ID_USER_SLOT_OPTION ) );
						if( pBattleUserOption )
							pBattleUserOption->SetOptionInfo( pSlotWnd->GetListName(), pSlotWnd->GetTeam(), pSlotWnd->GetListArray() );
					}
					else
					{
						BattleUserOptionWnd *pBattleUserOption = dynamic_cast<BattleUserOptionWnd*>( FindChildWnd( ID_USER_SLOT_OPTION ) );
						if( pBattleUserOption )
							pBattleUserOption->SetOptionInfo( pSlotWnd->GetListName(), pSlotWnd->GetTeam(), pSlotWnd->GetListArray() );
					}
				}
			}
		}
		break;
	case ID_CHANGE_INFO:
		if( cmd == IOBN_BTNUP || cmd == IOEX_BTNUP )
		{
			if( g_BattleRoomMgr.GetOwnerName() != g_MyInfo.GetPublicID() )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4) );
				return;
			}
			g_GUIMgr.ShowWnd( PARTY_OWNER_WND );
		}
		break;
	case ID_RANDOM_TEAM:
		if( cmd == IOBN_BTNUP )
		{
			if( g_BattleRoomMgr.GetOwnerName() == g_MyInfo.GetPublicID() )
			{
				if( m_mode_start_time == 0 )
				{
					g_BattleRoomMgr.SetRandomTeamMode( !g_BattleRoomMgr.IsRandomTeamMode() );
					
					g_BattleRoomMgr.SendBattleRoomOption();

					FixBattleModeOrderByRandomTeamOpt();
				}				
				else
					g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(5) );
			}
			else
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(6) );
			}
			UpdateShamBattle();
		}
		break;
	case ID_START_ENTER_X:
		if( cmd == IOBN_BTNUP )
		{
            if( g_BattleRoomMgr.GetOwnerName() == g_MyInfo.GetPublicID() )
			{
				if( m_mode_start_time == 0 )
				{
					if( m_bStartEnterXMode )
						g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(16) );
					else
					{
						g_BattleRoomMgr.SetStartRoomEnterX( !g_BattleRoomMgr.IsStartRoomEnterX() );

						g_BattleRoomMgr.SendBattleRoomOption();
					}
				}
				else
					g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(7) );
			}
			else
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(8) );
			}
			UpdateShamBattle();
		}
		break;
	case ID_NO_CHALLENGER:
		if( cmd == IOBN_BTNUP )
		{
            if( g_BattleRoomMgr.GetOwnerName() == g_MyInfo.GetPublicID() )
			{
				g_BattleRoomMgr.SetNoChallenger( !g_BattleRoomMgr.IsNoChallenger() );
				g_BattleRoomMgr.SendBattleRoomOption();
			}
			else
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(8) );
			}
			UpdateShamBattle();
		}
		break;
	case ID_AUTO_START:
		if( cmd == IOBN_BTNUP )
		{
			if( g_BattleRoomMgr.GetOwnerName() == g_MyInfo.GetPublicID() )
			{
				g_BattleRoomMgr.SetAutoModeStart( !g_BattleRoomMgr.IsAutoModeStart() );
				g_BattleRoomMgr.SendBattleRoomOption();
			}
			else
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(10) );
			}
			UpdateShamBattle();
		}
		break;
	case ID_BAD_PING_KICK:
		if( cmd == IOBN_BTNUP )
		{
			if( g_BattleRoomMgr.GetOwnerName() == g_MyInfo.GetPublicID() )
			{
				g_BattleRoomMgr.SetBadPingKick( !g_BattleRoomMgr.IsBadPingKick() );
				g_BattleRoomMgr.SendBattleRoomOption();
			}
			else
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(10) );
			}
			UpdateShamBattle();
		}
		break;
	case ID_CHANGE_MODE:
		if( cmd == IOBN_BTNUP )
		{
		  	ModeMapSelectList *pModeList = dynamic_cast<ModeMapSelectList *>(FindChildWnd( ID_MODE_MAP_LIST ));
			if( pModeList )
			{
				pModeList->OpenList( pWnd, true, g_BattleRoomMgr.GetSelectModeIndex(), g_BattleRoomMgr.IsSafetyLevelRoom(), pWnd->GetXPos(), pWnd->GetYPos() - 2 );

				if( g_BattleRoomMgr.IsBroadcastModeType() )
					g_ModeSelectInfo.GetBroadcastBattleModeUIList( pModeList );
				else
					g_ModeSelectInfo.GetBattleModeUIList( pModeList );
			}
		}
		else if( cmd == ID_MODE_MAP_LIST )
		{
			int iPrevModeIndex = g_BattleRoomMgr.GetSelectModeIndex();
			int iPrevMapIndex  = g_BattleRoomMgr.GetSelectMapIndex();

			int iModeIndex = g_ModeSelectInfo.CheckBattleModeIndex( (int)HIWORD( param ) );
			bool bSafetyLevelRoom = ( ( LOWORD( param ) == 0 ) ? false : true ) ;

			if( iModeIndex == g_BattleRoomMgr.GetSelectModeIndex() && bSafetyLevelRoom == g_BattleRoomMgr.IsSafetyLevelRoom() )
				return;

			if( bSafetyLevelRoom != g_BattleRoomMgr.IsSafetyLevelRoom() )
			{
				if( bSafetyLevelRoom && g_GUIMgr.IsShow( PARTY_INVITE_LIST_WND ) )
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(11) );
				}
				g_GUIMgr.HideWnd( PARTY_INVITE_LIST_WND );
			}

			// 모드가 변경되면 맵은 항상 첫번째 맵 리스트가 시작이다.
			g_BattleRoomMgr.SetSelectModeNMap( iModeIndex, g_ModeSelectInfo.CheckBattleFirstMapIndex( iModeIndex ), bSafetyLevelRoom ); 

			SP2Packet kPacket( CTPK_BATTLEROOM_COMMAND );
			kPacket << BATTLEROOM_MODE_SEL << g_BattleRoomMgr.GetSelectModeIndex() << g_BattleRoomMgr.GetSelectMapIndex() << g_BattleRoomMgr.IsSafetyLevelRoom();
			TCPNetwork::SendToServer( kPacket );

			ChangeModeType( true );
			UpdateShamBattle();

			if( IsChangeMaxPlayerLimit( iPrevModeIndex, iPrevMapIndex ) )
				SetMaxPlayerLimit();
			// 모드가 배틀 모드로 변경되면 일부 슬롯을 열고 닫는다.
			SwitchUserSlot();
		}
		break;
	case ID_CHANGE_MAP:
		if( cmd == IOBN_BTNUP )
		{
			if( g_BattleRoomMgr.GetSelectModeIndex() == -1 )
				return;             //모드가 랜덤이면 맵은 변경 불가

		  	ModeMapSelectList *pMapList = dynamic_cast<ModeMapSelectList *>(FindChildWnd( ID_MODE_MAP_LIST ));
			if( pMapList )
			{
				bool bAIMode = g_ModeSelectInfo.IsAIMode( g_BattleRoomMgr.GetSelectModeIndex() );
				pMapList->OpenList( pWnd, false, g_BattleRoomMgr.GetSelectMapIndex(), false, pWnd->GetXPos(), pWnd->GetYPos() - 2, bAIMode );
				g_ModeSelectInfo.GetBattleMapUIList( pMapList, g_BattleRoomMgr.GetSelectModeIndex() );
			}
		}
		else if( cmd == ID_MODE_MAP_LIST )
		{
			int iPrevModeIndex = g_BattleRoomMgr.GetSelectModeIndex();
			int iPrevMapIndex  = g_BattleRoomMgr.GetSelectMapIndex();

			int iMapIndex = g_ModeSelectInfo.CheckBattleMapIndex( g_BattleRoomMgr.GetSelectModeIndex(), (int)HIWORD( param ) );
			if( iMapIndex == g_BattleRoomMgr.GetSelectMapIndex() )
				return;

			g_BattleRoomMgr.SetSelectModeNMap( g_BattleRoomMgr.GetSelectModeIndex(), iMapIndex, g_BattleRoomMgr.IsSafetyLevelRoom() );
			int iMode = g_BattleRoomMgr.GetSelectModeIndex();
			int iMap = g_BattleRoomMgr.GetSelectMapIndex();
			SP2Packet kPacket( CTPK_BATTLEROOM_COMMAND );
			kPacket << BATTLEROOM_MODE_SEL << g_BattleRoomMgr.GetSelectModeIndex() << g_BattleRoomMgr.GetSelectMapIndex() << g_BattleRoomMgr.IsSafetyLevelRoom();
			TCPNetwork::SendToServer( kPacket );

			ChangeModeType();

/*			if( g_BattleRoomMgr.GetSelectMapIndex() != -1 && g_BattleRoomMgr.IsAutoModeStart() )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(12) );

				g_BattleRoomMgr.SetAutoModeStart( false );

				g_BattleRoomMgr.SendBattleRoomOption();

				UpdateShamBattle();
			}
*/
			if( IsChangeMaxPlayerLimit( iPrevModeIndex, iPrevMapIndex ) )
				SetMaxPlayerLimit();
		}
		break;
	case ID_CHANGE_AI_LEVEL:
		{
			if( cmd == IOBN_BTNUP )
			{
				if( g_BattleRoomMgr.GetSelectModeIndex() == -1 )
					return;             //모드가 랜덤이면 맵은 변경 불가

				ModeMapSelectList *pMapList = dynamic_cast<ModeMapSelectList *>(FindChildWnd( ID_MODE_MAP_LIST ));
				if( pMapList )
				{
					pMapList->OpenList( pWnd, false, g_BattleRoomMgr.GetSelectMapIndex(), false, pWnd->GetXPos() -97, pWnd->GetYPos() - 2, true );
					g_ModeSelectInfo.GetBattleAIUIList( pMapList );
				}
			}
			else if ( cmd == ID_MODE_MAP_LIST )
			{
				int iAiLevel = (int)HIWORD( param );				
				g_BattleRoomMgr.SetAiLevel( iAiLevel );
				SendAILevel( iAiLevel );
			}
		}
		break;
	case ID_BATTLE_START:
		if( cmd == IOBN_BTNUP )
		{
			// 레벨 제한 체크
			int iMapLimitGradeLevel = g_ModeSelectInfo.GetBattleMapLimitGrade( g_BattleRoomMgr.GetSelectModeIndex(), g_BattleRoomMgr.GetSelectMapIndex() );
			if( iMapLimitGradeLevel != -1 )
			{
				if( !g_BattleRoomMgr.IsAllUserHighGradeLevel( iMapLimitGradeLevel ) )
				{
					ioHashString szModeName = g_ModeSelectInfo.GetBattleModeTitle( g_BattleRoomMgr.GetSelectModeIndex() );
					ioHashString szMapName  = g_ModeSelectInfo.GetBattleMapTitle( g_BattleRoomMgr.GetSelectModeIndex(), g_BattleRoomMgr.GetSelectMapIndex() );
					char szGradeName[MAX_PATH] = "";
					g_LevelMgr.GetGradeName( iMapLimitGradeLevel, szGradeName, MAX_PATH, true, true );

					ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
					kPrinter[0].SetTextStyle( TS_NORMAL );
					kPrinter[0].SetBkColor( 0, 0, 0 );
					kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
					kPrinter[0].AddTextPiece( FONT_SIZE_13,STR(17), szModeName.c_str(), szMapName.c_str() );
					kPrinter[1].SetTextStyle( TS_NORMAL );
					kPrinter[1].SetBkColor( 0, 0, 0 );
					kPrinter[1].SetTextColor( TCT_DEFAULT_RED );
					kPrinter[1].AddTextPiece( FONT_SIZE_13, szGradeName );
					kPrinter[1].SetTextColor( TCT_DEFAULT_DARKGRAY );
					kPrinter[1].AddTextPiece( FONT_SIZE_13,STR(18) );
					g_GUIMgr.SetPrevMsgListBox( NULL, MB_OK, NULL, kPrinter );

					char szChat[MAX_PATH] = "";
					SafeSprintf( szChat, sizeof(szChat), STR(19), szModeName.c_str(), szMapName.c_str(), szGradeName );
					g_BattleRoomMgr.SendBattleRoomAnn( szChat );
					return;
				}
			}

			int nEnableLevel = g_RoomOptionMgr.GetEnableLevel();

			if( (g_BattleRoomMgr.GetSelectModeIndex() == MT_UNDERWEAR || g_BattleRoomMgr.GetSelectModeIndex() == MT_CBT || g_BattleRoomMgr.GetSelectModeIndex() == MT_FARMING ) )
				nEnableLevel = -1;

			// 가능 레벨 체크(참여자)
			if( g_BattleRoomMgr.IsUseExtraOption() && nEnableLevel > g_MyInfo.GetGradeLevel() )
			{
				char szGradeName[MAX_PATH] = "";
				int iExtraOptionLevel = g_RoomOptionMgr.GetEnableLevel();

				g_LevelMgr.GetGradeName( iExtraOptionLevel, szGradeName, MAX_PATH, true, true );

				ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
				kPrinter[0].SetTextStyle( TS_NORMAL );
				kPrinter[0].SetBkColor( 0, 0, 0 );

				kPrinter[0].SetTextColor( TCT_DEFAULT_GRAY );
				kPrinter[0].AddTextPiece( FONT_SIZE_15, STR(26), g_ModeSelectInfo.GetCustomModeTitle().c_str(), szGradeName );

				g_GUIMgr.SetPrevMsgListBox( NULL, MB_OK, NULL, kPrinter );
				return;
			}

			// 최대 인원 체크
			int iMapLimitMaxPlayer = g_ModeSelectInfo.GetBattleMapLimitMaxPlayer( g_BattleRoomMgr.GetSelectModeIndex(), g_BattleRoomMgr.GetSelectMapIndex() );
			if( iMapLimitMaxPlayer < MAX_PLAYER )
			{
				if( g_ModeSelectInfo.IsBattleStartSetMapLimitMaxPlayer( g_BattleRoomMgr.GetSelectModeIndex(), g_BattleRoomMgr.GetSelectMapIndex() ) )
				{
					// 배틀모드이면 현재 유저 인원과 모드 최대 인원이 같아야해서 체크해야됨
					ModeType eModeType = (ModeType)g_ModeSelectInfo.GetBattleMode(g_BattleRoomMgr.GetSelectModeIndex());	
					if(eModeType != MT_BATTLE)
						SetMaxPlayerLimit();
					if( g_BattleRoomMgr.GetCurPlayer() > iMapLimitMaxPlayer || ( IsUserCountFailed((iMapLimitMaxPlayer/2)) && eModeType == MT_BATTLE ) )
					{
						ioHashString szModeName = g_ModeSelectInfo.GetBattleModeTitle( g_BattleRoomMgr.GetSelectModeIndex() );
						ioHashString szMapName  = g_ModeSelectInfo.GetBattleMapTitle( g_BattleRoomMgr.GetSelectModeIndex(), g_BattleRoomMgr.GetSelectMapIndex() );
						g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(39), szModeName.c_str(), szMapName.c_str(), iMapLimitMaxPlayer );

						char szChat[MAX_PATH] = "";
						SafeSprintf( szChat, sizeof( szChat), STR(21), szMapName.c_str(), iMapLimitMaxPlayer );
						g_BattleRoomMgr.SendBattleRoomAnn( szChat );
						return;
					}
				}
				else
				{
					if( g_BattleRoomMgr.GetMaxPlayer() > iMapLimitMaxPlayer )
					{
						ioHashString szModeName = g_ModeSelectInfo.GetBattleModeTitle( g_BattleRoomMgr.GetSelectModeIndex() );
						ioHashString szMapName  = g_ModeSelectInfo.GetBattleMapTitle( g_BattleRoomMgr.GetSelectModeIndex(), g_BattleRoomMgr.GetSelectMapIndex() );
						g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(40), szModeName.c_str(), szMapName.c_str(), iMapLimitMaxPlayer );
						return;
					}
				}
			}

			// 코인 체크 여부
			int iUseCoinCnt = g_ModeSelectInfo.GetBattleMapCoinUseCnt( g_BattleRoomMgr.GetSelectModeIndex(), g_BattleRoomMgr.GetSelectMapIndex() );
			if( iUseCoinCnt > 0 )
			{
				ModeType eModeType = (ModeType)g_ModeSelectInfo.GetBattleMode(g_BattleRoomMgr.GetSelectModeIndex());
				if (eModeType == MT_RAID)
				{
					if( g_MyInfo.GetEtcRaidCoin() < iUseCoinCnt )
					{
						MonsterCoinUseWnd *pMonsterCoinWnd = dynamic_cast<MonsterCoinUseWnd*>( g_GUIMgr.FindWnd( MONSTER_COIN_USE_WND ) );
						if( pMonsterCoinWnd )
						{
							char szMent[MAX_PATH] = "";                                        
							sprintf( szMent, "%s - %s", g_ModeSelectInfo.GetBattleModeTitle( g_BattleRoomMgr.GetSelectModeIndex() ).c_str(), 
								g_ModeSelectInfo.GetBattleMapTitle( g_BattleRoomMgr.GetSelectModeIndex(), g_BattleRoomMgr.GetSelectMapIndex() ).c_str() );
							pMonsterCoinWnd->ShowPreStartCoinLack( szMent, iUseCoinCnt, eModeType );
							return;
						}

						//UJ 140120, 보안강화: 몬스터 코인 없이 계속 플레이할 수 있는 치팅을 막는다
						return;
					}
					else if( g_BattleRoomMgr.IsTeamNoneRaidCoinCheck( iUseCoinCnt ) )
					{
						ioHashStringVec vUserIDVec;
						g_BattleRoomMgr.GetNeedRaidCoinUserIDVec( iUseCoinCnt, vUserIDVec );

						int iSize = vUserIDVec.size();

						ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
						kPrinter[0].SetTextStyle( TS_NORMAL );
						kPrinter[0].SetBkColor( 0, 0, 0 );	
						kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
						kPrinter[0].AddTextPiece( FONT_SIZE_13,"티켓이 없는 유저가 ");
						kPrinter[0].SetTextColor( TCT_DEFAULT_GRAY );
						kPrinter[0].AddTextPiece( FONT_SIZE_13,"%d명", iSize );
						kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
						kPrinter[0].AddTextPiece( FONT_SIZE_13,STR(22));

						char szTemp[MAX_PATH]="";
						for (int i = 0; i < iSize ; i++)
						{
							StringCbCat( szTemp, sizeof( szTemp ), vUserIDVec[i].c_str() );
							if( i < iSize - 1 )
								StringCbCat( szTemp, sizeof( szTemp ), " / " );
						}
						enum { WIDTH_SIZE = 215, };
						ioHashString sCutText;
						if( g_FontMgr.GetTextWidth( szTemp, TS_NORMAL, FONT_SIZE_13 ) > WIDTH_SIZE )
							Help::TextCut( szTemp, WIDTH_SIZE, TS_NORMAL, FONT_SIZE_13, sCutText );
						else
							sCutText = szTemp;

						kPrinter[1].SetTextStyle( TS_NORMAL );
						kPrinter[1].SetBkColor( 0, 0, 0 );	
						kPrinter[1].SetTextColor( TCT_DEFAULT_GRAY );	
						kPrinter[1].AddTextPiece( FONT_SIZE_13, sCutText.c_str() );
						kPrinter[2].SetTextStyle( TS_NORMAL );
						kPrinter[2].SetBkColor( 0, 0, 0 );	
						kPrinter[2].SetTextColor( TCT_DEFAULT_DARKGRAY );
						kPrinter[2].AddTextPiece( FONT_SIZE_13,"레이드를 플레이하려면 각자");
						kPrinter[3].SetTextStyle( TS_NORMAL );
						kPrinter[3].SetBkColor( 0, 0, 0 );	
						kPrinter[3].SetTextColor( TCT_DEFAULT_DARKGRAY );
						kPrinter[3].AddTextPiece( FONT_SIZE_13,"레이드 티켓이 %d개씩 필요합니다.", iUseCoinCnt );

						g_GUIMgr.SetPrevMsgListBox( NULL, MB_OK, NULL, kPrinter );
						return;
					}
				}
				else
				{
					if( g_MyInfo.GetEtcMonsterCoin() + g_MyInfo.GetEtcGoldMonsterCoin() < iUseCoinCnt )
					{
						MonsterCoinUseWnd *pMonsterCoinWnd = dynamic_cast<MonsterCoinUseWnd*>( g_GUIMgr.FindWnd( MONSTER_COIN_USE_WND ) );
						if( pMonsterCoinWnd )
						{
							char szMent[MAX_PATH] = "";                                        
							sprintf( szMent, "%s - %s", g_ModeSelectInfo.GetBattleModeTitle( g_BattleRoomMgr.GetSelectModeIndex() ).c_str(), 
								g_ModeSelectInfo.GetBattleMapTitle( g_BattleRoomMgr.GetSelectModeIndex(), g_BattleRoomMgr.GetSelectMapIndex() ).c_str() );
							pMonsterCoinWnd->ShowPreStartCoinLack( szMent, iUseCoinCnt, eModeType );
							return;
						}

						//UJ 140120, 보안강화: 몬스터 코인 없이 계속 플레이할 수 있는 치팅을 막는다
						return;
					}
					else if( g_BattleRoomMgr.IsTeamNoneMonsterCoinCheck( iUseCoinCnt ) )
					{
						ioHashStringVec vUserIDVec;
						g_BattleRoomMgr.GetNeedMonsterCoinUserIDVec( iUseCoinCnt, vUserIDVec );

						int iSize = vUserIDVec.size();

						ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
						kPrinter[0].SetTextStyle( TS_NORMAL );
						kPrinter[0].SetBkColor( 0, 0, 0 );	
						kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
						kPrinter[0].AddTextPiece( FONT_SIZE_13,STR(14));
						kPrinter[0].SetTextColor( TCT_DEFAULT_GRAY );
						kPrinter[0].AddTextPiece( FONT_SIZE_13,STR(15), iSize );
						kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
						kPrinter[0].AddTextPiece( FONT_SIZE_13,STR(22));

						char szTemp[MAX_PATH]="";
						for (int i = 0; i < iSize ; i++)
						{
							StringCbCat( szTemp, sizeof( szTemp ), vUserIDVec[i].c_str() );
							if( i < iSize - 1 )
								StringCbCat( szTemp, sizeof( szTemp ), " / " );
						}
						enum { WIDTH_SIZE = 215, };
						ioHashString sCutText;
						if( g_FontMgr.GetTextWidth( szTemp, TS_NORMAL, FONT_SIZE_13 ) > WIDTH_SIZE )
							Help::TextCut( szTemp, WIDTH_SIZE, TS_NORMAL, FONT_SIZE_13, sCutText );
						else
							sCutText = szTemp;

						kPrinter[1].SetTextStyle( TS_NORMAL );
						kPrinter[1].SetBkColor( 0, 0, 0 );	
						kPrinter[1].SetTextColor( TCT_DEFAULT_GRAY );	
						kPrinter[1].AddTextPiece( FONT_SIZE_13, sCutText.c_str() );
						kPrinter[2].SetTextStyle( TS_NORMAL );
						kPrinter[2].SetBkColor( 0, 0, 0 );	
						kPrinter[2].SetTextColor( TCT_DEFAULT_DARKGRAY );
						kPrinter[2].AddTextPiece( FONT_SIZE_13,STR(29));
						kPrinter[3].SetTextStyle( TS_NORMAL );
						kPrinter[3].SetBkColor( 0, 0, 0 );	
						kPrinter[3].SetTextColor( TCT_DEFAULT_DARKGRAY );
						kPrinter[3].AddTextPiece( FONT_SIZE_13,STR(30), iUseCoinCnt );

						g_GUIMgr.SetPrevMsgListBox( NULL, MB_OK, NULL, kPrinter );
						return;
					}
				}
			}

			SetModeStart( true, false );
			InitAutoStart();
		}
		else if( cmd == IOEX_BTNUP )
		{
			if( g_BattleRoomMgr.GetOwnerName() != g_MyInfo.GetPublicID() )
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(31) );
			else
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(32) );
		}
		break;
	case ID_BATTLE_END:
		if( cmd == IOBN_BTNUP )
		{
			if( g_BattleRoomMgr.GetOwnerName() != g_MyInfo.GetPublicID() &&
				g_BattleRoomMgr.GetBattleCancelCount() > 0 )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(13) );
			}
			else
			{
				SP2Packet kPacket( CTPK_BATTLEROOM_COMMAND );
				kPacket << BATTLEROOM_STOP_COUNT;
				TCPNetwork::SendToServer( kPacket );
				SetModeStart( false, false );

				ioComplexStringPrinter kPrinter;
				g_ChatMgr.PresetChatComplexString( MSG_INFO_CHAT, kPrinter );
				kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(33) );
				g_ChatMgr.SetChatComplexString( STR(34), kPrinter );
				g_BattleRoomMgr.SetBattleCancelCount();

				m_dwChangeCountTime = 0;	
				m_bBattleOrderControl = true;
				// 배틀모드일때 순서 정하는 버튼 비활성화
				for( int iWndID = ID_EXHIBITION_BLUE_ORDER_1; iWndID < ID_EXHIBITION_RED_ORDER_3 + 1; iWndID++ )
				{
					PartyModeNMapBtn *pAllOrderBtn = dynamic_cast<PartyModeNMapBtn*>( FindChildWnd( iWndID ) );
					if(pAllOrderBtn)
					{
						if( g_MyInfo.GetUserIndex() == pAllOrderBtn->GetUserIndex() )
							pAllOrderBtn->SetActive();
					}
				}
			}			
		}
		break;
	case ID_BATTLE_EXIT:
		if( cmd == IOBN_BTNUP )
		{
			g_App.AltF4ExitProcess( false );
		}
		break;
	case ID_CHANGE_TEAM:
		if( cmd == IOBN_BTNUP )
		{
			OnChangeTeam();
		}
		else if( cmd == IOEX_BTNUP )
		{			
			if( g_BattleRoomMgr.IsBattleModePlaying() )
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(35) );
			else if( m_mode_start_time != 0 )
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(36) );
			else
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(37) );
		}
		break;
	case ID_OBSERVER_TEAM:
		if( cmd == IOBN_BTNUP )
		{
			if( g_GUIMgr.IsShow( PARTY_OBSERVER_WND ) )
				g_GUIMgr.HideWnd( PARTY_OBSERVER_WND );
			else
				g_GUIMgr.ShowWnd( PARTY_OBSERVER_WND );
		}
		break;
	case ID_EXTRA_OPTION_BTN:
		if( cmd == IOBN_BTNUP )
		{
			if( g_BattleRoomMgr.GetOwnerName() == g_MyInfo.GetPublicID() )
			{
				if( !IsModeStart() )
				{
					int nEnableLevel = g_RoomOptionMgr.GetEnableLevel();

					if( (g_BattleRoomMgr.GetSelectModeIndex() == MT_UNDERWEAR || g_BattleRoomMgr.GetSelectModeIndex() == MT_CBT || g_BattleRoomMgr.GetSelectModeIndex() == MT_FARMING ) )
						nEnableLevel = -1;

					if( g_BattleRoomMgr.IsBattleModePlaying() )
					{
						g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(23) );
					}
					else if( !g_BattleRoomMgr.IsUseExtraOption() && nEnableLevel > g_MyInfo.GetGradeLevel() )	// 계급 체크
					{
						char szHelp[MAX_PATH]="";
						g_LevelMgr.GetGradeName( g_RoomOptionMgr.GetEnableLevel(), szHelp, sizeof( szHelp ), false );

						ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
						kPrinter[0].SetTextStyle( TS_NORMAL );
						kPrinter[0].SetBkColor( 0, 0, 0 );
						kPrinter[0].SetTextColor( TCT_DEFAULT_RED );
						kPrinter[0].AddTextPiece( FONT_SIZE_13, STR(27), szHelp );
						kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
						kPrinter[0].AddTextPiece( FONT_SIZE_13, STR(28) );

						g_GUIMgr.SetPrevMsgListBox( NULL, MB_OK, NULL, kPrinter );
					}
					else
					{
						g_BattleRoomMgr.SetUseExtraOption( !g_BattleRoomMgr.IsUseExtraOption(), true );

						if( g_BattleRoomMgr.IsUseExtraOption() )
						{
							g_BattleRoomMgr.SetFirstPreSetMode();

							if( MT_UNDERWEAR != g_BattleRoomMgr.GetSelectModeIndex() && MT_CBT != g_BattleRoomMgr.GetSelectModeIndex() && MT_FARMING != g_BattleRoomMgr.GetSelectModeIndex() )
								g_GUIMgr.ShowWnd(PARTY_OPTION_WND);
						}
						else
							g_GUIMgr.HideWnd(PARTY_OPTION_WND);

						g_BattleRoomMgr.SendBattleRoomOption();
					}
				}
				else
					g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(24) );
			}
			else
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(25) );
			}

			UpdateShamBattle();
			//
		}
		break;
	case ID_EXHIBITION_BLUE_ORDER_1:
	case ID_EXHIBITION_BLUE_ORDER_2:
	case ID_EXHIBITION_BLUE_ORDER_3:
	case ID_EXHIBITION_RED_ORDER_1:
	case ID_EXHIBITION_RED_ORDER_2:
	case ID_EXHIBITION_RED_ORDER_3:
		if( cmd == IOBN_BTNUP )
		{
			if( m_bBattleOrderControl )
				SetBattleModeOrderListItem( pWnd );
		}
		else if( cmd == ID_MODE_MAP_LIST )
		{
			// 리스트에서 선택한 것 처리
			int iSelectBattleOrder = (int)HIWORD( param );
			g_BattleRoomMgr.UpdateUserBattleOrder(g_MyInfo.GetPublicID(), iSelectBattleOrder);
			if(!m_bDoNotSendPacket)
			{
				SP2Packet kPacket( CTPK_BATTLEROOM_COMMAND );
				kPacket << BATTLEROOM_RUNNER_SELECT << iSelectBattleOrder;
				TCPNetwork::SendToServer( kPacket );
			}
		}
		break;
	}
}

bool PartyShamBattleWnd::IsChangeMaxPlayerLimit( int iPrevModeIndex, int iPrevMapIndex )
{
	int iPrevDefaultMaxPlayer = g_ModeSelectInfo.GetBattleMapLimitMaxPlayer( iPrevModeIndex, iPrevMapIndex );
	int iNextDefaultMaxPlayer = g_ModeSelectInfo.GetBattleMapLimitMaxPlayer( g_BattleRoomMgr.GetSelectModeIndex(), g_BattleRoomMgr.GetSelectMapIndex() );
	return (iPrevDefaultMaxPlayer != iNextDefaultMaxPlayer );
}

bool PartyShamBattleWnd::IsMaxPlayerLimit( int iChangeMaxPlayer )
{
	//if( g_BattleRoomMgr.IsBattleModePlaying() || m_mode_start_time != 0 )
	{
		// 적은 수로 변경시는 가능하게하자.
		if( iChangeMaxPlayer < g_BattleRoomMgr.GetMaxPlayerBlue() + g_BattleRoomMgr.GetMaxPlayerRed() )
			return true;

		// 모드 플레이중이거나 모드 시작중에는 최대 인원 변경이 불가능.
		int iMapLimitMaxPlayer = g_ModeSelectInfo.GetBattleMapLimitMaxPlayer( g_BattleRoomMgr.GetSelectModeIndex(), g_BattleRoomMgr.GetSelectMapIndex() );
		if( iChangeMaxPlayer > iMapLimitMaxPlayer )
		{
			ioHashString szModeName = g_ModeSelectInfo.GetBattleModeTitle( g_BattleRoomMgr.GetSelectModeIndex() );
			ioHashString szMapName  = g_ModeSelectInfo.GetBattleMapTitle( g_BattleRoomMgr.GetSelectModeIndex(), g_BattleRoomMgr.GetSelectMapIndex() );
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2), szModeName.c_str(), szMapName.c_str(), iMapLimitMaxPlayer );
			return false;
		}		
	}

	return true;
}

void PartyShamBattleWnd::SetMaxPlayerLimit()
{
	if( g_BattleRoomMgr.GetOwnerName() != g_MyInfo.GetPublicID() ) 
		return;
	if( g_ModeSelectInfo.IsAIMode( g_BattleRoomMgr.GetSelectModeIndex() ) )
		return;

	int iMapDefaultMaxPlayer = g_ModeSelectInfo.GetBattleMapDefaultMaxPlayer( g_BattleRoomMgr.GetSelectModeIndex(), g_BattleRoomMgr.GetSelectMapIndex() );	
	int iBlueCurPlayer = g_BattleRoomMgr.GetBattleTeamUserCount( TEAM_BLUE );
	int iRedCurPlayer  = g_BattleRoomMgr.GetBattleTeamUserCount( TEAM_RED );
	int iBlueMaxPlayer = 0;
	int iRedMaxPlayer  = 0;
	ModeType eModeType = (ModeType)g_ModeSelectInfo.GetBattleMode(g_BattleRoomMgr.GetSelectModeIndex());
	int iSelectModeIndex = g_BattleRoomMgr.GetSelectModeIndex();
	int iSelectMapIndex = g_BattleRoomMgr.GetSelectMapIndex();

	if( iBlueCurPlayer >= iRedCurPlayer )
	{
		iBlueMaxPlayer = max( iBlueCurPlayer, iMapDefaultMaxPlayer / 2 );
		iRedMaxPlayer  = max( iRedCurPlayer, iMapDefaultMaxPlayer - iBlueMaxPlayer );
	}
	else
	{
		iRedMaxPlayer  = max( iRedCurPlayer, iMapDefaultMaxPlayer / 2 );
		iBlueMaxPlayer = max( iBlueCurPlayer, iMapDefaultMaxPlayer - iRedMaxPlayer );
	}
	// 위 코드 지우고 아래 코드로 바꾸고 싶지만 사이드 이펙트를 고려해 일단 if문으로 처리함 (배틀모드)
	// 배틀모드일때 각 팀 최대인원 수 보정...(팀 이동 가능한 추가 슬롯 때문에..)
	if(eModeType == MT_BATTLE)
	{
		iRedMaxPlayer  = iMapDefaultMaxPlayer / 2;
		iBlueMaxPlayer = iMapDefaultMaxPlayer / 2;
	}

	if( eModeType != MT_BATTLE && g_BattleRoomMgr.GetMaxPlayerBlue() != iBlueMaxPlayer || g_BattleRoomMgr.GetMaxPlayerRed() != iRedMaxPlayer )
	{
		SendMaxPlayer( iBlueMaxPlayer, iRedMaxPlayer, g_BattleRoomMgr.GetMaxObserver() );
	}
	else
		SendMaxPlayer( iBlueMaxPlayer, iRedMaxPlayer, g_BattleRoomMgr.GetMaxObserver() );
}

void PartyShamBattleWnd::SendMaxPlayer( int iMaxBluePlayer, int iMaxRedPlayer, int iMaxObserver )
{
	TCPNetwork::MouseBusy( true );
	SP2Packet kPacket( CTPK_MACRO_COMMAND );
	kPacket << MACRO_PLAYER_CNT << iMaxBluePlayer << iMaxRedPlayer << iMaxObserver;
	TCPNetwork::SendToServer( kPacket );
}

void PartyShamBattleWnd::ChangeModeType( bool bOwnerChange )
{ 
	bool bSendOption = false;
	ModeType eModeType = (ModeType)g_ModeSelectInfo.GetBattleMode(g_BattleRoomMgr.GetSelectModeIndex());
	int iModeIndex	= g_BattleRoomMgr.GetSelectModeIndex();
	if( g_ModeSelectInfo.IsFreeForAllMode( iModeIndex ) || 
		g_BattleRoomMgr.IsRandomTeamMode() )
	{
		m_bFreeForAllMode = true;
	}
	else
		m_bFreeForAllMode = false;

	if( iModeIndex == -1 )
	{
		SetChildInActive( ID_CHANGE_MAP );
	}
	else if( m_mode_start_time == 0)
	{
		SetChildActive( ID_CHANGE_MODE );
		SetChildActive( ID_CHANGE_MAP );
	}

	//ai mode 일 경우
	bool bAIMode = g_ModeSelectInfo.IsAIMode( g_BattleRoomMgr.GetSelectModeIndex() );
	if( bAIMode )
	{
		int iMapIndex = g_BattleRoomMgr.GetSelectMapIndex();
		int iSlotMax = g_BattleRoomMgr.GetAIMaxSlot( iMapIndex );		
		g_BattleRoomMgr.SetMaxPlayer( iSlotMax, 0, 0 );
		if( m_bOwner )
		{
			bSendOption = true;
			SendMaxPlayer( iSlotMax, 0, 0 );		
			SendAILevel( g_BattleRoomMgr.GetAiLevel() );
		}
	}
	OnChangeAILevel();
	UpdateUserList();
	
	if( !bAIMode )
	{
		SetChildActive( ID_AUTO_START );
		SetChildActive( ID_RANDOM_TEAM );
		SetChildWndStyleRemove( ID_OBSERVER_TEAM, IWS_EXACTIVE );
	}
	if( m_mode_start_time == 0)
	{
		if( g_ModeSelectInfo.IsFreeForAllMode( iModeIndex ) )
		{
			if( g_BattleRoomMgr.IsObserverUser( g_MyInfo.GetPublicID() ) )
			{
				SetChildWndStyleRemove( ID_CHANGE_TEAM, IWS_EXACTIVE );
			}
			else
			{
				SetChildWndStyleAdd( ID_CHANGE_TEAM, IWS_EXACTIVE );
			}
		}
		else
		{
			SetChildWndStyleRemove( ID_CHANGE_TEAM, IWS_EXACTIVE );
		}
	}
	else
	{
		SetChildWndStyleAdd( ID_CHANGE_TEAM, IWS_EXACTIVE );
	}

	if( IsUserCountFailed() )
	{
		if( m_bOwner )
		{
			SetChildWndStyleAdd( ID_BATTLE_START, IWS_EXACTIVE );	
		}				
	}
	else if( m_bOwner )
	{
		SetChildWndStyleRemove( ID_BATTLE_START, IWS_EXACTIVE );	
	}
	if( bAIMode )
	{
		SetChildWndStyleAdd( ID_CHANGE_TEAM, IWS_EXACTIVE );
		SetChildWndStyleAdd( ID_OBSERVER_TEAM, IWS_EXACTIVE );

		SetChildInActive( ID_AUTO_START );
		SetChildInActive( ID_RANDOM_TEAM );		

		g_BattleRoomMgr.SetStartRoomEnterX( true );
		g_BattleRoomMgr.SetAutoModeStart( false );
		g_BattleRoomMgr.SetRandomTeamMode( false );
	}

	if( !bAIMode )
	{
		HideChildWnd( ID_CHANGE_AI_LEVEL );

		if( m_bOwner )
		{
			m_bStartEnterXMode = false;		
			if( g_BattleRoomMgr.IsStartRoomEnterX() != g_ModeSelectInfo.IsStartEnterX( iModeIndex ) )
			{
				iwm_command( FindChildWnd( ID_START_ENTER_X ), IOBN_BTNUP, 0 ); 
			}
		}
		m_bStartEnterXMode = g_ModeSelectInfo.IsStartEnterX( iModeIndex );
	}
	else
		m_bStartEnterXMode = true;

	if( g_RoomOptionMgr.IsNoChallengerMode(iModeIndex) )
	{
		ShowChildWnd( ID_NO_CHALLENGER );
		SetChildActive( ID_NO_CHALLENGER );

		SetChildInActive( ID_START_ENTER_X );
		HideChildWnd( ID_START_ENTER_X );

		if( g_BattleRoomMgr.GetOwnerName() == g_MyInfo.GetPublicID() )
		{
			int iNoChallengerGrade = g_RoomOptionMgr.GetNoChallengerGrade();

			if( g_MyInfo.GetGradeLevel() < iNoChallengerGrade )
				g_BattleRoomMgr.SetNoChallenger( true );
			else
				g_BattleRoomMgr.SetNoChallenger( false );

			bSendOption = true;
		}
	}
	else
	{
		if(eModeType != MT_BATTLE)
		{
			ShowChildWnd( ID_START_ENTER_X );
			SetChildActive( ID_START_ENTER_X );
		}

		SetChildInActive( ID_NO_CHALLENGER );
		HideChildWnd( ID_NO_CHALLENGER );

		if( g_BattleRoomMgr.GetOwnerName() == g_MyInfo.GetPublicID() )
		{
			g_BattleRoomMgr.SetNoChallenger( false );
			bSendOption = true;
		}
	}
	if( bAIMode )
	{
		SetChildInActive( ID_START_ENTER_X );		
		SetChildWndStyleAdd( ID_OBSERVER_TEAM, IWS_EXACTIVE );		
	}
	else
		SetChildWndStyleRemove( ID_OBSERVER_TEAM, IWS_EXACTIVE );

	if( g_ModeSelectInfo.GetBattleMode(g_BattleRoomMgr.GetSelectModeIndex()) == MT_RAID )
	{
		SetChildInActive( ID_RANDOM_TEAM );
		SetChildInActive( ID_START_ENTER_X );	

		g_BattleRoomMgr.SetStartRoomEnterX( true );
		g_BattleRoomMgr.SetRandomTeamMode( false );
	}

	// option mode
	if( g_RoomOptionMgr.IsEnableOptionMode( iModeIndex ) )
	{
		if( IsShow() )
		{
			SetChildActive( ID_EXTRA_OPTION_BTN );

			ioCheckButton *pCheckBtn = (ioCheckButton*)FindChildWnd( ID_EXTRA_OPTION_BTN );

			if( iModeIndex == MT_UNDERWEAR || iModeIndex == MT_CBT || iModeIndex == MT_FARMING )
			{
				if(pCheckBtn)
				{
					if( !pCheckBtn->IsDisabled())
						SetChildInActive( ID_EXTRA_OPTION_BTN );

					if( !pCheckBtn->IsChecked() )
						pCheckBtn->SetCheck(true);

					if(iModeIndex == MT_UNDERWEAR )
						g_BattleRoomMgr.SetPreSetModeTypeManual(7);
					else if(iModeIndex == MT_CBT)
						g_BattleRoomMgr.SetPreSetModeTypeManual(8);
					else if(iModeIndex == MT_FARMING)
						g_BattleRoomMgr.SetPreSetModeTypeManual(9);
				}
			}
// 			else if( iModeIndex == )
// 			{
// 				if( pCheckBtn && pCheckBtn->IsChecked() )
// 				{
// 					g_BattleRoomMgr.SetUseExtraOption(false, false);
// 					pCheckBtn->SetCheck(false);
// 				}
// 			}
		}
	}
	else
	{
		SetChildInActive( ID_EXTRA_OPTION_BTN );

		if( g_BattleRoomMgr.GetOwnerName() == g_MyInfo.GetPublicID() )
		{
			bool bUse = g_BattleRoomMgr.IsUseExtraOption();
			if( bUse )
			{
				g_BattleRoomMgr.SetUseExtraOption( false, false );
				bSendOption = true;
			}
		}
	}
	//

	// Battle Mode Check
	if(eModeType == MT_BATTLE)
	{
		SetChildInActive( ID_AUTO_START );
		SetChildInActive( ID_RANDOM_TEAM );
		SetChildInActive( ID_START_ENTER_X );
		ShowChildWnd(ID_EXHIBITION_BLUE_ORDER_1);
		ShowChildWnd(ID_EXHIBITION_BLUE_ORDER_2);
		ShowChildWnd(ID_EXHIBITION_BLUE_ORDER_3);

		ShowChildWnd(ID_EXHIBITION_RED_ORDER_1);
		ShowChildWnd(ID_EXHIBITION_RED_ORDER_2);
		ShowChildWnd(ID_EXHIBITION_RED_ORDER_3);
		g_BattleRoomMgr.SetRandomTeamMode(false);
		// 팀 섞어, 중간 참여 옵션 
		//if( m_bOwner )
		//{
		//	// 현재 방장이 나이고 인원이 자신뿐이거나 자신이 모드를 변경했을때만 팀 섞어 기본 옵션이 true로 되게 변경
		//	if( g_BattleRoomMgr.GetCurJoiner() == 1 && g_BattleRoomMgr.GetOwnerName() == g_MyInfo.GetPublicID() || bOwnerChange )
		//		g_BattleRoomMgr.SetRandomTeamMode(true);
		//	g_BattleRoomMgr.SetStartRoomEnterX(true);
		//	if( g_BattleRoomMgr.IsRandomTeamMode() )
		//		FixBattleModeOrderByRandomTeamOpt();
		//}
	}
	else
	{
		ShowChildWnd(ID_AUTO_START);
		ShowChildWnd(ID_RANDOM_TEAM);
		HideChildWnd(ID_EXHIBITION_BLUE_ORDER_1);
		HideChildWnd(ID_EXHIBITION_BLUE_ORDER_2);
		HideChildWnd(ID_EXHIBITION_BLUE_ORDER_3);

		HideChildWnd(ID_EXHIBITION_RED_ORDER_1);
		HideChildWnd(ID_EXHIBITION_RED_ORDER_2);
		HideChildWnd(ID_EXHIBITION_RED_ORDER_3);
	}

	if( bSendOption )
	{
		g_BattleRoomMgr.SendBattleRoomOption();
	}

	// 2012.12.28. KDH
	/*
	ioWnd *pObserver = FindChildWnd( ID_OBSERVER_TEAM	);
	if( pObserver )
	{
		if( g_RoomOptionMgr.IsNoChallengerMode(iModeIndex) )
			pObserver->SetInActive();
		else
			pObserver->SetActive();
	}
	*/
}

void PartyShamBattleWnd::UpdateUserList()
{
	bool bCoinVisible = g_ModeSelectInfo.IsCoinVisible( g_BattleRoomMgr.GetSelectModeIndex() );
	bool bFreeForAllMode = g_ModeSelectInfo.IsFreeForAllMode( g_BattleRoomMgr.GetSelectModeIndex() );

	vWndList vChangeChatBubble;

	if(g_BattleRoomMgr.GetSelectModeIndex() != -1 && m_bCheckSlotByMode)
		SwitchUserSlot();
	int iBlueFirstUserIdx = ID_BLUE_USER_1;
	int iRedFirstUserIdx = ID_RED_USER_1;
	ModeType eModeType = (ModeType)g_ModeSelectInfo.GetBattleMode(g_BattleRoomMgr.GetSelectModeIndex());
	if(eModeType == MT_BATTLE)
	{
		iBlueFirstUserIdx = ID_BLUE_BATTLE_USER_1;
		iRedFirstUserIdx = ID_RED_BATTLE_USER_1;
	}
	// 유저 리스트 세팅
	int i = 0;
	//BLUE TEAM
	for(i = 0;i < MAX_PARTY_PLAYER/2;i++)
	{
		const BattleRoomUserInfo &kInfo = g_BattleRoomMgr.GetBattleUserInfo( TEAM_BLUE, i );
		PartyMemberList *pBtn = dynamic_cast<PartyMemberList*>(FindChildWnd( iBlueFirstUserIdx + i ));
		if( !pBtn ) continue;
	
		int iMonsterCoin = kInfo.m_iMonsterCoin;
		int iGoldMonsterCoin = kInfo.m_iGoldMonsterCoin;
		int iRaidCoin = kInfo.m_iRaidCoin;
		if( !bCoinVisible )
		{
			iMonsterCoin = iGoldMonsterCoin = iRaidCoin = -1;			
		}

		TeamType eUITeam = TEAM_BLUE;	
		if( bFreeForAllMode )
		{
			//해골영웅은 블루 / 데스매치&보스모드는 레드(자신만 블루)
			if( !bCoinVisible )
			{
				if( kInfo.m_szUserID != g_MyInfo.GetPublicID() )
					eUITeam = TEAM_RED;
			}
		}

		pBtn->MemberInfo( eUITeam, i + 1, kInfo.m_iGradeLevel, kInfo.m_szUserID, iMonsterCoin, iGoldMonsterCoin, iRaidCoin );
		pBtn->ShowWnd();

		if( pBtn->IsChangeChatBubble() )
			vChangeChatBubble.push_back( pBtn );

		if(i >= 3 || eModeType != MT_BATTLE)	// 배틀모드가 아니거나 4번째 슬롯에는 순서 정하는 버튼이 생성되지 않는다..'
			continue;
		PartyModeNMapBtn *pBlueOrderBtn = dynamic_cast<PartyModeNMapBtn*>(FindChildWnd( ID_EXHIBITION_BLUE_ORDER_1 + i ));
		if(kInfo.m_szUserID != "")
		{
			ShowChildWnd(ID_EXHIBITION_BLUE_ORDER_1 + i);
			if(pBlueOrderBtn)
			{
				pBlueOrderBtn->SetUserIndex(kInfo.m_dwUserIndex);
				g_BattleRoomMgr.UpdateUserBattleOrder(kInfo.m_szUserID, kInfo.m_iBattleModeOrder);
			}
		}
		else
			HideChildWnd(ID_EXHIBITION_BLUE_ORDER_1 + i);

	}

	//RED TEAM
	for(i = 0;i < MAX_PARTY_PLAYER/2;i++)
	{
		const BattleRoomUserInfo &kInfo = g_BattleRoomMgr.GetBattleUserInfo( TEAM_RED, i );
		PartyMemberList *pBtn = dynamic_cast<PartyMemberList*>(FindChildWnd( iRedFirstUserIdx + i ));
		if( !pBtn ) continue;
	
		int iMonsterCoin = kInfo.m_iMonsterCoin;
		int iGoldMonsterCoin = kInfo.m_iGoldMonsterCoin;
		int iRaidCoin = kInfo.m_iRaidCoin;
		if( !bCoinVisible )
		{
			iMonsterCoin = iGoldMonsterCoin = iRaidCoin = -1;
		}

		TeamType eUITeam = TEAM_RED;	
		if( bFreeForAllMode )
		{
			//해골영웅은 블루 / 데스매치&보스모드는 레드(자신만 블루)
			if( bCoinVisible ||  kInfo.m_szUserID == g_MyInfo.GetPublicID() )
				eUITeam = TEAM_BLUE;	
		}

		pBtn->MemberInfo( eUITeam, i + 1, kInfo.m_iGradeLevel, kInfo.m_szUserID, iMonsterCoin, iGoldMonsterCoin, iRaidCoin );	
		pBtn->ShowWnd();

		if( pBtn->IsChangeChatBubble() )
			vChangeChatBubble.push_back( pBtn );


		if(i >= 3 || eModeType != MT_BATTLE)	// 배틀모드가 아니거나 4번째 슬롯에는 순서 정하는 버튼이 생성되지 않는다..'
			continue;
		PartyModeNMapBtn *pRedOrderBtn = dynamic_cast<PartyModeNMapBtn*>(FindChildWnd( ID_EXHIBITION_RED_ORDER_1 + i ));
		if(kInfo.m_szUserID != "")
		{
			ShowChildWnd(ID_EXHIBITION_RED_ORDER_1 + i);
			if(pRedOrderBtn)
			{
				pRedOrderBtn->SetUserIndex(kInfo.m_dwUserIndex);
				g_BattleRoomMgr.UpdateUserBattleOrder(kInfo.m_szUserID, kInfo.m_iBattleModeOrder);
			}
		}
		else
			HideChildWnd(ID_EXHIBITION_RED_ORDER_1 + i);
	}
	// AI Mode의 경우 클라에서 AI 유저 정보를 만들어서 보여줌
	bool bAIMode = g_ModeSelectInfo.IsAIMode( g_BattleRoomMgr.GetSelectModeIndex() );	
	if( bAIMode )
	{
		int iMapIndex = g_BattleRoomMgr.GetSelectMapIndex();
		int iSlotMax = g_BattleRoomMgr.GetAIMaxSlot( iMapIndex );

		//AI BlueSlot(부족한 유저수만큼만)
		//BLUE TEAM
		for(i = 0;i < iSlotMax;i++)
		{
			const BattleRoomUserInfo &kInfo = g_BattleRoomMgr.GetBattleUserInfo( TEAM_BLUE, i );
			if( kInfo.m_szUserID.IsEmpty() )
			{
				BattleRoomUserInfo kInfoTemp;
				kInfoTemp.m_szUserID = "AI";
				kInfoTemp.m_iGradeLevel = -1;

				PartyMemberList *pBtn = dynamic_cast<PartyMemberList*>(FindChildWnd( iBlueFirstUserIdx + i ));
				if( !pBtn ) continue;

				int iMonsterCoin = -1;
				int iGoldMonsterCoin = -1;
				int iRaidCoin = -1;
				TeamType eUITeam = TEAM_BLUE;	
				if( i < iSlotMax )
					pBtn->MemberInfo( eUITeam, i + 1, kInfoTemp.m_iGradeLevel, kInfoTemp.m_szUserID, iMonsterCoin, iGoldMonsterCoin, iRaidCoin, true );
			}
		}


		//AI RedSlot
		ioHashString szName;
		BattleUserOptionWnd* pBattleUserOption = dynamic_cast<BattleUserOptionWnd*>( FindChildWnd( ID_USER_SLOT_OPTION ) );
		for(i = 0;i < MAX_PARTY_PLAYER/2;i++)
		{
			BattleRoomUserInfo kInfo;
			BattleRoomUserInfo kInfoTemp;
			kInfo.m_szUserID = "AI";
			kInfo.m_iGradeLevel = -1;

			PartyMemberList *pBtn = dynamic_cast<PartyMemberList*>(FindChildWnd( ID_RED_USER_1 + i ));
			if( !pBtn ) continue;

			int iMonsterCoin = -1;
			int iGoldMonsterCoin = -1;
			int iRaidCoin = -1;
			TeamType eUITeam = TEAM_RED;	
			if( i < iSlotMax )
				pBtn->MemberInfo( eUITeam, i + 1, kInfo.m_iGradeLevel, kInfo.m_szUserID, iMonsterCoin, iGoldMonsterCoin, iRaidCoin, true );
			else
				pBtn->MemberInfo( eUITeam, i + 1, kInfoTemp.m_iGradeLevel, kInfoTemp.m_szUserID, iMonsterCoin, iGoldMonsterCoin, iRaidCoin );

			pBtn->ShowWnd();
		}
	}
	{	// Chat Bubble Change		
		for(i = 0;i < (int)vChangeChatBubble.size();i++)
		{
			PartyMemberList *pBtn = dynamic_cast<PartyMemberList*>(vChangeChatBubble[i]);
			if( !pBtn ) continue;

			for(int j = 0;j < MAX_PARTY_PLAYER / 2;j++)
			{
				PartyMemberList *pBlueBtn = dynamic_cast<PartyMemberList*>(FindChildWnd( iBlueFirstUserIdx + j ));
				if( pBlueBtn && pBlueBtn != pBtn )
				{
					pBlueBtn->ChangeChatBubble( pBtn );
				}

				PartyMemberList *pRedBtn = dynamic_cast<PartyMemberList*>(FindChildWnd( iRedFirstUserIdx + j ));
				if( pRedBtn && pRedBtn != pBtn )
				{
					pRedBtn->ChangeChatBubble( pBtn );
				}
			}
		}
		vChangeChatBubble.clear();

		for(i = 0;i < MAX_PARTY_PLAYER / 2;i++)
		{
			PartyMemberList *pBlueBtn = dynamic_cast<PartyMemberList*>(FindChildWnd( iBlueFirstUserIdx + i ));
			if( pBlueBtn && pBlueBtn->IsChangeChatBubble() )
			{
				pBlueBtn->ClearChatBubble();
			}

			PartyMemberList *pRedBtn = dynamic_cast<PartyMemberList*>(FindChildWnd( iRedFirstUserIdx + i ));
			if( pRedBtn && pRedBtn->IsChangeChatBubble() )
			{
				pRedBtn->ClearChatBubble();
			}
		}	
	}
}

bool PartyShamBattleWnd::IsUserCountFailed(int iMinUser)
{
	int iStartMinUser = g_ModeSelectInfo.GetBattleStartMinUser( g_BattleRoomMgr.GetSelectModeIndex() );
	if(iMinUser != -1)
		iStartMinUser = iMinUser;
	// 배틀모드이고 유저 최소인원과 맞지 않으면 false 반환
	ModeType eModeType = (ModeType)g_ModeSelectInfo.GetBattleMode(g_BattleRoomMgr.GetSelectModeIndex());
	if( eModeType == MT_BATTLE )
	{
		int iEnterPlayerCnt = g_ModeSelectInfo.GetBattleMapLimitMaxPlayer( g_BattleRoomMgr.GetSelectModeIndex(), g_BattleRoomMgr.GetSelectMapIndex() );
		iEnterPlayerCnt /= 2;
		if( g_BattleRoomMgr.GetBattleTeamUserCount( TEAM_BLUE ) != iEnterPlayerCnt || g_BattleRoomMgr.GetBattleTeamUserCount( TEAM_RED ) != iEnterPlayerCnt )
			return true;
		else
			return false;
		// 임시로 위 코드 막음, 실제 들어갈때는 아래코드 삭제하고 위 코드 주석 해제
		if(iMinUser != -1)
			return true;
	}
	else
	{
		if( g_ModeSelectInfo.IsFreeForAllMode( g_BattleRoomMgr.GetSelectModeIndex() ) )
		{
			if( g_BattleRoomMgr.GetCurPlayer() < iStartMinUser )
				return true;
		}
		else if( g_BattleRoomMgr.GetBattleTeamUserCount( TEAM_BLUE ) < iStartMinUser || g_BattleRoomMgr.GetBattleTeamUserCount( TEAM_RED ) < iStartMinUser )
		{
			return true;
		}
	}
	
	return false;
}

void PartyShamBattleWnd::OnBattleReadyGO()
{
	m_mode_start_time = 0;
}

void PartyShamBattleWnd::UpdateShamBattle()
{
	if( g_BattleRoomMgr.GetOwnerName() == g_MyInfo.GetPublicID() )
	{
		m_bOwner = true;
	}
	else
	{
		m_bOwner = false;
	}

	if( g_ModeSelectInfo.IsFreeForAllMode( g_BattleRoomMgr.GetSelectModeIndex() ) || 
		g_BattleRoomMgr.IsRandomTeamMode() )
	{
		m_bFreeForAllMode = true;
	}
	else

		m_bFreeForAllMode = false;
	bool bAIMode = g_ModeSelectInfo.IsAIMode( g_BattleRoomMgr.GetSelectModeIndex() );
	if( !bAIMode )
	{
		SetChildWndStyleRemove( ID_CHANGE_TEAM, IWS_EXACTIVE );
		SetChildWndStyleRemove( ID_OBSERVER_TEAM, IWS_EXACTIVE );
		SetChildActive( ID_RANDOM_TEAM );
		HideChildWnd( ID_CHANGE_AI_LEVEL );
	} 
	OnChangeAILevel();
	if( m_mode_start_time == 0 )
	{
		if( g_ModeSelectInfo.IsFreeForAllMode( g_BattleRoomMgr.GetSelectModeIndex() ) )
		{
			if( g_BattleRoomMgr.IsObserverUser( g_MyInfo.GetPublicID() ) )
			{
				SetChildWndStyleRemove( ID_CHANGE_TEAM, IWS_EXACTIVE );
			}
			else
			{
				SetChildWndStyleAdd( ID_CHANGE_TEAM, IWS_EXACTIVE );
			}
		}
		else
		{
			SetChildWndStyleRemove( ID_CHANGE_TEAM, IWS_EXACTIVE );
		}
	}
	else
	{
		SetChildWndStyleAdd( ID_CHANGE_TEAM, IWS_EXACTIVE );
	}
	//ai 용	
	if( bAIMode )
	{
		SetChildWndStyleAdd( ID_CHANGE_TEAM, IWS_EXACTIVE );
		SetChildWndStyleAdd( ID_OBSERVER_TEAM, IWS_EXACTIVE );
		SetChildInActive( ID_RANDOM_TEAM );
		ShowChildWnd( ID_CHANGE_AI_LEVEL );
	}

	if( g_ModeSelectInfo.GetBattleMode(g_BattleRoomMgr.GetSelectModeIndex()) == MT_RAID )
		SetChildInActive( ID_RANDOM_TEAM );

	if( g_ModeSelectInfo.GetBattleMode(g_BattleRoomMgr.GetSelectModeIndex()) == MT_BATTLE )
		SetChildInActive( ID_RANDOM_TEAM );

	HideChildWnd( ID_CHANGE_MODE );
	HideChildWnd( ID_CHANGE_MAP );
	if( g_ModeSelectInfo.GetBattleMode(g_BattleRoomMgr.GetSelectModeIndex()) != MT_BATTLE )
		SetChildWndStyleAdd( ID_BATTLE_START, IWS_EXACTIVE );	

	if( IsUserCountFailed() )
	{
		if( m_bOwner )
		{
			ShowChildWnd( ID_CHANGE_MODE );
			ShowChildWnd( ID_CHANGE_MAP );
			SetChildWndStyleAdd( ID_BATTLE_START, IWS_EXACTIVE );	
			if( m_mode_start_time != 0 )
			{
				SP2Packet kPacket( CTPK_BATTLEROOM_COMMAND );
				kPacket << BATTLEROOM_STOP_COUNT;
				TCPNetwork::SendToServer( kPacket );
				SetModeStart( false, false );
			}
		}				
	}
	else
	{
		if( m_bOwner )
		{
			ShowChildWnd( ID_CHANGE_MODE );
			ShowChildWnd( ID_CHANGE_MAP );
			SetChildActive( ID_CHANGE_MODE );	
			SetChildActive( ID_CHANGE_MAP );	
			SetChildWndStyleRemove( ID_BATTLE_START, IWS_EXACTIVE );	
			if( m_mode_start_time != 0 )
			{
				HideChildWnd( ID_BATTLE_START );	
				ShowChildWnd( ID_BATTLE_END );		
				SetChildInActive( ID_CHANGE_MODE );	
				SetChildInActive( ID_CHANGE_MAP );	
			}
		}
		else
		{
			HideChildWnd( ID_CHANGE_MODE );
			HideChildWnd( ID_CHANGE_MAP );
			HideChildWnd( ID_CHANGE_AI_LEVEL );
			SetChildWndStyleAdd( ID_BATTLE_START, IWS_EXACTIVE );	
			if( m_mode_start_time != 0 )
			{
				HideChildWnd( ID_BATTLE_START );	
				ShowChildWnd( ID_BATTLE_END );	
			}			
		}
	}	

	if( m_bOwner )
	{
		SetChildWndStyleRemove( ID_CHANGE_INFO, IWS_EXACTIVE );
	}
	else 
	{
		SetChildWndStyleAdd( ID_CHANGE_INFO, IWS_EXACTIVE );
	}
	
	SetChildActive( ID_BATTLE_END );

	// 
	ioCheckButton *pCheckBtn = (ioCheckButton*)FindChildWnd( ID_RANDOM_TEAM );
	if( pCheckBtn )
		pCheckBtn->SetCheck( g_BattleRoomMgr.IsRandomTeamMode() );

	pCheckBtn = (ioCheckButton*)FindChildWnd( ID_START_ENTER_X );
	if( pCheckBtn )
		pCheckBtn->SetCheck( g_BattleRoomMgr.IsStartRoomEnterX() );

	pCheckBtn = (ioCheckButton*)FindChildWnd( ID_NO_CHALLENGER );
	if( pCheckBtn && pCheckBtn->IsShow() )
		pCheckBtn->SetCheck( !g_BattleRoomMgr.IsNoChallenger() );

	pCheckBtn = (ioCheckButton*)FindChildWnd( ID_AUTO_START );
	if( pCheckBtn )
		pCheckBtn->SetCheck( g_BattleRoomMgr.IsAutoModeStart() );

	pCheckBtn = (ioCheckButton*)FindChildWnd( ID_BAD_PING_KICK );
	if( pCheckBtn )
		pCheckBtn->SetCheck( g_BattleRoomMgr.IsBadPingKick() );

	pCheckBtn = (ioCheckButton*)FindChildWnd( ID_EXTRA_OPTION_BTN );
	if( pCheckBtn )
		pCheckBtn->SetCheck( g_BattleRoomMgr.IsUseExtraOption() );	

	int iModeIndex	= g_BattleRoomMgr.GetSelectModeIndex();
	bool bEnableOptionMode = g_RoomOptionMgr.IsEnableOptionMode( iModeIndex );
	bool bUseExtraOption = g_BattleRoomMgr.IsUseExtraOption();

	if(iModeIndex == MT_CBT || iModeIndex == MT_UNDERWEAR || iModeIndex == MT_FARMING )
		bUseExtraOption = false;

	PartyOptionWnd *pOption = dynamic_cast<PartyOptionWnd*>( g_GUIMgr.FindWnd(PARTY_OPTION_WND) );
	if( pOption )
	{
		if( !IsShow() || !bUseExtraOption )
		{
			pOption->HideWnd();
		}
		else if( !pOption->IsShow() )
		{
			pOption->ShowWnd();
		}

		pOption->UpdateOption();
	}
	//
	UpdateUserList();

	if( g_BattleRoomMgr.IsSafetyLevelRoom() || g_BattleRoomMgr.IsObserverUser( g_MyInfo.GetPublicID() ) )
		SetChildWndStyleAdd( ID_BATTLE_INVITE, IWS_EXACTIVE );
	else
		SetChildWndStyleRemove( ID_BATTLE_INVITE, IWS_EXACTIVE );

	{	
		if( g_BattleRoomMgr.IsBroadcastModeType() || g_BattleRoomMgr.IsTournamentModeType() )
		{
			if( g_MyInfo.GetUserEventType() == USER_TYPE_BROADCAST_MBC )
				SetChildWndStyleRemove( ID_BATTLE_INVITE, IWS_EXACTIVE );
			else
				SetChildWndStyleAdd( ID_BATTLE_INVITE, IWS_EXACTIVE );
		}		
	}

	// 전투중 UI 제어
	if( g_BattleRoomMgr.IsBattleModePlaying() )
	{
		SetChildWndStyleAdd( ID_BATTLE_START, IWS_EXACTIVE );	
		SetChildInActive( ID_BATTLE_END );
		SetChildInActive( ID_CHANGE_MODE );
		SetChildInActive( ID_CHANGE_MAP );
		SetChildWndStyleAdd( ID_CHANGE_TEAM, IWS_EXACTIVE );
		
		HideChildWnd( ID_CHANGE_MODE );
		HideChildWnd( ID_CHANGE_MAP );
	}

	UpdateShamBattleTournament(); 

	// 심플 UI도 갱신
	PartyBattleSimpleWnd *pBattleSimpleWnd = dynamic_cast<PartyBattleSimpleWnd*>(g_GUIMgr.FindWnd( PARTY_BATTLE_SIMPLE_WND ));
	if( pBattleSimpleWnd && pBattleSimpleWnd->IsShow() )
		pBattleSimpleWnd->UpdateBattleInfo();

	// 배틀모드 출전 순서 버튼 상태 변경
	for( int iWndID = ID_EXHIBITION_BLUE_ORDER_1; iWndID < ID_EXHIBITION_RED_ORDER_3 + 1; iWndID++ )
	{
		PartyModeNMapBtn *pAllOrderBtn = dynamic_cast<PartyModeNMapBtn*>( FindChildWnd( iWndID ) );
		if(pAllOrderBtn)
		{
			// 배틀 모드 순서 변경 버튼 컨트롤 가능한 상태이고 버튼에 저장한 유저 인덱스와 자기 유저 인덱스를 비교해서 일치하면 활성화 상태로 변경
			if( g_MyInfo.GetUserIndex() == pAllOrderBtn->GetUserIndex() && m_bBattleOrderControl )
				pAllOrderBtn->SetActive();
			else
				pAllOrderBtn->SetInActive();
		}
	}
}

void PartyShamBattleWnd::UpdateShamBattleTournament()
{
	if( g_BattleRoomMgr.IsTournamentModeType() == false ) 
		return;

	HideChildWnd( ID_CHANGE_MODE );
	HideChildWnd( ID_CHANGE_MAP );
	HideChildWnd( ID_CHANGE_INFO );
	HideChildWnd( ID_EXTRA_OPTION_BTN );
	HideChildWnd( ID_RANDOM_TEAM );
	HideChildWnd( ID_START_ENTER_X );
	HideChildWnd( ID_AUTO_START );
	HideChildWnd( ID_BAD_PING_KICK );
	HideChildWnd( ID_CHANGE_TEAM );
	HideChildWnd( ID_OBSERVER_TEAM );
	HideChildWnd( ID_BATTLE_INVITE );
	HideChildWnd( ID_BATTLE_START );
	HideChildWnd( ID_BATTLE_END );
	HideChildWnd( ID_NO_CHALLENGER );
	
	m_szTournamentRound.Clear();
	TournamentNode *pTournament = g_TournamentMgr.GetTournament( g_BattleRoomMgr.GetTournamentIndex() );
	if( pTournament )
	{
		//
		char szTitle[MAX_PATH] = "";
		if( pTournament->GetType() == TournamentNode::TYPE_REGULAR )
			sprintf_s_e( szTitle, "%s 정규 대회", pTournament->GetTournamentTitle().c_str() );
		else
			sprintf_s_e( szTitle, "%s 대회", pTournament->GetTournamentTitle().c_str() );  
		m_szTournamentTitle = szTitle;

		TeamGlobalData *pBlueTeam = g_TournamentTeamMgr.GetTeamData( g_BattleRoomMgr.GetTournamentBlueIndex() );
		if( pBlueTeam )
		{
			//
			int iRound = Help::TournamentCurrentRoundWithTeam( pTournament->GetTournamentStartTeamCount(), max( 1, pBlueTeam->GetTourPos() ) - 1 );
			if( iRound <= 2 )
				sprintf_s_e( szTitle, "결승" );
			else
				sprintf_s_e( szTitle, "%d강", iRound );
			m_szTournamentRound = szTitle;

			//
			m_szTournamentBlueTeamName = pBlueTeam->GetTeamName();
		}
		else
		{
			m_SearchEntryTeamType = TEAM_BLUE;
			m_szTournamentBlueTeamName = "?????";
		}

		TeamGlobalData *pRedTeam = g_TournamentTeamMgr.GetTeamData( g_BattleRoomMgr.GetTournamentRedIndex() );
		if( pRedTeam )
		{
			if( m_szTournamentRound.IsEmpty() )
			{
				int iRound = Help::TournamentCurrentRoundWithTeam( pTournament->GetTournamentStartTeamCount(), max( 1, pRedTeam->GetTourPos() ) - 1 );
				if( iRound <= 2 )
					sprintf_s_e( szTitle, "결승" );
				else
					sprintf_s_e( szTitle, "%d강", iRound );
				m_szTournamentRound = szTitle;
			}

			m_szTournamentRedTeamName = pRedTeam->GetTeamName();
		}
		else
		{
			m_SearchEntryTeamType = TEAM_RED;
			m_szTournamentRedTeamName = "?????";
		}
	}
}

void PartyShamBattleWnd::SetTournamentDrawBye()
{
	if( g_BattleRoomMgr.IsTournamentModeType() )
	{
		m_mode_start_time = 0;
		m_mode_cur_sec	  = -1;
	}
}

void PartyShamBattleWnd::SetModeStart( bool isStart, bool isAuto )
{
	if( isStart )
	{
		if( g_BattleRoomMgr.IsTournamentModeType() == false )
		{
			// 대회 경기에서는 시작 패킷을 서버가 보내준다.
			if( m_bOwner )
			{
				SP2Packet kPacket( CTPK_BATTLEROOM_COMMAND );
				kPacket << BATTLEROOM_START_COUNT;
				TCPNetwork::SendToServer(kPacket);			

				m_bBattleOrderControl = false;
				for( int iWndID = ID_EXHIBITION_BLUE_ORDER_1; iWndID < ID_EXHIBITION_RED_ORDER_3 + 1; iWndID++ )
				{
					PartyModeNMapBtn *pAllOrderBtn = dynamic_cast<PartyModeNMapBtn*>( FindChildWnd( iWndID ) );
					if(pAllOrderBtn)
					{
						if( g_MyInfo.GetUserIndex() == pAllOrderBtn->GetUserIndex() )
							pAllOrderBtn->SetInActive();
					}
				}
			}
		}
		else
		{
			m_bTournamentStart = true;
			TournamentNode* pNode =  g_TournamentMgr.GetTournament( g_BattleRoomMgr.GetTournamentIndex(), false );
			if( pNode && pNode->IsLogSend() && g_BattleRoomMgr.IsTournamentModeType() )
			{
				char szLog[MAX_PATH];
				sprintf( szLog, "[대회로그] 54321 카운트 시작 - %s", g_MyInfo.GetPublicID().c_str() );

				SP2Packet kPacket2( LUPK_LOG );
				kPacket2 << "TournamentLog";  // 로그 파일 타입
				kPacket2 << szLog;
				kPacket2 << 4000;  // 오류번호:4000
				kPacket2 << false; // write db
				P2PNetwork::SendLog(kPacket2);

				if( g_App.IsMeDeveloper() )
					LOG.PrintTimeAndLog(0, "%s - %s", __FUNCTION__, szLog );
			}
		}

		// 방장이 아닐때
		if( !m_bOwner )
		{
			m_bBattleOrderControl = false;
			// 배틀모드일때 순서 정하는 버튼 비활성화
			for( int iWndID = ID_EXHIBITION_BLUE_ORDER_1; iWndID < ID_EXHIBITION_RED_ORDER_3 + 1; iWndID++ )
			{
				PartyModeNMapBtn *pAllOrderBtn = dynamic_cast<PartyModeNMapBtn*>( FindChildWnd( iWndID ) );
				if(pAllOrderBtn)
				{
					if( g_MyInfo.GetUserIndex() == pAllOrderBtn->GetUserIndex() )
						pAllOrderBtn->SetInActive();
				}
			}
		}

		HideChildWnd( ID_BATTLE_START );
		ShowChildWnd( ID_BATTLE_END );
		SetChildInActive( ID_CHANGE_MODE );
		SetChildInActive( ID_CHANGE_MAP );
		SetChildInActive( ID_CHANGE_AI_LEVEL );
		SetChildWndStyleAdd( ID_CHANGE_TEAM, IWS_EXACTIVE );
		
		m_mode_start_time = FRAMEGETTIME();
		m_mode_cur_sec	  = -1;
		g_BattleRoomMgr.SetTournamentStartSender( true );

		ioComplexStringPrinter kPrinter;
		g_ChatMgr.PresetChatComplexString( MSG_INFO_CHAT, kPrinter );		
		if( g_BattleRoomMgr.GetBattleUserTeam( g_BattleRoomMgr.GetOwnerName() ) == TEAM_BLUE )
			kPrinter.SetTextColor( TCT_DEFAULT_BLUE );
		else
			kPrinter.SetTextColor( TCT_DEFAULT_RED );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), g_BattleRoomMgr.GetOwnerName().c_str() );
		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(2) );
		g_ChatMgr.SetChatComplexString( STR(1), kPrinter );
	}
	else
	{
		SetChildActive( ID_CHANGE_MODE );							
		SetChildActive( ID_CHANGE_MAP );
		SetChildActive( ID_CHANGE_AI_LEVEL );

		ShowChildWnd( ID_BATTLE_START );
		HideChildWnd( ID_BATTLE_END );
	
		if( g_ModeSelectInfo.IsFreeForAllMode( g_BattleRoomMgr.GetSelectModeIndex() ) )
		{
			SetChildWndStyleAdd( ID_CHANGE_TEAM, IWS_EXACTIVE );
		}
		else
		{
			SetChildWndStyleRemove( ID_CHANGE_TEAM, IWS_EXACTIVE );
		}

		bool bAIMode = g_ModeSelectInfo.IsAIMode( g_BattleRoomMgr.GetSelectModeIndex() );
		if( bAIMode )
		{
			SetChildWndStyleAdd( ID_CHANGE_TEAM, IWS_EXACTIVE );
		}
		if( g_BattleRoomMgr.IsTournamentModeType() )
		{
			m_mode_start_time  = FRAMEGETTIME();
		}
		else
		{
			m_mode_start_time  = 0;
		}
		
		m_mode_cur_sec	   = -1;
		m_bTournamentStart = true;

		// 방장이 아닐 때
		if( !m_bOwner )
		{
			m_bBattleOrderControl = true;
			// 배틀모드일때 순서 정하는 버튼 활성화
			for( int iWndID = ID_EXHIBITION_BLUE_ORDER_1; iWndID < ID_EXHIBITION_RED_ORDER_3 + 1; iWndID++ )
			{
				PartyModeNMapBtn *pAllOrderBtn = dynamic_cast<PartyModeNMapBtn*>( FindChildWnd( iWndID ) );
				if(pAllOrderBtn)
				{
					if( g_MyInfo.GetUserIndex() == pAllOrderBtn->GetUserIndex() )
						pAllOrderBtn->SetActive();
				}
			}
		}
	}

	UpdateShamBattleTournament();
}

void PartyShamBattleWnd::ProcessTournament()
{
	if( !g_BattleRoomMgr.IsTournamentModeType() )
		return;

	if( g_BattleRoomMgr.GetTournamentIndex() == 0 )
		return;

	if( IsModeStart() )
		return;

	bool bDataSync = false;
	TeamGlobalData *pSyncTeam = g_TournamentTeamMgr.GetTeamData( g_BattleRoomMgr.GetTournamentBlueIndex(), false );
	if( pSyncTeam && pSyncTeam->IsDataSync() )
		bDataSync = true;

	pSyncTeam = g_TournamentTeamMgr.GetTeamData( g_BattleRoomMgr.GetTournamentRedIndex(), false );
	if( pSyncTeam && pSyncTeam->IsDataSync() )
		bDataSync = true;

	if( bDataSync )
	{
		UpdateShamBattleTournament();
	}

	//
	if( g_BattleRoomMgr.IsTournamentStartSender() == false )
	{
		if( g_BattleRoomMgr.GetTournamentStartTime() == 0 )      // 시간이 지났다
		{
			if( m_bOwner )
			{
				SP2Packet sendPacket( CTPK_BATTLEROOM_COMMAND );
				sendPacket << BATTLEROOM_TOURNAMENT_START;
				TCPNetwork::SendToServer( sendPacket );
				g_BattleRoomMgr.SetTournamentStartSender( true );

				TournamentNode* pNode =  g_TournamentMgr.GetTournament( g_BattleRoomMgr.GetTournamentIndex(), false );
				if( pNode && pNode->IsLogSend() && g_BattleRoomMgr.IsTournamentModeType() )
				{
					char szLog[MAX_PATH];
					sprintf( szLog, "[대회로그] 5분 대기 완료 - %s", g_MyInfo.GetPublicID().c_str() );

					SP2Packet kPacket2( LUPK_LOG );
					kPacket2 << "TournamentLog";  // 로그 파일 타입
					kPacket2 << szLog;
					kPacket2 << 4000;  // 오류번호:4000
					kPacket2 << false; // write db
					P2PNetwork::SendLog(kPacket2);

					if( g_App.IsMeDeveloper() )
						LOG.PrintTimeAndLog(0, "%s - %s", __FUNCTION__, szLog );
				}
			}
		}
	}
}

void PartyShamBattleWnd::ProcessModeStart()
{
	if( m_mode_start_time != 0 )
	{
		int cur_sec = (FRAMEGETTIME() - m_mode_start_time)/1000;
		if( m_mode_cur_sec != cur_sec )
		{
			m_mode_cur_sec = cur_sec;
			switch( m_mode_cur_sec )
			{
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
				{
					g_BattleRoomMgr.PlayStartSound( 6 - m_mode_cur_sec );

					m_dwChangeCountTime = FRAMEGETTIME();
				}
				break;
			case 6:
				{
					g_BattleRoomMgr.PlayStartSound( -1 );

					m_dwChangeCountTime = FRAMEGETTIME();

					if( m_bOwner )
					{
						if( m_mode_cur_sec == 6 )
						{
							SP2Packet sendPacket( CTPK_BATTLEROOM_COMMAND );
							sendPacket << BATTLEROOM_READY_GO;
							TCPNetwork::SendToServer( sendPacket );

							TournamentNode* pNode =  g_TournamentMgr.GetTournament( g_BattleRoomMgr.GetTournamentIndex(), false );
							if( pNode && pNode->IsLogSend() && g_BattleRoomMgr.IsTournamentModeType() )
							{
								char szLog[MAX_PATH];
								sprintf( szLog, "[대회로그] 54321 카운트 완료 - %s", g_MyInfo.GetPublicID().c_str() );

								SP2Packet kPacket2( LUPK_LOG );
								kPacket2 << "TournamentLog";  // 로그 파일 타입
								kPacket2 << szLog;
								kPacket2 << 4000;  // 오류번호:4000
								kPacket2 << false; // write db
								P2PNetwork::SendLog(kPacket2);

								if( g_App.IsMeDeveloper() )
									LOG.PrintTimeAndLog(0, "%s - %s", __FUNCTION__, szLog );
							}

							if( g_GUIMgr.IsShow( TIMEGATE_SEARCH ) )
							{
								TimeGateSearch *pSearchWnd = dynamic_cast<TimeGateSearch*>( g_GUIMgr.FindWnd( TIMEGATE_SEARCH ) );
								if( pSearchWnd )
									pSearchWnd->SetReserve();
							}
						}
					}
				}
				break;
			}
		}
	}
	else
	{
		//카운트 다운후에 상대 유저가 모두 이탈하는 경우
		if( m_mode_cur_sec == - 1 && m_bTournamentStart && !m_bTournamentStartLog )
		{
			TournamentNode* pNode =  g_TournamentMgr.GetTournament( g_BattleRoomMgr.GetTournamentIndex(), false );
			if( pNode && pNode->IsLogSend() && g_BattleRoomMgr.IsTournamentModeType() )
			{
				char szLog[MAX_PATH];
				sprintf( szLog, "[대회로그] 상대 유저가 모두 이탈하여 대회를 시작할 수 없음 - %s", g_MyInfo.GetPublicID().c_str() );

				SP2Packet kPacket2( LUPK_LOG );
				kPacket2 << "TournamentLog";  // 로그 파일 타입
				kPacket2 << szLog;
				kPacket2 << 4000;  // 오류번호:4000
				kPacket2 << false; // write db
				P2PNetwork::SendLog(kPacket2);
				m_bTournamentStartLog = true;

				if( g_App.IsMeDeveloper() )
					LOG.PrintTimeAndLog(0, "%s - %s", __FUNCTION__, szLog );
			}
		}
	}
}

void PartyShamBattleWnd::ProcessOwnerAutoStart()
{
	if( !m_bOwner )
	{
		InitAutoStart();
		return;
	}	
	else if( IsUserCountFailed() || IsModeStart() || g_BattleRoomMgr.IsBattleModePlaying() || g_BattleRoomMgr.GetCurJoiner() == 1 ) 
	{	
		InitAutoStart();
		return;
	}
	else if( g_BattleRoomMgr.IsTournamentModeType() )
	{
		InitAutoStart();
		return;
	}
	else
	{
		// 맵 계급 제한
		int iMapLimitGradeLevel = g_ModeSelectInfo.GetBattleMapLimitGrade( g_BattleRoomMgr.GetSelectModeIndex(), g_BattleRoomMgr.GetSelectMapIndex() );
		if( iMapLimitGradeLevel != -1 )
		{
			if( !g_BattleRoomMgr.IsAllUserHighGradeLevel( iMapLimitGradeLevel ) )
			{
				InitAutoStart();
				return;
			}
		}

		int nEnableLevel = g_RoomOptionMgr.GetEnableLevel();

		if( (g_BattleRoomMgr.GetSelectModeIndex() == MT_UNDERWEAR || g_BattleRoomMgr.GetSelectModeIndex() == MT_CBT || g_BattleRoomMgr.GetSelectModeIndex() == MT_FARMING ) )
			nEnableLevel = -1;

		// 가능 레벨 체크(참여자)
		if( g_BattleRoomMgr.IsUseExtraOption() && nEnableLevel > g_MyInfo.GetGradeLevel() )
		{	
			InitAutoStart();
			return;
		}

		// 최대 인원 체크
		int iMapLimitMaxPlayer = g_ModeSelectInfo.GetBattleMapLimitMaxPlayer( g_BattleRoomMgr.GetSelectModeIndex(), g_BattleRoomMgr.GetSelectMapIndex() );
		if( iMapLimitMaxPlayer < MAX_PLAYER )
		{
			if( g_ModeSelectInfo.IsBattleStartSetMapLimitMaxPlayer( g_BattleRoomMgr.GetSelectModeIndex(), g_BattleRoomMgr.GetSelectMapIndex() ) )
			{
				if( g_BattleRoomMgr.GetCurPlayer() > iMapLimitMaxPlayer )
				{
					InitAutoStart();
					return;
				}
			}
			else
			{
				if( g_BattleRoomMgr.GetMaxPlayer() > iMapLimitMaxPlayer )
				{
					InitAutoStart();
					return;
				}
			}
		}

		// 코인 체크 여부
		int iUseCoinCnt = g_ModeSelectInfo.GetBattleMapCoinUseCnt( g_BattleRoomMgr.GetSelectModeIndex(), g_BattleRoomMgr.GetSelectMapIndex() );
		if( iUseCoinCnt > 0 )
		{
			InitAutoStart();
			return;
		}

		// 셋팅(ini)에서 설정한 값에 따라 AutoStart진행할건지 판단 -- SYH
		if(!g_ModeSelectInfo.IsAutoStart( g_BattleRoomMgr.GetSelectModeIndex() ))
		{
			InitAutoStart();
			return;
		}
	}
	bool bAIMode = g_ModeSelectInfo.IsAIMode( g_BattleRoomMgr.GetSelectModeIndex() );
	if( bAIMode )
	{
		if( !g_BattleRoomMgr.IsAutoModeStart() )
			return;
	}

	DWORD dwGapTime = FRAMEGETTIME() - m_dwAutoStartCurrentTime;
	int iSecond = dwGapTime / 1000;
	if( iSecond != m_iAutoStartSecond )
	{
		m_iAutoStartSecond = iSecond;
		int iMaxSecond = m_dwAutoStartCheckTime / 1000;
		int iRemainder = max( 0, iMaxSecond - m_iAutoStartSecond );
		switch( iRemainder )
		{
		case 10:
		case 5:
		case 4:
		case 3:
		case 2:
		case 1:
			{
				ioComplexStringPrinter kPrinter;
				g_ChatMgr.PresetChatComplexString( MSG_INFO_CHAT, kPrinter );
				kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(1), iRemainder );
				g_ChatMgr.SetChatComplexString( STR(2), kPrinter );
			}
			break;
		case 0:
			{
				HideChildWnd( ID_MODE_MAP_LIST );
				iwm_command( FindChildWnd( ID_BATTLE_START ), IOBN_BTNUP, 0 );
			}
			break;
		}
	}
}

void PartyShamBattleWnd::OnProcess( float fTimePerSec )
 {
	if( !IsShow() )
	{
		if( g_BattleRoomMgr.IsBattleRoom() || g_RankBattleMgr.IsRankBattlePlaying() )
			ProcessModeStart();
		return;
	}	

	ProcessTournament();
	ProcessModeStart();    
	ProcessOwnerAutoStart();
	ProcessModeHelpTooltip();
	ioMovingWnd::OnProcess( fTimePerSec );
	if( g_GUIMgr.IsDragMove( m_dwID ) || IsMoving() )
	{
		if( m_iDefaultX != GetXPos() )
		{
			if( COMPARE( GetXPos(), 0, (Setting::Width() - GetWidth() ) + 1 ) )
				m_iDefaultX = GetXPos();
		}

		if( m_iDefaultY != GetYPos() )
		{
			int iYPos = GetYPos();
			if( COMPARE( iYPos, 0, (Setting::Height() - GetHeight() ) + 1 ) )
				m_iDefaultY = iYPos;
		}
	}

	g_BackgroundMgr.ProcessBackground();

	if ( g_MyInfo.IsTutorialMode() && m_bOwner && !IsUserCountFailed() )
	{
		g_MyInfo.SetTutorialMode( false );
		if ( !m_bStartEnterXMode )
			iwm_command( FindChildWnd( ID_START_ENTER_X ), IOBN_BTNUP, 0 );
		iwm_command( FindChildWnd( ID_BATTLE_START ), IOBN_BTNUP, 0 );

	}
}

void PartyShamBattleWnd::ProcessModeHelpTooltip()
{	
	if( m_pModeHelpTooltip == NULL )
		m_pModeHelpTooltip = dynamic_cast< ModeHelpTooltip * >( FindChildWnd( ID_MODE_HELP_TOOLTIP ) );

	m_bModeTooltipOver = false;
	ModeHelpTooltip *pModeHelpTooltip = dynamic_cast< ModeHelpTooltip * >( m_pModeHelpTooltip );	
	if( pModeHelpTooltip )
	{
		if( g_App.IsMouseRender() && !m_bOwner )
		{
			if( g_GUIMgr.GetPreOverWnd() == this ||
				g_GUIMgr.GetPreClickWnd() == this )
			{
				ioLSMouse *pMouse = g_App.GetMouse();
				enum { STARTX = 553, STARTY = 390, WIDTH = 21, HEIGHT = 21, };
				RECT rcIconRect;
				rcIconRect.left   = GetDerivedPosX() + STARTX;
				rcIconRect.top    = GetDerivedPosY() + STARTY;
				rcIconRect.right  = rcIconRect.left + WIDTH;
				rcIconRect.bottom = rcIconRect.top + HEIGHT;
				if( ::PtInRect( &rcIconRect, pMouse->GetMousePos() ) )
				{
					m_bModeTooltipOver = true;	

					if( pMouse->IsLBtnUp() )
					{
						// 공략 창
						int iManualIndex = g_ModeSelectInfo.GetBattleModeManualIndex( g_BattleRoomMgr.GetSelectModeIndex() );
						if( iManualIndex != -1 )
						{
							ManualListWnd *pManualWnd = dynamic_cast<ManualListWnd*> (g_GUIMgr.FindWnd( MANUAL_LIST_WND ) );
							if( pManualWnd )
								pManualWnd->ShowWndByCurManual( iManualIndex );
						}				
					}
				}
			}
		}    

		if( m_bModeTooltipOver )
		{
			pModeHelpTooltip->SetModeHelpTooltip( this, g_ModeSelectInfo.GetBattleModeTitle( g_BattleRoomMgr.GetSelectModeIndex() ),
												  g_ModeSelectInfo.GetBattleModeHelpIndex( g_BattleRoomMgr.GetSelectModeIndex() ) );
		}
		else
		{
			pModeHelpTooltip->SetModeHelpTooltip( NULL, "", -1 );
		}
	}	
}

void PartyShamBattleWnd::OnTournamentEntrySearchRender( int iXPos, int iYPos )
{
	if( m_SearchEntryTeamType == TEAM_BLUE )
	{
		if( m_pTournamentEntryArmFrm )
		{
			m_pTournamentEntryArmFrm->Render( iXPos + 129, iYPos + 185 );		

			ioComplexStringPrinter kPrinter;
			kPrinter.SetTextStyle( TS_NORMAL );
			kPrinter.SetTextColor( TCT_DEFAULT_BLUE );
			kPrinter.AddTextPiece( FONT_SIZE_14, "예비엔트리 검색중" );	
			kPrinter.PrintFullText( iXPos + 129 + 77, iYPos + 185 + 4, TAT_CENTER );
			kPrinter.ClearList();

			kPrinter.SetTextStyle( TS_NORMAL );
			kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
			kPrinter.AddTextPiece( FONT_SIZE_11, "상대팀의 미참가로" );	
			kPrinter.PrintFullText( iXPos + 129 + 77, iYPos + 185 + 23, TAT_CENTER );
			kPrinter.ClearList();

			kPrinter.SetTextStyle( TS_NORMAL );
			kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
			kPrinter.AddTextPiece( FONT_SIZE_11, "다른팀을 검색 중입니다." );	
			kPrinter.PrintFullText( iXPos + 129 + 77, iYPos + 185 + 35, TAT_CENTER );
			kPrinter.ClearList();
		}

	}
	else if( m_SearchEntryTeamType == TEAM_RED )
	{
		if( m_pTournamentEntryArmFrm )
		{
			m_pTournamentEntryArmFrm->Render( iXPos + 489, iYPos + 185 );
		
			ioComplexStringPrinter kPrinter;
			kPrinter.SetTextStyle( TS_NORMAL );
			kPrinter.SetTextColor( TCT_DEFAULT_RED );
			kPrinter.AddTextPiece( FONT_SIZE_14, "예비엔트리 검색중" );	
			kPrinter.PrintFullText( iXPos + 489 + 77, iYPos + 185 + 4, TAT_CENTER );
			kPrinter.ClearList();

			kPrinter.SetTextStyle( TS_NORMAL );
			kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
			kPrinter.AddTextPiece( FONT_SIZE_11, "상대팀의 미참가로" );	
			kPrinter.PrintFullText( iXPos + 489 + 77, iYPos + 185 + 23, TAT_CENTER );
			kPrinter.ClearList();

			kPrinter.SetTextStyle( TS_NORMAL );
			kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
			kPrinter.AddTextPiece( FONT_SIZE_11, "다른팀을 검색 중입니다." );	
			kPrinter.PrintFullText( iXPos + 489 + 77, iYPos + 185 + 35, TAT_CENTER );
			kPrinter.ClearList();
		}
	}
}

void PartyShamBattleWnd::OnTournamentRender( int iXPos, int iYPos )
{
	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_OUTLINE_FULL_2X );
	kPrinter.SetBkColor( 12, 66, 111 );
	kPrinter.SetTextColor( 255, 255, 255 );		
	kPrinter.AddTextPiece( FONT_SIZE_17, "%s", m_szTournamentTitle.c_str() );
	kPrinter.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_17, " | " );
	kPrinter.SetTextColor( 255, 255, 255 );		
	kPrinter.AddTextPiece( FONT_SIZE_17, "%s", m_szTournamentRound.c_str() );
	kPrinter.PrintFullText( iXPos + 13, iYPos + 11, TAT_LEFT );
	kPrinter.ClearList();

	//
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetTextColor( TCT_DEFAULT_BLUE );
	kPrinter.AddTextPiece( FONT_SIZE_13, "● %s", m_szTournamentBlueTeamName.c_str() );
	kPrinter.PrintFullText( iXPos + 26, iYPos + 61, TAT_LEFT );
	kPrinter.ClearList();

	//
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetTextColor( TCT_DEFAULT_RED );
	kPrinter.AddTextPiece( FONT_SIZE_13, "● %s", m_szTournamentRedTeamName.c_str() );
	kPrinter.PrintFullText( iXPos + 389, iYPos + 61, TAT_LEFT );
	kPrinter.ClearList();

	//
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "● 게임이 시작될 때까지 잠시 기다려주세요." );
	kPrinter.PrintFullText( iXPos + 421, iYPos + 433, TAT_LEFT );
	kPrinter.ClearList();

	if( m_pTournamentStartGrayFrm )
	{
		m_pTournamentStartGrayFrm->Render( iXPos + 416, iYPos + 464 );
	}

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetTextColor( TCT_DEFAULT_RED );
	if( IsModeStart() )
	{
		kPrinter.AddTextPiece( FONT_SIZE_18, "곧 시작됩니다!!" );		
	}
	else 
	{
		int iMinute = g_BattleRoomMgr.GetTournamentStartTime() / 60000;
		int iSec    = ( g_BattleRoomMgr.GetTournamentStartTime() % 60000 ) / 1000;
		if( iMinute > 0 )
			kPrinter.AddTextPiece( FONT_SIZE_18, "%d분 %d초 ", iMinute, iSec );
		else
			kPrinter.AddTextPiece( FONT_SIZE_18, "%d초 ", max( 1, iSec ) );
		kPrinter.AddTextPiece( FONT_SIZE_18, " 후 게임시작" );
	}
	kPrinter.PrintFullText( iXPos + 580, iYPos + 476, TAT_CENTER );
	kPrinter.ClearList();
}

void PartyShamBattleWnd::OnRender()
{
	g_BackgroundMgr.RenderBackground();

	ioWnd::OnRender();

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	if( g_BattleRoomMgr.IsTournamentModeType() )
	{
		OnTournamentRender( iXPos, iYPos );
	}
	else
	{
		//Title
		ioComplexStringPrinter kPrinter;
		kPrinter.SetTextStyle( TS_OUTLINE_FULL_2X );
		kPrinter.SetBkColor( 12, 66, 111 );
		kPrinter.SetTextColor( 255, 255, 255 );		
		kPrinter.AddTextPiece( FONT_SIZE_17, STR(1), g_BattleRoomMgr.GetIndex() );
		kPrinter.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
		kPrinter.AddTextPiece( FONT_SIZE_17, " | " );
		kPrinter.SetTextColor( 255, 255, 255 );		
		kPrinter.AddTextPiece( FONT_SIZE_17, STR(2), g_BattleRoomMgr.GetName().c_str() );
		if( !g_BattleRoomMgr.GetPW().IsEmpty() )
		{
			if( m_bOwner )
				kPrinter.AddTextPiece( FONT_SIZE_17, STR(3), g_BattleRoomMgr.GetPW().c_str() );
			else
				kPrinter.AddTextPiece( FONT_SIZE_17, STR(4) );
		}
		kPrinter.PrintFullText( iXPos + 13, iYPos + 11, TAT_LEFT );
		kPrinter.ClearList();

		BattleRoomInfo BattleInfo;
		g_BattleRoomMgr.GetBattleRoomInfo( &BattleInfo );
		bool bFreeForAllMode = g_ModeSelectInfo.IsFreeForAllMode( g_BattleRoomMgr.GetSelectModeIndex() );
		kPrinter.SetTextStyle( TS_NORMAL );
		if( bFreeForAllMode )
			kPrinter.SetTextColor( 128, 128, 128 );
		else
			kPrinter.SetTextColor( TCT_DEFAULT_BLUE );
		kPrinter.AddTextPiece( FONT_SIZE_13, STR(5) );

		if( bFreeForAllMode )
			kPrinter.SetTextColor( 128, 128, 128 );
		else
			kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
		kPrinter.AddTextPiece( FONT_SIZE_13, STR(6), BattleInfo.m_iBlueWin, BattleInfo.m_iBlueLose );
		if( BattleInfo.m_iBlueVictories > 1 )
			kPrinter.AddTextPiece( FONT_SIZE_13, STR(7), BattleInfo.m_iBlueVictories );
		kPrinter.PrintFullText( iXPos + 26, iYPos + 61, TAT_LEFT );
		kPrinter.ClearList();

		kPrinter.SetTextStyle( TS_NORMAL );
		if( bFreeForAllMode )
			kPrinter.SetTextColor( 128, 128, 128 );
		else
			kPrinter.SetTextColor( TCT_DEFAULT_RED );
		kPrinter.AddTextPiece( FONT_SIZE_13, STR(8) );

		if( bFreeForAllMode )
			kPrinter.SetTextColor( 128, 128, 128 );
		else
			kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
		kPrinter.AddTextPiece( FONT_SIZE_13, STR(9), BattleInfo.m_iRedWin, BattleInfo.m_iRedLose );
		if( BattleInfo.m_iRedVictories > 1 )
			kPrinter.AddTextPiece( FONT_SIZE_13, STR(10), BattleInfo.m_iRedVictories );
		kPrinter.PrintFullText( iXPos + 389, iYPos + 61, TAT_LEFT );
		kPrinter.ClearList();
	}

	OnModeNMapRender( iXPos + 416, iYPos + 385 );
}

void PartyShamBattleWnd::OnRenderAfterChild()
{
	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	OnCountRender();

	if( m_bSearchEntry && g_BattleRoomMgr.IsTournamentModeType() )
	{
		OnTournamentEntrySearchRender( iXPos, iYPos );
	}
}

void PartyShamBattleWnd::OnCountRender()
{
	if( m_dwChangeCountTime == 0 )
		return;

	int iCurArray = MAX_WAR_COUNT - m_mode_cur_sec;
	if( !COMPARE( iCurArray, 0, MAX_WAR_COUNT ) )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	DWORD dwNumScaleTime = 150;
	DWORD dwCircleTime = 500;
	DWORD dwStartCircleTime1 = 150;
	DWORD dwStartCircleTime2 = 210;

	float fNumScaleRate = 2.0f;
	float fStartCircleRate = FLOAT05;
	float fCircleRate = 1.5f;

	float fNumCurScale = FLOAT1;
	float fCircleRate1 = FLOAT05;
	float fCircleRate2 = FLOAT05;
	float fCircleAlpha1 = FLOAT1;
	float fCircleAlpha2 = FLOAT1;

	bool bRenderCircle1 = false;
	bool bRenderCircle2 = false;

	DWORD dwGapTime = dwCurTime - m_dwChangeCountTime;
	if( dwGapTime < dwNumScaleTime )
	{
		float fTimeRate = (float)dwGapTime / (float)dwNumScaleTime;
		fNumCurScale = fNumScaleRate * cosf( (D3DX_PI/2) * fTimeRate );
	}
	else
	{
		bRenderCircle1 = true;
		fCircleRate1 = 2.0f;
		DWORD dwCircleGap1 = dwGapTime - dwStartCircleTime1;
		if( dwCircleGap1 < dwCircleTime )
		{
			float fTimeRate = (float)dwCircleGap1 / (float)dwCircleTime;
			fCircleRate1 = fStartCircleRate + fCircleRate * sinf( (D3DX_PI/2) * fTimeRate );
			fCircleAlpha1 = cosf( (D3DX_PI/2) * fTimeRate );
		}
		else
		{
			bRenderCircle1 = false;
		}

		if( dwGapTime > dwStartCircleTime2 )
		{
			bRenderCircle2 = true;

			fCircleRate2 = 2.0f;
			DWORD dwCircleGap2 = dwGapTime - dwStartCircleTime2;
			if( dwCircleGap2 < dwCircleTime )
			{
				float fTimeRate = (float)dwCircleGap2 / (float)dwCircleTime;
				fCircleRate2 = fStartCircleRate + fCircleRate * sinf( (D3DX_PI/2) * fTimeRate );
				fCircleAlpha2 = cosf( (D3DX_PI/2) * fTimeRate );
			}
			else
			{
				bRenderCircle2 = false;
			}
		}
	}

	enum
	{
		COUNT_X	= 386,
		COUNT_Y	= 286,
	};

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	// Circle
	if( m_pWarCountCircle )
	{
		if( bRenderCircle1 )
		{
			m_pWarCountCircle->SetAlpha((BYTE)(int)(255.0f * fCircleAlpha1));
			m_pWarCountCircle->SetScale( fCircleRate1 );
			m_pWarCountCircle->Render( iXPos+COUNT_X, iYPos+COUNT_Y, UI_RENDER_NORMAL, TFO_BILINEAR );
		}

		if( bRenderCircle2 )
		{
			m_pWarCountCircle->SetAlpha((BYTE)(int)(255.0f * fCircleAlpha2));
			m_pWarCountCircle->SetScale( fCircleRate2 );
			m_pWarCountCircle->Render( iXPos+COUNT_X, iYPos+COUNT_Y, UI_RENDER_NORMAL, TFO_BILINEAR );
		}
	}

	// Count
	if( m_pWarCount[iCurArray] )
	{
		m_pWarCount[iCurArray]->SetScale( fNumCurScale );
		m_pWarCount[iCurArray]->Render( iXPos+COUNT_X, iYPos+COUNT_Y, UI_RENDER_NORMAL, TFO_BILINEAR );
	}
}

void PartyShamBattleWnd::OnModeNMapRender( int iXPos, int iYPos )
{
	if( g_BattleRoomMgr.IsTournamentModeType() == false )
	{
		if( m_bOwner || !m_pModeNMapGrayFrm )
			return;
	}

	// 모드
	m_pModeNMapGrayFrm->Render( iXPos, iYPos );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 26, iYPos + 8, FONT_SIZE_12, STR(1) );

	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( iXPos + 46, iYPos + 8, FONT_SIZE_12, "|" );

	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );

	ioHashString kModeTitle = g_ModeSelectInfo.GetBattleModeTitle( g_BattleRoomMgr.GetSelectModeIndex() );
	g_FontMgr.PrintTextWidthCut( iXPos + 54, iYPos + 8, FONT_SIZE_12,FLOAT90, kModeTitle.c_str() );	

	ioUIRenderImage *pOrderMark = g_ModeSelectInfo.GetBattleModeOrderMarkSmall( g_BattleRoomMgr.GetSelectModeIndex() );
	if( pOrderMark )
	{
		int iTitleSize = g_FontMgr.GetTextWidthCutSize( kModeTitle.c_str(), TS_NORMAL, FONT_SIZE_12,FLOAT90 );
		pOrderMark->Render( iXPos + 56 + iTitleSize, iYPos + 10 );
	}

	if( m_pManualMark )
	{
		if( m_bModeTooltipOver )
			m_pManualMark->SetColor( 206, 78, 0 );
		else
			m_pManualMark->SetColor( 110, 110, 110 );
		m_pManualMark->Render( iXPos + 140, iYPos + 8, UI_RENDER_NORMAL_ADD_COLOR );
	}

	bool bAIMode = g_ModeSelectInfo.IsAIMode( g_BattleRoomMgr.GetSelectModeIndex() );
	if( bAIMode )
	{
		m_pModeNMapGrayFrm->SetSize( 95, m_pModeNMapGrayFrm->GetHeight() );
	}
	else
	{
		m_pModeNMapGrayFrm->SetSize( 163, m_pModeNMapGrayFrm->GetHeight() );
	}
	// 맵
	iXPos += 165;
	m_pModeNMapGrayFrm->Render( iXPos, iYPos );
	bool bUseMonsterCoin = g_ModeSelectInfo.IsCoinVisible( g_BattleRoomMgr.GetSelectModeIndex() );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	if( bUseMonsterCoin )
	{
		g_FontMgr.PrintText( iXPos + 26, iYPos + 8, FONT_SIZE_12, STR(2) );

		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
		g_FontMgr.PrintText( iXPos + 46, iYPos + 8, FONT_SIZE_12, "|" );

		g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );

		ioHashString kMapTitle = g_ModeSelectInfo.GetBattleMapTitle( g_BattleRoomMgr.GetSelectModeIndex(), g_BattleRoomMgr.GetSelectMapIndex() );
		g_FontMgr.PrintTextWidthCut( iXPos + 54, iYPos + 8, FONT_SIZE_12,FLOAT90, kMapTitle.c_str() );

		ioUIRenderImage *pOrderMark = g_ModeSelectInfo.GetBattleMapOrderMarkSmall( g_BattleRoomMgr.GetSelectModeIndex(), g_BattleRoomMgr.GetSelectMapIndex() );
		if( pOrderMark )
		{
			int iTitleSize = g_FontMgr.GetTextWidthCutSize( kMapTitle.c_str(), TS_NORMAL, FONT_SIZE_12,FLOAT90 );
			pOrderMark->Render( iXPos + 56 + iTitleSize, iYPos + 10 );
		}
	}
	else
	{
		if( bAIMode )
		{
			g_FontMgr.SetAlignType( TAT_LEFT );
			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );

			ioHashString kMapTitle = g_ModeSelectInfo.GetBattleMapTitle( g_BattleRoomMgr.GetSelectModeIndex(), g_BattleRoomMgr.GetSelectMapIndex() );
			g_FontMgr.PrintTextWidthCut( iXPos + 11, iYPos + 8, FONT_SIZE_12,FLOAT90, kMapTitle.c_str() );
		}
		else
		{
			g_FontMgr.PrintText( iXPos + 26, iYPos + 8, FONT_SIZE_12, STR(3) );

			g_FontMgr.SetAlignType( TAT_LEFT );
			g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
			g_FontMgr.PrintText( iXPos + 46, iYPos + 8, FONT_SIZE_12, "|" );

			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );

			ioHashString kMapTitle = g_ModeSelectInfo.GetBattleMapTitle( g_BattleRoomMgr.GetSelectModeIndex(), g_BattleRoomMgr.GetSelectMapIndex() );
			g_FontMgr.PrintTextWidthCut( iXPos + 54, iYPos + 8, FONT_SIZE_12,FLOAT90, kMapTitle.c_str() );

			ioUIRenderImage *pOrderMark = g_ModeSelectInfo.GetBattleMapOrderMarkSmall( g_BattleRoomMgr.GetSelectModeIndex(), g_BattleRoomMgr.GetSelectMapIndex() );
			if( pOrderMark )
			{
				int iTitleSize = g_FontMgr.GetTextWidthCutSize( kMapTitle.c_str(), TS_NORMAL, FONT_SIZE_12,FLOAT90 );
				pOrderMark->Render( iXPos + 56 + iTitleSize, iYPos + 10 );
			}
		}
	}
	// 난이도
	if( bAIMode )
	{
		m_pModeNMapGrayFrm->SetSize( 67, m_pModeNMapGrayFrm->GetHeight() );
		// 맵
		iXPos += 97;
		m_pModeNMapGrayFrm->Render( iXPos, iYPos );		
		ioHashString szLevel;
		int iAiLevel = g_BattleRoomMgr.GetAiLevel();
		if( iAiLevel == 0 )
			szLevel = "쉬움";
		else
			szLevel = "보통";
		g_FontMgr.PrintTextWidthCut( iXPos + 10, iYPos + 8, FONT_SIZE_12,FLOAT90, szLevel.c_str() );
		m_pModeNMapGrayFrm->SetSize( 163, m_pModeNMapGrayFrm->GetHeight() );
	}
}

void PartyShamBattleWnd::StartChatBubble( const char *szID, const char *szChat, bool bWholeChat )
{
	int iBlueFirstUserIdx = ID_BLUE_USER_1;
	int iRedFirstUserIdx = ID_RED_USER_1;
	ModeType eModeType = (ModeType)g_ModeSelectInfo.GetBattleMode(g_BattleRoomMgr.GetSelectModeIndex());
	if(eModeType == MT_BATTLE)
	{
		iBlueFirstUserIdx = ID_BLUE_BATTLE_USER_1;
		iRedFirstUserIdx = ID_RED_BATTLE_USER_1;
	}

	int i = 0;
	//BLUE TEAM
	for(i = 0;i < MAX_PARTY_PLAYER/2;i++)
	{
		PartyMemberList *pBtn = dynamic_cast<PartyMemberList*>(FindChildWnd( iBlueFirstUserIdx + i ));
		if( !pBtn ) continue;
		
		if( pBtn->GetListName() == szID )
			pBtn->StartChatBubble( szID, szChat, bWholeChat );
	}

	//RED TEAM
	for(i = 0;i < MAX_PARTY_PLAYER/2;i++)
	{
		PartyMemberList *pBtn = dynamic_cast<PartyMemberList*>(FindChildWnd( iRedFirstUserIdx + i ));
		if( !pBtn ) continue;

		if( pBtn->GetListName() == szID )
			pBtn->StartChatBubble( szID, szChat, bWholeChat );
	}
}

bool PartyShamBattleWnd::QuestGuide( int iModeSelectIndex, int iMapSelectIndex, float &rfXPos, float &rfYPos )
{
	if( !g_BattleRoomMgr.IsBattleRoom() )
		return false;
	if( !m_bOwner )
		return false;

	// 모드 체크
	if( iModeSelectIndex != -2 && g_BattleRoomMgr.GetSelectModeIndex() != iModeSelectIndex )
	{
		ioWnd *pSelectBtn = FindChildWnd( ID_CHANGE_MODE );
		ModeMapSelectList *pModeList = dynamic_cast<ModeMapSelectList *>(FindChildWnd( ID_MODE_MAP_LIST ));
		if( pModeList && pModeList->IsShow() )
		{
			if( pModeList->GetOpenBtn() == pSelectBtn )
			{
				if( pModeList->GetIndexToXYPos( iModeSelectIndex, rfXPos, rfYPos ) )
					return true;
			}
		}
		else if( pSelectBtn )
		{
			rfXPos = pSelectBtn->GetDerivedPosX() + ( pSelectBtn->GetWidth() / 2 );
			rfYPos = pSelectBtn->GetDerivedPosY();
			return true;			
		}
		return false;
	}
	
	// 맵 체크
	if( iMapSelectIndex != -2 && g_BattleRoomMgr.GetSelectMapIndex() != iMapSelectIndex )
	{
		ioWnd *pSelectBtn = FindChildWnd( ID_CHANGE_MAP );
		ModeMapSelectList *pMapList = dynamic_cast<ModeMapSelectList *>(FindChildWnd( ID_MODE_MAP_LIST ));
		if( pMapList && pMapList->IsShow() && pMapList->GetOpenBtn() == pSelectBtn )
		{
			// 모드를 먼저 선택해야함
			if( iModeSelectIndex != g_BattleRoomMgr.GetSelectModeIndex() )
				return false;

			if( pMapList->GetIndexToXYPos( iMapSelectIndex, rfXPos, rfYPos ) )
				return true;
		}
		else if( pSelectBtn )
		{
			rfXPos = pSelectBtn->GetDerivedPosX() + ( pSelectBtn->GetWidth() / 2 );
			rfYPos = pSelectBtn->GetDerivedPosY();
			return true;
		}
		return false;
	}

	// 시작 버튼
	ioWnd *pBattleStartBtn = FindChildWnd( ID_BATTLE_START );
	if( pBattleStartBtn && pBattleStartBtn->IsShow() )
	{
		rfXPos = pBattleStartBtn->GetDerivedPosX() + ( pBattleStartBtn->GetWidth() / 2 );
		rfYPos = pBattleStartBtn->GetDerivedPosY();
		return true;
	}
	return false;
}

void PartyShamBattleWnd::SetBattleModeOrderListItem( ioWnd* pWnd )
{
	if( g_BattleRoomMgr.GetSelectModeIndex() == -1 )
		return;             //모드가 랜덤이면 맵은 변경 불가

	ModeMapSelectList *pMapList = dynamic_cast<ModeMapSelectList *>(FindChildWnd( ID_MODE_MAP_LIST ));
	if( pMapList )
	{
		int iMapIndex = g_BattleRoomMgr.GetSelectMapIndex();
		pMapList->OpenList( pWnd, false, BATTLE_ORDER_FIRST, false, pWnd->GetXPos(), pWnd->GetYPos() - 2, false );
		pMapList->AddList( BATTLE_ORDER_FIRST, false, STR(1)/*"First"*/, -1, -1, NULL, -91 );
		if(g_BattleRoomMgr.GetSelectMapIndex() == 1)			// 3vs3
		{
			pMapList->AddList( BATTLE_ORDER_SECOND, false, STR(2)/*"Second"*/, -1, -1, NULL, -91 );
			pMapList->AddList( BATTLE_ORDER_THIRD, false, STR(3)/*"Third"*/, -1, -1, NULL, -91 );
		}
		else if(g_BattleRoomMgr.GetSelectMapIndex() == 2)			// 2vs2
			pMapList->AddList( BATTLE_ORDER_SECOND, false, STR(3)/*"Third"*/, -1, -1, NULL, -91 );

		pMapList->AddList( BATTLE_ORDER_RANDOM, false, STR(4)/*"Random"*/, -1, -1, NULL, -91 );
	}
}

void PartyShamBattleWnd::OnChangeAILevel()
{
	bool bAIMode = g_ModeSelectInfo.IsAIMode( g_BattleRoomMgr.GetSelectModeIndex() );
	if( bAIMode )
	{
		ShowChildWnd( ID_CHANGE_AI_LEVEL );
		PartyModeNMapBtn* pWnd = dynamic_cast<PartyModeNMapBtn*>(FindChildWnd(ID_CHANGE_MAP));
		if( pWnd )
			pWnd->OnAiMode( true );
	}
	else
	{
		HideChildWnd( ID_CHANGE_AI_LEVEL );
		PartyModeNMapBtn* pWnd = dynamic_cast<PartyModeNMapBtn*>(FindChildWnd(ID_CHANGE_MAP));
		if( pWnd )
			pWnd->OnAiMode( false );
	}
}

void PartyShamBattleWnd::SendAILevel( int iLevel )
{
	if( m_bOwner )
	{
		SP2Packet kPacket( CTPK_MACRO_COMMAND );
		kPacket << MACRO_AI_LEVEL_CHANGE << iLevel;
		TCPNetwork::SendToServer( kPacket );	
	}
}

void PartyShamBattleWnd::SwitchUserSlot()
{
	ModeType eModeType = (ModeType)g_ModeSelectInfo.GetBattleMode(g_BattleRoomMgr.GetSelectModeIndex());
	if(eModeType == MT_BATTLE)
	{
		for (int i = ID_BLUE_USER_1; i < (ID_BLUE_USER_8 + 1); i++)
			HideChildWnd( i );
		for (int j = ID_RED_USER_1; j < (ID_RED_USER_8 + 1); j++)
			HideChildWnd( j );
	}
	else
	{
		for (int i = ID_BLUE_BATTLE_USER_1; i < (ID_BLUE_BATTLE_USER_4 + 1); i++)
			HideChildWnd( i );
		for (int j = ID_RED_BATTLE_USER_1; j < (ID_RED_BATTLE_USER_4 + 1); j++)
			HideChildWnd( j );
	}
	m_bCheckSlotByMode = false;
}

void PartyShamBattleWnd::FixBattleModeOrderByRandomTeamOpt()
{
	m_bBattleOrderControl = !g_BattleRoomMgr.IsRandomTeamMode();
	for( int iWndID = ID_EXHIBITION_BLUE_ORDER_1; iWndID < ID_EXHIBITION_RED_ORDER_3 + 1; iWndID++ )
	{
		PartyModeNMapBtn *pAllOrderBtn = dynamic_cast<PartyModeNMapBtn*>( FindChildWnd( iWndID ) );
		if(pAllOrderBtn)
		{
			if( g_MyInfo.GetUserIndex() == pAllOrderBtn->GetUserIndex() )
			{
				if( m_bBattleOrderControl )
					pAllOrderBtn->SetActive();
				else
				{
					m_bDoNotSendPacket = true;
					pAllOrderBtn->SetInActive();
					m_bDoNotSendPacket = false;
				}
			}
			iwm_command( FindChildWnd( iWndID ), ID_MODE_MAP_LIST, 0 /*무작위*/ );
		}
	}
}

//////////////////////////////////////////////////////////////////////////
PartyBattleSimpleBtn::PartyBattleSimpleBtn()
{
	m_pBlueNormal = NULL;
	m_pRedNormal = NULL;
	m_pNoneNormal = NULL;
	m_pBlueOver = NULL;
	m_pRedOver = NULL;
	m_pNoneOver = NULL;

	m_pPeopleOrange = NULL;
	m_pPeopleGray   = NULL;

	m_eTeam = TEAM_BLUE;
	m_iSlotArray = m_iMaxSlot = 0;
	m_iGradeLevel = 0;
}

PartyBattleSimpleBtn::~PartyBattleSimpleBtn()
{
	SAFEDELETE( m_pBlueNormal );
	SAFEDELETE( m_pRedNormal );
	SAFEDELETE( m_pNoneNormal );
	SAFEDELETE( m_pBlueOver );
	SAFEDELETE( m_pRedOver );
	SAFEDELETE( m_pNoneOver );

	SAFEDELETE( m_pPeopleOrange );
	SAFEDELETE( m_pPeopleGray );
}

void PartyBattleSimpleBtn::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "BlueNormal" )
	{
		SAFEDELETE( m_pBlueNormal );
		m_pBlueNormal = pImage;
	}
	else if( szType == "RedNormal" )
	{
		SAFEDELETE( m_pRedNormal );
		m_pRedNormal = pImage;
	}
	else if( szType == "NoneNormal" )
	{
		SAFEDELETE( m_pNoneNormal );
		m_pNoneNormal = pImage;
	}
	else if( szType == "BlueOver" )
	{
		SAFEDELETE( m_pBlueOver );
		m_pBlueOver = pImage;
	}
	else if( szType == "RedOver" )
	{
		SAFEDELETE( m_pRedOver );
		m_pRedOver = pImage;
	}
	else if( szType == "NoneOver" )
	{
		SAFEDELETE( m_pNoneOver );
		m_pNoneOver = pImage;
	}
	else if( szType == "PeopleOrange" )
	{
		SAFEDELETE( m_pPeopleOrange );
		m_pPeopleOrange = pImage;
	}
	else if( szType == "PeopleGray" )
	{
		SAFEDELETE( m_pPeopleGray );
		m_pPeopleGray = pImage;
	}
	else
	{
		ioButton::AddRenderImage( szType, pImage );
	}
}

void PartyBattleSimpleBtn::iwm_create()
{
	m_PingStep.Create();
}

void PartyBattleSimpleBtn::SetMemberInfo( int iMaxSlot, int iSlotArray, const ioHashString &rkName, int iGradeLevel, TeamType eTeam )
{
	m_iMaxSlot   = iMaxSlot;
	m_iSlotArray = iSlotArray;
	m_szName     = rkName;
	m_eTeam      = eTeam;
	m_iGradeLevel= iGradeLevel;
}

void PartyBattleSimpleBtn::OnRenderBase( int iXPos, int iYPos )
{
	ioUIRenderImage *pNormal = NULL;
	ioUIRenderImage *pOver   = NULL;
	switch( m_eTeam )
	{
	case TEAM_BLUE:
		{
			pNormal = m_pBlueNormal;
			if( m_bOver || m_bClicked )
				pOver = m_pBlueOver;
		}
		break;
	case TEAM_RED:
		{
			pNormal = m_pRedNormal;
			if( m_bOver || m_bClicked )
				pOver = m_pRedOver;
		}
		break;
	default:
		{
			pNormal = m_pNoneNormal;
			if( m_bOver || m_bClicked )
				pOver = m_pNoneOver;
		}
		break;
	}

	if( pNormal )
		pNormal->Render( iXPos, iYPos );
	if( pOver )
		pOver->Render( iXPos, iYPos );
}

void PartyBattleSimpleBtn::OnRender()
{
	ioButton::OnRender();

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	OnRenderBase( iXPos, iYPos );

	g_FontMgr.SetTextStyle( TS_NORMAL );	
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	if( m_szName.IsEmpty() )
	{
		if( !m_pPeopleOrange || !m_pPeopleGray )
			return;

		if( m_iSlotArray < m_iMaxSlot )
		{
			if( m_bOver || m_bClicked )
			{
				m_pPeopleOrange->SetColor( 255, 255, 255 );
				m_pPeopleOrange->Render( iXPos + 4, iYPos + 2, UI_RENDER_NORMAL_ADD_COLOR );
				g_FontMgr.SetTextColor( 255, 255, 255 );
			}
			else
			{
				m_pPeopleOrange->Render( iXPos + 4, iYPos + 2 );
				if( m_eTeam == TEAM_BLUE )
					g_FontMgr.SetTextColor( TCT_DEFAULT_BLUE );
				else if( m_eTeam == TEAM_RED )
					g_FontMgr.SetTextColor( TCT_DEFAULT_RED );
				else
					g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
			}
			g_FontMgr.PrintText( iXPos + 23, iYPos + 3, FONT_SIZE_12, STR(1) );
		}
		else
		{
			if( m_bOver || m_bClicked )
			{
				m_pPeopleGray->SetColor( 255, 255, 255 );
				m_pPeopleGray->Render( iXPos + 4, iYPos + 2, UI_RENDER_NORMAL_ADD_COLOR );
				g_FontMgr.SetTextColor( 255, 255, 255 );
			}	
			else
			{
				m_pPeopleGray->Render( iXPos + 4, iYPos + 2 );
				g_FontMgr.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
			}
			g_FontMgr.PrintText( iXPos + 23, iYPos + 3, FONT_SIZE_12, STR(2) );	
		}
	}
	else
	{
		if( m_bOver || m_bClicked )
		{
			g_LevelMgr.RenderGrade( iXPos + 4, iYPos + 2, m_iGradeLevel, TEAM_PRIVATE_1, UI_RENDER_NORMAL_ADD_COLOR, RGB( 255, 255, 255 ) );
			g_FontMgr.SetTextColor( 255, 255, 255 );
		}
		else
		{
			g_LevelMgr.RenderGrade( iXPos + 4, iYPos + 2, m_iGradeLevel, TEAM_PRIVATE_1 );

			if( m_eTeam == TEAM_BLUE )
				g_FontMgr.SetTextColor( TCT_DEFAULT_BLUE );
			else if( m_eTeam == TEAM_RED )
				g_FontMgr.SetTextColor( TCT_DEFAULT_RED );
			else
				g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		}
		g_FontMgr.PrintTextWidthCut( iXPos + 23, iYPos + 3, FONT_SIZE_12, 103.0f, m_szName.c_str() );
		int iRealNameSize = min( 103.0f, g_FontMgr.GetTextWidth( m_szName.c_str(), TS_NORMAL, FONT_SIZE_12 ) );
		DWORD dwGuildIndex, dwGuildMark;
		g_UserInfoMgr.GetGuildInfo( m_szName, dwGuildIndex, dwGuildMark );
		g_GuildMarkMgr.RenderSmallMark( dwGuildIndex, dwGuildMark, iXPos + 23 + iRealNameSize + 3, iYPos + 4 );

		OnRenderPing( iXPos + 147, iYPos + 2 );
	}
}

void PartyBattleSimpleBtn::OnRenderPing( int iXPos, int iYPos )
{
	int  iPingStep = 0;
	bool bServerRelay = false;
	if( m_szName == g_MyInfo.GetPublicID() )
	{
		iPingStep = g_GlobalTimer.GetServerDelayMS() / PingStepWnd::PING_MS_VALUE;	
	}			
	else
	{
		iPingStep = abs(  P2PNetwork::GetP2pPingTime( m_szName ) ) / PingStepWnd::PING_MS_VALUE;
		if( P2PNetwork::IsServerRelay( m_szName ) )
			bServerRelay = true;
	}
	m_PingStep.RenderPing( iXPos, iYPos, iPingStep, bServerRelay );
}

void PartyBattleSimpleBtn::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;
	if( m_szName.IsEmpty() ) return;

	if( m_bOver && !m_bClicked )
	{
		ioLSMouse *pMouse = g_App.GetMouse();
		if( pMouse && pMouse->IsMouseShowing() )
			pMouse->SetToolTipID( m_szName, UserInfoToolTip::TT_GLOBAL );
	}	
}
//////////////////////////////////////////////////////////////////////////
PartyBattleSimpleWnd::PartyBattleSimpleWnd()
{
	m_pBottomFrm		= NULL;
	m_pModeIconBack		= NULL;
	m_pUserModeLine		= NULL;
	m_pUserModeBoxDark	= NULL;
	m_pUserModeBoxLight	= NULL;

	m_bIconOver			= false;
	m_pModeHelpTooltip  = NULL;
}

PartyBattleSimpleWnd::~PartyBattleSimpleWnd()
{
	SAFEDELETE( m_pBottomFrm );
	SAFEDELETE( m_pModeIconBack );
	SAFEDELETE( m_pUserModeLine );
	SAFEDELETE( m_pUserModeBoxDark );
	SAFEDELETE( m_pUserModeBoxLight );
}

void PartyBattleSimpleWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "BottomFrm" )
	{
		SAFEDELETE( m_pBottomFrm );
		m_pBottomFrm = pFrame;
	}
	else
	{
		ioWnd::AddRenderFrame( szType, pFrame );
	}
}

void PartyBattleSimpleWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "ModeIconBack" )
	{
		SAFEDELETE( m_pModeIconBack );
		m_pModeIconBack = pImage;
	}
	else if( szType == "UserModeLine" )
	{
		SAFEDELETE( m_pUserModeLine );
		m_pUserModeLine = pImage;
	}
	else if( szType == "UserModeBoxDark" )
	{
		SAFEDELETE( m_pUserModeBoxDark );
		m_pUserModeBoxDark = pImage;
	}
	else if( szType == "UserModeBoxLight" )
	{
		SAFEDELETE( m_pUserModeBoxLight );
		m_pUserModeBoxLight = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void PartyBattleSimpleWnd::UpdateUserList()
{
	bool bCoinVisible = g_ModeSelectInfo.IsCoinVisible( g_BattleRoomMgr.GetSelectModeIndex() );
	bool bFreeForAllMode = g_ModeSelectInfo.IsFreeForAllMode( g_BattleRoomMgr.GetSelectModeIndex() );

	// 유저 리스트 세팅
	int i = 0;
	//BLUE TEAM
	for(i = 0;i < MAX_PARTY_PLAYER/2;i++)
	{
		const BattleRoomUserInfo &kInfo = g_BattleRoomMgr.GetBattleUserInfo( TEAM_BLUE, i );
		PartyBattleSimpleBtn *pBlueBtn = dynamic_cast<PartyBattleSimpleBtn*>(FindChildWnd( ID_BLUE_TEAM_1 + i ));
		if( !pBlueBtn ) continue;

		TeamType eUITeam = TEAM_BLUE;	
		if( bFreeForAllMode || bCoinVisible )
			eUITeam = TEAM_NONE;
		pBlueBtn->SetMemberInfo( g_BattleRoomMgr.GetMaxPlayerBlue(), i, kInfo.m_szUserID, kInfo.m_iGradeLevel, eUITeam );
	}

	//RED TEAM
	for(i = 0;i < MAX_PARTY_PLAYER/2;i++)
	{
		const BattleRoomUserInfo &kInfo = g_BattleRoomMgr.GetBattleUserInfo( TEAM_RED, i );
		PartyBattleSimpleBtn *pRedBtn = dynamic_cast<PartyBattleSimpleBtn*>(FindChildWnd( ID_RED_TEAM_1 + i ));
		if( !pRedBtn ) continue;

		TeamType eUITeam = TEAM_RED;	
		if( bFreeForAllMode || bCoinVisible )
			eUITeam = TEAM_NONE;	
		pRedBtn->SetMemberInfo( g_BattleRoomMgr.GetMaxPlayerRed(), i, kInfo.m_szUserID, kInfo.m_iGradeLevel, eUITeam );
	}
}

void PartyBattleSimpleWnd::BottomButtonPos( DWORD dwID, int iBottomGap )
{
	ioWnd *pBottomBtn = FindChildWnd( dwID );
	if( pBottomBtn )
		pBottomBtn->SetWndPos( pBottomBtn->GetXPos(), GetHeight() - iBottomGap );
}

void PartyBattleSimpleWnd::UpdateBattleInfo()
{
	bool bExtraOption = g_BattleRoomMgr.IsUseExtraOption();
	int nMode = g_BattleRoomMgr.GetSelectModeIndex();

	if( ( nMode == MT_UNDERWEAR || nMode == MT_CBT || nMode == MT_FARMING ) )
		bExtraOption = false;

	UpdateUserList();

	if( bExtraOption )
	{
		SetSize( GetWidth(), USER_MODE_HEIGHT );
	}
	else
	{
		SetSize( GetWidth(), NORMAL_MODE_HEIGHT );
	}

	BottomButtonPos( ID_INVITE_LIST, BOTTOM_BTN_GAP );
	BottomButtonPos( ID_OBSERVER, BOTTOM_BTN_GAP );
	
	BottomButtonPos( ID_EXTRA_OPTION, BOTTOM_CHECK_BTN_GAP + 20 );
	BottomButtonPos( ID_RANDOM_TEAM, BOTTOM_CHECK_BTN_GAP + 20 );
	BottomButtonPos( ID_START_ENTER_X, BOTTOM_CHECK_BTN_GAP + 20 );
	BottomButtonPos( ID_NO_CHALLENGER, BOTTOM_CHECK_BTN_GAP + 20 );

	BottomButtonPos( ID_AUTO_START, BOTTOM_CHECK_BTN_GAP );
	BottomButtonPos( ID_BAD_PING_KICK, BOTTOM_CHECK_BTN_GAP );

	RECT rcDrag = { 0, 0, GetWidth(), GetHeight() };
	SetDragRect( rcDrag );

	// 비밀방
	char szPasswordRoom[MAX_PATH] = "";
	if( !g_BattleRoomMgr.GetPW().IsEmpty() )
		SafeSprintf( szPasswordRoom, sizeof( szPasswordRoom ), STR(1) );

	// UI TITLE	

	char szTitle[MAX_PATH] = "";
	if( bExtraOption )
		SafeSprintf( szTitle, sizeof( szTitle ), STR(2), g_ModeSelectInfo.GetCustomModeTitle().c_str(), g_BattleRoomMgr.GetCurPlayer(), g_BattleRoomMgr.GetMaxPlayer() );
	else
		SafeSprintf( szTitle, sizeof( szTitle ), STR(3), g_ModeSelectInfo.GetBattleModeTitle( nMode ).c_str(), g_BattleRoomMgr.GetCurPlayer(), g_BattleRoomMgr.GetMaxPlayer() );
	char szTitle2[MAX_PATH] = "";
	SafeSprintf( szTitle2, sizeof( szTitle2 ), STR(4), szTitle, szPasswordRoom );
	SetTitleText( szTitle2 );	

	//
	if( g_BattleRoomMgr.GetOwnerName() == g_MyInfo.GetPublicID() )
	{
		SetChildWndStyleRemove( ID_CHANGE_INFO, IWS_EXACTIVE );
	}
	else 
	{
		SetChildWndStyleAdd( ID_CHANGE_INFO, IWS_EXACTIVE );
	}
	
	// 
	ioCheckButton *pCheckBtn = (ioCheckButton*)FindChildWnd( ID_RANDOM_TEAM );
	if( pCheckBtn )
	{
		pCheckBtn->SetCheck( g_BattleRoomMgr.IsRandomTeamMode() );
		pCheckBtn->SetInActive();
	}

	pCheckBtn = (ioCheckButton*)FindChildWnd( ID_START_ENTER_X );
	if( pCheckBtn )
		pCheckBtn->SetCheck( g_BattleRoomMgr.IsStartRoomEnterX() );

	pCheckBtn = (ioCheckButton*)FindChildWnd( ID_NO_CHALLENGER );
	if( pCheckBtn )
		pCheckBtn->SetCheck( !g_BattleRoomMgr.IsNoChallenger() );

	pCheckBtn = (ioCheckButton*)FindChildWnd( ID_AUTO_START );
	if( pCheckBtn )
		pCheckBtn->SetCheck( g_BattleRoomMgr.IsAutoModeStart() );

	pCheckBtn = (ioCheckButton*)FindChildWnd( ID_BAD_PING_KICK );
	if( pCheckBtn )
	{
		pCheckBtn->SetCheck( g_BattleRoomMgr.IsBadPingKick() );
		pCheckBtn->SetInActive();
	}

	pCheckBtn = (ioCheckButton*)FindChildWnd( ID_EXTRA_OPTION );
	if( pCheckBtn )
	{
		pCheckBtn->SetCheck( g_BattleRoomMgr.IsUseExtraOption() );
		pCheckBtn->SetInActive();
	}

	if( g_BattleRoomMgr.IsSafetyLevelRoom() || g_BattleRoomMgr.IsObserverUser( g_MyInfo.GetPublicID() ) )
		SetChildWndStyleAdd( ID_INVITE_LIST, IWS_EXACTIVE );
	else
		SetChildWndStyleRemove( ID_INVITE_LIST, IWS_EXACTIVE );

	//
	if( g_RoomOptionMgr.IsNoChallengerMode(nMode) )
	{
		ShowChildWnd( ID_NO_CHALLENGER );
		SetChildActive( ID_NO_CHALLENGER );

		SetChildInActive( ID_START_ENTER_X );
		HideChildWnd( ID_START_ENTER_X );
	}
	else
	{
		ShowChildWnd( ID_START_ENTER_X );
		SetChildActive( ID_START_ENTER_X );

		SetChildInActive( ID_NO_CHALLENGER );
		HideChildWnd( ID_NO_CHALLENGER );
	}
	bool bAIMode = g_ModeSelectInfo.IsAIMode( nMode );
	if( bAIMode )
	{
		SetChildInActive( ID_START_ENTER_X );
		SetChildInActive( ID_AUTO_START );
		SetChildInActive( ID_OBSERVER );

		g_BattleRoomMgr.SetStartRoomEnterX( true );
		g_BattleRoomMgr.SetAutoModeStart( false );
	}
	else if( g_ModeSelectInfo.GetBattleMode(g_BattleRoomMgr.GetSelectModeIndex()) == MT_RAID /*|| g_ModeSelectInfo.GetBattleMode(g_BattleRoomMgr.GetSelectModeIndex()) == MT_BATTLE*/ )
	{
		SetChildInActive( ID_START_ENTER_X );

		g_BattleRoomMgr.SetStartRoomEnterX( true );
	}
	else if( g_ModeSelectInfo.GetBattleMode( g_BattleRoomMgr.GetSelectModeIndex() ) == MT_BATTLE )
	{
		SetChildInActive( ID_AUTO_START );
		SetChildInActive( ID_START_ENTER_X );

		g_BattleRoomMgr.SetStartRoomEnterX( true );
	}
	else
	{
		SetChildActive( ID_START_ENTER_X );
		SetChildActive( ID_AUTO_START );
		SetChildActive( ID_OBSERVER );
	}

	ioWnd *pObserverBtn = FindChildWnd( ID_OBSERVER );
	if( pObserverBtn )
	{
		char szObserverTitle[MAX_PATH] = "";
		SafeSprintf( szObserverTitle, sizeof( szObserverTitle ), STR(5), g_BattleRoomMgr.GetBattleTeamUserCount( TEAM_NONE ), g_BattleRoomMgr.GetMaxObserver() );
		pObserverBtn->SetTitleText( szObserverTitle );
	}
}

bool PartyBattleSimpleWnd::IsChangeMaxPlayerLimit( int iPrevModeIndex, int iPrevMapIndex )
{
	int iPrevDefaultMaxPlayer = g_ModeSelectInfo.GetBattleMapDefaultMaxPlayer( iPrevModeIndex, iPrevMapIndex );
	int iNextDefaultMaxPlayer = g_ModeSelectInfo.GetBattleMapDefaultMaxPlayer( g_BattleRoomMgr.GetSelectModeIndex(), g_BattleRoomMgr.GetSelectMapIndex() );
	return (iPrevDefaultMaxPlayer != iNextDefaultMaxPlayer );
}

bool PartyBattleSimpleWnd::IsMaxPlayerLimit( int iChangeMaxPlayer )
{
	//if( g_BattleRoomMgr.IsBattleModePlaying() || m_mode_start_time != 0 )
	{
		// 적은 수로 변경시는 가능하게하자.
		if( iChangeMaxPlayer < g_BattleRoomMgr.GetMaxPlayerBlue() + g_BattleRoomMgr.GetMaxPlayerRed() )
			return true;

		// 모드 플레이중이거나 모드 시작중에는 최대 인원 변경이 불가능.
		int iMapLimitMaxPlayer = g_ModeSelectInfo.GetBattleMapLimitMaxPlayer( g_BattleRoomMgr.GetSelectModeIndex(), g_BattleRoomMgr.GetSelectMapIndex() );
		if( iChangeMaxPlayer > iMapLimitMaxPlayer )
		{
			ioHashString szModeName = g_ModeSelectInfo.GetBattleModeTitle( g_BattleRoomMgr.GetSelectModeIndex() );
			ioHashString szMapName  = g_ModeSelectInfo.GetBattleMapTitle( g_BattleRoomMgr.GetSelectModeIndex(), g_BattleRoomMgr.GetSelectMapIndex() );
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2), szModeName.c_str(), szMapName.c_str(), iMapLimitMaxPlayer );
			return false;
		}		
	}

	return true;
}

void PartyBattleSimpleWnd::SendMaxPlayer( int iMaxBluePlayer, int iMaxRedPlayer, int iMaxObserver )
{
	TCPNetwork::MouseBusy( true );
	SP2Packet kPacket( CTPK_MACRO_COMMAND );
	kPacket << MACRO_PLAYER_CNT << iMaxBluePlayer << iMaxRedPlayer << iMaxObserver;
	TCPNetwork::SendToServer( kPacket );	
}

void PartyBattleSimpleWnd::iwm_show()
{	
	UpdateBattleInfo();
	SetWndPos( (Setting::Width()/2) - GetWidth()/2, ((Setting::Height()/2) - GetHeight()/2) - 20 );
}

void PartyBattleSimpleWnd::iwm_hide()
{
	ModeHelpTooltip *pModeHelpTooltip = dynamic_cast< ModeHelpTooltip * >( m_pModeHelpTooltip );	
	if( pModeHelpTooltip )
	{
		pModeHelpTooltip->SetModeHelpTooltip( NULL, "", -1 );
	}	
}

void PartyBattleSimpleWnd::SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate /* = false  */ )
{
	ioMovingWnd::SetPositionToDefault( iParentWidth, iParentHeight, bChildMove, bCreate );
	if( !bCreate )
	{
		if( IsShow() )
		{
			iwm_show();
		}
	}
}

void PartyBattleSimpleWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
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
	case ID_CHANGE_INFO:
		if( cmd == IOBN_BTNUP || cmd == IOEX_BTNUP )
		{
			if( g_BattleRoomMgr.GetOwnerName() != g_MyInfo.GetPublicID() )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
				return;
			}
			g_GUIMgr.ShowWnd( PARTY_OWNER_WND );
		}
		break;
	case ID_INVITE_LIST:
		if( cmd == IOBN_BTNUP || cmd == IOEX_BTNUP )
		{
			if( g_BattleRoomMgr.IsSafetyLevelRoom() )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
			}
			else if( g_BattleRoomMgr.IsObserverUser( g_MyInfo.GetPublicID() ) )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
			}
			else if( g_BattleRoomMgr.IsBattleModePlaying() && g_BattleRoomMgr.IsStartRoomEnterX() )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4) );
			}
			else
			{
				if( g_GUIMgr.IsShow( PARTY_INVITE_LIST_WND ) )
					g_GUIMgr.HideWnd( PARTY_INVITE_LIST_WND );
				else
					g_GUIMgr.ShowWnd( PARTY_INVITE_LIST_WND );
			}
		}
		break;
	case ID_OBSERVER:
		if( cmd == IOBN_BTNUP )
		{
			if( g_GUIMgr.IsShow( PARTY_OBSERVER_WND ) )
				g_GUIMgr.HideWnd( PARTY_OBSERVER_WND );
			else
				g_GUIMgr.ShowWnd( PARTY_OBSERVER_WND );
		}
		break;
	case ID_START_ENTER_X:
		if( cmd == IOBN_BTNUP )
		{
			if( g_BattleRoomMgr.GetOwnerName() == g_MyInfo.GetPublicID() )
			{
				if( g_ModeSelectInfo.IsStartEnterX( g_BattleRoomMgr.GetSelectModeIndex() ) )
					g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(5) );
				else
				{
					g_BattleRoomMgr.SetStartRoomEnterX( !g_BattleRoomMgr.IsStartRoomEnterX() );
					g_BattleRoomMgr.SendBattleRoomOption();
				}
			}
			else
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(6) );
			}
			UpdateBattleInfo();
		}
		break;
	case ID_NO_CHALLENGER:
		if( cmd == IOBN_BTNUP )
		{
			if( g_BattleRoomMgr.GetOwnerName() == g_MyInfo.GetPublicID() )
			{
				g_BattleRoomMgr.SetNoChallenger( !g_BattleRoomMgr.IsNoChallenger() );
				g_BattleRoomMgr.SendBattleRoomOption();
			}
			else
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(6) );
			}
			UpdateBattleInfo();
		}
		break;
	case ID_AUTO_START:
		if( cmd == IOBN_BTNUP )
		{
			if( g_BattleRoomMgr.GetOwnerName() == g_MyInfo.GetPublicID() )
			{
				g_BattleRoomMgr.SetAutoModeStart( !g_BattleRoomMgr.IsAutoModeStart() );
				g_BattleRoomMgr.SendBattleRoomOption();
			}
			else
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(8) );
			}
			UpdateBattleInfo();
		}
		break;
	case ID_BAD_PING_KICK:
		if( cmd == IOBN_BTNUP )
		{
			if( g_BattleRoomMgr.GetOwnerName() == g_MyInfo.GetPublicID() )
			{
				if( g_BattleRoomMgr.IsBattleModePlaying() )
					g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(9) );
				else
				{
					g_BattleRoomMgr.SetBadPingKick( !g_BattleRoomMgr.IsBadPingKick() );
					g_BattleRoomMgr.SendBattleRoomOption();
				}
			}
			else
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(10) );
			}
			UpdateBattleInfo();
		}
		break;
	case ID_BLUE_TEAM_1:
	case ID_BLUE_TEAM_2:
	case ID_BLUE_TEAM_3:
	case ID_BLUE_TEAM_4:
	case ID_BLUE_TEAM_5:
	case ID_BLUE_TEAM_6:
	case ID_BLUE_TEAM_7:
	case ID_BLUE_TEAM_8:
		if( cmd == IOBN_BTNUP )
		{
			PartyBattleSimpleBtn *pUserBtn = dynamic_cast<PartyBattleSimpleBtn*>(pWnd);
			if( pUserBtn->GetMemberName().IsEmpty() )
			{
				// 인원 변경	
				if( g_BattleRoomMgr.GetOwnerName() != g_MyInfo.GetPublicID() )
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(11) );			
					return;
				}

				// 닫기
				if( pUserBtn->GetSlotArray() < pUserBtn->GetMaxSlot() )
				{
					int iMaxPlayerTeam = pUserBtn->GetMaxSlot();
					int iMaxPlayer	   = pUserBtn->GetSlotArray();
					if( COMPARE( iMaxPlayer, 1, ( MAX_PARTY_PLAYER / 2 ) + 1 ) )
					{
						if( iMaxPlayer >= g_BattleRoomMgr.GetBattleTeamUserCount( TEAM_BLUE ) )
						{
							if( !IsMaxPlayerLimit( iMaxPlayer + g_BattleRoomMgr.GetMaxPlayerRed() ) )
									return;
							SendMaxPlayer( iMaxPlayer, g_BattleRoomMgr.GetMaxPlayerRed(), g_BattleRoomMgr.GetMaxObserver() );												
						}
					}		
				}
				else // 열기
				{
					int iMaxPlayerTeam = pUserBtn->GetMaxSlot();
					int iMaxPlayer     = pUserBtn->GetSlotArray() + 1;
					if( COMPARE( iMaxPlayer, 1, ( MAX_PARTY_PLAYER / 2 ) + 1 ) )
					{						
						if( iMaxPlayer >= g_BattleRoomMgr.GetBattleTeamUserCount( TEAM_BLUE ) )
						{
							if( !IsMaxPlayerLimit( iMaxPlayer + g_BattleRoomMgr.GetMaxPlayerRed() ) )
								return;
							SendMaxPlayer( iMaxPlayer, g_BattleRoomMgr.GetMaxPlayerRed(), g_BattleRoomMgr.GetMaxObserver() );	
						}
					}	
				}
			}
			else
			{
				g_App.OpenMannerWnd( pUserBtn->GetMemberName().c_str(), true );
			}
		}
		break;
	case ID_RED_TEAM_1:
	case ID_RED_TEAM_2:
	case ID_RED_TEAM_3:
	case ID_RED_TEAM_4:
	case ID_RED_TEAM_5:
	case ID_RED_TEAM_6:
	case ID_RED_TEAM_7:
	case ID_RED_TEAM_8:
		if( cmd == IOBN_BTNUP )
		{
			PartyBattleSimpleBtn *pUserBtn = dynamic_cast<PartyBattleSimpleBtn*>(pWnd);
			if( pUserBtn->GetMemberName().IsEmpty() )
			{
				// 인원 변경	
				if( g_BattleRoomMgr.GetOwnerName() != g_MyInfo.GetPublicID() )
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(12) );			
					return;
				}

				// 닫기
				if( pUserBtn->GetSlotArray() < pUserBtn->GetMaxSlot() )
				{
					int iMaxPlayerTeam = pUserBtn->GetMaxSlot();
					int iMaxPlayer	   = pUserBtn->GetSlotArray();
					if( COMPARE( iMaxPlayer, 1, ( MAX_PARTY_PLAYER / 2 ) + 1 ) )
					{
						if( iMaxPlayer >= g_BattleRoomMgr.GetBattleTeamUserCount( TEAM_RED ) )
						{
							if( !IsMaxPlayerLimit( g_BattleRoomMgr.GetMaxPlayerBlue() + iMaxPlayer ) )
								return;
							SendMaxPlayer( g_BattleRoomMgr.GetMaxPlayerBlue(), iMaxPlayer, g_BattleRoomMgr.GetMaxObserver() );												
						}
					}		
				}
				else // 열기
				{
					int iMaxPlayerTeam = pUserBtn->GetMaxSlot();
					int iMaxPlayer     = pUserBtn->GetSlotArray() + 1;
					if( COMPARE( iMaxPlayer, 1, ( MAX_PARTY_PLAYER / 2 ) + 1 ) )
					{						
						if( iMaxPlayer >= g_BattleRoomMgr.GetBattleTeamUserCount( TEAM_RED ) )
						{
							if( !IsMaxPlayerLimit( g_BattleRoomMgr.GetMaxPlayerBlue() + iMaxPlayer ) )
								return;
							SendMaxPlayer( g_BattleRoomMgr.GetMaxPlayerBlue(), iMaxPlayer, g_BattleRoomMgr.GetMaxObserver() );	
						}
					}	
				}
			}
			else
			{
				g_App.OpenMannerWnd( pUserBtn->GetMemberName().c_str(), true );
			}
		}
		break;
	}
}

void PartyBattleSimpleWnd::OnRender()
{
	ioMovingWnd::OnRender();

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	if( m_pBottomFrm )
	{
		m_pBottomFrm->Render( iXPos, iYPos + ( GetHeight() - BOTTOM_BAR_GAP ), UI_RENDER_MULTIPLY );
	}

	if( m_pModeIconBack )
	{
		m_pModeIconBack->SetScale( 0.60f, 0.60f );
		m_pModeIconBack->Render( iXPos + 39, iYPos + 70, UI_RENDER_MULTIPLY, TFO_BILINEAR );

		// 모드 아이콘
		ioHashString kModeTitle = g_ModeSelectInfo.GetBattleModeTitle( g_BattleRoomMgr.GetSelectModeIndex() );
		bool bAIMode = g_ModeSelectInfo.IsAIMode( g_BattleRoomMgr.GetSelectModeIndex() );
		if( bAIMode )
		{
			if( g_BattleRoomMgr.GetAiLevel() == 0 )
				kModeTitle += "(쉬움)";
			else
				kModeTitle += "(보통)";

		}

		ioUIRenderImage *pModeIcon = g_ModeSelectInfo.GetListModeUIResource( kModeTitle );

		if( pModeIcon )
		{
			pModeIcon->SetScale( 0.60f, 0.60f );
			pModeIcon->Render( iXPos + 39, iYPos + 70, UI_RENDER_NORMAL, TFO_BILINEAR );
			if( m_bIconOver )
				pModeIcon->Render( iXPos + 39, iYPos + 70, UI_RENDER_SCREEN, TFO_BILINEAR );
		}
		else
		{
			ioHashString kModeTitle = g_ModeSelectInfo.GetBattleMapTitle( g_BattleRoomMgr.GetSelectModeIndex(), g_BattleRoomMgr.GetSelectMapIndex() );
			ioUIRenderImage *pModeIcon = g_ModeSelectInfo.GetListModeUIResource( kModeTitle );
			if( pModeIcon )
			{
				pModeIcon->SetScale( 0.60f, 0.60f );
				pModeIcon->Render( iXPos + 39, iYPos + 70, UI_RENDER_NORMAL, TFO_BILINEAR );
				if( m_bIconOver )
					pModeIcon->Render( iXPos + 39, iYPos + 70, UI_RENDER_SCREEN, TFO_BILINEAR );
			}
		}
	}
	g_FontMgr.SetTextStyle( TS_NORMAL );	
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintTextWidthCut( iXPos + 65, iYPos + 55, FONT_SIZE_13, 300.0f, g_BattleRoomMgr.GetName().c_str() );

	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( iXPos + 65, iYPos + 73, FONT_SIZE_11, STR(1) );
    
	bool bExtraOption = g_BattleRoomMgr.IsUseExtraOption();
	int nMode = g_BattleRoomMgr.GetSelectModeIndex();

	if( ( nMode == MT_UNDERWEAR || nMode == MT_CBT || nMode == MT_FARMING ) )
		bExtraOption = false;

	if( bExtraOption )
	{
		OnRenderUserMode( iXPos, iYPos );
	}
}

void PartyBattleSimpleWnd::OnRenderUserMode( int iXPos, int iYPos )
{
	g_FontMgr.SetTextStyle( TS_NORMAL );	
	g_FontMgr.SetAlignType( TAT_CENTER);
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
	g_FontMgr.PrintText( iXPos + 191, iYPos + 277, FONT_SIZE_11, STR(1) );

	if( m_pUserModeLine )
		m_pUserModeLine->Render( iXPos + 18, iYPos + 282 );

	if( !m_pUserModeBoxDark || !m_pUserModeBoxLight )
		return;

	int i,j;
	enum { MAX_HORZ = 4, MAX_VERT = 6, XGAP = 87, YGAP = 19, };
	for(i = 0;i < MAX_HORZ;i++)
	{
		for(j = 0;j < MAX_VERT;j++)
		{
			if( j%2 == 1 )
				m_pUserModeBoxLight->Render( iXPos + 18 + (i*XGAP), iYPos + 294 + (j*YGAP) );
			else
				m_pUserModeBoxDark->Render( iXPos + 18 + (i*XGAP), iYPos + 294 + (j*YGAP) );
		}
	}

	// 유저 모드 옵션 내용
	enum { GLOBAL_HORZ = 2, GLOBAL_VERT = 6, TEAM_HORZ = 2, TEAM_VERT = 4, };
	for(i = 0;i < GLOBAL_HORZ;i++)
	{
		for(j = 0;j < GLOBAL_VERT;j++)
		{
			int iType = UMO_GROWTH_USE + j + (i * GLOBAL_VERT);
			if( IsDefatulUserModeType( iType ) )
				g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY2 );
			else
				g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
			g_FontMgr.PrintText( iXPos + 60 + (i* XGAP), iYPos + 297 + (j*YGAP), FONT_SIZE_11, GetUserModeDesc( iType ).c_str() );
		}
	}

	for(i = 0;i < TEAM_HORZ;i++)
	{
		for(j = 0;j < TEAM_VERT;j++)
		{
			int iType = UMO_BLUE_HP + j + (i * TEAM_VERT);
			if( IsDefatulUserModeType( iType ) )
				g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY2 );
			else
			{
				if( COMPARE( iType, UMO_BLUE_HP, UMO_RED_HP ) )
					g_FontMgr.SetTextColor( TCT_DEFAULT_BLUE );
				else
					g_FontMgr.SetTextColor( TCT_DEFAULT_RED );
			}
			g_FontMgr.PrintText( iXPos + 234 + (i* XGAP), iYPos + 297 + (j*YGAP), FONT_SIZE_11, GetUserModeDesc( iType ).c_str() );
		}
	}
}

ioHashString PartyBattleSimpleWnd::GetUserModeDesc( int iType )
{
	switch( iType )
	{
	case UMO_CHANGE_CHAR:
		return g_RoomOptionMgr.GetChangeCharDesc( g_BattleRoomMgr.GetChangeCharType(), true );
	case UMO_COOL_TIME:
		return g_RoomOptionMgr.GetCoolTimeDesc( g_BattleRoomMgr.GetCoolTimeType(), true );
	case UMO_DROP_DAMAGE:
		return g_RoomOptionMgr.GetDropDamageDesc( g_BattleRoomMgr.GetDropDamageType(), true );
	case UMO_TEAM_ATTACK:
		return g_RoomOptionMgr.GetTeamAttackDesc( g_BattleRoomMgr.GetTeamAttackType(), true );
	case UMO_GRAVITY:
		return g_RoomOptionMgr.GetGravityDesc( g_BattleRoomMgr.GetGravityType(), true );
	case UMO_GET_UP_DELAY:
		return g_RoomOptionMgr.GetGetUpDesc( g_BattleRoomMgr.GetGetUpType(), true );
	case UMO_KO:
		return g_RoomOptionMgr.GetKODesc( g_BattleRoomMgr.GetKOType(), true );
	case UMO_KO_EFFECT:
		return g_RoomOptionMgr.GetKOEffectDesc( g_BattleRoomMgr.GetKOEffectType(), true );
	case UMO_ROUND_TYPE:
		return g_RoomOptionMgr.GetRoundTypeDesc( g_BattleRoomMgr.GetRoundType(), true );
	case UMO_ROUND_TIME:
		return g_RoomOptionMgr.GetRoundTimeTypeDesc( g_BattleRoomMgr.GetRoundTimeType(), true );
	case UMO_BLUE_HP:
		return g_RoomOptionMgr.GetBlueHPDesc( g_BattleRoomMgr.GetBlueHPType(), true );
	case UMO_BLUE_MOVE_SPD:
		return g_RoomOptionMgr.GetBlueMoveSpeedDesc( g_BattleRoomMgr.GetBlueMoveSpeedType(), true );
	case UMO_BLUE_EQUIP:
		return g_RoomOptionMgr.GetBlueEquipDesc( g_BattleRoomMgr.GetBlueEquipType(), true );
	case UMO_BLUE_BLOW:
		return g_RoomOptionMgr.GetBlueBlowDesc( g_BattleRoomMgr.GetBlueBlowType(), true );
	case UMO_RED_HP:
		return g_RoomOptionMgr.GetRedHPDesc( g_BattleRoomMgr.GetRedHPType(), true );
	case UMO_RED_MOVE_SPD:
		return g_RoomOptionMgr.GetRedMoveSpeedDesc( g_BattleRoomMgr.GetRedMoveSpeedType(), true );
	case UMO_RED_EQUIP:
		return g_RoomOptionMgr.GetRedEquipDesc( g_BattleRoomMgr.GetRedEquipType(), true );
	case UMO_RED_BLOW:
		return g_RoomOptionMgr.GetRedBlowDesc( g_BattleRoomMgr.GetRedBlowType(), true );
	case UMO_GROWTH_USE:
		return g_RoomOptionMgr.GetGrowthUseDesc( g_BattleRoomMgr.GetGrowthUseType(), true );
	case UMO_EXTRAITEM_USE:
		return g_RoomOptionMgr.GetExtraItemUseDesc( g_BattleRoomMgr.GetExtraItemUseType(), true );
	}
	return ioHashString();
}

bool PartyBattleSimpleWnd::IsDefatulUserModeType( int iType )
{
	PreSetInfo kDefaultInfo;
	g_RoomOptionMgr.GetDefaultInfo( kDefaultInfo );

	switch( iType )
	{
	case UMO_CHANGE_CHAR:
		if( kDefaultInfo.m_iChangeCharType == g_BattleRoomMgr.GetChangeCharType() )
			return true;
		return false;
	case UMO_COOL_TIME:
		if( kDefaultInfo.m_iCoolTimeType == g_BattleRoomMgr.GetCoolTimeType() )
			return true;
		return false;
	case UMO_DROP_DAMAGE:
		if( kDefaultInfo.m_iDropDamageType == g_BattleRoomMgr.GetDropDamageType() )
			return true;
		return false;
	case UMO_TEAM_ATTACK:
		if( kDefaultInfo.m_iTeamAttackType == g_BattleRoomMgr.GetTeamAttackType() )
			return true;
		return false;
	case UMO_GRAVITY:
		if( kDefaultInfo.m_iGravityType == g_BattleRoomMgr.GetGravityType() )
			return true;
		return false;
	case UMO_GET_UP_DELAY:
		if( kDefaultInfo.m_iGetUpType == g_BattleRoomMgr.GetGetUpType() )
			return true;
		return false;
	case UMO_KO:
		if( kDefaultInfo.m_iKOType == g_BattleRoomMgr.GetKOType() )
			return true;
		return false;
	case UMO_KO_EFFECT:
		if( kDefaultInfo.m_iKOEffectType == g_BattleRoomMgr.GetKOEffectType() )
			return true;
		return false;
	case UMO_ROUND_TYPE:
		if( kDefaultInfo.m_iCatchModeRoundType == g_BattleRoomMgr.GetRoundType() )
			return true;
		return false;
	case UMO_ROUND_TIME:
		if( kDefaultInfo.m_iCatchModeRoundTimeType == g_BattleRoomMgr.GetRoundTimeType() )
			return true;
		return false;
	case UMO_BLUE_HP:
		if( kDefaultInfo.m_iBlueHPType == g_BattleRoomMgr.GetBlueHPType() )
			return true;
		return false;
	case UMO_BLUE_MOVE_SPD:
		if( kDefaultInfo.m_iBlueMoveSpeedType == g_BattleRoomMgr.GetBlueMoveSpeedType() )
			return true;
		return false;
	case UMO_BLUE_EQUIP:
		if( kDefaultInfo.m_iBlueEquipType == g_BattleRoomMgr.GetBlueEquipType() )
			return true;
		return false;
	case UMO_BLUE_BLOW:
		if( kDefaultInfo.m_iBlueBlowType == g_BattleRoomMgr.GetBlueBlowType() )
			return true;
		return false;
	case UMO_RED_HP:
		if( kDefaultInfo.m_iRedHPType == g_BattleRoomMgr.GetRedHPType() )
			return true;
		return false;
	case UMO_RED_MOVE_SPD:
		if( kDefaultInfo.m_iRedMoveSpeedType == g_BattleRoomMgr.GetRedMoveSpeedType() )
			return true;
		return false;
	case UMO_RED_EQUIP:
		if( kDefaultInfo.m_iRedEquipType == g_BattleRoomMgr.GetRedEquipType() )
			return true;
		return false;
	case UMO_RED_BLOW:
		if( kDefaultInfo.m_iRedBlowType == g_BattleRoomMgr.GetRedBlowType() )
			return true;
		return false;
	case UMO_GROWTH_USE:
		if( kDefaultInfo.m_iGrowthUseType == g_BattleRoomMgr.GetGrowthUseType() )
			return true;
		return false;
	case UMO_EXTRAITEM_USE:
		if( kDefaultInfo.m_iExtraItemUseType == g_BattleRoomMgr.GetExtraItemUseType() )
			return true;
		return false;
	}
	return false;
}

void PartyBattleSimpleWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )	return;

	ioMovingWnd::OnProcess( fTimePerSec );
	ProcessModeHelpTooltip();
}

void PartyBattleSimpleWnd::ProcessModeHelpTooltip()
{
	if( m_pModeHelpTooltip == NULL )
		m_pModeHelpTooltip = dynamic_cast< ModeHelpTooltip * >( FindChildWnd( ID_MODE_HELP_TOOLTIP ) );

	ModeHelpTooltip *pModeHelpTooltip = dynamic_cast< ModeHelpTooltip * >( m_pModeHelpTooltip );	
	if( pModeHelpTooltip )
	{
		m_bIconOver = false;

		if( g_App.IsMouseRender() )
		{
			if( g_GUIMgr.GetPreOverWnd() == this ||
				g_GUIMgr.GetPreClickWnd() == this )
			{
				ioLSMouse *pMouse = g_App.GetMouse();

				enum { STARTX = 17, STARTY = 48, WIDTH = 45, HEIGHT = 45, };
				RECT rcIconRect;
				rcIconRect.left   = GetDerivedPosX() + STARTX;
				rcIconRect.top    = GetDerivedPosY() + STARTY;
				rcIconRect.right  = rcIconRect.left + WIDTH;
				rcIconRect.bottom = rcIconRect.top + HEIGHT;
				if( ::PtInRect( &rcIconRect, pMouse->GetMousePos() ) )
				{
					m_bIconOver = true;	
				}
			}
		}    

		if( m_bIconOver )
		{
			pModeHelpTooltip->SetModeHelpTooltip( this, g_ModeSelectInfo.GetBattleModeTitle( g_BattleRoomMgr.GetSelectModeIndex() ),
												  g_ModeSelectInfo.GetBattleModeHelpIndex( g_BattleRoomMgr.GetSelectModeIndex() ) );
		}
		else
		{
			pModeHelpTooltip->SetModeHelpTooltip( NULL, "", -1 );
		}
	}	
}
//////////////////////////////////////////////////////////////////////////
PartyObserverBtn::PartyObserverBtn()
{
	m_pPeopleOrange = NULL;
	m_pPeopleGray   = NULL;

	m_iSlotArray = m_iMaxSlot = 0;
	m_iGradeLevel = 0;
}

PartyObserverBtn::~PartyObserverBtn()
{
	SAFEDELETE( m_pPeopleOrange );
	SAFEDELETE( m_pPeopleGray );
}

void PartyObserverBtn::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "PeopleOrange" )
	{
		SAFEDELETE( m_pPeopleOrange );
		m_pPeopleOrange = pImage;
	}
	else if( szType == "PeopleGray" )
	{
		SAFEDELETE( m_pPeopleGray );
		m_pPeopleGray = pImage;
	}
	else
	{
		ioButton::AddRenderImage( szType, pImage );
	}
}

void PartyObserverBtn::SetMemberInfo( int iMaxSlot, int iSlotArray, const ioHashString &rkName, int iGradeLevel )
{
	m_iMaxSlot   = iMaxSlot;
	m_iSlotArray = iSlotArray;
	m_szName     = rkName;
	m_iGradeLevel= iGradeLevel;
}

void PartyObserverBtn::OnRender()
{
	ioButton::OnRender();

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	g_FontMgr.SetTextStyle( TS_NORMAL );	
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	if( m_szName.IsEmpty() )
	{
		if( !m_pPeopleOrange || !m_pPeopleGray )
			return;

		if( m_iSlotArray < m_iMaxSlot )
		{
			if( m_bOver || m_bClicked )
			{
				m_pPeopleOrange->SetColor( 255, 255, 255 );
				m_pPeopleOrange->Render( iXPos + 4, iYPos + 2, UI_RENDER_NORMAL_ADD_COLOR );
				g_FontMgr.SetTextColor( 255, 255, 255 );
			}
			else
			{
				m_pPeopleOrange->Render( iXPos + 4, iYPos + 2 );
                g_FontMgr.SetTextColor( TCT_DEFAULT_ORANGE );
			}
			g_FontMgr.PrintText( iXPos + 23, iYPos + 3, FONT_SIZE_12, STR(1) );
		}
		else
		{
			if( m_bOver || m_bClicked )
			{
				m_pPeopleGray->SetColor( 255, 255, 255 );
				m_pPeopleGray->Render( iXPos + 4, iYPos + 2, UI_RENDER_NORMAL_ADD_COLOR );
				g_FontMgr.SetTextColor( 255, 255, 255 );
			}	
			else
			{
				m_pPeopleGray->Render( iXPos + 4, iYPos + 2 );
				g_FontMgr.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
			}
			g_FontMgr.PrintText( iXPos + 23, iYPos + 3, FONT_SIZE_12, STR(2) );	
		}
	}
	else
	{
		if( m_bOver || m_bClicked )
		{
			g_LevelMgr.RenderGrade( iXPos + 4, iYPos + 2, m_iGradeLevel, TEAM_PRIVATE_1, UI_RENDER_NORMAL_ADD_COLOR, RGB( 255, 255, 255 ) );
			g_FontMgr.SetTextColor( 255, 255, 255 );
		}
		else
		{
			g_LevelMgr.RenderGrade( iXPos + 4, iYPos + 2, m_iGradeLevel, TEAM_PRIVATE_1 );
			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		}
		g_FontMgr.PrintTextWidthCut( iXPos + 23, iYPos + 3, FONT_SIZE_12, 128.0f, m_szName.c_str() );
		int iRealNameSize = min( 128.0f, g_FontMgr.GetTextWidth( m_szName.c_str(), TS_NORMAL, FONT_SIZE_12 ) );
		DWORD dwGuildIndex, dwGuildMark;
		g_UserInfoMgr.GetGuildInfo( m_szName, dwGuildIndex, dwGuildMark );
		g_GuildMarkMgr.RenderSmallMark( dwGuildIndex, dwGuildMark, iXPos + 23 + iRealNameSize + 3, iYPos + 4 );
	}
}

void PartyObserverBtn::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;
	if( m_szName.IsEmpty() ) return;

	if( m_bOver && !m_bClicked )
	{
		ioLSMouse *pMouse = g_App.GetMouse();
		if( pMouse && pMouse->IsMouseShowing() )
			pMouse->SetToolTipID( m_szName, UserInfoToolTip::TT_GLOBAL );
	}	
}
//////////////////////////////////////////////////////////////////////////
PartyObserverWnd::PartyObserverWnd()
{
	m_iObserverUserCount = 0;
	m_bModeStart = false;
}

PartyObserverWnd::~PartyObserverWnd()
{

}

bool PartyObserverWnd::IsObserverLimit()
{
	if( g_BattleRoomMgr.IsBattleModePlaying() )
		return true;

	PartyShamBattleWnd *pBattleWnd = dynamic_cast<PartyShamBattleWnd*>(g_GUIMgr.FindWnd( PARTY_SHAM_BATTLE_WND ));
	if( pBattleWnd && pBattleWnd->IsShow() && pBattleWnd->IsModeStart() )
		return true;

	if( m_iObserverUserCount >= g_BattleRoomMgr.GetMaxObserver() )
		return true;

	if( g_BattleRoomMgr.IsObserverUser( g_MyInfo.GetPublicID() ) )
		return true;

	return false;
}

bool PartyObserverWnd::IsCancelLimit()
{
	if( g_BattleRoomMgr.IsBattleModePlaying() )
		return true;

	PartyShamBattleWnd *pBattleWnd = dynamic_cast<PartyShamBattleWnd*>(g_GUIMgr.FindWnd( PARTY_SHAM_BATTLE_WND ));
	if( pBattleWnd && pBattleWnd->IsShow() && pBattleWnd->IsModeStart() )
		return true;

	return false;
}

void PartyObserverWnd::UpdateObserver()
{
	m_iObserverUserCount = g_BattleRoomMgr.GetBattleTeamUserCount( TEAM_NONE );
	m_iMaxObserver       = g_BattleRoomMgr.GetMaxObserver();
	for(int i = 0;i < MAX_OBSERVER;i++)
	{
		const BattleRoomUserInfo &kInfo = g_BattleRoomMgr.GetBattleObserverInfo( i );
		PartyObserverBtn *pObserverBtn = dynamic_cast<PartyObserverBtn*>(FindChildWnd( ID_OBSERVER_1 + i ));
		if( !pObserverBtn ) continue;

		pObserverBtn->SetMemberInfo( g_BattleRoomMgr.GetMaxObserver(), i, kInfo.m_szUserID, kInfo.m_iGradeLevel );
	}
	
	if( IsObserverLimit() )
		SetChildWndStyleAdd( ID_OBSERVER, IWS_EXACTIVE );
	else
		SetChildWndStyleRemove( ID_OBSERVER, IWS_EXACTIVE );

	//
	if( g_BattleRoomMgr.IsObserverUser( g_MyInfo.GetPublicID() ) )
	{
		HideChildWnd( ID_OBSERVER );
		ShowChildWnd( ID_CANCEL );
	}
	else
	{
		ShowChildWnd( ID_OBSERVER );
		HideChildWnd( ID_CANCEL );
	}

	if( IsCancelLimit() )
	{
		SetChildWndStyleAdd( ID_CANCEL, IWS_EXACTIVE );
		m_bModeStart = true;
	}
	else
	{
		SetChildWndStyleRemove( ID_CANCEL, IWS_EXACTIVE );		
		m_bModeStart = false;
	}
}

void PartyObserverWnd::SendMaxPlayer( int iMaxBluePlayer, int iMaxRedPlayer, int iMaxObserver )
{
	TCPNetwork::MouseBusy( true );
	SP2Packet kPacket( CTPK_MACRO_COMMAND );
	kPacket << MACRO_PLAYER_CNT << iMaxBluePlayer << iMaxRedPlayer << iMaxObserver;
	TCPNetwork::SendToServer( kPacket );	
}

void PartyObserverWnd::iwm_show()
{
	UpdateObserver();
}

void PartyObserverWnd::iwm_hide()
{

}

void PartyObserverWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
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
	case ID_OBSERVER:
		if( cmd == IOBN_BTNUP || cmd == IOEX_BTNUP )
		{			
			if( g_BattleRoomMgr.IsBattleModePlaying() )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
			}
			else if( m_iObserverUserCount >= g_BattleRoomMgr.GetMaxObserver() )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
			}
			else if( g_BattleRoomMgr.IsObserverUser( g_MyInfo.GetPublicID() ) )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
			}
			else if( IsObserverLimit() )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4) );
			}
			else
			{
				SP2Packet kPacket( CTPK_BATTLEROOM_COMMAND );
				kPacket << BATTLEROOM_TEAM_CHANGE << TEAM_NONE;
				TCPNetwork::SendToServer( kPacket );
				TCPNetwork::MouseBusy( true );

				g_GUIMgr.HideWnd( PARTY_INVITE_LIST_WND );
			}
		}
		break;
	case ID_CANCEL:
		if( cmd == IOBN_BTNUP || cmd == IOEX_BTNUP )
		{			
			if( g_BattleRoomMgr.IsBattleModePlaying() )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(5) );
			}
			else
			{
				PartyShamBattleWnd *pBattleWnd = dynamic_cast<PartyShamBattleWnd*>(g_GUIMgr.FindWnd( PARTY_SHAM_BATTLE_WND ));
				if( pBattleWnd && pBattleWnd->IsShow() )
				{
					pBattleWnd->OnChangeTeam();
				}
			}
		}
		break;
	case ID_OBSERVER_1:
	case ID_OBSERVER_2:
	case ID_OBSERVER_3:
	case ID_OBSERVER_4:
		if( cmd == IOBN_BTNUP )
		{			
			PartyObserverBtn *pUserBtn = dynamic_cast<PartyObserverBtn*>(pWnd);
			if( pUserBtn->GetMemberName().IsEmpty() )
			{
				// 인원 변경	
				if( g_BattleRoomMgr.GetOwnerName() != g_MyInfo.GetPublicID() )
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(6) );			
					return;
				}

				// 닫기
				if( pUserBtn->GetSlotArray() < pUserBtn->GetMaxSlot() )
				{
					int iMaxPlayerTeam = pUserBtn->GetMaxSlot();
					int iMaxPlayer	   = pUserBtn->GetSlotArray();
					if( COMPARE( iMaxPlayer, 0, MAX_OBSERVER + 1 ) ) 
					{
						if( iMaxPlayer >= g_BattleRoomMgr.GetBattleTeamUserCount( TEAM_NONE ) )
						{
							SendMaxPlayer( g_BattleRoomMgr.GetMaxPlayerBlue(), g_BattleRoomMgr.GetMaxPlayerRed(), iMaxPlayer );
						}
					}		
				}
				else // 열기
				{
					int iMaxPlayerTeam = pUserBtn->GetMaxSlot();
					int iMaxPlayer     = pUserBtn->GetSlotArray() + 1;
					if( COMPARE( iMaxPlayer, 0, MAX_OBSERVER + 1 ) ) 
					{						
						if( iMaxPlayer >= g_BattleRoomMgr.GetBattleTeamUserCount( TEAM_NONE ) )
						{
							SendMaxPlayer( g_BattleRoomMgr.GetMaxPlayerBlue(), g_BattleRoomMgr.GetMaxPlayerRed(), iMaxPlayer );
						}
					}	
				}
			}
			else
			{
				if( g_BattleRoomMgr.IsBattleModePlaying() ||
					g_BattleRoomMgr.GetOwnerName() != g_MyInfo.GetPublicID() )
				{
					g_App.OpenMannerWnd( pUserBtn->GetMemberName().c_str(), true );
				}
				else
				{
					PartyObserverBtn *pSlotWnd = dynamic_cast<PartyObserverBtn*>(pWnd);
					if( pSlotWnd )
					{
						BattleUserOptionWnd *pBattleUserOption = dynamic_cast<BattleUserOptionWnd*>( FindChildWnd( ID_USER_SLOT_OPTION ) );
						if( pBattleUserOption )
							pBattleUserOption->SetOptionInfo( pSlotWnd->GetMemberName(), TEAM_NONE, pSlotWnd->GetSlotArray() );
					}
				}
			}
		}
		break;
	}
}

void PartyObserverWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )	return;

	ioMovingWnd::OnProcess( fTimePerSec );

	if( m_iObserverUserCount != g_BattleRoomMgr.GetBattleTeamUserCount( TEAM_NONE ) ||
		m_iMaxObserver != g_BattleRoomMgr.GetMaxObserver() ||
		m_bModeStart != IsCancelLimit() )
	{
		UpdateObserver();
	}
}

void PartyObserverWnd::OnRender()
{
	ioWnd::OnRender();
}
//////////////////////////////////////////////////////////////////////////
PartyOwnerWnd::PartyOwnerWnd() : m_pNameEdit( NULL ), m_pPWEdit( NULL ), m_pPreEdit( NULL )
{
}

PartyOwnerWnd::~PartyOwnerWnd()
{

}

void PartyOwnerWnd::iwm_show()
{
	m_pPreEdit = ioEdit::m_pKeyFocusEdit;

	m_szPartyName = g_BattleRoomMgr.GetName();
	m_szPartyPW   = g_BattleRoomMgr.GetPW();
	//
	m_pPWEdit = FindChildWnd( ID_EDIT_PW );
	if( m_pPWEdit )
	{
		ioEdit *pEdit = (ioEdit*)m_pPWEdit;
		pEdit->ClearString();
		pEdit->SetKeyFocus();
		g_InputBox.SetString( m_szPartyPW.c_str() );
	}

	m_pNameEdit = FindChildWnd( ID_EDIT_NAME );
	if( m_pNameEdit )
	{
		ioEdit *pEdit = (ioEdit*)m_pNameEdit;
		pEdit->ClearString();
		pEdit->SetKeyFocus();
		g_InputBox.SetString( m_szPartyName.c_str() );
	}
}

void PartyOwnerWnd::iwm_hide()
{
	if( m_pPWEdit )
	{
		ioEdit *pEdit = (ioEdit*)m_pPWEdit;
		pEdit->KillKeyFocus();
	}
	if( m_pNameEdit )
	{
		ioEdit *pEdit = (ioEdit*)m_pNameEdit;
		pEdit->KillKeyFocus();
	}
	if( m_pPreEdit )	
	{
		ioWnd *pParent = m_pPreEdit->GetParent();
		if( pParent )
		{
			if( pParent->IsShow() )
				((ioEdit*)m_pPreEdit)->SetKeyFocus();	
		}
	}	
	m_pPreEdit = NULL;
}

void PartyOwnerWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{	
	case ID_EXIT:
	case ID_CANCEL:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;
	case ID_CHANGE:
		if( cmd == IOBN_BTNUP )
		{
			ChangeBattleRoomInfo();
		}
		break;
	case ID_EDIT_NAME:
		if( cmd == IOED_INPUT )
		{
			if( param == 0 )
			{
				if( m_pPWEdit )
				{
					ioEdit *pPWEdit = (ioEdit*)m_pPWEdit;
					pPWEdit->SetKeyFocus();
				}
			}			
			else if( param == 1 )
			{
				ChangeBattleRoomInfo();
			}
		}
		break;
	case ID_EDIT_PW:
		if( cmd == IOED_INPUT )
		{
			if( param == 0 )
			{
				if( m_pNameEdit )
				{
					ioEdit *pNameEdit = (ioEdit*)m_pNameEdit;
					pNameEdit->SetKeyFocus();
				}				
			}			
			else if( param == 1 )
			{
				ChangeBattleRoomInfo();
			}
		}
		break;
	}
}

void PartyOwnerWnd::ChangeBattleRoomInfo()
{
	// 닉네임 변경
	ioEdit *pEdit = (ioEdit*)FindChildWnd( ID_EDIT_NAME );
	if( !pEdit ) return;

	ioHashString szName = g_SlangUtil.ConvertString( pEdit->GetText() );
	if( szName != g_BattleRoomMgr.GetName() )
	{
		if( g_MyInfo.GetBlockType() == BKT_LIMITATION )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
			return;
		}
	}

	// 비번 변경
	pEdit = (ioEdit*)FindChildWnd( ID_EDIT_PW );
	if( !pEdit ) return;

	ioHashString szPW = pEdit->GetText();
	if( m_szPartyName == szName && m_szPartyPW == szPW )
	{
		HideWnd();
		return;
	}
	m_szPartyName = szName;
	m_szPartyPW   = szPW;

	// 정보 변경
	SP2Packet kPacket( CTPK_MACRO_COMMAND );
	kPacket << MACRO_ROOM_NAME_PW << szName << szPW;
	TCPNetwork::SendToServer( kPacket );

	g_BattleRoomMgr.SetName( szName );
	g_BattleRoomMgr.SetPW( szPW );
	PartyShamBattleWnd *pShamBattle = dynamic_cast<PartyShamBattleWnd*>(g_GUIMgr.FindWnd( PARTY_SHAM_BATTLE_WND ));
	if( pShamBattle )
		pShamBattle->UpdateShamBattle();
	HideWnd();
}

void PartyOwnerWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( 110,110,110 );
	g_FontMgr.PrintText( iXPos + 16, iYPos + 57, FONT_SIZE_12, STR(1) );
	g_FontMgr.PrintText( iXPos + 16, iYPos + 80, FONT_SIZE_12, STR(2) );
}
//////////////////////////////////////////////////////////////////////////
PartyInviteList::PartyInviteList()
{
	m_iGradeLevel = 0;
	m_dwGuildIndex= 0;
	m_dwGuildMark = 0;

	m_pGuildMark	= NULL;
	m_pFriendMark	= NULL;
}

PartyInviteList::~PartyInviteList()
{
	SAFEDELETE( m_pGuildMark );
	SAFEDELETE( m_pFriendMark );
}

void PartyInviteList::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "GuildMark" )
	{
		SAFEDELETE( m_pGuildMark );
		m_pGuildMark = pImage;
	}
	else if( szType == "FriendMark" )
	{
		SAFEDELETE( m_pFriendMark );
		m_pFriendMark = pImage;
	}
	else
	{
		ioButton::AddRenderImage( szType, pImage );
	}
}

void PartyInviteList::InviteInfo( int iGradeLevel, ioHashString szName, DWORD dwGuildIndex, DWORD dwGuildMark )
{
	m_iGradeLevel = iGradeLevel;
	m_szName      = szName;
	m_dwGuildIndex= dwGuildIndex;
	m_dwGuildMark = dwGuildMark;

	HideChildWnd( ID_INVITE_BTN );
	if( !szName.IsEmpty() )
	{
		ShowChildWnd( ID_INVITE_BTN );
		SetActive();
	}
	else
	{
		SetInActive();
	}
}

void PartyInviteList::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_INVITE_BTN:
		if( cmd == IOBN_BTNUP )
		{
			if( g_App.IsAdminID( m_szName.c_str() ) )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
				return;
			}

			if( g_BattleRoomMgr.GetCurPlayer() >= g_BattleRoomMgr.GetMaxPlayer() )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
				return;
			}

			if( g_BattleRoomMgr.IsBattleModePlaying() && g_BattleRoomMgr.IsStartRoomEnterX() )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
				return;
			}

			//초대
			TCPNetwork::MouseBusy( true );
			SP2Packet kPacket( CTPK_BATTLEROOM_INVITE );
			kPacket << 1 << m_szName << m_iGradeLevel << m_dwGuildIndex << m_dwGuildMark;
			TCPNetwork::SendToServer( kPacket );
		}
		break;
	}
}

void PartyInviteList::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;
	if( m_szName.IsEmpty() ) return;

	if( m_bOver && !m_bClicked )
	{
		ioLSMouse *pMouse = g_App.GetMouse();
		if( pMouse && pMouse->IsMouseShowing() )
			pMouse->SetToolTipID( m_szName, UserInfoToolTip::TT_GLOBAL );
	}	
}

void PartyInviteList::OnRender()
{
	ioButton::OnRender();

	if( !m_szName.IsEmpty() )
	{
		int iXPos = GetDerivedPosX();
		int iYPos = GetDerivedPosY();

		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_LEFT );

		if( m_bOver || m_bClicked )
		{
			g_LevelMgr.RenderGrade( m_szName, iXPos + 4, iYPos + 2, m_iGradeLevel, TEAM_PRIVATE_1, UI_RENDER_NORMAL_ADD_COLOR, RGB( 255, 255, 255 ) );
			g_FontMgr.SetTextColor( 255, 255, 255 );
		}
		else
		{
			g_LevelMgr.RenderGrade( m_szName, iXPos + 4, iYPos + 2, m_iGradeLevel, TEAM_PRIVATE_1 );
			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		}
		g_FontMgr.PrintTextWidthCut( iXPos + 23, iYPos + 3, FONT_SIZE_12, 133.0f, m_szName.c_str() );
		int iRealNameSize = g_FontMgr.GetTextWidthCutSize( m_szName.c_str(), TS_NORMAL, FONT_SIZE_12, 133.0f );
		g_GuildMarkMgr.RenderSmallMark( m_dwGuildIndex, m_dwGuildMark, iXPos + 23 + iRealNameSize + 3, iYPos + 4 );
		if( m_dwGuildIndex != 0 )
			iRealNameSize += 16;
		// 길드 - > 친구 - > 일반
		if( g_GuildInfo.IsMyGuildUser( m_szName ) && m_pGuildMark )
			m_pGuildMark->Render( iXPos + 23 + iRealNameSize + 3, iYPos + 3 );
		else if( g_FriendManager.IsFriend( m_szName ) && m_pFriendMark )
			m_pFriendMark->Render( iXPos + 23 + iRealNameSize + 3, iYPos + 3 );
	}		
}
//////////////////////////////////////////////////////////////////////////
PartyInviteListWnd::PartyInviteListWnd() : m_pPreEdit( NULL )
{
	m_iCurPage = m_iPrevCurPage = m_iMaxPage = 0;
	m_dwSearchTime  = 0;
}

PartyInviteListWnd::~PartyInviteListWnd()
{
	m_vUserList.clear();
}

void PartyInviteListWnd::StartSend( bool bProcess )
{
	SP2Packet kPacket( CTPK_BATTLEROOM_INVITE_LIST );
	kPacket << m_iCurPage << MAX_PAGE_LIST;
	TCPNetwork::SendToServer( kPacket );
	m_dwSearchTime = FRAMEGETTIME();
}

void PartyInviteListWnd::ProcessSearch()
{	
	if( FRAMEGETTIME() - m_dwSearchTime > INVITE_SEARCH_TIME )
	{
		StartSend( true );
	}
}

void PartyInviteListWnd::iwm_show()
{	
	g_GUIMgr.HideWnd( INVITE_PROGRESS_WND );

	m_vUserList.clear();
	m_iCurPage = m_iPrevCurPage;
	m_iPrevCurPage = 0;
	StartSend();

	for(int i = ID_USER_LIST_1;i < ID_USER_LIST_10 + 1;i++)
	{
		PartyInviteList *pItem = dynamic_cast<PartyInviteList*> ( FindChildWnd( i ) );
		if( pItem )
		{			
			pItem->InviteInfo( 0, "", 0, 0 );
		}
	}	

	// 아이디 입력
	m_pPreEdit = ioEdit::m_pKeyFocusEdit;

	ioEdit *pNameEdit = (ioEdit*)FindChildWnd( ID_NAME_EDIT );	
	if( pNameEdit )
	{
		pNameEdit->ClearString();
		pNameEdit->SetKeyFocus();
		g_InputBox.SetString( "" );
	}
}

void PartyInviteListWnd::iwm_hide()
{	
	m_vUserList.clear();

	ioEdit *pNameEdit = (ioEdit*)FindChildWnd( ID_NAME_EDIT );
	if( pNameEdit )
	{
		pNameEdit->ClearString();
		pNameEdit->KillKeyFocus();		
	}

	if( m_pPreEdit )	
	{
		ioWnd *pParent = m_pPreEdit->GetParent();
		if( pParent )
		{
			if( pParent->IsShow() )
				((ioEdit*)m_pPreEdit)->SetKeyFocus();	
		}
	}	
	m_pPreEdit = NULL;
}

void PartyInviteListWnd::UpdateSetPage( int iCurPage )
{
	int iPrevPage = m_iCurPage;
	m_iCurPage = min( m_iMaxPage, iCurPage );

	if( m_iCurPage != iPrevPage )
		StartSend();

	bool bActive = false;

	int iListSIze = m_vUserList.size();
	int iStartPos = 0;
	for(int i = ID_USER_LIST_1;i < ID_USER_LIST_10 + 1;i++,iStartPos++)
	{
		PartyInviteList *pItem = (PartyInviteList*)FindChildWnd( i );
		if( pItem )
		{
			if( iStartPos < iListSIze )
			{
				UserInviteInfo &kInfo = m_vUserList[iStartPos];
				pItem->InviteInfo( kInfo.m_iLevel, kInfo.m_szName, g_UserInfoMgr.GetGuildIndex( kInfo.m_szName ), g_UserInfoMgr.GetGuildMark( kInfo.m_szName ) );
			}
			else
			{
				pItem->InviteInfo( 0, "", 0, 0 );
			}
		}
	}	
}

void PartyInviteListWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
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
	case ID_ALL_INVITE:
		if( cmd == IOBN_BTNUP )
		{
			AllInvite();
		}
		break;
	case ID_NAME_INVITE:
		if( cmd == IOBN_BTNUP )
		{
			NameInvite();
		}
		break;
	case ID_NAME_EDIT:
		if( cmd == IOED_INPUT )
		{
			if( param == 0 || param == 1 )
			{
				iwm_command( FindChildWnd( ID_NAME_INVITE ), IOBN_BTNUP, 0 );
			}			
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
	case ID_USER_LIST_1:
	case ID_USER_LIST_2:
	case ID_USER_LIST_3:
	case ID_USER_LIST_4:
	case ID_USER_LIST_5:
	case ID_USER_LIST_6:
	case ID_USER_LIST_7:
	case ID_USER_LIST_8:
	case ID_USER_LIST_9:
	case ID_USER_LIST_10:
		if( cmd == IOBN_BTNUP )
		{
			PartyInviteList *pListBtn = dynamic_cast<PartyInviteList*>(pWnd);
			if( !pListBtn->GetName().IsEmpty() )
				g_App.OpenMannerWnd( pListBtn->GetName().c_str(), true );
		}
		break;			
	}
}

void PartyInviteListWnd::AllInvite()
{
	if( g_BattleRoomMgr.GetCurPlayer() >= g_BattleRoomMgr.GetMaxPlayer() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		return;
	}
	
	if( g_BattleRoomMgr.IsBattleModePlaying() && g_BattleRoomMgr.IsStartRoomEnterX() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
		return;
	}

	int i;
	vUserInviteInfo vSendList;
	for(i = 0;i < (int)m_vUserList.size();i++)
	{
		UserInviteInfo &kInfo = m_vUserList[i];
		if( kInfo.m_szName.IsEmpty() ) continue;
		if( g_App.IsAdminID( kInfo.m_szName.c_str() ) ) continue;

		vSendList.push_back( kInfo );
	}

	if( vSendList.empty() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
		return;
	}

	m_iPrevCurPage = m_iCurPage;
	int iSize = vSendList.size();
	TCPNetwork::MouseBusy( true );
	SP2Packet kPacket( CTPK_BATTLEROOM_INVITE );
	kPacket << iSize;
	if( iSize == 1 )
	{
		UserInviteInfo &kInfo = vSendList[0];
		kPacket << kInfo.m_szName << kInfo.m_iLevel << g_UserInfoMgr.GetGuildIndex( kInfo.m_szName ) << g_UserInfoMgr.GetGuildMark( kInfo.m_szName );
	}
	else
	{
		for(i = 0;i < iSize;i++)
		{
			UserInviteInfo &kInfo = vSendList[i];
			kPacket << kInfo.m_szName;
		}
	}		
	TCPNetwork::SendToServer( kPacket );	
	vSendList.clear();
}

void PartyInviteListWnd::NameInvite()
{
	if( g_BattleRoomMgr.GetCurPlayer() >= g_BattleRoomMgr.GetMaxPlayer() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		return;
	}

	if( g_BattleRoomMgr.IsBattleModePlaying() && g_BattleRoomMgr.IsStartRoomEnterX() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
		return;
	}

	if( g_App.IsMouseBusy() )
		return;

	ioEdit *pNameEdit = (ioEdit*)FindChildWnd( ID_NAME_EDIT );
	if( !pNameEdit )
		return;

	char szName[MAX_PATH] = "";
	sprintf( szName, "%s", pNameEdit->GetText() );

	pNameEdit->ClearString();
	pNameEdit->SetKeyFocus();
	g_InputBox.SetString( "" );

	if( !g_App.IsRightID( szName ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
		return;
	}

	ioHashString szIsID = szName;
	if( g_MyInfo.GetPublicID() == szIsID )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4) );
		return;
	}

	if( g_App.IsAdminID( szName ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(5) );
		return;
	}		

	m_iPrevCurPage = m_iCurPage;
	TCPNetwork::MouseBusy( true );
	SP2Packet kPacket( CTPK_BATTLEROOM_INVITE );
	kPacket << 1 << szName << -1 << 0 << 0;
	TCPNetwork::SendToServer( kPacket );
}

void PartyInviteListWnd::StartInivteWait( int iValue, const ioHashString &szName )
{
	if( !IsShow() )
		return;

	HideWnd();
	// 초대 현황 UI SHOW
	InviteProgressWnd *pProgressWnd = dynamic_cast<InviteProgressWnd*>( g_GUIMgr.FindWnd( INVITE_PROGRESS_WND ) );
	if( pProgressWnd )
	{
		pProgressWnd->StartInivteWait( this, iValue, szName );
	}
}

void PartyInviteListWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	ioMovingWnd::OnProcess( fTimePerSec );

	ProcessSearch();		
}

void PartyInviteListWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, STR(1), m_iCurPage + 1 );

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, STR(2), m_iMaxPage + 1 );
	kPrinter.PrintFullText( iXPos + 151, iYPos + 273, TAT_CENTER );

	if( m_vUserList.empty() )
	{
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
		g_FontMgr.PrintText( iXPos + 151, iYPos + 120, FONT_SIZE_12, STR(3) );
	}
}

void PartyInviteListWnd::SetUserList( SP2Packet &rkPacket )
{	
	m_vUserList.clear();
	int i = 0;
	int iTotalList;
	int iCurList;
	rkPacket >> iTotalList >> iCurList;
	for(i = 0;i < iCurList;i++)
	{
		DWORD dwGuildIndex, dwGuildMark;
		UserInviteInfo kList;
		rkPacket >> kList.m_szName >> kList.m_iLevel >> dwGuildIndex >> dwGuildMark;
		m_vUserList.push_back( kList );
		g_GuildInfo.SetGuildMark( kList.m_szName, dwGuildIndex, dwGuildMark );
	}    	
	m_iMaxPage = max( 0, iTotalList - 1 ) / MAX_PAGE_LIST;
	UpdateSetPage( m_iCurPage );
}

//////////////////////////////////////////////////////////////////////////
MiddleJoinDelayWnd::MiddleJoinDelayWnd()
{
	m_pScaleImageBack = NULL;

	m_dwChangeDuration = 200;
	m_fBigScale = FLOAT1;
	m_fSmallScale = 0.7f;

	m_dwChangeTime	= 0;
	m_iCurBigIdx	= 0;
	m_bReverse	= false;
}

MiddleJoinDelayWnd::~MiddleJoinDelayWnd()
{
	ScaleImageList::iterator iter;
	for( iter=m_ScaleImageList.begin() ; iter!=m_ScaleImageList.end() ; ++iter )
	{
		ScaleImage *pImage = (*iter);
		if( !pImage ) continue;
		SAFEDELETE( pImage->m_pImage );
		SAFEDELETE( pImage );
	}
	m_ScaleImageList.clear();

	SAFEDELETE( m_pScaleImageBack );
}

void MiddleJoinDelayWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "ScaleImage" )
	{
		if( !pImage )
			return;
		ScaleImage *pScaleImage = new ScaleImage;
		if( !pScaleImage )
			return;
		pScaleImage->m_pImage = pImage;
		pScaleImage->m_dwScaleTime = 0;
		pScaleImage->m_fCurScale   = m_fSmallScale;
		pImage->SetScale( pScaleImage->m_fCurScale );

		m_ScaleImageList.push_back( pScaleImage );
	}
	else if( szType == "ScaleImageBack" )
	{
		SAFEDELETE( m_pScaleImageBack );
		m_pScaleImageBack = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void MiddleJoinDelayWnd::iwm_show()
{
	g_GUIMgr.HideWnd( TOTAL_RESULT_MAIN_WND );

	g_GUIMgr.HideWnd( ROUND_RESULT_MAIN_WND );
	g_GUIMgr.HideWnd( MINIMAP_WND );
	g_GUIMgr.HideWnd( TIME_SCORE_WND );
	g_GUIMgr.HideWnd( SKILL_LIST_WND );
	g_GUIMgr.HideWnd( ACCESSORY_SKILL_LIST_WND );
	g_GUIMgr.HideWnd( CENTER_INFO_WND );
	g_GUIMgr.HideWnd( MONSTER_BOSS_GAUGE_WND );
	g_GUIMgr.HideWnd( MODE_RANK_WND );
	g_GUIMgr.HideWnd( KO_CHAT_MSG_WND );
	g_GUIMgr.HideWnd( FIGHTCLUB_TIME_GAUGE_WND );
	g_GUIMgr.HideWnd( FIGHTCLUB_ROUND_END_WND );
	g_GUIMgr.HideWnd( FIGHTCLUB_CHALLENGER_WND );
	g_GUIMgr.HideWnd( TOWERDEF_USER_GAUGE_WND );
	g_GUIMgr.HideWnd( MINI_SOLDIER_SELECT_WND );
	g_GUIMgr.HideWnd( TUTORIAL_KEYBOARD_WND );
	g_GUIMgr.HideWnd( FIGHTCLUB_CHALLENGER_WND );
	g_GUIMgr.HideWnd( FIGHTCLUB_CONTINUE_WND );
	g_GUIMgr.HideWnd( TRAINING_FUNC_MENU );
	g_GUIMgr.HideWnd( BATTLE_MODE_TIME_GAUGE_WND );
	g_GUIMgr.HideWnd( BATTLE_MODE_FUNCTION_WND );
	g_GUIMgr.HideWnd( BATTLE_MODE_BLUE_USER_INFO_WND );
	g_GUIMgr.HideWnd( BATTLE_MODE_RED_USER_INFO_WND );
	g_ModeHelpMgr.HideHelpDirectly();

	m_dwChangeTime = FRAMEGETTIME() + m_dwChangeDuration;
	m_iCurBigIdx   = 0;

	if( !m_ScaleImageList.empty() )
	{
		ScaleImageList::iterator iter;
		for( iter=m_ScaleImageList.begin() ; iter!=m_ScaleImageList.end() ; ++iter )
		{
			ScaleImage *pImage = (*iter);
			if( !pImage ) 
				continue;
			pImage->m_fCurScale = m_fSmallScale;
			pImage->m_pImage->SetScale( pImage->m_fCurScale );
			pImage->m_dwScaleTime = 0;
		}

		if( COMPARE( m_iCurBigIdx, 0, (int) m_ScaleImageList.size() ) )
			m_ScaleImageList[m_iCurBigIdx]->m_dwScaleTime = FRAMEGETTIME();
	}
	m_bReverse  = false;
}

void MiddleJoinDelayWnd::iwm_hide()
{
	if( g_BattleRoomMgr.IsBattleModePlaying() )
	{
		g_GUIMgr.ShowWnd( MINIMAP_WND );
		g_GUIMgr.ShowWnd( TIME_SCORE_WND );
		g_GUIMgr.ShowWnd( SKILL_LIST_WND );
		g_GUIMgr.ShowWnd( ACCESSORY_SKILL_LIST_WND );
		g_GUIMgr.ShowWnd( KO_CHAT_MSG_WND );
		g_GUIMgr.ShowWnd( FIGHTCLUB_TIME_GAUGE_WND );
	}
}

void MiddleJoinDelayWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;	

	DWORD dwCurTime = FRAMEGETTIME();

	ioMovingWnd::OnProcess( fTimePerSec );

	if( m_dwChangeTime < dwCurTime )
	{
		if( !m_bReverse )
		{
			m_iCurBigIdx++;

			if( m_iCurBigIdx >= (int)m_ScaleImageList.size() )
			{
				m_iCurBigIdx = m_ScaleImageList.size() - 2;
				m_bReverse = true;
			}
		}
		else
		{
			m_iCurBigIdx--;
			if( m_iCurBigIdx < 0 )
			{
				m_iCurBigIdx = 1;
				m_bReverse = false;
			}			
		}			

		m_dwChangeTime = dwCurTime + m_dwChangeDuration;
		if( COMPARE( m_iCurBigIdx, 0, (int)m_ScaleImageList.size() ) )
		{
			m_ScaleImageList[m_iCurBigIdx]->m_fCurScale = m_fSmallScale;
			m_ScaleImageList[m_iCurBigIdx]->m_pImage->SetScale( m_ScaleImageList[m_iCurBigIdx]->m_fCurScale );
			m_ScaleImageList[m_iCurBigIdx]->m_dwScaleTime = FRAMEGETTIME();
		}
	}
	ScaleIconImage();
}

void MiddleJoinDelayWnd::ScaleIconImage()
{
	int iScaleImageCount = m_ScaleImageList.size();
	for( int i=0 ; i<iScaleImageCount ; i++ )
	{
		ScaleImage *pImage = m_ScaleImageList[i];
		if( !pImage )
			continue;

		int iGapTime  = FRAMEGETTIME() - pImage->m_dwScaleTime;
		int iAniTime  = m_dwChangeDuration;
		float fCurScale = m_fSmallScale;

		if( pImage->m_dwScaleTime != 0 )
		{
			if( iGapTime < iAniTime )
			{
				fCurScale += ( (m_fBigScale - m_fSmallScale) * sinf( ( D3DX_PI / 2 ) * ( (float)iGapTime / (float)iAniTime ) ) );				
			}
			else if( iGapTime < iAniTime * 2 )
			{
				fCurScale -= ( (m_fBigScale - m_fSmallScale) * cosf( ( D3DX_PI / 2 ) * ( (float)( iGapTime + iAniTime ) / (float)iAniTime ) ) );
			}
			else 
			{
				pImage->m_dwScaleTime = 0;
			}
		}

		fCurScale = max( m_fSmallScale, min( fCurScale, m_fBigScale ) );
		pImage->m_fCurScale = fCurScale;
		pImage->m_pImage->SetScale( pImage->m_fCurScale );
	}
}

void MiddleJoinDelayWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_dwChangeDuration = xElement.GetIntAttribute_e( "ChangeDuration" );
	m_fBigScale   = xElement.GetFloatAttribute_e( "BigScale" );
	m_fSmallScale = xElement.GetFloatAttribute_e( "SmallScale" );
}

void MiddleJoinDelayWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	g_FontMgr.SetTextStyle( TS_NORMAL );	
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( iXPos + 151, iYPos + 53, FONT_SIZE_13, STR(1) );

	int iRealXPos = 45;
	ScaleImageList::iterator iter;
	for( iter=m_ScaleImageList.begin() ; iter!=m_ScaleImageList.end() ; ++iter )
	{
		ScaleImage *pImage = (*iter);
		if( !pImage ) 
			continue;

		if( m_pScaleImageBack )
		{
			m_pScaleImageBack->SetScale( pImage->m_fCurScale );
			m_pScaleImageBack->Render( iXPos + iRealXPos, iYPos + 120, UI_RENDER_MULTIPLY, TFO_BILINEAR );
		}
		if( pImage->m_pImage )
		{
			pImage->m_pImage->Render( iXPos + iRealXPos, iYPos + 120, UI_RENDER_NORMAL, TFO_BILINEAR );
		}
		iRealXPos += 71;
	}
}
//////////////////////////////////////////////////////////////////////////
PartyOptionBtn::PartyOptionBtn()
{
	m_dwDefaultColor = m_dwChangeColor = 0;
}

PartyOptionBtn::~PartyOptionBtn()
{

}

void PartyOptionBtn::ParseExtraInfo( ioXMLElement &xElement )
{
	ioButton::ParseExtraInfo( xElement );
	m_dwDefaultColor = ioStringConverter::ParseColor( xElement.GetStringAttribute_e( "DefaultColor" ) );
	m_dwChangeColor  = ioStringConverter::ParseColor( xElement.GetStringAttribute_e( "ChangeColor" ) );
}

void PartyOptionBtn::SetOptionInfo( const char *szTitle, int iCurrentType, int iDefaultType )
{
	ioUITitle *pTitle = GetTitle();
	if( !pTitle ) 
		return;

	pTitle->SetText( szTitle, "" );
	if( iCurrentType == iDefaultType )
	{
		pTitle->SetColor( "Normal", m_dwDefaultColor, 0 );
		pTitle->SetColor( "Over", m_dwDefaultColor, 0 );
		pTitle->SetColor( "Push", m_dwDefaultColor, 0 );
	}
	else
	{
		pTitle->SetColor( "Normal", m_dwChangeColor, 0 );
		pTitle->SetColor( "Over", m_dwChangeColor, 0 );
		pTitle->SetColor( "Push", m_dwChangeColor, 0 );
	}
}
//////////////////////////////////////////////////////////////////////////
PartyOptionWnd::PartyOptionWnd()
{
}

PartyOptionWnd::~PartyOptionWnd()
{
}

void PartyOptionWnd::iwm_show()
{
	UpdateOption();
}

void PartyOptionWnd::SetButtonTitle( DWORD dwID, const char *szTitle, int iCurrentType, int iDefaultType )
{
	PartyOptionBtn *pOptionBtn = dynamic_cast<PartyOptionBtn*>( FindChildWnd( dwID ) );
	if( pOptionBtn )
	{
		pOptionBtn->SetOptionInfo( szTitle, iCurrentType, iDefaultType );
	}
}

void PartyOptionWnd::UpdateOption()
{
	int iModeIndex	= g_BattleRoomMgr.GetSelectModeIndex();
	if( g_RoomOptionMgr.IsEnableRoundOptionMode( iModeIndex ) )
	{
		SetChildActive( ID_ROUND_TYPE_TLE );
		SetChildActive( ID_ROUND_TYPE );
		SetChildActive( ID_ROUND_TIME_TYPE_TLE );
		SetChildActive( ID_ROUND_TIME_TYPE );
	}
	else
	{
		SetChildInActive( ID_ROUND_TYPE_TLE );
		SetChildInActive( ID_ROUND_TYPE );
		SetChildInActive( ID_ROUND_TIME_TYPE_TLE );
		SetChildInActive( ID_ROUND_TIME_TYPE );
	}

	int iCurType = g_BattleRoomMgr.GetPreSetModeType();
	ioButton *pCurBtn = dynamic_cast<ioButton*>( FindChildWnd( ID_PRESET_BTN ) );
	if( pCurBtn )
		pCurBtn->SetTitleText( g_RoomOptionMgr.GetPreSetDesc(iCurType).c_str() );

	PreSetInfo kDefaultInfo;
	g_RoomOptionMgr.GetDefaultInfo( kDefaultInfo );

	iCurType = g_BattleRoomMgr.GetChangeCharType();
	SetButtonTitle( ID_CHANGE_CHAR_TYPE, g_RoomOptionMgr.GetChangeCharDesc(iCurType).c_str(), iCurType, kDefaultInfo.m_iChangeCharType );

	iCurType = g_BattleRoomMgr.GetTeamAttackType();
	SetButtonTitle( ID_TEAM_ATTACK_TYPE, g_RoomOptionMgr.GetTeamAttackDesc(iCurType).c_str(), iCurType, kDefaultInfo.m_iTeamAttackType );

	iCurType = g_BattleRoomMgr.GetCoolTimeType();
	SetButtonTitle( ID_COOLTIME_TYPE, g_RoomOptionMgr.GetCoolTimeDesc(iCurType).c_str(), iCurType, kDefaultInfo.m_iCoolTimeType );

	iCurType = g_BattleRoomMgr.GetDropDamageType();
	SetButtonTitle( ID_DROP_DAMAGE_TYPE, g_RoomOptionMgr.GetDropDamageDesc(iCurType).c_str(), iCurType, kDefaultInfo.m_iDropDamageType );

	iCurType = g_BattleRoomMgr.GetGravityType();
	SetButtonTitle( ID_GRAVITY_TYPE, g_RoomOptionMgr.GetGravityDesc(iCurType).c_str(), iCurType, kDefaultInfo.m_iGravityType );

	iCurType = g_BattleRoomMgr.GetGetUpType();
	SetButtonTitle( ID_GETUP_TYPE, g_RoomOptionMgr.GetGetUpDesc(iCurType).c_str(), iCurType, kDefaultInfo.m_iGetUpType );

	iCurType = g_BattleRoomMgr.GetKOType();
	SetButtonTitle( ID_KO_TYPE, g_RoomOptionMgr.GetKODesc(iCurType).c_str(), iCurType, kDefaultInfo.m_iKOType );

	iCurType = g_BattleRoomMgr.GetKOEffectType();
	SetButtonTitle( ID_KO_EFFECT_TYPE, g_RoomOptionMgr.GetKOEffectDesc(iCurType).c_str(), iCurType, kDefaultInfo.m_iKOEffectType );

	iCurType = g_BattleRoomMgr.GetRedHPType();
	SetButtonTitle( ID_RED_HP_TYPE, g_RoomOptionMgr.GetRedHPDesc(iCurType).c_str(), iCurType, kDefaultInfo.m_iRedHPType );

	iCurType = g_BattleRoomMgr.GetBlueHPType();
	SetButtonTitle( ID_BLUE_HP_TYPE, g_RoomOptionMgr.GetBlueHPDesc(iCurType).c_str(), iCurType, kDefaultInfo.m_iBlueHPType );

	iCurType = g_BattleRoomMgr.GetRedBlowType();
	SetButtonTitle( ID_RED_BLOW_TYPE, g_RoomOptionMgr.GetRedBlowDesc(iCurType).c_str(), iCurType, kDefaultInfo.m_iRedBlowType );

	iCurType = g_BattleRoomMgr.GetBlueBlowType();
	SetButtonTitle( ID_BLUE_BLOW_TYPE, g_RoomOptionMgr.GetBlueBlowDesc(iCurType).c_str(), iCurType, kDefaultInfo.m_iBlueBlowType );

	iCurType = g_BattleRoomMgr.GetRedMoveSpeedType();
	SetButtonTitle( ID_RED_MOVE_SPEED_TYPE, g_RoomOptionMgr.GetRedMoveSpeedDesc(iCurType).c_str(), iCurType, kDefaultInfo.m_iRedMoveSpeedType );

	iCurType = g_BattleRoomMgr.GetBlueMoveSpeedType();
	SetButtonTitle( ID_BLUE_MOVE_SPEED_TYPE, g_RoomOptionMgr.GetBlueMoveSpeedDesc(iCurType).c_str(), iCurType, kDefaultInfo.m_iBlueMoveSpeedType );

	iCurType = g_BattleRoomMgr.GetRedEquipType();
	SetButtonTitle( ID_RED_EQUIP_TYPE, g_RoomOptionMgr.GetRedEquipDesc(iCurType).c_str(), iCurType, kDefaultInfo.m_iRedEquipType );

	iCurType = g_BattleRoomMgr.GetBlueEquipType();
	SetButtonTitle( ID_BLUE_EQUIP_TYPE, g_RoomOptionMgr.GetBlueEquipDesc(iCurType).c_str(), iCurType, kDefaultInfo.m_iBlueEquipType );

	iCurType = g_BattleRoomMgr.GetRoundType();
	SetButtonTitle( ID_ROUND_TYPE, g_RoomOptionMgr.GetRoundTypeDesc(iCurType).c_str(), iCurType, kDefaultInfo.m_iCatchModeRoundType );

	iCurType = g_BattleRoomMgr.GetRoundTimeType();
	SetButtonTitle( ID_ROUND_TIME_TYPE, g_RoomOptionMgr.GetRoundTimeTypeDesc(iCurType).c_str(), iCurType, kDefaultInfo.m_iCatchModeRoundTimeType );

	iCurType = g_BattleRoomMgr.GetGrowthUseType();
	SetButtonTitle( ID_GROWTH_USE_TYPE, g_RoomOptionMgr.GetGrowthUseDesc(iCurType).c_str(), iCurType, kDefaultInfo.m_iGrowthUseType );

	iCurType = g_BattleRoomMgr.GetExtraItemUseType();
	SetButtonTitle( ID_EXTRAITEM_USE_TYPE, g_RoomOptionMgr.GetExtraItemUseDesc(iCurType).c_str(), iCurType, kDefaultInfo.m_iExtraItemUseType );
}

void PartyOptionWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_DEFAULT_SET:
		if( cmd == IOBN_BTNUP )
		{
			if( g_BattleRoomMgr.GetOwnerName() == g_MyInfo.GetPublicID() )
			{
				PartyShamBattleWnd *pBattleWnd = dynamic_cast<PartyShamBattleWnd*>( g_GUIMgr.FindWnd( PARTY_SHAM_BATTLE_WND ) );
				if( pBattleWnd && !pBattleWnd->IsModeStart() )
				{
					if( g_BattleRoomMgr.IsBattleModePlaying() )
					{
						g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4) );
					}
					else
					{
						g_BattleRoomMgr.SetRoomOptionDefault( true );

						g_BattleRoomMgr.SendBattleRoomOption();
					}
				}
				else
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(11) );
				}

				UpdateOption();
			}
			else
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(12) );
			}
		}
		break;
	case ID_PRESET_BTN:
	case ID_TEAM_ATTACK_TYPE:
	case ID_CHANGE_CHAR_TYPE:
	case ID_COOLTIME_TYPE:
	case ID_RED_HP_TYPE:
	case ID_BLUE_HP_TYPE:
	case ID_DROP_DAMAGE_TYPE:
	case ID_GRAVITY_TYPE:
	case ID_GETUP_TYPE:
	case ID_KO_TYPE:
	case ID_KO_EFFECT_TYPE:
	case ID_RED_BLOW_TYPE:
	case ID_BLUE_BLOW_TYPE:
	case ID_RED_MOVE_SPEED_TYPE:
	case ID_BLUE_MOVE_SPEED_TYPE:
	case ID_RED_EQUIP_TYPE:
	case ID_BLUE_EQUIP_TYPE:
	case ID_ROUND_TYPE:
	case ID_ROUND_TIME_TYPE:
	case ID_GROWTH_USE_TYPE:
	case ID_EXTRAITEM_USE_TYPE:
		if( cmd == IOBN_BTNDOWN )
		{
			if( g_BattleRoomMgr.GetOwnerName() == g_MyInfo.GetPublicID() )
			{
				PartyShamBattleWnd *pBattleWnd = dynamic_cast<PartyShamBattleWnd*>( g_GUIMgr.FindWnd( PARTY_SHAM_BATTLE_WND ) );
				if( pBattleWnd && !pBattleWnd->IsModeStart() )
				{
					if( g_BattleRoomMgr.IsBattleModePlaying() )
					{
						g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(14) );
					}
					else
					{
						CheckShowChildListWnd( pWnd, dwID );
					}
				}
				else
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(15) );
				}
			}
			else
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(13) );
			}
		}
		else if( cmd == IO_RESOLUTION_CHANGE )
		{
			PartyShamBattleWnd *pBattleWnd = dynamic_cast<PartyShamBattleWnd*>( g_GUIMgr.FindWnd( PARTY_SHAM_BATTLE_WND ) );
			if( pBattleWnd && !pBattleWnd->IsModeStart() )
			{
				if( g_BattleRoomMgr.IsBattleModePlaying() )
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(14) );
				}
				else
				{
					bool bSuccess = SelectCheildListWnd( dwID, param );
					if( bSuccess )
					{
						g_BattleRoomMgr.SendBattleRoomOption();
					}
				}
			}
			else
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(15) );
			}

			UpdateOption();
		}
		break;
	}
}

void PartyOptionWnd::CheckShowChildListWnd( ioWnd *pWnd, DWORD dwID )
{
	if( !pWnd ) return;

	int iValue = 0;
	switch( dwID )
	{
	case ID_PRESET_BTN:
		iValue = g_BattleRoomMgr.GetPreSetModeType();
		break;
	case ID_TEAM_ATTACK_TYPE:
		iValue = g_BattleRoomMgr.GetTeamAttackType();
		break;
	case ID_CHANGE_CHAR_TYPE:
		iValue = g_BattleRoomMgr.GetChangeCharType();
		break;
	case ID_COOLTIME_TYPE:
		iValue = g_BattleRoomMgr.GetCoolTimeType();
		break;
	case ID_RED_HP_TYPE:
		iValue = g_BattleRoomMgr.GetRedHPType();
		break;
	case ID_BLUE_HP_TYPE:
		iValue = g_BattleRoomMgr.GetBlueHPType();
		break;
	case ID_DROP_DAMAGE_TYPE:
		iValue = g_BattleRoomMgr.GetDropDamageType();
		break;
	case ID_GRAVITY_TYPE:
		iValue = g_BattleRoomMgr.GetGravityType();
		break;
	case ID_GETUP_TYPE:
		iValue = g_BattleRoomMgr.GetGetUpType();
		break;
	case ID_KO_TYPE:
		iValue = g_BattleRoomMgr.GetKOType();
		break;
	case ID_KO_EFFECT_TYPE:
		iValue = g_BattleRoomMgr.GetKOEffectType();
		break;
	case ID_RED_BLOW_TYPE:
		iValue = g_BattleRoomMgr.GetRedBlowType();
		break;
	case ID_BLUE_BLOW_TYPE:
		iValue = g_BattleRoomMgr.GetBlueBlowType();
		break;
	case ID_RED_MOVE_SPEED_TYPE:
		iValue = g_BattleRoomMgr.GetRedMoveSpeedType();
		break;
	case ID_BLUE_MOVE_SPEED_TYPE:
		iValue = g_BattleRoomMgr.GetBlueMoveSpeedType();
		break;
	case ID_RED_EQUIP_TYPE:
		iValue = g_BattleRoomMgr.GetRedEquipType();
		break;
	case ID_BLUE_EQUIP_TYPE:
		iValue = g_BattleRoomMgr.GetBlueEquipType();
		break;
	case ID_ROUND_TYPE:
		iValue = g_BattleRoomMgr.GetRoundType();
		break;
	case ID_ROUND_TIME_TYPE:
		iValue = g_BattleRoomMgr.GetRoundTimeType();
		break;
	case ID_GROWTH_USE_TYPE:
		iValue = g_BattleRoomMgr.GetGrowthUseType();
		break;
	case ID_EXTRAITEM_USE_TYPE:
		iValue = g_BattleRoomMgr.GetExtraItemUseType();
		break;
	}

	if( dwID == ID_PRESET_BTN )
	{
		RoomOptionTypeList *pList = (RoomOptionTypeList*)FindChildWnd( ID_TYPE_LIST2_WND );
		if( pList )
		{
			pList->OpenList( pWnd->GetXPos(), pWnd->GetYPos() + 22, pWnd, dwID, iValue );
		}
	}
	else
	{
		RoomOptionTypeList *pList = (RoomOptionTypeList*)FindChildWnd( ID_TYPE_LIST_WND );
		if( pList )
		{
			pList->OpenList( pWnd->GetXPos() - 29, pWnd->GetYPos() + 20, pWnd, dwID, iValue );
		}
	}
}

bool PartyOptionWnd::SelectCheildListWnd( DWORD dwID, DWORD dwParam )
{
	bool bResult = false;

	switch( dwID )
	{
	case ID_PRESET_BTN:
		bResult = CheckPreSetModeType( dwParam );
		break;
	case ID_CHANGE_CHAR_TYPE:
		bResult = CheckChangeCharType( dwParam );
		break;
	case ID_COOLTIME_TYPE:
		bResult = CheckCoolTimeType( dwParam );
		break;
	case ID_RED_HP_TYPE:
		bResult = CheckRedHPType( dwParam );
		break;
	case ID_BLUE_HP_TYPE:
		bResult = CheckBlueHPType( dwParam );
		break;
	case ID_DROP_DAMAGE_TYPE:
		bResult = CheckDropDamageType( dwParam );
		break;
	case ID_GRAVITY_TYPE:
		bResult = CheckGravityType( dwParam );
		break;
	case ID_TEAM_ATTACK_TYPE:
		bResult = CheckTeamAttackType( dwParam );
		break;
	case ID_GETUP_TYPE:
		bResult = CheckGetUpType( dwParam );
		break;
	case ID_KO_TYPE:
		bResult = CheckKOType( dwParam );
		break;
	case ID_KO_EFFECT_TYPE:
		bResult = CheckKOEffectType( dwParam );
		break;
	case ID_RED_BLOW_TYPE:
		bResult = CheckRedBlowType( dwParam );
		break;
	case ID_BLUE_BLOW_TYPE:
		bResult = CheckBlueBlowType( dwParam );
		break;
	case ID_RED_MOVE_SPEED_TYPE:
		bResult = CheckRedMoveSpeedType( dwParam );
		break;
	case ID_BLUE_MOVE_SPEED_TYPE:
		bResult = CheckBlueMoveSpeedType( dwParam );
		break;
	case ID_RED_EQUIP_TYPE:
		bResult = CheckRedEquipType( dwParam );
		break;
	case ID_BLUE_EQUIP_TYPE:
		bResult = CheckBlueEquipType( dwParam );
		break;
	case ID_ROUND_TYPE:
		bResult = CheckRoundType( dwParam );
		break;
	case ID_ROUND_TIME_TYPE:
		bResult = CheckRoundTimeType( dwParam );
		break;
	case ID_GROWTH_USE_TYPE:
		bResult = CheckGrowthUseType( dwParam );
		break;
	case ID_EXTRAITEM_USE_TYPE:
		bResult = CheckExtraItemUseType( dwParam );
		break;
	}

	if( bResult && dwID != ID_PRESET_BTN )
	{
		int iDefault = g_RoomOptionMgr.GetOptionTypeCnt(PartyOptionWnd::ID_PRESET_BTN) - 1;
		iDefault = max( 0, iDefault );

		g_BattleRoomMgr.SetPreSetModeType( iDefault, false );
	}

	return bResult;
}

void PartyOptionWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	ioMovingWnd::OnProcess( fTimePerSec );
}

void PartyOptionWnd::OnRender()
{
	ioWnd::OnRender();
}

bool PartyOptionWnd::CheckPreSetModeType( int iIndex )
{
	if( g_BattleRoomMgr.GetPreSetModeType() != iIndex )
	{
		g_BattleRoomMgr.SetPreSetModeType( iIndex, true );
		return true;
	}

	return false;
}

bool PartyOptionWnd::CheckChangeCharType( int iIndex )
{
	if( g_BattleRoomMgr.GetChangeCharType() != iIndex )
	{
		g_BattleRoomMgr.SetChangeCharType( iIndex, true );
		return true;
	}

	return false;
}

bool PartyOptionWnd::CheckCoolTimeType( int iIndex )
{
	if( g_BattleRoomMgr.GetCoolTimeType() != iIndex )
	{
		g_BattleRoomMgr.SetCoolTimeType( iIndex, true );
		return true;
	}

	return false;
}

bool PartyOptionWnd::CheckRedHPType( int iIndex )
{
	if( g_BattleRoomMgr.GetRedHPType() != iIndex )
	{
		g_BattleRoomMgr.SetRedHPType( iIndex, true );
		return true;
	}

	return false;
}

bool PartyOptionWnd::CheckBlueHPType( int iIndex )
{
	if( g_BattleRoomMgr.GetBlueHPType() != iIndex )
	{
		g_BattleRoomMgr.SetBlueHPType( iIndex, true );
		return true;
	}

	return false;
}

bool PartyOptionWnd::CheckDropDamageType( int iIndex )
{
	if( g_BattleRoomMgr.GetDropDamageType() != iIndex )
	{
		g_BattleRoomMgr.SetDropDamageType( iIndex, true );
		return true;
	}

	return false;
}

bool PartyOptionWnd::CheckGravityType( int iIndex )
{
	if( g_BattleRoomMgr.GetGravityType() != iIndex )
	{
		g_BattleRoomMgr.SetGravityType( iIndex, true );
		return true;
	}

	return false;
}

bool PartyOptionWnd::CheckTeamAttackType( int iIndex )
{
	if( g_BattleRoomMgr.GetTeamAttackType() != iIndex )
	{
		g_BattleRoomMgr.SetTeamAttackType( iIndex, true );
		return true;
	}

	return false;
}

bool PartyOptionWnd::CheckGetUpType( int iIndex )
{
	if( g_BattleRoomMgr.GetGetUpType() != iIndex )
	{
		g_BattleRoomMgr.SetGetUpType( iIndex, true );
		return true;
	}

	return false;
}

bool PartyOptionWnd::CheckRedMoveSpeedType( int iIndex )
{
	if( g_BattleRoomMgr.GetRedMoveSpeedType() != iIndex )
	{
		g_BattleRoomMgr.SetRedMoveSpeedType( iIndex, true );
		return true;
	}

	return false;
}

bool PartyOptionWnd::CheckBlueMoveSpeedType( int iIndex )
{
	if( g_BattleRoomMgr.GetBlueMoveSpeedType() != iIndex )
	{
		g_BattleRoomMgr.SetBlueMoveSpeedType( iIndex, true );
		return true;
	}

	return false;
}

bool PartyOptionWnd::CheckRedEquipType( int iIndex )
{
	if( g_BattleRoomMgr.GetRedEquipType() != iIndex )
	{
		g_BattleRoomMgr.SetRedEquipType( iIndex, true );
		return true;
	}

	return false;
}

bool PartyOptionWnd::CheckBlueEquipType( int iIndex )
{
	if( g_BattleRoomMgr.GetBlueEquipType() != iIndex )
	{
		g_BattleRoomMgr.SetBlueEquipType( iIndex, true );
		return true;
	}

	return false;
}

bool PartyOptionWnd::CheckKOType( int iIndex )
{
	if( g_BattleRoomMgr.GetKOType() != iIndex )
	{
		g_BattleRoomMgr.SetKOType( iIndex, true );
		return true;
	}

	return false;
}

bool PartyOptionWnd::CheckKOEffectType( int iIndex )
{
	if( g_BattleRoomMgr.GetKOEffectType() != iIndex )
	{
		g_BattleRoomMgr.SetKOEffectType( iIndex, true );
		return true;
	}

	return false;
}

bool PartyOptionWnd::CheckRedBlowType( int iIndex )
{
	if( g_BattleRoomMgr.GetRedBlowType() != iIndex )
	{
		g_BattleRoomMgr.SetRedBlowType( iIndex, true );
		return true;
	}

	return false;
}

bool PartyOptionWnd::CheckBlueBlowType( int iIndex )
{
	if( g_BattleRoomMgr.GetBlueBlowType() != iIndex )
	{
		g_BattleRoomMgr.SetBlueBlowType( iIndex, true );
		return true;
	}

	return false;
}

bool PartyOptionWnd::CheckRoundType( int iIndex )
{
	if( g_BattleRoomMgr.GetRoundType() != iIndex )
	{
		g_BattleRoomMgr.SetRoundType( iIndex, true );
		return true;
	}

	return false;
}

bool PartyOptionWnd::CheckRoundTimeType( int iIndex )
{
	if( g_BattleRoomMgr.GetRoundTimeType() != iIndex )
	{
		g_BattleRoomMgr.SetRoundTimeType( iIndex, true );
		return true;
	}

	return false;
}

bool PartyOptionWnd::CheckGrowthUseType( int iIndex )
{
	if( g_BattleRoomMgr.GetGrowthUseType() != iIndex )
	{
		g_BattleRoomMgr.SetGrowthUseType( iIndex, true );
		return true;
	}

	return false;
}

bool PartyOptionWnd::CheckExtraItemUseType( int iIndex )
{
	if( g_BattleRoomMgr.GetExtraItemUseType() != iIndex )
	{
		g_BattleRoomMgr.SetExtraItemUseType( iIndex, true );
		return true;
	}

	return false;
}

ioHashString PartyOptionWnd::GetOptionTitle( int iType )
{
	ioButton *pCurBtn = dynamic_cast<ioButton*>( FindChildWnd( iType ) );
	if( pCurBtn )
	{
		ioHashString szTitle;
		ioUITitle *pTitle = pCurBtn->GetTitle();
		if( pTitle )
			pTitle->GetFirstText( szTitle );

		return szTitle;
	}

	return ioHashString();
}