#pragma once

class ioUserSelectShutDown
{
protected:
	static int m_iEarlyShutDownMinutes;

	bool  m_bShutDownUser;
	CTime m_ShutDownTime;

public:
	static void LoadINI();
	static void SetEarlyShutDownMinutes( int iEarlyShutDownMinutes ) { m_iEarlyShutDownMinutes = iEarlyShutDownMinutes; }

public:
	void SetSelectShutDown( int iShutDown, const DBTIMESTAMP &rkDts );
	bool IsShutDown( CTime CurrentTime );

	CTime GetShutDownTime() const { return m_ShutDownTime; }
	void SetShutDownTime( __int64 iShutDownTime) { m_ShutDownTime = iShutDownTime; }

	bool IsShutDownUser() const { return m_bShutDownUser; }
    void SetShutDownUser(bool bShutDownUser) { m_bShutDownUser = bShutDownUser; }

	void Initialize();

public:
	ioUserSelectShutDown(void);
	virtual ~ioUserSelectShutDown(void);
};