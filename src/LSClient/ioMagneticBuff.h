#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioMagneticBuff : public ioBuff
{
public:
	enum BuffState
	{
		BS_ENABLE,	// 밀림 가능 상태
		BS_DELAY,	// 밀림 불가능 상태
	};

	enum Magnet
	{
		MAGNET_NONE,
		MAGNET_N,
		MAGNET_S,
	};

	enum PushState
	{
		PS_PUSH,
		PS_PULL,
	};

protected:
	BuffState m_BuffState;
	Magnet    m_Magnet;

	// Push
	CEncrypt<float> m_fPushFriction;
	CEncrypt<bool>  m_bFrictionLinear;
	CEncrypt<float> m_fPushPower;

	DWORD m_dwLastPushTime;
	DWORD m_dwCurDelayTime;

	CEncrypt<DWORD> m_dwMagneticRefreshTime;

	CEncrypt<float> m_fMagneticRange;

	ioHashString m_PushAnimation;
	CEncrypt<float> m_fPushAniRate;
	CEncrypt<DWORD> m_dwPushDelayTime;
	ioHashString m_TargetBuffOnPush;

	ioHashString m_PullAnimation;
	CEncrypt<float> m_fPullAniRate;
	CEncrypt<DWORD> m_dwPullDelayTime;
	ioHashString m_TargetBuffOnPull;

	ioHashString m_EffectN;
	ioHashString m_EffectS;

	int m_iEtcStateCnt;

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
	bool CheckRestoreStateCheck();
	bool CheckOwnerState();
	void ProcessBuffState();

public:
	bool EnableMagnetic();
	void SetPush( D3DXVECTOR3 vPushDir, int iPushState );
	void SetMotion( int iPushState );
	int GetMagnet();

protected:
	bool IsOwnerBuff();

public:
	ioMagneticBuff();
	ioMagneticBuff( const ioMagneticBuff &rhs );
	virtual ~ioMagneticBuff();
};


inline ioMagneticBuff* ToMagneticBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_MAGNETIC )
		return NULL;

	return dynamic_cast< ioMagneticBuff* >( pBuff );
}