#pragma once


#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioPoisoningBuff : public ioBuff
{

protected:
	CEncrypt<float> m_fPoisonPerTick;
	CEncrypt<DWORD> m_dwPoisonTick;
	CEncrypt<DWORD> m_dwTickGap;

	CEncrypt<float> m_fCurDamage;
	CEncrypt<float> m_fExtraPosionValue;

	CEncrypt<bool> m_bUseLimitHP;
	CEncrypt<float> m_fLimitHPRate;

public:
	virtual ioBuff* Clone();
	virtual void LoadProperty( ioINILoader &rkLoader );

public :
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

	virtual void GetBuffInfo( SP2Packet &rkPacket );
	virtual void ApplyBuffInfo( SP2Packet &rkPacket );

public:
	ioPoisoningBuff();
	ioPoisoningBuff( const ioPoisoningBuff &rhs );
	virtual ~ioPoisoningBuff();
};

