#pragma once

#include "ioNormalSkill.h"
#include "ioUserKeyInput.h"
#include "ioAttackAttribute.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioTargetMissileSkill : public ioNormalSkill
{
public:
	enum SkillState
	{
		SS_PREDELAY,
		SS_NORMAL,
		SS_AIM_FIRE,
		SS_ZERO_AIM_FIRE,
		SS_END,
	};

	enum SkillSync
	{
		SSC_AIM_FIRE,
		SSC_ZERO_AIM_FIRE,
	};

	enum CheckAttack
	{
		CA_NONE,
		CA_AIM_FIRE,
		CA_ZERO_AIM_FIRE,		
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
	ioHashString	m_szZeroAimFireMotion;
	CEncrypt<float>	m_fZeroAimFireAniTimeRate;
	CEncrypt<DWORD>	m_dwZeroAimFireEndDelay;
	ioHashString	m_szZeroAimLoopMotion;
	CEncrypt<float>	m_fZeroAimLoopMotionRate;
	CEncrypt<DWORD>	m_dwZeroAimLoopDuration;
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
	CEncrypt<DWORD>	m_dwZeroAimDuration;
	DWORD			m_dwAimStartTime;
	DWORD			m_dwZeroAimStartTime;

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
	WeaponInfo		m_AimWeaponInfo;
	WeaponInfo		m_ZeroAimWeaponInfo;
	WeaponInfo		m_FinishWeaponInfo;
	int				m_iCurFireWeapon;

	// 발사간격
	CEncrypt<DWORD>	m_dwFireGapTime;
	DWORD			m_dwZeroAimedFireTime;

	bool m_bSetMoveAni;

	DWORD	m_dwActionStartTime;
	DWORD	m_dwSkillProtectTime;
	DWORD	m_dwCurSkillProtectTime;

	bool		m_bDirection;

protected:
	void ClearData();

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

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

protected:
	void SetNormalState( ioBaseChar *pChar );
	void SetAimFireState( ioBaseChar *pChar );
	void SetZeroAimFireState( ioBaseChar *pChar );
	void SetEndState( ioBaseChar *pChar );
	void SetLoopMotion( ioBaseChar *pChar );

	void ProcessAimFireState( ioBaseChar *pChar );
	void ProcessZeroAimFireState( ioBaseChar *pChar );	

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

public:
	ioTargetMissileSkill();
	ioTargetMissileSkill( const ioTargetMissileSkill &rhs );
	virtual ~ioTargetMissileSkill();
};

inline ioTargetMissileSkill* ToTargetMissileSkill( ioSkill *pSkill )
{
	ioNormalSkill *pNormal = ToNormalSkill( pSkill );
	if( !pNormal || pNormal->GetSubSkillType() != NST_TARGET_MISSILE )
		return NULL;

	return dynamic_cast< ioTargetMissileSkill* >( pSkill );
}

