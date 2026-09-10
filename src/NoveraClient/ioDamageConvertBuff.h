#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioDamageConvertBuff : public ioBuff
{
	enum DamageConvertType
	{
		DCVT_NORMAL,
		DCVT_TYPE1,//Ææ¸®¸£ÀÇ ±Í
	};

	DamageConvertType m_type;

protected:
	CEncrypt<float> m_fWoundedRecoveryGauge;
	CEncrypt<float> m_fWoundedRecoveryRate;

	CEncrypt<float> m_fAttackRecoveryGauge;
	CEncrypt<float> m_fAttackRecoveryRate;

	CEncrypt<float> m_fHPStealRate;
	CEncrypt<float> m_fHPStealByWounded;
	CEncrypt<float> m_fMaxHPStealByWounded;
	CEncrypt<float> m_fTotalHPStealByWounded;

	ioHashString m_HPStealEffect;
	ioHashString m_DamageConvertEffect;

	IntVec m_vAttackTypeList;

	int		m_nGaugeAttackCnt;
	int		m_nCurAttackCnt;
	CEncrypt<float> m_fAbsoluteRecoveryGague;

	bool	m_bCheckElementCheckType;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void StartBuffWithAniTime( ioBaseChar *pOwner, DWORD dwAniTime );
	virtual void ProcessBuff( float fTimePerSec );

	virtual void EndBuff();
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );

protected:
	float GetRecoveryGaugeByAttack();
	float GetRecoveryHPByAttack();
	float GetRecoveryHPByWounded();
	float GetRecoveryGaugeByWounded();

	bool CheckElementType( int iType );

	ioGrowthUpInfo* GetGrowthUpInfoByType( int iType, const ioHashString &szName );

public:
	ioDamageConvertBuff();
	ioDamageConvertBuff( const ioDamageConvertBuff &rhs );
	virtual ~ioDamageConvertBuff();
};

