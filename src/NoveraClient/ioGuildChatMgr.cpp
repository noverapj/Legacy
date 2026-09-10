#include "stdafx.h"

#include "ioGuildChatMgr.h"

#include "GUI/MessengerWnd.h"
#include "GUI/HelpWnd.h"

template<> ioGuildChatMgr* Singleton< ioGuildChatMgr >::ms_Singleton = 0;
ioGuildChatMgr::ioGuildChatMgr() : m_bUpdate( false )
{
	m_dwSentenceIndexer = 0;
}

ioGuildChatMgr::~ioGuildChatMgr()
{
	ClearChatInfoList();
}

ioGuildChatMgr& ioGuildChatMgr::GetSingleton()
{
	return Singleton< ioGuildChatMgr >::GetSingleton();
}

void ioGuildChatMgr::ClearChatInfoList()
{
	vChatInfo_iter iter;
	for(iter = m_vChatInfo.begin();iter != m_vChatInfo.end();iter++)
	{
		SAFEDELETE( *iter );
	}
	m_vChatInfo.clear();
}

bool ioGuildChatMgr::IsUpdate()
{
	if( !g_GuildInfo.IsGuild() )
		m_bUpdate = false;
	return m_bUpdate;
}

void ioGuildChatMgr::GetUserName( char *szName, int iMaxSize )
{
	char szBuf[MAX_PATH] = "";    
	ioGuildData *pGuildData = g_GuildInfo.GetGuildData( g_GuildInfo.GetGuildIndex() );
	if( pGuildData )
	{
		int iSize = pGuildData->GetGuildJoinUser();
		for(int i = 0;i < iSize;i++)
		{
			bool bOnline = false;
			ioHashString szUserID;
			pGuildData->GetUserInfo( i, szUserID, bOnline );
			if( szUserID.IsEmpty() ) continue;
			if( !bOnline ) continue;

			if( strlen( szBuf ) == 0 )
				sprintf( szBuf, "%s", szUserID.c_str() );
			else
				sprintf( szBuf, " / %s", szUserID.c_str() );

			strcat( szName, szBuf );
			if( (int)strlen( szName ) >= iMaxSize )
				break;
		}
	}
}

int ioGuildChatMgr::GetUserSize()
{
	ioGuildData *pGuildData = g_GuildInfo.GetGuildData( g_GuildInfo.GetGuildIndex() );
	if( pGuildData )
	{
		return pGuildData->GetOnlineUserSize();
	}
	return 0;
}

ChatInfo *ioGuildChatMgr::GetChatInfo( int iArray )
{
	if( !COMPARE( iArray, 0, (int)m_vChatInfo.size() ) )
		return NULL;
	return m_vChatInfo[iArray];
}

void ioGuildChatMgr::SetChatInfo( ioComplexStringPrinter &szChat , const char *szID , bool bFirst )
{
	ChatInfo *pChatNode = new ChatInfo;
	if( !pChatNode ) return;

	pChatNode->m_Index  = m_dwSentenceIndexer;
	pChatNode->m_bFirst = bFirst;
	szChat.CopyFullText( pChatNode->m_Printer );
	szChat.CopyFullText( pChatNode->m_OverPrinter );
	pChatNode->m_szID = szID;
	pChatNode->m_OverPrinter.SetFullTextColor( 0xCE4E00 );    //오버는 주황색

	m_vChatInfo.push_back( pChatNode );
	m_bUpdate = true;
	MessengerBtn::sg_bMessenger = true;	
}

void ioGuildChatMgr::AddSentenceIndex()
{
	m_dwSentenceIndexer++;
}

bool ioGuildChatMgr::IsMannerTrialChatHaveID( const ioHashString &rszID )
{
	enum { MAX_CHECK_CHAT = 20, };

	int iSize = GetChatInfoSize();
	int iStart = 0;
	if( iSize > MAX_CHECK_CHAT )
		iStart = iSize - MAX_CHECK_CHAT;

	for( int i = iStart; i < iSize; i++ )
	{
		ChatInfo *pChatInfo = GetChatInfo( i );
		if( !pChatInfo ) continue;
		if( rszID == pChatInfo->m_szID )
		{
			return true;
		}
	}

	return false;
}

void ioGuildChatMgr::GetMannerTrialChat( OUT ioHashString &rkChatList )
{
	std::string szMerge;

	enum { MAX_CHECK_CHAT = 20, };

	int iSize = GetChatInfoSize();
	int iStart = 0;
	if( iSize > MAX_CHECK_CHAT )
		iStart = iSize - MAX_CHECK_CHAT;

	for( int i = iStart; i < iSize; i++ )
	{
		ChatInfo *pChatInfo = GetChatInfo( i );
		if( !pChatInfo ) continue;
		if( pChatInfo->m_Printer.IsEmpty() ) continue;

		std::string szLine;
		pChatInfo->m_Printer.CopyOnlyText( szLine );
		szMerge += szLine;
		szMerge += "\n";
	}

	if( !szMerge.empty())
		rkChatList = szMerge.c_str();
}


void ioGuildChatMgr::ShowChatPopup( ioHashString szID, ioHashString szChat )
{
	if( !Setting::GuildChatAlarm() ) return;
	if( g_BattleRoomMgr.IsBattleRoom() )
	{
		if( g_BattleRoomMgr.IsTournamentModeType() || g_BattleRoomMgr.IsBroadcastModeType() ) 
			return;
	}

	MessengerPopup *pPopup = dynamic_cast<MessengerPopup*>(g_GUIMgr.FindWnd( MESSENGER_POPUP ));
	if( !pPopup )	return;

	pPopup->ShowPopup( MessengerPopup::GUILD_POPUP, szID, szChat );
}

void ioGuildChatMgr::AddAnnounce( ioHashString szAnnounce )
{
	AddSentenceIndex();

	MessengerBtn::sg_bMessenger = true;	

	const float fScale = FONT_SIZE_12;
	const float fWidhtSize = 265.0f;

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );

	char szSrcChat[MAX_PATH];
	SafeSprintf( szSrcChat, sizeof( szSrcChat ), STR(1), szAnnounce.c_str() );

	// 1라인을 넘어서지 않는다.
	if( g_FontMgr.GetTextWidth( szSrcChat, TS_NORMAL, fScale ) < fWidhtSize )
	{		
		kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
		kPrinter.AddTextPiece( fScale, szSrcChat );
		SetChatInfo( kPrinter , "", true );
	}	
	else
	{
		int iLen    = strlen( szSrcChat );
		int iResult = 0;
		bool bFirst = true;
		while( iResult < iLen )
		{
			char szDstChat[MAX_PATH];
			memset( szDstChat, 0, sizeof( szDstChat ) );
			iResult += Help::StringCutFun( fScale, fWidhtSize, TS_NORMAL, szDstChat, sizeof(szDstChat), &szSrcChat[iResult] );

			kPrinter.SetTextStyle( TS_NORMAL );
			kPrinter.SetBkColor( 0, 0, 0 );
			kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
			kPrinter.AddTextPiece( fScale, szDstChat );
			SetChatInfo( kPrinter ,"", bFirst);
			bFirst = false;
			kPrinter.ClearList();
		}
	}	
}

void ioGuildChatMgr::AddChat( const char *szID, ioHashString szChat )
{
	AddSentenceIndex();

	MessengerBtn::sg_bMessenger = true;
	g_SoundMgr.PlaySound( "interface\\team_invite.wav" );

	const float fScale = FONT_SIZE_12;
	const float fWidhtSize = 265.0f;
	float fIDWidth = 0.0f;

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );

	char szSrcChat[MAX_PATH];
	memset( szSrcChat, 0, sizeof( szSrcChat ) );
	sprintf( szSrcChat, "%s", szChat.c_str() );

	fIDWidth = g_FontMgr.GetTextWidth( szID, TS_NORMAL, fScale );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( fScale, STR(1), szID );

	// 1라인을 넘어서지 않는다.
	if( g_FontMgr.GetTextWidth( szSrcChat, TS_NORMAL, fScale ) < fWidhtSize - fIDWidth )
	{		
		kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
		kPrinter.AddTextPiece( fScale, szSrcChat );
		SetChatInfo( kPrinter , szID, true );
	}	
	else
	{
		int iLen    = strlen( szSrcChat );
		int iResult = 0;
		bool bFirst = true;
		while( iResult < iLen )
		{
			char szDstChat[MAX_PATH];
			memset( szDstChat, 0, sizeof( szDstChat ) );
			iResult += Help::StringCutFun( fScale, fWidhtSize - fIDWidth, TS_NORMAL, szDstChat, sizeof(szDstChat), &szSrcChat[iResult] );

			fIDWidth = 0.0f;
			kPrinter.SetTextStyle( TS_NORMAL );
			kPrinter.SetBkColor( 0, 0, 0 );
			kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
			kPrinter.AddTextPiece( fScale, szDstChat );
			SetChatInfo( kPrinter , szID, bFirst );
			bFirst = false;
			kPrinter.ClearList();			
		}
	}	
}

void ioGuildChatMgr::OnLoginUser( ioHashString szLoginUser )
{
	char szChat[MAX_PATH];
	SafeSprintf( szChat, sizeof( szChat ), STR(1), szLoginUser.c_str() );
	AddAnnounce( szChat );
}

void ioGuildChatMgr::OnLogOutUser( ioHashString szLogOutUser )
{
	char szChat[MAX_PATH];
	SafeSprintf( szChat, sizeof( szChat ), STR(1), szLogOutUser.c_str() );
	AddAnnounce( szChat );
}

void ioGuildChatMgr::OnChat( ioHashString szID, ioHashString szChat )
{
	if(g_BlackListManager.IsBlackList(szID)) return;

	AddChat( szID.c_str(), szChat );
	if( szID != g_MyInfo.GetPublicID() )
		ShowChatPopup(  szID, szChat );
}
