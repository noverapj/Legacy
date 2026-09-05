#include "StdAfx.h"
#include "ExcavationLevelUpWnd.h"
#include "../ioExcavationManager.h"


ExcavationLevelUpWnd::ExcavationLevelUpWnd(void)
{
	m_iLevel = 0;
	m_bKitUp = false;
	m_pIcon = NULL;
	m_pIconBack = NULL;
}


ExcavationLevelUpWnd::~ExcavationLevelUpWnd(void)
{
	SAFEDELETE( m_pIcon );
	SAFEDELETE( m_pIconBack );
}

void ExcavationLevelUpWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_Text[TEXT_RESULT_LEVEL_UP_1] = xElement.GetStringAttribute_e( "TextExcavation" );
	m_Text[TEXT_RESULT_LEVEL_UP_2] = xElement.GetStringAttribute_e( "TextLevel" );
	m_Text[TEXT_RESULT_LEVEL_UP_3] = xElement.GetStringAttribute_e( "TextLevelUp" );
	m_Text[TEXT_RESULT_KIT_UP] = xElement.GetStringAttribute_e( "TextKitLevelUp" );
	m_szUpSound = xElement.GetStringAttribute_e( "Level_Up_Sound" );
}

void ExcavationLevelUpWnd::iwm_show()
{
	SetIcon();

	m_LevelUpPrinter.ClearList();
	m_KitUpPrinter.ClearList();

	m_LevelUpPrinter.SetTextStyle( TS_NORMAL );
	//m_PagePrinter.SetBkColor( 0, 0, 0 );
	m_LevelUpPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	m_LevelUpPrinter.AddTextPiece( FONT_SIZE_16, m_Text[TEXT_RESULT_LEVEL_UP_1].c_str() );
	m_LevelUpPrinter.SetTextColor( TCT_DEFAULT_GREEN );
	m_LevelUpPrinter.AddTextPiece( FONT_SIZE_16, m_Text[TEXT_RESULT_LEVEL_UP_2].c_str(), m_iLevel );
	m_LevelUpPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	m_LevelUpPrinter.AddTextPiece( FONT_SIZE_16, m_Text[TEXT_RESULT_LEVEL_UP_3].c_str() );

	SetFrameSize( m_bKitUp );

	if( m_bKitUp )
	{
		m_KitUpPrinter.SetTextStyle( TS_NORMAL );
		m_KitUpPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
		m_KitUpPrinter.AddTextPiece( FONT_SIZE_14, m_Text[TEXT_RESULT_KIT_UP].c_str() );

		//m_bKitUp = false;
	}

	ShowChildWnd( ID_ALPHA_WND );

	if( !m_szUpSound.IsEmpty() )
	{
		g_SoundMgr.PlaySound( m_szUpSound );
	}
}

void ExcavationLevelUpWnd::iwm_hide()
{
	HideChildWnd( ID_ALPHA_WND );

	if( !m_szUpSound.IsEmpty() )
	{
		g_SoundMgr.StopSound( m_szUpSound, 0 );
	}
}

bool ExcavationLevelUpWnd::iwm_esc()
{
	if( g_App.IsMouseBusy() )
		return false;

	ioWnd *pWnd = FindChildWnd( ID_CLOSE );
	if( pWnd )
	{
		if( !pWnd->HasWndStyle( IWS_INACTIVE ) )
		{
			HideWnd();
			return true;
		}
	}

	return false;
}

void ExcavationLevelUpWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	if(!pWnd) return;

	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
	case ID_CLOSE:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;
	}
}

void ExcavationLevelUpWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_pIconBack )
	{
		m_pIconBack->Render( iXPos + 156, iYPos + 95, UI_RENDER_MULTIPLY, TFO_BILINEAR );
	}

	if( m_pIcon )
	{
		m_pIcon->Render( iXPos + 156, iYPos + 95, UI_RENDER_NORMAL, TFO_BILINEAR );
	}

	enum 
	{ 
		LEVEL_UP_X = 160,
		LEVEL_UP_Y = 144,
		KIT_UP_X = 160,
		KIT_UP_Y = 186
	};

	m_LevelUpPrinter.PrintFullText( iXPos + LEVEL_UP_X, iYPos + LEVEL_UP_Y, TAT_CENTER );
	m_KitUpPrinter.PrintFullText( iXPos + KIT_UP_X, iYPos + KIT_UP_Y, TAT_CENTER );
}

void ExcavationLevelUpWnd::SetLevel( int iLevel, bool bKitUp )
{
	m_iLevel = iLevel;
	m_bKitUp = bKitUp;
}

void ExcavationLevelUpWnd::SetIcon()
{
	SAFEDELETE( m_pIcon );

	ioHashString szIconName;
	if( g_ExcavationMgr.GetUseIconName( szIconName ) )
	{
		m_pIcon     = g_UIImageSetMgr.CreateImageByFullName( szIconName );
	}

	if( !m_pIconBack )
	{
		m_pIconBack	= g_UIImageSetMgr.CreateImageByFullName( "BaseImage002#iconeffect_3" );
	}
}

void ExcavationLevelUpWnd::SetFrameSize( bool pKitUp )
{
	if( pKitUp )
	{
		SetSize( WINDOW_WIDTH, WINDOW_HEIGHT_KITUP );

		m_pSelectedFrm->SetSize( WINDOW_WIDTH, WINDOW_HEIGHT_KITUP );
		m_pMultiplyFrm->SetPosition( m_pMultiplyFrm->GetPosX(), BOTTOM_FRAME_KITUP_YPOS );

		ioWnd *pWnd = FindChildWnd( ID_CLOSE );
		if( pWnd )
			pWnd->SetPosY( BTN_CLOSE_KITUP_YPOS );

		pWnd = FindChildWnd( ID_ALPHA_WND );
		if( pWnd )
			pWnd->SetSize( WINDOW_WIDTH, WINDOW_HEIGHT_KITUP );
	}
	else
	{
		SetSize( WINDOW_WIDTH, WINDOW_HEIGHT_NORMAL );

		m_pSelectedFrm->SetSize( WINDOW_WIDTH, WINDOW_HEIGHT_NORMAL );
		m_pMultiplyFrm->SetPosition( m_pMultiplyFrm->GetPosX(), BOTTOM_FRAME_YPOS );

		ioWnd *pWnd = FindChildWnd( ID_CLOSE );
		if( pWnd )
			pWnd->SetPosY( BTN_CLOSE_YPOS );

		pWnd = FindChildWnd( ID_ALPHA_WND );
		if( pWnd )
			pWnd->SetSize( WINDOW_WIDTH, WINDOW_HEIGHT_NORMAL );
	}
}