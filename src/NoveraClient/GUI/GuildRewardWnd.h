#pragma once
#include "NewShopIconBtn.h"

class RewardItemListIconBtn : public NewShopIconBtn
{
protected:
	ioComplexStringPrinter		m_ItemNamePrinter;
	ioHashString				m_szItemName;

protected:
	virtual void OnRender();

public:
	void SetItem( int iPresentType, int iValue1, int iValue2 );
	const ioHashString& GetItemName() { return m_szItemName; }

public:
	RewardItemListIconBtn();
	virtual ~RewardItemListIconBtn();
};
/////////////////////////////////////////////////////////////////////////////
class GuildPossibleRewardListWnd : public ioWnd
{
protected:
	enum
	{
		ID_PRE_BTN	= 1,
		ID_NEXT_BTN	= 2,
		ID_X		= 3,

		ID_RADIO_BTN1 = 10,
		ID_RADIO_BTN2 = 11,
		ID_RADIO_BTN3 = 12,
		ID_RADIO_BTN4 = 13,
		ID_RADIO_BTN5 = 14,

		ID_ICON_BTN1	= 100,
		ID_ICON_BTN2	= 101,
		ID_ICON_BTN3	= 102,
		ID_ICON_BTN4	= 103,
		ID_ICON_BTN5	= 104,
		ID_ICON_BTN6	= 105,
		ID_ICON_BTN7	= 106,
		ID_ICON_BTN8	= 107,
		ID_ICON_BTN9	= 108,
		ID_ICON_BTN10	= 109,
		ID_ICON_BTN11	= 110,
		ID_ICON_BTN12	= 111,

		ID_TOOLTIP = 200,
	};

	enum
	{
		MAX_TEXT    = 2,
		MAX_RADIO_BTN	= 5,
		MAX_ICON_BTN	= 12,
	};

protected:
	int						 m_iRewardType;
	DWORD					 m_dwCurRadioBtn;
	vGashaponPresent		 m_RewardItemVec;
	ioHashString			 m_szAttendTitleName;
	ioHashString			 m_szRankTitleName;

	ioComplexStringPrinter m_PagePrinter;
	ioComplexStringPrinter m_UnderLinePrinter[MAX_TEXT];

	int m_iMaxPage;
	int m_iCurPage;

public:
	void ShowRewardWnd( int iType );

protected:
	void ClearData();
	void SetTitleByType();
	void ChangeRadioBtn( DWORD dwSubBtn );
	void UpdatePage( int iPage );
	void UpdateIcon();
//virtual func
public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void ParseExtraInfo( ioXMLElement &xElement );
	virtual void OnRender();

public:
	GuildPossibleRewardListWnd(void);
	virtual ~GuildPossibleRewardListWnd(void);
};
//////////////////////////////////////////////////////////////
class GuildAttendUserBtn : public ioButton
{
protected:
	DWORD        m_dwBtnArray;
	ioHashString m_szUserID;
	bool		 m_bTodayAttend;
	bool		 m_bYesterdayAttend;
	int			 m_iUserLevel;

	ioUIRenderImage *m_pGrayAttendance;
	ioUIRenderImage *m_pRedAttendance;
	ioUIRenderImage *m_pGreenAttendance;

protected:
	void ClearData();

public:
	void MemberInfo( int iListArray, const ioHashString &szUserID, int iUserLevel, bool &bTodayAttend, bool &YesterdaybAttend );

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void OnRender();	

public:
	GuildAttendUserBtn();
	virtual ~GuildAttendUserBtn();
};
//////////////////////////////////////////////////////////////
class GuildAttendInfoListWnd : public ioWnd
{
public:
	enum
	{
		ID_USER_LIST_1   = 0,              //32개의 유저 버튼이 있다.
		ID_USER_LIST_32  = 31,

		ID_OPTION_WND = 1000,
	};

public:
	void UpdateUserList( ioGuildData *pGuildData );

protected:
	virtual void iwm_show();

public:
	GuildAttendInfoListWnd();
	virtual ~GuildAttendInfoListWnd();
};
//////////////////////////////////////////////////////////////
class GuildAttendRewardWnd : public ioWnd
{
protected:
	enum
	{
		ID_X		= 1,
		ID_OK		= 2,
	};
	ioUIRenderImage *m_pItemIcon;
	ioUIRenderImage *m_pSubIcon;
	ioComplexStringPrinter		m_AttendNumberPrinter;
	ioComplexStringPrinter		m_ItemNamePrinter;
	
public:
	void SettingRewardInfo( BYTE btYesterdayAttendCount, short iPresentType, int iPresentValue1, int iPresentValue2 );
	
public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnRender();

public:
	GuildAttendRewardWnd();
	virtual ~GuildAttendRewardWnd();
};
//////////////////////////////////////////////////////////////
class GuildRankRewardWnd : public ioWnd
{
protected:
	enum
	{
		ID_X		= 1,
		ID_OK		= 2,
	};
	ioUIRenderImage *m_pItemIcon;
	ioUIRenderImage *m_pSubIcon;
	ioComplexStringPrinter		m_GuildRankPrinter;
	ioComplexStringPrinter		m_ItemNamePrinter;

public:
	void SettingRewardInfo( int iGuildLevel, short iPresentType, int iPresentValue1, int iPresentValue2 );

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnRender();

public:
	GuildRankRewardWnd();
	virtual ~GuildRankRewardWnd();
};
//////////////////////////////////////////////////////////////
class GuildEnableRewardToolTip : public ioWnd
{
protected:
	enum
	{
		MIN_WIDTH		= 0,
		MIN_HEIGHT		= 26,
		TEXT_LINE_GAP   = 18,
		SIDE_TEXT_GAP	= 14,
	};
	ioComplexStringPrinter m_szPresentName;
	ioHashString		   m_szRewardInfoOverSnd;

public:
	void		 SetInfo( const ioHashString szItemName );

public:
	virtual void ParseExtraInfo( ioXMLElement &xElement );

protected:
	virtual void OnRender();
	virtual void OnProcess( float fTimePerSec );

public:
	GuildEnableRewardToolTip();
	virtual ~GuildEnableRewardToolTip();
};