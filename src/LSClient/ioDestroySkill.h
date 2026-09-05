#pragma once

#include "ioNormalSkill.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioDestroySkill : public ioNormalSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_LOOP,
		SS_CHARGE,
		SS_SUCCESS,
		SS_END,
	};
	
protected:	
	//무적 시간 설정
	DWORD m_dwSkillProtectTime;

	//루프 설정
	ioHashString m_szLoopAni;
	DWORD m_dwLoopDuration;

	//차지 시간
	DWORD m_dwChargeTime;

	//타겟 범위, 각도
	float m_fColRange;
	float m_fColAngle;

	//성공, 실패 이펙트
	ioHashString m_szStartEffect;
	ioHashString m_szSuccessEffect;
	ioHashString m_szFailEffect;
	ioHashString m_szFindEffect;
	//이펙트 오프셋
	D3DXVECTOR3 m_vEffectStartOffset;
	D3DXVECTOR3 m_vEffectSuccessOffset;
	D3DXVECTOR3 m_vEffectFailOffset;
	D3DXVECTOR3 m_vEffectFindOffset;
		
	ioHashStringVec m_vPassDummyList;

protected:
	//스킬 상태
	SkillState m_SkillState;
	//현재 스킬 보호 시간
	DWORD m_dwCurSkillProtectTime;
	//루프 시간 확인
	DWORD m_dwCheckLoopTime;
	//차지 시간 확인
	DWORD m_dwCheckChargeTime;
	//성공 여부
	bool m_bSuccess;

	DWORD m_dwStartEffectID;
	DWORD m_dwFindEffectID;
				
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnSkillEnd( ioBaseChar *pChar );	
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );

	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );
	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );

	virtual bool CheckUseItemMesh();
	virtual bool IsProtected( int iDefenseBreakType ) const;

	virtual bool IsEndState() const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;

protected:
	virtual void SetNormalSkill( ioBaseChar *pChar );
	
protected:
	//루프 상태로 전환
	void SetLoopState( ioBaseChar *pChar );
	//차지 상태로 전환
	void SetChargeState( ioBaseChar *pChar );
	//공격 상태로 전환
	void SetSuccessState( ioBaseChar *pChar );
	//종료 상태 설정
	void SetEndState( ioBaseChar *pChar );

protected:
	//키입력 확인
	void CheckInputKey( ioBaseChar *pChar );
	void CheckChargeKey( ioBaseChar *pChar );

protected:
	//무기 찾기
	void FindTargetWeapon( ioBaseChar *pChar );
	//마인 폭발
	void SetMineExplosion( ioBaseChar *pChar, ioPlayStage *pStage, TargetWeaponList &vWeaponList );
	//더미 찾기
	void FindTargetDummy( ioBaseChar *pChar );
	//더미 삭제
	void SetDummyDestroy( ioBaseChar *pChar, TargetDummyList &vDummyList );
	bool IsPassDummy( ioDummyChar *pDummy );

protected:
	//이펙트 생성
	void CreateActionEffect( ioBaseChar *pChar, ioHashString szEffect, D3DXVECTOR3 vOffset );
	void CreateLoopEffect( ioBaseChar *pChar, ioHashString szEffect, D3DXVECTOR3 vOffset, DWORD &dwID );
	void DestroyEffect( ioBaseChar *pChar, DWORD &dwID );
	
public:
	ioDestroySkill();
	ioDestroySkill( const ioDestroySkill &rhs );
	virtual ~ioDestroySkill();
};

inline ioDestroySkill* ToDestroySkill( ioSkill *pSkill )
{
	ioNormalSkill *pNormal = ToNormalSkill( pSkill );
	if( !pNormal || pNormal->GetSubSkillType() != NST_DESTROY )
		return NULL;

	return dynamic_cast< ioDestroySkill* >( pSkill );
}

