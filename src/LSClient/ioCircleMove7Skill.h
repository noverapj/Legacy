#pragma once

#include "ioNormalSkill.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioBaseChar;
class ioItem;
class SP2Packet;

class ioCircleMove7Skill : public ioNormalSkill
{
protected:
	enum SkillState
	{
		SS_None,
		SS_Action,
		SS_Loop,
		SS_Teleport,
		SS_Attack,
		SS_End,
	};

protected:	
	ioHashString m_ReadyCircle;
	float m_fCircleRange;
	float m_fCircleMoveSpeed;
	float m_fCircleHeightGap;
	float m_fCircleOffSet;

	ioHashString m_szLoopAni;
	DWORD m_dwLoopTime;

	float m_DropZoneOffset;

	ioHashStringVec m_vCreateBuffList;
	ioHashStringVec m_vRemoveBuffList;

	DWORD m_dwSkillProtectTime;

	bool m_bIsCanDown;
	bool m_bIsCanJump;
	bool m_bIsCanJumpOnBlow;
	bool m_bIsCanWound;
	bool m_bIsCanBlowWound;

	AttackAttribute m_AttackInfo;

protected:
	SkillState m_SkillState;

	D3DXVECTOR3 m_vCirclePos;
	DWORD m_dwCheckLoopTime;
	DWORD m_dwCurSkillProtectTime;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();
	
public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );

	virtual bool IsNoDropState() const;
	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool IsEndState() const;
	virtual bool IsAttackEndState() const;

public:
	//스킬 사용 상태 얻기
	virtual bool IsCanDownSkill() const;
	virtual bool IsCanJumpingSkill() const;
	virtual bool IsCanJumpingSkillOnBlowWound() const;
	virtual bool IsCanWoundSkill() const;
	virtual bool IsCanBlowWoundSkill() const;

protected:
	void LoadBuffInfo( ioINILoader &rkLoader );

protected:
	//상태 변경
	void SetActionState( ioBaseChar *pChar );
	void SetLoopState( ioBaseChar *pChar );
	void SetTeleportState( ioBaseChar *pChar );
	void SetAttackState( ioBaseChar *pChar );
	void SetEndState( ioBaseChar *pChar );

protected:
	//마법진 생성
	void CreateMagicCircle( ioBaseChar *pChar );
	//마법진 업데이트
	void UpdateMagicCircle( ioBaseChar *pChar );
	//마법진 위치얻기
	void GetMagicCirclePos( ioBaseChar *pChar );
	//마법진 방향으로 회전
	void CheckRotate( ioBaseChar *pChar );
	//키입력 확인
	void CheckInputKey( ioBaseChar *pChar );

protected:
	//버프 생성
	void CreateBuff( ioBaseChar *pChar );
	//버프 제거
	void RemoveBuff( ioBaseChar *pChar );

public:
	ioCircleMove7Skill();
	ioCircleMove7Skill( const ioCircleMove7Skill &rhs );
	virtual ~ioCircleMove7Skill();
};

inline ioCircleMove7Skill* ToCircleMove7Skill( ioSkill *pSkill )
{
	ioNormalSkill *pNormal = ToNormalSkill( pSkill );
	if( !pNormal || pNormal->GetSubSkillType() != NST_CIRCLE_MOVE7 )
		return NULL;

	return dynamic_cast< ioCircleMove7Skill* >( pSkill );
}