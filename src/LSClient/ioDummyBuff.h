#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioDummyBuff : public ioBuff
{
protected:
	CEncrypt<bool>	m_bDisableDownState;
	CEncrypt<bool>	m_bDisableDropDownState;
	CEncrypt<bool>	m_bUseWeaponEquipMesh;
	CEncrypt<bool>	m_bEscapeCatchState;
	CEncrypt<bool>	m_bDeleteSameBuff;

	DWORD			m_dwWeaponDeadIndex;

protected:
	bool			m_bEnableOutLine;

	float			m_fOutLineR;
	float			m_fOutLineG;
	float			m_fOutLineB;
	float			m_fOutLineAlpha;
	float			m_fOutLineThickness;

protected:
	CEncrypt<int> m_iMaxHitCount;
	int           m_iCurHitCount;

protected:
	ioHashStringVec m_RemoveAreaWeaponList;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

public:
	void SetWeaponDeadIndex( DWORD dwWeaponDeadIndex ){ m_dwWeaponDeadIndex = dwWeaponDeadIndex; }
	void IncreaseHitCount();

protected:
	bool CheckOwnerStateCheck();	
	bool CheckRestoreStateCheck();

protected:
	void LoadRemoveAreaWeaponList( ioINILoader &rkLoader );
	void RemoveAreaWeapon( ioBaseChar *pOwner );

public:
	ioDummyBuff();
	ioDummyBuff( const ioDummyBuff &rhs );
	virtual ~ioDummyBuff();
};


inline ioDummyBuff* ToDummyBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_DUMMY_BUFF )
		return NULL;

	return dynamic_cast< ioDummyBuff* >( pBuff );
}


