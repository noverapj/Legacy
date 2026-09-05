
#ifndef _ShuffleBonusMode_h_
#define _ShuffleBonusMode_h_

#include "Mode.h"
#include "ShuffleBonusModeHelp.h"
#include "MonsterSurvivalModeHelp.h"

class SP2Packet;
class User;
class BuffModeItem;

class ShuffleBonusMode : public Mode
{
protected:
	ShuffleBonusRecordList m_vRecordList;

	IntVec m_SingleTeamList;
	IntVec m_SingleTeamPosArray;

	DWORD m_dwRemainTime;

protected:
	// 셔플모드
	struct ShuffleStarRegenPos
	{
		float m_fXPos;
		float m_fZPos;
		ShuffleStarRegenPos()
		{
			m_fXPos = 0.0f;
			m_fZPos = 0.0f;
		}
	};
	typedef std::vector<ShuffleStarRegenPos> vShuffleStarRegenPos;
	typedef std::vector<ShuffleStarRegenPos>::iterator vShuffleStarRegenPos_iter;
	vShuffleStarRegenPos m_vShuffleStarRegenPos;	

	int m_iMaxStarRegenPos;
	int m_iCurStarRegenPos;
	int m_iMaxStarRegenCnt;
	int m_iCurStarRegenCnt;
	int m_iStarCount;

	DWORD m_dwStarRegenCheckTime;
	DWORD m_dwStarRegenTime;
	DWORD m_dwStarActivateTime;
	DWORD m_dwStarWaitTime;
	int m_iStarCountMin;
	int m_iStarCountMax;

	float m_fFloatPowerMin;
	float m_fFloatPowerMax;
	float m_fFloatSpeedMin;
	float m_fFloatSpeedMax;

	float m_fDropDieStarDecreaseRate;
	float m_fDropStarDecreaseRate;

protected:
	enum BlowTargetType
	{
		BTT_WEAK,
		BTT_BLOW,
		BTT_FLATTEN,
		BTT_M_STIFF,
		BTT_S_STIFF,
		BTT_STAND_KO,
		BTT_STAND_KO2,
		BTT_BLOW2,
		BTT_BLOW3,
		BTT_BOUND_BLOW,
		BTT_M_BLOW,
		BTT_GETUP,
		BTT_GETUP_STAND_KO,
		BTT_BLOW_EXTEND_WOUND,
		BTT_MAX_TYPE,
		BTT_NONE,
	};

	struct WoundedDropInfo
	{
		BlowTargetType m_BlowType;
		int m_iDropCount;

		WoundedDropInfo()
		{
			m_BlowType   = BTT_NONE;
			m_iDropCount = 0;
		}

		bool IsEmpty()
		{
			if( m_BlowType == BTT_NONE && m_iDropCount == 0 )
				return true;

			return false;
		}
	};
	typedef std::vector<WoundedDropInfo> WoundedDropInfoVec;
	WoundedDropInfoVec m_WoundedDropInfoVec;

protected:
	ModeItemVec m_vDropZoneStar;
	int m_iDropZoneUserStar;

protected:
	DWORD m_dwBuffItemCreateTime;
	DWORD m_dwCurrBuffItemCreateTime;
	DWORD m_dwBuffItemRegenTime;
	DWORD m_dwBuffItemWaitTime;
	DWORD m_dwBuffRandMaxRange;

	vShuffleStarRegenPos m_vBuffItemCreatePos;

	struct BuffItemRegenInfo
	{
		float m_fXPos;
		float m_fZPos;
		DWORD m_dwRegenTime;
		DWORD m_dwRand;

		BuffItemRegenInfo()
		{
			m_fXPos = 0.0f;
			m_fZPos = 0.0f;
			m_dwRegenTime = 0;
			m_dwRand      = 0;
		}
	};
	typedef::vector<BuffItemRegenInfo> vBuffItemRegenInfo;
	vBuffItemRegenInfo m_vBuffItemRegenInfo;

protected:
	struct NpcInfo
	{
		int m_iRandTable;		

		DWORD m_dwStartTime;
		DWORD m_dwAliveTime;
		bool m_bSpawn;
		ioHashString m_szNpcName;

		NpcInfo()
		{
			m_iRandTable  = 0;
			m_dwStartTime = 0;
			m_dwAliveTime = 0;
			m_bSpawn = false;
		}
	};
	typedef std::vector<NpcInfo> NpcInfoVec;
	NpcInfoVec m_NpcInfoVec;
	MonsterRecordList m_MonsterRecordVec;

public:
	virtual void DestroyMode();

	virtual void AddNewRecord( User *pUser );
	virtual void RemoveRecord( User *pUser, bool bRoomDestroy );

	virtual void SetStartPosArray();

	virtual void LoadINIValue();
	void LoadShuffleItemInfo();
	void LoadShuffleStarPosInfo();
	void LoadWoundedInfo( ioINILoader &rkLoader );
	void LoadNpc();

public:
	void DestoryModeItemByStar();

protected:
	virtual void LoadRoundCtrlValue( ioINILoader &rkLoader );

protected:
	virtual void ProcessReady();
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
	virtual void UpdateDropDieRecord( User *pDier, const ioHashString &szAttacker, const ioHashString &szBestAttacker );
	virtual void UpdateUserDieTime( User *pDier );

	virtual bool ProcessTCPPacket( User *pSend, SP2Packet &rkPacket );

public:
	virtual int  GetUserKickVoteLimit( const ioHashString &szKickUserName );

public:
	virtual ModeRecord* FindModeRecord( const ioHashString &rkName );
	virtual ModeRecord* FindModeRecord( User *pUser );
	virtual ModeRecord* FindModeRecord( int iIdx );

protected:
	ShuffleBonusRecord* FindShuffleBonusRecord( DWORD dwIndex );
	ShuffleBonusRecord* FindShuffleBonusRecord( User *pUser );
	ShuffleBonusRecord* FindShuffleBonusRecord( const ioHashString &rkName );
	ShuffleBonusRecord* FindShuffleBonusRecordByUserID( const ioHashString &rkUserID );

protected:
	ModeItem* FindModeItemByStarDrop( DWORD dwModeItemIndex );

protected:
	void ProcessGenerateBonusStar();	
	void ProcessGenerateNpc();
	void ProcessGenerateBuffItem();
	void ProcessRegenBuffItem();

	const ioHashString& SearchMonsterSyncUser();

protected:
	virtual void OnGetModeItem( SP2Packet &rkPacket );
	void OnGetModeItemByBuffItem( const ModeItem* pItem, SP2Packet &rkPacket );

	void OnUserShuffleRoomUserBlow( SP2Packet &rkPacket );
	void OnDropZoneDrop( SP2Packet &rkPacket );
	void OnDropZoneDropByStar( SP2Packet &rkPacket );
	void OnDropZoneDropByUser( SP2Packet &rkPacket );

protected:
	virtual void FinalRoundProcessByShuffle();

protected:
	void CalcStarCorrection();

protected:
	int GetBlowDropStarCount( int iBlowType );


public:
	void RemoveRecordChangeMonsterSync( const ioHashString &rkRemoveName );
	void PlayMonsterSync( ShuffleBonusRecord *pSendRecord );

	virtual void OnWeaponDieNpc( const ioHashString &rkDieName, SP2Packet &rkPacket );
	virtual void OnDropDieNpc( const ioHashString &rkDieName, SP2Packet &rkPacket );

public:
	ShuffleBonusMode( Room *pCreator );
	virtual ~ShuffleBonusMode();
};

inline ShuffleBonusMode* ToShuffleBonusMode( Mode *pMode )
{
	if( !pMode || pMode->GetModeType() != MT_SHUFFLE_BONUS )
		return NULL;

	return static_cast< ShuffleBonusMode* >( pMode );
}

#endif 
