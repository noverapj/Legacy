#pragma once

#include "ioAttackSkill.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioBaseChar;
class ioItem;
class SP2Packet;

class ioSpinChargingSkill : public ioAttackSkill
{
public:
	enum SkillChargingState
	{
		SCS_NONE,
		SCS_SPIN,
		SCS_FIRE,
		SCS_FAIL,
	};

	struct ChargeWeaponInfo
	{
		float m_fChargeRate;
		ioHashString m_WeaponEffect;

		ChargeWeaponInfo()
		{
			Init();
		}

		void Init()
		{
			m_fChargeRate = 0.0f;
			m_WeaponEffect.Clear();
		}
	};

	typedef std::vector< ChargeWeaponInfo > ChargeWeaponInfoList;

protected:
	SkillChargingState m_SkillChargingState;

	// spinmotion
	ioHashString m_szSpinLoopAni;
	float m_fSpinLoopAniRate;

	DWORD m_dwSpinDuration;
	DWORD m_dwSpinStartTime;

	DWORD m_dwDefaultSpinSpeed;
	DWORD m_dwCurSpinSpeed;
	DWORD m_dwAccelSpeed;

	// Failmotion
	ioHashString m_szSpinFailAni;
	float m_fSpinFailAniRate;

// change weapon effect
protected:
	bool m_bUseWeaponEffectChange;
	ChargeWeaponInfoList m_vChargeWeaponInfo;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual bool CheckUseItemMesh();

	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );
	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );

	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;

protected:
	void SetSpinLoopState( ioBaseChar *pChar );
	void SetSkillActionState( ioBaseChar *pChar );
	void SetSpinFailState( ioBaseChar *pChar );

	void CheckCurSpin( ioBaseChar *pChar );

	ioHashString CheckChangeWeaponEffect();
	
public:
	ioSpinChargingSkill();
	ioSpinChargingSkill( const ioSpinChargingSkill &rhs );
	virtual ~ioSpinChargingSkill();
};

inline ioSpinChargingSkill* ToSpinChargingSkill( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_SPIN_CHARGING )
		return NULL;

	return dynamic_cast< ioSpinChargingSkill* >(pAttack);
}



