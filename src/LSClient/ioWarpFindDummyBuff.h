#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioWarpFindDummyBuff : public ioBuff
{
protected:
	enum SyncType
	{
		ST_WARP,
		ST_KEY_WARP,
	};
protected:
	ioHashString m_szFindDummyName;
	ioHashString m_szWarpAni;
	float m_fWarpAniRate;
	DWORD m_dwMotionEndTime;
	bool  m_bTeleportByInputKey;
	ioHashString m_szWarpEffect;
	ioHashString m_szCreatorDummyName;
	float		 m_fEnableTeleportRangeByCreatorDummy;
	ioHashStringVec m_vTeamDisableTeleportBuffList;
	bool m_bBuffEndJump;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );

public:
	virtual void ApplyExtraBuffInfo( SP2Packet &rkPacket );
			
protected:
	bool WarpChar( ioBaseChar *pOwner );
	void SetWarpState( ioBaseChar *pOwner );
	void CheckTeleportByInputKey( ioBaseChar *pOwner );
	void CheckFindDummy( ioBaseChar *pOwner );

public:
	ioWarpFindDummyBuff();
	ioWarpFindDummyBuff( const ioWarpFindDummyBuff &rhs );
	virtual ~ioWarpFindDummyBuff();
};

inline ioWarpFindDummyBuff* ToWarpFindDummyBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_WARP_FIND_DUMMY )
		return NULL;

	return dynamic_cast< ioWarpFindDummyBuff* >( pBuff );
}