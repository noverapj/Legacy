#pragma once


#include "StatAttributeID.h"
#include "EncryptValue.h"
#include "ioAttackAttribute.h"

class ioSkill;

struct StatusValue
{
	CEncrypt<float>	m_fCurValue;
	CEncrypt<float>	m_fMaxValue;
	CEncrypt<float>	m_fRecovery;
	CEncrypt<DWORD>	m_dwRecoveryTick;
	CEncrypt<DWORD>	m_dwPreRecoveryTime;

	StatusValue()
	{
		m_fCurValue = 0.0f;
		m_fMaxValue = 0.0f;
		m_fRecovery = 0.0f;
		m_dwRecoveryTick = 0;
		m_dwPreRecoveryTime = 0;
	}
};

struct DamageInfo
{
	float m_fDamage;
	int m_AttackType;

	DamageInfo()
	{
		m_fDamage = 0.0f;
		m_AttackType = 0;
	}
};
typedef std::vector< DamageInfo > DamageInfoList;

struct HeadDamageInfo
{
	DWORD	dwStartTime;
	int		iDamage;
	bool	bPlus;

	HeadDamageInfo()
	{
		dwStartTime = 0;
		iDamage = 0;
		bPlus = false;
	}
};
typedef std::list< HeadDamageInfo > HeadDamageInfoList;

struct WeaponIgnoreInfo
{
	int m_iWeaponType;
	DWORD m_dwIgnoreTime;
	DWORD m_dwTypeProtectedTime;
	DWORD m_dwProtectedType;
	WeaponIgnoreInfo()
	{
		m_iWeaponType = -1;
		m_dwIgnoreTime = 0;

		m_dwTypeProtectedTime = 0;
		m_dwProtectedType = 0;
	}
};

struct ExtraAniJump
{
	DWORD m_dwTime;
	float m_fPower;

	ExtraAniJump()
	{
		m_dwTime = 0;
		m_fPower = 0.0f;
	}
};
typedef std::vector<ExtraAniJump> vExtraAniJumpList;

struct ExtraAniGravityRate
{
	DWORD m_dwTime;
	float m_fGravityRate;

	ExtraAniGravityRate()
	{
		m_dwTime = 0;
		m_fGravityRate = FLOAT1;
	}
};
typedef std::vector<ExtraAniGravityRate> vExtraAniGravityRateList;

enum AniCollisionType
{
	ACT_LEFT_HAND,
	ACT_RIGHT_HAND,
	ACT_LEFT_FOOT,
	ACT_RIGHT_FOOT,
	ACT_LEFT_WEAPON,
	ACT_RIGHT_WEAPON,
	ACT_NONE,
	ACT_SPECIAL,
	ACT_MAX_TYPE
};
AniCollisionType GetAniCollisionType( const ioHashString &rkName );

enum FireTimeType
{
	FTT_NORMAL_ATTACK,
	FTT_COUNTER_ATTACK,
	FTT_DASH_ATTACK,
	FTT_JUMP_ATTACK,
	FTT_SKILL_ATTACK,
	FTT_EXTEND_ATTACK,
	FTT_BUFF_ATTACK,
	FTT_EXTEND_DASH_ATTACK,
	FTT_RETURN_ATTACK,
	FTT_BUFF_DUMMY_ATTACK,	// 나중에 FTT_BUFF_ATTACK 것과 통합할 수 있으면 하는게 좋음.
	FTT_AREA_ATTACK,		// Areaweapon에서 충돌체 생성한 경우.
	FTT_DUMMY_CHAR_ATTACK,
	FTT_MACHINE_ATTACK,
	FTT_BUFF_ATTACK2,
};

enum ActionStopType
{
	AST_NONE,
	AST_BOTH,
	AST_ATTACKER,
	AST_TARGET,
};

enum AttackBlockType
{
	ATB_NONE,
	ATB_PROTECTION,
	ATB_SKILL,
	ATB_ICE,
	ATB_MINER,
	ATB_BUFF,
};

enum ChangeOutLineType
{
	COT_SYSTEM_PROTECTION,
	COT_BUFF_PROTECTION,
	COT_MOUSE_OVER,
	COT_TRAINING_PROTECTION,
	COT_FIXED_OUTLINE,
};

struct OutLineRecord
{
	ChangeOutLineType m_Type;
	D3DCOLORVALUE m_Color;
	float m_fThick;
};
typedef std::list< OutLineRecord > OutLineRecordList;

enum EmoticonType
{
	EMT_DEFAULT,
	EMT_GAME_EVENT,
	EMT_SKILL_BUFF,
	EMT_BAD_STATE,
	EMT_HIT_COMBO,
	EMT_CHAT_ICON,
	EMT_3D_CHAT_ICON,
	EMT_USER_RANK,
	EMT_MOVIE_EVENT,
	EMT_NAMED_ICON,
	EMT_NAMED_UNLIMITED_ICON,
	EMT_KILL_POINT,
	EMT_FISHING_GRADE,
	EMT_FISHING_LEVEL_UP,
};

enum DefenseAction
{
	DA_NONE,
	DA_WEAK,
	DA_STRONG,
	DA_SKILL,
};

struct FireTime
{
	DWORD dwStart;
	DWORD dwDuration;

	int iAttributeIdx;
	int iResistanceIdx;

	bool bLoopWoundedAni;
	ioHashString szWoundedAni;
	DWORD dwWoundedAniDuration;

	DWORD dwWeaponIdx;
	AniCollisionType eCollisionType;
	ioHashString szSpecialCollisionBox;
	FireTimeType eFireTimeType;

	ioHashString m_CreateBuffName;

	int m_iExtraValue;

	D3DXVECTOR3 m_vPos;

	FireTime()
	{
		dwStart = 0;
		dwDuration = 0;

		iAttributeIdx = 0;
		iResistanceIdx = 0;

		bLoopWoundedAni = false;
		dwWoundedAniDuration = 0;

		dwWeaponIdx = 0;
		eCollisionType = ACT_NONE;
		eFireTimeType = FTT_NORMAL_ATTACK;

		m_iExtraValue = 0;
		m_vPos = ioMath::VEC3_ZERO;
	}
};
typedef std::list< FireTime > FireTimeList;

struct AniDuration
{
	DWORD dwStartTime;
	DWORD dwFullTime;

	AniDuration()
	{
		dwStartTime = 0;
		dwFullTime  = 0;
	}

	inline DWORD EndTime() const
	{
		return dwStartTime + dwFullTime;
	}
};

struct FishingResultInfo
{
	CEncrypt<bool> m_bSuccess;
	CEncrypt<int> m_iResultItem;

	CEncrypt<int> m_iLevel;
	CEncrypt<int> m_iAddExp;

	CEncrypt<int> m_iClassType;
	CEncrypt<int> m_iAddSoldierExp;

	bool m_bAllAlarm;
	bool m_bRoomAlarm;
	bool m_bLevelUp;
	bool m_bAddSlot;

	bool m_bEventFishing;
	bool m_bSpecial;
	bool m_bPrivateSpecial;		// 낚시터 오브젝트에서 낚시할 때 나오는 아이템 획득 시

	int m_iGradeLevel;
	int m_iClassLevel;

	FishingResultInfo()
	{
		// AttachSoldierExp 함수에서 사용하므로 미리 초기화 시켜야함
		m_iClassType = 0;
		m_iAddSoldierExp = 0;

		ClearInfo();
	}

	void ClearInfo()
	{
		AttachSoldierExp();

		m_bSuccess = false;
		m_iResultItem = -1;
		m_bAllAlarm = false;
		m_bRoomAlarm= false;
		m_bLevelUp = false;
		m_bAddSlot = false;
		m_bSpecial = false;
		m_bPrivateSpecial = false;

		m_iLevel = 0;
		m_iAddExp = 0;

		m_iClassType = 0;
		m_iAddSoldierExp = 0;
		m_iGradeLevel = 0;
		m_iClassLevel = 0;
	}

	void AttachSoldierExp();
};

struct FishingPresentInfo
{
	ioHashString m_szSendID;

	bool m_bAlarm;

	short m_iPresentType;
	short m_iPresentMent;

	int m_iPresentValue1;
	int m_iPresentValue2;
	int m_iPresentValue3;
	int m_iPresentValue4;

	FishingPresentInfo()
	{
		ClearInfo();
	}

	void ClearInfo()
	{
		m_szSendID.Clear();

		m_iPresentType = 0;
		m_iPresentMent = 0;

		m_iPresentValue1 = 0;
		m_iPresentValue2 = 0;
		m_iPresentValue3 = 0;
		m_iPresentValue4 = 0;
	}
};

struct Stat
{
	CEncrypt<float>	m_fStrength;
	CEncrypt<float>	m_fDexterity;
	CEncrypt<float>	m_fIntelligence;
	CEncrypt<float>	m_fVitality;
	
	Stat()
	{
		Initialize();
	}

	void Initialize()
	{
		m_fStrength = 0.0f;
		m_fDexterity = 0.0f;
		m_fIntelligence = 0.0f;
		m_fVitality = 0.0f;
	}

	void ZeroBound()
	{
		m_fStrength     = max( m_fStrength, 0.0f );
		m_fDexterity    = max( m_fDexterity, 0.0f );
		m_fIntelligence = max( m_fIntelligence, 0.0f );
		m_fVitality     = max( m_fVitality, 0.0f );
	}

	Stat& operator += ( const Stat &rhs )
	{
		m_fStrength += rhs.m_fStrength;
		m_fDexterity += rhs.m_fDexterity;
		m_fIntelligence += rhs.m_fIntelligence;
		m_fVitality += rhs.m_fVitality;

		return *this;
	}
	
	Stat& operator -= ( const Stat &rhs )
	{
		m_fStrength -= rhs.m_fStrength;
		m_fDexterity -= rhs.m_fDexterity;
		m_fIntelligence -= rhs.m_fIntelligence;
		m_fVitality -= rhs.m_fVitality;

		return *this;
	}

	Stat& operator = ( const Stat &rhs )
	{
		m_fStrength     = rhs.m_fStrength;
		m_fDexterity    = rhs.m_fDexterity;
		m_fIntelligence = rhs.m_fIntelligence;
		m_fVitality     = rhs.m_fVitality;

		return *this;
	}
};

class ioBaseChar;
class ioGameEntity;
class ioPlayEntity;
class ioWeapon;
class ioDummyChar;

typedef std::vector< ioBaseChar* > BaseCharList;
typedef std::list< ioGameEntity* > GameEntityList;
typedef std::list< ioPlayEntity* > PlayEntityList;

typedef std::list< ioGameEntity* > MountAvailableList;
typedef std::vector< ioGameEntity* > MountedEntityList;
typedef std::vector< ioWeapon* > MountedWeaponList;

//서든 어택때 추가
typedef std::vector< ioWeapon* > TargetWeaponList;
typedef std::vector< ioDummyChar* > TargetDummyList;

//For WeakMagic
struct WeakMagicInfo
{
	ioHashString m_Name;
	int		m_iWeakMagicType;
	float	m_fWeakMagicRate;
	int     m_iProtectionType;
};
typedef std::vector< WeakMagicInfo > WeakMagicInfoList;

//For WeakBomb
struct WeakBombInfo
{
	ioHashString m_Name;
	int		m_iWeakBombType;
	float	m_fWeakBombRate;
};
typedef std::vector< WeakBombInfo > WeakBombInfoList;

//For Reserved Sliding
struct ReservedSliding
{
	DWORD		m_dwSlidingTime;
	float		m_fSlidingForce;
	float		m_fSlidingFriction;
	float		m_fSlidingAngle;

	bool		m_bMoveDir;

	bool		m_bUseCalcDir;
	D3DXVECTOR3	m_vMoveCalcDir;

	ReservedSliding()
	{
		m_dwSlidingTime		= 0;
		m_fSlidingForce		= 0.f;
		m_fSlidingFriction	= FLOAT1;
		m_fSlidingAngle		= 0.f;

		m_bMoveDir			= true;
		m_bUseCalcDir		= false;
		m_vMoveCalcDir		= ioMath::VEC3_ZERO;
	}
};
typedef std::vector< ReservedSliding > ReservedSlidingList;

//For Reflect
struct ReflectInfo
{
	// 변수 추가시 operator = 수정해 줄 것
	ioHashString m_Name;

	int m_iSlot;
	
	float m_fReflectPowerRate;
	float m_fReflectMinPower;
	float m_fReflectMaxPower;
	CEncrypt<float> m_fDecreaseDamageRate;

	AttackSide m_ReflectSideType;
	float m_fAirReflectRate;
	bool m_bAirReflect;
	bool m_bStateProtectIgnoreReflect;
	bool m_bEnableEscapeCatchAttackReflect;
	bool m_bIgnoreOtherReflect;

	ioHashString m_ReflectEffect;
	ioHashString m_ReflectAni;
	float m_fReflectAniRate;

	ReflectInfo()
	{
		Initialize();
	}

	// const struct 복사
	ReflectInfo& operator = ( const ReflectInfo &rhs )
	{
		m_Name  = rhs.m_Name;
		m_iSlot = rhs.m_iSlot;
		m_fReflectPowerRate = rhs.m_fReflectPowerRate;
		m_fReflectMinPower  = rhs.m_fReflectMinPower;
		m_fReflectMaxPower  = rhs.m_fReflectMaxPower; 
		m_fDecreaseDamageRate			  = rhs.m_fDecreaseDamageRate;
		m_ReflectSideType				  = rhs.m_ReflectSideType;
		m_fAirReflectRate				  = rhs.m_fAirReflectRate;
		m_bAirReflect					  = rhs.m_bAirReflect;
		m_bStateProtectIgnoreReflect	  = rhs.m_bStateProtectIgnoreReflect;
		m_bEnableEscapeCatchAttackReflect = rhs.m_bEnableEscapeCatchAttackReflect;
		m_bIgnoreOtherReflect			  = rhs.m_bIgnoreOtherReflect;
		m_ReflectEffect					  = rhs.m_ReflectEffect;
		m_ReflectAni					  = rhs.m_ReflectAni;
		m_fReflectAniRate				  = rhs.m_fReflectAniRate;

		return *this;
	}

	void Initialize()
	{
		m_iSlot = 0;
		m_fReflectPowerRate = 0.0f;
		m_fReflectMinPower = 0.0f;
		m_fReflectMaxPower = 0.0f;

		m_fReflectAniRate = FLOAT1;

		m_ReflectSideType = AS_NONE;

		m_fAirReflectRate = FLOAT1;

		m_bAirReflect = false;
		m_bStateProtectIgnoreReflect = false;
		m_bEnableEscapeCatchAttackReflect = false;
		m_bIgnoreOtherReflect = false;
	}

	float GetReflectPower( float fPower )
	{
		fPower *= m_fReflectPowerRate;
		fPower = max( m_fReflectMinPower, fPower );
		fPower = min( fPower, m_fReflectMaxPower );
		return fPower;
	}
};
typedef std::vector< ReflectInfo* > ReflectInfoList;

struct ioScaleInfo
{
	DWORD dwFire_s;
	DWORD dwFire_e;
	DWORD dwSlerpTime;

	D3DXVECTOR3 vScale;
	D3DXVECTOR3 vPreScale;

	AniCollisionType szSubInfo;

	ioScaleInfo()
	{
		dwFire_s = 0;
		dwFire_e = 0;
		dwSlerpTime = 0;

		vScale = D3DXVECTOR3(1.0f, FLOAT1, FLOAT1 );
		vPreScale = D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 );
	}
};

//For statattribute.
enum StatAttributeType
{
	SAT_NONE,
	SAT_STR,
	SAT_DEX,
	SAT_INT,
	SAT_VIT
};

struct StatAttribute
{
	CEncrypt<int> iAttributeType;
	CEncrypt<int> iAttributeID;
	CEncrypt<float> fChangeWidth;
	ioHashString szTitle;
	ioHashString szDescription;
};
typedef std::vector< StatAttribute* > StatAttributeList;

struct SecondAttribute
{
	int m_iAttributeIndex;
	float m_fLimiteValue;

	SecondAttribute()
	{
		m_iAttributeIndex = 0;
		m_fLimiteValue = 0.0f;
	}
};
typedef std::vector< SecondAttribute > SecondAttributeList;

enum GrowthType
{
	GT_NONE,
	GT_CHAR_ATTACK,
	GT_CHAR_DEFENSE,
	GT_CHAR_MOVE_SPEED,
	GT_CHAR_GETUP,
	GT_CHAR_DROP_DAMAGE,
	GT_CHAR_DUMMY,
	GT_SKILL_ATTACK,
	GT_SKILL_COOL_TIME,
	GT_SKILL_DEFENSE,
	GT_REFLECT_PUSH,
	GT_SPEED_BUFF_UP,
	GT_DODGE_RANGE,
	GT_SKILL_ANI_RATE,
	GT_MAGIC_CIRCLE_RANGE,
	GT_BUFF_DURATION,
	GT_BUFF_TIC_TIME,
	GT_ROPE_DURATION,
	GT_WEAPON_MOVE_RANGE,
	GT_HEAL_VALUE_UP,
	GT_DAMAGE_BUFF_UP,
	GT_BUFF_JUMP_SPEED_RATE,
	GT_WOUNDED_BUFF_UP,
	GT_FORZEN_DURATION,
	GT_CLOSE_ORDER_DURATION,
	GT_EXPLOSION_RANGE,
	GT_PUSHSTRUT_HP_UP,
	GT_POISON_UP,
	GT_SKILL_ATTACK_FORCE,
	GT_COWRUN_SPEED,
	GT_BLOW_PUSH,
	GT_REFLECT_DAMAGE,
	GT_TELEPORT_ANI_RATE,
	GT_BREAK_FALL_HEIGHT,
	GT_BREAK_FALL_LAND,
	GT_CONVERT_RATE,
	GT_SKILL_MOVE_SPEED,
	GT_SKILL_DROP_DAMAGE,
	GT_MAX,
};

struct CharGrowthLevel
{
#ifdef SHIPPING
	CEncrypt<BYTE> m_Growth[MAX_CHAR_GROWTH];
#else
	BYTE m_Growth[MAX_CHAR_GROWTH];
#endif

	CharGrowthLevel()
	{
		Init();
	}

	void Init()
	{
		for( int i=0; i < MAX_CHAR_GROWTH; ++i )
		{
			m_Growth[i] = 0;
		}
	}
};
typedef std::map< int, CharGrowthLevel > CharGrowthLevelMap;

struct ItemGrowthLevel
{
#ifdef SHIPPING
	CEncrypt<BYTE> m_Growth[MAX_ITEM_GROWTH];
#else
	BYTE m_Growth[MAX_ITEM_GROWTH];
#endif

	ItemGrowthLevel()
	{
		Init();
	}

	void Init()
	{
		for( int i=0; i < MAX_ITEM_GROWTH; ++i )
		{
			m_Growth[i] = 0;
		}
	}
};
typedef std::map< int, ItemGrowthLevel > ItemGrowthLevelMap;

struct GrowthTypeInfo
{
	GrowthType m_GrowthType;
	ioHashString m_TypeDesc;

	GrowthTypeInfo()
	{
		m_GrowthType = GT_NONE;
		m_TypeDesc.Clear();
	}
};
typedef std::vector<GrowthTypeInfo> GrowthTypeInfoVec;

enum
{
	TGS_PROCESS	= 1,
	TGS_WAIT	= 2,
};

struct TimeGrowthInfo
{
	int m_iClassType;
	int m_iTimeSlot;	// 어느 것이 강화중인지... 무, 갑, 투, 망, 공, 방, 이, 드 순서

	int m_iValue1;     // 년월일을 나타냄 20090715(2009년 7월 15일 )
	int m_iValue2;     // 시간을 나타냄 1232 (12시32분)

	int m_iState;

	TimeGrowthInfo()
	{
		m_iClassType = 0;
		m_iTimeSlot = 0;

		m_iValue1 = 0;
		m_iValue2 = 0;

		m_iState = TGS_PROCESS;
	}

	// 날짜용
	SHORT GetYear()
	{
		return m_iValue1/10000;           // [2009]0715
	}
	SHORT GetMonth()
	{
		return ( m_iValue1/100 ) % 100;   //  2009[07]15
	}
	SHORT GetDay()
	{
		return m_iValue1 % 100;           //  200907[15]
	}
	SHORT GetHour()
	{
		return m_iValue2 / 10000;           //  [21]23   ( 21시 23분 )
	}
	SHORT GetMinute()
	{
		return ( m_iValue2/100 ) % 100;           //  21[23]
	}
	SHORT GetSec()
	{
		return m_iValue2 % 100;
	}
	void SetDate( int iYear , int iMonth, int iDay, int iHour, int iMinute, int iSec )
	{
		m_iValue1 = ( iYear * 10000 ) + ( iMonth * 100 ) + iDay;
		m_iValue2 = ( iHour * 10000 ) + ( iMinute * 100 ) + iSec;
	}
};
typedef std::vector<TimeGrowthInfo> TimeGrowthInfoList;
typedef std::vector< ioHashString > StringList;
typedef std::vector< float > FloatVec;

struct AttackRate
{
	int iType;
	float fRate;
	float fSkillRate;
	ioHashString szBuff;
	AttackRate()
	{
		iType = 0;
		fRate = FLOAT1;
		fSkillRate = FLOAT1;
	}
};
typedef std::vector< AttackRate > AttackRateList;

struct PositionDistanceSq
{
	float fDistanceSq;
	int iIndex;

	bool operator<( const PositionDistanceSq &rhs ) const
	{
		return fDistanceSq < rhs.fDistanceSq;
	}
};
typedef std::vector< PositionDistanceSq > PositionDistanceSqList;

struct ChangeMaterialInfo
{
	DWORD m_dwTime;
	int m_iChangeColorIndex;
	D3DCOLORVALUE m_ChangeColor;

	ChangeMaterialInfo()
	{
		m_ChangeColor.r = 0.0f;
		m_ChangeColor.g = 0.0f;
		m_ChangeColor.b = 0.0f;
		m_ChangeColor.a = FLOAT1;
	}
};
typedef std::vector< ChangeMaterialInfo > ChangeMaterialInfoList;

class ChangeMaterialInfoSort : std::binary_function< const ChangeMaterialInfo&, const ChangeMaterialInfo&, bool >
{
public:
	bool operator()( const ChangeMaterialInfo &lhs , const ChangeMaterialInfo &rhs ) const
	{
		if( lhs.m_dwTime > rhs.m_dwTime )
		{
			return true;
		}

		return false;
	}
};

enum BuffJumpType
{
	BJT_NONE		= 0,
	BJT_MULTI		= 1,
	BJT_FLY			= 2,
	BJT_MULT_FLY	= 3,
	BJT_WALK		= 4,
};

enum BuffJumpUsedType
{
	BJUT_NONE		= 0,
	BJUT_CUR		= 1,
	BJUT_CONTINUE	= 2,
	BJUT_USED		= 3,
	BJUT_END		= 4,
	BJUT_WALK		= 5,
};

struct CharBuffJumpInfo
{
	ioHashString m_Name;

	BuffJumpType m_BuffJumpType;

	bool m_bEnableRotate;
	bool m_bContinue;

	float m_fBuffJumpPower;
	float m_fBuffJumpForce;
	float m_fBuffJumpSpeedRate;
	float m_fBuffJumpGravityRate;

	float m_fBuffJumpExtraPower;
	float m_fBuffJumpExtraForce;
	float m_fBuffJumpExtraGravityRate;
	
	float m_fBuffJumpAniRate;
	float m_fBuffJumpLandingRate;

	int m_iBuffJumpMaxCnt;

	float m_fBuffJumpHeight;
	DWORD m_dwBuffJumpEnableTime;

	ioHashStringVec m_vBuffJumpAnimationList;
	ioHashStringVec m_vBuffJumpExtraAnimationList;

	bool	m_bEnableMoveRotate;
	DWORD	m_dwMoveRotateSpeed;

	float	m_fBuffJumpMaxHeight;
	float	m_fBuffJumpEnableGauge;

	bool	m_bAniLoop;

	float	m_fBuffJumpEndJumpPower;
	float	m_fBuffAirWalkStartGauge;

	CharBuffJumpInfo()
	{
		m_BuffJumpType = BJT_NONE;
		m_bEnableRotate = false;

		m_fBuffJumpPower = 0.0f;
		m_fBuffJumpForce = 0.0f;
		m_fBuffJumpSpeedRate = 0.0f;
		m_fBuffJumpGravityRate = FLOAT1;

		m_fBuffJumpExtraPower = 0.0f;
		m_fBuffJumpExtraForce = 0.0f;
		m_fBuffJumpExtraGravityRate = FLOAT1;

		m_fBuffJumpAniRate = FLOAT1;
		m_fBuffJumpHeight = 0.0f;

		m_fBuffJumpLandingRate = 0.0f;

		m_iBuffJumpMaxCnt = 0;
		m_dwBuffJumpEnableTime = 0;

		m_vBuffJumpAnimationList.clear();
		m_vBuffJumpExtraAnimationList.clear();

		m_bEnableMoveRotate = false;
		m_dwMoveRotateSpeed = 0;

		m_fBuffJumpMaxHeight = 0.0f;
		m_fBuffJumpEnableGauge = FLOAT1;
		m_bAniLoop = false;
		m_fBuffJumpEndJumpPower = 0.0f;
	}
};
typedef std::vector<CharBuffJumpInfo> vCharBuffJumpInfoList;

class CharBuffJumpInfoSort : std::binary_function< const CharBuffJumpInfo&, const CharBuffJumpInfo&, bool >
{
public:
	bool operator()( const CharBuffJumpInfo &lhs , const CharBuffJumpInfo &rhs )
	{
		if( lhs.m_iBuffJumpMaxCnt > rhs.m_iBuffJumpMaxCnt )
		{
			return true;
		}

		return false;
	}
};

class CharBuffJumpFlyInfoSort : std::binary_function< const CharBuffJumpInfo&, const CharBuffJumpInfo&, bool >
{
public:
	bool operator()( const CharBuffJumpInfo &lhs , const CharBuffJumpInfo &rhs )
	{
		if ( lhs.m_BuffJumpType < rhs.m_BuffJumpType )
		{
			return true;
		}
		else if ( lhs.m_BuffJumpType == rhs.m_BuffJumpType )
		{
			if( lhs.m_iBuffJumpMaxCnt > rhs.m_iBuffJumpMaxCnt )
			{
				return true;
			}
		}
		return false;
	}
};

struct BuffDoubleJumpTypeInfo
{
	DWORD m_dwJumpType;			 //점프타입
	DWORD m_dwReduceGagueLevel;	 //패시브 게이지를 감소시킬 현재 버프점프의 카운트
};

struct CharFrictionBuffInfo
{
	ioHashString m_Name;
	float m_fFrictionRate;

	CharFrictionBuffInfo()
	{
		m_fFrictionRate = FLOAT1;
	}
};
typedef std::vector<CharFrictionBuffInfo> vCharFrictionBuffInfoList;

struct CharGravityBuffInfo
{
	ioHashString m_Name;
	float m_fGravityRate;

	CharGravityBuffInfo()
	{
		m_fGravityRate = FLOAT1;
	}
};
typedef std::vector<CharGravityBuffInfo> vCharGravityBuffInfoList;

struct FrontBackWoundInfo
{
	ioHashString m_BuffName;

	ioHashString m_FrontBuff;
	ioHashString m_FrontAttackerBuff;
	float m_fFrontDamageRate;

	ioHashString m_BackBuff;
	ioHashString m_BackAttackerBuff;
	float m_fBackDamageRate;

	ioHashString m_DownFrontBuff;
	ioHashString m_DownFrontAttackerBuff;
	float m_fDownFrontDamageRate;

	ioHashString m_DownBackBuff;
	ioHashString m_DownBackAttackerBuff;
	float m_fDownBackDamageRate;

	ioHashString m_AirFrontBuff;
	ioHashString m_AirFrontAttackerBuff;
	float m_fAirFrontDamageRate;

	ioHashString m_AirBackBuff;
	ioHashString m_AirBackAttackerBuff;
	float m_fAirBackDamageRate;

	FrontBackWoundInfo()
	{
		m_fFrontDamageRate = FLOAT1;
		m_fBackDamageRate = FLOAT1;

		m_fDownFrontDamageRate = FLOAT1;
		m_fDownBackDamageRate = FLOAT1;

		m_fAirFrontDamageRate = FLOAT1;
		m_fAirBackDamageRate = FLOAT1;
	}
};
typedef std::vector<FrontBackWoundInfo> vFrontBackWoundInfoList;

struct LandStunInfo
{
	ioHashString m_BuffName;
	ioHashString m_StunBuff;
};
typedef std::vector<LandStunInfo> vLandStunInfoList;

struct FloatBuffInfo
{
	bool m_bStopAction;

	float m_fFloatJumpPower;
	float m_fFloatFrictionAmt;
	float m_fFloatGravityAmt;
	float m_fFloatBlowRate;
	float m_fFloatSpeedRate;
	float m_fFloatEndJumpPower;
	
	DWORD m_dwFloatMinTime;
	DWORD m_dwFloatMaxTime;
	
	int m_iCancelType;
	int m_iMoveType;
	int m_iLandType;
	
	bool m_bEnableDamage;
	bool m_bEndJumpState;
	bool m_bEnableTeamAttack;

	FloatBuffInfo()
	{
		Init();
	}

	void Init()
	{
		m_bStopAction = false;

		m_fFloatJumpPower = 0.0f;
		m_fFloatFrictionAmt = 0.0f;
		m_fFloatGravityAmt = 0.0f;
		m_fFloatBlowRate = 0.0f;
		m_fFloatSpeedRate = 0.0f;
		m_fFloatEndJumpPower = 0.0f;

		m_iCancelType = 0;
		m_iMoveType = 0;

		m_bEnableDamage = false;
		m_bEndJumpState = false;
		m_bEnableTeamAttack = false;

		m_dwFloatMinTime = 0;
		m_dwFloatMaxTime = 0;
	}
};

struct AniRotateInfo
{
	DWORD m_dwTime;
	float m_fAngle;

	AniRotateInfo()
	{
		m_dwTime = 0;
		m_fAngle = 0.0f;
	}
};
typedef std::vector<AniRotateInfo> vAniRotateInfoList;

// Grappling State
enum GrapplingState
{
	GS_PULL,
	GS_MOVE,
	GS_PUSH,
	GS_CANCEL,
	GS_ATTACK_MOTION,
	GS_LAND_ATTACK,
	GS_END,
};

enum GrapplingSkillState
{
	GSS_PULL,
	GSS_LOOP,
	GSS_CANCEL,
	GSS_NO_CHANGE,
	GSS_MULTI,
	GSS_NO_CHANGE2,
	GSS_EXCEPT,
	GSS_COMPETE,
	GSS_NO_CHANGE3,
};

enum GrapplingPushedState
{
	GPS_GO,
	GPS_CHANGE,
	GPS_RETURN,
};

enum JumpGrapplingState
{
	JGS_MOVE,
	JGS_ATTACK,
	JGS_WOUND,
	JGS_CANCEL,
};

struct GrapplingWoundedInfo
{
	ioHashString	m_TargetGrapplingPullMotion;
	ioHashString	m_TargetGrapplingMotion;
	ioHashString	m_TargetGrapplingMoveMotion;
	ioHashString	m_TargetGrapplingCancelMotion;
	ioHashStringVec	m_TargetGrapplingBuff;

	float			m_fTargetGrapplingPullMotionRate;
	float			m_fTargetGrapplingMoveMotionRate;
	float			m_fTargetGrapplingCancelMotionRate;
	DWORD			m_dwGrapplingMaxTime;
	DWORD			m_dwGrapplingMinTime;
	DWORD			m_dwGrapplingFastValue;

	GrapplingWoundedInfo()
	{
		m_fTargetGrapplingPullMotionRate	= FLOAT1;
		m_fTargetGrapplingMoveMotionRate	= FLOAT1;
		m_fTargetGrapplingCancelMotionRate	= FLOAT1;
		m_dwGrapplingMaxTime				= 0;
		m_dwGrapplingMinTime				= 0;
		m_dwGrapplingFastValue				= 0;
	}
};

enum GrapplingColType
{
	GCT_NONE,
	GCT_ALL,
	GCT_CHAR,
	GCT_MAP,
};

struct GrapplingSkillWoundedInfo
{
	bool m_bReverseTargetRot;
	ioHashString m_GrapplingLoopMotion;//공격자
	ioHashString m_GrapplingLoopMotion2;//공격자 혹은 공격 당하는자(추가 윤태철)
	float m_fGrapplingLoopMotionRate;

	GrapplingColType m_GrapplingColType;

	ioHashStringVec m_SwingBuff;
	ioHashStringVec m_SwingWoundBuff;
	ioHashStringVec m_SwingCancelBuff;

	GrapplingSkillWoundedInfo()
	{
		Init();
	}

	void Init()
	{
		m_bReverseTargetRot = false;
		m_GrapplingLoopMotion.Clear();
		m_fGrapplingLoopMotionRate = FLOAT1;

		m_GrapplingColType = GCT_NONE;

		m_SwingBuff.clear();
		m_SwingWoundBuff.clear();
		m_SwingCancelBuff.clear();
	}
};

struct GrapplingPushedInfo
{
	DWORD m_dwGrapplingPushedDuration;
	float m_fGrapplingPushedSpeed;
	DWORD m_dwGrapplingPushedRotSpeed;

	DWORD m_dwGrapplingPushedColTime;

	ioHashString m_GrapplingPushedMotion;
	float m_fGrapplingPushedMotionRate;

	ioHashString m_GrapplingPushedLoopMotion;
	float m_fGrapplingPushedLoopMotionRate;

	ioHashString m_GrapplingPushedReturnMotion;
	float m_fGrapplingPushedReturnMotionRate;

	ioHashStringVec m_GrapplingPushedBuffList;
	ioHashStringVec m_GrapplingColBuffList;

	GrapplingPushedInfo()
	{
		Init();
	}

	void Init()
	{
		m_dwGrapplingPushedDuration = 0;
		m_fGrapplingPushedSpeed = 0.0f;
		m_dwGrapplingPushedRotSpeed = 0;
		m_dwGrapplingPushedColTime = 0;

		m_GrapplingPushedMotion.Clear();
		m_GrapplingPushedLoopMotion.Clear();
		m_GrapplingPushedReturnMotion.Clear();

		m_fGrapplingPushedMotionRate = FLOAT1;
		m_fGrapplingPushedLoopMotionRate = FLOAT1;
		m_fGrapplingPushedReturnMotionRate = FLOAT1;

		m_GrapplingPushedBuffList.clear();
		m_GrapplingColBuffList.clear();
	}
};

enum GrapplingType
{
	GPT_NONE					= 0,
	GPT_WRESTLING				= 1,
	GPT_PSYCHIC					= 2,
	GPT_WRESTLING_ALL			= 3,
	GPT_TOSS					= 4,
	GPT_ATTACK					= 5,
	GPT_NO_CHANGE				= 6,
	GPT_MULTI					= 7,
	GPT_NO_CHANGE2				= 8,
	GPT_EXCEPT					= 9,
	GPT_COMPETE					= 10,
	GPT_NO_GRAPPLING			= 11,		// 잡기타가 아니지만 혼자만 맞게할 필요성 때문
	GPT_LUCIFER_AIR				= 12,		//루시퍼 ( 공중 잡기 )
	GPT_NAKORURU				= 13,		//나코루루( 공중 잡기 )
	GPT_MICHAEL					= 14,		//미카엘 특별 이동 공격잡기
	GPT_NO_CHANGE3				= 15,		//타이탄SD
	GPT_SUPER_DIMENSION			= 16,		//초시공 전사 점프 dash 잡기
	GPT_STRIKER_AIR				= 17,		//스트라이커 점프 대쉬( 공중 대쉬 )
	GPT_JUMPER					= 18,		//점퍼
	GPT_FLY_JUMP_EXTRA_ATTACK	= 19,		//FLY_JUMP 버프 추가 공격
	GPT_NO_CHANGE4				= 20,		//진화 레슬러
	GPT_GRAPPLER_JUMP			= 21,
	GPT_GALFORD					= 22,
	GPT_DRUID					= 23,
	GPT_PSYCHIC2				= 24,
	GPT_GUNNER					= 25,
	GPT_WARP					= 26,
	GPT_CHANGE_ON_GRIFFIN		= 27,
};

struct PsycGrapplingWoundedInfo
{
	ioHashStringVec m_StartBuffList;
	ioHashStringVec m_PullBuffList;
	ioHashStringVec m_BoomBuffList;
	ioHashStringVec m_PushBuffList;

	PsycGrapplingWoundedInfo()
	{
		Init();
	}

	void Init()
	{
		m_StartBuffList.clear();
		m_PullBuffList.clear();
		m_BoomBuffList.clear();
		m_PushBuffList.clear();
	}
};

struct JumpGrapplingInfo
{
	ioHashString m_JumpGrapplingAni;
	float m_fJumpGrapplingAniRate;

	float m_fJumpGrapplingMoveSpeed;
	DWORD m_dwJumpGrapplingRotateTime;

	AttackAttribute m_JumpGrapplingAttack;

	JumpGrapplingInfo()
	{
		Init();
	}

	void Init()
	{
		m_fJumpGrapplingAniRate = FLOAT1;
		m_fJumpGrapplingMoveSpeed = 0.0f;
		m_dwJumpGrapplingRotateTime = 0;

		m_JumpGrapplingAttack.Init();
	}
};

struct JumpGrappingWoundInfo
{
	bool m_bReverseTargetRot;

	ioHashString m_WoundedAni;
	float m_fWoundedAniRate;

	float m_fOffSet;
	float m_fHeightRate;
	DWORD m_dwJumpGrapplingRotateTime;

	ioHashStringVec m_CancelBuffList;

	GrapplingColType m_GrapplingColType;

	JumpGrappingWoundInfo()
	{
		Init();
	}

	void Init()
	{
		m_bReverseTargetRot = false;

		m_GrapplingColType = GCT_NONE;

		m_fWoundedAniRate = FLOAT1;
		m_CancelBuffList.clear();

		m_fOffSet = 0.0f;
		m_fHeightRate = FLOAT05;
		m_dwJumpGrapplingRotateTime = 0;
	}
};

enum BlowDashState
{
	BDS_BLOWING,
	BDS_DOWN,
	BDS_GETUP,
	BDS_GETUP_ATTACK,
};

typedef struct tagBlowDashInfo
{
	ioHashString m_BlowDashDownAni;
	DWORD m_dwBlowDashDownDuration;

	ioHashString m_BlowDashGetUpAni;
	float m_fBlowDashGetUpAniRate;

	AttackAttribute m_BlowDashGetUpAttack;

	tagBlowDashInfo()
	{
		Init();
	}

	void Init()
	{
		m_dwBlowDashDownDuration = 0;
		m_fBlowDashGetUpAniRate = FLOAT1;
	}
} BlowDashInfo;

enum BreakFallType
{
	BFT_NORMAL,
	BFT_JUMP,
	BFT_ATTACK,
};

typedef struct tagBreakFallInfo
{
	BreakFallType m_BreakFallType;
	AttackAttribute m_BreakFallAttack;

	tagBreakFallInfo()
	{
		Init();
	}

	void Init()
	{
		m_BreakFallType = BFT_NORMAL;
		m_BreakFallAttack.Init();
	}
} BreakFallInfo;

typedef struct tagDummyCharBaseInfo
{
	ioHashString m_DummyCharPath;
	ioHashString m_DummyCharTitle;

	ioHashString m_DummyCharName;
	ioHashString m_DummyCharModel;
	ioHashString m_DummyCharModelW;

	int m_iDummyCharType;
	D3DXVECTOR3 m_vScale;

	tagDummyCharBaseInfo()
	{
		Init();
	}

	void Init()
	{
		m_iDummyCharType = 0;
		m_vScale = ioMath::VEC3_ZERO;
	}
} DummyCharBaseInfo;
typedef std::vector<DummyCharBaseInfo> vDummyCharBaseInfoList;

typedef struct tagDummyCharLoadInfo
{
	ioHashString m_DummyCharName;
	float m_fDummyCharOffset;
	float m_fDummyCharSideOffset;
	float m_fDummyCharStartAngle;

	tagDummyCharLoadInfo()
	{
		Init();
	}

	void Init()
	{
		m_fDummyCharOffset = 0.0f;
		m_fDummyCharSideOffset = 0.0f;
		m_fDummyCharStartAngle = 0.0f;
	}
} DummyCharLoadInfo;
typedef std::vector<DummyCharLoadInfo> vDummyCharLoadInfoList;

typedef struct tagDummyCharCreateInfo
{
	ioHashString m_DummyCharName;
	int m_iDummyCharIdx;
	float m_fDummyCharStartAngle;
	D3DXVECTOR3 m_vTargetPos;

	tagDummyCharCreateInfo()
	{
		Init();
	}

	void Init()
	{
		m_iDummyCharIdx = 0;
		m_fDummyCharStartAngle = 0.0f;

		m_vTargetPos = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	}

} DummyCharCreateInfo;
typedef std::vector<DummyCharCreateInfo> vDummyCharCreateInfoList;

typedef struct tagReserveBuffInfo
{
	ioHashString m_BuffName;
	ioHashString m_CreatorName;
	ioHashString m_CreateItem;
	ioSkill* m_pSkill;
	bool m_bTeamAttack;

	tagReserveBuffInfo()
	{
		Init();
	}

	void Init()
	{
		m_pSkill = NULL;
	}

} ReserveBuffInfo;
typedef std::vector<ReserveBuffInfo> vReserveBuffInfoList;

typedef struct tagWitchFlyInfo
{
	ioHashString m_WaitAni;
	DWORD m_dwWaitDuration;

	ioHashString m_StartAni;
	float m_fStartAniRate;

	float m_fStartAngle;
	float m_fStartMaxRange;
	DWORD m_dwStartDuration;

	ioHashString m_DelayAni;
	float m_fDelayAniRate;

	ioHashString m_MoveAni;
	float m_fMoveAniRate;

	float m_fMoveSpeed;
	float m_fUpDownSpeed;

	DWORD m_dwRotateTime;

	ioHashString m_WitchFlyEffect;

	DWORD m_dwChargeTime;
	ioHashString m_ChargeAni;

	float m_fWitchFlyJumpAmt;

	AttackAttribute m_WitchAttackAttribute;
	AttributeList m_WitchComboList;

	tagWitchFlyInfo()
	{
		Init();
	}

	void Init()
	{
		m_WaitAni.Clear();
		m_dwWaitDuration = 0;

		m_DelayAni.Clear();
		m_fDelayAniRate = FLOAT1;

		m_MoveAni.Clear();
		m_fMoveAniRate = FLOAT1;

		m_fStartAngle = 0.0f;
		m_fStartMaxRange = 0.0f;
		m_dwStartDuration = 0;

		m_fMoveSpeed = 0.0f;
		m_fUpDownSpeed = 0.0f;

		m_dwRotateTime = 0;
		m_dwChargeTime = 0;

		m_fWitchFlyJumpAmt = 0.0f;

		m_WitchComboList.clear();
	}
} WitchFlyInfo;

typedef struct tagPanicMoveInfo
{
	FloatVec m_AngleList;
	CEncrypt<DWORD> m_dwBaseSeed;
	ioHashString m_MoveAni;
	ioHashString m_CatchMoveAni;
	CEncrypt<float> m_fMoveSpeed;
	CEncrypt<DWORD> m_dwMoveMinTime;
	CEncrypt<DWORD> m_dwMoveMaxTime;
	CEncrypt<DWORD> m_dwCurMoveTime;
	CEncrypt<int> m_iCurCount;

	tagPanicMoveInfo()
	{
		Init();
	}

	void Init()
	{
		m_AngleList.clear();
		m_dwBaseSeed = 0;
		m_fMoveSpeed = 0.0f;
		m_dwMoveMinTime = 0;
		m_dwMoveMaxTime = 0;
		m_dwCurMoveTime = 0;
		m_iCurCount = 0;
	}

} PanicMoveInfo;

typedef struct tagGenerateInfo
{
	D3DXVECTOR3 vPos;
	DWORD	dwGapTime;
	int iAttributeIndex;
} GenerateInfo;
typedef std::vector< GenerateInfo > GenerateList;

enum ExtraGaugeType
{
	EGT_NONE,
	EGT_ATTACK,
	EGT_ATTACK_DEF,
	EGT_WOUNDED,
	EGT_WOUNDED_DEF,
};

enum MechanicsFlyType
{
	MFT_NORMAL,
	MFT_AUTO,
};

enum LuciferFlyType
{
	LFT_NORMAL,
	LFT_AUTO,
};

typedef struct tagMechanicsFlyInfo
{
	MechanicsFlyType m_MechanicsType;

	ioHashString m_StartAni;
	float m_fStartAniRate;

	float m_fStartAngle;
	float m_fStartMaxRange;
	DWORD m_dwStartDuration;

	ioHashString m_DelayAni;
	float m_fDelayAniRate;

	ioHashString m_MoveAniFr;
	float m_fMoveAniFrRate;

	ioHashString m_MoveAniBk;
	float m_fMoveAniBkRate;

	ioHashString m_MoveAniRt;
	float m_fMoveAniRtRate;

	ioHashString m_MoveAniLt;
	float m_fMoveAniLtRate;

	ioHashString m_MoveAniUp;
	float m_fMoveAniUpRate;
	ioHashString m_MoveAniDown;
	float m_fMoveAniDownRate;

	float m_fMoveForceAmt;
	float m_fMoveForceFric;
	float m_fUpDownSpeed;


	TargetWoundType m_TargetWoundType;
	float m_fTargetAngle;
	float m_fTargetRange;
	float m_fTargetUpHeight;
	float m_fTargetDownHeight;
	float m_fTargetMarkerRange;
	
	float m_fAttackMoveSpeed;
	float m_fAttackMoveRange;
	float m_fAttackMoveRate;

	ioHashString m_AttackMoveAni;
	float m_fAttackMoveAniRate;

	AttackAttribute m_MechanicsAttack;

	ioHashString m_MechanicsFlyEffect;
	ioHashString m_MechanicsFlyMoveEffect;
	float m_fMechanicsFlyJumpAmt;

	tagMechanicsFlyInfo()
	{
		Init();
	}

	void Init()
	{
		m_MechanicsType = MFT_NORMAL;

		m_fStartAniRate = FLOAT1;

		m_fStartAngle = 0.0f;
		m_fStartMaxRange = 0.0f;
		m_dwStartDuration = 0;

		m_fDelayAniRate = FLOAT1;

		m_fMoveAniFrRate = FLOAT1;
		m_fMoveAniBkRate = FLOAT1;
		m_fMoveAniRtRate = FLOAT1;
		m_fMoveAniLtRate = FLOAT1;

		m_fMoveAniUpRate = FLOAT1;
		m_fMoveAniDownRate = FLOAT1;

		m_fMoveForceAmt = 0.0f;
		m_fMoveForceFric = FLOAT1;
		m_fUpDownSpeed = 0.0f;

		m_TargetWoundType = TWT_DOWN_WOUND_EXCEPT;
		m_fTargetAngle = 30.0f;
		m_fTargetRange = 30.0f;
		m_fTargetUpHeight = 30.0f;
		m_fTargetDownHeight = 30.0f;

		m_fAttackMoveSpeed = 0.0f;
		m_fAttackMoveRange = 0.0f;
		m_fAttackMoveRate = FLOAT1;

		m_fAttackMoveAniRate = FLOAT1;
		
		m_fMechanicsFlyJumpAmt = 0.0f;
	}
} MechanicsFlyInfo;

typedef struct tagPhantomFlyInfo
{
	// mesh list
	ioHashStringVec m_PhantomMeshList;
	ioHashStringVec m_PhantomFemaleMeshList;

	// wait
	ioHashString m_WaitAni;
	DWORD m_dwWaitDuration;

	// start
	ioHashString m_StartAni;
	float m_fStartAniRate;
	float m_fAlphaRate;

	float m_fStartAngle;
	float m_fStartMaxRange;
	DWORD m_dwStartDuration;

	// delay
	ioHashString m_DelayAni;
	float m_fDelayAniRate;

	// move
	ioHashString m_MoveAni;
	float m_fMoveAniRate;

	float m_fMoveSpeed;
	float m_fUpDownSpeed;

	DWORD m_dwRotateTime;

	// targe
	TargetWoundType m_TargetWoundType;
	float m_fTargetAngle;
	float m_fTargetRange;
	float m_fTargetUpHeight;
	float m_fTargetDownHeight;
	float m_fTargetMarkerRange;

	// trace
	float m_fTraceMoveSpeed;
	float m_fTraceMoveRange;
	float m_fTraceMoveGapRange;
	DWORD m_dwTraceRotateTime;

	ioHashString m_TraceMoveAni;
	float m_fTraceMoveRate;

	// attack
	WeaponInfo m_WeaponInfo;

	// possession
	ioHashString m_PossessionAni;
	float m_fPossessionRate;

	D3DXVECTOR3 m_vPossessionOffset;
	ioHashStringVec m_vPossessionBuff;
	ioHashStringVec m_vPossessionEndBuff;

	// end
	float m_fPhantomFlyJumpAmt;

	// etc
	ioHashString m_PhantomFlyEffect;
	DWORD m_dwPhantomFlyEnableTime;

	tagPhantomFlyInfo()
	{
		Init();
	}

	void Init()
	{
		// mesh list
		m_PhantomMeshList.clear();
		m_PhantomFemaleMeshList.clear();

		// wait
		m_WaitAni.Clear();
		m_dwWaitDuration = 0;

		// start
		m_StartAni.Clear();
		m_fStartAniRate = FLOAT1;

		m_fStartAngle = 0.0f;
		m_fStartMaxRange = 0.0f;
		m_dwStartDuration = 0;

		m_fAlphaRate = FLOAT1;

		// delay
		m_DelayAni.Clear();
		m_fDelayAniRate = FLOAT1;

		// move
		m_MoveAni.Clear();
		m_fMoveAniRate = FLOAT1;

		m_fMoveSpeed = 0.0f;
		m_fUpDownSpeed = 0.0f;

		m_dwRotateTime = 0;

		// target
		m_TargetWoundType = TWT_DOWN_WOUND_EXCEPT;
		m_fTargetAngle = 30.0f;
		m_fTargetRange = 30.0f;
		m_fTargetUpHeight = 30.0f;
		m_fTargetDownHeight = 30.0f;

		// trace
		m_fTraceMoveSpeed = 0.0f;
		m_fTraceMoveRange = 0.0f;
		m_fTraceMoveGapRange = 0.0f;
		m_dwTraceRotateTime = 0;

		m_TraceMoveAni.Clear();
		m_fTraceMoveRate = FLOAT1;

		// possession
		m_PossessionAni.Clear();
		m_fPossessionRate = FLOAT1;
		m_vPossessionBuff.clear();
		m_vPossessionEndBuff.clear();

		// end
		m_fPhantomFlyJumpAmt = 0.0f;

		// etc
		m_PhantomFlyEffect.Clear();
		m_dwPhantomFlyEnableTime = 0;
	}
} PhantomFlyInfo;
//////////////////////////////////////////////////////////////////////////
typedef struct tagLuciferFlyInfo
{
	LuciferFlyType m_Type;

	ioHashString m_StartAni;
	float m_fStartAniRate;

	float m_fStartAngle;
	float m_fStartMaxRange;
	DWORD m_dwStartDuration;

	ioHashString m_DelayAni;
	float m_fDelayAniRate;

	ioHashString m_MoveAni;
	float m_fMoveAniRate;

	DWORD m_dwRotateTime;
	DWORD m_dwChargeTime;

	ioHashString m_MoveAniUp;
	float m_fMoveAniUpRate;
	ioHashString m_MoveAniDown;
	float m_fMoveAniDownRate;

	float m_fMoveSpeed;
	float m_fUpDownSpeed;

	TargetWoundType m_TargetWoundType;
	float m_fTargetAngle;
	float m_fTargetRange;
	float m_fTargetUpHeight;
	float m_fTargetDownHeight;
	float m_fTargetMarkerRange;

	AttackAttribute m_Attack;

	ioHashString m_FlyEffect;
	ioHashString m_FlyMoveEffect;
	float m_fFlyJumpAmt;

	DWORD m_dwAirAttackDelayTime;
	ioHashString m_AirAttackActionAni;
	float m_fAirAttackActionAniRate;
	DWORD m_dwAirAttackActionType;

	tagLuciferFlyInfo()
	{
		Init();
	}

	void Init()
	{
		m_Type = LFT_NORMAL;

		m_fStartAniRate = FLOAT1;

		m_fStartAngle = 0.0f;
		m_fStartMaxRange = 0.0f;
		m_dwStartDuration = 0;

		m_fDelayAniRate = FLOAT1;
		m_fMoveAniRate = FLOAT1;

		m_dwRotateTime = 0;
		m_dwChargeTime = 0;

		m_fMoveAniUpRate = FLOAT1;
		m_fMoveAniDownRate = FLOAT1;

		m_fMoveSpeed = 0.0f;
		m_fUpDownSpeed = 0.0f;

		m_TargetWoundType = TWT_DOWN_WOUND_EXCEPT;
		m_fTargetAngle = 30.0f;
		m_fTargetRange = 30.0f;
		m_fTargetUpHeight = 30.0f;
		m_fTargetDownHeight = 30.0f;

		m_dwAirAttackDelayTime = 0;
		m_fAirAttackActionAniRate = FLOAT1;
		m_dwAirAttackActionType = 0;

		m_fFlyJumpAmt = 0.0f;
	}
} LuciferFlyInfo;

//피터팬
typedef struct tagPeterpanFlyInfo
{
	ioHashString m_StartAni;
	float m_fStartAniRate;

	float m_fStartAngle;
	float m_fStartMaxRange;
	DWORD m_dwStartDuration;

	ioHashString m_MoveAni;
	float m_fMoveAniRate;

	DWORD m_dwRotateTime;
	DWORD m_dwChargeTime;

	AttackAttribute m_Attack;

	ioHashString m_FlyEffect;
	ioHashString m_FlyMoveEffect;
	float m_fFlyJumpAmt;

	float m_fSpeed;
	float m_fFrictionAmt;
	float m_fGravityAmt;
	DWORD m_dwRotateSpeed;
	float m_fOffHeight;
	DWORD m_dwHeightOffTime;
	float m_fJumpPower;

	tagPeterpanFlyInfo()
	{
		Init();
	}

	void Init()
	{
		m_fStartAniRate = FLOAT1;

		m_fStartAngle = 0.0f;
		m_fStartMaxRange = 0.0f;
		m_dwStartDuration = 0;

		m_dwRotateTime = 0;
		m_dwChargeTime = 0;

		m_fFlyJumpAmt = 0.0f;

		m_fSpeed = 0;
		m_fFrictionAmt = 0;
		m_fGravityAmt = 0;
		m_dwRotateSpeed = 0;
		m_fOffHeight = 0;
		m_dwHeightOffTime = 0;
		m_fJumpPower = 0;
	}
} PeterpanFlyInfo;

//서큐버스
typedef struct tagSuccubusAttackInfo
{
	// start
	ioHashString	m_StartAniCenter;
	ioHashString	m_StartAniUp;
	ioHashString	m_StartAniDown;
	AttackAttribute m_FirstAttack;

	float m_fStartAngle;

	ioHashString	m_AttackLoopAniCenter;
	ioHashString	m_AttackLoopAniUp;
	ioHashString	m_AttackLoopAniDown;
	float			m_fAttackLoopAniRate;

	DWORD			m_dwWaitMaxDelayTime;

	ioHashString	m_AddAttackAni;
	AttackAttribute m_AddAttack;

	// 타겟 대상 검색용
	TargetWoundType m_TargetWoundType;
	float m_fTargetAngle;
	float m_fTargetRange;
	float m_fTargetUpHeight;
	float m_fTargetDownHeight;

	float m_fAttackMoveAngle;

	float m_fAttackerHeightRate;
	float m_fTargetHeightRate;

	AttributeList m_vExtendAttributeList;
	AttributeList m_vExtendPlusAttributeList;

	tagSuccubusAttackInfo()
	{
		Init();
	}

	void Init()
	{
		m_fStartAngle = 0.0f;
		m_fAttackLoopAniRate = FLOAT1;

		m_FirstAttack.Init();
		m_AddAttack.Init();

		m_TargetWoundType = TWT_ALL;
		m_fTargetAngle = 0.0f;
		m_fTargetRange = 0.0f;
		m_fTargetUpHeight = 0.0f;
		m_fTargetDownHeight = 0.0f;

		m_fAttackMoveAngle = 0.f;

		m_fAttackerHeightRate = FLOAT05;
		m_fTargetHeightRate = FLOAT05;
		m_dwWaitMaxDelayTime = 0;
	}
} SuccubusAttackInfo;

//하데스
typedef struct tagHadesAttackInfo
{
	// start
	ioHashString	m_StartAniCenter;
	ioHashString	m_StartAniUp;
	ioHashString	m_StartAniDown;

	AttackAttribute m_FirstAttack;
	AttackAttribute m_MoveAttackInfo;

	float m_fStartAngle;

	ioHashString	m_AttackLoopAniCenter;
	ioHashString	m_AttackLoopAniUp;
	ioHashString	m_AttackLoopAniDown;
	float			m_fAttackLoopAniRate;

	DWORD			m_dwWaitMaxDelayTime;

	ioHashString	m_AttackEndAni;
	float			m_fAttackEndAniRate;

	bool			m_bLandCheck;

	// 타겟 대상 검색용
	TargetWoundType m_TargetWoundType;
	float m_fTargetAngle;
	float m_fTargetRange;
	float m_fTargetUpHeight;
	float m_fTargetDownHeight;

	float m_fAttackMoveAngle;

	float m_fAttackerHeightRate;
	float m_fTargetHeightRate;

	float m_fEndJumpPower;
	float m_fAttackMoveMaxRange;
	float m_fAttackMoveSpeed;

	AttributeList m_vExtendPlusAttributeList;
	AttributeList m_vExtendPlusAirAttributeList;

	tagHadesAttackInfo()
	{
		Init();
	}

	void Init()
	{
		m_fStartAngle = 0.0f;
		m_fAttackLoopAniRate = FLOAT1;
		m_fAttackEndAniRate = FLOAT1;

		m_FirstAttack.Init();
		m_MoveAttackInfo.Init();

		m_vExtendPlusAttributeList.clear();
		m_vExtendPlusAirAttributeList.clear();

		m_TargetWoundType = TWT_ALL;
		m_fTargetAngle = 0.0f;
		m_fTargetRange = 0.0f;
		m_fTargetUpHeight = 0.0f;
		m_fTargetDownHeight = 0.0f;

		m_fAttackMoveAngle = 0.f;

		m_fAttackerHeightRate = FLOAT05;
		m_fTargetHeightRate = FLOAT05;
		m_dwWaitMaxDelayTime = 0;

		m_fEndJumpPower = 0.f;
		m_fAttackMoveMaxRange = 0.f;
		m_fAttackMoveSpeed = 0.f;

		m_bLandCheck = false;
	}
} HadesAttackInfo;

//미드나잇
typedef struct tagMidnightAttackInfo
{
	// start
	ioHashString	m_StartAniCenter;
	ioHashString	m_StartAniUp;
	ioHashString	m_StartAniDown;
	AttackAttribute m_FirstAttack;

	float m_fStartAngle;

	ioHashString	m_AttackLoopAniCenter;
	ioHashString	m_AttackLoopAniUp;
	ioHashString	m_AttackLoopAniDown;
	float			m_fAttackLoopAniRate;

	DWORD			m_dwWaitMaxDelayTime;

	ioHashString	m_AddAttackAni;
	AttackAttribute m_AddAttack;

	// 타겟 대상 검색용
	TargetWoundType m_TargetWoundType;
	float m_fTargetAngle;
	float m_fTargetRange;
	float m_fTargetUpHeight;
	float m_fTargetDownHeight;

	float m_fAttackMoveAngle;

	float m_fAttackerHeightRate;
	float m_fTargetHeightRate;

	AttributeList m_vExtendAttributeList;
	AttributeList m_vExtendPlusAttributeList;

	tagMidnightAttackInfo()
	{
		Init();
	}

	void Init()
	{
		m_fStartAngle = 0.0f;
		m_fAttackLoopAniRate = FLOAT1;

		m_FirstAttack.Init();
		m_AddAttack.Init();

		m_TargetWoundType = TWT_ALL;
		m_fTargetAngle = 0.0f;
		m_fTargetRange = 0.0f;
		m_fTargetUpHeight = 0.0f;
		m_fTargetDownHeight = 0.0f;

		m_fAttackMoveAngle = 0.f;

		m_fAttackerHeightRate = FLOAT05;
		m_fTargetHeightRate = FLOAT05;
		m_dwWaitMaxDelayTime = 0;
	}
} MidnightAttackInfo;

typedef struct tagDestroyerFlyInfo
{
	ioHashString m_StartAni;
	float m_fStartAniRate;

	float m_fStartAngle;
	float m_fStartMaxRange;
	DWORD m_dwStartDuration;

	ioHashString m_DelayAni;
	float m_fDelayAniRate;

	//////////////////////////////////////////////////////////////////////////
	ioHashString m_MoveAniFr;
	float m_fMoveAniFrRate;

	ioHashString m_MoveAniFrCharge;
	float m_fMoveAniFrRateCharge;
	//////////////////////////////////////////////////////////////////////////
	ioHashString m_MoveAniBk;
	float m_fMoveAniBkRate;

	ioHashString m_MoveAniBkCharge;
	float m_fMoveAniBkRateCharge;
	//////////////////////////////////////////////////////////////////////////
	ioHashString m_MoveAniRt;
	float m_fMoveAniRtRate;

	ioHashString m_MoveAniRtCharge;
	float m_fMoveAniRtRateCharge;
	//////////////////////////////////////////////////////////////////////////
	ioHashString m_MoveAniLt;
	float m_fMoveAniLtRate;

	ioHashString m_MoveAniLtCharge;
	float m_fMoveAniLtRateCharge;
	//////////////////////////////////////////////////////////////////////////
	float m_fMoveForceAmt;
	float m_fMoveForceFric;
	float m_fUpDownSpeed;
	
	TargetWoundType m_TargetWoundType;
	float m_fTargetAngle;
	float m_fTargetRange;
	float m_fTargetUpHeight;
	float m_fTargetDownHeight;
	float m_fTargetMarkerRange;

	float m_fAttackMoveSpeed;
	float m_fAttackMoveRange;
	float m_fAttackMoveRate;

	ioHashString m_AttackMoveAni;
	float m_fAttackMoveAniRate;

	AttackAttribute m_DestroyerAttack;//일반 공격

	//////////////////////////////////////////////////////////////////////////
	DWORD m_dwChargingTime;
	ioHashString m_ChargingAni;
	float m_fChargingAniRate;
	DWORD m_dwChargingAniPreDelay;

	ioHashString m_ChargedAni;
	float m_fChargedAniRate;

	AttackAttribute m_DestroyerChargeAttack;//차지 공격
	//////////////////////////////////////////////////////////////////////////

	ioHashString m_DestroyerFlyEffect;
	ioHashString m_DestroyerFlyMoveEffect;
	float m_fDestroyerFlyJumpAmt;

	tagDestroyerFlyInfo()
	{
		Init();
	}

	void Init()
	{
		m_fStartAniRate = FLOAT1;

		m_fStartAngle = 0.0f;
		m_fStartMaxRange = 0.0f;
		m_dwStartDuration = 0;

		m_fDelayAniRate = FLOAT1;

		m_fMoveAniFrRate = FLOAT1;
		m_fMoveAniBkRate = FLOAT1;
		m_fMoveAniRtRate = FLOAT1;
		m_fMoveAniLtRate = FLOAT1;
		m_fMoveAniFrRateCharge = FLOAT1;
		m_fMoveAniBkRateCharge = FLOAT1;
		m_fMoveAniRtRateCharge = FLOAT1;
		m_fMoveAniLtRateCharge = FLOAT1;

		m_fMoveForceAmt = 0.0f;
		m_fMoveForceFric = FLOAT1;
		m_fUpDownSpeed = 0.0f;

		m_TargetWoundType = TWT_DOWN_WOUND_EXCEPT;
		m_fTargetAngle = 30.0f;
		m_fTargetRange = 30.0f;
		m_fTargetUpHeight = 30.0f;
		m_fTargetDownHeight = 30.0f;

		m_fAttackMoveSpeed = 0.0f;
		m_fAttackMoveRange = 0.0f;
		m_fAttackMoveRate = FLOAT1;

		m_fAttackMoveAniRate = FLOAT1;

		m_fDestroyerFlyJumpAmt = 0.0f;

		m_dwChargingTime = 0;
		m_dwChargingAniPreDelay = 0;
		m_fChargingAniRate = FLOAT1;
		m_fChargedAniRate = FLOAT1;
	}
} DestroyerFlyInfo;
//////////////////////////////////////////////////////////////////////////
typedef struct tagMichaelFlyInfo
{
	// start - attack move - grappling loop - grappling move - end attack - jump or delay
	ioHashString m_StartAni;
	float m_fStartAniRate;

	float m_fStartAngle;
	float m_fStartMaxRange;
	DWORD m_dwStartDuration;

	// 타겟 대상 검색용
	TargetWoundType m_TargetWoundType;
	float m_fTargetAngle;
	float m_fTargetRange;
	float m_fTargetUpHeight;
	float m_fTargetDownHeight;

	// attack move
	AttackAttribute m_AttackMove;

	float m_fAttackMoveSpeed;
	float m_fAttackMoveMinRange;
	float m_fAttackMoveRange;
	float m_fAttackMoveRate;

	// grappling loop
	GrapplingSkillWoundedInfo m_GrapplingInfo;

	ioHashString m_GrapplingLoopAni;
	float m_fGrapplingLoopAniRate;
	DWORD m_dwGrapplingLoopDuration;
	DWORD m_dwGrapplingEnableMoveTime;

	float m_fGrapplingRange;
	float m_fGrapplingHeightRate;

	// grappling move
	ioHashString m_GrapplingMoveAni;
	float m_fGrapplingMoveAniRate;

	ioHashString m_GrapplingDirMoveAni;
	float m_fGrapplingDirMoveAniRate;

	float m_fGrapplingMoveSpeed;
	float m_fGrapplingMoveAngle;
	float m_fGrapplingMoveRange;

	float m_fGrapplingDirMoveSpeed;
	float m_fGrapplingDirMoveAngle;
	float m_fGrapplingDirMoveRange;

	// end attack
	AttackAttribute m_EndAttack;

	// 점프 전환용
	float m_fFlyEndJumpAmt;

	tagMichaelFlyInfo()
	{
		Init();
	}

	void Init()
	{
		// start
		m_fStartAniRate = FLOAT1;

		m_fStartAngle = 0.0f;
		m_fStartMaxRange = 0.0f;
		m_dwStartDuration = 0;

		m_TargetWoundType = TWT_ALL;
		m_fTargetAngle = 0.0f;
		m_fTargetRange = 0.0f;
		m_fTargetUpHeight = 0.0f;
		m_fTargetDownHeight = 0.0f;

		m_GrapplingInfo.Init();
		m_AttackMove.Init();

		m_fAttackMoveSpeed = 0.0f;
		m_fAttackMoveRange = 0.0f;
		m_fAttackMoveMinRange = 0.0f;
		m_fAttackMoveRate = FLOAT1;

		m_fGrapplingLoopAniRate = FLOAT1;
		m_dwGrapplingLoopDuration = 0;
		m_dwGrapplingEnableMoveTime = 0;

		m_fGrapplingRange = 0.0f;
		m_fGrapplingHeightRate = 0.0f;

		m_fGrapplingMoveAniRate = FLOAT1;
		m_fGrapplingDirMoveAniRate = FLOAT1;

		m_fGrapplingMoveSpeed = 0.0f;
		m_fGrapplingMoveAngle = 0.0f;
		m_fGrapplingMoveRange = 0.0f;

		m_fGrapplingDirMoveSpeed = 0.0f;
		m_fGrapplingDirMoveAngle = 0.0f;
		m_fGrapplingDirMoveRange = 0.0f;

		m_EndAttack.Init();

		m_fFlyEndJumpAmt = 0.0f;
	}
} MichaelFlyInfo;
//////////////////////////////////////////////////////////////////////////
typedef struct tagGoomihoFlyInfo
{
	// start
	ioHashString m_StartAni;
	float m_fStartAniRate;

	float m_fStartAngle;
	float m_fStartMaxRange;
	DWORD m_dwStartDuration;

	// 타겟 대상 검색용
	TargetWoundType m_TargetWoundType;
	float m_fTargetAngle;
	float m_fTargetRange;
	float m_fTargetUpHeight;
	float m_fTargetDownHeight;
	
	// attack move
	AttackAttribute m_AttackMove;
	float m_fAttackMoveAngle;

	float m_fAttackerHeightRate;
	float m_fTargetHeightRate;

	AttributeList m_vExtendAttributeList;
	AttributeList m_vExtendPlusAttributeList;

	float m_fAttackMoveSpeed;
	float m_fAttackMoveMinRange;
	float m_fAttackMoveRange;
	float m_fAttackMoveRate;

	// end attack
	AttackAttribute m_EndAttack;

	// 점프 전환용
	float m_fFlyEndJumpAmt;

	//점프중에 D~
	bool m_bJumpAttack;

	tagGoomihoFlyInfo()
	{
		Init();
	}

	void Init()
	{
		// start
		m_fStartAniRate = FLOAT1;

		m_fStartAngle = 0.0f;
		m_fStartMaxRange = 0.0f;
		m_dwStartDuration = 0;

		m_TargetWoundType = TWT_ALL;
		m_fTargetAngle = 0.0f;
		m_fTargetRange = 0.0f;
		m_fTargetUpHeight = 0.0f;
		m_fTargetDownHeight = 0.0f;

		m_AttackMove.Init();
		m_fAttackMoveAngle = 0.f;

		m_fAttackerHeightRate = FLOAT05;
		m_fTargetHeightRate = FLOAT05;

		m_fAttackMoveSpeed = 0.0f;
		m_fAttackMoveRange = 0.0f;
		m_fAttackMoveMinRange = 0.0f;
		m_fAttackMoveRate = FLOAT1;

		m_EndAttack.Init();
		m_vExtendAttributeList.clear();
		m_vExtendPlusAttributeList.clear();

		m_fFlyEndJumpAmt = 0.0f;
		m_bJumpAttack = false;
	}
} GoomihoFlyInfo;
//////////////////////////////////////////////////////////////////////////
typedef struct tagCyvogueFlyInfo
{
	ioHashString m_StartAni;
	float m_fStartAniRate;

	ioHashString m_DelayAni;
	float m_fDelayAniRate;

	ioHashString m_EndAni;
	float m_fEndAniRate;

	//////////////////////////////////////////////////////////////////////////
	ioHashString m_MoveAniFr;
	float m_fMoveAniFrRate;
	//////////////////////////////////////////////////////////////////////////
	ioHashString m_MoveAniBk;
	float m_fMoveAniBkRate;
	//////////////////////////////////////////////////////////////////////////
	ioHashString m_MoveAniRt;
	float m_fMoveAniRtRate;
	//////////////////////////////////////////////////////////////////////////
	ioHashString m_MoveAniLt;
	float m_fMoveAniLtRate;
	//////////////////////////////////////////////////////////////////////////
	AttackAttribute m_A_Attack;
	AttackAttribute m_S_Attack;
	AttackAttribute m_D_Attack;
	//////////////////////////////////////////////////////////////////////////

	tagCyvogueFlyInfo()
	{
		Init();
	}

	void Init()
	{
		m_A_Attack.Init();
		m_S_Attack.Init();
		m_D_Attack.Init();

		m_fStartAniRate = FLOAT1;
		m_fDelayAniRate = FLOAT1;
		m_fEndAniRate	= FLOAT1;

		m_fMoveAniFrRate = FLOAT1;
		m_fMoveAniBkRate = FLOAT1;
		m_fMoveAniRtRate = FLOAT1;
		m_fMoveAniLtRate = FLOAT1;
	}
} CyvogueFlyInfo;
//////////////////////////////////////////////////////////////////////////
typedef struct tagGhostStealerInfo
{
	AttackAttribute m_ExtendAttackAttribute;
	AttackAttribute m_AddExtendAttackAttribute;

	ioHashString m_Ani;
	float m_fAniRate;

	float m_fMoveSpeed;
	float m_fMoveRate;
	float m_fMoveAngle;
	float m_fMoveLength;
	float m_fMoveHeightRate;

	float m_fCurAngle;
	float m_fEndJumpAmt;


	tagGhostStealerInfo()
	{
		Init();
	}

	void Init()
	{
		m_ExtendAttackAttribute.Init();
		m_AddExtendAttackAttribute.Init();	

		m_fAniRate = FLOAT1;
		
		m_fMoveSpeed = 0.f;
		m_fMoveRate = 0.f;
		m_fMoveAngle = 0.f;
		m_fMoveLength = 0.f;

		m_fCurAngle = 0.f;
		m_fEndJumpAmt = 0.f;

		m_fMoveHeightRate = FLOAT1;
	}
} GhostStealerInfo;
//////////////////////////////////////////////////////////////////////////
typedef struct tagDraculaFlyInfo
{
	float m_fStartAngle;
	float m_fStartMaxRange;
	DWORD m_dwStartDuration;

	DWORD m_dwRotateTime;

	float m_fMoveSpeed;
	float m_fUpDownSpeed;

	ioHashStringVec m_vBuffList;

	tagDraculaFlyInfo()
	{
		Init();
	}

	void Init()
	{
		m_fStartAngle = 0.0f;
		m_fStartMaxRange = 0.0f;
		m_dwStartDuration = 0;

		m_dwRotateTime = 0;

		m_fMoveSpeed = 0.0f;
		m_fUpDownSpeed = 0.0f;
	}
} DraculaFlyInfo;
//////////////////////////////////////////////////////////////////////////
typedef struct tagHakumenDefenseInfo
{
	AttackAttribute m_DefenseAttack;

	ioHashString	m_stDefenseReadyMotion;
	float			m_fDefenseReadyMotionRate;

	ioHashString	m_stDefenseMotion;
	float			m_fDefenseMotionRate;

	float m_fEndJumpAmt;

	tagHakumenDefenseInfo()
	{
		Init();
	}

	void Init()
	{
		m_fEndJumpAmt = 0.f;
		m_fDefenseMotionRate = FLOAT1;
		m_fDefenseReadyMotionRate = FLOAT1;
	}
} HakumenDefenseInfo;
//////////////////////////////////////////////////////////////////////////
typedef struct tagDevilFlyInfo
{
	ioHashString m_StartAni;
	float m_fStartAniRate;

	float m_fStartAngle;
	float m_fStartMaxRange;
	DWORD m_dwStartDuration;

	DWORD m_dwFlyAttackTimeGap;

	float m_fFlyFireNeedGauge;

	ioHashString m_MoveAni;
	float m_fMoveAniRate;

	DWORD m_dwRotateTime;
	DWORD m_dwChargeTime;

	DWORD m_dwCancelTime;

	ioHashString m_MoveAniUp;
	float m_fMoveAniUpRate;
	ioHashString m_MoveAniDown;
	float m_fMoveAniDownRate;

	float m_fMoveSpeed;
	float m_fUpDownSpeed;

	DWORD m_dwMoveAttackDelay;

	AttackAttribute m_MoveAttack;
	AttackAttribute m_FireBallAttack;

	float m_fFlyJumpAmt;

	DWORD m_dwAirAttackDelayTime;
	ioHashString m_AirAttackActionAni;
	float m_fAirAttackActionAniRate;
	DWORD m_dwAirAttackActionType;

	tagDevilFlyInfo()
	{
		Init();
	}

	void Init()
	{
		m_fStartAniRate = FLOAT1;

		m_fStartAngle = 0.0f;
		m_fStartMaxRange = 0.0f;
		m_dwStartDuration = 0;

		m_dwFlyAttackTimeGap = 0;
		m_fMoveAniRate = FLOAT1;

		m_dwRotateTime = 0;
		m_dwChargeTime = 0;

		m_fMoveAniUpRate = FLOAT1;
		m_fMoveAniDownRate = FLOAT1;

		m_fMoveSpeed = 0.0f;
		m_fUpDownSpeed = 0.0f;

		m_dwAirAttackDelayTime = 0;
		m_fAirAttackActionAniRate = FLOAT1;
		m_dwAirAttackActionType = 0;

		m_fFlyJumpAmt = 0.0f;

		m_dwMoveAttackDelay = 0;

		m_fFlyFireNeedGauge = 0.f;

		m_dwCancelTime = 0;
	}
} DevilFlyInfo;
//////////////////////////////////////////////////////////////////////////
const int MAX_TRINITY_ATTACK_CNT = 6;

typedef struct tagTrinityInfo
{
	AttackAttribute m_FullAttack[MAX_TRINITY_ATTACK_CNT];
	AttackAttribute m_NoneFullAttack[MAX_TRINITY_ATTACK_CNT];

	ioHashStringVec	m_AddFullMesh[MAX_TRINITY_ATTACK_CNT];
	ioHashStringVec	m_AddNoneMesh[MAX_TRINITY_ATTACK_CNT];
	ioHashStringVec m_AddNoneMeshForJump[MAX_TRINITY_ATTACK_CNT];
	ioHashStringVec m_AddNormalMesh[MAX_TRINITY_ATTACK_CNT];

	ioHashString	m_AddFullEffect[MAX_TRINITY_ATTACK_CNT];
	ioHashString	m_AddNoneEffect[MAX_TRINITY_ATTACK_CNT];
	ioHashString    m_AddNoneEffectForJump[MAX_TRINITY_ATTACK_CNT];
	ioHashString	m_AddNormalEffect[MAX_TRINITY_ATTACK_CNT];

	bool			m_bUseGravityForJumpAttack[MAX_TRINITY_ATTACK_CNT];

	ioHashString	m_ReloadAni;
	float			m_fReloadAniRate;
	//////////////////////////////////////////////////////////////////////////

	tagTrinityInfo()
	{
		Init();
	}

	void Init()
	{
		for( size_t i = 0; i < MAX_TRINITY_ATTACK_CNT; i++ )
		{
			m_FullAttack[i].Init();
			m_NoneFullAttack[i].Init();
			m_bUseGravityForJumpAttack[i] = false;
		}

		m_fReloadAniRate = FLOAT1;
	}
} TrinityInfo;
//////////////////////////////////////////////////////////////////////////
typedef struct tagItemJumpAttackInfo
{
	GrapplingSkillWoundedInfo m_GrapplingInfo;
	float			m_fJumpPower;
	float			m_fGravity;
	AttackAttribute m_LandAttackAttribute;

	bool m_bGrapAttackUseMove;

	tagItemJumpAttackInfo()
	{
		Init();
	}

	void Init()
	{
		m_fJumpPower = 0.f;
		m_fGravity = 0.0f;
		m_bGrapAttackUseMove = false;
	}

} ItemJumpAttackInfo;
//////////////////////////////////////////////////////////////////////////
typedef struct tagEvyBlinkInfo
{
	// start
	float m_fDashSpeed;
	float m_fDashLength;
	ioHashString m_stDashAni;
	float m_fDashAniRate;

	ioHashString m_stDashEndAni;
	float m_fDashEndAniRate;

	float m_fMaxBlinkLength;

	float m_fMapHeightGap;

	ioHashString m_StartBlinkMotion;
	float		 m_fStartBlinkMotionRate;

	ioHashString m_EndBlinkMotion;
	float m_fEndBlinkMotionRate;

	float m_fJumpAmt;

	//약 D꾹 후 나가는 정보
	AttributeList m_SpecialAttributeList;
	float m_fSpecilAfterJumpPower;

	tagEvyBlinkInfo()
	{
		Init();
	}

	void Init()
	{
		m_fDashSpeed = 0.f;
		m_fDashLength = 0.f;
		m_fDashAniRate = FLOAT1;
		m_fDashEndAniRate = FLOAT1;

		m_fMaxBlinkLength = 0.f;
		m_fMapHeightGap = 0.f;
		m_fEndBlinkMotionRate = FLOAT1;

		m_fJumpAmt = 0.f;

		m_fSpecilAfterJumpPower = 0.f;
	}
} EvyBlinkInfo;
//////////////////////////////////////////////////////////////////////////
// 탑블레이트
typedef struct tagStTopMoveInfo
{
	float fRotate_x;
	float fRotate_y;
	float fLength;
	bool  bCreate;
}StTopMoveInfo;
//////////////////////////////////////////////////////////////////////////
typedef struct tagValkyrieFlyInfo
{
	ioHashString	m_WaitAnimation;
	DWORD			m_dwWaitDuration;

	float			m_fChargeFlyAngle;
	float			m_fChargeFlyMaxRange;

	float			m_fChargeFlyMaxSpeed;
	float			m_fChargeFlyMinSpeed;
	float			m_fChargeFlyAccelSpeed;
	float			m_fChargeFlyJumpAmt;

	DWORD			m_dwChargeFlyMinSpeedDuration;	
	DWORD			m_dwRotateTime;

	AttackAttribute	m_Attribute;
	
	tagValkyrieFlyInfo()
	{
		Init();
	}

	void Init()
	{
		m_WaitAnimation.Clear();

		m_fChargeFlyAngle				= 0.0f;
		m_fChargeFlyMaxRange			= 0.0f;
		m_fChargeFlyMaxSpeed			= 0.0f;
		m_fChargeFlyMinSpeed			= 0.0f;		
		m_fChargeFlyAccelSpeed			= 0.0f;
		m_fChargeFlyJumpAmt				= 0.0f;

		m_dwWaitDuration				= 0;
		m_dwChargeFlyMinSpeedDuration	= 0;
		m_dwRotateTime					= 0;

		m_Attribute.Init();
	}

} ValkyrieFlyInfo;
//////////////////////////////////////////////////////////////////////////
typedef struct tagBladeFlyInfo
{
	ioHashString m_StartAni;
	float m_fStartAniRate;

	float m_fStartAngle;
	float m_fStartMaxRange;
	float m_fStartSpeed;

	tagBladeFlyInfo()
	{
		Init();
	}

	void Init()
	{
		m_fStartAniRate = FLOAT1;

		m_fStartAngle = 0.0f;
		m_fStartMaxRange = 0.0f;
		m_fStartSpeed = 0.f;
	}
} BladeFlyInfo;
//////////////////////////////////////////////////////////////////////////
//전우치
typedef struct tagJeonWooChiDashMoveInfo
{
	AttackAttribute m_PowerChargeAttribute;
	float			m_fMaxMoveLength;
	float			m_fMoveSpeed;
	ioHashString	m_stMoveAni;
	float			m_fMoveAniRate;
	AttackAttribute	m_MoveAttack;
	AttackAttribute m_AddMoveAttack;
	
	tagJeonWooChiDashMoveInfo()
	{
		Init();
	}

	void Init()
	{
		m_fMaxMoveLength = 0.f;
		m_fMoveSpeed = 0.f;
		m_fMoveAniRate = FLOAT1;
	}

}JeonWooChiDashMoveInfo;
//////////////////////////////////////////////////////////////////////////
typedef struct tagRedHoodFlyInfo
{
	ioHashString m_StartAni;
	float m_fStartAniRate;

	float m_fStartAngle;
	float m_fStartMaxRange;
	float m_fStartSpeed;

	float m_fEndJumpAmt;

	tagRedHoodFlyInfo()
	{
		Init();
	}

	void Init()
	{
		m_fStartAniRate = FLOAT1;

		m_fStartAngle = 0.0f;
		m_fStartMaxRange = 0.0f;
		m_fStartSpeed = 0.f;

		m_fEndJumpAmt = 0.f;
	}
} RedHoodFlyInfo;
//////////////////////////////////////////////////////////////////////////
typedef struct tagBaseCharDistanceSq
{
	ioHashString m_Name;
	float m_fDistanceSq;
	D3DXVECTOR3 m_vPos;

	tagBaseCharDistanceSq()
	{
		m_fDistanceSq = 0.0f;
		m_vPos = ioMath::VEC3_ZERO;
	}

} BaseCharDistanceSq;
typedef std::vector< BaseCharDistanceSq > BaseCharDistanceSqList;

class BaseCharDistanceSqSort : std::binary_function< const BaseCharDistanceSq&, const BaseCharDistanceSq&, bool >
{
public:
	bool operator()( const BaseCharDistanceSq &lhs , const BaseCharDistanceSq &rhs ) const
	{
		if( lhs.m_fDistanceSq > rhs.m_fDistanceSq )
		{
			return true;
		}

		return false;
	}
};

class BaseCharDistanceSqSort2 : std::binary_function< const BaseCharDistanceSq&, const BaseCharDistanceSq&, bool >
{
public:
	bool operator()( const BaseCharDistanceSq &lhs , const BaseCharDistanceSq &rhs ) const
	{
		if( lhs.m_fDistanceSq < rhs.m_fDistanceSq )
		{
			return true;
		}

		return false;
	}
};

typedef struct tagBaseCharHP
{
	ioHashString m_Name;
	float m_fHP;
	bool m_bPrisoner;

	tagBaseCharHP()
	{
		m_fHP = 0.0f;
		m_bPrisoner = false;
	}

} BaseCharHP;
typedef std::vector< BaseCharHP > BaseCharHPList;

class BaseCharHPSort : std::binary_function< const BaseCharHP&, const BaseCharHP&, bool >
{
public:
	bool operator()( const BaseCharHP &lhs , const BaseCharHP &rhs ) const
	{
		if ( lhs.m_bPrisoner )
			return false;

		if( lhs.m_fHP < rhs.m_fHP )
		{
			return true;
		}

		return false;
	}
};

enum DashAttEndType
{
	DAET_NONE,
	DAET_NORMAL,
	DAET_EXTRA,
};

struct BuffValueInfo
{
	ioHashString m_BuffName;
	ioHashString m_szBuffValue;
	float m_fBuffValue;
	float m_fBuffValue2;

	BuffValueInfo()
	{
		m_fBuffValue = 0.0f;
		m_fBuffValue2 = 0.0f;
	}
};
typedef std::vector<BuffValueInfo> vBuffValueInfoInfoList;

class BuffValueInfoSort : std::binary_function< const BuffValueInfo&, const BuffValueInfo&, bool >
{
public:
	bool operator()( const BuffValueInfo &lhs , const BuffValueInfo &rhs ) const
	{
		if( lhs.m_fBuffValue > rhs.m_fBuffValue )
		{
			return true;
		}

		return false;
	}
};

struct DummyPosInfo
{
	D3DXVECTOR3 m_vPos;
	ioHashString m_OwnerName;
	int m_iDummyIndex;
};
typedef std::vector<DummyPosInfo> SummonList;

typedef struct tagBuffCameraInfo
{
	ioHashString m_BuffName;
	int m_iBuffCamType;

	float m_fBuffCamDistance;
	float m_fBuffCamHeight;
	float m_fBuffCamFov;
	DWORD m_dwBuffCamZoomTime;
	bool m_bPriorityOverSkill;

	tagBuffCameraInfo()
	{
		Init();
	}

	void Init()
	{
		m_iBuffCamType = CT_DEFAULT;
		
		m_fBuffCamDistance = 0.0f;
		m_fBuffCamHeight = 0.0f;
		m_fBuffCamFov = 0.0f;

		m_dwBuffCamZoomTime = 0;

		m_bPriorityOverSkill = false;
	}

} BuffCameraInfo;
typedef std::vector<BuffCameraInfo> BuffCamInfoList;

typedef struct tagSlasherRunInfo
{
	float m_fFirstRunSpeed;
	DWORD m_dwFirstRunTime;
	float m_fFirstMaxMoveDist;

	float m_fSecondRunSpeed;
	DWORD m_dwSecondRunTime;
	float m_fSecondMaxMoveDist;
	
	ioHashString m_szFirstRunReadyAni;
	float m_fFirstRunReadyAniRate;
	ioHashString m_szFirstRunLoopAni;
	ioHashString m_szFirstRunEndAni;
	float m_fFirstRunEndAniRate;

	ioHashString m_szSecondRunReadyAni;
	float m_fSecondRunReadyAniRate;
	ioHashString m_szSecondRunLoopAni;
	ioHashString m_szSecondRunEndAni;
	float m_fSecondRunEndAniRate;

	WeaponInfo m_sFirstInfo;
	WeaponInfo m_sSecondInfo;

	tagSlasherRunInfo()
	{
		Init();
	}

	void Init()
	{
		m_fFirstRunSpeed = 0.0f;
		m_dwFirstRunTime = 0;
		m_fFirstMaxMoveDist = 0.0f;

		m_fSecondRunSpeed = 0.0f;
		m_dwSecondRunTime = 0;
		m_fSecondMaxMoveDist = 0.0f;
		
		m_fFirstRunReadyAniRate = FLOAT1;
		m_fFirstRunEndAniRate = FLOAT1;

		m_szFirstRunReadyAni.Clear();
		m_szFirstRunLoopAni.Clear();
		m_szFirstRunEndAni.Clear();

		m_fSecondRunReadyAniRate = FLOAT1;
		m_fSecondRunEndAniRate = FLOAT1;

		m_szSecondRunReadyAni.Clear();
		m_szSecondRunLoopAni.Clear();
		m_szSecondRunEndAni.Clear();

		m_sFirstInfo.Init();
		m_sSecondInfo.Init();
	}

}SlasherRunInfo;

//타이탄
typedef struct tagTitanExtendInfo
{
	AttackAttribute   attribute;
	AttackAttribute   attribute_land;
	
	float			m_fMoveSpeed;
	float			m_fMoveAngle;
	float			m_fMoveRate;
	

	tagTitanExtendInfo()
	{
		Init();
	}

	void Init()
	{
		attribute.Init();
		attribute_land.Init();

		m_fMoveSpeed = 0.f;
		m_fMoveAngle = 0.f;
		m_fMoveRate = FLOAT1;
	}

}TitanExtendInfo;
//////////////////////////////////////////////////////////////////////////
//전우치
typedef struct tagJeonWooChiExtendInfo
{
	AttackAttribute   attribute;
	AttackAttribute   attribute_land;

	float			m_fMoveSpeed;
	float			m_fMoveAngle;
	float			m_fMoveRate;


	tagJeonWooChiExtendInfo()
	{
		Init();
	}

	void Init()
	{
		attribute.Init();
		attribute_land.Init();

		m_fMoveSpeed = 0.f;
		m_fMoveAngle = 0.f;
		m_fMoveRate = FLOAT1;
	}

}JeonWooChiExtendInfo;

//초시공전사
typedef struct tagSuperDimension_HP_Scale_Info
{
	float		m_fHpPercent;
	float		m_fScale;
	float		m_fWeaponScale;

	tagSuperDimension_HP_Scale_Info()
	{
		Init();
	}

	void Init()
	{
		m_fHpPercent = FLOAT1;
		m_fScale = FLOAT1;
		m_fWeaponScale = FLOAT1;
	}
}SuperDimensionHpScaleInfo;

typedef struct tagSuperDimensionExtendDashInfo
{
	// start - attack move - grappling loop - grappling move - end attack - jump or delay
	ioHashString m_StartAni;
	float m_fStartAniRate;
	DWORD m_dwStartDuration;

	// 타겟 대상 검색용
	TargetWoundType m_TargetWoundType;
	float m_fTargetAngle;
	float m_fTargetRange;
	float m_fTargetUpHeight;
	float m_fTargetDownHeight;

	// attack move
	AttackAttribute m_AttackMove;

	float m_fAttackMoveSpeed;
	float m_fAttackMoveRange;

	// grappling attack 
	GrapplingSkillWoundedInfo m_GrapplingInfo;
	ioHashString m_GrapplingAttackAni;
	float m_fGrapplingAttackAniRate;

	//loop
	ioHashString m_GrapplingLoopAni;
	float m_fGrapplingLoopAniRate;
	DWORD m_dwGrapplingLoopRotateSpeed;
	float m_fGrapplingLoopHeightRate;
	float m_fGrapplingLoopGapRange;
	DWORD m_dwGrapplingLoopMaxTime;
	float m_fGrapplingRange;
	float m_fGrapplingHeightRate;

	ioHashString m_GrapplingWoundLoopAni;
	float m_fGrapplingLoopWoundAniRate;
	
	// grappling move
	ioHashString m_GrapplingMoveAni;
	float m_fGrapplingMoveAniRate;

	ioHashString m_GrapplingDirMoveAni;
	float m_fGrapplingDirMoveAniRate;

	float m_fGrapplingMoveSpeed;
	float m_fGrapplingMoveAngle;
	float m_fGrapplingMoveRange;

	// end attack
	AttackAttribute m_EndAttack;

	// 점프 전환용
	float m_fFlyEndJumpAmt;

	tagSuperDimensionExtendDashInfo()
	{
		Init();
	}

	void Init()
	{
		// start
		m_fStartAniRate = FLOAT1;
		m_dwStartDuration = 0;

		m_TargetWoundType = TWT_ALL;
		m_fTargetAngle = 0.0f;
		m_fTargetRange = 0.0f;
		m_fTargetUpHeight = 0.0f;
		m_fTargetDownHeight = 0.0f;

		m_GrapplingInfo.Init();
		m_AttackMove.Init();

		m_fAttackMoveSpeed = 0.0f;
		m_fAttackMoveRange = 0.0f;

		m_fGrapplingLoopAniRate = FLOAT1;

		m_fGrapplingRange = 0.0f;
		m_fGrapplingHeightRate = 0.0f;

		m_fGrapplingMoveAniRate = FLOAT1;
		m_fGrapplingDirMoveAniRate = FLOAT1;

		m_fGrapplingMoveSpeed = 0.0f;
		m_fGrapplingMoveAngle = 0.0f;
		m_fGrapplingMoveRange = 0.0f;

		m_EndAttack.Init();

		m_fFlyEndJumpAmt = 0.0f;
	}
} SuperDimensionExtendDashInfo;
//////////////////////////////////////////////////////////////////////////
typedef struct tagCuchulainSearchInfo
{
	float fAngle;
	float fRange;

	tagCuchulainSearchInfo()
	{
		Init();
	}

	void Init()
	{
		fAngle = 0.f;
		fRange = 0.f;
	}

} CuchulainSearchInfo;

typedef struct tagCuchulainFlyInfo
{
	ioHashString m_StartAni;
	ioHashString m_FlyAni;
	float m_fStartAniRate;
	float m_fFlyAniRate;

	DWORD m_dwStartWaitTime;
	float m_fFlyAngle;
	float m_fFlyMaxRange;
	float m_fFlySpeed;

	ioHashString m_stBuff;
	float m_fEndJumpPower;

	tagCuchulainFlyInfo()
	{
		Init();
	}

	void Init()
	{
		m_dwStartWaitTime = 0;

		m_fStartAniRate = FLOAT1;
		m_fFlyAniRate = FLOAT1;

		m_fFlyAngle = 0.0f;
		m_fFlyMaxRange = 0.0f;
		m_fFlySpeed = 0.f;

		m_fEndJumpPower = 0.f;
	}
} CuchulainFlyInfo;

typedef struct tagMagicEngineerAttackInfo
{
	ioHashString m_StartAni;
	float m_fAniRate;

	tagMagicEngineerAttackInfo()
	{
		Init();
	}

	void Init()
	{
		m_fAniRate = 1.f;
	}
} MagicEngineerAttackInfo;

typedef struct tagMEDummyMoveInfo
{
	ioHashString m_DummyCharName;
	D3DXVECTOR3  m_vDummyCharOffset;
	float m_fRotate_x;
	float m_fRotate_y;
	float m_fLength;
}MEDummyMoveInfo;