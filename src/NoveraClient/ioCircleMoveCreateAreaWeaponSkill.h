#pragma once

#include "ioNormalSkill.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioCircleMoveCreateAreaWeaponSkill : public ioNormalSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_LOOP,
		SS_ACTION,
		SS_END,
		SS_CREATE_WEAPON,
	};
	
protected:
	DWORD m_dwSkillProtectTime;

	float m_fGaugePerTic;
	DWORD m_dwTicTime;

	float m_fCircleRange;
	float m_fCircleMoveSpeed;
	float m_fCircleHeightGap;
	float m_fCircleOffSet;
	ioHashString m_ReadyCircle;
	
	ioHashString m_szLoopAni;

	int m_nAreaWeaponMaxCnt;
	ioHashStringVec m_szAreaWeaponName;
	ioHashStringVec m_OwnerBuffList;

protected:
	SkillState m_SkillState;
	DWORD m_dwCurSkillProtectTime;
	DWORD m_dwCheckGaugeTime;

	D3DXVECTOR3 m_vCirclePos;
	DWORD m_dwFireStartTime;
	int m_nAreaWeaponCurCnt;

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
	void LoadAreaWeaponList( ioINILoader &rkLoader );
	void Init();

	//성장 범위 얻기
	float GetGrowthCircleRange( ioBaseChar *pChar );
	//마법진 생성
	void CreateMagicCircle( ioBaseChar *pChar );
	//마법진 업데이트
	void UpdateMagicCircle( ioBaseChar *pChar );
	//마법진 위치얻기
	void GetMagicCirclePos( ioBaseChar *pChar );

protected:
	//루프 상태로 전환
	void SetLoopState( ioBaseChar *pChar );
	//공격 상태로 전환
	void SetActionState( ioBaseChar *pChar );
	//종료 상태 설정
	void SetEndState( ioBaseChar *pChar );

protected:
	//키입력 확인
	void CheckInputKey( ioBaseChar *pChar );
	//마법진 방향으로 회전
	void CheckRotate( ioBaseChar *pChar );

protected:	
	void CreateWeapon( ioBaseChar *pChar );

protected:
	void LoadOwnerBuffList( ioINILoader &rkLoader );
	void SetOwnerBuffList( ioBaseChar *pOwner );

public:
	ioCircleMoveCreateAreaWeaponSkill();
	ioCircleMoveCreateAreaWeaponSkill( const ioCircleMoveCreateAreaWeaponSkill &rhs );
	virtual ~ioCircleMoveCreateAreaWeaponSkill();
};

inline ioCircleMoveCreateAreaWeaponSkill* ToCircleMoveCreateAreaWeaponSkill( ioSkill *pSkill )
{
	ioNormalSkill *pNormal = ToNormalSkill( pSkill );
	if( !pNormal || pNormal->GetSubSkillType() != NST_CIRCLE_MOVE_AREA_WEAPON )
		return NULL;

	return static_cast< ioCircleMoveCreateAreaWeaponSkill* >( pSkill );
}

