#pragma once

#ifndef ANTIHACK
class ioRelayGroupInfoMgr : public SuperParent
{
public:
	ioRelayGroupInfoMgr(void);
	virtual ~ioRelayGroupInfoMgr(void);

public:
	void InitData(int iSeedRoom);

public:
	void InsertRoom( RelayHeader* pRelayHeader );
	void RemoveRoom( RelayHeader* pRelayHeader );

	BOOL SendRelayPacket( DWORD dwUserIndex, SP2Packet& kPacket );

protected:
	BOOL CreateRelayGroup(DWORD dwRoomIndex, DWORD dwUserIndex, int iPort, char* szIP);
	RelayGroup* RemoveRelayGroupByRoom( const DWORD dwRoomIndex );
	RelayGroup* GetRelayGroupByRoom( const DWORD dwRoomIndex );

protected:
	BOOL InsertUser( const DWORD dwRoomIndex, const DWORD dwUserIndex, const int iPort, const char* szIP );
	BOOL RemoveUser( const DWORD dwRoomIndex, const DWORD dwUserIndex );
	BOOL GetRelayUserList( const DWORD dwUserIndex, RelayGroup::RelayGroups& vGroupUsers );

protected:
	typedef ATL::CAtlMap<DWORD,RelayGroup*> RELAY_GROUP_MAP;
	typedef RELAY_GROUP_MAP::CPair* PGROUPRESULT;

	MemPooler<RelayGroup> m_vRelayGroupPool;
	RELAY_GROUP_MAP m_vRelayGroups;
};

typedef cSingleton<ioRelayGroupInfoMgr> S_IORELAYGROUPINFOMGR;
#define g_RelayGroupInfoMgr S_IORELAYGROUPINFOMGR::GetInstance()

//////////////////////////////////////////////////////////////////////////
#else

struct SkillInfo;
struct sWeaponInfo;
class ioRelayGroupInfoMgr : public SuperParent
{
public:
	ioRelayGroupInfoMgr(void);
	virtual ~ioRelayGroupInfoMgr(void);

	DWORD	m_dwAntiWaitTime;
	float	m_fAntiErrorRate;

	//무적 치트 관련
	DWORD	m_dwPenguinCount;
	DWORD	m_dwKickCount;
	int		m_iTimeDecrease;

	//스킬 치트 관련
	DWORD	m_dwSkillPenguinCount;
	DWORD	m_dwSkillKickCount;
	int		m_iSkillTimeDecrease;

	bool	m_bRoomLog;
	bool	m_bIsWrite;

	std::vector<int> m_vecExceptID;

public:
	void InitData( int iSeedRoom, DWORD dwAntiWaitTime, float fAntiErrorRate,
		int iPenguinCount, int iKickCount, int iTimeDecrease, int iSkillHackCount, int iSkillKickCount, int iSkillTimeDrease, std::vector<int>& vecExceptList );
	void OnRUDPRoomLog() { m_bRoomLog = true; }
	bool IsRoomLog() { return m_bRoomLog; }

	void SetWriteLog( bool bWrite ) { m_bIsWrite = bWrite; }
	bool IsWriteLog(){ return m_bIsWrite; }


public:
	void InsertRoom( RelayHeader* pRelayHeader );
	void RemoveRoom( RelayHeader* pRelayHeader );

	BOOL BroadcastPacket( DWORD dwUserIndex, SP2Packet& kPacket );
	int  GetModeType( DWORD dwRoomIndex );
	int  GetModeStyle( DWORD dwRoomIndex );

	// 안티핵 서버 전까지 사용할 용도로 스킬정보를 UserData 쪽에 넣어줌
	bool CheckControlSeed( DWORD dwRoomIndex, DWORD dwUserIndex, DWORD dwSeed, DWORD dwHostTime, bool bUser );
	void NotUseControlSeed( DWORD dwRoomIndex, DWORD dwUserIndex, SP2Packet& rkPacket );

	//패킷 받음
	void OnSkillUse( DWORD dwRoomIndex, DWORD dwUserIndex, DWORD dwHostTime, ioHashString& hsSkillName, int iLevel, float fGauge );
	void OnSkillExtraInfo( DWORD dwRoomIndex, DWORD dwUserIndex, DWORD dwHostTime, int iSize, DWORD* dwUserIndexs );
	void OnSkillAntihack( DWORD dwRoomIndex, DWORD dwUserIndex, DWORD dwHostTime, int nSize, int* iSlots, DWORD* dwLatedTime );

	void OnAntiHackHit( DWORD dwRoomIndex, DWORD dwUserIndex, DWORD dwNPCIndex, DWORD dwHostTime, DWORD dwHitUserIndex, DWORD dwWeaponIndex, BYTE eAttackType );
	void OnAntiHackWounded( DWORD dwRoomIndex, DWORD dwUserIndex, DWORD dwNPCIndex, DWORD dwHostTime, DWORD dwAttackerIndex, DWORD dwWeaponIndex, BYTE eAttackType );
	void OnAntiHackDefence( DWORD dwRoomIndex, DWORD dwUserIndex, DWORD dwNPCIndex, DWORD dwHostTime, DWORD dwAttackerIndex, DWORD dwWeaponIndex, BYTE eAttackType );

	void OnAnthHackBulletInfo( DWORD dwRoomIndex, DWORD dwUserIndex, DWORD dwNPCIndex, DWORD dwHostTime, DWORD dwItemCode, DWORD dwWasteCount, DWORD dwWasteTime );
	void OnAnthHackReloadBullet( DWORD dwRoomIndex, DWORD dwUserIndex, DWORD dwNPCIndex, DWORD dwHostTime, DWORD dwItemCode );

	//relaygroup이 정보 들고 있게

	std::map<DWORD,sWeaponInfo*> m_mapWeaponInfo;
	sWeaponInfo* GetWeaponInfo( DWORD dwItemCode );
	void CheckBulletData( UserData* pUser, DWORD dwItemCode );

	//안티핵 히트 검증
	void CheckAntiHackData();
	void ReloadAntiHack( float fAntiErrorRate, int iAntiWaitTime, int iPenguinCount, int iKickCount,
		int iTimeDecrease, int iSkillHackCount, int iSkillKickCount, int iSkillTimeDecrease, int* iArray );

	bool IsExceptID( int iSkillid );

	//업데이트 관련
	void UpdateRelayGroupWin( DWORD dwRoomIndex, int iRedTeamWinCnt, int iBlueTeamWindCnt );
	void UpdateRelayGroupScore( DWORD dwRoomIndex, int iTeamType );
	void UpdateUserSPPotion( DWORD dwRoomIndex, DWORD dwUserIndex );
	void UpdateDieState( DWORD dwRoomIndex, DWORD dwUserIndex, BOOL bState );

	void AddPenaltyPoint( UserData* pUser, int iPenaltyPoint );
	void AddPenaltySkill( UserData* pUser, ioHashString& hsSkillName, float fTime1, float fTime2 );

	//로직(스킬)
	void OnSkillAstRandomGenerate( UserData* pUser, SkillInfo* pSkillInfo, float fRate, float fUseTimeDiff );
	void OnSkillBasic( UserData* pUser, ioHashString& hsSkillName, float fIncMaxCoolTime, float fUseTimeDiff, float fErrorRate );
	void OnSkillBuff( UserData* pUser, SkillInfo* pSkillInfo, float fIncMaxCoolTime, float fUseTimeDiff, float fErrorRate );
	void OnSkillCount( UserData* pUser, SkillInfo* pSkillInfo, DWORD dwCurTime );

	//스킬 재작업
	bool IsCheckSkillTime( UserData* pUserData, SkillInfo* pSkillInfo );
	void CheckSkillTime( UserData* pUserData, RelayGroup* pRelayGroup, SkillInfo* pSkillInfo,  int iLevel, DWORD dwHostTime );
	void OnAfterSkill( UserData* pUserData, SkillInfo* pSkillInfo, DWORD dwHostTime );


	DWORD GetLastSkillTime( UserData* pUserData, SkillInfo* pSkillInfo );
	void SetLastSkillTime( UserData* pUserData, SkillInfo* pSkillInfo, DWORD dwHostTime );

protected:
	BOOL CreateRelayGroup( InsertData& rkData );
	RelayGroup* RemoveRelayGroupByRoom( const DWORD dwRoomIndex );
	RelayGroup* GetRelayGroupByRoom( const DWORD dwRoomIndex );

protected:
	BOOL InsertUser( InsertData& rkData );
	BOOL RemoveUser( const DWORD dwRoomIndex, const DWORD dwUserIndex );
	RelayGroup::RelayGroups* GetRelayUserList( const DWORD dwUserIndex );

protected:
	typedef ATL::CAtlMap<DWORD,RelayGroup*> RELAY_GROUP_MAP;
	typedef RELAY_GROUP_MAP::CPair* PGROUPRESULT;

	MemPooler<RelayGroup> m_vRelayGroupPool;
	RELAY_GROUP_MAP m_vRelayGroups;
};

typedef cSingleton<ioRelayGroupInfoMgr> S_IORELAYGROUPINFOMGR;
#endif