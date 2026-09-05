#pragma once

#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"
#include "ioSpecialStateBase.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioNinjaExpansionItem : public ioWeaponItem
{
public:
	enum SpecialMoveState
	{
		SM_NONE,
		SM_MOVE,
		SM_LANDING,
	};

private:

	typedef struct tagNinjaExpansionMoveInfo
	{
		ioHashString	m_MoveAni;
		float			m_fMoveAniRate;
		float			m_fMoveAngle;

		float			m_fMoveMaxRange;
		float			m_fMoveSpeed;

		//혹시 몰라서
		DWORD			m_dwMaxMoveTime;

		float			m_fEndJumpPower;

		ioHashString	m_MoveLandingAni;
		float			m_MoveLandingAniRate;

		float			m_fLandingCheckHeight;

		tagNinjaExpansionMoveInfo()
		{
			Init();
		}

		void Init()
		{
			m_fMoveAniRate = FLOAT1;
			m_fMoveAngle = 0.f;
			m_fMoveMaxRange = 0.f;
			m_fMoveSpeed = FLOAT1;

			m_fEndJumpPower = FLOAT1;

			m_MoveLandingAniRate = FLOAT1;

			m_fLandingCheckHeight = FLOAT10;
		}
	} NinjaExpansionMoveInfo;


protected:
	ioHashStringVec m_AttackReadyAniList;
	AttributeList m_vExtendAttributeList;
	AttackAttribute m_ExtendMaxAttribute;
	AttackAttribute m_RemoveMaxAttribute;

	//////////////////////////////////////////////////////////////////////////
	//진화 닌자 특수 이동
	NinjaExpansionMoveInfo m_LandMoveInfo;
	NinjaExpansionMoveInfo m_JumpMoveInfo;

	float m_fMoveCurRange;
	DWORD m_dwMoveStartTime;

	//true면 대각선 아래로
	bool m_JumpMove;
	//////////////////////////////////////////////////////////////////////////

protected:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_GATHERING,
		CS_ATTACK_FIRE,
		CS_NORMAL_ATTACK,
		CS_RETREAT,
	};

	SpecialMoveState m_SpecialMoveState;
	DWORD	m_dwSpecialMoveLandingEndTime;

	ChargeState m_ChargeState;
	DWORD m_dwAttackStartTime;
	int m_iCurCombo;

	// PowerCharge
	ioHashString m_GatherAnimation;
	float m_fGatherAniRate;

	ioHashString m_GatheringEffect;
	UniqueObjID m_GatheringEffectID;

	DWORD m_dwGatheringStartTime;

	ioHashString m_ChargingMoveAnimation;
	bool m_bSetChargingMoveAni;

	int m_iDisableWeaponType;

	// max
	CEncrypt<bool> m_bEnableMaxCharge;

	ioHashString m_GatheringMaxEffect;
	UniqueObjID m_GatheringMaxEffectID;
	ioHashString m_GatheringMaxSound;
	ioHashStringVec m_GatheringMaxBuff;

	// retreat attack
	AttackAttribute m_RetreatAttribute;
	DWORD m_dwRetreatEndTime;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );

	virtual void OnReleased( ioBaseChar *pOwner );

	virtual void OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem=false );
	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );
	virtual bool IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck );
	virtual bool IsEnableAttackRotateCheck( ioBaseChar *pOwner );

	virtual void GetAutoTargetValue( ioBaseChar *pOwner,
		float &fRange, float &fMinAngle, float &fMaxAngle,
		AutoTargetType eType );

public:
	virtual WeaponSubType GetSubType() const;
	bool IsCharCollisionSkipState();

protected:
	void OnCharging( ioBaseChar *pOwner );
	void OnGathering( ioBaseChar *pOwner );
	void OnRetreatState( ioBaseChar *pOwner );

	void ChangeToGathering( ioBaseChar *pOwner );
	void ChangeToAttackFire( ioBaseChar *pOwner, bool bCharged );
	void ChangeToRetreatAttack( ioBaseChar *pOwner );
	void ApplyAttackFire( ioBaseChar *pOwner, bool bCharged, bool bMaxAttack, bool bMaxBuffRemove );

	void CheckChargingMoveState( ioBaseChar *pOwner );

	bool IsChargeAutoTarget( ioBaseChar *pOwner );
	bool CheckHasMaxBuff( ioBaseChar *pOwner );

public:
	virtual void ClearCurChargeRateForSkill();

	virtual float GetChargeDamageRateForSkill();
	virtual float GetChargeForceRateForSkill();
	virtual float GetChargeBlowRateForSkill();
	virtual float GetChargeRangeRateForSkill();
	virtual float GetChargeSpeedRateForSkill();
	virtual float GetChargeFloatRateForSkill();
	virtual float GetChargeScaleRateForSkill();
	virtual float GetChargeExplosionRateForSkill();

public:
	void SetNinjaExpansionMove( ioBaseChar *pOwner, bool bJump );
	void ProcessFlyState( ioBaseChar *pOwner, float fHeightGap );
	int GetSpecialMoveState();
private:
	bool ProcessNinjaSpecialMoveCheck( ioBaseChar *pOwner, float fHeightGap );

	void SetSpecialMoveLanding( ioBaseChar *pOwner );
	void ProcessNinjaSpecailMoveLanding( ioBaseChar *pOwner );

public:
	ioNinjaExpansionItem();
	ioNinjaExpansionItem( const ioNinjaExpansionItem &rhs );
	virtual ~ioNinjaExpansionItem();
};

inline ioNinjaExpansionItem* ToNinjaExpansionItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_NINJA_EXPANSION_ITEM )
		return NULL;

	return dynamic_cast< ioNinjaExpansionItem* >( pItem );
}

//-----------------------------------------------------------------------------------------------------------

class ioNinjaExpansionSpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_NINJA_EXPANSION_MOVE"; }
	virtual const CharState GetState(){ return CS_NINJA_EXPANSION_MOVE; }
	virtual bool IsEnableBlowWounded( ioBaseChar *pOwner ){ return false; }

public:
	virtual bool IsCharCollisionSkipState( const ioBaseChar *const pOwner,
		DWORD dwStartTime,
		DWORD dwEndTime,
		DWORD dwSkipType,
		bool bTeamOnly,
		bool bDefense,
		TeamType eOwnerTeam,
		TeamType eOtherTeam ) const;
	virtual void CheckSpecialState( ioBaseChar* pOwner );
	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );
	virtual bool ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime );

public:
	ioNinjaExpansionSpecialState();
	virtual ~ioNinjaExpansionSpecialState();
};

inline ioNinjaExpansionSpecialState* ToNinjaExpansionSpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_NINJA_EXPANSION_MOVE )
		return NULL;

	return dynamic_cast< ioNinjaExpansionSpecialState* >( pState );
}