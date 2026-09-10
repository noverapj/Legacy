#pragma once


#include "ioNormalSkill.h"
#include "ioUserKeyInput.h"
#include "ioAttackAttribute.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioCircleMoveAttackSkill5 : public ioNormalSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_LOOP,
		SS_ATTACK,
		SS_END,
	};

protected:
	CEncrypt<int> m_State;

	// Attack
	DWORD m_dwFireStartTime;
	AttackAttribute m_AttackAttribute;

	// Circle
	FloatVec        m_vCircleRange;
	CEncrypt<float> m_fCircleRange;
	CEncrypt<float> m_fCircleMoveSpeed;
	CEncrypt<float> m_fCircleHeightGap;
	CEncrypt<float> m_fCircleOffSet;
	ioHashString    m_ReadyCircle;
	ioHashString    m_RedEffect;
	ioHashString    m_BlueEffect;
	UniqueObjID     m_dwCurMapEffect;

	D3DXVECTOR3 m_vCirclePos;
	CEncrypt<float> m_fHeightOffset;
	CEncrypt<float> m_fFindTargetRange;
	
	// Skill gauge
	CEncrypt<bool>	m_bDisableGrowth;
	CEncrypt<float>	m_fGuagePerTic;
	CEncrypt<DWORD>	m_dwTicTime;
	DWORD			m_dwCheckTime;
	CEncrypt<float> m_fTicGaugeRateByGrowth;
	CEncrypt<DWORD> m_dwTicTimeByGrowth;

	// End Animation
	ioHashString    m_SkillEndAnimation;
	CEncrypt<float> m_fSkillEndAniRate;

	// count
	CEncrypt<int> m_iMaxAttackCount;
	int m_iCurAttackCount;

protected:
	void ClearData();

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool IsProtected( int iDefenseBreakType ) const;

	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnSkillEnd( ioBaseChar *pChar );	
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );

	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );

	virtual bool CheckUseItemMesh();

public:
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual SkillCharColType IsCharSkipState() const;
	virtual bool IsSkipState() const;

protected:
	virtual void SetNormalSkill( ioBaseChar *pChar );

	virtual bool HideHPGauge();

protected:
	void SetSkillLoopState( ioBaseChar *pChar );
	void SetSkillAttackState( ioBaseChar *pChar );
	void SetSkillEndState( ioBaseChar *pChar, bool bEndMotion );

	void ProcessSkillLoopState( ioBaseChar *pChar );
	void ProcessSkillAttackState( ioBaseChar *pChar );

	void CheckSkillGauge( ioBaseChar *pChar );
	void RotateToNearChar( ioBaseChar *pChar );

	bool CheckAttackCount();

public:
	ioCircleMoveAttackSkill5();
	ioCircleMoveAttackSkill5( const ioCircleMoveAttackSkill5 &rhs );
	virtual ~ioCircleMoveAttackSkill5();
};

inline ioCircleMoveAttackSkill5* ToCircleMoveAttackSkill5( ioSkill *pSkill )
{
	ioNormalSkill *pNormal = ToNormalSkill( pSkill );
	if( !pNormal || pNormal->GetSubSkillType() != NST_CIRCLE_MOVE_ATTACK5 )
		return NULL;

	return dynamic_cast< ioCircleMoveAttackSkill5* >( pSkill );
}

