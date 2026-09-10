
#include "StdAfx.h"

#include "ioCostumeReleaseWnd.h"

//////////////////////////////////////////////////////////////////////////
ioCostumeReleaseWnd::ioCostumeReleaseWnd()
{
	m_pIconBack		 = NULL;
	m_pCostumeIcon = NULL;
	m_pCostumeMark = NULL;
	m_dwHideDelayTime = 0;
	m_nGradeType = 0;
}

ioCostumeReleaseWnd::~ioCostumeReleaseWnd()
{
	SAFEDELETE( m_pIconBack );
	SAFEDELETE( m_pCostumeIcon );
	SAFEDELETE( m_pCostumeMark );
}

void ioCostumeReleaseWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "IconBack" )
	{
		SAFEDELETE( m_pIconBack );
		m_pIconBack = pImage;
	}
	else if( szType == "costume_image" )
	{
		SAFEDELETE( m_pCostumeMark );
		m_pCostumeMark = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}

void ioCostumeReleaseWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
	{
		if( FRAMEGETTIME() - m_dwHideDelayTime < 100 )
			return;

		if( g_MyInfo.IsReserveDeleteCostume() )
			ShowWnd();
		return;
	}

	ioMovingWnd::OnProcess( fTimePerSec );
}

void ioCostumeReleaseWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_pIconBack )
		m_pIconBack->Render( iXPos, iYPos, UI_RENDER_MULTIPLY );

	g_UIImageRenderMgr.RenderGradeIconBack( m_nGradeType, iXPos + 103, iYPos + 98 );

	if( m_pCostumeIcon )
		m_pCostumeIcon->Render( iXPos + 103, iYPos + 98 );

	// UI코스튬 관련 (코스튬 마크)
	if( m_pCostumeMark )
		m_pCostumeMark->Render( iXPos + 103, iYPos + 98 );

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 103, iYPos + 144, FONT_SIZE_13, "%s의", m_szClassName.c_str() ); 
	g_FontMgr.PrintText( iXPos + 103, iYPos + 162, FONT_SIZE_13, "%s", m_szName.c_str() ); 
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( iXPos + 103, iYPos + 180, FONT_SIZE_13, "기간만료로 해제되었습니다" ); 
}

void ioCostumeReleaseWnd::iwm_show()
{
	SAFEDELETE( m_pCostumeIcon );
	m_szClassName.Clear();
	m_szName.Clear();
	m_szIconName.Clear();

	int nCostumeCode = 0;

	g_MyInfo.GetFirstDeleteCostume( m_szClassName, m_szName, m_szIconName, nCostumeCode );
	m_pCostumeIcon = g_UIImageSetMgr.CreateImageByFullName( m_szIconName );

	m_nGradeType = g_CostumeInfoMgr.GetGradeType( nCostumeCode );
}

void ioCostumeReleaseWnd::iwm_hide()
{
	m_szClassName.Clear();
	m_szName.Clear();
	m_szIconName.Clear();

	m_dwHideDelayTime = FRAMEGETTIME();
}

bool ioCostumeReleaseWnd::iwm_esc()
{
	if( !IsShow() ) 
		return false;

	if( ioWnd::iwm_esc() )
	{
		g_MyInfo.EraseDeleteCostume( false );
		return true;
	}
	return false;
}

void ioCostumeReleaseWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_ALL_OK:
		if( cmd == IOBN_BTNUP )
		{
			g_MyInfo.EraseDeleteCostume( true );
			HideWnd();
		}
		break;
	case ID_X:
	case ID_OK:
		if( cmd == IOBN_BTNUP )
		{
			g_MyInfo.EraseDeleteCostume( false );
			HideWnd();
		}
		break;
	}
}