#pragma once
#include "ioattackskill.h"
class ioTargetMoveComboAttackSkill :
	public ioAttackSkill
{
protected:
	enum SkillState
	{
		SS_NONE,
		SS_MOVEATTACK,
		SS_NEXTACTION,
		SS_LINEAR,
		SS_GRAPPLING,
		SS_END,
	};

	enum SkillEndState
	{
		SES_NONE,
		SES_ATTACK,
		SES_LAND_ATTACK,
	};
protected:
	SkillState m_SkillState;
	SkillEndState m_SkillEndState;
	//protect time
	DWORD		 m_dwSkillProtectStartTime;
	DWORD		 m_dwSkillProtectTime ;
	//MoveAttack
	float  m_fRunSpeed;
	float  m_fCurMoveDist;				//현재 이동 거리.
	float  m_fMaxMoveDist;
	DWORD  m_dwMoveAttackWeaponIndex;
	//NextAction
	ioHashString m_NextActionAnimation;
	float m_fNextActionAniRate;
	bool		m_bIsJumpPower;
	//linear combo
	AttributeList m_ComboList;
	int m_iCurLinearCombo;
	bool  m_bUseLinearAutoTargetValue;
	float m_fLinearAutoTargetAngle;
	float m_fLinearAutoTargetMinRange;
	float m_fLinearAutoTargetMaxRange;
	bool  m_bLinearAutoTargetEnableDownAim;
	//grapple
	float			m_fGrapplingJumpPower;
	int				m_iLastAttack;
	AttackAttribute m_GrappleAttribute;
	ioHashString m_GrapplingAnimation;
	float m_fGrapplingAniRate;
	AttackAttribute m_LandAttackAttribute;
	ioHashString m_GrapplingTargetName;
	float m_fCharHeightRate;
	float m_fTargetGapRange;
	//skill end
	AttackAttribute m_EndAttackAttribute;
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();
public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual void SetNextSkillState( ioBaseChar *pWounder, ioWeapon *pWeapon = NULL );
	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );
	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
	virtual bool SetSkillGrapplingState( ioBaseChar *pChar, const ioHashString &szTargetName );
	virtual D3DXVECTOR3 GetGrapplingTargetPos( ioBaseChar *pChar );
	//virtual bool IsAttackEndState() const;
	//virtual bool IsEndState() const;
	virtual bool IsNoDropState() const;
	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual AttackAttribute& GetAttribute();
	virtual const AttackAttribute& GetAttributeConst() const;
protected:
	void ClearData();
	//move attack state
	void SetMoveAttackState( ioBaseChar *pChar, ioPlayStage *pStage );
	void DestroyMoveAttackWeapon( ioBaseChar *pChar );
	//맵 충돌체크
	bool CheckMove( ioBaseChar *pChar );
	//next action state
	void SetNextActionState( ioBaseChar *pChar );
	//linear combo
	void CheckLinearCombo( ioBaseChar *pChar );
	//next linear combo
	void SetNextLinearCombo( ioBaseChar *pChar );
	void SearchTargetTracking( ioBaseChar *pChar );
	//grappling state
	void SetGrapplingAttackState( ioBaseChar *pChar );
	//땅에 착지 체크
	bool CheckLanding( ioBaseChar *pChar );
	//end state
	void SetEndState( ioBaseChar *pChar, SkillEndState endstate );
public:
	ioTargetMoveComboAttackSkill(void);
	ioTargetMoveComboAttackSkill( const ioTargetMoveComboAttackSkill &rhs );
	~ioTargetMoveComboAttackSkill(void);
};

inline ioTargetMoveComboAttackSkill* ToTargetMoveComboAttackSkill( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_TARGET_MOVE_COMBO_ATTACK )
		return NULL;

	return dynamic_cast< ioTargetMoveComboAttackSkill* >(pAttack);
}