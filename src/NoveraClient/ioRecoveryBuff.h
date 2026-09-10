#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioRecoveryBuff : public ioBuff
{
protected:
	ioHashString m_StartEffect;
	CEncrypt<float> m_fRecoveryHP;
	CEncrypt<DWORD> m_dwRecoveryTick;

	CEncrypt<float> m_fCurRecoveryHP;
	float m_fExtraHPRecoveryValue;

protected:
	CEncrypt<DWORD> m_dwTickGap;
	CEncrypt<bool>  m_bNowRecovery;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

	virtual bool IsLiveButStopped() const;

	virtual void GetBuffInfo( SP2Packet &rkPacket );
	virtual void ApplyBuffInfo( SP2Packet &rkPacket );

protected:
	void CheckOwnerState();

public:
	ioRecoveryBuff();
	ioRecoveryBuff( const ioRecoveryBuff &rhs );
	virtual ~ioRecoveryBuff();
};

inline ioRecoveryBuff* ToRecoveryBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_RECOVERY )
		return NULL;

	return dynamic_cast< ioRecoveryBuff* >( pBuff );
}

