

#include "StdAfx.h"

#include "AlchemicChangeAlarmWnd.h"

AlchemicChangeAlarmWnd::AlchemicChangeAlarmWnd()
{
	m_iPeso = 0;
	m_iPieceCount = 0;
}

AlchemicChangeAlarmWnd::~AlchemicChangeAlarmWnd()
{
}

void AlchemicChangeAlarmWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_szTopText = xElement.GetStringAttribute_e( "TopText" );
	m_szBottomText[0] = xElement.GetStringAttribute_e( "BottomText1" );
	m_szBottomText[1] = xElement.GetStringAttribute_e( "BottomText2" );
}

void AlchemicChangeAlarmWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	IoString str;
	str.Set(0, 0, m_szTopText.c_str(), FONT_SIZE_13, 275, TS_NORMAL, TCT_DEFAULT_DARKGRAY, 0, TAT_CENTER, TVA_TOP, 5);
	str.OnRender( iXPos + 158, iYPos + 58 );
	
	char szConvertNum[MAX_PATH] = "";
	Help::ConvertNumToStrComma( m_iPeso, szConvertNum, sizeof( szConvertNum ) );
	g_FontMgr.SetVertAlign( TVA_TOP );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetTextColor( TCT_DEFAULT_BLUE );
	g_FontMgr.PrintText( iXPos + 158, iYPos + 210, FONT_SIZE_16, m_szBottomText[0].c_str(), szConvertNum );

	Help::ConvertNumToStrComma( m_iPieceCount, szConvertNum, sizeof( szConvertNum ) );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 158, iYPos + 231, FONT_SIZE_13, m_szBottomText[1].c_str(), szConvertNum );
}

void AlchemicChangeAlarmWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_EXIT:
	case ID_EXIT2:
		if( cmd == IOBN_BTNUP)
			HideWnd();

		break;

	case ID_PRESENT:
		if( cmd == IOBN_BTNUP)
		{
			MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
			if( pInvenWnd )
				pInvenWnd->ShowSubscriptionTabDirect();

			HideWnd();
		}
		break;
	}
}

bool AlchemicChangeAlarmWnd::iwm_spacebar()
{
	ioWnd *pBtn = FindChildWnd( ID_PRESENT );
	if( !pBtn ) return false;
	if( !pBtn->IsShow() ) return false;

	iwm_command( pBtn, IOBN_BTNUP, 0 );
	return true;
}

void AlchemicChangeAlarmWnd::iwm_hide()
{
	m_iPeso = 0;
	m_iPieceCount = 0;

	if( m_pPlayStage )
		m_pPlayStage->ClearPieceToPesoInfo();
}

void AlchemicChangeAlarmWnd::SetInfo( int iPeso, int iCount )
{
	m_iPeso = iPeso;
	m_iPieceCount = iCount;

	if( m_iPeso != 0 )
		ShowWnd();
}

void AlchemicChangeAlarmWnd::SetPlayStage( ioPlayStage *pPlayStage )
{ 
	 m_pPlayStage = pPlayStage;
	 if( m_pPlayStage )
		 m_pPlayStage->GetPieceToPesoInfo( m_iPeso, m_iPieceCount );

	 if( m_iPeso != 0 )
		 ShowWnd();
}