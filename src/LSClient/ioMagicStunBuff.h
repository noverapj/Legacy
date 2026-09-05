#pragma once

#include "ioBuff.h"

class ioINILoader;
class ioBaseChar;

class ioMagicStunBuff : public ioBuff
{
protected:
	ioHashString m_StunAnimation;
	ioHashString m_CatchAnimation;

	ioHashStringVec m_AttackSuccess;
	ioHashStringVec m_DefenseSuccess;
	ioHashStringVec m_JumpSuccess;

	ioHashStringVec m_AttackFail;
	ioHashStringVec m_DefenseFail;
	ioHashStringVec m_JumpFail;

	CEncrypt<float> m_fSuccessRate;
	CEncrypt<bool> m_bSuccess;
	CEncrypt<bool> m_bAddBuff;

	CEncrypt<DWORD> m_dwDisableTime;
	CEncrypt<DWORD> m_dwEnableTime;

	ioHashString m_DisableEffect;
	ioHashString m_EnableEffect;

	bool m_bEnableKeyState;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );

protected:
	virtual void SetAction();
	virtual void ProcessDelay( float fTimePerSec );

	bool CheckSuccessFailBuff();
	void SetTimeOverBuff();

public:
	ioMagicStunBuff();
	ioMagicStunBuff( const ioMagicStunBuff &rhs );
	virtual ~ioMagicStunBuff();
};

inline ioMagicStunBuff* ToMagicStunBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_MAGIC_STUN )
		return NULL;

	return dynamic_cast< ioMagicStunBuff* >( pBuff );
}

