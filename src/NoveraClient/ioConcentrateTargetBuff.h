#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioConcentrateTargetBuff : public ioBuff
{
protected:
	CEncrypt<bool> m_bDisableDownState;
	CEncrypt<bool> m_bDisableDownStateByConcentrateAttacker;
	CEncrypt<bool> m_bDisableDropDownState;

	ioHashString m_AttackerName;
	ioHashString m_TargetName;

	ioHashString m_AttackerEffectRed;
	ioHashString m_AttackerEffectBlue;
	UniqueObjID  m_AttackerEffectID;

	ioHashString m_TargetEffectRed;
	ioHashString m_TargetEffectBlue;
	UniqueObjID  m_TargetEffectID;

	CEncrypt<float> m_fPushPowerRateByTarget;
	CEncrypt<float> m_fAirPushPowerRateByTarget;
	CEncrypt<float> m_fDownPushPowerRateByTarget;
	CEncrypt<float> m_fBlowPowerRateByTarget;
	CEncrypt<float> m_fAirBlowPowerRateByTarget;
	CEncrypt<float> m_fDownBlowPowerRateByTarget;

	CEncrypt<float> m_fPushPowerRateByNoTarget;
	CEncrypt<float> m_fAirPushPowerRateByNoTarget;
	CEncrypt<float> m_fDownPushPowerRateByNoTarget;
	CEncrypt<float> m_fBlowPowerRateByNoTarget;
	CEncrypt<float> m_fAirBlowPowerRateByNoTarget;
	CEncrypt<float> m_fDownBlowPowerRateByNoTarget;

	CEncrypt<float> m_fTargetDamageRate;
	CEncrypt<float> m_fNoTargetDamageRate;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

	virtual void GetBuffInfo( SP2Packet &rkPacket );
	virtual void ApplyBuffInfo( SP2Packet &rkPacket );

protected:
	bool CheckOwnerStateCheck();
	bool CheckRestoreStateCheck();

public:
	void SetConcentrateCharName( const ioHashString &szAttackerName, const ioHashString &szTargetName );
	void RemoveConcentrateChar();
	float GetPushPowerRateByTarget( ioBaseChar *pOwner, ioBaseChar *pTarget );
	float GetBlowPowerRateByTarget( ioBaseChar *pOwner, ioBaseChar *pTarget );

	inline const ioHashString& GetConcentrateAttackerName() { return m_AttackerName; }
	inline const ioHashString& GetConcentrateTargetName() { return m_TargetName; }

	inline const float GetTargetDamageRate() { return m_fTargetDamageRate; }
	inline const float GetNoTargetDamageRate() { return m_fNoTargetDamageRate; }

public:
	ioConcentrateTargetBuff();
	ioConcentrateTargetBuff( const ioConcentrateTargetBuff &rhs );
	virtual ~ioConcentrateTargetBuff();
};


inline ioConcentrateTargetBuff* ToConcentrateTargetBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_CONCENTRATE_TARGET )
		return NULL;

	return dynamic_cast< ioConcentrateTargetBuff* >( pBuff );
}