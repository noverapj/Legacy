#pragma once

#include "ioAttackSkill.h"
#include "ioAttackAttribute.h"
#include "ioUserKeyInput.h"

class ioINILoader;
class ioBaseChar;
class ioItem;
class SP2Packet;

class ioChargingFireSkill8 : public ioAttackSkill
{
	struct GatheringAniInfo
	{
		ioHashString m_stAni;
		float m_fAniRate;
	};

public:
	//스킬의 상태
	enum SkillState
	{		
		SS_NONE,
		SS_CHARGING,
		SS_CHANGEROT,
		SS_CHARGESHOT,
		SS_CHARGE_GRADE_CHANGE,
		SS_END,
		SS_JUMPING,
		SS_LAND_ATTACK,
	};

	//추가된 변수////////////////////
protected:	
	//스킬 보호 시간
	DWORD m_dwSkillProtectTime;
	//현재 스킬 보호 시간
	DWORD m_dwCurSkillProtectTime;
	//스킬 중인 상태
	CEncrypt<bool> m_bSetAttackFire;
	//현재 방향
	ioUserKeyInput::DirKeyInput m_CurDirKey;		
	////////////////////////////////

	typedef std::vector< GatheringAniInfo > ChargeAniInfoList;
	ChargeAniInfoList m_vChargeAniInfoList;
	//추가한 변수///////////////////
protected:
	//충전 스킬 리스트 (단계별 속성)
	AttributeList m_vChargeShotList;

	//충전 시작 시간, 최대 충전 시간, 충전 단계의 차이값
	DWORD m_dwStartChargeTime;
	DWORD m_dwMaxChargeTime;
	DWORD m_dwChargeTic;

	//충전 중 모션, 빠르기
	ioHashString m_ChargeMotionName;
	float m_fChargeAnimationTimeRate;

	//충전 초기화 상태, 충전 단계 
	bool m_bIsChargeStateInit;
	int m_nChargeGrade;
	//충전 발사 초기화 상태
	bool m_bIsChargeShotInit;
	//현재 입력 방향
	ioUserKeyInput::DirKeyInput m_InputDirKey;	
	//첫공격 발동여부
	bool m_bSetFirstAttack;
	//회전 가능
	bool m_bSetRotation;
	///////////////////////////////////

	float m_fJumpPower;

	AttackAttribute m_LandAttackAttribute;
	

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
	void ChangeGradeMotion( ioBaseChar *pChar, int nGrade );

	//추가한 함수///////////////////////
protected:
	//충전 데이터 읽기
	void LoadChargeProperty( ioINILoader &rkLoader );
	//충전 반복 애니메이션 설정
	void SetChargeLoopAnimation( ioBaseChar *pChar );
	//충전 상태일때 캐릭터 회전
	void SetCharacterRot( ioBaseChar *pChar );
	//충전 단계 확인
	void CheckChargeShotState( ioBaseChar *pChar );
	//충전 발사 
	void ActionChargeShot( ioBaseChar *pChar );
	///////////////////////////////////

	bool CheckLanding( ioBaseChar *pChar );
	void SetLandAttack( ioBaseChar *pChar );
	void SetJumpAction( ioBaseChar *pChar );

public:
	//기본 생성자
	ioChargingFireSkill8();
	//복사 생성자 ( Clone 에서 쓰임)
	ioChargingFireSkill8( const ioChargingFireSkill8 &rhs );
	//소멸자
	~ioChargingFireSkill8();
};

//스킬 형변환 (기본 형태에 맞게 변경)
inline ioChargingFireSkill8* ToChargingFireSkill8( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_CHARGING_FIRE8 )
		return NULL;

	return dynamic_cast< ioChargingFireSkill8* >(pAttack);
}

