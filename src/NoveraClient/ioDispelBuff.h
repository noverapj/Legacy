#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioDispelBuff : public ioBuff
{
protected:
	bool m_bDisableDownState;
	bool m_bUseWeaponEquipMesh;
	CEncrypt<bool> m_bEscapeCatchState;

	GoodBadType m_DispelType;

protected:
	bool m_bEnableOutLine;

	float m_fOutLineR;
	float m_fOutLineG;
	float m_fOutLineB;
	float m_fOutLineAlpha;
	float m_fOutLineThickness;

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
	bool CheckRestoreStateCheck();

public:
	ioDispelBuff();
	ioDispelBuff( const ioDispelBuff &rhs );
	virtual ~ioDispelBuff();
};

inline ioDispelBuff* ToDispelBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_DISPEL_BUFF )
		return NULL;

	return dynamic_cast< ioDispelBuff* >( pBuff );
}


