

#include "stdafx.h"

#include "ioThrowItem.h"
#include "ioBaseChar.h"

#include "WeaponDefine.h"

ioThrowItem::ioThrowItem()
{
}

ioThrowItem::ioThrowItem( const ioThrowItem &rhs )
: ioObjectItem( rhs )
{
}

ioThrowItem::~ioThrowItem()
{
}

void ioThrowItem::LoadProperty( ioINILoader &rkLoader )
{
	ioObjectItem::LoadProperty( rkLoader );
}

ioItem* ioThrowItem::Clone()
{
	return new ioThrowItem( *this );
}

void ioThrowItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	if( !pWeapon ) return;

	ioObjectItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

	ioBaseChar *pOwner = pWeapon->GetOwner();
	if( pOwner )
	{
		D3DXVECTOR3	vMoveDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize(&vMoveDir,&vMoveDir);

		D3DXVECTOR3 vPos = pWeapon->GetPosition() + (vMoveDir * m_fOffSet);
		vPos.y += m_fHeightOffSet;

		pWeapon->SetPosition( vPos );
	}
}

ioObjectItem::ObjectSubType ioThrowItem::GetObjectSubType() const
{
	return OST_THROW;
}

