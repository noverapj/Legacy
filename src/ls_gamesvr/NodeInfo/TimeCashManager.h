#pragma once

struct TimeCashInfo
{
	DWORD m_dwCode;
	int m_iEvenID;
	int	m_iFirstCash;
	int m_iContinueCash;
	int m_iCashExpire;
	
	TimeCashInfo()
	{
		m_dwCode		= 0;
		m_iEvenID		= 0;
		m_iFirstCash	= 0;
		m_iContinueCash	= 0;
		m_iCashExpire	= 0;
	};
};

class TimeCashManager	: public Singleton< TimeCashManager >
{
public:
	TimeCashManager();
	virtual ~TimeCashManager();

	void Init();
	void Destroy();

public:
	BOOL LoadINI(BOOL bReload = FALSE);

public:
	static TimeCashManager& GetSingleton();

public:
	DWORD GetStandardDate(DWORD dwDate);

	void GetTimeCashInfo(const DWORD dwCode, TimeCashInfo& stInfo);

	int GetRenewalHour() { return m_iRenewalHour; }

protected:
	typedef std::vector<TimeCashInfo> vCashTable;

protected:
	vCashTable m_vCashTable;
	int m_iRenewalHour;
};

#define g_TimeCashMgr TimeCashManager::GetSingleton()