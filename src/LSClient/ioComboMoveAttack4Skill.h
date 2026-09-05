#pragma once

#include "ioNormalSkill.h"
#include "ioUserKeyInput.h"
#include "ioAttackAttribute.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioComboMoveAttack4Skill : public ioNormalSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_PRE_ACTION,
		SS_LOOP,
		SS_ACTION,
		SS_END_JUMP,
	};

	enum SkillSync
	{
		SSC_PRE_ACTION,
		SSC_LOOP,
		SSC_ACTION,
	};
	enum 
	{
		K_ATTACK,
		K_DEFENSE,
		K_JUMP,
	};
protected:
	CEncrypt<int> m_State;

	//Combo Info
	AttackAttribute m_AttackAttribute;
	AttackAttribute m_AttackAttributeA;
	AttackAttribute m_AttackAttributeS;

	// Attack
	DWORD m_dwProtectTime;
	DWORD m_dwCurProtectTime;

	ioHashString m_SkillLoopMotion;
	float m_fSkillLoopMotionRate;

	// Char
	float m_fCharJumpPower;

	DWORD m_dwPreActionEndTime;

	DWORD m_dwOrgLoopDuration;
	DWORD m_dwLoopDurationEnd;
	DWORD m_dwLoopDurationStart;

	// Circle
	float m_fCircleRange;
	float m_fCircleMoveSpeed;
	float m_fCircleHeightGap;
	float m_fCircleOffSet;

	D3DXVECTOR3 m_vCirclePos;

	ioHashString m_ReadyCircle;	
	ioHashString m_RedEffect;
	ioHashString m_BlueEffect;

	UniqueObjID m_dwCurMapEffect;
	ioUserKeyInput::DirKeyInput m_CurDirKey;

	bool			m_bSkillInJump;

	//점프
	CEncrypt<float>		m_fJumpPowerInJump;

	//점프중에 사용시
	CEncrypt<float>		m_fPreActionAniRate_Jump;
	ioHashString		m_strPreActionAni_Jump;

	bool m_bNoAttackDelay;
	DWORD m_dwAttackStartTime;
	DWORD m_dwAttackEndTime;
	DWORD m_dwAttackDurationTime;
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnSkillEnd( ioBaseChar *pChar );	
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );

	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );

	virtual bool CheckUseItemMesh();

public:
	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool IsAttackEndState() const;
	virtual bool IsNoDropState() const;

protected:
	virtual bool StartPreDelayAnimation( ioBaseChar *pChar );

protected:
	void SetPreActionState( ioBaseChar *pChar );
	void SetLoopState( ioBaseChar *pChar, bool bSend );
	void CheckLoopState( ioBaseChar *pChar, ioPlayStage *pStage );

	void SetActionState( ioBaseChar *pChar, ioPlayStage *pStage, int iKey, bool bSend );
	void SetEndState( ioBaseChar *pChar );

	void CheckCharMoveDir( ioBaseChar *pChar );

public:
	ioComboMoveAttack4Skill();
	ioComboMoveAttack4Skill( const ioComboMoveAttack4Skill &rhs );
	virtual ~ioComboMoveAttack4Skill();
};

inline ioComboMoveAttack4Skill* ToComboMoveAttack4Skill( ioSkill *pSkill )
{
	ioNormalSkill *pNormal = ToNormalSkill( pSkill );
	if( !pNormal || pNormal->GetSubSkillType() != NST_COMBO_MOVE_ATTACK4 )
		return NULL;

	return dynamic_cast< ioComboMoveAttack4Skill* >( pSkill );
}

