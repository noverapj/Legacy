#pragma once

class SP2Packet;
class ioAttendanceRewardManager : public Singleton< ioAttendanceRewardManager >
{
protected:
	enum
	{
		MAX_WEEKS   = 6,
		MAX_DAY		= 31,
	};

protected:
	struct TodayAttendanceReward
	{
		int m_iPresentType;
		int m_iValue1;
		int m_iValue2;
		int m_iPeriod;
		int m_iMent;

		TodayAttendanceReward()
		{
			m_iPresentType = 0;
			m_iValue1      = 0;
			m_iValue2      = 0;
			m_iPeriod	   = 0;
			m_iMent        = 0;
		}
	};
	typedef std::map< DWORD, TodayAttendanceReward> TodayAttendanceMap;

	struct AccrueAttendanceReward
	{
		int m_iAccureTerm;

		int m_iPresentType;
		int m_iValue1;
		int m_iValue2;
		int m_iPeriod;
		int m_iMent;		

		AccrueAttendanceReward()
		{
			m_iAccureTerm   = 0;

			m_iPresentType  = 0;
			m_iValue1       = 0;
			m_iValue2       = 0;
			m_iPeriod		= 0;
			m_iMent			= 0;
		}
	};
	typedef std::map< DWORD, AccrueAttendanceReward> AccrueAttendanceMap;

#ifdef _DEBUG
	ioHashStringVec m_szLogVec;
#endif

protected:
	TodayAttendanceMap  m_TodayAttendanceMap;
	AccrueAttendanceMap m_AccrueAttendanceMap;

public:
	void LoadINI();
	void LoadTodayAttance( ioINILoader& rkLoader, DWORD dwTtitle );
	void LoadAccrueAttance( ioINILoader& rkLoader, DWORD dwTtitle );

public:
	void Destroy();

public:
	bool IsTodayReward();

	void SendTodayAttanceReward( User* pUser, DWORD dwTodayDate, OUT int& iPresentType, OUT int& iValue1, OUT int& iValue2 );
	void SendAccurePeriodReward( User* pUser, int iYear, int iMonth, int iAccurePeriod, OUT int& iPresentType, OUT int& iValue1, OUT int& iValue2 );

public:
	static ioAttendanceRewardManager& GetSingleton();

public:
	ioAttendanceRewardManager();
	virtual ~ioAttendanceRewardManager();
};
#define g_AttendanceRewardMgr ioAttendanceRewardManager::GetSingleton()