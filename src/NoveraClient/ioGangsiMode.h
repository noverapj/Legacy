#pragma once

#include "ioPlayMode.h"

class SP2Packet;

class ioPlayEntity;
class ioBaseChar;
class ioPushStruct;
class TimeScoreWnd;

class ioGangsiMode : public ioPlayMode
{
protected:
	typedef std::vector< GangsiRecord > RecordList;
	RecordList m_vCharRecordList;

	Vector3Vec m_vRevivalPos;
	IntVec m_SingleTeamPosArray;
	IntVec m_RandomPosArray;
	
	ioHashString m_LastDieChar;
	ioHashString m_LastKillChar;

	TeamType     m_eResultMotionWinTeam;

protected:
	struct BuffList
	{
		DWORD m_dwCheckTime;
		ioHashStringVec m_BuffList;
		BuffList()
		{
			m_dwCheckTime = 0;
		}
	};
	typedef std::vector< BuffList > vBuffList;
	
	DWORD m_dwGangsiBuffTime;
	vBuffList m_GangsiBuff;

	DWORD m_dwUserBuffTime;
	vBuffList m_UserBuff;

	// 강시될 때 HP 세팅
	float m_fChangeGangsiHPRate;
	int   m_iChangeGangsiSkinMan;
	int   m_iChangeGangsiSkinWoman;
	
	// N초 간격 알람 틱 
	DWORD m_dwAlarmTickTime;
	DWORD m_dwAlarmWarningTime;
	DWORD m_dwNextAlarmTick;
	DWORD m_dwWarningSoundTime;

	// 강시 본.
	ioHashString m_szHostGangsiSkeletonMan;
	ioHashString m_szHostGangsiSkeletonWoman;
	ioHashString m_szInfectionGangsiSkeletonMan;
	ioHashString m_szInfectionGangsiSkeletonWoman;

	// 강시 아이템
	IntVec m_HostGangsiItemCode;
	IntVec m_InfectionGangsiItemCode;

protected:
	DWORD  m_dwCenterUIDelayTime;
	DWORD  m_dwGangsiSurvivorTime;

public:
	virtual void InitMode( int iMapIndex );
	virtual void ProcessMode();
	virtual void RenderPreGUI();
	virtual void RenderAfterGUI() {}
	virtual void ReStartMode();

public:
	virtual bool ProcessUDPPacket( SP2Packet &rkPacket );

	virtual void ApplyModeInfo( SP2Packet &rkPacket );
	virtual void ApplyCharModeInfo( const ioHashString &rkName,
									SP2Packet &rkPacket, bool bDieCheck = false );

protected:
	//EndSceneCamera
	virtual bool IsNeedRevival( ioBaseChar *pChar );
	virtual void CheckEndSceneLookAt();
	virtual void ClearModeData();

public:
	virtual void NotifyEntityDie( ioPlayEntity *pEntity, ioPlayEntity *pKiller, ioBaseChar *pBestAttacker,
								  float fLastRate, float fBestRate );
	virtual void NotifyCharDropDie( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker,
									float fLastRate, float fBestRate );

public:
	virtual D3DXVECTOR3 GetStartPos( TeamType eTeamType, bool bRandom = true );
	virtual D3DXVECTOR3 GetRevivalPos( ioBaseChar *pChar );
	virtual int GetRoundWinCnt( TeamType eTeamType );

protected:
	virtual void LoadMap();
	virtual void LoadModeResource( ioINILoader &rkLoader );
	virtual void LoadContribute();

protected:
	void ProcessReadyState();
	void ProcessPlayState();
	void ProcessResultState();
	void ProcessGUI();

	void CheckRoundEnd();
	void ProcessRevival();
	void UpdateTimeScoreWnd();
	void GetMostNearPosList( PositionDistanceSqList &rkResultList, const D3DXVECTOR3 &vTargetPos );
	ioBaseChar* CheckMostNearChar( ioBaseChar *pChar );

public:
	virtual void ProcessCharDropZoneCollision();
	virtual void FillOwnerDamagedListByOtherTeam( const ioHashString &szTarget, TeamType eTeam, SP2Packet &rkPacket, bool bAttach );

protected:
	virtual void UpdateMiniMapSpotPoint();
	virtual void UpdateRoundStateWnd();
	virtual void UpdateRevivalTime( const ioHashString &szName, DWORD dwCurRevivalTime );

	void UpdateCenterInfoWnd();

	virtual void SetRoundEndInfo( WinTeamType eTeam, bool bRoundSetEnd );
	virtual void SetRoundEndMotion();
	virtual void SetRoundEndVictories();
	virtual void SetResultScoreWnd();
	virtual void SetTotalScoreWnd( IntVec &rvClassTypeList,
								   IntVec &rvClassPointList,
								   float fBlueGuildTeamBonus,
								   float fRedGuildTeamBonus,
								   float fBonusArray[BA_MAX] );

	virtual void SetResultBriefingWnd();
	virtual void SetTotalResultBriefingWnd();

	virtual bool GetRoundInfo( int iIndex, RoundInfo &rkInfo );
	virtual bool GetResultInfo( int iIndex, ResultInfo &rkInfo );
	virtual bool GetFinalInfo( int iIndex, FinalInfo &rkInfo );

	virtual void ShowEndScoreWnd( bool bShow );

public:
	virtual void AddNewRecord( ioBaseChar *pChar );
	virtual void RemoveRecord( const ioHashString &rkName );
	virtual void RemoveRecord( int nIndex );

	virtual ioBaseChar* GetRecordChar( int i );
	virtual ioBaseChar* GetRecordChar( const ioHashString &rkName );
	virtual int GetRecordCharCnt() const;

	virtual ioBaseChar* GetRecordCharTeamArray( TeamType eTeam, int iArray );

protected:
	virtual ModeRecord* FindModeRecord( int iIdx );
	virtual ModeRecord* FindModeRecordCharName( const ioHashString &rkName );

	GangsiRecord* FindRecordCharName( const ioHashString &rkName );

protected:
	virtual void UpdateDropDieRecord( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker );
	virtual void UpdateKillDeathRecord( ioPlayEntity *pDieEntity, ioPlayEntity *pKiller, ioPlayEntity *pBestAttacker );
	void OnNotifyCharDie( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker,
						  float fLastRate, float fBestRate );

protected:
	virtual void OnFinalRoundResult( SP2Packet &rkPacket );

protected:
	void ProcessAlarmTickTime();
	void ProcessChangeGangsiState(); 
	void ProcessGangsiSurvivorTime();
	void SetInfectionProcess( ioHashString szDier, ioHashString szKiller, ioHashString szKillerSkillName, DWORD dwKillerItemCode );
	void UserBuffProcess();
	void SetUserBuff( ioBaseChar *pChar );
	
	void GangsiBuffProcess();
	void SetGangsiBuff( ioBaseChar *pChar );

public:    //기여도.
	void ContributeConvertTeam( TeamType eTeam );
	virtual void ContributeConvert();
	virtual int ContributeMyRank( int &rRankValue1, int &rRankValue2, int &rRankValue3, int &rRankValue4 );
	virtual void ContributeRoundAllPercent( DWORD dwCurTime = 0, DWORD dwTotalTime = 0 );
	virtual void DamageContribute( const ioHashString szAttacker, const ioHashString szTarget, float fDamage, DWORD dwItemCode, int iWeaponType = -1 );
	virtual void KillContribute( const ioHashString szKiller, const ioHashString szKilled, DWORD dwItemCode );
	virtual void AttackCntContribute( const ioHashString szAttacker, const ioHashString szTarget, DWORD dwItemCode, int iAttackValue = 100 );
	virtual void InfectionCntContribute( const ioHashString szAttacker, const ioHashString szTarget );
	virtual void SurvivorCntContribute( const ioHashString szTarget, DWORD dwTime );
	virtual void ContributeLOG();

	virtual void ContributeResultSync( const ioHashString &szName, int iContribute );
	virtual int GetContributePerChar( const ioHashString& szName );

	virtual void FillLastPlayRecordInfo( SP2Packet &rkPacket );
	virtual void FillPlayRecordInfo( SP2Packet &rkPacket );
	virtual void ApplyPlayRecordInfo( SP2Packet &rkPacket );

	virtual void UpdateRoundInfoList();

	// For Rank
	virtual int GetCurRank( const ioHashString& szCharName );
	virtual void UpdateUIRank();

public:
	virtual void OnChangeCharNextProcess( ioBaseChar *pChar, bool bNewChar );

public:
	void SendChangeGangsi( ioBaseChar *pChar );

public:
	virtual bool ProcessTCPPacket( SP2Packet &rkPacket );
	void OnSelectGangsi( SP2Packet &rkPacket );
	void OnChangeGangsi( SP2Packet &rkPacket );
	virtual void OnZeroHP( SP2Packet &rkPacket );
	virtual void OnRoundJoinView( SP2Packet &rkPacket );

public:
	ioGangsiMode( ioPlayStage *pCreator );
	virtual ~ioGangsiMode();
};

inline ioGangsiMode* ToGangsiMode( ioPlayMode *pMode )
{
	if( !pMode || pMode->GetModeType() != MT_GANGSI )
		return NULL;

	return dynamic_cast< ioGangsiMode* >( pMode );
}
