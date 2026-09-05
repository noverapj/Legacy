#pragma once

#include "ioAttackSkill.h"
#include "ioUserKeyInput.h"

class ioINILoader;
class ioBaseChar;
class ioItem;
class SP2Packet;

class ioRunningAttackSkill5 : public ioAttackSkill
{
public:
	enum SkillState
	{
		SS_NONE,		
		SS_RUN,
		SS_ROTATE,
		SS_SWAPRUN,
		SS_END,
	};

protected:
	//스킬 상태
	SkillState m_SkillState;
	//버프 리스트
	BuffInfoList m_OwnerBuffList;	
	//스킬 보호 시간
	DWORD m_dwSkillProtectTime;
	//현재 스킬 보호 시간
	DWORD m_dwCurSkillProtectTime;
	//확인할 버프 이름
	ioHashString m_szCheckBuff;
	//딜레이 애니메이션
	ioHashString m_szDelayAnimation;
	ioHashString m_szDelayEffect;
	DWORD m_dwDelayEffectID;
	//루프 애니메이션
	ioHashString m_szLoopAnimation;
	ioHashString m_szLoopEffect;
	DWORD m_dwLoopEffectID;
	//달리는 중인가
	bool m_bIsRun;
	//딜레이 인가 루프인가
	bool m_bSetDelayInit;
	bool m_bSetLoopInit;
	//달리는 시간
	DWORD m_dwRunningStartTime;
	DWORD m_dwRunningEndTime;	
	DWORD m_dwRunningDuration;
	//속도
	CEncrypt<float> m_fRunSpeed;
	//회전
	bool m_bTargetRot;
	bool m_bLeftRot;
	ioUserKeyInput::DirKeyInput m_CurDirKey;
	CEncrypt<DWORD> m_dwRotateTime;

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

protected:
	virtual void AttackSkillFire( ioBaseChar *pChar );

	void CheckKeyInput( ioBaseChar *pOwner );
	void ProcessRotate( ioBaseChar *pOwner );

	void SetRunState( ioBaseChar *pChar );
	void SetRunEndState( ioBaseChar *pChar );

protected:
	//버프 데이터 읽기
	void LoadOwnerBuffList( ioINILoader &rkLoader );
	//버프 생성
	void SetOwnerBuffList( ioBaseChar *pChar, ioPlayStage* pStage );
	//버프 확인
	bool CheckBuff( ioBaseChar *pChar );
	//무기 생성
	void CreateWeapon( ioBaseChar *pChar );
	//딜레이, 루프 애니메이션 설정
	void SetLoopAnimation( ioBaseChar *pChar );
	void SetDelayAnimation( ioBaseChar *pChar );
	//이펙트 제거
	void DeleteEffect();
	//방향키 확인
	bool CheckDirKey( ioUserKeyInput::DirKeyInput CurDirKey );

public:
	ioRunningAttackSkill5();
	ioRunningAttackSkill5( const ioRunningAttackSkill5 &rhs );
	virtual ~ioRunningAttackSkill5();
};

inline ioRunningAttackSkill5* ToRunningAttackSkill5( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_RUNNING_ATTACK5 )
		return NULL;

	return dynamic_cast< ioRunningAttackSkill5* >( pAttack );
}

