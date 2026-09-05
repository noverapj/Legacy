#pragma once

#include "ioAttackSkill.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioBaseChar;
class SP2Packet;

class ioTraceSkill3 : public ioAttackSkill
{
public:
	enum SkillState
	{
		SS_PRE,
		SS_ACTION,
		SS_LOOP,
		SS_EXTRA_ACTION,
		SS_END,
	};

	enum SkillSync
	{
		SSC_TARGET,
		SSC_FIREMOTION,
		SSC_EXTRA_ACTION,
	};
	struct TargetAngleInfo
	{
		float m_fMinAngle;
		float m_fMaxAngle;

		TargetAngleInfo()
		{
			m_fMinAngle = 0.0f;
			m_fMaxAngle = 0.0f;
		}
	};
	typedef std::vector< TargetAngleInfo > TargetAngleInfoList;

protected:
	SkillState m_State;
	
	DWORD	m_dwFireStartTime;
	DWORD	m_dwFireEndTime;

	ioHashString m_szLoopMotion;
	DWORD m_dwLoopStartTime;
	// Action
	AttackAttribute m_LandAttack;

	ioHashStringVec	m_vTargetList;
	DWORD			m_dwWaitDuration;
	DWORD			m_dwAttackEndTime;
	FloatVec m_vAngleList;
	int m_iWeaponWay;

	ioHashString m_CurTargetName;
	ioHashString m_TargetEffect;

	CEncrypt<float> m_fTargetRange;
	CEncrypt<float> m_fTargetMinRange;
	TargetAngleInfoList	m_vTargetAngleList;
	TargetAngleInfoList m_vTargetAngleInfoList;

	CEncrypt<int>	m_iTargetLimitCount;

	ioHashStringVec	m_vWarpTargetList;
	DWORD m_dwWaitTimeForAttack;

	float m_fTargetOffSetRate;
	float m_fTargetYOffSet;

	bool m_bAttackedWeapon;//웨폰이 누군가를 공격을 성공했다.
	DWORD m_dwProtectionTime;

	DWORD m_dwEnableKeyInputTime;

	TargetWoundType m_TargetWoundType;

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

protected:
	virtual void AttackSkillFire( ioBaseChar *pChar );

public:
	virtual bool IsProtected( int iDefenseBreakType ) const;

	virtual bool IsAttackEndState() const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;

public:
	void SetWarpTargetList(const ioHashString &szTargetName);
	D3DXVECTOR3 CalcReturnUroborusPosition( ioWeapon *pWeapon );

protected:
	void ClearData();
	void SetActionState( ioBaseChar *pChar );
	void SetLoopState( ioBaseChar* pChar );
	void SetExtraActionState( ioBaseChar *pChar );

	void FindTarget( ioBaseChar *pOwner );
	void StartLandAttackMotion( ioBaseChar *pOwner );
	bool IsEnableTargetState( ioBaseChar *pTarget, bool bCheckTargetWound );

public:
	ioTraceSkill3();
	ioTraceSkill3( const ioTraceSkill3 &rhs );
	virtual ~ioTraceSkill3();
};

inline ioTraceSkill3* ToTraceSkill3( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_TRACE3 )
		return NULL;

	return dynamic_cast< ioTraceSkill3* >(pAttack);
}