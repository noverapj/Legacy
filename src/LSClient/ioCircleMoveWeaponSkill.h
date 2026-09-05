#pragma once

#include "ioNormalSkill.h"
#include "ioAttackAttribute.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioCircleMoveWeaponSkill : public ioNormalSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_WAIT,
		SS_ACTION,
	};

protected:
	CEncrypt<int> m_State;

	// Attack
	DWORD m_dwFireStartTime;

	AttackAttribute m_AttackAttribute;

	// Circle
	float m_fCircleRange;
	float m_fCircleMoveSpeed;
	float m_fCircleHeightGap;
	float m_fCircleOffSet;

	D3DXVECTOR3 m_vCirclePos;
	D3DXVECTOR3 m_vAttackDir;
	float m_fMoveRange;

	ioHashString m_ReadyCircle;
	ioHashString m_TargetCircleEffect;

	ioHashString m_RedEffect;
	ioHashString m_BlueEffect;

	UniqueObjID m_dwCurMapEffect;
	UniqueObjID m_dwTargetCircleEffect;

	ioHashString m_StartEffect;
	ioHashString m_EndEffect;

	// Weapon
	int m_iCurWeaponCnt;
	CEncrypt<float> m_fWeaponFloatPower;
	CEncrypt<float> m_fWeaponFloatMinPower;
	CEncrypt<float> m_fRandomRange;

	vWeaponMoveInfoList m_vWeaponMoveInfoList;
	BuffInfoList m_OwnerBuffList;

	DWORD m_dwSkillProtectedTime;
	DWORD m_dwCurSkillProtectedTime;

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
	void SetWaitState( ioBaseChar *pChar );
	void SetActionState( ioBaseChar *pChar, ioPlayStage *pStage );

	void ProcessAction( ioBaseChar *pChar );

	float GetFlightDuration( float fCurGravity, float fCurWeaponFloatPower );

protected:
	void LoadOwnerBuffList( ioINILoader &rkLoader );
	void AddOwnerBuff( ioBaseChar *pChar );

	bool CheckUseBuff( ioBaseChar *pChar );

public:
	ioCircleMoveWeaponSkill();
	ioCircleMoveWeaponSkill( const ioCircleMoveWeaponSkill &rhs );
	virtual ~ioCircleMoveWeaponSkill();
};

inline ioCircleMoveWeaponSkill* ToCircleMoveWeaponSkill( ioSkill *pSkill )
{
	ioNormalSkill *pNormal = ToNormalSkill( pSkill );
	if( !pNormal || pNormal->GetSubSkillType() != NST_CIRCLE_MOVE_WEAPON )
		return NULL;

	return dynamic_cast< ioCircleMoveWeaponSkill* >( pSkill );
}

