#pragma once


#include "ioWeapon.h"

class ioEffect;

class ioTargetWeapon : public ioWeapon
{
protected:
	ioHashString m_TargetName;
	ioOrientBox m_LocalAttackBox;

public:
	static ActionStopType m_ActionStopType;
	static ActionStopType m_DieActionStopType;

public:
	void SetTargetName( const ioHashString& szTarget, ioPlayStage *pStage );

public:
	virtual void SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage );
	virtual void Process( float fTimePerSec, ioPlayStage *pStage );

	virtual const ioOrientBox& GetLocalAttackBox() const;
	virtual WeaponType GetType() const;
	virtual ActionStopType GetActionStopType() const;
	virtual ActionStopType GetDieActionStopType() const;

	virtual bool IsCollision( ioPlayEntity *pEntity, bool bMapCollision=false );

public:
	ioTargetWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioTargetWeapon();
};

inline ioTargetWeapon* ToTargetWeapon( ioWeapon *pWeapon )
{
	if( !pWeapon || pWeapon->GetType() != ioWeapon::WT_TARGET )
		return NULL;

	return dynamic_cast< ioTargetWeapon* >( pWeapon );
}

