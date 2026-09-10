#pragma once

#include "ioAttackSkill.h"
#include "ioAttackAttribute.h"
#include "ioUserKeyInput.h"

class ioINILoader;
class ioBaseChar;
class ioItem;
class SP2Packet;

class ioLinearComboSkill2 : public ioAttackSkill
{
public:
	enum ProtectCheckType
	{
		PCT_TIME	= 1,
		PCT_ANI		= 2,
	};

	enum SkillState
	{
		SS_NONE,
		SS_ACTION,
		SS_COMBO,
		SS_RETREAT,
		SS_LAST_ATTACK,
	};

protected:
	SkillState m_SkillState;

	AttributeList m_vComboList;
	AttributeList m_vJumpComboList;
	AttributeList m_vDownComboList;

	AttackAttribute m_LastAttack;

	int m_iCurLinearCombo;
	DWORD m_dwNextComboTime;

	DWORD m_dwSkillProtectTime;
	DWORD m_dwCurSkillProtectTime;

	bool m_bAutoComboAction;

	ProtectCheckType m_ProtectCheckType;

	// end other attack
	bool m_bUseEndOtherAttack;
	bool m_bUsedEndOtherAttack;

	AttackAttribute m_EndOtherAttack;
	AttackAttribute m_JumpEndOtherAttack;
	AttackAttribute m_DownEndOtherAttack;

	//
	CEncrypt<bool> m_bNoGravityCombo;
	CEncrypt<bool> m_bUsedComboList;
	CEncrypt<bool> m_bSetAttackFire;

	ioUserKeyInput::DirKeyInput m_CurDirKey;

	// retreat
	AttackAttribute m_RetreatAttack;

	CEncrypt<int> m_iMaxRetreatCnt;
	CEncrypt<int> m_iCurRetreatCnt;

	//Desperado
	ioHashString	m_stTargetName;

	bool			m_bUseChangeDirection;
	bool			m_bCheckJumpNoDrop;
	bool			m_bUseEndJumpPower;

	DWORD			m_dwAreMapEffectID;
	ioHashString	m_stAreaMapEffect;
	float			m_fAreaMapEffectLookOffset;

protected:
	bool m_bUseEndCombo;
	int m_nCurEndCombo;
	int m_nMaxEndCombo;
	bool m_bReserveLastAttack;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual bool CheckUseItemMesh();

	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );

	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsEnableReserve() const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;

	virtual bool IsJumpState() const;
	virtual bool IsNoDropState() const;
	virtual bool IsCanSendControl() const;

	virtual bool IsEnableDashCheck( ioBaseChar *pChar );
	virtual bool IsAttackEndState() const;

protected:
	virtual void AttackSkillFire( ioBaseChar *pChar );

protected:
	void SetSkillActionState( ioBaseChar *pChar );

	void CheckLinearCombo( ioBaseChar *pChar );
	void SetNextLinearCombo( ioBaseChar *pChar, bool bOther );

	void SetLastAttack( ioBaseChar *pChar );

	bool CheckEnableRetreatState( ioBaseChar *pChar );
	void SetRetreatState( ioBaseChar *pChar );

	const AttributeList& GetComboList();
	AttackAttribute GetEndAttribute();

public:
	ioLinearComboSkill2();
	ioLinearComboSkill2( const ioLinearComboSkill2 &rhs );
	virtual ~ioLinearComboSkill2();
};

inline ioLinearComboSkill2* ToLinearComboSkill2( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_LINEAR_COMBO2 )
		return NULL;

	return dynamic_cast< ioLinearComboSkill2* >(pAttack);
}
