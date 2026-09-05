#include <stdafx.h>

#include "ChatMessageFunc.h"
#include "ItemDefine.h"
#include "ioExcavationManager.h"
#include "ioExcavationItem.h"

#include "GUI/ExcavationWnd.h"
#include "GUI/ExcavationResultWnd.h"
#include "GUI/ExcavationShovelWnd.h"
#include "GUI/ExcavationLevelUpWnd.h"
#include "GUI/GlobalWnd.h"

#include "DataHeaders/LSC_decoitem_info.h"
#include "DataHeaders/LSC_Jewel_present.h"

#include "ioDecorationPrice.h"
#include "ioGameAnnMgr.h"

template<> ioExcavationManager *Singleton< ioExcavationManager >::ms_Singleton = 0;

ioExcavationManager::ioExcavationManager(void) : m_pStage(NULL)
{
	m_iExcavationState = ES_DISABLE;
	m_dwStateCheckTime = 0;
	m_ExcavationPoint = ioMath::VEC3_ZERO;
	m_vExcavationUser.clear();
	m_iShovelCount = 5;
	m_dwPacketSendTime = 0;
	m_dwShowResultTime = 0;
	m_bSendRechargeProcess = false;
	m_dwRemainChargeTime = 0;
	m_vUserExcavationLv.clear();

	m_pInfoManager = NULL;
	m_pGradeManager = NULL;
}

ioExcavationManager::~ioExcavationManager(void)
{
	SAFEDELETE( m_pInfoManager );
	SAFEDELETE( m_pGradeManager );

	vRewardIcon::iterator iter = m_vRewardIcon.begin();
	for( ; iter!=m_vRewardIcon.end(); ++iter )
	{
		SAFEDELETE( (*iter).icon );
	}
	m_vRewardIcon.clear();
}

ioExcavationManager &ioExcavationManager::GetSingleton()
{
	return Singleton< ioExcavationManager >::GetSingleton();
}

void ioExcavationManager::Load()
{
	LoadINI();
	LoadData();
}

void ioExcavationManager::LoadINI()
{
	ioINILoader_e kLoader( "config/lsc_excavation.ini" );

	kLoader.SetTitle_e( "common" );
	m_dwKitEtcItemCode = kLoader.LoadInt_e( "kit_etc_item_code", 0 );
	m_dwKitItemCode    = kLoader.LoadInt_e( "kit_item_code", 0 );
	m_dwResultTime     = kLoader.LoadInt_e( "result_time", 3000 );
	m_iMaxShovelCount  = kLoader.LoadInt_e( "max_shovel_count", 5 );
	m_iManualIndex     = kLoader.LoadInt_e( "manual_id", 1 );
	m_iEffectGrade     = kLoader.LoadInt_e( "effect_grade", 1 );

	kLoader.SetTitle_e( "level_bonus" );
	m_iCoolTimeCheckLv  = kLoader.LoadInt_e( "cool_time_check_lv", 1 );
	m_dwCoolTimeBonus   = kLoader.LoadInt_e( "cool_time_lv_bonus", 0 );
	m_dwDefaultCoolTime = kLoader.LoadInt_e( "cool_time_default", 30000 );
	m_dwMinimumCoolTime = kLoader.LoadInt_e( "cool_time_minimum", 30000 );

	m_iSpeedCheckLv = kLoader.LoadInt_e( "cool_time_check_lv", 1 );
	if( m_iSpeedCheckLv < 1 )
	{
		m_iSpeedCheckLv = 1;
	}

	m_fSpeedBonus   = kLoader.LoadFloat_e( "speed_lv_bonus", 0.0f );
	m_fDefaultSpeed = kLoader.LoadFloat_e( "speed_default", 35 );
	m_fMinimumSpeed = kLoader.LoadFloat_e( "speed_minimum", 35 );

	kLoader.SetTitle_e( "error_msg" );
	kLoader.LoadString_e( "cool_time"     ,      "", m_ErrorMsg[ERR_COOL_TIME], MAX_PATH );
	kLoader.LoadString_e( "equip_need_kit",      "", m_ErrorMsg[ERR_NEED_KIT], MAX_PATH );
	kLoader.LoadString_e( "need_shovel",         "", m_ErrorMsg[ERR_NEED_SHOVEL], MAX_PATH );
	kLoader.LoadString_e( "invalid_mode",        "", m_ErrorMsg[ERR_INVAILD_MODE], MAX_PATH );
	kLoader.LoadString_e( "invalid_pos",         "", m_ErrorMsg[ERR_INVAILD_POS], MAX_PATH );
	kLoader.LoadString_e( "no_equip_kit",        "", m_ErrorMsg[ERR_NO_EQUIP_KIT], MAX_PATH );
	kLoader.LoadString_e( "shovel_max_count",    "", m_ErrorMsg[ERR_SHOVEL_MAX_COUNT], MAX_PATH );
	kLoader.LoadString_e( "invalid_charge_time", "", m_ErrorMsg[ERR_INVALID_TIME], MAX_PATH );
	kLoader.LoadString_e( "shovel_charge_fail",  "", m_ErrorMsg[ERR_SHOVEL_CHARGE_FAIL], MAX_PATH );
	kLoader.LoadString_e( "exception_msg",       "", m_ErrorMsg[ERR_EXCEPTION], MAX_PATH );

	kLoader.SetTitle_e( "system_msg" );
	kLoader.LoadString_e( "excavation_success",  "", m_SysMsg[MSG_SUCCESS], MAX_PATH );

	kLoader.SetTitle_e( "guide" );

	char szBuf[MAX_PATH] = "";
	kLoader.LoadString_e( "guide_effect", "", szBuf, MAX_PATH );
	m_szGuideEffectName = szBuf;

	kLoader.LoadString_e( "point_effect", "", szBuf, MAX_PATH );
	m_szPointEffectName = szBuf;

	m_fLimitAngle = kLoader.LoadFloat_e( "angle_limit", 0.0f );
	m_fHeightOffset = kLoader.LoadFloat_e( "height_offset", 0.0f );
	m_fDistanceHeightOffset = kLoader.LoadFloat_e( "distance_height_offset", 0.0f );
	m_fSight = kLoader.LoadFloat_e( "excavation_sight", 300.0f );

	kLoader.SetTitle_e( "emoticon" );
	kLoader.LoadString_e( "emoticon_excavating", "", szBuf, MAX_PATH );
	m_szEmoticonExcavating = szBuf;

	kLoader.LoadString_e( "emoticon_digging", "", szBuf, MAX_PATH );
	m_szEmoticonDigging = szBuf;

	kLoader.LoadString_e( "emoticon_fail", "", szBuf, MAX_PATH );
	m_szEmoticonFail = szBuf;

	char szKey[MAX_PATH] = "";
	kLoader.SetTitle_e( "icon_info" );
	int iCnt = kLoader.LoadInt_e( "info_cnt", 0 );
	for( int i=0; i<iCnt; ++i )
	{
		IconInfo kInfo;

		wsprintf( szKey, "info%d_min_lv", i+1 );
		kInfo.min_lv = kLoader.LoadInt( szKey, 0 );

		wsprintf( szKey, "info%d_max_lv", i+1 );
		kInfo.max_lv = kLoader.LoadInt( szKey, 0 );

		wsprintf( szKey, "info%d_use_icon_name", i+1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.use_icon_name = szBuf;

		m_vIconInfo.push_back( kInfo );
	}
}

void ioExcavationManager::LoadData()
{
	m_pInfoManager = new LSC_Excavation_info_Manager;
	m_pInfoManager->LoadData(EXCAVATION_INFO_TABLE);

	m_pGradeManager = new LSC_Excavation_grade_Manager;
	m_pGradeManager->LoadData(EXCAVATION_GRADE_TABLE);

	m_vRewardIcon.clear();
	for( int i=0; i<m_pInfoManager->GetTotal(); ++i )
	{
		LSC_Excavation_info *pInfo = m_pInfoManager->GetAt( i );
		if( pInfo )
		{
			char szBuf[MAX_PATH] = "";
			if( pInfo->ItemType == RESULT_ARTIFACT )
			{
				strcpy( szBuf, pInfo->ItemIcon );
			}
			else if( pInfo->ItemType == RESULT_ITEM )
			{
				// 추후 ETC 타입 이외의 아이템이 보상으로 추가될 경우 여기에 추가한다.
				if( pInfo->RewardType == PRESENT_ETC_ITEM )
				{
					ioEtcItem *pEtcItem;
					pEtcItem = g_EtcItemMgr.FindEtcItem( (DWORD)pInfo->ItemIndex );
					if( pEtcItem )
					{
						ioHashString szIconName = pEtcItem->GetIconName();
						strcpy( szBuf, szIconName.c_str() ) ;
					}
				}
			}

			RewardIcon kRewardIcon;
			kRewardIcon.index = pInfo->ItemIndex;
			kRewardIcon.icon = g_UIImageSetMgr.CreateImageByFullName( szBuf );
			m_vRewardIcon.push_back( kRewardIcon );
		}
	}
}

void ioExcavationManager::SetDisableState()
{
	m_iExcavationState = ES_DISABLE;
	m_dwStateCheckTime = 0;
}

void ioExcavationManager::SetChargingState()
{
	if( GetExcavationState() == ES_CHARGING )
	{
		return;
	}

	if( m_iShovelCount <= 0 )
	{
		m_iExcavationState = ES_DISABLE;
		return;
	}

	m_iExcavationState = ES_CHARGING;
	m_dwStateCheckTime = REALGETTIME();
}

void ioExcavationManager::SetStandByState()
{
	if( GetExcavationState() == ES_STANDBY )
		return;

	m_iExcavationState = ES_STANDBY;
	m_dwStateCheckTime = 0;
}

void ioExcavationManager::SetDiggingState()
{
	if( GetExcavationState() == ES_DIGGING )
		return;

	m_iExcavationState = ES_DIGGING;
}

void ioExcavationManager::SetDiggingMotion( const ioHashString &szName )
{
	if( !m_pStage )
		return;

	ioBaseChar *pChar = m_pStage->GetBaseChar( szName );
	if( pChar && pChar->GetState() != CS_EXCAVATING )
	{
		ioEquipSlot* pEquipSlot = pChar->GetEquipSlot();
		if( pEquipSlot )
		{
			ioExcavationItem *pExcavationItem = ToExcavationItem( pEquipSlot->GetObject() );
			if( pExcavationItem )
			{
				pExcavationItem->SetExcavatingStartAni( pChar );

				try
				{
					if( !pChar->IsOwnerChar() )
						pChar->SetLongTimeLiveEmoticonAndScale( m_szEmoticonDigging, false );
				}
				catch(...)
				{

				}
			}
		}
	}
}

void ioExcavationManager::CheckResultMotion()
{
	if( !m_pStage )
		return;

	int iSize = m_vExcavationUser.size();
	for( int i=0; i<iSize; ++i )
	{
		if( m_vExcavationUser[i].result_show_time < FRAMEGETTIME() )
		{
			ioBaseChar *pChar = m_pStage->GetBaseChar( m_vExcavationUser[i].name );
			if( pChar )
			{
				ioEquipSlot* pEquipSlot = pChar->GetEquipSlot();
				if( !pEquipSlot )
					continue;

				ioExcavationItem *pExcavationItem = ToExcavationItem( pEquipSlot->GetObject() );
				if( !pExcavationItem )
					continue;

				if( m_vExcavationUser[i].is_success )
				{
					if( pChar->IsOwnerChar() )
					{
						if( !g_GUIMgr.IsShow(SOLDIER_LEVEL_UP_WND) && EnableShowResult() )
						{
							g_GUIMgr.ShowWnd( EXCAVATION_RESULT_WND );
						}
					}

					pExcavationItem->SetResultSuccessAni( pChar );

					if( !pChar->IsOwnerChar() )
					{
						DWORD dwDuration = pExcavationItem->GetAnimationEndTime() - FRAMEGETTIME();
						ioHashString szIconName;
						GetResultIconName( m_iReserveItemType, m_iReserveItemIndex, szIconName );
						pChar->EndLongTimeLiveEmoticon();
						pChar->SetFishingLevelUpEmoticon( szIconName, "", dwDuration );
						m_iReserveItemType = 0;
						m_iReserveItemIndex = 0;
					}
				}
				else
				{
					pExcavationItem->SetResultFailAni( pChar );

					if( !pChar->IsOwnerChar() )
					{
						DWORD dwDuration = pExcavationItem->GetAnimationEndTime() - FRAMEGETTIME();

						pChar->EndLongTimeLiveEmoticon();
						pChar->SetFishingLevelUpEmoticon( m_szEmoticonFail, "", dwDuration );
					}

					ExcavationWnd* pExcavationWnd = dynamic_cast<ExcavationWnd*>( g_GUIMgr.FindWnd( EXCAVATION_WND ) );
					if( pExcavationWnd )
					{
						if( pExcavationWnd->IsProcessDelay() )
						{
							pExcavationWnd->SetProcessDelay( false );
						}

						if( pExcavationWnd->IsLevelUpDelay() )
						{
							pExcavationWnd->SetLevelUpDelay( false );

							g_GUIMgr.ShowWnd( EXCAVATION_LEVELUP_WND );
						}
					}
				}
			}

			m_vExcavationUser[i].is_end = true;
		}
	}

	vExcavationUserInfo::iterator iter = m_vExcavationUser.begin();
	while( iter != m_vExcavationUser.end() )
	{
		if( (*iter).is_end )
			iter = m_vExcavationUser.erase(iter);
		else
			++iter;
	}
}

void ioExcavationManager::ProcessExcavtion()
{
	if( m_dwShowResultTime>0 && m_dwShowResultTime < FRAMEGETTIME() )
	{
		CheckResultMotion();
	}

	if( !m_pStage )
		return;

	ioBaseChar *pChar = m_pStage->GetOwnerChar();
	if( !pChar )
		return;

	if( pChar->IsOwnerChar() )
	{
		if( !pChar->IsExcavating() )
		{
			SetDisableState();
			return;
		}

		if( m_iExcavationState == ES_CHARGING && m_dwStateCheckTime > 0 )
		{
			DWORD dwGapTime = REALGETTIME() - m_dwStateCheckTime;
			if( dwGapTime >= GetCooltime() )
			{
				SetStandByState();
			}
		}
	}
}

void ioExcavationManager::ProcessRecharge()
{
	if( m_iShovelCount >= m_iMaxShovelCount )
		return;

	if( m_bSendRechargeProcess )
		return;

	if( GetRemainChargeTime() <= 0 )
	{
		SendTimeRechargeShovel();
	}
}

void ioExcavationManager::SetExcavation( const ioHashString &szName, bool bUse )
{
	if( !m_pStage )
		return;

	ioBaseChar *pChar = m_pStage->GetBaseChar( szName );
	if( !pChar )
		return;

	if( bUse )
	{
		if( pChar->IsWereWolfState() )
		{
			pChar->ReleaseWereWolfState();
		}

		if( pChar->IsGPState() )
		{
			pChar->ReleaseGPState();
		}

		ioExcavationItem *pItem = ToExcavationItem( m_pStage->CreateItem( m_dwKitItemCode ) );
		if( pItem )
		{
			pItem->ReLoadProperty();
			pItem->SetOwnerName( pChar->GetCharName() );
			pItem->SetItemCreateIndex( pChar->GetIndex() * 100 );
			pItem->SetItemReinforce( 0 );
			pItem->SetItemCustom( 0, 0 );
			pItem->CheckMeshAndAni( pChar );
			ioItem *pPreItem = pChar->EquipItem( pItem );
			SAFEDELETE( pPreItem );

			if( pChar->IsOwnerChar() )
			{
				SetChargingState();

				g_GUIMgr.ShowWnd( EXCAVATION_WND );

				pChar->CreateExcavationGuide();
				pChar->SetExcavationPos( m_ExcavationPoint );

				UpdateShovelCount();
			}
			else
			{
				// 이모티콘
				pChar->SetLongTimeLiveEmoticonAndScale( m_szEmoticonExcavating, false );
			}
		}
		else
		{
			pChar->SetState( CS_DELAY ); 
		}
	}
	else
	{
		ioEquipSlot* pEquipSlot = pChar->GetEquipSlot();
		if( pEquipSlot )
		{
			ioExcavationItem *pExcavationItem = ToExcavationItem( pEquipSlot->GetObject() );
			if( pExcavationItem )
			{
				pExcavationItem->SetReleaseAni( pChar );
			}
		}

		if( pChar->IsOwnerChar() )
		{
			SetDisableState();

			pChar->DestroyExcavationGuide();
			pChar->DestroyExcavationPoint();

			g_GUIMgr.HideWnd( EXCAVATION_WND );
		}
		else
		{
			pChar->EndLongTimeLiveEmoticon();
		}
	}
}

void ioExcavationManager::CheckInputKey( DWORD dwActionKey, bool bExcavating )
{
	if( !bExcavating )
		return;

	if( dwActionKey != ioUserKeyInput::AKI_ATTACK )
		return;

	if( GetExcavationState() == ES_DIGGING )
		return;

	if( !m_pStage )
		return;

	ioBaseChar *pChar = m_pStage->GetBaseChar( g_MyInfo.GetPublicID() );
	if( !pChar )
	{
		return;
	}

	if( pChar->GetState() != CS_DELAY && pChar->GetState() != CS_RUN )
	{
		return;
	}

	if( GetShovelCount() <= 0 )
	{
		g_ChatMgr.SetSystemMsg( m_ErrorMsg[ERR_NEED_SHOVEL] );
		return;
	}
	else if( GetExcavationState() != ES_STANDBY )
	{
		g_ChatMgr.SetSystemMsg( m_ErrorMsg[ERR_COOL_TIME] );
		return;
	}

	if( dwActionKey == ioUserKeyInput::AKI_ATTACK && g_ExcavationMgr.IsSendExcavationPacket() )
	{
		g_GUIMgr.HideWnd( EXCAVATION_SHOVEL_WND );

		pChar->SetState( CS_ETC_STATE );
		SendStartDigging();
	}
}

float ioExcavationManager::GetChargingRate()
{
	return Help::GetAniRateByTime( REALGETTIME() - m_dwStateCheckTime, GetCooltime(), Help::RATE_NORMAL );
}

DWORD ioExcavationManager::GetCooltime()
{
	DWORD dwCoolTimeBonus = ( g_MyInfo.GetExcavationLevel() / m_iCoolTimeCheckLv ) * m_dwCoolTimeBonus;
	DWORD dwCoolTime = 0;
	if( m_dwDefaultCoolTime < dwCoolTimeBonus )
		dwCoolTime = 0;
	else
		dwCoolTime = m_dwDefaultCoolTime - dwCoolTimeBonus;

	dwCoolTime = max( m_dwMinimumCoolTime, dwCoolTime );

	return dwCoolTime;
}

float ioExcavationManager::GetSpeedClass( const ioHashString &szName )
{
	if( g_MyInfo.GetPublicID() == szName )
	{
		float fSpeed = m_fDefaultSpeed - (g_MyInfo.GetExcavationLevel() / m_iSpeedCheckLv) * m_fSpeedBonus;
		return max( m_fMinimumSpeed, fSpeed );
	}

	UserExcavationLvInfo *pInfo = FindUser( szName );
	if( pInfo )
	{
		float fSpeed = m_fDefaultSpeed - ( pInfo->excavation_lv / m_iSpeedCheckLv ) * m_fSpeedBonus;
		return max( m_fMinimumSpeed, fSpeed );
	}

	return m_fDefaultSpeed;
}

void ioExcavationManager::GetResultIconName( int iType, int iItemIndex, ioHashString &szIconName )
{
	if( iType == 0 && iItemIndex == 0 )
		return;

	LSC_Excavation_info *pInfo  = g_ExcavationMgr.GetArtifactInfo( iItemIndex );
	if( !pInfo )
		return;

	if( iType == RESULT_ARTIFACT )
	{
		szIconName = pInfo->ItemIcon;
	}
	else
	{
		// 추후 ETC 타입 이외의 아이템이 보상으로 추가될 경우 여기에 추가한다.
		if( pInfo->RewardType == PRESENT_ETC_ITEM )
		{
			/*ioEtcItem *pEtcItem;
			pEtcItem = g_EtcItemMgr.FindEtcItem( (DWORD)pInfo->ItemIndex );
			szIconName = pEtcItem->GetIconName();*/
			szIconName = pInfo->ItemIcon;
		}
	}
}

void ioExcavationManager::GetResultItemName( LSC_Excavation_info *pInfo, ioHashString &szItemName )
{
	if( !pInfo )
		return;

	if( pInfo->ItemType == RESULT_ARTIFACT )
	{
		szItemName = GET_STRING(pInfo->ItemName);
	}
	else
	{
		// 추후 ETC 타입 이외의 아이템이 보상으로 추가될 경우 여기에 추가한다.
		if( pInfo->RewardType == PRESENT_ETC_ITEM )
		{
			ioEtcItem *pEtcItem;
			pEtcItem = g_EtcItemMgr.FindEtcItem( (DWORD)pInfo->ItemIndex );
			szItemName = pEtcItem->GetName();
		}
	}
}

void ioExcavationManager::GetResultItemValue( LSC_Excavation_info *pInfo, ioHashString &szItemName, int iCount )
{
	if( !pInfo )
		return;

	if( pInfo->ItemType == RESULT_ARTIFACT )
		return;

	if( pInfo->RewardType != PRESENT_ETC_ITEM )
		return;

	ioEtcItem* pEtcItem = g_EtcItemMgr.FindEtcItem( pInfo->ItemIndex );

	if( !pEtcItem )
		return;

	if( pInfo->ItemIndex / ioEtcItem::USE_TYPE_CUT_VALUE == ioEtcItem::UT_TIME )
	{
		char szTime[MAX_PATH] = {0,};
		enum { DAY_SEC = 86400, HOUR_SEC = 3600, MINUTE_SEC = 60, };
		int iDay    = ( pEtcItem->GetValue( 0 ) * iCount ) / DAY_SEC;
		int iHourSec= ( pEtcItem->GetValue( 0 ) * iCount ) - ( iDay * DAY_SEC );
		int iHour   = iHourSec / HOUR_SEC;
		int iMinute = ( iHourSec - ( iHour * HOUR_SEC ) ) / MINUTE_SEC;

		if( iDay <= 0 && iHour <= 0 && iMinute <= 0 )
			SafeSprintf( szTime, MAX_PATH, STR(1) );
		else if( iDay <= 0 && iHour <= 0 && iMinute > 0 )
			SafeSprintf( szTime, MAX_PATH, STR(2), iMinute );
		else if( iDay <= 0 && iHour > 0 && iMinute <= 0 )
			SafeSprintf( szTime, MAX_PATH, STR(3), iHour );
		else if( iDay <= 0 && iHour > 0 && iMinute > 0 )
			SafeSprintf( szTime, MAX_PATH, STR(6), iHour, iMinute );
		else if( iDay > 0 && iHour <= 0 )
			SafeSprintf( szTime, MAX_PATH, STR(4), iDay );
		else 
			SafeSprintf( szTime, MAX_PATH, STR(5), iDay, iHour );

		szItemName = szTime;
	}
	else if( pInfo->ItemIndex / ioEtcItem::USE_TYPE_CUT_VALUE == ioEtcItem::UT_DATE )
	{
		char szTime[MAX_PATH] = {0,};
		SafeSprintf( szTime, MAX_PATH, STR(4), iCount );

		szItemName = szTime;
	}
	else
	{
		char szCount[MAX_PATH] = {0, };
		SafeSprintf( szCount, MAX_PATH, STR(7), iCount );

		szItemName = szCount;
	}
}

int ioExcavationManager::GetArtifactCount()
{
	if( m_pInfoManager )
		return m_pInfoManager->GetTotal();

	return 0;
}

LSC_Excavation_info *ioExcavationManager::GetArtifactInfoByArray( int iArray )
{
	if( m_pInfoManager )
		return m_pInfoManager->GetAt( iArray );

	return NULL;
}

LSC_Excavation_info *ioExcavationManager::GetArtifactInfo( int iKey )
{
	if( m_pInfoManager )
		return m_pInfoManager->GetData( iKey );

	return NULL;
}

LSC_Excavation_grade *ioExcavationManager::GetGradeInfo( int iKey )
{
	if( m_pGradeManager )
		return m_pGradeManager->GetData( iKey );

	return NULL;
}

void ioExcavationManager::SetRemainChargingTime( DWORD dwNextCharginfTime )
{
	m_dwRemainChargeTime = REALGETTIME() + dwNextCharginfTime * 1000;
}

DWORD ioExcavationManager::GetRemainChargeTime( bool bRemain )
{
	if( m_iShovelCount >= m_iMaxShovelCount )
		return 0;

	if( m_dwRemainChargeTime < REALGETTIME() )
		return 0;

	if( bRemain )
		return m_dwRemainChargeTime - REALGETTIME();
	else
		return m_dwRemainChargeTime;
}

bool ioExcavationManager::IsSendExcavationPacket()
{
	if( g_ExcavationMgr.GetPacketSendTime() == 0 || REALGETTIME() > g_ExcavationMgr.GetPacketSendTime() + ioExcavationManager::PACKET_SEND_DELAY )
		return true;

	return false;
}

bool ioExcavationManager::IsExcavating()
{
	if( !m_pStage )
		return false;

	ioBaseChar *pChar = m_pStage->GetOwnerChar();
	if( pChar && pChar->IsExcavating() )
		return true;

	return false;
}

/*bool ioExcavationManager::HasEtcItem()
{
	ioUserEtcItem *pUserEtcItem = g_MyInfo.GetUserEtcItem();
	if( pUserEtcItem )
	{
		ioUserEtcItem::ETCITEMSLOT kEtcItemSlot;
		if( pUserEtcItem->GetEtcItem( ioEtcItem::EIT_ETC_EXCAVATING_KIT, kEtcItemSlot ) )
			return true;
	}
	return false;
}*/

bool ioExcavationManager::OnExcavationCommand( SP2Packet &rkPacket )
{
	TCPNetwork::MouseBusy( false, CTPK_EXCAVATION_COMMAND );
	m_dwPacketSendTime = REALGETTIME();

	int iCommand;
	PACKET_GUARD_BOOL_READ( rkPacket, iCommand );

	switch( iCommand )
	{
	case EQUIP_EXCAVATION_KIT:
		{
			int iResult;
			PACKET_GUARD_BOOL_READ( rkPacket, iResult );
			switch( iResult )
			{
			case EQUIP_KIT_SUCCESS:
				{
					ioHashString szName;
					PACKET_GUARD_BOOL_READ( rkPacket, szName );

					int iExcavationLevel;
					PACKET_GUARD_BOOL_READ( rkPacket, iExcavationLevel );

					int iX, iY, iZ;
					PACKET_GUARD_BOOL_READ( rkPacket, iX );
					PACKET_GUARD_BOOL_READ( rkPacket, iY );
					PACKET_GUARD_BOOL_READ( rkPacket, iZ );

					if( g_MyInfo.GetPublicID() == szName )
					{
						m_ExcavationPoint = D3DXVECTOR3( (float)iX, (float)iY, (float)iZ );
					}
					else
					{
						CheckUserExcavationLv( szName, iExcavationLevel );
					}

					SetExcavation( szName, true );
				}
				break;
			case EXCAVATION_FAIL_NEED_KIT:
				{
					g_ChatMgr.SetSystemMsg( m_ErrorMsg[ERR_NEED_KIT] );
				}
				break;
			case EXCAVATION_IMPOSSIBLE_MODE:
				{
					g_ChatMgr.SetSystemMsg( m_ErrorMsg[ERR_INVAILD_MODE] );
				}
				break;
			case EXCEPTION_ERROR:
				{
					g_ChatMgr.SetSystemMsg( m_ErrorMsg[ERR_INVAILD_MODE] );
				}
				break;
			}
		}
		return true;

	case RELEASE_EXCAVATION_KIT:
		{
			ioHashString szName;
			PACKET_GUARD_BOOL_READ( rkPacket, szName );
			g_ExcavationMgr.SetExcavation( szName, false );

			if( g_MyInfo.GetPublicID() == szName )
				g_GUIMgr.HideWnd( EXCAVATION_WND );
		}
		return true;

	case START_DIGGING:
		{
			int iResult;
			PACKET_GUARD_BOOL_READ( rkPacket, iResult );

			ioHashString szName;
			PACKET_GUARD_BOOL_READ( rkPacket, szName );

			if( g_MyInfo.GetPublicID() == szName )
			{
				SetDiggingState();
			}

			if( iResult == DIGGING_SUCCESS )
			{
				SetDiggingMotion( szName );
				//UpdateShovelCount();
			}
		}
		return true;

	case EXCAVATION_RESULT:
		{
			m_dwShowResultTime = FRAMEGETTIME() + m_dwResultTime;

			int iResult;
			PACKET_GUARD_BOOL_READ( rkPacket, iResult );

			switch( iResult )
			{
			case EXCAVATION_SUCCESS:
				{
					ioHashString szName;
					PACKET_GUARD_BOOL_READ( rkPacket, szName );

					int iItemType, iItemIdx, iGradeIdx, iPrice, iIdentifyPrice, iGetCnt;
					PACKET_GUARD_BOOL_READ( rkPacket, iItemType );
					PACKET_GUARD_BOOL_READ( rkPacket, iItemIdx );
					PACKET_GUARD_BOOL_READ( rkPacket, iGradeIdx );
					PACKET_GUARD_BOOL_READ( rkPacket, iPrice );
					PACKET_GUARD_BOOL_READ( rkPacket, iGetCnt );
					PACKET_GUARD_BOOL_READ( rkPacket, iIdentifyPrice );

					// 다른 유저에게 보여줄 아이콘을 위한 임시 저장용 변수.
					m_iReserveItemType = iItemType;
					m_iReserveItemIndex = iItemIdx;

					int iExcavationLV, iExcavationEXP;
					PACKET_GUARD_BOOL_READ( rkPacket, iExcavationLV );
					PACKET_GUARD_BOOL_READ( rkPacket, iExcavationEXP );

					int iClassType, iCLassLV, iClassExp;
					PACKET_GUARD_BOOL_READ( rkPacket, iClassType );
					PACKET_GUARD_BOOL_READ( rkPacket, iCLassLV );
					PACKET_GUARD_BOOL_READ( rkPacket, iClassExp );

					int iUserLV, iUserEXP;
					PACKET_GUARD_BOOL_READ( rkPacket, iUserLV );
					PACKET_GUARD_BOOL_READ( rkPacket, iUserEXP );

					m_vExcavationUser.push_back( ExcavationUserInfo( szName, true, FRAMEGETTIME() + m_dwResultTime, iItemIdx ) );
					CheckUserExcavationLv( szName, iExcavationLV );

					if( g_MyInfo.GetPublicID() == szName )
					{
						int iGetUserExp = iUserEXP - g_MyInfo.GetGradeExpert();
						int iGetClassExp = iClassExp - g_MyInfo.GetClassExpert( iClassType );
						bool bIsRental = g_MyInfo.IsCharRentalToClassType( iClassType );
						bool bIsLevelUp = false;

						if( g_MyInfo.GetGradeLevel() != iUserLV )
						{
							iGetUserExp = g_LevelMgr.GetNextGradeupExp( g_MyInfo.GetGradeLevel() ) - g_MyInfo.GetGradeExpert() + iUserEXP;
						}
						if( g_MyInfo.GetClassLevel(iClassType, bIsRental) != iCLassLV )
						{
							iGetClassExp = g_LevelMgr.GetNextLevelupExp( g_MyInfo.GetClassLevel(iClassType, bIsRental) ) - g_MyInfo.GetClassExpert(iClassType) + iClassExp;
						}

						if( g_MyInfo.GetExcavationLevel() < iExcavationLV )
						{
							m_kResultInfo.m_bLevelUp = true;
							bIsLevelUp = true;
						}

						if( g_MyInfo.GetGradeLevel() < iUserLV )
						{
							m_kResultInfo.m_bGradeLevelUp = true;
						}

						m_kResultInfo.m_iAddGradeExp += iGetUserExp;
						m_kResultInfo.m_iAddExp += iExcavationEXP;
						m_kResultInfo.m_iClassType = iClassType;
						m_kResultInfo.m_iAddSoldierExp += iGetClassExp;

						if( iUserLV == g_LevelMgr.GetMaxGradeLevel() )
							iGetUserExp = 0;

						if( iCLassLV == g_LevelMgr.GetSoldierMaxLevel() )
							iGetClassExp = 0;

						g_ChatMgr.SetSystemMsg( m_SysMsg[MSG_SUCCESS], iGetUserExp, iGetClassExp );

						g_MyInfo.SetExcavationLevel( iExcavationLV, iExcavationEXP );
						g_MyInfo.SetClassExpNLevel( iClassType, iClassExp, iCLassLV );
						g_MyInfo.SetGrade( iUserLV, iUserEXP );

						ExcavationWnd* pExcavationWnd = dynamic_cast<ExcavationWnd*>( g_GUIMgr.FindWnd( EXCAVATION_WND ) );
						if( pExcavationWnd )
						{
							pExcavationWnd->SetProcessDelay( true );
						}

						if( bIsLevelUp )
						{
							ChangeUseIcon();

							ExcavationLevelUpWnd* pLevelUp = dynamic_cast<ExcavationLevelUpWnd*>( g_GUIMgr.FindWnd( EXCAVATION_LEVELUP_WND ) );
							if( pLevelUp )
							{
								pLevelUp->SetLevel( iExcavationLV, IsChangeKitLevel( iExcavationLV ) );
							}

							if( pExcavationWnd )
							{
								pExcavationWnd->SetLevelUpDelay( true );
							}
						}

						if( iItemType == ioExcavationManager::RESULT_ITEM )
						{
							LSC_Excavation_info *pInfo  = g_ExcavationMgr.GetArtifactInfo( iItemIdx );
							if( pInfo )
							{
								int iRewardType = pInfo->RewardType;
								g_QuestMgr.QuestCompleteTerm( QC_EXCAVATION_SUCCESS, iItemType, iRewardType, iItemIdx );
							}
						}

						ExcavationResultWnd *pResultWnd = dynamic_cast<ExcavationResultWnd*>( g_GUIMgr.FindWnd(EXCAVATION_RESULT_WND) );
						if( pResultWnd )
						{
							pResultWnd->SetReserveResultItem( iItemType, iItemIdx, iGradeIdx, iPrice, iIdentifyPrice, iGetCnt );
						}

						g_QuestMgr.QuestCompleteTermReserve( QC_EXCAVATION_LEVELUP );
					}
				}
				break;
			case EXCAVATION_FAIL_NEED_KIT:
				{
					ioHashString szName;
					PACKET_GUARD_BOOL_READ( rkPacket, szName );
					m_vExcavationUser.push_back( ExcavationUserInfo( szName, false, FRAMEGETTIME() + m_dwResultTime, 0 ) );
				}
				break;
			case EXCAVATION_FAIL_INVALID_POSITION:
				{
					ioHashString szName;
					PACKET_GUARD_BOOL_READ( rkPacket, szName );
					m_vExcavationUser.push_back( ExcavationUserInfo( szName, false, FRAMEGETTIME() + m_dwResultTime, 0 ) );

					int iExcavationLV, iExcavationEXP;
					PACKET_GUARD_BOOL_READ( rkPacket, iExcavationLV );
					PACKET_GUARD_BOOL_READ( rkPacket, iExcavationEXP );

					int iClassType, iCLassLV, iClassExp;
					PACKET_GUARD_BOOL_READ( rkPacket, iClassType );
					PACKET_GUARD_BOOL_READ( rkPacket, iCLassLV );
					PACKET_GUARD_BOOL_READ( rkPacket, iClassExp );

					int iUserLV, iUserEXP;
					PACKET_GUARD_BOOL_READ( rkPacket, iUserLV );
					PACKET_GUARD_BOOL_READ( rkPacket, iUserEXP );

					CheckUserExcavationLv( szName, iExcavationLV );

					if( g_MyInfo.GetPublicID() == szName )
					{
						int iGetUserExp = iUserEXP - g_MyInfo.GetGradeExpert();
						int iGetClassExp = iClassExp - g_MyInfo.GetClassExpert( iClassType );
						bool bIsRental = g_MyInfo.IsCharRentalToClassType( iClassType );
						bool bIsLevelUp = false;

						if( g_MyInfo.GetGradeLevel() != iUserLV )
						{
							iGetUserExp = g_LevelMgr.GetNextGradeupExp( g_MyInfo.GetGradeLevel() ) - g_MyInfo.GetGradeExpert() + iUserEXP;
						}
						if( g_MyInfo.GetClassLevel(iClassType, bIsRental) != iCLassLV )
						{
							iGetClassExp = g_LevelMgr.GetNextLevelupExp( g_MyInfo.GetClassLevel(iClassType, bIsRental) ) - g_MyInfo.GetClassExpert(iClassType) + iClassExp;
						}

						if( g_MyInfo.GetExcavationLevel() < iExcavationLV )
						{
							m_kResultInfo.m_bLevelUp = true;
							bIsLevelUp = true;
						}

						if( g_MyInfo.GetGradeLevel() < iUserLV )
						{
							m_kResultInfo.m_bGradeLevelUp = true;
						}

						m_kResultInfo.m_iAddGradeExp += iGetUserExp;
						m_kResultInfo.m_iAddExp += iExcavationEXP;
						m_kResultInfo.m_iClassType = iClassType;
						m_kResultInfo.m_iAddSoldierExp += iGetClassExp;

						g_MyInfo.SetExcavationLevel( iExcavationLV, iExcavationEXP );
						g_MyInfo.SetClassExpNLevel( iClassType, iClassExp, iCLassLV );
						//g_MyInfo.SetClassLevel( iClassType, iCLassLV );
						//g_MyInfo.SetClassExp( iClassType, iClassExp );
						g_MyInfo.SetGrade( iUserLV, iUserEXP );

						ExcavationWnd* pExcavationWnd = dynamic_cast<ExcavationWnd*>( g_GUIMgr.FindWnd( EXCAVATION_WND ) );
						if( pExcavationWnd )
						{
							pExcavationWnd->SetProcessDelay( true );
						}

						if( bIsLevelUp )
						{
							ChangeUseIcon();

							ExcavationLevelUpWnd* pLevelUp = dynamic_cast<ExcavationLevelUpWnd*>( g_GUIMgr.FindWnd( EXCAVATION_LEVELUP_WND ) );
							if( pLevelUp )
							{
								pLevelUp->SetLevel( iExcavationLV, IsChangeKitLevel( iExcavationLV ) );
							}

							if( pExcavationWnd )
							{
								pExcavationWnd->SetLevelUpDelay( true );
							}
						}

						g_ChatMgr.SetSystemMsg( m_ErrorMsg[ERR_INVAILD_POS] );
					}
				}
				break;
			case EXCAVATION_FAIL_NONE_EQUIP_KIT:
				{
					ioHashString szName;
					PACKET_GUARD_BOOL_READ( rkPacket, szName );
					m_vExcavationUser.push_back( ExcavationUserInfo( szName, false, FRAMEGETTIME() + m_dwResultTime, 0 ) );

					if( g_MyInfo.GetPublicID() == szName )
					{
						g_ChatMgr.SetSystemMsg( m_ErrorMsg[ERR_NO_EQUIP_KIT] );
					}
				}
				break;
			case EXCAVATION_TIMEOUT:
				{
					ioHashString szName;
					PACKET_GUARD_BOOL_READ( rkPacket, szName );
					m_vExcavationUser.push_back( ExcavationUserInfo( szName, false, FRAMEGETTIME() + m_dwResultTime, 0 ) );
				}
				break;
			case EXCAVATION_COOLTIME_LIMIT:
				{
					DWORD dwGapTime;
					PACKET_GUARD_BOOL( rkPacket.Read(dwGapTime) );

					m_iExcavationState = ES_CHARGING;
					m_dwStateCheckTime = REALGETTIME() - dwGapTime;

					if( m_pStage )
					{
						ioBaseChar *pChar = m_pStage->GetOwnerChar();
						if( pChar )
						{
							pChar->SetState( CS_DELAY );
						}
					}
				}
				break;
			case EXCAVATION_FAIL_ADDICITON:
				{
				}
				break;
			case EXCEPTION_ERROR:
				{
					ioHashString szName;
					PACKET_GUARD_BOOL_READ( rkPacket, szName );
					m_vExcavationUser.push_back( ExcavationUserInfo( szName, false, FRAMEGETTIME() + m_dwResultTime, 0 ) );

					CheckExceptionError( iCommand, iResult );
				}
				break;
			}
		}
		break;

	case REQUEST_REAPPRAISAL:
		{
			int iResult;
			PACKET_GUARD_BOOL_READ( rkPacket, iResult );
			switch ( iResult )
			{
			case REAPPRAISAL_SUCCESS:
				{
					int iItemType, iItemIdx, iGradeIdx, iPrice, iIdentifyPrice, iGetCnt;
					INT64 iCurGold;
					PACKET_GUARD_BOOL_READ( rkPacket, iItemType );
					PACKET_GUARD_BOOL_READ( rkPacket, iItemIdx );
					PACKET_GUARD_BOOL_READ( rkPacket, iGradeIdx );
					PACKET_GUARD_BOOL_READ( rkPacket, iPrice );
					PACKET_GUARD_BOOL_READ( rkPacket, iGetCnt );
					PACKET_GUARD_BOOL_READ( rkPacket, iIdentifyPrice );
					PACKET_GUARD_BOOL_READ( rkPacket, iCurGold );
					
					g_MyInfo.SetMoney( iCurGold );

					ExcavationResultWnd *pResultWnd = dynamic_cast<ExcavationResultWnd*>( g_GUIMgr.FindWnd(EXCAVATION_RESULT_WND) );
					if( pResultWnd )
					{
						pResultWnd->SetReappraisal( iItemType, iItemIdx, iGradeIdx, iPrice, iIdentifyPrice, iGetCnt );
					}
				}
				break;
			case GOLD_SHORTAGE:
				{
				}
				break;
			case INVALID_REAPPRAISAL_ITEM:
				{
				}
				break;
			case EXCEPTION_ERROR:
				{
					CheckExceptionError( iCommand, iResult );
				}
			}
		}
		return true;

	case IDENTIFY_COMPLETE:
		{
			int iResult;
			PACKET_GUARD_BOOL_READ( rkPacket, iResult );
			switch( iResult )
			{
			case IDENTIFY_SUCCESS:
				{
					ioHashString szName;
					rkPacket >> szName;

					ioBaseChar *pChar = NULL;
					if( m_pStage )
						pChar = m_pStage->GetOwnerChar();

					if( pChar && szName == pChar->GetCharName() )
					{
						__int64 iGetGold;
						int iIndex, iType, iGradeIdx;
						rkPacket >> iGradeIdx;
						rkPacket >> iIndex;
						rkPacket >> iType;
						rkPacket >> iGetGold;
						g_MyInfo.SetMoney( iGetGold );

						int iX, iY, iZ;
						rkPacket >> iX;
						rkPacket >> iY;
						rkPacket >> iZ;
						m_ExcavationPoint = D3DXVECTOR3( (float)iX, (float)iY, (float)iZ );

						pChar->SetExcavationPos( m_ExcavationPoint );

						SetChargingState();

						g_GUIMgr.HideWnd( EXCAVATION_RESULT_WND );

						if( iGetGold > 0 && iType == ioExcavationManager::RESULT_ARTIFACT )
						{
							g_QuestMgr.QuestCompleteTerm( QC_EXCAVATION_SUCCESS, iType, iGetGold, iIndex );
						}
					}					
				}
				break;

			case IDENTIFY_SUCCESS_ROOM:
				{
					ioHashString szName;
					rkPacket >> szName;

					int iRewardType, iIndex, iType, iGrade, iMultiple, iGetGold;
					bool bRoomAlarm, bServerAlarm;
					rkPacket >> iRewardType >> iIndex >> iType >> iGrade >> iGetGold >> iMultiple >> bRoomAlarm >> bServerAlarm;

					if( bServerAlarm == false && bRoomAlarm == true )
						SetExcavatingSuccessAlarm( szName, iRewardType, iIndex, iType, iGrade, iGetGold, iMultiple, false );
				}
				break;

			case EXCEPTION_ERROR:
				{
					ioHashString szName;
					PACKET_GUARD_BOOL_READ( rkPacket, szName );
					
					ioBaseChar *pChar = NULL;
					if( m_pStage )
						pChar = m_pStage->GetOwnerChar();

					if( pChar && szName == pChar->GetCharName() )
					{
						SetChargingState();

						g_GUIMgr.HideWnd( EXCAVATION_RESULT_WND );
					}

					CheckExceptionError( iCommand, iResult, true );
				}
				break;
			}
		}
		return true;

	case TIME_RECHARGE_SHOVEL:
		{
			m_bSendRechargeProcess = false;

			int iResult;
			PACKET_GUARD_BOOL_READ( rkPacket, iResult );
			switch( iResult )
			{
			case RECHARGE_KIT_SUCCESS:
				{
					ExcavationShovelWnd *pExcavationShovelWnd = static_cast<ExcavationShovelWnd*>( g_GUIMgr.FindWnd( EXCAVATION_SHOVEL_WND ) );
					if( pExcavationShovelWnd )
					{
						pExcavationShovelWnd->UpdateUI();
					}
				}
				break;
			case RECHARGE_KIT_FAIL_MAX_COUNT:
				{
					g_ChatMgr.SetSystemMsg( m_ErrorMsg[ERR_SHOVEL_MAX_COUNT] );
				}
				break;
			case RECHARGE_KIT_FAIL_TIME_REMAIN:
				{
					DWORD dwNextChargeTime;
					PACKET_GUARD_BOOL_READ( rkPacket, dwNextChargeTime );

					SetRemainChargingTime( dwNextChargeTime );
				}
				break;
			case EXCEPTION_ERROR:
				{
					CheckExceptionError( iCommand, iResult );
				}
				break;
			}
		}
		return true;

	case RECHARGE_SHOVEL:
		{
			int iResult;
			PACKET_GUARD_BOOL_READ( rkPacket, iResult );
			switch( iResult )
			{
			case RECHARGE_KIT_SUCCESS:
				{
					ExcavationShovelWnd *pExcavationShovelWnd = static_cast<ExcavationShovelWnd*>( g_GUIMgr.FindWnd( EXCAVATION_SHOVEL_WND ) );
					if( pExcavationShovelWnd )
					{
						pExcavationShovelWnd->UpdateUI();
					}
				}
				break;
			case RECHARGE_KIT_FAIL_MAX_COUNT:
				{
					g_ChatMgr.SetSystemMsg( m_ErrorMsg[ERR_SHOVEL_MAX_COUNT] );
				}
				break;
			case RECHARGE_KIT_FAIL_TIME_REMAIN:
				{
					DWORD dwNextChargeTime;
					PACKET_GUARD_BOOL_READ( rkPacket, dwNextChargeTime );

					SetRemainChargingTime( dwNextChargeTime );
				}
				break;
			}
		}
		return true;

	case EXCEPTION_ERROR:
		{
			CheckExceptionError( iCommand, 0, true );
		}
		break;
	}

	return false;
}

void ioExcavationManager::SendEquipKit()
{
	if( g_App.IsMouseBusy() )
		return;

	if( !g_ExcavationMgr.IsSendExcavationPacket() )
		return;

	SP2Packet kPacket( CTPK_EXCAVATION_COMMAND );
	kPacket << EQUIP_EXCAVATION_KIT;
	kPacket << g_App.GetCurModeSubNum();
	TCPNetwork::SendToServer( kPacket );
	TCPNetwork::MouseBusy( true, CTPK_EXCAVATION_COMMAND );

	if( m_pStage )
	{
		SP2Packet kPacket2( CUPK_EXCAVATING );
		kPacket2 << m_pStage->GetOwnerChar()->GetCharName();
		P2PNetwork::SendToAllPlayingUser( kPacket2 );
	}

	//m_dwPacketSendTime = REALGETTIME();
}

void ioExcavationManager::SendReleaseKit()
{
	if( g_App.IsMouseBusy() )
		return;

	if( !g_ExcavationMgr.IsSendExcavationPacket() )
		return;

	if( m_iExcavationState == ES_DIGGING )
		return;

	ioBaseChar* pChar = m_pStage->GetBaseChar( g_MyInfo.GetPublicID() );
	if( pChar && pChar->GetState() != CS_DELAY )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		return;
	}

	SP2Packet kPacket( CTPK_EXCAVATION_COMMAND );
	kPacket << RELEASE_EXCAVATION_KIT;
	TCPNetwork::SendToServer( kPacket );
	TCPNetwork::MouseBusy( true, CTPK_EXCAVATION_COMMAND );

	//m_dwPacketSendTime = REALGETTIME();
}

void ioExcavationManager::SendStartDigging()
{
	if( g_App.IsMouseBusy() )
		return;

	if( !g_ExcavationMgr.IsSendExcavationPacket() )
		return;

	if( !m_pStage )
		return;

	ioBaseChar *pChar = m_pStage->GetOwnerChar();
	if( !pChar )
		return;

	SP2Packet kPacket( CTPK_EXCAVATION_COMMAND );
	kPacket << START_DIGGING;
	kPacket << static_cast<int>( pChar->GetWorldPosition().x );
	kPacket << static_cast<int>( pChar->GetWorldPosition().y );
	kPacket << static_cast<int>( pChar->GetWorldPosition().z );
	TCPNetwork::SendToServer( kPacket );
	TCPNetwork::MouseBusy( true, CTPK_EXCAVATION_COMMAND );

	//m_dwPacketSendTime = REALGETTIME();
}

void ioExcavationManager::SendIdentify()
{
	if( g_App.IsMouseBusy() )
		return;

	if( !g_ExcavationMgr.IsSendExcavationPacket() )
		return;

	SP2Packet kPacket( CTPK_EXCAVATION_COMMAND );
	kPacket << REQUEST_REAPPRAISAL;
	TCPNetwork::SendToServer( kPacket );
	TCPNetwork::MouseBusy( true, CTPK_EXCAVATION_COMMAND );

	//m_dwPacketSendTime = REALGETTIME();
}

void ioExcavationManager::SendIdentifyComplete()
{
	if( g_App.IsMouseBusy() )
		return;

	if( !g_ExcavationMgr.IsSendExcavationPacket() )
		return;

	SP2Packet kPacket( CTPK_EXCAVATION_COMMAND );
	kPacket << IDENTIFY_COMPLETE;
	TCPNetwork::SendToServer( kPacket );
	TCPNetwork::MouseBusy( true, CTPK_EXCAVATION_COMMAND );

	//m_dwPacketSendTime = REALGETTIME();
}

void ioExcavationManager::SendTimeRechargeShovel()
{
	if( g_App.IsMouseBusy() )
		return;

	if( !g_ExcavationMgr.IsSendExcavationPacket() )
		return;

	SP2Packet kPacket( CTPK_EXCAVATION_COMMAND );
	kPacket << TIME_RECHARGE_SHOVEL;
	TCPNetwork::SendToServer( kPacket );
	TCPNetwork::MouseBusy( true, CTPK_EXCAVATION_COMMAND );

	m_bSendRechargeProcess = true;
	//m_dwPacketSendTime = REALGETTIME();
}

void ioExcavationManager::SendRechargeShovel()
{
	if( g_App.IsMouseBusy() )
		return;

	if( !g_ExcavationMgr.IsSendExcavationPacket() )
		return;

	SP2Packet kPacket( CTPK_EXCAVATION_COMMAND );
	kPacket << RECHARGE_SHOVEL;
	TCPNetwork::SendToServer( kPacket );
	TCPNetwork::MouseBusy( true, CTPK_EXCAVATION_COMMAND );

	//m_dwPacketSendTime = REALGETTIME();
}

void ioExcavationManager::SetShovelCount( int iCnt )
{
	int iPreCount = m_iShovelCount;
	m_iShovelCount = iCnt;
	m_iShovelCount = max( 0, m_iShovelCount  );
	m_iShovelCount = min( m_iShovelCount, m_iMaxShovelCount );

	if( iPreCount == 0 && iCnt > 0 && GetExcavationState() == ES_DISABLE )
	{
		SetChargingState();
	}
}

void ioExcavationManager::UpdateShovelCount()
{
	ioUserEtcItem *pUserEtcItem = g_MyInfo.GetUserEtcItem();
	ioUserEtcItem::ETCITEMSLOT kEtcData;
	if( pUserEtcItem && pUserEtcItem->GetEtcItem( ioEtcItem::EIT_ETC_NEW_EXCAVATING_KIT, kEtcData ) )
	{
		/*int iPreCnt = g_ExcavationMgr.GetShovelCount();
		int iCurCnt = kEtcData.m_iValue1;
		g_ExcavationMgr.SetShovelCount( kEtcData.m_iValue1 );
		g_ExcavationMgr.SetRemainChargingTime( kEtcData.m_iValue2 );

		ExcavationWnd *pExcavationWnd = dynamic_cast<ExcavationWnd*>( g_GUIMgr.FindWnd( EXCAVATION_WND ) );
		if( pExcavationWnd && pExcavationWnd->IsShow() && iPreCnt < iCurCnt )
		{
			pExcavationWnd->SetShovelEffect();
		}*/

		if( g_GUIMgr.IsShow( EXCAVATION_SHOVEL_WND ) )
		{
			ExcavationShovelWnd *pExcavationShovelWnd = static_cast<ExcavationShovelWnd*>( g_GUIMgr.FindWnd( EXCAVATION_SHOVEL_WND ) );
			if( pExcavationShovelWnd )
			{
				pExcavationShovelWnd->UpdateUI();
			}
		}
	}
}

ioUIRenderImage *ioExcavationManager::GetRewardIcon( int iIndex )
{
	auto FindReward = [iIndex]( RewardIcon kInfo )->bool{ return kInfo.index == iIndex; };
	vRewardIcon::iterator iter = std::find_if( m_vRewardIcon.begin(), m_vRewardIcon.end(), FindReward );
	if( iter != m_vRewardIcon.end() )
		return (*iter).icon;

	return NULL;
}

ioExcavationManager::UserExcavationLvInfo *ioExcavationManager::FindUser( const ioHashString &szName )
{
	auto FindUserInfo = [szName]( UserExcavationLvInfo kUserInfo )->bool{ return kUserInfo.public_id == szName; };
	vUserExcavationLvInfo::iterator iter = std::find_if( m_vUserExcavationLv.begin(), m_vUserExcavationLv.end(), FindUserInfo );
	if( iter != m_vUserExcavationLv.end() )
	{
		UserExcavationLvInfo &kInfo = *iter;
		return &kInfo;
	}

	return NULL;
}

void ioExcavationManager::CheckUserExcavationLv( const ioHashString &szName, int iLevel )
{
	if( g_MyInfo.GetPublicID() == szName )
		return;

	UserExcavationLvInfo *pInfo = FindUser( szName );
	if( pInfo )
	{
		pInfo->excavation_lv = iLevel;
	}
	else
	{
		m_vUserExcavationLv.push_back( UserExcavationLvInfo( szName, iLevel ) );
	}
}

void ioExcavationManager::EraseUserExcavationLv( const ioHashString &szName )
{
	UserExcavationLvInfo *pInfo = FindUser( szName );

	if( !pInfo )
		return;

	for( vUserExcavationLvInfo::iterator iter = m_vUserExcavationLv.begin(); iter != m_vUserExcavationLv.end(); ++iter )
	{
		if( pInfo->public_id == szName )
		{
			m_vUserExcavationLv.erase( iter );
			break;
		}
	}
}

void ioExcavationManager::ClearExcavationUserInfo()
{
	m_vExcavationUser.clear();
	m_vUserExcavationLv.clear();
}

bool ioExcavationManager::EnableShowResult()
{
	ExcavationResultWnd *pExcavationResultWnd = dynamic_cast<ExcavationResultWnd*>( g_GUIMgr.FindWnd(EXCAVATION_RESULT_WND) );
	if( pExcavationResultWnd && !pExcavationResultWnd->IsReserved() )
	{
		return false;
	}

	int iSize = m_vExcavationUser.size();
	for( int i=0; i<iSize; ++i )
	{
		ExcavationUserInfo kMyInfo = m_vExcavationUser[i];
		if( kMyInfo.name == g_MyInfo.GetPublicID() && kMyInfo.result_show_time > FRAMEGETTIME() )
		{
			return false;
		}
	}

	return true;
}

int ioExcavationManager::GetUserExcavationLv( const ioHashString &szName )
{
	if( g_MyInfo.GetPublicID() == szName )
		return g_MyInfo.GetExcavationLevel();

	UserExcavationLvInfo *pInfo = FindUser( szName );
	if( pInfo )
	{
		return pInfo->excavation_lv;
	}

	return 0;
}

bool ioExcavationManager::GetUseIconName( ioHashString &szIconName )
{
	int iCnt = m_vIconInfo.size();
	for( int i=0; i<iCnt; ++i )
	{
		if( m_vIconInfo[i].EnableIcon( g_MyInfo.GetExcavationLevel() ) )
		{
			szIconName = m_vIconInfo[i].use_icon_name;
			return true;
		}
	}

	return false;
}

bool ioExcavationManager::GetUseIconName( int iLevel, ioHashString &szIconName )
{
	int iCnt = m_vIconInfo.size();
	for( int i=0; i<iCnt; ++i )
	{
		if( m_vIconInfo[i].EnableIcon( iLevel ) )
		{
			szIconName = m_vIconInfo[i].use_icon_name;
			return true;
		}
	}

	return false;
}

bool ioExcavationManager::IsChangeKitLevel( int iLevel )
{
	int iCnt = m_vIconInfo.size();
	for( int i=0; i<iCnt; ++i )
	{
		if( m_vIconInfo[i].min_lv == iLevel )
		{
			return true;
		}
	}

	return false;
}

bool ioExcavationManager::IsForkcraneLevel( int iLevel )
{
	if( m_vIconInfo[EQUIPMENT_FORKCRANE].min_lv <= iLevel )
		return true;

	return false;
}

void ioExcavationManager::ChangeUseIcon()
{
	ExcavationWnd *pWnd = dynamic_cast<ExcavationWnd*>( g_GUIMgr.FindWnd(EXCAVATION_WND) );
	if( pWnd )
	{
		pWnd->ChangeUseIcon();
	}
}

void ioExcavationManager::CheckExceptionError( int iCommand, int iResult, bool bChangeDelayState /* = false */ )
{
	g_GUIMgr.SetMsgBox( MB_OK, NULL, "EXCEPTION_ERROR (%1:%2)", iCommand, iResult );

	if( bChangeDelayState )
	{
		if( m_pStage )
		{
			ioBaseChar *pOwner = m_pStage->GetOwnerChar();
			if( pOwner && pOwner->GetState() != CS_DELAY )
			{
				pOwner->SetState( CS_DELAY );
			}
		}
	}
}

void ioExcavationManager::SetExcavatingSuccessAlarm( ioHashString &szSendID, int iRewardType, int iIndex, int iType, int iGrade, int iPrice, int iMultiple, bool bAllAlarm )
{
	ioComplexStringPrinter kPrinter;
	if( bAllAlarm )
		g_ChatMgr.PresetChatComplexString( MSG_ANNOUNCE_CHAT, kPrinter );
	else
		g_ChatMgr.PresetChatComplexString( MSG_INFO_CHAT, kPrinter );

	kPrinter.SetTextColor( TCT_DEFAULT_VIOLET );
	kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), szSendID.c_str() );

	kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
	kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(2) );

	char szMent[MAX_PATH]="";
	//if( iType < ioExcavationManager::EXTRAITEM_START_TYPE )
	if( iType == ioExcavationManager::RESULT_ARTIFACT )
	{
		LSC_Excavation_info *pInfo = GetArtifactInfo( iIndex );
		StringCbPrintf( szMent, sizeof( szMent ), "%s", pInfo->ItemName );
	}
	else
	{
		/*ioHashString szName = g_PresentMgr.GetPresentValue1Text( PRESENT_EXTRAITEM, iArtifactValue1, 0 );

		int iHours = (iArtifactValue2%10000); // %10000은 선물하기 value2의 장비 기간값과 동일함.				
		if( iHours == 0 )
			SafeSprintf( szMent, sizeof( szMent ), STR(3), iArtifactValue2/10000, szName.c_str() );
		else if( ( iHours/24 ) > 0 )
			SafeSprintf( szMent, sizeof( szMent ), STR(4), iArtifactValue2/10000, szName.c_str(), iHours/24 );
		else
			SafeSprintf( szMent, sizeof( szMent ), STR(5), iArtifactValue2/10000, szName.c_str(), iHours );*/
		if( iRewardType == PRESENT_ETC_ITEM )
		{
			ioEtcItem* pEtcItem = g_EtcItemMgr.FindEtcItem( iIndex );

			if( pEtcItem )
			{
				ioHashString szName = pEtcItem->GetName();

				int iEtcItemType = pEtcItem->GetType() / ioEtcItem::USE_TYPE_CUT_VALUE;
				if( iEtcItemType == ioEtcItem::UT_COUNT || iEtcItemType == ioEtcItem::UT_ONCE || iEtcItemType == ioEtcItem::UT_ETERNITY )
				{
					SafeSprintf( szMent, sizeof(szMent), STR(3), iMultiple, szName.c_str() );
				}
				else if( iEtcItemType == ioEtcItem::UT_TIME )
				{
					int iHours = pEtcItem->GetValue( 0 ) / 3600;

					if( iHours >= 24 )
					{
						SafeSprintf( szMent, sizeof(szMent), STR(4), iMultiple, szName.c_str(), iHours / 24 );
					}
					else
					{
						SafeSprintf( szMent, sizeof(szMent), STR(5), iMultiple, szName.c_str(), iHours );
					}
				}
				else if( iEtcItemType == ioEtcItem::UT_DATE )
				{
					int iDate = pEtcItem->GetValue( 0 );
					SafeSprintf( szMent, sizeof(szMent), STR(4), iMultiple, szName.c_str(), iDate );
				}

				/*if( pEtcItem->GetUseType() == ioEtcItem::UT_TIME )
				{
					int iHours = pEtcItem->GetValue( 0 ) / 3600;
					if( ( iHours/24 ) > 0 )
						SafeSprintf( szMent, sizeof( szMent ), STR(4), iArtifactValue2/10000, szName.c_str(), iHours/24 );
					else
						SafeSprintf( szMent, sizeof( szMent ), STR(5), iArtifactValue2/10000, szName.c_str(), iHours );
				}
				else //if( pEtcItem->GetUseType() == ioEtcItem::UT_ETERNITY )
				{
					SafeSprintf( szMent, sizeof( szMent ), STR(3), iArtifactValue2/10000, szName.c_str() );
				}*/
			}
		}
	}

	kPrinter.SetTextColor( TCT_DEFAULT_VIOLET );
	kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), szMent );

	ZeroMemory( szMent, sizeof( szMent ) );
	//if( iType < ioExcavationManager::EXTRAITEM_START_TYPE )
	if( iType == ioExcavationManager::RESULT_ARTIFACT )
	{
		char szComma[MAX_PATH]="";
		LSC_Excavation_grade *pGrade = GetGradeInfo( iGrade );
		int iPeso = iPrice;
		//iPeso = ( iPeso * pGrade->GradeValue ) * fMapRate;
		iPeso = iPeso * iMultiple;

		Help::ConvertNumToStrComma( iPeso, szComma, sizeof( szComma ) );

		SafeSprintf( szMent, sizeof( szMent ), STR(6), pGrade->GradeName, szComma );
	}
	else
		SafeSprintf( szMent, sizeof( szMent ), STR(7) );

	kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
	kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), szMent );

	g_ChatMgr.SetChatComplexString( STR(1), kPrinter );
	//
}

void ioExcavationManager::OnExcavationBtnUp( ioBaseChar *pOwner )
{
	if( g_GUIMgr.IsShow( BRIEFING_WND ) ) 
		return;

	if( !pOwner )
	{
		LOG.PrintTimeAndLog( 0, "%s pOwner == NULL.", __FUNCTION__ );
		return;
	}

	if( pOwner->GetState() == CS_FISHING || pOwner->GetState() == CS_CONSTRUCT_HOUSE )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		return;
	}

	// 캐릭터 상태 체크
	if( pOwner->GetState() != CS_DELAY || pOwner->IsCatchMode() || pOwner->IsPrisonerMode() || pOwner->IsObejctEquipState() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
		return;
	}

	// Buff 체크
	if( pOwner->HasBuff( BT_LIMIT_MOVE ) || pOwner->HasBuff( BT_LIMIT_ACTION ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
		return;
	}

	// 아이템 체크
	if( pOwner->IsHasCrownByAllMode() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4) );
		return;
	}

	ioObjectItem *pObject = pOwner->GetObject();
	if( pObject && !ToExcavationItem( pObject ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(5) );
		return;
	}

	ioEquipSlot *pEquipslot = pOwner->GetEquipSlot();
	ioItem *pItem = NULL;
	if( pEquipslot )
		pItem = pEquipslot->GetEquipedItem( ES_WEAPON );
	if( pItem )
	{
		ioHookItem *pHookItem = ToHookItem( pItem );
		if( pHookItem && pHookItem->CheckOnHand() )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(6) );
			return;
		}
	}

	// 광장 체크
	if( pOwner->GetCurModeType() != MT_TRAINING )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(7) );
		return;
	}


	ioUserEtcItem *pUserEtcItem = g_MyInfo.GetUserEtcItem();
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}
	
	bool bExist = false;
	ioUserEtcItem::ETCITEMSLOT kItemSlot;
	if( pUserEtcItem->GetEtcItem( ioEtcItem::EIT_ETC_NEW_EXCAVATING_KIT, kItemSlot ) )
	{
		bExist = true;
	}

	if( !bExist )
	{
		g_GUIMgr.ShowWnd( EXCAVATION_BUY_WND );
		return;
	}

	SendEquipKit();

	if( !pOwner->IsExcavating() )
	{
		g_QuestMgr.QuestCompleteTerm( QC_EXCAVATION_TRY );
	}

	m_kResultInfo.Clear();
}

void ioExcavationManager::OnExcavationUDPPacket( SP2Packet &rkPacket, ioPlayStage *pPlayStage )
{
	if( !pPlayStage )
	{
		LOG.PrintTimeAndLog( 0, "%s pPlayStage == NULL. ", __FUNCTION__ );
		return;
	}

	ioHashString szPublicID;
	rkPacket >> szPublicID;

	ioBaseChar *pChar = pPlayStage->GetBaseChar( szPublicID );
	if( !pChar )
	{
		LOG.PrintTimeAndLog( 0, "%s pBaseChar == NULL. ", __FUNCTION__ );
		return;
	}

	ioEquipSlot* pEquipSlot = pChar->GetEquipSlot();
	if( !pEquipSlot )
	{
		LOG.PrintTimeAndLog( 0, "%s pEquipSlot == NULL. ", __FUNCTION__ );
		return;
	}

	ioExcavationItem *pExcavationItem = ToExcavationItem( pEquipSlot->GetObject() );
	if( !pExcavationItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pItem == NULL. ", __FUNCTION__ );
		return;
	}

	pExcavationItem->SetExcavatingStartAni( pChar );
}
//

void ioExcavationManager::NoticeLevelAndExp()
{
	if( m_kResultInfo.IsEmpty() )
		return;

	// 경험치 알림
	ioComplexStringPrinter kPrinter;
	g_ChatMgr.PresetChatComplexString( MSG_INFO_CHAT, kPrinter );

	kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
	kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(1) );

	kPrinter.SetTextColor( TCT_DEFAULT_VIOLET );
	kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(2), (int)m_kResultInfo.m_iAddExp );

	if( g_MyInfo.IsCharExerciseStyleToClassType( m_kResultInfo.m_iClassType, EXERCISE_RENTAL ) == false )
	{
		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(3), g_MyInfo.GetClassName( m_kResultInfo.m_iClassType, g_MyInfo.GetCharPowerUpGradeByClassType( m_kResultInfo.m_iClassType ) ) );

		kPrinter.SetTextColor( TCT_DEFAULT_VIOLET );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(4), (int)m_kResultInfo.m_iAddSoldierExp );
	}

	kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
	kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(5) );

	kPrinter.SetTextColor( TCT_DEFAULT_VIOLET );
	kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(6), (int)m_kResultInfo.m_iAddGradeExp );

	kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
	kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(7) );

	g_ChatMgr.SetChatComplexString( STR(8), kPrinter );

	if( m_kResultInfo.m_bGradeLevelUp )
	{
		//g_GUIMgr.ShowWnd( GRADE_UP_WND );
	}

	if( g_MyInfo.IsClassLevelUP( m_kResultInfo.m_iClassType ) )   // 용병 레벨업
	{
		int iPeso = g_MyInfo.GetClassLevelUPPeso( m_kResultInfo.m_iClassType );
		SoldierLevelUPWnd *pLevelUPWnd = dynamic_cast<SoldierLevelUPWnd*>(g_GUIMgr.FindWnd( SOLDIER_LEVEL_UP_WND ));
		if( pLevelUPWnd )
			pLevelUPWnd->SetLevelUPData( m_kResultInfo.m_iClassType, iPeso );
		g_MyInfo.InitLevelUPData();
	}

	g_QuestMgr.QuestCompleteTerm( QC_EXCAVATION_LEVELUP );
	// 레벨업 메세지
	if( m_kResultInfo.m_bLevelUp )
	{
		ioComplexStringPrinter kPrinter;
		g_ChatMgr.PresetChatComplexString( MSG_INFO_CHAT, kPrinter );
		kPrinter.SetTextColor( TCT_DEFAULT_VIOLET );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(9), g_MyInfo.GetPublicID().c_str(), (int)g_MyInfo.GetExcavationLevel() );
		g_ChatMgr.SetChatComplexString( STR(10), kPrinter );		
		g_QuestMgr.QuestOccurTerm( QO_EXCAVATION_LEVELUP );
	}
	m_kResultInfo.Clear();
}