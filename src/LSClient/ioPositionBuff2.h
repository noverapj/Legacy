#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioPositionBuff2 : public ioBuff
{
protected:
	bool m_bDisableDownState;
	bool m_bUseWeaponEquipMesh;
	CEncrypt<bool> m_bEscapeCatchState;

	//헤당 버프를 나 죽을때 같이 죽여 주세요~
	//추후 봐서 ioBuff쪽으로 옮기자.
	ioHashString m_stEraseBuff;

	int m_nFirstDummyID;

	ioHashString m_stControlBuff;

	int		m_nTotalAllowCnt;
	bool	m_bSafeCreate;

	ioHashString m_CheckBuffName;
	ioHashString m_ExceptCallBuffName;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

	void SetSafeCreate( bool b );

protected:
	virtual void SetAction();

	void CheckBuffCnt();

	void EndCall();

public:
	ioPositionBuff2();
	ioPositionBuff2( const ioPositionBuff2 &rhs );
	virtual ~ioPositionBuff2();
};


inline ioPositionBuff2* ToPositionBuff2( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_POSITION2 )
		return NULL;

	return dynamic_cast< ioPositionBuff2* >( pBuff );
}


