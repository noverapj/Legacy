#ifndef __ioShutDownManager_h__
#define __ioShutDownManager_h__

#include "../Util/Singleton.h"

class User;

class ioShutDownManager : public Singleton< ioShutDownManager >
{
protected:
	enum
	{
		TOTAL_MIN_PER_TIME = 60,
		TOTAL_MIN_PER_DAY  = 1440,
	};
protected:
	DWORD m_dwCurrentTime;
	DWORD m_dwStartTotalMinutes;
	DWORD m_dwEndTotalMinutes;
	DWORD m_dwBaseYears;
	bool  m_bActive;
	bool  m_bTemporaryUserShutDown;

public:
	void LoadINI();
	void Process();
	bool IsActive() const { return m_bActive; }
	bool CheckShutDownUser( const char *szBirthDate, short iYearType, bool bFormalityUser );

public:
	static ioShutDownManager &GetSingleton();

public:
	ioShutDownManager(void);
	virtual ~ioShutDownManager(void);
};

#define g_ShutDownMgr ioShutDownManager::GetSingleton()

#endif // __ioShutDownManager_h__