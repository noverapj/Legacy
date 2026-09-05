#pragma once

#include "ioAttackAttribute.h"
#include "ioExtendJump.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioChargeComboJump2 : public ioExtendJump
{
public:
	enum ChageComboState
	{
		CCS_NONE,
		CCS_NORMAL,
		CCS_CHARGE,
		CCS_EXTEND_ATTACK,
		CCS_DOUBLE,
		CCS_DELAY,
		CCS_FLASH,
		CCS_FLASH_END,
		CCS_DASH,
		CCS_BUFF_ADD,
		CCS_MECHANICS_FLY,							//10
		CCS_EXCALIBUR,
		CCS_EXTEND_COMBO,
		CCS_PHANTOM_FLY,
		CCS_DESTROYER,
		CCS_MICHAEL_FLY,
		CCS_GOOMIHO,
		CCS_GHOST_STEALER,
		CCS_DRACULA,
		CCS_EVY,
		CCS_SLASHER,								// 20
		CCS_TOP_BL,
		CCS_TITAN,
		CCS_TITAN_MOVE_FAIL,
		CCS_TITAN_MOVE_STATE,
		CCS_KAELTHAS_EXTEND,
		CCS_THUNDER_BIRD,
		CCS_PARKILPYO,
		CCS_JACK_CHARGE,
		CCS_EXTRA_JUMP,
		CCS_JACK_EXTRA_JUMP_ATTACK,					//30
		CCS_EXTRA_JUMP_STATE,
		CCS_NEW_JUMP,
		CCS_KEY_INPUT_LAST_ATTACK,
		CCS_MAGIC_SWORD_EXTEND_ATTACK,
		CCS_MAGIC_SWORD_EXTEND_INPUT_KEY_ATTACK,
		CCS_BULLFIGHT_CHARGE,
		CCS_BULLFIGHT_CHARGE_EXTEND,
		CCS_JEONWOOCHI_CHARGE,
		CCS_JEONWOOCHI_CHARGE_FAIL,
		CCS_CHAIN_MAGICIAN_CHARGE,
		CCS_BLACK_SPEAR_DASH,
	};

	enum JumpExtendType
	{
		JET_NONE,
		JET_BUFF_ADD,
		JET_MECHANICS_FLY,
		JET_EXCALIBUR,
		JET_EXTEND_COMBO,
		JET_PHANTOM_FLY,
		JET_DESTROYER,
		JET_MICHAEL_FLY,
		JET_GOOMIHO_FLY,
		JET_SCATHI,
		JET_DRACULA,					//10
		JET_EVY,
		JET_SLASHER,
		JET_TOP_BL,
		JET_TITAN,
		JET_KAELTHAS,
		JET_THUNDER_BIRD,
		JET_PARKILPYO,
		JET_JACK,
		JET_NINJA_EXPANSION,
		JET_ANGEL_OF_DEATH,				//20
		JET_MAGIC_SWORD,
		JET_RESEARCHER_REAR_FLY,
		JET_BULLFIGHT,
		JET_JEONWOOCHI,
		JET_CHMISTRY,//화학병
		JET_SAWANG_SLIDE,
		JET_CHAIN_MAGICIAN,
		JET_BLACK_SPHEAR,
		JET_SURFER,
		JET_REAPER_EXPANSION,			//30
		JET_EXPANSION_LANCER,
		JET_MAGICENGINEER,
		JET_MAGICSTAFF,
		JET_SANTA,
	};

	enum ComboDashType
	{
		CDT_GRAVITY		= 1,
		CDT_NO_GRAVITY	= 2,
	};

	enum NewJumpState
	{
		NJS_NONE,
		NJS_DASH,
		NJS_ADD_ATTACK,
		NJS_DASH_LAND,
		NJS_END,
	};

protected:
	NewJumpState m_NewJumpState;
	DWORD m_dwNewJumpAddAttackReserveTime;
	DWORD m_dwNewJumpEndTime;
	int m_nNewJumpAttackIndex;

	AttributeList m_vNewJumpAddAttackList;

	void SetNewJumpDashAddAttack( ioBaseChar* pOwner, int nIndex );
	void SetJumpDashLand( ioBaseChar* pOwner );

protected:
	ChageComboState m_ChargeComboState;

	float m_fJumpSpeedRateFR;
	float m_fJumpSpeedRateSI;
	float m_fJumpSpeedRateBG;

	bool m_bUsedComboAttack;
	bool m_bUsedDoubleJump;
	bool m_bUsedFlash;

	float m_fExtraJumpPowerRate;
	DWORD m_dwJumpChargeTime;

	bool  m_bCanEvySpecial;

	// NormalJump Attack
protected:
	AttributeList m_ComboAttackList;
	int m_iCurComboCnt;

	DWORD m_dwEnableComboTime_S;
	DWORD m_dwEnableComboTime_E;

	bool m_bReserveJumpAttack;

protected:
	DWORD m_dwChargingStartTime;
	DWORD m_dwChargingTime;
	DWORD m_dwHalfChargingTime;

	ioHashStringVec m_ChargingAniList;

protected:
	DWORD m_dwFireStartTime;
	DWORD m_dwCreateWeaponIndex;

	UniqueObjID m_GuidEffectID;

	// FlashJump Attack
protected:
	CEncrypt<bool> m_bEnableFlashJump;

	ioHashString m_FlashDelayAni;
	CEncrypt<float> m_fFlashDelayRate;

	CEncrypt<float> m_fFlashForce;
	CEncrypt<float> m_fFlashGravityAmt;

	DWORD m_dwFlashDelayEndTime;
	DWORD m_dwFlashEndTime;

	AttackAttribute m_FlashJumpAttack;

	// DoubleJump
protected:
	CEncrypt<bool> m_bEnableDoubleJump;
	bool m_bEnableDoubleJumpAfterDash;

	bool m_bDashEndImmediateAttack;
	float m_fDashEndJumpPower;
	float m_fDoubleJumpPower;
	float m_fDoubleJumpForce;
	float m_fDoubleJumpEnableHeight;
	DWORD m_dwDoubleJumpEnableTime;

	float m_fDoubleJumpAniRate;
	ioHashStringVec m_vDoubleJumpAniList;

	int m_iMaxJumpCnt;
	int m_iCurJumpCnt;

	float m_fDoubleJumpSpeedRateFR;
	float m_fDoubleJumpSpeedRateSI;
	float m_fDoubleJumpSpeedRateBG;

	// Jump Dash
protected:
	ComboDashType m_ComboDashType;

	AttackAttribute m_DashJumpAttack;
	float m_fJumpDashGravityAmt;

	ioHashString m_JumpDashLandAni;
	float m_fJumpDashLandAniRate;
	DWORD m_dwJumpDashLandDelayTime;

	bool m_bUseNewJumpTypeDash;
	bool m_bUseJumpDashRotate;
	bool m_bUseDoubleJumpNewJumpDash;

	//공중 1타후 뉴점프대쉬가 가능한지옵션
	//1타는 하드 코딩
	bool m_bUseAttackAfterNewJumpDash;
	
	CEncrypt<bool> m_bEnableJumpDash;
	int m_iJumpDashMaxCount;
	int m_iJumpDashCurCount;

	float m_fJumpDashAniRate;
	ioHashStringVec m_vJumpDashAniList;

	ForceInfoList m_vForceInfoList;
	DWORD m_dwActionEndTime;

	bool m_bDashEnd;
	bool m_bJumpAttackReserve;
	bool m_bEnableExtendComboColSkip;

	// charge attack
protected:
	CEncrypt<bool> m_bUsedChargeAttack;

	CEncrypt<float> m_fChargeGravityRate;
	CEncrypt<float> m_fChargeJumpPower;
	CEncrypt<float> m_fChargeBaseRange;

	float m_fCurCharMoveSpeed;

	D3DXVECTOR3 m_vCharMoveDir;
	DWORD m_dwFlightTime;

	//진화 닌자
protected:
	DWORD			m_dwEnableNinjaDashTime;

	//메이드
	//weapon에 있는 점프 max숫자만큼 뛰어도 마지막 타는 charge공격은 나가게 셋팅하는거
protected:
	bool m_bMaxJumpLastChargeAttack;

// jump extend type
protected:
	JumpExtendType m_JumpExtendType;

// buff add
	AttackAttribute m_BuffAddAttribute;
	ioHashStringVec m_vBuffAddList;

// mechanics fly
	MechanicsFlyInfo m_JumpMechanicsFly;

// 사이보그
	DestroyerFlyInfo m_JumpDestroyerFly;

// michael fly
	MichaelFlyInfo m_JumpMichaelFly;

// 구미호
	GoomihoFlyInfo m_JumpGoomihoFly;

// 드라큘라
	DraculaFlyInfo m_JumpDracula;

// 이비
	EvyBlinkInfo	m_JumpEvy;
	
// 슬래셔
	SlasherRunInfo m_JumpSlasher;

// 타이탄
	TitanExtendInfo m_JumpTitanExtend;

// 탑블레이트
	std::vector< StTopMoveInfo > m_vTopMoveNormalInfo;
	StTopMoveInfo	m_TopMoveChargeInfo;
	AttackAttribute m_TopBl_ChargeAttack;

//신비술사
	AttackAttribute m_KaelthasExtendAttack;

// 전우치
	AttackAttribute m_JeonWooChiExtendAttack;

//썬더버드
	DWORD m_dwThunderBirdMotionEnd;
	float m_fThunderBirdEndJumpPower;
	float m_fThunderBirdNormalUseGauge;
	float m_fThunderBirdChargeUseGauge;
	bool m_bSetForceActionStopInit;

//박일표
	float m_fParkilpyoChargeUseGauge;

//파괴자
	bool m_bChargeAttacked;
	bool m_bSetChargeAttackEndWait;
	DWORD m_dwChargeAttackEndTime;

//잭
	float m_fJackChargeUseGauge;
	float m_fJackExtraJumpUseGauge;
	float m_fJackExtraJumpUseGaugeDropZone;
	float m_fJackExtraJumpAttackUseGauge;
	AttackAttribute m_JackExtraJumpAttack;
	bool m_bJackExtraJumpReserve;
	float m_fJackExtraJumpAttackGravityAmt;

//추가 A키 기능
	bool m_bEnableExtraJump;
	DWORD m_dwExtraJumpEnableTime;
	float m_fExtrajumpEndPower;
	DWORD m_dwExtrajumpEnableKeyTime;
	bool m_bExtraJumpReserveSkill;
	bool m_bSetExtraJumpForcePower;
	float m_fExtraJumpPower;
	float m_fExtraJumpForce;
	AttackAttribute m_ExtraJump;

//투우사
	float m_fBullFightChargeHeight;
	float m_fBullFightUseGauge;
	AttackAttribute m_BullFightChargeAttack;
	AttackAttribute m_BullFightChargeExtendAttack;

//체인메지션
	float m_fChainMagicianChargeUseGauge;
	float m_fChainMagicianChargeHeight;

// excalibur
	CEncrypt<bool> m_bUsedExcaliburAtt;
	AttributeList m_vExcaliburAttList;

	CEncrypt<float> m_fExcaliburForceAmt;

// 서퍼
	bool m_bUsedSurferCharge;

// extend combo
	AttributeList m_ExtendAttackList;
	AttackAttribute m_ExtendLandAttack;
	bool m_bExtendChnageNewJumpAttackState;

	bool m_bUseExtraSpeedRate;

//MagicEngineer
	DWORD m_dwMESpecialMoveMacroTime_S;
	DWORD m_dwMESpecialMoveMacroTime_E;

protected:
	CEncrypt<bool> m_bEnableJumpComboDash;
	DWORD			m_dwComboDashEndTime;
	int				m_iMaxJumpComboDashCount;
	int				m_iCurJumpComboDashCount;

//마검사
	bool						m_bKeyInputLastAttack;
	AttackAttribute				m_AttackKeyAttribute;
	AttackAttribute				m_DefenseKeyAttribute;
	AttackAttribute				m_JumpKeyAttribute;

	AttackAttribute				m_MagicSwordExtendAttackAttribute;
	AttackAttribute				m_MagicSwordExtendAttackKeyAttribute;
	AttackAttribute				m_MagicSwordExtendDefenseKeyAttribute;
	AttackAttribute				m_MagicSwordExtendJumpKeyAttribute;

	float						m_fDashJumpAttackEndJumpPowerRate;

	//사신 진화
	bool						m_bUsingExpansionReaperJumpChargeAttack;
	DWORD						m_dwEnableReaperExpansionTeleportTime;
	DWORD						m_dwReaperExpansionTeleportTicTime;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioExtendJump* Clone();
	virtual JumpType GetType() const;

public:
	virtual void InitJump();
	virtual void SetJumpping( ioBaseChar *pOwner, bool bFullTime );
	virtual void ProcessJumpping( ioBaseChar *pOwner, float fHeightGap, float fNeedGauge );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );
	virtual void ApplyJump( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnEndExtendJump( ioBaseChar *pOwner );

	virtual DWORD GetChargingTime( ioBaseChar *pChar );

	virtual bool IsCanJumpingSkill( ioBaseChar *pOwner );

	virtual float GetExtraJumpSpeedRate( const D3DXVECTOR3 &vDir, const D3DXQUATERNION &qtRot ) const;
	virtual void SetLandingState( ioBaseChar *pOwner );
		
	bool IsNoProcessGravity();
	int GetCurChargeComboState();

	virtual bool IsEnableJumpDash( ioBaseChar *pOwner );

	virtual bool IsCanJumpAttack( ioBaseChar *pOwner );

	// Normal JumpAttack
public:
	virtual void CheckComboJumpAttack( ioBaseChar *pOwner );

protected:
	void LoadComboList( ioINILoader &rkLoader );

	void SetComboJumpAttack( ioBaseChar *pOwner );
	void ProcessComboJumpAttack( ioBaseChar *pOwner );

	void SetChargingState( ioBaseChar *pOwner, bool bException = false );
	void ProcessCharging( ioBaseChar *pOwner );

	//다크시즈때 추가(뉴점프 공격후 지상에서 연타 구현)
	void ProcessNewJump( ioBaseChar *pOwner );

	// charge attack
protected:
	void ChangeToPowerChargeJumpAttack( ioBaseChar *pOwner );
	void PowerChargeMove( ioBaseChar *pChar, ioPlayStage *pStage );
	float GetFlightDuration( float fCurGravity );

	bool CheckJeonWooChiDummy( ioBaseChar *pOwner );

public:
	inline float GetChargeGravityRate() const { return m_fChargeGravityRate; }
	inline DWORD GetFlightTime() const { return m_dwFlightTime; }

	bool IsChargeAttackState();
	inline const ioHashString& GetChargeLandAni() const { return m_ExtendLandAttack.m_AttackAnimation; }
	inline float GetChargeLandAniRate() const { return m_ExtendLandAttack.m_fAttackAniRate; }

	// Double Jump
protected:
	bool IsCanDoubleJump( ioBaseChar *pOwner, float fHeightGap );
	void SetDoubleJump( ioBaseChar *pOwner );
	void SetDoubleJumpAni( ioBaseChar *pOwner );

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

protected:
	bool IsCanJumpDash( ioBaseChar *pOwner, float fHeightGap );
	bool IsCanJumpDashAfterNormalAttack( ioBaseChar *pOwner, float fHeightGap );
	int GetJumpDashAni( ioBaseChar *pOwner );

	void ProcessJumpDash( ioBaseChar *pOwner );
	
	virtual void CheckKeyReserve( ioBaseChar *pOwner );

	void SetNewJumpTypeDashAttack( ioBaseChar *pOwner, DWORD dwEndTime, DWORD dwReserveTime );

	bool SetDashAttack( ioBaseChar *pOwner,  OUT int &iAniID, OUT float &fTimeRate ); 

// buff add
protected:
	bool CheckEnableBuffAddState( ioBaseChar *pOwner );
	void SetBuffAddState( ioBaseChar *pOwner );

// mechanics fly
protected:
	bool CheckEnableMechanicsFlyState( ioBaseChar *pOwner );
	void SetMechanicsFlyState( ioBaseChar *pOwner );

	void LoadMechanicsFlyInfo( ioINILoader &rkLoader );

//싸이
protected:
	bool CheckEnableDestroyerFlyState( ioBaseChar *pOwner );
	void SetDestroyerFlyState( ioBaseChar *pOwner );
	void LoadDestroyerFlyInfo( ioINILoader &rkLoader );

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

// michael fly
protected:
	bool CheckEnableMichaelFlyState( ioBaseChar *pOwner );
	void SetMichaelFlyState( ioBaseChar *pOwner );

	void LoadMichaelFlyInfo( ioINILoader &rkLoader );

// 구미호
protected:
	bool CheckEnableGoomihoFlyState( ioBaseChar *pOwner );
	void SetGoomihoFlyState( ioBaseChar *pOwner );

	void LoadGoomihoFlyInfo( ioINILoader &rkLoader );

// 스카디
protected:
	bool CheckEnableScathiState( ioBaseChar *pOwner );
	void SetScathiState( ioBaseChar *pOwner );

//화학병
protected:
	bool CheckEnableChemistryState( ioBaseChar *pOwner );
	void SetChemistryState( ioBaseChar *pOwner );

// 드라큘라
protected:
	bool CheckEnableDraculaState( ioBaseChar *pOwner );
	void SetDraculaState( ioBaseChar *pOwner );

	void LoadDraculaInfo( ioINILoader &rkLoader );

// 이비
protected:
	bool CheckEnableEvyBlinkState( ioBaseChar *pOwner );
	void SetEvyBlinkState( ioBaseChar *pOwner );
	void LoadEvyBlinkInfo( ioINILoader &rkLoader );
public:
	void SetEvySpecial( bool b );
	
	
protected:
	bool CheckEnableSlasherState( ioBaseChar *pOwner );
	void SetSlasherState( ioBaseChar *pOwner );

	void LoadSlasherInfo( ioINILoader &rkLoader );

// 탑블레이트
protected:
	void LoadTopBlInfo( ioINILoader &rkLoader );
	bool CheckIsExtend_Top_Bl_mod( ioBaseChar *pOwner );
	void SetTopBlExtendAttack( ioBaseChar *pOwner );
	void SetTopBlNormalAttack( ioBaseChar *pOwner, int nCombo );

// 타이탄
protected:
	void LoadTitanExtendInfo( ioINILoader &rkLoader );
	bool CheckIsEnableTitanExtendAttack( ioBaseChar *pOwner );
	void SetTitanExtendAttack( ioBaseChar *pOwner );
	void ProcessTitanExtendAttack(  ioBaseChar *pOwner );
	D3DXVECTOR3	CheckTitanExtendDir( ioBaseChar *pOwner );
	void TitanExtendMoveFail( ioBaseChar *pOwner, bool bSend );

//신비술사
protected:
	void LoadKaelthasExtendInfo( ioINILoader &rkLoader );
	bool CheckKaelthasExtendAttack( ioBaseChar *pOwner );
	void SetKaelthasExtendAttack( ioBaseChar *pOwner );
	//void ProcessKaelthasExtendAttck( ioBaseChar *pOwner );

//썬더버드
protected:
	void LoadThunderBirdExtendInfo( ioINILoader &rkLoader );
	bool CheckIsEnableThunderBirdExtendAttack( ioBaseChar *pOwner );
	void SetThunderBirdExtendAttack( ioBaseChar *pOwner );
	void ProcessThunderBirdExtendAttack(  ioBaseChar *pOwner );

//박일표
protected:
	void LoadParkilpyoExtendInfo( ioINILoader &rkLoader );
	void SetParkilpyoExtendAttack( ioBaseChar *pOwner );
	void ProcessParkilpyoExtendAttack( ioBaseChar *pOwner );

//파괴자
public:
	bool IsChargeAttacked();
	bool IsChargeEnd();

//잭
protected:
	void LoadJackExtendInfo( ioINILoader &rkLoader );
	bool CheckEnableJackExtendAttack( ioBaseChar *pOwner );
	void SetJackExtendAttack( ioBaseChar *pOwner );
	void ProcessJackExtendAttack( ioBaseChar *pOwner );
	void SetJackExtraJumpAttack( ioBaseChar *pOwner );
	void CheckExtraJumpReserveKeyJack( ioBaseChar *pOwner );
	bool CheckExtraJumpReserveSkill( ioBaseChar *pOwner );
	void CheckDropZoneDecreaseGauge( ioBaseChar *pOwner );

//투우사
protected:
	void LoadBullFightInfo( ioINILoader &rkLoader );
	bool CheckEnableBullFightChargeState( ioBaseChar *pOwner );
	void SetBullFightChargeAttack( ioBaseChar *pOwner );
	void ProcessBullFightChargeAttack( ioBaseChar *pOwner );
	void SetBullFightChargeExtendAttack( ioBaseChar *pOwner );
	void ProcessBullFightChargeExtendAttack( ioBaseChar *pOwner );

//전우치
protected:
	void LoadJeonWooChiChargeInfo( ioINILoader &rkLoader );
	bool CheckEnableJeonWooChiChargeAttack( ioBaseChar *pOwner );
	void SetJeonWooChiExtendAttack( ioBaseChar *pOwner );
	void ProcessJeonWooChiExtendAttack( ioBaseChar *pOwner );
	void JeonWooChiExtendChargeFail( ioBaseChar *pOwner, bool bSend );

//Sawang
protected:
	bool CheckEnableSawangState( ioBaseChar *pOwner );
	void SetSawangState( ioBaseChar *pOwner );

//체인메지션
protected:
	void LoadChainMagicianInfo( ioINILoader &rkLoader );
	bool CheckIsEnableChainMagicianAttack( ioBaseChar *pOwner );
	void SetChainMagicianAttack( ioBaseChar *pOwner );
	void ProcessChainMagicianAttack( ioBaseChar *pOwner );

//닌자진화
protected:
	bool CheckNinjaExtendMove( ioBaseChar *pOwner );

//MagicEngineer
	void SetMacroTime( ioEntityGroup* pGrp, int iAniID, float fTimeRate, DWORD dwEndTime );

//점프중 A키입력시
public:
	virtual void SetExtraJump( ioBaseChar *pOwner );
	virtual void ProcessExtraJump( ioBaseChar *pOwner );

protected:
	bool IsCanExtraJump( ioBaseChar *pOwner );
	void CheckExtraJumpReserveKey( ioBaseChar *pOwner );

public:
	bool IsExtraJumpState();

public:
	const TitanExtendInfo& GetTitanExtendInfo(){ return m_JumpTitanExtend; }

//메이드때 추가
public:
	bool IsCanMaxJumpAndChargeAttack();

//마검사
protected:
	void LoadMagicSwordExtendInfo( ioINILoader &rkLoader );

	void CheckKeyInputLastAttackState( ioBaseChar * pOwner );
	void SetKeyInputLastAttackState( ioBaseChar * pOwner, bool bAttackKey, bool bDefenseKey, bool bJumpKey );
	void ProcessKeyInputLastAttack( ioBaseChar *pOwner );

	bool CheckEnableMagicSwordExtendAttack( ioBaseChar *pOwner );
	void SetMagicSwordExtendAttack( ioBaseChar *pOwner );
	void SetMagicSwordExtendInputKeyAttack( ioBaseChar * pOwner, bool bAttackKey, bool bDefenseKey, bool bJumpKey );
	void ProcessMagicSwordExtendAttack( ioBaseChar *pOwner );
	void ProcessMagicSwordExtendInputKeyAttack( ioBaseChar *pOwner );

//에너지 연구원
protected:
	bool CheckEnableResearcherRearFlyState( ioBaseChar *pOwner );
	void SetResearcherRearFlyState( ioBaseChar *pOwner );

public:
	const AttackAttribute& GetLandAttackAttribute() const  { return m_ExtendLandAttack; }

//블랙스피어
protected:
	void CheckBlackSpearBuffCheckState( ioBaseChar *pOwner );
	void SetBlackSpearJumpComboDashState( ioBaseChar *pOwner );
	bool IsCanComboDash( ioBaseChar *pOwner, float fHeightGap );

//서퍼
protected:
	bool CheckEnableSurferState( ioBaseChar *pOwner );
	void SetSurferState( ioBaseChar *pOwner );

//사신 진화
	bool CheckExpansionReaperJumpChargeAttackState( ioBaseChar *pOwner );
	void ChangeToExpansionReaperJumpChargeAttackState( ioBaseChar *pOwner );
	bool CheckReaperExpansionTeleport( ioBaseChar *pOwner );
	void SetReaperExpansionTeleportState( ioBaseChar *pOwner );

//랜서 진화
	bool CheckExpansionLancerChargeAttack( ioBaseChar *pOwner );
	void ChangeToExpansionLancerChargeAttack( ioBaseChar *pOwner );

	bool CheckMagicEngineerChargeAttack( ioBaseChar *pOwner, bool bCheckProcess );
	void ChangeToMagicEngineerChargeAttack( ioBaseChar *pOwner, bool bHalf );

	bool CheckSantaChargeAttack( ioBaseChar *pOwner );
	void ChangeToSantaChargeAttack( ioBaseChar *pOwner );

public:
	bool GetUsedSurferCharge() { return m_bUsedSurferCharge; }

public:
	bool IsComboDashState();
	void ProcessComboDash( ioBaseChar *pOwner );
public:
	void GetCurGravityAmt( float& fCurGravity );

public:
	ioChargeComboJump2();
	ioChargeComboJump2( const ioChargeComboJump2 &rhs );
	virtual ~ioChargeComboJump2();
};

inline ioChargeComboJump2* ToChargeComboJump2( ioExtendJump *pJump )
{
	if( !pJump || pJump->GetType() != JT_CHARGE_COMBO2 )
		return NULL;

	return dynamic_cast< ioChargeComboJump2* >( pJump );
}

