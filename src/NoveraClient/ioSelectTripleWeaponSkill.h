#pragma once

class ioINILoader;
class ioBaseChar;

#include "ioAttackSkill.h"
#include "ioUserKeyInput.h"


class ioSelectTripleWeaponSkill : public ioAttackSkill
{
public:
	enum SendType
	{
		SNT_CHANGE	= 1,
		SNT_ATTACK	= 2,
	};

	enum SkillState
	{
		SS_NONE,
		SS_LOOP,
		SS_FIRE,
	};

	enum AttackKeyType
	{
		AKT_NONE,
		AKT_JUMP,
		AKT_DEFENSE,
		AKT_ATTACK,
	};

protected:
	struct AttackInfo
	{
		WeaponInfoList m_WeaponInfoList;
		ForceInfoList  m_ForceInfoList;
		AttackInfo()
		{
			m_WeaponInfoList.clear();
			m_ForceInfoList.clear();
		}
	};

protected:
	SkillState    m_SkillState;
	AttackKeyType m_AttackKeyType;

	ioHashString m_SkillLoopMotion;
	DWORD        m_dwSkillLoopDuration;
	
	DWORD m_dwFireStartTime;

	DWORD m_dwSkillProtectTime;

	CEncrypt<float>	m_fAimRange;
	ioHashString    m_szAimMarkEffect;
	UniqueObjID     m_dwAimMarkEffectID;
	TargetWoundType	m_TargetWoundType;

	// TargetList
	BaseCharDistanceSqList	m_vTargetInfoList;
	ioHashString	m_TargetName;

	bool m_bLeftKey;
	bool m_bRightKey;

	ioUserKeyInput::DirKeyInput m_CurDirKey;

	AttackAttribute m_AttackInfo_A;
	AttackAttribute m_AttackInfo_S;
	AttackAttribute m_AttackInfo_D;

	D3DXVECTOR3 m_vAttackDir;

	CEncrypt<int> m_iAWeaponUseCount;
	CEncrypt<int> m_iSWeaponUseCount;
	CEncrypt<int> m_iDWeaponUseCount;
	
protected:
	void ClearData();

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual bool IsEnableReserve() const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool IsProtected( int iDefenseBreakType ) const;

	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );

	virtual bool CheckUseItemMesh();

protected:
	virtual void AttackSkillFire( ioBaseChar *pChar );

protected:
	void SetLoopState( ioBaseChar *pOwner, ioPlayStage *pStage );
	void SetFireState( ioBaseChar *pOwner );

	void CheckKeyInput( ioBaseChar *pOwner );
	void CheckTarget( ioBaseChar *pChar, ioPlayStage *pStage );
	void CheckDir( ioBaseChar *pChar, ioPlayStage *pStage );

protected:
	void UpdateAimMark( ioBaseChar *pChar, ioPlayStage *pStage );
	void DestroyAimMark();

protected:
	inline void SetSkillState( SkillState eState ) { m_SkillState = eState; }
	inline void SetAttackKeyType( AttackKeyType eKeyType ) { m_AttackKeyType = eKeyType; }

public:
	ioSelectTripleWeaponSkill();
	ioSelectTripleWeaponSkill( const ioSelectTripleWeaponSkill &rhs );
	virtual ~ioSelectTripleWeaponSkill();
};

inline ioSelectTripleWeaponSkill* ToSelectTripleWeaponSkill( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_SELECT_TRIPLE_WEAPON )
		return NULL;

	return dynamic_cast< ioSelectTripleWeaponSkill* >( pAttack );
}