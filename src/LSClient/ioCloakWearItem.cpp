#include "StdAfx.h"
#include "ioCloakWearItem.h"


ioCloakWearItem::ioCloakWearItem(void)
{
}

ioCloakWearItem::ioCloakWearItem( const ioCloakWearItem &rhs ) : ioWearItem( rhs ), m_iCloakWearType( rhs.m_iCloakWearType ), m_EquipMeshRed( rhs.m_EquipMeshRed ), m_EquipMeshBlue( rhs.m_EquipMeshBlue )
{

}

ioCloakWearItem::~ioCloakWearItem(void)
{
}

void ioCloakWearItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWearItem::LoadProperty( rkLoader );

	m_iCloakWearType = rkLoader.LoadInt_e( "cloak_wear_type", 0 );

	char szBuf[MAX_PATH];
	rkLoader.LoadString_e( "equip_mesh_red", "", szBuf, MAX_PATH );
	m_EquipMeshRed = szBuf;

	rkLoader.LoadString_e( "equip_mesh_blue", "", szBuf, MAX_PATH );
	m_EquipMeshBlue = szBuf;
}

ioItem* ioCloakWearItem::Clone()
{
	return new ioCloakWearItem( *this );
}

ioWearItem::WearSubType ioCloakWearItem::GetSubType() const
{
	return WRT_CLOAK;
}

void ioCloakWearItem::OnEquiped( ioBaseChar *pOwner )
{
	if( !pOwner )
	{
		LOG.PrintTimeAndLog( 0, "ioCloakWearItem::OnEquiped() - Owner is NULL" );
		return;
	}

	/*ioCloakItem *pCloak = pOwner->GetCloak();
	if( pCloak )
	{
		m_EquipMesh = pCloak->GetEquipMesh( pOwner );
	}
	else
	{
		m_EquipMesh.Clear();
	}*/
	//m_EquipMesh.Clear();
	if( pOwner->GetTeam() == TEAM_RED )
		m_EquipMesh = m_EquipMeshRed;
	else if( pOwner->GetTeam() == TEAM_BLUE )
		m_EquipMesh = m_EquipMeshBlue;
	else //°³ÀÎÀü
	{
		if( pOwner->GetSingleConvertTeam() == TEAM_BLUE )
			m_EquipMesh = m_EquipMeshBlue;
		else
			m_EquipMesh = m_EquipMeshRed;
	}

	//
	int iEquipBuffCnt = m_EquipBuffList.size();
	for( int i=0; i < iEquipBuffCnt; ++i )
	{
		ioHashString szBuffName = m_EquipBuffList[i];
		pOwner->AddNewBuff( szBuffName, pOwner->GetCharName(), GetName(), NULL );
	}
}

int ioCloakWearItem::GetCloakWearItemType()
{
	return m_iCloakWearType;
}