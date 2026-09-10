#include "StdAfx.h"

#include "../io3DEngine/ioWnd.h"

#include "../Housing/ioUserBlockStorage.h"
#include "../Housing/ioBlockManager.h"
#include "../Housing/ioBlockProperty.h"
#include "../Housing/ioBlockSetupManager.h"

#include "ioHousingBlockInvenWnd.h"

#include "../ioLSMouse.h"
#include "../ioPlayStage.h"

ioHousingBlockInvenIcon::ioHousingBlockInvenIcon()
{
	m_pIcon		= NULL;
	m_pIconBack	= NULL;
}

ioHousingBlockInvenIcon::~ioHousingBlockInvenIcon()
{
	SAFEDELETE( m_pIconBack );
	SAFEDELETE( m_pIcon );
}

void ioHousingBlockInvenIcon::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "IconBack" )
	{
		SAFEDELETE( m_pIconBack );
		m_pIconBack = pImage;
	}
	else
	{
		ioButton::AddRenderImage( szType, pImage );
	}
}

void ioHousingBlockInvenIcon::SetIcon( const ioHashString& szName )
{
	SAFEDELETE( m_pIcon );
	m_pIcon = g_UIImageSetMgr.CreateImageByFullName( szName.c_str() );

	if( m_pIconBack )
		m_pIconBack->SetScale( 0.75f, 0.75f );

	if( m_pIcon )
		m_pIcon->SetScale( 0.75f, 0.75f );
}

void ioHousingBlockInvenIcon::OnDrawNormal( int iXPos, int iYPos )
{
	ioButton::OnDrawNormal( iXPos, iYPos );

	if( m_pIconBack )
		m_pIconBack->Render( iXPos + GetWidth() / 2, iYPos + GetHeight() / 2, UI_RENDER_MULTIPLY, TFO_BILINEAR );	

	if( m_pIcon )
		m_pIcon->Render( iXPos + GetWidth() / 2, iYPos + GetHeight() / 2, UI_RENDER_NORMAL, TFO_BILINEAR );
}

void ioHousingBlockInvenIcon::OnDrawOvered( int iXPos, int iYPos )
{
	if( HasWndStyle( IWS_INACTIVE ) )
	{
		ioButton::OnDrawNormal( iXPos, iYPos );

		if( m_pIconBack )
			m_pIconBack->Render( iXPos + GetWidth() / 2, iYPos + GetHeight() / 2, UI_RENDER_MULTIPLY, TFO_BILINEAR );

		if( m_pIcon )
			m_pIcon->Render( iXPos + GetWidth() / 2, iYPos + GetHeight() / 2, UI_RENDER_GRAY, TFO_BILINEAR );
	}
	else
	{
		ioButton::OnDrawOvered( iXPos, iYPos );
		if( m_pIconBack )
			m_pIconBack->Render( iXPos + GetWidth() / 2, iYPos + GetHeight() / 2, UI_RENDER_MULTIPLY, TFO_BILINEAR );

		if( m_pIcon )
		{
			m_pIcon->Render( iXPos + GetWidth() / 2, iYPos + GetHeight() / 2, UI_RENDER_NORMAL, TFO_BILINEAR );		
			m_pIcon->Render( iXPos + GetWidth() / 2, iYPos + GetHeight() / 2, UI_RENDER_SCREEN, TFO_BILINEAR );
		}
	}
}

void ioHousingBlockInvenIcon::OnDrawPushed( int iXPos, int iYPos )
{
	if( HasWndStyle( IWS_INACTIVE ) )
	{
		ioButton::OnDrawNormal( iXPos, iYPos );

		if( m_pIconBack )
			m_pIconBack->Render( iXPos + GetWidth() / 2, iYPos + GetHeight() / 2, UI_RENDER_MULTIPLY, TFO_BILINEAR );

		if( m_pIcon )
			m_pIcon->Render( iXPos + GetWidth() / 2, iYPos + GetHeight() / 2, UI_RENDER_GRAY, TFO_BILINEAR );
	}
	else
	{
		ioButton::OnDrawOvered( iXPos, iYPos );

		if( m_pIconBack )
			m_pIconBack->Render( iXPos + GetWidth() / 2, iYPos + GetHeight() / 2, UI_RENDER_MULTIPLY, TFO_BILINEAR );

		if( m_pIcon )
		{
			m_pIcon->Render( iXPos + GetWidth() / 2, iYPos + GetHeight() / 2, UI_RENDER_NORMAL, TFO_BILINEAR );		
			m_pIcon->Render( iXPos + GetWidth() / 2, iYPos + GetHeight() / 2, UI_RENDER_SCREEN, TFO_BILINEAR );
		}
	}
}

void ioHousingBlockInvenIcon::OnDrawDisabled( int iXPos, int iYPos )
{
	ioButton::OnDrawDisabled( iXPos, iYPos );

	if( m_pIconBack )
		m_pIconBack->Render( iXPos + GetWidth() / 2, iYPos + GetHeight() / 2, UI_RENDER_MULTIPLY, TFO_BILINEAR );

	if( m_pIcon )
		m_pIcon->Render( iXPos + GetWidth() / 2, iYPos + GetHeight() / 2, UI_RENDER_GRAY, TFO_BILINEAR );
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ioHousingBlockInvenItemButton::ioHousingBlockInvenItemButton() : m_nBlockCode(0), m_nEtcCode(0), m_nCount(0), m_bGray(false)
{
}

ioHousingBlockInvenItemButton::~ioHousingBlockInvenItemButton()
{
}

void ioHousingBlockInvenItemButton::iwm_show()
{
}

void ioHousingBlockInvenItemButton::iwm_hide()
{	
}

void ioHousingBlockInvenItemButton::SetInfo( const ioHashString& szName, const ioHashString& szIcon, int nBlockCode, int nEtcCode, int nCount )
{
	m_szName		= szName;
	m_szIcon		= szIcon;
	m_nBlockCode	= nBlockCode;
	m_nEtcCode		= nEtcCode;
	m_nCount		= nCount;

	if( nBlockCode == 0 || nEtcCode == 0 || m_szName.IsEmpty() || m_szIcon.IsEmpty() )
	{
		HideChildWnd( ID_ICON );
		SetInActive();
	}
	else
	{
		ShowChildWnd( ID_ICON );
		SetActive();
	}

	if( m_nCount == 0 )
		m_bGray = true;
	else
		m_bGray = false;

	ioHousingBlockInvenIcon* pIcon = dynamic_cast<ioHousingBlockInvenIcon*>( FindChildWnd( ID_ICON ) );
	if( pIcon )
	{
		if( m_bGray )
			pIcon->SetInActive();
		else
			pIcon->SetActive();

		pIcon->SetIcon( szIcon );
	}

	ShowWnd();
}

bool ioHousingBlockInvenItemButton::IsOver( ioLSMouse *pMouse )
{
	ioHousingBlockInvenIcon* pIcon = dynamic_cast<ioHousingBlockInvenIcon*>( FindChildWnd( ID_ICON ) );
	if( pIcon && pMouse && pIcon->IsInWndRect( pMouse->GetMousePos() ) )
		return true;
	
	return false;
}

void ioHousingBlockInvenItemButton::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;
}

void ioHousingBlockInvenItemButton::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_ICON:
		if( cmd == IOBN_BTNUP || cmd == IOWN_OVERED || cmd == IOWN_LEAVED )
		{
			if( GetParent() )
				GetParent()->iwm_command( this, cmd, dwID );
		}
		break;
	}
}

void ioHousingBlockInvenItemButton::OnRender()
{
	ioButton::OnRender();

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	if( HasWndStyle( IWS_INACTIVE ) )
		return;

	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );

	if( m_bGray )
	{
		g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
		g_FontMgr.PrintTextWidthCut( iXPos + 42, iYPos + 73, FONT_SIZE_12, 82, " %s", m_szName.c_str() );
	}
	else
	{
		g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		g_FontMgr.PrintTextWidthCut( iXPos + 42, iYPos + 73, FONT_SIZE_12, 82, " %s", m_szName.c_str() );
	}

	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );	

	char szNum[MAX_PATH];
	Help::ConvertNumToStrComma( m_nCount, szNum, MAX_PATH );

	if( m_bGray )
	{
		g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
		g_FontMgr.PrintText( iXPos + 42, iYPos + 89, FONT_SIZE_12, " %s개", szNum );
	}
	else
	{
		g_FontMgr.SetTextColor( TCT_DEFAULT_RED );
		g_FontMgr.PrintText( iXPos + 42, iYPos + 89, FONT_SIZE_12, " %s개", szNum );
	}
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ioHousingBlockInvenWnd::ioHousingBlockInvenWnd()
{
	m_pPreEdit			= NULL;
	m_eBlockModeType	= 0;
}

ioHousingBlockInvenWnd::~ioHousingBlockInvenWnd()
{
}

ioUserBlockStorage* ioHousingBlockInvenWnd::GetStorage()
{
	ioUserBlockStorage* pStorage = NULL;
	switch( m_eBlockModeType )
	{
	case BMT_GUILD:
		pStorage = g_App.GetGuildStorage();
		break;
	case BMT_PERSONAL:
		pStorage = g_App.GetHomeStorage();
		break;
	}

	return pStorage;
}

void ioHousingBlockInvenWnd::iwm_show()
{
	g_GUIMgr.HideWnd( NEW_SHOP_WND );

	ioPlayStage* pStage = g_GUIMgr.GetPlayStage();
	if( pStage->HasCameraFixedStyle( CAMERA_STYLE_HOUSING ) )
		pStage->ClearCameraModeFixed( CAMERA_STYLE_HOUSING );	

	m_nCurrPage		= 0;
	m_nCurrnItemMax	= 0;

	UpdateTab( ID_TAB01 );
	ParseButton();
	ParseScroll();

	ioUserBlockStorage* pStorage = GetStorage();
	if( pStorage )
		pStorage->SendBlockInvenSync();

	EnableEditFocus();
	switch( m_eBlockModeType )
	{
	case BMT_GUILD:
		SetTitleText( "길드본부 꾸미기" );
		break;
	case BMT_PERSONAL:
		SetTitleText( "개인본부 꾸미기" );
		break;
	}
}

void ioHousingBlockInvenWnd::iwm_hide()
{
	g_GUIMgr.HideWnd( HOUSING_BLOCK_INVEN_TOOLTIP );
	DisableEditFocus();
}

bool ioHousingBlockInvenWnd::iwm_esc()
{
	HideWnd();
	return true;
}

void ioHousingBlockInvenWnd::EnableEditFocus()
{
	ioEdit *pEdit = dynamic_cast<ioEdit*>( FindChildWnd( ID_EDITBOX ) );
	if( pEdit && pEdit->IsShow() )
	{
		if( m_pPreEdit != pEdit )
			m_pPreEdit = ioEdit::m_pKeyFocusEdit;

		pEdit->ClearString();
		pEdit->SetKeyFocus();
		g_InputBox.SetString( "" );
		m_KeyWord.Clear();
	}
}

void ioHousingBlockInvenWnd::DisableEditFocus()
{
	ioEdit* pEdit = dynamic_cast<ioEdit*>( m_pPreEdit );
	if( pEdit )	
	{
		ioWnd *pParent = pEdit->GetParent();
		if( pParent )
		{
			if( pParent->IsShow() )
			{
				pEdit->SetKeyFocus();
			}
		}
	}
	m_pPreEdit = NULL;
	m_KeyWord.Clear();
}

bool ioHousingBlockInvenWnd::IsKeyFocus()
{
	ioEdit *pEdit = dynamic_cast<ioEdit*>( FindChildWnd( ID_EDITBOX ) );
	if( pEdit && pEdit->IsShow() && pEdit->IsKeyFocus() )
	{
		return true;
	}

	return false;
}

void ioHousingBlockInvenWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	//동기화 패킷을 받은 후 정보가 갱신되도록 OnProcess에서 처리
	ProcessEdit();
	ParseButton();
	ProcessToolTip();	
}

void ioHousingBlockInvenWnd::ParseButton()
{
	if( m_nCurrPage > GetMaxPage() )
		m_nCurrPage = GetMaxPage();

	ioUserBlockStorage* pStorage = GetStorage();
	if( pStorage == NULL )
		return;

	m_nCurrnItemMax = 0;
	ioUserBlockStorage::InvenBlockItemList List;
	for( int i = 0; i < pStorage->GetInvenItemCount(); ++i )
	{
		ioUserBlockStorage::InvenBlockItem* pItem = pStorage->GetInvenBlockItem( i, true );
		if( !pItem )
			continue;
		
		if( m_eCategory != ioBlockProperty::IC_NONE && m_eCategory != pItem->m_nItemCategory )
			continue;

		//소문자 변환
		if( !pItem->m_szName.IsEmpty() && !m_KeyWord.IsEmpty() )
		{
			std::string szName = pItem->m_szName.c_str();
			std::transform( szName.begin(), szName.end(), szName.begin(), tolower );

			ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
			if( pLocal && pLocal->GetType() == ioLocalManager::LCT_KOREA )
			{
				//조합형 글자 비교(초성 비교 초함)
				if( !Help::Strstr( szName.c_str(), m_KeyWord.c_str() ) )
					continue;
			}
			else
			{
				if( strstr( szName.c_str(), m_KeyWord.c_str() ) == NULL )
					continue;
			}
		}

		//필터 및 검색어 기준으로 아이템을 걸러내고 현재 조건에 맞는 맥스값을 구함
		m_nCurrnItemMax++;

		if( m_nCurrnItemMax < m_nCurrPage * PAGE_ELEMENT_COUNT )
			continue;

		List.push_back( *pItem );
	}

	for( int nBtn = ID_ITEM101; nBtn < ID_ITEM_MAX; ++nBtn )
	{
		ioHousingBlockInvenItemButton* pButton = dynamic_cast<ioHousingBlockInvenItemButton*>( FindChildWnd( nBtn ) );
		if( pButton )
		{
			int nIndex = nBtn - ID_ITEM101;
			if( COMPARE( nIndex, 0, (int)List.size() ) )
			{
				pButton->SetInfo( List[nIndex].m_szName, List[nIndex].m_szIcon, List[nIndex].m_nBlockCode, List[nIndex].m_nEtcCode, List[nIndex].m_nCount );
			}
			else
			{
				pButton->SetInfo( "", "", 0, 0, 0 );
			}
		}
	}
}

void ioHousingBlockInvenWnd::ParseScroll()
{
	ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_SCROLL );
	if( pScroll )
	{
		int nMax =  m_nCurrnItemMax / PAGE_ELEMENT_COUNT;
		if( m_nCurrnItemMax % PAGE_ELEMENT_COUNT )
			nMax++;

		pScroll->SetScrollRange( 0, nMax );
		pScroll->SetScrollPos( m_nCurrPage );
	}
}

void ioHousingBlockInvenWnd::ProcessToolTip()
{
	ioLSMouse *pMouse = g_App.GetMouse();
	if( pMouse )
	{
		pMouse->SetToolTipWndID( 0 );

		for(int i = ID_ITEM101; i < ID_ITEM_MAX ; ++i )
		{
			ioHousingBlockInvenItemButton *pButton = dynamic_cast<ioHousingBlockInvenItemButton*> ( FindChildWnd( i ) );
			if( pButton && pButton->IsOver( pMouse ) )
			{
				pMouse->SetToolTipWndID( pButton->GetCode() );
			}
		}
	}
}

void ioHousingBlockInvenWnd::ProcessEdit()
{
	ioEdit* pEdit = dynamic_cast<ioEdit*>( FindChildWnd( ID_EDITBOX ) );
	if( pEdit && pEdit->IsKeyFocus() )
	{
		if( g_InputBox.IsEnter() )
		{
			m_KeyWord	= g_InputBox.GetString();
			m_nCurrPage	= 0;
			iwm_command( FindChildWnd( ID_EDIT_OK ), IOBN_BTNUP, 0 );
		}
	}
}

int	ioHousingBlockInvenWnd::GetMaxPage()
{
	ioUserBlockStorage* pStorage = GetStorage();	
	if( pStorage == NULL )
		return 0;

	return pStorage->GetInvenItemCount() / PAGE_ELEMENT_COUNT;
}

void ioHousingBlockInvenWnd::UpdateTab( DWORD dwID )
{
	m_dwCurrTab = dwID;
	CheckRadioButton( ID_TAB01, ID_TAB05, m_dwCurrTab );
	switch( m_dwCurrTab )
	{
	case ID_TAB01:
		m_eCategory = ioBlockProperty::IC_NONE;
		break;
	case ID_TAB02:
		m_eCategory = ioBlockProperty::IC_BLOCK;
		break;
	case ID_TAB03:
		m_eCategory = ioBlockProperty::IC_TILE;
		break;
	case ID_TAB04:
		m_eCategory = ioBlockProperty::IC_DECO;
		break;
	case ID_TAB05:
		m_eCategory = ioBlockProperty::IC_FUNCTION;
		break;
	}
}

void ioHousingBlockInvenWnd::iwm_wheel( int zDelta )
{
	if( !IsShow() )
	{
		ioWnd::iwm_wheel( zDelta );
	}
	else
	{
		if( g_App.IsMouseBusy() )
			return;

		ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_SCROLL );
		if( pScroll == NULL )
			return;

		if( !pScroll->IsShow() )
			return;

		if( zDelta == WHEEL_DELTA )
			pScroll->SetScrollPos( m_nCurrPage - 1 );
		else if( zDelta == -WHEEL_DELTA )
			pScroll->SetScrollPos( m_nCurrPage + 1 );
	}
}

void ioHousingBlockInvenWnd::iwm_vscroll( DWORD dwID, int curPos )
{
	ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_SCROLL );
	if( pScroll == NULL )
		return;

	if( !pScroll->IsShow() )
		return;

	m_nCurrPage = curPos;
}

void ioHousingBlockInvenWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;
	case ID_SHOP:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();

			NewShopWnd *pShopWnd = dynamic_cast<NewShopWnd*>( g_GUIMgr.FindWnd(NEW_SHOP_WND) );
			if( pShopWnd )
				pShopWnd->ShowEtcItemTab();

			if( ioBlockSetupManager::GetSingletonPtr() )
				g_BlockSetupManager.EndSetUpState();
		}
		break;
	case ID_TAB01:
	case ID_TAB02:
	case ID_TAB03:
	case ID_TAB04:
	case ID_TAB05:
		if( cmd == IOBN_BTNUP )
		{
			EnableEditFocus();
			UpdateTab( dwID );
			ParseButton();
			ParseScroll();
		}
		break;
	case ID_EDIT_OK:
		{
			if( cmd == IOBN_BTNUP )
			{
				m_KeyWord = g_InputBox.GetString();
				UpdateTab( m_dwCurrTab );
				ParseButton();
				ParseScroll();
			}
		}
		break;
	}

	if( COMPARE( dwID, ID_ITEM101, ID_ITEM_MAX ) )
	{
		if( cmd == IOBN_BTNUP )
		{
			ioHousingBlockInvenItemButton* pItem = dynamic_cast<ioHousingBlockInvenItemButton*>( pWnd );
			if( pItem )
			{
				if( 0 < pItem->GetCount() )
				{
					ioUserBlockStorage* pStorage = GetStorage();
					if( pStorage )
						pStorage->SelectItem( pItem->GetCode() );

					if( ioBlockSetupManager::GetSingletonPtr() )
						g_BlockSetupManager.ChangeToSetUp();

					HideWnd();
				}
				else
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, "갯수가 0개인 아이템은 # 사용 할 수 없습니다." );
				}
			}
		}
	}
}

void ioHousingBlockInvenWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ioHousingBlockInvenToolTip::ioHousingBlockInvenToolTip() : m_nManualIndex( 0 )
{
}

ioHousingBlockInvenToolTip::~ioHousingBlockInvenToolTip()
{
}

void ioHousingBlockInvenToolTip::iwm_show()
{
}

void ioHousingBlockInvenToolTip::iwm_hide()
{
}

void ioHousingBlockInvenToolTip::SetBlockInvenItemInfo( int nBlockCode )
{
	m_nBlockCode = nBlockCode;

	ioUserBlockStorage* pStorage = NULL;
	ioHousingBlockInvenWnd* pWnd = dynamic_cast<ioHousingBlockInvenWnd*>( g_GUIMgr.FindWnd( HOUSING_BLOCK_INVEN_WND ) );
	if( pWnd && pWnd->IsShow() )
	{
		pStorage = pWnd->GetStorage();
	}
	
	if( pStorage )
	{
		ioUserBlockStorage::InvenBlockItem* pItem = pStorage->GetInvenBlockItemByCode( nBlockCode, true );
		if( pItem )
		{
			m_Name			= pItem->m_szName;
			m_nManualIndex	= pItem->m_nManualIdx;

			char szNum[MAX_PATH];
			Help::ConvertNumToStrComma( pItem->m_nCount, szNum, MAX_PATH );
			m_Count = szNum;

			InventoryIconBtn *pIcon = dynamic_cast<InventoryIconBtn*>( FindChildWnd( ID_ICON ) );
			if( pIcon )
			{
				pIcon->SetIcon( g_UIImageSetMgr.CreateImageByFullName( pItem->m_szIcon ), FLOAT1, FLOAT1 );
				if( pItem->m_nCount == 0 )			
					pIcon->SetInActive();
				else
					pIcon->SetActive();
			}

			UpdateWndSize();
			ShowWnd();
		}
	}
}

void ioHousingBlockInvenToolTip::OnProcess( float fTimePerSec )
{
	ioLSMouse *pMouse = g_App.GetMouse();
	if( pMouse )
	{
		if( IsShow() )
		{
			int nCode = pMouse->GetToolTipWndID();
			if( nCode > 0 )
			{
				pMouse->SetToolTipWndID( 0 );
				pMouse->SetToolTipCursor();
				int iXPos = 0;
				int iYPos = 0;
				Help::GetNotCutWndPos( pMouse->GetMousePos().x, pMouse->GetMousePos().y, GetWidth(), GetHeight(), iXPos, iYPos );
				SetWndPos( iXPos, iYPos );

				SetBlockInvenItemInfo( nCode );
			}
			else
			{
				HideWnd();
			}
		}
		else
		{
			int nCode = pMouse->GetToolTipWndID();
			if( nCode > 0 )
			{
				int iXPos = 0;
				int iYPos = 0;
				Help::GetNotCutWndPos( pMouse->GetMousePos().x, pMouse->GetMousePos().y, GetWidth(), GetHeight(), iXPos, iYPos );
				SetWndPos( iXPos, iYPos );

				SetBlockInvenItemInfo( nCode );
			}
		}
	}
}

void ioHousingBlockInvenToolTip::UpdateWndSize()
{
	ManualMgr *pManual = ioEtcInvenManualMgr::GetSingletonPtr();
	if( !pManual )
		return;

	bool bAutoLine = false;
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal && pLocal->IsTooltipAutoLine() )
		bAutoLine = true;

	if( !bAutoLine )
	{
		int iMaxLine = pManual->GetMaxLine( m_nManualIndex );

		 // 1번째는 타이틀이므로 삭제
		iMaxLine--;
		if( iMaxLine <= 0 )
			iMaxLine = 0;

		SetSize( GetWidth(), 123 + 18 * iMaxLine );
	}
	else
	{
		int iMaxLine = pManual->GetMaxLine( m_nManualIndex );

		// 1번째는 타이틀이므로 삭제
		int iNewLine = iMaxLine - 1; 
		if( iNewLine <= 0 )
			iNewLine = 0;

		ioComplexStringPrinter kPrinter;
		int iTotalLineGap = 18;
		if( iNewLine > 0 )
		{
			for(int iLine = 1; iLine < iMaxLine; iLine++)
			{
				int iGap = pManual->GetSpaceLine( m_nManualIndex, iLine );				
				bool bFirstGap = true;
				for (int i = 0; i < 20 ; i++)
				{
					if( kPrinter.GetFullWidth() < DESC_WIDTH )
					{
						if( bFirstGap )
						{
							iTotalLineGap += pManual->GetSpaceLine( m_nManualIndex, iLine);
							bFirstGap = false;
						}
						else
							iTotalLineGap += 18;
						kPrinter.ClearList();
						break;
					}

					ioComplexStringPrinter kPrinterA, kPrinterB;
					kPrinter.SplitFullText( kPrinterA, kPrinterB, DESC_WIDTH );
					if( bFirstGap )
					{
						iTotalLineGap += pManual->GetSpaceLine( m_nManualIndex, iLine);
						bFirstGap = false;
					}
					else
					{
						iTotalLineGap += 18;
					}

					kPrinterB.CopyFullText( kPrinter );
					kPrinterA.ClearList();
					kPrinterB.ClearList();
				}
			}
		}

		SetSize( GetWidth(), 123 + iTotalLineGap );
	}
}

void ioHousingBlockInvenToolTip::OnRender()
{
	ioWnd::OnRender();

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	RenderDesc( iXPos, iYPos );
	PrintManual( iXPos + 14, iYPos + 99, FONT_SIZE_12 );
}

void ioHousingBlockInvenToolTip::RenderDesc( int iXPos, int iYPos )
{
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintTextWidthCut( iXPos + 90, iYPos + 32, FONT_SIZE_17, 130.0f, "%s", m_Name.c_str() );

	g_FontMgr.SetTextColor( TCT_DEFAULT_RED );
	g_FontMgr.PrintTextWidthCut( iXPos + 90, iYPos + 53, FONT_SIZE_13, 130.0f, "%s개", m_Count.c_str() );
}

void ioHousingBlockInvenToolTip::PrintManual( int iXPos, int iYPos, float fScale )
{
	ManualMgr* pManual = ioEtcInvenManualMgr::GetSingletonPtr();
	if( !pManual )
		return;

	bool bAutoLine = false;
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal && pLocal->IsTooltipAutoLine() )
		bAutoLine = true;


	ioComplexStringPrinter kPrinter;
	int iMaxLine = pManual->GetMaxLine( m_nManualIndex );
	for(int iLine = 1; iLine < iMaxLine; iLine++ )
	{
		if( !bAutoLine )
		{
			pManual->SetPrinter( m_nManualIndex, iLine, fScale, kPrinter);
			iYPos += pManual->GetSpaceLine( m_nManualIndex, iLine);
			kPrinter.PrintFullTextWidthCut( iXPos, iYPos, TAT_LEFT, DESC_WIDTH );
			kPrinter.ClearList();
		}
		else
		{
			pManual->SetPrinter( m_nManualIndex, iLine, fScale, kPrinter);

			bool bFirstGap = true;
			for( int i = 0; i < 20 ; ++i )
			{
				if( kPrinter.GetFullWidth() < DESC_WIDTH )
				{
					if( bFirstGap )
					{
						iYPos += pManual->GetSpaceLine( m_nManualIndex, iLine);
						bFirstGap = false;
					}
					else
						iYPos += 18;
					kPrinter.PrintFullText( iXPos, iYPos, TAT_LEFT );
					kPrinter.ClearList();
					break;
				}

				ioComplexStringPrinter kPrinterA, kPrinterB;
				kPrinter.SplitFullText( kPrinterA, kPrinterB, DESC_WIDTH );
				if( bFirstGap )
				{
					iYPos += pManual->GetSpaceLine( m_nManualIndex, iLine);
					bFirstGap = false;
				}
				else
					iYPos += 18;
				kPrinterA.PrintFullText( iXPos, iYPos, TAT_LEFT );
				kPrinterB.CopyFullText( kPrinter );
				kPrinterA.ClearList();
				kPrinterB.ClearList();
			}
		}
	}	
}
