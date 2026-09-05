#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioLimitActionBuff : public ioBuff
{
protected:
	CEncrypt<bool> m_bLimitAttack;
	CEncrypt<bool> m_bLimitJump;
	CEncrypt<bool> m_bLimitExtendJump;
	CEncrypt<bool> m_bLimitDefense;
	CEncrypt<bool> m_bLimitSkill;
	CEncrypt<bool> m_bLimitDash;

	CEncrypt<bool> m_bEnableOwnerItemSkill;

	bool			m_bDisableDownState;
	bool			m_bDisableDropState;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );

	virtual void EndBuff();
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );

public:
	inline bool IsLimitAttack() const { return m_bLimitAttack; }
	inline bool IsLimitJump() const { return m_bLimitJump; }
	inline bool IsLimitExtendJump() const { return m_bLimitExtendJump; }
	inline bool IsLimitDefense() const { return m_bLimitDefense; }
	inline bool IsLimitSkill() const { return m_bLimitSkill; }
	inline bool IsLimitDash() const { return m_bLimitDash; }

	inline bool EnableOwnerItemSkill() const { return m_bEnableOwnerItemSkill; }

protected:
	void CheckOwnerStateCheck();

public:
	ioLimitActionBuff();
	ioLimitActionBuff( const ioLimitActionBuff &rhs );
	virtual ~ioLimitActionBuff();
};

inline ioLimitActionBuff* ToLimitActionBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_LIMIT_ACTION )
		return NULL;

	return dynamic_cast< ioLimitActionBuff* >( pBuff );
}