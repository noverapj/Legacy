
#include "StdAfx.h"

#include "PetResellResultWnd.h"

#include "PetWnd.h"

//////////////////////////////////////////////////////////////////////////

PetResellResultWnd::PetResellResultWnd()
{
	m_pIcon	= NULL;
	m_pBackEffect = NULL;	

	m_nPetCode = 0;
	m_nPetRank = 0;
}

PetResellResultWnd::~PetResellResultWnd()
{
	SAFEDELETE(m_pIcon);
	SAFEDELETE(m_pBackEffect);
}

/////////////////////////////////////////////////////////////////////////////////////////

void PetResellResultWnd::iwm_show()
{

}

void PetResellResultWnd::iwm_hide()
{
	m_nPetCode = 0;
	m_nPetRank = 0;
}

void PetResellResultWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{

}

/////////////////////////////////////////////////////////////////////////////////////////

void PetResellResultWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "back_effect" )
	{
		SAFEDELETE( m_pBackEffect );
		m_pBackEffect = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}

/////////////////////////////////////////////////////////////////////////////////////////

void PetResellResultWnd::OnRender()
{	
	ioWnd::OnRender();

	int nXPos = GetDerivedPosX();
	int nYPos = GetDerivedPosY();
	
	RenderIcon( nXPos, nYPos );
	RenderDesc( nXPos, nYPos );
}

void PetResellResultWnd::RenderIcon( int nXPos, int nYPos )
{
	enum
	{
		ICON_X		= 129,
		ICON_Y		= 105,
	};

	if ( m_nPetCode <= 0 || m_nPetRank <= 0 )
		return;

	if ( !m_pIcon || !m_pBackEffect )
		return;

	m_pBackEffect->Render( nXPos+ICON_X, nYPos+ICON_Y, UI_RENDER_MULTIPLY );	
	m_pIcon->Render( nXPos+ICON_X, nYPos+ICON_Y, UI_RENDER_NORMAL, TFO_BILINEAR );	
}

void PetResellResultWnd::RenderDesc( int nXPos, int nYPos )
{
	enum
	{
		DESC_BIG_X	= 129,
		DESC_BIG_Y	= 152,

		DESC_X		= 27,
		DESC_Y		= 206,

		FINAL_DESC_X	= 128,
		FINAL_DESC_Y	= 421,

		MAX_DESC = 2,
	};

	// Title
	for( int i=0; i < MAX_DESC; ++i )
	{
		if( m_Title[i].IsEmpty() )
			continue;

		m_Title[i].PrintFullText( nXPos+DESC_BIG_X, nYPos+DESC_BIG_Y+(i*23), TAT_CENTER );
	}

	// Desc
	for( int j=0; j < MAX_DESC; ++j )
	{
		if( m_Desc[j].IsEmpty() )
			continue;

		m_Desc[j].PrintFullText( nXPos+DESC_X, nYPos+DESC_Y+(j*18), TAT_LEFT );
	}

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GREEN );
	g_FontMgr.PrintText( nXPos+FINAL_DESC_X, nYPos+FINAL_DESC_Y, FONT_SIZE_17, STR(1) );
}
/////////////////////////////////////////////////////////////////////////////////////////

//Æê È®ÀÎ
void PetResellResultWnd::SetResellResult( int nPetCode, int nPetRank, int nPetLevel )
{
	if ( nPetCode <= 0 || nPetRank <= 0 )
		return;

	m_nPetCode = nPetCode;
	m_nPetRank = nPetRank;
	m_nPetLevel = nPetLevel;

	ioHashString szPetName = g_PetInfoMgr.GetPetName( m_nPetCode, (PetRankType)m_nPetRank );
	ioHashString szPetRank = g_PetInfoMgr.GetRankName( m_nPetRank );
	if ( szPetName.IsEmpty() || szPetRank.IsEmpty() )
		return;
	
	int nResellPeso = g_PetInfoMgr.GetResellPeso();

	char szConvertNum[MAX_PATH] = "";
	Help::ConvertNumToStrComma( nResellPeso, szConvertNum, sizeof( szConvertNum ) );

	m_Title[0].ClearList();
	m_Title[0].SetTextStyle( TS_NORMAL );
	m_Title[0].SetBkColor( 0, 0, 0 );	
	m_Title[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
	m_Title[0].AddTextPiece( FONT_SIZE_17, STR(1), m_nPetLevel, szPetName.c_str(), szPetRank.c_str() );

	m_Title[1].ClearList();
	m_Title[1].SetTextStyle( TS_NORMAL );
	m_Title[1].SetBkColor( 0, 0, 0 );
	m_Title[1].SetTextColor( TCT_DEFAULT_DARKGRAY );
	m_Title[1].AddTextPiece( FONT_SIZE_17, STR(2), szConvertNum );

	m_Desc[0].ClearList();
	m_Desc[0].SetTextStyle( TS_NORMAL );
	m_Desc[0].SetBkColor( 0, 0, 0 );	
	m_Desc[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
	m_Desc[0].AddTextPiece( FONT_SIZE_12, STR(3),  m_nPetLevel, szPetName.c_str(), szPetRank.c_str() );

	m_Desc[1].ClearList();
	m_Desc[1].SetTextStyle( TS_NORMAL );
	m_Desc[1].SetBkColor( 0, 0, 0 );
	m_Desc[1].SetTextColor( TCT_DEFAULT_DARKGRAY );
	m_Desc[1].AddTextPiece( FONT_SIZE_12, STR(4), szConvertNum );

	ioHashString szPetIcon = g_PetInfoMgr.GetPetIcon( m_nPetCode, (PetRankType)m_nPetRank );
	if ( !szPetIcon.IsEmpty() )
	{
		SAFEDELETE( m_pIcon );
		m_pIcon = g_UIImageSetMgr.CreateImageByFullName( szPetIcon );
	}

	ShowWnd();
}