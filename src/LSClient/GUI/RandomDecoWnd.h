#pragma once

#include "../ioComplexStringPrinter.h"
#include "../io3DEngine/ioStringConverter.h"
#include "../io3DEngine/ioButton.h"

#include "ioMessageBox.h"

class ManualMgr;

class RandomDecoWnd : public ioWnd
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

	bool m_bMan;
	ioHashStringVec m_vIconListM;
	ioHashStringVec m_vIconListW;

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
	DWORD m_dwRollingTotalTime;

	int m_iCurEffectArray;
	DWORD m_dwRollingEffectTime;
	DWORD m_dwRollingEffectCheckTime;

	int m_iCurIconArray;
	DWORD m_dwRollingCheckTime;
	DWORD m_dwRollingEndTime;
	DWORD m_dwScaleStartTime;

	ioHashString m_szRollingSound;
	ioHashString m_szResultSound;


public:
	void ShowRandomBox( bool bMan );
	void OnResultInfo( short iPresentType, int iPresentValue1, int iPresentValue2, int iPresentValue3, int iPresentValue4, bool bAlram, int iPresentPeso );

	bool IsRolling();

protected:
	void ReBatchButton( int iType );
	void SetPresentIcon();

	void PrintManual( int a_iXPos, int a_iYPos , float fScale );

	void RollingIcon( float fTimePerSec );
	void RollingEffect( float fTimePerSec );

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void iwm_hide();

	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

protected:
	virtual void OnRender();
	virtual void OnProcess( float fTimePerSec );

public:
	RandomDecoWnd(void);
	virtual ~RandomDecoWnd(void);
};

