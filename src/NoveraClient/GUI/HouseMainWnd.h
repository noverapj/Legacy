#pragma once

#include "../io3DEngine/ioMovingWnd.h"

#include "ResultInfo.h"
#include "GlobalWnd.h"

class SP2Packet;
class ioPlayStage;
class ioUIRenderImage;

 struct HouseInfo
{
	ioHashString	m_szMasterName;	
    int				m_iMaxUserCount;
	

	HouseInfo()
	{
		m_iMaxUserCount	= 0;
	}
};

struct HouseUserInfo
{
	ioHashString	m_szUserID;
	
	int				m_iGradeLevel;
	int				m_iServerPingStep;
	bool			m_bMyFriend;
	
	DWORD			m_dwUserIndex;
	DWORD			m_dwGuildIndex;
	DWORD			m_dwGuildMark;

	HouseUserInfo()
	{
		m_iGradeLevel		= 0;
		m_iServerPingStep	= 0;
		m_bMyFriend			= false;
		
		m_dwUserIndex		= 0;
		m_dwGuildIndex		= 0;
		m_dwGuildMark		= 0;
	}
};

class HouseInfoWnd : public ioMovingWnd
{
public:
	enum 
	{
		ID_EXIT			= 1,
		ID_ENTER_HOUSE	= 2,
		ID_CANCEL		= 3,
	};	

	enum
	{
		MAX_HORZ = 4,
		MAX_VERT = 8,
	};

	typedef std::vector< HouseUserInfo > vUserInfo;

protected:
	ioUIRenderImage*	m_pBarNormal;
	ioUIRenderImage*	m_pBarNormalLight;
	ioUIRenderImage*	m_pBarOver;
	ioUIRenderImage*	m_pPeopleOrange;
	ioUIRenderImage*	m_pPeopleGray;
	ioUIRenderImage*	m_pModeIconBack;
	ioUIRenderImage*	m_pModeIcon;

	HouseInfo			m_HouseInfo;
	vUserInfo			m_vUserList;

	bool				m_bMyCurrentRoom;
	ioHashString		m_MouseOverName;
	bool				m_bMouseLBtnDown;


public:
	void SetInfo( SP2Packet &rkPacket );

protected:
	void ProcessSiteClick();
	void JoinHouseRoom();

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:	
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();
	void OnRenderBar( int iXPos, int iYPos, int iMaxSlot, int iCurSlot, const ioHashString szName, int iGradeLevel );

public:
	HouseInfoWnd();
	virtual ~HouseInfoWnd();
};
//////////////////////////////////////////////////////////////////////////
class HouseJoinRequestWnd : public ioMovingWnd
{
public:
	enum
	{
		ID_EXIT		= 1,
		ID_AGREE    = 2,
		ID_REFUSE   = 3,
	};

	enum
	{
		REQUEST_DELAY_SEC = 7,
	};

protected:
	ioUIRenderImage*	m_pIconBack;
	ioUIRenderImage*	m_pIcon;

protected:
	ioHashString		m_szRequestName;
	
protected:
	DWORD				m_dwCurrentTime;

protected:
	void OnAgree();
	void OnRefuse();

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual bool iwm_spacebar();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	void SetRequestName( const ioHashString &rkName );

public:
	HouseJoinRequestWnd();
	virtual ~HouseJoinRequestWnd();
};
//////////////////////////////////////////////////////////////////////////
class HouseMainOptionWnd : public ioWnd
{
public:
	enum
	{
		OPTION_INFO		   = 1000,
		OPTION_KICK		   = 1001,
		OPTION_CLOSE	   = 1002,
		OPTION_OPEN        = 1003,
	};

protected:
	ioUIRenderImage*	m_pDarkLine;
	ioUIRenderImage*	m_pLightLine;
	ioUIRenderImage*	m_pOverLine;

	int					m_iAddWidth;
	int					m_iAddHeight;
	int					m_iListXPos;
	int					m_iListYPos;
	int					m_iHeightGap;
	int					m_iStartXPos;
	int					m_iStartYPos;
	int					m_iOverListIndex;

protected:
	ioHashString		m_szName;
	ioHashString		m_szMasterName;
	int					m_iSlotArray;
	int					m_iMaxSlot;

	typedef std::map< DWORD, ioHashString > OptionMap;
	OptionMap m_OptionMap;

public:
	virtual void ParseExtraInfo( ioXMLElement &xElement );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

public:
	void SetOptionInfo( const ioHashString &rkName, const ioHashString &rkMasterName, int iSlotArray, int iMaxSlot );

protected:
	void CommandOption( DWORD dwCmd );

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	HouseMainOptionWnd();
	virtual ~HouseMainOptionWnd();
};

class HouseMainWnd : public ioMovingWnd
{
public:
	enum 
	{
		ID_EXIT			= 1,
		ID_INVITE_BTN   = 2,

		ID_OPTION_WND   = 1000,
	};	

	enum
	{
		MAX_HORZ = 4,
		MAX_VERT = 8,
	};

protected:
	ioUIRenderImage *m_pBarNormal;
	ioUIRenderImage *m_pBarNormalLight;
	ioUIRenderImage *m_pBarOver;
	ioUIRenderImage *m_pPeopleOrange;
	ioUIRenderImage *m_pPeopleGray;
	ioUIRenderImage *m_pModeIconBack;
	ioUIRenderImage *m_pModeIcon;

	HouseInfo m_HouseInfo;
	
	ioPlayStage *m_pPlayStage;

	ioHashString m_MouseOverName;
	int          m_MouseOverIndex;
	bool         m_bMouseLBtnDown;	
	
	typedef std::vector< RoundInfo > RoundInfoList;
	RoundInfoList m_UserInfoList;

	bool m_bMyCurrentRoom;

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual bool iwm_esc();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

public:
	virtual void SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate = false );

public:
	void SetPlayStage( ioPlayStage *pPlayStage ){ m_pPlayStage = pPlayStage; }
	void UpdateInfo();

protected:
	void ProcessSiteClick();
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();	
	void OnRenderBar( int iXPos, int iYPos, int iMaxSlot, int iCurSlot, const ioHashString szName, int iGradeLevel );

public:
	HouseMainWnd();
	virtual ~HouseMainWnd();
};
//////////////////////////////////////////////////////////////////////////
/*
class HouseOwnerWnd : public ioWnd
{

public:
	enum 
	{
		ID_EXIT		  = 1,
		ID_OPEN_BTN   = 2,
		ID_CLOSE_BTN  = 3,
		ID_CHANGE_BTN = 100,
		ID_CANCEL_BTN = 101,
	};	

protected:
	DWORD m_dwRadioBtnID;

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnRender();	

public:
	HouseOwnerWnd();
	virtual ~HouseOwnerWnd();
};
*/
//////////////////////////////////////////////////////////////////////////
class HouseInviteList : public ioButton
{
public:
	enum
	{
		ID_INVITE_BTN = 1,
	};

protected:
	ioUIRenderImage *m_pGuildMark;
	ioUIRenderImage *m_pFriendMark;

protected:
	int			 m_iGradeLevel;
	ioHashString m_szName;
	DWORD        m_dwGuildIndex;
	DWORD        m_dwGuildMark;
	bool         m_bInviteActive;

public:
	void InviteInfo( int iGradeLevel, ioHashString szName, DWORD dwGuildIndex, DWORD dwGuildMark, bool bActive );

public:
	int  GetLevel() const { return m_iGradeLevel; }
	const ioHashString &GetName() const { return m_szName; }
	DWORD GetGuildIndex() const { return m_dwGuildIndex; }
	DWORD GetGuildMark() const { return m_dwGuildMark; }

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();	

public:
	HouseInviteList();
	virtual ~HouseInviteList();
};

class HouseInviteListWnd : public ioMovingWnd
{
public:
	enum
	{
		ID_EXIT		   = 1,
		ID_LEFT_PAGE   = 2,
		ID_RIGHT_PAGE  = 3,
		ID_ALL_INVITE  = 4,
		ID_NAME_INVITE = 5,
		ID_NAME_EDIT   = 6,

		//USER LIST
		ID_USER_LIST_1  = 100,
		ID_USER_LIST_2  = 101,
		ID_USER_LIST_3  = 102,
		ID_USER_LIST_4  = 103,
		ID_USER_LIST_5  = 104,
		ID_USER_LIST_6  = 105,
		ID_USER_LIST_7  = 106,
		ID_USER_LIST_8  = 107,
		ID_USER_LIST_9  = 108,
		ID_USER_LIST_10 = 109,
	};

	enum
	{
		MAX_PAGE_LIST      = 10,
		INVITE_SEARCH_TIME = 15000,
	};

protected:
	ioPlayStage *m_pPlayStage;

protected:
	struct UserInviteInfo
	{
		ioHashString m_szName;
		int			 m_iLevel;

		UserInviteInfo()
		{
			m_iLevel = 0;
		}
	};
	typedef std::vector< UserInviteInfo > vUserInviteInfo;
	vUserInviteInfo m_vUserList;

	bool  m_bInviteActive;

	void AllInvite();
	void NameInvite();

	DWORD      m_dwSearchTime;
	int        m_iCurPage;
	int        m_iPrevCurPage;
	int        m_iMaxPage;

	//
	ioWnd *m_pPreEdit;

	void UpdateSetPage( int iCurPage );
	void StartSend( bool bProcess = false );
	void ProcessSearch();

public:
	void SetUserList( SP2Packet &rkPacket );
	void SetPlayStage( ioPlayStage *pPlayStage ){ m_pPlayStage = pPlayStage; }
	void StartInivteWait( int iValue, const ioHashString &szName );

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	HouseInviteListWnd();
	virtual ~HouseInviteListWnd();
};
//////////////////////////////////////////////////////////////////////////
class HouseInvitedBtn : public ioButton
{
protected:
	ioUIRenderImage *m_pPeopleOrange;
	ioUIRenderImage *m_pPeopleGray;

protected:
	ioHashString m_szName;
	int m_iGradeLevel;
	int m_iPingStep;
	int m_iSlotArray;
	int m_iMaxSlot;

protected:
	PingStepWnd m_PingStep;

public:
	virtual void iwm_create();

public:
	void SetMemberInfo( int iMaxSlot, int iSlotArray, const ioHashString &rkName, int iGradeLevel, int iPingStep );
	const ioHashString &GetMemberName(){ return m_szName; }

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void OnRender();
	virtual void OnProcess( float fTimePerSec );

public:
	HouseInvitedBtn();
	virtual ~HouseInvitedBtn();
};
//////////////////////////////////////////////////////////////////////////
class HouseInvitedWnd : public ioMovingWnd
{
public:
	enum 
	{
		ID_EXIT				= 1,
		ID_AGREE			= 2,
		ID_REFUSE			= 3,
		ID_AUTO_REFUSE		= 4,
		ID_LEFT_PAGE		= 5,
		ID_RIGHT_PAGE		= 6,

		ID_LIST_1			= 10,
		ID_LIST_2			= 11,
		ID_LIST_3			= 12,
		ID_LIST_4			= 13,
		ID_LIST_5			= 14,
		ID_LIST_6			= 15,
		ID_LIST_7			= 16,
		ID_LIST_8			= 17,
	};

	enum
	{
		INVITE_DELAY_SEC	= 20,
		MAX_PAGE_LIST		= 8,
	};

	typedef std::vector< HouseUserInfo > vUserInfo;

protected:
	static bool			sg_bAutoRefuse;

protected:
	ioUIRenderImage*	m_pModeIconBack;
	ioUIRenderImage*	m_pModeIcon;

protected:
	ioHashString		m_szMasterName;
	int					m_iMaxPlayer;
	
	vUserInfo			m_vUserList;

	DWORD				m_dwStartTime;
	int					m_iCurPage;
	int					m_iMaxPage;

public:
	void SetInfo( const ioHashString &rkMasterName, SP2Packet &rkPacket );

protected:
	void UpdatePage( int iCurPage );

protected:
	void JoinHouse();
	const HouseUserInfo &GetUserInfo( int iArray );

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void iwm_show();	
	virtual void iwm_hide();

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	HouseInvitedWnd();
	virtual ~HouseInvitedWnd();
};

