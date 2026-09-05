
#include "stdafx.h"

#include "ioMyInfo.h"
#include "ioShuffleRoomMgr.h"
#include "ioLadderTeamMgr.h"
#include "ioBingo.h"
#include "ioReservedTask.h"
#include "ioSoldierAwakeManager.h"
#include "ioPowerUpManager.h"
#include "ioDateHelp.h"
#include "ioRSoldierInfoManager.h"
#include "ioPriateRullet.h"
#include "ioPriateRenewalRullet.h"
#include "ioRankBattleManager.h"
#include "ioMatchingGame.h"
#include "ioDiceGame.h"

#include "GUI/ioTimeGrowthEndWnd.h"
#include "GUI/ioMemoManager.h"
#include "GUI/MessengerWnd.h"
#include "GUI/NewShopItemPurchaseLeedWnd.h"
#include "GUI/FishingInventoryWnd.h"

//-------------------------------------------------------------------------
SoldierSelectUIBeforeLoading::SoldierSelectUIBeforeLoading()
{
	m_pCurrentCharBufferList = &m_CharBufferList;
}

SoldierSelectUIBeforeLoading::~SoldierSelectUIBeforeLoading()
{
	CharBufferList::iterator iter = m_CharBufferList.begin();
	for(;iter != m_CharBufferList.end();++iter)
	{
		UICharLoadingBuffer *pBuffer = *iter;
		SAFEDELETE( pBuffer->m_aMyChar );
		SAFEDELETE( pBuffer->m_pUI3DRender );
		SAFEDELETE( pBuffer );
	}
	m_CharBufferList.clear();

	iter = m_ArenaCharBufferList.begin();
	for(;iter != m_ArenaCharBufferList.end();++iter)
	{
		UICharLoadingBuffer *pBuffer = *iter;
		SAFEDELETE( pBuffer->m_aMyChar );
		SAFEDELETE( pBuffer->m_pUI3DRender );
		SAFEDELETE( pBuffer );
	}
	m_ArenaCharBufferList.clear();
}

void SoldierSelectUIBeforeLoading::ClearList()
{
	CharBufferList::iterator iter = m_pCurrentCharBufferList->begin();
	for(;iter != m_pCurrentCharBufferList->end();++iter)
	{
		UICharLoadingBuffer *pBuffer = *iter;
		SAFEDELETE( pBuffer->m_aMyChar );
		SAFEDELETE( pBuffer->m_pUI3DRender );
		SAFEDELETE( pBuffer );
	}
	m_pCurrentCharBufferList->clear();
}

bool SoldierSelectUIBeforeLoading::IsAlreadyChar( DWORD dwIndex )
{
	CharBufferList::iterator iter = m_pCurrentCharBufferList->begin();
	for(;iter != m_pCurrentCharBufferList->end();++iter)
	{
		UICharLoadingBuffer *pBuffer = *iter;
		if( pBuffer->m_dwCharIndex == dwIndex )
			return true;
	}
	return false;
}

void SoldierSelectUIBeforeLoading::Update( const CHARACTERDATA &rkCharacter )
{
	CharBufferList::iterator iter = m_pCurrentCharBufferList->begin();
	for(;iter != m_pCurrentCharBufferList->end();++iter)
	{
		UICharLoadingBuffer *pBuffer = *iter;
		if( pBuffer->m_aMyChar == NULL ) continue;
		if( pBuffer->m_pUI3DRender == NULL ) continue;

		if( pBuffer->m_dwCharIndex == rkCharacter.m_dwIndex )
		{
			SAFEDELETE( pBuffer->m_aMyChar );

			char szBuf[MAX_PATH] = "";
			wsprintf_e( szBuf, "MyChar%d", rkCharacter.m_dwIndex );
			pBuffer->m_aMyChar = pBuffer->m_pUI3DRender->CreateChar( rkCharacter.m_data, szBuf, true );
			if( !pBuffer->m_aMyChar )
				return;

			// Equip Item
			// 코스튬 관련 (용병선택창에서 캐릭터)
			for( int i=0 ; i< ioCreateChar::MAX_CCS ; i++ )
			{
				DWORD dwCostumeMaleCustom = rkCharacter.m_data.m_EquipCostume[i].m_Costume_Male_Custom;
				DWORD dwCostumeFemaleCustom = rkCharacter.m_data.m_EquipCostume[i].m_Costume_Female_Custom;

				if ( rkCharacter.m_data.m_EquipCostume[i].m_CostumeCode <= 0 )
				{
					dwCostumeMaleCustom = rkCharacter.m_equip_item[i].m_item_male_custom;
					dwCostumeFemaleCustom = rkCharacter.m_equip_item[i].m_item_female_custom;
				}
				pBuffer->m_aMyChar->EquipItem( rkCharacter.m_equip_item[i].m_item_code, dwCostumeMaleCustom, dwCostumeFemaleCustom, rkCharacter.m_data.m_EquipCostume[i].m_CostumeCode );
			}

			UpdateAwake( pBuffer, rkCharacter );

			pBuffer->m_bAnimationPlay = true;
			if( !pBuffer->m_aMyChar->SetWeaponShopMotion() )
			{
				ioHashString szUIAnimation = pBuffer->m_pUI3DRender->GetModelAnimation( 0, pBuffer->m_aMyChar->IsMale() );
				if( !szUIAnimation.IsEmpty() )
				{
					pBuffer->m_aMyChar->SetLoopAni( szUIAnimation );
				}
			}
			break;			
		}
	}
}

void SoldierSelectUIBeforeLoading::UpdateAwake( UICharLoadingBuffer *pBuffer, const CHARACTERDATA &rkCharacter )
{		
	if( pBuffer->m_aMyChar == NULL )
		return;

	if( pBuffer->m_pUI3DRender == NULL )
		return;

	if( pBuffer->m_dwCharIndex == rkCharacter.m_dwIndex )
	{
		if( rkCharacter.m_data.m_iAwakeType == AWAKE_NONE )
		{
			if( pBuffer->m_iAwakeEffect != AWAKE_NONE )
			{					
				pBuffer->m_aMyChar->EndEffect( g_AwakeMgr.GetAwakeEffect( pBuffer->m_iAwakeEffect ).c_str(), true );
				pBuffer->m_iAwakeEffect = AWAKE_NONE;
			}
		}
		else
		{
			if( pBuffer->m_iAwakeEffect == AWAKE_NONE )
			{
				pBuffer->m_iAwakeEffect = static_cast<AwakeType>( rkCharacter.m_data.m_iAwakeType );					
				pBuffer->m_aMyChar->AttachEffect( g_AwakeMgr.GetAwakeEffect( pBuffer->m_iAwakeEffect ).c_str(), NULL );
			}
		}
	}
}

void SoldierSelectUIBeforeLoading::ChangeCharIndex( DWORD dwPrevIndex, DWORD dwNextIndex )
{
	CharBufferList::iterator iter = m_pCurrentCharBufferList->begin();
	for(;iter != m_pCurrentCharBufferList->end();++iter)
	{
		UICharLoadingBuffer *pBuffer = *iter;
		if( pBuffer->m_aMyChar == NULL ) continue;
		if( pBuffer->m_pUI3DRender == NULL ) continue;

		if( pBuffer->m_dwCharIndex == dwPrevIndex )
		{
			pBuffer->m_dwCharIndex = dwNextIndex;
			break;
		}
	}
}

void SoldierSelectUIBeforeLoading::PushCharList( const CHARACTERDATA &rkCharacter, bool bThread )
{
	if( IsAlreadyChar( rkCharacter.m_dwIndex ) )
		return;

	UICharLoadingBuffer *pCharBuffer = new UICharLoadingBuffer;
	pCharBuffer->m_pUI3DRender = new ioUI3DRender;
	pCharBuffer->m_pUI3DRender->Initialize( "config/sp2_stage_charselect_map.ini", "my_soldier_cam" );

	char szBuf[MAX_PATH];
	wsprintf_e( szBuf, "MyChar%d", rkCharacter.m_dwIndex );
    pCharBuffer->m_aMyChar = pCharBuffer->m_pUI3DRender->CreateChar( rkCharacter.m_data, szBuf, bThread );
	if( !pCharBuffer->m_aMyChar ) 
	{
		LOG.PrintTimeAndLog( 0, "CharLoadingBuffer::PushCharList : NULL" );
		SAFEDELETE( pCharBuffer->m_aMyChar );
		SAFEDELETE( pCharBuffer->m_pUI3DRender );
		SAFEDELETE( pCharBuffer );
		return;
	}
	// Equip Item..
	// 코스튬 관련 (캐릭터 슬롯에 등록)
	for( int i=0 ; i< ioCreateChar::MAX_CCS ; i++ )
	{
		DWORD dwCostumeMaleCustom = rkCharacter.m_data.m_EquipCostume[i].m_Costume_Male_Custom;
		DWORD dwCostumeFemaleCustom = rkCharacter.m_data.m_EquipCostume[i].m_Costume_Female_Custom;

		if ( rkCharacter.m_data.m_EquipCostume[i].m_CostumeCode <= 0 )
		{
			dwCostumeMaleCustom = rkCharacter.m_equip_item[i].m_item_male_custom;
			dwCostumeFemaleCustom = rkCharacter.m_equip_item[i].m_item_female_custom;
		}
		pCharBuffer->m_aMyChar->EquipItem( rkCharacter.m_equip_item[i].m_item_code, dwCostumeMaleCustom, dwCostumeFemaleCustom, rkCharacter.m_data.m_EquipCostume[i].m_CostumeCode );
	}

	pCharBuffer->m_bAnimationPlay = true;
	if( !pCharBuffer->m_aMyChar->SetWeaponShopMotion() )
	{
		ioHashString szUIAnimation = pCharBuffer->m_pUI3DRender->GetModelAnimation( 0, pCharBuffer->m_aMyChar->IsMale() );
		if( !szUIAnimation.IsEmpty() )
		{
			pCharBuffer->m_aMyChar->SetLoopAni( szUIAnimation );
			for(int i = 0;i < 5000;i++)
				pCharBuffer->m_aMyChar->Update( i );
		}
	}

	pCharBuffer->m_dwCharIndex = rkCharacter.m_dwIndex;
	UpdateAwake( pCharBuffer,  rkCharacter );
	m_pCurrentCharBufferList->push_back( pCharBuffer );
}

void SoldierSelectUIBeforeLoading::EraseCharList( DWORD dwCharIndex )
{
	CharBufferList::iterator iter = m_pCurrentCharBufferList->begin();
	for(;iter != m_pCurrentCharBufferList->end();++iter)
	{
		UICharLoadingBuffer *pBuffer = *iter;
		if( pBuffer->m_dwCharIndex == dwCharIndex )
		{
			UICharLoadingBuffer *pBuffer = *iter;
			SAFEDELETE( pBuffer->m_aMyChar );
			SAFEDELETE( pBuffer->m_pUI3DRender );
			SAFEDELETE( pBuffer );
			m_pCurrentCharBufferList->erase( iter );
			return;
		}
	}
}

void SoldierSelectUIBeforeLoading::SetArenaBufferList( bool bArena )
{
	if( bArena )
		m_pCurrentCharBufferList = &m_ArenaCharBufferList;
	else
		m_pCurrentCharBufferList = &m_CharBufferList;
}

UICharLoadingBuffer *SoldierSelectUIBeforeLoading::GetUIChar( DWORD dwCharIndex )
{
	CharBufferList::iterator iter = m_pCurrentCharBufferList->begin();
	for(;iter != m_pCurrentCharBufferList->end();++iter)
	{
		UICharLoadingBuffer *pBuffer = *iter;
		if( pBuffer->m_dwCharIndex == dwCharIndex )
		{
			if( !pBuffer->m_bAnimationPlay )
			{
				pBuffer->m_bAnimationPlay = true;
				if( !pBuffer->m_aMyChar->SetWeaponShopMotion() )
				{
					ioHashString szUIAnimation = pBuffer->m_pUI3DRender->GetModelAnimation( 0, pBuffer->m_aMyChar->IsMale() );
					if( !szUIAnimation.IsEmpty() )
					{
						pBuffer->m_aMyChar->SetLoopAni( szUIAnimation );
					}
				}
			}

			return *iter;
		}
	}
	return NULL;
}
//-------------------------------------------------------------------------
MannerTrialInfo::MannerTrialInfo()
{
	Clear();
}

MannerTrialInfo::~MannerTrialInfo()
{
	Clear();
}

void MannerTrialInfo::UpdateDate()
{
	time_t kCurServerTime = g_MyInfo.GetServerDate();
	kCurServerTime += ( REALGETTIME()/1000 );// 현재까지의 클라이언트초를 넣는다.

	struct tm *pDate = localtime(&kCurServerTime);
	if( !pDate )
	{
		m_dwTrailDate = 19000101;
		return;
	}

	DWORD dwCurDate = ( (pDate->tm_year+1900)*10000) + ( ( pDate->tm_mon+1 )*100) + pDate->tm_mday;
	if( m_dwTrailDate != dwCurDate )
	{
		Clear();
		m_dwTrailDate = dwCurDate;
	}
}

void MannerTrialInfo::Clear()
{
	m_dwTrailDate = 0;
	m_vTrailPublicIDVec.clear();
}

void MannerTrialInfo::Load()
{
	if( !m_vTrailPublicIDVec.empty() )
		return;

	if( g_MyInfo.GetPublicID().IsEmpty() )
		return;

	DWORD dwCRC = 0;
	CCrc32Static::StringCrc32((LPCTSTR)g_MyInfo.GetPublicID().c_str(), dwCRC );

	if( dwCRC == 0 )
		dwCRC = g_MyInfo.GetPublicID().GetHashCode();

	char szFileName[MAX_PATH]="";
	StringCbPrintf_e( szFileName, sizeof( szFileName ), "info\\%u", dwCRC );

	char szLoadData[MAX_PATH*4]="";
	ZeroMemory( szLoadData, sizeof( szLoadData ) );

	int iFileSize = 0;
	FILE	*fp = NULL;
	fp = fopen(szFileName, "rb");
	if( fp != NULL )
	{	
		fseek( fp, 0, SEEK_END );
		iFileSize = ftell( fp );
		fseek( fp, 0, SEEK_SET );
		fread(szLoadData, sizeof(szLoadData), 1, fp);
		fclose(fp);
	}
	else
		return;

	if( iFileSize > MAX_PATH*4 )
		iFileSize = MAX_PATH*4;

	char szItem[MAX_PATH]="";
	ZeroMemory( szItem, sizeof( szItem ) );
	int  iItemCnt  = 0;
	int  iTokenCnt = 0;
	int  iIDCnt    = 0;
	char chEncrypt = 0;
	for (int i = 0; i < iFileSize ; i++)
	{
		if( i == 0 )
		{
			chEncrypt = szLoadData[i];
			continue;
		}
		szItem[iItemCnt] = szLoadData[i] ^ 0xFF ^ g_MyInfo.GetPublicID().At(iIDCnt) ^ chEncrypt;
		iIDCnt++;
		if( iIDCnt >= g_MyInfo.GetPublicID().Length() )
			iIDCnt = 0;
		iItemCnt++;
		if( iItemCnt >= MAX_PATH )
			break;
		if( szItem[iItemCnt-1] == '|' )
		{
			szItem[iItemCnt-1]=NULL;
			iItemCnt = 0;
			iTokenCnt++;

			if( iTokenCnt == 1 )
				m_dwTrailDate = atoi( szItem );
			else
			{
				if( m_vTrailPublicIDVec.size() < MAX_TRAIL )
				{
					ioHashString szTemp = szItem;
					m_vTrailPublicIDVec.push_back( szTemp );
				}
				else
					break;
			}
			ZeroMemory( szItem, sizeof( szItem ) );
		}
	}
}

void MannerTrialInfo::Save()
{
	if( m_vTrailPublicIDVec.empty() )
		return;

	if( g_MyInfo.GetPublicID().IsEmpty() )
		return;

	DWORD dwCRC = 0;
	CCrc32Static::StringCrc32((LPCTSTR)g_MyInfo.GetPublicID().c_str(), dwCRC );

	if( dwCRC == 0 )
		dwCRC = g_MyInfo.GetPublicID().GetHashCode();

	char szFileName[MAX_PATH]="";
	StringCbPrintf_e( szFileName, sizeof( szFileName ), "info\\%u", dwCRC );

	char chEncrypt = rand()%256;
	char szSaveData[MAX_PATH*4]="";
	ZeroMemory( szSaveData, sizeof( szSaveData ) );
	szSaveData[0] = chEncrypt;
	StringCbPrintf( &szSaveData[1], sizeof( szSaveData ), "%u|", m_dwTrailDate, chEncrypt );
	int iSize = m_vTrailPublicIDVec.size();
	for (int i = 0; i < iSize ; i++)
	{
		StringCbCat( szSaveData, sizeof( szSaveData ), m_vTrailPublicIDVec[i].c_str() );
		StringCbCat( szSaveData, sizeof( szSaveData ), "|" );
	}

	int iLen   = strlen( szSaveData );
	int iIDCnt = 0;
	for (int i = 0; i < iLen ; i++) 
	{
		if( i == 0 ) // 0번째는 암호화 키이므로 패스
			continue;
		szSaveData[i] = szSaveData[i] ^ 0xFF ^ g_MyInfo.GetPublicID().At(iIDCnt) ^ chEncrypt;
		iIDCnt++;
		if( iIDCnt >= g_MyInfo.GetPublicID().Length() )
			iIDCnt = 0;
	}

	FILE *fp = fopen( szFileName, "wb");
	if( fp != NULL )	
	{				
		fwrite(szSaveData, iLen, 1, fp);
		fclose(fp);
	}
}

void MannerTrialInfo::AddTrailInfo( const ioHashString &rszPublicID )
{
	if( m_vTrailPublicIDVec.size() < MAX_TRAIL )
		m_vTrailPublicIDVec.push_back( rszPublicID );
}

MannerTrialInfo::ReturnType MannerTrialInfo::CheckRight( IN const ioHashString &rszPublicID )
{
	int iSize = m_vTrailPublicIDVec.size();

	if( iSize >= MAX_TRAIL )
		return RT_ERROR_MAX;

	for (int i = 0; i < iSize ; i++)
	{
		if( rszPublicID == m_vTrailPublicIDVec[i] )
			return RT_ERROR_ID;
	}

	return RT_OK;
}
//-------------------------------------------------------------------------
template<> ioMyInfo* Singleton< ioMyInfo >::ms_Singleton = 0;

ioMyInfo::ioMyInfo()
{
	m_pCharData = &m_char_data;
	m_pCurrentCharInfoList = &m_vPlayCharInfoList;
	m_select_char_array = 0;
	m_iPrevReferenceCharacterDataType = REFERENCE_CHARACTER_DATA_NORMAL;

	m_iSealLevel = 10;
	m_iNextSealLevel = 20;

	m_fMaxRevivalPenalty = 50.0f;
	m_fRecoverPenaltyPerSec = FLOAT1;
	m_fRecoverPenaltyPlayPerSec = FLOAT1;

	m_Inventory.Initialize();
	m_ClassExpert.Initialize();
	m_Award.Initialize();
	m_UserRecord.Initialize();
	m_UserEtcItem.Initialize();
	m_UserFishItem.Initialize();
	m_UserMedalItem.Initialize();
	m_UserHeroData.Initialize();
	m_CharRentalData.Initialize();
	m_UserHeadquartersData.Initialize();
	m_Clover.Initialize();

	m_pBingo = new ioBingo;
	m_pBingo->Initialize();

	m_pPriateRullet = new ioPriateRullet;
	m_pPriateRenewalRullet = new ioPriateRenewalRullet;

	m_pMatchingGame = new ioMatchingGame;
	
	m_pDiceGame = new ioDiceGame;
	
	m_vFishingItemList.clear();
	m_iCurFishingBait = 0;
	m_iCurFishingRod = 0;

	m_dwRoomOutTimer = 0;
	m_iAbilityLevel  = 0;

	m_iPenaltyPeso = 1000;
	m_fPenaltyPesoRate = FLOAT1;
	m_bCurModeSafetyRoom = false;
	m_dwPCRoomAuthority = 0;
	m_bViewState = false;
	m_bReserveEnterTraining = false;
	m_bReserveEnterFighting = false;
	m_bExerciseEventSoldier = false;
	m_bFirstCameraFixedHelp = true;
	m_bReservePCRoomCheck   = false;

	m_iMyContributePer = 0;

	m_iTotalUserRegCount = 0;
	m_iGradeExpRateBeforeCon = 0;

	m_iServerDate = 0;
	m_iLocalDate  = 0;

	m_vReserveReleaseInfoList.clear();
	m_vReserveReleaseExMedalSlotList.clear();
	m_vReserveReleaseCostumeInfoList.clear();
	m_vReserveReleaseAccessoryInfoList.clear();

	m_iRefillCoinCurTime = 0;
	m_iLogOutLaterDay = m_iLogOutLaterSec = 0;
	m_dwRentalProcessTime = 0;
	m_dwOneMinuteTimeCheck = 0;

	m_iMileage = 0;
	m_bShutDownUser = false;

	m_dwLobbyLockType = 0;
	m_dwBattleLockType= 0;
	m_dwLobbyLockTypeMsgBoxID = 0;

	m_dwMacroNotifyTime = 0;

	m_PcRoomCharVec.clear();
	m_iExcercisePCRoomCharMax = 1;

	m_dwConnectAttendanceTime = 0;

	m_iEquipType = 0;
	m_iCharArray = 0;
	m_iEquipSlot = 0;
	m_iExtraItemIndex = 0;

	m_bTutorialMode = false;

	m_PreLogoutTime = 0;

	m_iRSoldierPossessionCount = 0;
	m_vSoldierSetCntVec.clear();

	m_bUserHeroEvent = false;

	m_iSuccessionMMRPoint = 0;
	m_iMaxSuccessionCnt = 0;
	m_iCurSuccessionCnt = 0;
	m_bSuccessionRevengeMode = false;

	m_TimeGateTime = CTime(0);
}

ioMyInfo::~ioMyInfo()
{
	m_char_data.clear();
	m_char_arena_data.clear();
	m_vCharTimeLimitMap.clear();

	for(PlayCharInfoList::iterator iter = m_vPlayCharInfoList.begin(); iter != m_vPlayCharInfoList.end(); ++iter)
	{
		PlayCharInfo *pInfo = (*iter);
	    SAFEDELETE( pInfo );
	}
	m_vPlayCharInfoList.clear();

	for(PlayCharInfoList::iterator iter = m_vArenaCharInfoList.begin(); iter != m_vArenaCharInfoList.end(); ++iter)
	{
		PlayCharInfo *pInfo = (*iter);
		SAFEDELETE( pInfo );
	}
	m_vArenaCharInfoList.clear();

	m_vFishingItemList.clear();
	
	m_vReserveReleaseInfoList.clear();
	m_vReserveReleaseExMedalSlotList.clear();
	m_vReserveReleaseCostumeInfoList.clear();
	m_vReserveReleaseAccessoryInfoList.clear();

	m_RentalTimeDataMap.clear();
	SAFEDELETE( m_pBingo );
	SAFEDELETE( m_pPriateRullet );
	SAFEDELETE( m_pPriateRenewalRullet );

	SAFEDELETE( m_pMatchingGame );
	SAFEDELETE( m_pDiceGame );
}

ioMyInfo& ioMyInfo::GetSingleton()
{
	return Singleton< ioMyInfo >::GetSingleton();
}

bool ioMyInfo::IsHost() const
{
	if( GetPublicID().IsEmpty() || m_HostName.IsEmpty() )
		return false;

	if( GetPublicID() == m_HostName )
		return true;

	return false;
}

void ioMyInfo::SetUserState( int iState )
{ 
	bool bTutorialUser = IsTutorialUser();

#if defined( USE_GA )
	int iBeforeState = m_user_data.m_user_state;
#endif

	m_user_data.m_user_state = iState;

#if defined( USE_GA )	
	if( g_HttpMng.GetTutorialStart() == true )
	{
		// 듀토리얼 종료된 유저 state 값
		if( iState == US_TUTORIAL_CLEAR )
		{
			// ACCOUNT_TUTORIAL_END
			g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FACCOUNT%2FTUTORIAL%2FEND" );

			g_HttpMng.SetTutorialStart( false );
		}
	}
	else
	{
		// 듀토리얼 시작
		if( iState != US_TUTORIAL_CLEAR )
		{
			g_HttpMng.SetTutorialStart( true );

			// ACCOUNT_TUTORIAL_START
			g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FACCOUNT%2FTUTORIAL%2FSTART" );
		}		

		// 듀토리얼 중간에 강제 종료등으로 인해 iState : US_TUTORIAL_CLEAR 값으로 내려오고,
		// 그 전 iState 값이 1204 ( 듀토리얼 종료 직전 스탭 ) 인 경우.
		if( iState == US_TUTORIAL_CLEAR && iBeforeState == 1204 )
		{
			// ACCOUNT_TUTORIAL_START
			g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FACCOUNT%2FTUTORIAL%2FSTART" );
			// ACCOUNT_TUTORIAL_END
			g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FACCOUNT%2FTUTORIAL%2FEND" );
		}
	}
#endif
}

bool ioMyInfo::IsTutorialUser()
{
	if( m_user_data.m_user_state == US_TUTORIAL_CLEAR ) 
		return false;

	return true;
}

bool ioMyInfo::IsBankruptcy()
{
	int iResult = 0;
	for( int i=0 ; i < GetCharCount() ; i++ )
	{
		if( m_pCharData->at(i).m_data.m_chExerciseStyle != EXERCISE_NONE )
			continue;
		if( !m_pCharData->at(i).m_data.m_bActive )	
			continue;

		iResult++;
	}

	if( iResult != 0 ) 
		return false;

	if( GetMoney() < g_ClassPrice.GetBankruptcyPeso() )
		return true;

	return false;
}

bool ioMyInfo::IsCharBankruptcy()
{
	int iResult = 0;
	for( int i=0 ; i < GetCharCount() ; i++ )
	{
		if( m_pCharData->at(i).m_data.m_chExerciseStyle != EXERCISE_NONE )
			continue;
		if( !m_pCharData->at(i).m_data.m_bActive )
			continue;

		iResult++;
	}

	if( iResult != 0 ) 
		return false;
	
	if( GetMoney() < g_ClassPrice.GetBankruptcyPeso() )
		return false;

	return true;
}

void ioMyInfo::SetMyAddress( const ioHashString &szIP, int iPort )
{
	m_MyIP = szIP;
	m_iMyPort = iPort;
}

bool ioMyInfo::IsSameMyIP( const ioHashString &szCheckIP )
{
	return ( strcmp( GetMyIP().c_str(), szCheckIP.c_str() ) == 0 );
}

void ioMyInfo::SetPrivateID( ioHashString szPrivateID )
{
	m_user_data.m_private_id  = szPrivateID;
}

void ioMyInfo::SetPublicID( ioHashString szPublicID )
{
	m_user_data.m_public_id = szPublicID;
}

void ioMyInfo::SetMoney( __int64 iMoney )
{
	m_user_data.m_money = iMoney;
}

void ioMyInfo::SetCash( int iCash )
{
	m_user_data.m_cash = iCash;
}

void ioMyInfo::SetPurchasedCash( int iPurchasedCash )
{
	m_user_data.m_purchased_cash = iPurchasedCash;
}

void ioMyInfo::SetChannelingCash( int iCash )
{
	m_user_data.m_iChannelingCash = iCash;
}

void ioMyInfo::SetLadderPoint( int iLadderPoint )
{
	m_user_data.m_ladder_point = iLadderPoint;
	g_UserInfoMgr.SetLadderPoint( GetPublicID(), iLadderPoint );
}

void ioMyInfo::SetAccrueHeroExpert( int iAccrueHeroExp )
{
	m_user_data.m_iAccrueHeroExpert = iAccrueHeroExp;
}

void ioMyInfo::SetHeroExpert( int iHeroExp )
{
	m_user_data.m_iHeroExpert = iHeroExp;
}

void ioMyInfo::SetUserCampPos( int iCampPosition )
{
	m_user_data.m_camp_position = iCampPosition;
	g_UserInfoMgr.SetCampPosition( GetPublicID(), iCampPosition );
	
	if( iCampPosition != CAMP_NONE )
	{
		g_QuestMgr.QuestCompleteTerm( QC_CAMP_JOIN );
	}
}

void ioMyInfo::SetFirstSolderBonus()
{
	if( GetUserState() != 0 )
		return;

	int iFirstClassType = 0;
	ioINILoader_e kLoader( "config/sp2_leader_item.ini" );
	kLoader.SetTitle_e( "LEADER_CREATE_INFO" );
	int max_item = kLoader.LoadInt_e( "max_item", 0 );
	char szKeyName[MAX_PATH];
	for(int i = 0;i < max_item;i++)
	{
		sprintf_e( szKeyName, "item_code_%d", i + 1);
		int iClassType = kLoader.LoadInt( szKeyName, 0 );
		DWORD dwSetItemCode = iClassType + SET_ITEM_CODE;
		const ioSetItemInfo *pSetItemInfo = g_SetItemInfoMgr.GetSetInfoByCode( dwSetItemCode );
		if( IsCanBuyItem( pSetItemInfo ) && !g_ClassPrice.IsCashOnly( iClassType ) )
		{
			iFirstClassType = iClassType;
			break;
		}
	}		

	if( iFirstClassType == 0 )
		return;

	if( GetClassArray( iFirstClassType ) != -1 )
		return;

	SP2Packet kPacket( CTPK_CHAR_CREATE );
	kPacket << CHAR_CREATE_FIRST_HIRE;
	kPacket << iFirstClassType;
	kPacket << 1;      // 종족 인간
	kPacket	<< 1;      // 성별 남자
	kPacket	<< g_ClassPrice.GetFirstHireLimit();
	kPacket	<< 0;      // iPrice
	kPacket << false;  // bCash
	kPacket << CPT_TIME;
	TCPNetwork::SendToServer( kPacket );
	TCPNetwork::MouseBusy( true );
}

void ioMyInfo::SetKickedInfo( const char *szSubInfo )
{
	m_szKickSubInfo = szSubInfo;
}

void ioMyInfo::ClearKickedInfo()
{
	m_szKickSubInfo.Clear();
}

void ioMyInfo::SetVoteKickReason( const ioHashString &rkReason )
{
	m_szReserveKickReason = rkReason;
}

bool ioMyInfo::IsGeneralGrade()
{
	if( COMPARE( GetGradeLevel(), 50, 55 ) )
		return true;

	return false;
}

int ioMyInfo::GetGradeLevel()
{
	return m_user_data.m_grade_level;
}

int ioMyInfo::GetGradeExpert()
{
	return m_user_data.m_grade_exp;
}

int ioMyInfo::GetClassLevel( int iClassType, bool bOriginal )
{
	if( !bOriginal )
	{
		DWORD dwCharIndex = GetCharIndexByClassType( iClassType );
		if( IsCharExerciseStyle( GetCharArray( dwCharIndex ), EXERCISE_RENTAL ) )
		{
			if( IsCharExerciseRentalInActive( GetCharArray( dwCharIndex ) ) == false )
				return m_CharRentalData.GetClassLevel( dwCharIndex );
		}
	}

	return m_ClassExpert.GetClassLevel( iClassType ) ;
}

int ioMyInfo::GetExpRate( int iClassType )
{
	int iClassLevel = m_ClassExpert.GetClassLevel( iClassType );
	int iClassExp   = m_ClassExpert.GetClassExpert( iClassType );

	int iNeedExp = g_LevelMgr.GetNextLevelupExp( iClassLevel );
	if( iNeedExp > 0 )
	{
		float fRate = (float)iClassExp / (float)iNeedExp;		
		return min( 99, (int)( fRate * FLOAT100 ) );
	}
	return 0;
}

int ioMyInfo::GetClassExpert( int iClassType )
{
	return m_ClassExpert.GetClassExpert( iClassType );
}

const ioClassExpert::CLASSSLOT* ioMyInfo::GetClassExpertSlot( int iClassType )
{
	return m_ClassExpert.GetClassExpertSlot( iClassType );
}

int ioMyInfo::GetGradeExpRate()
{
	if( IsGeneralGrade() ) 
		return m_user_data.m_grade_exp;  // 장성급 유저는 계급 경험치를 그대로 return 한다.

	int iNeedExp = g_LevelMgr.GetNextGradeupExp( GetGradeLevel() );
	if( iNeedExp > 0 )
	{
		float fRate = (float)GetGradeExpert() / (float)iNeedExp;
		return min( 99, (int)( fRate * FLOAT100 ) );
	}
	return 0;
}

int ioMyInfo::GetClassType( int iCharArray )
{
	if( !COMPARE( iCharArray, 0, GetCharCount() ) ) 
		return -1;
	return m_pCharData->at(iCharArray).m_data.m_class_type;
}

int ioMyInfo::GetClassType( DWORD dwIndex )
{
	for(int i = 0;i < GetCharCount();i++)
	{
		if( m_pCharData->at(i).m_dwIndex == dwIndex )
		{
			return GetClassType( i );
		}		
	}
	return -1;
}

int ioMyInfo::GetClassType()
{
	if( !COMPARE( m_select_char_array, 0, GetCharCount() )  )
		return -1;
	return m_pCharData->at(m_select_char_array).m_data.m_class_type;
}

int ioMyInfo::GetTopLevelClassType()
{
	return m_ClassExpert.GetTopLevelClassType();
}

const char* ioMyInfo::GetClassName( int iClassType, int eCharPowerUpType /* = PUGT_NONE */ )
{
	const ioSetItemInfo *pSetInfo = g_SetItemInfoMgr.GetSetInfoByIdx( iClassType-1 );
	if( pSetInfo )
	{
		if( eCharPowerUpType != PUGT_NONE )
		{
			ioHashString szGradeName = g_PowerUpManager.GetCharGradeTitle( iClassType, eCharPowerUpType );
			if( !szGradeName.IsEmpty() )
			{
				static char szBuf[MAX_PATH];
				sprintf_s( szBuf, "%s %s", szGradeName.c_str(), pSetInfo->GetName().c_str() );
				return szBuf;
			}
		}

		return pSetInfo->GetName().c_str();
	}

	return "None";
}

void ioMyInfo::SetGrade( int iGradeLevel, int iExp )
{
	if( m_user_data.m_grade_level != iGradeLevel )
	{
#if defined( USE_GA )
		char chLabel[32] = {0,};

		if ( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
			sprintf_e( chLabel, "%d", iGradeLevel );
		else
			SafeSprintf( chLabel, sizeof(chLabel), "%1", iGradeLevel );

		// ACCOUNT_LV_UP
		g_HttpMng.GA_EventTracking( g_MyInfo.GetUserIndex()
			, "Account"
			, "LevelUp"
			, chLabel
			, 1
			, "%2FACCOUNT%2FLEVEL_UP" );
#endif
	}
	else
	{
#if defined( USE_GA )
		char chLabel[32] = {0,};

		if ( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
			sprintf_e( chLabel, "%d", iExp );
		else
			SafeSprintf( chLabel, sizeof(chLabel), "%1", iExp );

		// ACCOUNT_GET_EXP
		g_HttpMng.GA_EventTracking( g_MyInfo.GetUserIndex()
			, "Account"
			, "GetExp"
			, chLabel
			, 1
			, "%2FACCOUNT%2FGET_EXP" );
#endif
	}

	m_user_data.m_grade_level = iGradeLevel;
	m_user_data.m_grade_exp   = iExp;

	if( g_BattleRoomMgr.IsBattleRoom() )
	{
		g_BattleRoomMgr.UpdateUserLevel( GetPublicID(), GetGradeLevel() );
	}
	else if( g_LadderTeamMgr.IsLadderTeam() )
	{
		g_LadderTeamMgr.UpdateUserData( GetPublicID(), GetGradeLevel(), 0 );
	}
	else if( g_ShuffleRoomMgr.IsShuffleRoom() )
	{
		g_ShuffleRoomMgr.UpdateUserLevel( GetPublicID(), GetGradeLevel() );
	}
	g_UserInfoMgr.SetGrade( GetPublicID(), GetGradeLevel(), GetGradeExpRate() );
	g_QuestMgr.QuestCompleteTerm( QC_GRADE_UP );
	g_QuestMgr.QuestOccurTerm( QO_GRADE_EXP_ACQUIRE );
}

void ioMyInfo::SetChangeChar( const DWORD dwIndex )
{
	for(int i = 0;i < GetCharCount();i++)
	{
		if( m_pCharData->at(i).m_dwIndex == dwIndex )
		{
			m_select_char_array = i;
			break;
		}		
	}
}

void ioMyInfo::SetUserData( IN SP2Packet &rkPacket ,OUT DWORD &dwPublicIDHashCode )
{
	ioHashString szPublicID;
	rkPacket >> m_user_data.m_user_idx >> szPublicID;

	            // 메모리 조작을 방지하기 위해서 지역 변수에 할당해서 체크 값에 넣어준다.
				dwPublicIDHashCode = szPublicID.GetHashCode();
	            m_user_data.m_public_id = szPublicID;

	rkPacket >> m_user_data.m_cash
			 >> m_user_data.m_money >> m_user_data.m_connect_count
			 >> m_user_data.m_user_state >> m_user_data.m_grade_level >> m_user_data.m_grade_exp
			 >> m_user_data.m_fishing_level >> m_user_data.m_fishing_exp >> m_user_data.m_refill_data
			 >> m_user_data.m_user_event_type >> m_user_data.m_user_rank >> m_user_data.m_connect_date;

	int iEntryType;
	rkPacket >> iEntryType;
	m_user_data.m_eEntryType = (EntryType)iEntryType;
	rkPacket >> m_user_data.m_camp_position >> m_user_data.m_ladder_point >> m_user_data.m_camp_rank;

	// channeling
	int iChannelingType;
	rkPacket >> iChannelingType >> m_user_data.m_szChannelingUserID >> m_user_data.m_szChannelingUserNo >> m_user_data.m_iChannelingCash >> m_user_data.m_purchased_cash;
	m_user_data.m_eChannelingType = (ChannelingType)iChannelingType;

	// 차단
	int iBlockType;
	rkPacket >> iBlockType >> m_user_data.m_dwBlockYearMonthDay >> m_user_data.m_dwBlockHourMinute;
	m_user_data.m_eBlockType = (BlockType)iBlockType;

	// excavation
	rkPacket >> m_user_data.m_iExcavationLevel >> m_user_data.m_iExcavationExp;

	// HeroMatch
	rkPacket >> m_user_data.m_iAccrueHeroExpert >> m_user_data.m_iHeroExpert;

	// 상대계급 정보
	rkPacket >> m_user_relative_grade_data.m_init_code >> m_user_relative_grade_data.m_enable_reward;

	// date
	int iServerDate = 0;
	rkPacket >> iServerDate >> m_iLogOutLaterDay >> m_iLogOutLaterSec;  
	m_iServerDate = iServerDate;
	time( &m_iLocalDate );
	
	__int64 iPreLogOutTime;
	rkPacket >> iPreLogOutTime;
	m_PreLogoutTime = CTime( iPreLogOutTime );
	
	//수련장 임시 추가

	rkPacket >> m_szGuid;

	// Default Select Char 
	rkPacket >> m_select_char_array;

	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal )
		pLocal->SetTimeZone();

	m_iRefillCoinCurTime = FRAMEGETTIME();

	m_GrowthLevel.SetOwnerName( GetPublicID() );        

	g_FriendManager.SendFriendList();
	g_FriendManager.SendFriendRequestList();
	g_MemoManager.SendOfflineMemo();

	m_AnnounceGameAddiction.SetTime( REALGETTIME() );

#ifndef SHIPPING
	struct tm *pDate = localtime(&m_iServerDate);
	if( pDate )
		LOG.PrintTimeAndLog( 0, "Server Date : %d-%d-%d %d:%d : BlockType : %d (%d %d)", pDate->tm_year+1900, pDate->tm_mon+1, pDate->tm_mday, pDate->tm_hour, pDate->tm_min, iBlockType, m_user_data.m_dwBlockYearMonthDay, m_user_data.m_dwBlockHourMinute );
#endif
}

void ioMyInfo::SetUserHeroData( SP2Packet &rkPacket )
{
	rkPacket >> m_UserHeroData.m_iHeroTitle >> m_UserHeroData.m_iHeroTodayRank;
	for(int i = 0;i < HERO_SEASON_RANK_MAX;i++)
		rkPacket >> m_UserHeroData.m_iHeroSeasonRank[i];
}

bool ioMyInfo::IsCharEquipItem( int eid )
{
	if( !COMPARE( eid, 0, GetCharCount() ) )
		return false;
	
	return false;
}

bool ioMyInfo::IsCharRentalToClassType( int iClassType )
{
	for( int i=0 ; i<GetCharCount() ; i++ )
	{
		if( m_pCharData->at(i).m_data.m_class_type == iClassType )
		{
			if( m_pCharData->at(i).m_data.m_sRentalType == CRT_RENTAL )
				return true;
			return false;
		}
	}
	return false;
}

DWORD ioMyInfo::GetCharRentalMinuteToArray( int eid )
{
	if( !COMPARE( eid, 0, GetCharCount() ) ) return 0;

	if( m_pCharData->at(eid).m_data.m_sRentalType != CRT_RENTAL && 
		m_pCharData->at(eid).m_data.m_chExerciseStyle != EXERCISE_RENTAL )
	{
		return 0;
	}
	return m_pCharData->at(eid).m_data.m_dwRentalMinute;
}

bool ioMyInfo::IsCharRentalToExtraItem( int iSlotIndex )
{
	for(int i = 0;i < GetCharCount();i++)
	{
		CHARACTER &rkCharInfo = m_pCharData->at(i).m_data;
		for(int k = 0;k < MAX_INVENTORY;k++)
		{
			if( rkCharInfo.m_extra_item[k] == iSlotIndex )
			{
				if( rkCharInfo.m_sRentalType == CRT_RENTAL )
					return true;
				return false;
			}
		}
	}
	return false;
}

void ioMyInfo::SetCharDied( int iArray, bool bDie )
{
	if( COMPARE( iArray, 0, (int)m_pCurrentCharInfoList->size() ) )
	{
		if( !m_pCurrentCharInfoList->at(iArray) )
			return;
		m_pCurrentCharInfoList->at(iArray)->m_bCharDied = bDie;

		if( bDie )
		{
			m_pCurrentCharInfoList->at(iArray)->m_fRevivalPenalty = m_fMaxRevivalPenalty;
		}
		else
		{
			m_pCurrentCharInfoList->at(iArray)->m_fRevivalPenalty = FLOAT1;
		}
	}
}

void ioMyInfo::SetCharDiedPenalty( int iArray, bool bDie )
{
	if( COMPARE( iArray, 0, (int)m_pCurrentCharInfoList->size() ) )
	{
		if( !m_pCurrentCharInfoList->at(iArray) )
			return;
		m_pCurrentCharInfoList->at(iArray)->m_bCharDiedPenalty = bDie;
	}
}

void ioMyInfo::ClearCharDiedInfo()
{
	int iSize = m_pCurrentCharInfoList->size();
	for( int i=0; i < iSize; i++ )
	{
		if( !m_pCurrentCharInfoList->at(i) )
			continue;
		m_pCurrentCharInfoList->at(i)->m_fRevivalPenalty = FLOAT1;
		m_pCurrentCharInfoList->at(i)->m_bCharDied = false;
	}
}

void ioMyInfo::ClearCharDiedPenaltyInfo()
{
	int iSize = m_pCurrentCharInfoList->size();
	for( int i=0; i < iSize; i++ )
	{
		if( !m_pCurrentCharInfoList->at(i) )
			continue;
		m_pCurrentCharInfoList->at(i)->m_bCharDiedPenalty = false;
	}
}

bool ioMyInfo::IsCharDied( int iArray ) const
{
	if( COMPARE( iArray, 0, (int)m_pCurrentCharInfoList->size() ) )
	{
		if( !m_pCurrentCharInfoList->at(iArray) )
			return true;
		return m_pCurrentCharInfoList->at(iArray)->m_bCharDied;
	}

	return true;
}

bool ioMyInfo::IsCharDiedPenalty( int iArray ) const
{
	if( COMPARE( iArray, 0, (int)m_pCurrentCharInfoList->size() ) )
	{
		if( !m_pCurrentCharInfoList->at(iArray) )
			return true;

		return m_pCurrentCharInfoList->at(iArray)->m_bCharDiedPenalty;
	}

	return true;
}

void ioMyInfo::SetCharJoined( int iArray, bool bJoin )
{
	if( COMPARE( iArray, 0, (int)m_pCurrentCharInfoList->size() ) )
	{
		if( !m_pCurrentCharInfoList->at(iArray) )
			return;
		m_pCurrentCharInfoList->at(iArray)->m_bCharJoined = bJoin;
	}
}

void ioMyInfo::SetCharJoined( const DWORD dwIndex, bool bJoin )
{
	for( int i=0 ; i<GetCharCount() ; i++ )
	{
		if( m_pCharData->at(i).m_dwIndex == dwIndex )
		{
			SetCharJoined( i, bJoin );			
			break;
		}
	}
}

void ioMyInfo::ClearCharJoinedInfo()
{
	int iSize = m_pCurrentCharInfoList->size();
	for( int i=0; i < iSize; i++ )
	{
		if( !m_pCurrentCharInfoList->at(i) )
			continue;
		m_pCurrentCharInfoList->at(i)->m_bCharJoined = false;
	}
}

bool ioMyInfo::IsCharJoined( int iArray ) const
{
	if( COMPARE( iArray, 0, (int)m_pCurrentCharInfoList->size() ) )
	{
		if( !m_pCurrentCharInfoList->at(iArray) )
			return true;
		return m_pCurrentCharInfoList->at(iArray)->m_bCharJoined;
	}

	return true;
}

void ioMyInfo::RecoverRevivalPenalty( bool bPlay )
{
	float fRecoveredPenalty = 0.0f;
	int iSize = m_pCurrentCharInfoList->size();
	for( int i=0; i < iSize; i++ )
	{
		if( !m_pCurrentCharInfoList->at(i) )
			continue;
		if( m_pCurrentCharInfoList->at(i)->m_bCharDied )
		{
			fRecoveredPenalty = m_pCurrentCharInfoList->at(i)->m_fRevivalPenalty;

			if( bPlay )
				fRecoveredPenalty += m_fRecoverPenaltyPlayPerSec;
			else
				fRecoveredPenalty += m_fRecoverPenaltyPerSec;

			m_pCurrentCharInfoList->at(i)->m_fRevivalPenalty = min( fRecoveredPenalty, FLOAT1 );
		}
	}
}

float ioMyInfo::GetRevivalPenalty( int iArray ) const
{
	if( COMPARE( iArray, 0, (int)m_pCurrentCharInfoList->size() ) )
	{
		if( !m_pCurrentCharInfoList->at(iArray) )
			return 0.0f;

		if( m_pCurrentCharInfoList->at(iArray)->m_bCharDied )
			return m_pCurrentCharInfoList->at(iArray)->m_fRevivalPenalty;
		else
			return FLOAT1;
	}

	return 0.0f;
}

int ioMyInfo::GetHeroSeasonRank( int iSeason )
{
	if( COMPARE( iSeason, HERO_SEASON_RANK_1, HERO_SEASON_RANK_MAX ) )
		return m_UserHeroData.m_iHeroSeasonRank[iSeason];
	return 0;
}

bool ioMyInfo::IsSlotItem( int iType, int iCode )
{
	return m_Inventory.IsSlotItem( iType, iCode );
}

void ioMyInfo::AddSlotItem( int iType, int iCode )
{
	m_Inventory.AddSlotItem( iType, iCode );
}

bool ioMyInfo::RemoveSlotItem( int iType, int iCode )
{
	return m_Inventory.RemoveSlotItem( iType, iCode );
}

void ioMyInfo::SetEquipItem( int iType, int iCode )
{
	m_Inventory.SetEquipItem( iType, iCode );
}

void ioMyInfo::GetEquipItemCode( CHARACTER &charInfo )
{
	m_Inventory.GetEquipItemCode( charInfo );
}

int ioMyInfo::GetEquipItemCode( const int iItemType )
{
	return m_Inventory.GetEquipItemCode( iItemType );
}

bool ioMyInfo::IsClassTypeMale( int iClassType )
{
	RaceDetailType eRaceType = m_Inventory.GetEquipRaceType( iClassType );
	// 여성 캐릭터 타입
	switch( eRaceType )
	{
	case RDT_HUMAN_WOMAN:
	case RDT_ELF_WOMAN:
	case RDT_DWARF_WOMAN:
		return false;
	}

	return true;
}

bool ioMyInfo::IsClassTypeInventory( int iClassType )
{
	return m_Inventory.IsClassTypeInventory( iClassType );
}

bool ioMyInfo::IsClassTypeInventory( int iClassType, int iSexType, int iDecoType )
{
	return m_Inventory.IsClassTypeInventory( iClassType, iSexType, iDecoType );
}

void ioMyInfo::ApplyRoundEndInfo( SP2Packet &rkPacket )
{
	int iRoomStyle, iModeType, iWin, iLose, iKill, iDeath;
	rkPacket >> iRoomStyle >> iModeType >> iWin >> iLose >> iKill >> iDeath >> m_user_data.m_ladder_point;

	switch( iRoomStyle )
	{
	case RSTYLE_BATTLEROOM:
	case RSTYLE_SHUFFLEROOM:
	case RSTYLE_MATCHROOM:
		{
			int iPrevWin = m_UserRecord.GetRecordWin( RECORD_BATTLE );
			if( iPrevWin != -1 )
			{
				if( iWin > iPrevWin )
				{
					g_QuestMgr.QuestCompleteTerm( QC_BATTLE_PVP_WIN, max( 0, iWin - iPrevWin ), ioPlayMode::GetModeType(), g_VictoriesMgr.GetVictories( GetPublicID() ) );
				}
			}

			m_UserRecord.SetRecordInfo( RECORD_BATTLE, iWin, iLose, iKill, iDeath );	
			g_UserInfoMgr.SetPartyExp( GetPublicID(), GetPartyExp() );
			g_QuestMgr.QuestCompleteTerm( QC_BATTLE_EXP_ACQUIRE );
		}
		break;
	case RSTYLE_LADDERBATTLE:
		{
			int iPrevWin = 0;
			if( iModeType == MT_HEROMATCH )
				iPrevWin = m_UserRecord.GetRecordWin( RECORD_HEROMATCH );
			else
				iPrevWin = m_UserRecord.GetRecordWin( RECORD_LADDER );
			if( iPrevWin != -1 )
			{
				if( iWin > iPrevWin )
				{
					g_QuestMgr.QuestCompleteTerm( QC_CAMP_BATTLE_WIN, max( 0, iWin - iPrevWin ), ioPlayMode::GetModeType(), g_VictoriesMgr.GetVictories( GetPublicID() ) );
				}
			}
			if( iModeType == MT_HEROMATCH )
			{
				m_UserRecord.SetRecordInfo( RECORD_HEROMATCH, iWin, iLose, iKill, iDeath );					
				
				// 영웅전은 시즌 정보도 포함
				rkPacket >> iWin >> iLose >> iKill >> iDeath;
				m_UserRecord.SetHeroSeasonRecordInfo( iWin, iLose, iKill, iDeath );

				g_UserInfoMgr.SetHeroWin( GetPublicID(), GetRecordWin( RECORD_HEROMATCH ) );
				g_UserInfoMgr.SetHeroLose( GetPublicID(), GetRecordLose( RECORD_HEROMATCH ) );
			}
			else
			{
				m_UserRecord.SetRecordInfo( RECORD_LADDER, iWin, iLose, iKill, iDeath );	
				g_UserInfoMgr.SetLadderExp( GetPublicID(), GetLadderExp() );
			}
			g_QuestMgr.QuestCompleteTerm( QC_CAMP_EXP_ACQUIRE );
		}
		break;
	}
}

const CHARACTERDATA* ioMyInfo::GetCharacterData()
{	
	if( COMPARE( m_select_char_array, 0, GetCharCount() ) )
		return &m_pCharData->at(m_select_char_array);

	return NULL;
}

const CHARACTERDATA* ioMyInfo::GetCharacterData( int eid )
{
	if( COMPARE( eid, 0, GetCharCount() ) )
		return &m_pCharData->at(eid);

	return NULL;
}

const CHARACTERDATA* ioMyInfo::GetCharacterDataByIndex( DWORD dwIdx )
{
	for( int i=0 ; i< GetCharCount() ; i++ )
	{
		if( m_pCharData->at(i).m_dwIndex == dwIdx )
		{
			return &m_pCharData->at(i);
		}
	}

	return NULL;
}

const CHARACTERDATA* ioMyInfo::GetCharacterDataByClassType( int iClassType )
{
	for( int i=0 ; i< GetCharCount() ; i++ )
	{
		if( m_pCharData->at(i).m_data.m_class_type == iClassType )
		{
			return &m_pCharData->at(i);
		}
	}

	return NULL;
}

const CHARACTER& ioMyInfo::GetCharacter()
{
	static CHARACTER stChar;
	if( !COMPARE( m_select_char_array, 0, GetCharCount() ) ) return stChar;

	return m_pCharData->at(m_select_char_array).m_data;
}

const CHARACTER& ioMyInfo::GetCharacter(DWORD eid)
{
	static CHARACTER stChar;
	if( !COMPARE( eid, 0, (DWORD)GetCharCount() ) ) return stChar;

	return m_pCharData->at(eid).m_data;
}

const CHARACTER& ioMyInfo::GetCharacterToClassType(int iClassType)
{
	for( int i=0 ; i<GetCharCount() ; i++ )
	{
		if( m_pCharData->at(i).m_data.m_class_type == iClassType )
		{
			return m_pCharData->at(i).m_data;
		}
	}

	static CHARACTER stChar;
	return stChar;
}

void ioMyInfo::CopyCharacter(int eid, CHARACTER *charInfo)
{
	if( !COMPARE( eid, 0, GetCharCount() ) ) return ;

	*charInfo = m_pCharData->at(eid).m_data;
}

bool ioMyInfo::IsOriginalCharacter()
{
	bool bIsOriginal = false;

	if( m_pCharData == &m_char_data )
		bIsOriginal = true;
	else if( m_pCharData == &m_char_arena_data )
		bIsOriginal = false;

	return bIsOriginal;
}

bool ioMyInfo::IsCharUse( int iIdx )
{
	if( COMPARE( iIdx, 0, GetCharCount() ) )
		return true;
	return false;
}

bool ioMyInfo::IsItemSlotUse( int iIdx )
{
	return false;
}

const ITEM_DATA& ioMyInfo::GetCharItem( int index )
{
	static ITEM_DATA stItem;
	if( !COMPARE( m_select_char_array, 0, GetCharCount() ) ) return stItem;

	return m_pCharData->at(m_select_char_array).m_equip_item[index];
}

const ITEM_DATA& ioMyInfo::GetCharItem( int eid, int index )
{
	static ITEM_DATA stItem;
	if( !COMPARE( eid, 0, GetCharCount() ) ) return stItem;

	return m_pCharData->at(eid).m_equip_item[index];
}

const ITEM_DATA& ioMyInfo::GetCharItemToClassType( int iClassType, int index )
{
	static ITEM_DATA stItem;
	if( !COMPARE( index, 0, MAX_INVENTORY ) )
		return stItem;

	for( int i=0; i < GetCharCount(); i++ )
	{
		if( m_pCharData->at(i).m_data.m_class_type == iClassType )
			return m_pCharData->at(i).m_equip_item[index];
	}

	return stItem;
}

ITEM_DATA& ioMyInfo::GetCharWeaponItem()
{
	static ITEM_DATA stItem;
	if( !COMPARE( m_select_char_array, 0, GetCharCount() ) ) return stItem;

	return m_pCharData->at(m_select_char_array).m_equip_item[0];
}

ITEM_DATA& ioMyInfo::GetCharItemData( int index )
{
	static ITEM_DATA stItem;
	if( !COMPARE( m_select_char_array, 0, GetCharCount() ) ) return stItem;

	return m_pCharData->at(m_select_char_array).m_equip_item[index];
}

void ioMyInfo::InitCharWeaponItem()
{
	for( int iCharArray=0; iCharArray < GetCharCount(); iCharArray++ )
	{
		for( int i=0 ; i<MAX_INVENTORY ; i++ )
		{
			m_pCharData->at(iCharArray).m_equip_item[i].m_item_bullet = -1;
		}
	}
}

void ioMyInfo::StartCharLimitDate( DWORD dwIndex, DWORD dwCheckSecond )
{
	CharTimeLimitMap::iterator iter = m_vCharTimeLimitMap.find( dwIndex );
	if( iter != m_vCharTimeLimitMap.end() )
	{
		CharTimeLimit &kTimeLimit = iter->second;
		kTimeLimit.m_dwLimitTimer = FRAMEGETTIME();
		kTimeLimit.m_dwLimitSecond= dwCheckSecond;
	}
	else
	{
		CharTimeLimit kTimeLimit;
		kTimeLimit.m_dwLimitTimer = FRAMEGETTIME();
		kTimeLimit.m_dwLimitSecond= dwCheckSecond;
		m_vCharTimeLimitMap.insert( CharTimeLimitMap::value_type( dwIndex, kTimeLimit ) );
	}
}

void ioMyInfo::StopCharLimitDate()
{
	m_vCharTimeLimitMap.clear();
}

int ioMyInfo::GetCharTotalSecond( int iCharArray )
{
	DWORD dwCharIndex = GetCharIndex( iCharArray );
	if( dwCharIndex == 0 ) return 0;

	int iTotalSecond = m_pCharData->at(iCharArray).m_data.m_iLimitSecond;

	CharTimeLimitMap::iterator iter = m_vCharTimeLimitMap.find( dwCharIndex );
	if( iter == m_vCharTimeLimitMap.end() ) return iTotalSecond;
	if( m_pCharData->at(iCharArray).m_data.m_ePeriodType == CPT_MORTMAIN ) return 0;

	CharTimeLimit &kTimeLimit = iter->second;	

	if( kTimeLimit.m_dwLimitTimer != 0 && kTimeLimit.m_dwLimitSecond != 0 )
	{
		DWORD dwGapSecond = ( FRAMEGETTIME() - kTimeLimit.m_dwLimitTimer ) / kTimeLimit.m_dwLimitSecond;
		iTotalSecond -= dwGapSecond;
		if( iTotalSecond < 0 )
		{
			iTotalSecond = 0;
		}
	}
	return iTotalSecond;
}

void ioMyInfo::SetCharTotalSecond( int iCharArray, int iTotalSecond )
{
	if( COMPARE( iCharArray, 0, GetCharCount() ) )
		m_pCharData->at(iCharArray).m_data.m_iLimitSecond = iTotalSecond;
}

void ioMyInfo::SetCharTotalSecond( DWORD dwIndex, int iTotalSecond )
{
	for( int i=0 ; i<GetCharCount() ; i++ )
	{
		if( m_pCharData->at(i).m_dwIndex == dwIndex )
		{
			SetCharTotalSecond( i, iTotalSecond );
			break;
		}
	}
}

UICharLoadingBuffer *ioMyInfo::GetSoldierSelectUIChar( DWORD dwCharIndex )
{
	return m_SoldierSelectUIBeforeLoading.GetUIChar( dwCharIndex );
}	

void ioMyInfo::SetBeforeCharLoading( bool bThread )
{
	m_SoldierSelectUIBeforeLoading.ClearList();
	for( int i=0 ; i<GetCharCount() ; i++ )
	{
		m_SoldierSelectUIBeforeLoading.PushCharList( m_pCharData->at(i), bThread );
	}
}

void ioMyInfo::SetBeforeCharLoading_NormalCharacter( bool bThread )
{
	m_SoldierSelectUIBeforeLoading.SetArenaBufferList( false );
	SetBeforeCharLoading( bThread );
}

void ioMyInfo::SetBeforeCharLoading_ArenaCharacter( bool bThread )
{
	m_SoldierSelectUIBeforeLoading.SetArenaBufferList( true );
	SetBeforeCharLoading( bThread );
}

void ioMyInfo::ChangeReferenceCharacterData( int iType )
{
	if( !COMPARE( iType, MT_NONE, MAX_MODE_TYPE ) )
		return;

	int iReferenceType;
	if( iType == MT_ARENA )
		iReferenceType = REFERENCE_CHARACTER_DATA_ARENA;
	else
		iReferenceType = REFERENCE_CHARACTER_DATA_NORMAL;

	if( m_pCharData == &m_char_data )
		m_iPrevReferenceCharacterDataType = REFERENCE_CHARACTER_DATA_NORMAL;
	else if( m_pCharData == &m_char_arena_data )
		m_iPrevReferenceCharacterDataType = REFERENCE_CHARACTER_DATA_ARENA;

	if( iReferenceType == REFERENCE_CHARACTER_DATA_NORMAL )
		m_pCharData = &m_char_data;
	else if( iReferenceType == REFERENCE_CHARACTER_DATA_ARENA )
		m_pCharData = &m_char_arena_data;
	}

void ioMyInfo::RestoreReferenceCharacterData()
{
	if( m_iPrevReferenceCharacterDataType == REFERENCE_CHARACTER_DATA_NORMAL )
		ChangeReferenceCharacterData( MT_NONE );
	else if( m_iPrevReferenceCharacterDataType == REFERENCE_CHARACTER_DATA_ARENA )
		ChangeReferenceCharacterData( MT_ARENA );
}

void ioMyInfo::AddCharacter( const CHARACTERDATA &character )
{
	// ITEM SET
	m_char_data.push_back( character );	
	int iArray = m_char_data.size() - 1;

	if( character.m_data.m_chExerciseStyle == EXERCISE_RENTAL )
	{	
		m_CharRentalData.EquipItem( m_char_data[iArray] );
		m_CharRentalData.EquipCostume( m_char_data[iArray] );
	}
	else
	{
		m_UserExtraItem.EquipItem( m_char_data[iArray], true );
	}

	if( !g_App.IsFirstLoading() )
	{
		m_SoldierSelectUIBeforeLoading.SetArenaBufferList( false );
		m_SoldierSelectUIBeforeLoading.PushCharList( m_char_data[iArray], true );
	}

	PlayCharInfo *pInfo = new PlayCharInfo;
	m_vPlayCharInfoList.push_back( pInfo );	

	// 용병 획득 
	if( !GetPublicID().IsEmpty() )
	{
		g_QuestMgr.QuestCompleteTerm( QC_SOLDIER_ACQUIRE, character.m_data.m_class_type, (int)character.m_data.m_ePeriodType, (int)character.m_data.m_chExerciseStyle );

		// :: 체험 용병도 퀘스트를 받는다. (본부체험이벤트제외)
		if( character.m_data.m_chExerciseStyle != EXERCISE_EVENT )
			g_QuestMgr.QuestOccurTerm( QO_SOLDIER_ACQUIRE, character.m_data.m_class_type );
	}
	SerUserInfoCharData( character.m_data.m_class_type );
}

void ioMyInfo::AddArenaCharacter( DWORD dwType, int iPowerUp, int iFace, int iHair, int iSkinColor, int iHairColor, int iUnderWear )
{
	CHARACTERDATA	characterData;

	characterData.m_data.m_class_type	= dwType;
	characterData.m_data.m_face	= iFace;
	characterData.m_data.m_hair	= iHair;
	characterData.m_data.m_hair_color	= iHairColor;
	characterData.m_data.m_skin_color	= iSkinColor;
	characterData.m_data.m_underwear	= iUnderWear;
	characterData.m_data.m_eCharPowerUpType	= iPowerUp;
	characterData.m_data.m_iSlotIndex	= m_char_arena_data.size();
	characterData.m_data.m_ePeriodType	= CPT_MORTMAIN;

	characterData.m_dwIndex	= characterData.m_data.m_iSlotIndex + 1;

	m_UserExtraItem.EquipItem( characterData, true );

	m_char_arena_data.push_back( characterData );

	if( !g_App.IsFirstLoading() )
	{
		m_SoldierSelectUIBeforeLoading.SetArenaBufferList( true );
		m_SoldierSelectUIBeforeLoading.PushCharList( m_char_arena_data[m_char_arena_data.size() - 1], true );
	}

	PlayCharInfo *pInfo = new PlayCharInfo;
	m_vArenaCharInfoList.push_back( pInfo );
}

void ioMyInfo::SetCharacter( int iCharArray ,  const CHARACTERDATA &character )
{
	if( !COMPARE( iCharArray, 0, GetCharCount() ) ) return;

	if( character.m_data.m_chExerciseStyle != EXERCISE_RENTAL && 
		m_pCharData->at(iCharArray).m_data.m_chExerciseStyle == EXERCISE_RENTAL )
	{
		m_CharRentalData.DeleteRentalData( m_pCharData->at(iCharArray).m_dwIndex );
		RemoveRentalTimeData( m_pCharData->at(iCharArray).m_dwIndex );
	}

	m_pCharData->at(iCharArray) = character;

	if( character.m_data.m_chExerciseStyle == EXERCISE_RENTAL )
	{
		m_CharRentalData.EquipItem( m_pCharData->at(iCharArray) );
		m_CharRentalData.EquipCostume( m_pCharData->at(iCharArray) );
	}
	else
	{
		m_UserExtraItem.EquipItem( m_pCharData->at(iCharArray), true );
	}

	m_SoldierSelectUIBeforeLoading.Update( m_pCharData->at(iCharArray) );

	SerUserInfoCharData( character.m_data.m_class_type );
}

void ioMyInfo::SetCharChangePeriod( int iCharArray )
{
	if( !COMPARE( iCharArray, 0, GetCharCount() ) ) return;

	m_pCharData->at(iCharArray).m_data.m_bActive     = true;
	m_pCharData->at(iCharArray).m_data.m_ePeriodType = CPT_MORTMAIN;
	m_pCharData->at(iCharArray).m_data.m_iLimitSecond= 0;

	SerUserInfoCharData( m_pCharData->at(iCharArray).m_data.m_class_type );
}

void ioMyInfo::SetRefillCoinData( int iRefillData, int iRefillCount )
{
	m_user_data.m_refill_data = iRefillData;
	m_iRefillCoinCurTime = FRAMEGETTIME();
	if( iRefillCount > 0 )
	{
		AddEtcMonsterCoin( iRefillCount );
		g_ChatMgr.SetSystemMsg( STR(1), GetEtcMonsterCoin(), Help::GetRefillCoinMax() );
	}
}

void ioMyInfo::AddGradeExp( int iExp )
{
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal ) 
	{
		int iLimitLevel = pLocal->GetLimitGradeLevel();
		if( iLimitLevel != -1 && iLimitLevel <= GetGradeLevel() )
		{
			return;
		}
	}

	int iNextExp = g_LevelMgr.GetNextGradeupExp( GetGradeLevel() );
	m_user_data.m_grade_exp += iExp;	
	if( iNextExp <= GetGradeExpert() )
	{
		if( GetGradeLevel() < g_LevelMgr.GetMaxGradeUp() )
		{
			m_user_data.m_grade_level++;

			int iRemainExp = m_user_data.m_grade_exp - iNextExp;
			m_user_data.m_grade_exp = 0;

			if( iRemainExp > 0 )
				AddGradeExp( iRemainExp );
		}
	}
	g_UserInfoMgr.SetGrade( GetPublicID(), GetGradeLevel(), GetGradeExpRate() );
	g_QuestMgr.QuestCompleteTerm( QC_GRADE_UP );
	g_QuestMgr.QuestOccurTerm( QO_GRADE_EXP_ACQUIRE );
}

void ioMyInfo::AddClassExp( int iClassType, int iExp )
{
	int iRemainExp = 0;
	bool bLevelUP = m_ClassExpert.AddClassExp( iClassType, iExp, iRemainExp );

	if( ioPlayMode::GetModeType() != MT_ARENA )
	{
		// 경험치가 남아있으면 계속 Add한다.
		if( iRemainExp > 0 )
			AddClassExp( iClassType, iRemainExp );

		g_QuestMgr.QuestCompleteTerm( QC_SLODIER_LEVELUP, iClassType );
		g_QuestMgr.QuestOccurTerm( QO_SOLDIER_EXP_ACQUIRE, iClassType, m_ClassExpert.GetClassLevel( iClassType ) );
	}

	SerUserInfoCharData( iClassType );

	// Level UP
	if( bLevelUP )
	{
		ioUserEtcItem::ETCITEMSLOT kSlot;
		if( m_UserEtcItem.GetEtcItem( ioEtcItem::EIT_ETC_SOLDIER_EXP_BONUS, kSlot ) )
		{
			// 용병 부스터 특별아이템 사용 여부		
			int iClassLevel = GetClassLevel( iClassType, true );
			if( iClassType == kSlot.GetDateExcludeValue2() )
			{
				ioEtcItemSoldierExpBonus *pEtcItem = dynamic_cast< ioEtcItemSoldierExpBonus * >( g_EtcItemMgr.FindEtcItem( ioEtcItem::EIT_ETC_SOLDIER_EXP_BONUS ) );
				if( pEtcItem )
				{
					if( iClassLevel == pEtcItem->GetLimitLevel() )
					{
						ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
						kPrinter[0].SetTextStyle( TS_NORMAL );
						kPrinter[0].SetBkColor( 0, 0, 0 );
						kPrinter[0].SetTextColor( TCT_DEFAULT_GRAY );
						kPrinter[0].AddTextPiece( FONT_SIZE_13, STR(1), GetClassName( iClassType, GetCharPowerUpGradeByClassType( iClassType ) ), iClassLevel );

						kPrinter[1].SetTextStyle( TS_NORMAL );
						kPrinter[1].SetBkColor( 0, 0, 0 );
						kPrinter[1].SetTextColor( TCT_DEFAULT_GRAY );
						kPrinter[1].AddTextPiece( FONT_SIZE_13, "             " );

						kPrinter[2].SetTextStyle( TS_NORMAL );
						kPrinter[2].SetBkColor( 0, 0, 0 );
						kPrinter[2].SetTextColor( TCT_DEFAULT_GRAY );
						kPrinter[2].AddTextPiece( FONT_SIZE_13, STR(2) );

						kPrinter[3].SetTextStyle( TS_NORMAL );
						kPrinter[3].SetBkColor( 0, 0, 0 );
						kPrinter[3].SetTextColor( TCT_DEFAULT_GRAY );
						kPrinter[3].AddTextPiece( FONT_SIZE_13, STR(3) );
						g_GUIMgr.SetPrevMsgListBox( NULL, MB_OK, NULL, kPrinter );
					}
				}
			}
		}

#if defined( USE_GA )
		char chClassType[32]	= {0,};
		char chLabel[32]		= {0,};
		char chSlash[16]		= {0,};
		char chPostData[256]	= {0,};

		if ( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
		{
			sprintf_e( chClassType, "%d", iClassType );
			sprintf_e( chLabel, "%d", GetClassLevel( iClassType, true ) );
			sprintf_e( chSlash, "%%2F" );
			sprintf_e( chPostData, "%sCHAR%sLEVEL_UP%s%s", chSlash, chSlash, chSlash, chClassType );
		}
		else
		{
			SafeSprintf( chClassType, sizeof(chClassType), "%1", iClassType );
			SafeSprintf( chLabel, sizeof(chLabel), "%1", GetClassLevel( iClassType, true ) );
			SafeSprintf( chSlash, sizeof(chSlash), "%2F" );
			SafeSprintf( chPostData, sizeof(chPostData), "%1CHAR%2LEVEL_UP%3%4", chSlash, chSlash, chSlash, chClassType );
		}		

		// CHAR_LV_UP
		g_HttpMng.GA_EventTracking( g_MyInfo.GetUserIndex()
			, "Character"
			, "LevelUp"
			, chLabel
			, 1
			, chPostData );
#endif
	}
	else
	{
#if defined( USE_GA )
 		char chClassType[32]	= {0,};
 		char chLabel[32]		= {0,};
 		char chSlash[16]		= {0,};
 		char chPostData[256]	= {0,};
 
		if ( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
		{
			sprintf_e( chClassType, "%d", iClassType );
			sprintf_e( chLabel, "%d", iExp );
			sprintf_e( chSlash, "%%2F" );
			sprintf_e( chPostData, "%sCHAR%sGET_EXP%s%s", chSlash, chSlash, chSlash, chClassType );
		}
		else
		{
			SafeSprintf( chClassType, sizeof(chClassType), "%1", iClassType );
			SafeSprintf( chLabel, sizeof(chLabel), "%1", iExp );
			SafeSprintf( chSlash, sizeof(chSlash), "%2F" );
			SafeSprintf( chPostData, sizeof(chPostData), "%1CHAR%2GET_EXP%3%4", chSlash, chSlash, chSlash, chClassType );
		}		

 		// CHAR_GET_EXP
 		g_HttpMng.GA_EventTracking( g_MyInfo.GetUserIndex()
			, "Character"
			, "GetExp"
			, chLabel
 			, 1
 			, chPostData );
#endif
	}
}

void ioMyInfo::SetArenaModeCharacter( ModeType eMode )
{
	if( eMode == MT_ARENA )
	{
		m_pCharData = &m_char_arena_data;
		m_pCurrentCharInfoList = &m_vArenaCharInfoList;
		m_SoldierSelectUIBeforeLoading.SetArenaBufferList( true );
	}
	else
	{
		m_pCharData = &m_char_data;
		m_pCurrentCharInfoList = &m_vPlayCharInfoList;
		m_SoldierSelectUIBeforeLoading.SetArenaBufferList( false );
	}

	//g_UserInfoMgr.ResetCharacterData( GetPublicID() );
	//SerUserInfoAllCharData();

#ifndef SHIPPING
	LOG.PrintConsoleLog( 0, "ioMyInfo::SetArenaModeCharacter - eMode = %d", eMode );
#endif

	//SetBeforeCharLoading( false );
}

void ioMyInfo::SetClassExpNLevel( int iClassType, int iExp, int iLevel )
{
	bool bLevelUP = false;

	if( iLevel > m_ClassExpert.GetClassLevel( iClassType ) )
		bLevelUP = true;

	m_ClassExpert.SetClassExp( iClassType, iExp );
	m_ClassExpert.SetClassLevel( iClassType, iLevel );

	if( ioPlayMode::GetModeType() != MT_ARENA )
	{
		g_QuestMgr.QuestCompleteTerm( QC_SLODIER_LEVELUP, iClassType );
		g_QuestMgr.QuestOccurTerm( QO_SOLDIER_EXP_ACQUIRE, iClassType, m_ClassExpert.GetClassLevel( iClassType ) );
	}

	SerUserInfoCharData( iClassType );

	// Level UP
	if( bLevelUP )
	{
		ioUserEtcItem::ETCITEMSLOT kSlot;
		if( m_UserEtcItem.GetEtcItem( ioEtcItem::EIT_ETC_SOLDIER_EXP_BONUS, kSlot ) )
		{
			// 용병 부스터 특별아이템 사용 여부		
			int iClassLevel = GetClassLevel( iClassType, true );
			if( iClassType == kSlot.GetDateExcludeValue2() )
			{
				ioEtcItemSoldierExpBonus *pEtcItem = dynamic_cast< ioEtcItemSoldierExpBonus * >( g_EtcItemMgr.FindEtcItem( ioEtcItem::EIT_ETC_SOLDIER_EXP_BONUS ) );
				if( pEtcItem )
				{
					if( iClassLevel == pEtcItem->GetLimitLevel() )
					{
						ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
						kPrinter[0].SetTextStyle( TS_NORMAL );
						kPrinter[0].SetBkColor( 0, 0, 0 );
						kPrinter[0].SetTextColor( TCT_DEFAULT_GRAY );
						kPrinter[0].AddTextPiece( FONT_SIZE_13, STR(1), GetClassName( iClassType, GetCharPowerUpGradeByClassType( iClassType ) ), iClassLevel );

						kPrinter[1].SetTextStyle( TS_NORMAL );
						kPrinter[1].SetBkColor( 0, 0, 0 );
						kPrinter[1].SetTextColor( TCT_DEFAULT_GRAY );
						kPrinter[1].AddTextPiece( FONT_SIZE_13, "             " );

						kPrinter[2].SetTextStyle( TS_NORMAL );
						kPrinter[2].SetBkColor( 0, 0, 0 );
						kPrinter[2].SetTextColor( TCT_DEFAULT_GRAY );
						kPrinter[2].AddTextPiece( FONT_SIZE_13, STR(2) );

						kPrinter[3].SetTextStyle( TS_NORMAL );
						kPrinter[3].SetBkColor( 0, 0, 0 );
						kPrinter[3].SetTextColor( TCT_DEFAULT_GRAY );
						kPrinter[3].AddTextPiece( FONT_SIZE_13, STR(3) );
						g_GUIMgr.SetPrevMsgListBox( NULL, MB_OK, NULL, kPrinter );
					}
				}
			}
		}

#if defined( USE_GA )
		char chClassType[32]	= {0,};
		char chLabel[32]		= {0,};
		char chSlash[16]		= {0,};
		char chPostData[256]	= {0,};

		if ( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
		{
			sprintf_e( chClassType, "%d", iClassType );
			sprintf_e( chLabel, "%d", GetClassLevel( iClassType, true ) );
			sprintf_e( chSlash, "%%2F" );
			sprintf_e( chPostData, "%sCHAR%sLEVEL_UP%s%s", chSlash, chSlash, chSlash, chClassType );
		}
		else
		{
			SafeSprintf( chClassType, sizeof(chClassType), "%1", iClassType );
			SafeSprintf( chLabel, sizeof(chLabel), "%1", GetClassLevel( iClassType, true ) );
			SafeSprintf( chSlash, sizeof(chSlash), "%2F" );
			SafeSprintf( chPostData, sizeof(chPostData), "%1CHAR%2LEVEL_UP%3%4", chSlash, chSlash, chSlash, chClassType );
		}		

		// CHAR_LV_UP
		g_HttpMng.GA_EventTracking( g_MyInfo.GetUserIndex()
			, "Character"
			, "LevelUp"
			, chLabel
			, 1
			, chPostData );
#endif
	}
	else
	{
#if defined( USE_GA )
		char chClassType[32]	= {0,};
		char chLabel[32]		= {0,};
		char chSlash[16]		= {0,};
		char chPostData[256]	= {0,};

		if ( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
		{
			sprintf_e( chClassType, "%d", iClassType );
			sprintf_e( chLabel, "%d", iExp );
			sprintf_e( chSlash, "%%2F" );
			sprintf_e( chPostData, "%sCHAR%sGET_EXP%s%s", chSlash, chSlash, chSlash, chClassType );
		}
		else
		{
			SafeSprintf( chClassType, sizeof(chClassType), "%1", iClassType );
			SafeSprintf( chLabel, sizeof(chLabel), "%1", iExp );
			SafeSprintf( chSlash, sizeof(chSlash), "%2F" );
			SafeSprintf( chPostData, sizeof(chPostData), "%1CHAR%2GET_EXP%3%4", chSlash, chSlash, chSlash, chClassType );
		}		

		// CHAR_GET_EXP
		g_HttpMng.GA_EventTracking( g_MyInfo.GetUserIndex()
			, "Character"
			, "GetExp"
			, chLabel
			, 1
			, chPostData );
#endif
	}
}

void ioMyInfo::AddKill( ModeType eMode, int iCount, bool bDropKill, bool bBestAttacker )
{
	if( g_BattleRoomMgr.IsBattleRoom() )
	{
		m_UserRecord.AddRecordKill( RECORD_BATTLE, iCount );

		if( eMode != MT_MONSTER_SURVIVAL && eMode != MT_RAID && eMode != MT_DUNGEON_A && !(Help::IsMonsterDungeonMode(eMode)))
		{
			g_QuestMgr.QuestCompleteTerm( QC_BATTLE_PVP_KO, max( 0, iCount ), (int)eMode, (int)bBestAttacker );
			g_QuestMgr.QuestCompleteTerm( QC_EXTRA_ITEM_EQUIP_KO, max( 0, iCount ), (int)bBestAttacker );

			if( bDropKill )
				g_QuestMgr.QuestCompleteTerm( QC_DROP_KILL, max( 0, iCount ), (int)eMode, (int)bBestAttacker );
			g_QuestMgr.QuestCompleteTerm( QC_MULTI_KILL, max( 0, iCount ), (int)eMode, (int)bBestAttacker );
		}
		g_QuestMgr.QuestCompleteTerm( QC_BATTLE_EXP_ACQUIRE );
	}
	else if( g_LadderTeamMgr.IsLadderTeam() )
	{
		if( eMode == MT_HEROMATCH )
		{	
			m_UserRecord.AddRecordKill( RECORD_HEROMATCH, iCount );
		}
		else
		{	
			m_UserRecord.AddRecordKill( RECORD_LADDER, iCount );
		}

		g_QuestMgr.QuestCompleteTerm( QC_CAMP_BATTLE_KO, max( 0, iCount ), (int)eMode, (int)bBestAttacker );
		g_QuestMgr.QuestCompleteTerm( QC_EXTRA_ITEM_EQUIP_KO, max( 0, iCount ), (int)bBestAttacker );

		if( bDropKill )
			g_QuestMgr.QuestCompleteTerm( QC_DROP_KILL, max( 0, iCount ), (int)eMode,(int)bBestAttacker );
		g_QuestMgr.QuestCompleteTerm( QC_MULTI_KILL, max( 0, iCount ), (int)eMode, (int)bBestAttacker );
		g_QuestMgr.QuestCompleteTerm( QC_CAMP_EXP_ACQUIRE );
	}
	else if( g_ShuffleRoomMgr.IsShuffleRoom() )
	{
		m_UserRecord.AddRecordKill( RECORD_BATTLE, iCount );

		if( eMode != MT_MONSTER_SURVIVAL && eMode != MT_RAID && eMode != MT_DUNGEON_A && !(Help::IsMonsterDungeonMode(eMode)))
		{
			g_QuestMgr.QuestCompleteTerm( QC_BATTLE_PVP_KO, max( 0, iCount ), (int)eMode, (int)bBestAttacker );
			g_QuestMgr.QuestCompleteTerm( QC_EXTRA_ITEM_EQUIP_KO, max( 0, iCount ), (int)bBestAttacker );

			if( bDropKill )
				g_QuestMgr.QuestCompleteTerm( QC_DROP_KILL, max( 0, iCount ), (int)eMode, (int)bBestAttacker );
			g_QuestMgr.QuestCompleteTerm( QC_MULTI_KILL, max( 0, iCount ), (int)eMode, (int)bBestAttacker );
		}
		g_QuestMgr.QuestCompleteTerm( QC_BATTLE_EXP_ACQUIRE );
	}
	else if( g_RankBattleMgr.IsRankBattlePlaying() )
	{
		m_UserRecord.AddRecordKill( RECORD_BATTLE, iCount );

		g_QuestMgr.QuestCompleteTerm( QC_BATTLE_PVP_KO, max( 0, iCount ), (int)eMode, (int)bBestAttacker );
		g_QuestMgr.QuestCompleteTerm( QC_EXTRA_ITEM_EQUIP_KO, max( 0, iCount ), (int)bBestAttacker );

		if( bDropKill )
			g_QuestMgr.QuestCompleteTerm( QC_DROP_KILL, max( 0, iCount ), (int)eMode, (int)bBestAttacker );
			
		g_QuestMgr.QuestCompleteTerm( QC_MULTI_KILL, max( 0, iCount ), (int)eMode, (int)bBestAttacker );
		g_QuestMgr.QuestCompleteTerm( QC_BATTLE_EXP_ACQUIRE );
	}
	g_UserInfoMgr.SetKillDeathLevel( GetPublicID(), GetKillDeathLevel() );
}

void ioMyInfo::AddDeath( ModeType eMode, int iCount )
{
	if( g_BattleRoomMgr.IsBattleRoom() )
	{
		m_UserRecord.AddRecordDeath( RECORD_BATTLE, iCount );
		g_QuestMgr.QuestCompleteTerm( QC_BATTLE_EXP_ACQUIRE );
	}
	else if( g_LadderTeamMgr.IsLadderTeam() )
	{
		if( eMode == MT_HEROMATCH )
		{	
			m_UserRecord.AddRecordDeath( RECORD_HEROMATCH, iCount );
		}
		else
		{	
			m_UserRecord.AddRecordDeath( RECORD_LADDER, iCount );
		}
		g_QuestMgr.QuestCompleteTerm( QC_CAMP_EXP_ACQUIRE );
	}
	else if( g_ShuffleRoomMgr.IsShuffleRoom() )
	{
		m_UserRecord.AddRecordDeath( RECORD_BATTLE, iCount );
		g_QuestMgr.QuestCompleteTerm( QC_BATTLE_EXP_ACQUIRE );
	}
	else if( g_RankBattleMgr.IsRankBattlePlaying() )
	{
		m_UserRecord.AddRecordDeath( RECORD_BATTLE, iCount );
		g_QuestMgr.QuestCompleteTerm( QC_BATTLE_EXP_ACQUIRE );
	}
	g_UserInfoMgr.SetKillDeathLevel( GetPublicID(), GetKillDeathLevel() );
}

void ioMyInfo::AddWinCount( RoomStyle eRoomStyle, ModeType eMode, int iCount )
{
	switch( eRoomStyle )
	{
	case RSTYLE_BATTLEROOM:
	case RSTYLE_SHUFFLEROOM:
	case RSTYLE_MATCHROOM:
		{
			m_UserRecord.AddRecordWin( RECORD_BATTLE, iCount );
			
			g_UserInfoMgr.SetPartyExp( GetPublicID(), GetPartyExp() );
		}
		break;
	case RSTYLE_LADDERBATTLE:
		if( eMode == MT_HEROMATCH )
		{	
			m_UserRecord.AddRecordWin( RECORD_HEROMATCH, iCount );
			
			g_UserInfoMgr.SetHeroWin( GetPublicID(), GetRecordWin( RECORD_HEROMATCH ) );
		}
		else
		{	
			m_UserRecord.AddRecordWin( RECORD_LADDER, iCount );

			g_UserInfoMgr.SetLadderExp( GetPublicID(), GetLadderExp() );
		}
		break;
	}
}

void ioMyInfo::AddLoseCount( RoomStyle eRoomStyle, ModeType eMode, int iCount )
{
	switch( eRoomStyle )
	{
	case RSTYLE_BATTLEROOM:
	case RSTYLE_SHUFFLEROOM:
	case RSTYLE_MATCHROOM:
		{
			m_UserRecord.AddRecordLose( RECORD_BATTLE, iCount );

			g_UserInfoMgr.SetPartyExp( GetPublicID(), GetPartyExp() );
		}
		break;
	case RSTYLE_LADDERBATTLE:
		if( eMode == MT_HEROMATCH )
		{	
			m_UserRecord.AddRecordLose( RECORD_HEROMATCH, iCount );

			g_UserInfoMgr.SetHeroLose( GetPublicID(), GetRecordLose( RECORD_HEROMATCH ) );
		}
		else
		{	
			m_UserRecord.AddRecordLose( RECORD_LADDER, iCount );

			g_UserInfoMgr.SetLadderExp( GetPublicID(), GetLadderExp() );
		}
		break;
	}
}

int ioMyInfo::GetCharIndex(int array)
{
	if( !COMPARE( array, 0, GetCharCount() ) ) return 0;

	return m_pCharData->at(array).m_dwIndex;
}

DWORD ioMyInfo::GetCharIndexByClassType( int iClassType )
{
	for( int i=0 ; i<GetCharCount() ; i++ )
	{
		if( m_pCharData->at(i).m_data.m_class_type == iClassType )
		{
			return m_pCharData->at(i).m_dwIndex;
		}
	}

	return 0;
}

int ioMyInfo::GetCharArray( DWORD dwIndex )
{
	for( int i=0 ; i<GetCharCount() ; i++ )
	{
		if( m_pCharData->at(i).m_dwIndex == dwIndex )
		{
			return i;
		}
	}

	return -1;
}

int ioMyInfo::GetClassArray( int iClassType )
{
	for( int i=0 ; i<GetCharCount() ; i++ )
	{
		if( m_pCharData->at(i).m_data.m_class_type == iClassType )
		{
			return i;
		}
	}

	return -1;
}

int ioMyInfo::GetClassArrayExceptExercise( int iClassType )
{
	for( int i=0 ; i<GetCharCount() ; i++ )
	{
		if( m_pCharData->at(i).m_data.m_chExerciseStyle != EXERCISE_NONE ) continue;
		if( m_pCharData->at(i).m_data.m_class_type == iClassType )
		{
			return i;
		}
	}

	return -1;
}

int ioMyInfo::GetExerciseCharArrayByClass( int iClassType )
{
	for( int i=0 ; i<GetCharCount() ; i++ )
	{
		if( m_pCharData->at(i).m_data.m_chExerciseStyle == EXERCISE_NONE ) continue;
		if( m_pCharData->at(i).m_data.m_class_type == iClassType )
		{
			return i;
		}
	}
	return -1;
}

int ioMyInfo::GetCharEmptyFirstSlot()
{
	int i = 0;
	IntVec vCharSlot;
	for(i = 0;i < GetCurMaxCharSlot();i++)
		vCharSlot.push_back( i );

	for(i = 0;i < GetCharCount();i++)
	{
		for(int j = 0;j < (int)vCharSlot.size();j++)
		{
			if( vCharSlot[j] == m_pCharData->at(i).m_data.m_iSlotIndex )
			{
				vCharSlot.erase( vCharSlot.begin() + j );
				break;
			}
		}
	}

	if( vCharSlot.empty() )
		return -1;
	
	int iReturnSlot = vCharSlot[0];
	vCharSlot.clear();
	return iReturnSlot;
}

void ioMyInfo::GetCharEmptySlotArray( int iArraySize, IntVec& vSlotArray )
{
	int i = 0;
	IntVec vCharSlot;
	for(i = 0; i < GetCurMaxCharSlot(); ++i )
		vCharSlot.push_back( i );

	for( i = 0; i < GetCharCount(); ++i )
	{
		for( int j = 0; j < (int)vCharSlot.size(); ++j )
		{
			if( vCharSlot[j] == m_pCharData->at(i).m_data.m_iSlotIndex )
			{
				vCharSlot.erase( vCharSlot.begin() + j );
				break;
			}
		}
	}	

	if( vCharSlot.empty() )
		return;

	vSlotArray.clear();
	for( int i = 0 ; i < iArraySize; ++i )
	{
		if( !vCharSlot.empty() )
		{
			vSlotArray.push_back( vCharSlot[0] );
			vCharSlot.erase( vCharSlot.begin() );
		}		
	}
}

int ioMyInfo::GetCharSlotIndexToArray( int iSlotArray )
{
	int iSlot;

	if( m_pCharData == &m_char_arena_data )
		iSlot = iSlotArray + GetOriginalCharacterCount();
	else
		iSlot = iSlotArray;

	for(int i = 0;i < GetCharCount();i++)
	{
		if( m_pCharData->at(i).m_data.m_iSlotIndex == iSlot )
		{
			return i;
		}
	}
	return -1;
}

int ioMyInfo::GetCharArrayToSlotIndex( int iArray )
{
	if( !COMPARE( iArray, 0, GetCharCount() ) ) return -1;

	return m_pCharData->at(iArray).m_data.m_iSlotIndex;
}

int ioMyInfo::GetCharIndexToSlotIndex( DWORD dwIndex )
{
	for(int i = 0;i < GetCharCount();i++)
	{
		if( m_pCharData->at(i).m_dwIndex == dwIndex )
		{
			return m_pCharData->at(i).m_data.m_iSlotIndex;
		}
	}

	return -1;
}

int ioMyInfo::GetCharCount() const
{
	return m_pCharData->size();
}

int ioMyInfo::GetCharRentalSetCount()
{
	int iResult = 0;
	for( int i=0 ; i<GetCharCount() ; i++ )
	{
		if( m_pCharData->at(i).m_data.m_sRentalType != CRT_GENERAL )
		{
			iResult++;
		}
	}

	return iResult;
}

int  ioMyInfo::GetActiveCharCount()
{
	int iResult = 0;
	for( int i=0 ; i<GetCharCount() ; i++ )
	{
		if( m_pCharData->at(i).m_data.m_bActive )
		{
			iResult++;
		}
	}

	return iResult;
}

int  ioMyInfo::GetInAtciveCharCount()
{
	int iResult = 0;
	for( int i=0 ; i<GetCharCount() ; i++ )
	{
		if( !m_pCharData->at(i).m_data.m_bActive )
		{
			iResult++;
		}
	}

	return iResult;
}

int ioMyInfo::GetActiveExerciseCharCount()
{
	int iResult = 0;
	for( int i=0 ; i<GetCharCount() ; i++ )
	{
		if( m_pCharData->at(i).m_data.m_bActive && m_pCharData->at(i).m_data.m_chExerciseStyle != EXERCISE_NONE )
			iResult++;
	}

	return iResult;
}

int ioMyInfo::GetExerciseCharCount()
{
	int iResult = 0;
	for( int i = 0; i < GetCharCount() ; i++ )
	{
		if( m_pCharData->at(i).m_data.m_chExerciseStyle != EXERCISE_NONE )
			iResult++;
	}

	return iResult;
}

int ioMyInfo::GetExerciseCharCount( int chExerciseStyle )
{
	int iResult = 0;
	for( int i = 0; i < GetCharCount() ; i++ )
	{
		if( m_pCharData->at(i).m_data.m_chExerciseStyle == chExerciseStyle )
			iResult++;
	}

	return iResult;
}

DWORD ioMyInfo::GetExerciseCharIndex( int chExerciseStyle )
{
	for( int i = 0 ; i < GetCharCount() ; i++ )
	{
		if( m_pCharData->at(i).m_data.m_chExerciseStyle == chExerciseStyle )
			return m_pCharData->at(i).m_dwIndex;
	}

	return 0;
}

int ioMyInfo::GetMortmainCharCount()
{
	int iCurMortmainChar = 0;
	for (int i = 0; i < GetCharCount() ; i++)
	{
		if( m_pCharData->at(i).m_data.m_chExerciseStyle == EXERCISE_RENTAL ) continue;     // 빌린 용병은 제외

		if( m_pCharData->at(i).m_data.m_ePeriodType == CPT_MORTMAIN )
			iCurMortmainChar++;
	}

	return iCurMortmainChar;
}

void ioMyInfo::SetArenaCharacterIndex( int iType, DWORD dwIndex )
{
	for( int i = 0; i < GetCharCount(); i++ )
	{
		if( m_char_arena_data[i].m_data.m_class_type == iType )
		{
			m_char_arena_data[i].m_dwIndex = dwIndex;
			return;
		}
	}
}

void ioMyInfo::SetArenaCharacterSlot( int iType, int iSlot )
{
	for( int i = 0; i < GetCharCount(); i++ )
	{
		if( m_char_arena_data[i].m_data.m_class_type == iType )
		{
			m_char_arena_data[i].m_data.m_iSlotIndex = iSlot;
#ifndef SHIPPING
			LOG.PrintConsoleLog( 0, "ioMyInfo::SetArenaCharacterSlot - iType = %d, iSlot = %d", iType, iSlot );
#endif
			return;
		}
	}
}

int ioMyInfo::GetCharEquipedItemCnt( int iCharArray ) const
{
	if( !COMPARE( iCharArray, 0, GetCharCount() ) )
		return 0;

	int iEquipCnt = 0;
	for( int i=0 ; i<MAX_INVENTORY ; i++ )
	{
		if( m_pCharData->at(iCharArray).m_equip_item[i].m_item_code != 0 )
		{
			iEquipCnt++;
		}
	}

	return iEquipCnt;	
}

void ioMyInfo::ApplyCharDecoration( int iCharArray, SP2Packet &rkPacket )
{
	if( !COMPARE( iCharArray, 0, GetCharCount() ) )
		return;

	rkPacket >> m_pCharData->at(iCharArray).m_data;

	if( m_pCharData->at(iCharArray).m_data.m_chExerciseStyle == EXERCISE_RENTAL )
	{
		m_CharRentalData.EquipItem( m_pCharData->at(iCharArray), false );
		m_CharRentalData.EquipCostume( m_pCharData->at(iCharArray) );
	}
	else
	{
		m_UserExtraItem.EquipItem( m_pCharData->at(iCharArray), false, false );
	}

	m_SoldierSelectUIBeforeLoading.Update( m_pCharData->at(iCharArray) );

	SerUserInfoCharData( m_pCharData->at(iCharArray).m_data.m_class_type );
}

void ioMyInfo::ApplyCharDecoration( DWORD dwCharIndex, CHARACTER &rkInfo )
{
	for( int i=0 ; i < GetCharCount() ; i++ )
	{
		if( m_pCharData->at(i).m_dwIndex == dwCharIndex )
		{
			m_pCharData->at(i).m_data = rkInfo;

			if( rkInfo.m_chExerciseStyle == EXERCISE_RENTAL )
			{
				m_CharRentalData.EquipItem( m_pCharData->at(i), false );
				m_CharRentalData.EquipCostume( m_pCharData->at(i) );
			}
			else
			{
				m_UserExtraItem.EquipItem( m_pCharData->at(i), false, false );
			}	
			m_SoldierSelectUIBeforeLoading.Update( m_pCharData->at(i) );

			SerUserInfoCharData( m_pCharData->at(i).m_data.m_class_type );
			break;
		}
	}
}

DWORD ioMyInfo::ApplyCharPackage( CHARACTER &rkInfo )
{
	// 여기서는 성별 / 머리 / 얼굴 / 머리색 / 피부색 / 속옷만 교체한다.
	for( int i=0 ; i < GetCharCount() ; i++ )
	{
		if( m_pCharData->at(i).m_data.m_chExerciseStyle != EXERCISE_NONE ) continue;

		if( m_pCharData->at(i).m_data.m_class_type == rkInfo.m_class_type )
		{
			// rkInfo - > m_pCharData[i].m_data
			m_pCharData->at(i).m_data.m_kindred = rkInfo.m_kindred;
			m_pCharData->at(i).m_data.m_sex     = rkInfo.m_sex;
			m_pCharData->at(i).m_data.m_hair    = rkInfo.m_hair;
			m_pCharData->at(i).m_data.m_face    = rkInfo.m_face;
			m_pCharData->at(i).m_data.m_hair_color = rkInfo.m_hair_color;
			m_pCharData->at(i).m_data.m_skin_color = rkInfo.m_skin_color;
			m_pCharData->at(i).m_data.m_underwear  = rkInfo.m_underwear;
			m_pCharData->at(i).m_data.m_ePeriodType = rkInfo.m_ePeriodType;
			m_pCharData->at(i).m_data.m_iLimitSecond= rkInfo.m_iLimitSecond;
			m_pCharData->at(i).m_data.m_bActive = rkInfo.m_bActive;
			m_pCharData->at(i).m_data.m_chExerciseStyle = rkInfo.m_chExerciseStyle;

			// m_pCharData[i].m_data - > rkInfo
			for(int k = 0;k < MAX_INVENTORY;k++)
			{
				rkInfo.m_extra_item[k] = m_pCharData->at(i).m_data.m_extra_item[k];
			}
			rkInfo.m_iSlotIndex = m_pCharData->at(i).m_data.m_iSlotIndex;

			m_SoldierSelectUIBeforeLoading.Update( m_pCharData->at(i) );

			SerUserInfoCharData( m_pCharData->at(i).m_data.m_class_type );

			// 룸 동기화
			SP2Packet kPacket( CUPK_CHANGE_CHARINFO);
			kPacket << m_pCharData->at(i).m_dwIndex << m_pCharData->at(i).m_data;
			FillCharItemData( m_pCharData->at(i).m_dwIndex, kPacket );
			P2PNetwork::SendToAllUser( kPacket );
			return m_pCharData->at(i).m_dwIndex;
		}
	}
	// 캐릭터가 없을 수 있는데 없다면 DB Insert 중이므로 Insert가 끝나면 모든 데이터가 한번에 도착한다.
	return 0;
}

void ioMyInfo::ApplyCharExtend( int iCharArray, SP2Packet &rkPacket )
{
	if( !COMPARE( iCharArray, 0, GetCharCount() ) )
		return;

#if defined( USE_GA )
	int iBeforeSec	= m_pCharData->at(iCharArray).m_data.m_iLimitSecond;
#endif

	rkPacket >> m_pCharData->at(iCharArray).m_data.m_bActive;
	rkPacket >> m_pCharData->at(iCharArray).m_data.m_iLimitSecond;		

#if defined( USE_GA )
	int iAfterSec	= m_pCharData->at(iCharArray).m_data.m_iLimitSecond;
	int iLimitDate	= iAfterSec - iBeforeSec;

	g_HttpMng.SetHeroLimitDate( iLimitDate );
#endif

	int iHour = m_pCharData->at(iCharArray).m_data.m_iLimitSecond / 3600;
	int iMin = ( m_pCharData->at(iCharArray).m_data.m_iLimitSecond % 3600 ) / 60;
	g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1), GetClassName( m_pCharData->at(iCharArray).m_data.m_class_type ), iHour, iMin );

	DWORD dwCheckSecond;
	rkPacket >> dwCheckSecond;
	if( dwCheckSecond != 0 )
		StartCharLimitDate( m_pCharData->at(iCharArray).m_dwIndex, dwCheckSecond );

	if( m_pCharData->at(iCharArray).m_data.m_bActive )
	{
		g_QuestMgr.QuestCompleteTerm( QC_SOLDIER_ACQUIRE, m_pCharData->at(iCharArray).m_data.m_class_type, (int)m_pCharData->at(iCharArray).m_data.m_ePeriodType, (int)m_pCharData->at(iCharArray).m_data.m_chExerciseStyle );
	}
}

void ioMyInfo::ApplyInventorySlotData( SP2Packet &rkPacket )
{
	m_Inventory.ApplyInventorySlotData( rkPacket );
}

void ioMyInfo::ApplyBlockType( SP2Packet &rkPacket )
{
	int iBlockType = BKT_NONE;
	rkPacket >> iBlockType;
	m_user_data.m_eBlockType = (BlockType) iBlockType;
	g_UserInfoMgr.SetBlockType( GetPublicID(), iBlockType );
}

void ioMyInfo::ApplyClassExpert( SP2Packet &rkPacket )
{
	m_ClassExpert.ApplyClassInfo( rkPacket );
}

void ioMyInfo::ApplyAwardData( SP2Packet &rkPacket )
{
	m_Award.ApplyAwardData( rkPacket );
}

void ioMyInfo::AddAwardData( int iCategory, int iPoint )
{
	m_Award.AddAwardExp( iPoint );

	g_UserInfoMgr.SetAwardExp( GetPublicID(), GetAwardLevel(), GetAwardExp() );
}

void ioMyInfo::ApplyUserRecordData( SP2Packet &rkPacket )
{
	m_UserRecord.ApplyRecordData( rkPacket );
}

void ioMyInfo::ApplyUserHeroSeasonRecord( SP2Packet &rkPacket )
{
	m_UserRecord.ApplyHeroSeasonRecord( rkPacket );
}

void ioMyInfo::ApplyGrowthLevelData( SP2Packet &rkPacket )
{
	m_GrowthLevel.ApplyGrowthLevelData( rkPacket );
	m_GrowthLevel.CheckGrowthPoint();
}

void ioMyInfo::ApplyFishItemData( SP2Packet &rkPacket )
{
	m_UserFishItem.ApplyFishItemData( rkPacket );
}

void ioMyInfo::ApplyQuestData( SP2Packet &rkPacket )
{
	m_Quest.ApplyQuestData( rkPacket );
}

void ioMyInfo::ApplyQuestCompleteData( SP2Packet &rkPacket )
{
	m_Quest.ApplyQuestCompleteData( rkPacket );
}

void ioMyInfo::FillCharItemData( DWORD dwCharIndex, SP2Packet &rkPacket )
{
	for( int i=0 ; i<GetCharCount() ; i++ )
	{
		if( m_pCharData->at(i).m_dwIndex == dwCharIndex )
		{
			for(int iSlot = 0;iSlot < MAX_INVENTORY;iSlot++)
			{
				rkPacket << m_pCharData->at(i).m_equip_item[iSlot].m_item_code;
				rkPacket << m_pCharData->at(i).m_equip_item[iSlot].m_item_male_custom;
				rkPacket << m_pCharData->at(i).m_equip_item[iSlot].m_item_female_custom;
			}
			break;
		}
	}
}

void ioMyInfo::ChangeCharEquipItemCode( DWORD dwCharIndex, int iEquipIndex, int iItemCode )
{
	for( int i=0 ; i<GetCharCount() ; i++ )
	{
		if( m_pCharData->at(i).m_dwIndex == dwCharIndex )
		{
			m_pCharData->at(i).m_equip_item[iEquipIndex].m_item_code = iItemCode;
			break;
		}
	}
}

ioGrowthLevel* ioMyInfo::GetGrowthLevel()
{
	return &m_GrowthLevel;
}

bool ioMyInfo::CharSlotChange( int iLeftArray, int iLeftSlotIndex, int iRightArray, int iRightSlotIndex )
{
	if( COMPARE( iLeftArray, 0, GetCharCount() ) )
	{
		m_pCharData->at(iLeftArray).m_data.m_iSlotIndex = iRightSlotIndex;
	}
	if( COMPARE( iRightArray, 0, GetCharCount() ) )
	{
		m_pCharData->at(iRightArray).m_data.m_iSlotIndex = iLeftSlotIndex;
	}
	return true;
}

bool ioMyInfo::CharDelete( int eid )
{
	if( !COMPARE( eid, 0, GetCharCount() ) ) return false;

	// 추가장비 장착해제 부터...
	for( int k=0; k < MAX_INVENTORY; ++k )
	{
		int iSlotIndex = m_pCharData->at(eid).m_data.m_extra_item[k];
		m_UserExtraItem.SetCharEquip( iSlotIndex, false );
		m_pCharData->at(eid).m_data.m_extra_item[k] = 0; 
	}
	for( int k=0; k < MAX_ACC_INVENTORY; ++k )
	{
		int iSlotIndex = m_pCharData->at(eid).m_data.m_EquipAccessory[k].m_iAccessoryIdx;
		m_UserAccessory.SetAccessoryEquip( iSlotIndex, false );
		m_pCharData->at(eid).m_data.m_EquipAccessory[k].Init();
	}

	// 장착메달 해제
	m_UserMedalItem.ReleaseEquipMedal( m_pCharData->at(eid).m_data.m_class_type );

	// 코스튬 관련 (캐릭터 해고시 코스튬 해제)
	for( int k=0; k<MAX_INVENTORY; ++k )
	{
		int nSlotIndex = m_pCharData->at(eid).m_data.m_EquipCostume[k].m_CostumeIdx;
		m_UserCostume.SetCostumeEquip( nSlotIndex, false );
	}

	m_SoldierSelectUIBeforeLoading.EraseCharList( m_pCharData->at(eid).m_dwIndex );	

	g_UserInfoMgr.DeleteCharData( GetPublicID(), m_pCharData->at(eid).m_data.m_class_type );

	if( m_pCharData->at(eid).m_data.m_chExerciseStyle == EXERCISE_RENTAL )
	{
		m_CharRentalData.DeleteRentalData( m_pCharData->at(eid).m_dwIndex );
		RemoveRentalTimeData( m_pCharData->at(eid).m_dwIndex );
	}

	m_pCharData->erase( m_pCharData->begin() + eid );	

	if( COMPARE( eid, 0, (int)m_pCurrentCharInfoList->size() ) )
	{
		SAFEDELETE( m_pCurrentCharInfoList->at(eid) );
		m_pCurrentCharInfoList->erase( m_pCurrentCharInfoList->begin() + eid );
	}

	if( m_select_char_array > eid )
		m_select_char_array = max( 0, m_select_char_array - 1 );
	else if( m_select_char_array == eid )
	{
		m_select_char_array = 0;
		for(int i = 0;i < GetCharCount();i++)
		{
			if( m_pCharData->at(i).m_data.m_bActive )
			{
				m_select_char_array = i;
				break;
			}
		}
	}
	SyncBattleNLadderCharIndex();
	return true;
}

int ioMyInfo::CharDelete( DWORD dwIdx )
{
	int iClassType = -1;
	for( int i=0 ; i<GetCharCount() ; i++ )
	{
		if( m_pCharData->at(i).m_dwIndex == dwIdx )
		{
			iClassType = m_pCharData->at(i).m_data.m_class_type;
			CharDelete( i );
			break;
		}
	}

	return iClassType;
}

void ioMyInfo::CharLeaderChange( DWORD dwCharIndex )
{
	for( int i=0 ; i<GetCharCount() ; i++ )
	{
		if( m_pCharData->at(i).m_dwIndex == dwCharIndex )
		{
			m_pCharData->at(i).m_data.m_sLeaderType = CLT_LEADER;
			SerUserInfoCharData( m_pCharData->at(i).m_data.m_class_type );
		}
		else if( m_pCharData->at(i).m_data.m_sLeaderType == CLT_LEADER )
		{
			m_pCharData->at(i).m_data.m_sLeaderType = CLT_GENERAL;
			SerUserInfoCharData( m_pCharData->at(i).m_data.m_class_type );
		}
	}
}

void ioMyInfo::CharSetMyRental( DWORD dwCharIndex, short sRentalType )
{
	for( int i=0 ; i<GetCharCount() ; i++ )
	{
		if( m_pCharData->at(i).m_dwIndex == dwCharIndex )
		{			
			m_pCharData->at(i).m_data.m_sRentalType = sRentalType;			
			SerUserInfoCharData( m_pCharData->at(i).m_data.m_class_type );

			switch( sRentalType )
			{
			case CRT_GENERAL:
				{
					ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
					kPrinter[0].SetTextStyle( TS_NORMAL );
					kPrinter[0].SetBkColor( 0, 0, 0 );	
					kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
					kPrinter[0].AddTextPiece( FONT_SIZE_13, STR(1), GetClassName( m_pCharData->at(i).m_data.m_class_type ) );
					g_GUIMgr.SetPrevMsgListBoxWithTitle( NULL, MB_OK, NULL, kPrinter, STR(2), STR(3), "", "" );
					
					SetRoomOutTimer(0 );
				}
				break;
			case CRT_RENTAL:
				{
					ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
					kPrinter[0].SetTextStyle( TS_NORMAL );
					kPrinter[0].SetBkColor( 0, 0, 0 );	
					kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
					kPrinter[0].AddTextPiece( FONT_SIZE_13, STR(4), GetClassName( m_pCharData->at(i).m_data.m_class_type ) );

					kPrinter[1].SetTextStyle( TS_NORMAL );
					kPrinter[1].SetBkColor( 0, 0, 0 );	
					kPrinter[1].SetTextColor( TCT_DEFAULT_DARKGRAY );
					kPrinter[1].AddTextPiece( FONT_SIZE_13, STR(5) );
					kPrinter[1].SetTextColor( TCT_DEFAULT_GREEN );
					kPrinter[1].AddTextPiece( FONT_SIZE_13, STR(6) );
					kPrinter[1].SetTextColor( TCT_DEFAULT_DARKGRAY );
					kPrinter[1].AddTextPiece( FONT_SIZE_13, STR(7) );

					kPrinter[2].SetTextStyle( TS_NORMAL );
					kPrinter[2].SetBkColor( 0, 0, 0 );	
					kPrinter[2].SetTextColor( TCT_DEFAULT_GREEN );
					kPrinter[2].AddTextPiece( FONT_SIZE_13, STR(8) );
					kPrinter[2].SetTextColor( TCT_DEFAULT_DARKGRAY );
					kPrinter[2].AddTextPiece( FONT_SIZE_13, STR(9) );
					g_GUIMgr.SetPrevMsgListBoxWithTitle( NULL, MB_OK, NULL, kPrinter, STR(10), STR(11), "", "" );
				}
				break;
			}
			break;
		}
	}
}

void ioMyInfo::SetCharActive( DWORD dwIdx, bool bActive )
{
	for( int i=0 ; i<GetCharCount() ; i++ )
	{
		if( m_pCharData->at(i).m_dwIndex == dwIdx )
		{
			m_pCharData->at(i).m_data.m_bActive = bActive;
			break;
		}
	}
}

bool ioMyInfo::IsCharActive( int eid )
{
	if( !COMPARE( eid, 0, GetCharCount() ) ) 
		return false;
	return m_pCharData->at(eid).m_data.m_bActive;
}

bool ioMyInfo::IsCharRental( int eid )
{
	if( !COMPARE( eid, 0, GetCharCount() ) ) 
		return false;
	if( m_pCharData->at(eid).m_data.m_sRentalType == CRT_RENTAL )
		return true;
	return false;
}

bool ioMyInfo::IsCharExerciseRentalInActive( int eid )
{
	if( !COMPARE( eid, 0, GetCharCount() ) ) 
		return false;
	if( m_pCharData->at(eid).m_data.m_chExerciseStyle == EXERCISE_RENTAL )
	{
		if( m_pCharData->at(eid).m_data.m_dwRentalMinute == 0 )
			return true;
	}
	return false;
}

bool ioMyInfo::IsCharInActive( int iClassType )
{
	for( int i = 0 ; i< GetCharCount() ; i++ )
	{
		if( m_pCharData->at(i).m_data.m_chExerciseStyle != EXERCISE_NONE )
			continue;

		if( m_pCharData->at(i).m_data.m_class_type == iClassType )
		{
			return !m_pCharData->at(i).m_data.m_bActive;
		}
	}
	return false;
}

bool ioMyInfo::IsCharExercise( int iArray )
{
	if( !COMPARE( iArray, 0, GetCharCount() ) ) 
		return false;
	if( m_pCharData->at(iArray).m_data.m_chExerciseStyle != EXERCISE_NONE )
		return true;
	return false;
}

bool ioMyInfo::IsCharExerciseStyle( int iArray, int chExerciseStyle )
{
	if( !COMPARE( iArray, 0, GetCharCount() ) ) 
		return false;
	if( m_pCharData->at(iArray).m_data.m_chExerciseStyle == chExerciseStyle )
		return true;
	return false;
}

bool ioMyInfo::IsCharExerciseStyleToClassType( int iClassType, int chExerciseStyle )
{
	for( int i=0 ; i<GetCharCount() ; i++ )
	{
		if( m_pCharData->at(i).m_data.m_class_type == iClassType )
		{
			if( m_pCharData->at(i).m_data.m_chExerciseStyle == chExerciseStyle )
				return true;
		}
	}
	return false;
}

int ioMyInfo::GetAutoSelectChar( int iSelectId, int iPreSelectId )
{
	if( GetCharIndex( iSelectId ) == 0 )
	{
		iSelectId = iPreSelectId;
	}

	if( IsCharActive( iSelectId ) && !IsCharRental( iSelectId ) && !IsCharExerciseRentalInActive( iSelectId ) )
		return iSelectId;

	IntVec vSelectChar;
	for( int i=0 ; i<GetCharCount() ; i++ )
	{
		if( IsCharRental( i ) ) continue;
		if( IsCharExerciseRentalInActive( i ) ) continue;

		if( m_pCharData->at(i).m_data.m_bActive )
		{
			vSelectChar.push_back( i );
		}
	}

	if( !vSelectChar.empty() )
	{
		int iSize = vSelectChar.size();
		iSelectId = vSelectChar[ rand() % iSize ];
		vSelectChar.clear();
	}
	else if( GetActiveCharCount() == 0 && GetCharCount() != 0 )
	{
		iSelectId = rand() % GetCharCount();
	}
	return iSelectId;
}

bool ioMyInfo::IsCharMortmain( int iClassType, bool bPowerUp )
{
	bool bHasHero = false;
	for( int i=0 ; i<GetCharCount() ; i++ )
	{
		if( m_char_data[i].m_data.m_class_type == iClassType )
		{
			if( m_char_data[i].m_data.m_ePeriodType == CPT_MORTMAIN )
				return true;
			if( m_char_data[i].m_data.m_ePeriodType == CPT_TIME )
				bHasHero = true;
		}
	}
	if(!bHasHero && bPowerUp)
		return true;
	return false;
}

float ioMyInfo::GetSoldierExpBonusByGradeExp( int iClassType, int iExp )
{
	ioUserEtcItem::ETCITEMSLOT kSlot;
	if( m_UserEtcItem.GetEtcItem( ioEtcItem::EIT_ETC_SOLDIER_EXP_BONUS, kSlot ) )
	{
		if( iClassType == kSlot.GetDateExcludeValue2() )
		{
			ioEtcItemSoldierExpBonus *pEtcItem = dynamic_cast< ioEtcItemSoldierExpBonus * >( g_EtcItemMgr.FindEtcItem( ioEtcItem::EIT_ETC_SOLDIER_EXP_BONUS ) );
			if( pEtcItem )
			{
				int iClassLevel = GetClassLevel( iClassType, true );
				if( iClassLevel < pEtcItem->GetLimitLevel() )
				{
					float fExp = (float)iExp;
					return ( fExp / ( FLOAT1 + ( (float)pEtcItem->GetUseValue() / FLOAT100 ) ) ) + 0.0001f;
				}
			}
		}
	}
	return iExp;
}

void ioMyInfo::LoadINIInfo( ioINILoader &rkLoader )
{
	rkLoader.SetTitle( "etc" );

	m_fMaxRevivalPenalty = rkLoader.LoadFloat_e( "max_revival_penalty", FLOAT05 );
	m_fRecoverPenaltyPerSec = rkLoader.LoadFloat_e( "recover_revival_penalty", 0.01f );
	m_fRecoverPenaltyPlayPerSec = rkLoader.LoadFloat_e( "recover_revival_penalty_play", 0.01f );
	m_fRecoverItemGaugeRate = rkLoader.LoadFloat_e( "recover_item_gauge_rate", FLOAT1 );

	ioINILoader &rkCharLoader = Help::GetCharLoader();
	rkCharLoader.SetTitle_e( "penalty_info" );
	m_iPenaltyPeso = rkCharLoader.LoadInt_e( "penalty_peso", 500 );
	m_fPenaltyPesoRate = rkCharLoader.LoadFloat_e( "safety_penalty_rate", 0.1f );

	m_SelectShutDownAnnouncer.LoadINI();
	m_Clover.LoadINI();

	if( m_pBingo )
	{
		m_pBingo->LoadINI();
		m_pBingo->CreateIconMap();
	}

	if( m_pPriateRullet )
		m_pPriateRullet->Initialize();
	if( m_pPriateRenewalRullet )
		m_pPriateRenewalRullet->Initialize();
}

int ioMyInfo::GetPartyLevel()
{
	int iPartyExp = GetPartyExp();
	int iPartyLevel = 0;
	for(;iPartyLevel < 99;iPartyLevel++)
	{
		__int64 iExp = g_LevelMgr.GetNextPartyupExp( iPartyLevel );
		if( iExp > iPartyExp )
			break;
	}
	return iPartyLevel;
}

__int64 ioMyInfo::GetPartyExp()
{
	return m_UserRecord.GetRecordEXP( RECORD_BATTLE );
}

int ioMyInfo::GetLadderLevel()
{
	int iLadderExp = GetLadderExp();
	int iLadderLevel = 0;
	for(;iLadderLevel < 99;iLadderLevel++)
	{
		__int64 iExp = g_LevelMgr.GetNextLadderupExp( iLadderLevel );
		if( iExp > iLadderExp )
			break;
	}
	return iLadderLevel;
}

__int64 ioMyInfo::GetLadderExp()
{
	return m_UserRecord.GetRecordEXP( RECORD_LADDER );
}

int ioMyInfo::GetHeroTitle()
{
	return m_UserHeroData.m_iHeroTitle;
}

int ioMyInfo::GetHeroRank()
{
	return m_UserHeroData.m_iHeroTodayRank;
}

int ioMyInfo::GetAwardLevel()
{
	return m_Award.GetAwardLevel();
}

int ioMyInfo::GetAwardExp()
{
	return m_Award.GetAwardExp();
}

int ioMyInfo::GetRecordWin( RecordType eRecordType )
{
	return (float)m_UserRecord.GetRecordWin( eRecordType ) / 30;
}

int ioMyInfo::GetRecordLose( RecordType eRecordType )
{
	return (float)m_UserRecord.GetRecordLose( eRecordType ) / 10;
}

float ioMyInfo::GetRecordWinLoseRate( RecordType eRecordType )
{
	return m_UserRecord.GetWinLoseRate( eRecordType );
}

int ioMyInfo::GetHeroSeasonWin()
{
	return (float)m_UserRecord.GetHeroSeasonRecordWin() / 30;
}

int ioMyInfo::GetHeroSeasonLose()
{
	return (float)m_UserRecord.GetHeroSeasonRecordLose() / 10;
}

float ioMyInfo::GetHeroSeasonWinLoseRate()
{
	return (float)m_UserRecord.GetHeroSeasonWinLoseRate();
}

void ioMyInfo::SetUserInfoMgr()
{
	int iClassType = GetClassType();
	MedalSyncInfoVec vMedalItemTypeVec;
	IntVec vGrowthLevelVec;

	GetMedalTypeAndGrowthLevel( iClassType, vMedalItemTypeVec, vGrowthLevelVec);
	
	//펫 성장
	PetSlot sPetSlot;
	m_UserPet.GetPetSlotEquip( sPetSlot );
	
	//유저 정보
	g_UserInfoMgr.SetUserInfo(	GetPublicID(), GetGradeLevel(), GetGradeExpRate(), GetPartyExp(), GetLadderExp(), GetAwardLevel(), GetAwardExp(),
								GetKillDeathLevel(), GetFishingLevel(), GetFishingExpert(), GetExcavationLevel(), GetExcavationExpert(),
								GetRecordWin( RECORD_HEROMATCH ), GetRecordLose( RECORD_HEROMATCH ), GetHeroTitle(), GetHeroRank(), (int)GetBlockType(), GetUserRanking(), 
								GetUserCampPos(), GetLadderPoint(), GetClassLevel( iClassType, false ), iClassType, vMedalItemTypeVec, vGrowthLevelVec, 
								sPetSlot.m_nPetCode, sPetSlot.m_nCurLevel, IsHouseOpen(), GetNamedTitleCode(), GetNamedTitleLevel(), GetNamedTitlePremium(), m_vSoldierSetCntVec,
								m_iCurSuccessionCnt, m_iMaxSuccessionCnt );
	
	//각성
	g_UserInfoMgr.SetCharAwake( GetPublicID(), iClassType, static_cast<int>( GetAwakeType( iClassType ) ) );
	
	// 길드 정보
	g_UserInfoMgr.SetGuildIndex( GetPublicID(), g_GuildInfo.GetGuildIndex() );	

	// 코스튬 관련 (유저 정보 설정)
	g_UserInfoMgr.SetEquipCostume( GetPublicID(), iClassType, GetCharacterToClassType( iClassType ).m_EquipCostume );
}

void ioMyInfo::SerUserInfoAllCharData()
{
	for(int i = 0;i < GetCharCount();i++)
	{
		SerUserInfoCharData( m_pCharData->at(i).m_data.m_class_type );
	}
}

void ioMyInfo::SerUserInfoCharData( int iClassType )
{
	if( m_pCharData == &m_char_arena_data )
		return;

	const CHARACTER &rkCharInfo = GetCharacterToClassType( iClassType );	

	if( rkCharInfo.m_class_type <= 0 )
		return;

	if( rkCharInfo.m_chExerciseStyle != EXERCISE_NONE )
		return;
	
	int i = 0;
	// 캐릭터 정보
	g_UserInfoMgr.SetCharacter( GetPublicID(), rkCharInfo, GetClassLevel( iClassType, false ) );
    
	// 캐릭터 아이템
	ITEM_DATA kEquipItem[MAX_INVENTORY];
	for(i = 0;i < MAX_INVENTORY;i++)
	{
		kEquipItem[i] = GetCharItemToClassType( iClassType, i );
	}
	g_UserInfoMgr.SetEquipItem( GetPublicID(), iClassType, kEquipItem );

	// 캐릭터 메달
	MedalSyncInfoVec kMedal;
	GetMedalItemTypeVec( kMedal, iClassType );
	g_UserInfoMgr.SetEquipMedal( GetPublicID(), iClassType, kMedal );

	// 캐릭터 성장
	int kCharGrowth[MAX_CHAR_GROWTH], kItemGrowth[MAX_ITEM_GROWTH];
	for( i = 0; i < MAX_CHAR_GROWTH; i++)
		kCharGrowth[i] = (int)m_GrowthLevel.GetCharGrowthLevel( iClassType, i, false );
	for( i = 0; i < MAX_ITEM_GROWTH; i++)
		kItemGrowth[i] = (int)m_GrowthLevel.GetItemGrowthLevel( iClassType, i, false );

	g_UserInfoMgr.SetCharGrowth( GetPublicID(), iClassType, kCharGrowth, kItemGrowth, rkCharInfo.m_iAwakeType );
	g_UserInfoMgr.SetCharPowerUp( GetPublicID(), iClassType, rkCharInfo.m_eCharPowerUpType );
}

int ioMyInfo::GetKillDeathLevel()
{
	int iCalculationLevel	= GetGradeLevel() + g_LevelMgr.GetAddGradeLevel();
	int iTotalKill			= m_UserRecord.GetTotalKill();
	float fKillDeathRate	= FLOAT1;

	if( iTotalKill > g_LevelMgr.GetLeaveSafetyRoomKo() )
	{
		fKillDeathRate = (float)iCalculationLevel * m_UserRecord.GetKillDeathPer();
		int iMatchLevel = fKillDeathRate;
		if( iCalculationLevel >= g_LevelMgr.GetRoomKillDeathSafetyLevel() )
		{
			iMatchLevel = max( g_LevelMgr.GetRoomKillDeathSafetyLevel(), iMatchLevel );
		}
		iCalculationLevel = iMatchLevel;
	}

	int iMinKillDeathLevel	= max( 0, GetGradeLevel() + g_LevelMgr.GetAddGradeLevel() - g_LevelMgr.GetKillDeathMinLevelGap() );
	int iKillDeathLevel		= max( min( iCalculationLevel, g_LevelMgr.GetRoomEnterLevelMax() ), iMinKillDeathLevel );
	
#ifndef SHIPPING 
	if( Setting::ShowExtraInfo() )
	{
		LOG.PrintTimeAndLog(0, "%s - Curr Grade Level : %d, TotalKill : %d", __FUNCTION__, GetGradeLevel() + g_LevelMgr.GetAddGradeLevel(), iTotalKill );
		LOG.PrintTimeAndLog(0, "%s - Curr Kill-Death Rate : %f", __FUNCTION__, fKillDeathRate );
		LOG.PrintTimeAndLog(0, "%s - Curr Kill-Death Level : %d", __FUNCTION__, iCalculationLevel );
		LOG.PrintTimeAndLog(0, "%s - Curr Min KillDeathLevel : %d", __FUNCTION__, iMinKillDeathLevel );
		LOG.PrintTimeAndLog(0, "%s - Curr KillDeathLevel : %d", __FUNCTION__, iKillDeathLevel );
	}
#endif

	return iKillDeathLevel;
}

bool ioMyInfo::IsSafetyLevel()
{
	// 10킬 이하 용병단 레벨이 보호레벨 이하
	int iTotalKill = m_UserRecord.GetTotalKill();
	if( iTotalKill <= g_LevelMgr.GetLeaveSafetyRoomKo() && GetGradeLevel() + g_LevelMgr.GetAddGradeLevel() < g_LevelMgr.GetRoomEnterSafetyLevel() )
		return true;

	// 킬뎃 레벨이 보호 레벨 이하 용병단 레벨이 보호레벨 이하
	if( GetKillDeathLevel() < g_LevelMgr.GetRoomEnterSafetyLevel() && GetGradeLevel() + g_LevelMgr.GetAddGradeLevel() < g_LevelMgr.GetRoomEnterSafetyLevel() )
		return true;

	return false;
}

int ioMyInfo::GetCurPenaltyPeso() const
{
	int iCurPenaltyPeso = m_iPenaltyPeso;
	if( m_bCurModeSafetyRoom )
	{
		iCurPenaltyPeso *= m_fPenaltyPesoRate;
	}

	return iCurPenaltyPeso;
}

void ioMyInfo::SetKickOutPlaza( int iRoomIndex )
{
	KickOutPlaza kop;
	kop.m_iPlazaIndex    = iRoomIndex;
	kop.m_dwKickOutTimer = FRAMEGETTIME();
	m_vKickOutPlaza.push_back( kop );
}

bool ioMyInfo::IsKickOutPlaza( int iRoomIndex )
{
	int iSize = m_vKickOutPlaza.size();
	if( iSize == 0 )
		return false;

	for(int i = 0;i < iSize;i++)
	{
		KickOutPlaza &kop = m_vKickOutPlaza[i];
		if( kop.m_iPlazaIndex == iRoomIndex )
		{
			if( FRAMEGETTIME() - kop.m_dwKickOutTimer > 180000 )
			{
				m_vKickOutPlaza.erase( m_vKickOutPlaza.begin() + i );
				return false;
			}
			else
				return true;
		}
	}
	return false;
}

void ioMyInfo::SetRoomOutTimer( DWORD dwTimer )
{
	m_dwRoomOutTimer = dwTimer;
	if( m_dwRoomOutTimer == 0 )
		g_GUIMgr.HideWnd( SOLDIER_LIMIT_POPUP_WND );
	else
		g_GUIMgr.ShowWnd( SOLDIER_LIMIT_POPUP_WND );
}

DWORD ioMyInfo::GetRoomOutSec()
{
	if( m_dwRoomOutTimer == 0 ) return -1;

	DWORD dwGap = ( FRAMEGETTIME() - m_dwRoomOutTimer ) / 1000;
	int   iSec  = (int)30 - dwGap;
	if( iSec < 0 )
	{
		// 룸 이탈 패킷 전송
		SP2Packet kPacket( CTPK_EXIT_ROOM );
		kPacket << EXIT_ROOM_CHAR_LIMIT << 0 << true;
		TCPNetwork::SendToServer( kPacket );
		SetRoomOutTimer( 0 );
		return 0;
	}

	return iSec;
}

bool ioMyInfo::IsSortContribute()
{
	static DWORD dwStartTime = 0;
	if( FRAMEGETTIME() - dwStartTime > 1000 )
	{
		dwStartTime = FRAMEGETTIME();
		return true;
	}
	return false;
}

DWORD ioMyInfo::DeleteCharData( int iCharArray )
{
	DWORD dwCharIndex = 0;

	if( COMPARE( iCharArray, 0, GetCharCount() ) )
	{
		// 용병이 추가장비 착용상태면 해제
		for( int i=0; i < MAX_INVENTORY; ++i )
		{
			if( m_pCharData->at(iCharArray).m_data.m_extra_item[i] > 0 )
			{
				m_UserExtraItem.SetCharEquip( m_pCharData->at(iCharArray).m_data.m_extra_item[i], false );
				m_pCharData->at(iCharArray).m_data.m_extra_item[i] = 0;
			}
		}

		// 코스튬 관련 (캐릭터 해고시 코스튬 해제)
		for( int i=0; i<MAX_INVENTORY; ++i )
		{
			int nSlotIndex = m_pCharData->at(iCharArray).m_data.m_EquipCostume[i].m_CostumeIdx;
			m_UserCostume.SetCostumeEquip( nSlotIndex, false );
		}

		// 장착메달 해제
		m_UserMedalItem.ReleaseEquipMedal( m_pCharData->at(iCharArray).m_data.m_class_type );

		m_SoldierSelectUIBeforeLoading.EraseCharList( m_pCharData->at(iCharArray).m_dwIndex );

		g_UserInfoMgr.DeleteCharData( GetPublicID(), m_pCharData->at(iCharArray).m_data.m_class_type );

		if( m_pCharData->at(iCharArray).m_data.m_chExerciseStyle == EXERCISE_RENTAL )
		{
			m_CharRentalData.DeleteRentalData( m_pCharData->at(iCharArray).m_dwIndex );
			RemoveRentalTimeData( m_pCharData->at(iCharArray).m_dwIndex );
		}

		dwCharIndex = m_pCharData->at(iCharArray).m_dwIndex;
		m_pCharData->erase( m_pCharData->begin() + iCharArray );

		if( COMPARE( iCharArray, 0, (int)m_pCurrentCharInfoList->size() ) )
		{
			SAFEDELETE( m_pCurrentCharInfoList->at(iCharArray) );
			m_pCurrentCharInfoList->erase( m_pCurrentCharInfoList->begin() + iCharArray );
		}
		SyncBattleNLadderCharIndex();
	}
	return dwCharIndex;
}

bool ioMyInfo::DeleteExerciseChar( byte chExerciseStyle )
{
	bool bDlete = false;
	for(int i = 0;i < GetCharCount();)
	{
		if( m_pCharData->at(i).m_data.m_chExerciseStyle == chExerciseStyle )
		{
			DeleteCharData( i );
			bDlete = true;
		}
		else
			i++;
	}			

	return bDlete;
}

void ioMyInfo::FixSelectChar()
{
	if( COMPARE( m_select_char_array, 0, GetCharCount() ) )
		return;

	m_select_char_array = 0;
	for(int i = 0;i < GetCharCount();i++)
	{
		if( m_pCharData->at(i).m_data.m_bActive )
		{
			m_select_char_array = i;
			break;
		}
	}
}

void ioMyInfo::ApplyExerciseIndex( SP2Packet &rkPacket )
{
	int iSize = 0;
    rkPacket >> iSize ;
	
	int iCnt = 0;
	for (int i = 0; i < GetCharCount() ; i++)
	{
		if( iCnt >= iSize ) break;
		if( m_pCharData->at(i).m_data.m_chExerciseStyle != EXERCISE_NONE )
		{
			DWORD dwNewIndex = 0;
			rkPacket >> dwNewIndex;

			m_SoldierSelectUIBeforeLoading.ChangeCharIndex( m_pCharData->at(i).m_dwIndex, dwNewIndex );
			m_pCharData->at(i).m_dwIndex = dwNewIndex;
			iCnt++;
		}
	}
}

bool ioMyInfo::IsDeleteExerciseChar( ModeType eType , bool bSafetyLevelRoom )
{
	switch( eType )
	{
	case MT_MYROOM:
	case MT_TRAINING:
	case MT_HEADQUARTERS:
	case MT_HOUSE:
		return false;
	}

	return true;
}

bool ioMyInfo::IsCharExerciseRentalCharGrowthByClassType( int iClassType, int iSlot, BYTE &kGrowthLevel )
{
	if( IsCharExerciseStyleToClassType( iClassType, EXERCISE_RENTAL ) == false )
		return false;

	DWORD dwCharIndex = GetCharIndexByClassType( iClassType );
	int iCharArray = GetCharArray( dwCharIndex );
	if( IsCharExerciseRentalInActive( iCharArray ) )
		return false;

	m_CharRentalData.GetCharGrowth( dwCharIndex, kGrowthLevel, iSlot );
	return true;
}

bool ioMyInfo::IsCharExerciseRentalItemGrowthByClassType( int iClassType, int iSlot, BYTE &kGrowthLevel )
{
	if( IsCharExerciseStyleToClassType( iClassType, EXERCISE_RENTAL ) == false )
		return false;

	DWORD dwCharIndex = GetCharIndexByClassType( iClassType );
	int iCharArray = GetCharArray( dwCharIndex );
	if( IsCharExerciseRentalInActive( iCharArray ) )
		return false;

	m_CharRentalData.GetItemGrowth( dwCharIndex, kGrowthLevel, iSlot );
	return true;
}

int ioMyInfo::GetNoneExerciseCharCount()
{
	int iResult = 0;
	for( int i=0 ; i < GetCharCount() ; i++ )
	{
		if( m_pCharData->at(i).m_data.m_chExerciseStyle == EXERCISE_NONE )
			iResult++;
	}

	return iResult;
}

EventUserManager& ioMyInfo::GetEventUserMgr()
{
	return m_EventUserMgr;
}

bool ioMyInfo::IsCanBuyItem( const ioSetItemInfo *pSetItemInfo )
{
	if( pSetItemInfo == NULL )
		return false;

	// 모든 제한이 해제된다.
	if( GetUserEventType() == USER_TYPE_LIMIT_IGNORE )
		return true;

	bool isCanBuy = false;
	int iMax = pSetItemInfo->GetNeedLevelInfoListCnt();
	for (int i = 0; i < iMax ; i++)
	{
		ioSetItemInfo::NeedLevelType eNeedType = pSetItemInfo->GetNeedLevelType( i );
		int iNeedLevel = pSetItemInfo->GetNeedLevel( i );
		isCanBuy = false;

		if( iNeedLevel == -1 )
			break;

		if( eNeedType == ioSetItemInfo::NLT_GRADE )
		{
			if( GetGradeLevel() >= iNeedLevel )
				isCanBuy = true;
		}
		else if( eNeedType == ioSetItemInfo::NLT_BATTLE )
		{
			if( GetPartyExp() >= g_LevelMgr.GetNextPartyupExp( iNeedLevel-1 ) )
				isCanBuy = true;
		}
		else if( eNeedType == ioSetItemInfo::NLT_AWARD )
		{
			if( GetAwardLevel() >= iNeedLevel )
				isCanBuy = true;
		}

		if( isCanBuy == false )
			break;
	}

	return isCanBuy;
}

void ioMyInfo::GetNeedLevelInfo( IN const ioSetItemInfo *pSetItemInfo , OUT int &riNeedType, OUT int &riNeedLevel )
{
	riNeedType  = ioSetItemInfo::NLT_NONE;
	riNeedLevel = -1;

	if( pSetItemInfo == NULL )
		return;

	// 모든 제한이 해제된다.
	if( GetUserEventType() == USER_TYPE_LIMIT_IGNORE )
		return;

	int iMax = pSetItemInfo->GetNeedLevelInfoListCnt();
	for (int i = 0; i < iMax ; i++)
	{
		ioSetItemInfo::NeedLevelType eNeedType = pSetItemInfo->GetNeedLevelType( i );
		int iNeedLevel = pSetItemInfo->GetNeedLevel( i );

		riNeedLevel = iNeedLevel;
		if( iNeedLevel == -1 )
		{
			riNeedType  = ioSetItemInfo::NLT_NONE;
			return;
		}

		if( eNeedType == ioSetItemInfo::NLT_GRADE )
		{
			if( GetGradeLevel() < iNeedLevel )
			{
				riNeedType = ioSetItemInfo::NLT_GRADE;
				return;
			}
		}
		else if( eNeedType == ioSetItemInfo::NLT_BATTLE )
		{
			if( GetPartyExp() < g_LevelMgr.GetNextPartyupExp( iNeedLevel-1 ) )
			{
				riNeedType = ioSetItemInfo::NLT_BATTLE;
				return;
			}
		}
		else if( eNeedType == ioSetItemInfo::NLT_AWARD )
		{
			if( GetAwardLevel() < iNeedLevel )
			{
				riNeedType = ioSetItemInfo::NLT_AWARD;
				return;
			}
		}
	}
}

void ioMyInfo::GetFinalResultChar( IntVec &rvClassTypeList )
{
	int i = 0;
	IntVec vClassType;
	for(i = 0;i < GetCharCount();i++)
	{
		if( m_pCharData->at(i).m_data.m_chExerciseStyle == EXERCISE_GENERAL ) continue;
		if( !m_pCharData->at(i).m_data.m_bActive ) continue;
		
		int iClassType = m_pCharData->at(i).m_data.m_class_type;
		int iSize = rvClassTypeList.size();

		for(int j = 0;j < iSize;j++)
		{
			if( rvClassTypeList[j] == iClassType )
			{
				iClassType = 0;
				break;
			}
		}

		if( iClassType != 0 )
			vClassType.push_back( iClassType );
	}
	if( vClassType.empty() ) return;

	for(i = 0;i < m_iCurMaxCharSlot;i++)
	{
		int iClassType = vClassType[0];
		rvClassTypeList.push_back( iClassType );

		vClassType.erase( vClassType.begin() );
		if( vClassType.empty() ) return;
	}

	vClassType.clear();
}

void ioMyInfo::InitLevelUPData()
{
	m_vLevelUPList.clear();
}

void ioMyInfo::SetLevelUP( int iBonusType, int iValue1, int iValue2, int iValue3 )
{
	if( iBonusType == CLASS_UP_BONUS_PESO )
	{
		// 동일한 용병이 여러번 레벨업을하면 보너스 페소를 합쳐서 보여준다.
		int iSize = m_vLevelUPList.size();
		for(int i = 0;i < iSize;i++)
		{
			LevelUPData &kInfo = m_vLevelUPList[i];
			if( kInfo.m_iBonusType == iBonusType && 
				kInfo.m_iValue1 == iValue1 )
			{
				kInfo.m_iValue2 += iValue2;
				return;
			}
		}		
	}
	LevelUPData kInfo;
	kInfo.m_iBonusType = iBonusType;
	kInfo.m_iValue1    = iValue1;
	kInfo.m_iValue2    = iValue2;
	kInfo.m_iValue3    = iValue3;
	m_vLevelUPList.push_back( kInfo );

#if defined( USE_GA )
	// PESO_GET_LV_UP
	g_HttpMng.GA_EventTracking( g_MyInfo.GetUserIndex()
		, "Peso"
		, "LevelUp"
		, ""
		, iValue2
		, "%2FPESO_GET_LV_UP" );
#endif
}

bool ioMyInfo::IsGradeLevelUP()
{
	int iSize = m_vLevelUPList.size();
	for(int i = 0;i < iSize;i++)
	{
		LevelUPData &kInfo = m_vLevelUPList[i];
		if( kInfo.m_iBonusType == GRADE_UP_BONUS_PESO  ||
			kInfo.m_iBonusType == GRADE_UP_BONUS_CLASS ||
			kInfo.m_iBonusType == GRADE_UP_BONUS_ETCITEM ) 
			return true;
	}
	return false;
}

void ioMyInfo::GetGradeUPBonusInfo( int &iBonusType, int &iBonusValue1, int &iBonusValue2 )
{
	iBonusType = iBonusValue1 = iBonusValue2 = 0;
	int iSize = m_vLevelUPList.size();
	for(int i = 0;i < iSize;i++)
	{
		LevelUPData &kInfo = m_vLevelUPList[i];
		if( kInfo.m_iBonusType == GRADE_UP_BONUS_CLASS || kInfo.m_iBonusType == GRADE_UP_BONUS_PESO ) 
		{
			iBonusType   = kInfo.m_iBonusType;
			iBonusValue1 = kInfo.m_iValue2;
			iBonusValue2 = kInfo.m_iValue3;
			return;
		}
	}
}

bool ioMyInfo::IsClassLevelUP( int iClassType )
{
	int iSize = m_vLevelUPList.size();
	for(int i = 0;i < iSize;i++)
	{
		LevelUPData &kInfo = m_vLevelUPList[i];
		if( kInfo.m_iBonusType == CLASS_UP_BONUS_PESO ||
			kInfo.m_iBonusType == CLASS_UP_BONUS_CLASS )
		{
			if( kInfo.m_iValue1 == iClassType )
			{				
				return true;
			}
		}
	}
	return false;
}

int ioMyInfo::GetClassLevelUPPeso( int iClassType )
{
	int iSize = m_vLevelUPList.size();
	for(int i = 0;i < iSize;i++)
	{
		LevelUPData &kInfo = m_vLevelUPList[i];
		if( kInfo.m_iBonusType == CLASS_UP_BONUS_PESO )
		{
			if( kInfo.m_iValue1 == iClassType )
				return kInfo.m_iValue2;
		}
	}
	return 0;
}

bool ioMyInfo::IsGradeLevelUPEtcItem()
{
	int iSize = m_vLevelUPList.size();
	for(int i = 0;i < iSize;i++)
	{
		LevelUPData &kInfo = m_vLevelUPList[i];
		if( kInfo.m_iBonusType == GRADE_UP_BONUS_ETCITEM ) 
			return true;
	}
	return false;
}

bool ioMyInfo::IsExpertEntryTemporary()
{
	if( GetEntryType() == ET_TEMPORARY && !IsSafetyLevel() )
		return true;

	return false;
}

bool ioMyInfo::IsEntryFormality() const
{
	if( GetEntryType() == ET_FORMALITY || 
		GetEntryType() == ET_FORMALITY_CASH )
	{
		return true;
	}

	return false;
}

void ioMyInfo::SetPCRoomAuthority( DWORD dwPCRoomAuthority, int iExcercisePCRoomCharMax )
{
	DWORD dwPrevPCRoom = m_dwPCRoomAuthority;
	m_dwPCRoomAuthority = dwPCRoomAuthority;
	m_iExcercisePCRoomCharMax = iExcercisePCRoomCharMax;

	if(m_dwPCRoomAuthority == FREEDAY_EVENT_CODE)
	{
		m_bUserHeroEvent = true;
	}
	else
	{
		m_bUserHeroEvent = false;
	}
	
	if( dwPrevPCRoom == m_dwPCRoomAuthority )
		return;

	// 완료 조건
	if( m_dwPCRoomAuthority && m_dwPCRoomAuthority != FREEDAY_EVENT_CODE)
	{
		g_QuestMgr.QuestCompleteTerm( QC_PCROOM_LOGIN, m_iLogOutLaterSec );  // 로그인 체크
	}
}

ioUIRenderImage *ioMyInfo::GetSoldierIcon( int iClassType, bool bMale )
{
	enum { MAX_ICON = 18, };        //한페이지에 용병 아이콘 18명씩(남/여) 포함됨
	char szBuf[MAX_PATH] = "";
	if( bMale )
		StringCbPrintf( szBuf, sizeof(szBuf), "SoldierIconPack%.3d#%.3d", ( ( iClassType - 1 ) / MAX_ICON ) + 1, iClassType );
	else
		StringCbPrintf( szBuf, sizeof(szBuf), "SoldierIconPack%.3d#%.3df", ( ( iClassType - 1 ) / MAX_ICON ) + 1, iClassType );

	ioUIRenderImage *pSoldierIcon = g_UIImageSetMgr.CreateImageByFullName( szBuf );
	if( !pSoldierIcon )
	{
		StringCbCopy( szBuf, sizeof( szBuf ), "SoldierIconPack001#000" );
		pSoldierIcon = g_UIImageSetMgr.CreateImageByFullName( szBuf );
	}
	return pSoldierIcon;
}

const ioHashString &ioMyInfo::GetSoldierIconName( int iClassType, bool bMale )
{
	static ioHashString szReturn;

	enum { MAX_ICON = 18, };        //한페이지에 용병 아이콘 18명씩(남/여) 포함됨
	char szBuf[MAX_PATH] = "";
	if( bMale )
		StringCbPrintf( szBuf, sizeof(szBuf), "SoldierIconPack%.3d#%.3d", ( ( iClassType - 1 ) / MAX_ICON ) + 1, iClassType );
	else
		StringCbPrintf( szBuf, sizeof(szBuf), "SoldierIconPack%.3d#%.3df", ( ( iClassType - 1 ) / MAX_ICON ) + 1, iClassType );

	szReturn = szBuf;
	return szReturn;
}

ioUIRenderImage *ioMyInfo::GetMySoldierIcon( int iClassType )
{
	return GetSoldierIcon( iClassType, IsClassTypeMale( iClassType ) );
}

ioHashString &ioMyInfo::GetMySoldierIconName( int iClassType )
{
	static ioHashString szReturn;

	enum { MAX_ICON = 18, };        //한페이지에 용병 아이콘 18명씩(남/여) 포함됨
	char szBuf[MAX_PATH] = "";
	if( IsClassTypeMale( iClassType ) )
		StringCbPrintf( szBuf, sizeof(szBuf), "SoldierIconPack%.3d#%.3d", ( ( iClassType - 1 ) / MAX_ICON ) + 1, iClassType );
	else
		StringCbPrintf( szBuf, sizeof(szBuf), "SoldierIconPack%.3d#%.3df", ( ( iClassType - 1 ) / MAX_ICON ) + 1, iClassType );
	szReturn = szBuf;
	return szReturn;
}

ioUIRenderImage *ioMyInfo::GetSoldierSubIcon( int iClassType )
{
	return g_UIImageSetMgr.CreateImageByFullName( GetSoldierSubIconName( iClassType ) );
}

ioHashString &ioMyInfo::GetSoldierSubIconName( int iClassType )
{
	static ioHashString szReturn;

	const ioSetItemInfo *pInfo = g_SetItemInfoMgr.GetSetInfoByClass( iClassType );
	if( pInfo )
		szReturn = pInfo->GetSubIconName();

	return szReturn;
}

// For Praise : Contribute
void ioMyInfo::InitPraise()
{
	ioINILoader_e kLoader( "config/sp2_praise.ini" );

	// Contribute
	kLoader.SetTitle_e( "contribute" );
	m_iDefaultContribute = kLoader.LoadInt_e( "default_contribute", 110 );
	m_iMaxContribute = kLoader.LoadInt_e( "max_contribute", 200 );
	m_iIncreaseContribute = kLoader.LoadInt_e( "increase_contribute", 10 );

	m_iCurContribute = m_iDefaultContribute;
}

// Contribute
int ioMyInfo::CheckContributePraise( int iNewContribute )
{
	if( m_iMaxContribute > 0 && iNewContribute > m_iMaxContribute )
		return -1;

	if( m_iIncreaseContribute <= 0 )
		return -1;

	bool bChange = false;
	while( iNewContribute >= m_iCurContribute )
	{
		m_iCurContribute += m_iIncreaseContribute;
		bChange = true;
	}

	if( bChange )
	{
		int iPraiseContribute = m_iCurContribute - m_iIncreaseContribute;
		return iPraiseContribute;
	}

	return -1;
}

ChannelingType ioMyInfo::GetChannelingType() const
{
	return m_user_data.m_eChannelingType;
}

const ioHashString & ioMyInfo::GetChannelingUserID() const
{
	return m_user_data.m_szChannelingUserID;
}

int ioMyInfo::GetChannelingCash() const
{
	return m_user_data.m_iChannelingCash;
}

int ioMyInfo::GetTotalCash()
{
	return m_user_data.m_cash + m_user_data.m_iChannelingCash + m_user_data.m_iBonusCash;
}

BlockType ioMyInfo::GetBlockType() const
{
	return m_user_data.m_eBlockType;
}

DWORD ioMyInfo::GetBlockYearMonthDay() const
{
	return m_user_data.m_dwBlockYearMonthDay;
}

DWORD ioMyInfo::GetBlockHourMinute() const
{
	return m_user_data.m_dwBlockHourMinute;
}

float ioMyInfo::GetBlockPointPer()
{
	if( m_user_data.m_eBlockType == BKT_NORMAL )
		return FLOAT1;
	else if( m_user_data.m_eBlockType == BKT_WARNNING )
		return 0.7f;
	else if( m_user_data.m_eBlockType == BKT_LIMITATION )
		return FLOAT05;

	return 0.0f; 
}

void ioMyInfo::LoadCharSlotInfo()
{
	ioINILoader_e kLoader( "config/sp2_CharSlotInfo.ini" );
	kLoader.SetTitle_e( "common" );

	m_iLimiteMaxCharSlot = kLoader.LoadInt_e( "limite_max_char_slot", 12 );
	m_iDefaultMaxCharSlot = kLoader.LoadInt_e( "default_max_char_slot", 12 );

	m_iDefaultMaxCharSlot = min( m_iDefaultMaxCharSlot, m_iLimiteMaxCharSlot );
	m_iCurMaxCharSlot = m_iDefaultMaxCharSlot;

	kLoader.SetTitle_e( "arena" );

	m_iArenaMaxCharSlot = kLoader.LoadInt_e( "default_max_arena_char_slot", 36 );
}

void ioMyInfo::CheckCurMaxCharSlot()
{
	m_iCurMaxCharSlot = m_iDefaultMaxCharSlot;

	// 슬롯 확장 아이템 체크
	// m_iCurMaxCharSlot += 확장된 슬롯수
	ioUserEtcItem::ETCITEMSLOT kSlot;
	if( !m_UserEtcItem.GetEtcItem( ioEtcItem::EIT_ETC_CHAR_SLOT_EXTEND, kSlot ) )
	{
		return;
	}

	m_iCurMaxCharSlot += kSlot.GetUse();
	m_iCurMaxCharSlot = min( m_iCurMaxCharSlot, m_iLimiteMaxCharSlot );
}

const int ioMyInfo::GetCurMaxCharSlot()
{
	if( ioPlayMode::GetModeType() == MT_ARENA && g_BattleRoomMgr.IsBattleModePlaying() )
	{
		return m_iArenaMaxCharSlot;
	}
	else
	{
		return m_iCurMaxCharSlot;
	}
}

int ioMyInfo::GetCurMaxFishingInventory()
{
	return m_UserFishItem.GetCurMaxInventory();
}

bool ioMyInfo::CheckFishingRod()
{
	if( m_iCurFishingRod == 0 )
		return false;

	ioUserEtcItem::ETCITEMSLOT kSlot;
	if( !m_UserEtcItem.GetEtcItem( m_iCurFishingRod, kSlot ) )
		return false;

	time_t kLimitTime = DateHelp::ConvertSecondTime( kSlot.GetYear(), kSlot.GetMonth(), kSlot.GetDay(), kSlot.GetHour(), kSlot.GetMinute(), 0 );
	time_t kCurServerTime = GetServerDate();
	kCurServerTime += ( REALGETTIME()/1000 );// 현재까지의 클라이언트초를 넣는다.

	int iGapSec = kLimitTime - kCurServerTime;
	enum { DAY_SEC = 86400, HOUR_SEC = 3600, MINUTE_SEC = 60, };
	int iDay    = iGapSec/DAY_SEC;
	int iHourSec= iGapSec-(iDay*DAY_SEC);
	int iHour   = iHourSec / HOUR_SEC;
	int iMinute = ( iHourSec-(iHour*HOUR_SEC) ) / MINUTE_SEC;

	if( iDay <= 0 && iHour <= 0 && iMinute <= 0 )
		return false;

	return true;
}

int ioMyInfo::GetCurFishingBaitCnt()
{
	if( m_iCurFishingBait == 0 )
		return 0;

	ioUserEtcItem::ETCITEMSLOT kSlot;
	if( !m_UserEtcItem.GetEtcItem( m_iCurFishingBait, kSlot ) )
		return 0;

	return kSlot.GetUse();
}

void ioMyInfo::AddFishingItem( BYTE iType, int iIndexArray )
{
	m_UserFishItem.AddFishItemData( iType, iIndexArray );
}

void ioMyInfo::RemoveFishingItem( int iIndexArray )
{
	m_UserFishItem.DeleteFishItemData( iIndexArray );
}

int ioMyInfo::GetCurFishingItemCnt()
{
	return m_UserFishItem.GetCurFishItemCnt();
}

bool ioMyInfo::IsFishingItemMax()
{
	int iSize = m_UserFishItem.GetCurFishItemCnt();

	if( iSize >= GetCurMaxFishingInventory() )
		return true;

	return false;
}

int ioMyInfo::GetFishingItemType( int iListArray )
{
	return m_UserFishItem.GetFishItemType( iListArray );
}

int ioMyInfo::GetFishingItemArray( int iListArray )
{
	return m_UserFishItem.GetFishItemArray( iListArray );
}

int ioMyInfo::GetFishingRodItemType( int iListArray )
{
	return m_UserFishItem.GetFishingRodItemType( iListArray );
}

int ioMyInfo::GetFishingBaitItemType( int iListArray )
{
	return m_UserFishItem.GetFishingBaitItemType( iListArray );
}

int ioMyInfo::GetFishingGroundItemType( int iListArray )
{
	return m_UserFishItem.GetFishingGroundItemType( iListArray );
}

void ioMyInfo::ChangeFishingGroundItemOwn( int iListArray, bool bOwn )
{
	m_UserFishItem.ChangeFishingGroundItemOwn( iListArray, bOwn );
}

bool ioMyInfo::GetFishingGearOwnByTabType( int iListArray, FishingGearType eFishingGearType )
{
	return m_UserFishItem.GetFishingGearOwnByTabType( iListArray, eFishingGearType );
}

bool ioMyInfo::GetFishingGearOwnByEtcCode( int iEtcCode, FishingGearType eFishingGearType )
{
	return m_UserFishItem.GetFishingGearOwnByEtcCode( iEtcCode, eFishingGearType );
}

bool ioMyInfo::CheckUseFishingGear( int iListArray, FishingGearType eFishingGearType )
{
	DWORD dwEtcItemCode = m_UserFishItem.GetFishingGearEtcCodeByTabType( iListArray, eFishingGearType );

	if( dwEtcItemCode == m_iCurFishingRod && eFishingGearType == FGT_ROD )
		return true;
	else if( dwEtcItemCode == m_iCurFishingBait && eFishingGearType == FGT_BAIT )
		return true;
	else if( g_PushStructListMgr.FindFishingGroundByEtcCode( GetPublicID(), dwEtcItemCode ) && eFishingGearType == FGT_FISHING_GROUND )
		return true;

	return false;
}

DWORD ioMyInfo::GetSelectFishingGearEtcCode( int iListArray, FishingGearType eFishingGearType )
{
	return m_UserFishItem.GetFishingGearEtcCodeByTabType( iListArray, eFishingGearType );
}

int ioMyInfo::GetFishingGearCountByTabType( FishingGearType eFishingGearType )
{
	return m_UserFishItem.GetFishingGearCountByTabType( eFishingGearType );
}

void ioMyInfo::AlignFishingGearByOwn()
{
	m_UserFishItem.AlignFishingGearByOwn();
}

DWORD ioMyInfo::GetFishingRodByFirstOrder()
{
	return m_UserFishItem.GetFishingRodByFirstOrder();
}

DWORD ioMyInfo::GetFishingBaitByFirstOrder()
{
	return m_UserFishItem.GetFishingBaitByFirstOrder();
}

void ioMyInfo::SetFirstFishingRod_Bait( int iCurFishingRod, int iCurFishingBait, bool bNoneSelect )
{
	if( !bNoneSelect )
	{
		if( iCurFishingRod == 0)
			m_iCurFishingRod = (int)m_UserFishItem.GetFishingRodByFirstOrder();
		else
			m_iCurFishingRod = iCurFishingRod;

		if( iCurFishingBait == 0 )
			m_iCurFishingBait = (int)m_UserFishItem.GetFishingBaitByFirstOrder();
		else
			m_iCurFishingBait = iCurFishingBait;
	}
	else
	{
		m_iCurFishingRod = 0;
		m_iCurFishingBait = 0;
	}
}

void ioMyInfo::SetFishingLevel( int iLevel, int iExpert )
{
	m_user_data.m_fishing_level = iLevel;
	m_user_data.m_fishing_exp = iExpert;

	g_UserInfoMgr.SetFishingExp( GetPublicID(), GetFishingLevel(), GetFishingExpert() );
}

int ioMyInfo::GetFishingLevel()
{
	return m_user_data.m_fishing_level;
}

int ioMyInfo::GetFishingExpert()
{
	return m_user_data.m_fishing_exp;
}

int ioMyInfo::GetFishingSlotExtendItem()
{
	ioUserEtcItem::ETCITEMSLOT kSlot;
	if( !m_UserEtcItem.GetEtcItem( ioEtcItem::EIT_ETC_FISHING_SLOT_EXTEND, kSlot ) )
	{
		return 0;
	}

	return kSlot.GetUse();
}

int ioMyInfo::GetCurEnableFishingRod()
{
	ioUserEtcItem::ETCITEMSLOT kRodSlot;
	if( m_UserEtcItem.GetEtcItem( ioEtcItem::EIT_ETC_PCROOM_FISHING_ROD, kRodSlot ) )
		return ioEtcItem::EIT_ETC_PCROOM_FISHING_ROD;

	if( m_UserEtcItem.GetEtcItem( ioEtcItem::EIT_ETC_FISHING_MOON_ROD, kRodSlot ) )
	{
		time_t kLimitTime = DateHelp::ConvertSecondTime( kRodSlot.GetYear(), kRodSlot.GetMonth(), kRodSlot.GetDay(), kRodSlot.GetHour(), kRodSlot.GetMinute(), 0 );
		time_t kCurServerTime = GetServerDate();
		kCurServerTime += ( REALGETTIME()/1000 );// 현재까지의 클라이언트초를 넣는다.

		int iGapSec = kLimitTime - kCurServerTime;
		if( iGapSec > 0 )
			return ioEtcItem::EIT_ETC_FISHING_MOON_ROD;
	}

	if( m_UserEtcItem.GetEtcItem( ioEtcItem::EIT_ETC_FISHING_ROD, kRodSlot ) )
	{
		time_t kLimitTime = DateHelp::ConvertSecondTime( kRodSlot.GetYear(), kRodSlot.GetMonth(), kRodSlot.GetDay(), kRodSlot.GetHour(), kRodSlot.GetMinute(), 0 );
		time_t kCurServerTime = GetServerDate();
		kCurServerTime += ( REALGETTIME()/1000 );// 현재까지의 클라이언트초를 넣는다.

		int iGapSec = kLimitTime - kCurServerTime;
		if( iGapSec > 0 )
			return ioEtcItem::EIT_ETC_FISHING_ROD;
	}

	return 0;
}

void ioMyInfo::CheckCurFishingTackle( bool bFirst )
{
	if( bFirst )
	{
		m_iCurFishingRod = 0;
		m_iCurFishingBait = 0;

		// Fishing Rod
		//m_iCurFishingRod = GetCurEnableFishingRod();

		// Fishing Bait
		ioUserEtcItem::ETCITEMSLOT kBaitSlot;
		if( m_UserEtcItem.GetEtcItem( ioEtcItem::EIT_ETC_PCROOM_FISHING_BAIT, kBaitSlot ) )
			m_iCurFishingBait = ioEtcItem::EIT_ETC_PCROOM_FISHING_BAIT;
		else if( m_UserEtcItem.GetEtcItem( ioEtcItem::EIT_ETC_FISHING_MOON_BAIT, kBaitSlot ) )
			m_iCurFishingBait = ioEtcItem::EIT_ETC_FISHING_MOON_BAIT;
		else if( m_UserEtcItem.GetEtcItem( ioEtcItem::EIT_ETC_FISHING_BAIT, kBaitSlot ) )
			m_iCurFishingBait = ioEtcItem::EIT_ETC_FISHING_BAIT;
	}
	else
	{
		bool bRodChange, bBaitChange;
		bRodChange = bBaitChange = false;

		int iPreRod = m_iCurFishingRod;
		int iPreBait = m_iCurFishingBait;

		ioHashString szPreRod, szCurRod, szPreBait, szCurBait;
		ioEtcItem *pItem = NULL;

		// Fishing Rod
		ioUserEtcItem::ETCITEMSLOT kRodSlot;
		if( !m_UserEtcItem.GetEtcItem( m_iCurFishingRod, kRodSlot ) )
		{
			//m_iCurFishingRod = GetCurEnableFishingRod();

			if( iPreRod != m_iCurFishingRod && iPreRod > 0 && m_iCurFishingRod > 0 )
			{
				pItem = g_EtcItemMgr.FindEtcItem( iPreRod );
				if( pItem )
					szPreRod = pItem->GetName();

				pItem = g_EtcItemMgr.FindEtcItem( m_iCurFishingRod );
				if( pItem )
					szCurRod = pItem->GetName();

				bRodChange = true;
			}
		}
		else
		{
			//m_iCurFishingRod = GetCurEnableFishingRod();

			if( iPreRod != m_iCurFishingRod && iPreRod > 0 && m_iCurFishingRod > 0 )
			{
				pItem = g_EtcItemMgr.FindEtcItem( iPreRod );
				if( pItem )
					szPreRod = pItem->GetName();

				pItem = g_EtcItemMgr.FindEtcItem( m_iCurFishingRod );
				if( pItem )
					szCurRod = pItem->GetName();

				bRodChange = true;
			}
		}

		// Fishing Bait
		ioUserEtcItem::ETCITEMSLOT kBaitSlot;
		if( !m_UserEtcItem.GetEtcItem( m_iCurFishingBait, kBaitSlot ) )
		{
			DWORD dwBaitEtcCode = m_UserFishItem.GetAscendingOrderFishingBaitEtcCodeByOwn();
			if( m_UserEtcItem.GetEtcItem( dwBaitEtcCode, kBaitSlot ) )
				m_iCurFishingBait = dwBaitEtcCode;

			if( dwBaitEtcCode == 0 )
				m_iCurFishingBait = 0;

			FishingInventoryWnd *pFishingInventoryWnd = dynamic_cast<FishingInventoryWnd*> (g_GUIMgr.FindWnd(FISHING_INVENTORY_WND));
			if( pFishingInventoryWnd && pFishingInventoryWnd->IsShow() )
				pFishingInventoryWnd->ChangeUsingFishingBait();

			if( iPreBait > 0 && m_iCurFishingBait > 0 )
			{
				pItem = g_EtcItemMgr.FindEtcItem( iPreBait );
				if( pItem )
					szPreBait = pItem->GetName();

				pItem = g_EtcItemMgr.FindEtcItem( m_iCurFishingBait );
				if( pItem )
					szCurBait = pItem->GetName();

				bBaitChange = true;
			}
		}
		else
		{
			if( kBaitSlot.GetUse() <= 0 )
			{
				DWORD dwBaitEtcCode = 0;
				dwBaitEtcCode = m_UserFishItem.GetAscendingOrderFishingBaitEtcCodeByOwn();
				if( m_UserEtcItem.GetEtcItem( dwBaitEtcCode, kBaitSlot ) )
					m_iCurFishingBait = dwBaitEtcCode;

				if( dwBaitEtcCode == 0 )
					m_iCurFishingBait = 0;

				FishingInventoryWnd *pFishingInventoryWnd = dynamic_cast<FishingInventoryWnd*> (g_GUIMgr.FindWnd(FISHING_INVENTORY_WND));
				if( pFishingInventoryWnd && pFishingInventoryWnd->IsShow() )
					pFishingInventoryWnd->ChangeUsingFishingBait();

				if( iPreBait > 0 && m_iCurFishingBait > 0 )
				{
					pItem = g_EtcItemMgr.FindEtcItem( iPreBait );
					if( pItem )
						szPreBait = pItem->GetName();

					pItem = g_EtcItemMgr.FindEtcItem( m_iCurFishingBait );
					if( pItem )
						szCurBait = pItem->GetName();

					bBaitChange = true;
				}
			}
		}

		ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
		kPrinter[0].SetTextStyle( TS_NORMAL );
		kPrinter[0].SetBkColor( 0, 0, 0 );
		kPrinter[1].SetTextStyle( TS_NORMAL );
		kPrinter[1].SetBkColor( 0, 0, 0 );

		if( bRodChange && bBaitChange )
		{
			kPrinter[0].SetTextColor( TCT_DEFAULT_GRAY );
			kPrinter[0].AddTextPiece( FONT_SIZE_13, STR(1), szPreBait.c_str(), szCurBait.c_str() );

			kPrinter[1].SetTextColor( TCT_DEFAULT_GRAY );
			kPrinter[1].AddTextPiece( FONT_SIZE_13, STR(2), szPreRod.c_str(), szCurRod.c_str() );
			
			g_GUIMgr.SetPrevMsgListBox( NULL, MB_OK, NULL, kPrinter );
			return;
		}
		else if( !bRodChange && bBaitChange )
		{
			kPrinter[0].SetTextColor( TCT_DEFAULT_GRAY );
			kPrinter[0].AddTextPiece( FONT_SIZE_13, STR(3), szPreBait.c_str(), szCurBait.c_str() );
			
			g_GUIMgr.SetPrevMsgListBox( NULL, MB_OK, NULL, kPrinter );
			return;
		}
		else if( bRodChange && !bBaitChange )
		{
			kPrinter[0].SetTextColor( TCT_DEFAULT_GRAY );
			kPrinter[0].AddTextPiece( FONT_SIZE_13, STR(4), szPreRod.c_str(), szCurRod.c_str() );

			g_GUIMgr.SetPrevMsgListBox( NULL, MB_OK, NULL, kPrinter );
			return;
		}
	}
}

bool ioMyInfo::SetCurFishingRod( int iType )
{
	if( iType != m_iCurFishingRod )
	{
		m_iCurFishingRod = iType;
		return true;
	}

	return false;
}

bool ioMyInfo::SetCurFishingBait( int iType )
{
	if( iType != m_iCurFishingBait )
	{
		m_iCurFishingBait = iType;
		return true;
	}

	return false;
}

int ioMyInfo::GetCurFishingRod()
{
	return m_iCurFishingRod;
}

int ioMyInfo::GetCurFishingBait()
{
	return m_iCurFishingBait;
}

bool ioMyInfo::HasEtcItem( DWORD dwEtcItemCode )
{
	ioUserEtcItem::ETCITEMSLOT kBaitSlot;
	if( m_UserEtcItem.GetEtcItem( dwEtcItemCode, kBaitSlot ) )
		return true;
	return false;
}

void ioMyInfo::ApplyChangeExtraItem( int iCharArray, int iSlot, int iNewSlotIndex )
{
	if( !COMPARE( iCharArray, 0, GetCharCount() ) ) 
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1));
		LOG.PrintTimeAndLog( 0, "ioMyInfo::ApplyChangeExtraItem Wrong Array : %d", iCharArray );
		return;
	}
		
	if( iNewSlotIndex == 0 )		// 해제
	{
		// Char ExtraSlot
		int iPreIndex = m_pCharData->at(iCharArray).m_data.m_extra_item[iSlot];
		m_pCharData->at(iCharArray).m_data.m_extra_item[iSlot] = 0;

		// power up check
		int iItemCode = 0;
		byte  eCharPowerUpType = m_pCharData->at(iCharArray).m_data.m_eCharPowerUpType;
		if( eCharPowerUpType != PUGT_NONE )
		{
			iItemCode = (DEFAULT_BASIC_ITEM_CODE * iSlot) + ConvertPowerUpCharToItem( eCharPowerUpType ) + m_pCharData->at(iCharArray).m_data.m_class_type;
			if( !g_PowerUpManager.IsPowerUpGradeItem( m_pCharData->at(iCharArray).m_data.m_class_type, iItemCode ) || !g_ItemMaker.IsItem( iItemCode ) )
			{
				// Char Base EquipSlot
				iItemCode = (DEFAULT_BASIC_ITEM_CODE * iSlot) + m_pCharData->at(iCharArray).m_data.m_class_type;
			}
		}
		else
		{
			// Char Base EquipSlot
			iItemCode = (DEFAULT_BASIC_ITEM_CODE * iSlot) + m_pCharData->at(iCharArray).m_data.m_class_type;
		}

		m_pCharData->at(iCharArray).m_equip_item[iSlot].m_item_code				= iItemCode;
		m_pCharData->at(iCharArray).m_equip_item[iSlot].m_item_reinforce		= 0;
		m_pCharData->at(iCharArray).m_equip_item[iSlot].m_item_male_custom		= 0;
		m_pCharData->at(iCharArray).m_equip_item[iSlot].m_item_female_custom	= 0;
		
		// ExtraItem Inventory Equip
		if( iPreIndex > 0 )
			m_UserExtraItem.SetCharEquip( iPreIndex, false );
	}
	else							// 장착
	{
		EXTRAITEMSLOT kSlot;
		if( !m_UserExtraItem.GetExtraItem( iNewSlotIndex, kSlot ) )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2));
			LOG.PrintTimeAndLog( 0, "ioMyInfo::ApplyChangeExtraItem Wrong SlotIndex : %d", iNewSlotIndex );
			return;
		}

		int iItemCode = kSlot.m_iItemCode;
		iSlot = iItemCode / DEFAULT_BASIC_ITEM_CODE;

		if( !COMPARE( iSlot, 0, MAX_INVENTORY ) )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3));
			LOG.PrintTimeAndLog( 0, "ioMyInfo::ApplyChangeExtraItem Wrong Slot : %d", iSlot );
			return;
		}

		// Char ExtraSlot
		int iPreIndex = m_pCharData->at(iCharArray).m_data.m_extra_item[iSlot];
		m_pCharData->at(iCharArray).m_data.m_extra_item[iSlot] = iNewSlotIndex;

		// Char Base EquipSlot
		m_pCharData->at(iCharArray).m_equip_item[iSlot].m_item_code = kSlot.m_iItemCode;
		m_pCharData->at(iCharArray).m_equip_item[iSlot].m_item_reinforce = kSlot.m_iReinforce;
		m_pCharData->at(iCharArray).m_equip_item[iSlot].m_item_male_custom = kSlot.m_dwMaleCustom;
		m_pCharData->at(iCharArray).m_equip_item[iSlot].m_item_female_custom = kSlot.m_dwFemaleCustom;

		kSlot.m_iTradeState = EET_DISABLE;
		m_UserExtraItem.SetExtraItem( kSlot );

		// ExtraItem Inventory Equip
		if( iPreIndex > 0 )
			m_UserExtraItem.SetCharEquip( iPreIndex, false );

		m_UserExtraItem.SetCharEquip( iNewSlotIndex, true );
	}

	m_SoldierSelectUIBeforeLoading.Update( m_pCharData->at(iCharArray) );

	SerUserInfoCharData( m_pCharData->at(iCharArray).m_data.m_class_type );
	
	// 장비를 변경했다.
	if( g_BattleRoomMgr.IsBattleRoom() )
	{
		if( g_BattleRoomMgr.GetUserCharIndex( GetPublicID() ) == m_pCharData->at(iCharArray).m_dwIndex )
		{
			SP2Packet kPacket( CUPK_SYNC_CHARINFO );
			kPacket << GetPublicID() << m_pCharData->at(iCharArray).m_dwIndex << m_pCharData->at(iCharArray).m_data;
			FillCharItemData( m_pCharData->at(iCharArray).m_dwIndex, kPacket );
			P2PNetwork::SendToAllUser( kPacket );
		}		
	}
	else if( g_LadderTeamMgr.IsLadderTeam() )
	{
		if( g_LadderTeamMgr.GetUserCharIndex( GetPublicID() ) == m_pCharData->at(iCharArray).m_dwIndex )
		{
			SP2Packet kPacket( CUPK_SYNC_CHARINFO );
			kPacket << GetPublicID() << m_pCharData->at(iCharArray).m_dwIndex << m_pCharData->at(iCharArray).m_data;
			FillCharItemData( m_pCharData->at(iCharArray).m_dwIndex, kPacket );
			P2PNetwork::SendToAllUser( kPacket );
		}	
	}
	else if( g_ShuffleRoomMgr.IsShuffleRoom() )
	{
		if( g_ShuffleRoomMgr.GetUserCharIndex( GetPublicID() ) == m_pCharData->at(iCharArray).m_dwIndex )
		{
			SP2Packet kPacket( CUPK_SYNC_CHARINFO );
			kPacket << GetPublicID() << m_pCharData->at(iCharArray).m_dwIndex << m_pCharData->at(iCharArray).m_data;
			FillCharItemData( m_pCharData->at(iCharArray).m_dwIndex, kPacket );
			P2PNetwork::SendToAllUser( kPacket );
		}		
	}
}

void ioMyInfo::RefreshExtraItemEquip()
{
	m_UserExtraItem.ClearCharEquip();

	int iSize = GetCharCount();
	for( int iArray=0; iArray < iSize; ++iArray )
	{
		if( m_pCharData->at(iArray).m_data.m_chExerciseStyle == EXERCISE_RENTAL )
		{
			m_CharRentalData.EquipItem( m_pCharData->at(iArray) );
			m_CharRentalData.EquipCostume( m_pCharData->at(iArray) );
		}
		else
		{
			m_UserExtraItem.EquipItem( m_pCharData->at(iArray), true );
		}

		m_SoldierSelectUIBeforeLoading.Update( m_pCharData->at(iArray) );

		SerUserInfoCharData( m_pCharData->at(iArray).m_data.m_class_type );
	}
}

int ioMyInfo::FindEquipedExtraItemChar( int iSlotIndex )
{
	int iSize = GetCharCount();
	for(int i = 0;i < iSize;i++)
	{
		for( int j=0; j < MAX_INVENTORY; ++j )
		{
			if( m_pCharData->at(i).m_data.m_extra_item[j] == iSlotIndex )
				return i;
		}
	}

	return -1;
}

bool ioMyInfo::DeleteExtraItemData( SP2Packet &rkPacket )
{
	int iSize;
	rkPacket >> iSize;

	if( iSize <= 0 )
		return false;

	bool bReturn = false;
	for( int i=0; i < iSize; ++i )
	{
		int iSlotIndex;
		rkPacket >> iSlotIndex;

		// 용병 착용상태면 해제후 인벤에서 제거
		int iCharArray = FindEquipedExtraItemChar( iSlotIndex );

		if( COMPARE( iCharArray, 0, GetCharCount() ) )
		{
			for( int j = 0; j < MAX_INVENTORY; ++j )
			{
				if( m_pCharData->at(iCharArray).m_data.m_extra_item[j] == iSlotIndex )
				{
					m_pCharData->at(iCharArray).m_data.m_extra_item[j] = 0;
					
					int iItemCode = 0;
					if( m_pCharData->at(iCharArray).m_data.m_eCharPowerUpType != PUGT_NONE )
					{
						iItemCode = (DEFAULT_BASIC_ITEM_CODE * j) + ConvertPowerUpCharToItem( m_pCharData->at(iCharArray).m_data.m_eCharPowerUpType ) + m_pCharData->at(iCharArray).m_data.m_class_type;
						if( !g_PowerUpManager.IsPowerUpGradeItem( m_pCharData->at(iCharArray).m_data.m_class_type, iItemCode ) || !g_ItemMaker.IsItem( iItemCode ) )
						{
							iItemCode = (DEFAULT_BASIC_ITEM_CODE * j) + m_pCharData->at(iCharArray).m_data.m_class_type;
						}
					}
					else
					{
						iItemCode = (DEFAULT_BASIC_ITEM_CODE * j) + m_pCharData->at(iCharArray).m_data.m_class_type;
					}
					
					m_pCharData->at(iCharArray).m_equip_item[j].m_item_code				= iItemCode;
					m_pCharData->at(iCharArray).m_equip_item[j].m_item_reinforce		= 0;
					m_pCharData->at(iCharArray).m_equip_item[j].m_item_male_custom		= 0;
					m_pCharData->at(iCharArray).m_equip_item[j].m_item_female_custom	= 0;

					ioHashString szClassName = GetClassName( m_pCharData->at(iCharArray).m_data.m_class_type );

					char szDesc[MAX_PATH] = "";
					EXTRAITEMSLOT kSlot;
					m_UserExtraItem.GetExtraItem( iSlotIndex, kSlot );

					ReleaseNameInfo kInfo;
					kInfo.m_szClassName = szClassName;
					kInfo.m_szItemName  = kSlot.m_Name;
					kInfo.m_szIconName  = kSlot.m_IconName;
					kInfo.m_iReinforce  = kSlot.m_iReinforce;
					kInfo.m_nItemCode	= kSlot.m_iItemCode;

					m_vReserveReleaseInfoList.push_back( kInfo );
				}
			}
			m_SoldierSelectUIBeforeLoading.Update( m_pCharData->at(iCharArray) );

			SerUserInfoCharData( m_pCharData->at(iCharArray).m_data.m_class_type );
		}

		bReturn = m_UserExtraItem.DeleteExtraItemData( iSlotIndex );
	}

	bool bImmediately;
	rkPacket >> bImmediately;

	return bReturn;
}

bool ioMyInfo::IsReserveDeleteExtraItem()
{
	if( m_vReserveReleaseInfoList.empty() )
		return false;
	return true;
}

void ioMyInfo::GetFirstDeleteExtraItem( ioHashString &rkClassName, ioHashString &rkName, ioHashString &rkIconName, int &iReinforce, int &nItemCode )
{
	if( m_vReserveReleaseInfoList.empty() )
		return;

	rkClassName = m_vReserveReleaseInfoList[0].m_szClassName;
	rkName      = m_vReserveReleaseInfoList[0].m_szItemName;
	rkIconName  = m_vReserveReleaseInfoList[0].m_szIconName;
	iReinforce  = m_vReserveReleaseInfoList[0].m_iReinforce;
	nItemCode	= m_vReserveReleaseInfoList[0].m_nItemCode;
}

void ioMyInfo::EraseDeleteExtraItem( bool bAllErase )
{
	if( m_vReserveReleaseInfoList.empty() )
		return;

	if( bAllErase )
		m_vReserveReleaseInfoList.clear();
	else
		m_vReserveReleaseInfoList.erase( m_vReserveReleaseInfoList.begin() );
}

// For Expand Medal Slot
void ioMyInfo::SetReserveDeleteExMedalSlot( ioHashString &szIconName, ioHashString &szSubIconName, ioHashString &szClassName, int iSlotLevel )
{
	ReleaseExMedallSlotInfo kInfo;
	kInfo.m_szIconName  = szIconName;
	kInfo.m_szSubIconName = szSubIconName;
	kInfo.m_szClassName = szClassName;
	kInfo.m_iSlotLevel  = iSlotLevel;
	m_vReserveReleaseExMedalSlotList.push_back( kInfo );
}

bool ioMyInfo::IsReserveDeleteExMedalSlot()
{
	if( m_vReserveReleaseExMedalSlotList.empty() )
		return false;
	return true;
}

void ioMyInfo::GetFirstDeleteExMedalSlot( ioHashString &szIconName, ioHashString &szSubIconName, ioHashString &szClassName, int &iSlotLevel )
{
	if( m_vReserveReleaseExMedalSlotList.empty() )
		return;

	szIconName  = m_vReserveReleaseExMedalSlotList[0].m_szIconName;
	szSubIconName  = m_vReserveReleaseExMedalSlotList[0].m_szSubIconName;
	szClassName = m_vReserveReleaseExMedalSlotList[0].m_szClassName;
	iSlotLevel  = m_vReserveReleaseExMedalSlotList[0].m_iSlotLevel;
}

void ioMyInfo::EraseDeleteExMedalSlot( bool bAllErase )
{
	if( m_vReserveReleaseExMedalSlotList.empty() )
		return;

	if( bAllErase )
		m_vReserveReleaseExMedalSlotList.clear();
	else
		m_vReserveReleaseExMedalSlotList.erase( m_vReserveReleaseExMedalSlotList.begin() );
}

void ioMyInfo::UpdateEquipExtraItem( int iSlotIndex )
{
	for(int i = 0;i < (int)m_pCharData->size();i++)
	{
		for(int k = 0;k < MAX_INVENTORY;k++)
		{
			if( m_pCharData->at(i).m_data.m_extra_item[k] == iSlotIndex )	// 장비 아이템
			{
				EXTRAITEMSLOT kSlot;
				if( m_UserExtraItem.GetExtraItem( m_pCharData->at(i).m_data.m_extra_item[k], kSlot ) )
				{
					m_pCharData->at(i).m_equip_item[k].m_item_code          = kSlot.m_iItemCode;
					m_pCharData->at(i).m_equip_item[k].m_item_reinforce     = kSlot.m_iReinforce;
					m_pCharData->at(i).m_equip_item[k].m_item_bullet        = -1;
					m_pCharData->at(i).m_equip_item[k].m_item_male_custom   = kSlot.m_dwMaleCustom;
					m_pCharData->at(i).m_equip_item[k].m_item_female_custom = kSlot.m_dwFemaleCustom;

					m_UserExtraItem.SetCharEquip( m_pCharData->at(i).m_data.m_extra_item[k], true );
				}
			}
		}
		m_SoldierSelectUIBeforeLoading.Update( m_pCharData->at(i) );

		SerUserInfoCharData( m_pCharData->at(i).m_data.m_class_type );
	}
}

bool ioMyInfo::DeleteExtraItem( int iSlotIndex )
{
	return m_UserExtraItem.DeleteExtraItemData( iSlotIndex );
}

bool ioMyInfo::IsCharEquipExtraItemPeriodCheck( int iClassType, int iPeriodType )
{
	for(int i = 0;i < (int)m_pCharData->size();i++)
	{
		if( m_pCharData->at(i).m_data.m_class_type != iClassType ) continue;

		for(int k = 0;k < MAX_INVENTORY;k++)
		{
			EXTRAITEMSLOT kSlot;
			if( m_UserExtraItem.GetExtraItem( m_pCharData->at(i).m_data.m_extra_item[k], kSlot ) )
			{
				if( kSlot.m_PeriodType == iPeriodType )
					return true;
			}
		}	
	}
	return false;
}

bool ioMyInfo::UseModeStartMonsterCoin( IN int iUseCnt, IN bool bUseGoldCoin, OUT int& nCoinUseCount, OUT int& nGoldCoinUseCount )
{
	if( iUseCnt == 0 ) 
		return true;
	
	int iPrevMonsterCoin = GetEtcMonsterCoin();
	if( iPrevMonsterCoin >= Help::GetRefillCoinMax() )
	{
		m_iRefillCoinCurTime = FRAMEGETTIME();
	}

	ioUserEtcItem::ETCITEMSLOT kMonsterCoinSlot, kGoldMonsterCoinSlot;
	m_UserEtcItem.GetEtcItem( ioEtcItem::EIT_ETC_MONSTER_COIN, kMonsterCoinSlot );
	m_UserEtcItem.GetEtcItem( ioEtcItem::EIT_ETC_GOLDMONSTER_COIN, kGoldMonsterCoinSlot );

	if( bUseGoldCoin )
	{
		if( kMonsterCoinSlot.GetUse() + kGoldMonsterCoinSlot.GetUse() < iUseCnt )
			return false;
	}
	else
	{
		if( kMonsterCoinSlot.GetUse() < iUseCnt )
			return false;
	}

	int nNeedGoldCoinCount = 0;

	// 몬스터 코인 감소
	int iMonsterCoin = kMonsterCoinSlot.GetUse();
	if( iMonsterCoin >= iUseCnt )
	{		
		kMonsterCoinSlot.m_iValue1	= iMonsterCoin - iUseCnt;
		nCoinUseCount				= iUseCnt;
	}
	else
	{
		nNeedGoldCoinCount			= iUseCnt - iMonsterCoin;
		nCoinUseCount				= iMonsterCoin;
		kMonsterCoinSlot.m_iValue1	= 0;
	}

	// 삭제하지 않는다
	m_UserEtcItem.SetEtcItem( kMonsterCoinSlot );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, kMonsterCoinSlot.m_iType, nCoinUseCount );

	// 골드 몬스터 코인 감소
	if( nNeedGoldCoinCount > 0 )
	{
		kGoldMonsterCoinSlot.AddUse( -nNeedGoldCoinCount );
		nGoldCoinUseCount = nNeedGoldCoinCount;

		if( kGoldMonsterCoinSlot.GetUse() <= 0 )
		{
			m_UserEtcItem.DeleteEtcItem( kGoldMonsterCoinSlot.m_iType );
		}
		else
		{
			m_UserEtcItem.SetEtcItem( kGoldMonsterCoinSlot );
		}
		g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, kGoldMonsterCoinSlot.m_iType, nGoldCoinUseCount );
	}

	g_BattleRoomMgr.SyncMonsterCoin();
	return true;
}

bool ioMyInfo::UseGoldMonsterCoin()
{
	return AddEtcGoldMonsterCoin( -1 );
}

int  ioMyInfo::GetEtcMonsterCoin()
{
	ioUserEtcItem::ETCITEMSLOT kSlot;
	m_UserEtcItem.GetEtcItem( ioEtcItem::EIT_ETC_MONSTER_COIN, kSlot );
	if( kSlot.m_iType <= 0 )
	{
		return 0;
	}
	return kSlot.GetUse();
}

int  ioMyInfo::GetEtcGoldMonsterCoin()
{
	ioUserEtcItem::ETCITEMSLOT kSlot;
	m_UserEtcItem.GetEtcItem( ioEtcItem::EIT_ETC_GOLDMONSTER_COIN, kSlot );
	if( kSlot.m_iType <= 0 )
	{
		return 0;
	}
	return kSlot.GetUse();
}

int  ioMyInfo::GetEtcRaidCoin()
{
	ioUserEtcItem::ETCITEMSLOT kSlot;
	m_UserEtcItem.GetEtcItem( ioEtcItem::EIT_ETC_RAID_COIN, kSlot );
	if( kSlot.m_iType <= 0 )
	{
		return 0;
	}
	return kSlot.GetUse();
}

bool ioMyInfo::AddEtcMonsterCoin( int iAddCoin )
{
	ioUserEtcItem::ETCITEMSLOT kSlot;
	m_UserEtcItem.GetEtcItem( ioEtcItem::EIT_ETC_MONSTER_COIN, kSlot );
	if( kSlot.m_iType <= 0 )
	{
		return false;
	}

	if( iAddCoin < 0 && kSlot.GetUse() <= 0 )
		return false;

	kSlot.AddUse( iAddCoin );
	m_UserEtcItem.SetEtcItem( kSlot );
	g_BattleRoomMgr.SyncMonsterCoin();
	return true;
}

bool ioMyInfo::AddEtcGoldMonsterCoin( int iAddCoin )
{
	ioUserEtcItem::ETCITEMSLOT kSlot;
	m_UserEtcItem.GetEtcItem( ioEtcItem::EIT_ETC_GOLDMONSTER_COIN, kSlot );
	if( kSlot.m_iType <= 0 )
	{
		return false;
	}
	
	if( iAddCoin < 0 && kSlot.GetUse() <= 0 )
		return false;

	kSlot.AddUse( iAddCoin );
	if( kSlot.GetUse() <= 0 )
	{
		m_UserEtcItem.DeleteEtcItem( kSlot.m_iType );
	}
	else
	{
		m_UserEtcItem.SetEtcItem( kSlot );
	}
	g_BattleRoomMgr.SyncMonsterCoin();
	return true;
}

bool ioMyInfo::SetEtcMonsterCoin( int iCoin )
{
	ioUserEtcItem::ETCITEMSLOT kSlot;
	m_UserEtcItem.GetEtcItem( ioEtcItem::EIT_ETC_MONSTER_COIN, kSlot );
	if( kSlot.m_iType <= 0 )
	{
		return false;
	}
	kSlot.AddUse( -kSlot.GetUse() );  // 초기화
	kSlot.AddUse( iCoin );
	return true;
}

bool ioMyInfo::SetEtcGoldMonsterCoin( int iCoin )
{
	ioUserEtcItem::ETCITEMSLOT kSlot;
	m_UserEtcItem.GetEtcItem( ioEtcItem::EIT_ETC_GOLDMONSTER_COIN, kSlot );
	if( kSlot.m_iType <= 0 )
	{
		return false;
	}
	kSlot.AddUse( -kSlot.GetUse() ); // 초기화
	kSlot.AddUse( iCoin );
	return true;
}

bool ioMyInfo::SetEtcRaidCoin( int iCoin )
{
	ioUserEtcItem::ETCITEMSLOT kSlot;
	m_UserEtcItem.GetEtcItem( ioEtcItem::EIT_ETC_RAID_COIN, kSlot );
	if( kSlot.m_iType <= 0 )
	{
		return false;
	}

	kSlot.AddUse( -kSlot.GetUse() ); // 초기화
	kSlot.AddUse( iCoin );

	m_UserEtcItem.SetEtcItem( kSlot );

	return true;
}

int  ioMyInfo::RemainRefillMinute()
{
	if( Help::GetRefillCoinSec() == 0 || Help::GetRefillCoinMax() <= 0 ) 
		return Help::GetRefillCoinSec();
	
	int iEtcMonsterCoin = GetEtcMonsterCoin();
	if( iEtcMonsterCoin >= Help::GetRefillCoinMax() ) 
		return Help::GetRefillCoinSec();

	const int const_min = 60;
	int iGapSec = ( FRAMEGETTIME() - m_iRefillCoinCurTime ) / 1000;
	if( iGapSec <= 30 )         // 30초 이상이되면 갱신
	{
		return m_user_data.m_refill_data / const_min;
	}
	m_iRefillCoinCurTime = FRAMEGETTIME();
	m_user_data.m_refill_data = max( const_min, m_user_data.m_refill_data - iGapSec );
	return m_user_data.m_refill_data / const_min;
}

int  ioMyInfo::GetEtcCoinMoney( DWORD dwEtcItem )
{
	ioUserEtcItem::ETCITEMSLOT kCoinSlot;
	m_UserEtcItem.GetEtcItem( dwEtcItem, kCoinSlot );
	return kCoinSlot.GetUse();
}

void ioMyInfo::UseEtcCoinMoney( DWORD dwEtcItem, int iUseCoin )
{
	ioUserEtcItem::ETCITEMSLOT kCoinSlot;
	m_UserEtcItem.GetEtcItem( dwEtcItem, kCoinSlot );
	kCoinSlot.AddUse( -min( kCoinSlot.GetUse(), iUseCoin ) );
	m_UserEtcItem.SetEtcItem( kCoinSlot );
}

bool ioMyInfo::InitEtcItemUseBattleRecord()
{
	m_UserRecord.SetRecordInfo( RECORD_BATTLE, 0, 0, 0, 0 );
	g_UserInfoMgr.SetPartyExp( GetPublicID(), GetPartyExp() );
	g_UserInfoMgr.SetKillDeathLevel( GetPublicID(), GetKillDeathLevel() );
	return true;
}

bool ioMyInfo::InitEtcItemUseLadderRecord()
{
	m_UserRecord.SetRecordInfo( RECORD_LADDER, 0, 0, 0, 0 );
	g_UserInfoMgr.SetLadderExp( GetPublicID(), GetLadderExp() );
	g_UserInfoMgr.SetKillDeathLevel( GetPublicID(), GetKillDeathLevel() );
	return true;
}

bool ioMyInfo::InitEtcItemUseHeroRecord()
{
	m_UserRecord.SetRecordInfo( RECORD_HEROMATCH, 0, 0, 0, 0 );
	m_UserRecord.SetHeroSeasonRecordInfo( 0, 0, 0, 0 );
	m_user_data.m_iHeroExpert = 0;
	g_UserInfoMgr.SetHeroWin( GetPublicID(), GetRecordWin( RECORD_HEROMATCH ) );
	g_UserInfoMgr.SetHeroLose( GetPublicID(), GetRecordLose( RECORD_HEROMATCH ) );
	g_UserInfoMgr.SetKillDeathLevel( GetPublicID(), GetKillDeathLevel() );
	return true;
}

DWORD ioMyInfo::GetTimeGashaponEtcItem()
{
	for(int iEtcType = ioEtcItem::EIT_ETC_ITEM_TIME_GASHAPON1; iEtcType < ioEtcItem::EIT_ETC_ITEM_TIME_GASHAPON100 + 1; ++iEtcType )
	{
		ioUserEtcItem::ETCITEMSLOT kSlot;
		if( m_UserEtcItem.GetEtcItem( iEtcType, kSlot ) )
		{
			return iEtcType;
		}
	}

	for(int iEtcType = ioEtcItem::EIT_ETC_ITEM_TIME_GASHAPON101; iEtcType < ioEtcItem::EIT_ETC_ITEM_TIME_GASHAPON300 + 1; ++iEtcType )
	{
		ioUserEtcItem::ETCITEMSLOT kSlot;
		if( m_UserEtcItem.GetEtcItem( iEtcType, kSlot ) )
		{
			return iEtcType;
		}
	}

	for(int iEtcType = ioEtcItem::EIT_ETC_ITEM_TIME_GASHAPON301; iEtcType < ioEtcItem::EIT_ETC_ITEM_TIME_GASHAPON600 + 1; ++iEtcType )
	{
		ioUserEtcItem::ETCITEMSLOT kSlot;
		if( m_UserEtcItem.GetEtcItem( iEtcType, kSlot ) )
		{
			return iEtcType;
		}
	}

	return 0;
}

ioHashString ioMyInfo::GetEtcMotionAni( DWORD dwMotionType, bool bMale, const ioHashString &rkOriginalAni )
{
	ioHashStringVec kMotionList;
	int iCurrentEtcSlot = m_UserEtcItem.GetEtcItemCurrentSlot();
	for(int i = 0;i < iCurrentEtcSlot;i++)
	{
		ioUserEtcItem::ETCITEMSLOT kSlot;
		if( m_UserEtcItem.GetEtcItemByArray( i, kSlot ) )
		{
			if( COMPARE( kSlot.m_iType, ioEtcItem::EIT_ETC_MOTION1, ioEtcItem::EIT_ETC_MOTION100 + 1 ) || 
				COMPARE( kSlot.m_iType, ioEtcItem::EIT_ETC_MOTION101, ioEtcItem::EIT_ETC_MOTION400 + 1 ) )
			{
				if( kSlot.GetUse() != dwMotionType ) continue;

				ioEtcItemMotion *pEtcItemMotion = dynamic_cast< ioEtcItemMotion * >( g_EtcItemMgr.FindEtcItem( kSlot.m_iType ) );
				if( pEtcItemMotion )
				{
					if( pEtcItemMotion->GetMotionOptionTitle( dwMotionType ).IsEmpty() ) continue;

					if( bMale )
					{
						kMotionList.push_back( pEtcItemMotion->GetHumanManMotion() );
					}
					else
					{
						kMotionList.push_back( pEtcItemMotion->GetHumanWomanMotion() );
					}					
				}
			}			
		}
	}

	if( kMotionList.empty() ) 
		return rkOriginalAni;

	int iRandom = rand() % (int)kMotionList.size();
	ioHashString kReturn = kMotionList[iRandom];
	kMotionList.clear();
	return kReturn;
}

DWORD ioMyInfo::GetEtcMotionAni( DWORD dwMotionType )
{
	DWORDVec kMotionList;
	int iCurrentEtcSlot = m_UserEtcItem.GetEtcItemCurrentSlot();
	for(int i = 0;i < iCurrentEtcSlot;i++)
	{
		ioUserEtcItem::ETCITEMSLOT kSlot;
		if( m_UserEtcItem.GetEtcItemByArray( i, kSlot ) )
		{
			if( COMPARE( kSlot.m_iType, ioEtcItem::EIT_ETC_MOTION1, ioEtcItem::EIT_ETC_MOTION100 + 1 ) || 
				COMPARE( kSlot.m_iType, ioEtcItem::EIT_ETC_MOTION101, ioEtcItem::EIT_ETC_MOTION400 + 1 ) )
			{
				if( kSlot.GetUse() != dwMotionType ) continue;

				ioEtcItemMotion *pEtcItemMotion = dynamic_cast< ioEtcItemMotion * >( g_EtcItemMgr.FindEtcItem( kSlot.m_iType ) );
				if( pEtcItemMotion )
				{
					if( pEtcItemMotion->GetMotionOptionTitle( dwMotionType ).IsEmpty() ) continue;

					kMotionList.push_back( kSlot.m_iType );
				}		
			}			
		}
	}

	if( kMotionList.empty() ) 
		return 0;

	int iRandom = rand() % (int)kMotionList.size();
	DWORD kReturn = kMotionList[iRandom];
	kMotionList.clear();
	return kReturn;
}

void ioMyInfo::AddTimeGrowthEndInfo( int iClassType, int iTimeSlot, bool bConfirm )
{
	TimeGrowthClassInfo kInfo;
	kInfo.m_iClassType = iClassType;
	kInfo.m_iTimeSlot = iTimeSlot;
	kInfo.m_bConfirm = bConfirm;

	m_vTimeGrowthInfoList.push_back( kInfo );
}

void ioMyInfo::ShowTimeGrowthEndInfo()
{
	if( m_vTimeGrowthInfoList.empty() )
		return;

	int iSize = m_vTimeGrowthInfoList.size();
	ioTimeGrowthEndWnd *pEndInfoWnd = dynamic_cast<ioTimeGrowthEndWnd*>(g_GUIMgr.FindWnd(TIME_GROWTH_END_WND));
	if( pEndInfoWnd )
	{
		if( !pEndInfoWnd->IsShow() )
		{
			pEndInfoWnd->InitInfoWnd();
		}
	}
	else
	{
		// 없으면 추가해 준다
		pEndInfoWnd = dynamic_cast<ioTimeGrowthEndWnd*>(	g_GUIMgr.AddWnd("XML/TimeGrowthEndWnd.xml"));
		if( pEndInfoWnd )
		{
			pEndInfoWnd->InitInfoWnd();
		}
	}

	if( !pEndInfoWnd )
	{
		LOG.PrintTimeAndLog( 0, "ioMyInfo::ShowTimeGrowthEndInfo() - Create Fail ReleaseWnd" );
		m_vTimeGrowthInfoList.clear();
		return;
	}

	for( int i=0; i < iSize; ++i )
	{
		int iClassType = m_vTimeGrowthInfoList[i].m_iClassType;
		int iTimeSlot = m_vTimeGrowthInfoList[i].m_iTimeSlot;
		bool bConfirm = m_vTimeGrowthInfoList[i].m_bConfirm;

		ioHashString szClassName = GetClassName( iClassType );
		ioHashString szGrowthName;
		Help::GetGrowthTitle( iTimeSlot, true, szGrowthName );

		int iLevel = (int)m_GrowthLevel.GetGrowthLevelByTimeSlot( iClassType, iTimeSlot );

		ioHashString szIconName = GetMySoldierIconName(iClassType);
		ioHashString szSubIconName = GetSoldierSubIconName(iClassType);

		pEndInfoWnd->AddEndInfo( iClassType, szClassName, szIconName, szSubIconName, szGrowthName, iLevel, bConfirm );
	}

	if( !pEndInfoWnd->IsShow() )
		pEndInfoWnd->ShowEndInfoWnd();

	m_vTimeGrowthInfoList.clear();
}

int ioMyInfo::GetTimeGrowthPossibleCount()
{
	int iCount = 0;
	ioGrowthLevel *pLevel = GetGrowthLevel();
	
	if( !pLevel ) return iCount;

	for( int i=0 ; i<GetCharCount() ; i++ )
	{
		CHARACTER kCharInfo = m_pCharData->at(i).m_data;
		if( !IsCharActive( i ) ) continue;
		if( IsCharRental( i ) ) continue;
		
		if( pLevel->FindTimeGrowthInfo( kCharInfo.m_class_type ) )
			iCount++;
		else if( pLevel->GetCharGrowthPoint( kCharInfo.m_class_type ) > 0 )
		{
			if( pLevel->CheckEnableTimeGrowthClass( kCharInfo.m_class_type ) )
				iCount++;
		}
	}
	return iCount;
}

bool ioMyInfo::IsTimeGrowthCharCheck( int iClassType )
{
	if( m_GrowthLevel.FindTimeGrowthInfo( iClassType ) == NULL )
		return false;
	return true;
}

int ioMyInfo::GetTimeGrowingCount()
{
	int iCount = 0;
	ioGrowthLevel *pLevel = GetGrowthLevel();

	if( !pLevel ) return iCount;

	return pLevel->GetCurTimeGrowthClassCnt();
}

void ioMyInfo::SyncBattleNLadderCharIndex()
{
	DWORD dwSyncCharIndex = 0;
	if( g_BattleRoomMgr.IsBattleRoom() )
	{
		DWORD dwPrevCharIndex = g_BattleRoomMgr.GetUserCharIndex( GetPublicID() );
		DWORD dwNextCharIndex = GetCharIndex( GetSelectCharArray() );
		if( dwNextCharIndex == 0 )
			dwNextCharIndex = GetCharIndex( 0 );

		if( dwPrevCharIndex != dwNextCharIndex )
		{
			dwSyncCharIndex = dwNextCharIndex;
			g_BattleRoomMgr.UpdateUserCharIndex( GetPublicID(), dwNextCharIndex, false );
		}
	}
	else if( g_LadderTeamMgr.IsLadderTeam() )
	{
		DWORD dwPrevCharIndex = g_LadderTeamMgr.GetUserCharIndex( GetPublicID() );
		DWORD dwNextCharIndex = GetCharIndex( GetSelectCharArray() );
		if( dwNextCharIndex == 0 )
			dwNextCharIndex = GetCharIndex( 0 );

		if( dwPrevCharIndex != dwNextCharIndex )
		{
			dwSyncCharIndex = dwNextCharIndex;
			g_LadderTeamMgr.UpdateUserCharIndex( GetPublicID(), dwNextCharIndex, false );
		}
	}
	else if( g_ShuffleRoomMgr.IsShuffleRoom() )
	{
		DWORD dwPrevCharIndex = g_ShuffleRoomMgr.GetUserCharIndex( GetPublicID() );
		DWORD dwNextCharIndex = GetCharIndex( GetSelectCharArray() );
		if( dwNextCharIndex == 0 )
			dwNextCharIndex = GetCharIndex( 0 );

		if( dwPrevCharIndex != dwNextCharIndex )
		{
			dwSyncCharIndex = dwNextCharIndex;
			g_ShuffleRoomMgr.UpdateUserCharIndex( GetPublicID(), dwNextCharIndex, false );
		}
	}

	if( dwSyncCharIndex > 0 )
	{
		CHARACTER kCharInfo;
		CopyCharacter( GetCharArray( dwSyncCharIndex ), &kCharInfo );

		SP2Packet kPacket( CUPK_SYNC_CHARINFO );
		kPacket << GetPublicID() << dwSyncCharIndex << kCharInfo;
		FillCharItemData( dwSyncCharIndex, kPacket );
		P2PNetwork::SendToAllUser( kPacket );
	}
}

void ioMyInfo::NewLoginQuestCheck()
{
	LOG.PrintTimeAndLog(0, "NewLoginQuestCheck - %d, %d", m_iLogOutLaterDay, m_iLogOutLaterSec );

	// 완료 조건
	g_QuestMgr.QuestCompleteTerm( QC_GAME_LOGIN, m_iLogOutLaterSec );  // 로그인 체크

	// 발생 조건
	g_QuestMgr.QuestOccurTerm( QO_GAME_LOGIN );  // 로그인 체크
	g_QuestMgr.QuestOccurTerm( QO_GAME_LOGIN_DORMANT, m_iLogOutLaterDay );
	g_QuestMgr.QuestOccurTerm( QO_GAME_LOGIN_DORMANT_CUSTOM, m_PreLogoutTime.GetYear(), m_PreLogoutTime.GetMonth(), m_PreLogoutTime.GetDay() );

	for( int i=0 ; i<GetCharCount() ; i++ )
	{
		CHARACTER kCharInfo = m_pCharData->at(i).m_data;

		// 완료 조건
		if( kCharInfo.m_bActive )
		{
			g_QuestMgr.QuestCompleteTerm( QC_SOLDIER_ACQUIRE, kCharInfo.m_class_type, (int)kCharInfo.m_ePeriodType, (int)kCharInfo.m_chExerciseStyle );
		}

		// 발생 조건
		g_QuestMgr.QuestOccurTerm( QO_SOLDIER_ACQUIRE, kCharInfo.m_class_type ); // 용병 관련 퀘스트 확인
	}
}

void ioMyInfo::SetExcavationLevel( int iLevel, int iExpert )
{
	m_user_data.m_iExcavationLevel = iLevel;
	m_user_data.m_iExcavationExp   = iExpert;

	g_UserInfoMgr.SetExcavationExp( GetPublicID(), GetExcavationLevel(), GetExcavationExpert() );
}

int ioMyInfo::GetExcavationLevel()
{
	return m_user_data.m_iExcavationLevel;
}

int ioMyInfo::GetExcavationExpert()
{
	return m_user_data.m_iExcavationExp;
}

void ioMyInfo::ApplyChangeMedalItem( int iCharArray, int iItemType, int iCustomIndex, bool bEquip )
{
	if( !COMPARE( iCharArray, 0, GetCharCount() ) ) 
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		LOG.PrintTimeAndLog( 0, "%s Wrong Array : %d", __FUNCTION__, iCharArray );
		return;
	}

	ioUserMedalItem::MEDALITEMSLOT kSlot;
	if( bEquip )
	{
		if( !m_UserMedalItem.GetMedalItem( iItemType, iCustomIndex, kSlot, true ) )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
			LOG.PrintTimeAndLog( 0, "%s Wrong Type : %d", __FUNCTION__, iItemType );
			return;
		}
		kSlot.m_iEquipClass = m_pCharData->at(iCharArray).m_data.m_class_type;
		m_UserMedalItem.SetMedalItem( kSlot, true );
		SerUserInfoCharData( m_pCharData->at(iCharArray).m_data.m_class_type );
	}
	else
	{
		int nClassType = m_pCharData->at(iCharArray).m_data.m_class_type;
		if( !m_UserMedalItem.GetMedalItem( iItemType, iCustomIndex, kSlot, false, nClassType ) )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
			LOG.PrintTimeAndLog( 0, "%s Wrong Type : %d", __FUNCTION__, iItemType );
			return;
		}
		kSlot.m_iEquipClass = 0; // 0이면 미장착
		m_UserMedalItem.SetMedalItem( kSlot, false, nClassType );
		SerUserInfoCharData( m_pCharData->at(iCharArray).m_data.m_class_type );
	}
}

bool ioMyInfo::DeleteMedalItemData( SP2Packet &rkPacket )
{
	int iSize;
	rkPacket >> iSize;

	if( iSize <= 0 )
		return false;

	bool bReturn = true;
	for( int i=0; i < iSize; ++i )
	{
		int iItemType = 0;
		rkPacket >> iItemType;
		if( !m_UserMedalItem.DeleteMedalItemData( iItemType, 0 ) )
			bReturn = false;
	}

	bool bImmediately;
	rkPacket >> bImmediately;

	return bReturn;
}

bool ioMyInfo::DeleteMedalItem( int iItemType, int iCustomIndex )
{
	return m_UserMedalItem.DeleteMedalItemData( iItemType, iCustomIndex );
}

void ioMyInfo::GetMedalItemTypeVec( OUT MedalSyncInfoVec &rvItemTypeVec, IN int iClassType )
{
	// 대여 용병이면 대여 메달 OUT
	for(int i = 0;i < GetCharCount();i++)
	{
		if( m_pCharData->at(i).m_data.m_class_type == iClassType )
		{
			if( m_pCharData->at(i).m_data.m_chExerciseStyle == EXERCISE_RENTAL )
			{
				m_CharRentalData.GetEquipMedal( m_pCharData->at(i).m_dwIndex, rvItemTypeVec );
				return;
			}
		}
	}

	m_UserMedalItem.GetMedalItemTypeVec( rvItemTypeVec, iClassType );
}

void ioMyInfo::GetMedalTypeAndGrowthLevel( IN int iClassType, OUT MedalSyncInfoVec &rvMedalItemTypeVec, OUT IntVec &rvGrowthLevelVec )
{
	rvMedalItemTypeVec.clear();
	rvGrowthLevelVec.clear();

	GetMedalItemTypeVec( rvMedalItemTypeVec, iClassType );

	int i = 0;
	ioGrowthLevel *pGrowthLevel = GetGrowthLevel();
	for( i=0; i < MAX_CHAR_GROWTH; i++ )
	{
		int iLevel = 0;
		if( pGrowthLevel )
			iLevel = pGrowthLevel->GetCharGrowthLevel( iClassType, i, false );
		rvGrowthLevelVec.push_back( iLevel );
	}

	for( i=0; i < MAX_ITEM_GROWTH; i++ )
	{
		int iLevel = 0;
		if( pGrowthLevel )
			iLevel = pGrowthLevel->GetItemGrowthLevel( iClassType, i, false );
		rvGrowthLevelVec.push_back( iLevel );
	}

	int iSize = rvGrowthLevelVec.size();
	for ( i = 0; i < MAX_INVENTORY ; i++)
	{
		int iCharArray = GetClassArray( iClassType );
		const ITEM_DATA &kItem = GetCharItem( iCharArray, i );
		if( COMPARE( i, 0, iSize ) )
		{
			if ( kItem.m_item_reinforce <= g_CompoundMaterialMgr.GetFixedFailReinforce() )
				rvGrowthLevelVec[i] += kItem.m_item_reinforce;
			else
				rvGrowthLevelVec[i] += g_GrowthInfoMgr.GetReinforceAddStat( kItem.m_item_reinforce );
		}
		int iArray = i+MAX_CHAR_GROWTH;
		if( COMPARE( iArray, 0, iSize ) )
		{
			if ( kItem.m_item_reinforce <= g_CompoundMaterialMgr.GetFixedFailReinforce() )
				rvGrowthLevelVec[iArray] += kItem.m_item_reinforce;
			else
				rvGrowthLevelVec[iArray] += g_GrowthInfoMgr.GetReinforceAddStat( kItem.m_item_reinforce );
		}
	}

	int iMedalSize = rvMedalItemTypeVec.size();
	for ( i = 0; i < iMedalSize ; i++)
	{
		int j = 0;
		for ( j = 0; j < MAX_CHAR_GROWTH; j++)
		{
			int iArray = j+MAX_ITEM_GROWTH;
			if( COMPARE( j, 0, iSize ) && COMPARE( iArray, 0, iSize ) )
			{
				if( rvMedalItemTypeVec[i].m_iCustomIndex == 0 )
				{
					rvGrowthLevelVec[j] += g_MedalItemMgr.GetMedalItemGrowth( rvMedalItemTypeVec[i].m_iItemType, true, j );
					rvGrowthLevelVec[iArray] += g_MedalItemMgr.GetMedalItemGrowth( rvMedalItemTypeVec[i].m_iItemType, false, j );
				}
				else
				{
					rvGrowthLevelVec[j] += rvMedalItemTypeVec[i].m_iStat[j];
					rvGrowthLevelVec[iArray] += rvMedalItemTypeVec[i].m_iStat[iArray];
				}
			}
		}
	}

	//펫 성장
	PetSlot sPetSlot;
	m_UserPet.GetPetSlotEquip( sPetSlot );

	if ( sPetSlot.m_nPetCode > 0)
	{
		for ( i = 0; i < MAX_CHAR_GROWTH + MAX_ITEM_GROWTH ; i++)
			rvGrowthLevelVec[i] += g_PetInfoMgr.GetTotalStat( sPetSlot.m_nPetCode, i, sPetSlot.m_nCurLevel );
	}

	//각성
	for( i = 0; i < MAX_CHAR_GROWTH; i++ )
	{
		if( COMPARE( i, 0, iSize ) )
		{
			const CHARACTER& rkChar = GetCharacterToClassType( iClassType );
			rvGrowthLevelVec[i] += g_AwakeMgr.GetAwakeCharGrowth( static_cast<AwakeType>( rkChar.m_iAwakeType ), i );
		}
	}

	for( i = 0; i < MAX_CHAR_GROWTH; i++ )
	{
		int iArray = i + MAX_ITEM_GROWTH;
		if( COMPARE( iArray, 0, iSize ) )
		{
			const CHARACTER& rkChar = GetCharacterToClassType( iClassType );
			rvGrowthLevelVec[iArray] += g_AwakeMgr.GetAwakeItemGrowth( static_cast<AwakeType>( rkChar.m_iAwakeType ), i );
		}
	}

	const CHARACTER& rkCharInfo = GetCharacterToClassType( iClassType );
	if( g_RSoldierInfoManager.IsRSoldierType( rkCharInfo ) )
	{
		for ( i = 0; i < MAX_CHAR_GROWTH + MAX_ITEM_GROWTH ; ++i )
			rvGrowthLevelVec[i] += g_RSoldierInfoManager.GetSoldierAddStat( i, m_iRSoldierPossessionCount );
	}

	if( g_RSoldierInfoManager.IsSoldierSet( rkCharInfo ) )
	{
		for ( i = 0; i < MAX_CHAR_GROWTH + MAX_ITEM_GROWTH ; ++i )
			rvGrowthLevelVec[i] += g_RSoldierInfoManager.GetSoldierSetAddStat( i, iClassType, GetSoldierSetCnt( g_RSoldierInfoManager.GetSoldierSetNum( iClassType ) ) );
	}
}

bool ioMyInfo::IsFirstCameraFixedHelp()
{
	if( g_GUIMgr.IsShow( MOVIE_MODE_HELP_WND ) ) return true;
	if( !m_bFirstCameraFixedHelp ) return false;

	m_bFirstCameraFixedHelp = false;
	g_GUIMgr.ShowWnd( MOVIE_MODE_HELP_WND );
	return true;
}

bool ioMyInfo::IsExtraItemFull( bool bSetMsgBox )
{
	if( m_UserExtraItem.IsSlotFull() )
	{
		if( bSetMsgBox )
		{
			NewShopItemPurchaseLeedWnd *pPurchaseLeedWnd = dynamic_cast<NewShopItemPurchaseLeedWnd*> ( g_GUIMgr.FindWnd( ITEM_PURCHASE_LEED_WND ) );
			if ( pPurchaseLeedWnd )
			{
				ioEtcItem *pItem = g_EtcItemMgr.FindEtcItem( ioEtcItem::EIT_ETC_EXTRAITEM_SLOT_EXTEND );
				if ( pItem )
				{
					ioHashString sIcon = pItem->GetIconName();
					ioHashString sSubIcon = pItem->GetSubIconName();
					ioHashString sTitle= pItem->GetName();
					ioHashStringVec sDescVec;
					sDescVec.push_back( "[장비슬롯확장]이 필요합니다." );
					sDescVec.push_back( "부족한 장비 슬롯을 위해 구입해주세요." );
					sDescVec.push_back( "상점 - 특별에서 구입하실 수 있습니다." );
					if( pPurchaseLeedWnd->SetInfo( ioEtcItem::EIT_ETC_EXTRAITEM_SLOT_EXTEND, sIcon, sSubIcon, sTitle, sDescVec ) )
						pPurchaseLeedWnd->ShowWnd();
				}
			}
		}
		return true;
	}
	return false;
}

bool ioMyInfo::IsDecorationFull( bool bSetMsgBox )
{
	if( m_Inventory.IsInventoryItemFull() )
	{
		if( bSetMsgBox )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1),
								m_Inventory.GetInventoryItemCnt(), ioInventory::MAX_SLOT_CNT );
		}
		return true;
	}
	return false;
}

bool ioMyInfo::IsMedalItemFull( bool bSetMsgBox )
{
	if( m_UserMedalItem.IsSlotFull() )
	{
		if( bSetMsgBox )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1),
				m_UserMedalItem.GetExtraItemCnt(), ioUserMedalItem::MAX_SLOT_CNT );
		}
		return true;
	}
	return false;
}

bool ioMyInfo::IsPetFull( bool bSetMsgBox )
{
	if( m_UserPet.IsSlotFull() )
	{
		if( bSetMsgBox )
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1), m_UserPet.GetPetSlotCnt(), ioUserPet::MAX_SLOT_CNT );
		return true;
	}
	return false;
}

bool ioMyInfo::IsCostumeFull( bool bSetMsgBox )
{
	if( m_UserCostume.IsSlotFull() )
	{
		if( bSetMsgBox )
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1), m_UserCostume.GetCostumeSlotCnt(), ioUserCostume::MAX_SLOT_CNT );
		return true;
	}
	return false;
}

bool ioMyInfo::IsAccessoryFull( bool bSetMsgBox )
{
	if( m_UserAccessory.IsSlotFull() )
	{
		if( bSetMsgBox )
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1), m_UserAccessory.GetAccessorySlotCnt(), ioUserAccessory::MAX_SLOT_CNT );
		return true;
	}
	return false;
}

void ioMyInfo::CharRentalTimeMsgBox( DWORD dwLimitTime )
{
	if( dwLimitTime == 0 )
	{
		g_App.OpenMannerWnd( GetPublicID().c_str(), true );
	}
	else
	{
		ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
		kPrinter[0].SetTextStyle( TS_NORMAL );
		kPrinter[0].SetBkColor( 0, 0, 0 );	
		kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter[0].AddTextPiece( FONT_SIZE_13, STR(1) );

		kPrinter[1].SetTextStyle( TS_NORMAL );
		kPrinter[1].SetBkColor( 0, 0, 0 );	
		kPrinter[1].SetTextColor( TCT_DEFAULT_GRAY );
		kPrinter[1].AddTextPiece( FONT_SIZE_13, STR(2), dwLimitTime );
		g_GUIMgr.SetPrevMsgListBox( NULL, MB_OK, NULL, kPrinter );
	}
}

ioMyInfo::RentalTimeData &ioMyInfo::GetRentalTimeData( DWORD dwCharIndex )
{
	RentalTimeDataMap::iterator iter = m_RentalTimeDataMap.find( dwCharIndex );
	if( iter == m_RentalTimeDataMap.end() )
	{
		RentalTimeData kTimeData;
		kTimeData.m_dwCurrentTime = FRAMEGETTIME();
		m_RentalTimeDataMap.insert( RentalTimeDataMap::value_type( dwCharIndex, kTimeData ) );
		return GetRentalTimeData( dwCharIndex );
	}
	return iter->second;
}

void ioMyInfo::RemoveRentalTimeData( DWORD dwCharIndex )
{
	RentalTimeDataMap::iterator iter = m_RentalTimeDataMap.find( dwCharIndex );
	if( iter != m_RentalTimeDataMap.end() )
	{
		m_RentalTimeDataMap.erase( iter );
	}
}

void ioMyInfo::ReturnRentalCharToOwenrID( const ioHashString &rkOwnerID )
{
	for(int i = 0;i < GetCharCount();i++)
	{
		CHARACTERDATA &rkCharData = m_pCharData->at(i);
		if( rkCharData.m_data.m_chExerciseStyle == EXERCISE_RENTAL )
		{
			if( m_CharRentalData.GetOwnerName( rkCharData.m_dwIndex ) == rkOwnerID )
			{
				// 반납
				rkCharData.m_data.m_dwRentalMinute = 0;       
				RemoveRentalTimeData( rkCharData.m_dwIndex );			

				SP2Packet kPacket( CTPK_USER_CHAR_RENTAL_TIME_END );
				kPacket << rkCharData.m_dwIndex;
				TCPNetwork::SendToServer( kPacket );
			}
		}
	}
}

void ioMyInfo::SetRentalCharTime( int iClassType, DWORD dwMinute )
{
	for(int i = 0;i < GetCharCount();i++)
	{
		CHARACTERDATA &rkCharData = m_pCharData->at(i);
		if( rkCharData.m_data.m_class_type == iClassType )
		{
			rkCharData.m_data.m_dwRentalMinute = dwMinute;
			if( dwMinute == 0 )
			{
				RemoveRentalTimeData( rkCharData.m_dwIndex );
			}
			SerUserInfoCharData( rkCharData.m_data.m_class_type );
			return;
		}
	}
}

void ioMyInfo::RentalCharTimeCheck()
{
	if( FRAMEGETTIME() - m_dwRentalProcessTime < 10000 ) return;

	m_dwRentalProcessTime = FRAMEGETTIME();
	for(int i = 0;i < GetCharCount();i++)
	{
		CHARACTERDATA &rkCharData = m_pCharData->at(i);
		if( rkCharData.m_data.m_dwRentalMinute == 0 ) continue;
		
        if( rkCharData.m_data.m_sRentalType == CRT_RENTAL )
		{
			// 공급자
			RentalTimeData &rkTimeData = GetRentalTimeData( rkCharData.m_dwIndex );
			DWORD dwGapMinute = (float)( FRAMEGETTIME() - rkTimeData.m_dwCurrentTime ) / 60000;
			if( dwGapMinute >= 1 )
			{
				if( rkCharData.m_data.m_dwRentalMinute > dwGapMinute )
					rkCharData.m_data.m_dwRentalMinute -= dwGapMinute;
				else
				{
					// Send - 서버 확인 - 용병 복귀
					rkCharData.m_data.m_dwRentalMinute = 0;       
					CharRentalReturnWnd::SetRentalReturn( rkCharData.m_data.m_class_type, rkCharData.m_data.m_eCharPowerUpType );
					RemoveRentalTimeData( rkCharData.m_dwIndex );

					SP2Packet kPacket( CTPK_USER_CHAR_RENTAL_TIME_END );
					kPacket << rkCharData.m_dwIndex;
					TCPNetwork::SendToServer( kPacket );
				}
				SerUserInfoCharData( rkCharData.m_data.m_class_type );
				rkTimeData.m_dwCurrentTime = FRAMEGETTIME();
			}
		}
		else if( rkCharData.m_data.m_chExerciseStyle == EXERCISE_RENTAL )
		{
			// 대여자
			RentalTimeData &rkTimeData = GetRentalTimeData( rkCharData.m_dwIndex );
			DWORD dwGapMinute = (float)( FRAMEGETTIME() - rkTimeData.m_dwCurrentTime ) / 60000;
			if( dwGapMinute >= 1 )
			{
				if( rkCharData.m_data.m_dwRentalMinute > dwGapMinute )
					rkCharData.m_data.m_dwRentalMinute -= dwGapMinute;
				else
				{
					// Send - 서버 확인 - 용병 반환
					rkCharData.m_data.m_dwRentalMinute = 0;       
					RemoveRentalTimeData( rkCharData.m_dwIndex );

					SP2Packet kPacket( CTPK_USER_CHAR_RENTAL_TIME_END );
					kPacket << rkCharData.m_dwIndex;
					TCPNetwork::SendToServer( kPacket );
				}
				SerUserInfoCharData( rkCharData.m_data.m_class_type );
				rkTimeData.m_dwCurrentTime = FRAMEGETTIME();
			}
		}
	}
}

bool ioMyInfo::IsDisplaySoldier( int iClassType )
{
	DWORD dwCharIndex = GetCharIndexByClassType( iClassType );
	if( dwCharIndex == 0 ) return false;

	for(int i = 0;i < MAX_DISPLAY_CNT;i++)
	{
		if( m_UserHeadquartersData.m_dwCharacterIndex[i] == dwCharIndex )
			return true;
	}
	return false;
}

int ioMyInfo::GetDisplaySoldierCount()
{
	int iResult = 0;
	for(int i = 0;i < MAX_DISPLAY_CNT;i++)
	{
		if( m_UserHeadquartersData.m_dwCharacterIndex[i] != 0 )
		{
			int iCharArray = GetCharArray( m_UserHeadquartersData.m_dwCharacterIndex[i] );
			if( iCharArray != -1 )
			{
				iResult++;
			}
		}
	}
	return iResult;
}

void ioMyInfo::SetDisplaySoldier( int iClassType, int iXPos, int iZPos, bool bClear )
{
	DWORD dwCharIndex = GetCharIndexByClassType( iClassType );
	if( dwCharIndex == 0 ) return;

	int i = 0;
	if( bClear )  // 해제
	{
		for(i = 0;i < MAX_DISPLAY_CNT;i++)
		{
			if( m_UserHeadquartersData.m_dwCharacterIndex[i] == dwCharIndex )
			{
				m_UserHeadquartersData.m_dwCharacterIndex[i] = 0;
				m_UserHeadquartersData.m_iCharacterXPos[i] = 0;
				m_UserHeadquartersData.m_iCharacterZPos[i] = 0;
				
				SP2Packet kPacket( CTPK_HEADQUARTERS_OPTION_CMD );
				kPacket << HEADQUARTERS_OPTION_DISPLAY_DEL;
				kPacket << dwCharIndex;
				TCPNetwork::SendToServer( kPacket );
				break;
			}
		}
	}
	else		// 적용
	{
		bool bNewSlot = true;
		for(i = 0;i < MAX_DISPLAY_CNT;i++)
		{
			if( m_UserHeadquartersData.m_dwCharacterIndex[i] == dwCharIndex )
			{
				m_UserHeadquartersData.m_iCharacterXPos[i] = iXPos;
				m_UserHeadquartersData.m_iCharacterZPos[i] = iZPos;

				SP2Packet kPacket( CTPK_HEADQUARTERS_OPTION_CMD );
				kPacket << HEADQUARTERS_OPTION_DISPLAY_SET;
				kPacket << dwCharIndex << iXPos << iZPos;
				TCPNetwork::SendToServer( kPacket );

				bNewSlot = false;
				break;
			}
		}

		if( bNewSlot )
		{
			for(i = 0;i < MAX_DISPLAY_CNT;i++)
			{
				int iCharArray = GetCharArray( m_UserHeadquartersData.m_dwCharacterIndex[i] );
				if( iCharArray != -1 ) continue;

				m_UserHeadquartersData.m_dwCharacterIndex[i] = dwCharIndex;
				m_UserHeadquartersData.m_iCharacterXPos[i] = iXPos;
				m_UserHeadquartersData.m_iCharacterZPos[i] = iZPos;


				SP2Packet kPacket( CTPK_HEADQUARTERS_OPTION_CMD );
				kPacket << HEADQUARTERS_OPTION_DISPLAY_SET;
				kPacket << dwCharIndex << iXPos << iZPos;
				TCPNetwork::SendToServer( kPacket );
				break;
			}
		}
	}
}

void ioMyInfo::PCRoomQuestCheck()
{
	m_bReservePCRoomCheck = false;
	g_QuestMgr.QuestOccurTerm( QO_PCROOM_AUTHORITY );

	ioTask* task = new ioTask;
	task->eType = RESERVEDCMD_TYPE_PCROOOM;
	task->eWhen	= RESERVEDCMD_WHEN_RECV_PCROOOM;
	g_RserveTask.AddReservedTask( task );
}

void ioMyInfo::OneMinuteTimeCheckProcess()
{
	if( REALGETTIME() - m_dwOneMinuteTimeCheck < 60000 ) return;

	int iGapMinute = (float)( REALGETTIME() - m_dwOneMinuteTimeCheck ) / 60000;
	m_UserEtcItem.UpdateGashaponTime( iGapMinute );

	m_dwOneMinuteTimeCheck = REALGETTIME();
}

void ioMyInfo::GetServerTime( OUT SYSTEMTIME &rkST )
{
	if( m_iServerDate == 0 )
		return;

	time_t kCurServerTime = m_iServerDate;
	kCurServerTime += ( REALGETTIME()/1000 );// 현재까지의 클라이언트초를 넣는다.
	struct tm *pDate = localtime(&kCurServerTime);
	if( !pDate ) 
		return;
	
	// return 
	rkST.wYear  = pDate->tm_year+1900;
	rkST.wMonth = pDate->tm_mon+1;
	rkST.wDay   = pDate->tm_mday;
	rkST.wDayOfWeek = pDate->tm_wday;
	rkST.wHour      = pDate->tm_hour;
	rkST.wMinute    = pDate->tm_min;
	rkST.wSecond    = pDate->tm_sec;
}

void ioMyInfo::SetLobbyLockType( DWORD dwLockType, DWORD dwLobbyLockTypeMsgBoxID )
{
	m_dwLobbyLockType = dwLockType;
	m_dwLobbyLockTypeMsgBoxID = dwLobbyLockTypeMsgBoxID;

	if( m_dwLobbyLockType != 0 )
	{
		// 로그 서버 전송
		char szLog[MAX_PATH] = "";
		sprintf( szLog, "Lobby Lock User : %s - %d", GetPublicID().c_str(), dwLockType );
		SP2Packet kPacket( LUPK_LOG );
		kPacket << "ClientError";  // 로그 파일 타입
		kPacket << szLog;
		kPacket << 317;  // 오류번호:317
		kPacket << false; // write db
		P2PNetwork::SendLog(kPacket);
	}
}

void ioMyInfo::SetBattleLockType( DWORD dwLockType )
{
	m_dwBattleLockType = dwLockType;
}

void ioMyInfo::ProcessAnnounceGameAddiction()
{
	m_AnnounceGameAddiction.ProcessTime( REALGETTIME() );
	if( !m_AnnounceGameAddiction.IsAnnounce() )
		return;

	char szAnnounce[MAX_PATH]="";
	sprintf_s( szAnnounce, STR(1), m_AnnounceGameAddiction.GetPassedHour() );

	ioHashString sText = szAnnounce;
	g_GameAnnMgr.InsertServerAnnounce( sText, 10 );
	g_ChatMgr.SetSystemMsg( sText.c_str() );
}

void ioMyInfo::SetSelectShutDownTime( int iYear, int iMonth, int iDay, int iHour, int iMinute )
{
	m_SelectShutDownAnnouncer.SetShutDownTime( iYear, iMonth, iDay, iHour, iMinute );
}

void ioMyInfo::ProcessSelectShutDownAnnouncer()
{
	m_SelectShutDownAnnouncer.Process( GetServerDate(), REALGETTIME() );
	if( !m_SelectShutDownAnnouncer.IsAnnounce() )
		return;

	ioHashString sAnnounce;
	m_SelectShutDownAnnouncer.GetAnnounce( m_SelectShutDownAnnouncer.GetRemainMinutes(), sAnnounce );

	g_GameAnnMgr.InsertServerAnnounce( sAnnounce, 100 );
	g_ChatMgr.SetSystemMsg( sAnnounce.c_str() );
}

void ioMyInfo::SendMgameShutDownDate()
{
	if( GetChannelingType() != CNT_MGAME )
		return;

	m_SelectShutDownAnnouncer.ReadFile();
	if( m_SelectShutDownAnnouncer.GetYear() <= 0 )
		return;
	if( m_SelectShutDownAnnouncer.GetMonth() <= 0 )
		return;
	if( m_SelectShutDownAnnouncer.GetDay() <= 0 )
		return;
	if( m_SelectShutDownAnnouncer.GetHour() <= 0 )
		return;

	time_t kShutdownTime  = DateHelp::ConvertSecondTime( m_SelectShutDownAnnouncer.GetYear(), m_SelectShutDownAnnouncer.GetMonth(), m_SelectShutDownAnnouncer.GetDay(), m_SelectShutDownAnnouncer.GetHour(), 0, 0 );
	time_t kCurServerTime = GetServerDate();
	kCurServerTime += ( REALGETTIME()/1000 );// 현재까지의 클라이언트초를 넣는다.

 	int iGapSec = kShutdownTime - kCurServerTime;
 	if( iGapSec <= 0 )
 	{
		LOG.PrintTimeAndLog( 0, "ExitProgram - 10" );

#if defined( USE_GA )
		if( g_App.GetGAStart() == true )
		{
			char chLabel[32] = {0,};

			if ( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
				sprintf_e( chLabel, "%d", 10 );
			else
				SafeSprintf( chLabel, sizeof(chLabel), "%1", 10 );

			// GAME_END_ERR_POS
			g_HttpMng.GA_EventTracking( g_MyInfo.GetUserIndex()
				, "Game"
				, "Error"
				, chLabel
				, 1
				, "%2FGAME%2FOVER%2FERR"
				, 1 );

			g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FGAME%2FOVER%2FERR", 6, chLabel );
		}
#endif

 		g_App.SetExitProgram();
 		return;
 	}
	
	// send
	SP2Packet kPacket( CTPK_SHUTDOWN_DATE );
	kPacket << m_SelectShutDownAnnouncer.GetYear() << m_SelectShutDownAnnouncer.GetMonth() << m_SelectShutDownAnnouncer.GetDay() << m_SelectShutDownAnnouncer.GetHour();
	TCPNetwork::SendToServer( kPacket );
}

void ioMyInfo::ApplyTournamentTeamList( SP2Packet &rkPacket )
{
	m_UserTournament.ApplyTeamData( rkPacket );
}

bool ioMyInfo::IsTournamentConfirm( DWORD dwTourIndex )
{
	DWORDVec::iterator iter = m_TournamentConfirmList.begin();
	for(;iter != m_TournamentConfirmList.end();iter++)
	{
		DWORD &rkTourIndex = *iter;
		if( rkTourIndex == dwTourIndex )
			return true;
	}
	return false;
}

void ioMyInfo::AddTournamentConfirm( DWORD dwTourIndex )
{
	if( !IsTournamentConfirm( dwTourIndex ) )
	{
		m_TournamentConfirmList.push_back( dwTourIndex );
	}	
}

bool ioMyInfo::IsTournamentConfirmRequest( DWORD dwTourIndex )
{
	TournamentConfirmRequestVec::iterator iter = m_TournamentConfirmRequestList.begin();
	for(;iter != m_TournamentConfirmRequestList.end();iter++)
	{
		TournamentConfirmRequest &rkRequestData = *iter;
		if( rkRequestData.m_dwTourIndex == dwTourIndex )
		{
			if( FRAMEGETTIME() - rkRequestData.m_dwRequestTime < 300000 )
			{
				return true;
			}
		}
	}
	return false;
}

void ioMyInfo::AddTournamentConfirmRequest( DWORD dwTourIndex )
{
	TournamentConfirmRequestVec::iterator iter = m_TournamentConfirmRequestList.begin();
	for(;iter != m_TournamentConfirmRequestList.end();iter++)
	{
		TournamentConfirmRequest &rkRequestData = *iter;
		if( rkRequestData.m_dwTourIndex == dwTourIndex )
		{
			rkRequestData.m_dwRequestTime = FRAMEGETTIME();
			return;
		}
	}

	TournamentConfirmRequest kRequestData;
	kRequestData.m_dwTourIndex = dwTourIndex;
	kRequestData.m_dwRequestTime = FRAMEGETTIME();
	m_TournamentConfirmRequestList.push_back( kRequestData );
}

void ioMyInfo::RemoveTournamentConfirmRequest( DWORD dwTourIndex )
{
	TournamentConfirmRequestVec::iterator iter = m_TournamentConfirmRequestList.begin();
	for(;iter != m_TournamentConfirmRequestList.end();iter++)
	{
		TournamentConfirmRequest &rkRequestData = *iter;
		if( rkRequestData.m_dwTourIndex == dwTourIndex )
		{
			m_TournamentConfirmRequestList.erase( iter );
			return;
		}
	}
}

int ioMyInfo::GetCurGiftCloverCount()
{
	return m_Clover.GetCurGiftCloverCount();
}

void ioMyInfo::UpdateClover()
{
	m_Clover.UpdateClover();
}

void ioMyInfo::ClearBingoIcon()
{
	if( m_pBingo )
	{
		m_pBingo->ClearIconMap();
		m_pBingo->ClearDummyInfo();
	}
}

void ioMyInfo::InsertPcRoomChar( int iClassType )
{
	m_PcRoomCharVec.push_back( iClassType );
}

void ioMyInfo::RemovePcRoomChar( int iClassType )
{
	IntVec::iterator iter = m_PcRoomCharVec.begin();
	for( ; iter != m_PcRoomCharVec.end(); ++iter )
	{
		if( *iter == iClassType )
		{
			m_PcRoomCharVec.erase( iter );
			return;
		}
	}
}

void ioMyInfo::ApplyAttendanceRecord( SP2Packet &rkPacket )
{	
	int iSize = 0;
	rkPacket >> m_dwConnectAttendanceTime;
	rkPacket >> iSize;

	m_AttendanceRecord.clear();

	for( int i = 0; i < iSize; i++ )
	{
		DWORD dwCheckRecord = 0;
		rkPacket >> dwCheckRecord;

		if( dwCheckRecord == 0 )
		{
			LOG.PrintTimeAndLog(0 , "%s:%s - Invalid time value", __FUNCTION__, GetPublicID().c_str() );
			continue;
		}

		m_AttendanceRecord.push_back( dwCheckRecord );
	}
}

void ioMyInfo::AddAttendanceRecord( DWORD dwDate )
{
	m_AttendanceRecord.push_back( dwDate );
}

bool ioMyInfo::HasAttendanceRecord( DWORD dwDate )
{
	AttendanceRecord::const_iterator iter = m_AttendanceRecord.begin();
	for( ; iter != m_AttendanceRecord.end(); ++iter )
	{
		DWORD dwRecordDate = *iter;
		if( dwRecordDate == dwDate )
			return true;
	}

	return false;
}

void ioMyInfo::ResetAttendanceRecord()
{
	m_AttendanceRecord.clear();
}

int ioMyInfo::GetAttendanceRecordCount()
{
	return m_AttendanceRecord.size();
}

DWORD ioMyInfo::GetAttendanceConnectTime()
{
	return m_dwConnectAttendanceTime;
}

void ioMyInfo::ChangeAwakeInfo( int iCharDataIdx, int iAwakeType )
{
	for( int i = 0; i < GetCharCount(); ++i )
	{
		if( COMPARE( i, 0, GetCharCount() ) && m_pCharData->at(i).m_dwIndex == iCharDataIdx )
		{			
			m_pCharData->at(i).m_data.m_iAwakeType = static_cast<AwakeType>( iAwakeType );
			m_SoldierSelectUIBeforeLoading.Update( m_pCharData->at(i) );
			SerUserInfoCharData( m_pCharData->at(i).m_data.m_class_type );
		}
	}
}

void ioMyInfo::ChangeAwakeDate( int iCharDataIdx, int iLimitDate )
{
	for( int i = 0; i < GetCharCount(); ++i )
	{
		if( COMPARE( i, 0, GetCharCount() ) && m_pCharData->at(i).m_dwIndex == iCharDataIdx )
		{
			m_pCharData->at(i).m_data.m_iAwakeLimitTime = iLimitDate;
			SerUserInfoCharData( m_pCharData->at(i).m_data.m_class_type );
		}
	}
}

void ioMyInfo::ChangeAwakeClear( int iCharDataIdx )
{
	for( int i = 0; i < GetCharCount(); ++i )
	{
		if( COMPARE( i, 0, GetCharCount() ) && m_pCharData->at(i).m_dwIndex == iCharDataIdx )
		{
			m_pCharData->at(i).m_data.m_iAwakeLimitTime = 0;
			m_pCharData->at(i).m_data.m_iAwakeType		= AWAKE_NONE;
			m_SoldierSelectUIBeforeLoading.Update( m_pCharData->at(i) );
			SerUserInfoCharData( m_pCharData->at(i).m_data.m_class_type );
		}
	}
}

byte ioMyInfo::GetAwakeType( int iClassType )
{
	const CHARACTER& rkChar = GetCharacterToClassType( iClassType );
	return static_cast<byte>( rkChar.m_iAwakeType );
}

int ioMyInfo::GetAwakeLimitTime( int iClassType )
{
	const CHARACTER& rkChar = GetCharacterToClassType( iClassType );
	return rkChar.m_iAwakeLimitTime;
}

void ioMyInfo::ApplyCharGrade( int iCharIndex, byte eCharGrade )
{
	for( int i = 0 ; i < GetCharCount(); i++ )
	{
		if( m_pCharData->at(i).m_dwIndex == iCharIndex )
		{
			m_pCharData->at(i).m_data.m_eCharPowerUpType = eCharGrade;

			m_ClassExpert.AddPowerUpGrade( m_pCharData->at(i).m_data.m_class_type, eCharGrade );

			SerUserInfoCharData( m_pCharData->at(i).m_data.m_class_type );
		}
	}
}

PowerUpGradeType ioMyInfo::GetCharPowerUpGrade( DWORD dwIndex )
{
	for(int i = 0;i < GetCharCount();i++)
	{
		if( m_pCharData->at(i).m_dwIndex == dwIndex )
		{
			return static_cast<PowerUpGradeType>( m_pCharData->at(i).m_data.m_eCharPowerUpType );
		}		
	}
	return PUGT_NONE;
}

PowerUpGradeType ioMyInfo::GetCharPowerUpGradeByClassType( int iClassType )
{
	for(int i = 0;i < GetCharCount();i++)
	{
		if( m_pCharData->at(i).m_data.m_class_type == iClassType )
		{
			return static_cast<PowerUpGradeType>( m_pCharData->at(i).m_data.m_eCharPowerUpType );
		}		
	}
	return PUGT_NONE;
}

void ioMyInfo::AddPowerUpGrade( int iClassType, byte eCharPowerUpType )
{
	m_ClassExpert.AddPowerUpGrade( iClassType, eCharPowerUpType );
}

// 코스튬 관련 (코스튬 슬롯 변경)
void ioMyInfo::ApplyChangeCostume( int nCharArray, int nSlot, int nSlotIndex, bool bEquip )
{
	if( !COMPARE( nCharArray, 0, GetCharCount() ) ) 
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1));
		LOG.PrintTimeAndLog( 0, "ioMyInfo::ApplyChangeExtraItem Wrong Array : %d", nCharArray );
		return;
	}

	if( !COMPARE( nSlot, 0, MAX_INVENTORY ) ) 
	{
		LOG.PrintTimeAndLog( 0, "ioMyInfo::ApplyChangeExtraItem Wrong Slot : %d", nSlot );
		return;
	}

	// 해제
	if ( !bEquip )
	{
		m_pCharData->at(nCharArray).m_data.m_EquipCostume[nSlot].Init();
		m_UserCostume.SetCostumeEquip( nSlotIndex, false );
	}
	// 장착
	else
	{
		CostumeSlot kSlot;
		if( !m_UserCostume.GetCostumeSlot( nSlotIndex, kSlot ) )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2));
			LOG.PrintTimeAndLog( 0, "ioMyInfo::ApplyChangeExtraItem Wrong SlotIndex : %d", nSlotIndex );
			return;
		}

		int nPreIndex = m_pCharData->at(nCharArray).m_data.m_EquipCostume[nSlot].m_CostumeIdx;
		if( nPreIndex > 0 )
			m_UserCostume.SetCostumeEquip( nPreIndex, false );

		EquipCostumeInit( nCharArray, nSlot, kSlot );

		m_UserCostume.SetCostumeEquip( nSlotIndex, true );
	}

	m_SoldierSelectUIBeforeLoading.Update( m_pCharData->at(nCharArray) );

	SerUserInfoCharData( m_pCharData->at(nCharArray).m_data.m_class_type );

	if( g_BattleRoomMgr.IsBattleRoom() )
	{
		if( g_BattleRoomMgr.GetUserCharIndex( GetPublicID() ) == m_pCharData->at(nCharArray).m_dwIndex )
		{
			SP2Packet kPacket( CUPK_SYNC_CHARINFO );
			kPacket << GetPublicID() << m_pCharData->at(nCharArray).m_dwIndex << m_pCharData->at(nCharArray).m_data;
			FillCharItemData( m_pCharData->at(nCharArray).m_dwIndex, kPacket );
			P2PNetwork::SendToAllUser( kPacket );
		}		
	}
	else if( g_LadderTeamMgr.IsLadderTeam() )
	{
		if( g_LadderTeamMgr.GetUserCharIndex( GetPublicID() ) == m_pCharData->at(nCharArray).m_dwIndex )
		{
			SP2Packet kPacket( CUPK_SYNC_CHARINFO );
			kPacket << GetPublicID() << m_pCharData->at(nCharArray).m_dwIndex << m_pCharData->at(nCharArray).m_data;
			FillCharItemData( m_pCharData->at(nCharArray).m_dwIndex, kPacket );
			P2PNetwork::SendToAllUser( kPacket );
		}	
	}
	else if( g_ShuffleRoomMgr.IsShuffleRoom() )
	{
		if( g_ShuffleRoomMgr.GetUserCharIndex( GetPublicID() ) == m_pCharData->at(nCharArray).m_dwIndex )
		{
			SP2Packet kPacket( CUPK_SYNC_CHARINFO );
			kPacket << GetPublicID() << m_pCharData->at(nCharArray).m_dwIndex << m_pCharData->at(nCharArray).m_data;
			FillCharItemData( m_pCharData->at(nCharArray).m_dwIndex, kPacket );
			P2PNetwork::SendToAllUser( kPacket );
		}		
	}
	else if ( ioPlayMode::GetModeType() == MT_HEADQUARTERS || ioPlayMode::GetModeType() == MT_TRAINING || ioPlayMode::GetModeType() == MT_HOUSE )
	{
		SP2Packet kPacket( CUPK_SYNC_CHARINFO );
		kPacket << GetPublicID() << m_pCharData->at(nCharArray).m_dwIndex << m_pCharData->at(nCharArray).m_data;
		FillCharItemData( m_pCharData->at(nCharArray).m_dwIndex, kPacket );
		P2PNetwork::SendToAllUser( kPacket );
	}
}

bool ioMyInfo::DeleteCostume( int nSlotIndex )
{
	return m_UserCostume.DeleteCostumeSlotData( nSlotIndex );
}

bool ioMyInfo::DeleteCostumeData( SP2Packet &rkPacket )
{
	int nSize;
	rkPacket >> nSize;

	if( nSize <= 0 )
		return false;

	bool bReturn = false;
	for( int i=0; i<nSize; ++i )
	{
		int nSlotIndex;
		rkPacket >> nSlotIndex;

		// 용병 착용상태면 해제후 인벤에서 제거
		int iCharArray = FindEquipedCostumeChar( nSlotIndex );

		if( COMPARE( iCharArray, 0, GetCharCount() ) )
		{
			for( int j=0; j < MAX_INVENTORY; ++j )
			{
				if( m_pCharData->at(iCharArray).m_data.m_EquipCostume[j].m_CostumeIdx == nSlotIndex )
				{
					m_pCharData->at(iCharArray).m_data.m_EquipCostume[j].Init();

					ioHashString szClassName = GetClassName( m_pCharData->at(iCharArray).m_data.m_class_type );

					CostumeSlot kSlot;
					m_UserCostume.GetCostumeSlot( nSlotIndex, kSlot );

					ReleaseCostumeInfo kInfo;
					kInfo.m_szClassName = szClassName;
					kInfo.m_szItemName  = kSlot.m_Name;
					kInfo.m_szIconName  = kSlot.m_IconName;
					kInfo.m_nCostumeCode = kSlot.m_nCostumeCode;

					m_vReserveReleaseCostumeInfoList.push_back( kInfo );
				}
			}
			m_SoldierSelectUIBeforeLoading.Update( m_pCharData->at(iCharArray) );

			SerUserInfoCharData( m_pCharData->at(iCharArray).m_data.m_class_type );
		}

		bReturn = m_UserCostume.DeleteCostumeSlotData( nSlotIndex );
	}
	
	return bReturn;
}

int ioMyInfo::FindEquipedCostumeChar( int iSlotIndex )
{
	int iSize = GetCharCount();
	for( int i=0; i<iSize; i++ )
	{
		for( int j=0; j<MAX_INVENTORY; ++j )
		{
			if( m_pCharData->at(i).m_data.m_EquipCostume[j].m_CostumeIdx == iSlotIndex )
				return i;
		}
	}

	return -1;
}

bool ioMyInfo::IsReserveDeleteCostume()
{
	if( m_vReserveReleaseCostumeInfoList.empty() )
		return false;
	return true;
}

void ioMyInfo::GetFirstDeleteCostume( ioHashString &rkClassName, ioHashString &rkName, ioHashString &rkIconName, int &nCostumeCode )
{
	if( m_vReserveReleaseCostumeInfoList.empty() )
		return;

	rkClassName = m_vReserveReleaseCostumeInfoList[0].m_szClassName;
	rkName      = m_vReserveReleaseCostumeInfoList[0].m_szItemName;
	rkIconName  = m_vReserveReleaseCostumeInfoList[0].m_szIconName;
	nCostumeCode = m_vReserveReleaseCostumeInfoList[0].m_nCostumeCode;
}

void ioMyInfo::EraseDeleteCostume( bool bAllErase )
{
	if( m_vReserveReleaseCostumeInfoList.empty() )
		return;

	if( bAllErase )
		m_vReserveReleaseCostumeInfoList.clear();
	else
		m_vReserveReleaseCostumeInfoList.erase( m_vReserveReleaseCostumeInfoList.begin() );
}

bool ioMyInfo::IsCharRentalToCostume( int iSlotIndex )
{
	for(int i = 0;i < GetCharCount();i++)
	{
		CHARACTER &rkCharInfo = m_pCharData->at(i).m_data;
		for(int k = 0;k < MAX_INVENTORY;k++)
		{
			if( rkCharInfo.m_EquipCostume[k].m_CostumeIdx == iSlotIndex )
			{
				if( rkCharInfo.m_sRentalType == CRT_RENTAL )
					return true;
				return false;
			}
		}
	}
	return false;
}

void ioMyInfo::UpdateEquipCostume( int iSlotIndex )
{
	for(int i = 0;i < (int)m_pCharData->size();i++)
	{
		for( int k=0; k<MAX_INVENTORY; k++ )
		{
			if( m_pCharData->at(i).m_data.m_EquipCostume[k].m_CostumeIdx == iSlotIndex )	// 장비 아이템
			{
				CostumeSlot kSlot;
				if( m_UserCostume.GetCostumeSlot( iSlotIndex, kSlot ) )
				{
					m_pCharData->at(i).m_data.m_EquipCostume[k].m_CostumeCode			= kSlot.m_nCostumeCode;
					m_pCharData->at(i).m_data.m_EquipCostume[k].m_CostumeIdx			= kSlot.m_nIndex;
					m_pCharData->at(i).m_data.m_EquipCostume[k].m_Costume_Male_Custom   = kSlot.m_dwMaleCustom;
					m_pCharData->at(i).m_data.m_EquipCostume[k].m_Costume_Female_Custom	= kSlot.m_dwFemaleCustom;

					m_UserCostume.SetCostumeEquip( iSlotIndex, true );
				}
			}
		}	

		m_SoldierSelectUIBeforeLoading.Update( m_pCharData->at(i) );

		SerUserInfoCharData( m_pCharData->at(i).m_data.m_class_type );
	}
}

void ioMyInfo::EquipCostumeInit( int nCharArray, int nSlot, const CostumeSlot &sCostumeSlot )
{
	if( !COMPARE( nCharArray, 0, GetCharCount() ) ) 
		return;

	if( !COMPARE( nSlot, 0, MAX_INVENTORY ) ) 
		return;

	if ( sCostumeSlot.m_nIndex <= 0 )
		return;

	m_pCharData->at(nCharArray).m_data.m_EquipCostume[nSlot].m_CostumeIdx = sCostumeSlot.m_nIndex;
	m_pCharData->at(nCharArray).m_data.m_EquipCostume[nSlot].m_CostumeCode = sCostumeSlot.m_nCostumeCode;
	m_pCharData->at(nCharArray).m_data.m_EquipCostume[nSlot].m_Costume_Male_Custom = sCostumeSlot.m_dwMaleCustom;
	m_pCharData->at(nCharArray).m_data.m_EquipCostume[nSlot].m_Costume_Female_Custom = sCostumeSlot.m_dwFemaleCustom;
}


//추가 장비 교체 실패 관련 임시 변수
void ioMyInfo::SetExtraItemChange( int iType, int iCharArr, int iSlot, int iIndex )
{
	m_iEquipType      = iType;
	m_iCharArray      = iCharArr;
	m_iEquipSlot      = iSlot;
	m_iExtraItemIndex = iIndex;
}

void ioMyInfo::GetExtraItemChange( int& iType, int& iCharArr, int& iSlot, int& iIndex )
{
	iType	 = m_iEquipType;
	iCharArr = m_iCharArray;
	iSlot	 = m_iEquipSlot;
	iIndex	 = m_iExtraItemIndex;

	m_iEquipType	  = 0;
	m_iCharArray	  = 0;
	m_iEquipSlot	  = 0;
	m_iExtraItemIndex = 0;

}

void ioMyInfo::CalcRCharPossessionCount()
{
	m_iRSoldierPossessionCount = 0;
	int iMaxCount = GetCharCount();
	for ( int i=0 ; i<iMaxCount ; ++i )
	{
		if ( !g_RSoldierInfoManager.IsRSoldierType( m_pCharData->at(i).m_data ) )
			continue;
		m_iRSoldierPossessionCount++;
	}
}

bool ioMyInfo::IsHouseOpen()
{
	ioUserEtcItem::ETCITEMSLOT kHome;
	ioUserEtcItem* pInven = GetUserEtcItem();
	if( pInven && pInven->GetEtcItem( ioEtcItem::EIT_ETC_CREATE_HOME, kHome ) )
	{
		time_t kLimitTime = DateHelp::ConvertSecondTime( kHome.GetYear(), kHome.GetMonth(), kHome.GetDay(), kHome.GetHour(), kHome.GetMinute(), 0 );
		time_t kCurServerTime = GetServerDate();
		kCurServerTime += ( REALGETTIME()/1000 );

		int iGapSec = kLimitTime - kCurServerTime;
		if( iGapSec > 0 )
		{
			return true;
		}
	}

	return false;
}

// 칭호 관련 (칭호 슬롯 변경)
void ioMyInfo::ApplyEquipChangeNamedTitle( DWORD dwCode, BYTE byEquip )
{	
	// 해제
	m_pUserNamedTitle.ClearNamedTitleEquip();

	// 장착
	if ( byEquip )
	{
		NamedTitleSlot kNewEquipSlot;
		if( !m_pUserNamedTitle.GetNamedTitleSlot( dwCode, kNewEquipSlot ) )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "해당 칭호가 존재하지 않습니다." );
			LOG.PrintTimeAndLog( 0, "ioMyInfo::ApplyChangeNamedTitle Wrong SlotIndex : %d", dwCode );
			return;
		}

		m_pUserNamedTitle.SetNamedTitleEquip( dwCode );
	}
}

// 칭호 관련 (칭호 등급 변경)
void ioMyInfo::ApplyGradeChangeNamedTitle( DWORD dwCode, int nGrade )
{	
	NamedTitleSlot kNewEquipSlot;
	if( !m_pUserNamedTitle.GetNamedTitleSlot( dwCode, kNewEquipSlot ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "해당 칭호가 존재하지 않습니다." );
		LOG.PrintTimeAndLog( 0, "ioMyInfo::ApplyChangeNamedTitle Wrong SlotIndex : %d", dwCode );
		return;
	}

	m_pUserNamedTitle.SetNamedTitleGrade( dwCode, nGrade );
}

// 칭호 관련 (칭호 프리미엄 변경)
void ioMyInfo::ApplyPremiumChangeNamedTitle( DWORD dwCode )
{	
	NamedTitleSlot kNewEquipSlot;
	if( !m_pUserNamedTitle.GetNamedTitleSlot( dwCode, kNewEquipSlot ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "해당 칭호가 존재하지 않습니다." );
		LOG.PrintTimeAndLog( 0, "ioMyInfo::ApplyChangeNamedTitle Wrong SlotIndex : %d", dwCode );
		return;
	}

	m_pUserNamedTitle.SetNamedTitlePremium( dwCode );
}

DWORD ioMyInfo::GetNamedTitleCode()
{
	NamedTitleSlot kEquipSlot;
	if ( !m_pUserNamedTitle.GetNamedTitleSlotEquip( kEquipSlot) )
		return 0;

	return kEquipSlot.m_dwCode;
}

int ioMyInfo::GetNamedTitleLevel()
{
	NamedTitleSlot kEquipSlot;
	if ( !m_pUserNamedTitle.GetNamedTitleSlotEquip( kEquipSlot) )
		return 0;

	return kEquipSlot.m_nLevel;
}

BYTE ioMyInfo::GetNamedTitlePremium()
{
	NamedTitleSlot kEquipSlot;
	if ( !m_pUserNamedTitle.GetNamedTitleSlotEquip( kEquipSlot) )
		return false;

	return kEquipSlot.m_byPremium;
}

void ioMyInfo::CalcSoldierSetCnt()
{
	m_vSoldierSetCntVec.clear();
	
	int nMaxCount = GetCharCount();
	for ( int i=0; i<g_RSoldierInfoManager.GetSoldierSetDataMaxCnt(); i++ )
	{
		int nCharSetCnt = 0;
		for ( int j=0; j<nMaxCount; j++ )
		{
			if ( !g_RSoldierInfoManager.IsSoldierSet( m_pCharData->at(j).m_data ) )
				continue;

			int nSetNum = g_RSoldierInfoManager.GetSoldierSetNum( m_pCharData->at(j).m_data.m_class_type );
			if ( nSetNum > 0 && nSetNum == i+1)
				nCharSetCnt++;
		}
		m_vSoldierSetCntVec.push_back( nCharSetCnt );
	}
}

int ioMyInfo::GetSoldierSetCnt( int nSetCode )
{
	if ( m_vSoldierSetCntVec.empty() || !COMPARE( nSetCode-1, 0, (int)m_vSoldierSetCntVec.size() ) )
		return 0;

	return m_vSoldierSetCntVec[nSetCode-1];
}

void ioMyInfo::GetSoldierSetCnt( IntVec& vSoldierSetCnt )
{
	vSoldierSetCnt.clear();

	for ( int i=0; i<(int)m_vSoldierSetCntVec.size(); i++ )
		vSoldierSetCnt.push_back( m_vSoldierSetCntVec[i] );
}

//보너스 캐쉬
void ioMyInfo::SetBonusCash( int iCash )
{
	m_user_data.m_iBonusCash = iCash;
}

int ioMyInfo::GetBonusCash() const
{
	return m_user_data.m_iBonusCash;
}

void ioMyInfo::ApplyChangeAccessory( int iCharArray, int iSlot, int iSlotIndex, bool bEquip )
{
	if( !COMPARE( iCharArray, 0, GetCharCount() ) ) 
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "예외 오류 발생1");
		LOG.PrintTimeAndLog( 0, "%s Wrong Array : %d", __FUNCTION__, iCharArray );
		return;
	}

	if( !COMPARE( iSlot, 0, MAX_ACC_INVENTORY ) ) 
	{
		LOG.PrintTimeAndLog( 0, "%s Wrong Slot : %d", __FUNCTION__, iSlot );
		return;
	}

	// 해제
	if ( !bEquip )
	{
		m_pCharData->at(iCharArray).m_data.m_EquipAccessory[iSlot].Init();
		m_UserAccessory.SetAccessoryEquip( iSlotIndex, false );
	}
	// 장착
	else
	{
		AccessorySlot kSlot;
		if( !m_UserAccessory.GetAccessorySlot( iSlotIndex, kSlot ) )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "예외 오류 발생2");
			LOG.PrintTimeAndLog( 0, "%s Wrong SlotIndex : %d", __FUNCTION__, iSlotIndex );
			return;
		}

		int iPreIndex = m_pCharData->at(iCharArray).m_data.m_EquipAccessory[iSlot].m_iAccessoryIdx;
		if( iPreIndex > 0 )
			m_UserAccessory.SetAccessoryEquip( iPreIndex, false );

		EquipAccessoryInit( iCharArray, iSlot, kSlot );

		m_UserAccessory.SetAccessoryEquip( iSlotIndex, true );
	}

	m_SoldierSelectUIBeforeLoading.Update( m_pCharData->at(iCharArray) );

	SerUserInfoCharData( m_pCharData->at(iCharArray).m_data.m_class_type );

	if( g_BattleRoomMgr.IsBattleRoom() )
	{
		if( g_BattleRoomMgr.GetUserCharIndex( GetPublicID() ) == m_pCharData->at(iCharArray).m_dwIndex )
		{
			SP2Packet kPacket( CUPK_SYNC_CHARINFO );
			kPacket << GetPublicID() << m_pCharData->at(iCharArray).m_dwIndex << m_pCharData->at(iCharArray).m_data;
			FillCharItemData( m_pCharData->at(iCharArray).m_dwIndex, kPacket );
			P2PNetwork::SendToAllUser( kPacket );
		}		
	}
	else if( g_LadderTeamMgr.IsLadderTeam() )
	{
		if( g_LadderTeamMgr.GetUserCharIndex( GetPublicID() ) == m_pCharData->at(iCharArray).m_dwIndex )
		{
			SP2Packet kPacket( CUPK_SYNC_CHARINFO );
			kPacket << GetPublicID() << m_pCharData->at(iCharArray).m_dwIndex << m_pCharData->at(iCharArray).m_data;
			FillCharItemData( m_pCharData->at(iCharArray).m_dwIndex, kPacket );
			P2PNetwork::SendToAllUser( kPacket );
		}	
	}
	else if( g_ShuffleRoomMgr.IsShuffleRoom() )
	{
		if( g_ShuffleRoomMgr.GetUserCharIndex( GetPublicID() ) == m_pCharData->at(iCharArray).m_dwIndex )
		{
			SP2Packet kPacket( CUPK_SYNC_CHARINFO );
			kPacket << GetPublicID() << m_pCharData->at(iCharArray).m_dwIndex << m_pCharData->at(iCharArray).m_data;
			FillCharItemData( m_pCharData->at(iCharArray).m_dwIndex, kPacket );
			P2PNetwork::SendToAllUser( kPacket );
		}		
	}
	else if ( ioPlayMode::GetModeType() == MT_HEADQUARTERS || ioPlayMode::GetModeType() == MT_TRAINING || ioPlayMode::GetModeType() == MT_HOUSE )
	{
		SP2Packet kPacket( CUPK_SYNC_CHARINFO );
		kPacket << GetPublicID() << m_pCharData->at(iCharArray).m_dwIndex << m_pCharData->at(iCharArray).m_data;
		FillCharItemData( m_pCharData->at(iCharArray).m_dwIndex, kPacket );
		P2PNetwork::SendToAllUser( kPacket );
	}
}

bool ioMyInfo::DeleteAccessory( int iSlotIndex )
{
	return m_UserAccessory.DeleteAccessorySlotData( iSlotIndex );
}

bool ioMyInfo::DeleteAccessoryData( SP2Packet &rkPacket )
{
	int nSize;
	rkPacket >> nSize;

	if( nSize <= 0 )
		return false;

	bool bReturn = false;
	for( int i=0; i<nSize; ++i )
	{
		int nSlotIndex;
		rkPacket >> nSlotIndex;

		// 용병 착용상태면 해제후 인벤에서 제거
		int iCharArray = FindEquipedAccessoryChar( nSlotIndex );

		if( COMPARE( iCharArray, 0, GetCharCount() ) )
		{
			for( int j=0; j < MAX_ACC_INVENTORY; ++j )
			{
				if( m_pCharData->at(iCharArray).m_data.m_EquipAccessory[j].m_iAccessoryIdx == nSlotIndex )
				{
					m_pCharData->at(iCharArray).m_data.m_EquipAccessory[j].Init();

					ioHashString szClassName = GetClassName( m_pCharData->at(iCharArray).m_data.m_class_type );

					AccessorySlot kSlot;
					m_UserAccessory.GetAccessorySlot( nSlotIndex, kSlot );

					ReleaseAccessoryInfo kInfo;
					kInfo.m_szClassName = szClassName;
					kInfo.m_szItemName  = kSlot.GetName();
					kInfo.m_szIconName  = kSlot.m_IconName;
					kInfo.m_iAccessoryCode = kSlot.m_iAccessoryCode;

					m_vReserveReleaseAccessoryInfoList.push_back( kInfo );
				}
			}
			m_SoldierSelectUIBeforeLoading.Update( m_pCharData->at(iCharArray) );

			SerUserInfoCharData( m_pCharData->at(iCharArray).m_data.m_class_type );
		}

		bReturn = m_UserAccessory.DeleteAccessorySlotData( nSlotIndex );
	}

	return bReturn;
}

int ioMyInfo::FindEquipedAccessoryChar( int iSlotIndex )
{
	int iSize = GetCharCount();
	for( int i=0; i<iSize; i++ )
	{
		for( int j=0; j<MAX_INVENTORY; ++j )
		{
			if( m_pCharData->at(i).m_data.m_EquipAccessory[j].m_iAccessoryIdx == iSlotIndex )
				return i;
		}
	}

	return -1;
}

bool ioMyInfo::IsReserveDeleteAccessory()
{
	if( m_vReserveReleaseAccessoryInfoList.empty() )
		return false;
	return true;
}

void ioMyInfo::GetFirstDeleteAccessory( ioHashString &rkClassName, ioHashString &rkName, ioHashString &rkIconName, int &nAccessoryCode )
{
	if( m_vReserveReleaseAccessoryInfoList.empty() )
		return;

	rkClassName = m_vReserveReleaseAccessoryInfoList[0].m_szClassName;
	rkName      = m_vReserveReleaseAccessoryInfoList[0].m_szItemName;
	rkIconName  = m_vReserveReleaseAccessoryInfoList[0].m_szIconName;
	nAccessoryCode = m_vReserveReleaseAccessoryInfoList[0].m_iAccessoryCode;
}

void ioMyInfo::EraseDeleteAccessory( bool bAllErase )
{
	if( m_vReserveReleaseAccessoryInfoList.empty() )
		return;

	if( bAllErase )
		m_vReserveReleaseAccessoryInfoList.clear();
	else
		m_vReserveReleaseAccessoryInfoList.erase( m_vReserveReleaseAccessoryInfoList.begin() );
}

bool ioMyInfo::IsCharRentalToAccessory( int iSlotIndex )
{
	for(int i = 0;i < GetCharCount();i++)
	{
		CHARACTER &rkCharInfo = m_pCharData->at(i).m_data;
		for(int k = 0;k < MAX_INVENTORY;k++)
		{
			if( rkCharInfo.m_EquipAccessory[k].m_iAccessoryIdx == iSlotIndex )
			{
				if( rkCharInfo.m_sRentalType == CRT_RENTAL )
					return true;
				return false;
			}
		}
	}
	return false;
}

void ioMyInfo::UpdateEquipAccessory( int iSlotIndex )
{
	for(int i = 0;i < (int)m_pCharData->size();i++)
	{
		for( int k=0; k<MAX_ACC_INVENTORY; k++ )
		{
			if( m_pCharData->at(i).m_data.m_EquipAccessory[k].m_iAccessoryIdx == iSlotIndex )	// 장비 아이템
			{
				AccessorySlot kSlot;
				if( m_UserAccessory.GetAccessorySlot( iSlotIndex, kSlot ) )
				{
					m_pCharData->at(i).m_data.m_EquipAccessory[k].m_iAccessoryCode			= kSlot.m_iAccessoryCode;
					m_pCharData->at(i).m_data.m_EquipAccessory[k].m_iAccessoryIdx			= kSlot.m_iIndex;
					m_UserAccessory.SetAccessoryEquip( iSlotIndex, true );
				}
			}
		}	

		m_SoldierSelectUIBeforeLoading.Update( m_pCharData->at(i) );

		SerUserInfoCharData( m_pCharData->at(i).m_data.m_class_type );
	}
}

void ioMyInfo::EquipAccessoryInit( int iCharArray, int iSlot, const AccessorySlot &sAccessorySlot )
{
	if( !COMPARE( iCharArray, 0, GetCharCount() ) ) 
		return;

	if( !COMPARE( iSlot, 0, MAX_INVENTORY ) ) 
		return;

	if ( sAccessorySlot.m_iIndex <= 0 )
		return;

	m_pCharData->at(iCharArray).m_data.m_EquipAccessory[iSlot].m_iAccessoryIdx = sAccessorySlot.m_iIndex;
	m_pCharData->at(iCharArray).m_data.m_EquipAccessory[iSlot].m_iAccessoryCode = sAccessorySlot.m_iAccessoryCode;
	m_pCharData->at(iCharArray).m_data.m_EquipAccessory[iSlot].m_iRandomNumber = sAccessorySlot.m_iRandomNumber;
	m_pCharData->at(iCharArray).m_data.m_EquipAccessory[iSlot].m_iCompoundCode = sAccessorySlot.m_iCompoundCode;
	m_pCharData->at(iCharArray).m_data.m_EquipAccessory[iSlot].m_iCompoundValue = sAccessorySlot.m_iCompoundValue;
}