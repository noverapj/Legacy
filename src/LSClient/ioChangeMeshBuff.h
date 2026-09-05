#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioChangeMeshBuff : public ioBuff
{
protected:
	CEncrypt<bool> m_bDisableDownState;
	CEncrypt<bool> m_bDisableDropDownState;
	bool m_bUseWeaponEquipMesh;
	CEncrypt<bool>	m_bInvisibleCharMesh;
	ioHashStringVec m_vChangeMeshList;
	ioHashStringVec m_vChangeMeshListW;
	bool	m_bMaintainHideFaceMesh;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

	bool	GetMaintainHideFaceMeshOption();

protected:
	bool CheckOwnerStateCheck();
	bool CheckRestoreStateCheck();

public:
	ioChangeMeshBuff();
	ioChangeMeshBuff( const ioChangeMeshBuff &rhs );
	virtual ~ioChangeMeshBuff();
};


inline ioChangeMeshBuff* ToChangeMeshBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_CHANGE_MESH )
		return NULL;

	return dynamic_cast< ioChangeMeshBuff* >( pBuff );
}


