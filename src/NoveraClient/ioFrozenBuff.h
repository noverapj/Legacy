#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioFrozenBuff : public ioBuff
{
public:
	enum FrozenType
	{
		FZT_NONE,
		FZT_EXCEPT,
	};

protected:
	DWORD m_dwAniEndTime;
	bool m_bSetEffect;
	bool m_bNotFrozen;

	FrozenType m_FrozenType;

	DWORD m_dwMaxTime;
	DWORD m_dwMinTime;
	DWORD m_dwMaxTimeByNpc;
	DWORD m_dwMinTimeByNpc;
	DWORD m_dwMaxTimeByBossNpc;
	DWORD m_dwMinTimeByBossNpc;

	DWORD m_dwTeamMaxTime;
	DWORD m_dwTeamMinTime;
	DWORD m_dwTeamMaxTimeByNpc;
	DWORD m_dwTeamMinTimeByNpc;
	DWORD m_dwTeamMaxTimeByBossNpc;
	DWORD m_dwTeamMinTimeByBossNpc;

	DWORD m_dwExtraFrozenDuration;

	float m_fDecreaseRate;

	bool m_bRseveSetFrozen;

public:
	virtual ioBuff* Clone();
	virtual void LoadProperty( ioINILoader &rkLoader );

	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );

	virtual void GetBuffInfo( SP2Packet &rkPacket );
	virtual void ApplyBuffInfo( SP2Packet &rkPacket );

public:
	inline FrozenType GetFrozenType() const { return m_FrozenType; }

protected:
	DWORD GetMaxTime();
	DWORD GetMinTime();
	DWORD GetTeamMaxTime();
	DWORD GetTeamMinTime();

public:
	ioFrozenBuff();
	ioFrozenBuff( const ioFrozenBuff &rhs );
	virtual ~ioFrozenBuff();
};

inline const ioFrozenBuff* ToFrozenConst( const ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_FROZEN_STATUE )
		return NULL;

	return dynamic_cast< const ioFrozenBuff* >( pBuff );
}
