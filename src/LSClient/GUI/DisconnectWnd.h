#pragma once

class DisconnectWnd : public ioWnd
{
public:
	enum
	{
		TYPE_NONE          = 0,
		TYPE_LOCAL_EXIT    = 1,
		TYPE_NETWORK_EXIT  = 2,
		TYPE_LANCABLE_EXIT = 3,
		TYPE_XTRAP_EXIT    = 4,
		TYPE_BLOCK_EXIT    = 5,
		TYPE_SERVER_TO_CLIENT_PING_EXIT = 6,
		TYPE_NPROTECT_EXIT = 7,
		TYPE_QUEST_ABUSE_EXIT = 8,
		TYPE_SHUT_DOWN_EXIT   = 9,
		TYPE_SELECT_SHUT_DOWN_EXIT = 10,
	};

	enum
	{
		ID_OK				= 1,
	};

protected:
	DWORD m_dwShowTime;
	int   m_iType;
	DWORD m_dwMainIndex;
	DWORD m_dwSubIndex;

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void iwm_hide();
	virtual void iwm_show();
	virtual void iwm_create();

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	void SetType(int iType);
	void SetQuestData( DWORD dwMainIndex, DWORD dwSubIndex );

public:
	DisconnectWnd();
	virtual ~DisconnectWnd();
};

