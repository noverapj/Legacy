#pragma once

class ioINILoader;
class ioBaseChar;

#include "ioAttackSkill.h"
#include "ioUserKeyInput.h"


class ioMechanicsFireSkill : public ioAttackSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_CHARGE,
		SS_ACTION,
		SS_END,
	};

	enum SkillSyncType
	{
		SST_KEY,
		SST_ACTION,
	};

protected:
	SkillState m_SkillState;

	CEncrypt<int> m_iMaxFireCnt;
	CEncrypt<int> m_iCurFireCnt;

	CEncrypt<DWORD> m_dwSkillProtectTime;
	CEncrypt<DWORD> m_dwCurSkillProtectTime;

	ioHashString m_ChargeMotion;
	CEncrypt<DWORD> m_dwChargeDuration;
	CEncrypt<DWORD> m_dwFireEnableDuration;

	CEncrypt<DWORD> m_dwChargeMaxTime;
	CEncrypt<DWORD> m_dwFireEnableTime;
	CEncrypt<DWORD> m_dwActionEndTime;

	ioHashString m_ChargeEffect;
	ioHashString m_ChargeMaxEffect;
	UniqueObjID m_dwChargeEffect;
	UniqueObjID m_dwChargeMaxEffect;

	CEncrypt<bool> m_bSetMaxCharge;
	CEncrypt<bool> m_bTargetRot;
	CEncrypt<bool> m_bSetUpKey;
	CEncrypt<bool> m_bSetDownKey;

	CEncrypt<float> m_fUpDownSpeed;

	ioHashString m_ChargeUpEffect;
	ioHashString m_ChargeDownEffect;
	UniqueObjID m_dwUpDownEffect;

	CEncrypt<DWORD> m_dwRotateTime;
	ioUserKeyInput::DirKeyInput m_PreDirKey;
	ioUserKeyInput::DirKeyInput m_CurDirKey;

	bool		m_bJumpSkill;
public:
	bool		IsJumpSkill();

public:	// overide ioSkill
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );

	virtual bool CheckUseItemMesh();

	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool IsNoDropState() const;
	virtual bool IsEnableReserve() const;

public:
	inline int GetSkillState() const { return m_SkillState; }

protected:
	virtual void AttackSkillFire( ioBaseChar *pChar );

	void SetChargeState( ioBaseChar *pChar, bool bFirst );
	void SetEndState( ioBaseChar *pChar );

protected:
	void CheckKeyInput( ioBaseChar *pOwner );
	void CheckUpDownEffect( ioBaseChar *pOwner );

	void ApplyActionState( ioBaseChar *pOwner, SP2Packet &rkPacket );

	void ProcessRotate( ioBaseChar *pOwner );
	void ProcessMove( ioBaseChar *pOwner );

public:
	ioMechanicsFireSkill();
	ioMechanicsFireSkill( const ioMechanicsFireSkill &rhs );
	virtual ~ioMechanicsFireSkill();
};

inline ioMechanicsFireSkill* ToMechanicsFireSkill( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_MECHANICS_FIRE )
		return NULL;

	return dynamic_cast< ioMechanicsFireSkill* >( pAttack );
}

