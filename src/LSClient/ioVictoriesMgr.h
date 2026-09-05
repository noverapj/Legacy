#pragma once

class ioVictoriesMgr  : public Singleton< ioVictoriesMgr >
{
protected:
	struct VictoriesData
	{
		ioHashString m_szUserID;
		int m_iVictories;
		VictoriesData()
		{
			m_iVictories = 0;
		}
	};
	typedef std::vector< VictoriesData > vVictoriesData;
	vVictoriesData m_VictoriesData;
	
protected:
	ioVictoriesMgr::VictoriesData &FindList( const ioHashString &szUserID );

public:
	void Initialize();

public:
	void EnterUser( const ioHashString &szUserID );
	void LeaveUser( const ioHashString &szUserID );

public:
	int  GetVictories( const ioHashString &szUserID );
	void SetVictories( const ioHashString &szUserID, bool bWinTeam );
	void SetVictories( const ioHashString &szUserID, int iVictories );

public:
	static ioVictoriesMgr& GetSingleton();

public:
	ioVictoriesMgr();
	virtual ~ioVictoriesMgr();
};

#define g_VictoriesMgr ioVictoriesMgr::GetSingleton()

