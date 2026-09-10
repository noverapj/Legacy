#pragma once


class NewShopBoughtWnd : public ioWnd
{
public:
	enum 
	{
		ID_ROLLING_BTN	        = 1,
		ID_BUY_ADD_BTN          = 2,
		ID_GO_BTN               = 3,
	};

	enum State
	{
		S_ICON_ROLLING   = 1,
		S_RESULT_SETTING = 2,
		S_ICON_SCALING   = 3,
	};

protected:
	ioUIRenderImage *m_pIcon;
	ioUIRenderImage *m_pSubIcon;
	ioUIRenderImage *m_pNum;
	ioUIRenderImage *m_pPlus;
	ioUIRenderImage *m_pEffect;

	ioUIRenderImage *m_pIconBack;
	ioUIRenderImage *m_pResultIconBack;
	ioUIRenderImage *m_pCostumeMark;

	ioUIRenderFrame *m_pStateOrangeFrm;
	ioUIRenderFrame *m_pStateGreenFrm;

	ioHashString     m_sTitle1;
	ioHashString     m_sTitle2;

	ioHashString     m_sRollingSound;
	ioHashString     m_sResultSound;

	ioHashStringVec  m_vDescOneLineVec;
	ioHashStringVec  m_vDescTwoLineVec;

	short            m_iPresentType;
	int              m_iPresentValue1;
	int              m_iPresentValue2;
	bool             m_bAlarm;

	int				 m_iTradeType;

	ioHashStringVec  m_vIconList;
	ioHashStringVec  m_vSubIconList;
	int              m_iCurIconArray;

	float			 m_fIconScale;
	DWORD            m_dwIconScaleTime;
	DWORD            m_dwRollingTime;
	DWORD            m_dwRollingTotalTime;

	DWORD            m_dwRollingCheckTime;
	DWORD            m_dwRollingEndTime;
	DWORD            m_dwScaleStartTime;

	State            m_eState;

	ioHashStringVec  m_vRollingEffect;
	ioHashStringVec  m_vEndEfffect;

	int              m_iCurEffectArray;
	DWORD            m_dwRollingEffectTime;
	DWORD            m_dwRollingEffectCheckTime;

	bool m_bResultState;

	bool m_bIsCostume;
	int m_nGradeType;
	float m_fGradeScale;
	IntVec m_vGradeList;

public:
	void ShowBoughtWnd( short iPresentType, int iPresentValue1, int iPresentValue2 );
	void SetResultInfo( short iPresentType, int iPresentValue1, int iPresentValue2, bool bAlarm, int iTradeType );

protected:
	void ProcessResult( DWORD dwCurTime );
	void ProcessScale( DWORD dwCurTime );
	void ProcessIcon( DWORD dwCurTime );
	void RenderReinforce( int iXPos, int iYPos, int iReinforce );
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
	NewShopBoughtWnd();
	virtual ~NewShopBoughtWnd();
};
