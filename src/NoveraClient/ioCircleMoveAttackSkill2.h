#pragma once


#include "ioNormalSkill.h"
#include "ioUserKeyInput.h"
#include "ioAttackAttribute.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioCircleMoveAttackSkill2 : public ioNormalSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_WAIT,
		SS_ACTION,
		SS_END,
	};

	enum SkillSync
	{
		SSC_ACTION,
	};

protected:
	CEncrypt<int> m_State;

	// Attack
	DWORD m_dwFireStartTime;

	AttackAttribute m_AttackAttribute;
	AttackAttribute m_LandAttackAttribute;


	// Circle
	float m_fCircleRange;
	float m_fCircleMoveSpeed;
	float m_fCircleHeightGap;
	float m_fCircleOffSet;

	D3DXVECTOR3 m_vCirclePos;

	// Char
	float m_fCharMoveSpeed;
	float m_fCurCharMoveSpeed;
	float m_fCharJumpPower;
	D3DXVECTOR3 m_vCharMoveDir;

	ioHashString m_ReadyCircle;
	ioHashString m_TargetCircleEffect;

	ioHashString m_RedEffect;
	ioHashString m_BlueEffect;

	UniqueObjID m_dwCurMapEffect;
	UniqueObjID m_dwTargetCircleEffect;

	float m_fEndMotionRate;
	ioHashString m_SkillEndMotion;

	ioHashString m_StartEffect;
	ioHashString m_EndEffect;

	// Wait
	ioHashString m_szWaitMotion;
	float m_fWaitMotionRate;

	DWORD m_dwWaitStartTime;
	CEncrypt<DWORD> m_dwWaitDuration;

	// LoopCnt
	CEncrypt<int> m_iMaxLoopCnt;
	CEncrypt<int> m_iCurLoopCnt;

	// etc
	CEncrypt<DWORD> m_dwSkillProtectTime;
	CEncrypt<DWORD> m_dwCurSkillProtectTime;

	BuffInfoList m_OwnerBuffList;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool IsProtected( int iDefenseBreakType ) const;

	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnSkillEnd( ioBaseChar *pChar );	
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );

	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );

	virtual bool CheckUseItemMesh();

public:
	virtual bool IsEndState() const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual SkillCharColType IsCharSkipState() const;

protected:
	virtual bool StartPreDelayAnimation( ioBaseChar *pChar );
	virtual void SetNormalSkill( ioBaseChar *pChar );

	void LoadOwnerBuffList( ioINILoader &rkLoader );
	bool SetOwnerBuffList( ioBaseChar *pChar, ioPlayStage* pStage );
	void RemoveOwnerBuffList( ioBaseChar *pChar );

protected:
	void ProcessAction( ioBaseChar *pChar );
	void ProcessMove( ioBaseChar *pChar, ioPlayStage *pStage );
	void ProcessEnd( ioBaseChar *pChar );
	bool CheckLanding( ioBaseChar *pChar );

	void SetWaitState( ioBaseChar *pChar );
	void SetActionState( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetEndState( ioBaseChar *pChar );

	float GetFlightDuration( float fCurGravity );

public:
	ioCircleMoveAttackSkill2();
	ioCircleMoveAttackSkill2( const ioCircleMoveAttackSkill2 &rhs );
	virtual ~ioCircleMoveAttackSkill2();
};

inline ioCircleMoveAttackSkill2* ToCircleMoveAttackSkill2( ioSkill *pSkill )
{
	ioNormalSkill *pNormal = ToNormalSkill( pSkill );
	if( !pNormal || pNormal->GetSubSkillType() != NST_CIRCLE_MOVE_ATTACK2 )
		return NULL;

	return dynamic_cast< ioCircleMoveAttackSkill2* >( pSkill );
}

