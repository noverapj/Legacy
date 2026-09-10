#include "StdAfx.h"

#include "../io3DEngine/ioWnd.h"
#include "../io3DEngine/ioFontManager.h"
#include "../io3DEngine/iostringmanager.h"

#include "../ioMyInfo.h"
#include "../ioEtcItemManager.h"
#include "../ioMyLevelMgr.h"
#include "../TextColorTable.h"

#include ".\gradeupbonusetcitemwnd.h"
#include "..\TextColorTable.h"


GradeUpBonusEtcItemWnd::GradeUpBonusEtcItemWnd(void)
{
	m_bOneShow = false;
}

GradeUpBonusEtcItemWnd::~GradeUpBonusEtcItemWnd(void)
{
}

void GradeUpBonusEtcItemWnd::iwm_show()
{
	if( m_bOneShow )
	{
		HideWnd();
		return;
	}
	m_bOneShow = true;

	if( !g_MyInfo.IsGradeLevelUPEtcItem() )	
	{
		HideWnd();
		return;
	}

	ioEtcItemBuyMortmainChar *pEtcItem = dynamic_cast<ioEtcItemBuyMortmainChar*> ( g_EtcItemMgr.FindEtcItem( ioEtcItem::EIT_ETC_BUY_MORTMAIN_CHAR ) );
	if( !pEtcItem )
	{
		HideWnd();
		return;
	}
	
	if( !pEtcItem->AddEtcItem( g_MyInfo.GetUserEtcItem(), g_MyInfo.GetGradeLevel(), g_LevelMgr.GetGradeLevelForEtcBonus() ) )
	{
		HideWnd();
		return;
	}
}

void GradeUpBonusEtcItemWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	enum 
	{
		FIRST_X_OFFSET = 105,
		FIRST_Y_OFFSET = 45,
		
		SECOND_X_OFFSET = 105,
		SECOND_Y_OFFSET = 75,

		LINE_Y_GAP      = 20,
	};

	g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetAlignType( TAT_LEFT );

	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY ); // ÁÖÈ²
	g_FontMgr.PrintText( iXPos + FIRST_X_OFFSET, iYPos + FIRST_Y_OFFSET, FONT_SIZE_15, STR(1) );

	iYPos = iYPos + SECOND_Y_OFFSET;
	g_FontMgr.SetTextColor( 225, 180, 0 );
	g_FontMgr.PrintText( iXPos + SECOND_X_OFFSET, iYPos, FONT_SIZE_15, STR(2) );
	iYPos += LINE_Y_GAP;
	g_FontMgr.PrintText( iXPos + SECOND_X_OFFSET, iYPos, FONT_SIZE_15, STR(3) );
	iYPos += LINE_Y_GAP;
	g_FontMgr.PrintText( iXPos + SECOND_X_OFFSET, iYPos, FONT_SIZE_15, STR(4) );
}

void GradeUpBonusEtcItemWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_EXIT:
	case ID_OK:
		if( cmd == IOBN_BTNUP )
			HideWnd();
		break;
	}
}

bool GradeUpBonusEtcItemWnd::iwm_esc()
{
	ioWnd *pBtn = FindChildWnd( ID_OK );
	if( !pBtn ) return false;
	if( !pBtn->IsShow() ) return false;

	iwm_command( pBtn, IOBN_BTNUP, 0 );
	return true;
}
