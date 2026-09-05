

#ifndef _TeamSurvivalMode_h_
#define _TeamSurvivalMode_h_

#include "Mode.h"
#include "TeamSurvivalModeHelp.h"

class SP2Packet;
class User;

class TeamSurvivalMode : public Mode
{
protected:
	TeamSurvivalRecordList m_vRecordList;

	int m_iRedKillPoint;
	int m_iBlueKillPoint;

	float m_fRedKillPointRate;
	float m_fBlueKillPointRate;

	float m_fWinScoreConstant;

	float m_fScoreGapConst;
	float m_fScoreGapRateConst;
	float m_fLadderScoreGapConst;
	float m_fLadderScoreGapRateConst;


protected:
	virtual void ProcessPlay();
	virtual void RestartMode();

	virtual void LoadRoundCtrlValue( ioINILoader &rkLoader );

	virtual void SendRoundResult( WinTeamType eWinTeam );

public:
	virtual void LoadINIValue();
	virtual void DestroyMode();

	virtual void AddNewRecord( User *pUser );
	virtual void RemoveRecord( User *pUser, bool bRoomDestroy = false );

	virtual void InitObjectGroupList();
	virtual const char* GetModeINIFileName() const;

public:
	virtual ModeType GetModeType() const;
	virtual void GetModeInfo( SP2Packet &rkPacket );
	virtual void GetExtraModeInfo( SP2Packet &rkPacket );
	virtual void GetModeHistory( SP2Packet &rkPacket );

	virtual void GetCharModeInfo( SP2Packet &rkPacket, const ioHashString &rkName, bool bDieCheck = false );

	virtual bool CheckRoundJoin( User *pSend );
	
	virtual void CheckUserLeaveEnd();
	virtual void CheckRoundEnd( bool bProcessCall );

	virtual void UpdateDropDieRecord( User *pDier, const ioHashString &szAttacker, const ioHashString &szBestAttacker );
	virtual void UpdateWeaponDieRecord( User *pDier, const ioHashString &szAttacker, const ioHashString &szBestAttacker );
	virtual void UpdateRoundRecord();

	virtual int GetRecordCnt() const;

	virtual TeamType GetNextTeamType();
	virtual float GetResultScoreGapValue( bool bLadderPoint, TeamType eWinTeam );
	virtual float GetUserCorrection( TeamType eWinTeam, float fRoundPoint, float fScoreGap );

public:
	virtual int  GetUserKickVoteLimit( const ioHashString &szKickUserName );

public:
	virtual ModeRecord* FindModeRecord( const ioHashString &rkName );
	virtual ModeRecord* FindModeRecord( User *pUser );
	virtual ModeRecord* FindModeRecord( int iIdx );

protected:
	TeamSurvivalRecord* FindTeamSurvivalRecord( const ioHashString &rkName );
	TeamSurvivalRecord* FindTeamSurvivalRecord( User *pUser );

protected:
	virtual void OnEventSceneEnd( User *pSend, SP2Packet &rkPacket );
	virtual void SetRoundEndInfo( WinTeamType eWinTeam );
	virtual int GetCurTeamUserCnt( TeamType eTeam );

	virtual void FinalRoundPoint( ModeRecord *pRecord, bool bAbuseUser, float fTotalVictoriesRate, float fTotalConsecutivelyRate );

protected:
	void UpdateCurKillPoint( TeamType eTeam, int iPreCnt );
	void UpdateCurKillPointRate();

	TeamType CheckKillPoint();

	void SetScore( TeamType eTeam );

public:
	TeamSurvivalMode( Room *pCreator );
	virtual ~TeamSurvivalMode();
};

inline TeamSurvivalMode* ToTeamSurvivalMode( Mode *pMode )
{
	if( !pMode || pMode->GetModeType() != MT_TEAM_SURVIVAL )
		return NULL;

	return static_cast< TeamSurvivalMode* >( pMode );
}

#endif

