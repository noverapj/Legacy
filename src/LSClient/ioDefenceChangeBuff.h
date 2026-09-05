#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioDefenceChangeBuff : public ioBuff
{
protected:
	DWORD			m_dwDefensePreDuration;
	DWORD			m_dwDefenseEndDuration;
	DWORD			m_dwDefenseDuration;
	DWORD			m_dwDefenseDurationEx;
	bool			m_bDefenseBackSide;
	ioHashString	m_DefenseEffect;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

public:
	DWORD GetDefensePreDuration() { return m_dwDefensePreDuration; }
	DWORD GetDefenseEndDuration() { return m_dwDefenseEndDuration; }
	DWORD GetDefenseDuration() { return m_dwDefenseDuration; }
	DWORD GetDefenseDurationEx() { return m_dwDefenseDurationEx; }
	bool IsCanDefenseBackSide() { return m_bDefenseBackSide; }
	const ioHashString& GetDefenseEffect() { return m_DefenseEffect; }

public:
	ioDefenceChangeBuff();
	ioDefenceChangeBuff( const ioDefenceChangeBuff &rhs );
	virtual ~ioDefenceChangeBuff();
};


inline ioDefenceChangeBuff* ToDefenceChangeBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_DEFENCE_CHANGE_BUFF )
		return NULL;

	return dynamic_cast< ioDefenceChangeBuff* >( pBuff );
}

inline const ioDefenceChangeBuff* ToDefenceChangeBuffConst( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_DEFENCE_CHANGE_BUFF )
		return NULL;

	return dynamic_cast< const ioDefenceChangeBuff* >( pBuff );
}


