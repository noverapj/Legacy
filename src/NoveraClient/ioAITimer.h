#pragma once
class	ioAITimer
{
public:
	ioAITimer() : m_nDuration(0), m_nStartTimeStamp(0) {}
	virtual ~ioAITimer() {}

	void	SetDuration( const unsigned int& duration )	{	m_nDuration = duration;	}

	void	StartTimer( unsigned int newDuration = 0 );
	void	StartTimerOnce( unsigned int newDuration = 0 );
	void	StopTimer();
	bool	HasTimeoutOccured();
	void	ResetTimer();

	bool	IsRunning()	{	return (m_nStartTimeStamp != 0);	}

	unsigned int	GetDuration()	{	return m_nDuration;			}
	unsigned int	GetStartTime()	{	return m_nStartTimeStamp;	}

private:
	unsigned int	m_nDuration;
	unsigned int	m_nStartTimeStamp;
};