#pragma once
#include "iobuff.h"
class ioCreateDummyByAttackedBuff2 :
	public ioBuff
{
protected:
	vDummyCharLoadInfoList m_DummyList;
	ioHashStringVec m_vDestroyDummyList;

	int				m_iCheckPercent;			//1000´ÜÀ§( 100%¸é 1000 )
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ApplyExtraBuffInfo( SP2Packet &rkPacket );
	virtual void ProcessBuff( float fTimePerSec );
public:
	void OwnerCharApplyWeapon();
	void CreateDummyChar( ioBaseChar *pOwner );
public:
	ioCreateDummyByAttackedBuff2(void);
	ioCreateDummyByAttackedBuff2( const ioCreateDummyByAttackedBuff2 &rhs );
	virtual ~ioCreateDummyByAttackedBuff2(void);
};

inline ioCreateDummyByAttackedBuff2* ToCreateDummyByAttackedBuff2( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_CREATE_DUMMY_BY_ATTACKED_BUFF2 )
		return NULL;

	return dynamic_cast< ioCreateDummyByAttackedBuff2* >( pBuff );
}