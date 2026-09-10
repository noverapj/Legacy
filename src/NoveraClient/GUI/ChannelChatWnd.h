#pragma once

#include "ioChannelManager.h"
#include "../io3DEngine/ioButton.h"

//--------------------------------------------------------------------------------------------------------------------------------------------------
class LineChatWnd : public ioWnd
{
protected:
	int	 m_iChannelIdx;
	int  m_iChatInfoArray;
	bool m_bChatOver;
	
public:
	void  SetChatInfo( int iChannelIdx, int iChatInfoArray );
	DWORD GetSentenceIndex();
	void  SetBigBtnOver( bool bOver );
	bool  IsBigBtnOver();
	void  OpenMannerWnd();

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRenderAfterChild();

public:
	LineChatWnd();
	virtual ~LineChatWnd();
};
//--------------------------------------------------------------------------------------------------------------------------------------------------
class ChannelChatWnd : public ioWnd
{
public:
	enum
	{
		//BTN
		ID_CHAT_SEND     = 1,
		ID_CHANNEL_INVITE= 2,
		ID_CHANNEL_LEAVE = 3,
		ID_EDIT_CHAT     = 4,

		//SCROLL
		ID_VERT_SCROLL    = 1000,

		//wnd
		ID_LINE_CHAT_WND_1   = 101,
		ID_LINE_CHAT_WND_2   = 102,
		ID_LINE_CHAT_WND_3   = 103,
		ID_LINE_CHAT_WND_4   = 104,
		ID_LINE_CHAT_WND_5   = 105,
		ID_LINE_CHAT_WND_6   = 106,
		ID_LINE_CHAT_WND_7   = 107,
		ID_LINE_CHAT_WND_8   = 108,
		ID_LINE_CHAT_WND_9   = 109,
		ID_LINE_CHAT_WND_10  = 110,
	};

	enum
	{
		MAX_LIST_SIZE = 10,
	};
	
protected:
	int m_iCurPage;
	int m_iChannelIdx;
	ioWnd *m_pPreEdit;

	int   m_iChannelUserFrame;
	DWORD m_dwChannelUserTimer;
	char  m_szChannelUser[2048];

	bool  m_bSettingScrollPos;

	void SetScrollSetting( bool bSettingScrollPos );
	bool MacroProcess( const char *szChat );
	void ProcessChannelUser();
	void ProcessChatInfo();
	void OnInvite( char *szString );

public:
	void ErrorEdit( char *szError );
	void SetChannelIndex( int iIndex );
	int GetChannelIndex(){ return m_iChannelIdx; }
	void SendChat();
	void InviteResult( int iResult, ioHashString szInvitedName );
	bool IsChatBlock();
	void SetChatFocus();

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

public:
	virtual void iwm_vscroll(DWORD dwID, int curPos );
	virtual void iwm_wheel( int zDelta );
	virtual void iwm_lbuttondown( const ioMouse& mouse );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	ChannelChatWnd();
	virtual ~ChannelChatWnd();
};

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------
class GuildLineChatWnd : public ioWnd
{
protected:
	int  m_iChatInfoArray;
	bool m_bChatOver;

public:
	void SetChatInfo( int iChatInfoArray );
	DWORD GetSentenceIndex();
	void  SetBigBtnOver( bool bOver );
	bool  IsBigBtnOver();
	void  OpenMannerWnd();

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRenderAfterChild();

public:
	GuildLineChatWnd();
	virtual ~GuildLineChatWnd();
};

//--------------------------------------------------------------------------------------------------------------------------------------------------
class GuildChatWnd : public ioWnd
{
public:
	enum
	{
		//BTN
		ID_CHAT_SEND       = 1,
		ID_GUILD_INFO	   = 2,
		ID_GUILD_HOME_PAGE = 3,
		ID_EDIT_CHAT       = 4,

		//SCROLL
		ID_VERT_SCROLL    = 1000,

		//wnd
		ID_LINE_CHAT_WND_1   = 101,
		ID_LINE_CHAT_WND_2   = 102,
		ID_LINE_CHAT_WND_3   = 103,
		ID_LINE_CHAT_WND_4   = 104,
		ID_LINE_CHAT_WND_5   = 105,
		ID_LINE_CHAT_WND_6   = 106,
		ID_LINE_CHAT_WND_7   = 107,
		ID_LINE_CHAT_WND_8   = 108,
		ID_LINE_CHAT_WND_9   = 109,
		ID_LINE_CHAT_WND_10  = 110,
	};

	enum
	{
		MAX_LIST_SIZE = 10,
	};

protected:
	int    m_iCurPage;
	ioWnd *m_pPreEdit;

	int   m_iGuildUserFrame;
	DWORD m_dwGuildUserTimer;
	char  m_szGuildUser[2048];

	bool  m_bSettingScrollPos;

	void SetScrollSetting( bool bSettingScrollPos );
	void ProcessGuildUser();
	void ProcessChatInfo();

public:
	void SendChat();
	bool IsChatBlock();
	void SetChatFocus();

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	
public:
	virtual void iwm_vscroll(DWORD dwID, int curPos );
	virtual void iwm_wheel( int zDelta );
	virtual void iwm_lbuttondown( const ioMouse& mouse );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	GuildChatWnd();
	virtual ~GuildChatWnd();
};
