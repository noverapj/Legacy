#pragma once

#include "ioNormalSkill.h"
#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioCircleMove6Skill : public ioNormalSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_LOOP,
		SS_ACTION,
		SS_END,
		SS_WAIT,
		
		SS_CREATE_WEAPON,
		SS_NEXT_SKILL_STATE,

		SS_SECOND_LOOP,
		SS_SECOND_ACTION,
		SS_CREATE_SECOND_WEAPON,
	};
	
protected:
	//마법진 설정
	float m_fCircleRange;
	float m_fCircleMoveSpeed;
	float m_fCircleHeightGap;
	float m_fCircleOffSet;

	//이펙트 설정
	ioHashString m_ReadyCircle;
	ioHashString m_RedEffect;
	ioHashString m_BlueEffect;
	ioHashString m_RedGuidCircle;
	ioHashString m_BlueGuidCircle;
	//이펙트 생성 오프셋
	D3DXVECTOR3 m_vEffectOffset;
	
	//무적 시간 설정
	DWORD m_dwSkillProtectTime;
	//공격 무기 설정
	int m_nFireWeaponNum;
	int m_nFireWeaponResist;
	//루프 설정
	ioHashString m_szLoopAni;
	DWORD m_dwLoopDuration;
	
	ioHashString m_GuideEffect;
	bool m_bEnableTeamOnlyVisible;		
protected:
	//스킬 상태
	SkillState m_SkillState;
	//현재 스킬 보호 시간
	DWORD m_dwCurSkillProtectTime;
	//루프 시간 확인
	DWORD m_dwCheckLoopTime;
	//마법진 위치
	D3DXVECTOR3 m_vCirclePos;
	//공격 시간
	DWORD m_dwFireStartTime;
	//무기 생성 여부
	CEncrypt<bool> m_bCreateWeapon;
	DWORD m_dwGuideEffectID;


	ioHashString m_szSecondLoopAni;
	float		m_fSecondLoopAniRate;
	DWORD m_dwSecondLoopDuration;

	ioHashString m_szSecondActionAni;
	float		m_fSecondActionAniRate;
	ioHashString	m_szTargetName;
	//공격 무기 설정
	int m_iSecondFireWeaponNum;
	int m_iSecondFireWeaponResist;
				
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

	virtual void SetNextSkillState( ioBaseChar *pWounder, ioWeapon *pWeapon = NULL );
protected:
	virtual void SetNormalSkill( ioBaseChar *pChar );
	
protected:
	//성장 범위 얻기
	float GetGrowthCircleRange( ioBaseChar *pChar );
	//루프 상태로 전환
	void SetLoopState( ioBaseChar *pChar );
	//마법진 생성
	void CreateMagicCircle( ioBaseChar *pChar );
	//마법진 업데이트
	void UpdateMagicCircle( ioBaseChar *pChar );
	//마법진 방향으로 회전
	void CheckRotate( ioBaseChar *pChar );
	//키입력 확인
	void CheckInputKey( ioBaseChar *pChar );
	//공격 상태로 전환
	void SetActionState( ioBaseChar *pChar );
	//마법진 위치얻기
	void GetMagicCirclePos( ioBaseChar *pChar );
	//이펙트 생성
	void CreateEffect( ioBaseChar *pChar );
	//무기 생성
	void CreateWeapon( ioBaseChar *pChar );
	//종료 상태 설정
	void SetEndState( ioBaseChar *pChar );
	void CreateGuideEffect( ioBaseChar *pChar );
protected:
	void SetSecondLoopState( ioBaseChar *pChar );
	void SetSecondActionState( ioBaseChar *pChar );
	void TeleportTarget( ioBaseChar *pChar );
	void CreateSecondWeapon( ioBaseChar *pChar );
	void CreateSecondMagicCircle( ioBaseChar *pChar );

public:
	ioCircleMove6Skill();
	ioCircleMove6Skill( const ioCircleMove6Skill &rhs );
	virtual ~ioCircleMove6Skill();
};

inline ioCircleMove6Skill* ToCircleMove6Skill( ioSkill *pSkill )
{
	ioNormalSkill *pNormal = ToNormalSkill( pSkill );
	if( !pNormal || pNormal->GetSubSkillType() != NST_CIRCLE_MOVE6 )
		return NULL;

	return dynamic_cast< ioCircleMove6Skill* >( pSkill );
}

