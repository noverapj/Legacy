#pragma once

#include "ioNormalSkill.h"
#include "ioUserKeyInput.h"
#include "ioAttackAttribute.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioChargingFireSkill6 : public ioNormalSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_PRE_ACTION,
		SS_LOOP,
		SS_ACTION,
		SS_FAIL,
	};

	enum InputType
	{
		IT_ATTACK,
		IT_DEFENSE,
		IT_JUMP,
	};

protected:
	CEncrypt<int> m_State;

	// Attack
	AttackAttribute m_Attribute_a;
	AttackAttribute m_Attribute_s;
	AttackAttribute m_Attribute_d;

	// Buff
	TargetColType m_TargetColType;
	TargetWoundType m_TargetWoundType;

	CEncrypt<float> m_fSkillRange;
	CEncrypt<float> m_fSkillAngle;
	CEncrypt<float> m_fOffSet;

	CEncrypt<float> m_fUpHeight;
	CEncrypt<float> m_fUnderHeight;
	CEncrypt<float> m_fWidth;

	BuffInfoList m_EnemyBuffList;

	ioHashString    m_SkillLoopMotion;
	CEncrypt<float> m_fSkillLoopMotionRate;

	ioHashString    m_szFailAni;
	CEncrypt<float> m_fFailAniRate;

	DWORD m_dwMaxChargingTime;

	DWORD m_dwAttackEndTime;

	ioHashString m_szChargingEffect;
	UniqueObjID m_dwChargingEffect;

	DWORD m_dwFireStartTime;
	DWORD m_dwPreActionEndTime;
	DWORD m_dwLoopStartTime;
	DWORD m_dwLoopDuration;

	CEncrypt<DWORD> m_dwSkillProtectTime;
	
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnSkillEnd( ioBaseChar *pChar );	
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );

	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );

	virtual bool CheckUseItemMesh();

public:
	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;

protected:
	virtual void SetNormalSkill( ioBaseChar *pChar );

public:
	void SetEnemyBuffList( ioBaseChar *pOwner, ioPlayStage *pStage );

protected:
	void SetLoopState( ioBaseChar *pChar );
	void SetActionState( ioBaseChar *pChar, ioPlayStage *pStage, InputType eType );
	void SetFailState( ioBaseChar *pChar );
	void ProcessLoopState(  ioBaseChar *pChar, ioPlayStage *pStage );

public:
	ioChargingFireSkill6();
	ioChargingFireSkill6( const ioChargingFireSkill6 &rhs );
	virtual ~ioChargingFireSkill6();
};

inline ioChargingFireSkill6* ToChargingFireSkill6( ioSkill *pSkill )
{
	ioNormalSkill *pNormal = ToNormalSkill( pSkill );
	if( !pNormal || pNormal->GetSubSkillType() != NST_CHARGING_FIRE6 )
		return NULL;

	return dynamic_cast< ioChargingFireSkill6* >( pSkill );
}

