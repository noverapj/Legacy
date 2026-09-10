#pragma once

#include "ioAttackSkill.h"
#include "ioAttackAttribute.h"
#include "ioUserKeyInput.h"

class ioINILoader;
class ioBaseChar;
class ioItem;
class SP2Packet;

class ioLinearComboSkill : public ioAttackSkill
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
	};

protected:
	SkillState m_SkillState;

	AttributeList m_vComboList;
	AttributeList m_vJumpComboList;
	AttributeList m_vDownComboList;

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

	// dummychar
	ioHashString m_DummyCharEffect;
	vDummyCharLoadInfoList m_DummyCharList;
	ioHashStringVec m_vDestroyDummyCharList;

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

	bool			m_bUseAreaWeapon;
	ioHashString	m_stAreaWeaponName;
	DWORD			m_dwAreaWeaponIndex;


protected:
	bool m_bUseEndCombo;
	int m_nCurEndCombo;
	int m_nMaxEndCombo;

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

	bool CheckEnableRetreatState( ioBaseChar *pChar );
	void SetRetreatState( ioBaseChar *pChar );

	const AttributeList& GetComboList();
	AttackAttribute GetEndAttribute();

	void CreateAreaWeapon( ioBaseChar *pChar );
	void DestroyAreaWeapon( ioBaseChar *pOwner );
	
public:
	ioLinearComboSkill();
	ioLinearComboSkill( const ioLinearComboSkill &rhs );
	virtual ~ioLinearComboSkill();
};

inline ioLinearComboSkill* ToLinearComboSkill( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_LINEAR_COMBO )
		return NULL;

	return dynamic_cast< ioLinearComboSkill* >(pAttack);
}
