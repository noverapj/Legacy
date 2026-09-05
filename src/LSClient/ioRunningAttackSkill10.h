#pragma once

class ioINILoader;
class ioBaseChar;

#include "ioAttackSkill.h"
#include "ioUserKeyInput.h"

class ioRunningAttackSkill10 : public ioAttackSkill
{
public:
	enum RunningState
	{
		RST_NONE,
		RST_RUN,
		RST_STUN_END,
		RST_FORCE_END,
		RST_END,
	};

	enum RunEndType
	{
		RET_NORMAL_END,
		RET_FORCE_END,		
		RET_STUN_END,		
	};

	enum
	{
		SST_RUN,
		SST_ROTATE,
		SST_AREA_WEAPON,
	};

	enum SkillEndKey
	{
		SEK_ALL,
		SEK_ATTACK,
		SEK_DEFENCE,
		SEK_JUMP,
	};

	enum SkillNameType
	{
		SKN_DEFAULT,
		SKN_TITAN,
	};

protected:
	SkillNameType m_ThisSkillName;

	CEncrypt<bool> m_bUseFireEnd;

	RunningState m_RunningState;
	RunEndType m_RunEndType;
	RunEndType m_RunKeyEndType;

	SkillEndKey m_SkillEndKey;

	CEncrypt<bool> m_bSkillEndJump;
	CEncrypt<bool> m_bStartFly;

	ioHashStringVec m_ActionBuffList;

	DWORD m_dwFireStartTime;
	DWORD m_dwLoopTime;
	DWORD m_dwRunningEndTime;
	DWORD m_dwRunningStartTime;
	DWORD m_dwRunEndEnableTime;
	DWORD m_dwRunningEndEndTime;
	//
	DWORD		 m_dwFlyUpDuration;

	CEncrypt<DWORD> m_dwSkillProtectedTime;
	CEncrypt<float> m_fRunSpeed;

	ioUserKeyInput::DirKeyInput m_CurDirKey;

	CEncrypt<DWORD> m_dwRotateTime;
	CEncrypt<DWORD> m_dwFireDuration;
	CEncrypt<bool> m_bDisableRunColSkip;
	CEncrypt<float> m_fSkillMoveDecrease;

	SkillEndJumpType m_JumpEndType;

	AttackAttribute m_EndAttackAttribute;
	AttackAttribute m_StunEndAttackAttribute;

	DWORD m_dwCharRotateTime;

	CEncrypt<bool> m_bSetUpKey;
	CEncrypt<bool> m_bSetDownKey;
	float m_fUpDownSpeed;

	WeaponInfo m_RunExtraAttack;

	CEncrypt<DWORD> m_dwExtraAttackCheckTime;
	CEncrypt<DWORD> m_dwExtraAttackDuration;

	int m_iCurBlazeCnt;
	float m_fRunExtranWeaponOffsetZ;
	float m_fRunExtranWeaponOffsetY;

	DWORD m_dwCurSkillProtectTime;

	ioHashStringVec m_OwnerBuffList;
	bool m_bSetStartPos;
	D3DXVECTOR3 m_vStartPos;
	D3DXQUATERNION m_qtStartRot;

	ioHashString m_AreaWeaponName;
	D3DXVECTOR3  m_vAreaWeaponRotAngle;
	float m_fAreaWeaponOffsetZ;
	float m_fAreaWeaponOffsetY;
	DWORD m_dwAreaWeaponCheckTime;
	DWORD m_dwAreaWeaponDuration;

	//value
	float		 m_fFlayUpMaxRange;
	float		 m_fFlyUpAngle;
	float		 m_fFlyMoveSpeed;
	float		 m_fFlyCurRange;
	float		 m_fFlyUpDownSpeed;

public:	// overide ioSkill
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	void SetRunState( ioBaseChar *pChar );
	void SetForceEndState( ioBaseChar *pChar );
	void SetStunEndState( ioBaseChar *pChar );
	void SetEndState( ioBaseChar *pChar );

	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsEndState() const;
	virtual bool IsAttackEndState() const;
	virtual SkillEndJumpType GetSkillEndNextJumpMustEnable();

	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );
	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );

	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual void SetCreateWeaponDependency( ioBaseChar *pOwner, ioPlayStage *pStage );
	virtual bool IsNoDropState() const;

protected:
	virtual void AttackSkillFire( ioBaseChar *pChar );

	bool CheckSkillEndKey( ioBaseChar *pOwner );
	void CheckSkillEnd( ioBaseChar *pOwner, RunEndType endType );
	void CheckKeyInput( ioBaseChar *pOwner );

	void ProcessRotate( ioBaseChar *pOwner );
	void ProcessCharRotate( ioBaseChar *pChar );
	bool ProcessUpDown( ioBaseChar *pChar );
	void ProcessExtraAttack( ioBaseChar *pOwner );
	void ProcessAreaWeapon( ioBaseChar *pOwner );
	//
	bool ProcessFlyUp( ioBaseChar *pChar );

	void ClearData();
	bool CheckControlWeapon( ioBaseChar *pChar, DWORD dwCurTime  );

protected:
	void LoadOwnerBuffList( ioINILoader &rkLoader );
	void SetOwnerBuffList( ioBaseChar *pOwner );
	void RemoveOwnerBuffList( ioBaseChar *pOwner );

public:
	ioRunningAttackSkill10();
	ioRunningAttackSkill10( const ioRunningAttackSkill10 &rhs );
	virtual ~ioRunningAttackSkill10();
};

inline ioRunningAttackSkill10* ToRunningAttackSkill10( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_RUNNING_ATTACK10 )
		return NULL;

	return dynamic_cast< ioRunningAttackSkill10* >( pAttack );
}

