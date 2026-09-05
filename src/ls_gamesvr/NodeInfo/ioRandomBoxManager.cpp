#include "stdafx.h"

#include "../DataBase/DBClient.h"
#include "../DataBase/LogDBClient.h"

#include "../MainProcess.h"
#include "../EtcHelpFunc.h"

#include "User.h"
#include "ioRandomBoxManager.h"
#include "ioPresentHelper.h"
#include "../MainServerNode/MainServerNode.h"
#include "../NodeInfo/ioEtcItemManager.h"

extern CLog EventLOG;
template<> ioRandomBoxManager* Singleton< ioRandomBoxManager >::ms_Singleton = 0;

ioRandomBoxManager::ioRandomBoxManager()
{
	m_pRandomboxManager = NULL;
}

ioRandomBoxManager& ioRandomBoxManager::GetSingleton()
{
	return Singleton<ioRandomBoxManager>::GetSingleton();
}

ioRandomBoxManager::~ioRandomBoxManager()
{
	if( m_pRandomboxManager )
	{
		m_pRandomboxManager->Release();
		SAFEDELETE( m_pRandomboxManager );
	}

	for(mRandomBoxInfo::iterator iter = m_mRandomBoxInfoList.begin(); iter != m_mRandomBoxInfoList.end(); ++iter)
	{
		RandomBoxInfo *rkInfo = iter->second;
		rkInfo->m_vRandomBoxCategoryInfoList.clear();
	}
	m_mRandomBoxInfoList.clear();
}

void ioRandomBoxManager::LoadINI()
{
	LoadRandomBoxPackage();
}

void ioRandomBoxManager::CheckNeedReload()
{
	LoadRandomBoxPackage();
}


void ioRandomBoxManager::LoadRandomBoxPackage()
{
	if( m_pRandomboxManager )
	{
		m_pRandomboxManager->Release();
		SAFEDELETE( m_pRandomboxManager );
	}

	m_pRandomboxManager = new LSC_New_Gashapon_info_Manager;
	m_pRandomboxManager->LoadData( RANDOM_BOX_INFO_TABLE );

	for(mRandomBoxInfo::iterator iter = m_mRandomBoxInfoList.begin(); iter != m_mRandomBoxInfoList.end(); ++iter)
	{
		RandomBoxInfo* rkInfo = iter->second;
		rkInfo->m_vRandomBoxCategoryInfoList.clear();
	}
	m_mRandomBoxInfoList.clear();

	// 로드완료되면 
	EventLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "LoadRandomBoxPackage INI Start!!!!!!!!!!" );

    for (int i = 0; i < m_pRandomboxManager->GetTotal(); ++i)
    {
        LSC_New_Gashapon_info* pkInfo = m_pRandomboxManager->GetAt(i);
        if (NULL == pkInfo)
        {
			continue;
        }

		if(pkInfo->EtcItemCode == 0)
		{
			continue;
		}

		bool bCreate = false;
		RandomBoxInfo* pkBoxInfo = GetRandomBoxInfo(pkInfo->EtcItemCode);
		if(pkBoxInfo == NULL)
		{
			bCreate = true;
			pkBoxInfo = new RandomBoxInfo;
			pkBoxInfo->m_vRandomBoxCategoryInfoList.clear();
			pkBoxInfo->m_dwEtcItemCode		= pkInfo->EtcItemCode;
			pkBoxInfo->m_dwRandomBoxSeed	= 10000;
		}

		if(bCreate == true)
		{
			m_mRandomBoxInfoList.insert( mRandomBoxInfo::value_type( pkBoxInfo->m_dwEtcItemCode, pkBoxInfo ) );
		}
		
		LoadCategory( pkInfo, pkBoxInfo);
	}
}


void ioRandomBoxManager::LoadCategory( LSC_New_Gashapon_info* pkInfo, RandomBoxInfo* pkBoxInfo )
{
	bool bCategoryCreate = true;
	RandomBoxCategoryInfo* pkCategoryInfo = NULL;
	if(pkBoxInfo->m_vRandomBoxCategoryInfoList.size() > 0)
	{
		for( vRandomBoxCategoryInfo::iterator iter = pkBoxInfo->m_vRandomBoxCategoryInfoList.begin(); iter < pkBoxInfo->m_vRandomBoxCategoryInfoList.end(); ++iter )
		{
			RandomBoxCategoryInfo* pkCategory = *iter;
			if(pkCategory->m_dwCategoryIndex == pkInfo->CategoryIndex)
			{
				bCategoryCreate = false;
				pkCategoryInfo = pkCategory;
				break;
			}
		}
	}

	if(bCategoryCreate == true)
	{
		pkCategoryInfo						= new RandomBoxCategoryInfo;
		pkCategoryInfo->m_dwCategoryIndex	= pkInfo->CategoryIndex;
		pkCategoryInfo->m_dwCategoryType	= pkInfo->CategoryType;
		pkCategoryInfo->m_dwCategoryRand	= pkInfo->Category_Rand;
		pkBoxInfo->m_vRandomBoxCategoryInfoList.push_back( pkCategoryInfo );
	}

	bool bPackageCreate = true;
	RandomBoxPackage* pkPackageInfo;
	if(pkCategoryInfo->m_vRandomBoxPackageList.size() > 0)
	{
		for( vRandomBoxPackage::iterator iter = pkCategoryInfo->m_vRandomBoxPackageList.begin(); iter < pkCategoryInfo->m_vRandomBoxPackageList.end(); ++iter )
		{
			RandomBoxPackage* pkPackage = *iter;
			if(pkPackage->m_dwIndex == pkInfo->PackageIndex)
			{
				bPackageCreate = false;
				pkPackageInfo = pkPackage;
				break;
			}
		}
	}
	
	if(bPackageCreate == true)
	{
		pkPackageInfo					= new RandomBoxPackage;
		pkPackageInfo->m_dwIndex		= pkInfo->PackageIndex;
		pkPackageInfo->m_dwRand			= pkInfo->Category_Package_Rand;
		pkPackageInfo->m_bWholeAlarm	= pkInfo->Package_Alarm;
		pkCategoryInfo->m_dwRandomBoxPackageSeed	+= pkInfo->Category_Package_Rand;
		pkCategoryInfo->m_vRandomBoxPackageList.push_back(pkPackageInfo);
	}

	sPackageBoxElement* pkElement = new sPackageBoxElement;
	pkElement->m_iPresentType	= pkInfo->Category_Package_Element_Type;
	pkElement->m_iPresentValue1	= pkInfo->Category_Package_Element_Value1;
	pkElement->m_iPresentValue2	= pkInfo->Category_Package_Element_Value2;
	pkElement->m_iBoxMent		= pkInfo->Category_Package_Ment;
	pkElement->m_iBoxPeriod		= pkInfo->Period;
	pkPackageInfo->m_vPackageBoxElement.push_back(pkElement);
}


ioRandomBoxManager::RandomBoxInfo* ioRandomBoxManager::GetRandomBoxInfo( DWORD dwEtcItemCode )
{
	RandomBoxInfo* pkInfo = NULL;
	mRandomBoxInfo::iterator iter = m_mRandomBoxInfoList.find( dwEtcItemCode );
	if( iter != m_mRandomBoxInfoList.end() )
	{
		pkInfo = iter->second;
	}

	return pkInfo;
}


bool ioRandomBoxManager::SendRandomBoxRandPackage( User *pSendUser, DWORD dwEtcItemCode, vector<cBoxInfo> &vPackageBox )
{	
	if( !pSendUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pSendUser == NULL.(%d)", __FUNCTION__ );
		return false;
	}

	RandomBoxInfo* pkInfo = GetRandomBoxInfo( dwEtcItemCode );
	if( pkInfo == NULL)
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s kInfo == NULL.(%d)", __FUNCTION__, dwEtcItemCode );
		return false;
	}

	for( vRandomBoxCategoryInfo::iterator iter = pkInfo->m_vRandomBoxCategoryInfoList.begin(); iter < pkInfo->m_vRandomBoxCategoryInfoList.end(); ++iter )
	{
		RandomBoxCategoryInfo* pkCategory = *iter;
		DWORD dwRand = pkInfo->m_RandomBoxRandom.Random(pkInfo->m_dwRandomBoxSeed);

		if( false == COMPARE( dwRand, 0, pkCategory->m_dwCategoryRand ) )
		{
			continue;
		}

		DWORD dwPakageRand = pkInfo->m_RandomBoxRandom.Random(pkCategory->m_dwRandomBoxPackageSeed);

		DWORD dwCurValue = 0;
		for( vRandomBoxPackage::iterator iter = pkCategory->m_vRandomBoxPackageList.begin(); iter < pkCategory->m_vRandomBoxPackageList.end(); ++iter )
		{
			RandomBoxPackage* pkPackage = *iter;

			if( false == COMPARE( dwPakageRand, dwCurValue, dwCurValue + pkPackage->m_dwRand ) )
			{
				dwCurValue += pkPackage->m_dwRand;
				continue;
			}

			SendRandomBoxSelectPackage( pSendUser, dwEtcItemCode, pkPackage, g_MainServer.GetSendID() );
			// 정보 생성
			cBoxInfo kBoxInfo;
			kBoxInfo.dwCategoryIndex	= pkCategory->m_dwCategoryIndex;
			kBoxInfo.dwPackageIndex		= pkPackage->m_dwIndex;
			kBoxInfo.bPackageAlarm		= pkPackage->m_bWholeAlarm;
			vPackageBox.push_back( kBoxInfo );
			break;
		}
	}

	if(vPackageBox.size() > 0)
	{
		return true;
	}

	return false;
}


void ioRandomBoxManager::SendRandomBoxSelectPackage( User *pSendUser, DWORD dwEtcItemCode, const RandomBoxPackage* pkPackage, const ioHashString& szGashaponSendID )
{
	if( !pSendUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pSendUser == NULL.(%d)", __FUNCTION__ );
		return;
	}

	IORandom Rand;
	Rand.SetRandomSeed( timeGetTime() );


	for( vPackageBoxElement::const_iterator iter = pkPackage->m_vPackageBoxElement.begin(); iter < pkPackage->m_vPackageBoxElement.end(); ++iter )
	{
		sPackageBoxElement* pkElement = *iter;

		int iMent = pkElement->m_iBoxMent;
		int	iPeriod = pkElement->m_iBoxPeriod;

#ifdef __OHTG_SOLDIER_TAKE_CHANGE__
		bool bPresent = false;
		if( pkElement->m_iPresentType == IT_SOLDIER )
		{
			bPresent = pSendUser->SendSoldierTake(pkElement->m_iPresentValue1, pkElement->m_iPresentValue2);
		}

		if(false == bPresent)
		{
			CTimeSpan cPresentGapTime( iPeriod, 0, 0, 0 );
			CTime kPresentTime = CTime::GetCurrentTime() + cPresentGapTime;
			pSendUser->AddPresentMemory( szGashaponSendID, pkElement->m_iPresentType, pkElement->m_iPresentValue1, pkElement->m_iPresentValue2, 0, 0, iMent, kPresentTime, ioUserPresent::PRESENT_STATE_NORMAL );
		}

		char szNote[MAX_PATH]="";
		StringCbPrintf( szNote, sizeof( szNote ) , "SuperGashapon : %d", dwEtcItemCode );
		g_LogDBClient.OnInsertPresent( 0, szGashaponSendID, g_App.GetPublicIP().c_str(), pSendUser->GetUserIndex(), pkElement->m_iPresentType, pkElement->m_iPresentValue1, pkElement->m_iPresentValue2, 0, 0, LogDBClient::PST_RECIEVE, szNote );
#else //__OHTG_SOLDIER_TAKE_CHANGE__
		CTimeSpan cPresentGapTime( iPeriod, 0, 0, 0 );
		CTime kPresentTime = CTime::GetCurrentTime() + cPresentGapTime;
		pSendUser->AddPresentMemory( szGashaponSendID, pkElement->m_iPresentType, pkElement->m_iPresentValue1, pkElement->m_iPresentValue2, 0, 0, iMent, kPresentTime, ioUserPresent::PRESENT_STATE_NORMAL );

		char szNote[MAX_PATH]="";
		StringCbPrintf( szNote, sizeof( szNote ) , "SuperGashapon : %d", dwEtcItemCode );
		g_LogDBClient.OnInsertPresent( 0, szGashaponSendID, g_App.GetPublicIP().c_str(), pSendUser->GetUserIndex(), pkElement->m_iPresentType, pkElement->m_iPresentValue1, pkElement->m_iPresentValue2, 0, 0, LogDBClient::PST_RECIEVE, szNote );
#endif //__OHTG_SOLDIER_TAKE_CHANGE__
	}
}
