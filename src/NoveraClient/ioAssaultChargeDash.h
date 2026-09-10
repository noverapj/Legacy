#pragma once

#include "ioExtendDash.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioBaseChar;
class ioPlayStage;

class ioAssaultChargeDash : public ioExtendDash
{
public:
	enum ChangeDashState
	{
		CDS_PREDELAY,
		CDS_CHARGING,
		CDS_FIRE,
	};

protected:
	ChangeDashState	m_DashState;

	ioUserKeyInput::DirKeyInput	m_CurDirKey;

	//Charging
	bool			m_bOnCharging;
	CEncrypt<float> m_fChargingSpeedRate;
	float			m_fCurChargingRate;
	DWORD			m_dwGatheringStartTime;
	CEncrypt<DWORD>	m_dwMaxChargeTime;

	CEncrypt<float> m_fCurChargeRate;
	CEncrypt<float> m_fCurChargeRateForSkill;

	ioHashString	m_szGatheringEffect;
	ioHashString	m_szMaxGatheringsound;
	DWORD			m_dwGatheringEffectID;

	CEncrypt<float> m_fMaxDamageRate;
	CEncrypt<float> m_fMaxForceRate;
	CEncrypt<float> m_fMaxBlowRate;

	CEncrypt<float> m_fMaxRangeRate;
	CEncrypt<float> m_fMaxSpeedRate;
	CEncrypt<float> m_fMaxChargeAniRate;
	CEncrypt<float> m_fMaxForceSlidingRate;

	CEncrypt<float> m_fMaxTargetRangeRate;
	CEncrypt<float> m_fMaxTargetAngleRate;

	CEncrypt<float> m_fMaxFloatRate;

	CEncrypt<float> m_fMaxScaleRate;
	CEncrypt<float> m_fMaxExplosionRate;
	CEncrypt<float> m_fMaxLiveTimeRate;
	CEncrypt<float> m_fMaxWoundedTimeRate;

	//Delay Animation
	ioHashString	m_szDelayAnimation;
	CEncrypt<float>	m_fDelayAniRate;
	DWORD			m_dwPreDelayEndTime;

	//Loop Animation
	ioHashString	m_szLoopAnimation;
	CEncrypt<float>	m_fLoopAniRate;
	CEncrypt<DWORD>	m_dwLoopDuration;
	DWORD			m_dwLoopStartTime;
	DWORD			m_dwLoopEndTime;

	//Fire Animation
	AttackAttribute m_AttackAttribute;

	DWORD m_dwActionEndTime;

	float m_fOffSet;

	ioHashString m_CircleEffectRed;
	ioHashString m_CircleEffectBlue;

// element att
protected:
	CEncrypt<bool> m_bUseDashElementAtt;
	MagicElementInfo m_MagicElementAtt;
	
protected:
	void ClearData();

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual DashType GetType() const;
	virtual ioExtendDash* Clone();

	virtual void ApplyDash( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual bool StartDash( ioBaseChar *pOwner );
	virtual void ProcessDash( ioBaseChar *pOwner );

public:
	virtual void CheckChargeInfo( ioBaseChar * pOwner, ioWeapon *pWeapon );
	void SetExtraInfoToWeapon( ioBaseChar *pOwner, ioPlayStage *pStage, ioWeapon *pWeapon, const ioHashString &szName );

protected:
	void CheckDirKey( ioBaseChar *pOwner );
	void SetChargingState( ioBaseChar* pOwner );
	void SetFireState( ioBaseChar *pOwner, int iSubType, bool bSend );

	bool CheckElementAttack( ioBaseChar *pOwner );

public:
	const ioHashString GetGatheringEffect(){ return m_szGatheringEffect; }
	DWORD GetGatheringEffectID(){ return m_dwGatheringEffectID; }

public:
	ioAssaultChargeDash();
	ioAssaultChargeDash( const ioAssaultChargeDash &rhs );
	virtual ~ioAssaultChargeDash();
};

inline ioAssaultChargeDash* ToAssaultChargeDash( ioExtendDash *pDash )
{
	if( !pDash || pDash->GetType() != DT_ASSAULT_CHARGE )
		return NULL;

	return dynamic_cast< ioAssaultChargeDash* >( pDash );
}

