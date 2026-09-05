#include "StdAfx.h"

#include "../io3DEngine/ioWnd.h"

#include "../Housing/ioUserBlockStorage.h"
#include "../Housing/ioBlockSetupManager.h"
#include "../Housing/ioBlockEventAgency.h"
#include "../Housing/ioBlockManager.h"
#include "../Housing/ioBlock.h"


#include "../StateClass/ioConstructHouseSpecialState.h"

#include "ioHousingBlockInvenWnd.h"
#include "ioHousingBlockSetupWnd.h"

ioHousingShortCutKeyButton::ioHousingShortCutKeyButton()
{
	m_pShortcutKey = NULL;
}

ioHousingShortCutKeyButton::~ioHousingShortCutKeyButton()
{
	SAFEDELETE( m_pShortcutKey );
}

void ioHousingShortCutKeyButton::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "ShortCut" )
	{
		SAFEDELETE( m_pShortcutKey );
		m_pShortcutKey = pImage;
	}
	else
	{
		ioButton::AddRenderImage( szType, pImage );
	}
}

void ioHousingShortCutKeyButton::OnDrawNormal( int iXPos, int iYPos )
{
	ioButton::OnDrawNormal( iXPos, iYPos );
}

void ioHousingShortCutKeyButton::OnDrawOvered( int iXPos, int iYPos )
{
	if( m_pOver )
	{
		m_pOver->Render( iXPos, iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
		m_pOver->SetAlpha( (float)MAX_ALPHA_RATE * FLOAT08 );
		m_pOver->Render( iXPos, iYPos, UI_RENDER_ADD, TFO_BILINEAR );
	}
	OnDrawOveredAdd( iXPos, iYPos );
	RenderImageList( iXPos, iYPos, UI_RENDER_NORMAL );
}

void ioHousingShortCutKeyButton::OnDrawPushed( int iXPos, int iYPos )
{
	ioButton::OnDrawPushed( iXPos, iYPos );

	if( m_pPush )
	{
		m_pPush->Render( iXPos, iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
		m_pPush->SetAlpha( (float)MAX_ALPHA_RATE * FLOAT08 );
		m_pPush->Render( iXPos, iYPos, UI_RENDER_ADD, TFO_BILINEAR );
	}

	RenderImageList( iXPos, iYPos, UI_RENDER_NORMAL );
}

void ioHousingShortCutKeyButton::OnDrawDisabled( int iXPos, int iYPos )
{
	ioButton::OnDrawDisabled( iXPos, iYPos );

	if( m_pDisable )
	{
		m_pDisable->Render( iXPos, iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
		m_pDisable->SetAlpha( (float)MAX_ALPHA_RATE * FLOAT08 );
		m_pDisable->Render( iXPos, iYPos, UI_RENDER_ADD, TFO_BILINEAR );
	}
	RenderImageList( iXPos, iYPos, UI_RENDER_GRAY );
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ioHousingIconButton::ioHousingIconButton()
{
	m_pIcon = NULL;
}

ioHousingIconButton::~ioHousingIconButton()
{
	SAFEDELETE( m_pIcon );
}

void ioHousingIconButton::SetIcon( const ioHashString& szName )
{
	SAFEDELETE( m_pIcon );
	m_pIcon = g_UIImageSetMgr.CreateImageByFullName( szName.c_str() );

	if( m_pIcon )
		SetSize( m_pIcon->GetWidth(), m_pIcon->GetHeight() );
}

void ioHousingIconButton::OnRender()
{
	ioButton::OnRender();

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	if( m_pIcon )
		m_pIcon->Render( iXPos, iYPos );
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ioHousingBlockSetupWnd::ioHousingBlockSetupWnd()
{
	m_pSetUpBack		= NULL;
	m_pRemoveBack		= NULL;	

	m_eBlockModeType	= BMT_GUILD;
}

ioHousingBlockSetupWnd::~ioHousingBlockSetupWnd()
{
	SAFEDELETE( m_pSetUpBack );
	SAFEDELETE( m_pRemoveBack );
}

void ioHousingBlockSetupWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "SetUpBack" )
	{
		SAFEDELETE( m_pSetUpBack );
		m_pSetUpBack = pImage;
	}
	else if( szType == "RemoveBack" )
	{
		SAFEDELETE( m_pRemoveBack );
		m_pRemoveBack = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void ioHousingBlockSetupWnd::iwm_show()
{	
	SetWndPos( Setting::Width()/2 - GetWidth()/2, Setting::Height() - GetHeight() - 51 );

	m_State				= 0;
	m_nCurrRemoveCode	= 0;
	m_nCurrSetUpIndex	= 0;

	m_nInvenItemCount	= 0;	
}

void ioHousingBlockSetupWnd::iwm_hide()
{
}

bool ioHousingBlockSetupWnd::iwm_esc()
{
	g_BlockSetupManager.EndSetUpState();
	HideWnd();
	return true;
}

void ioHousingBlockSetupWnd::SetBlockModeType(int eType )
{
	m_eBlockModeType = eType;
}

ioUserBlockStorage* ioHousingBlockSetupWnd::GetStorage()
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

void ioHousingBlockSetupWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	ioConstructHouseSpecialState* pState = g_BlockSetupManager.GetAdminState();
	if( pState )
	{
		int eCurrState = pState->GetCurrState();
		if( eCurrState == ioConstructHouseSpecialState::S_ERASE )
		{
			if( m_nCurrRemoveCode != pState->GetCurrRemoveCode() || m_State != eCurrState )
			{
				ChangeErase( pState->GetCurrRemoveCode() );
				m_nCurrRemoveCode	= pState->GetCurrRemoveCode();
				m_State				= eCurrState;
			}
			else
			{
				RefreshErase( pState->GetCurrRemoveCode() );
			}
		}
		else
		{
			ioUserBlockStorage* pStorage = GetStorage();
			if( pStorage )
			{
				if( m_nCurrSetUpIndex != pStorage->GetCurrInvenIndex() || m_State != eCurrState )
				{					
					ChangeSetUp( pStorage->GetCurrInvenIndex() );
					m_nCurrSetUpIndex = pStorage->GetCurrInvenIndex();
					m_State = eCurrState;
				}
				else
				{
					RefreshSetUp( m_nCurrSetUpIndex );
				}
			}
		}
	}
}

void ioHousingBlockSetupWnd::ChangeErase( int nRemoveCode )
{
	HideChildWnd( ID_ICON );
	HideChildWnd( ID_EXIT_SETUP );
	ShowChildWnd( ID_EXIT_REMOVE );

	const ioBlockProperty* pProperty = g_BlockManager.GetBlockTemplete( nRemoveCode );
	if( pProperty )
	{
		m_Name				= pProperty->GetName();
		m_nWidth			= pProperty->GetWidth();
		m_nDepth			= pProperty->GetDepth();
		m_nHeight			= pProperty->GetHeight();
		m_nInvenItemCount	= 0;

		ioUserBlockStorage* pStorage = GetStorage();
		if( pStorage )
		{
			ioUserBlockStorage::InvenBlockItem* pItem = pStorage->GetInvenBlockItemByCode( nRemoveCode );
			if( pItem )
				m_nInvenItemCount = pItem->m_nCount;
		}
		
		ioHousingIconButton* pButton = dynamic_cast<ioHousingIconButton*>( FindChildWnd( ID_ICON ) );
		if( pButton )
		{			
			pButton->SetIcon( pProperty->GetIconName() );
			pButton->ShowWnd();		
		}
	}
	else
	{
		m_Name.Clear();

		m_nInvenItemCount	= 0;
		m_nWidth			= 0;
		m_nDepth			= 0;
		m_nHeight			= 0;
	}

	HideChildWnd( ID_PREV );
	HideChildWnd( ID_NEXT );
	HideChildWnd( ID_SETUP_OK );
	HideChildWnd( ID_SETUP_DESC );	
	ShowChildWnd( ID_REMOVE_OK );
	ShowChildWnd( ID_REMOVE_DESC );
}

void ioHousingBlockSetupWnd::RefreshErase( int nRemoveCode )
{
	ioUserBlockStorage* pStorage = GetStorage();
	if( pStorage )
	{
		ioUserBlockStorage::InvenBlockItem* pItem = pStorage->GetInvenBlockItemByCode( nRemoveCode );
		if( pItem )
			m_nInvenItemCount = pItem->m_nCount;
	}
}

void ioHousingBlockSetupWnd::ChangeSetUp( int nCurrSetUpIndex )
{
	HideChildWnd( ID_ICON );	
	HideChildWnd( ID_EXIT_REMOVE );
	ShowChildWnd( ID_EXIT_SETUP );

	ioUserBlockStorage* pStorage = GetStorage();
	if( pStorage )
	{
		ioUserBlockStorage::InvenBlockItem* pItem = pStorage->GetInvenBlockItem( nCurrSetUpIndex );
		if( pItem )
		{
			m_nInvenItemCount = pItem->m_nCount;		

			const ioBlockProperty* pProperty = g_BlockManager.GetBlockTemplete( pItem->m_nBlockCode );
			if( pProperty )
			{
				m_Name		= pProperty->GetName();
				m_nWidth	= pProperty->GetWidth();
				m_nDepth	= pProperty->GetDepth();
				m_nHeight	= pProperty->GetHeight();

				ioHousingIconButton* pButton = dynamic_cast<ioHousingIconButton*>( FindChildWnd( ID_ICON ) );	
				if( pButton )
				{
					pButton->SetIcon( pProperty->GetIconName() );
					pButton->ShowWnd();
				}
			}
		}
		else
		{
			m_Name.Clear();

			m_nInvenItemCount	= 0;		
			m_nWidth			= 0;
			m_nDepth			= 0;
			m_nHeight			= 0;
		}
	}

	ShowChildWnd( ID_PREV );
	ShowChildWnd( ID_NEXT );
	ShowChildWnd( ID_SETUP_OK );
	ShowChildWnd( ID_SETUP_DESC );
	HideChildWnd( ID_REMOVE_OK );
	HideChildWnd( ID_REMOVE_DESC );
}

void ioHousingBlockSetupWnd::RefreshSetUp( int nCurrSetUpIndex )
{
	ioUserBlockStorage* pStorage = GetStorage();
	if( pStorage )
	{
		ioUserBlockStorage::InvenBlockItem* pItem = pStorage->GetInvenBlockItem( nCurrSetUpIndex );
		if( pItem )
		{
			m_nInvenItemCount	= pItem->m_nCount;		
		}
		else
		{
			m_nInvenItemCount	= 0;		
		}
	}
}

void ioHousingBlockSetupWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT_SETUP:
	case ID_EXIT_REMOVE:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
			g_BlockSetupManager.EndSetUpState();
		}
		break;
	case ID_PREV:
		if( cmd == IOBN_BTNUP )
		{
			ioUserBlockStorage* pStorage = GetStorage();
			if( pStorage )
			{
				pStorage->PrevItem();
				g_BlockSetupManager.ChangeToSetUp();
			}
		}
		break;
	case ID_NEXT:
		if( cmd == IOBN_BTNUP )
		{
			ioUserBlockStorage* pStorage = GetStorage();
			if( pStorage )
			{
				pStorage->NextItem();
				g_BlockSetupManager.ChangeToSetUp();
			}
		}
		break;
	case ID_SETUP_OK:
		if( cmd == IOBN_BTNUP )
		{
			ioConstructHouseSpecialState* pState = g_BlockSetupManager.GetAdminState();
			if( pState )
			{
				if( !pState->SendSetUp() )
					g_ChatMgr.SetSystemMsg( "여기에는 설치 할 수 없습니다." );

			}
		}
		break;
	case ID_REMOVE_OK:
		if( cmd == IOBN_BTNUP )
		{
			ioConstructHouseSpecialState* pState = g_BlockSetupManager.GetAdminState();
			if( pState )
			{
				if( !pState->SendRemove() )
					g_ChatMgr.SetSystemMsg( "선택된 블럭이 없습니다." );
			}
		}
		break;
	case ID_ITEM_LIST:
		if( cmd == IOBN_BTNUP )
		{
			ioHousingBlockInvenWnd* pWnd = dynamic_cast<ioHousingBlockInvenWnd*>( g_GUIMgr.FindWnd( HOUSING_BLOCK_INVEN_WND ) );
			if( pWnd )
			{
				pWnd->SetBlockModeType( m_eBlockModeType );
				pWnd->ShowWnd();
			}
		}
		break;
	}
}

void ioHousingBlockSetupWnd::OnRender()
{
	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	OnBackRender( iXPos, iYPos );
	
	ioWnd::OnRender();

	if( m_State == ioConstructHouseSpecialState::S_ERASE )
	{
		if( m_Name.IsEmpty() )
		{
			OnRemoveDescRender( iXPos, iYPos );
		}
		else
		{
			OnBlockDescRender( iXPos, iYPos );
		}
	}
	else
	{
		OnBlockDescRender( iXPos, iYPos );
	}

}

void ioHousingBlockSetupWnd::OnBackRender( int nXPos, int nYPos )
{
	if( m_State == ioConstructHouseSpecialState::S_ERASE )
	{
		if( m_pRemoveBack )
			m_pRemoveBack->Render( nXPos, nYPos );
	}
	else
	{
		if( m_pSetUpBack )
			m_pSetUpBack->Render( nXPos, nYPos );
	}
}

void ioHousingBlockSetupWnd::OnBlockDescRender( int nXPos, int nYPos )
{

	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );	
	g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
	if( m_Name.IsEmpty() )
		g_FontMgr.PrintTextWidthCut( nXPos + 247, nYPos + 120, FONT_SIZE_11, 155, "-" );
	else
		g_FontMgr.PrintTextWidthCut( nXPos + 247, nYPos + 120, FONT_SIZE_11, 155, " %s", m_Name.c_str() );

	g_FontMgr.SetTextColor( 0xFF73F0FF );
	g_FontMgr.PrintTextWidthCut( nXPos + 247, nYPos + 136, FONT_SIZE_11, 155, "넓이%d × 깊이%d × 높이%d", m_nWidth, m_nDepth, m_nHeight );

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( 0xFFF92104 );
	kPrinter.AddTextPiece( FONT_SIZE_11, "%d개", m_nInvenItemCount );
	kPrinter.PrintFullText( nXPos + 247, nYPos + 152, TAT_CENTER );
	kPrinter.ClearList();
}

void ioHousingBlockSetupWnd::OnRemoveDescRender( int nXPos, int nYPos )
{
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );	
	g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
	g_FontMgr.PrintText( nXPos + 247, nYPos + 128, FONT_SIZE_11, " 마법진으로 아이템 선택 후" );

	g_FontMgr.SetTextColor( 0xFFFF9393 );
	g_FontMgr.PrintText( nXPos + 247, nYPos + 144, FONT_SIZE_11, " D키 입력" );
}