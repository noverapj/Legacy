#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioExtraCallBuff : public ioBuff
{
protected:
	bool m_bDisableDownState;
	bool m_bUseWeaponEquipMesh;
	CEncrypt<bool> m_bEscapeCatchState;

protected:
	CEncrypt<bool> m_bReserveExtraCall;

	ioHashStringVec m_ExtraCallBuffList;
	WeaponInfoList m_ExtraCallWeaponInfoList;

	CEncrypt<DWORD> m_dwCheckCallBuffTime;
	CEncrypt<DWORD> m_dwEnableCallBuffTime;
	
	CEncrypt<DWORD> m_dwCheckCallAttackTime;
	CEncrypt<DWORD> m_dwEnableCallAttackTime;

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

	void CheckExtraCall();
	void CheckExtraCallBuff();
	void CheckExtraCallAttack();

public:
	void SetReserveExtraCall();

public:
	ioExtraCallBuff();
	ioExtraCallBuff( const ioExtraCallBuff &rhs );
	virtual ~ioExtraCallBuff();
};


inline ioExtraCallBuff* ToExtraCallBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_EXTRA_CALL )
		return NULL;

	return dynamic_cast< ioExtraCallBuff* >( pBuff );
}


