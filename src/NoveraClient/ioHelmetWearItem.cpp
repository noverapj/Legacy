

#include "stdafx.h"

#include "ioHelmetWearItem.h"

#include "ioBaseChar.h"

ioHelmetWearItem::ioHelmetWearItem()
{
	m_ChangeHair = HWH_HALF_SKIP;
}

ioHelmetWearItem::ioHelmetWearItem( const ioHelmetWearItem &rhs )
: ioWearItem( rhs ),
  m_EquipMeshRed( rhs.m_EquipMeshRed ),
  m_EquipMeshBlue( rhs.m_EquipMeshBlue ),
  m_ChangeFace( rhs.m_ChangeFace )
{
	m_ChangeHair = rhs.m_ChangeHair;
}

ioHelmetWearItem::~ioHelmetWearItem()
{
}

void ioHelmetWearItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWearItem::LoadProperty( rkLoader );
	
	char szBuf[MAX_PATH];
	rkLoader.LoadString_e( "equip_mesh_red", "", szBuf, MAX_PATH );
	m_EquipMeshRed = szBuf;

	rkLoader.LoadString_e( "equip_mesh_blue", "", szBuf, MAX_PATH );
	m_EquipMeshBlue = szBuf;

	m_ChangeHair = (HelmetWearHair)rkLoader.LoadInt_e( "hair_change_all", 0 );
	m_ChangeHair = max( HWH_HALF_SKIP, min( HWH_NO_SKIP, m_ChangeHair ) );

	m_ChangeFace = (HelmetWearFace)rkLoader.LoadInt_e( "face_change_all", 0 );
	m_ChangeFace = max( HWF_NONE, min( HWF_HALF_SKIP, m_ChangeFace ) );
}

ioItem* ioHelmetWearItem::Clone()
{
	return new ioHelmetWearItem( *this );
}

ioWearItem::WearSubType ioHelmetWearItem::GetSubType() const
{
	return WRT_HELMET;
}

void ioHelmetWearItem::OnEquiped( ioBaseChar *pOwner )
{
	if( !pOwner )
	{
		LOG.PrintTimeAndLog( 0, "ioHelmetWearItem::OnEquiped() - Owner is NULL" );
		return;
	}

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
