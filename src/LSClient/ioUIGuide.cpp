#include <StdAfx.h>

#include "ioUIGuide.h"

template<> ioUIGuide* Singleton< ioUIGuide >::ms_Singleton = 0;
ioUIGuide::ioUIGuide()
{
	m_pGuideLine = NULL;
	m_bShow = false;
}

ioUIGuide::~ioUIGuide()
{
	SAFEDELETE( m_pGuideLine );
}

ioUIGuide& ioUIGuide::GetSingleton()
{
	return Singleton< ioUIGuide >::GetSingleton();
}

void ioUIGuide::LoadINIData()
{
	ioINILoader_e kLoader( "config/lsc_ui_guide.ini" );	
	kLoader.SetTitle_e( "Resource" );
	char szBuf[MAX_PATH] = "";
	kLoader.LoadString_e( "GuideFrame", "", szBuf, sizeof( szBuf ) );
	m_pGuideLine = g_GUIMgr.CreateFrame( szBuf );
}

void ioUIGuide::Render()
{
	if( !m_bShow )
		return;

	ioWnd *pCurrWnd = g_GUIMgr.GetPreOverWnd();

	ioLSMouse *pMouse = g_App.GetMouse();
	if( !pMouse )
		return;

	int iXPos = pMouse->GetMousePos().x;
	int iYPos = pMouse->GetMousePos().y;

	if( !pCurrWnd )
	{
		ioComplexStringPrinter szUIText;
		szUIText.SetTextStyle( TS_OUTLINE_FULL_2X );
		szUIText.SetBkColor( TCT_DEFAULT_BLACK );
		szUIText.SetTextColor( TCT_DEFAULT_WHITE );
		szUIText.AddTextPiece( FONT_SIZE_14, "[Parent:NULL][Over:NULL]" );
		szUIText.PrintFullText( pMouse->GetMousePos().x, pMouse->GetMousePos().y - 20, TAT_CENTER );
	}
	else
	{
		if( m_pGuideLine )
		{
			m_pGuideLine->SetSize( pCurrWnd->GetWidth(), pCurrWnd->GetHeight() );
			m_pGuideLine->Render( pCurrWnd->GetDerivedPosX(), pCurrWnd->GetDerivedPosY() );
		}

		char szSizeText[MAX_PATH] = "";
		sprintf_e( szSizeText, "%d,%d (%dx%d)", pCurrWnd->GetXPos(), pCurrWnd->GetYPos(), pCurrWnd->GetWidth(), pCurrWnd->GetHeight() );
		g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetVertAlign( TVA_TOP );
		g_FontMgr.SetBkColor( TCT_DEFAULT_BLACK );
		g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
		g_FontMgr.PrintText( pCurrWnd->GetDerivedPosX()+2, pCurrWnd->GetDerivedPosY()+2, FONT_SIZE_12, szSizeText );

		//if( g_Input.IsKeyDown(KEY_LEFTSHIFT) )
		//	RenderXMLName( pCurrWnd, iXPos, iYPos - 70 );
		//else
			RenderClassName( pCurrWnd, iXPos, iYPos - 70 );
	}

	g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
	if( iXPos <= 40 )
		g_FontMgr.SetAlignType( TAT_LEFT );
	else
		g_FontMgr.SetAlignType( TAT_RIGHT );
	g_FontMgr.SetVertAlign( TVA_TOP );
	g_FontMgr.SetBkColor( TCT_DEFAULT_BLACK );
	g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
	g_FontMgr.PrintText( iXPos, iYPos - 13, FONT_SIZE_12, "%d, %d", iXPos, iYPos );
}

void ioUIGuide::RenderClassName( ioWnd *pCurrWnd, int iXPos, int iYPos )
{
	if( !pCurrWnd )
		return;

	ioComplexStringPrinter szUIText0;
	ioComplexStringPrinter szUIText1;
	ioComplexStringPrinter szUIText2;
	szUIText0.SetTextStyle( TS_OUTLINE_FULL_3X );
	szUIText0.SetBkColor( TCT_DEFAULT_BLACK );
	szUIText1.SetTextStyle( TS_OUTLINE_FULL_3X );
	szUIText1.SetBkColor( TCT_DEFAULT_BLACK );
	szUIText2.SetTextStyle( TS_OUTLINE_FULL_3X );
	szUIText2.SetBkColor( TCT_DEFAULT_BLACK );

	if( pCurrWnd->GetParent() )
	{
		if( pCurrWnd->GetParent()->GetParent() )
		{
			szUIText0.SetTextColor( TCT_DEFAULT_WHITE );
			szUIText0.AddTextPiece( FONT_SIZE_14, "[+] " );
			if( strcmp( pCurrWnd->GetParent()->GetParent()->GetClassNameEX(), "" ) != 0 )
			{
				szUIText0.AddTextPiece( FONT_SIZE_14, "class:" );
				szUIText0.SetTextColor( TCT_DEFAULT_GREEN );
				szUIText0.AddTextPiece( FONT_SIZE_14, "%s", pCurrWnd->GetParent()->GetParent()->GetClassNameEX() );
				szUIText0.SetTextColor( TCT_DEFAULT_WHITE );
				szUIText0.AddTextPiece( FONT_SIZE_14, ", " );
			}
			szUIText0.SetTextColor( TCT_DEFAULT_WHITE );
			szUIText0.AddTextPiece( FONT_SIZE_14, "id:" );
			szUIText0.SetTextColor( TCT_DEFAULT_GREEN );
			szUIText0.AddTextPiece( FONT_SIZE_14, "%d", pCurrWnd->GetParent()->GetParent()->GetID() );
		}

		szUIText1.SetTextColor( TCT_DEFAULT_WHITE );
		szUIText1.AddTextPiece( FONT_SIZE_14, "[+] " );
		if( strcmp( pCurrWnd->GetParent()->GetClassNameEX(), "" ) != 0 )
		{
			szUIText1.AddTextPiece( FONT_SIZE_14, "class:" );
			szUIText1.SetTextColor( TCT_DEFAULT_GREEN );
			szUIText1.AddTextPiece( FONT_SIZE_14, "%s", pCurrWnd->GetParent()->GetClassNameEX() );
			szUIText1.SetTextColor( TCT_DEFAULT_WHITE );
			szUIText1.AddTextPiece( FONT_SIZE_14, ", " );
		}
		szUIText1.SetTextColor( TCT_DEFAULT_WHITE );
		szUIText1.AddTextPiece( FONT_SIZE_14, "id:" );
		szUIText1.SetTextColor( TCT_DEFAULT_GREEN );
		szUIText1.AddTextPiece( FONT_SIZE_14, "%d", pCurrWnd->GetParent()->GetID() );
	}
	else
	{
		szUIText1.SetTextColor( TCT_DEFAULT_WHITE );
		szUIText1.AddTextPiece( FONT_SIZE_14, "[+] " );
		szUIText1.AddTextPiece( FONT_SIZE_14, "NULL" );
	}

	szUIText2.SetTextColor( TCT_DEFAULT_WHITE );
	szUIText2.AddTextPiece( FONT_SIZE_14, "[-] " );
	szUIText2.AddTextPiece( FONT_SIZE_14, "class:" );
	szUIText2.SetTextColor( TCT_DEFAULT_GREEN );
	szUIText2.AddTextPiece( FONT_SIZE_14, "%s", pCurrWnd->GetClassNameEX() );
	szUIText2.SetTextColor( TCT_DEFAULT_WHITE );
	szUIText2.AddTextPiece( FONT_SIZE_14, ", id:" );
	szUIText2.SetTextColor( TCT_DEFAULT_GREEN );
	szUIText2.AddTextPiece( FONT_SIZE_14, "%d", pCurrWnd->GetID() );

	int iOffset = max( szUIText1.GetFullWidth(), szUIText2.GetFullWidth() );
	iOffset = max( iOffset, szUIText0.GetFullWidth() );
	if( iXPos+iOffset/2 > g_App.GetCurWidth() )
		iXPos = g_App.GetCurWidth() - iOffset;
	else
		iXPos -= iOffset/2;

	iXPos = max( iXPos, 0 );
	iYPos = max( iYPos, 0 );
	int iYOffset = 0;
	if( !szUIText0.IsEmpty() )
	{
		szUIText0.PrintFullText( iXPos, iYPos + iYOffset, TAT_LEFT );
		iYOffset += 20;
	}
	szUIText1.PrintFullText( iXPos, iYPos + iYOffset, TAT_LEFT );
	iYOffset += 20;
	szUIText2.PrintFullText( iXPos, iYPos + iYOffset, TAT_LEFT );
}

//void ioUIGuide::RenderXMLName( ioWnd *pCurrWnd, int iXPos, int iYPos )
//{
//	if( !pCurrWnd )
//		return;
//
//	ioComplexStringPrinter szUIText0;
//	ioComplexStringPrinter szUIText1;
//	ioComplexStringPrinter szUIText2;
//	szUIText0.SetTextStyle( TS_OUTLINE_FULL_3X );
//	szUIText0.SetBkColor( TCT_DEFAULT_BLACK );
//	szUIText1.SetTextStyle( TS_OUTLINE_FULL_3X );
//	szUIText1.SetBkColor( TCT_DEFAULT_BLACK );
//	szUIText2.SetTextStyle( TS_OUTLINE_FULL_3X );
//	szUIText2.SetBkColor( TCT_DEFAULT_BLACK );
//
//	ioHashString szXMLName;
//
//	if( pCurrWnd->GetParent() )
//	{
//		if( pCurrWnd->GetParent()->GetParent() )
//		{
//			szXMLName = pCurrWnd->GetParent()->GetParent()->GetXMLName();
//			if( szXMLName.IsEmpty() )
//				szXMLName = "None";
//
//			szUIText0.SetTextColor( TCT_DEFAULT_WHITE );
//			szUIText0.AddTextPiece( FONT_SIZE_14, "[+] xml:" );
//			szUIText0.SetTextColor( TCT_DEFAULT_GREEN );
//			szUIText0.AddTextPiece( FONT_SIZE_14, szXMLName.c_str() );
//			szUIText0.SetTextColor( TCT_DEFAULT_WHITE );
//			szUIText0.AddTextPiece( FONT_SIZE_14, ", " );
//			szUIText0.SetTextColor( TCT_DEFAULT_WHITE );
//			szUIText0.AddTextPiece( FONT_SIZE_14, "id:" );
//			szUIText0.SetTextColor( TCT_DEFAULT_GREEN );
//			szUIText0.AddTextPiece( FONT_SIZE_14, "%1", pCurrWnd->GetParent()->GetParent()->GetID() );
//		}
//
//		szXMLName = pCurrWnd->GetParent()->GetXMLName();
//		if( szXMLName.IsEmpty() )
//			szXMLName = "None";
//		szUIText1.SetTextColor( TCT_DEFAULT_WHITE );
//		szUIText1.AddTextPiece( FONT_SIZE_14, "[+] xml:" );
//		szUIText1.SetTextColor( TCT_DEFAULT_GREEN );
//		szUIText1.AddTextPiece( FONT_SIZE_14, szXMLName.c_str() );
//		szUIText1.SetTextColor( TCT_DEFAULT_WHITE );
//		szUIText1.AddTextPiece( FONT_SIZE_14, ", " );
//		szUIText1.SetTextColor( TCT_DEFAULT_WHITE );
//		szUIText1.AddTextPiece( FONT_SIZE_14, "id:" );
//		szUIText1.SetTextColor( TCT_DEFAULT_GREEN );
//		szUIText1.AddTextPiece( FONT_SIZE_14, "%1", pCurrWnd->GetParent()->GetID() );
//	}
//	else
//	{
//		szUIText1.SetTextColor( TCT_DEFAULT_WHITE );
//		szUIText1.AddTextPiece( FONT_SIZE_14, " [+] " );
//		szUIText1.AddTextPiece( FONT_SIZE_14, "NULL" );
//	}
//
//	szXMLName = pCurrWnd->GetXMLName();
//	if( szXMLName.IsEmpty() )
//		szXMLName = "None";
//
//	szUIText2.SetTextColor( TCT_DEFAULT_WHITE );
//	szUIText2.AddTextPiece( FONT_SIZE_14, "[-] xml:" );
//	szUIText2.SetTextColor( TCT_DEFAULT_GREEN );
//	szUIText2.AddTextPiece( FONT_SIZE_14, szXMLName.c_str() );
//	szUIText2.SetTextColor( TCT_DEFAULT_WHITE );
//	szUIText2.AddTextPiece( FONT_SIZE_14, ", id:" );
//	szUIText2.SetTextColor( TCT_DEFAULT_GREEN );
//	szUIText2.AddTextPiece( FONT_SIZE_14, "%1", pCurrWnd->GetID() );
//
//	int iOffset = max( szUIText1.GetFullWidth(), szUIText2.GetFullWidth() );
//	iOffset = max( iOffset, szUIText0.GetFullWidth() );
//	if( iXPos+iOffset/2 > g_App.GetCurWidth() )
//		iXPos = g_App.GetCurWidth() - iOffset;
//	else
//		iXPos -= iOffset/2;
//
//	iXPos = max( iXPos, 0 );
//	iYPos = max( iYPos, 0 );
//	int iYOffset = 0;
//	if( !szUIText0.IsEmpty() )
//	{
//		szUIText0.PrintFullText( iXPos, iYPos + iYOffset, TAT_LEFT );
//		iYOffset += 20;
//	}
//	szUIText1.PrintFullText( iXPos, iYPos + iYOffset, TAT_LEFT );
//	iYOffset += 20;
//	szUIText2.PrintFullText( iXPos, iYPos + iYOffset, TAT_LEFT );
//}
