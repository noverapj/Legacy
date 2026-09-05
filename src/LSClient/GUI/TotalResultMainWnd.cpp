
#include "StdAfx.h"

#include "../ioGuildMarkMgr.h"
#include "../ioTreasureCardMgr.h"
#include "../ioTournamentManager.h"
#include "../ioTournamentTeamManager.h"
#include "../ioShuffleRoomMgr.h"
#include "../ioSoldierAwakeManager.h"
#include "../ioExtendSoundManager.h"
#include "../ioUserSpiritManager.h"

#include "ioInterfaceBackground.h"
#include "BriefingWnd.h"
#include "ChatMsgWnd.h"
#include "TotalResultRewardWnd.h"
#include "TotalResultMainWnd.h"
#include "LadderTeamWnd.h"
#include "UserInfoToolTip.h"
//////////////////////////////////////////////////////////////////////////
AwardChatBubbleWnd::AwardChatBubbleWnd()
{
	m_pTooltipTail = NULL;

	m_iDefaultX  = 18;
	m_iDefaultY	 = 69;
	m_iDefaultW  = 0;
	m_iDefaultH  = 0;
	m_iAddHeight = 0;
	m_iStartXPos = 0;
	m_iStartYPos = 0;
	m_iWidthCut  = 0;
	m_iMaxLine   = 0;
	m_dwShowTime = 0;
	m_dwStartTime= 0;
	m_dwShowEventTime = 0;
	m_dwShowAniTime = 0;
}

AwardChatBubbleWnd::~AwardChatBubbleWnd()
{
	SAFEDELETE( m_pTooltipTail );
}

void AwardChatBubbleWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "TooltipTail" )
	{
		SAFEDELETE( m_pTooltipTail );
		m_pTooltipTail = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void AwardChatBubbleWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_iDefaultX  = xElement.GetIntAttribute_e( "DefaultX" );
	m_iDefaultY	 = xElement.GetIntAttribute_e( "DefaultY" );
	m_iDefaultW  = xElement.GetIntAttribute_e( "DefaultW" );
	m_iDefaultH  = xElement.GetIntAttribute_e( "DefaultH" );
	m_iAddHeight = xElement.GetIntAttribute_e( "AddHeight" );
	m_iStartXPos = xElement.GetIntAttribute_e( "StartXPos" );
	m_iStartYPos = xElement.GetIntAttribute_e( "StartYPos" );
	m_iWidthCut  = xElement.GetIntAttribute_e( "WidthCut" );
	m_iMaxLine   = xElement.GetIntAttribute_e( "MaxLine" );
	m_dwShowTime = xElement.GetIntAttribute_e( "ShowTime" );
	m_dwShowEventTime = xElement.GetIntAttribute_e( "ShowEventTime" );
}

void AwardChatBubbleWnd::StartChatBubble( const char *szBubbleName, const char *szChat, bool bWholeChat )
{
	if( szChat == NULL ) return;
	if( strlen( szChat ) == 0 ) return;

	HideWnd();
	m_BubbleList.clear();
	DWORD dwTextColor = 0xFF3A3A3A;
	if( bWholeChat )
	{	
		dwTextColor = 0xFF005A53;
	}

	int iLen = (int)strlen( szChat );
	int iResult = 0;
	for(int i = 0;i < m_iMaxLine;i++)
	{
		ChatBubble kBubble;
		kBubble.m_dwTextColor = dwTextColor;

		char szDst[MAX_PATH] = "";
		memset( szDst, 0, sizeof( szDst ) );
		if( i == m_iMaxLine - 1 )
		{
			sprintf( szDst, "%s", &szChat[iResult] ); // 마지막 채팅은 전부 넣는다.			
			iResult = iLen;
		}
		else
		{
			iResult += Help::StringCutFun( FONT_SIZE_12, m_iWidthCut, TS_NORMAL, szDst, sizeof( szDst ), &szChat[iResult] );
		}
		kBubble.m_szText = szDst;
		m_BubbleList.push_back( kBubble );

		if( iResult >= iLen ) break;
	}

	m_szBubbleName  = szBubbleName;
	m_dwShowAniTime = FRAMEGETTIME();
	m_dwStartTime   = FRAMEGETTIME();
	SetSize( m_iDefaultW, m_iDefaultH + (m_BubbleList.size() * m_iAddHeight) );
	SetWndPos( m_iDefaultX, m_iDefaultY - (m_BubbleList.size() * m_iAddHeight) );
	ShowWnd();

	AwardSlotWnd *pParentWnd = dynamic_cast<AwardSlotWnd*>(GetParent());
	if( pParentWnd && !pParentWnd->IsOriginal() )
	{
		// 롤링 중이면 Show Ani 안함
		m_dwShowAniTime = 0;
	}
}

void AwardChatBubbleWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	if( FRAMEGETTIME() - m_dwStartTime > m_dwShowTime )
	{
		HideWnd();
	}
}

bool AwardChatBubbleWnd::OnRenderShowAndHide()
{
	if( m_dwShowAniTime == 0 || m_dwShowEventTime == 0 ) return false;
	if( FRAMEGETTIME() - m_dwShowAniTime > m_dwShowEventTime )
	{
		m_dwShowAniTime = 0;
		if( m_pSelectedFrm )
		{
			m_pSelectedFrm->SetSize( GetWidth(), GetHeight() );
		}
		return false;
	}

	// 확대 & 축소를 20% ~ 100% 사이만 진행 시킨다.
	DWORD dwGapTime = FRAMEGETTIME() - m_dwShowAniTime;
	float fTimeRate = ( (float)dwGapTime / m_dwShowEventTime ) * FLOAT08;
	if( IsShow() )
	{
		fTimeRate = min( FLOAT08, fTimeRate ) + 0.2f;
	}
	else
	{
		fTimeRate = max( 0.0f, FLOAT08 - fTimeRate ) + 0.2f;
	}

	if( m_pSelectedFrm )
	{
		int iWidth = GetWidth() * fTimeRate;
		int iHeight= GetHeight() * fTimeRate;
		int iXPos  = GetDerivedPosX() + ( ( GetWidth() - iWidth ) / 2 );
		int iYPos  = GetDerivedPosY() + ( ( GetHeight() - iHeight ) / 2 );
		m_pSelectedFrm->SetSize( iWidth, iHeight );
		m_pSelectedFrm->Render( iXPos, iYPos );
	}		
	return true;
}

void AwardChatBubbleWnd::OnRender()
{
	AwardSlotWnd *pParentWnd = dynamic_cast<AwardSlotWnd*>(GetParent());
	if( pParentWnd && !pParentWnd->IsOriginal() ) 
	{
		// 롤링 중이면 표안함.
		return;
	}

	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	for(int i = 0;i < (int)m_BubbleList.size();i++)
	{
		ChatBubble &rkBubble = m_BubbleList[i];

		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( rkBubble.m_dwTextColor );
		g_FontMgr.PrintTextWidthCut( iXPos + m_iStartXPos, iYPos + m_iStartYPos + (i * m_iAddHeight), FONT_SIZE_12, m_iWidthCut, rkBubble.m_szText.c_str() );
	}

	if( m_pTooltipTail )
	{
		m_pTooltipTail->Render( iXPos, ( iYPos + GetHeight() ) - 2 );
	}
}
//////////////////////////////////////////////////////////////////////////
AwardSlotNameBtn::AwardSlotNameBtn()
{
	m_pThickBlueFrm = NULL;
	m_pLightBlueFrm = NULL;
	m_pThickRedFrm  = NULL;
	m_pLightRedFrm  = NULL;

	m_iClassType	= -1;
	m_iGradeLevel	= 0;
	m_eTeam			= TEAM_NONE;
	m_bOriginal     = false;
}

AwardSlotNameBtn::~AwardSlotNameBtn()
{
	SAFEDELETE( m_pThickBlueFrm );
	SAFEDELETE( m_pLightBlueFrm );
	SAFEDELETE( m_pThickRedFrm );
	SAFEDELETE( m_pLightRedFrm );
}

void AwardSlotNameBtn::SetAwardName( int iClassType, int iLevel, TeamType eTeam, const ioHashString &szName, bool bOriginal )
{
	m_iClassType  = iClassType;
	m_iGradeLevel = iLevel;
	m_eTeam       = eTeam;
	m_szName      = szName;
	m_bOriginal   = bOriginal;
	if( m_bOriginal )
		SetActive();
	else
		SetInActive();
}

void AwardSlotNameBtn::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "ThickBlueFrm" )
	{
		SAFEDELETE( m_pThickBlueFrm );
		m_pThickBlueFrm = pFrame;
	}
	else if( szType == "LightBlueFrm" )
	{
		SAFEDELETE( m_pLightBlueFrm );
		m_pLightBlueFrm = pFrame;
	}
	else if( szType == "ThickRedFrm" )
	{
		SAFEDELETE( m_pThickRedFrm );
		m_pThickRedFrm = pFrame;
	}
	else if( szType == "LightRedFrm" )
	{
		SAFEDELETE( m_pLightRedFrm );
		m_pLightRedFrm = pFrame;
	}
	else
	{
		ioButton::AddRenderFrame( szType, pFrame );
	}
}

void AwardSlotNameBtn::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;
	if( !GetParent() ) return;

	if( m_bOver && !m_szName.IsEmpty() )
	{
		ioLSMouse *pMouse = g_App.GetMouse();
		if( pMouse && pMouse->IsMouseShowing() )
		{
			pMouse->SetToolTipID( m_szName, m_iClassType );
		}
	}
}

void AwardSlotNameBtn::OnRender()
{
	ioButton::OnRender();

	if( m_szName.IsEmpty() ) return;

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextStyle( TS_NORMAL );	
	g_FontMgr.SetAlignType( TAT_LEFT );
	int iIDSize = g_FontMgr.GetTextWidthCutSize( m_szName.c_str(), TS_NORMAL, FONT_SIZE_12, FLOAT100 );
	int iStartX = (GetWidth() / 2) - ( ( iIDSize + 15 ) / 2 );
	switch( m_eTeam )
	{
	case TEAM_BLUE:
		if( m_bOver || m_bClicked || m_szName == g_MyInfo.GetPublicID() )
		{
			if( m_pThickBlueFrm )
			{
				m_pThickBlueFrm->Render( iXPos, iYPos );
			}
			g_LevelMgr.RenderGrade( iXPos + iStartX, iYPos + 2, m_iGradeLevel, TEAM_PRIVATE_1, UI_RENDER_NORMAL_ADD_COLOR, RGB( 255, 255, 255 ) );
			g_FontMgr.SetTextColor( 255, 255, 255 );		

		}
		else if( m_pLightBlueFrm )
		{
			m_pLightBlueFrm->Render( iXPos, iYPos );
			g_LevelMgr.RenderGrade( iXPos + iStartX, iYPos + 2, m_iGradeLevel, TEAM_PRIVATE_1 );
			g_FontMgr.SetTextColor( TCT_DEFAULT_BLUE );
		}
		break;
	default:
		if( m_bOver || m_bClicked || m_szName == g_MyInfo.GetPublicID() )
		{
			if( m_pThickRedFrm )
			{
				m_pThickRedFrm->Render( iXPos, iYPos );
			}
			g_LevelMgr.RenderGrade( iXPos + iStartX, iYPos + 2, m_iGradeLevel, TEAM_PRIVATE_1, UI_RENDER_NORMAL_ADD_COLOR, RGB( 255, 255, 255 ) );
			g_FontMgr.SetTextColor( 255, 255, 255 );
		}
		else if( m_pLightRedFrm )
		{
			m_pLightRedFrm->Render( iXPos, iYPos );
			g_LevelMgr.RenderGrade( iXPos + iStartX, iYPos + 2, m_iGradeLevel, TEAM_PRIVATE_1 );
			g_FontMgr.SetTextColor( TCT_DEFAULT_RED );
		}
		break;
	}
	g_FontMgr.PrintTextWidthCut( ( iXPos + iStartX ) + 15, iYPos + 3, FONT_SIZE_12, FLOAT100, m_szName.c_str() );
}
////////////////////////////////////////////////////////////////////////////
int AwardSlotWnd::sg_iMyAwardCount = 0;
AwardSlotWnd::AwardSlotWnd()
{
	m_pUI3DRender   = NULL;
	m_aMyChar	    = NULL;

	m_pBackEffect   = NULL;
	m_pCharBottom   = NULL;
	m_pTitleBack    = NULL;
	m_pEmptyAward   = NULL;
	m_pEtcItemIcon  = NULL;
	m_pSubIcon  = NULL;
	m_pEtcItemNum   = NULL;
	m_pEtcItemPlus  = NULL;

	m_iCurFrame		= 0;
	m_dwCurTimer	= 0;
	m_dwCurTickTime	= 100;
	m_dwRandTime	= 1000;
	m_dwRandTickTime= 100;
	m_dwOriginalTime= 1000;
	m_dwSlowTime	= 1000;
	m_bRandTimeEnd  = true;
	m_bCharOver     = false;
	m_fOriginalRate = FLOAT1;
	sg_iMyAwardCount= 0;
}

AwardSlotWnd::~AwardSlotWnd()
{
	SAFEDELETE( m_aMyChar );
	SAFEDELETE( m_pUI3DRender );

	SAFEDELETE( m_pBackEffect );
	SAFEDELETE( m_pCharBottom );
	SAFEDELETE( m_pTitleBack );
	SAFEDELETE( m_pEmptyAward );
	SAFEDELETE( m_pEtcItemIcon );
	SAFEDELETE( m_pSubIcon );
	SAFEDELETE( m_pEtcItemNum );
	SAFEDELETE( m_pEtcItemPlus );
}

void AwardSlotWnd::iwm_create()
{
	SAFEDELETE( m_pUI3DRender );

	m_pUI3DRender = new ioUI3DRender;
	m_pUI3DRender->Initialize( "config/sp2_stage_charselect_map.ini", "award_cam" );

	g_GUIMgr.AddWnd( "XML/awardchatbubblewnd.xml", this );
}

void AwardSlotWnd::iwm_show()
{
	SetChildInActive( ID_DUMMY );

	if( COMPARE( m_CurAwardInfo.m_iType, 1, g_AwardMgr.GetMaxAward() + 1 ) )
	{
		m_dwCurTimer	= FRAMEGETTIME();
		m_bRandTimeEnd	= false;		
		m_bOriginal     = false;

		if( GetID() == TotalResultAwardWnd::ID_AWARD_1 )
		{
			PlayCountSound();
		}
	}
	else
	{
		// 시상 없음
		if( GetParent() )
		{
			GetParent()->iwm_command( this, AWARD_COMPLETE, 0 );
		}
		m_bOriginal = true;
	}
	m_fOriginalRate = FLOAT1;
	m_bCharOver = false;

	// 모션 정보 유저들에게 전송
	if( g_MyInfo.GetPublicID() == m_AwardInfo.m_szName )
	{
		if( m_AwardInfo.m_dwCharMotionItem != 0 )
		{		
			SP2Packet kPacket( CUPK_ETCITEM_MOTION_OPTION_SYNC );
			kPacket << m_AwardInfo.m_szName << (ioEtcItemMotion::MOTION_OPTION_3 + m_AwardInfo.m_iGoodBadType) << m_AwardInfo.m_dwCharMotionItem;
			kPacket << m_AwardInfo.m_iType;
			P2PNetwork::SendToAllUser( kPacket );
		}
	}
}

void AwardSlotWnd::iwm_hide()
{
	HideChildWnd( ID_DUMMY );
}

void AwardSlotWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_NAME:
		if( m_bOriginal )
		{
			if( cmd == IOBN_BTNUP )
			{
				AwardSlotNameBtn *pNameBtn = dynamic_cast<AwardSlotNameBtn *>(pWnd);
				if( pNameBtn )
				{
					if( !pNameBtn->GetName().IsEmpty() )
					{
						g_App.OpenMannerWnd( pNameBtn->GetName().c_str(), true );
					}
				}
			}
			else if( cmd == IOWN_OVERED )
				m_bCharOver = true;
			else if( cmd == IOWN_LEAVED )
				m_bCharOver = false;
		}
		break;
	case ID_DUMMY:
		if( m_bOriginal )
		{
			if( cmd == IOWN_OVERED )
				m_bCharOver = true;
			else if( cmd == IOWN_LEAVED )
				m_bCharOver = false;
		}
		break;
	}
}

void AwardSlotWnd::CreateCharacter( const CHARACTER &rkInfo, const ITEM_DATA *pItem, const int iAniCode, const ioHashString &szAniName, DWORD dwCharMotionItem, const ioHashString &szID )
{
	ioCreateChar *pChar = m_pUI3DRender->CreateChar( rkInfo, "AwardChar" );

	D3DXVECTOR3 vCharPos( 0.0f, 0.0f, 675.0f );
	pChar->SetPosition( vCharPos );
	pChar->SetYaw( FLOAT10 );

	m_pUI3DRender->SetLookAt( vCharPos );

	// 코스튬 관련 (수상 관련)
	int TempCostume[MAX_INVENTORY];
	for ( int i=0; i<MAX_INVENTORY; i++ )
	{
		TempCostume[i] = rkInfo.m_EquipCostume[i].m_CostumeCode;

		// 코스튬 관련 (자신만 적용)
		if ( !Setting::IsCostumeShowChange() && !g_CostumeInfoMgr.IsEnableCostumeMode() && szID != g_MyInfo.GetPublicID() )
			TempCostume[i] = 0;
	}

	const ioSetItemInfo *pSetInfo = g_SetItemInfoMgr.GetSetInfoByIdx( rkInfo.m_class_type - 1 );
	if( pSetInfo )
	{
		ConstItemVector rkItemList = pSetInfo->GetSetItemList();

		int iMaxItemCnt = min( rkItemList.size(), MAX_INVENTORY );
		for(int i = 0;i < iMaxItemCnt;i++)
		{
			ITEM_DATA kItem = pItem[i];
			int iItemCode = kItem.m_item_code;
			DWORD dwCostumeMaleCustom = rkInfo.m_EquipCostume[i].m_Costume_Male_Custom;
			DWORD dwCostumeFemaleCustom = rkInfo.m_EquipCostume[i].m_Costume_Female_Custom;
			
			if( iItemCode == 0 )
			{
				if ( rkInfo.m_EquipCostume[i].m_CostumeCode <= 0 )
				{
					dwCostumeMaleCustom = 0;
					dwCostumeFemaleCustom = 0;
				}
				pChar->EquipItem( rkItemList[i]->GetItemCode(), dwCostumeMaleCustom, dwCostumeFemaleCustom, TempCostume[i] );
			}
			else
			{
				if ( rkInfo.m_EquipCostume[i].m_CostumeCode <= 0 )
				{
					dwCostumeMaleCustom = kItem.m_item_male_custom;
					dwCostumeFemaleCustom = kItem.m_item_female_custom;
				}
				pChar->EquipItem( kItem.m_item_code, dwCostumeMaleCustom, dwCostumeFemaleCustom, TempCostume[i] );
			}
		}
	}

	ioHashString kMotionAni;
	ioEtcItemMotion *pMotionItem = dynamic_cast< ioEtcItemMotion * > ( g_EtcItemMgr.FindEtcItem( dwCharMotionItem ) );
	if( pMotionItem )
	{
		if( pChar->IsMale() )
			kMotionAni = pMotionItem->GetHumanManMotion();
		else
			kMotionAni = pMotionItem->GetHumanWomanMotion();
	}

	if( kMotionAni.IsEmpty() )
	{
		if( !szAniName.IsEmpty() )
			pChar->SetLoopAni( szAniName );
		else // 애니메이션 코드
			pChar->SetAwardWeaponMotion( iAniCode );
	}
	else
	{
		pChar->SetMotionItemAni( kMotionAni );
	}	

	if( !m_szAwakeEffect.IsEmpty() )
		pChar->EndEffect( m_szAwakeEffect, true );

	m_szAwakeEffect = g_AwakeMgr.GetAwakeEffect( static_cast<AwakeType>( rkInfo.m_iAwakeType ) ).c_str();
	pChar->AttachEffect( m_szAwakeEffect, NULL );
	
	SAFEDELETE( m_aMyChar );
	m_aMyChar = pChar;
}

void AwardSlotWnd::SetAward( SP2Packet &rkPacket )
{
	rkPacket >> m_AwardInfo.m_iType >> m_AwardInfo.m_iLevel >> m_AwardInfo.m_iTeam >> m_AwardInfo.m_szName 
			 >> m_AwardInfo.m_iValue >> m_AwardInfo.m_iPoint >> m_AwardInfo.m_fAwardBonus >> m_AwardInfo.m_iEtcItemType >> m_AwardInfo.m_iEtcItemCount
			 >> m_AwardInfo.m_charInfo;

	rkPacket >> m_AwardInfo.m_iClassLevel;

	for( int i=0; i < MAX_INVENTORY; ++i )
	{
		rkPacket >> m_AwardInfo.m_EquipItem[i].m_item_code;
		rkPacket >> m_AwardInfo.m_EquipItem[i].m_item_reinforce;
		rkPacket >> m_AwardInfo.m_EquipItem[i].m_item_male_custom;
		rkPacket >> m_AwardInfo.m_EquipItem[i].m_item_female_custom;
	}

	if( ioPlayMode::GetModeType() == MT_ARENA )
	{
		memcpy_s( &(m_AwardInfo.m_charInfo), sizeof(CHARACTER), &(g_MyInfo.GetCharacterToClassType( m_AwardInfo.m_charInfo.m_class_type )), sizeof(CHARACTER) );

		for( int i=0; i < MAX_INVENTORY; ++i )
		{
			if( i == 0 )
				m_AwardInfo.m_EquipItem[i].m_item_code = ConvertPowerUpCharToItem( m_AwardInfo.m_charInfo.m_eCharPowerUpType ) + m_AwardInfo.m_charInfo.m_class_type;
			else
				m_AwardInfo.m_EquipItem[i].m_item_code = m_AwardInfo.m_charInfo.m_class_type + ( i * 100000 );
			m_AwardInfo.m_EquipItem[i].m_item_reinforce = 0;
			m_AwardInfo.m_EquipItem[i].m_item_male_custom = 0;
			m_AwardInfo.m_EquipItem[i].m_item_female_custom = 0;
		}
	}

	m_AwardInfo.m_vMedalItemTypeVec.clear();
	for (int i = 0; i < ioMedalItemInfoManager::MAX_SLOT_NUM ; i++)
	{
		MedalSyncInfo cInfo;
		rkPacket >> cInfo.m_iItemType;
		if( cInfo.m_iItemType != 0 )
			m_AwardInfo.m_vMedalItemTypeVec.push_back( cInfo );
	}
	for (int i = 0; i < ioMedalItemInfoManager::MAX_SLOT_NUM ; i++)
	{
		MedalSyncInfo cInfo;
		rkPacket >> cInfo.m_iItemType >> cInfo.m_iCustomIndex;
		if( cInfo.m_iCustomIndex != 0 )
		{
			for( int j = 0; j < 8; j++ )
				rkPacket >> cInfo.m_iStat[j];

			m_AwardInfo.m_vMedalItemTypeVec.push_back( cInfo );
		}
	}

	if( m_AwardInfo.m_iType == ioAwardManager::AC_LUCKY )
	{
		// 선물 내용 뽑아내기~
		rkPacket >> m_AwardInfo.m_iPresentType >> m_AwardInfo.m_iPresentValue1 >> m_AwardInfo.m_iPresentValue2 >> m_AwardInfo.m_iPresentValue3 >> m_AwardInfo.m_iPresentValue4 >> m_AwardInfo.m_bGlobalAlarm;
	}

	m_AwardInfo.m_iGoodBadType = g_AwardMgr.GetGoodBadType( m_AwardInfo.m_iType );
	if( m_AwardInfo.m_szName == g_MyInfo.GetPublicID() )
	{
		if( m_AwardInfo.m_iType != 0 )
			g_MyInfo.AddAwardData( m_AwardInfo.m_iType, m_AwardInfo.m_iPoint );
	}

	// 모션 아이템
	if( m_AwardInfo.m_szName == g_MyInfo.GetPublicID() )
	{
		m_AwardInfo.m_dwCharMotionItem = g_MyInfo.GetEtcMotionAni( ioEtcItemMotion::MOTION_OPTION_3 + m_AwardInfo.m_iGoodBadType );	
	}

	m_CurAwardInfo = m_AwardInfo;
	SAFEDELETE( m_pEtcItemIcon );
	SAFEDELETE( m_pSubIcon );
}

void AwardSlotWnd::AddFinalInfo( const FinalInfo &rkInfo )
{
	m_FinalInfoList.push_back( rkInfo );
}

void AwardSlotWnd::PlayCountSound()
{
	if( COMPARE( m_AwardInfo.m_iType, 1, g_AwardMgr.GetMaxAward() + 1 ) )
	{
		g_SoundMgr.PlaySound( m_szCoundSound );
	}	
}

bool AwardSlotWnd::IsOriginal()
{
	// 수상이 없으면 무조건 오리지날~
	if( !COMPARE( m_AwardInfo.m_iType, 1, g_AwardMgr.GetMaxAward() + 1 ) ) 
		return true;

	return m_bOriginal;
}

void AwardSlotWnd::SetOriginalAward()
{
	if( !COMPARE( m_AwardInfo.m_iType, 1, g_AwardMgr.GetMaxAward() + 1 ) ) return;

	m_CurAwardInfo = m_AwardInfo;
	// 이름
	AwardSlotNameBtn *pName = (AwardSlotNameBtn *)FindChildWnd( ID_NAME );
	if( pName )
	{
		TeamType eTeamType = (TeamType)m_AwardInfo.m_iTeam;
		if( eTeamType != TEAM_RED && eTeamType != TEAM_BLUE )
		{
			if( m_AwardInfo.m_szName == g_MyInfo.GetPublicID() )
				eTeamType = TEAM_BLUE;
			else
				eTeamType = TEAM_RED;
		}
		pName->SetAwardName( m_AwardInfo.m_charInfo.m_class_type, m_AwardInfo.m_iLevel, eTeamType, m_AwardInfo.m_szName, true );
	}

	// 캐릭터
	CreateCharacter( m_AwardInfo.m_charInfo, (const ITEM_DATA*)&m_AwardInfo.m_EquipItem, m_AwardInfo.m_iGoodBadType, "", m_AwardInfo.m_dwCharMotionItem, m_AwardInfo.m_szName );
	if( m_aMyChar )
		m_aMyChar->AttachEffect( "award_eff.txt", NULL );

	const ioHashString &rkSound = g_AwardMgr.GetAwardSound( m_AwardInfo.m_iType );
	if( !rkSound.IsEmpty() )
		g_SoundMgr.PlaySound( rkSound );

	if( GetParent() )
	{
		GetParent()->iwm_command( this, AWARD_COMPLETE, 0 );
	}

	if( g_MyInfo.GetPublicID() == m_AwardInfo.m_szName )
	{
		sg_iMyAwardCount++;
		g_QuestMgr.QuestCompleteTerm( QC_AWARD_ACQUIRE, m_AwardInfo.m_iType, m_AwardInfo.m_iPoint );
	}

	if( sg_iMyAwardCount > 0 && ioPlayMode::GetModeType() != MT_MONSTER_SURVIVAL && 
		ioPlayMode::GetModeType() != MT_DUNGEON_A && 
		ioPlayMode::GetModeType() != MT_RAID && 
		!Help::IsMonsterDungeonMode(ioPlayMode::GetModeType()) )
	{
		g_QuestMgr.QuestCompleteTerm( QC_BATTLE_PVP_AWARD_ROW_ACQUIRE, sg_iMyAwardCount );
	}

	if( m_CurAwardInfo.m_iEtcItemType > 0 && m_CurAwardInfo.m_iEtcItemCount > 0 )
	{
		SAFEDELETE( m_pEtcItemIcon );
		SAFEDELETE( m_pSubIcon );
		ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( m_CurAwardInfo.m_iEtcItemType );
		if( pEtcItem )
		{
			m_pEtcItemIcon = g_UIImageSetMgr.CreateImageByFullName( pEtcItem->GetIconName() );
			m_pSubIcon = g_UIImageSetMgr.CreateImageByFullName( pEtcItem->GetSubIconName() );
		}
	}

	m_bOriginal = true;
	m_dwCurTimer    = FRAMEGETTIME();
	m_fOriginalRate = 2.0f;
	SetChildActive( ID_DUMMY );
}

void AwardSlotWnd::OnProcessRand()
{
	if( m_bOriginal ) return;

	DWORD dwGap = FRAMEGETTIME() - m_dwCurTimer;
	DWORD dwTickTime = m_dwCurTickTime * m_iCurFrame;

	if( dwGap > dwTickTime )
	{
		if( m_bRandTimeEnd )
		{
			if( dwGap > m_dwSlowTime )
			{
				m_dwCurTimer = 0;
				SetOriginalAward();
				return;
			}
			else
			{
				m_dwCurTickTime = m_dwRandTickTime;// + ( (float)m_dwRandTickTime * sinf( ( D3DX_PI/2 * dwGap ) / m_dwSlowTime ) );
			}
		}		

		m_iCurFrame++;

		m_CurAwardInfo.m_iType  = g_AwardMgr.GetRandAwardType();
		m_CurAwardInfo.m_iValue = rand()%100 + 50;
		m_CurAwardInfo.m_iPoint = rand()%800 + 100;
		m_CurAwardInfo.m_charInfo = g_GlobalSoldierInfo.GetAwardRandSoldier( m_iCurFrame );

		// 이름
		AwardSlotNameBtn *pName = (AwardSlotNameBtn *)FindChildWnd( ID_NAME );
		if( pName )
		{
			if( m_FinalInfoList.empty() )
				pName->SetAwardName( m_AwardInfo.m_charInfo.m_class_type, m_AwardInfo.m_iLevel, (TeamType)m_AwardInfo.m_iTeam, m_AwardInfo.m_szName );
			else
			{
				int iUserArray = rand()%m_FinalInfoList.size();
				FinalInfo &kInfo = m_FinalInfoList[iUserArray];
				TeamType eTeamType = (TeamType)kInfo.m_Team;
				if( eTeamType != TEAM_RED && eTeamType != TEAM_BLUE )
				{
					if( m_AwardInfo.m_szName == g_MyInfo.GetPublicID() )
						eTeamType = TEAM_BLUE;
					else
						eTeamType = TEAM_RED;
				}
				pName->SetAwardName( m_CurAwardInfo.m_charInfo.m_class_type, kInfo.m_iLevel, eTeamType, kInfo.m_szName );
			}
		}

		// 캐릭터
		ITEM_DATA kEquipItem[MAX_INVENTORY];
		for( int i = 0; i < MAX_INVENTORY; i++ )
			kEquipItem[i].Initialize();

		CreateCharacter( m_CurAwardInfo.m_charInfo, (const ITEM_DATA*)&kEquipItem, 0, g_AwardMgr.GetAwardAniName( m_CurAwardInfo.m_iType ), 0, m_CurAwardInfo.m_szName );
	}

	if( !m_bRandTimeEnd )
	{
		if( dwGap > m_dwRandTime && m_dwRandTime != 0 )
		{
			m_dwCurTimer	= FRAMEGETTIME();
			m_dwRandTime	= 0;
			m_iCurFrame		= 1;
			m_bRandTimeEnd	= true;
		}
	}
}

void AwardSlotWnd::OnProcessOriginal()
{
	if( !m_bOriginal ) return;
	if( m_dwOriginalTime == 0 ) return;
	if( m_dwCurTimer == 0 ) return;
	
	DWORD dwGap = FRAMEGETTIME() - m_dwCurTimer;
	if( dwGap < m_dwOriginalTime )
	{
		float fRate = FLOAT1 - ( (float)dwGap / m_dwOriginalTime );
		m_fOriginalRate = FLOAT1 + fRate;
	}
	else
	{
		m_dwCurTimer = 0;
		m_fOriginalRate = FLOAT1;
	}
}

void AwardSlotWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_dwRandTime		= xElement.GetIntAttribute_e( "RandTime" );
	m_dwRandTickTime	= xElement.GetIntAttribute_e( "RandTickTime" );
	m_dwOriginalTime    = xElement.GetIntAttribute_e( "OriginalTime" );
	m_szCoundSound      = xElement.GetStringAttribute_e( "CountSound" );
}

void AwardSlotWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "BackEffect" )
	{
		SAFEDELETE( m_pBackEffect );
		m_pBackEffect = pImage;
	}
	else if( szType == "CharBottom" )
	{
		SAFEDELETE( m_pCharBottom );
		m_pCharBottom = pImage;
	}
	else if( szType == "TitleBack" )
	{
		SAFEDELETE( m_pTitleBack );
		m_pTitleBack = pImage;
	}
	else if( szType == "EmptyAward" )
	{
		SAFEDELETE( m_pEmptyAward );
		m_pEmptyAward = pImage;
	}
	else if( szType == "EtcItemNum" )
	{
		SAFEDELETE( m_pEtcItemNum );
		m_pEtcItemNum = pImage;
	}
	else if( szType == "EtcItemPlus" )
	{
		SAFEDELETE( m_pEtcItemPlus );
		m_pEtcItemPlus = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void AwardSlotWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	if( m_aMyChar )
	{		
		m_aMyChar->Update( fTimePerSec );
	}

	OnProcessRand();
	OnProcessOriginal();
	if( m_bCharOver && !m_AwardInfo.m_szName.IsEmpty() )
	{
		ioLSMouse *pMouse = g_App.GetMouse();
		if( pMouse && pMouse->IsMouseShowing() )
		{
			pMouse->SetToolTipID( m_CurAwardInfo.m_szName, m_CurAwardInfo.m_charInfo.m_class_type );
		}
	}
}

void AwardSlotWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	if( m_pBackEffect )
	{
		m_pBackEffect->SetScale( 0.70f );
		m_pBackEffect->Render( iXPos + 62, iYPos + 172, UI_RENDER_MULTIPLY, TFO_BILINEAR );
	}

	if( m_pCharBottom )
	{
		m_pCharBottom->SetScale( 0.60f );
		m_pCharBottom->Render( iXPos + 62 - (m_pCharBottom->GetWidth() / 2), iYPos + 252 - (m_pCharBottom->GetHeight() / 2), UI_RENDER_MULTIPLY, TFO_BILINEAR );
	}

	if( m_pUI3DRender )
	{
		D3DXMATRIX matUI = m_pUI3DRender->MakeUIProjection( iXPos - 340, iYPos - 50 );
		m_pUI3DRender->Render( &matUI, true );
	}

	if( m_pTitleBack )
		m_pTitleBack->Render( iXPos + 62, iYPos + 156 );

	if( !COMPARE( m_CurAwardInfo.m_iType, 1, g_AwardMgr.GetMaxAward() + 1 ) ) 
	{
		if( m_pEmptyAward )
		{
			m_pEmptyAward->Render( iXPos + 62, iYPos + 156 );
		}		
	}
	else
	{
		if( m_pEtcItemIcon )
		{
			// 이벤트 특별 아이템을 받았다.
			m_pEtcItemIcon->SetScale( 0.85f );
			m_pEtcItemIcon->SetAlpha( MAX_ALPHA_RATE );
			m_pEtcItemIcon->Render( iXPos + 62, iYPos + 41, UI_RENDER_NORMAL, TFO_BILINEAR );
			
			if( m_bOriginal )
			{
				if( m_fOriginalRate > FLOAT1 )
				{
					float fRate = m_fOriginalRate - FLOAT1;
					m_pEtcItemIcon->SetColor( TCT_DEFAULT_WHITE );
					m_pEtcItemIcon->SetAlpha( (float)MAX_ALPHA_RATE * fRate );
					m_pEtcItemIcon->Render( iXPos + 62, iYPos + 41, UI_RENDER_NORMAL_ADD_COLOR, TFO_BILINEAR );
				}
			}

			if( m_pSubIcon )
			{
				// 이벤트 특별 아이템을 받았다.
				m_pSubIcon->SetScale( 0.85f );
				m_pSubIcon->SetAlpha( MAX_ALPHA_RATE );
				m_pSubIcon->Render( iXPos + 62, iYPos + 41, UI_RENDER_NORMAL, TFO_BILINEAR );

				if( m_bOriginal )
				{
					if( m_fOriginalRate > FLOAT1 )
					{
						float fRate = m_fOriginalRate - FLOAT1;
						m_pSubIcon->SetColor( TCT_DEFAULT_WHITE );
						m_pSubIcon->SetAlpha( (float)MAX_ALPHA_RATE * fRate );
						m_pSubIcon->Render( iXPos + 62, iYPos + 41, UI_RENDER_NORMAL_ADD_COLOR, TFO_BILINEAR );
					}
				}			
			}
			RenderNumInfo( iXPos + 62, iYPos + 41 );
		}
		else
		{
			ioUIRenderImage *pIcon = g_AwardMgr.GetAwardIcon( m_CurAwardInfo.m_iType );
			if( pIcon )
			{
				pIcon->SetScale( 0.85f );
				pIcon->SetAlpha( MAX_ALPHA_RATE );
				pIcon->Render( iXPos + 62, iYPos + 41, UI_RENDER_NORMAL, TFO_BILINEAR );

				if( m_bOriginal )
				{
					if( m_fOriginalRate > FLOAT1 )
					{
						float fRate = m_fOriginalRate - FLOAT1;
						pIcon->SetColor( TCT_DEFAULT_WHITE );
						pIcon->SetAlpha( (float)MAX_ALPHA_RATE * fRate );
						pIcon->Render( iXPos + 62, iYPos + 41, UI_RENDER_NORMAL_ADD_COLOR, TFO_BILINEAR );
					}
				}
			}
		}	

		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
		g_FontMgr.PrintText( iXPos + 62, iYPos + 122, FONT_SIZE_17, STR(1), (int)( m_CurAwardInfo.m_iPoint + 100 ) / 100 );

		g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_3X );	
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetTextColor( 193, 193, 193 );
		g_FontMgr.PrintText( iXPos + 62, iYPos + 172, FONT_SIZE_11, g_AwardMgr.GetAwardDesc( m_CurAwardInfo.m_iType ).c_str(), m_CurAwardInfo.m_iValue );

		g_FontMgr.SetTextColor( 255, 204, 0 );
		if( m_CurAwardInfo.m_fAwardBonus > 0.0f )
		{
			int iAwardBonus = abs( ( m_CurAwardInfo.m_fAwardBonus + 0.005 ) * 100 );
			g_FontMgr.PrintText( iXPos + 62, iYPos + 188, FONT_SIZE_13, STR(2), iAwardBonus );

		}
		else if( m_CurAwardInfo.m_iPresentType > 0 )
		{
			char szPresentName[MAX_PATH] = "";
			sprintf( szPresentName, "%s %s", g_PresentMgr.GetPresentValue1Text( m_CurAwardInfo.m_iPresentType, m_CurAwardInfo.m_iPresentValue1, m_CurAwardInfo.m_iPresentValue2 ).c_str(),
											 g_PresentMgr.GetPresentValue2Text( m_CurAwardInfo.m_iPresentType, m_CurAwardInfo.m_iPresentValue1, m_CurAwardInfo.m_iPresentValue2 ).c_str() );
			g_FontMgr.PrintText( iXPos + 62, iYPos + 188, FONT_SIZE_13, szPresentName );
		}

		// 
		ioUIRenderImage *pTitle = g_AwardMgr.GetAwardTitleRes( m_CurAwardInfo.m_iType );
		if( pTitle )
		{
			if( m_bOriginal )
			{
				pTitle->SetScale( max( FLOAT1, m_fOriginalRate ) );			
			}
			else
			{
				pTitle->SetScale( FLOAT1 );				
			}
			pTitle->Render( iXPos + 62, iYPos + 140, UI_RENDER_NORMAL, TFO_BILINEAR );
		}
	}
}

void AwardSlotWnd::RenderNumInfo( int iXPos, int iYPos )
{
	if( !m_pEtcItemNum || !m_pEtcItemPlus )
		return;
	if( m_CurAwardInfo.m_iEtcItemCount <= 0 )
		return;

	int iNewXPos = iXPos;
	int iNewYPos = iYPos;

	// Num
	enum
	{
		REINFORCE_Y	= 14,

		NUM_SIZE	= 20,
		PLUS_SIZE	= 14,

		NUM_GAP		= 4,
	};
	
	int iCurPos = 0;
	int iDefaultPos = iNewXPos;

	int iCurYPos = iNewYPos+(REINFORCE_Y * 0.85f);

	int iTotalSize = 0;
	if( m_CurAwardInfo.m_iEtcItemCount < 10 )
		iTotalSize = PLUS_SIZE + NUM_SIZE - NUM_GAP;
	else if( m_CurAwardInfo.m_iEtcItemCount < 100 )
		iTotalSize = PLUS_SIZE + NUM_SIZE*2 - NUM_GAP*2;
	else
		iTotalSize = PLUS_SIZE + NUM_SIZE*3 - NUM_GAP*3;

	iCurPos = iDefaultPos - (iTotalSize / 2);

	m_pEtcItemPlus->Render( iCurPos, iCurYPos );

	iCurPos += (PLUS_SIZE-NUM_GAP);

	m_pEtcItemNum->RenderNum( iCurPos, iCurYPos, m_CurAwardInfo.m_iEtcItemCount, -NUM_GAP );
}

const ioHashString& AwardSlotWnd::GetCharName()
{
	return m_AwardInfo.m_szName;
}

int AwardSlotWnd::GetAwardType()
{
	return m_AwardInfo.m_iType;
}

int AwardSlotWnd::GetItemReinforce( int iSlot )
{
	if( COMPARE( iSlot, 0, MAX_INVENTORY ) )
		return m_AwardInfo.m_EquipItem[iSlot].m_item_reinforce;

	return 0;
}

int AwardSlotWnd::GetCharLevel()
{
	return m_AwardInfo.m_iClassLevel;
}

void AwardSlotWnd::GetMedalItemType( OUT MedalSyncInfoVec &rvMedalItemType )
{
	rvMedalItemType = m_AwardInfo.m_vMedalItemTypeVec;
}

void AwardSlotWnd::SetAwardCharMotion( DWORD dwCharMotion )
{
	m_AwardInfo.m_dwCharMotionItem = dwCharMotion;

	if( IsOriginal() )
	{
		if( m_aMyChar )
		{
			ioHashString kMotionAni;
			ioEtcItemMotion *pMotionItem = dynamic_cast< ioEtcItemMotion * > ( g_EtcItemMgr.FindEtcItem( m_AwardInfo.m_dwCharMotionItem ) );
			if( pMotionItem )
			{
				if( m_aMyChar->IsMale() )
					kMotionAni = pMotionItem->GetHumanManMotion();
				else
					kMotionAni = pMotionItem->GetHumanWomanMotion();
			}

			if( !kMotionAni.IsEmpty() )
			{
				m_aMyChar->SetMotionItemAni( kMotionAni );
			}
		}	
	}
}

void AwardSlotWnd::StartChatBubble( const char *szBubbleName, const char *szChat, bool bWholeChat )
{
	AwardChatBubbleWnd *pBubbleWnd = dynamic_cast<AwardChatBubbleWnd*>(FindChildWnd( ID_CHAT_BUBBLE ));
	if( pBubbleWnd )
		pBubbleWnd->StartChatBubble( szBubbleName, szChat, bWholeChat );
}
////////////////////////////////////////////////////////
TotalResultAwardWnd::TotalResultAwardWnd()
{
	m_dwCurTime   = 0;
	m_dwShowTime  = 0;
	m_iAwardCount = 0;
	m_bPresentAlarmBlock = false;
}

TotalResultAwardWnd::~TotalResultAwardWnd()
{
	if( !m_BackSound.IsEmpty() )
		g_SoundMgr.StopSound( m_BackSound, 0 );
}

void TotalResultAwardWnd::SetPlayStage( ioPlayStage *pPlayStage )
{
	m_pPlayStage = pPlayStage;
}

void TotalResultAwardWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_BackSound  = xElement.GetStringAttribute_e( "BackSound" );
}

void TotalResultAwardWnd::iwm_show()
{
	ShowChildWnd( ID_AWARD_1 );
	ShowChildWnd( ID_AWARD_2 );
	ShowChildWnd( ID_AWARD_3 );
	ShowChildWnd( ID_AWARD_4 );

	m_dwCurTime = FRAMEGETTIME();

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

	if( !m_BackSound.IsEmpty() )
		g_SoundMgr.PlaySound( m_BackSound );

	g_ExSoundMgr.PlaySound( ExSound::EST_AWARD );
}

void TotalResultAwardWnd::iwm_hide()
{
	HideChildWnd( ID_AWARD_1 );
	HideChildWnd( ID_AWARD_2 );
	HideChildWnd( ID_AWARD_3 );
	HideChildWnd( ID_AWARD_4 );

	if( GetParent() )
		GetParent()->iwm_command( this, AWARD_TIME_END, 0 );
}

void TotalResultAwardWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_AWARD_1:
	case ID_AWARD_2:
	case ID_AWARD_3:
	case ID_AWARD_4:
		if( cmd == AwardSlotWnd::AWARD_COMPLETE )
		{
			if( dwID == (DWORD)m_iAwardCount )  // 마지막 롤링 끝...
			{
				// 롤링 End
				if( GetParent() )
					GetParent()->iwm_command( this, AWARD_ROLLING_END, 0 );
			}

			if( dwID++ <= ID_AWARD_4 )
			{
				AwardSlotWnd *pAwardWnd = dynamic_cast<AwardSlotWnd*>(FindChildWnd( dwID ));
				if( pAwardWnd )
					pAwardWnd->PlayCountSound();
			}
		}
		break;
	}
}

void TotalResultAwardWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;    	

	if( m_dwCurTime != 0 )
	{
		if( FRAMEGETTIME() - m_dwCurTime > m_dwShowTime )
		{
			m_dwCurTime = 0;			
		}
	}	

	if( m_dwCurTime == 0 )
		HideWnd();
}

void TotalResultAwardWnd::SetAwarding( SP2Packet &rkPacket )
{
	rkPacket >> m_dwShowTime >> m_iAwardCount;

	for( int i = 0;i < m_iAwardCount;i++)
	{
		AwardSlotWnd *pAward = dynamic_cast<AwardSlotWnd*>(FindChildWnd( ID_AWARD_1 + i ));
		if( pAward )
		{
			pAward->SetAward( rkPacket );
		}
	}	

	if( m_iAwardCount > 0 )
		m_bPresentAlarmBlock = true;         

	// 시상식 ShowTime에서 최종 결과 브리핑시간을 제외한다.
	BriefingWnd *pBriefingWnd = dynamic_cast<BriefingWnd*>( g_GUIMgr.FindWnd( BRIEFING_WND ) );
	if( pBriefingWnd )
	{
		m_dwShowTime -= pBriefingWnd->GetTotalResultTime();
	}
}

void TotalResultAwardWnd::SetAwardingCharMotion( const ioHashString &rkCharName, int iAwardType, DWORD dwCharMotionItem )
{
	for( int i = ID_AWARD_1;i < ID_AWARD_4+1;i++)
	{
		AwardSlotWnd *pAward = dynamic_cast<AwardSlotWnd*>(FindChildWnd( i ));
		if( !pAward ) continue;
		if( pAward->GetCharName() != rkCharName ) continue;
		if( pAward->GetAwardType() != iAwardType ) continue;
		
		pAward->SetAwardCharMotion( dwCharMotionItem );
		break;
	}	
}

void TotalResultAwardWnd::AddFinalInfo( const FinalInfo &rkInfo )
{
	for( int i = ID_AWARD_1;i < ID_AWARD_4+1;i++)
	{
		AwardSlotWnd *pAward = (AwardSlotWnd *)FindChildWnd( i );
		if( pAward )
			pAward->AddFinalInfo( rkInfo );
	}

	m_FinalInfoList.push_back( rkInfo );
}

int TotalResultAwardWnd::GetMyTeam()
{
	int i = 0;
	int iUserSize = m_FinalInfoList.size();
	for( i = 0 ; i < iUserSize ; i++ )
	{
		FinalInfo kInfo = m_FinalInfoList[i];
		if( kInfo.m_bOwner )
			return kInfo.m_Team;
	}

	return TEAM_NONE;
}

bool TotalResultAwardWnd::IsPresentAlarmBlock()
{
	if( !m_bPresentAlarmBlock ) 
		return false;

	for( int i = ID_AWARD_1;i < ID_AWARD_4+1;i++)
	{
		AwardSlotWnd *pAward = (AwardSlotWnd *)FindChildWnd( i );
		if( pAward && !pAward->IsOriginal() )
			return true;
	}

	m_bPresentAlarmBlock = false;
	return m_bPresentAlarmBlock;
}

int TotalResultAwardWnd::GetAwardCharItemReinforce( const ioHashString &szName, int iSlot )
{
	if( szName.IsEmpty() )
		return 0;

	for( int i = 0;i < m_iAwardCount;i++)
	{
		AwardSlotWnd *pAward = (AwardSlotWnd *)FindChildWnd( ID_AWARD_1 + i );
		if( pAward && pAward->GetCharName() == szName )
		{
			return pAward->GetItemReinforce( iSlot );
		}
	}
	return 0;
}

int TotalResultAwardWnd::GetAwardCharLevel( const ioHashString &szName )
{
	if( szName.IsEmpty() )
		return 0;

	for( int i = 0;i < m_iAwardCount;i++)
	{
		AwardSlotWnd *pAward = (AwardSlotWnd *)FindChildWnd( ID_AWARD_1 + i );
		if( pAward && pAward->GetCharName() == szName )
		{
			return pAward->GetCharLevel();
		}
	}
	return 0;
}

void TotalResultAwardWnd::GetMedalItemType( IN const ioHashString &szName, OUT MedalSyncInfoVec &rvMedalItemTypeVec )
{
	if( szName.IsEmpty() )
		return;

	for( int i = 0;i < m_iAwardCount;i++)
	{
		AwardSlotWnd *pAward = (AwardSlotWnd *)FindChildWnd( ID_AWARD_1 + i );
		if( pAward && pAward->GetCharName() == szName )
		{
			pAward->GetMedalItemType( rvMedalItemTypeVec );
			return;
		}
	}
}

void TotalResultAwardWnd::StartChatBubble( const char *szBubbleName, const char *szChat, bool bWholeChat )
{
	for( int i = 0;i < m_iAwardCount;i++)
	{
		AwardSlotWnd *pAward = dynamic_cast<AwardSlotWnd *>(FindChildWnd( ID_AWARD_1 + i ));
		if( pAward && pAward->GetCharName() == szBubbleName )
		{
			pAward->StartChatBubble( szBubbleName, szChat, bWholeChat );
			break;
		}
	}
}
//////////////////////////////////////////////////////////////////////////
TotalResultTreasureCardBtn::TotalResultTreasureCardBtn()
{
	m_pCardCover = NULL;
	m_pPresentCover = NULL;
	m_pPresentCoverOverAdd = NULL;
	m_pPresentCoverMe = NULL;

	m_pPresentIcon = NULL;
	m_pSubIcon = NULL;

	m_iPresentType = 0;
	m_iPresentValue1 = m_iPresentValue2 = 0;

	m_bScreenStart = false;
	m_bScreen = false;
	m_dwCurrentTime = 0;
}

TotalResultTreasureCardBtn::~TotalResultTreasureCardBtn()
{
	SAFEDELETE( m_pCardCover );
	SAFEDELETE( m_pPresentCover );
	SAFEDELETE( m_pPresentCoverOverAdd );
	SAFEDELETE( m_pPresentCoverMe );

	SAFEDELETE( m_pPresentIcon );
	SAFEDELETE( m_pSubIcon );
}

void TotalResultTreasureCardBtn::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "CardCover" )
	{
		SAFEDELETE( m_pCardCover );
		m_pCardCover = pImage;
	}
	else if( szType == "PresentCover" )
	{
		SAFEDELETE( m_pPresentCover );
		m_pPresentCover = pImage;
	}
	else if( szType == "PresentCoverOverAdd" )
	{
		SAFEDELETE( m_pPresentCoverOverAdd );
		m_pPresentCoverOverAdd = pImage;
	}
	else if( szType == "PresentCoverMe" )
	{
		SAFEDELETE( m_pPresentCoverMe );
		m_pPresentCoverMe = pImage;
	}
	else
	{
		ioButton::AddRenderImage( szType, pImage );
	}
}

void TotalResultTreasureCardBtn::SetName( const ioHashString &rkName )
{
	m_Name = rkName;
}

void TotalResultTreasureCardBtn::SetPresent( short iPresentType, int iPresentValue1, int iPresentValue2 )
{
	m_iPresentType	= iPresentType;
	m_iPresentValue1= iPresentValue1;
	m_iPresentValue2= iPresentValue2;

	SAFEDELETE( m_pPresentIcon );
	SAFEDELETE( m_pSubIcon );
	const ioHashString &rkPresentIcon = g_PresentMgr.GetPresentIconText( m_iPresentType, m_iPresentValue1, m_iPresentValue2 );
	m_pPresentIcon = g_UIImageSetMgr.CreateImageByFullName( rkPresentIcon );
	m_pSubIcon = g_UIImageSetMgr.CreateImageByFullName( g_PresentMgr.GetPresentSubIconText( m_iPresentType, m_iPresentValue1, m_iPresentValue2 ) );
}

void TotalResultTreasureCardBtn::SetScreenStart()
{
	if( m_Name.IsEmpty() )
		return;

	m_bScreenStart = true;
	m_dwCurrentTime= FRAMEGETTIME();
}

void TotalResultTreasureCardBtn::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	if( m_bScreenStart )
	{
		if( FRAMEGETTIME() - m_dwCurrentTime > 200 )
		{
			m_dwCurrentTime = FRAMEGETTIME();
			m_bScreen = !m_bScreen;
		}
	}
}

void TotalResultTreasureCardBtn::OnRender()
{
	ioButton::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_iPresentType == 0 )
	{
		if( m_pCardCover )
			m_pCardCover->Render( iXPos + 35, iYPos + 44 );
			
		if( m_bOver || m_bClicked )
		{
			if( m_pPresentCoverOverAdd )
				m_pPresentCoverOverAdd->Render( iXPos + 35, iYPos + 44, UI_RENDER_ADD );
		}
	}
	else
	{
		OnRenderPresent( iXPos, iYPos );
	}
}

void TotalResultTreasureCardBtn::OnRenderPresent( int iXPos, int iYPos )
{
	if( m_Name == g_MyInfo.GetPublicID() )
	{
		if( m_pPresentCoverMe )
			m_pPresentCoverMe->Render( iXPos + 35, iYPos + 44 );
	}
	else if( m_pPresentCover )
		m_pPresentCover->Render( iXPos + 35, iYPos + 44 );

	if( m_pPresentIcon )
	{
		m_pPresentIcon->SetScale( 0.68f );
		m_pPresentIcon->Render( iXPos + 35, iYPos + 29, UI_RENDER_NORMAL, TFO_BILINEAR );
	}
	if( m_pSubIcon )
	{
		m_pSubIcon->SetScale( 0.68f );
		m_pSubIcon->Render( iXPos + 35, iYPos + 29, UI_RENDER_NORMAL, TFO_BILINEAR );
	}
	char szPresentName1[MAX_PATH] = "";
	sprintf( szPresentName1, g_PresentMgr.GetPresentValue1Text( m_iPresentType, m_iPresentValue1, m_iPresentValue2 ).c_str() );
	
	char szPresentName2[MAX_PATH] = "";
	sprintf( szPresentName2, g_PresentMgr.GetPresentValue2Text( m_iPresentType, m_iPresentValue1, m_iPresentValue2 ).c_str() );

	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	
	char szWidthCut[MAX_PATH] = "";
	Help::StringCutFun( FONT_SIZE_11, 62, TS_NORMAL, szWidthCut, MAX_PATH, szPresentName1 );
	g_FontMgr.PrintText( iXPos + 35, iYPos + 60, FONT_SIZE_11, szWidthCut );

	Help::StringCutFun( FONT_SIZE_11, 62, TS_NORMAL, szWidthCut, MAX_PATH, szPresentName2 );
	g_FontMgr.PrintText( iXPos + 35, iYPos + 72, FONT_SIZE_11, szWidthCut );

	if( m_bScreen )
	{
		if( m_pPresentCoverOverAdd )
			m_pPresentCoverOverAdd->Render( iXPos + 35, iYPos + 44, UI_RENDER_ADD );
	}
}

TotalResultTreasureCardWnd::TotalResultTreasureCardWnd() : m_pPlayStage( NULL )
{
	m_pCardText1	= NULL;
	m_pCardText2	= NULL;
	m_pCardTextNum	= NULL;
	m_pNoneCardText1= NULL;
	m_pNoneCardText2= NULL;

	m_pNameBubble		 = NULL;	
	m_pNameBubbleTail	 = NULL;
	m_pNameBubbleBlue	 = NULL;
	m_pNameBubbleTailBlue= NULL;

	m_bPresentAlarmBlock = false;
	m_iStartTreasureCount	 = 0;	
	m_iCurrentTreasureCount	 = 0;

	m_dwState		= STATE_NONE;
	m_dwCurrentTime = 0;
	m_iCurrentStateXPos = 0;
	m_iCurrentStateYPos = 0;
	m_fCurrentStateRate = 0.0f;
	m_iCurrentStateSec  = 0;

	m_dwStartTime		= 0;
	m_dwStartAlphaTime	= 0;
	m_dwClickTime		= 0;
	m_dwAutoOpenTime	= 0;
	m_dwClearTime		= 0;
}

TotalResultTreasureCardWnd::~TotalResultTreasureCardWnd()
{
	SAFEDELETE( m_pCardText1 );
	SAFEDELETE( m_pCardText2 );
	SAFEDELETE( m_pCardTextNum );
	SAFEDELETE( m_pNoneCardText1 );
	SAFEDELETE( m_pNoneCardText2 );

	SAFEDELETE( m_pNameBubble );
	SAFEDELETE( m_pNameBubbleTail );
	SAFEDELETE( m_pNameBubbleBlue );
	SAFEDELETE( m_pNameBubbleTailBlue );

	if( !m_BackSound.IsEmpty() )
		g_SoundMgr.StopSound( m_BackSound, 0 );
}

void TotalResultTreasureCardWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "NameBubble" )
	{
		SAFEDELETE( m_pNameBubble );
		m_pNameBubble = pFrame;
	}
	else if( szType == "NameBubbleBlue" )
	{
		SAFEDELETE( m_pNameBubbleBlue );
		m_pNameBubbleBlue = pFrame;
	}
	else
	{
		ioWnd::AddRenderFrame( szType, pFrame );
	}
}

void TotalResultTreasureCardWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "CardText1" )
	{
		SAFEDELETE( m_pCardText1 );
		m_pCardText1 = pImage;
	}
	else if( szType == "CardText2" )
	{
		SAFEDELETE( m_pCardText2 );
		m_pCardText2 = pImage;
	}
	else if( szType == "CardTextNum" )
	{
		SAFEDELETE( m_pCardTextNum );
		m_pCardTextNum = pImage;
	}
	else if( szType == "NoneCardText1" )
	{
		SAFEDELETE( m_pNoneCardText1 );
		m_pNoneCardText1 = pImage;
	}
	else if( szType == "NoneCardText2" )
	{
		SAFEDELETE( m_pNoneCardText2 );
		m_pNoneCardText2 = pImage;
	}
	else if( szType == "NameBubbleTail" )
	{
		SAFEDELETE( m_pNameBubbleTail );
		m_pNameBubbleTail = pImage;
	}
	else if( szType == "NameBubbleTailBlue" )
	{
		SAFEDELETE( m_pNameBubbleTailBlue );
		m_pNameBubbleTailBlue = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void TotalResultTreasureCardWnd::iwm_show()
{
	m_bPresentAlarmBlock = true;

	if( !m_BackSound.IsEmpty() )
		g_SoundMgr.PlaySound( m_BackSound );

	g_GUIMgr.HideWnd( MINIMAP_WND );
	g_GUIMgr.HideWnd( TIME_SCORE_WND );
	g_GUIMgr.HideWnd( SKILL_LIST_WND );
	g_GUIMgr.HideWnd( ACCESSORY_SKILL_LIST_WND );
	g_GUIMgr.HideWnd( CENTER_INFO_WND );
	g_GUIMgr.HideWnd( MONSTER_BOSS_GAUGE_WND );
	g_GUIMgr.HideWnd( MODE_RANK_WND );
	g_GUIMgr.HideWnd( KO_CHAT_MSG_WND );
	g_GUIMgr.HideWnd( MINI_SOLDIER_SELECT_WND );
	g_GUIMgr.HideWnd( TRAINING_FUNC_MENU );

	SetAllCardActive( false );
	SetTitleText( "잠시 기다려 주세요" );
}

void TotalResultTreasureCardWnd::iwm_hide()
{
	if( !m_BackSound.IsEmpty() )
		g_SoundMgr.StopSound( m_BackSound, 0 );
}

void TotalResultTreasureCardWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	if( COMPARE( dwID, ID_CARD_1, ID_CARD_21 + 1 ) )
	{
		if( m_dwState == STATE_CLICK )
		{
			if( cmd == IOBN_BTNUP )
			{
				if( m_iCurrentTreasureCount > 0 )
				{
					SP2Packet kPacket( CTPK_TREASURE_CARD_COMMAND );
					kPacket << TREASURE_CARD_CMD_CLICK << dwID;
					TCPNetwork::SendToServer( kPacket );
					TCPNetwork::MouseBusy( true );
				}
			}
		}
	}
}

void TotalResultTreasureCardWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_BackSound = xElement.GetStringAttribute_e( "BackSound" );
	m_AutoOpenSound = xElement.GetStringAttribute_e( "AutoOpenSound" );

	m_dwStartTime		= xElement.GetIntAttribute_e( "StartTime" );
	m_dwStartAlphaTime	= xElement.GetIntAttribute_e( "StartAlphaTime" );
	m_dwClickTime		= xElement.GetIntAttribute_e( "ClickTime" );
	m_dwAutoOpenTime    = xElement.GetIntAttribute_e( "AutoOpenTime" );
	m_dwClearTime		= xElement.GetIntAttribute_e( "ClearTime" );

	m_dwClickRemainTime = m_dwClickTime;
}

bool TotalResultTreasureCardWnd::IsPresentAlarmBlock()
{
	if( !m_bPresentAlarmBlock ) 
		return false;
	
	// 보물카드를 뒤집지 않았으면 블럭
	if( m_dwState != STATE_CLEAR )
		return true;

	m_bPresentAlarmBlock = false;
	return m_bPresentAlarmBlock;
}

void TotalResultTreasureCardWnd::SetStartTreasure( int iTreasureCount )
{
	m_iStartTreasureCount = iTreasureCount;
	m_iCurrentTreasureCount = iTreasureCount;

	m_dwState        = STATE_START;
	m_dwCurrentTime  = FRAMEGETTIME();

	m_iCurrentStateXPos = 400;
	m_iCurrentStateYPos = 0;
	m_fCurrentStateRate = 0.0f;
}

void TotalResultTreasureCardWnd::SetPlayStage( ioPlayStage *pPlayStage )
{
	m_pPlayStage = pPlayStage;
}

void TotalResultTreasureCardWnd::AddFinalInfo( const FinalInfo &rkInfo )
{
	m_FinalInfoList.push_back( rkInfo );
}

void TotalResultTreasureCardWnd::SetAllCardScreenStart()
{
	for(int i = ID_CARD_1;i < ID_CARD_21 + 1;i++)
	{
		TotalResultTreasureCardBtn *pCardBtn = dynamic_cast< TotalResultTreasureCardBtn * >( FindChildWnd( i ) );
		if( !pCardBtn ) continue;

		pCardBtn->SetScreenStart();
	}
}

void TotalResultTreasureCardWnd::SetAllCardActive( bool bActive )
{
	for(int i = ID_CARD_1;i < ID_CARD_21 + 1;i++)
	{
		TotalResultTreasureCardBtn *pCardBtn = dynamic_cast< TotalResultTreasureCardBtn * >( FindChildWnd( i ) );
		if( !pCardBtn ) continue;

		if( bActive )
			pCardBtn->SetActive();
		else
			pCardBtn->SetInActive();
	}
}

void TotalResultTreasureCardWnd::TreasureCardTimeOut( SP2Packet &rkPacket )
{
	DWORD dwSeed;
	rkPacket >> dwSeed;
	g_TreasureCardMgr.TreasureCardRandSeed( dwSeed );

	int iOpenCardCount;
	rkPacket >> iOpenCardCount;
	for(int i = 0;i < iOpenCardCount;i++)
	{
		DWORD dwID;
		ioHashString rkName;
		short iPresentType;
		int iPresentValue1, iPresentValue2;
		rkPacket >> dwID >> rkName >> iPresentType >> iPresentValue1 >> iPresentValue2;

		TotalResultTreasureCardBtn *pCardBtn = dynamic_cast< TotalResultTreasureCardBtn * >( FindChildWnd( dwID ) );
		if( pCardBtn )
		{
			pCardBtn->SetPresent( iPresentType, iPresentValue1, iPresentValue2 );
			pCardBtn->SetName( rkName );
			pCardBtn->SetInActive();
		}
	}

	m_dwState       = STATE_AUTO_OPEN;
	m_dwCurrentTime = FRAMEGETTIME();

	int iEmptyCardCount = GetEmptyCardCount() + 3;
	m_dwAutoOpenTime = iEmptyCardCount * m_dwAutoOpenTime;
	if( m_dwAutoOpenTime == 0 )
	{
		m_dwState       = STATE_CLEAR;
		m_dwCurrentTime = FRAMEGETTIME();
	}
	else
	{
		m_dwCurrentTimeTick = FRAMEGETTIME();
		m_dwTickTime        = (float)m_dwAutoOpenTime / iEmptyCardCount;
	}

	if( !m_AutoOpenSound.IsEmpty() )
		g_SoundMgr.PlaySound( m_AutoOpenSound );
}

void TotalResultTreasureCardWnd::TreasureCardClick( SP2Packet &rkPacket )
{
	DWORD dwID;
	rkPacket >> dwID;

	if( dwID == 0 )
	{
		TCPNetwork::MouseBusy( false );
		return;
	}

	ioHashString rkName;
	short iPresentType;
	int iPresentValue1, iPresentValue2, iTreasureCount;
	rkPacket >> rkName >> iTreasureCount >> iPresentType >> iPresentValue1 >> iPresentValue2;

	TotalResultTreasureCardBtn *pCardBtn = dynamic_cast< TotalResultTreasureCardBtn * >( FindChildWnd( dwID ) );
	if( pCardBtn )
	{
		pCardBtn->SetPresent( iPresentType, iPresentValue1, iPresentValue2 );
		pCardBtn->SetName( rkName );
		pCardBtn->SetInActive();
	}

	if( rkName == g_MyInfo.GetPublicID() )
	{
		m_iCurrentTreasureCount = iTreasureCount;
		if( m_iCurrentTreasureCount <= 0 )
		{
			// 더 이상 클릭을 할 수없으면 모든 카드 클릭 금지
			SetAllCardActive( false );
		}

		TCPNetwork::MouseBusy( false );
	}

	int iSize = m_FinalInfoList.size();
	for(int i = 0;i < iSize;i++)
	{
		FinalInfo &rkInfo = m_FinalInfoList[i];
		if( rkInfo.m_szName == rkName )
		{
			rkInfo.m_iTreasureCardCnt = max( 0, rkInfo.m_iTreasureCardCnt - 1 );
			break;
		}
	}

	// 남은 카드 없으면 모두 뒤집기
	if( GetTotalRemainCardClickCount() == 0 )
	{
		if( m_dwState == STATE_CLICK )
		{
			m_dwClickRemainTime = m_dwClickTime - min( m_dwClickTime, FRAMEGETTIME() - m_dwCurrentTime );
		}

		SendTimeOut();
	}
}

int TotalResultTreasureCardWnd::GetEmptyCardCount()
{
	int iReturn = 0;
	for(int i = ID_CARD_1;i < ID_CARD_21 + 1;i++)
	{
		TotalResultTreasureCardBtn *pCardBtn = dynamic_cast< TotalResultTreasureCardBtn * >( FindChildWnd( i ) );
		if( !pCardBtn ) continue;
		if( !pCardBtn->IsEmptyCard() ) continue;

		iReturn++;
	}
	return iReturn;
}

int TotalResultTreasureCardWnd::GetTotalRemainCardClickCount()
{
	int iResult = 0;
	int iSize = m_FinalInfoList.size();
	for(int i = 0;i < iSize;i++)
	{
		FinalInfo &rkInfo = m_FinalInfoList[i];
		iResult += rkInfo.m_iTreasureCardCnt;
	}
	return iResult;
}

void TotalResultTreasureCardWnd::ProcessRandomCard( bool bOneCard )
{
	if( m_pPlayStage )
	{
		for(int i = ID_CARD_1;i < ID_CARD_21 + 1;i++)
		{
			TotalResultTreasureCardBtn *pCardBtn = dynamic_cast< TotalResultTreasureCardBtn * >( FindChildWnd( i ) );
			if( !pCardBtn ) continue;
			if( !pCardBtn->IsEmptyCard() ) continue;

			short iPresentType;
			int iPresentValue1, iPresentValue2;
			m_pPlayStage->TreasureCardRandom( iPresentType, iPresentValue1, iPresentValue2 );
			pCardBtn->SetPresent( iPresentType, iPresentValue1, iPresentValue2 );
			pCardBtn->SetInActive();
			if( bOneCard )
				break;
		}
	}
}

void TotalResultTreasureCardWnd::SendTimeOut()
{
	m_dwState       = STATE_DELAY;
	m_dwCurrentTime = FRAMEGETTIME();

	SetTitleText( "잠시 기다려 주세요" );
	SP2Packet kPacket( CTPK_TREASURE_CARD_COMMAND );
	kPacket << TREASURE_CARD_CMD_TIME_OUT;
	TCPNetwork::SendToServer( kPacket );
}

void TotalResultTreasureCardWnd::ProcessState()
{
	if( m_dwCurrentTime == 0 ) return;

	DWORD dwGapTime = FRAMEGETTIME() - m_dwCurrentTime;
	switch( m_dwState )
	{
	case STATE_START:
		if( dwGapTime > m_dwStartTime )
		{
			if( GetTotalRemainCardClickCount() == 0 )
			{
				SendTimeOut();   //즉시 종료
				m_dwClickRemainTime = m_dwClickTime;
			}
			else
			{
				SetAllCardActive( true );
				m_dwState          = STATE_CLICK;
				m_dwCurrentTime    = FRAMEGETTIME();

				m_iCurrentStateSec = (float)m_dwClickTime / 1000;

				char szTitle[MAX_PATH] = "";
				if( m_iCurrentTreasureCount == 0 )
				{
					sprintf( szTitle, "잠시 기다려 주세요 . . %d", m_iCurrentStateSec );
				}
				else
				{
					sprintf( szTitle, "보물 카드를 %d개 선택해 주세요 . . %d", m_iStartTreasureCount, m_iCurrentStateSec );
				}
				SetTitleText( szTitle );
			}
		}
		else
		{
			if( dwGapTime <= m_dwStartAlphaTime )
			{				
				float fTimeRate = (float)dwGapTime / m_dwStartAlphaTime;
				m_fCurrentStateRate = sinf( ( D3DX_PI / 2 ) * fTimeRate );
				m_iCurrentStateXPos = 400 - ( 400 * m_fCurrentStateRate );
			}
		}
		break;
	case STATE_CLICK:
		if( dwGapTime > m_dwClickTime )
		{
			SendTimeOut();
		}
		else
		{
			int iSec = ( (float)( m_dwClickTime - dwGapTime ) / 1000 ) + 1;
			if( m_iCurrentStateSec != iSec )
			{
				m_iCurrentStateSec = max( 0, iSec );
				char szTitle[MAX_PATH] = "";
				if( m_iCurrentTreasureCount == 0 )
				{
					sprintf( szTitle, "잠시 기다려 주세요 . . %d", m_iCurrentStateSec );
				}
				else
				{
					sprintf( szTitle, "보물 카드를 %d개 선택해 주세요 . . %d", m_iStartTreasureCount, m_iCurrentStateSec );
				}
				SetTitleText( szTitle );
			}
		}
		break;
	case STATE_DELAY:
		if( dwGapTime > 5000 )
		{
			m_dwState       = STATE_AUTO_OPEN;
			m_dwCurrentTime = FRAMEGETTIME();
			
			g_TreasureCardMgr.TreasureCardRandSeed( g_BattleRoomMgr.GetIndex() );
			int iEmptyCardCount = GetEmptyCardCount() + 3;  
			m_dwAutoOpenTime = iEmptyCardCount * m_dwAutoOpenTime;
			if( m_dwAutoOpenTime == 0 )
			{
				m_dwState       = STATE_CLEAR;
				m_dwCurrentTime = FRAMEGETTIME();
			}
			else
			{
				m_dwCurrentTimeTick = FRAMEGETTIME();
				m_dwTickTime        = (float)m_dwAutoOpenTime / iEmptyCardCount;
			}
		}
		break;
	case STATE_AUTO_OPEN:
		if( dwGapTime > m_dwAutoOpenTime + m_dwClickRemainTime )
		{
			m_dwState       = STATE_CLEAR;
			m_dwCurrentTime = FRAMEGETTIME();
			ProcessRandomCard( false );
			SetAllCardScreenStart();
		}
		else
		{
			if( FRAMEGETTIME() - m_dwCurrentTimeTick > m_dwTickTime )
			{
				m_dwCurrentTimeTick = FRAMEGETTIME();
				ProcessRandomCard( true );
			}
		}
		break;
	case STATE_CLEAR:
		if( dwGapTime > m_dwClearTime )
		{
			m_dwCurrentTime = 0;
			if( GetParent() )
				GetParent()->iwm_command( this, STATE_CLEAR, 0 );
		}
		break;
	}
}

void TotalResultTreasureCardWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	ProcessState();
}

void TotalResultTreasureCardWnd::OnRenderAfterChild()
{
	ioWnd::OnRenderAfterChild();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	OnRenderStartMent( iXPos, iYPos );

	for(int i = ID_CARD_1;i < ID_CARD_21 + 1;i++)
	{
		TotalResultTreasureCardBtn *pCardBtn = dynamic_cast< TotalResultTreasureCardBtn * >( FindChildWnd( i ) );
		if( !pCardBtn ) continue;
		if( pCardBtn->GetName().IsEmpty() ) continue;
		
		OnRenderBubbleName( pCardBtn->GetName(), pCardBtn->GetDerivedPosX() - 4, pCardBtn->GetDerivedPosY() - 4 );
	}
}

void TotalResultTreasureCardWnd::OnRenderStartMent( int iXPos, int iYPos )
{
	if( m_dwState != STATE_START ) return;

	iXPos += m_iCurrentStateXPos;

	if( m_iStartTreasureCount == 0 )
	{
		if( !m_pNoneCardText1 || !m_pNoneCardText2 ) return;

		m_pNoneCardText1->SetAlpha( MAX_ALPHA_RATE * m_fCurrentStateRate );
		m_pNoneCardText1->Render( iXPos + 85, iYPos + 141 );

		m_pNoneCardText2->SetAlpha( MAX_ALPHA_RATE * m_fCurrentStateRate );
		m_pNoneCardText2->Render( iXPos + 85, iYPos + 201 );
	}
	else
	{
		if( !m_pCardText1 || !m_pCardText2 || !m_pCardTextNum ) return;

		m_pCardText1->SetAlpha( MAX_ALPHA_RATE * m_fCurrentStateRate );
		m_pCardText1->Render( iXPos + 121, iYPos + 141 );

		m_pCardTextNum->SetAlpha( MAX_ALPHA_RATE * m_fCurrentStateRate );
		m_pCardTextNum->RenderNum( iXPos + 93, iYPos + 201, m_iStartTreasureCount );

		m_pCardText2->SetAlpha( MAX_ALPHA_RATE * m_fCurrentStateRate );
		m_pCardText2->Render( iXPos + 132, iYPos + 201 );
	}
}

void TotalResultTreasureCardWnd::OnRenderBubbleName( const ioHashString &rkName, int iXPos, int iYPos )
{	
	if( rkName == g_MyInfo.GetPublicID() )
	{
		if( m_pNameBubbleBlue && m_pNameBubbleTailBlue )
		{
			m_pNameBubbleBlue->Render( iXPos, iYPos );
			m_pNameBubbleTailBlue->Render( iXPos + 34, iYPos + 20 );
		}

		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
		g_FontMgr.PrintText( iXPos + 39, iYPos + 4, FONT_SIZE_12, rkName.c_str() );
	}
	else
	{
		if( m_pNameBubble && m_pNameBubbleTail )
		{
			m_pNameBubble->Render( iXPos, iYPos );
			m_pNameBubbleTail->Render( iXPos + 34, iYPos + 20 );
		}

		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY);
		g_FontMgr.PrintText( iXPos + 39, iYPos + 4, FONT_SIZE_12, rkName.c_str() );
	}
}
//////////////////////////////////////////////////////////////////////////
TotalResultTournamentWnd::TotalResultTournamentWnd()
{
	m_pUI3DRender = NULL;
	m_aMyChar	  = NULL;

	m_pResultTextBack	  = NULL;
	m_pBack				  = NULL;
	m_pProgressGaugeBack  = NULL;
	m_pProgressGauge	  = NULL;
	m_pProgressGaugeArrow = NULL;

	m_pChampIcon		 = NULL;
	m_pChampIconBack	 = NULL;

	m_pPesoIcon		= NULL;
	m_pPesoIconBack = NULL;
	m_pPesoText		= NULL;
	m_pPesoNum		= NULL;

	m_pBlueRoundNum			= NULL;
	m_pBlueRoundText		= NULL;
	m_pBlueFinalRoundText	= NULL;
	m_pBlueNextGoText		= NULL;
	m_pBlueNextFailText		= NULL;
	m_pBlueChampionText		= NULL;
	m_pBlueHalfChampionText	= NULL;

	m_pRedRoundNum			= NULL;
	m_pRedRoundText			= NULL;
	m_pRedFinalRoundText	= NULL;
	m_pRedNextGoText		= NULL;
	m_pRedNextFailText		= NULL;
	m_pRedChampionText		= NULL;
	m_pRedHalfChampionText	= NULL;

	m_iTournamentCurRound = 0;
	m_szTournamentTeamName.Clear();

	m_iTournamentPageMax		= 0;
	m_iTournamentCurPage		= 0;
	m_iTournamentCurPos			= 0;
	m_iTournamentStartTeamCount = 0;

	m_iTournamentGaugeWidth = 0;
	
	m_dwRolliningStartTime = 0;

	m_eWinner = WTT_NONE;
	m_eMyTeam = TEAM_NONE;

	m_iPesoCurValue = 0;
	m_iPesoOrgValue = 0;

	m_fTournamentTeamAniRate			= 0.0f;
	m_fTournamentResultTextAniRate		= 0.0f;
	m_fTournamentResultTextBackAniRate	= 0.0f;
	m_fTournamentResultTextBlinkAniRate	= 0.0f;	
	m_fTournamentPesoBlinkAniRate		= 0.0f;
	m_fTournamentPesoRollingAniRate		= 0.0f;
	m_fTournamentPesoScalingAniRate		= 0.0f;
	m_fTournamentGaugeStrechAniRate		= 0.0f;
	m_fTournamentGaugeAlphaAniRate		= 0.0f;

	m_bResultTextSound = true;
	m_bPesoRollingSound = true;
	m_bGaugeStrechSound = true;
}

TotalResultTournamentWnd::~TotalResultTournamentWnd()
{
	SAFEDELETE( m_aMyChar );
	SAFEDELETE( m_pUI3DRender );

	SAFEDELETE( m_pResultTextBack );
	SAFEDELETE( m_pBack );
	SAFEDELETE( m_pProgressGaugeBack );
	SAFEDELETE( m_pProgressGauge );
	SAFEDELETE( m_pProgressGaugeArrow );

	SAFEDELETE( m_pChampIcon );
	SAFEDELETE( m_pChampIconBack );

	SAFEDELETE( m_pPesoIcon );
	SAFEDELETE( m_pPesoIconBack );
	SAFEDELETE( m_pPesoText );
	SAFEDELETE( m_pPesoNum );

	SAFEDELETE( m_pBlueRoundNum );
	SAFEDELETE( m_pBlueRoundText );
	SAFEDELETE( m_pBlueFinalRoundText );
	SAFEDELETE( m_pBlueNextGoText );
	SAFEDELETE( m_pBlueNextFailText );
	SAFEDELETE( m_pBlueChampionText );
	SAFEDELETE( m_pBlueHalfChampionText );

	SAFEDELETE( m_pRedRoundNum );
	SAFEDELETE( m_pRedRoundText );
	SAFEDELETE( m_pRedFinalRoundText );
	SAFEDELETE( m_pRedNextGoText );
	SAFEDELETE( m_pRedNextFailText );
	SAFEDELETE( m_pRedChampionText );
	SAFEDELETE( m_pRedHalfChampionText );

	if( !m_szWinBackSound.IsEmpty() )
		g_SoundMgr.StopSound( m_szWinBackSound, 0 );

	if( !m_szLoseBackSound.IsEmpty() )
		g_SoundMgr.StopSound( m_szLoseBackSound, 0 );

	if( !m_szResultTextSound.IsEmpty() )
		g_SoundMgr.StopSound( m_szResultTextSound, 0 );

	if( !m_szPesoRollingSound.IsEmpty() )
		g_SoundMgr.StopSound( m_szPesoRollingSound, 0 );

	if( !m_szGaugeStrechSound.IsEmpty() )
		g_SoundMgr.StopSound( m_szGaugeStrechSound, 0 );
}


void TotalResultTournamentWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "ResultTextBack" )
	{
		SAFEDELETE( m_pResultTextBack );
		m_pResultTextBack = pFrame;
	}
	else
	{
		ioWnd::AddRenderFrame( szType, pFrame );
	}
}

void TotalResultTournamentWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "Back" )
	{
		SAFEDELETE( m_pBack );
		m_pBack = pImage;
	}
	else if( szType == "ProgressGaugeBack" )
	{
		SAFEDELETE( m_pProgressGaugeBack );
		m_pProgressGaugeBack = pImage;
	}
	else if( szType == "ProgressGauge" )
	{
		SAFEDELETE( m_pProgressGauge );
		m_pProgressGauge = pImage;
	}
	else if( szType == "ProgressGaugeArrow" )
	{
		SAFEDELETE( m_pProgressGaugeArrow );
		m_pProgressGaugeArrow = pImage;
	}
	else if( szType == "ChampIcon" )
	{
		SAFEDELETE( m_pChampIcon );
		m_pChampIcon = pImage;
	}
	else if( szType == "ChampIconBack" )
	{
		SAFEDELETE( m_pChampIconBack );
		m_pChampIconBack = pImage;
	}
	else if( szType == "PesoIcon" )
	{
		SAFEDELETE( m_pPesoIcon );
		m_pPesoIcon = pImage;
	}
	else if( szType == "PesoIconBack" )
	{
		SAFEDELETE( m_pPesoIconBack );
		m_pPesoIconBack = pImage;
	}
	else if( szType == "PesoText" )
	{
		SAFEDELETE( m_pPesoText );
		m_pPesoText = pImage;
	}
	else if( szType == "PesoNum" )
	{
		SAFEDELETE( m_pPesoNum );
		m_pPesoNum = pImage;
	}
	else if( szType == "BlueRoundNum" )
	{
		SAFEDELETE( m_pBlueRoundNum );
		m_pBlueRoundNum = pImage;
	}
	else if( szType == "BlueRoundText" )
	{
		SAFEDELETE( m_pBlueRoundText );
		m_pBlueRoundText = pImage;
	}
	else if( szType == "BlueFinalRoundText" )
	{
		SAFEDELETE( m_pBlueFinalRoundText );
		m_pBlueFinalRoundText = pImage;
	}
	else if( szType == "BlueNextGoText" )
	{
		SAFEDELETE( m_pBlueNextGoText );
		m_pBlueNextGoText = pImage;
	}
	else if( szType == "BlueNextFailText" )
	{
		SAFEDELETE( m_pBlueNextFailText );
		m_pBlueNextFailText = pImage;
	}
	else if( szType == "BlueChampionText" )
	{
		SAFEDELETE( m_pBlueChampionText );
		m_pBlueChampionText = pImage;
	}
	else if( szType == "BlueHalfChampionText" )
	{
		SAFEDELETE( m_pBlueHalfChampionText );
		m_pBlueHalfChampionText = pImage;
	}
	else if( szType == "RedRoundNum" )
	{
		SAFEDELETE( m_pRedRoundNum );
		m_pRedRoundNum = pImage;
	}
	else if( szType == "RedRoundText" )
	{
		SAFEDELETE( m_pRedRoundText );
		m_pRedRoundText = pImage;
	}
	else if( szType == "RedFinalRoundText" )
	{
		SAFEDELETE( m_pRedFinalRoundText );
		m_pRedFinalRoundText = pImage;
	}
	else if( szType == "RedNextGoText" )
	{
		SAFEDELETE( m_pRedNextGoText );
		m_pRedNextGoText = pImage;
	}
	else if( szType == "RedNextFailText" )
	{
		SAFEDELETE( m_pRedNextFailText );
		m_pRedNextFailText = pImage;
	}
	else if( szType == "RedChampionText" )
	{
		SAFEDELETE( m_pRedChampionText );
		m_pRedChampionText = pImage;
	}
	else if( szType == "RedHalfChampionText" )
	{
		SAFEDELETE( m_pRedHalfChampionText );
		m_pRedHalfChampionText = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void TotalResultTournamentWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_szWinBackSound = xElement.GetStringAttribute_e( "WinBackSound" );
	m_szLoseBackSound = xElement.GetStringAttribute_e( "LoseBackSound" );
	m_szResultTextSound = xElement.GetStringAttribute_e( "ResultTextEffectSound" );
	m_szPesoRollingSound = xElement.GetStringAttribute_e( "PesoRollingSound" );
	m_szGaugeStrechSound = xElement.GetStringAttribute_e( "GaugeStrechEffectSound" );

	m_dwTeamNameStartTime = xElement.GetIntAttribute_e( "TeamNameStartTime" );
	m_dwTeamNameGapTime =  xElement.GetIntAttribute_e( "TeamNameGapTime" );
	m_dwTeamNameXPos = xElement.GetIntAttribute_e( "TeamNameXPos" );

	m_dwResultTextStartTime = xElement.GetIntAttribute_e( "ResultTextStartTime" );
	m_dwResultTextGapTime = xElement.GetIntAttribute_e( "ResultTextGapTime" );

	m_dwResultTextBackStartTime = xElement.GetIntAttribute_e( "ResultTextBackStartTime" );
	m_dwResultTextBackGapTime = xElement.GetIntAttribute_e( "ResultTextBackGapTime" );

	m_dwResultTextBlinkStartTime = xElement.GetIntAttribute_e( "ResultTextBlinkStartTime" );
	m_dwResultTextBlinkGapTime = xElement.GetIntAttribute_e( "ResultTextBlinkGapTime" );

	m_dwPesoRollingStartTime = xElement.GetIntAttribute_e( "PesoRollingStartTime" );
	m_dwPesoRollingGapTime = xElement.GetIntAttribute_e( "PesoRollingGapTime" );
	m_dwPesoRollingTime = xElement.GetIntAttribute_e( "PesoRollingTime" );

	m_dwPesoBlinkStartTime = xElement.GetIntAttribute_e( "PesoBlinkStartTime" );
	m_dwPesoBlinkGapTime = xElement.GetIntAttribute_e( "PesoBlinkGapTime" );
	m_dwPesoBlinkScale = xElement.GetFloatAttribute_e( "PesoScale" );

	m_dwGagueStrechStartTime = xElement.GetIntAttribute_e( "GagueStrechStartTime" );
	m_dwGagueStrechGapTime = xElement.GetIntAttribute_e( "GagueStrechGapTime" );

	m_dwRoundTextStartTime = xElement.GetIntAttribute_e( "RoundTextStartTime" );
	m_dwRoundTextGapTime = xElement.GetIntAttribute_e( "RoundTextGapTime" );
}

void TotalResultTournamentWnd::SetPlayStage( ioPlayStage *pPlayStage )
{
	m_pPlayStage = pPlayStage;
}

void TotalResultTournamentWnd::SetTournamentData()
{
	TournamentNode* pTournament = g_TournamentMgr.GetTournament( g_BattleRoomMgr.GetTournamentIndex() );
	ioUserTournament *pUserTournament = g_MyInfo.GetTournament();
	if( pTournament && pUserTournament )
	{
		bool bObserver = g_BattleRoomMgr.IsObserverUser( g_MyInfo.GetPublicID() );
		DWORD dwTeamIndex = pUserTournament->GetTeamIndex( g_BattleRoomMgr.GetTournamentIndex() );
		if( bObserver || dwTeamIndex == 0 )
		{
			if( m_eWinner == WTT_BLUE_TEAM )
			{
				dwTeamIndex = g_BattleRoomMgr.GetTournamentBlueIndex();
				m_eMyTeam = TEAM_BLUE; //관전자는 승리팀을 따라간다.
			}
			else
			{
				dwTeamIndex = g_BattleRoomMgr.GetTournamentRedIndex();
				m_eMyTeam = TEAM_RED; //관전자는 승리팀을 따라간다.
			}
		}

		m_iTournamentMaxRound  = pTournament->GetTournamentMaxRound();

		TeamGlobalData* pTeam = g_TournamentTeamMgr.GetTeamData( dwTeamIndex, false );
		if( pTeam )
		{
			m_szTournamentTeamName      = pTeam->GetTeamName();
			m_iTournamentCurPos			= pTeam->GetTourPos(); // 여기에 -1을 해야 현재 라운드
			m_iTournamentStartTeamCount = pTournament->GetTournamentStartTeamCount();
			m_iTournamentCurRound  = Help::TournamentCurrentRoundWithTeam( m_iTournamentStartTeamCount, m_iTournamentCurPos );
		}
		else
		{
			//데이터가 없을 경우(관전방 선택시 동기화 됨으로 여기로 들어오지 않음, 들어오면 버그)
			if( m_eWinner == WTT_BLUE_TEAM )
				m_eMyTeam = TEAM_BLUE;
			else
				m_eMyTeam = TEAM_RED;

			m_szTournamentTeamName      = "관전자";
			m_iTournamentCurPos         = pTournament->GetState() - TournamentNode::STATE_TOURNAMENT;
			m_iTournamentStartTeamCount = pTournament->GetTournamentStartTeamCount();
			m_iTournamentCurRound       = Help::TournamentCurrentRoundWithTeam( m_iTournamentStartTeamCount, m_iTournamentCurPos );
		}

		m_szWinMotion = pTournament->GetResultWinMotion();
		m_szLoseMotion = pTournament->GetResultLoseMotion();
	}

	m_iPesoCurValue = m_iPesoOrgValue;
	m_iPesoSize = GetNumberCount( m_iPesoOrgValue );
	
	SetGaugeValue();

	DWORD dwCharIndex = g_MyInfo.GetCharIndex( g_MyInfo.GetSelectCharArray() );
	m_CharInfo = g_GlobalSoldierInfo.GetSoldierData( g_MyInfo.GetPublicID(), dwCharIndex );
	if( m_CharInfo.m_class_type == 0 )
		return;

	ioCreateChar *pChar = m_pUI3DRender->CreateChar( m_CharInfo, "ResultChar" );
	if( !pChar ) return;

	pChar->SetPosition( -50.0f, -120.0f, 860.0f );
	pChar->SetYaw( FLOAT10 );
	m_pUI3DRender->SetLookAt( 0.0f, 0.0f, 860.0f );

	float fAspect = (float)VIEW_WIDTH/(float)VIEW_HEIGHT;
	m_pUI3DRender->SetProjection( FLOAT1, 600.0f, 45.0f, fAspect );

	m_aMyChar = pChar;

	// 코스튬 관련 (??)
	const ioSetItemInfo *pSetInfo = g_SetItemInfoMgr.GetSetInfoByIdx( m_CharInfo.m_class_type - 1 );
	if( pSetInfo )
	{
		ConstItemVector rkItemList = pSetInfo->GetSetItemList();
		int iMaxItemCnt = min( rkItemList.size(), MAX_INVENTORY );
		for(int i = 0;i < iMaxItemCnt;i++)
		{
			DWORD dwItemCode = g_GlobalSoldierInfo.GetSoldierItemCode( g_MyInfo.GetPublicID(), dwCharIndex, i );
			DWORD dwCostumeMaleCustom = m_CharInfo.m_EquipCostume[i].m_Costume_Male_Custom;
			DWORD dwCostumeFemaleCustom = m_CharInfo.m_EquipCostume[i].m_Costume_Female_Custom;

			if( dwItemCode == 0 )
			{
				if ( m_CharInfo.m_EquipCostume[i].m_CostumeCode <= 0 )
				{
					dwCostumeMaleCustom = 0;
					dwCostumeFemaleCustom = 0;
				}
				pChar->EquipItem( rkItemList[i]->GetItemCode(), dwCostumeMaleCustom, dwCostumeFemaleCustom, m_CharInfo.m_EquipCostume[i].m_CostumeCode );
			}
			else
			{
				if ( m_CharInfo.m_EquipCostume[i].m_CostumeCode <= 0 )
				{
					dwCostumeMaleCustom = g_GlobalSoldierInfo.GetSoldierItemMaleCustom( g_MyInfo.GetPublicID(), dwCharIndex, i );
					dwCostumeFemaleCustom = g_GlobalSoldierInfo.GetSoldierItemFemaleCustom( g_MyInfo.GetPublicID(), dwCharIndex, i );
				}
				pChar->EquipItem( dwItemCode, dwCostumeMaleCustom, dwCostumeFemaleCustom, m_CharInfo.m_EquipCostume[i].m_CostumeCode );
			}
		}
	}
		
	if( m_eMyTeam == TEAM_BLUE )
	{
		if( m_eWinner == WTT_BLUE_TEAM )
		{
			if( !m_szWinMotion.IsEmpty() )
			{
				m_aMyChar->SetLoopAni( m_szWinMotion );
			}
			else
			{
				m_aMyChar->SetWeaponShopMotion();
			}
		}
		else
		{
			if(!m_szLoseMotion.IsEmpty() )
			{
				m_aMyChar->SetLoopAni( m_szLoseMotion );
			}
			else
			{
				m_aMyChar->SetWeaponShopMotion();
			}
		}
	}
	else
	{
		if( m_eWinner == WTT_RED_TEAM )
		{
			if( !m_szWinMotion.IsEmpty() )
			{
				m_aMyChar->SetLoopAni( m_szWinMotion );
			}
			else
			{
				m_aMyChar->SetWeaponShopMotion();
			}
		}
		else
		{
			if( !m_szLoseMotion.IsEmpty() )
			{
				m_aMyChar->SetLoopAni( m_szLoseMotion );
			}
			else
			{
				m_aMyChar->SetWeaponShopMotion();
			}
		}
	}
}

void TotalResultTournamentWnd::SetGaugeValue()
{
	//페이지 수 구하기
	m_iTournamentPageMax =  max( 0, m_iTournamentMaxRound - TOURNAMENT_VIEW_MAX );

	m_iTournamentPageMaxRound = TOURNAMENT_VIEW_MAX;		//최대 8라운드 표시
	if( m_iTournamentMaxRound < m_iTournamentPageMaxRound ) //대회가 8라운드 이하의 대회 일 경우 라운드 수만큼 그리기
		m_iTournamentPageMaxRound = m_iTournamentMaxRound;
	
	//현재 페이지의 라운드가 4라운드 이상 진행됬으면 페이지를 넘겨준다.
	if( GAGUE_MIDDILE < m_iTournamentCurPos && TOURNAMENT_VIEW_MAX < m_iTournamentMaxRound )
	{
		m_iTournamentCurPage = m_iTournamentCurPos - GAGUE_MIDDILE;
		m_iTournamentCurPage = min( m_iTournamentCurPage, m_iTournamentPageMax );

		//하단 게이지 넓이
		m_iTournamentGaugeWidth = GAUGE_WIDTH * ( FLOAT1 / max( 1, m_iTournamentPageMaxRound ) ) * ( m_iTournamentCurPos - m_iTournamentCurPage );
	}
	else
	{
		//하단 게이지 넓이
		m_iTournamentGaugeWidth = GAUGE_WIDTH * ( FLOAT1 / max( 1, m_iTournamentPageMaxRound ) ) * m_iTournamentCurPos;
	}
}

void TotalResultTournamentWnd::SetTeamScore( TeamType eMyTeam, float fBlueWin, float fRedWin )
{
	if( !m_pPlayStage ) return;
	
	m_eMyTeam = eMyTeam;
	m_eWinner = WTT_DRAW;

	if( fBlueWin > fRedWin )
		m_eWinner = WTT_BLUE_TEAM;
	else if( fBlueWin < fRedWin )
		m_eWinner = WTT_RED_TEAM;
	else 
		m_eWinner = WTT_BLUE_TEAM;
}

void TotalResultTournamentWnd::SetPesoValue( int iPeso )
{
	m_iPesoOrgValue = iPeso;
}

void TotalResultTournamentWnd::iwm_create()
{
	SAFEDELETE( m_pUI3DRender );

	m_pUI3DRender = new ioUI3DRender;
	m_pUI3DRender->Initialize( "config/sp2_stage_charselect_map.ini", "ladder_cam" );
}

void TotalResultTournamentWnd::iwm_show()
{
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

	m_dwStartTime = FRAMEGETTIME();

	m_bResultTextSound = true;
	m_bPesoRollingSound = true;
	m_bGaugeStrechSound = true;

	if( m_eMyTeam == TEAM_BLUE )
	{
		if( m_eWinner == WTT_BLUE_TEAM )
		{
			if( !m_szWinBackSound.IsEmpty() )
				g_SoundMgr.PlaySound( m_szWinBackSound );
		}
		else
		{
			if( !m_szLoseBackSound.IsEmpty() )
				g_SoundMgr.PlaySound( m_szLoseBackSound );
		}
	}
	else
	{
		if( m_eWinner == WTT_RED_TEAM )
		{
			if( !m_szWinBackSound.IsEmpty() )
				g_SoundMgr.PlaySound( m_szWinBackSound );
		}
		else
		{
			if( !m_szLoseBackSound.IsEmpty() )
				g_SoundMgr.PlaySound( m_szLoseBackSound );
		}
	}
}

void TotalResultTournamentWnd::OnTeamNameMoveProcess( float dwCurTime )
{
	//팀명
	DWORD dwGapTime = dwCurTime - m_dwStartTime;
	if( dwGapTime > m_dwTeamNameStartTime )
	{
		dwGapTime -= m_dwTeamNameStartTime;
		if( dwGapTime < m_dwTeamNameGapTime )
		{
			m_fTournamentTeamAniRate = (float)dwGapTime / m_dwTeamNameGapTime;
		}
		else
		{
			m_fTournamentTeamAniRate = FLOAT1;
		}
	}
	else
	{
		m_fTournamentTeamAniRate = 0.0f;
	}
}

void TotalResultTournamentWnd::OnResultTextMoveProcess( float dwCurTime )
{
	//토너먼트진출결과
	DWORD dwGapTime = dwCurTime - m_dwStartTime;
	if( dwGapTime > m_dwResultTextStartTime )
	{
		dwGapTime -= m_dwResultTextStartTime;
		if( dwGapTime < m_dwResultTextGapTime )
		{
			m_fTournamentResultTextAniRate = (float)dwGapTime / m_dwResultTextGapTime;
		}
		else
		{
			m_fTournamentResultTextAniRate = FLOAT1;
		}
	}
	else
	{
		m_fTournamentResultTextAniRate = 0.0f;
	}
}

void TotalResultTournamentWnd::OnResultTextBackProcess( float dwCurTime )
{
	//팀명배경
	DWORD dwGapTime = FRAMEGETTIME() - m_dwStartTime;
	if( dwGapTime > m_dwResultTextBackStartTime )
	{
		dwGapTime -= m_dwResultTextBackStartTime;
		if( dwGapTime < m_dwResultTextBackGapTime )
		{
			m_fTournamentResultTextBackAniRate = (float)dwGapTime / m_dwResultTextBackGapTime;
		}
		else
		{
			m_fTournamentResultTextBackAniRate = FLOAT1;
		}
	}
	else
	{
		m_fTournamentResultTextBackAniRate = 0.0f;
	}
}

void TotalResultTournamentWnd::OnResultTextBlinkProcess( float dwCurTime )
{
	DWORD dwGapTime = dwCurTime - m_dwStartTime;
	if( dwGapTime > m_dwResultTextBlinkStartTime )
	{
		if( m_bResultTextSound && !m_szResultTextSound.IsEmpty() )
		{
			g_SoundMgr.PlaySound( m_szResultTextSound );
			m_bResultTextSound = false;
		}

		dwGapTime -= m_dwResultTextBlinkStartTime;
		if( dwGapTime < m_dwResultTextBlinkGapTime )
		{
			m_fTournamentResultTextBlinkAniRate = (float)dwGapTime / m_dwResultTextBlinkGapTime;
			m_fTournamentPesoScalingAniRate = (float)dwGapTime / m_dwResultTextBlinkGapTime;
		}
		else
		{
			m_fTournamentResultTextBlinkAniRate = FLOAT1;
			m_fTournamentPesoScalingAniRate = FLOAT1;
		}
	}
	else
	{
		m_fTournamentResultTextBlinkAniRate = 0.0f;
		m_fTournamentPesoScalingAniRate = 0.0f;
	}
}

void TotalResultTournamentWnd::OnPesoRollingProcess( float dwCurTime )
{
	DWORD dwGapTime = dwCurTime - m_dwStartTime;
	if( dwGapTime > m_dwPesoRollingStartTime )
	{
		dwGapTime -= m_dwPesoRollingStartTime;
		if( dwGapTime < m_dwPesoRollingGapTime )
		{
			m_fTournamentPesoRollingAniRate = (float)dwGapTime / m_dwPesoRollingGapTime;
			if( dwCurTime - m_dwRolliningStartTime > m_dwPesoRollingTime )
			{
				int iSize = (int)pow(10, (double)(m_iPesoSize-1));
				m_iPesoCurValue = rand() % (iSize * 9) + iSize;
				m_dwRolliningStartTime = dwCurTime;
			}
		}
		else
		{
			m_iPesoCurValue = m_iPesoOrgValue;
			m_fTournamentPesoRollingAniRate = FLOAT1;
		}
	}
	else
	{
		m_iPesoCurValue = m_iPesoOrgValue;
		m_fTournamentPesoRollingAniRate = 0.0f;
	}
}

void TotalResultTournamentWnd::OnPesoBlinkProcess( float dwCurTime )
{
	DWORD dwGapTime = dwCurTime - m_dwStartTime;
	if( dwGapTime > m_dwPesoBlinkStartTime )
	{
		if( m_bPesoRollingSound && !m_szPesoRollingSound.IsEmpty() )
		{
			g_SoundMgr.PlaySound( m_szPesoRollingSound );
			m_bPesoRollingSound = false;
		}

		dwGapTime -= m_dwPesoBlinkStartTime;
		if( dwGapTime < m_dwPesoBlinkGapTime )
		{
			m_fTournamentPesoBlinkAniRate = (float)dwGapTime / m_dwPesoBlinkGapTime;
		}
		else
		{
			m_fTournamentPesoBlinkAniRate = FLOAT1;
		}
	}
	else
	{
		m_fTournamentPesoBlinkAniRate = 0.0f;
	}
}

void TotalResultTournamentWnd::OnGaugeStrechProcess( float dwCurTime )
{
	DWORD dwGapTime = dwCurTime - m_dwStartTime;
	if( dwGapTime > m_dwGagueStrechStartTime )
	{
		dwGapTime -= m_dwGagueStrechStartTime;
		if( dwGapTime < m_dwGagueStrechGapTime )
		{
			if( m_bGaugeStrechSound && !m_szGaugeStrechSound.IsEmpty() )
			{
				if( GetParent() )
					GetParent()->iwm_command( this, TORUNAMENT_GUAGE_COMPLETE, 0 );

				g_SoundMgr.PlaySound( m_szGaugeStrechSound );
				m_bGaugeStrechSound = false;
			}

			m_fTournamentGaugeStrechAniRate = (float)dwGapTime / m_dwGagueStrechGapTime;
		}
		else
		{
			m_fTournamentGaugeStrechAniRate = FLOAT1;	
		}
	}
	else
	{
		m_fTournamentGaugeStrechAniRate = 0.0f;
	}
}

void TotalResultTournamentWnd::OnGaugeTextAlphaProcess( float dwCurTime )
{
	DWORD dwGapTime = dwCurTime - m_dwStartTime;
	if( dwGapTime > m_dwRoundTextStartTime )
	{
		dwGapTime -= m_dwRoundTextStartTime;
		if( dwGapTime < m_dwRoundTextGapTime )
		{
			m_fTournamentGaugeAlphaAniRate = (float)dwGapTime / m_dwRoundTextGapTime;
		}
		else
		{
			m_fTournamentGaugeAlphaAniRate = FLOAT1;
		}
	}
	else
	{
		m_fTournamentGaugeAlphaAniRate = 0.0f;
	}
}

void TotalResultTournamentWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	if( m_aMyChar )
		m_aMyChar->Update( fTimePerSec );
	
	DWORD dwCurTime = FRAMEGETTIME();

	OnTeamNameMoveProcess( dwCurTime );
	OnResultTextMoveProcess( dwCurTime );
	OnResultTextBackProcess( dwCurTime );
	OnResultTextBlinkProcess( dwCurTime );
	OnPesoRollingProcess( dwCurTime );
	OnPesoBlinkProcess( dwCurTime );
	OnGaugeStrechProcess( dwCurTime );
	OnGaugeTextAlphaProcess( dwCurTime );
} 

void TotalResultTournamentWnd::OnRender()
{
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();
	
	ioWnd::OnRender();

	if( m_pBack )
		m_pBack->Render( iXPos + 12.0f, iYPos + 39.0f );

	OnRenderSoldier( iXPos, iYPos );
	OnTeamNameRender( iXPos, iYPos );
	OnTournamentTextRender( iXPos, iYPos );
	OnTournamentPesoRender( iXPos, iYPos );
	OnProgressGaugeRender( iXPos, iYPos );

	if( m_pChampIconBack )
	{
		m_pChampIconBack->SetScale( 0.66f, 0.66f );
		m_pChampIconBack->Render( iXPos + 493.0f, iYPos + 318.0f, UI_RENDER_MULTIPLY, TFO_BILINEAR );
	}

	if( m_pChampIcon )
	{
		m_pChampIcon->SetScale( 0.66f, 0.66f );
		m_pChampIcon->Render( iXPos + 493, iYPos + 318, UI_RENDER_NORMAL, TFO_BILINEAR );
	}
}

void TotalResultTournamentWnd::OnTeamNameRender( int iXPos, int iYPos )
{
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_RIGHT );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	
	int iCurXPos = (1.0f - sin( D3DX_PI/2 * m_fTournamentTeamAniRate)) * m_dwTeamNameXPos;
	BYTE Alpha = sin( D3DX_PI/2 * m_fTournamentTeamAniRate) * 255;
	
	g_FontMgr.PrintTextAlpha( iXPos + 512 + iCurXPos, iYPos + 68, FONT_SIZE_20, Alpha, "%s 대회팀", m_szTournamentTeamName );
}

void TotalResultTournamentWnd::OnTournamentTextRender( int iXPos, int iYPos )
{
	if( !m_pResultTextBack			||
		!m_pRedHalfChampionText		||
		!m_pBlueChampionText		||
		!m_pBlueHalfChampionText	||
		!m_pRedChampionText			||
		!m_pBlueFinalRoundText		||
		!m_pRedFinalRoundText		||
		!m_pBlueNextGoText			||
		!m_pBlueNextFailText		||
		!m_pBlueRoundText			||
		!m_pBlueRoundNum			||
		!m_pRedNextGoText			||
		!m_pRedNextFailText			||
		!m_pRedRoundText			||
		!m_pRedRoundNum )
		return;
	

	enum{ IYPOS = 36, IHEIGHT = 72 };

	int iCurYPos = ( FLOAT1 - sin( D3DX_PI/2 * m_fTournamentResultTextBackAniRate )) * IYPOS;
	int iCurHeight = sin( D3DX_PI/2 * m_fTournamentResultTextBackAniRate ) * IHEIGHT;

	m_pResultTextBack->SetSize( 293, 2 + iCurHeight );	
	m_pResultTextBack->Render( iXPos + 246, iYPos + 93 + iCurYPos );
	
	float SCALE_RATE = 2.5f;
	float fScale = ( FLOAT1 - sin( D3DX_PI/2 * m_fTournamentResultTextAniRate )) * SCALE_RATE;
	fScale = max( fScale, FLOAT1 );
	BYTE Alpha = sin( D3DX_PI/2 * m_fTournamentResultTextAniRate) * 255;
	
	int iWidth = 0;
	DWORD dwColor = 0;
	DWORD dwGapTime = FRAMEGETTIME() - m_dwStartTime;
	if( dwGapTime > m_dwResultTextBlinkStartTime )
	{
		dwGapTime -= m_dwResultTextBlinkStartTime;
		if( dwGapTime < m_dwResultTextBlinkGapTime )
		{
			dwColor = ( FLOAT1 - sin( D3DX_PI/2 * m_fTournamentResultTextBlinkAniRate)) * 255;
		}
		else
		{
			dwColor = 0;
		}
	}
	else
	{
		dwColor = 0;
	}
	
	//우승, 준우승
	if( m_iTournamentCurRound == 1 )
	{
		//우승팀이 블루일때
		if( m_eWinner == WTT_BLUE_TEAM )
		{
			if( m_eMyTeam == TEAM_BLUE )
			{
				m_pBlueChampionText->SetAlpha( Alpha );
				m_pBlueChampionText->SetScale( fScale, fScale );
				m_pBlueChampionText->SetHorzFormatting(ioUIRenderImage::HF_RIGHT_ALIGN );
				m_pBlueChampionText->SetColor( dwColor, dwColor, dwColor );
				m_pBlueChampionText->Render( iXPos + 525, iYPos + 95, UI_RENDER_NORMAL_ADD_COLOR );

			}			
			else
			{
				m_pRedHalfChampionText->SetAlpha( Alpha );
				m_pRedHalfChampionText->SetScale( fScale, fScale );
				m_pRedHalfChampionText->SetHorzFormatting(ioUIRenderImage::HF_RIGHT_ALIGN );
				m_pRedHalfChampionText->SetColor( dwColor, dwColor, dwColor );
				m_pRedHalfChampionText->Render( iXPos + 525, iYPos + 95, UI_RENDER_NORMAL_ADD_COLOR  );
			}
		}
		//우승팀이 레드 일때
		else
		{
			//블루팀 준우승
			if( m_eMyTeam == TEAM_RED )
			{
				m_pRedChampionText->SetAlpha( Alpha );
				m_pRedChampionText->SetScale( fScale, fScale );
				m_pRedChampionText->SetHorzFormatting(ioUIRenderImage::HF_RIGHT_ALIGN );
				m_pRedChampionText->SetColor( dwColor, dwColor, dwColor );
				m_pRedChampionText->Render( iXPos + 525, iYPos + 95, UI_RENDER_NORMAL_ADD_COLOR  );
			}
			else
			{
				m_pBlueHalfChampionText->SetAlpha( Alpha );
				m_pBlueHalfChampionText->SetScale( fScale, fScale );
				m_pBlueHalfChampionText->SetHorzFormatting(ioUIRenderImage::HF_RIGHT_ALIGN );
				m_pBlueHalfChampionText->SetColor( dwColor, dwColor, dwColor );
				m_pBlueHalfChampionText->Render( iXPos + 525, iYPos + 95, UI_RENDER_NORMAL_ADD_COLOR  );
			}
		}
	}
	else
	{
		//블루팀 진출
		if( m_eWinner == WTT_BLUE_TEAM )
		{
			if( m_eMyTeam == TEAM_BLUE )
			{
				m_pBlueNextGoText->SetAlpha( Alpha );
				m_pBlueNextGoText->SetScale( fScale, fScale );
				m_pBlueNextGoText->SetHorzFormatting(ioUIRenderImage::HF_RIGHT_ALIGN );
				m_pBlueNextGoText->SetColor( dwColor, dwColor, dwColor );
				m_pBlueNextGoText->Render( iXPos + 525, iYPos + 95, UI_RENDER_NORMAL_ADD_COLOR  );
				iWidth += m_pBlueNextGoText->GetWidth();

			}
			else
			{
				m_pRedNextFailText->SetAlpha( Alpha );
				m_pRedNextFailText->SetScale( fScale, fScale );
				m_pRedNextFailText->SetHorzFormatting(ioUIRenderImage::HF_RIGHT_ALIGN );
				m_pRedNextFailText->SetColor( dwColor, dwColor, dwColor );
				m_pRedNextFailText->Render( iXPos + 525, iYPos + 95, UI_RENDER_NORMAL_ADD_COLOR );
				iWidth += m_pRedNextFailText->GetWidth();
			}
		}	
		//레드팀 진출
		else
		{
			if( m_eMyTeam == TEAM_RED )
			{
				m_pRedNextGoText->SetAlpha( Alpha );
				m_pRedNextGoText->SetScale( fScale, fScale );
				m_pRedNextGoText->SetHorzFormatting(ioUIRenderImage::HF_RIGHT_ALIGN );
				m_pRedNextGoText->SetColor( dwColor, dwColor, dwColor );
				m_pRedNextGoText->Render( iXPos + 525, iYPos + 95, UI_RENDER_NORMAL_ADD_COLOR );
				iWidth += m_pRedNextGoText->GetWidth();
			}
			else
			{
				m_pBlueNextFailText->SetAlpha( Alpha );
				m_pBlueNextFailText->SetScale( fScale, fScale );
				m_pBlueNextFailText->SetHorzFormatting(ioUIRenderImage::HF_RIGHT_ALIGN );
				m_pBlueNextFailText->SetColor( dwColor, dwColor, dwColor );
				m_pBlueNextFailText->Render( iXPos + 525, iYPos + 95, UI_RENDER_NORMAL_ADD_COLOR );
				iWidth += m_pBlueNextFailText->GetWidth();
			}
		}

		//결승
		if( m_iTournamentCurRound <= 2 )
		{
			if( m_eMyTeam == TEAM_BLUE )
			{
				m_pBlueFinalRoundText->SetAlpha( Alpha );
				m_pBlueFinalRoundText->SetScale( fScale, fScale );
				m_pBlueFinalRoundText->SetHorzFormatting(ioUIRenderImage::HF_RIGHT_ALIGN );
				m_pBlueFinalRoundText->SetColor( dwColor, dwColor, dwColor );
				m_pBlueFinalRoundText->Render( iXPos + 525 - iWidth, iYPos + 95, UI_RENDER_NORMAL_ADD_COLOR  );
			}
			else
			{
				m_pRedFinalRoundText->SetAlpha( Alpha );
				m_pRedFinalRoundText->SetScale( fScale, fScale );
				m_pRedFinalRoundText->SetHorzFormatting(ioUIRenderImage::HF_RIGHT_ALIGN );
				m_pRedFinalRoundText->SetColor( dwColor, dwColor, dwColor );
				m_pRedFinalRoundText->Render( iXPos + 525 - iWidth, iYPos + 95, UI_RENDER_NORMAL_ADD_COLOR  );
			}
		}
		//n강
		else
		{
			if( m_eMyTeam == TEAM_BLUE )
			{
				m_pBlueRoundText->SetAlpha( Alpha );
				m_pBlueRoundText->SetScale( fScale, fScale );
				m_pBlueRoundText->SetHorzFormatting(ioUIRenderImage::HF_RIGHT_ALIGN );
				m_pBlueRoundText->SetColor( dwColor, dwColor, dwColor );
				m_pBlueRoundText->Render( iXPos + 525 - iWidth, iYPos + 95, UI_RENDER_NORMAL_ADD_COLOR );

				if( 1 < GetNumberCount( m_iTournamentCurRound ) )
					iWidth += m_pBlueRoundText->GetWidth() - 20;
				else
					iWidth += m_pBlueRoundText->GetWidth() - 5;
								
				m_pBlueRoundNum->SetAlpha( Alpha );
				m_pBlueRoundNum->SetHorzFormatting(ioUIRenderImage::HF_RIGHT_ALIGN );
				m_pBlueRoundNum->SetColor( dwColor, dwColor, dwColor );
				m_pBlueRoundNum->RenderNum( iXPos + 525 - iWidth, iYPos + 95 - ( 35 * (fScale - FLOAT1)), m_iTournamentCurRound, -10.0f, fScale, UI_RENDER_NORMAL_ADD_COLOR );		
			}
			else
			{
				m_pRedRoundText->SetAlpha( Alpha );
				m_pRedRoundText->SetHorzFormatting(ioUIRenderImage::HF_RIGHT_ALIGN );
				m_pRedRoundText->SetColor( dwColor, dwColor, dwColor );
				m_pRedRoundText->Render( iXPos + 525 - iWidth, iYPos + 95, UI_RENDER_NORMAL_ADD_COLOR );

				if( 1 < GetNumberCount( m_iTournamentCurRound ) )
					iWidth += m_pRedRoundText->GetWidth() - 20;
				else
					iWidth += m_pRedRoundText->GetWidth() - 5;

				m_pRedRoundNum->SetAlpha( Alpha );
				m_pRedRoundNum->SetHorzFormatting(ioUIRenderImage::HF_RIGHT_ALIGN );
				m_pRedRoundNum->SetColor( dwColor, dwColor, dwColor );
				m_pRedRoundNum->RenderNum( iXPos + 525 - iWidth, iYPos + 95 - ( 35 * (fScale - FLOAT1)), m_iTournamentCurRound, -10.0f, fScale, UI_RENDER_NORMAL_ADD_COLOR );
			}
		}
	}
}

void TotalResultTournamentWnd::OnTournamentPesoRender( int iXPos, int iYPos )
{
	if( !m_pPesoNum || !m_pPesoIconBack || !m_pPesoIcon || !m_pPesoText )
		return;

	m_pPesoText->SetHorzFormatting(ioUIRenderImage::HF_RIGHT_ALIGN );
	m_pPesoText->Render( iXPos + 514, iYPos + 191 );
		
	DWORD dwColor = 0;
	float fScale = FLOAT1;
	DWORD dwGapTime = FRAMEGETTIME() - m_dwStartTime;
	if( dwGapTime > m_dwPesoBlinkStartTime )
	{
		dwGapTime -= m_dwPesoBlinkStartTime;
		if( dwGapTime < m_dwPesoBlinkGapTime )
		{
			dwColor = 255;
			fScale  = 0.25f * (1.0f - m_fTournamentPesoBlinkAniRate);
			fScale  += FLOAT1;
		}
		else
		{
			dwColor = 0;
			fScale = FLOAT1;
		}
	}
	else
	{
		dwColor = 0;
		fScale = FLOAT1;
	}
		
 	int iNumWidth = ((m_iPesoSize * 24) - (m_iPesoSize-1) * 5);
	m_pPesoIconBack->SetScale( 0.65f, 0.65f );
	m_pPesoIconBack->Render( iXPos + 474 - (iNumWidth+22), iYPos + 202, UI_RENDER_MULTIPLY, TFO_BILINEAR );
	
	m_pPesoIcon->SetScale( 0.65f, 0.65f );
	m_pPesoIcon->Render( iXPos + 474 - (iNumWidth+22), iYPos + 202, UI_RENDER_NORMAL, TFO_BILINEAR );	

	m_pPesoNum->SetHorzFormatting( ioUIRenderImage::HF_CENTER );
	m_pPesoNum->SetColor( 0, 0, 0 );
	m_pPesoNum->RenderNum( iXPos + 474 - iNumWidth/2, iYPos + 188, m_iPesoCurValue, -5.0f, FLOAT1, UI_RENDER_NORMAL_ADD_COLOR );

	m_pPesoNum->SetColor( dwColor, dwColor, dwColor );
	m_pPesoNum->RenderNum( iXPos + 474 - iNumWidth/2, iYPos + 188, m_iPesoCurValue, -5.0f, fScale, UI_RENDER_NORMAL_ADD_COLOR );
}

void TotalResultTournamentWnd::OnProgressGaugeRender( int iXPos, int iYPos )
{
	if( !m_pProgressGaugeBack || !m_pProgressGauge || !m_pProgressGaugeArrow )
		return;
	
	m_pProgressGaugeBack->Render( iXPos + 22, iYPos + 295 );
	DWORD dwGapTime = FRAMEGETTIME() - m_dwStartTime;
	
	float fScale = sin( D3DX_PI/2 * m_fTournamentGaugeStrechAniRate );
	m_pProgressGauge->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
		
	m_pProgressGauge->SetSize( (m_iTournamentGaugeWidth + 44) * fScale, 16 );
	if( m_dwGagueStrechStartTime < dwGapTime )
	{
		m_pProgressGauge->Render( iXPos + 24, iYPos + 311 );	
		m_pProgressGaugeArrow->Render( iXPos + 22 + (44 + m_iTournamentGaugeWidth) * fScale, iYPos + 299 );
	}

	BYTE Alpha = sin( D3DX_PI/2 * m_fTournamentGaugeAlphaAniRate) * 255;

	int iXOffset = 22;
	int iCnt = 1;
	for( int i = m_iTournamentCurPage; i < m_iTournamentCurPage + m_iTournamentPageMaxRound; ++i )
	{
		ioComplexStringPrinter kPrinter;
		kPrinter.SetTextStyle( TS_NORMAL );
		kPrinter.SetBkColor( 0, 0, 0 );

		if( i < m_iTournamentCurPos + 1 )
			kPrinter.SetTextColor( TCT_DEFAULT_LEMON );
		else
			kPrinter.SetTextColor( TCT_DEFAULT_LIGHTGRAY );

		if( m_iTournamentCurPage < m_iTournamentPageMax &&	//마지막 페이지가 아닐 경우에만
			0 < m_iTournamentPageMax &&						//페이지가 없는 경우 제외
			iCnt == TOURNAMENT_VIEW_MAX &&					//8번쨰 라운드만
			1 < m_iTournamentMaxRound - i )					//결승라운드는 ~ 표시 제외
		{
			kPrinter.AddTextPiece( FONT_SIZE_10, " ~ ");
		}
		else if( 1 == m_iTournamentMaxRound - i )
		{
			kPrinter.AddTextPiece( FONT_SIZE_10, "결승");
		}
		else
		{
			kPrinter.AddTextPiece( FONT_SIZE_10, "%d강", (int)pow( 2.0, (double)m_iTournamentMaxRound-i ) );
		}

		kPrinter.PrintFullText( iXPos + 44 + iXOffset, iYPos + 313, TAT_RIGHT, Alpha );
		kPrinter.ClearList();

		iXOffset += GAUGE_WIDTH / m_iTournamentPageMaxRound;
		iCnt++;
	}
}

void TotalResultTournamentWnd::OnRenderSoldier( int iXPos, int iYPos )
{
	if( !m_pUI3DRender || !m_aMyChar ) return;

	D3DRECT rcD3D;
	rcD3D.x1 = iXPos + 2;
	rcD3D.y1 = iYPos + 2;
	rcD3D.x2 = rcD3D.x1 + VIEW_WIDTH;
	rcD3D.y2 = rcD3D.y1 + VIEW_HEIGHT;
	m_pUI3DRender->RenderViewPort( &rcD3D, true );
}
//////////////////////////////////////////////////////////////////////////
ResultScoreUserWnd::ResultScoreUserWnd()
{
	m_pMyTeamFrm    = NULL;
	m_pGuildMarkFrm = NULL;
	m_pCampBonusFrm = NULL;
	m_pShuffleFrm   = NULL;
	
	m_pBlueWin	= NULL;
	m_pBlueLose = NULL;
	m_pBlueDraw = NULL;
	m_pRedWin   = NULL;
	m_pRedLose  = NULL;
	m_pRedDraw  = NULL;
	m_pGameOver = NULL;
	m_pClear    = NULL;
	m_pCampBlueMark = NULL;
	m_pCampRedMark  = NULL;
	m_pCampMarkBack = NULL;
	m_pModeIconBack = NULL;
	m_pHumanIcon	= NULL;
	m_pHumanText	= NULL;
	m_pGangsiText	= NULL;

	m_pBlueDark = NULL;
	m_pBlueLight= NULL;
	m_pBlueOver = NULL;
	m_pRedDark  = NULL;
	m_pRedLight = NULL;
	m_pRedOver  = NULL;

	m_pPCRoomBlue = NULL;
	m_pPCRoomRed  = NULL;
	m_pGuildBlue  = NULL;
	m_pGuildRed   = NULL;
	m_pFriendBlue = NULL;
	m_pFriendRed  = NULL;
	m_pVictoriesBlue1 = NULL;
	m_pVictoriesRed1  = NULL;
	m_pVictoriesBlue2 = NULL;
	m_pVictoriesRed2  = NULL;
	m_pVictoriesNumBlue = NULL;
	m_pVictoriesNumRed  = NULL;

	m_pBlueNumber	= NULL;
	m_pRedNumber	= NULL;
	m_pBluePer		= NULL;
	m_pRedPer		= NULL;
	m_pBluePointText= NULL;
	m_pRedPointText = NULL;
	m_pRedRankText	= NULL;

	m_pGradeUpText  = NULL;

	m_pCurrentDark = NULL;
	m_pCurrentLight= NULL;
	m_pCurrentOver = NULL;
	m_pCurrentResult = NULL;
	m_pCurrentPoint  = NULL;
	m_pCurrentPointText = NULL;

	m_pPlayStage= NULL;
	m_eWindowTeam = TEAM_NONE;

	m_iMaxPlayer = 0;
	m_dwTickTime = 0;
	m_dwCurrentTime  = 0;
	m_MouseOverIndex = -1;
	m_fBlueGuildTeamBonus = 0.0f;
	m_fRedGuildTeamBonus = 0.0f;
	m_iStartIndex = -1;

	m_dwRollingTick = 100;
	m_dwRollingCurrentTime = 0;
	m_iRollingIndex = 0;
	m_iPoint = 0;
	m_bGangsiWin = false;
	m_bSocrePointRender = true;

	m_pShuffleResultIcon     = NULL;
	m_pShuffleResultIconBack = NULL;
	m_pShuffleLine           = NULL;

	InitGradeUpAni();
}

ResultScoreUserWnd::~ResultScoreUserWnd()
{
	SAFEDELETE( m_pMyTeamFrm );
	SAFEDELETE( m_pGuildMarkFrm );
	SAFEDELETE( m_pCampBonusFrm );
	SAFEDELETE( m_pShuffleFrm );
	
	SAFEDELETE( m_pBlueWin );
	SAFEDELETE( m_pBlueLose );
	SAFEDELETE( m_pBlueDraw );
	SAFEDELETE( m_pRedWin );
	SAFEDELETE( m_pRedLose );
	SAFEDELETE( m_pRedDraw );
	SAFEDELETE( m_pGameOver );
	SAFEDELETE( m_pClear );
	SAFEDELETE( m_pCampMarkBack );
	SAFEDELETE( m_pCampBlueMark );
	SAFEDELETE( m_pCampRedMark );
	SAFEDELETE( m_pModeIconBack );
	SAFEDELETE( m_pHumanIcon );
	SAFEDELETE( m_pHumanText );
	SAFEDELETE( m_pGangsiText );

	SAFEDELETE( m_pBlueDark );
	SAFEDELETE( m_pBlueLight );
	SAFEDELETE( m_pBlueOver );
	SAFEDELETE( m_pRedDark );
	SAFEDELETE( m_pRedLight );
	SAFEDELETE( m_pRedOver );

	SAFEDELETE( m_pPCRoomBlue );
	SAFEDELETE( m_pPCRoomRed );
	SAFEDELETE( m_pGuildBlue );
	SAFEDELETE( m_pGuildRed );
	SAFEDELETE( m_pFriendBlue );
	SAFEDELETE( m_pFriendRed );
	SAFEDELETE( m_pVictoriesBlue1 );
	SAFEDELETE( m_pVictoriesRed1 );
	SAFEDELETE( m_pVictoriesBlue2 );
	SAFEDELETE( m_pVictoriesRed2 );
	SAFEDELETE( m_pVictoriesNumBlue );
	SAFEDELETE( m_pVictoriesNumRed );

	SAFEDELETE( m_pBlueNumber );
	SAFEDELETE( m_pRedNumber );
	SAFEDELETE( m_pBluePer );
	SAFEDELETE( m_pRedPer );
	SAFEDELETE( m_pBluePointText );
	SAFEDELETE( m_pRedPointText );
	SAFEDELETE( m_pRedRankText );

	SAFEDELETE( m_pGradeUpText );

	SAFEDELETE( m_pShuffleResultIcon );
	SAFEDELETE( m_pShuffleResultIconBack );
	SAFEDELETE( m_pShuffleLine );
}

void ResultScoreUserWnd::InitGradeUpAni()
{
	enum { INFOVIEW = 500, SCALE_UP1 = 50, SCALE_UP2 = 50, SCALE_UP3 = 500, };

	m_GradeUpAni.SetCurState( GUA_NONE );

	FSMState *pState = new FSMState( GUA_NONE );
	if( pState )
		pState->Add( GUA_NONE, GUA_START );
	m_GradeUpAni.Add( pState );

	pState = new FSMState( GUA_START );
	if( pState )
		pState->Add( GUA_START, GUA_INFO_VIEW, 0 );
	m_GradeUpAni.Add( pState );

	pState = new FSMState( GUA_INFO_VIEW );
	if( pState )
		pState->Add( GUA_INFO_VIEW, GUA_SCALE_UP1, INFOVIEW );
	m_GradeUpAni.Add( pState );

	pState = new FSMState( GUA_SCALE_UP1 );
	if( pState )
		pState->Add( GUA_SCALE_UP1, GUA_SCALE_UP2, SCALE_UP1 );
	m_GradeUpAni.Add( pState );	

	pState = new FSMState( GUA_SCALE_UP2 );
	if( pState )
		pState->Add( GUA_SCALE_UP2, GUA_SCALE_UP3, SCALE_UP2 );
	m_GradeUpAni.Add( pState );

	pState = new FSMState( GUA_SCALE_UP3 );
	if( pState )
		pState->Add( GUA_SCALE_UP3, GUA_INFO_VIEW, SCALE_UP3 );
	m_GradeUpAni.Add( pState );

	m_GradeUpAni.SetCurState( GUA_START );
}

void ResultScoreUserWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "MyTeamFrm" )
	{
		SAFEDELETE( m_pMyTeamFrm );
		m_pMyTeamFrm = pFrame;
	}
	else if( szType == "GuildMarkFrm" )
	{
		SAFEDELETE( m_pGuildMarkFrm );
		m_pGuildMarkFrm = pFrame;
	}
	else if( szType == "CampBonusFrm" )
	{
		SAFEDELETE( m_pCampBonusFrm );
		m_pCampBonusFrm = pFrame;
	}
	else if( szType == "ShuffleFrm" )
	{
		SAFEDELETE( m_pShuffleFrm );
		m_pShuffleFrm = pFrame;
	}
	else
	{
		ioWnd::AddRenderFrame( szType, pFrame );
	}
}

void ResultScoreUserWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "BlueWin" )
	{
		SAFEDELETE( m_pBlueWin );
		m_pBlueWin = pImage;
	}
	else if( szType == "BlueLose" )
	{
		SAFEDELETE( m_pBlueLose );
		m_pBlueLose = pImage;
	}
	else if( szType == "BlueDraw" )
	{
		SAFEDELETE( m_pBlueDraw );
		m_pBlueDraw = pImage;
	}
	else if( szType == "RedWin" )
	{
		SAFEDELETE( m_pRedWin );
		m_pRedWin = pImage;
	}
	else if( szType == "RedLose" )
	{
		SAFEDELETE( m_pRedLose );
		m_pRedLose = pImage;
	}
	else if( szType == "RedDraw" )
	{
		SAFEDELETE( m_pRedDraw );
		m_pRedDraw = pImage;
	}
	else if( szType == "GameOver" )
	{
		SAFEDELETE( m_pGameOver );
		m_pGameOver = pImage;
	}
	else if( szType == "Clear" )
	{
		SAFEDELETE( m_pClear );
		m_pClear = pImage;
	}
	else if( szType == "CampMarkBack" )
	{
		SAFEDELETE( m_pCampMarkBack );
		m_pCampMarkBack = pImage;
	}
	else if( szType == "ModeIconBack" )
	{
		SAFEDELETE( m_pModeIconBack );
		m_pModeIconBack = pImage;
	}
	else if( szType == "HumanIcon" )
	{
		SAFEDELETE( m_pHumanIcon );
		m_pHumanIcon = pImage;
	}
	else if( szType == "HumanText" )
	{
		SAFEDELETE( m_pHumanText );
		m_pHumanText = pImage;
	}
	else if( szType == "GangsiText" )
	{
		SAFEDELETE( m_pGangsiText );
		m_pGangsiText = pImage;
	}	
	else if( szType == "BlueDark" )
	{
		SAFEDELETE( m_pBlueDark );
		m_pBlueDark = pImage;
	}
	else if( szType == "BlueLight" )
	{
		SAFEDELETE( m_pBlueLight );
		m_pBlueLight = pImage;
	}
	else if( szType == "BlueOver" )
	{
		SAFEDELETE( m_pBlueOver );
		m_pBlueOver = pImage;
	}
	else if( szType == "RedDark" )
	{
		SAFEDELETE( m_pRedDark );
		m_pRedDark = pImage;
	}
	else if( szType == "RedLight" )
	{
		SAFEDELETE( m_pRedLight );
		m_pRedLight = pImage;
	}
	else if( szType == "RedOver" )
	{
		SAFEDELETE( m_pRedOver );
		m_pRedOver = pImage;
	}
	else if( szType == "PCRoomBlue" )
	{
		SAFEDELETE( m_pPCRoomBlue );
		m_pPCRoomBlue = pImage;
	}
	else if( szType == "PCRoomRed" )
	{
		SAFEDELETE( m_pPCRoomRed );
		m_pPCRoomRed = pImage;
	}
	else if( szType == "GuildBlue" )
	{
		SAFEDELETE( m_pGuildBlue );
		m_pGuildBlue = pImage;
	}
	else if( szType == "GuildRed" )
	{
		SAFEDELETE( m_pGuildRed );
		m_pGuildRed = pImage;
	}
	else if( szType == "FriendBlue" )
	{
		SAFEDELETE( m_pFriendBlue );
		m_pFriendBlue = pImage;
	}
	else if( szType == "FriendRed" )
	{
		SAFEDELETE( m_pFriendRed );
		m_pFriendRed = pImage;
	}
	else if( szType == "VictoriesBlue1" )
	{
		SAFEDELETE( m_pVictoriesBlue1 );
		m_pVictoriesBlue1 = pImage;
	}
	else if( szType == "VictoriesRed1" )
	{
		SAFEDELETE( m_pVictoriesRed1 );
		m_pVictoriesRed1 = pImage;
	}
	else if( szType == "VictoriesBlue2" )
	{
		SAFEDELETE( m_pVictoriesBlue2 );
		m_pVictoriesBlue2 = pImage;
	}
	else if( szType == "VictoriesRed2" )
	{
		SAFEDELETE( m_pVictoriesRed2 );
		m_pVictoriesRed2 = pImage;
	}
	else if( szType == "VictoriesNumBlue" )
	{
		SAFEDELETE( m_pVictoriesNumBlue );
		m_pVictoriesNumBlue = pImage;
	}
	else if( szType == "VictoriesNumRed" )
	{
		SAFEDELETE( m_pVictoriesNumRed );
		m_pVictoriesNumRed = pImage;
	}
	else if( szType == "BlueNumber" )
	{
		SAFEDELETE( m_pBlueNumber );
		m_pBlueNumber = pImage;
	}
	else if( szType == "RedNumber" )
	{
		SAFEDELETE( m_pRedNumber );
		m_pRedNumber = pImage;
	}
	else if( szType == "BluePer" )
	{
		SAFEDELETE( m_pBluePer );
		m_pBluePer = pImage;
	}
	else if( szType == "RedPer" )
	{
		SAFEDELETE( m_pRedPer );
		m_pRedPer = pImage;
	}
	else if( szType == "BluePointText" )
	{
		SAFEDELETE( m_pBluePointText );
		m_pBluePointText = pImage;
	}
	else if( szType == "RedPointText" )
	{
		SAFEDELETE( m_pRedPointText );
		m_pRedPointText = pImage;
	}
	else if( szType == "RedRankText" )
	{
		SAFEDELETE( m_pRedRankText );
		m_pRedRankText = pImage;
	}
	else if( szType == "GradeUpText" )
	{
		SAFEDELETE( m_pGradeUpText );
		m_pGradeUpText = pImage;

		if( m_pGradeUpText )
		{
			m_pGradeUpText->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
			m_pGradeUpText->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
		}
	}
	else if( szType == "ShuffleResultIcon" )
	{
		SAFEDELETE( m_pShuffleResultIcon );
		m_pShuffleResultIcon = pImage;

		if( m_pShuffleResultIcon )
			m_pShuffleResultIcon->SetScale( FLOAT08, FLOAT08 );
	}
	else if( szType == "ShuffleResultIconBack" )
	{
		SAFEDELETE( m_pShuffleResultIconBack );
		m_pShuffleResultIconBack = pImage;

		if( m_pShuffleResultIconBack )
			m_pShuffleResultIconBack->SetScale( FLOAT08, FLOAT08 );
	}
	else if( szType == "ShuffleLine" )
	{
		SAFEDELETE( m_pShuffleLine );
		m_pShuffleLine = pImage;

		if( m_pShuffleLine )
		{
			m_pShuffleLine->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
			m_pShuffleLine->SetSize( 89, 1 );
		}
	}
	else 
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void ResultScoreUserWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_iMaxPlayer = xElement.GetIntAttribute_e( "MaxPlayer" );
	m_dwTickTime = xElement.GetIntAttribute_e( "TickTime" );
	m_dwRollingTick = xElement.GetIntAttribute_e( "RollingTickTime" );
	m_szCountSound = xElement.GetStringAttribute_e( "CountSound" );
}

int ResultScoreUserWnd::GetMyContribute()
{
	for(int i = 0;i < (int)m_FinalInfoList.size();i++)
	{
		FinalInfo &rkInfo = m_FinalInfoList[i];
		if( rkInfo.m_bOwner )
			return rkInfo.m_iContributePer;
	}
	return 0;
}

int ResultScoreUserWnd::GetMyKO()
{
	for(int i = 0;i < (int)m_FinalInfoList.size();i++)
	{
		FinalInfo &rkInfo = m_FinalInfoList[i];
		if( rkInfo.m_bOwner )
			return rkInfo.m_iUniqueTotalKill;
	}
	return 0;
}

int ResultScoreUserWnd::GetResultLadderPoint()
{
	for(int i = 0;i < (int)m_FinalInfoList.size();i++)
	{
		FinalInfo &rkInfo = m_FinalInfoList[i];
		if( rkInfo.m_iLadderPoint > 0 )
			return rkInfo.m_iResultLadderPoint;
	}
	return 0;
}

int ResultScoreUserWnd::GetMyShuffleStar()
{
	for(int i = 0;i < (int)m_FinalInfoList.size();i++)
	{
		FinalInfo &rkInfo = m_FinalInfoList[i];
		if( rkInfo.m_bOwner )
			return rkInfo.m_iStarCount;
	}
	return 0;
}

int ResultScoreUserWnd::GetMyHunterCoin()
{
	for(int i = 0;i < (int)m_FinalInfoList.size();i++)
	{
		FinalInfo &rkInfo = m_FinalInfoList[i];
		if( rkInfo.m_bOwner )
			return rkInfo.m_iHunterCoinCnt;
	}
	return 0;
}

bool ResultScoreUserWnd::IsMyTeam()
{
	for(int i = 0;i < (int)m_FinalInfoList.size();i++)
	{
		FinalInfo &rkInfo = m_FinalInfoList[i];
		if( rkInfo.m_bOwner )
			return true;
	}
	return false;
}

void ResultScoreUserWnd::SetCustomWnd( int iMaxPlayer, int iWndXPos, int iWndYPos, int iWndWidth, int iWndHeight )
{
	m_iMaxPlayer = iMaxPlayer;

	SetWndPos( iWndXPos, iWndYPos );
	SetSize( iWndWidth, iWndHeight );

	if( m_pMyTeamFrm )
	{
		m_pMyTeamFrm->SetSize( iWndWidth, iWndHeight );
	}

	m_bSocrePointRender = true;
}

void ResultScoreUserWnd::SetStartScore()
{
	m_dwCurrentTime = FRAMEGETTIME();
}

void ResultScoreUserWnd::SetWindowTeam( TeamType eTeam )
{
	m_eWindowTeam = eTeam; 
}

void ResultScoreUserWnd::SetPlayStage( ioPlayStage *pPlayStage )
{
	m_pPlayStage = pPlayStage;
}

void ResultScoreUserWnd::AddFinalInfo( const FinalInfo &rkInfo )
{
	m_FinalInfoList.push_back( rkInfo );
	m_iStartIndex = m_FinalInfoList.size() + 1; // +1은 내 UI 프레임을 확인하기 위한 훼이크 -_-;
}

void ResultScoreUserWnd::SetSingleScore()
{
	m_pCurrentResult    = NULL;
	m_pCurrentPoint     = m_pRedNumber;
	m_pCurrentPointText = m_pRedRankText;

	// 내 순위
	m_iPoint = 1;
	for(int i = 0;i < (int)m_FinalInfoList.size();i++)
	{
		FinalInfo &rkInfo = m_FinalInfoList[i];
		if( rkInfo.m_bOwner ) break;

		m_iPoint++;
	}
}

void ResultScoreUserWnd::SetDungeonsScore( int iResultFloor, int iMaxFloor )
{
	if( iResultFloor >= iMaxFloor )
		m_pCurrentResult = m_pClear;
	else
		m_pCurrentResult = m_pGameOver;
	m_pCurrentPoint      = NULL;
	m_pCurrentPointText  = NULL;
	m_iPoint = iResultFloor;
}

void ResultScoreUserWnd::SetTeamScore( TeamType eMyTeam, int iBlueWin, int iRedWin, bool bSocrePointRender )
{
	m_bSocrePointRender = bSocrePointRender;

	if( m_eWindowTeam == TEAM_BLUE )
	{
		if( iBlueWin > iRedWin )
			m_pCurrentResult = m_pBlueWin;
		else if( iBlueWin < iRedWin )
			m_pCurrentResult = m_pBlueLose;
		else
			m_pCurrentResult = m_pBlueDraw;
		if( ioPlayMode::GetModeType() == MT_TEAM_SURVIVAL || ioPlayMode::GetModeType() == MT_TEAM_SURVIVAL_AI )
		{
			m_pCurrentPoint  = m_pBlueNumber;
			m_pCurrentPointText = m_pBluePer;
		}
		else if( ioPlayMode::GetModeType() == MT_DOUBLE_CROWN )
		{
			m_pCurrentPoint		= NULL;
			m_pCurrentPointText = NULL;
		}
		else if( ioPlayMode::GetModeType() == MT_HEROMATCH )
		{
			m_pCurrentPoint  = m_pBlueNumber;
			m_pCurrentPointText = m_pBluePointText;

			if( iBlueWin < iRedWin )
			{
				if( !m_FinalInfoList.empty() && iBlueWin < HEROMATCH_NEED_ROUND && iRedWin < HEROMATCH_NEED_ROUND )
					m_pCurrentResult = m_pBlueDraw;
			}
			else if( iBlueWin > iRedWin )
			{
				if( m_FinalInfoList.empty() )
					m_pCurrentResult = m_pBlueDraw;
			}
		}
		else
		{
			m_pCurrentPoint  = m_pBlueNumber;
			m_pCurrentPointText = m_pBluePointText;
		}

		m_iPoint = iBlueWin;
	}
	else
	{
		if( iRedWin > iBlueWin )
			m_pCurrentResult = m_pRedWin;
		else if( iRedWin < iBlueWin )
			m_pCurrentResult = m_pRedLose;
		else
			m_pCurrentResult = m_pRedDraw;

		if( ioPlayMode::GetModeType() == MT_TEAM_SURVIVAL || ioPlayMode::GetModeType() == MT_TEAM_SURVIVAL_AI )
		{
			m_pCurrentPoint  = m_pRedNumber;
			m_pCurrentPointText = m_pRedPer;
		}
		else if( ioPlayMode::GetModeType() == MT_DOUBLE_CROWN )
		{
			m_pCurrentPoint		= NULL;
			m_pCurrentPointText = NULL;
		}
		else if( ioPlayMode::GetModeType() == MT_HEROMATCH )
		{
			m_pCurrentPoint  = m_pRedNumber;
			m_pCurrentPointText = m_pRedPointText;

			if( iRedWin < iBlueWin )
			{
				if( !m_FinalInfoList.empty() && iBlueWin < HEROMATCH_NEED_ROUND && iRedWin < HEROMATCH_NEED_ROUND )
					m_pCurrentResult = m_pRedDraw;
			}
			else if( iRedWin > iBlueWin )
			{
				if( m_FinalInfoList.empty() )
					m_pCurrentResult = m_pRedDraw;
			}
		}
		else
		{
			m_pCurrentPoint  = m_pRedNumber;
			m_pCurrentPointText = m_pRedPointText;
		}

		m_iPoint = iRedWin;
	}
}

void ResultScoreUserWnd::SetGangsiScore( bool bBlueTeamWin )
{
	m_bGangsiWin = bBlueTeamWin;
	if( m_eWindowTeam == TEAM_BLUE )
	{
		if( m_bGangsiWin )
			m_pCurrentResult = m_pBlueWin;
		else 
			m_pCurrentResult = m_pBlueLose;
	}
	else
	{
		if( !m_bGangsiWin )
			m_pCurrentResult = m_pRedWin;
		else
			m_pCurrentResult = m_pRedLose;
	}
}

void ResultScoreUserWnd::SetGuildTeamBonus( float fBlueGuildTeamBonus, float fRedGuildTeamBonus )
{
	m_fBlueGuildTeamBonus = fBlueGuildTeamBonus;
	m_fRedGuildTeamBonus = fRedGuildTeamBonus;
}

void ResultScoreUserWnd::iwm_show()
{
	if( m_eWindowTeam == TEAM_BLUE )
	{
		m_pCurrentDark = m_pBlueDark;
		m_pCurrentLight= m_pBlueLight;
		m_pCurrentOver = m_pBlueOver;
	}
	else
	{
		m_pCurrentDark = m_pRedDark;
		m_pCurrentLight= m_pRedLight;
		m_pCurrentOver = m_pRedOver;
	}

	m_dwRollingCurrentTime = FRAMEGETTIME();
	m_iRollingIndex        = 0;

	TournamentNode *pTournament = g_TournamentMgr.GetTournament( g_TournamentMgr.GetRegularIndex() );
	if( pTournament )
	{
		m_pCampBlueMark		= pTournament->GetCampIcon( CAMP_BLUE );
		m_pCampRedMark		= pTournament->GetCampIcon( CAMP_RED );
		m_szBlueTeamName	= pTournament->GetCampName( CAMP_BLUE );
		m_szRedTeamName		= pTournament->GetCampName( CAMP_RED );
	}

	m_iStarCount            = 0;
	m_iStarCountByCalcBonus = 0;
	m_fBonusStarPercent     = 0.0f;

	if( g_ShuffleRoomMgr.IsShuffleRoom() )
	{
		m_iStarCount            = g_ShuffleRoomMgr.GetShuffleStarCnt();
		m_iStarCountByCalcBonus	= g_ShuffleRoomMgr.GetShuffleStarByCalcBonus();
		m_fBonusStarPercent     = g_ShuffleRoomMgr.GetShuffleBonusPercent();
	}
}

void ResultScoreUserWnd::ProcessSiteClick()
{
	m_MouseOverIndex = -1;

	if( !g_App.IsVisibleMouse() || g_GUIMgr.GetPreOverWnd() != this )
		return;

	int iXPos, iYPos;
	iXPos = GetDerivedPosX() + 8;
	iYPos = GetDerivedPosY() + 59;

	int iUserSize = m_FinalInfoList.size();
	ioLSMouse *pMouse = g_App.GetMouse();
	for(int i = 0;i < iUserSize;i++)
	{
		RECT rcNameRect;
		rcNameRect.left   = iXPos;
		rcNameRect.top    = iYPos + i * 21;
		rcNameRect.right  = rcNameRect.left + 196;
		rcNameRect.bottom = rcNameRect.top + 21;

		if( ::PtInRect( &rcNameRect, pMouse->GetMousePos() ) )
		{
			m_MouseOverIndex = i;
			if( pMouse->IsLBtnUp() )
			{
				g_App.OpenMannerWnd( m_FinalInfoList[i].m_szName.c_str(), true );
			}
			else if( pMouse->IsMouseShowing() )
				pMouse->SetToolTipID( m_FinalInfoList[i].m_szName, UserInfoToolTip::TT_GLOBAL );
			break;
		}
	}
}

void ResultScoreUserWnd::ProcessRolling()
{
	if( FRAMEGETTIME() - m_dwRollingCurrentTime > m_dwRollingTick )
	{
		m_dwRollingCurrentTime = FRAMEGETTIME();
		m_iRollingIndex++;
	}
}

void ResultScoreUserWnd::ProcessGradeUpAni()
{
	if( m_GradeUpAni.GetCurState() == GUA_NONE ) return;

	m_GradeUpAni.UpdateTime();
	switch( m_GradeUpAni.GetCurState() )
	{
	case GUA_INFO_VIEW:
		break;
	case GUA_SCALE_UP1:
		{
			if( m_pGradeUpText )
				m_pGradeUpText->SetSize( 89, 2 );
		}
		break;
	case GUA_SCALE_UP2:
		{
			if( m_pGradeUpText )
				m_pGradeUpText->SetSize( 89, 6 );
		}
		break;
	case GUA_SCALE_UP3:
		{
			if( m_pGradeUpText )
				m_pGradeUpText->SetSize( 89, 14 );
		}
		break;
	}
}

void ResultScoreUserWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	ProcessSiteClick();
	if( m_iStartIndex >= 0 && m_dwCurrentTime != 0 )
	{
		if( FRAMEGETTIME() - m_dwCurrentTime > m_dwTickTime )
		{
			int iPrevIndex = m_iStartIndex;
			m_dwCurrentTime = FRAMEGETTIME();
			m_iStartIndex = max( m_iStartIndex - 1, -1 );
			
			if( m_iStartIndex < (int)m_FinalInfoList.size() && iPrevIndex != m_iStartIndex )
			{
				if( !m_szCountSound.IsEmpty() )
					g_SoundMgr.PlaySound( m_szCountSound );
			}

			if( m_iStartIndex == 0 )
			{	
				if( g_BattleRoomMgr.IsBattleRoom() )
					g_QuestMgr.QuestCompleteTerm( QC_BATTLE_PVP_FINALRESULT_SHOW, GetMyContribute() );
				else if( g_LadderTeamMgr.IsLadderTeam() )
					g_QuestMgr.QuestCompleteTerm( QC_CAMP_FINALRESULT_SHOW, GetMyContribute() );
			}
		}	
	}	
	ProcessRolling();
	ProcessGradeUpAni();
}

void ResultScoreUserWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_iStartIndex <= (int)m_FinalInfoList.size() )
	{
		if( m_pMyTeamFrm && IsMyTeam() )
			m_pMyTeamFrm->Render( iXPos, iYPos );
	}
	
	switch( ioPlayMode::GetModeType() )
	{
	case MT_GANGSI:
		OnRenderGangsiResult( iXPos, iYPos );
		break;
	default:
		OnRenderResult( iXPos, iYPos );
		break;
	}

	OnRenderBack( iXPos + 8, iYPos + 59 );
	OnRenderUser( iXPos + 8, iYPos + 59, ioPlayMode::GetModeType() );
	OnRenderCampBonus( iXPos + 54, iYPos + 27 );

	if( ioPlayMode::GetModeType() == MT_SHUFFLE_BONUS )
		OnRenderShuffleResult(  iXPos + 49, iYPos + 235 );
}

void ResultScoreUserWnd::OnRenderResult( int iXPos, int iYPos )
{	
	int iResultGap = 0;
	if( m_pCurrentResult )
	{
    	m_pCurrentResult->Render( iXPos + 13, iYPos + 13 );
		iResultGap = m_pCurrentResult->GetWidth() + 6;
	}

	if( m_pCurrentPoint && m_bSocrePointRender )
	{
		m_pCurrentPoint->RenderNum( iXPos + 13 + iResultGap, iYPos + 13, m_iPoint, 0.0f, 0.71f, UI_RENDER_NORMAL, TFO_BILINEAR );
		iResultGap += (float)( GetNumberCount( m_iPoint ) * 23 ) * 0.71f;
	}

	if( m_pCurrentPointText && m_bSocrePointRender )
	{
		m_pCurrentPointText->SetScale( 0.71f );
		m_pCurrentPointText->Render( iXPos + 13 + iResultGap, iYPos + 9, UI_RENDER_NORMAL, TFO_BILINEAR );
	}

	if( g_BattleRoomMgr.IsBattleRoom() )
	{
		BattleRoomInfo BattleInfo;
		g_BattleRoomMgr.GetBattleRoomInfo( &BattleInfo );

		bool bFreeForAllMode = g_ModeSelectInfo.IsFreeForAllMode( g_BattleRoomMgr.GetSelectModeIndex() );
		if( bFreeForAllMode )
		{
			// 싱글 모드는 기여도 + KO 표시
			g_FontMgr.SetBkColor( 0, 0, 0 );
			g_FontMgr.SetAlignType( TAT_LEFT );
			g_FontMgr.SetTextStyle( TS_NORMAL );
			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );

			if( BattleInfo.m_iModeType == MT_SHUFFLE_BONUS )
				g_FontMgr.PrintText( iXPos + 13, iYPos + 36, FONT_SIZE_13, "별 획득 %d", GetMyShuffleStar() );
			else if( BattleInfo.m_iModeType == MT_RAID )
				g_FontMgr.PrintText( iXPos + 13, iYPos + 36, FONT_SIZE_13, "헌터 코인 획득 %d", GetMyHunterCoin() );
			else
				g_FontMgr.PrintText( iXPos + 13, iYPos + 36, FONT_SIZE_13, STR(1), GetMyContribute(), GetMyKO() );
		}
		else
		{
			// 싱글 모드가 아닐 때만 승무패표시
			g_FontMgr.SetBkColor( 0, 0, 0 );
			g_FontMgr.SetAlignType( TAT_LEFT );
			g_FontMgr.SetTextStyle( TS_NORMAL );
			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );

			if( m_eWindowTeam == TEAM_BLUE )
			{
				if( BattleInfo.m_iBlueVictories > 1 )
					g_FontMgr.PrintTextWidthCut( iXPos + 13, iYPos + 36, FONT_SIZE_13, 135.0f, STR(2), BattleInfo.m_iBlueWin, BattleInfo.m_iBlueLose, BattleInfo.m_iBlueVictories );
				else
					g_FontMgr.PrintTextWidthCut( iXPos + 13, iYPos + 36, FONT_SIZE_13, 135.0f, STR(3), BattleInfo.m_iBlueWin, BattleInfo.m_iBlueLose );
			}
			else if( m_eWindowTeam == TEAM_RED )
			{
				if( BattleInfo.m_iRedVictories > 1 )
					g_FontMgr.PrintTextWidthCut( iXPos + 13, iYPos + 36, FONT_SIZE_13, 135.0f, STR(4), BattleInfo.m_iRedWin, BattleInfo.m_iRedLose, BattleInfo.m_iRedVictories );
				else
					g_FontMgr.PrintTextWidthCut( iXPos + 13, iYPos + 36, FONT_SIZE_13, 135.0f, STR(5), BattleInfo.m_iRedWin, BattleInfo.m_iRedLose );
			}
		}

		// 모드 아이콘
		if( m_pModeIconBack )
		{
			m_pModeIconBack->SetScale( 0.54f );
			m_pModeIconBack->Render( iXPos + 178, iYPos + 30, UI_RENDER_MULTIPLY, TFO_BILINEAR );

			bool bExtra = false;
			if ( ioPlayMode::GetModeType() == MT_SUCCESSION )
				bExtra = g_MyInfo.GetSuccessionRevengeMode();
			else
				bExtra = g_BattleRoomMgr.IsUseExtraOption();
			ioUIRenderImage *pModeIcon = g_ModeSelectInfo.GetModeIcon( ioPlayMode::GetModeType(), m_eWindowTeam, bExtra );
			if( pModeIcon )
			{
				pModeIcon->SetScale( 0.54f );
				pModeIcon->Render( iXPos + 178, iYPos + 30, UI_RENDER_NORMAL, TFO_BILINEAR );
			}
		}
	}
	else
	{
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		
		if( m_eWindowTeam == TEAM_BLUE )
		{
			LadderTeamInfo LadderInfo = g_LadderTeamMgr.GetLadderBlueTeamInfo();

			char szTitle[MAX_PATH] = "";
			if( LadderInfo.m_dwGuildIndex != 0 )
			{
				if( LadderInfo.m_iVictoriesRecord > 1 )
					SafeSprintf( szTitle, sizeof( szTitle ), STR(6), LadderInfo.m_szTeamName.c_str(), LadderInfo.m_iWinRecord, LadderInfo.m_iLoseRecord, LadderInfo.m_iVictoriesRecord );
				else
					SafeSprintf( szTitle, sizeof( szTitle ), STR(7), LadderInfo.m_szTeamName.c_str(), LadderInfo.m_iWinRecord, LadderInfo.m_iLoseRecord );
			}		
			else
			{
				if( LadderInfo.m_iVictoriesRecord > 1 )
					SafeSprintf( szTitle, sizeof( szTitle ), STR(8), LadderInfo.m_iWinRecord, LadderInfo.m_iLoseRecord, LadderInfo.m_iVictoriesRecord );
				else
					SafeSprintf( szTitle, sizeof( szTitle ), STR(9), LadderInfo.m_iWinRecord, LadderInfo.m_iLoseRecord );
			}

			g_FontMgr.PrintTextWidthCut( iXPos + 13, iYPos + 36, FONT_SIZE_13, 135.0f, szTitle );
			if( LadderInfo.m_dwGuildIndex != 0 )
			{
				ioGuildData *pGuildData = g_GuildInfo.GetGuildData( LadderInfo.m_dwGuildIndex );
				if( pGuildData )
				{
					g_GuildMarkMgr.RenderMarkSize( 35, 35, pGuildData->GetGuildIndex(), pGuildData->GetGuildMark(), iXPos + 162, iYPos + 13 );
					if( m_pGuildMarkFrm )
						m_pGuildMarkFrm->Render( iXPos + 160, iYPos + 11 );
				}
			}
		}
		else if( m_eWindowTeam == TEAM_RED )
		{
			LadderTeamInfo LadderInfo = g_LadderTeamMgr.GetLadderRedTeamInfo();			

			char szTitle[MAX_PATH] = "";
			if( LadderInfo.m_dwGuildIndex != 0 )
			{
				if( LadderInfo.m_iVictoriesRecord > 1 )
					SafeSprintf( szTitle, sizeof( szTitle ), STR(10), LadderInfo.m_szTeamName.c_str(), LadderInfo.m_iWinRecord, LadderInfo.m_iLoseRecord, LadderInfo.m_iVictoriesRecord );
				else
					SafeSprintf( szTitle, sizeof( szTitle ), STR(11), LadderInfo.m_szTeamName.c_str(), LadderInfo.m_iWinRecord, LadderInfo.m_iLoseRecord );
			}		
			else
			{
				if( LadderInfo.m_iVictoriesRecord > 1 )
					SafeSprintf( szTitle, sizeof( szTitle ), STR(12), LadderInfo.m_iWinRecord, LadderInfo.m_iLoseRecord, LadderInfo.m_iVictoriesRecord );
				else
					SafeSprintf( szTitle, sizeof( szTitle ), STR(13), LadderInfo.m_iWinRecord, LadderInfo.m_iLoseRecord );
			}

			g_FontMgr.PrintTextWidthCut( iXPos + 13, iYPos + 36, FONT_SIZE_13, 135.0f, szTitle );
			if( LadderInfo.m_dwGuildIndex != 0 )
			{
				ioGuildData *pGuildData = g_GuildInfo.GetGuildData( LadderInfo.m_dwGuildIndex );
				if( pGuildData )
				{
					g_GuildMarkMgr.RenderMarkSize( 35, 35, pGuildData->GetGuildIndex(), pGuildData->GetGuildMark(), iXPos + 162, iYPos + 13 );
					if( m_pGuildMarkFrm )
						m_pGuildMarkFrm->Render( iXPos + 160, iYPos + 11 );
				}
			}
		}
	}
}

void ResultScoreUserWnd::OnRenderGangsiResult( int iXPos, int iYPos )
{	
	int iResultGap = 0;
	if( m_pCurrentResult )
	{
		m_pCurrentResult->Render( iXPos + 13, iYPos + 13 );
		iResultGap = m_pCurrentResult->GetWidth() + 6;
	}

	if( m_eWindowTeam == TEAM_RED )
	{
		if( m_pHumanText )
			m_pHumanText->Render( iXPos + 13 + iResultGap, iYPos + 13 );

		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		g_FontMgr.PrintText( iXPos + 13, iYPos + 36, FONT_SIZE_13, STR(1), (int)m_FinalInfoList.size() );

		// 인간 아이콘
		if( m_pModeIconBack )
		{
			m_pModeIconBack->SetScale( 0.54f );
			m_pModeIconBack->Render( iXPos + 178, iYPos + 30, UI_RENDER_MULTIPLY, TFO_BILINEAR );

			if( m_pHumanIcon )
			{
				m_pHumanIcon->SetScale( 0.54f );
				m_pHumanIcon->Render( iXPos + 178, iYPos + 30, UI_RENDER_NORMAL, TFO_BILINEAR );
			}
		}
	}
	else if( m_eWindowTeam == TEAM_BLUE )
	{
		if( m_pGangsiText )
			m_pGangsiText->Render( iXPos + 13 + iResultGap, iYPos + 13 );

		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		if( m_bGangsiWin )
		{
			g_FontMgr.PrintText( iXPos + 13, iYPos + 36, FONT_SIZE_13, STR(2) );
		}
		else
		{
			g_FontMgr.PrintText( iXPos + 13, iYPos + 36, FONT_SIZE_13, STR(3), (int)m_FinalInfoList.size() );
		}

		// 강시 아이콘
		if( m_pModeIconBack )
		{
			m_pModeIconBack->SetScale( 0.54f );
			m_pModeIconBack->Render( iXPos + 178, iYPos + 30, UI_RENDER_MULTIPLY, TFO_BILINEAR );

			ioUIRenderImage *pModeIcon = g_ModeSelectInfo.GetModeIcon( ioPlayMode::GetModeType(), m_eWindowTeam, g_BattleRoomMgr.IsUseExtraOption() );
			if( pModeIcon )
			{
				pModeIcon->SetScale( 0.54f );
				pModeIcon->Render( iXPos + 178, iYPos + 30, UI_RENDER_NORMAL, TFO_BILINEAR );
			}
		}
	}
}

void ResultScoreUserWnd::OnRenderBack( int iXPos, int iYPos )
{
	if( !m_pCurrentDark || !m_pCurrentLight ) return;

	for(int i = 0;i < m_iMaxPlayer;i++)
	{
		if( i%2 == 0 )
			m_pCurrentDark->Render( iXPos, iYPos + i * 21 );
		else
			m_pCurrentLight->Render( iXPos, iYPos + i * 21 );
	}
}

void ResultScoreUserWnd::OnRenderUser( int iXPos, int iYPos, ModeType eType )
{
	if( !m_pCurrentOver || !m_pBlueOver || !m_pRedOver ) return;
	
	int iSize = m_FinalInfoList.size();
	for(int i = 0;i < iSize;i++,iYPos+=21 )
	{
		FinalInfo kInfo;
		if( i < m_iStartIndex )
		{
			int iRollingIndex = (m_iRollingIndex + i) % iSize;
			kInfo = m_FinalInfoList[iRollingIndex];
			
			// 롤링 정보이므로 중요 데이터 삭제
			kInfo.m_bLevelUP = kInfo.m_bOwner = false;
		}
		else
		{
			kInfo = m_FinalInfoList[i];
		}

		if( kInfo.m_bOwner )
		{
			if( kInfo.m_Team == TEAM_BLUE )
				m_pBlueOver->Render( iXPos, iYPos );
			else if( kInfo.m_Team == TEAM_RED )
				m_pRedOver->Render( iXPos, iYPos );
		}
		else if( m_MouseOverIndex == i )
		{	
			m_pCurrentOver->Render( iXPos, iYPos );
		}

		// 레벨 업
		if( kInfo.m_bLevelUP && m_pGradeUpText )
		{
			if( m_GradeUpAni.GetCurState() != GUA_INFO_VIEW )
			{
				if( kInfo.m_bOwner || m_MouseOverIndex == i )
					m_pGradeUpText->SetColor( 255, 255, 255 );	
				else if( kInfo.m_Team == TEAM_BLUE )
					m_pGradeUpText->SetColor( 22, 70, 178 );
				else
					m_pGradeUpText->SetColor( 185, 22, 0 );

				switch( m_GradeUpAni.GetCurState() )
				{
				case GUA_SCALE_UP1:
					{
						m_pGradeUpText->Render( iXPos + 54, iYPos + 16, UI_RENDER_NORMAL_ADD_COLOR );
					}
					break;
				case GUA_SCALE_UP2:
					{
						m_pGradeUpText->Render( iXPos + 54, iYPos + 12, UI_RENDER_NORMAL_ADD_COLOR );
					}
					break;
				case GUA_SCALE_UP3:
					{
						m_pGradeUpText->Render( iXPos + 54, iYPos + 4, UI_RENDER_NORMAL_ADD_COLOR );
					}
					break;
				}
				continue;
			}
		}
		// 계급
		if( m_MouseOverIndex == i || kInfo.m_bOwner )
            g_LevelMgr.RenderGrade( kInfo.m_szName, iXPos + 4, iYPos + 2, kInfo.m_iLevel, TEAM_PRIVATE_1, UI_RENDER_NORMAL_ADD_COLOR, TCT_DEFAULT_WHITE );
		else
			g_LevelMgr.RenderGrade( kInfo.m_szName, iXPos + 4, iYPos + 2, kInfo.m_iLevel, TEAM_PRIVATE_1 );

		// 아이디
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetTextStyle( TS_NORMAL );
		if( m_MouseOverIndex == i || kInfo.m_bOwner )
			g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
		else if( kInfo.m_Team == TEAM_BLUE )
			g_FontMgr.SetTextColor( TCT_DEFAULT_BLUE );
		else
			g_FontMgr.SetTextColor( TCT_DEFAULT_RED );
		g_FontMgr.PrintTextWidthCut( iXPos + 23, iYPos + 4, FONT_SIZE_12, 76.0f, kInfo.m_szName.c_str() );
		int iIDWidth = g_FontMgr.GetTextWidthCutSize( kInfo.m_szName.c_str(), TS_NORMAL, FONT_SIZE_12, 76.0f );

		// 길드마크
		int iGuildMarkWidth = 0;
		DWORD dwGuildIndex, dwGuildMark;
		g_UserInfoMgr.GetGuildInfo( kInfo.m_szName, dwGuildIndex, dwGuildMark );
		g_GuildMarkMgr.RenderSmallMark( dwGuildIndex, dwGuildMark, iXPos + 23 + iIDWidth + 3, iYPos + 4 );	
		if( dwGuildIndex > 0 )
			iGuildMarkWidth = 16;
        
		// (PC방,길드,친구,연승)
		if( kInfo.m_bPCRoomBonus )
		{
			if( kInfo.m_Team == TEAM_BLUE && m_pPCRoomBlue )
				m_pPCRoomBlue->Render( iXPos + 23 + iIDWidth + iGuildMarkWidth + 3, iYPos + 3 );
			else if( kInfo.m_Team == TEAM_RED && m_pPCRoomRed )
				m_pPCRoomRed->Render( iXPos + 23 + iIDWidth + iGuildMarkWidth + 3, iYPos + 3 );
		}
		else if( kInfo.m_bGuildBonus )
		{
			if( kInfo.m_Team == TEAM_BLUE && m_pGuildBlue )
				m_pGuildBlue->Render( iXPos + 23 + iIDWidth + iGuildMarkWidth + 3, iYPos + 3 );
			else if( kInfo.m_Team == TEAM_RED && m_pGuildRed )
				m_pGuildRed->Render( iXPos + 23 + iIDWidth + iGuildMarkWidth + 3, iYPos + 3 );
		}
		else if( kInfo.m_bFriendBonus )
		{
			if( kInfo.m_Team == TEAM_BLUE && m_pFriendBlue )
				m_pFriendBlue->Render( iXPos + 23 + iIDWidth + iGuildMarkWidth + 3, iYPos + 3 );
			else if( kInfo.m_Team == TEAM_RED && m_pFriendRed )
				m_pFriendRed->Render( iXPos + 23 + iIDWidth + iGuildMarkWidth + 3, iYPos + 3 );
		}
		else if( kInfo.m_iVictories > 1 )
		{
			if( kInfo.m_iVictories < 10 )
			{
				if( kInfo.m_Team == TEAM_BLUE && m_pVictoriesBlue1 )
				{
					m_pVictoriesBlue1->Render( iXPos + 23 + iIDWidth + iGuildMarkWidth + 3, iYPos + 3 );
					if( m_pVictoriesNumBlue )
						m_pVictoriesNumBlue->RenderNum( iXPos + 27 + iIDWidth + iGuildMarkWidth + 3, iYPos + 7, kInfo.m_iVictories );
				}
				else if( kInfo.m_Team == TEAM_RED && m_pVictoriesRed1 )
				{
					m_pVictoriesRed1->Render( iXPos + 23 + iIDWidth + iGuildMarkWidth + 3, iYPos + 3 );
					if( m_pVictoriesNumRed )
						m_pVictoriesNumRed->RenderNum( iXPos + 27 + iIDWidth + iGuildMarkWidth + 3, iYPos + 7, kInfo.m_iVictories );
				}
			}
			else
			{
				if( kInfo.m_Team == TEAM_BLUE && m_pVictoriesBlue2 )
				{	
					m_pVictoriesBlue2->Render( iXPos + 23 + iIDWidth + iGuildMarkWidth + 3, iYPos + 3 );
					if( m_pVictoriesNumBlue )
						m_pVictoriesNumBlue->RenderNum( iXPos + 27 + iIDWidth + iGuildMarkWidth + 3, iYPos + 7, min( 99, kInfo.m_iVictories ) );
				} 
				else if( kInfo.m_Team == TEAM_RED && m_pVictoriesRed2 )
				{
					m_pVictoriesRed2->Render( iXPos + 23 + iIDWidth + iGuildMarkWidth + 3, iYPos + 3 );
					if( m_pVictoriesNumRed )
						m_pVictoriesNumRed->RenderNum( iXPos + 27 + iIDWidth + iGuildMarkWidth + 3, iYPos + 7, min( 99, kInfo.m_iVictories ) );
				}
			}
		}

		// (기여도)
		g_FontMgr.SetAlignType( TAT_RIGHT );
		switch( eType )
		{
		case MT_SHUFFLE_BONUS:
			{
				g_FontMgr.PrintText( iXPos + 191, iYPos + 4, FONT_SIZE_12, "%d개", kInfo.m_iStarCount );
			}
			break;
		case MT_RAID:
			{
				g_FontMgr.PrintText( iXPos + 191, iYPos + 4, FONT_SIZE_12, "%d개", kInfo.m_iHunterCoinCnt );
			}
			break;
		default:
			{
				g_FontMgr.PrintText( iXPos + 191, iYPos + 4, FONT_SIZE_12, STR(2), kInfo.m_iContributePer );
			}
			break;
		}		
	}
}

void ResultScoreUserWnd::OnRenderCampBonus( int iXPos, int iYPos )
{
	if( m_iStartIndex >= 0 ) return;
	if( !g_LadderTeamMgr.IsLadderTeam() ) return;
	if( !m_pCampBonusFrm || !m_pCampBlueMark || !m_pCampRedMark || !m_pCampMarkBack ) return;

	m_pCampBonusFrm->Render( iXPos, iYPos );

	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	
	if( m_eWindowTeam == TEAM_BLUE )
	{
		const LadderTeamInfo& LadderInfo = g_LadderTeamMgr.GetLadderBlueTeamInfo();

		m_pCampMarkBack->Render( iXPos + 51, iYPos + 56, UI_RENDER_MULTIPLY );

		if ( LadderInfo.m_iCampType == CAMP_BLUE )
		{
			m_pCampBlueMark->Render( iXPos + 51, iYPos + 56 );

			g_FontMgr.SetTextColor( TCT_DEFAULT_BLUE );
			g_FontMgr.PrintText( iXPos + 51, iYPos + 100, FONT_SIZE_13, m_szBlueTeamName.c_str() );
			g_FontMgr.PrintText( iXPos + 51, iYPos + 116, FONT_SIZE_13, STR(2) );
			g_FontMgr.PrintText( iXPos + 51, iYPos + 132, FONT_SIZE_17, STR(3), GetResultLadderPoint() );

			char szBonus[MAX_PATH] = "";
			SafeSprintf( szBonus, sizeof( szBonus ), STR(4), m_fBlueGuildTeamBonus );
			g_FontMgr.PrintText( iXPos + 51, iYPos + 154, FONT_SIZE_13, szBonus );
		}
		else
		{
			m_pCampRedMark->Render( iXPos + 51, iYPos + 56 );

			g_FontMgr.SetTextColor( TCT_DEFAULT_RED );
			g_FontMgr.PrintText( iXPos + 51, iYPos + 100, FONT_SIZE_13, m_szRedTeamName.c_str() );
			g_FontMgr.PrintText( iXPos + 51, iYPos + 116, FONT_SIZE_13, STR(6) );
			g_FontMgr.PrintText( iXPos + 51, iYPos + 132, FONT_SIZE_17, STR(7), GetResultLadderPoint() );

			char szBonus[MAX_PATH] = "";
			SafeSprintf( szBonus, sizeof( szBonus ), STR(8), m_fRedGuildTeamBonus );
			g_FontMgr.PrintText( iXPos + 51, iYPos + 154, FONT_SIZE_13, szBonus );
		}
		
	}
	else if( m_eWindowTeam == TEAM_RED )
	{
		const LadderTeamInfo& LadderInfo = g_LadderTeamMgr.GetLadderRedTeamInfo();

		m_pCampMarkBack->Render( iXPos + 51, iYPos + 56, UI_RENDER_MULTIPLY );

		if ( LadderInfo.m_iCampType == CAMP_BLUE )
		{
			m_pCampBlueMark->Render( iXPos + 51, iYPos + 56 );

			g_FontMgr.SetTextColor( TCT_DEFAULT_BLUE );
			g_FontMgr.PrintText( iXPos + 51, iYPos + 100, FONT_SIZE_13, m_szBlueTeamName.c_str() );
			g_FontMgr.PrintText( iXPos + 51, iYPos + 116, FONT_SIZE_13, STR(2) );
			g_FontMgr.PrintText( iXPos + 51, iYPos + 132, FONT_SIZE_17, STR(3), GetResultLadderPoint() );

			char szBonus[MAX_PATH] = "";
			SafeSprintf( szBonus, sizeof( szBonus ), STR(4), m_fBlueGuildTeamBonus );
			g_FontMgr.PrintText( iXPos + 51, iYPos + 154, FONT_SIZE_13, szBonus );
		}
		else
		{
			m_pCampRedMark->Render( iXPos + 51, iYPos + 56 );

			g_FontMgr.SetTextColor( TCT_DEFAULT_RED );
			g_FontMgr.PrintText( iXPos + 51, iYPos + 100, FONT_SIZE_13, m_szRedTeamName.c_str() );
			g_FontMgr.PrintText( iXPos + 51, iYPos + 116, FONT_SIZE_13, STR(6) );
			g_FontMgr.PrintText( iXPos + 51, iYPos + 132, FONT_SIZE_17, STR(7), GetResultLadderPoint() );

			char szBonus[MAX_PATH] = "";
			SafeSprintf( szBonus, sizeof( szBonus ), STR(8), m_fRedGuildTeamBonus );
			g_FontMgr.PrintText( iXPos + 51, iYPos + 154, FONT_SIZE_13, szBonus );
		}
	}
}

void ResultScoreUserWnd::OnRenderShuffleResult( int iXPos, int iYPos )
{
	if( m_pShuffleFrm )
		m_pShuffleFrm->Render( iXPos, iYPos, UI_RENDER_NORMAL );

	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 57, iYPos + 15, FONT_SIZE_12, "내가 획득한");
	g_FontMgr.PrintText( iXPos + 57, iYPos + 31, FONT_SIZE_12, "스타 코인 갯수");

	if( m_pShuffleResultIconBack )
		m_pShuffleResultIconBack->Render( iXPos + 57, iYPos + 76, UI_RENDER_MULTIPLY, TFO_BILINEAR );

	if( m_pShuffleResultIcon )
		m_pShuffleResultIcon->Render( iXPos + 57, iYPos + 76, UI_RENDER_NORMAL, TFO_BILINEAR );
	
	int Seq = Help::GetNumberLength( m_iStarCountByCalcBonus ) - 1;
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetTextColor( TCT_DEFAULT_BLUE );
	g_FontMgr.PrintText( iXPos + 44 - max(0, Seq) * 6, iYPos + 112, FONT_SIZE_20, "%d", m_iStarCountByCalcBonus );
	
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 57 + max(0, Seq) * 6, iYPos + 118, FONT_SIZE_12, "개" );
	
	if( m_pShuffleLine )
		m_pShuffleLine->Render( iXPos + 12, iYPos + 134, UI_RENDER_NORMAL, TFO_BILINEAR );

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_BLUE );
	kPrinter.AddTextPiece( FONT_SIZE_11, "%d", m_iStarCount );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_11, "개" );

	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_11, " × " );

	kPrinter.SetTextColor( TCT_DEFAULT_RED );
	kPrinter.AddTextPiece( FONT_SIZE_11, "%.1f", m_fBonusStarPercent * 100 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_11, "％" );
	kPrinter.PrintFullText( iXPos + 57, iYPos + 141, TAT_CENTER );
	kPrinter.ClearList();

	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
	g_FontMgr.PrintText( iXPos + 57, iYPos + 157, FONT_SIZE_12, "(획득갯수 × 보너스)");
}
//////////////////////////////////////////////////////////////////////////
ResultRaidScoreUserBtn::ResultRaidScoreUserBtn() : m_pPlayStage( NULL )
{
	m_pSlotNormal	= NULL;
	m_pSlotOver		= NULL;

	m_pSlotGra		= NULL;
	m_pSlotGraOver	= NULL;
	m_pEmptySlot	= NULL;
	m_pSlotLineDark	= NULL;
	m_pSlotLineLight= NULL;

	int i = 0;
	for(i = 0;i < MAX_MVP_NUMBER;i++)
		m_pSlotMVP[i] = NULL;
		
	for(i = 0;i < MAX_RANK;i++)
		m_pSlotRank[i] = NULL;

	m_pUI3DRender    = NULL;
	m_aMyChar	     = NULL;

	for(i = 0;i < MAX_RANK_ELEMENT;i++)
		m_iRankElement[i] = -1;

	m_fCurrentRankRate = FLOAT1;
	m_iCurrentRank = 0;
	m_bRaidClear = false;
	m_dwCurrentTime = 0;
	m_dwState       = STATE_NONE;
	m_fCurrentMVPRate = 0.0f;

	m_dwRollingTime = 0;
	m_dwRollingStopTime = 0;
	m_dwMVPTime = 0;
}

ResultRaidScoreUserBtn::~ResultRaidScoreUserBtn()
{
	SAFEDELETE( m_pSlotNormal );
	SAFEDELETE( m_pSlotOver );

	SAFEDELETE( m_pSlotGra );
	SAFEDELETE( m_pSlotGraOver );
	SAFEDELETE( m_pEmptySlot );
	SAFEDELETE( m_pSlotLineDark );
	SAFEDELETE( m_pSlotLineLight );

	int i = 0;
	for(i = 0;i < MAX_MVP_NUMBER;i++)
		SAFEDELETE( m_pSlotMVP[i] );
	for(i = 0;i < MAX_RANK;i++)
		SAFEDELETE( m_pSlotRank[i] );

	SAFEDELETE( m_aMyChar );
	SAFEDELETE( m_pUI3DRender );
}

void ResultRaidScoreUserBtn::iwm_create()
{
	SAFEDELETE( m_pUI3DRender );

	m_pUI3DRender = new ioUI3DRender;
	m_pUI3DRender->Initialize( "config/sp2_stage_charselect_map.ini", "ladder_cam" );
}

void ResultRaidScoreUserBtn::SetStartScore( bool bClear )
{
	m_bRaidClear = bClear;

	if( m_FinalUser.m_szName.IsEmpty() )
	{
		// 바로 넘김
		if( GetParent() )
			GetParent()->iwm_command( this, STATE_RANK_ROLLING_STOP, 0 );
	}
	else
	{
		m_iCurrentRank  = rand() % MAX_RANK;
		m_dwCurrentTime = FRAMEGETTIME();	
		m_dwState       = STATE_RANK_ROLLING;
	}
}

void ResultRaidScoreUserBtn::SetMVP()
{
	if( !m_bRaidClear ) return;
	if( m_FinalUser.m_szName.IsEmpty() ) return;
	if( m_FinalUser.m_iRank != 1 ) return;

	m_dwCurrentTime   = FRAMEGETTIME();	
	m_dwState         = STATE_MVP;
	m_fCurrentMVPRate = 2.50f;

	if( !m_szMVPSnd.IsEmpty() )
	{
		g_SoundMgr.PlaySound( m_szMVPSnd );
	}
}

int ResultRaidScoreUserBtn::GetRaidRankChange( int iMaxPlayer, int iRank )
{
	enum { S_RANK = 0, A_RANK = 1, B_RANK = 2, C_RANK = 3, };

	if( iMaxPlayer <= 1 )
	{
		// 1인 플레이는 모두 C
		return C_RANK;
	}
	else if( iMaxPlayer <= 2 )
	{
		// 2인 플레이는 A / C
		if( iRank == 0 )
			return A_RANK;
		else 
			return C_RANK;
	}
	else if( iMaxPlayer <= 3 )
	{
		// 3인 플레이는 A / B / C
		if( iRank == 0 )
			return A_RANK;
		else if( iRank == 1 )
			return B_RANK;
		else 
			return C_RANK;
	}
	else if( iMaxPlayer <= 4 )
	{
		// 4인 플레이는 S / A / B / C
		if( COMPARE( iRank, S_RANK, C_RANK + 1 ) )
			return iRank;
		else 
			return C_RANK;
	}
	return C_RANK;
}

void ResultRaidScoreUserBtn::SetPlayStage( ioPlayStage *pPlayStage )
{
	m_pPlayStage = pPlayStage;

	if( m_pPlayStage && !m_FinalUser.m_szName.IsEmpty() )
	{
		m_pPlayStage->ContributeRank(m_FinalUser.m_szName, m_iRankElement[0], m_iRankElement[1], m_iRankElement[2], m_iRankElement[3] );
	}
}

void ResultRaidScoreUserBtn::SetFinalInfo( int iMaxPlayer, const FinalInfo &rkInfo )
{
	m_FinalUser = rkInfo;

	CreateCharacter();
}

void ResultRaidScoreUserBtn::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "SlotNormal" )
	{
		SAFEDELETE( m_pSlotNormal );
		m_pSlotNormal = pFrame;
	}
	else if( szType == "SlotOver" )
	{
		SAFEDELETE( m_pSlotOver );
		m_pSlotOver = pFrame;
	}
	else
	{
		ioButton::AddRenderFrame( szType, pFrame );
	}
}

void ResultRaidScoreUserBtn::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "SlotGra" )
	{
		SAFEDELETE( m_pSlotGra );
		m_pSlotGra = pImage;
	}
	else if( szType == "SlotGraOver" )
	{
		SAFEDELETE( m_pSlotGraOver );
		m_pSlotGraOver = pImage;
	}
	else if( szType == "EmptySlot" )
	{
		SAFEDELETE( m_pEmptySlot );
		m_pEmptySlot = pImage;
	}
	else if( szType == "SlotMVP1" )
	{
		SAFEDELETE( m_pSlotMVP[0] );
		m_pSlotMVP[0] = pImage;
	}
	else if( szType == "SlotMVP2" )
	{
		SAFEDELETE( m_pSlotMVP[1] );
		m_pSlotMVP[1] = pImage;
	}
	else if( szType == "SlotMVP3" )
	{
		SAFEDELETE( m_pSlotMVP[2] );
		m_pSlotMVP[2] = pImage;
	}
	else if( szType == "SlotMVP4" )
	{
		SAFEDELETE( m_pSlotMVP[3] );
		m_pSlotMVP[3] = pImage;
	}
	else if( szType == "SlotMVP5" )
	{
		SAFEDELETE( m_pSlotMVP[4] );
		m_pSlotMVP[4] = pImage;
	}
	else if( szType == "SlotLineDark" )
	{
		SAFEDELETE( m_pSlotLineDark );
		m_pSlotLineDark = pImage;
	}
	else if( szType == "SlotLineLight" )
	{
		SAFEDELETE( m_pSlotLineLight );
		m_pSlotLineLight = pImage;
	}
	else if( szType == "SlotRankS" )
	{
		SAFEDELETE( m_pSlotRank[0] );
		m_pSlotRank[0] = pImage;
	}
	else if( szType == "SlotRankA" )
	{
		SAFEDELETE( m_pSlotRank[1] );
		m_pSlotRank[1] = pImage;
	}
	else if( szType == "SlotRankB" )
	{
		SAFEDELETE( m_pSlotRank[2] );
		m_pSlotRank[2] = pImage;
	}
	else if( szType == "SlotRankC" )
	{
		SAFEDELETE( m_pSlotRank[3] );
		m_pSlotRank[3] = pImage;
	}
	else
	{
		ioButton::AddRenderImage( szType, pImage );
	}
}

void ResultRaidScoreUserBtn::CreateCharacter()
{
	SAFEDELETE( m_aMyChar );

	if( m_FinalUser.m_szName.IsEmpty() )
		return;

 	CHARACTER kCharInfo = g_GlobalSoldierInfo.GetSoldierData( m_FinalUser.m_szName, m_FinalUser.m_dwPvECharIndex );
	if( kCharInfo.m_class_type == 0 )
	{
		kCharInfo.m_class_type = m_FinalUser.m_iPvEClassType;
	}

	ioCreateChar *pChar = m_pUI3DRender->CreateChar( kCharInfo, "ResultChar" );
	if( !pChar )	return;

	pChar->SetPosition( 0.0f, -FLOAT90, 900.0f );
	pChar->SetYaw( FLOAT10 );
	m_pUI3DRender->SetLookAt( 0.0f, 0.0f, 900.0f );

	float fAspect = (float)VIEW_WIDTH/(float)VIEW_HEIGHT;
	m_pUI3DRender->SetProjection( FLOAT1, 600.0f, 45.0f, fAspect );

	m_aMyChar = pChar;

	// 코스튬 관련 (??)
	int TempCostume[MAX_INVENTORY];
	for ( int i=0; i<MAX_INVENTORY; i++ )
	{
		TempCostume[i] = kCharInfo.m_EquipCostume[i].m_CostumeCode;
		
		// 코스튬 관련 (자신만 적용)
		if ( !Setting::IsCostumeShowChange() && !g_CostumeInfoMgr.IsEnableCostumeMode() && m_FinalUser.m_szName != g_MyInfo.GetPublicID() )
			TempCostume[i] = 0;
	}

	const ioSetItemInfo *pSetInfo = g_SetItemInfoMgr.GetSetInfoByIdx( kCharInfo.m_class_type - 1 );
	if( pSetInfo )
	{
		ConstItemVector rkItemList = pSetInfo->GetSetItemList();
		int iMaxItemCnt = min( rkItemList.size(), MAX_INVENTORY );
		for(int i = 0;i < iMaxItemCnt;i++)
		{			
			DWORD dwItemCode = g_GlobalSoldierInfo.GetSoldierItemCode( m_FinalUser.m_szName, m_FinalUser.m_dwPvECharIndex, i );
			DWORD dwCostumeMaleCustom = kCharInfo.m_EquipCostume[i].m_Costume_Male_Custom;
			DWORD dwCostumeFemaleCustom = kCharInfo.m_EquipCostume[i].m_Costume_Female_Custom;

			if( dwItemCode == 0 )
			{
				if ( kCharInfo.m_EquipCostume[i].m_CostumeCode <= 0 )
				{
					dwCostumeMaleCustom = 0;
					dwCostumeFemaleCustom = 0;
				}
				pChar->EquipItem( rkItemList[i]->GetItemCode(), dwCostumeMaleCustom, dwCostumeFemaleCustom, TempCostume[i] );
			}
			else
			{
				if ( kCharInfo.m_EquipCostume[i].m_CostumeCode <= 0 )
				{
					dwCostumeMaleCustom = g_GlobalSoldierInfo.GetSoldierItemMaleCustom( m_FinalUser.m_szName, m_FinalUser.m_dwPvECharIndex, i );
					dwCostumeFemaleCustom = g_GlobalSoldierInfo.GetSoldierItemFemaleCustom( m_FinalUser.m_szName, m_FinalUser.m_dwPvECharIndex, i );
				}
				pChar->EquipItem( dwItemCode, dwCostumeMaleCustom, dwCostumeFemaleCustom, TempCostume[i] );
			}
		}
	}

	if( !m_aMyChar->SetWeaponShopMotion() )
	{
		ioHashString szUIAnimation = m_pUI3DRender->GetModelAnimation( 0, m_aMyChar->IsMale() );
		if( !szUIAnimation.IsEmpty() )
		{
			m_aMyChar->SetLoopAni( szUIAnimation );
		}
	}
}

int ResultRaidScoreUserBtn::GetRankCompare( int iRank )
{
	if( iRank <= -1 )
		iRank = MAX_RANK_ELEMENT - 1;
	return min( iRank, MAX_RANK_ELEMENT - 1 );
}

void ResultRaidScoreUserBtn::ParseExtraInfo( ioXMLElement &xElement )
{
	ioButton::ParseExtraInfo( xElement );
	
	m_dwRollingTime = xElement.GetIntAttribute_e( "RollingTime" );
	m_dwRollingStopTime = xElement.GetIntAttribute_e( "RollingStopTime" );
	m_dwMVPTime = xElement.GetIntAttribute_e( "MVPTime" );
	m_szRollingStopSnd = xElement.GetStringAttribute_e( "RollingStopSnd" );
	m_szMVPSnd = xElement.GetStringAttribute_e( "MVPSnd" );
}

void ResultRaidScoreUserBtn::ProcessRolling()
{
	if( m_dwCurrentTime == 0 ) return;
	
	DWORD dwGapTime = FRAMEGETTIME() - m_dwCurrentTime;

	switch( m_dwState )
	{
	case STATE_RANK_ROLLING:
		if( dwGapTime > m_dwRollingTime )
		{
			m_fCurrentRankRate = 2.50f;
			m_iCurrentRank     = m_FinalUser.m_iRank - 1;
			m_dwCurrentTime    = FRAMEGETTIME();
			m_dwState          = STATE_RANK_ROLLING_STOP;

			if( !m_szRollingStopSnd.IsEmpty() )
			{
				g_SoundMgr.PlaySound( m_szRollingStopSnd );
			}
		}
		else
		{
			m_iCurrentRank     = ( GetID() + ( dwGapTime / 75 ) ) % MAX_RANK;
			m_fCurrentRankRate = FLOAT1;
		}
		break;
	case STATE_RANK_ROLLING_STOP:
		if( dwGapTime > m_dwRollingStopTime )
		{
			m_fCurrentRankRate = FLOAT1;
			m_iCurrentRank     = m_FinalUser.m_iRank - 1;
			m_dwCurrentTime    = 0;
			m_dwState          = STATE_NONE;
			
			if( GetParent() )
				GetParent()->iwm_command( this, STATE_RANK_ROLLING_STOP, 0 );
		}
		else
		{
			float fRate = ( (float)dwGapTime / m_dwRollingStopTime ) * 1.5f;
			m_fCurrentRankRate = 2.50f - fRate;
			if( m_fCurrentRankRate <= FLOAT1 )
				m_fCurrentRankRate = FLOAT1;
		}
		break;
	case STATE_MVP:
		if( dwGapTime <= m_dwMVPTime )
		{
			float fRate = ( (float)dwGapTime / m_dwMVPTime ) * 1.5f;
			m_fCurrentMVPRate = 2.50f - fRate;
			if( m_fCurrentMVPRate <= FLOAT1 )
				m_fCurrentMVPRate = FLOAT1;
		}
		else
		{
			m_fCurrentMVPRate = FLOAT1;
		}
		break;
	}
}

void ResultRaidScoreUserBtn::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;
	if( m_FinalUser.m_szName.IsEmpty() ) return;

	ProcessRolling();
	if( m_bOver && !m_bClicked )
	{
		ioLSMouse *pMouse = g_App.GetMouse();
		if( pMouse && pMouse->IsMouseShowing() )
		{
			pMouse->SetToolTipID( m_FinalUser.m_szName, m_FinalUser.m_iPvEClassType );
		}
	}	

	if( m_aMyChar )
		m_aMyChar->Update( fTimePerSec );
}

void ResultRaidScoreUserBtn::OnRender()
{
	ioButton::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	OnRenderSlot( iXPos, iYPos );
	if( !m_FinalUser.m_szName.IsEmpty() )
	{
		g_LevelMgr.RenderGrade( m_FinalUser.m_szName, iXPos + 5, iYPos + 146, m_FinalUser.m_iLevel, TEAM_PRIVATE_2 );

		g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetBkColor( 24, 66, 120 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
		g_FontMgr.PrintTextWidthCut( iXPos + 23, iYPos + 147, FONT_SIZE_12, 75.0f, m_FinalUser.m_szName.c_str() );
		
		OnRenderRank( iXPos, iYPos );

		if( m_dwState == STATE_MVP )
		{
			if( m_FinalUser.m_iRank == 1 )
			{
				int iMVPCard = Help::GetRaidModeMVPTreasureCard() - 1;
				if( COMPARE( iMVPCard, 0, MAX_MVP_NUMBER ) )
				{
					if( m_pSlotMVP[iMVPCard] )
					{
						m_pSlotMVP[iMVPCard]->SetScale( m_fCurrentMVPRate );
						m_pSlotMVP[iMVPCard]->Render( iXPos + 38, iYPos + 17 );
					}
				}
			}
		}
	}
}

void ResultRaidScoreUserBtn::OnRenderSoldier( int iXPos, int iYPos )
{
	if( m_FinalUser.m_szName.IsEmpty() ) return;
	if( !m_pUI3DRender || !m_aMyChar ) return;

	D3DRECT rcD3D;
	rcD3D.x1 = iXPos;
	rcD3D.y1 = iYPos;
	rcD3D.x2 = rcD3D.x1 + VIEW_WIDTH;
	rcD3D.y2 = rcD3D.y1 + VIEW_HEIGHT;
	m_pUI3DRender->RenderViewPort( &rcD3D, true );
}

void ResultRaidScoreUserBtn::OnRenderSlot( int iXPos, int iYPos )
{
	if( !m_pSlotNormal || !m_pSlotOver || !m_pSlotGra || !m_pSlotGraOver ) return;

	if( !m_FinalUser.m_szName.IsEmpty() && ( m_bOver || m_bClicked || g_MyInfo.GetPublicID() == m_FinalUser.m_szName ) )
	{
		m_pSlotOver->Render( iXPos, iYPos );
		
		OnRenderSoldier( iXPos + 3, iYPos + 3 );

		m_pSlotGraOver->SetSize( 98, 55 );
		m_pSlotGraOver->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
		m_pSlotGraOver->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
		m_pSlotGraOver->Render( iXPos + 3, iYPos + 168 );
	}
	else
	{
		m_pSlotNormal->Render( iXPos, iYPos );

		OnRenderSoldier( iXPos + 3, iYPos + 3 );

		m_pSlotGra->SetSize( 98, 55 );
		m_pSlotGra->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
		m_pSlotGra->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
		m_pSlotGra->Render( iXPos + 3, iYPos + 168 );

		if( m_FinalUser.m_szName.IsEmpty() )
		{
			if( m_pEmptySlot )
			{
				m_pEmptySlot->Render( ( iXPos + ( GetWidth() / 2 ) ) - ( m_pEmptySlot->GetWidth() / 2 ), 
									  ( iYPos + ( GetHeight() / 2 ) ) - ( m_pEmptySlot->GetHeight() / 2 ), UI_RENDER_MULTIPLY );
			}
		}
	}

	if( !m_FinalUser.m_szName.IsEmpty() && m_pSlotLineDark && m_pSlotLineLight )
	{
		m_pSlotLineDark->SetSize( 98, 19 );
		m_pSlotLineDark->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
		m_pSlotLineDark->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
		m_pSlotLineDark->Render( iXPos + 3, iYPos + 164, UI_RENDER_MULTIPLY );

		m_pSlotLineLight->SetSize( 98, 19 );
		m_pSlotLineLight->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
		m_pSlotLineLight->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
		m_pSlotLineLight->Render( iXPos + 3, iYPos + 183, UI_RENDER_MULTIPLY );

		m_pSlotLineDark->Render( iXPos + 3, iYPos + 202, UI_RENDER_MULTIPLY );
	}
}

void ResultRaidScoreUserBtn::OnRenderRank( int iXPos, int iYPos )
{
	const char cRank[MAX_RANK_ELEMENT][MAX_PATH] = { "S", "A", "B", "C", };
	char szRank[MAX_PATH] = "";
	// 타격
	int iRank = GetRankCompare( m_iRankElement[0] );
	sprintf( szRank, "공격 %s", cRank[iRank]  );
	g_FontMgr.PrintText( iXPos + 7, iYPos + 166, FONT_SIZE_12, szRank );

	// 데미지
	iRank = GetRankCompare( m_iRankElement[1] );
	sprintf( szRank, "방어 %s", cRank[iRank]  );
	g_FontMgr.PrintText( iXPos + 7, iYPos + 185, FONT_SIZE_12, szRank );

	// 피격
	iRank = GetRankCompare( m_iRankElement[2] );
	sprintf( szRank, "타임 %s", cRank[iRank]  );
	g_FontMgr.PrintText( iXPos + 7, iYPos + 204, FONT_SIZE_12, szRank );

	if( COMPARE( m_iCurrentRank, 0, MAX_RANK ) )
	{
		if( m_pSlotRank[m_iCurrentRank] )
		{
			m_pSlotRank[m_iCurrentRank]->SetScale( m_fCurrentRankRate );
			m_pSlotRank[m_iCurrentRank]->Render( iXPos + 80, iYPos + 192 );
		}
	}
}
//////////////////////////////////////////////////////////////////////////
ResultRaidScoreUserWnd::ResultRaidScoreUserWnd()
{
	m_pPlayStage = NULL;
	m_bRaidClear = false;
	m_dwCurrentTime = 0;
	m_dwState    = STATE_NONE;
	m_dwMVPDelay = 0;
	m_dwEndDelay = 0;
}

ResultRaidScoreUserWnd::~ResultRaidScoreUserWnd()
{
	if( !m_szRollingSound.IsEmpty() )
		g_SoundMgr.StopSound( m_szRollingSound, 0 );
}

void ResultRaidScoreUserWnd::SetStartScore()
{
	if( !m_szRollingSound.IsEmpty() )
		g_SoundMgr.PlaySound( m_szRollingSound, DSBVOLUME_MAX, PSM_THREAD | PSM_LOOP );

	for(int i = ID_USER_SLOT1;i < ID_USER_SLOT4+1;i++)
	{
		ResultRaidScoreUserBtn *pResultRaidScoreUserBtn = dynamic_cast< ResultRaidScoreUserBtn * >(FindChildWnd( i ));
		if( pResultRaidScoreUserBtn )
			pResultRaidScoreUserBtn->SetStartScore( m_bRaidClear );
	}
}

void ResultRaidScoreUserWnd::SetRaidScore( bool bClear )
{
	m_bRaidClear = bClear;
}

void ResultRaidScoreUserWnd::SetDungeonsScore( int iResultFloor, int iMaxFloor )
{
	if( iResultFloor >= iMaxFloor )
		m_bRaidClear = true;
	else
		m_bRaidClear = false;
}

void ResultRaidScoreUserWnd::SetPlayStage( ioPlayStage *pPlayStage )
{
	m_pPlayStage = pPlayStage;

	for(int i = ID_USER_SLOT1;i < ID_USER_SLOT4+1;i++)
	{
		ResultRaidScoreUserBtn *pResultRaidScoreUserBtn = dynamic_cast< ResultRaidScoreUserBtn * >(FindChildWnd( i ));
		if( pResultRaidScoreUserBtn )
			pResultRaidScoreUserBtn->SetPlayStage( pPlayStage );
	}
}

void ResultRaidScoreUserWnd::AddFinalInfo( int iMaxPlayer, const FinalInfo &rkInfo )
{
	for(int i = ID_USER_SLOT1;i < ID_USER_SLOT4+1;i++)
	{
		ResultRaidScoreUserBtn *pResultRaidScoreUserBtn = dynamic_cast< ResultRaidScoreUserBtn * >(FindChildWnd( i ));
		if( pResultRaidScoreUserBtn == NULL ) continue;
		if( !pResultRaidScoreUserBtn->IsEmptySlot() ) continue;

		pResultRaidScoreUserBtn->SetFinalInfo( iMaxPlayer, rkInfo );
		break;
	}
}

void ResultRaidScoreUserWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_USER_SLOT1:
	case ID_USER_SLOT2:
	case ID_USER_SLOT3:
	case ID_USER_SLOT4:
		if( cmd == IOBN_BTNUP )
		{
			ResultRaidScoreUserBtn *pUserBtn = dynamic_cast< ResultRaidScoreUserBtn * >(pWnd);
			if( !pUserBtn->IsEmptySlot() )
			{
				g_App.OpenMannerWnd( pUserBtn->GetUserName().c_str(), true );
			}
		}
		else if( cmd == ResultRaidScoreUserBtn::STATE_RANK_ROLLING_STOP )
		{
			bool bAllStop = true;
			for(int i = ID_USER_SLOT1;i < ID_USER_SLOT4+1;i++)
			{
				ResultRaidScoreUserBtn *pResultRaidScoreUserBtn = dynamic_cast< ResultRaidScoreUserBtn * >(FindChildWnd( i ));
				if( pResultRaidScoreUserBtn == NULL ) continue;
				if( pResultRaidScoreUserBtn->IsEmptySlot() ) continue;

				if( pResultRaidScoreUserBtn->GetState() == ResultRaidScoreUserBtn::STATE_RANK_ROLLING )
					bAllStop = false;
			}

			if( bAllStop )
			{
				// 모든 롤링이 끝났다.
				if( m_bRaidClear )
				{
					// MVP 표시
					m_dwCurrentTime = FRAMEGETTIME();
					m_dwState       = STATE_MVP;
				}
				else
				{
					// MVP 없음
					m_dwCurrentTime = FRAMEGETTIME();
					m_dwState       = STATE_DELAY;
				}

				if( !m_szRollingSound.IsEmpty() )
					g_SoundMgr.StopSound( m_szRollingSound, 0 );
			}
		}
		break;
	}
}

void ResultRaidScoreUserWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_dwMVPDelay = xElement.GetIntAttribute_e( "MVPDelay" );
	m_dwEndDelay = xElement.GetIntAttribute_e( "EndDelay" );
	m_szRollingSound = xElement.GetStringAttribute_e( "RollingSound" );
}

void ResultRaidScoreUserWnd::ProcessState()
{	
	if( m_dwCurrentTime == 0 ) return;

	DWORD dwGapTime = FRAMEGETTIME() - m_dwCurrentTime;

	switch( m_dwState )
	{
	case STATE_MVP:
		if( dwGapTime > m_dwMVPDelay )
		{
			for(int i = ID_USER_SLOT1;i < ID_USER_SLOT4+1;i++)
			{
				ResultRaidScoreUserBtn *pResultRaidScoreUserBtn = dynamic_cast< ResultRaidScoreUserBtn * >(FindChildWnd( i ));
				if( pResultRaidScoreUserBtn == NULL ) continue;
				if( pResultRaidScoreUserBtn->IsEmptySlot() ) continue;
				
				pResultRaidScoreUserBtn->SetMVP();
			}
			m_dwCurrentTime = FRAMEGETTIME();
			m_dwState       = STATE_DELAY;
		}
		break;
	case STATE_DELAY:
		if( dwGapTime > m_dwEndDelay )
		{
			m_dwCurrentTime = 0;
			m_dwState       = STATE_NONE;

			if( GetParent() )
				GetParent()->iwm_command( this, STATE_DELAY, 0 );
		}
		break;
	}
}

void ResultRaidScoreUserWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	ProcessState();
}
//////////////////////////////////////////////////////////////////////////
TotalResultScoreWnd::TotalResultScoreWnd()
{
	m_pPlayStage = NULL;
}

TotalResultScoreWnd::~TotalResultScoreWnd()
{
}

void TotalResultScoreWnd::SetPlayStage( ioPlayStage *pPlayStage )
{
	m_pPlayStage = pPlayStage;
}

void TotalResultScoreWnd::SetStartScore()
{
	ResultScoreUserWnd *pUserWnd = dynamic_cast<ResultScoreUserWnd *>(FindChildWnd( ID_SINGLE_WND ));
	if( pUserWnd )
		pUserWnd->SetStartScore();

	ResultScoreUserWnd *pWinnerWnd = dynamic_cast<ResultScoreUserWnd *>(FindChildWnd( ID_WINNER_WND ));
	if( pWinnerWnd )
		pWinnerWnd->SetStartScore();

	ResultScoreUserWnd *pLoserWnd = dynamic_cast<ResultScoreUserWnd *>(FindChildWnd( ID_LOSER_WND ));
	if( pLoserWnd )
		pLoserWnd->SetStartScore();	

	ResultRaidScoreUserWnd *pRaidWnd = dynamic_cast<ResultRaidScoreUserWnd *>(FindChildWnd( ID_RAID_WND ));
	if( pRaidWnd )
		pRaidWnd->SetStartScore();
}

void TotalResultScoreWnd::AddFinalInfo( const FinalInfo &rkInfo )
{
	m_FinalInfoList.push_back( rkInfo );
}

void TotalResultScoreWnd::SetGangsiScore()
{
	if( !m_pPlayStage ) return;

	std::sort( m_FinalInfoList.begin(), m_FinalInfoList.end(), FinalInfoSort() );

	int iRedUserCount, iBlueUserConut;
	iRedUserCount = iBlueUserConut = 0;

	int i = 0;
	int iUserSize = m_FinalInfoList.size();
	for(i = 0;i < iUserSize;i++)
	{
		FinalInfo &rkInfo = m_FinalInfoList[i];
		if( rkInfo.m_Team == TEAM_BLUE )
			iBlueUserConut++;
		else if( rkInfo.m_Team == TEAM_RED )
			iRedUserCount++;		
	}

	char szTitle[MAX_PATH] = "";
	if( iRedUserCount == 0 )
		SafeSprintf( szTitle, sizeof( szTitle ), STR(1) );
	else
		SafeSprintf( szTitle, sizeof( szTitle ), STR(2), iRedUserCount );
	SetTitleText( szTitle );

	enum { CUSTOM_X_POS = 10, CUSTOM_Y_POS = 50, CUSTOM_FULL_SIZE = 465, CUSTOM_LOSER_Y_POS = 113, CUSTOM_DEFAULT_H = 68, CUSTOM_USER_SIZE = 21 };
	// 강시 승
	if( iRedUserCount == 0 )  
	{
		ResultScoreUserWnd *pWinWnd = dynamic_cast<ResultScoreUserWnd *>(FindChildWnd( ID_WINNER_WND ));
		if( pWinWnd )
		{
			int iUserSize = m_FinalInfoList.size();
			for(int i = 0;i < iUserSize;i++)
			{
				FinalInfo &rkInfo = m_FinalInfoList[i];
				pWinWnd->AddFinalInfo( rkInfo );
			}

			pWinWnd->SetCustomWnd( MAX_PLAYER, CUSTOM_X_POS, CUSTOM_Y_POS, pWinWnd->GetWidth(), CUSTOM_FULL_SIZE );
			pWinWnd->SetWindowTeam( TEAM_BLUE );
			pWinWnd->SetPlayStage( m_pPlayStage );
			pWinWnd->SetGangsiScore( true );      
			pWinWnd->ShowWnd();
		}
	}
	else  
	{
		// 유저 승
		ResultScoreUserWnd *pWinWnd = dynamic_cast<ResultScoreUserWnd *>(FindChildWnd( ID_WINNER_WND ));
		if( pWinWnd )
		{
			int iUserSize = m_FinalInfoList.size();
			for(int i = 0;i < iUserSize;i++)
			{
				FinalInfo &rkInfo = m_FinalInfoList[i];

				if( rkInfo.m_Team == TEAM_RED )
				{
					pWinWnd->AddFinalInfo( rkInfo );
				}
			}

			if( iBlueUserConut == 0 )
				pWinWnd->SetCustomWnd( MAX_PLAYER, CUSTOM_X_POS, CUSTOM_Y_POS, pWinWnd->GetWidth(), CUSTOM_FULL_SIZE );
			else
				pWinWnd->SetCustomWnd( iRedUserCount, CUSTOM_X_POS, CUSTOM_Y_POS, pWinWnd->GetWidth(), CUSTOM_DEFAULT_H + (iRedUserCount * CUSTOM_USER_SIZE) );
			pWinWnd->SetWindowTeam( TEAM_RED );
			pWinWnd->SetPlayStage( m_pPlayStage );
			pWinWnd->SetGangsiScore( false );      
			pWinWnd->ShowWnd();
		}

		if( iBlueUserConut > 0 )
		{
			// 강시 패
			ResultScoreUserWnd *pLoseWnd = dynamic_cast<ResultScoreUserWnd *>(FindChildWnd( ID_LOSER_WND ));
			if( pLoseWnd )
			{
				int iUserSize = m_FinalInfoList.size();
				for(int i = 0;i < iUserSize;i++)
				{
					FinalInfo &rkInfo = m_FinalInfoList[i];

					if( rkInfo.m_Team == TEAM_BLUE )
					{
						pLoseWnd->AddFinalInfo( rkInfo );
					}
				}

				pLoseWnd->SetCustomWnd( MAX_PLAYER - iRedUserCount, CUSTOM_X_POS, CUSTOM_LOSER_Y_POS + ( CUSTOM_USER_SIZE * iRedUserCount ), pLoseWnd->GetWidth(), CUSTOM_DEFAULT_H + ((MAX_PLAYER-iRedUserCount) * CUSTOM_USER_SIZE) );
				pLoseWnd->SetWindowTeam( TEAM_BLUE );
				pLoseWnd->SetPlayStage( m_pPlayStage );
				pLoseWnd->SetGangsiScore( false );      
				pLoseWnd->ShowWnd();
			}
		}			
	}			
}

void TotalResultScoreWnd::SetSingleScore()
{
	if( !m_pPlayStage ) return;

	if( m_pPlayStage->GetModeType() == MT_SHUFFLE_BONUS )
		std::sort( m_FinalInfoList.begin(), m_FinalInfoList.end(), FinalInfoShuffleStarSort() );
	else
		std::sort( m_FinalInfoList.begin(), m_FinalInfoList.end(), FinalInfoSort() );

	char szTitle[MAX_PATH] = "";
	if( m_pPlayStage->GetModeType() == MT_SURVIVAL )
		SafeSprintf( szTitle, sizeof( szTitle ), STR(1) );
	else if( m_pPlayStage->GetModeType() == MT_BOSS )
		SafeSprintf( szTitle, sizeof( szTitle ), STR(2) );
	else if( m_pPlayStage->GetModeType() == MT_FIGHT_CLUB )
		SafeSprintf( szTitle, sizeof( szTitle ), STR(3) );
	else if( m_pPlayStage->GetModeType() == MT_SHUFFLE_BONUS )
		SafeSprintf( szTitle, sizeof( szTitle ), "보너스모드" );
	else if( m_pPlayStage->GetModeType() == MT_RAID )
		SafeSprintf( szTitle, sizeof( szTitle ), "레이드" );
	SetTitleText( szTitle );

	// 윈도우 세팅
	ResultScoreUserWnd *pUserWnd = dynamic_cast<ResultScoreUserWnd *>(FindChildWnd( ID_SINGLE_WND ));
	if( pUserWnd )
	{
		int iUserSize = m_FinalInfoList.size();
		for(int i = 0;i < iUserSize;i++)
		{
			FinalInfo &rkInfo = m_FinalInfoList[i];
			
			// 자신은 블루 
			if( rkInfo.m_bOwner )
				rkInfo.m_Team = TEAM_BLUE;
			else
				rkInfo.m_Team = TEAM_RED;
			pUserWnd->AddFinalInfo( rkInfo );
		}
		pUserWnd->SetPlayStage( m_pPlayStage );
		pUserWnd->SetWindowTeam( TEAM_RED );
		pUserWnd->SetSingleScore();
		pUserWnd->ShowWnd();
	}
}

void TotalResultScoreWnd::SetDungeonsScore( int iResultFloor, int iMaxFloor )
{
	if( !m_pPlayStage ) return;

	std::sort( m_FinalInfoList.begin(), m_FinalInfoList.end(), FinalInfoSort() );

	char szTitle[MAX_PATH] = "";
	SafeSprintf( szTitle, sizeof( szTitle ), STR(1), iResultFloor );
	SetTitleText( szTitle );

	// 윈도우 세팅
	ResultScoreUserWnd *pUserWnd = dynamic_cast<ResultScoreUserWnd *>(FindChildWnd( ID_SINGLE_WND ));
	if( pUserWnd )
	{
		int iUserSize = m_FinalInfoList.size();
		for(int i = 0;i < iUserSize;i++)
		{
			FinalInfo &rkInfo = m_FinalInfoList[i];

			// 모두 블루 
			rkInfo.m_Team = TEAM_BLUE;
			pUserWnd->AddFinalInfo( rkInfo );
		}
		pUserWnd->SetPlayStage( m_pPlayStage );
		pUserWnd->SetWindowTeam( TEAM_BLUE );
		pUserWnd->SetDungeonsScore( iResultFloor, iMaxFloor );
		pUserWnd->ShowWnd();
	}
}

void TotalResultScoreWnd::SetTowerDefScore()
{
	if( !m_pPlayStage ) return;

	// 윈도우 세팅
	ResultRaidScoreUserWnd *pRaidWnd = dynamic_cast<ResultRaidScoreUserWnd *>(FindChildWnd( ID_RAID_WND ));
	if( pRaidWnd )
	{
		int iUserSize = m_FinalInfoList.size();
		for(int i = 0;i < iUserSize;i++)
		{
			FinalInfo &rkInfo = m_FinalInfoList[i];

			// 모두 블루 
			rkInfo.m_Team = TEAM_BLUE;
			pRaidWnd->AddFinalInfo( iUserSize, rkInfo );
		}
		pRaidWnd->SetPlayStage( m_pPlayStage );
		pRaidWnd->SetDungeonsScore( 1, 1 );
		pRaidWnd->ShowWnd();
	}
}

void TotalResultScoreWnd::SetRaidScore( bool bClear )
{
	if( !m_pPlayStage ) return;

	char szTitle[MAX_PATH] = "";
	if( bClear )
	{
		SafeSprintf( szTitle, sizeof( szTitle ), STR(1), g_ModeSelectInfo.GetBattleModeTitle( g_BattleRoomMgr.GetSelectModeIndex() ).c_str() );
	}
	else
	{
		SafeSprintf( szTitle, sizeof( szTitle ), STR(2), g_ModeSelectInfo.GetBattleModeTitle( g_BattleRoomMgr.GetSelectModeIndex() ).c_str() );
	}
	SetTitleText( szTitle );

	// 윈도우 세팅
	ResultRaidScoreUserWnd *pRaidWnd = dynamic_cast<ResultRaidScoreUserWnd *>(FindChildWnd( ID_RAID_WND ));
	if( pRaidWnd )
	{
		int iUserSize = m_FinalInfoList.size();
		for(int i = 0;i < iUserSize;i++)
		{
			FinalInfo &rkInfo = m_FinalInfoList[i];

			rkInfo.m_Team = TEAM_BLUE;
			pRaidWnd->AddFinalInfo( iUserSize, rkInfo );
		}
		pRaidWnd->SetPlayStage( m_pPlayStage );
		pRaidWnd->SetRaidScore( bClear );
		pRaidWnd->ShowWnd();
	}
}

void TotalResultScoreWnd::SetTeamScore( TeamType eMyTeam, int iBlueWin, int iRedWin, TeamType eTourWinTeam )
{
	if( !m_pPlayStage  ) return;

	std::sort( m_FinalInfoList.begin(), m_FinalInfoList.end(), FinalInfoSort() );

	bool bTournamentSocreRender = true;
	char szTitle[MAX_PATH] = "";
	switch( m_pPlayStage->GetModeType() )
	{
	case MT_SYMBOL:
		SafeSprintf( szTitle, sizeof( szTitle ), STR(1), iBlueWin, iRedWin );
		break;
	case MT_FIRE_TEMPLE:
	case MT_DARK_XMAS:
	case MT_TOWER_DEFENSE:
	case MT_FACTORY:
	case MT_RAID:
		SafeSprintf( szTitle, sizeof( szTitle ), STR(1), iBlueWin, iRedWin );		
		break;
	case MT_CATCH:
		SafeSprintf( szTitle, sizeof( szTitle ), STR(2), iBlueWin, iRedWin );		
		break;
	case MT_CATCH_RUNNINGMAN:
		SafeSprintf( szTitle, sizeof( szTitle ), STR(7), iBlueWin, iRedWin );		
		break;
	case MT_KING:
		SafeSprintf( szTitle, sizeof( szTitle ), STR(3), iBlueWin, iRedWin );		
		break;
	case MT_TEAM_SURVIVAL:
	case MT_TEAM_SURVIVAL_AI:
		if( g_BattleRoomMgr.IsTournamentModeType() && eTourWinTeam != TEAM_NONE )
		{
			bTournamentSocreRender = false;
			SafeSprintf( szTitle, sizeof( szTitle ), STR(8) );
		}
		else
		{
			SafeSprintf( szTitle, sizeof( szTitle ), STR(4), iBlueWin, iRedWin );
		}
		break;
	case MT_DOUBLE_CROWN:
		SafeSprintf( szTitle, sizeof( szTitle ), "더블크라운" );
		break;
	case MT_FOOTBALL:
		SafeSprintf( szTitle, sizeof( szTitle ), STR(5), iBlueWin, iRedWin );		
		break;
	case MT_HEROMATCH:
		SafeSprintf( szTitle, sizeof( szTitle ), STR(6), iBlueWin, iRedWin );		
		break;
	case MT_UNDERWEAR:
		SafeSprintf( szTitle, sizeof( szTitle ), "팬티몬", iBlueWin, iRedWin );		
		break;
	case MT_CBT:
		SafeSprintf( szTitle, sizeof( szTitle ), "클베모드", iBlueWin, iRedWin );		
		break;
	case MT_FLAG_CAPTURE:
		SafeSprintf( szTitle, sizeof( szTitle ), STR(9), iBlueWin, iRedWin );		
		break;
	case MT_ARENA:
		SafeSprintf( szTitle, sizeof( szTitle ), STR(10), iBlueWin, iRedWin );		
		break;
	case MT_FARMING:
		SafeSprintf( szTitle, sizeof( szTitle ), STR(12), iBlueWin, iRedWin );		
		break;
	}
	SetTitleText( szTitle );

	WinTeamType eWinTeam = WTT_DRAW;
	if( eTourWinTeam != TEAM_NONE )
	{
		if( eTourWinTeam == TEAM_BLUE )
			eWinTeam = WTT_BLUE_TEAM;
		else
			eWinTeam = WTT_RED_TEAM;
	}
	else
	{
		if( iBlueWin > iRedWin )
			eWinTeam = WTT_BLUE_TEAM;
		else if( iBlueWin < iRedWin )
			eWinTeam = WTT_RED_TEAM;
		else 
			eWinTeam = WTT_BLUE_TEAM;         //비겼을 때는 WINNER윈도우가 블루팀이된다.
	}

	ResultScoreUserWnd *pWinWnd = dynamic_cast<ResultScoreUserWnd *>(FindChildWnd( ID_WINNER_WND ));
	if( pWinWnd )
	{
		int iUserSize = m_FinalInfoList.size();
		for(int i = 0;i < iUserSize;i++)
		{
			FinalInfo &rkInfo = m_FinalInfoList[i];
			if( IsWinTeam( eWinTeam, rkInfo.m_Team )  )
				pWinWnd->AddFinalInfo( rkInfo );
		}

		if( eWinTeam == WTT_BLUE_TEAM )
			pWinWnd->SetWindowTeam( TEAM_BLUE );
		else
			pWinWnd->SetWindowTeam( TEAM_RED );

		pWinWnd->SetPlayStage( m_pPlayStage );

		if( g_BattleRoomMgr.IsTournamentModeType() && eTourWinTeam != TEAM_NONE )
			pWinWnd->SetTeamScore( eMyTeam, iBlueWin, iRedWin, false );
		else
			pWinWnd->SetTeamScore( eMyTeam, iBlueWin, iRedWin );

		pWinWnd->ShowWnd();
	}

	ResultScoreUserWnd *pLoseWnd = dynamic_cast<ResultScoreUserWnd *>(FindChildWnd( ID_LOSER_WND ));
	if( pLoseWnd )
	{
		int iUserSize = m_FinalInfoList.size();
		for(int i = 0;i < iUserSize;i++)
		{
			FinalInfo &rkInfo = m_FinalInfoList[i];
			if( !IsWinTeam( eWinTeam, rkInfo.m_Team )  )
				pLoseWnd->AddFinalInfo( rkInfo );
		}

		if( eWinTeam == WTT_BLUE_TEAM )
			pLoseWnd->SetWindowTeam( TEAM_RED );
		else
			pLoseWnd->SetWindowTeam( TEAM_BLUE );

		pLoseWnd->SetPlayStage( m_pPlayStage );

		if(g_BattleRoomMgr.IsTournamentModeType() && eTourWinTeam != TEAM_NONE )
			pLoseWnd->SetTeamScore( eMyTeam, iBlueWin, iRedWin, false );
		else
			pLoseWnd->SetTeamScore( eMyTeam, iBlueWin, iRedWin );

		pLoseWnd->ShowWnd();
	}			
}

void TotalResultScoreWnd::SetGuildTeamBonus( float fBlueGuildTeamBonus, float fRedGuildTeamBonus )
{
	ResultScoreUserWnd *pSingleWnd = dynamic_cast<ResultScoreUserWnd *>(FindChildWnd( ID_SINGLE_WND ));
	if( pSingleWnd )
	{
		pSingleWnd->SetGuildTeamBonus( fBlueGuildTeamBonus, fRedGuildTeamBonus );
	}

	ResultScoreUserWnd *pWinnerWnd = dynamic_cast<ResultScoreUserWnd *>(FindChildWnd( ID_WINNER_WND ));
	if( pWinnerWnd )
	{
		pWinnerWnd->SetGuildTeamBonus( fBlueGuildTeamBonus, fRedGuildTeamBonus );
	}

	ResultScoreUserWnd *pLoserWnd = dynamic_cast<ResultScoreUserWnd *>(FindChildWnd( ID_LOSER_WND ));
	if( pLoserWnd )
	{
		pLoserWnd->SetGuildTeamBonus( fBlueGuildTeamBonus, fRedGuildTeamBonus );
	}
}

void TotalResultScoreWnd::iwm_hide()
{
	HideChildWnd( ID_SINGLE_WND );
	HideChildWnd( ID_WINNER_WND );
	HideChildWnd( ID_LOSER_WND );
	HideChildWnd( ID_RAID_WND );
}

void TotalResultScoreWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_RAID_WND:
		if( cmd == ResultRaidScoreUserWnd::STATE_DELAY )
		{
			// 랭킹쪽 애니 종료 되었음
			if( GetParent() )
				GetParent()->iwm_command( this, cmd, param );
		}
		break;
	}
}
//////////////////////////////////////////////////////////////////////////
bool TotalResultMainWnd::sg_bHelpBarShow = false;
bool TotalResultMainWnd::sg_bLevelGaugeAniEnd = false;
TotalResultMainWnd::TotalResultMainWnd()
{
	m_pPlayStage  = NULL;
	sg_bHelpBarShow = false;
	sg_bLevelGaugeAniEnd = false;
	g_MyInfo.InitLevelUPData();

	m_iDefaultX   = 0;
	m_iDefaultY   = 0;

	m_pTournamentTtitleBack = NULL;
	m_pTournamentUITitle	= NULL;

	m_fContributeTotalBlueScore = 0;
	m_fContributeTotalRedScore  = 0;

	m_iSpiritCode = 0;
	m_iSpiritCount = 0;
}

TotalResultMainWnd::~TotalResultMainWnd()
{
	SAFEDELETE( m_pTournamentTtitleBack );
	SAFEDELETE( m_pTournamentUITitle );

	g_MyInfo.InitLevelUPData();

	if( !COMPARE( m_iDefaultX, 0, Setting::Width() ) || 
		!COMPARE( m_iDefaultY, 0, Setting::Height() ) )
		return;

	g_GUIMgr.GUIPosSaveInt( "TotalResultMainWnd", "XPos", m_iDefaultX ,POS_SAVE_VERSION );
	g_GUIMgr.GUIPosSaveInt( "TotalResultMainWnd", "YPos", m_iDefaultY ,POS_SAVE_VERSION );
}

void TotalResultMainWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "TtitleBack" )
	{
		SAFEDELETE( m_pTournamentTtitleBack );
		m_pTournamentTtitleBack = pFrame;
	}
	else
	{
		ioWnd::AddRenderFrame( szType, pFrame );
	}
}

void TotalResultMainWnd::SetPlayStage( ioPlayStage *pPlayStage )
{
	m_pPlayStage = pPlayStage;

	TotalResultAwardWnd *pAwardWnd = dynamic_cast<TotalResultAwardWnd*>(FindChildWnd(ID_AWARD_WND));
	if( pAwardWnd )
	{
		pAwardWnd->SetPlayStage( pPlayStage );
	}

	TotalResultScoreWnd *pSocreWnd = dynamic_cast<TotalResultScoreWnd*>(FindChildWnd(ID_SCORE_WND));
	if( pSocreWnd )
	{
		pSocreWnd->SetPlayStage( pPlayStage );
	}

	TotalResultChatWnd *pChatWnd = dynamic_cast<TotalResultChatWnd*>(FindChildWnd(ID_CHAT_WND));
	if( pChatWnd )
	{
		pChatWnd->SetPlayStage( pPlayStage );
	}
	
	TotalResultTreasureCardWnd *pTreasureWnd = dynamic_cast<TotalResultTreasureCardWnd*>(FindChildWnd(ID_TREASURE_WND));
	if( pTreasureWnd )
	{
		pTreasureWnd->SetPlayStage( pPlayStage );
	}
	TotalResultTournamentWnd *pTournament = dynamic_cast<TotalResultTournamentWnd*>(FindChildWnd(ID_TOURNAMENT_RESULT_WND));
	if( pTournament )
	{
		pTournament->SetPlayStage( pPlayStage );
	}

	TotalResultRaidCardWnd *pRaidCardWnd = dynamic_cast<TotalResultRaidCardWnd*>(FindChildWnd(ID_RAID_CARD_WND));
	if( pRaidCardWnd )
	{
		pRaidCardWnd->SetPlayStage( pPlayStage );
	}
}

void TotalResultMainWnd::iwm_create()
{
	g_GUIMgr.AddWnd( "XML/totalresultawardwnd.xml", this );
	g_GUIMgr.AddWnd( "XML/totalresultscorewnd.xml", this );
	g_GUIMgr.AddWnd( "XML/totalresultexpwnd.xml", this );
	g_GUIMgr.AddWnd( "XML/totalresultrawardwnd.xml", this );
	g_GUIMgr.AddWnd( "XML/totalresultchatwnd.xml", this );
	g_GUIMgr.AddWnd( "XML/totalresulttreasurecardwnd.xml", this );
	g_GUIMgr.AddWnd( "XML/totalresulttournamentwnd.xml", this );
	g_GUIMgr.AddWnd( "XML/totalresultraidcardwnd.xml", this );

	int iXPos = g_GUIMgr.GUIPosLoadInt( "TotalResultMainWnd", "XPos", GetXPos() ,POS_SAVE_VERSION );
	int iYPos = g_GUIMgr.GUIPosLoadInt( "TotalResultMainWnd", "YPos", GetYPos() ,POS_SAVE_VERSION );

	if( COMPARE( iXPos, 0, Setting::Width() ) && COMPARE( iYPos, 0, Setting::Height() ) )
	{
		SetWndPos( iXPos, iYPos );
	}
	m_iDefaultX = GetXPos();
	m_iDefaultY = GetYPos();
}

void TotalResultMainWnd::iwm_show()
{
	g_GUIMgr.HideWnd( GLOBAL_CHAT_WND );

	if( g_BattleRoomMgr.IsTournamentModeType() )
	{ 
		ShowChildWnd( ID_TOURNAMENT_RESULT_WND );

		SAFEDELETE( m_pTournamentUITitle );
		m_pTournamentUITitle = new ioUITitle;
				
		m_pTournamentUITitle->SetSize( 17 );
		m_pTournamentUITitle->SetText( "대회 결과", "" );
		m_pTournamentUITitle->SetStyle( TS_OUTLINE_FULL_2X );
		m_pTournamentUITitle->SetHorzAlign( TAT_CENTER );
		m_pTournamentUITitle->SetColor( "Normal", "FFFFFFFF", "FF0C426F" );
	}
	else if( ioPlayMode::GetModeType() == MT_DUNGEON_A || 
			Help::IsMonsterDungeonMode(ioPlayMode::GetModeType()) || 
			ioPlayMode::GetModeType() == MT_RAID )
	{
		TotalResultScoreWnd *pSocreWnd = dynamic_cast<TotalResultScoreWnd*>(FindChildWnd(ID_SCORE_WND));
		if( pSocreWnd )
		{
			pSocreWnd->SetStartScore();
		}
		
		ShowChildWnd( ID_RAID_CARD_WND );
	}
	else
	{
		ShowChildWnd( ID_AWARD_WND );
	}

	ShowChildWnd( ID_SCORE_WND );
	ShowChildWnd( ID_CHAT_WND );

	g_BackgroundMgr.SetBackground( BGT_RESULT );

	// 지금부터 하단바는 계속 나와있는다.
	sg_bHelpBarShow = true;      
}

void TotalResultMainWnd::iwm_hide()
{
	HideChildWnd( ID_AWARD_WND );
	HideChildWnd( ID_SCORE_WND );
	HideChildWnd( ID_EXP_WND );
	HideChildWnd( ID_REWARD_WND );
	HideChildWnd( ID_TREASURE_WND );
	HideChildWnd( ID_TOURNAMENT_RESULT_WND );
	HideChildWnd( ID_RAID_CARD_WND );
	HideChildWnd( ID_SPIRIT_REWARD_WND );
}

bool TotalResultMainWnd::iwm_esc()
{
	TotalResultChatWnd *pChatWnd = dynamic_cast<TotalResultChatWnd*>(FindChildWnd(ID_CHAT_WND));
	if( pChatWnd && pChatWnd->IsShow() )
		return pChatWnd->iwm_esc();
	return false;
}

void TotalResultMainWnd::SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate )
{
	ioMovingWnd::SetPositionToDefault( iParentWidth, iParentHeight, bChildMove, bCreate );

	if( !bCreate )
	{
		m_iDefaultX = GetXPos();
		m_iDefaultY = GetYPos();
	}
}

void TotalResultMainWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_AWARD_WND:
		if( cmd == TotalResultAwardWnd::AWARD_ROLLING_END )
		{
			// 롤링이 끝났으므로 좌측 결과 UI 롤링
			TotalResultScoreWnd *pSocreWnd = dynamic_cast<TotalResultScoreWnd*>(FindChildWnd(ID_SCORE_WND));
			if( pSocreWnd )
			{
				pSocreWnd->SetStartScore();
			}
			ShowChildWnd( ID_ALPHA_WND );
		}
		else if( cmd == TotalResultAwardWnd::AWARD_TIME_END )
		{
			// 시상 관련 내용이 종료되었으므로 좌측 결과 & 시상식을 닫고 보상 UI로 전환
			HideChildWnd( ID_AWARD_WND );
			HideChildWnd( ID_SCORE_WND );

			ShowChildWnd( ID_EXP_WND );
			ShowChildWnd( ID_REWARD_WND );
			ShowSpiritRewardWnd( false );
		}
		break;
	case ID_SCORE_WND:
		if( cmd == ResultRaidScoreUserWnd::STATE_DELAY )
		{
			//보물 카드 UI 활성화
			TotalResultTreasureCardWnd *pTreasureWnd = dynamic_cast<TotalResultTreasureCardWnd*>(FindChildWnd(ID_TREASURE_WND));
			if( pTreasureWnd )
			{
				int iTreasureCount = 0;
				for(int i = 0;i < (int)m_FinalInfoList.size();i++)
				{
					FinalInfo &rkInfo = m_FinalInfoList[i];
					if( rkInfo.m_szName == g_MyInfo.GetPublicID() )
					{
						iTreasureCount = rkInfo.m_iTreasureCardCnt;
						break;
					}
				}
				pTreasureWnd->SetStartTreasure( iTreasureCount );
			}
		}
		break;
	case ID_TREASURE_WND:
		if( cmd == TotalResultTreasureCardWnd::STATE_CLEAR )
		{
			HideChildWnd( ID_TREASURE_WND );
			HideChildWnd( ID_SCORE_WND );

			ShowChildWnd( ID_EXP_WND );
			ShowChildWnd( ID_REWARD_WND );
		}
		break;
	case ID_EXP_WND:
		if( cmd == TotalResultExpGradeWnd::END_GAUGE_ANI )
		{
			sg_bLevelGaugeAniEnd = true;

			// 보상 UI 애니메이션 시작...
			TotalResultRawardWnd *pReward = dynamic_cast<TotalResultRawardWnd*>(FindChildWnd(ID_REWARD_WND));
			if( pReward )
			{
				pReward->StartBonus();
			}
			ShowChildWnd( ID_ALPHA_WND );
		}
		break;
	case ID_TOURNAMENT_RESULT_WND:
		{
			if( cmd == TotalResultTournamentWnd::TORUNAMENT_GUAGE_COMPLETE )
			{
				// 롤링이 끝났으므로 좌측 결과 UI 롤링
				TotalResultScoreWnd *pSocreWnd = dynamic_cast<TotalResultScoreWnd*>(FindChildWnd(ID_SCORE_WND));
				if( pSocreWnd )
				{
					pSocreWnd->SetStartScore();
				}
				ShowChildWnd( ID_ALPHA_WND );
			}
		}
		break;
	case ID_RAID_CARD_WND:
		if( cmd == TotalResultRaidCardWnd::STATE_CLEAR )
		{
			HideChildWnd( ID_RAID_CARD_WND );
			HideChildWnd( ID_SCORE_WND );
			HideChildWnd( ID_SPIRIT_REWARD_WND );

			ShowChildWnd( ID_EXP_WND );
			ShowChildWnd( ID_REWARD_WND );
		}
		break;
	}
}

void TotalResultMainWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

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
}

void TotalResultMainWnd::OnRender()
{
	g_BackgroundMgr.RenderBackground();

	ioWnd::OnRender();
}

void TotalResultMainWnd::OnRenderAfterChild()
{
	ioWnd::OnRenderAfterChild();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();
	
	if( g_BattleRoomMgr.IsTournamentModeType() )
	{
		if( m_pTournamentTtitleBack )
		{
			m_pTournamentTtitleBack->Render( iXPos, iYPos );
		}
		if( m_pTournamentUITitle )
		{
			m_pTournamentUITitle->PrintNormal( iXPos + 499, iYPos + 11 );
		}
	}
}

void TotalResultMainWnd::SetAwarding( SP2Packet &rkPacket )
{
	// 시상식 정보
	TotalResultAwardWnd *pAwardWnd = dynamic_cast<TotalResultAwardWnd*>(FindChildWnd(ID_AWARD_WND));
	if( pAwardWnd )
	{
		pAwardWnd->SetAwarding( rkPacket );
	}
}

void TotalResultMainWnd::SetAwardingCharMotion( const ioHashString &rkCharName, int iAwardType, DWORD dwCharMotionItem )
{
	TotalResultAwardWnd *pAwardWnd = dynamic_cast<TotalResultAwardWnd*>(FindChildWnd(ID_AWARD_WND));
	if( pAwardWnd )
	{
		pAwardWnd->SetAwardingCharMotion( rkCharName, iAwardType, dwCharMotionItem );
	}
}

void TotalResultMainWnd::AddFinalInfo( const FinalInfo &rkInfo )
{
	if( rkInfo.m_Team == TEAM_NONE ) 
		return;

	// 플레이 유저 정보
	m_FinalInfoList.push_back( rkInfo );

	TotalResultAwardWnd *pAwardWnd = dynamic_cast<TotalResultAwardWnd*>(FindChildWnd(ID_AWARD_WND));
	if( pAwardWnd )
	{
		pAwardWnd->AddFinalInfo( rkInfo );
	}

	TotalResultScoreWnd *pSocreWnd = dynamic_cast<TotalResultScoreWnd*>(FindChildWnd(ID_SCORE_WND));
	if( pSocreWnd )
	{
		pSocreWnd->AddFinalInfo( rkInfo );
	}
	
	TotalResultTreasureCardWnd *pTreasureCardWnd = dynamic_cast< TotalResultTreasureCardWnd * >( FindChildWnd( ID_TREASURE_WND ) );
	if( pTreasureCardWnd )
	{
		pTreasureCardWnd->AddFinalInfo( rkInfo );
	}
	TotalResultRaidCardWnd *pRaidCardWnd = dynamic_cast<TotalResultRaidCardWnd*>(FindChildWnd(ID_RAID_CARD_WND));
	if( pRaidCardWnd )
	{
		pRaidCardWnd->AddFinalInfo( rkInfo );
	}
}

bool TotalResultMainWnd::IsPresentAlarmBlock()
{
	TotalResultAwardWnd *pAwardWnd = dynamic_cast<TotalResultAwardWnd*>(FindChildWnd(ID_AWARD_WND));
	if( pAwardWnd && pAwardWnd->IsShow() )
		return pAwardWnd->IsPresentAlarmBlock();
		
	TotalResultTreasureCardWnd *pTreasureWnd = dynamic_cast<TotalResultTreasureCardWnd*>(FindChildWnd(ID_TREASURE_WND));
	if( pTreasureWnd && pTreasureWnd->IsShow() )
		return pTreasureWnd->IsPresentAlarmBlock();

	TotalResultRaidCardWnd *pRaidCardWnd = dynamic_cast<TotalResultRaidCardWnd*>(FindChildWnd(ID_RAID_CARD_WND));
	if( pRaidCardWnd && pRaidCardWnd->IsShow() )
		return pRaidCardWnd->IsPresentAlarmBlock();

	return false;
}

int TotalResultMainWnd::GetAwardCharItemReinforce( const ioHashString &szName, int iSlot )
{
	TotalResultAwardWnd *pAwardWnd = dynamic_cast<TotalResultAwardWnd*>(FindChildWnd(ID_AWARD_WND));
	if( !pAwardWnd )
		return 0;
	if( pAwardWnd->IsShow() )
		return pAwardWnd->GetAwardCharItemReinforce( szName, iSlot );
	return 0;
}

int TotalResultMainWnd::GetAwardCharLevel( const ioHashString &szName )
{
	TotalResultAwardWnd *pAwardWnd = dynamic_cast<TotalResultAwardWnd*>(FindChildWnd(ID_AWARD_WND));
	if( !pAwardWnd )
		return 0;
	if( pAwardWnd->IsShow() )
		return pAwardWnd->GetAwardCharLevel( szName );
	return 0;
}

void TotalResultMainWnd::GetAwardMedalItemType( IN const ioHashString &szName, OUT MedalSyncInfoVec &rvMedalItemTypeVec )
{
	TotalResultAwardWnd *pAwardWnd = dynamic_cast<TotalResultAwardWnd*>(FindChildWnd(ID_AWARD_WND));
	if( !pAwardWnd )
		return;
	if( pAwardWnd->IsShow() )
		pAwardWnd->GetMedalItemType( szName, rvMedalItemTypeVec );
}

void TotalResultMainWnd::SetGangsiScore()
{
	TotalResultScoreWnd *pSocreWnd = dynamic_cast<TotalResultScoreWnd*>(FindChildWnd(ID_SCORE_WND));
	if( pSocreWnd )
	{
		pSocreWnd->SetGangsiScore();
	}
}

void TotalResultMainWnd::SetSingleScore()
{
	TotalResultScoreWnd *pSocreWnd = dynamic_cast<TotalResultScoreWnd*>(FindChildWnd(ID_SCORE_WND));
	if( pSocreWnd )
	{
		pSocreWnd->SetSingleScore();
	}
}

void TotalResultMainWnd::SetDungeonsScore( int iResultFloor, int iMaxFloor )
{
	TotalResultScoreWnd *pSocreWnd = dynamic_cast<TotalResultScoreWnd*>(FindChildWnd(ID_SCORE_WND));
	if( pSocreWnd )
	{
		pSocreWnd->SetDungeonsScore( iResultFloor, iMaxFloor );
	}
}

void TotalResultMainWnd::SetTowerDefScore()
{
	TotalResultScoreWnd *pSocreWnd = dynamic_cast<TotalResultScoreWnd*>(FindChildWnd(ID_SCORE_WND));
	if( pSocreWnd )
	{
		pSocreWnd->SetTowerDefScore();
	}
}

void TotalResultMainWnd::SetRaidScore( bool bClear )
{
	TotalResultScoreWnd *pSocreWnd = dynamic_cast<TotalResultScoreWnd*>(FindChildWnd(ID_SCORE_WND));
	if( pSocreWnd )
	{
		pSocreWnd->SetRaidScore( bClear );
	}
}

void TotalResultMainWnd::SetTournamentContributeScore( float fBlueScore, float fRedScore )
{
	m_fContributeTotalBlueScore = fBlueScore;
	m_fContributeTotalRedScore  = fRedScore;
}

void TotalResultMainWnd::SetTeamScore( TeamType eMyTeam, int iBlueWin, int iRedWin )
{
	if( g_BattleRoomMgr.IsBattleRoom() )
	{
		if( iRedWin > iBlueWin )
			g_BattleRoomMgr.UpdateRecord( TEAM_RED );
		else if( iBlueWin > iRedWin )
			g_BattleRoomMgr.UpdateRecord( TEAM_BLUE );
		else
			g_BattleRoomMgr.UpdateRecord( TEAM_NONE );
	}	
	else if( g_LadderTeamMgr.IsLadderTeam() )
	{
		if( ioPlayMode::GetModeType() == MT_HEROMATCH )
		{
			TeamType eWinTeam = TEAM_NONE;
			if( iRedWin > iBlueWin )
				eWinTeam = TEAM_RED;
			else if( iBlueWin > iRedWin )
				eWinTeam = TEAM_BLUE;
			else
				eWinTeam = TEAM_NONE;
			
			if( eMyTeam != eWinTeam )
			{
				// 상대가 중간 이탈로 인한 패배라면 패배처리 안함.
				if( iBlueWin < HEROMATCH_NEED_ROUND && iRedWin < HEROMATCH_NEED_ROUND )
					eWinTeam = TEAM_NONE;
			}
			g_LadderTeamMgr.UpdateRecord( eMyTeam, eWinTeam );
		}
		else
		{
			if( iRedWin > iBlueWin )
				g_LadderTeamMgr.UpdateRecord( eMyTeam, TEAM_RED );
			else if( iBlueWin > iRedWin )
				g_LadderTeamMgr.UpdateRecord( eMyTeam, TEAM_BLUE );
			else
				g_LadderTeamMgr.UpdateRecord( eMyTeam, TEAM_NONE );
		}
	}
	
	TotalResultScoreWnd *pSocreWnd = dynamic_cast<TotalResultScoreWnd*>(FindChildWnd(ID_SCORE_WND));
	if( pSocreWnd )
	{
		if( g_BattleRoomMgr.IsTournamentModeType() && 0 < m_fContributeTotalBlueScore && 0 < m_fContributeTotalRedScore ) 
		{
			TeamType eWinTeam = TEAM_NONE;
			if( m_fContributeTotalRedScore > m_fContributeTotalBlueScore )
			{
				eWinTeam =  TEAM_RED;
				pSocreWnd->SetTeamScore( eMyTeam, 0, 1, eWinTeam );
			}
			else if( m_fContributeTotalBlueScore > m_fContributeTotalRedScore )
			{
				eWinTeam =  TEAM_BLUE;	
				pSocreWnd->SetTeamScore( eMyTeam, 1, 0, eWinTeam );
			}			
		}
		else
		{
			pSocreWnd->SetTeamScore( eMyTeam, iBlueWin, iRedWin );
		}
	}
	if( g_BattleRoomMgr.IsTournamentModeType() )
	{
		TotalResultTournamentWnd *pTournamentScoreWnd = dynamic_cast<TotalResultTournamentWnd*>(FindChildWnd(ID_TOURNAMENT_RESULT_WND));
		if( pTournamentScoreWnd )
		{
			if( 0 < m_fContributeTotalBlueScore && 0 < m_fContributeTotalRedScore )
				pTournamentScoreWnd->SetTeamScore( eMyTeam, m_fContributeTotalBlueScore, m_fContributeTotalRedScore );
			else
				pTournamentScoreWnd->SetTeamScore( eMyTeam, iBlueWin, iRedWin );

			pTournamentScoreWnd->SetTournamentData();
		}
	}

	//데이터 전달 후 초기화
	m_fContributeTotalRedScore  = 0;
	m_fContributeTotalBlueScore = 0;
}

void TotalResultMainWnd::SetGuildTeamBonus( float fBlueGuildTeamBonus, float fRedGuildTeamBonus )
{
	TotalResultScoreWnd *pSocreWnd = dynamic_cast<TotalResultScoreWnd*>(FindChildWnd(ID_SCORE_WND));
	if( pSocreWnd )
	{
		pSocreWnd->SetGuildTeamBonus( fBlueGuildTeamBonus, fRedGuildTeamBonus );
	}
}

void TotalResultMainWnd::SetFinalRoundPoint( IntVec &rvClassTypeList, IntVec &rvClassPointList, float fBonusArray[ioPlayMode::BA_MAX] )
{
	FinalInfo kOwnerInfo;
	for(int i = 0;i < (int)m_FinalInfoList.size();i++)
	{
		if( m_FinalInfoList[i].m_bOwner )
			kOwnerInfo = m_FinalInfoList[i];
	}

	// 보상 UI에서 처리
	TotalResultExpWnd *pExpWnd = dynamic_cast<TotalResultExpWnd*>(FindChildWnd(ID_EXP_WND));
	if( pExpWnd )
	{
		pExpWnd->SetFinalRoundPoint( kOwnerInfo.m_iResultExp, kOwnerInfo.m_bLevelUP, rvClassTypeList, rvClassPointList );
	}

	TotalResultRawardWnd *pReward = dynamic_cast<TotalResultRawardWnd*>(FindChildWnd(ID_REWARD_WND));
	if( pReward )
	{
		pReward->SetFinalRoundPoint( kOwnerInfo.m_iContributePer, kOwnerInfo.m_iResultExp, kOwnerInfo.m_iResultPeso, rvClassTypeList, fBonusArray );
	}
	if( g_BattleRoomMgr.IsTournamentModeType() )
	{
		TotalResultTournamentWnd *pTournamentScoreWnd = dynamic_cast<TotalResultTournamentWnd*>(FindChildWnd(ID_TOURNAMENT_RESULT_WND));
		if( pTournamentScoreWnd )
		{
			pTournamentScoreWnd->SetPesoValue( kOwnerInfo.m_iResultPeso );
		}
	}
}

void TotalResultMainWnd::StartChatBubble( const char *szBubbleName, const char *szChat, bool bWholeChat )
{
	TotalResultAwardWnd *pAwardWnd = dynamic_cast<TotalResultAwardWnd*>(FindChildWnd(ID_AWARD_WND));
	if( !pAwardWnd ) return;
	if( !pAwardWnd->IsShow() ) return;

	pAwardWnd->StartChatBubble( szBubbleName, szChat, bWholeChat );
}

void TotalResultMainWnd::TreasureCardTimeOut( SP2Packet &rkPacket )
{
	TotalResultTreasureCardWnd *pTreasureWnd = dynamic_cast<TotalResultTreasureCardWnd*>(FindChildWnd(ID_TREASURE_WND));
	if( pTreasureWnd && pTreasureWnd->IsShow() )
		pTreasureWnd->TreasureCardTimeOut( rkPacket );

	TotalResultRaidCardWnd *pRaidCardWnd = dynamic_cast<TotalResultRaidCardWnd*>(FindChildWnd(ID_RAID_CARD_WND));
	if( pRaidCardWnd && pRaidCardWnd->IsShow() )
		pRaidCardWnd->TreasureCardTimeOut( rkPacket );
}

void TotalResultMainWnd::TreasureCardClick( SP2Packet &rkPacket )
{
	TotalResultTreasureCardWnd *pTreasureWnd = dynamic_cast<TotalResultTreasureCardWnd*>(FindChildWnd(ID_TREASURE_WND));
	TotalResultRaidCardWnd *pRaidCardWnd = dynamic_cast<TotalResultRaidCardWnd*>(FindChildWnd(ID_RAID_CARD_WND));
	if( pTreasureWnd && pTreasureWnd->IsShow() )
		pTreasureWnd->TreasureCardClick( rkPacket );
	else if( pRaidCardWnd && pRaidCardWnd->IsShow() )
		pRaidCardWnd->TreasureCardClick( rkPacket );
	else
	{
		DWORD dwID;
		ioHashString kName;
		rkPacket >> dwID >> kName;
		if( dwID == 0 || kName == g_MyInfo.GetPublicID() )
			TCPNetwork::MouseBusy( false );
	}
}

void TotalResultMainWnd::SetShufflePoint( int iPlayPoint, int iWinningPoint, int iConsecutivePoint, int iAwardPoint )
{	
	TotalResultRawardWnd *pReward = dynamic_cast<TotalResultRawardWnd*>( FindChildWnd( ID_REWARD_WND ) );
	if( pReward )
	{
		pReward->SetShufflePoint( iPlayPoint, iWinningPoint, iConsecutivePoint, iAwardPoint );
	}
}

void TotalResultMainWnd::SetSuccessionScore( TeamType eMyTeam, int iBlueWin, int iRedWin, int iBonusPeso )
{
	TotalResultScoreWnd *pSocreWnd = dynamic_cast<TotalResultScoreWnd*>(FindChildWnd(ID_SCORE_WND));
	if( pSocreWnd )
		pSocreWnd->SetTeamScore( eMyTeam, iBlueWin, iRedWin );

	//데이터 전달 후 초기화
	m_fContributeTotalRedScore  = 0;
	m_fContributeTotalBlueScore = 0;

	TotalResultRawardWnd *pReward = dynamic_cast<TotalResultRawardWnd*>( FindChildWnd( ID_REWARD_WND ) );
	if( pReward )
		pReward->SetSuccessionBonusPeso( iBonusPeso );
}

void TotalResultMainWnd::SetStartRaidTreasure( int nCnt )
{
	TotalResultRaidCardWnd *pRaidCardWnd = dynamic_cast<TotalResultRaidCardWnd*>(FindChildWnd(ID_RAID_CARD_WND));
	if( pRaidCardWnd )
		pRaidCardWnd->SetStartTreasure( nCnt );
}

void TotalResultMainWnd::ShowSpiritRewardWnd( bool bCard )
{
	TotalResultSpiritRewardWnd *pRewardWnd = dynamic_cast<TotalResultSpiritRewardWnd*>(FindChildWnd(ID_SPIRIT_REWARD_WND));
	if( pRewardWnd && m_iSpiritCode > 0 )
	{
		pRewardWnd->SetInfo( m_iSpiritCode, m_iSpiritCount );
		if( bCard )
			pRewardWnd->SetWndPos( 398, 279 );
		else
			pRewardWnd->SetWndPos( 398, 234 );

		m_iSpiritCode = 0;
		m_iSpiritCount = 0;
	}
}

void TotalResultMainWnd::SetSpiritRewardInfo( int iCode, int iCount )
{ 
	m_iSpiritCode = iCode;
	m_iSpiritCount = iCount; 
}
//////////////////////////////////////////////////////////////////////////
TotalResultChatWnd::TotalResultChatWnd()
{
}

TotalResultChatWnd::~TotalResultChatWnd()
{
}

void TotalResultChatWnd::SetPlayStage( ioPlayStage *pPlayStage )
{
	GameChatInput *pGameChatInput = dynamic_cast<GameChatInput*>(FindChildWnd( ID_CHAT_INPUT_WND ));
	if( pGameChatInput )
	{
		pGameChatInput->SetPlayStage( pPlayStage );
		pGameChatInput->SetChatTypeToBtn( CT_ALL );
	}
}

void TotalResultChatWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_szLoadInfo = xElement.GetStringAttribute_e( "LoadInfo" );		
}

void TotalResultChatWnd::iwm_show()
{
	g_GUIMgr.HideWnd( GLOBAL_CHAT_WND );

	g_ChatMgr.LoadChatInfo( m_szLoadInfo );

	ShowChildWnd( ID_CHAT_MSG_WND );
	ShowChildWnd( ID_CHAT_INPUT_WND );

	GameChatInput *pGameChatInput = dynamic_cast<GameChatInput*>(FindChildWnd( ID_CHAT_INPUT_WND ));
	if( pGameChatInput )
	{
		pGameChatInput->SetFirstChatTypeToBtn( CT_ALL );
	}

	g_ChatMgr.SetMsgWndFeature( 0 );
}

void TotalResultChatWnd::iwm_hide()
{
	HideChildWnd( ID_CHAT_MSG_WND );
	HideChildWnd( ID_CHAT_INPUT_WND );
}

bool TotalResultChatWnd::iwm_esc()
{
	GameChatInput *pGameChatInput = dynamic_cast<GameChatInput*>(FindChildWnd( ID_CHAT_INPUT_WND ));
	if( pGameChatInput )
		return pGameChatInput->iwm_esc();
	return false;
}

void TotalResultChatWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
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
TotalResultRaidCardBtn::TotalResultRaidCardBtn()
{
	m_pCardCover = NULL;
	m_pPresentCover = NULL;
	m_pPresentCoverOverAdd = NULL;
	m_pPresentCoverMe = NULL;

	m_pPresentIcon = NULL;
	m_pSubIcon = NULL;

	m_iPresentType = 0;
	m_iPresentValue1 = m_iPresentValue2 = 0;

	m_bScreenStart = false;
	m_bScreen = false;
	m_dwCurrentTime = 0;
}

TotalResultRaidCardBtn::~TotalResultRaidCardBtn()
{
	SAFEDELETE( m_pCardCover );
	SAFEDELETE( m_pPresentCover );
	SAFEDELETE( m_pPresentCoverOverAdd );
	SAFEDELETE( m_pPresentCoverMe );

	SAFEDELETE( m_pPresentIcon );
	SAFEDELETE( m_pSubIcon );
}

void TotalResultRaidCardBtn::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "CardCover" )
	{
		SAFEDELETE( m_pCardCover );
		m_pCardCover = pImage;
	}
	else if( szType == "PresentCover" )
	{
		SAFEDELETE( m_pPresentCover );
		m_pPresentCover = pImage;
	}
	else if( szType == "PresentCoverOverAdd" )
	{
		SAFEDELETE( m_pPresentCoverOverAdd );
		m_pPresentCoverOverAdd = pImage;
	}
	else if( szType == "PresentCoverMe" )
	{
		SAFEDELETE( m_pPresentCoverMe );
		m_pPresentCoverMe = pImage;
	}
	else
	{
		ioButton::AddRenderImage( szType, pImage );
	}
}

void TotalResultRaidCardBtn::SetName( const ioHashString &rkName )
{
	m_Name = rkName;
}

void TotalResultRaidCardBtn::SetPresent( short iPresentType, int iPresentValue1, int iPresentValue2 )
{
	m_iPresentType	= iPresentType;
	m_iPresentValue1= iPresentValue1;
	m_iPresentValue2= iPresentValue2;

	SAFEDELETE( m_pPresentIcon );
	SAFEDELETE( m_pSubIcon );
	const ioHashString &rkPresentIcon = g_PresentMgr.GetPresentIconText( m_iPresentType, m_iPresentValue1, m_iPresentValue2 );
	m_pPresentIcon = g_UIImageSetMgr.CreateImageByFullName( rkPresentIcon );
	m_pSubIcon = g_UIImageSetMgr.CreateImageByFullName( g_PresentMgr.GetPresentSubIconText( m_iPresentType, m_iPresentValue1, m_iPresentValue2 ) );
}

void TotalResultRaidCardBtn::SetScreenStart()
{
	if( m_Name.IsEmpty() )
		return;

	m_bScreenStart = true;
	m_dwCurrentTime= FRAMEGETTIME();
}

void TotalResultRaidCardBtn::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	if( m_bScreenStart )
	{
		if( FRAMEGETTIME() - m_dwCurrentTime > 200 )
		{
			m_dwCurrentTime = FRAMEGETTIME();
			m_bScreen = !m_bScreen;
		}
	}
}

void TotalResultRaidCardBtn::OnRender()
{
	ioButton::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_iPresentType == 0 )
	{
		if( m_pCardCover )
			m_pCardCover->Render( iXPos + 60, iYPos + 136 );

		if( m_bOver || m_bClicked )
		{
			if( m_pPresentCoverOverAdd )
				m_pPresentCoverOverAdd->Render( iXPos + 60, iYPos + 136, UI_RENDER_ADD );
		}
	}
	else
	{
		OnRenderPresent( iXPos, iYPos );
	}
}

void TotalResultRaidCardBtn::OnRenderPresent( int iXPos, int iYPos )
{
	if( m_Name == g_MyInfo.GetPublicID() )
	{
		if( m_pPresentCoverMe )
			m_pPresentCoverMe->Render( iXPos + 60, iYPos + 136 );
	}
	else if( m_pPresentCover )
		m_pPresentCover->Render( iXPos + 60, iYPos + 136 );

	if( m_pPresentIcon )
	{
		m_pPresentIcon->SetScale( FLOAT1 );
		m_pPresentIcon->Render( iXPos + 63, iYPos + 114, UI_RENDER_NORMAL, TFO_BILINEAR );
	}
	if( m_pSubIcon )
	{
		m_pSubIcon->SetScale( FLOAT1 );
		m_pSubIcon->Render( iXPos + 63, iYPos + 114, UI_RENDER_NORMAL, TFO_BILINEAR );
	}

	char szPresentName1[MAX_PATH] = "";
	sprintf( szPresentName1, g_PresentMgr.GetPresentValue1Text( m_iPresentType, m_iPresentValue1, m_iPresentValue2 ).c_str() );

	char szPresentName2[MAX_PATH] = "";
	sprintf( szPresentName2, g_PresentMgr.GetPresentValue2Text( m_iPresentType, m_iPresentValue1, m_iPresentValue2 ).c_str() );

	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );

	g_FontMgr.PrintTextWidthCut( iXPos + 60, iYPos + 160, FONT_SIZE_14, 117, szPresentName1 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_RED );
	g_FontMgr.PrintTextWidthCut( iXPos + 60, iYPos + 181, FONT_SIZE_14, 117, szPresentName2 );

	if( m_bScreen )
	{
		if( m_pPresentCoverOverAdd )
			m_pPresentCoverOverAdd->Render( iXPos + 60, iYPos + 136, UI_RENDER_ADD );
	}
}

TotalResultRaidCardWnd::TotalResultRaidCardWnd() : m_pPlayStage( NULL )
{
	m_pCardText1	= NULL;
	m_pCardText2	= NULL;
	m_pCardTextNum	= NULL;
	m_pNoneCardText1= NULL;
	m_pNoneCardText2= NULL;

	m_pNameBubble		 = NULL;	
	m_pNameBubbleTail	 = NULL;
	m_pNameBubbleBlue	 = NULL;
	m_pNameBubbleTailBlue= NULL;

	m_bPresentAlarmBlock = false;
	m_iStartTreasureCount	 = 0;	
	m_iCurrentTreasureCount	 = 0;

	m_dwState		= STATE_NONE;
	m_dwCurrentTime = 0;
	m_iCurrentStateXPos = 0;
	m_iCurrentStateYPos = 0;
	m_fCurrentStateRate = 0.0f;
	m_iCurrentStateSec  = 0;

	m_dwStartTime		= 0;
	m_dwStartAlphaTime	= 0;
	m_dwClickTime		= 0;
	m_dwAutoOpenTime	= 0;
	m_dwClearTime		= 0;
}

TotalResultRaidCardWnd::~TotalResultRaidCardWnd()
{
	SAFEDELETE( m_pCardText1 );
	SAFEDELETE( m_pCardText2 );
	SAFEDELETE( m_pCardTextNum );
	SAFEDELETE( m_pNoneCardText1 );
	SAFEDELETE( m_pNoneCardText2 );

	SAFEDELETE( m_pNameBubble );
	SAFEDELETE( m_pNameBubbleTail );
	SAFEDELETE( m_pNameBubbleBlue );
	SAFEDELETE( m_pNameBubbleTailBlue );

	if( !m_BackSound.IsEmpty() )
		g_SoundMgr.StopSound( m_BackSound, 0 );
}

void TotalResultRaidCardWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "NameBubble" )
	{
		SAFEDELETE( m_pNameBubble );
		m_pNameBubble = pFrame;
	}
	else if( szType == "NameBubbleBlue" )
	{
		SAFEDELETE( m_pNameBubbleBlue );
		m_pNameBubbleBlue = pFrame;
	}
	else
	{
		ioWnd::AddRenderFrame( szType, pFrame );
	}
}

void TotalResultRaidCardWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "CardText1" )
	{
		SAFEDELETE( m_pCardText1 );
		m_pCardText1 = pImage;
	}
	else if( szType == "CardText2" )
	{
		SAFEDELETE( m_pCardText2 );
		m_pCardText2 = pImage;
	}
	else if( szType == "CardTextNum" )
	{
		SAFEDELETE( m_pCardTextNum );
		m_pCardTextNum = pImage;
	}
	else if( szType == "NoneCardText1" )
	{
		SAFEDELETE( m_pNoneCardText1 );
		m_pNoneCardText1 = pImage;
	}
	else if( szType == "NoneCardText2" )
	{
		SAFEDELETE( m_pNoneCardText2 );
		m_pNoneCardText2 = pImage;
	}
	else if( szType == "NameBubbleTail" )
	{
		SAFEDELETE( m_pNameBubbleTail );
		m_pNameBubbleTail = pImage;
	}
	else if( szType == "NameBubbleTailBlue" )
	{
		SAFEDELETE( m_pNameBubbleTailBlue );
		m_pNameBubbleTailBlue = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void TotalResultRaidCardWnd::iwm_show()
{
	m_bPresentAlarmBlock = true;

	if( !m_BackSound.IsEmpty() )
		g_SoundMgr.PlaySound( m_BackSound );

	g_GUIMgr.HideWnd( MINIMAP_WND );
	g_GUIMgr.HideWnd( TIME_SCORE_WND );
	g_GUIMgr.HideWnd( SKILL_LIST_WND );
	g_GUIMgr.HideWnd( CENTER_INFO_WND );
	g_GUIMgr.HideWnd( MONSTER_BOSS_GAUGE_WND );
	g_GUIMgr.HideWnd( MODE_RANK_WND );
	g_GUIMgr.HideWnd( KO_CHAT_MSG_WND );
	g_GUIMgr.HideWnd( MINI_SOLDIER_SELECT_WND );
	g_GUIMgr.HideWnd( TRAINING_FUNC_MENU );

	SetAllCardActive( false );
	SetTitleText( "잠시 기다려 주세요" );

	m_dwCurrentTime  = FRAMEGETTIME();
}

void TotalResultRaidCardWnd::iwm_hide()
{
	if( !m_BackSound.IsEmpty() )
		g_SoundMgr.StopSound( m_BackSound, 0 );
}

void TotalResultRaidCardWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	if( COMPARE( dwID, ID_CARD_1, MAX_CARD_SLOT ) )
	{
		if( m_dwState == STATE_CLICK )
		{
			if( cmd == IOBN_BTNUP )
			{
				if( m_iCurrentTreasureCount > 0 )
				{
					SP2Packet kPacket( CTPK_TREASURE_CARD_COMMAND );
					kPacket << TREASURE_CARD_CMD_CLICK << dwID;
					TCPNetwork::SendToServer( kPacket );
					TCPNetwork::MouseBusy( true );
				}
			}
		}
	}
}

void TotalResultRaidCardWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_BackSound = xElement.GetStringAttribute_e( "BackSound" );
	m_AutoOpenSound = xElement.GetStringAttribute_e( "AutoOpenSound" );

	m_dwStartTime		= xElement.GetIntAttribute_e( "StartTime" );
	m_dwStartAlphaTime	= xElement.GetIntAttribute_e( "StartAlphaTime" );
	m_dwClickTime		= xElement.GetIntAttribute_e( "ClickTime" );
	m_dwAutoOpenTime    = xElement.GetIntAttribute_e( "AutoOpenTime" );
	m_dwClearTime		= xElement.GetIntAttribute_e( "ClearTime" );

	m_dwClickRemainTime = m_dwClickTime;
}

bool TotalResultRaidCardWnd::IsPresentAlarmBlock()
{
	if( !m_bPresentAlarmBlock ) 
		return false;

	// 보물카드를 뒤집지 않았으면 블럭
	if( m_dwState != STATE_CLEAR )
		return true;

	m_bPresentAlarmBlock = false;
	return m_bPresentAlarmBlock;
}

void TotalResultRaidCardWnd::SetStartTreasure( int iTreasureCount )
{
	m_iStartTreasureCount = iTreasureCount;
	m_iCurrentTreasureCount = iTreasureCount;

	m_dwState        = STATE_START;

	m_iCurrentStateXPos = 400;
	m_iCurrentStateYPos = 0;
	m_fCurrentStateRate = 0.0f;
}

void TotalResultRaidCardWnd::SetPlayStage( ioPlayStage *pPlayStage )
{
	m_pPlayStage = pPlayStage;
}

void TotalResultRaidCardWnd::SetAllCardScreenStart()
{
	for(int i = ID_CARD_1;i < MAX_CARD_SLOT;i++)
	{
		TotalResultRaidCardBtn *pCardBtn = dynamic_cast< TotalResultRaidCardBtn * >( FindChildWnd( i ) );
		if( !pCardBtn ) 
			continue;

		pCardBtn->SetScreenStart();
	}
}

void TotalResultRaidCardWnd::SetAllCardActive( bool bActive )
{
	for(int i = ID_CARD_1;i < MAX_CARD_SLOT;i++)
	{
		TotalResultRaidCardBtn *pCardBtn = dynamic_cast< TotalResultRaidCardBtn * >( FindChildWnd( i ) );
		if( !pCardBtn ) continue;

		if( bActive )
			pCardBtn->SetActive();
		else
			pCardBtn->SetInActive();
	}
}

void TotalResultRaidCardWnd::TreasureCardTimeOut( SP2Packet &rkPacket )
{
	DWORD dwSeed;
	rkPacket >> dwSeed;
	g_TreasureCardMgr.TreasureCardRandSeed( dwSeed );

	int iOpenCardCount;
	rkPacket >> iOpenCardCount;
	for(int i = 0;i < iOpenCardCount;i++)
	{
		DWORD dwID;
		ioHashString rkName;
		short iPresentType;
		int iPresentValue1, iPresentValue2;
		rkPacket >> dwID >> rkName >> iPresentType >> iPresentValue1 >> iPresentValue2;

		TotalResultRaidCardBtn *pCardBtn = dynamic_cast< TotalResultRaidCardBtn * >( FindChildWnd( dwID ) );
		if( pCardBtn )
		{
			pCardBtn->SetPresent( iPresentType, iPresentValue1, iPresentValue2 );
			pCardBtn->SetName( rkName );
			pCardBtn->SetInActive();
		}
	}

	m_dwState       = STATE_AUTO_OPEN;
	m_dwCurrentTime = FRAMEGETTIME();

	int iEmptyCardCount = GetEmptyCardCount() + 3;
	m_dwAutoOpenTime = iEmptyCardCount * m_dwAutoOpenTime;
	if( m_dwAutoOpenTime == 0 )
	{
		m_dwState       = STATE_CLEAR;
		m_dwCurrentTime = FRAMEGETTIME();
	}
	else
	{
		m_dwCurrentTimeTick = FRAMEGETTIME();
		m_dwTickTime        = (float)m_dwAutoOpenTime / iEmptyCardCount;
	}

	if( !m_AutoOpenSound.IsEmpty() )
		g_SoundMgr.PlaySound( m_AutoOpenSound );
}

void TotalResultRaidCardWnd::TreasureCardClick( SP2Packet &rkPacket )
{
	DWORD dwID;
	rkPacket >> dwID;

	if( dwID == 0 )
	{
		TCPNetwork::MouseBusy( false );
		return;
	}

	ioHashString rkName;
	short iPresentType;
	int iPresentValue1, iPresentValue2, iTreasureCount;
	rkPacket >> rkName >> iTreasureCount >> iPresentType >> iPresentValue1 >> iPresentValue2;

	TotalResultRaidCardBtn *pCardBtn = dynamic_cast< TotalResultRaidCardBtn * >( FindChildWnd( dwID ) );
	if( pCardBtn )
	{
		pCardBtn->SetPresent( iPresentType, iPresentValue1, iPresentValue2 );
		pCardBtn->SetName( rkName );
		pCardBtn->SetInActive();
	}

	if( rkName == g_MyInfo.GetPublicID() )
	{
		m_iCurrentTreasureCount = iTreasureCount;
		if( m_iCurrentTreasureCount <= 0 )
		{
			// 더 이상 클릭을 할 수없으면 모든 카드 클릭 금지
			SetAllCardActive( false );
		}

		TCPNetwork::MouseBusy( false );
	}

	int iSize = m_FinalInfoList.size();
	for(int i = 0;i < iSize;i++)
	{
		FinalInfo &rkInfo = m_FinalInfoList[i];
		if( rkInfo.m_szName == rkName )
		{
			rkInfo.m_iTreasureCardCnt = max( 0, rkInfo.m_iTreasureCardCnt - 1 );
			break;
		}
	}

	// 남은 카드 없으면 모두 뒤집기
	if( GetTotalRemainCardClickCount() == 0 )
	{
		if( m_dwState == STATE_CLICK )
		{
			m_dwClickRemainTime = m_dwClickTime - min( m_dwClickTime, FRAMEGETTIME() - m_dwCurrentTime );
		}

		SendTimeOut();
	}
}

int TotalResultRaidCardWnd::GetTotalRemainCardClickCount()
{
	int iResult = 0;
	int iSize = m_FinalInfoList.size();
	for(int i = 0;i < iSize;i++)
	{
		FinalInfo &rkInfo = m_FinalInfoList[i];
		iResult += rkInfo.m_iTreasureCardCnt;
	}
	return iResult;
}

void TotalResultRaidCardWnd::AddFinalInfo( const FinalInfo &rkInfo )
{
	m_FinalInfoList.push_back( rkInfo );
}

int TotalResultRaidCardWnd::GetEmptyCardCount()
{
	int iReturn = 0;
	for(int i = ID_CARD_1;i < MAX_CARD_SLOT;i++)
	{
		TotalResultRaidCardBtn *pCardBtn = dynamic_cast< TotalResultRaidCardBtn * >( FindChildWnd( i ) );
		if( !pCardBtn ) continue;
		if( !pCardBtn->IsEmptyCard() ) continue;

		iReturn++;
	}
	return iReturn;
}

void TotalResultRaidCardWnd::ProcessRandomCard( bool bOneCard )
{
	if( m_pPlayStage )
	{
		for(int i = ID_CARD_1;i < MAX_CARD_SLOT;i++)
		{
			TotalResultRaidCardBtn *pCardBtn = dynamic_cast< TotalResultRaidCardBtn * >( FindChildWnd( i ) );
			if( !pCardBtn ) continue;
			if( !pCardBtn->IsEmptyCard() ) continue;

			short iPresentType;
			int iPresentValue1, iPresentValue2;
			m_pPlayStage->TreasureCardRandom( iPresentType, iPresentValue1, iPresentValue2 );
			pCardBtn->SetPresent( iPresentType, iPresentValue1, iPresentValue2 );
			pCardBtn->SetInActive();
			if( bOneCard )
				break;
		}
	}
}

void TotalResultRaidCardWnd::SendTimeOut()
{
	m_dwState       = STATE_DELAY;
	m_dwCurrentTime = FRAMEGETTIME();

	SetTitleText( "잠시 기다려 주세요" );
	SP2Packet kPacket( CTPK_TREASURE_CARD_COMMAND );
	kPacket << TREASURE_CARD_CMD_TIME_OUT;
	TCPNetwork::SendToServer( kPacket );

	TotalResultMainWnd* pMainWnd = dynamic_cast<TotalResultMainWnd*>( m_pParent );
	if( pMainWnd )
		pMainWnd->ShowSpiritRewardWnd( true );
}

void TotalResultRaidCardWnd::ProcessState()
{
	if( m_dwCurrentTime == 0 ) return;

	DWORD dwGapTime = FRAMEGETTIME() - m_dwCurrentTime;
	switch( m_dwState )
	{
	case STATE_START:
		if( dwGapTime > m_dwStartTime )
		{
			if( GetTotalRemainCardClickCount() == 0 )
			{
				SendTimeOut();   //즉시 종료
				m_dwClickRemainTime = m_dwClickTime;
			}
			else
			{
				SetAllCardActive( true );
				m_dwState          = STATE_CLICK;
				m_dwCurrentTime    = FRAMEGETTIME();

				m_iCurrentStateSec = (float)m_dwClickTime / 1000;

				char szTitle[MAX_PATH] = "";
				if( m_iCurrentTreasureCount == 0 )
					sprintf( szTitle, "잠시 기다려 주세요 . . %d", m_iCurrentStateSec );
				else
					sprintf( szTitle, "보물 카드를 %d개 선택해 주세요 . . %d", m_iStartTreasureCount, m_iCurrentStateSec );

				SetTitleText( szTitle );
			}
		}
		else
		{
			if( dwGapTime <= m_dwStartAlphaTime )
			{				
				float fTimeRate = (float)dwGapTime / m_dwStartAlphaTime;
				m_fCurrentStateRate = sinf( ( D3DX_PI / 2 ) * fTimeRate );
				m_iCurrentStateXPos = 400 - ( 400 * m_fCurrentStateRate );
			}
		}
		break;
	case STATE_CLICK:
		if( dwGapTime > m_dwClickTime )
		{
			SendTimeOut();
		}
		else
		{
			int iSec = ( (float)( m_dwClickTime - dwGapTime ) / 1000 ) + 1;
			if( m_iCurrentStateSec != iSec )
			{
				m_iCurrentStateSec = max( 0, iSec );
				char szTitle[MAX_PATH] = "";
				if( m_iCurrentTreasureCount == 0 )
				{
					sprintf( szTitle, "잠시 기다려 주세요 . . %d", m_iCurrentStateSec );
				}
				else
				{
					sprintf( szTitle, "보물 카드를 %d개 선택해 주세요 . . %d", m_iStartTreasureCount, m_iCurrentStateSec );
				}
				SetTitleText( szTitle );
			}
		}
		break;
	case STATE_DELAY:
		if( dwGapTime > 5000 )
		{
			m_dwState       = STATE_AUTO_OPEN;
			m_dwCurrentTime = FRAMEGETTIME();

			g_TreasureCardMgr.TreasureCardRandSeed( g_BattleRoomMgr.GetIndex() );
			int iEmptyCardCount = GetEmptyCardCount() + 3;  
			m_dwAutoOpenTime = iEmptyCardCount * m_dwAutoOpenTime;
			if( m_dwAutoOpenTime == 0 )
			{
				m_dwState       = STATE_CLEAR;
				m_dwCurrentTime = FRAMEGETTIME();
			}
			else
			{
				m_dwCurrentTimeTick = FRAMEGETTIME();
				m_dwTickTime        = (float)m_dwAutoOpenTime / iEmptyCardCount;
			}
		}
		break;
	case STATE_AUTO_OPEN:
		if( dwGapTime > m_dwAutoOpenTime + m_dwClickRemainTime )
		{
			m_dwState       = STATE_CLEAR;
			m_dwCurrentTime = FRAMEGETTIME();
			ProcessRandomCard( false );
			SetAllCardScreenStart();
		}
		else
		{
			if( FRAMEGETTIME() - m_dwCurrentTimeTick > m_dwTickTime )
			{
				m_dwCurrentTimeTick = FRAMEGETTIME();
				ProcessRandomCard( true );
			}
		}
		break;
	case STATE_CLEAR:
		if( dwGapTime > m_dwClearTime )
		{
			m_dwCurrentTime = 0;
			if( GetParent() )
				GetParent()->iwm_command( this, STATE_CLEAR, 0 );
		}
		break;
	}
}

void TotalResultRaidCardWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	ProcessState();
}

void TotalResultRaidCardWnd::OnRenderAfterChild()
{
	ioWnd::OnRenderAfterChild();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	OnRenderStartMent( iXPos, iYPos );

	for(int i = ID_CARD_1;i < MAX_CARD_SLOT;i++)
	{
		TotalResultRaidCardBtn *pCardBtn = dynamic_cast< TotalResultRaidCardBtn * >( FindChildWnd( i ) );
		if( !pCardBtn ) 
			continue;

		if( pCardBtn->GetName().IsEmpty() ) 
			continue;

		OnRenderBubbleName( pCardBtn->GetName(), pCardBtn->GetDerivedPosX() + 11, pCardBtn->GetDerivedPosY() + 46 );
	}
}

void TotalResultRaidCardWnd::OnRenderStartMent( int iXPos, int iYPos )
{
	if( m_dwState != STATE_START ) return;

	iXPos += m_iCurrentStateXPos;

	if( m_iStartTreasureCount == 0 )
	{
		if( !m_pNoneCardText1 || !m_pNoneCardText2 ) return;

		m_pNoneCardText1->SetAlpha( MAX_ALPHA_RATE * m_fCurrentStateRate );
		m_pNoneCardText1->Render( iXPos + 85, iYPos + 141 );

		m_pNoneCardText2->SetAlpha( MAX_ALPHA_RATE * m_fCurrentStateRate );
		m_pNoneCardText2->Render( iXPos + 85, iYPos + 201 );
	}
	else
	{
		if( !m_pCardText1 || !m_pCardText2 || !m_pCardTextNum ) return;

		m_pCardText1->SetAlpha( MAX_ALPHA_RATE * m_fCurrentStateRate );
		m_pCardText1->Render( iXPos + 121, iYPos + 141 );

		m_pCardTextNum->SetAlpha( MAX_ALPHA_RATE * m_fCurrentStateRate );
		m_pCardTextNum->RenderNum( iXPos + 93, iYPos + 201, m_iStartTreasureCount );

		m_pCardText2->SetAlpha( MAX_ALPHA_RATE * m_fCurrentStateRate );
		m_pCardText2->Render( iXPos + 132, iYPos + 201 );
	}
}

void TotalResultRaidCardWnd::OnRenderBubbleName( const ioHashString &rkName, int iXPos, int iYPos )
{	
	if( rkName == g_MyInfo.GetPublicID() )
	{
		if( m_pNameBubbleBlue && m_pNameBubbleTailBlue )
		{
			m_pNameBubbleBlue->Render( iXPos, iYPos );
			m_pNameBubbleTailBlue->Render( iXPos + 44, iYPos + 20 );
		}

		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
		g_FontMgr.PrintTextWidthCut( iXPos + 50, iYPos + 4, FONT_SIZE_12, 95, rkName.c_str() );
	}
	else
	{
		if( m_pNameBubble && m_pNameBubbleTail )
		{
			m_pNameBubble->Render( iXPos, iYPos );
			m_pNameBubbleTail->Render( iXPos + 44, iYPos + 20 );
		}

		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY);
		g_FontMgr.PrintTextWidthCut( iXPos + 50, iYPos + 4, FONT_SIZE_12, 95, rkName.c_str() );
	}
}
//////////////////////////////////////////////////////////////////////////
TotalResultSpiritRewardWnd::TotalResultSpiritRewardWnd()
{
	m_iSpiritCode = 0;
	m_iCount = 0;

	m_pIcon = NULL;
	m_pEffect = NULL;
}

TotalResultSpiritRewardWnd::~TotalResultSpiritRewardWnd()
{
	SAFEDELETE( m_pIcon );
}

void TotalResultSpiritRewardWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_szDescText[0] = xElement.GetStringAttribute_e( "DescText1" );
	m_szDescText[1] = xElement.GetStringAttribute_e( "DescText2" );
}

void TotalResultSpiritRewardWnd::iwm_show()
{
	m_pEffect = GetEffect("Effect");
	if( m_pEffect )
	{
		if( !m_pEffect->IsLive() )
			m_pEffect->Start();

		m_pEffect->SetPosX( 105 - m_pEffect->GetWidth() / 2 );
		m_pEffect->SetPosY( 28 - m_pEffect->GetHeight() / 2 );
	}
}

void TotalResultSpiritRewardWnd::iwm_hide()
{
	if( m_pEffect )
		m_pEffect->Stop();
}

void TotalResultSpiritRewardWnd::SetInfo( int iCode, int iCount )
{
	m_iSpiritCode = iCode;
	m_iCount = iCount;

	SAFEDELETE( m_pIcon );
	m_pIcon = g_UIImageSetMgr.CreateImageByFullName( g_SpiritMgr.GetIconName( m_iSpiritCode ) );
	if( m_pIcon )
		m_pIcon->SetScale( 0.85f );

	ShowWnd();
}

void TotalResultSpiritRewardWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_pIcon )
		m_pIcon->Render( iXPos + 19, iYPos + 22 );

	g_FontMgr.SetVertAlign( TVA_TOP );
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 61, iYPos + 13, FONT_SIZE_13, m_szDescText[0].c_str() );

	g_FontMgr.SetTextColor( TCT_DEFAULT_BLUE );
	char szKey[MAX_PATH] = "";
	sprintf( szKey, m_szDescText[1].c_str(), g_SpiritMgr.GetName(m_iSpiritCode).c_str(), m_iCount );
	g_FontMgr.PrintText( iXPos + 61, iYPos + 31, FONT_SIZE_13, szKey );
}