#pragma once


class ioPlayBonusMgr  : public Singleton< ioPlayBonusMgr >
{
protected:
	struct PlayBonusData
	{
		bool m_bPCRoomBonus;
		bool m_bGuildBonus;
		bool m_bFriendBonus;

		PlayBonusData()
		{
			Init();
		}

		void Init()
		{
			m_bPCRoomBonus = m_bGuildBonus = m_bFriendBonus = false;
		}
	};
	typedef std::map< ioHashString, PlayBonusData > PlayBonusMap;
	PlayBonusMap  m_PlayBonusMap;	

public:
	void Initialize();

public:
	void EnterUser( const ioHashString &szUserID );
	void LeaveUser( const ioHashString &szUserID );

public:
	void SetUserPlayBonus( const ioHashString &rkName, bool &rkPCRoomBonus, bool &rkGuildBonus, bool &rkFriendBonus );
	void GetUserPlayBonus( const ioHashString &rkName, bool &rkPCRoomBonus, bool &rkGuildBonus, bool &rkFriendBonus );

public:
	static ioPlayBonusMgr& GetSingleton();

public:
	ioPlayBonusMgr();
	virtual ~ioPlayBonusMgr();
};

#define g_PlayBonusMgr ioPlayBonusMgr::GetSingleton()

