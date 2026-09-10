#pragma once

class ManualMgr;
//////////////////////////////////////////////////////////////////////
class ioNewMedalRandomBox : public ioWnd
{
public:
	enum
	{
		ID_EXIT = 1,
		ID_SELECT	= 2,
		ID_ROLLING	= 3,
		ID_CONFIRM = 4,
		ID_RETRY = 5,
		ID_LOCK = 6,
		ID_START = 7,
		ID_CHECKBTN = 11,

		CHECKBTN_COUNT = 8,
	};
	enum
	{
		SELECT_BTN = 1,
		ROLLING_BTN = 2,
		RESULT_BTN	= 3,
	};

protected:
	ioUIRenderImage *m_pIcon;
	ioUIRenderImage *m_pSubIcon;
	ioUIRenderImage *m_pBackEffect;
	ioUIRenderImage *m_pFinalBackEffect;
	ioUIRenderImage *m_pOverInfoBox;

	ioHashString m_ItemName;
	ioHashString m_szRecvStatErrMsg;

	int m_iState;
	int m_iEnableSelectStat;
	int m_iSelectedStat;
	int m_iPresentIndex;
	int m_iPresentSlot;
	int m_iMedalCode;

	bool m_bResult;
	bool m_bFinalResult;

	DWORD m_dwRollingTotalTime;
	DWORD m_dwRollingEndTime;

	ioHashString m_szRollingSound;
	ioHashString m_szResultSound;
	ioHashString m_vCheckText[CHECKBTN_COUNT];
	ioHashString m_vCheckIcon[CHECKBTN_COUNT];

	ioHashString m_szTitleSubText;
	ioHashString m_szSelectText;
	ioHashString m_szLoopText;
	ioHashString m_szResultText;

	ioUI3DEffectRender*	m_pLoopEffect;
	ioUI3DEffectRender*	m_pResultEffect;
	int m_iEffectPosX;
	int m_iEffectPosY;
	int m_iFinalStatus[CHECKBTN_COUNT];

public:
	void ShowSelectStatus( int iPresentIndex, int iPresentSlot, int iMedalCode );
	void SetStatus( int iPresentIndex, int iPresentSlot, int iStat[8] );

protected:
	void ReBatchButton( int iType );
	void RollingIcon( float fTimePerSec );
	void SetBtnPos();
	void SetRolling();
	void SetFinalResult();
	void SetConfirm();

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void iwm_hide();

	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

protected:
	virtual void OnRender();
	virtual void OnProcess( float fTimePerSec );

public:
	ioNewMedalRandomBox();
	virtual ~ioNewMedalRandomBox();
};

class ioNewMedalBtn : public ioCheckButton
{
protected:
	ioUIRenderImage *m_pIcon;
	ioUIRenderImage *m_pIconBack;
	ioUIRenderImage *m_pNumText;
	ioUIRenderImage *m_pPlusText;
	ioUIRenderFrame *m_pSelectFrm;
	ioUIRenderFrame *m_pCheckFrm;

	ioHashString m_szTitleText;
	int m_iStatue;
	int m_iType;
	int m_iRandomMin;
	int m_iRandomMax;
	DWORD m_dwStatusAniTime;
	DWORD m_dwStatusAniStartTime;
	DWORD m_dwStatusEffectGapTime;

public:
	void SetIconTitle( ioUIRenderImage *pIcon, ioHashString szTitle );
	void SetFinalStatus( int iValue, int iType );
	void SetRandomRange( int iMin, int iMax );

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void ParseExtraInfo( ioXMLElement &xElement );
	virtual void OnRender();

public:
	ioNewMedalBtn();
	virtual ~ioNewMedalBtn();
};