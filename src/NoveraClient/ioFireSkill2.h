#pragma once

#include "ioAttackSkill.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioBaseChar;
class ioItem;
class SP2Packet;

class ioFireSkill2 : public ioAttackSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_ACTION,
		SS_END,
	};

	enum SyncType
	{
		ST_STATE,
		ST_FIRE,
	};

protected:
	SkillState m_SkillState;

	CEncrypt<DWORD> m_dwSkillProtectTime;
	DWORD m_dwActionStartTime;

	CEncrypt<int> m_iMaxFireCnt;
	CEncrypt<int> m_iCurFireCnt;

	TargetColType m_TargetColType;
	TargetWoundType m_TargetWoundType;

	CEncrypt<float> m_fSkillRange;
	CEncrypt<float> m_fSkillAngle;
	CEncrypt<float> m_fOffSet;
	CEncrypt<float> m_fHeightOffSet;

	CEncrypt<float> m_fUpHeight;
	CEncrypt<float> m_fUnderHeight;
	CEncrypt<float> m_fWidth;

	ioHashString m_LastTargetName;
	D3DXVECTOR3 m_vTargetPos;

	ioHashString m_FireUpAni;
	ioHashString m_FireDownAni;

	CEncrypt<float> m_fTargetAimRate;
	CEncrypt<float> m_fDownTargetAimRate;

	ioHashString m_szAimedMarker;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual bool CheckUseItemMesh();

	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool IsProtected( int iDefenseBreakType ) const;

	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );
	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );

protected:
	virtual void AttackSkillFire( ioBaseChar *pChar );

	void SetSkillActionState( ioBaseChar *pChar, ioPlayStage *pStage );

	void CheckNextTarget( ioBaseChar *pChar, ioPlayStage *pStage );

	void AimedFireToUp( ioEntityGroup *pGrp,
						float fWeight,
						float fTimeRate,
						DWORD dwPreDelay,
						bool bHold );

	void AimedFireToDown( ioEntityGroup *pGrp,
						  float fWeight,
						  float fTimeRate,
						  DWORD dwPreDelay,
						  bool bHold );

	bool CheckDownAimState( ioBaseChar *pTarget );
	
public:
	ioFireSkill2();
	ioFireSkill2( const ioFireSkill2 &rhs );
	virtual ~ioFireSkill2();
};

inline ioFireSkill2* ToFireSkill2( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_FIRE2 )
		return NULL;

	return dynamic_cast< ioFireSkill2* >(pAttack);
}

