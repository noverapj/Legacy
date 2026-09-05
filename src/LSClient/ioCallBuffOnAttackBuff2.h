#pragma once


class ioBaseChar;
class ioINILoader;

class ioCallBuffOnAttackBuff2 : public ioBuff
{
protected:
	bool m_bDisableDownState;
	bool m_bUseWeaponEquipMesh;

	ioHashString	m_CallBuffName;

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
	inline const ioHashString& GetCallBuffName() const { return m_CallBuffName; }

public:
	ioCallBuffOnAttackBuff2();
	ioCallBuffOnAttackBuff2( const ioCallBuffOnAttackBuff2 &rhs );
	virtual ~ioCallBuffOnAttackBuff2();
};


inline ioCallBuffOnAttackBuff2* ToCallBuffOnAttackBuff2( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_CALL_BUFF_ON_ATTACK2 )
		return NULL;

	return dynamic_cast< ioCallBuffOnAttackBuff2* >( pBuff );
}


