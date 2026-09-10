#pragma once

#include "ioAttackSkill.h"

class ioGrapplingAttack5Skill : public ioAttackSkill
{
private:
	typedef std::vector< UniqueObjID > ArrowList;

	struct GrapplingTargetInfo
	{
		ioHashString name;
		ArrowList    guide;
		D3DXVECTOR3  direction;
		float        distance;
		DWORD        grappling_time;

		GrapplingTargetInfo()
		{
			name.Clear();
			guide.clear();
			distance       = 0.0f;
			direction      = ioMath::VEC3_ZERO;
			grappling_time = 0;
		}
	};
	typedef std::vector<GrapplingTargetInfo> GrapplingTargetList;
public:
	enum SkillState
	{
		SS_NONE,
		SS_ACTION,
		SS_GRAPPLING,
		SS_END,
	};

	enum SkillSync
	{
		SSC_GRAPPLING,
		SSC_ATTACK,
		SSC_SKILL_END,
	};

protected:
	SkillState   m_SkillState;

	ioHashString m_GrapplingTargetName;
	GrapplingTargetList m_GrapplingTargetList;

	// 모션 및 공격
	WeaponInfoList  m_GrapplingAttackList;
	ioHashString    m_szLoopMotion;
	CEncrypt<float> m_fLoopTimeRate;

	DWORD m_dwFireStartTime;

	// 키 연타
	CEncrypt<DWORD>	m_dwEnableKeyTime;
	DWORD			m_dwDelayKeyTime;
	DWORD			m_dwCheckKeyTime;

	// 콤보
	CEncrypt<int>	m_iMaxCombo;
	int				m_iCurCombo;

	// 타겟 위치
	D3DXVECTOR3	m_vTargetPosition;

	// 타겟 검색
	ioHashStringVec m_TargetList;
	int   m_iTargetCnt;
	float m_fTargetRange;
	float m_fTargetAngle;

	// Guide Effect
	typedef std::vector< UniqueObjID > ArrowList;
	ioHashString  m_GuideEffect;
	CEncrypt<int> m_iMaxEffectCount;

	// Protect
	CEncrypt<DWORD> m_dwProtectTime;

protected:
	void ClearData();

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );
	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );

	virtual bool SetSkillGrapplingState( ioBaseChar *pChar, const ioHashString &szTargetName );
	virtual D3DXVECTOR3 GetGrapplingTargetPos( ioBaseChar *pChar );

	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsEnableReserve() const;

	virtual bool CheckUseItemMesh();

protected:
	virtual void AttackSkillFire( ioBaseChar *pChar );

private:
	void ProcessAction( ioBaseChar *pChar );

private:
	void FindTarget( ioBaseChar *pOwner );
	bool IsEnableTargetState( ioBaseChar *pTarget );

	void GrapplingAttackFire( ioBaseChar *pChar );

public:
	void SetSkillEndState( ioBaseChar *pChar, bool bSend = false );

	// Guide Effect
protected:
	void ProcessGuideEffect( ioBaseChar *pOwner );
	void DestroyArrow();
	bool CheckArrowCreated( ioBaseChar *pOwner );
	void ArrangeArrows( ioBaseChar *pOwner );

private:
	void AddTargetInfo( ioBaseChar *pChar, ioBaseChar *pTarget );
	bool CheckAlreadyTarget( const ioHashString &szName );
	bool CheckTarget( ioBaseChar *pChar );

public:
	ioGrapplingAttack5Skill();
	ioGrapplingAttack5Skill( const ioGrapplingAttack5Skill &rhs );
	virtual ~ioGrapplingAttack5Skill();
};

inline ioGrapplingAttack5Skill* ToGrapplingAttack5( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_GRAPPLING_ATTACK5 )
		return NULL;

	return static_cast< ioGrapplingAttack5Skill* >( pAttack );
}

