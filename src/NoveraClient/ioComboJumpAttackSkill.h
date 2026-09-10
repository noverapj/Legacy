#pragma once

#include "ioAttackSkill.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioBaseChar;
class ioItem;

class ioComboJumpAttackSkill : public ioAttackSkill
{
protected:
	enum SkillState
	{
		SS_NONE,
		SS_LOOP,
		SS_ACTION,
		SS_END,
	};

protected:
	SkillState     m_SkillState;

	// 스킬 게이지 감소
	CEncrypt<bool> m_bDisableGrowth;
	CEncrypt<float> m_fGuagePerTic;
	CEncrypt<DWORD> m_dwTicTime;

	DWORD m_dwCheckTime;
	CEncrypt<float> m_fTicGaugeRateByGrowth;
	CEncrypt<DWORD> m_dwTicTimeByGrowth;
	
	ioHashString m_szRunLoopMotion;

	float m_fJumpPower;

	DWORD m_dwLoopStartTime;
	DWORD m_dwJumpDuration;

	DWORD m_dwKeyCheckStartTime;
	CEncrypt<DWORD>	m_dwKeyAddDuration;
	CEncrypt<DWORD>	m_dwKeyEnableTime;
	bool m_bAddKey;

	WeaponInfo m_WeaponInfo;

	bool m_bEnd;

	bool m_bWeaponAttack;
	float m_fGravityAmt;

	DWORD m_dwSkillProtectTime;

	float m_fSetHeightGap;

protected:
	void ClearData();

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );
	
	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );
	virtual bool IsEndState() const;
	virtual bool IsAttackEndState() const;
	virtual bool IsProtected( int iDefenseBreakType ) const;

protected:
	void SetActionState( ioBaseChar *pChar );
	void SetLoopState( ioBaseChar *pChar );

	bool CheckLanding( ioBaseChar *pChar );
	void LandingExplosion( ioBaseChar* pChar );
	
public:
	void SetWeaponAttack( bool bAttack ) { m_bWeaponAttack = bAttack; }

public:
	ioComboJumpAttackSkill(void);
	ioComboJumpAttackSkill( const ioComboJumpAttackSkill &rhs );
	virtual ~ioComboJumpAttackSkill();
};

inline ioComboJumpAttackSkill* ToComboJumpAttackSkill( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_COMBO_JUMP_ATTACK )
		return NULL;

	return dynamic_cast< ioComboJumpAttackSkill* >( pAttack );
}