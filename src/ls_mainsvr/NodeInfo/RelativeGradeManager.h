#ifndef _RelativeGradeManager_h_
#define _RelativeGradeManager_h_

class ServerNode;
class RelativeGradeManager : public SuperParent
{
protected:
	static RelativeGradeManager *sg_Instance;

protected:
	enum
	{
		SUNDAY,
		MONDAY,
		TUESDAY,
		WEDNESDAY,
		THURSDAY,
		FRIDAY,
		SATURDAY,
	};

	int m_UpdateWeek;
	int m_UpdateHour;
	int m_UpdateMinute;
	DWORD m_dwUniqueCode;
	bool m_bEnableUpdate;

	int m_ReduceRate;

	// Test
	bool m_bTestProcess;
	CTime m_cTestLastUpdateTime;
	int m_iTestUpdateMin;

public:
	void LoadINIData();

public:
	void Process();

protected:
	bool EnableProcess();
	bool EnableTestProcess();

protected:
	void UpdateRealtiveGradeToAllServer();

public:
	static RelativeGradeManager &GetInstance();
	static void ReleaseInstance();

private:     	/* Singleton Class */
	RelativeGradeManager();
	virtual ~RelativeGradeManager();
};
#define g_RelativeGradeMgr RelativeGradeManager::GetInstance()
#endif