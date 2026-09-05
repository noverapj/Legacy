#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioTargetEquipObjectItemBuff : public ioBuff
{
public:
	enum ObjectState
	{
		OE_NONE,
		OE_EQUIP,
		OE_RELEASE,
	};

protected:

	bool		m_bDisableDownState;

	DWORDVec	m_vObjectItemList;
	DWORD		m_dwCurObjectItem;

	bool		m_bTargetEquip;
	ObjectState	m_ObjectState;

	bool		m_bCreatorItemCheck;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );
	virtual void SetExtraDurationByGrowth( float fDuration );

protected:
	virtual void SetAction();
	virtual void ProcessDelay( float fTimePerSec );

	void SetObjectItemEquip( ioPlayStage *pStage, ioBaseChar* pOwner );

	void ProcessActive( float fTimePerSec );

	void CheckOwnerStateCheck();

	void CheckCreatorItem();

public:
	bool ReleaseObject( ioPlayStage *pStage, ioBaseChar* pOwner, bool bSyncCharState = true );

protected:
	void ProcessCheckObject();

public:
	inline void SetReleaseState(){ m_ObjectState = OE_RELEASE; }
	inline void SetEquipState(){ m_ObjectState = OE_EQUIP; }
	inline ObjectState GetEquipState(){ return m_ObjectState; }

public:
	ioTargetEquipObjectItemBuff();
	ioTargetEquipObjectItemBuff( const ioTargetEquipObjectItemBuff &rhs );
	virtual ~ioTargetEquipObjectItemBuff();

};

inline ioTargetEquipObjectItemBuff* ToTargetEquipObjectItemBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_TARGET_EQUIP_OBJECTITEM )
		return NULL;

	return dynamic_cast< ioTargetEquipObjectItemBuff* > ( pBuff );
}

