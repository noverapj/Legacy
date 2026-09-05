#include "stdafx.h"

#include "../DataBase/DBClient.h"
#include "../DataBase/LogDBClient.h"

#include "../MainProcess.h"
#include "../EtcHelpFunc.h"

#include "User.h"
#include "ioSuperGashaponMgr.h"
#include "ioPresentHelper.h"
#include "../MainServerNode/MainServerNode.h"
#include "../NodeInfo/ioEtcItemManager.h"

extern CLog EventLOG;
template<> ioSuperGashaponMgr* Singleton< ioSuperGashaponMgr >::ms_Singleton = 0;

ioSuperGashaponMgr::ioSuperGashaponMgr()
{

}

ioSuperGashaponMgr& ioSuperGashaponMgr::GetSingleton()
{
	return Singleton<ioSuperGashaponMgr>::GetSingleton();
}

ioSuperGashaponMgr::~ioSuperGashaponMgr()
{
	for(vSuperGashaponPackageInfo::iterator iter = m_vSuperGashaponPackageInfoList.begin(); iter != m_vSuperGashaponPackageInfoList.end(); ++iter)
	{
		SuperGashaponPackageInfo &rkInfo = (*iter);
		rkInfo.m_vSuperGashaponPackageList.clear();
	}
	m_vSuperGashaponPackageInfoList.clear();
}

void ioSuperGashaponMgr::LoadINI()
{
	ioINILoader kSuperGashponLoader;
	kSuperGashponLoader.ReloadFile( "config/sp2_super_gashapon_present.ini" );
	LoadSuperGashaponPackage( kSuperGashponLoader );
}

void ioSuperGashaponMgr::CheckNeedReload()
{
	ioINILoader kSuperGashponLoader;
	kSuperGashponLoader.ReloadFile( "config/sp2_super_gashapon_present.ini" );
	LoadSuperGashaponPackage( kSuperGashponLoader );
// 	kSuperGashponLoader.SetFileName( "config/sp2_super_gashapon_present.ini" );
// 	if( kSuperGashponLoader.ReadBool( "Common", "Change", false ) )
// 	{
// 		kSuperGashponLoader.ReloadFile( "config/sp2_super_gashapon_present.ini" );
// 		LoadSuperGashaponPackage( kSuperGashponLoader );
// 	}
}

#define NOTFOUND_USEDCOUNT -1000

void ioSuperGashaponMgr::LoadSuperGashaponPackage( ioINILoader &rkLoader )
{
	EventLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "LoadSuperGashaponPackage INI Start!!!!!!!!!!" );

	for(vSuperGashaponPackageInfo::iterator iter = m_vSuperGashaponPackageInfoList.begin(); iter != m_vSuperGashaponPackageInfoList.end(); ++iter)
	{
		SuperGashaponPackageInfo &rkInfo = (*iter);
		rkInfo.m_vSuperGashaponPackageList.clear();
	}
	m_vSuperGashaponPackageInfoList.clear();

	sprintf_s( m_szUsedCountININame, "sp2_super_gashapon_present_leftovercount_%d.ini",  GetPrivateProfileInt("Default", "CSPORT", 9000, g_App.GetINI().c_str()) );

	rkLoader.SetTitle( "Common" );
	int iMaxInfo = rkLoader.LoadInt( "MaxInfo", 0 );
	for ( int iInfoCnt = 0; iInfoCnt < iMaxInfo; ++iInfoCnt )
	{
		SuperGashaponPackageInfo kInfo;
		kInfo.m_vSuperGashaponPackageList.clear();
		kInfo.m_dwSuperGashaponPackageSeed = 0;
		
		ZeroMemory( m_szBuffer, sizeof( m_szBuffer ) );
		char szName[MAX_PATH]="";
		StringCbPrintf( szName, sizeof( szName ), "GashaponPresent%d", iInfoCnt+1 );
		rkLoader.SetTitle( szName );

		kInfo.m_dwEtcItemType  = rkLoader.LoadInt( "EtcItemType", 0 );

		kInfo.m_dwLimit = rkLoader.LoadInt( "MaxLimit", 0 );
		kInfo.m_iSuperGashaponPeriod = rkLoader.LoadInt( "Period", 0 );

		LoadPackage( rkLoader, kInfo );//상품
		std::sort( kInfo.m_vSuperGashaponPackageList.begin(), kInfo.m_vSuperGashaponPackageList.end(), SuperGashaponPackageSort() );
		EventLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "LoadSuperGashaponPackage INI [%d] End Max RandSeed : %d And Max Package : %d", kInfo.m_dwEtcItemType, kInfo.m_dwSuperGashaponPackageSeed, (int)kInfo.m_vSuperGashaponPackageList.size() );
		kInfo.m_SuperGashaponPackageRandom.Randomize();


		if( 0 < kInfo.m_dwLimit )
		{
			LoadPackage( rkLoader, kInfo, true );	//대체상품
			kInfo.m_SuperGashaponSubPackageRandom.Randomize();

			std::sort( kInfo.m_vSuperGashaponSubPackageList.begin(), kInfo.m_vSuperGashaponSubPackageList.end(), SuperGashaponPackageSort() );
			EventLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "LoadSuperGashaponSubPackage INI [%d] End Max RandSeed : %d And Max Package : %d", kInfo.m_dwEtcItemType, kInfo.m_dwSuperGashaponSubPackageSeed, (int)kInfo.m_vSuperGashaponSubPackageList.size() );
		}
		
		m_vSuperGashaponPackageInfoList.push_back( kInfo );
	}
}

void ioSuperGashaponMgr::LoadPackage( ioINILoader &rkLoader, SuperGashaponPackageInfo& rkInfo, bool bSubPackage )
{
	ZeroMemory( m_szBuffer, sizeof( m_szBuffer ) );
	if( bSubPackage )		
		sprintf_s( m_szBuffer, "%s", "Sub" );
			
	sprintf_s( m_szKey, "%s%s", m_szBuffer, "MaxPackage" );
	int	iMaxItem = rkLoader.LoadInt( m_szKey, 0 );

	ioINILoader kLoaderUsedCount;
	kLoaderUsedCount.ReloadFile( m_szUsedCountININame );

	for(int i = 0; i < iMaxItem; ++i )
	{
		SuperGashaponPackage Package;
		
		Package.m_dwIndex = i;		

		sprintf_s( m_szKey, "%sPackage%d_limit", m_szBuffer, i + 1 );
		Package.m_iLimit = rkLoader.LoadInt( m_szKey, -1 );

		sprintf_s( m_szKey, "%sPackage%d_Rand", m_szBuffer, i + 1 );
		Package.m_dwRand = rkLoader.LoadInt( m_szKey, 0 );
		if( Package.m_dwRand == 0 )
		{
			EventLOG.PrintTimeAndLog( 0, "[%02d] Rand is zero. it is not setting(super gashpon).", i + 1 );
			continue;
		}

		sprintf_s( m_szKey, "%sPackage%d_Ment", m_szBuffer, i + 1 );
		Package.m_iSuperGashaponMent = rkLoader.LoadInt( m_szKey, 0 );

		// 전체 알림 여부
		sprintf_s( m_szKey, "%sPackage%d_Alaram", m_szBuffer, i + 1 );
		Package.m_bWholeAlarm = rkLoader.LoadBool( m_szKey, 0 );

		LoadPackageElement( rkLoader, Package, i, bSubPackage );


		char csItemType[128] = "\0";
		if ( bSubPackage )
		{
			sprintf_s(csItemType, "%d_sub", rkInfo.m_dwEtcItemType);
		}
		else
		{
			sprintf_s(csItemType, "%d", rkInfo.m_dwEtcItemType);
		}

		kLoaderUsedCount.SetTitle( csItemType );

		char csPackageNumber[128] = "\0";
		sprintf_s(csPackageNumber, "%d", i + 1);
		int iLeftOverCount = kLoaderUsedCount.LoadInt( csPackageNumber, NOTFOUND_USEDCOUNT );
		if ( iLeftOverCount != NOTFOUND_USEDCOUNT && iLeftOverCount < 1 )
		{
			//이러면 다 나온거니까 그냥 패스
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][supergasha] RELOAD BUT SOLD OUT [%d][%d]", rkInfo.m_dwEtcItemType, i + 1 );
			continue;
		}
		if ( iLeftOverCount > 0 )
		{
			Package.m_iLimit = iLeftOverCount;
		}

		if( bSubPackage )
		{
			rkInfo.m_vSuperGashaponSubPackageList.push_back( Package );
			rkInfo.m_dwSuperGashaponSubPackageSeed += Package.m_dwRand;
			EventLOG.PrintTimeAndLog( 0, "%02d] (%05d:%05d)(super gashpon subpackage) = %d : %d : %d", i + 1, rkInfo.m_dwSuperGashaponSubPackageSeed, rkInfo.m_dwSuperGashaponSubPackageSeed + Package.m_dwRand );
		}
		else
		{
			rkInfo.m_vSuperGashaponPackageList.push_back( Package );
			rkInfo.m_dwSuperGashaponPackageSeed += Package.m_dwRand;
			EventLOG.PrintTimeAndLog( 0, "%02d] (%05d:%05d)(super gashpon package) = %d : %d : %d", i + 1, rkInfo.m_dwSuperGashaponPackageSeed, rkInfo.m_dwSuperGashaponPackageSeed + Package.m_dwRand );
		}
	}
}

void ioSuperGashaponMgr::LoadPackageElement( ioINILoader &rkLoader, SuperGashaponPackage& rkPackage, int iPackageIndex, bool bSubPackage )
{
	ZeroMemory( m_szBuffer, sizeof( m_szBuffer ) );
	if( bSubPackage )
		sprintf_s( m_szBuffer, "%s", "Sub" );

	sprintf_s( m_szKey, "%sPackage%d_Element_Cnt", m_szBuffer, iPackageIndex + 1 );
	int iElementCnt = rkLoader.LoadInt( m_szKey, 0 );
	
	for( int j = 0; j < iElementCnt; ++j )
	{
		SuperGashaponElement kElement;
		sprintf_s( m_szKey, "%sPackage%d_Element%d_Type", m_szBuffer, iPackageIndex + 1, j + 1  );
		kElement.m_iPresentType = (short)rkLoader.LoadInt( m_szKey, 0 );
		sprintf_s( m_szKey, "%sPackage%d_Element%d_Value1", m_szBuffer, iPackageIndex + 1, j + 1  );
		kElement.m_iPresentValue1 = rkLoader.LoadInt( m_szKey, 0 );
		sprintf_s( m_szKey, "%sPackage%d_Element%d_Value2", m_szBuffer, iPackageIndex + 1, j + 1  );
		kElement.m_iPresentValue2 = rkLoader.LoadInt( m_szKey, 0 );
		sprintf_s( m_szKey, "%sPackage%d_Element%d_Ment", m_szBuffer, iPackageIndex + 1, j + 1  );
		kElement.m_iSuperGashaponMent = rkLoader.LoadInt( m_szKey, 0 );
		sprintf_s( m_szKey, "%sPackage%d_Element%d_Period", m_szBuffer, iPackageIndex + 1, j + 1  );
		kElement.m_iSuperGashaponPeriod = rkLoader.LoadInt( m_szKey, 0 );

		rkPackage.m_vPackageElement.push_back( kElement );

		EventLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%02d] (%05d:%05d)(super gashpon element) = %d, %d, %d, %d", j + 1, kElement.m_iPresentType, kElement.m_iPresentValue1, kElement.m_iPresentValue2 );
	}
}

ioSuperGashaponMgr::SuperGashaponPackageInfo *ioSuperGashaponMgr::GetSuperGashaponPackageInfo( DWORD dwEtcItemType )
{
	for(vSuperGashaponPackageInfo::iterator iter = m_vSuperGashaponPackageInfoList.begin(); iter != m_vSuperGashaponPackageInfoList.end(); ++iter)
	{
		SuperGashaponPackageInfo &kInfo = (*iter);
		if( kInfo.m_dwEtcItemType == dwEtcItemType )
			return &kInfo;
	}	

	return NULL;
}

DWORD ioSuperGashaponMgr::GetSuperGashaponPackageLimit( DWORD dwEtcItemType )
{
	SuperGashaponPackageInfo *pInfo = GetSuperGashaponPackageInfo( dwEtcItemType );
	if( !pInfo )
		return 0;

	return pInfo->m_dwLimit;
}

void ioSuperGashaponMgr::SendSuperGashaponSelectPackage( User *pSendUser, DWORD dwEtcItemType, const SuperGashaponPackage &rkPackage, int iPeriod, const ioHashString& szGashaponSendID )
{
	if( !pSendUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pSendUser == NULL.(%d)", __FUNCTION__ );
		return;
	}

	for( vSuperGashaponElement::const_iterator iter = rkPackage.m_vPackageElement.begin(); iter < rkPackage.m_vPackageElement.end(); ++iter )
	{
		const SuperGashaponElement rkElement = *iter;

		int iMent = rkPackage.m_iSuperGashaponMent;
		
		if( 0 < rkElement.m_iSuperGashaponMent )
			iMent = rkElement.m_iSuperGashaponMent;
		if( 0 < rkElement.m_iSuperGashaponPeriod )
			iPeriod = rkElement.m_iSuperGashaponPeriod;


#ifdef __OHTG_SOLDIER_TAKE_CHANGE__
		bool bPresent = false;
		if( rkElement.m_iPresentType == IT_SOLDIER )
		{
			bPresent = pSendUser->SendSoldierTake(rkElement.m_iPresentValue1, rkElement.m_iPresentValue2);
		}

		if(false == bPresent)
		{
			CTimeSpan cPresentGapTime( iPeriod, 0, 0, 0 );
			CTime kPresentTime = CTime::GetCurrentTime() + cPresentGapTime;
			pSendUser->AddPresentMemory( szGashaponSendID, rkElement.m_iPresentType, rkElement.m_iPresentValue1, rkElement.m_iPresentValue2, 0, 0, iMent, kPresentTime, ioUserPresent::PRESENT_STATE_NORMAL );
		}

		char szNote[MAX_PATH]="";
		StringCbPrintf( szNote, sizeof( szNote ) , "SuperGashapon : %d", dwEtcItemType );
		g_LogDBClient.OnInsertPresent( 0, szGashaponSendID, g_App.GetPublicIP().c_str(), pSendUser->GetUserIndex(), rkElement.m_iPresentType, rkElement.m_iPresentValue1, rkElement.m_iPresentValue2, 0, 0, LogDBClient::PST_RECIEVE, szNote );
#else //__OHTG_SOLDIER_TAKE_CHANGE__
		CTimeSpan cPresentGapTime( iPeriod, 0, 0, 0 );
		CTime kPresentTime = CTime::GetCurrentTime() + cPresentGapTime;
		pSendUser->AddPresentMemory( szGashaponSendID, rkElement.m_iPresentType, rkElement.m_iPresentValue1, rkElement.m_iPresentValue2, 0, 0, iMent, kPresentTime, ioUserPresent::PRESENT_STATE_NORMAL );

		char szNote[MAX_PATH]="";
		StringCbPrintf( szNote, sizeof( szNote ) , "SuperGashapon : %d", dwEtcItemType );
		g_LogDBClient.OnInsertPresent( 0, szGashaponSendID, g_App.GetPublicIP().c_str(), pSendUser->GetUserIndex(), rkElement.m_iPresentType, rkElement.m_iPresentValue1, rkElement.m_iPresentValue2, 0, 0, LogDBClient::PST_RECIEVE, szNote );
#endif //__OHTG_SOLDIER_TAKE_CHANGE__
	}
}

bool ioSuperGashaponMgr::SendSuperGashaponRandPackage( User *pSendUser, DWORD dwEtcItemType, DWORD& dwPackageIndex, bool & bPackageWholeAlarm )
{	
	if( !pSendUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pSendUser == NULL.(%d)", __FUNCTION__ );
		return false;
	}

	SuperGashaponPackageInfo *pInfo = GetSuperGashaponPackageInfo( dwEtcItemType );
	if( !pInfo )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pInfo == NULL.(%d)", __FUNCTION__, dwEtcItemType );
		return false;
	}

	DWORD dwRand = pInfo->m_SuperGashaponPackageRandom.Random( pInfo->m_dwSuperGashaponPackageSeed );
	DWORD dwCurValue = 0;
	//Information("%s Get Rand[%d][%d][%d]\n", __FUNCTION__, dwEtcItemType, dwRand, pInfo->m_dwSuperGashaponPackageSeed);

	for( vSuperGashaponPackage::iterator iter = pInfo->m_vSuperGashaponPackageList.begin(); iter < pInfo->m_vSuperGashaponPackageList.end(); ++iter )
	{
		SuperGashaponPackage &rkPackage = *iter;
		if( COMPARE( dwRand, dwCurValue, dwCurValue + rkPackage.m_dwRand ) )
		{
#ifdef __OHTG_SOLDIER_TAKE_CHANGE__
			/* 테스트용
			rkPackage.m_vPackageElement[0].m_iPresentValue1 = 13;
			IORandom mRand;
			mRand.SetRandomSeed( timeGetTime() );
			dwRand = mRand.Random( 0, 2 );
			if(dwRand == 0)
			{
				rkPackage.m_vPackageElement[0].m_iPresentValue2 = 0;
			}
			*/
#endif //__OHTG_SOLDIER_TAKE_CHANGE__

			SendSuperGashaponSelectPackage( pSendUser, dwEtcItemType, rkPackage, pInfo->m_iSuperGashaponPeriod, g_MainServer.GetSendID() );
			dwPackageIndex = rkPackage.m_dwIndex;

			// 알람 여부
			bPackageWholeAlarm = rkPackage.m_bWholeAlarm;

			if ( rkPackage.m_iLimit > 0 )
			{
				rkPackage.m_iLimit--;
				int iRemindCount = rkPackage.m_iLimit;
				if ( rkPackage.m_iLimit < 1 )
				{
					pInfo->m_dwSuperGashaponPackageSeed -= rkPackage.m_dwRand;
					if ( pInfo->m_dwSuperGashaponPackageSeed < 1 ) pInfo->m_dwSuperGashaponPackageSeed = 0;
					pInfo->m_vSuperGashaponPackageList.erase(iter);
					Information("[info][supergasha] SOLD OUT [%d][%d]\n", dwEtcItemType, rkPackage.m_dwIndex);
					LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][supergasha] SOLD OUT [%d][%d]", dwEtcItemType, rkPackage.m_dwIndex );
				}
				else
				{
					LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][supergasha] SOLD REMIND [%d][%d][%d]", dwEtcItemType, rkPackage.m_dwIndex, rkPackage.m_iLimit );
				}
				// INI 저장
				ioINILoader kLoader;
				kLoader.LoadFile( m_szUsedCountININame, true );
				char csItemType[MAX_PATH]="";
				StringCbPrintf( csItemType, sizeof( csItemType ) , "%d", dwEtcItemType );
				kLoader.SetTitle( csItemType );
				char csPackageIndex[MAX_PATH]="";
				StringCbPrintf( csPackageIndex, sizeof( csPackageIndex ) , "%d", dwPackageIndex + 1 );
				kLoader.SaveInt( csPackageIndex, iRemindCount );
				Information("%s Minus[%d][%d]\n", __FUNCTION__, dwEtcItemType, rkPackage.m_iLimit);
			}

			return true;
		}
		dwCurValue += rkPackage.m_dwRand;
		Information("%s Pass[%d][%d][%d][%d]\n", __FUNCTION__, dwEtcItemType, dwRand, dwCurValue, dwCurValue + rkPackage.m_dwRand);
	}
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s None Rand Value[%d][%d]", __FUNCTION__, dwEtcItemType, dwRand );
	Information("[warning][supergasha] None Rand Value[%d][%d]\n", dwEtcItemType, dwRand);
	return false;
}


bool ioSuperGashaponMgr::SendSuperGashaponRandSubPackage( User *pSendUser, DWORD dwEtcItemType, DWORD& dwSubPackageIndex )
{
	if( !pSendUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pSendUser == NULL.(%d)", __FUNCTION__ );
		return false;
	}

	SuperGashaponPackageInfo *pInfo = GetSuperGashaponPackageInfo( dwEtcItemType );
	if( !pInfo )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pInfo == NULL.(%d)", __FUNCTION__, dwEtcItemType );
		return false;
	}

	DWORD dwRand = pInfo->m_SuperGashaponSubPackageRandom.Random( pInfo->m_dwSuperGashaponSubPackageSeed );
	DWORD dwCurValue = 0;

	for( vSuperGashaponPackage::iterator iter = pInfo->m_vSuperGashaponSubPackageList.begin(); iter < pInfo->m_vSuperGashaponSubPackageList.end(); ++iter )
	{
		SuperGashaponPackage &rkPackage = *iter;
		if( COMPARE( dwRand, dwCurValue, dwCurValue + rkPackage.m_dwRand ) )
		{
			SendSuperGashaponSelectPackage( pSendUser, dwEtcItemType, rkPackage, pInfo->m_iSuperGashaponPeriod, g_MainServer.GetSendID() );
			dwSubPackageIndex = rkPackage.m_dwIndex;

			if ( rkPackage.m_iLimit > 0 )
			{
				rkPackage.m_iLimit--;
				int iRemindCount = rkPackage.m_iLimit;
				if ( rkPackage.m_iLimit < 1 )
				{
					pInfo->m_dwSuperGashaponSubPackageSeed -= rkPackage.m_dwRand;
					if ( pInfo->m_dwSuperGashaponSubPackageSeed < 1 ) pInfo->m_dwSuperGashaponSubPackageSeed = 0;
					pInfo->m_vSuperGashaponPackageList.erase(iter);
					LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s SOLD OUT [%d][%d]", __FUNCTION__, dwEtcItemType, rkPackage.m_dwIndex );
				}
				// INI 저장
				ioINILoader kLoader;
				kLoader.LoadFile( m_szUsedCountININame, true );
				char csItemType[MAX_PATH]="";
				StringCbPrintf( csItemType, sizeof( csItemType ) , "%d_sub", dwEtcItemType );
				kLoader.SetTitle( csItemType );
				char csPackageIndex[MAX_PATH]="";
				StringCbPrintf( csPackageIndex, sizeof( csPackageIndex ) , "%d", dwSubPackageIndex + 1 );
				kLoader.SaveInt( csPackageIndex, iRemindCount );
			}

			return true;
		}
		dwCurValue += rkPackage.m_dwRand;
	}
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s None Rand Value[%d][%d]", __FUNCTION__, dwEtcItemType, dwRand );

	return false;
}

bool ioSuperGashaponMgr::SendSuperGashaponAllPackage( User *pSendUser, DWORD dwEtcItemType )
{
	if( !pSendUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pSendUser == NULL.(%d)", __FUNCTION__ );
		return false;
	}

	SuperGashaponPackageInfo *pInfo = GetSuperGashaponPackageInfo( dwEtcItemType );
	if( !pInfo )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pInfo == NULL.(%d)", __FUNCTION__, dwEtcItemType );
		return false;
	}
		
	for( vSuperGashaponPackage::const_iterator iter = pInfo->m_vSuperGashaponPackageList.begin(); iter < pInfo->m_vSuperGashaponPackageList.end(); ++iter )
	{
		const SuperGashaponPackage &rkPackage = *iter;
		SendSuperGashaponSelectPackage( pSendUser, dwEtcItemType, rkPackage, pInfo->m_iSuperGashaponPeriod, g_MainServer.GetSendID() );
	}

	for( vSuperGashaponPackage::const_iterator iter = pInfo->m_vSuperGashaponSubPackageList.begin(); iter < pInfo->m_vSuperGashaponSubPackageList.end(); ++iter )
	{
		const SuperGashaponPackage &rkPackage = *iter;
		SendSuperGashaponSelectPackage( pSendUser, dwEtcItemType, rkPackage, pInfo->m_iSuperGashaponPeriod, g_MainServer.GetSendID() );
	}
	
	return true;
}

bool ioSuperGashaponMgr::SendSuperGashaponPackage( User *pSendUser, DWORD dwEtcItemType, DWORD dwPackageIndex )
{
	if( !pSendUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pSendUser == NULL.(%d)", __FUNCTION__ );
		return false;
	}

	SuperGashaponPackageInfo *pInfo = GetSuperGashaponPackageInfo( dwEtcItemType );
	if( !pInfo )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pInfo == NULL.(%d)", __FUNCTION__, dwEtcItemType );
		return false;
	}
	else if( dwPackageIndex < 0 )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s dwPresentIndex < 0(%d)", __FUNCTION__, dwPackageIndex );
		return false;
	}
	else if( dwPackageIndex >= (DWORD)pInfo->m_vSuperGashaponPackageList.size() )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s dwPresentIndex > list size(%d)", __FUNCTION__, pInfo->m_vSuperGashaponPackageList.size() );
		return false;
	}

	for( vSuperGashaponPackage::const_iterator iter = pInfo->m_vSuperGashaponPackageList.begin(); iter < pInfo->m_vSuperGashaponPackageList.end(); ++iter )
	{
		const SuperGashaponPackage &rkPackage = *iter;
		if( rkPackage.m_dwIndex == dwPackageIndex )
		{
			SendSuperGashaponSelectPackage( pSendUser, dwEtcItemType, rkPackage, pInfo->m_iSuperGashaponPeriod, g_MainServer.GetSendID() );
			return true;
		}
	}	

	return false;
}

bool ioSuperGashaponMgr::FindSuperGashaponPackageRandom( User *pSendUser, DWORD dwEtcItemType, DWORD& rdwPackageIndex, DWORD& rdwPackageLimitMax )
{
	if( !pSendUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pSendUser == NULL.(%d)", __FUNCTION__ );
		return false;
	}

	SuperGashaponPackageInfo *pInfo = GetSuperGashaponPackageInfo( dwEtcItemType );
	if( !pInfo )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pInfo == NULL.(%d)", __FUNCTION__, dwEtcItemType );
		return false;
	}

	DWORD dwRand = pInfo->m_SuperGashaponPackageRandom.Random( pInfo->m_dwSuperGashaponPackageSeed );
	DWORD dwCurValue = 0;
	DWORD dwIndex = 0;
	for( vSuperGashaponPackage::const_iterator iter = pInfo->m_vSuperGashaponPackageList.begin(); iter < pInfo->m_vSuperGashaponPackageList.end(); ++iter, ++dwIndex )
	{
		const SuperGashaponPackage &rkPackage = *iter;
		if( COMPARE( dwRand, dwCurValue, dwCurValue + rkPackage.m_dwRand ) )
		{
			rdwPackageIndex = rkPackage.m_dwIndex;
			rdwPackageLimitMax = pInfo->m_dwLimit;
			return true;
		}
		dwCurValue += rkPackage.m_dwRand;
	}
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s None Rand Value[%d][%d]", __FUNCTION__, dwEtcItemType, dwRand );

	return false;
}

// 2019-03-11 by bckim, 티메가테 
bool ioSuperGashaponMgr::FindSuperGashaponPackageRandomPresent( User *pSendUser, DWORD dwEtcItemType, DWORD& dwPackageIndex)
{
	if( !pSendUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pSendUser == NULL.(%d)", __FUNCTION__ );
		return false;
	}

	SuperGashaponPackageInfo *pInfo = GetSuperGashaponPackageInfo( dwEtcItemType );
	if( !pInfo )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pInfo == NULL.(%d)", __FUNCTION__, dwEtcItemType );
		return false;
	}

	DWORD dwRand = pInfo->m_SuperGashaponPackageRandom.Random( pInfo->m_dwSuperGashaponPackageSeed );
	DWORD dwCurValue = 0;
	DWORD dwIndex = 0;
	for( vSuperGashaponPackage::const_iterator iter = pInfo->m_vSuperGashaponPackageList.begin(); iter < pInfo->m_vSuperGashaponPackageList.end(); ++iter, ++dwIndex )
	{
		const SuperGashaponPackage &rkPackage = *iter;
		if( COMPARE( dwRand, dwCurValue, dwCurValue + rkPackage.m_dwRand ) )
		{
			//dwPackageIndex = rkPackage.m_dwIndex;
			vSuperGashaponElement temp = rkPackage.m_vPackageElement;
			if( temp.size() == 1) 
			{
				dwPackageIndex = temp[0].m_iPresentValue1;								
			}
			else 
			{
				return false;
			}

			DWORD rdwPackageLimitMax = pInfo->m_dwLimit;
			return true;
		}
		dwCurValue += rkPackage.m_dwRand;
	}
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s None Rand Value[%d][%d]", __FUNCTION__, dwEtcItemType, dwRand );

	return false;
}
// End. 2019-03-11 by bckim, 티메가테 

// 2019-07-03 by bckim, 파밍모드 추가
bool ioSuperGashaponMgr::FindSuperGashaponPackageRandomPresent( DWORD dwEtcItemType, DWORD& dwPackageIndex)
{
	SuperGashaponPackageInfo *pInfo = GetSuperGashaponPackageInfo( dwEtcItemType );
	if( !pInfo )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pInfo == NULL.(%d)", __FUNCTION__, dwEtcItemType );
		return false;
	}

	DWORD dwRand = pInfo->m_SuperGashaponPackageRandom.Random( pInfo->m_dwSuperGashaponPackageSeed );
	DWORD dwCurValue = 0;
	DWORD dwIndex = 0;
	for( vSuperGashaponPackage::const_iterator iter = pInfo->m_vSuperGashaponPackageList.begin(); iter < pInfo->m_vSuperGashaponPackageList.end(); ++iter, ++dwIndex )
	{
		const SuperGashaponPackage &rkPackage = *iter;
		if( COMPARE( dwRand, dwCurValue, dwCurValue + rkPackage.m_dwRand ) )
		{	
			vSuperGashaponElement temp = rkPackage.m_vPackageElement;
			if( temp.size() == 1) 
				dwPackageIndex = temp[0].m_iPresentValue1;	
			else 
				return false;

			DWORD rdwPackageLimitMax = pInfo->m_dwLimit;
			return true;
		}
		dwCurValue += rkPackage.m_dwRand;
	}
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s None Rand Value[%d][%d]", __FUNCTION__, dwEtcItemType, dwRand );
	return false;
}
// End. 2019-07-03 by bckim, 파밍모드 추가



bool ioSuperGashaponMgr::IsLimitGashapon( DWORD dwEtcItemType )
{
	SuperGashaponPackageInfo *pInfo = GetSuperGashaponPackageInfo( dwEtcItemType );
	if( !pInfo )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pInfo == NULL.(%d)", __FUNCTION__, dwEtcItemType );
		return false;
	}

	if( 0 < pInfo->m_dwLimit )
		return true;

	return false;
}

bool ioSuperGashaponMgr::SendSuperGashponLimitCheck( User *pSendUser, DWORD dwEtcItemType, int iUseType )
{
	if( !pSendUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pSendUser == NULL.(%d)", __FUNCTION__ );
		return false;
	}

	DWORD dwPackageIndex = 0;
	DWORD dwPackageLimitMax = 0;

	ioEtcItem* pEtcItem = g_EtcItemMgr.FindEtcItem( dwEtcItemType );
	if( !pEtcItem )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s etcitem not find(%d)", __FUNCTION__, dwEtcItemType );
		return false;
	}

	if( FindSuperGashaponPackageRandom( pSendUser, dwEtcItemType, dwPackageIndex, dwPackageLimitMax ) )
	{		
		SP2Packet kPacket( MSTPK_SUPER_GASHPON_LIMIT_CHECK );
		kPacket << pSendUser->GetUserIndex() << dwEtcItemType << dwPackageIndex << dwPackageLimitMax << iUseType;
		g_MainServer.SendMessage( kPacket );
		return true;
	}

	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Present not find(%d, %d)", __FUNCTION__, dwEtcItemType, dwPackageIndex );
	return false;
}