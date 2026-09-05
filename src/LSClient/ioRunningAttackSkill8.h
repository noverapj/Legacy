#pragma once

class ioINILoader;
class ioBaseChar;

#include "ioAttackSkill.h"
#include "ioUserKeyInput.h"

class ioRunningAttackSkill8 : public ioAttackSkill
{
public:
	enum RunningState
	{
		RST_NONE,
		RST_READY,
		RST_RUN,
		RST_RUN_JUMP,
		RST_THROW,
		RST_END,
	};

	enum
	{
		SST_RUN,
		SST_ROTATE,
		SST_THROW,
		SST_JUMP,
		SST_INCREASE_RUN_SPEED,
		SST_SKILL_EXPLICIT_FIRE,
	};

	enum RunningActionType
	{
		RAT_NONE,
		RAT_STOP,
		RAT_JUMP,
		RAT_ATTACKKEY,
	};

	enum RunningActionEndType
	{
		RAET_ALL,
		REAT_ATTACK,
		REAT_DEFENCE,
		REAT_JUMP,
		REAT_NOT_ATTACK,
		REAT_NOT_DEFENCE,
		REAT_NOT_JUMP,
	};

	enum RunningExtraAttackType
	{
		RET_NONE,
		RET_BLAZE,
		RET_FIRE_BALL,
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
	CEncrypt<float> m_fRunningEndSpeed;
	bool m_bTargetRot;
	bool m_bLeftRot;
	ioUserKeyInput::DirKeyInput m_CurDirKey;

	CEncrypt<DWORD> m_dwRotateTime;

	DWORD m_dwFireDuration;

	bool m_bDisableRunColSkip;

	AttackAttribute m_LastThrowAttack;

protected:
	CEncrypt<bool> m_bUseRunningJump;
	CEncrypt<float> m_fRunningJumpAmt;
	CEncrypt<int> m_iMaxRunningColJumpCnt;
	CEncrypt<int> m_iCurRunningJumpCnt;

	WeaponInfo m_RunningJumpLandAttack;

protected:
	RunningExtraAttackType m_RunExtraAttackType;
	WeaponInfo m_RunExtraAttack;

	CEncrypt<DWORD> m_dwExtraAttackCheckTime;
	CEncrypt<DWORD> m_dwExtraAttackDuration;
	int m_iCurBlazeCnt;

	float m_fRunExtraAttackOffsetFr;
	float m_fRunExtraAttackOffsetHe;
	float m_fRunExtraAttackOffsetSi;
	float m_fRunExtraAttackAngle;

protected:
	//Ãß°¡ yong jun
	bool					m_bUseAttackKey;
	float					m_fCurSpeed;	
	float					m_fSpeedIncreaseTic;
	float					m_fMinSpeed;
	float					m_fMaxSpeed;
	DWORD					m_dwSpeedIncreaseTicTime;
	DWORD					m_dwNextSpeedIncreaseTime;
	bool					m_bAlready_Max_Speed;
	RunningActionEndType	m_eRunningActionEndType;
	DWORD					m_dwSkillProtectTime;

protected:
	struct TimeWeapon
	{
		AttackAttribute	Info;
		DWORD			dwTime;
	};
	typedef std::vector<TimeWeapon> TimeWeaponList;
	TimeWeaponList	m_TimeWeaponList;
	int				m_iCurrTimeWeaponIndex;
	int				m_iTimeAttackHeightOffset;

protected:
	void CheckAttackKey( ioBaseChar *pChar );
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
	void SetRunEndStateByJump( ioBaseChar *pChar );
	void SetLastThrowState( ioBaseChar *pChar );
	void SetLastThrowEndState( ioBaseChar *pChar );

	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsEndState() const;
	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );

	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;

	virtual void SetInfoByApplyWeapon( ioBaseChar *pOwner, ioBaseChar *pTarget, DWORD dwWeaponBaseIndex, bool bBlock );
	virtual bool IsNoDropState() const;

	virtual bool IsAttackEndState() const;

protected:
	virtual void AttackSkillFire( ioBaseChar *pChar );
	void AttackSkillExplicitFire( ioBaseChar *pChar );

	void CheckKeyInput( ioBaseChar *pOwner );
	void ProcessRotate( ioBaseChar *pOwner );

	void ProcessExtraAttack( ioBaseChar *pOwner );
	void ProcessSkillExplicitFire( ioBaseChar *pChar );
	void AttackSkillExplicitFire( ioBaseChar *pChar, int iWeaponIndex, D3DXVECTOR3 vPos );

	void ClearData();

	int CheckRunningActionType( ioBaseChar *pOwner );

public:
	ioRunningAttackSkill8();
	ioRunningAttackSkill8( const ioRunningAttackSkill8 &rhs );
	virtual ~ioRunningAttackSkill8();
};

inline ioRunningAttackSkill8* ToRunningAttackSkill8( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_RUNNING_ATTACK8 )
		return NULL;

	return dynamic_cast< ioRunningAttackSkill8* >( pAttack );
}

