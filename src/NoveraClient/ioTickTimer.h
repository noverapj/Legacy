#pragma once


class ioTickTimer
{
private:
	DWORD	m_dwPreTick;
	DWORD	m_dwTickGap;

	DWORD	m_dwStartFPSTick;
	int		m_iFrameCnt;
	float	m_fFPS;

public:
	void UpdateTick();

public:
	DWORD GetCurTick() const { return m_dwTickGap; }
	float GetFPS() const { return m_fFPS; }

public:
	ioTickTimer();
	virtual ~ioTickTimer();
};

