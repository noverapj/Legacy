#pragma once

#include "../io3DEngine/ioStringConverter.h"
#include "../io3DEngine/ioButton.h"

#include "../ioComplexStringPrinter.h"
#include "ioMessageBox.h"

class ioUIRenderImage;
class ioUIRenderFrame;
class ioXMLElement;
class ManualMgr;

////////////////////////////////////////////////////////////////////////////////
class ItemCompoundResultWnd : public ioWnd
{
public:
	enum
	{
		ID_ROLLING	= 1,
		ID_USE		= 2,
		ID_GO		= 3,
	};

	enum
	{
		ROLLING_BTN = 1,
		RESULT_BTN	= 2,
	};

protected:
	ioUIRenderImage *m_pIcon;
	ioUIRenderImage *m_pEffect;

	ioUIRenderImage *m_pBackEffect;
	ioUIRenderImage *m_pFinalBackEffect;

	ManualMgr* m_pManual;

	ioUIRenderImage *m_pNumText;
	ioUIRenderImage *m_pPlusText;

	ioHashStringVec m_vRollingEffect;
	ioHashStringVec m_vEndEfffect_S;	// 성공
	ioHashStringVec m_vEndEfffect_F;	// 실패

	DWORD m_dwEtcItemCode;

	bool m_bResult;
	bool m_bSuccess;
	int m_iTargetSlot;
	int m_iPreReinforce;
	int m_iResultReinforce;
	int m_iItemCode;
	ioHashString m_ItemName;

	float m_fIconScale;
	DWORD m_dwIconScaleTime;
	DWORD m_dwRollingTime;
	DWORD m_dwRollingTotalTime;

	DWORD m_dwRollingCheckTime;
	DWORD m_dwRollingEndTime;
	DWORD m_dwScaleStartTime;


	int m_iCurEffectArray;
	DWORD m_dwRollingEffectTime;
	DWORD m_dwRollingEffectCheckTime;


	ioHashString m_szRollingSound;
	ioHashString m_szSuccessSound;
	ioHashString m_szBigSuccessSound;
	ioHashString m_szFailSound;

	int m_nGradeType;
	float m_fGradeScale;

protected:
	void ReBatchButton( int iType );

	void UpdateCompoundInfo();

	void RollingEffect( float fTimePerSec );
	void PrintManual( int a_iXPos, int a_iYPos , float fScale );

public:
	void SetCompoundResultInfo( bool bSuccess, int iTargetSlot, int iReinforce, DWORD dwCode );

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void iwm_hide();

	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	ItemCompoundResultWnd();
	virtual ~ItemCompoundResultWnd();
};
////////////////////////////////////////////////////////////////////////////////////////////////
class ItemGrowthCatalystResultWnd : public ioWnd
{
public:
	enum
	{
		ID_ROLLING	= 1,
		ID_USE		= 2,
		ID_GO		= 3,
	};

	enum
	{
		ROLLING_BTN = 1,
		RESULT_BTN	= 2,
	};

protected:
	ioUIRenderImage *m_pIcon;
	ioUIRenderImage *m_pEffect;

	ioUIRenderImage *m_pBackEffect;
	ioUIRenderImage *m_pFinalBackEffect;

	ioUIRenderFrame *m_pCatalystMark;

	ManualMgr* m_pManual;

	ioUIRenderImage *m_pNumText;
	ioUIRenderImage *m_pPlusText;

	ioHashStringVec m_vRollingEffect;
	ioHashStringVec m_vEndEfffect_S;	// 성공
	ioHashStringVec m_vEndEfffect_F;	// 실패

	DWORD m_dwEtcItemCode;

	bool m_bResult;
	bool m_bSuccess;
	int m_iTargetSlot;
	int m_iPreReinforce;
	int m_iResultReinforce;
	int m_iItemCode;
	int m_PeriodType;
	ioHashString m_ItemName;

	float m_fIconScale;
	DWORD m_dwIconScaleTime;
	DWORD m_dwRollingTime;
	DWORD m_dwRollingTotalTime;

	DWORD m_dwRollingCheckTime;
	DWORD m_dwRollingEndTime;
	DWORD m_dwScaleStartTime;


	int m_iCurEffectArray;
	DWORD m_dwRollingEffectTime;
	DWORD m_dwRollingEffectCheckTime;


	ioHashString m_szRollingSound;
	ioHashString m_szSuccessSound;
	ioHashString m_szBigSuccessSound;
	ioHashString m_szFailSound;


protected:
	void ReBatchButton( int iType );

	void UpdateGrowthCatalystInfo();

	void RollingEffect( float fTimePerSec );
	void PrintManual( int a_iXPos, int a_iYPos , float fScale );

public:
	void SetGrowthCatalystResultInfo( bool bSuccess, int iTargetSlot, int iReinforce, int iTargetPeriodType, DWORD dwCode );
	void SetGrowthCatalystDelayInfo( int iTargetSlot );

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void iwm_hide();

	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	ItemGrowthCatalystResultWnd();
	virtual ~ItemGrowthCatalystResultWnd();
};
////////////////////////////////////////////////////////////////////////////////////////////////
class ItemCompoundExResultWnd : public ioWnd
{
public:
	enum
	{
		ID_ROLLING	= 1,
		ID_USE		= 2,
		ID_GO		= 3,
	};

	enum
	{
		ROLLING_BTN = 1,
		RESULT_BTN	= 2,
	};

protected:
	ioUIRenderImage *m_pIcon;
	ioUIRenderImage *m_pEffect;

	ioUIRenderImage *m_pBackEffect;
	ioUIRenderImage *m_pFinalBackEffect;

	ManualMgr* m_pManual;

	ioUIRenderImage *m_pNumText;
	ioUIRenderImage *m_pPlusText;

	ioHashStringVec m_vRollingEffect;
	ioHashStringVec m_vEndEfffect_S;	// 성공
	ioHashStringVec m_vEndEfffect_F;	// 실패

	DWORD m_dwEtcItemCode;

	bool m_bResult;
	bool m_bSuccess;
	int m_iTargetSlot;
	int m_iPreReinforce;
	int m_iResultReinforce;
	int m_iItemCode;
	ioHashString m_ItemName;

	float m_fIconScale;
	DWORD m_dwIconScaleTime;
	DWORD m_dwRollingTime;
	DWORD m_dwRollingTotalTime;

	DWORD m_dwRollingCheckTime;
	DWORD m_dwRollingEndTime;
	DWORD m_dwScaleStartTime;


	int m_iCurEffectArray;
	DWORD m_dwRollingEffectTime;
	DWORD m_dwRollingEffectCheckTime;


	ioHashString m_szRollingSound;
	ioHashString m_szSuccessSound;
	ioHashString m_szBigSuccessSound;
	ioHashString m_szFailSound;

	int m_nGradeType;
	float m_fGradeScale;

protected:
	void ReBatchButton( int iType );

	void UpdateItemCompoundExInfo();

	void RollingEffect( float fTimePerSec );
	void PrintManual( int a_iXPos, int a_iYPos , float fScale );

public:
	void SetItemCompoundExInfo( bool bSuccess, int iTargetSlot, int iReinforce, DWORD dwCode );

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void iwm_hide();

	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	ItemCompoundExResultWnd();
	virtual ~ItemCompoundExResultWnd();
};
////////////////////////////////////////////////////////////////////////////////////////////////
class ItemLuckyCoinResultWnd : public ioWnd
{
public:
	enum
	{
		ID_ROLLING	= 1,
		ID_USE		= 2,
		ID_GO		= 3,
	};

	enum
	{
		ROLLING_BTN = 1,
		RESULT_BTN	= 2,
	};

protected:
	ioUIRenderImage *m_pIcon;
	ioUIRenderImage *m_pEffect;

	ioUIRenderImage *m_pFinalBackEffect;
	ioUIRenderImage *m_pCostumeMark;

	ManualMgr* m_pManual;

	ioHashStringVec m_vEndEfffect_S;	// 성공
	
	DWORD m_dwEtcItemCode;

	int m_iExchangeIndex;
	int m_iExchangeManual;
	ioHashString m_szExchangeName;
	ioHashString m_szExchangeIcon;
	bool m_bExchangeCostume;

	bool m_bResult;
	bool m_bSuccess;;

	float m_fIconScale;
	DWORD m_dwIconScaleTime;
	DWORD m_dwRollingTime;
	DWORD m_dwRollingTotalTime;

	DWORD m_dwRollingCheckTime;
	DWORD m_dwRollingEndTime;
	DWORD m_dwScaleStartTime;

	int m_iCurEffectArray;
	DWORD m_dwRollingEffectTime;
	DWORD m_dwRollingEffectCheckTime;

	ioHashString m_szSuccessSound;

	int m_nGradeType;

protected:
	void ReBatchButton( int iType );
	void RollingEffect( float fTimePerSec );
	void PrintManual( int a_iXPos, int a_iYPos , float fScale );

public:
	void SetItemLuckyCoinResultInfo( DWORD dwCode, int iSelectIndex );

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void iwm_hide();

	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	ItemLuckyCoinResultWnd();
	virtual ~ItemLuckyCoinResultWnd();
};
////////////////////////////////////////////////////////////////////////////////////////////////
class ItemRainbowMixerResultWnd : public ioWnd
{
public:
	enum
	{
		ID_ROLLING	= 1,
		ID_USE		= 2,
		ID_GO		= 3,
	};

	enum
	{
		ROLLING_BTN = 1,
		RESULT_BTN	= 2,
	};

protected:
	ioUIRenderImage *m_pIcon;
	ioUIRenderImage *m_pEffect;

	ioUIRenderImage *m_pFinalBackEffect;

	ManualMgr* m_pManual;

	ioHashStringVec m_vEndEfffect_S;	// 성공

	DWORD m_dwEtcItemCode;
	ioHashString m_szPresentName;

	bool m_bResult;
	bool m_bSuccess;;

	float m_fIconScale;
	DWORD m_dwIconScaleTime;
	DWORD m_dwRollingTime;
	DWORD m_dwRollingTotalTime;

	DWORD m_dwRollingCheckTime;
	DWORD m_dwRollingEndTime;
	DWORD m_dwScaleStartTime;

	int m_iCurEffectArray;
	DWORD m_dwRollingEffectTime;
	DWORD m_dwRollingEffectCheckTime;

	ioHashString m_szSuccessSound;

protected:
	void ReBatchButton( int iType );
	void RollingEffect( float fTimePerSec );
	void PrintManual( int a_iXPos, int a_iYPos , float fScale );

public:
	void SetInfoAndShow( DWORD dwCode, int iSelectIndex );

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void iwm_hide();

	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	ItemRainbowMixerResultWnd();
	virtual ~ItemRainbowMixerResultWnd();
};
////////////////////////////////////////////////////////////////////////////////////////////////
class ItemLostSagaMixerResultWnd : public ioWnd
{
public:
	enum
	{
		ID_ROLLING	= 1,
		ID_USE		= 2,
		ID_GO		= 3,
	};

	enum
	{
		ROLLING_BTN = 1,
		RESULT_BTN	= 2,
	};

protected:
	ioUIRenderImage *m_pIcon;
	ioUIRenderImage *m_pEffect;

	ioUIRenderImage *m_pFinalBackEffect;

	ManualMgr* m_pManual;

	ioHashStringVec m_vEndEfffect_S;	// 성공

	DWORD m_dwEtcItemCode;
	ioHashString m_szPresentName;

	bool m_bResult;
	bool m_bSuccess;;

	float m_fIconScale;
	DWORD m_dwIconScaleTime;
	DWORD m_dwRollingTime;
	DWORD m_dwRollingTotalTime;

	DWORD m_dwRollingCheckTime;
	DWORD m_dwRollingEndTime;
	DWORD m_dwScaleStartTime;

	int m_iCurEffectArray;
	DWORD m_dwRollingEffectTime;
	DWORD m_dwRollingEffectCheckTime;

	ioHashString m_szSuccessSound;

protected:
	void ReBatchButton( int iType );
	void RollingEffect( float fTimePerSec );
	void PrintManual( int a_iXPos, int a_iYPos , float fScale );

public:
	void SetInfoAndShow( DWORD dwCode, int iSelectIndex );

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void iwm_hide();

	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	ItemLostSagaMixerResultWnd();
	virtual ~ItemLostSagaMixerResultWnd();
};
////////////////////////////////////////////////////////////////////////////////////////////////
class AccessoryCompoundResultWnd : public ioWnd
{
public:
	enum
	{
		ID_ROLLING	= 1,
		ID_GO		= 3,
	};

	enum
	{
		ROLLING_BTN = 1,
		RESULT_BTN	= 2,

		COMPOUND_EFFECT_DESC_SIZE = 3,
	};

protected:
	ioUIRenderImage *m_pIcon;
	ioUIRenderImage *m_pEffect;
	ioUIRenderImage *m_pBackEffect;
	ioUIRenderImage *m_pFinalBackEffect;

	ManualMgr* m_pManual;

	ioHashStringVec m_vRollingEffect;
	ioHashStringVec m_vEndEfffect;
	ioHashString m_szRollingMsg;
	ioHashString m_szResultMsg;

	DWORD m_dwEtcItemCode;

	bool m_bResult;
	int m_iTargetIndex;
	int m_iCompoundCode;
	int m_iCompoundValue;
	int m_iItemCode;
	ioHashString m_ItemName;

	float m_fIconScale;
	DWORD m_dwIconScaleTime;
	DWORD m_dwRollingTime;
	DWORD m_dwRollingTotalTime;

	DWORD m_dwRollingCheckTime;
	DWORD m_dwRollingEndTime;
	DWORD m_dwScaleStartTime;

	int m_iCurEffectArray;
	DWORD m_dwRollingEffectTime;
	DWORD m_dwRollingEffectCheckTime;
	
	ioHashString m_szRollingSound;
	ioHashString m_szSuccessSound;

protected:
	void ReBatchButton( int iType );

	void UpdateCompoundInfo();

	void RollingEffect( float fTimePerSec );
	void PrintManual( int a_iXPos, int a_iYPos , float fScale );

public:
	void SetCompoundResultInfo( int iTargetIndex, int iCompoundCode, int iCompoundValue, DWORD dwCode );

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void iwm_hide();

	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	AccessoryCompoundResultWnd();
	virtual ~AccessoryCompoundResultWnd();
};