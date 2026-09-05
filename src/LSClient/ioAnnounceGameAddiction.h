#pragma once

class ioAnnounceGameAddiction
{
public:
	enum
	{
		ANNOUNCE_INTERVAL_TIME = 3600000, // 1½Ã°£
	};
protected:
	DWORD m_dwPreAnnouceTime;
	bool  m_bAnnounce;
	DWORD m_dwPassedHour;

public:
	void SetTime( DWORD dwRealGetTime );
	void ProcessTime( DWORD dwCurrentRealGetTime );
	bool IsAnnounce();
	int GetPassedHour();

public:
	ioAnnounceGameAddiction(void);
	virtual ~ioAnnounceGameAddiction(void);
};

