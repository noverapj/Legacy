#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioFloatBuff2 : public ioBuff
{
protected:
	ioHashString m_FloatAnimation;
	float m_fMaxFloatHeight;
	float m_fFloatSpeed;

	bool m_bDisableDownState;
	bool m_bNoChangeState;

	ioHashStringVec m_OwnerBuffList;

	bool m_bNoCancel;
	bool m_bSetEndJump;
	float m_fEndJumpPower;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

protected:
	void LoadOwnerBuffList( ioINILoader &rkLoader );
	void SetOwnerBuffList( ioBaseChar *pOwner );

protected:
	void SetFloatState( ioBaseChar *pOwner );
	bool CheckFloat( ioBaseChar *pOwner );
	bool CheckOwnerStateCheck( ioBaseChar *pOwner );
	void ProcessEnd( ioBaseChar *pOwner );

public:
	bool IsNoCancel();

public:
	ioFloatBuff2();
	ioFloatBuff2( const ioFloatBuff2 &rhs );
	virtual ~ioFloatBuff2();
};

inline ioFloatBuff2* ToFloatBuff2( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_FLOAT2 )
		return NULL;

	return dynamic_cast< ioFloatBuff2* >( pBuff );
}