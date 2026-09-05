

#ifndef _BattleMode_h_
#define _BattleMode_h_

#include "Mode.h"
#include "BattleModeHelp.h"

class SP2Packet;
class User;

class BattleMode : public Mode
{
protected:
	BattleModeRecordList m_vRecordList;	

	int m_iRedKillPoint;
	int m_iBlueKillPoint;

	int m_iBlueDiePlayerCnt;
	int m_iRedDiePlayerCnt;
							
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

	void LoadAdditionalInfo();


	virtual void AddNewRecord( User *pUser );
	virtual void RemoveRecord( User *pUser, bool bRoomDestroy = false );

	//virtual void InitObjectGroupList();
	virtual const char* GetModeINIFileName() const;

	virtual void SetOrder( DWORD dwUserIDX, int iOrder );		// 2019-02-14 by bckim, 배틀 모드 추가
	
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
	virtual void OnEventSceneEnd( User *pSend, SP2Packet &rkPacket );
	virtual void SetRoundEndInfo( WinTeamType eWinTeam );
	virtual int GetCurTeamUserCnt( TeamType eTeam );

	virtual void FinalRoundPoint( ModeRecord *pRecord, bool bAbuseUser, float fTotalVictoriesRate, float fTotalConsecutivelyRate );

protected:
	void UpdateCurKillPoint( TeamType eTeam, int iPreCnt );
	void UpdateCurKillPointRate();

	void SetScore( TeamType eTeam );

	// 2019-02-14 by bckim, 배틀 모드 추가
public:
	virtual bool ProcessTCPPacket( User *pSend, SP2Packet &rkPacket );			// 2019-02-14 by bckim, 배틀 모드 추가 ProcessTCPPacket	

	virtual void OnDropDieUser( User *pDieUser, SP2Packet &rkPacket );
	virtual void OnWeaponDieUser( User *pDieUser, SP2Packet &rkPacket );
	virtual Vector3 GetRandomItemPos(ioItem *pItem = NULL);

	virtual void UpdateDieState( User *pDier );

	void BattleModeWarTimeStart( User *pSend, SP2Packet &rkPacket);
	void BattleModeTagWaitForRequest( User *pSend, SP2Packet &rkPacket);
	void BattleModeTagAccept( User *pSend, SP2Packet &rkPacket);
	void BattleModeEntryStart( User *pSend, SP2Packet &rkPacket);
	void BattleModeSetEntryStateFlag( User *pSend, SP2Packet &rkPacket);
	
	bool SetBattleModeUserHp( BattleModeRecord *pUser , int iSyncHP);					// HP 갱신
	
	bool GetBattleModeTagFlag( BattleModeRecord *pSyncUserRecord) { return pSyncUserRecord->m_bTagWaiting; }						// 테그 상태 
	bool SetBattleModeTagFalg( BattleModeRecord *pSyncUserRecord, bool bFlag);			// 테그 변경 
	void InitBattleModeTagFalg( BattleModeRecord *pSyncUserRecord) { pSyncUserRecord->m_bTagWaiting = false; } 
	int GetBattleModeOrder( BattleModeRecord *pSyncUserRecord) {return pSyncUserRecord->m_iBattle_Order;}						// order 
	
	// 난입 유저 상태 확인 
	int GetEntryActionCount( BattleModeRecord *pSyncUserRecord) {return pSyncUserRecord->m_iEntryActionCount;}
	int GetEntryActivation( BattleModeRecord *pSyncUserRecord) {return pSyncUserRecord->m_iEntryActivation;}
	BattleModeRecord* GetRecordOnBoardEntryActTeam( TeamType team_type );
	bool SetEntryActivation( BattleModeRecord *pSyncUserRecord, int iFlag);

	int GetEntryActionPermitCount() {return m_iEntryActionPermitCount;}

	void CheckTickTagAcceptTime();
	void CheckEntryDurationTime();
	void EntryDurationTimeOut();

	bool CheckTagAcceptTime(BattleModeRecord *pUserRecord);
	DWORD GetTagAcceptTime(BattleModeRecord *pUserRecord);
	void InitTagAcceptTime(BattleModeRecord *pUserRecord);

	bool SetUserState(BattleModeRecord *pSyncUserRecord, int state);
	int GetUserState(BattleModeRecord *pSyncUserRecord);


protected:

	DWORD m_dwDurationTime;
	DWORD m_dwTagAcceptHoldTime;			// 해당 시간내에는 재테그 주자 교체가 불가능  default 10초
	int m_iEntryActionPermitCount;			

	int	m_iBlueUserTagCount;
	int	m_iRedUserTagCount;

protected:
	BattleModeRecord* FindBattleModeRecord( const ioHashString &rkName );
	BattleModeRecord* FindBattleModeRecord( User *pUser );
	
	virtual void ProcessTime();

	void IncreaseTeamTagCount(TeamType teamtype);
	int GetTeamTagCount(TeamType teamtype);

	void SendNextRunnerInfo( User* pUser, bool bDieUser);

	// End. 2019-02-14 by bckim, 배틀 모드 추가
	
public:
	BattleMode( Room *pCreator );
	virtual ~BattleMode();
};

inline BattleMode* ToBattleMode( Mode *pMode )
{
	if( !pMode || pMode->GetModeType() != MT_BATTLE )
		return NULL;

	return static_cast< BattleMode* >( pMode );
}

#endif

