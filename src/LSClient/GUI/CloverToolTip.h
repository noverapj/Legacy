#pragma once

#include "../ioComplexStringPrinter.h"

class CloverToolTip : public ioWnd
{
protected:
	enum MSG_TYPE
	{
		TYPE_NONE,
		TYPE_SEND,
		TYPE_RECV,
	};

	enum
	{
		WND_HEIGHT		= 26,
		START_TEXT_XPOS = 7,
		START_TEXT_YPOS = 6,
		SIDE_TEXT_GAP	= 14,
		TEXT_LINE_GAP   = 16,
	};

	ioHashString m_szUserName;
	ioComplexStringPrinter m_Printer[MAX_MSG_LIST_SIZE];

	MSG_TYPE m_Type;
	bool m_bFriendRegTime;
	bool m_bSendRemainTime;
	int m_iSendCloverCount;
	int m_iRecvCloverCount;
	int m_iRecvWaitCloverCount;

public:
	virtual void iwm_show();	
	virtual void iwm_hide();
	virtual void iwm_create();

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

protected:
	void RenderSendInfoString( int iXPos, int iYPos );
	void RenderRecvInfoString( int iXPos, int iYPos );

public:
	void SetInfo( const ioHashString &szUserName, int iType );

public:
	CloverToolTip();
	virtual ~CloverToolTip();
};
