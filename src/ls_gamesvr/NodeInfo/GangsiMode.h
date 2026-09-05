
#ifndef _GangsiMode_h_
#define _GangsiMode_h_

#include "Mode.h"
#include "GangsiModeHelp.h"

class SP2Packet;
class User;

class GangsiMode : public Mode
{
protected:
	GangsiRecordList m_vRecordList;

	IntVec m_HostGangsiItemCode;
	IntVec m_InfectionGangsiItemCode;
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
	virtual void ProcessReady();
	virtual void ProcessPlay();
	virtual void RestartMode(){}    // 1라운드 플레이

	virtual void SetRoundEndInfo( WinTeamType eWinTeam );

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
	virtual void UpdateWeaponDieRecord( User *pDier, const ioHashString &szAttacker, const ioHashString &szBestAttacker );
	virtual void UpdateUserDieNextProcess( User *pDier, const ioHashString &szAttacker, const ioHashString &szBestAttacker );

public:
	virtual int  GetUserKickVoteLimit( const ioHashString &szKickUserName );

public:
	virtual ModeRecord* FindModeRecord( const ioHashString &rkName );
	virtual ModeRecord* FindModeRecord( User *pUser );
	virtual ModeRecord* FindModeRecord( int iIdx );

protected:
	GangsiRecord* FindGangsiRecord( User *pUser );
	GangsiRecord* FindGangsiRecord( const ioHashString &rkName );
	GangsiRecord* FindGangsiRecordByUserID( const ioHashString &rkUserID );

public:
	virtual void OnEventSceneEnd( User *pSend, SP2Packet &rkPacket );	
	virtual void OnWeaponDieUser( User *pDieUser, SP2Packet &rkPacket );

public:
	void CheckGangsiToRandomGangsi();

public:
	int GetGangsiItem( int iSlot );
	void ChangeGangsiUser( User *pUser );


public:
	GangsiMode( Room *pCreator );
	virtual ~GangsiMode();
};

inline GangsiMode* ToGangsiMode( Mode *pMode )
{
	if( !pMode || pMode->GetModeType() != MT_GANGSI )
		return NULL;

	return static_cast< GangsiMode* >( pMode );
}

#endif 
