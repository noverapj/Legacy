#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////
class ioCostumeRandomBox : public ioWnd
{
public:
	enum
	{
		ID_ROLLING	= 1,
		ID_GO		= 2,
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
	ioUIRenderImage *m_pCostumeMark;

	ioHashStringVec m_vIconList;

	ioHashStringVec m_vRollingEffect;
	ioHashStringVec m_vEndEfffect;

	ioHashString m_szCostumeName;
	int m_nCostumeCode;
	int m_nPeriodTime;

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

public:
	void ShowRandomBox( int nMachineCode );
	void OnResultInfo( int nCostumeCode, int nPeriodTime );

protected:
	void ReBatchButton( int nType );	
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
	ioCostumeRandomBox();
	virtual ~ioCostumeRandomBox();
};
////////////////////////////////////////////////////////////////////////////////////////////////
