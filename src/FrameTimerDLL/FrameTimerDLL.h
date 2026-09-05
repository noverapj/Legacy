/********************************************************************
	created:	2003/02/25
	created:	25:2:2003   10:40
	filename: 	d:\service\frametimerdll\frametimer\cframetimer.h
	file path:	d:\service\frametimerdll\frametimer
	file base:	CFrameTimerDLL
	file ext:	h
	author:		±Ë¿Œ¡ﬂ 
	
	purpose:	
	history:	
*********************************************************************/
#ifndef __CFRAMETIMER_H__
#define __CFRAMETIMER_H__

#define WIN32_LEAN_AND_MEAN

#ifdef FRAMETIMERDLL_EXPORTS
#define __EX __declspec(dllexport)
#else
#define __EX __declspec(dllimport)
#endif

#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "Winmm.lib")

#define TIMEGETTIME()	FrameTimer.GetTotalSec()

typedef unsigned __int64 UINT64;

class __EX CFrameTimer
{
public:
    CFrameTimer();

    void Start(float fFramesPerSec);
    void SetFrame();

    float GetFramesPerSec();
    float GetSecsPerFrame();

    UINT64 GetTicks();
    UINT64 GetTicksPerSec();
    UINT64 GetTicksPerFrame();

	DWORD  GetTotalSec();

protected:
    float m_fTicksPerSec;
    float m_fFramesPerSec;
    float m_fSecsPerFrame;

	UINT64 m_qwTicksStart;
    UINT64 m_qwTicks;
    UINT64 m_qwTicksPerSec;
    UINT64 m_qwTicksPerFrame;
};


extern __EX CFrameTimer FrameTimer;


//////////////////////////////////////////////////////////////////////////////
// Inline methods ////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

inline float
CFrameTimer::GetFramesPerSec()
{
    return m_fFramesPerSec;
};

inline float
CFrameTimer::GetSecsPerFrame()
{
    return m_fSecsPerFrame;
};

inline UINT64
CFrameTimer::GetTicksPerSec()
{
    return m_qwTicksPerSec;
};

inline UINT64
CFrameTimer::GetTicksPerFrame()
{
    return m_qwTicksPerFrame;
};


#endif __CFRAMETIMER_H__