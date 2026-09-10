

#include "stdafx.h"

#include <strsafe.h>

#include "ioCreateChar.h"
#include "ioUICharAniEventHandler.h"

ioUICharAniEventHandler::ioUICharAniEventHandler()
{
	m_pChar = NULL;
}

ioUICharAniEventHandler::~ioUICharAniEventHandler()
{
	m_ShopEffectList.clear();
	m_ShopSoundList.clear();
}

void ioUICharAniEventHandler::SetCreateChar( ioCreateChar *pChar )
{
	m_pChar = pChar;
}

bool ioUICharAniEventHandler::EventHandling( const AniEventTime *pEvent, const ioAnimation *pNotifier )
{
	if( ioAniEventHandler::EventHandling( pEvent, pNotifier ) )
		return true;

	if( pEvent->szEventType == "face_change" )
	{
		OnFaceChange( pEvent, pNotifier );
		return true;
	}
	else if( pEvent->szEventType == "auto_effect" )
	{
		OnAutoEffect( pEvent, pNotifier );
		return true;
	}
	else if( pEvent->szEventType == "shop_effect" )
	{
		OnShopEffect( pEvent, pNotifier );
		return true;
	}
	else if( pEvent->szEventType == "shop_sound" )
	{
		if( IsCanShopSound() )
		{
			OnShopSound( pEvent, pNotifier );
		}
		return true;
	}
	else if( pEvent->szEventType == "show_weapon" )
	{
		OnShowWeaponMesh( pEvent, pNotifier );
		return true;
	}
	else if( pEvent->szEventType == "show_armor" )
	{
		OnShowArmorMesh( pEvent, pNotifier );
		return true;
	}
	else if( pEvent->szEventType == "show_helm" )
	{
		OnShowHelmMesh( pEvent, pNotifier );
		return true;
	}
	else if( pEvent->szEventType == "show_cloak" )
	{
		OnShowCloakMesh( pEvent, pNotifier );
		return true;
	}
	else if( pEvent->szEventType == "show_hair" )
	{
		OnShowHairMesh( pEvent, pNotifier );
		return true;
	}
	else if( pEvent->szEventType == "hide_weapon" )
	{
		OnHideWeaponMesh( pEvent, pNotifier );
		return true;
	}
	else if( pEvent->szEventType == "hide_armor" )
	{
		OnHideArmorMesh( pEvent, pNotifier );
		return true;
	}
	else if( pEvent->szEventType == "hide_helm" )
	{
		OnHideHelmMesh( pEvent, pNotifier );
		return true;
	}
	else if( pEvent->szEventType == "hide_cloak" )
	{
		OnHideCloakMesh( pEvent, pNotifier );
		return true;
	}
	else if( pEvent->szEventType == "hide_hair" )
	{
		OnHideHairMesh( pEvent, pNotifier );
		return true;
	}
	return false;
}

void ioUICharAniEventHandler::OnFaceChange( const AniEventTime *pEvent, const ioAnimation *pNotifier )
{
	const StringVector &vParam = ioStringConverter::Split( pEvent->szSubInfo.c_str(), " " );

	if( vParam.size() < 2 )
	{
		ioHashString szAniName;
		if ( pNotifier )
			szAniName = pNotifier->GetName();
		LOG.PrintTimeAndLog( 0, "ioUICharAniEventHandler::OnFaceChange - %s Param size Error( Expected least 2 ), Name : %s",
								pEvent->szSubInfo.c_str(), szAniName );
		return;
	}

	int iDuration = ioStringConverter::ParseInt( vParam[1].c_str() ) * pNotifier->GetTimeFactor();

	ioHashString szType( "all" );
	if( vParam.size() >= 3 )
	{
		szType = vParam[2].c_str();
	}

	m_pChar->FaceChange( vParam[0].c_str(), iDuration, szType );
}

void ioUICharAniEventHandler::OnAutoEffect( const AniEventTime *pEvent, const ioAnimation *pNotifier )
{
	m_pChar->AttachEffect( pEvent->szSubInfo, NULL );
}

void ioUICharAniEventHandler::OnShopEffect( const AniEventTime *pEvent, const ioAnimation *pNotifier )
{
	if( !m_pChar ) return;

	AniEventInfoList::iterator iter = m_ShopEffectList.begin();
	while( iter != m_ShopEffectList.end() )
	{
		if( (*iter).m_EventName == pEvent->szSubInfo )
		{
			m_pChar->EndEffect( pEvent->szSubInfo, false );
			m_ShopEffectList.erase( iter );
			break;
		}
		else
		{
			++iter;
		}
	}

	m_pChar->AttachEffect( pEvent->szSubInfo, NULL );

	AniEventInfo kInfo;
	kInfo.m_AniName = pNotifier->GetName();
	kInfo.m_EventName = pEvent->szSubInfo;

	m_ShopEffectList.push_back( kInfo );
}

void ioUICharAniEventHandler::OnShopSound( const AniEventTime *pEvent, const ioAnimation *pNotifier )
{
	AniSoundEventInfoList::iterator iter = m_ShopSoundList.begin();
	while( iter != m_ShopSoundList.end() )
	{
		if( (*iter).m_EventName == pEvent->szSubInfo )
		{
			g_SoundMgr.StopSound( (*iter).m_EventName, (*iter).m_SoundID );
			//g_SoundMgr.StopSound( (*iter).m_EventName, 0 );
			iter = m_ShopSoundList.erase( iter );
		}
		else
		{
			++iter;
		}
	}

	UniqueObjID eSoundID = g_SoundMgr.PlaySound( pEvent->szSubInfo, m_pOwner->GetParentSceneNode() );

	AniSoundEventInfo kInfo;
	kInfo.m_AniName = pNotifier->GetName();
	kInfo.m_EventName = pEvent->szSubInfo;
	kInfo.m_SoundID = eSoundID;

	m_ShopSoundList.push_back( kInfo );
}

void ioUICharAniEventHandler::OnAnimationClear( const AniEventTime *pEvent, const ioAnimation *pNotifier )
{
	// Hide 메쉬 복귀
	if( !m_HideMeshList.empty() )
	{
		AniEventInfoList::iterator iter = m_HideMeshList.begin();
		while( iter != m_HideMeshList.end() )
		{
			AniEventInfo kInfo = *iter;

			if( kInfo.m_AniName == pNotifier->GetName() )
			{
				m_pOwner->SetEntityVisible( kInfo.m_EventName, true );
				iter = m_HideMeshList.erase( iter );
			}
			else
			{
				++iter;
			}
		}
	}

	// Hide 메쉬에 따른 이펙트 복귀
	if( !m_HideMeshLinkEffectList.empty() )
	{
		AniEventInfoList::iterator iter = m_HideMeshLinkEffectList.begin();
		while( iter != m_HideMeshLinkEffectList.end() )
		{
			AniEventInfo kInfo = *iter;

			if( kInfo.m_AniName == pNotifier->GetName() )
			{
				m_pChar->AttachEffect( kInfo.m_EventName, NULL );
				iter = m_HideMeshLinkEffectList.erase( iter );
			}
			else
			{
				++iter;
			}
		}
	}

	// ShopEffect 제거
	if( !m_ShopEffectList.empty() )
	{
		AniEventInfoList::iterator iter = m_ShopEffectList.begin();
		while( iter != m_ShopEffectList.end() )
		{
			AniEventInfo kInfo = *iter;

			if( kInfo.m_AniName == pNotifier->GetName() )
			{
				m_pOwner->EndEffect( kInfo.m_EventName, false );
				iter = m_ShopEffectList.erase( iter );
			}
			else
			{
				++iter;
			}
		}
	}

	// ShopSound 제거
	if( !m_ShopSoundList.empty() )
	{
		AniSoundEventInfoList::iterator iter = m_ShopSoundList.begin();
		while( iter != m_ShopSoundList.end() )
		{
			AniSoundEventInfo kInfo = *iter;

			if( kInfo.m_AniName == pNotifier->GetName() )
			{
				g_SoundMgr.StopSound( kInfo.m_EventName, kInfo.m_SoundID );
				iter = m_ShopSoundList.erase( iter );
			}
			else
			{
				++iter;
			}
		}
	}
}

bool ioUICharAniEventHandler::IsCanShopSound()
{
	if( !m_pChar )
		return false;

	ioHashString szName = m_pChar->GetName();

	if( szName == "HireChar" ||
		szName == "ShopChar" ||
		szName == "AwardChar" )
	{
		return true;
	}

	return false;
}

ioEntity* ioUICharAniEventHandler::GetWeaponEntity()
{
	if( !m_pChar )	return NULL;

	const ioWeaponItem *pWeapon = ToWeaponItemConst( m_pChar->GetSlotItem( ioCreateChar::CCS_WEAPON ) );
	if( !pWeapon )	return NULL;

	return m_pOwner->GetEntity( pWeapon->GetEquipMesh(NULL) );
}

ioEntity* ioUICharAniEventHandler::GetHandEntity()
{
	if( !m_pChar )	return NULL;

	ioHashString szMeshName;

	const ioArmorItem *pArmor = ToArmorItemConst( m_pChar->GetSlotItem( ioCreateChar::CCS_ARMOR ) );
	if( pArmor )
	{
		if( pArmor->HasHandMesh(NULL) )
			szMeshName = pArmor->GetHandMesh(NULL);
		else
			szMeshName = pArmor->GetBodyMesh(NULL);
	}
	else
	{
		szMeshName = m_pChar->GetDefaultHandMesh();
	}

	const CostumeInfo *pCostumeInfo = m_pChar->GetCostumeSlot( ioCreateChar::CCS_ARMOR );
	if ( pCostumeInfo && pCostumeInfo->m_nCostumeCode > 0 )
	{
		if ( !pCostumeInfo->m_szHandMesh.IsEmpty() )
			szMeshName = pCostumeInfo->m_szHandMesh;
		else
			szMeshName = pCostumeInfo->m_szBodyMesh;
	}

	if( szMeshName.IsEmpty() )
		return NULL;

	return m_pOwner->GetEntity( szMeshName );
}

ioEntity* ioUICharAniEventHandler::GetLegEntity()
{
	if( !m_pChar )	return NULL;

	ioHashString szMeshName;

	const ioArmorItem *pArmor = ToArmorItemConst( m_pChar->GetSlotItem( ioCreateChar::CCS_ARMOR ) );
	if( pArmor )
	{
		szMeshName = pArmor->GetBodyMesh(NULL);
	}
	else
	{
		szMeshName = m_pChar->GetDefaultUnderwearMesh();
	}

	const CostumeInfo *pCostumeInfo = m_pChar->GetCostumeSlot( ioCreateChar::CCS_ARMOR );
	if ( pCostumeInfo && pCostumeInfo->m_nCostumeCode > 0 )
		szMeshName = pCostumeInfo->m_szBodyMesh;

	if( szMeshName.IsEmpty() )
		return NULL;

	return m_pOwner->GetEntity( szMeshName );
}


void ioUICharAniEventHandler::OnShowWeaponMesh( const AniEventTime *pEvent, const ioAnimation *pNotifier )
{
	if( !m_pChar )	return;

	const ioWeaponItem *pWeapon = ToWeaponItemConst( m_pChar->GetSlotItem( ioCreateChar::CCS_WEAPON ) );
	if( !pWeapon ) return;

	ShowMesh( pWeapon->GetEquipMesh(NULL), pWeapon->GetItemEquipEffect(m_pChar->IsMale()), pNotifier );
}

void ioUICharAniEventHandler::OnShowArmorMesh( const AniEventTime *pEvent, const ioAnimation *pNotifier )
{
	if( !m_pChar )	return;

	ioHashString szBodyMesh;
	ioHashString szHandMesh;
	ioHashString szEffect;

	const ioArmorItem *pArmor = ToArmorItemConst( m_pChar->GetSlotItem( ioCreateChar::CCS_ARMOR ) );
	if( !pArmor ) return;

	szBodyMesh = pArmor->GetBodyMesh(NULL);
	szHandMesh = pArmor->GetHandMesh(NULL);
	szEffect = pArmor->GetItemEquipEffect(m_pChar->IsMale());

	const CostumeInfo *pCostumeInfo = m_pChar->GetCostumeSlot( ioCreateChar::CCS_ARMOR );
	if ( pCostumeInfo && pCostumeInfo->m_nCostumeCode > 0 )
	{
		szBodyMesh = pCostumeInfo->m_szBodyMesh;
		szHandMesh = pCostumeInfo->m_szHandMesh;
		szEffect = m_pChar->GetCostumeEffect( pCostumeInfo );
	}

	if( strcmp( pEvent->szSubInfo.c_str(), "body" ) == 0 )
		ShowMesh( szBodyMesh, szEffect, pNotifier );
	else if( strcmp( pEvent->szSubInfo.c_str(), "hand" ) == 0 )
		ShowMesh( szHandMesh, szEffect, pNotifier );
	else
	{
		ShowMesh( szBodyMesh, szEffect, pNotifier );
		ShowMesh( szHandMesh, szEffect, pNotifier );
	}
}

void ioUICharAniEventHandler::OnShowHelmMesh( const AniEventTime *pEvent, const ioAnimation *pNotifier )
{
	if( !m_pChar )	return;

	ioHashString szHelmetMesh;
	ioHashString szEffect;

	const ioHelmetItem *pHelmetItem = ToHelmetItemConst( m_pChar->GetSlotItem( ioCreateChar::CCS_HELMET ) );
	if( !pHelmetItem ) return;

	szHelmetMesh = pHelmetItem->GetEquipMesh(NULL);
	szEffect = pHelmetItem->GetItemEquipEffect(m_pChar->IsMale());

	const CostumeInfo *pCostumeInfo = m_pChar->GetCostumeSlot( ioCreateChar::CCS_HELMET );
	if ( pCostumeInfo && pCostumeInfo->m_nCostumeCode > 0 )
	{
		szHelmetMesh = pCostumeInfo->m_szHelmetMesh;
		szEffect = m_pChar->GetCostumeEffect( pCostumeInfo );
	}

	ShowMesh( szHelmetMesh, szEffect, pNotifier );
}

void ioUICharAniEventHandler::OnShowCloakMesh( const AniEventTime *pEvent, const ioAnimation *pNotifier )
{
	if( !m_pChar )	return;

	ioHashString szCloakMesh;
	ioHashString szEffect;

	const ioCloakItem *pCloakItem = ToCloakItemConst( m_pChar->GetSlotItem( ioCreateChar::CCS_CLOAK ) );
	if( !pCloakItem ) return;

	szCloakMesh = pCloakItem->GetEquipMesh(NULL);
	szEffect = pCloakItem->GetItemEquipEffect(m_pChar->IsMale());

	const CostumeInfo *pCostumeInfo = m_pChar->GetCostumeSlot( ioCreateChar::CCS_CLOAK );
	if ( pCostumeInfo && pCostumeInfo->m_nCostumeCode > 0 )
	{
		szCloakMesh = pCostumeInfo->m_szCloakMesh;
		szEffect = m_pChar->GetCostumeEffect( pCostumeInfo );
	}

	ShowMesh( szCloakMesh, szEffect, pNotifier );
}


void ioUICharAniEventHandler::OnShowHairMesh( const AniEventTime *pEvent, const ioAnimation *pNotifier )
{
	if( !m_pChar || !m_pChar->GetGroup() )
		return;

	ioEntity *pHair =m_pChar->GetGroup()->GetEntity( m_pChar->GetDefaultHairMesh() );
	if( !pHair )
	{
		pHair = m_pChar->GetGroup()->GetEntity( m_pChar->GetDefaultBindHairMesh() );

		if( !pHair )
			pHair = m_pChar->GetGroup()->GetEntity( m_pChar->GetDefaultHalfHairMesh() );
	}

	if( !pHair )
		return;//이러면???

	ShowMesh( pHair->GetMeshName(), "", pNotifier );
}

void ioUICharAniEventHandler::ShowMesh( const ioHashString &rkMeshName, const ioHashString &rkEffectName, const ioAnimation *pNotifier )
{
	ShowMeshLinkEffect( rkEffectName, pNotifier );

	if( !rkMeshName.IsEmpty() )
	{
		// 메쉬
		ioHashString szNewMeshName = Help::ConvertName( rkMeshName, m_pChar->IsMale() );
		if( !m_pChar->IsMale() )
		{
			char szFullName[MAX_PATH]="";
			StringCbPrintf( szFullName, sizeof( szFullName ), "resource\\mesh\\%s.msh" , szNewMeshName.c_str() );
			if( !g_ResourceLoader.IsFileExist( szFullName ) )
			{
				szNewMeshName = Help::ConvertName( rkMeshName, true );
			}
		}

		m_pOwner->SetEntityVisible( szNewMeshName, true );

		if( !m_HideMeshList.empty() )
		{
			AniEventInfoList::iterator iter;

			for( iter=m_HideMeshList.begin(); iter != m_HideMeshList.end(); ++iter )
			{
				AniEventInfo kInfo = *iter;

				if( szNewMeshName == kInfo.m_EventName )
					break;
			}

			if( iter != m_HideMeshList.end() )
			{
				m_HideMeshList.erase( iter );
			}
		}
	}	
}

void ioUICharAniEventHandler::ShowMeshLinkEffect( const ioHashString &rkEffectName, const ioAnimation *pNotifier )
{
	if( rkEffectName.IsEmpty() ) return;

	// 이펙트
	m_pOwner->AttachEffect( rkEffectName, NULL );
	if( !m_HideMeshLinkEffectList.empty() )
	{
		AniEventInfoList::iterator iter;

		for( iter=m_HideMeshLinkEffectList.begin(); iter != m_HideMeshLinkEffectList.end(); ++iter )
		{
			AniEventInfo kInfo = *iter;

			if( rkEffectName == kInfo.m_EventName )
				break;
		}

		if( iter != m_HideMeshLinkEffectList.end() )
		{
			m_HideMeshLinkEffectList.erase( iter );
		}
	}
}

void ioUICharAniEventHandler::OnHideWeaponMesh( const AniEventTime *pEvent, const ioAnimation *pNotifier )
{
	if( !m_pChar )	return;

	const StringVector &vParam = ioStringConverter::Split( pEvent->szSubInfo.c_str(), " " );
	int iReturnSize = vParam.size();
	for(int i = 0;i < iReturnSize;i++)
	{
		if( (int)m_pChar->GetClassType() == atoi( vParam[i].c_str() ) )
			return;
	}

	const ioWeaponItem *pWeapon = ToWeaponItemConst( m_pChar->GetSlotItem( ioCreateChar::CCS_WEAPON ) );
	if( !pWeapon ) return;

	HideMesh( pWeapon->GetEquipMesh(NULL), pWeapon->GetItemEquipEffect(m_pChar->IsMale()), pNotifier );
}

void ioUICharAniEventHandler::OnHideArmorMesh( const AniEventTime *pEvent, const ioAnimation *pNotifier )
{
	if( !m_pChar )	return;

	char szPart[MAX_PATH] = "";
	const StringVector &vParam = ioStringConverter::Split( pEvent->szSubInfo.c_str(), " " );
	int iReturnSize = vParam.size();
	for(int i = 0;i < iReturnSize;i++)
	{
		if( i == 0 )
		{
			sprintf_e( szPart, "%s", vParam[i].c_str() );
		}
		else if( (int)m_pChar->GetEquipedItemCode( ioCreateChar::CCS_WEAPON ) == atoi( vParam[i].c_str() ) )
		{
			return;
		}
	}

	ioHashString szBodyMesh;
	ioHashString szHandMesh;
	ioHashString szEffect;

	const ioArmorItem *pArmorItem = ToArmorItemConst( m_pChar->GetSlotItem( ioCreateChar::CCS_ARMOR ) );
	if( !pArmorItem ) return;

	szBodyMesh = pArmorItem->GetBodyMesh(NULL);
	szHandMesh = pArmorItem->GetHandMesh(NULL);
	szEffect = pArmorItem->GetItemEquipEffect(m_pChar->IsMale());

	const CostumeInfo *pCostumeInfo = m_pChar->GetCostumeSlot( ioCreateChar::CCS_ARMOR );
	if ( pCostumeInfo && pCostumeInfo->m_nCostumeCode > 0 )
	{
		szBodyMesh = pCostumeInfo->m_szBodyMesh;
		szHandMesh = pCostumeInfo->m_szHandMesh;
		szEffect = m_pChar->GetCostumeEffect( pCostumeInfo );
	}

	if( strcmp( szPart, "body" ) == 0 )
	{
		HideMesh( szBodyMesh, szEffect, pNotifier );
	}
	else if( strcmp( szPart, "hand" ) == 0 )
	{
		HideMesh( szHandMesh, szEffect, pNotifier );
	}
	else
	{
		HideMesh( szBodyMesh, szEffect, pNotifier );
		HideMesh( szHandMesh, szEffect, pNotifier );
	}
}

void ioUICharAniEventHandler::OnHideHelmMesh( const AniEventTime *pEvent, const ioAnimation *pNotifier )
{
	if( !m_pChar )	return;

	const StringVector &vParam = ioStringConverter::Split( pEvent->szSubInfo.c_str(), " " );
	int iReturnSize = vParam.size();
	for(int i = 0;i < iReturnSize;i++)
	{
		if( (int)m_pChar->GetEquipedItemCode( ioCreateChar::CCS_WEAPON ) == atoi( vParam[i].c_str() ) )
			return;
	}

	ioHashString szHelmetMesh;
	ioHashString szEffect;

	const ioHelmetItem *pHelmetItem = ToHelmetItemConst( m_pChar->GetSlotItem( ioCreateChar::CCS_HELMET ) );
	if( !pHelmetItem ) return;

	szHelmetMesh = pHelmetItem->GetEquipMesh(NULL);
	szEffect = pHelmetItem->GetItemEquipEffect(m_pChar->IsMale());

	const CostumeInfo *pCostumeInfo = m_pChar->GetCostumeSlot( ioCreateChar::CCS_HELMET );
	if ( pCostumeInfo && pCostumeInfo->m_nCostumeCode > 0 )
	{
		szHelmetMesh = pCostumeInfo->m_szHelmetMesh;
		szEffect = m_pChar->GetCostumeEffect( pCostumeInfo );
	}

	HideMesh( szHelmetMesh, szEffect, pNotifier );
}

void ioUICharAniEventHandler::OnHideCloakMesh( const AniEventTime *pEvent, const ioAnimation *pNotifier )
{
	if( !m_pChar )	return;

	const StringVector &vParam = ioStringConverter::Split( pEvent->szSubInfo.c_str(), " " );
	int iReturnSize = vParam.size();
	for(int i = 0;i < iReturnSize;i++)
	{
		if( (int)m_pChar->GetEquipedItemCode( ioCreateChar::CCS_WEAPON ) == atoi( vParam[i].c_str() ) )
			return;
	}

	ioHashString szCloakMesh;
	ioHashString szEffect;

	const ioCloakItem *pCloakItem = ToCloakItemConst( m_pChar->GetSlotItem( ioCreateChar::CCS_CLOAK ) );
	if( !pCloakItem ) return;

	szCloakMesh = pCloakItem->GetEquipMesh(NULL);
	szEffect = pCloakItem->GetItemEquipEffect(m_pChar->IsMale());

	const CostumeInfo *pCostumeInfo = m_pChar->GetCostumeSlot( ioCreateChar::CCS_CLOAK );
	if ( pCostumeInfo && pCostumeInfo->m_nCostumeCode > 0 )
	{
		szCloakMesh = pCostumeInfo->m_szCloakMesh;
		szEffect = m_pChar->GetCostumeEffect( pCostumeInfo );
	}

	HideMesh( szCloakMesh, szEffect, pNotifier );
}


void ioUICharAniEventHandler::OnHideHairMesh( const AniEventTime *pEvent, const ioAnimation *pNotifier )
{
	if( !m_pChar || !m_pChar->GetGroup() )
		return;

	ioEntity *pHair =m_pChar->GetGroup()->GetEntity( m_pChar->GetDefaultHairMesh() );
	if( !pHair )
	{
		pHair = m_pChar->GetGroup()->GetEntity( m_pChar->GetDefaultBindHairMesh() );

		if( !pHair )
			pHair = m_pChar->GetGroup()->GetEntity( m_pChar->GetDefaultHalfHairMesh() );
	}

	if( !pHair )
		return;//이러면???

	HideMesh( pHair->GetMeshName(), "", pNotifier );
}

void ioUICharAniEventHandler::HideMesh( const ioHashString &rkMeshName, const ioHashString &rkEffectName, const ioAnimation *pNotifier )
{
	HideMeshLinkEffect( rkEffectName, pNotifier );

	if( !rkMeshName.IsEmpty() )
	{
		ioHashString szNewMeshName = Help::ConvertName( rkMeshName, m_pChar->IsMale() );
		if( !m_pChar->IsMale() )
		{
			char szFullName[MAX_PATH]="";
			StringCbPrintf( szFullName, sizeof( szFullName ), "resource\\mesh\\%s.msh" , szNewMeshName.c_str() );
			if( !g_ResourceLoader.IsFileExist( szFullName ) )
			{
				szNewMeshName = Help::ConvertName( rkMeshName, true );
			}
		}

		m_pOwner->SetEntityVisible( szNewMeshName, false );

		bool bFind = false;
		AniEventInfoList::iterator iter = m_HideMeshList.begin();
		while( iter != m_HideMeshList.end() )
		{
			if( (*iter).m_EventName == szNewMeshName )
			{
				bFind = true;
				break;
			}
			else
			{
				++iter;
			}
		}

		if( bFind )
		{
			(*iter).m_AniName = pNotifier->GetName();
			return;
		}

		AniEventInfo kInfo;
		kInfo.m_AniName   = pNotifier->GetName();
		kInfo.m_EventName = szNewMeshName;

		m_HideMeshList.push_back( kInfo );
		pNotifier->SetEndEventNotify();
	}
}

void ioUICharAniEventHandler::HideMeshLinkEffect( const ioHashString &rkEffectName, const ioAnimation *pNotifier )
{
	if( rkEffectName.IsEmpty() )
		return;

	m_pChar->EndEffect( rkEffectName, false );

	bool bFind = false;
	AniEventInfoList::iterator iter = m_HideMeshLinkEffectList.begin();
	while( iter != m_HideMeshLinkEffectList.end() )
	{
		if( (*iter).m_EventName == rkEffectName )
		{
			bFind = true;
			break;
		}
		else
		{
			++iter;
		}
	}

	if( bFind )
	{
		(*iter).m_AniName = pNotifier->GetName();
		return;
	}

	AniEventInfo kInfo;
	kInfo.m_AniName   = pNotifier->GetName();
	kInfo.m_EventName = rkEffectName;

	m_HideMeshLinkEffectList.push_back( kInfo );
}