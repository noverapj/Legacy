

#ifndef _HeroMatchMode_h_
#define _HeroMatchMode_h_

#include "Mode.h"
#include "HeroMatchModeHelp.h"

class SP2Packet;
class User;
class HeroMatchMode : public Mode
{
protected:
	HeroMatchRecordList m_vRecordList;
	
public:
	virtual void LoadINIValue();
	virtual void DestroyMode();

	virtual void AddNewRecord( User *pUser );
	virtual void RemoveRecord( User *pUser, bool bRoomDestroy = false );

protected:
	virtual void ProcessPlay();
	virtual void RestartMode();

public:
	virtual void UpdateDieState( User *pDier );
	virtual void UpdateUserDieTime( User *pDier );

public:
	virtual ModeType GetModeType() const;
	virtual void GetModeInfo( SP2Packet &rkPacket );
	virtual void GetModeHistory( SP2Packet &rkPacket );
	virtual void GetCharModeInfo( SP2Packet &rkPacket, const ioHashString &rkName, bool bDieCheck = false );
	virtual int GetRecordCnt() const;
	
	virtual const char* GetModeINIFileName() const;
	virtual TeamType GetNextTeamType();
	virtual void CheckRoundEnd( bool bProcessCall );

	virtual void InitObjectGroupList();

	virtual bool CheckRoundJoin( User *pSend );

public:
	virtual int GetWinLoseTiePoint( TeamType eMyTeam, TeamType eWinTeam, float fPlayTimePer );

public:
	virtual void UpdateRoundRecord();

public:
	virtual int  GetUserKickVoteLimit( const ioHashString &szKickUserName );

public:
	virtual ModeRecord* FindModeRecord( const ioHashString &rkName );
	virtual ModeRecord* FindModeRecord( User *pUser );
	virtual ModeRecord* FindModeRecord( int iIdx );

protected:
	HeroMatchRecord* FindHeroMatchRecord( const ioHashString &rkName );
	HeroMatchRecord* FindHeroMatchRecord( User *pUser );

protected:
	virtual void OnEventSceneEnd( User *pSend, SP2Packet &rkPacket );
	virtual void SetRoundEndInfo( WinTeamType eWinTeam );
	virtual int GetCurTeamUserCnt( TeamType eTeam );
	bool IsTeamUserDie( TeamType eTeam ); 

public:
	virtual bool ProcessTCPPacket( User *pSend, SP2Packet &rkPacket );

public:
	HeroMatchMode( Room *pCreator );
	virtual ~HeroMatchMode();
};

inline HeroMatchMode* ToHeroMatchMode( Mode *pMode )
{
	if( !pMode || pMode->GetModeType() != MT_HEROMATCH )
		return NULL;

	return static_cast< HeroMatchMode* >( pMode );
}

#endif

