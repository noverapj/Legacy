#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioEscapeDropZoneBuff : public ioBuff
{
public:
	enum BuffState
	{
		BS_ON_MAP,
		BS_ON_DROP_ZONE,
		BS_END,
	};

protected:
	BuffState m_BuffState;

	D3DXVECTOR3 m_vLastMapPosition;
	D3DXVECTOR3 m_vCirclePos;

	DWORD m_dwMagicCircleStartTime;

	ioHashString m_RedCircle;
	ioHashString m_BlueCircle;
	ioHashString m_ReadyCircle;
	ioHashString m_EscapeEndBuff;
	ioHashString m_EscapeSound;

	CEncrypt<DWORD> m_dwCircleDuration;
	CEncrypt<float> m_fCircleRange;
	CEncrypt<float> m_fCircleOffset;
	CEncrypt<float> m_fCircleMoveSpeed;
	CEncrypt<float> m_fCircleHeightGap;

	DWORD m_dwCurMapEffect;

	CEncrypt<float> m_fDamageRate;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

public:
	virtual void ApplyExtraBuffInfo( SP2Packet &rkPacket );

public:
	inline float GetSystemDamageRate(){ return m_fDamageRate; }

protected:
	bool CheckRestoreStateCheck();
	void ProcessOnMap();

	void SetEscapeDropZone();
	void ProcessOnDropZone();

	void SetTeleport( ioBaseChar *pOwner );

public:
	ioEscapeDropZoneBuff();
	ioEscapeDropZoneBuff( const ioEscapeDropZoneBuff &rhs );
	virtual ~ioEscapeDropZoneBuff();
};


inline ioEscapeDropZoneBuff* ToEscapeDropZoneBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_ESCAPE_DROP_ZONE )
		return NULL;

	return dynamic_cast< ioEscapeDropZoneBuff* >( pBuff );
}


