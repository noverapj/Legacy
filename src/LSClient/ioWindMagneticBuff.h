#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioWindMagneticBuff : public ioBuff
{
public:
	enum PullEndType
	{
		PET_NORMAL	= 1,
		PET_EXTEND	= 2,
	};

	enum BuffState
	{
		BS_NONE,
		BS_PULLING,
		BS_PULL_END,
		BS_PULL_EXTEND,
		BS_PULL_EXTEND_END,
		BS_END,
	};

protected:
	CEncrypt<float> m_fPullRange;
	CEncrypt<float> m_fPullMinRange;
	CEncrypt<float> m_fPullSpeed;

	CEncrypt<float> m_fTargetOffsetFr;
	CEncrypt<float> m_fTargetOffsetHe;
	CEncrypt<float> m_fTargetOffsetSi;

	ioHashString m_szPullAni;
	ioHashString m_szPullExtendAni;

	PullEndType m_PullEndType;
	BuffState m_BuffState;
	bool	  m_bNotUseOwnerBuff;

	WeaponInfo m_PullEndWeaponInfo;

	ioHashString m_szPullControlBuff;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );

public:
	virtual bool IsCharCollisionSkip() const;

	virtual bool IsCollisionAvailableWeapon() const;

protected:
	void SetPullingState();
	void SetPullEndState();	
	void SetPullExtendState();
	void SetPullExtendEndState();
	void SetEndState();

	bool CheckBuffState( float fTimePerSec );
	bool CheckMagneticRange();
	void ProcessBuffMove( float fTimePerSec );

	bool CheckValidateTarget();

public:
	ioWindMagneticBuff();
	ioWindMagneticBuff( const ioWindMagneticBuff &rhs );
	virtual ~ioWindMagneticBuff();

};

inline ioWindMagneticBuff* ToWindMagneticBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_WIND_MAGNETIC )
		return NULL;

	return dynamic_cast< ioWindMagneticBuff* >( pBuff );
}

