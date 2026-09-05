#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioCreateObjectItemBuff : public ioBuff
{
public:
	enum ObjectState
	{
		OE_NONE,
		OE_EQUIP,
		OE_RELEASE,
	};

protected:
	ObjectState		m_ObjectState;

	DWORD			m_dwObjectItemCode;

	bool			m_bDisableDownState;
	bool			m_bDisableDropDownState;
	
	ioHashString	m_szObjectCheckBuff;	
	float			m_fEndJumpPower;
	IntVec			m_BuffEndState;
	
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();
	
protected:
	bool CheckOwnerStateCheck();
	bool CheckOwnerObject();

protected:
	void ProcessCheckObject();

protected:
	void SetEquipObjectItem( ioBaseChar *pOwner );

public:
	bool SetReleaseObjectItem( ioBaseChar *pOwner );

public:
	inline void SetReleaseState(){ m_ObjectState = OE_RELEASE; }
	inline void SetEquipState(){ m_ObjectState = OE_EQUIP; }
	inline ObjectState GetEquipState(){ return m_ObjectState; }

public:
	bool IsBuffEndJumpDisable( ioBaseChar *pOwner );

public:
	ioCreateObjectItemBuff();
	ioCreateObjectItemBuff( const ioCreateObjectItemBuff &rhs );
	virtual ~ioCreateObjectItemBuff();
};


inline ioCreateObjectItemBuff* ToCreateObjectItemBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_CREATE_OBJECT_ITEM )
		return NULL;

	return dynamic_cast< ioCreateObjectItemBuff* >( pBuff );
}


