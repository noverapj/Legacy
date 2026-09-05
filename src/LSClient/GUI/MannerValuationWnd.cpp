#include "StdAfx.h"

#include "../ioGuildMarkMgr.h"
#include "../ioLadderTeamMgr.h"
#include "../ioTournamentManager.h"
#include "../ioShuffleRoomMgr.h"
#include "../ioSoldierAwakeManager.h"
#include "../ioPowerUpManager.h"
#include "../ioRSoldierInfoManager.h"

#include "GuildWnd.h"
#include "MannerValuationWnd.h"
#include "MessengerWnd.h"
#include "VoiceChatWnd.h"
#include "KickUserVoteWnd.h"
#include "PlazaMainWnd.h"
#include "HeroMainWnd.h"
#include "UISoldierSelectWnd.h"
#include "NamedTitleBtn.h"
#include "NamedTitleSelectWnd.h"

MannerTrialWnd::MannerTrialWnd() :m_pPreEdit( NULL )
{
	m_szTargetID.Clear();
	m_szChatList.Clear();
	m_bChatHaveID = false;
	m_eTrialType  = ioMannerTrialChatManager::TT_NONE;
	m_dwChannelIndex     = 0;
	m_bVoiceChat  = false;
}

MannerTrialWnd::~MannerTrialWnd()
{
	m_szTargetID.Clear();
	m_szChatList.Clear();
}

bool MannerTrialWnd::OnMinusMannerAndTrial()
{
	ioHashString szReason;
	GetReasonString( szReason );

	MannerTrialInfo::ReturnType eReturnType = MannerTrialInfo::RT_OK;
	MannerTrialInfo *pMannerTrialInfo = g_MyInfo.GetMannerTrialInfo();
	if( pMannerTrialInfo )
	{
		pMannerTrialInfo->Load();
		pMannerTrialInfo->UpdateDate();
		eReturnType = pMannerTrialInfo->CheckRight( m_szTargetID );
	}

	enum { MAX_REASON_LEN = 10, };
	if( szReason.IsEmpty() || szReason.Length() <= MAX_REASON_LEN )
	{
		int iLetterLen = MAX_REASON_LEN/2;
		if( !szReason.IsEmpty() )
		{

#if defined( SRC_OVERSEAS )

#if defined( MULTI_BYTE_CHECK )
			if( !IsDBCSLeadByteEx( COUNTRY_CODE_PAGE, (BYTE) szReason.At(0) ) )
#else
			if( true || !IsDBCSLeadByteEx( COUNTRY_CODE_PAGE, (BYTE) szReason.At(0) ) )
#endif

#else
			if( !IsDBCSLeadByte( (BYTE) szReason.At(0) ) )
#endif

				iLetterLen = MAX_REASON_LEN;
		}
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1), iLetterLen );
		return false;
	}

	if( g_MyInfo.GetPublicID() == m_szTargetID )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
		return true;
	}
	else if( !g_App.IsCanMannerTrialTime() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4) );
		return true;
	}
	else if(  eReturnType != MannerTrialInfo::RT_OK )
	{
		if( !g_App.IsMeDeveloper() && !g_App.IsMeGameMaster() )
		{
			if(  eReturnType == MannerTrialInfo::RT_ERROR_MAX )
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(10), MannerTrialInfo::MAX_TRAIL );
			else if(  eReturnType == MannerTrialInfo::RT_ERROR_ID )
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(11) );
			else
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(12) );
			return false;
		}
	}

	SP2Packet kPacket( CTPK_TRIAL );
	kPacket << m_szTargetID;                                        //신고 당하는 유저 닉네임    
	kPacket << m_szChatList;                                        //최근 20줄의 채팅
	kPacket << GetTypeString();                                     //신고하기 타입
	kPacket << (int) m_eTrialType;
	if( m_eTrialType == ioMannerTrialChatManager::TT_CHANNEL_CHAT )
		kPacket << m_dwChannelIndex;
	g_BattleRoomMgr.FillBattleRoomInfo( kPacket );                  //내가 속해있는 전투방 유저 리스트
	kPacket << g_App.GetUserGlobalIP( m_szTargetID );               //신고 당하는 유저 외부 아이피
	kPacket << szReason;
	TCPNetwork::SendToServer( kPacket );
	g_App.SetMannerTrialTime();

	if( pMannerTrialInfo )
		pMannerTrialInfo->AddTrailInfo( m_szTargetID );

	// 신고완료멘트 ( 서버전송 및 insert 쿼리 호출이므로 굳이 서버응답 확인 필요 없음. )
	ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];

	kPrinter[0].SetTextStyle( TS_NORMAL );
	kPrinter[0].SetBkColor( 0, 0, 0 );
	kPrinter[0].SetTextColor( TCT_DEFAULT_RED );
	kPrinter[0].AddTextPiece( FONT_SIZE_13, m_szTargetID.c_str() );
	kPrinter[0].SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter[0].AddTextPiece( FONT_SIZE_13, STR(5) );

	kPrinter[1].SetTextStyle( TS_NORMAL );
	kPrinter[1].SetBkColor( 0, 0, 0 );
	kPrinter[1].SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter[1].AddTextPiece( FONT_SIZE_13, STR(6) );

	kPrinter[2].SetTextStyle( TS_NORMAL );
	kPrinter[2].SetBkColor( 0, 0, 0 );
	kPrinter[2].SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter[2].AddTextPiece( FONT_SIZE_13, STR(7) );

	kPrinter[3].SetTextStyle( TS_NORMAL );
	kPrinter[3].SetBkColor( 0, 0, 0 );
	kPrinter[3].SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter[3].AddTextPiece( FONT_SIZE_13, STR(8) );

	kPrinter[4].SetTextStyle( TS_NORMAL );
	kPrinter[4].SetBkColor( 0, 0, 0 );
	kPrinter[4].SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter[4].AddTextPiece( FONT_SIZE_13, " " );

	kPrinter[5].SetTextStyle( TS_NORMAL );
	kPrinter[5].SetBkColor( 0, 0, 0 );
	kPrinter[5].SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter[5].AddTextPiece( FONT_SIZE_13, STR(9) );

	g_GUIMgr.SetPrevMsgListBox( NULL, MB_OK, NULL, kPrinter );
	return true;
}

void MannerTrialWnd::SetMannerTrialInfo( const ioHashString &szID , ioMannerTrialChatManager::TrialType eType, DWORD dwChannelIndex )
{
	m_szTargetID		= szID;
	m_eTrialType		= eType;
	m_dwChannelIndex    = dwChannelIndex;
}

void MannerTrialWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetTextColor( TCT_DEFAULT_RED );
	g_FontMgr.PrintTextWidthCut( iXPos + 17, iYPos + 53, FONT_SIZE_13, 340.0f, "신고대상자:%s:", m_szTargetID.c_str() );
	g_FontMgr.SetTextColor( TCT_DEFAULT_BLUE );
	g_FontMgr.PrintTextWidthCut( iXPos + 17, iYPos + 71, FONT_SIZE_13, 340.0f, "신고접수자:%s:", g_MyInfo.GetPublicID().c_str() );

	enum { MAX_LINE = 5, };
	ioComplexStringPrinter kPrinter[MAX_LINE];
	kPrinter[0].SetTextStyle( TS_NORMAL );
	kPrinter[0].SetTextColor( 0, 0, 0 );
	kPrinter[0].SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter[0].AddTextPiece( FONT_SIZE_13, "욕설,불법프로그램 사용자 등 비매너 행위에 대해 신고가 가능합니다." );
	kPrinter[0].PrintFullText(  iXPos + 17, iYPos + 96, TAT_LEFT );

	kPrinter[1].SetTextStyle( TS_NORMAL );
	kPrinter[1].SetTextColor( 0, 0, 0 );
	kPrinter[1].SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter[1].AddTextPiece( FONT_SIZE_13, "신고 시 " );
	kPrinter[1].SetTextColor( TCT_DEFAULT_RED );
	kPrinter[1].AddTextPiece( FONT_SIZE_13, "신고대상자의 모든 데이터가 저장되고, 게임 데이터 검토가 진행 되어" );
	kPrinter[1].PrintFullText(  iXPos + 17, iYPos + 114, TAT_LEFT );

	kPrinter[2].SetTextStyle( TS_NORMAL );
	kPrinter[2].SetTextColor( 0, 0, 0 );
	kPrinter[2].SetTextColor( TCT_DEFAULT_RED );
	kPrinter[2].AddTextPiece( FONT_SIZE_13, "운영정책에 의거 제재 조치 " );
	kPrinter[2].SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter[2].AddTextPiece( FONT_SIZE_13, "됩니다. " );
	kPrinter[2].SetTextColor( TCT_DEFAULT_BLUE );
	kPrinter[2].AddTextPiece( FONT_SIZE_13, "만약, 허위 신고 또는 신고 악용일 경우 최대" );
	kPrinter[2].PrintFullText(  iXPos + 17, iYPos + 132, TAT_LEFT );

	kPrinter[3].SetTextStyle( TS_NORMAL );
	kPrinter[3].SetTextColor( 0, 0, 0 );
	kPrinter[3].SetTextColor( TCT_DEFAULT_BLUE );
	kPrinter[3].AddTextPiece( FONT_SIZE_13, "제한 90일 제재 조치" );
	kPrinter[3].SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter[3].AddTextPiece( FONT_SIZE_13, "를 받을 수 있으니 신중히 신고해 주시기 바랍니다." );
	kPrinter[3].PrintFullText(  iXPos + 17, iYPos + 150, TAT_LEFT );

	kPrinter[4].SetTextStyle( TS_NORMAL );
	kPrinter[4].SetTextColor( 0, 0, 0 );
	kPrinter[4].SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter[4].AddTextPiece( FONT_SIZE_13, "신고 이유를 간략히 적어 주세요. (예: 불법프로그램 사용자 신고)" );
	kPrinter[4].PrintFullText(  iXPos + 17, iYPos + 179, TAT_LEFT );
}

void MannerTrialWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
	case ID_CANCEL:
		if( cmd == IOBN_BTNUP )
			HideWnd();
		break;
	case ID_TRIAL:
		if( cmd == IOBN_BTNUP)
		{
			if( OnMinusMannerAndTrial() )
				HideWnd();
		}
		break;
	case ID_EDIT_REASON_1:
		if( cmd == IOED_INPUT )
		{
			if( param == 0 || param == 1 )
			{
				if( OnMinusMannerAndTrial() )
					HideWnd();
			}					
		}
		break;
	}
}

void MannerTrialWnd::iwm_show()
{
	if( m_szTargetID.IsEmpty() )
		HideWnd();

	m_pPreEdit = ioEdit::m_pKeyFocusEdit;

	ioEdit *pEdit = dynamic_cast< ioEdit* > ( FindChildWnd( ID_EDIT_REASON_1 ) );
	if( pEdit )
	{
		pEdit->ClearString();
		pEdit->SetKeyFocus();
		g_InputBox.SetString( "" );
	}

	m_szChatList.Clear();
	m_bChatHaveID = false;

	m_bChatHaveID = g_MannerTrialChatMgr.IsHaveID( m_eTrialType, m_szTargetID );
	if( m_bChatHaveID )
		g_MannerTrialChatMgr.GetChatList( m_eTrialType, m_szChatList );
	
	if( g_VoiceChat.IsSoundPlayer( ioHashStringVC( m_szTargetID.c_str() ) ) || 
		g_VoiceOffListManager.IsHave( m_szTargetID ) )
	{
		m_bChatHaveID = true;
		m_bVoiceChat  = true;
	}
	else
		m_bVoiceChat = false;
}

void MannerTrialWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	g_MannerTrialChatMgr.SetMaxChat( xElement.GetIntAttribute_e( "MaxMannerTrialChat" ) );
}

void MannerTrialWnd::iwm_hide()
{
	ioEdit *pEdit = dynamic_cast< ioEdit* > ( FindChildWnd( ID_EDIT_REASON_1 ) );
	if( pEdit )
	{
		pEdit->ClearString();
		pEdit->KillKeyFocus();		
	}

	if( m_pPreEdit )	
	{
		ioWnd *pParent = m_pPreEdit->GetParent();
		if( pParent )
		{
			if( pParent->IsShow() )
				m_pPreEdit->SetKeyFocus();	
		}
	}	
	m_pPreEdit = NULL;

	m_eTrialType = ioMannerTrialChatManager::TT_NONE;
	m_szTargetID.Clear();
	m_szChatList.Clear();
	m_bChatHaveID = false;
}

void MannerTrialWnd::GetReasonString( OUT ioHashString &rszReason )
{
	std::string szTempReason;
	ioEdit *pEdit = dynamic_cast<ioEdit*> ( FindChildWnd( ID_EDIT_REASON_1 ) );
	if( pEdit )
	{
		szTempReason = pEdit->GetText();
	}

	if( !szTempReason.empty() )
		rszReason = szTempReason.c_str();
}

const ioHashString MannerTrialWnd::GetTypeString()
{
	std::string szReturn = g_MannerTrialChatMgr.GetTrialTypeName( m_eTrialType );
	if( m_bVoiceChat )
		szReturn += STR(1);

	return szReturn.c_str();
}

void MannerTrialWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return ;
	ioMovingWnd::OnProcess( fTimePerSec );
}
//////////////////////////////////////////////////////////////////////////
MannerValuationSlotBtn::MannerValuationSlotBtn()
{
	m_pBlueNor		= NULL;
	m_pBlueOver		= NULL;
	m_pBlueNorGra	= NULL;
	m_pBlueOverGra	= NULL;
	m_pLoadCharBack = NULL;
	for(int i = 0;i < MAX_CHAR_LODING;i++)
		m_pCharLoading[i] = NULL;
	m_pRentalMark   = NULL;

	m_pUI3DRender   = NULL;
	m_aMyChar	    = NULL;
	m_pRentalSetBtn = NULL;
	m_fAlphaRate    = 0.0f;

	m_dwCharLoadTime = 0;	
	m_iCharLoadArray = 0;
	m_dwCharAniDelayTime = 0;

	m_bOnline	= false;
	m_pUserInfo = NULL;
	m_pPlayStage= NULL;
	m_iListArray= 0;
}

MannerValuationSlotBtn::~MannerValuationSlotBtn()
{
	SAFEDELETE( m_pBlueNor );
	SAFEDELETE( m_pBlueOver );
	SAFEDELETE( m_pBlueNorGra );
	SAFEDELETE( m_pBlueOverGra );
	SAFEDELETE( m_pLoadCharBack );
	for(int i = 0;i < MAX_CHAR_LODING;i++)
		SAFEDELETE( m_pCharLoading[i] );
	SAFEDELETE( m_pRentalMark );

	SAFEDELETE( m_aMyChar );
	SAFEDELETE( m_pUI3DRender );
}

void MannerValuationSlotBtn::iwm_create()
{
	SAFEDELETE( m_pUI3DRender );

	m_pUI3DRender = new ioUI3DRender;
	m_pUI3DRender->Initialize( "config/sp2_stage_charselect_map.ini", "battle_cam" );
}

void MannerValuationSlotBtn::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
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
	else if( szType == "RentalMark" )
	{
		SAFEDELETE( m_pRentalMark );
		m_pRentalMark = pImage;
	}
	else
		ioButton::AddRenderImage( szType, pImage );
}

void MannerValuationSlotBtn::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
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
	else
		ioButton::AddRenderFrame( szType, pFrame );
}

void MannerValuationSlotBtn::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_RENTAL:
		if( cmd == IOBN_BTNUP )
		{
			SendRentalRequest();
		}
		break;
	case ID_EXPERIENCE:
		if( cmd == IOBN_BTNUP )
		{
			SendExperience();
		}
		break;
	case ID_SET_RENTAL:
		if( cmd == IOBN_BTNUP )
		{
			SendRentalSet( CRT_RENTAL );
		}
		break;
	case ID_DEL_RENTAL:
		if( cmd == IOBN_BTNUP )
		{
			SendRentalSet( CRT_GENERAL );
		}
		break;
	}
}

void MannerValuationSlotBtn::SendRentalRequest()
{
	if( m_pUserInfo == NULL ) return;

	if( g_MyInfo.GetPublicID() == m_pUserInfo->GetUserID() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1), Help::GetCharRentalMinute() );	
	}
	else
	{
		FriendNode *pFriendNode = g_FriendManager.GetNode( m_pUserInfo->GetUserID() );
		if( pFriendNode == NULL || pFriendNode->IsOtherBF() == false )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );	
			return;
		}

		int iEmptySlot = g_MyInfo.GetCharEmptyFirstSlot();
		if( iEmptySlot == -1 )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );	
			return;
		}

		UserInfoCharData &rkUserCharData = m_pUserInfo->GetUserCharData();
		CharInfoData &rkChar = rkUserCharData.GetSlotChar( m_iListArray );
		if( rkChar.m_CharInfo.m_class_type == 0 )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4) );	
			return;
		}

		if( rkChar.m_CharInfo.m_sRentalType != CRT_RENTAL )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(5) );	
			return;
		}

		if( rkChar.m_CharInfo.m_dwRentalMinute != 0 )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(6) );	
			return;
		}

		DWORD dwRequestTime = FRAMEGETTIME() - rkChar.m_dwRentalRequestTime;
		if( dwRequestTime < 5000 )
		{
			int iSec = max( 1, (float)( 5000 - dwRequestTime ) / 1000 );
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(7), g_MyInfo.GetClassName( rkChar.m_CharInfo.m_class_type, rkChar.m_CharInfo.m_eCharPowerUpType ), iSec );
			return;
		}

		if( g_MyInfo.GetCharIndexByClassType( rkChar.m_CharInfo.m_class_type ) != 0 )
		{			
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(8), g_MyInfo.GetClassName( rkChar.m_CharInfo.m_class_type, rkChar.m_CharInfo.m_eCharPowerUpType ) );
			return;
		}
        
		SP2Packet kPacket( CTPK_USER_CHAR_RENTAL_REQUEST );
		kPacket << m_pUserInfo->GetUserID() << rkChar.m_CharInfo.m_class_type;
		TCPNetwork::SendToServer( kPacket );
		TCPNetwork::MouseBusy( true );
	}
}

void MannerValuationSlotBtn::SendExperience()
{
	if( m_pUserInfo == NULL ) return;

	if( g_MyInfo.GetPublicID() == m_pUserInfo->GetUserID() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );	
	}
	else
	{
		if( m_pPlayStage == NULL || ( m_pPlayStage->GetModeType() != MT_TRAINING && m_pPlayStage->GetModeType() != MT_HEADQUARTERS && m_pPlayStage->GetModeType() != MT_HOUSE ) )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );	
			return;
		}

		if( m_pPlayStage->GetBaseChar( m_pUserInfo->GetUserID() ) == NULL )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );	
			return;
		}

		ioBaseChar *pOwnerChar = m_pPlayStage->GetOwnerChar();
		if( !pOwnerChar )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4) );	
			return;
		}

		UserInfoCharData &rkUserCharData = m_pUserInfo->GetUserCharData();
		CharInfoData &rkChar = rkUserCharData.GetSlotChar( m_iListArray );
		if( rkChar.m_CharInfo.m_class_type == 0 )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(5) );	
			return;
		}

		// 2019-07-15
		if( true == pOwnerChar->IsChangeWaitState() )
		{
			return;
		}

		pOwnerChar->SetExperienceMode( EMS_CHANGE_EXPERIENCE, m_pUserInfo->GetUserID(), rkChar.m_CharInfo.m_class_type );
		g_GUIMgr.HideWnd( MANNER_VALUATION_WND );
	}
}

void MannerValuationSlotBtn::SendRentalSet( short sRentalType )
{
	if( m_pUserInfo == NULL ) return;

	if( g_MyInfo.GetPublicID() != m_pUserInfo->GetUserID() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );		
	}
	else
	{
		UserInfoCharData &rkUserCharData = m_pUserInfo->GetUserCharData();
		CharInfoData &rkChar = rkUserCharData.GetSlotChar( m_iListArray );
		
		DWORD dwCharIndex = g_MyInfo.GetCharIndexByClassType( rkChar.m_CharInfo.m_class_type );
		if( dwCharIndex == 0 )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );	
			return;
		}

		switch( sRentalType )
		{
		case CRT_RENTAL:
			if( g_MyInfo.GetCharRentalSetCount() >= Help::GetMaxCharRentSet() )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3), Help::GetMaxCharRentSet() );	
				return;
			}
			else if( m_pPlayStage == NULL )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4) );	
				return;
			}			
			else
			{
				ioBaseChar *pOwnerChar = m_pPlayStage->GetOwnerChar();
				if( pOwnerChar )
				{
					if( pOwnerChar->GetCharacterInfo().m_class_type == rkChar.m_CharInfo.m_class_type )
					{
						g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(5) );	
						return;
					}
				}

				bool bNotRental = false;
				if( g_MyInfo.GetGradeLevel() < Help::GetCharRentalGrade() ||
					rkChar.m_CharInfo.m_ePeriodType != CPT_MORTMAIN ||
					rkChar.m_CharInfo.m_chExerciseStyle != EXERCISE_NONE ||
					g_MyInfo.IsCharEquipExtraItemPeriodCheck( rkChar.m_CharInfo.m_class_type, EPT_TIME ) ||
					g_MyInfo.IsCharEquipExtraItemPeriodCheck( rkChar.m_CharInfo.m_class_type, EPT_GROW_TIME ) ||
					g_MyInfo.IsTimeGrowthCharCheck( rkChar.m_CharInfo.m_class_type ) )
				{
					bNotRental = true;
				}

				if( bNotRental )
				{
					ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
					kPrinter[0].SetTextStyle( TS_NORMAL );
					kPrinter[0].SetBkColor( 0, 0, 0 );	
					kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
					kPrinter[0].AddTextPiece( FONT_SIZE_13, STR(6) );

					kPrinter[1].SetTextStyle( TS_NORMAL );
					kPrinter[1].SetBkColor( 0, 0, 0 );	
					kPrinter[1].SetTextColor( TCT_DEFAULT_GREEN );
					kPrinter[1].AddTextPiece( FONT_SIZE_13, STR(7) );

					kPrinter[2].SetTextStyle( TS_NORMAL );
					kPrinter[2].SetBkColor( 0, 0, 0 );	
					kPrinter[2].SetTextColor( TCT_DEFAULT_DARKGRAY );
					kPrinter[2].AddTextPiece( FONT_SIZE_13, STR(8) );
					g_GUIMgr.SetPrevMsgListBoxWithTitle( NULL, MB_OK, NULL, kPrinter, STR(9), STR(10), "", "" );
					return;
				}
			}
			break;
		case CRT_GENERAL:
			{
				if( rkChar.m_CharInfo.m_sRentalType != CRT_RENTAL )
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(11) );	
					return;
				}

				if( rkChar.m_CharInfo.m_dwRentalMinute != 0 )
				{
					g_MyInfo.CharRentalTimeMsgBox( rkChar.m_CharInfo.m_dwRentalMinute );
					return;
				}
			}
			break;
		}

		SP2Packet kPacket( CTPK_SET_MY_RENTAL );
		kPacket << dwCharIndex << sRentalType;
		TCPNetwork::SendToServer( kPacket );
		TCPNetwork::MouseBusy( true );
	}
}

void MannerValuationSlotBtn::SetCharSlot( int iListArray, UserInfoNode *pUserInfo, bool bOnline, ioPlayStage * pPlayStage )
{
	m_iListArray = iListArray;
	m_pUserInfo  = pUserInfo;
	m_bOnline    = bOnline;
	m_pPlayStage = pPlayStage;

	SAFEDELETE( m_aMyChar );
	HideChildWnd( ID_RENTAL );
	HideChildWnd( ID_EXPERIENCE );
	HideChildWnd( ID_SET_RENTAL );
	HideChildWnd( ID_DEL_RENTAL );

	m_pRentalSetBtn = NULL;
	if( m_pUserInfo && m_bOnline )
	{
		ShowChildWnd( ID_EXPERIENCE );

		UserInfoCharData &rkUserCharData = m_pUserInfo->GetUserCharData();	
		CharInfoData &rkChar = rkUserCharData.GetSlotChar( m_iListArray );
		CreateCharacter( rkChar );
	}
}

void MannerValuationSlotBtn::CreateCharacter( CharInfoData &rkInfo )
{
	SAFEDELETE( m_aMyChar );

	if( rkInfo.m_dwCharLoadingTime == 0 )
		rkInfo.m_dwCharLoadingTime = FRAMEGETTIME();

	if( FRAMEGETTIME() - rkInfo.m_dwCharLoadingTime < ( GetID() - MannerValuationExtensionWnd::ID_SOLDIER_SLOT_1 ) * CHARINFODATA_UI_LOAD_TIME ) 
		return;

	rkInfo.m_bChangeData = false;
	m_CharInfo = rkInfo.m_CharInfo;
	if( m_CharInfo.m_class_type == 0 )
		return;

	if( m_pUI3DRender == NULL )
		return;

	ioCreateChar *pChar = m_pUI3DRender->CreateChar( m_CharInfo, "ResultChar" );
	if( !pChar )
		return;

	pChar->SetPosition( 0.0f, -FLOAT90, 1050.0f );
	pChar->SetYaw( FLOAT10 );
	m_pUI3DRender->SetLookAt( 0.0f, 0.0f, 900.0f );

	float fAspect = (float)VIEW_WIDTH/(float)VIEW_HEIGHT;
	m_pUI3DRender->SetProjection( FLOAT1, 600.0f, 45.0f, fAspect );

	m_aMyChar = pChar;

	// 코스튬 관련 (캐릭터 클릭시 윈도우 버튼의 캐릭터)
	int TempCostume[MAX_INVENTORY];
	for ( int i=0; i<MAX_INVENTORY; i++ )
	{
		TempCostume[i] = rkInfo.m_CharInfo.m_EquipCostume[i].m_CostumeCode;

		// 코스튬 관련 (자신만 적용)
		if ( !Setting::IsCostumeShowChange() && !g_CostumeInfoMgr.IsEnableCostumeMode() && m_pUserInfo->GetUserID() != g_MyInfo.GetPublicID() )
			TempCostume[i] = 0;
	}	

	for(int i = 0;i < MAX_INVENTORY;i++)
	{
		DWORD dwCostumeMaleCustom = rkInfo.m_CharInfo.m_EquipCostume[i].m_Costume_Male_Custom;
		DWORD dwCostumeFemaleCustom = rkInfo.m_CharInfo.m_EquipCostume[i].m_Costume_Female_Custom;

		if( rkInfo.m_EquipItemInfo[i].m_item_code != 0 )
		{
			if ( rkInfo.m_CharInfo.m_EquipCostume[i].m_CostumeCode <= 0 )
			{
				dwCostumeMaleCustom = rkInfo.m_EquipItemInfo[i].m_item_male_custom;
				dwCostumeFemaleCustom = rkInfo.m_EquipItemInfo[i].m_item_female_custom;
			}
			m_aMyChar->EquipItem( rkInfo.m_EquipItemInfo[i].m_item_code, dwCostumeMaleCustom, dwCostumeFemaleCustom, TempCostume[i] );
		}
		else
		{
			int iItemCode = 0;
			if( rkInfo.m_CharInfo.m_eCharPowerUpType != PUGT_NONE )
			{
				iItemCode = (DEFAULT_BASIC_ITEM_CODE * i) + ConvertPowerUpCharToItem( (int)m_CharInfo.m_eCharPowerUpType ) + m_CharInfo.m_class_type;
				if( !g_PowerUpManager.IsPowerUpGradeItem( m_CharInfo.m_class_type, iItemCode ) || !g_ItemMaker.IsItem( iItemCode ) )
				{
					iItemCode = (DEFAULT_BASIC_ITEM_CODE * i) + m_CharInfo.m_class_type;
				}
			}
			else
			{
				iItemCode = (DEFAULT_BASIC_ITEM_CODE * i) + m_CharInfo.m_class_type;
			}

			if ( rkInfo.m_CharInfo.m_EquipCostume[i].m_CostumeCode <= 0 )
			{
				dwCostumeMaleCustom = 0;
				dwCostumeFemaleCustom = 0;
			}
			m_aMyChar->EquipItem( iItemCode, dwCostumeMaleCustom, dwCostumeFemaleCustom, TempCostume[i] );
		}
	}

	char szDealyAni[MAX_PATH] = "";
	if( m_aMyChar->IsMale() )
		sprintf_e( szDealyAni, "public_shop_delay.ani" );
	else
		sprintf_e( szDealyAni, "public_shop_delay_hw.ani" );
	m_aMyChar->SetLoopAni( szDealyAni );
	m_dwCharAniDelayTime = FRAMEGETTIME();

	if( !m_szAwakeEffect.IsEmpty() )
		pChar->EndEffect( m_szAwakeEffect, true );

	m_szAwakeEffect = g_AwakeMgr.GetAwakeEffect( static_cast<AwakeType>( rkInfo.m_iAwakeType ) ).c_str();
	pChar->AttachEffect( m_szAwakeEffect, NULL );

	SetSlotButton();
}

void MannerValuationSlotBtn::SetSlotButton()
{
	HideChildWnd( ID_RENTAL );
	HideChildWnd( ID_EXPERIENCE );
	HideChildWnd( ID_SET_RENTAL );
	HideChildWnd( ID_DEL_RENTAL );
	if( m_pUserInfo && m_bOnline )
	{
		ShowChildWnd( ID_EXPERIENCE );
	}

	//전투체험 금지 주석처리 20170406
	//if( m_pUserInfo == NULL ) return;

	//UserInfoCharData &rkUserCharData = m_pUserInfo->GetUserCharData();
	//CharInfoData &rkChar = rkUserCharData.GetSlotChar( m_iListArray );
	//if( rkChar.m_CharInfo.m_sRentalType == CRT_RENTAL )
	//{
	//	ShowChildWnd( ID_RENTAL );		
	//}

	//ioButton *pPrevBtn = m_pRentalSetBtn;	
	//if( m_pUserInfo )
	//{
	//	m_pRentalSetBtn = NULL;
	//	if( m_pUserInfo->GetUserID() == g_MyInfo.GetPublicID() )
	//	{
	//		if( m_CharInfo.m_sRentalType == CRT_RENTAL )
	//			m_pRentalSetBtn = dynamic_cast< ioButton * >( FindChildWnd( ID_DEL_RENTAL ) );
	//		else
	//		{
	//			if( g_MyInfo.GetCharRentalSetCount() < Help::GetMaxCharRentSet() )
	//				m_pRentalSetBtn = dynamic_cast< ioButton * >( FindChildWnd( ID_SET_RENTAL ) );
	//		}
	//	}
	//}   

	//if( m_pRentalSetBtn != pPrevBtn )
	//{
	//	if( pPrevBtn )
	//		pPrevBtn->HideWnd();
	//}
}

void MannerValuationSlotBtn::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	if( FRAMEGETTIME() - m_dwCharLoadTime > 100 )
	{
		m_dwCharLoadTime = FRAMEGETTIME();
		m_iCharLoadArray++;
		if( m_iCharLoadArray >= MAX_CHAR_LODING )
			m_iCharLoadArray = 0;
	}

	if( m_aMyChar )
	{
		m_aMyChar->Update( fTimePerSec );

		if( m_dwCharAniDelayTime > 0 && FRAMEGETTIME() - m_dwCharAniDelayTime > 3000 )
		{
			m_dwCharAniDelayTime = 0;
			if( !m_aMyChar->SetWeaponShopMotion() )
			{
				ioHashString szUIAnimation = m_pUI3DRender->GetModelAnimation( 0, m_aMyChar->IsMale() );
				if( !szUIAnimation.IsEmpty() )
				{
					m_aMyChar->SetLoopAni( szUIAnimation );
				}
			}
		}
	}

	if( m_pUserInfo && m_bOnline )
	{
		UserInfoCharData &rkUserCharData = m_pUserInfo->GetUserCharData();
		CharInfoData &rkChar = rkUserCharData.GetSlotChar( m_iListArray );
		if( rkChar.m_bChangeData )
		{	
			CreateCharacter( rkChar );	
		}
	}

	if( m_bOnline ) 
	{
		if( m_pUserInfo )
		{
			UserInfoCharData &rkUserCharData = m_pUserInfo->GetUserCharData();
			if( !rkUserCharData.IsDataSync() )
				SetCharSlot( m_iListArray, m_pUserInfo, false, m_pPlayStage );
		}
	}
	else
	{
		if( m_pUserInfo )
		{
			UserInfoCharData &rkUserCharData = m_pUserInfo->GetUserCharData();
			if( rkUserCharData.IsDataSync() )
				SetCharSlot( m_iListArray, m_pUserInfo, true, m_pPlayStage );
		}
	}

	if( m_pRentalSetBtn )
	{
		float fAlphaTime = fTimePerSec * 600;
		if( g_GUIMgr.GetPreOverWnd() == this ||
			g_GUIMgr.GetPreClickWnd() == this ||
			g_GUIMgr.GetPreOverWnd() == m_pRentalSetBtn ||
			g_GUIMgr.GetPreClickWnd() == m_pRentalSetBtn )
		{
			if( !m_pRentalSetBtn->IsShow() )
				m_pRentalSetBtn->ShowWnd();			
			
			m_fAlphaRate = min( MAX_ALPHA_RATE, m_fAlphaRate + fAlphaTime );
			m_pRentalSetBtn->SetWindowAlpha( (int)m_fAlphaRate );
		}
		else
		{
			m_fAlphaRate = max( 0.0f, m_fAlphaRate - fAlphaTime );			
			if( m_pRentalSetBtn->IsShow() )
			{
				m_pRentalSetBtn->SetWindowAlpha( (int)m_fAlphaRate );
				if( m_fAlphaRate == 0.0f )
					m_pRentalSetBtn->HideWnd();
			}
		}
	}
}

void MannerValuationSlotBtn::OnRenderSoldier( int iXPos, int iYPos )
{
	if( !m_pUI3DRender || !m_aMyChar ) return;

	D3DRECT rcD3D;
	rcD3D.x1 = iXPos;
	rcD3D.y1 = iYPos;
	rcD3D.x2 = rcD3D.x1 + VIEW_WIDTH;
	rcD3D.y2 = rcD3D.y1 + VIEW_HEIGHT;
	m_pUI3DRender->RenderViewPort( &rcD3D, true );
}

void MannerValuationSlotBtn::OnRenderFrame( int iXPos, int iYPos )
{
	if( !m_pBlueNor || !m_pBlueOver || !m_pBlueNorGra || !m_pBlueOverGra )
		return;

	if( m_bOver || m_bClicked )
	{
		m_pBlueOver->Render( iXPos, iYPos );

		if( m_aMyChar == NULL )
			OnRenderLoadingChar( iXPos, iYPos );
		else
			OnRenderSoldier( iXPos + 3, iYPos + 3 );

		m_pBlueOverGra->SetSize( 79, 40 );
		m_pBlueOverGra->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
		m_pBlueOverGra->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
		m_pBlueOverGra->Render( iXPos + 3, iYPos + 152 );
	}	
	else
	{
		m_pBlueNor->Render( iXPos, iYPos );

		if( m_aMyChar == NULL )
			OnRenderLoadingChar( iXPos, iYPos );
		else
			OnRenderSoldier( iXPos + 3, iYPos + 3 );

		m_pBlueNorGra->SetSize( 79, 40 );
		m_pBlueNorGra->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
		m_pBlueNorGra->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
		m_pBlueNorGra->Render( iXPos + 3, iYPos + 152 );
	}
}

void MannerValuationSlotBtn::OnRenderLoadingChar( int iXPos, int iYPos )
{
	if( !m_pUserInfo || !m_bOnline ) return;
	if( !COMPARE( m_iCharLoadArray, 0, MAX_CHAR_LODING ) ) return;

	UserInfoCharData &rkUserCharData = m_pUserInfo->GetUserCharData();
	if( m_iListArray >= rkUserCharData.GetMaxChar() ) return;

	if( m_pLoadCharBack )
		m_pLoadCharBack->Render( iXPos, iYPos );

	if( m_pCharLoading[m_iCharLoadArray] )
		m_pCharLoading[m_iCharLoadArray]->Render( iXPos + 31, iYPos + 86 );
}

void MannerValuationSlotBtn::OnRender()
{
	ioButton::OnRender();

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	OnRenderFrame( iXPos, iYPos );

	if( m_pUserInfo && m_bOnline )
	{
		UserInfoCharData &rkUserCharData = m_pUserInfo->GetUserCharData();
		CharInfoData &rkChar = rkUserCharData.GetSlotChar( m_iListArray );
		if( rkChar.m_CharInfo.m_class_type != 0 )
		{
			if( rkChar.m_CharInfo.m_sRentalType == CRT_RENTAL && rkChar.m_CharInfo.m_dwRentalMinute != 0 )
			{
				if( m_pRentalMark )
					m_pRentalMark->Render( iXPos + 6, iYPos + 6 );
			}
			else
			{
				g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
				g_FontMgr.SetAlignType( TAT_LEFT );
				g_FontMgr.SetBkColor( 59, 125, 207 );
				g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
				g_FontMgr.PrintText( iXPos + 5, iYPos + 3, FONT_SIZE_12, STR(1), rkChar.m_iClassLevel );
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////
MannerValuationControlWnd::MannerValuationControlWnd()
{
	InitAniState();

	m_bOnline   = false;
	m_pUserInfo = NULL;
	m_PlayStage = NULL;
}

MannerValuationControlWnd::~MannerValuationControlWnd()
{
}

void MannerValuationControlWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_LEFT_ROTATE_BTN:
	case ID_RIGHT_ROTATE_BTN:
	case ID_DEFAULT_POS_BTN:
	case ID_LEADER_CHANGE_BTN:
		if( cmd == IOBN_BTNUP || cmd == IOBN_BTNDOWN )
		{
			if( ioWnd::GetParent() )
				ioWnd::GetParent()->iwm_command( this, cmd, dwID );
		}
		break;
	}
}

void MannerValuationControlWnd::SetDefaultPosTitle( const char* szTitle )
{
	ioWnd *pWnd = FindChildWnd( ID_DEFAULT_POS_BTN );
	if( pWnd )
		pWnd->SetTitleText( szTitle );
}

void MannerValuationControlWnd::InitAniState()
{
	enum { DELAY_ALPHA = 500, };

	FSMState *pState = new FSMState(AT_NONE);
	if( pState )
		pState->Add( AT_ALPHA_IN,  AT_ALPHA_IN );
	m_AniState.Add( pState );

	pState = new FSMState(AT_ALPHA_IN);
	if( pState )
	{
		pState->Add( AT_ALPHA_IN, AT_NORMAL, DELAY_ALPHA );
		pState->Add( AT_ALPHA_OUT, AT_ALPHA_OUT );
	}
	m_AniState.Add( pState );	

	pState = new FSMState(AT_NORMAL);
	if( pState )
	{
		pState->Add( AT_ALPHA_OUT, AT_ALPHA_OUT );
	}
	m_AniState.Add( pState );	

	pState = new FSMState(AT_ALPHA_OUT);
	if( pState )
	{
		pState->Add( AT_ALPHA_OUT, AT_HIDE, DELAY_ALPHA );
	}
	m_AniState.Add( pState );	

	m_AniState.SetCurState( AT_NONE );
}

void MannerValuationControlWnd::iwm_show()
{
	m_AniState.SetCurState( AT_ALPHA_IN );
	HideChildWnd( ID_DEFAULT_POS_BTN );

	ioWnd* pBtn = FindChildWnd( ID_LEADER_CHANGE_BTN );
	if( pBtn && m_PlayStage )
	{
		ioPlayMode* pPlayMode = m_PlayStage->GetPlayMode();
		if( pPlayMode )
		{
			//if( pPlayMode->GetModeType() == MT_ARENA && ( pPlayMode->GetModeState() == ioPlayMode::MS_PLAY || ( !g_GUIMgr.IsShow( PARTY_SHAM_BATTLE_WND ) && pPlayMode->GetModeState() == ioPlayMode::MS_RESULT ) ) )
			if( !g_MyInfo.IsOriginalCharacter() )
				pBtn->SetInActive();
			else
				pBtn->SetActive();
		}
	}
}

void MannerValuationControlWnd::iwm_hide()
{
	m_AniState.SetCurState( AT_NONE );
}

void MannerValuationControlWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return ;

	m_AniState.UpdateTime();

	if( m_AniState.GetCurState() == AT_ALPHA_IN )
	{
		float fRate = ( (float) m_AniState.GetCurTime() / (float) m_AniState.GetDelay() );
		int iAlpha = 255.0f * fRate;

		SetChildAlpha( iAlpha );
	}
	else if( m_AniState.GetCurState() == AT_ALPHA_OUT )
	{
		float fRate = ( (float) m_AniState.GetCurTime() / (float) m_AniState.GetDelay() );
		int iAlpha = 255.0f * ( FLOAT1 - fRate );

		SetChildAlpha( iAlpha );
	}
	else if( m_AniState.GetCurState() == AT_HIDE )
	{
		HideWnd();
	}
	else
	{
		SetChildAlpha( 255 );
	}
}

void MannerValuationControlWnd::SetChildAlpha( int iAlphaRate )
{
	for (int i = 0; i < MAX_BTN ; i++)
	{
		ioButton *pBtn = dynamic_cast<ioButton*> ( FindChildWnd( ID_LEFT_ROTATE_BTN + i ) );
		if( !pBtn ) continue;

		pBtn->SetWindowAlpha( iAlphaRate );
	}
}

void MannerValuationControlWnd::HideWndAnimation()
{
	if( !IsShow() )
		return;
	m_AniState.ChangeState( AT_ALPHA_OUT );
}

void MannerValuationControlWnd::SetMannerValuationUserInfo( UserInfoNode *pUserInfo, bool bOnline )
{
	m_pUserInfo = pUserInfo;
	m_bOnline   = bOnline;

	if( !pUserInfo || !bOnline )
	{
		HideChildWnd( ID_LEFT_ROTATE_BTN );
		HideChildWnd( ID_RIGHT_ROTATE_BTN );
		HideChildWnd( ID_LEADER_CHANGE_BTN );
	}
	else if( pUserInfo && pUserInfo->GetUserID() == g_MyInfo.GetPublicID() )
	{
		ShowChildWnd( ID_LEFT_ROTATE_BTN );
		ShowChildWnd( ID_RIGHT_ROTATE_BTN );
		ShowChildWnd( ID_LEADER_CHANGE_BTN );
	}
	else
	{
		ShowChildWnd( ID_LEFT_ROTATE_BTN );
		ShowChildWnd( ID_RIGHT_ROTATE_BTN );
		HideChildWnd( ID_LEADER_CHANGE_BTN );
	}

	if( !IsShow() )
		ShowWnd();
}

void MannerValuationControlWnd::SetPlayStage( ioPlayStage* pStage )
{
	m_PlayStage = pStage;
}
//////////////////////////////////////////////////////////////////////////
MannerValuationLeaderSlotWnd::MannerValuationLeaderSlotWnd()
{
	m_pUI3DRender = NULL;
	m_aMyChar	  = NULL;

	m_pBackBar    = NULL;
	m_pShadowChar = NULL;

	m_bOnline     = false;
	m_pUserInfo   = NULL;

	m_fRotateYaw  = 0.0f;
	m_fPreYaw     = 0.0f;
	m_fXMLRotateYaw = 0.0f;
	m_fZPosition = FLOAT500;

	m_bLeftRotateOn  = false;
	m_bRightRotateOn = false;
}

MannerValuationLeaderSlotWnd::~MannerValuationLeaderSlotWnd()
{
	SAFEDELETE( m_aMyChar );
	SAFEDELETE( m_pUI3DRender );

	SAFEDELETE( m_pBackBar );
	SAFEDELETE( m_pShadowChar );
}

void MannerValuationLeaderSlotWnd::iwm_create()
{
	SAFEDELETE( m_pUI3DRender );

	m_pUI3DRender = new ioUI3DRender;
	m_pUI3DRender->Initialize( "config/sp2_stage_charselect_map.ini", "shop_cam" );

	m_fZPosition = 550.0f;
}

void MannerValuationLeaderSlotWnd::CreateCharacter( CharInfoData &rkInfo )
{
	if( rkInfo.m_dwCharLoadingTime == 0 )
		rkInfo.m_dwCharLoadingTime = FRAMEGETTIME();

	if( FRAMEGETTIME() - rkInfo.m_dwCharLoadingTime < CHARINFODATA_UI_LOAD_TIME )
		return;

	rkInfo.m_bChangeData = false;
	int iPreClassType = m_CharInfo.m_class_type;
	int iPreSexType   = m_CharInfo.m_sex;

	m_CharInfo = rkInfo.m_CharInfo;

	ioCreateChar *pChar = m_pUI3DRender->CreateChar( m_CharInfo, "ShopChar" );
	if( !pChar )	return;

	if( m_aMyChar )
	{
		m_fPreYaw = m_aMyChar->GetYaw();
	}

	D3DXVECTOR3 vCharPos( 0.0f, 0.0f, m_fZPosition );
	pChar->SetPosition( vCharPos );
	pChar->SetYaw( m_fPreYaw );

	m_pUI3DRender->SetLookAt( vCharPos );

	SAFEDELETE( m_aMyChar );
	m_aMyChar = pChar;

	// 코스튬 관련 (캐릭터 클릭시 나오는 리더 캐릭터)
	int TempCostume[MAX_INVENTORY];
	for ( int i=0; i<MAX_INVENTORY; i++ )
	{
		TempCostume[i] = rkInfo.m_CharInfo.m_EquipCostume[i].m_CostumeCode;

		// 코스튬 관련 (자신만 적용)
		if ( !Setting::IsCostumeShowChange() && !g_CostumeInfoMgr.IsEnableCostumeMode() && m_pUserInfo->GetUserID() != g_MyInfo.GetPublicID() )
			TempCostume[i] = 0;
	}

	for(int i = 0;i < MAX_INVENTORY;i++)
	{
		DWORD dwCostumeMaleCustom = rkInfo.m_CharInfo.m_EquipCostume[i].m_Costume_Male_Custom;
		DWORD dwCostumeFemaleCustom = rkInfo.m_CharInfo.m_EquipCostume[i].m_Costume_Female_Custom;
		if( rkInfo.m_EquipItemInfo[i].m_item_code != 0 )
		{	
			if ( rkInfo.m_CharInfo.m_EquipCostume[i].m_CostumeCode <= 0 )
			{
				dwCostumeMaleCustom = rkInfo.m_EquipItemInfo[i].m_item_male_custom;
				dwCostumeFemaleCustom = rkInfo.m_EquipItemInfo[i].m_item_female_custom;
			}
			EquipItem( rkInfo.m_EquipItemInfo[i].m_item_code, dwCostumeMaleCustom, dwCostumeFemaleCustom, TempCostume[i] );
		}
		else
		{
			int iItemCode = 0;
			if( rkInfo.m_CharInfo.m_eCharPowerUpType != PUGT_NONE )
			{
				iItemCode = (DEFAULT_BASIC_ITEM_CODE * i) + ConvertPowerUpCharToItem( (int)m_CharInfo.m_eCharPowerUpType ) + m_CharInfo.m_class_type;
				if( !g_PowerUpManager.IsPowerUpGradeItem( m_CharInfo.m_class_type, iItemCode ) || !g_ItemMaker.IsItem( iItemCode ) )
				{
					iItemCode = (DEFAULT_BASIC_ITEM_CODE * i) + m_CharInfo.m_class_type;
				}
			}
			else
			{
				iItemCode = (DEFAULT_BASIC_ITEM_CODE * i) + m_CharInfo.m_class_type;
			}

			if ( rkInfo.m_CharInfo.m_EquipCostume[i].m_CostumeCode <= 0 )
			{
				dwCostumeMaleCustom = 0;
				dwCostumeFemaleCustom = 0;
			}
			EquipItem( iItemCode, dwCostumeMaleCustom, dwCostumeFemaleCustom, TempCostume[i] );
		}
	}

	SetCharAnimation( true );

	if( iPreClassType != m_CharInfo.m_class_type || iPreSexType != m_CharInfo.m_sex )
	{
		InitRotate();
	}
}

void MannerValuationLeaderSlotWnd::SetCharAnimation( bool bShopMotion )
{
	if( !m_aMyChar ) return;

	DWORD dwCharMotionItem = 0;
	if( m_pUserInfo )
	{
		if( m_pUserInfo->GetUserID() == g_MyInfo.GetPublicID() )
		{
			dwCharMotionItem = g_MyInfo.GetEtcMotionAni( ioEtcItemMotion::MOTION_OPTION_10 );	
		}
		else
		{
			UserInfoCharData &rkUserCharData = m_pUserInfo->GetUserCharData();
			CharInfoData &rkLeader = rkUserCharData.GetLeaderChar();
			dwCharMotionItem = rkLeader.m_dwMotionEtcItem;
		}
	}

	ioHashString kMotionAni;
	ioEtcItemMotion *pMotionItem = dynamic_cast< ioEtcItemMotion * > ( g_EtcItemMgr.FindEtcItem( dwCharMotionItem ) );
	if( pMotionItem )
	{
		if( m_aMyChar->IsMale() )
			kMotionAni = pMotionItem->GetHumanManMotion();
		else
			kMotionAni = pMotionItem->GetHumanWomanMotion();
	}

	if( !kMotionAni.IsEmpty() )
	{
		m_aMyChar->SetLoopAni( kMotionAni );
	}
	else
	{
		if( bShopMotion )
		{
			if( m_aMyChar->SetWeaponShopMotion() )
				return;
		}

		ioHashString szUIAnimation = m_pUI3DRender->GetModelAnimation( 0, m_aMyChar->IsMale() );
		if( !szUIAnimation.IsEmpty() )
		{
			m_aMyChar->SetLoopAni( szUIAnimation );
		}
	}	
}

void MannerValuationLeaderSlotWnd::EquipItem( DWORD dwItemCode, DWORD dwItemMaleCustom, DWORD dwItemFemaleCustom, int nCostumeCode )
{
	if( m_aMyChar )
	{
		m_aMyChar->EquipItem( dwItemCode, dwItemMaleCustom, dwItemFemaleCustom, nCostumeCode );
	}
}

void MannerValuationLeaderSlotWnd::ReleaseItem( int iSlot )
{
	if( m_aMyChar )
	{
		m_aMyChar->ReleaseItem( iSlot );
	}
}

void MannerValuationLeaderSlotWnd::RotateY( float fYaw )
{
	m_fRotateYaw = fYaw;
}

void MannerValuationLeaderSlotWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "BackBar" )
	{
		SAFEDELETE( m_pBackBar );
		m_pBackBar = pImage;
	}
	else if( szType == "ShadowChar" )
	{
		SAFEDELETE( m_pShadowChar );
		m_pShadowChar = pImage;
		if( m_pShadowChar )
		{
			m_pShadowChar->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
			m_pShadowChar->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
			m_pShadowChar->SetSize( 118, 187 );
		}
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void MannerValuationLeaderSlotWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return ;

	if( m_aMyChar )
	{
		m_aMyChar->RotateY( m_fRotateYaw );
		m_aMyChar->Update( fTimePerSec );
	}

	if( m_pUserInfo && m_bOnline )
	{
		UserInfoCharData &rkUserCharData = m_pUserInfo->GetUserCharData();
		CharInfoData &rkLeader = rkUserCharData.GetLeaderChar();
		if( rkLeader.m_bChangeData )
		{
			CreateCharacter( rkLeader );	
		}
	}

	if( m_bOnline == false ) 
	{
		if( m_pUserInfo )
		{
			UserInfoCharData &rkUserCharData = m_pUserInfo->GetUserCharData();
			if( rkUserCharData.IsDataSync() )
				SetLeader( m_pUserInfo, true );
		}
	}

	ioLSMouse *pMouse = g_App.GetMouse();
	if( !pMouse )
		return;

	RECT rcNameRect;
	rcNameRect.left = GetDerivedPosX();
	rcNameRect.top  = GetDerivedPosY();
	rcNameRect.right  = rcNameRect.left + GetWidth();
	rcNameRect.bottom = rcNameRect.top + GetHeight();

	if( ::PtInRect( &rcNameRect, pMouse->GetMousePos() ) )
	{
		MannerValuationControlWnd *pControlWnd = dynamic_cast<MannerValuationControlWnd*> (FindChildWnd( ID_CHAR_CONTROL_WND ) );
		if( pControlWnd )
		{
			pControlWnd->SetMannerValuationUserInfo( m_pUserInfo, m_bOnline );
		}
	}
	else
	{
		MannerValuationControlWnd *pControlWnd = dynamic_cast<MannerValuationControlWnd*> (FindChildWnd( ID_CHAR_CONTROL_WND ) );
		if( pControlWnd )
			pControlWnd->HideWndAnimation();
	}
}

void MannerValuationLeaderSlotWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_pBackBar )
	{
		m_pBackBar->SetScale( FLOAT1, 0.7f );
		m_pBackBar->Render( iXPos - 14, iYPos + 225, UI_RENDER_MULTIPLY, TFO_BILINEAR );
	}

	if( !m_aMyChar )
	{
		if( m_pShadowChar )
		{
			m_pShadowChar->Render( iXPos + 19, iYPos + 34, UI_RENDER_NORMAL, TFO_BILINEAR );
		}
	}

	D3DXMATRIX matUI = m_pUI3DRender->MakeUIProjection( iXPos - 330, iYPos - 65 );
	m_pUI3DRender->Render( &matUI, true );
}

void MannerValuationLeaderSlotWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_CHAR_CONTROL_WND:
		if( cmd == IOBN_BTNDOWN)
		{
			if( param == MannerValuationControlWnd::ID_LEFT_ROTATE_BTN )
			{
				RotateY( m_fXMLRotateYaw );
			}
			else if( param == MannerValuationControlWnd::ID_RIGHT_ROTATE_BTN )
			{
				RotateY( -m_fXMLRotateYaw );
			}
		}
		else if( cmd == IOBN_BTNUP )
		{
			if( param == MannerValuationControlWnd::ID_DEFAULT_POS_BTN )
			{
				if( m_aMyChar )
					m_aMyChar->SetYaw( FLOAT10 );
				if( ioWnd::m_pParent )
					ioWnd::m_pParent->iwm_command( this, cmd, param );
				InitRotate();
			}
			else if( param == MannerValuationControlWnd::ID_LEFT_ROTATE_BTN )
			{
				RotateY( 0.0f );
			}
			else if( param == MannerValuationControlWnd::ID_RIGHT_ROTATE_BTN )
			{
				RotateY( 0.0f );
			}
			else if( param == MannerValuationControlWnd::ID_LEADER_CHANGE_BTN )
			{
				UISoldierSelectWnd *pUISoldierWnd = dynamic_cast<UISoldierSelectWnd*> ( g_GUIMgr.FindWnd( UI_SOLDIERSELECT_WND ) );
				if( pUISoldierWnd )
				{
					if( m_pParent )
					{
						pUISoldierWnd->ShowUISoldierSelectWnd( m_pParent );
						int iCharArray = g_MyInfo.GetClassArray( m_CharInfo.m_class_type );
						if( iCharArray != -1 )
						{
							pUISoldierWnd->SetSelectCharArray( iCharArray );
						}
					}
				}
			}
		}
		break;
	}
}

void MannerValuationLeaderSlotWnd::SetZPosition( float ZPos )
{
	m_fZPosition = ZPos;
}

void MannerValuationLeaderSlotWnd::SetLeader( UserInfoNode *pUserInfo, bool bOnline )
{
	m_pUserInfo = pUserInfo;
	m_bOnline   = bOnline;

	SAFEDELETE( m_aMyChar );
	if( m_pUserInfo && m_bOnline )
	{
		UserInfoCharData &rkUserCharData = m_pUserInfo->GetUserCharData();
		CharInfoData &rkLeader = rkUserCharData.GetLeaderChar();
		if( rkLeader.m_CharInfo.m_class_type == 0 )
			return;

		CreateCharacter( rkLeader );
	}
}

void MannerValuationLeaderSlotWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_fXMLRotateYaw = xElement.GetFloatAttribute_e( "RotateYaw" );
}

void MannerValuationLeaderSlotWnd::InitRotate()
{
	RotateY( 0.0f );
	m_bLeftRotateOn  = false;
	m_bRightRotateOn = false;
	if( m_aMyChar )
		m_aMyChar->SetYaw( FLOAT10 );
}

void MannerValuationLeaderSlotWnd::SetCharScale( float fScale )
{
	if( m_aMyChar )
		m_aMyChar->SetScale( fScale );
}

void MannerValuationLeaderSlotWnd::iwm_show()
{
	HideChildWnd( ID_CHAR_CONTROL_WND );
}

void MannerValuationLeaderSlotWnd::iwm_hide()
{
	HideChildWnd( ID_CHAR_CONTROL_WND );

	SAFEDELETE( m_aMyChar );
}
//////////////////////////////////////////////////////////////////////////
MannerValuationChatWnd::MannerValuationChatWnd()
{
	m_bOnline   = false;
	m_pUserInfo = NULL;
}

MannerValuationChatWnd::~MannerValuationChatWnd()
{
	m_pUserInfo = NULL;
}

void MannerValuationChatWnd::SetChatUser( UserInfoNode *pUserInfo, bool bOnline )
{
	m_bOnline   = bOnline;
	m_pUserInfo = pUserInfo;

	if( m_bOnline )
	{	
		SetChildWndStyleRemove( ID_CHAT, IWS_EXACTIVE );
	}
	else
	{
		SetChildWndStyleAdd( ID_CHAT, IWS_EXACTIVE );
	}
	ShowWnd();
}

void MannerValuationChatWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
		if( cmd == IOBN_BTNUP )
			HideWnd();
		break;
	case ID_CHAT:
		if( cmd == IOBN_BTNUP || cmd == IOEX_BTNUP )
		{
			HideWnd();

			OnChannelChat();
		}
		break;
	case ID_MEMO:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();

			OnOpenMemo();
		}
		break;
	}
}

void MannerValuationChatWnd::OnChannelChat()
{
	if( m_pUserInfo == NULL ) return;

	if( !m_bOnline )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		return;
	}

	if( m_pUserInfo->GetUserID() == g_MyInfo.GetPublicID() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
		return;
	}

	MessengerWnd *pMessengerWnd = dynamic_cast<MessengerWnd*>(g_GUIMgr.FindWnd( MESSENGER_WND ));
	if( pMessengerWnd )
	{
		if( !pMessengerWnd->QuickChannelInvite( m_pUserInfo->GetUserID() ) )
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
	}
}

void MannerValuationChatWnd::OnOpenMemo()
{
	if( g_MyInfo.GetBlockType() == BKT_LIMITATION )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		return;
	}

	if( m_pUserInfo == NULL ) return;

	if( m_pUserInfo->GetUserID() == g_MyInfo.GetPublicID() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
		return;
	}

	MessengerWnd *pMessengerWnd = dynamic_cast<MessengerWnd*>(g_GUIMgr.FindWnd( MESSENGER_WND ));
	if( pMessengerWnd )
	{
		pMessengerWnd->MoveMemo( m_pUserInfo->GetUserID().c_str() );
	}
}

void MannerValuationChatWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );	
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( iXPos + 89, iYPos + 129, FONT_SIZE_11, STR(1) );
	g_FontMgr.PrintText( iXPos + 89, iYPos + 145, FONT_SIZE_11, STR(2) );

	g_FontMgr.PrintText( iXPos + 236, iYPos + 129, FONT_SIZE_11, STR(3) );
	g_FontMgr.PrintText( iXPos + 236, iYPos + 145, FONT_SIZE_11, STR(4) );
}
//////////////////////////////////////////////////////////////////////////
bool MannerValuationExtensionWnd::sg_bUpdateInfo = false;
MannerValuationExtensionWnd::MannerValuationExtensionWnd()
{
	m_pIconBack		 = NULL;
	m_pNoneIcon		 = NULL;
	m_pGradeIcon     = NULL;
	m_pInfoOver      = NULL;
	m_pDot			 = NULL;
	m_pMyBestFriend		= NULL;
	m_pOtherBestFriend	= NULL;
	m_pCoupleBestFriend	= NULL;

	m_iPrevGradeLevel= -1;
	m_pUserInfo      = NULL;
	m_pPlayStage     = NULL;
	m_eTrialType     = ioMannerTrialChatManager::TT_NONE;
	m_dwChannelIndex = 0;
	m_bOnline		 = false;

	m_dwTabID = ID_TOTAL_INFO;
	m_dwSysPopupMsg = 0;

	m_szCampName.Clear();

	m_pNamedTitleUiMark = NULL;
}

MannerValuationExtensionWnd::~MannerValuationExtensionWnd()
{
	SAFEDELETE( m_pIconBack );
	SAFEDELETE( m_pNoneIcon );
	SAFEDELETE( m_pGradeIcon );
	SAFEDELETE( m_pInfoOver );
	SAFEDELETE( m_pDot );
	SAFEDELETE( m_pMyBestFriend );
	SAFEDELETE( m_pOtherBestFriend );
	SAFEDELETE( m_pCoupleBestFriend );
	SAFEDELETE( m_pNamedTitleUiMark );
}

void MannerValuationExtensionWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "NoneIcon" )
	{
		SAFEDELETE( m_pNoneIcon );
		m_pNoneIcon = pImage;
	}
	else if( szType == "IconBack" )
	{
		SAFEDELETE( m_pIconBack );
		m_pIconBack = pImage;
	}
	else if( szType == "InfoOver" )
	{
		SAFEDELETE( m_pInfoOver );
		m_pInfoOver = pImage;
	}
	else if( strcmp(szType.c_str(), "Dot") == 0 )
	{
		SAFEDELETE( m_pDot );
		m_pDot = pImage;
	}
	else if( szType == "MyBestFriend" )
	{
		SAFEDELETE( m_pMyBestFriend );
		m_pMyBestFriend = pImage;
	}
	else if( szType == "OtherBestFriend" )
	{
		SAFEDELETE( m_pOtherBestFriend );
		m_pOtherBestFriend = pImage;
	}
	else if( szType == "CoupleBestFriend" )
	{
		SAFEDELETE( m_pCoupleBestFriend );
		m_pCoupleBestFriend = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void MannerValuationExtensionWnd::iwm_show()
{
	if( g_MyInfo.GetEntryType() == ET_TERMINATION || g_MyInfo.GetEntryType() == ET_TEMPORARY ||
		g_MyInfo.IsExpertEntryTemporary() )
	{
		HideWnd();
		g_App.OpenFormalityEntryPage();
		return;
	}

	if( m_szTargetID.IsEmpty() )
		HideWnd();	
	else
	{
		m_dwTabID = ID_TOTAL_INFO;
		CheckRadioButton( ID_TOTAL_INFO, ID_DETAIL_INFO, m_dwTabID );
	}

	m_iCurPage = 0;
	ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
	if( pScroll )
	{
		int iMaxScroll = 1;
		if( m_pUserInfo && m_bOnline )
		{
			UserInfoCharData &rkUserCharData = m_pUserInfo->GetUserCharData();
			iMaxScroll += max( 0, rkUserCharData.GetMaxChar() - 1 ) / MAX_SLOT;
		} 
		pScroll->SetScrollRange( 0, iMaxScroll );
		pScroll->SetScrollPos( m_iCurPage );
	}

	MannerValuationLeaderSlotWnd *pLeaderSlot = dynamic_cast<MannerValuationLeaderSlotWnd*>(FindChildWnd(ID_LEADER_SLOT));	
	if( pLeaderSlot )
	{
		pLeaderSlot->ShowWnd();
		pLeaderSlot->SetLeader( m_pUserInfo, m_bOnline );
	}

	for(int i = 0;i < MAX_SLOT;i++)
	{
		MannerValuationSlotBtn *pSlotBtn = dynamic_cast<MannerValuationSlotBtn*>( FindChildWnd( ID_SOLDIER_SLOT_1 + i ) );
		if( pSlotBtn )
		{
			pSlotBtn->ShowWnd();
			pSlotBtn->SetCharSlot( m_iCurPage * MAX_SLOT + i, m_pUserInfo, m_bOnline, m_pPlayStage );
		}
	}
}

void MannerValuationExtensionWnd::iwm_hide()
{
	MannerValuationSlotTooltip *pTooltip = dynamic_cast<MannerValuationSlotTooltip*>(FindChildWnd( ID_SLOT_TOOLTIP ));
	if( pTooltip )
	{
		pTooltip->SetTooltipInfo( NULL, -1 );
	}

	HideChildWnd( ID_LEADER_SLOT );
	for(int i = 0;i < MAX_SLOT;i++)
		HideChildWnd( ID_SOLDIER_SLOT_1 + i );
}

void MannerValuationExtensionWnd::iwm_vscroll( DWORD dwID, int curPos )
{
	UpdatePage( curPos );
}

void MannerValuationExtensionWnd::iwm_wheel( int zDelta )
{
	if( !IsShow() )
	{
		ioMovingWnd::iwm_wheel( zDelta );
	}
	else
	{ 
		// 휠 이벤트 처리를 했다면 부모로 이벤트를 날리지 않는다.
		if( zDelta == WHEEL_DELTA )
		{
			ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
			if( pScroll )
				pScroll->SetScrollPos( m_iCurPage - 1 );

			if( g_GUIMgr.GetPreOverWnd() )
			{
				ioMouse NullMouse;
				g_GUIMgr.GetPreOverWnd()->iwm_mouseover( NullMouse );
			}
		}
		else if( zDelta == -WHEEL_DELTA )
		{
			ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
			if( pScroll )
				pScroll->SetScrollPos( m_iCurPage + 1 );

			if( g_GUIMgr.GetPreOverWnd() )
			{
				ioMouse NullMouse;
				g_GUIMgr.GetPreOverWnd()->iwm_mouseover( NullMouse );
			}
		}
	}
}

void MannerValuationExtensionWnd::UpdatePage( int iCurPos )
{
	if( m_iCurPage == iCurPos )
		return;

	m_iCurPage = iCurPos;

	for(int i = 0;i < MAX_SLOT;i++)
	{
		MannerValuationSlotBtn *pSlotBtn = dynamic_cast<MannerValuationSlotBtn*>( FindChildWnd( ID_SOLDIER_SLOT_1 + i ) );
		if( pSlotBtn )
		{
			pSlotBtn->SetCharSlot( m_iCurPage * MAX_SLOT + i, m_pUserInfo, m_bOnline, m_pPlayStage );
		}
	}
}

bool MannerValuationExtensionWnd::IsOpenState( const ioHashString &szID )
{
	if( szID == g_MyInfo.GetPublicID() )
		return true;

	if( m_pPlayStage )
	{
		if( m_pPlayStage->GetModeType() != MT_MYROOM )
		{
			ioBaseChar *pOwnerChar = m_pPlayStage->GetOwnerChar();
			if( pOwnerChar )
			{	
				if( !pOwnerChar->IsMiniHomePageState() )
				{
					// 상태로 인해 오픈 불가
					return false;
				}
			}
		}
	}
	return true;
}

void MannerValuationExtensionWnd::SetButton()
{
	sg_bUpdateInfo = true;
	if( m_szTargetID.IsEmpty() ) return;

	if( m_szTargetID == g_MyInfo.GetPublicID() ||
		g_App.IsAdminID( m_szTargetID.c_str() ) )
	{
		SetChildInActive( ID_HEADQUARTER_MOVE );
		SetChildInActive( ID_HOME_MOVE );
		SetChildInActive( ID_MEMO_CHAT );
		SetChildInActive( ID_INSERT_FRIEND );
		SetChildInActive( ID_DELETE_FRIEND );
		SetChildInActive( ID_INSERT_BESTFRIEND );
		SetChildInActive( ID_DELETE_BESTFRIEND );
		SetChildInActive( ID_INSERT_BLACK );
		SetChildInActive( ID_DELETE_BLACK );
		SetChildInActive( ID_MINUS_MANNER_TRIAL );
		SetChildInActive( ID_KICK_VOTE );
	}
	else
	{
		SetChildActive( ID_HEADQUARTER_MOVE );
		SetChildActive( ID_HOME_MOVE );
		SetChildActive( ID_MEMO_CHAT );
		SetChildActive( ID_INSERT_FRIEND );
		SetChildActive( ID_DELETE_FRIEND );
		SetChildActive( ID_INSERT_BESTFRIEND );
		SetChildActive( ID_DELETE_BESTFRIEND );
		SetChildActive( ID_INSERT_BLACK );
		SetChildActive( ID_DELETE_BLACK );
		SetChildActive( ID_MINUS_MANNER_TRIAL );
		if( m_bOnline )
		{
			SetChildActive( ID_KICK_VOTE );
		}
		else
		{
			SetChildInActive( ID_KICK_VOTE );
			SetChildInActive( ID_HEADQUARTER_MOVE );
			SetChildInActive( ID_HOME_MOVE );
		}

		SetChildWndStyleAdd( ID_HOME_MOVE, IWS_EXACTIVE );
		if( m_pUserInfo->GetHouseOpen() )
		{
			SetChildWndStyleRemove( ID_HOME_MOVE, IWS_EXACTIVE );
		}
	}

	if ( m_szTargetID != g_MyInfo.GetPublicID() )
		HideChildWnd( ID_NAMED_TITLE );
	else
		ShowChildWnd( ID_NAMED_TITLE );
	
	SetNamedTitleSparkle();

	ApplyTitle();

	// 친구 여부에 따라 버튼 변경
	if( g_FriendManager.IsFriend( m_szTargetID ) )
	{
		HideChildWnd( ID_INSERT_FRIEND );
		ShowChildWnd( ID_DELETE_FRIEND );
	}
	else
	{
		ShowChildWnd( ID_INSERT_FRIEND );
		HideChildWnd( ID_DELETE_FRIEND );
	}

	// 절친 여부에 따라 버튼 변경
	if( g_FriendManager.GetBestFriendState( m_szTargetID ) == BFT_SET )
	{
		HideChildWnd( ID_INSERT_BESTFRIEND );
		ShowChildWnd( ID_DELETE_BESTFRIEND );
	}
	else
	{
		ShowChildWnd( ID_INSERT_BESTFRIEND );
		HideChildWnd( ID_DELETE_BESTFRIEND );
	}

	// 블랙리스트 여부에 따라 버튼 변경
	if( g_BlackListManager.IsBlackList( m_szTargetID ) )
	{
		HideChildWnd( ID_INSERT_BLACK );
		ShowChildWnd( ID_DELETE_BLACK );
	}
	else
	{
		ShowChildWnd( ID_INSERT_BLACK );
		HideChildWnd( ID_DELETE_BLACK );
	}		

	ProcessGuildButton();

	if( m_pPlayStage )
	{
		if( !m_pPlayStage->IsBaseChar( m_szTargetID ) || m_pPlayStage->GetModeType() == MT_HEADQUARTERS|| m_pPlayStage->GetModeType() == MT_HOUSE )
			SetChildInActive( ID_KICK_VOTE );
	}
}

void MannerValuationExtensionWnd::SetMannerValuationInfo( const ioHashString &szID, bool bOnline, ioMannerTrialChatManager::TrialType eType, DWORD dwChannelIndex /*= 0*/ )
{
	m_pUserInfo         = NULL;
	m_szTargetID        = szID;
	m_eTrialType        = eType;
	m_dwChannelIndex    = dwChannelIndex;
	m_bOnline           = bOnline;

	//실시간으로 로딩한다.
	m_iPrevGradeLevel   = -1;
	m_pGuildBtn = NULL;

	if( !m_szTargetID.IsEmpty() )
	{
		m_pUserInfo = g_UserInfoMgr.GetUserInfoNode( m_szTargetID );
		g_UserInfoMgr.UserInfoCharDataSync( m_szTargetID, m_bOnline );
		
		if( m_pUserInfo )
		{
			if( m_szTargetID == g_MyInfo.GetPublicID() )
			{
				//최신으로 갱신
				g_MyInfo.SerUserInfoAllCharData();	
			}

			TournamentNode *pTournament = g_TournamentMgr.GetTournament( g_TournamentMgr.GetRegularIndex() );
			if( pTournament )
			{
				if( m_pUserInfo->GetCampPosition() != 0 )
					m_szCampName = pTournament->GetCampName( m_pUserInfo->GetCampPosition() );
			}
		}
		SetButton();
	}
}

void MannerValuationExtensionWnd::ProcessGuildButton( bool bProcess )
{
	if( !m_pUserInfo ) return;
	if( bProcess && !m_pUserInfo->IsGuildInfoChange() ) return;

	if( m_pUserInfo->GetGuildIndex() != 0 )
	{
		ioGuildData *pGuildData = g_GuildInfo.GetGuildData( m_pUserInfo->GetGuildIndex() );
		if( pGuildData )
			pGuildData->ProcessTitleSync();

		HideChildWnd( ID_GUILD_INVITE );

		m_pGuildBtn = FindChildWnd( ID_GUILD_INFO );
		if( m_pGuildBtn )
			m_pGuildBtn->ShowWnd();
	}
	else
	{
		m_pGuildBtn = NULL;
		HideChildWnd( ID_GUILD_INVITE );
		HideChildWnd( ID_GUILD_INFO );
		if( m_bOnline && g_GuildInfo.IsGuild() && m_szTargetID != g_MyInfo.GetPublicID() )
		{
			m_pGuildBtn = FindChildWnd( ID_GUILD_INVITE );
			if( m_pGuildBtn )
				m_pGuildBtn->ShowWnd();
		}
	}

	if( !m_bOnline || m_dwTabID == ID_DETAIL_INFO )
	{
		if( m_pGuildBtn && m_pGuildBtn->IsShow() )
			m_pGuildBtn->HideWnd();
	}
}

void MannerValuationExtensionWnd::ProcessScroll()
{
	if( !m_pUserInfo ) return;
	if( !m_bOnline ) return;

	ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
	if( pScroll )
	{
		int iMaxScroll = 1;
		UserInfoCharData &rkUserCharData = m_pUserInfo->GetUserCharData();
		iMaxScroll += max( 0, rkUserCharData.GetMaxChar() - 1 ) / MAX_SLOT;

		if( pScroll->GetMaxPos() != iMaxScroll - 1 )
		{
			pScroll->SetScrollRange( 0, iMaxScroll );
			pScroll->SetScrollPos( m_iCurPage );
		}		
	}	

	UserInfoCharData &rkUserCharData = m_pUserInfo->GetUserCharData();
	rkUserCharData.ScrollSyncCharData( m_pUserInfo->GetUserID(), ( m_iCurPage + 1 ) * MAX_SLOT );
}

void MannerValuationExtensionWnd::ProcessToolTip()
{
	for(int i = 0;i < MAX_SLOT;i++)
	{
		MannerValuationSlotBtn *pSlotBtn = dynamic_cast<MannerValuationSlotBtn*>( FindChildWnd( ID_SOLDIER_SLOT_1 + i ) );
		if( pSlotBtn == g_GUIMgr.GetPreOverWnd() )
		{
			//툴팁 세팅
			MannerValuationSlotTooltip *pTooltip = dynamic_cast<MannerValuationSlotTooltip*>(FindChildWnd( ID_SLOT_TOOLTIP ));
			if( pTooltip )
			{
				pTooltip->SetTooltipInfo( m_pUserInfo, pSlotBtn->GetListArray() );
			}		
			return;
		}
	}

	// 툴팁 클리어
	MannerValuationSlotTooltip *pTooltip = dynamic_cast<MannerValuationSlotTooltip*>(FindChildWnd( ID_SLOT_TOOLTIP ));
	if( pTooltip )
	{
		pTooltip->SetTooltipInfo( NULL, -1 );
	}
}

void MannerValuationExtensionWnd::OnHeadquartersMove()
{
	if( m_pUserInfo == NULL )
		return;

	if( g_App.IsMouseBusy() )
		return;

	// 훈련소 정보창
	SP2Packet kPacket( CTPK_HEADQUARTERS_INFO );
	kPacket << m_pUserInfo->GetUserID();
	TCPNetwork::SendToServer( kPacket );
}

void MannerValuationExtensionWnd::OnHouseMove()
{
	if( m_pUserInfo == NULL )
		return;

	if( g_App.IsMouseBusy() )
		return;

	// 개인본부 정보창
	SP2Packet kPacket( CTPK_PERSONAL_HQ_INFO );
	kPacket << m_pUserInfo->GetUserID();
	TCPNetwork::SendToServer( kPacket );
}

void MannerValuationExtensionWnd::OnApplicationFriend()
{
	if( g_App.IsMouseBusy() )
		return;

	if( !g_FriendManager.IsLoadingComplete() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		return;
	}

	if( !g_App.IsRightID( m_szTargetID.c_str() ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
		return;
	}

	if( g_MyInfo.GetPublicID() == m_szTargetID )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
		return;
	}

	if( g_FriendManager.IsFriend( m_szTargetID ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4) );
		return;
	}

	int iSlotSize = g_FriendManager.GetSlotSize();
	if( g_FriendManager.GetNodeSize() >= iSlotSize )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(5), iSlotSize );
		return;
	}

	if( g_App.IsAdminID( m_szTargetID.c_str() ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(6) );
		return;
	}

	TCPNetwork::MouseBusy( true );
	SP2Packet kPacket( CTPK_FRIEND_APPLICATION );
	kPacket << m_szTargetID;
	TCPNetwork::SendToServer( kPacket );
}

void MannerValuationExtensionWnd::_OnDeleteFriend()
{
	if( !g_FriendManager.IsLoadingComplete() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		return;
	}

	if( !g_FriendManager.IsFriend( m_szTargetID ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
		return;
	}

	SP2Packet kPacket( CTPK_FRIEND_DELETE );
	kPacket << m_szTargetID;
	TCPNetwork::SendToServer( kPacket );

	g_FriendManager.DeleteFriend( m_szTargetID );
	g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );

	ShowChildWnd( ID_INSERT_FRIEND );
	HideChildWnd( ID_DELETE_FRIEND );

	ShowChildWnd( ID_INSERT_BESTFRIEND );
	HideChildWnd( ID_DELETE_BESTFRIEND );
}

void MannerValuationExtensionWnd::OnDeleteFriend()
{
	if( !g_FriendManager.IsLoadingComplete() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		return;
	}

	if( !g_FriendManager.IsFriend( m_szTargetID ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
		return;
	}

	if( g_FriendManager.GetBestFriendState( m_szTargetID ) == BFT_SET )
	{
		m_dwSysPopupMsg = ID_DELETE_FRIEND;
		g_GUIMgr.SetMsgBox( MB_YESNO, this, STR(3), m_szTargetID.c_str() );
	}
	else
	{
		SP2Packet kPacket( CTPK_FRIEND_DELETE );
		kPacket << m_szTargetID;
		TCPNetwork::SendToServer( kPacket );

		g_FriendManager.DeleteFriend( m_szTargetID );
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4) );

		ShowChildWnd( ID_INSERT_FRIEND );
		HideChildWnd( ID_DELETE_FRIEND );

		ShowChildWnd( ID_INSERT_BESTFRIEND );
		HideChildWnd( ID_DELETE_BESTFRIEND );
	}
}

void MannerValuationExtensionWnd::OnInsertBestFriend()
{
	if( g_App.IsMouseBusy() )
		return;

	if( !g_FriendManager.IsLoadingComplete() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		return;
	}

	if( m_szTargetID.IsEmpty() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
		return;
	}

	FriendNode *pFriend = g_FriendManager.GetNode( m_szTargetID );
	if( pFriend == NULL )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
		return;
	}

	if( g_FriendManager.GetBestFriendState( pFriend->GetUserIndex() ) == BFT_SET )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4), pFriend->GetName().c_str() );
		return;
	}
	
	SP2Packet kPacket( CTPK_INSERT_BESTFRIEND );
	kPacket << pFriend->GetUserIndex();
	TCPNetwork::SendToServer( kPacket );
	TCPNetwork::MouseBusy( true );
}

void MannerValuationExtensionWnd::_OnDeleteBestFriend()
{
	if( g_App.IsMouseBusy() )
		return;

	if( !g_FriendManager.IsLoadingComplete() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		return;
	}

	FriendNode *pFriendNode = g_FriendManager.GetNode( m_szTargetID );
	if( pFriendNode == NULL )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2), m_szTargetID.c_str() );
		return;
	}

	if( g_FriendManager.GetBestFriendState( pFriendNode->GetUserIndex() ) != BFT_SET )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3), pFriendNode->GetName().c_str() );
		return;
	}

	SP2Packet kPacket( CTPK_DISMISS_BESTFRIEND );
	kPacket << pFriendNode->GetUserIndex() << pFriendNode->GetName();
	TCPNetwork::SendToServer( kPacket );
	TCPNetwork::MouseBusy( true );

	ShowChildWnd( ID_INSERT_BESTFRIEND );
	HideChildWnd( ID_DELETE_BESTFRIEND );
}

void MannerValuationExtensionWnd::OnDeleteBestFriend()
{
	if( g_App.IsMouseBusy() )
		return;

	if( !g_FriendManager.IsLoadingComplete() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		return;
	}

	FriendNode *pFriendNode = g_FriendManager.GetNode( m_szTargetID );
	if( pFriendNode == NULL )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2), m_szTargetID.c_str() );
		return;
	}

	if( g_FriendManager.GetBestFriendState( pFriendNode->GetUserIndex() ) != BFT_SET )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3), pFriendNode->GetName().c_str() );
		return;
	}

	m_dwSysPopupMsg = ID_DELETE_BESTFRIEND;
	g_GUIMgr.SetMsgBox( MB_YESNO, this, STR(4), pFriendNode->GetName().c_str() );
}

void MannerValuationExtensionWnd::OnInsertBlack()
{
	if( g_App.IsMouseBusy() )
		return;

	if( g_BlackListManager.IsBlackList( m_szTargetID ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		return;
	}
	else if( g_App.IsAdminID( m_szTargetID.c_str() ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
		return;
	}
	else if( g_BlackListManager.GetNodeSize() >= ioBlackListManager::MAX_BLACK_LIST)
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
		return;
	}
	else if( g_MyInfo.GetPublicID() == m_szTargetID )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4) );
		return;
	}

	if( g_BlackListManager.InsertBlackList( m_szTargetID ) )
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(5), m_szTargetID.c_str() );

	HideChildWnd( ID_INSERT_BLACK );
	ShowChildWnd( ID_DELETE_BLACK );
}

void MannerValuationExtensionWnd::OnDeleteBlack()
{
	g_BlackListManager.RemoveBlackList( m_szTargetID );
	g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );

	ShowChildWnd( ID_INSERT_BLACK );
	HideChildWnd( ID_DELETE_BLACK );
}

void MannerValuationExtensionWnd::OnKickVote()
{
	if( !m_pPlayStage ) return;
	if( m_szTargetID.IsEmpty() ) return;

	if( g_BattleRoomMgr.IsTournamentModeType() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "대회 경기방은 강퇴가 되지 않습니다." );
		return;
	}

	if( g_RankBattleMgr.IsRankBattlePlaying() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "랭킹전은 강퇴 기능이 적용되지 않습니다." );
		return;
	}

	if( m_szTargetID == g_MyInfo.GetPublicID() )
	{
		KickVoteResultHelp( m_pPlayStage, USER_KICK_VOTE_PROPOSAL_ERROR_1, "", 0, 0, 0 );
		return;
	}

	if( g_App.IsAdminID( m_szTargetID.c_str() ) )
	{
		KickVoteResultHelp( m_pPlayStage, USER_KICK_VOTE_PROPOSAL_ERROR_2, "", 0, 0, 0 );
		return;
	}

	// 유저가 동일한 룸에 있는지 확인
	if( !m_pPlayStage->IsBaseChar( m_szTargetID ) )
	{
		KickVoteResultHelp( m_pPlayStage, USER_KICK_VOTE_PROPOSAL_ERROR_7, "", 0, 0, 0 );
		return;
	}

	// 차단상태인지 확인
	if( g_MyInfo.GetBlockType() != BKT_NORMAL )
	{
		KickVoteResultHelp( m_pPlayStage, USER_KICK_VOTE_PROPOSAL_ERROR_3, "", 0, 0, 0 );
		return;
	}	

	// 관전자는 투표 제안 X
	if( m_pPlayStage->GetModeType() != MT_TRAINING )
	{
		if( g_BattleRoomMgr.IsBattleRoom() && g_BattleRoomMgr.IsObserverUser( g_MyInfo.GetPublicID() ) )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
			return;
		}
	}
	else if( m_pPlayStage->GetModeType() == MT_TRAINING )
	{
		// 방장이 없는 광장에서는 강퇴 투표 불가
		PlazaMainWnd *pMainWnd = dynamic_cast<PlazaMainWnd*>(g_GUIMgr.FindWnd( PLAZA_MAIN_WND ));
		if( pMainWnd )
		{
			if( pMainWnd->GetPlazaMasterName().IsEmpty() )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
				return;
			}
			else if( pMainWnd->IsBugBearEmerge() )
			{
				g_GUIMgr.SetMsgBox( MB_OK,  NULL, "몬스터 출현으로 강퇴 기능이#일시적으로 중단됩니다." );			
				return;
			}
		}
	}

	// 현재 투표가 진행중인지 확인
	if( g_GUIMgr.IsShow( KICK_VOTE_PROGRESS_WND ) )
	{
		KickVoteResultHelp( m_pPlayStage, USER_KICK_VOTE_PROPOSAL_ERROR_5, "", 0, 0, 0 );
		return;
	}

	// 1회 투표 제안을 했던 유저 처리
	if( m_pPlayStage->GetKickOutVoteProposal() != 0 )
	{
		KickVoteResultHelp( m_pPlayStage, USER_KICK_VOTE_PROPOSAL_ERROR_6, "", 0, 0, 0 );
		return;
	}

	// 보스 유저는 신고 불가
	if( m_pPlayStage->GetModeType() == MT_BOSS )
	{
		if( m_pPlayStage->GetBaseCharTeam( m_szTargetID ) == TEAM_BLUE )
		{
			KickVoteResultHelp( m_pPlayStage, USER_KICK_VOTE_PROPOSAL_ERROR_13, "", 0, 0, 0 );
			return;
		}
	}

	// 진영전 플레이중 강퇴 불가
	if( m_pPlayStage->GetModeType() != MT_HEADQUARTERS && m_pPlayStage->GetModeType() != MT_TRAINING && m_pPlayStage->GetModeType() != MT_HOUSE && g_LadderTeamMgr.IsLadderTeam() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
		return;
	}

	ioPlayMode* pMode = m_pPlayStage->GetPlayMode();
	if( pMode )
	{
		DWORD dwEnableTime = g_ShuffleRoomMgr.GetKickOutVoteEnableTime();
		if( g_ShuffleRoomMgr.IsShuffleRoom() &&  dwEnableTime != 0 && dwEnableTime < FRAMEGETTIME() - pMode->GetModePlayStartTime() )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "강퇴 가능 시간이 초과하였습니다." );
			return;
		}
	}

	KickVoteReasonWnd *pKickVoteReason = dynamic_cast<KickVoteReasonWnd*>( g_GUIMgr.FindWnd( KICK_VOTE_REASON_WND ) );
	if( pKickVoteReason )
	{
		pKickVoteReason->SetKickUserInfo( m_szTargetID );
	}
	HideWnd();
}

void MannerValuationExtensionWnd::OnGuildInfo()
{
	if( !m_pUserInfo ) return;
	if( m_pUserInfo->GetGuildIndex() == 0 ) return;

	GuildMainWnd *pGuildMainWnd = dynamic_cast<GuildMainWnd *>(g_GUIMgr.FindWnd( GUILD_MAIN_WND ));
	if( pGuildMainWnd )
		pGuildMainWnd->SetGuildIndex( m_pUserInfo->GetGuildIndex() );
}

void MannerValuationExtensionWnd::OnGuildInvite()
{
	if( g_App.IsMouseBusy() ) return;	
	if( m_szTargetID.IsEmpty() ) return;
	if( m_pUserInfo->GetGuildIndex() != 0 )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		return;
	}

	if( m_szTargetID == g_MyInfo.GetPublicID() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
		return;
	}

	if( g_GuildInfo.IsInvitationUser( m_szTargetID ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
		return;
	}

	if( g_App.IsAdminID( m_szTargetID.c_str() ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4) );
		return;
	}

	ioGuildData *pGuildData = g_GuildInfo.GetGuildData( g_GuildInfo.GetGuildIndex() );
	if( pGuildData )
	{
		if( pGuildData->GetGuildJoinUser() != 0 && pGuildData->GetGuildMaxUser() != 0 )
		{
			if( pGuildData->GetGuildJoinUser() >= pGuildData->GetGuildMaxUser() )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(5) );
				return;
			}
		}
	}

	SP2Packet kPacket( CTPK_GUILD_INVITATION );
	kPacket << m_szTargetID;
	TCPNetwork::SendToServer( kPacket );
	TCPNetwork::MouseBusy( true );
}

void MannerValuationExtensionWnd::OnLeaderCharSelect( int iSelectArray )
{
	if( !IsShow() ) return;
	if( iSelectArray == -1 ) return;
	
	DWORD dwCharIndex = g_MyInfo.GetCharIndex( iSelectArray );
	if( dwCharIndex == 0 )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		return;
	}

	const CHARACTER &rkCharInfo = g_MyInfo.GetCharacter( iSelectArray );
	if( rkCharInfo.m_sLeaderType == CLT_LEADER )
		return;

	if( g_MyInfo.IsCharExercise( iSelectArray ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
		return;
	}

	SP2Packet kPacket( CTPK_CHANGE_MY_LEADER );
	kPacket << dwCharIndex;
	TCPNetwork::SendToServer( kPacket );
	TCPNetwork::MouseBusy( true );
}

void MannerValuationExtensionWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
		if( cmd == IOBN_BTNUP )
			HideWnd();
		break;
	case ID_HEADQUARTER_MOVE:
		if( cmd == IOBN_BTNUP )
		{
			OnHeadquartersMove();
		}
		break;
	case ID_HOME_MOVE:
		if( cmd == IOBN_BTNUP )
		{
			OnHouseMove();
		}
		else if( cmd == IOEX_BTNUP )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "해당 유저는 개인본부가# 활성화 되지 않았습니다." );
		}
		break;
	case ID_INSERT_FRIEND:
		if( cmd == IOBN_BTNUP )
		{
			OnApplicationFriend();
		}
		break;
	case ID_DELETE_FRIEND:
		if( cmd == IOBN_BTNUP )
		{
			OnDeleteFriend();
		}
		break;
	case ID_INSERT_BESTFRIEND:
		if( cmd == IOBN_BTNUP )
		{
			OnInsertBestFriend();
		}
		break;
	case ID_DELETE_BESTFRIEND:
		if( cmd == IOBN_BTNUP )
		{
			OnDeleteBestFriend();
		}
		break;
	case ID_INSERT_BLACK:
		if( cmd == IOBN_BTNUP )
		{
			OnInsertBlack();
		}
		break;
	case ID_DELETE_BLACK:
		if( cmd == IOBN_BTNUP )
		{
			OnDeleteBlack();
		}
		break;
	case ID_MEMO_CHAT:
		if( cmd == IOBN_BTNUP )
		{
			MannerValuationChatWnd *pChatSelectWnd = dynamic_cast< MannerValuationChatWnd * >( g_GUIMgr.FindWnd( MANNER_VALUATION_CHAT_WND ) );
			if( pChatSelectWnd )
			{
				pChatSelectWnd->SetChatUser( m_pUserInfo, m_bOnline );
			}
		}
		break;
	case ID_KICK_VOTE:
		if( cmd == IOBN_BTNUP )
		{
			OnKickVote();
		}
		break;
	case ID_GUILD_INFO:
		if( cmd == IOBN_BTNUP )
		{
			OnGuildInfo();
		}
		break;
	case ID_GUILD_INVITE:
		if( cmd == IOBN_BTNUP )
		{
			OnGuildInvite();
		}
		break;
	case ID_MINUS_MANNER_TRIAL:
		if( cmd == IOBN_BTNUP )
		{
			if( g_MyInfo.GetEntryType() == ET_TERMINATION || g_MyInfo.GetEntryType() == ET_TEMPORARY ||
				g_MyInfo.IsExpertEntryTemporary() )
			{
				g_App.OpenFormalityEntryPage();
			}
			else
			{
				MannerTrialWnd *pTrialWnd = dynamic_cast<MannerTrialWnd *>(g_GUIMgr.FindWnd( MANNER_TRIAL_WND ));
				if(pTrialWnd)
				{
					pTrialWnd->SetMannerTrialInfo( m_szTargetID , m_eTrialType, m_dwChannelIndex );
					pTrialWnd->ShowWnd();
				}
			}
		}
		break;
	case ID_HEADQUARTER:
		if( cmd == IOBN_BTNUP )
		{
			if( !m_szTargetID.IsEmpty() )
			{
				if( m_szTargetID == STR(1) )
					g_App.OpenHeadQuaterPage( m_szTargetID.c_str(), false );
				else
					g_App.OpenHeadQuaterPage( m_szTargetID.c_str() );
			}
		}
		break;
	case ID_HERO_RANKING:
		if( cmd == IOBN_BTNUP )
		{
			GuildHeroRankingListWnd *pHeroList = dynamic_cast<GuildHeroRankingListWnd*>(g_GUIMgr.FindWnd( GUILD_HERO_RANKING_LIST_WND ));
			if( pHeroList )				
			{
				pHeroList->SetRankRadioBtn( GuildHeroRankingListWnd::ID_HERO_TOP100_BTN );
				pHeroList->ShowWnd();
			}
		}
		break;
	case ID_NAMED_TITLE:
		if( cmd == IOBN_BTNUP )
		{
			NamedTitleSelectWnd *pWnd = dynamic_cast<NamedTitleSelectWnd*>( g_GUIMgr.FindWnd( NAMED_TITLE_SELECT_WND ) );
			if( pWnd )
				pWnd->ShowSelectWnd();
		}
		break;
	case ID_TOTAL_INFO:
	case ID_DETAIL_INFO:
		if( cmd == IOBN_BTNDOWN )
		{
			m_dwTabID = dwID;
			CheckRadioButton( ID_TOTAL_INFO, ID_DETAIL_INFO, m_dwTabID );
			ProcessGuildButton( false );
		}
		break;
	case UI_SOLDIERSELECT_WND:
		if( cmd == IOBN_BTNUP )
		{
			OnLeaderCharSelect( param );
		}
		break;
	case MESSAGE_BOX:
		if( cmd == IOBN_BTNUP )
		{
			if( param != IDYES ) return;
            
			switch( m_dwSysPopupMsg )
			{
			case ID_DELETE_FRIEND:
				{
					_OnDeleteFriend();
				}
				break;
			case ID_DELETE_BESTFRIEND:
				{
					_OnDeleteBestFriend();
				}
				break;
			}
		}
		break;
	}
}

void MannerValuationExtensionWnd::SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate /* = false  */ )
{
	ioMovingWnd::SetPositionToDefault( iParentWidth, iParentHeight, bChildMove, bCreate );
	if( !bCreate )
	{
		if( IsShow() )
		{
			ProcessGuildButton();
		}
	}
}

void MannerValuationExtensionWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	if( !m_bOnline )        //오프라인 상태인데 동기화가 되었으면 온라인 상태로 판단
	{
		if( m_pUserInfo && m_pUserInfo->IsSyncInfo() )
			SetMannerValuationInfo( m_szTargetID, true, m_eTrialType, m_dwChannelIndex );
	}
	else
	{
		ProcessScroll();
		ProcessToolTip();
	}
	ProcessGuildButton( true );

	if( sg_bUpdateInfo )
	{
		SetButton();
	}
	ioMovingWnd::OnProcess( fTimePerSec );
}

void MannerValuationExtensionWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	int nFriendOffsetX = 0;
	if( m_pUserInfo )
	{
		FriendNode *pFriendNode = g_FriendManager.GetNode( m_pUserInfo->GetUserID() );
		if( pFriendNode )
		{
			DWORD dwBFState = g_FriendManager.GetBestFriendState( m_pUserInfo->GetUserID() );
			float fNameSize = g_FontMgr.GetTextWidth( m_pUserInfo->GetUserID().c_str(), TS_OUTLINE_FULL_2X, FONT_SIZE_17 );
			if( pFriendNode->IsOtherBF() && dwBFState == BFT_SET )
			{
				if( m_bOnline && m_pCoupleBestFriend )
				{
					m_pCoupleBestFriend->Render( iXPos + 14 + fNameSize, iYPos + 12 );
					nFriendOffsetX = 48;
				}
				else if( m_pMyBestFriend )
				{
					m_pMyBestFriend->Render( iXPos + 14 + fNameSize, iYPos + 12 );
					nFriendOffsetX = 31;
				}
			}
			else if( dwBFState == BFT_SET )
			{
				if( m_pMyBestFriend )
				{
					m_pMyBestFriend->Render( iXPos + 14 + fNameSize, iYPos + 12 );
					nFriendOffsetX = 31;
				}
			}
			else if( pFriendNode->IsOtherBF() )
			{
				if( m_bOnline && m_pOtherBestFriend )
				{
					m_pOtherBestFriend->Render( iXPos + 14 + fNameSize, iYPos + 12 );
					nFriendOffsetX = 31;
				}
			}
		}
	}

	OnRenderNamedTitle( iXPos, iYPos, nFriendOffsetX );
	OnRenderDot( iXPos + 312, iYPos + 54 );
	OnRenderGrade( iXPos, iYPos );
	OnRenderHeroTitle( iXPos + 212, iYPos );

	switch( m_dwTabID )
	{
	case ID_TOTAL_INFO:
		OnRenderGradeInfo( iXPos, iYPos + 323 );
		OnRenderKillDeathLevel( iXPos, iYPos + 344 );
		OnRenderLadderInfo( iXPos, iYPos + 365 );
		OnRenderUserManner( iXPos, iYPos + 386 );
		OnRenderUserCamp( iXPos, iYPos + 407 );
		OnRenderGuild( iXPos, iYPos + 428 );
		break;
	case ID_DETAIL_INFO:
		OnRenderUserBattle( iXPos, iYPos + 323 );
		OnRenderUserLadder( iXPos, iYPos + 344 );
		OnRenderUserAward( iXPos, iYPos + 365 );		
		OnRenderUserFishing( iXPos, iYPos + 386 );
		OnRenderUserExcavation( iXPos, iYPos + 407 );
		OnRenderSuccession( iXPos, iYPos + 428 );
		break;
	}
}

void MannerValuationExtensionWnd::OnRenderDot( int iXPos, int iYPos )
{
	if( !m_pDot ) return;

	for(int i = 0;i < MAX_DOT;i++)
	{
		m_pDot->Render( iXPos, iYPos + i * 5 );
	}
}

void MannerValuationExtensionWnd::OnRenderGrade( int iXPos, int iYPos )
{
	if( !m_pNoneIcon || !m_pUserInfo || !m_pIconBack ) return;

	// 계급을 알수없다.
	if( m_pUserInfo->GetGradeLevel() == -1 || !m_bOnline )
	{
		m_pNoneIcon->SetScale( 0.80f );
		m_pNoneIcon->Render( iXPos + 50, iYPos + 83, UI_RENDER_MULTIPLY, TFO_BILINEAR );
		m_pNoneIcon->Render( iXPos + 50, iYPos + 83, UI_RENDER_MULTIPLY, TFO_BILINEAR );
	}
	else
	{
		if( m_pUserInfo->GetGradeLevel() != m_iPrevGradeLevel )
		{
			m_iPrevGradeLevel = m_pUserInfo->GetGradeLevel();
			SAFEDELETE( m_pGradeIcon );
			m_pGradeIcon = g_LevelMgr.GetGradeCircleIcon( m_pUserInfo->GetGradeLevel() );
		}

		if( m_pIconBack && m_pGradeIcon )
		{
			m_pIconBack->SetScale( 0.80f );
			m_pIconBack->Render( iXPos + 50, iYPos + 83, UI_RENDER_MULTIPLY, TFO_BILINEAR );

			m_pGradeIcon->SetScale( 0.80f );
			m_pGradeIcon->Render( iXPos + 50, iYPos + 83, UI_RENDER_NORMAL, TFO_BILINEAR );
		}
	}

	if( m_pUserInfo->GetGradeLevel() == -1 || !m_bOnline )
	{
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetBkColor( 0, 0, 0 );	
		g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		g_FontMgr.PrintText( iXPos + 51, iYPos + 120, FONT_SIZE_12, STR(1) );
	}
	else
	{	
		char szGradeName[MAX_PATH] = "";
		g_LevelMgr.GetGradeName( m_pUserInfo->GetGradeLevel(), szGradeName, MAX_PATH, true, true );
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetBkColor( 0, 0, 0 );	
		g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		g_FontMgr.PrintTextWidthCut( iXPos + 51, iYPos + 114, FONT_SIZE_12, 76.0f, szGradeName );

		g_FontMgr.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
		if( m_pUserInfo->GetRanking() == 0 )
			g_FontMgr.PrintText( iXPos + 51, iYPos + 128, FONT_SIZE_12, STR(2) );
		else
			g_FontMgr.PrintTextWidthCut( iXPos + 51, iYPos + 128, FONT_SIZE_12, 76.0f, STR(3), m_pUserInfo->GetRanking() );
	}
}

void MannerValuationExtensionWnd::OnRenderHeroTitle( int iXPos, int iYPos )
{
	if( !m_pNoneIcon || !m_pUserInfo ) return;

	// 영웅 계급을 알수없다.
	if( m_pUserInfo->GetHeroTitle() == 0 || !m_bOnline )
	{
		m_pNoneIcon->SetScale( 0.80f );
		m_pNoneIcon->Render( iXPos + 50, iYPos + 83, UI_RENDER_MULTIPLY, TFO_BILINEAR );
		m_pNoneIcon->Render( iXPos + 50, iYPos + 83, UI_RENDER_MULTIPLY, TFO_BILINEAR );
	}
	else
	{
		if( m_pIconBack )
		{
			m_pIconBack->SetScale( 0.80f );
			m_pIconBack->Render( iXPos + 50, iYPos + 83, UI_RENDER_MULTIPLY );
			g_LevelMgr.RenderHeroTitleIcon( m_pUserInfo->GetHeroTitle(), iXPos + 50, iYPos + 83, 0.80f );
		}
	}
	
	if( m_pUserInfo->GetGradeLevel() == -1 || !m_bOnline )
	{
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetBkColor( 0, 0, 0 );	
		g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		g_FontMgr.PrintText( iXPos + 51, iYPos + 120, FONT_SIZE_12, STR(1) );
	}
	else
	{
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetBkColor( 0, 0, 0 );	
		g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		g_FontMgr.PrintTextWidthCut( iXPos + 51, iYPos + 114, FONT_SIZE_12, 76.0f, g_LevelMgr.GetHeroTitleString( m_pUserInfo->GetHeroTitle() ).c_str() );

		g_FontMgr.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
		if( m_pUserInfo->GetHeroRank() == 0 )
			g_FontMgr.PrintText( iXPos + 51, iYPos + 128, FONT_SIZE_12, STR(2) );
		else
			g_FontMgr.PrintTextWidthCut( iXPos + 51, iYPos + 128, FONT_SIZE_12, 109.0f, STR(3), m_pUserInfo->GetHeroRank() );
	}
}

void MannerValuationExtensionWnd::OnRenderGradeInfo( int iXPos, int iYPos )
{
	if( m_pUserInfo == NULL ) return;

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetBkColor( 0, 0, 0 );	
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 23, iYPos, FONT_SIZE_12, STR(1) );

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_RIGHT );
	g_FontMgr.SetBkColor( 0, 0, 0 );	
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );

	if( m_bOnline )
	{
		int iGradeLevel = m_pUserInfo->GetGradeLevel();
		char szGradeName[MAX_PATH] = "";
		g_LevelMgr.GetGradeName( iGradeLevel, szGradeName, sizeof( szGradeName ), true );
		if( COMPARE( iGradeLevel, 50, 54 + 1 ) )
		{
			char szConvertText[MAX_PATH] = "";
			Help::ConvertNumToStrComma( m_pUserInfo->GetGradeExp(), szConvertText, sizeof( szConvertText ) );
			g_FontMgr.PrintText( iXPos + 290, iYPos, FONT_SIZE_12, "%s (%s점)", szGradeName, szConvertText );
		}
		else
		{
			g_FontMgr.PrintText( iXPos + 290, iYPos, FONT_SIZE_12, STR(2), szGradeName, m_pUserInfo->GetGradeExp() );
		}
	}
	else
	{
		g_FontMgr.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
		g_FontMgr.PrintText( iXPos + 290, iYPos, FONT_SIZE_12, STR(3) );
	}
}

void MannerValuationExtensionWnd::OnRenderKillDeathLevel( int iXPos, int iYPos )
{
	if( m_pUserInfo == NULL ) return;

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetBkColor( 0, 0, 0 );	
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 23, iYPos, FONT_SIZE_12, STR(1) );
	
	ioComplexStringPrinter kPrinter;
	kPrinter.SetBkColor( 0, 0, 0 );
	if( m_bOnline )
	{
		kPrinter.SetTextStyle( TS_NORMAL );
		kPrinter.SetTextColor( TCT_DEFAULT_RED );

		char szGradeName[MAX_PATH] = "";
		g_LevelMgr.GetGradeName( min( g_LevelMgr.GetMaxGradeLevel(), max( 0, m_pUserInfo->GetKillDeathLevel() - g_LevelMgr.GetAddGradeLevel() ) ), szGradeName, sizeof( szGradeName ), true );

		kPrinter.AddTextPiece( FONT_SIZE_12, STR(2), szGradeName );		
		kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter.AddTextPiece( FONT_SIZE_12, STR(3) );		
	}
	else
	{
		kPrinter.SetTextStyle( TS_NORMAL );
		kPrinter.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
		kPrinter.AddTextPiece( FONT_SIZE_12, STR(4) );
	}
	kPrinter.PrintFullText( iXPos + 290, iYPos, TAT_RIGHT );
}

void MannerValuationExtensionWnd::OnRenderLadderInfo( int iXPos, int iYPos )
{
	if( m_pUserInfo == NULL ) return;

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetBkColor( 0, 0, 0 );	
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 23, iYPos, FONT_SIZE_12, STR(1) );

	//
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_RIGHT );
	g_FontMgr.SetBkColor( 0, 0, 0 );		
	if( m_bOnline )
	{
		if( m_pUserInfo->GetCampPosition() == CAMP_BLUE )
			g_FontMgr.SetTextColor( TCT_DEFAULT_BLUE );
		else if( m_pUserInfo->GetCampPosition() == CAMP_RED )
			g_FontMgr.SetTextColor( TCT_DEFAULT_RED );
		else
			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );

		int iWinLoseRate = m_pUserInfo->GetHeroRecordRate() * 100;
		g_FontMgr.PrintText( iXPos + 290, iYPos, FONT_SIZE_12, STR(2),
			m_pUserInfo->GetHeroWin(), m_pUserInfo->GetHeroLose(), iWinLoseRate );
	}
	else
	{
		g_FontMgr.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
		g_FontMgr.PrintText( iXPos + 290, iYPos, FONT_SIZE_12, STR(3) );
	}
}

void MannerValuationExtensionWnd::OnRenderUserManner( int iXPos, int iYPos )
{
	if( m_pUserInfo == NULL ) return;

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetBkColor( 0, 0, 0 );

	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 23, iYPos, FONT_SIZE_12, STR(1) );

	g_FontMgr.SetAlignType( TAT_RIGHT );
	if( m_bOnline )
	{
		if( m_pUserInfo->GetBlockType() == BKT_NORMAL )
		{
			g_FontMgr.SetTextColor( TCT_DEFAULT_GREEN );
			g_FontMgr.PrintText( iXPos + 290, iYPos, FONT_SIZE_12, STR(2) );	
		}
		else if( m_pUserInfo->GetBlockType() == BKT_WARNNING )
		{
			g_FontMgr.SetTextColor( 206,78,0 );
			g_FontMgr.PrintText( iXPos + 290, iYPos, FONT_SIZE_12, STR(3) );	
		}
		else if( m_pUserInfo->GetBlockType() == BKT_LIMITATION )
		{
			g_FontMgr.SetTextColor( TCT_DEFAULT_RED );
			g_FontMgr.PrintText( iXPos + 290, iYPos, FONT_SIZE_12, STR(4) );	
		}
		else
		{
			g_FontMgr.SetTextColor( TCT_DEFAULT_RED );
			g_FontMgr.PrintText( iXPos + 290, iYPos, FONT_SIZE_12, STR(5) );	
		}		
	}
	else
	{
		g_FontMgr.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
		g_FontMgr.PrintText( iXPos + 290, iYPos, FONT_SIZE_12, STR(6) );
	}
}

void MannerValuationExtensionWnd::OnRenderUserCamp( int iXPos, int iYPos )
{
	if( m_pUserInfo == NULL ) return;	

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetBkColor( 0, 0, 0 );

	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 23, iYPos, FONT_SIZE_12, STR(1) );

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );		
	if( m_bOnline )
	{
		if( m_pUserInfo->GetCampPosition() == CAMP_BLUE )
		{
			kPrinter.SetTextColor( TCT_DEFAULT_BLUE );
			kPrinter.AddTextPiece( FONT_SIZE_12, m_szCampName.c_str() );

			kPrinter.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
			kPrinter.AddTextPiece( FONT_SIZE_12, " | " );

			char szConvertText[MAX_PATH] = "";
			Help::ConvertNumToStrComma( m_pUserInfo->GetLadderPoint(), szConvertText, sizeof( szConvertText ) );
			kPrinter.SetTextColor( TCT_DEFAULT_BLUE );
			kPrinter.AddTextPiece( FONT_SIZE_12, STR(3), szConvertText );
		}
		else if( m_pUserInfo->GetCampPosition() == CAMP_RED )
		{
			kPrinter.SetTextColor( TCT_DEFAULT_RED );
			kPrinter.AddTextPiece( FONT_SIZE_12, m_szCampName.c_str() );

			kPrinter.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
			kPrinter.AddTextPiece( FONT_SIZE_12, " | " );

			char szConvertText[MAX_PATH] = "";
			Help::ConvertNumToStrComma( m_pUserInfo->GetLadderPoint(), szConvertText, sizeof( szConvertText ) );
			kPrinter.SetTextColor( TCT_DEFAULT_RED );
			kPrinter.AddTextPiece( FONT_SIZE_12, STR(5), szConvertText );
		}
		else
		{
			kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
			kPrinter.AddTextPiece( FONT_SIZE_12, STR(6) );
		}
	}
	else
	{
		kPrinter.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
		kPrinter.AddTextPiece( FONT_SIZE_12, STR(7) );
	}
	kPrinter.PrintFullText( iXPos + 290, iYPos, TAT_RIGHT );
	kPrinter.ClearList();
}

void MannerValuationExtensionWnd::OnRenderGuild( int iXPos, int iYPos )
{
	if( m_pUserInfo == NULL ) return;	
	if( m_pInfoOver == NULL ) return;

	DWORD dwDarkGrayColor = TCT_DEFAULT_DARKGRAY;
	DWORD dwOrangeColor = TCT_DEFAULT_ORANGE;
	DWORD dwLightGrayColor = TCT_DEFAULT_LIGHTGRAY;
	if( m_pGuildBtn && m_pGuildBtn->IsShow() )
	{
		if( m_pGuildBtn->IsOver() || m_pGuildBtn->IsClicked() )
		{
			dwDarkGrayColor = dwLightGrayColor = dwOrangeColor = TCT_DEFAULT_WHITE;
			m_pInfoOver->Render( iXPos + 17, iYPos - 3 );
		}
	}
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetBkColor( 0, 0, 0 );

	g_FontMgr.SetTextColor( dwDarkGrayColor );
	g_FontMgr.PrintText( iXPos + 23, iYPos, FONT_SIZE_12, STR(1) );

	g_FontMgr.SetAlignType( TAT_RIGHT );
	if( m_bOnline )
	{
		if( m_pUserInfo->GetGuildIndex() == 0 )
		{
			if( g_GuildInfo.IsGuild() )
			{
				g_FontMgr.SetTextColor( dwOrangeColor );
				g_FontMgr.PrintText( iXPos + 290, iYPos, FONT_SIZE_12, STR(2) );
			}
			else
			{
				g_FontMgr.SetTextColor( dwDarkGrayColor );
				g_FontMgr.PrintText( iXPos + 290, iYPos, FONT_SIZE_12, STR(3) );
			}
		}
		else
		{
			ioGuildData *pGuildData = g_GuildInfo.GetGuildData( m_pUserInfo->GetGuildIndex() );
			if( pGuildData )
			{
				g_FontMgr.SetTextColor( dwOrangeColor );
				g_FontMgr.PrintText( iXPos + 290, iYPos, FONT_SIZE_12, pGuildData->GetGuildName().c_str() );

				int iRealGuildName = g_FontMgr.GetTextWidth( pGuildData->GetGuildName().c_str(), TS_NORMAL, FONT_SIZE_12 ) + 16;    // 16은 길드 마크와 갭
				g_GuildMarkMgr.RenderSmallMark( pGuildData->GetGuildIndex(), pGuildData->GetGuildMark(), iXPos + 290 - iRealGuildName, iYPos + 1 );
			}
		}
	}	
	else
	{
		g_FontMgr.SetTextColor( dwLightGrayColor );
		g_FontMgr.PrintText( iXPos + 290, iYPos, FONT_SIZE_12, STR(4) );
	}

}

void MannerValuationExtensionWnd::OnRenderUserBattle( int iXPos, int iYPos )
{
	if( m_pUserInfo == NULL ) return;	

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetBkColor( 0, 0, 0 );
    
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 23, iYPos, FONT_SIZE_12, STR(1) );

	ioComplexStringPrinter kPrinter;

	kPrinter.SetBkColor( 0, 0, 0 );
	if( m_bOnline )
	{
		kPrinter.SetTextStyle( TS_NORMAL );

		kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter.AddTextPiece( FONT_SIZE_12, STR(2), m_pUserInfo->GetPartyLevel() );	

		kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
		kPrinter.AddTextPiece( FONT_SIZE_12, STR(3), m_pUserInfo->GetPartyExp() );	
	}
	else
	{
		kPrinter.SetTextStyle( TS_NORMAL );
		kPrinter.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
		kPrinter.AddTextPiece( FONT_SIZE_12, STR(4) );
	}
	kPrinter.PrintFullText( iXPos + 290, iYPos, TAT_RIGHT );
}

void MannerValuationExtensionWnd::OnRenderUserAward( int iXPos, int iYPos )
{
	if( m_pUserInfo == NULL ) return;	

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetBkColor( 0, 0, 0 );

	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 23, iYPos, FONT_SIZE_12, STR(1) );

	ioComplexStringPrinter kPrinter;

	kPrinter.SetBkColor( 0, 0, 0 );
	if( m_bOnline )
	{
		kPrinter.SetTextStyle( TS_NORMAL );

		kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter.AddTextPiece( FONT_SIZE_12, STR(2), m_pUserInfo->GetAwardLevel() );	

		kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
		kPrinter.AddTextPiece( FONT_SIZE_12, STR(3), m_pUserInfo->GetAwardExp() );	
	}
	else
	{
		kPrinter.SetTextStyle( TS_NORMAL );
		kPrinter.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
		kPrinter.AddTextPiece( FONT_SIZE_12, STR(4) );
	}
	kPrinter.PrintFullText( iXPos + 290, iYPos, TAT_RIGHT );
}

void MannerValuationExtensionWnd::OnRenderUserLadder( int iXPos, int iYPos )
{
	if( m_pUserInfo == NULL ) return;	

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetBkColor( 0, 0, 0 );

	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 23, iYPos, FONT_SIZE_12, STR(1) );

	ioComplexStringPrinter kPrinter;

	kPrinter.SetBkColor( 0, 0, 0 );
	if( m_bOnline )
	{
		kPrinter.SetTextStyle( TS_NORMAL );

		kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter.AddTextPiece( FONT_SIZE_12, STR(2), m_pUserInfo->GetLadderLevel() );	

		kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
		kPrinter.AddTextPiece( FONT_SIZE_12, STR(3), m_pUserInfo->GetLadderExp() );	
	}
	else
	{
		kPrinter.SetTextStyle( TS_NORMAL );
		kPrinter.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
		kPrinter.AddTextPiece( FONT_SIZE_12, STR(4) );
	}
	kPrinter.PrintFullText( iXPos + 290, iYPos, TAT_RIGHT );
}

void MannerValuationExtensionWnd::OnRenderUserFishing( int iXPos, int iYPos )
{
	if( m_pUserInfo == NULL ) return;	

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetBkColor( 0, 0, 0 );

	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 23, iYPos, FONT_SIZE_12, STR(1) );

	ioComplexStringPrinter kPrinter;

	kPrinter.SetBkColor( 0, 0, 0 );
	if( m_bOnline )
	{
		kPrinter.SetTextStyle( TS_NORMAL );

		kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter.AddTextPiece( FONT_SIZE_12, STR(2), m_pUserInfo->GetFishingLevel() );	

		kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
		kPrinter.AddTextPiece( FONT_SIZE_12, STR(3), m_pUserInfo->GetFishingExp() );	
	}
	else
	{
		kPrinter.SetTextStyle( TS_NORMAL );
		kPrinter.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
		kPrinter.AddTextPiece( FONT_SIZE_12, STR(4) );
	}
	kPrinter.PrintFullText( iXPos + 290, iYPos, TAT_RIGHT );
}

void MannerValuationExtensionWnd::OnRenderUserExcavation( int iXPos, int iYPos )
{
	if( m_pUserInfo == NULL ) return;	

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetBkColor( 0, 0, 0 );

	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 23, iYPos, FONT_SIZE_12, STR(1) );

	ioComplexStringPrinter kPrinter;

	kPrinter.SetBkColor( 0, 0, 0 );
	if( m_bOnline )
	{
		kPrinter.SetTextStyle( TS_NORMAL );

		kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter.AddTextPiece( FONT_SIZE_12, STR(2), m_pUserInfo->GetExcavationLevel() );	

		kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
		kPrinter.AddTextPiece( FONT_SIZE_12,STR(3), m_pUserInfo->GetExcavationExp() );	
	}
	else
	{
		kPrinter.SetTextStyle( TS_NORMAL );
		kPrinter.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
		kPrinter.AddTextPiece( FONT_SIZE_12, STR(4) );
	}
	kPrinter.PrintFullText( iXPos + 290, iYPos, TAT_RIGHT );
}

void MannerValuationExtensionWnd::OnRenderSuccession( int iXPos, int iYPos )
{
	if( m_pUserInfo == NULL )
		return;	

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetBkColor( 0, 0, 0 );

	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 23, iYPos, FONT_SIZE_12, "1vs1모드" );

	ioComplexStringPrinter kPrinter;

	kPrinter.SetBkColor( 0, 0, 0 );
	if( m_bOnline )
	{
		kPrinter.SetTextStyle( TS_NORMAL );
		kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter.AddTextPiece( FONT_SIZE_12, "최대연승" );

		kPrinter.SetTextColor( TCT_DEFAULT_RED );
		kPrinter.AddTextPiece( FONT_SIZE_12, " %d ", m_pUserInfo->GetMaxSuccessionCount() );

		kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
		kPrinter.AddTextPiece( FONT_SIZE_12, "(현재연승 %d)", m_pUserInfo->GetCurSuccessionCount() );
	}
	else
	{
		kPrinter.SetTextStyle( TS_NORMAL );
		kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter.AddTextPiece( FONT_SIZE_12, "최대연승" );

		kPrinter.SetTextColor( TCT_DEFAULT_RED );
		kPrinter.AddTextPiece( FONT_SIZE_12, " 0 " );

		kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
		kPrinter.AddTextPiece( FONT_SIZE_12, "(현재연승 0)" );
	}
	kPrinter.PrintFullText( iXPos + 290, iYPos, TAT_RIGHT );
}

void MannerValuationExtensionWnd::SetPlayStage( ioPlayStage * pPlayStage )
{
	m_pPlayStage = pPlayStage;

	MannerValuationLeaderSlotWnd* pLeaderSlot = dynamic_cast<MannerValuationLeaderSlotWnd*>( FindChildWnd( ID_LEADER_SLOT ) );
	if( pLeaderSlot )
	{
		MannerValuationControlWnd* pControlWnd = dynamic_cast<MannerValuationControlWnd*>( pLeaderSlot->FindChildWnd( MannerValuationLeaderSlotWnd::ID_CHAR_CONTROL_WND ) );
		if( pControlWnd )
		{
			pControlWnd->SetPlayStage( m_pPlayStage );
		}
	}
}

void MannerValuationExtensionWnd::OnRenderNamedTitle( int nXPos, int nYPos, int nFriendOffsetX )
{
	if ( m_szNamedTitle.IsEmpty() )
		return;

	int nNameWidth = g_FontMgr.GetTextWidth( m_szTargetID.c_str(), GetTitleTextStyle(), FONT_SIZE_17 );
	g_FontMgr.SetAlignType( GetTitleTextHorzAlign() );
	g_FontMgr.SetTextStyle( GetTitleTextStyle() );
	g_FontMgr.SetBkColor( 12, 66, 111 );

	if ( m_byNamedTitlePremium && m_pNamedTitleUiMark )
	{
		int nMaxWidth = 577 - 63 - nNameWidth - nFriendOffsetX - 3;
		int nMarkStartX = nXPos + GetTitleOffsetX() + nNameWidth + 5 + nFriendOffsetX;
		int nMarkWidth = m_pNamedTitleUiMark->GetWidth();

		m_pNamedTitleUiMark->Render( nMarkStartX + nMarkWidth / 2, nYPos + GetTitleOffsetY(), UI_RENDER_NORMAL, TFO_BILINEAR );
		m_pNamedTitleUiMark->SetReverseFormatting( ioUIRenderImage::RF_HORZ_REVERSE );

		g_FontMgr.SetTextColor( 240, 189, 48 );
		g_FontMgr.PrintTextWidthCut( nMarkStartX + nMarkWidth, nYPos + GetTitleOffsetY(), FONT_SIZE_17, nMaxWidth, "%s", m_szNamedTitle.c_str() );

		int nNameTitleWidth = g_FontMgr.GetTextWidth( m_szNamedTitle.c_str(), GetTitleTextStyle(), FONT_SIZE_17 );
		int nLastXPos = nMarkStartX + nMarkWidth + nNameTitleWidth + nMarkWidth/2 + 2;
		if ( nNameTitleWidth >= nMaxWidth )
		{
			nNameTitleWidth = g_FontMgr.GetTextWidthCutSize( m_szNamedTitle.c_str(), GetTitleTextStyle(), FONT_SIZE_17, nMaxWidth );
			nLastXPos = nMarkStartX + nMarkWidth + nNameTitleWidth + nMarkWidth/2 + 6;
		}

		m_pNamedTitleUiMark->Render( nLastXPos, nYPos + GetTitleOffsetY(), UI_RENDER_NORMAL, TFO_BILINEAR );
		m_pNamedTitleUiMark->SetReverseFormatting( ioUIRenderImage::RF_NONE_REVERSE );		
	}
	else
	{
		int nMaxWidth = 577 - 17 - nNameWidth - nFriendOffsetX - 3;
		int nMarkStartX = nXPos + GetTitleOffsetX() + nNameWidth + 5 + nFriendOffsetX;
		int nMarkWidth = g_FontMgr.GetTextWidth( "[", GetTitleTextStyle(), FONT_SIZE_17 );

		g_FontMgr.SetTextColor( 255, 255, 255 );
		g_FontMgr.PrintText( nMarkStartX, nYPos + GetTitleOffsetY(), FONT_SIZE_17, "[" );
		g_FontMgr.PrintTextWidthCut( nMarkStartX + nMarkWidth - 2, nYPos + GetTitleOffsetY(), FONT_SIZE_17, nMaxWidth, "%s", m_szNamedTitle.c_str() );

		int nNameTitleWidth = g_FontMgr.GetTextWidth( m_szNamedTitle.c_str(), GetTitleTextStyle(), FONT_SIZE_17 );
		int nLastXPos = nMarkStartX + nMarkWidth + nNameTitleWidth - 4;
		if ( nNameTitleWidth >= nMaxWidth )
		{
			nNameTitleWidth = g_FontMgr.GetTextWidthCutSize( m_szNamedTitle.c_str(), GetTitleTextStyle(), FONT_SIZE_17, nMaxWidth );
			nLastXPos = nMarkStartX + nMarkWidth + nNameTitleWidth + 2;
		}

		g_FontMgr.PrintText( nLastXPos, nYPos + GetTitleOffsetY(), FONT_SIZE_17, "]" );
	}
}

void MannerValuationExtensionWnd::ApplyTitle()
{
	SetTitleText( m_szTargetID.c_str() );
	m_szNamedTitle.Clear();
	m_byNamedTitlePremium = 0;
	SAFEDELETE( m_pNamedTitleUiMark );

	if ( m_szTargetID == g_MyInfo.GetPublicID() )
	{
		int nNamedTitleCode = g_MyInfo.GetNamedTitleCode();
		if ( nNamedTitleCode > 0 )
		{
			m_szNamedTitle = g_NamedTitleInfoMgr.GetNamedTitleName( nNamedTitleCode );
			m_byNamedTitlePremium = g_MyInfo.GetNamedTitlePremium();
			m_pNamedTitleUiMark = g_UIImageSetMgr.CreateImageByFullName( g_NamedTitleInfoMgr.GetNamedTitleUiMark() );
		}
	}
	else
	{
		UserInfoNode *pTargetNode = g_UserInfoMgr.GetUserInfoNode( m_szTargetID, false );
		if ( pTargetNode && pTargetNode->GetNamedTitleCode() > 0 )
		{
			m_szNamedTitle = g_NamedTitleInfoMgr.GetNamedTitleName( pTargetNode->GetNamedTitleCode() );
			m_byNamedTitlePremium = pTargetNode->GetNamedTitlePremium();
			m_pNamedTitleUiMark = g_UIImageSetMgr.CreateImageByFullName( g_NamedTitleInfoMgr.GetNamedTitleUiMark() );
		}
	}
}

void MannerValuationExtensionWnd::SetNamedTitleSparkle()
{
	if ( m_szTargetID != g_MyInfo.GetPublicID() )
		return;

	ioUserNamedTitle *pUserNamedTitle = g_MyInfo.GetUserNamedTitle();
	if ( !pUserNamedTitle )
		return;

	NamedTitleBtn *pNamedTitleBtn = dynamic_cast<NamedTitleBtn*>( FindChildWnd( ID_NAMED_TITLE ) );
	if ( !pNamedTitleBtn )
		return;

	if ( pUserNamedTitle->IsNewNamedTitle() )
	{
		if ( !pNamedTitleBtn->IsEffect() )
			pNamedTitleBtn->SetStartEffect();
	}
	else
	{
		if ( pNamedTitleBtn->IsEffect() )
			pNamedTitleBtn->SetEndEffect();
	}
}
//////////////////////////////////////////////////////////////////////////
MannerValuationSlotTooltip::MannerValuationSlotTooltip()
{
	m_pUserInfo = NULL;
	m_iListArray= -1;

	int i = 0;
	m_pInfoLine = NULL;
	m_pInfoLineHroz = NULL;
	m_pCostumeInfoLine = NULL;
	m_pCostumeInfoLineHroz = NULL;
	m_pInfoBlackLine = NULL;
	m_pInfoBlackLineHroz = NULL;
	for(i = 0;i < MAX_INFO_LODING;i++)
		m_pInfoLoading[i] = NULL;
	m_dwInfoLoadTime = 0;	
	m_iInfoLoadArray = 0;
	m_dwSubInfoSyncTime = 0;

	for(i = 0;i < MAX_ITEM_GROWTH;i++)
	{
		m_ItemGrowth[i].m_pIcon = NULL;
	}

	for(i = 0;i < MAX_CHAR_GROWTH;i++)
	{
		m_CharGrowth[i].m_pIcon = NULL;
	}

	m_pNoneMedal = NULL;
	m_pIconEffect= NULL;
	m_pMedalSlot = NULL;
	m_pNumText	 = NULL;
	m_pPlusText  = NULL;

	for( i=0; i<MAX_INVENTORY; i++ )
		m_pCostumeSlot[i] = NULL;
	m_pCostumeMark = NULL;

	m_pNoneTitleFrm = NULL;
	m_pPowerUpNAwakeTitleFrm = NULL;

	m_nPetCode		= 0;
	m_nPetLevel		= 0;
	m_bExtraInfo = false;

	for( i=0; i<MAX_ACC_INVENTORY; i++ )
		m_pAccessorySlot[i] = NULL;
}

MannerValuationSlotTooltip::~MannerValuationSlotTooltip()
{
	SAFEDELETE( m_pInfoLine );
	SAFEDELETE( m_pInfoLineHroz );
	SAFEDELETE( m_pInfoBlackLine );
	SAFEDELETE( m_pInfoBlackLineHroz );
	SAFEDELETE( m_pCostumeInfoLine );
	SAFEDELETE( m_pCostumeInfoLineHroz );

	int i = 0;
	for(i = 0;i < MAX_INFO_LODING;i++)
		SAFEDELETE( m_pInfoLoading[i] );

	for(i = 0;i < MAX_ITEM_GROWTH;i++)
		SAFEDELETE( m_ItemGrowth[i].m_pIcon );

	for(i = 0;i < MAX_CHAR_GROWTH;i++)
		SAFEDELETE( m_CharGrowth[i].m_pIcon );

	ioUIRenderImageMap::iterator iCreator = m_MedalIconList.begin();
	for(;iCreator != m_MedalIconList.end();++iCreator)
	{
		SAFEDELETE( iCreator->second );
	}
	m_MedalIconList.clear();

	iCreator = m_CostumeIconList.begin();
	for( ; iCreator!=m_CostumeIconList.end(); ++iCreator )
		SAFEDELETE( iCreator->second );
	m_CostumeIconList.clear();

	SAFEDELETE( m_pNoneMedal );
	SAFEDELETE( m_pIconEffect );
	SAFEDELETE( m_pMedalSlot );
	SAFEDELETE( m_pNumText );
	SAFEDELETE( m_pPlusText );

	for( i=0; i<MAX_INVENTORY; i++ )
		SAFEDELETE( m_pCostumeSlot[i] );
	SAFEDELETE( m_pCostumeMark );

	SAFEDELETE( m_pNoneTitleFrm );
	SAFEDELETE( m_pPowerUpNAwakeTitleFrm );

	for( i=0; i<MAX_ACC_INVENTORY; i++ )
		SAFEDELETE( m_pAccessorySlot[i] );
}

void MannerValuationSlotTooltip::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "InfoLine" )
	{
		SAFEDELETE( m_pInfoLine );
		m_pInfoLine = pImage;
	}
	else if( szType == "InfoLineHroz" )
	{
		SAFEDELETE( m_pInfoLineHroz );
		m_pInfoLineHroz = pImage;
	}
	else if( szType == "CharGrowth1" )
	{
		SAFEDELETE( m_CharGrowth[0].m_pIcon );
		m_CharGrowth[0].m_pIcon = pImage;
	}
	else if( szType == "CharGrowth2" )
	{
		SAFEDELETE( m_CharGrowth[1].m_pIcon );
		m_CharGrowth[1].m_pIcon = pImage;
	}
	else if( szType == "CharGrowth3" )
	{
		SAFEDELETE( m_CharGrowth[2].m_pIcon );
		m_CharGrowth[2].m_pIcon = pImage;
	}
	else if( szType == "CharGrowth4" )
	{
		SAFEDELETE( m_CharGrowth[3].m_pIcon );
		m_CharGrowth[3].m_pIcon = pImage;
	}
	else if( szType == "NoneMedal" )
	{
		SAFEDELETE( m_pNoneMedal );
		m_pNoneMedal = pImage;
	}
	else if( szType == "IconEffect" )
	{
		SAFEDELETE( m_pIconEffect );
		m_pIconEffect = pImage;
	}
	else if( szType == "MedalSlot" )
	{
		SAFEDELETE( m_pMedalSlot );
		m_pMedalSlot = pImage;
	}
	else if( szType == "NumText" )
	{
		SAFEDELETE( m_pNumText );
		m_pNumText = pImage;
	}
	else if( szType == "PlusText" )
	{
		SAFEDELETE( m_pPlusText );
		m_pPlusText = pImage;
	}
	else if( szType == "InfoLoading1" )
	{
		SAFEDELETE( m_pInfoLoading[0] );
		m_pInfoLoading[0] = pImage;
	}
	else if( szType == "InfoLoading2" )
	{
		SAFEDELETE( m_pInfoLoading[1] );
		m_pInfoLoading[1] = pImage;
	}
	else if( szType == "InfoLoading3" )
	{
		SAFEDELETE( m_pInfoLoading[2] );
		m_pInfoLoading[2] = pImage;
	}
	else if( szType == "InfoLoading4" )
	{
		SAFEDELETE( m_pInfoLoading[3] );
		m_pInfoLoading[3] = pImage;
	}
	else if( szType == "InfoLoading5" )
	{
		SAFEDELETE( m_pInfoLoading[4] );
		m_pInfoLoading[4] = pImage;
	}
	else if( szType == "InfoLoading6" )
	{
		SAFEDELETE( m_pInfoLoading[5] );
		m_pInfoLoading[5] = pImage;
	}
	else if( szType == "InfoLoading7" )
	{
		SAFEDELETE( m_pInfoLoading[6] );
		m_pInfoLoading[6] = pImage;
	}
	else if( szType == "InfoLoading8" )
	{
		SAFEDELETE( m_pInfoLoading[7] );
		m_pInfoLoading[7] = pImage;
	}
	else if( szType == "CostumeSlot1" )
	{
		SAFEDELETE( m_pCostumeSlot[0] );
		m_pCostumeSlot[0] = pImage;
	}
	else if( szType == "CostumeSlot2" )
	{
		SAFEDELETE( m_pCostumeSlot[1] );
		m_pCostumeSlot[1] = pImage;
	}
	else if( szType == "CostumeSlot3" )
	{
		SAFEDELETE( m_pCostumeSlot[2] );
		m_pCostumeSlot[2] = pImage;
	}
	else if( szType == "CostumeSlot4" )
	{
		SAFEDELETE( m_pCostumeSlot[3] );
		m_pCostumeSlot[3] = pImage;
	}
	else if( szType == "CostumeInfoLine" )
	{
		SAFEDELETE( m_pCostumeInfoLine );
		m_pCostumeInfoLine = pImage;
	}
	else if( szType == "CostumeInfoLineHroz" )
	{
		SAFEDELETE( m_pCostumeInfoLineHroz );
		m_pCostumeInfoLineHroz = pImage;
	}
	else if( szType == "costume_image" )
	{
		SAFEDELETE( m_pCostumeMark );
		m_pCostumeMark = pImage;
	}
	else if( szType == "InfoBlackLine" )
	{
		SAFEDELETE( m_pInfoBlackLine );
		m_pInfoBlackLine = pImage;
	}
	else if( szType == "InfoBlackLineHroz" )
	{
		SAFEDELETE( m_pInfoBlackLineHroz );
		m_pInfoBlackLineHroz = pImage;
	}
	else if( szType == "AccessorySlot1" )
	{
		SAFEDELETE( m_pAccessorySlot[0] );
		m_pAccessorySlot[0] = pImage;
		m_pAccessorySlot[0]->SetScale( 0.75f );
	}
	else if( szType == "AccessorySlot2" )
	{
		SAFEDELETE( m_pAccessorySlot[1] );
		m_pAccessorySlot[1] = pImage;
		m_pAccessorySlot[1]->SetScale( 0.75f );
	}
	else if( szType == "AccessorySlot3" )
	{
		SAFEDELETE( m_pAccessorySlot[2] );
		m_pAccessorySlot[2] = pImage;
		m_pAccessorySlot[2]->SetScale( 0.75f );
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}

void MannerValuationSlotTooltip::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "NoneTitle" )
	{
		SAFEDELETE( m_pNoneTitleFrm );
		m_pNoneTitleFrm = pFrame;
	}
	else if( szType == "PowerUpNAwakeTitle" )
	{
		SAFEDELETE( m_pPowerUpNAwakeTitleFrm );
		m_pPowerUpNAwakeTitleFrm = pFrame;
	}
	else
		ioWnd::AddRenderFrame( szType, pFrame );
}

void MannerValuationSlotTooltip::SetTooltipInfo( UserInfoNode *pUserInfo, int iListArray )
{
	m_pUserInfo = pUserInfo;
	m_iListArray= iListArray;
	
	if( m_pUserInfo == NULL || m_iListArray == -1 )
	{
		HideWnd();
		return;
	}

	UserInfoCharData &rkUserCharData = m_pUserInfo->GetUserCharData();
	CharInfoData &rkChar = rkUserCharData.GetSlotChar( m_iListArray );
	if( rkChar.m_CharInfo.m_class_type == 0 )
	{
		HideWnd();
		return;
	}

	m_bExtraInfo = false;
	int iExtra_Add_Y = 0;
	AwakeType eType = static_cast<AwakeType>( rkChar.m_iAwakeType );
	if ( rkChar.m_CharInfo.m_eCharPowerUpType != PUGT_NONE || eType != AWAKE_NONE )
	{
		m_bExtraInfo = true;
		iExtra_Add_Y = EXTRA_ADD_HEIGHT;
	}

	if( rkChar.m_bSubDataSync )
	{
		// 동기화 되었다.
		if( (int)rkChar.m_EquipMedalItem.size() > MAX_MEDAL / 2 )
			SetSize( GetWidth(), HEIGHT_BIG + iExtra_Add_Y );
		else
			SetSize( GetWidth(), HEIGHT_SMALL + iExtra_Add_Y );
	}
	else if( m_dwSubInfoSyncTime == 0 )
	{
		// 동기화 안되었음
		m_dwSubInfoSyncTime = FRAMEGETTIME();
		SetSize( GetWidth(), HEIGHT_SMALL );
	}

	//펫 툴팁 동기화
	m_nPetCode = pUserInfo->GetPetCode();
	m_nPetLevel = pUserInfo->GetPetLevel();
	
	ShowWnd();
	ProcessPosition();
}

void MannerValuationSlotTooltip::ProcessPosition()
{
	ioLSMouse *pMouse = g_App.GetMouse();
	if( pMouse )
	{
		int iOffSetXY = 8;
		int iXPos = pMouse->GetMousePos().x;
		int iYPos = pMouse->GetMousePos().y;

		if( iXPos + iOffSetXY + GetWidth() > Setting::Width() )
			iXPos -= ( GetWidth() + iOffSetXY );
		else 
			iXPos += iOffSetXY;

		if( iYPos + iOffSetXY + GetHeight() > Setting::Height() - 45 )
			iYPos -= ( GetHeight() + iOffSetXY );
		else 
			iYPos += iOffSetXY;

		if( GetParent() )
		{
			iXPos -= GetParent()->GetDerivedPosX();
			iYPos -= GetParent()->GetDerivedPosY();
		}

		SetWndPos( iXPos, iYPos );
		pMouse->SetToolTipCursor();
	}
}

void MannerValuationSlotTooltip::ProcessLoading()
{
	if( FRAMEGETTIME() - m_dwInfoLoadTime > 100 )
	{
		m_dwInfoLoadTime = FRAMEGETTIME();
		m_iInfoLoadArray++;
		if( m_iInfoLoadArray >= MAX_INFO_LODING )
			m_iInfoLoadArray = 0;
	}
}

void MannerValuationSlotTooltip::ProcessSubInfoSync()
{
	if( m_pUserInfo == NULL )
		return;

	if( m_dwSubInfoSyncTime == 0 )
		return;

	if( FRAMEGETTIME() - m_dwSubInfoSyncTime < SUB_INFO_SYNC_TIME )
		return;

    m_dwSubInfoSyncTime = 0;

	UserInfoCharData &rkUserCharData = m_pUserInfo->GetUserCharData();
	CharInfoData &rkChar = rkUserCharData.GetSlotChar( m_iListArray );
	if( rkChar.m_CharInfo.m_class_type != 0 )
	{
		rkUserCharData.SyncSubCharData( m_pUserInfo->GetUserID(), rkChar.m_CharInfo.m_class_type );
	}
}

void MannerValuationSlotTooltip::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	ProcessSubInfoSync();
	ProcessLoading();
	ProcessPosition();
}

ioUIRenderImage *MannerValuationSlotTooltip::GetMedalIcon( int iType )
{
	const ioMedalItemInfoManager::ItemInfo *pItemInfo = g_MedalItemMgr.GetItemInfo( iType );
	if( !pItemInfo )
		return m_pNoneMedal;

	ioUIRenderImageMap::iterator iter = m_MedalIconList.find( pItemInfo->m_sIcon );
	if( iter == m_MedalIconList.end() )
	{
		ioUIRenderImage *pIcon = g_UIImageSetMgr.CreateImageByFullName( pItemInfo->m_sIcon );
		if( pIcon )
		{
			m_MedalIconList.insert( ioUIRenderImageMap::value_type( pItemInfo->m_sIcon, pIcon )  );
			return pIcon;
		}
		else
			return m_pNoneMedal;
	}

	return iter->second;
}

ioUIRenderImage *MannerValuationSlotTooltip::GetMedalSubIcon( int iType )
{
	const ioMedalItemInfoManager::ItemInfo *pItemInfo = g_MedalItemMgr.GetItemInfo( iType );
	if( !pItemInfo )
		return NULL;

	ioUIRenderImageMap::iterator iter = m_MedalIconList.find( pItemInfo->m_sSubIcon );
	if( iter == m_MedalIconList.end() )
	{
		ioUIRenderImage *pIcon = g_UIImageSetMgr.CreateImageByFullName( pItemInfo->m_sSubIcon );
		if( pIcon )
		{
			m_MedalIconList.insert( ioUIRenderImageMap::value_type( pItemInfo->m_sSubIcon, pIcon )  );
			return pIcon;
		}
		else
			return NULL;
	}

	return iter->second;
}

void MannerValuationSlotTooltip::OnRender()
{
	ioWnd::OnRender();

	if( m_pUserInfo == NULL ) return;

	UserInfoCharData &rkUserCharData = m_pUserInfo->GetUserCharData();
	CharInfoData &rkChar = rkUserCharData.GetSlotChar( m_iListArray );
	if( rkChar.m_CharInfo.m_class_type == 0 )
		return;

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	char szClassInfo[MAX_PATH] = "";
	char szClassName[MAX_PATH] = "";

	AwakeType eType = static_cast<AwakeType>( rkChar.m_iAwakeType );

	bool bAwake = false;
	bool bPowerUp = false;
	if ( eType != AWAKE_NONE )
		bAwake = true;

	if ( rkChar.m_CharInfo.m_eCharPowerUpType != PUGT_NONE )
		bPowerUp = true;

	if( !m_bExtraInfo )
	{
		if( m_pNoneTitleFrm)
			m_pNoneTitleFrm->Render( iXPos, iYPos );
	}
	else
	{
		if( m_pPowerUpNAwakeTitleFrm)
			m_pPowerUpNAwakeTitleFrm->Render( iXPos, iYPos );
	}
	
	//유저 ID출력
	g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 12, 66, 111 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
	g_FontMgr.PrintText( iXPos + 87, iYPos + 10, FONT_SIZE_13, m_pUserInfo->GetUserID().c_str() );
	
	//캐릭터 레벨 이름 출력
	StringCbPrintf( szClassInfo, sizeof( szClassInfo ), "LV%d %s", rkChar.m_iClassLevel, g_MyInfo.GetClassName( rkChar.m_CharInfo.m_class_type ));
	g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
	g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_3X );
	g_FontMgr.PrintText( iXPos + 87, iYPos + 26, FONT_SIZE_11, szClassInfo );

	if ( bAwake && bPowerUp )
	{
		std::string szAwakeName = g_AwakeMgr.GetAwakeName( eType );
		ioHashString szPowerUpName;
		szPowerUpName = g_PowerUpManager.GetCharGradeTitle( rkChar.m_CharInfo.m_class_type, rkChar.m_CharInfo.m_eCharPowerUpType );
		memset( szClassInfo, 0,  sizeof( szClassInfo ) );

		SafeSprintf( szClassInfo, sizeof( szClassInfo ), STR(2), szPowerUpName.c_str(), szAwakeName.c_str() );
		g_FontMgr.SetTextColor( 0xFFFF7F7F );
		g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_3X );
		g_FontMgr.PrintText( iXPos + 87, iYPos + 41, FONT_SIZE_11, szClassInfo );
	}
	else if ( bAwake || bPowerUp )
	{
		memset( szClassInfo, 0,  sizeof( szClassInfo ) );
		if ( bAwake )
		{
			std::string szAwakeName = g_AwakeMgr.GetAwakeName( eType );
			StringCbPrintf( szClassInfo, sizeof( szClassInfo ), "(%s)", szAwakeName.c_str() );
		}
		else if( bPowerUp )
		{
			ioHashString szPowerUpName;
			szPowerUpName = g_PowerUpManager.GetCharGradeTitle( rkChar.m_CharInfo.m_class_type, rkChar.m_CharInfo.m_eCharPowerUpType );
			SafeSprintf( szClassInfo, sizeof( szClassInfo ), STR(3), szPowerUpName.c_str() );
		}
		g_FontMgr.SetTextColor( 0xFFFF7F7F );
		g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_3X );
		g_FontMgr.PrintText( iXPos + 87, iYPos + 41, FONT_SIZE_11, szClassInfo );
	}
	
	if( rkChar.m_bSubDataSync )
	{
		// 성장 + 장비 + 메달 정보 표시
		OnRenderTooltip( iXPos, iYPos );
	}
	else
	{
		if( COMPARE( m_iInfoLoadArray, 0, MAX_INFO_LODING ) )
		{
			g_FontMgr.SetTextStyle( TS_NORMAL );
			g_FontMgr.SetAlignType( TAT_CENTER );
			g_FontMgr.SetBkColor( 0, 0, 0 );
			g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
			g_FontMgr.PrintText( iXPos + 87, iYPos + 96, FONT_SIZE_11, STR(1) );

			if( m_pInfoLoading[m_iInfoLoadArray] )
				m_pInfoLoading[m_iInfoLoadArray]->Render( iXPos + 63, iYPos + 113 );
		}
	}	
}

void MannerValuationSlotTooltip::OnRenderTooltip( int iXPos, int iYPos )
{
	int iExtra_Add_Y = 0;
	if ( m_bExtraInfo )
		iExtra_Add_Y = EXTRA_ADD_HEIGHT;

	if( m_pInfoLine && m_pInfoLineHroz && m_pInfoBlackLine && m_pInfoBlackLineHroz )
	{
		m_pInfoLine->Render( iXPos + 7, iYPos + 57 + iExtra_Add_Y );
		m_pInfoLineHroz->Render( iXPos + 112, iYPos + 57 + iExtra_Add_Y );

		// UI코스튬 관련 (코스튬 UI 설정)
		if ( m_pCostumeInfoLine && m_pCostumeInfoLineHroz )
		{
			m_pCostumeInfoLine->Render( iXPos + 7, iYPos + 159 + iExtra_Add_Y );
			m_pCostumeInfoLineHroz->Render( iXPos + 117, iYPos + 159 + iExtra_Add_Y );
		}

		m_pInfoBlackLine->Render( iXPos + 7, iYPos + 230 + iExtra_Add_Y );
		m_pInfoBlackLineHroz->Render( iXPos + 122, iYPos + 230 + iExtra_Add_Y );

		m_pInfoLine->Render( iXPos + 7, iYPos + 300 + iExtra_Add_Y );
		m_pInfoLineHroz->Render( iXPos + 112, iYPos + 300 + iExtra_Add_Y );
	}

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( iXPos + 87, iYPos + 52 + iExtra_Add_Y, FONT_SIZE_11, STR(1) );
	
	// UI코스튬 관련 (코스튬 UI 설정) + 액세서리
	g_FontMgr.PrintText( iXPos + 88, iYPos + 153 + iExtra_Add_Y, FONT_SIZE_11, "장착코스튬" );
	g_FontMgr.PrintText( iXPos + 88, iYPos + 224 + iExtra_Add_Y, FONT_SIZE_11, "장착액세서리" );
	g_FontMgr.PrintText( iXPos + 87, iYPos + 297 + iExtra_Add_Y, FONT_SIZE_11, STR(2) );

	UserInfoCharData &rkUserCharData = m_pUserInfo->GetUserCharData();
	CharInfoData &rkChar = rkUserCharData.GetSlotChar( m_iListArray );

	OnRenderItemGrowth( rkChar, iXPos + 27, iYPos + 88 + iExtra_Add_Y );
	OnRenderCharGrowth( rkChar, iXPos + 27, iYPos + 127 + iExtra_Add_Y );
	
	// UI코스튬 관련 (캐릭터 툴팁)
	OnRenderCostume( rkChar, iXPos + 33, iYPos + 196 + iExtra_Add_Y );
	OnRenderAccessory( rkChar, iXPos + 33, iYPos + 267 + iExtra_Add_Y );
	OnRenderMedal( rkChar, iXPos + 33, iYPos + 338 + iExtra_Add_Y );
}

void MannerValuationSlotTooltip::OnRenderItemGrowth( CharInfoData &rkChar, int iXPos, int iYPos )
{
	for( int i = 0; i < MAX_INVENTORY; i++ )
	{
		ioHashString szLargeIcon;
		DWORD dwItemCode = rkChar.m_EquipItemInfo[i].m_item_code;
		if( dwItemCode == 0 )
		{
			dwItemCode = (DEFAULT_BASIC_ITEM_CODE * i) + rkChar.m_CharInfo.m_class_type;
		}

		const ioItem *pConstItem = g_ItemMaker.GetItemConst( dwItemCode, __FUNCTION__ );
		if( pConstItem )
		{
			const ioSkill *pSkill = g_SkillMaker.GetSkillConst( pConstItem->GetSkillName() );
			if( pSkill )
			{
				szLargeIcon = pSkill->GetIconName();
			}
		}

		if( i >= MAX_ITEM_GROWTH ) continue;
		if( m_ItemGrowth[i].m_szFullName == szLargeIcon ) continue;

		SAFEDELETE( m_ItemGrowth[i].m_pIcon );
		m_ItemGrowth[i].m_szFullName = szLargeIcon;
		m_ItemGrowth[i].m_pIcon      = g_UIImageSetMgr.CreateImageByFullName( szLargeIcon );
	}

	for( int i = 0; i < MAX_ITEM_GROWTH; i++ )
	{
		if( m_ItemGrowth[i].m_pIcon == NULL ) continue;

		int iItemGrowth = rkChar.GetItemGrowth( i );
		int iMedalSize = rkChar.m_EquipMedalItem.size();
		for( int k = 0; k < iMedalSize; k++ )
		{
			if( rkChar.m_EquipMedalItem[k].m_iCustomIndex == 0 )
				iItemGrowth += g_MedalItemMgr.GetMedalItemGrowth( rkChar.m_EquipMedalItem[k].m_iItemType, false, i );
			else
				iItemGrowth += rkChar.m_EquipMedalItem[k].m_iStat[i + MAX_CHAR_GROWTH];
		}
		//펫 성장
		if ( m_nPetCode > 0 )
			iItemGrowth += g_PetInfoMgr.GetTotalStat( m_nPetCode, i+MAX_ITEM_GROWTH, m_nPetLevel );

		iItemGrowth += g_AwakeMgr.GetAwakeItemGrowth( static_cast<AwakeType>(rkChar.m_iAwakeType), i );

		if( g_RSoldierInfoManager.IsRSoldierType( rkChar.m_CharInfo ) )
		{
			if( g_MyInfo.GetPublicID() == m_pUserInfo->GetUserID() )
				iItemGrowth += g_RSoldierInfoManager.GetSoldierAddStat( i+MAX_ITEM_GROWTH, g_MyInfo.GetRSoldierPossessionCount() );
			else
				iItemGrowth += g_RSoldierInfoManager.GetSoldierAddStat( i+MAX_ITEM_GROWTH, m_pUserInfo->GetRSoldierPossessionCount() );
		}
		
		if( g_RSoldierInfoManager.IsSoldierSet( rkChar.m_CharInfo ) )
		{
			if( g_MyInfo.GetPublicID() == m_pUserInfo->GetUserID() )
				iItemGrowth += g_RSoldierInfoManager.GetSoldierSetAddStat( i+MAX_ITEM_GROWTH, rkChar.m_CharInfo.m_class_type, g_MyInfo.GetSoldierSetCnt( g_RSoldierInfoManager.GetSoldierSetNum( rkChar.m_CharInfo.m_class_type ) ) );
			else
				iItemGrowth += g_RSoldierInfoManager.GetSoldierSetAddStat( i+MAX_ITEM_GROWTH, rkChar.m_CharInfo.m_class_type, m_pUserInfo->GetSoldierSetCnt( g_RSoldierInfoManager.GetSoldierSetNum( rkChar.m_CharInfo.m_class_type ) ) );
		}

		if( iItemGrowth <= 0 )
		{
			if( m_pIconEffect )
			{
				m_pIconEffect->SetScale( 0.58f );
				m_pIconEffect->Render( iXPos + i * 40, iYPos, UI_RENDER_MULTIPLY, TFO_BILINEAR );
			}

			m_ItemGrowth[i].m_pIcon->SetScale( 0.58f );
			m_ItemGrowth[i].m_pIcon->Render( iXPos + i * 40, iYPos, UI_RENDER_GRAY, TFO_BILINEAR );
		}
		else
		{
			if( m_pIconEffect )
			{
				m_pIconEffect->SetScale( 0.50f );
				m_pIconEffect->Render( iXPos + i * 40, iYPos, UI_RENDER_MULTIPLY, TFO_BILINEAR );
			}

			m_ItemGrowth[i].m_pIcon->SetScale( 0.50f );
			m_ItemGrowth[i].m_pIcon->Render( iXPos + i * 40, iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );

			OnRenderGrowthPoint( iXPos + i * 40, iYPos, iItemGrowth );
		}
	}
}

void MannerValuationSlotTooltip::OnRenderCharGrowth( CharInfoData &rkChar, int iXPos, int iYPos )
{
	for( int i = 0; i < MAX_CHAR_GROWTH; i++ )
	{
		if( m_CharGrowth[i].m_pIcon == NULL ) continue;

		int iCharGrowth = rkChar.GetCharGrowth( i );
		int iMedalSize = rkChar.m_EquipMedalItem.size();
		for(int k = 0;k < iMedalSize;k++)
		{
			if( rkChar.m_EquipMedalItem[k].m_iCustomIndex == 0 )
				iCharGrowth  += g_MedalItemMgr.GetMedalItemGrowth( rkChar.m_EquipMedalItem[k].m_iItemType, true, i );
			else
				iCharGrowth += rkChar.m_EquipMedalItem[k].m_iStat[i];
		}			

		//펫 성장
		if ( m_nPetCode > 0 )
			iCharGrowth += g_PetInfoMgr.GetTotalStat( m_nPetCode, i, m_nPetLevel );

		iCharGrowth += g_AwakeMgr.GetAwakeCharGrowth( static_cast<AwakeType>(rkChar.m_iAwakeType), i );

		if( g_RSoldierInfoManager.IsRSoldierType( rkChar.m_CharInfo ) )
		{
			if( g_MyInfo.GetPublicID() == m_pUserInfo->GetUserID() )
				iCharGrowth += g_RSoldierInfoManager.GetSoldierAddStat( i, g_MyInfo.GetRSoldierPossessionCount() );
			else
				iCharGrowth += g_RSoldierInfoManager.GetSoldierAddStat( i, m_pUserInfo->GetRSoldierPossessionCount() );
		}
		
		if( g_RSoldierInfoManager.IsSoldierSet( rkChar.m_CharInfo ) )
		{
			if( g_MyInfo.GetPublicID() == m_pUserInfo->GetUserID() )
				iCharGrowth += g_RSoldierInfoManager.GetSoldierSetAddStat( i, rkChar.m_CharInfo.m_class_type, g_MyInfo.GetSoldierSetCnt( g_RSoldierInfoManager.GetSoldierSetNum( rkChar.m_CharInfo.m_class_type ) ) );
			else
				iCharGrowth += g_RSoldierInfoManager.GetSoldierSetAddStat( i, rkChar.m_CharInfo.m_class_type, m_pUserInfo->GetSoldierSetCnt( g_RSoldierInfoManager.GetSoldierSetNum( rkChar.m_CharInfo.m_class_type ) ) );
		}

		if( iCharGrowth <= 0 )
		{
			if( m_pIconEffect )
			{
				m_pIconEffect->SetScale( 0.58f );
				m_pIconEffect->Render( iXPos + i * 40, iYPos, UI_RENDER_MULTIPLY, TFO_BILINEAR );
			}

			m_CharGrowth[i].m_pIcon->SetScale( 0.58f );
			m_CharGrowth[i].m_pIcon->Render( iXPos + i * 40, iYPos, UI_RENDER_GRAY, TFO_BILINEAR );
		}
		else
		{
			if( m_pIconEffect )
			{
				m_pIconEffect->SetScale( 0.50f );
				m_pIconEffect->Render( iXPos + i * 40, iYPos, UI_RENDER_MULTIPLY, TFO_BILINEAR );
			}

			m_CharGrowth[i].m_pIcon->SetScale( 0.50f );
			m_CharGrowth[i].m_pIcon->Render( iXPos + i * 40, iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );

			OnRenderGrowthPoint( iXPos + i * 40, iYPos, iCharGrowth );
		}
	}
}

void MannerValuationSlotTooltip::OnRenderMedal( CharInfoData &rkChar, int iXPos, int iYPos )
{
	int iNewXPos = iXPos;
	int iNewYPos = iYPos;
	int iMedalSize = rkChar.m_EquipMedalItem.size();
	int iSlotNum   = g_MedalItemMgr.GetSlotNum( rkChar.m_iClassLevel );
	if( iMedalSize <= MAX_MEDAL / 2 )
		iSlotNum = min( iSlotNum, MAX_MEDAL / 2 );

	for(int i = 0;i < MAX_MEDAL;i++)
	{
		if( i < iMedalSize )
		{
			ioUIRenderImage *pMedalIcon = GetMedalIcon( rkChar.m_EquipMedalItem[i].m_iItemType );
			ioUIRenderImage *pMedalSubIcon = GetMedalSubIcon( rkChar.m_EquipMedalItem[i].m_iItemType );
			if( pMedalIcon )
			{
				if( m_pIconEffect )
				{
					m_pIconEffect->SetScale( 0.65f );
					m_pIconEffect->Render( iNewXPos, iNewYPos, UI_RENDER_MULTIPLY, TFO_BILINEAR );
				}

				pMedalIcon->SetScale( 0.65f );
				pMedalIcon->Render( iNewXPos, iNewYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
				if( pMedalSubIcon )
				{
					pMedalSubIcon->SetScale( 0.65f );
					pMedalSubIcon->Render( iNewXPos, iNewYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
				}
			}
		}
		else if( i < iSlotNum )
		{
			if( m_pMedalSlot )
			{
				m_pMedalSlot->SetScale( 0.75f );
				m_pMedalSlot->Render( iNewXPos, iNewYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
			}
		}
		else 
		{
			if( i < MAX_MEDAL / 2 || iMedalSize > MAX_MEDAL / 2 )
			{
				if( m_pNoneMedal )
				{
					m_pNoneMedal->SetScale( 0.68f );
					m_pNoneMedal->Render( iNewXPos, iNewYPos, UI_RENDER_MULTIPLY, TFO_BILINEAR );
				}
			}
		}		

		iNewXPos += 54;
		if( i + 1 == MAX_MEDAL / 2 ) 
		{
			iNewXPos = iXPos;
			iNewYPos = iYPos + 51;
		}
	}

	if( GetHeight() == HEIGHT_SMALL )
	{
		if( iMedalSize > MAX_MEDAL / 2 )
		{
			SetSize( GetWidth(), HEIGHT_BIG );
		}
	}
	else 
	{
		if( iMedalSize <= MAX_MEDAL / 2 )
		{
			SetSize( GetWidth(), HEIGHT_SMALL );
		}
	}
}

void MannerValuationSlotTooltip::OnRenderGrowthPoint( int iXPos, int iYPos, int iGrowthPoint )
{
	if( iGrowthPoint <= 0 ) return;
	if( !m_pNumText ) return;
	if( !m_pPlusText ) return;

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

	int iCurYPos = iNewYPos+(REINFORCE_Y * 0.58f);

	int iTotalSize = 0;
	if( iGrowthPoint < 10 )
		iTotalSize = PLUS_SIZE + NUM_SIZE - NUM_GAP;
	else if( iGrowthPoint < 100 )
		iTotalSize = PLUS_SIZE + NUM_SIZE*2 - NUM_GAP*2;
	else
		iTotalSize = PLUS_SIZE + NUM_SIZE*3 - NUM_GAP*3;

	iCurPos = iDefaultPos - (iTotalSize / 2);

	m_pPlusText->Render( iCurPos, iCurYPos );

	iCurPos += (PLUS_SIZE-NUM_GAP);

	m_pNumText->RenderNum( iCurPos, iCurYPos, iGrowthPoint, -NUM_GAP );
}

ioUIRenderImage *MannerValuationSlotTooltip::GetCostumeIcon( int nCostumeCode )
{
	ioHashString szIcon = g_CostumeInfoMgr.GetCostumeIcon( nCostumeCode );
	if( szIcon.IsEmpty() )
		return NULL;

	ioUIRenderImageMap::iterator iter = m_CostumeIconList.find( szIcon );
	if( iter == m_CostumeIconList.end() )
	{
		ioUIRenderImage *pIcon = g_UIImageSetMgr.CreateImageByFullName( szIcon );
		if( pIcon )
		{
			m_CostumeIconList.insert( ioUIRenderImageMap::value_type( szIcon, pIcon )  );
			return pIcon;
		}
		else
			return NULL;
	}

	return iter->second;
}

void MannerValuationSlotTooltip::OnRenderCostume( CharInfoData &rkChar, int nXPos, int nYPos )
{
	// UI코스튬 관련 ( 캐릭터 코스튬 슬롯 - 1부터 시작:무기 제외)
	for ( int i=1; i<MAX_INVENTORY; i++ )
	{
		ioUIRenderImage *pCostumeIcon = GetCostumeIcon( rkChar.m_CharInfo.m_EquipCostume[i].m_CostumeCode );
		if( pCostumeIcon )
		{
			if( m_pIconEffect )
			{
				m_pIconEffect->SetScale( 0.65f );
				m_pIconEffect->Render( nXPos + (i-1) * 54, nYPos, UI_RENDER_MULTIPLY, TFO_BILINEAR );
			}

			g_UIImageRenderMgr.RenderGradeIconBack( g_CostumeInfoMgr.GetGradeType( rkChar.m_CharInfo.m_EquipCostume[i].m_CostumeCode ), nXPos + (i-1) * 54, nYPos, 0.65f );

			pCostumeIcon->SetScale( 0.65f );
			pCostumeIcon->Render( nXPos + (i-1) * 54, nYPos, UI_RENDER_NORMAL, TFO_BILINEAR );

			// UI코스튬 관련 (코스튬 마크)
			if( m_pCostumeMark )
			{
				m_pCostumeMark->SetScale( 0.65f );
				m_pCostumeMark->Render( nXPos + (i-1) * 54, nYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
			}
		}
		else
		{
			if( m_pCostumeSlot[i] )
			{
				m_pCostumeSlot[i]->SetScale( 0.75f );
				m_pCostumeSlot[i]->Render( nXPos + (i-1) * 54, nYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
			}
		}
	}
}

ioUIRenderImage * MannerValuationSlotTooltip::GetAccessoryIcon( int iAccessoryCode )
{
	ioHashString szIcon = g_ItemMaker.GetItemLargeIcon( iAccessoryCode, 0 );
	if( szIcon.IsEmpty() )
		return NULL;

	ioUIRenderImageMap::iterator iter = m_AccessoryIconList.find( szIcon );
	if( iter == m_AccessoryIconList.end() )
	{
		ioUIRenderImage *pIcon = g_UIImageSetMgr.CreateImageByFullName( szIcon );
		if( pIcon )
		{
			m_AccessoryIconList.insert( ioUIRenderImageMap::value_type( szIcon, pIcon )  );
			return pIcon;
		}
		else
			return NULL;
	}

	return iter->second;
}

void MannerValuationSlotTooltip::OnRenderAccessory( CharInfoData &rkChar, int iXPos, int iYPos )
{
	for ( int i=0; i<MAX_ACC_INVENTORY; i++ )
	{
		ioUIRenderImage *pAccessoryIcon = GetAccessoryIcon( rkChar.m_CharInfo.m_EquipAccessory[i].m_iAccessoryCode );
		if( pAccessoryIcon )
		{
			if( m_pIconEffect )
			{
				m_pIconEffect->SetScale( 0.65f );
				m_pIconEffect->Render( iXPos + i * 54, iYPos, UI_RENDER_MULTIPLY, TFO_BILINEAR );
			}
			pAccessoryIcon->SetScale( 0.65f );
			pAccessoryIcon->Render( iXPos + i * 54, iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
		}
		else
		{
			if( m_pAccessorySlot[i] )
				m_pAccessorySlot[i]->Render( iXPos + i * 54, iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
		}
	}
}
