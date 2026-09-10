#pragma once

#include "GUI/ioChannelManager.h"

class ioGuildChatMgr : public Singleton< ioGuildChatMgr >
{
public:
	enum
	{
		CHAT_ID_COLOR       = 0xFFFF8200,
		CHAT_NORMAL_COLOR   = 0xFFFFB400,
		CHAT_ANNOUNCE_COLOR = 0xFFE5E5E5,
	};

protected:
	vChatInfo    m_vChatInfo;
	bool         m_bUpdate;
	DWORD        m_dwSentenceIndexer;
    
public:
	void GetUserName( char *szName, int iMaxSize );
	int  GetUserSize();

	int  GetChatInfoSize(){ return m_vChatInfo.size(); }
	void ClearChatInfoList();

public:
	void AddSentenceIndex();

public:
	ChatInfo *GetChatInfo( int iArray );	
	void SetChatInfo( ioComplexStringPrinter &szChat , const char *szID , bool bFirst );
	bool IsMannerTrialChatHaveID( const ioHashString &rszID );
	void GetMannerTrialChat( OUT ioHashString &rkChatList );

public:
	bool IsUpdate();
	void ClearUpdate(){ m_bUpdate = false; }

public:
	void ShowChatPopup( ioHashString szID, ioHashString szChat );

public:
	void AddAnnounce( ioHashString szAnnounce );
	void AddChat( const char *szID, ioHashString szChat );

public:   // ÆÐÅ¶
	void OnLoginUser( ioHashString szLoginUser );
	void OnLogOutUser( ioHashString szLogOutUser );
	void OnChat( ioHashString szID, ioHashString szChat );

public:
	static ioGuildChatMgr &GetSingleton();

public:
	ioGuildChatMgr();
	virtual ~ioGuildChatMgr();	
};
#define g_GuildChatMgr ioGuildChatMgr::GetSingleton()
