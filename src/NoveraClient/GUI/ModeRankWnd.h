#pragma once

#include "../io3DEngine/ioMovingWnd.h"

class ioPlayStage;
class ioUIRenderImage;
class ModeRankWnd : public ioMovingWnd
{
public:
	enum
	{
		NONE_TYPE = 0,
		DEATH_TYPE = 1,             // 죽은 후 나오는 UI
		RESULT_TYPE= 2,				// 결과시에 같이 나오는 UI
		TABKEY_TYPE= 3,             // 탭키 누른 후 나오는 UI
	};

	enum
	{
		S_RANK = 0,
		A_RANK = 1,
		B_RANK = 2,
		C_RANK = 3,
		MAX_RANK = 4,
		MAX_RANK_ELEMENT = 4,
	};

	enum 
	{
		POS_SAVE_VERSION = 4,
	};
protected:
	ioUIRenderImage *m_pStonePoint;
	ioUIRenderImage *m_pPrisonerPoint;
	ioUIRenderImage *m_pKingPoint;
	ioUIRenderImage *m_pKillPoint;
	ioUIRenderImage *m_pAttackPoint;
	ioUIRenderImage *m_pDamagePoint;
	ioUIRenderImage *m_pNpcKillPts;
	ioUIRenderImage *m_pNpcTowerPts;
	ioUIRenderImage *m_pTimePts;
	ioUIRenderImage *m_pFlagPoint;


	ioUIRenderImage *m_pGoalPoint;
	ioUIRenderImage *m_pBallTouchPoint;
	ioUIRenderImage *m_pBallTimePoint;

	ioUIRenderImage *m_pGangsiInfectionPoint;
	ioUIRenderImage *m_pGangsiSurvivorPoint;

	ioUIRenderImage *m_pRank[MAX_RANK];

	ioHashString m_RollingSound;
	ioHashString m_RollingStopSound;
	
	ioPlayStage *m_pPlayStage;
	ModeType m_eModeType;

	bool m_bPosSave;
	int  m_iRankType;
	bool m_bRollingEnd;
	int  m_iAlphaRate;
	struct RankData
	{
		int m_iRank;
		int m_iCurRank;
		DWORD m_dwRollingTime;
		DWORD m_dwRankUPTime;
		RankData()
		{
			m_iRank    = -1;
			m_iCurRank = -1;
			m_dwRollingTime = 0;
		}
	};
	RankData m_RankData[MAX_RANK_ELEMENT];

	DWORD m_dwCurTime;
	DWORD m_dwShowTime;
	DWORD m_dwHideTime;
	DWORD m_dwStartTime;
	DWORD m_dwGapTime;
	DWORD m_dwCurHideTime;

	int m_iDefaultX;
	int m_iDefaultY;
	void UIPositionSave( bool bDefault = false );

public:
	void SetPlayStage( ioPlayStage *pPlayStage ){ m_pPlayStage = pPlayStage; }
	void SetAlpha( int iAlphaRate ){ m_iAlphaRate = iAlphaRate; }
	void SetRank( int iRankType );
	int GetRankType(){ return m_iRankType; }

public:
	virtual void iwm_show();
	virtual void iwm_hide();

public:
	virtual void SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate = false );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

protected:
	void DeathTypeProcessShow();
	void TabTypeProcessHide();
	void ProcessRank();
	void SetPositionByRankType( int iRankType );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

	void RenderRank( ioUIRenderImage *pRankValue1,
					 ioUIRenderImage *pRankValue2,
					 ioUIRenderImage *pRankValue3,
					 ioUIRenderImage *pRankValue4 );

public:
	ModeRankWnd();
	virtual ~ModeRankWnd();
};

