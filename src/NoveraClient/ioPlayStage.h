#pragma once

#include "ioGameStage.h"
#include "ScreenBlindSetting.h"
#include "ioFieldItem.h"

class ioRay;
class ioCamera;
class ioLookAtCameraController;
class ioRenderSystem;
class ioGameSceneManager;
class ioEntityGroup;
class ioGameEntity;
class ioPlayEntity;
class ioBaseChar;
class ioNpcChar;
class ioWeapon;
class ioMineWeapon;
class ioMineWeapon2;
class ioMineWeapon3;
class ioMineWeapon4;
class ioMineWeapon5;
class ioMineWeapon6;
class ioMapEffect;
class ioWorldDecoration;
class ioPushStruct;
class ioWeaponManager;
class ioEntity;
class ioSceneNode;
class ioEffect;
class FindPredicate;
class FindListPredicate;
class ioPushStructListManager;
class ioAreaWeaponManager;
class ioTalismanMaker;
class ioObserverWndManager;
class ioWorldEntityFactory;
class ioNoviceGradeBenefitManager;
class ioBlockManager;

class ioBall;
class ioBallManager;

class ioMachineStruct;
class ioMachineStructMgr;

class ioDummyChar;
class ioDummyCharManager;

class ioPet;
class ioPetManager;

class ioSoldierCacheManager;

class ioItem;
class ioFieldItem;
class ioFieldRewardItem;
class ioShakeCameraManager;
class ioCameraEventManager;
class ioAnimate2DImageRender;
class ioINILoader;
class ioUIRenderImage;
class ioOrientBox;
class ioGrowthLevel;

class ioPlayMode;
class ioBlockWorld;
class SP2Packet;

struct Stat;
struct ShakeCamera;

#define HEIGHTMAP_TYPE	1
#define MESHMAP_TYPE	2

// 카메라 스타일
#define CAMERA_STYLE_NORMAL		0x0000		// 일반모드
#define CAMERA_STYLE_MOVIE		0x0001		// 영화모드
#define CAMERA_STYLE_VIEW		0x0002		// 관전모드
#define CAMERA_STYLE_HOUSING	0x0004		// 하우징 설치 모드

class ioPlayStage : public ioGameStage
{
public:
	enum ModeStyle
	{
		MST_NONE		= 0,
		MST_SAFETY		= 1,
		MST_TIMEGATE	= 2,
		MST_BATTLE		= 3,
		MST_PLAZA		= 4,
		MST_LADDER      = 5,
		MST_MATCH		= 6,
	};

	enum VoiceChatType
	{
		VCT_ALL  = 0,
		VCT_TEAM = 1,
	};

	enum
	{
		MAX_LOSS_WEAPON_WOUND_EVENT_COUNT	= 200
	};

	typedef ioListIterator< GameEntityList >GameEntityIterator;

protected:
	GameEntityList	m_GameEntityList;
	GameEntityList	m_FieldItemBufferList;

	MountAvailableList m_MountAvailableList;
	BaseCharList m_BaseCharList;
	BaseCharList m_VisibleBaseList;

	typedef std::vector< ioGameEntity* > ReserveAddEntityList;
	ReserveAddEntityList m_ReserveAddEntityList;

	typedef std::list< ioMapEffect* > MapEffectList;
	MapEffectList m_EffectList;

	struct MapDefaultEffect
	{
		int m_iEffectIndex;
		ioHashString m_FileName;
		D3DXVECTOR3 m_vPos;
		UniqueObjID m_UniqueID;
		RenderGroupID m_eRenderGroupID;
	};
	typedef std::vector< MapDefaultEffect > MapDefaultEffectList;
	MapDefaultEffectList m_MapDefaultEffectList;

	struct MapDefaultObject
	{
		int m_iMultiMapIndex;    
		ioHashString m_FileName;
		D3DXVECTOR3 m_vPos;
		MapObjectQuality m_Quality;
		int m_iAnimationIndex;
		UniqueObjID m_UniqueID;
		bool m_bShadowCast;
		RenderGroupID m_eRenderGroupID;

		bool m_bUseChangeAlphaCheck;
		int m_iChangeAlphaRate;
	};
	typedef std::vector< MapDefaultObject > MapDefaultObjectList;
	MapDefaultObjectList m_MapDefaultObjectList;

	struct MapCameraFixedObject
	{
		ioHashString m_FileName;
		D3DXVECTOR3  m_vPos;
		UniqueObjID  m_UniqueID;
		bool m_bShadowCast;
	};
	typedef std::vector< MapCameraFixedObject > MapCameraFixedObjectList;
	MapCameraFixedObjectList m_MapCameraFixedObjectList;

	struct MapCustomObject
	{
		int           m_iCustomIndex;
		ioHashString  m_FileName;
		D3DXVECTOR3   m_vPos;
		UniqueObjID   m_UniqueID;
		bool          m_bShadowCast;
		RenderGroupID m_eRenderGroupID;
	};
	typedef std::vector< MapCustomObject > MapCustomObjectList;
	MapCustomObjectList m_MapCustomObjectList;

	struct MapCustomEffect
	{
		int           m_iCustomIndex;
		ioHashString  m_FileName;
		D3DXVECTOR3   m_vPos;
		UniqueObjID   m_UniqueID;
		RenderGroupID m_eRenderGroupID;
	};
	typedef std::vector< MapCustomEffect > MapCustomEffectList;
	MapCustomEffectList m_MapCustomEffectList;

	struct EventStruct
	{
		int m_iEventType;
		IntVec m_vStructList;
	};
	typedef std::vector< EventStruct > EventStructList;
	EventStructList m_EventStructList;

	typedef std::map< ioHashString, int > NetCheckMap;
	NetCheckMap m_NetCheckMap;

	//////////////////////////////////////////////////////////////////////////
	//웨폰 wound_sync 보완(윤태철)
	struct WeaponWoundEvent
	{
		ioHashString szAttacker;
		DWORD dwWeaponIdx;

		DWORD dwLifeStartTime;
		DWORD dwLifeEndTime;

		SP2Packet packet;

		WeaponWoundEvent()
		{
			dwWeaponIdx = 0;
			dwLifeStartTime = 0;
			dwLifeEndTime = 0;
		}
	};
	typedef std::vector< WeaponWoundEvent > LossWeaponWoundEvent;

	LossWeaponWoundEvent m_LossWeaponWoundEventList;
	//////////////////////////////////////////////////////////////////////////

	ioPushStructListManager *m_pPushStructMgr;
	ioAreaWeaponManager *m_pAreaWeaponMgr;
	ioTalismanMaker *m_pTalismanMaker;
	ioBallManager *m_pBallMgr;
	ioMachineStructMgr *m_pMachineStructMgr;
	ioDummyCharManager *m_pDummyCharMgr;

	ioObserverWndManager *m_pObserverWndMgr;

	ioRecvPacketLogManager *m_pRecvPacketLogManager;
	ioNoviceGradeBenefitManager *m_pNoviceGradeBenefitManager;

	ioWorldEntityFactory *m_pWorldEntityFactory;

	ioPlayMode		*m_pPlayMode;
	ioBlockWorld	*m_pBlockWorld;
	ioBlockManager	*m_pBlockMgr;

	int m_iModeMapIndex;
	int m_iNextModeMapIndex;

	int m_iModeSubNum;
	int m_iNextModeSubNum;

	ioAnimate2DImageRender *m_pGetupStick;
	ioAnimate2DImageRender *m_pGetupStick2;
	ioAnimate2DImageRender *m_pPressButton;

	Vector3Vec m_vItemCreatePosList;
	std::deque< D3DXVECTOR3 > m_vItemShufflePosList;

	DWORD	m_dwPreUpdatedTime;

	D3DXVECTOR3 m_vCurLookAt;

	int		m_iCameraShakePriority;
	DWORD	m_dwCameraShakeStartTime;
	DWORD	m_dwCameraShakeCheckTime;
	DWORD	m_dwCameraShakeGapTime;
	float	m_fCameraShakeMaxRate;
	float	m_fCameraShakeEndAmount;
	float	m_fCameraShakeOffsetX;
	float	m_fCameraShakeOffsetY;

	DWORD	m_dwCameraRollStartTime;
	DWORD	m_dwCameraRollCheckTime;
	DWORD	m_dwCameraRollGapTime;
	DWORD	m_dwCameraRollEndSlerp;
	DWORD	m_dwCameraRollEndSlerpTime;
	float	m_fCameraRollMaxRate;
	float	m_fCameraRollEndAmount;
	float	m_fCameraRollValue;

	float	m_fShadowEnableRange;

	ioHashString m_ViewChar;
	DWORD	m_dwViewMovingTime;
	DWORD	m_dwCamSlerpTime;
	float   m_fCamLookAtHeightOffset;

	DWORD	m_dwViewMovingTimeItem;
	DWORD	m_dwCamZoomTimeItem;

	bool	m_bCameraZooming;
	bool	m_bCameraChanging;
	bool	m_bCameraBuffing;
	bool	m_bCameraSkilling;

	float	m_fGapDistance;
	float	m_fCurGapDistance;
	float	m_fMaxGapDistance;
	float	m_fGapHeight;
	float	m_fCurGapHeight;
	float	m_fMaxGapHeight;
	float	m_fGapFov;
	float	m_fCurGapFov;
	float	m_fMaxGapFov;
	float   m_fArcadeCameraDistanceLimit;

	DWORD	m_dwZoomStartTime;
	DWORD	m_dwZoomSlerpTime;
	DWORD	m_dwCurSlerpTime;
	DWORD	m_dwChargingStartTime;
	DWORD	m_dwBuffCamStartTime;
	
	float	m_fDefCamHeight;
	float	m_fDefCamDistance;
	float	m_fDefCamFov;

	float	m_fStartDistance;
	float	m_fStartHeight;
	float	m_fStartFov;

	float	m_fCurCamHeight;
	float	m_fCurCamDistance;
	float	m_fCurCamFov;

	float	m_fSpringConst;
	float	m_fDampConst;
	float	m_fSpringXOffset;
	float	m_fSpringZOffset;

	bool	m_bSpringDampUse;

	D3DXVECTOR3 m_vLerpStart;


	bool m_bObserverTPSView;
	float m_fTPSLookAtRate;
	float m_fTPSLookAtOffSet;
	float m_fTPSLookAtOffSetFr;

	float m_fTPSDistance;
	float m_fTPSHeight;
	float m_fTPSFov;

	float m_fTPSOffSet;

// 카메라 마우스조정 변수
	bool        m_bCameraFixedBlock;       // 작동 금지
	DWORD       m_dwCameraFixedStyle;
	D3DXVECTOR3 m_vCamMouseAmt;
	D3DXVECTOR3 m_vCamMouseDefMovie;
	D3DXVECTOR3 m_vCamMouseDefView;
	float       m_fCameraFixedFov;
	float	    m_fMouseCameraZ;
	float	    m_fMousePreTargetZ;
	float		m_fCamMouseFixedLookAtDist;	
	float       m_fCamMouseFixedLookAtDistHeight;	
	//
	D3DXVECTOR3 m_vCamMouseTargetPrevDir;
	D3DXVECTOR3 m_vCamMouseTargetPrevPos;
	D3DXVECTOR3 m_vCamMouseTargetCurLookAt;
	DWORD m_dwCamMouseLookAtMoveCurTime;				
	DWORD m_dwCamMouseLookAtMoveTime;

	D3DXVECTOR3 m_vCamMouseFixedTargetLookAt;	
	D3DXVECTOR3 m_vMouseCameraRot;
	D3DXVECTOR3 m_vMousePreTargetRot;
	
	DWORD	m_dwMouseCamCtrlTime;

// 카메라 마우스 조정 상수
	float	m_fMouseRotateAngle;
	float	m_fMouseRotateAngleByHousing;

	float	m_fMouseRotateSpeed;
	float	m_fMouseRotateSpeedByHousing;

	float	m_fMouseZoomSpeed;
	float	m_fMouseZoomMin;
	float	m_fMouseZoomMax;

	float	m_fMouseZoomSpeedByHousing;
	float	m_fMouseZoomMinByHousing;
	float	m_fMouseZoomMaxByHousing;


	float	m_fMouseZoomTotalMin;
	float	m_fMouseZoomTotalMax;
	float	m_fMouseZoomRestoreSpeed;

	DWORD	m_dwMouseCamCtrlRestoreTime;

	DWORD   m_dwZoomKeyboardTick;

	ioTimer m_UserSyncTimer;
	ioTimer m_PushStructTimer;
	ioTimer m_P2PPingTimer;
	ioTimer m_BallStructTimer;
	DWORD   m_dwPreCharCollisionSyncTime;

	bool    m_bDrawGlowOfTObectAll;
	float   m_fRenderGlowness;
	float   m_fRenderGlowTexcelInc;

	///OnControl 에러 체크용 ////
	typedef std::map< ioHashString, int > map_OnColtrolErr;
	map_OnColtrolErr m_OnControlErr;
	std::vector< ioHashString > m_vOnControl_Err_SendName;

protected:
	float m_fPickItemRange;
	DWORD m_dwOwnerCameraLoseTime;

protected:
	DWORD m_dwCurCameraPauseTime;
	DWORD m_dwCameraPauseTime;

protected:
	bool	m_bUseRevisionSkillGauge;
	bool	m_bReLoadRevisionSkillGauge;//죽고난후에 다시 ini에서 값을 읽을 것인가.
	float	m_fRevisionDefaultSkillGauge;

protected:
	bool  m_bEventSceneCamera;
	bool  m_bEndSceneCamera;
	bool  m_bNoneCameraWorking;
	bool  m_bArcadeCameraWorking;
	bool  m_bFirstEventSceneCamera;
	bool  m_bEventSceneEndSendPacket;
	bool  m_bFirstEventSceneEndSendCheck;
	DWORD m_dwEventSceneEndNextCameraEvent;
	ioHashString m_szEventSceneEndNextTarget;

	bool m_bSetCameraToNowOwner;
	bool m_bTargetLookAt;

public:
	D3DXVECTOR3 m_vMapCenter;
	float	m_fMapFriction;
	float	m_fMapGravity;
	DWORD   m_dwMapClearColor;

	int		m_iMaxSinglePlayer;

	bool	m_bRenderDamage;
	bool	m_bShowCharName;
	bool	m_bShowBalanceValue;
	bool	m_bShowItemName;
	bool	m_bShowItemIcon;
	bool    m_bBlindAniEnd;

	float	m_fItemNameSize;
	int		m_iItemNameColorRed;
	int		m_iItemNameColorGreen;
	int		m_iItemNameColorBlue;

protected:
	CEncrypt<RoomStyle> m_RoomStyle;
	int       m_iRoomIndex;
	
	CEncrypt<int> m_iRedForBalance;
	CEncrypt<int> m_iBlueForBalance;

	CEncrypt<float> m_fMemberBalanceRed;
	CEncrypt<float> m_fMemberBalanceBlue;

	CEncrypt<float> m_fMemberBalanceRedForGauge;
	CEncrypt<float> m_fMemberBalanceBlueForGauge;

	CEncrypt<float> m_fMemberBalanceRedForSpeed;
	CEncrypt<float> m_fMemberBalanceBlueForSpeed;

	CEncrypt<int> m_iMemberBalanceRedCntForBuff;
	CEncrypt<int> m_iMemberBalanceBlueCntForBuff;

	CEncrypt<float> m_fMemberBalanceRedForBlowTime;
	CEncrypt<float> m_fMemberBalanceBlueForBlowTime;

	struct SingleTeamBalance
	{
		TeamType m_eTeam;
		int      m_iUserCount;
		float    m_fDamageBalance;
		float    m_fGaugeBalance;
		SingleTeamBalance()
		{
			m_eTeam          = TEAM_NONE;
			m_iUserCount	 = 0;
			m_fDamageBalance = FLOAT1;
			m_fGaugeBalance  = FLOAT1;
		}
	};
	typedef std::vector< SingleTeamBalance > vSingleTeamBalance;
	vSingleTeamBalance m_vSingleTeamBalance;

	int m_iPreRedCnt;
	int m_iPreBlueCnt;

protected:
	ioCameraEventManager *m_pCameraEventMgr;
	ioShakeCameraManager *m_pShakeCameraMgr;

	ModeType m_ReserveNextModeType;
	bool m_bReservedExit;

protected:
	D3DCOLORVALUE m_CharOverColor;
	float m_fCharOverThickness;

	D3DCOLORVALUE m_PushOverColor;
	float m_fPushOverThickness;

protected:
	ScreenBlindSetting m_SkillBlind;
	ScreenBlindSetting m_KillBlind;
	ScreenBlindSetting m_DeathBlind;

	ScreenBlindTime m_KillBlindTime;
	ScreenBlindTime m_DeathBlindTime;

	ScreenBlindSetting m_CatchKillBlind;
	ScreenBlindSetting m_CatchDeathBlind;
	ScreenBlindTime m_CatchKillBlindTime;
	ScreenBlindTime m_CatchDeathBlindTime;

	ScreenBlindSetting m_FixedBlind;
	ScreenBlindTime m_FixedBlindTime;

	ScreenBlindSetting m_CurBlindSetting;
	ScreenBlindTime  m_CurBlindTime;

	ScreenBlindState m_ScreenBlindState;
	DWORD m_dwScreenBlindCurTime;

	ScreenBlindType m_ScreenBlindType;

	float m_fScreenBlindBuffRange;

	DWORD m_dwKillCameraEvent;
	DWORD m_dwDeathCameraEvent;

	DWORD m_dwCatchKillCameraEvent;
	DWORD m_dwCatchDeathCameraEvent;

	DWORD m_dwStartSelectCharCameraEvent;

	//
	ioUIRenderImage *m_pBuffBlind;
	ScreenBlindTime m_BuffBlindTime;
	ScreenBlindState m_BuffBlindState;
	
	DWORD m_dwBuffBlindCurTime;
	int m_iBuffBlindMaxAlpha;

protected:
	ioHashString  m_szRoomKickOutUser;
	int           m_iRoomKickOutVoteProposal;
	PlazaType     m_ePlazaType;
	bool          m_bOpenPlaza;
	VoiceChatType m_eVoiceChatType;

protected:         //Error Check
	static int sg_iPlayModeErrorCheck;

protected:
	int  m_iBadNetworkCount;
	bool m_bEnableScreenRedFilter;
	bool m_bEnableScreenOveray;

protected:
	float m_fGameEntityCollisionCheckRange;

	int m_iPieceChangeCount;
	int m_iPieceChangePeso;

//etc
protected:
	DWORD m_dwCheckValidateNetTableTime;

protected:         //Equip Item Error Check
	static bool sg_bEquipItemError;

//for ioSummonWoodSkill
protected:	
	enum SummonPosSync
	{
		SPS_ADD,
		SPS_REMOVE,
	};
	
	SummonList m_WoodSummonPositionList;

protected:
	ioPetManager*	m_pPetMgr;
	ioSoldierCacheManager* m_pSoldierCacheManager;

	D3DXVECTOR3 m_vObserverLookAt;
	bool		m_bSpecialObserver;
	bool		m_bStartChangeObserver;
	float		m_fObserverCameraSpeed;

public:
	virtual bool InitStage();
	virtual void Render( ioRenderSystem *pSystem );
	virtual void ClearStage();

	void FindCameraDistCharCheck( ioCamera *pCamera );
	void ClearCameraDistCharCheck();
public:
	virtual void UpdatePreLoop( ioCamera *pCamera );
	virtual void UpdateLoop( ioCamera *pCamera );
	virtual void UpdateAfterLoop( ioCamera *pCamera );
	
protected:
	void ProcessSyncControl();
	void ProcessSyncNpcControl();
	void ProcessSyncPushStruct();
	void UpdateMapEffect();
	void ProcessSyncOwnerPing();
	void ProcessLossWeaponWoundEvent();

public:
	virtual GameStage GetTypeID() const;
	virtual const char* GetTypeText() const;
	int GetScriptModeStyle();

public:
	virtual bool ProcessUDPPacket( SP2Packet &rkPacket );
	virtual bool ProcessTCPPacket( SP2Packet &rkPacket );

	void OnRelayControl( SP2Packet & rkPacket );

public:
	void ChangePlayMode( ModeType eType, PlazaType ePlazaType );
	void ReserveNextModeType( ModeType eType, int iSubNum, int iMapIndex );

public:
	void CreateBlockWorld( int eType );

protected:
	void InitGUI();
	void InitUIImages();
	void InitCameraController();
	void ClearPlayMode();

	void InitRoomOptionInfo();

public:
	void LoadMap( ioINILoader &rkLoader );
	void LoadModeCustomBlind( ioINILoader &rkLoader );
	
	void EnableMapDefaultEffect( bool bEnable );
	void EnableMapDefaultEffect( bool bEnable, int iEffectIndex );		// 맵 이펙트 활성화 (인덱스 번호에 따라... 1부터 시작함)
	void SetMapDefaultObjectQuality( MapObjectQuality eQuality );
	void SetMapCameraFixedObject( bool bFixed );
	void SetMapCustomObject( bool bShow, int iCustomIndex );
	void SetMapCustomEffect( bool bShow, int iCustomIndex );

protected:
	void LoadItemCreatePos( ioINILoader &rkLoader );
	void LoadMapEffectList( ioINILoader &rkLoader );
	void LoadMapObject( ioINILoader &rkLoader );
	void LoadCameraFixedMapObject( ioINILoader &rkLoader );
	void LoadCustomMapObject( ioINILoader &rkLoader );
	void LoadCustomMapEffect( ioINILoader &rkLoader );

public:
	void StartScreenBlind( ScreenBlindType eType, const ScreenBlindTime *pTime, bool bCameraEvent );
	void StartScreenBlindSkill( const ScreenBlindTime *pTime,
								const ScreenBlindSetting *pSetting,
								bool bCameraEvent );

	void StartBuffBlind( const ioHashString &szBlind,
						 const ScreenBlindTime *pTime,
						 int iMaxAlpha );
	void EndBuffBlind();

	void StartScreenBlindBuff( const ScreenBlindTime *pTime, const ScreenBlindSetting *pSetting, float fRange );
	void EndScreenBlindBuff();

	void EndScreenBlind();
	void SetScreenBlindDownState();

protected:
	void ProcessScreenBlind();
	void ProcessBuffBlind();

public:
	void SetScreenRedFilter();
	void ReloadScreenFilter();
	void SetScreenOveray();
	void ReloadScreenOveray();

public:
	void AddGameEntity( ioGameEntity *pEntity );
	void AddBaseChar( ioBaseChar *pBaseChar );
	void AddAniEventHandler( ioBaseChar *pBaseChar, ioEntityGroup *pGrp );
	
	ioBaseChar* CreateBaseChar( const ioHashString &kFileName );
	ioWorldDecoration* CreateWorldDecoration( const ioHashString &kFileName );
	ioPlayEntity* CreateWorldEntity( ioINILoader &rkLoader, int iCreateIndex );
	ioPlayEntity* GetWorldEntity( int iEntitySubType, int iCreateIndex );
	void RemoveWorldEntity( ioPlayEntity *pWorldEntity );

	ioPushStruct* CreatePushStruct( int iININum,
									int iIndex,
									const D3DXVECTOR3 &vTargetPos,
									const D3DXQUATERNION &qtTargetRot,
									const ioHashString &szOwnerName,
									DWORD dwCreateGapTime,
									DWORD dwSeed,
									DWORD dwEtcCode,
									bool bAddList = true );

	ioPushStruct* CreateTestPushStruct( int iININum,
		int iIndex,
		const D3DXVECTOR3 &vTargetPos,
		const D3DXQUATERNION &qtTargetRot,
		const ioHashString &szOwnerName,
		DWORD dwEtcCode );

	ioBall* CreateBallStruct( int iININum, int iIndex,
							  const D3DXVECTOR3 &vTargetPos,
							  const D3DXQUATERNION &qtTargetRot,
							  bool bAddList = true );

	ioMachineStruct* CreateMachineStruct( int iININum,
										  int iArrayNum,
										  const D3DXVECTOR3 &vTargetPos,
										  const D3DXVECTOR3 &vScale,
										  const D3DXQUATERNION &qtTargetRot,
										  DWORD dwCreateGapTime,
										  DWORD dwSeed,
										  bool bAddList = true );

	ioDummyChar* CreateDummyChar( const ioHashString &szDummyCharName,
								  int iIndex,
								  const ioHashString &szOwnerName,
								  const D3DXVECTOR3 &vTargetPos,
								  float fStartAngle,
								  DWORD dwGapTime,
								  bool bAddList = true );

	ioDummyChar* ApplyCreateDummyChar( const ioHashString &szDummyCharName,
									   int iIndex,
									   const ioHashString &szOwnerName,
									   const D3DXVECTOR3 &vTargetPos,
									   float fStartAngle,
									   DWORD dwGapTime,
									   bool bAddList = true, bool bNetWork = false );

	ioEntityGroup* CreateEntityGrpAndSceneNode( const ioHashString &kFileName, bool bThread = false, bool bTestObj = false );

protected:
	GameEntityList::iterator EraseGameEntity( GameEntityList::iterator iter );
	
public:
	ioBaseChar* FindUser( FindPredicate &rkPred );
	bool FindEntityList( FindListPredicate &rkPred );

	ioBaseChar* GetBaseChar( const ioHashString &szCharName );
	ioBaseChar* GetBaseChar( DWORD dwCharIndex );
	ioBaseChar* GetBaseCharFirstTeam( TeamType eTeam );

	const BaseCharList& GetBaseCharList() const { return m_BaseCharList; }
	bool IsBaseChar( const ioHashString &szCharName );
	TeamType GetBaseCharTeam( const ioHashString &szCharName );

	const GameEntityList& GetGameEntityList() const { return m_GameEntityList; }

	void ChangeBaseCharName( const ioHashString &rszCharName, const ioHashString &rszNewCharName );

	void CheckValidateNetTable();
	bool CheckValidateMember( ioHashString &rszCharName );

public:
	const ioHashString& GetOwnerName();
	const ioHashString& GetOwnerLeader();
	ioBaseChar* GetOwnerChar();
	TeamType GetOwnerTeam();

	inline RoomStyle GetRoomStyle() const { return m_RoomStyle; }
	inline int GetRoomIndex() const { return m_iRoomIndex; }
	inline bool IsNoneCameraWorking() const { return m_bNoneCameraWorking; }
	inline bool IsArcadeCameraWorking() const { return m_bArcadeCameraWorking; }
	inline void SetRoomKickOutUser( const ioHashString &rkRoomOut ){ m_szRoomKickOutUser = rkRoomOut; }
	inline const ioHashString& GetRoomKickOutUser(){ return m_szRoomKickOutUser; }

public:
	inline ioPlayMode* GetPlayMode() const { return m_pPlayMode; }
	inline ioBlockWorld* GetBlockWorld() const { return m_pBlockWorld; }

	ModeType GetModeType() const;
	int GetModeSubNum(){ return m_iModeSubNum; }
	bool IsModeStateResult();
	bool IsSafetyLevelRoom() const;
	int GetRoomNum();
	PlazaType GetPlazaModeType();
	bool IsCustomPlazaMode(); // 공개방의 반대로 유저가 만든방(관리자-방장-존재함)
	bool HasCameraFixedStyle( DWORD dwStyle );
	bool IsCameraFixedBlock(){ return m_bCameraFixedBlock; }

public:
	ioAxisAlignBox GetWorldMapBox() const;

	float GetMapHeight( float fXPos,
						float fZPos,
						const ioGameEntity *pEntity = NULL,
						bool bMidPositionRay = true,
						float fDefaultRayHeight = 0.0f,
						bool bIgonreBlockWorld = false,
						bool bIgonreMountObject = false );

	float GetMapHeightByRay( float fXPos,
							 float fZPos,
							 const ioGameEntity *pEntity = NULL,
							 bool bMidPositionRay = true,
							 float fDefaultRayHeight = 0.0f );

	float GetMapHeightNoneEntity( float fXPos,
								  float fZPos,
								  float fDefaultRayHeight = 0.0f,
								  bool bIgonreBlockWorld = false,
								  bool bIgonreMountObject = false );

	float GetMapHeightOnlyTerrain( float fXPos,
								   float fZPos,
								   float fDefaultRayHeight = 0.0f );

	float GetMapHeightByRevial( float fXPos, float fZPos, const ioGameEntity *pEntity = NULL, bool bMidPositionRay = true, float fDefaultRayHeight = 0.0f );

	// 특정 높이에서 체크한 맵 높이, 하늘에서 체크한 맵 높이, 기준점 세가지 비교하여 높이값 구하는 것.
	float GetMapHeightByTargetMid( float fXPos, float fZPos, float fBaseY, float fMidY );

	float GetMapHeightForBlockGuid( const D3DXVECTOR3 &vCurPos, const ioOrientBox &rkBox );

	float GetBoundHeight();	

	ioFieldItem* GetCrownFieldItem( ioItem::ModeCrownType Type );
	void GetCrownFieldItemByAll( ioItem::ModeCrownType Type, ioFieldItemVec& ItemVec );

	ioFieldItem* GetFlagFieldItem();

	ioBaseChar* GetCameraTargetChar();

	GameEntityIterator GetFieldItemIterator();

public:
	ioMapEffect* CreateMapEffect( const ioHashString &rkFileName,
								  const D3DXVECTOR3 &vScale );

	ioMapEffect* CreateMapEffect( const ioHashString &rkFileName,
								  const D3DXVECTOR3 &vPos,
								  const D3DXVECTOR3 &vScale );

	void DestroyMapEffect( ioMapEffect *pEffect );
	void DestroyMapEffect( UniqueObjID eUniqueID );

	ioMapEffect* FindMapEffect( UniqueObjID eUniqueID );

public:
	ioFieldRewardItem *CreateFieldRewardItem( int iFieldRewardUniqueIndex, int iFieldRewardResourceIndex );

	ioFieldRewardItem* FindUserPickFieldRewardItem( ioBaseChar *pChar );
	ioFieldRewardItem* GetFieldRewardItemByIndex( int iUniqueIndex );

	void HidingAllFieldRewardItem();
	void HidingFieldRewardItem( int iFieldRewardUniqueIndex );

public:
	ioItem* CreateItem( const ioHashString &rkName );
	ioItem* CreateItem( DWORD dwItemCode );

	ioFieldItem* CreateFieldItem( ioItem *pItem, bool bAddList = true );
	ioFieldItem* GetFieldItemByIndex( int iPickItemIndex );
	D3DXVECTOR3 GetItemRandomPosition();

	void PickFieldItem( const D3DXVECTOR3 &vDropPos, int iPickItemIndex, int iEquipedBullet, float fEquipedGauge, ioBaseChar *pChar );
	ioFieldItem* DropFieldItem( const D3DXVECTOR3 &vItemPos, ioItem *pItem, ioBaseChar *pChar );
	ioFieldItem* MoveDropFieldItem( const ioHashString &szAttacker, const ioHashString &szSkillName, const D3DXVECTOR3 &vStartPos,
									const D3DXVECTOR3 &vTargetPos, float fMoveSpeed, ioItem *pItem, ioBaseChar *pChar );

	D3DXVECTOR3 CheckDropItemPos( const D3DXVECTOR3 &vDropPos, ioBaseChar *pChar );

	ioFieldItem* FindUserPickFieldItem( ioBaseChar *pChar );
	int FindUserPushStruct( ioBaseChar *pChar );

	void HidingAllFieldItem();
	void HidingFieldItem( int iItemCreateIndex );
	void ClearItemOwnerByOutUser( const ioHashString &rkName );	
	void ReleaseFieldItemByIndex( int itemIndex );

protected:
	void UpdateGameEntityList();
	void UpdateAfterGameEntityList();
	void AddReserveEntityList();

	void ProcessCollision();
	void ProcessWeaponVSMapCollision();	
	void ProcessWeaponVSMineCollision();
	void ProcessWeaponVSMine6Collision();
	void ProcessWeaponVSTrackingMineCollision();
	void ProcessWeaponVSEntityCollision();
	void ProcessWeaponVSAirMineCollision();

	void ProcessPlayEntityCollision();
	void ProcessPlayEntityMapCollision();	// BallStruct는 제외
	void ProcessBallStructMapCollision();

	void ProcessModeItemCollision();

public:
	bool CheckEnableEntity( ioGameEntity *pEntity );
	bool UpdateMountedEntityList( ioGameEntity *pEntity, MountedEntityList &vMountEntityList, ioBaseChar* pOwner );
	bool UpdateMountedEntityListByDummyChar( ioGameEntity *pEntity, MountedEntityList &vMountEntityList, ioBaseChar* pOwner );

	bool UpdateMountedWeaponList( ioGameEntity *pEntity, MountedWeaponList &vMountWeaponList );
	bool UpdateMountedWeaponListByBox( ioGameEntity *pEntity, MountedWeaponList &vMountWeaponList );

	float GetNearMountEntity( ioGameEntity* pEntity );

	void DestroyWeaponList();
	void DestroyAllPushStruct();
	void DestroyPushStructByEtcCode( DWORD dwEtcCode );
	void DestroyAllBallStruct();
	void DestroyAllDummyChar();
	void DestroyAllRouletteEntity();

	void DestroyDieMachineStruct();
	void DestroyAllMachineStruct();

	void AddWeaponExplosionEffect( ioPlayEntity *pEntity,
								   ioWeapon *pWeapon,
								   ApplyWeaponExp eType );
	
	void AddWeaponExplosionEffect( ioPlayEntity *pEntity,
								   const ioHashString &szEffName,
								   const ioHashString &szSound,
								   const D3DXVECTOR3 &vAttackDir,
								   DWORD dwStopDuration,
								   float fLooseRate );

	void AddWeaponExtraExplosionEffect( ioBaseChar *pAttackChar,
										ioBaseChar *pWoundChar,
										const ioHashString &szEffName,
										const D3DXVECTOR3 &vAttackDir,
										DWORD dwStopDuration,
										float fLooseRate );

	bool ReCalculateMoveVectorByTerrain( ioBaseChar *pChar, D3DXVECTOR3 *pMove, OUT bool &bCol );
	bool ReCalculateMoveVectorByTerrain( ioDummyChar *pDummy, D3DXVECTOR3 *pMove, OUT bool &bCol );
	bool ReCalculateMoveVectorByTerrain( ioDummyChar *pDummy, D3DXVECTOR3 *pMove, OUT bool &bCol, const MountedEntityList& rkMount );
	bool CheckMapCollision( ioBaseChar *pChar, const ioRay &rkRay, const D3DXVECTOR3 &vMovePos );

public:
	void ShakeCameraStart( const ShakeCamera &rkShake, int iPriority = 0 );
	void RollCameraStart( const ShakeCamera &rkShake );

	D3DXVECTOR3 GetCameraShakeAmt();
	float GetCameraRollAmt();

	void StartSelectCharCameraEvent();
	void SoldierSelectWndForcedShow();

protected:
	void ProcessCharCollisionLocal();
	void ProcessCharCollisionNetwork();
	void ProcessMovingPushEntityCollision();
	void ProcessDontMoveEntityCollision();
	void ProcessBallEntityCollision();
	void ProcessDummyCharEntityCollision();
	void ProcessNPCEntityCollision();

	bool CalcEntityCollisionPushAmt( const ioPlayEntity *pFirst,
									 const ioPlayEntity *pSecond,
									 D3DXVECTOR3 &vPushDir,
									 float &fPushAmt );

	void DestroyUnAvailableFieldItem();
	void DestroyUnAvailableFieldItemByTime();
	void DestroyUnAvailableFieldRewardItem();
	void DestroyDiePushStruct();
	void DestroyDieDummyChar();
	void DestroyRouletteEntity();

	void ProcessByOwner( ioCamera *pCamera );
	void ProcessFunctionKey();

	bool CheckObserverKey();

	void CheckShadowCastObjects( ioCamera *pCamera );
	void CheckOwnerCanPickItems();
	void CheckOwnerCanTackMachine();
	void CheckCharMouseClick( ioCamera *pCamera );

	void CheckOwnerCanStartRoulette();

	void SaveOwnerPositionToClipBoard();

public:
	bool TestWorldMeshCollision( const ioRay &rkRay, D3DXVECTOR3 *pColPoint );
	bool TestWorldMeshCollision( const ioOrientBox &rkBox );
	bool CheckCollisionLine( const D3DXVECTOR3 &vStart, const D3DXVECTOR3 &vEnd, bool bCheckStruct=true );
	bool CheckCollisionLine( const D3DXVECTOR3 &vStart, const D3DXVECTOR3 &vEnd, D3DXVECTOR3& vColPos, bool bCheckStruct=true, bool bColCharSkip=true );

	bool CheckCollisionFishingGroundAndPoint( const D3DXVECTOR3 &vStart, const D3DXVECTOR3 &vEnd );
	bool CheckCollisionFishingGroundAndFishingGround( ioPushStruct* pPushStruct );

protected:
	ioBaseChar* GetModeCameraTarget();
	ioBaseChar* CheckCameraTarget();

	void UpdateCamera( ioCamera *pCamera );
	D3DXVECTOR3 UpdateCameraFixedLookAt( ioBaseChar *pTarget, D3DXVECTOR3 vNewLookAt );
	void UpdateCameraPos( ioCamera *pCamera, ioBaseChar *pTarget, D3DXVECTOR3 vNewLookAt );
	void UpdateObserverCameraPos( ioCamera *pCamera, ioBaseChar *pTarget, D3DXVECTOR3 vNewLookAt );

	void UpdateCameraByMouse( ioCamera *pCamera );
	bool IsNowMouseCamera() const;

	float MouseSpringDistDamp( float fSpringConst,
							   float fDampConst,
							   float fCurDist,
							   float fTargetDist,
							   float fPreDist );

	float ReduceMouseCamAmount( float fPreAmt, float fReduceAmt );

	D3DXVECTOR3 CheckMapCameraLimit( D3DXVECTOR3 vNewLookAt ) const;

	D3DXVECTOR3 UpdateNewLookAt( ioBaseChar *pTarget, const D3DXVECTOR3 &vPreLookAt );
	D3DXVECTOR3 UpdateNewObserverLookAt( ioBaseChar *pTarget, const D3DXVECTOR3 &vPreLookAt );

	float GetArcadeCameraDistance( ioBaseChar *pTarget );
	void GetArcadeCameraGapXY(  ioBaseChar *pTarget, float &rXGap, float &rYGap, bool &bMeHighPos, float fGapOffset );
	D3DXVECTOR3 UpdateArcadeNewLookAt( ioBaseChar *pTarget, const D3DXVECTOR3 &vPos );

	D3DXVECTOR3 UpdateNewTPSLookAt( ioBaseChar *pTarget,
									const D3DXVECTOR3 &vPreLookAt );
	void UpdateTPSCameraPos( ioCamera *pCamera, ioBaseChar *pTarget, D3DXVECTOR3 vNewLookAt );

	D3DXVECTOR3 SpringDamp( float fSpringConst,
							float fDampConst,
							const D3DXVECTOR3 &vCurPos,
							const D3DXVECTOR3 &vTarPos,
							const D3DXVECTOR3 &vPreTarPos,
							float fEpsilon = FLOAT05 );

	D3DXVECTOR3 RunDashCamera( const D3DXVECTOR3 &vLookAt,
							   const D3DXVECTOR3 &vTarPos,
							   float &fFov,
							   ioBaseChar *pTarget );

	D3DXVECTOR3 ChargingCamera( const D3DXVECTOR3 &vLookAt,
							   const D3DXVECTOR3 &vTarPos,
							   float &fFov,
							   ioBaseChar *pTarget );

	D3DXVECTOR3 BuffCamera( const D3DXVECTOR3 &vLookAt,
					 	    const D3DXVECTOR3 &vTarPos,
						    float &fFov,
						    ioBaseChar *pTarget );

	D3DXVECTOR3 SkillCamera( const D3DXVECTOR3 &vLookAt,
					 	     const D3DXVECTOR3 &vTarPos,
						     float &fCamFov,
						     ioBaseChar *pTarget );

public:
	void StartCameraPause( DWORD dwPauseTime = 0 );
	void EndCameraPause();
	bool IsCameraPause();

public:
	void CallBackCameraEventEnd( DWORD dwEndCode );
	void StartTurnStartEventCamera( DWORD dwEventSceneID, DWORD dwNextCameraEvent, const ioHashString &rkNextTarget );
	void StartModeStartEventCamera( bool bFirst, bool bSceneEndSendPacket = true, DWORD dwEventSceneID = 0 );
	void UpdateEventSceneCamera( ioCamera *pCamera );
	void SetModeEndSceneCamera( bool bEndScene );
	void UpdateEndSceneCamera( ioCamera *pCamera );
	void EndModeStartEventCamera();

	void SetCameraToOwnerExplicit( ioBaseChar *pOwner );
	void SetTargetLookAt( bool bTarget );

	void SetCameraModeFixedBlock( bool bBlock );
	void SetCameraModeFixed( DWORD dwStyle );
	void SetCameraModeFixedByHousing();
	void ClearCameraModeFixed( DWORD dwStyle );
	bool IsOwnerViewModeCameraFiexed();

protected:
	void RenderSceneNodeBox();
	void RenderSceneNode( ioSceneNode *pNode );

	void RenderCharBox();
	void RenderWeaponBox();

public:
	void RenderEntitySphere();
	void RenderWeaponSphere();
	void RenderSphere( float fRadius, const D3DXQUATERNION &qtOrientaton, const D3DXVECTOR3 &vPosition, const D3DXVECTOR3 &vScale, DWORD dwColor );

#ifndef SHIPPING
protected:
	void RenderModeEntity();
#endif

public:
	void RenderBox( const D3DXVECTOR3 *pVtxArray, DWORD dwColor );
	void RenderGrid( const D3DXVECTOR3 *pVtxArray, DWORD dwColor );
	void RenderLine( const D3DXVECTOR3 *pVtxArray, DWORD dwColor );

protected:
	void RenderOwnerCharState();
	void RenderOverdItemName();
	void RenderAllFieldItemIcon();

	void UpdateEntityGauge();
	void RenderEntityInfo();
	void RenderEntityGauge();
	void RenderEntityDamage();

public:
	void UpdateCharChatBubble();
	void RenderCharChatBubble();

public:
	void UpdateCharEmoticon();
	void RenderUnderUIEmoticon();
	void RenderUpperBlindEmoticon();
	void RenderAllEmoticon();

	void UpdateShuffleModeUI();
	void RenderShuffleModeUI();

	void RenderBuffBlind();

public:
	void NotifyOwnerCameraLoseTime( DWORD dwLoseTime );
	void NotifyOwnerRevival();
	void ChangeCurViewChar( const ioHashString &rkName, int iMaxChar );
	void ChangeCurViewCharByDie( ioBaseChar *pChar );
	void ChangeCurViewCharByView( ioBaseChar *pChar );

	ioBaseChar* GetNextViewChar( const ioHashString& rkName );

	void NotifyOwnerSendControlInfo();
	void NotifyPushStructSendInfo();
	void NotifyBallStructSendInfo();

	void NotifySpringDampUseSet( bool bSpringDampUse );
	bool IsNoCharInputWndOpen();

public:
	ioBaseChar* AddNewUser( const ioHashString &rkName, SP2Packet &rkPacket );
	void RemoveUser( const ioHashString &rkName ,bool isRoomOut );

	void ChangeCharModel( ioBaseChar *pChar, const ioHashString &rkName, DWORD dwCharIndex,  SP2Packet &rkPacket, bool bFastSync, byte eAwakeType );
	ioBaseChar* ChangeCharModel( const ioHashString &rkName, DWORD dwCharIndex, TeamType eTeam, SP2Packet &rkPacket, byte eAwakeType );
	void ChangeCharEquipItem( ioBaseChar *pChar, float fPrevSkillGauge[MAX_INVENTORY], SP2Packet &rkPacket, bool bFastSync = false, bool bBornChange = false );
	void ClearUserCreateEntities( const ioHashString &rkName );

public:
	bool CheckSensingMine( ioMineWeapon *pMine, float fSensingRange );
	bool CheckSensingMine2( ioMineWeapon2 *pMine, float fSensingRange );
	bool CheckSensingMine3( ioMineWeapon3 *pMine, float fSensingRange );
	bool CheckSensingMine4( ioMineWeapon4 *pMine, float fSensingRange );
	bool CheckSensingMine5( ioMineWeapon5 *pMine, float fSensingRange );
	bool CheckSensingMine6( ioMineWeapon6 *pMine, float fSensingRange );

public:
	void ReCalculateSymbolHP( int iCharCnt );
	void ReCalculateDamageRate( bool bRestart );
	void ReCalculateDamageRateByMyRoom();
	void SetMemberBalanceMsg();

	void IncreaseTeamCntForBalance( TeamType eTeam );
	void DecreaseTeamCntForBalance( TeamType eTeam );

	void SetChatIcon( const ioHashString &szName,
					  const ioHashString &szChat,
					  bool bNoSound, 
					  bool bCusomSound );

	bool IsNetworkMode();	
	bool IsCollisionCheckByMode();
	bool IsOneMyChar();
	bool IsRoundSetEnd();

	void AddMySoldier( DWORD dwIndex, bool bBuySoldier = false  );
	void RemoveMySoldier( DWORD dwIndex );
	void ReLoadMySoldier( int iCharArray );
	void ReLoadAllMySoldier();
	void ReLoadCharInfo( DWORD dwCharIndex, CHARACTER &rkInfo );
	void MySoldierDefPos();
	void SetOneMyChar();
	void SetAllMyChar();
	void TreasureCardRandom( short &rPresentType, int &rPresentValue1, int &rPresentValue2 );

	float GetMemberBalance( TeamType eTeam );
	float GetMemberBalanceForGauge( TeamType eTeam );
	float GetMemberBalanceForBlowTime( TeamType eTeam );
	float GetMemberBalanceForDropDamage( TeamType eTeam, float fDamage );
	float GetMemberBalanceForStateProtectionTime( TeamType eTeam, int iType );
	float GetMemberBalanceForBuffItemDrop( TeamType eTeam );
	float GetMemberBalanceForMoveSpeed( TeamType eTeam );

	inline int GetRedMember() const { return m_iRedForBalance; }
	inline int GetBlueMember() const { return m_iBlueForBalance; }

public:
	void SendOwnerDamageList( bool bLast = false );
	void CheckFlyMapLimit( D3DXVECTOR3 *pFlyPos );
	void EnableWorldEntityFunctions( bool bEnable );

	void CheckObserverCameraTarget( const ioHashString& szName );
	bool IsStealthMode();

	bool CheckAxisLimit( D3DXVECTOR3 *pNewPos );
	bool CheckAxisLimit( const D3DXVECTOR3& vNewPos );

public:
	void SetScreenShotMode( bool bStart );

public:
	void ContributeLOG();
	int ContributeMyRank( int &rRankValue1, int &rRankValue2, int &rRankValue3, int &rRankValue4 );
	int ContributeRank( const ioHashString &rkName, int &rRankValue1, int &rRankValue2, int &rRankValue3, int &rRankValue4 );

	void RecordLOG();

	void SetReservedExit( bool bExit ) { m_bReservedExit = bExit; }
	bool IsReservedExit() const { return m_bReservedExit; }
	
	VoiceChatType GetVoiceChatType() const { return m_eVoiceChatType; }
	void SetBlindAniEnd( bool bEnd ){ m_bBlindAniEnd = bEnd; }

	void UpdateGrowthData();
	void UpdateGrowthDataOne( const ioHashString &szName );

	ioGrowthLevel* GetGrowthLevel( const ioHashString &szName );

	int GetKickOutVoteProposal(){ return m_iRoomKickOutVoteProposal; }

	// 이벤트 구조물 체크
	void CheckEventStruct( int iEventType );				// 본부에 들어갈때
	void CheckEventStruct( int iEventType, bool bAlive );	// 서버로부터 정보 받았을때
	bool IsExistEventStruct( int iEventType );
	
	void SkeletonEtcItemSync( const ioHashString &rkName, int iEtcItem );

public:
	void ChangeMultiMapIndex( int iMapIndex );

public:
	// NPC 첫번째 입장 유저를 동기화 주체로 변경
	void AddAllNPCSyncFirstUser();
	// NPC 타겟 고정
	void SetMonsterTargetFixing( const ioHashString &rkTargetName, float fFixingTime );

protected:
	void OnHostUserID( SP2Packet &rkPacket );
	void OnJoinRoomData( SP2Packet &rkPacket );
	void OnModeInfo( SP2Packet &rkPacket );
	void OnJoinPlayingUserData( SP2Packet &rkPacket );
	void OnJoinUserData( SP2Packet &rkPacket );
	void OnLeaveUserData( SP2Packet &rkPacket );
	void OnCreateRoom( SP2Packet &rkPacket );
	void OnJoinRoom( SP2Packet &rkPacket );
	void OnExitRoom( SP2Packet &rkPacket );
	void OnChangeStage( SP2Packet &rkPacket );
	void OnChangeUDPInfo( SP2Packet &rkPacket );
	void OnPlayRecordInfo( SP2Packet &rkPacket );
	void OnParentCharEquipData( SP2Packet &rkPacket );
	void OnCharEquipData( SP2Packet &rkPacket );
	void OnFieldItemList( SP2Packet &rkPacket );
	void OnDropItem( SP2Packet &rkPacket );
	void OnItemMoveDrop( SP2Packet &rkPacket );
	void OnRepositionFieldItem( SP2Packet &rkPacket );
	void OnPickItem( SP2Packet &rkPacket );
	void OnPickItemFail( SP2Packet &rkPacket );
	void OnDeleteFieldItem( SP2Packet &rkPacket );
	void OnDeleteFieldItemFail( SP2Packet &rkPacket );
	void OnPickRewardItem( SP2Packet &rkPacket );
	void OnDropItemFail( SP2Packet &rkPacket );
	void OnDropDie( SP2Packet &rkPacket );
	void OnWeaponDie( SP2Packet &rkPacket );
	void OnRevivalTime( SP2Packet &rkPacket );
	
	ioBaseChar *_OnChangeChar( SP2Packet &rkPacket );
	void OnParentChangeChar( SP2Packet &rkPacket, bool bUdp );
	
	void OnJoinRoomInfoEnd( SP2Packet &rkPacket );
	void OnStartSetNextMode( SP2Packet &rkPacket );
	void OnEndSetNextMode( SP2Packet &rkPacket );
	void OnAwardingInfo( SP2Packet &rkPacket );
	void OnAwardingResult( SP2Packet &rkPacket );
	void OnFollowUserResult( SP2Packet &rkPacket );
	void OnRequestBankruptResult( SP2Packet &rkPacket );
	void OnEtcItemBuyResult( SP2Packet &rkPacket );
	void OnRefillCoin( SP2Packet &rkPacket );	
	void OnPlazaRoomList( SP2Packet &rkPacket );
	void OnPlazaCommand( SP2Packet &rkPacket );
	void OnPlazaInviteList( SP2Packet &rkPacket );
	void OnHeadquartersInviteList( SP2Packet &rkPacket );
	void OnHouseInviteList( SP2Packet &rkPacket );
	void OnPlazaInvite( SP2Packet &rkPacket );
	void OnImmediatelyEquipItem( SP2Packet &rkPacket );
	void OnEtcItemUse( SP2Packet &rkPacket );
	void OnEtcItemSell( SP2Packet &rkPacket );
	void OnEtcItemAdd( SP2Packet &rkPacket );
	void OnMovieControl( SP2Packet &rkPacket );
	void OnAbsorbInfo( SP2Packet &rkPacket );
	void OnChatModeState( SP2Packet &rkPacket );
	void OnExperienceModeState( SP2Packet &rkPacket );

	void OnHeadquartersInfo( SP2Packet &rkPacket );
	void OnHouseInfo( SP2Packet &rkPacket );

	void OnHeadquartersJoinRequest( SP2Packet &rkPacket );
	void OnHouseJoinRequest( SP2Packet &rkPacket );

	void OnHeadquartersInvite( SP2Packet &rkPacket );
	void OnHouseInvite( SP2Packet &rkPacket );

	void OnGrowthPointBonus( SP2Packet &rkPacket );
	void OnGrowthLevelUp( SP2Packet &rkPacket );
	void OnGrowthLevelDown( SP2Packet &rkPacket );
	void OnGrowthAllLevelDown( SP2Packet &rkPacket );
	void OnGrowthLevelInit( SP2Packet &rkPacket );
	void OnLevelGrowth( SP2Packet &rkPacket );

	void OnTimeGrowthAdd( SP2Packet &rkPacket );
	void OnTimeGrowthRemove( SP2Packet &rkPacket );

	void OnFishingState( SP2Packet &rkPacket );
	void OnItemCompound( SP2Packet &rkPacket );
	void OnItemMaterialCompound( SP2Packet &rkPacket );

	void OnItemGrowthCatalyst( SP2Packet &rkPacket );
	void OnItemCompoundEx( SP2Packet &rkPacket );

	void OnAllItemDrop( SP2Packet &rkPacket );

	void OnExtraItemBuyResult( SP2Packet &rkPacket );
	void OnExtraItemSellResult( SP2Packet &rkPacket );
	void OnExtraItemChange( SP2Packet &rkPacket );
	void OnExtraItemDisassembleResult( SP2Packet &rkPacket );

	void OnMedalItemSellResult( SP2Packet &rkPacket );
	void OnCustomMedalItemSellResult( SP2Packet &rkPacket );

	void OnSendPresentByEtcItemFail( SP2Packet &rkPacket );
	void OnSoldierExpAddUpdate( SP2Packet &rkPacket );
	void OnRechargeExtraItem( SP2Packet &rkPacket );
	void OnRechargeAccessory( SP2Packet &rkPacket );
	void OnCompoundAccessory( SP2Packet &rkPacket );
	void OnReinforceAccessory( SP2Packet &rkPacket );
	void OnReinforceExtraItem( SP2Packet &rkPacket );

	// Trade
	void OnTradeStateChange( SP2Packet &rkPacket );
	void OnTradeCreate( SP2Packet &rkPacket );
	void OnTradeItemBuy( SP2Packet &rkPacket );
	void OnTradeItemCancel( SP2Packet &rkPacket );
	void OnTradeTimeOut( SP2Packet &rkPacket );
	
	void OnUserKickVote( SP2Packet &rkPacket );
	void OnLadderTeamRankList( SP2Packet &rkPacket );
	void OnLadderUserHQMove( SP2Packet &rkPacket );
	void OnEtcItemAction( SP2Packet &rkPacket );
	void OnEtcItemSwitch( SP2Packet &rkPacket );	
	void OnEtcItemLuckyCoinUseResult( SP2Packet &rkPacket );
	void OnEtcItemRainbowMixerResult( SP2Packet &rkPacket );
	void OnEtcItemLostSagaMixerResult( SP2Packet &rkPacket );
	void OnEtcItemTimeGashaponUpdate( SP2Packet &rkPacket );
	void OnEtcItemGoldBoxResult( SP2Packet &rkPacket );
	void OnRoomUserEtcItemSync( SP2Packet &rkPacket );	
	void OnEtcItemSkeletonDelete( SP2Packet &rkPacket );
	void OnMyRoomServerChange( SP2Packet &rkPacket );
	void OnPresentBuy( SP2Packet &rkPacket );
	void OnExcavation( SP2Packet &rkPacket );
	void OnGradeSync( SP2Packet &rkPacket );

	// MachineStruct
	void OnMachineStruct( SP2Packet &rkPacket );

	void OnEtcItemMotionStateTCP( SP2Packet &rkPacket );

	void OnFieldRewardItemList( SP2Packet &rkPacket );
	void OnFieldRewardItemChange( SP2Packet &rkPacket );

	// exception exit room
	void ExceptionExitRoom();

	// 청약철회
	void OnSubscriptionBuy( SP2Packet &rkPacket );
	void OnSubscriptionRetrCheck( SP2Packet &rkPacket );

	void OnTimeCashInfo( SP2Packet &rkPacket );

	void OnRaidCoinInfo( SP2Packet &rkPacket );

public:
	void OnFieldItemSupply( SP2Packet &rkPacket );
	void OnBadPingCrownDrop( SP2Packet &rkPacket );
	void OnCharEquipMedalItemData( SP2Packet &rkPacket );
	void OnCharGrowthLevelData( SP2Packet &rkPacket );
	void OnCharNamedTitleData( SP2Packet &rkPacket );
	void OnCharEquipAccessoryData( SP2Packet &rkPacket );
	void OnBadPingFlagDrop( SP2Packet &rkPacket );

public:
	void OnArenaModeCharCreate( SP2Packet &rkPacket );

protected:
	void OnChat( SP2Packet &rkPacket );
	void OnHoleCheck( SP2Packet &rkPacket );
	void OnSyncTime( SP2Packet &rkPacket );

	// user connect..
	void OnNewUserInfo( SP2Packet &rkPacket );
	void OnExistUserInfo( SP2Packet &rkPacket );

	// play..
	void OnControl( SP2Packet &rkPacket );
	void OnAttack( SP2Packet &rkPacket );
	void OnDefense( SP2Packet &rkPacket );
	void OnDefenseRelease( SP2Packet &rkPacket );
	void OnPreDefenseRelease( SP2Packet &rkPacket );
	void OnJump( SP2Packet &rkPacket );
	void OnCrossBowJump( SP2Packet &rkPacket );
	void OnCrossBowJumpAim( SP2Packet &rkPacket );
	void OnCrossBowAim( SP2Packet &rkPacket );
	void OnFlyJump( SP2Packet &rkPacket );
	void OnBoostJump( SP2Packet &rkPacket );
	void OnDoubleJump( SP2Packet &rkPacket );
	void OnBuffJump( SP2Packet &rkPacket );
	void OnBuffFlyJump( SP2Packet &rkPacket );
	void OnBuffMultiFlyJump( SP2Packet &rkPacket );
	void OnJumpAttack( SP2Packet &rkPacket );
	void OnJumpReload( SP2Packet &rkPacket );
	void OnExtendJump( SP2Packet &rkPacket );
	void OnComboJump( SP2Packet &rkPacket );
	void OnAimJump( SP2Packet &rkPacket );
	void OnJumpping( SP2Packet &rkPacket );
	void OnDashAttack( SP2Packet &rkPacket );
	void OnPickItemTry( SP2Packet &rkPacket );
	void OnDeleteFieldItemTry( SP2Packet &rkPacket );
	void OnPickRewardItemTry( SP2Packet &rkPacket );
	void OnUseSkill( SP2Packet &rkPacket );
	void OnUseAutoSkill( SP2Packet &rkPacket );
	void OnWeaponDefense( SP2Packet &rkPacket );
	void OnWeaponWounded( SP2Packet &rkPacket );
	void OnExtendDefense( SP2Packet &rkPacket );
	void OnDropDieMotion( SP2Packet &rkPacket );
	void OnBlowGetUp( SP2Packet &rkPacket );
	void OnDropDownGetUp( SP2Packet &rkPacket );
	void OnSystemDamage( SP2Packet &rkPacket );
	void OnMineWaitExplosion( SP2Packet &rkPacket );
	void OnNormalDeadMine( SP2Packet &rkPacket );
	void OnMineExplosion( SP2Packet &rkPacket );
	void OnCounterAttack( SP2Packet &rkPacket );
	void OnExplosionOnHand( SP2Packet &rkPacket );
	void OnExplosionDrop( SP2Packet &rkPacket );
	void OnItemGauge( SP2Packet &rkPacket );
	void OnFallDamage( SP2Packet &rkPacket );
	void OnRequestControl( SP2Packet &rkPacket );
	void OnChangeDashRun( SP2Packet &rkPacket );
	void OnMagicCircle( SP2Packet &rkPacket );
	void OnReturnAttackExplicit( SP2Packet &rkPacket );
	void OnDummyExplicit( SP2Packet &rkPacket );
	void OnTraceWeaponTrace( SP2Packet &rkPacket );
	void OnTraceWeaponWait( SP2Packet &rkPacket );
	void OnJumpDash( SP2Packet &rkPacket );
	void OnAreaWeaponInfo( SP2Packet &rkPacket );
	void OnCurseRolling( SP2Packet &rkPacket );
	void OnDownRolling( SP2Packet &rkPacket );
	void OnBuffAirAttack( SP2Packet &rkPacket );

	void OnPushStructCreateWeapon( SP2Packet &rkPacket );
	void OnPushStructSearchState( SP2Packet &rkPacket );
	void OnPushStructTracingState( SP2Packet &rkPacket );
	void OnPushStructFireState( SP2Packet &rkPacket );
	void OnPushStructMove( SP2Packet &rkPacket );
	void OnPushStructDelay( SP2Packet &rkPacket );
	void OnSkillExtraInfo( SP2Packet &rkPacket );
	void OnEquipSkillExtraInfo( SP2Packet &rkPacket );
	void OnPushStructInfo( SP2Packet &rkPacket );
	void OnFieldItemInfo( SP2Packet &rkPacket );
	void OnAimFire( SP2Packet &rkPacket );
	void OnDrink( SP2Packet &rkPacket );	
	void OnDropThrowBomb( SP2Packet &rkPacket );
	void OnDropSmartBomb( SP2Packet &rkPacket );
	void OnWoundedPoweredArmor( SP2Packet &rkPacket );
	void OnBattleRoomRoundEnd( SP2Packet &rkPacket );
	void OnPassage( SP2Packet &rkPacket );
	void OnFrozenRelease( SP2Packet &rkPacket );
	void OnFloatRelease( SP2Packet &rkPacket );
	void OnCloseOrderRelease( SP2Packet &rkPacket );
	void OnFallState( SP2Packet &rkPacket );
	void OnEndBuff( SP2Packet &rkPacket );
	void OnEndArea( SP2Packet &rkPacket );
	void OnSkillEnd( SP2Packet &rkPacket );
	void OnMovingRoom( SP2Packet &rkPacket );
	void OnCharSlotChange( SP2Packet &rkPacket );
	void OnAddBuffBySkill( SP2Packet &rkPacket );
	void OnRemoveBuff( SP2Packet &rkPacket );
	void OnPreventTargetBuff( SP2Packet &rkPacket );
	void OnElevatorChanged( SP2Packet &rkPacket );
	void OnCloseOrderDirection( SP2Packet &rkPacket );
	void OnBreakFallState( SP2Packet &rkPacket );
	void OnPlazaJoinInfo( SP2Packet &rkPacket );
	void OnEmoticonSync( SP2Packet &rkPacket );
	void OnTeleportState( SP2Packet &rkPacket );
	void OnChangeCharInfo( SP2Packet &rkPacket );
	void OnNonePlayState( SP2Packet &rkPacket );
	void OnSpecialMotionState( SP2Packet &rkPacket );
	void OnEtcItemMotionStateUDP( SP2Packet &rkPacket );
	void OnEtcItemMotionOptionFirstSync( SP2Packet &rkPacket );
	void OnEtcItemMotionOptionSync( SP2Packet &rkPacket );
	void OnUserInfoRefresh( SP2Packet &rkPacket );
	void OnUserInfoSync( SP2Packet &rkPacket );
	void OnCharExtraItemChange( SP2Packet &rkPacket );
	void OnNpcStuckState( SP2Packet &rkPacket );
	void OnMonsterCoinFirstSync( SP2Packet &rkPacket );
	void OnMonsterCoinSync( SP2Packet &rkPacket );
	void OnMonsterGlobalTarget( SP2Packet &rkPacket );
	void OnChainCollision( SP2Packet &rkPacket );
	void OnChainChange( SP2Packet &rkPacket );
	void OnJumpAutoShot( SP2Packet &rkPacket );
	void OnExcavationUDP( SP2Packet &rkPacket );
	void OnDodgeDefense( SP2Packet &rkPacket );
	void OnBallWeaponCollision( SP2Packet &rkPacket );
	void OnBallWeaponChange( SP2Packet &rkPacket );
	void OnMachineStructUDP( SP2Packet &rkPacket );

	void OnMedalItemBuy( SP2Packet &rkPacket );
	void OnMedalItemChange( SP2Packet &rkPacket );
	void OnCustomMedalItemChange( SP2Packet &rkPacket );

	void OnExpandMedalSlotOpen( SP2Packet &rkPacket );

	void OnHeroMatchOtherName( SP2Packet &rkPacket );
	void OnHeroMatchOtherInfo( SP2Packet &rkPacket );

	void OnP2PPing( SP2Packet &rkPacket );
	void OnShopping( SP2Packet &rkPacket );
	void OnGuildInfoChange( SP2Packet &rkPacket );
	void OnUserPosIndex( SP2Packet &rkPacket );
	void OnRelayChat( SP2Packet &rkPacket );	
	void OnFirstSyncCharInfo( SP2Packet &rkPacket );
	void OnSyncCharInfo( SP2Packet &rkPacket );
	void OnPushStructBomber( SP2Packet &rkPacket );
	void OnWeaponByWeaponExplicit( SP2Packet &rkPacket );
	void OnWeaponByWeaponExplicitList( SP2Packet &rkPacket );
	void OnReflectAttackExplicit( SP2Packet &rkPacket );

	void OnCollectJumpAttack( SP2Packet &rkPacket );
	void OnNewJumpAttack( SP2Packet &rkPacket );
	void OnThrowBombBound( SP2Packet &rkPacket );
	void OnSmartBombBound( SP2Packet &rkPacket );
	void OnDefenseAttack( SP2Packet &rkPacket );
	void OnCreateStructState( SP2Packet &rkPacket );

	void OnWeaponSync( SP2Packet &rkPacket );
	void OnExtendDash( SP2Packet &rkPacket );
	void OnGrapplingSync( SP2Packet &rkPacket );
	void OnDummyCharSync( SP2Packet &rkPacket );
	void OnBlowDash( SP2Packet &rkPacket );
	void OnCounterAttackSync( SP2Packet &rkPacket );

	void OnHideObjectWeaponIndexSync( SP2Packet &rkPacket );

	void OnWereWolfState( SP2Packet &rkPacket );

	void OnCharActionInfo( SP2Packet &rkPacket );

	void CheckPassiveGauge( SP2Packet &rkPacket );
	void OnWitchState( SP2Packet &rkPacket );
	void OnNakoruruState( SP2Packet &rkPacket );

	void OnGPState( SP2Packet &rkPacket );

	void OnBuffExtraInfo( SP2Packet &rkPacket );
	void OnCheckEscapeSkill( SP2Packet &rkPacket );

	void OnSmartBoomObjectItemGenerateWeapon( SP2Packet &rkPacket );

	// voice chat..
	void OnPushVoiceData( SP2Packet &rkPacket );
	void OnVoiceInfo( SP2Packet &rkPacket );

	//파티
	void OnBattleRoomList( SP2Packet &rkPacket );
	void OnBattleRoomCreate( SP2Packet &rkPacket );
	void OnBattleRoomInfo( SP2Packet &rkPacket );
	void OnBattleRoomUserInfo( SP2Packet &rkPacket, bool bCompleteCheck = true );
	void OnBattleRoomJoin( SP2Packet &rkPacket );
	void OnBattleLeaveUser( SP2Packet &rkPacket );

	void OnMacroCmd( SP2Packet &rkPacket );
	void OnMacroCmdByRoomNamePw( SP2Packet &rkPacket );
	void OnMacroCmdByKickOut( SP2Packet &rkPacket );
	void OnMacroCmdByPayCnt( SP2Packet &rkPacket );

	void OnBattleRoomInviteList( SP2Packet &rkPacket );
	void OnBattleRoomInvited( SP2Packet &rkPacket );		
	void OnBattleRoomJoinInfo( SP2Packet &rkPacket );
	void OnBattleRoomCmd( SP2Packet &rkPacket );
	void OnBattleRoomInviteResult( SP2Packet &rkPacket );	
	void OnBattleRoomEnterUserSync( SP2Packet &rkPacket );

	// 셔플모드
	void OnShuffleRoomJoinCancel( SP2Packet &rkPacket );
	void OnShuffleRoomCmd( SP2Packet &rkPacket );
	void OnShuffleRoomCmdByCreateOK( SP2Packet &rkPacket );
	void OnShuffleRoomCmdByReadyGoOK( SP2Packet &rkPacket  );
	void OnShuffleRoomCmdByKickOut( SP2Packet &rkPacket );

	void OnShuffleRoomInfo( SP2Packet &rkPacket );
	void OnShuffleRoomUserInfo( SP2Packet &rkPacket );
	void OnShuffleRoomEnterUserSync( SP2Packet &rkPacket );
	void OnShuffleRoomLeaveUser( SP2Packet &rkPacket );
	
	//길드
	void OnGuildRankList( SP2Packet &rkPacket );
	void OnLadderTeamList( SP2Packet &rkPacket );
	void OnCreateLadderTeam( SP2Packet &rkPacket );
	void OnLadderTeamInfo( SP2Packet &rkPacket );
	void OnLadderTeamUserInfo( SP2Packet &rkPacket );
	void OnLadderTeamLeave( SP2Packet &rkPacket );
	void OnLadderTeamMacro( SP2Packet &rkPacket );
	void OnLadderTeamJoinInfo( SP2Packet &rkPacket );
	void OnJoinLadderTeam( SP2Packet &rkPacket );
	void OnLadderTeamInviteList( SP2Packet &rkPacket );
	void OnLadderTeamInvited( SP2Packet &rkPacket );
	void OnLadderTeamRanking( SP2Packet &rkPacket );
	void OnLadderOtherTeamInfo( SP2Packet &rkPacket );
	void OnLadderOtherNameChange( SP2Packet &rkPacket );

	// custom sound
	void OnCustomSoundInfo( SP2Packet &rkPacket );
	void OnCustomSoundData( SP2Packet &rkPacket );

	//for ioSummonWoodSkill
	void OnSummonDummyInfo( SP2Packet &rkPacket );

	//Roulette
	void OnRouletteStartAni( SP2Packet &rkPacket );
	void OnRouletteStart( SP2Packet &rkPacket );
	void OnRouletteJoinEnd( SP2Packet &rkPacket );
	void OnRouletteEnd( SP2Packet &rkPacket );
	void OnRouletteException( SP2Packet &rkPacket );

	void OnBingoData( SP2Packet &rkPacket );
	void OnBingoNumberInit( SP2Packet &rkPacket );

public:
	bool CheckRoomChangeChar();
	bool CheckRoomWeakWounded( TeamType eTeam );
	bool CheckRoomFastGetUp();
	bool CheckRoomZeroKO();
	bool CheckRoomKOEffect();
	bool CheckRoomEquip( TeamType eTeam );
	bool CheckRoomGrowthUse();
	bool CheckRoomExtraItemUse();

	float GetRoomGravity();
	float GetRoomFriction();
	float GetRoomCoolTimeRate();
	float GetRoomHPRate( TeamType eTeam );
	float GetRoomDropDamageRate();
	float GetRoomGetUpRate();
	float GetRoomMoveSpeedRate( TeamType eTeam );

	void DestroyAreaWeapon( DWORD dwIndex );

	void OnFastChangeChar( SP2Packet &rkPacket );
	void OnTraceMineFlyTraceStart( SP2Packet &rkPacket );
	void OnTraceMineWaitExplosion( SP2Packet &rkPacket );
	void OnTraceMineNormalDead( SP2Packet &rkPacket );
	void OnTraceMineExplosion( SP2Packet &rkPacket );
	void OnExpandMedalSlotData( SP2Packet &rkPacket );

	void OnSelectExtraGashaponResult( SP2Packet &rkPacket );

	void OnTrace2MineDelay( SP2Packet &rkPacket );
	void OnTrace2MineNormalDead( SP2Packet &rkPacket );
	void OnTrace2MineWaitExplosion( SP2Packet &rkPacket );
	void OnTrace2MineExplosion( SP2Packet &rkPacket );

	void OnAirMineWaitExplosion( SP2Packet &rkPacket );
	void OnAirNormalDeadMine( SP2Packet &rkPacket );
	void OnAirMineExplosion( SP2Packet &rkPacket );

//for ioSummonWoodSkill
public:
	void SetSummonDummyInfo( ioHashString pOwnerName, int iDummyIndex, D3DXVECTOR3 vPos, bool bSend = true );
	void RemoveSummonDummyInfo(  int iDummyIndex, bool bSend = true );
	SummonList GetSummonDummyInfoList();

	void SetChargingCamera( bool b ){ m_bCameraChanging = b; }

public:
	void OnSelectGashaponResult( SP2Packet &rkPacket );
	void OnBingoType( SP2Packet &rkPacket );
	void OnSelectGashaponsUseResult( SP2Packet &rkPacket );

	//펫관련
public:
	//펫 생성
	ioPet* CreatePet( const ioHashString &szOwnerName, const D3DXVECTOR3 &vTargetPos, bool bAddList = true );

public:
	//모든 펫 제거
	void DestroyAllPet();

protected:
	//엔드 펫 제거(엔티티 리스트에서 삭제)
	void DestroyDiePet();

//Accessory
protected:
	void OnPassiveSkillExtraInfo( SP2Packet &rkPacket );

protected:
	void OnShareSkillGauge( SP2Packet &rkPacket );

//연승전
protected:
	void OnSuccessionData( SP2Packet &rkPacket );
	void OnSuccessionMatchingResult( SP2Packet &rkPacket );
	void OnSuccessionMatchingCancel( SP2Packet &rkPacket );
	void OnSuccessionGameStart( SP2Packet &rkPacket );
	void OnSuccessionGameEnd( SP2Packet &rkPacket );
	void OnSuccessionRevengeRequest( SP2Packet &rkPacket );
	void OnSuccessionOtherUserInfo( SP2Packet &rkPacket );

	void OnPieceToPesoPacket( SP2Packet &rkPacket );

public:
	bool GetCurPetInfo( ioHashString szCharName, CurPetInfo &sCurPetInfo );
	void ClearPieceToPesoInfo();
	void GetPieceToPesoInfo( int &iPeso, int &iCount );

protected:
	void OnPlayingUserDataInfo( SP2Packet &rkPacket );	
	void OnPowerUpInfo( SP2Packet &rkPacket );
	void OnObjectItemRelease( SP2Packet &rkPacket );

protected:
	void OnLagAction( SP2Packet &rkPacket );

protected:
	void OnInitializeSpecialState( SP2Packet &rkPacket );
	void OnSpecialStateSync( SP2Packet &rkPacket );

public:
	ioPlayStage( ioGameSceneManager *pSceneMgr );
	virtual ~ioPlayStage();
};

inline ioPlayStage* ToPlayStage( ioGameStage *pStage )
{
	if( !pStage || pStage->GetTypeID() != ioGameStage::GS_PLAY )
		return NULL;

	return dynamic_cast< ioPlayStage* >( pStage );
}
