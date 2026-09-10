#include "stdafx.h"

#include "ioSoldierAwakeManager.h"
#include "ioModeSelectInfo.h"
#include "ioTime.h"
#include "ioBasicTooltipManager.h"

#include "GUI/MyInventoryInfoWnd.h"
#include "GUI/ioPullDownList.h"
#include "GUI/MannerValuationWnd.h"
#include "GUI/UserInfoToolTip.h"

template<> ioSoldierAwakeManager* Singleton< ioSoldierAwakeManager >::ms_Singleton = 0;
ioSoldierAwakeManager& ioSoldierAwakeManager::GetSingleton()
{
	return Singleton< ioSoldierAwakeManager >::GetSingleton();
}

ioSoldierAwakeManager::ioSoldierAwakeManager()
{
	m_nMaxAwakeLimit = 0;
}

ioSoldierAwakeManager::~ioSoldierAwakeManager()
{
}

void ioSoldierAwakeManager::Initialize()
{
	ioINILoader_e kLoader( "config/sp2_char_awake_info.ini" );

	kLoader.SetTitle_e( "common" );
	int nMaxAwake	 = kLoader.LoadInt_e( "max_awake", 0 );
	m_nMaxAwakeLimit = kLoader.LoadInt_e( "max_awake_period_limit", 0 );

	char szBuff[MAX_PATH];
	for( int i = 1; i <= nMaxAwake; ++i )
	{
		AwakeInfo Info;
		sprintf_s_e( szBuff, "awake%d", i );
		kLoader.SetTitle( szBuff );
		LoadGrowth( kLoader, Info );
		LoadUI( kLoader, Info );
		LoadEffect( kLoader, Info );

		Info.eAwakeType = static_cast<AwakeType>( i );
		m_AwakeInfoMap.insert( AwakeInfoMap::value_type( i, Info ) );
	}

	LoadMtrl( kLoader );
	LoadAwakeProduct( kLoader );
}

void ioSoldierAwakeManager::LoadGrowth( ioINILoader& rkLoader, AwakeInfo& rkInfo )
{
	char szBuff[MAX_PATH];

	for( int i = 0; i < MAX_CHAR_GROWTH; ++i )
	{
		sprintf_s_e( szBuff, "char_growth_%d", i+1 );
		rkInfo.nCharGrowth[i] = rkLoader.LoadInt( szBuff, 0 );
	}

	for( int i = 0; i < MAX_ITEM_GROWTH; ++i )
	{
		sprintf_s_e( szBuff, "item_growth_%d", i+1 );
		rkInfo.nItemGrowth[i] = rkLoader.LoadInt( szBuff, 0 );
	}
}

void ioSoldierAwakeManager::LoadUI( ioINILoader& rkLoader, AwakeInfo& rkInfo )
{
	//UI 데이터임으로 보안코드 사용 X
	char szBuff[MAX_PATH];
	rkLoader.LoadString( "name", "", szBuff, MAX_PATH );
	rkInfo.szName = szBuff;

	rkInfo.dwNameColor = rkLoader.LoadInt( "name_color", 0 );

	rkLoader.LoadString( "tooltip", "", szBuff, MAX_PATH );
	rkInfo.szToopTipFile = szBuff;
	g_BasicTooltipMgr.NewReserveToolTip( szBuff );
	
}

void ioSoldierAwakeManager::LoadEffect( ioINILoader& rkLoader, AwakeInfo& rkInfo )
{
	//U임으로 보안코드 사용 X
	char szBuff[MAX_PATH];
	rkLoader.LoadString( "effect", "", szBuff, MAX_PATH );
	rkInfo.szEffect = szBuff;
}

void ioSoldierAwakeManager::LoadMtrl( ioINILoader& rkLoader )
{
	rkLoader.SetTitle_e( "common" );
	int nMaxMtrl = rkLoader.LoadInt_e( "max_material", 0 );
	
	char szBuff[MAX_PATH];
	for( int i = 1; i <= nMaxMtrl; ++i )
	{
		sprintf_s_e( szBuff, "material%d", i );
		rkLoader.SetTitle( szBuff );

		NeedMaterial Mtrl;
		Mtrl.eType		= static_cast<NeedMaterialType>( rkLoader.LoadInt_e( "material_type", static_cast<int>( NMT_NONE ) ) );		
		Mtrl.dwNeedCode	= rkLoader.LoadInt_e( "material_code", 0 );
		Mtrl.nNeedCount	= rkLoader.LoadInt_e( "material_need_cnt", 0 );

		if( Mtrl.eType == NMT_PIECE )
		{
#ifdef _DEBUG
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "ioSoldierAwakeManager material_type 으로 기존 조각 설정 버그" );
#endif
			continue;
		}
		m_NeedMaterialMap.insert( NeedMaterialMap::value_type( i, Mtrl ) );
	}
}

void ioSoldierAwakeManager::LoadAwakeProduct( ioINILoader& rkLoader )
{
	char szBuff[MAX_PATH];
	rkLoader.SetTitle_e( "common" );
	int nMaxMtrl = rkLoader.LoadInt_e( "max_awake_product", 0 );

	for( int i = 1; i <= nMaxMtrl; ++i )
	{
		sprintf_s_e( szBuff, "awake_product%d", i );
		rkLoader.SetTitle( szBuff );

		AwakeProduct sProduct;
		sProduct.nNeedDay				= rkLoader.LoadInt_e( "need_day", 0 );
		sProduct.nNeedMtrlIdx			= rkLoader.LoadInt_e( "need_mtrl", 0 );
		sProduct.nNeedAwakeIdx			= rkLoader.LoadInt_e( "need_awake", 0 );
		
		//UI데이터
		sProduct.sMainTitle.dwTitleColor	= rkLoader.LoadInt( "menu_color", 0 );
		sProduct.sMainTitle.iTitleSize		= rkLoader.LoadInt( "menu_size", 0 );				
		sProduct.sSubTitle.dwTitleColor		= rkLoader.LoadInt( "submenu_color", 0 );
		sProduct.sSubTitle.iTitleSize		= rkLoader.LoadInt( "submenu_size", 0 );

		GetAwakeTtitle( sProduct.sMainTitle.szTitle, sProduct.nNeedAwakeIdx, sProduct.nNeedDay );

		m_AwakeProductMap.insert( AwakeProductMap::value_type( i, sProduct ) );
	}
}

const NeedMaterial* ioSoldierAwakeManager::GetMtrl( int nIdx ) const
{
	NeedMaterialMap::const_iterator iter = m_NeedMaterialMap.find( nIdx );
	if( iter != m_NeedMaterialMap.end() )
	{
		return &iter->second;
	}

	return NULL;
}

const AwakeInfo* ioSoldierAwakeManager::GetAwake( int nIdx ) const
{
	AwakeInfoMap::const_iterator iter = m_AwakeInfoMap.find( nIdx );
	if( iter != m_AwakeInfoMap.end() )
	{
		return &iter->second;
	}

	return NULL;
}

const AwakeInfo* ioSoldierAwakeManager::GetAwakeByType( AwakeType eType ) const
{
	AwakeInfoMap::const_iterator iter = m_AwakeInfoMap.begin();
	for( ; iter != m_AwakeInfoMap.end(); ++iter )
	{
		if( iter->second.eAwakeType == eType )
		{
			return &iter->second;
		}
	}

	return NULL;
}

const AwakeProduct* ioSoldierAwakeManager::GetAwakeProduct( int nIdx ) const
{
	AwakeProductMap::const_iterator iter = m_AwakeProductMap.find( nIdx );
	if( iter != m_AwakeProductMap.end() )
	{
		return &iter->second;
	}

	return NULL;
}

const std::string& ioSoldierAwakeManager::GetAwakeEffect( AwakeType eType ) const
{
	static std::string szNull;
	AwakeInfoMap::const_iterator iter = m_AwakeInfoMap.begin();
	for( ; iter != m_AwakeInfoMap.end(); ++iter )
	{
		if( iter->second.eAwakeType == eType )
		{
			return iter->second.szEffect;
		}
	}

	return szNull;
}

void ioSoldierAwakeManager::GetMtrlTitle( std::string& szTtitle, int nIdx ) const
{
	char szBuff[MAX_PATH];
	const NeedMaterial* pMtrl = GetMtrl( nIdx );
	if( pMtrl )
	{
		if( pMtrl->eType == NMT_ADDITIVE )
		{			
			ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( pMtrl->dwNeedCode );
			if( pEtcItem )
				sprintf_s( szBuff, "%s %d개", pEtcItem->GetName().c_str(), pMtrl->nNeedCount );
		}
		else
		{
			std::string szName = g_PresentMgr.GetPresentValue1Text( PRESENT_PESO, 0, 0 ).c_str();
			sprintf_s( szBuff, "%d %s", pMtrl->nNeedCount, szName.c_str() );
		}
		szTtitle = szBuff;
	}
	else
	{
		szTtitle.clear();
	}
}

void ioSoldierAwakeManager::GetAwakeTtitle( std::string& szTtitle, int nIdx, int nPeriod ) const
{
	char szBuff[MAX_PATH];
	const AwakeInfo* pAwake = GetAwake( nIdx );
	if( pAwake )
	{
		sprintf_s( szBuff, "%s %d일", pAwake->szName.c_str(), nPeriod );
		szTtitle = szBuff;
	}
	else
	{
		szTtitle.clear();
	}
}

int ioSoldierAwakeManager::GetAwakeCharGrowth( AwakeType eType, int iArray )
{	
	if( eType == AWAKE_NONE )
		return 0;

	const AwakeInfo* pAwake = GetAwakeByType( static_cast<AwakeType>( eType ) );
	if( !pAwake )
		return 0;

	if( COMPARE( iArray, 0, MAX_CHAR_GROWTH ) )
		return pAwake->nCharGrowth[iArray];

	return 0;
}

int ioSoldierAwakeManager::GetAwakeItemGrowth( AwakeType eType, int iArray )
{	
	if( eType == AWAKE_NONE )
		return 0;

	const AwakeInfo* pAwake = GetAwakeByType( static_cast<AwakeType>( eType ) );
	if( !pAwake )
		return 0;

	if( COMPARE( iArray, 0, MAX_ITEM_GROWTH ) )
		return pAwake->nItemGrowth[iArray];

	return 0;
}

const std::string& ioSoldierAwakeManager::GetAwakeName( AwakeType eType ) const
{
	AwakeInfoMap::const_iterator iter = m_AwakeInfoMap.begin();
	for( ; iter != m_AwakeInfoMap.end(); ++iter )
	{
		if( iter->second.eAwakeType == eType )
		{
			return iter->second.szName;
		}
	}

	static std::string szNull;
	return szNull;
}

void ioSoldierAwakeManager::GetRemainTime( int iLimitTime, std::string& szText )
{
	char szBuff[MAX_PATH];

	//서버 기준 현재 시간
	SYSTEMTIME rkServerTime;
	g_MyInfo.GetServerTime( rkServerTime );
	CTime NowTime( rkServerTime.wYear, rkServerTime.wMonth, rkServerTime.wDay, rkServerTime.wHour, rkServerTime.wMinute, 0 );

	//각성 종료 시간
	CTime LimitTime( ioTime::GetYear( iLimitTime ), ioTime::GetMonth( iLimitTime ), ioTime::GetDay( iLimitTime ), ioTime::GetHour( iLimitTime ), ioTime::GetMinute( iLimitTime ), 0 );

	//각성 남은 시간
	CTimeSpan GapTime = LimitTime - NowTime;

	//UI 텍스트
	int iRestHour = GapTime.GetTotalHours();		
	if( iRestHour < 1 )
	{
		int iRestMin = GapTime.GetTotalMinutes();
		if( iRestMin <= 1 )
			sprintf_s( szBuff, "(1분 미만)", iRestMin );
		else
			sprintf_s( szBuff, "(%d분 남음)",iRestMin );
	}
	else if( iRestHour < 24 )
	{
		sprintf_s( szBuff, "(%d시간 남음)", iRestHour );
	}
	else
	{
		sprintf_s( szBuff, "(%d일 %d시간 남음)", iRestHour / 24, iRestHour % 24 );
	}

	szText = szBuff;
}

void ioSoldierAwakeManager::GetMenuListName( std::string& szName, int iAwakeType, int iLimitTime )
{
	if( iAwakeType == AWAKE_NONE )
	{
		if( m_AwakeInfoMap.empty() )
		{
			szName ="각성정보 없음";
		}
		else
		{
			AwakeInfoMap::iterator iter = m_AwakeInfoMap.begin();
			szName = iter->second.szName;
		}
	}
	else
	{
		const AwakeInfo* pAwake = GetAwakeByType( static_cast<AwakeType>( iAwakeType ) );
		if( pAwake )
		{
			szName = pAwake->szName;
		}
		
		std::string szRemainTime;
		GetRemainTime( iLimitTime, szRemainTime );

		char szBuff[MAX_PATH];
		sprintf_s( szBuff, "%s %s", szName.c_str(), szRemainTime.c_str() );
		szName = szBuff;
	}
}

void ioSoldierAwakeManager::GetMenuList( IN ioPullDownList* pList, int iClassType )
{
	if( pList == NULL )
		return;
	
	AwakeProductMap::iterator iter = m_AwakeProductMap.begin();
	for( int i = 1; iter != m_AwakeProductMap.end(); ++iter )
	{
		AwakeProduct& rkProduct = iter->second;	

		CharAwakePullDownItem* pPullDown = new CharAwakePullDownItem;		

		const AwakeInfo* pInfo		= GetAwake( rkProduct.nNeedAwakeIdx );
		const NeedMaterial* pMtrl	= GetMtrl( rkProduct.nNeedMtrlIdx );
		if( pMtrl && pInfo )
		{
			GetMtrlTitle( rkProduct.sSubTitle.szTitle, rkProduct.nNeedMtrlIdx );

			//메뉴 이름 및 상품정보 할당
			pPullDown->m_iProductIdx		= iter->first;
			pPullDown->m_szTitle			= rkProduct.sMainTitle.szTitle;
			pPullDown->m_szSubTitle			= rkProduct.sSubTitle.szTitle;
			pPullDown->m_dwTitleSubColor	= TCT_DEFAULT_BLUE;

			//각성 불가에 따른 색상 변경
			const CHARACTER& rkSelectChar = g_MyInfo.GetCharacterToClassType( iClassType );		
			if( rkSelectChar.m_iAwakeType == AWAKE_RARE && pInfo->eAwakeType == AWAKE_NORMAL )
			{
				pPullDown->m_dwTitleColor		= TCT_DEFAULT_LIGHTGRAY;
				pPullDown->m_dwTitleSubColor	= TCT_DEFAULT_LIGHTGRAY;
			}

			//툴팁 할당	
			pPullDown->m_iTooltipIdx = g_BasicTooltipMgr.GetToolTipID( pInfo->szToopTipFile );
		}
		pList->AddList( pPullDown );
	}
}

byte ioSoldierAwakeManager::GetAwakeType( const CharAwakePullDownItem* pItem ) const
{
	const AwakeProduct* pProduct = GetAwakeProduct( pItem->m_iProductIdx );
	if( !pProduct )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "잘못된 각성 정보입니다." );
		return AWAKE_NONE;
	}

	const AwakeInfo* pAwake = GetAwake( pProduct->nNeedAwakeIdx );
	if( !pAwake )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "잘못된 재료 정보입니다." );
		return AWAKE_NONE;
	}

	return pAwake->eAwakeType;
}

AWAKERESULT ioSoldierAwakeManager::IsEnableAwakeMtrlUse( const CharAwakePullDownItem* pItem, int iClassType ) const
{
	const AwakeProduct* pProduct = GetAwakeProduct( pItem->m_iProductIdx );
	if( !pProduct )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "잘못된 각성 정보입니다." );
		return AR_ERROR;
	}

	const NeedMaterial* pMtrl = GetMtrl( pProduct->nNeedMtrlIdx );
	if( !pMtrl )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "잘못된 재료 정보입니다." );
		return AR_ERROR;
	}

	if( pMtrl->eType == NMT_ADDITIVE ) 
	{
		ioUserEtcItem *pUserEtcItem = g_MyInfo.GetUserEtcItem();
		if( !pUserEtcItem )
			return AR_ERROR;

		ioUserEtcItem::ETCITEMSLOT rkEtcItem;
		if( !pUserEtcItem->GetEtcItem( pMtrl->dwNeedCode, rkEtcItem ) )
			return AR_FAIL;
		if( rkEtcItem.m_iValue1 < pMtrl->nNeedCount )
			return AR_FAIL;
	}
	else if( pMtrl->eType == NMT_PESO ) 
	{
		if( g_MyInfo.GetMoney() < pMtrl->nNeedCount )
			return AR_FAIL;
	}
	return AR_OK;
}

void ioSoldierAwakeManager::OnAwakeMenuToolTipShow( const OnOverPullDownEvent& Event, int iCharArray ) const
{
	const ioPullDownList* pList = Event.GetEventCaller();
	if( !pList )
		return;

	const CharAwakePullDownItem* pItem = dynamic_cast<const CharAwakePullDownItem*>( pList->GetPullDown( Event.iOverIdx ) );
	if( !pItem )
		return;

	g_BasicTooltipMgr.ShowToolTip( pItem->m_iTooltipIdx );
}

void ioSoldierAwakeManager::OnAwakeMenuToolTipHide( const OnLeavePullDownEvent& Event, int iCharArray ) const
{
	const ioPullDownList* pList = Event.GetEventCaller();
	if( !pList )
		return;

	const CharAwakePullDownItem* pItem = dynamic_cast<const CharAwakePullDownItem*>( pList->GetPullDown( Event.iLeaveIdx ) );
	if( !pItem )
		return;

	g_BasicTooltipMgr.HideToolTip( pItem->m_iTooltipIdx );
}

bool ioSoldierAwakeManager::AwakeTryMessageBox( const ClickPullDownEvent& Event, int iCharArray, ioWnd* pCalledWnd )
{
	const ioPullDownList* pList = Event.GetEventCaller();
	if( !pList )
		return false;

	const CharAwakePullDownItem* pItem = dynamic_cast<const CharAwakePullDownItem*>( pList->GetPullDown( Event.iClickIdx ) );
	if( !pItem )
		return false;

	const CHARACTERDATA* pChar = g_MyInfo.GetCharacterData( iCharArray );
	if( !pChar )
		return false;
	
	const AwakeProduct* pProduct = GetAwakeProduct( pItem->m_iProductIdx );
	if( !pProduct )	
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "잘못된 각성 정보입니다." );
		return false;
	}
	
	const AwakeInfo* pInfo = GetAwake( pProduct->nNeedAwakeIdx );
	if( !pInfo )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "잘못된 각성 정보입니다." );
		return false;
	}

	const NeedMaterial* pMtrl = GetMtrl( pProduct->nNeedMtrlIdx );
	if( !pMtrl )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "잘못된 재료 정보입니다." );
		return false;
	}

	if( pInfo->eAwakeType == AWAKE_NORMAL && pChar->m_data.m_iAwakeType == AWAKE_RARE )
	{
		AwakeChangeFailMessageBox( GetAwakeName( static_cast<AwakeType>( static_cast<AwakeType>( pChar->m_data.m_iAwakeType ) ) ), GetAwakeName( pInfo->eAwakeType ) );
		return false;
	}

	//재료 타이틀
	std::string szMtrlTtitle;	
	GetMtrlTitle( szMtrlTtitle, pProduct->nNeedMtrlIdx );
	int iCode = pMtrl->dwNeedCode;
	int iArray = 0;
	ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
	kPrinter[iArray].SetTextStyle( TS_NORMAL );
	kPrinter[iArray].SetBkColor( 0, 0, 0 );
	kPrinter[iArray].SetTextColor( TCT_DEFAULT_GREEN );
	kPrinter[iArray].AddTextPiece( FONT_SIZE_13, "%d일간 ", pProduct->nNeedDay );
	kPrinter[iArray].SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter[iArray].AddTextPiece( FONT_SIZE_13, "%s을 하시겠습니까?", pInfo->szName.c_str() );
	iArray++;

	if( pInfo->eAwakeType == AWAKE_RARE && pChar->m_data.m_iAwakeType == AWAKE_NORMAL )
	{
		std::string szAwakeTitle = GetAwakeName( static_cast<AwakeType>( pChar->m_data.m_iAwakeType ) );
		kPrinter[iArray].SetTextStyle( TS_NORMAL );
		kPrinter[iArray].SetBkColor( 0, 0, 0 );
		kPrinter[iArray].SetTextColor( TCT_DEFAULT_RED );
		kPrinter[iArray].AddTextPiece( FONT_SIZE_13, "※ 현재의 %s은 사라집니다", szAwakeTitle.c_str() );
		iArray++;
		kPrinter[iArray].AddTextPiece( FONT_SIZE_13, "            " );
		iArray++;
	}
	else if( pChar->m_data.m_iAwakeType != AWAKE_NONE )
	{
		kPrinter[iArray].SetTextStyle( TS_NORMAL );
		kPrinter[iArray].SetBkColor( 0, 0, 0 );
		kPrinter[iArray].SetTextColor( TCT_DEFAULT_BLUE );
		kPrinter[iArray].AddTextPiece( FONT_SIZE_13, "(%d일이 추가됨)", pProduct->nNeedDay );
		iArray++;
		kPrinter[iArray].AddTextPiece( FONT_SIZE_13, "            " );
		iArray++;
	}

	kPrinter[iArray].SetTextStyle( TS_NORMAL );
	kPrinter[iArray].SetBkColor( 0, 0, 0 );
	kPrinter[iArray].SetTextColor( TCT_DEFAULT_BLUE );
	kPrinter[iArray].AddTextPiece( FONT_SIZE_13, "%s 소모", szMtrlTtitle.c_str() );
	iArray++;

	int iCount = 0;
	char szCount[MAX_PATH];
	kPrinter[iArray].SetTextStyle( TS_NORMAL );
	kPrinter[iArray].SetBkColor( 0, 0, 0 );
	kPrinter[iArray].SetTextColor( TCT_DEFAULT_GRAY );	

	if( pMtrl->eType == NMT_ADDITIVE )
	{
		ioUserEtcItem *pUserEtcItem = g_MyInfo.GetUserEtcItem();
		ioUserEtcItem::ETCITEMSLOT rkEtcItem;
		if( pUserEtcItem && pUserEtcItem->GetEtcItem( iCode, rkEtcItem ) )
			iCount = rkEtcItem.m_iValue1;

		Help::ConvertNumToStrComma( iCount, szCount, sizeof( szCount ) );
		kPrinter[iArray].AddTextPiece( FONT_SIZE_13, "(현재 %s개 보유)", szCount );
	}
	else
	{
		Help::ConvertNumToStrComma( g_MyInfo.GetMoney(), szCount, sizeof( szCount ) );
		kPrinter[iArray].AddTextPiece( FONT_SIZE_13, "(현재 %s페소 보유)", szCount );
	}
	iArray++;

	g_GUIMgr.SetPrevMsgListBoxWithTitle( NULL, MB_YESNO, pCalledWnd, kPrinter, pInfo->szName.c_str(), "확인", "확인", "취소" );
	return true;
}

void ioSoldierAwakeManager::AwakeChangeFailMessageBox( const std::string& szOrgName, const std::string& szChangeName )
{
	ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
	kPrinter[0].SetTextStyle( TS_NORMAL );
	kPrinter[0].SetBkColor( 0, 0, 0 );
	kPrinter[0].SetTextColor( TCT_DEFAULT_RED );	
	kPrinter[0].AddTextPiece( FONT_SIZE_13, "%s 상태입니다.", szOrgName.c_str() );

	kPrinter[1].SetTextStyle( TS_NORMAL );
	kPrinter[1].SetBkColor( 0, 0, 0 );
	kPrinter[1].SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter[1].AddTextPiece( FONT_SIZE_13, "%s을 할 수 없습니다.", szChangeName.c_str() );

	g_GUIMgr.SetPrevMsgListBoxWithTitle( NULL, MB_OK, NULL, kPrinter, szChangeName.c_str(), "닫기", "닫기", "닫기" );
}

void ioSoldierAwakeManager::AwakeResultMessageBox( const std::string& szName, int iLimitTime )
{
	ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
	kPrinter[0].SetTextStyle( TS_NORMAL );
	kPrinter[0].SetBkColor( 0, 0, 0 );
	kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );	
	kPrinter[0].AddTextPiece( FONT_SIZE_13, "각성되었습니다." );

	std::string szRemainTime;
	GetRemainTime( iLimitTime, szRemainTime );
	kPrinter[1].SetTextStyle( TS_NORMAL );
	kPrinter[1].SetBkColor( 0, 0, 0 );
	kPrinter[1].SetTextColor( TCT_DEFAULT_GREEN );
	kPrinter[1].AddTextPiece( FONT_SIZE_13, "%s", szRemainTime.c_str() );

	g_GUIMgr.SetPrevMsgListBoxWithTitle( NULL, MB_OK, NULL, kPrinter, szName.c_str(), "닫기", "닫기", "닫기" );
}

void ioSoldierAwakeManager::AwakeCharPeriodTypeError( const std::string& szName ) const
{
	ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
	kPrinter[0].SetTextStyle( TS_NORMAL );
	kPrinter[0].SetBkColor( 0, 0, 0 );
	kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );	
	kPrinter[0].AddTextPiece( FONT_SIZE_13, "기간제 용병은" );
	
	kPrinter[1].SetTextStyle( TS_NORMAL );
	kPrinter[1].SetBkColor( 0, 0, 0 );
	kPrinter[1].SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter[1].AddTextPiece( FONT_SIZE_13, "%s을 진행할 수 없습니다.", szName.c_str() );

	g_GUIMgr.SetPrevMsgListBoxWithTitle( NULL, MB_OK, NULL, kPrinter, szName.c_str(), "닫기", "닫기", "닫기" );
}

void ioSoldierAwakeManager::AwakeCharExerciseTypeError( const std::string& szName ) const
{
	ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
	kPrinter[0].SetTextStyle( TS_NORMAL );
	kPrinter[0].SetBkColor( 0, 0, 0 );
	kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );	
	kPrinter[0].AddTextPiece( FONT_SIZE_13, "체험 용병은" );

	kPrinter[1].SetTextStyle( TS_NORMAL );
	kPrinter[1].SetBkColor( 0, 0, 0 );
	kPrinter[1].SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter[1].AddTextPiece( FONT_SIZE_13, "%s을 진행할 수 없습니다.", szName.c_str() );

	g_GUIMgr.SetPrevMsgListBoxWithTitle( NULL, MB_OK, NULL, kPrinter, szName.c_str(), "닫기", "닫기", "닫기" );
}

void ioSoldierAwakeManager::OnAwakeMenuEvent( const ClickPullDownEvent& Event, int iCharArray ) const
{
	const ioPullDownList* pList = Event.GetEventCaller();
	if( !pList )
		return;

	const CharAwakePullDownItem* pItem = dynamic_cast<const CharAwakePullDownItem*>( pList->GetPullDown( Event.iClickIdx ) );
	if( !pItem )
		return;

	const CHARACTERDATA* pChar = g_MyInfo.GetCharacterData( iCharArray );
	if( !pChar )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "잘못된 캐릭터 정보입니다." );
		return;
	}

	//각성 재료를 가지고 있는지 검사
	AWAKERESULT eReuslt = g_AwakeMgr.IsEnableAwakeMtrlUse( pItem, pChar->m_data.m_class_type );
	if( eReuslt == AR_ERROR )
	{
		return;
	}
	else if( eReuslt == AR_FAIL )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "재료가 모자랍니다" );
		return;
	}

	byte eType = GetAwakeType( pItem );
	if( eType != AWAKE_NONE && pChar->m_data.m_iAwakeType == AWAKE_NONE )
	{
		//각성 추가
		SetAwake( pItem, pChar );
	}
	else if( eType != AWAKE_NONE && pChar->m_data.m_iAwakeType == eType )
	{
		//기간 연장
		ExtendAwake( pItem, pChar );
	}
	else if( eType == AWAKE_RARE && pChar->m_data.m_iAwakeType == AWAKE_NORMAL )
	{
		//노말 각성 -> 초월
		SetAwake( pItem, pChar );
	}
	else
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "잘못된 각성형식입니다." );
	}
}

void ioSoldierAwakeManager::SetAwake( const CharAwakePullDownItem* pItem, const CHARACTERDATA* pChar ) const
{
	if( !pChar )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "잘못된 각성 대상 캐릭터입니다" );
	}

	const AwakeProduct* pProduct = GetAwakeProduct( pItem->m_iProductIdx );
	if( !pProduct )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "잘못된 각성 정보입니다." );
		return;
	}

	const AwakeInfo* pAwake = GetAwake( pProduct->nNeedAwakeIdx );
	if( !pAwake )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "잘못된 각성 정보입니다." );
		return;
	}
	
	if( pChar->m_data.m_chExerciseStyle != EXERCISE_NONE )
	{
		AwakeCharExerciseTypeError( pAwake->szName );
		return;
	}

	if( pChar->m_data.m_ePeriodType != CPT_MORTMAIN )
	{
		AwakeCharPeriodTypeError( pAwake->szName );
		return;
	}

	SP2Packet kPacket( CTPK_CHAR_AWAKE );
	kPacket << pChar->m_dwIndex;
	kPacket << static_cast<byte>( pAwake->eAwakeType );
	kPacket << static_cast<byte>( pProduct->nNeedDay );
	TCPNetwork::SendToServer( kPacket );
	TCPNetwork::MouseBusy( true );
}

void ioSoldierAwakeManager::ExtendAwake( const CharAwakePullDownItem* pItem, const CHARACTERDATA* pChar ) const
{
	if( !pChar )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "잘못된 캐릭터입니다(각성)" );
	}

	const AwakeProduct* pProduct = GetAwakeProduct( pItem->m_iProductIdx );
	if( !pProduct )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "잘못된 각성 정보입니다." );
		return;
	}

	const AwakeInfo* pAwake = GetAwake( pProduct->nNeedAwakeIdx );
	if( !pAwake )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "잘못된 각성 정보입니다." );
		return;
	}
	
	if( pChar->m_data.m_chExerciseStyle != EXERCISE_NONE )
	{
		AwakeCharExerciseTypeError( pAwake->szName );
		return;
	}

	if( pChar->m_data.m_ePeriodType != CPT_MORTMAIN )
	{
		AwakeCharPeriodTypeError( pAwake->szName );
		return;
	}
	
	SP2Packet kPacket( CTPK_CHAR_AWAKE_EXTEND );
	kPacket << pChar->m_dwIndex;
	kPacket << static_cast<byte>( pProduct->nNeedDay );	
	TCPNetwork::SendToServer( kPacket );
	TCPNetwork::MouseBusy( true );
}

void ioSoldierAwakeManager::ApplyAwake( SP2Packet &rkPacket )
{	
	TCPNetwork::MouseBusy( false );

	byte eResult;
	rkPacket >> eResult;
	if( eResult == AWAKE_MY_OK )
	{
		byte iAwakeType;
		int iCharIndex;
		int iMtrlCode;
		int iLimitDate;
		short nMtrlCnt;
		rkPacket >> iCharIndex >> iAwakeType >> iMtrlCode >> nMtrlCnt >> iLimitDate;

		const CHARACTERDATA* pCharData = g_MyInfo.GetCharacterDataByIndex( iCharIndex );
		if( !pCharData )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "잘못된 각성대상 캐릭터입니다" );
			return;
		}

		g_MyInfo.ChangeReferenceCharacterData( MT_NONE );
		g_MyInfo.ChangeAwakeInfo( iCharIndex, iAwakeType );
		g_MyInfo.ChangeAwakeDate( iCharIndex, iLimitDate );

		__int64 iMoney;
		rkPacket >> iMoney;
		g_MyInfo.SetMoney( iMoney );

		const AwakeInfo* pAwake = GetAwakeByType( static_cast<AwakeType>( iAwakeType ) );
		if( pAwake )
		{
			AwakeResultMessageBox( pAwake->szName, iLimitDate );
		}

		//유저정보 갱신
		g_UserInfoMgr.SetCharAwake( g_MyInfo.GetPublicID(), pCharData->m_data.m_class_type, static_cast<int>( iAwakeType ) );

		//나와있는 캐릭터의 각성정보를 갱신
		if( m_pPlayStage )
		{
			const CHARACTER& rkSelectChar = g_MyInfo.GetCharacter( g_MyInfo.GetCharIndexByClassType( pCharData->m_data.m_class_type ) );
			ioBaseChar* pChar = m_pPlayStage->GetBaseChar( g_MyInfo.GetPublicID() );

			if( pChar && pChar->GetCharacterInfo().m_class_type == pCharData->m_data.m_class_type )
			{
				pChar->ChangeAwake( iAwakeType );
				pChar->ChangeAwakeDate( iLimitDate );
			}
		}

		//인벤토리 갱신
		MyInventoryWnd *pMyInventoryWnd = dynamic_cast<MyInventoryWnd*> ( g_GUIMgr.FindWnd( MY_INVENTORY_WND ) );
		if( pMyInventoryWnd )
		{
			if( pMyInventoryWnd->IsShow() )
				pMyInventoryWnd->UpdateTab( false, true );
		}

		g_MyInfo.RestoreReferenceCharacterData();

#if defined( USE_GA )
		if( iAwakeType == 1 )
		{
			char chLabel[32] = {0,};

			if ( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
				sprintf_e( chLabel, "%d", iCharIndex );
			else
				SafeSprintf( chLabel, sizeof(chLabel), "%1", iCharIndex );

			// CHAR_WAKEUP_NORMAL
			g_HttpMng.GA_EventTracking( g_MyInfo.GetUserIndex()
				, "Character"
				, "Normal"
				, chLabel
				, 1
				, "%2FCHAR%2FWAKEUP%2FNORMAL" );
		}
		if( iAwakeType == 2 )
		{
			char chLabel[32] = {0,};

			if ( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
				sprintf_e( chLabel, "%d", iCharIndex );
			else
				SafeSprintf( chLabel, sizeof(chLabel), "%1", iCharIndex );

			// CHAR_WAKEUP_SPECIAL
			g_HttpMng.GA_EventTracking( g_MyInfo.GetUserIndex()
				, "Character"
				, "Special"
				, chLabel
				, 1
				, "%2FCHAR%2FWAKEUP%2FSPECIAL" );
		}		
#endif

	}
	else if( eResult == AWAKE_OTHER_OK )
	{
		ioHashString szName;
		byte eAwakeType;
		int iClassType;
		rkPacket >> szName >> eAwakeType >> iClassType;
		g_UserInfoMgr.SetCharAwake( szName, iClassType, static_cast<int>( eAwakeType ) );

		if( m_pPlayStage )
		{ 
			ioBaseChar* pChar = m_pPlayStage->GetBaseChar( szName );
			if( pChar && pChar->GetCharacterInfo().m_class_type == iClassType )
			{
				pChar->ChangeAwake( static_cast<int>( eAwakeType ) );
				pChar->SetAwakeChangeEffect();
			}
		}		
	}
	else
	{
		byte nError;
		rkPacket >> nError;
		switch( nError )
		{
		case AWAKE_NONE_MATERIAL:
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "해당 각성 재료는 존재 하지 않습니다." );
			}
			break;
		case AWAKE_MATERIAL_SHORTAGE:
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "각성 재료가 부족합니다." );
			}
			break;
		case AWAKE_EXCEPTION:
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "잘못된 각성 정보입니다" );
			}
			break;
		}
	}
}

void ioSoldierAwakeManager::ApplyAwakeExtend( SP2Packet &rkPacket )
{
	TCPNetwork::MouseBusy( false );

	byte eResult;
	rkPacket >> eResult;

	if( eResult == AWAKE_MY_OK )
	{
		int iCharIndex;
		int iMtrlCode;
		int iLimitDate;
		short nMtrlCnt;
		rkPacket >> iCharIndex >> iMtrlCode >> nMtrlCnt >> iLimitDate;	

		g_MyInfo.ChangeReferenceCharacterData( MT_NONE );

		const CHARACTERDATA* pCharData = g_MyInfo.GetCharacterDataByIndex( iCharIndex );
		if( !pCharData )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "잘못된 각성대상 캐릭터입니다" );
			return;
		}	
		g_MyInfo.ChangeAwakeDate( iCharIndex, iLimitDate );

		__int64 iMoney;
		rkPacket >> iMoney;
		g_MyInfo.SetMoney( iMoney );

		const AwakeInfo* pAwake = GetAwakeByType( static_cast<AwakeType>( pCharData->m_data.m_iAwakeType ) );
		if( pAwake )
		{
			AwakeResultMessageBox( pAwake->szName, iLimitDate );
		}

		//유저정보 갱신
		g_UserInfoMgr.SetCharAwake( g_MyInfo.GetPublicID(), pCharData->m_data.m_class_type, static_cast<int>( pCharData->m_data.m_iAwakeType ) );

		//나와있는 캐릭터의 각성정보를 갱신
		if( m_pPlayStage )
		{
			const CHARACTER& rkSelectChar = g_MyInfo.GetCharacter( g_MyInfo.GetCharIndexByClassType( pCharData->m_data.m_class_type ) );
			ioBaseChar* pChar = m_pPlayStage->GetBaseChar( g_MyInfo.GetPublicID() );
			if( pChar && pChar->GetCharacterInfo().m_class_type == pCharData->m_data.m_class_type )
			{
				pChar->ChangeAwakeDate( iLimitDate );
			}
		}

		g_MyInfo.RestoreReferenceCharacterData();

#if defined( USE_GA )
		if( pCharData->m_data.m_iAwakeType == 1 )
		{
			char chLabel[32] = {0,};

			if ( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
				sprintf_e( chLabel, "%d", iCharIndex );
			else
				SafeSprintf( chLabel, sizeof(chLabel), "%1", iCharIndex );

			// CHAR_WAKEUP_NORMAL_EXTEND
			g_HttpMng.GA_EventTracking( g_MyInfo.GetUserIndex()
				, "Character"
				, "Normal_Extend"
				, chLabel
				, 1
				, "%2FCHAR%2FWAKEUP%2FNORMAL%2FEXTEND" );
		}
		if( pCharData->m_data.m_iAwakeType == 2 )
		{
			char chLabel[32] = {0,};

			if ( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
				sprintf_e( chLabel, "%d", iCharIndex );
			else
				SafeSprintf( chLabel, sizeof(chLabel), "%1", iCharIndex );

			// CHAR_WAKEUP_SPECIAL_EXTEND
			g_HttpMng.GA_EventTracking( g_MyInfo.GetUserIndex()
				, "Character"
				, "Special_Extend"
				, chLabel
				, 1
				, "%2FCHAR%2FWAKEUP%2FSPECIAL%2FEXTEND" );
		}		
#endif

	}
	else
	{
		byte nError;
		rkPacket >> nError;
		switch( nError )
		{
		case AWAKE_EXTEND_OVER_DATE:
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "더 이상 각성시간 연장을 할 수 없습니다." );
			}
			break;
		case AWAKE_EXTEND_NONE_MATERIAL:
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "해당 각성 재료는 존재 하지 않습니다." );
			}
			break;
		case AWAKE_EXTEND_MATERIAL_SHORTAGE:
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "각성 연장 할 재료가 부족합니다." );
			}
			break;
		case AWAKE_EXTEND_EXCEPTION:
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "잘못된 각성 정보입니다" );
			}
			break;
		}
	}
}

void ioSoldierAwakeManager::ApplyAwakeEnd( SP2Packet &rkPacket )
{
	int iMax = 0;
	rkPacket >> iMax;	
	for( int i = 0; i < iMax; ++i )
	{
		int iCharIndex;		
		byte eType;
		rkPacket >> iCharIndex >> eType;
		const CHARACTERDATA* pChar	= g_MyInfo.GetCharacterDataByIndex( iCharIndex );
		const AwakeInfo* pAwake		= GetAwakeByType( static_cast<AwakeType>( eType ) );
		if( pChar && pAwake )
		{
			int iClassType = pChar->m_data.m_class_type;

			AwakeEndInfo Info;
			Info.iClassType			= iClassType;
			Info.szClassName		= g_MyInfo.GetClassName( iClassType, g_MyInfo.GetCharPowerUpGradeByClassType( iClassType ) );
			Info.szClassIconName	= g_MyInfo.GetMySoldierIconName( iClassType ).c_str();
			Info.szClassSubIconName	= g_MyInfo.GetSoldierSubIconName( iClassType ).c_str();
			Info.szAwakeName		= pAwake->szName;
			Info.dwAwakeColor		= pAwake->dwNameColor;

			m_AwakeEndVector.push_back( Info );
			g_MyInfo.ChangeAwakeClear( iCharIndex );

			//유저정보 갱신
			g_UserInfoMgr.SetCharAwake( g_MyInfo.GetPublicID(), iClassType, static_cast<int>( AWAKE_NONE ) );

			//인벤토리 갱신
			MyInventoryWnd *pMyInventoryWnd = dynamic_cast<MyInventoryWnd*> (g_GUIMgr.FindWnd( MY_INVENTORY_WND ) );
			if( pMyInventoryWnd )
			{
				if( pMyInventoryWnd->IsShow() )
					pMyInventoryWnd->UpdateTab( false, true );
			}
		}
		else
		{
			LOG.PrintTimeAndLog(0, "%s - Awake End Error - max : %d, index : %d, type : %d", iMax, iCharIndex, static_cast<int>( eType ) );
		}
	}
}

bool ioSoldierAwakeManager::IsReserveDeleteAwake()
{
	if( m_AwakeEndVector.empty() )
		return false;

	return true;
}

void ioSoldierAwakeManager::GetFirstDeleteAwake( std::string& szClassName, std::string& szClassIconName, std::string& szClassSubIconName, std::string& szAwakeName, DWORD& dwAwakeColor, int& nClassType )
{
	if( m_AwakeEndVector.empty() )
		return;

	const AwakeEndInfo& rkInfo = *m_AwakeEndVector.begin();

	szClassName		= rkInfo.szClassName;
	szClassIconName	= rkInfo.szClassIconName;
	szClassSubIconName = rkInfo.szClassSubIconName;
	szAwakeName		= rkInfo.szAwakeName;
	dwAwakeColor	= dwAwakeColor;
	nClassType		= rkInfo.iClassType;
}

void ioSoldierAwakeManager::EraseDeleteExtraAwake( bool bAllErase )
{
	if( m_AwakeEndVector.empty() )
		return;

	if( bAllErase )
		m_AwakeEndVector.clear();
	else
		m_AwakeEndVector.erase( m_AwakeEndVector.begin() );
}