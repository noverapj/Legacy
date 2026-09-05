#include "StdAfx.h"
#include "ioMultiSuperGashaponMgr.h"
#include "DataHeaders/LSC_New_Gashapon_info.h"
#include "GUI/MultiSuperGashaponWnd.h"


template<> ioMultiSuperGashaponMgr* Singleton< ioMultiSuperGashaponMgr >::ms_Singleton = 0;
ioMultiSuperGashaponMgr& ioMultiSuperGashaponMgr::GetSingleton()
{
	return Singleton< ioMultiSuperGashaponMgr >::GetSingleton();
}

ioMultiSuperGashaponMgr::ioMultiSuperGashaponMgr(void)
{
}


ioMultiSuperGashaponMgr::~ioMultiSuperGashaponMgr(void)
{
}

void ioMultiSuperGashaponMgr::Initialize()
{
	LSC_New_Gashapon_info_Manager* pNewGashaponInfoManager = new LSC_New_Gashapon_info_Manager;
	pNewGashaponInfoManager->LoadData(NEW_GASHAPON_INFO_TABLE);

	int iTotal = pNewGashaponInfoManager->GetTotal();
	for( int i = 0; i < iTotal; i++ )
	{
		LSC_New_Gashapon_info* pInfo = pNewGashaponInfoManager->GetAt( i );
		if( !pInfo )
			continue;

		NewSuperGashaponElement element;
		//element.PackageIndex = pInfo->PackageIndex;
		element.ElementIndex = pInfo->ElementIndex;
		element.CategoryType = pInfo->CategoryType;
		element.Category_Package_Element_Type = pInfo->Category_Package_Element_Type;
		element.Category_Package_Element_Value1 = pInfo->Category_Package_Element_Value1;
		element.Category_Package_Element_Value2 = pInfo->Category_Package_Element_Value2;
		element.Period = pInfo->Period;
		element.Package_Alarm = pInfo->Package_Alarm;

		m_MultiSuperGashaponMap[pInfo->EtcItemCode].m_vNewSuperGashaponMap[pInfo->CategoryIndex][pInfo->PackageIndex].push_back( element );
	}

	SAFEDELETE( pNewGashaponInfoManager );
}

/*void ioMultiSuperGashaponMgr::LoadPackage( ioINILoader& rkLoader, SuperGashaponPackageInfo& rkInfo )
{
	rkInfo.m_vSuperGashaponPackage.clear();
	int iMaxPackage = rkLoader.LoadInt_e( "MaxPackage", 0 );
	for( int i = 1; i <= iMaxPackage; ++i )
	{		
		SuperGashaponPackage Package;
		LoadElement( rkLoader, Package, i );
		rkInfo.m_vSuperGashaponPackage.push_back( Package );
	}

	iMaxPackage = rkLoader.LoadInt_e( "SubMaxPackage", 0 );
	for( int i = 1; i <= iMaxPackage; ++i )
	{		
		SuperGashaponPackage Package;
		LoadElement( rkLoader, Package, i, true );
		rkInfo.m_vSuperGashaponSubPackage.push_back( Package );
	}
}

void ioMultiSuperGashaponMgr::LoadRollingIcon( SuperGashaponPackageInfo& rkInfo )
{
	rkInfo.m_vRollIconName.clear();	
	rkInfo.m_vSubIconName.clear();

	LoadRollingIconVec( rkInfo.m_vSuperGashaponPackage, rkInfo.m_vRollIconName, rkInfo.m_vSubIconName );
	LoadRollingIconVec( rkInfo.m_vSuperGashaponSubPackage, rkInfo.m_vRollIconName, rkInfo.m_vSubIconName );
}

void ioMultiSuperGashaponMgr::LoadRollingIconVec( const vSuperGashaponPackage& vPackage, ioHashStringVec& vRollIconVec, ioHashStringVec& vSubIconVec )
{
	vSuperGashaponPackage::const_iterator present_iter = vPackage.begin();
	for( ; present_iter != vPackage.end(); ++present_iter )
	{
		const SuperGashaponPackage& rkPackage = *present_iter;
		vSuperGashaponElement::const_iterator element_iter = rkPackage.m_vSuperGashaponElement.begin();
		for( ; element_iter != rkPackage.m_vSuperGashaponElement.end(); ++element_iter )
		{
			const SuperGashaponElement& rkElement = *element_iter;
			ioHashString szIconName = g_PresentMgr.GetPresentIconText( rkElement.m_iPresentType, rkElement.m_iPresentValue1, rkElement.m_iPresentValue2 );
			ioHashString szSubIconName = g_PresentMgr.GetPresentSubIconText( rkElement.m_iPresentType, rkElement.m_iPresentValue1, rkElement.m_iPresentValue2 );
			if( szIconName.IsEmpty() )
			{
				LOG.PrintTimeAndLog( 0, "%s icon empty : %d", __FUNCTION__, rkElement.m_iPresentValue1 );
				continue;
			}
			else if( HasIcon( vRollIconVec, szIconName ) )
			{
				continue;
			}
			vRollIconVec.push_back( szIconName );
			vSubIconVec.push_back( szSubIconName );
		}
	}
}

void ioMultiSuperGashaponMgr::LoadCostume( SuperGashaponPackageInfo& rkInfo )
{
	rkInfo.m_vCostume.clear();	

	LoadCostumeVec( rkInfo.m_vSuperGashaponPackage, rkInfo.m_vCostume );
	LoadCostumeVec( rkInfo.m_vSuperGashaponSubPackage, rkInfo.m_vCostume );
}

void ioMultiSuperGashaponMgr::LoadCostumeVec( const vSuperGashaponPackage& vPackage, BoolVec& vCostumeVec )
{
	vSuperGashaponPackage::const_iterator present_iter = vPackage.begin();
	for( ; present_iter != vPackage.end(); ++present_iter )
	{
		const SuperGashaponPackage& rkPackage = *present_iter;
		vSuperGashaponElement::const_iterator element_iter = rkPackage.m_vSuperGashaponElement.begin();
		for( ; element_iter != rkPackage.m_vSuperGashaponElement.end(); ++element_iter )
		{
			const SuperGashaponElement& rkElement = *element_iter;
			if ( rkElement.m_iPresentType == PRESENT_COSTUME || rkElement.m_iPresentType == PRESENT_COSTUME_BOX )
				vCostumeVec.push_back( true );
			else
				vCostumeVec.push_back( false );
		}
	}
}

void ioMultiSuperGashaponMgr::LoadGrade( SuperGashaponPackageInfo& rkInfo )
{
	rkInfo.m_vGrade.clear();	

	LoadGradeVec( rkInfo.m_vSuperGashaponPackage, rkInfo.m_vGrade );
	LoadGradeVec( rkInfo.m_vSuperGashaponSubPackage, rkInfo.m_vGrade );
}

void ioMultiSuperGashaponMgr::LoadGradeVec( const vSuperGashaponPackage& vPackage, IntVec& vGradeVec )
{
	vSuperGashaponPackage::const_iterator present_iter = vPackage.begin();
	for( ; present_iter != vPackage.end(); ++present_iter )
	{
		const SuperGashaponPackage& rkPackage = *present_iter;
		vSuperGashaponElement::const_iterator element_iter = rkPackage.m_vSuperGashaponElement.begin();
		for( ; element_iter != rkPackage.m_vSuperGashaponElement.end(); ++element_iter )
		{
			const SuperGashaponElement& rkElement = *element_iter;
			int nGradeType = g_PresentMgr.GetGradeType( rkElement.m_iPresentType, rkElement.m_iPresentValue1 );

			vGradeVec.push_back( nGradeType );
		}
	}
}

void ioMultiSuperGashaponMgr::LoadRollingTime( ioINILoader& rkLoader, SuperGashaponPackageInfo& rkInfo )
{
	rkInfo.m_dwRollingTotalTime = (DWORD)rkLoader.LoadInt( "rolling_total_time", 0 );
}*/

bool ioMultiSuperGashaponMgr::HasIcon( const ioHashStringVec& szIconVec, const ioHashString& szIconName )
{
	if( szIconVec.empty() )
		return false;

	ioHashStringVec::const_iterator iter = szIconVec.begin();
	for( ; iter != szIconVec.end(); ++iter )
	{
		const ioHashString& rkIconName = *iter;
		if( strcmp( rkIconName.c_str(), szIconName.c_str() ) == 0 )
			return true;
	}

	return false;
}

/*void ioMultiSuperGashaponMgr::LoadElement( ioINILoader& rkLoader, SuperGashaponPackage& rkPackage, int iPackage, bool bSubPackage )
{
	ZeroMemory( m_szBuffer, sizeof( m_szBuffer ) );
	if( bSubPackage )
		sprintf_s( m_szBuffer, "Sub" );

	sprintf_s_e( m_szKey, "%sPackage%d_Element_Cnt", m_szBuffer, iPackage );
	int iMaxElement = rkLoader.LoadInt( m_szKey, 0 );
	rkPackage.m_vSuperGashaponElement.clear();

	if( 0 < iMaxElement )
		rkPackage.m_vSuperGashaponElement.reserve( iMaxElement );

	sprintf_s_e( m_szKey, "%sPackage%d_Alaram", m_szBuffer, iPackage );
	rkPackage.m_bWholeAlarm = rkLoader.LoadBool( m_szKey, 0 );

	for( int i = 1; i <= iMaxElement; ++i )
	{
		sprintf_s_e( m_szKey, "%sPackage%d_Element%d_Alarm", m_szBuffer, iPackage, i );
		bool bAlarm = rkLoader.LoadBool( m_szKey, false );

		sprintf_s_e( m_szKey, "%sPackage%d_Element%d_Type", m_szBuffer, iPackage, i );
		int iType = rkLoader.LoadInt( m_szKey, 0 );

		sprintf_s_e( m_szKey, "%sPackage%d_Element%d_Value1", m_szBuffer, iPackage, i );
		int iValue1 = rkLoader.LoadInt( m_szKey, 0 );

		sprintf_s_e( m_szKey, "%sPackage%d_Element%d_Value2", m_szBuffer, iPackage, i );		
		int iValue2 = rkLoader.LoadInt( m_szKey, 0 );

		SuperGashaponElement Element;
		Element.m_iPresentType	 = iType;
		Element.m_iPresentValue1 = iValue1;
		Element.m_iPresentValue2 = iValue2;

		rkPackage.m_vSuperGashaponElement.push_back( Element );
	}
}*/

bool ioMultiSuperGashaponMgr::IsMultiSuperGashpon( DWORD dwEtcItemType )
{
	NewSuperGashaponPackageInfoMap::const_iterator info_iter = m_MultiSuperGashaponMap.find( dwEtcItemType );
	if( info_iter == m_MultiSuperGashaponMap.end() )
		return false;

	return true;
}

const NewSuperGashaponElementList& ioMultiSuperGashaponMgr::GetSuperGashaponPackage( DWORD dwEtcItemType, DWORD dwCategory, DWORD dwPackage )
{
	static NewSuperGashaponElementList kNone;
	
	NewSuperGashaponPackageInfoMap::iterator iter = m_MultiSuperGashaponMap.find( dwEtcItemType );
	if( iter != m_MultiSuperGashaponMap.end() )
	{
		NewSuperGashaponPackageInfo& packageInfo = iter->second;

		if( packageInfo.m_vNewSuperGashaponMap.find( dwCategory) != packageInfo.m_vNewSuperGashaponMap.end() && packageInfo.m_vNewSuperGashaponMap[dwCategory].find( dwPackage ) != packageInfo.m_vNewSuperGashaponMap[dwCategory].end() )
		{
			return packageInfo.m_vNewSuperGashaponMap[dwCategory][dwPackage];
		}
	}

	return kNone;
}

/*void ioMultiSuperGashaponMgr::GetSuperGashaponPackageElement( DWORD dwEtcItemType, int iPackage, ioSuperGashaponMgr::vSuperGashaponElement& vElement )
{
	if( iPackage < 0 )
		return;

	SuperGashaponPackageInfoMap::iterator info_iter = m_SuperGashaponPackageInfoMap.find( dwEtcItemType );
	if( info_iter != m_SuperGashaponPackageInfoMap.end() )
	{
		const SuperGashaponPackageInfo rkInfo = info_iter->second;

		if( iPackage < (int)rkInfo.m_vSuperGashaponPackage.size() )
			vElement = rkInfo.m_vSuperGashaponPackage[iPackage].m_vSuperGashaponElement;
	}
}*/

void ioMultiSuperGashaponMgr::GetRollIconVec( DWORD dwEtcItemType, ioHashStringVec& vRollIconVec )
{ 
	NewSuperGashaponPackageInfoMap::iterator iter = m_MultiSuperGashaponMap.find( dwEtcItemType );
	if( iter != m_MultiSuperGashaponMap.end() )
	{
		NewSuperGashaponPackageInfo packageInfo = iter->second;
		NewSuperGashaponMap::iterator iter1 = packageInfo.m_vNewSuperGashaponMap.begin();
		for( ; iter1 != packageInfo.m_vNewSuperGashaponMap.end(); iter1++ )
		{
			NewSuperGashaponPackageList::iterator iter2 = packageInfo.m_vNewSuperGashaponMap[iter1->first].begin();
			for( ; iter2 != packageInfo.m_vNewSuperGashaponMap[iter1->first].end(); iter2++ )
			{
				NewSuperGashaponElementList::iterator iter3 = packageInfo.m_vNewSuperGashaponMap[iter1->first][iter2->first].begin();
				for( ; iter3 != packageInfo.m_vNewSuperGashaponMap[iter1->first][iter2->first].end(); iter3++ )
				{
					NewSuperGashaponElement element = *iter3;
					ioHashString szIconName = g_PresentMgr.GetPresentIconText( element.Category_Package_Element_Type, element.Category_Package_Element_Value1, element.Category_Package_Element_Value2 );
					vRollIconVec.push_back( szIconName );
				}
			}
		}
	}
}

/*const ioHashStringVec& ioMultiSuperGashaponMgr::GetSubIconVec( DWORD dwEtcItemType )
{ 
	static ioHashStringVec szNoneVec;
	SuperGashaponPackageInfoMap::const_iterator info_iter = m_SuperGashaponPackageInfoMap.find( dwEtcItemType );
	if( info_iter != m_SuperGashaponPackageInfoMap.end() )
	{
		const SuperGashaponPackageInfo& rkInfo = info_iter->second;
		return rkInfo.m_vSubIconName;
	}

	return szNoneVec;
}*/

void ioMultiSuperGashaponMgr::GetCostumeVec( DWORD dwEtcItemType, BoolVec& vCostumeVec )
{ 
	NewSuperGashaponPackageInfoMap::iterator iter = m_MultiSuperGashaponMap.find( dwEtcItemType );
	if( iter != m_MultiSuperGashaponMap.end() )
	{
		NewSuperGashaponPackageInfo packageInfo = iter->second;
		NewSuperGashaponMap::iterator iter1 = packageInfo.m_vNewSuperGashaponMap.begin();
		for( ; iter1 != packageInfo.m_vNewSuperGashaponMap.end(); iter1++ )
		{
			NewSuperGashaponPackageList::iterator iter2 = packageInfo.m_vNewSuperGashaponMap[iter1->first].begin();
			for( ; iter2 != packageInfo.m_vNewSuperGashaponMap[iter1->first].end(); iter2++ )
			{
				NewSuperGashaponElementList::iterator iter3 = packageInfo.m_vNewSuperGashaponMap[iter1->first][iter2->first].begin();
				for( ; iter3 != packageInfo.m_vNewSuperGashaponMap[iter1->first][iter2->first].end(); iter3++ )
				{
					NewSuperGashaponElement element = *iter3;
					if ( element.Category_Package_Element_Type == PRESENT_COSTUME || element.Category_Package_Element_Type == PRESENT_COSTUME_BOX )
						vCostumeVec.push_back( true );
					else
						vCostumeVec.push_back( false );
				}
			}
		}
	}
}

void ioMultiSuperGashaponMgr::GetGradeVec( DWORD dwEtcItemType, IntVec& vGradeVec )
{ 
	NewSuperGashaponPackageInfoMap::iterator iter = m_MultiSuperGashaponMap.find( dwEtcItemType );
	if( iter != m_MultiSuperGashaponMap.end() )
	{
		NewSuperGashaponPackageInfo packageInfo = iter->second;
		NewSuperGashaponMap::iterator iter1 = packageInfo.m_vNewSuperGashaponMap.begin();
		for( ; iter1 != packageInfo.m_vNewSuperGashaponMap.end(); iter1++ )
		{
			NewSuperGashaponPackageList::iterator iter2 = packageInfo.m_vNewSuperGashaponMap[iter1->first].begin();
			for( ; iter2 != packageInfo.m_vNewSuperGashaponMap[iter1->first].end(); iter2++ )
			{
				NewSuperGashaponElementList::iterator iter3 = packageInfo.m_vNewSuperGashaponMap[iter1->first][iter2->first].begin();
				for( ; iter3 != packageInfo.m_vNewSuperGashaponMap[iter1->first][iter2->first].end(); iter3++ )
				{
					NewSuperGashaponElement element = *iter3;
					int nGradeType = g_PresentMgr.GetGradeType( element.Category_Package_Element_Type, element.Category_Package_Element_Value1 );
					vGradeVec.push_back( nGradeType );
				}
			}
		}
	}
}

DWORD ioMultiSuperGashaponMgr::GetRollingTime( DWORD dwEtcItemType )
{ 
	/*static IntVec szNoneVec;
	SuperGashaponPackageInfoMap::const_iterator info_iter = m_SuperGashaponPackageInfoMap.find( dwEtcItemType );
	if( info_iter != m_SuperGashaponPackageInfoMap.end() )
	{
		const SuperGashaponPackageInfo& rkInfo = info_iter->second;
		return rkInfo.m_dwRollingTotalTime;
	}
	// 기본 값
	return 1500;*/
	return 0;
}

bool ioMultiSuperGashaponMgr::PrintSuperGashaponChatAlaram( const ioHashString& szSendID, DWORD dwEtcItemType, DWORD dwCategory, DWORD dwPackage )
{
	const NewSuperGashaponElementList& rkPackage = GetSuperGashaponPackage( dwEtcItemType, dwCategory, dwPackage );

	if( rkPackage.size() == 0 )
	{
		LOG.PrintTimeAndLog( 0, "%s Element Empty : %d, %d, %d", __FUNCTION__, dwEtcItemType, dwCategory, dwPackage );
		return false;
	}

	ioHashString szEtcItemName;
	ioEtcItem* pItem = g_EtcItemMgr.FindEtcItem( dwEtcItemType );	
	if( pItem )
	{
		szEtcItemName = pItem->GetName();
	}
	else
	{
		LOG.PrintTimeAndLog( 0, "%s not find etcitemtype : %d", __FUNCTION__, dwEtcItemType );
		return false;
	}

	//추후 이부분에서 Etc타입별로 메시지 분류
	PrintDefaultAlaram( szSendID, rkPackage, szEtcItemName );

	return true;
}

void ioMultiSuperGashaponMgr::PrintDefaultAlaram( const ioHashString& szSendID, const NewSuperGashaponElementList& rkPackage, const ioHashString& szEtcItemName )
{
	ioComplexStringPrinter kPrinter;
	g_ChatMgr.PresetChatComplexString( MSG_ANNOUNCE_CHAT, kPrinter );

	kPrinter.SetTextColor( TCT_DEFAULT_VIOLET );
	kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), szSendID.c_str() );

	kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
	kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(1) );

	kPrinter.SetTextColor( TCT_DEFAULT_VIOLET );
	kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(2), szEtcItemName.c_str() );

	kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
	kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(3) );

	NewSuperGashaponElementList::const_iterator iter = rkPackage.begin();
	if( iter != rkPackage.end() )
	{
		const NewSuperGashaponElement& element = *iter;
		sprintf_s_e( m_szBuffer, "[%s]", g_PresentMgr.GetPresentValue1Text( element.Category_Package_Element_Type, element.Category_Package_Element_Value1, element.Category_Package_Element_Value2 ).c_str() );		

		if( rkPackage.size() <= 1 )
		{
			kPrinter.SetTextColor( TCT_DEFAULT_VIOLET );
			kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(4), m_szBuffer );
		}
		else
		{
			kPrinter.SetTextColor( TCT_DEFAULT_VIOLET );
			kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(5), m_szBuffer, max( 0, rkPackage.size() - 1 ) );
		}

		if( element.CategoryType > MultiSuperGashaponWnd::CATEGORY_TYPE_COMMON )
		{
			kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
			kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(6) );
		}
		else
		{
			kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
			kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(7) );
		}
	}	
	else
	{
		LOG.PrintTimeAndLog( 0, "%s Element Empty", __FUNCTION__ );
	}	
	g_ChatMgr.SetChatComplexString( STR(8), kPrinter );
}

bool ioMultiSuperGashaponMgr::SendAutoUse( DWORD dwEtcItemType, byte eType )
{
	ioEtcItemMultiSuperGashapon* pEtcItem = dynamic_cast<ioEtcItemMultiSuperGashapon*>( g_EtcItemMgr.FindEtcItem( dwEtcItemType ) );
	if( !pEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pEtcItem == NULL : %d", __FUNCTION__, dwEtcItemType );
		return false;
	}

	ioUserEtcItem *pUserEtcItem =  g_MyInfo.GetUserEtcItem();
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL : %d", __FUNCTION__, dwEtcItemType );
		return false;
	}

	if( !pEtcItem->IsCanPresentUse() )
		return true;

	if( eType == EBRT_BUY )
	{
		pEtcItem->SetUseType( MULTISUPERGASHPON_BUY );

		//선물함이 꽉찬 경우
		if( !pEtcItem->OnUseBtnPush( dwEtcItemType, pUserEtcItem ) )
			return false;

		//상점에서 구매시 자동으로 사용
		NewShopWnd *pNewShopWnd = dynamic_cast<NewShopWnd*> ( g_GUIMgr.FindWnd( NEW_SHOP_WND ) );
		if( pNewShopWnd && pNewShopWnd->IsShow() )
		{
			//구입결과창 Show
			MultiSuperGashaponWnd *pMultiSuperGashaponWnd = dynamic_cast<MultiSuperGashaponWnd*> ( pNewShopWnd->FindChildWnd( NewShopWnd::ID_MULTI_SUPERGASHAPON_WND ) );
			if( pMultiSuperGashaponWnd )
				pMultiSuperGashaponWnd->ShowMultiSuperGashaponWnd( PRESENT_ETC_ITEM, dwEtcItemType, 0, MULTISUPERGASHPON_BUY );

			//구입정보창 Hide
			ioWnd *pBuyWnd = pNewShopWnd->FindChildWnd( NewShopWnd::ID_BUY_WND );
			if( pBuyWnd )
				pBuyWnd->HideWnd();
		}
	}

	return true;
}