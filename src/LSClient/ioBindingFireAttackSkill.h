#pragma once
#include "ioattackskill.h"
class ioBindingFireAttackSkill :	public ioAttackSkill
{
private:
	enum SkillState
	{
		SS_NONE,
		SS_PRE_ATTACK,
		SS_BINDING,
		SS_FIRE,
		SS_END,
	};

	enum KeyState
	{
		KS_ATTACK,
		KS_DEFENSE,
	};

	enum SkillSyncType
	{
		SST_BINDING,
		SST_BINDING_FIRE,
		SST_FIRE,
		SST_END,
	};

private:
	SkillState			m_SkillState;

	DWORD				m_dwProtectTime;
	DWORD				m_dwProtectEndTime;

	//Pre Attack
	AttackAttribute		m_PreAttackAttribute;

	//Binding Attack
	AttackAttribute		m_BindingAttackAttribute;
	WeaponInfo			m_BindingWeaponInfo;
	DWORD				m_dwFireWeaponIndex;

	//Fire
	DWORD				m_dwFireStartTime;
	DWORD				m_dwCurFireAttackWeaponLiveTime;
	float				m_fCurFireAttackWeaponSpeed;
	D3DXVECTOR3			m_vFireAttackWeaponDir;

	AttackAttribute		m_AttackKeyFireAttackAttribute;
	float				m_fAttackKeyFireWeaponAngle;
	float				m_fAttackKeyFireAttackWeaponSpeed;
	DWORD				m_dwAttackKeyFireAttackWeaponLiveTime;
	ioHashStringVec		m_vAttackKeyFireAttackBuffList;

	AttackAttribute		m_DefenseKeyFireAttackAttribute;
	float				m_fDefenseKeyFireWeaponAngle;
	float				m_fDefenseKeyFireAttackWeaponSpeed;
	DWORD				m_dwDefenseKeyFireAttackWeaponLiveTime;
	ioHashStringVec		m_vDefenseKeyFireAttackBuffList;

	//Buff Check
	ioHashStringVec		m_vCheckBuffList;
	

	
private:
	void ClearData();
		
	void SetPreAttackState( ioBaseChar *pOwner );
	void SetBindingAttackState( ioBaseChar *pOwner );
	void SetFireState( ioBaseChar *pOwner, ioPlayStage *pStage, KeyState CurKeyState );
	void SetFireDir( ioBaseChar *pOwner, KeyState CurKeyState );
	void SetActionAni( const AttackAttribute& rkAttri, ioBaseChar *pOwner, bool bRefreshFireTimeList );
	void SetEndState( ioBaseChar *pOwner, bool bDestroyWeapon );


	void FireBindingWeapon( ioBaseChar *pOwner );
	void ShootWeapon( ioBaseChar *pOwner );
	
	void AddFireBuff( ioBaseChar *pOwner, ioPlayStage *pStage, KeyState CurKeyState );
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();
public:
public:
	virtual bool OnSkillStart( ioBaseChar *pOwner );
	virtual void OnProcessState( ioBaseChar *pOwner, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pOwner );

	virtual void ApplyExtraSkillInfo( ioBaseChar *pOwner, ioPlayStage *pStage, SP2Packet &rkPacket );

	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
public:
	ioBindingFireAttackSkill(void);
	ioBindingFireAttackSkill( const ioBindingFireAttackSkill &rhs );
	~ioBindingFireAttackSkill(void);
};

inline ioBindingFireAttackSkill* ToBindingFireAttackSkill( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_BINDING_FIRE_ATTACK )
		return NULL;

	return dynamic_cast< ioBindingFireAttackSkill* >( pAttack );
}