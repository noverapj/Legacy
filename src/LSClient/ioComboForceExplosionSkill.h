#pragma once

#include "ioAttackSkill.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioBaseChar;
class ioItem;
class SP2Packet;

class ioComboForceExplosionSkill : public ioAttackSkill
{
public:
	enum SkillState
	{
		SS_READY,
		SS_ACTION,
		SS_GRAPPLING_TARGET,
		SS_COMBO_LOOP,
		SS_FINISH,
		SS_END,
	};

protected:
	SkillState m_SkillState;

	// 잡힌 대상
	ioHashString	m_GrapplingTargetName;

	// 스킬 이펙트
	ioHashString	m_szSkillEffect;

	// 루프모션
	ioHashString	m_szLoopMotion;
	CEncrypt<float>	m_fLoopMotionRate;
	CEncrypt<DWORD>	m_dwLoopMotionDuration;

	// 정기폭발 AttackAttribute
	AttackAttribute m_FinishAttackAttribute;

	CEncrypt<float>	m_fTargetLoopGapRange;
	CEncrypt<float>	m_fCharHeightRate;

	CEncrypt<DWORD> m_dwSkillProtectTime;
	CEncrypt<DWORD> m_dwCurSkillProtectTime;

	CEncrypt<float>	m_fGrapplingMinAngle;
	CEncrypt<float>	m_fGrapplingMaxAngle;
	CEncrypt<float>	m_fGrapplingRange;

	// 연타 관련
	CEncrypt<DWORD>	m_dwKeyAddDuration;
	CEncrypt<DWORD>	m_dwKeyEnableTime;
	CEncrypt<DWORD>	m_dwKeyMaxTime;
	bool			m_bAddKey;
	DWORD			m_dwKeyCheckStartTime;
	int				m_iCurCharging;
	int				m_iPreCharging;

	//float			m_fMaxScaleRate;
	float			m_fCurScaleRate;
	// 발사 시점
	DWORD	m_dwFireStartTime;
	DWORD	m_dwLoopStartTime;

	// 이펙트 시간. (필요없으면 삭제)
	ioHashString	m_szMapExpEffect;
	CEncrypt<float>	m_fXOffset;
	CEncrypt<float>	m_fYOffset;
	D3DXVECTOR3		m_vAttackDir;
	D3DXVECTOR3		m_vEffectPos;

	// 공격처리 관련
	DWORD m_dwBombWeaponIndex;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	void ClearData();
public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual bool SetSkillGrapplingState( ioBaseChar *pChar, const ioHashString &szTargetName );
	virtual D3DXVECTOR3 GetGrapplingTargetPos( ioBaseChar *pChar );

	virtual bool CheckUseItemMesh();

	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );

	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool IsProtected( int iDefenseBreakType ) const;

protected:
	virtual void AttackSkillFire( ioBaseChar *pChar );

protected:
	void SetActionState( ioBaseChar *pChar );
	void SetLoopState( ioBaseChar *pChar );
	void SetFinishState( ioBaseChar *pChar );
	void UpdateEffect( ioBaseChar* pChar, ioPlayStage* pStage );
	void FinishAttackSkillFire( ioBaseChar* pChar );

public:
	ioComboForceExplosionSkill();
	ioComboForceExplosionSkill( const ioComboForceExplosionSkill &rhs );
	virtual ~ioComboForceExplosionSkill();
};

inline ioComboForceExplosionSkill* ToComboForceExplosionSkill( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_COMBO_FORCE_EXPLOSION_FIRE )
		return NULL;

	return dynamic_cast< ioComboForceExplosionSkill* >(pAttack);
}

