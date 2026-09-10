#pragma once

#include "ioAttackSkill.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioBaseChar;
class ioItem;
class SP2Packet;

typedef struct tagWhipInfo
{
	float m_fWhipRate;
	
	ioHashString m_WhipEffect;
	WeaponInfo m_WeaponInfo;
	
	ioHashString m_FinishWhipEffect;
	WeaponInfo m_FinishWeaponInfo;

	tagWhipInfo()
	{
		Init();
	}

	void Init()
	{
		m_fWhipRate = 0.0f;

		m_WhipEffect.Clear();
		m_WeaponInfo.Init();

		m_FinishWhipEffect.Clear();
		m_FinishWeaponInfo.Init();
	}

} WhipInfo;

typedef std::vector< WhipInfo > WhipInfoList;

class ioComboWhipSkill : public ioAttackSkill
{
public:
	enum SkillState
	{
		SS_READY,
		SS_ACTION,
		SS_LOOP,
		SS_FINISH,
		SS_END,
	};

protected:
	SkillState m_SkillState;

	DWORD	m_dwFireStartTime;
	DWORD	m_dwLoopStartTime;

	CEncrypt<DWORD> m_dwSkillProtectTime;
	CEncrypt<DWORD> m_dwCurSkillProtectTime;

	// 연타 관련
	bool		m_bAddKey;
	DWORD		m_dwCheckStartTime;
	DWORD		m_dwAddDuration;
	DWORD		m_dwEnableTime;

	ioHashString	m_LoopAniName;
	DWORD			m_dwLoopDuration;
	float			m_fLoopTimeRate;

	// Finish
	ioHashString m_FinishMotion;
	float m_fFinishMotionRate;

	// 공격처리 관련
	DWORD m_dwWhipWeaponIndex;

	WhipInfoList m_WhipInfoList;

	CEncrypt<float> m_fCurWhipRate;

	ioHashString m_CurWhipEffect;
	UniqueObjID m_dwCurWhipEffect;
	UniqueObjID m_dwFinishWhipEffect;

	// 이펙트 loop 처리
	DWORD m_dwEffectLoopCheckTime;
	DWORD m_dwEffectLoopDuration;

	// EndState
	float m_fEndMotionRate;
	ioHashString m_SkillEndMotion;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual bool CheckUseItemMesh();

	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );

	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool IsProtected( int iDefenseBreakType ) const;

protected:
	void SetActionState( ioBaseChar *pChar );
	void SetLoopState( ioBaseChar *pChar );
	void SetFinishState( ioBaseChar *pChar );
	void SetEndState( ioBaseChar *pChar, bool bSend );

	void CheckCurWhipInfo( ioBaseChar *pChar );
	void CheckFinishWhipInfo( ioBaseChar *pChar );

	void CheckEffectLoop( ioBaseChar *pChar );

public:
	ioComboWhipSkill();
	ioComboWhipSkill( const ioComboWhipSkill &rhs );
	virtual ~ioComboWhipSkill();
};

inline ioComboWhipSkill* ToComboWhipSkill( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_COMBOWHIP )
		return NULL;

	return dynamic_cast< ioComboWhipSkill* >(pAttack);
}
