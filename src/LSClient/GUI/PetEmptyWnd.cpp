
#include "StdAfx.h"

#include "PetEmptyWnd.h"

#include "../ioComplexStringPrinter.h"

#include "NewShopWnd.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PetEmptyWnd::PetEmptyWnd()
{
	m_pBottomFrm = NULL;
	m_pBackEffect = NULL;
	m_pPetIcon = NULL;
}

PetEmptyWnd::~PetEmptyWnd()
{
	SAFEDELETE( m_pBottomFrm );
	SAFEDELETE( m_pBackEffect );
	SAFEDELETE( m_pPetIcon );
}

void PetEmptyWnd::iwm_show()
{	
	NewShopWnd *pShopWnd = dynamic_cast<NewShopWnd*>( g_GUIMgr.FindWnd( NEW_SHOP_WND ) );
	if( pShopWnd && pShopWnd->IsShow() )
		pShopWnd->HideWnd();

	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>( g_GUIMgr.FindWnd( MY_INVENTORY_WND ) );
	if( pInvenWnd && pInvenWnd->IsShow() )
		pInvenWnd->HideWnd();
}

void PetEmptyWnd::iwm_hide()
{
	
}

void PetEmptyWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
	case ID_CANCEL:
		if( cmd == IOBN_BTNUP )
			HideWnd();
		break;
	case ID_SHOP:
		if( cmd == IOBN_BTNUP )
		{	
			NewShopWnd *pShopWnd = dynamic_cast<NewShopWnd*>(g_GUIMgr.FindWnd(NEW_SHOP_WND));
			if( pShopWnd )
				pShopWnd->ShowEtcItemTab();

			HideWnd();					
		}
		break;
	}
}

void PetEmptyWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "pet_icon" )
	{
		SAFEDELETE( m_pPetIcon );
		m_pPetIcon = pImage;
	}
	else if( szType == "back_effect" )
	{
		SAFEDELETE( m_pBackEffect );
		m_pBackEffect = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}

void PetEmptyWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "BottomFrm" )
	{
		SAFEDELETE( m_pBottomFrm );
		m_pBottomFrm = pFrame;
	}
	else
		ioWnd::AddRenderFrame( szType, pFrame );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PetEmptyWnd::OnRender()
{
	ioWnd::OnRender();

	int nXPos = GetDerivedPosX();
	int nYPos = GetDerivedPosY();

	RenderDesc( nXPos, nYPos );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PetEmptyWnd::RenderDesc( int nXPos, int nYPos )
{
	enum
	{
		BACK_X_OFFSET = 151,
		BACK_Y_OFFSET = 98,
	};

	if( m_pBottomFrm )
		m_pBottomFrm->Render( nXPos, nYPos, UI_RENDER_MULTIPLY );

	if ( m_pBackEffect )
		m_pBackEffect->Render( nXPos + BACK_X_OFFSET, nYPos + BACK_Y_OFFSET, UI_RENDER_MULTIPLY );

	if ( m_pPetIcon )
		m_pPetIcon->Render( nXPos + BACK_X_OFFSET, nYPos + BACK_Y_OFFSET );
}