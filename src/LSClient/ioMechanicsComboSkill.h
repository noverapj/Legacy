#pragma once

#include "ioAttackSkill.h"
#include "ioAttackAttribute.h"
#include "ioUserKeyInput.h"

class ioINILoader;
class ioBaseChar;
class ioItem;
class SP2Packet;

typedef struct tagShotInfo
{
	ioHashString m_ShotUpAni;
	ioHashString m_ShotDownAni;
	ioHashString m_ShotEffect;
} ShotInfo;

typedef std::vector< ShotInfo > ShotInfoList;

class ioMechanicsComboSkill : public ioAttackSkill
{
public:
	enum ProtectCheckType
	{
		PCT_TIME	= 1,
		PCT_ANI		= 2,
	};

	enum ComboAttackType
	{
		CAT_NORMAL	= 1,
		CAT_SHOT	= 2,
	};

	enum SkillState
	{
		SS_NONE,
		SS_MOVE,
		SS_COMBO,
	};

	enum SkillSyncType
	{
		SST_TARGET,
		SST_COMBO,
	};

protected:
	SkillState m_SkillState;

	AttributeList m_vComboList;
	AttributeList m_vJumpComboList;
	AttributeList m_vDownComboList;

	ShotInfoList m_vShotInfoList;
	ShotInfoList m_vJumpShotInfoList;
	ShotInfoList m_vDownShotInfoList;

	int m_iCurLinearCombo;
	DWORD m_dwNextComboTime;

	DWORD m_dwSkillProtectTime;
	DWORD m_dwCurSkillProtectTime;

	bool m_bAutoComboAction;

	ProtectCheckType m_ProtectCheckType;

	// end other attack
	bool m_bUseEndOtherAttack;
	bool m_bUsedEndOtherAttack;

	AttackAttribute m_EndOtherAttack;
	AttackAttribute m_JumpEndOtherAttack;
	AttackAttribute m_DownEndOtherAttack;

	// dummychar
	ioHashString m_DummyCharEffect;
	vDummyCharLoadInfoList m_DummyCharList;
	ioHashStringVec m_vDestroyDummyCharList;

	//
	CEncrypt<bool> m_bNoGravityCombo;
	CEncrypt<bool> m_bUsedComboList;
	CEncrypt<bool> m_bSetAttackFire;

	ioUserKeyInput::DirKeyInput m_CurDirKey;

	//
	ComboAttackType m_ComboAttackType;
	ioHashString m_ComboTargetName;
	bool m_bBallTarget;

	D3DXVECTOR3 m_vTargetPos;
	D3DXVECTOR3 m_vTargetDir;

	DWORD m_dwSkillMoveEndTime;

	TargetWoundType m_TargetWoundType;
	CEncrypt<float> m_fTargetAngle;
	CEncrypt<float> m_fTargetRange;
	CEncrypt<float> m_fTargetUpHeight;
	CEncrypt<float> m_fTargetDownHeight;

	CEncrypt<float> m_fAttMoveSpeed;
	CEncrypt<float> m_fAttMoveRange;
	CEncrypt<float> m_fAttMoveRate;

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

	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsEnableReserve() const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;

	virtual bool IsJumpState() const;
	virtual bool IsNoDropState() const;
	virtual bool IsCanSendControl() const;

protected:
	virtual void AttackSkillFire( ioBaseChar *pChar );

protected:
	void SetSkillActionState( ioBaseChar *pChar );

	void ProcessMoveState( ioBaseChar *pChar );
	
	void CheckLinearCombo( ioBaseChar *pChar );
	void SetNextLinearCombo( ioBaseChar *pChar, bool bOther, bool bSendNet );

	void SetNormalCombo( ioBaseChar *pChar, bool bOther, bool bSendNet );

	float GetTargetUpDownWeight( float fMaxAngle );
	void SetShotCombo( ioBaseChar *pChar, bool bOther, bool bSendNet );

	const AttributeList& GetComboList();
	const ShotInfoList& GetShotInfoList();
	AttackAttribute GetEndAttribute();

protected:
	void CheckFindTarget( ioBaseChar *pChar );
	bool IsEnableTargetState( ioBaseChar *pTarget );
	bool CompareTarget( ioBaseChar *pChar );

	bool CheckTargetValidate( ioBaseChar *pOwner );
	
public:
	ioMechanicsComboSkill();
	ioMechanicsComboSkill( const ioMechanicsComboSkill &rhs );
	virtual ~ioMechanicsComboSkill();
};

inline ioMechanicsComboSkill* ToMechanicsComboSkill( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_MECHANICS_COMBO )
		return NULL;

	return dynamic_cast< ioMechanicsComboSkill* >(pAttack);
}
