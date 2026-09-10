

#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioCloakItem.h"

ioCloakItem::ioCloakItem()
{
	m_pSpringParam = NULL;
}

ioCloakItem::ioCloakItem( const ioCloakItem &rhs )
: ioItem( rhs ),
  m_EquipMesh( rhs.m_EquipMesh ),
  m_SpringBiped( rhs.m_SpringBiped )
{
	if( rhs.m_pSpringParam )
	{
		m_pSpringParam = new RopeSpringParam;
		*m_pSpringParam = *rhs.m_pSpringParam;
	}
	else
	{
		m_pSpringParam = NULL;
	}
}

ioCloakItem::~ioCloakItem()
{
	SAFEDELETE( m_pSpringParam );
}

void ioCloakItem::LoadProperty( ioINILoader &rkLoader )
{
	ioItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	char szTitle[MAX_PATH];
	
	m_EquipMeshMap.clear();

	int iDecoCount = 0;
	while( true )
	{
		sprintf_e( szTitle, "equip_mesh%d", iDecoCount );
		rkLoader.LoadString( szTitle, "", szBuf, MAX_PATH );
		if( strcmp( szBuf, "" ) == 0 )
			break;

		m_EquipMeshMap.insert( DecoInfoMap::value_type( iDecoCount, szBuf ) );
		iDecoCount++;
	}

	SetItemType();      //Default.

	rkLoader.LoadString_e( "spring_biped", "", szBuf, MAX_PATH );

	m_SpringBiped = szBuf;
	SAFEDELETE( m_pSpringParam );

	if( !m_SpringBiped.IsEmpty() )
	{
		m_pSpringParam = new RopeSpringParam;
		m_pSpringParam->m_fGravity    = 400.0f;
		m_pSpringParam->m_fBoxWeights = 15.0f;

		m_pSpringParam->m_fDamping    = FLOAT05;
		m_pSpringParam->m_fAngDamping = FLOAT05;

		m_pSpringParam->m_fLengthFixRate = 0.4f;

		SwingParam kSwing;
		kSwing.m_fHSwingDOWN = -45.0f;
		kSwing.m_fHSwingUP = 45.0f;
		kSwing.m_fVSwingDOWN = -80.0f;
		kSwing.m_fVSwingUP = 30.0f;
		m_pSpringParam->m_SwingList.push_back( kSwing );

		kSwing.m_fVSwingDOWN = FLOAT10;
		kSwing.m_fVSwingUP = 70.0f;
		m_pSpringParam->m_SwingList.push_back( kSwing );
	}
}

const ioHashString& ioCloakItem::FindEquipMesh() const
{
	// Default Return
	DecoInfoMap::const_iterator iter = m_EquipMeshMap.find( 0 );
	if( iter != m_EquipMeshMap.end() )
		return iter->second;		

	LOG.PrintTimeAndLog( 0, "ioCloakItem::FindEquipMesh : Unknown Default" );
	return m_EquipMesh;
}

void ioCloakItem::SetItemType()
{
	ioItem::SetItemType();
	m_EquipMesh = FindEquipMesh();
}

ioItem::ItemType ioCloakItem::GetType() const
{
	return IT_CLOAK;
}

ioItem* ioCloakItem::Clone()
{
	return new ioCloakItem( *this );
}

void ioCloakItem::AddGlowLine( ioBaseChar *pOwner )
{
	ioHashString szCurEquipMesh = m_EquipMesh;
	if ( !m_CostumeInfo.m_szHelmetMesh.IsEmpty() )
		szCurEquipMesh = m_CostumeInfo.m_szHelmetMesh;

	D3DCOLORVALUE CurGlowColor = m_GlowColor;
	if ( m_CostumeInfo.m_nCostumeCode > 0 )
		CurGlowColor = m_CostumeInfo.m_GlowColor;

	pOwner->AddGlowLine( szCurEquipMesh, CurGlowColor, 2.0f );
}

void ioCloakItem::RemoveGlowLine( ioBaseChar *pOwner )
{
	ioHashString szCurEquipMesh = m_EquipMesh;
	if ( !m_CostumeInfo.m_szHelmetMesh.IsEmpty() )
		szCurEquipMesh = m_CostumeInfo.m_szHelmetMesh;

	pOwner->RemoveGlowLine( szCurEquipMesh );
}

bool ioCloakItem::HasSpringRope() const
{
	if ( !m_CostumeInfo.m_szSpring_Biped.IsEmpty() && m_CostumeInfo.m_pCostumeSpringParam )
		return true;
	
	if( m_SpringBiped.IsEmpty() || !m_pSpringParam )
		return false;

	return true;
}

const ioHashString& ioCloakItem::GetSpringBiped() const
{ 
	if ( !m_CostumeInfo.m_szSpring_Biped.IsEmpty() )
		return m_CostumeInfo.m_szSpring_Biped;

	return m_SpringBiped; 
}

const RopeSpringParam* ioCloakItem::GetSpringParam() const 
{
	if ( m_CostumeInfo.m_pCostumeSpringParam )
		return m_CostumeInfo.m_pCostumeSpringParam;

	return m_pSpringParam;
}

const ioHashString& ioCloakItem::GetEquipMesh( ioBaseChar *pOwner ) const
{
	if ( m_CostumeInfo.m_nCostumeCode > 0 )
	{
		if ( !m_CostumeInfo.m_szCloakMesh.IsEmpty() )
			return m_CostumeInfo.m_szCloakMesh;
		else
			return m_EquipMesh;
	}

	if( !g_ItemMaker.IsBaseEquipMesh() )
		return m_EquipMesh;

	if( !pOwner )
		return m_EquipMesh;

	int iClassType = pOwner->GetCharacterInfo().m_class_type;
	int iParts = DEFAULT_BASIC_ITEM_CODE * (GetType() - 1);
	if( iParts < 0 )
		return m_EquipMesh;

	DWORD dwItemCode = iParts + iClassType;
	const ioItem *pItem = g_ItemMaker.GetItemConst( dwItemCode, __FUNCTION__ );
	if( !pItem )
		return m_EquipMesh;

	switch( pItem->GetType() )
	{
	case ioItem::IT_CLOAK:
		if( ToCloakItemConst(pItem) )
			return ToCloakItemConst(pItem)->GetEquipMesh(NULL);
		break;
	}

	return m_EquipMesh;
}