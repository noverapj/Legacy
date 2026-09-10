#pragma once
#include "../io3DEngine/ioMovingWnd.h"

#include "../ioPlayMode.h"
#include "../CharacterInfo.h"

#include "FSMStateMgr.h"
#include "ResultInfo.h"

class SP2Packet;
class ioUI3DRender;
class ioCreateChar;
class ioXMLElement;
class ioPlayStage;
class ioUIRenderImage;
class ioUIRenderFrame;

//////////////////////////////////////////////////////////////////////////
class TotalResultExpCharSlotWnd : public ioWnd
{
protected:
	ioUI3DRender *m_pUI3DRender;
	ioCreateChar *m_aMyChar;

protected:
	ioUIRenderImage *m_pCharBottom;
	ioUIRenderImage *m_pGaugeBar;
	ioUIRenderImage *m_pGaugeOrange;
	ioUIRenderImage *m_pGaugeYellow;
	ioUIRenderImage *m_pLevelUP;
	ioUIRenderImage *m_pExpBonus;

protected:
	int m_iClassType;
	int m_iClassPoint;

protected:
	int   m_iCharOffSetX;
	int   m_iCharOffSetY;
	float m_fCharXPos;
	float m_fCharYPos;
	float m_fCharZPos;

	int   m_iTitleX;
	int   m_iTitleY;

	DWORD m_dwGaugeTime;
	DWORD m_dwCurrentTime;
	float m_fCurrentPer;

protected:
	ioHashString m_szCharGaugeSound;
	ioHashString m_szCharEffectSound;
	ioHashString m_szAwakeEffect;

protected:
	bool  m_bSoldierExpBonus;

protected:
	enum LevelUpAni
	{
		LUA_NONE = 0,
		LUA_START,
		LUA_FADEIN,	
		LUA_DELAY_IN,	
		LUA_FADEOUT,
		LUA_DELAY_OUT,
	};
	static FSMStateMgr sg_LevelUpAni;
	static float sg_fLevelUpAlpha;

protected:
	void InitLevelUpAni();

	void CreateCharacter( const CHARACTER &rkInfo, const ITEM_DATA* pItem, int iAniCode, const ioHashString &szAniName );

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_hide();


protected:
	void OnProcessLevelUpAni();
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();
	void OnRenderTitle( int iXPos, int iYPos );

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void ParseExtraInfo( ioXMLElement &xElement );

public:
	void SetFinalRoundPoint( int iClassType, int iClassPoint, int iAniCode = 0 );
	void StartGauge( DWORD dwGaugeTime );

public:
	TotalResultExpCharSlotWnd();
	virtual ~TotalResultExpCharSlotWnd();
};
//////////////////////////////////////////////////////////////////////////
class TotalResultExpGradeWnd : public ioWnd
{
public:
	enum
	{
		END_GAUGE_ANI = 1000,
	};

protected:
	ioUIRenderImage *m_pBackEffect;
	ioUIRenderImage *m_pGaugeBar;
	ioUIRenderImage *m_pGaugeOrange;
	ioUIRenderImage *m_pGaugeYellow;
	ioUIRenderImage *m_pGradeLevelUP;

	//
	ioUIRenderImage *m_pGradeIcon;

protected:
	int  m_iResultExp;
	bool m_bLevelUP;

protected:
	DWORD m_dwGaugeTime;
	DWORD m_dwCurrentTime;
	float m_fCurrentPer;

protected:
	enum GradeUpAni
	{
		GUA_NONE = 0,
		GUA_START,
		GUA_FADEIN,	
		GUA_DELAY_IN,	
		GUA_FADEOUT,
		GUA_DELAY_OUT,	
	};
	FSMStateMgr m_GradeUpAni;
	float m_fGradeUpAlpha;

protected:
	int m_iGradeIconPage;
	ioHashString m_szGradeIconMainName;
	ioHashString m_szGradeIconSubName;
	ioHashString m_szGradeGaugeSound;

protected:
	void InitGradeUpAni();

protected:
	void OnProcessGradeUpAni();
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();	

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void ParseExtraInfo( ioXMLElement &xElement );

public:
	void SetFinalRoundPoint( int iResultExp, bool bLevelUP );
	void StartGauge( DWORD dwGaugeTime );

public:
	TotalResultExpGradeWnd();
	virtual ~TotalResultExpGradeWnd();
};
//////////////////////////////////////////////////////////////////////////
class TotalResultExpWnd : public ioWnd
{
public:
	enum
	{
		ID_CHAR_1 = 1,
		ID_CHAR_2 = 2,
		ID_CHAR_3 = 3,
		ID_CHAR_4 = 4,
		ID_CHAR_5 = 5,
		ID_GRADE  = 6,
	};

protected:
	ioUIRenderImage *m_pBackEffect;

protected:
	DWORD m_dwCharShowTime;
	DWORD m_dwGaugeTime;
	DWORD m_dwCurrentTime;
	DWORD m_dwShowID;

protected:
	ioHashString m_szBackSound;

protected:
	void StartGauge();

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	void SetFinalRoundPoint( int iGradeExp, bool bLevelUP, IntVec &rvClassTypeList, IntVec &rvClassPointList );

public:
	TotalResultExpWnd();
	virtual ~TotalResultExpWnd();
};
//////////////////////////////////////////////////////////////////////////
class TotalResultRawardWnd : public ioWnd
{
protected:
	enum
	{
		BONUS_SOLDIER			= 1,
		BONUS_GUILD				= 2,
		BONUS_PCROOM			= 3,
		BONUS_MODE				= 4,
		BONUS_MANNER			= 5,
		BONUS_FRIEND			= 6,
		BONUS_EVENT				= 7,
		BONUS_PESO_EVENT		= 8,
		BONUS_ETC_ITEM			= 9,
		BONUS_CAMP_BATTLE		= 10,
		BONUS_AWARD				= 11,
		BONUS_ETC_ITEM_EXP_PESO	= 12,
		BONUS_VICTORIES_PESO	= 13,
		BONUS_EVENT_PESO_EXP	= 14,
		BONUS_LEVELUP           = 15,
		BONUS_CONTRIBUTE        = 16,
		BONUS_HERO_TITLE_PESO   = 17,
		BONUS_MODE_CONSECUTIVELY= 18,
	};

	enum
	{
		MAX_BONUS = 9,
		MAX_BONUS_BY_SHUFFLE = 6,
	};
protected:
	ioUIRenderFrame *m_pBlueStateFrm;
	ioUIRenderFrame *m_pOrangeStateFrm;
	ioUIRenderFrame *m_pBonusFrm;
	ioUIRenderFrame *m_pBonusNoneFrm;
	ioUIRenderFrame *m_pShufflePointFrm;

	ioUIRenderImage *m_pBonusOffX;
	ioUIRenderImage *m_pIconBack;
	ioUIRenderImage *m_pPesoIcon;
	ioUIRenderImage *m_pGradeIcon;

	ioUIRenderImage *m_pBonusIconBack;
	ioUIRenderImage *m_pMannerIcon;
	ioUIRenderImage *m_pSoldierIcon;
	ioUIRenderImage *m_pGuildIcon;
	ioUIRenderImage *m_pFriendIcon;
	ioUIRenderImage *m_pPcroomIcon;
	ioUIRenderImage *m_pModeIcon;
	ioUIRenderImage *m_pAwardIcon;
	ioUIRenderImage *m_pCampIcon;
	ioUIRenderImage *m_pEtcItemIcon;
	ioUIRenderImage *m_pBonusIcon;
	ioUIRenderImage *m_pPesoBonusIcon;
	ioUIRenderImage *m_pEtcItemExpIcon;
	ioUIRenderImage *m_pEtcItemPesoIcon;
	ioUIRenderImage *m_pEtcItemExpPesoIcon;
	ioUIRenderImage *m_pVictoriesIcon;
	ioUIRenderImage *m_pContributeIcon;
	ioUIRenderImage *m_pModeConsecutively;
	ioUIRenderImage *m_pRewardPesoNum;
	ioUIRenderImage *m_pRewardExpNum;
	ioUIRenderImage *m_pShuffleStarBg;
	ioUIRenderImage *m_pShuffleStar;
	ioUIRenderImage *m_pDarkLine;
	ioUIRenderImage *m_pLightLine;

	ioUIRenderFrame *m_pSuccessionPesoFrm;
	ioUIRenderImage *m_pSuccessionIconBg;
	ioUIRenderImage *m_pSuccessionIcon;
	bool			 m_bStartSuccessionBonusPeso;

protected:
	struct BonusData
	{
		DWORD m_dwBonusID;       
		int   m_iBonusExp;
		int   m_iBonusPeso;
		DWORD m_dwStartTime;
		ioUIRenderImage *m_pClassIcon;
		ioUIRenderImage *m_pClassSubIcon;

		// Render
		ioHashString m_szTitle;
		BonusData()
		{
			m_dwBonusID   = 0;
			m_iBonusExp   = 0;
			m_iBonusPeso  = 0;
			m_dwStartTime = 0;
			m_pClassIcon  = NULL;
			m_pClassSubIcon = NULL;
		}
	};
	typedef std::vector< BonusData > vBonusData;
	vBonusData m_BonusData;
	int        m_iResultExp;
	int        m_iResultPeso;

protected:
	int m_iGradeIconPage;
	ioHashString m_szGradeIconMainName;
	ioHashString m_szGradeIconSubName;

protected:
	int   m_iCurrentBonus;
	DWORD m_dwCurrentTime;
	DWORD m_dwBonusTickTime;

	DWORD m_dwScaleTime;
	DWORD m_dwPesoScaleTime;
	DWORD m_dwExpScaleTime;
	float m_fPesoScale;
	float m_fExpScale;

protected:
	ioHashString m_szPesoRollingSound;
	ioHashString m_szExpRollingSound;
    ioHashString m_szPesoStopSound;
	ioHashString m_szExpStopSound;
	ioHashString m_szBonusCountSound;

protected:
	int m_iPlayPoint;
	int m_iWinningPoint;
	int m_iConsecutivePoint;
	int m_iAwardPoint;
protected:
	int m_iSuccessionBonusPeso;

protected:
	int GetNumberCountRand( int iNumber );

public:
	void StartBonus();

public:
	virtual void iwm_show();
	virtual void iwm_hide();

public:
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

protected:
	void OnRenderPeso( int iXPos, int iYPos );
	void OnRenderExp( int iXPos, int iYPos );
	void OnRenderBonusTtitle( int iXPos, int iYPos );
	void OnRenderBonus( int iXPos, int iYPos );
	void OnRenderBonusIcon( const BonusData &rkData, int iXPos, int iYPos );
	void OnRenderShufflePoint( int iXPos, int iYPos );
	void OnRenderSuccessionPeso( int iXPos, int iYPos );

public:
	void SetFinalRoundPoint( int iResultContribute, int iResultExp, int iResultPeso, IntVec &rvClassTypeList, float fBonusArray[ioPlayMode::BA_MAX] );
	void SetShufflePoint( int iPlayPoint, int iWinningPoint, int iConsecutivePoint, int iAwardPoint );
public:
	void SetSuccessionBonusPeso( int iBonusPeso );

public:
	TotalResultRawardWnd();
	virtual ~TotalResultRawardWnd();
};
