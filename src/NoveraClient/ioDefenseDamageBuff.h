#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioDefenseDamageBuff : public ioBuff
{
public:
	enum DefenseType
	{
		DT_SKILL_GAUGE	= 1,
	};

protected:
	float m_fOutLineR;
	float m_fOutLineG;
	float m_fOutLineB;
	float m_fOutLineAlpha;
	float m_fOutLineThickness;
	bool m_bEnableOutLine;

	DefenseType m_DefenseType;
	CEncrypt<float> m_fDefenseMaxRate;
	bool m_bDisableDownState;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );

	float GetDefenseRate();

protected:
	virtual void SetAction();
	virtual void ProcessDelay( float fTimePerSec );

	bool CheckOwnerStateCheck();

public:
	ioDefenseDamageBuff();
	ioDefenseDamageBuff( const ioDefenseDamageBuff &rhs );
	virtual ~ioDefenseDamageBuff();

};

inline ioDefenseDamageBuff* ToDefenseDamageBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_DEFENSE_DAMAGE )
		return NULL;

	return dynamic_cast< ioDefenseDamageBuff* >( pBuff );
}

