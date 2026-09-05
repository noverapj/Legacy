#pragma once

////////////////////////////////////////////////////////////////////////////////
class ItemMaterialCompoundResultWnd : public ioWnd
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

	ioUIRenderImage *m_pGaugeMainBack;
	ioUIRenderImage *m_pGaugeBack;
	ioUIRenderImage *m_pNewGauge;
	ioUIRenderImage *m_pPreGauge;

	ManualMgr* m_pManual;

	ioUIRenderImage *m_pNumText;
	ioUIRenderImage *m_pPlusText;

	ioHashStringVec m_vRollingEffect;
	ioHashStringVec m_vExtraRollingEffect;
	// 성공
	ioHashStringVec m_vEndEfffect_S;	
	// 초월성공
	ioHashStringVec m_vEndEfffect_ExtraS;
	// 실패
	ioHashStringVec m_vEndEfffect_F;	

	DWORD m_dwEtcItemCode;

	bool m_bResult;
	bool m_bSuccess;
	int m_nTargetSlot;
	int m_nPreReinforce;
	int m_nResultReinforce;
	int m_nItemCode;
	ioHashString m_ItemName;

	float m_fIconScale;
	DWORD m_dwIconScaleTime;
	DWORD m_dwRollingTotalTime;

	DWORD m_dwRollingEndTime;
	DWORD m_dwScaleStartTime;

	int m_iCurEffectArray;
	DWORD m_dwRollingEffectTime;
	DWORD m_dwRollingEffectCheckTime;

	ioHashString m_szRollingSound;
	ioHashString m_szSuccessSound;
	ioHashString m_szExtraSuccessSound;
	ioHashString m_szFailSound;

	int m_nMaxFailExp;
	float m_fNewFailExpRate;
	float m_fPreFailExpRate;

	int m_nGradeType;
	float m_fGradeScale;
	
public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void iwm_hide();

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

protected:
	void ReBatchButton( int iType );
	void RollingEffect( float fTimePerSec );
	void RenderFailExp( int iXPos, int iYPos );

protected:
	void UpdateCompoundInfo();

public:
	void SetCompoundResultInfo( bool bSuccess, int nTargetSlot, int nReinforce, DWORD dwCode );
	void SetFailExp( int nPreFailExp, int nCurFailExp, int nItemCode );

public:
	ItemMaterialCompoundResultWnd();
	virtual ~ItemMaterialCompoundResultWnd();
};
