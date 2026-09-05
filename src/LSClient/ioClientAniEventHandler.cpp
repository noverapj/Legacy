

#include "stdafx.h"

#include <strsafe.h>

#include "ioClientAniEventHandler.h"

#include "ioNpcChar.h"

ioClientAniEventHandler::ioClientAniEventHandler()
{
	m_pChar = NULL;
	m_bItemMeshHide = false;
}

ioClientAniEventHandler::~ioClientAniEventHandler()
{
	m_ShopEffectList.clear();
	m_ShopSoundList.clear();
	m_HideMeshLinkEffectList.clear();
}

void ioClientAniEventHandler::SetBaseChar( ioBaseChar *pChar )
{
	m_pChar = pChar;
}

bool ioClientAniEventHandler::EventHandling( const AniEventTime *pEvent,
											 const ioAnimation *pNotifier )
{
	if( ioAniEventHandler::EventHandling( pEvent, pNotifier ) )
		return true;

	if( pEvent->szEventType == "auto_shake" )
	{
		OnAutoShakeCamera( pEvent, pNotifier );
		return true;
	}
	else if( pEvent->szEventType == "face_change" )
	{
		OnFaceChange( pEvent, pNotifier );
		return true;
	}
	else if( pEvent->szEventType == "motion_sound" )
	{
		OnMotionSound( pEvent, pNotifier );
		return true;
	}
	else if( pEvent->szEventType == "voice_sound" )
	{
		OnVoiceSound( pEvent, pNotifier );
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
	else if( pEvent->szEventType == "dummy_attack" )
	{
		OnDummyCharAttack( pEvent, pNotifier );
		return true;
	}
	else if( pEvent->szEventType == "lobelia_action" )
	{
		OnLobeliaAction( pEvent, pNotifier );
		return true;
	}
	else if( pEvent->szEventType == "cmd_action" )
	{
		OnCmdAction( pEvent, pNotifier );
		return true;
	}
	else if( pEvent->szEventType == "cmd_end" )
	{
		OnCmdEnd( pEvent, pNotifier );
		return true;
	}
	else if( pEvent->szEventType == "skill_action" )
	{
		OnSkillAction( pEvent, pNotifier );
		return true;
	}
	else if( pEvent->szEventType == "use_gp_action" )
	{
		OnUseGPAction( pEvent, pNotifier );
		return true;
	}
	else if( pEvent->szEventType == "trinity_show" )
	{
		OnShowTrinityMesh( pEvent, pNotifier );
		return true;
	}
	else if( pEvent->szEventType == "trinity_hide")
	{
		OnHideTrinityMesh( pEvent, pNotifier );
		return true;
	}

	return false;
}

void ioClientAniEventHandler::OnAutoShakeCamera( const AniEventTime *pEvent, const ioAnimation *pNotifier )
{
	if( m_pChar->IsOwnerChar() )
	{
		int iShake = ioStringConverter::ParseInt( pEvent->szSubInfo.c_str() );
		if( iShake > 0 )
		{
			g_ShakeCameraMgr.SetShakeCamera( iShake );
		}
	}
}

void ioClientAniEventHandler::OnFaceChange( const AniEventTime *pEvent, const ioAnimation *pNotifier )
{
	const StringVector &vParam = ioStringConverter::Split( pEvent->szSubInfo.c_str(), " " );
	if( vParam.size() < 2 )
	{
		LOG.PrintTimeAndLog( 0, "ioClientAniEventHandler::OnFaceChange - %s Param size Error( Expected least 2 )",
								pEvent->szSubInfo.c_str() );
		return;
	}

	int iDuration = ioStringConverter::ParseInt( vParam[1].c_str() ) * pNotifier->GetTimeFactor();

	if( ToNpcChar(m_pChar) && ToNpcChar(m_pChar)->IsUseHeadMeshChange() )
	{
		StringVector vHeadParam = ioStringConverter::Split( std::string(vParam[0].c_str()), ".", 1 );
		if( vHeadParam.size() == 2 )
		{
			if( strcmp( vHeadParam[0].c_str(), "" ) != 0 )
			{
				m_pChar->HeadMeshChange( vHeadParam[0].c_str(), iDuration );
			}
		}
		return;
	}

	m_pChar->FaceChange( vParam[0].c_str(), iDuration );
}

void ioClientAniEventHandler::OnVoiceSound( const AniEventTime *pEvent, const ioAnimation *pNotifier )
{
	ioHashString szNewSound = m_pChar->GetVoiceSoundName( pEvent->szSubInfo );

	g_SoundMgr.PlaySound( szNewSound, m_pOwner->GetParentSceneNode() );
}

void ioClientAniEventHandler::OnMotionSound( const AniEventTime *pEvent, const ioAnimation *pNotifier )
{
	std::string szNewSound = m_pChar->GetMotionSoundName( pEvent->szSubInfo );
	
	g_SoundMgr.PlaySound( ioHashString(szNewSound.c_str()), m_pOwner->GetParentSceneNode() );
}

void ioClientAniEventHandler::OnAutoEffect( const AniEventTime *pEvent, const ioAnimation *pNotifier )
{
	m_pChar->AttachEffect( pEvent->szSubInfo );
}

void ioClientAniEventHandler::OnShopEffect( const AniEventTime *pEvent, const ioAnimation *pNotifier )
{
	AniEventInfoList::iterator iter = m_ShopEffectList.begin();
	while( iter != m_ShopEffectList.end() )
	{
		if( (*iter).m_EventName == pEvent->szSubInfo )
		{
			m_pOwner->EndEffect( pEvent->szSubInfo, false );
			m_ShopEffectList.erase( iter );
			break;
		}
		else
		{
			++iter;
		}
	}

	m_pChar->AttachEffect( pEvent->szSubInfo );

	AniEventInfo kInfo;
	kInfo.m_AniName = pNotifier->GetName();
	kInfo.m_EventName = pEvent->szSubInfo;

	m_ShopEffectList.push_back( kInfo );
}

void ioClientAniEventHandler::OnShopSound( const AniEventTime *pEvent, const ioAnimation *pNotifier )
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

void ioClientAniEventHandler::OnDummyCharAttack( const AniEventTime *pEvent, const ioAnimation *pNotifier )
{
	int iCode = ioStringConverter::ParseInt( pEvent->szSubInfo.c_str() );
	if( iCode > 0 )
	{
		g_DummyCharMgr.SetReserveAttackCode( m_pChar, iCode );
	}
}

void ioClientAniEventHandler::OnLobeliaAction( const AniEventTime *pEvent, const ioAnimation *pNotifier )
{
	if( m_pChar )
	{
		g_WeaponMgr.CheckLobeliaWeaponAction( m_pChar->GetCharName() );
	}
}

void ioClientAniEventHandler::OnCmdAction( const AniEventTime *pEvent, const ioAnimation *pNotifier )
{
	if( m_pChar && m_pChar->IsNeedProcess() )
	{
		int iType = ioStringConverter::ParseInt( pEvent->szSubInfo.c_str() );
		m_pChar->SetCmdInputStart( iType );
	}
}


void ioClientAniEventHandler::OnShowTrinityMesh( const AniEventTime *pEvent, const ioAnimation *pNotifier )
{
	if( m_pChar )
	{
		m_bItemMeshHide = false;
		m_pChar->SetItemMeshShow();
	}
}


void ioClientAniEventHandler::OnHideTrinityMesh( const AniEventTime *pEvent, const ioAnimation *pNotifier )
{
	if( m_pChar )
	{
		m_bItemMeshHide = true;
		m_pChar->SetItemMeshHide();
	}
}

void ioClientAniEventHandler::OnCmdEnd( const AniEventTime *pEvent, const ioAnimation *pNotifier )
{
	if( m_pChar && m_pChar->IsNeedProcess() )
	{
		m_pChar->SetCmdInputEnd();
	}
}

void ioClientAniEventHandler::OnSkillAction( const AniEventTime *pEvent, const ioAnimation *pNotifier )
{
	if( m_pChar && m_pChar->IsNeedProcess() )
	{
		m_pChar->SetSkillInputStart();
	}
}

void ioClientAniEventHandler::OnUseGPAction( const AniEventTime *pEvent, const ioAnimation *pNotifier )
{
	float fAmt = ioStringConverter::ParseFloat( pEvent->szSubInfo.c_str() );
	if( fAmt > 0.0f )
	{
		if( m_pChar && m_pChar->IsNeedProcess() )
		{
			m_pChar->CheckUseExtraGauge( fAmt );
		}
	}
}

void ioClientAniEventHandler::ClearAniEvent()
{
	m_HideMeshList.clear();
	m_HideMeshLinkEffectList.clear();
}

void ioClientAniEventHandler::ClearAniEffect()
{
	if( !m_ShopEffectList.empty() )
	{
		AniEventInfoList::iterator iter = m_ShopEffectList.begin();
		for( ; iter != m_ShopEffectList.end(); iter++ )
		{
			AniEventInfo kInfo = *iter;
			m_pOwner->EndEffect( kInfo.m_EventName, false );
		}
		m_ShopEffectList.clear();
	}
}

void ioClientAniEventHandler::OnAnimationClear( const AniEventTime *pEvent, const ioAnimation *pNotifier )
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
				m_pChar->AttachEffect( kInfo.m_EventName );
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
				g_SoundMgr.StopSound( kInfo.m_EventName, 0 );
				iter = m_ShopSoundList.erase( iter );
			}
			else
			{
				++iter;
			}
		}
	}

	//트리니티
	if( m_bItemMeshHide )
	{
		m_bItemMeshHide = false;
		m_pChar->SetItemMeshShow();
	}
}

void ioClientAniEventHandler::OnTrailStart( const AniEventTime *pEvent, const ioAnimation *pNotifier )
{
	const StringVector &vParam = ioStringConverter::Split( pEvent->szSubInfo.c_str(), " " );

	ioHashString szTexture;
	if( vParam.size() >= 1 )
		szTexture = vParam[0].c_str();

	FXRenderType eRenderType = FRT_SRCALPHA;
	if( vParam.size() >= 2 )
		eRenderType = ioStringConverter::ParseFXRenderType( vParam[1].c_str() );

	int iTickSegment = 20;
	if( vParam.size() >= 3 )
		iTickSegment = ioStringConverter::ParseInt( vParam[2].c_str() );

	float fMaxTick = FLOAT100;
	if( vParam.size() >= 4 )
		fMaxTick = ioStringConverter::ParseFloat( vParam[3].c_str() );

	ioHashString szParts;
	if( vParam.size() >= 5 )
		szParts = vParam[4].c_str();

	float fSampleTick = 200.0f;
	if( vParam.size() >= 6 )
		fSampleTick = ioStringConverter::ParseFloat( vParam[5].c_str() );

	bool bDepthCheck = true;
	if( vParam.size() >= 7 )
		bDepthCheck = ioStringConverter::ParseBool( vParam[6].c_str() );

	int iCtrlStart = 0;
	int iCtrlEnd = INT_MAX;
	if( vParam.size() >= 8 )
	{
		// vParam은 ::Split 내부의 static변수의 참조인데, Split내부에서 시작시 지워버리기때문에 삭제되버려서 임시로 복사
		std::string szCtrlParam = vParam[7];

		const StringVector &vCtrl = ioStringConverter::Split( szCtrlParam, "-", 1 );
		if( vCtrl.size() == 2 )
		{
			iCtrlStart = ioStringConverter::ParseInt( vCtrl[0].c_str() );
			iCtrlEnd   = ioStringConverter::ParseInt( vCtrl[1].c_str() ) + 1;
		}
	}
	
	float fGlowness = 2.0f;
	if( vParam.size() >= 9 )
		fGlowness = ioStringConverter::ParseFloat( vParam[8].c_str() );

	float fIntensity = 5.0f;
	if( vParam.size() >= 10 )
		fIntensity = ioStringConverter::ParseFloat( vParam[9].c_str() );

	ioEntity *pEntity = NULL;
	if( szParts == "weapon" )
	{
		pEntity = GetWeaponEntity();
	}
	else if( szParts == "hand" )
	{
		pEntity = GetHandEntity();
	}
	else if( strcmp( szParts.c_str(), "leg") == 0 )
	{
		pEntity = GetLegEntity();
	}

	if( pEntity && pEntity->GetControlPointCnt() > 0 )
	{
		ConstCtrlPointList rkStart = pEntity->GetControlPointList( "trailstart" );
		ConstCtrlPointList rkEnd = pEntity->GetControlPointList( "trailend" );

		if( !rkStart.empty() && rkStart.size() == rkEnd.size() )
		{
			ConstCtrlPointList::const_iterator iStart, iEnd;
			iStart = rkStart.begin();
			iEnd = rkEnd.begin();

			for( int iIdx=0 ; iStart != rkStart.end() ; ++iStart, ++iEnd, iIdx++ )
			{
				if( !COMPARE( iIdx, iCtrlStart, iCtrlEnd ) )
					continue;

				ioMeshTrailer *pTrailer = m_pOwner->CreateTrailer();
				if( !pTrailer ) continue;

				pTrailer->SetTexture( szTexture );
				pTrailer->SetRenderType( eRenderType );
				pTrailer->SetIntensity( fIntensity );
				pTrailer->SetGlowness( fGlowness );
				pTrailer->BuildMaterial();

				pTrailer->SetDepthCheck( bDepthCheck );

				pTrailer->SetMaxTickTime( fMaxTick );
				pTrailer->SetMaxSegment( iTickSegment );
				pTrailer->SetStartEndPoint( *iStart, *iEnd );
				
				bool bDataOK = pTrailer->GenerateTrailData( m_pOwner->GetSkeleton()->GetName(),
															pEntity->GetMeshName(),
															pNotifier,
															iIdx,
															FLOAT1000 / fSampleTick,
															pEvent->fEventTime,
															400.0f );

				if( !bDataOK )
				{
					m_pOwner->DestroyTrailer( pTrailer );
					pTrailer = NULL;	// 주의!!
				}
			}
		}
	}

	pNotifier->SetEndEventNotify();
}

ioEntity* ioClientAniEventHandler::GetWeaponEntity()
{
	if( !m_pChar )	return NULL;

	ioWeaponItem *pWeapon = m_pChar->GetWeapon();
	ioObjectItem *pObject = m_pChar->GetObject();

	ioHashString szNewMeshName;

	if( pObject )
	{
		szNewMeshName = Help::ConvertName( pObject->GetEquipMesh(m_pChar), m_pChar->IsMale() );
		if( !m_pChar->IsMale() )
		{
			char szFullName[MAX_PATH]="";
			StringCbPrintf( szFullName, sizeof( szFullName ), "resource\\mesh\\%s.msh" , szNewMeshName.c_str() );
			if( !g_ResourceLoader.IsFileExist( szFullName ) )
			{
				szNewMeshName = Help::ConvertName( pObject->GetEquipMesh(m_pChar), true );
			}
		}
	}
	else if( pWeapon )
	{
		szNewMeshName = Help::ConvertName( pWeapon->GetEquipMesh(m_pChar), m_pChar->IsMale() );
		if( !m_pChar->IsMale() )
		{
			char szFullName[MAX_PATH]="";
			StringCbPrintf( szFullName, sizeof( szFullName ), "resource\\mesh\\%s.msh" , szNewMeshName.c_str() );
			if( !g_ResourceLoader.IsFileExist( szFullName ) )
			{
				szNewMeshName = Help::ConvertName( pWeapon->GetEquipMesh(m_pChar), true );
			}
		}
	}
	else
	{
		return NULL;
	}

	return m_pOwner->GetEntity( szNewMeshName );
}

ioEntity* ioClientAniEventHandler::GetHandEntity()
{
	if( !m_pChar )	return NULL;

	ioHashString szMeshName;

	ioArmorItem *pArmor = m_pChar->GetArmor();
	if( pArmor )
	{
		if( pArmor->HasHandMesh(m_pChar) )
			szMeshName = pArmor->GetHandMesh(m_pChar);
		else
			szMeshName = pArmor->GetBodyMesh(m_pChar);
	}
	else
	{
		szMeshName = m_pChar->GetEquipSlot()->GetDefaultHandMesh();
	}

	if( szMeshName.IsEmpty() )
		return NULL;

	return m_pOwner->GetEntity( szMeshName );
}

ioEntity* ioClientAniEventHandler::GetLegEntity()
{
	if( !m_pChar )	return NULL;

	ioHashString szMeshName;

	ioArmorItem *pArmor = m_pChar->GetArmor();
	if( pArmor )
	{
		szMeshName = pArmor->GetBodyMesh(m_pChar);
	}
	else
	{
		szMeshName = m_pChar->GetEquipSlot()->GetDefaultUnderwearMesh();
	}

	if( szMeshName.IsEmpty() )
		return NULL;

	return m_pOwner->GetEntity( szMeshName );
}

bool ioClientAniEventHandler::IsCanShopSound()
{
	return g_GUIMgr.IsCanShopSoundForField();
}

void ioClientAniEventHandler::OnShowWeaponMesh( const AniEventTime *pEvent, const ioAnimation *pNotifier )
{
	const ioWeaponItem *pWeaponItem = m_pChar->GetWeapon();
	if( !pWeaponItem ) return;

	ShowMesh( pWeaponItem->GetEquipMesh(m_pChar), pWeaponItem->GetItemEquipEffect(m_pChar->IsMale()), pNotifier );

	m_pChar->SetItemMeshShow();
	m_bItemMeshHide = false;
}

void ioClientAniEventHandler::OnShowArmorMesh( const AniEventTime *pEvent, const ioAnimation *pNotifier )
{
	const ioArmorItem *pArmorItem = m_pChar->GetArmor();
	if( !pArmorItem ) return;

	if( strcmp( pEvent->szSubInfo.c_str(), "body" ) == 0 )
		ShowMesh( pArmorItem->GetBodyMesh(m_pChar), pArmorItem->GetItemEquipEffect(m_pChar->IsMale()), pNotifier );
	else if( strcmp( pEvent->szSubInfo.c_str(), "hand" ) == 0 )
		ShowMesh( pArmorItem->GetHandMesh(m_pChar), pArmorItem->GetItemEquipEffect(m_pChar->IsMale()), pNotifier );
	else
	{
		ShowMesh( pArmorItem->GetBodyMesh(m_pChar), pArmorItem->GetItemEquipEffect(m_pChar->IsMale()), pNotifier );
		ShowMesh( pArmorItem->GetHandMesh(m_pChar), pArmorItem->GetItemEquipEffect(m_pChar->IsMale()), pNotifier );
	}
}

void ioClientAniEventHandler::OnShowHelmMesh( const AniEventTime *pEvent, const ioAnimation *pNotifier )
{
	const ioHelmetItem *pHelmetItem = m_pChar->GetHelmet();
	if( !pHelmetItem ) return;

	ShowMesh( pHelmetItem->GetEquipMesh(m_pChar), pHelmetItem->GetItemEquipEffect(m_pChar->IsMale()), pNotifier );
}

void ioClientAniEventHandler::OnShowCloakMesh( const AniEventTime *pEvent, const ioAnimation *pNotifier )
{
	const ioCloakItem *pCloakItem = m_pChar->GetCloak();
	if( !pCloakItem ) return;

	ShowMesh( pCloakItem->GetEquipMesh(m_pChar), pCloakItem->GetItemEquipEffect(m_pChar->IsMale()), pNotifier );
}

void ioClientAniEventHandler::ShowMesh( const ioHashString &rkMeshName, const ioHashString &rkEffectName, const ioAnimation *pNotifier )
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

void ioClientAniEventHandler::ShowMeshLinkEffect( const ioHashString &rkEffectName, const ioAnimation *pNotifier )
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

void ioClientAniEventHandler::OnHideWeaponMesh( const AniEventTime *pEvent, const ioAnimation *pNotifier )
{
	const StringVector &vParam = ioStringConverter::Split( pEvent->szSubInfo.c_str(), " " );
	int iReturnSize = vParam.size();
	for(int i = 0;i < iReturnSize;i++)
	{
		if( m_pChar->GetCharacterInfo().m_class_type == atoi( vParam[i].c_str() ) )
			return;
	}

	const ioWeaponItem *pWeaponItem = m_pChar->GetWeapon();
	if( !pWeaponItem ) return;

	HideMesh( pWeaponItem->GetEquipMesh(m_pChar), pWeaponItem->GetItemEquipEffect(m_pChar->IsMale()), pNotifier );

	m_bItemMeshHide = true;
	m_pChar->SetItemMeshHide();
}


void ioClientAniEventHandler::OnHideHairMesh( const AniEventTime *pEvent, const ioAnimation *pNotifier )
{
	const StringVector &vParam = ioStringConverter::Split( pEvent->szSubInfo.c_str(), " " );
	int iReturnSize = vParam.size();
	for(int i = 0;i < iReturnSize;i++)
	{
		if( m_pChar->GetCharacterInfo().m_class_type == atoi( vParam[i].c_str() ) )
			return;
	}

	if( !m_pChar || !m_pChar->GetGroup() || !m_pChar->GetEquipSlot() )
		return;

	ioEntity *pHair =m_pChar->GetGroup()->GetEntity( m_pChar->GetEquipSlot()->GetDefaultHairMesh() );
	if( !pHair )
	{
		pHair = m_pChar->GetGroup()->GetEntity( m_pChar->GetEquipSlot()->GetDefaultBindHairMesh() );

		if( !pHair )
			pHair = m_pChar->GetGroup()->GetEntity( m_pChar->GetEquipSlot()->GetDefaultHalfHairMesh() );
	}

	if( !pHair )
		return;//이러면???

	HideMesh( pHair->GetMeshName(), "", pNotifier );
}


void ioClientAniEventHandler::OnShowHairMesh( const AniEventTime *pEvent, const ioAnimation *pNotifier )
{
	if( !m_pChar || !m_pChar->GetGroup() || !m_pChar->GetEquipSlot() )
		return;

	ioEntity *pHair =m_pChar->GetGroup()->GetEntity( m_pChar->GetEquipSlot()->GetDefaultHairMesh() );
	if( !pHair )
	{
		pHair = m_pChar->GetGroup()->GetEntity( m_pChar->GetEquipSlot()->GetDefaultBindHairMesh() );

		if( !pHair )
			pHair = m_pChar->GetGroup()->GetEntity( m_pChar->GetEquipSlot()->GetDefaultHalfHairMesh() );
	}

	if( !pHair )
		return;//이러면???

	ShowMesh( pHair->GetMeshName(), "", pNotifier );
}


void ioClientAniEventHandler::OnHideArmorMesh( const AniEventTime *pEvent, const ioAnimation *pNotifier )
{
	char szPart[MAX_PATH] = "";
	const StringVector &vParam = ioStringConverter::Split( pEvent->szSubInfo.c_str(), " " );
	int iReturnSize = vParam.size();
	for(int i = 0;i < iReturnSize;i++)
	{
		if( i == 0 )
		{
			sprintf_e( szPart, "%s", vParam[i].c_str() );
		}
		else if( m_pChar->GetCharacterInfo().m_class_type == atoi( vParam[i].c_str() ) )
		{
			return;
		}
	}

	const ioArmorItem *pArmorItem = m_pChar->GetArmor();
	if( !pArmorItem ) return;

	if( strcmp( szPart, "body" ) == 0 )
	{
		HideMesh( pArmorItem->GetBodyMesh(m_pChar), pArmorItem->GetItemEquipEffect(m_pChar->IsMale()), pNotifier );
	}
	else if( strcmp( szPart, "hand" ) == 0 )
	{
		HideMesh( pArmorItem->GetHandMesh(m_pChar), pArmorItem->GetItemEquipEffect(m_pChar->IsMale()), pNotifier );
	}
	else
	{
		HideMesh( pArmorItem->GetBodyMesh(m_pChar), pArmorItem->GetItemEquipEffect(m_pChar->IsMale()), pNotifier );
		HideMesh( pArmorItem->GetHandMesh(m_pChar), pArmorItem->GetItemEquipEffect(m_pChar->IsMale()), pNotifier );
	}
}

void ioClientAniEventHandler::OnHideHelmMesh( const AniEventTime *pEvent, const ioAnimation *pNotifier )
{
	const StringVector &vParam = ioStringConverter::Split( pEvent->szSubInfo.c_str(), " " );
	int iReturnSize = vParam.size();
	for(int i = 0;i < iReturnSize;i++)
	{
		if( m_pChar->GetCharacterInfo().m_class_type == atoi( vParam[i].c_str() ) )
			return;
	}

	const ioHelmetItem *pHelmetItem = m_pChar->GetHelmet();
	if( !pHelmetItem ) return;

	HideMesh( pHelmetItem->GetEquipMesh(m_pChar), pHelmetItem->GetItemEquipEffect(m_pChar->IsMale()), pNotifier );
}

void ioClientAniEventHandler::OnHideCloakMesh( const AniEventTime *pEvent, const ioAnimation *pNotifier )
{
	const StringVector &vParam = ioStringConverter::Split( pEvent->szSubInfo.c_str(), " " );
	int iReturnSize = vParam.size();
	for(int i = 0;i < iReturnSize;i++)
	{
		if( m_pChar->GetCharacterInfo().m_class_type == atoi( vParam[i].c_str() ) )
			return;
	}

	const ioCloakItem *pCloakItem = m_pChar->GetCloak();
	if( !pCloakItem ) return;

	HideMesh( pCloakItem->GetEquipMesh(m_pChar), pCloakItem->GetItemEquipEffect(m_pChar->IsMale()), pNotifier );
}

void ioClientAniEventHandler::HideMesh( const ioHashString &rkMeshName, const ioHashString &rkEffectName, const ioAnimation *pNotifier )
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

void ioClientAniEventHandler::HideMeshLinkEffect( const ioHashString &rkEffectName, const ioAnimation *pNotifier )
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
