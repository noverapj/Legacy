#pragma once

#include "ioNormalSkill.h"
#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioMultiCircleSkill : public ioNormalSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_LOOP,
		SS_ACTION,
		SS_END,
	};

	enum CircleState
	{
		CIS_NONE,
		CIS_NORMAL,
		CIS_NORMAL_FIX,
		CIS_SPECIAL,
		CIS_SPECIAL_FIX,
	};

	enum SkillSyncState
	{
		SSS_ACTION,
	};

	enum AttackType
	{
		AT_CIRCLE,
		AT_TARGETING,
	};

protected:
	CEncrypt<int> m_State;
	AttackType	  m_AttackType;

	DWORD m_dwFireStartTime;
	DWORD m_dwActionStartTime;

	CEncrypt<int> m_iMaxAttackCnt;
	CEncrypt<int> m_iCurAttackCnt;

	// magic circle move
	float m_fCircleRange;
	float m_fCircleMoveSpeed;
	float m_fCircleHeightGap;
	float m_fCircleOffSet;

	D3DXVECTOR3 m_vCirclePos;

	// circle
	CircleState m_CircleState;

	ioHashString m_NormalCircle;
	ioHashString m_NormalCircleFix;
	ioHashString m_SpecialCircle;
	ioHashString m_SpecialCircleFix;

	ioHashString m_RedGuidCircle;
	ioHashString m_BlueGuidCircle;

	DWORD m_dwNormalDuration;
	DWORD m_dwSpecialDuration;
	DWORD m_dwCircleChangeTime;

	// attack
	WeaponInfo m_NormalAttackAttribute;
	WeaponInfo m_SpecialAttackAttribute;

	//
	float m_fEndMotionRate;
	ioHashString m_SkillEndMotion;

	ioHashString	m_LoopAnimation;
	CEncrypt<DWORD>	m_dwLoopProtectTime;
	DWORD			m_dwLoopStartTime;
	DWORD			m_dwFirstLoopStartTime;

	//
	ioHashString m_StartEffect;
	ioHashString m_EndEffect;

	CEncrypt<bool>	m_bPreventRecoveryGauge;

	ioHashString	m_szTargetingEffect;
	DWORD			m_dwTargetingEffectID;
	float			m_fTargetingMaxHeightOffset;
	float			m_fTargetingMinHeightOffset;
	float			m_fTargetingMarkUpDownSpeed;
	bool			m_bTargetingMarkUp;
	float			m_fTargetingMarkHeight;
	D3DXVECTOR3		m_vTargetingMarkAttackOffset;

	BuffInfoList	m_OwnerBuffList;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	void LoadBuffList( ioINILoader &rkLoader );
	void AddOwnerBuff( ioBaseChar *pChar );
	void RemoveOwnerBuff( ioBaseChar *pChar );

	virtual ioSkill* Clone();

public:
	virtual bool IsProtected( int iDefenseBreakType ) const;

	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnSkillEnd( ioBaseChar *pChar );	
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );

	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );
	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );

	virtual bool CheckUseItemMesh();

public:
	virtual bool IsEndState() const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;

protected:
	virtual bool StartPreDelayAnimation( ioBaseChar *pChar );
	virtual void SetNormalSkill( ioBaseChar *pChar );

protected:
	void UpdateMagicCircle( ioBaseChar *pChar, ioPlayStage *pStage );
	void ChangeMagicCircle( ioBaseChar *pChar, ioPlayStage *pStage, CircleState eState );
	
	void CheckCreateAttack( ioBaseChar *pChar, ioPlayStage *pStage );

	const WeaponInfo& GetCircleAttribute();

	void SetMagicCircle( ioBaseChar *pChar );
	void SetLoopState( ioBaseChar *pChar );
	void SetActionState( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetEndState( ioBaseChar *pChar );

	void SetTargetingMark( ioBaseChar *pChar );
	void UpdateTargeingMark( ioBaseChar *pChar, ioPlayStage *pStage );

public:
	bool CheckKOEffect() { return m_bPreventRecoveryGauge; }

public:
	ioMultiCircleSkill();
	ioMultiCircleSkill( const ioMultiCircleSkill &rhs );
	virtual ~ioMultiCircleSkill();
};

inline ioMultiCircleSkill* ToMultiCircleSkill( ioSkill *pSkill )
{
	ioNormalSkill *pNormal = ToNormalSkill( pSkill );
	if( !pNormal || pNormal->GetSubSkillType() != NST_MULTI_CIRCLE2 )
		return NULL;

	return dynamic_cast< ioMultiCircleSkill* >( pSkill );
}

