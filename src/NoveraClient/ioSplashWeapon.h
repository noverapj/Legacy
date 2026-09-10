#pragma once

#include "ioWeapon.h"

class ioEntityGroup;
class ioPlayStage;

class ioSplashWeapon : public ioWeapon
{
protected:
	ioHashString m_SplashEffect;
	float m_fSplashScale_w;
	float m_fSplashScale_h;
	bool m_bSplash;

public:
	static ActionStopType m_ActionStopType;
	static ActionStopType m_DieActionStopType;

public:
	void SetSplashEffect( const ioHashString &rkSplash );

public:
	virtual void Process( float fTimePerSec, ioPlayStage *pStage );

public:
	virtual void LoadProperty( ioINILoader &rkLoader );

	virtual WeaponType GetType() const;
	virtual ActionStopType GetActionStopType() const;
	virtual ActionStopType GetDieActionStopType() const;

protected:
	virtual void UpdateWorldAttackBox( float fTimePerSec, bool bSetNull );

public:
	ioSplashWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioSplashWeapon();
};

inline ioSplashWeapon* ToSplashWeapon( ioWeapon *pWeapon )
{
	if( !pWeapon || pWeapon->GetType() != ioWeapon::WT_SPLASH )
		return NULL;

	return dynamic_cast< ioSplashWeapon* >( pWeapon );
}

