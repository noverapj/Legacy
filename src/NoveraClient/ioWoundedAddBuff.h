#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioWoundedAddBuff : public ioBuff
{
protected:
	bool m_bEnableOutLine;
	float m_fOutLineR;
	float m_fOutLineG;
	float m_fOutLineB;
	float m_fOutLineAlpha;
	float m_fOutLineThickness;

	ioHashStringVec m_szAddBuffList;
	ioHashStringVec m_szRemoveBuffList;
	ioHashString m_szWoundEffect;

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
	void LoadAddBuffList( ioINILoader &rkLoader );
	void LoadRemoveBuffList( ioINILoader &rkLoader );
	void Init();
	void RemoveBuff( ioBaseChar *pOwner );

public:
	void SetAddBuff( ioBaseChar *pAttacker );
		
public:
	ioWoundedAddBuff();
	ioWoundedAddBuff( const ioWoundedAddBuff &rhs );
	virtual ~ioWoundedAddBuff();
};

inline ioWoundedAddBuff* ToWoundedAddBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_WOUNDED_ADD_BUFF )
		return NULL;

	return dynamic_cast< ioWoundedAddBuff* >( pBuff );
}


