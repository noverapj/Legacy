#pragma once

#include "ioNormalSkill.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioTargetMissile2Skill : public ioNormalSkill
{
public:
	enum SkillState
	{
		SS_DELAY,
		SS_NORMAL,
		SS_AIM_FIRE,
		SS_LOOP,
		SS_CHARGE_AIM_FIRE,
		SS_END,
	};

	enum SkillSync
	{
		SSC_AIM_FIRE,
		SSC_LOOP,
		SSC_KEY,
		SSC_CHARGE_AIM_FIRE,
	};

	enum CheckAttack
	{
		CA_NONE,
		CA_AIM_FIRE,
		CA_LOOP,
	};

	struct MissileInfo
	{
		float fAngle;
		float fRangeOffset;
		float fHeightOffset;
		D3DXVECTOR3	vPosition;

		MissileInfo()
		{
			fAngle = 0.0f;
			fRangeOffset = 0.0f;
			fHeightOffset = 0.0f;
			vPosition = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
		}
	};

protected:
	SkillState	m_SkillState;

	ioHashString	m_szTargetName;
	ioHashString	m_szNormalDelayAnimation;
	ioHashString	m_szNormalMoveAnimation;

	// 공격모션
	CEncrypt<DWORD>	m_dwSkillEndDelay;

	// 조준공격
	ioHashString	m_szChargeAimFireMotion;
	CEncrypt<float>	m_fChargeAimFireAniTimeRate;
	CEncrypt<DWORD>	m_dwChargeAimFireEndDelay;
	ioHashString	m_szChargeAimLoopMotion;
	CEncrypt<float>	m_fChargeAimLoopMotionRate;
	CEncrypt<DWORD>	m_dwChargeAimLoopDuration;
	CEncrypt<DWORD>	m_dwLoopProtectTime;
	DWORD			m_dwLoopStartTime;
	DWORD			m_dwProtectEndTime;

	// 조준마크
	ioHashString	m_szGuideMarker;
	ioHashString	m_szAimMarker;
	ioHashString	m_szZeroAimMarker;

	DWORD			m_dwGuideMakerID;
	DWORD			m_dwAimMarkerID;
	DWORD			m_dwZeroAimMarkerID;

	CEncrypt<DWORD>	m_dwAimDuration;
	CEncrypt<DWORD>	m_dwChargeDuration;
	DWORD			m_dwAimStartTime;
	DWORD			m_dwAimedFireTime;
	DWORD			m_dwChargeAimStartTime;

	TargetWoundType m_TargetWoundType;
	CEncrypt<float>	m_fTargetAngle;
	CEncrypt<float>	m_fTargetRange;
	CEncrypt<float>	m_fTargetMinRange;

	// 스킬 게이지 감소
	CEncrypt<bool>	m_bDisableGrowth;
	CEncrypt<float>	m_fGuagePerTic;
	CEncrypt<DWORD>	m_dwTicTime;
	DWORD			m_dwCheckTime;
	CEncrypt<float> m_fTicGaugeRateByGrowth;
	CEncrypt<DWORD> m_dwTicTimeByGrowth;

	// 발사체 배치
	ioHashString				m_szWeaponEffect;
	DWORDVec					m_WeaponEffectID;
	CEncrypt<int>				m_iMissileCount;
	std::vector<MissileInfo>	m_MissileInfoList;
	CEncrypt<int>				m_iSelectBaseBullut;
	float						m_fRadiansForHeight;
	CEncrypt<DWORD>				m_dwUpDownSpeed;
	CEncrypt<float>				m_fUpDownOffset;

	// 발사체 정보
	WeaponInfo		m_NoAimWeaponInfo;
	WeaponInfo		m_AimWeaponInfo;	
	WeaponInfo		m_AimChargeWeaponInfo;
	int				m_iCurFireWeapon;

	// 발사간격
	CEncrypt<DWORD>	m_dwFireGapTime;
	DWORD			m_dwChargeAimedFireTime;

	bool m_bSetMoveAni;

	DWORD	m_dwSkillProtectTime;
	DWORD	m_dwCurSkillProtectTime;

	bool m_bDirection;
	bool m_bFire;

	DWORD	m_dwStartChargeTime;
	DWORD	m_dwChargeActiveCount;

	ioUserKeyInput::DirKeyInput m_CurDirKey;

	MissileInfo		m_ChargeMissileInfo;
	ioHashString	m_szChargeWeaponEffect;
	UniqueObjID		m_ChargeWeaponEffectID;
	bool m_bCharge;

	BuffInfoList m_OwnerBuffList;
	DWORD m_dwWeaponChargeTime;
	DWORD m_dwChargeGapTime;

	WeaponInfoList m_vChargeWeaponList;
	int m_iChargeWeaponCnt;
	WeaponInfo m_vChargeWeaponInfo;
	bool m_bDamage;
	
	ioHashString	m_szNormalLoopAnimation;
	DWORD m_fNomalMotionRate;
	DWORD m_dwNomalLoopDuration;
	DWORD m_dwNomalLoopStartTime;
protected:
	void ClearData();

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();
	void LoadChargeWeaponList( ioINILoader &rkLoader );
	void LoadOwnerBuffList( ioINILoader &rkLoader );
	
public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnSkillEnd( ioBaseChar *pChar );	
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );

	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );

	virtual bool CheckUseItemMesh();

public:
	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool IsEndState() const;
	void SetDamage( bool bDamage ){ m_bDamage = bDamage;}
protected:
	void SetNormalState( ioBaseChar *pChar );
	void SetNormalLoopState( ioBaseChar *pChar );
	void SetAimFireState( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetChargeAimFireState( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetEndState( ioBaseChar *pChar );
	void SetLoopState( ioBaseChar *pChar );

	void ProcessAimFireState( ioBaseChar *pChar );
	void ProcessChargeAimFireState( ioBaseChar *pChar );	

	void CheckNormalMoveAni( ioBaseChar *pChar );
	int CheckInputByAimTime( ioBaseChar *pChar );

	void FindTarget( ioBaseChar *pOwner );
	bool IsEnableTargetState( ioBaseChar *pTarget, bool bCheckTargetWound );

	void CreateGuideMarker( ioBaseChar *pChar );
	void UpdateGuideMarker( ioBaseChar *pChar );
	void DestroyGuideMarker( ioBaseChar *pChar );

	void CreateAimMarker();
	void UpdateAimMarker();
	void DestroyAimMarker();

	void CreateWeaponEffect( ioBaseChar *pChar );
	void UpdateWeaponEffect( ioBaseChar *pChar );
	void DestroyAllWeaponEffect();
	void DestroyWeaponEffect( int WeaponNum );

	D3DXVECTOR3 CalcWeaponEffectPosition( int iCount, ioBaseChar* pChar);

	void CreateChargeWeaponEffect( ioBaseChar *pChar );
	D3DXVECTOR3 CalcChargeWeaponEffectPosition( ioBaseChar* pChar );
	void UpdateChargeWeaponEffect( ioBaseChar *pChar );
	void DestroyChargeWeaponEffect();
	void CheckKeyInput( ioBaseChar *pChar );
	void AddOwnerBuff( ioBaseChar *pChar );
	void RemoveOwnerBuff( ioBaseChar *pChar );
	void CheckChargeWeaponTime( ioBaseChar *pChar );
	void CheckDamage( ioBaseChar *pChar );
	void CheckSetNomalAni( ioBaseChar *pChar );
public:
	ioTargetMissile2Skill();
	ioTargetMissile2Skill( const ioTargetMissile2Skill &rhs );
	virtual ~ioTargetMissile2Skill();
};

inline ioTargetMissile2Skill* ToTargetMissile2Skill( ioSkill *pSkill )
{
	ioNormalSkill *pNormal = ToNormalSkill( pSkill );
	if( !pNormal || pNormal->GetSubSkillType() != NST_TARGET_MISSILE2 )
		return NULL;

	return dynamic_cast< ioTargetMissile2Skill* >( pSkill );
}
