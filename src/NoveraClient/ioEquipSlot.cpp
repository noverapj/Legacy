

#include "stdafx.h"
#include "ioEquipSlot.h"

#include "ItemDefine.h"
#include "BuffDefine.h"
#include "ioAccessoryInfoManager.h"

ioEquipSlot::ioEquipSlot( ioBaseChar *pOwner )
{
	memset( m_EquipItemSlot, 0, sizeof(m_EquipItemSlot) );
	memset( m_EquipSkillSlot, 0, sizeof(m_EquipSkillSlot) );
	memset( m_EquipSkillSlotW, 0, sizeof(m_EquipSkillSlotW) );
	memset( m_EquipAccessoryCompoundSkillSlot, 0, sizeof(m_EquipAccessoryCompoundSkillSlot) );

	m_pOwnerChar = pOwner;

	InitDefaultMeshNameList();

	m_iSetItemLev = 0;
	m_WearDropRate = 100;

	for( int i=0 ; i<MAX_EQUIP_SLOT ; i++ )
	{
		m_DropWaitSlot[i] = false;
		m_EquipWaitSlot[i] = false;

		m_ItemSlot[i].dwItemCode = 0;
		m_ItemSlot[i].dwItemMaleCustom = m_ItemSlot[i].dwItemFemaleCustom = 0;
		m_ItemSlot[i].vCustomTexList.clear();
	}
}

ioEquipSlot::~ioEquipSlot()
{
	for( int i=0 ; i<MAX_EQUIP_SLOT ; i++ )
	{
		SAFEDELETE( m_EquipItemSlot[i] );
		SAFEDELETE( m_EquipSkillSlot[i] );
		SAFEDELETE( m_EquipSkillSlotW[i] );
	}
	for( int i=0 ; i<MAX_ACCESSORY_SLOT ; i++ )
	{
		SAFEDELETE( m_EquipAccessoryCompoundSkillSlot[i] );
	}
}

void ioEquipSlot::ChangeDefaultFace( const ioHashString &szFace, const ioHashString &szEffect )
{
	if( m_DefaultFaceMesh == szFace )
		return;

	bool bMale = true;
	if( m_pOwnerChar )
		bMale = m_pOwnerChar->IsMale();

	bool bDefaultFace = false;

	if( m_pOwnerChar )
		bDefaultFace = m_pOwnerChar->HasEntity(m_DefaultFaceMesh);


	ioHelmetWearItem *pWear = ToHelmetWearItem(m_EquipItemSlot[ES_WEAR]);	
	ioHelmetItem *pHelmet = ToHelmetItem( m_EquipItemSlot[ES_HELMET] );

	// 기존것 제거
	m_pOwnerChar->EndEffect( m_FaceEffect, false );
	if( pWear && !bDefaultFace )
	{
		switch( pWear->GetChangeFace() )
		{
		case ioHelmetWearItem::HWF_NONE:
			RemoveEntityInOwner( m_DefaultFaceMesh );
			break;
		case ioHelmetWearItem::HWF_HALF_SKIP:
			RemoveEntityInOwner( m_SkipFaceMesh );
			break;
		}
	}
	else if( pHelmet && !bDefaultFace )
	{
		switch( pHelmet->GetChangeFace( bMale ) )
		{
		case ioHelmetItem::HF_NONE:
			RemoveEntityInOwner( m_DefaultFaceMesh );
			break;
		case ioHelmetItem::HF_HALF_SKIP:
			RemoveEntityInOwner( m_SkipFaceMesh );
			break;
		}
	}
	else
	{
		RemoveEntityInOwner( m_DefaultFaceMesh );
	}

	// Mesh 갱신
	m_DefaultFaceMesh = szFace;

	char szHalfFace[MAX_PATH];
	wsprintf_e( szHalfFace, "skip_%s", szFace );
	m_SkipFaceMesh = szHalfFace;

	char szFullName[MAX_PATH]="";
	StringCbPrintf( szFullName, sizeof( szFullName ), "resource\\mesh\\%s.msh" , m_SkipFaceMesh.c_str() );
	if( !g_ResourceLoader.IsFileExist( szFullName ) )
	{
		// half가 없으면 기본으로
		m_SkipFaceMesh = szFace;
	}

	// Mesh 변경
	if( pWear && !bDefaultFace )
	{
		switch( pWear->GetChangeFace() )
		{
		case ioHelmetWearItem::HWF_NONE:
			AddEntityToOwner( m_DefaultFaceMesh );
			m_CurFaceMesh = m_DefaultFaceMesh;
			break;
		case ioHelmetWearItem::HWF_HALF_SKIP:
			AddEntityToOwner( m_SkipFaceMesh );
			m_CurFaceMesh = m_SkipFaceMesh;
			break;
		}
	}
	else if( pHelmet && !bDefaultFace )
	{
		switch( pHelmet->GetChangeFace( bMale ) )
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

	m_FaceEffect = szEffect;
	m_pOwnerChar->AttachEffect( m_FaceEffect );
	
	// 얼굴 텍스쳐 변경
	UpdateDefaultFaceTextureName();
}

void ioEquipSlot::ChangeDefaultHair( const ioHashString &szHair )
{
	if( m_DefaultHairMesh == szHair )
		return;

	bool bMale = true;
	if( m_pOwnerChar )
		bMale = m_pOwnerChar->IsMale();

	bool bDefaultHair = false;
	
	if( m_pOwnerChar )
		bDefaultHair = m_pOwnerChar->HasEntity(m_DefaultHairMesh);
	
	ioHelmetWearItem *pWear = ToHelmetWearItem(m_EquipItemSlot[ES_WEAR]);	
	ioHelmetItem *pHelmet = ToHelmetItem( m_EquipItemSlot[ES_HELMET] );

	if( pWear && !bDefaultHair )
	{
		switch( pWear->GetChangeHair() )
		{
		case ioHelmetWearItem::HWH_HALF_SKIP:
			RemoveEntityInOwner( m_DefaultHalfHairMesh );
			break;
		case ioHelmetWearItem::HWH_ALL_SKIP:
			RemoveEntityInOwner( m_DefaultHairMesh );
			break;
		case ioHelmetWearItem::HWH_BIND_SKIP:
			RemoveEntityInOwner( m_DefaultBindHairMesh );
			break;
		case ioHelmetWearItem::HWH_NO_SKIP:
			RemoveEntityInOwner( m_DefaultHairMesh );
			break;
		}
	}
	else if( pHelmet && !bDefaultHair )
	{
		switch( pHelmet->GetChangeHair( bMale ) )
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

	if( pWear && !bDefaultHair )
	{
		switch( pWear->GetChangeHair() )
		{
		case ioHelmetWearItem::HWH_HALF_SKIP:
			AddEntityToOwner( m_DefaultHalfHairMesh );
			break;
		case ioHelmetWearItem::HWH_ALL_SKIP:
			// 추가 안해도 된다
			break;
		case ioHelmetWearItem::HWH_BIND_SKIP:
			AddEntityToOwner( m_DefaultBindHairMesh );
			break;
		case ioHelmetWearItem::HWH_NO_SKIP:
			AddEntityToOwner( m_DefaultHairMesh );
			break;
		}
	}
	else if( pHelmet && !bDefaultHair )
	{
		switch( pHelmet->GetChangeHair( bMale ) )
		{
		case ioHelmetItem::HH_HALF_SKIP:
			AddEntityToOwner( m_DefaultHalfHairMesh );
			break;
		case ioHelmetItem::HH_ALL_SKIP:
			// 추가 안해도 된다
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
}

void ioEquipSlot::ChangeDefaultBeard( const ioHashString &szBeard )
{
	if( m_DefaultBeardMesh == szBeard )
		return;

	RemoveEntityInOwner( m_DefaultBeardMesh );

	m_DefaultBeardMesh = szBeard;
	AddEntityToOwner( m_DefaultBeardMesh );
}

void ioEquipSlot::ChangeDefaultAccessory( const ioHashString &szAccessory )
{
	if( m_DefaultAccessoryMesh == szAccessory )
		return;

	RemoveEntityInOwner( m_DefaultAccessoryMesh );

	m_DefaultAccessoryMesh = szAccessory;
	AddEntityToOwner( m_DefaultAccessoryMesh );
}

void ioEquipSlot::ChangeDefaultUnderwear( const ioHashString &szUnderwear )
{
	if( m_DefaultUnderwearMesh == szUnderwear )
		return;

	ioArmorItem *pArmor = ToArmorItem( m_EquipItemSlot[ES_ARMOR] );

	bool bDefaultMesh = false;
	if( m_pOwnerChar )
		bDefaultMesh = m_pOwnerChar->HasEntity(m_DefaultUnderwearMesh);	

	if( pArmor && !bDefaultMesh )
	{
		m_DefaultUnderwearMesh = szUnderwear;
		return;
	}

	RemoveEntityInOwner( m_DefaultUnderwearMesh );

	m_DefaultUnderwearMesh = szUnderwear;
	AddEntityToOwner( m_DefaultUnderwearMesh );
}

void ioEquipSlot::AddEntityEvent( ioEntity *pEntity )
{
	if( !pEntity ) return;
	if( !m_pOwnerChar ) return;

	for(int i = ES_WEAPON;i <= ES_CLOAK;i++)
	{
		EquipItemInfo &rkSlot = m_ItemSlot[i];

		DWORD dwCustomIndex = 0;
		if( m_pOwnerChar->IsMale() && rkSlot.dwItemMaleCustom > 0 )
			dwCustomIndex = rkSlot.dwItemMaleCustom;
		if( !m_pOwnerChar->IsMale() && rkSlot.dwItemFemaleCustom  > 0 )
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
					if( g_CustomTextureMgr.IsExistenceFile( m_pOwnerChar->GetCharName(), kOriginalList[iOri].m_szTextureName, dwCustomIndex, kCustomList[iOri].m_dwCustomSeq ) )
					{						
						Help::CustomEntityTexture( pEntity, kOriginalList[iOri].m_szTextureName, kCustomList[iOri].m_szTextureName, kCustomList[iOri].m_dwCustomSeq );

						m_pOwnerChar->ItemEntityChangeAfterUIProcess( i + 1 );   // +1이 아이템 타입
					}
				}
			}
		}
	}
}

ioItem* ioEquipSlot::EquipItem( ioItem *pItem, bool bThread )
{
	ioItem *pPreItem = NULL;

	switch( pItem->GetType() )
	{
	case ioItem::IT_WEAPON:
		pPreItem = EquipWeapon( pItem, bThread );
		break;
	case ioItem::IT_ARMOR:
		pPreItem = EquipArmor( pItem, bThread );
		break;
	case ioItem::IT_HELMET:
		pPreItem = EquipHelmet( pItem, bThread );
		break;
	case ioItem::IT_CLOAK:
		pPreItem = EquipCloak( pItem, bThread );
		break;
	case ioItem::IT_OBJECT:
		pPreItem = EquipObject( pItem, bThread );
		break;
	case ioItem::IT_WEAR:
		pPreItem = EquipWear( pItem, bThread );
		break;
	//Accessory
	case ioItem::IT_RING:
		pPreItem = EquipRing( pItem, bThread );
		break;
	case ioItem::IT_NECKLACE:
		pPreItem = EquipNecklace( pItem, bThread );
		break;
	case ioItem::IT_BRACELET:
		pPreItem = EquipBracelet( pItem, bThread );
		break;
	}

	if( m_pOwnerChar )
	{
		m_pOwnerChar->ApplyModifiedMaterial();
		m_pOwnerChar->ItemEquipAfterUIProcess( pItem->GetType() );
	}

	return pPreItem;
}

ioItem* ioEquipSlot::ReleaseItem( EquipSlot eSlot )
{
	ioItem *pPreItem = NULL;
	switch( eSlot )
	{
	case ES_WEAPON:
		pPreItem = ReleaseWeaponSlot();
		break;
	case ES_ARMOR:
		pPreItem = ReleaseArmorSlot();
		break;
	case ES_HELMET:
		pPreItem = ReleaseHelmetSlot();
		break;
	case ES_CLOAK:
		pPreItem = ReleaseCloakSlot();
		break;
	case ES_OBJECT:
		pPreItem = ReleaseObjectSlot();
		break;
	case ES_WEAR:
		pPreItem = ReleaseWearSlot();
		break;
	//Accessory
	case ES_RING:
		pPreItem = ReleaseRingSlot();
		break;
	case ES_NECKLACE:
		pPreItem = ReleaseNecklaceSlot();
		break;
	case ES_BRACELET:
		pPreItem = ReleaseBraceletSlot();
		break;
	}

	if( m_pOwnerChar )
	{
		m_pOwnerChar->ApplyModifiedMaterial();
		m_pOwnerChar->ItemReleaseAfterUIProcess( eSlot + 1 );      // +1아이템 타입
	}

	return pPreItem;
}

ioItem* ioEquipSlot::ReleaseItem( DWORD dwItemCode, int iItemCreateIndex )
{
	for( int i=0 ; i<MAX_EQUIP_SLOT ; i++ )
	{
		ioItem *pItem = m_EquipItemSlot[i];
		if( !pItem )	continue;

		if( pItem->GetItemCode() == dwItemCode &&
			pItem->GetItemCreateIndex() == iItemCreateIndex )
		{
			return ReleaseItem( (EquipSlot)i );
		}
	}

	return NULL;
}

int ioEquipSlot::CheckDropItem( bool bMove )
{
	bool bWeapon, bArmor, bHelmet, bCloak;
	bWeapon = bArmor = bHelmet = bCloak = false;

	if( m_EquipItemSlot[ES_WEAPON] && !m_DropWaitSlot[ES_WEAPON] )
		bWeapon = true;

	if( m_EquipItemSlot[ES_ARMOR] && !m_DropWaitSlot[ES_ARMOR] )
		bArmor = true;

	if( m_EquipItemSlot[ES_HELMET] && !m_DropWaitSlot[ES_HELMET] )
		bHelmet = true;

	if( m_EquipItemSlot[ES_CLOAK] && !m_DropWaitSlot[ES_CLOAK] )
		bCloak = true;

	int iSlot = -1;

	if( !bMove )
		g_DropManager.DropItemCheck( m_pOwnerChar, bWeapon, bArmor, bHelmet, bCloak, false );
	else
	{
		iSlot = g_DropManager.DropItemCheck2( bWeapon, bArmor, bHelmet, bCloak );
	}

	return iSlot;
}

int ioEquipSlot::CheckArmorDropItem()
{
	bool bArmor = false;
	int iSlot = -1;

	if( m_EquipItemSlot[ES_ARMOR] && !m_DropWaitSlot[ES_ARMOR] )
		bArmor = true;

	if( bArmor )
		return ES_ARMOR;

	return iSlot;
}

EquipSlot ioEquipSlot::CheckDropWearItem(bool bDie)
{
	if(bDie && !m_DropWaitSlot[ES_WEAR] )
		return ES_WEAR;	

	int iDropRate = ( rand() % 100 );
	if( (iDropRate < m_WearDropRate) && !m_DropWaitSlot[ES_WEAR] )
		return ES_WEAR;

	return ES_UNKNOWN;
}

EquipSlot ioEquipSlot::CheckDropObjectItem( bool bDie )
{
	if( !m_DropWaitSlot[ES_OBJECT] )
		return ES_OBJECT;

	return ES_UNKNOWN;
}

bool ioEquipSlot::IsCustomItemCheck( DWORD dwCustomIndex, bool bMale )
{
	for(int i = 0;i < MAX_EQUIP_SLOT;i++)
	{
		if( bMale )
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

void ioEquipSlot::EquipSlotData( int iSlot, DWORD dwItemCode, DWORD dwItemMaleCustom, DWORD dwItemFemaleCustom )
{
	if( COMPARE( iSlot, ES_WEAPON, MAX_EQUIP_SLOT ) )
	{
		m_ItemSlot[iSlot].dwItemCode = dwItemCode;
		m_ItemSlot[iSlot].dwItemMaleCustom = dwItemMaleCustom;
		m_ItemSlot[iSlot].dwItemFemaleCustom = dwItemFemaleCustom;
		m_ItemSlot[iSlot].vCustomTexList.clear();
	}
}

void ioEquipSlot::EquipSlotEntity( int iSlot, ioEntity *pEntity )
{
	if( !pEntity ) return;
	if( !m_pOwnerChar ) return;

	if( COMPARE( iSlot, ES_WEAPON, MAX_EQUIP_SLOT ) )
	{
		if( m_pOwnerChar->IsMale() && m_ItemSlot[iSlot].dwItemMaleCustom > 0 || !m_pOwnerChar->IsMale() && m_ItemSlot[iSlot].dwItemFemaleCustom > 0 )
		{
			Help::CustomEntityTextureNameCnt( pEntity, 0, m_ItemSlot[iSlot].vCustomTexList );
			AddEntityEvent( pEntity );
		}
	}
}

ioItem* ioEquipSlot::EquipWeapon( ioItem *pItem, bool bThread )
{
	ioItem *pPreItem = ReleaseItem( ES_WEAPON );
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );

	if( pItem->GetItemMaleCustom() > 0 || pItem->GetItemFemaleCustom() > 0 )
		bThread = false;    // 커스텀 되어있으면 즉시 로딩

	EquipSlotData( ES_WEAPON, pItem->GetItemCode(), pItem->GetItemMaleCustom(), pItem->GetItemFemaleCustom() );
	EquipSlotEntity( ES_WEAPON, AddEntityToOwner( pWeapon->GetEquipMesh(m_pOwnerChar), true, bThread ) );

	if( ToHookItem( pItem ) )
	{
		ioHashString szHand = GetCurHandMesh();
		if( !szHand.IsEmpty() )
		{
			if( szHand == m_DefaultHandMesh )
				RemoveEntityInOwner( m_DefaultHandMesh );
			else
				RemoveEntityInOwner( szHand, true );
		}
	}

	m_EquipItemSlot[ES_WEAPON] = pItem;
	AddEquipedSkill( ES_WEAPON, pItem );

	pItem->OnEquiped( m_pOwnerChar );

	UpdateSetItemLev();
	m_pOwnerChar->RecalcStatusValue();

	return pPreItem;
}

ioItem* ioEquipSlot::EquipArmor( ioItem *pItem, bool bThread )
{
	ioArmorItem *pArmor = ToArmorItem( pItem );
	ioArmorItem *pPreItem = ToArmorItem( m_EquipItemSlot[ ES_ARMOR ] );
	if( pPreItem )
	{
		RemoveEquipedSkillBuffInOwner( ES_ARMOR );
		RemoveEquipedSkill( ES_ARMOR );
		pPreItem->OnReleased( m_pOwnerChar );

		if( pPreItem->HasSpringRope() )
		{
			ioHelmetItem *pCheckHelmet = ToHelmetItem( m_EquipItemSlot[ES_HELMET] );
			ioCloakItem *pCheckCloak = ToCloakItem( m_EquipItemSlot[ES_CLOAK] );

			bool bHelmetHas = true;
			bool bCloakHas = true;

			if( !pCheckHelmet || !pCheckHelmet->HasSpringRope() )
				bHelmetHas = false;

			if( !pCheckCloak || !pCheckCloak->HasSpringRope() )
				bCloakHas = false;

			if( !bHelmetHas && !bCloakHas )
				m_pOwnerChar->RemoveRopeSpring( pPreItem->GetSpringBiped() );
		}
	}

	if( pItem->GetItemMaleCustom() > 0 || pItem->GetItemFemaleCustom() > 0 )
		bThread = false;    // 커스텀 되어있으면 즉시 로딩

	EquipSlotData( ES_ARMOR, pItem->GetItemCode(), pItem->GetItemMaleCustom(), pItem->GetItemFemaleCustom() );
	EquipSlotEntity( ES_ARMOR, AddEntityToOwner( pArmor->GetBodyMesh(m_pOwnerChar), true, bThread ) );

	if( pPreItem )
	{
		RemoveEntityInOwner( pPreItem->GetBodyMesh(m_pOwnerChar), true );
	}
	else
	{
		RemoveEntityInOwner( m_DefaultUnderwearMesh );
	}

	// about hands....
	if( !ToHookItem(GetWeapon()) )
	{
		if( pArmor->HasHandMesh(m_pOwnerChar) )
		{
			EquipSlotEntity( ES_ARMOR, AddEntityToOwner( pArmor->GetHandMesh(m_pOwnerChar), true, bThread ) );

			if( pPreItem && pPreItem->HasHandMesh(m_pOwnerChar) )
			{
				RemoveEntityInOwner( pPreItem->GetHandMesh(m_pOwnerChar), true );
			}
			else
			{
				RemoveEntityInOwner( m_DefaultHandMesh );
			}
		}
		else
		{
			if( pPreItem && pPreItem->HasHandMesh(m_pOwnerChar) )
			{
				AddEntityToOwner( m_DefaultHandMesh, false, bThread );
				RemoveEntityInOwner( pPreItem->GetHandMesh(m_pOwnerChar), true );
			}
		}
	}

	m_EquipItemSlot[ES_ARMOR] = pItem;

	AddEquipedSkill( ES_ARMOR, pItem );
	pItem->OnEquiped( m_pOwnerChar );

	UpdateSetItemLev();
	m_pOwnerChar->RecalcStatusValue();

	if( pArmor->HasSpringRope() )
	{
		m_pOwnerChar->AddRopeSpring( pArmor->GetSpringBiped(), pArmor->GetSpringParam() );
	}

	return pPreItem;
}

ioItem* ioEquipSlot::EquipHelmet( ioItem *pItem, bool bThread )
{
	ioHelmetItem *pHelmet = ToHelmetItem( pItem );

	bool bMale = true;
	if( m_pOwnerChar )
		bMale = m_pOwnerChar->IsMale();

	ioHelmetItem::HelmetHair eNewHairType = pHelmet->GetChangeHair( bMale );
	ioHelmetItem::HelmetFace eNewFaceType = pHelmet->GetChangeFace( bMale );

	ioHelmetWearItem *pWear = ToHelmetWearItem(m_EquipItemSlot[ES_WEAR]);	
	ioItem *pPreItem = m_EquipItemSlot[ES_HELMET];

	EquipSlotData( ES_HELMET, pItem->GetItemCode(), pItem->GetItemMaleCustom(), pItem->GetItemFemaleCustom() );

	if( pPreItem )
	{
		RemoveEquipedSkillBuffInOwner( ES_HELMET );
		RemoveEquipedSkill( ES_HELMET );
		pPreItem->OnReleased( m_pOwnerChar );

		ioHelmetItem *pPreHelmet = ToHelmetItem( pPreItem );
		if( pPreHelmet && pPreHelmet->HasSpringRope() )
		{
			ioArmorItem *pCheckArmor = ToArmorItem( m_EquipItemSlot[ES_ARMOR] );
			ioCloakItem *pCheckCloak = ToCloakItem( m_EquipItemSlot[ES_CLOAK] );

			bool bArmorHas = true;
			bool bCloakHas = true;

			if( !pCheckArmor || !pCheckArmor->HasSpringRope() )
				bArmorHas = false;

			if( !pCheckCloak || !pCheckCloak->HasSpringRope() )
				bCloakHas = false;

			if( !bArmorHas && !bCloakHas )
				m_pOwnerChar->RemoveRopeSpring( pPreHelmet->GetSpringBiped() );
		}
	}

	if( !pWear )
	{
		if( !pPreItem )
		{
			switch( eNewHairType )
			{
			case ioHelmetItem::HH_HALF_SKIP:
				AddEntityToOwner( m_DefaultHalfHairMesh, false, bThread );
				RemoveEntityInOwner( m_DefaultHairMesh );
				break;
			case ioHelmetItem::HH_ALL_SKIP:
				RemoveEntityInOwner( m_DefaultHairMesh );
				break;
			case ioHelmetItem::HH_BIND_SKIP:
				AddEntityToOwner( m_DefaultBindHairMesh, false, bThread );
				RemoveEntityInOwner( m_DefaultHairMesh );
				break;
			case ioHelmetItem::HH_NO_SKIP:
				break;
			}

			switch( eNewFaceType )
			{
			case ioHelmetItem::HF_HALF_SKIP:
				AddEntityToOwner( m_SkipFaceMesh, false, bThread );
				RemoveEntityInOwner( m_DefaultFaceMesh );
				m_CurFaceMesh = m_SkipFaceMesh;
				break;
			}
		}
		else
		{
			ioHelmetItem *pPreHelmet = ToHelmetItem( pPreItem );
			RemoveEntityInOwner( pPreHelmet->GetEquipMesh(m_pOwnerChar), true );

			ioHelmetItem::HelmetHair eOldHairType = pPreHelmet->GetChangeHair( bMale );
			ioHelmetItem::HelmetFace eOldFaceType = pPreHelmet->GetChangeFace( bMale );
			switch( eNewHairType )
			{
			case ioHelmetItem::HH_HALF_SKIP:
				if( eOldHairType == ioHelmetItem::HH_ALL_SKIP )
				{
					AddEntityToOwner( m_DefaultHalfHairMesh, false, bThread );
				}
				else if( eOldHairType == ioHelmetItem::HH_BIND_SKIP )
				{
                    AddEntityToOwner( m_DefaultHalfHairMesh, false, bThread );
					RemoveEntityInOwner( m_DefaultBindHairMesh );
				}
				else if( eOldHairType == ioHelmetItem::HH_NO_SKIP )
				{
					AddEntityToOwner( m_DefaultHalfHairMesh, false, bThread );
					RemoveEntityInOwner( m_DefaultHairMesh );
				}
				break;
			case ioHelmetItem::HH_ALL_SKIP:
				if( eOldHairType == ioHelmetItem::HH_HALF_SKIP )
				{
					RemoveEntityInOwner( m_DefaultHalfHairMesh );
				}
				else if( eOldHairType == ioHelmetItem::HH_BIND_SKIP )
				{
					RemoveEntityInOwner( m_DefaultBindHairMesh );
				}
				else if( eOldHairType == ioHelmetItem::HH_NO_SKIP )
				{
					RemoveEntityInOwner( m_DefaultHairMesh );
				}
				break;
			case ioHelmetItem::HH_BIND_SKIP:
				if( eOldHairType == ioHelmetItem::HH_HALF_SKIP )
				{
					AddEntityToOwner( m_DefaultBindHairMesh, false, bThread );
					RemoveEntityInOwner( m_DefaultHalfHairMesh );
				}
				else if( eOldHairType == ioHelmetItem::HH_ALL_SKIP )
				{
					AddEntityToOwner( m_DefaultBindHairMesh, false, bThread );
				}
				else if( eOldHairType == ioHelmetItem::HH_NO_SKIP )
				{
					AddEntityToOwner( m_DefaultBindHairMesh, false, bThread );
					RemoveEntityInOwner( m_DefaultHairMesh );
				}
				break;
			case ioHelmetItem::HH_NO_SKIP:
				if( eOldHairType == ioHelmetItem::HH_ALL_SKIP )
				{
					AddEntityToOwner( m_DefaultHairMesh, false, bThread );
				}
				else if( eOldHairType == ioHelmetItem::HH_HALF_SKIP )
				{
					AddEntityToOwner( m_DefaultHairMesh, false, bThread );
					RemoveEntityInOwner( m_DefaultHalfHairMesh );
				}
				else if( eOldHairType == ioHelmetItem::HH_BIND_SKIP )
				{
					AddEntityToOwner( m_DefaultHairMesh, false, bThread );
					RemoveEntityInOwner( m_DefaultBindHairMesh );
				}
				break;
			}

			switch( eNewFaceType )
			{
			case ioHelmetItem::HF_NONE:
				if( eOldFaceType == ioHelmetItem::HF_HALF_SKIP )
				{
					AddEntityToOwner( m_DefaultFaceMesh, false, bThread );
					RemoveEntityInOwner( m_SkipFaceMesh );
					m_CurFaceMesh = m_DefaultFaceMesh;
				}
				break;
			case ioHelmetItem::HF_HALF_SKIP:
				if( eOldFaceType == ioHelmetItem::HF_NONE )
				{
					AddEntityToOwner( m_SkipFaceMesh, false, bThread );
					RemoveEntityInOwner( m_DefaultFaceMesh );
					m_CurFaceMesh = m_SkipFaceMesh;
				}
				break;
			}
		}

		if( pItem->GetItemMaleCustom() > 0 || pItem->GetItemFemaleCustom() > 0 )
			bThread = false;    // 커스텀 되어있으면 즉시 로딩

		EquipSlotEntity( ES_HELMET, AddEntityToOwner( pHelmet->GetEquipMesh(m_pOwnerChar), true, bThread ) );
	}

	m_EquipItemSlot[ES_HELMET] = pItem;
	
	AddEquipedSkill( ES_HELMET, pItem );
	pItem->OnEquiped( m_pOwnerChar );

	UpdateSetItemLev();
	m_pOwnerChar->RecalcStatusValue();

	if( pHelmet && pHelmet->HasSpringRope() )
	{
		m_pOwnerChar->AddRopeSpring( pHelmet->GetSpringBiped(), pHelmet->GetSpringParam() );
	}

	return pPreItem;
}

ioItem* ioEquipSlot::EquipCloak( ioItem *pItem, bool bThread )
{
	ioItem *pPreItem = ReleaseItem( ES_CLOAK );
	ioCloakItem *pCloak = ToCloakItem( pItem );

	if( pItem->GetItemMaleCustom() > 0 || pItem->GetItemFemaleCustom() > 0 )
		bThread = false;    // 커스텀 되어있으면 즉시 로딩

	EquipSlotData( ES_CLOAK, pItem->GetItemCode(), pItem->GetItemMaleCustom(), pItem->GetItemFemaleCustom() );
	EquipSlotEntity( ES_CLOAK, AddEntityToOwner( pCloak->GetEquipMesh(m_pOwnerChar), true, bThread ) );
	m_EquipItemSlot[ES_CLOAK] = pItem;

	AddEquipedSkill( ES_CLOAK, pItem );
	pItem->OnEquiped( m_pOwnerChar );

	UpdateSetItemLev();
	m_pOwnerChar->RecalcStatusValue();

	if( pCloak->HasSpringRope() )
	{
		m_pOwnerChar->AddRopeSpring( pCloak->GetSpringBiped(), pCloak->GetSpringParam() );
	}

	return pPreItem;
}

ioItem* ioEquipSlot::EquipObject( ioItem *pItem, bool bThread )
{
	ioItem *pPreItem = ReleaseItem( ES_OBJECT );
	ioObjectItem *pObject = ToObjectItem( pItem );

	AddEntityToOwner( pObject->GetEquipMesh(m_pOwnerChar), false , bThread );
	m_EquipItemSlot[ES_OBJECT] = pItem;

	AddEquipedSkill( ES_OBJECT, pItem );
	pItem->OnEquiped( m_pOwnerChar );
	
	ioWeaponItem *pWeapon = GetWeapon();
	if( pWeapon )
	{
		if( m_pOwnerChar )
			m_pOwnerChar->ShowWeaponItemMesh( false );
	}

	UpdateSetItemLev();
	m_pOwnerChar->RecalcStatusValue();

	return pPreItem;
}

ioItem* ioEquipSlot::EquipWear( ioItem *pItem, bool bThread )
{
	// helmet wear
	if(ToHelmetWearItem( pItem ) 
	   && !m_EquipItemSlot[ES_WEAR]) // helmet + wear
	{
		return EquipHelmetWearOnHelmet( pItem );
	}
	else if(ToHelmetWearItem( pItem ) 
	      && m_EquipItemSlot[ES_WEAR]
		  && ToHelmetWearItem( m_EquipItemSlot[ES_WEAR] ) ) // helmet + wear + wear
	{
		return EquipHelmetWearOnWear( pItem );
	}
	else if( ToCloakWearItem( pItem ) 
		  && !m_EquipItemSlot[ES_WEAR] )
	{
		return EquipCloakWearOnCloak( pItem );
	}

	LOG.PrintTimeAndLog( 0, "ioEquipSlot::EquipWear - Error");
	return NULL;
}

ioItem* ioEquipSlot::EquipHelmetWearOnHelmet( ioItem *pItem)
{
	ioHelmetWearItem *pHelmetWear = ToHelmetWearItem( pItem );
	if( !pHelmetWear ) return NULL;

	ioHelmetWearItem::HelmetWearHair eWearHair = pHelmetWear->GetChangeHair();
	ioHelmetWearItem::HelmetWearFace eWearFace = pHelmetWear->GetChangeFace();

	ioHelmetItem *pHelmet = ToHelmetItem( m_EquipItemSlot[ES_HELMET] );
	if( !pHelmet )
	{
		switch( eWearHair )
		{
		case ioHelmetWearItem::HWH_HALF_SKIP:
			AddEntityToOwner( m_DefaultHalfHairMesh );
			RemoveEntityInOwner( m_DefaultHairMesh );
			break;
		case ioHelmetWearItem::HWH_ALL_SKIP:
			RemoveEntityInOwner( m_DefaultHairMesh );
			break;
		case ioHelmetWearItem::HWH_BIND_SKIP:
			AddEntityToOwner( m_DefaultBindHairMesh );
			RemoveEntityInOwner( m_DefaultHairMesh );
			break;
		case ioHelmetWearItem::HWH_NO_SKIP:
			break;
		}

		switch( eWearFace )
		{
		case ioHelmetWearItem::HWF_HALF_SKIP:
			AddEntityToOwner( m_SkipFaceMesh );
			RemoveEntityInOwner( m_DefaultFaceMesh );
			m_CurFaceMesh = m_SkipFaceMesh;
			break;
		}
	}
	else
	{
		ioHelmetItem::InvisibleFaceType eType = pHelmet->GetInvisibleFaceType();

		switch( eType )
		{
		case ioHelmetItem::IFT_ALL:
			m_pOwnerChar->ShowFaceMesh( true );
			break;
		case ioHelmetItem::IFT_MALE:
			if( m_pOwnerChar->IsMale() )
				m_pOwnerChar->ShowFaceMesh( true );
			break;
		case ioHelmetItem::IFT_FEMALE:
			if( !m_pOwnerChar->IsMale() )
				m_pOwnerChar->ShowFaceMesh( true );
			break;
		}

		bool bMale = true;
		if( m_pOwnerChar )
			bMale = m_pOwnerChar->IsMale();

		ioHashString szEquipEffect = pHelmet->GetItemEquipEffect(bMale);
		if( !szEquipEffect.IsEmpty() )
			m_pOwnerChar->EndEffect( szEquipEffect, false );

		RemoveEntityInOwner( pHelmet->GetEquipMesh(m_pOwnerChar), true );

		ioHelmetItem::HelmetHair eHelmetHair = pHelmet->GetChangeHair( bMale );
		ioHelmetItem::HelmetFace eHelmetFace = pHelmet->GetChangeFace( bMale );

		switch( eWearHair )
		{
		case ioHelmetWearItem::HWH_HALF_SKIP:
			if( eHelmetHair == ioHelmetItem::HH_ALL_SKIP )
			{
				AddEntityToOwner( m_DefaultHalfHairMesh );
			}
			else if( eHelmetHair == ioHelmetItem::HH_BIND_SKIP )
			{
				AddEntityToOwner( m_DefaultHalfHairMesh );
				RemoveEntityInOwner( m_DefaultBindHairMesh );
			}
			else if( eHelmetHair == ioHelmetItem::HH_NO_SKIP )
			{
				AddEntityToOwner( m_DefaultHalfHairMesh );
				RemoveEntityInOwner( m_DefaultHairMesh );
			}
			break;
		case ioHelmetWearItem::HWH_ALL_SKIP:
			if( eHelmetHair == ioHelmetItem::HH_HALF_SKIP )
			{
				RemoveEntityInOwner( m_DefaultHalfHairMesh );
			}
			else if( eHelmetHair == ioHelmetItem::HH_BIND_SKIP )
			{
				RemoveEntityInOwner( m_DefaultBindHairMesh );
			}
			else if( eHelmetHair == ioHelmetItem::HH_NO_SKIP )
			{
				RemoveEntityInOwner( m_DefaultHairMesh );
			}
			break;
		case ioHelmetWearItem::HWH_BIND_SKIP:
			if( eHelmetHair == ioHelmetItem::HH_HALF_SKIP )
			{
				AddEntityToOwner( m_DefaultBindHairMesh );
				RemoveEntityInOwner( m_DefaultHalfHairMesh );
			}
			else if( eHelmetHair == ioHelmetItem::HH_ALL_SKIP )
			{
				AddEntityToOwner( m_DefaultBindHairMesh );
			}
			else if( eHelmetHair == ioHelmetItem::HH_NO_SKIP )
			{
				AddEntityToOwner( m_DefaultBindHairMesh );
				RemoveEntityInOwner( m_DefaultHairMesh );
			}
			break;
		case ioHelmetWearItem::HWH_NO_SKIP:
			if( eHelmetHair == ioHelmetItem::HH_HALF_SKIP )
			{
				AddEntityToOwner( m_DefaultHairMesh );
				RemoveEntityInOwner( m_DefaultHalfHairMesh );
			}
			else if( eHelmetHair == ioHelmetItem::HH_ALL_SKIP )
			{
				AddEntityToOwner( m_DefaultHairMesh );
			}
			else if( eHelmetHair == ioHelmetItem::HH_BIND_SKIP )
			{
				AddEntityToOwner( m_DefaultHairMesh );
				RemoveEntityInOwner( m_DefaultBindHairMesh );
			}
			break;
		}

		switch( eWearFace )
		{
		case ioHelmetWearItem::HWF_NONE:
			if( eHelmetFace == ioHelmetItem::HF_HALF_SKIP )
			{
				AddEntityToOwner( m_DefaultFaceMesh );
				RemoveEntityInOwner( m_SkipFaceMesh );
				m_CurFaceMesh = m_DefaultFaceMesh;
			}
			break;
		case ioHelmetWearItem::HWF_HALF_SKIP:
			if( eHelmetFace == ioHelmetItem::HF_NONE )
			{
				AddEntityToOwner( m_SkipFaceMesh );
				RemoveEntityInOwner( m_DefaultFaceMesh );
				m_CurFaceMesh = m_SkipFaceMesh;
			}
			break;
		}
	}
	
	pItem->OnEquiped( m_pOwnerChar );
	AddEntityToOwner( pHelmetWear->GetEquipMesh(m_pOwnerChar) );
	
	m_EquipItemSlot[ES_WEAR] = pItem;
	AddEquipedSkill( ES_WEAR, pItem );
	
	UpdateSetItemLev();
	m_pOwnerChar->RecalcStatusValue();
	
	return NULL;
}

ioItem* ioEquipSlot::EquipHelmetWearOnWear( ioItem *pItem )
{
	ioHelmetWearItem *pHelmetWear = ToHelmetWearItem( pItem );
	if( !pHelmetWear )	return NULL;

	ioItem *pPreItem = m_EquipItemSlot[ES_WEAR];
	if( pPreItem )
	{
		RemoveEquipedSkillBuffInOwner( ES_WEAR );
		RemoveEquipedSkill( ES_WEAR );
		pPreItem->OnReleased( m_pOwnerChar );
	}

	ioHelmetWearItem::HelmetWearHair eNewHair = pHelmetWear->GetChangeHair();
	ioHelmetWearItem::HelmetWearFace eNewFace = pHelmetWear->GetChangeFace();
	
	if( !pPreItem )
	{
		switch( eNewHair )
		{
		case ioHelmetWearItem::HWH_HALF_SKIP:
			AddEntityToOwner( m_DefaultHalfHairMesh );
			RemoveEntityInOwner( m_DefaultHairMesh );
			break;
		case ioHelmetWearItem::HWH_ALL_SKIP:
			RemoveEntityInOwner( m_DefaultHairMesh );
			break;
		case ioHelmetWearItem::HWH_BIND_SKIP:
			AddEntityToOwner( m_DefaultBindHairMesh );
			RemoveEntityInOwner( m_DefaultHairMesh );
			break;
		case ioHelmetWearItem::HWH_NO_SKIP:
			break;
		}

		switch( eNewFace )
		{
		case ioHelmetWearItem::HWF_HALF_SKIP:
			AddEntityToOwner( m_SkipFaceMesh );
			RemoveEntityInOwner( m_DefaultFaceMesh );
			m_CurFaceMesh = m_SkipFaceMesh;
			break;
		}
	}
	else
	{
		ioHelmetWearItem *pPreHelmetWear = ToHelmetWearItem( pPreItem );
		RemoveEntityInOwner( pPreHelmetWear->GetEquipMesh(m_pOwnerChar) );

		ioHelmetWearItem::HelmetWearHair eOldHair = pPreHelmetWear->GetChangeHair();
		ioHelmetWearItem::HelmetWearFace eOldFace = pPreHelmetWear->GetChangeFace();

		switch( eNewHair )
		{
		case ioHelmetWearItem::HWH_HALF_SKIP:
			if( eOldHair == ioHelmetWearItem::HWH_ALL_SKIP )
			{
				AddEntityToOwner( m_DefaultHalfHairMesh );
			}
			else if( eOldHair == ioHelmetWearItem::HWH_BIND_SKIP )
			{
				AddEntityToOwner( m_DefaultHalfHairMesh );
				RemoveEntityInOwner( m_DefaultBindHairMesh );
			}
			else if( eOldHair == ioHelmetWearItem::HWH_NO_SKIP )
			{
				AddEntityToOwner( m_DefaultHalfHairMesh );
				RemoveEntityInOwner( m_DefaultHairMesh );
			}
			break;
		case ioHelmetWearItem::HWH_ALL_SKIP:
			if( eOldHair == ioHelmetWearItem::HWH_HALF_SKIP )
			{
				RemoveEntityInOwner( m_DefaultHalfHairMesh );
			}
			else if( eOldHair == ioHelmetWearItem::HWH_BIND_SKIP )
			{
				RemoveEntityInOwner( m_DefaultBindHairMesh );
			}
			else if( eOldHair == ioHelmetWearItem::HWH_NO_SKIP )
			{
				RemoveEntityInOwner( m_DefaultHairMesh );
			}
			break;
		case ioHelmetWearItem::HWH_BIND_SKIP:
			if( eOldHair == ioHelmetWearItem::HWH_HALF_SKIP )
			{
				AddEntityToOwner( m_DefaultBindHairMesh );
				RemoveEntityInOwner( m_DefaultHalfHairMesh );
			}
			else if( eOldHair == ioHelmetWearItem::HWH_ALL_SKIP )
			{
				AddEntityToOwner( m_DefaultBindHairMesh );
			}
			else if( eOldHair == ioHelmetWearItem::HWH_NO_SKIP )
			{
				AddEntityToOwner( m_DefaultBindHairMesh );
				RemoveEntityInOwner( m_DefaultHairMesh );
			}
			break;
		case ioHelmetWearItem::HWH_NO_SKIP:
			if( eOldHair == ioHelmetWearItem::HWH_HALF_SKIP )
			{
				AddEntityToOwner( m_DefaultHairMesh );
				RemoveEntityInOwner( m_DefaultHalfHairMesh );
			}
			else if( eOldHair == ioHelmetWearItem::HWH_ALL_SKIP )
			{
				AddEntityToOwner( m_DefaultHairMesh );
			}
			else if( eOldHair == ioHelmetWearItem::HWH_BIND_SKIP )
			{
				AddEntityToOwner( m_DefaultHairMesh );
				RemoveEntityInOwner( m_DefaultBindHairMesh );
			}
			break;
		}

		switch( eNewFace )
		{
		case ioHelmetWearItem::HWF_NONE:
			if( eOldFace == ioHelmetWearItem::HWF_HALF_SKIP )
			{
				AddEntityToOwner( m_DefaultFaceMesh );
				RemoveEntityInOwner( m_SkipFaceMesh );
				m_CurFaceMesh = m_DefaultFaceMesh;
			}
			break;
		case ioHelmetWearItem::HWF_HALF_SKIP:
			if( eOldFace == ioHelmetWearItem::HWF_NONE )
			{
				AddEntityToOwner( m_SkipFaceMesh );
				RemoveEntityInOwner( m_DefaultFaceMesh );
				m_CurFaceMesh = m_SkipFaceMesh;
			}
			break;
		}
	}
	
	AddEntityToOwner( pHelmetWear->GetEquipMesh(m_pOwnerChar) );
	
	m_EquipItemSlot[ES_WEAR] = pItem;
	AddEquipedSkill( ES_WEAR, pItem );
	pItem->OnEquiped( m_pOwnerChar );
	
	UpdateSetItemLev();
	m_pOwnerChar->RecalcStatusValue();
	
	return pPreItem;
}

ioItem* ioEquipSlot::EquipCloakWearOnCloak( ioItem *pItem)
{
	ioCloakWearItem *pCloakWear = ToCloakWearItem( pItem );
	if( !pCloakWear ) return NULL;

	pItem->OnEquiped( m_pOwnerChar );
	AddEntityToOwner( pCloakWear->GetEquipMesh(m_pOwnerChar) );

	// 변신시 해당 아이템을 계속 화면에 출력할지 여부
	ioEntityGroup *pGroup = m_pOwnerChar->GetGroup();
	if( pGroup )
	{
		ioEntity *pEntity = pGroup->GetEntity( pCloakWear->GetEquipMesh(m_pOwnerChar) );
		if( pEntity )
			pEntity->SetVisibleChangePass( pCloakWear->IsVisibleChangePass() );
	}

	m_EquipItemSlot[ES_WEAR] = pItem;
	AddEquipedSkill( ES_WEAR, pItem );

	UpdateSetItemLev();
	m_pOwnerChar->RecalcStatusValue();

	return NULL;
}

void ioEquipSlot::ReleaseSlotData( int iSlot )
{
	if( COMPARE( iSlot, ES_WEAPON, MAX_EQUIP_SLOT ) )
	{
		m_ItemSlot[iSlot].dwItemCode = 0;
		m_ItemSlot[iSlot].dwItemMaleCustom = 0;
		m_ItemSlot[iSlot].dwItemFemaleCustom = 0;
		m_ItemSlot[iSlot].vCustomTexList.clear();
	}
}

ioItem* ioEquipSlot::ReleaseWeaponSlot()
{
	ioItem *pPreItem = m_EquipItemSlot[ES_WEAPON];
	if( pPreItem )
	{
		ReleaseSlotData( ES_WEAPON );
		RemoveEquipedSkillBuffInOwner( ES_WEAPON );
		RemoveEquipedSkill( ES_WEAPON );
		pPreItem->OnReleased( m_pOwnerChar );

		ioWeaponItem *pWeapon = ToWeaponItem( pPreItem );
		if( pWeapon && !RemoveEntityInOwner( pWeapon->GetEquipMesh(m_pOwnerChar), true ) )
		{
			if( !ToSuddenAttackItem( pWeapon ) && !ToShaolinItem( pWeapon ) )
			{
				LOG.PrintTimeAndLog( 0, "ReleaseWeapon - %s Char %s Entity Destory Failed",
					m_pOwnerChar->GetCharName().c_str(),
					pWeapon->GetEquipMesh(m_pOwnerChar).c_str() );
			}
		}

		if( ToHookItem( pPreItem ) )
		{
			ioArmorItem *pArmor = GetArmor();
			if( pArmor && pArmor->HasHandMesh(m_pOwnerChar) )
			{
				AddEntityToOwner( pArmor->GetHandMesh(m_pOwnerChar), true );
			}
			else
			{
				AddEntityToOwner( m_DefaultHandMesh );
			}
		}

		m_EquipItemSlot[ES_WEAPON] = NULL;

		UpdateSetItemLev();
		m_pOwnerChar->RecalcStatusValue();
		m_pOwnerChar->SetCurNormalAttackItem( NULL );
	}

	return pPreItem;
}

ioItem* ioEquipSlot::ReleaseArmorSlot()
{
	ioArmorItem *pArmor = ToArmorItem( m_EquipItemSlot[ES_ARMOR] );
	if( pArmor )
	{
		ReleaseSlotData( ES_ARMOR );
		RemoveEquipedSkillBuffInOwner( ES_ARMOR );
		RemoveEquipedSkill( ES_ARMOR );
		pArmor->OnReleased( m_pOwnerChar );

		AddEntityToOwner( m_DefaultUnderwearMesh );
		RemoveEntityInOwner( pArmor->GetBodyMesh(m_pOwnerChar), true );

		if( !ToHookItem( GetWeapon() ) )
		{
			if( pArmor->HasHandMesh(m_pOwnerChar) )
			{
				AddEntityToOwner( m_DefaultHandMesh );
				RemoveEntityInOwner( pArmor->GetHandMesh(m_pOwnerChar), true );
			}
		}

		m_EquipItemSlot[ES_ARMOR] = NULL;

		UpdateSetItemLev();
		m_pOwnerChar->RecalcStatusValue();

		if( pArmor->HasSpringRope() )
		{
			ioHelmetItem *pCheckHelmet = ToHelmetItem( m_EquipItemSlot[ES_HELMET] );
			ioCloakItem *pCheckCloak = ToCloakItem( m_EquipItemSlot[ES_CLOAK] );

			bool bHelmetHas = true;
			bool bCloakHas = true;

			if( !pCheckHelmet || !pCheckHelmet->HasSpringRope() )
				bHelmetHas = false;

			if( !pCheckCloak || !pCheckCloak->HasSpringRope() )
				bCloakHas = false;

			if( !bHelmetHas && !bCloakHas )
				m_pOwnerChar->RemoveRopeSpring( pArmor->GetSpringBiped() );
		}
	}

	return pArmor;
}

bool ioEquipSlot::IsHideHairMeshTiming()
{
	if( m_pOwnerChar && m_pOwnerChar->HasBuff(BT_CHANGE_MESH) )
	{
		ioChangeMeshBuff* pCMB = ToChangeMeshBuff( m_pOwnerChar->GetBuff(BT_CHANGE_MESH) );
		if( pCMB && pCMB->GetMaintainHideFaceMeshOption() )
			return true;
	}
	return false;
}

ioItem* ioEquipSlot::ReleaseHelmetSlot()
{
	ioItem *pPreItem = m_EquipItemSlot[ES_HELMET];
	if( pPreItem )
	{
		bool bMale = true;
		if( m_pOwnerChar )
			bMale = m_pOwnerChar->IsMale();

		ReleaseSlotData( ES_HELMET );
		RemoveEquipedSkillBuffInOwner( ES_HELMET );
		RemoveEquipedSkill( ES_HELMET );
		pPreItem->OnReleased( m_pOwnerChar );

		ioHelmetItem *pPreHelmet = ToHelmetItem( pPreItem );
		RemoveEntityInOwner( pPreHelmet->GetEquipMesh(m_pOwnerChar), true );

		ioHelmetWearItem *pWear = ToHelmetWearItem(m_EquipItemSlot[ES_WEAR]);	
		if( !pWear )
		{
			if( !m_pOwnerChar->IsInvisibleHead() )
			{
				switch( pPreHelmet->GetChangeHair( bMale ) )
				{
				case ioHelmetItem::HH_HALF_SKIP:
					AddEntityToOwner( m_DefaultHairMesh );
					RemoveEntityInOwner( m_DefaultHalfHairMesh );
					break;
				case ioHelmetItem::HH_ALL_SKIP:
					AddEntityToOwner( m_DefaultHairMesh );
					break;
				case ioHelmetItem::HH_BIND_SKIP:
					AddEntityToOwner( m_DefaultHairMesh );
					RemoveEntityInOwner( m_DefaultBindHairMesh );
					break;
				case ioHelmetItem::HH_NO_SKIP:
					break;
				}

				if( IsHideHairMeshTiming() )
				{
					ioEntityGroup* pGroup = m_pOwnerChar->GetGroup();
					if(pGroup)
					{
						ioEntity *pEntity = pGroup->GetEntity( m_DefaultHairMesh );
						pEntity->SetVisible( false );
					}
				}

				switch( pPreHelmet->GetChangeFace( bMale ) )
				{
				case ioHelmetItem::HF_HALF_SKIP:
					AddEntityToOwner( m_DefaultFaceMesh );
					RemoveEntityInOwner( m_SkipFaceMesh );
					m_CurFaceMesh = m_DefaultFaceMesh;
					break;
				}
			}
		}

		m_EquipItemSlot[ES_HELMET] = NULL;

		UpdateSetItemLev();
		m_pOwnerChar->RecalcStatusValue();
	}

	ioHelmetItem *pHelmet = ToHelmetItem( pPreItem );
	if( pHelmet && pHelmet->HasSpringRope() )
	{
		ioArmorItem *pCheckArmor = ToArmorItem( m_EquipItemSlot[ES_ARMOR] );
		ioCloakItem *pCheckCloak = ToCloakItem( m_EquipItemSlot[ES_CLOAK] );

		bool bArmorHas = true;
		bool bCloakHas = true;

		if( !pCheckArmor || !pCheckArmor->HasSpringRope() )
			bArmorHas = false;

		if( !pCheckCloak || !pCheckCloak->HasSpringRope() )
			bCloakHas = false;

		if( !bArmorHas && !bCloakHas )
			m_pOwnerChar->RemoveRopeSpring( pHelmet->GetSpringBiped() );
	}

	return pPreItem;
}

ioItem* ioEquipSlot::ReleaseCloakSlot()
{
	ioItem *pPreItem = m_EquipItemSlot[ES_CLOAK];
	if( pPreItem )
	{
		ReleaseSlotData( ES_CLOAK );
		RemoveEquipedSkillBuffInOwner( ES_CLOAK );
		RemoveEquipedSkill( ES_CLOAK );
		pPreItem->OnReleased( m_pOwnerChar );

		ioCloakItem *pCloak = ToCloakItem( pPreItem );
		if( pCloak && pCloak->HasSpringRope() )
		{
			ioArmorItem *pCheckArmor = ToArmorItem( m_EquipItemSlot[ES_ARMOR] );
			ioHelmetItem *pCheckHelmet = ToHelmetItem( m_EquipItemSlot[ES_HELMET] );

			bool bArmorHas = true;
			bool bHelmetHas = true;

			if( !pCheckArmor || !pCheckArmor->HasSpringRope() )
				bArmorHas = false;

			if( !pCheckHelmet || !pCheckHelmet->HasSpringRope() )
				bHelmetHas = false;

			if( !bArmorHas && !bHelmetHas )
				m_pOwnerChar->RemoveRopeSpring( pCloak->GetSpringBiped() );
		}

		RemoveEntityInOwner( pCloak->GetEquipMesh(m_pOwnerChar), true );
		m_EquipItemSlot[ES_CLOAK] = NULL;

		UpdateSetItemLev();
		m_pOwnerChar->RecalcStatusValue();
	}

	return pPreItem;
}

ioItem* ioEquipSlot::ReleaseObjectSlot()
{
	ioItem *pPreItem = m_EquipItemSlot[ES_OBJECT];
	if( pPreItem )
	{
		RemoveEquipedSkillBuffInOwner( ES_OBJECT );
		RemoveEquipedSkill( ES_OBJECT );
		pPreItem->OnReleased( m_pOwnerChar );

		ioObjectItem *pObject = ToObjectItem( pPreItem );
		if( pObject && !RemoveEntityInOwner( pObject->GetEquipMesh(m_pOwnerChar) ) )
		{
			LOG.PrintTimeAndLog( 0, "ReleaseWeapon - %s Char %s Entity Destory Failed",
									m_pOwnerChar->GetCharName().c_str(),
									pObject->GetEquipMesh(m_pOwnerChar).c_str() );
		}

		ioWeaponItem *pWeapon = GetWeapon();
		if( pWeapon )
		{
			if( m_pOwnerChar )
				m_pOwnerChar->ShowWeaponItemMesh( true );

			pWeapon->ReleaseObjectState( m_pOwnerChar, pObject );
		}
		m_EquipItemSlot[ES_OBJECT] = NULL;

		UpdateSetItemLev();
		m_pOwnerChar->RecalcStatusValue();
	}

	return pPreItem;
}

ioItem* ioEquipSlot::ReleaseWearSlot()
{
	ioItem *pPreItem = m_EquipItemSlot[ES_WEAR];

	if( !pPreItem )
		return NULL;

	RemoveEquipedSkillBuffInOwner( ES_WEAR );
	RemoveEquipedSkill( ES_WEAR );
	pPreItem->OnReleased( m_pOwnerChar );

	if( ToHelmetWearItem( pPreItem ))
	{
		ioHelmetWearItem *pPreHelmetWear = ToHelmetWearItem( pPreItem );
		RemoveEntityInOwner( pPreHelmetWear->GetEquipMesh(m_pOwnerChar) );

		ioHelmetWearItem::HelmetWearHair eWearHair = pPreHelmetWear->GetChangeHair();
		ioHelmetWearItem::HelmetWearFace eWearFace = pPreHelmetWear->GetChangeFace();

		// set hair
		if( !m_EquipItemSlot[ES_HELMET] )
		{
			if( !m_pOwnerChar->IsInvisibleHead() )
			{
				switch( eWearHair )
				{
				case ioHelmetWearItem::HWH_HALF_SKIP:
					AddEntityToOwner( m_DefaultHairMesh );
					RemoveEntityInOwner( m_DefaultHalfHairMesh );
					break;
				case ioHelmetWearItem::HWH_ALL_SKIP:
					AddEntityToOwner( m_DefaultHairMesh );
					break;
				case ioHelmetWearItem::HWH_BIND_SKIP:
					AddEntityToOwner( m_DefaultHairMesh );
					RemoveEntityInOwner( m_DefaultBindHairMesh );
					break;
				case ioHelmetWearItem::HWH_NO_SKIP:
					break;
				}

				switch( eWearFace )
				{
				case ioHelmetWearItem::HWF_HALF_SKIP:
					AddEntityToOwner( m_DefaultFaceMesh );
					RemoveEntityInOwner( m_SkipFaceMesh );
					m_CurFaceMesh = m_DefaultFaceMesh;
					break;
				}
			}
		}
		// set helmet
		else
		{
			bool bMale = true;
			if( m_pOwnerChar )
				bMale = m_pOwnerChar->IsMale();

			ioHelmetItem *pHelmet = ToHelmetItem( m_EquipItemSlot[ES_HELMET] );
			ioHelmetItem::HelmetHair eHelmetHair = pHelmet->GetChangeHair( bMale );
			ioHelmetItem::HelmetFace eHelmetFace = pHelmet->GetChangeFace( bMale );

			if( !m_pOwnerChar->IsInvisibleHead() )
			{
				switch( eHelmetHair )
				{
				case ioHelmetItem::HH_HALF_SKIP:
					if( eWearHair == ioHelmetWearItem::HWH_ALL_SKIP )
					{
						AddEntityToOwner( m_DefaultHalfHairMesh );
					}
					else if( eWearHair == ioHelmetWearItem::HWH_BIND_SKIP )
					{
						AddEntityToOwner( m_DefaultHalfHairMesh );
						RemoveEntityInOwner( m_DefaultBindHairMesh );
					}
					else if( eWearHair == ioHelmetWearItem::HWH_NO_SKIP )
					{
						AddEntityToOwner( m_DefaultHalfHairMesh );
						RemoveEntityInOwner( m_DefaultHairMesh );
					}
					break;
				case ioHelmetItem::HH_ALL_SKIP:
					if( eWearHair == ioHelmetWearItem::HWH_HALF_SKIP )
					{
						RemoveEntityInOwner( m_DefaultHalfHairMesh );
					}
					else if( eWearHair == ioHelmetWearItem::HWH_BIND_SKIP )
					{
						RemoveEntityInOwner( m_DefaultBindHairMesh );
					}
					else if( eWearHair == ioHelmetWearItem::HWH_NO_SKIP )
					{
						RemoveEntityInOwner( m_DefaultHairMesh );
					}
					break;
				case ioHelmetItem::HH_BIND_SKIP:
					if( eWearHair == ioHelmetWearItem::HWH_HALF_SKIP )
					{
						AddEntityToOwner( m_DefaultBindHairMesh );
						RemoveEntityInOwner( m_DefaultHalfHairMesh );
					}
					else if( eWearHair == ioHelmetWearItem::HWH_ALL_SKIP )
					{
						AddEntityToOwner( m_DefaultBindHairMesh );
					}
					else if( eWearHair == ioHelmetWearItem::HWH_NO_SKIP )
					{
						AddEntityToOwner( m_DefaultBindHairMesh );
						RemoveEntityInOwner( m_DefaultHairMesh );
					}
					break;
				case ioHelmetItem::HH_NO_SKIP:
					if( eWearHair == ioHelmetWearItem::HWH_HALF_SKIP )
					{
						AddEntityToOwner( m_DefaultHairMesh );
						RemoveEntityInOwner( m_DefaultHalfHairMesh );
					}
					else if( eWearHair == ioHelmetWearItem::HWH_ALL_SKIP )
					{
						AddEntityToOwner( m_DefaultHairMesh );
					}
					else if( eWearHair == ioHelmetWearItem::HWH_BIND_SKIP )
					{
						AddEntityToOwner( m_DefaultHairMesh );
						RemoveEntityInOwner( m_DefaultBindHairMesh );
					}
					break;
				}

				switch( eHelmetFace )
				{
				case ioHelmetItem::HF_NONE:
					if( eWearFace == ioHelmetWearItem::HWF_HALF_SKIP )
					{
						AddEntityToOwner( m_DefaultFaceMesh );
						RemoveEntityInOwner( m_SkipFaceMesh );
						m_CurFaceMesh = m_DefaultFaceMesh;
					}
					break;
				case ioHelmetItem::HF_HALF_SKIP:
					if( eWearFace == ioHelmetWearItem::HWF_NONE )
					{
						AddEntityToOwner( m_SkipFaceMesh );
						RemoveEntityInOwner( m_DefaultFaceMesh );
						m_CurFaceMesh = m_SkipFaceMesh;
					}
					break;
				}

				AddEntityToOwner( pHelmet->GetEquipMesh(m_pOwnerChar), true );

				ioHashString szEquipEffect = pHelmet->GetItemEquipEffect(bMale);
				if( !szEquipEffect.IsEmpty() )
					m_pOwnerChar->AttachEffect( szEquipEffect );
			}

			ioHelmetItem::InvisibleFaceType eType = pHelmet->GetInvisibleFaceType();

			switch( eType )
			{
			case ioHelmetItem::IFT_ALL:
				m_pOwnerChar->ShowFaceMesh( false );
				break;
			case ioHelmetItem::IFT_MALE:
				if( m_pOwnerChar->IsMale() )
					m_pOwnerChar->ShowFaceMesh( false );
				break;
			case ioHelmetItem::IFT_FEMALE:
				if( !m_pOwnerChar->IsMale() )
					m_pOwnerChar->ShowFaceMesh( false );
				break;
			}
		}
	}
	else if( ToCloakWearItem( pPreItem ))
	{
		ioCloakWearItem *pPreCloakWear = ToCloakWearItem( pPreItem );
		RemoveEntityInOwner( pPreCloakWear->GetEquipMesh(m_pOwnerChar) );
	}

	m_EquipItemSlot[ES_WEAR] = NULL;

	UpdateSetItemLev();
	m_pOwnerChar->RecalcStatusValue();

	return pPreItem;
}

void ioEquipSlot::ClearItemOwnerByOutUser( const ioHashString &rkName )
{
	ioItem *pItem;
	for( int i=0 ; i<MAX_EQUIP_SLOT ; i++ )
	{
		pItem = m_EquipItemSlot[i];
		if( pItem )
		{
			pItem->ClearIfOwnerName( rkName );
		}
	}
}

void ioEquipSlot::InitDefaultMeshNameList()
{
	ioEntityGroup *pGrp = m_pOwnerChar->GetGroup();
	if( !pGrp )	return;

	ioEntity *pEntity;

	pEntity = pGrp->GetEntity(0);
	if( pEntity )
		m_DefaultFaceMesh = pEntity->GetMeshName();
	else
		m_DefaultFaceMesh.Clear();

	pEntity = pGrp->GetEntity(1);
	if( pEntity )
		m_DefaultHairMesh = pEntity->GetMeshName();
	else
		m_DefaultHairMesh.Clear();

	pEntity = pGrp->GetEntity(2);
	if( pEntity )
		m_DefaultUnderwearMesh = pEntity->GetMeshName();
	else
		m_DefaultUnderwearMesh.Clear();

	pEntity = pGrp->GetEntity(3);
	if( pEntity )
		m_DefaultHandMesh = pEntity->GetMeshName();
	else
		m_DefaultHandMesh.Clear();

	pEntity = pGrp->GetEntity(4);
	if( pEntity )
		m_DefaultBeardMesh = pEntity->GetMeshName();
	else
		m_DefaultBeardMesh.Clear();

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

void ioEquipSlot::ChangeDefaultMeshNameList( const ioHashStringVec &rkNameList )
{
	if( rkNameList.empty() )
		return;

	if( rkNameList.size() >= 3 )
		m_DefaultUnderwearMesh = rkNameList[2];
	else
		m_DefaultUnderwearMesh.Clear();

	if( rkNameList.size() >= 4 )
		m_DefaultHandMesh = rkNameList[3];
	else
		m_DefaultHandMesh.Clear();
}

void ioEquipSlot::UpdateDefaultFaceTextureName()
{
	ioEntityGroup *pGrp = m_pOwnerChar->GetGroup();

	ioEntity *pFace = pGrp->GetEntity( m_CurFaceMesh );
	if( !pFace )	return;

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

ioEntity *ioEquipSlot::AddEntityToOwner( const ioHashString &rkMeshName, bool bSex, bool bThread )
{
	if( rkMeshName.IsEmpty() )	return NULL;
	if( !m_pOwnerChar ) return NULL;

	if( bSex )
	{
		ioHashString szNewMeshName = Help::ConvertName( rkMeshName, m_pOwnerChar->IsMale() );
		if( m_pOwnerChar->IsMale() )
		{
			return m_pOwnerChar->AddEntity( szNewMeshName );
		}
		else
		{
			char szFullName[MAX_PATH]="";
			StringCbPrintf( szFullName, sizeof( szFullName ), "resource\\mesh\\%s.msh" , szNewMeshName.c_str() );
			if( g_ResourceLoader.IsFileExist( szFullName ) )
			{
				return m_pOwnerChar->AddEntity( szNewMeshName );
			}
			else // 없다면 남자 설정
			{
				szNewMeshName = Help::ConvertName( rkMeshName, true );

				return m_pOwnerChar->AddEntity( szNewMeshName );
			}
		}
	}
	else
	{
		ioHashString szNewMeshName = Help::ConvertName( rkMeshName, true ); // 남성

		return m_pOwnerChar->AddEntity( szNewMeshName );
	}
	return NULL;
}

bool ioEquipSlot::RemoveEntityInOwner( const ioHashString &rkMeshName, bool bSex )
{
	if( rkMeshName.IsEmpty() )	return false;
	if( !m_pOwnerChar ) return false;

	if( bSex )
	{
		ioHashString szNewMeshName = Help::ConvertName( rkMeshName, m_pOwnerChar->IsMale() );
		if( m_pOwnerChar->IsMale() )
		{
			return m_pOwnerChar->DestroyEntity( szNewMeshName );
		}
		else
		{
			char szFullName[MAX_PATH]="";
			StringCbPrintf( szFullName, sizeof( szFullName ), "resource\\mesh\\%s.msh" , szNewMeshName.c_str() );
			if( g_ResourceLoader.IsFileExist( szFullName ) )
			{
				return m_pOwnerChar->DestroyEntity( szNewMeshName );
			}
			else // 없다면 남자 설정
			{
				szNewMeshName = Help::ConvertName( rkMeshName, true );
				return m_pOwnerChar->DestroyEntity( szNewMeshName );
			}
		}
	}
	else
	{
		ioHashString szNewMeshName = Help::ConvertName( rkMeshName, true ); // 남성
		return m_pOwnerChar->DestroyEntity( szNewMeshName );
	}
}

void ioEquipSlot::AddEquipedSkill( EquipSlot eSlot, ioItem *pOwnerItem )
{
	if( !pOwnerItem->HasSkill() )
		return;

	ioHashString szSkillName = pOwnerItem->GetSkillName();

	char szNewName[MAX_PATH]="";
	StringCbPrintf_e( szNewName, sizeof(szNewName), "%s_hw", szSkillName.c_str() );

	bool bUsePassive = false;
	ioSkill *pSkill = g_SkillMaker.CreateSkill( szSkillName );
	if( pSkill )
	{
		pSkill->SetOwnerItem( pOwnerItem );
		pSkill->SetAccessoryRandomNum( pOwnerItem->GetAccessoryRandomNum() );
	}

	m_EquipSkillSlot[ eSlot ] = pSkill;

	ioSkill *pSkillW = NULL;
	if( g_SkillMaker.HasSkill( szNewName ) )
	{
		pSkillW = g_SkillMaker.CreateSkill( szNewName );
		if( pSkillW )
		{
			pSkillW->SetOwnerItem( pOwnerItem );
			pSkillW->SetAccessoryRandomNum( pOwnerItem->GetAccessoryRandomNum() );
		}
	}

	m_EquipSkillSlotW[ eSlot ] = pSkillW;


	if( m_pOwnerChar->IsMale() )
	{
		if( pSkill && pSkill->GetTypeID() == ST_PASSIVE && pSkill->IsCanUseZeroHP( m_pOwnerChar ) )
		{
			if( !m_pOwnerChar->CheckDisableSkillByMode( pSkill->GetName() ) )
				pSkill->OnSkillStart( m_pOwnerChar );
		}
	}
	else
	{
		if( pSkillW )
		{
			if( pSkillW->GetTypeID() == ST_PASSIVE && pSkillW->IsCanUseZeroHP( m_pOwnerChar ) )
			{
				if( !m_pOwnerChar->CheckDisableSkillByMode( pSkillW->GetName() ) )
					pSkillW->OnSkillStart( m_pOwnerChar );
			}
		}
		else if( pSkill )
		{
			if( pSkill && pSkill->GetTypeID() == ST_PASSIVE && pSkill->IsCanUseZeroHP( m_pOwnerChar ) )
			{
				if( !m_pOwnerChar->CheckDisableSkillByMode( pSkill->GetName() ) )
					pSkill->OnSkillStart( m_pOwnerChar );
			}
		}
	}

	if( COMPARE( eSlot, ES_RING, ES_BRACELET + 1 ) )
	{
		AccessoryCompoundInfo cInfo = g_AccessoryInfoMgr.FindCompoundInfo( pOwnerItem->GetAccessoryCompoundCode() );
		if( cInfo.m_iCompoundCode > 0 )
		{
			ioSkill *pSkill = g_SkillMaker.CreateSkill( cInfo.m_szSkillName );
			if( pSkill )
			{
				pSkill->SetOwnerItem( pOwnerItem );
				pSkill->SetAccessoryRandomNum( pOwnerItem->GetAccessoryCompoundValue() );
			}
			m_EquipAccessoryCompoundSkillSlot[ eSlot - ES_RING ] = pSkill;

			if( pSkill && pSkill->GetTypeID() == ST_PASSIVE && pSkill->IsCanUseZeroHP( m_pOwnerChar ) )
			{
				if( !m_pOwnerChar->CheckDisableSkillByMode( pSkill->GetName() ) )
					pSkill->OnSkillStart( m_pOwnerChar );
			}
		}
	}
}

void ioEquipSlot::RemoveEquipedSkillBuffInOwner( EquipSlot eSlot )
{
	ioSkill *pSkill = m_EquipSkillSlot[eSlot];
	if( pSkill )
	{
		ioPassiveSkill *pPassiveSkill = ToPassiveSkill( pSkill );
		ioBuffSkill *pBuffSkill = ToBuffSkill( pSkill );
		if( pPassiveSkill )
		{
			pPassiveSkill->RemoveSkillBuff( m_pOwnerChar );
		}
		else if( pBuffSkill )
		{
			pBuffSkill->CheckRemoveEquipSkillBuff( true );
			pBuffSkill->RemoveSkillBuff( m_pOwnerChar );
			pBuffSkill->RemoveAreaWeapon( m_pOwnerChar );
		}
	}

	ioSkill *pSkillW = m_EquipSkillSlotW[eSlot];
	if( pSkillW )
	{
		ioPassiveSkill *pPassiveSkill = ToPassiveSkill( pSkillW );
		ioBuffSkill *pBuffSkill = ToBuffSkill( pSkillW );
		if( pPassiveSkill )
		{
			pPassiveSkill->RemoveSkillBuff( m_pOwnerChar );
		}
		else if( pBuffSkill )
		{
			pBuffSkill->CheckRemoveEquipSkillBuff( true );
			pBuffSkill->RemoveSkillBuff( m_pOwnerChar );
			pBuffSkill->RemoveAreaWeapon( m_pOwnerChar );
		}
	}

	if( COMPARE( eSlot, ES_RING, ES_BRACELET + 1 ) )
	{
		ioSkill *pSkill = m_EquipAccessoryCompoundSkillSlot[eSlot - ES_RING];
		if( pSkill )
		{
			ioPassiveSkill *pPassiveSkill = ToPassiveSkill( pSkill );
			ioBuffSkill *pBuffSkill = ToBuffSkill( pSkill );
			if( pPassiveSkill )
			{
				pPassiveSkill->RemoveSkillBuff( m_pOwnerChar );
			}
			else if( pBuffSkill )
			{
				pBuffSkill->CheckRemoveEquipSkillBuff( true );
				pBuffSkill->RemoveSkillBuff( m_pOwnerChar );
				pBuffSkill->RemoveAreaWeapon( m_pOwnerChar );
			}
		}
	}
}

void ioEquipSlot::RemoveEquipedSkill( EquipSlot eSlot )
{
	ioSkill *pSkill = m_EquipSkillSlot[eSlot];
	if( pSkill )
	{
		pSkill->OnSkillItemRelease( m_pOwnerChar );

		m_pOwnerChar->ClearIfThisActiveSkill( pSkill );

		if( pSkill->GetTypeID() == ST_PASSIVE )
		{
			pSkill->OnSkillEnd( m_pOwnerChar );
		}

		m_EquipSkillSlot[eSlot] = NULL;
		SAFEDELETE( pSkill );
	}

	ioSkill *pSkillW = m_EquipSkillSlotW[eSlot];
	if( pSkillW )
	{
		pSkillW->OnSkillItemRelease( m_pOwnerChar );

		m_pOwnerChar->ClearIfThisActiveSkill( pSkillW );

		if( pSkillW->GetTypeID() == ST_PASSIVE )
		{
			pSkillW->OnSkillEnd( m_pOwnerChar );
		}

		m_EquipSkillSlotW[eSlot] = NULL;
		SAFEDELETE( pSkillW );
	}

	if( COMPARE( eSlot, ES_RING, ES_BRACELET + 1 ) )
	{
		ioSkill *pSkill = m_EquipAccessoryCompoundSkillSlot[eSlot - ES_RING];
		if( pSkill )
		{
			pSkill->OnSkillItemRelease( m_pOwnerChar );

			m_pOwnerChar->ClearIfThisActiveSkill( pSkill );

			if( pSkill->GetTypeID() == ST_PASSIVE )
			{
				pSkill->OnSkillEnd( m_pOwnerChar );
			}

			m_EquipAccessoryCompoundSkillSlot[eSlot - ES_RING] = NULL;
			SAFEDELETE( pSkill );
		}
	}
}

void ioEquipSlot::ChangeEquipedSkill( EquipSlot eSlot, ioItem *pOwnerItem )
{
	RemoveEquipedSkill( eSlot );

	AddEquipedSkill( eSlot, pOwnerItem );
}

void ioEquipSlot::ChangeEquipedSkill( EquipSlot eSlot, const ioHashString &rkSkillName )
{
	if( !COMPARE( eSlot, ES_WEAPON, MAX_EQUIP_SLOT ) )
		return;

	if( !m_EquipItemSlot[eSlot] )
		return;

	if( m_EquipItemSlot[eSlot]->GetSkillName() == rkSkillName )
		return;

	m_EquipItemSlot[eSlot]->SetSkillName( rkSkillName );
	ChangeEquipedSkill( eSlot, m_EquipItemSlot[eSlot] );
}

ioWeaponItem* ioEquipSlot::GetWeapon() const
{
	return ToWeaponItem( m_EquipItemSlot[ES_WEAPON] );
}

ioArmorItem* ioEquipSlot::GetArmor() const
{
	return ToArmorItem( m_EquipItemSlot[ES_ARMOR] );
}

ioHelmetItem* ioEquipSlot::GetHelmet() const
{
	return ToHelmetItem( m_EquipItemSlot[ES_HELMET] );
}

ioCloakItem* ioEquipSlot::GetCloak() const
{
	return ToCloakItem( m_EquipItemSlot[ES_CLOAK] );
}

ioObjectItem* ioEquipSlot::GetObject() const
{
	return ToObjectItem( m_EquipItemSlot[ES_OBJECT] );
}

ioWearItem* ioEquipSlot::GetWear() const
{
	return ToWearItem( m_EquipItemSlot[ES_WEAR] );
}

int ioEquipSlot::GetEquipedItemCnt()
{
	int iEquipCnt = 0;
	for( int i=0 ; i < MAX_EQUIP_SLOT ; i++ )
	{
		if( m_EquipItemSlot[i] && !m_DropWaitSlot[i] )
		{
			int iType = m_EquipItemSlot[i]->GetType();
			if ( iType == ioItem::IT_RING || iType == ioItem::IT_NECKLACE || iType == ioItem::IT_BRACELET )
				continue;
			iEquipCnt++;
		}
	}

	return iEquipCnt;
}

ioItem* ioEquipSlot::GetEquipedItem( int iSlot )
{
	if( COMPARE( iSlot, ES_WEAPON, MAX_EQUIP_SLOT ) )
		return m_EquipItemSlot[iSlot];

	LOG.PrintTimeAndLog( 0, "ioEquipSlot::GetEquipedItem - %d Overflow", iSlot );
	return NULL;
}

ioItem* ioEquipSlot::GetEquipedItemByCode( DWORD dwCode )
{
	for( int i=0 ; i<MAX_EQUIP_SLOT ; i++ )
	{
		ioItem *pItem = m_EquipItemSlot[i];
		if( pItem && pItem->GetItemCode() == dwCode )
			return pItem;
	}

	return NULL;
}

ioItem* ioEquipSlot::GetEquipedItem( const ioHashString &szItemName )
{
	for( int i=0 ; i<MAX_EQUIP_SLOT ; i++ )
	{
		ioItem *pItem = m_EquipItemSlot[i];
		if( pItem && pItem->GetName() == szItemName )
			return pItem;
	}

	return NULL;
}

ioSkill* ioEquipSlot::GetEquipedSkill( int iSlot )
{
	if( COMPARE( iSlot, ES_WEAPON, MAX_EQUIP_SLOT ) )
	{
		if( m_pOwnerChar->IsMale() )
		{
			return m_EquipSkillSlot[iSlot];
		}
		else
		{
			if( !m_EquipSkillSlotW[iSlot] )
				return m_EquipSkillSlot[iSlot];

			return m_EquipSkillSlotW[iSlot];
		}
	}

	LOG.PrintTimeAndLog( 0, "ioEquipSlot::GetEquipedSkill - %d Overflow", iSlot );
	return NULL;
}

ioSkill* ioEquipSlot::GetUseEnableSkill( int iSlot, bool bHelpMsg )
{
	ioSkill *pSkill = GetEquipedSkill( iSlot );
	if( !pSkill ) return NULL;

	if( m_pOwnerChar && m_pOwnerChar->GetState() == CS_LOCKUP_BUFF )
		return NULL;

	// 썬더버드 상태이면
	if( !pSkill->IsCanUseThunderBirdHold( m_pOwnerChar ) )
		return NULL;

	// 데탐에 사용가능한지 체크
	if( !pSkill->IsCanUseZeroHP( m_pOwnerChar ) )
		return NULL;

	// 보스 사용가능한지 체크
	if( !pSkill->IsCanUseBoss( m_pOwnerChar ) )
		return NULL;

	// 축구모드에서 사용가능한지 체크
	if( !pSkill->IsCanUseFootball() )
		return NULL;

	// 강시모드에서 사용가능한지 체크
	if( !pSkill->IsCanUseGangSiMode() )
		return NULL;

	// 왕관 쓰고 사용가능한지 체크
	if( !pSkill->IsCanUseSkillWithCrown( m_pOwnerChar ) )
		return NULL;

	// 충돌체 타입 관련 체크
	if( !pSkill->IsCanUseByWeaponType( m_pOwnerChar ) )
		return NULL;

	// 남은 체력에 따라 사용가능하지 체크
	if( !pSkill->IsCanUseLimitHP( m_pOwnerChar ) )
		return NULL;

	// 기본(게이지를 가장 나중에 체크)
	if( !pSkill->IsCanUseSkill( m_pOwnerChar, bHelpMsg ) )
		return NULL;
	
	return pSkill;
}

ioSkill* ioEquipSlot::GetEquipedSkill( const ioHashString &szSkillName )
{
	if( !m_pOwnerChar->IsMale() )
	{
		for( int i=0 ; i<MAX_EQUIP_SLOT ; i++ )
		{
			ioSkill *pSkillW = m_EquipSkillSlotW[i];
			if( pSkillW && pSkillW->GetName() == szSkillName )
				return pSkillW;
		}
	}

	for( int i=0 ; i<MAX_EQUIP_SLOT ; i++ )
	{
		ioSkill *pSkill = m_EquipSkillSlot[i];
		if( pSkill && pSkill->GetName() == szSkillName )
			return pSkill;
	}

	for( int i=0 ; i<MAX_ACCESSORY_SLOT ; i++ )
	{
		ioSkill *pSkill = m_EquipAccessoryCompoundSkillSlot[i];
		if( pSkill && pSkill->GetName() == szSkillName )
			return pSkill;
	}
	return NULL;
}

ioSkill* ioEquipSlot::GetAccessoryCompoundSkill( int iSlot )
{
	if( COMPARE( iSlot, ES_RING, MAX_EQUIP_SLOT ) )
	{
		return m_EquipAccessoryCompoundSkillSlot[iSlot - ES_RING];
	}

	LOG.PrintTimeAndLog( 0, "ioEquipSlot::GetAccessoryCompoundSkill - %d Overflow", iSlot );
	return NULL;
}

bool ioEquipSlot::IsDefaultHair( const ioHashString &rkName ) const
{
	if( rkName == m_DefaultHairMesh ||
		rkName == m_DefaultHalfHairMesh )
		return true;

	return false;
}

void ioEquipSlot::UpdateSetItemLev()
{
	ioItem *pWeapon = m_EquipItemSlot[ES_WEAPON];
	if( !pWeapon || pWeapon->GetSetItemCode() == 0 )
	{
		m_iSetItemLev = 0;
		return;
	}

	m_iSetItemLev = 0;
	DWORD dwWeaponSetCode = pWeapon->GetSetItemCode();

	if( m_EquipItemSlot[ES_ARMOR] )
	{
		if( m_EquipItemSlot[ES_ARMOR]->GetSetItemCode() == dwWeaponSetCode )
			m_iSetItemLev++;
	}

	if( m_EquipItemSlot[ES_HELMET] )
	{
		if( m_EquipItemSlot[ES_HELMET]->GetSetItemCode() == dwWeaponSetCode )
			m_iSetItemLev++;
	}

	if( m_EquipItemSlot[ES_CLOAK] )
	{
		if( m_EquipItemSlot[ES_CLOAK]->GetSetItemCode() == dwWeaponSetCode )
			m_iSetItemLev++;
	}
}

bool ioEquipSlot::IsEquipSlot( EquipSlot eSlot )
{
	if( m_EquipItemSlot[eSlot] )
		return true;

	return false;
}

void ioEquipSlot::SetDropWaitSlot( int eSlot, bool bWait )
{
	if( COMPARE( eSlot, ES_WEAPON, MAX_EQUIP_SLOT ) )
	{
		m_DropWaitSlot[eSlot] = bWait;
		return;
	}

	LOG.PrintTimeAndLog( 0, "ioEquipSlot::SetDropWaitSlot - %d Overflow", eSlot );
}

void ioEquipSlot::SetEquipWaitSlot( int eSlot, bool bWait )
{
	if( COMPARE( eSlot, ES_WEAPON, MAX_EQUIP_SLOT ) )
	{
		m_EquipWaitSlot[eSlot] = bWait;
		return;
	}

	LOG.PrintTimeAndLog( 0, "ioEquipSlot::SetEquipWaitSlot - %d Overflow", eSlot );	
}

bool ioEquipSlot::IsEquipWait()
{
	bool bEquipWait = false;
	for( int i=0 ; i<MAX_EQUIP_SLOT ; i++ )
	{
		if( m_EquipWaitSlot[i] )
		{
			bEquipWait = true;
			break;
		}
	}

	return bEquipWait;
}

bool ioEquipSlot::IsEquipWait( int eSlot )
{
	if( COMPARE( eSlot, 0, MAX_EQUIP_SLOT ) )
	{
		return m_EquipWaitSlot[eSlot];
	}

	return false;
}

bool ioEquipSlot::IsDropWait( int eSlot )
{
	if( COMPARE( eSlot, 0, MAX_EQUIP_SLOT ) )
	{
		return m_DropWaitSlot[eSlot];
	}

	return false;
}

void ioEquipSlot::CheckCurHair( ioItem *pItem, bool bRemove )
{
	ioHelmetItem *pHelmet = ToHelmetItem( pItem );
	bool bMale = true;
	if( m_pOwnerChar )
		bMale = m_pOwnerChar->IsMale();

	ioHelmetItem::HelmetHair eHairType = pHelmet->GetChangeHair( bMale );
	ioHelmetItem::HelmetFace eFaceType = pHelmet->GetChangeFace( bMale );

	if( bRemove )
	{
		switch( eHairType )
		{
		case ioHelmetItem::HH_HALF_SKIP:
			AddEntityToOwner( m_DefaultHairMesh );
			RemoveEntityInOwner( m_DefaultHalfHairMesh );
			break;
		case ioHelmetItem::HH_ALL_SKIP:
			AddEntityToOwner( m_DefaultHairMesh );
			break;
		case ioHelmetItem::HH_BIND_SKIP:
			AddEntityToOwner( m_DefaultHairMesh );
			RemoveEntityInOwner( m_DefaultBindHairMesh );
			break;
		case ioHelmetItem::HH_NO_SKIP:
			break;
		}

		switch( eFaceType )
		{
		case ioHelmetItem::HF_HALF_SKIP:
			AddEntityToOwner( m_DefaultFaceMesh );
			RemoveEntityInOwner( m_SkipFaceMesh );
			m_CurFaceMesh = m_DefaultFaceMesh;
			break;
		}
	}
	else
	{
		switch( eHairType )
		{
		case ioHelmetItem::HH_HALF_SKIP:
			AddEntityToOwner( m_DefaultHalfHairMesh );
			RemoveEntityInOwner( m_DefaultHairMesh );
			break;
		case ioHelmetItem::HH_ALL_SKIP:
			RemoveEntityInOwner( m_DefaultHairMesh );
			break;
		case ioHelmetItem::HH_BIND_SKIP:
			AddEntityToOwner( m_DefaultBindHairMesh );
			RemoveEntityInOwner( m_DefaultHairMesh );
			break;
		case ioHelmetItem::HH_NO_SKIP:
			break;
		}

		switch( eFaceType )
		{
		case ioHelmetItem::HF_HALF_SKIP:
			AddEntityToOwner( m_SkipFaceMesh );
			RemoveEntityInOwner( m_DefaultFaceMesh );
			m_CurFaceMesh = m_SkipFaceMesh;
			break;
		}
	}

	if( m_pOwnerChar )
	{
		m_pOwnerChar->ApplyModifiedMaterial();
	}
}

const ioHashString& ioEquipSlot::GetCurHandMesh() const
{
	ioWeaponItem *pWeapon = GetWeapon();

	if( pWeapon && ToHookItem(pWeapon) )
		return m_DefaultHandMesh;

	ioArmorItem *pArmor = GetArmor();
	if( pArmor && pArmor->HasHandMesh(m_pOwnerChar) )
		return pArmor->GetHandMesh(m_pOwnerChar);

	return m_DefaultHandMesh;
}

ioItem* ioEquipSlot::ReleaseItemMesh( EquipSlot eSlot )
{
	ioItem *pPreItem = NULL;
	switch( eSlot )
	{
	case ES_WEAPON:
		pPreItem = ReleaseWeaponMesh();
		break;
	case ES_ARMOR:
		pPreItem = ReleaseArmorMesh();
		break;
	case ES_HELMET:
		pPreItem = ReleaseHelmetMesh();
		break;
	case ES_CLOAK:
		pPreItem = ReleaseCloakMesh();
		break;
	case ES_OBJECT:
		pPreItem = ReleaseObjectMesh();
		break;
	case ES_WEAR:
		pPreItem = ReleaseWearMesh();
		break;
	}

	if( m_pOwnerChar )
	{
		m_pOwnerChar->ApplyModifiedMaterial();
	}

	return pPreItem;
}

ioItem* ioEquipSlot::ReleaseWeaponMesh()
{
	ioItem *pPreItem = m_EquipItemSlot[ES_WEAPON];
	if( pPreItem )
	{
		ioWeaponItem *pWeapon = ToWeaponItem( pPreItem );
		if( pWeapon && !RemoveEntityInOwner( pWeapon->GetEquipMesh(m_pOwnerChar), true ) )
		{
			LOG.PrintTimeAndLog( 0, "ReleaseWeapon - %s Char %s Entity Destory Failed",
									m_pOwnerChar->GetCharName().c_str(),
									pWeapon->GetEquipMesh(m_pOwnerChar).c_str() );
		}

		if( ToHookItem( pPreItem ) )
		{
			if( pPreItem->IsVisible() )
			{
				AddEntityToOwner( m_DefaultHandMesh );
			}
		}

		ioHashString szEquipEffect = pPreItem->GetItemEquipEffect(m_pOwnerChar->IsMale());
		if( !szEquipEffect.IsEmpty() )
		{
			m_pOwnerChar->EndEffect( szEquipEffect, false );
		}

		pPreItem->ReleaseExtendEquipEffect( m_pOwnerChar );
	}

	return pPreItem;
}

ioItem* ioEquipSlot::ReleaseArmorMesh()
{
	ioArmorItem *pArmor = ToArmorItem( m_EquipItemSlot[ES_ARMOR] );
	if( pArmor )
	{
		AddEntityToOwner( m_DefaultUnderwearMesh );
		RemoveEntityInOwner( pArmor->GetBodyMesh(m_pOwnerChar), true );

		if( !ToHookItem( GetWeapon() ) )
		{
			if( pArmor->HasHandMesh(m_pOwnerChar) )
			{
				AddEntityToOwner( m_DefaultHandMesh );
				RemoveEntityInOwner( pArmor->GetHandMesh(m_pOwnerChar), true );
			}
		}

		if( pArmor->HasSpringRope() )
		{
			ioHelmetItem *pCheckHelmet = ToHelmetItem( m_EquipItemSlot[ES_HELMET] );
			ioCloakItem *pCheckCloak = ToCloakItem( m_EquipItemSlot[ES_CLOAK] );

			bool bHelmetHas = true;
			bool bCloakHas = true;

			if( !pCheckHelmet || !pCheckHelmet->HasSpringRope() )
				bHelmetHas = false;

			if( !pCheckCloak || !pCheckCloak->HasSpringRope() )
				bCloakHas = false;

			if( !bHelmetHas && !bCloakHas )
				m_pOwnerChar->RemoveRopeSpring( pArmor->GetSpringBiped() );
		}

		ioHashString szEquipEffect = pArmor->GetItemEquipEffect(m_pOwnerChar->IsMale());
		if( !szEquipEffect.IsEmpty() )
		{
			m_pOwnerChar->EndEffect( szEquipEffect, false );
		}

		pArmor->ReleaseExtendEquipEffect( m_pOwnerChar );
	}

	return pArmor;
}

ioItem* ioEquipSlot::ReleaseHelmetMesh()
{
	ioItem *pPreItem = m_EquipItemSlot[ES_HELMET];
	if( pPreItem )
	{
		bool bMale = true;
		if( m_pOwnerChar )
			bMale = m_pOwnerChar->IsMale();

		ioHelmetItem *pPreHelmet = ToHelmetItem( pPreItem );
		RemoveEntityInOwner( pPreHelmet->GetEquipMesh(m_pOwnerChar), true );

		if( m_pOwnerChar )
		{
			if( m_pOwnerChar->HasEntity(m_DefaultHalfHairMesh) )
			{
				RemoveEntityInOwner( m_DefaultHalfHairMesh );
			}
			else if( m_pOwnerChar->HasEntity(m_DefaultBindHairMesh) )
			{
				RemoveEntityInOwner( m_DefaultBindHairMesh );
			}

			if( !m_pOwnerChar->HasEntity(m_DefaultHairMesh) )
			{
				AddEntityToOwner( m_DefaultHairMesh );
			}
			
			// Face
			if( m_pOwnerChar->HasEntity( m_SkipFaceMesh ) )
			{
				RemoveEntityInOwner( m_SkipFaceMesh );
				AddEntityToOwner( m_DefaultFaceMesh );
				m_CurFaceMesh = m_DefaultFaceMesh;
			}
		}

		ioHashString szEquipEffect = pPreItem->GetItemEquipEffect(m_pOwnerChar->IsMale());
		if( !szEquipEffect.IsEmpty() )
		{
			m_pOwnerChar->EndEffect( szEquipEffect, false );
		}

		pPreItem->ReleaseExtendEquipEffect( m_pOwnerChar );
	}

	ioHelmetItem *pHelmet = ToHelmetItem( pPreItem );
	if( pHelmet && pHelmet->HasSpringRope() )
	{
		ioArmorItem *pCheckArmor = ToArmorItem( m_EquipItemSlot[ES_ARMOR] );
		ioCloakItem *pCheckCloak = ToCloakItem( m_EquipItemSlot[ES_CLOAK] );

		bool bArmorHas = true;
		bool bCloakHas = true;

		if( !pCheckArmor || !pCheckArmor->HasSpringRope() )
			bArmorHas = false;

		if( !pCheckCloak || !pCheckCloak->HasSpringRope() )
			bCloakHas = false;

		if( !bArmorHas && !bCloakHas )
			m_pOwnerChar->RemoveRopeSpring( pHelmet->GetSpringBiped() );
	}

	return pPreItem;
}

ioItem* ioEquipSlot::ReleaseCloakMesh()
{
	ioItem *pPreItem = m_EquipItemSlot[ES_CLOAK];
	if( pPreItem )
	{
		ioCloakItem *pCloak = ToCloakItem( pPreItem );
		if( pCloak && pCloak->HasSpringRope() )
		{
			ioArmorItem *pCheckArmor = ToArmorItem( m_EquipItemSlot[ES_ARMOR] );
			ioHelmetItem *pCheckHelmet = ToHelmetItem( m_EquipItemSlot[ES_HELMET] );

			bool bArmorHas = true;
			bool bHelmetHas = true;

			if( !pCheckArmor || !pCheckArmor->HasSpringRope() )
				bArmorHas = false;

			if( !pCheckHelmet || !pCheckHelmet->HasSpringRope() )
				bHelmetHas = false;

			if( !bArmorHas && !bHelmetHas )
				m_pOwnerChar->RemoveRopeSpring( pCloak->GetSpringBiped() );
		}

		RemoveEntityInOwner( pCloak->GetEquipMesh(m_pOwnerChar), true );

		ioHashString szEquipEffect = pPreItem->GetItemEquipEffect(m_pOwnerChar->IsMale());
		if( !szEquipEffect.IsEmpty() )
		{
			m_pOwnerChar->EndEffect( szEquipEffect, false );
		}

		pPreItem->ReleaseExtendEquipEffect( m_pOwnerChar );
	}

	return pPreItem;
}

ioItem* ioEquipSlot::ReleaseObjectMesh()
{
	ioItem *pPreItem = m_EquipItemSlot[ES_OBJECT];
	if( pPreItem )
	{
		ioObjectItem *pObject = ToObjectItem( pPreItem );
		if( pObject && !RemoveEntityInOwner( pObject->GetEquipMesh(m_pOwnerChar) ) )
		{
			LOG.PrintTimeAndLog( 0, "ReleaseObject - %s Char %s Entity Destory Failed",
									m_pOwnerChar->GetCharName().c_str(),
									pObject->GetEquipMesh(m_pOwnerChar).c_str() );
		}

		ioWeaponItem *pWeapon = GetWeapon();
		if( pWeapon )
		{
			if( m_pOwnerChar )
				m_pOwnerChar->ShowWeaponItemMesh( true );
		}

		ioHashString szEquipEffect = pPreItem->GetItemEquipEffect( m_pOwnerChar->IsMale() );
		if( !szEquipEffect.IsEmpty() )
		{
			m_pOwnerChar->EndEffect( szEquipEffect, false );
		}
		pPreItem->ReleaseExtendEquipEffect( m_pOwnerChar );
	}

	return pPreItem;
}

ioItem* ioEquipSlot::ReleaseWearMesh()
{
	ioItem *pPreItem = m_EquipItemSlot[ES_WEAR];
	if( ToHelmetWearItem( pPreItem ))
	{
		ioHelmetWearItem *pPreHelmetWear = ToHelmetWearItem( pPreItem );
		RemoveEntityInOwner( pPreHelmetWear->GetEquipMesh(m_pOwnerChar) );

		ioHelmetWearItem::HelmetWearHair eWearHair = pPreHelmetWear->GetChangeHair();
		ioHelmetWearItem::HelmetWearFace eWearFace = pPreHelmetWear->GetChangeFace();

		bool bDefaultMesh = false;
		if( m_pOwnerChar )
			bDefaultMesh = m_pOwnerChar->HasEntity( m_DefaultHairMesh );

		if( bDefaultMesh )
		{
			return pPreItem;
		}

		// set hair
		if( !m_EquipItemSlot[ES_HELMET] )
		{
			if( !m_pOwnerChar->IsInvisibleHead() )
			{
				switch( eWearHair )
				{
				case ioHelmetWearItem::HWH_HALF_SKIP:
					AddEntityToOwner( m_DefaultHairMesh );
					RemoveEntityInOwner( m_DefaultHalfHairMesh );
					break;
				case ioHelmetWearItem::HWH_ALL_SKIP:
					AddEntityToOwner( m_DefaultHairMesh );
					break;
				case ioHelmetWearItem::HWH_BIND_SKIP:
					AddEntityToOwner( m_DefaultHairMesh );
					RemoveEntityInOwner( m_DefaultBindHairMesh );
					break;
				case ioHelmetWearItem::HWH_NO_SKIP:
					break;
				}

				switch( eWearFace )
				{
				case ioHelmetWearItem::HWF_HALF_SKIP:
					AddEntityToOwner( m_DefaultFaceMesh );
					RemoveEntityInOwner( m_SkipFaceMesh );
					m_CurFaceMesh = m_DefaultFaceMesh;
					break;
				}
			}
		}
		// set helmet
		else
		{
			if( !m_pOwnerChar->IsInvisibleHead() )
			{
				bool bMale = true;
				if( m_pOwnerChar )
					bMale = m_pOwnerChar->IsMale();

				ioHelmetItem *pHelmet = ToHelmetItem( m_EquipItemSlot[ES_HELMET] );
				ioHelmetItem::HelmetHair eHelmetHair = pHelmet->GetChangeHair( bMale );
				ioHelmetItem::HelmetFace eHelmetFace = pHelmet->GetChangeFace( bMale );

				switch( eHelmetHair )
				{
				case ioHelmetItem::HH_HALF_SKIP:
					if( eWearHair == ioHelmetWearItem::HWH_ALL_SKIP )
					{
						AddEntityToOwner( m_DefaultHalfHairMesh );
					}
					else if( eWearHair == ioHelmetWearItem::HWH_BIND_SKIP )
					{
						AddEntityToOwner( m_DefaultHalfHairMesh );
						RemoveEntityInOwner( m_DefaultBindHairMesh );
					}
					else if( eWearHair == ioHelmetWearItem::HWH_NO_SKIP )
					{
						AddEntityToOwner( m_DefaultHalfHairMesh );
						RemoveEntityInOwner( m_DefaultHairMesh );
					}
					break;
				case ioHelmetItem::HH_ALL_SKIP:
					if( eWearHair == ioHelmetWearItem::HWH_HALF_SKIP )
					{
						RemoveEntityInOwner( m_DefaultHalfHairMesh );
					}
					else if( eWearHair == ioHelmetWearItem::HWH_BIND_SKIP )
					{
						RemoveEntityInOwner( m_DefaultBindHairMesh );
					}
					else if( eWearHair == ioHelmetWearItem::HWH_NO_SKIP )
					{
						RemoveEntityInOwner( m_DefaultHairMesh );
					}
					break;
				case ioHelmetItem::HH_BIND_SKIP:
					if( eWearHair == ioHelmetWearItem::HWH_HALF_SKIP )
					{
						AddEntityToOwner( m_DefaultBindHairMesh );
						RemoveEntityInOwner( m_DefaultHalfHairMesh );
					}
					else if( eWearHair == ioHelmetWearItem::HWH_ALL_SKIP )
					{
						AddEntityToOwner( m_DefaultBindHairMesh );
					}
					else if( eWearHair == ioHelmetWearItem::HWH_NO_SKIP )
					{
						AddEntityToOwner( m_DefaultBindHairMesh );
						RemoveEntityInOwner( m_DefaultHairMesh );
					}
					break;
				case ioHelmetItem::HH_NO_SKIP:
					if( eWearHair == ioHelmetWearItem::HWH_HALF_SKIP )
					{
						AddEntityToOwner( m_DefaultHairMesh );
						RemoveEntityInOwner( m_DefaultHalfHairMesh );
					}
					else if( eWearHair == ioHelmetWearItem::HWH_ALL_SKIP )
					{
						AddEntityToOwner( m_DefaultHairMesh );
					}
					else if( eWearHair == ioHelmetWearItem::HWH_BIND_SKIP )
					{
						AddEntityToOwner( m_DefaultHairMesh );
						RemoveEntityInOwner( m_DefaultBindHairMesh );
					}
					break;
				}

				switch( eHelmetFace )
				{
				case ioHelmetItem::HF_NONE:
					if( eWearFace == ioHelmetWearItem::HWF_HALF_SKIP )
					{
						AddEntityToOwner( m_DefaultFaceMesh );
						RemoveEntityInOwner( m_SkipFaceMesh );
						m_CurFaceMesh = m_DefaultFaceMesh;
					}
					break;
				case ioHelmetItem::HF_HALF_SKIP:
					if( eWearFace == ioHelmetWearItem::HWF_NONE )
					{
						AddEntityToOwner( m_SkipFaceMesh );
						RemoveEntityInOwner( m_DefaultFaceMesh );
						m_CurFaceMesh = m_SkipFaceMesh;
					}
					break;
				}
			}
		}
	}
	else if( ToCloakWearItem( pPreItem ) )
	{
		ioCloakWearItem *pPreCloakWear = ToCloakWearItem( pPreItem );
		RemoveEntityInOwner( pPreCloakWear->GetEquipMesh(m_pOwnerChar) );
	}

	return pPreItem;
}

ioItem* ioEquipSlot::EquipItemMesh( ioItem *pItem, bool bShow, bool bThread )
{
	ioItem *pPreItem = NULL;

	switch( pItem->GetType() )
	{
	case ioItem::IT_WEAPON:
		pPreItem = EquipWeaponMesh( pItem, bShow, bThread );
		break;
	case ioItem::IT_ARMOR:
		pPreItem = EquipArmorMesh( pItem, bShow, bThread );
		break;
	case ioItem::IT_HELMET:
		pPreItem = EquipHelmetMesh( pItem, bShow, bThread );
		break;
	case ioItem::IT_CLOAK:
		pPreItem = EquipCloakMesh( pItem, bShow, bThread );
		break;
	case ioItem::IT_OBJECT:
		pPreItem = EquipObjectMesh( pItem, bShow, bThread );
		break;
	case ioItem::IT_WEAR:
		pPreItem = EquipWearMesh( pItem, bShow, bThread );
		break;
	}

	if( m_pOwnerChar )
	{
		m_pOwnerChar->ApplyModifiedMaterial();
	}

	return pPreItem;
}

ioItem* ioEquipSlot::EquipWeaponMesh( ioItem *pItem, bool bShow, bool bThread )
{
	// 사용시 주의!! 착용중인 Mesh가 없다는 전제하에 진행하는 함수 2009.03.04 KDH
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );

	if( pItem->GetItemMaleCustom() > 0 || pItem->GetItemFemaleCustom() > 0 )
		bThread = false;    // 커스텀 되어있으면 즉시 로딩

	EquipSlotData( ES_WEAPON, pWeapon->GetItemCode(), pWeapon->GetItemMaleCustom(), pWeapon->GetItemFemaleCustom() );
	EquipSlotEntity( ES_WEAPON, AddEntityToOwner( pWeapon->GetEquipMesh(m_pOwnerChar), true, bThread ) );

	if( ToHookItem( pItem ) )
	{
		RemoveEntityInOwner( GetCurHandMesh() );
	}

	if( pItem )
	{
		ioHashString szEquipEffect = pItem->GetItemEquipEffect(m_pOwnerChar->IsMale());
		if( !szEquipEffect.IsEmpty() )
		{
			m_pOwnerChar->AttachEffect( szEquipEffect );
		}

		pItem->SetExtendEquipEffect( m_pOwnerChar );
	}

	return NULL;
}

ioItem* ioEquipSlot::EquipArmorMesh( ioItem *pItem, bool bShow, bool bThread )
{
	// 사용시 주의!! 착용중인 Mesh가 없다는 전제하에 진행하는 함수 2009.03.04 KDH
	ioArmorItem *pArmor = ToArmorItem( pItem );

	if( pItem->GetItemMaleCustom() > 0 || pItem->GetItemFemaleCustom() > 0 )
		bThread = false;    // 커스텀 되어있으면 즉시 로딩

	EquipSlotData( ES_ARMOR, pArmor->GetItemCode(), pArmor->GetItemMaleCustom(), pArmor->GetItemFemaleCustom() );
	EquipSlotEntity( ES_ARMOR, AddEntityToOwner( pArmor->GetBodyMesh(m_pOwnerChar), true, bThread ) );
	RemoveEntityInOwner( m_DefaultUnderwearMesh );

	// about hands....
	if( !ToHookItem(GetWeapon()) )
	{
		if( pArmor->HasHandMesh(m_pOwnerChar) )
		{
			EquipSlotEntity( ES_ARMOR, AddEntityToOwner( pArmor->GetHandMesh(m_pOwnerChar), true, bThread ) );

			RemoveEntityInOwner( m_DefaultHandMesh );
		}
	}

	if( pArmor->HasSpringRope() )
	{
		m_pOwnerChar->AddRopeSpring( pArmor->GetSpringBiped(), pArmor->GetSpringParam() );
	}

	if( pItem )
	{
		ioHashString szEquipEffect = pItem->GetItemEquipEffect(m_pOwnerChar->IsMale());
		if( !szEquipEffect.IsEmpty() )
		{
			m_pOwnerChar->AttachEffect( szEquipEffect );
		}

		pItem->SetExtendEquipEffect( m_pOwnerChar );
	}

	return NULL;
}

ioItem* ioEquipSlot::EquipHelmetMesh( ioItem *pItem, bool bShow, bool bThread )
{
	// 사용시 주의!! 착용중인 Mesh가 없다는 전제하에 진행하는 함수 2009.03.04 KDH
	// Wear 아이템도 메시는 벗겨진 상태라 전제하고 진행
	ioHelmetItem *pHelmet = ToHelmetItem( pItem );

	if( pItem )
	{
		ioHashString szEquipEffect = pItem->GetItemEquipEffect(m_pOwnerChar->IsMale());
		if( !szEquipEffect.IsEmpty() )
		{
			m_pOwnerChar->AttachEffect( szEquipEffect );
		}

		pItem->SetExtendEquipEffect( m_pOwnerChar );
	}

	bool bMale = true;
	if( m_pOwnerChar )
		bMale = m_pOwnerChar->IsMale();

	if( !m_pOwnerChar->IsInvisibleHead() )
	{
		if( bShow )
		{
			ioHelmetItem::HelmetHair eNewHairType = pHelmet->GetChangeHair( bMale );
			ioHelmetItem::HelmetFace eNewFaceType = pHelmet->GetChangeFace( bMale );
			switch( eNewHairType )
			{
			case ioHelmetItem::HH_HALF_SKIP:
				AddEntityToOwner( m_DefaultHalfHairMesh, false, bThread );
				RemoveEntityInOwner( m_DefaultHairMesh );
				break;
			case ioHelmetItem::HH_ALL_SKIP:
				RemoveEntityInOwner( m_DefaultHairMesh );
				break;
			case ioHelmetItem::HH_BIND_SKIP:
				AddEntityToOwner( m_DefaultBindHairMesh, false, bThread );
				RemoveEntityInOwner( m_DefaultHairMesh );
				break;
			case ioHelmetItem::HH_NO_SKIP:
				break;
			}

			switch( eNewFaceType )
			{
			case ioHelmetItem::HF_HALF_SKIP:
				AddEntityToOwner( m_SkipFaceMesh );
				RemoveEntityInOwner( m_DefaultFaceMesh );
				m_CurFaceMesh = m_SkipFaceMesh;
				break;
			}
		}

		if( pItem->GetItemMaleCustom() > 0 || pItem->GetItemFemaleCustom() > 0 )
			bThread = false;    // 커스텀 되어있으면 즉시 로딩

		EquipSlotData( ES_HELMET, pHelmet->GetItemCode(), pHelmet->GetItemMaleCustom(), pHelmet->GetItemFemaleCustom() );
		EquipSlotEntity( ES_HELMET, AddEntityToOwner( pHelmet->GetEquipMesh(m_pOwnerChar), true, bThread ) );
	}

	if( pHelmet && pHelmet->HasSpringRope() )
	{
		m_pOwnerChar->AddRopeSpring( pHelmet->GetSpringBiped(), pHelmet->GetSpringParam() );
	}

	return NULL;
}

ioItem* ioEquipSlot::EquipCloakMesh( ioItem *pItem, bool bShow, bool bThread )
{
	// 사용시 주의!! 착용중인 Mesh가 없다는 전제하에 진행하는 함수 2009.03.04 KDH
	ioCloakItem *pCloak = ToCloakItem( pItem );

	if( pItem )
	{
		ioHashString szEquipEffect = pItem->GetItemEquipEffect(m_pOwnerChar->IsMale());
		if( !szEquipEffect.IsEmpty() )
		{
			m_pOwnerChar->AttachEffect( szEquipEffect );
		}

		pItem->SetExtendEquipEffect( m_pOwnerChar );
	}

	if( pItem->GetItemMaleCustom() > 0 || pItem->GetItemFemaleCustom() > 0 )
		bThread = false;    // 커스텀 되어있으면 즉시 로딩

	EquipSlotData( ES_CLOAK, pCloak->GetItemCode(), pCloak->GetItemMaleCustom(), pCloak->GetItemFemaleCustom() );
	EquipSlotEntity( ES_CLOAK, AddEntityToOwner( pCloak->GetEquipMesh(m_pOwnerChar), true, bThread ) );

	if( pCloak->HasSpringRope() )
	{
		m_pOwnerChar->AddRopeSpring( pCloak->GetSpringBiped(), pCloak->GetSpringParam() );
	}

	return NULL;
}

ioItem* ioEquipSlot::EquipObjectMesh( ioItem *pItem, bool bShow, bool bThread )
{
	// 사용시 주의!! 착용중인 Mesh가 없다는 전제하에 진행하는 함수 2009.03.04 KDH
	ioObjectItem *pObject = ToObjectItem( pItem );

	AddEntityToOwner( pObject->GetEquipMesh(m_pOwnerChar), false , bThread );
	
	ioWeaponItem *pWeapon = GetWeapon();
	if( pWeapon )
	{
		if( m_pOwnerChar )
			m_pOwnerChar->ShowWeaponItemMesh( false );
	}

	return NULL;
}

ioItem* ioEquipSlot::EquipWearMesh( ioItem *pItem, bool bShow, bool bThread )
{
	// 사용시 주의!! 착용중인 Mesh가 없다는 전제하에 진행하는 함수 2009.03.04 KDH
	// helmet wear
	if(ToHelmetWearItem( pItem ) )
	{
		return EquipHelmetWearOnHelmetMesh( pItem );
	}

	return NULL;
}

ioItem* ioEquipSlot::EquipHelmetWearOnHelmetMesh( ioItem *pItem)
{
	// 사용시 주의!! 착용중인 Mesh가 없다는 전제하에 진행하는 함수 2009.03.04 KDH
	// Helmet은 있을수 있음
	ioHelmetWearItem *pHelmetWear = ToHelmetWearItem( pItem );
	if( !pHelmetWear ) return NULL;

	ioHelmetWearItem::HelmetWearHair eWearHair = pHelmetWear->GetChangeHair();
	ioHelmetWearItem::HelmetWearFace eWearFace = pHelmetWear->GetChangeFace();

	ioHelmetItem *pHelemt = ToHelmetItem( m_EquipItemSlot[ES_HELMET] );
	if( !pHelemt )
	{
		switch( eWearHair )
		{
		case ioHelmetWearItem::HWH_HALF_SKIP:
			AddEntityToOwner( m_DefaultHalfHairMesh );
			RemoveEntityInOwner( m_DefaultHairMesh );
			break;
		case ioHelmetWearItem::HWH_ALL_SKIP:
			RemoveEntityInOwner( m_DefaultHairMesh );
			break;
		case ioHelmetWearItem::HWH_BIND_SKIP:
			AddEntityToOwner( m_DefaultBindHairMesh );
			RemoveEntityInOwner( m_DefaultHairMesh );
			break;
		case ioHelmetWearItem::HWH_NO_SKIP:
			break;
		}

		switch( eWearFace )
		{
		case ioHelmetWearItem::HWF_HALF_SKIP:
			AddEntityToOwner( m_SkipFaceMesh );
			RemoveEntityInOwner( m_DefaultFaceMesh );
			m_CurFaceMesh = m_SkipFaceMesh;
			break;
		}
	}
	else
	{
		bool bMale = true;
		if( m_pOwnerChar )
			bMale = m_pOwnerChar->IsMale();

		RemoveEntityInOwner( pHelemt->GetEquipMesh(m_pOwnerChar), true );

		ioHelmetItem::HelmetHair eHelmetHair = pHelemt->GetChangeHair( bMale );
		ioHelmetItem::HelmetFace eHelmetFace = pHelemt->GetChangeFace( bMale );

		switch( eWearHair )
		{
		case ioHelmetWearItem::HWH_HALF_SKIP:
			if( eHelmetHair == ioHelmetItem::HH_ALL_SKIP )
			{
				AddEntityToOwner( m_DefaultHalfHairMesh );
			}
			else if( eHelmetHair == ioHelmetItem::HH_BIND_SKIP )
			{
				AddEntityToOwner( m_DefaultHalfHairMesh );
				RemoveEntityInOwner( m_DefaultBindHairMesh );
			}
			else if( eHelmetHair == ioHelmetItem::HH_NO_SKIP )
			{
				AddEntityToOwner( m_DefaultHalfHairMesh );
				RemoveEntityInOwner( m_DefaultHairMesh );
			}
			break;
		case ioHelmetWearItem::HWH_ALL_SKIP:
			if( eHelmetHair == ioHelmetItem::HH_HALF_SKIP )
			{
				RemoveEntityInOwner( m_DefaultHalfHairMesh );
			}
			else if( eHelmetHair == ioHelmetItem::HH_BIND_SKIP )
			{
				RemoveEntityInOwner( m_DefaultBindHairMesh );
			}
			else if( eHelmetHair == ioHelmetItem::HH_NO_SKIP )
			{
				RemoveEntityInOwner( m_DefaultHairMesh );
			}
			break;
		case ioHelmetWearItem::HWH_BIND_SKIP:
			if( eHelmetHair == ioHelmetItem::HH_HALF_SKIP )
			{
				AddEntityToOwner( m_DefaultBindHairMesh );
				RemoveEntityInOwner( m_DefaultHalfHairMesh );
			}
			else if( eHelmetHair == ioHelmetItem::HH_ALL_SKIP )
			{
				AddEntityToOwner( m_DefaultBindHairMesh );
			}
			else if( eHelmetHair == ioHelmetItem::HH_NO_SKIP )
			{
				AddEntityToOwner( m_DefaultBindHairMesh );
				RemoveEntityInOwner( m_DefaultHairMesh );
			}
		case ioHelmetWearItem::HWH_NO_SKIP:
			if( eHelmetHair == ioHelmetItem::HH_HALF_SKIP )
			{
				AddEntityToOwner( m_DefaultHairMesh );
				RemoveEntityInOwner( m_DefaultHalfHairMesh );
			}
			else if( eHelmetHair == ioHelmetItem::HH_ALL_SKIP )
			{
				AddEntityToOwner( m_DefaultHairMesh );
			}
			else if( eHelmetHair == ioHelmetItem::HH_BIND_SKIP )
			{
				AddEntityToOwner( m_DefaultHairMesh );
				RemoveEntityInOwner( m_DefaultBindHairMesh );
			}
			break;
		}

		switch( eWearFace )
		{
		case ioHelmetWearItem::HWF_NONE:
			if( eHelmetFace == ioHelmetItem::HF_HALF_SKIP )
			{
				AddEntityToOwner( m_SkipFaceMesh );
				RemoveEntityInOwner( m_DefaultFaceMesh );
				m_CurFaceMesh = m_SkipFaceMesh;
			}
			break;
		case ioHelmetWearItem::HWF_HALF_SKIP:
			if( eHelmetFace == ioHelmetItem::HF_NONE )
			{
				AddEntityToOwner( m_DefaultFaceMesh );
				RemoveEntityInOwner( m_SkipFaceMesh );
				m_CurFaceMesh = m_DefaultFaceMesh;
			}
			break;
		}
	}
	
	AddEntityToOwner( pHelmetWear->GetEquipMesh(m_pOwnerChar) );

	return NULL;
}

void ioEquipSlot::ShowHeadMesh( ioItem *pItem, bool bShow )
{
	ioHelmetWearItem *pWear = ToHelmetWearItem( pItem );
	ioHelmetItem *pHelmet = ToHelmetItem( pItem );

	bool bMale = true;
	if( m_pOwnerChar )
		bMale = m_pOwnerChar->IsMale();

	if( pWear )
	{
		ioHelmetWearItem::HelmetWearHair eWearHair = pWear->GetChangeHair();

		if( !bShow )
		{
			switch( eWearHair )
			{
			case ioHelmetWearItem::HWH_HALF_SKIP:
				AddEntityToOwner( m_DefaultHairMesh );
				RemoveEntityInOwner( m_DefaultHalfHairMesh );
				break;
			case ioHelmetWearItem::HWH_ALL_SKIP:
				AddEntityToOwner( m_DefaultHairMesh );
				break;
			case ioHelmetWearItem::HWH_BIND_SKIP:
				AddEntityToOwner( m_DefaultHairMesh );
				RemoveEntityInOwner( m_DefaultBindHairMesh );
				break;
			case ioHelmetWearItem::HWH_NO_SKIP:
				break;
			}

			ioEntityGroup *pGrp = m_pOwnerChar->GetGroup();
			if( pGrp )
				pGrp->SetEntityVisible( m_DefaultHairMesh, false );
		}
		else
		{
			ioEntityGroup *pGrp = m_pOwnerChar->GetGroup();
			if( pGrp )
				pGrp->SetEntityVisible( m_DefaultHairMesh, true );

			switch( eWearHair )
			{
			case ioHelmetWearItem::HWH_HALF_SKIP:
				AddEntityToOwner( m_DefaultHalfHairMesh );
				RemoveEntityInOwner( m_DefaultHairMesh );
				break;
			case ioHelmetWearItem::HWH_ALL_SKIP:
				RemoveEntityInOwner( m_DefaultHairMesh );
				break;
			case ioHelmetWearItem::HWH_BIND_SKIP:
				AddEntityToOwner( m_DefaultBindHairMesh );
				RemoveEntityInOwner( m_DefaultHairMesh );
				break;
			case ioHelmetWearItem::HWH_NO_SKIP:
				break;
			}
		}
	}
	else if( pHelmet )
	{
		ioHelmetItem::HelmetHair eHairType = pHelmet->GetChangeHair( bMale );

		if( !bShow )
		{
			switch( eHairType )
			{
			case ioHelmetItem::HH_HALF_SKIP:
				AddEntityToOwner( m_DefaultHairMesh );
				RemoveEntityInOwner( m_DefaultHalfHairMesh );
				break;
			case ioHelmetItem::HH_ALL_SKIP:
				AddEntityToOwner( m_DefaultHairMesh );
				break;
			case ioHelmetItem::HH_BIND_SKIP:
				AddEntityToOwner( m_DefaultHairMesh );
				RemoveEntityInOwner( m_DefaultBindHairMesh );
				break;
			case ioHelmetItem::HH_NO_SKIP:
				break;
			}

			ioEntityGroup *pGrp = m_pOwnerChar->GetGroup();
			if( pGrp )
				pGrp->SetEntityVisible( m_DefaultHairMesh, false );
		}
		else
		{
			ioEntityGroup *pGrp = m_pOwnerChar->GetGroup();
			if( pGrp )
				pGrp->SetEntityVisible( m_DefaultHairMesh, true );

			switch( eHairType )
			{
			case ioHelmetItem::HH_HALF_SKIP:
				AddEntityToOwner( m_DefaultHalfHairMesh );
				RemoveEntityInOwner( m_DefaultHairMesh );
				break;
			case ioHelmetItem::HH_ALL_SKIP:
				RemoveEntityInOwner( m_DefaultHairMesh );
				break;
			case ioHelmetItem::HH_BIND_SKIP:
				AddEntityToOwner( m_DefaultBindHairMesh );
				RemoveEntityInOwner( m_DefaultHairMesh );
				break;
			case ioHelmetItem::HH_NO_SKIP:
				break;
			}
		}
	}
	else
	{
		RemoveEntityInOwner( m_DefaultHairMesh );
		AddEntityToOwner( m_DefaultHairMesh );

		if( !bShow )
		{
			ioEntityGroup *pGrp = m_pOwnerChar->GetGroup();
			if( pGrp )
				pGrp->SetEntityVisible( m_DefaultHairMesh, false );
		}
		else
		{
			ioEntityGroup *pGrp = m_pOwnerChar->GetGroup();
			if( pGrp )
				pGrp->SetEntityVisible( m_DefaultHairMesh, true );
		}
	}

	if( m_pOwnerChar )
	{
		m_pOwnerChar->ApplyModifiedMaterial();
	}
}

void ioEquipSlot::RestoreWeaponCustomize()
{
	if( !m_pOwnerChar ) return;
	ioWeaponItem *pWeapon = ToWeaponItem( m_EquipItemSlot[ES_WEAPON] );
	if( pWeapon )
	{
		ioHashString szMeshName = pWeapon->GetEquipMesh(m_pOwnerChar);
		if( szMeshName.IsEmpty() )	return;

		ioHashString szNewMeshName = Help::ConvertName( szMeshName, m_pOwnerChar->IsMale() );
		if( !m_pOwnerChar->IsMale() )
		{
			char szFullName[MAX_PATH]="";
			StringCbPrintf( szFullName, sizeof( szFullName ), "resource\\mesh\\%s.msh" , szNewMeshName.c_str() );
			if( !g_ResourceLoader.IsFileExist( szFullName ) )
			{
				szNewMeshName = Help::ConvertName( szMeshName, true );
			}
		}

		ioEntity *pEntity = m_pOwnerChar->GetGroup()->GetEntity( szNewMeshName );
		if( pEntity )
		{
			m_pOwnerChar->AddEntityEvent( pEntity );
		}
	}
}

void ioEquipSlot::RestoreArmorCustomize()
{
	if( !m_pOwnerChar ) return;
	ioArmorItem *pArmor = ToArmorItem( m_EquipItemSlot[ES_ARMOR] );
	if( pArmor )
	{
		ioHashString szMeshName = pArmor->GetBodyMesh(m_pOwnerChar);
		if( szMeshName.IsEmpty() )	return;

		ioHashString szNewMeshName = Help::ConvertName( szMeshName, m_pOwnerChar->IsMale() );
		if( !m_pOwnerChar->IsMale() )
		{
			char szFullName[MAX_PATH]="";
			StringCbPrintf( szFullName, sizeof( szFullName ), "resource\\mesh\\%s.msh" , szNewMeshName.c_str() );
			if( !g_ResourceLoader.IsFileExist( szFullName ) )
			{
				szNewMeshName = Help::ConvertName( szMeshName, true );
			}
		}

		ioEntity *pBodyEntity = m_pOwnerChar->GetGroup()->GetEntity( szNewMeshName );
		if( pBodyEntity )
		{
			m_pOwnerChar->AddEntityEvent( pBodyEntity );
		}
		
		if( !ToHookItem( GetWeapon() ) )
		{
			if( pArmor->HasHandMesh(m_pOwnerChar) )
			{
				szMeshName = pArmor->GetHandMesh(m_pOwnerChar);
				if( szMeshName.IsEmpty() )	return;

				szNewMeshName = Help::ConvertName( szMeshName, m_pOwnerChar->IsMale() );
				if( !m_pOwnerChar->IsMale() )
				{
					char szFullName[MAX_PATH]="";
					StringCbPrintf( szFullName, sizeof( szFullName ), "resource\\mesh\\%s.msh" , szNewMeshName.c_str() );
					if( !g_ResourceLoader.IsFileExist( szFullName ) )
					{
						szNewMeshName = Help::ConvertName( szMeshName, true );
					}
				}

				ioEntity *pHandEntity = m_pOwnerChar->GetGroup()->GetEntity( szNewMeshName );
				if( pHandEntity )
				{
					m_pOwnerChar->AddEntityEvent( pHandEntity );
				}
			}
		}
	}
}

void ioEquipSlot::RestoreHelmetCustomize()
{
	if( !m_pOwnerChar ) return;
	ioHelmetItem *pPreHelmet = ToHelmetItem( m_EquipItemSlot[ES_HELMET] );
	if( !pPreHelmet ) return;

	ioHashString szMeshName = pPreHelmet->GetEquipMesh(m_pOwnerChar);
	if( szMeshName.IsEmpty() )	return;

	ioHashString szNewMeshName = Help::ConvertName( szMeshName, m_pOwnerChar->IsMale() );
	if( !m_pOwnerChar->IsMale() )
	{
		char szFullName[MAX_PATH]="";
		StringCbPrintf( szFullName, sizeof( szFullName ), "resource\\mesh\\%s.msh" , szNewMeshName.c_str() );
		if( !g_ResourceLoader.IsFileExist( szFullName ) )
		{
			szNewMeshName = Help::ConvertName( szMeshName, true );
		}
	}

	ioEntity *pEntity = m_pOwnerChar->GetGroup()->GetEntity( szNewMeshName );
	if( pEntity )
	{
		m_pOwnerChar->AddEntityEvent( pEntity );
	}
}

void ioEquipSlot::RestoreCloakCustomize()
{
	if( !m_pOwnerChar ) return;
	ioCloakItem *pPreCloak = ToCloakItem( m_EquipItemSlot[ES_CLOAK] );
	if( !pPreCloak ) return;

	ioHashString szMeshName = pPreCloak->GetEquipMesh(m_pOwnerChar);
	if( szMeshName.IsEmpty() )	return;

	ioHashString szNewMeshName = Help::ConvertName( szMeshName, m_pOwnerChar->IsMale() );
	if( !m_pOwnerChar->IsMale() )
	{
		char szFullName[MAX_PATH]="";
		StringCbPrintf( szFullName, sizeof( szFullName ), "resource\\mesh\\%s.msh" , szNewMeshName.c_str() );
		if( !g_ResourceLoader.IsFileExist( szFullName ) )
		{
			szNewMeshName = Help::ConvertName( szMeshName, true );
		}
	}

	ioEntity *pEntity = m_pOwnerChar->GetGroup()->GetEntity( szNewMeshName );
	if( pEntity )
	{
		m_pOwnerChar->AddEntityEvent( pEntity );
	}
}

void ioEquipSlot::SetChangeEquipMesh( ioItem *pItem, bool bChangeEquip )
{
	if( !pItem )
		return;

	ioWeaponItem *pWeaponItem = ToWeaponItem( pItem );
	if( !pWeaponItem )
		return;

	ioHashString szCurMesh = pWeaponItem->GetEquipMesh(m_pOwnerChar);
	RemoveEntityInOwner( szCurMesh, true );

	pWeaponItem->SetChangeEquipMesh( bChangeEquip );

	bool bThread = true;
	if( pItem->GetItemMaleCustom() > 0 || pItem->GetItemFemaleCustom() > 0 )
		bThread = false;    // 커스텀 되어있으면 즉시 로딩

	EquipSlotEntity( ES_WEAPON, AddEntityToOwner( pWeaponItem->GetEquipMesh(m_pOwnerChar), true, bThread ) );
}

DWORD ioEquipSlot::StopEquipedPassiveSkill( ioHashStringVec &rkNoStopList )
{
	DWORD dwID = 0;
	for( int i=0; i < MAX_EQUIP_SLOT; ++i )
	{
		ioSkill *pSkill = m_EquipSkillSlot[i];
		if( pSkill )
		{
			ioPassiveSkill *pPassiveSkill = ToPassiveSkill( pSkill );
			if( pPassiveSkill && !CheckNoStopSkill( pSkill->GetName(), rkNoStopList ) )
			{
				dwID = pPassiveSkill->StopPassiveSkill( m_pOwnerChar );
			}
		}

		ioSkill *pSkillW = m_EquipSkillSlotW[i];
		if( pSkillW )
		{
			ioPassiveSkill *pPassiveSkill = ToPassiveSkill( pSkillW );
			if( pPassiveSkill && !CheckNoStopSkill( pSkillW->GetName(), rkNoStopList ) )
			{
				dwID = pPassiveSkill->StopPassiveSkill( m_pOwnerChar );
			}
		}
	}

	return dwID;
}

void ioEquipSlot::RestoreEquipedPassiveSkill( ioHashStringVec &rkNoStopList, const DWORD &dwID )
{
	for( int i=0; i < MAX_EQUIP_SLOT; ++i )
	{
		ioSkill *pSkill = m_EquipSkillSlot[i];
		ioSkill *pSkillW = m_EquipSkillSlotW[i];

		if( m_pOwnerChar->IsMale() )
		{
			if( ToPassiveSkill(pSkill) &&
				!CheckNoStopSkill( pSkill->GetName(), rkNoStopList ) &&
				pSkill->IsCanUseZeroHP( m_pOwnerChar ) )
			{
				ToPassiveSkill(pSkill)->RestorePassiveSkill( m_pOwnerChar, dwID );
			}
		}
		else
		{
			if( ToPassiveSkill(pSkillW) )
			{
				if( !CheckNoStopSkill( pSkillW->GetName(), rkNoStopList ) && pSkillW->IsCanUseZeroHP( m_pOwnerChar ) )
				{
					ToPassiveSkill(pSkillW)->RestorePassiveSkill( m_pOwnerChar, dwID );
				}
			}
			else if( ToPassiveSkill(pSkill) )
			{
				if( !CheckNoStopSkill( pSkill->GetName(), rkNoStopList ) && pSkill->IsCanUseZeroHP( m_pOwnerChar ) )
				{
					ToPassiveSkill(pSkill)->RestorePassiveSkill( m_pOwnerChar, dwID );
				}
			}
		}
	}
}

bool ioEquipSlot::CheckNoStopSkill( const ioHashString& szSkill, ioHashStringVec &rkNoStopList )
{
	if( rkNoStopList.empty() )
		return false;

	ioHashStringVec::iterator iter = rkNoStopList.begin();
	while( iter != rkNoStopList.end() )
	{
		if( (*iter) == szSkill )
			return true;

		++iter;
	}

	return false;
}

void ioEquipSlot::ShowFaceMesh( bool bShow )
{
	if( !m_pOwnerChar )
		return;

	ioEntityGroup *pGrp = m_pOwnerChar->GetGroup();
	if( !pGrp )
		return;

	if( !m_pOwnerChar->IsInvisibleHead() )
	{
		pGrp->SetEntityVisible( m_DefaultHairMesh, bShow );
		pGrp->SetEntityVisible( m_DefaultBindHairMesh, bShow );
		pGrp->SetEntityVisible( m_DefaultHalfHairMesh, bShow );
	}

	if( m_pOwnerChar )
	{
		m_pOwnerChar->ApplyModifiedMaterial();
	}

	if( bShow )
	{
		pGrp->SetEntityVisible( m_CurFaceMesh, true );
	}
	else
	{
		pGrp->SetEntityVisible( m_CurFaceMesh, false );
	}
}

DWORD ioEquipSlot::GetItemCustomIndex( bool bMale, int iSlot )
{
	switch( iSlot )
	{
	case ES_WEAPON:
		if( bMale )
			return m_ItemSlot[ES_WEAPON].dwItemMaleCustom;
		else
			return m_ItemSlot[ES_WEAPON].dwItemFemaleCustom;
		break;
	case ES_ARMOR:
		if( bMale )
			return m_ItemSlot[ES_ARMOR].dwItemMaleCustom;
		else
			return m_ItemSlot[ES_ARMOR].dwItemFemaleCustom;
		break;
	case ES_HELMET:
		if( bMale )
			return m_ItemSlot[ES_HELMET].dwItemMaleCustom;
		else
			return m_ItemSlot[ES_HELMET].dwItemFemaleCustom;
		break;
	case ES_CLOAK:
		if( bMale )
			return m_ItemSlot[ES_CLOAK].dwItemMaleCustom;
		else
			return m_ItemSlot[ES_CLOAK].dwItemFemaleCustom;
		break;
	}

	return 0;
}

ioItem* ioEquipSlot::EquipRing( ioItem *pItem, bool bThread /*= false */ )
{
	ioItem *pPreItem = ReleaseItem( ES_RING );
	ioRingItem *pRing = ToRingItem( pItem );

	if( pItem->GetItemMaleCustom() > 0 || pItem->GetItemFemaleCustom() > 0 )
		bThread = false;    // 커스텀 되어있으면 즉시 로딩

	EquipSlotData( ES_RING, pItem->GetItemCode(), pItem->GetItemMaleCustom(), pItem->GetItemFemaleCustom() );
	m_EquipItemSlot[ES_RING] = pItem;

	AddEquipedSkill( ES_RING, pItem );
	pItem->OnEquiped( m_pOwnerChar );

	UpdateSetItemLev();
	m_pOwnerChar->RecalcStatusValue();

	return pPreItem;
}

ioItem* ioEquipSlot::EquipNecklace( ioItem *pItem, bool bThread /*= false */ )
{
	ioItem *pPreItem = ReleaseItem( ES_NECKLACE );
	ioNecklaceItem *pNecklace = ToNecklaceItem( pItem );

	if( pItem->GetItemMaleCustom() > 0 || pItem->GetItemFemaleCustom() > 0 )
		bThread = false;    // 커스텀 되어있으면 즉시 로딩

	EquipSlotData( ES_NECKLACE, pItem->GetItemCode(), pItem->GetItemMaleCustom(), pItem->GetItemFemaleCustom() );
	m_EquipItemSlot[ES_NECKLACE] = pItem;

	AddEquipedSkill( ES_NECKLACE, pItem );
	pItem->OnEquiped( m_pOwnerChar );

	UpdateSetItemLev();
	m_pOwnerChar->RecalcStatusValue();

	return pPreItem;
}

ioItem* ioEquipSlot::EquipBracelet( ioItem *pItem, bool bThread /*= false */ )
{
	ioItem *pPreItem = ReleaseItem( ES_BRACELET );
	ioBraceletItem *pBracelet = ToBraceletItem( pItem );

	if( pItem->GetItemMaleCustom() > 0 || pItem->GetItemFemaleCustom() > 0 )
		bThread = false;    // 커스텀 되어있으면 즉시 로딩

	EquipSlotData( ES_BRACELET, pItem->GetItemCode(), pItem->GetItemMaleCustom(), pItem->GetItemFemaleCustom() );
	m_EquipItemSlot[ES_BRACELET] = pItem;

	AddEquipedSkill( ES_BRACELET, pItem );
	pItem->OnEquiped( m_pOwnerChar );

	UpdateSetItemLev();
	m_pOwnerChar->RecalcStatusValue();

	return pPreItem;
}

ioItem* ioEquipSlot::ReleaseRingSlot()
{
	ioRingItem *pRing = ToRingItem( m_EquipItemSlot[ES_RING] );
	if( pRing )
	{
		ReleaseSlotData( ES_RING );
		RemoveEquipedSkillBuffInOwner( ES_RING );
		RemoveEquipedSkill( ES_RING );
		pRing->OnReleased( m_pOwnerChar );
		m_EquipItemSlot[ES_RING] = NULL;
		UpdateSetItemLev();
		m_pOwnerChar->RecalcStatusValue();
	}

	return pRing;
}

ioItem* ioEquipSlot::ReleaseNecklaceSlot()
{
	ioNecklaceItem *pNecklaceItem = ToNecklaceItem( m_EquipItemSlot[ES_NECKLACE] );
	if( pNecklaceItem )
	{
		ReleaseSlotData( ES_NECKLACE );
		RemoveEquipedSkillBuffInOwner( ES_NECKLACE );
		RemoveEquipedSkill( ES_NECKLACE );
		pNecklaceItem->OnReleased( m_pOwnerChar );
		m_EquipItemSlot[ES_NECKLACE] = NULL;
		UpdateSetItemLev();
		m_pOwnerChar->RecalcStatusValue();
	}

	return pNecklaceItem;
}

ioItem* ioEquipSlot::ReleaseBraceletSlot()
{
	ioBraceletItem *pBracelet = ToBraceletItem( m_EquipItemSlot[ES_BRACELET] );
	if( pBracelet )
	{
		ReleaseSlotData( ES_BRACELET );
		RemoveEquipedSkillBuffInOwner( ES_BRACELET );
		RemoveEquipedSkill( ES_BRACELET );
		pBracelet->OnReleased( m_pOwnerChar );
		m_EquipItemSlot[ES_BRACELET] = NULL;
		UpdateSetItemLev();
		m_pOwnerChar->RecalcStatusValue();
	}

	return pBracelet;
}