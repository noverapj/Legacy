#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioSelfControlBuff : public ioBuff
{
public:
	enum SelfControlState
	{
		SCS_NONE,
		SCS_CONTROL,
	};

	enum SyncState
	{
		SS_KEY,
	};
protected:
	SelfControlState m_State;
	float m_fCurMoveSpeed;
	ioHashString m_szMoveAni;

	DWORD m_dwMoveRotateTime;

	ioUserKeyInput::DirKeyInput m_CurDirKey;

	float m_fSkillTotalMoveAmount;
	DWORD m_dwWeaponIndex;
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );
	virtual void ApplyExtraBuffInfo( SP2Packet &rkPacket );
public:
	void ProcessBuffAction( float fTimePerSec );
protected:
	void CheckKeyInput( ioBaseChar *pChar );
	void ProcessRotate( ioBaseChar *pChar, float fRotateTime );
	bool CheckWeaponDead( ioBaseChar *pChar );

	void CallingAttribute();
	void CheckCallingAttribute();

public:
	ioSelfControlBuff();
	ioSelfControlBuff( const ioSelfControlBuff &rhs );
	virtual ~ioSelfControlBuff();
};

inline ioSelfControlBuff* ToSelfControlBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_SELF_CONTROL )
		return NULL;

	return dynamic_cast< ioSelfControlBuff* >( pBuff );
}