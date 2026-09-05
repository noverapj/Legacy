

#ifndef _FootballMode_h_
#define _FootballMode_h_

#include "Mode.h"
#include "FootballModeHelp.h"

class SP2Packet;
class User;

class FootballMode : public Mode
{
protected:
	FootballRecordList m_vRecordList;

	bool m_bRedGoal;
	bool m_bBlueGoal;

public:
	virtual void LoadINIValue();
	virtual void DestroyMode();

	virtual void AddNewRecord( User *pUser );
	virtual void RemoveRecord( User *pUser, bool bRoomDestroy = false );

protected:
	virtual void ProcessPlay();
	virtual void RestartMode();

	virtual void OnEventSceneEnd( User *pSend, SP2Packet &rkPacket );
	virtual void SetRoundEndInfo( WinTeamType eWinTeam );

	virtual int GetCurTeamUserCnt( TeamType eTeam );

public:
	virtual void InitObjectGroupList();

	virtual ModeType GetModeType() const;
	virtual void GetModeInfo( SP2Packet &rkPacket );
	virtual void GetModeHistory( SP2Packet &rkPacket );
	virtual void GetCharModeInfo( SP2Packet &rkPacket, const ioHashString &szName, bool bDieCheck = false );
	virtual int GetRecordCnt() const;

	virtual const char* GetModeINIFileName() const;
	virtual TeamType GetNextTeamType();
	virtual void CheckUserLeaveEnd();

	virtual bool CheckRoundJoin( User *pSend );
	virtual void CheckRoundEnd( bool bProcessCall );
	
public:
	virtual void UpdateRoundRecord();
	virtual int  GetUserKickVoteLimit( const ioHashString &szKickUserName );

public:
	virtual ModeRecord* FindModeRecord( const ioHashString &rkName );
	virtual ModeRecord* FindModeRecord( User *pUser );
	virtual ModeRecord* FindModeRecord( int iIdx );

protected:
	FootballRecord* FindFootballRecord( User *pUser );
	FootballRecord* FindFootballRecord( const ioHashString &rkName );

public:
	virtual bool ProcessTCPPacket( User *pSend, SP2Packet &rkPacket );

protected:
	void OnGoal( User *pUser, SP2Packet &rkPacket );

public:
	FootballMode( Room *pCreator );
	virtual ~FootballMode();
};

inline FootballMode* ToFootballMode( Mode *pMode )
{
	if( !pMode || pMode->GetModeType() != MT_FOOTBALL )
		return NULL;

	return static_cast< FootballMode* >( pMode );
}

#endif

