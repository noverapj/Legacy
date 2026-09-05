#ifndef _TeamSurvivalAIMode_h_
#define _TeamSurvivalAIMode_h_

#include "Mode.h"
#include "TeamSurvivalModeHelp.h"

class SP2Packet;
class User;

class TeamSurvivalAIMode : public Mode, public BoostPooler<TeamSurvivalAIMode>
{
protected:
	TeamSurvivalAIRecordList m_vRecordList;

	int m_iRedKillPoint;
	int m_iBlueKillPoint;

	float m_fRedKillPointRate;
	float m_fBlueKillPointRate;

	float m_fWinScoreConstant;

	float m_fScoreGapConst;
	float m_fScoreGapRateConst;
	float m_fLadderScoreGapConst;
	float m_fLadderScoreGapRateConst;

	//난이도 - 보통일때
	float m_fNormalPesoCorrection;
	float m_fNormalExpCorrection;

	struct RandomStartPos
	{
		float m_fStartXPos;
		float m_fStartXRange;
		float m_fStartZPos;
		float m_fStartZRange;
		RandomStartPos()
		{
			m_fStartXPos = m_fStartXRange = m_fStartZPos = m_fStartZRange = 0.0f;
		}
	};
	typedef std::vector< RandomStartPos > vRandomStartPos;
	vRandomStartPos m_RandomStartPos;

	DWORD m_dwLimitNpcSpawn;
	DWORD m_dwReadyTime;
	DWORD m_dwSpawnTime;

	DWORD m_dwGenCount;
	DWORD m_dwDuplication;

	DWORD m_dwNextNPCCode;
	NPCRecordList	m_vNPCList;
	NPCCodeList		m_vNPCCodeList;
	NPCCodeList		m_vNPCCodeListHard;
	DWORDVec		m_vSlotCount;

	int		m_iLevel;

	struct BalancePoint
	{
		int min_lv;
		int max_lv;
		float exp_rate;
		float gold_rate;
		bool IsInSection( int iLV )
		{
			if( COMPARE( iLV, min_lv, max_lv+1 ) )
				return true;

			return false;
		}
	};
	typedef std::vector<BalancePoint> vBalancePoint;
	vBalancePoint m_vBalancePoint;

	struct sEquipItemCode
	{
		DWORD dwItemCode[4];
		sEquipItemCode()
		{
			dwItemCode[0] = dwItemCode[1] = dwItemCode[2] = dwItemCode[3] = 0;
		}
	};

	std::map<DWORD,sEquipItemCode> m_mapItemCode;
	ITEM_DATA	GetEquipItemData( DWORD dwNPCCode, int iSlot );
	void		InitNPCEquipItem( NPCRecord& rkMonster );
	ioItem*		ReleaseItem( NPCRecord* pRecord, int iSlot );

protected:
	virtual void ProcessPlay();
	virtual void RestartMode();

	virtual void LoadRoundCtrlValue( ioINILoader &rkLoader );
	virtual void SendRoundResult( WinTeamType eWinTeam );

	virtual void ProcessReady();
	virtual void LoadMapINIValue();

	void LoadNPCInfo(ioINILoader &rkLoader );
	void LoadNPCEquipInfo(ioINILoader &kLoader );
	void LoadBalancePoint(ioINILoader &rkLoader );

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
	
	virtual void OnDropDieUser( User *pDieUser, SP2Packet &rkPacket );
	virtual void OnWeaponDieUser( User *pDieUser, SP2Packet &rkPacket );
	virtual void OnWeaponDieNpc( const ioHashString &rkDieName, SP2Packet &rkPacket );
	virtual void UpdateNPCDieRecord( const ioHashString &szAttacker, const ioHashString &szBestAttacker );
	virtual void OnDropDieNpc( const ioHashString &rkDieName, SP2Packet &rkPacket );
	virtual void OnDropItemNpc( User *pSendUser, const ioHashString &rkOwnerID, SP2Packet &rkPacket );

	virtual int GetRecordCnt() const;
	int			GetRealUserCnt();
	//virtual int GetPlayingUserCnt() const;

	virtual TeamType GetNextTeamType();
	virtual float GetResultScoreGapValue( bool bLadderPoint, TeamType eWinTeam );
	virtual float GetUserCorrection( TeamType eWinTeam, float fRoundPoint, float fScoreGap );

	void SetAILevel( int iLevel );

public:
	virtual int  GetUserKickVoteLimit( const ioHashString &szKickUserName );

public:
	virtual ModeRecord* FindModeRecord( const ioHashString &rkName );
	virtual ModeRecord* FindModeRecord( User *pUser );
	virtual ModeRecord* FindModeRecord( int iIdx );

protected:
	TeamSurvivalAIRecord* FindTeamSurvivalAIRecord( const ioHashString &rkName );
	TeamSurvivalAIRecord* FindTeamSurvivalAIRecord( User *pUser );

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

	DWORD GetNextNPCCode();

	bool ModeStart();
	void SpawnNPC( const DWORD dwMaxCount, bool bEnemy );
	void ReviveNPC();

	float GetMonsterStartXPos( float fXPos, int &rRandIndex );
	float GetMonsterStartZPos( float fZPos, int iRandIndex );

	DWORD GetLiveMonsterCount( );
	DWORD GetDeadMonsterCount( );
	NPCRecord* FindMonsterInfo( const ioHashString &rkName );

	void UpdateMonsterDieRecord( const ioHashString &szAttacker, const ioHashString &szBestAttacker );
	DWORD NameGenerator( OUT char *szName, IN int iSize );

	const ioHashString &SearchMonsterSyncUser();
	void RemoveRecordChangeMonsterSync( const ioHashString &rkRemoveName );
	void PlayMonsterSync( TeamSurvivalRecord *pSendRecord );

	void CheckSpawn();

	void OnRequestStart( User *pUser, SP2Packet &rkPacket );
	void OnRequestStartDev( User *pUser, SP2Packet &rkPacket );
	DWORD GetMapIndexbySlotCount();

public:
	TeamSurvivalAIMode( Room *pCreator );
	virtual ~TeamSurvivalAIMode();
};

inline TeamSurvivalAIMode* ToTeamSurvivalAIMode( Mode *pMode )
{
	if( !pMode || pMode->GetModeType() != MT_TEAM_SURVIVAL_AI )
		return NULL;

	return static_cast< TeamSurvivalAIMode* >( pMode );
}
#endif

