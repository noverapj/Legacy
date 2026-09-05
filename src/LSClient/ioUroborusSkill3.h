#pragma once

#include "ioAttackSkill.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioBaseChar;
class ioItem;
class SP2Packet;

struct UroborusTargetInfo
{
	float m_fRange;
	ioHashString m_szName;
	D3DXVECTOR3 m_vUroborusDir;
};
typedef std::vector< UroborusTargetInfo > UroborusTargetInfoVec;

class UroborusTargetSort : public std::binary_function< const UroborusTargetInfo&, const UroborusTargetInfo&, bool >
{
public:
	bool operator()( const UroborusTargetInfo& lhs , const UroborusTargetInfo& rhs ) const
	{
		return ( lhs.m_fRange < rhs.m_fRange );
	}
};

class ioUroborusSkill3 : public ioAttackSkill
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

	UroborusTargetInfoVec m_UroborusTargets;
	int m_iWeaponIndex;
	bool m_bWeaponFire;
	DWORD m_dwUroborusFireTime;

	float m_fTargetMidRate;
	float m_fTargetDownMidRate;

	int m_iTargetCount;
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
	ioUroborusSkill3();
	ioUroborusSkill3( const ioUroborusSkill3 &rhs );
	virtual ~ioUroborusSkill3();
};

inline ioUroborusSkill3* ToUroborusSkill3( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_UROBORUS3 )
		return NULL;

	return dynamic_cast< ioUroborusSkill3* >(pAttack);
}

