#pragma once

#include "ioBuff.h"


class ioBaseChar;
class ioINILoader;

class ioTeleportOnEndBuff : public ioBuff
{
protected:
	D3DXVECTOR3		m_vCheckPosition;
	ioHashString	m_TeleportTargetEffect;
	DWORD			m_dwTeleportTargetEffectID;
	ioHashString	m_TeleportStartEffect;
	ioHashString	m_TeleportEndEffect;

	CEncrypt<float>	m_fJumpPower;

	CEncrypt<bool>	m_bBuffEndBuffOnTimeEnd;
	bool			m_bCheckOwnerItem;

	ioHashString	m_BuffEndSound;

	CEncrypt<float>	m_fLimitDistance;
	CEncrypt<bool>	m_bReturnToOverDist;
	bool			m_bCheckOverDist;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );	
	virtual void EndBuff();

	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );

	virtual void GetBuffInfo( SP2Packet &rkPacket );
	virtual void ApplyBuffInfo( SP2Packet &rkPacket );

protected:
	void CheckDistance();

public:
	ioTeleportOnEndBuff();
	ioTeleportOnEndBuff( const ioTeleportOnEndBuff &rhs );
	virtual ~ioTeleportOnEndBuff();
};

