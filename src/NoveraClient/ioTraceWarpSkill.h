#pragma once

#include "ioAttackSkill.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioBaseChar;
class ioItem;
class SP2Packet;

class ioTraceWarpSkill : public ioAttackSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_LOOP,
		SS_END,
	};
	enum SkillSync
	{
		SSC_TARGET,
		SSC_WARP_TARGET,
	};

protected:
	SkillState		m_SkillState;

	ioHashStringVec	m_vTargetList;
	DWORD			m_dwFireStartTime;

	ioHashString m_CurTargetName;
	ioHashString m_TargetEffect;

	CEncrypt<float> m_fTargetRange;
	CEncrypt<float> m_fTargetMinRange;
	CEncrypt<float> m_fTargetAngle;
	CEncrypt<int>	m_iTargetLimitCount;

	std::vector<D3DXVECTOR3> m_vTargetPosList;

	CEncrypt<DWORD>	m_dwWaitTimeForWarp;
	ioHashStringVec	m_vWarpTargetList;
	ioHashStringVec	m_szWarpBuffList;
	ioHashString	m_szWarpStartEffect;
	ioHashString	m_szWarpEndEffect;
	ioHashStringVec	m_szWarpExceptTargetBuffList;
	CEncrypt<bool>	m_bCheckDropZone;

	ioHashString	m_SkillLoopMotion;
	CEncrypt<float>	m_fSkillLoopMotionRate;

	DWORD			m_dwSkillLoopStartTime;
	CEncrypt<DWORD> m_dwSkillLoopDuration;
	CEncrypt<DWORD> m_dwSkillLoopProtectTime;

	CEncrypt<bool>	m_bSelectCollisionBuffEnable;

protected:
	TargetWoundType m_TargetWoundType;
	TargetWoundType m_WarpTargetWoundType;

protected:
	void ClearData();

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool IsProtected( int iDefenseBreakType ) const;

	virtual bool CheckUseItemMesh();

	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );
	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );

protected:
	virtual void AttackSkillFire( ioBaseChar *pChar );

protected:
	void SetSkillLoopState( ioBaseChar *pChar );
	void SetSkillEndState( ioBaseChar *pChar );

	void SetWarpTarget( ioBaseChar *pChar, ioPlayStage *pStage, bool bSend = false );

protected:
	void FindTarget( ioBaseChar *pOwner );
	bool IsEnableTargetState( ioBaseChar *pTarget, bool bCheckTargetWound, bool bWarp = false );

public:
	void SetWarpTargetList( const ioHashString &szTargetName );

public:
	ioTraceWarpSkill();
	ioTraceWarpSkill( const ioTraceWarpSkill &rhs );
	virtual ~ioTraceWarpSkill();
};

inline ioTraceWarpSkill* ToTraceWarpSkill( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_TRACE_WARP )
		return NULL;

	return dynamic_cast< ioTraceWarpSkill* >(pAttack);
}

