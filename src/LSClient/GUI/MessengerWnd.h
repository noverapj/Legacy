#pragma once

//////////////////////////////////////////////////////////////////////////
class MessengerPopup : public ioWnd
{
public:
	enum
	{
		ID_COMMAND  = 1,
	};

	enum
	{
		FRIEND_POPUP		= 1,
		MEMO_POPUP			= 2,
		CHANNEL_POPUP		= 3,
		GUILD_POPUP			= 4,
		BEST_FRIEND_POPUP	= 5,
	};

protected:
	ioUIRenderImage *m_pTooltipLeft;
	ioUIRenderImage *m_pTooltipLeftIn;
	ioUIRenderImage *m_pTooltipCenter;
	ioUIRenderImage *m_pTooltipRightIn;
	ioUIRenderImage *m_pTooltipRight;

protected:
	int			 m_iPopupType;
	ioHashString m_szID;
	ioHashString m_szFirstContents;
	ioHashString m_szLastContents;
	float        m_fTextWidth;
	int          m_iChannelID;

	DWORD        m_dwShowTime;

public:
	void ShowPopup( int iType, ioHashString szID, ioHashString szContents, int iChannelID = -1 );

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	

protected:
	void OnRenderTooltip( int iXPos, int iYPos );

public:
	MessengerPopup();
	virtual ~MessengerPopup();
};
//////////////////////////////////////////////////////////////////////////
class MessengerListBtn : public ioRadioButton
{
public:	
	virtual void SetActive();
	virtual void SetInActive();

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	MessengerListBtn();
	virtual ~MessengerListBtn();
};

class MessengerWnd : public ioMovingWnd
{
public:
	enum
	{
		//BTN
		ID_EXIT				= 1,
		ID_FRIEND_HELP		= 2,
		ID_BESTFRIEND_HELP  = 3,
		ID_CLOVER_HELP      = 4,
		ID_USER_LIST		= 5,
		ID_MEMO_LIST		= 6,
		ID_CHANNEL_1		= 7,
		ID_CHANNEL_2		= 8,
		ID_GUILD			= 9,

		//WND
		ID_USER_LIST_WND		= 100,
		ID_MEMO_LIST_WND		= 101,
		ID_CHANNEL_CHAT_WND		= 102,
		ID_GUILD_CHAT_WND       = 103,
	};

	enum
	{
		CMD_MEMO   = 1,
		CMD_INVITE = 2,
	};

	static int sg_iTabID;

protected:
	int	m_iFriendHelpID;
	int m_iBestFriendHelpID;
	int m_iCloverHelpID;

protected:
	ioUIRenderImage *m_pCloverInfoBox;
	ioUIRenderImage *m_pCloverAniIconGold;
	ioUIRenderImage *m_pCloverAniIconGreen;
	ioUIRenderImage *m_pCloverIconGold;
	ioUIRenderImage *m_pCloverIconGreen1;
	ioUIRenderImage *m_pCloverIconGreen2;
	ioUIRenderImage *m_pCloverIconGreen3;
	ioUIRenderImage *m_pCloverIconGreen4;
	ioUIRenderImage *m_pCloverIconGreen5;
	ioUIRenderImage *m_pCloverIconEmpty1;
	ioUIRenderImage *m_pCloverIconEmpty2;
	ioUIRenderImage *m_pCloverIconEmpty3;
	ioUIRenderImage *m_pCloverIconEmpty4;
	ioUIRenderImage *m_pCloverIconEmpty5;

	D3DXVECTOR2 m_CloverGoldPosition;
	D3DXVECTOR2 m_CloverGreenPosition;

	struct CloverAniInfo
	{
		enum{ CLOVER_MOVING, CLOVER_MOVE_END_ANI, CLOVER_MOVE_END };
		int         m_iIndex;
		D3DXVECTOR2 m_vPos;
		D3DXVECTOR2 m_vCurPos;
		float       m_fDistance;
		float       m_fCurDistance;
		int         m_iAlpha;
		float       m_fEndDuration;
		float       m_fScale;
		int         m_State;
		CloverAniInfo()
		{
			m_iIndex    = 0;
			m_vPos      = D3DXVECTOR2( 0.0f, 0.0f );
			m_vCurPos   = D3DXVECTOR2( 0.0f, 0.0f );
			m_fDistance = 0.0f;
			m_fCurDistance = 0.0f;
			m_iAlpha = MAX_ALPHA_RATE;
			m_fEndDuration = 0.0f;
			m_fScale = FLOAT1;
			m_State  = CLOVER_MOVE_END;
		}
	};
	typedef std::vector<CloverAniInfo> CloverAniInfoList;
	CloverAniInfoList m_MyCloverAniList;
	CloverAniInfoList m_GiftCloverAniList;

	int m_iSendCloverMovingAniTime;
	int m_iRecvCloverMovingAniTime;
	int m_iSendCloverEndAniTime;
	int m_iRecvCloverEndAniTime;

protected:
	void CheckButton( DWORD dwID );

public:
	void InviteResult( int iResult, ioHashString szInvitedName );
	void ChannelCreate( int iResult, ioHashString szManToManName );
	void Init();
	bool IsChatBlock();
	void MoveChannel( int iIndex );
	void MoveMemo( const char *szID );	
	void MoveGuild();
	bool IsViewChannel( int iIndex );
	bool IsViewGuild();
	void SetPlayStage( ioPlayStage *pPlayStage );
	void ChangeChannelIndex( int iPrevIndex, int iChangeIndex );
	bool QuickChannelInvite( ioHashString szInviteName );

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

public:
	virtual void iwm_wheel( int zDelta );
	virtual void iwm_lbuttondown( const ioMouse& mouse );

public:
	virtual void ParseExtraInfo( ioXMLElement &xElement );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();
	virtual void OnRenderAfterChild();

protected:
	int RenderGiftClover( int iXGap );

public:
	void SetMyCloverAni( int iIndex, int iXPos, int iYPos );
	void RenderMyCloverAni( int iXPos, int iYPos );
	void SetGiftCloverAni( int iIndex, int iXPos, int iYPos );
	void RenderGiftCloverAni( int iXPos, int iYPos );

public:
	MessengerWnd();
	virtual ~MessengerWnd();
};
//////////////////////////////////////////////////////////////////////////
class ChannelInviteWnd : public ioWnd
{
public:
	enum
	{
		ID_EXIT = 1,
		ID_INVITE = 2,
		ID_CANCEL = 3,

		ID_EDIT = 100,
	};

protected:
	ioWnd *m_pPreEdit;
	int    m_iChannelIdx;

protected:
	void OnChannelInvite();

public:
	void SetChannelIndex( int iIndex );

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnRender();

public:
	ChannelInviteWnd();
	virtual ~ChannelInviteWnd();
};
//////////////////////////////////////////////////////////////////////////
class BlackListInsertWnd : public ioWnd
{
public:
	enum
	{
		ID_EXIT = 1,
		ID_INSERT = 2,
		ID_CANCEL = 3,

		ID_EDIT = 100,
	};

protected:
	ioWnd *m_pPreEdit;

protected:
	void OnBlackListInsert();

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnRender();

public:
	BlackListInsertWnd();
	virtual ~BlackListInsertWnd();
};
//////////////////////////////////////////////////////////////////////////
class FriendDeleteWnd : public ioWnd
{
public:
	enum
	{
		ID_EXIT = 1,
		ID_DELETE = 2,
		ID_CANCEL = 3,

		ID_EDIT = 100,
	};

protected:
	ioWnd *m_pPreEdit;

protected:
	void OnDeleteFriend();

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnRender();

public:
	FriendDeleteWnd();
	virtual ~FriendDeleteWnd();
};
//////////////////////////////////////////////////////////////////////////
class FriendApplicationWnd : public ioWnd
{
public:
	enum
	{
		ID_EXIT = 1,
		ID_APPLICATION = 2,
		ID_CANCEL = 3,

		ID_EDIT = 100,
	};

protected:
	ioWnd *m_pPreEdit;

protected:
	void OnApplicationFriend();

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnRender();

public:
	FriendApplicationWnd();
	virtual ~FriendApplicationWnd();
};
//////////////////////////////////////////////////////////////////////////
class FriendRequestWnd : public ioMovingWnd
{
public:
	enum
	{
		ID_EXIT		= 1,
		ID_AGREE    = 2,
		ID_REFUSE   = 3,
		ID_BLACKLIST= 4,
		ID_REQUEST_IGNORE = 5,
		ID_FRIEND_INFO = 6,
	};

protected:
	ioUIRenderImage *m_pIconBack;
	ioUIRenderImage *m_pFriendIcon;
	ioUIRenderImage *m_pSearchIcon;
	ioHashString m_szRequestText;

protected:
	DWORD m_dwHideDelayTime;

protected:
	static bool sg_bRequestIgnore;
	static bool sg_bFirstView;

protected:
	void OnAgree();
	void OnRefuse();
    void OnBlackList();

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void ParseExtraInfo( ioXMLElement &xElement );
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();
	virtual void OnRenderAfterChild();

public:
	static void SetFirstView( bool bFirstView ){ sg_bFirstView = bFirstView; }

public:
	FriendRequestWnd();
	virtual ~FriendRequestWnd();
};
//////////////////////////////////////////////////////////////////////////
class CharRentalRequestWnd : public ioMovingWnd
{
public:
	enum
	{
		ID_EXIT		= 1,
		ID_AGREE	= 2,
		ID_REFUSE	= 3,
	};

	enum
	{
		REQUEST_DELAY_SEC = 20,
	};

protected:
	ioUIRenderImage *m_pIconBack;
	ioUIRenderImage *m_pCharIcon;
	ioUIRenderImage *m_pCharSubIcon;
	int m_nGradeType;

protected:
	static ioHashString sg_szUserID;
	static int sg_ClassType;
	static int sg_PowerUpChaGrade;
	static DWORD sg_RequestTime;

protected:
	void OnAgree();

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	static void SetRentalRequest( const ioHashString szUserID, const int iClassType, const int iPowerUpChaGrade );

public:
	CharRentalRequestWnd();
	virtual ~CharRentalRequestWnd();
};
//////////////////////////////////////////////////////////////////////////
class CharRentalReturnWnd : public ioMovingWnd
{
public:
	enum
	{
		ID_EXIT		= 1,
		ID_CLOSE	= 2,
	};

protected:
	ioUIRenderImage *m_pIconBack;
	ioUIRenderImage *m_pCharIcon;
	ioUIRenderImage *m_pCharSubIcon;
	int m_nGradeType;

protected:
	static int sg_ClassType;
	static int sg_PowerUpChaGrade;

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	static void SetRentalReturn( const int iClassType, int iPowerUpChaGrade );

public:
	CharRentalReturnWnd();
	virtual ~CharRentalReturnWnd();
};
//////////////////////////////////////////////////////////////////////////
class CharRentalAcquireWnd : public ioMovingWnd
{
public:
	enum
	{
		ID_EXIT		= 1,
		ID_CLOSE	= 2,
	};

protected:
	ioUIRenderImage *m_pIconBack;
	ioUIRenderImage *m_pCharIcon;
	ioUIRenderImage *m_pCharSubIcon;
	int m_nGradeType;

protected:
	ioHashString m_szOwnerName;
	int m_iClassType;

public:
	virtual void iwm_show();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void OnRender();

public:
	void SetRentalAcquire( const ioHashString &rkOwnerName, int iClassType );

public:
	CharRentalAcquireWnd();
	virtual ~CharRentalAcquireWnd();
};
