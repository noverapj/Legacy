

#ifndef _UnderwearMode_h_
#define _UnderwearMode_h_

#include "Mode.h"
#include "CatchModeHelp.h"

class SP2Packet;
class User;

class UnderwearMode : public Mode
{
protected:
	CatchRecordList m_vRecordList;

	int m_iRedCatchBluePlayer;
	int m_iBlueCatchRedPlayer;
	
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
	virtual void UpdateRoundRecord();

public:
	virtual int  GetUserKickVoteLimit( const ioHashString &szKickUserName );

public:
	virtual ModeRecord* FindModeRecord( const ioHashString &rkName );
	virtual ModeRecord* FindModeRecord( User *pUser );
	virtual ModeRecord* FindModeRecord( int iIdx );

protected:
	CatchRecord* FindCatchRecord( const ioHashString &rkName );
	CatchRecord* FindCatchRecord( User *pUser );

protected:
	virtual void OnEventSceneEnd( User *pSend, SP2Packet &rkPacket );
	virtual void SetRoundEndInfo( WinTeamType eWinTeam );
	virtual int GetCurTeamUserCnt( TeamType eTeam );

public:
	virtual bool ProcessTCPPacket( User *pSend, SP2Packet &rkPacket );

protected:
	void OnPrisonerEscape( User *pUser, SP2Packet &rkPacket );
	void OnPrisonerDrop( User *pUser, SP2Packet &rkPacket );
	void OnPrisonerMode( User *pUser, SP2Packet &rkPacket );

public:
	UnderwearMode( Room *pCreator );
	virtual ~UnderwearMode();
};

inline UnderwearMode* ToUnderwearMode( Mode *pMode )
{
	if( !pMode || pMode->GetModeType() != MT_UNDERWEAR )
		return NULL;

	return static_cast< UnderwearMode* >( pMode );
}

#endif

