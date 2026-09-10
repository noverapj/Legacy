#pragma once

class ioINILoader;
class ioPlayStage;
class ioBaseChar;

class ioAbuseManager : public Singleton< ioAbuseManager >
{
public:
	enum AbuseType
	{
		AT_KEY,
		AT_DAMAGE,
	};

protected:
	CEncrypt<DWORD> m_dwKeyInputTimer;
	CEncrypt<DWORD> m_dwMaxKeyInputTime;
	CEncrypt<DWORD> m_dwOpenSpaceMaxKeyInputTime;

	CEncrypt<DWORD> m_dwDamageTimer;
	CEncrypt<DWORD> m_dwMaxDamageTime;

	CEncrypt<DWORD> m_dwCurAbuseTimer;
	CEncrypt<bool>  m_bAbuseSend;

protected:
	void CheckAbuseKey( ioPlayStage *pPlayStage );
	void CheckAbuseDamage( ioPlayStage *pPlayStage );
	bool IsAbuseNoCheck( ioPlayStage *pPlayStage );

public:
	static ioAbuseManager& GetSingleton();

public:
	void LoadINIInfo( ioINILoader &rkLoader );
	void Initialize();

public:
	void CheckInputKey( DWORD dwDirKey, DWORD dwActionKey );
	void AddDamage( int iDamage );

public:
	void ProcessLobbyStage();
	void ProcessPlayStage( ioPlayStage *pPlayStage );

public:
	ioAbuseManager();
	virtual ~ioAbuseManager();
};

#define g_AbuseMgr ioAbuseManager::GetSingleton()

