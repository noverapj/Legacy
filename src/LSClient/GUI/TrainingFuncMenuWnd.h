#pragma once

class TrainingFuncMenuWnd : public ioMovingWnd
{
public:
	enum
	{
		ID_BATTLE_BTN		= 1,
		ID_PLAZA_BTN		= 2,
		ID_TRADE_BTN		= 3,
		ID_HEAD_QUATER_BTN	= 4,
		ID_HOME_BTN			= 5,
		ID_GUILD_HQ_BTN		= 6,
		ID_BTN_MAX			= 7,
	};
	
	enum ShowAniType 
	{
		SAT_NONE         = 0,
		SAT_START_ROUND  = 1,
		SAT_DELAY        = 2,
		SAT_UP           = 3,  
		SAT_DOWN         = 4,  
		SAT_UP_STOP      = 5, 
		SAT_STOP         = 6,
		SAT_END          = 7,
	};

protected:
	enum 
	{
		POS_SAVE_VERSION = 4,
	};
	
	ioPlayMode* m_pCurrMode;

	int m_iDefaultX;
	int m_iDefaultY;

	bool m_bChangeResolution;
	bool m_bStartAni;

	FSMStateMgr m_ShowAniState;

public:
	void SetPlayMode( ioPlayMode* pMode ){ m_pCurrMode = pMode; }

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

protected:
	void UpdateHQButton();
	void UpdateHomeButton();
	void UpdateGuildHQButton();

public:
	virtual void SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate = false );
	void ResetWindow();

	void InitShowAniState();
	void ChangeShowAniState( ShowAniType eShowAniType );
	int GetCurState() const;

protected:
	void UpdateDefaultPos();
	bool SkipRender();

protected:
	void ShowTrainingFuncWnd( DWORD dwID );

public:
	TrainingFuncMenuWnd();
	virtual ~TrainingFuncMenuWnd();
};