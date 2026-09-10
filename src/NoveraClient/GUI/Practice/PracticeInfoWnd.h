#pragma once

#include "../FSMStateMgr.h"

class PracticeInfoWnd : public ioWnd
{
public:
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

protected:
	FSMStateMgr m_ShowAniState;
	int m_iX;
	int m_iY;

	ioUIRenderImage* m_pStarLine;
	ioUIRenderImage* m_pStarYellow;
	ioUIRenderImage* m_pStarGray;
	ioUIRenderImage* m_pTimeSec;
	ioUIRenderImage* m_pTimeSmall;
	ioUIRenderImage* m_pTimeClear;
	ioUIRenderImage* m_pTimeEa;
	ioUIRenderImage* m_pTimeClear2;
	int m_iStar;
	int m_iGradeA;
	int m_iGradeB;
	int m_iGradeC;

	ioHashString m_szRanker[3];
	ioHashString m_szTime[3];
	ioHashString m_szRank;
	ioHashString m_szName;

	IoString m_Star3;
	IoString m_Star2;
	IoString m_Star1;

	ioHashString m_szNoRecordSound;

	int m_iRenderTextType;

public:
	void SetPracticeInfo( int iPracticeIndex );
	void SetStar( int iStar );
	void RenderStar(int iXPos, int iYPos);
	void RenderSecond(int iXPos, int iYPos);
	void RenderRanker(int iXPos, int iYPos);

protected:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	PracticeInfoWnd();
	virtual ~PracticeInfoWnd();
};
