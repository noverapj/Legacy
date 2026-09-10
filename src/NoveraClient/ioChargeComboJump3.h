#pragma once

#include "ioAttackAttribute.h"
#include "ioExtendJump.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioChargeComboJump3 : public ioExtendJump
{
public:
	enum ChageComboState
	{
		CCS_NONE,
		CCS_NORMAL,
		CCS_CHARGE,
		CCS_EXTEND_ATTACK,
		CCS_DOUBLE,
		CCS_DELAY,							// 5
		CCS_FLASH,
		CCS_FLASH_END,
		CCS_DASH,
		CCS_BUFF_ADD,
		CCS_MECHANICS_FLY,					// 10
		CCS_EXCALIBUR,
		CCS_EXTEND_COMBO,
		CCS_PHANTOM_FLY,
		CCS_NEW_JUMP,
		CCS_LUCIFER_FLY,					// 15
		CCS_NAKORURU_GRAP_ATTACK,
		CCS_DESTROYER,
		CCS_DASH_END,
		CCS_PETERPAN_FLY,
		
		CCS_GHOST_STEALER,					// 20
		CCS_GHOST_STEALER_ADD_ATTACK,
		CCS_GHOST_STEALER_STATE,

		CCS_DEVIL_FLY,

		CCS_SUPER_DIMENSION_DASH,

		CCS_STRIKER_MOVE_ATTACK,			// 25
		CCS_STRIKER_MOVE_END,
		CCS_STRIKER_MOVE_ADD_ATTACK,

		CCS_EQUIP_OBJECT,
		CCS_SHADOW_RANGER,
		CCS_LOOK_AT_DASH,					// 30
		CCS_BLADE,
		CCS_NO_DROP_EXTEND_ATTACK,
		CCS_CUCHULAIN_S_ATTACK,

		CCS_DRUID_EAGLE,
		CCS_VALKI_JUMP,

		CCS_BUBBLE_FIGHTER_SPECIAL_ATTACK,
		CCS_BUBBLE_FIGHTER_CHARGE_ATTACK,

		CCS_STRANGER_SPECIAL_ATTACK,
		CCS_STRANGER_SPECIAL_LOOP,
		CCS_STRANGER_SPECIAL_END,

		CCS_MEGA_AGENT_FLY,
		CCS_SMILE_JOKER_FLY,
		CCS_CHANGE_ON,
		CCS_JUMP_CHARGE,
		CCS_JUMP_CHARGE2,
		CCS_DEFENSE_CHARGE,
	};

	enum JumpExtendType
	{
		JET_NONE,
		JET_BUFF_ADD,
		JET_MECHANICS_FLY,
		JET_EXCALIBUR,
		JET_EXTEND_COMBO,
		JET_PHANTOM_FLY,			// 5
		JET_LUCIFER_FLY,
		JET_NAKORURU,
		JET_DESTROYER,
		JET_PETERPAN,
		JET_GHOST_STEALER,			// 10
		JET_DEVIL,
		JET_SUPER_DIMENSION,
		JET_STRIKER,
		JET_DONQUIXOTE,
		JET_EQUIP_OBJECT,			// 15
		JET_SHADOW_RANGER,
		JET_LOOK_AT_DASH,
		JET_JUMPER,
		JET_GEOMJE,
		JET_HADES,					// 20
		JET_BLADE,
		JET_RED_HOOD,
		JET_APOSTATE_MONK,
		JET_NO_DROP_EXTEND,
		JET_MIR_EXTEND,				// 25
		JET_CUCHULAIN_EXTEND,
		JET_ONIMUSHA,
		JET_ONIMUSHA_OBJECT,
		JET_DRUID_EAGLE,
		JET_PREDATOR,				// 30
		JET_CROW,
		JET_WITCH_DOCTOR,
		JET_BUBBLE_FIGHTER,
		JET_WINDGOD,
		JET_WARP,					// 35
		JET_DRILL,
		JET_STRANGER,
		JET_GARGOYLE,
		JET_JINRAH,
		JET_MEGA_AGENT,				// 40
		JET_SMILE_JOKER,
		JET_CHANGE_ON,
		JET_MURIM_GOSU,
		JET_GHOST,
	};

	enum ComboDashType
	{
		CDT_GRAVITY		= 1,
		CDT_NO_GRAVITY	= 2,
	};

	enum DoubleJumpSync
	{
		DJS_DEFAULT,
		DJS_DOUBLE_JUMP_FLY,
	};

protected:
	ChageComboState		m_ChargeComboState;

	float				m_fJumpSpeedRateFR;
	float				m_fJumpSpeedRateSI;
	float				m_fJumpSpeedRateBG;
	float				m_fExtraJumpPowerRate;

	bool				m_bUsedComboAttack;
	bool				m_bUsedDoubleJump;
	bool				m_bUsedHighJump;
	bool				m_bUsedFlash;
	bool				m_bSecondJump;	

	DWORD				m_dwJumpChargeTime;

// NormalJump Attack
protected:
	AttributeList		m_ComboAttackList;
	AttributeList		m_ChargeComboAttackList;
	AttributeList		m_ComboSecondAttackList;

	int					m_iCurComboCnt;

	DWORD				m_dwEnableComboTime_S;
	DWORD				m_dwEnableComboTime_E;
	DWORD				m_dwInput_Dash_S_StartTime;

	bool				m_bReserveJumpAttack;

protected:
	DWORD				m_dwChargingStartTime;
	DWORD				m_dwJumpChargingStartTime;
	DWORD				m_dwDefenseChargingStartTime;
	DWORD				m_dwHalfChargingTime;
	DWORD				m_dwChargingTime;
	ioHashStringVec		m_ChargingAniList;

protected:
	DWORD				m_dwFireStartTime;
	UniqueObjID			m_GuidEffectID;

// FlashJump Attack
protected:
	CEncrypt<bool>		m_bEnableFlashJump;

	ioHashString		m_FlashDelayAni;

	CEncrypt<float>		m_fFlashDelayRate;
	CEncrypt<float>		m_fFlashForce;
	CEncrypt<float>		m_fFlashGravityAmt;

	DWORD				m_dwFlashDelayEndTime;
	DWORD				m_dwFlashEndTime;
	AttackAttribute		m_FlashJumpAttack;

// DoubleJump
protected:
	CEncrypt<bool>		m_bEnableDoubleJump;

	float				m_fDoubleJumpPower;
	float				m_fDoubleJumpForce;
	float				m_fDoubleJumpEnableHeight;
	float				m_fDoubleJumpAniRate;
	float				m_fDoubleJumpSpeedRateFR;
	float				m_fDoubleJumpSpeedRateSI;
	float				m_fDoubleJumpSpeedRateBG;

	ioHashStringVec		m_vDoubleJumpAniList;

	DWORD				m_dwDoubleJumpEnableTime;
	int					m_iMaxJumpCnt;
	int					m_iCurJumpCnt;
	int					m_iAddJumpCnt;

// Jump Dash
protected:
	CEncrypt<bool>		m_bEnableJumpDash;

	ComboDashType		m_ComboDashType;
	AttackAttribute		m_DashJumpAttack;
	bool				m_bDashJumpAttackCharLookAtLock;
		
	DWORD				m_dwJumpDashStartTime;
	DWORD				m_dwJumpDashEndTime;

	ioHashString		m_JumpDashLandAni;
	float				m_fJumpDashLandAniRate;
	float				m_fJumpDashGravityAmt;

	bool				m_bUseNewJumpTypeDash;
	bool				m_bUsedDashJumpAttack;

protected:
	bool				m_bLookAtLocInputDirRotate;
	float				m_fLookAtLockDashAniRate;
	ioHashStringVec		m_vLookAtLockDashAniList;
	ForceInfoList		m_vLookAtLockDashForceInfoList;


//charge attack
protected:
	CEncrypt<bool>		m_bUsedChargeAttack;
	CEncrypt<bool>		m_bUsedStrikerChargeAttack;

	CEncrypt<float>		m_fChargeGravityRate;
	CEncrypt<float>		m_fChargeJumpPower;
	CEncrypt<float>		m_fChargeBaseRange;
	float				m_fCurCharMoveSpeed;

	D3DXVECTOR3			m_vCharMoveDir;
	DWORD				m_dwFlightTime;

	ioHashString		m_ChargeAttackLandAni;
	float				m_fChargeAttackLandAniRate;
	//AttackAttribute	m_ChargeLandAttack;

// jump extend type
protected:
	JumpExtendType		m_JumpExtendType;

// buff add
	AttackAttribute		m_BuffAddAttribute;
	ioHashStringVec		m_vBuffAddList;

// mechanics fly
	MechanicsFlyInfo	m_JumpMechanicsFly;

// excalibur
	CEncrypt<bool>		m_bUsedExcaliburAtt;
	AttributeList		m_vExcaliburAttList;

	CEncrypt<float>		m_fExcaliburForceAmt;

// extend combo
	AttributeList		m_ExtendAttackList;
	bool				m_bExtendAttackUseAirPower;

// 루시퍼
	LuciferFlyInfo		m_JumpLuciferFly;
	ItemJumpAttackInfo	m_ItemLandAttackInfo;

//싸이
	DestroyerFlyInfo	m_JumpDestroyerFly;

//피터펜
	PeterpanFlyInfo		m_JumpPeterpanFly;

//데비f
	DevilFlyInfo		m_JumpDevilFly;

//블레이드
	BladeFlyInfo		m_JumpBladeFly;

//악령도둑
	DWORD				m_dwFireEndTime;
	DWORD				m_dwKeyReserveTime;
	GhostStealerInfo	m_JumpGhostStealer;

//스트라이커
	AttackAttribute		m_StrkerJumpPowerAttack;
	AttackAttribute		m_StrkerJumpPowerAddAttack;
	DWORD				m_dwStrikerDashStartTime;
	DWORD				m_dwStrikerDashEndTime;
	float				m_fStrikerDasnEndJumpPower;
	float				m_fStrikerDashHitAddJumpPower;
	float				m_fStrikerChargeAttackEnableHeight;

//초시공전사
	//착지를 하지 않으면 계속 dash를 사용 가능해서 추가함
	bool				m_bLanded;
	bool				m_bCanSuperDimentionDash;

//돈키호테때 추가
	bool				m_bUseLandJumpAttack;
	bool				m_bUseExtendJumpAttack;

	float				m_fGravityAmt;

//쉐도우 레인저
protected:
	float				m_fShadowRangerUseGauge;

//RedHood
	DWORD				m_dwRedHoodSpecialMoveMacroTime_S;
	DWORD				m_dwRedHoodSpecialMoveMacroTime_E;

//쿠훌린
	AttackAttribute		m_Cuchulain_S_Attack;
	DWORD				m_Cuchulain_S_EndTime;
	ioHashString		m_stCuchulain_S_Attack_Target;
	float				m_fCuchulain_S_Attack_End_Jump_Power;
	DWORD				m_dwCuchulain_Special_Key_Reserve_Time;
//버블파이터
protected:
	//S
	AttackAttribute		m_BubbleFighterSpecialAttack;
	DWORD				m_dwBubbleFighterSpecialAttack_S;
	DWORD				m_dwBubbleFighterSpecialAttack_E;
	DWORD				m_dwBubbleFighterSpecialAttack_S_Time;
	DWORD				m_dwBubbleFighterSpecialAttack_E_Time;

protected:
	DWORD				m_dwNoDropExtendAttackEndTime;
	float				m_fNoDropExtendAttackEndJumpPower;
	bool				m_bNoDropExtendAttack;

protected:
	float m_fOnimushaChargeUseGauge;
	float m_fOnimushaObjectChargeUseGauge;
	bool m_bJumpDashCombo;
	bool m_bUsedFly;

//Stranger
protected:
	bool m_bEnableDefenseAttack;
	float m_fDefenseAttackEnableHeight;
	DWORD m_dwStrangerSpecialAttackTime;
	DWORD m_dwStrangerSpecialAttackIndex;
	AttackAttribute m_StrangerSpecialAttack;
	ioHashString m_szStrangerSpecialLoopAni;
	float		 m_fStrangerSpecialLoopAniRate;
	float		m_fStrangerSpeicialGravityAmt;

public:
	void Init();

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioExtendJump* Clone();
	virtual JumpType GetType() const;

public:
	void SetCurrComboCnt( int iCurrCnt ){ m_iCurComboCnt = iCurrCnt; }
	int GetCurrComboCnt(){ return m_iCurComboCnt; }

public:
	virtual void InitJump();
	virtual void SetJumpping( ioBaseChar *pOwner, bool bFullTime );
	virtual void ProcessJumpping( ioBaseChar *pOwner, float fHeightGap, float fNeedGauge );
	virtual void ProcessLanding( ioBaseChar *pOwner );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );
	virtual void ApplyJump( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnEndExtendJump( ioBaseChar *pOwner );

	virtual DWORD GetChargingTime( ioBaseChar *pChar );

	virtual bool IsCanJumpingSkill( ioBaseChar *pOwner );

	virtual float GetExtraJumpSpeedRate( const D3DXVECTOR3 &vDir, const D3DXQUATERNION &qtRot ) const;
	virtual void SetLandingState( ioBaseChar *pOwner );

	virtual bool IsEnableJumpDash( ioBaseChar *pOwner );

	virtual bool IsNoProcessMove();
	int GetCurChargeComboState();

protected:
	void LoadPropertyByJumpType( ioINILoader &rkLoader );
	// Normal JumpAttack
public:
	virtual void CheckComboJumpAttack( ioBaseChar *pOwner );
	void SetComboJumpAttack( ioBaseChar *pOwner );

protected:
	void LoadComboList( ioINILoader &rkLoader );
	void LoadChargeComboList( ioINILoader &rkLoader );

	void SetComboChargeJumpAttack( ioBaseChar *pOwner );
	void ProcessComboJumpAttack( ioBaseChar *pOwner );

	void SetChargingState( ioBaseChar *pOwner );
	void ProcessCharging( ioBaseChar *pOwner );

	// charge attack
protected:
	void PowerChargeMove( ioBaseChar *pChar, ioPlayStage *pStage );
	float GetFlightDuration( float fCurGravity );

public:
	void ChangeToPowerChargeJumpAttack( ioBaseChar *pOwner );

public:
	inline float GetChargeGravityRate() const { return m_fChargeGravityRate; }
	inline DWORD GetFlightTime() const { return m_dwFlightTime; }

	bool IsChargeAttackState();

	inline const ioHashString& GetChargeLandAni() const { return m_ChargeAttackLandAni; }
	inline float GetChargeLandAniRate() const { return m_fChargeAttackLandAniRate; }

	virtual void SetNextJumpItemState( ioBaseChar *pOwner, ioBaseChar *pWounder, bool bDefense, ChangeNextItemType type, ioWeapon* pWeapon, float fDamage );
	
	// Double Jump
protected:
	bool IsCanDoubleJump( ioBaseChar *pOwner, float fHeightGap, bool bCharge );
	bool IsCanJumpCharge( ioBaseChar *pOwner, float fHeightGap );
	bool IsCanDefenseCharge( ioBaseChar *pOwner, float fHeightGap );
	bool CheckJumpCharging( ioBaseChar *pOwner );
	bool CheckDefenseCharging( ioBaseChar *pOwner );
	void SetDoubleJump( ioBaseChar *pOwner );
	void SetDoubleJumpAni( ioBaseChar *pOwner );
	void SetJumpChargingState( ioBaseChar *pOwner );
	void SetDefenseChargingState( ioBaseChar *pOwner );

	// Flash Jump
public:
	bool SetFlashJumpAttack( ioBaseChar *pOwner );
	float GetCurFlashForceAmt();
	float GetCurFlashGravityAmt();

	bool IsCanFlashJumpAttack( ioBaseChar *pOwner );

protected:
	void ProcessDelay( ioBaseChar *pOwner );
	void ProcessFlashMove( ioBaseChar *pOwner );
	void ProcessFlashEndMove( ioBaseChar *pOwner );

	// Jump Dash
public:
	bool IsJumpDashState();
	bool IsJumpAttackReserve();
	bool IsJumpNoDirectionKeyState();
	bool IsJumpStrangerSpeialAttackState();

	const ioHashString& GetJumpDashLandAni() const { return m_JumpDashLandAni; }
	float GetJumpDashLandAniRate() const { return m_fJumpDashLandAniRate; }

	float GetCurGravityAmt();

	virtual bool IsEnableJumpLandEndDash();

	bool IsNoProcessGravity();
	bool IsStrikerDashState();
	bool IsUseLandJumpAttack();

protected:
	bool IsCanJumpDash( ioBaseChar *pOwner );

	int GetJumpDashAni( ioBaseChar *pOwner );
	void SetJumpDash( ioBaseChar *pOwner );
	void SetLookAtLockJumpDash( ioBaseChar *pOwner );

	void SetJumpDashEnd( ioBaseChar *pOwner );
	void SetNewJumpTypeDashAttack( ioBaseChar *pOwner, DWORD dwEndTime, DWORD dwReserveTime );

	void ProcessJumpDash( ioBaseChar *pOwner );
	void ProcessJumpDashByNoGravity( ioBaseChar *pOwner );

// buff add
protected:
	bool CheckEnableBuffAddState( ioBaseChar *pOwner );
	void SetBuffAddState( ioBaseChar *pOwner );

// mechanics fly
protected:
	bool CheckEnableMechanicsFlyState( ioBaseChar *pOwner );
	void SetMechanicsFlyState( ioBaseChar *pOwner );

	void LoadMechanicsFlyInfo( ioINILoader &rkLoader );

// excalibur special
protected:
	bool CheckEnableExcaliburSpecialAtt( ioBaseChar *pOwner );
	void SetExcaliburSpecialAtt( ioBaseChar *pOwner );

// extend combo
protected:
	bool CheckEnableExtendCombo( ioBaseChar *pOwner );
	void SetExtendComboState( ioBaseChar *pOwner, bool bRefresh );

	void CheckExtendComboAttack( ioBaseChar *pOwner );
	void ProcessExtendComboAttack( ioBaseChar *pOwner );

// phantom fly
protected:
	bool CheckEnablePhantomFlyState( ioBaseChar *pOwner );
	void SetPhantomFlyState( ioBaseChar *pOwner );
	
	// 루시퍼
protected:
	bool CheckEnableLuciferFlyState( ioBaseChar *pOwner );
	void SetLuciferFlyState( ioBaseChar *pOwner );
	void LoadLuciferFlyInfo( ioINILoader &rkLoader );

	//싸이
protected:
	bool CheckEnableDestroyerFlyState( ioBaseChar *pOwner );
	void SetDestroyerFlyState( ioBaseChar *pOwner );
	void LoadDestroyerFlyInfo( ioINILoader &rkLoader );

	//블레이드
protected:
	bool CheckEnableBladeFlyState( ioBaseChar *pOwner );
	void SetBladeFlyState( ioBaseChar *pOwner );
	void LoadBladeFlyInfo( ioINILoader &rkLoader );

	//피터팬
protected:
	bool CheckEnablePeterpanFlyState( ioBaseChar *pOwner );
	void SetPeterpanFlyState( ioBaseChar *pOwner );
	void LoadPeterpanFlyInfo( ioINILoader &rkLoader );

	// 악령도둑
protected:
	bool CheckEnableGhostStealerState( ioBaseChar *pOwner );
	void SetGhostStealerState( ioBaseChar *pOwner, bool bAddAttack );
	void ProcessGhostStealerAttack( ioBaseChar *pOwner );
	void LoadGhostStealerInfo( ioINILoader &rkLoader );
	void ChangeToGhostStealerState( ioBaseChar *pOwner );

	//초시공전사
protected:
	bool CheckEnableSuperDimensionFireState( ioBaseChar *pOwner );
	void SetSuperDimensionFireState( ioBaseChar *pOwner );

public:
	bool IsCanSuperDimensionDash( ioBaseChar *pOwner );
	void SetSuperDimensionDashState( ioBaseChar *pOwner, ioHashString stTarget = "");
	
	//스트라이커
protected:
	bool CheckStrikerChareJumpAttack( ioBaseChar *pOwner );
	void SetStrikerChargeJumpAttack( ioBaseChar *pOwner );
	void SetStrikerChargeJumpAttackEnd( ioBaseChar *pOwner );
	void SetStrikerDashAddAttack( ioBaseChar *pOwner, ioBaseChar *pWounder );
	void LoadStrikerInfo( ioINILoader &rkLoader );

protected:
	bool CheckEnableDevilFlyState( ioBaseChar *pOwner );
	void SetDevilFlyState( ioBaseChar *pOwner );
	void LoadDevilFlyInfo( ioINILoader &rkLoader );

protected:
	bool CheckEnableEquipObjectItemState( ioBaseChar *pOwner );
	void SetObjectEquipState( ioBaseChar *pOwner );	

	//쉐도우레인저
protected:
	void LoadShadowRangerExtendInfo( ioINILoader &rkLoader );
	bool CheckEnableShadowRangerState( ioBaseChar *pOwner );
	void SetShadowRangerMoveState( ioBaseChar *pOwner );

	//하데스
protected:
	bool CheckEnableHadesState( ioBaseChar *pOwner );
	void SetHadesHalfChargeState( ioBaseChar *pOwner );
	void SetHadesFullChargeState( ioBaseChar *pOwner );

	//RedHood
protected:
	void SetMacroTime( ioEntityGroup* pGrp, int iAniID, float fTimeRate, DWORD dwEndTime );

	//파계승
protected:
	bool IsCanApostateMonkDash( ioBaseChar* pOwner );
	void SetApostateMonkJumpDashAttack( ioBaseChar* pOwner );

	//캐릭터 회전 고정 대쉬
protected:
	void LoadLookAtLockDash( ioINILoader &rkLoader );

protected:
	bool IsEnableInputJumpDashS( ioBaseChar* pOwner );
	void SetInputDashS( ioBaseChar *pOwner );

protected:
	bool CheckEnableJumperAtt( ioBaseChar *pOwner );
	void SetJumperAtt( ioBaseChar *pOwner );

//미르2 전사
protected:
	bool CheckEnableMirExtendState( ioBaseChar *pOwner );
	void ChangeToMirExtendJumpAttack( ioBaseChar* pOwner );

	bool CheckJinrahChargeAttack( ioBaseChar *pOwner, bool bHalf );
	void ChangeToJinrahChargeAttack( ioBaseChar *pOwner, bool bHalf );

//쿠훌린
protected:
	void LoadCuchulainIfo( ioINILoader &rkLoader );
	bool IsCanCuchulainJumpAttack( ioBaseChar *pOwner );
	void ProcessCuchulain_S_Attack( ioBaseChar *pOwner );
	void CuchulainChargeBuffAttack( ioBaseChar *pOwner );

//드루이드
public:
	bool CheckDruidTransformEagleState( ioBaseChar *pOwner );
	void SetDruidTransformEagle( ioBaseChar *pOwner );
	bool CheckDruidTransformBearState( ioBaseChar *pOwner );
	void SetDruidTransformBear( ioBaseChar *pOwner );

	bool CheckChangeOnGriffinState( ioBaseChar *pOwner );
	void SetChangeOnGriffinState( ioBaseChar *pOwner );

	bool CheckEnableMurimGosuState( ioBaseChar *pOwner );
	bool CheckEnableMurimGosuCmdState( ioBaseChar *pOwner );
	void SetMurimGosuState( ioBaseChar *pOwner, int iDir );

	bool CheckGhostState( ioBaseChar *pOwner );
	void SetGhostState( ioBaseChar *pOwner );

public:
	void SetCuchulainJumpAttack( ioBaseChar *pOwner );

//귀무사
protected:
	void SetOniMushaObjectEquipState( ioBaseChar *pOwner );
	bool IsCanJumpDashCombo( ioBaseChar *pOwner );
	bool CheckEnableOniMushaHalfChargeState( ioBaseChar *pOwner );

protected:
	bool CheckEnablePredatorState( ioBaseChar *pOwner );
	void SetPredatorState( ioBaseChar *pOwner );

//부두술사
protected:
	bool CheckEnableWitchDoctorPredatorState( ioBaseChar *pOwner );
	void SetWitchDoctorCallDummy( ioBaseChar *pOwner );

protected:
	void LoadBubbleFighterInfo( ioINILoader &rkLoader );
	bool IsCanBubbleFighterSpecialAttack( ioBaseChar* pOwner );
	void SetBubbleFighterSpecialAttack( ioBaseChar* pOwner );

	bool IsCanBubbleFighterAttack( ioBaseChar* pOwner );
	bool CheckEnableBubbleFighterChargeAttack( ioBaseChar* pOwner );
	void SetBubbleFighterChargeAttack( ioBaseChar* pOwner );

// Warp
protected:
	bool CheckEnableWarpState( ioBaseChar *pOwner );
	void SetWarpState( ioBaseChar *pOwner );

//Drill
protected:
	bool CheckEnableDrillChargeAttackState( ioBaseChar *pOwner );
	void SetDrillChargeAttackState( ioBaseChar *pOwner );

//Stranger
protected:
	void LoadStrangerInfo( ioINILoader &rkLoader );
	bool IsCanDefenseKeyAttack( ioBaseChar *pOwner, float fHeightGap );
	void ChangeToStrangerSpecialAttack( ioBaseChar *pOwner );
	void ProcessStrangerSpecialAttack( ioBaseChar *pOwner );
	void ChangeToStrangerSpecialLoop( ioBaseChar *pOwner );
	void ProcessStrangerSpecialLoop( ioBaseChar *pOwner );

	bool CheckEnableStrangerChargeAttackState( ioBaseChar *pOwner );
	void SetStrangerChargeAttackState( ioBaseChar *pOwner );

	void ChangeToSmileJokerFlyState( ioBaseChar *pOwner );

//Gargoyle
protected:
	bool CheckEnableGargoyleSpecialState( ioBaseChar *pOwner );
	void SetGargoyleSpecialState( ioBaseChar *pOwner );

public:
	bool IsJumpDashComboState();

	int GetCurJumpCount()				{ return m_iCurJumpCnt; }
	void SetCurJumpCount( int iCount )	{ m_iCurJumpCnt = iCount; }

protected:
	bool CheckEnableMegaAgentFlyState( ioBaseChar *pOwner );
	void SetMegaAgentFlyState( ioBaseChar *pOwner );	

public:
	void ResetUsedFly()			{ m_bUsedFly = false; }

public:
	void SetReserveJumpAttack( ioBaseChar *pOwner );

	virtual bool IsDoubleBuffFlyChange( ioBaseChar* pOwner );	
	void ChangeToDoubleJumpFly( ioBaseChar *pOwner );
	virtual bool CheckUseExtendJump( ioBaseChar* pOwner );

protected:
	bool CheckEnableDropExtendAttack( ioBaseChar *pOwner );
	bool CheckEnableCrowExtendAttacck( ioBaseChar *pOwner );
	void ChangeToNoDropExtendAttack( ioBaseChar *pOwner );
	void NoDropExtendAttack( ioBaseChar* pChar, ioPlayStage* pStage );

public:
	ioChargeComboJump3();
	ioChargeComboJump3( const ioChargeComboJump3 &rhs );
	virtual ~ioChargeComboJump3();
};

inline ioChargeComboJump3* ToChargeComboJump3( ioExtendJump *pJump )
{
	if( !pJump || pJump->GetType() != JT_CHARGE_COMBO3 )
		return NULL;

	return dynamic_cast< ioChargeComboJump3* >( pJump );
}

