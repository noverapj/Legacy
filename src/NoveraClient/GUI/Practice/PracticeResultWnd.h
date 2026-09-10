#pragma once

#include "../ToolTip/ToolTipManager.h"

class PracticeResultWnd : public ioWnd
{
protected:
	enum
	{
		ID_RETRY	= 1,
		ID_OUT		= 2,
		ID_EXIT		= 11,
	};

	enum
	{
		INVENTORY_ITEM_EXTRAITEM	= 1,
		INVENTORY_ITEM_DECO			= 2,
		INVENTORY_ITEM_ETC			= 3,
		INVENTORY_ITEM_PRESENT		= 4,
		INVENTORY_ITEM_MEDALITEM    = 5,
		INVENTORY_ITEM_SUBSCRIPTION	= 6,
		INVENTORY_ITEM_COSTUME		= 7,
		POPUPSTORE_ITEM_PRESENT		= 8,
		INVENTORY_ITEM_ACCESSORY	= 9,
	};

protected:
	typedef std::map< ioHashString, ioUIRenderImage * >PracticeIconMap;
	PracticeIconMap			m_RewardIconMap;

	ioUIRenderImage*	m_pClear;
	ioUIRenderImage*	m_pFail;
	ioUIRenderImage*	m_pStamp;
	ioUIRenderImage*	m_pItemIconBg;
	ioUIRenderImage*	m_pRewardIcon;
	ioUIRenderImage*	m_pBackGround;
	ioUIRenderImage*	m_pStarYellow;
	ioUIRenderImage*	m_pStarGray;

	IoString			m_RewardString;

	ioHashString			m_szRewardHelp[3];
	ioHashString			m_szRewardName[3];

	ioHashString		m_szFailReward;
	ioHashString		m_szGetReward;
	ioHashString		m_szAlreadyReward;

	ioHashString		m_szSuccessSound;
	ioHashString		m_szFailSound;
	ioHashString		m_szStarSound;

	ioHashString		m_szRetryBtn;
	ioHashString		m_szExitBtn;

	int m_iResultStar;
	ioHashString m_szRewardCode[3];

	DWORD m_dwStarTime;
	DWORD m_dwDelay;
	DWORD m_dwSpeed;
	int m_iButtonYpos;
	int m_iYPosGap;
	int m_iSoundCount;

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void ParseExtraInfo( ioXMLElement &xElement );
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command(ioWnd *pWnd, int cmd, DWORD param);
	virtual bool iwm_spacebar();
	virtual bool iwm_esc();

protected:
	ioUIRenderImage* GetImage( const ioHashString &szIconName );
	float GetStarScale( float dwTime );
	void RenderReward( int iPosX, int iPosY );
	void ClearIconMap();
	void SetRewardIconName();
	void StopSound();

public:
	void SetResult( int iPracticeIndex, int iResultStar, DWORDVec &vRewardList );


public:
	PracticeResultWnd();
	virtual ~PracticeResultWnd();
};

class PracticeUpdateRank : public ioWnd
{
	enum
	{
		ID_EXIT = 1,

		OFFSET_X = 167,
		OFFSET_Y = 160,
	};
protected:
	ioUIRenderImage*	m_pBackGround;
	ioUIRenderImage*	m_pNum_Blue;
	ioUIRenderImage*	m_pScore;

	ioUI3DEffectRender*	m_pResultEffect;

	ioHashString		m_szExitBtn;
	ioHashString		m_szNewRecord;
	ioHashString		m_szUpdate;
	ioHashString		m_szFromTo[3];

	int m_iEffectPosX;
	int m_iEffectPosY;

	int m_iCurrentRank;
	int m_iUpdateRank;

	DWORD m_dwStarTime;

protected:
	void RenderRank( int iPosX, int iPosY );
	void RenderText( int iPosX, int iPosY );
	void SetResult();

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

	virtual void OnRender();
	virtual void OnProcess( float fTimePerSec );
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command(ioWnd *pWnd, int cmd, DWORD param);

public:
	void SetUpdateRank(int iRank, int iUpdateRank);


public:
	PracticeUpdateRank();
	virtual ~PracticeUpdateRank();
};