#pragma once

#include "ioAttackSkill.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioBaseChar;
class SP2Packet;

class ioProtectCountAreaAttackSkill : public ioAttackSkill
{
public:
	//스킬의 상태
	enum SkillState
	{		
		SS_NONE,
		SS_PROTECT,
		SS_AREA_ATTACK,
		SS_END
	};
	
protected:	
	//스킬 보호 시간
	DWORD m_dwSkillProtectTime;
	//현재 스킬 보호 시간
	DWORD m_dwCurSkillProtectTime;
	
protected:
	//방어 시간
	DWORD m_dwProtectTime;
	//방어 시간 확인
	DWORD m_dwCheckProtectTime;
	//바라보기 버프 이름
	ioHashString m_szLookBuffName;
	ioHashString m_szAttackerName;
	//범위 무기 이름
	ioHashString m_szAreaWeaponName;
	//공격 애니메이션 이름
	ioHashString m_szAreaAttackAnimation;
	//공격 애니메이션 배율
	float m_fAreaAttackAnimationRate;
	//공격 이펙트 이름
	ioHashString m_szAreaAttackEffect;
	//공격 딜레이 시간
	DWORD m_dwAreaAttackDelayTime;
	//범위 무기 생성 시간
	DWORD m_dwAreaFireTime;
	//애니메이션이 초기화 되었나
	bool m_bIsAreaAttackInit;
	//한번 생성을 하였나
	bool m_bIsAreaCreateWeapon;
	//무기 생성을 시간에 따라 할 것인가
	bool m_bSetAreaFireDelayTime;
	//무기 생성 딜레이 시간
	DWORD m_dwAreaFireDelayTime;
	//공격 애니 무적 설정
	bool m_bSetNoWound;
	//무적 설정 시간
	DWORD m_dwNoWoundTime;
	DWORD m_dwCheckNoWoundTime;
	BuffInfoList m_OwnerBuffList;

protected:
	//현재 스킬의 상태
	SkillState m_SkillState;

public:
	//ini 파일 읽기
	virtual void LoadProperty( ioINILoader &rkLoader );
	//스킬 복사 생성 후 반환
	virtual ioSkill* Clone();

public:
	//스킬 시작 부분 설정
	virtual bool OnSkillStart( ioBaseChar *pChar );
	//스킬 발동 중 설정 (Loop 상태)
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	//스킬 종료 부분 설정
	virtual void OnSkillEnd( ioBaseChar *pChar );

	//메쉬 변경 설정
	virtual bool CheckUseItemMesh();

	//무기 속성 적용
	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
	//스킬 정보 추가 적용 (패킷을 받아 캐릭터의 상태를 처리, 이동, 회전, 특수 기능 동기화)
	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );

	//스킬 종료 모션이 가능한가
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	//스킬 사용시 무적 상태에 대한 처리
	virtual bool IsProtected( int iDefenseBreakType ) const;
	//예약 가능한가
	virtual bool IsEnableReserve() const;
	//스킬이 점프 상태인가
	virtual bool IsJumpState() const;
	//스킬이 떠있는 상태인가
	virtual bool IsNoDropState() const;
	//패킷 (위치를 갱신) 을 보낼 수 있는 상태인가
	virtual bool IsCanSendControl() const;
	//대쉬 확인이 가능한가
	virtual bool IsEnableDashCheck( ioBaseChar *pChar );
	//스킬이 종료 상태인가
	virtual bool IsEndState() const;
	//공격 완료 상태 (점프 스킬 사용 후 상태 변경) 
	virtual bool IsAttackEndState() const;

protected:
	virtual void AttackSkillFire( ioBaseChar *pChar );

	//추가한 함수///////////////////////
protected:
	//데이터 읽기
	void LoadProtectCountProperty( ioINILoader &rkLoader );
	void LoadBuffInfoProperty( ioINILoader &rkLoader );
	///////////////////////////////////

protected:
	//공격 애니메이션 설정
	void SetAreaAttackAnimation( ioBaseChar *pChar );
	//범위 무기 생성 확인
	void CheckCreateAreaWeapon( ioBaseChar *pChar );
	//버프 생성
	void AddLookBuff( ioBaseChar *pChar );
	//공격 상태 동기화
	void SendAreaAttackState( ioBaseChar *pChar );

public:
	//스킬 상태가 방어 상태인가
	bool IsProtectState() const;
	//상태 변경
	void SetAreaAttackState( ioHashString szAttacker );
	
public:
	//기본 생성자
	ioProtectCountAreaAttackSkill();
	//복사 생성자 ( Clone 에서 쓰임)
	ioProtectCountAreaAttackSkill( const ioProtectCountAreaAttackSkill &rhs );
	//소멸자
	~ioProtectCountAreaAttackSkill();
};

//스킬 형변환 (기본 형태에 맞게 변경)
inline ioProtectCountAreaAttackSkill* ToProtectCountAreaAttackSkill( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_PROTECT_COUNT_AREA_ATTACK )
		return NULL;

	return dynamic_cast< ioProtectCountAreaAttackSkill* >(pAttack);
}

