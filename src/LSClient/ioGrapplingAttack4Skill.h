#pragma once

class ioINILoader;
class ioBaseChar;
class ioUroborusGuideArrow;

#include "ioAttackSkill.h"
#include "ioUserKeyInput.h"

class ioGrapplingAttack4Skill : public ioAttackSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_ACTION,
		SS_LOOP,
		SS_GRAPPLING,
		SS_END,
	};

	enum SkillSync
	{
		SSC_TARGET,
		SSC_GRAPPLING,
		SSC_ATTACK,
	};

	enum AttackDir
	{
		AD_NONE,
		AD_UP,
		AD_UP_RIGHT,
		AD_RIGHT,
		AD_DOWN_RIGHT,
		AD_DOWN,
		AD_DOWN_LEFT,
		AD_LEFT,
		AD_UP_LEFT,
		AD_MAX,
	};

	enum GrapplingTargetPosition
	{
		GTP_NONE,
		GTP_AIR,
		GTP_STAND,
		GTP_DOWN,
	};

	struct AttackInfo
	{
		ioHashString	m_vAttackAni;
		float			m_fAttackAniRate;
		DWORD			m_dwPreDelay;
		DWORD			m_dwEndDelay;
		WeaponInfoList	m_vWeaponInfoList;
		AttackInfo()
		{
			Init();
		}
		void Init()
		{
			m_vAttackAni.Clear();
			m_fAttackAniRate = FLOAT1;
			m_dwPreDelay = 0;
			m_dwEndDelay = 0;
			m_vWeaponInfoList.clear();
		}
	};
//	typedef std::vector<AttackInfo> AttackInfoList;

protected:
	SkillState	m_SkillState;
	AttackDir	m_eInputDir;

	GrapplingTargetPosition	m_GrapplingTargetPosition;

	WeaponInfoList	m_vWeaponToAirList;
	WeaponInfoList	m_vWeaponToDownList;

	ioHashString m_GrapplingTargetName;

	CEncrypt<DWORD>	m_dwSkillProtectTime;
	DWORD			m_dwCurSkillProtectTime;

	ioHashString	m_SkillLoopMotion;
	CEncrypt<float>	m_fSkillLoopMotionRate;
	DWORD			m_dwSkillLoopStartTime;
	CEncrypt<DWORD>	m_dwSkillLoopDuration;
	CEncrypt<DWORD>	m_dwSkillLoopProtectTime;

	// 모션 및 공격
	AttackInfo	m_vAttackUp;
	AttackInfo	m_vAttackDown;
	AttackInfo	m_vAttackLeft;
	AttackInfo	m_vAttackRight;
	AttackInfo	m_vAttackUpLeft;
	AttackInfo	m_vAttackDownLeft;
	AttackInfo	m_vAttackUpRight;	
	AttackInfo	m_vAttackDownRight;

	DWORD			m_dwGrapplingStartTime;

	// 키 입력
	CEncrypt<DWORD>	m_dwEnableKeyTime;
	ioUserKeyInput::DirKeyInput m_CurDirKey;

	// 타겟 위치
	D3DXVECTOR3	m_vTargetPosition;

	// 타겟 검색
	ioHashString m_TargetName;
	float m_fTargetRange;
	float m_fTargetMinRange;
	float m_fTargetAngle;
	
	float m_fCharHeightOffset;

	D3DXVECTOR3 m_vAttackDir;

protected:
	void ClearData();

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

protected:
	void LoadWeaponInfo( const char *szWeapon, AttackInfo &rkAttackInfo, ioINILoader &rkLoader );

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );
	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );

	virtual bool SetSkillGrapplingState( ioBaseChar *pChar, const ioHashString &szTargetName );
	virtual D3DXVECTOR3 GetGrapplingTargetPos( ioBaseChar *pChar );

	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsEnableReserve() const;

	virtual bool CheckUseItemMesh();

protected:
	virtual void AttackSkillFire( ioBaseChar *pChar );

protected:
	void SetSkillLoopMotion( ioBaseChar *pChar );
	void FindTarget( ioBaseChar *pOwner );
	void CheckTargetState( ioHashString &szTargetName );
	bool IsEnableTargetState( ioBaseChar *pTarget );

	void CheckKeyInput( ioBaseChar *pOwner );
	void GrapplingAttackFire( ioBaseChar *pChar );

public:
	void SetSkillEndState();

public:
	ioGrapplingAttack4Skill();
	ioGrapplingAttack4Skill( const ioGrapplingAttack4Skill &rhs );
	virtual ~ioGrapplingAttack4Skill();
};

inline ioGrapplingAttack4Skill* ToGrapplingAttack4( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_GRAPPLING_ATTACK4 )
		return NULL;

	return dynamic_cast< ioGrapplingAttack4Skill* >( pAttack );
}

