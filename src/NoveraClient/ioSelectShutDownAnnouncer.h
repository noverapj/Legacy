#pragma once

class ioSelectShutDownAnnouncer
{
public:
	enum { CHECK_TIME = 60000 }; // 1분

	typedef std::map< DWORD , ioHashString > mAnnounceInfo;

protected:
	mAnnounceInfo m_mAnnounceInfo;
	int m_iShutDownYear;
	int m_iShutDownMonth;
	int m_iShutDownDay;
	int m_iShutDownHour;
	int m_iShutDownMinute;
	bool  m_bAnnounce;
	DWORD m_dwRemainMinutes;
	DWORD m_dwCheckTime;
	DWORD m_dwAnnounceRemainMinutes;

	bool m_bProcess;
	
public:
	void LoadINI();
	void SetShutDownTime( int iYear, int iMonth, int iDay, int iHour, int iMinute );
	void Process( time_t kServerTime, DWORD dwRealGetTime );
	bool IsAnnounce();
	bool IsProcess() const { return m_bProcess; }
	DWORD GetRemainMinutes();
	void GetAnnounce( IN DWORD dwRemainMinutes, OUT ioHashString &rsAnnounce );
	time_t ConvertSecondTime( int year, int month, int day, int hour, int minute, int second ); // 중복 함수 고민 필요.

	// 엠게임 기능
	void WriteFile( const char *szDate );
	void ReadFile();

	int GetYear();
	int GetMonth();
	int GetDay();
	int GetHour();

public:
	ioSelectShutDownAnnouncer(void);
	virtual ~ioSelectShutDownAnnouncer(void);
};

