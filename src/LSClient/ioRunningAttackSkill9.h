#pragma once

class ioINILoader;
class ioBaseChar;

#include "ioAttackSkill.h"

class ioRunningAttackSkill9 : public ioAttackSkill
{
protected:
	enum SkillState
	{
		SS_None,
		SS_Run_Ready,
		SS_Run_Normal_Loop,
		SS_Run_Charge_Loop,
		SS_Run_Charge_End,
		SS_Rotate,
		SS_Run_End,
		SS_End,
	};
	
protected:
	DWORD m_dwSkillProtectTime;
	
	ioHashString m_szRunReadyAni;
	float m_fRunReadyAniRate;
	ioHashString m_szRunLoopAni;
	float m_fRunLoopAniRate;
	ioHashString m_szRunChargeEndLoopAni;
	float m_fRunChargeEndLoopAniRate;
	ioHashString m_szRunEndAni;
	float m_fRunEndAniRate;

	float m_fNormalMoveSpeed;
	float m_fChargeMoveSpeed;
	float m_fEndMoveSpeed;
	float m_fForceSameMoveSpeed;

	DWORD m_dwNormalRotateSpeed;
	DWORD m_dwChargeRotateSpeed;
	DWORD m_dwEndRotateSpeed;
	DWORD m_dwForceSameRotateSpeed;

	DWORD m_dwMaxRunLoopTime;
	DWORD m_dwMaxChargeTime;
	DWORD m_dwMaxChargeRunLoopTime;

	float m_fJumpPower;
	DWORD m_dwJumpTime;

	float m_fJumpHeightGap;

	ioHashStringVec m_AreaWeaponList;
	DWORD m_dwCreateAreaTime;

	bool m_bNoDropState;

	bool m_bSetForceSameMove;

protected:
	SkillState m_SkillState;
	DWORD m_dwCurSkillProtectTime;

	DWORD m_dwCheckMaxRunLoopTime;
	DWORD m_dwCheckMaxChargeRunLoopTime;
	DWORD m_dwCheckMaxChargeTime;
	DWORD m_dwCheckJumpTime;

	bool m_bTargetRot;
	bool m_bLeftRot;
	ioUserKeyInput::DirKeyInput m_CurDirKey;

	DWORD m_dwAreaWeaponIndex;
	DWORD m_dwCheckAreaWeaponTime;
	int m_nAreaWeaponCnt;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );
	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );

	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool IsAttackEndState() const;
	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual SkillCharColType IsCharSkipState() const;
	virtual bool IsEntityCollisionSkipState() const;
	bool IsNoFallDropZone() const;
	virtual bool IsCanSendControl() const;
	
protected:
	void Init();
	void SetRunningReadyState( ioBaseChar *pChar );
	void SetRunningNormalLoopState( ioBaseChar *pChar );
	void SetRunningChargeLoopState( ioBaseChar *pChar );
	void SetRunningChargeEndState( ioBaseChar *pChar );
	void SetRunningEndState( ioBaseChar *pChar );

protected:
	void ProcessLoopState( ioBaseChar *pChar, float fMoveSpeed, DWORD dwRotateSpeed );
	void CheckRotateInput( ioBaseChar *pChar );
	void CheckRotate( ioBaseChar *pChar, DWORD dwRotateSpeed );

protected:
	bool CheckAttackInput( ioBaseChar *pChar );
	bool CheckAttackRelease( ioBaseChar *pChar );

protected:
	void CheckChargeJump( ioBaseChar *pChar );
	void CheckChargeAreaWeapon( ioBaseChar *pChar );
	bool IsFloatState( ioBaseChar *pChar );

public:
	ioRunningAttackSkill9();
	ioRunningAttackSkill9( const ioRunningAttackSkill9 &rhs );
	virtual ~ioRunningAttackSkill9();
};

inline ioRunningAttackSkill9* ToRunningAttackSkill9( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_RUNNING_ATTACK9 )
		return NULL;

	return dynamic_cast< ioRunningAttackSkill9* >( pAttack );
}