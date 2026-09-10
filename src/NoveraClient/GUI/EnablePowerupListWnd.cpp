#include "StdAfx.h"
#include "EnablePowerupListWnd.h"
#include "../ioPowerUpManager.h"

//////////////////////////////////////////////////////////////////////////
EnablePowerupListIconBtn::EnablePowerupListIconBtn()
{
	m_iItemCode = 0;
}

EnablePowerupListIconBtn::~EnablePowerupListIconBtn()
{
	m_szNameList.clear();
}

void EnablePowerupListIconBtn::OnRender()
{
	if ( m_szNameList.empty() )
		return;
	NewShopIconBtn::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetAlignType( TAT_CENTER );

	if ( m_iItemCode > 0 )
		g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	else
		g_FontMgr.SetTextColor( TCT_DEFAULT_LIGHTGRAY );

	enum { X_TEXT_OFFSET = 32, Y_TEXT_OFFSET = 79, Y_OFFSET_LING_GAP = 18, Y_TEXT_OFFSET_ADD = -8, Y_OFFSET_LING_GAP_ADD = -4,  };

	int iYAdd    = 0;
	int iYGapAdd = 0;
	int iSize = m_szNameList.size();
	if( iSize >= 2 )
	{
		iYAdd    = Y_TEXT_OFFSET_ADD;
		iYGapAdd = Y_OFFSET_LING_GAP_ADD; 
	}

	for(int i = 0;i < iSize;i++)
	{
		g_FontMgr.PrintTextWidthCut( iXPos+X_TEXT_OFFSET, iYPos+Y_TEXT_OFFSET+iYAdd+( i*(Y_OFFSET_LING_GAP+iYGapAdd) ), FONT_SIZE_13, 104.0f, m_szNameList[i].c_str() );
	}
}

void EnablePowerupListIconBtn::SetName( const ioHashString &rName )
{
	SplitDescription( rName );
}

void EnablePowerupListIconBtn::SplitDescription( const ioHashString &szName )
{
	m_szNameList.clear();
	m_szNameList = ioStringConverter::Split( szName.c_str(), "#" );
}
//////////////////////////////////////////////////////////////////////////
EnablePowerupListWnd::EnablePowerupListWnd()
{
	m_iMaxPage = 0;
	m_iCurPage = 0;

	m_dwCurSubBtn = ID_HERO_BTN;

	m_vCurList.clear();

	for ( int i= 0 ; i<MAX_TEXT ; ++i )
		m_UnderLinePrinter[i].ClearList();

	m_PagePrinter.ClearList();
}

EnablePowerupListWnd::~EnablePowerupListWnd()
{
}

void EnablePowerupListWnd::ShowItemList()
{
	ShowWnd();

	ChangeSubBtn( ID_HERO_BTN );
}

void EnablePowerupListWnd::SettingText()
{
	for ( int i= 0 ; i<MAX_TEXT ; ++i )
		m_UnderLinePrinter[i].ClearList();

	m_PagePrinter.ClearList();

	for ( int i= 0 ; i<MAX_TEXT ; ++i )
	{
		m_UnderLinePrinter[i].SetTextStyle( TS_NORMAL);
		m_UnderLinePrinter[i].SetBkColor( 0, 0, 0 );
	}

	if( m_dwCurSubBtn == ID_HERO_BTN )
	{
		m_UnderLinePrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY ); 
		m_UnderLinePrinter[0].AddTextPiece( FONT_SIZE_13, STR(1) );

		m_UnderLinePrinter[0].SetTextColor( TCT_DEFAULT_RED ); 
		m_UnderLinePrinter[0].AddTextPiece( FONT_SIZE_13, STR(2) );

		m_UnderLinePrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY ); 
		m_UnderLinePrinter[0].AddTextPiece( FONT_SIZE_13, STR(3) );

		m_UnderLinePrinter[1].SetTextColor( TCT_DEFAULT_DARKGRAY ); 
		m_UnderLinePrinter[1].AddTextPiece( FONT_SIZE_13, STR(4) );
	}
	else if( m_dwCurSubBtn == ID_WEAPON_BTN )
	{
		m_UnderLinePrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY ); 
		m_UnderLinePrinter[0].AddTextPiece( FONT_SIZE_13, STR(5) );

		m_UnderLinePrinter[0].SetTextColor( TCT_DEFAULT_RED ); 
		m_UnderLinePrinter[0].AddTextPiece( FONT_SIZE_13, STR(6) );

		m_UnderLinePrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY ); 
		m_UnderLinePrinter[0].AddTextPiece( FONT_SIZE_13, STR(7) );

		m_UnderLinePrinter[1].SetTextColor( TCT_DEFAULT_DARKGRAY ); 
		m_UnderLinePrinter[1].AddTextPiece( FONT_SIZE_13, STR(8) );
	}
	else
	{
		m_UnderLinePrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY ); 
		m_UnderLinePrinter[0].AddTextPiece( FONT_SIZE_13, STR(9) );

		m_UnderLinePrinter[0].SetTextColor( TCT_DEFAULT_RED ); 
		m_UnderLinePrinter[0].AddTextPiece( FONT_SIZE_13, STR(10) );

		m_UnderLinePrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY ); 
		m_UnderLinePrinter[0].AddTextPiece( FONT_SIZE_13, STR(11) );

		m_UnderLinePrinter[1].SetTextColor( TCT_DEFAULT_DARKGRAY ); 
		m_UnderLinePrinter[1].AddTextPiece( FONT_SIZE_13, STR(12) );
	}

	m_PagePrinter.SetTextStyle( TS_NORMAL);
	m_PagePrinter.SetBkColor( 0, 0, 0 );
	m_PagePrinter.SetTextColor( TCT_DEFAULT_DARKGRAY ); 
	m_PagePrinter.AddTextPiece( FONT_SIZE_13, m_szPageText.c_str() );
}

void EnablePowerupListWnd::ChangeSubBtn( DWORD dwSubBtn )
{
	m_dwCurSubBtn = dwSubBtn;
	CheckRadioButton( ID_HERO_BTN, ID_CLOAK_BTN, m_dwCurSubBtn );
	
	if ( m_dwCurSubBtn == ID_HERO_BTN || m_dwCurSubBtn == ID_WEAPON_BTN )
	{
		const IntVec EnablePowerUpClassNumVec = g_PowerUpManager.GetEnablePowerUpClassNumberInfo();
		if ( EnablePowerUpClassNumVec.empty() )
			return;

		SetItemList( EnablePowerUpClassNumVec, m_vCurList );
	}
	else
	{
		int nSlot = m_dwCurSubBtn - ID_HERO_BTN - 1;
		const IntVec EnablePowerUpExtraItemNumVec = g_PowerUpManager.GetEnablePowerUpExtraItemNumberInfo( nSlot );
		if ( !EnablePowerUpExtraItemNumVec.empty() )
			SetItemList( EnablePowerUpExtraItemNumVec, m_vCurList );
		else
			m_vCurList.clear();
	}
		
	int iTotalCnt = m_vCurList.size();
	m_iMaxPage = iTotalCnt / ICON_BTN_MAX;

	if( iTotalCnt%ICON_BTN_MAX > 0 )
		m_iMaxPage += 1;

	if ( m_iMaxPage == 0 )
		m_iMaxPage = 1;

	UpdatePage( 0 );
	SettingText();
}

void EnablePowerupListWnd::SetItemList( const IntVec EnablePowerUpNumVec, PowerupListInfoVector &vItemList )
{
	vItemList.clear();

	for ( int i = 0 ; i < (int)EnablePowerUpNumVec.size() ; ++i )
	{
		PowerupListInfo kInfo;
		if ( m_dwCurSubBtn == ID_HERO_BTN )
		{
			kInfo.m_iData = EnablePowerUpNumVec[i];
			ioHashString szError;
			if( g_PowerUpManager.CheckEnableCharPowerUp( kInfo.m_iData, szError ) )
				kInfo.m_iShopMarkType = SHOP_MARK_TYPE_BOUGHT;
		}
		else if ( m_dwCurSubBtn == ID_WEAPON_BTN )
		{
			//고급 무기만 체크
			kInfo.m_iData = EnablePowerUpNumVec[i] + DEFAULT_EXTRA_ITEM_CODE;
			// solt check
			EXTRAITEMSLOT kSlot;
			ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
			if( !pExtraItem )
				return;

			const ioItem *pItem = g_ItemMaker.GetItemConst( kInfo.m_iData, __FUNCTION__ );
			if ( !pItem )
				return;

			ExtraItemList vExtraItem;
			vExtraItem.clear();
			pExtraItem->FindAllItem( kInfo.m_iData, vExtraItem );
			if ( !vExtraItem.empty() )
			{
				for ( int i= 0 ; i < (int)vExtraItem.size() ; ++i )
				{
					if( vExtraItem[i].m_PeriodType == EPT_MORTMAIN )
						kInfo.m_iShopMarkType = SHOP_MARK_TYPE_BOUGHT;
				}
			}
		}
		else
		{
			kInfo.m_iData = EnablePowerUpNumVec[i];

			EXTRAITEMSLOT kSlot;
			ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
			if( !pExtraItem )
				return;

			const ioItem *pItem = g_ItemMaker.GetItemConst( kInfo.m_iData, __FUNCTION__ );
			if ( !pItem )
				return;

			ExtraItemList vExtraItem;
			vExtraItem.clear();
			pExtraItem->FindAllItem( kInfo.m_iData, vExtraItem );
			if ( !vExtraItem.empty() )
			{
				for ( int i= 0 ; i < (int)vExtraItem.size() ; ++i )
				{
					if( vExtraItem[i].m_PeriodType == EPT_MORTMAIN )
						kInfo.m_iShopMarkType = SHOP_MARK_TYPE_BOUGHT;
				}
			}
		}

		//보유중인 아이템 및 용병을 먼저 노출
		kInfo.m_iOrder = ( kInfo.m_iShopMarkType * 1000000 ) + kInfo.m_iData;

		vItemList.push_back( kInfo );
	}

	if( vItemList.empty() )
		return;

	std::sort( vItemList.begin(), vItemList.end(), EnablePowerupListSort() );
}

void EnablePowerupListWnd::UpdatePage( int iPage )
{
	m_iCurPage = iPage;

	char szBuf[MAX_PATH] = "";
	wsprintf_e( szBuf, "%d/%d", m_iCurPage+1, m_iMaxPage );
	m_szPageText = szBuf;

	SetChildActive( ID_PRE_BTN );
	SetChildActive( ID_NEXT_BTN );

	if( m_iCurPage == 0 )
		SetChildInActive( ID_PRE_BTN );

	if( m_iCurPage == m_iMaxPage-1 )
		SetChildInActive( ID_NEXT_BTN );

	UpdateIconBtn();
}

void EnablePowerupListWnd::UpdateIconBtn()
{
	int iStartArray = m_iCurPage * ICON_BTN_MAX;

	for( int i=0; i < ICON_BTN_MAX; ++i )
	{
		EnablePowerupListIconBtn *pBtn = dynamic_cast< EnablePowerupListIconBtn* >(FindChildWnd(ID_ICON_BTN1+i));
		if( !pBtn ) 
			continue;

		PowerupListInfo kInfo;

		int iCurArray = iStartArray + i;
		if( COMPARE( iCurArray, 0, (int)m_vCurList.size() ) )
		{
			kInfo = m_vCurList[iCurArray];
			pBtn->SetActive();
		}
		else
		{
			pBtn->SetInActive();
			pBtn->SetName( "-" );
			pBtn->SetItemCode( 0 );
			pBtn->AddWndStyle( IWS_NO_MOUSE_RESPONSE );
		}

		if ( m_dwCurSubBtn == ID_HERO_BTN )
		{
			const ioSetItemInfo *pInfo = g_SetItemInfoMgr.GetSetInfoByIdx( kInfo.m_iData - 1 );
			if ( !pInfo )
				continue;

			pBtn->RemoveWndStyle( IWS_NO_MOUSE_RESPONSE );
			pBtn->SetName( pInfo->GetName() );
			pBtn->SetItemCode( kInfo.m_iData );
			pBtn->SetMarkType( kInfo.m_iShopMarkType );
			pBtn->SetIcon(  g_UIImageSetMgr.CreateImageByFullName( g_MyInfo.GetMySoldierIconName( kInfo.m_iData ), 0 ) );
			pBtn->SetSubIcon(  g_UIImageSetMgr.CreateImageByFullName( g_MyInfo.GetSoldierSubIconName( kInfo.m_iData ), 0 ) );
			pBtn->SetGradeType( g_UIImageRenderMgr.GetPowerUpGradeType( kInfo.m_iData ) );
		}
		else
		{
			const ioItem *pItem = g_ItemMaker.GetItemConst( kInfo.m_iData, __FUNCTION__ );
			if ( pItem )
			{
				if ( m_dwCurSubBtn == ID_WEAPON_BTN )
					pBtn->AddWndStyle( IWS_NO_MOUSE_RESPONSE );
				else
					pBtn->RemoveWndStyle( IWS_NO_MOUSE_RESPONSE );

				pBtn->SetName( pItem->GetName() );
				pBtn->SetItemCode( kInfo.m_iData );
				pBtn->SetMarkType( kInfo.m_iShopMarkType );
				if( !pItem->GetItemLargeIconName().IsEmpty() )
					pBtn->SetIcon(  g_UIImageSetMgr.CreateImageByFullName( pItem->GetItemLargeIconName(), 0 ) );
				pBtn->SetGradeType( pItem->GetGradeType() );
			}
		}
	}
}

void EnablePowerupListWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	enum
	{
		DESC_X		= 238,
		DESC_Y		= 470,

		LINE_GAP	= 20,

		PAGE_X		= 238,
		PAGE_Y		= 425,
	};

	for (int i = 0; i < MAX_TEXT ; i++)
		m_UnderLinePrinter[i].PrintFullText( iXPos + DESC_X, iYPos + DESC_Y + ( i*LINE_GAP), TAT_CENTER );

	m_PagePrinter.PrintFullText( iXPos+PAGE_X, iYPos+PAGE_Y, TAT_CENTER );
}

void EnablePowerupListWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_PRE_BTN:
		if( cmd == IOBN_BTNUP && !pWnd->HasWndStyle( IWS_INACTIVE ) )
		{
			int iNewPage = m_iCurPage - 1;
			UpdatePage( iNewPage );
		}
		break;
	case ID_NEXT_BTN:
		if( cmd == IOBN_BTNUP && !pWnd->HasWndStyle( IWS_INACTIVE ) )
		{
			int iNewPage = m_iCurPage + 1;
			UpdatePage( iNewPage );
		}
		break;
	case ID_HERO_BTN:
	case ID_WEAPON_BTN:
	case ID_ARMOR_BTN:
	case ID_HELMET_BTN:
	case ID_CLOAK_BTN:
		if( cmd == IOBN_BTNDOWN )
		{
			ChangeSubBtn( dwID );
		}
		break;
	case ID_X:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;
	default:
		if ( COMPARE( dwID, ID_ICON_BTN1, ID_ICON_BTN1 + ICON_BTN_MAX ) )
		{
			if( cmd == IOWN_OVERED )
			{
				EnablePowerupListIconBtn* pBtn = dynamic_cast< EnablePowerupListIconBtn* >(FindChildWnd( dwID ) );
				if ( !pBtn )
					break;
				
				PowerUpToolTip* pToolTip = dynamic_cast< PowerUpToolTip* >(FindChildWnd( ID_POWERUP_TOOLTIP ) );
				if ( pToolTip && !pToolTip->IsShow() )
				{
					int nToolTipType = PowerUpToolTip::Power_Char;
					switch( m_dwCurSubBtn )
					{
					case ID_ARMOR_BTN:
					case ID_HELMET_BTN:
					case ID_CLOAK_BTN:
						nToolTipType = PowerUpToolTip::Power_Item;
						break;
					}

					pToolTip->SetInfo( nToolTipType, pBtn->GetItemCode() );
					pToolTip->ShowWnd();
				}
			}
			else if( cmd == IOWN_LEAVED )
				HideChildWnd( ID_POWERUP_TOOLTIP );
		}
		break;
	}
}
//////////////////////////////////////////////////////////////////////////
PowerUpToolTip::PowerUpToolTip()
{
}

PowerUpToolTip::~PowerUpToolTip()
{
}

void PowerUpToolTip::OnProcess( float fTimePerSec )
{
	if ( IsShow() )
	{
		ioLSMouse *pMouse = g_App.GetMouse();
		if( !pMouse ) return;
		pMouse->SetToolTipCursor();
		int iXPos = 0;
		int iYPos = 0;
		Help::GetNotCutWndPos( pMouse->GetMousePos().x, pMouse->GetMousePos().y, GetWidth(), GetHeight(), iXPos, iYPos );
		SetScreenWndPos( iXPos, iYPos );
		GoToTop();
	}
}

void PowerUpToolTip::SetInfo( int nType, int iIndex )
{
	switch( nType )
	{
	case Power_Char:
		SetCharInfo( iIndex );
		break;
	case Power_Item:
		SetItemInfo( iIndex );
		break;
	}
}

void PowerUpToolTip::SetCharInfo( int nClassType )
{
	if( !g_PowerUpManager.GetCharGradeInfo( nClassType ) )
	{
		HideWnd();
		return;
	}

	int SizeX;
	g_PowerUpManager.GetCharGradeToolTipSize( nClassType, SizeX );

	m_szToolTip.Set( 0, 0, g_PowerUpManager.GetCharToolTipString( nClassType ).c_str(), FONT_SIZE_12, SizeX - 10, TS_NORMAL, TCT_DEFAULT_DARKGRAY, 0, TAT_LEFT, TVA_TOP, 4);
	SetSize( SizeX, m_szToolTip.GetLineSize() * 18 + 10 );

	ioLSMouse *pMouse = g_App.GetMouse();
	if( !pMouse ) 
		return;

	int iXPos = 0;
	int iYPos = 0;
	Help::GetNotCutWndPos( pMouse->GetMousePos().x, pMouse->GetMousePos().y, GetWidth(), GetHeight(), iXPos, iYPos );

	iXPos -= m_pParent->GetPosX();
	iYPos -= m_pParent->GetPosY();
	SetWndPos( iXPos, iYPos );
}

void PowerUpToolTip::SetItemInfo( int nItemCode )
{
	if( !g_PowerUpManager.GetRarePowerUpItemInfo( nItemCode ) )
	{
		HideWnd();
		return;
	}

	int SizeX;
	g_PowerUpManager.GetExtraItemGradeToolTipSize( nItemCode, SizeX );

	m_szToolTip.Set( 0, 0, g_PowerUpManager.GetExtraItemToolTipString( nItemCode ).c_str(), FONT_SIZE_12, 200, TS_NORMAL, TCT_DEFAULT_DARKGRAY, 0, TAT_LEFT, TVA_TOP, 4);
	SetSize( SizeX, m_szToolTip.GetLineSize() * 18 + 10 );

	ioLSMouse *pMouse = g_App.GetMouse();
	if( !pMouse ) 
		return;

	int iXPos = 0;
	int iYPos = 0;
	Help::GetNotCutWndPos( pMouse->GetMousePos().x, pMouse->GetMousePos().y, GetWidth(), GetHeight(), iXPos, iYPos );

	iXPos -= m_pParent->GetPosX();
	iYPos -= m_pParent->GetPosY();
	SetWndPos( iXPos, iYPos );
}

void PowerUpToolTip::OnRender()
{
	ioWnd::OnRender();
	int nXPos = GetDerivedPosX();
	int nYPos = GetDerivedPosY();

	enum 
	{	
		PRINT_XOFFSET = 9,
		PRINT_YOFFSET = 9,
	};
	m_szToolTip.OnRender( nXPos+PRINT_XOFFSET, nYPos+PRINT_YOFFSET );
}