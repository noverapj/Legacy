

#ifndef _ioTimer_h_
#define _ioTimer_h_

class __EX ioTimer
{
private:
	float	m_fTotalTime;
	float	m_fPresetTime;

public:
	void InitTimer();
	bool IsTimePast( DWORD dwTime );
	bool IsPresetTimePast( DWORD dwTime );

public:
	ioTimer();
	~ioTimer();
};

#endif