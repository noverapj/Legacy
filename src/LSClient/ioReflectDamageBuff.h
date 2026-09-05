#pragma once

#include "ioBuff.h"


class ioBaseChar;
class ioINILoader;

struct ReflectDamageInfo
{
	WeaponInfo m_WeaponInfo;

	float m_fRangeMinDamage;
	float m_fRangeMaxDamage;

	ReflectDamageInfo()
	{
		m_WeaponInfo.Init();

		m_fRangeMinDamage = 0.0f;
		m_fRangeMaxDamage = 0.0f;
	}
};

typedef std::vector<ReflectDamageInfo> ReflectDamageInfoVec;

class ioReflectDamageBuff : public ioBuff
{
protected:
	bool m_bDisableDownState;
	bool m_bAirReflect;
	bool m_bEnableEscapeCatchAttackReflect;
	
	ReflectDamageInfoVec m_ReflectDamageInfoVec;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );	
	virtual void EndBuff();

	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );

public:
	const WeaponInfo& GetReflectAttack( float fDamage );

	inline bool GetAirReflect(){ return m_bAirReflect; }
	inline bool GetEnableEscapeCatchAttackReflect(){ return m_bEnableEscapeCatchAttackReflect; }

protected:
	void CheckOwnerStateCheck();

public:
	ioReflectDamageBuff();
	ioReflectDamageBuff( const ioReflectDamageBuff &rhs );
	virtual ~ioReflectDamageBuff();
};


inline ioReflectDamageBuff* ToReflectDamageBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_REFLECT_DAMAGE )
		return NULL;

	return dynamic_cast< ioReflectDamageBuff* > ( pBuff );
}