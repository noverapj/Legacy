#pragma once

class ManualMgr;
////////////////////////////////////////////////////////////////////////////////////////////////
class ioExtraItemRandomBox : public ioWnd
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
		MAX_GASHAPON = 2,
	};

protected:
	ioUIRenderImage *m_pIcon;
	ioUIRenderImage *m_pEffect;

	ioUIRenderImage *m_pBackEffect;
	ioUIRenderImage *m_pFinalBackEffect;

	ManualMgr* m_pManual;

	ioUIRenderImage *m_pNumText;
	ioUIRenderImage *m_pPlusText;

	ioUIRenderFrame *m_pStateGreenFrm;
	ioUIRenderFrame *m_pStateOrangeFrm;

	ioHashStringVec m_vIconList;

	ioHashStringVec m_vRollingEffect;
	ioHashStringVec m_vEndEfffect;

	ioHashString m_ItemName;
	int m_iItemCode;
	int m_iPeriodTime;
	int m_iReinforce;
	int m_iTradeType;

	bool m_bResult;
	bool m_bFinalResult;

	float m_fIconScale;
	DWORD m_dwIconScaleTime;
	DWORD m_dwRollingTime;
	DWORD m_dwRollingTotalTime;

	int m_iCurIconArray;
	DWORD m_dwRollingCheckTime;
	DWORD m_dwRollingEndTime;
	DWORD m_dwScaleStartTime;

	int m_iCurEffectArray;
	DWORD m_dwRollingEffectTime;
	DWORD m_dwRollingEffectCheckTime;

	ioHashString m_szRollingSound;
	ioHashString m_szResultSound;

	int m_nGradeType;
	float m_fGradeScale;
	IntVec m_vGradeList;

public:
	void ShowRandomBox( int iMachineCode );
	void OnResultInfo( int iItemCode, int iPeriodTime, int iReinforce, int iTradeType );

protected:
	void ReBatchButton( int iType );

	void PrintManual( int a_iXPos, int a_iYPos , float fScale );

	void RollingIcon( float fTimePerSec );
	void RollingEffect( float fTimePerSec );

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void iwm_hide();

	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

protected:
	virtual void OnRender();
	virtual void OnProcess( float fTimePerSec );

public:
	ioExtraItemRandomBox();
	virtual ~ioExtraItemRandomBox();
};
////////////////////////////////////////////////////////////////////////////////////////////////
