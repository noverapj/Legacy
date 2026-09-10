#pragma once

#include "ioBaseCharParent.h"
#include "ioBaseGUISupport.h"

#define CHAR_SYNC_TIME 45.0f
#define CHAR_SYNC_SPEED 35.0f
#define MIN_FRAME_TIME 33.4f

class ioChampionAI;
enum ObjectEqipState
{
	OES_NONE,

	OES_WEREWOLF,	
	OES_GP,
	OES_GALAXY,
	OES_THUNDER_BIRD,
	OES_ALICE,
	OES_PARKILPYO,
	OES_DARKKNIHT,
	OES_ONIMUSHA,
	OES_DRUID,
	OES_MIDNIGHT_SCYTHE,
	OES_MIDNIGHT_GUN,

	//항상 마지막에 유지
	OES_ALL,
};

class ioNpcAppearance;
class ioSpecialStateBase;

class ioBaseChar : public ioBaseCharParent, public ioBaseGUISupport
{
protected:
	ioStatePriority *m_pDefaultPriority;
	typedef std::list< ioScaleInfo > EnlargeList;

	// For Race Camera
	float m_DefaultCamHeight;
	float m_DefaultCamDistance;
	float m_DefaultCamFov;
	DWORD m_DefaultCamZoomTime;

	EnlargeList	m_ScaleList;
	bool	m_blHandScale;
	bool	m_brHandScale;
	bool	m_blFootScale;
	bool	m_brFootScale;
	DWORD	m_dwScaleTime;
	D3DXVECTOR3 m_vStartScale;

	ioBaseChar *m_pSightTarget;
	ioHashString m_ForceSightTarget;
	float	m_fSightExtraRot;

	DWORD        m_dwIndex;
	int          m_iLevel;
	int          m_iClassLevel;
	ioHashString m_Name;   
	ioHashString m_ViewName;

	DWORD m_dwNPCIndex;
	DWORD m_dwUserIndex;

	//UJ 130829, 최적화: state 조회가 매우 느리기 때문에 CEncrypt 쓰지않는다, 
	//CEncrypt<CharState> m_CharState;
	//CEncrypt<CharState> m_PreCharState;
	CharState m_CharState;
	CharState m_PreCharState;
	CharState m_NextCharState;
	ioSpecialStateBase* m_pCharSpeicalState;
	CharLandingType m_CharLandType;

	RaceType  m_RaceType;
	bool	  m_bMale;

	int       m_iSelectCharArray;

	DashType m_DefaultDashType;
	DashType m_CurDashType;
	JumpType m_DefaultJumpType;
	JumpType m_CurJumpType;
	CounterAttackType m_DefaultCAT;
	CounterAttackType m_CurCAT;

	bool m_bJumpAttackEnable;

	ioPlayStage *m_pCreator;
	ioEquipSlot *m_pEquipSlot;
	ioAniCollisionBoxGrp *m_pAniCollisionBoxGrp;
	ioAniCollisionBoxGrp *m_pSpecialBox;
	ioWoundedCollisionBoxGrp *m_pWoundedCollisionBoxGrp;

	CHARACTER m_CharInfo;

	typedef ioTPtrGrowArray<ioBuff> BuffArray;
	BuffArray *m_aBuffArray;

	struct BuffOnDoubleJumpBuff
	{
		ioHashString m_BuffName;
		bool m_bLive;
		BuffOnDoubleJumpBuff()
		{
			m_BuffName.Clear();
			m_bLive = false;
		}
		BuffOnDoubleJumpBuff( const ioHashString &szName, bool bLive )
		{
			m_BuffName = szName;
			m_bLive = bLive;
		}
	};
	typedef std::vector<BuffOnDoubleJumpBuff> vBuffOnDoubleJumpBuff;
	vBuffOnDoubleJumpBuff m_vBuffOnDoubleJumpBuff;
	bool m_bSetBuffOnDoubleJumpBuff;

	vReserveBuffInfoList m_ReserveBuffInfoList;

	D3DXQUATERNION m_qtTargetRot;
	D3DXVECTOR3	   m_vMoveDir;
	D3DXVECTOR3	   m_vInputDir;

	void		   UpdateMoveDir2BYTE();
	void		   UpdateInputDir2BYTE();
	void		   UpdateForceDir2BYTE();
	BYTE		   m_byAngleMoveDir;
	BYTE		   m_byAngleInputDir;
	BYTE		   m_byAngleForceDirX;
	BYTE		   m_byAngleForceDirY;
	BYTE		   m_byAngleForceDirZ;
	HALF		   m_halfRot[3];

	D3DXVECTOR3    m_vPreFlyMoveDir;
	D3DXVECTOR3	   m_vDefenseDir;

	D3DXVECTOR3	   m_vPrePosition;

	Stat	m_DefaultStat;
	Stat	m_CurrentStat;
	Stat	m_ExtraStat;

	CEncrypt<float> m_fCurMoveSpeed;
	CEncrypt<float>	m_fMaxMoveSpeed;
	CEncrypt<float>	m_fJumpMaxMoveSpeed;
	CEncrypt<float>	m_fJumpPreMoveSpeed;

	// For RecoveryGauge
	static CEncrypt<float> m_fDefaultRecoveryGauge;
	static CEncrypt<DWORD> m_dwDefaultRecoveryGaugeTic;

	static CEncrypt<float> m_fDelayRunGaugeRate;
	static CEncrypt<float> m_fEtcGaugeRate;

	//Disable SkillRecoveryGauge
	ioHashStringVec m_szDisableRecoverySkillGaugeBuffList;

	CEncrypt<float> m_fDefaultRecover;
	CEncrypt<DWORD> m_dwDefaultRecoveryTick;

	CEncrypt<float>	m_fMoveAccelSpeed;
	CEncrypt<float>	m_fRotateSpeed;

	CEncrypt<float>	m_fJumpPower;
	CEncrypt<float>	m_fJumpNeedGauge;
	CEncrypt<float>	m_fJumpExtraGauge;

	D3DCOLORVALUE m_CurDiffuseRate;
	D3DCOLORVALUE m_TargetDiffuseRate;

	D3DXVECTOR3	m_vForceDir;
	CEncrypt<float>	m_fForceAmt;
	CEncrypt<float>	m_fForceFrictionRate;
	//CEncrypt<float>	m_fGravityAmt;
	float	m_fGravityAmt;
	CEncrypt<bool>	m_bForcePowerLinear;
	static FloatVec	m_vBoundBlowGravityAmt;
	int		m_iBoundBlowGravityAmtIndex;

	ReservedSlidingList m_vReservedSlidingList;

	ioHashString m_AutoTargetName;
	D3DXVECTOR3  m_vAutoTargetBasisDir;
	DWORD	m_dwAutoTargetTrackingTime;
	bool	m_bAutoTargetBall;
	bool	m_bAutoTargetDummyChar;
	AutoTargetType m_AutoTargetType;

	ioUserKeyInput m_KeyInput;
	ioSkill *m_pActiveSkill;
	DWORD	m_dwCurActionKeyBySkill;

	AniDuration	m_AnimationTime;
	FireTimeList m_vAttackFireTimeList;
	FireTimeList m_vAttackFireTimeList_MustCall;

	CEncrypt<int>m_iAttackCombo;
	bool	m_bMaxCombo;
	bool	m_bAttackReserved;
	bool	m_bDefenseReservedAttack;
	bool	m_bJumpDashAttack;
	bool	m_bCrossBowJump;

	CEncrypt<JumpState> m_JumpState;
	float	m_fJumpLandingRate;
	float	m_fExtendLandingRate;
	float	m_fCurJumpLandingRate;
	DWORD	m_dwJumpStartTime;
	DWORD	m_dwJumpMoveStartTime;
	DWORD	m_dwJumpStateEndTime;
	DWORD   m_dwJumpStateEndStart;
	DWORD	m_dwJumpAttackEndTime;
	float	m_fCurJumpPowerAmt;
	float	m_fCurSkillEndJumpAmt;

	bool	m_bUsedJumpAttack;
	bool	m_bObjectItemJumpAttack;
	bool	m_bUsedBuffJumpAttack;
	bool	m_bAimState;
	bool	m_bUsedFlashJump;
	bool	m_bUsedChangeJump;
	bool    m_bUsedJumpReload;

	bool m_bUsedJumpDefense;
	bool m_bReleaseJumpDefense;
	
	bool m_bSuccessJumpAttack;

	DWORD m_dwExtendJumpAttackEndTime;
	float m_fExtendJumpAttackEndJumpAmt;

	BuffJumpUsedType m_UsedBuffJump;
	BuffJumpUsedType m_UsedBuffFlyJump;
	BuffJumpUsedType m_UsedBuffMultiFlyJump;

	// Bug -
	bool m_bBugCheck;
	DWORD m_dwBugCheckTime;

	ioHashString m_ExtendJumpEndAnimation;

	BlowTargetType m_BlowTargetType;
	AttackSide	   m_AttackedSide;

	ioHashString m_WeakWounded;
	ioHashString m_MidStiffWounded;
	ioHashString m_StrStiffWounded;

	ioHashString m_BackWeakWounded;
	ioHashString m_BackMidStiffWounded;
	ioHashString m_BackStrStiffWounded;

	float	m_fWoundedDefenseEnableRate;

	bool m_bFallBlowWounded;
	bool m_bBlowEndAni;
	bool m_bJumpedBlowWounded;
	bool m_bFastGetUp;
	bool m_bPreGetUpStick;
	bool m_bPreGetUpStick2;
	bool m_bShowPressButton;
	
	bool m_bEnableWoundSkill;

	DWORD	m_dwGeneralBlowWoundedGetUpTime;	// 일반 공격으로 쓰러진 경우 최대 지연시간
	DWORD	m_dwPrisonerBlowWoundedGetUpTime;	// 포로상태일때의 최대 지연시간
	DWORD	m_dwCatchedBlowWoundedGetUpTime;	// 포박상태일때의 최대 지연시간
	DWORD	m_dwMaxBlowWoundedGetUpTime;		// 공격형태에 대응하는 최대 지연 시간

	DWORD	m_dwFastGetUpStartTime;				// 기립조작 가능 시점 시간 체크(로그용)

	CEncrypt<BlowWoundedState> m_BlowState;
	DWORD	m_dwBlowGetUpCheckStartTime;
	DWORD	m_dwCurBlowGetUpTime;
	DWORD	m_dwBlowAniEndTime;
	DWORD	m_dwGetUpDurationTime;

	bool	m_bPreLeftInput;					// 앞선 스틱 키 입력이 왼쪽이었는지 체크
	DWORD   m_dwStickCheckTime;					// 좌,우 스틱 키 입력 체크 시간
	static DWORD   m_dwStickTicTime;			// 좌,우 스틱 키 입력 겝 시간

	CEncrypt<FallState> m_FallState;
	DWORD	m_dwFallLandingTime;

	DWORD	m_dwPreBackAttackedTime;
	int		m_iBackAttackedCnt;

	DefenseAction m_DefenseAction;
	DWORD   m_dwDefenseKeyPressTime;
	DWORD	m_dwDefenseEnableTime;
	DWORD	m_dwDefenseReleaseTime;
	DWORD	m_dwDefenseReleaseTimeEx;
	DWORD	m_dwDefenseEndCheckTime;
	DWORD	m_dwDefenseWoundedTime;
	bool	m_bNowDefenseEnable;
	bool	m_bDefenseKeyReleased;
	bool	m_bDefenseUseDelay;
	bool	m_bReloadMoveEnable;
	bool	m_bAttackMoveEnable;
	bool	m_bSkillMoveEnable;
	bool	m_bSkillMoveChangeDir;

	bool m_bDefenseMoveEnable;
	bool m_bDefenseMoveRotate;

// 키 예약 체크
protected:
	DWORD	m_dwReserveEnableTime;
	int		m_iReserveSkillInput;
	bool	m_bEnableDefenseState;
	bool	m_bEnableAttackState;
	bool	m_bEnableJumpState;

	bool m_bEnableExtendDefenseEffect;
	ioHashString m_CurDefenseEffect;
	ioHashString m_CurDelayCameraBuff;

	DWORD	m_dwDelayStateStartTime;	// Item PickUp State

	ioUserKeyInput::DirKeyInput m_CurDirKey;
	ioUserKeyInput::DirKeyInput m_DefenseDirKey;
	DWORD	m_dwNoInputDelayEndTime;

	DWORD m_dwDieBlowEndTime;
	DWORD m_dwSetDieStateTime;
	static DWORD m_dwLimiteDieFlyTime;
	static DWORD m_dwLimiteNpcDieFlyTime;
	
	CEncrypt<DieState> m_DieState;

	CEncrypt<DWORD>	m_dwProtectionModeTime;
	CEncrypt<DWORD>	m_dwProtectionStartTime;
	CEncrypt<ProtectionMode> m_ProtectionMode;

	ioFormalAttackGrp m_EmptyFormalAttack;
	DWORD   m_dwNormalAttackEndDelay;
	ioAttackableItem *m_pCurNormalAttackItem;
	bool	m_bNowDefaultNormalAttackProcess;

	ioEnhanceRate *m_pPushPowerEnhance;
	ioEnhanceRate *m_pWoundPushPowerEnhance;

	float	m_fBlowGetUpTimeRate;
	CEncrypt<float>	m_fSpeedRate;

	bool	m_bOwnerChar;

	bool	m_bCurFrameDrop;
	float	m_fCurGravityGapAmt;

	D3DXVECTOR3 m_vRackMoveDir;
	float m_fRackMoveAmt;
	D3DXVECTOR3 m_vRackMoveAmt;

	DWORD m_dwWeaponIndexBase;

	ioHashString m_LastAttackerName;
	ioHashString m_LastAttackerSkillName;
	TeamType     m_LastAttackerTeam;
	DWORD        m_dwLastAttackerItemCode;

	DWORD	m_dwLastAttackedCheckTime;
	DWORD	m_dwLastAttackedTime;
	bool	m_bForceDropping;
	bool	m_bFootDustEffectSetted;
	bool	m_bHideFootDustEffect;

	int		m_iFloatingWoundedCnt;

	D3DCOLORVALUE m_SkinColor;
	D3DCOLORVALUE m_SkinOutLineColor;

	ioHashString m_HairColorTex;
	D3DCOLORVALUE m_HairOutLineColor;

	ioHashString m_BeardColorTex;
	D3DCOLORVALUE m_BeardOutLineColor;

	OutLineRecordList m_OutLineChangeList;
	D3DCOLORVALUE m_OutLineColor;
	float m_fOutLineSize;
	bool  m_bOutLineModified;

	ioHashString m_szFaceChangeName;
	DWORD m_dwFaceChangeTime;

	// For HeadMeshChange
	ioHashString m_szDefaultHeadMeshName;
	ioHashString m_szChangeHeadMeshName;
	DWORD m_dwHeadMeshChangeTime;

	ioHashStringVec m_ModifyExceptList;
	ChangeMaterialInfoList m_ModifiedList;
	int m_iCurChangeColorIndex;

	ioTargetMarker *m_pTargetMarker;
	ioTargetMarker *m_pTargetMarkerOther;
	CEncrypt<bool>  m_bPrisonerMode;

	RoundEndMotion m_RoundEndMotion;
	DWORD m_dwRoundEndCharMotionItem;
	DWORD m_dwPreControlRequestTime;
	bool  m_bPreFloatingState;

	static float m_fSightRange;
	static float m_fSightAngle;
	static float m_fAngleSpeed;
	
	static CEncrypt<float> m_fDefaultHP;
	static CEncrypt<float> m_fDefaultSpeed;

	// For Safety
	static CEncrypt<float> m_fSafetyHPRate1;
	static CEncrypt<float> m_fSafetySkillRate1;
	static CEncrypt<float> m_fSafetyHPRate2;
	static CEncrypt<float> m_fSafetySkillRate2;
	static CEncrypt<float> m_fSafetyHPRate3;
	static CEncrypt<float> m_fSafetySkillRate3;
	static CEncrypt<float> m_fSafetyHPRate4;
	static CEncrypt<float> m_fSafetySkillRate4;
	static CEncrypt<float> m_fSafetyHPRate5;
	static CEncrypt<float> m_fSafetySkillRate5;

	static DWORD m_dwExtendDefenseCheckTime;
	static float m_fDefenseReserveAttackTimeRate;
	static float m_fDefenseReserveEnableRate;

	static CEncrypt<float> m_fDefenseMaxSpeedRate;
	static CEncrypt<float> m_fAttackMaxSpeedRate;
	static CEncrypt<float> m_fJumpMaxSpeedRate;
	
	static float m_fJumpAccelRate;
	static float m_fBackJumpSpeedRate;

	static DWORD m_dwFastGetupValue;
	static float m_fMinFastGetupTimeRate;
	static float m_fMinJumpAttackEnableHeight;
	static DWORD m_dwMinJumpAttackEnableTime;

	static float m_fKOActionBlowPower;
	static float m_fFloatingBlowDownRate;

	// For PickItem
	static DWORD m_dwItemPickUpStandardTime;			// 아이템을 줍기 이모티콘 위한 대기시간

	static float m_fFrontSideAngleCosine;
	static CEncrypt<float> m_fBackAttackDamageRate;
	static CEncrypt<float> m_fDefenseBrokenDamageRate;
	static CEncrypt<float> m_fSkillProtectDamageRate;
	static CEncrypt<float> m_fFrozenDamageRate;

	static float m_fWeakAttackWoundedRate;
	static float m_fMidStiffWoundedRate;
	static float m_fStrStiffWoundedRate;
	static float m_fStandKoWoundedRate;
	static float m_fStandKo2WoundedRate;

	static DWORD m_dwBackAttackCntGapTime;

	static float m_fLevelUpScaleIncreaseValue;
	static float m_fLevelUpScaleBalanceValue;

	static float m_fProtectionLineWidth;
	static D3DCOLORVALUE m_ProtectionColorRed;
	static D3DCOLORVALUE m_ProtectionColorBlue;
	static D3DCOLORVALUE m_ProtectionColorBoss;
	static D3DCOLORVALUE m_FixedOutlineColor;

	static DWORD m_dwProtectionCheckTime1;		// 정상으로 보이는 시간
	static DWORD m_dwProtectionCheckTime2;		// 컬러값 적용된 형태로 보이는 시간
	static D3DCOLORVALUE m_ProtectionRedSkinColor;		// 무적상태 캐릭터 색
	static D3DCOLORVALUE m_ProtectionBlueSkinColor;	// 무적상태 캐릭터 색

	int m_iProtectionColorIndex;				// 적용된 컬러값 인덱스
	bool m_bProtectionColor;					// 컬러값 적용된 상태
	DWORD m_dwProtectionChangeTime;				// 컬러값 적용 - 미적용 교체 시점

	static DWORD m_dwSkillCheckTime1;
	static DWORD m_dwSkillCheckTime2;
	static D3DCOLORVALUE m_SkillSkinColor;
	
	bool m_bSkillColor;
	bool m_bSkillProtection;
	int m_iSkillColorIndex;
	DWORD m_dwSkillChangeTime;

	static DWORD m_dwBlowProtectionTime;		// 쓰러졌다가 일어났을때 무적 시간
	static DWORD m_dwDropZoneGetUpTime;
	static DWORD m_dwNoInputDelayTime;			// 시작/부활/드롭시 딜레이시간 - 유저
	static DWORD m_dwNpcNoInputDelayTime;			// 시작/부활/드롭시 딜레이시간 - 몬스터
	static DWORD m_dwStartProtectionTime;		// 시작 무적시간
	static DWORD m_dwStartProtectionTime2;		// 팀서바 시작 무적시간
	static DWORD m_dwChangeCharProtectTime;			// 용병교체시 무적시간

	static CEncrypt<float> m_fBlowStateDamageRate;
	static CEncrypt<float> m_fFloatStateDamageRate;
	
	// Die Animation Rate
	static float m_fDieActionRate;
	static float m_fDieDownActionRate;
	static float m_fNpcDieActionRate;
	static float m_fNpcDieDownActionRate;


	static CEncrypt<float> m_fSetItemGaugeRecoverRate;
	static CEncrypt<float> m_fNoSetItemGaugeRecoverRate;

	// Fall Static Varible
	static float m_fFallGravityAmt;
	static float m_fFallDamageGravityAmt;
	static float m_fMinFallHeightRate;			// Min Fall Rate 

	static float m_fMinJumpHeightRate;			// Min Jump Rate
	static float m_fForcePowerAddRate;			// SetForcePower에서 기존힘 비율

	static CEncrypt<float> m_fProtectionDamageRate;
	static bool  m_bIgnoreAttackInProtection;

	static bool m_bWeakCounterAttackEnable;
	static bool m_bStrongCounterAttackEnable;
	
	static DWORD m_dwDashKeyCheckDuration;
	static DWORD m_dwExtendDashCheckDuration;
	static DWORD m_dwDashUpkeepDuration;
	
	static D3DXVECTOR3 m_vEnlargeScale;
	static bool m_bUseEnlargeScale;
	static bool m_bUseGetUpTimeRefresh;
	static bool m_bUseGetUpProtect;

	// Skill Combo 
	static DWORD m_dwSkillComboTime;
	static DWORD m_dwSkillComboFireAddTime;
	static int   m_iSkillComboViewCnt;
	static int   m_iSkillComboMaxCnt;

	// Kill Combo
	static int   m_iKillComboMaxCnt;

	// ChangeChar
	static DWORD m_dwChangeCharDisableTime;
	static DWORD m_dwChangeCharInputGap;

	DWORD m_dwChangeCharCheckTime;

	ioHashString m_szAttackerName;

public:
	static float m_fAutoAttackMinAngle;
	static float m_fAutoAttackMaxAngle;
	static float m_fAutoAttackRange;

	static DWORD m_dwActionStopDefaultDelay;
	static float m_fActionStopDelayRate;
	static float m_fActionStopLooseRate;

	static DWORD m_dwActionStopDieDefaultDelay;
	static float m_fActionStopDieLooseRate;
	
	static float m_fAxisCollisionBoxRate;
	static float m_fAttackCollisionBoxScale;
		
protected:
	ioExtendDash *m_pExtendDash;
	ioExtendJump *m_pExtendJump;
	ioCounterAttack *m_pCounterAttack;

	float m_fWeakAttackPushRate;

	bool m_bEnableDashCheck;

// Defense
protected:
	ioHashString m_AniDefense;
	ioHashString m_AniDefenseWounded;
	ioHashString m_AniDefenseCriticalWounded;
	ioHashString m_AniDefenseSkillWounded;

	DWORD m_dwDefensePreDuration;
	DWORD m_dwDefenseEndDuration;
	DWORD m_dwDefenseDuration;
	DWORD m_dwDefenseDurationEx;

	DWORD m_dwCurDefensePreDuration;
	DWORD m_dwCurDefenseEndDuration;
	DWORD m_dwCurDefenseDuration;
	DWORD m_dwCurDefenseDurationEx;

	float m_fDefenseWeakAttackAniRate;
	float m_fDefenseStrongAttackAniRate;
	float m_fDefenseSkillAttackAniRate;

protected:
	int m_iCurPushStructIndex;
	int m_iCurMachineStructArray;

protected:
	DWORD m_dwActionStopCurAdd;
	DWORD m_dwActionStopCurDelay;
	DWORD m_dwActionStopTotalDelay;

	DWORD m_dwDashCheckTime;
	DWORD m_dwComboCheckTime;

	bool m_bDirRelese;
	bool m_bBlowFrozenState;
	bool m_bBlowEtcState;
	bool m_bBlowStopMotion;

	CEncrypt<int> m_iStopMotionType;

//For Dash
	CEncrypt<float> m_fDashRunMaxSpeedRate;
	DWORD m_dwDashExtraTic;
	DWORD m_dwDashExtraCheckTime;
	ioHashString m_szExtendDashLoopEffect;
	DWORD m_dwDashStartTime;

//For Symbol
protected:
	CEncrypt<float> m_fSkillGaugeRate;
	CEncrypt<float> m_fExtraDamageRate;

//For Buff
protected:
	DWORD m_dwRandomSeed;

	AttackRateList m_WoundedRateList;
	AttackRateList m_AttackRateList;
	AttackRateList m_AttackRateNPCList;

	AttackSide m_WoundedRateSideType;

	float m_fMaxDamageRate;
	float m_fMaxWoundedRate;

	CEncrypt<float> m_fExtraDashSpeedRate;
	CEncrypt<float> m_fTotalExtraMoveSpeed;
	CEncrypt<float> m_fExtraMoveSpeed;
	CEncrypt<float> m_fMaxExtraSpeed;
	CEncrypt<float> m_fMinExtraSpeed;

	ReflectInfoList m_vReflectInfoList;

	WeakMagicInfoList m_vWeakMagicInfoList;
	WeakBombInfoList m_vWeakBombInfoList;

	float m_fMaxScaleRate;
	float m_fKingScaleRate;
	float m_fHeadScaleRate;
	CEncrypt<float> m_fExtraRecoveryGaugeRate;

	DWORD m_dwAttachWeaponIndex;
	ioHashString m_szAttachWeaponCreator;

	// For ChangeMotion
	bool m_bChangeMotion;
	ioHashString m_ChangeDelayAni;
	ioHashString m_ChangeMoveAni;

	ioHashString m_ChangeJumpReadyAni;
	ioHashString m_ChangeJumppingAni;
	ioHashString m_ChangeJumpEndAni;
	float m_fChangeJumpEndAniRate;

	CEncrypt<int> m_iChangeJumppingCnt;
	CEncrypt<int> m_iChangeJumppingLimit;

	// For Hide
	bool m_bEnableHide;
	ioHashString m_HideDelayAni;
	ioHashString m_HideMoveAni;
	bool m_bEnableDashOnHideBuff;

	// For NoWound
	bool m_bNoWoundState;

	// For Invisible
	bool m_bInvisibleState;
	bool m_bInvisibleNowState;

	// For ChangeTeam
	bool m_bChangeTeamBuff;

	//
	vCharBuffJumpInfoList m_vBuffJumpInfoList;
	int m_iBuffJumpCurCnt;
	DWORD m_dwBuffJumpTime;

	//
	vCharBuffJumpInfoList m_vBuffFlyJumpInfoList;
	int m_iBuffFlyJumpCurCnt;

	//
	vCharBuffJumpInfoList m_vBuffMultiFlyJumpInfoList;

	vCharFrictionBuffInfoList m_vFrictionBuffInfoList;
	vCharGravityBuffInfoList m_vGravityBuffInfoList;
	vFrontBackWoundInfoList m_vFrontBackWoundInfoList;

	vBuffValueInfoInfoList m_vDamageDefenseBuffInfoList;


	bool m_bReverseDirection;
	bool m_bChangeDirection;

	// For CloseOrder
	DWORD m_dwCloseOrderTime;
	ioHashString m_CloseOrderMove;

	DWORD m_dwCloseOrderMinTime;
	DWORD m_dwCloseOrderMaxTime;

	float m_fCloseOrderAniRate;
	float m_fCloseOrderBackAngle;

	bool m_bOnlyMoveDir;
	bool m_bCloseOrderAngleSetted;

	int m_iCloseOrderType;
	//

	CEncrypt<bool> m_bEnableManaShield;
	bool m_bSetDelayCameraBuff;
	
	int m_iEtcStateCnt;
	DWORD m_dwEtcCheckTime;

	float m_fJumpPowerRateByBuff;
	float m_fLandingRateByBuff;
	float m_fExtendLandingRateByBuff;

	// For DamageConvert
	DamageInfoList m_vAttackDamageInfoList;
	DamageInfoList m_vWoundDamageInfoList;
	
	// For ReturnAttackBuff
	CEncrypt<int> m_iReturnAttackAttribute;
	CEncrypt<int> m_iReturnAttackAttributeResist;

	// For BuffAirAttack
	AttackAttribute m_BuffAirAttackAttribute;

	// For LimitAction
	CEncrypt<bool> m_bBuffLimitAttack;
	CEncrypt<bool> m_bBuffLimitJump;
	CEncrypt<bool> m_bBuffLimitExtendJump;
	CEncrypt<bool> m_bBuffLimitDefense;
	CEncrypt<bool> m_bBuffLimitSkill;
	CEncrypt<bool> m_bBuffLimitDash;

	// For CurseBuff
	ioHashString m_CurseRollingAni;
	float m_fCurseRollingAniRate;

	ForceInfoList m_vCurseForceInfoList;

	// For Invisible Head
	bool m_bInvisibleHead;
	bool m_bInvisibleFace; 

	// For landstun
	vLandStunInfoList m_vLandStunInfoList;

	// For IceBuff
	ioHashString m_IceBuffName;
	ioHashString m_IceBuffCreator;
	WeaponInfo m_IceBoundWeapon;
	float m_fEnableIceBoundGravityAmt;

	bool m_bCheckIceBound;

	// For SkipStun
	ioHashString m_SkipStunName;

	// For Adhesive
	ioHashString m_AdhesiveName;

	// For Enginner toolbox
	ioHashString m_MineTraceStateBuff;

// Fly Variable
protected:
	ioHashString m_FlyBuffName;
	FlyMoveType m_FlyMoveType;
	
	CEncrypt<float> m_fFlyMoveSpeed;
	float m_fFlyFrictionAmt;
	float m_fFlyGravityAmt;
	DWORD m_dwFlyRotateTime;

	bool m_bFlyLeftRot;
	bool m_bFlyTargetRot;

	bool m_bSetBoost;
	bool m_bJumpBoost;			// 점프 상태에서 한번 더 점프 입력 시 스페이스 솔져 망토 스킬과 같이 부스트 상태 나타내는 플래그(바닥에 착지 시 false)
	bool m_bJumpBoostAttack;

	float m_fFlyOffHeight;
	DWORD m_dwFlyOffTime;
	DWORD m_dwFlyOffCheckTime;

// For Floating State
	FloatBuffInfo m_FloatBuffInfo;

	DWORD m_dwFloatStartTime;


// For RunDash Camera
	float m_fRunCamDistance;
	float m_fRunCamHeight;
	float m_fRunCamFov;
	DWORD m_dwRunCamZoomTime;

// For Buff Camera
	float m_fBuffCamDistance;
	float m_fBuffCamHeight;
	float m_fBuffCamFov;
	DWORD m_dwBuffCamZoomTime;

	bool m_bPriorityOverSkill;
	bool m_bSetNewCamBuff;
	int m_iCameraTypeByBuff;

	ioHashString m_DefaultDelayCameraBuff;

	BuffCamInfoList m_BuffCamInfoList;

// For JumpingSkill
	bool m_bJumpingSkillState;

// For SKill Next Jump
	static float m_fSkillEndNextLandRate;
	float m_fCurSkillEndNextLandRate;
	float m_fSkillNextJumpAmt;
	bool m_bSkillNextJump;

// For None Play
	CEncrypt<NonePlayState> m_NonePlayState;
	static ioHashStringVec m_NonePlayWarningBuff;
	static CEncrypt<float> m_fNonePlayGaugeRate;
	static DWORD m_dwNonePlayCareTime;
	static DWORD m_dwNonePlayWarningTime;
	DWORD m_dwNonePlayCheckTime;

// For ChatMode
	CEncrypt<ChatModeState> m_ChatModeState;

	static DWORD m_dwChatModeChatTime;
	static DWORD m_dwChatModeNormalTime;

	DWORD m_dwChatModeChangeEndTime;

// For ExperienceMode
	CEncrypt<DWORD> m_ExperienceMode;
	ioHashString    m_ExperienceID;
	int             m_ExperienceClassType;
	DWORD m_ExperienceChangeEndTime;
	static DWORD m_ExperienceChangeTime;
	bool         m_bNoChangeCharOnExperienceChange;

// For Fishing
	static ioHashString m_FishingReadyAni;
	static ioHashString m_FishingLoopAni;
	static ioHashString m_FishingHookAni;
	static ioHashString m_FishingResultSuccessAni;
	static ioHashString m_FishingResultFailAni;

	static float m_fFishingReadyAniRate;
	static float m_fFishingHookAniRate;

	static DWORD m_dwFishingResultSuccessDuration;
	static DWORD m_dwFishingResultFailDuration;

	static DWORD m_dwFishingCamera;

	static float m_fFishingRange;

	static ioHashString m_FishingEmoticon;
	static ioHashString m_FishingHookEmoticon;
	static ioHashString m_FishingFailEmoticon;
	static ioHashString m_FishingLevelUpText;

	CEncrypt<FishingState> m_FishingState;

	CEncrypt<DWORD> m_dwFishingLoopCurTime;
	DWORD m_dwFishingStateChangeTime;

	int m_iCurFishingRodType;
	int m_iCurFishingBaitType;

	ioHashString m_FishingCurRod;
	ioHashString m_FishingCurReadyEffect;
	ioHashString m_FishingCurWaitEffect;
	ioHashString m_FishingCurPullEffect;

	FishingResultInfo m_FishingResultInfo;
	FishingPresentInfo m_EventFishingPresentInfo;	// 낚시광 이벤트
	FishingPresentInfo m_FishingPresentInfo;		// 특별템 낚기

	// 내가 사용하고 있는 낚시터 주인
	ioHashString m_UseFishGroundOwner;
	int m_UseFishGroundIdx;

	// 낚시 성공 플래그
	bool m_bPrivateFishingSuccess;

// For Roulette
	RouletteState       m_RouletteState;
	static ioHashString m_RouletteAni;
	static ioHashString m_RouletteEndAni;
	static float        m_fRouletteAniRate;
	static DWORD        m_dwRouletteStartStandardTime;
	static float        m_fRouletteAniJumpPower;
	static float        m_fRouletteGravityRate;
	float               m_fRouletteAniMoveSpeed;
	float               m_fCurRouletteAniMoveSpeed;
	DWORD               m_dwRouletteAniEndTime;
	bool                m_bSetRouletteEmoticon;
	
// For WitchFly
	ioHashString m_WitchFlyBuffName;
// For MechanicsFly
	ioHashString m_MechanicsBuffName;
// For Destroyer
	ioHashString m_DestroyerBuffName;
	
// For Etc.
	int m_iVoiceType;
	bool m_bChargingState;
	bool m_bWeaponDie;

	static DWORD m_dwCheckSpecialMotionMinTime;
	static DWORD m_dwCheckSpecialMotionMaxTime;

	DWORD m_dwCheckSpecialMotionCurTime;

	ioHashString m_SpecialMotion;
	DWORD m_dwSpecialMotionCamera;

// For TeleportState
	ioHashString m_szTeleportStart;
	ioHashString m_szTeleportEnd;
	
	float m_fTeleportStartRate;
	float m_fTeleportEndRate;

	DWORD m_dwTeleportStartEndTime;
	DWORD m_dwTeleportEndEndTime;

	D3DXVECTOR3 m_vTeleportEndPos;

// For Growth
	CEncrypt<float> m_fBuffJumpSpeedGrowthRate;
	CEncrypt<float> m_fGrowthMoveSpeedRate;

// For Reinforce
	CEncrypt<int> m_DefaultReinforce[4];

// For Prisoner
protected:
	static float m_fPrisonerJumpRate;
	static CEncrypt<float> m_fPrisonerSpeed;
	static CEncrypt<float> m_fPrisonerHPRate;
	static CEncrypt<float> m_fPrisonerHPRateDraw;
	static CEncrypt<float> m_fPrisonerHPRateLose;
	static float m_fPrisonerEscapeJumpPower;
	static bool  m_bPrisonerGaugeStopEnable;

	static ioHashString m_PrisonerEscapeAni;
	static float m_fPrisonerEscapeAniRate;
	static ioHashString m_PrisonerEffect;
	static ioHashString m_PrisonerEscapeEffect;
	static ioHashString m_PrisonerRope;
	static ioHashString m_PrisonerSound;

	static float m_fPrisonerPushPower;
	static float m_fPrisonerBlowPower;

	static ioHashStringVec m_EscapeBuffList;

// For Emoticon
protected:
	bool	m_bSetPickEmoticon;
	bool	m_bSetNetworkTimeOutEmoticon;
	int		m_iNetworkTimeOutReserveChatMode;

	static int  m_cNetworkBad;
	static int  m_iMaxNetworkBad;
	static int  m_iNetworkBadMS;

// For Armor & Speed Class
protected:
	CEncrypt<float> m_fCurArmorClass;
	CEncrypt<float> m_fAddArmorClass;
	CEncrypt<float> m_fMaxAddArmorClass;
	CEncrypt<float> m_fCurSpeedClass;
	CEncrypt<float> m_fAddSpeedClass;
	CEncrypt<float> m_fMaxAddSpeedClass;

// For Frozen, Flame Buff
protected:
	bool m_bFrozenState;
	DWORD m_dwFrozenStartTime;
	DWORD m_dwFrozenMinTime;
	DWORD m_dwFrozenCurTime;
	DWORD m_dwFlameMinTime;

protected:
	CEncrypt<float> m_fCurHPRate;

	static ioHashString m_ChangeWaitAni;
	static ioHashString m_ChangeWaitEffect;

// For Catch
protected:
	static ioHashString m_CatchEscapeAni;
	static float m_fCatchEscapeAniRate;

	ioHashString m_CatchStateEffect;
	ioHashString m_CatchEscapeEffect;
	ioHashString m_CatchRope;
	ioHashString m_CatchRopeCreateChar;

	static float m_fCatchJumpRate;
	static CEncrypt<float> m_fCatchMoveSpeed;
	static float m_fCatchEscapeJumpPower;
	
	bool m_bCatchState;
	bool m_bEscapeFail;
	DWORD m_dwEscapeCatchTime;

	bool m_bReturnJumpping;

protected:
	// For DashFail
	ioHashString m_DashFailEmoticon;
	ioHashString m_DashFailAni;
	float m_fDashFailAniRate;

	// For Exception Dash Fail
	static DWORD m_dwDashFailDuration;
	DWORD m_dwDashFailStartTime;
	bool m_bExceptionDashFail;

protected:  // Skill Combo 
	int m_iSkillComboCnt;
	DWORD m_dwCurSkillCombo;

protected:	// Real Combo
	struct RealCombo
	{
		ioHashString m_szCharName;
		int			 m_iRealComboCnt;

		RealCombo()
		{
			m_iRealComboCnt = 0;
		}
	};
	typedef std::vector< RealCombo > RealComboVec;
	RealComboVec m_RealComboList;
	DWORD m_dwRealComboDefenseCheckTime;

protected:
	static ioHashString m_ChampStartEffect;
	static ioHashString m_ChampEffect;
	static ioHashString m_ChampEndEffect;

	static ioHashString m_ChampGetSound;
	static ioHashString m_ChampLoseSound;
	static DWORD m_dwChampChangeTime;

	UniqueObjID m_ChampObjID[3];
	UniqueObjID m_CurProtectionEffect;

	DWORD m_dwChampStartTime;
	DWORD m_dwChampStartChangeTime;

protected:
	bool   m_bBossCharacter;
	float  m_fBossCharacterScale;
	ioHashString m_BossPrevSkeleton;

protected:
	bool   m_bGangsiCharacter;

protected:
	CEncrypt<bool> m_bOnlyTeamCharColSkip;

	CEncrypt<DWORD> m_dwCharColSkipType;		// 1자리는 팀, 모두 여부, 10자리는 방어돌파 여부

	CEncrypt<DWORD> m_dwCharCollisionSkipStart;
	CEncrypt<DWORD> m_dwCharCollisionSkipEnd;

	CEncrypt<DWORD> m_dwCharColSkipSkillStart;
	CEncrypt<DWORD> m_dwCharColSkipSkillEnd;

	CEncrypt<DWORD> m_dwEnableAttackDefenseStart;
	CEncrypt<DWORD> m_dwEnableAttackDefenseEnd;

// For MiniMapSpot
protected:
	DWORD m_dwSetStartTime;
	DWORD m_dwSetPrisonerTime;
	DWORD m_dwEscapePrisonerTime;
	DWORD m_dwCrownPickTime;

// For All Item Drop
protected:
	IORandom m_DropRangeRand;

// For WeaponIgnore
protected:
	WeaponIgnoreInfo m_WeaponIgnoreInfo;

// For DashAttack
protected:
	bool m_bEnableExtraDashAttack;
	bool m_bSetExtraDashAttack;
	CEncrypt<int> m_iCurExtraDashAttack;

	bool m_bEnableExtraOtherDashAttack;
	bool m_bSetExtraOtherDashAttack;
	CEncrypt<int> m_iCurExtraOtherDashAttack;

	bool m_bSetDashAttackEndDash;
	DashAttEndType m_DashAttackEndDash;

	DWORD m_dwDashAttackEndTime;
	float m_fDashAttackEndJumpAmt;

	DWORD m_dwExtendAttackEndTime;
	float m_fExtendAttackEndJumpAmt;

	int m_iCurExtraAniJump;
	DWORD m_dwExtraAniJumpTime;
	float m_fExtraAniJumpPower;
	vExtraAniJumpList m_vExtraAniJumpList;
	vExtraAniJumpList m_vObjectEquipAniJumpList;

// For Checker
protected:
	D3DXVECTOR3 m_vSetJumpPos;

public:
	#ifndef SHIPPING
	static D3DXVECTOR3 m_vRangeBoxArrayForTest[8];
	#endif

// Excavation
protected:
	CEncrypt<bool> m_bExcavating;

// BTT_BOUND_BLOW
protected:
	int m_iMaintainItemCreateIndex;
	//

// BTT_BOUND_BLOW
protected:
	CEncrypt<float> m_fBoundBlowJumpAmt;
	CEncrypt<float> m_fBoundBlowForceAmt;
	CEncrypt<float> m_fBoundBlowGravityRate;
	CEncrypt<float> m_fBoundBlowBoundDamage;
	CEncrypt<int> m_iBoundBlowAttackType;
	CEncrypt<float> m_fPreBoundBlowJumpAmt;
	CEncrypt<float> m_fPreBoundBlowForceAmt;

// BTT_BLOW_EXTEND_WOUND
protected:
	ioHashString m_ExtendWoundAni;
	CEncrypt<DWORD> m_dwExtendWoundDuration;
	CEncrypt<float> m_fExtendWoundJumpAmt;
	CEncrypt<float> m_fExtendWoundForceAmt;
	
	CEncrypt<float> m_fExtendWoundDefenseRate;
	CEncrypt<bool> m_bExtendWoundLoop;

// Cur Blow
protected:
	ioHashString m_CurBlowWoundAction;
	ioHashString m_CurBlowDownAction;

protected:
	CEncrypt<int> m_iMedalItemCharGrowth[MAX_CHAR_GROWTH];
	CEncrypt<int> m_iMedalItemItemGrowth[MAX_ITEM_GROWTH];
	MedalSyncInfoVec m_vMedalItemTypeVec; // UI 설정용으로 보안설정 없음

protected:
	IntVec m_vExpandMedalSlotNumber;	// 툴팁에서 보여주기 위함.

protected:
	bool m_bSpecialHide;

protected:
	DWORD m_dwDownRollingEndTime;
	DWORD m_dwDownRollingGetUpEndTime;

	ioHashString m_DownGetUpMotion;
	float m_fDownGetUpMotionRate;
	
protected:
	ioHashString m_szDefaultSkeleton;
	ioHashString m_szChangeSkeleton;
	int m_iEtcItemSkeleton;
	int m_iEtcItemMotion;
	DWORD m_dwEtcItemMotionTime;

protected:
	DWORD m_dwNewJumpAttackEndTime;
	float m_fNewJumpAttackGravity;

	float m_fAirAttackGravity;
	bool m_bAirAttackLand;

protected:
	DWORD m_dwCreateStructStartTime;
	DWORD m_dwCreateStructEndTime;

	DWORD m_dwCreateStructDelayTime;

	int m_iCreateStructIndex;
	DWORD m_dwCreateStructCode;

protected:
	DWORD   m_dwSendChangeCharTime;

// Grappling
protected:
	GrapplingType m_GrapplingType;

	GrapplingState m_WoundGrapplingState;
	GrapplingPushedState m_GrapplingPushedState;
	GrapplingSkillState m_GrapplingSkillState;

	ioHashString m_GrapplingTarget;
	ioHashString m_GrapplingAttacker;
	ioHashString m_GrapplingSkillName;

	// target
	DWORD m_dwTargetGrapplingMoveStartTime;

	GrapplingWoundedInfo m_TargetGrapplingInfo;

	bool m_bSetTargetGrapplingMove;

	// pushed
	GrapplingPushedInfo m_PushedGrapplingInfo;

	DWORD m_dwGrapplingPushedStartTime;
	DWORD m_dwGrapplingPushedReturnTime;
	DWORD m_dwGrapplingPushedGapTime;

	// skill
	GrapplingSkillWoundedInfo m_SkillGrapplingInfo;

	ioUserKeyInput::DirKeyInput m_GrapplingKey;

	// Psyc Grappling
	ioHashString m_PsycGrapplingAttacker;
	PsycGrapplingWoundedInfo m_PsycGrapplingInfo;

	// Jump Grappling
	JumpGrapplingState m_JumpGrapplingState;
	JumpGrapplingInfo m_JumpGrapplingInfo;
	JumpGrappingWoundInfo m_JumpGrapplingWoundInfo;

	D3DXVECTOR3 m_vJumpGrapplingAttackPos;
	D3DXVECTOR3 m_vJumpGrapplingDir;

// input cancel
protected:
	ioHashString m_szInputCancelAni;
	float m_fInputCancelAniRate;

	DWORD m_dwCancelCheckTime;
	DWORD m_dwInputCancelCheckTime;

// BlowDash
protected:
	BlowDashInfo m_BlowDashInfo;
	BlowDashState m_BlowDashState;

	DWORD m_dwBlowDashCheckTime;
	
// sylphid
protected:
	CEncrypt<float> m_fSylphidJumpSpeedRate;
	CEncrypt<float> m_fSylphidJumpGravityRate;

// Iljimae
	CEncrypt<DWORD>	m_dwReturnMarkEffectID;

// summon dummychar
	CEncrypt<DWORD> m_dwSummonDummyCharTime;

// last applyweapon
	CEncrypt<DWORD> m_dwLastApplyWeaponIdx;
	CEncrypt<DWORD> m_dwLastApplyWeaponTime;
		
// ani rotate
protected:
	vAniRotateInfoList m_vAniRotateInfoList;

	int m_iCurAniRotate;
	DWORD m_dwAniRotateTime;
	float m_fAniRotateAngle;

protected:
	DWORD m_dwCharChangeIndex;

// Ghost State
protected:
	float m_fPreHeightForGhostState;
	bool m_bUseGhostState;

// panic move
protected:
	PanicMoveInfo m_PanicMoveInfo;
	CEncrypt<DWORD> m_dwNextPanicMoveTime;

protected: // 용병 교체 패킷 보정
	struct ChangeCharPacketReserve
	{
		DWORD m_dwCharIndex;
		DWORD m_dwStartTime;
		SP2Packet m_Packet;

		ChangeCharPacketReserve()
		{
			m_dwCharIndex = m_dwStartTime = 0;
		}
	};
	typedef std::vector< ChangeCharPacketReserve > ChangeCharPacketReserveVec;
	ChangeCharPacketReserveVec m_ChangeCharPacketReserveList;

protected: // 런닝맨
	DWORD m_dwRunningManDecoIndex;

protected:
	CEncrypt<DWORD> m_dwCurInvisibleID;
	DWORDVec m_vWeaponInvisibleID;
	DWORDVec m_vHelmetInvisibleID;
	DWORDVec m_vCloakInvisibleID;

protected:
	CEncrypt<DWORD> m_dwCheckValueTime;

	ioHashString m_PossessionName;

protected:
	bool m_bEnableKeyInputMust;
	bool m_bDisableConveyer;

//각성 이펙트
protected:
	ioHashString m_pCurrAwakeEffectName;

protected:
	bool m_bChatReserve;

protected:
	struct ReservedEquipObjectItem
	{
		DWORD			dwCode;
		int				eCreateType;		
		ioHashString	szCratorName;

		ReservedEquipObjectItem()
		{
			dwCode		= 0;
			eCreateType	= 0;
		}
	};
	typedef std::vector<ReservedEquipObjectItem> ReservedEquipObjectItemList;
	ReservedEquipObjectItemList m_ReservedEquipObjectItemList;

protected:
	bool m_bDisableEquipObject;

protected:
	typedef std::map< int, ioHashString > ComboSoundMap;
	static ComboSoundMap m_ComboSoundMap;
	ioHashString m_szCurSkillComboSound;

protected:
	int m_iCurRSoldierCount;

protected:
	int m_nSoldierSetCnt;
	ioHashString m_szExtendDieMotion;
	ioHashString m_szExtendDieBackMotion;
	ioHashString m_szExtendBlowDieAction;
	ioHashString m_szExtendBackBlowDieAction;
	ioHashString m_szExtendDownBlowDieAction;
	ioHashString m_szExtendBackdownBlowDieAction;
	ioHashString m_szExtendKillEffect;

private:
	//방어력 관통
	float m_fArmorPenetration;
	//추가 방어력
	float m_fExtraArmorClass;
	//치명타율
	static CEncrypt<float>	m_fMinCriticalRate;
	static CEncrypt<float>	m_fMaxCriticalRate;
	CEncrypt<float>			m_fTotalCriticalRate;
	//치명타 데미지
	static CEncrypt<float>	m_fMinCriticalDamageRate;
	static CEncrypt<float>	m_fMaxCriticalDamageRate;
	CEncrypt<float>			m_fTotalCriticalDamageRate;
	//점프 착지 딜레이 감소
	float m_fExtraLandingRateByBuff;
	float m_fExtraExtendLandingRateByBuff;

	float	m_fNPCWoundedAttckerRate;
	float	m_fWoundedRate;

private:
	DWORD m_dwExcavationGuideID;
	DWORD m_dwExcavationPointID;
	D3DXVECTOR3 m_ExcavationPos;


	// 낚시터 오브젝트 설치 플래그
	bool m_bCreateFishingGround;


protected:
	bool m_bCharacterImmediatelyChange;
	float	m_fCharacterChangeImmediatelyTime;
	static CEncrypt<float> m_fImmediatelyChangeJumpPower;

	static CEncrypt<float> m_fImmediatelyChangeDelay;
	static CEncrypt<float> m_fImmediatelyChangeRun;
	static CEncrypt<float> m_fImmediatelyChangeDash;
	static CEncrypt<float> m_fImmediatelyChangeAttack;
	static CEncrypt<float> m_fImmediatelyChangeWound;

public:
	static void InitStaticINIValue( ioINILoader &rkLoader, ioINILoader &rkBuff );

private:
	static void _InitStaticINIValueCommon1( ioINILoader &rkLoader );
	static void _InitStaticINIValueCommon2( ioINILoader &rkLoader );
	static void _InitStaticINIValueInfo( ioINILoader &rkLoader );
	static void _InitStaticINIValueAni( ioINILoader &rkLoader );
	static void _InitStaticINIValueEtc( ioINILoader &rkLoader );

public:
	void InitAttackReserve();
	void InitINIValue( ioINILoader &rkLoader );

	void InitRaceInfo( bool bSaveCharInfo = false );	// 싱글플레이
	void InitRaceInfo( const CHARACTER &rkCharInfo );
	void InitRaceInfo( ioINILoader &rkLoader );
	void InitRaceInfo( ioNpcAppearance* npcappr );

	void SetReloadDecoration( const CHARACTER &rkCharInfo );
	void ChangeDefaultMeshNameList( const ioHashStringVec &rkNameList );
	void ChangeSkinNumber( int iSkinNumber );
	void ChangeSkeleton( const ioHashString &rkSkeleton );
	void ChangeEtcItemSkeleton( int iEtcItem );

	void RecalcStatusValue();

	void SetFootDustEffect();
	void SetHideFootDustEffect( bool bIgnore ) { m_bHideFootDustEffect = bIgnore; }
	void CheckFootDustEffect();
	void SetCharChangeEffect();	
	void SetEndFocusEffect( const ioHashString &szEndFocusEffect );

	void SetIsOwnerChar( bool bOwner );
	inline bool IsOwnerChar() const { return m_bOwnerChar; }

	void EnableKeyInputCheck( bool bEnable );
	void EnableKeyInputMust( bool bEnable );

public:
	virtual void Update( float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime );
	virtual void UpdateAfter( float fFrameGap );

	virtual void SetTeam( TeamType eType );
	virtual void Revival();
	virtual void FillMaxHP();

	TeamType GetSingleConvertTeam();                 // 개인전인데 팀 표시는 블루 레드로 표시하도록 변경

public:
	virtual void AddEntityEvent( ioEntity *pEntity );
	void CustomTextureEffectEvent( ioEffect *pEffect );

public:
	void CustomTextureDownLoadComplete( const ioHashString &rkDstTex, const DWORD dwCustomIndex, const DWORD dwSubIndex );

public:
	virtual bool IsNeedCheckMapCollision() const;
	virtual void DontMoveCollisionPushingTranslate( ioGameEntity *pPusher, const D3DXVECTOR3 &vMoveAmt );
	virtual int  DontMoveEntityLevel() const;
	
	virtual void UpdateGauge();
	virtual void RenderGauge();
	bool IsVisibleHP();
	bool IsBoss(){ return m_bBossCharacter; }
	bool IsGangsi() const { return m_bGangsiCharacter; }
	void SetGangsi( bool bGangsi ){ m_bGangsiCharacter = bGangsi; }

public:
	virtual ioWorldEventReceiver* GetWorldEventReceiver();

	virtual void OnTelepotation( const D3DXVECTOR3 &vTargetPos );
	virtual void OnFireZoneDamage( float fDamage, const ioHashString &szBuff );
	virtual void OnPoisonZoneDamage();

protected:
	void CheckSpecialCollisionBox( const ioHashString &szFileName );

public:
	virtual ApplyWeaponExp ApplyWeapon( ioWeapon *pWeapon );
	virtual bool IsCollisionAvailableWeapon( ioWeapon *pWeapon );
	virtual bool IsEntityCollisionSkipState( ioGameEntity* pColEntity ) const;
	virtual bool IsCharCollisionSkipState( TeamType eTeamType, int iState=0 ) const;
	virtual bool IsWoundedCollision( const ioOrientBox &rkAttackBox, D3DXVECTOR3 *pColPoint = NULL ) const;
	virtual ioOrientBox GetAttackCollisionBox( AniCollisionType eType = ACT_NONE, bool bNoScale=false, float fScale = FLOAT1 );
	virtual ioOrientBox GetAttackCollisionBoxbyWeapon( ioAniCollisionBoxGrp *pSpecialBox, AniCollisionType eType = ACT_NONE, bool bNoScale=false, float fScale = FLOAT1 );

	virtual bool CheckSphereDistance( const D3DXVECTOR3 &vStart, float fRange, float &fDistance ) const;
	virtual bool CheckSpherePoint( const D3DXVECTOR3 &vStart, float fRange, D3DXVECTOR3 &vPoint ) const;
	virtual bool CheckCylinderPoint( const ioCylinder &rkCylinder,
									 const D3DXVECTOR3 &vStart,
									 D3DXVECTOR3 &vPoint ) const;

	virtual PlaySubType GetSubType() const;
	virtual ArmorType GetArmorType() const;

	bool IsCharCollisionSkipUse() const;

public:
	bool CheckCollisionLine( const D3DXVECTOR3 &vColPoint, bool bCheckStruct=true );
	bool CheckCollisionLine( const D3DXVECTOR3 &vStartPos, const D3DXVECTOR3 &vColPoint, bool bCheckStruct=true );
	bool IsEnableAimTarget( bool bDownAim );
	bool IsEnableTeamCollision( ioWeapon *pWeapon );

	bool CheckCollisionFishingGroundAndPoint( const D3DXVECTOR3 &vColPoint );

	void CheckCharColSkipTime( int iAniID, float fTimeRate, DWORD dwPreDelay=0 );
	void CheckCharColSkipSkillTime( int iAniID, float fTimeRate, DWORD dwPreDelay=0 );
	void SetCharCollisionSkipTime( DWORD dwStartTime, DWORD dwEndTime, bool bOnlyTeam=false );
	void SetCharColSkipSkillTime( DWORD dwStartTime, DWORD dwEndTime, bool bOnlyTeam=false );
	void ClearCharColSkipTime();
	void ClearCharColSkipSkillTime();

	void SetIgnoreWeaponInfo( ioWeapon *pWeapon );

public:
	void ApplyWeaponDefense( ioWeapon *pWeapon,
							 AttackSide eSide,
							 const D3DXVECTOR3 &vAttackDir );
	float ApplyWeaponDefenseInNetwork( SP2Packet &rkPacket, 
										const ioHashString &szAttacker,
										const ioHashString &szCreateItem,
										DWORD dwWeaponIndex, 
										const WeaponAttribute *pAttr );

	float ApplyWeaponWoundedInNetwork( SP2Packet &rkPacket,
									   const ioHashString &szAttacker,
									   const ioHashString &szSkill,
									   DWORD dwWeaponIndex, 
									   int iAttributeIdx,
									   DefenseBreakType& iDefenseBreakType,
									   bool& bBuffCreate
									   );
	
	void RotateByWeaponCollision( ioWeapon *pWeapon );
	void RotateByBuffCollision( const ioHashString &szTarget, int iType );

	void OnWeaponDie( const ioHashString &szWeaponOwner, const ioHashString &szWeaponOwnerSkill );

	inline bool GetBlowStopMotionState() const { return m_bBlowStopMotion; }

	inline void SetVoiceType( int iType ) { m_iVoiceType = iType; }
	inline void SetCurFrozenState( bool bFrozen ) { m_bFrozenState = bFrozen; }
	inline bool GetFrozenState() const { return m_bFrozenState; }
	inline bool GetBlowFrozenState() const { return m_bBlowFrozenState; }
	
	inline void SetBlowEtcState( bool bBlow ) { m_bBlowEtcState = bBlow; }
	inline bool GetBlowEtcState() const { return m_bBlowEtcState; }

	bool IsBlowWoundedDownState( DWORD dwCheckTime );
	bool IsCanLandRollingState( float fRate, float fExtendRate );
	
	const ioHashString& GetWoundExtraEffect() const;
	int GetKillShakeCamera();

protected:
	void UpdateGroupAndGenerateWeapon( float fTimePerSec,
									   DWORD dwPreTime,
									   DWORD dwCurTime );

protected:
	float GetDefenseStateApplyDamage( ioWeapon *pWeapon );
	float GetProtectStateApplyDamage( ioWeapon *pWeapon, float fDamage );
	float GetWoundedStateApplyDamage( ioWeapon *pWeapon, AttackSide eSide );
	float GetBoundWoundedStateApplyDamage( ioWeapon *pWeapon, AttackSide eSide );
	void  UpdateBackAttackCount( AttackSide eSide );
	void  SetWoundedStateApplyBuff( int iAttackType, TeamType eAttackTeam );

public:
	float GetScaleByPlayLevel( int iPlayLevel );

public:
	void CheckExplosionOnHand( bool bNetwork );
	void CheckExplosionItemWoundedDrop( const D3DXVECTOR3 &vDropDir, bool bNetwork );
	void CheckDropThrowBomb( const D3DXVECTOR3 &vDropDir, bool bNextwork );
	void CheckDropSmartBomb( const D3DXVECTOR3 &vDropDir );
	void CheckDropBladeChargeObject( const D3DXVECTOR3 &vDropDir );
	void CheckPowerArmorWounded();
	void CheckReleaseRidingItem();

	void NormalAttackFireExplicit( const WeaponInfo &rkInfo, const D3DXVECTOR3 &vDir = ioMath::VEC3_ZERO );
	void NormalAttackFireExplicit( const WeaponInfo &rkInfo, const D3DXVECTOR3 &vDir, const D3DXVECTOR3 &vPos );
	void WoundedFireAppointExplicit( const WeaponInfo &rkInfo, const D3DXVECTOR3 &vDir, const D3DXVECTOR3 &vPos );

	ioWeapon* SkillFireExplicit( const WeaponInfo &rkInfo,
								 const ioHashString &szSkillName,
								 DWORD dwWeaponIndex,
								 const D3DXVECTOR3 &vFirePos,
								 const D3DXVECTOR3 &vDir = ioMath::VEC3_ZERO );

	ioWeapon* ExtendFireExplicit( const FireTime &rkFireTime,
							 const D3DXVECTOR3 &vFirePos,
							 const D3DXVECTOR3 &vFireDir,
							 const ioHashString &szCreatItem );

	void ReturnAttackExplicit( const WeaponInfo &rkInfo,
							   DWORD dwWeaponIndex,
							   const D3DXVECTOR3 &vFirePos,
							   const ioHashString &szTarget );

	ioWeapon* AreaFireExplicit( const WeaponInfo &rkInfo,
								DWORD dwWeaponIndex,
								const D3DXVECTOR3 &vFirePos,
								const D3DXVECTOR3 &vFireDir,
								DWORD dwDuration=0 );

	void BuffFireExplicit( const WeaponInfo &rkInfo, const ioHashString &szBuffName );
	void BuffFireExplicit2( const WeaponInfo &rkInfo,
							const ioHashString &szBuffName,
							DWORD dwWeaponIdx,
							ioHashString &szTargetName );

	ioWeapon* DummyExplicit( const WeaponInfo &rkInfo,
							 const D3DXVECTOR3 &vFirePos,
							 DWORD dwWeaponIndex,
							 bool bNet,
							 const ioHashString &szCharName,
							 const ioHashString &szBuffName,
							 bool bUseWeaponCol,
							 bool bChangeWeaponDir = false );

	ioWeapon* ReflectFireExplicit( const WeaponInfo &rkInfo,
								   const D3DXVECTOR3 &vFirePos,
								   DWORD dwWeaponIndex,
								   const ioHashString &szCharName,
								   const ioHashString &szBuffName,
				  				   D3DXVECTOR3 vFireDir );

	void WeaponByWeaponExplicitList( const WeaponInfoList &vInfoList,
									 FireTimeType eFireTimeType,
									 const D3DXVECTOR3 &vFirePos,
									 const D3DXVECTOR3 &vFireDir,
									 bool bSend = true );

	ioWeapon* WeaponByWeaponExplicitUnit( const WeaponInfo &vInfo,
										  FireTimeType eFireTimeType,
										  const D3DXVECTOR3 &vFirePos,
										  const D3DXVECTOR3 &vFireDir,
										  exReserveWeaponCollisionInfo exCollisionInfo,
										  bool bSend, bool bReserveCall = false );

	void ApplyWeaponByWeaponExplicit( SP2Packet &rkPacket );
	void ApplyWeaponByWeaponExplicitList( SP2Packet &rkPacket );
	void ApplyReflectFireExplicit( SP2Packet &rkPacket );

	void CheckReturnAttackBuff( ioWeapon *pWeapon, const D3DXVECTOR3 &vPos );
	
	void CheckChainWeapon( ioWeapon *pWeapon );
	void CheckBallWeapon( ioWeapon *pWeapon );
	bool ApplyChainWeaponChange( int iWeaponIndex, int iNext, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vDir );
	bool ApplyBallWeaponChange( int iWeaponIndex, int iNext,
								const D3DXVECTOR3 &vPos,
								const D3DXVECTOR3 &vMoveDir,
								const D3DXVECTOR3 &vAttackDir );

	void InitForcePower();
	void SetCurSpeedToForcePower( float fRate = FLOAT1 );
	void SetForcePower( const D3DXVECTOR3 &vForceDir,
						float fForceAmt,
						float fFrictionRate = FLOAT1,
						bool bForceLinear = false );
	
	void WeakAttackPush( const D3DXVECTOR3 &vAttackDir, float fPushPower );
	void ReverseReflectPush( const D3DXVECTOR3 &vAttackDir, float fPushPower );

	void SetJumpPower( float fPowerAmt );
	void SetAirJumpPower( float fPowerAmt );
	void SetTargetDiffuseRate( const D3DCOLORVALUE &kDiffuse );

	void SetReservedSliding( const ForceInfoList &vForceInfoList,
							 int iAniID,
							 float fTimeRate,
							 DWORD dwPreDely,
							 float fExtraForceRate = FLOAT1,
							 bool bMoveDir=true );
	void AddReservedSliding( DWORD dwSlidingTime, float fForceAmt, float fFriction, float fAngle, bool bMoveDir );
	void AddReservedSliding2( DWORD dwSlidingTime, float fForceAmt,	float fFriction, const D3DXVECTOR3 &vMoveDir );
	void AddReservedSliding3( DWORD dwSlidingTime, float fForceAmt,	float fFriction, float fSlidingAngle, const D3DXVECTOR3 &vMoveDir );
	void CheckReservedSliding();
	void ClearReservedSliding();

	void RefreshFireTimeList( int iAniID,
							  const WeaponInfoList &vInfoList,
							  FireTimeType eFireTimeType,
							  float fTimeRate,
							  DWORD dwPreDelay, bool bMustCallAttack = false,
							  D3DXVECTOR3 &vPos = ioMath::VEC3_ZERO );
	
	void RefreshFireTimeListWithBase( DWORD dwWeaponBaseIndex,
									  int iAniID,
									  const WeaponInfoList &vInfoList,
									  FireTimeType eFireTimeType,
									  float fTimeRate,
									  DWORD dwPreDelay, bool bMustCallAttack = false,
									  D3DXVECTOR3 &vPos = ioMath::VEC3_ZERO );

	void RecoveryHP( float fAmt );

	int GetBuffJumpCnt();

public:
	void NotifyOwnerDamaged( const ioHashString &szAttacker, float fDamage, DWORD dwItemCode, int iWeaponType = -1 );
	bool CheckAimedTarget( float fAimAngle, float fAimMinRange, float fAimRange,
						   ioHashString &rkTarget, bool bDownAim, bool bOwnerTeam = true );
	bool CheckAimedTargetByBow( float fAimAngle, float fAimMinRange, float fAimRange, ioHashString &rkTarget, bool bDownAim, bool bOwnerTeam = true );
	bool CheckAimedTargetByStaff( float fAimAngle, float fAimMinRange, float fAimRange, ioHashString &rkTarget, bool bOwnerTeam = true );
	
	ioBaseChar* GetBaseChar( const ioHashString &rkName );

	D3DXVECTOR3 GetAimTargetPos();

public:
	DWORD AddPushPowerEnhanceRate( EnhanceOwnerType eType, float fRate, bool bWound );
	void RemovePushPowerEnhanceRate( EnhanceOwnerType eType, DWORD dwID, bool bWound );
	bool HasPushPowerEnhanceRate( EnhanceOwnerType eType, bool bWound );

	float GetEnhanceRate( EnhanceOwnerType eType );
	float GetWoundEnhanceRate( EnhanceOwnerType eType );


	void ModifyGetUpTimeRate( float fRate );
	void ModifyExtraStat( const Stat &rkStat, bool bAdd );

	void AddReflectInfo( const ReflectInfo &rkReflectInfo );
	void RemoveReflectInfo( const ioHashString &szName );

	void SetExtraRecoveryGaugeRate( float fRate );
	void SetRestorationGaugeByRate( float fRate );

	void AddDisableRecoverySkillGaugeBuff(ioHashString szBuffName);
	void RemoveDisableRecoverySkillGaugeBuff(ioHashString szBuffName);
	
	void SetKingScaleRate( float &fRate );
	void SetKingScaleSkeleton( ioHashString &rkSkeleton );
	float GetKingScaleRate(){ return m_fKingScaleRate; }

	void RemoveKingScaleRate( float fRate );
	void RemoveKingScaleSkeleton( const ioHashString &rkRestore );

	void SetHeadScaleRate( float &fRate );
	void RemoveHeadScaleRate( float fRate );

	void ModifyWoundRate( AttackRateList &vWoundedRateList, AttackSide iSide );
	void RemoveWoundRate( const AttackRateList &vWoundedRateList );
	void ModifyDamageRate( AttackRateList &vAttackRateList );
	void RemoveDamageRate( const AttackRateList &vAttackRateList );
	void ModifyNPCDamageRate( AttackRateList &vAttackRateList );
	void RemoveNPCDamageRate( const AttackRateList &vAttackRateList );

	bool IsDamageProtect();
	bool IsStateProtect( AttackSide eAttackSide, DefenseBreakType eBreakType );
	int GetStateProtectHPCancelType();
	bool IsRangeProtect( AttackMethodType eType );
	bool CheckIgnoreWeaponBuff( DWORD dwWeaponIndex );

	float GetCurStateProtectRate( AttackSide eAttackSide );
	AttackSide GetStateProtectSide( AttackSide eAttackSide );
	bool IsNoBlockStateProtect( AttackSide eAttackSide );
	bool IsNoDefenseBreakStateProtect( AttackSide eAttackSide );

	float GetCurDefenseDamageBuffRate();

	void SetExtraMoveSpeed( float fExtraSpeed, float fExtraDashSpeedRate = FLOAT1 );
	void RemoveExtraMoveSpeed( float fExtraSpeed );

	inline void SetExtraSkillGaugeRate( float fRate ) { m_fSkillGaugeRate = fRate; }
	inline void SetExtraDamageRate( float fRate ) { m_fExtraDamageRate = fRate; }

	void SetHideChar( bool bHide );
	void SetSpecialHide( bool bHide );
	inline const bool& GetSpecialHide() { return m_bSpecialHide; }

	void SetWeakBombType( ioHashString &szName, float fRate, int eType );
	void RemoveWeakBomb( ioHashString &szName );
	float GetWeakBombType( int eType );

	void SetWeakMagic( ioHashString &szName, float fRate, int eType, int eWeakType );
	void RemoveWeakMagic( ioHashString &szName );
	float GetWeakMagicType( int eType );
	bool IsProtectionMagicType( int eType );

	void SetReturnAttackAttribute( int iAttribute, int iResist );

public:
	void ApplyModifiedMaterial();
	int ModifyMaterialColor( const D3DCOLORVALUE &rkColor );
	void RestoreMaterialColor( int iChangeIndex );

	void AddMaterialModifyExcept( const ioHashString &szEntity );
	void RemoveMaterialModifyExcept( const ioHashString &szEntity );

	bool AddGlowLine( const ioHashString &rkEntity,
					  const D3DCOLORVALUE &rkColor,
					  float fThick );

	bool RemoveGlowLine( const ioHashString &rkEntity );

	void ApplyOutLineChange( ChangeOutLineType eType, const D3DCOLORVALUE &rkColor, float fSize );
	void RestoreOutLineChange( ChangeOutLineType eType );
	bool HasOutLineType( ChangeOutLineType eType );
	void FixedOutLine();

protected:
	void ApplyModifiedOutLine();
	void ApplyModifiedMaterialColor();

	bool IsModifyExceptEntity( const ioHashString &szName );

	void RemoveChangeOutLineRecord( ChangeOutLineType eType );
	void ChangeOutLine( const D3DCOLORVALUE &rkOutLineColor, float fOutLineSize );
	void RestoreOutLine();

public:
	int CheckItemDrop( bool bMove, bool bUsedSkill = false );
	int CheckArmorItemDrop();
	void CheckWearItemDrop( bool bDie = false );

	bool IsEnableDropObjectItem( ioItem *pItem );
	void CheckSpecialObjectItemDrop();
	void CheckObjectItemDrop( bool bDie = false, bool bDropZone = false );
	void ItemDrop( EquipSlot eSlot, float fXPos = 0.0f, float fZPos = 0.0f );
	void ItemMoveDrop( EquipSlot eSlot, const ioHashString &szAttacker, const ioHashString &szSkillName,
					   float fOffSet, float fSpeed );

	void AllItemDrop();
	void FillAllItemDrop( SP2Packet &rkPacket );
	
	void ClearItemOwnerByOutUser( const ioHashString &rkName );
	void ClearIfThisActiveSkill( ioSkill *pSkill );
	void ClearSpecialMotion();
	void ClearEtcItemMotion();

	ioItem* EquipItem( ioItem *pItem, bool bThread = false );
	void NewReserveEquipObjectItem( DWORD dwCode, int eCreateType, ioHashString szCratorName );
	void ExecuteReserveEquipObjectItem();
	void ClearReserveEquipObjectItem();

	ioItem* ReleaseItemExplicit( DWORD dwItemCode, DWORD dwItemCreateIndex, int iSlot );
	void DestroyAllItem();
	void DestroyAllAccessoryItem();

	int GetEquipItemCnt();
	ioItem* GetEquipedItem( int iSlot );
	ioItem* GetEquipedItem( const ioHashString &szItemName );
	ioItem* GetEquipedItemByItemCode( DWORD dwItemCode );
	ioSkill* GetEquipedSkill( int iSlot );
	ioSkill* GetEquipedSkill( const ioHashString &szSkillName );
	ioSkill* GetEquipedSkillByItemName( const ioHashString &szItemName );
	ioSkill* GetUseEnableSkill( int iSlot, SkillUseType eType, bool bHelpMsg=false );
	ioSkill* GetAccessoryCompoundSkill( int iSlot );
	bool CheckDisableSkillByMode( const ioHashString &szSkillName );

	bool CheckCharChangeStateByEquipItem();

	bool EnableDecoChangeByEquipItem( int iDecoType );

	ioSkill* GetCurActiveSkill();
	ioSkill* GetMustActiveSkill();

	ioEquipSlot* GetEquipSlot() { return m_pEquipSlot; }
	ioAttackableItem* GetEquipedAttackableItem( DWORD dwItemCode, int iCreateIndex ) const;
	ioAttackableItem* GetPriorityItem( StatePriorityType eType ) const;
	int GetDefaultPriority( StatePriorityType eType ) const;
	ioAttackableItem* GetPriorityItemByAttackType( AttackType eType ) const;
	bool GetPriorityFeature( StatePriorityType eType,
							 ItemFeature eFeature,
							 bool bNullTrue = true ) const;

	ioWeaponItem* GetWeapon() const;
	DWORD GetWeaponItemCode();
	ioArmorItem* GetArmor() const;
	ioHelmetItem* GetHelmet() const;
	ioCloakItem* GetCloak() const;
	ioObjectItem* GetObject() const;
	ioWearItem* GetWear() const;

	bool IsHasCrown() const;
	bool IsHasCrown( ioItem::ModeCrownType Type ) const;
	bool IsHasCrownByAllMode() const;

	void ObjectItemUse( DWORD dwItemCode, int eState );
	void ReleaseObjectItem( const ioHashString& szLog, int eState = -1, bool bSendNetwork = true );
	void ReleaseObjectItemImmediately( const ioHashString& szLog, int eState = -1, bool bSendNetwork = true );
	void OnReleaseObjectItem( int eState = INT_MAX );

public:
	void ItemEquipAfterUIProcess( int iItemType );
	void ItemReleaseAfterUIProcess( int iItemType );
	void ItemEntityChangeAfterUIProcess( int iItemType );

public:
	ioEffect* AttachEffect( const ioHashString &rkEffect, ioEntityEvent *pEntityEventOwner = NULL, bool bDependence=true );

	ioEffect* GetEffectByObjID( UniqueObjID eObjID );
	ioEffect* GetEffectByFileName( const ioHashString &rkFileName );

	void EndEffect( const ioHashString &rkEffect, bool bGrace = true, bool bAllEnd = true );
	void EndEffect( UniqueObjID eObjID, bool bGrace = true );

	bool IsAxisLimit();
	bool CheckAxisLimit( D3DXVECTOR3 *pNewPos );

protected:
	void ProcessPreState();
	void ProcessState();
	void ProcessExtraState();	
	void ProcessFlyMove( float fTimePerSec );
	void ProcessPushMove( float fTimePerSec );
	void ProcessFloatMove( float fTimePerSec );
	void ProcessFlashMove( float fTimePerSec );
	void ProcessRealComboResetCheck();
	void ProcessDefenceCounter( float fTimerPerSec );

	void CheckFlyMoveDir( float fTimePerSec );
	bool CheckFlyHeight();

	void ProcessMapFriction( float fTimePerSec );
	void ProcessMoveSpeed( float fTimePerSec );

	bool ProcessCurMoveSpeed( float fPreMoveSpeed, float fTimePerSec, D3DXVECTOR3 &vMoveAmt );
	bool ProcessRackMove( float fTimePerSec, D3DXVECTOR3 &vMoveAmt );

	void ProcessRotate( float fTimePerSec );
	void ProcessBuff( float fTimePerSec );

	void ProcessProtectionMode();
	void ProcessActionStopDelay( float fTimePerSec, float fLooseRate );
	void ProcessRopeWorldForce( float fTimePerSec );

	void ProcessDiffuseRate( float fTimePerSec );
	void ProcessDiffuseRateColor( float &fCurRate, float fTargetRate, float fTime );

	void ProcessNoInputDelayTime();
	void ProcessNetworkTimeOutEmoticon();
	
	void ProcessBossScale();

	void ProcessKeyInput();
	void ProcessMove( float fTimePerSec );

protected:
	void ProcessObjectItem();

protected:
	void ProcessCheckCharSpecialState();

public:
	bool IsFlyMapLimit();
	bool ProcessForceMove( float fTimePerSec, D3DXVECTOR3 &vMoveAmt );

protected:
	virtual void ProcessRecovery();

public:
	void GhostWalk( const D3DXVECTOR3 &vRecvPos,
					DWORD dwHostTime,
					bool bFloatingState,
					bool bFloating );

	bool ChangeDirectionByInputDir( bool bMoveDirOnly, bool bSendNetWork = true );

	bool ChangeDirectionExplicitIfKeyChanged( bool bMoveDirOnly );
	bool ChangeDirectionExplicitIfDirChanged( bool bMoveDirOnly );
	
	void StartNoInputDelayTime( DWORD dwDuration = 0 );

protected:
	void PredictMove( float fTimePerSec );
	void ChangeJDirectionByInputDir( ioUserKeyInput::DirKeyInput eDir );

public:

	void SetNPCIndex( DWORD dwNPCIndex ) { m_dwNPCIndex = dwNPCIndex; }
	void SetUserIndex( DWORD dwUserIndex ) { m_dwUserIndex = dwUserIndex; }	
	inline DWORD GetUserIndex() { return m_dwUserIndex; }
	inline DWORD GetNPCIndex() { return m_dwNPCIndex; }

	ioBuff* AddNewBuff( const ioHashString &rkBuffName,
						const ioHashString &szCreator,
						const ioHashString &szCreateItem,
						ioSkill *pSkill,
						bool bTeamAttack = false );

	ioBuff* AddNewBuffWithAnimateTime( const ioHashString &rkBuffName,
									   const ioHashString &szCreator,
									   const ioHashString &szCreateItem,
									   ioSkill *pSkill,
									   DWORD dwAniTime,
									   bool bPassive = false );

	void ReserveAddNewBuff( const ioHashString &rkBuffName,
							const ioHashString &szCreator,
							const ioHashString &szCreateItem,
							ioSkill *pSkill,
							bool bTeamAttack = false );

	bool RemoveBuff( int iTypeID, bool bRemovePassive = true );
	bool RemoveBuff( const ioHashString &rkBuffName, bool bRemovePassive = true );
	bool RemoveBuffType( int iTypeID, bool bRemovePassive = true );
	bool RemoveBuffOwnerName( const ioHashString &rkBuffName, const ioHashString &szCreatorName );
	void RemoveAllBuff();
	void RemoveRoundEndAllBuff();
	void RemoveActiveBuff();
	void RemoveHasAreaWeaponBuff( const ioHashString &szCreator );
	void CheckWoundCancelBuff( int iTypeID );
	void CheckExtraCallBuff();

	bool HasBuff( int iBuffType ) const;
	bool HasBuff( int iBuffType, const ioHashString &rkCreator ) const;
	bool HasBuff( const ioHashString &rkName ) const;
	bool HasBuff( const ioHashString &rkCreator, const ioHashString &rkName ) const;

	int GetHasBuffCnt( const ioHashString &rkName );
	bool HasSameSkillBuff( const ioHashString &rkSkillName, const ioHashString &rkBuffName ) const;
	bool HasOtherBuff( ioBuff *pBuff ) const;
	bool IsSkillBuffLiveButStopped( const ioHashString &rkSkillName ) const;
	bool HasIgnoreDefenseBreakBuff() const;
	bool HasDefenceBreakHalfPassiveBuff() const;
	bool HasCharCollisionSkipBuff() const;
	bool HasDisableSkillFlameBuff() const;
	bool HasChangeTeamBuff( bool bCheckNoAim ) const;
	bool HasEnablePossessionBuff();
	bool HasCollisionAvailableBuff() const;

	float GetBuffAccumSpeedRate() const;
	
	ioBuff* GetRefudBuff();

	ioBuff* GetBuff( const ioHashString &rkName );
	ioBuff* GetBuff( const ioHashString &rkName, const ioHashString &szCreatorName );
	ioBuff* GetBuff( int iBuffType );
	ioBuff* GetBuff( int iBuffType ) const;
	ioBuff* GetBuffConst( int iBuffType ) const;
	ioBuff* GetBuff( int iBuffType, const ioHashString &szCreatorName );
	void GetAllBuff( ioHashStringVec& rDestBuffList );
	int GetInvisibleSetBuffCount();

	void SetReserveEndBuff( const ioHashString& szNuffName );
	void SetReserveEndBuff( int eType, const ioBuff* const pIgonreBuff = NULL );

	ioBuff*	GetUseActiveCountInBuff( const ioHashString &szSkillName );
	ioBuff* GetUseActiveCountInBuff();

	bool CheckMustBlowWoundBuff();
	float CheckCarefulStepCloseOrder();
	void CheckWoundEffectBuff();
	
	float CheckChangeMoveSpeedBuff() const;

	bool CheckCurBuffAdd( ioBuff *pBuff );
	bool CheckBuffKillException( ioBuff *pBuff );
	bool CheckBuffSuicideExcetion( ioBuff *pBuff );
	bool CheckBuffRemoveByType( ioBuff *pBuff );

	void ApplyEndBuff( const ioHashString &szBuff );
	void ApplyNonePlayStateBuff( bool bNonePlayState );
	void FloatModeEventBuffEnd();

protected:
	void SetReserveAddNewBuff();

public:
	void InitExtendJumpAttackTagInfo();
	void SetExtendJumpAttackEndJumpInfo( int iAniID, float fTimeRate, DWORD dwPreDelay );
	bool CheckExtendJumpAttackEndJump();
	bool CheckJumpLimitCnt();
	void InitCurSkillEndNextLandRate()			{ m_fCurSkillEndNextLandRate = FLOAT1; }

protected:
	void GenerateWeaponDetail( ioWeapon *pWeapon,
							   const FireTime *pFireTime,
							   const D3DXVECTOR3 &vFirePos,
							   const D3DXVECTOR3 &vFireDir,
							   ioSkill *pFireSkill = NULL );

	bool IsDiffuseRateSame() const;
	bool IsCanComboJumpAttack( float fHeightGap );
	bool IsCan_D_JumpAttack( float fHeightGap );
	bool IsCanFlashJumpAttack( float fHeightGap );
	bool IsCanComboFlashAttack( float fHeightGap );
	bool IsCanCollectJumpAttack( float fHeightGap );
	bool IsCanJumpReload( ioAttackableItem *pItem, float fHeightGap );
	bool IsCanRJumpAttack( float fHeightGap );
	bool IsCanChargeComboJumpAttack( float fHeightGap );

	bool IsDefenseEnable( AttackSide eSide, DefenseBreakType eBreakType, bool bCheckBreak ) const;
	bool IsCanAimJumpAttack( float fHeightGap ) const;
	bool IsCanBuffJump( float fHeightGap );
	bool IsCanBuffFlyJump( float fHeightGap );
	bool IsCanBuffMultiFlyJump( float fHeightGap );
	bool IsCanJumpAttack( float fHeightGap );
	bool IsCanBuffAirWalk( float fHeightGap );
	
	bool CheckDownSkill();
	bool CheckBlowWoundSkill();
	bool CheckWoundSkill();
	bool CheckEscapeSkill();
	bool CheckIceStateSkill();
	
	AttackSide GetAttackedSide( const D3DXVECTOR3 &vAttackDir ) const;

	ioFieldRewardItem *CheckNowPickRewardItem();
	ioFieldItem* CheckNowPickItem();
	ioFieldItem* CheckNowPickItemByKey();
	int CheckNowPushStruct();

	void CheckLanding( float fHeightGap, DWORD dwCurTime );
	
public:
	bool CheckJumpingSkill();
	bool CheckJumpingSkill( int iEquipSlot );

	bool CheckLandingInfo( float fHeightGap );
	CharLandingType GetLandingType() { return m_CharLandType; }

public:
	inline bool IsPrisonerMode() const { return m_bPrisonerMode; }
	inline bool IsCatchMode() const { return m_bCatchState; }
	inline bool IsReturnJumpping() const { return m_bReturnJumpping; }
	void SetReturnJumpping( bool b )	{ m_bReturnJumpping = b; }

public:
	const ForceInfoList& GetForceInfoList( AttackType eType, int iSubIdx = 0 ) const;
	const WeaponInfoList& GetAttackAttributeIdx( AttackType eType, int iSubIdx = 0 ) const;

	float GetFirstFireTime( const ioHashString &rkAniName );
	float GetAttackDamageRate( ioWeapon *pWeapon );
	float GetAttackNPCDamageRate( ioWeapon *pWeapon );

	int GetMaxCombo() const;
	int GetExtraDashAttackMax() const;
	int GetExtraOtherDashAttackMax() const;

	float GetJumpPower() const;
	float GetWeakAttackPushRate();

protected:
	int GetAnimationIdxAfterConvertName( const ioHashString &szAniName, bool bMale ) const;

public:
	DWORD GetCurAnimationFireTime();

public:
	// SpecialState인 특수 상태는 현재 CS_BATTLE_WAITING, 
	// 현재 상태(m_CharState)가 CS_BATTLE_WAITING이고 bSpecialStateUnLock == true일 경우 특수 상태에서 다른 상태로 변경될 수 있다.. false면 변경되지 않음
	void SetState( int iState, bool bSendNetWork = true, bool bUnLockSpecialState = false );		

	inline CharState GetState() const { return m_CharState; }	
	inline CharState GetPreState() const { return m_PreCharState; }
	inline CharState GetNextState() const { return m_NextCharState; }
	inline JumpState GetJumpState() const { return m_JumpState; }
	inline JumpType GetJumpType() const { return m_CurJumpType; }
	inline BlowDashState GetBlowDashState() const { return m_BlowDashState; }
	inline BlowWoundedState GetBlowWoundedState() const { return m_BlowState; }
	inline DieState GetDieState(){ return (DieState)m_DieState; }
	inline ioSpecialStateBase* GetSpecialState(){ return m_pCharSpeicalState; }
	const char* GetStateText() const;
	const char* GetJumpStateText() const;
	const char* GetAttackStateText() const;

	// dwDuration == 0 이면 프로텍션 실패. 만약 걸려있었으면 해제까지 됨.
	void SetProtectionMode( ProtectionMode eMode, DWORD dwDuration, bool bUseEmoticon = false );
	ProtectionMode GetProtectionMode() { return m_ProtectionMode; }
	DWORD GetChangeCharProtectTime() { return m_dwChangeCharProtectTime; }

	void SetDashState( bool bAimState = false );
	void SetRunDashState();
	void SendDashState( bool bAimState, bool bPassAutoTarget = false);
	DWORD GetDashStartTime() { return m_dwDashStartTime; }

	void SetDashExtraInfo();
	void CheckExtendDashState();
	void CheckRefreshExtendDashState();

	// For DashFail
	void SetDashFailValue( const ioHashString& szAni, float fRate, const ioHashString& szEmoticon );
	void SetDashFailState();
	void CheckDashFailState();

	void SetDashFailStart();

	void SetDefenseState();
	void CheckDefenseState();
	void CheckWoundedDefenseState();
	bool SetExtendDefenseState();
	bool CheckExtendDefenseEnable();
	void CheckExtendDefenseState();
	void ApplyExtendDefenseState( SP2Packet &rkPacket );

	void CheckDodgeDefenseState();
	void ApplyDodgeDefenseState( SP2Packet &rkPacket );

	void CheckDefenseAttackState();
	void ApplyDefenseAttackState( SP2Packet &rkPacket );

	void PreReleaseDefenseState();
	void ReleaseDefenseState();

	bool IsCanJumpState();
	bool IsSpecailAttackState();

	void SetJumpState( bool bSync = true );
	void SetCrossBowJumpState();
	void SetSKillEndJumpState( float fJumpPower, bool bUsedAttack=false, bool bInitLandRate=false, bool bSendNet=true, bool bMustJump=false, bool bExtendJumpAni = false );
	inline void SetSkillNextJumpAmt( float fSkillNextJumpAmt ){ m_fSkillNextJumpAmt = fSkillNextJumpAmt; }
	inline float GetSkillNextJumpAmt(){ return m_fSkillNextJumpAmt; }

	
	void SetChangeJumppingState( float fPowerRate, bool bCheckState, int iLimitCnt );
	void ApplyChangeJumppingState( SP2Packet &rkPacket );

	int GetCurChangeJumppingCnt();
	int GetLimitChangeJumppingCnt();
	void IncreaseChangeJumppingCnt();

	void SetAimJumpState( bool bFullTime );
	void SetJumpAttack();
	void SetBuffJump();
	void SetBuffFlyJump();
	void SetBuffFlyJumpEnd();

	void SetBuffMultiFlyJump();
	void SetBuffMultiFlyJumpEnd();

	void SetBuffAirWalk();
	void SetBuffAirWalkEnd();

	void CheckPassvieDoubleJumpBuff();

	void CheckBuffJumpAniIndex( IN const CharBuffJumpInfo &rkJumpInfo,
								OUT int &iIndex, OUT int &iIndex2, OUT float &fRate,
								OUT D3DXVECTOR3 &vJumpDir,
								OUT float &fJumpPower,
								OUT float &fForcePower );
	bool CheckBuffJumpEnableAttack() const;
	
	void SetJumpReload();
	void ApplyJumpReload( SP2Packet &rkPacket );

	void SetJumpping( bool bFullTime, bool bSync = true );
	void SetJumppingState( bool bInitJumpPowerAmt = true );		//JS_JUMPPING로 변경
	void SetJumpAttackState();		//JS_JUMP_ATTACK로 셋팅
	void SetCollectJumpAttack( int iType );
	void SetComboDashState();

	void SetNewJumpAttackState( bool bExtendAttack = false, bool bSendPacket = true );
	void SetNewJumpAttackStateInfo( DWORD dwEndTime, DWORD dwResereveTime, float fGravityAmt, bool bRefresh=false );

	void SetJumpDefense( DWORD dwKeyPressTime, DWORD dwDefenseEnableTime, DWORD dwReleaseTime );
	void ReleaseJumpDefense( DWORD dwReleaseTime );

	void SetWoundedState( const ioHashString &szWoundedAni,
						  DWORD dwWoundedDuration,
						  float fDefenseEnableRate,
						  bool bLoopAni,
						  bool bIgnoreCatch );

	void SetBlowWoundedState( bool bJumped, const BlowWoundAniInfo &rkBlowWoundAniInfo,
							  bool bGetUpState = false, bool bFallState = false );
	void SetBlowWoundedGetUp();

	void SetBoundWoundedState( float fBoundJump,
							   float fBoundForce,
							   float fBoundGravity,
							   float fBoundDamage,
							   bool bGetUpState = false );

	void SetBlowExtendWoundedState( const ioHashString& szExtendWoundAni,
									DWORD dwExtendWoundDuration,
									float fExtendWoundJump,
									float fExtendWoundForce,
									float fDefenseRate,
									bool bWoundAniLoop );

	bool IsCanNormalAttack() const;
	void SetNormalAttack( int iCurCombo );
	void SetDefaultNormalAttack( int iCurCombo, bool bRefreshFire = true );
	DWORD SetNormalAttackByAttribute( const AttackAttribute &rkAttr,
									 bool bRefreshFire = true,
									 float fExtraAniRate=1.0f,
									 float fExtraForceRate=1.0f );

	void SetCurNormalAttackItem( ioAttackableItem *pItem );

	void ApplyNormalAttackResult( int iCurCombo, bool bComboEnd = false );
	bool CheckDefaultNormalAttack();
	bool CheckDefaultNormalAttack( DWORD dwStart, DWORD dwDuration, DWORD dwDelay );
	bool CheckCancelNormalAttack();
	bool CheckCancelNormalAttack( DWORD dwStart, DWORD dwDuration, DWORD dwDelay );
	void NormalAttackOver( bool bDelay=true );

	bool IsShowGetUpStick();
	bool IsShowGetUpStick2();

	void SetShowPressButton( bool bShow );
	bool IsShowPressButton() const;

	bool IsCanDashAttack() const;
	void SetDashAttack();
	void SetExtraDashAttack();
	void SetExtraOtherDashAttack( bool bEndDash );

	bool CheckEnableDashAttEndDash();

	void SetDashAttackByAttribute( const AttackAttribute &rkAttr, bool bSetAutoTarget = true );
	void SetBlowDashAttackByAttribute( const AttackAttribute &rkAttr, const BlowDashInfo &rkBlowDashInfo );

	void SetPickRewardItemState( ioFieldRewardItem *pPickItem );
	void SetPickItemState( ioFieldItem *pPickItem );
	void SetDeleteFieldItemState( ioFieldItem *pPickItem, ioHashString szDeleteAni );
	bool SetUseSkill(int iSkillNum, SkillUseType eType, bool bNetwork=false, bool bCancelSkill=false );
	bool SetUseSlotSkill(const ioHashString &szSkillName, SkillUseType eType, bool bNetwork=false, bool bCancelSkill=false );

	inline ioSkill* GetCurrActiveSkill(){ return m_pActiveSkill; }

	void ClearAttackFireTimeAndSkill( ClearAttackFireType eType = CAFT_NONE );

	void SetDropZoneDownState();

	bool IsCanCounterAttack() const;
	void SetCounterAttack( CharCounterAttackKeyState KeyState );
	void SetCounterAttackByAttribute( const AttackAttribute &rkAttr );
	void ProcessDefaultCAT();

	void SetEscapePrisonerState();
	void SetPushStructState( int iIndex );
	void SetCurPushStructByNetwork( int iIndex );

	void SetDieState();
	bool IsChangeWaitState();
	void SetChangeWaitState( int iArray, bool bSendChangeCharMsg = true, bool bClassType = false );
	void SendChangeChar( int iArray );
	void TrainingChangeChar( int iClassType );
	void ClearBullet();
	void AddChangeCharPacketReserve( DWORD dwCharIndex, SP2Packet &rkPacket );
	void ChangeCharReservePacketProcess();

	void SetFrozenState( bool bPreFrozen, DWORD dwMinTime, DWORD  dwMaxTime );
	void SetStopMotion( int iType );
	void SetReadyState();
	void SetObserverState();
	void SetDrinkState();
	void SetRoundEndMotion( RoundEndMotion eEndMotion );
	bool SetRoundEndCharMotionItem( DWORD dwRoundEndCharMotionItem, bool bAniSet );
	void SetCreateMotionState();
	void SetStartMotionState();
	void SetCoinRebirthState();
	void SetChangeGangsiState();
	void SetCurseRollingState();
	void SetGradeUpMotionState();

	void SetIceState( const ioHashString &szBuffName,
					  const ioHashString &szBuffCreator,
					  const WeaponInfo &rkInfo,
					  float fEnableGravityAmt );

	void CheckIceBound();
	inline bool IsCheckIceBound() const { return m_bCheckIceBound; }

	void SetStartPos();

	void SetWarpState();
	D3DXVECTOR3 GetEnableWarpPosition( ioBaseChar *pChar, int iAngelCnt, float fRange, float fMaxRange, bool bCheckDropZone,
									   bool bCheckHeight, float fLimitHeight, bool bSkillPos = false, int iStartingPoint = 0 );
	D3DXVECTOR3 GetEnableTeleportPosition( const D3DXVECTOR3& vMainPos, ioBaseChar *pChar, int iAngelCnt, float fRange, float fGapHeight );

	bool IsCanJumpAttackEnableTime();

	void SetCmdInputStart( int iType );
	void SetCmdInputEnd();
	void SetSkillInputStart();

	void SetCmdAttackState();
	void CheckCmdAttackState();

	void StartScreenBlindBuff2( const ScreenBlindTime *pTime, const ScreenBlindSetting *pSetting, float fRange );
	void EndScreenBlindBuff2();
	void EndScreenBlind2();

	void SetItemMeshShow();
	void SetItemMeshHide();

	// defense cancel state
	void CheckDefenseCancelState();

	// 임시 : 더미상태
	void CheckDummyState();

public:
	bool IsEnableActionStopDelay();
	bool IgnoreDashFail();
	bool IgnoreBlock();
	bool IsEnableJumpKeyEtcAction();

public:
	bool IsActionStop() const;
	void InitActionStopDelay( bool bAll );
	void SetActionStopDelay( float fDamage, bool bDie, int iShakeCamera = 0 );

	DWORD GetActionStopCurAdd() const { return m_dwActionStopCurAdd; }
	DWORD GetActionStopTotalDelay() const { return m_dwActionStopTotalDelay; }	// kaseton : action delay

	static DWORD CalcActionStopDelay( float fDamage, bool bDie );
	static float GetActionStopLooseRate( bool bDie );

	void SetCurJumpType( JumpType iJumpType );
	void SetCurDashType( DashType iDashType );

	void SetAimState( bool bAim = false );
	void ApplyAimState( SP2Packet &rkPacket );
	void SetJumpAutoShotState();

	void ClearJumpAimState();
	
	void SetCrossBowAimState();
	void SetCrossBowJumpAimState();
	void ApplyCrossBowAimState( SP2Packet &rkPacket );

protected:
	void CheckDefaultJumpType( char* szJumpType );
	void CheckCurJumpType();
	void CheckCurDashType();
	void CheckCurCounterType();
	
public:
	void ClearAttackFireTimeList();

protected:
	void CheckDelayRunState();
	void CheckJumpState();
	void CheckSkillState();
	void CheckNormalAttackState();
	void CheckComboAttackReserve( DWORD dwStart, DWORD dwDuration, DWORD dwDelay );
	void CheckDashAttackState();
	void CheckWoundedState();
	void CheckBlowWoundedState();
	void CheckPickItemState();
	void CheckEndMotionState();
	void CheckPushStructState();
	void CheckEscapeState();
	void CheckDropZoneDownState();
	void CheckCounterAttackState();
	void CheckDrinkState();
	void CheckEtcState();
	void CheckMissFireState();

	void CheckForceDropping();
	void CheckDefenseReserveAttack();

	void CheckFastGetup();
	void CheckStopMotionState();
	void CheckFrozenState();
	void CheckCloseOrderState();
	void CheckStartMotionState();
	void CheckCreateMotionState();
	void CheckCoinRebirthMotionState();
	void CheckTeleportState();
	void CheckGradeUpMotionState();

	void CheckBoundBlowWoundedState();
	void CheckBlowExtendWoundedState();
	void CheckChangeGangsiMotionState();
	void CheckCurseRollingState();
	void CheckDownRollingState();
	void CheckBuffAirAttackState();

	void CheckNotMoveProtect();

	void CheckIceState();
	void CheckNewJumpAttackState();
	void CheckRetreatState();

	void CheckBlowDashAttackState();
	void CheckAdhesiveState();
	
	void CheckObjectEquipState();

	void CheckLandAfterState();
	void CheckReserveKeyInput();

	void ProcessReserveKeyInput( bool bDefense = false );
	bool CheckDefenceKeyInput();
	bool IsReserveKeyInput();

	int CheckBlowEndAni();

public:
	void ClearReserveKeyInput();
	void ClearSkillReserveKey();

	bool IsEnableReserveKeyInput();

public:
	void SetAutoTarget( AutoTargetType eType );
	void SetAutoTargetExplicit( const ioHashString &szTarget,
								const D3DXVECTOR3 &vBasisDir,
								DWORD dwDuration,
								AutoTargetType eType,
								bool bAutoTargetBall,
								bool bAutoTargetDummyChar );

	void StopAutoTargetTracking();
	void SetAutoTargetTrackingEndTime( DWORD dwTime );

	void FillAutoTargetInfo( SP2Packet &rkPacket );
	void ApplyAutoTargetInfo( SP2Packet &rkPacket );

public:
	void CheckFallState();	// Falling Process 
	void CheckFlyState();
	void CheckFloatState();
	void CheckProtectionState();
	void CheckToFallStateChange();

	void SetFallState( bool bChange );	// bChange == false -> Not AniChange

	void SetTeleportState( const ioHashString &szStartAni,
						   float fStartRate,
						   const ioHashString &szEndAni,
						   float fEndRate,
						   const D3DXVECTOR3 &vEndPos );

public:		// Process Prisoner Mode(포로)
	void SetPrisonerMode();
	void EscapePrisonerMode( TeamType eLoseTeam, bool bUseEffect, bool bZeroHP=false );
	bool CheckPrisonerMode( float fDamage );
	void DestroyPrisonerRope();

	const ioHashString &GetPrisonerRope();
	const ioHashString &GetPrisonerEffect();
	const ioHashString &GetEscapeEffect();

public:
	void ApplyUseAutoSkill( int iSkillNum );

	void SetDieBlowEnd( bool bEffect=true );	
	void ApplyDropDamage( float fDropDamage );
	void ApplyFallDamage( bool bNetwork );
	void ApplyDamage( float fDamage, bool bSysDamage );
	void DropDieProcess();

	bool CheckBoundDie();

public:
	void SetIndex( const DWORD dwIndex );
	void SetLevel( const int iLevel );
	void SetClassLevel( const int iLevel );
	void SetCharName( const ioHashString &rkName );

	void SetUpKeyValue( const ioUserKeyInput::UseKeyValue &rkUseKeyValue  );
	void SetUpSecondKeyValue( const ioUserKeyInput::UseKeyValue &rkUseKeyValue );
	void SetUpJoyKeyValue(  const ioUserKeyInput::UseKeyValue &rkUseKeyValue  );

	void SetCurMoveSpeed( float fSpeed );
	void SetSpeedRate( float fSpeed) {m_fSpeedRate = fSpeed;}

	void SetTargetRotAndMoveDirChange( const D3DXQUATERNION &qtRot, bool bRotateOnly = false );
	void SetTargetRot( const D3DXQUATERNION &qtRot );
	
	void SetMoveDirection( const D3DXVECTOR3 &vDir );
	void SetMoveDirByRotate( const D3DXQUATERNION &qtRot );
	void SetInputDirection( const D3DXVECTOR3 &vDir );

	void SetTargetRotToTargetPos( const ioBaseChar *pTarget, bool bImmediately, bool bRotateOnly = false );
	void SetTargetRotToTargetPos( const D3DXVECTOR3 &vPos, bool bImmediately, bool bRotateOnly = false );
	void SetTargetRotToRotate( const D3DXQUATERNION &qtRot, bool bImmediately, bool bRotateOnly = false );
	void SetTargetRotToDir( const D3DXVECTOR3 &vDir, bool bImmediately, bool bRotateOnly = false );

	void SetJumpAttckTargetRot( const D3DXQUATERNION &qtRot, const D3DXVECTOR3 &vDir, const D3DXVECTOR3 &vInput );

	void SetTargetRotToDirKey( ioUserKeyInput::DirKeyInput eDirKey );

	void SetDefenseEnable( bool bDefense );
	void SetDefenseMoveEnable( bool bDefenseMove, bool bDefenseRotate );
	void SetReloadMoveEnable( bool bDefenseMove );
	void SetAttackMoveEnable( bool bAttackMove );
	void SetSkillMoveEnable( bool bSkillMove, bool bChangeDir );

	inline DWORD GetIndex() const { return m_dwIndex; }
	inline int GetLevel() const { return m_iLevel; }
	inline int GetClassLevel() const { return m_iClassLevel; }
	inline const ioHashString& GetCharName() const { return m_Name; }
	inline const ioHashString& GetViewName() const { return m_ViewName; }

	inline const D3DXVECTOR3& GetMoveDir() const { return m_vMoveDir; }
	inline const D3DXVECTOR3& GetInputDir() const { return m_vInputDir; }
	inline const void SetInputDir( D3DXVECTOR3 vDir )	{ m_vInputDir = vDir; }

	inline const D3DXQUATERNION& GetTargetRot() const { return m_qtTargetRot; }
	inline const D3DXVECTOR3& GetPrePosition() const { return m_vPrePosition; }

	inline const D3DXVECTOR3& GetRackMoveAmt() const { return m_vRackMoveAmt; }

	inline DWORD GetSendChangeCharTime() { return m_dwSendChangeCharTime; }
	inline void  SetSendChangeCharTime( DWORD dwRealTime ) { m_dwSendChangeCharTime = dwRealTime; }
	inline bool  IsAttackMoveEnable(){ return m_bAttackMoveEnable; }

	float GetCurSpeed();
	float GetAccelSpeed() const;
	float GetMaxSpeed( bool bExtra=true ) const;
	float GetJumpMaxSpeed( bool bExtra=true ) const;
	void  RecalcJumpMaxSpeed( bool bExtra=true, bool bReset = false );
	float GetRunDashAniRate() const;

	void SetComboCheckTime( DWORD dwComboTime, bool bKeyReserve = false );

	inline void SetUsedJumpAttack( bool bUsed ) { m_bUsedJumpAttack = bUsed; }
	inline bool GetUsedJumpAttack() const { return m_bUsedJumpAttack; }

	inline void SetUsedBuffJumpAttack( bool bUsed ) { m_bUsedBuffJumpAttack = bUsed; }
	inline bool GetUsedBuffJumpAttack() const { return m_bUsedBuffJumpAttack; }

	inline int GetUsedBuffJump() const { return m_UsedBuffJump; }
	inline int GetUsedBuffFlyJump() const { return m_UsedBuffFlyJump; }
	inline int GetUsedBuffMultiFlyJump() const { return m_UsedBuffMultiFlyJump; }
	inline DWORD GetJumpMoveStartTime() { return m_dwJumpMoveStartTime; }
	inline void SetUsedFlashJump( bool bFlashJump ) { m_bUsedFlashJump = bFlashJump; }

	inline void SetCurJumpPowerAmt( float fAmt ) { m_fCurJumpPowerAmt = fAmt; }
	inline float GetCurJumpPowerAmt() { return m_fCurJumpPowerAmt; }

	inline void SetGravityAmt( float fAmt ) { m_fGravityAmt = fAmt; }
	inline float GetGravityAmt() { return m_fGravityAmt; }

	inline bool IsCurFrameDrop() { return m_bCurFrameDrop; }
	inline void SetCurFrameDrop( bool bFrameDrop ) { m_bCurFrameDrop = bFrameDrop; }

	inline void SetCurGravityGapAmt( float fAmt ) { m_fCurGravityGapAmt = fAmt; }
	inline float GetCurGravityGapAmt() { return m_fCurGravityGapAmt; }
	bool CheckEnableLandAttackByGravity( float fGravity );

	inline void SetForcePowerAmt( float fAmt ) { m_fForceAmt = fAmt; }
	inline float GetForceAmt() const { return m_fForceAmt; }
	inline const D3DXVECTOR3& GetForceDir() const { return m_vForceDir; }

	inline AttackSide GetCurAttackedSide() const { return m_AttackedSide; }

	inline void SetSuccessJumpAttack( bool bSuccess ) { m_bSuccessJumpAttack = bSuccess; }

	inline const ioHashString& GetCurSkipStunBuffName() const { return m_SkipStunName; }
	inline const ioHashString& GetCurAdhesiveBuffName() const { return m_AdhesiveName; }

	float GetCharGravity();

	// For RecoveryGauge
	inline float GetDefaultRecoveryGauge() const { return m_fDefaultRecoveryGauge; }
	inline DWORD GetDefaultRecoveryGaugeTic() const { return m_dwDefaultRecoveryGaugeTic; }

	inline float GetDelayRunGaugeRate() const { return m_fDelayRunGaugeRate; }
	inline float GetEtcGaugeRate() const { return m_fEtcGaugeRate; }

	//
	inline const ioHashString& GetCurMineTraceStateBuffName() const { return m_MineTraceStateBuff; }

public:
	SkillUseType	GetAvailableSkillUseType();	// using in AI : implemented on ioBaseCharAISupport.cpp
	int				GetCurAttackComboNum()	{	return m_iAttackCombo;	}
	bool			IsCharacterWounded();

public:
	bool IsJumpingState();		
	bool IsDownState();
	bool IsBlowDownState();
	bool IsFloating();	// 맵 바닥보다 높은 상태
	bool IsJumppingState() const;	// CS_JUMP 이거나 CS_DASH_ATTACK && m_bJumpDashAttack
	bool IsFloatingState() const;
	bool IsDropZoneDownState() const;
	bool IsWoundedState() const;
	bool IsWoundedGrapplingState( GrapplingType eCheckCharState ) const;

	// 2020-02-19
	bool IsAttackState();
	bool IsDashState();

protected:
	virtual void UpdateBound() const;
	virtual void UpdateBottomHeight() const;
	void UpdateWorldAxisBox() const;

public:
	D3DXVECTOR3 GetCharHeadPos( float fSizeScale ) const;
	virtual D3DXVECTOR3 GetEntityCenterPos() const;
	bool IsCharDropZonePos();

public:
	void SendCurrentPositionToAllUser( const ioHashString rkName = "" );
	void SendControlInfoToUser( const ioHashString &rkName );

protected:
	void FillControlInfo( SP2Packet &rkPacket );

public:
	void SetWeaponIndexBase( DWORD dwBase );
	DWORD IncreaseWeaponIndexBase();
	DWORD GetWeaponIndexBase() const;

	void ApplyCharControl( SP2Packet &rkPacket );
	void ApplyNormalAttack( SP2Packet &rkPacket );
	void ApplyDashAttack( SP2Packet &rkPacket );
	void ApplyJumpState( SP2Packet &rkPacket );
	void ApplyCrossBowJumpState( SP2Packet &rkPacket );
	void ApplyFlyJumpState( SP2Packet &rkPacket );
	void ApplyBoostJumpState( SP2Packet &rkPacket );
	void ApplyDoubleJump( SP2Packet &rkPacket );
	void ApplyBuffJump( SP2Packet &rkPacket );
	void ApplyBuffFlyJump( SP2Packet &rkPacket );
	void ApplyBuffMultiFlyJump( SP2Packet &rkPacket );
	void ApplyExtendJump( SP2Packet &rkPacket );
	void ApplyChangeDashRun( SP2Packet &rkPacket );
	void ApplyJumpDash( SP2Packet &rkPacket );
	void ApplyCurseRolling( SP2Packet &rkPacket );
	void ApplyExtendDash( SP2Packet &rkPacket );
	void ApplyBlowDash( SP2Packet &rkPacket );
	void ApplyCounterAttack( SP2Packet &rkPacket );

	void GetCharInfoInNetwork( SP2Packet &rkPacket );
	void ApplyCharInfoInNetwork( SP2Packet &rkPacket );

	bool ApplyItemGauge( SP2Packet &rkPacket );
	void ApplySkillExtraInfo( ioHashString szSkillName, SP2Packet &rkPacket );
	void ApplyPassiveSkillExtraInfo( ioHashString szSkillName, SP2Packet &rkPacket );

	int GetAnimationEventCnt( const ioHashString &szAnimation,
							  const ioHashString &szEventType ) const;

	DWORD GetAnimationEndTime();

	void SetSkinColor( const D3DCOLORVALUE &rkSkin, const D3DCOLORVALUE &rkOutLine );
	void SetHairColor( const ioHashString &szHairColorTex, const D3DCOLORVALUE &rkOutLine );
	void SetBeardColor( const ioHashString &szBeardColorTex, const D3DCOLORVALUE &rkOutLine );

	void ChangeSkinColor();

public:
	void SetCurItemSkillGauge( int iSkillNum, float fGauge );
	void SetCurItemSkillGauge( const ioHashString &szName, float fGauge );

	float ReduceNeedGaugeBySkill( ioSkill *pSkill, float fUseGauge );
	float ReduceCurSkillGaugeBySkill( ioItem *pItem, float fUseGauge );
	float ReduceCurSkillGaugeBySkill( const ioHashString &szName, float fUseGauge );

	float GetCurItemSkillGauge( ioItem *pItem = NULL );	
	float GetCurItemSkillGauge( const ioHashString &szName );

public:
	void SetLastAttackerName( const ioHashString &rkName, const ioHashString &rkSkillName );
	void SetDieLastAttackerInfo( const ioHashString &rkName, const ioHashString &rkSkillName, const TeamType eTeam, const DWORD dwLastItem );          //죽을 때 서버로부터 받은 최종 정보. 죽고나서 처리할 때 사용
	void CheckLastAttackerTime( bool bCallProcess = true );

	void SetLastApplyWeapon( int iIndex, DWORD dwTime );

	RaceDetailType GetRaceDetailType() const;

	inline RaceType GetRaceType() const { return m_RaceType; }
	inline const ioHashString& GetAutoTargetName() const { return m_AutoTargetName; }
	inline const ioHashString& GetLastAttackerName() const { return m_LastAttackerName; }
	inline const ioHashString& GetLastAttackerSkillName() const { return m_LastAttackerSkillName; }
	inline const TeamType GetLastAttackerTeam() const { return m_LastAttackerTeam; }
	inline const DWORD GetLastAttackerItemCode() const { return m_dwLastAttackerItemCode; }
	inline const Stat& GetCurrentStat() const { return m_CurrentStat; }
	inline bool IsMale() const { return m_bMale; }

	inline const CHARACTER& GetCharacterInfo() const { return m_CharInfo; }

public:
	bool IsDefenseKeyPress() const;
	bool IsDefenseKeyDown() const;
	bool IsAttackKeyDown() const;
	bool IsAttackKeyPress() const;
	bool IsAttackKey() const;
	bool IsJumpKey() const;
	bool IsJumpKeyDown() const;
	bool IsJumpKeyPress() const;
	bool IsDefenseKey() const;
	bool IsSpaceKeyPress() const;
	
	bool CheckViewChange() const;
	bool CheckSpecialViewChange() const;
	bool CheckMovieModeChange() const;
	bool CheckUseMonsterCoin() const;

	bool IsHelmetSkillKey() const;
	
	bool IsItemCanPickUpByKey();
	bool IsItemCanPickUp();  // true - item pick up enable <-> flase ;
	bool IsMachineTakeEnableState( bool bKey );

	bool IsCanDefenseMove( bool bCheckDefense ) const;
	bool IsCanReloadMove() const;
	bool IsCanAttackMove() const;

	bool IsSettedDirection() const;
	bool IsChangeDirection();
	void ChangeDirection();

	void SetCurDefenseDirKey( ioUserKeyInput::DirKeyInput eDirKey );
	const D3DXVECTOR3& GetDefenseDir();

	ioUserKeyInput::DirKeyInput GetXformDir( ioUserKeyInput::DirKeyInput eDirKey );
	ioUserKeyInput::DirKeyInput GetCurDirKey();
	ioUserKeyInput::DirKeyInput GetDefenseDirKey();
	ioUserKeyInput::DirKeyInput CheckCurDirKey();
	void ClearDirKey();

public:
	float GetCamHeight();
	float GetCamDistance();
	float GetCamFov();
	DWORD GetCamZoomTime();

public:
	void CheckScaleList();
	D3DXVECTOR3& UpdateScale( ioScaleInfo& rkScale );
	void ClearScaleList( bool bAll );
    void RefreshScaleList(int iAniID, DWORD dwStartTime, float fTimeRate);
	
	void UpdateSightTarget(float fTimePerSec);
	void ReleaseExtraRotate();

public: 
	// AreaWeaponControl
	bool CheckAreaWeaponControl();
	const D3DXVECTOR3& GetAreaWeaponPos();
	const D3DXVECTOR3& GetAreaWeaponPrePos();

public:
	bool HasExtendDash();
	bool SetExtendDash();
	void CheckExtendDashLoopEffect();

	ioExtendDash* CreateExtendDash( ioINILoader &rkLoader );
	ioExtendJump* CreateExtendJump( ioINILoader &rkLoader );
	ioCounterAttack* CreateCounterAttack( ioINILoader &rkLoader );

protected:
	bool IsEnableDashState();
	void CheckEnableDashCheck();

public:
	void SetSelectCharArray(int iArray){ m_iSelectCharArray = iArray; }
	int  GetSelectCharArray() const { if( m_pModeParent->GetModeType() == MT_ARENA )  return m_iSelectCharArray - g_MyInfo.GetOriginalCharacterCount(); else return m_iSelectCharArray; }
	int  GetSkillKeyInput();

public:
	void SetAddCharArmorClass( float fAddArmorClass, float fMaxAddArmorClass );
	void UpdateCharArmorClass();
	void SetAddCharSpeedClass( float fAddSpeedClass, float fMaxAddSpeedClass );
	void UpdateCharSpeedClass();

public:
	bool IsForceDropping() { return m_bForceDropping; }
	bool IsJumpState();
	bool IsProtectState();
	bool IsSkillProtected();

	int CheckBlockEmoticonType();

public:
	void SetFlyStateValue( const ioHashString& szBuffName,
						   float fStartJumpPower,
						   float fMoveSpeed,
						   float fFrictionAmt,
						   float fGravityAmt,
						   DWORD dwRoateSpeed,
						   float fOffHeight,
						   DWORD dwOffTime,
						   FlyMoveType eMoveType );

	void SetFloatStateValue( const FloatBuffInfo &rkInfo );

	void GetTargetCharListBySphere( const D3DXVECTOR3 &vPos,
									float fRange,
									float fAngle,
									BaseCharList &vCharList );

	void GetTargetCharListByCylinder( const D3DXVECTOR3 &vPos,
									  float fRange,
									  float fAngle,
									  float fUpHeight,
									  float fUnderHeight,
									  BaseCharList &vCharList );

	void GetNpcTargetCharListByCylinder( const D3DXVECTOR3 &vPos,
							  			 float fRange, float fAngle, float fUpHeight, float fUnderHeight, BaseCharList &vCharList );

	bool IsNpcTargetCharListByCylinder( const D3DXVECTOR3 &vPos,
									    float fRange, float fAngle, float fUpHeight, float fUnderHeight, ioBaseChar *pTarget );

	void GetTargetCharListByBox( const D3DXVECTOR3 &vPos,
		                         float fRange,
								 float fWidth,
								 float fUpHeight,
								 float fUnderHeight,
								 BaseCharList &vCharList );

// For Item Gauge
public:
	bool IsCanUpdateGauge();
	bool ClearEquipItemGauge( int iType );
	bool ClearEquipItemGaugeByPercent( EquipSlot eItemSlot, float fDecraseActiveGaugeRate, float fDecrasePassiveGaugeRate );

protected:
	void UpdateEquipItemGauge();
	void UpdateAccessoryItemGauge();

public:
	float GetMemberBalanceRateForDamage( TeamType eTeam );
	float GetMemberBalanceRateForGauge( TeamType eTeam );
	float GetMemberBalanceRateForBlowTime( TeamType eTeam );
	float GetMemberBalanceRateForStateProtectionTime( int iType );
	float GetMemberBalanceRateForBuffItemDrop();

// For Safety
protected:
	float GetCurSafetyHPRate();
	float GetCurSafetySkillRate();

// For Camera
public:
	int CheckCameraType();
	int CheckCameraType( bool &bCamState );

	// RunDash Camera
	void GetRunDashCamera( float &fDistance, float &fHeight, float &fFov, DWORD &dwZoomTime );

	// Charging Camera
	inline void SetChargingState( bool bState ) { m_bChargingState = bState; }
	bool GetChargingCamera( float &fDistance, float &fHeight, float &fFov, DWORD &dwZoomTime );
	bool GetSkillCamera( float &fDistance, float &fHeight, float &fFov, DWORD &dwZoomTime );

	// Buff Camera
	void SetCameraTypeByBuff( int iType );
	void SetBuffCamera( const ioHashString &szName, int iCamType, float fDistance, float fHeight, float fFov, DWORD dwZoomTime, bool bPriority );
	void GetBuffCamera( float &fDistance, float &fHeight, float &fFov, DWORD &dwZoomTime );

	void SetBuffCameraEnd( const ioHashString &szName );

	inline void SetDelayCameraBuff( bool bEnable ) { m_bSetDelayCameraBuff = bEnable; }

protected:
	void CheckDelayCameraBuff();

// For Etc Sync
public:
	void SetMineInfo( SP2Packet &rkPacket );
	void SetMapAttachInfo( SP2Packet &rkPacket );
	void SetDummyCharInfo( SP2Packet &rkPacket );

// For Flame Buff
public:
	void SetFlameStateValue( DWORD dwMinTime, DWORD dwMaxTime );
	void ResetFlameStateValue();

// For HP
public:
	void SetCurHPRate( float fRate ) { m_fCurHPRate = fRate; }
	float GetCurHPRate() { return m_fCurHPRate; }
	void SetKOHPRecover( float fRate, const ioHashString &szEffect );

// For ManaShield Buff
public:
	float CheckSkillGaugeByDamage( float fDamage );
	void CheckManaShield( bool bEnable );

// For EtcState
public:
	inline int GetCurEtcStateCnt() { return m_iEtcStateCnt; }
	int GetCurModeType();

	void SetDispelBuff( int iGoodBadType, ioBuff *pBuff );

// For Catch
public:
	void SetCatchState( DWORD dwDuration,
						const ioHashString &szTieEffect,
						const ioHashString &szStateEffect,
						const ioHashString &szEscapeEffect,
						const ioHashString &szRope,
						const ioHashString &szCreateChar );

	void ApplyCatchState( DWORD dwDuration,
						  const ioHashString &szTieEffect,
						  const ioHashString &szStateEffect,
						  const ioHashString &szEscapeEffect,
						  const ioHashString &szRope,
						  const ioHashString &szCreateChar );

	void SetEscapeCatchState();
	void ApplyEscapeCatchState( bool bEscapeFail );

protected:
	void CheckEscapeCatchState();
	void CheckCatchState();

public:
	int GetWoundedBoxCount() const;
	void GetWoundedBoxVertices( int iIndex, D3DXVECTOR3 vArray[8] ) const;

public:
	void SendBuffList( const ioHashString &szJoinChar );
	void ReceiveBuffList( SP2Packet &rkPacket );

	void SendItemGauge( const ioHashString &szJoinChar );
	void ReceiveItemGauge( SP2Packet &rkPacket );

	void SetThrowSkillEnd();
	void SetWhipSkillEnd();

	bool IsCanChangeCharState( int iCharArray, bool bInputCheck = false, bool bRequestBankrupt = false, bool bSystemMsg = true );
	bool IsCurUsingSkill( ioHashString szSkillName );
	bool IsCanMiniMapVisible();
	bool IsCanChangeTeamInMiniMap();
	void SetChangeMotion( bool bEnable, const ioHashString& szDelayAni, const ioHashString &szMoveAni );
	void SetChangeJumpMotion( bool bEnable,
							  const ioHashString& szReadyAni,
							  const ioHashString& szJumpAni,
							  const ioHashString& szEndAni,
							  float fEndAniRate );
	
	void SetEnableHide( bool bEnable, ioHashString szDelayAni = "", ioHashString szMoveAni = "" );
	bool IsCanHide();
	bool IsCanLimitMove( const ioHashString &szBuffName );

	void SetNoWoundState( bool bNoWound );
	bool IsCanNoWound();

	void SetInvisibleState( bool bInvisible, bool bNow=false, bool bINvisiblePass = false );
	bool IsCanInvisible();

	void SetEnableChangeTeam( bool bEnable );
	bool IsCanChangeTeam();
	bool IsCanChangeTeamEx();

	void SetJumpPowerRateByBuff( float fRate );
	void SetLandingRateByBuff( float fRate, float fExtendRate );

	void SetBuffJumpValue( const CharBuffJumpInfo &rkJumpInfo );
	void RemoveBuffJumpValue( const ioHashString &szName );

	void SetBuffFlyJumpValue( const CharBuffJumpInfo &rkJumpInfo );
	void RemoveBuffFlyJumpValue( const ioHashString &szName );

	void SetBuffMultiFlyJumpValue( const CharBuffJumpInfo &rkJumpInfo );
	void RemoveBuffMultiFlyJumpValue( const ioHashString &szName );

	const ioHashString& GetBuffFlyJumpName() const { return m_vBuffFlyJumpInfoList[0].m_Name; }
	const ioHashString& GetBuffJumpName() const { return m_vBuffJumpInfoList[0].m_Name; }

	void SetCloseOrderAni( const ioHashString &szCloseOrderMove,
						   float fAniRate, float fBackAngle,
						   DWORD dwStartTime, DWORD dwMinTime, DWORD dwMaxTime,
						   bool bOnlyMove,
						   int iType );

	void SetCloseOrderDir( float fCurAngle, bool bSet );

	void SetFrictionBuffInfo( const ioHashString &szName, float fRate );
	void RemoveFrictionBuffInfo( const ioHashString &szName );
	float GetBuffFrictionRate();

	void SetGravityBuffInfo( const ioHashString &szName, float fRate );
	void RemoveGravityBuffInfo( const ioHashString &szName );
	float GetBuffGravityRate();

	void SetFrontBackWoundInfo( const FrontBackWoundInfo &rkInfo );
	void RemoveFrontBackWoundInfo( const ioHashString &szName );
	bool GetFrontBackWoundInfo( OUT FrontBackWoundInfo &rkInfo );

	void SetDamageDefenseBuffInfo( const BuffValueInfo &rkInfo );
	void RemoveDamageDefenseBuffInfo( const ioHashString &szName );
	float GetDamageDefenseBuffValue();
	void GetDamageDefenseBuffStringValue( OUT ioHashString &rszValue );
	float GetDamageDefenseBuffValue_To_Forzen();

	void SetLandStunBuff( const ioHashString &szBuffName, const ioHashString &szStunBuff );
	void RemoveLandStunBuff( const ioHashString &szBuffName );
	bool CheckLandStunBuff( ioWeapon *pWeapon );

public:
	inline bool IsSetBoost() const { return m_bSetBoost; }
	inline void SetBoostState( bool bUseBoost ) { m_bSetBoost = bUseBoost; }

	bool IsJumpBoost() const { return m_bJumpBoost; }
	void SetJumpBoostState( bool bUseJumpBoost ) { m_bJumpBoost = bUseJumpBoost; }

	bool IsJumpBoostAttack() const { return m_bJumpBoostAttack; }
	void SetJumpBoostAttackState( bool bUseJumpBoostAttack ) { m_bJumpBoostAttack = bUseJumpBoostAttack; }

	bool IsNoneCheckJumpAttack();

	void CheckJumpBoostBuff();

	bool CheckStandOnGround();

	bool CheckAxisLimitByBuff();

	ioMapEffect* CreateMapEffectBySkill( const ioHashString &szEffectName, const D3DXVECTOR3 &vPos, D3DXVECTOR3 &vScale = D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 ) );
	ioMapEffect* CreateMapEffectWithDir( const ioHashString &szEffectName, const D3DXVECTOR3 &vDir );
	ioMapEffect* CreateMapEffectAttachChar( const ioHashString &szEffectName );
	void UpdateAttachMapEffect( UniqueObjID eObjID, bool bOnlyPos=false );
	void DestroyMapEffect( UniqueObjID eObjID );

public:	
	
	void SetPickEmoticon();
	void CheckNamedEmoticon( int iType, bool bUnLimited );

	void SetKillEmoticon( int iKillCount = 0 );
	void SetDeathEmoticon();

	void SetDisableAttackEmoticonForProtection( ProtectionMode eType );
	void CheckBlockState( AttackBlockType eBlockType, ProtectionMode eProtectionType );
	
	void SetProtectWound( int iEmoticonType );
	void SetCoinEventEmoticon();
	bool CheckAndSetNetworkBadEmoticon( int iServerDelayMS );
	
	void SetNoInputEmoticon( DWORD dwDuration );

	void RenderChatBubble( ioBaseChar *pCameraTarget );


public:
	void SetChampEmoticon( bool bEnd, bool bSound );
	void ResetChampEmoticon();
	void CheckChampEmoticon();	
	void EndMapEffect( UniqueObjID eObjID, bool bGrace = true );

public:
	void ResetRealComboHit( const ioHashString &rkName );

public:
	bool CheckComboSkillHit();
	bool CheckComboAddTime();
	bool CheckSetStateBySkill();
	bool CheckSemiSwitchSkill( int iSkillNum );
	int  GetCurPlayingTeamUserCnt();

public:
	void ShowWeaponItemMesh( bool bShow, bool bBombAlso = false );
	void ShowItemMesh( ioItem *pItem, bool bShow );
	bool ChangeSkillWeaponMesh( ioSkill *pSkill, const ioHashString &szAddEntity, bool bVisibleEquipMesh );
	bool RestoreSkillWeaponMesh( ioSkill *pSkill, const ioHashString &szRemoveEntity, bool bVisibleEquipMesh, bool bRemoveHair=false );

	void ShowEquipItemMesh( bool bShow, int iSlot );
	void ShowHeadMesh( bool bShow );
	void ShowFaceMesh( bool bShow );

	void InvisibleEquipItemMesh( int iSlot, OUT DWORD &dwID );
	void VisibleEquipItemMesh( int iSlot, DWORD dwID );

	void CheckSkillItemHide( bool bShow, int iItemType );

	void SetChangeEquipMesh( const ioHashString& szItemName, bool bChangeEquip );

public:
	// DownState Check
	bool IsApplyDownState( bool bApplyAir=true );

	void CheckAttackCnt( ioBaseChar *pChar );
	void CheckTeamBuffAttackCnt( ioBaseChar *pChar );

	//SkillEvent
	void CheckSkillEvent( int iBuffType );

// For None Play
public:
	void CheckNonePlayTime( bool bClear, bool bPlayerChanged );
	bool IsNonePlayState();

protected:
	void SetNonePlayEmoticon();

// For ChatMode
public:
	void SetChatMode( int iChatMode );
	void SetChatModeReserve( bool bChatReserve ){ m_bChatReserve = bChatReserve; }
	bool IsChatModeReserve(){ return m_bChatReserve; }
	bool IsCanChatModeState();
	
	bool IsChatModeState( bool bForKey ) const;

	void SetChatModeBox();

	float GetChatModeGapTimeRate();

	inline ChatModeState GetChatModeState() const { return m_ChatModeState; }

// For SlotBuff
public:
	bool IsCanSlotBuff();
	bool IsCanSlotRevive();

// For ExperienceMode
public:
	bool  SetExperienceMode( DWORD dwExperienceMode, const ioHashString &rkExperienceID, int iClassType );
	inline const DWORD GetExperienceMode() const { return m_ExperienceMode; }
	void SetOnlyExperienceValue( DWORD dwExperienceMode ){ m_ExperienceMode = dwExperienceMode; }
	void SetNoChangeCharOnExperienceChange( bool bSet ) { m_bNoChangeCharOnExperienceChange = bSet; }
	bool GetNoChangeCharOnExperienceChange() { return m_bNoChangeCharOnExperienceChange; }

// For Fishing
public:
	void GetFishingPos( D3DXVECTOR3& vTargetPos );
	bool EnableFishing( bool bFirst, bool bFishingUIOpen = false );
	void SetFishingState( bool bFirst );
	void SetFishingEndState();

	void OnFishingState( SP2Packet &rkPacket );
	void OnFishingResult( SP2Packet &rkPacket );
	void OnFishingEndState( SP2Packet &rkPacket );
	void OnFishingSell(	SP2Packet &rkPacket );

	void ApplyFishingLoopState( int iRodType, int iBaitType );			// 중간참여자에게 보여주기 위한 처리.

	void InitUseFishGroundOwner() { m_UseFishGroundOwner.Clear(); }
	void CreateFishingGround() { m_bCreateFishingGround = true; }
	void ClearUseFishGroundOwnerInfo();
	void SetUseFishGroundOwnerInfo( ioHashString OwnerName, int iIndex );
	ioHashString& GetUseFishGroundOwner() { return m_UseFishGroundOwner; }
	int GetUseFishGroundIdx() { return m_UseFishGroundIdx; }
	bool CheckUsingFishGroundOwner( ioHashString OwnerName, int iIndex );		// true이면 현재 사용중인 낚시터

	// 낚시터 생성 시 캐릭터 상태 체크 중 하나 
	bool CheckPoweredObjectItem( ioObjectItem* pObjectItem );

protected:
	void CheckFishingState();
	void CheckFishingHookState();

// For Roulette
public:
	bool EnableRoulette();
	void SetRouletteStartState();
	void SetRouletteEndState();
	void OnRouletteState( SP2Packet &rkPacket );
	void SetRouletteEmoticon();

protected:
	void CheckRouletteState();

protected:
	void ProcessChatMode();
	void ProcessExperienceMode();

// For JumpingSkill
public:
	inline bool IsJumpingSkillState() const { return m_bJumpingSkillState; }

	void CheckUseBuffSkill( const ioHashString& szBuffName, bool bAll=true );

// For WereWolf
public:
	bool CheckChangeWereWolfState( int iExtendType );
	void CheckEnableChangeWereWolfGauge();

	void SetWereWolfState( const ioHashString& szAni, float fTimeRate );
	void ReleaseWereWolfState();
	void SetObjectEquipState( const AttackAttribute* pAttribute );
	void SetObjectEquipJumpState( int iAniID, float fTimeRate, DWORD dwStartTime, DWORD dwPreDelay );
	void CheckObjectEquipJump();

	void ApplyWereWolfState( SP2Packet &rkPacket );

	bool IsWereWolfState();

	void IncreaseChangeDamage( float fDamage, int iType, bool bBuff );
	void IncreaseChangeDamageByWeapon( float fDamage );

	void FillWereWolfStateInfo( SP2Packet &rkPacket );
	void SetWereWolfStateInfo( SP2Packet &rkPacket );

public:
// For 서든어택
	bool IsSuddenAttackState();
	void FillSuddenAttackStateInfo( SP2Packet &rkPacket );
	void SetSuddenAttackStateInfo( SP2Packet &rkPacket );

public:
// For 탑블레이트
	bool IsTopBlAttackState();
	void SetTopBlExtendStateInfo( SP2Packet &rkPacket );

//초시공 전사
public:
	bool IsSuperDimensionWeaponIncState();
	void FillSuperDimensionWeaponInfo( SP2Packet &rkPacket );
	void SetSuperDimensionWeaponInfo( SP2Packet &rkPacket );

protected:
	int m_nSuddenMeshType;

public:
	int GetSuddenMeshType(){ return m_nSuddenMeshType; }


// For YuMiRa
public:
	bool IsEquipEnableObject();
	void CheckUseExtraGauge( float fAmt );

	void ApplyGPState( SP2Packet &rkPacket );
	void ReleaseGPState();

	void FillGPStateInfo( SP2Packet &rkPacket );
	void SetGPStateInfo( SP2Packet &rkPacket );

	bool IsGPState();

//for galaxy
public:
	void FillGalaxyInfo( SP2Packet &rkPacket );
	void SetGalaxyInfo( SP2Packet &rkPacket );

//for Alice
public:
	void FillAliceInfo( SP2Packet &rkPacket );
	void SetAliceInfo( SP2Packet &rkPacket );

public:
	ObjectEqipState GetCurrObjectEquipState();
	bool IsObejctEquipState( ObjectEqipState eState = OES_ALL );
	void ReleaseObejctEquipState( ObjectEqipState eState = OES_ALL );
	void ReleaseWeaponItemState();

// Etc
public:
	void StartBuffBlind( const ioHashString &szBlind, const ScreenBlindTime *pTime, int iMaxAlpha );
	void EndBuffBlind();
	void SetBreakFallState( const ioHashString &szName );
	void SetBreakFallLanding( DWORD dwEndTime );
	void SetEtcState( const ioHashString &szName, float fRate );
	void SetMissFireState( const ioHashString &szName, float fRate );
	void SetReverseDirection( bool bReverse );
	void SetSkipStunState( const ioHashString &szBuffName );
	void SetUroborusState( const ioHashString &szAniName,
						   float fAniRate,
						   D3DXVECTOR3 vTargetPos,
						   float fSpeed,
						   float fOffSetAngle,
						   float fMoveRate,
						   float fEndJumpAmt );

	void SetGhostStealerState( const ioHashString &szAniName,
								float fAniRate,
								const D3DXVECTOR3& vTargetPos,
								float fSpeed,
								float fMoveRate,
								float fEndJumpAmt, float fVolumeRate );

	void SetMutantMoveState();

	void SetTitanExtendMoveState( const ioHashString &szAniName,
							float fAniRate,
							D3DXVECTOR3 vTargetPos,
							float fSpeed,
							float fMoveRate,
							bool bJumpLandAttack );

	void SetJeonWooChiExtenDashState( D3DXVECTOR3 vMoveDir, D3DXVECTOR3 vTargetPos, float fMoveSpeed, float fMaxRange );

	void SetAdhesiveState( const ioHashString &szBuffName );
	void CheckAdhesiveStateCol();

	void SetMineTraceBuff( const ioHashString &szBuffName );

	void SetSpecialMotionState();
	void CheckSpecialMotionTime();
	bool IsCanSpecialMotion();
	bool CheckSpecialMotionCamera();

	void SetEtcItemMotionState( int iEtcItemCode );
	void SendEtcItemMotionState( const ioHashString &rkJoinUser );

	void SetCreateStructState( ioEtcItemBlock *pEtcBlock, bool bHeightCheck = true );
	void CheckCreateStructState();
	void ApplyCreateStructState( SP2Packet &rkPacket );

	void SetRandomSeed( DWORD dwSeed );
	
	bool IsEnableStealHP();
	bool IsEnableAttackCancelState();

	void InitAttackDamageInfoList();
	void InitWoundDamageInfoList();

	void AddAttackDamageInfoList( float fDamage, int iAttackType );
	void AddWoundDamageInfoList( float fDamage, int iAttackType );

	const DamageInfoList& GetAttackDamageInfoList() const;
	const DamageInfoList& GetWoundDamageInfoList() const;

	void CheckDamageInfo(bool bStart );

	inline DWORD GetRandomSeed() const { return m_dwRandomSeed; }

	void CheckWoundPassiveSkill( ioBaseChar *pAttacker );
	int GetReflectInfoIndex( AttackSide iAttackSide );
	DWORD GetEventGapTime();

	ioHashString GetVoiceSoundName( const ioHashString& szName );
	std::string  GetMotionSoundName( const ioHashString& szName );
	std::string  GetWoundedSoundName( const ioHashString& szName );
	std::string  GetDefensedSoundName( const ioHashString& szName );

public:
	bool IsLeftKeyRelease() const { return m_KeyInput.IsLeftKeyRelease(); }
	bool IsRightKeyRelease() const { return m_KeyInput.IsRightKeyRelease(); }
	bool IsUpKeyRelease() const { return m_KeyInput.IsUpKeyRelease(); }
	bool IsDownKeyRelease() const { return m_KeyInput.IsDownKeyRelease(); }

	bool IsLeftKeyDown() const { return m_KeyInput.IsLeftKeyDown(); }
	bool IsRightKeyDown() const { return m_KeyInput.IsRightKeyDown(); }
	bool IsUpKeyDown() const { return m_KeyInput.IsUpKeyDown(); }
	bool IsDownKeyDown() const { return m_KeyInput.IsDownKeyDown(); }

	bool IsDefenseKeyRelease() const { return m_KeyInput.IsDefenseKeyRelease(); }
	bool IsJumpKeyRelease() const { return m_KeyInput.IsJumpKeyRelease(); }
	bool IsAttackKeyRelease() const { return m_KeyInput.IsAttackKeyRelease(); }

	bool IsDirKeyDoubleClick() const { return m_KeyInput.IsDirDoubleClick(); }

	bool IsDirKeyRelease( ioUserKeyInput::DirKeyInput eCurDir ) { return m_KeyInput.IsDirKeyRelease(eCurDir); }

	void ClearDirDoubleClick();

	ioUserKeyInput::ActionKeyInput GetCurActionKey() const { return m_KeyInput.GetCurActionKey(); }
	ioUserKeyInput::DirMixInput GetCurDirMixKey() const { return m_KeyInput.GetCurDirMixKey(); }

	ioUserKeyInput::DirKeyInput GetDirKey() const { return m_KeyInput.GetCurDirKey(); }
	ioUserKeyInput::DirKeyInput GetPreDirKey() const { return m_KeyInput.GetPreDirKey(); }
	ioUserKeyInput::DirKeyInput GetFourDirKey() const { return m_KeyInput.GetCurFourDirKey(); }

	int	GetSkillInput(){ return m_KeyInput.GetSkillInput(); }

	void SetNetworkKeyInput( ioUserKeyInput::DirKeyInput eKey );

public:
	void AddRopeSpring( const ioHashString &rkSpring, const RopeSpringParam *pParam );
	void RemoveRopeSpring( const ioHashString &rkSpring );

public:
	bool IsVisibleChar();

public:
	int GetGrowthInfoNumByType( int iType, int &iGrowthNum ) const;
	int GetGrowthInstantLevel();

	ioGrowthUpInfo* GetGrowthUpInfoForChar( int iType );
	ioGrowthUpInfo* GetGrowthUpInfoForItem( int iType );
	ioGrowthUpInfo* GetGrowthUpInfoForItem( int iType, const ioHashString &szCharName, const ioHashString &szItemName );

public:
	static bool IsOnlyMyEmoticon(){ return m_bOnlyMyEmoticon; }


protected:
	void SetEnlargementScale( OUT ioHashString &rszPrevSkeleton );

public:
	// For DashAttack
	void SetDashAttackEndJumpInfo( int iAniID, float fTimeRate, DWORD dwPreDelay );

public:
	void InitExtendAttackTagInfo();
	void SetExtendAttackEndJumpInfo( int iAniID, float fTimeRate, DWORD dwPreDelay );
	void SetBaseAttackExtraAniJump( int iAniID, float fTimeRate, DWORD dwPreDelay );
	void CheckExtraAniJump();
	bool CheckExtendAttackEndJump();
	void SetExtendAttackEndJump( float fJumpAmt, float fLandRate, bool bInitRate=false, bool SendNet=true, bool bMustJump = false, bool bExtendJumpAni = false );

	bool IsCanRotateJumpAttack();

	float CheckKeyReserveTime( int iAniID, float fTimeRate, DWORD dwPredelay=0 );

public:
	void SetSkillProtection();
	void RestoreSkillProtection();
	void CheckSkillProtection();

	void SetCurseRollingInfo( const ioHashString &szRollingAni, float fAniRate, const ForceInfoList &vForceInfoList );

	void SetDownRollingState( const ioHashString &szRollingAniName, float fRollingAniRate,
							  const ioHashString &szGetUpAniName, float fGetUpAniRate,
							  const ForceInfoList &vForceInfoList, bool bMoveDirOnly = false );
	void ApplyDownRollingState( SP2Packet &rkPacket );

	bool CheckBuffAirAttackEnable();
	void SetBuffAirAttackInfo( const AttackAttribute &rkAttr );
	void SetBuffAirAttackState( float fAirAttackGravity );
	void ApplyBuffAirAttackState( SP2Packet &rkPacket );

	bool CheckTargetAllWoundedState();

	void CheckRunChargeCancel( bool bCancel );

	void SetMachineTakeWait( ioMachineStruct *pMachine );
	void SetMachineTake( ioMachineStruct *pMachine );
	void SetMachineRelease( ioMachineStruct *pMachine, const D3DXVECTOR3 &vPos );
	void ApplyMachineTake( SP2Packet &rkPacket );
	void ApplyMachineRelease( SP2Packet &rkPacket );

	void ChangeEquipedSkill( int iSkillSlot, ioItem *pItem );
	void ApplyCurSkillName( int iSkillSlot, const ioHashString &szName );
	
// For ExtraItem
	bool SetExtraItemChangeState( bool bNet );

	void CheckDefaultReinforce();
	int GetDefaultReinforce( int iSlot );

	void SetLimitActionBuff( bool bAttack, bool bDefense, bool bJump, bool bExtendJump, bool bSkill, bool bDash );
	void RemoveLimitActionBuff( const ioHashString &szBuffName );

	void SetExtraGaugeTypeByWeapon( int iType, int iWGT, float fDamage );

	bool IsBuffLimitAttack() const;
	bool IsBuffLimitJump() const;
	bool IsBuffLimitExtendJump() const;
	bool IsBuffLimitSkill() const;
	bool IsBuffLimitDash() const;
	bool IsBuffLimitDefense() const;

	inline bool IsInvisibleHead() const { return m_bInvisibleHead; }
	inline bool IsInvisibleFace() const { return m_bInvisibleFace; }

// Grappling
public:
	void SetGrapplingType( GrapplingType eType );

	void SetAttackGrapplingState( const ioHashString &szTarget );

	void SetWoundGrapplingState( const ioHashString &szAttacker, DWORD dwCode, GrapplingWoundedInfo kWoundedInfo );
	void SetGrapplingPushedState( GrapplingPushedInfo kPushedInfo );
	void SetGrapplingPushedBuffState( GrapplingPushedInfo kPushedInfo, const ioHashString &szAttacker );

	void SetAttackSkillGrapplingState( const ioHashString &szTarget );
	void SetAttackItemGrapplingState( const ioHashString &szTarget );
	void SetSkillGrapplingState( const ioHashString &szAttacker, GrapplingSkillWoundedInfo kWoundedInfo, const ioHashString &szSkillName );
	void SetGrapplingWoundSwing( const ioHashString &szAttacker, GrapplingSkillWoundedInfo kWoundedInfo,
								 const ioHashString &szWoundAni, float fWoundAniRate );
	void SetNoChangeGrapplingState( int iType );

	void SetJumpGrapplingState( const ioHashString &szTarget );
	void SetJumpGrapplingWoundState( const ioHashString &szAttacker, DWORD dwCode, JumpGrappingWoundInfo kWoundedInfo );

	//루시퍼
	bool SetItemGrapplingState( const ioHashString &szAttacker, GrapplingSkillWoundedInfo kWoundedInfo );

	void ApplyAttackGrapplingState( SP2Packet &rkPacket );
	void ApplyWoundGrapplingState( SP2Packet &rkPacket );
	void ApplyPushedGrapplingState( SP2Packet &rkPacket );
	void ApplyPushedBuffGrapplingState( SP2Packet &rkPacket );
	void ApplySkillGrapplingState( SP2Packet &rkPacket );
	void ApplyGrapplingWoundSwingState( SP2Packet &rkPacket );

	void ApplyJumpGrapplingState( SP2Packet &rkPacket );
	void ApplyJumpGrapplingWoundState( SP2Packet &rkPacket );

	void ChangeWoundGrapplingMoveState();
	void ChangeWoundGrapplingCancelState();

	inline const ioHashString GetGrapplingAttacker() const { return m_GrapplingAttacker; }
	inline const ioHashString GetGrapplingTarget() const { return m_GrapplingTarget; }
	inline GrapplingState GetWoundGrapplingState() const { return m_WoundGrapplingState; }
	void SetWoundGrapplingState( GrapplingState dwState );
	inline GrapplingPushedState GetPushedGrapplingState() const { return m_GrapplingPushedState; }

	inline const ioHashStringVec& GetCollisionGrapplingBufList() const { return m_PushedGrapplingInfo.m_GrapplingColBuffList; }
	inline const ioHashStringVec& GetCollisionSwingGrapplingBufList() const { return m_SkillGrapplingInfo.m_SwingWoundBuff; }

protected:
	void CheckGrapplingState();
	void CheckGrapplingWoundState();
	void CheckGrapplingPushedState();
	void CheckGrapplingPushedBuffState();
	void CheckGrapplingSkillState();
	void CheckGrapplingItemState();

	void CheckJumpGrapplingState();
	void CheckJumpGrapplingWoundState();

	void ProcessWoundGrapplingMoveState();
	void ProcessWoundGrapplingAttackState();
	void ProcessWoundGrapplingCancelState();

	void ProcessGrapplingGoState();

	void ProcessSkillWoundGrapplingLoop();
	void ProcessSkillWoundGrapplingNoChange();
	void ProcessSkillWoundGrapplingMulti();
	void ProcessSkillWoundGrapplingCompete();

public:
	//루시퍼
	void ProcessItemWoundGrapplingLoop( ioBaseChar *pAttacker );


protected:
	void CheckGrapplingPushedKey();
	void CheckGrapplingPushedDir();

	void FillGrapplingPushedInfo( SP2Packet &rkPacket );
	void ApplyGrapplingPushedInfo( SP2Packet &rkPacket, GrapplingPushedInfo &rkPushedInfo );

	void FillGrapplingSkillWoundedInfo( SP2Packet &rkPacket );
	void ApplyGrapplingSkillWoundedInfo( SP2Packet &rkPacket, GrapplingSkillWoundedInfo &rkWoundedInfo );

public:
	bool CheckGrapplingExceptionState();

	D3DXVECTOR3 GetGrapplingGapPosition();
	D3DXVECTOR3 GetGrapplingSkillGapPosition();
	D3DXVECTOR3 GetGrapplingSkillMultiGapPosition( ioBaseChar *pChar );

	//루시퍼
	D3DXVECTOR3 GetGrapplingItemGapPosition();

	bool CheckGrapplingPushedCollision();
	bool CheckGrapplingPushedBuffCollision();
	bool CheckGrapplingSkillCollision();
	bool CheckGrapplingSwingCollision();

	void CheckWoundGrapplingBuff( ioHashString szColChar );
	void CheckWoundGrapplingPushedBuff();
	void CheckCollisionGrapplingBuff( const ioHashString &szAttacker, const ioHashStringVec &rkBuffList );
	void CheckCollisionGrapplingPushedBuff( const ioHashString &szAttacker, const ioHashStringVec &rkBuffList );

	void CheckSkillGrapplingCancelBuff( ioHashString szColChar, bool bCol=true );
	void CheckCollisionSwingGrapplingBuff( const ioHashString &szAttacker, const ioHashStringVec &rkBuffList );

	void CheckJumpGrapplingCancelBuff( ioHashString szColChar, bool bCol=true );

	void GetGrapplingMoveInfo( OUT int &iCurDir, OUT bool &bSetMotion );

// psyc grapping
public:
	void SetPsycGrappling( const ioHashString &szTarget );
	void SetWoundPsycGrappling( const ioHashString &szAttacker,
								const ioHashString &szSkillName,
								PsycGrapplingWoundedInfo kWoundInfo );

	void SetPsycGrapplingAction( const ioHashString &szAttacker,
								 const ioHashString &szSkillName,
								 PsycGrapplingWoundedInfo kWoundInfo,
								 int iKeyType );

	void ApplyWoundPsycGrappling( SP2Packet &rkPacket );
	void ApplyWoundPsycGrapplingAction( SP2Packet &rkPacket );

	inline const ioHashString GetPsycGrapplingAttacker() const { return m_PsycGrapplingAttacker; }

// WallCollisionBuff
	void CheckWallCollisionBuff();

protected:
	void CheckWoundPsycGrappling();

// etc grappling
public:
	void SetGrapplingBlowWound();

// Warp Target List
public:
	void SetWarpTargetList( const ioHashString &szAttacker, const ioHashString &szCharName );

// etc
public:
	void SetSkillCreateWeaponDependency( const ioHashString &szSkillName );

// DummyChar
public:
	void CheckDummyCharCreate( int iIndex );
	void CheckDummyCharDestroy( int iIndex );

	bool CheckDummyCharControl();
	D3DXVECTOR3 GetDummyCharControlPos();

	bool CheckWeaponCharControl();
	D3DXVECTOR3 GetWeaponCharControlPos();

	bool CheckTargetTeleport();
	D3DXVECTOR3 GetTargetTeleportPos();

	bool CheckPossessionChar();
	D3DXVECTOR3 GetPossessionPos();

public:
//Destroyer fly
	bool SetDestroyerFlyState( const DestroyerFlyInfo &rkFlyInfo );
	bool SetDestroyerFlyStateSkill( const ioHashString &szBuffName, const DestroyerFlyInfo &rkFlyInfo );
	void CheckEnableDestroyerGauge();
	bool IsEnableDestroyerFly();

//싸이보그
	bool SetCyvogueModeState( const CyvogueFlyInfo &rkFlyInfo );
	bool SetCyvogueModeStateSkill( const CyvogueFlyInfo &rkFlyInfo );
	void CheckEnableCyvogueGauge();
	bool IsEnableCyvogueModeState();

//악령도둑
	bool SetGhostStealerState( const GhostStealerInfo &rkInfo );

//트리니티
	bool SetTrinityState( const TrinityInfo &rkInfo );
	bool IsEnableTrinityState();

//루시퍼 날기
	bool SetLuciferFlyState( const LuciferFlyInfo &rkFlyInfo );
	bool IsEnableLuciferFly();

//닌자(진화)
	bool SetNinjaExpansionMove( bool bJump );

//드라큘라 변신
	bool SetDraculaFlyState( const DraculaFlyInfo &rkInfo );
	bool IsEnableDraculaFly();

	bool SetNightmareFlyState();
	bool IsEnableNightmareFly();

	bool IsEnableAzazelAttack();
	bool IsEnableAzazelSpecialAttack();

//하쿠멘 방어
	bool SetHakumenDefenseState( const HakumenDefenseInfo &rkDefenseInfo );
	bool IsEnableHakumenDefense();

//피터팬 날기
	bool SetPeterpanFlyState( const PeterpanFlyInfo &rkFlyInfo );
	bool IsEnablePeterpanFly();

//데빌 날기
	bool SetDevilFlyState( const DevilFlyInfo &rkFlyInfo );
	bool IsEnableDevilFly();
	
// charge fly
	void SetChargeFlyState();
	void CheckChargeFlyState();

// witch fly
	bool SetWitchFlyStateSkill( const ioHashString &szBuffName, const WitchFlyInfo &rkWitchFlyInfo );
	void CheckEnableWitchGauge();
	void ApplyWitchState( SP2Packet &rkPacket );

	// nakoruru
	void CheckEnableNakoruruGauge();
	void ApplyNakoruruState( SP2Packet &rkPacket );

// mechanics fly
	bool SetMechanicsFlyState( const MechanicsFlyInfo &rkFlyInfo );
	bool SetMechanicsFlyStateSkill( const ioHashString &szBuffName, const MechanicsFlyInfo &rkFlyInfo );
	void CheckEnableMechanicsGauge();
	bool IsEnableMechanicsFly();

// michael fly
	bool SetMichaelFlyStateByJumpDash( const MichaelFlyInfo &rkFlyInfo );
	void CheckEnableMichaelGauge();
	bool IsEnableMichaelFly();

// goomiho fly
	bool SetGoomihoFlyStateByJumpDash( const GoomihoFlyInfo &rkFlyInfo );
	void CheckEnableGoomihoGauge();
	bool IsEnableGoomihoFly();
	
	void SetRunRotateChargeState();

	// evy blank
	bool SetEvyBlinkStateByJumpDash( const EvyBlinkInfo &rkInfo );

	// 서큐버스
	bool SetSuccubusStateByDash( const SuccubusAttackInfo &rkInfo );
	void CheckEnableSuccubusGauge();
	bool IsEnableSuccubusAttack();
	
	//Flash
	bool SetFlashSpecialState( bool bDashCharge );
	bool IsEnableFlashSpecial();
	
	//Slasher
	bool SetSlasherSpecialState( const SlasherRunInfo& RunInfo );
	bool IsEnableSlasherSpecial();
				
	//썬더버드
	void SetThunderBirdObjectSpecialState( bool bInit = false );
	bool IsEnableThunderBirdObjectSpecialState();
	void CheckThunderBirdSpecialBuff( bool bTeamAttacked );
	void FillThunderBirdInfo( SP2Packet &rkPacket );
	void SetThunderBirdInfo( SP2Packet &rkPacket );

	//Valkyrie
	void ClearValkyrieDobuleJumpFly();

	//박일표
	void SetParkilpyoEquipObject( int nState );
	bool IsEnableParkilpyoEquipObject();
	void FillParkilpyoInfo( SP2Packet &rkPacket );
	void SetParkilpyoInfo( SP2Packet &rkPacket );
	
	//쉐도우레인저
	bool SetShadowRangerSpecialMoveState( float fUseGauge );
	bool IsEnableShadowRangerSpecial( float fUseGauge );

	//나타
	bool SetNataSpecialComboState( bool bNormal, bool bDash, bool bJump );
	bool IsEnableNataSpecialCombo( bool bNormal, bool bDash, bool bJump );
	bool IsEnableNataSpecialFly();
	
	//귀무사
	void SetOnimushaEquipObject( int nState );
	bool IsEnableOnimushaEquipObject();
	void FillOnimushaInfo( SP2Packet &rkPacket );
	void SetOnimushaInfo( SP2Packet &rkPacket );


	//드루이드
	void FillDruidInfo( SP2Packet &rkPacket );
	void SetDruidInfo( SP2Packet &rkPacket );

	//미드나잇
	bool SetMidnightStateByDash( const MidnightAttackInfo& rkInfo );
	bool IsEnableMidnightAttack();

	void FillMidnightScytheInfo( SP2Packet& rkPacket );
	void SetMidnightScytheInfo( SP2Packet& rkPacket );
	void FillMidnightGunInfo( SP2Packet& rkPacket );
	void SetMidnightGunInfo( SP2Packet& rkPacket );

	// 이브 2020-11-24
	void FillEvaSubWeaponInfo( SP2Packet& rkPacket );
	void SetEvaSubWeaponInfoList( SP2Packet& rkPacket );

	// 마그네틱 2020-12-16
	void FillMagneticWeaponInfo( SP2Packet& rkPacket );
	void SetMagneticWeaponInfoList( SP2Packet& rkPacket );

// phantom fly
	bool SetPhantomFlyStateAttack();
	bool SetPhantomFlyState( const PhantomFlyInfo &rkFlyInfo );
	void CheckEnablePhantomGauge();
	bool IsEnablePhantomFly();
		
	void CheckEnablePossessionState( const ioHashString &szTargetName );

	void SetPossessionChar( const ioHashString &szPossession );
	void ClearPossessionChar();
	void CheckPossessionKey();
	ioUserKeyInput::DirKeyInput GetPossessionDirKey();

	const ioHashString& GetPossessionChar() const { return m_PossessionName; }

//MegaAgent
	bool SetMegaAgentFlyState();
	bool IsEnableMegaAgentFly();

	void ChangeToSmileJokerFlyState();

// default item
	void CallItemProcess();
	bool AddCreateObjectItemInfo( int iCreateObjectIndex );

// specialatt effect
	void CheckSpecialAttEffect();

// panic move
public:
	void SetPanicMoveInfo( const FloatVec &rkAngleList,
						   const ioHashString &szMoveAni,
						   const ioHashString &szCatchMoveAni,
						   DWORD dwBaseSeed,
						   DWORD dwMinTime, DWORD dwMaxTime,
						   float fMoveSpeed );

protected:
	void CheckNextPanicMove();
	void CheckPanicMoveState();

// sylphid
protected:
	void CheckSylphid( bool bDefense );
	void CheckSylphidByRachelItem( bool bDefense );
	void CheckSylphidByWereWolfObjItem();

// Iljimae
public:
	void SetReturnMark();

protected:
	void CheckReturnMark();
	void CheckIljimaeTarget();

public:
	void ApplyCharActionInfo( SP2Packet &rkPacket );

	// sylphid
	void SetSylphidByRachelItem( bool bDefense );
	void SetSylphidByWereWolfObjItem();
	bool CheckObjectSylphid( bool bCheckBullet=true );
	void ClearSylphid();

	// summon dummychar
	void CheckInitSummonDummyChar( int iSubState );

	bool EnableSummonDummyChar( int iSubState );
	bool CheckSummonDummyChar( int iSubState );

	void ApplySummonDummyChar( SP2Packet &rkPacket );

	// weapon action check
	bool CheckGraffitiWeaponAction();

// input cancel
public:
	void ClearInputCancelInfo();
	void SetInputCancelInfo( ioHashString &szAnimation, float fAniRate );

protected:
	void CheckCancelReserve();
	bool ProcessCancel();

// ani rotate
public:
	void SetAniRotate( int iAniID, float fTimeRate );
	void CheckAniRotate();

// ani gravity
protected:
	DWORD m_dwExtraAniGravityRateTime;
	int   m_iCurExtraAniGravityRateArr;
	float m_fExtraAniGravityRate;
	vExtraAniGravityRateList m_vExtraAniGravityRateList;

public:
	inline DWORD GetCharChangeUniqueIndex(){ return m_dwCharChangeIndex; }
	inline void  SetCharChangeUniqueIndex( DWORD dwIndex ){ m_dwCharChangeIndex = dwIndex; }

	/************************************************************************/
	/* USER캐릭터와 NPC캐릭터간에 다른 처리를 해야한다면 아래에 추가하자.   */
	/************************************************************************/
public:
	virtual ControlType GetControlType() const { return CONTROL_USER; }  
	
protected:
	ioBaseChar* FindAutoTarget( AutoTargetType eType );
	virtual void TrackingAutoTarget();

	ioBall* CompareAutoTarget( const ioHashString &szChar, AutoTargetType eType );

protected:
	virtual void CheckDieState();

public:
	ioBall* FindBall();
	ioDummyChar* FindDummyChar( AutoTargetType eType );

protected:
	virtual void InitUserStat();
	virtual void UpdateStatAttribute();

public:
	virtual bool IsNeedProcess() const;
	virtual bool IsNeedSendNetwork() const;
	virtual bool IsDropDamage(){ return true; }         //유저와 개발자K는 드랍데미지를 받고 전투형 NPC는 드랍데미지 없음.
	virtual bool IsItemDrop(){ return true; }

public:
	virtual DWORD GetNoInputDelayTime() { return m_dwNoInputDelayTime; }
	virtual DWORD GetLimiteDieFlyTime() { return m_dwLimiteDieFlyTime; }
	virtual float GetDieActionRate() { return m_fDieActionRate; }
	virtual float GetDieDownActionRate() { return m_fDieDownActionRate; }

public:
	virtual const ioHashString &GetAniStartAction( bool &bWeaponAni );
	virtual const ioHashString &GetAniDieAction();
	virtual const ioHashString &GetAniBackDieAction();
	virtual const ioHashString &GetAniBlowDieAction();
	virtual const ioHashString &GetAniBackBlowDieAction();
	virtual const ioHashString &GetAniDownBlowDieAction();
	virtual const ioHashString &GetAniBackdownBlowDieAction();
	virtual ioHashString &GetAniFalling(){return m_AniFalling;};
	virtual ioHashString &GetDropDownAction(){return m_AniDropAction;}
	virtual ioHashString &GetAniBlowGetUp(){return m_AniBlowGetUp;}
	virtual ioHashString &GetAniBackBlowGetUp(){return m_AniBackBlowGetUp;}

public:
	virtual DWORD GetStartProtectionTime(){ return m_dwStartProtectionTime; }
	virtual DWORD GetBlowStateProtectionTime(){ return m_dwBlowProtectionTime; }

public:
	virtual int GetDelayAnimationIdx() const;
	virtual int GetRunAnimationIdx() const;
	virtual int GetDashRunAnimationIdx() const;
	virtual int GetDefenseAnimationIdx() const;
	virtual int GetWoundedAnimationIdx( AttackSide eAttackSide, BlowTargetType eBlowTargetType ) const;

	virtual int GetJumpReadyAnimationIdx() const;
	virtual int GetJumppingAnimationIdx( bool bExtend=false ) const;
	virtual int GetJumpEndAnimationIdx();
	inline float GetJumpEndAnimationRate() const { return m_fCurJumpLandingRate; }

	virtual int   GetWeaponItemAniIdx( AttackType eType, int iSubIdx = 0 ) const;
	virtual int   GetWeaponItemEndAniIdx( AttackType eType, int iSubIdx = 0 ) const;
	const AttackAttribute* GetAttackAttribute( AttackType eType, int iSubIdx = 0 ) const;
	float GetAttackAniRate( AttackType eType, int iSubIdx = 0 ) const;
	float GetAttackEndAniRate( AttackType eType, int iSubIdx = 0 ) const;
	DWORD GetAttackPreDelay( AttackType eType, int iSubIdx = 0 ) const;
	DWORD GetAttackAnimationDelay( AttackType eType, int iSubIdx = 0 ) const;

	int	  GetCurExtraDashAttackCombo() { return m_iCurExtraDashAttack; }

public:
	void GetDamagedTopChar( ioHashString &rkReturnName );
	void GetDamagedLowestChar( ioHashString &rkReturnName );
	void GetModeCondition( ioHashString &rkReturnName );
	int  GetAllDamaged( const ioHashString &szFindName );


	bool IsEnableTeamAttack();
	bool IsEnableTeamDamage();

public:
	bool IsSystemActivityState();
	bool IsSystemState();
	bool IsEnableTarget( bool bCheckHide = true, bool bCheckState = true );

public:
	virtual void FaceChange( const char *szTexPostfix, int iDuration );
	void CheckFaceChanging();
	void StopFaceChange();
	const ioHashString &GetNowFaceName(){ return m_szFaceChangeName; }

	bool CheckInvisibleWeaponSkill();

	void ClearAllEquipEffect();
	void RestoreAllEquipEffect();
	void RestroeAllEquipItemCustomize();

// For HeadMeshChange
public:
	virtual void HeadMeshChange( const char *szMeshName, int iDuration );
	virtual void CheckHeadMeshChange();
	virtual void StopHeadMeshChange();

// excavation
protected:
	void CheckExcavatingState();
	void ClearExcavatingState();

//
protected:
	bool IsCanPlayWoundedAniFromExtendDash();

public:
	bool IsExcavating() const { return m_bExcavating; }
	void SetExcavating( bool bExcavating) { m_bExcavating = bExcavating; }

// Maintain Item while Char Change
public:
	int GetMaintainItemCreateIndex() { return m_iMaintainItemCreateIndex; }
	void SetMaintainItemCreateIndex( int iIndex ) { m_iMaintainItemCreateIndex = iIndex; }
public:
	void SetMedalItem( int iItemType, bool bEquip );
	void SetCustomMedalItem( int iItemType, int iCustomIndex, int iStat[8], bool bEquip );
	void GetMedalItemGrowth( OUT int &riGrowthLevel, IN bool bCharGrowth, IN int iMedalItemGrowthArray ) const;
	void ClearMedalItem();
	void ClearCharGrowthLevel( int iClassType );
	void InitMedalItemTypeVec();
	void GetMedalItemType( OUT MedalSyncInfoVec &rvMedalItemType );
	void GetExMedalSlot( OUT IntVec &rvExMedalSlot );
	void SetExMedalSlot( OUT IntVec &rvExMedalSlot );

	inline ioPlayStage* GetCreator() { return m_pCreator; }

public:
	bool IsMiniHomePageState();

public:
	inline DWORD GetCharKeyReserveEnableTime() const { return m_dwReserveEnableTime; }
	inline bool IsReserveAttack() const { return m_bEnableAttackState; }
	inline bool IsReserveJump() const { return m_bEnableJumpState; }
	inline bool IsReserveDefense() const { return m_bEnableDefenseState; }

	bool IsEnableDefenseByObject();

// Passive Gauge Sync
	void ApplyPassiveGauge( SP2Packet &rkPacket );

// Ghost State
public:
	inline bool CheckGhostState() const { return m_bUseGhostState; }
	inline void SetGhostState( bool bState ){ m_bUseGhostState = bState; }
	float GetPreHeightForGhostState() const { return m_fPreHeightForGhostState; }
	void SetPreHeightForGhostState( float fHeight ) { m_fPreHeightForGhostState = fHeight; }

// RunningMan
public:
	void SetRunningManDecoIndex( DWORD dwIndex, bool bReloadDeco );

// HideBuff
	const bool HasHideBuff();
	void SetEnableDashOnHideBuff( bool bDash ) { m_bEnableDashOnHideBuff = bDash; }
	const bool HasHitCountHoldBuff();

// AffectWoundedValueBuff
	ioBuff *GetAffectWoundedValueBuff();
	float GetGetUpRateOnBuff();

// CheckEscapeSkill
	void ApplyCheckEscapeSkill( SP2Packet &rkPacket );

// Buff Sync
	void ApplyExtraBuffInfo( ioHashString szBuffName, SP2Packet &rkPacket );

// Magnetic Buff
	bool CheckMagneticBuff( D3DXVECTOR3 vPushDir, int iPushState );
	bool IsMagneticBuffOwner();
	int GetMagnet();

// IsBuffProtect
	bool IsBuffProtect();

	float GetSpecialGaugeBuffRate( SpecialGaugeType eType );

// CheckTargetWoundType
	bool CheckTargetWoundType( TargetWoundType eType );

protected:
	bool CheckDontMoveStateOnBuff();

public:
	const bool IsSkipState() const;

public:
	void SetJumpBuffOnDoubleJumpBuffName( ioHashStringVec vBuffList );
	void SetJumpBuffOnDoubleJumpBuffEnd();

protected:
	void ProcessJumpBuffOnDoubleJumpBuff();
	void SetJumpBuffOnDoubleJumpBuff();
	void RemoveJumpBuffOnDoubleJumpBuff();

public:
	const ioHashString& GetConcentrateAttackerName();
	const ioHashString& GetConcentrateTargetName();
	float GetConcentratePushPowerByTarget( ioBaseChar *pTarget );
	float GetConcentrateBlowPowerByTarget( ioBaseChar *pTarget );
	float GetConcentrateTargetDamageRate();
	float GetConcentrateNoTargetDamageRate();

public:
	void SetTargetControlState( int iState );	

public:
	bool CheckExceptionSkillCamera();
	D3DXVECTOR3 GetExceptionSkillCameraPos();
	bool IsFixedExceptionCamera();

public:
	float GetFrontSideAngleCosine(){ return m_fFrontSideAngleCosine; }

protected:
	void CheckReflectDameage( bool bContactType, bool bStructWeapon, ioBaseChar* pAttacker, ioWeapon* pWeapon );

protected:
	void DropShuffleStarItem( ioWeapon* pWeapon );

	//Reflect 이펙트 고정 때문에 추가 (jch)
public:
	void CreateReflectEffect();
	
	//만든 사람과 버프를 확인 jch
public:
	bool IsHasBuffCheckCreator( ioHashString szBuffName, ioHashString szCreator ) const;

	//패시브 스킬 게이지를 사용, 퍼센트 얻기 jch
public:
	void UsePassiveSkillGauge( ioHashString szSkillName, float fUseGauge );
	float GetPassiveSkillGaugeRate( ioHashString szSkillName );
	
public:
	void GetTargetWeaponListBySphere( const D3DXVECTOR3 &vPos,
		float fRange,
		float fAngle,
		TargetWeaponList &vWeaponList );

	void GetTargetDummyListBySphere( const D3DXVECTOR3 &vPos,
		float fRange,
		float fAngle,
		TargetDummyList &vDummyList );
		
public:
	void SetSelfControlInfo( const ioHashString &szMoveAni );
	
public:
	void ApplyEquipSlotSkillExtraInfo( ioHashString szSkillName, SP2Packet &rkPacket );

public:
	void AttackComboInit();
	bool CheckKeepComboAttack();
	bool CheckKeepComboAttack( DWORD dwStart, DWORD dwDuration, DWORD dwDelay );

public:
	void SetConveyerDisable( bool bDisable ) { m_bDisableConveyer = bDisable; }
	bool GetConveyerDisable() { return m_bDisableConveyer; }

public:
	void SetExcavationPos( D3DXVECTOR3 vPos );
	void CreateExcavationGuide();
	void DestroyExcavationGuide();
	void CreateExcavationPoint();
	void DestroyExcavationPoint();
	void ProcessExcavationEffect();

	DWORD GetExcavationGuideID()		{ return m_dwExcavationGuideID; }

public:
	void SetViewName( const ioHashString &szViewName ){ m_ViewName = szViewName; }

	//펫 관련
public:
	//캐릭터 하이드 상태 확인
	bool IsHideState();
	//캐릭터 알파값 확인
	int GetAlphaRate();
	//펫 능력치 적용
	void GetPetGrowth( int &nGrowthLevel, bool bCharGrowth, int nGrowthArray ) const;

protected:
	//캐릭터 상태에 따라 착용 펫의 상태 변경
	void SetPetState( int eCharState );

public:
	void SetJumppingLimitCnt( int n );

	bool IsEnableExtendJump();
	void NotifyExtendJumpUse();

	void ClearAniEvent();

public:
	void GetAwakeCharGrowth( OUT int &riGrowthLevel, IN int iGrowthArray ) const;
	void GetAwakeItemGrowth( OUT int &riGrowthLevel, IN int iGrowthArray ) const;
	int GetAwakeType();
	void ChangeAwake( int iAwakeType );
	void ChangeAwakeDate( int iLimitDate );
	void ChangeAwakeClear();
	void SetAwakeChangeEffect();

	void ChangePowerUpCharGrade( int iCharPowerUpGrade );

public:
	void SetEquipCostumeInfo( int nEquipSlot, int nCostumeCode, DWORD dwMaleCustom, DWORD dwFemaleCustom );
	void SetEquipAccessoryInfo( int iEquipSlot, int iAccessoryIdx, int iAccessoryCode, int iRandomNum, int iCompoundCode, int iCompoundValue );
	
public:
	void SetExtraJumpState();

public: 
	void SetAttackBuffGrapplingState( const ioHashString &szTarget, int nBuffType );
	bool SetBuffGrapplingState( const ioHashString &szAttacker, GrapplingSkillWoundedInfo kWoundedInfo );
	void ProcessBuffWoundGrapplingLoop( ioBaseChar *pAttacker, int nBuffType );
	D3DXVECTOR3 GetGrapplingBuffGapPosition( int nBuffType );

public:
	void SetDisableEquipObject( bool bDisable ){ m_bDisableEquipObject = bDisable; }
	bool IsDisableEquipObject(){ return m_bDisableEquipObject; }

protected:
	void CheckGrapplingBuffState();

protected:
	bool IsEnableJumpAttack();

public:
	void ClearAniEffect();
	
protected:
	bool IsSwitchSkillEndJump();

protected:
	void ApplyWeaponWoundedEnableChangeWoundState( ioWeapon *pWeapon, bool &bEnableCancel, bool &bNoCancel, DefenseBreakType eBreakType );
	void ApplyWeaponWoundedCheckFrozenState( ioWeapon *pWeapon, ioBaseChar *pAttacker, bool &bFrozenBuff, bool &bFrozenState, int &iFrozenType );
	void ApplyWeaponWoundedBasePushBlowCalc( ioWeapon *pWeapon, float fDamage, float &fPushPower, float &fBlowPower, bool &bForceLinear );
	void ApplyWeaponWoundedExtraPushBlowCalc( ioWeapon *pWeapon, float &fPushPower, float &fBlowPower );
	void ApplyWeaponWoundedBaseDamageCalc( ioWeapon *pWeapon, AttackSide eSide, float &fDamage, float &fBoundDamage, bool bTeamAttack );
	void ApplyWeaponWoundedCalcByBuff( ioBaseChar *pAttacker, float &fPushPower, float &fBlowPower, float &fBoundDamage, float &fBoundJumpAmt, float &fBoundForceAmt, float &fBoundGrvityRate );
	void ApplyWeaponWoundedProtectDamageCalc( ioBaseChar *pAttacker, bool &bProtectDamage, float &fPreCheckDamage, float &fDamage, 
												AttackMethodType eMethodType, int iIgnoreProtect, DefenseBreakType eBreakType, AttackSide eSide, bool bCheckStopMotion );
	void ApplyWeaponWoundedCheckBlock( ioWeapon *pWeapon, int iIgnoreProtect, float &fDamage, bool &bProtectDamage, bool bContactType, 
										bool &bOnlyEmoticon, AttackBlockType &eBlockType, ProtectionMode &eProtectionType, AttackSide eAttackSide, DefenseBreakType eBreakType,
										AttackMethodType eMethodType, bool bFrozenBuff, bool bFrozenState, int iFrozenType, bool bCheckStopMotion, bool &bProtectionMagic, bool &bProtectShakeCamera, bool bAirTargetBuff );
	void ApplyWeaponWoundedReflect(  ioWeapon *pWeapon, ioBaseChar *pAttacker, 
										bool &bStateProtectIgnoreReflect, bool &bApplyStateProtect, ioHashString &szReflectAni, float &fReflectAniRate,
										bool &bPushAttacker, float &fAttackerPushPower, float &fWeakAttackPushPower,
										AttackSide eSide, bool bOnlyEmoticon, const D3DXVECTOR3 &vAttackDir, AttackBlockType eBlockType, ProtectionMode eProtectionType,
										float &fDamage, bool bContactType, EscapeCatchType eEscapeCatchType, DefenseBreakType eBreakType, bool bStructWeapon );
	void ApplyWeaponWoundedFrontBackBuff( ioBaseChar *pAttacker, AttackSide eAttackSide, bool bAirTargetBuff, bool bTargetBuff, bool bDownTargetBuff,
											ioHashString &szFrontBackWoundBuff, ioHashString  &szFrontBackAttackerBuff, float &fDamage );
	void ApplyWeaponWoundedExtraDamageCalc( ioWeapon *pWeapon, ioBaseChar *pAttacker, bool bProtectDamage, float fPreCheckDamage, float &fDamage );
	void ApplyWeaponWoundedDamageFunc( ioWeapon *pWeapon, float fDamage );
	void ApplyWeaponWoundedApplyPushPower( ioWeapon *pWeapon, bool &bApplyPushPower, bool &bCharDie, 
											bool bFrozenState, bool bFrozenBuff, int iFrozenType, DefenseBreakType eBreakType, 
											AttackSide eAttackSide, AttackMethodType eMethodType, int iIgnoreProtect, bool bProtectionMagic,
											bool bTeamAttack, bool bContactType, bool bCheckStopMotion, const D3DXVECTOR3 &vAttackDir, float fDamage );

	void ApplyWeaponWoundedChangeState( ioWeapon *pWeapon, ioBaseChar *pAttacker, 
											bool &bAllWeakWounded, ioHashString &szWoundAni, DWORD &dwWoundDuration, bool &bWoundedLoopAni, bool &bIgnorCatch, float &fDefenseEnableRate, BlowTargetType &eNewBlowType,
											bool &bCharDie, float &fPushPower, float &fBlowPower, bool bApplyPushPower, bool bTeamAttack, 
											float fDamage, float fBoundJumpAmt, float fBoundForceAmt, float fBoundGrvityRate, float fBoundDamage,
											bool &bPrisoner, bool bEnableCancel, bool bNoCancel,
											ioHashString &szExtendWoundAni, DWORD &dwExtendWoundDuration, float fExtendWoundJumpAmt, float fExtendWoundForceAmt );
	void ApplyWeaponWoundedChangePrisonerState( bool bTeamAttack, bool bFrozenState, bool bContactType, EscapeCatchType eEscapeCatchType );
	void ApplyWeaponWoundedPushBlowCalcByBuff( ioWeapon *pWeapon, ioBaseChar *pAttacker, float &fPushPower, float &fBlowPower, bool bTeamAttack );
	void ApplyWeaponWoundedPushBlowPower( ioWeapon *pWeapon, bool bApplyPushPower, const D3DXVECTOR3 &vAttackDir, float fPushPower, float &fBlowPower, bool bForceLinear );
	void ApplyWeaponWoundedAddBuff( ioWeapon *pWeapon, ioBaseChar *pAttacker, 
										bool bApplyPushPower, bool bEnableCancel, bool bTeamAttack, bool &bEnableTeamAttack, bool &bTeamAttackNoDamage,
										bool bPrisoner, ioHashString &szFrontBackWoundBuff, ioHashString &szFrontBackAttackerBuff, ioHashString &szDefenseBreakBuff,
										bool bAirTargetBuff, bool bTargetBuff, bool bDownTargetBuff, bool bAttackTargetBuff, bool bDashTargetBuff,
										DefenseBreakType eBreakType, bool &bLandStunBuff, BuffInfoList &kAddBuffList );
	void ApplyWeaponWoundedActionStopDelay( ioWeapon *pWeapon, ioBaseChar *pAttacker, int &iShakeCamera, bool &bTargetDie, ActionStopType &eActionStype,
												bool bProtectShakeCamera, bool bCharDie, bool bStructWeapon, float fDamage, bool bApplyPushPower );
	void ApplyWeaponWoundedGrappling( ioWeapon *pWeapon, bool &bNoChangeGrappling, bool bApplyPushPower, bool bTargetDie, bool bPrisoner, CharState eCheckCharState );
	void ApplyWeaponWoundedWeaponFunc( ioWeapon *pWeapon, ioBaseChar *pAttacker, bool bApplyPushPower );
	void ApplyWeaponWoundedSkillFunc( ioWeapon *pWeapon, ioBaseChar *pAttacker, bool bApplyPushPower, float fDamage, DefenseBreakType eBreakType, AttackBlockType eBlockType, ProtectionMode eProtectionType );
	void ApplyWeaponWoundedBuffFunc( ioBaseChar *pAttacker );
	void ApplyWeaponWoundedNextStateFunc( ioWeapon *pWeapon, ioBaseChar *pAttacker, float fDamage, BuffInfoList &kAddBuffList );
	void ApplyWeaponWoundedSingleFunc( ioWeapon *pWeapon, ioBaseChar *pAttacker, float fDamage, bool bAirTargetBuff, bool bApplyPushPower, AttackBlockType eBlockType, ProtectionMode eProtectionType );

public:
	void ApplyWeaponWounded( ioWeapon *pWeapon, AttackSide eSide, const D3DXVECTOR3 &vAttackDir );

public:
	bool IsReverseDirection() { return m_bReverseDirection; }
	void SetOnlyReverseDirection( bool bSetReverseDirection ) { m_bReverseDirection = bSetReverseDirection; }	
	float GetMinFallHeightRate() { return m_fMinFallHeightRate; }
	
	//createindex가 맞으면 벗겨주고 itemcode로 새로 생성
	ioItem* ReleaseNCreateExplict( DWORD dwItemCode, DWORD dwItemCreateIndex, int iSlot );
	std::vector<ioChampionAI*> m_vecChampionAI;
	void AddAIRef( ioChampionAI* pAI );
	void ReleaseAIRef( ioChampionAI* pAI );
	std::map<ioHashString,int> m_mapAggro;	//npc전용
	void SetAggro( const ioHashString& hsName, int iAggroValue );
	int GetAggro( const ioHashString& hsName );

protected:
	//extra gravity rate
	void InitExtraGravityRate();
	void SetExtraGravityRate( int iAniID, float fTimeRate, DWORD dwPredelay=0 );
	void CheckExtraAniGravityRate();

//R Soldier
protected:
	void GetRSoldierGrowth( int &nGrowthLevel, bool bCharGrowth, int nGrowthArray ) const;
public:
	void SetCurRSoldierCount( int iCurRSoldierCount ) { m_iCurRSoldierCount = iCurRSoldierCount; }
	inline const int& GetCurRSoldierCount() { return m_iCurRSoldierCount; }

public:
	bool IsInvisibleBottomCircleBySkill();

protected:
	void GetSoldierSetGrowth( int &nGrowthLevel, bool bCharGrowth, int nGrowthArray ) const;

public:
	void SetCurSoldierSetCnt( int nCurCharSetCnt ) { m_nSoldierSetCnt = nCurCharSetCnt; }
	inline const int& GetCurSoldierSetCnt() { return m_nSoldierSetCnt; }

protected:
	void CheckCreateEquipDummy();
	
public:
	DWORD GetGetupDurationTime() { return m_dwGetUpDurationTime; }

public:
	void AddArmorPenetration( float fAddArmorPenetration );
	void RemoveArmorPenetration( float fAddArmorPenetration );
	
	void AddExtraArmorClass( float fExtraArmorClass );
	void RemoveExtraArmorClass( float fExtraArmorClass );

	void AddCriticalRate( float fCriticalRate );
	void RemoveCriticalRate( float fCriticalRate );
	bool CaclCriticalProbability( ioWeapon *pWeapon );

	void AddCriticalDamageRate( float fCriticalDamageRate );
	void RemoveCriticalDamageRate( float fCriticalDamageRate );

	void SetExtraLandingRateByBuff( float fRate, float fExtendRate );

	void SetAttackerName( ioHashString szName )		{ m_szAttackerName = szName; }
	ioHashString& GetAttackerName()					{ return m_szAttackerName; }

	// 낚은 치장 아이템 확인 - STPK_PRESENT_DATA 패킷에서 받은 값인 파라메터와 낚시 결과 패킷에서 받은 값이랑 일치하면 PresentState 1로 변경하도록 true 반환
	bool CheckRecvFishItem( int iPresentType, int iPresentValue1, int iPresentValue2 );

	bool CheckImmediatelyChange( int iCharArray );
	inline bool GetImmediatelyChange() { return m_bCharacterImmediatelyChange; }
	inline void SetImmediatelyChange( bool bChange ) { m_bCharacterImmediatelyChange = bChange; }
	inline float GetCharacterChangeImmediatelyTime() { return m_fCharacterChangeImmediatelyTime; }
	inline void SetCharacterChangeImmediatelyTime( DWORD dwTime ) { m_fCharacterChangeImmediatelyTime = dwTime; }

	void ProcessImmediatelyChangeCoolTime();

//연승전
protected:
	void GetGrowthLevelByMode( int &nGrowthLevel, bool bCharGrowth, int nGrowthArray ) const;
public:
	ioBaseChar( ioEntityGroup *pGrp, ioPlayStage *pCreator, ioPlayMode *pMode );
	virtual ~ioBaseChar();
};

inline ioBaseChar* ToBaseChar( ioGameEntity *pObj )
{
	ioPlayEntity *pPlay = ToPlayEntity( pObj );
	if( !pPlay || pPlay->GetSubType() != ioPlayEntity::PST_CHAR )
		return NULL;

	return dynamic_cast< ioBaseChar* >( pPlay );
}

inline const ioBaseChar* ToBaseCharConst( const ioGameEntity *pObj )
{
	const ioPlayEntity *pPlay = ToPlayEntityConst( pObj );
	if( !pPlay || pPlay->GetSubType() != ioPlayEntity::PST_CHAR )
		return NULL;

	return dynamic_cast< const ioBaseChar* >( pPlay );
}

inline ioBaseChar* ToUserChar( ioGameEntity *pObj )
{
	ioBaseChar *pChar = ToBaseChar( pObj );
	if( !pChar || pChar->GetControlType() != CONTROL_USER )
		return NULL;

	return pChar;
}
