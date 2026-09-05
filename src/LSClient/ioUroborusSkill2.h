#pragma once

#include "ioAttackSkill.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioBaseChar;
class ioItem;
class SP2Packet;

class ioUroborusSkill2 : public ioAttackSkill
{
public:
	enum UroborusSkillState
	{
		USS_NONE,
		USS_FIRE,
		USS_UROBORUS,
		USS_COMBO,
		USS_END,
	};

	enum UroborusSync
	{
		US_PRE_INFO,
		US_COMBO,
	};

protected:
	UroborusSkillState m_SkillState;

	// uroborus autotarget
	TargetColType m_TargetColType;
	TargetWoundType m_TargetWoundType;

	float m_fSkillRange;
	float m_fSkillAngle;
	float m_fOffSet;

	float m_fUpHeight;
	float m_fUnderHeight;
	float m_fWidth;

	ioHashString m_szUroborusTargetName;

	float m_fTargetMidRate;
	float m_fTargetDownMidRate;

	// uroborus fire
	D3DXVECTOR3 m_vUroborusDir;

	CEncrypt<float> m_fTargetAngle;
	CEncrypt<float> m_fTargetRange;
	D3DXVECTOR3 m_vTargetPos;

	// uroborus motion
	ioHashString m_UroborusMotion;

	// combo
	bool m_bAutoComboAction;

	DWORD m_dwSkillProtectTime;
	DWORD m_dwCurSkillProtectTime;

	AttributeList m_vComboList;
	int m_iCurLinearCombo;

	// uroborus end
	ioHashString m_UroborusEndMotion;
	float m_fUroborusEndMotionRate;

	// Bug Check
	DWORD m_dwCheckStartTime;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual bool CheckUseItemMesh();

	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );
	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );

	virtual bool IsEnableReserve() const;
	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;

protected:
	virtual void AttackSkillFire( ioBaseChar *pChar );

public:
	void CheckUroborusAutoTarget( ioBaseChar *pChar );

	void SetUroborusMotionState( ioBaseChar *pChar, ioWeapon *pWeapon );
	void SetSkillEndState( ioBaseChar *pChar );

	void CheckComboState( ioBaseChar *pChar );

protected:
	void CheckPreUroborusInfo( ioBaseChar *pChar );
	bool CheckDownAimState( ioBaseChar *pTarget );

	void CheckLinearCombo( ioBaseChar *pChar, bool bFirst );
	void SetNextLinearCombo( ioBaseChar *pChar );
	
public:
	ioUroborusSkill2();
	ioUroborusSkill2( const ioUroborusSkill2 &rhs );
	virtual ~ioUroborusSkill2();
};

inline ioUroborusSkill2* ToUroborusSkill2( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_UROBORUS2 )
		return NULL;

	return dynamic_cast< ioUroborusSkill2* >(pAttack);
}



