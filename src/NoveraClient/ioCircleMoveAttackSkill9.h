#pragma once
#include "ioNormalSkill.h"
class ioCircleMoveAttackSkill9 :public ioNormalSkill
{
protected:
	enum SkillState
	{
		SS_NONE,
		SS_CIRCLE_MOVE,
		SS_ACTION,
		SS_LOOP,
		SS_END,
	};
protected:
	SkillState		m_SkillState;

	ioHashString	m_szCircleMoveLoopAni;
	float			m_fCircleMoveLoopAniRate;
	DWORD			m_dwCircleMoveLoopDuration;

	ioHashString	m_szLoopAni;
	float			m_fLoopAniRate;
	DWORD			m_dwLoopDuration;

	DWORD			m_dwFireStartTime;

	DWORD			m_dwSkillProtectStartTime;
	DWORD			m_dwSkillProtectTime;

	//gauge
	bool			m_bDisableGrowth;
	DWORD			m_dwTicTime;
	DWORD			m_dwTicTimeByGrowth;
	float			m_fGuagePerTic;
	float			m_fTicGaugeRateByGrowth;
	DWORD			m_dwCheckTime;

	// Circle
	D3DXVECTOR3		m_vCirclePos;

	float			m_fCircleRange;
	float			m_fCircleMoveSpeed;
	float			m_fCircleHeightGap;
	float			m_fCircleOffSet;

	ioHashString	m_ReadyCircle;
	ioHashString	m_RedGuidCircle;
	ioHashString	m_BlueGuidCircle;

	float			m_fEndMotionRate;
	ioHashString	m_SkillEndMotion;

	WeaponInfoList	m_CircleWeaponList;

	DWORD			m_dwCircleMapEffectID;
protected:
	void ClearData();

	void SetCircleMoveState( ioBaseChar *pChar );
	void SetActionState( ioBaseChar *pChar );
	void SetLoopState( ioBaseChar *pChar );
	void SetEndState( ioBaseChar *pChar );

	void CreateWeapon( ioBaseChar *pChar, ioPlayStage *pStage );

	void CreateMapEffect( ioBaseChar *pChar, ioPlayStage *pStage );
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnSkillEnd( ioBaseChar *pChar );	
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );

	virtual void SetNormalSkill( ioBaseChar *pChar );

	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool IsProtected( int iDefenseBreakType ) const;

	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );
public:
	ioCircleMoveAttackSkill9(void);
	ioCircleMoveAttackSkill9( const ioCircleMoveAttackSkill9 &rhs );
	virtual ~ioCircleMoveAttackSkill9(void);
};

inline ioCircleMoveAttackSkill9* ToCircleMoveAttackSkill9( ioSkill *pSkill )
{
	ioNormalSkill *pNormal = ToNormalSkill( pSkill );
	if( !pNormal || pNormal->GetSubSkillType() != NST_CIRCLE_MOVE_ATTACK9 )
		return NULL;

	return dynamic_cast< ioCircleMoveAttackSkill9* >( pSkill );
}
