#pragma once

#include "../io3DEngine/ioStringConverter.h"

class ioPlayStage;
class UserInfoNode;
class ioUIRenderImage;

#define USERINFO_TOOLTIP_GROWTH_SCALE        0.50f
#define USERINFO_TOOLTIP_NONE_ITEM_SCALE     0.58f
#define USERINFO_TOOLTIP_MEDAL_SCALE         0.65f
#define USERINFO_TOOLTIP_MEDAL_EMPTY_SCALE   0.75f
#define USERINFO_TOOLTIP_MEDAL_DISABLE_SCALE 0.68f

#define USERINFO_TOOLTIP_COSTUME_SCALE         0.65f
#define USERINFO_TOOLTIP_COSTUME_EMPTY_SCALE   0.75f
#define USERINFO_TOOLTIP_COSTUME_DISABLE_SCALE 0.68f

#define USERINFO_TOOLTIP_ACCESSORY_SCALE         0.65f
#define USERINFO_TOOLTIP_ACCESSORY_EMPTY_SCALE   0.75f
#define USERINFO_TOOLTIP_ACCESSORY_DISABLE_SCALE 0.68f

#define USERINFO_TOOLTIP_WIDTHCUT            140.0f

class UserInfoToolTip : public ioWnd
{
public:
	// type
	enum 
	{
		TT_ROOM   = -2,
		TT_GLOBAL = -1,
		// TT_CHAR ~ ClassType
	};

	// wnd id
	enum 
	{
		ID_GROWTH_1_BTN = 1,
		ID_GROWTH_2_BTN = 2,
		ID_GROWTH_3_BTN = 3,
		ID_GROWTH_4_BTN = 4,
		ID_GROWTH_5_BTN = 5,
		ID_GROWTH_6_BTN = 6,
		ID_GROWTH_7_BTN = 7,
		ID_GROWTH_8_BTN = 8,

		ID_MADAL_1_BTN  = 101,
		ID_MADAL_2_BTN  = 102,
		ID_MADAL_3_BTN  = 103,
		ID_MADAL_4_BTN  = 104,
		ID_MADAL_5_BTN  = 105,
		ID_MADAL_6_BTN  = 106,

		ID_COSTUME_1_BTN  = 201,
		ID_COSTUME_2_BTN  = 202,
		ID_COSTUME_3_BTN  = 203,
		ID_COSTUME_4_BTN  = 204,

		ID_ACCESSORY_1_BTN  = 301,
		ID_ACCESSORY_2_BTN  = 302,
		ID_ACCESSORY_3_BTN  = 303,
	};

	// etc
	enum 
	{
		MAX_GROWTH = 8,
		MAX_MEDAL  = 6,
		MAX_COSTUME = 4,

		HEIGHT_MEDAL3 = 371,
		HEIGHT_MEDAL6 = 422,

		MAX_INFO_LODING = 8,

		//초월각성, 진화용병시 추가되는 높이
		EXTRA_ADD_HEIGHT = 16,
	};
	enum
	{
		MAX_INFO_BOX    = 8,
	};
public:
	struct UserInfo
	{
		DWORD m_dwCreateTime;
		DWORD m_dwRefreshTime;
		ioHashString m_sName;	

		int m_iClassType;
		int m_iClassLevel;
		int m_iPowerUpCharGrade;

		//각성타입
		int m_iAwakeType;

		IntVec  m_vGrowthLevelVec;  
		MedalSyncInfoVec  m_vMedalItemTypeVec;
		IntVec	m_vExMedalSlotVec;
		DWORDVec  m_vItemCodeVec;
		IntVec  m_vCostumeCodeVec;
		IntVec  m_vAccessoryCodeVec;
		
		//강제 동기화
		bool m_bMustSync;

		UserInfo()
		{
			m_dwCreateTime		= 0;
			m_dwRefreshTime		= 0;
			m_iClassType		= 0;
			m_iClassLevel		= 0;
			m_iAwakeType		= 0;
			m_iPowerUpCharGrade	= 0;
			m_bMustSync			= false;
		}
	};

	typedef std::vector< UserInfo * > vUserInfo;

protected:	
	ioPlayStage *m_pPlayStage;

	ioHashString m_sName;
	ioHashString m_sClassInfo;
	ioHashString m_sExtraInfo;//진화 및 각성시 텍스트 출력

	ioHashStringVec m_vGrowthCharIconVec;
	DWORD m_dwMaxRefreshDelayTime;
	DWORD m_dwMaxLiveTime;

	static DWORD m_dwMaxUserInfo;
	static vUserInfo m_vUserInfo;

	ioUIRenderImage *m_pInfoBox[MAX_INFO_BOX];
	ioUIRenderImage *m_pInfoLoading[MAX_INFO_LODING];
	DWORD            m_dwInfoLoadTime;
	int              m_iInfoLoadArray;
	DWORD            m_dwRefreshDelayTime;

	bool             m_bCreate;
	
	ioUIRenderFrame *m_pNoneTitleFrm;
	ioUIRenderFrame *m_pPowerUpNAwakeTitleFrm;
	bool			m_bExtraInfo; //초월각성, 진화용병시 true

	DWORD	m_dwPackSendTime;

protected:
	void SetUserInfoToolInfo( UserInfo &rkInfo );
	bool GetUserInfoMy( IN const ioHashString &sName, IN int iType, OUT UserInfo &rkUserInfo );
	bool GetUserInfoRoom( IN const ioHashString &sName, IN int iType, OUT UserInfo &rkUserInfo );
	bool GetUserInfoNetwork( IN const ioHashString &rsName, OUT UserInfo &rkInfo );

	void ProcessLoading();
	
public:
	void SetPlayStage( ioPlayStage *pStage );
	void SetRefreshDelayTime( DWORD dwRefreshDelayTime ) { m_dwRefreshDelayTime = dwRefreshDelayTime; }

	static void InsertUserInfo( IN UserInfo &rkInfo );	
	static void Clear();

public:
	void RefreshUserInfo( const ioHashString& szName );

public:
	virtual void ParseExtraInfo( ioXMLElement &xElement );

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );

public:
	virtual void iwm_show();

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();
	virtual void OnRenderAfterChild();

public:
	UserInfoToolTip();
	virtual ~UserInfoToolTip();
};