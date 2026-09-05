#include "StdAfx.h"
#include "ioAccessoryReleseWnd.h"

ioAccessoryReleseWnd::ioAccessoryReleseWnd(void)
{
	m_pIconBack		 = NULL;
	m_pAccessoryIcon = NULL;

	m_dwHideDelayTime = 0;
	m_nGradeType = 0;
}

ioAccessoryReleseWnd::~ioAccessoryReleseWnd(void)
{
	SAFEDELETE( m_pIconBack );
	SAFEDELETE( m_pAccessoryIcon );
}

void ioAccessoryReleseWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "IconBack" )
	{
		SAFEDELETE( m_pIconBack );
		m_pIconBack = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}

void ioAccessoryReleseWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
	{
		if( FRAMEGETTIME() - m_dwHideDelayTime < 100 )
			return;

		if( g_MyInfo.IsReserveDeleteAccessory() )
		{
			ShowWnd();
		}
		return;
	}
	ioMovingWnd::OnProcess( fTimePerSec );
}

void ioAccessoryReleseWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_pIconBack )
		m_pIconBack->Render( iXPos, iYPos, UI_RENDER_MULTIPLY );

	g_UIImageRenderMgr.RenderGradeIconBack( m_nGradeType, iXPos + 103, iYPos + 98 );

	if( m_pAccessoryIcon )
	{
		m_pAccessoryIcon->Render( iXPos + 103, iYPos + 98 );
	}

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 103, iYPos + 144, FONT_SIZE_13, "%s 의", m_szClassName.c_str() ); 
	g_FontMgr.PrintText( iXPos + 103, iYPos + 162, FONT_SIZE_13, "%s", m_szItemName.c_str() ); 
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( iXPos + 103, iYPos + 180, FONT_SIZE_13, "기간만료로 해제되었습니다" ); 
}

void ioAccessoryReleseWnd::iwm_show()
{
	SAFEDELETE( m_pAccessoryIcon );
	m_szClassName.Clear();
	m_szItemName.Clear();
	m_szIconName.Clear();

	int nItemCode = 0;
	g_MyInfo.GetFirstDeleteAccessory( m_szClassName, m_szItemName, m_szIconName, nItemCode );
	m_pAccessoryIcon = g_UIImageSetMgr.CreateImageByFullName( m_szIconName );

	const ioItem *pItem = g_ItemMaker.GetItemConst( nItemCode, __FUNCTION__ );
	if( pItem )
		m_nGradeType = pItem->GetGradeType();
}

void ioAccessoryReleseWnd::iwm_hide()
{
	m_szClassName.Clear();
	m_szItemName.Clear();
	m_szIconName.Clear();

	m_dwHideDelayTime = FRAMEGETTIME();
}

bool ioAccessoryReleseWnd::iwm_esc()
{
	if( !IsShow() ) return false;
	if( ioWnd::iwm_esc() )
	{
		g_MyInfo.EraseDeleteAccessory( false );
		return true;
	}
	return false;
}

void ioAccessoryReleseWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_ALL_OK:
		if( cmd == IOBN_BTNUP )
		{
			g_MyInfo.EraseDeleteAccessory( true );
			HideWnd();
		}
		break;
	case ID_X:
	case ID_OK:
		if( cmd == IOBN_BTNUP )
		{
			g_MyInfo.EraseDeleteAccessory( false );
			HideWnd();
		}
		break;
	}
}
