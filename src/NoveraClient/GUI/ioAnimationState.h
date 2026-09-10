#pragma once

class ioAnimationState
{
public:
	enum ShowAniType 
	{
		SAT_NONE         = 0,
		SAT_START_ROUND  = 1,
		SAT_DELAY        = 2,
		SAT_UP           = 3,  // 가속
		SAT_DOWN         = 4,  // 감속
		SAT_UP_STOP      = 5,  // 가속
		SAT_STOP         = 6,
		SAT_END          = 7,
	};

protected:
	FSMStateMgr m_ShowAniState;

protected:
	void InitShowAniState();
	
public:
	bool IsRender();
	bool UpdateShowAni(int nDefX, int nDefY, int &nCurrY);

	
	void InitAniState();
	void ChangeShowAniState( ShowAniType eShowAniType );
	int  GetCurShowAniState();

public:
	ioAnimationState(void);
	virtual ~ioAnimationState(void);
};

