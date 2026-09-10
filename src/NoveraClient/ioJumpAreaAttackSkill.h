#pragma once
#include "ioattackskill.h"
class ioJumpAreaAttackSkill :public ioAttackSkill
{
private:
	enum SkillState
	{
		SS_NONE,
		SS_PREACTION,
		SS_PREATTACACTION,
		SS_LOOP,
		SS_ACTION,
		SS_ENDACTION,
		SS_END,
	};

	enum Sync
	{
		S_DIRKEY,
		S_ATTACKKEY,
	};

protected:
	//현재 스킬 상태
	SkillState m_SkillState;

	//점프하는 스킬
	float m_fJumpAngle;
	float m_fJumpPower;

	//pre action 추가
	ioHashString m_szPreActionName;
	ioHashString m_szPreActionEffect;
	DWORD m_dwPreActionEndTime;
	float m_fPreActionRate;
		
	//pre attack 추가
	DWORD		 m_dwPreAttackStartTime;
	DWORD		 m_dwPreAttackEndTime;
	DWORD		 m_dwPreAttackProtectTime;
	DWORD		 m_dwPreAttackKeyReserveTime;
	bool		 m_bKeyInput;
	
	//loop 추가.
	ioHashString m_szSkillLoopMotion;
	ioHashString m_szSkillLoopEffect;
	DWORD m_dwLoopStartTime;
	//loop시간. (키 입력을 지속할 시 weapon 갯수가 0이 아니라면 m_dwLoopDuration시간 만큼 loop를 돈 후 스킬 종료.
	DWORD m_dwLoopDuration;
	float m_fSkillLoopMotionRate;
	DWORD m_dwSkillLoopProtectTime;

	//Attack Motion 추가
	ioHashString m_szAttackMotionName;
	float		 m_fAttackMotionRate;

	//Weapon 관련.
	D3DXVECTOR3 m_vWeaponOffset;
	bool m_bFirstWeaponCall;	//ini 세팅값

	//KeyInput관련
	DWORD m_dwKeyInputLimitTime;
	DWORD m_dwCreatedWeaponTime;
	DWORD m_dwCreateWeaponTicTime;
	//현재 케릭터 방향
	ioUserKeyInput::DirKeyInput m_CurDirKey;
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();
public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsNoDropState() const;
	virtual bool IsEndState() const;
	virtual bool IsAttackEndState() const;
	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );
	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
protected:
	void WeaponFire( ioBaseChar *pChar, ioPlayStage *pStage );
protected:
	//캐릭터를 jump시킨다.
	void SetCharJumping( ioBaseChar *pChar );
	// loop가 끝나는 상태를 체크한다.
	bool CheckLoopEnd( ioBaseChar *pChar );
	//PreAction상태로 변경
	void SetPreActionState( ioBaseChar *pChar );
	//PreAttackAction 상태로 변경
	void SetPreAttackAction( ioBaseChar *pChar );
	//Loop 상태로 변경
	void SetLoopState( ioBaseChar *pChar );
	//ActionEnd상태로 변경 (만약 끝나는 Action이 있다면) 없다면 바로 SetEndState 호출
	void SetActionEndState( ioBaseChar *pChar );
	//스킬 종료 상태
	void SetEndState( ioBaseChar *pChar );
	//Weapon 생성
	void CreateWeapon( ioBaseChar *pChar, ioPlayStage *pStage );
	void CreateWeaponByAni( ioBaseChar *pChar, int iAniID, float fTimeRate, DWORD dwPreDelay );
	void CheckKeyInput( ioBaseChar *pChar, ioPlayStage *pStage );
	void CheckDirKeyInput( ioBaseChar *pChar );
	void SetFirstWeaponCall( ioBaseChar *pChar, int iAniID, float fTimeRate, DWORD dwPreDelay );
	void ClearData();
public:
	ioJumpAreaAttackSkill(void);
	ioJumpAreaAttackSkill( const ioJumpAreaAttackSkill &rhs );
	virtual ~ioJumpAreaAttackSkill(void);
};

inline ioJumpAreaAttackSkill* ToJumpAreaAttackSkill( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_JUMP_AREA_ATTACK )
		return NULL;

	return dynamic_cast< ioJumpAreaAttackSkill* >(pAttack);
}