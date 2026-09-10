#pragma once

class ioINILoader;
class ioBaseChar;

#include "ioAttackSkill.h"
#include "ioUserKeyInput.h"

class ioRunningAttackSkill6 : public ioAttackSkill
{
public:
	enum RunningState
	{
		RST_NONE,
		RST_READY,
		RST_RUN,
		RST_END,
	};

	enum
	{
		SST_ROTATE,		
	};

protected:
	bool m_bUseFireEnd;
	bool m_bDirection;

	RunningState m_RunningState;
	ioHashStringVec m_ActionBuffList;

	ioHashString m_RunningReadyAni;
	float m_fRunningReadyAniRate;
	DWORD m_dwRunningReadyEndTime;

	DWORD m_dwLoopTime;
	DWORD m_dwRunningEndTime;

	DWORD m_dwRunningStartTime;
	DWORD m_dwRunEndEnableTime;

	ioHashString m_RunningEndAni;
	float m_fRunningEndAniRate;
	DWORD m_dwRunningEndEndTime;

	CEncrypt<float> m_fRunSpeed;
	bool m_bTargetRot;
	bool m_bLeftRot;
	ioUserKeyInput::DirKeyInput m_CurDirKey;

	CEncrypt<DWORD> m_dwRotateTime;

	DWORD m_dwFireDuration;

	bool m_bDisableRunColSkip;
protected:
	CEncrypt<int> m_iMaxWeaponWay;
	int m_iWeaponWay;
	BulletInfoList m_BulletInfoList;

public:	// overide ioSkill
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );

	void SetReadyState( ioBaseChar *pChar );
	void SetRunState( ioBaseChar *pChar );
	void SetRunEndState( ioBaseChar *pChar );

	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsEndState() const;
	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );

	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;

protected:
	virtual void AttackSkillFire( ioBaseChar *pChar );

	void CheckKeyInput( ioBaseChar *pOwner );
	void ProcessRotate( ioBaseChar *pOwner );

	void ClearData();
public:
	ioRunningAttackSkill6();
	ioRunningAttackSkill6( const ioRunningAttackSkill6 &rhs );
	virtual ~ioRunningAttackSkill6();
};

inline ioRunningAttackSkill6* ToRunningAttackSkill6( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_RUNNING_ATTACK6 )
		return NULL;

	return dynamic_cast< ioRunningAttackSkill6* >( pAttack );
}

