#pragma once

#include "ioUserKeyInput.h"
#include "ioAttackAttribute.h"

#include "ioCountSkill.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioScabSkill : public ioCountSkill//public ioNormalSkill
{
public:
	enum SkillState
	{
		SS_PRE,
		SS_START_FIRE,
		SS_WEAPON_WOUNED,
		SS_LOOP,
		SS_LOOP_FIRE,
		SS_END,
	};	

protected:
	SkillState m_SkillState;

	//ZeroAim
	ioHashString m_SkillZeroAimMotion;
	float m_fSkillZeroAimMotionLate;

	DWORD m_dwSkillZeroAimMotionEnd;
	DWORD m_dwSkillZeroAimAttackCnt;
	DWORD m_dwCurSkillZeroAimAttackCnt;	
	bool m_bSkillZeroAimAttackGauge;

	//StartFire
	DWORD m_dwSkillAimMotionEnd;
	AttackAttribute m_StartAttribute;
	bool m_bSkillAimAttackGauge;
	DWORD m_dwStartFireEndMotionTime;

	//LoopFire
	AttackAttribute m_LoopFireAttribute;
	DWORD m_dwLoopFireEndMotionTime;

	//TargetList
	ioHashString m_LoopDelayAnimation;
	ioHashString m_LoopMoveAnimation;
	bool m_bSetMoveAni;

	//Send List
	ioHashStringVec m_vAddTargetList;

	// 스킬 게이지 감소
	CEncrypt<bool> m_bDisableGrowth;
	CEncrypt<float> m_fGuagePerTic;
	CEncrypt<DWORD> m_dwTicTime;

	DWORD m_dwCheckTime;
	CEncrypt<float> m_fTicGaugeRateByGrowth;
	CEncrypt<DWORD> m_dwTicTimeByGrowth;

	CEncrypt<DWORD> m_dwSkillProtectedTime;
	DWORD m_dwStartSkillProtectedTime;

	BuffInfoList m_TargetAimAttackBuffList;

	ioHashString m_SkillMapEffect;
	float m_fCharEffectOffset;

	//////////////////////////////////////////////////////////////////////////
	CEncrypt<float>	 m_fTargetMaxRange;
	CEncrypt<float>	 m_fTargetMinRange;
	CEncrypt<float>	 m_fTargetAngle;
	TargetWoundType  m_TargetWoundType;

	BuffInfoList m_OwnerBuffList;
	ioHashString m_stTargetCheckBuffName;

	float		m_fLoopFireLookOffSet;
	float		m_fLoopFireHeightOffSet;

	ioHashString m_stZoneWeaponTargetName;
	
	DWORD		m_dwMaxLoopDelayTime;
	DWORD       m_dwEndLoopTime;

	ioHashString	m_stFailAttackEndMotion;
	float			m_fFailAttackEndMotionRate;
	//////////////////////////////////////////////////////////////////////////

protected:
	void ClearData();
	void FindTarget( ioBaseChar *pChar, ioPlayStage* pStage );
	bool IsEnableTargetState( ioBaseChar *pTarget, bool bCheckTargetWound, bool bCheckProtected = false );

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

	void LoadBuffList( ioINILoader &rkLoader );

public:
	virtual bool IsCanUseSkill( ioBaseChar *pChar, bool bHelpMsg );

	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnSkillEnd( ioBaseChar *pChar );	
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );

	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );

	virtual bool CheckUseItemMesh();

public:
	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;

protected:
	void SetStartFireState( ioBaseChar *pChar,ioPlayStage *pStage );
	void SetLoopState( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetLoopFireState( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetEndState( ioBaseChar *pChar, ioPlayStage *pStage );

	void CheckNormalMoveAni( ioBaseChar* pChar );

	void SetMove();
	void SetPreDelay();

	void AddOwnerBuff( ioBaseChar *pChar );
	void EraseOwnerBuff( ioBaseChar *pChar );

	void FomationTarget( ioBaseChar *pChar, float fSourceHeight, float fTargetHeight, D3DXVECTOR3 vTargetPos );

public:
	virtual void SetNextSkillState( ioBaseChar *pWounder, ioWeapon *pWeapon = NULL );
public:
	ioScabSkill();
	ioScabSkill( const ioScabSkill &rhs );
	virtual ~ioScabSkill();
};

inline ioScabSkill* ToScabSkill( ioSkill *pSkill )
{
	ioCountSkill *pCountSkill = ToCountSkill( pSkill );
	if( !pCountSkill || pCountSkill->GetSubSkillType() != NST_SCAB_SKILL )
		return NULL;

	return dynamic_cast< ioScabSkill* >( pSkill );
}

