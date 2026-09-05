#pragma once

#include "ioAttackSkill.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioBaseChar;
class ioItem;
class SP2Packet;

class ioChangePositionSkill : public ioAttackSkill
{
protected:
	enum SkillState
	{
		SS_NONE,
		SS_ACTION,
		SS_LOOP,
		SS_LOOP_END,
		SS_END,
	};

	struct LoopMotionInfo
	{
		ioHashString	m_SkillLoopMotionName;
		float	m_fSkillLoopMotionRate;
		float	m_fSearchRange;
		float	m_fSearchAngle;
		void Init()
		{
			m_SkillLoopMotionName.Clear();
			m_fSkillLoopMotionRate = FLOAT1;
			m_fSearchRange = 0.0f;
			m_fSearchAngle = 0.0f;
		}
	};
	typedef std::vector<LoopMotionInfo> LoopMotionList;

protected:
	SkillState m_SkillState;

	ioHashString	m_szTargetName;
	LoopMotionList	m_vLoopMotionList;
	LoopMotionInfo	m_LoopMotionInfo;
	CEncrypt<DWORD>	m_dwSkillLoopProtectTime;
	DWORD			m_dwSkillLoopStartTime;

	DWORD m_dwSkillLoopEndTime;

	D3DXVECTOR3	m_vAttackDir;

	CEncrypt<DWORD> m_dwChangePositionTime;
	DWORD			m_dwCheckChangePosTime;

	bool	m_bChangePosition;

	ioHashString	m_szChangePositionEffect;
	D3DXVECTOR3		m_vOwnerPos;
	D3DXVECTOR3		m_vTargetPos;

	ioHashStringVec	m_TargetBuffList;

	bool m_bWaitChangePosition;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	void LoadLoopMotionInfo( const char *szAttack, ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual bool CheckUseItemMesh();

	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );
	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );

	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsNoDropState() const;
	virtual bool IsEndState() const;

protected:
	virtual void AttackSkillFire( ioBaseChar *pChar );

protected:
	void SetSkillActionState( ioBaseChar *pChar );
	void SetSkillLoopState( ioBaseChar *pChar );
	void SetSkillLoopEndState( ioBaseChar *pChar );
	void SetSkillEndState();
	void SetChangePosition( ioBaseChar *pOwner, ioBaseChar *pTarget );

	void ProcessSkillLoopState( ioBaseChar *pChar );
	void ProcessSkillLoopEndState( ioBaseChar *pChar );

	ioBaseChar *CheckTarget( ioBaseChar *pOwner );

	void AddTargetBuff( ioBaseChar *pOwner );
	void RemoveTargetBuff( ioBaseChar *pOwner );

public:
	virtual void SetCreateWeaponDependency( ioBaseChar *pOwner, ioPlayStage *pStage );
	virtual void SetInfoByApplyWeapon( ioBaseChar *pOwner, ioBaseChar *pTarget, DWORD dwWeaponBaseIndex, bool bBlock );
	
public:
	ioChangePositionSkill();
	ioChangePositionSkill( const ioChangePositionSkill &rhs );
	virtual ~ioChangePositionSkill();
};

inline ioChangePositionSkill* ToChangePositionSkill( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_CHANGE_POSITION )
		return NULL;

	return dynamic_cast< ioChangePositionSkill* >(pAttack);
}

