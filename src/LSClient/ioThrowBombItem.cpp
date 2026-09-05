

#include "stdafx.h"

#include "ioThrowBombItem.h"
#include "ioBaseChar.h"

#include "WeaponDefine.h"

ioThrowBombItem::ioThrowBombItem()
{
	m_bDropBomb = false;
}

ioThrowBombItem::ioThrowBombItem( const ioThrowBombItem &rhs )
: ioObjectItem( rhs ),
  m_EquipedEffect( rhs.m_EquipedEffect )
{
	m_bDropBomb = false;
}

ioThrowBombItem::~ioThrowBombItem()
{
}

void ioThrowBombItem::LoadProperty( ioINILoader &rkLoader )
{
	ioObjectItem::LoadProperty( rkLoader );
	
	char szBuf[MAX_PATH];
	rkLoader.LoadString_e( "equiped_effect", "", szBuf, MAX_PATH );
	m_EquipedEffect = szBuf;
}

ioItem* ioThrowBombItem::Clone()
{
	return new ioThrowBombItem( *this );
}

void ioThrowBombItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	if( !pWeapon ) return;

	ioObjectItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

	ioThrowBombWeapon *pBomb = ToThrowBombWeapon( pWeapon );
	ioBoundBombWeapon *pBound = ToBoundBombWeapon( pWeapon );

	if( m_bDropBomb )
	{
		if( pBomb )
		{
			pBomb->SetBombDrop();
			pBomb->SetMoveDir( m_vDropMoveDir );
		}
		else if( pBound )
		{
			pBound->SetBombDrop();
			pBound->SetMoveDir( m_vDropMoveDir );
		}
	}
	else if( bJumpAttack )
	{
		if( pBomb )
		{
			pBomb->SetJumpThrow();
		}
		else if( pBound )
		{
			pBound->SetJumpThrow();
		}
	}

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

ioObjectItem::ObjectSubType ioThrowBombItem::GetObjectSubType() const
{
	return OST_THROWBOMB;
}

void ioThrowBombItem::ChangeDropState( const D3DXVECTOR3 &vDropDir )
{
	m_vDropMoveDir = vDropDir;
	m_bDropBomb = true;
}

void ioThrowBombItem::OnEquiped( ioBaseChar *pOwner )
{
	ioObjectItem::OnEquiped( pOwner );

	if( !m_EquipedEffect.IsEmpty() )
	{
		pOwner->AttachEffect( m_EquipedEffect );
	}
}

void ioThrowBombItem::OnReleased( ioBaseChar *pOwner )
{
	ioObjectItem::OnReleased( pOwner );

	if( !m_EquipedEffect.IsEmpty() )
	{
		pOwner->EndEffect( m_EquipedEffect );
	}
}