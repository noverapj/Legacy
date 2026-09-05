#pragma once

#include "ioAttackSkill.h"
#include "ioUserKeyInput.h"

class ioINILoader;
class ioBaseChar;
class ioItem;
class SP2Packet;

class ioRepeatMoveAttack2 : public ioAttackSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_PRE_ATTACK,
		SS_MOVE,
		SS_FIND,
		SS_ATTACK,
		SS_ROTATE_MOVE,
		SS_END,
	};

protected:
	DWORD m_dwSkillProtectTime;

	AttributeList m_vMoveInfo;
	AttackAttribute m_AttackInfo;
	AttackAttribute m_PreAttackInfo;

	int m_nMaxRepeatCnt;
	bool m_bNoDropState;

	ioHashString m_szAimedMarker;
	float m_fAimRange;

	ioHashString m_szCheckBuff;
	D3DXVECTOR3 m_vBaseAimPos;

	ioHashStringVec m_RemoveBuff;

	bool m_bSetUseCount;

protected:
	SkillState m_SkillState;
	DWORD m_dwCurSkillProtectTime;

	DWORD m_dwAttackReserveTime;

	int m_nCurRepeatCnt;
	bool m_bUseActiveCnt;

	ioHashString m_szAimedTarget;
	BaseCharDistanceSqList m_vTargetInfoList;

	DWORD m_dwMakerID;

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
	bool CheckKeyInput( ioBaseChar *pChar, bool bSetMove );

protected:
	void SetPreAttackState( ioBaseChar *pChar );
	void SetMoveState( ioBaseChar *pChar, int nDir );
	void SetAttackState( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetRotateMoveState( ioBaseChar *pChar );
	void SetEndState( ioBaseChar *pChar );

public:
	bool IsNoFallDropZone() const;
	virtual bool IsUseActiveCnt() const;
	virtual int GetMaxActiveCnt( ioBaseChar *pChar ) const;
	virtual int GetCurActiveCnt( ioBaseChar *pChar ) const;

protected:
	bool GetTargetList( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetTargetRotation( ioBaseChar *pChar );

protected:
	DWORD CreateEffect( ioBaseChar *pChar, ioPlayStage *pStage, ioHashString& szEffect, D3DXVECTOR3& vPos, D3DXVECTOR3& vScale );
	void DestroyEffect( ioBaseChar *pChar, ioPlayStage *pStage, DWORD& dwEffectID );
	void UpdateMaker( ioBaseChar *pChar, ioPlayStage *pStage );

protected:
	void LoadRemoveBuffList( ioINILoader &rkLoader );

public:
	ioRepeatMoveAttack2();
	ioRepeatMoveAttack2( const ioRepeatMoveAttack2 &rhs );
	virtual ~ioRepeatMoveAttack2();
};

inline ioRepeatMoveAttack2* ToRepeatMoveAttack2( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_REPEAT_MOVE_ATTACK2 )
		return NULL;

	return dynamic_cast< ioRepeatMoveAttack2* >( pAttack );
}

