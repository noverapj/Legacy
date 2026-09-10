#pragma once

#include "../io3DEngine/ioINILoader.h"

enum AttackMethodType
{
	AMT_NONE,
	AMT_RANGE,
	AMT_CONTACT,
	AMT_MAGIC,
	AMT_BOMB,
	MAX_AMT_TYPE
};

enum AttackStatType
{
	ATT_STAT_NONE,
	ATT_STAT_STR,
	ATT_STAT_DEX,
	ATT_STAT_INT,
	ATT_STAT_VIT,
};

enum AttackWeaponType
{
	AWT_DUMMY,
	AWT_CONTACT,
	AWT_MISSILE,
	AWT_THROW,
	AWT_BOMB,
	AWT_MINE,
	AWT_NUCLEAR,
	AWT_FLAME,
	AWT_ZONE_EFFECT,
	AWT_FLAMEWAVE,
	AWT_SPLASH,						// 10,
	AWT_ARROW,
	AWT_BLIZZARD,
	AWT_THROWBOMB,
	AWT_FLOAT_THROW,
	AWT_THROW_RETURN,
	AWT_EXPLOSION,
	AWT_BOMB_MISSILE,
	AWT_CANNON_BALL,
	AWT_ROTATE,
	AWT_TRACE,						// 20,
	AWT_BOUNDBOMB,
	AWT_TARGET,
	AWT_CHAIN,
	AWT_BALL_WEAPON,
	AWT_IGNORE_MISSILE,
	AWT_AREA_EFFECT,
	AWT_MAP_ATTACH,
	AWT_LIMIT_ATTACH,
	AWT_OWNER_ATTACK,
	AWT_RUN_CONTACT,				// 30,
	AWT_WHIP,
	AWT_ICE_MISSILE,
	AWT_SUMMON_MISSILE,
	AWT_GROUND_TRACE,
	AWT_CONTROL_RETURN,
	AWT_CONTROL_MISSILE,
	AWT_TRACE_RETURN,
	AWT_TSUNAMI,
	AWT_TRACE2,
	AWT_NORMAL,						// 40,
	AWT_ADHESIVE_MISSILE,
	AWT_LIMIT_ATTACH2,
	AWT_CONTACT2,
	AWT_BOMB_MISSILE2,
	AWT_ADHESIVE_MISSILE2,
	AWT_ZONE_EFFECT2,
	AWT_UROBORUS,
	AWT_UROBORUS2,
	AWT_TRACE3,
	AWT_MISSILE2,					// 50,
	AWT_CALL_ATTRIBUTE_ARROW,
	AWT_RETURN_PINGPONG,
	AWT_MISSILE3,
	AWT_LOBELIA,
	AWT_SYLPHID_MISSILE,
	AWT_GUIDE,
	AWT_BALL2_WEAPON,
	AWT_GUIDE2,
	AWT_TRACE4,
	AWT_FALL_WEAPON,				//60
	AWT_TRACE5,
	AWT_DOUBLE_ICE_MISSILE_WEAPON,
	AWT_SMART_BOOM_WEAPON,
	AWT_PULL_WEAPON,
	AWT_ADHESIVE_MISSILE3,
	AWT_GRAFFITI,
	AWT_MINE2,
	AWT_TRACE6,
	AWT_CONTROL_SWORD,
	AWT_PULL_WEAPON2,				//70
	AWT_GROUND_TRACE2,
	AWT_SELECT_POSITION_MOVE,
	AWT_SIMPLE_BOX,
	AWT_ADHESIVE_MISSILE4,
	AWT_BOMB_MISSILE3,		//제갈량 공중 공격
	AWT_TRACE7,
	AWT_UROBORUS3,
	AWT_LUCIFER_AIR_GRAP,			//루시퍼 공중 잡기
	AWT_TRACE_OWNER,
	AWT_IGNORE_MISSILE2,			//80
	AWT_ONE_TARGET_WEAPON,
	AWT_BOMB_MISSILE4,
	AWT_ATTACH_WEAPON,
	AWT_RETURN_MISSILE_WEAPON,
	AWT_UROBORUS4,
	AWT_FIRE_WORKS_STEP1_WEAPON,
	AWT_FIRE_THOW_BOM_WEAPON,
	AWT_THROWBOMB2,
	AWT_ROTATE_TARGET_WEAPON,
	AWT_MINE3,						//90
	AWT_RETURN_MISSILE_WEAPON2,
	AWT_ZONE_MOVE_WEAPON,
	AWT_PRISON_WEAPON,
	AWT_UROBORUS5,
	AWT_ROTATE2,
	AWT_CONTROL_MESH,
	AWT_MINE4,
	AWT_DESTROY_CALL,
	AWT_ATTACH_AREA,
	AWT_MISSILE4,					//100
	AWT_UROBORUS6,
	AWT_ROTATE_ATTACH_CHAR,
	AWT_PULL_WEAPON3,
	AWT_CONTACT3,
	AWT_BOMB_MISSILE5,
	AWT_MINE5,
	AWT_MISSILE5,
	AWT_PULL_WEAPON4,
	AWT_ROTATE_RETURN_WEAPON,
	AWT_UROBORUS7,					//110
	AWT_CONTROL_RETURN2,
	AWT_GROUND_TRACE3,
	AWT_THROWBOMB3,
	AWT_SUMMON_MISSILE2,
	AWT_MINE6,
	AWT_PRISON_WEAPON2,
	AWT_NO_COLLISION_WEAPON,
	AWT_MISSILE6,
	AWT_CONTINIOUS_CREATE,
	AWT_PRISON_WEAPON3,
	AWT_MAX,
};

enum BlowTargetType
{
	BTT_WEAK				= 0,	//약피격
	BTT_BLOW				= 1,	//강피격, Blow_Wounded 애니메이션 사용
	BTT_FLATTEN				= 2,	//납짝
	BTT_M_STIFF				= 3,	//경직(단), BTT_M_STIFF 상태에서 다시 BTT_M_STIFF이 들어오면 BTT_STAND_KO으로 변경
	BTT_S_STIFF				= 4,	//경직(장), 스턴상태
	BTT_STAND_KO			= 5,	//무릎 꿇기(천천히 쓰러짐)
	BTT_STAND_KO2			= 6,	//무릎 꿇기(천천히 쓰러짐), 다른 애니메이션 사용
	BTT_BLOW2				= 7,	//강피격, BTT_BLOW과 다른 Blow_Wounded 애니메이션 사용
	BTT_BLOW3				= 8,	//강피격, BTT_BLOW과 다른 Blow_Wounded 애니메이션 사용
	BTT_BOUND_BLOW			= 9,	//바운드, Bound_Blow_Wounded 애니메이션 사용
	BTT_M_BLOW				= 10,	//약경직, BTT_M_BLOW 상태에서 BTT_M_BLOW가 추가로 들어올경우 BTT_BLOW3변경, BTT_M_BLOW상태에서 BTT_M_STIFF들어 올 경우 BTT_STAND_KO 변경
	BTT_GETUP				= 11,	//기립, BTT_GETUP 상태에서 BTT_M_STIFF가 들어오면 BTT_STAND_KO로 변경
	BTT_GETUP_STAND_KO		= 12,	//강제 기립, 상태에 관계없이 대상을 일으켜 세워 BTT_STAND_KO2로 변경
	BTT_BLOW_EXTEND_WOUND	= 13,	//날려가다가 지면에서 서서히 미끄러짐
	BTT_MAX_TYPE,
	BTT_NONE,
};

enum CharPiercingType
{
	CPT_NONE,
	CPT_NORMAL,
	CPT_LIMITE,
	CPT_ALL,
	CPT_ALL_LIMITE,
	CPT_MAX,
};

enum DefenseBreakType
{
	DBT_NONE,
	DBT_NORMAL,
	DBT_ALL,
	DBT_CHANGE,
	DBT_MAX,
};

enum IgnoreProtectType
{
	IPT_NONE,
	IPT_ALL,
	IPT_EXCEPT_SYS,
};

enum DefenseSuccessType
{
	DST_STRONG,
	DST_WEAK,
	DST_SKILL,
	DST_MAX,
};

enum EnableSideType
{
	EST_NORMAL,
	EST_FRONT,
	EST_BACK,
	EST_MAX,
};

enum ChangeJumpType
{
	CJT_NONE,
	CJT_NORMAL,
	CJT_WOUNDED_ONLY,
	CJT_BLOW_WOUNDED_ONLY,
};

enum ChangeNextItemType
{
	CNIT_NONE,
	CNIT_TYPE1,//(무기쪽에 사용)
	CNIT_TYPE2,//(점프쪽에 사용)
	CNIT_TYPE3,//(버프쪽에 사용)
	CNIT_TYPE4,//(강림도령 사용)
	CNIT_TIGER1,//타이거 진화때 사용
	CNIT_TIGER2,//타이거 진화때 사용
	CNIT_TYPE5,//(대쉬쪽에 사용)
};

//딱지 스킬때 추가
enum ChangeNextSkillType
{
	CNST_NONE,
	CNST_TYPE1,
};

enum EscapeCatchType
{
	ECT_NONE,
	ECT_CATCH,
	ECT_CATCH_RETURN,
};

enum SkillEndJumpType
{
	SEJT_DEFAULT,
	SEJT_NOT_JUMP,
	SEJT_JUMP,	
};

enum WeaponExtraGaugeType
{
	WGT_NONE			= 0,
	WGT_EXCALIBUR		= 1,
	WGT_COMMAND			= 2,
	WGT_YUMIRA			= 3,
	WGT_CHARGE_CONTACT	= 4,
	WGT_WINDLORD		= 5,
	WGT_DARKKNIGHT		= 6,
	WGT_LEESUNSIN		= 7,
	WGT_ONIMUSHA		= 8,
	WGT_SASINMU			= 9,
	WGT_MEPHISTO		= 10,
	WGT_STRANGER		= 11,
	WGT_JINRAH			= 12,
	WGT_UFC				= 13,
	WGT_CHANGE_ON		= 14,
	WGT_SALADIN_EXPAND	= 15,	
	WGT_OMEN			= 16,
	WGT_EVA				= 17	// 2020-11-03 이브 추가
};

enum MineType
{
	MIT_MINE,
	MIT_TRACE,
	MIT_TRAP,
	MIT_TRAP2,
	MIT_AIR,
	MIT_CONTROL_MINE,
	MIT_CIRCLE_MINE,
	MIT_TRAP3,
	MIT_TRAP4,
	MIT_MAX_CNT,
};

typedef struct BuffInfoStruct
{
	ioHashString m_BuffName;
	int m_iBuffType;
	bool m_bCallNextWeapon;

	BuffInfoStruct()
	{
		m_BuffName.Clear();
		m_iBuffType = 0;
		m_bCallNextWeapon = false;
	}

	BuffInfoStruct( const ioHashString &szName, int iType )
	{
		m_BuffName  = szName;
		m_iBuffType = iType;
		m_bCallNextWeapon = false;
	}
} ioBuffInfo;

typedef std::vector< ioBuffInfo > BuffInfoList;

typedef struct tagBlowWoundAniInfo
{
	ioHashString m_BlowingAni;
	ioHashString m_BlowingBackAni;

	ioHashString m_JumpBlowingAni;
	ioHashString m_JumpBlowingBackAni;

	ioHashString m_DownAni;
	ioHashString m_DownBackAni;

	tagBlowWoundAniInfo()
	{
		Init();
	}

	void Init()
	{
		m_BlowingAni.Clear();
		m_BlowingBackAni.Clear();

		m_JumpBlowingAni.Clear();
		m_JumpBlowingBackAni.Clear();

		m_DownAni.Clear();
		m_DownBackAni.Clear();
	}
} BlowWoundAniInfo;

struct WeaponAttribute
{
	int m_iAttributeIndex;

	AttackWeaponType m_WeaponType;
	BlowTargetType	 m_BlowTargetType;
	ElementType		 m_ElementType;
	AttackStatType	 m_StatType;
	CharPiercingType m_PiercingType;
	DefenseBreakType m_DefenseBreakType;
	DefenseSuccessType m_DefenseSuccessType;
	AttackMethodType m_AttackMethodType;
	EnableSideType m_EnableSideType;
	EscapeCatchType m_EscapeCatchType;

	int m_iShakeCamera;

	float m_fPushPower;
	float m_fBlowPower;
	bool  m_bNoCalcFloatingCnt;//계속 공격 당할시 FloatCnt가 중가 되면서 BlowPower가 점점 준다. 그걸 예외 처리 할때 필요
	float m_fAirPushPower;
	float m_fAirBlowPower;
	float m_fDefensePushPower;
	float m_fPushAngle;

	float m_fBallPushPower;
	float m_fBallBlowPower;
	float m_fBallAirPushPower;
	float m_fBallAirBlowPower;

	float m_fGangsiPushPower;
	float m_fGangsiBlowPower;
	float m_fGangsiAirPushPower;
	float m_fGangsiAirBlowPower;

	CEncrypt<float> m_fDamageRate;
	float m_fDefenseEnableRate;
	float m_fNeedGauge;

	float m_fDownStateBlowPowerRate;
	float m_fDownStatePushPowerRate;

	float m_fBoundBlowJumpAmt;
	float m_fBoundBlowForceAmt;
	float m_fBoundBlowGravityRate;
	CEncrypt<float> m_fBoundBlowBoundDamage;

	CEncrypt<float> m_fDefenseDamageRate;
	CEncrypt<float> m_fDefenseDamageRate2;

	ioHashString m_ExtendWoundAni;
	DWORD m_dwExtendWoundDuration;
	float m_fExtendWoundJumpAmt;
	float m_fExtendWoundForceAmt;

	BlowWoundAniInfo m_BlowWoundAniInfo;

	ioHashString m_ExplosionEffect;
	ioHashString m_ExtraExplosionEffect;
	ioHashString m_WeaponEffect;
	ioHashString m_WeaponEndEffect;

	bool m_bNoExplosionEffectWithChar;

	ioHashString m_ExplosionSound;
	ioHashString m_WoundedSound;
	ioHashString m_DefensedSound;
	ioHashString m_LoopSound;

	CEncrypt<float> m_fMoveSpeed;
	CEncrypt<float> m_fMoveRange;
	float m_fFloatPower;
	DWORD m_dwWeaponFeature;
	
	float m_fMapCollisionBoxRate;
	float m_fScaleRate;

	CEncrypt<int> m_iBlockingRate;
	CEncrypt<int> m_iNpcBlockingRate;
	int m_iNpcBlockingType;
	CEncrypt<int> m_iGangsiBlockingRate;
	CEncrypt<int> m_iFCBlockingRate;

	DWORD m_dwCollisionDelay;
	DWORD m_dwPiercingLiveTime;

	DWORD m_dwRefreshDuration;
	DWORD m_dwIgnoreDuration;

	BuffInfoList m_TargetAirBuffList;
	BuffInfoList m_TargetBuffList;
	BuffInfoList m_TargetDownBuffList;
	BuffInfoList m_TargetDefenseBuffList;

	// 공격, 대시 2020-02-19
	BuffInfoList	m_TargetAttackBuffList;
	bool			m_bTargetAttackBuffActive;
	BuffInfoList	m_TargetDashBuffList;
	bool			m_bTargetDashBuffActive;

	// increase buff
	ioHashString m_DamageIncreaseBuff;
	ioHashString m_PushIncreaseBuff;
	ioHashString m_BlowIncreaseBuff;

	ioHashString m_AreaWeaponName;

	bool m_bDefenseBreakBuffEnable;
	ioBuffInfo m_DefenseBreakBuff;

	bool m_bSelectCollisionBuffEnable;
	bool m_bSelectCollisionBuffEnableReverse;

	// werewolf
	CEncrypt<float> m_fWereWolfChangeDamage;

	bool m_bEnableSylphid;
	int m_iSylphidNeed;
	WeaponExtraGaugeType m_WeaponExtraGaugeType;
	bool m_bEnablePossession;

	// for last apply weapon
	DWORD m_dwIgnoreWeaponTime;

	//송기운 요청( 데스나이트 )
	DWORD m_dwTypeProtectedTime;
	DWORD m_dwProtectedType;

	// 잡힌상태의 대상이 존재하면 그 대상만 공격하기 위한 옵션
	bool m_bUseGrapplingTarget;

	// defense cancel
	ioHashString m_DefenseCancelAni;
	float m_fDefenseCancelAniRate;

	WeaponAttribute()
	{
		m_ElementType  = ET_NONE;
		m_AttackMethodType = AMT_NONE;

		m_dwWeaponFeature = 0;
		m_iShakeCamera = 0;

		m_fPushAngle = 0.0f;

		m_fMoveSpeed = 0.0f;
		m_fMoveRange = 0.0f;
		m_fFloatPower = 0.0f;
		m_dwCollisionDelay = 0;
		m_dwPiercingLiveTime = 0;

		m_dwRefreshDuration = 0;
		m_dwIgnoreDuration = 0;

		m_fMapCollisionBoxRate = FLOAT1;
		m_fScaleRate = 0.0f;

		m_fDownStateBlowPowerRate = FLOAT1;
		m_fDownStatePushPowerRate = FLOAT1;

		m_bDefenseBreakBuffEnable = false;
		m_bSelectCollisionBuffEnable = false;

		m_bEnableSylphid = false;
		m_bEnablePossession = false;

		m_WeaponExtraGaugeType = WGT_NONE;

		m_dwIgnoreWeaponTime = 0;

		m_dwTypeProtectedTime = 0;
		m_dwProtectedType = 0;

		m_fDefenseDamageRate = FLOAT1;
		m_fDefenseDamageRate2 = FLOAT1;

		m_bUseGrapplingTarget = false;

		m_bNoCalcFloatingCnt = false;

		m_fDefenseCancelAniRate = FLOAT1;
	}
};

typedef std::vector< WeaponAttribute > WeaponAttributeList;

