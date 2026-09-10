

#include "stdafx.h"

#include "ioChatMacro.h"
#include "ioLadderTeamMgr.h"
#include "ioShuffleRoomMgr.h"
#include "ioRankBattleManager.h"

#include "GUI/ChatMacro.h"

template<> ioChatMacro* Singleton< ioChatMacro >::ms_Singleton = 0;

ioChatMacro::ioChatMacro()
{
	m_iCurChatCnt = 0;
	m_dwMacroChatCheckTime = 0;
	m_dwDisableTime = 0;
	m_ChatIconList.clear();

	m_iCurNormalChatCnt = 0;
	m_dwNormalChatCheckTime = 0;

	m_iCurSoundCnt = 0;
	m_dwSoundCheckTime = 0;
}

ioChatMacro::~ioChatMacro()
{
	m_ChatIconList.clear();
}

void ioChatMacro::LoadChatMacroInfo()
{
	ioINILoader_e kLoader( "config/sp2_chat_macro.ini" );

	kLoader.SetTitle_e( "common" );

	m_iLimiteChatCnt = kLoader.LoadInt_e( "macro_chat_limite_count", 3 );
	m_dwMacroChatGapTime = kLoader.LoadInt_e( "macrochat_gap_time", 0 );
	m_dwDisableGapTime = kLoader.LoadInt_e( "macrochat_disable_gap_time", 2000 );

	m_iNormalChatLimiteCount = kLoader.LoadInt_e( "chat_limite_count", 1 );
	m_dwNormalChatGapTime = kLoader.LoadInt_e( "chat_gap_time", 0 );

	m_iSoundLimiteCount = kLoader.LoadInt_e( "sound_limite_count", 1 );
	m_dwSoundGapTime = kLoader.LoadInt_e( "sound_gap_time", 0 );

	// ChatEmoticon
	kLoader.SetTitle_e( "chat_icon" );

	char szBuf[MAX_PATH];
	kLoader.LoadString_e( "default_chat_icon", "", szBuf, MAX_PATH );
	m_DefaultChatIcon = szBuf;
	kLoader.LoadString_e( "default_chat_sound", "", szBuf, MAX_PATH );
	m_DefaultChatSound = szBuf;

	m_ChatIconList.clear();
	
	char szKey[MAX_PATH], szChat[MAX_PATH];
	int iMaxIconCnt = kLoader.LoadInt_e( "max_icon_cnt", 0 );
	for(int i=0; i < iMaxIconCnt; i++ )
	{
		ChatIconInfo eChatIconInfo;
		wsprintf_e( szKey, "chat_icon%d", i+1 );
		kLoader.SetTitle( szKey );

		int iChatCnt = kLoader.LoadInt_e( "chat_cnt", 0 );
		for( int j=0; j < iChatCnt; j++ )
		{
			wsprintf_e( szChat, "chat_string%d", j+1 );
			kLoader.LoadString( szChat, "", szBuf, MAX_PATH );
			eChatIconInfo.m_ChatIconStringList.push_back( szBuf );
		}

		int iChatSoundCnt = kLoader.LoadInt_e( "chat_sound_cnt", 0 );
		for( j=0; j < iChatSoundCnt; j++ )
		{
			wsprintf_e( szChat, "chat_icon_sound%d", j+1 );
			kLoader.LoadString( szChat, "", szBuf, MAX_PATH );
			eChatIconInfo.m_ChatIconSoundList.push_back( szBuf );
		}

		kLoader.LoadString_e( "chat_icon_name", "", szBuf, MAX_PATH );
		eChatIconInfo.m_ChatIconName = szBuf;

		m_ChatIconList.push_back( eChatIconInfo );
	}
}

bool ioChatMacro::IsCheckStealthMode( ioPlayStage *pPlayStage )
{
	if( pPlayStage )
	{
		if( pPlayStage->GetModeType() == MT_NONE			|| 
			pPlayStage->GetModeType() == MT_MYROOM			|| 
			pPlayStage->GetModeType() == MT_TRAINING		|| 
			pPlayStage->GetModeType() == MT_HEADQUARTERS	||
			pPlayStage->GetModeType() == MT_SUCCESSION		||
			pPlayStage->GetModeType() == MT_HOUSE )
		{
			return false;
		}

		if( g_App.IsMeDeveloper() )
		{
			if( !g_BattleRoomMgr.IsBattleRoom() && !g_LadderTeamMgr.IsLadderTeam() && 
				( g_RankBattleMgr.IsRankBattlePlaying() || g_ShuffleRoomMgr.IsShuffleRoom() ) )
			{
				return true;
			}
		}
	}
	return false;
}

void ioChatMacro::SetChatMacro(const ioBaseChar *pOwnerChar,
							   const CKeyInfo::MecroTextInfo *pInfo,
							   ModeType eModeType /*= MT_NONE*/,
							   const int iRoomIndex/*=-1*/,
							   ioPlayStage *pPlayStage /*= NULL */ )
{
	if( !pOwnerChar )
		return;

	if( !pInfo )
		return;

	if( pInfo->m_szText.IsEmpty() )
		return;

	IntVec MarroItemList;
	g_ChatMacroMgr.GetEtcItemMacro( (char*)pInfo->m_szText.c_str(), MarroItemList );
	if( MarroItemList.size() == 0 && !g_ChatMacroMgr.CheckCreateStructMacro( (char*)pInfo->m_szText.c_str() ) )
	{
		if( !CheckNormalChatEnable() )
			return;

		if( !CheckMacroChatEnable() )
			return;
	}

	DWORD dwCustomSoundIndex = 0;
	DWORD dwCustomSoundCRC   = 0;
	DWORD dwCustomSoundSize  = 0;
	g_CustomSoundMgr.GetCustomSoundInfo( pInfo->m_szText, dwCustomSoundIndex, dwCustomSoundCRC, dwCustomSoundSize );

	bool bCanUseEtcItem = false;
	ioUserEtcItem *pUserEtcItem = g_MyInfo.GetUserEtcItem();
	if( pUserEtcItem && pUserEtcItem->IsCanUse( ioEtcItem::EIT_ETC_CUSTOM_SOUND ) )
		bCanUseEtcItem = true;

	bool bCustomSound = false;
	if( dwCustomSoundIndex != 0 && g_CustomSoundMgr.IsCanTimePlayMy() && bCanUseEtcItem )
	{
		bCustomSound = true;
		g_ChatMgr.SetNoChatSound( true );
	}

	int iMacroType = g_ChatMacroMgr.MacroProcess( pInfo->m_szText.c_str(), true );
	if( iMacroType != ChatMacro::NONE_MACRO ) 
		return;	

	if( g_MyInfo.GetBlockType() == BKT_LIMITATION )
	{
		g_ChatMgr.SetSystemMsg( STR(1));
		return;
	}

	if( !pPlayStage )
		return;

	if( IsCheckStealthMode( pPlayStage ) )
		return;

	bool bChatMacro = false;
	TeamType eTeam = pOwnerChar->GetTeam();
	ioHashString szID = g_MyInfo.GetPublicID();

	char szChatBuf[MAX_PATH] = "";
	ioHashString szOwnerChat = pInfo->m_szText;
	if( !Setting::IsSlangFilterOther() )
	{
		StringCbCopy( szChatBuf, MAX_PATH, g_SlangUtil.ConvertStringWithPreLine( pInfo->m_szText.c_str() ) );
		szOwnerChat = szChatBuf;
	}

	if( pInfo->m_bAll )
	{
		bChatMacro = true;
		pPlayStage->SetChatIcon( pOwnerChar->GetCharName(), pInfo->m_szText, g_ChatMgr.IsNoChatSound(), bCustomSound);
		switch( pPlayStage->GetModeType() )
		{
		case MT_MYROOM:    // 서버 채널 채팅
			{
				if( ioMyRoomMode::GetMyRoomType() == MRT_LOBBY )
					SetServerLobbyChatMsg( TEAM_BLUE, szID.c_str(), szOwnerChat.c_str(), g_ChatMgr.IsNoChatSound() );
				else
					SetChatMsg( eTeam, szID.c_str(), szOwnerChat.c_str(), g_ChatMgr.IsNoChatSound(), bCustomSound );
			}
			break;
		default:           // 전체 채팅
			SetChatMsg( eTeam, szID.c_str(), szOwnerChat.c_str(), g_ChatMgr.IsNoChatSound(), bCustomSound );
			break;
		}
	}
	else
	{
		switch( pPlayStage->GetModeType() )
		{
		case MT_MYROOM:			// 서버 채널 채팅 팀 X			
		case MT_TRAINING:		// 광장 채팅 팀 X
		case MT_SURVIVAL:		// 데스매치 팀 X
		case MT_FIGHT_CLUB:     // 
		case MT_SHUFFLE_BONUS:
		case MT_HEADQUARTERS:   
		case MT_HOUSE:
			g_ChatMgr.SetSystemMsg( STR(2) );
			break;
		default:           
			if( g_BattleRoomMgr.IsObserverUser( g_MyInfo.GetPublicID() ) )        // 옵저버 팀 X
				g_ChatMgr.SetSystemMsg( STR(3) );
			else 
			{

				pPlayStage->SetChatIcon( pOwnerChar->GetCharName(), pInfo->m_szText, g_ChatMgr.IsNoChatSound(), bCustomSound);
				SetTeamChatMsg( eTeam, szID.c_str(), szOwnerChat.c_str(), g_ChatMgr.IsNoChatSound(), bCustomSound );
				bChatMacro = true;
			}
			break;
		}
	}

	if( strlen( pInfo->m_szText.c_str() ) > 0 )
	{
		if( pInfo->m_bAll )
		{
			switch( pPlayStage->GetModeType() )
			{
			case MT_MYROOM:    // 서버 채널 채팅
				{
					if( ioMyRoomMode::GetMyRoomType() == MRT_LOBBY )
						SendServerLobbyChatMsg( szID.c_str(), szOwnerChat.c_str() );
					else
						SendChatMsg( eTeam, szID.c_str(), szOwnerChat.c_str(), eModeType, iRoomIndex, bCustomSound );
				}
				break;
			case MT_TRAINING:  // 광장 채팅
				SendChatMsg( eTeam, szID.c_str(), szOwnerChat.c_str(), eModeType, iRoomIndex, bCustomSound );
				break;
			default:           // 전체 채팅
				SendChatMsg( eTeam, szID.c_str(), szOwnerChat.c_str(), eModeType, iRoomIndex, bCustomSound );
				break;
			}
		}
		else
		{
			// 팀챗이 가능하다.
			if( bChatMacro )
			{
				SendTeamChatMsg(  eTeam, szID.c_str(), pInfo->m_szText.c_str(), eModeType, iRoomIndex, bCustomSound );
			}
		}
	}

	if( bChatMacro )
	{
		if( bCanUseEtcItem && g_CustomSoundMgr.IsCanTimePlayMy() )
		{
			g_CustomSoundMgr.SendCustomSoundInfo( dwCustomSoundIndex, dwCustomSoundCRC, dwCustomSoundSize, true, eTeam );
			g_CustomSoundMgr.PlayMy( dwCustomSoundIndex, dwCustomSoundCRC );
			if( pUserEtcItem )
				pUserEtcItem->SendUse( ioEtcItem::EIT_ETC_CUSTOM_SOUND );
		}
	}	
}

const ioHashString& ioChatMacro::GetChatIcon( ioHashString szChat )
{
	if( szChat.IsEmpty() || m_ChatIconList.empty() )
		return m_DefaultChatIcon;

	char string[MAX_PATH];
	memset( string, 0, MAX_PATH );

	char szFilterChar[]={'\t',' ','_','-','.',':','^','/','\\',29,28,NULL}; /* 29:ctrl+] , 28: ctrl+\ */
    FilterChar( szChat.c_str(), string, szFilterChar);

	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal )
		pLocal->GetChangedString( string );

	int iMaxIconCnt = m_ChatIconList.size();
	for( int i=0; i < iMaxIconCnt; i++ )
	{
		const ChatIconInfo &rkChatInfo = m_ChatIconList[i];

		int iMaxStringCount = rkChatInfo.m_ChatIconStringList.size();
		for( int j=0; j < iMaxStringCount; j++ )
		{
			char *pResult = strstr( string, rkChatInfo.m_ChatIconStringList[j].c_str() );
			if( pResult )
			{
				int iMaxSoundCount = rkChatInfo.m_ChatIconSoundList.size();

				if( !COMPARE( j, 0, iMaxSoundCount ) )
				{
					g_ChatMgr.SetCurChatSound( m_DefaultChatSound );
				}
				else
				{
					if( !rkChatInfo.m_ChatIconSoundList[j].IsEmpty() )
						g_ChatMgr.SetCurChatSound( rkChatInfo.m_ChatIconSoundList[j] );
					else
						g_ChatMgr.SetCurChatSound( m_DefaultChatSound );
				}

				return rkChatInfo.m_ChatIconName;
			}
		}
	}

	return m_DefaultChatIcon;
}

void ioChatMacro::FilterChar( const char *src, char *dst, const char *chars )
{
	char *srcp = (char *) src;
    char *dstp = dst;
	
    while (*srcp) 
	{
		if (strchr(chars, *srcp) == NULL) 
		{
			*dstp = *srcp;
			dstp += 1;
		}
		else
		{
			int msb, lsb;
			msb = *(unsigned char *)(srcp-1);
			lsb = *(unsigned char *)srcp;
			if (srcp > src && (
				(msb == 0x82 && lsb > 0x9e && lsb < 0xf2)	// hirakana
				|| (msb == 0x83 && lsb > 0x3f && lsb < 0x97)	// katakana
				|| (msb >= 0x88 && msb <= 0x9f && msb != 0x7f	// kanji 1 group
				&& lsb > 0x3f && lsb < 0xfd)
				|| (msb >= 0xe0 && msb <= 0xfc && msb != 0x7f	// kanji 2 group
				&& lsb > 0x3f && lsb < 0xfd))) 
			{
				*dstp = *srcp;   
				dstp += 1;
			}
		}
		srcp += 1;
    }
    *dstp = 0;
}

bool ioChatMacro::CheckMacroChatEnable()
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwDisableTime != 0 && m_dwDisableTime > dwCurTime )
		return false;

	if( m_dwMacroChatCheckTime == 0 )
	{
		m_dwMacroChatCheckTime = dwCurTime;
		m_iCurChatCnt = 1;
		return true;
	}
	else if( m_dwMacroChatCheckTime + m_dwMacroChatGapTime > dwCurTime )
	{
		if( m_iCurChatCnt < m_iLimiteChatCnt )
		{
			m_iCurChatCnt++;
			return true;
		}

		m_dwMacroChatCheckTime = 0;
		m_dwDisableTime = dwCurTime + m_dwDisableGapTime;
		m_iCurChatCnt = 0;
		return false;
	}
	else
	{
		m_dwMacroChatCheckTime = dwCurTime;
		m_iCurChatCnt = 1;
		return true;
	}
}

bool ioChatMacro::CheckNormalChatEnable()
{
	if( m_dwSoundCheckTime == 0 || m_dwSoundCheckTime + m_dwSoundGapTime < FRAMEGETTIME() )
	{
		m_iCurSoundCnt = 1;
		m_dwSoundCheckTime = FRAMEGETTIME();
		g_ChatMgr.SetNoChatSound( false );
	}
	else
	{
		m_iCurSoundCnt++;
		if( m_iCurSoundCnt > m_iSoundLimiteCount )
			g_ChatMgr.SetNoChatSound( true );
		else
			g_ChatMgr.SetNoChatSound( false );
	}

	if( m_dwNormalChatCheckTime == 0 || m_dwNormalChatCheckTime + m_dwNormalChatGapTime < FRAMEGETTIME() )
	{
		m_iCurNormalChatCnt = 1;
		m_dwNormalChatCheckTime = FRAMEGETTIME();
		return true;
	}
	else
	{
		m_iCurNormalChatCnt++;
		if( m_iCurNormalChatCnt > m_iNormalChatLimiteCount )
			return false;

		return true;
	}
}

ioChatMacro& ioChatMacro::GetSingleton()
{
	return Singleton< ioChatMacro >::GetSingleton();
}