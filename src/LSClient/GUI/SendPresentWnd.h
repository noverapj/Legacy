#pragma once

#include "../io3DEngine/ioMovingWnd.h"
class NewShopBuyWnd;

class SendPresentIdWnd : public ioWnd
{
public:
	enum
	{
		ID_EXIT   = 1,
		ID_NEXT   = 2,
		ID_CANCEL = 3, 
		ID_FRIEND_BTN  = 4,

		ID_FRIEND_LIST = 1000,
	};
protected:
	NewShopBuyWnd *m_pReturnWnd;

	short  m_iPresentType;
	int    m_iPresentValue1;
	int    m_iPresentValue2;

	ioHashString m_szFriend;

protected:
	bool ShowConfirmWnd();

public:
	void ShowSendPresentIdWnd( NewShopBuyWnd *pReturnWnd, short iPresentType, int iPresentValue1, int iPresentValue2 );

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnRender();

public:
	SendPresentIdWnd();
	virtual ~SendPresentIdWnd();
};
//////////////////////////////////////////////////////////////////////////
class SendPresentFriendListWnd : public ioWnd
{
public:
	enum
	{
		ID_VERT_SCROLL = 1000,		
	};

	enum
	{
		MAX_PAGE_LIST = 5,
	};

protected:
	ioUIRenderImage *m_pDarkLine;
	ioUIRenderImage *m_pLightLine;
	ioUIRenderImage *m_pOverLine;

	int   m_iAddWidth;
	int   m_iAddHeight;
	int   m_iListXPos;
	int   m_iListYPos;
	int   m_iHeightGap;
	int   m_iCutWidth;
	int   m_iStartXPos;
	int   m_iStartYPos;

protected:
	ioHashStringVec m_FriendUserList;
	ioHashString    m_szOverListUser;
	ioHashString    m_szPushListUser;
	int				m_iCurPos;	

public:
	virtual void ParseExtraInfo( ioXMLElement &xElement );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_wheel( int zDelta );
	virtual void iwm_vscroll( DWORD dwID, int curPos );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	const ioHashString &GetOverUser(){ return m_szOverListUser; }
	const ioHashString &GetPushUser(){ return m_szPushListUser; }

public:
	SendPresentFriendListWnd();
	virtual ~SendPresentFriendListWnd();
};

//---------------------------------------------------------------------------------------------------------------------------------
class SendPresentConfirmWnd : public ioWnd
{
public:
	enum
	{
		ID_EXIT_BTN       = 1,
		ID_OK_BTN         = 2,
		ID_CANCEL_BTN     = 3, 
		ID_CLAUSE_VIEW_BTN= 4,
	};

	enum
	{   // 높이값만 바뀐다
		NORMAL_HEIGHT     = 398,
		MGAME_HEIGHT      = 438,
	};

protected:
	ioUIRenderImage *m_pPresentIcon;
	ioUIRenderImage *m_pSubIcon;
	ioUIRenderImage *m_pCostumeMark;

	NewShopBuyWnd   *m_pReturnWnd;

	short        m_iPresentType;
	int          m_iPresentValue1;
	int          m_iPresentValue2;
	ioHashString m_szRecvPublicID;

	int          m_iBonusPeso;
	int          m_iCash;

	bool m_bIsUnableBonus;
	bool m_bIsEnableBonus;

protected:
	void SendBuyFromReturnWnd();
	void SetPresentIcon();

	int GetBonusPeso( short iPresentType, int iPresentValue1, int iPresentValue2 );
	int GetCash( short iPresentType, int iPresentValue1, int iPresentValue2 );

public:
	void ShowSendPresentConfirmWnd( NewShopBuyWnd *pReturnWnd, short iPresentType, int iPresentValue1, int iPresentValue2, const char *szRecvPublicID );
	const ioHashString &GetRecvPublicID() { return m_szRecvPublicID; }
	void SendPresentComplete(); 

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

public:
	virtual void iwm_show();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnRender();

public:
	SendPresentConfirmWnd();
	virtual ~SendPresentConfirmWnd();
};
