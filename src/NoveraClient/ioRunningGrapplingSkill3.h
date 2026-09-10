#pragma once

#include "ioAttackSkill.h"

class ioINILoader;
class ioBaseChar;

class ioRunningGrapplingSkill3 : public ioAttackSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_RUN,
		SS_ACTION,
		SS_COMBO,
		SS_END,
	};

	enum SkillSync
	{
		SSC_GRAPPLING,
		SSC_ROTATE,
		SSC_COMBO,
	};

protected:
	SkillState		m_SkillState;

	AttackAttribute m_EndAttackAttribute;

	ioHashStringVec	m_BuffSkill;
	ioHashStringVec	m_EnemyBuffSkill;

	// Effect/Sound for LoopAnimation
	ioHashString	m_LoopEffect;
	ioHashString	m_LoopSound;
	UniqueObjID m_LoopSoundID;

	// Grappling Motion
	ioHashString	m_GrapLoopMotion;
	CEncrypt<float>	m_fGrapLoopMotionRate;

	// Grappling Motion Time
	DWORD			m_dwGrapLoopStartTime;
	DWORD			m_dwGrapLoopEndTime;
	CEncrypt<DWORD> m_dwGrapLoopDuration;

	CEncrypt<DWORD> m_dwSkillLoopProtectTime;

	// Target Position
	CEncrypt<float> m_fTargetLoopGapRange;
	CEncrypt<float> m_fCharHeightRate;

	// About Running Speed
	DWORD			m_dwRunLoopStartTime;
	DWORD			m_dwRunLoopEndTime;
	CEncrypt<DWORD>	m_dwRunLoopDuration;
	CEncrypt<float> m_fRunSpeed;
	CEncrypt<DWORD> m_dwRunRotateTime;

	// About target
	ioHashString m_GrapplingTargetName;
	bool m_bTargetRot;

	// About Move Input
	bool		m_bLeftRot;
	ioUserKeyInput::DirKeyInput m_CurDirKey;

protected:
	AttributeList m_vComboList;
	
	int m_iCurLinearCombo;
	DWORD m_dwNextComboTime;
	
	bool m_bUseChangeDirection;
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
	virtual bool IsCanSendControl() const;

	virtual bool CheckUseItemMesh();

	virtual void SetCancelSkill( ioBaseChar *pChar );

protected:
	virtual void AttackSkillFire( ioBaseChar *pChar );

	void CheckKeyInput( ioBaseChar *pOwner );
	void ProcessRotate( ioBaseChar *pOwner );

	void AddEnemyBuff( ioBaseChar *pOwner );
	void RemoveEnemyBuff( ioBaseChar *pOwner );
	void AddOwnerBuff( ioBaseChar *pOwner );
	void RemoveOwnerBuff( ioBaseChar *pOwner );

protected:
	void SetSkillRunState( ioBaseChar *pChar );
	void SetSkillRunEndState( ioBaseChar *pChar );
	void SetSkillActionState( ioBaseChar *pChar );
	void SetSkillEndState( ioBaseChar *pChar, bool bSuccess );

protected:
	void PlayLoopSound( ioBaseChar *pChar );
	void StopLoopSound();

protected:
	void CheckLinearCombo( ioBaseChar *pChar );
	void SetNextLinearCombo( ioBaseChar *pChar );
public:
	ioRunningGrapplingSkill3();
	ioRunningGrapplingSkill3( const ioRunningGrapplingSkill3 &rhs );
	virtual ~ioRunningGrapplingSkill3();
};

inline ioRunningGrapplingSkill3* ToRunningGrappling3( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_RUNNING_GRAPPLING3 )
		return NULL;

	return dynamic_cast< ioRunningGrapplingSkill3* >( pAttack );
}

