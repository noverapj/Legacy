

#ifndef _ioFrameTimer_h_
#define _ioFrameTimer_h_

#include "io3DCommon.h"
#include "ioPrerequisites.h"

class __EX ioFrameTimer : public Singleton< ioFrameTimer >
{
protected:
    float m_fSecsPerFrame;
	float m_fCurFrameGap;

	DWORD m_dwTicksStart;
	DWORD m_dwTicks;

	DWORD m_dwLoopGetTime;
	DWORD m_dwFrameCounter;

	float m_fTimeRate;

	bool   m_bTimerPause;

public:
	static void SetWindowsModule( bool bWin2k );

public:
	void  Start( float fFramesPerSec );
	DWORD Frame();

public:
	void SetTimeRate( float fRate );
	void SetTimerPause( bool bPause );
	void SetCurFrameGapForce( float fFrameGap );
	void SetLoopGetTime( DWORD dwLoopTime );

public:
	inline DWORD GetLoopSec() const { return m_dwLoopGetTime; }
	inline float GetCurFrameGap() const { return m_fCurFrameGap; }
	inline float GetSecPerFrame() const { return m_fSecsPerFrame; }
	inline DWORD GetFrameCounter() const { return m_dwFrameCounter; }

	DWORD GetTotalSec();		// 1/1000 단위
	DWORD GetPerfTotalSec();	// 1/10000 단위

public:
	static ioFrameTimer& GetSingleton();

public:
	ioFrameTimer();
	virtual ~ioFrameTimer();
};

#define g_FrameTimer ioFrameTimer::GetSingleton()

#define REALGETTIME()	g_FrameTimer.GetTotalSec()
#define FRAMEGETTIME()	g_FrameTimer.GetLoopSec()
#define PERFCHECKTIME()	g_FrameTimer.GetPerfTotalSec()

#endif
