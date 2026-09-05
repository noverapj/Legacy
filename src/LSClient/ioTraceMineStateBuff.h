#pragma once

//#include "ioBuff.h"
//

class ioBaseChar;
class ioINILoader;

class ioTraceMineStateBuff : public ioBuff
{

protected:
	
	CEncrypt<int> m_iArmorType;
	CEncrypt<int> m_iMaxMineCount;

	int m_iCurMineCnt;
	
	typedef std::vector<DWORD> MineWeaponList;
	MineWeaponList m_vMineWeaponList;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );	
	virtual void EndBuff();

	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );

public:
	int inline GetOwerItemEquipSlot(){ return m_iArmorType; }
	int GetCurMineCnt();
	int GetMaxMineCnt();

	ioItem* GetBuffOwnerItem();

public:
	ioTraceMineStateBuff();
	ioTraceMineStateBuff( const ioTraceMineStateBuff &rhs );
	virtual ~ioTraceMineStateBuff();
};

inline ioTraceMineStateBuff* ToTraceMineStateBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_TRACE_MINE_STATE )
		return NULL;

	return dynamic_cast< ioTraceMineStateBuff* > ( pBuff );
}


