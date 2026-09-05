#include "../stdafx.h"
#include "./goodsmanager.h"
#include <strsafe.h>
#include "../Local\ioLocalParent.h"

extern CLog LOG;

template<> GoodsManager* Singleton< GoodsManager >::ms_Singleton = 0;

GoodsManager::GoodsManager(void)
{
	m_vSoldierInfo.reserve(10);
	m_iMortmainCharGoodsNoValue = 0;
	m_vPackageEtcItemShortTypeList.reserve(10);
	m_vPackageExtraItemMachineCodeList.reserve(10);
}

GoodsManager::~GoodsManager(void)
{
	m_GoodsInfoMap.clear();
	m_vSoldierInfo.clear();
	m_vPackageEtcItemShortTypeList.clear();
	m_vPackageExtraItemMachineCodeList.clear();
}

bool GoodsManager::LoadINI( const char *szFileName, const char *szGoodsNameFile )
{
	ioINILoader kLoader( szFileName );
	
	// SoldierInfo
	kLoader.SetTitle( "SoldierInfo" );
	int iMax = kLoader.LoadInt( "Max", 0 );

	if( iMax <= 0 )
	{
		LOG.PrintTimeAndLog(0, "GoodsManager::LoadINI Max is Error : %d", iMax );
		return false;
	}

	for (int i = 0; i < iMax ; i++)
	{
		char szKeyName[MAX_PATH]="";
		StringCbPrintf( szKeyName, sizeof( szKeyName ), "LimitSeconds_%d", i+1 );

		SoldierInfo kInfo;
		kInfo.m_iLimitSeconds = kLoader.LoadInt( szKeyName, 0 );
		if( kInfo.m_iLimitSeconds <= 0 )
		{
			LOG.PrintTimeAndLog(0, "GoodsManager::LoadINI LimitSeconds is Error : [%d] %d", i+1, kInfo.m_iLimitSeconds );
			return false;
		}

		ZeroMemory( szKeyName, sizeof(szKeyName) );
		StringCbPrintf( szKeyName, sizeof( szKeyName ), "GoodsNoValue_%d", i+1 );
		kInfo.m_iGoodsNoValue = kLoader.LoadInt( szKeyName , 0 );

		if( kInfo.m_iGoodsNoValue <= 0 )
		{
			LOG.PrintTimeAndLog(0, "GoodsManager::LoadINI GoodsNoValue is Error : [%d] %d", i+1, kInfo.m_iGoodsNoValue );
			return false;
		}

		m_vSoldierInfo.push_back( kInfo );
	}
	
	m_iMortmainCharGoodsNoValue = kLoader.LoadInt( "MortmainCharGoodsNoValue", 0 );
	char szTempName[MAX_PATH]="";
	kLoader.LoadString( "PresentAddGoodsName", "", szTempName, sizeof( szTempName) );
	m_szPresentAddGoodsName = szTempName;

	// GoodsList
	ioINILoader kGoodsNameLoader( szGoodsNameFile );
	
	kGoodsNameLoader.SetTitle( "GoodsList" );

	bool bLoadGoodsNameList = false;
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal && pLocal->IsLoadGoodsNameList() )
		bLoadGoodsNameList = true;

	kLoader.SetTitle( "GoodsList" );
	iMax = kLoader.LoadInt( "Max", 0 );

	if( iMax <= 0 )
	{
		LOG.PrintTimeAndLog(0, "GoodsManager::LoadINI Goods Max is Error : %d", iMax );
		return false;
	}

	for (int i = 0; i < iMax ; i++)
	{
		char szKeyName[MAX_PATH]="";
		StringCbPrintf( szKeyName, sizeof( szKeyName ), "GoodsNo_%d", i+1 );
		DWORD dwGoodsNo = kLoader.LoadInt( szKeyName, 0 );
		if( dwGoodsNo == 0 )
		{
			LOG.PrintTimeAndLog(0, "[error][goods]GoodsManager::LoadINI GoodsNo is Zero [%d]", i+1 );
			return false;
		}

		ZeroMemory( szKeyName, sizeof(szKeyName) );
		ioHashString szGoodsName;

		if( bLoadGoodsNameList )
		{
			// 새로운 INI에서 읽는다.
			StringCbPrintf( szKeyName, sizeof( szKeyName ), "GoodsName_%d", i+1 );
			char szTempName[MAX_PATH]="";
			kGoodsNameLoader.LoadString( szKeyName, "", szTempName, sizeof( szTempName) );
			szGoodsName = szTempName;
		}
		else
		{
			StringCbPrintf( szKeyName, sizeof( szKeyName ), "GoodsName_%d", i+1 );
			char szTempName[MAX_PATH]="";
			kLoader.LoadString( szKeyName, "", szTempName, sizeof( szTempName) );
			szGoodsName = szTempName;
		}

		if( szGoodsName.IsEmpty() )
		{
			LOG.PrintTimeAndLog(0, "[error][goods]GoodsManager::LoadINI GoodsName is Empty [%d]", i+1 );
			szGoodsName = "unknown";
			//return false;
		}
		
		GoodsInfoMap::const_iterator iter = m_GoodsInfoMap.find( dwGoodsNo );
		if( iter != m_GoodsInfoMap.end() )
		{
			ioHashString szOutGoodsName;
			szOutGoodsName = iter->second;
			LOG.PrintTimeAndLog(0, "GoodsManager::LoadINI Duplication Goods No. [%d][%d:%s]", i+1 , dwGoodsNo, szOutGoodsName.c_str() );
			return false;
		}

		m_GoodsInfoMap.insert( GoodsInfoMap::value_type( dwGoodsNo, szGoodsName ) );
	}

	kLoader.SetTitle( "PackageShortTypeList" );
	iMax = kLoader.LoadInt( "Max", 0 );
	for (int i = 0; i < iMax ; i++)
	{
		char szKeyName[MAX_PATH]="";
		StringCbPrintf( szKeyName, sizeof( szKeyName ), "ShortType_%d", i+1 );
		DWORD dwShortType = kLoader.LoadInt( szKeyName, 0 );
		if( dwShortType == 0 )
		{
			LOG.PrintTimeAndLog(0, "[error][goods]GoodsManager::LoadINI iShortType is Zero [%d]", i+1 );
			return false;
		}

		m_vPackageEtcItemShortTypeList.push_back( dwShortType );
	}

	kLoader.SetTitle( "PackageExtraItemMachineCodeList" );
	iMax = kLoader.LoadInt( "Max", 0 );
	for (int i = 0; i < iMax ; i++)
	{
		char szKeyName[MAX_PATH]="";
		StringCbPrintf( szKeyName, sizeof( szKeyName ), "MachineCode_%d", i+1 );
		DWORD dwShortType = kLoader.LoadInt( szKeyName, 0 );
		if( dwShortType == 0 )
		{
			LOG.PrintTimeAndLog(0, "[error][goods]GoodsManager::LoadINI MachineCode is Zero [%d]", i+1 );
			return false;
		}

		m_vPackageExtraItemMachineCodeList.push_back( dwShortType );
	}

	int iCnt= 0 ;
	LOG.PrintTimeAndLog( 0, "<< Goods List Start >>" );
	for(GoodsInfoMap::iterator iter = m_GoodsInfoMap.begin(); iter != m_GoodsInfoMap.end(); ++iter)
	{
		LOG.PrintTimeAndLog( 0, "[%04d] %u %s", ++iCnt, iter->first, iter->second.c_str() );

		/* 정상구매 되는지 테스트 코드
		IIPGClientPtr ptrClient;
		ptrClient.CreateInstance(__uuidof(IPGClient));

		_variant_t vsUserNo, vsUserID, vsCompanyCD, vsGoodsNo, vsPayAmt, vsAgencyNo, vsUserIP, vsEtc1, vsEtc2;// input
		_variant_t vsBxaid, vsRetMsg;    // output

		vsUserNo    = (LPSTR) "cashtest01"; 
		vsUserID    = (LPSTR) "cashtest01";
		vsCompanyCD = (LPSTR) "LOSA"; 	
		vsGoodsNo   = (LONG)  iter->first;
		vsPayAmt    = (LONG)  1;
		vsUserIP    = (LPSTR) "192.168.1.103";
		vsEtc1      = (LPSTR) "Test";
		vsEtc2      = (LPSTR) "cashtest01";

		// 값을 받을때까지 대기 
		ptrClient->OutputIPG(&vsUserNo, &vsUserID, &vsCompanyCD, &vsGoodsNo,&vsPayAmt ,&vsAgencyNo, &vsUserIP, &vsEtc1
			                ,&vsEtc1, &vsBxaid, &vsRetMsg ); 
		*/
	}
	LOG.PrintTimeAndLog( 0, "<< Goods List End >>" );

	iCnt= 0 ;
	LOG.PrintTimeAndLog( 0, "<< Package Short Type List Start >>" );
	for(DWORDVec::iterator iter = m_vPackageEtcItemShortTypeList.begin(); iter != m_vPackageEtcItemShortTypeList.end(); ++iter)
	{
	    LOG.PrintTimeAndLog( 0, "[%04d] %u", ++iCnt, (*iter) );
	}
	LOG.PrintTimeAndLog( 0, "<< Package Short Type List End >>" );


	iCnt= 0 ;
	LOG.PrintTimeAndLog( 0, "<< Package Extra item MacineCode List Start >>" );
	for(DWORDVec::iterator iter = m_vPackageExtraItemMachineCodeList.begin(); iter != m_vPackageExtraItemMachineCodeList.end(); ++iter)
	{
		LOG.PrintTimeAndLog( 0, "[%04d] %u", ++iCnt, (*iter) );
	}
	LOG.PrintTimeAndLog( 0, "<< Package Extra item MacineCode End >>" );

	return true;
}

bool GoodsManager::ReloadINI( const char *szFileName, const char *szGoodsNameFile )
{
	ioINILoader kLoader;
	kLoader.ReloadFile( szFileName );
	
	// SoldierInfo
	kLoader.SetTitle( "SoldierInfo" );
	int iMax = kLoader.LoadInt( "Max", 0 );

	if( iMax <= 0 )
	{
		LOG.PrintTimeAndLog(0, "Reload GoodsManager::LoadINI Max is Error : %d", iMax );
		return false;
	}

	m_vSoldierInfo.clear();
	for (int i = 0; i < iMax ; i++)
	{
		char szKeyName[MAX_PATH]="";
		StringCbPrintf( szKeyName, sizeof( szKeyName ), "LimitSeconds_%d", i+1 );

		SoldierInfo kInfo;
		kInfo.m_iLimitSeconds = kLoader.LoadInt( szKeyName, 0 );
		if( kInfo.m_iLimitSeconds <= 0 )
		{
			LOG.PrintTimeAndLog(0, "Reload GoodsManager::LoadINI LimitSeconds is Error : [%d] %d", i+1, kInfo.m_iLimitSeconds );
			return false;
		}

		ZeroMemory( szKeyName, sizeof(szKeyName) );
		StringCbPrintf( szKeyName, sizeof( szKeyName ), "GoodsNoValue_%d", i+1 );
		kInfo.m_iGoodsNoValue = kLoader.LoadInt( szKeyName , 0 );

		if( kInfo.m_iGoodsNoValue <= 0 )
		{
			LOG.PrintTimeAndLog(0, "Reload GoodsManager::LoadINI GoodsNoValue is Error : [%d] %d", i+1, kInfo.m_iGoodsNoValue );
			return false;
		}

		m_vSoldierInfo.push_back( kInfo );
	}
	
	m_iMortmainCharGoodsNoValue = kLoader.LoadInt( "MortmainCharGoodsNoValue", 0 );
	char szTempName[MAX_PATH]="";
	kLoader.LoadString( "PresentAddGoodsName", "", szTempName, sizeof( szTempName) );
	m_szPresentAddGoodsName = szTempName;

	// GoodsList
	ioINILoader kGoodsNameLoader;
	kGoodsNameLoader.ReloadFile( szGoodsNameFile );
	
	kGoodsNameLoader.SetTitle( "GoodsList" );

	bool bLoadGoodsNameList = false;
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal && pLocal->IsLoadGoodsNameList() )
		bLoadGoodsNameList = true;

	kLoader.SetTitle( "GoodsList" );
	iMax = kLoader.LoadInt( "Max", 0 );

	if( iMax <= 0 )
	{
		LOG.PrintTimeAndLog(0, "Reload GoodsManager::LoadINI Goods Max is Error : %d", iMax );
		return false;
	}

	m_GoodsInfoMap.clear();
	for (int i = 0; i < iMax ; i++)
	{
		char szKeyName[MAX_PATH]="";
		StringCbPrintf( szKeyName, sizeof( szKeyName ), "GoodsNo_%d", i+1 );
		DWORD dwGoodsNo = kLoader.LoadInt( szKeyName, 0 );
		if( dwGoodsNo == 0 )
		{
			LOG.PrintTimeAndLog(0, "[error][goods]Reload GoodsManager::LoadINI GoodsNo is Zero [%d]", i+1 );
			return false;
		}

		ZeroMemory( szKeyName, sizeof(szKeyName) );
		ioHashString szGoodsName;

		if( bLoadGoodsNameList )
		{
			// 새로운 INI에서 읽는다.
			StringCbPrintf( szKeyName, sizeof( szKeyName ), "GoodsName_%d", i+1 );
			char szTempName[MAX_PATH]="";
			kGoodsNameLoader.LoadString( szKeyName, "", szTempName, sizeof( szTempName) );
			szGoodsName = szTempName;
		}
		else
		{
			StringCbPrintf( szKeyName, sizeof( szKeyName ), "GoodsName_%d", i+1 );
			char szTempName[MAX_PATH]="";
			kLoader.LoadString( szKeyName, "", szTempName, sizeof( szTempName) );
			szGoodsName = szTempName;
		}

		if( szGoodsName.IsEmpty() )
		{
			LOG.PrintTimeAndLog(0, "[error][goods]Reload GoodsManager::LoadINI GoodsName is Empty [%d]", i+1 );
			szGoodsName = "unknown";
			return false;
		}
		
		GoodsInfoMap::const_iterator iter = m_GoodsInfoMap.find( dwGoodsNo );
		if( iter != m_GoodsInfoMap.end() )
		{
			ioHashString szOutGoodsName;
			szOutGoodsName = iter->second;
			LOG.PrintTimeAndLog(0, "Reload GoodsManager::LoadINI Duplication Goods No. [%d][%d:%s]", i+1 , dwGoodsNo, szOutGoodsName.c_str() );
			return false;
		}

		m_GoodsInfoMap.insert( GoodsInfoMap::value_type( dwGoodsNo, szGoodsName ) );
	}

	kLoader.SetTitle( "PackageShortTypeList" );
	iMax = kLoader.LoadInt( "Max", 0 );
	
	m_vPackageEtcItemShortTypeList.clear();
	for (int i = 0; i < iMax ; i++)
	{
		char szKeyName[MAX_PATH]="";
		StringCbPrintf( szKeyName, sizeof( szKeyName ), "ShortType_%d", i+1 );
		DWORD dwShortType = kLoader.LoadInt( szKeyName, 0 );
		if( dwShortType == 0 )
		{
			LOG.PrintTimeAndLog(0, "[error][goods]Reload GoodsManager::LoadINI iShortType is Zero [%d]", i+1 );
			return false;
		}

		m_vPackageEtcItemShortTypeList.push_back( dwShortType );
	}

	kLoader.SetTitle( "PackageExtraItemMachineCodeList" );
	iMax = kLoader.LoadInt( "Max", 0 );
	
	m_vPackageExtraItemMachineCodeList.clear();
	for (int i = 0; i < iMax ; i++)
	{
		char szKeyName[MAX_PATH]="";
		StringCbPrintf( szKeyName, sizeof( szKeyName ), "MachineCode_%d", i+1 );
		DWORD dwShortType = kLoader.LoadInt( szKeyName, 0 );
		if( dwShortType == 0 )
		{
			LOG.PrintTimeAndLog(0, "[error][goods]Reload GoodsManager::LoadINI MachineCode is Zero [%d]", i+1 );
			return false;
		}

		m_vPackageExtraItemMachineCodeList.push_back( dwShortType );
	}

	int iCnt= 0 ;
	LOG.PrintTimeAndLog( 0, "<< Reload Goods List Start >>" );
	for(GoodsInfoMap::iterator iter = m_GoodsInfoMap.begin(); iter != m_GoodsInfoMap.end(); ++iter)
	{
		LOG.PrintTimeAndLog( 0, "Reload [%04d] %u %s", ++iCnt, iter->first, iter->second.c_str() );

		/* 정상구매 되는지 테스트 코드
		IIPGClientPtr ptrClient;
		ptrClient.CreateInstance(__uuidof(IPGClient));

		_variant_t vsUserNo, vsUserID, vsCompanyCD, vsGoodsNo, vsPayAmt, vsAgencyNo, vsUserIP, vsEtc1, vsEtc2;// input
		_variant_t vsBxaid, vsRetMsg;    // output

		vsUserNo    = (LPSTR) "cashtest01"; 
		vsUserID    = (LPSTR) "cashtest01";
		vsCompanyCD = (LPSTR) "LOSA"; 	
		vsGoodsNo   = (LONG)  iter->first;
		vsPayAmt    = (LONG)  1;
		vsUserIP    = (LPSTR) "192.168.1.103";
		vsEtc1      = (LPSTR) "Test";
		vsEtc2      = (LPSTR) "cashtest01";

		// 값을 받을때까지 대기 
		ptrClient->OutputIPG(&vsUserNo, &vsUserID, &vsCompanyCD, &vsGoodsNo,&vsPayAmt ,&vsAgencyNo, &vsUserIP, &vsEtc1
			                ,&vsEtc1, &vsBxaid, &vsRetMsg ); 
		*/
	}
	LOG.PrintTimeAndLog( 0, "<< Reload Goods List End >>" );

	iCnt= 0 ;
	LOG.PrintTimeAndLog( 0, "<< Reload Package Short Type List Start >>" );
	for(DWORDVec::iterator iter = m_vPackageEtcItemShortTypeList.begin(); iter != m_vPackageEtcItemShortTypeList.end(); ++iter)
	{
	    LOG.PrintTimeAndLog( 0, "Reload [%04d] %u", ++iCnt, (*iter) );
	}
	LOG.PrintTimeAndLog( 0, "<< Reload Package Short Type List End >>" );


	iCnt= 0 ;
	LOG.PrintTimeAndLog( 0, "<< Reload Package Extra item MacineCode List Start >>" );
	for(DWORDVec::iterator iter = m_vPackageExtraItemMachineCodeList.begin(); iter != m_vPackageExtraItemMachineCodeList.end(); ++iter)
	{
		LOG.PrintTimeAndLog( 0, "Reload [%04d] %u", ++iCnt, (*iter) );
	}
	LOG.PrintTimeAndLog( 0, "<< Reload Package Extra item MacineCode End >>" );

	return true;
}


int GoodsManager::GetGoodsNoValue( int iLimitSeconds )
{
	for(vSoldierInfo::iterator iter = m_vSoldierInfo.begin(); iter != m_vSoldierInfo.end(); ++iter)
	{
		SoldierInfo &rkInfo = (*iter);
		if( rkInfo.m_iLimitSeconds == iLimitSeconds )
			return rkInfo.m_iGoodsNoValue;
	}

	return -1;
}


bool GoodsManager::GetGoodsName( IN DWORD dwGoodsNo, OUT ioHashString &rszGoodsName ) 
{
	GoodsInfoMap::const_iterator iter = m_GoodsInfoMap.find( dwGoodsNo );
	if( iter != m_GoodsInfoMap.end() )
	{
		rszGoodsName = iter->second;
		return true;
	}

	LOG.PrintTimeAndLog(0, "GoodsManager::GetGoodsName Not Exist : %u", dwGoodsNo );
	return false;
}

bool GoodsManager::GetGoodsInfoSoldier( IN int iClassType, IN int iLimitSeconds, IN int iPeriodType, OUT DWORD &rdwGoodsNo, OUT ioHashString &rszGoodsName )
{
	int iGoodsNoValue = 0;
	if( iPeriodType == CPT_TIME )
		iGoodsNoValue = GetGoodsNoValue( iLimitSeconds );
	else if( iPeriodType == CPT_MORTMAIN )
		iGoodsNoValue = m_iMortmainCharGoodsNoValue;
#ifdef DATE_CHAMP
	else if( iPeriodType == CPT_DATE )
		iGoodsNoValue = GetGoodsNoValue( iLimitSeconds );
#endif
	if( iGoodsNoValue <= 0 )
	{
		LOG.PrintTimeAndLog(0, "GoodsManager::GetGoodsInfoSoldier GoodsNoValue is Error :%d", iGoodsNoValue );
		return false;
	}

	rdwGoodsNo = 100000000;                    // 1억은 용병
	rdwGoodsNo +=   iClassType;                // 1~9999            :클래스자리
	rdwGoodsNo += ( iGoodsNoValue * 10000 );   // 10000 ~ 99990000  :구매시간자리

	if( !GetGoodsName( rdwGoodsNo, rszGoodsName ) )
		return false;

	if( rszGoodsName.IsEmpty() )
	{
		LOG.PrintTimeAndLog(0, "[error][goods]GoodsManager::GetGoodsInfoSoldier GoodsName is Empty" );
		return false;
	}

	return true;
}

bool GoodsManager::GetGoodsInfoDeco( IN int iType, IN int iDecoCode, OUT DWORD &rdwGoodsNo, OUT ioHashString &rszGoodsName )
{
	int iDecoType  = iType % 1000;
	int iKindred   = ( iType % 100000) / 1000;

	rdwGoodsNo = 200000000;                      // 2억은 치장
	rdwGoodsNo += iDecoCode;                     // 1~999 치장코드 : 얼굴이라면 웃는 얼굴, 화난 얼굴, 멍한 얼굴등
	rdwGoodsNo += ( iDecoType * 1000 );          // 1000~999000 치장타입 : 얼굴,머리,머리색,속옷등
	rdwGoodsNo += ( iKindred  * 1000000 );       // 1000000~99000000 종족:  / 0 : 남자 / 1 : 여자 / 2 : 엘프남자 /

	if( !GetGoodsName( rdwGoodsNo, rszGoodsName ) )
		return false;

	if( rszGoodsName.IsEmpty() )
	{
		LOG.PrintTimeAndLog(0, "[error][goods]GoodsManager::GetGoodsInfoSoldier GoodsName is Empty" );
		return false;
	}

	return true;
}

bool GoodsManager::GetGoodsInfoEtc( IN DWORD dwType, IN int iArray, OUT DWORD &rdwGoodsNo, OUT ioHashString &rszGoodsName )
{
	enum { 	USE_TYPE_CUT_VALUE = 1000000, ARRAY_POS = 1000000,};

	int iType = dwType%USE_TYPE_CUT_VALUE;       // 사용타입을 제거한다. 

	rdwGoodsNo = 300000000;                      // 3억은 권한
	rdwGoodsNo += iType;                         // AABBBBBB --> AA: 갯수나 시간 나타내는 array sp2_etcitem_info.ini value값에 대음됨 예) 확성기 10개, 20개 / BBBBBB: 종류 1~99999 : 권한 , 100000 ~ 199999 : 클래스별 권한 아이템
    rdwGoodsNo += ( iArray * ARRAY_POS );
	if( IsPackageItem( iType ) )
		rdwGoodsNo += 100000000;                 // 패키지 아이템은 4억대가 됨 ( 삼성측 요청 )

	if( !GetGoodsName( rdwGoodsNo, rszGoodsName ) )
		return false;

	if( rszGoodsName.IsEmpty() )
	{
		LOG.PrintTimeAndLog(0, "[error][goods]GoodsManager::GetGoodsInfoEtc GoodsName is Empty" );
		return false;
	}

	return true;
}

bool GoodsManager::GetGoodsInfoExtraBox( IN int iMachineCode, OUT DWORD &rdwGoodsNo, OUT ioHashString &rszGoodsName )
{
	rdwGoodsNo = 500000000;                      // 5억은 장비 보급
	rdwGoodsNo += iMachineCode;                 

	if( IsPackageItemExtra( iMachineCode ) )
		rdwGoodsNo -= 90000000;                 // 패키지 아이템은 4억1천만대 됨 
	
	if( !GetGoodsName( rdwGoodsNo, rszGoodsName ) )
		return false;

	if( rszGoodsName.IsEmpty() )
	{
		LOG.PrintTimeAndLog(0, "[error][goods]%s GoodsName is Empty", __FUNCTION__ );
		return false;
	}

	return true;
}


bool GoodsManager::GetGoodsInfoExtra( IN int iItemCode, OUT DWORD &rdwGoodsNo, OUT ioHashString &rszGoodsName )
{
	rdwGoodsNo = 600000000;                      // 6억은 장비
	rdwGoodsNo += iItemCode;                 

	if( !GetGoodsName( rdwGoodsNo, rszGoodsName ) )
		return false;

	if( rszGoodsName.IsEmpty() )
	{
		LOG.PrintTimeAndLog(0, "[error][goods]%s GoodsName is Empty", __FUNCTION__ );
		return false;
	}

	return true;
}

bool GoodsManager::GetGoodsInfo( IN short iPresentType, IN int iBuyValue1, IN int iBuyValue2, OUT DWORD &rdwGoodsNo, OUT ioHashString &rszGoodsName )
{
	DWORD         dwInGoodsNo = 0;
	ioHashString  szInGoodsName;
	if( iPresentType == PRESENT_SOLDIER )
	{
		int iPeriodType = CPT_TIME;
#ifdef DATE_CHAMP
		if( ioLocalManager::GetLocalType() == ioLocalManager::LCT_LATIN )
			iPeriodType = CPT_DATE;
#endif
		if( iBuyValue2 == 0 )
			iPeriodType = CPT_MORTMAIN;
		
		if( !GetGoodsInfoSoldier( iBuyValue1, iBuyValue2, iPeriodType, dwInGoodsNo, szInGoodsName ) )
			return false;
	}
	else if( iPresentType == PRESENT_DECORATION )
	{
		if( !GetGoodsInfoDeco( iBuyValue1, iBuyValue2, dwInGoodsNo, szInGoodsName ) )
			return false;
	}
	else if( iPresentType == PRESENT_ETC_ITEM )
	{
		if( !GetGoodsInfoEtc( iBuyValue1, iBuyValue2, dwInGoodsNo, szInGoodsName ) )
			return false;
	}
	else if( iPresentType == PRESENT_EXTRAITEM )
	{
		if( !GetGoodsInfoExtra( iBuyValue1, dwInGoodsNo, szInGoodsName )	)
			return false;
	}
	else if( iPresentType == PRESENT_EXTRAITEM_BOX )
	{
		if( !GetGoodsInfoExtraBox( iBuyValue1, dwInGoodsNo, szInGoodsName ) )
			return false;
	}
	else
	{
		LOG.PrintTimeAndLog(0, "%s PresentType is wrong. %d", __FUNCTION__, iPresentType );
		return false;
	}


	//rdwGoodsNo = 1000000000; // 10억은 선물
	//rdwGoodsNo += dwInGoodsNo;
	rdwGoodsNo = dwInGoodsNo;
	rszGoodsName = szInGoodsName;


	return true;
}

bool GoodsManager::GetGoodsInfoPresent( IN short iPresentType, IN int iBuyValue1, IN int iBuyValue2, OUT DWORD &rdwGoodsNo, OUT ioHashString &rszGoodsName )
{
	bool rtVal = GetGoodsInfo(iPresentType, iBuyValue1, iBuyValue2, rdwGoodsNo, rszGoodsName );

	rdwGoodsNo += 1000000000;

	char szTempGoodsName[MAX_PATH]="";
 
	StringCbPrintf( szTempGoodsName, sizeof( szTempGoodsName ), "%s %s", m_szPresentAddGoodsName.c_str(), rszGoodsName.c_str() );

	rszGoodsName = szTempGoodsName;

	return rtVal;
}

bool GoodsManager::GetGoodsInfoSubcription( IN short iPresentType, IN int iBuyValue1, IN int iBuyValue2, OUT DWORD &rdwGoodsNo, OUT ioHashString &rszGoodsName )
{
	bool rtVal = GetGoodsInfo(iPresentType, iBuyValue1, iBuyValue2, rdwGoodsNo, rszGoodsName );

	return rtVal;
}
bool GoodsManager::GetGoodsInfoPresentMileage( IN short iPresentType, IN int iBuyValue1, IN int iBuyValue2, IN bool bPresent, OUT DWORD &rdwGoodsNo, OUT ioHashString &rszGoodsName )
{
	// 일본에서 이름 표시 하지 말 것 요청
	//if( !GetGoodsInfoPresent( iPresentType, iBuyValue1, iBuyValue2, rdwGoodsNo, rszGoodsName ) )
	{
		if( bPresent )
			rdwGoodsNo += 1000000000;

		char szName[MAX_PATH]="";
		StringCbPrintf( szName, sizeof( szName ), "PresentType:%d Value1:%d Value2:%d Present:%d", iPresentType, iBuyValue1, iBuyValue2, (int) bPresent );
		rszGoodsName = szName;
		return false;
	}

	if( !bPresent )
		rdwGoodsNo -= 1000000000;

	return true;
}

GoodsManager & GoodsManager::GetSingleton()
{
	return Singleton< GoodsManager >::GetSingleton();
}

bool GoodsManager::IsPackageItem( int iEtcItemShortType )
{
	for(DWORDVec::iterator iter = m_vPackageEtcItemShortTypeList.begin(); iter != m_vPackageEtcItemShortTypeList.end(); ++iter)
	{
	    if( iEtcItemShortType == ( *iter) )
			return true;
	}

	return false;
}

bool GoodsManager::IsPackageItemExtra( int iExtraItemMachineCode )
{
	for(DWORDVec::iterator iter = m_vPackageExtraItemMachineCodeList.begin(); iter != m_vPackageExtraItemMachineCodeList.end(); ++iter)
	{
		if( iExtraItemMachineCode == ( *iter) )
			return true;
	}

	return false;
}

bool GoodsManager::GetGoodsInfoPopup( IN short iPresentType, IN int iBuyValue3, OUT DWORD &rdwGoodsNo, OUT ioHashString &rszGoodsName )
{
	rdwGoodsNo = 700000000;
	rdwGoodsNo += iBuyValue3;

	if( !GetGoodsName( rdwGoodsNo, rszGoodsName ) )
		return false;

	if( rszGoodsName.IsEmpty() )
	{
		LOG.PrintTimeAndLog(0, "[error][goods]%s GoodsName is Empty", __FUNCTION__ );
		return false;
	}

	return true;
}
