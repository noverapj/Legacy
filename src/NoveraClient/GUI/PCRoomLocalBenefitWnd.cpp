#include "stdafx.h"

#include "../io3DEngine/ioWnd.h"
#include "../io3DEngine/ioUIRenderImage.h"
#include "../io3DEngine/ioXMLDocument.h"
#include "../io3DEngine/ioXMLElement.h"
#include "../io3DEngine/ioFontManager.h"
#include "../io3DEngine/ioUIRenderFrame.h"

#include ".\pcroomlocalbenefitwnd.h"
#include <strsafe.h>
#include "ioSP2GUIManager.h"
#include "..\Setting.h"
#include "..\TextColorTable.h"

PCRoomLocalBenefitWnd::PCRoomLocalBenefitWnd(void)
{
	m_pMarkGold   = NULL;
	m_pMarkSilver = NULL;
}

PCRoomLocalBenefitWnd::~PCRoomLocalBenefitWnd(void)
{
	SAFEDELETE( m_pMarkGold );
	SAFEDELETE( m_pMarkSilver );
	m_vText.clear();
}

void PCRoomLocalBenefitWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "MarkGold" )
	{
		SAFEDELETE( m_pMarkGold );
		m_pMarkGold = pImage;
	}
	else if( szType == "MarkSilver" )
	{
		SAFEDELETE( m_pMarkSilver );
		m_pMarkSilver = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}


void PCRoomLocalBenefitWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	for (int i = 0; i < MAX_TEXT_LINE ; i++)
	{
		char szName[MAX_PATH]="";
		StringCbPrintf( szName, sizeof( szName ), "Text%d", i+1 );
		ioHashString szText = xElement.GetStringAttribute( szName );
		if( szText.IsEmpty() )
			break;
		m_vText.push_back( szText );
	}
}

void PCRoomLocalBenefitWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_EXIT:
	case ID_OK:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;
	}
}

bool PCRoomLocalBenefitWnd::iwm_esc()
{
	ioWnd *pBtn = FindChildWnd( ID_OK );
	if( !pBtn ) return false;
	if( !pBtn->IsShow() ) return false;

	iwm_command( pBtn, IOBN_BTNUP, 0 );
	return true;
}

void PCRoomLocalBenefitWnd::iwm_show()
{
	enum { BASIC_HEIGHT = 203, LINE_SIZE = 20, BTN_GAP = 48, BOTTOM_HEIGHT = 65, };
	
	int iLine = m_vText.size();
	int iHeight = BASIC_HEIGHT + ( LINE_SIZE * iLine );
	SetSize( GetWidth(), iHeight );
	SetWndPos( (Setting::Width()/2) - GetWidth()/2, ((Setting::Height()/2) - GetHeight()/2) - 20 );

	ioWnd *pWnd = FindChildWnd( ID_OK );
	if( pWnd )
	{
		pWnd->SetWndPos( pWnd->GetXPos(), GetHeight() - BTN_GAP );
	}

	// ÇÏ´Ü¹Ù
	if( m_pMultiplyFrm )
	{
		m_pMultiplyFrm->SetSize( GetWidth(), BOTTOM_HEIGHT );
		m_pMultiplyFrm->SetPosition( 0, GetHeight() - m_pMultiplyFrm->GetHeight() );
	}
}

void PCRoomLocalBenefitWnd::OnRender()
{
	enum 
	{
		MARK_X_OFFSET = 16,
		MARK_Y_OFFSET = 55,

		TEXT_X_OFFSET = 17,
		TEXT_Y_OFFSET = 138,
		TEXT_LINE_GAP = 20,
	};

	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_pMarkSilver )
	{
		m_pMarkSilver->Render( iXPos + MARK_X_OFFSET, iYPos + MARK_Y_OFFSET );
	}

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );

	int iSize = m_vText.size();
	for (int i = 0; i < iSize; i++)
	{
		g_FontMgr.PrintText( iXPos + TEXT_X_OFFSET, iYPos + TEXT_Y_OFFSET + ( i*TEXT_LINE_GAP), FONT_SIZE_13, m_vText[i].c_str() );	
	}
}
