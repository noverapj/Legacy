#include "StdAfx.h"
#include "../ioMyInfo.h"
#include "../io3DEngine/ioWnd.h"
#include "../io3DEngine/ioFontManager.h"
#include "../io3DEngine/iostringmanager.h"
#include "../TextColorTable.h"
#include "../ioApplication.h"
#include "../WndID.h"
#include "../NetworkWrappingFunc.h"
#include "ioSP2GUIManager.h"
#include "../io3DEngine/ioUIRenderImage.h"
#include "../io3DEngine/ioUIRenderFrame.h"
#include "../io3DEngine/ioUIRenderElement.h"
#include "../io3DEngine/ioUI3DEffectRender.h"

#include "PopUpStoreWnd.h"
#include "../ioDecorationPrice.h"
#include "MyInventoryWnd.h"

PopupStoreWnd::PopupStoreWnd() :
  m_pBtnBuy(NULL)
, m_pBtnShow(NULL)
, m_pBtnClose(NULL)
, m_pWndGolds(NULL)
, m_pWndEtcs(NULL)
, m_pWndImgs1(NULL)
, m_pWndImgs2(NULL)
, m_pWndName(NULL)
, m_pWndCount(NULL)
, m_pWndItem(NULL)
, m_iPopupIndex(0)
, m_pWndGoldWnd1(NULL)
, m_pWndGoldWnd2(NULL)
, m_pWndGoldWnd3(NULL)
, m_pWndGoldWnd4(NULL)
, m_pWndEtcWnd1(NULL)
, m_pWndEtcWnd2(NULL)
, m_pWndEtcWnd3(NULL)
, m_pWndEtcWnd4(NULL)
, m_iCash(-1)
, m_iMsgType(0)
, m_pWndEtcWnd5(NULL)
, m_pWndEtcWnd6(NULL)
, m_bSubPopup(false)
{
}
PopupStoreWnd::~PopupStoreWnd()
{
}
void PopupStoreWnd::iwm_create()
{
	// ini 정보 세팅
	char szKey[MAX_PATH] = {0,};
	char szBuf[MAX_PATH] = {0,};

	ioINILoader_e iLoader("config/sp2_popupstore.ini");
	iLoader.SetTitle_e("Common");

	int iMaxCount = iLoader.LoadInt( "Max", 0 );
	for( int i = 0; i < iMaxCount; ++i )
	{
		sprintf( szKey, "Popup%d", i+1 );
		iLoader.SetTitle( szKey );

		sPopupItemInfo popupInfo;
		popupInfo.bActive = iLoader.LoadBool( "Active", true );
		popupInfo.iIndex = iLoader.LoadInt( "Index", 0 );		
		popupInfo.iPresentType = iLoader.LoadInt( "PresentType", 0 );
		popupInfo.iPresentValue1 = iLoader.LoadInt( "PresentValue1", 0 );
		popupInfo.iPresentValue2 = iLoader.LoadInt( "PresentValue2", 0 );
		popupInfo.iCash = iLoader.LoadInt( "Cash", 0 );
		popupInfo.iPrevCash = iLoader.LoadInt( "PrevCash", 0 );
		popupInfo.iDisCount = iLoader.LoadInt( "Discount", 0 );
		popupInfo.iManualIdx = iLoader.LoadInt( "ManualIndex", 0 );
		popupInfo.iChance = iLoader.LoadInt( "Chance", 0 );
		

		for( int j = 0; j < ePopupConditionMax; ++j )
		{
			sprintf( szKey, "Condition%d_Min", j+1 );
			popupInfo.dwConditionMin[j] =iLoader.LoadInt( szKey, 0 );
		}

		m_vecPopupItemInfo.push_back( popupInfo );
	}

	ioWnd* pBaseWnd = FindChildWnd(eID_BASE);

	m_pWndImgs1 = pBaseWnd->FindChildWnd(eID_IMGS1);
	m_pWndImgs2 = pBaseWnd->FindChildWnd(eID_IMGS2);

	m_pWndItem = pBaseWnd->FindChildWnd( eID_ITEM );
	m_pWndName = pBaseWnd->FindChildWnd( eID_NAME );
	m_pWndCount = pBaseWnd->FindChildWnd( eID_COUNT );
	m_pWndGolds = pBaseWnd->FindChildWnd( eID_GOLDS );
	m_pBtnBuy = (ioButton*)pBaseWnd->FindChildWnd( eID_BUY );
	m_pBtnBuy->SetCheckParentOutSide(true);
	m_pBtnShow = (ioButton*)pBaseWnd->FindChildWnd( eID_SHOW );
	m_pBtnClose = (ioButton*)pBaseWnd->FindChildWnd( eID_CLOSE );
	
	m_pWndEtcs = pBaseWnd->FindChildWnd( eID_ETCS );

	ioWnd* pEffectWnd = FindChildWnd(eID_EFFECTS);
	m_pEffectItem[eEffectBefore] = pEffectWnd->GetEffect( "ItemBefore" );
	m_pEffectItem[eEffectAfter] = pBaseWnd->GetEffect( "ItemAfter" );
	m_pEffectItem[eEffectResult] = pEffectWnd->GetEffect( "ItemResult" );
	m_pEffectItem[eEffectSub] = pEffectWnd->GetEffect( "SubEffect" );

	m_pWndInfo1 = pBaseWnd->FindChildWnd( eID_INFO ); 
	m_pWndInfo2 = pBaseWnd->FindChildWnd( eID_INFO2 ); 

	//Bind
 	m_pBtnBuy->SetBindFunction( TR1BINDNA(&PopupStoreWnd::OnBtnBuy,this), IOBN_BTNUP );
	m_pBtnShow->SetBindFunction( TR1BINDNA(&PopupStoreWnd::OnBtnShow,this), IOBN_BTNUP );
	m_pBtnClose->SetBindFunction( TR1BINDNA(&PopupStoreWnd::OnBtnClose,this), IOBN_BTNUP );

	ioButton* pBtn = (ioButton*)m_pWndItem->FindChildWnd( eID_ITEM_IMG );
	pBtn->SetBindFunction( TR1BINDNA(&PopupStoreWnd::OnItemOver,this), IOWN_OVERED );
	pBtn->SetBindFunction( TR1BINDNA(&PopupStoreWnd::OnItemLeave,this), IOWN_LEAVED );
	pBtn->SetCheckParentOutSide(true);

	m_pWndGoldWnd1 = m_pWndGolds->FindChildWnd( eID_GOLD_SUB1 );
	m_pWndGoldWnd2 = m_pWndGolds->FindChildWnd( eID_GOLD_SUB2 );
	m_pWndGoldWnd3 = m_pWndGolds->FindChildWnd( eID_GOLD_SUB3 );
	m_pWndGoldWnd4 = m_pWndGolds->FindChildWnd( eID_GOLD_SUB4 );

	m_pWndEtcWnd1 = m_pWndEtcs->FindChildWnd( eID_ETC_SUB1 );
	m_pWndEtcWnd2 = m_pWndEtcs->FindChildWnd( eID_ETC_SUB2 );
	m_pWndEtcWnd3 = m_pWndEtcs->FindChildWnd( eID_ETC_SUB3 );
	m_pWndEtcWnd4 = m_pWndEtcs->FindChildWnd( eID_ETC_SUB4 );
	m_pWndEtcWnd5 = m_pWndEtcs->FindChildWnd( eID_ETC_SUB5 );
	m_pWndEtcWnd6 = m_pWndEtcs->FindChildWnd( eID_ETC_SUB6 );
	
	
 	m_pWndEtcWnd3->SetBindFunction( TR1BINDNA(&PopupStoreWnd::OnChargeOver,this), IOWN_OVERED );
	m_pWndEtcWnd3->SetBindFunction( TR1BINDNA(&PopupStoreWnd::OnBtnCharge,this), IOWN_BTNUP );	
 	m_pWndEtcWnd3->SetBindFunction( TR1BINDNA(&PopupStoreWnd::OnChargeLeave,this), IOWN_LEAVED );

	m_pWndEtcWnd4->SetBindFunction( TR1BINDNA(&PopupStoreWnd::OnCloseOver,this), IOWN_OVERED );
 	m_pWndEtcWnd4->SetBindFunction( TR1BINDNA(&PopupStoreWnd::OnBtnExit,this), IOWN_BTNUP );
 	m_pWndEtcWnd4->SetBindFunction( TR1BINDNA(&PopupStoreWnd::OnCloseLeave,this), IOWN_LEAVED );

	m_szGold = STR(1);
	m_szRealClose = STR(2);
	m_szRealBuy = STR(3);
	m_szCount = STR(4);
	m_szString1 = STR(5);
	m_szString2 = STR(6);
	m_szBtnNameBuy = STR(7);
	m_szBtnNameCharge = STR(8);
	m_szBtnNameClose = STR(9);
	m_szBtnNameShow = STR(10);
	m_szError = STR(11);

	m_pWndInfo1->SetTitleText( m_szString1.c_str() );
	m_pWndInfo2->SetTitleText( m_szString2.c_str() );	
	m_pBtnBuy->SetTitleText( m_szBtnNameBuy.c_str() );
	m_pWndEtcWnd3->SetTitleText( m_szBtnNameCharge.c_str() );
	m_pWndEtcWnd4->SetTitleText( m_szBtnNameClose.c_str() );
	m_pBtnClose->SetTitleText( m_szBtnNameClose.c_str() );
	m_pBtnShow->SetTitleText( m_szBtnNameShow.c_str() );

	m_szEffectPlaySound = "interface\\ceremony\\star_player_get_00.wav";
	m_szBtnOverSound = "interface\\public_mouseover_00.wav";
	m_szBtnClickSound = "interface\\public_mouseclick_00.wav";
}
void PopupStoreWnd::iwm_hide()
{
	for( int i = 0; i < eEffectMax; ++i )
		m_pEffectItem[i]->Stop();
}

void PopupStoreWnd::OnRender()
{
	ioWnd::OnRender();
}

void PopupStoreWnd::iwm_show()
{
	// 보여질때 윈도우 정렬
	int iWidth = Setting::Width();
	int iHeight = Setting::Height();

	if( iWidth == 0 || iHeight == 0 )
		return;

	int iHalfX = GetWidth() / 2;
	int iHalfY = GetHeight() / 2;

	int XPos = (iWidth/2) - iHalfX;
	int YPos = (iHeight/2) - iHalfY;

	SetWndPos( XPos, YPos );

	m_dwShowStartTime = FRAMEGETTIME();
	
	ioWnd* pBaseWnd = FindChildWnd(eID_BASE);
	pBaseWnd->HideWnd();

	m_pEffectItem[eEffectSub]->Start();
	if( m_bSubPopup )
		g_SoundMgr.PlaySound( "interface\\pet\\compound.wav" );
}

void PopupStoreWnd::OnProcess( float fTimePerSec )
{
	ioWnd::OnProcess(fTimePerSec);

	//DWORD dwShowTime = 3000;

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_bSubPopup && (dwCurTime-m_dwShowStartTime) > 3000 )
	{	
		OnChanceAfter();
	}

	int iCash = g_MyInfo.GetCash();
	OnUpdateMyCash( iCash );
}

int PopupStoreWnd::OnBtnBuy()
{
	// 돈검사 해서
	if( g_MyInfo.GetTotalCash() < m_OnPopupItemInfo.iCash )
	{
		g_GUIMgr.ShowWnd( BILLING_WANT_WND );
		return 1;
	}

	m_iMsgType = eMsgRealBuy;
	g_GUIMgr.SetMsgBox( MB_YESNO, this, m_szRealBuy.c_str() );

	return 1;
}

void PopupStoreWnd::OpenPopup( int iPopupIndex )
{	
	int nSize = m_vecPopupItemInfo.size();
	for( int i = 0; i < nSize; ++i )
	{
		if( m_vecPopupItemInfo[i].iIndex == iPopupIndex )
		{
			m_iPopupIndex = iPopupIndex;
			m_OnPopupItemInfo = m_vecPopupItemInfo[i];

			// 이름, 아이콘 264
			OnUpdatePresentText();
			OnUpdatePresentIconText();

			OnUpdatePresentWnd();
			g_App.SaveOpenPopupIndex( iPopupIndex );

			//여기서 확률적용..
			if( m_OnPopupItemInfo.iChance == 0 )
			{
				SetSubPopup(false);
				ShowWnd();
				OnChanceAfter();
				OnItemLeave();
				return;
			}

			int iRandom = rand()%100;
			if( iRandom < m_OnPopupItemInfo.iChance )
			{
				PopupStoreSubWnd* pPopupStoreSubWnd = dynamic_cast<PopupStoreSubWnd*>(g_GUIMgr.FindWnd( POPUP_STORE_SUB_WND ));
				if( !pPopupStoreSubWnd )
					pPopupStoreSubWnd = dynamic_cast<PopupStoreSubWnd*>(g_GUIMgr.AddWnd( "xml/popupstoreSub.xml" ));
				if( pPopupStoreSubWnd )
					pPopupStoreSubWnd->ShowWnd();
				OnItemLeave();
				break;
			}
		}
	}

	HideWnd();
	OnItemLeave();
}

void PopupStoreWnd::OnUpdatePresentIconText()
{
	int iPresentType = m_OnPopupItemInfo.iPresentType;
	int iPresentValue1 = m_OnPopupItemInfo.iPresentValue1;
	int iPresentValue2 = m_OnPopupItemInfo.iPresentValue2;

	m_szIconName = g_PresentMgr.GetPresentIconText( (short)iPresentType, iPresentValue1, iPresentValue2 );
	m_szSubIconName = g_PresentMgr.GetPresentSubIconText( (short)iPresentType, iPresentValue1, iPresentValue2 );
}

void PopupStoreWnd::OnUpdatePresentText()
{
	ioHashString szReturn;

	int iPresentType = m_OnPopupItemInfo.iPresentType;
	int iPresentValue1 = m_OnPopupItemInfo.iPresentValue1;
	int iPresentValue2 = m_OnPopupItemInfo.iPresentValue2;

	m_szTitle = g_PresentMgr.GetPresentValue1Text( (short)iPresentType, iPresentValue1, iPresentValue2 );
}

void PopupStoreWnd::OnUpdatePresentText2()
{
	ioHashString szReturn;

	int iPresentType = m_OnPopupItemInfo.iPresentType;
	int iPresentValue1 = m_OnPopupItemInfo.iPresentValue1;
	int iPresentValue2 = m_OnPopupItemInfo.iPresentValue2;

	m_szTitle2 = g_PresentMgr.GetPresentValue2Text( (short)iPresentType, iPresentValue1, iPresentValue2 );
}

void PopupStoreWnd::OnUpdatePresentWnd()
{
	// 아이템 이미지
	ioButton* pBtn = (ioButton*)m_pWndItem->FindChildWnd( eID_ITEM_IMG );
	if( pBtn )
	{
		ioUIImage* pIcon = g_UIImageSetMgr.GetImageByFullName( m_szIconName );
		if( pIcon )
			pBtn->ChangeRenderImage( pIcon );
	}

	// 19,800 골드
	char szPrice[MAX_PATH]={0,};
	Help::ConvertNumToStrComma( m_OnPopupItemInfo.iCash, szPrice, sizeof( szPrice ) );
	m_szCashText1 = szPrice;
	m_szCashText1 += m_szGold;

	// 할인!!
	if( m_OnPopupItemInfo.iPrevCash > 0 )
	{
		// 198,00 골드 >> 
		Help::ConvertNumToStrComma( m_OnPopupItemInfo.iPrevCash, szPrice, sizeof( szPrice ) );
		m_szCashText2 = szPrice;
		m_szCashText2 += m_szGold;

		//(50%off)
#ifdef USE_LIST_ARQUMENTS_TEXT_FUNCTION
		sprintf_e( szPrice, "(%d%%%%할인)", m_OnPopupItemInfo.iDisCount );
#else
		sprintf_e( szPrice, "(%d%%off)", m_OnPopupItemInfo.iDisCount );
#endif
		
		m_szCashText3 = szPrice;
	}

	OnUpdateGoldWindow();
	OnPage1();
}

PopupStoreWnd::sPopupItemInfo PopupStoreWnd::GetPopupItemInfo()
{
	return m_OnPopupItemInfo;
}

int PopupStoreWnd::OnItemOver()
{
	ioLSMouse* pMouse = g_App.GetMouse();
	if( pMouse )
		pMouse->SetToolTipWndID( eID_ITEM_IMG );

	return 1;
}

int PopupStoreWnd::OnItemLeave()
{
	ioLSMouse* pMouse = g_App.GetMouse();
	if( pMouse )
	{
		pMouse->SetToolTipWndID( 0 );
		pMouse->SetToolTipCursor();
	}

	return 1;
}

void PopupStoreWnd::OnUpdateGoldWindow()
{
	// 가격, 할인 정보
	if( m_OnPopupItemInfo.iPrevCash > 0 )
		m_bGoldShowType = true;
	else
		m_bGoldShowType = false;

	m_pWndGolds->ShowWnd();
	m_pWndEtcs->ShowWnd();

	if( m_bGoldShowType )
	{
		m_pWndGoldWnd1->ShowWnd();
		m_pWndGoldWnd2->ShowWnd();
		m_pWndGoldWnd4->ShowWnd();

		m_pWndGoldWnd3->SetTitleText( m_szCashText1.c_str() );		
		m_pWndGoldWnd1->SetTitleText( m_szCashText2.c_str() );
		m_pWndGoldWnd4->SetTitleText( m_szCashText3.c_str() );

		int iArray[4] = {0,};
		iArray[0] = g_FontMgr.GetTextWidth( m_szCashText2.c_str(), FONT_SIZE_12 ) + 6;
		iArray[1] = 17+6;
		iArray[2] = g_FontMgr.GetTextWidth( m_szCashText1.c_str(), FONT_SIZE_18 ) + 1;
		iArray[3] = g_FontMgr.GetTextWidth( m_szCashText3.c_str(), FONT_SIZE_18 );

		int iTotal = 0;
		for( int i = 0; i < 4; ++i )
			iTotal += iArray[i];		
		int iCenter = iTotal >> 1;
		iCenter = -iCenter;
		
		int iPosY = m_pWndGoldWnd1->GetPosY();
		m_pWndGoldWnd1->SetWndPos( iCenter, iPosY );		
		iCenter += iArray[0];
		iPosY = m_pWndGoldWnd2->GetPosY();
		m_pWndGoldWnd2->SetWndPos( iCenter, iPosY );
		iCenter += iArray[1];
		m_pWndGoldWnd3->SetWndPos( iCenter, 0 );
		iCenter += iArray[2];
		m_pWndGoldWnd4->SetWndPos( iCenter, 0 );
	}
	else
	{
		m_pWndGoldWnd1->HideWnd();
		m_pWndGoldWnd2->HideWnd();
		m_pWndGoldWnd4->HideWnd();

		m_pWndGolds->SetWndPos( 0, 76 );
		m_pWndGoldWnd3->SetTitleText( m_szCashText1.c_str() );
		int nWidth = g_FontMgr.GetTextWidth( m_szCashText1.c_str(), FONT_SIZE_18 );
		if( nWidth != 0 )
			nWidth = nWidth >> 1;
		m_pWndGoldWnd3->SetWndPos( -nWidth, 0 );
	}

	m_pWndName->SetTitleText( m_szTitle.c_str() );
	m_pWndCount->SetTitleText( m_szTitle2.c_str() );

	// 내골드 업데이트
	int iCash = g_MyInfo.GetCash();
	OnUpdateMyCash( iCash );
}

void PopupStoreWnd::OnUpdateMyCash( int iCash )
{
	if( m_iCash == iCash )
		return;

	m_iCash = iCash;

	
	char szPrice[MAX_PATH]={0,};
	Help::ConvertNumToStrComma( m_iCash, szPrice, sizeof( szPrice ) );
	m_pWndEtcWnd2->SetTitleText( szPrice );
	
	const char* pText1 = m_pWndEtcWnd1->GetTitleText();	
	sprintf( szPrice, "%s ", pText1 );


	int iArray[4] = {0,};
	iArray[0] = g_FontMgr.GetTextWidth( szPrice, FONT_SIZE_12 );
	iArray[1] = g_FontMgr.GetTextWidth( m_pWndEtcWnd2->GetTitleText(), TS_NORMAL, FONT_SIZE_12 ) + 9;
	iArray[2] = g_FontMgr.GetTextWidth( m_pWndEtcWnd3->GetTitleText(), TS_NORMAL, FONT_SIZE_12 ) + 9;
	iArray[3] = g_FontMgr.GetTextWidth( m_pWndEtcWnd4->GetTitleText(), TS_NORMAL, FONT_SIZE_12 );

	// 값이 정확히 안 나오는듯??
	int iHeight1 = g_FontMgr.GetTextHeight( m_pWndEtcWnd3->GetTitleText(), TS_NORMAL, FONT_SIZE_12 ) + 3;
	int iHeight2 = g_FontMgr.GetTextHeight( m_pWndEtcWnd4->GetTitleText(), TS_NORMAL, FONT_SIZE_12 ) + 3;

	m_pWndEtcWnd3->SetSize( iArray[2], iHeight1 );
	m_pWndEtcWnd4->SetSize( iArray[3], iHeight2 );

	int iTotal = 0;
	for( int i = 0; i < 4; ++i )
		iTotal += iArray[i];		
	int iCenter = iTotal >> 1;
	iCenter = -iCenter;
	m_pWndEtcWnd1->SetWndPos( iCenter, 0 );		
	iCenter += iArray[0];
	m_pWndEtcWnd2->SetWndPos( iCenter, 0 );
	iCenter += iArray[1];
	m_pWndEtcWnd5->SetWndPos( iCenter-5, 1 );
	m_pWndEtcWnd3->SetWndPos( iCenter, 0 );
	iCenter += iArray[2];
	m_pWndEtcWnd6->SetWndPos( iCenter-5, 1 );
	m_pWndEtcWnd4->SetWndPos( iCenter, 0 );
}

int PopupStoreWnd::OnChargeOver()
{
	g_SoundMgr.PlaySound( m_szBtnOverSound );
	m_pWndEtcWnd3->SetTitleColor( 0xffffffff, 0xffffffff );

	return 1;
}

int PopupStoreWnd::OnBtnCharge()
{
	g_SoundMgr.PlaySound( m_szBtnClickSound );
	ioLocalParent* pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal )
	{
		pLocal->OpenFillCashPage();
	}

	return 1;
}

int PopupStoreWnd::OnChargeLeave()
{
	m_pWndEtcWnd3->SetTitleColor( 0xffff6c00, 0xffffffff );

	return 1;
}

int PopupStoreWnd::OnCloseOver()
{
	g_SoundMgr.PlaySound( m_szBtnOverSound );
	m_pWndEtcWnd4->SetTitleColor( 0xffffffff, 0xffffffff );

	return 1;
}

int PopupStoreWnd::OnBtnExit()
{
	//HideWnd();

	g_SoundMgr.PlaySound( m_szBtnClickSound );
	m_iMsgType = eMsgClose;
	g_GUIMgr.SetMsgBox( MB_YESNO, this, m_szRealClose.c_str() );

	return 1;
}

int PopupStoreWnd::OnCloseLeave()
{
	m_pWndEtcWnd4->SetTitleColor( 0xff00fcff, 0xffffffff );

	return 1;
}

int PopupStoreWnd::OnBtnShow()
{
	HideWnd();

	//선물함 으로 가기
	MyInventoryWnd* pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
	if( pInvenWnd )
	{
		pInvenWnd->ShowPresentTabDirect();
	}

	return 1;
}

void PopupStoreWnd::OnBuySuccess()
{

#if defined( USE_GA )
	int nSize = m_vecPopupItemInfo.size();
	for( int i = 0; i < nSize; ++i )
	{
		if( m_vecPopupItemInfo[i].iIndex == m_iPopupIndex )
		{
			int iCashPrice	= m_vecPopupItemInfo[i].iCash;
			int iItemCode	= m_vecPopupItemInfo[i].iPresentValue1;

			if( iCashPrice != 0 && iItemCode != 0 )
			{
				// GOLD_USE_POPUP
				g_HttpMng.GA_ItemHitTracking( g_MyInfo.GetUserIndex() 
					, "PopUp"
					, iCashPrice
					, 1
					, iItemCode
					, "Gold"
					, "%2FGOLD%2FUSE%2FPOPUP"
					, true );
			}

			break;
		}
	}
#endif

	
	OnPage2();
}

void PopupStoreWnd::OnPage1()
{
	m_bPage = true;

	m_pWndImgs1->ShowWnd();
	m_pWndImgs2->HideWnd();
	m_pBtnBuy->ShowWnd();
	m_pBtnShow->HideWnd();
	m_pBtnClose->HideWnd();
	m_pEffectItem[eEffectBefore]->Start();
	m_pEffectItem[eEffectAfter]->Start();
	m_pEffectItem[eEffectResult]->Stop();

	m_pWndGolds->ShowWnd();
	m_pWndEtcs->ShowWnd();

 	m_pWndItem->SetWndPos( 0, -2 );
 	m_pWndName->SetWndPos( 0, 52 );
	m_pWndCount->HideWnd();

	m_pWndInfo1->ShowWnd();
	m_pWndInfo2->HideWnd();
}

void PopupStoreWnd::OnPage2()
{
	m_bPage = false;

	m_pWndImgs2->ShowWnd();
	m_pWndImgs1->HideWnd();
	m_pBtnBuy->HideWnd();
	m_pBtnShow->ShowWnd();
	m_pBtnClose->ShowWnd();
	m_pEffectItem[eEffectBefore]->Stop();
	m_pEffectItem[eEffectAfter]->Stop();
	m_pEffectItem[eEffectResult]->Start();

	if( !m_szEffectPlaySound.IsEmpty() )
	{
		g_SoundMgr.PlaySound( m_szEffectPlaySound );
	}

	m_pWndGolds->HideWnd();
	m_pWndEtcs->HideWnd();

 	m_pWndItem->SetWndPos( 0, -27 );
 	m_pWndName->SetWndPos( 0, 22 );
	m_pWndCount->ShowWnd();

	m_pWndInfo2->ShowWnd();
	m_pWndInfo1->HideWnd();
}

void PopupStoreWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	if( pWnd->IsBind() )
	{
		pWnd->UpdateBindFunction( cmd );
	}

	if( pWnd->GetID() == MESSAGE_BOX )
	{
		if( m_iMsgType == eMsgClose )
		{
			if( param == IDYES )
				HideWnd();
		}
		else if( m_iMsgType == eMsgRealBuy )
		{
			if( param == IDYES )
			{
				if( m_iPopupIndex == 0 )
					return;

				TCPNetwork::MouseBusy( true );
				SP2Packet kPacket( CTPK_POPUP_ITEM_BUY );
				kPacket << m_iPopupIndex;
				TCPNetwork::SendToServer( kPacket );
			}
		}
		else if( m_iMsgType == eMsgError )
		{
			HideWnd();
		}
	}
}

int PopupStoreWnd::OnBtnClose()
{
	HideWnd();
	
	return 1;
}
void PopupStoreWnd::OnButFaild( DWORD dwReturnType )
{
	m_iMsgType = eMsgError;

	char szTemp[MAX_PATH]={0,};
	//sprintf( szTemp,"%s", m_szError.c_str(), dwReturnType );
	sprintf_s( szTemp, m_szError.c_str(), dwReturnType );
	g_GUIMgr.SetMsgBox( MB_OK, this, szTemp );
}

bool PopupStoreWnd::iwm_esc()
{
	return true;
}

void PopupStoreWnd::OnChanceAfter()
{
	m_bSubPopup = false;
	ioWnd* pBaseWnd = FindChildWnd(eID_BASE);
	pBaseWnd->ShowWnd();

	m_pEffectItem[eEffectSub]->Stop();

	if( m_bPage )
		OnPage1();
	else
		OnPage2();
}

PopupStoreSubWnd::PopupStoreSubWnd() :
  m_pBtnOpen(NULL)
, m_dwShowStartTime(0)
, m_pEffectReady(NULL)
{
}

PopupStoreSubWnd::~PopupStoreSubWnd()
{
}

void PopupStoreSubWnd::iwm_create()
{
	ioWnd* pBaseWnd = FindChildWnd(eID_BASE);

	m_pBtnOpen = (ioButton*)pBaseWnd->FindChildWnd(eBTN_OPEN);
	m_pBtnOpen->SetBindFunction( TR1BINDNA(&PopupStoreSubWnd::OnBtnOpen,this), IOBN_BTNUP );
	m_pBtnOpen->SetCheckParentOutSide(true);

	m_pEffectReady = pBaseWnd->GetEffect("Ready");	
}

int PopupStoreSubWnd::OnBtnOpen()
{
	EraseInModalList();
	HideWnd();

	PopupStoreWnd* pPopupStoreWnd = dynamic_cast<PopupStoreWnd*>( g_GUIMgr.FindWnd( POPUP_STORE_WND ) );
	if( pPopupStoreWnd )
	{
		pPopupStoreWnd->SetSubPopup( true );
		pPopupStoreWnd->ShowWnd();
	}

	return 1;
}

void PopupStoreSubWnd::iwm_show()
{
	// 보여질때 윈도우 정렬
	int iWidth = Setting::Width();
	int iHeight = Setting::Height();

	if( iWidth == 0 || iHeight == 0 )
		return;

	int iHalfX = GetWidth() / 2;
	int iHalfY = GetHeight() / 2;

	int XPos = (iWidth/2) - iHalfX;
	int YPos = (iHeight/2) - iHalfY;

	SetWndPos( XPos, YPos );

	m_pEffectReady->Start();
}

void PopupStoreSubWnd::iwm_hide()
{
	m_pEffectReady->Stop();
}
