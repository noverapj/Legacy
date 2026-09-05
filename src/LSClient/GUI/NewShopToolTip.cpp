#include "StdAfx.h"

#include "../ioDateHelp.h"

#include "NewShopToolTip.h"

#include "NewShopWnd.h"
#include "NewShopStartWnd.h"
#include "NewShopIconBtn.h"

NewShopToolTip::NewShopToolTip()
{
	m_iItemType    = 0;
	m_iManualIndex = 0;

	m_nGradeType = 0;
}

NewShopToolTip::~NewShopToolTip()
{
	ClearDesc();
}

void NewShopToolTip::OnProcess( float fTimePerSec )
{
	NewShopWnd *pNewShop = dynamic_cast<NewShopWnd*> ( g_GUIMgr.FindWnd( NEW_SHOP_WND ) );
	if( !pNewShop || !pNewShop->IsShow() )
	{
		HideWnd();
		return;
	}

	ioLSMouse *pMouse = g_App.GetMouse();
	if( !pMouse )
		return;

	NewShopItemInfo kCurInfo;

	NewShopStartWnd* pNewShopStart = dynamic_cast<NewShopStartWnd*>( pNewShop->FindChildWnd( NewShopWnd::ID_START_WND ) );
	if( pNewShopStart && pNewShopStart->IsShow() )
	{
		if( COMPARE( pMouse->GetToolTipWndID(), NewShopStartWnd::ID_GOODS_01_BTN, NewShopStartWnd::ID_GOODS_BTN_MAX ) )
		{
			ioProductLinkButton* pButton = dynamic_cast<ioProductLinkButton*>( pNewShopStart->FindChildWnd( pMouse->GetToolTipWndID() ) );
			if( pButton )
				kCurInfo = pButton->GetShopItemInfo();
		}
	}
	else
	{
		// id가 중복되면 수정 필요.
		if( COMPARE( pMouse->GetToolTipWndID(), NewShopWnd::ID_ITEM_INFO_WND_1_BTN, NewShopWnd::ID_ITEM_INFO_WND_8_BTN + 1) )
		{
			pNewShop->GetCurItemInfo( kCurInfo, pMouse->GetToolTipWndID() );
		}
		else if( COMPARE( pMouse->GetToolTipWndID(), NewShopNeedPesoWnd::ID_ITEM_1_BTN, NewShopNeedPesoWnd::ID_ITEM_3_BTN + 1 ) )
		{
			NewShopNeedPesoWnd *pNeedPesoWnd = dynamic_cast<NewShopNeedPesoWnd*> ( g_GUIMgr.FindWnd( NEW_SHOP_NEED_PESO_WND ) );
			if( pNeedPesoWnd )
				pNeedPesoWnd->GetCurItemInfo( kCurInfo, pMouse->GetToolTipWndID() );
		}
		else if( COMPARE( pMouse->GetToolTipWndID(), NewShopBuyWnd::ID_DECO_PREVIEW_KINDRED_BTN, NewShopBuyWnd::ID_DECO_PREVIEW_UNDERWEAR_BTN + 1 ) )
		{
			NewShopBuyWnd *pNewShopBuyWnd = dynamic_cast<NewShopBuyWnd*> ( pNewShop->FindChildWnd( NewShopWnd::ID_BUY_WND ) );
			if( pNewShopBuyWnd )
			{
				InventoryIconBtn *pBtn = dynamic_cast<InventoryIconBtn*> ( pNewShopBuyWnd->FindChildWnd( pMouse->GetToolTipWndID() ) );
				if( pBtn )
					pBtn->GetNewShopItemInfo( kCurInfo );
			}
		}
	}

	if( IsShow() )
	{	
		if( !SetInfo( kCurInfo ) )                                          
			HideWnd();
		else
		{
			pMouse->SetToolTipWndID( 0 );
			pMouse->SetToolTipCursor();
			UpdateWndSize();
			int iXPos = 0;
			int iYPos = 0;
			Help::GetNotCutWndPos( pMouse->GetMousePos().x, pMouse->GetMousePos().y, GetWidth(), GetHeight(), iXPos, iYPos );
			SetWndPos( iXPos, iYPos );

		}
	}
	else
	{
		if( SetInfo( kCurInfo ) ) // setinfo 후에 showwnd
		{
			ShowWnd();
			UpdateWndSize();
			int iXPos = 0;
			int iYPos = 0;
			Help::GetNotCutWndPos( pMouse->GetMousePos().x, pMouse->GetMousePos().y, GetWidth(), GetHeight(), iXPos, iYPos );
			SetWndPos( iXPos, iYPos );
		}
	}
}

void NewShopToolTip::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	RenderDesc( iXPos, iYPos );

	enum { PRINT_XOFFSET = 14, PRINT_YOFFSET = 99, };
	PrintManual( iXPos+PRINT_XOFFSET, iYPos+PRINT_YOFFSET, FONT_SIZE_12 );
}

void NewShopToolTip::ClearDesc()
{
	for (int i = 0; i < MAX_DESC ; i++)
	{
		m_szDescArray[i].ClearList();
	}
}

void NewShopToolTip::RenderDesc( int iXPos, int iYPos )
{
	enum 
	{
		X_OFFSET       = 90,
		MAX_TYPE       = 2,
	};

	iXPos += X_OFFSET;

	int iYPosArray[MAX_TYPE][MAX_DESC]={ 21, 41, 59, 
		32, 53, 0, };

	int iDescCnt = 0;
	for( int i = 0 ; i < MAX_DESC ; i++ )
	{
		if( m_szDescArray[i].IsEmpty() )
			continue;
		iDescCnt++;
	}

	int iType = 0;
	if( iDescCnt != MAX_DESC )
		iType = 1;

	int iCnt  = 0;
	for( int i = 0 ; i < MAX_DESC ; i++ )
	{
		if( m_szDescArray[i].IsEmpty() )
			continue;
		if( COMPARE( iType, 0, MAX_TYPE ) && COMPARE( iCnt, 0, MAX_DESC ) )
			m_szDescArray[i].PrintFullTextWidthCut( iXPos, iYPos + iYPosArray[iType][iCnt] ,TAT_LEFT, NEW_SHOP_TOOLTIP_WIDTHCUT_SIZE );
		iCnt++;
	}
}

void NewShopToolTip::PrintManual( int a_iXPos, int a_iYPos , float fScale )
{
	ManualMgr *pManual = NULL;
	if( m_iItemType == PRESENT_SOLDIER )	
		pManual = ioItemManualMgr::GetSingletonPtr();
	else if( m_iItemType == PRESENT_DECORATION )
		pManual = ioDecoManualMgr::GetSingletonPtr();
	else if( m_iItemType == PRESENT_ETC_ITEM || m_iItemType == PRESENT_EXTRAITEM_BOX )
		pManual = ioEtcManualMgr::GetSingletonPtr();
	else if( m_iItemType == PRESENT_DEFAULT )
		pManual = ioEventGoodsManualMgr::GetSingletonPtr();
	else if( m_iItemType == PRESENT_COSTUME_BOX )
		pManual = ioCostumeShopManualMgr::GetSingletonPtr();

	if( !pManual )
		return;

	bool bAutoLine = false;
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal && pLocal->IsTooltipAutoLine() )
		bAutoLine = true;

	ioComplexStringPrinter kPrinter;
	int iMaxLine = pManual->GetMaxLine( m_iManualIndex );
	for(int iLine = 1; iLine < iMaxLine; iLine++)
	{
		if( !bAutoLine )
		{
			pManual->SetPrinter( m_iManualIndex, iLine, fScale, kPrinter);
			a_iYPos += pManual->GetSpaceLine( m_iManualIndex, iLine);
			kPrinter.PrintFullTextWidthCut( a_iXPos, a_iYPos, TAT_LEFT, DESC_WIDTH );
			kPrinter.ClearList();
		}
		else
		{
			pManual->SetPrinter( m_iManualIndex, iLine, fScale, kPrinter);
			enum { MAX_LOOP = 20, Y_GAP = 18, };
			bool bFirstGap = true;
			for (int i = 0; i < MAX_LOOP ; i++)
			{
				if( kPrinter.GetFullWidth() < DESC_WIDTH )
				{
					if( bFirstGap )
					{
						a_iYPos += pManual->GetSpaceLine( m_iManualIndex, iLine);
						bFirstGap = false;
					}
					else
						a_iYPos += Y_GAP;
					kPrinter.PrintFullText( a_iXPos, a_iYPos, TAT_LEFT );
					kPrinter.ClearList();
					break;
				}

				ioComplexStringPrinter kPrinterA, kPrinterB;
				kPrinter.SplitFullText( kPrinterA, kPrinterB, DESC_WIDTH );
				if( bFirstGap )
				{
					a_iYPos += pManual->GetSpaceLine( m_iManualIndex, iLine);
					bFirstGap = false;
				}
				else
					a_iYPos += Y_GAP;
				kPrinterA.PrintFullText( a_iXPos, a_iYPos, TAT_LEFT );
				kPrinterB.CopyFullText( kPrinter );
				kPrinterA.ClearList();
				kPrinterB.ClearList();
			}
		}
	}	
}

bool NewShopToolTip::SetInfo( NewShopItemInfo &rkInfo )
{
	if( rkInfo.m_sName.IsEmpty() || rkInfo.m_sIcon.IsEmpty() )
	{
		ClearDesc();
		return false;
	}

	// desc
	ClearDesc();

	// 상점가격디폴트
	bool bTextPeso = false;
	if( ( rkInfo.m_bShowPeso && rkInfo.GetPeso( 0 ) > 0 )       ||
		( rkInfo.GetPeso( 0 ) > 0 && rkInfo.GetCash( 0 ) <= 0 ) ||
		( rkInfo.m_iNeedLevel == 0 && rkInfo.GetPeso( 0 ) > 0 && rkInfo.GetCash( 0 ) > 0 ) )
		bTextPeso = true;

	// 이벤트 상품이다
	bool bTextStamp = false;  
	if( rkInfo.GetStamp( 0 ) > 0 )
		bTextStamp = true;

	// 이벤트 상품 장비 강화값
	int iReinforceNumber = 0;
	if( rkInfo.m_iItemType == PRESENT_DEFAULT )
	{
		// 장비일 경우
		if( rkInfo.m_iValue0 == PRESENT_EXTRAITEM )
		{
			// 강화값
			iReinforceNumber = ( rkInfo.m_iValue2 % PRESENT_EXTRAITEM_DIVISION_1 ) / PRESENT_EXTRAITEM_DIVISION_2;
		}

		// 한정 상품이 전부 팔렸으면 Sold Out로 교체
		if( rkInfo.m_iShopMarkType == SHOP_MARK_TYPE_LIMIT_GOODS || rkInfo.m_iShopMarkType == SHOP_MARK_TYPE_LIMIT_DAYS )
		{
			if( rkInfo.GetValue3( 1 ) == 0 )
				rkInfo.m_iShopMarkType = SHOP_MARK_TYPE_SOLD_OUT;
		}
	}

	//
	m_szDescArray[0].SetTextStyle( TS_NORMAL );
	m_szDescArray[0].SetBkColor( 0, 0, 0 );	
	m_szDescArray[0].SetTextColor( TCT_DEFAULT_DARKGRAY ); // 진회색
	m_szDescArray[0].AddTextPiece( FONT_SIZE_17, rkInfo.m_sName.c_str() );

	m_szDescArray[1].SetTextStyle( TS_NORMAL );
	m_szDescArray[1].SetBkColor( 0, 0, 0);	
	m_szDescArray[1].SetTextColor( TCT_DEFAULT_GRAY ); // 연회색

	bool bAscendingOrder = false;

	if( rkInfo.m_iItemType == PRESENT_ETC_ITEM ||
		rkInfo.m_iItemType == PRESENT_EXTRAITEM_BOX ||
		rkInfo.m_iItemType == PRESENT_DECORATION ||
		rkInfo.m_iItemType == PRESENT_COSTUME_BOX )
		bAscendingOrder = true;

	// 확성기는 마지막 가격 표시
	if( rkInfo.m_iItemType == PRESENT_ETC_ITEM && rkInfo.m_iValue1 == ioEtcItem::EIT_ETC_WHOLE_CHAT )
		bAscendingOrder = false;

	// 용병세일이벤트이면 가장 마지막 캐쉬 가격
	if( rkInfo.m_iItemType == PRESENT_SOLDIER && rkInfo.m_iShopMarkType == SHOP_MARK_TYPE_EVENT )
	{
		bTextPeso       = false;
	}

	// 경험치+200 예외
	if( rkInfo.m_iItemType == PRESENT_ETC_ITEM && rkInfo.m_iValue1 == ioEtcItem::EIT_ETC_EXP_BONUS && rkInfo.GetPeso( 2 ) > 0 )
	{
		bTextPeso = true;
	}

	// 페소+100 예외
	if( rkInfo.m_iItemType == PRESENT_ETC_ITEM && rkInfo.m_iValue1 == ioEtcItem::EIT_ETC_PESO_BONUS && rkInfo.GetPeso( 2 ) > 0 )
	{
		bTextPeso = true;
	}

	bool bTournamentCoin = false;
	if( 0 < rkInfo.GetTournamentCoin( 0 ) )
	{		
		bTournamentCoin = true;
		bTextPeso		= false;
		bTextStamp		= false;
	}

	// 상점 표시 디폴트
	if( rkInfo.m_iItemType == PRESENT_DEFAULT )
	{
		m_szDescArray[1].AddTextPiece( FONT_SIZE_13, g_PresentMgr.GetPresentValue2Text( rkInfo.m_iValue0, rkInfo.m_iValue1, rkInfo.m_iValue2 ).c_str() );
	}
	else if( !bTournamentCoin )
	{
		// 상점가격디폴트
		int iBonusPesoArray = 0;
		if( !bAscendingOrder )
			iBonusPesoArray = rkInfo.m_vBonusPeso.size()-1;

		if( rkInfo.GetBonusPeso( iBonusPesoArray ) <= 0 )
		{
			if( !bAscendingOrder )
				iBonusPesoArray--;
			else
				iBonusPesoArray++;
		}

		if( rkInfo.m_iNeedLevel > 0 )
		{
			char szText[MAX_PATH]="";
			if( rkInfo.m_iItemType == PRESENT_DECORATION )
				SafeSprintf( szText, sizeof( szText ),  STR(1), rkInfo.m_iNeedLevel );
			else
			{
				if( rkInfo.GetPeso( 0 ) != 0 )
				{
					g_LevelMgr.GetGradeName( rkInfo.m_iNeedLevel, szText, sizeof(szText), true );
					StringCbCat( szText, sizeof( szText ), STR(2) );
				}
				else
					StringCbCopy( szText, sizeof( szText ), STR(3) );
			}
			m_szDescArray[1].AddTextPiece( FONT_SIZE_13, szText );
		}
		else if( rkInfo.GetBonusPeso( iBonusPesoArray ) > 0 && !bTextPeso )
		{
			char szComma[MAX_PATH]="";
			Help::ConvertNumToStrComma( rkInfo.GetBonusPeso( iBonusPesoArray  ), szComma, sizeof( szComma ) );
			char szText[MAX_PATH]="";
			SafeSprintf( szText, sizeof( szText ), STR(4), szComma );
			m_szDescArray[1].AddTextPiece( FONT_SIZE_13, szText );
		}
	}

	if( bTextPeso )
	{
		// 상점가격디폴트
		int iArray = 0;
		// 경험치+200 예외
		if( rkInfo.m_iItemType == PRESENT_ETC_ITEM && rkInfo.m_iValue1 == ioEtcItem::EIT_ETC_EXP_BONUS && rkInfo.GetPeso( 2 ) > 0 )
		{
			iArray = 2;
		}
		// 페소+100 예외
		if( rkInfo.m_iItemType == PRESENT_ETC_ITEM && rkInfo.m_iValue1 == ioEtcItem::EIT_ETC_PESO_BONUS && rkInfo.GetPeso( 2 ) > 0 )
		{
			iArray = 2;
		}

		char szComma[MAX_PATH]="";
		Help::ConvertNumToStrComma( rkInfo.GetPeso( iArray ), szComma, sizeof( szComma ) );
		m_szDescArray[2].SetTextStyle( TS_NORMAL );
		m_szDescArray[2].SetBkColor( 0, 0, 0 );	
		m_szDescArray[2].SetTextColor( TCT_DEFAULT_BLUE ); // 파랑색
		m_szDescArray[2].AddTextPiece( FONT_SIZE_17, STR(5), szComma );
	}
	else if( bTextStamp )
	{
		// 상점가격디폴트
		int iArray = 0;
		char szComma[MAX_PATH]="";
		Help::ConvertNumToStrComma( rkInfo.GetStamp( iArray ), szComma, sizeof( szComma ) );
		m_szDescArray[2].SetTextStyle( TS_NORMAL );
		m_szDescArray[2].SetBkColor( 0, 0, 0 );	
		m_szDescArray[2].SetTextColor( TCT_DEFAULT_BLUE ); // 파랑색

		ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( rkInfo.m_dwStampEtcItem );
		if( pEtcItem )
			m_szDescArray[2].AddTextPiece( FONT_SIZE_17, STR(8), pEtcItem->GetName().c_str(), szComma );
		else
			m_szDescArray[2].AddTextPiece( FONT_SIZE_17, STR(7), szComma );
	}
	else if( bTournamentCoin )
	{
		int iArray = 0;
		char szComma[MAX_PATH]="";
		Help::ConvertNumToStrComma( rkInfo.GetTournamentCoin( iArray ), szComma, sizeof( szComma ) );
		m_szDescArray[2].SetTextStyle( TS_NORMAL );
		m_szDescArray[2].SetBkColor( 0, 0, 0 );	
		m_szDescArray[2].SetTextColor( TCT_DEFAULT_ORANGE );
		m_szDescArray[2].AddTextPiece( FONT_SIZE_17, "%s 대회코인", szComma );
	}
	else
	{
		// 상점가격디폴트
		int iArray = 0;
		if( !bAscendingOrder )
			iArray = rkInfo.m_vCash.size()-1;

		if( rkInfo.GetCash( iArray ) <= 0 )
		{
			if( !bAscendingOrder )
				iArray--;
			else
				iArray++;
		}

		char szComma[MAX_PATH]="";
		Help::ConvertNumToStrComma( rkInfo.GetCash( iArray ) , szComma, sizeof( szComma ) );
		m_szDescArray[2].SetTextStyle( TS_NORMAL );
		m_szDescArray[2].SetBkColor( 0, 0, 0 );	
		m_szDescArray[2].SetTextColor( 141,87,4 ); // 골드색
		m_szDescArray[2].AddTextPiece( FONT_SIZE_17, STR(6), szComma );
	}

	// icon btn
	NewShopIconBtn *pBtn = dynamic_cast< NewShopIconBtn* > ( FindChildWnd( ID_ICON_BTN ) );
	if( !pBtn )
	{
		ClearDesc();
		LOG.PrintTimeAndLog( 0, "%s pBtn == NULL", __FUNCTION__ );
		return false;
	}
	pBtn->SetNumber( iReinforceNumber );

	ioUIRenderImage *pIcon = g_UIImageSetMgr.CreateImageByFullName( rkInfo.m_sIcon );
	ioUIRenderImage *pSubIcon = g_UIImageSetMgr.CreateImageByFullName( rkInfo.m_sSubIcon );
	if( !pBtn->SetIcon( pIcon ) )
	{
		ClearDesc();
		return false;
	}
	pBtn->SetSubIcon( pSubIcon );
	pBtn->SetMarkType( rkInfo.m_iShopMarkType );
	if( rkInfo.m_iShopMarkType == SHOP_MARK_TYPE_DATE && 0 < rkInfo.m_iLimitDate )
	{
		CTime kLimitTime;
		CTime kCurrTime = DateHelp::GetCurrTime();
		DateHelp::ConvertCTime( rkInfo.m_iLimitDate, kLimitTime );
		CTimeSpan TimeGap = kLimitTime - kCurrTime;
		if( 0 < TimeGap.GetDays() )
			pBtn->SetLimitDay( TimeGap.GetDays() );
		else
			pBtn->SetLimitDay( 0 );
	}

	m_nGradeType = g_PresentMgr.GetGradeType( rkInfo.m_iItemType, rkInfo.m_iValue1 );
	pBtn->SetGradeType( m_nGradeType );

	if( rkInfo.m_iItemType == PRESENT_COSTUME || rkInfo.m_iItemType == PRESENT_COSTUME_BOX )
		pBtn->SetTopSubIconType( NewShopIconBtn::TSIT_COSTUME );

	int iArray = 0;
	if( !bAscendingOrder )
		iArray = rkInfo.m_vManualIndex.size() - 1;

	m_iItemType    = rkInfo.m_iItemType;
	m_iManualIndex = rkInfo.GetManualIndex( iArray );

	return true;
}

void NewShopToolTip::iwm_show()
{
	UpdateWndSize();
	return;
}

void NewShopToolTip::UpdateWndSize()
{
	ManualMgr *pManual = NULL;
	if( m_iItemType == PRESENT_SOLDIER )	
		pManual = ioItemManualMgr::GetSingletonPtr();
	else if( m_iItemType == PRESENT_DECORATION )
		pManual = ioDecoManualMgr::GetSingletonPtr();
	else if( m_iItemType == PRESENT_ETC_ITEM || m_iItemType == PRESENT_EXTRAITEM_BOX )
		pManual = ioEtcManualMgr::GetSingletonPtr();
	else if( m_iItemType == PRESENT_DEFAULT )
		pManual = ioEventGoodsManualMgr::GetSingletonPtr();
	else if( m_iItemType == PRESENT_COSTUME_BOX )
		pManual = ioCostumeShopManualMgr::GetSingletonPtr();

	if( !pManual )
		return;

	int iMaxLine = pManual->GetMaxLine( m_iManualIndex );
	int iNewLine = iMaxLine - 1; // 1번째는 타이틀이므로 삭제
	if( iNewLine <= 0 )
		return;

	bool bAutoLine = false;
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal && pLocal->IsTooltipAutoLine() )
		bAutoLine = true;

	if( !bAutoLine )
	{
		int iTotalLineGap = 18;
		for(int iLine = 1; iLine < iMaxLine; iLine++)
		{
			iTotalLineGap += pManual->GetSpaceLine( m_iManualIndex, iLine);
		}

		enum { DEFAULT_HEIGHT = 111, };
		SetSize( GetWidth(), DEFAULT_HEIGHT+iTotalLineGap );
	}
	else
	{
		ioComplexStringPrinter kPrinter;
		int iTotalLineGap = 18;
		for(int iLine = 1; iLine < iMaxLine; iLine++)
		{
			pManual->SetPrinter( m_iManualIndex, iLine, FONT_SIZE_12, kPrinter);
			enum { MAX_LOOP = 20, Y_GAP = 18, };
			bool bFirstGap = true;
			for (int i = 0; i < MAX_LOOP ; i++)
			{
				if( kPrinter.GetFullWidth() < DESC_WIDTH )
				{
					if( bFirstGap )
					{
						iTotalLineGap += pManual->GetSpaceLine( m_iManualIndex, iLine);
						bFirstGap = false;
					}
					else
						iTotalLineGap += Y_GAP;
					kPrinter.ClearList();
					break;
				}

				ioComplexStringPrinter kPrinterA, kPrinterB;
				kPrinter.SplitFullText( kPrinterA, kPrinterB, DESC_WIDTH );
				if( bFirstGap )
				{
					iTotalLineGap += pManual->GetSpaceLine( m_iManualIndex, iLine);
					bFirstGap = false;
				}
				else
					iTotalLineGap += Y_GAP;
				kPrinterB.CopyFullText( kPrinter );
				kPrinterA.ClearList();
				kPrinterB.ClearList();
			}
		}

		enum { DEFAULT_HEIGHT = 111, }; 
		SetSize( GetWidth(), DEFAULT_HEIGHT+iTotalLineGap );
	}
}