
#ifndef _SurvivalMode_h_
#define _SurvivalMode_h_

#include "Mode.h"
#include "SurvivalModeHelp.h"

class SP2Packet;
class User;

class SurvivalMode : public Mode
{
protected:
	SurvivalRecordList m_vRecordList;

	IntVec m_SingleTeamList;
	IntVec m_SingleTeamPosArray;

public:
	virtual void DestroyMode();

	virtual void AddNewRecord( User *pUser );
	virtual void RemoveRecord( User *pUser, bool bRoomDestroy );

	virtual void SetStartPosArray();

	virtual void LoadINIValue();

protected:
	virtual void LoadRoundCtrlValue( ioINILoader &rkLoader );

protected:
	virtual void ProcessPlay();
	virtual void RestartMode(){}    // 1라운드 플레이

	virtual void SetRoundEndInfo( WinTeamType eWinTeam );

	void AddTeamType( TeamType eTeam );
	void RemoveTeamType( TeamType eTeam );

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

public:
	virtual int  GetUserKickVoteLimit( const ioHashString &szKickUserName );

public:
	virtual ModeRecord* FindModeRecord( const ioHashString &rkName );
	virtual ModeRecord* FindModeRecord( User *pUser );
	virtual ModeRecord* FindModeRecord( int iIdx );

protected:
	SurvivalRecord* FindSurvivalRecord( User *pUser );
	SurvivalRecord* FindSurvivalRecord( const ioHashString &rkName );
	SurvivalRecord* FindSurvivalRecordByUserID( const ioHashString &rkUserID );

public:
	SurvivalMode( Room *pCreator );
	virtual ~SurvivalMode();
};

inline SurvivalMode* ToSurvivalMode( Mode *pMode )
{
	if( !pMode || pMode->GetModeType() != MT_SURVIVAL )
		return NULL;

	return static_cast< SurvivalMode* >( pMode );
}

#endif 
