#pragma once
#include "ioattackskill.h"

class ioINILoader;
class ioBaseChar;
class SP2Packet;

class ioRandomGenerate2Skill :	public ioAttackSkill
{
private:
	enum SkillState
	{
		SS_NONE,
		SS_WAIT,
		SS_ACTION,
		SS_FIRE,
		SS_END,
	};

	enum WeaponStartPositionState
	{
		WSPS_AREA,
		WSPS_PLYER,
	};

protected:
	//현재 skill 상태
	int m_State;
	int m_WeaponStartPositionState;

	//magic circle 관련 변수들
	float m_fMagicCircleOffSet;
	float m_fMagicCircleMoveSpeed;
	float m_fMagicCircleHeightGap;
	float m_fMagicCircleRange;

	//웨폰 소환되는 마법진 반지름 값
	float m_fCircleRadius;

	//magic circle move가능한 시간
	DWORD m_dwCircleDuration;
	//magic circle move 종료 시간.
	DWORD m_dwCircleEnd;
	//magic circle 위치
	D3DXVECTOR3 m_vCirclePos;

	//magic circle image
	ioHashString m_ReadyCircle;
	//마법진
	ioHashString m_RedCircle;
	ioHashString m_BlueCircle;

	//무기 info list
	GenerateList m_vGenerateList;
	//무기 시작위치의 높이 추가
	float m_fGenerateStartHeight;
	//무기 생성시작시간
	DWORD m_dwGenerateStartTime;
	//다음무기 생성 시작 시간
	DWORD m_dwNextGenerateTime;

	//다음 무기 생성 tic 타임 세팅 관련 INI 변수
	DWORD m_dwMinGenerateTime;
	DWORD m_dwMaxGenerateTime;

	//WeaponStartPositionState에 따른 Pos 세팅
	D3DXVECTOR3 m_GeneratePos;

	//weapon 의 movedir
	D3DXVECTOR3 m_vWeaponMoveDir;

	UniqueObjID m_dwCurMapEffect;

	//Loop 추가.
	ioHashString m_SkillLoopMotion;
	DWORD m_dwLoopStartTime;
	DWORD m_dwLoopDuration;
	float m_fSkillLoopMotionRate;

	DWORD m_dwFireProtectEndTime;

protected:
	DWORD	m_dwSkillProtectTime;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnSkillEnd( ioBaseChar *pChar );
	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );

	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );
	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
protected:
	virtual void AttackSkillFire( ioBaseChar *pChar );
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;

protected:
	//랜덤 weapon 생성
	void GenerateWeapon( ioBaseChar *pChar );
	//루프상태로 변경
	void SetWaitState( ioBaseChar *pChar );
	//공격 상태로 변경
	void SetActionState( ioBaseChar *pChar );
	//공격 상태 진행
	void FireWeapon( ioBaseChar *pChar, ioPlayStage *pStage );
	//마법진 생성
	void CreateMagicCircle( ioBaseChar *pChar );
	//마법진 업데이트
	void UpdateMagicCircle( ioBaseChar *pChar );
	//마법진 방향으로 회전
	void CheckRotate( ioBaseChar *pChar );
	//키입력 체크
	void CheckInputKey( ioBaseChar *pChar );
	//마법진 위치얻기
	void GetMagicCirclePos( ioBaseChar *pChar );
	//Action 이펙트 생성
	void CreateEffect( ioBaseChar *pChar );
	//move dir 세팅
	void SetMoveDir();
	//Skill 종료 상태로 변경
	void SetDeadState( ioBaseChar *pChar );
public:
	void OnGenerateList( ioBaseChar *pChar, SP2Packet &rkPacket );
public:
	ioRandomGenerate2Skill(void);
	ioRandomGenerate2Skill( const ioRandomGenerate2Skill &rhs );
	virtual ~ioRandomGenerate2Skill(void);
};

inline ioRandomGenerate2Skill* ToRandomGenerate2Skill( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_RANDOM_GENERATE2 )
		return NULL;

	return dynamic_cast< ioRandomGenerate2Skill* >(pAttack);
}