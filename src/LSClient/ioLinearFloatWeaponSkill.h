#pragma once


#include "ioNormalSkill.h"
#include "ioAttackAttribute.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioLinearFloatWeaponSkill : public ioNormalSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_ACTION,
	};

protected:
	CEncrypt<int> m_State;

	// Attack
	DWORD m_dwFireStartTime;

	AttackAttribute m_AttackAttribute;

	// Circle
	float m_fCircleRange;
	float m_fCircleOffSet;
	D3DXVECTOR3 m_vCirclePos;

	float m_fRevisionRate;

	ioHashString m_RedEffect;
	ioHashString m_BlueEffect;

	UniqueObjID m_dwCurMapEffect;

	ioHashString m_StartEffect;
	ioHashString m_EndEffect;

	// Weapon
	int m_iCurWeaponCnt;
	CEncrypt<float> m_fWeaponGravityRate;
	CEncrypt<float> m_fWeaponFloatPower;
	CEncrypt<float> m_fWeaponFloatMinPower;
	CEncrypt<float> m_fWeaponMinSpeed;
	CEncrypt<float> m_fWeaponGapDistance;

	vWeaponMoveInfoList m_vWeaponMoveInfoList;

	float m_fUpLimitHeight;
	float m_fUpLimitRange;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool IsProtected( int iDefenseBreakType ) const;

	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnSkillEnd( ioBaseChar *pChar );	
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );

	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );

	virtual bool CheckUseItemMesh();
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;

protected:
	virtual bool StartPreDelayAnimation( ioBaseChar *pChar );
	virtual void SetNormalSkill( ioBaseChar *pChar );

protected:
	void SetActionState( ioBaseChar *pChar, ioPlayStage *pStage );

	void ProcessAction( ioBaseChar *pChar );

	float GetFlightDuration( float fCurGravity, float fCurWeaponFloatPower, float fGapHeight );
	float GetTargetTime( IN float fCurGravity, IN float fGapHeight, OUT float &fFloatPower );

	void CheckWeaponFloatInfo( ioBaseChar *pOwner );

public:
	ioLinearFloatWeaponSkill();
	ioLinearFloatWeaponSkill( const ioLinearFloatWeaponSkill &rhs );
	virtual ~ioLinearFloatWeaponSkill();
};

inline ioLinearFloatWeaponSkill* ToLinearFloatWeaponSkill( ioSkill *pSkill )
{
	ioNormalSkill *pNormal = ToNormalSkill( pSkill );
	if( !pNormal || pNormal->GetSubSkillType() != NST_LINEAR_FLOAT_WEAPON )
		return NULL;

	return dynamic_cast< ioLinearFloatWeaponSkill* >( pSkill );
}

