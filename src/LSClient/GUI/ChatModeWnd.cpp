

#include "StdAfx.h"

#include "../io3DEngine/ioWnd.h"
#include "../io3DEngine/ioFontManager.h"
#include "../io3DEngine/ioUIRenderImage.h"
#include "../io3DEngine/ioUIImageSetManager.h"
#include "../io3DEngine/ioXMLElement.h"
#include "../io3DEngine/ioSoundManager.h"
#include "../io3DEngine/iostringmanager.h"
#include "../ioVoiceChat/Fourier.h"

#include "../TextColorTable.h"
#include "../ioBaseChar.h"
#include "../ioMyInfo.h"
#include "../NetworkWrappingFunc.h"
#include "../WndID.h"
#include "../Setting.h"
#include "../ioApplication.h"
#include "../ChatMacroMgr.h"

#include "ioSP2GUIManager.h"
#include "ioTutorialManager.h"

#include "ChatModeWnd.h"

ChatModeWnd::ChatModeWnd()
{
}

ChatModeWnd::~ChatModeWnd()
{
}

bool ChatModeWnd::iwm_spacebar()
{
	if( !IsShow() ) return false;

	iwm_command( FindChildWnd( ID_CHANGE ), IOBN_BTNUP, 0 );
	return true;
}

void ChatModeWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
	case ID_CLOSE:
		if( cmd == IOBN_BTNUP )
			HideWnd();
		break;
	case ID_CHANGE:
		if( cmd == IOBN_BTNUP )
		{
			g_ChatMacroMgr.MacroProcess( STR(1), true );
			HideWnd();
		}
		break;		
	}
}

void ChatModeWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 151, iYPos + 144, FONT_SIZE_13, STR(1) );

	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( iXPos + 151, iYPos + 164, FONT_SIZE_13, STR(2) );
	g_FontMgr.PrintText( iXPos + 151, iYPos + 184, FONT_SIZE_13, STR(3) );
	g_FontMgr.PrintText( iXPos + 151, iYPos + 204, FONT_SIZE_13, STR(4) );
}