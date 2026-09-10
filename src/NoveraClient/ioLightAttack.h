#pragma once

#include "ioAttackSkill.h"

class ioLightAttack : public ioAttackSkill
{
private:
	enum SkillState
	{
		SS_PRE,
		SS_PREACTION,
		SS_ACTION,
		SS_LOOP,
		SS_TELEPORT,
		SS_END,
	};
	
	enum SkillSync
	{
		SSC_PREACTION,
		SSC_TELEPORT,
	};

protected:
	SkillState		m_State;
	
	BuffInfoList m_OwnerBuffList;
	BuffInfoList m_ActionOwnerBuffList;
	bool			m_bSkillInJump;
	
	ioHashString m_SkillLoopMotion;
	CEncrypt<float> m_fSkillLoopMotionRate;
	CEncrypt<DWORD> m_dwLoopDuration;

	//점프
	CEncrypt<float>		m_fCharJumpPower;
	CEncrypt<float>		m_fJumpPowerInJump;

	//일반적으로 사용시
	CEncrypt<float>		m_fPreActionAniRate_Normal;
	ioHashString		m_strPreActionAni_Normal;

	//점프중에 사용시
	CEncrypt<float>	m_fPreActionAniRate_Jump;
	ioHashString		m_strPreActionAni_Jump;

	DWORD				m_dwPreActionEndTime;
	
	float				m_fJumpAngle;
	float				m_fJumpAngleInJump;

	float				m_fPreActionAniRate;
	ioHashString		m_strPreActionAni;

	float				m_fActionAniRate;
	ioHashString		m_strActionAni;

	// Circle
	float m_fCircleRange;
	float m_fCircleMoveSpeed;
	float m_fCircleHeightGap;
	float m_fCircleOffSet;

	D3DXVECTOR3 m_vCirclePos;

	ioHashString m_Circle;	

	ioHashString m_RedMapEffect;
	ioHashString m_BlueMapEffect;

	CEncrypt<DWORD> m_dwTeleportEndTime;
	DWORD m_dwCurTeleportEndTime;

	UniqueObjID m_dwSkillMapEffect;
	bool m_bEnd;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();
	
	void LoadActionOwnerBuffList( ioINILoader &rkLoader );
	void LoadOwnerBuffList( ioINILoader &rkLoader );
	void ClearData();
public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );

	virtual bool IsNoDropState() const;
	virtual bool IsEndState() const;
	virtual bool IsAttackEndState() const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool HasBuff( int iBuffType ) const;

	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );
public:
	void SetPreAction( ioBaseChar *pChar );
	void SetLoopState( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetTeleport( ioBaseChar *pChar, bool bSend );

	void AddActionOwnerBuff( ioBaseChar *pChar );
	void RemoveActionOwnerBuff( ioBaseChar *pChar );
	void AddOwnerBuff( ioBaseChar *pChar );
	
protected:
	virtual void AttackSkillFire( ioBaseChar *pChar );
	virtual bool CheckUseBuff( ioBaseChar *pChar );
public:
	ioLightAttack(void);
	ioLightAttack( const ioLightAttack &rhs );
	virtual ~ioLightAttack(void);
};

inline ioLightAttack* ToLightAttack( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_LIGHT_ATTACK )
		return NULL;

	return dynamic_cast< ioLightAttack* >(pAttack);
}
