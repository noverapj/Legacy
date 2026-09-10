#pragma once

class UsePresentEtcItemWnd : public ioWnd
{
public:
	enum
	{
		ID_EXIT         = 1,
		ID_SEND_BTN		= 2,
		ID_CANCEL_BTN   = 3,
		ID_PRESENT_BTN  = 4,
		ID_FRIEND_BTN   = 5,
		
		ID_SEND_PRESENT_CONFIRM = 10,

		ID_PRESENT_LIST = 1000,
		ID_FRIEND_LIST  = 1001,
	};

protected:
	ioUIRenderImage *m_pEmptyIcon;
	ioUIRenderImage *m_pPresentIcon;
	ioUIRenderImage *m_pSubIcon;
	ioUIRenderImage *m_pStripGrayDark;
	std::vector<ioUIRenderImage*> m_vItemIcon;
	std::vector<ioUIRenderImage*> m_vSubIcon;

protected:
	DWORD m_dwEtcItemIndex;
	ioEtcItemSendPresent::vPresentData m_vPresentList;
	int          m_iSelectedPresent;
	ioHashString m_SelectedFriendName;
	ioHashString m_PresentItemName;

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

public:
	void SendPresent();
	void SendPresentComplete();

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void iwm_show();	
	virtual void iwm_hide();
	virtual bool iwm_esc();

public:
	void SetInfo( DWORD dwEtcItemIdx, const ioEtcItemSendPresent::vPresentData &PresentList );
	void SetSelectedPresentArray( int iArray ) { m_iSelectedPresent = iArray; }
	void SetSelectedFriendName( const ioHashString &szName ) { m_SelectedFriendName = szName; }

protected:
	virtual void OnRender();

	void SetConfirmWnd();

public:
	UsePresentEtcItemWnd();
	virtual ~UsePresentEtcItemWnd();
};
//////////////////////////////////////////////////////////////////////////
class FriendListWnd : public ioWnd
{
public:
	enum
	{
		ID_VERT_SCROLL = 1000,		
	};

	enum
	{
		MAX_PAGE_LIST = 10,
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
	ioHashStringVec m_FriendList;
	ioHashString    m_szOverListUser;
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

public:
	FriendListWnd();
	virtual ~FriendListWnd();
};

//////////////////////////////////////////////////////////////////////////
class PresentListWnd : public ioWnd
{
public:
	enum
	{
		ID_VERT_SCROLL = 1000,		
	};

	enum
	{
		MAX_PAGE_LIST = 10,
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
	struct PresentInfo
	{
		ioHashString m_Name;
		int m_iArray;
		PresentInfo( const ioHashString &szName, int iArray )
		{
			m_Name = szName;
			m_iArray = iArray;
		}
	};
	typedef std::vector<PresentInfo> vPresentInfo;
	vPresentInfo m_PresentList;
	int          m_iSelectedPresent;
	ioHashString m_szOverListItem;
	int          m_iCurPos;	

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
	void SetPresentList( const ioEtcItemSendPresent::vPresentData &PresentList );

	const int GetSelectedPresentArray() { return m_iSelectedPresent; }

public:
	PresentListWnd();
	virtual ~PresentListWnd();
};
//////////////////////////////////////////////////////////////////////////
class SendPresentEtcItemConfirmWnd : public ioWnd
{
public:
	enum
	{
		ID_EXIT_BTN       = 1,
		ID_OK_BTN         = 2,
		ID_CANCEL_BTN     = 3, 
	};

	enum
	{   // 높이값만 바뀐다
		NORMAL_HEIGHT     = 127,
		MGAME_HEIGHT      = 147,
	};
protected:
	ioUIRenderImage *m_pItemIcon;
	ioUIRenderImage *m_pSubIcon;

protected:
	ioComplexStringPrinter m_PrinterList[MAX_MSG_LIST_SIZE];
	ioWnd *m_pReturnWnd;

protected:
	void SendPresent();

public:
	void SetInfo( ioWnd *pReturnWnd, const ioHashString &szFriendName, const ioEtcItemSendPresent::PresentData &kPresent );

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnRender();

public:
	SendPresentEtcItemConfirmWnd();
	virtual ~SendPresentEtcItemConfirmWnd();
};
