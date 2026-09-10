

#include "stdafx.h"

#include "ioHelmetItem.h"
#include "ioBaseChar.h"

ioHelmetItem::ioHelmetItem()
{
	m_pSpringParam = NULL;
}

ioHelmetItem::ioHelmetItem( const ioHelmetItem &rhs )
: ioItem( rhs ), m_EquipMesh( rhs.m_EquipMesh ),
 m_HelmetFaceInfo( rhs.m_HelmetFaceInfo ),
 m_SpringBiped( rhs.m_SpringBiped ),
 m_InvisibleFaceType( rhs.m_InvisibleFaceType )
{
	m_HelmetHairInfo = rhs.m_HelmetHairInfo;

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

ioHelmetItem::~ioHelmetItem()
{
	m_EquipMeshMap.clear();
	m_ChangeHairMap.clear();
	m_ChangeFaceMap.clear();
	SAFEDELETE(m_pSpringParam);
}

void ioHelmetItem::LoadProperty( ioINILoader &rkLoader )
{
	ioItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	char szTitle[MAX_PATH];

	//
	m_InvisibleFaceType = (InvisibleFaceType)rkLoader.LoadInt_e( "invisible_face", IFT_NONE );

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

	m_EquipMeshMap.clear();
	m_ChangeHairMap.clear();
	m_ChangeFaceMap.clear();

	for( int i=0 ; true ; i++ )
	{
		sprintf_e( szTitle, "equip_mesh%d", i );
		rkLoader.LoadString( szTitle, "", szBuf, MAX_PATH );
		if( strcmp( szBuf, "" ) == 0 )
			break;

		m_EquipMeshMap.insert( DecoInfoMap::value_type( i, szBuf ) );

		HelmetHairInfo kInfo;
		// human man 
		sprintf_e( szTitle, "hair_change_all%d_hm", i );
		kInfo.m_HumanMan = (HelmetHair) rkLoader.LoadInt( szTitle, HH_HALF_SKIP );
		kInfo.m_HumanMan = max( HH_HALF_SKIP, min( HH_NO_SKIP, kInfo.m_HumanMan ) );
		// human woman
		sprintf_e( szTitle, "hair_change_all%d_hw", i );
		kInfo.m_HumanWoman = (HelmetHair) rkLoader.LoadInt( szTitle, HH_NONE ); // hw은 없을 수 있으므로 HH_NONE이 기본값
		kInfo.m_HumanWoman = max( HH_HALF_SKIP, min( HH_NO_SKIP, kInfo.m_HumanWoman ) );

		m_ChangeHairMap.insert( HelmetHairMap::value_type( i, kInfo ) );


		HelmetFaceInfo kFaceInfo;
		// man
		sprintf_e( szTitle, "face_change_%d_hm", i );
		kFaceInfo.m_HumanMan = (HelmetFace)rkLoader.LoadInt( szTitle, HF_NONE );
		kFaceInfo.m_HumanMan = max( HF_NONE, min( HF_HALF_SKIP, kFaceInfo.m_HumanMan ) );

		// wouman
		sprintf_e( szTitle, "face_change_%d_hw", i );
		kFaceInfo.m_HumanWoman = (HelmetFace)rkLoader.LoadInt( szTitle, HF_NONE );
		kFaceInfo.m_HumanWoman = max( HF_NONE, min( HF_HALF_SKIP, kFaceInfo.m_HumanWoman ) );

		m_ChangeFaceMap.insert( HelmetFaceMap::value_type( i, kFaceInfo ) );
	}

	SetItemType();      //Default.
}

const ioHashString& ioHelmetItem::FindEquipMesh() const
{
	// Default Return
	DecoInfoMap::const_iterator iter = m_EquipMeshMap.find( 0 );
	if( iter != m_EquipMeshMap.end() )
		return iter->second;		

	LOG.PrintTimeAndLog( 0, "ioHelmetItem::FindEquipMesh : Unknown Default : %d", (int)GetItemCode() );
	return m_EquipMesh;
}

ioHelmetItem::HelmetHairInfo ioHelmetItem::FindChangeHair() const
{
	// Default Return
	HelmetHairMap::const_iterator iter = m_ChangeHairMap.find( 0 );
	if( iter != m_ChangeHairMap.end() )
		return iter->second;		

	LOG.PrintTimeAndLog( 0, "ioHelmetItem::FindChangeAllHair : Unknown Default : %d", (int)GetItemCode() );
	return m_HelmetHairInfo;
}

ioHelmetItem::HelmetFaceInfo ioHelmetItem::FindChangeFace() const
{
	// Default Return
	HelmetFaceMap::const_iterator iter = m_ChangeFaceMap.find( 0 );
	if( iter != m_ChangeFaceMap.end() )
		return iter->second;		

	LOG.PrintTimeAndLog( 0, "ioHelmetItem::FindChangeFace : Unknown Default : %d", (int)GetItemCode() );
	return m_HelmetFaceInfo;
}

void ioHelmetItem::SetItemType()
{
	ioItem::SetItemType();
	
	m_EquipMesh  = FindEquipMesh();
	m_HelmetHairInfo = FindChangeHair();
	m_HelmetFaceInfo = FindChangeFace();
}

ioItem::ItemType ioHelmetItem::GetType() const
{
	return IT_HELMET;
}

ioItem* ioHelmetItem::Clone()
{
	return new ioHelmetItem( *this );
}

void ioHelmetItem::AddGlowLine( ioBaseChar *pOwner )
{
	ioHashString szCurEquipMesh = m_EquipMesh;
	if ( !m_CostumeInfo.m_szHelmetMesh.IsEmpty() )
		szCurEquipMesh = m_CostumeInfo.m_szHelmetMesh;

	D3DCOLORVALUE CurGlowColor = m_GlowColor;
	if ( m_CostumeInfo.m_nCostumeCode > 0 )
		CurGlowColor = m_CostumeInfo.m_GlowColor;

	pOwner->AddGlowLine( szCurEquipMesh, CurGlowColor, 2.0f );
}

void ioHelmetItem::RemoveGlowLine( ioBaseChar *pOwner )
{
	ioHashString szCurEquipMesh = m_EquipMesh;
	if ( !m_CostumeInfo.m_szHelmetMesh.IsEmpty() )
		szCurEquipMesh = m_CostumeInfo.m_szHelmetMesh;

	pOwner->RemoveGlowLine( szCurEquipMesh );
}

ioHelmetItem::HelmetHair ioHelmetItem::GetChangeHair( bool bMale ) const
{
	HelmetHair eHair = HH_NONE;
	if ( m_CostumeInfo.m_nCostumeCode > 0 )
	{
		if( bMale )
			eHair = (HelmetHair)m_CostumeInfo.m_nHairHM;
		else
			eHair = (HelmetHair)m_CostumeInfo.m_nHairHW;
	}
	else
	{
		if( bMale )
			eHair = m_HelmetHairInfo.m_HumanMan;
		else
		{
			if( m_HelmetHairInfo.m_HumanWoman == ioHelmetItem::HH_NONE ) // 여자설정이 없다면
				eHair = m_HelmetHairInfo.m_HumanMan;
			else
				eHair = m_HelmetHairInfo.m_HumanWoman;
		}
	}

	return eHair;
}

ioHelmetItem::HelmetFace ioHelmetItem::GetChangeFace( bool bMale ) const
{
	HelmetFace eFace = HF_NONE;
	if ( m_CostumeInfo.m_nCostumeCode > 0 )
	{
		if( bMale )
			eFace = (HelmetFace)m_CostumeInfo.m_nFaceHM;
		else		
			eFace = (HelmetFace)m_CostumeInfo.m_nFaceHW;
	}
	else
	{
		if( bMale )
			eFace = m_HelmetFaceInfo.m_HumanMan;
		else
			eFace = m_HelmetFaceInfo.m_HumanWoman;
	}

	return eFace;
}

bool ioHelmetItem::HasSpringRope() const
{
	if ( !m_CostumeInfo.m_szSpring_Biped.IsEmpty() && m_CostumeInfo.m_pCostumeSpringParam )
		return true;

	if( m_SpringBiped.IsEmpty() || !m_pSpringParam )
		return false;

	return true;
}

const ioHashString& ioHelmetItem::GetSpringBiped() const
{ 
	if ( !m_CostumeInfo.m_szSpring_Biped.IsEmpty() )
		return m_CostumeInfo.m_szSpring_Biped;

	return m_SpringBiped; 
}

const RopeSpringParam* ioHelmetItem::GetSpringParam() const 
{
	if ( m_CostumeInfo.m_pCostumeSpringParam )
		return m_CostumeInfo.m_pCostumeSpringParam;

	return m_pSpringParam;
}

ioHelmetItem::InvisibleFaceType ioHelmetItem::GetInvisibleFaceType() const 
{
	if ( m_CostumeInfo.m_nCostumeCode > 0 )
		return (InvisibleFaceType)m_CostumeInfo.m_nInvisibleFaceType;

	return m_InvisibleFaceType; 
}

void ioHelmetItem::OnEquiped( ioBaseChar *pOwner )
{
	ioItem::OnEquiped( pOwner );

	if( pOwner && !pOwner->IsHasCrown() )
	{
		InvisibleFaceType CurInvisibleFaceType = m_InvisibleFaceType;
		if ( m_CostumeInfo.m_nCostumeCode > 0 )
			CurInvisibleFaceType = (InvisibleFaceType)m_CostumeInfo.m_nInvisibleFaceType;

		switch( CurInvisibleFaceType )
		{
		case IFT_ALL:
			pOwner->ShowFaceMesh( false );
			break;
		case IFT_MALE:
			if( pOwner->IsMale() )
				pOwner->ShowFaceMesh( false );
			break;
		case IFT_FEMALE:
			if( !pOwner->IsMale() )
				pOwner->ShowFaceMesh( false );
			break;
		}
	}
}


void ioHelmetItem::ReCalcVisible( ioBaseChar *pOwner )
{
	if( pOwner && !pOwner->IsHasCrown() )
	{
		InvisibleFaceType CurInvisibleFaceType = m_InvisibleFaceType;
		if ( m_CostumeInfo.m_nCostumeCode > 0 )
			CurInvisibleFaceType = (InvisibleFaceType)m_CostumeInfo.m_nInvisibleFaceType;

		switch( CurInvisibleFaceType )
		{
		case IFT_ALL:
			pOwner->ShowFaceMesh( false );
			break;
		case IFT_MALE:
			if( pOwner->IsMale() )
				pOwner->ShowFaceMesh( false );
			else
				pOwner->ShowFaceMesh( true );
			break;
		case IFT_FEMALE:
			if( !pOwner->IsMale() )
				pOwner->ShowFaceMesh( false );
			else
				pOwner->ShowFaceMesh( true );
			break;
		default: pOwner->ShowFaceMesh( true );
		}
	}
}


void ioHelmetItem::OnReleased( ioBaseChar *pOwner )
{
	ioItem::OnReleased( pOwner );

	if( pOwner && !pOwner->IsHasCrown() )
	{
		InvisibleFaceType CurInvisibleFaceType = m_InvisibleFaceType;
		if ( m_CostumeInfo.m_nCostumeCode > 0 )
			CurInvisibleFaceType = (InvisibleFaceType)m_CostumeInfo.m_nInvisibleFaceType;

		switch( CurInvisibleFaceType )
		{
		case IFT_ALL:
			pOwner->ShowFaceMesh( true );
			break;
		case IFT_MALE:
			if( pOwner->IsMale() )
				pOwner->ShowFaceMesh( true );
			break;
		case IFT_FEMALE:
			if( !pOwner->IsMale() )
				pOwner->ShowFaceMesh( true );
			break;
		}
	}
}

const ioHashString& ioHelmetItem::GetEquipMesh( ioBaseChar *pOwner ) const
{
	if ( m_CostumeInfo.m_nCostumeCode > 0 )
	{
		if ( !m_CostumeInfo.m_szHelmetMesh.IsEmpty() )
			return m_CostumeInfo.m_szHelmetMesh;
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
	case ioItem::IT_HELMET:
		if( ToHelmetItemConst(pItem) )
			return ToHelmetItemConst(pItem)->GetEquipMesh(NULL);
		break;
	}

	return m_EquipMesh;
}