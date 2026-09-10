#pragma once

#include "ioExtendDash.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioBaseChar;
class ioPlayStage;

class ioChargeComboDash : public ioExtendDash
{
public:
	enum DashState
	{
		DS_NONE,
		DS_CHARGE,
		DS_NORMAL_ATTACK,
		DS_COMBO_ATTACK,
		DS_BUFF_ADD,
		DS_MECHANICS_FLY,
		DS_EXTEND_COMBO,
		DS_RUN_ROTATE,
		DS_RUN_ROTATE_END,
		DS_POWER_CHARGE,
		DS_POWER_CHARGE_ATTACK,				// 10
		DS_PHANTOM_FLY,
		DS_LUCIFER_FLY,
		DS_DESTROYER_FLY,
		DS_MICHAEL_FLY,
		DS_GOOMIHO_FLY,
		DS_SUCCUBUS_CHAIN,
		DS_DRACULA,
		DS_FLY_DASH_ATTACK,
		DS_FLY_DASH_ATTACK_LANDING,
		DS_FLY_DASH_ATTACK_END,				// 20
		DS_FLASH,
		DS_EVY,
		DS_SLASHER,
		DS_TOP_BL_EXTEND,
		DS_TOP_BL_RETURN,
		DS_KAELTHAS_EXTEND,
		DS_DRAGONNEST_ASS,
		DS_THUNDER_BIRD,
		DS_EQUIP_OBJECT,
		DS_DELAY_STATE,						// 30
		DS_LEE_DRAGON_ATTACK_STATE,
		DS_PARKILPYO_NORMAL_GATHER,
		DS_PARKILPYO_NORMAL_CHARGE_ATTACK,
		DS_PARKILPYO_NORMAL_CHARGE_COMBO,
		DS_PARKILPYO_NORMAL_EQUIP,
		DS_PARKILPYO_OBJECT_CHARGE_ATTACK,
		DS_JACK_CHARGE_ATTACK,
		DS_SHADOW_RANGER,
		DS_EXTEND_COMBO2,
		DS_KEY_INPUT_LAST_ATTACK,			// 40
		DS_BULLFIGHT,
		DS_JEONWOOCHI_DASH,
		DS_RED_HOOD_CHARGE_ATTACK_STATE,
		DS_CUCHULAIN_MIDDLE_ATTACK_STATE,
		DS_ONIMUSHA_CHARGE_ATTACK,
		DS_ONIMUSHA_RUN,
		DS_HALF_EXTEND_ATTACK,
		DS_SASINMU,
		DS_BUBBLE_FIGHTER_ATTACK,
		DS_NIGHTMARE,						// 50
		DS_AZAZEL,
		DS_ROTATE_CHARGE,
		DS_SPECIAL_ATTACK,
		DS_EXTEND_CHARGE,
		DS_MIDNIGHT
	};

	enum DashExtendType
	{
		DET_NONE,
		DET_BUFF_ADD,
		DET_MECHANICS_FLY,
		DET_EXTEND_COMBO,
		DET_RUN_ROTATE,
		DET_POWER_CHARGE,
		DET_RUN_ROTATE2,
		DET_PHANTOM_FLY,
		DET_LUCIFER_FLY,
		DET_DESTROYER_FLY,
		DET_MICHAEL_FLY,		// 10
		DET_GOOMIHO_FLY,
		DET_SUCCUBUS_CHAIN,
		DET_DRACULA,
		DET_FLY_DASH_ATTACK,
		DET_FLASH,
		DET_EVY,
		DET_SLASHER,
		DET_TOP_BL,
		DET_KAELTHAS,
		DET_DRAGONNEST_ASS,		// 20
		DET_THUNDER_BIRD,
		DET_EQUIP_OBJECT,
		DET_LEE,
		DET_PARKILPYO_NORMAL,
		DET_PARKILPYO_OBJECT,
		DET_JACK,
		DET_SHADOW_RANGER,
		DET_JUMPER,
		DET_SABER,
		DET_ANGEL_OF_DEATH,		// 30
		DET_HADES,
		DET_MAGIC_SWORD,
		DET_NATA,
		DET_RESEARCHER_REAR_FLY,
		DET_BULLFIGHT,
		DET_JEONWOOCHI,
		DET_CHEMISTRY,
		DET_SAWANG_SLIDE,
		DET_RED_HOOD,
		DET_STRIDER,			//40
		DET_APOSTATE_MONK,
		DET_CUCHULAIN,
		DET_ONIMUSHA,
		DET_DRUID,
		DET_SASINMU,
		DET_PREDATOR,
		DET_CROW,
		DET_BLACK_SPHEAR,
		DET_MEPHISTO,
		DET_BUBBLE_FIGHTER,		//50
		DET_URIEL,
		DET_WINDGOD,
		DET_STRANGER,
		DET_HACKER,
		DET_MAGICENGINEER,
		DET_NIGHTMARE,
		DET_AZAZEL,
		DET_NEO,
		DET_AUTOMATON,
		DET_UFC,				//60
		DET_VULCAN,
		DET_CHANGE_ON,
		DET_CHANGE_ON_WOLF,
		DET_GHOST,
		DET_HANZO,
		DET_OMEN,
		DET_MIDNIGHT,
		DET_ROCKET_LAUNCHER,
		DET_HERACLES
	};

	enum DashSyncType
	{
		DST_STATE,
		DST_KEY,
	};

	enum JeonWooChiMoveStateType
	{
		JWCST_NONE,
		JWCST_POWER_CHARGE,
		JWCST_DASH_STATE,
		JWCST_DASH_ATTACK,
		JWCST_DASH_ADD_ATTACK,
	};
	enum SpecialState
	{
		SS_NORMAL_S,
		SS_NORMAL_A,
		SS_CHARGE_S,
		SS_CHARGE_A,
	};

protected:
	JeonWooChiMoveStateType m_JeonWooChiState;

	DashState m_DashState;

	bool m_bNormalAttacked;
	bool m_bSetCombo;
	int m_iCurComboCnt;

	DWORD m_dwActionEndTime;
	DWORD m_dwActionStartTime;

	DWORD m_dwPreReserveEnableTime;

	DWORD m_dwChargingTime;
	DWORD m_dwHalfChargingTime;
	DWORD m_dwChargingStartTime;
	DWORD m_dwExtendChargingTime;

	ioHashStringVec m_ChargingAniList;
	ioHashStringVec m_ExtendChargingAniList;

	//탑블레이트
	std::vector<StTopMoveInfo> m_vDummyMoveInfoCombo;
	StTopMoveInfo m_TopBl_Normal;
	StTopMoveInfo m_TopBl_ChargeInfo;
	AttackAttribute m_TopBl_ChargeAttack;

	DWORD			m_dwChargeAttackDummyDelayTime;
	DWORD			m_dwChargeAttackDummyFireTime;

// dash extend type
protected:
	DashExtendType m_DashExtendType;

	// buff add
	AttackAttribute m_BuffAddAttribute;
	ioHashStringVec m_vBuffAddList;

	// mechanics fly
	MechanicsFlyInfo m_DashMechanicsFly;

	// LuciferFlyInfo
	LuciferFlyInfo	 m_DashLuciferFly;

	//싸이
	DestroyerFlyInfo	 m_DestroyerFly;

	// michael fly
	MichaelFlyInfo m_DashMichaelFly;

	//구미호
	GoomihoFlyInfo m_DashGoomihoFly;

	//서큐버스
	SuccubusAttackInfo m_DashSuccubusChain;

	//드라큘라
	DraculaFlyInfo m_DashDraculaFly;

	//이비
	EvyBlinkInfo m_EvyBlinkInfo;

	//슬래셔
	SlasherRunInfo m_SlasherRunInfo;

	//전우치
	JeonWooChiDashMoveInfo m_JeonWooChiDashInfo;

	//미드나잇
	MidnightAttackInfo m_DashMidnightDashInfo;
	//////////////////////////////////////////////////////////////////////////
	float m_fJeonWooChiCurMoveLength;
	float m_fJeonWooChiNeedMoveLength;
	D3DXVECTOR3 m_vJeonWooChiMoveDir;
	ioHashString m_stJeonWooChiChargeTargeted;
	//////////////////////////////////////////////////////////////////////////

	// extend combo
	AttributeList m_ExtendAttackList;

	// extend combo2
	AttributeList m_ExtendAttackList2;

	//신비술사
	AttackAttribute m_KaelthasExtendAttribute;
	DWORD			m_dwKaelthasExtendAttackStartTime;

	//LeeDragonAttack
	AttackAttribute m_LeeDragonExtendAttribute;
	DWORD			m_dwLeeExtendAttackStartTime;
	ioHashString	m_stDrangonAttackBuff;
	ioHashString	m_stDrangonAttackCancelBuff;

	//fly dash
	CEncrypt<float> m_fFlyDashSpeed;
	CEncrypt<DWORD> m_dwFlyDashDuration;

	// run rotate
	CEncrypt<float> m_fRunSpeed;
	DWORDVec m_vRunDurationVec;
	CEncrypt<DWORD> m_dwRotateSpeed;

	DWORD m_dwRunRotateStartTime;
	DWORD m_dwRunRotateJumpTime;
	DWORD m_dwFlyDashAttackStartTime;
	DWORD m_dwReserveKeyTime;
	DWORD m_dwRunRotateChargeTime;
	bool m_bRotateFirstAttack;
	bool m_bRotateChargeEnable;

	int m_iRunRotateAttackMaxCount;
	AttributeList m_RunRotateAttackList;
	AttackAttribute m_FlyDashAttack;

	ioHashString	m_FlyDashLandingAni;
	float			m_fFlyDashLandingAniRate;

	CEncrypt<bool> m_bTargetRot;
	ioUserKeyInput::DirKeyInput m_CurDirKey;

	AttackAttribute m_RunRotateAttackEnd;
	AttackAttribute m_FlyDashAttackEnd;

	// power charge
	ioHashString m_PowerChargeAni;

	CEncrypt<DWORD> m_dwMaxChargeTime;
	CEncrypt<DWORD> m_dwChargeStartTime;

	CEncrypt<float> m_fCurChargeRate;

	CEncrypt<float> m_fMaxDamageRate;
	CEncrypt<float> m_fMaxForceRate;
	CEncrypt<float> m_fMaxBlowRate;

	CEncrypt<float> m_fMaxRangeRate;
	CEncrypt<float> m_fMaxSpeedRate;
	CEncrypt<float> m_fMaxChargeAniRate;
	CEncrypt<float> m_fMaxForceSlidingRate;

	CEncrypt<float> m_fMaxTargetRangeRate;
	CEncrypt<float> m_fMaxTargetAngleRate;

	CEncrypt<float> m_fMaxFloatRate;

	CEncrypt<float> m_fMaxScaleRate;
	CEncrypt<float> m_fMaxExplosionRate;
	CEncrypt<float> m_fMaxLiveTimeRate;
	CEncrypt<float> m_fMaxWoundedTimeRate;

	AttackAttribute m_PowerChargeAttack;

	ioHashString m_DashGatheringEffect;
	UniqueObjID m_GatheringEffectID;

	CEncrypt<bool> m_bPowerChargeUseDefense;

	DWORD m_dwInputCancelCheckTime;
	DWORD m_dwCancelCheckTime;

	bool m_bPlayDashExtendWoundAni;

	//Fly Dash Attack 게이지 소모 
	float	m_fFlyDashDecreseGauge;

	//KeyReserve와 상관없이 Input_Jump_S 매크로가 있는 시간부터 따로 체크
	bool m_bExceptionInputJumpSCheck;
	bool m_bInitActionDelayTime_NormalAttack;

protected:
	// For Ani Rotate
	vAniRotateInfoList m_vAniRotateInfoList;

	int m_iCurAniRotate;
	DWORD m_dwAniRotateTime;
	float m_fAniRotateAngle;

//드래곤 슬레이어
protected:
	bool m_bAniJumpInputS;

protected:
	ioHashString				m_szInputEventAnimation;
	float						m_fInputEventAnimationRate;	
	ioUserKeyInput::DirKeyInput	m_eSpecialDashCurrDirkey;

//드래곤 네스트 어쎄신
protected:
	enum DragonNestAssSubState
	{
		DNASS_Pre,
		DNASS_Teleport,
		DNASS_LandMove,
	};

protected:
	DragonNestAssSubState m_DragonNestAssSubState;

	AttackAttribute m_DragonNestAssassinPreAttribute;
	AttackAttribute m_DragonNestAssassinTeleportAttackAttribute;
	float m_fDragonNestAssassinExtendTeleportEndJump;
	D3DXVECTOR3 m_vDragonNestAssassinTeleportOffSet;

//썬더버드
protected:
	float m_fThunderBirdUseGauge;

protected:
	bool m_bMaxDashLastChargeAttack;

//박일표
protected:
	ioHashString m_szParkilpyoGatherAni;
	float m_fParkilpyoGatherAniRate;
	AttackAttribute m_ParkilpyoChargeAttack;
	AttributeList m_ParkilpyoChargeComboList;
	float m_fParkilpyoUseGauge;
	DWORD m_dwEnableKeyTime;
	int m_nCurChargeCombo;

//잭
protected:
	bool m_bSetEnableReserveDash;
	AttackAttribute m_JackChargeAttack;
	float m_fJackUseGauge;
	int m_nMaxReserveDash;
	int m_nCurReserveDash;

//쉐도우레인저
protected:
	float m_fShadowRangerUseGauge;

//마검사
	bool						m_bKeyInputLastAttack;
	AttackAttribute				m_AttackKeyAttribute;
	AttackAttribute				m_DefenseKeyAttribute;
	AttackAttribute				m_JumpKeyAttribute;

//RedHood
	DWORD						m_dwRedHoodSpecialMoveMacroTime_S;
	DWORD						m_dwRedHoodSpecialMoveMacroTime_E;
	
//MagicEngineer
	DWORD m_dwMESpecialMoveMacroTime_S;
	DWORD m_dwMESpecialMoveMacroTime_E;

//쿠훌린
	AttackAttribute				m_CuchulainMiddleAttack;
	AttackAttribute				m_CuchulainChargeAttack;
	AttributeList				m_CuchulainChargeComboAttack;
	bool						m_bMiddleChargeStarted;
	CuchulainFlyInfo			m_CuchulainFlyInfo;

	//귀무사
protected:
	AttackAttribute m_OnimushaChargeAttack;
	float m_fOnimushaChargeUseGauge;
	float m_fOnimushaObjectChargeUseGauge;
	bool m_bOnimushaChargeAttack;
	
//약 D~ 추가
	AttackAttribute				m_HalfExtendAttack;
	bool						m_bEnableHalfExtendAttack;
	int							m_nHalfAttackCount;

	bool						m_bExtendDashInpuDashEnable;

	DWORD m_dwFireTime;
	float m_fAzazelRange;
	float m_fAzazelAngle;
	ioHashString m_AzazelAimedTarget;
	D3DXVECTOR3 m_vAzazelAttackOffset;
	
	AttackAttribute		m_NormalSAttribute;
	AttackAttribute		m_NormalAAttribute;
	AttackAttribute		m_ChargeSAttribute;
	AttackAttribute		m_ChargeAAttribute;

	float m_fGhostExtendDashJumpRate;

protected:
	void CheckAniRotate( ioBaseChar *pChar );

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual DashType GetType() const;
	virtual ioExtendDash* Clone();

	virtual bool StartDash( ioBaseChar *pOwner );
	virtual void ProcessDash( ioBaseChar *pOwner );
	virtual void ExtendDashEnd( ioBaseChar *pOwner );

	virtual void ApplyDash( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual bool GetAutoTargetValue( IN float &fDashRange, IN float &fDashMinAngle, IN float &fDashMaxAngle,
									 OUT float &fRange, OUT float &fMinAngle, OUT float &fMaxAngle );

	virtual bool IsEnableDefense();
	bool IsPowerChargingState();

	//전우치때 추가
	bool CheckProcessDash( ioBaseChar *pOwner );

	virtual void SetNextDashState( ioBaseChar *pWoundChar, ioBaseChar *pAttacker, ioWeapon *pWeapon, bool bDefense, float fDamage );

public:
	void SetExtraInfoToWeapon( ioBaseChar *pOwner, ioPlayStage *pStage, ioWeapon *pWeapon, const ioHashString &szName );
	void CheckCancelReserveByConditionalSpecialDash( ioBaseChar *pOwner );

public:
	void SetCombo( bool bSetCombo ){ m_bSetCombo = bSetCombo; }
	void SetCurrComboCnt( int iCurrComoboCnt ){ m_iCurComboCnt = iCurrComoboCnt; }
	int GetCurrComboCnt(){ return m_iCurComboCnt; }

protected:
	void SetComboDash( ioBaseChar *pOwner, bool bCharge = false );

	void ProcessCharging( ioBaseChar *pOwner );
	void ProcessNormalComboDash( ioBaseChar *pOwner );
	void ProcessExtendComboDash( ioBaseChar *pOwner );
	void ProcessRunRotate( ioBaseChar *pOwner );
	void ProcessRunRotateEnd( ioBaseChar *pOwner );
	void ProcessPowerCharge( ioBaseChar *pOwner );
	void ProcessPowerChargeAttack( ioBaseChar *pOwner );
	void ProcessFlyDashAttack( ioBaseChar *pOwner );
	void ProcessFlyDashAttackLanding( ioBaseChar *pOwner );
	void ProcessFlyDashAttackEnd( ioBaseChar *pOwner );
	void ProcessKaelthasExtendAttack( ioBaseChar *pOwner );
	void ProcessDragonNestAssassinState( ioBaseChar *pOwner );
	void ProcessJeonWooChiDashState( ioBaseChar *pOwner );
	void ProcessExtendCharging( ioBaseChar *pOwner );

	void ChangeToCharging( ioBaseChar *pOwner );
	void ChangeToExtendCharging( ioBaseChar *pOwner );
	void ChangeToNormalAttack( ioBaseChar *pOwner, bool bCancelAttack = false, bool bChargeAttack = false);

	virtual void CheckKeyReserve( ioBaseChar *pOwner );
	virtual void ProcessKeyReserve( ioBaseChar *pOwner );
	
	void CheckExtraReserve( ioBaseChar *pOwner );
	void CheckExtendReserve( ioBaseChar *pOwner );
	bool CheckPreReserve( ioBaseChar *pOwner );

	void ApplyDashState( ioBaseChar *pOwner, SP2Packet &rkPacket );
	void ApplyDashDirKey( ioBaseChar *pOwner, SP2Packet &rkPacket );

// buff add
protected:
	bool CheckEnableBuffAddState( ioBaseChar *pOwner );
	void SetBuffAddState( ioBaseChar *pOwner );

// fly dash attack
	bool CheckEnableFlyDashAttackState( ioBaseChar *pOwner );
	void SetFlyDashAttackState( ioBaseChar *pOwner, bool bSendNet );
	void SetFlyDashAttackLandingState( ioBaseChar *pOwner );
	void SetFlyDashAttackStateEnd( ioBaseChar *pOwner );

protected:
	bool CheckEnableMechanicsFlyState( ioBaseChar *pOwner );
	void SetMechanicsFlyState( ioBaseChar *pOwner );

	void LoadMechanicsFlyInfo( ioINILoader &rkLoader );

protected:
	bool CheckEnableDestroyerFlyState( ioBaseChar *pOwner );
	void SetDestroyerFlyState( ioBaseChar *pOwner );

	void LoadDestroyerFlyInfo( ioINILoader &rkLoader );

protected:
	bool CheckEnableExtendComboState( ioBaseChar *pOwner );
	void SetExtendComboState( ioBaseChar *pOwner );

protected:
	bool CheckEnableAngelOfDeathPowerAttackState( ioBaseChar *pOwner );
	void SetAngelOfDeathPowerAttackState( ioBaseChar *pOwner );

protected:
	bool CheckRunRotateState( ioBaseChar *pOwner, bool bCheckComboCount = false );
	bool CheckRunRotateState2( ioBaseChar *pOwner );

	void SetRunRotateState( ioBaseChar *pOwner, bool bSendNet, bool bFirstAttack );
	void SetRunRotateChargeState( ioBaseChar *pOwner, bool bSendNet );
	void SetRunRotateStateEnd( ioBaseChar *pOwner );

	void CheckKeyInput( ioBaseChar *pOwner );
	void ProcessRotate( ioBaseChar *pOwner );

	bool CheckJumpRunRoateEnd( ioBaseChar *pOwner );

	bool CheckAniJump_Input_Jump_S( ioBaseChar *pOwner );
	bool CheckAniJump_Input_Jump_S_Exception(  ioBaseChar *pOwner );
	bool CheckAniDash_Input_Speical_Dash( ioBaseChar *pOwner );

	// 2020-05-25 신규 용병 레어(권왕)
	bool ProcessCancelBySpeicalDash( ioBaseChar *pOwner );
	bool CheckAniJump_Input_Speical_Jump( ioBaseChar *pOwner );		
	bool CheckAniCharge_Input_Speical_Charge( ioBaseChar *pOwner );	

protected:
	bool CheckPowerChargeState( ioBaseChar *pOwner );
	void SetPowerChargeState( ioBaseChar *pOwner, bool bSendNet );
	void SetPowerChargeAttack( ioBaseChar *pOwner );

	void CheckChargeInfo( ioWeapon *pWeapon );

protected:
	bool CheckEnablePhantomFlyState( ioBaseChar *pOwner );
	void SetPhantomFlyState( ioBaseChar *pOwner );

	//루시퍼
protected:
	bool CheckEnableLuciferFlyState( ioBaseChar *pOwner );
	void SetLuciferFlyState( ioBaseChar *pOwner );

	void LoadLuciferFlyInfo( ioINILoader &rkLoader );

// michael fly
protected:
	bool CheckEnableMichaelFlyState( ioBaseChar *pOwner );
	void SetMichaelFlyState( ioBaseChar *pOwner );

	void LoadMichaelFlyInfo( ioINILoader &rkLoader );

//구미호 
protected:
	bool CheckEnableGoomihoFlyState( ioBaseChar *pOwner );
	void SetGoomihoFlyState( ioBaseChar *pOwner );

	void LoadGoomihoFlyInfo( ioINILoader &rkLoader );

//서큐버스
protected:
	bool CheckEnableSuccubusState( ioBaseChar *pOwner );
	void SetSuccubusState( ioBaseChar *pOwner );
	
	void LoadSuccubusStateInfo( ioINILoader &rkLoader );

//드라큘라
protected:
	bool CheckEnableDraculaState( ioBaseChar * pOwner );
	void SetDraculaState( ioBaseChar *pOwner );

	void LoadDraculaStateInfo( ioINILoader &rkLoader );

	bool CheckEnableNightmareState( ioBaseChar * pOwner );
	void SetNightmareState( ioBaseChar *pOwner );

	bool CheckEnableAzazelState( ioBaseChar * pOwner );
	bool CheckEnableAzazelSpecialState( ioBaseChar * pOwner );
	void SetAzazelState( ioBaseChar *pOwner, bool bAim );
	void SetAzazelSpecialState( ioBaseChar *pOwner );

	bool CheckEnableHanzoState( ioBaseChar * pOwner );
	void SetHanzoState( ioBaseChar *pOwner, int iType );

//플래쉬
protected:
	bool CheckEnableFlashState( ioBaseChar * pOwner );
	void SetFlashState( ioBaseChar *pOwner );

//이비
protected:
	bool CheckEnableEvyState( ioBaseChar *pOwner );
	void SetEvyState( ioBaseChar *pOwner );

	void LoadEvyStateInfo( ioINILoader &rkLoader );
	
protected:
	bool CheckEnableSlasherState( ioBaseChar * pOwner );
	void SetSlasherState( ioBaseChar *pOwner );

	void LoadSlasherStateInfo( ioINILoader &rkLoader );

//신비술사
protected:
	bool CheckKaelthasExtendAttack( ioBaseChar *pOwner );
	void SetKaelthasExtendAttack( ioBaseChar *pOwner );

	void LoadKaelthasExtendAttackInfo( ioINILoader &rkLoader );

//탑블레이트
protected:
	void LoadTopBlDummyInfo( ioINILoader &rkLoader );
	bool CheckEnalbe_Top_Bl_ExtendMod( ioBaseChar *pOwner );
	bool CheckIsExtend_Top_Bl_mod( ioBaseChar *pOwner );

	void SetTopBlExtendAttack( ioBaseChar *pOwner );
	void SetTopBlExtendMod( ioBaseChar *pOwner );

	void ProcessTopBlReturn( ioBaseChar *pOwner );

	void SetTopBlDummyNormalMove( ioBaseChar *pOwner, bool bCombo, int nCombo );

//하데스
	bool CheckEnableHadesState( ioBaseChar *pOwner );
	void SetHadesHalfChargeState( ioBaseChar *pOwner );
	void SetHadesFullChargeState( ioBaseChar *pOwner );

	bool CheckEnableMagicEngineerState( ioBaseChar *pOwner );
	void SetMagicEngineerChargeState( ioBaseChar *pOwner, bool bHalf );

//전우치
	void LoadJeonWooChiExtendDashInfo( ioINILoader &rkLoader );
	bool CheckEnableJeonWooChiChargeAttack( ioBaseChar *pOwner );
	void SetJeonWooChiChargeState( ioBaseChar *pOwner );
	void SetJeonWooChiDashState( ioBaseChar *pOwner );
	void SetJeonWooChiDashAttack( ioBaseChar *pOwner );
	void SetJeonWooChiDashAddAttack( ioBaseChar *pOwner );
	void OnJeconWooChiDashMove( ioBaseChar *pOwner );

//화학병
	bool CheckEnableChemistyChargeAttack( ioBaseChar *pOwner );
	void SetChemistryChargeState( ioBaseChar *pOwner );
	
//드래곤 네스트 어쎄신
protected:
	void LoadDragonNestAssExtendInfo( ioINILoader &rkLoader );
	bool CheckDragonNestAssExtendState( ioBaseChar *pOwner );
	void CheckDragonNestAssExtendReserveKey( ioBaseChar *pOwner );

//RedHood
protected:
	void SetMacroTime( ioEntityGroup* pGrp, int iAniID, float fTimeRate, DWORD dwEndTime );
	
protected:
	void SetDragonNestAssExtendPreState( ioBaseChar *pOwner );
	void SetDragonNestAssExtendTeleportState( ioBaseChar *pOwner );

public:
	void SetDragonNestEnableTeleport( ioBaseChar *pOwner );

//썬더버드
protected:
	bool CheckEnableThunderBirdState( ioBaseChar * pOwner );
	void SetThunderBirdState( ioBaseChar *pOwner );

//엘리스
protected:
	bool CheckEnableEquipObjectItemState( ioBaseChar *pOwner );
	void SetObjectEquipState( ioBaseChar *pOwner );

//LeeDragonAttack
protected:
	bool CheckEnableLeeDragonAttackState( ioBaseChar *pOwner );
	void SetLeeDragonAttackState( ioBaseChar *pOwner );
	bool DecreaseLeeGauge( ioBaseChar *pOwner );
	void ProcessLeeDragonAttackState( ioBaseChar *pOwner );

//박일표
protected:
	void LoadParkilpyoNormalExtendInfo( ioINILoader &rkLoader );
	void ChangeToParkilpyoNormalGathering( ioBaseChar *pOwner );
	void ProcessParkilpyoNormalGathering( ioBaseChar *pOwner );
	void ChangeToParkilpyoNormalChargeAttack( ioBaseChar *pOwner );
	void ChangeToParkilpyoNormalChargeCombo( ioBaseChar *pOwner );

	bool CheckEnableParkilpyoNormalState( ioBaseChar * pOwner );
	void SetParkilpyoNormalEquipState( ioBaseChar *pOwner );

	bool CheckParkilpyoChargeComboInput( ioBaseChar *pOwner );
	void CheckParkilpyoCancelReserve( ioBaseChar *pOwner );

	void LoadParkilpyoObjectExtendInfo( ioINILoader &rkLoader );
	void ChangeToParkilpyoObjectChargeAttack( ioBaseChar *pOwner );

//잭
protected:
	void LoadJackExtendInfo( ioINILoader &rkLoader );
	bool CheckEnableJackChargeAttack( ioBaseChar *pOwner );
	void ChangeToJackChargeAttack( ioBaseChar *pOwner );

//쉐도우레인저
protected:
	void LoadShadowRangerExtendInfo( ioINILoader &rkLoader );
	bool CheckEnableShadowRangerState( ioBaseChar * pOwner );
	void SetShadowRangerMoveState( ioBaseChar *pOwner );

//점퍼
protected:
	bool CheckEnableJumperState( ioBaseChar * pOwner );
	void SetJumperState( ioBaseChar *pOwner );

//세이버
protected:
	bool CheckEnableSaberExtendState( ioBaseChar * pOwner );
	void SetSaberExtendState( ioBaseChar *pOwner );

	bool CheckEnableNeoExtendState( ioBaseChar *pOwner, bool bHalf );
	void SetNeoExtendState( ioBaseChar *pOwner, bool bHalf );

	bool CheckEnableAutomatonState( ioBaseChar *pOwner );
	void SetAutomatonState( ioBaseChar *pOwner );

	bool CheckEnableVulcanState( ioBaseChar *pOwner );
	void SetVulcanState( ioBaseChar *pOwner );

	bool CheckEnableChangeOnState( ioBaseChar *pOwner );
	void SetChangeOnState( ioBaseChar *pOwner );

	bool CheckEnableChangeOnWolfState( ioBaseChar *pOwner );
	void SetChangeOnWolfState( ioBaseChar *pOwner );

	bool CheckEnableGhostState( ioBaseChar *pOwner );
	void SetGhostState( ioBaseChar *pOwner );

	bool CheckEnableOmenState( ioBaseChar* pOwner );
	void SetOmenState( ioBaseChar* pOwner );

//미드나잇
	bool CheckEnableMidnightState( ioBaseChar* pOwner );
	void SetMidnightState( ioBaseChar* pOwner );
	void LoadMidnightStateInfo( ioINILoader &rkLoader );

//마검사
protected:
	void ChangeToKeyInputAttack( ioBaseChar * pOwner );
	void ProcessKeyInputLastAttack( ioBaseChar *pOwner );
	bool CheckEnableMagicSwordAttackState( ioBaseChar * pOwner );
	void SetMagicSwordAttackState( ioBaseChar *pOwner );
	
//나타
protected:
	bool CheckEnableNataState( ioBaseChar * pOwner );
	void SetNataComboState( ioBaseChar *pOwner );


//에너지연구원
protected:
	bool CheckEnableResearcherRearFlyState( ioBaseChar * pOwner );
	void SetResearcherRearFlyState( ioBaseChar *pOwner );

//딜레이 상태로 변경
protected:
	bool CheckMaxComboSetDelayState( ioBaseChar *pOwner );

//투우사
protected:
	bool CheckEnableBullFightState( ioBaseChar * pOwner );
	void SetBullFightState( ioBaseChar *pOwner );

//Sawang
protected:
	bool CheckEnableSawangState( ioBaseChar *pOwner );
	void SetSawangState( ioBaseChar *pOwner );

//Strider
protected:
	bool CheckEnableStriderExtendState( ioBaseChar * pOwner );
	void SetStriderExtendState( ioBaseChar *pOwner );

//파계승
protected:
	bool CheckEnableApostateMonkExtendState( ioBaseChar * pOwner );
	void SetApostateMonkExtendState( ioBaseChar *pOwner );

//쿠훌린
	void LoadCuchulainInfo( ioINILoader &rkLoader );
	void MiddleAimInit_Cuchulain( ioBaseChar *pOwner );
	void CheckMiddleCharge_Cuchulain( ioBaseChar *pOwner );
	void ClearMiddleAim_Cuchulain( ioBaseChar *pOwner );
	void ChangeCuchulainMiddleAttack( ioBaseChar *pOwner );
	void ProcessCuchulainMiddleAttack( ioBaseChar *pOwner );
	void SetCuchulainSpecialMove( ioBaseChar *pOwner );
	bool IsEnableCuchulainSpecialMove( ioBaseChar *pOwner );
	ioHashString GetCuchulainMiddleTarget( ioBaseChar *pOwner );
	void CuchulainChargeBuffAttack( ioBaseChar *pOwner );

protected:
	void LoadOniMushaInfo( ioINILoader &rkLoader );
	void SetOniMushaObjectEquipState( ioBaseChar *pOwner );
	bool CheckEnableOniMushaHalfChargeState( ioBaseChar *pOwner );
	bool CheckEnableOniMushaRunState( ioBaseChar *pOwner );
	void ChangeToOniMushaRunState( ioBaseChar *pOwner );
	void ChangeToOniMushaChargeAttack( ioBaseChar *pOwner );
	void ProcessOniMushaChargeAttack( ioBaseChar *pOwner );

//드루이드
protected:
	bool CheckEnableDruidBearState( ioBaseChar *pOwner );
	void SetDruidBearState( ioBaseChar *pOwner );

protected:
	bool CheckEnableCrowSpecialDashState( ioBaseChar *pOwner );
	void SetCrowSpecialDashState( ioBaseChar *pOwner );

//블랙스피어
protected:
	void CheckSpecialStateByExtendType( ioBaseChar *pOwner );

//메피스토
protected:
	bool CheckEnableMephistoSpecialDashState( ioBaseChar *pOwner );
	void SetMephistoSpecialDashState( ioBaseChar *pOwner );

//버블 파이터
protected:
	bool CheckEnableBubbleFighterDashAttack( ioBaseChar *pOwner );
	void SetBubbleFighterDashAttack( ioBaseChar *pOwner );
	bool CheckBubbleFighterBullet( ioBaseChar *pOwner );

//Uriel
protected:
	bool CheckEnableUrielChargeDashAttack( ioBaseChar *pOwner );
	void SetUrielChargeDashAttack( ioBaseChar *pOwner );

//풍신
protected:
	bool CheckEnableWindGodDashAttack( ioBaseChar *pOwner );
	void SetWindGodDashAttack( ioBaseChar *pOwner );

	void CheckBaseballBatting( ioBaseChar *pOwner );

//약 D~
protected:
	void ChangeHalfExtendAttack( ioBaseChar *pOwner );

protected:
	bool CheckEnableSasinmuState( ioBaseChar *pOwner );
	void SetSasinmuState( ioBaseChar *pOwner );

protected:
	bool CheckEnablePredatorState( ioBaseChar *pOwner );
	void SetPredatorState( ioBaseChar *pOwner );

//스트레인져
protected:
	bool CheckEnableStrangerState( ioBaseChar *pOwner );
	void SetStrangerState( ioBaseChar *pOwner );

	//해커
protected:
	bool CheckEnableHackerDashAttackState( ioBaseChar *pOwner );
	void SetHackerDashAttackState( ioBaseChar *pOwner );

	void LoadAzazelInfo( ioINILoader &rkLoader );
	void ProcessAzazelAttack( ioBaseChar *pOwner );

	void ChangeToSpecialAttackFire( ioBaseChar *pOwner, int iSpecialState );
	void ProcessSpecialAttack( ioBaseChar *pOwner );

// 헤라클레스
protected:
	bool CheckHeraclesJump_Input_Jump_S( ioBaseChar *pOwner );

	//공용
public:
	bool IsCanPlayWoundedAniFromExtendDash();
	bool IsNoDropState();

	inline DashState GetDashState() { return m_DashState; }

protected:
	bool CheckCanComboDash( ioBaseChar *pOwner );

	void SetRunRotateBuff( ioBaseChar *pOwner );
	void RemoveRundRotateBuff( ioBaseChar *pOwner );

	void InitAniRotate();
	void SetAniRotate( ioBaseChar *pChar, int iAniID, float fAniRate );
public:
	ioChargeComboDash();
	ioChargeComboDash( const ioChargeComboDash &rhs );
	virtual ~ioChargeComboDash();
};

inline ioChargeComboDash* ToChargeComboDash( ioExtendDash *pDash )
{
	if( !pDash || pDash->GetType() != DT_CHARGE_COMBO )
		return NULL;

	return dynamic_cast< ioChargeComboDash* >( pDash );
}

