
#ifndef _BossMode_h_
#define _BossMode_h_

#include "Mode.h"
#include "BossModeHelp.h"

class SP2Packet;
class User;

class BossMode : public Mode
{
protected:
	BossRecordList m_vRecordList;

	IntVec m_SingleTeamPosArray;

protected:
	int    m_iBossMaxLevel;

public:
	virtual void DestroyMode();

	virtual void AddNewRecord( User *pUser );
	virtual void RemoveRecord( User *pUser, bool bRoomDestroy );

	virtual void SetStartPosArray();

	virtual void LoadINIValue();

protected:
	virtual void LoadRoundCtrlValue( ioINILoader &rkLoader );

protected:
	virtual void ProcessReady();
	virtual void ProcessPlay();
	virtual void RestartMode(){}    // 1라운드 플레이

	virtual void SetRoundEndInfo( WinTeamType eWinTeam );

	void ChangeBossUser( User *pDier, const ioHashString &szNextBossUser );
	void CheckBossToRandomBoss();

	virtual int GetCurTeamUserCnt( TeamType eTeam );

public:
	virtual ModeType GetModeType() const;
	virtual void GetModeInfo( SP2Packet &rkPacket );
	virtual void GetCharModeInfo( SP2Packet &rkPacket, const ioHashString &szName, bool bDieCheck = false );
	virtual int GetRecordCnt() const;

	virtual void InitObjectGroupList();
	virtual const char* GetModeINIFileName() const;
	
	virtual TeamType GetNextTeamType();
	virtual void CheckUserLeaveEnd();
	virtual void CheckRoundEnd( bool bProcessCall );

	virtual void UpdateRoundRecord();

	virtual void UpdateUserDieTime( User *pDier );
	virtual void UpdateDropDieRecord( User *pDier, const ioHashString &szAttacker, const ioHashString &szBestAttacker );
	virtual void UpdateUserDieNextProcess( User *pDier, const ioHashString &szAttacker, const ioHashString &szBestAttacker );

public:
	virtual int  GetUserKickVoteLimit( const ioHashString &szKickUserName );

public:
	virtual ModeRecord* FindModeRecord( const ioHashString &rkName );
	virtual ModeRecord* FindModeRecord( User *pUser );
	virtual ModeRecord* FindModeRecord( int iIdx );

protected:
	BossRecord* FindBossRecord( User *pUser );
	BossRecord* FindBossRecord( const ioHashString &rkName );
	BossRecord* FindBossRecordByUserID( const ioHashString &rkUserID );

public:
	virtual void OnEventSceneEnd( User *pSend, SP2Packet &rkPacket );	

public:
	BossMode( Room *pCreator );
	virtual ~BossMode();
};

inline BossMode* ToBossMode( Mode *pMode )
{
	if( !pMode || pMode->GetModeType() != MT_BOSS )
		return NULL;

	return static_cast< BossMode* >( pMode );
}

#endif 
