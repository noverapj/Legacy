#pragma once

#include "ioAttackSkill.h"
#include "ioAttackAttribute.h"
#include "ioUserKeyInput.h"

class ioINILoader;
class ioBaseChar;
class ioItem;
class SP2Packet;

class ioChargeLineComboSkill : public ioAttackSkill
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
		SS_CHARGE,
		SS_GATHERING,
		SS_GATHER_ATTACK,
		SS_COMBO,
		SS_RETREAT,
		SS_GATHERING_ROT,
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

	//Ç³¿ùÁÖ
	DWORD			m_dwMaxChargeTime;
	DWORD			m_dwChargeStartTime;
	DWORD			m_dwGatheringStartTime;
	DWORD			m_dwMaxGatheringTime;

	ioHashString	m_stGatheringAni;
	float			m_fGatheringAniRate;

	DWORD			m_dwGatherTic;

	AttributeList   m_GatherAttackList;

	AttackAttribute m_GatherAttack1;
	AttackAttribute m_GatherAttack2;

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

protected:
	virtual void AttackSkillFire( ioBaseChar *pChar );

protected:
	void SetSkillActionState( ioBaseChar *pChar );
	
	void CheckLinearCombo( ioBaseChar *pChar );
	void SetNextLinearCombo( ioBaseChar *pChar, bool bOther );
	void ChangeChargeState( ioBaseChar *pChar );

	bool CheckEnableRetreatState( ioBaseChar *pChar );
	void SetRetreatState( ioBaseChar *pChar );
	void ChangeGatheringState( ioBaseChar *pChar );
	void ChangeGatherAttack( ioBaseChar *pChar, int nTic );

	const AttributeList& GetComboList();
	AttackAttribute GetEndAttribute();

	void CheckKeyRot( ioBaseChar *pChar );
	
public:
	ioChargeLineComboSkill();
	ioChargeLineComboSkill( const ioChargeLineComboSkill &rhs );
	virtual ~ioChargeLineComboSkill();
};

inline ioChargeLineComboSkill* ToChargeLineComboSkill( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_CHARGE_LINE_COMBO )
		return NULL;

	return dynamic_cast< ioChargeLineComboSkill* >(pAttack);
}
