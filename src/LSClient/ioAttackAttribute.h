#pragma once


enum AttackType
{
	AT_NORMAL,
	AT_COUNTER_ATTACK,
	AT_DASH_ATTACK,
	AT_JUMP_ATTACK,
	AT_EXTEND_JUMP_ATTACK,
	AT_LAND_JUMP_ATTACK,
	AT_EXTEND_LAND_JUMP_ATTACK,
	AT_HIGH_JUMP_ATTACK,
	AT_EXTRA_DASH_ATTACK,
	AT_EXTRA_OTHER_DASH_ATTACK,
	MAX_ATTACK_TYPE
};

enum AutoTargetType
{
	ATT_NORMAL,
	ATT_DASH,
	ATT_COUNTER,
	ATT_JUMP,
	ATT_SKILL,
};

enum DirectlyType
{
	DT_NONE,
	DT_BOOM,
	DT_ONE_TARGET,
};

class ioINILoader;

#define NO_WEAPON_ATTRIBUTE 0

struct ForceInfo
{
	float m_fForceMovePower;
	float m_fForceMoveFriction;
	float m_fForceAngle;

	ForceInfo()
	{
		m_fForceMovePower = 0.0f;
		m_fForceMoveFriction = FLOAT1;
		m_fForceAngle = 0.0f;
	}
};

typedef std::vector< ForceInfo > ForceInfoList;

struct WeaponInfo
{
	int m_iWeaponIdx;
	int m_iResistanceIdx;
	ioHashString m_Animation;
	ioHashString m_WoundedAnimation;
	DWORD m_dwWoundedDuration;
	DWORD m_dwCallTime;
	bool m_bWoundedLoopAni;
	DWORD m_dwDuration;
	float m_fAniRate;

	WeaponInfo()
	{
		Init();
	}

	void Init()
	{
		m_iWeaponIdx = 0;
		m_iResistanceIdx = 0;

		m_dwWoundedDuration = 0;
		m_dwCallTime = 0;

		m_bWoundedLoopAni = false;

		m_dwDuration = 0;
		m_fAniRate = 0.f;
	}
};

typedef std::vector< WeaponInfo > WeaponInfoList;

struct WeaponFireInfo
{
	DWORD m_dwWeaponFireTime;
	float m_fWeaponFireAngle;

	WeaponInfo m_WeaponInfo;
};
typedef std::vector< WeaponFireInfo > WeaponFireInfoList;

struct AttackAttribute
{
	ioHashString m_AttackAnimation;
	float m_fAttackAniRate;
	ioHashString m_AttackEndAnimation;
	float m_fAttackEndAniRate;

	WeaponInfoList m_vWeaponInfoList;
	ForceInfoList m_vForceInfoList;

	DWORD m_dwPreDelay;
	DWORD m_dwEndDelay;

	AttackAttribute()
	{
		Init();
	}

	void Init();
};

typedef std::vector< AttackAttribute > AttributeList;

#ifndef SHIPPING //UJ
	#define LoadAttackAttribute_e		LoadAttackAttribute
#else
//UJ 130812, 스트링 상수 암호화 및 숨기기 위한 코드
//
// 주의1: 최대 63글자(64bytes)까지만 지원
// 주의2: debug 빌드에서는 string 보호가 되지 않는다
// 주의3: 절대 문자열 상수 말고 char* szKey 등을 넘기지마라!! crash 가능!!
//
template <int N>
__forceinline void LoadAttackAttribute_e(
	const char (&szAttack)[N],
	AttackAttribute &rkAttribute,
	ioINILoader &rkLoader )
{
	_ENCSTR(szAttack, szEncAttack);
	LoadAttackAttribute(szEncAttack, rkAttribute, rkLoader);
}
#endif // _DEBUG

void LoadAttackAttribute( const char *szAttack,
						  AttackAttribute &rkAttribute,
						  ioINILoader &rkLoader );
void LoadAttackAttribute( const char *szAttack, AttackAttribute &rkAttribute, ioINILoader &rkLoader );
void LoadForceInfo( const char *szAttack, AttackAttribute &rkAttribute, ioINILoader &rkLoader );
void LoadWeaponInfo( const char *szAttack, AttackAttribute &rkAttribute, ioINILoader &rkLoader );
void LoadWeaponInfo( const char *szWeapon, WeaponInfo &rkWeaponInfo, ioINILoader &rkLoader );
void LoadWeaponInfoList( const char *szAttack, WeaponInfoList &WeaponInfoList, ioINILoader &rkLoader );

struct RangeAttribute
{
	float fRange;
	int iAttributeIdx;
};

typedef std::vector< RangeAttribute > RangeAttributeList;