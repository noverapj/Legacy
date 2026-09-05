#pragma once


#include "../io3DEngine/ioMovingWnd.h"
#include "FSMStateMgr.h"

class ioINILoader;
class ioXMLElement;
class ioAxisAlignBox;
class ioUIRenderImage;
class ioPlayMode;

class MiniMap : public ioMovingWnd
{
public:
	enum SpotType
	{
		SPOT_USER = 0,   // blue
		SPOT_USER_RED,
		SPOT_DROP_USER,		//blue
		SPOT_DROP_USER_RED,
		SPOT_ME,			// aroud outside
		SPOT_SYMBOL,		// blue
		SPOT_SYMBOL_RED,
		SPOT_SYMBOL_NEUTRAL,	// neutral
		SPOT_PRISON_USER,		
		SPOT_PRISON_USER_RED,
		SPOT_PRISON_ME,

		SPOT_KING_USER,		//blue
		SPOT_KING_USER_RED, //red
		SPOT_CROWN,			// neutral
		SPOT_KING_ME,		
		
		SPOT_DROP_ME,		// equal SPOT_ME 
		SPOT_DEATH_MON,	
		SPOT_BOSS_MON,
		SPOT_DROP_BOSS_MON,
		SPOT_BALL,

		SPOT_NEXUS, 
		SPOT_NEXUS_RED,
		SPOT_LAIR,
		SPOT_LAIR_RED,
		SPOT_TOWER,
		SPOT_TOWER_RED,

		SPOT_NPC,
		SPOT_NPC_RED,

		SPOT_ROULETTE,

		SPOT_DOUBLECROWN_DROP_RED,
		SPOT_DOUBLECROWN_DROP_BLUE,
		SPOT_STAR_ON,
		SPOT_STAR,
		SPOT_SHUFFFLE_MONSTER,
		SPOT_SHUFFLE_BUFFBOX,
		
		SPOT_CHECK_POINT,
		SPOT_GOAL,
		SPOT_ITEM,
		SPOT_BOMB,
		SPOT_BEAR,
		SPOT_SUMMONCIRCLE,

		SPOT_FLAG_NEUTRAL,
		SPOT_FLAG,
		SPOT_FLAG_RED,
		SPOT_FLAG_ME,
		MAX_SPOT,
	};

	enum ShowAniType 
	{
		SAT_NONE         = 0,
		SAT_START_ROUND  = 1,
		SAT_DELAY        = 2,
		SAT_UP           = 3,  // 가속
		SAT_DOWN         = 4,  // 감속
		SAT_UP_STOP      = 5,  // 가속
		SAT_STOP         = 6,
		SAT_END          = 7,
	};

	enum EventType
	{
		EVENT_START_MY_POSITION,

		EVENT_SYMBOL_DESTROY,
		EVENT_SYMBOL_NEXT_GUIDE,
		EVENT_SYMBOL_DAMAGED_HALF,

		EVENT_CROWN_START_POSITION,
		EVENT_CROWN_ACQUIRE,
		EVENT_CROWN_DROP,
		EVENT_CROWN_POINT_HALF,

		EVENT_CATCH_TEAM_CATCHED,
		EVENT_CATCH_TEAM_DEATH,
		EVENT_CATCH_ENEMY_ESCAPE,
	};

protected:
	enum 
	{
		POS_SAVE_VERSION = 4,
	};

	enum
	{
		TIME_ICON = 0,
		MAP_TITLE,
		MAP_DIFFICULTY,
		TIME_NUMBER,
		TIME_COLON,
		TIME_DOT,
		MAX_IMAGE,
	};

protected:
	struct SpotPoint
	{
		SpotType m_Type;
		int m_iXOffset;
		int m_iYOffset;
		TeamType m_eTeamType;
		bool m_bRender;
	};

	typedef std::vector< SpotPoint > SpotPointList;
	SpotPointList m_SpotPointList;

	ioPlayMode *m_pModeParent;

	float m_fMapStartX;
	float m_fMapStartZ;
	float m_fMapRealWidth;
	float m_fMapRealHeight;
	
	int m_iMiniMapOffX;
	int m_iMiniMapOffY;
	int m_iPointOffX;
	int m_iPointOffY;

	int m_iBlindOffSetY;
	int m_iPreBlindOffSetY;

	ioUIRenderImage *m_pMiniMap;
	ioUIRenderImage *m_pAreaMe;
	ioUIRenderImage *m_pSpot[MAX_SPOT];
	ioUIRenderImage *m_pExtraImg[MAX_IMAGE];

	FSMStateMgr m_ShowAniState;

	int m_iDefaultX;
	int m_iDefaultY;

	DWORD m_dwSpotDuration;
	DWORD m_dwSpotVisibleGap;
	DWORD m_dwSpotHideGap;

	DWORD m_dwTime;

	float m_fSpotPositionRate;

	bool m_bShowTime;
	POINT m_ptOffSet[MAX_IMAGE];

public:
	void InitMapInfo( ioINILoader &rkLoader,
					  const ioAxisAlignBox &rkWorldBox,
					  ioPlayMode *pModeParent );

	void SetBlindOffSet( int iYOffSet ) { m_iBlindOffSetY = iYOffSet; }
	void SetDefaultY(int iDefaultY) { m_iDefaultY = iDefaultY; }
	void SetTimeImage(ioINILoader &rkLoader);
	void SetShowTime(bool bShow) {m_bShowTime = bShow;}

public:
	void ClearPointList();
	void AddPoint( float fXPos, float fZPos, SpotType eSpot, TeamType eTeam, DWORD dwGapTime );
	void AddEvent( float fXPos, float fZPos, EventType eEvent, TeamType eTeam );

	void AddNextSymbolEvent( const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vDir, TeamType eTeam );
	void UpdateTime(DWORD dwTime) {m_dwTime = dwTime;}

public:
	void ChangeShowAniState( ShowAniType eShowAniType );
	int  GetCurShowAniState();

	float GetSpotPositionRate() const { return m_fSpotPositionRate; }

protected:
	void InitShowAniState();
	void UpdateAniState();
	bool UpdateShowAni();
	void UpdateDefaultPos();

	bool CheckEnableRenderPoint( DWORD dwGapTime );

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate = false );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

public:
	virtual void iwm_create();

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

protected:
	void RenderSpotPoint( int iXPos, int iYPos, const SpotPoint &rkPoint );
	void RenderDigitTime( int nPosX, int nPosY );
	int GetPointOffsetX( float fXPos );
	int GetPointOffsetY( float fZPos );
	TeamType GetOwnerTeam();

public:
	MiniMap();
	virtual ~MiniMap();
};

