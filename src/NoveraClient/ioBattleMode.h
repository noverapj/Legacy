#pragma once

class ioPlayStage;
class ioPlayEntity;
class ioBaseChar;
class SP2Packet;

#include "ioPlayMode.h"

class ioBattleMode : public ioPlayMode
{
	enum
	{
		BATTLE_ORDER_NONE = -1,
		BATTLE_ORDER_RANDOM = 0,
		BATTLE_ORDER_FIRST = 1,
		BATTLE_ORDER_SECOND = 2,
		BATTLE_ORDER_THIRD = 3,
	};

	enum BATTLE_WAIT_POSITION_INDEX
	{
		BATTLE_BLUE_TOP = 1,
		BATTLE_BLUE_BOTTOM = 2,
		BATTLE_RED_TOP = 3,
		BATTLE_RED_BOTTOM = 4,
	};

	// 아래 enum값은 sp2_battle_mode%d_map%d.ini 파일의 [map_passive_buff] 부분 버프들 순서가 달라지면 이것도 달라져야한다. - SYH
	enum MAP_PASSIVE_BUFF
	{
		ALL_BUFF = -1,
		IMMOTAL_UNLIMITE_BUFF = 0,		// 버프이름 : 000_protect_State3 (제한 시간없는 무적버프)
		IMMOTAL_3SEC_BUFF = 1,			// 버프이름 : 000_protect_State1 (제한 3초 무적버프)
		PIERROT_JUMP_BUFF = 2,			// 버프이름 : 061EB_deathangel_asd_lv1 (강림도령 갑옷 스킬에 붙어있는 이동기 버프)
		ENTRY_BLUE_EFFECT_BUFF = 3,			// 버프이름 : (임시)oni_protect (난입했다고 표시만하는 버프)
		ENTRY_RED_EFFECT_BUFF = 4,			// 버프이름 : (임시)oni_protect (난입했다고 표시만하는 버프)
		MAX_BUFF_COUNT = ENTRY_RED_EFFECT_BUFF + 1
	};

	enum
	{
		KILL_INFO_WND_OFFSET_Y	= 100
	};
public:
	enum TAG_STATE
	{
		TAG_IMPOSSIBLE_REQ		= -2,		// 태그 불가능한 상태에서 태그 신청
		TAG_IMPOSSIBLE			= -1,		// 태그 불가능한 상태
		TAG_POSSIBLE			= 0,		// 태그 가능한 상태
		TAG_POSSIBLE_REQ		= 1,		// 태그 가능한 상태에서 태그 신청
		TAG_PROCEEDING			= 2,		// 태그 진행 중
	};

protected:
	typedef std::vector< BattleModeRecord > RecordList;
	RecordList m_vCharRecordList;

	struct TagZone
	{
		D3DXVECTOR3  m_vZonePos;
		float        m_fCollisionRange;
		DWORD        m_dwCollisionTime;
		DWORD        m_dwCurCollisionTime;
		float        m_fCollisionRangeEx;
		DWORD        m_dwCollisionTimeEx;
		DWORD        m_dwCurCollisionTimeEx;
		ioHashString m_szCollisionBuff;
		ioHashString m_szStartSound;
		TagZone()
		{
			Init();
		}

		void Init()
		{
			m_vZonePos = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
			m_fCollisionRange = m_fCollisionRangeEx = 0.0f;
			m_dwCollisionTime = m_dwCurCollisionTime = 0;
			m_dwCollisionTimeEx = m_dwCurCollisionTimeEx = 0;
		}
	};
	typedef std::vector< TagZone > TagZoneList;
	TagZoneList m_TagZoneList;
	TagZone	  m_CurrentTagZone;

	struct TagStateInitInfo
	{
		DWORD			m_dwEndTime;
		ioHashString	m_szWaitUser;
	};
	typedef std::list< TagStateInitInfo > TagUserInitInfoList;
	TagUserInitInfoList m_TagStateList;

	typedef std::map< ioHashString, bool > mapFirstCheck;		// 각 유저 별 첫셋팅 플래그 
	mapFirstCheck m_mapFirstCheck;

protected:
	CEncrypt<int> m_iRedKillPoint;
	CEncrypt<int> m_iBlueKillPoint;

	CEncrypt<float> m_fFinalRedPoint;
	CEncrypt<float> m_fFinalBluePoint;

	float m_fWinScoreConstant;

	ioHashString m_LastDieChar;
	ioHashString m_LastKillChar;

	ioHashString m_KillPointEmoticonRed;
	ioHashString m_KillPointEmoticonBlue;
	ioHashString m_KillPointNum;
	ioHashString m_KillPointPer;
	ioHashString m_KillPointSound;

	Vector3Vec m_vWaitingPos;

	// 태그 요청 / 취소 버튼 비활성화 시간
	DWORD		m_dwTagBtnDisableTime;
	DWORD		m_dwStartTagBtnDisableTime;

	// 첫 입장 패킷 보냈는지 체크
	bool		m_bWarTimeStartPacketSend;

	// 난입 유저들을 위한 난입 전 좌표 저장 (블루, 레드팀 각각 하나씩)
	D3DXVECTOR3 m_vBlueTeamEntryPos;
	D3DXVECTOR3 m_vRedTeamEntryPos;

	// 카메라 첫 시점 변경 로직 막는 플래그
	bool m_bFirstBlockCameraView;

	// 재태그 불가 시간
	DWORD m_dwTagDisableTime;

	// 남은 난입 횟수
	int m_iMaxEntryCount;
	int m_iCurEntryCount;

	// 초기 난입 시간
	DWORD m_dwEntryEnableTime;

	// 난입 복귀하라는 패킷 전 복귀 할수 있냐는 패킷 받을때 TRUE시킬 플래그
	bool m_bEntryTimeOutEnableCheck;

	// 블루팀 레드팀 스코어 새로 만듦
	int m_iBlueTeamScore;
	int m_iRedTeamScore;

	// 난입 복귀 이펙트
	ioHashString m_szEntryTimeOutEffect;

	// 유저 셋팅 플래그(모드 시작할때는 false)
	bool m_bUserSetting;

	// 첫 출전일 경우, 처음 용병 선택 전까지 캐릭터 상태 값 CS_LOADING으로 고정되게 하는 플래그
	bool m_bFixLoadingState;

	// 태그 시 
	DWORD m_dwTagWaitStateBackTime;

	typedef std::vector< TagZone > TagZoneList;

public:
	virtual void InitMode( int iMapIndex );
	virtual void ProcessMode();
	virtual void RenderPreGUI();
	virtual void RenderAfterGUI();
	virtual void ReStartMode();

protected:
	virtual void LoadModeResource( ioINILoader &rkLoader );
	virtual void LoadMap();
	void LoadTagZone();

public:
	virtual bool ProcessTCPPacket( SP2Packet &rkPacket );
	virtual bool ProcessUDPPacket( SP2Packet &rkPacket );

	virtual void ApplyModeInfo( SP2Packet &rkPacket );
	virtual void ApplyCharModeInfo( const ioHashString &rkName, SP2Packet &rkPacket, bool bDieCheck = false );

public:
	virtual void NotifyEntityDie( ioPlayEntity *pEntity, ioPlayEntity *pKiller, ioBaseChar *pBestAttacker,
		float fLastRate, float fBestRate );
	virtual void NotifyCharDropDie( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker,
		float fLastRate, float fBestRate );


public:
	virtual void AddNewRecord( ioBaseChar *pChar );
	virtual void RemoveRecord( const ioHashString &rkName );
	virtual void RemoveRecord( int nIndex );

	virtual ioBaseChar* GetRecordChar( int i );
	virtual ioBaseChar* GetRecordChar( const ioHashString &rkName );
	virtual int GetRecordCharCnt() const;

protected:
	virtual ModeRecord* FindModeRecord( int iIdx );
	virtual ModeRecord* FindModeRecordCharName( const ioHashString &rkName );

	BattleModeRecord* FindRecordCharName( const ioHashString &rkName );

	virtual void SetRoundEndInfo( WinTeamType eTeam, bool bRoundSetEnd );
	virtual bool GetRoundInfo( int iIndex, RoundInfo &rkInfo );
	virtual bool GetResultInfo( int iIndex, ResultInfo &rkInfo );
	virtual bool GetFinalInfo( int iIndex, FinalInfo &rkInfo );

	void ProcessTagZoneCollision();
	void FindBaseCharRecordNameList( ioHashStringVec &rkRecordNameList );

	// Guide
protected:
	//EndSceneCamera
	virtual bool IsNeedRevival( ioBaseChar *pChar );
	virtual void CheckEndSceneLookAt();
	virtual void ClearModeData();

protected:
	//virtual void UpdateMiniMapSpotPoint();
	virtual void UpdateRoundStateWnd();

	virtual void SetResultScoreWnd();
	virtual void SetTotalScoreWnd( IntVec &rvClassTypeList,
		IntVec &rvClassPointList,
		float fBlueGuildTeamBonus,
		float fRedGuildTeamBonus,
		float fBonusArray[BA_MAX] );

	virtual void ShowEndScoreWnd( bool bShow );


protected:
	void ProcessGUI();

	void ProcessReadyState();
	void ProcessPlayState();
	void ProcessResultState();

	void UpdateTimeScoreWnd();
	void CheckRoundEnd();

	float GetBlueKillRate();
	float GetRedKillRate();
	float GetScoreKillPoint( TeamType eTeam );

	void SetKillPointEmoticon( TeamType eTeam, const ioHashString& szAttacker );
	void OnNotifyCharDie( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker,
		float fLastRate, float fBestRate );

	int GetTeamPlayerCount( TeamType eTeam );
	ioBaseChar* CheckMostNearChar( ioBaseChar *pChar );

	// 태그 요청 패킷 To Server
	void OnTagWaitForRequest(SP2Packet &rkPacket);
	void OnTagAccept(SP2Packet &rkPacket);
	void OnBattleModeStartCheck(SP2Packet &rkPacket);
	void OnBattleModeEntryStart(SP2Packet &rkPacket);
	void OnBattleModeEntryTimeOut(SP2Packet &rkPacket);
	void OnBattleModeDieStart(SP2Packet &rkPacket);			// 전장에서 싸우던 유저가 죽었을 경우
	void OnTagAcceptTimeOK(SP2Packet &rkPacket);			// 모든 유저들에게 특정 유저의 태그 신청이 바로 태그될 수 있는 상태인 것을 알림

public:
	void ApplyExtraModeInfo( SP2Packet &rkPacket );

public:
	virtual D3DXVECTOR3 GetRevivalPos( ioBaseChar *pChar );
	D3DXVECTOR3 GetWaitingPos( ioBaseChar *pChar, TeamType eTeamType );
	D3DXVECTOR3 GetWaitingPosByPosIdx( ioBaseChar *pChar );
	// 맵 버프 적용
	virtual void SetMapPassiveBuff( ioBaseChar *pChar );
	void SetMapPassiveBuffDuringBattle( ioBaseChar *pChar, int iBuffIdx = -1 );		// 배틀모드 중간에 맵 버프 적용
	void RemoveMapPassiveBuffDuringBattle( ioBaseChar *pChar, int iBuffIdx = -1 );		// 배틀모드 중간에 맵 버프 해제

	// 배틀모드 상태 스위칭 함수 - 대기상태 -> 출전(or 난입)상태, 출전(or 난입)상태 -> 대기상태
	void SwitchBattleState( bool bUserState );
	void StartTagButtonInactive();

	void ShowTagAlarm( ioHashString& userName );
	void ShowEntryAlarm( ioHashString& userName );
	
	// 스코어 반환
	int GetScore( TeamType eTeam );

	// 난입 횟수 반환 - 추후 최대 갯수도...
	int GetCurEntryCount();
	int GetMaxEntryCount();

	// 용병 맵 중앙을 바라보도록 회전시키는 함수
	void RotateSoldierToCenter( ioBaseChar* pOwner );

	// 현재 출전될 유저 시점으로 변경
	void ChangeCurFighterView();

	void AddBattleModeUserInfoToUI( ioBaseChar* pAddChar );

	// 스킬 게이지 공유
	void ShareSkillGaugeForSameTeam( ioHashString szEndUserName, bool bDie = false );

	bool GetEntryTimeOutEnableCheck() { return m_bEntryTimeOutEnableCheck; }

	// 떨궈진 장비 상태 셋팅 (기본 해제)
	void SetUnmountingEquipItem( const ioHashString &szUserID, int iItemType, bool bUnmount = true );
	// 떨궈진 장비 상태 반환
	bool GetUnmountingEquipItem( const ioHashString &szUserID, int iItemType );

	void ChangeScore( TeamType eTeam );

	// [ UI ] 유저 리스트에서 정렬이 WaitPosIdx 값 기준으로 되어있기 떄문에 태그 시 바꿔줘야함
	void SwapUserWaitPosIdxInUI( ioBaseChar* pChar_IN, ioBaseChar* pChar_OUT );

	DWORD GetTagBtnDisableTime() { return m_dwTagBtnDisableTime; }

	bool AllFirstUserCheck();
public:    //기여도.
	void ContributeConvertTeam( TeamType eTeam );
	virtual void ContributeConvert();
	virtual int ContributeMyRank( int &rRankValue1, int &rRankValue2, int &rRankValue3, int &rRankValue4 );
	virtual void ContributeRoundAllPercent( DWORD dwCurTime = 0, DWORD dwTotalTime = 0 );
	virtual void DamageContribute( const ioHashString szAttacker, const ioHashString szTarget, float fDamage, DWORD dwItemCode, int iWeaponType = -1 );
	virtual void KillContribute( const ioHashString szKiller, const ioHashString szKilled, DWORD dwItemCode );
	virtual void AttackCntContribute( const ioHashString szAttacker, const ioHashString szTarget, DWORD dwItemCode, int iAttackValue = 100 );
	virtual void ContributeLOG();

	virtual void ContributeResultSync( const ioHashString &szName, int iContribute );

	virtual int GetContributePerChar( const ioHashString& szName );

	virtual void FillLastPlayRecordInfo( SP2Packet &rkPacket );
	virtual void FillPlayRecordInfo( SP2Packet &rkPacket );
	virtual void ApplyPlayRecordInfo( SP2Packet &rkPacket );
	// For Rank
	virtual int GetCurRank( const ioHashString& szCharName );

protected:
	virtual void OnRoundJoin( SP2Packet &rkPacket );
	virtual void OnRoundJoinView( SP2Packet &rkPacket );
	virtual void OnRoundJoinObserver( SP2Packet &rkPacket );
	virtual void OnRoundReady( SP2Packet &rkPacket );
	virtual void OnZeroHP( SP2Packet &rkPacket );

	virtual void OnRoundEnd( SP2Packet &rkPacket );
public:
	// 각 팀 유저 정보 UI (대표 용병 아이콘, 닉네임, 계급)
	// 용병 교체 시 아이콘 변경
	void ChangeCharIcon( ioBaseChar* pChangeChar );

	// 태그 존 On / Off
	void SwitchTagZone( bool bOn, ioHashString szTagReqName );
	// 태그 불가능 이펙트 On / Off
	void SwitchNonTaggedDisplay( bool bOn, ioHashString szTagReqName );

	// 난입할 용병 위치 저장
	void SaveEntrySoldierPos( ioBaseChar* pChar );
	void SwitchEntrySoldierPosToWaitPos( ioBaseChar* pChar );

	// 첫 카메라 시점 변경 막기(막지 않으면 첫 시점이 내 용병이 아닌 다른 용병 시점으로 됨
	bool GetFirstBlockCameraView() { return m_bFirstBlockCameraView; }
	void ChangeFirstBlockCameraView( bool bFirstBlockCameraView ) { m_bFirstBlockCameraView = bFirstBlockCameraView; }

public:
	ioBattleMode( ioPlayStage *pCreator );
	virtual ~ioBattleMode();
};

inline ioBattleMode* ToBattleMode( ioPlayMode *pMode )
{
	if( !pMode || pMode->GetModeType() != MT_BATTLE )
		return NULL;

	return dynamic_cast< ioBattleMode* >( pMode );
}

