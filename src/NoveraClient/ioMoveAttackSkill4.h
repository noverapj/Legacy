#pragma once
#include "ioattackskill.h"
class ioMoveAttackSkill4 :
	public ioAttackSkill
{
private:
	enum SkillState
	{
		SS_NONE,
		SS_DELAY,
		SS_JUMP,
		SS_LANDING,
		SS_NORMAL_ATTACK,
		SS_JUMP_ATTACK,
		SS_END,
	};

	enum SkillSyncType
	{
		SST_DELAY,
		SST_JUMP,
		SST_LANDING,
		SST_NORMAL_ATTACK,
		SST_JUMP_ATTACK,
		SST_JUMP_ATTACK_END,
		SST_JUMP_CHANGE,
		SST_FIRE,
		SST_END,
	};
private:
	SkillState		m_SkillState;
	
	//Delay
	ioHashString	m_szDelayAni;
	float			m_fDelayAniRate;
	ioHashString	m_szRunAni;
	float			m_fRunAniRate;
	bool			m_bDelayMove;

	//Jump
	ioHashString	m_szJumpAni;
	float			m_fJumpAniRate;
	float			m_fJumpPower;
	float			m_fJumpNormalGravityRate;
	float			m_fJumpFreeGravityRate;
	float			m_fSkillJumpMoveSpeedRate;
	bool			m_bChangeJumpGravity;

	//Landing
	ioHashString	m_szLandingAni;
	float			m_fLandingAniRate;

	//Skill Gauge
	bool m_bDisableGrowth;
	float m_fGuagePerTic;
	DWORD m_dwTicTime;

	DWORD m_dwCheckTime;
	float m_fTicGaugeRateByGrowth;
	DWORD m_dwTicTimeByGrowth;

	//Attack
	D3DXVECTOR3		m_vWeaponOffset;
	D3DXVECTOR3		m_vWeaponMoveDir;

	//Circle
	ioHashString	m_szCircleEffect;
	DWORD			m_dwCircleEffectID;
	D3DXVECTOR3		m_vCircleOffset;
	D3DXVECTOR3		m_vCirclePos;
	float			m_fCircleMapHeight;

protected:
	void ClearData();
	void SetDelayState( ioBaseChar *pOwner, ioPlayStage *pStage, bool bSend );
	void CheckMove( ioBaseChar *pOwner, bool bFirst );

	void SetJumpState( ioBaseChar *pOwner );
	bool CheckLanding( ioBaseChar *pOwner );

	void SetLandingState( ioBaseChar *pOwner );

	void SetNormalAttackState( ioBaseChar *pOwner );
	void SetJumpAttackState( ioBaseChar *pOwner );

	void SetEndState( ioBaseChar *pOwner );

	void FireWeapon( ioBaseChar *pOwner );
	void CheckCircle( ioBaseChar *pOwner, ioPlayStage *pStage );
	void CheckGauge( ioBaseChar *pOwner );
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();
public:
	virtual bool OnSkillStart( ioBaseChar *pOwner );
	virtual void OnProcessState( ioBaseChar *pOwner, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pOwner );
	virtual float GetSkillGravityRate();
	virtual float GetSkillMoveRate() const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool IsAttackEndState() const;
	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );
public:
	ioMoveAttackSkill4(void);
	ioMoveAttackSkill4( const ioMoveAttackSkill4 &rhs );
	virtual ~ioMoveAttackSkill4(void);
};

inline ioMoveAttackSkill4* ToMoveAttackSkill4( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_MOVE_ATTACK4 )
		return NULL;

	return dynamic_cast< ioMoveAttackSkill4* >( pAttack );
}