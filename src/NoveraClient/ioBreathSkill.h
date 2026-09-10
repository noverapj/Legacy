#pragma once

#include "ioAttackSkill.h"
#include "ioAttackAttribute.h"

#include "ioUserKeyInput.h"

class ioINILoader;
class ioBaseChar;
class ioItem;
class SP2Packet;

class ioBreathSkill : public ioAttackSkill
{
public:
	enum ActionState
	{
		AS_READY,
		AS_NORMAL,
		AS_FLAME,	// 단타 스킬. 이후로 관련 변수명은 flame으로 한다.
		AS_BREATH,	// 연타 스킬. 이후로 관련 변수명은 breath로 한다.
		AS_END,
	};

	enum SyncType
	{
		SST_CHANGE_STATE,
		SST_CHANGE_DIR,
	};

protected:
	ActionState m_ActionState;

	bool m_bDirection;
	bool m_bSetMoveAni;

	ioHashString m_NormalMotion;
	ioHashString m_NormalMoveMotion;

	ioHashString m_BreathMotion;
	ioHashString m_BreathMoveMotion;

	CEncrypt<float> m_fBreathMoveRate;

	//
	bool m_bReserveAttackKey;
	CEncrypt<DWORD> m_dwAddDuration;
	CEncrypt<DWORD> m_dwEnableKeyTime;
	DWORD m_dwCheckStartTime;

	//
	bool m_bTargetRot;
	CEncrypt<DWORD> m_dwRotateSpeed;
	ioUserKeyInput::DirKeyInput m_CurDirKey;

	//
	DWORD m_dwBreathStartTime;

	int m_iWeaponFireCnt;
	WeaponInfo m_WeaponInfo;

	DWORD m_dwWeaponCreateTime;
	DWORD m_dwCheckWeaponTime;

	//
	CEncrypt<DWORD> m_dwSkillFullTime;
	CEncrypt<DWORD> m_dwSkillProtectTime;
	CEncrypt<DWORD> m_dwCurSkillProtectTime;

	// EndState
	float m_fEndMotionRate;
	ioHashString m_SkillEndMotion;

	//
	CEncrypt<bool> m_bCheckSkillGauge;

protected:
	ioHashString m_NormalStateEffect;
	ioHashString m_FlameStateEffect;
	ioHashString m_BreathStateEffect;

	UniqueObjID m_dwNormalStateEffect;
	UniqueObjID m_dwFlameStateEffect;
	UniqueObjID m_dwBreathStateEffect;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar* pChar, ioPlayStage* pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual bool CheckUseItemMesh();

	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );
	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );

	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool IsProtected( int iDefenseBreakType ) const;

	virtual bool IsEnableReserve() const;

	virtual float GetSkillMoveRate() const;

protected:
	virtual void AttackSkillFire( ioBaseChar *pChar );

protected:
	void SetNormalState( ioBaseChar *pChar );
	void SetFlameState( ioBaseChar *pChar );
	void SetBreathState( ioBaseChar *pChar );
	void SetEndState( ioBaseChar *pChar );

	void ProcessFlameState( ioBaseChar *pChar );
	void ProcessBreathState( ioBaseChar *pChar );

	void CheckKeyInput( ioBaseChar* pChar );
	void ProcessRotate( ioBaseChar* pChar );

	void CheckNormalMoveAni( ioBaseChar *pChar );
	void CheckBreathMoveAni( ioBaseChar *pChar );

	bool CheckSkillGauge( ioBaseChar *pChar );

public:
	void CreateWeapon( ioBaseChar *pOwner );

	bool IsEnableMoveState();

public:
	ioBreathSkill();
	ioBreathSkill( const ioBreathSkill &rhs );
	virtual ~ioBreathSkill();
};

inline ioBreathSkill* ToBreathSkill( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_BREATH )
		return NULL;

	return dynamic_cast< ioBreathSkill* >(pAttack);
}

