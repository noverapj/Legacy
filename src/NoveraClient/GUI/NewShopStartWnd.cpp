#include "StdAfx.h"

#include "../ioClassPrice.h"
#include "../ioSetItemInfoManager.h"
#include "../ioExtraItemInfoManager.h"
#include "../ioDecorationPrice.h"
#include "../ioEtcItemManager.h"
#include "../ioEtcItem.h"
#include "../ioCostumeShopInfoManager.h"
#include "../ioPresentMgr.h"
#include "../EtcHelpFunc.h"
#include "../ioDateHelp.h"

#include "NewShopWnd.h"
#include "NewShopStartWnd.h"
#include "ADImagePanelWnd.h"
#include "NewShopSortDefine.h"

NewShopStartWnd::NewShopStartWnd()
{
	m_pProductFrm	= NULL;

	m_iCurrPage		= 0;
}

NewShopStartWnd::~NewShopStartWnd()
{
	SAFEDELETE( m_pProductFrm );
}

void NewShopStartWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "ProductFrm" )
	{
		SAFEDELETE( m_pProductFrm );
		m_pProductFrm = pFrame;
	}
	else
	{
		ioWnd::AddRenderFrame( szType, pFrame );
	}
}

void NewShopStartWnd::iwm_show()
{
	m_CurrCategoryIndex	= 0;
	m_iCurrPage			= 0;

	ShowChildWnd( ID_AD_IMAGE_WND );
	ShowChildWnd( ID_AD_TEXT_WND );

	ParseProduct();
	ParseButton();
}

void NewShopStartWnd::iwm_hide()
{	
	HideChildWnd( ID_AD_IMAGE_WND );
	HideChildWnd( ID_AD_TEXT_WND );
}

void NewShopStartWnd::SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate )
{
	ioWnd::SetPositionToDefault( iParentWidth, iParentHeight, bChildMove, bCreate );

	ParseCategoryButton();
}

void NewShopStartWnd::ParseProduct()
{
	const ioHashString* CategoryName = g_ShopManager.GetADCategoryName( m_CurrCategoryIndex );
	if( CategoryName && !CategoryName->IsEmpty() )
	{
		m_CurrCategoryName = *CategoryName;
	}

	m_CurrADProductList.clear();
	for( int i = 0; i < g_ShopManager.GetADCategoryCount(); ++i )
	{	
		if( m_CurrCategoryIndex == i )
		{
			IntVec PrductList;
			g_ShopManager.GetADCategoryProductList( i, PrductList );
			for( IntVec::iterator iter = PrductList.begin(); iter != PrductList.end(); ++iter )
			{
				int Index = *iter;
				const ADProduct* pProduct = g_ShopManager.GetADProduct( Index - 1 );
				if( pProduct )
					m_CurrADProductList.push_back( *pProduct );
			}
		}
	}
}

void NewShopStartWnd::ParseButton()
{
	int iPos		= 0;
	int iCurrPage	= m_iCurrPage * (ID_GOODS_BTN_MAX - ID_GOODS_01_BTN);

	for( int iButtonID = ID_GOODS_01_BTN; iButtonID < ID_GOODS_BTN_MAX; ++iButtonID, ++iPos )
	{
		ioProductLinkButton* pButton = dynamic_cast<ioProductLinkButton*>( FindChildWnd( iButtonID ) );
		if( pButton )
		{
			if( COMPARE( iCurrPage + iPos, 0, (int)m_CurrADProductList.size() ) )
			{	
				pButton->SetProductInfo( m_CurrADProductList[iCurrPage + iPos] );
			}
			else
			{
				pButton->SetBlank();
			}
		}
	}

	ParseCategoryButton();
	ParsePageButton();
}

void NewShopStartWnd::ParseCategoryButton()
{
	ioButton* pCategoryPrev = dynamic_cast<ioButton*>( FindChildWnd( ID_CATEGORY_PREV_BTN ) );
	ioButton* pCategoryNext = dynamic_cast<ioButton*>( FindChildWnd( ID_CATEGORY_NEXT_BTN ) );
	if( !pCategoryPrev || !pCategoryNext )
		return;

	//버튼 카테고리 타이틀 좌우측에 일정 간격을 두고 위치하도록 조정
	int iWidth = 0;
	if( !m_CurrCategoryName.IsEmpty() )
	{
		iWidth = g_FontMgr.GetTextWidth( m_CurrCategoryName.c_str(), TS_NORMAL, FONT_SIZE_13 );
	}

	if( pCategoryPrev )
		pCategoryPrev->SetWndPos( CATEGORY_TITLE_X - iWidth / 2 - pCategoryPrev->GetWidth()- CATEGORY_TITLE_OFFSET, CATEGORY_TITLE_Y );

	if( pCategoryNext )			
		pCategoryNext->SetWndPos( CATEGORY_TITLE_X + iWidth / 2 + CATEGORY_TITLE_OFFSET, CATEGORY_TITLE_Y );

	//카테고리 버튼 활성/비활성화 체크
	if( m_CurrCategoryIndex <= 0 )
	{
		pCategoryPrev->SetInActive();
	}
	else
	{
		pCategoryPrev->SetActive();
	}

	if( m_CurrCategoryIndex >= g_ShopManager.GetADCategoryCount() - 1 )
	{
		pCategoryNext->SetInActive();
	}
	else
	{
		pCategoryNext->SetActive();
	}
}

void NewShopStartWnd::ParsePageButton()
{
	//페이지 버튼
	ioButton* pPagePrev = dynamic_cast<ioButton*>( FindChildWnd( ID_PAGE_PREV_BTN ) );
	ioButton* pPageNext = dynamic_cast<ioButton*>( FindChildWnd( ID_PAGE_NEXT_BTN ) );
	if( !pPagePrev || !pPageNext )
		return;

	//페이지 버튼 활성/비활성화 체크
	if( m_iCurrPage <= 0 )
	{
		pPagePrev->SetInActive();
	}
	else
	{
		pPagePrev->SetActive();
	}

	if( m_iCurrPage >= GetPageMax() )
	{
		pPageNext->SetInActive();
	}
	else
	{
		pPageNext->SetActive();
	}
}

int NewShopStartWnd::GetPageMax()
{
	return (int)(m_CurrADProductList.size()-1) / (ID_GOODS_BTN_MAX - ID_GOODS_01_BTN);
}

void NewShopStartWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_AD_IMAGE_WND:
		{
			if( cmd == IOBN_BTNUP )
			{
				ADImagePanelWnd* pAD = dynamic_cast<ADImagePanelWnd*>( pWnd );
				if( pAD && !pAD->IsSliding() )
					pAD->MoveDirectLink();
			}
		}
		break;
	case ID_GOODS_01_BTN:
	case ID_GOODS_02_BTN:
	case ID_GOODS_03_BTN:
	case ID_GOODS_04_BTN:
	case ID_GOODS_05_BTN:
	case ID_GOODS_06_BTN:
	case ID_GOODS_07_BTN:
		{
			if( cmd == IOBN_BTNUP )
			{
				ioProductLinkButton* pButton = dynamic_cast<ioProductLinkButton*>( pWnd );
				if( pButton )
					pButton->MoveDirectLink();
			}
		}
		break;
	case ID_CATEGORY_PREV_BTN:
		{
			if( cmd == IOBN_BTNUP )
			{				
				m_CurrCategoryIndex--;
				m_CurrCategoryIndex	= max( 0, m_CurrCategoryIndex );
				m_iCurrPage			= 0;
				ParseProduct();
				ParseButton();
			}
		}
		break;
	case ID_CATEGORY_NEXT_BTN:
		{
			if( cmd == IOBN_BTNUP )
			{
				m_CurrCategoryIndex++;
				m_CurrCategoryIndex = min( g_ShopManager.GetADCategoryCount() - 1, m_CurrCategoryIndex );
				m_iCurrPage			= 0;
				ParseProduct();
				ParseButton();
			}
		}
		break;
	case ID_PAGE_PREV_BTN:
		{
			if( cmd == IOBN_BTNUP )
			{
				m_iCurrPage--;
				m_iCurrPage = max( 0, m_iCurrPage );
				ParseProduct();
				ParseButton();
			}
		}
		break;
	case ID_PAGE_NEXT_BTN:
		{
			if( cmd == IOBN_BTNUP )
			{
				m_iCurrPage++;
				m_iCurrPage = min( GetPageMax(), m_iCurrPage );
				ParseProduct();
				ParseButton();
			}
		}
		break;
	}
}

void NewShopStartWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;
}

void NewShopStartWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_pProductFrm )
		m_pProductFrm->Render( iXPos, iYPos );

	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );

	if( !m_CurrCategoryName.IsEmpty() )
	{	
		g_FontMgr.PrintText( iXPos + CATEGORY_TITLE_X, iYPos + CATEGORY_TITLE_TEXT_Y - 1, FONT_SIZE_13, m_CurrCategoryName.c_str() );
	}

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_WHITE );
	kPrinter.AddTextPiece( FONT_SIZE_13, "%d", m_CurrCategoryIndex + 1 );

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, "/%d", g_ShopManager.GetADCategoryCount() );
	kPrinter.PrintFullText( iXPos + CATEGORY_PAGE_X, iYPos + CATEGORY_PAGE_Y, TAT_LEFT );
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
ioProductLinkButton::ioProductLinkButton()
{
	Clear();
}

ioProductLinkButton::~ioProductLinkButton()
{
}

void ioProductLinkButton::Clear()
{
	m_szName.Clear();
	m_szGold.Clear();
	m_szPeso.Clear();
	m_CurrADProductLink.Clear();
	m_ShopItemInfo.Clear();
}

void ioProductLinkButton::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_ICON_BTN:
		{
			if( cmd == IOBN_BTNUP )
			{
				MoveDirectLink();
			}
		}
		break;
	}
}

void ioProductLinkButton::SetBlank()
{
	SetInActive();
	Clear();
	ParseButton( "", "", SHOP_MARK_TYPE_NONE, 0, 0, false, 0 );
}

void ioProductLinkButton::SetProductInfo( ADProduct& Product )
{	
	SetActive();
	Clear();

	switch( Product.m_Link.m_eADProductType )
	{
	case ADPT_SOLDIER:
		if( SetSoldierInfo( Product ) )
			return;
		break;
	case ADPT_EXTRA:
		if( SetExtraItemInfo( Product ) )
			return;
		break;
	case ADPT_DECO:
		if( SetDecoInfo( Product ) )
			return;
		break;
	case ADPT_ETC:
		if( SetEtcInfo( Product ) )
			return;
		break;
	case ADPT_COSTUME:
		if( SetCostumeInfo( Product ) )
			return;
		break;
	}
		
	SetBlank();
}

bool ioProductLinkButton::SetSoldierInfo( ADProduct& Product )
{	
	if( !g_ClassPrice.IsActive( Product.m_Link.m_iValue1 ) )
		return false;
		
	int iClassType = Product.m_Link.m_iValue1;
	const ioSetItemInfo *pInfo = g_SetItemInfoMgr.GetSetInfoByClass( iClassType );
	if( !pInfo )
		return false;

	if( pInfo->GetShopMarkType() == SHOP_MARK_TYPE_DATE && 0 < pInfo->GetLimitDate() )
	{
		if( !NewShopItemHelp::IsAvailableDate( pInfo->GetLimitDate() ) )
			return false;
	}

	if( pInfo->GetName().IsEmpty() )
		return false;

	m_CurrADProductLink	= Product.m_Link;
	m_szName			= pInfo->GetName();
		
	int iGold = g_ClassPrice.GetMortmainCharCash( Product.m_Link.m_iValue1 );	
	int iPeso = g_ClassPrice.GetClassBuyPeso( Product.m_Link.m_iValue1, g_ClassPrice.GetLimitDatePeso( Product.m_Link.m_iValue1 , 0 ) );

	char szComma[MAX_PATH]="";
	if( 0 < iGold )
	{
		Help::ConvertNumToStrComma( iGold, szComma, sizeof( szComma ) );
		m_szGold = szComma;
	}
	else
	{
		Help::ConvertNumToStrComma( iPeso, szComma, sizeof( szComma ) );
		m_szPeso = szComma;
	}

	int eShopMarkType = pInfo->GetShopMarkType();
	if( eShopMarkType == SHOP_MARK_TYPE_NONE )
	{
		if( g_MyInfo.GetClassArray( Product.m_Link.m_iValue1 ) != -1 )
			eShopMarkType = SHOP_MARK_TYPE_BOUGHT;
	}

	NewShopItemHelp::CreateSoldierItemInfo( pInfo, m_ShopItemInfo );

	ParseButton( g_MyInfo.GetMySoldierIconName( Product.m_Link.m_iValue1 ), g_MyInfo.GetSoldierSubIconName( Product.m_Link.m_iValue1 ), 
		eShopMarkType, 0, pInfo->GetLimitDate(), false, g_UIImageRenderMgr.GetPowerUpGradeType( Product.m_Link.m_iValue1 ) );
	
	return true;
}

bool ioProductLinkButton::SetExtraItemInfo( ADProduct& Product )
{
	RandomMachineInfo *pInfo = g_ExtraItemInfoMgr.GetRandomMachineInfo( Product.m_Link.m_iValue1 );
	if( !pInfo )
		return false;

	if( !pInfo->IsActive() )
		return false;

	if( pInfo->GetShopMarkType() == SHOP_MARK_TYPE_DATE && 0 < pInfo->GetLimitDate() )
	{
		if( !NewShopItemHelp::IsAvailableDate( pInfo->GetLimitDate() ) )
			return false;
	}

	if( pInfo->GetName().IsEmpty() )
		return false;

	m_CurrADProductLink = Product.m_Link;

	m_szName		= pInfo->GetName();
	int iGold		= pInfo->GetNeedCash();
	int iPeso		= pInfo->GetNeedPeso( 0 );
	int nReinForce	= ( pInfo->GetGroup() % PRESENT_EXTRAITEM_DIVISION_1 ) / PRESENT_EXTRAITEM_DIVISION_2;
	
	char szComma[MAX_PATH]="";
	if( 0 < iGold )
	{
		Help::ConvertNumToStrComma( iGold, szComma, sizeof( szComma ) );
		m_szGold = szComma;
	}
	else
	{
		Help::ConvertNumToStrComma( iPeso, szComma, sizeof( szComma ) );
		m_szPeso = szComma;
	}
	
	NewShopItemHelp::CreateExtraItemInfo( pInfo, m_ShopItemInfo );

	ParseButton( pInfo->GetIconName(), "", pInfo->GetShopMarkType(), nReinForce, pInfo->GetLimitDate(), false, pInfo->GetGradeType() );

	return true;
}

bool ioProductLinkButton::SetDecoInfo( ADProduct& Product )
{
	const DecoData* pInfo = g_DecorationPrice.GetDecoDataPtr( Product.m_Link.m_iValue1, Product.m_Link.m_iValue2, Product.m_Link.m_iValue3 - 1 );
	if( !pInfo )
		return false;

	if( !pInfo->m_bActive )
		return false;

	if( pInfo->m_iShopMarkType == SHOP_MARK_TYPE_DATE && 0 < pInfo->m_iLimitDate )
	{
		if( !NewShopItemHelp::IsAvailableDate( pInfo->m_iLimitDate ) )
			return false;
	}
	
	if( pInfo->m_szName.IsEmpty() )
		return false;

	m_CurrADProductLink = Product.m_Link;

	m_szName	= pInfo->m_szName;
	int iGold	= pInfo->m_iCash;
	int iPeso	= pInfo->m_iPeso;
	
	char szComma[MAX_PATH]="";
	if( 0 < iGold )
	{
		Help::ConvertNumToStrComma( iGold, szComma, sizeof( szComma ) );
		m_szGold = szComma;
	}
	else
	{
		Help::ConvertNumToStrComma( iPeso, szComma, sizeof( szComma ) );
		m_szPeso = szComma;
	}

	NewShopItemHelp::CreateDecoItemInfo( pInfo,Product.m_Link.m_iValue1, Product.m_Link.m_iValue2, m_ShopItemInfo );

	ParseButton( pInfo->m_szIcon, "", pInfo->m_iShopMarkType, 0, pInfo->m_iLimitDate, false, 0 );

	return true;
}

bool ioProductLinkButton::SetEtcInfo( ADProduct& Product )
{
	ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( Product.m_Link.m_iValue1 );
	if( !pEtcItem )
		return false;

	int iArray = Product.m_Link.m_iValue2 - 1;
	if( !pEtcItem->IsActive( iArray ) )
		return false;

	if( pEtcItem->GetShopMarkType( iArray ) == SHOP_MARK_TYPE_DATE && 0 < pEtcItem->GetLimitDate( iArray ) )
	{
		if( !NewShopItemHelp::IsAvailableDate( pEtcItem->GetLimitDate( iArray ) ) )
			return false;
	}

	if( pEtcItem->GetShopName( iArray ).IsEmpty() )
		return false;

	m_CurrADProductLink = Product.m_Link;

	m_szName	= pEtcItem->GetShopName( iArray );
	int iGold	= pEtcItem->GetCash( iArray );
	int iPeso	= pEtcItem->GetPeso( 0 );

	char szComma[MAX_PATH]="";
	if( 0 < iGold )
	{
		Help::ConvertNumToStrComma( iGold, szComma, sizeof( szComma ) );
		m_szGold = szComma;
	}
	else
	{
		Help::ConvertNumToStrComma( iPeso, szComma, sizeof( szComma ) );
		m_szPeso = szComma;
	}

	NewShopItemHelp::CreateEtcItemInfo( pEtcItem, 0, m_ShopItemInfo );
	ParseButton( pEtcItem->GetIconName(), pEtcItem->GetSubIconName(), pEtcItem->GetShopMarkType( iArray ), 0, pEtcItem->GetLimitDate( iArray ), false, pEtcItem->GetGradeType() );

	return true;
}

bool ioProductLinkButton::SetCostumeInfo( ADProduct& Product )
{
	CostumeShopInfo* pCsotume = g_CostumeShopInfoMgr.GetCostumeMachineInfo( Product.m_Link.m_iValue1 );
	if( !pCsotume )
		return false;

	if( !pCsotume->IsActive() )
		return false;
	
	if( pCsotume->GetName().IsEmpty() )
		return false;

	m_CurrADProductLink = Product.m_Link;

	m_szName	= pCsotume->GetName();
	int iGold	= pCsotume->GetNeedCash();
	int	iPeso	= pCsotume->GetNeedPeso( 0 );

	if( m_szName.IsEmpty() )
		return false;
		
	char szComma[MAX_PATH]="";
	if( 0 < iGold )
	{
		Help::ConvertNumToStrComma( iGold, szComma, sizeof( szComma ) );
		m_szGold = szComma;
	}
	else
	{
		Help::ConvertNumToStrComma( iPeso, szComma, sizeof( szComma ) );
		m_szPeso = szComma;
	}

	NewShopItemHelp::CreateCostumeInfo( pCsotume, m_ShopItemInfo );
	ParseButton( pCsotume->GetIconName(), "", pCsotume->GetShopMarkType(), 0, 0, true, pCsotume->GetGradeType() );

	return true;
}

void ioProductLinkButton::ParseButton( const ioHashString& szIconName, const ioHashString& szSubIcon, int iMarkType, int iNumber, int iLimitDate, bool bCostume, int nGradeType )
{
	ioProductIconButton* pButton = dynamic_cast<ioProductIconButton*>( FindChildWnd( ID_ICON_BTN ) );
	if( pButton )
	{
		pButton->SetProductIconInfo( szIconName, szSubIcon, iMarkType, iNumber, iLimitDate, bCostume, nGradeType );
	}
}

void ioProductLinkButton::MoveDirectLink()
{
	NewShopWnd *pNewShopWnd = dynamic_cast<NewShopWnd*> ( g_GUIMgr.FindWnd( NEW_SHOP_WND ) );
	if( !pNewShopWnd )
		return;

	if( m_CurrADProductLink.IsNoneLink() )
		return;

	if( m_CurrADProductLink.m_eADProductType == ADPT_DECO )
	{
		pNewShopWnd->SetReserveDecoSex( m_CurrADProductLink.m_iValue1 );
		pNewShopWnd->SetDirectSmallTab( m_CurrADProductLink.m_iTab, NewShopWnd::ID_SMALL_TAB_1_BTN + m_CurrADProductLink.m_iSubTab, m_CurrADProductLink.m_iValue2, m_CurrADProductLink.m_iValue3 );
	}
	else
	{
		pNewShopWnd->SetDirectSmallTab( m_CurrADProductLink.m_iTab, NewShopWnd::ID_SMALL_TAB_1_BTN + m_CurrADProductLink.m_iSubTab, m_CurrADProductLink.m_iValue1, m_CurrADProductLink.m_iValue2 );
	}
}

void ioProductLinkButton::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;
}

void ioProductLinkButton::OnNormalRender( int iXPos, int iYPos )
{
	if( !m_szName.IsEmpty() )
	{
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );		
		g_FontMgr.PrintTextWidthCut( iXPos + 45, iYPos + 65, FONT_SIZE_10, 83, m_szName.c_str() );
	}

	if( !m_szGold.IsEmpty() )
	{
		g_FontMgr.SetTextColor( 141, 87, 4 );
		g_FontMgr.PrintText( iXPos + 45, iYPos + 78, FONT_SIZE_10, "%s 골드", m_szGold );
	}
	else if( !m_szPeso.IsEmpty() )
	{
		g_FontMgr.SetTextColor( TCT_DEFAULT_BLUE );
		g_FontMgr.PrintText( iXPos + 45, iYPos + 78, FONT_SIZE_10, "%s 페소", m_szPeso );
	}

	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 45, iYPos + 100, FONT_SIZE_10, "자세히보기" );
}

void ioProductLinkButton::OnDrawNormal( int iXPos, int iYPos )
{
	ioButton::OnDrawNormal( iXPos, iYPos );
	OnNormalRender( iXPos, iYPos );
}

void ioProductLinkButton::OnDrawOvered( int iXPos, int iYPos )
{
	ioButton::OnDrawOvered( iXPos, iYPos );
	OnNormalRender( iXPos, iYPos );
}

void ioProductLinkButton::OnDrawPushed( int iXPos, int iYPos )
{
	ioButton::OnDrawPushed( iXPos, iYPos );
	OnNormalRender( iXPos, iYPos );
}

void ioProductLinkButton::OnDrawDisabled( int iXPos, int iYPos )
{
	ioButton::OnDrawDisabled( iXPos, iYPos );

	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
	g_FontMgr.PrintText( iXPos + 45, iYPos + 63, FONT_SIZE_10, "상품없음" );
	g_FontMgr.PrintText( iXPos + 45, iYPos + 78, FONT_SIZE_10, "-");
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
ioProductIconButton::ioProductIconButton()
{
	m_pIcon					= NULL;
	m_pSubIcon = NULL;
	m_pIconBack				= NULL;
	m_pIconEmpty			= NULL;
	m_pNewMark				= NULL;
	m_pHotMark				= NULL;
	m_pEventMark			= NULL;
	m_pLimitMark			= NULL;
	m_pLimitDayMark			= NULL;
	m_pSoldOutMark			= NULL;
	m_pGradeLimitClearMark	= NULL;
	m_pNumText				= NULL;
	m_pPlusText				= NULL;
	m_pCostumeMark			= NULL;

	m_pStateBlueFrm			= NULL;
	m_pStatePinkFrm			= NULL;
	m_pDDayMark			= NULL;

	m_iMarkType				= SHOP_MARK_TYPE_NONE;
	m_iReinForceNumber		= 0;
	m_iLimitDay				= 0;

	m_bCostume				= false;
	m_bScreen				= false;

	m_dwScreenTime			= 200;
	m_dwScreenGapTime		= 200;
	m_dwScreenCheckTime		= 0;
	m_nGradeType = 0;
}

ioProductIconButton::~ioProductIconButton()
{
	SAFEDELETE( m_pIcon );
	SAFEDELETE( m_pSubIcon );
	SAFEDELETE( m_pIconBack );
	SAFEDELETE( m_pIconEmpty );
	SAFEDELETE( m_pNewMark );
	SAFEDELETE( m_pHotMark );
	SAFEDELETE( m_pEventMark );
	SAFEDELETE( m_pLimitMark );
	SAFEDELETE( m_pSoldOutMark );
	SAFEDELETE( m_pLimitDayMark );
	SAFEDELETE( m_pGradeLimitClearMark );
	SAFEDELETE( m_pNumText );
	SAFEDELETE( m_pPlusText );
	SAFEDELETE( m_pCostumeMark );

	SAFEDELETE( m_pStateBlueFrm );
	SAFEDELETE( m_pStatePinkFrm );
	SAFEDELETE( m_pDDayMark );
}

void ioProductIconButton::SetProductIconInfo( const ioHashString& szIconName, const ioHashString& szSubIcon, int iMarkType, int iReinForceNumber, int iLimitDate, bool bCostume, int nGradeType )
{
	SAFEDELETE( m_pIcon );
	SAFEDELETE( m_pSubIcon );
	m_nGradeType = nGradeType;

	if( !szIconName.IsEmpty() )
	{
		SetActive();
		m_pIcon				= g_UIImageSetMgr.CreateImageByFullName( szIconName );
		m_pSubIcon = g_UIImageSetMgr.CreateImageByFullName( szSubIcon );
		m_iMarkType			= iMarkType;
		m_iReinForceNumber	= iReinForceNumber;
		m_bCostume			= bCostume;
		m_iLimitDay			= 0;
		m_dwScreenCheckTime = FRAMEGETTIME();

		if( 0 < iLimitDate )
		{
			CTime kLimitTime;
			CTime kCurrTime = DateHelp::GetCurrTime();
			DateHelp::ConvertCTime( iLimitDate, kLimitTime );
			CTimeSpan TimeGap = kLimitTime - kCurrTime;
			m_iLimitDay = TimeGap.GetDays();
		}

		if( m_pIcon )
			m_pIcon->SetScale( 0.67f );
		if( m_pSubIcon )
			m_pSubIcon->SetScale( 0.67f );
	}
	else
	{
		m_iMarkType			= SHOP_MARK_TYPE_NONE;
		m_iReinForceNumber	= 0;
		m_bCostume			= false;
		m_iLimitDay			= 0;

		SetInActive();
	}
}

void ioProductIconButton::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "IconBack" )
	{
		SAFEDELETE( m_pIconBack );
		m_pIconBack = pImage;

		if( m_pIconBack )		
			m_pIconBack->SetScale( 0.67f );
	}
	else if( szType == "IconEmpty" )
	{	
		SAFEDELETE( m_pIconEmpty );
		m_pIconEmpty = pImage;

		if( m_pIconEmpty )		
			m_pIconEmpty->SetScale( 0.67f );
	}
	else if( szType == "NewMark" )
	{
		SAFEDELETE( m_pNewMark );
		m_pNewMark = pImage;
	}
	else if( szType == "HotMark" )
	{
		SAFEDELETE( m_pHotMark );
		m_pHotMark = pImage;
	}
	else if( szType == "EventMark" )
	{
		SAFEDELETE( m_pEventMark );
		m_pEventMark = pImage;
	}
	else if( szType == "LimitMark" )
	{
		SAFEDELETE( m_pLimitMark );
		m_pLimitMark = pImage;
	}
	else if( szType == "LimitDayMark" )
	{
		SAFEDELETE( m_pLimitDayMark );
		m_pLimitDayMark = pImage;
	}
	else if( szType == "SoldOutMark" )
	{
		SAFEDELETE( m_pSoldOutMark );
		m_pSoldOutMark = pImage;
	}
	else if( szType == "GradeLimitClearMark" )
	{
		SAFEDELETE( m_pGradeLimitClearMark );
		m_pGradeLimitClearMark = pImage;
	}
	else if( szType == "NumText" )
	{
		SAFEDELETE( m_pNumText );
		m_pNumText = pImage;
	}
	else if( szType == "PlusText" )
	{
		SAFEDELETE( m_pPlusText );
		m_pPlusText = pImage;
	}
	else if( szType == "CostumeImage" )
	{
		SAFEDELETE( m_pCostumeMark );
		m_pCostumeMark = pImage;

		if( m_pCostumeMark )		
			m_pCostumeMark->SetScale( 0.67f );
	}
	else if( szType == "DDayMark" )
	{
		SAFEDELETE( m_pDDayMark );
		m_pDDayMark = pImage;
	}
	else
	{
		ioButton::AddRenderImage( szType, pImage );	
	}	
}

void ioProductIconButton::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( pFrame )
	{
		if( szType == "StateBlueFrm" )
		{
			SAFEDELETE( m_pStateBlueFrm );
			m_pStateBlueFrm = pFrame;
		}
		else if( szType == "StatePinkFrm" )
		{
			SAFEDELETE( m_pStatePinkFrm );
			m_pStatePinkFrm = pFrame;
		}
		else
		{	
			ioButton::AddRenderFrame( szType, pFrame );
		}
	}
}

void ioProductIconButton::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	if( m_dwScreenCheckTime > 0 && m_dwScreenGapTime > 0 && m_dwScreenTime > 0 )
	{
		DWORD dwCurTime = FRAMEGETTIME();
		if( m_bScreen )
		{
			if( m_dwScreenCheckTime+m_dwScreenTime < dwCurTime )
			{
				m_bScreen = false;
				m_dwScreenCheckTime = dwCurTime;
			}
		}
		else
		{
			if( m_dwScreenCheckTime+m_dwScreenGapTime < dwCurTime )
			{
				m_bScreen = true;
				m_dwScreenCheckTime = dwCurTime;
			}
		}
	}

	if( m_bOver && !m_bClicked )
	{
		ioLSMouse *pMouse = g_App.GetMouse();
		if( pMouse )
			pMouse->SetToolTipWndID( m_pParent->GetID() );
	}	
}

void ioProductIconButton::OnIconRender( int iXPos, int iYPos, UIRenderType eIconBackType, UIRenderType eIconType )
{
	if( m_pIconBack )
		m_pIconBack->Render( iXPos + GetWidth()/2, iYPos + GetHeight()/2, eIconBackType, TFO_BILINEAR );

	if ( eIconType != UI_RENDER_SCREEN )
		g_UIImageRenderMgr.RenderGradeIconBack( m_nGradeType, iXPos + GetWidth()/2, iYPos + GetHeight()/2, 0.67f, eIconType );

	if( m_pIcon )
		m_pIcon->Render( iXPos + GetWidth()/2, iYPos + GetHeight()/2, eIconType, TFO_BILINEAR );
	if( m_pSubIcon )
		m_pSubIcon->Render( iXPos + GetWidth()/2, iYPos + GetHeight()/2, eIconType, TFO_BILINEAR );
}

void ioProductIconButton::OnMarkRender( int iXPos, int iYPos )
{
	if( m_iMarkType == SHOP_MARK_TYPE_NEW )
	{
		if( m_pNewMark )
			m_pNewMark->Render( iXPos + X_MARK_OFFSET, iYPos + Y_MARK_OFFSET, UI_RENDER_NORMAL );
	}
	else if( m_iMarkType == SHOP_MARK_TYPE_HIT )
	{
		if( m_pHotMark )
			m_pHotMark->Render( iXPos + X_MARK_OFFSET, iYPos + Y_MARK_OFFSET, UI_RENDER_NORMAL );
	}
	else if( m_iMarkType == SHOP_MARK_TYPE_SALE )
	{
	}
	else if( m_iMarkType == SHOP_MARK_TYPE_EVENT )
	{
		if( m_pEventMark )
		{
			m_pEventMark->Render( iXPos + X_MARK_OFFSET, iYPos + Y_MARK_OFFSET, UI_RENDER_NORMAL );
			if( m_bScreen )
				m_pEventMark->Render( iXPos + X_MARK_OFFSET, iYPos + Y_MARK_OFFSET, UI_RENDER_SCREEN );
		}
	}
	else if( m_iMarkType == SHOP_MARK_TYPE_BOUGHT )
	{
		if( m_pStateBlueFrm )
		{
			m_pStateBlueFrm->Render( iXPos + X_MARK_OFFSET, iYPos + Y_MARK_OFFSET, UI_RENDER_NORMAL );
			if( m_bScreen )
				m_pStateBlueFrm->Render( iXPos + X_MARK_OFFSET, iYPos + Y_MARK_OFFSET, UI_RENDER_SCREEN );
		}

		g_FontMgr.SetTextStyle( TS_BOLD );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
		g_FontMgr.PrintText( iXPos + X_TEXT_OFFSET, iYPos + Y_TEXT_OFFSET, FONT_SIZE_11, "보유중" );
	}
	else if( m_iMarkType == SHOP_MARK_TYPE_LIMIT_GOODS )
	{
		if( m_pLimitMark )
		{
			m_pLimitMark->Render( iXPos + X_MARK_OFFSET, iYPos + Y_MARK_OFFSET, UI_RENDER_NORMAL );
			if( m_bScreen )
				m_pLimitMark->Render( iXPos + X_MARK_OFFSET, iYPos + Y_MARK_OFFSET, UI_RENDER_SCREEN );
		}
	}
	else if( m_iMarkType == SHOP_MARK_TYPE_LIMIT_DAYS )
	{
		if( m_pLimitDayMark )
		{
			m_pLimitDayMark->Render( iXPos + X_MARK_OFFSET, iYPos + Y_MARK_OFFSET, UI_RENDER_NORMAL );
			if( m_bScreen )
				m_pLimitDayMark->Render( iXPos + X_MARK_OFFSET, iYPos + Y_MARK_OFFSET, UI_RENDER_SCREEN );
		}
	}
	else if( m_iMarkType == SHOP_MARK_TYPE_SOLD_OUT )
	{
		if( m_pSoldOutMark )
		{
			m_pSoldOutMark->Render( iXPos + X_MARK_OFFSET, iYPos + Y_MARK_OFFSET, UI_RENDER_NORMAL );
			if( m_bScreen )
				m_pSoldOutMark->Render( iXPos + X_MARK_OFFSET, iYPos + Y_MARK_OFFSET, UI_RENDER_SCREEN );
		}
	}
	else if( m_iMarkType == SHOP_MARK_TYPE_GRADE_LIMIT_CLEAR )
	{
		if( m_pGradeLimitClearMark )
		{
			m_pGradeLimitClearMark->Render( iXPos + X_MARK_OFFSET, iYPos + Y_MARK_OFFSET, UI_RENDER_NORMAL );
			if( m_bScreen )
				m_pGradeLimitClearMark->Render( iXPos + X_MARK_OFFSET, iYPos + Y_MARK_OFFSET, UI_RENDER_SCREEN );
		}
	}
	else if( m_iMarkType == SHOP_MARK_TYPE_DATE )
	{	
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );

		if( m_iLimitDay == 0 )
		{
			if( m_pDDayMark )
			{
				m_pDDayMark->Render( iXPos + X_MARK_OFFSET, iYPos + Y_MARK_OFFSET, UI_RENDER_NORMAL );
				if( m_bScreen )
					m_pDDayMark->Render( iXPos + X_MARK_OFFSET, iYPos + Y_MARK_OFFSET, UI_RENDER_SCREEN );
			}
		}
		else
		{
			if( m_pStatePinkFrm )
			{
				m_pStatePinkFrm->Render( iXPos + X_MARK_OFFSET, iYPos + Y_MARK_OFFSET, UI_RENDER_NORMAL );
				if( m_bScreen )
					m_pStatePinkFrm->Render( iXPos + X_MARK_OFFSET, iYPos + Y_MARK_OFFSET, UI_RENDER_SCREEN );
			}

			g_FontMgr.PrintText( iXPos + X_TEXT_OFFSET, iYPos + Y_TEXT_OFFSET, FONT_SIZE_10, "D-%d", m_iLimitDay );
			g_FontMgr.PrintText( iXPos + X_TEXT_OFFSET - 1, iYPos + Y_TEXT_OFFSET, FONT_SIZE_10, "D-%d", m_iLimitDay );
		}		
	}
}

void ioProductIconButton::OnCostumeRender( int iXPos, int iYPos )
{
	// UI코스튬 관련 (코스튬 마크)
	if( m_bCostume && m_pCostumeMark )
		m_pCostumeMark->Render( iXPos + GetWidth()/2, iYPos + GetHeight()/2, UI_RENDER_NORMAL );
}

void ioProductIconButton::OnNumberRender( int iXPos, int iYPos )
{
	if( m_iReinForceNumber <= 0 || !m_pNumText || !m_pPlusText )
		return;
		
	int iNewXPos	= iXPos + GetWidth()/2;
	int iNewYPos	= iYPos + GetHeight()/2;
	int iCurPos		= 0;
	int iDefaultPos	= iNewXPos;
	int iCurYPos	= iNewYPos + REINFORCE_Y;
	int iTotalSize	= 0;

	if( m_iReinForceNumber < 10 )
		iTotalSize = PLUS_SIZE + NUM_SIZE - NUM_GAP;
	else if( m_iReinForceNumber < 100 )
		iTotalSize = PLUS_SIZE + NUM_SIZE*2 - NUM_GAP*2;
	else
		iTotalSize = PLUS_SIZE + NUM_SIZE*3 - NUM_GAP*3;

	iCurPos = iDefaultPos - (iTotalSize / 2);
	m_pPlusText->Render( iCurPos, iCurYPos );

	iCurPos += PLUS_SIZE-NUM_GAP;
	m_pNumText->RenderNum( iCurPos, iCurYPos, m_iReinForceNumber, -NUM_GAP );
}

void ioProductIconButton::OnDrawNormal( int iXPos, int iYPos )
{
	ioButton::OnDrawNormal( iXPos, iYPos );

	OnIconRender( iXPos, iYPos, UI_RENDER_MULTIPLY, UI_RENDER_NORMAL );
	OnMarkRender( iXPos, iYPos );
	OnCostumeRender( iXPos, iYPos );
}

void ioProductIconButton::OnDrawOvered( int iXPos, int iYPos )
{
	ioButton::OnDrawOvered( iXPos, iYPos );

	OnIconRender( iXPos, iYPos, UI_RENDER_MULTIPLY, UI_RENDER_NORMAL );
	OnIconRender( iXPos, iYPos, UI_RENDER_MULTIPLY, UI_RENDER_SCREEN );
	OnMarkRender( iXPos, iYPos );
	OnCostumeRender( iXPos, iYPos );
}

void ioProductIconButton::OnDrawPushed( int iXPos, int iYPos )
{
	ioButton::OnDrawPushed( iXPos, iYPos );
	
	OnIconRender( iXPos, iYPos, UI_RENDER_MULTIPLY, UI_RENDER_NORMAL );
	OnIconRender( iXPos, iYPos, UI_RENDER_MULTIPLY, UI_RENDER_SCREEN );
	OnMarkRender( iXPos, iYPos );
	OnCostumeRender( iXPos, iYPos );
}

void ioProductIconButton::OnDrawDisabled( int iXPos, int iYPos )
{
	ioButton::OnDrawDisabled( iXPos, iYPos );

	if( m_pIconEmpty )
		m_pIconEmpty->Render( iXPos + GetWidth()/2, iYPos + GetHeight()/2, UI_RENDER_MULTIPLY, TFO_BILINEAR );
}