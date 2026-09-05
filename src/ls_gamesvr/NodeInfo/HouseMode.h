#pragma once

#include "Mode.h"
#include "User.h"
#include "ModeHelp.h"

struct HouseModeRecord : public ModeRecord
{
};

typedef std::vector<HouseModeRecord> PersonalRecordList;

class HouseMode : public Mode
{
public:
	HouseMode( Room *pCreator );
	virtual ~HouseMode();

public:
	virtual void DestroyMode();

	virtual void AddNewRecord( User *pUser );
	virtual void RemoveRecord( User *pUser, bool bRoomDestroy );

	virtual void SetStartPosArray();
	virtual int GetCurTeamUserCnt( TeamType eTeam ){ return 0; }

protected:
	virtual void LoadRoundCtrlValue( ioINILoader &rkLoader );

protected:
	virtual void ProcessPlay();
	virtual void RestartMode(){}    // 무한 플레이

	virtual void SetRoundEndInfo( WinTeamType eWinTeam ){} // 결과 없음

	void AddTeamType( TeamType eTeam );
	void RemoveTeamType( TeamType eTeam );

public:
	//virtual bool OnModeChangeChar( User *pSend, int iCharArray, bool bWait, int iSelectCharArray, DWORD dwCharChangeIndex );
	virtual	void OnModeJoinLockUpdate( User *pSend, bool bJoinLock );
	//virtual void OnModeLogoutAlarm( const ioHashString &rkMasterName );

public:
	virtual ModeType GetModeType() const;
	virtual void GetModeInfo( SP2Packet &rkPacket );
	virtual void GetCharModeInfo( SP2Packet &rkPacket, const ioHashString &szName, bool bDieCheck = false );
	virtual int GetRecordCnt() const;

	virtual const char* GetModeINIFileName() const;
	virtual TeamType GetNextTeamType();
	virtual void CheckRoundEnd( bool bProcessCall ){}    // 무한 플레이

	virtual void UpdateRoundRecord();

	virtual bool IsTimeClose(){ return false; }
	virtual void UpdateUserDieTime( User *pDier );

public:
	//virtual void InitObjectGroupList();
	//virtual Vector3 GetRandomItemPos(ioItem *pItem = NULL);

	virtual int  GetUserKickVoteLimit( const ioHashString &szKickUserName );

public:
	void SetCharState( DWORD dwState );
	void SetHeadquartersMaster( const ioHashString &rkName ){ m_szMasterName = rkName; }
	void ReSetCreateCharacter( User *pUser );
	void CreateCharacter( User *pUser );
	void InsertCharacter( User *pUser, ioCharacter *pCharacter );

public:
	bool IsMasterJoin();
	const ioHashString &GetMasterName(){ return m_szMasterName; }
	DWORD GetMasterIndex() { return m_dwMasterIndex; }

	bool IsHeadquartersJoinLock(){ return m_bJoinLock; }

	void MasterExistHQ(User* pUser);

	void SetHouseMaster( const ioHashString &rkName, const DWORD dwMasterIndex );

public:
	virtual ModeRecord* FindModeRecord( const ioHashString &rkName );
	virtual ModeRecord* FindModeRecord( User *pUser );
	virtual ModeRecord* FindModeRecord( int iIdx );

protected:
	HouseModeRecord* FindHQRecord( User *pUser );
	HouseModeRecord* FindHQRecord( const ioHashString &rkName );
	HouseModeRecord* FindHQRecordByUserID( const ioHashString &rkUserID );
	HouseModeRecord* FindCharacterInfo( const ioHashString &rkName );

public:
	virtual bool ProcessTCPPacket( User *pSend, SP2Packet &rkPacket );

public:
	void SetCreateTime();
	BOOL IsDeleteTime();

protected:
	void OnUserInvite( User *pSend, SP2Packet &rkPacket );

protected:
	PersonalRecordList m_vRecordList;
	
	bool m_bJoinLock;

	IntVec m_TeamList;
	IntVec m_TeamPosArray;

	ioHashString	m_szMasterName;
	DWORD			m_dwMasterIndex;
	DWORD			m_dwCreateTime;
};

inline HouseMode* ToHouseMode( Mode *pMode )
{
	if( !pMode || pMode->GetModeType() != MT_HOUSE )
		return NULL;

	return static_cast< HouseMode* >( pMode );
}