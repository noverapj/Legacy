#pragma once

class ioINILoader;

class ioGameEntity;
class ioPlayEntity;
class ioBaseChar;
class ioPushStruct;
class ioPlayStage;

class SP2Packet;

class ioUIRenderImage;
class ioUIRenderFrame;
class ioAnimate2DImageRender;
class DamageTable;

class ioWeapon;
class ioItem;
class ioFieldItem;
class ioModeGuideArrow;
class ioMapEffect;

class ioGrowthLevel;

class ioComplexStringPrinter;

class ioModeItem;

struct RoundInfo;
struct RoundHistory;
struct ResultInfo;
struct FinalInfo;


#include "ModeRecord.h"
#include "EncryptValue.h"

class ioPlayMode
{
public:
	enum ModeState
	{
		MS_READY,
		MS_PLAY,
		MS_NEW_CHALLENGER,		// fight club 에서 사용, 중간 난입시 처리용
		MS_NPC_CONTINUE,		// fight club 에서 사용, 결과후 continue 처리용
		MS_RESULT,
		MS_MACRO,
	};

	enum ScoreSound
	{
		SS_FIRST,
		SS_EQUAL,
		SS_REVERSAL,
		SS_DEFAULT,
		SS_LAST,
	};

	enum BonusArray // array로사용되므로 값은 순차적으로 증가 시킬것
	{
		BA_SOLDIER_CNT = 0,
		BA_GUILD,
		BA_PCROOM_EXP,
		BA_PLAYMODE,
		BA_FRIEND,
		BA_EVENT,
		BA_EVENT_PESO,
		BA_ETC_ITEM,
		BA_CAMP_BONUS,
		BA_AWARD_BONUS,
		BA_ETC_ITEM_PESO,
		BA_ETC_ITEM_EXP,
		BA_VICTORIES_PESO,
		BA_HERO_TITLE_PESO,
		BA_MODE_CONSECUTIVELY,
		BA_PCROOM_PESO,
		BA_MAX,
	};

	typedef std::vector<ioModeItem*> vModeItem;
	typedef std::vector<ioModeItem*>::iterator vModeItem_iter;

	struct MapBuffInfo
	{
		bool			m_bFirstApply;
		ioHashString	m_szBuffName;

		MapBuffInfo()
		{
			m_bFirstApply = false;
			m_szBuffName.Clear();
		}
		~MapBuffInfo()
		{
			m_bFirstApply = false;
			m_szBuffName.Clear();
		}
	};

protected:
	typedef std::map< int, ioHashString > ModePathMap;
	ModePathMap m_ModePathMap;

	ioHashString m_ModePathName;

protected:
	typedef std::vector< RoundInfo > RoundInfoList;

	typedef std::vector< ioGrowthLevel* > GrowthLevelList;
	GrowthLevelList m_vGrowthLevelList;

    ioPlayStage *m_pCreator;

	ModeState m_ModeState;
	
	ScoreSound m_PreScoreSound;
	ScoreSound m_ScoreSound;

	WinTeamType m_WinTeamType;
	int m_iCurRoundCount;
	int m_iMaxRoundCount;

	CEncrypt<int> m_iRedRoundWinCnt;
	CEncrypt<int> m_iBlueRoundWinCnt;

	float m_fRoundVictoryRate;
	float m_fRoundWinRate;

	bool m_bFirstSelect;

	DWORD m_dwDefaultRevivalTime;
	DWORD m_dwIncreaseRevivalTime;
	DWORD m_dwMaxRevivalTime;
	DWORD m_dwCurRevivalTime;

	CEncrypt<float> m_fMaxHPRate;
	CEncrypt<float> m_fRevivalHPRate;

	float m_fRandomRange;
	DWORD m_dwRandomSeed;

	// For KO HP Recover
	CEncrypt<float> m_fKORecoverRate;
	ioHashString m_KORecoverEffect;
	ioHashString m_PerfectRecoverEffect;

	ioHashString m_KOBuff;
	//
	typedef std::vector< MapBuffInfo > vecMapBuffInfo;
	vecMapBuffInfo m_vMapPassiveBuffList;
	
	//ioHashStringVec m_vMapPassiveBuffList;
	ioHashStringVec m_vStartBuffList;

	RoundInfoList m_RoundInfoList;
	
	bool  m_bViewRoundState;
	bool  m_bReserveMyPositionEvent;

	DWORD m_dwEndSceneCameraEvent;
	DWORD m_dwEndSceneCameraMoveTime;
	DWORD m_dwEndSceneZoomTime;

	//포로탈출에서 round끝나는 시점에 스킬을 사용해서 소환을 하는 버그수정 [0001067]
	bool m_bIsCanCreateDummyType;

protected:
	DWORD m_dwLoadEventStartTime;
	DWORD m_dwLoadEventDuration;
	DWORD m_dwMapCameraSceneID;
	typedef std::map< DWORD, D3DXVECTOR3 > EventTimeMap;
	EventTimeMap m_EventTimeMap;
	EventTimeMap m_EventTimeMapOffset;

protected:
	DWORD m_dwRoundDuration;
	DWORD m_dwReadyStartTime;
	DWORD m_dwPlayStartTime;
	DWORD m_dwResultStartTime;
	DWORD m_dwCurResultShowDuration;
	DWORD m_dwSuddenDeathTime;
	DWORD m_dwSuddenDaathStartTime;
	
	
	bool m_bResultCameraChange;
	DWORD m_dwResultCameraDuration;

protected:
	static ioHashString m_CurMapFileName;

	int m_iModeSubNum;
	int m_iModeMapIndex;

	int m_iBluePosArray;
	int m_iRedPosArray;

	Vector3Vec m_vStartPos;
	D3DXVECTOR3 m_vMapCenter;

protected:
	DWORD m_dwShowResultWndTime;
	DWORD m_dwRoundResultDuration;
	DWORD m_dwFinalRoundResultDuration;

	DWORD m_dwShowSoldierWndTime;
	DWORD m_dwStartSoldierWndTime;

	float m_fDropZoneDamage;
	float m_fDropZoneMinDamage;
	float m_fDropZoneMaxDamage;

	float m_fDropDamageMapRate;

	float m_fFallDamage;
	float m_fFallMinDamage;
	float m_fFallMaxDamage;

	float m_fNpcFallDamage;
	float m_fNpcFallMinDamage;
	float m_fNpcFallMaxDamage;

	int m_iDropProtectLevel;
	float m_fDropProtectRate;

	DWORD m_dwCurRoundDuration;

	int  m_iSelectCharacter;
	bool m_bEnterOneCheckQuest;

protected:
	ioModeGuideArrow *m_pGuideArrow;
	ioAnimate2DImageRender *m_pPushCursor;

	// 자신이 받은 데미지만을 처리하지 않고 자신이 관리하는 NPC의 데미지도 처리한다.
	typedef std::map< ioHashString, DamageTable* > DamageTableMap;
	DamageTableMap m_DamageTableMap;

	bool m_bRoundSetEnd;
	bool m_bCharDieClearByRound;
	bool m_bSafetyLevelRoom;

protected:
	typedef std::vector< RoundHistory > RoundHisList;
	RoundHisList m_vRoundHistory;

protected:
	int m_ModeMapSoundType;
	ioHashString m_ModeMapSound;
	ioHashString m_WarningSoundDef;
	ioHashStringVec m_WarningSound;
	ioHashString m_TimeOutSound;

	DWORD m_dwModeSoundTime;
	DWORD m_dwModeSoundDelayTime;

	DWORD m_dwStartWarningTime;
	DWORD m_dwCheckWarningTime;

	ioHashString m_FirstScoreSound;
	ioHashString m_EqualScoreSound;
	ioHashString m_ReversalScoreSound;
	ioHashString m_AddScoreSound;
	ioHashString m_LastScoreSound;
	ioHashString m_LoseScoreSound;

	ioHashString m_PointUpEmoticon;

	bool m_bScoreWarning;
	float m_fScoreWarningRate;
	ioHashString m_ScoreWarningSound;

protected:
	DWORDVec m_vContributeTable;
	DWORDVec m_vContributeAddPointItemCode;

protected:
	ioHashString m_CurChampName;

// For Blind
protected:
	ioHashString m_BlindMoveSound;

	ioUIRenderImage *m_pBlindUp;
	ioUIRenderImage *m_pBlindDown;
	ioUIRenderFrame *m_pRevivalGaugeBack;
	ioUIRenderFrame *m_pRevivalGauge;
	ioUIRenderImage *m_pTextRevival;
	ioUIRenderImage *m_pTextRoundDelay;

	float m_fUpBlindHeight;
	float m_fBottomBlindHeight;
	float m_fCurBlindHeightRate;

	bool  m_bBlindRender;
	DWORD m_dwSetBlindModeTime;
	DWORD m_dwEndBlindModeStartTime;
	DWORD m_dwBlindSlerpTime;
	DWORD m_dwCurBlindSlerpTime;

protected:
	static CEncrypt<bool> m_bZeroHP;
	static CEncrypt<ModeType> m_ModeType;

protected:
	D3DXVECTOR3  m_vEndLookAt;
	ioHashString m_szEndLookCharName;
	float m_fEndHeight;
	float m_fEndDistance;
	float m_fEndFov;	
	ioHashString m_RoundEndEffect;

protected:
	int   m_iKillDeathPoint;

protected:
	DWORD m_dwCurAutoStartAlarmTime;
	DWORD m_dwAutoStartAlarmTime;

protected:   // 특정 모드에서만 Wounded Effect와 Kill Camera를 바꾼다.
	ioHashString m_CharWoundExtraEffect;
	int m_iKillShakeCamera;

	//Test 윤태철
protected:
	struct MapDummyObject
	{
		ioHashString m_stName;
		D3DXVECTOR3 m_vPos;
		float m_fAngle;
	};

	typedef std::vector< MapDummyObject > MapDummyObjectList;
	MapDummyObjectList m_MapDummyObjectList;

protected:
	struct MapAreaWeapon
	{
		ioHashString m_AreaWeaponName;

		bool m_bFirst;
		DWORD m_dwCreateTime;
		float m_fCreateAngle;

		D3DXVECTOR3  m_vPos;

		DWORD m_dwCheckTime;

		bool m_bOneTimeCreate;
		bool m_bCreated;

		MapAreaWeapon()
		{
			m_bFirst = true;
			m_dwCreateTime = 0;
			m_fCreateAngle = 0.0f;
			m_dwCheckTime = 0;

			m_bOneTimeCreate = false;
			m_bCreated = false;
		}
	};

	typedef std::vector< MapAreaWeapon > MapAreaWeaponList;
	MapAreaWeaponList m_MapAreaWeaponList;

	DWORD m_dwAreaWeaponMapIndex;

protected: 
	bool m_bMonsterVsMonsterCollision;    // 몬스터와 몬스터 충돌 체크 여부

protected:
	bool m_bProtoTypeMode;
	bool m_bTeamAttackMode;   // 팀 타격은 가능
	bool m_bTeamDamageMode;   // 팀 데미지 가능

protected:   // 모드 전용 카메라 아래의 값이 세팅되지 않으면 sp2.ini의 카메라 세팅으로 모드가 진행된다.
	bool m_bModeCamera;
	float m_fModeCameraFov;
	float m_fModeCameraHeight;
	float m_fModeCameraDistance;
	float m_fModeCameraLookAtHeightOffset;
	float m_fModeCameraDistanceLimit;
	DWORD m_dwModeCameraSlerpTime;

protected:
	ioHashStringVec m_DisableSkillList;

protected:
	bool		m_bShuffleBonusMode;
	vModeItem	m_vModeItemList;

protected:
	float m_vModeGrowthRate[MAX_CHAR_GROWTH+MAX_ITEM_GROWTH];

	int	m_iRoundEndRemainUserCnt;

	bool	m_bCharacterChangeImmediately;
	DWORD	m_dwChangeCoolTime;
	
public:
	virtual void InitMode( int iMapIndex ) = 0;
	virtual void ProcessMode() = 0;
	virtual void RenderPreGUI() = 0;
	virtual void RenderAfterGUI() = 0;

	virtual void ReStartMode() = 0;
	virtual void ClearMode();

public:
	virtual void StartEventScene( bool bFirst, DWORD dwEventSceneID );
	void ReserveMyStartPositionEvent( bool bFirst );

	virtual D3DXVECTOR3 UpdateEventScenePos( DWORD dwCurTime ) const;
	virtual D3DXVECTOR3 UpdateEventSceneOffset( DWORD dwCurTime ) const;
	
	virtual void ClearModeData();
	virtual void CheckEndSceneLookAt();
	virtual bool IsNeedRevival( ioBaseChar *pChar );
	virtual bool IsDieStateEndMotionNewPos( ioBaseChar *pChar ){ return true; }
	virtual bool ShowSoldierSettingUI( ioBaseChar *pOwnerChar, int iArray );

	DWORD GetEndSceneCameraEvent(){ return m_dwEndSceneCameraEvent;}
	DWORD GetEndSceneCameraMoveTime(){ return m_dwEndSceneCameraMoveTime;}
	DWORD GetEndSceneZoomTime(){ return m_dwEndSceneZoomTime;}
	void GetEndSceneCameraPos( float &fHeight, float &fDistance, float &fFov );
	D3DXVECTOR3 GetEndSceneLookAt() const;
	const ioHashString &GetEndSceneLookCharName() const { return m_szEndLookCharName; }
	const ioHashString &GetRoundEndEffect(){ return m_RoundEndEffect; }

	virtual void AddEventSceneTime( DWORD dwTime, const D3DXVECTOR3 &vEventPos, const D3DXVECTOR3 &vEventOffset );
	virtual bool IsEventSceneOver() const;
	bool IsRoundSetEnd(){ return m_bRoundSetEnd; }

public:
	bool IsModeCameraUse(){ return m_bModeCamera; }
	float GetModeCameraFov(){ return m_fModeCameraFov; }
	float GetModeCameraHeight(){ return m_fModeCameraHeight; }
	float GetModeCameraDistance(){ return m_fModeCameraDistance; }
	float GetModeCameraLookAtHeightOffset(){ return m_fModeCameraLookAtHeightOffset; }
	float GetModeCameraSlerpTime(){ return m_dwModeCameraSlerpTime;}
	float GetModeCameraDistanceLimit(){ return m_fModeCameraDistanceLimit; }

public:
	// 로비 룸에서 사용하는 함수
	virtual bool IsNetworkMode(){ return true; }
	virtual void AddMySoldier( DWORD dwIndex, bool bBuySoldier = false ){}
	virtual void RemoveMySoldier( DWORD dwIndex ){}
	virtual void ReLoadMySoldier( int iCharArray ){}
	virtual void ReLoadAllMySoldier(){}
	virtual void MySoldierDefPos(){}
	virtual bool IsCollisionCheck();
	virtual void SetOneMyChar(){}
	virtual void SetAllMyChar(){}
	virtual bool IsOneMyChar(){ return true; }

public:
	virtual bool ProcessTCPPacket( SP2Packet &rkPacket );
	virtual bool ProcessUDPPacket( SP2Packet &rkPacket );

	virtual void GetModeSpecialInfo( SP2Packet &rkPacket );
	
	virtual void ApplyModeInfo( SP2Packet &rkPacket );
	virtual void ApplyCharModeInfo( const ioHashString &rkName,
									SP2Packet &rkPacket, bool bDieCheck = false ) = 0;
	virtual void ApplyNewUserModeInfo( const ioHashString &rkName, SP2Packet &rkPacket ){}

	virtual void AddNewRecord( ioBaseChar *pChar ) = 0;
	virtual void RemoveRecord( const ioHashString &rkName ) = 0;
	virtual void RemoveRecord( int nIndex ) = 0;

	virtual ioBaseChar* GetRecordChar( int i ) = 0;
	virtual ioBaseChar* GetRecordChar( const ioHashString &rkName ) = 0;
	virtual int GetRecordCharCnt() const = 0;

	virtual ioBaseChar* GetRecordCharTeamArray( TeamType eTeam, int iArray );
	virtual void SkeletonEtcItemSync( const ioHashString &rkName, int iEtcItem ){}
	virtual void ChangeSkeletonEtcItem( const ioHashString &rkName ){}

	void ClearGrowthLevel();
	void AddGrowthLevel( ioBaseChar *pChar );
	void AddGrowthLevel( const ioHashString &rkName );
	void RemoveGrowthLevel( ioBaseChar *pChar );
	ioGrowthLevel* GetGrowthLevel( const ioHashString &rkName );

	

protected:
	virtual ModeRecord* FindModeRecord( int iIdx ) = 0;
	virtual ModeRecord* FindModeRecordCharName( const ioHashString &rkName ) = 0;

protected:
	virtual void SetRoundEndInfo( WinTeamType eTeam, bool bRoundSetEnd );
	virtual void SetRoundEndMotion();
	virtual void SetRoundEndVictories();
	virtual void SetAutoStartAlarmTime( DWORD dwTotalResultTime );

public:
	virtual void NotifyEntityDie( ioPlayEntity *pEntity, ioPlayEntity *pKiller, ioBaseChar *pBestAttacker,
								  float fLastRate, float fBestRate ) = 0;
	virtual void NotifyCharDropDie( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker,
									float fLastRate, float fBestRate ) = 0;
	virtual void NotifyCharDrop( ioPlayEntity *pEntity, float fDropDamage );
	virtual void NotifyCharFall( ioPlayEntity *pEntity ) {}
	virtual void NotifyDropItem( ioItem *pItem, ioBaseChar *pChar) {}
	virtual void NotifyPickItem( ioItem *pItem, ioBaseChar *pChar) {}
	virtual void NotifyFieldItemSupply( ioItem *pItem ) {}
	virtual void NottifyWeaponWounded( ioBaseChar* pChar ){}
	virtual void NotifyWeaponBlowWounded( ioBaseChar* pAttacker, ioBaseChar *pTarget ){}

	virtual DamageTable *FindDamageList( const ioHashString &rkName );
	virtual void NotifyOwnerDamaged( const ioHashString &szTarget, const ioHashString &szAttacker, float fDamage );
	virtual void AttachOwnerDamageList( const ioHashString &szName );
	virtual bool FillAllDamagedList( SP2Packet &rkPacket );
	virtual void FillOwnerDamagedListByOtherTeam( const ioHashString &szTarget, TeamType eTeam, SP2Packet &rkPacket, bool bAttach );
	virtual void GetDamagedTopChar( const ioHashString &szTarget, ioHashString &rkReturnName );
	virtual void GetDamagedLowestChar( const ioHashString &szTarget, ioHashString &rkReturnName );
	virtual void GetModeCondition( const ioHashString &szTarget, ioHashString &rkReturnName );
	virtual int  GetAllDamaged( const ioHashString &szTarget, const ioHashString &szFindName );

protected:
	virtual void LoadMap();
	virtual void LoadModeINIPATH();

	virtual void LoadPushStructs();
	virtual void LoadObjectItems();
	virtual void LoadFuntionalObjects();
	virtual void LoadMapAreaWeapon( ioINILoader &rkLoader );
	virtual void LoadModeCamera( ioINILoader &rkLoader );
	virtual void LoadMachineStruct();
	virtual void LoadMapDummyObject( ioINILoader &rkLoader );

	virtual void LoadCharacters();
	virtual void LoadNPC();
	virtual void LoadGUIValues();
	virtual void LoadPushCursor();
	virtual void LoadModeResource( ioINILoader &rkLoader );
	virtual void LoadContribute();

	void LoadRevivalTime( ioINILoader &rkLoader );

	void SetMapAreaWeapon();
	void RemoveMapAreaWeapon();

	//Test
	void SetMapDummy();
	void RemoveMapDummy();

	void ProcessMapAreaWeapon();
	virtual bool CheckCreateMapAreaWeapon( DWORD dwCheckTime, DWORD dwCreateTime, bool bFirst );
	virtual bool CheckOneTimeCreateMapAreaWeapon( DWORD dwCheckTime, DWORD dwCreateTime, bool bFirst );

protected:
	virtual void ProcessPushEnableCursor();
	virtual void RenderPushEnableCursor();

	virtual void UpdateRoundStateWnd();
	virtual void SetResultScoreWnd();
	virtual void UpdateEndScoreWndTime();
	virtual void UpdateSoldierRevivalPenalty();
	virtual void UpdateSoldierWndTime();
	virtual void UpdateObserverWnd();
	virtual void UpdateTimeGrowthTime();

	virtual void ShowEndScoreWnd( bool bShow );
	virtual void SetTotalScoreWnd( IntVec &rvClassTypeList,
								   IntVec &rvClassPointList,
								   float fBlueGuildTeamBonus,
								   float fRedGuildTeamBonus,
								   float fBonusArray[BA_MAX] );

	virtual void SetResultBriefingWnd();
	virtual void SetTotalResultBriefingWnd();

	void ProcessDeathTimeHelp();
	void ProcessSelectCharacter();
	void ProcessAutoStartAlarm();
	void UpdateRoundCount();
	void ClearRoundEndAllBuff();

protected:
	virtual void UpdateMiniMapSpotPoint();
	virtual bool CheckMyStartPositionEventFire();

	virtual bool GetRoundInfo( int iIndex, RoundInfo &rkInfo );
	virtual bool GetResultInfo( int iIndex, ResultInfo &rkInfo );
	virtual bool GetFinalInfo( int iIndex, FinalInfo &rkInfo );

protected:
	virtual void UpdateDropDieRecord( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker );
	virtual void UpdateKillDeathRecord( ioPlayEntity *pDieEntity, ioPlayEntity *pKiller, ioPlayEntity *pBestAttacker );

	virtual void UpdateRoundWinRecord();
	virtual void UpdateUserRank();

public:
	virtual void SetCenterHelp( ioComplexStringPrinter &rkPrinter, DWORD dwFirstDelay, DWORD dwShowTime );

public:
	virtual void ProcessCharDropZoneCollision();
	virtual void ProcessZoneCollision(){};

public:
	bool CheckCharExist( const ioHashString &szCharName );
	bool IsEnableBalanceMsg();
	bool IsFirstSelect();
	bool IsNeedUpdateMapEffect( ioBaseChar *pChar );

	// 기여도 변화에 따른 체크
	virtual void CheckNonePlayTime( ioBaseChar *pChar, bool bPlayerChanged = false );
	// 상황변화 (인원수)에 따른 체크
	virtual void CheckOwnerNonePlayTime();

protected:
	virtual void SetModeState( ModeState eState );
	void ClearObjectItem();

public:
	const ioHashString& GetOwnerName();
	ioBaseChar* GetOwnerChar();
	TeamType GetOwnerTeam();

public:
	ioMapEffect* CreateMapEffect( const ioHashString &szFileName, const D3DXVECTOR3 &vScale );
	ioMapEffect* CreateMapEffect( const ioHashString &rkFileName, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vScale );
	ioMapEffect* GetMapEffect( UniqueObjID eObjID );
	void DestroyMapEffect( UniqueObjID eObjID );

public:
	void SetModeSubNum( int iSubNum ) { m_iModeSubNum = iSubNum; }
	inline int GetModeSubNum() const { return m_iModeSubNum; }
	void SetModeCreateValueSet();
	bool GetDummyTypeCreateValue() const { return m_bIsCanCreateDummyType; }

public:
	void SetViewRoundState( bool bView ){ m_bViewRoundState = bView; }
	inline bool IsViewRoundState() const { return m_bViewRoundState; }

public:
	virtual D3DXVECTOR3 GetStartPos( int iPosArray, bool bRandom = true ) const;
	virtual D3DXVECTOR3 GetStartPos( TeamType eTeamType, bool bRandom = true );
	virtual D3DXVECTOR3 GetRevivalPos( ioBaseChar *pChar ) = 0;

	int GetRevivalCnt( ioBaseChar *pChar );

public:
	void SetWinTeamType( WinTeamType eWinTeam );
	WinTeamType GetWinTeamType() const;
	DWORD GetRevivalGapTime( int iRevivalCnt );

public:
	virtual void StartChangeCharacter( int now_select_char, DWORD dwSelectTime );
	virtual	void EndChangeCharacter();

	virtual bool IsCanExpGoToTarget( const ioHashString &szTarget );
	virtual int GetRoundWinCnt( TeamType eTeamType );

	virtual void SetResultJoinState();

public:
	inline ioPlayStage* GetCreator() const { return m_pCreator; }
	inline ModeState GetModeState() const { return m_ModeState; }
	inline int GetCurRound() const { return m_iCurRoundCount; }
	inline void SetCurRevivalTime( DWORD dwCurRevivalTime ) { m_dwCurRevivalTime = dwCurRevivalTime; }
	inline void SetRandomSeed( DWORD dwSeed ) { m_dwRandomSeed = dwSeed; }

	inline void SetSafetyLevelRoom( bool bSafety ) { m_bSafetyLevelRoom = bSafety; }
	inline bool IsSafetyLevelRoom() const { return m_bSafetyLevelRoom; }

	float GetMapHeight( float fXPos,
						float fZPos,
						const ioGameEntity *pEntity = NULL,
						bool bMidPositionRay = true ) const;
	
	float GetMapHeightByRevial( float fXPos, float fZPos, const ioGameEntity *pEntity = NULL, bool bMidPositionRay = true ) const;

	int GetCurPlayingTeamUserCnt( TeamType eTeam );

	virtual void UpdateRevivalTime( const ioHashString &szName, DWORD dwCurRevivalTime );

	virtual void SetMiniMapEvent( const D3DXVECTOR3 &vPos, int iEvent, TeamType eTeam );
	virtual void PointUpEvent( TeamType eTeam );

	virtual void SendOwnerDamageList( bool bLast = false );

	virtual void SetCharSlotChange(){}
	virtual void SetOwnerDieUIProcess();

	virtual void SetZeroHP( bool bZero );

	virtual int GetRoomNum(){ return -1; }

	virtual bool IsEntityUpdateState();

public:        //전적
	virtual int GetKillPoint( TeamType eMyTeam );
	virtual int GetDeathPoint( TeamType eMyTeam );

// Guide
protected:
	virtual void ProcessModeGuide();
	virtual ioGameEntity* GetGuideTarget( const D3DXVECTOR3& vArrowPos );

public:    //기여도.
	bool IsContributeAddPointItemCode( DWORD dwItemCode );
	void ContributeLowerSort( ContributeInfoList &vInfo );
	float ContributeTeamPer( TeamType eMyTeam, float fLimit );
	float ContributeTeamPerA( TeamType eMyTeam, float fMaxLimit, float fMinLimit );        // (우리팀원수 / 상대팀원수)
	float ContributeTeamPerB( TeamType eMyTeam, float fMaxLimit, float fMinLimit );        // (상대팀원수-1 / 우리팀원수-1)
	virtual void ContributeConvert() = 0;
	virtual int ContributeMyRank( int &rRankValue1, int &rRankValue2, int &rRankValue3, int &rRankValue4 ){ return 0; }
	virtual int ContributeRank( const ioHashString &rkName, int &rRankValue1, int &rRankValue2, int &rRankValue3, int &rRankValue4 ){ return 0; }
	virtual void ContributeRoundPercent( DWORD dwCurTime, DWORD dwTotalTime ){}         //팀별 비율
	virtual void ContributeRoundAllPercent( DWORD dwCurTime = 0, DWORD dwTotalTime = 0 ){}      //전체 유저 비율
	virtual void ContributeLOG(){}
	virtual void DamageContribute( const ioHashString szAttacker, const ioHashString szTarget, float fDamage, DWORD dwItemCode, int iWeaponType = -1 ) = 0;
	virtual void KillContribute( const ioHashString szKiller, const ioHashString szKilled, DWORD dwItemCode ){}
	virtual void AttackCntContribute( const ioHashString szAttacker, const ioHashString szTarget, DWORD dwItemCode, int iAttackValue = 100 ){}

	virtual void ContributeResultSync( const ioHashString &szName, int iContribute ) = 0;

	virtual int GetContributePerChar( const ioHashString& szName ) { return 0; }

	virtual void FillLastPlayRecordInfo( SP2Packet &rkPacket ) = 0;
	virtual void FillPlayRecordInfo( SP2Packet &rkPacket ) = 0;
	virtual void ApplyPlayRecordInfo( SP2Packet &rkPacket )= 0;

	virtual void RecordLOG();

	// For Rank
	virtual int GetCurRank( const ioHashString& szCharName );
	virtual void UpdateUIRank(){}

public:
	virtual void OnChangeCharNextProcess( ioBaseChar *pChar, bool bNewChar ){}

protected:
	virtual void OnUserRevival( SP2Packet &rkPacket );
	virtual void OnRoundReady( SP2Packet &rkPacket );
	virtual void OnRoundStart( SP2Packet &rkPacket );
	virtual void OnRoundEnd( SP2Packet &rkPacket );
	virtual void OnPushStructCreate( SP2Packet &rkPacket );
	virtual void OnPushStructDie( SP2Packet &rkPacket );
	virtual void OnPushStructInfo( SP2Packet &rkPacket );
	virtual void OnFinalRoundResult( SP2Packet &rkPacket );
	virtual void OnRoundJoin( SP2Packet &rkPacket );
	virtual void OnRoundJoinView( SP2Packet &rkPacket );
	virtual void OnRoundJoinObserver( SP2Packet &rkPacket );
	virtual void OnObjectSupply( SP2Packet &rkPacket );
	virtual void OnDeleteFieldItem( SP2Packet &rkPacket );
	virtual void OnCatchChar( SP2Packet &rkPacket );
	virtual void OnEscapeCatchChar( SP2Packet &rkPacket );
	virtual void OnRoundTimeStop( SP2Packet &rkPacket );
	virtual void OnScore( SP2Packet &rkPacket );
	virtual void OnStartSelectChar( SP2Packet &rkPacket );
	virtual void OnBallStructInfo( SP2Packet &rkPacket );
	virtual void OnBallStructReposition( SP2Packet &rkPacket );
	virtual void OnZeroHP( SP2Packet &rkPacket ) {}
	virtual void OnSuddenDeath( SP2Packet &rkPacket );
	virtual void OnFinalResultUserInfo( SP2Packet &rkPacket );
	virtual void OnUserBonusList( SP2Packet &rkPacket );
	virtual void OnTurnStart( SP2Packet &rkPacket ){}
	virtual void OnTurnEnd( SP2Packet &rkPacket ){}
	virtual void OnMonsterSyncChange( SP2Packet &rkPacket ){}
	virtual void OnMonsterInfoSync( SP2Packet &rkPacket ){}
	virtual void OnRoundContinue( SP2Packet &rkPacket ){}

	virtual void OnPushStructDieByOwnerClear( SP2Packet &rkPacket );
	virtual void OnPushStructInfoEnter( SP2Packet &rkPacket );

	virtual void OnMachineStructInfo( SP2Packet &rkPacket );

public:
	virtual void OnModeSpecialInfo( SP2Packet &rkPacket );

protected:
	void OnCampInfluenceAlarm( SP2Packet &rkPacket );
	void OnServerAlarmMent( SP2Packet &rkPacket );
	void OnModePing( SP2Packet &rkPacket );
	void OnPushStructDamaged( SP2Packet &rkPacket );
	void ApplyBallStructInfo( SP2Packet &rkPacket );

	void PushStructSupply( SP2Packet &rkPacket );
	void ProcessWarningSound( DWORD dwDuration );
	void ProcessScoreWarningSound( bool bWarning );
	void ProcessScoreSound( TeamType eTeam, ScoreSound eScoreSound );

	void BallStructSupply( SP2Packet &rkPacket );

	void MachineStructSupply( SP2Packet &rkPacket );

public:
	void SetModeMapSound( bool bImmediately=false );
	inline int GetModeMotionSoundType() { return m_ModeMapSoundType; }

protected:
	void CheckModeMapSound();
	virtual void PlayExtendSound();

public:
	virtual void CheckTotalChamp( bool bSound=true );
	virtual void ResetTotalChamp();

public:
	inline int GetModeMapIndex() const { return m_iModeMapIndex; }
	const char* GetCurrentMapINIFileName();

	const char* GetModeINIPATH( ModeType eType );
	const char* GetModeINIFileName( ModeType eType );
	const char* GetModeMapINIFileName( ModeType eType, int iSubNum, int iMapIndex );
	static bool IsZeroHP() { return m_bZeroHP; }
	static ModeType GetModeType() { return m_ModeType; }

public:
	float GetMaxHPRateByMode();
	float GetRevivalHPRateByMode();

	float GetFallDamage( ioBaseChar *pChar );

public: 
	void SetRankUI( int iRankType, bool bShow );
	void SetPlayRankUI( ioBaseChar *pChar, int iRankType, bool bShow );

	void ProcessByOwner();

	virtual void SetMapPassiveBuff( ioBaseChar *pChar );
	void SetRecoveryGauge( ioBaseChar *pChar, float fRate );

	//
	bool IsMonsterVsMonsterCollision(){ return m_bMonsterVsMonsterCollision; }
	//
	bool IsProtoTypeMode(){ return m_bProtoTypeMode; }
	bool IsTeamAttackMode(){ return m_bTeamAttackMode; }
	bool IsTeamDamageMode(){ return m_bTeamDamageMode; }

public:
	virtual void ItemEquipAfterUIProcess( ioBaseChar *pChar, int iItemType ){}
	virtual void ItemReleaseAfterUIProcess( ioBaseChar *pChar, int iItemType ){}
	virtual void ItemEntityChangeAfterUIProcess( ioBaseChar *pChar, int iItemType ){}

// For Blind
public:
	virtual void SetBlindMode( bool bStart, bool bBlindRender = true );
	virtual void ProcessBlindMode();
	virtual void RenderBlind();
	virtual void RenderPlayMode();
	virtual bool IsBlindWithRevivalMode();

	bool GetBlindRect( RECT &rcTop, RECT &rcBottom );

	virtual void UpdateRoundInfoList();

// For ExtraItem
public:
	virtual void SetChangeExtraItem( int iCharArray, int iSlot ) {}

public:
	const ioHashString &GetCharWoundedExtraEffect(){ return m_CharWoundExtraEffect; }
	int GetKillShakeCamera(){ return m_iKillShakeCamera; }

public:
	void LoadDisableSkillList();
	void LoadStartBuffList();
	bool CheckDisableSkill( const ioHashString &szSkillName );

public:
	virtual void SetStartBuffList( ioBaseChar *pChar );
	virtual void RemoveStartBuffList();

public:
	virtual void ClearModeItem();
	virtual ioModeItem* FindModeItem( DWORD dwModeItemIndex );

	virtual void OnCreateModeItem( SP2Packet &rkPacket );
	virtual void OnGetModeItem( SP2Packet &rkPacket );
	virtual void CreateModeItem( D3DXVECTOR3 vPos, D3DXVECTOR3 vDir, float fSpeed, float fPower );
	virtual void ProcessModeItem();

public:
	virtual void OnShuffleBonusRewardResult( SP2Packet &rkPacket );

public:
	virtual void ProcessModeItemVSMapCollision( ioOpcodeShape *pShape, ioPlayStage* pStage, const D3DXMATRIX* pShapeWorldMat );
	virtual void ProcessModeItemVSCharCollision( const BaseCharList &rkTargetList );

#ifndef SHIPPING
public:
	virtual void RenderModeEntity();
#endif

public:
	inline DWORD GetModePlayStartTime(){ return m_dwPlayStartTime; }

protected:
	virtual void LoadModeGrowthRate();
public:
	float GetModeGrowthRate( int iGrowthArray );

	inline DWORD GetChangeCoolTime() { return m_dwChangeCoolTime; }
	bool IsCharacterChangeImmediately();

public:
	ioPlayMode( ioPlayStage *pCreator );
	virtual ~ioPlayMode();
};
