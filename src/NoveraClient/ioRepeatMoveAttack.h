#pragma once

#include "ioAttackSkill.h"
#include "ioUserKeyInput.h"

class ioINILoader;
class ioBaseChar;
class ioItem;
class SP2Packet;

class ioRepeatMoveAttack : public ioAttackSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_MOVE,
		SS_ATTACK,
		SS_ROTATE_MOVE,
		SS_END,
	};

	enum MoveDir
	{
		MD_Forward,
		MD_RightForward,
		MD_Right,
		MD_RightBackward,
		MD_Backward,
		MD_LeftBackward,
		MD_Left,
		MD_LeftForward,
	};

protected:
	//스킬 상태
	SkillState m_SkillState;
	//스킬 보호 시간
	DWORD m_dwSkillProtectTime;
	//현재 스킬 보호 시간
	DWORD m_dwCurSkillProtectTime;

	DWORD m_dwAttackReserveTime;

	AttributeList m_vMoveInfo;
	AttackAttribute m_AttackInfo;

	int m_nMaxRepeatCnt;
	int m_nCurRepeatCnt;

	bool m_bNoDropState;
	bool m_bUseActiveCnt;

	std::vector<D3DXVECTOR3> m_vOffsetList;
	int m_nCurOffset;

	ioHashString m_szWeaponRedEffect;
	ioHashString m_szWeaponBlueEffect;

	bool m_bChangeDir;
	ioUserKeyInput::DirKeyInput m_DirKey;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );
	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
		
	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;

	virtual bool IsAttackEndState() const;
	
protected:
	bool CheckKeyInput( ioBaseChar *pChar );

protected:
	void SetMoveState( ioBaseChar *pChar, int nDir );
	void SetAttackState( ioBaseChar *pChar );
	void SetRotateMoveState( ioBaseChar *pChar );
	void SetEndState( ioBaseChar *pChar );

public:
	bool IsNoFallDropZone() const;
	virtual bool IsUseActiveCnt() const;
	virtual int GetMaxActiveCnt( ioBaseChar *pChar ) const;
	virtual int GetCurActiveCnt( ioBaseChar *pChar ) const;

protected:
	void CreateWeaponEffect( ioBaseChar *pChar, ioPlayStage *pStage );

public:
	ioRepeatMoveAttack();
	ioRepeatMoveAttack( const ioRepeatMoveAttack &rhs );
	virtual ~ioRepeatMoveAttack();
};

inline ioRepeatMoveAttack* ToRepeatMoveAttack( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_REPEAT_MOVE_ATTACK )
		return NULL;

	return dynamic_cast< ioRepeatMoveAttack* >( pAttack );
}

