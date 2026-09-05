#pragma once


#include "ioWeapon.h"

class ioEffect;

class ioOwnerAttackWeapon : public ioWeapon
{
public:
	enum OwnerAttDirType
	{
		OADT_NONE,
		OADT_TARGET,
		OADT_REVERSE_TARGET,
		OADT_MOVE_DIR,
	};

protected:
	ioHashString m_TargetName;
	ioOrientBox m_LocalAttackBox;

	bool m_bUseAttackDir;
	bool m_bUseReverseAttackDir;

	OwnerAttDirType m_AttackDirType;

	D3DXVECTOR3 m_vMoveDir;

public:
	static ActionStopType m_ActionStopType;
	static ActionStopType m_DieActionStopType;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );

	virtual void SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage );
	virtual void Process( float fTimePerSec, ioPlayStage *pStage );

	virtual const ioOrientBox& GetLocalAttackBox() const;
	virtual WeaponType GetType() const;
	virtual ActionStopType GetActionStopType() const;
	virtual ActionStopType GetDieActionStopType() const;

	virtual D3DXVECTOR3 GetAttackDir( const ioPlayEntity *pEntity ) const;

	virtual bool IsCollision( ioPlayEntity *pEntity, bool bMapCollision=false );

	virtual void SetMoveDir( const D3DXVECTOR3 &vMoveDir );
	virtual D3DXVECTOR3 GetMoveDir();

public:
	void SetTargetName( const ioHashString& szTarget, ioPlayStage *pStage );

public:
	ioOwnerAttackWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioOwnerAttackWeapon();
};

inline ioOwnerAttackWeapon* ToOwnerAttackWeapon( ioWeapon *pWeapon )
{
	if( !pWeapon || pWeapon->GetType() != ioWeapon::WT_OWNER_ATTACK )
		return NULL;

	return dynamic_cast< ioOwnerAttackWeapon* >( pWeapon );
}

