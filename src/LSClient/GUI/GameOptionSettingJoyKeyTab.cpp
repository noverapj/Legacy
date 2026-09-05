#include "StdAfx.h"

#include "../io3DEngine/InputBox.h"
#include "../io3DEngine/ioWnd.h"
#include "../io3DEngine/ioEdit.h"
#include "../io3DEngine/ioButton.h"
#include "../io3DEngine/ioScroll.h"
#include "../io3DEngine/ioUITitle.h"
#include "../io3Dengine/ioXMLDocument.h"
#include "../io3Dengine/ioXMLElement.h"
#include "../io3DEngine/ioUIRenderImage.h"
#include "../io3DEngine/ioFontManager.h"
#include "../io3DEngine/iostringmanager.h"
#include "../io3DEngine/ioMovingWnd.h"

#include "../WndID.h"
#include "../TextColorTable.h"
#include "../Setting.h"
#include "../DirectInput.h"
#include "../ioKeyManager.h"
#include "../ioApplication.h"
#include "../EtcHelpFunc.h"
#include "../ioPlayStage.h"
#include "../ioBaseChar.h"

#include "ioModeHelpManager.h"
#include "ioSP2GUIManager.h"

#include "GameOptionWnd.h"
#include "ioMessageBox.h"
#include "GameOptionSettingTab.h"

#include <shlobj.h>
#include <strsafe.h>
#include ".\gameoptionsettingjoykeytab.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SettingJoyBtn::SettingJoyBtn()
{
	m_eKeyType = CJoyKeyInfo::JTA_NONE;
	m_wKey     = KEY_NULL;
}

SettingJoyBtn::~SettingJoyBtn()
{

}

void SettingJoyBtn::SetKeyType( CJoyKeyInfo::KeyTypeArray eKeyType )
{
	m_eKeyType = eKeyType;
}

CJoyKeyInfo::KeyTypeArray SettingJoyBtn::GetKeyType()
{
	return m_eKeyType;
}

void SettingJoyBtn::SetSelect()
{
	if(m_pUITitle)
	{
		if(IsDisabled())
			m_pUITitle->SetAllColor(0xFFA7A7A7, 0xFF000000);
		else
			m_pUITitle->SetAllColor( 0xFF117222, 0xFF000000 );
		m_pUITitle->SetText(STR(1), "");

	}
}

void SettingJoyBtn::UnSelect( float fWidthCutSize )
{
	if(m_pUITitle)
	{
		if( IsDisabled() )
			m_pUITitle->SetAllColor(0xFFA7A7A7, 0xFF000000);
		else
			m_pUITitle->SetAllColor( 0xFF3A3A3A, 0xFF000000 );
		m_pUITitle->SetText( g_KeyManager.GetKeyText(m_wKey) , "");
		m_pUITitle->SetWidthCutSize( fWidthCutSize );
		m_pUITitle->SetPrintTextStyle( ioUITitle::PTS_WIDTHCUT );
	}
}

void SettingJoyBtn::SetKeyValue( WORD wKey )
{
	m_wKey = wKey;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SettingJoyKeyItemWnd::SettingJoyKeyItemWnd()
{
	m_eType = IT_NONE;
	
	m_pIconBack   = NULL;
	m_pIconJoypad = NULL;
}

SettingJoyKeyItemWnd::~SettingJoyKeyItemWnd()
{
	SAFEDELETE( m_pIconBack );
	SAFEDELETE( m_pIconJoypad );
}

void SettingJoyKeyItemWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "IconBack" )
	{
		SAFEDELETE( m_pIconBack );
		m_pIconBack = pImage;
	}
	else if( szType == "IconJoypad" )
	{
		SAFEDELETE( m_pIconJoypad );
		m_pIconJoypad = pImage;

	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void SettingJoyKeyItemWnd::SetItem( ItemType eItemType,
								    CJoyKeyInfo::KeyTypeArray eFirstKeyType,
									WORD wFirstKey )
{
	m_eType = eItemType;

	if( m_eType == IT_TITLE_BASIC   ||
		m_eType == IT_TITLE_SKILL   ||
		m_eType == IT_TITLE_SOLDIER )
	{
		HideChildWnd( ID_BIG_BTN );
		HideChildWnd( ID_SMALL_FIRST_BTN );
		HideChildWnd( ID_SMALL_SECOND_BTN );
	}
    // 방향 Second key 사용하지 않음 ( IT_UP ~ IT_RIGHT )
	else if( COMPARE(m_eType, IT_ATTACK, IT_JUMP+1) )
	{
		ShowChildWnd( ID_BIG_BTN );
		HideChildWnd( ID_SMALL_FIRST_BTN );
		HideChildWnd( ID_SMALL_SECOND_BTN );

		SettingJoyBtn *pBtn = dynamic_cast<SettingJoyBtn*> ( FindChildWnd(ID_BIG_BTN) );
		if(pBtn)
		{
			pBtn->SetKeyType(eFirstKeyType);
			pBtn->SetKeyValue(wFirstKey);
		}
	}
	else
	{
		ShowChildWnd( ID_BIG_BTN );
		HideChildWnd( ID_SMALL_FIRST_BTN );
		HideChildWnd( ID_SMALL_SECOND_BTN );

		SettingJoyBtn *pBtn = dynamic_cast<SettingJoyBtn*> ( FindChildWnd(ID_BIG_BTN) );
		if(pBtn)
		{
			pBtn->SetKeyType(eFirstKeyType);
			pBtn->SetKeyValue(wFirstKey);
		}
	}	

	SetHelpText();
}
void SettingJoyKeyItemWnd::UnSelect()
{
	for (int i = ID_SMALL_FIRST_BTN; i < ID_BIG_BTN + 1 ; i++)
	{
		SettingJoyBtn *pBtn = dynamic_cast<SettingJoyBtn*> ( FindChildWnd(i) );
		if( pBtn && pBtn->IsShow() )
		{
			if(i == ID_BIG_BTN )
				pBtn->UnSelect( 320.0f );
			else
				pBtn->UnSelect( 150.0f );
		}
	}
}

void SettingJoyKeyItemWnd::SetSelect( CJoyKeyInfo::KeyTypeArray eKeyType )
{
	for (int i = ID_SMALL_FIRST_BTN; i < ID_BIG_BTN + 1 ; i++)
	{
		SettingJoyBtn *pBtn = dynamic_cast<SettingJoyBtn*> ( FindChildWnd(i) );
		if( pBtn && pBtn->IsShow() )
		{	
			if( pBtn->GetKeyType() == eKeyType )
			{
				pBtn->SetSelect();
				break;
			}
		}
	}
}

void SettingJoyKeyItemWnd::OnRender()
{
	ioWnd::OnRender();

	if( m_eType == IT_TITLE_BASIC   ||
		m_eType == IT_TITLE_SKILL ||
		m_eType == IT_TITLE_SOLDIER )
	{

		// 타이틀 아이콘
		int iXPos = GetDerivedPosX();
		int iYPos = GetDerivedPosY();
		if( m_pIconBack )
		{
			m_pIconBack->SetScale( 0.30f );
			m_pIconBack->Render( iXPos + 13, iYPos + 12, UI_RENDER_MULTIPLY, TFO_BILINEAR );
		}
		if( m_pIconJoypad )
		{
			m_pIconJoypad->SetScale( 0.30f );
			m_pIconJoypad->Render( iXPos + 13, iYPos + 12, UI_RENDER_NORMAL, TFO_BILINEAR );
		}
	}
}

void SettingJoyKeyItemWnd::SetHelpText()
{
	if( !m_pUITitle ) return;

	if(m_eType == IT_TITLE_BASIC)
	{
		m_pUITitle->SetOffset( 27, 4 );
		m_pUITitle->SetSize( 15 );
		m_pUITitle->SetStyle( TS_NORMAL );
		m_pUITitle->SetText(STR(1), "");
		m_pUITitle->SetAllColor( 0xFF3A3A3A, 0xFF000000 );
	}
	else if(m_eType == IT_UP)
	{
		m_pUITitle->SetOffset( 4, 4 );
		m_pUITitle->SetSize(14);
		m_pUITitle->SetStyle( TS_NORMAL );
		m_pUITitle->SetText(STR(2), "");
		m_pUITitle->SetAllColor( 0xFF6E6E6E, 0xFF000000 );
	}
	else if(m_eType == IT_DOWN)
	{
		m_pUITitle->SetOffset( 4, 4 );
		m_pUITitle->SetSize(14);
		m_pUITitle->SetStyle( TS_NORMAL );
		m_pUITitle->SetText(STR(3), "");
		m_pUITitle->SetAllColor( 0xFF6E6E6E, 0xFF000000 );
	}
	else if(m_eType == IT_LEFT)
	{
		m_pUITitle->SetOffset( 4, 4 );
		m_pUITitle->SetSize(14);
		m_pUITitle->SetStyle( TS_NORMAL );
		m_pUITitle->SetText(STR(4), "");
		m_pUITitle->SetAllColor( 0xFF6E6E6E, 0xFF000000 );
	}
	else if(m_eType == IT_RIGHT)
	{
		m_pUITitle->SetOffset( 4, 4 );
		m_pUITitle->SetSize(14);
		m_pUITitle->SetStyle( TS_NORMAL );
		m_pUITitle->SetText(STR(5), "");
		m_pUITitle->SetAllColor( 0xFF6E6E6E, 0xFF000000 );
	}
	else if(m_eType == IT_ATTACK)
	{
		m_pUITitle->SetOffset( 4, 4 );
		m_pUITitle->SetSize(14);
		m_pUITitle->SetStyle( TS_NORMAL );
		m_pUITitle->SetText(STR(6), "");
		m_pUITitle->SetAllColor( 0xFF6E6E6E, 0xFF000000 );
	}
	else if(m_eType == IT_DEFENSE)
	{
		m_pUITitle->SetOffset( 4, 4 );
		m_pUITitle->SetSize(14);
		m_pUITitle->SetStyle( TS_NORMAL );
		m_pUITitle->SetText(STR(7), "");
		m_pUITitle->SetAllColor( 0xFF6E6E6E, 0xFF000000 );
	}
	else if(m_eType == IT_JUMP)
	{
		m_pUITitle->SetOffset( 4, 4 );
		m_pUITitle->SetSize(14);
		m_pUITitle->SetStyle( TS_NORMAL );
		m_pUITitle->SetText(STR(8), "");
		m_pUITitle->SetAllColor( 0xFF6E6E6E, 0xFF000000 );
	}
	else if(m_eType == IT_TITLE_SKILL)
	{
		m_pUITitle->SetOffset( 27, 4 );
		m_pUITitle->SetSize( 15 );
		m_pUITitle->SetStyle( TS_NORMAL );
		m_pUITitle->SetText(STR(9), "");
		m_pUITitle->SetAllColor( 0xFF3A3A3A, 0xFF000000 );
	}
	else if(m_eType == IT_CLOAK_SKILL)
	{
		m_pUITitle->SetOffset( 4, 4 );
		m_pUITitle->SetSize(14);
		m_pUITitle->SetStyle( TS_NORMAL );
		m_pUITitle->SetText( "AS", "");
		m_pUITitle->SetAllColor( 0xFF6E6E6E, 0xFF000000 );
	}
	else if(m_eType == IT_HELM_SKILL)
	{
		m_pUITitle->SetOffset( 4, 4 );
		m_pUITitle->SetSize(14);
		m_pUITitle->SetStyle( TS_NORMAL );
		m_pUITitle->SetText( "AD", "");
		m_pUITitle->SetAllColor( 0xFF6E6E6E, 0xFF000000 );
	}
	else if(m_eType == IT_ARMOR_SKILL)
	{
		m_pUITitle->SetOffset( 4, 4 );
		m_pUITitle->SetSize(14);
		m_pUITitle->SetStyle( TS_NORMAL );
		m_pUITitle->SetText( "ASD", "");
		m_pUITitle->SetAllColor( 0xFF6E6E6E, 0xFF000000 );
	}
	else if(m_eType == IT_WEAPON_SKILL)
	{
		m_pUITitle->SetOffset( 4, 4 );
		m_pUITitle->SetSize(14);
		m_pUITitle->SetStyle( TS_NORMAL );
		m_pUITitle->SetText( "SD", "");
		m_pUITitle->SetAllColor( 0xFF6E6E6E, 0xFF000000 );
	}
	else if(m_eType == IT_TITLE_SOLDIER)
	{
		m_pUITitle->SetOffset( 27, 4 );
		m_pUITitle->SetSize( 15 );
		m_pUITitle->SetStyle( TS_NORMAL );
		m_pUITitle->SetText(STR(10), "");
		m_pUITitle->SetAllColor( 0xFF3A3A3A, 0xFF000000 );
	}
	else if(m_eType == IT_SOLDIER1)
	{
		m_pUITitle->SetOffset( 4, 4 );
		m_pUITitle->SetSize(14);
		m_pUITitle->SetStyle( TS_NORMAL );
		m_pUITitle->SetText(STR(11), "");
		m_pUITitle->SetAllColor( 0xFF6E6E6E, 0xFF000000 );
	}
	else if(m_eType == IT_SOLDIER2)
	{
		m_pUITitle->SetOffset( 4, 4 );
		m_pUITitle->SetSize(14);
		m_pUITitle->SetStyle( TS_NORMAL );
		m_pUITitle->SetText(STR(12), "");
		m_pUITitle->SetAllColor( 0xFF6E6E6E, 0xFF000000 );
	}
	else if(m_eType == IT_SOLDIER3)
	{
		m_pUITitle->SetOffset( 4, 4 );
		m_pUITitle->SetSize(14);
		m_pUITitle->SetStyle( TS_NORMAL );
		m_pUITitle->SetText(STR(13), "");
		m_pUITitle->SetAllColor( 0xFF6E6E6E, 0xFF000000 );
	}
	else if(m_eType == IT_SOLDIER4)
	{
		m_pUITitle->SetOffset( 4, 4 );
		m_pUITitle->SetSize(14);
		m_pUITitle->SetStyle( TS_NORMAL );
		m_pUITitle->SetText(STR(14), "");
		m_pUITitle->SetAllColor( 0xFF6E6E6E, 0xFF000000 );
	}
	else if(m_eType == IT_SOLDIER5)
	{
		m_pUITitle->SetOffset( 4, 4 );
		m_pUITitle->SetSize(14);
		m_pUITitle->SetStyle( TS_NORMAL );
		m_pUITitle->SetText(STR(15), "");
		m_pUITitle->SetAllColor( 0xFF6E6E6E, 0xFF000000 );
	}
	else if(m_eType == IT_SOLDIER6)
	{
		m_pUITitle->SetOffset( 4, 4 );
		m_pUITitle->SetSize(14);
		m_pUITitle->SetStyle( TS_NORMAL );
		m_pUITitle->SetText(STR(16), "");
		m_pUITitle->SetAllColor( 0xFF6E6E6E, 0xFF000000 );
	}
	else if(m_eType == IT_SOLDIER7)
	{
		m_pUITitle->SetOffset( 4, 4 );
		m_pUITitle->SetSize(14);
		m_pUITitle->SetStyle( TS_NORMAL );
		m_pUITitle->SetText(STR(17), "");
		m_pUITitle->SetAllColor( 0xFF6E6E6E, 0xFF000000 );
	}
	else if(m_eType == IT_SOLDIER8)
	{
		m_pUITitle->SetOffset( 4, 4 );
		m_pUITitle->SetSize(14);
		m_pUITitle->SetStyle( TS_NORMAL );
		m_pUITitle->SetText(STR(18), "");
		m_pUITitle->SetAllColor( 0xFF6E6E6E, 0xFF000000 );
	}
	else if(m_eType == IT_SOLDIER9)
	{
		m_pUITitle->SetOffset( 4, 4 );
		m_pUITitle->SetSize(14);
		m_pUITitle->SetStyle( TS_NORMAL );
		m_pUITitle->SetText(STR(19), "");
		m_pUITitle->SetAllColor( 0xFF6E6E6E, 0xFF000000 );
	}
	else if(m_eType == IT_SOLDIER10)
	{
		m_pUITitle->SetOffset( 4, 4 );
		m_pUITitle->SetSize(14);
		m_pUITitle->SetStyle( TS_NORMAL );
		m_pUITitle->SetText(STR(20), "");
		m_pUITitle->SetAllColor( 0xFF6E6E6E, 0xFF000000 );
	}
	else if(m_eType == IT_SOLDIER11)
	{
		m_pUITitle->SetOffset( 4, 4 );
		m_pUITitle->SetSize(14);
		m_pUITitle->SetStyle( TS_NORMAL );
		m_pUITitle->SetText(STR(21), "");
		m_pUITitle->SetAllColor( 0xFF6E6E6E, 0xFF000000 );
	}
	else if(m_eType == IT_SOLDIER12)
	{
		m_pUITitle->SetOffset( 4, 4 );
		m_pUITitle->SetSize(14);
		m_pUITitle->SetStyle( TS_NORMAL );
		m_pUITitle->SetText(STR(22), "");
		m_pUITitle->SetAllColor( 0xFF6E6E6E, 0xFF000000 );
	}
	else if(m_eType == IT_SOLDIER13)
	{
		m_pUITitle->SetOffset( 4, 4 );
		m_pUITitle->SetSize(14);
		m_pUITitle->SetStyle( TS_NORMAL );
		m_pUITitle->SetText( STR(23), "");
		m_pUITitle->SetAllColor( 0xFF6E6E6E, 0xFF000000 );
	}
	else if(m_eType == IT_SOLDIER14)
	{
		m_pUITitle->SetOffset( 4, 4 );
		m_pUITitle->SetSize(14);
		m_pUITitle->SetStyle( TS_NORMAL );
		m_pUITitle->SetText( STR(24), "");
		m_pUITitle->SetAllColor( 0xFF6E6E6E, 0xFF000000 );
	}
	else if(m_eType == IT_SOLDIER15)
	{
		m_pUITitle->SetOffset( 4, 4 );
		m_pUITitle->SetSize(14);
		m_pUITitle->SetStyle( TS_NORMAL );
		m_pUITitle->SetText( STR(25), "");
		m_pUITitle->SetAllColor( 0xFF6E6E6E, 0xFF000000 );
	}
	else if(m_eType == IT_SOLDIER16)
	{
		m_pUITitle->SetOffset( 4, 4 );
		m_pUITitle->SetSize(14);
		m_pUITitle->SetStyle( TS_NORMAL );
		m_pUITitle->SetText( STR(26), "");
		m_pUITitle->SetAllColor( 0xFF6E6E6E, 0xFF000000 );
	}
	else if(m_eType == IT_SOLDIER17)
	{
		m_pUITitle->SetOffset( 4, 4 );
		m_pUITitle->SetSize(14);
		m_pUITitle->SetStyle( TS_NORMAL );
		m_pUITitle->SetText( STR(27), "");
		m_pUITitle->SetAllColor( 0xFF6E6E6E, 0xFF000000 );
	}
	else if(m_eType == IT_SOLDIER18)
	{
		m_pUITitle->SetOffset( 4, 4 );
		m_pUITitle->SetSize(14);
		m_pUITitle->SetStyle( TS_NORMAL );
		m_pUITitle->SetText( STR(28), "");
		m_pUITitle->SetAllColor( 0xFF6E6E6E, 0xFF000000 );
	}
	else if(m_eType == IT_SOLDIER19)
	{
		m_pUITitle->SetOffset( 4, 4 );
		m_pUITitle->SetSize(14);
		m_pUITitle->SetStyle( TS_NORMAL );
		m_pUITitle->SetText( STR(29), "");
		m_pUITitle->SetAllColor( 0xFF6E6E6E, 0xFF000000 );
	}
	else if(m_eType == IT_SOLDIER20)
	{
		m_pUITitle->SetOffset( 4, 4 );
		m_pUITitle->SetSize(14);
		m_pUITitle->SetStyle( TS_NORMAL );
		m_pUITitle->SetText( STR(30), "");
		m_pUITitle->SetAllColor( 0xFF6E6E6E, 0xFF000000 );
	}
	else if(m_eType == IT_SOLDIER21)
	{
		m_pUITitle->SetOffset( 4, 4 );
		m_pUITitle->SetSize(14);
		m_pUITitle->SetStyle( TS_NORMAL );
		m_pUITitle->SetText( STR(31), "");
		m_pUITitle->SetAllColor( 0xFF6E6E6E, 0xFF000000 );
	}
	else if(m_eType == IT_SOLDIER22)
	{
		m_pUITitle->SetOffset( 4, 4 );
		m_pUITitle->SetSize(14);
		m_pUITitle->SetStyle( TS_NORMAL );
		m_pUITitle->SetText( STR(32), "");
		m_pUITitle->SetAllColor( 0xFF6E6E6E, 0xFF000000 );
	}
	else if(m_eType == IT_SOLDIER23)
	{
		m_pUITitle->SetOffset( 4, 4 );
		m_pUITitle->SetSize(14);
		m_pUITitle->SetStyle( TS_NORMAL );
		m_pUITitle->SetText( STR(33), "");
		m_pUITitle->SetAllColor( 0xFF6E6E6E, 0xFF000000 );
	}
	else if(m_eType == IT_SOLDIER24)
	{
		m_pUITitle->SetOffset( 4, 4 );
		m_pUITitle->SetSize(14);
		m_pUITitle->SetStyle( TS_NORMAL );
		m_pUITitle->SetText( STR(34), "");
		m_pUITitle->SetAllColor( 0xFF6E6E6E, 0xFF000000 );
	}
	else if(m_eType == IT_SOLDIER25)
	{
		m_pUITitle->SetOffset( 4, 4 );
		m_pUITitle->SetSize(14);
		m_pUITitle->SetStyle( TS_NORMAL );
		m_pUITitle->SetText( STR(35), "");
		m_pUITitle->SetAllColor( 0xFF6E6E6E, 0xFF000000 );
	}
	else if(m_eType == IT_SOLDIER26)
	{
		m_pUITitle->SetOffset( 4, 4 );
		m_pUITitle->SetSize(14);
		m_pUITitle->SetStyle( TS_NORMAL );
		m_pUITitle->SetText( STR(36), "");
		m_pUITitle->SetAllColor( 0xFF6E6E6E, 0xFF000000 );
	}
	else if(m_eType == IT_SOLDIER27)
	{
		m_pUITitle->SetOffset( 4, 4 );
		m_pUITitle->SetSize(14);
		m_pUITitle->SetStyle( TS_NORMAL );
		m_pUITitle->SetText( STR(37), "");
		m_pUITitle->SetAllColor( 0xFF6E6E6E, 0xFF000000 );
	}
	else if(m_eType == IT_SOLDIER28)
	{
		m_pUITitle->SetOffset( 4, 4 );
		m_pUITitle->SetSize(14);
		m_pUITitle->SetStyle( TS_NORMAL );
		m_pUITitle->SetText( STR(38), "");
		m_pUITitle->SetAllColor( 0xFF6E6E6E, 0xFF000000 );
	}
	else if(m_eType == IT_SOLDIER29)
	{
		m_pUITitle->SetOffset( 4, 4 );
		m_pUITitle->SetSize(14);
		m_pUITitle->SetStyle( TS_NORMAL );
		m_pUITitle->SetText( STR(39), "");
		m_pUITitle->SetAllColor( 0xFF6E6E6E, 0xFF000000 );
	}
	else if(m_eType == IT_SOLDIER30)
	{
		m_pUITitle->SetOffset( 4, 4 );
		m_pUITitle->SetSize(14);
		m_pUITitle->SetStyle( TS_NORMAL );
		m_pUITitle->SetText( STR(40), "");
		m_pUITitle->SetAllColor( 0xFF6E6E6E, 0xFF000000 );
	}
	else if(m_eType == IT_SOLDIER31)
	{
		m_pUITitle->SetOffset( 4, 4 );
		m_pUITitle->SetSize(14);
		m_pUITitle->SetStyle( TS_NORMAL );
		m_pUITitle->SetText( STR(41), "");
		m_pUITitle->SetAllColor( 0xFF6E6E6E, 0xFF000000 );
	}
	else if(m_eType == IT_SOLDIER32)
	{
		m_pUITitle->SetOffset( 4, 4 );
		m_pUITitle->SetSize(14);
		m_pUITitle->SetStyle( TS_NORMAL );
		m_pUITitle->SetText( STR(42), "");
		m_pUITitle->SetAllColor( 0xFF6E6E6E, 0xFF000000 );
	}
	else if(m_eType == IT_SOLDIER33)
	{
		m_pUITitle->SetOffset( 4, 4 );
		m_pUITitle->SetSize(14);
		m_pUITitle->SetStyle( TS_NORMAL );
		m_pUITitle->SetText( STR(43), "");
		m_pUITitle->SetAllColor( 0xFF6E6E6E, 0xFF000000 );
	}
	else if(m_eType == IT_SOLDIER34)
	{
		m_pUITitle->SetOffset( 4, 4 );
		m_pUITitle->SetSize(14);
		m_pUITitle->SetStyle( TS_NORMAL );
		m_pUITitle->SetText( STR(44), "");
		m_pUITitle->SetAllColor( 0xFF6E6E6E, 0xFF000000 );
	}
	else if(m_eType == IT_SOLDIER35)
	{
		m_pUITitle->SetOffset( 4, 4 );
		m_pUITitle->SetSize(14);
		m_pUITitle->SetStyle( TS_NORMAL );
		m_pUITitle->SetText( STR(45), "");
		m_pUITitle->SetAllColor( 0xFF6E6E6E, 0xFF000000 );
	}
	else if(m_eType == IT_SOLDIER36)
	{
		m_pUITitle->SetOffset( 4, 4 );
		m_pUITitle->SetSize(14);
		m_pUITitle->SetStyle( TS_NORMAL );
		m_pUITitle->SetText( STR(46), "");
		m_pUITitle->SetAllColor( 0xFF6E6E6E, 0xFF000000 );
	}
}

void SettingJoyKeyItemWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_BIG_BTN:
	case ID_SMALL_FIRST_BTN:
	case ID_SMALL_SECOND_BTN:
		if( cmd == IOBN_BTNUP )
		{
			if( GetParent() )
			{
				SettingJoyBtn *pBtn = dynamic_cast<SettingJoyBtn*> (pWnd);
				if(pBtn)
				{
					GetParent()->iwm_command( this , cmd, pBtn->GetKeyType());
				}
			}
		}
		break;
	}
}

void SettingJoyKeyItemWnd::SetActive()
{
	m_dwStyle &= ~IWS_INACTIVE;

	SetChildActive(ID_SMALL_FIRST_BTN);
	SetChildActive(ID_SMALL_SECOND_BTN);
	SetChildActive(ID_BIG_BTN);
}

void SettingJoyKeyItemWnd::SetInActive()
{
	m_dwStyle |= IWS_INACTIVE;

	SetChildInActive(ID_SMALL_FIRST_BTN);
	SetChildInActive(ID_SMALL_SECOND_BTN);
	SetChildInActive(ID_BIG_BTN);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioPlayStage *GameOptionSettingJoyKeyTab::m_pPlayStage = NULL;
GameOptionSettingJoyKeyTab::GameOptionSettingJoyKeyTab()
{
	m_pIconBack	  = NULL;
	m_pIconJoypad = NULL;

	m_eSelectKeyType = CJoyKeyInfo::JTA_NONE;
	m_iCurPage = 0;
	m_vWholeItemType.reserve(SettingJoyKeyItemWnd::IT_MAX);
	m_KeyInfo.Load();

	for (int i = 0; i < SettingJoyKeyItemWnd::IT_MAX ; i++)
	{
		m_vWholeItemType.push_back((SettingJoyKeyItemWnd::ItemType)i);
	}
}

GameOptionSettingJoyKeyTab::~GameOptionSettingJoyKeyTab()
{
	SAFEDELETE( m_pIconBack );
	SAFEDELETE( m_pIconJoypad );
	m_vWholeItemType.clear();
}

void GameOptionSettingJoyKeyTab::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "IconBack" )
	{
		SAFEDELETE( m_pIconBack );
		m_pIconBack = pImage;
	}
	else if( szType == "IconJoypad" )
	{
		SAFEDELETE( m_pIconJoypad );
		m_pIconJoypad = pImage;

	}
	else
	{
		GameOptionBasicWnd::AddRenderImage( szType, pImage );
	}
}

void GameOptionSettingJoyKeyTab::iwm_show()
{
	GameOptionBasicWnd::iwm_show();

	UpdateByCurrentOption();
	CheckUseJoyPad();

	UpdateSetPage( m_iCurPage );
	ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
	if( pScroll )
	{
		pScroll->SetScrollRange( 0, SettingJoyKeyItemWnd::IT_MAX );
		pScroll->SetScrollPos( m_iCurPage );
	}
	SetChildInActive( ID_APPLY );
}

void GameOptionSettingJoyKeyTab::iwm_hide()
{
	m_eSelectKeyType = CJoyKeyInfo::JTA_NONE;
}

void GameOptionSettingJoyKeyTab::iwm_vscroll( DWORD dwID, int curPos )
{
	UpdateSetPage( curPos );
}

void GameOptionSettingJoyKeyTab::iwm_wheel( int zDelta )
{
	if( !IsShow() )
		ioWnd::iwm_wheel( zDelta );
	else
	{ 
		// 휠 이벤트 처리를 했다면 부모로 이벤트를 날리지 않는다.
		if( zDelta == WHEEL_DELTA )
		{
			ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
			if( pScroll )
				pScroll->SetScrollPos( m_iCurPage - 1 );
		}
		else if( zDelta == -WHEEL_DELTA )
		{
			ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
			if( pScroll )
				pScroll->SetScrollPos( m_iCurPage + 1 );
		}
	}
}

void GameOptionSettingJoyKeyTab::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_SETTING_KEY_ITEM_WND1:
	case ID_SETTING_KEY_ITEM_WND2:
	case ID_SETTING_KEY_ITEM_WND3:
	case ID_SETTING_KEY_ITEM_WND4:
	case ID_SETTING_KEY_ITEM_WND5:
	case ID_SETTING_KEY_ITEM_WND6:
	case ID_SETTING_KEY_ITEM_WND7:
	case ID_SETTING_KEY_ITEM_WND8:
	case ID_SETTING_KEY_ITEM_WND9:
	case ID_SETTING_KEY_ITEM_WND10:
	case ID_SETTING_KEY_ITEM_WND11:
		if( cmd == IOBN_BTNUP )
		{
			m_eSelectKeyType = (CJoyKeyInfo::KeyTypeArray) param;
			SetSelect( m_eSelectKeyType );
			SetChildActive( ID_APPLY );
		}
		break;
	case ID_USE_CHECK:
	case ID_UNUSED_CHECK:
		if( cmd == IOBN_BTNDOWN )
		{
			SetUseJoyPad( dwID );
			SetChildActive( ID_APPLY );
		}
		break;
	case ID_SAVE:
		if( cmd == IOBN_BTNUP )
		{
			Save();
			if( GetParent() )
				GetParent()->HideWnd();
		}
		break;
	case ID_CANCEL:
		if( cmd == IOBN_BTNUP )
		{
			if( GetParent() )
				GetParent()->HideWnd();
		}
		break;
	case ID_APPLY:
		if( cmd == IOBN_BTNUP )
		{
			Save();
			UpdateByCurrentOption();
			CheckUseJoyPad();
			UpdateSetPage( m_iCurPage );
			SetChildInActive( ID_APPLY );
		}
		break;
	case ID_DEFAULT:
		if( cmd == IOBN_BTNUP )
		{
			m_CurrentOption.m_bUseJoyPad = false;
			UpdateByCurrentOption();

			CheckUseJoyPad();

			g_KeyManager.GetJoyDefault( m_KeyInfo );
			UpdateSetPage( m_iCurPage );
		}
		break;
	}
}

void GameOptionSettingJoyKeyTab::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;
	if( m_eSelectKeyType == CJoyKeyInfo::JTA_NONE ) return;

	// 조이패드 값만 받는다
	for( WORD i=KEY_LSTICKLEFT; i<KEY_NUMKEYS; i++)
	{
		if( g_Input.IsKeyPress( i ) )
		{
			ChangeKeyValue( i );
			break;
		}
	}

	if( g_Input.IsKeyPress( KEY_NULL ) )
		ChangeKeyValue( KEY_NULL );

	if( g_Input.IsKeyPress( KEY_ENTER ) )
		ChangeKeyValue( KEY_ENTER );

	if( g_Input.IsKeyPress( KEY_ESCAPE ) )
		ChangeKeyValue( KEY_ESCAPE );

	if( g_Input.IsKeyPress( KEY_LWIN ) )
		ChangeKeyValue( KEY_LWIN );

	if( g_Input.IsKeyPress( KEY_RWIN ) )
		ChangeKeyValue( KEY_RWIN );
}

void GameOptionSettingJoyKeyTab::UpdateSetPage( int iNewPage )
{
	m_iCurPage = iNewPage;
	m_eSelectKeyType = CJoyKeyInfo::JTA_NONE;

	for (int i = 0; i < MAX_SHOW_ITEM_WND ; i++)
	{
		SettingJoyKeyItemWnd *pChild = dynamic_cast<SettingJoyKeyItemWnd*> ( FindChildWnd( ID_SETTING_KEY_ITEM_WND1 + i ) );
		if(pChild)
		{
			SettingJoyKeyItemWnd::ItemType eItemType = GetItemType( m_iCurPage + i);
			CJoyKeyInfo::KeyTypeArray eFirsKeyType = CJoyKeyInfo::JTA_NONE;
			GetKeyType( eItemType, eFirsKeyType );
			pChild->SetItem(eItemType,
				            eFirsKeyType,
							m_KeyInfo.GetKey(eFirsKeyType) );
			pChild->UnSelect();
		}
	}
}

SettingJoyKeyItemWnd::ItemType GameOptionSettingJoyKeyTab::GetItemType( int iPage )
{
	if( !COMPARE(iPage, 0, (int) m_vWholeItemType.size()) )
		return SettingJoyKeyItemWnd::IT_NONE;

	return m_vWholeItemType[iPage];
}

void GameOptionSettingJoyKeyTab::GetKeyType( IN SettingJoyKeyItemWnd::ItemType eItemType, 
											 OUT CJoyKeyInfo::KeyTypeArray &reFirstKeyType )
{
	reFirstKeyType = CJoyKeyInfo::JTA_NONE;
	
	switch( eItemType )
	{
	case SettingJoyKeyItemWnd::IT_UP:
		reFirstKeyType  = CJoyKeyInfo::JTA_UP;
		break;
	case SettingJoyKeyItemWnd::IT_DOWN:
		reFirstKeyType  = CJoyKeyInfo::JTA_DOWN;
		break;
	case SettingJoyKeyItemWnd::IT_LEFT:
		reFirstKeyType  = CJoyKeyInfo::JTA_LEFT;
	    break;
	case SettingJoyKeyItemWnd::IT_RIGHT:
		reFirstKeyType  = CJoyKeyInfo::JTA_RIGHT;
	    break;
	case SettingJoyKeyItemWnd::IT_ATTACK:
		reFirstKeyType  = CJoyKeyInfo::JTA_ATTACK;
		break;
	case SettingJoyKeyItemWnd::IT_DEFENSE:
		reFirstKeyType  = CJoyKeyInfo::JTA_DEFENSE;
		break;
	case SettingJoyKeyItemWnd::IT_JUMP:
		reFirstKeyType  = CJoyKeyInfo::JTA_JUMP;
	    break;
	case SettingJoyKeyItemWnd::IT_CLOAK_SKILL:
		reFirstKeyType = CJoyKeyInfo::JTA_CLOAK_SKILL;
		break;
	case SettingJoyKeyItemWnd::IT_HELM_SKILL:
		reFirstKeyType = CJoyKeyInfo::JTA_HELM_SKILL;
		break;
	case SettingJoyKeyItemWnd::IT_ARMOR_SKILL:
		reFirstKeyType = CJoyKeyInfo::JTA_ARMOR_SKILL;
		break;
	case SettingJoyKeyItemWnd::IT_WEAPON_SKILL:
		reFirstKeyType = CJoyKeyInfo::JTA_WEAPON_SKILL;
		break;
	case SettingJoyKeyItemWnd::IT_SOLDIER1:
		reFirstKeyType  = CJoyKeyInfo::JTA_SOLDIER1;
		break;
	case SettingJoyKeyItemWnd::IT_SOLDIER2:
		reFirstKeyType  = CJoyKeyInfo::JTA_SOLDIER2;
		break;
	case SettingJoyKeyItemWnd::IT_SOLDIER3:
		reFirstKeyType  = CJoyKeyInfo::JTA_SOLDIER3;
		break;
	case SettingJoyKeyItemWnd::IT_SOLDIER4:
		reFirstKeyType  = CJoyKeyInfo::JTA_SOLDIER4;
		break;
	case SettingJoyKeyItemWnd::IT_SOLDIER5:
		reFirstKeyType  = CJoyKeyInfo::JTA_SOLDIER5;
		break;
	case SettingJoyKeyItemWnd::IT_SOLDIER6:
		reFirstKeyType  = CJoyKeyInfo::JTA_SOLDIER6;
		break;
	case SettingJoyKeyItemWnd::IT_SOLDIER7:
		reFirstKeyType  = CJoyKeyInfo::JTA_SOLDIER7;
		break;
	case SettingJoyKeyItemWnd::IT_SOLDIER8:
		reFirstKeyType  = CJoyKeyInfo::JTA_SOLDIER8;
		break;
	case SettingJoyKeyItemWnd::IT_SOLDIER9:
		reFirstKeyType  = CJoyKeyInfo::JTA_SOLDIER9;
		break;
	case SettingJoyKeyItemWnd::IT_SOLDIER10:
		reFirstKeyType  = CJoyKeyInfo::JTA_SOLDIER10;
		break;
	case SettingJoyKeyItemWnd::IT_SOLDIER11:
		reFirstKeyType  = CJoyKeyInfo::JTA_SOLDIER11;
		break;
	case SettingJoyKeyItemWnd::IT_SOLDIER12:
		reFirstKeyType  = CJoyKeyInfo::JTA_SOLDIER12;
		break;
	case SettingJoyKeyItemWnd::IT_SOLDIER13:
		reFirstKeyType  = CJoyKeyInfo::JTA_SOLDIER13;
		break;
	case SettingJoyKeyItemWnd::IT_SOLDIER14:
		reFirstKeyType  = CJoyKeyInfo::JTA_SOLDIER14;
		break;
	case SettingJoyKeyItemWnd::IT_SOLDIER15:
		reFirstKeyType  = CJoyKeyInfo::JTA_SOLDIER15;
		break;
	case SettingJoyKeyItemWnd::IT_SOLDIER16:
		reFirstKeyType  = CJoyKeyInfo::JTA_SOLDIER16;
		break;
	case SettingJoyKeyItemWnd::IT_SOLDIER17:
		reFirstKeyType  = CJoyKeyInfo::JTA_SOLDIER17;
		break;
	case SettingJoyKeyItemWnd::IT_SOLDIER18:
		reFirstKeyType  = CJoyKeyInfo::JTA_SOLDIER18;
		break;
	case SettingJoyKeyItemWnd::IT_SOLDIER19:
		reFirstKeyType  = CJoyKeyInfo::JTA_SOLDIER19;
		break;
	case SettingJoyKeyItemWnd::IT_SOLDIER20:
		reFirstKeyType  = CJoyKeyInfo::JTA_SOLDIER20;
		break;
	case SettingJoyKeyItemWnd::IT_SOLDIER21:
		reFirstKeyType  = CJoyKeyInfo::JTA_SOLDIER21;
		break;
	case SettingJoyKeyItemWnd::IT_SOLDIER22:
		reFirstKeyType  = CJoyKeyInfo::JTA_SOLDIER22;
		break;
	case SettingJoyKeyItemWnd::IT_SOLDIER23:
		reFirstKeyType  = CJoyKeyInfo::JTA_SOLDIER23;
		break;
	case SettingJoyKeyItemWnd::IT_SOLDIER24:
		reFirstKeyType  = CJoyKeyInfo::JTA_SOLDIER24;
		break;
	case SettingJoyKeyItemWnd::IT_SOLDIER25:
		reFirstKeyType  = CJoyKeyInfo::JTA_SOLDIER25;
		break;
	case SettingJoyKeyItemWnd::IT_SOLDIER26:
		reFirstKeyType  = CJoyKeyInfo::JTA_SOLDIER26;
		break;
	case SettingJoyKeyItemWnd::IT_SOLDIER27:
		reFirstKeyType  = CJoyKeyInfo::JTA_SOLDIER27;
		break;
	case SettingJoyKeyItemWnd::IT_SOLDIER28:
		reFirstKeyType  = CJoyKeyInfo::JTA_SOLDIER28;
		break;
	case SettingJoyKeyItemWnd::IT_SOLDIER29:
		reFirstKeyType  = CJoyKeyInfo::JTA_SOLDIER29;
		break;
	case SettingJoyKeyItemWnd::IT_SOLDIER30:
		reFirstKeyType  = CJoyKeyInfo::JTA_SOLDIER30;
		break;
	case SettingJoyKeyItemWnd::IT_SOLDIER31:
		reFirstKeyType  = CJoyKeyInfo::JTA_SOLDIER31;
		break;
	case SettingJoyKeyItemWnd::IT_SOLDIER32:
		reFirstKeyType  = CJoyKeyInfo::JTA_SOLDIER32;
		break;
	case SettingJoyKeyItemWnd::IT_SOLDIER33:
		reFirstKeyType  = CJoyKeyInfo::JTA_SOLDIER33;
		break;
	case SettingJoyKeyItemWnd::IT_SOLDIER34:
		reFirstKeyType  = CJoyKeyInfo::JTA_SOLDIER34;
		break;
	case SettingJoyKeyItemWnd::IT_SOLDIER35:
		reFirstKeyType  = CJoyKeyInfo::JTA_SOLDIER35;
		break;
	case SettingJoyKeyItemWnd::IT_SOLDIER36:
		reFirstKeyType  = CJoyKeyInfo::JTA_SOLDIER36;
		break;
	}
}

void GameOptionSettingJoyKeyTab::ChangeKeyValue( WORD wKey )
{
	if( !m_KeyInfo.IsRightKey(wKey) )
	{
		for (int i = 0; i < MAX_SHOW_ITEM_WND ; i++)
		{
			SettingJoyKeyItemWnd *pChild = dynamic_cast<SettingJoyKeyItemWnd*> ( FindChildWnd( ID_SETTING_KEY_ITEM_WND1 + i ) );
			if(pChild)
				pChild->UnSelect();
		}
	}
	else
	{
		CJoyKeyInfo::KeyTypeArray eEqualKeyType = m_KeyInfo.GetEqual( wKey );
		if( eEqualKeyType == CJoyKeyInfo::JTA_NONE )
		{
			m_KeyInfo.SetKey( m_eSelectKeyType, wKey );
			UpdateSetPage(m_iCurPage);
		}
		else if( m_eSelectKeyType == eEqualKeyType )
		{
			UpdateSetPage(m_iCurPage);
		}
		else if( m_eSelectKeyType != eEqualKeyType )
		{
			WORD wSelectKey = m_KeyInfo.GetKey(m_eSelectKeyType);
			m_KeyInfo.SetKey( eEqualKeyType, wSelectKey);
			m_KeyInfo.SetKey( m_eSelectKeyType, wKey );
			UpdateSetPage(m_iCurPage);
		}
	}
}

void GameOptionSettingJoyKeyTab::SetSelect( CJoyKeyInfo::KeyTypeArray eKeyType )
{
	for (int i = 0; i < MAX_SHOW_ITEM_WND ; i++)
	{
		SettingJoyKeyItemWnd *pChild = dynamic_cast<SettingJoyKeyItemWnd*> ( FindChildWnd( ID_SETTING_KEY_ITEM_WND1 + i ) );
		if(pChild)
		{
			pChild->UnSelect();
			pChild->SetSelect( eKeyType );
		}
	}
}

void GameOptionSettingJoyKeyTab::Init()
{
	m_DefaultSpec   = Setting::GetSpec();
	m_CurrentOption = Setting::GetSettingOption();

	g_KeyManager.GetJoyCurrent(m_KeyInfo);
	m_iCurPage = 0;

	//
	ioINILoader_e kLoader( "config/sp2_default_joykey.ini" );
	kLoader.SetTitle_e( "warning_desc" );

	m_Warning[0].ClearList();
	m_Warning[1].ClearList();
	m_Warning[2].ClearList();

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	int i, j;
	for( i=0; i < 3; ++i )
	{
		m_Warning[i].SetTextStyle( TS_NORMAL );
		m_Warning[i].SetBkColor( 0, 0, 0 );

		for( j=0; j < 10; ++j )
		{
			wsprintf_e( szKey, "warning_desc%d_color%d", i+1, j+1 );
			kLoader.LoadString( szKey, "", szBuf, MAX_PATH );

			char *pTemp = NULL;
			DWORD dwColor = strtoul( szBuf, &pTemp, 16 );
			m_Warning[i].SetTextColor( dwColor );

			wsprintf_e( szKey, "warning_desc%d_text%d", i+1, j+1 );  
			kLoader.LoadString( szKey, "", szBuf, MAX_PATH );

			if( strcmp( szBuf, "" ) == 0 )
				continue;

			Help::ChangeCharSet( szBuf, '#', ' ' );
			m_Warning[i].AddTextPiece( FONT_SIZE_15, szBuf );
		}
	}
	//
}

void GameOptionSettingJoyKeyTab::Save()
{
	// 옵션 저장을 먼저
	ApplyChangedOption();

	if( g_KeyManager.SetCustomJoyKey( m_KeyInfo ) )
	{
		g_KeyManager.SaveJoyKey();
	}

	if(m_pPlayStage)
	{
		ioBaseChar *pOnwer = m_pPlayStage->GetOwnerChar();
		if(pOnwer)
		{
			ioUserKeyInput::UseKeyValue kKeys;
			kKeys.SetDefaultJoyKey();

			g_KeyManager.GetJoyGameKey(kKeys);

			pOnwer->SetUpJoyKeyValue( kKeys );
		}
	}
}

void GameOptionSettingJoyKeyTab::SetUseJoyPad( DWORD dwID )
{
	CheckRadioButton( ID_USE_CHECK, ID_UNUSED_CHECK, dwID );
	
	bool bPrevUseJoyPad = m_CurrentOption.m_bUseJoyPad;
	switch( dwID )
	{
	case ID_USE_CHECK:
		{
			m_CurrentOption.m_bUseJoyPad = true;
		}
		break;
	case ID_UNUSED_CHECK:
		{
			m_CurrentOption.m_bUseJoyPad = false;
		}
		break;
	}
	CheckUseJoyPad();
	UpdateSetPage( m_iCurPage );

	//
	if( !bPrevUseJoyPad && m_CurrentOption.m_bUseJoyPad )
	{
		ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
		for( int i=0;i < 3;i++ )
		{
			if( m_Warning[i].IsEmpty() )
				break;

			m_Warning[i].CopyFullText( kPrinter[i] );
		}
		g_GUIMgr.SetPrevMsgListBox( NULL, MB_OK, NULL, kPrinter );
	}
}

void GameOptionSettingJoyKeyTab::UpdateByCurrentOption()
{
	if( m_CurrentOption.m_bUseJoyPad )
		SetUseJoyPad( ID_USE_CHECK );
	else
		SetUseJoyPad( ID_UNUSED_CHECK );
}

void GameOptionSettingJoyKeyTab::CheckUseJoyPad()
{
	if( m_CurrentOption.m_bUseJoyPad )
	{
		SetChildActive(ID_SETTING_KEY_ITEM_WND1);
		SetChildActive(ID_SETTING_KEY_ITEM_WND2);
		SetChildActive(ID_SETTING_KEY_ITEM_WND3);
		SetChildActive(ID_SETTING_KEY_ITEM_WND4);
		SetChildActive(ID_SETTING_KEY_ITEM_WND5);
		SetChildActive(ID_SETTING_KEY_ITEM_WND6);
		SetChildActive(ID_SETTING_KEY_ITEM_WND7);
		SetChildActive(ID_SETTING_KEY_ITEM_WND8);
		SetChildActive(ID_SETTING_KEY_ITEM_WND9);
		SetChildActive(ID_SETTING_KEY_ITEM_WND10);
		SetChildActive(ID_SETTING_KEY_ITEM_WND11);
	}
	else
	{
		SetChildInActive(ID_SETTING_KEY_ITEM_WND1);
		SetChildInActive(ID_SETTING_KEY_ITEM_WND2);
		SetChildInActive(ID_SETTING_KEY_ITEM_WND3);
		SetChildInActive(ID_SETTING_KEY_ITEM_WND4);
		SetChildInActive(ID_SETTING_KEY_ITEM_WND5);
		SetChildInActive(ID_SETTING_KEY_ITEM_WND6);
		SetChildInActive(ID_SETTING_KEY_ITEM_WND7);
		SetChildInActive(ID_SETTING_KEY_ITEM_WND8);
		SetChildInActive(ID_SETTING_KEY_ITEM_WND9);
		SetChildInActive(ID_SETTING_KEY_ITEM_WND10);
		SetChildInActive(ID_SETTING_KEY_ITEM_WND11);
	}
}

void GameOptionSettingJoyKeyTab::OnRender()
{
	GameOptionBasicWnd::OnRender();

	// 타이틀 아이콘
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();
	if( m_pIconBack )
	{
		m_pIconBack->SetScale( 0.30f );
		m_pIconBack->Render( iXPos + 30, iYPos + 27, UI_RENDER_MULTIPLY, TFO_BILINEAR );
	}
	if( m_pIconJoypad )
	{
		m_pIconJoypad->SetScale( 0.30f );
		m_pIconJoypad->Render( iXPos + 30, iYPos + 27, UI_RENDER_NORMAL, TFO_BILINEAR );
	}

	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 44, iYPos + 19, FONT_SIZE_14, STR(1) );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( iXPos + 21, iYPos + 43, FONT_SIZE_14, STR(2) );
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


