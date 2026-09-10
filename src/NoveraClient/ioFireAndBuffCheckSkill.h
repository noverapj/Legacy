#pragma once
#include "ioAttackSkill.h"
class ioFireAndBuffCheckSkill :	public ioAttackSkill
{
protected:
	enum SkillState
	{
		SS_NONE,
		SS_FIRE,
		SS_LINEAR,
		SS_WHOLE_ATTACK,
		SS_END,
	};

	enum TargetAttackType
	{
		TAT_SINGLE_COMBO,
		TAT_WHOLE_TARGET,
	};

	struct UpDownAttackAttribute
	{
		AttackAttribute m_Attribute;
		ioHashString	m_UpAnimation;
		ioHashString	m_DownAnimation;

		UpDownAttackAttribute()
		{
			Init();
		}

		void Init()
		{
			m_Attribute.Init();
			m_UpAnimation.Clear();
			m_DownAnimation.Clear();
		}
	};

	typedef std::vector< UpDownAttackAttribute > UpDownAttributeList;
protected:
	//State
	SkillState				m_SkillState;
	TargetAttackType		m_TargetAttackType;

	//Ani
	UpDownAttributeList		m_vUpDownLinearComboList;
	DWORD					m_dwFireStartTime;
	int						m_iCurComboIndex;
	AttackAttribute			m_WholeTargetAttack;
	DWORD					m_dwTargetCheckTime;
	bool					m_bUsingTargetCheckTime;


	//Attack Count
	WeaponInfoList			m_iCurWeaponInfo;
	int						m_iMaxAttackCount;
	int						m_iCurAttackCount;

	//Target
	ioHashString			m_szTargetName;
	ioHashString			m_szCheckBuff;
	BaseCharDistanceSqList	m_vTargetInfoList;
	D3DXVECTOR3				m_vTargetDir;
	ioHashStringVec			m_RemoveBuff;
protected:
	void ClearData();

	void SetFireState( ioBaseChar *pChar );
	void SetLinearCombo( ioBaseChar *pChar );
	void SetTargetRotation( ioBaseChar *pChar );
	void SetAttackAni( ioEntityGroup *pGrp, const ioHashString& szCurAni, const ioHashString& szUpAni, const ioHashString& szDownAni, const float& fTimeRate, const DWORD dwPreDelay );
	void SetEndState( ioBaseChar *pChar, bool bSend = false );

	void SetWholeAttack( ioBaseChar *pChar );

	bool GetTargetList( ioBaseChar *pOwner, ioPlayStage *pStage );
	float GetAimedUpDownWeight( const D3DXVECTOR3 &vAimDir, float fMaxAngle );

	bool CheckLinearAttack( ioBaseChar *pOwner );
	bool CheckTargetList( ioBaseChar *pOwner );

	void FireTarget( ioBaseChar *pOwner );
	void FireWholeTarget( ioBaseChar *pOwner, ioPlayStage *pStage );
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();
public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );

	virtual void OnSkillEnd( ioBaseChar *pChar );
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;

	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );
public:
	ioFireAndBuffCheckSkill(void);
	ioFireAndBuffCheckSkill( const ioFireAndBuffCheckSkill &rhs );
	~ioFireAndBuffCheckSkill(void);
};

inline ioFireAndBuffCheckSkill* ToFireAndBuffCheckSkill( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_FIRE_AND_BUFF_CHECK )
		return NULL;

	return dynamic_cast< ioFireAndBuffCheckSkill* >(pAttack);
}