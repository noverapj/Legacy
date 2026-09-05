#pragma once

#include "ioAttackSkill.h"
#include "ioAttackAttribute.h"
#include "ioUserKeyInput.h"

class ioINILoader;
class ioBaseChar;
class ioItem;
class SP2Packet;

class ioAreaCheckBuffSkill : public ioAttackSkill
{
public:
	//스킬의 상태
	enum SkillState
	{		
		SS_NONE,
		SS_ACTION,
		SS_CHECKBUFF,
		SS_CHECKHASBUFF,
		SS_END
	};
	
	//추가된 변수////////////////////
protected:	
	//버프 액션
	AttackAttribute m_vBuffAction;
	//스킬 보호 시간
	DWORD m_dwSkillProtectTime;
	//현재 스킬 보호 시간
	DWORD m_dwCurSkillProtectTime;
	//스킬 중인 상태
	CEncrypt<bool> m_bSetAttackFire;	
	////////////////////////////////
	
	//추가한 변수///////////////////////////
protected:
	//Area무기를 생성할 것인가
	bool m_bIsCreateAreaWeapon;
	//Area무기 번호
	ioHashString m_szAreaWeaponName;
	//Area무기 발동 시간
	DWORD m_dwAreaFireTime;
	//Area무기 생성 위치
	D3DXVECTOR3 m_AreaWeaponPos;
	//버프 액션 중 이펙트
	ioHashString m_szBuffActionEffect;
	//표시 버프이름 (확인할 버프는 하나!)
	ioHashString m_szCheckBuff;
	//생성 버프이름 (생성할 버프는 여러개)
	BuffInfoList m_EnemyBuffList;
	BuffInfoList m_OwnerBuffList;
	//충돌 확인 타입
	TargetColType m_TargetColType;
	//충돌 피격 타입
	TargetWoundType m_TargetWoundType;
	//충돌 범위
	float m_fColRange;
	//충돌 각도
	float m_fColAngle;
	//충돌 높이
	float m_fColUpHeight;
	//충돌 아래 높이
	float m_fColUnderHeight;
	//충돌 너비
	float m_fColWidth;
	//충돌 오프셋
	float m_fColOffSet;
	//버프 액션 초기화
	bool m_bIsBuffActionInit;
	//버프를 만들었나
	bool m_bSetEnemyBuff;
	//애니메이션 예약시간을 사용할 것인가
	bool m_bSetReserveTime;
	//키입력 가능 시간
	DWORD m_dwDelayInputTime;
	//키입력 가능 시간 확인
	DWORD m_dwCheckInputTime;
	///////////////////////////////////////

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
	//공격 스킬 발동
	virtual void AttackSkillFire( ioBaseChar *pChar );
	
	//추가한 함수////////////////////////////////////
protected:
	//추가 데이터 읽기
	void LoadBuffActionProperty( ioINILoader &rkLoader );
	void LoadTargetInfoProperty( ioINILoader &rkLoader );
	void LoadBuffInfoProperty( ioINILoader &rkLoader );
	//자신에게 버프 생성

	//Area무기 생성 확인
	void CheckCreateAreaWeapon( ioBaseChar *pChar );
	//버프 확인 스테이트
	void CheckBuff( ioBaseChar *pChar );
	//버프 확인 애니메이션 설정
	void SetCheckBuffAnimation( ioBaseChar *pChar );
	//실제 타겟에서 표시버프가 있는가 확인
	void CheckTargetHasBuff( ioBaseChar *pChar );
	/////////////////////////////////////////////////

public:
	//기본 생성자
	ioAreaCheckBuffSkill();
	//복사 생성자 ( Clone 에서 쓰임)
	ioAreaCheckBuffSkill( const ioAreaCheckBuffSkill &rhs );
	//소멸자
	~ioAreaCheckBuffSkill();
};

//스킬 형변환 (기본 형태에 맞게 변경)
inline ioAreaCheckBuffSkill* ToAreaCheckBuffSkill( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_AREA_CHECK_BUFF )
		return NULL;

	return dynamic_cast< ioAreaCheckBuffSkill* >(pAttack);
}

