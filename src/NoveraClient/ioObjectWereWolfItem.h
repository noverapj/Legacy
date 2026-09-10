#pragma once

#include "ioObjectItem.h"
#include "ioAttackAttribute.h"
#include "ioSpecialStateBase.h"
#include "ioRachelItem.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioObjectWereWolfItem : public ioObjectItem
{
public:
	enum WolfDefenseType
	{
		WDT_NONE,
		WDT_NORMAL,
		WDT_DISABLE,
	};
	enum
	{
		eLand,
		eSpecial,
	};
	enum SylphidType
	{
		ST_NotUse,
		ST_Up,
		ST_Down,
	};

protected:
	ioHashStringVec m_AttackReadyAniList;
	AttributeList m_vExtendAttributeList;
	AttributeList m_vChargeComboAttList;

protected:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_GATHERING,
		CS_RELEASE,
		CS_ATTACK_FIRE,
		CS_SPECAIL,
		CS_COMBO,
		CS_JUMP_CHARGING,
		CS_JUMP_GATHERING,
		CS_JUMP_ATTACK,
		CS_JUMP_END_ATTACK,

		SS_SPEEDCHANGE,
		SS_ROTATE,
		// Sync State
		SS_JUMP_D_ATTACK
	};

	ChargeState m_ChargeState;
	DWORD m_dwAttackStartTime;
	int m_iCurCombo;
	int m_iChargeCombo;

	bool m_bCharged;
	bool m_bCheckReleaseWereWolf;

	CEncrypt<bool> m_bEnableMaxCharge;
	ioHashString m_GatherAnimation;
	float m_fGatherAniRate;

	DWORD m_dwGatheringStartTime;

	ioHashString m_JumpGatherAnimation;
	ioHashString m_JumpGatheringEffect;
	ioHashString m_JumpGatheringMaxEffect;
	ioHashString m_JumpGatheringMaxSound;	
	ioHashString m_JumpGatheringMoveAnimation;
	UniqueObjID m_JumpGatheringEffectID;
	UniqueObjID m_JumpGatheringMaxEffectID;
	DWORD m_dwJumpGatheringMaxTime;
	float m_fJumpGatherAniRate;
	bool m_bSetJumpGatheringMoveAni;
	bool m_bJumpGatherMax;

	DWORD m_dwJumpChargeAttackLoopTime;
	DWORD m_dwJumpChargeAttackLoopRateTime;
	DWORD m_dwJumpChargeEndTime;
	float m_fJumpChargeRunSpeed;
	float m_fJumpChargeRate;
	ioHashStringVec m_JumpChargeActionBuffList;
	AttackAttribute m_JumpChargeAttack;
	AttackAttribute m_JumpChargeMaxAttack;
	AttackAttribute m_JumpChargeEndAttack;
	AttackAttribute m_JumpChargeMaxEndAttack;
	AttackAttribute m_JumpChargeEnd;

	// Rotate
	bool m_bTargetRot;
	bool m_bLeftRot;
	DWORD m_dwRotateTime;
	ioUserKeyInput::DirKeyInput m_CurDirKey;

	WolfDefenseType m_WolfDefenseType;

	WeaponInfo m_szLandAreaWeapon;
	WeaponInfo m_szSpecialAttackAreaWeapon;

	int m_iUseSylphidType;
	bool m_bEnableSylphid;	
	SylphidInfo m_SylphidInfo;

	bool m_bEnableSpeedChange;
	bool m_bUseSpeedChange;
	bool m_bEnableDefenseRelease;
	bool m_bEnableJumpCharge;
	float m_fChangeSpeed;
	DWORD m_dwJumpChargeTime;

	CEncrypt<DWORD> m_dwStartComboTime;
	CEncrypt<DWORD> m_dwComboAttackTimeEnd;
	CEncrypt<DWORD> m_dwMaxComboDuration;

	// [Dr Spider] ( 이 아래 변수들은 Dr Spider용병 전용으로 사용함)
	// 추가 AttackAttribute
	//AttackAttribute m_D_AttackAttribute;
	//DWORD	m_dwAttackKeyReservTime;

	////한화면에 해당 웨폰이 해당 숫자 이상 존재 제한
	//int     m_n_D_Attack_Max_Weapon_Cnt;
	//int		m_n_D_Attack_Type;

	// air_jump
protected:
	float m_fJumpAttackAirPower;
	float m_fJumpAttackAirForcePower;
	float m_fJumpAttackAirPowerRate;

	float m_fDummyReleaseAirPower;
	float m_fDummyReleaseAirForcePower;
	float m_fDummyReleaseAirPowerRate;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();
	virtual void OnReleased( ioBaseChar *pOwner );
	virtual void OnEquiped( ioBaseChar *pOwner );

public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );

	virtual void OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem=false );
	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );
	virtual bool IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck );

	virtual bool SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );

	virtual bool CheckLanding( ioBaseChar *pChar );
	virtual void SetJumpping( ioBaseChar *pOwner, bool bFullTime );

	virtual float GetAttackSpeedRate() const;
	void SetDefenseSpeedChange( ioBaseChar *pOwner );

	virtual DWORD GetJumpChargeTime( ioBaseChar *pChar ) const;

public:
	virtual ObjectSubType GetObjectSubType() const;

	virtual bool IsOneTimeWeaponItem() const;
	virtual bool IsEnableDash() const;
	virtual bool IsEnableDefense() const;

	inline bool IsReleaseWereWolf() const { return m_bCheckReleaseWereWolf; }

	inline bool IsEnableSylphid() { return m_bEnableSylphid; }
	inline int GetUseSylphidType() { return m_iUseSylphidType; }
	inline const SylphidInfo& GetSylphidInfo() const { return m_SylphidInfo; }
	void SetUseSylphidType( int iType );

	inline bool IsEnableSpeedChange() { return m_bEnableSpeedChange; }
	inline bool IsEnableDefenseRelease() { return m_bEnableDefenseRelease; }
	inline bool IsEnableJumpCharge() { return m_bEnableJumpCharge; }
	bool IsUseGaugeMode();	

	void ChangeToComboState( ioBaseChar *pOwner );
	void ChangeToGriffin( ioBaseChar *pOwner );
	void ChangeToRelease( ioBaseChar *pOwner );
	void ChangeToAttackFire( ioBaseChar *pOwner, bool bCharged );
	void ChangeToJumpChargeAttackFire( ioBaseChar *pOwner );
	void CheckAirJumpAttack(ioBaseChar *pOwner );
	void CheckDummyReleaseJump(ioBaseChar *pOwner );
	void SetJumpChargeState( ioBaseChar *pOwner );
	void SetJumpGatheringState( ioBaseChar *pOwner );
	void SetJumpChargeEndAttackState( ioBaseChar *pOwner, bool bAttack );

protected:
	void OnCharging( ioBaseChar *pOwner );
	void OnGathering( ioBaseChar *pOwner );
	void OnComboState( ioBaseChar *pOwner );
	void OnJumpGathering( ioBaseChar *pOwner );

	void ChangeToGathering( ioBaseChar *pOwner );		

	virtual void CheckReserve( ioBaseChar *pOwner );
	virtual bool ProcessCancel( ioBaseChar *pOwner );

	void CreateAreaWeapon( ioBaseChar *pChar, int iType );
	void LoadSylphidInfo( ioINILoader &rkLoader );

	void SetNextComboState( ioBaseChar *pOwner );
	void CheckJumpGatheringMoveState( ioBaseChar *pOwner );

	void CheckKeyInput( ioBaseChar *pOwner );
	void ProcessRotate( ioBaseChar *pOwner );

	virtual void ProcessJumpping( ioBaseChar *pOwner, float fHeightGap );

public:
	ioObjectWereWolfItem();
	ioObjectWereWolfItem( const ioObjectWereWolfItem &rhs );
	virtual ~ioObjectWereWolfItem();
};

inline ioObjectWereWolfItem* ToObjectWereWolfItem( ioItem *pItem )
{
	ioObjectItem *pObject = ToObjectItem( pItem );
	if( !pObject || pObject->GetObjectSubType() != ioObjectItem::OST_WEREWOLF )
		return NULL;

	return dynamic_cast< ioObjectWereWolfItem* >( pItem );
}

class ioBubbleObjectSpecialState : public ioSpecialStateBase
{
private:
	enum GardenerAttackState
	{
		GAS_DELAY,
		GAS_MOVE,
		GAS_END,
	};

	GardenerAttackState m_State;

	ioHashString m_szStartAni;
	ioHashString m_szEndAni;
	float m_fStartAniRate;
	float m_fEndAniRate;

	float m_fCircleMoveSpeed;
	float m_fCircleHeightGap;
	float m_fCircleOffSet;
	D3DXVECTOR3 m_vCirclePos;
	D3DXVECTOR3 m_vCircleMoveDir;
	ioHashString m_ReadyCircle;

	float m_fFlightDuration;
	float m_fCharMoveSpeed;
	float m_fCurCharMoveSpeed;
	float m_fCharJumpPower;
	float m_fSkillGravityRate;
	D3DXVECTOR3 m_vCharMoveDir;
	DWORD m_dwActionStartTime;
	DWORD m_dwMotionEndTime;

	WeaponInfoList m_vWeaponInfoList;

protected:
	float GetFlightDuration( float fCurGravity );

	void CheckControl( ioBaseChar *pChar );
	void ProcessControl( ioBaseChar *pCChar, ioPlayStage *pStage );

	void CheckMove( ioBaseChar *pChar, ioPlayStage *pStage );
	void ProcessMove( ioBaseChar *pChar, ioPlayStage *pStage );

	bool CheckLanding( ioBaseChar *pChar );
	void SetEndState( ioBaseChar *pChar );

	void ProcessEnd( ioBaseChar *pChar );

public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_BUBBLE_OBJECT_SPECIAL"; }
	virtual const CharState GetState(){ return CS_BUBBLE_OBJECT_SPECIAL; }	
	virtual bool IsEnableBlowWounded( ioBaseChar *pOwner ){ return false; }

public:
	virtual void CheckSpecialState( ioBaseChar* pOwner );
	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );
	virtual void LoadProperty( ioINILoader &rkLoader );

	virtual void ApplyStateSync( SP2Packet &rkPacket, ioBaseChar* pOwner );

public:
	ioBubbleObjectSpecialState();
	virtual ~ioBubbleObjectSpecialState();
};

inline ioBubbleObjectSpecialState* ToBubbleObjectSpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_BUBBLE_OBJECT_SPECIAL )
		return NULL;

	return dynamic_cast< ioBubbleObjectSpecialState* >( pState );
}