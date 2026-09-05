#pragma once

class ioINILoader;
class ioBaseChar;

#include "ioAttackSkill.h"
#include "ioUserKeyInput.h"

class ioTargetThrowWeaponSkill : public ioAttackSkill
{
public:
	enum SkillState
	{
		SS_None,
		SS_Action,
		SS_Create_Weapon,
		SS_End,
	};

protected:
	DWORD m_dwSkillProtectTime;
	ioHashString m_szResultMarker;

	int m_nMaxAttackCnt;
	AttributeList m_vAttackList;

	float m_fWeaponTargetFloatPower;
	float m_fWeaponTargetFloatMinPower;

	float m_fWeaponDefaultFloatPower;
	float m_fWeaponDefaultFloatMinPower;

	float m_fTargetRange;
	float m_nTargetMaxCnt;

	Vector3Vec m_DefaultList;

protected:
	SkillState m_SkillState;
	DWORD m_dwCurSkillProtectTime;

	int m_nCurAttackCnt;
	DWORD m_dwFireStart;

	Vector3Vec m_TargetList;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );

	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;

protected:
	void Init();

protected:
	void SetAttackState( ioBaseChar *pChar, ioPlayStage *pStage, const AttackAttribute &sAttribute );
	void SetEndState( ioBaseChar *pChar );
	
protected:
	DWORD CreateEffect( ioPlayStage *pStage, ioHashString& szEffect, D3DXVECTOR3& vPos, D3DXVECTOR3& vScale );

protected:
	void CreateWeapon( ioBaseChar *pChar, ioPlayStage *pStage );
	bool GetTargetList( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetWeaponFloatValue( ioBaseChar *pChar, ioPlayStage *pStage, ioWeapon *pWeapon, D3DXVECTOR3 vTargetPos, bool bTarget );
	float GetFlightDuration( float fCurGravity, float fCurWeaponFloatPower );

public:
	ioTargetThrowWeaponSkill();
	ioTargetThrowWeaponSkill( const ioTargetThrowWeaponSkill &rhs );
	virtual ~ioTargetThrowWeaponSkill();
};

inline ioTargetThrowWeaponSkill* ToTargetThrowWeaponSkill( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_TARGET_THROW_WEAPON )
		return NULL;

	return dynamic_cast< ioTargetThrowWeaponSkill* >( pAttack );
}