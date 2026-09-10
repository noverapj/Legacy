
#include "stdafx.h"

#include "ioCreateChar.h"

#include "ItemDefine.h"

ioCreateChar::ioCreateChar( ioEntityGroup *pGrp )
{
	m_pGrp = pGrp;
	m_RaceType = RT_HUMAN;
	m_bMale = true;
	m_bThreadLoading = true;

	Help::InitColorValue( m_SkinColor, FLOAT1, FLOAT1, FLOAT1, FLOAT1 );

	Help::InitColorValue( m_HairOutLineColor, 0.0f, 0.0f, 0.0f, FLOAT1 );
	Help::InitColorValue( m_BeardOutLineColor, 0.0f, 0.0f, 0.0f, FLOAT1 );

	for(int i = 0;i < MAX_CCS;i++)
	{
		m_ItemSlot[i].dwItemCode = 0;
		m_ItemSlot[i].dwItemMaleCustom = m_ItemSlot[i].dwItemFemaleCustom = 0;
		m_ItemSlot[i].vCustomTexList.clear();
	}

	m_iFaceType = -1;
	m_dwFaceChangeTime = 0;

	m_dwMotionItemAniMaxTime = 0;
	m_dwMotionItemAniCurTime = 0;
	m_bMotionItemDelay       = false;

	if( m_pGrp )
		m_pGrp->SetEventParent( this );

	g_CustomTextureMgr.CreateNewUIChar( (DWORD)this );

	for( int i = 0; i<MAX_CCS; i++ )
		m_CostumeSlot[i].Init();
}

ioCreateChar::~ioCreateChar()
{
	g_CustomTextureMgr.DestroyUIChar( (DWORD)this );

	ioEntityGroup *pGrp = GetGroup();
	if( m_pGrp )
	{
		ioSceneNode *pSceneNode = pGrp->GetParentSceneNode();
		if( pSceneNode )
		{
			pSceneNode->RemoveAndDestroyAllChildren();

			ioSceneManager *pSceneMgr = pSceneNode->GetSceneMgr();
			if( pSceneMgr )
			{
				pSceneMgr->DestroySceneNode( pSceneNode->GetUniqueID() );
				pSceneMgr->DestroyEntityGrp( pGrp );
				m_pGrp = NULL;
			}
		}
	}
}

void ioCreateChar::Update( float fTimePerSec )
{
	if( !m_pGrp )	return;

	m_pGrp->Update( fTimePerSec * FLOAT1000 );
	m_pGrp->UpdateAttachedObjects( fTimePerSec * FLOAT1000 );
	UpdateMotionItemAni();
	CheckFaceChanging();
	UpdateBound();
}

void ioCreateChar::UpdateMotionItemAni()
{
	if( !m_pGrp ) return;
	if( m_MotionItemAniName.IsEmpty() ) return;

	if( FRAMEGETTIME() - m_dwMotionItemAniCurTime > m_dwMotionItemAniMaxTime )
	{
		if( m_bMotionItemDelay )
		{
			SetMotionItemAni( m_MotionItemAniName );
		}
		else
		{
			m_bMotionItemDelay = true;

			char szDealyAni[MAX_PATH] = "";
			if( IsMale() )
				sprintf_e( szDealyAni, "public_shop_delay.ani" );
			else
				sprintf_e( szDealyAni, "public_shop_delay_hw.ani" );

			int iAniID = m_pGrp->GetAnimationIdx( szDealyAni );
			if( iAniID != -1 )
			{
				m_pGrp->ClearAllActionAni( FLOAT100 );
				m_pGrp->ClearAllLoopAni( FLOAT100 );
				m_pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1 );

				m_dwMotionItemAniCurTime  = FRAMEGETTIME();
				m_dwMotionItemAniMaxTime  = m_pGrp->GetAnimationFullTime( iAniID );
			}
		}
	}
}

void ioCreateChar::Initialize( RaceType eType, bool bMale, bool bThread )
{
	m_RaceType = eType;
	m_bMale = bMale;
	m_bThreadLoading = bThread;

	if( !m_pGrp )	return;

	ioEntity *pEntity;

	pEntity = m_pGrp->GetEntity(0);
	if( pEntity )
		m_DefaultFaceMesh = pEntity->GetMeshName();
	else
		m_DefaultFaceMesh.Clear();

	pEntity = m_pGrp->GetEntity(1);
	if( pEntity )
		m_DefaultHairMesh = pEntity->GetMeshName();
	else
		m_DefaultHairMesh.Clear();

	pEntity = m_pGrp->GetEntity(2);
	if( pEntity )
		m_DefaultUnderwearMesh = pEntity->GetMeshName();
	else
		m_DefaultUnderwearMesh.Clear();

	pEntity = m_pGrp->GetEntity(3);
	if( pEntity )
		m_DefaultHandMesh = pEntity->GetMeshName();
	else
		m_DefaultHandMesh.Clear();

	if( !m_DefaultHairMesh.IsEmpty() )
	{
		char szHalfHair[MAX_PATH], szBindHair[MAX_PATH];
		
		wsprintf_e( szHalfHair, "half%s", m_DefaultHairMesh.c_str() );
		m_DefaultHalfHairMesh = szHalfHair;

		wsprintf_e( szBindHair, "bind%s", m_DefaultHairMesh.c_str() );
		m_DefaultBindHairMesh = szBindHair;
	}
	else
	{
		m_DefaultBindHairMesh.Clear();
		m_DefaultHalfHairMesh.Clear();
	}


	if( !m_DefaultFaceMesh.IsEmpty() )
	{
		char szHalfFace[MAX_PATH];

		wsprintf_e( szHalfFace, "skip_%s", m_DefaultFaceMesh.c_str() );
		m_SkipFaceMesh = szHalfFace;

		char szFullName[MAX_PATH]="";
		StringCbPrintf( szFullName, sizeof( szFullName ), "resource\\mesh\\%s.msh" , m_SkipFaceMesh.c_str() );
		if( !g_ResourceLoader.IsFileExist( szFullName ) )
		{
			// half가 없으면 기본으로
			m_SkipFaceMesh = m_DefaultFaceMesh;
		}

		m_CurFaceMesh = m_DefaultFaceMesh;
	}
	else
	{
		m_SkipFaceMesh.Clear();
		m_CurFaceMesh.Clear();
	}

	UpdateDefaultFaceTextureName();
}

void ioCreateChar::SetMannequins( bool bSexClass )
{
	// 얼굴 / 머리카락 / 속옷 제거.
	if( !bSexClass )
	{
		RemoveEntityInOwner( m_DefaultHairMesh );
		RemoveEntityInOwner( m_DefaultBindHairMesh );
		RemoveEntityInOwner( m_DefaultHalfHairMesh );
	}
	ChangeFaceTexture( "empty_face.dds" );
}

void ioCreateChar::SetDefaultBeard( const ioHashString &szBeard )
{
	if( szBeard.IsEmpty() )
		return;

	if( m_DefaultBeardMesh == szBeard )
		return;

	RemoveEntityInOwner( m_DefaultBeardMesh );

	m_DefaultBeardMesh = szBeard;

	AddEntityToOwner( m_DefaultBeardMesh );
	SetBeardColor( m_BeardColorTex, m_BeardOutLineColor );
}

void ioCreateChar::SetDefaultFace( const ioHashString &szFace, const ioHashString &szEffect, int iFaceType )
{
	if( szFace.IsEmpty() )
		return;

	if( m_DefaultFaceMesh == szFace )
		return;

	ioHelmetItem::HelmetFace eFaceType = ioHelmetItem::HF_NONE;
	ioHelmetItem::InvisibleFaceType eInvisibleFaceType = ioHelmetItem::IFT_NONE;
	bool bApply = false;

	const ioItem *pItem = GetSlotItem( CCS_HELMET );
	const ioHelmetItem *pHelmet = ToHelmetItemConst( pItem );
	if ( pHelmet )
	{
		eFaceType = pHelmet->GetChangeFace( m_bMale );
		eInvisibleFaceType = pHelmet->GetInvisibleFaceType();
		bApply = true;
	}

	const CostumeInfo &HelmetCostume = m_CostumeSlot[CCS_HELMET];
	if ( HelmetCostume.m_nCostumeCode > 0 )
	{
		if ( m_bMale )
			eFaceType = (ioHelmetItem::HelmetFace)HelmetCostume.m_nFaceHM;
		else
			eFaceType = (ioHelmetItem::HelmetFace)HelmetCostume.m_nFaceHW;

		eInvisibleFaceType = (ioHelmetItem::InvisibleFaceType)HelmetCostume.m_nInvisibleFaceType;
		bApply = true;
	}

	if( bApply )
	{
		switch( eFaceType )
		{
		case ioHelmetItem::HF_HALF_SKIP:
			RemoveEntityInOwner( m_SkipFaceMesh );
			break;
		}
	}
	else
	{
		RemoveEntityInOwner( m_DefaultFaceMesh );
	}

	m_pGrp->EndEffect( m_FaceEffect, false );
	
	m_DefaultFaceMesh = szFace;
	m_iFaceType = iFaceType;

	char szHalfFace[MAX_PATH];
	wsprintf_e( szHalfFace, "skip_%s", m_DefaultFaceMesh.c_str() );
	m_SkipFaceMesh = szHalfFace;

	char szFullName[MAX_PATH]="";
	StringCbPrintf( szFullName, sizeof( szFullName ), "resource\\mesh\\%s.msh" , m_SkipFaceMesh.c_str() );
	if( !g_ResourceLoader.IsFileExist( szFullName ) )
	{
		// half가 없으면 기본으로
		m_SkipFaceMesh = m_DefaultFaceMesh;
	}

	if( bApply )
	{
		switch( eFaceType )
		{
		case ioHelmetItem::HF_NONE:
			AddEntityToOwner( m_DefaultFaceMesh );
			m_CurFaceMesh = m_DefaultFaceMesh;
			break;
		case ioHelmetItem::HF_HALF_SKIP:
			AddEntityToOwner( m_SkipFaceMesh );
			m_CurFaceMesh = m_SkipFaceMesh;
			break;
		}
	}
	else
	{
		AddEntityToOwner( m_DefaultFaceMesh );
		m_CurFaceMesh = m_DefaultFaceMesh;
	}

	UpdateDefaultFaceTextureName();

	m_FaceEffect = szEffect;
	AttachEffect( m_FaceEffect, NULL );

	SetSkinColor( m_SkinColor, m_SkinOutLineColor );

	//
	if( bApply )
	{
		switch( eInvisibleFaceType )
		{
		case ioHelmetItem::IFT_ALL:
			ShowFaceMesh( false );
			break;
		case ioHelmetItem::IFT_MALE:
			if( IsMale() )
				ShowFaceMesh( false );
			break;
		case ioHelmetItem::IFT_FEMALE:
			if( !IsMale() )
				ShowFaceMesh( false );
			break;
		}
	}
}

void ioCreateChar::SetDefaultHair( const ioHashString &szHair )
{
	if( szHair.IsEmpty() )
		return;

	if( m_DefaultHairMesh == szHair )
		return;

	ioHelmetItem::HelmetHair eHairType = ioHelmetItem::HH_NONE;
	ioHelmetItem::InvisibleFaceType eInvisibleFaceType = ioHelmetItem::IFT_NONE;
	bool bApply = false;

	const ioItem *pItem = GetSlotItem( CCS_HELMET );
	const ioHelmetItem *pHelmet = ToHelmetItemConst( pItem );	
	if ( pHelmet )
	{
		eHairType = pHelmet->GetChangeHair( m_bMale );
		eInvisibleFaceType = pHelmet->GetInvisibleFaceType();
		bApply = true;
	}

	const CostumeInfo &HelmetCostume = m_CostumeSlot[CCS_HELMET];
	if ( HelmetCostume.m_nCostumeCode > 0 )
	{
		if ( m_bMale )
			eHairType = (ioHelmetItem::HelmetHair)HelmetCostume.m_nHairHM;
		else
			eHairType = (ioHelmetItem::HelmetHair)HelmetCostume.m_nHairHW;

		eInvisibleFaceType = (ioHelmetItem::InvisibleFaceType)HelmetCostume.m_nInvisibleFaceType;
		bApply = true;
	}

	if( bApply )
	{
		switch( eHairType )
		{
		case ioHelmetItem::HH_HALF_SKIP:
			RemoveEntityInOwner( m_DefaultHalfHairMesh );
			break;
		case ioHelmetItem::HH_ALL_SKIP:
			// 이미 머리가 없다.
			break;
		case ioHelmetItem::HH_BIND_SKIP:
			RemoveEntityInOwner( m_DefaultBindHairMesh );
			break;
		case ioHelmetItem::HH_NO_SKIP:
			RemoveEntityInOwner( m_DefaultHairMesh );
			break;
		}
	}
	else
	{
		RemoveEntityInOwner( m_DefaultHairMesh );
	}
		
	char szHalfHair[MAX_PATH], szBindHair[MAX_PATH];
	wsprintf_e( szHalfHair, "half%s", szHair );
	m_DefaultHalfHairMesh = szHalfHair;
	wsprintf_e( szBindHair, "bind%s", szHair );
	m_DefaultBindHairMesh = szBindHair;
	m_DefaultHairMesh = szHair;

	if( bApply )
	{
		switch( eHairType )
		{
		case ioHelmetItem::HH_HALF_SKIP:
			AddEntityToOwner( m_DefaultHalfHairMesh );
			break;
		case ioHelmetItem::HH_ALL_SKIP:
			// 추가 안해도 된다.
			break;
		case ioHelmetItem::HH_BIND_SKIP:
			AddEntityToOwner( m_DefaultBindHairMesh );
			break;
		case ioHelmetItem::HH_NO_SKIP:
			AddEntityToOwner( m_DefaultHairMesh );
			break;
		}
	}
	else
	{
		AddEntityToOwner( m_DefaultHairMesh );
	}
	
	SetHairColor( m_HairColorTex, m_HairOutLineColor );

	//
	if( bApply )
	{
		switch( eInvisibleFaceType )
		{
		case ioHelmetItem::IFT_ALL:
			ShowFaceMesh( false );
			break;
		case ioHelmetItem::IFT_MALE:
			if( IsMale() )
				ShowFaceMesh( false );
			break;
		case ioHelmetItem::IFT_FEMALE:
			if( !IsMale() )
				ShowFaceMesh( false );
			break;
		}
	}
}

void ioCreateChar::SetDefaultAccessory( const ioHashString &szAccessory )
{
	if( szAccessory.IsEmpty() )
		return;

	if( m_DefaultAccessoryMesh == szAccessory )
		return;

	RemoveEntityInOwner( m_DefaultAccessoryMesh );

	m_DefaultAccessoryMesh = szAccessory;
	AddEntityToOwner( m_DefaultAccessoryMesh );
}

void ioCreateChar::SetDefaultUnderwear( const ioHashString &szUnderwear )
{
	if( szUnderwear.IsEmpty() )
		return;

	if( m_DefaultUnderwearMesh == szUnderwear )
		return;

	const ioItem *pItem = GetSlotItem( CCS_ARMOR );
	const ioArmorItem *pArmor = ToArmorItemConst( pItem );
	if( pArmor )
	{
		m_DefaultUnderwearMesh = szUnderwear;
		return;
	}

	RemoveEntityInOwner( m_DefaultUnderwearMesh );

	m_DefaultUnderwearMesh = szUnderwear;
	AddEntityToOwner( m_DefaultUnderwearMesh );
}

void ioCreateChar::ChangeSkeleton( const ioHashString &rkSkeleton )
{
	if( rkSkeleton.IsEmpty() ) return;
	if( !m_pGrp || !m_pGrp->HasSkeleton() )	return;
	if( m_szChangeSkeleton == rkSkeleton ) return;

	m_szChangeSkeleton = rkSkeleton;
	m_pGrp->SetSkeletonName( rkSkeleton );
}

void ioCreateChar::SetSkinColor( const D3DCOLORVALUE &rkSkin, const D3DCOLORVALUE &rkOutLine )
{
	m_SkinColor = rkSkin;
	m_SkinOutLineColor = rkOutLine;
	Help::ChangeSkinColor( m_pGrp, m_SkinColor, m_SkinOutLineColor );
}

void ioCreateChar::SetHairColor( const ioHashString &szHairColorTex, const D3DCOLORVALUE &rkOutLIne )
{
	if( !m_pGrp )	return;

	m_HairColorTex = szHairColorTex;
	m_HairOutLineColor = rkOutLIne;

	if( m_HairColorTex.IsEmpty() )	return;

	ioEntity *pHair = m_pGrp->GetEntity( m_DefaultHairMesh );
	if( !pHair )
	{
		pHair = m_pGrp->GetEntity( m_DefaultBindHairMesh );

		if( !pHair )
			pHair = m_pGrp->GetEntity( m_DefaultHalfHairMesh );
	}

	Help::ChangeColorTexAndOutLine( pHair, m_HairColorTex, &m_HairOutLineColor );
}

void ioCreateChar::SetBeardColor( const ioHashString &szBeardColorTex, const D3DCOLORVALUE &rkOutLine )
{
	if( !m_pGrp )	return;

	m_BeardColorTex = szBeardColorTex;
	m_BeardOutLineColor = rkOutLine;

	if( m_BeardColorTex.IsEmpty() )	return;

	if( !m_DefaultBeardMesh.IsEmpty() )
	{
		Help::ChangeColorTexAndOutLine( m_pGrp->GetEntity( m_DefaultBeardMesh ),
										m_BeardColorTex,
										&m_BeardOutLineColor );
	}
}

void ioCreateChar::AddEntityEvent( ioEntity *pEntity )
{
	if( !pEntity ) return;

	for(int i = CCS_WEAPON;i <= CCS_CLOAK;i++)
	{
		EquipItemInfo &rkSlot = m_ItemSlot[i];

		DWORD dwCustomIndex = 0;
		if( IsMale() && rkSlot.dwItemMaleCustom > 0 )
			dwCustomIndex = rkSlot.dwItemMaleCustom;
		if( !IsMale() && rkSlot.dwItemFemaleCustom  > 0 )
			dwCustomIndex = rkSlot.dwItemFemaleCustom;

		if( dwCustomIndex == 0 ) continue;

		if( rkSlot.vCustomTexList.empty() ) continue;

		CustomTextureList kOriginalList;
		Help::CustomEntityTextureNameCnt( pEntity, 0, kOriginalList );

		CustomTextureList kCustomList;
		Help::CustomEntityTextureNameCnt( pEntity, dwCustomIndex, kCustomList );

		if( kOriginalList.size() == 0 ) continue;
		if( kOriginalList.size() != kCustomList.size() ) continue;

		for(int iOri = 0;iOri < (int)kOriginalList.size();iOri++)
		{
			for(int iCustom = 0;iCustom < (int)rkSlot.vCustomTexList.size();iCustom++)
			{
				if( kOriginalList[iOri].m_szTextureName == rkSlot.vCustomTexList[iCustom].m_szTextureName )
				{
					if( g_CustomTextureMgr.IsExistenceFile( g_MyInfo.GetPublicID(), kOriginalList[iOri].m_szTextureName, dwCustomIndex, kCustomList[iOri].m_dwCustomSeq ) )
					{
						Help::CustomEntityTexture( pEntity, kOriginalList[iOri].m_szTextureName, kCustomList[iOri].m_szTextureName, kCustomList[iOri].m_dwCustomSeq );
					}
				}
			}
		}
	}
}

bool ioCreateChar::IsCustomItemCheck( DWORD dwCustomIndex )
{
	for(int i = 0;i < MAX_CCS;i++)
	{
		if( IsMale() )
		{
			if( m_ItemSlot[i].dwItemMaleCustom == dwCustomIndex )
				return true;
		}
		else
		{
			if( m_ItemSlot[i].dwItemFemaleCustom == dwCustomIndex )
				return true;
		}
	}
	return false;
}

void ioCreateChar::CustomTextureDownLoadComplete( const ioHashString &rkDstTex, const DWORD dwCustomIndex, const DWORD dwSubIndex )
{
	if( !IsCustomItemCheck( dwCustomIndex ) ) return;

	// 현재 착용중인 장비의 CustomIndex / SubIndex를 확인하여 교체한다.
	char szSrcName[MAX_PATH] = "";
	sprintf( szSrcName, "%d_%d.lsc", dwCustomIndex, dwSubIndex );
	Help::CustomEntityGroupTexture( m_pGrp, rkDstTex, szSrcName, dwSubIndex );
}

void ioCreateChar::InheritToNewChar( ioCreateChar *pNewChar )
{
	for( int i=0 ; i<MAX_CCS ; i++ )
	{
		if( m_ItemSlot[i].dwItemCode != 0 )
		{
			pNewChar->EquipItem( m_ItemSlot[i].dwItemCode, m_ItemSlot[i].dwItemMaleCustom, m_ItemSlot[i].dwItemFemaleCustom );
		}
	}
}

const ioItem* ioCreateChar::GetSlotItem( int iSlot )
{
	if( !COMPARE( iSlot, 0, MAX_CCS ) )
		return NULL;

	if( m_ItemSlot[iSlot].dwItemCode == 0 )
		return NULL;

	return g_ItemMaker.GetItemConst( m_ItemSlot[iSlot].dwItemCode, __FUNCTION__ );
}

void ioCreateChar::EquipItem( DWORD dwItemCode, DWORD dwItemMaleCustom, DWORD dwItemFemaleCustom, int nCostumCode )
{
	if( dwItemCode == 0 )	return;

	const ioItem *pItem = g_ItemMaker.GetItemConst( dwItemCode, __FUNCTION__ );
	if( !pItem )	return;
	
	switch( pItem->GetType() )
	{
	case ioItem::IT_WEAPON:
		EquipWeapon( pItem, dwItemMaleCustom, dwItemFemaleCustom );
		break;
	case ioItem::IT_ARMOR:
		EquipArmor( pItem, dwItemMaleCustom, dwItemFemaleCustom, nCostumCode );
		break;
	case ioItem::IT_HELMET:
		EquipHelmet( pItem, dwItemMaleCustom, dwItemFemaleCustom, nCostumCode );
		break;
	case ioItem::IT_CLOAK:
		EquipCloak( pItem, dwItemMaleCustom, dwItemFemaleCustom, nCostumCode );
		break;
	}
	
	SetSkinColor( m_SkinColor, m_SkinOutLineColor );
}

void ioCreateChar::EquipSlotData( int iSlot, DWORD dwItemCode, DWORD dwItemMaleCustom, DWORD dwItemFemaleCustom )
{
	if( COMPARE( iSlot, CCS_WEAPON, MAX_CCS ) )
	{
		m_ItemSlot[iSlot].dwItemCode = dwItemCode;
		m_ItemSlot[iSlot].dwItemMaleCustom = dwItemMaleCustom;
		m_ItemSlot[iSlot].dwItemFemaleCustom = dwItemFemaleCustom;
		m_ItemSlot[iSlot].vCustomTexList.clear();
	}
}

void ioCreateChar::EquipSlotEntity( int iSlot, ioEntity *pEntity )
{
	if( !pEntity ) return;

	if( COMPARE( iSlot, CCS_WEAPON, MAX_CCS ) )
	{
		if( IsMale() && m_ItemSlot[iSlot].dwItemMaleCustom > 0 || !IsMale() && m_ItemSlot[iSlot].dwItemFemaleCustom > 0 )
		{
			Help::CustomEntityTextureNameCnt( pEntity, 0, m_ItemSlot[iSlot].vCustomTexList );
			AddEntityEvent( pEntity );
		}
	}
}

void ioCreateChar::EquipWeapon( const ioItem *pItem, DWORD dwItemMaleCustom, DWORD dwItemFemaleCustom )
{
	ReleaseItem( CCS_WEAPON );

	const ioWeaponItem *pWeapon = ToWeaponItemConst( pItem );
	if( pWeapon )
	{
		AttachEffect( pWeapon->GetItemEquipEffect( m_bMale ), NULL );

		EquipSlotData( CCS_WEAPON, pItem->GetItemCode(), dwItemMaleCustom, dwItemFemaleCustom );
		EquipSlotEntity( CCS_WEAPON, AddEntityToOwner( pWeapon->GetEquipMesh(NULL), true ) );
	}

	if( ToHookItem( pWeapon ) )
	{
		RemoveEntityInOwner( m_DefaultHandMesh );
	}
}

void ioCreateChar::EquipArmor( const ioItem *pItem, DWORD dwItemMaleCustom, DWORD dwItemFemaleCustom, int nCostumeCode )
{
	ReleaseArmorSlot( false );
	RemoveEntityInOwner( m_DefaultUnderwearMesh );
	RemoveEntityInOwner( m_DefaultHandMesh );
	
	CostumeInfo sNewCostume;
	g_CostumeInfoMgr.GetCostumeInfo( nCostumeCode, sNewCostume );

	const ioArmorItem *pArmor = ToArmorItemConst( pItem );	

	ioHashString szEquipEffect;
	ioHashString szBodyMesh;
	
	//장착할 메쉬 가져오기
	if ( sNewCostume.m_nCostumeCode > 0 )
	{
		szEquipEffect = GetCostumeEffect( &sNewCostume );
		szBodyMesh = sNewCostume.m_szBodyMesh;
	}
	else if( pArmor )
	{
		szEquipEffect = pArmor->GetItemEquipEffect( m_bMale );
		szBodyMesh = pArmor->GetBodyMesh(NULL);
	}

	if( !szEquipEffect.IsEmpty() )
	{
		AttachEffect( szEquipEffect, NULL );
		szEquipEffect.Clear();
	}

	if ( pArmor && !szBodyMesh.IsEmpty() )
	{
		EquipSlotData( CCS_ARMOR, pItem->GetItemCode(), dwItemMaleCustom, dwItemFemaleCustom );
		EquipSlotEntity( CCS_ARMOR, AddEntityToOwner( szBodyMesh, true ) );
	}

	szBodyMesh.Clear();
	bool bSex = true;
	
	//손
	ioHashString szHandMesh;
	const ioWeaponItem *pWeapon = ToWeaponItemConst( GetSlotItem( CCS_WEAPON ) );
	if( !ToHookItem(pWeapon) )
	{
		//장착할 손 생성
		if ( sNewCostume.m_nCostumeCode > 0 )
			szHandMesh = sNewCostume.m_szHandMesh;
		else if( pArmor->HasHandMesh(NULL) )
			szHandMesh = pArmor->GetHandMesh(NULL);
		
		if ( szHandMesh.IsEmpty() )
		{
			szHandMesh = m_DefaultHandMesh;
			bSex = false;
		}

		if ( !szHandMesh.IsEmpty() )
			EquipSlotEntity( CCS_ARMOR, AddEntityToOwner( szHandMesh, bSex ) );

		szHandMesh.Clear();
		bSex = true;
	}

	ioHashString szSpringRope;
	const RopeSpringParam *pSpringParam = NULL;

	if ( sNewCostume.m_nCostumeCode > 0 )
	{
		szSpringRope = sNewCostume.m_szSpring_Biped;
		pSpringParam = sNewCostume.m_pCostumeSpringParam;
	}
	else if( pArmor->HasSpringRope() )
	{
		szSpringRope = pArmor->GetSpringBiped();
		pSpringParam = pArmor->GetSpringParam();
	}

	if( !szSpringRope.IsEmpty() && pSpringParam && m_pGrp )
		m_pGrp->AddRopeSpringCurve( szSpringRope, pSpringParam );

	SetSkinColor( m_SkinColor, m_SkinOutLineColor );

	m_CostumeSlot[CCS_ARMOR] = sNewCostume;
}

void ioCreateChar::EquipHelmet( const ioItem *pItem, DWORD dwItemMaleCustom, DWORD dwItemFemaleCustom, int nCostumeCode )
{
	ReleaseHelmetSlot( false );
	RemoveEntityInOwner( m_DefaultHairMesh );
	RemoveEntityInOwner( m_DefaultFaceMesh );

	CostumeInfo sNewCostume;
	g_CostumeInfoMgr.GetCostumeInfo( nCostumeCode, sNewCostume );

	const ioHelmetItem *pHelmet = ToHelmetItemConst( pItem );
	
	ioHelmetItem::HelmetHair eNewHair = ioHelmetItem::HH_NONE; 
	ioHelmetItem::HelmetFace eNewFace = ioHelmetItem::HF_NONE;
	ioHelmetItem::InvisibleFaceType eInvisibleFaceType = ioHelmetItem::IFT_NONE;
	
	if ( sNewCostume.m_nCostumeCode > 0 )
	{
		if ( m_bMale )
		{
			eNewHair = (ioHelmetItem::HelmetHair)sNewCostume.m_nHairHM;
			eNewFace = (ioHelmetItem::HelmetFace)sNewCostume.m_nFaceHM;
		}
		else
		{
			eNewHair = (ioHelmetItem::HelmetHair)sNewCostume.m_nHairHW;
			eNewFace = (ioHelmetItem::HelmetFace)sNewCostume.m_nFaceHW;
		}

		eInvisibleFaceType = (ioHelmetItem::InvisibleFaceType)sNewCostume.m_nInvisibleFaceType;
	}
	else if ( pHelmet )
	{
		eNewHair = pHelmet->GetChangeHair( m_bMale );
		eNewFace = pHelmet->GetChangeFace( m_bMale );
		eInvisibleFaceType = pHelmet->GetInvisibleFaceType();
	}

	switch( eNewHair )
	{
	case ioHelmetItem::HH_HALF_SKIP:
		AddEntityToOwner( m_DefaultHalfHairMesh );
		break;
	case ioHelmetItem::HH_ALL_SKIP:
		break;
	case ioHelmetItem::HH_BIND_SKIP:
		AddEntityToOwner( m_DefaultBindHairMesh );
		break;
	case ioHelmetItem::HH_NO_SKIP:
		AddEntityToOwner( m_DefaultHairMesh );
		break;
	}

	switch( eNewFace )
	{
	case ioHelmetItem::HF_NONE:
		AddEntityToOwner( m_DefaultFaceMesh );
		m_CurFaceMesh = m_DefaultFaceMesh;
		break;
	case ioHelmetItem::HF_HALF_SKIP:
		AddEntityToOwner( m_SkipFaceMesh );
		m_CurFaceMesh = m_SkipFaceMesh;
		break;
	}

	switch( eInvisibleFaceType )
	{
	case ioHelmetItem::IFT_ALL:
		ShowFaceMesh( false );
		break;
	case ioHelmetItem::IFT_MALE:
		if( IsMale() )
			ShowFaceMesh( false );
		break;
	case ioHelmetItem::IFT_FEMALE:
		if( !IsMale() )
			ShowFaceMesh( false );
		break;
	}

	ioHashString szEquipEffect;
	
	if ( sNewCostume.m_nCostumeCode > 0 )
		szEquipEffect = GetCostumeEffect( &sNewCostume );
	else if( pHelmet )
		szEquipEffect = pHelmet->GetItemEquipEffect( m_bMale );

	if ( !szEquipEffect.IsEmpty() )
	{
		AttachEffect( szEquipEffect, NULL );
		szEquipEffect.Clear();
	}
	
	ioHashString szHelmetMesh;
	if ( sNewCostume.m_nCostumeCode > 0 )
		szHelmetMesh = sNewCostume.m_szHelmetMesh;
	else
		szHelmetMesh = pHelmet->GetEquipMesh(NULL);

	if ( pHelmet && !szHelmetMesh.IsEmpty() )
	{
		EquipSlotData( CCS_HELMET, pItem->GetItemCode(), dwItemMaleCustom, dwItemFemaleCustom );
		EquipSlotEntity( CCS_HELMET, AddEntityToOwner( szHelmetMesh, true ) );
	}

	ioHashString szSpringRope;
	const RopeSpringParam *pSpringParam = NULL;

	if ( sNewCostume.m_nCostumeCode > 0 )
	{
		szSpringRope = sNewCostume.m_szSpring_Biped;
		pSpringParam = sNewCostume.m_pCostumeSpringParam;
	}
	else if( pHelmet->HasSpringRope() )
	{
		szSpringRope = pHelmet->GetSpringBiped();
		pSpringParam = pHelmet->GetSpringParam();
	}

	if( !szSpringRope.IsEmpty() && pSpringParam && m_pGrp )
		m_pGrp->AddRopeSpringCurve( szSpringRope, pSpringParam );

	SetHairColor( m_HairColorTex, m_HairOutLineColor );
	SetBeardColor( m_BeardColorTex, m_BeardOutLineColor );
	SetSkinColor( m_SkinColor, m_SkinOutLineColor );

	m_CostumeSlot[CCS_HELMET] = sNewCostume;
}

void ioCreateChar::EquipCloak( const ioItem *pItem, DWORD dwItemMaleCustom, DWORD dwItemFemaleCustom, int nCostumeCode )
{
	ReleaseCloakSlot();

	CostumeInfo sNewCostume;
	g_CostumeInfoMgr.GetCostumeInfo( nCostumeCode, sNewCostume );

	const ioCloakItem *pCloak = ToCloakItemConst( pItem );

	ioHashString szEquipEffect;
	ioHashString szCloakMesh;

	if ( sNewCostume.m_nCostumeCode > 0 )
	{
		szEquipEffect = GetCostumeEffect( &sNewCostume );
		szCloakMesh = sNewCostume.m_szCloakMesh;
	}
	else if( pCloak )
	{
		szEquipEffect = pCloak->GetItemEquipEffect( m_bMale );
		szCloakMesh = pCloak->GetEquipMesh(NULL);
	}

	if ( !szEquipEffect.IsEmpty() )
	{
		AttachEffect( szEquipEffect, NULL );
		szEquipEffect.Clear();
	}

	if ( pCloak && !szCloakMesh.IsEmpty() )
	{
		EquipSlotData( CCS_CLOAK, pItem->GetItemCode(), dwItemMaleCustom, dwItemFemaleCustom );
		EquipSlotEntity( CCS_CLOAK, AddEntityToOwner( szCloakMesh, true ) );
	}

	ioHashString szSpringRope;
	const RopeSpringParam *pSpringParam = NULL;

	if ( sNewCostume.m_nCostumeCode > 0 )
	{
		szSpringRope = sNewCostume.m_szSpring_Biped;
		pSpringParam = sNewCostume.m_pCostumeSpringParam;
	}
	else if( pCloak->HasSpringRope() )
	{
		szSpringRope = pCloak->GetSpringBiped();
		pSpringParam = pCloak->GetSpringParam();
	}

	if( !szSpringRope.IsEmpty() && pSpringParam && m_pGrp )
		m_pGrp->AddRopeSpringCurve( szSpringRope, pSpringParam );

	SetSkinColor( m_SkinColor, m_SkinOutLineColor );

	m_CostumeSlot[CCS_CLOAK] = sNewCostume;
}

void ioCreateChar::ReleaseAllEquipItems()
{
	for( int i=0 ; i<MAX_CCS ; i++ )
	{
		if( m_ItemSlot[i].dwItemCode != 0 )
		{
			ReleaseItem( i );
		}
	}
}

void ioCreateChar::ReleaseItem( int iSlot )
{
	switch( iSlot )
	{
	case 0:
		ReleaseWeaponSlot();
		break;
	case 1:
		ReleaseArmorSlot();
		break;
	case 2:
		ReleaseHelmetSlot();
		break;
	case 3:
		ReleaseCloakSlot();
		break;
	}
	
	SetSkinColor( m_SkinColor, m_SkinOutLineColor );
}

void ioCreateChar::ReleaseSlotData( int iSlot )
{
	if( COMPARE( iSlot, CCS_WEAPON, MAX_CCS ) )
	{
		m_ItemSlot[iSlot].dwItemCode = 0;
		m_ItemSlot[iSlot].dwItemMaleCustom = m_ItemSlot[iSlot].dwItemFemaleCustom = 0;
		m_ItemSlot[iSlot].vCustomTexList.clear();
	}
}

void ioCreateChar::ReleaseWeaponSlot()
{
	const ioItem *pItem = GetSlotItem( CCS_WEAPON );
	if( !pItem )	return;

	ReleaseSlotData( CCS_WEAPON );

	ioHashString szEquipEffect = pItem->GetItemEquipEffect( m_bMale );
	if( !szEquipEffect.IsEmpty() )
		m_pGrp->EndEffect( szEquipEffect, false);

	const ioWeaponItem *pWeapon = ToWeaponItemConst( pItem );
	if( pWeapon )
	{
		RemoveEntityInOwner( pWeapon->GetEquipMesh(NULL), true );
	}

	if( ToHookItem( pWeapon ) )
	{
		AddEntityToOwner( m_DefaultHandMesh );
	}
}

void ioCreateChar::ReleaseArmorSlot( bool bDefault )
{
	const CostumeInfo *PreArmorCostume = &m_CostumeSlot[CCS_ARMOR];
	if ( !PreArmorCostume )
		return;

	const ioArmorItem *pArmor = ToArmorItemConst( GetSlotItem( CCS_ARMOR ) );
	ReleaseSlotData( CCS_ARMOR );
	
	ioHashString szEquipEffect;
	ioHashString szBodyMesh;
	ioHashString szHandMesh;

	if ( PreArmorCostume->m_nCostumeCode > 0 )
	{
		szEquipEffect = GetCostumeEffect( PreArmorCostume );
		szBodyMesh = PreArmorCostume->m_szBodyMesh;
		szHandMesh = PreArmorCostume->m_szHandMesh;
	}
	else if ( pArmor )
	{
		szEquipEffect = pArmor->GetItemEquipEffect( m_bMale );
		szBodyMesh = pArmor->GetBodyMesh(NULL);
		szHandMesh = pArmor->GetHandMesh(NULL);
	}

	if( !szEquipEffect.IsEmpty() )
	{
		m_pGrp->EndEffect( szEquipEffect, false );
		szEquipEffect.Clear();
	}

	if ( !szBodyMesh.IsEmpty() )
	{
		RemoveEntityInOwner( szBodyMesh, true );
		szBodyMesh.Clear();
	}

	if ( bDefault )
		AddEntityToOwner( m_DefaultUnderwearMesh );

	const ioWeaponItem *pWeapon = ToWeaponItemConst( GetSlotItem( CCS_WEAPON ) );
	if( !ToHookItem( pWeapon ) )
	{
		if ( bDefault )
			AddEntityToOwner( m_DefaultHandMesh );

		if ( !szHandMesh.IsEmpty() )
		{
			RemoveEntityInOwner( szHandMesh, true );
			szHandMesh.Clear();
		}
	}

	//스프링 로프 해제
	ioHashString szSpringRope;
	const RopeSpringParam *pSpringParam = NULL;

	if ( PreArmorCostume->m_nCostumeCode > 0 )
	{
		szSpringRope = PreArmorCostume->m_szSpring_Biped;
		pSpringParam = PreArmorCostume->m_pCostumeSpringParam;		
	}
	else if( pArmor && pArmor->HasSpringRope() )
	{
		szSpringRope = pArmor->GetSpringBiped();
		pSpringParam = pArmor->GetSpringParam();
	}

	if ( !szSpringRope.IsEmpty() && pSpringParam )
	{
		const CostumeInfo *pCheckHelmetCostume = &m_CostumeSlot[CCS_HELMET];
		const CostumeInfo *pCheckCloakCostume = &m_CostumeSlot[CCS_CLOAK];

		const ioHelmetItem *pCheckHelmet = ToHelmetItemConst( GetSlotItem( CCS_HELMET ) );
		const ioCloakItem *pCheckCloak = ToCloakItemConst( GetSlotItem( CCS_CLOAK ) );

		bool bHelmetHas = true;
		bool bCloaktHas = true;

		if ( pCheckHelmetCostume && pCheckHelmetCostume->m_nCostumeCode > 0 && 
			( pCheckHelmetCostume->m_szSpring_Biped.IsEmpty() || !pCheckHelmetCostume->m_pCostumeSpringParam ) )
			bHelmetHas = false;
		else if( !pCheckHelmet || !pCheckHelmet->HasSpringRope() )
			bHelmetHas = false;

		if ( pCheckCloakCostume && pCheckCloakCostume->m_nCostumeCode > 0 && 
			( pCheckCloakCostume->m_szSpring_Biped.IsEmpty() || !pCheckCloakCostume->m_pCostumeSpringParam ) )
			bCloaktHas = false;
		else if( !pCheckCloak || !pCheckCloak->HasSpringRope() )
			bCloaktHas = false;

		if( m_pGrp && !bHelmetHas && !bCloaktHas )
		{
			m_pGrp->RemoveRopeSpringCurve( szSpringRope );
		}
	}
}

void ioCreateChar::ReleaseHelmetSlot( bool bDefault )
{
	const CostumeInfo *PreHelmetCostume = &m_CostumeSlot[CCS_HELMET];
	if ( !PreHelmetCostume )
		return;

	const ioHelmetItem *pHelmet = ToHelmetItemConst( GetSlotItem( CCS_HELMET ) );
	ReleaseSlotData( CCS_HELMET );

	ioHashString szEquipEffect;
	ioHashString szHelmetMesh;

	if ( PreHelmetCostume->m_nCostumeCode > 0 )
	{
		szEquipEffect = GetCostumeEffect( PreHelmetCostume );
		szHelmetMesh = PreHelmetCostume->m_szHelmetMesh;
	}
	else if ( pHelmet )
	{
		szEquipEffect = pHelmet->GetItemEquipEffect( m_bMale );
		szHelmetMesh = pHelmet->GetEquipMesh(NULL);
	}

	if( !szEquipEffect.IsEmpty() )
	{
		m_pGrp->EndEffect( szEquipEffect, false );
		szEquipEffect.Clear();
	}

	if ( !szHelmetMesh.IsEmpty() )
	{
		RemoveEntityInOwner( szHelmetMesh, true );
		szHelmetMesh.Clear();
	}
	
	ioHelmetItem::HelmetHair eHairType = ioHelmetItem::HH_NONE; 
	ioHelmetItem::HelmetFace eFaceType = ioHelmetItem::HF_NONE;
	ioHelmetItem::InvisibleFaceType eInvisibleFaceType = ioHelmetItem::IFT_NONE;
	
	if ( PreHelmetCostume->m_nCostumeCode > 0 )
	{
		if ( m_bMale )
		{
			eHairType = (ioHelmetItem::HelmetHair)PreHelmetCostume->m_nHairHM;
			eFaceType = (ioHelmetItem::HelmetFace)PreHelmetCostume->m_nFaceHM;
		}
		else
		{
			eHairType = (ioHelmetItem::HelmetHair)PreHelmetCostume->m_nHairHW;
			eFaceType = (ioHelmetItem::HelmetFace)PreHelmetCostume->m_nFaceHW;
		}

		eInvisibleFaceType = (ioHelmetItem::InvisibleFaceType)PreHelmetCostume->m_nInvisibleFaceType;
	}
	else if ( pHelmet )
	{
		eHairType = pHelmet->GetChangeHair( m_bMale );
		eFaceType = pHelmet->GetChangeFace( m_bMale );
		eInvisibleFaceType = pHelmet->GetInvisibleFaceType();
	}

	switch( eHairType )
	{
	case ioHelmetItem::HH_HALF_SKIP:
		if ( bDefault )
			AddEntityToOwner( m_DefaultHairMesh );
		RemoveEntityInOwner( m_DefaultHalfHairMesh );
		break;
	case ioHelmetItem::HH_ALL_SKIP:
		if ( bDefault )
			AddEntityToOwner( m_DefaultHairMesh );
		break;
	case ioHelmetItem::HH_BIND_SKIP:
		if ( bDefault )
			AddEntityToOwner( m_DefaultHairMesh );
		RemoveEntityInOwner( m_DefaultBindHairMesh );
		break;
	case ioHelmetItem::HH_NO_SKIP:
		break;
	}
	
	switch( eFaceType )
	{
	case ioHelmetItem::HF_HALF_SKIP:
		RemoveEntityInOwner( m_SkipFaceMesh );
		if ( bDefault )
		{
			AddEntityToOwner( m_DefaultFaceMesh );
			m_CurFaceMesh = m_DefaultFaceMesh;
		}
		break;
	}

	SetHairColor( m_HairColorTex, m_HairOutLineColor );
	SetBeardColor( m_BeardColorTex, m_BeardOutLineColor );
	SetSkinColor( m_SkinColor, m_SkinOutLineColor );

	//스프링 로프 해제
	ioHashString szSpringRope;
	const RopeSpringParam *pSpringParam = NULL;

	if ( PreHelmetCostume->m_nCostumeCode > 0 )
	{
		szSpringRope = PreHelmetCostume->m_szSpring_Biped;
		pSpringParam = PreHelmetCostume->m_pCostumeSpringParam;		
	}
	else if( pHelmet && pHelmet->HasSpringRope() )
	{
		szSpringRope = pHelmet->GetSpringBiped();
		pSpringParam = pHelmet->GetSpringParam();
	}

	if ( !szSpringRope.IsEmpty() && pSpringParam )
	{
		const CostumeInfo *pCheckArmorCostume = &m_CostumeSlot[CCS_ARMOR];
		const CostumeInfo *pCheckCloakCostume = &m_CostumeSlot[CCS_CLOAK];

		const ioArmorItem *pCheckArmor = ToArmorItemConst( GetSlotItem( CCS_ARMOR ) );
		const ioCloakItem *pCheckCloak = ToCloakItemConst( GetSlotItem( CCS_CLOAK ) );

		bool bArmorHas = true;
		bool bCloaktHas = true;

		if ( pCheckArmorCostume && pCheckArmorCostume->m_nCostumeCode > 0 && 
			( pCheckArmorCostume->m_szSpring_Biped.IsEmpty() || !pCheckArmorCostume->m_pCostumeSpringParam ) )
			bArmorHas = false;
		else if( !pCheckArmor || !pCheckArmor->HasSpringRope() )
			bArmorHas = false;

		if ( pCheckCloakCostume && pCheckCloakCostume->m_nCostumeCode > 0 && 
			( pCheckCloakCostume->m_szSpring_Biped.IsEmpty() || !pCheckCloakCostume->m_pCostumeSpringParam ) )
			bCloaktHas = false;
		else if( !pCheckCloak || !pCheckCloak->HasSpringRope() )
			bCloaktHas = false;

		if( m_pGrp && !bArmorHas && !bCloaktHas )
		{
			m_pGrp->RemoveRopeSpringCurve( szSpringRope );
		}
	}
	
	//
	switch( eInvisibleFaceType )
	{
	case ioHelmetItem::IFT_ALL:
		ShowFaceMesh( true );
		break;
	case ioHelmetItem::IFT_MALE:
		if( IsMale() )
			ShowFaceMesh( true );
		break;
	case ioHelmetItem::IFT_FEMALE:
		if( !IsMale() )
			ShowFaceMesh( true );
		break;
	}
}

void ioCreateChar::ReleaseCloakSlot()
{
	const CostumeInfo *PreCloakCostume = &m_CostumeSlot[CCS_CLOAK];
	if ( !PreCloakCostume )
		return;

	const ioCloakItem *pCloak = ToCloakItemConst( GetSlotItem( CCS_CLOAK ) );
	ReleaseSlotData( CCS_CLOAK );

	ioHashString szEquipEffect; 
	ioHashString szCloakMesh;

	if ( PreCloakCostume->m_nCostumeCode > 0 )
	{
		szEquipEffect = GetCostumeEffect( PreCloakCostume );
		szCloakMesh = PreCloakCostume->m_szCloakMesh;
	}
	else if ( pCloak )
	{
		szEquipEffect = pCloak->GetItemEquipEffect( m_bMale );
		szCloakMesh = pCloak->GetEquipMesh(NULL);
	}

	if( !szEquipEffect.IsEmpty() )
	{
		m_pGrp->EndEffect( szEquipEffect, false );
		szEquipEffect.Clear();
	}

	if ( !szCloakMesh.IsEmpty() )
	{
		RemoveEntityInOwner( szCloakMesh, true );
		szCloakMesh.Clear();
	}

	//스프링 로프 해제
	ioHashString szSpringRope;
	const RopeSpringParam *pSpringParam = NULL;

	if ( PreCloakCostume->m_nCostumeCode > 0 )
	{
		szSpringRope = PreCloakCostume->m_szSpring_Biped;
		pSpringParam = PreCloakCostume->m_pCostumeSpringParam;		
	}
	else if( pCloak && pCloak->HasSpringRope() )
	{
		szSpringRope = pCloak->GetSpringBiped();
		pSpringParam = pCloak->GetSpringParam();
	}

	if ( !szSpringRope.IsEmpty() && pSpringParam )
	{
		const CostumeInfo *pCheckArmorCostume = &m_CostumeSlot[CCS_ARMOR];
		const CostumeInfo *pCheckHelmetCostume = &m_CostumeSlot[CCS_HELMET];

		const ioArmorItem *pCheckArmor = ToArmorItemConst( GetSlotItem( CCS_ARMOR ) );
		const ioHelmetItem *pCheckHelmet = ToHelmetItemConst( GetSlotItem( CCS_HELMET ) );

		bool bArmorHas = true;
		bool bHelmetHas = true;

		if ( pCheckArmorCostume && pCheckArmorCostume->m_nCostumeCode > 0 && 
			( pCheckArmorCostume->m_szSpring_Biped.IsEmpty() || !pCheckArmorCostume->m_pCostumeSpringParam ) )
			bArmorHas = false;
		else if( !pCheckArmor || !pCheckArmor->HasSpringRope() )
			bArmorHas = false;

		if ( pCheckHelmetCostume && pCheckHelmetCostume->m_nCostumeCode > 0 && 
			( pCheckHelmetCostume->m_szSpring_Biped.IsEmpty() || !pCheckHelmetCostume->m_pCostumeSpringParam ) )
			bHelmetHas = false;
		else if( !pCheckHelmet || !pCheckHelmet->HasSpringRope() )
			bHelmetHas = false;

		if( m_pGrp && !bArmorHas && !bHelmetHas )
		{
			m_pGrp->RemoveRopeSpringCurve( szSpringRope );
		}
	}
}

DWORD ioCreateChar::GetEquipedItemCode( int iSlot ) const
{
	if( COMPARE( iSlot, CCS_WEAPON, MAX_CCS ) )
		return m_ItemSlot[iSlot].dwItemCode;

	return 0;
}

int ioCreateChar::GetClassType()
{
	int iClassType = m_ItemSlot[CCS_WEAPON].dwItemCode % 1000;
	
	return iClassType;
}

ioEntity *ioCreateChar::AddEntityToOwner( const ioHashString &rkMeshName, bool bSex )
{
	if( rkMeshName.IsEmpty() )	return NULL;
	if( !m_pGrp ) return NULL;

	if( bSex )
	{
		ioHashString szNewMeshName = Help::ConvertName( rkMeshName, m_bMale );
		if( m_bMale )
		{
			return m_pGrp->AddEntity( szNewMeshName, m_bThreadLoading );
		}
		else
		{
			char szFullName[MAX_PATH]="";
			StringCbPrintf( szFullName, sizeof( szFullName ), "resource\\mesh\\%s.msh" , szNewMeshName.c_str() );
			if( g_ResourceLoader.IsFileExist( szFullName ) )
			{
				return m_pGrp->AddEntity( szNewMeshName, m_bThreadLoading );
			}
			else // 없다면 남자 설정
			{
				szNewMeshName = Help::ConvertName( rkMeshName, true );
				return m_pGrp->AddEntity( szNewMeshName, m_bThreadLoading );
			}
		}
	}
	else
	{
		ioHashString szNewMeshName = Help::ConvertName( rkMeshName, true ); // 남성
		return m_pGrp->AddEntity( szNewMeshName, m_bThreadLoading );
	}
	return NULL;
}

bool ioCreateChar::RemoveEntityInOwner( const ioHashString &rkMeshName, bool bSex )
{
	if( rkMeshName.IsEmpty() )	return false;
	if( !m_pGrp ) return false;

	if( bSex )
	{
		ioHashString szNewMeshName = Help::ConvertName( rkMeshName, m_bMale );
		if( m_bMale )
		{
			return m_pGrp->DestroyEntity( szNewMeshName );
		}
		else
		{
			char szFullName[MAX_PATH]="";
			StringCbPrintf( szFullName, sizeof( szFullName ), "resource\\mesh\\%s.msh" , szNewMeshName.c_str() );
			if( g_ResourceLoader.IsFileExist( szFullName ) )
			{
				return m_pGrp->DestroyEntity( szNewMeshName );
			}
			else // 없다면 남자 설정
			{
				szNewMeshName = Help::ConvertName( rkMeshName, true );
				return m_pGrp->DestroyEntity( szNewMeshName );
			}
		}
	}
	else
	{
		ioHashString szNewMeshName = Help::ConvertName( rkMeshName, true ); // 남성
		return m_pGrp->DestroyEntity( szNewMeshName );
	}
}

void ioCreateChar::UpdateBound()
{
	if( m_pGrp && m_pGrp->IsAttached() )
	{
		ioSceneNode *pNode = m_pGrp->GetParentSceneNode();
		if( !pNode )
			return;
		m_WorldCollisionBox = m_pGrp->GetCollisionBox();
		m_WorldCollisionBox.Transform( pNode->GetDerivedPosition(),
									   pNode->GetDerivedScale(),
									   pNode->GetDerivedOrientation() );
	}
	else
	{
		m_WorldCollisionBox.SetNull();
	}
}

void ioCreateChar::SetPosition( float fXPos, float fYPos, float fZPos )
{
	SetPosition( D3DXVECTOR3( fXPos, fYPos, fZPos ) );
}

void ioCreateChar::SetPosition( const D3DXVECTOR3 &vPos )
{
	ioSceneNode *pSNode = m_pGrp->GetParentSceneNode();
	if( pSNode )
	{
		pSNode->SetPosition( vPos );
	}
}

void ioCreateChar::SetPitch( float fPitch )
{
	ioSceneNode *pSNode = m_pGrp->GetParentSceneNode();
	if( pSNode )
	{
		D3DXQUATERNION qtOrient;
		D3DXQuaternionRotationYawPitchRoll( &qtOrient, 0.0f, DEGtoRAD(fPitch), 0.0f );
		pSNode->SetOrientation( qtOrient );
	}
}

void ioCreateChar::SetYaw( float fYaw )
{
	ioSceneNode *pSNode = m_pGrp->GetParentSceneNode();
	if( pSNode )
	{
		D3DXQUATERNION qtOrient;
		D3DXQuaternionRotationYawPitchRoll( &qtOrient, DEGtoRAD( fYaw ), 0.0f, 0.0f );
		pSNode->SetOrientation( qtOrient );
	}
}

void ioCreateChar::RotateX( float fPitch )
{
	ioSceneNode *pSNode = m_pGrp->GetParentSceneNode();
	if( pSNode )
	{
		pSNode->PitchX( fPitch );
	}
}

void ioCreateChar::RotateY( float fYaw )
{
	ioSceneNode *pSNode = m_pGrp->GetParentSceneNode();
	if( pSNode )
	{
		pSNode->YawY( fYaw );
	}
}

void ioCreateChar::SetScale( float fScale )
{
	SetScale( D3DXVECTOR3( fScale, fScale, fScale ) );
}

void ioCreateChar::SetScale( const D3DXVECTOR3 &vScale )
{
	ioSceneNode *pSNode = m_pGrp->GetParentSceneNode();
	if( pSNode )
	{
		pSNode->SetScale( vScale );
	}
}

void ioCreateChar::SetScale( float fXScale, float fYScale, float fZScale )
{
	SetScale( D3DXVECTOR3( fXScale, fYScale, fZScale ) );
}

void ioCreateChar::SetActionAni( const ioHashString &szAniName, float fFadeIn, float fFadeOut )
{
	if( m_pGrp )
	{
		m_pGrp->ClearAllActionAni( 0.0f );
		m_pGrp->ClearAllLoopAni( 0.0f );

		m_pGrp->SetActionAni( szAniName, fFadeIn, fFadeOut );
	}
}

void ioCreateChar::SetLoopAni( const ioHashString &szAniName, float fFadeIn )
{
	if( m_pGrp )
	{
		m_pGrp->ClearAllActionAni( 0.0f );
		m_pGrp->ClearAllLoopAni( 0.0f );

		m_pGrp->SetLoopAni( szAniName, fFadeIn );
		m_pGrp->Update( 0.001f );
	}
}

void ioCreateChar::SetLoopAni( int iAniID, float fFadeIn /*= 0.0f */ )
{
	if( m_pGrp )
	{
		m_pGrp->ClearAllActionAni( 0.0f );
		m_pGrp->ClearAllLoopAni( 0.0f );

		m_pGrp->SetLoopAni( iAniID, fFadeIn );
		m_pGrp->Update( 0.001f );
	}
}

void ioCreateChar::ClearAllAnimations( bool bNotifyClearEvent )
{
	if( m_pGrp )
	{
		m_pGrp->ClearAllActionAni( 0.0f, bNotifyClearEvent );
		m_pGrp->ClearAllLoopAni( 0.0f, bNotifyClearEvent );
	}
}

bool ioCreateChar::SetWeaponShopMotion()
{
	if( !m_pGrp )	return false;

	DWORD dwWeaponCode = GetEquipedItemCode( CCS_WEAPON );

	const ioItem *pItem = g_ItemMaker.GetItemConst( dwWeaponCode, __FUNCTION__ );
	const ioWeaponItem *pWeapon = ToWeaponItemConst( pItem );
	if( pWeapon && !pWeapon->GetUIMotion().IsEmpty() )
	{
		SetLoopAni( GetAnimationIdxAfterConvertName( pWeapon->GetUIMotion(), m_bMale ) );
		return true;
	}

	return false;
}

ioHashString ioCreateChar::GetWeaponShopMotion()
{
	if( !m_pGrp ) return "";

	DWORD dwWeaponCode = GetEquipedItemCode( CCS_WEAPON );

	const ioItem *pItem = g_ItemMaker.GetItemConst( dwWeaponCode, __FUNCTION__ );
	const ioWeaponItem *pWeapon = ToWeaponItemConst( pItem );
	if( pWeapon && !pWeapon->GetUIMotion().IsEmpty() )
	{
		ioHashString szNewAniName = Help::ConvertTailName( pWeapon->GetUIMotion(), m_bMale );
		if( !m_bMale )
		{
			char szFullName[MAX_PATH]="";
			StringCbPrintf( szFullName, sizeof( szFullName ), "resource\\animation\\%s" , szNewAniName.c_str() );
			if( !g_ResourceLoader.IsFileExist( szFullName ) )
			{
				szNewAniName = Help::ConvertTailName( pWeapon->GetUIMotion(), true ); // 남자로
			}
		}
		return szNewAniName;
	}

	return "";
}

void ioCreateChar::SetAwardWeaponMotion( int iAniCode )
{
	if( !m_pGrp )	return;

	DWORD dwWeaponCode = GetEquipedItemCode( CCS_WEAPON );

	const ioItem *pItem = g_ItemMaker.GetItemConst( dwWeaponCode, __FUNCTION__ );
	const ioWeaponItem *pWeapon = ToWeaponItemConst( pItem );
	if( pWeapon )
	{
		const ioHashString& szAniMotion = pWeapon->GetAwardMotion( iAniCode );
		if( !szAniMotion.IsEmpty() )
			SetLoopAni( GetAnimationIdxAfterConvertName( szAniMotion, m_bMale ) );
		return;
	}

	return;
}

void ioCreateChar::SetMotionItemAni( const ioHashString &rkMotion )
{
	if( !m_pGrp ) return;

	int iAniID = m_pGrp->GetAnimationIdx( rkMotion );
	if( iAniID == -1 )
		return;

	m_MotionItemAniName = rkMotion;
	m_dwMotionItemAniMaxTime = 0;
	m_dwMotionItemAniCurTime = 0;
	m_bMotionItemDelay		 = false;

	m_pGrp->SetPlayActionCurTime( iAniID, 0.0f );
	m_pGrp->ClearAllActionAni( FLOAT100 );
	m_pGrp->ClearAllLoopAni( FLOAT100 );
	m_pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1 );

	std::list<float> cmdJumpAniTimeCnt;
	m_pGrp->GetAnimationEventTimeList_e( iAniID, "jump_anitime", cmdJumpAniTimeCnt );
	if( cmdJumpAniTimeCnt.empty() )
	{
		m_dwMotionItemAniCurTime  = FRAMEGETTIME();
		m_dwMotionItemAniMaxTime  = m_pGrp->GetAnimationFullTime( iAniID );
	}
	else
	{
		m_dwMotionItemAniCurTime  = FRAMEGETTIME();
		m_dwMotionItemAniMaxTime  = INFINITE;
	}
}

RaceDetailType ioCreateChar::GetRaceDetailType() const
{
	switch( m_RaceType )
	{
	case RT_HUMAN:
		if( m_bMale )
			return RDT_HUMAN_MAN;
		else
			return RDT_HUMAN_WOMAN;
		break;
	case RT_ELF:
		if( m_bMale )
			return RDT_ELF_MAN;
		else
			return RDT_ELF_WOMAN;
		break;
	case RT_DWARF:
		if( m_bMale )
			return RDT_DWARF_MAN;
		else
			return RDT_DWARF_WOMAN;
		break;
	}

	LOG.PrintTimeAndLog( 0, "ioCreateChar::GetRaceDetailType - UnknownType Race:%d, male:%d",
							m_RaceType, m_bMale );

	return RDT_HUMAN_MAN;
}


ioEffect* ioCreateChar::AttachEffect( const ioHashString &rkEffect, ioEntityEvent *pEntityEventOwner )
{
	if( !m_pGrp ) return NULL;

	if( !rkEffect.IsEmpty() )
		return m_pGrp->AttachEffect( rkEffect, pEntityEventOwner );

	return NULL;
}

void ioCreateChar::EndEffect( const ioHashString &rkEffect, bool bGrace )
{
	if( !m_pGrp ) return;

	m_pGrp->EndEffect( rkEffect, bGrace );
}

void ioCreateChar::DirectFaceChange( const char *szNewFaceName, int iDuration )
{
	m_dwFaceChangeTime = FRAMEGETTIME() + iDuration;

	//HARDCODE: ioBaseChar와 달리 얼굴타입(m_iFaceType)을 체크해서 파일명을 조정하지 않는다. ( 1번타입만.. )
	if( !m_DefaultFaceMesh.IsEmpty() )
	{
		ChangeFaceTexture( szNewFaceName );
	}
}

void ioCreateChar::FaceChange( const char *szTexPostfix, int iDuration, const ioHashString &szType )
{
	ioHashString szNewFaceName;
	if( !GetFaceChangeName( szNewFaceName, szTexPostfix, m_bMale, m_iFaceType ) )
		return;

	if( !m_bMale )
	{
		char szFullName[MAX_PATH]="";
		StringCbPrintf( szFullName, sizeof( szFullName ), "resource\\texture\\%s" , szNewFaceName.c_str() );
		if( !g_ResourceLoader.IsFileExist( szFullName ) )
		{
			if( !GetFaceChangeName( szNewFaceName, szTexPostfix, true, m_iFaceType ) ) // 남자로 설정.
				return;
		}
	}

	m_dwFaceChangeTime = FRAMEGETTIME() + iDuration;

	//HARDCODE: ioBaseChar와 달리 얼굴타입(m_iFaceType)을 체크해서 파일명을 조정하지 않는다. ( 1번타입만.. )
	if( !m_CurFaceMesh.IsEmpty() )
	{
		ChangeFaceTexture( szNewFaceName );
	}
}

void ioCreateChar::CheckFaceChanging()
{
	if( m_dwFaceChangeTime == 0 )
		return;

	if( m_dwFaceChangeTime < FRAMEGETTIME() )
	{
		StopFaceChange();
	}
}

void ioCreateChar::StopFaceChange()
{
	if( m_dwFaceChangeTime != 0 )
	{
		RestoreDefaultFace();
		m_dwFaceChangeTime = 0;
	}
}

void ioCreateChar::UpdateDefaultFaceTextureName()
{
	ioEntity *pFace = m_pGrp->GetEntity( m_CurFaceMesh );
	if( !pFace ) return;

	ioTechnique *pTech = pFace->GetSubEntity(0)->GetMaterial()->GetBestTechnique();

	int iNumPass = pTech->GetNumPasses();
	for( int iPass=0 ; iPass<iNumPass ; iPass++ )
	{
		ioPass *pPass = pTech->GetPass( iPass );

		int iNumStage = pPass->GetNumTextureUnitStates();
		for( int iStage=0 ; iStage<iNumStage ; iStage++ )
		{
			ioTextureUnitState *pStage = pPass->GetTextureUnitState( iStage );

			if( !pStage->IsUseLightTexture() && !pStage->GetTextureName().IsEmpty() )
			{
				m_DefaultFaceTexture = pStage->GetTextureName();
				return;
			}
		}
	}

	m_DefaultFaceTexture.Clear();
}

void ioCreateChar::ChangeFaceTexture( const ioHashString &szNewFaceTexture )
{
	Help::FaceChangeColorTexAndOutLine( m_pGrp->GetEntity( m_CurFaceMesh ), szNewFaceTexture );
}

void ioCreateChar::RestoreDefaultFace()
{
	if( !m_CurFaceMesh.IsEmpty() )
	{
		ChangeFaceTexture( m_DefaultFaceTexture );
	}
}

void ioCreateChar::DestroyHandAndBody()
{
	if( !m_pGrp )	return;

	int iNumEntity = m_pGrp->GetNumEntities();
	
	ioHashStringVec vNameList;
	vNameList.reserve( iNumEntity );

	for( int i=0 ; i<iNumEntity ; i++ )
	{
		ioEntity *pEntity = m_pGrp->GetEntity( i );
		if( pEntity )
		{
			if( strstr( pEntity->GetMeshName().c_str(), "_body") ||
				strstr( pEntity->GetMeshName().c_str(), "_hand") )
			{
				vNameList.push_back( pEntity->GetMeshName() );
			}			
		}
	}

	ioHashStringVec::iterator iter;
	for( iter=vNameList.begin() ; iter!=vNameList.end() ; ++iter )
	{
		m_pGrp->DestroyEntity( *iter );
	}
}

bool ioCreateChar::DestroyEntity( const ioHashString &szName )
{
	if( m_pGrp )
		return m_pGrp->DestroyEntity( szName );

	return false;
}

bool ioCreateChar::DestroyEntity( int iIndex )
{
	if( m_pGrp )
		return m_pGrp->DestroyEntity( iIndex );

	return false;
}

void ioCreateChar::ChangeHairOutLineThick( float fThick )
{
	int iNumEntity = m_pGrp->GetNumEntities();
	for( int i=0 ; i<iNumEntity ; i++ )
	{
		ioEntity *pEntity = m_pGrp->GetEntity( i );

		if( strstr( pEntity->GetMeshName().c_str(), "_head" ) )
		{
			ApplyOutLineByChangedThick( pEntity, fThick );
		}
	}
}

void ioCreateChar::ChangeHeadOutLineThick( float fThick )
{
	int iNumEntity = m_pGrp->GetNumEntities();
	for( int i=0 ; i<iNumEntity ; i++ )
	{
		ioEntity *pEntity = m_pGrp->GetEntity( i );

		if( strstr( pEntity->GetMeshName().c_str(), "_face" ) )
		{
			ApplyOutLineByChangedThick( pEntity, fThick );
		}
	}
}

void ioCreateChar::RestoreOutLineThick()
{
	int iNumEntity = m_pGrp->GetNumEntities();
	for( int i=0 ; i<iNumEntity ; i++ )
	{
		ioEntity *pEntity = m_pGrp->GetEntity( i );

		int iNumSubEntity = pEntity->GetNumSubEntity();
		for( int j=0 ; j<iNumSubEntity ; j++ )
		{
			ioSubEntity *pSubEntity = pEntity->GetSubEntity( j );
			if( !pSubEntity )	continue;

			ioTechnique *pCloneTech = NULL;
			ioMaterial *pClone = pSubEntity->GetMaterial();
			if( pClone )
			{
				pCloneTech = pClone->GetBestTechnique();
			}

			ioTechnique *pSrcTech = NULL;
			ioMaterial *pSrc = ioMaterialManager::GetSingleton().GetMaterial( pSubEntity->GetMaterialName() );
			if( pSrc )
			{
				pSrcTech = pSrc->GetBestTechnique();
			}

			if( !pSrcTech || !pCloneTech )
				continue;

			ioPass *pClonePass = pCloneTech->GetPassByShaderGrp( SGT_OUTLINE );
			ioPass *pSrcPass = pSrcTech->GetPassByShaderGrp( SGT_OUTLINE );

			if( !pClonePass || !pSrcPass )
				continue;

			ioShaderCustomArg *pArg = pSrcPass->GetCustomArg( "OutLineThickness" );
			if( pArg )
			{
				pClonePass->AddCustomParameter( *pArg );
			}
		}
	}
}

void ioCreateChar::ApplyOutLineByChangedThick( ioEntity *pEntity, float fThick )
{
	if( !pEntity )	return;

	ioShaderCustomArg kLineThickness( "OutLineThickness" );
	kLineThickness.SetValue( fThick );

	int iNumSubEntity = pEntity->GetNumSubEntity();
	for( int j=0 ; j<iNumSubEntity ; j++ )
	{
		ioSubEntity *pSubEntity = pEntity->GetSubEntity( j );

		ioMaterial *pMaterial = pSubEntity->GetMaterial();
		if( !pMaterial ) continue;

		ioTechnique *pTech = pMaterial->GetBestTechnique();
		if( !pTech ) continue;

		ioPass *pOutLine = pTech->GetPassByShaderGrp( SGT_OUTLINE );
		if( pOutLine )
		{
			pOutLine->AddCustomParameter( kLineThickness );
		}
	}
}

D3DXVECTOR3 ioCreateChar::GetWorldPosition() const
{
	return m_pGrp->GetParentNode()->GetDerivedPosition();
}

D3DXVECTOR3 ioCreateChar::GetMidPositionByRate( float fRate ) const
{
	D3DXVECTOR3 vPos = GetWorldPosition();
	ioOrientBox kBox = GetWorldCollisionBox();

	float fMidHeight;
	fMidHeight = kBox.GetCenter().y + kBox.GetExtents(1) - vPos.y;
	fMidHeight = vPos.y + fMidHeight * fRate;
	vPos.y = fMidHeight;

	return vPos;
}

float ioCreateChar::GetYaw()
{
	ioSceneNode *pSNode = m_pGrp->GetParentSceneNode();
	if( pSNode )
	{
		return RADtoDEG( ioMath::QuaterToYaw( pSNode->GetOrientation() ) );
	}

	return 0.0f;
}

int ioCreateChar::GetAnimationIdxAfterConvertName( const ioHashString &szAniName, bool bMale ) const
{
	ioHashString szNewAniName = Help::ConvertTailName( szAniName, bMale );

	if( !bMale )
	{
		char szFullName[MAX_PATH]="";
		StringCbPrintf( szFullName, sizeof( szFullName ), "resource\\animation\\%s" , szNewAniName.c_str() );
		if( !g_ResourceLoader.IsFileExist( szFullName ) )
		{
			szNewAniName = Help::ConvertTailName( szAniName, true ); // 남자로
		}
	}

	return m_pGrp->GetAnimationIdx( szNewAniName );
}

void ioCreateChar::ReloadEquipItem( DWORD dwItemCode, DWORD dwMaleCustom, DWORD dwFemaleCustom )
{
	if( dwItemCode == 0 ) return;

	int i  = 0;
	bool bChangeItem = false;
	EquipItemInfo kItemSlot[MAX_CCS];
	for(i = 0;i < MAX_CCS;i++)
	{
		if( m_ItemSlot[i].dwItemCode == dwItemCode )
		{
			kItemSlot[i].dwItemCode = dwItemCode;
			kItemSlot[i].dwItemMaleCustom = dwMaleCustom;
			kItemSlot[i].dwItemFemaleCustom = dwFemaleCustom;
			bChangeItem = true;
		}
		else
		{
			kItemSlot[i].dwItemCode = m_ItemSlot[i].dwItemCode;
			kItemSlot[i].dwItemMaleCustom = m_ItemSlot[i].dwItemMaleCustom;
			kItemSlot[i].dwItemFemaleCustom = m_ItemSlot[i].dwItemFemaleCustom;
		}
	}

	if( bChangeItem )
	{
		ReleaseAllEquipItems();
		for(i = 0;i < MAX_CCS;i++)
		{
			EquipItem( kItemSlot[i].dwItemCode, kItemSlot[i].dwItemMaleCustom, kItemSlot[i].dwItemFemaleCustom );
		}
	}
}

void ioCreateChar::ShowFaceMesh( bool bShow )
{
	m_pGrp->SetEntityVisible( m_CurFaceMesh, bShow );

	m_pGrp->SetEntityVisible( m_DefaultHairMesh, bShow );
	m_pGrp->SetEntityVisible( m_DefaultBindHairMesh, bShow );
	m_pGrp->SetEntityVisible( m_DefaultHalfHairMesh, bShow );

	/*
	if( bShow )
		AttachEffect( m_FaceEffect, NULL );
	else
		m_pGrp->EndEffect( m_FaceEffect, false );
	*/
}

void ioCreateChar::ReleaseEquipEffect( int iSlot )
{
	const ioItem *pItem = GetSlotItem( iSlot );
	if( !pItem )	return;

	ioHashString szEquipEffect = pItem->GetItemEquipEffect( m_bMale );
	if( !szEquipEffect.IsEmpty() )
	{
		m_pGrp->EndEffect( szEquipEffect, false);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioCreateChar::SetCostumeSlot( int nSlot, int nCostumeCode )
{
	if( !COMPARE( nSlot, CCS_WEAPON, MAX_CCS ) )
		return;

	g_CostumeInfoMgr.GetCostumeInfo( nCostumeCode, m_CostumeSlot[nSlot] );
}

const CostumeInfo* ioCreateChar::GetCostumeSlot( int nSlot )
{
	if( !COMPARE( nSlot, CCS_WEAPON, MAX_CCS ) )
		return NULL;

	return &m_CostumeSlot[nSlot];
}

ioHashString ioCreateChar::GetCostumeEffect( const CostumeInfo* pCostumeInfo )
{
	ioHashString szEquipEffect;
	if ( !pCostumeInfo )
		return szEquipEffect;

	if( m_bMale )
		szEquipEffect = pCostumeInfo->m_szEffect;
	else if( pCostumeInfo->m_szEffectHW.IsEmpty() )
		szEquipEffect = pCostumeInfo->m_szEffect;
	else
		szEquipEffect = pCostumeInfo->m_szEffectHW;

	return szEquipEffect;
}