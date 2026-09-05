#include "StdAfx.h"
#include "../io3DEngine/iostringmanager.h"
#include "../NetworkWrappingFunc.h"

#include ".\mannertrialchat.h"

CMannerTrialChat::CMannerTrialChat(void)
{
	m_iMaxChat = 0;
}

CMannerTrialChat::~CMannerTrialChat(void)
{
	DeleteAll();
}

void CMannerTrialChat::DeleteAll()
{
	for(ChatList::iterator it = m_ChatList.begin(); it != m_ChatList.end(); ++it)
	{
	    delete *it;
	}

	m_ChatList.clear();
}

void CMannerTrialChat::Add( const char *szID, const char *szChat )
{
	ChatItem *pItem = new ChatItem;
	if( pItem )
	{
		pItem->szID   = szID;
		pItem->szChat = szChat;
		m_ChatList.push_back( pItem ); 
		CheckDeleteHead();
	}
}

void CMannerTrialChat::DeleteHead()
{
	if( !m_ChatList.empty() )
	{
		ChatItem *pItem = m_ChatList.front();
		if( pItem )
			delete pItem;

		m_ChatList.pop_front();
	}
}

void CMannerTrialChat::CheckDeleteHead()
{
	if( (int)m_ChatList.size() <= m_iMaxChat )
		return;

	while( (int)m_ChatList.size() > m_iMaxChat)
		DeleteHead();
}

void CMannerTrialChat::SetMaxChat( int iMax )
{
	m_iMaxChat = iMax;
	CheckDeleteHead();
}

bool CMannerTrialChat::IsHaveID( const ioHashString &rszID )
{
	for(ChatList::const_iterator it = m_ChatList.begin(); it != m_ChatList.end(); ++it)
	{
	    if( (*it)->szID == rszID )
			return true;
	}
	return false;
}

void CMannerTrialChat::GetChatList( OUT ioHashString &rkChatList )
{
	std::string szMerge;
	for(ChatList::const_iterator it = m_ChatList.begin(); it != m_ChatList.end(); ++it)
	{
		if( (*it) == NULL ) continue;
		szMerge += (*it)->szID.c_str();
		szMerge += ":";
		szMerge += (*it)->szChat.c_str();
		szMerge += "\n";
	}

	if( !szMerge.empty())
		rkChatList = szMerge.c_str();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<> ioMannerTrialChatManager* Singleton< ioMannerTrialChatManager >::ms_Singleton = 0;

ioMannerTrialChatManager::ioMannerTrialChatManager()
{
	for (int i = TT_NONE; i <  TT_MAX ; i++)
	{
		if( i == TT_NONE )
			continue;
		CMannerTrialChat *pChat = new CMannerTrialChat;
		m_MannerTrialChatMap.insert( MannerTrialChatMap::value_type( i, pChat ) );
	}
}

ioMannerTrialChatManager::~ioMannerTrialChatManager()
{
	for(MannerTrialChatMap::iterator iter = m_MannerTrialChatMap.begin(); iter != m_MannerTrialChatMap.end(); ++iter)
	{
	    delete iter->second;
	}
	m_MannerTrialChatMap.clear();
}

ioMannerTrialChatManager& ioMannerTrialChatManager::GetSingleton()
{
	return Singleton< ioMannerTrialChatManager >::GetSingleton();
}

void ioMannerTrialChatManager::SetMaxChat( int iMax )
{
	for(MannerTrialChatMap::iterator iter = m_MannerTrialChatMap.begin(); iter != m_MannerTrialChatMap.end(); ++iter)
	{
	    CMannerTrialChat *pChat = iter->second;
		if( pChat )
			pChat->SetMaxChat( iMax );
	}
}

void ioMannerTrialChatManager::Add( TrialType eTrialType, const char *szID, const char *szChat )
{
	MannerTrialChatMap::iterator iter = m_MannerTrialChatMap.find( (DWORD) eTrialType );
	if( iter == m_MannerTrialChatMap.end() )
		return;
	CMannerTrialChat *pChat = iter->second;
	if( !pChat )
		return;
	pChat->Add( szID, szChat );
}

bool ioMannerTrialChatManager::IsHaveID( TrialType eTrialType, const ioHashString &rszID )
{
	MannerTrialChatMap::iterator iter = m_MannerTrialChatMap.find( (DWORD) eTrialType );
	if( iter == m_MannerTrialChatMap.end() )
		return false;
	CMannerTrialChat *pChat = iter->second;
	if( !pChat )
		return false;
	return pChat->IsHaveID( rszID );
}

void ioMannerTrialChatManager::GetChatList( IN TrialType eTrialType, OUT ioHashString &rkChatList )
{
	MannerTrialChatMap::iterator iter = m_MannerTrialChatMap.find( (DWORD) eTrialType );
	if( iter == m_MannerTrialChatMap.end() )
		return;
	CMannerTrialChat *pChat = iter->second;
	if( !pChat )
		return;
	pChat->GetChatList( rkChatList );
}

const char *ioMannerTrialChatManager::GetTrialTypeName( TrialType eTrialType )
{
	if( eTrialType == TT_NORMAL_CHAT )
		return STR(1);
	else if( eTrialType == TT_CHANNEL_CHAT )
		return STR(2);
	else if( eTrialType == TT_GUILD_CHAT )
		return STR(3);
	else if( eTrialType == TT_MEMO )
		return STR(4);
	else if( eTrialType == TT_BATTLE_CHAT )
		return STR(5);
	else if( eTrialType == TT_SERVER_LOBBY_CHAT )
		return STR(6);
	else if( eTrialType == TT_WHOLE_SERVER_CHAT )
		return STR(7);
	else if( eTrialType == TT_LADDER_CHAT )
		return STR(8);
	else if( eTrialType == TT_WHOLE_SERVER_RAINBOW_CHAT )
		return STR(9);
	else
		return STR(10);
}