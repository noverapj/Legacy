#pragma once

#include "../ioComplexStringPrinter.h"
#include "../io3DEngine/ioStringConverter.h"
#include "../io3DEngine/ioButton.h"

#include "ioMessageBox.h"

class ManualMgr;

class GashaponWnd : public ioWnd
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
		ROLLING_BTN  = 1,
		RESULT_BTN	 = 2,
		MAX_GASHAPON = 1553,
	};

protected:
	ioUIRenderImage *m_pIcon;
	ioUIRenderImage *m_pSubIcon;
	ioUIRenderImage *m_pEffect;

	ioUIRenderImage *m_pBackEffect;
	ioUIRenderImage *m_pFinalBackEffect;
	ioUIRenderImage *m_pCostumeMark;

	ManualMgr* m_pManual;

	ioHashStringVec m_vIconList[MAX_GASHAPON];
	std::vector<bool> m_vCostumeList[MAX_GASHAPON];
	IntVec m_vGradeTypeList[MAX_GASHAPON];
	
	// 서버에서 보내온 icon을 임의로 교체
	IntVec m_vIconChangePresentTypeVec[MAX_GASHAPON];
	IntVec m_vIconChangePresentValue1Vec[MAX_GASHAPON];
	IntVec m_vIconChangePresentValue2Vec[MAX_GASHAPON];
	IntVec m_vIconChangePresentValue3Vec[MAX_GASHAPON];
	IntVec m_vIconChangePresentValue4Vec[MAX_GASHAPON];
	ioHashStringVec m_vIconChangeIconVec[MAX_GASHAPON];

	ioHashStringVec m_vRollingEffect;
	ioHashStringVec m_vEndEfffect;

	short m_iPresentType;
	int   m_iPresentValue1;
	int   m_iPresentValue2;
	int   m_iPresentValue3;
	int   m_iPresentValue4;
	bool  m_bAlarm;
	int   m_iPresentPeso;

	bool m_bResult;
	bool m_bFinalResult;

	float m_fIconScale;
	DWORD m_dwIconScaleTime;
	DWORD m_dwRollingTime;
	//DWORD m_dwRollingTotalTime;

	int m_iCurIconArray;
	DWORD m_dwRollingCheckTime;
	DWORD m_dwRollingEndTime;
	DWORD m_dwScaleStartTime;

	int m_iCurEffectArray;
	DWORD m_dwRollingEffectTime;
	DWORD m_dwRollingEffectCheckTime;

	ioHashString m_szRollingSound;
	ioHashString m_szResultSound;

	ioHashString m_szDesc1[MAX_GASHAPON];
	ioHashString m_szDesc2[MAX_GASHAPON];
	ioHashString m_szTitle[MAX_GASHAPON];
	DWORD m_dwRollingTotalTime[MAX_GASHAPON];

	DWORD m_dwEtcItemType;
	DWORD m_dwPossessEtcItemType;         // 시간 가챠 아이템은 1개당 4가지 가챠를 가진다. 그러므로 3가지는 가챠만 세팅되어있고 1개만 유저가 보유중인 아이템

	bool m_bIsCostume;
	int m_nGradeType;
	float m_fGradeScale;

public:
	void ShowRandomBox( DWORD dwPossessEtcItemType, DWORD dwEtcItemType );
	void OnResultInfo( short iPresentType, int iPresentValue1, int iPresentValue2, int iPresentValue3, int iPresentValue4, bool bAlram, int iPresentPeso );

	bool IsRolling();

protected:
	void ReBatchButton( int iType );
	void SetPresentIcon();
	void SetGashaponList( IntVec &rPresentTypeList, IntVec &rPresentValue1List, IntVec &rPresentValue2List, IntVec &rPresentValue3List, IntVec &rPresentValue4List );

	int  GetGashaponArray();

	void PrintManual( int a_iXPos, int a_iYPos , float fScale );

	void RollingIcon( float fTimePerSec );
	void RollingEffect( float fTimePerSec );

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void iwm_hide();
	virtual void iwm_show();

	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

protected:
	virtual void OnRender();
	virtual void OnProcess( float fTimePerSec );

public:
	GashaponWnd(void);
	virtual ~GashaponWnd(void);
};

