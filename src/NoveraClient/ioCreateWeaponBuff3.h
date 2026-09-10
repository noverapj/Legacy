#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioCreateWeaponBuff3 : public ioBuff
{
protected:
	enum BuffCheckState
	{
		BS_NONE,
		BS_WAIT,
		BS_FIRE,
	};

protected:
	bool m_bEnableOutLine;
	float m_fOutLineR;
	float m_fOutLineG;
	float m_fOutLineB;
	float m_fOutLineAlpha;
	float m_fOutLineThickness;

	ioHashString m_szWaitEffect;
	DWORD m_dwWaitTime;
	
	float m_fFireHeightOffSet;

	float m_fCheckDist;
	WeaponInfo m_ShortWeaponInfo;
	WeaponInfo m_LongWeaponInfo;

protected:
	BuffCheckState m_BuffState;
	DWORD m_dwCheckWaitTime;
	WeaponInfo m_CurWeaponInfo;

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
	void Init();
	void SetWaitState( ioBaseChar *pOwner );
	void SetFireState( ioBaseChar *pOwner );
	void CreateWeapon();
	
public:
	ioCreateWeaponBuff3();
	ioCreateWeaponBuff3( const ioCreateWeaponBuff3 &rhs );
	virtual ~ioCreateWeaponBuff3();
};

inline ioCreateWeaponBuff3* ToCreateWeaponBuff3( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_CREATE_WEAPON_BUFF3 )
		return NULL;

	return dynamic_cast< ioCreateWeaponBuff3* >( pBuff );
}


