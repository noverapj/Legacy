

#ifndef _SuccessionMode_h_
#define _SuccessionMode_h_

#include "Mode.h"
#include "SuccessionModeHelp.h"

class SP2Packet;
class User;

class SuccessionMode : public Mode
{
protected:
	SuccessionRecordList m_vRecordList;

	int m_iRedCatchBluePlayer;
	int m_iBlueCatchRedPlayer;
	
	int m_iBasePoint;
	int m_iLeaveWinPoint;
	int m_iLeaveLosePoint;

	bool m_bReserveRevengeMatch;

	DWORD m_dwBlueUserIndex;
	DWORD m_dwRedUserIndex;

public:
	virtual void InitMode();
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
	virtual void CheckLeaveMatch( User* pUser );

	void SetTeam( DWORD dwBlueUserIndex, DWORD dwRedUserIndex );
	TeamType GetTeamTypeByUserIndex( DWORD dwUserIndex );

public:
	virtual void UpdateRoundRecord();

public:
	virtual int  GetUserKickVoteLimit( const ioHashString &szKickUserName );

public:
	virtual ModeRecord* FindModeRecord( const ioHashString &rkName );
	virtual ModeRecord* FindModeRecord( User *pUser );
	virtual ModeRecord* FindModeRecord( int iIdx );

protected:
	SuccessionRecord* FindSuccessionRecord( const ioHashString &rkName );
	SuccessionRecord* FindSuccessionRecord( User *pUser );

protected:
	virtual void OnEventSceneEnd( User *pSend, SP2Packet &rkPacket );
	virtual void SetRoundEndInfo( WinTeamType eWinTeam );
	virtual void SendRoundResult( WinTeamType eWinTeam );
	virtual int GetCurTeamUserCnt( TeamType eTeam );

public:
	virtual bool ProcessTCPPacket( User *pSend, SP2Packet &rkPacket );

protected:
	void OnPrisonerEscape( User *pUser, SP2Packet &rkPacket );
	void OnPrisonerDrop( User *pUser, SP2Packet &rkPacket );
	void OnPrisonerMode( User *pUser, SP2Packet &rkPacket );
	void OnRequestRevenge( User *pUser, SP2Packet &rkPacket );

	virtual void FinalRoundProcess();
	virtual void FinalRoundResult( ModeRecord *pRecord, DWORD dwServerDate );
	void CalculateMMR();

public:
	bool IsReserveRevengeMatch() { return m_bReserveRevengeMatch; }
	void SetReserveRevengeMatch( bool bRevenge ) { m_bReserveRevengeMatch = bRevenge; }

public:
	SuccessionMode( Room *pCreator );
	virtual ~SuccessionMode();
};

inline SuccessionMode* ToSuccessionMode( Mode *pMode )
{
	if( !pMode || pMode->GetModeType() != MT_SUCCESSION )
		return NULL;

	return static_cast< SuccessionMode* >( pMode );
}

#endif

