#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioMoveAreaWeaponBuff : public ioBuff
{
protected:
	enum BuffState
	{
		BS_None,
		BS_Teleport,
		BS_End,
	};

protected:
	ioHashString m_szAttachedAni;
	float m_fAttachedAniRate;
	bool m_bDisableDownState;
	ioHashStringVec m_OwnerBuffList;
	bool m_bColSkip;
	ioHashString m_szAreaWeapon;
	BuffState m_BuffState;
	float m_fOffsetHeight;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();
	virtual void ApplyExtraBuffInfo( SP2Packet &rkPacket );

protected:
	void LoadOwnerBuffList( ioINILoader &rkLoader );
	void SetOwnerBuffList( ioBaseChar *pOwner );
	void RemoveOwnerBuffList( ioBaseChar *pOwner );

protected:
	void Init();
	void SetTeleportState( ioBaseChar *pOwner );
	void SetEndState( ioBaseChar *pOwner );

protected:
	bool CheckOwnerStateCheck( ioBaseChar *pOwner );
	bool CheckAreaWeapon( ioBaseChar *pOwner );

public:
	bool IsCharCollisionSkipState();

public:
	ioMoveAreaWeaponBuff();
	ioMoveAreaWeaponBuff( const ioMoveAreaWeaponBuff &rhs );
	virtual ~ioMoveAreaWeaponBuff();
};

inline ioMoveAreaWeaponBuff* ToMoveAreaWeaponBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_MOVE_AREA_WEAPON )
		return NULL;

	return dynamic_cast< ioMoveAreaWeaponBuff* >( pBuff );
}