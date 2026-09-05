

#include "StdAfx.h"

#include "../io3DEngine/ioWnd.h"
#include "../io3DEngine/ioFontManager.h"
#include "../io3DEngine/ioUIRenderer.h"
#include "../io3DEngine/ioUIImageSetManager.h"
#include "../io3DEngine/ioUIRenderImage.h"
#include "../io3DEngine/ioSoundManager.h"
#include "../io3DEngine/HelpFunc.h"
#include "../io3DEngine/iostringmanager.h"
#include "../ioPresentMgr.h"

#include "../TextColorTable.h"
#include "../ChatMessageFunc.h"
#include "../WndID.h"
#include "../EtcHelpFunc.h"
#include "../ioMyInfo.h"
#include "../ioApplication.h"
#include "../ioPlayStage.h"

#include "ioUserInfoMgr.h"
#include "ioMessageBox.h"
#include "ioSP2GUIManager.h"
#include "RelativeGradeBonusWnd.h"
#include <strsafe.h>

RelativeGradeBonusWnd::RelativeGradeBonusWnd()
{
	m_pIconBack = NULL;
	m_pInfoBack = NULL;
	m_pRankingIcon = NULL;
	for( int i=GRADE_LEVEL_0; i<GRADE_LEVEL_MAX; ++i )
	{
		m_pGradeText[i] = NULL;
		m_pGradeIcon[i] = NULL;
	}

	m_pRewardIcon[0] = NULL;
	m_pRewardIcon[1] = NULL;
	m_pSubIcon[0] = NULL;
	m_pSubIcon[1] = NULL;

	m_iGradeType = GRADE_LEVEL_0;
	m_szLastExp.Clear();
}

RelativeGradeBonusWnd::~RelativeGradeBonusWnd()
{
	SAFEDELETE( m_pIconBack );
	SAFEDELETE( m_pInfoBack );
	SAFEDELETE( m_pRankingIcon );
	for( int i=GRADE_LEVEL_0; i<GRADE_LEVEL_MAX; ++i )
	{
		SAFEDELETE( m_pGradeText[i] );
		SAFEDELETE( m_pGradeIcon[i] );
	}
	SAFEDELETE( m_pRewardIcon[0] );
	SAFEDELETE( m_pRewardIcon[1] );
	SAFEDELETE( m_pSubIcon[0] );
	SAFEDELETE( m_pSubIcon[1] );
	m_szLastExp.Clear();
}

void RelativeGradeBonusWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "IconBack" )
	{
		SAFEDELETE( m_pIconBack );
		m_pIconBack = pImage;
	}
	else if( szType == "InfoBack" )
	{
		SAFEDELETE( m_pInfoBack );
		m_pInfoBack = pImage;
	}
	else if( szType == "ranking" )
	{
		SAFEDELETE( m_pRankingIcon );
		m_pRankingIcon = pImage;
	}
	else if( szType == "GradeText0" )
	{
		SAFEDELETE( m_pGradeText[GRADE_LEVEL_0] );
		m_pGradeText[GRADE_LEVEL_0] = pImage;
	}
	else if( szType == "GradeText1" )
	{
		SAFEDELETE( m_pGradeText[GRADE_LEVEL_1] );
		m_pGradeText[GRADE_LEVEL_1] = pImage;
	}
	else if( szType == "GradeText2" )
	{
		SAFEDELETE( m_pGradeText[GRADE_LEVEL_2] );
		m_pGradeText[GRADE_LEVEL_2] = pImage;
	}
	else if( szType == "GradeText3" )
	{
		SAFEDELETE( m_pGradeText[GRADE_LEVEL_3] );
		m_pGradeText[GRADE_LEVEL_3] = pImage;
	}
	else if( szType == "GradeText4" )
	{
		SAFEDELETE( m_pGradeText[GRADE_LEVEL_4] );
		m_pGradeText[GRADE_LEVEL_4] = pImage;
	}
	else if( szType == "GradeIcon0" )
	{
		SAFEDELETE( m_pGradeIcon[GRADE_LEVEL_0] );
		m_pGradeIcon[GRADE_LEVEL_0] = pImage;
	}
	else if( szType == "GradeIcon1" )
	{
		SAFEDELETE( m_pGradeIcon[GRADE_LEVEL_1] );
		m_pGradeIcon[GRADE_LEVEL_1] = pImage;
	}
	else if( szType == "GradeIcon2" )
	{
		SAFEDELETE( m_pGradeIcon[GRADE_LEVEL_2] );
		m_pGradeIcon[GRADE_LEVEL_2] = pImage;
	}
	else if( szType == "GradeIcon3" )
	{
		SAFEDELETE( m_pGradeIcon[GRADE_LEVEL_3] );
		m_pGradeIcon[GRADE_LEVEL_3] = pImage;
	}
	else if( szType == "GradeIcon4" )
	{
		SAFEDELETE( m_pGradeIcon[GRADE_LEVEL_4] );
		m_pGradeIcon[GRADE_LEVEL_4] = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void RelativeGradeBonusWnd::iwm_show()
{

}

void RelativeGradeBonusWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
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

void RelativeGradeBonusWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_pGradeIcon[m_iGradeType] || m_pGradeText[m_iGradeType] && m_pIconBack )
	{
		m_pGradeText[m_iGradeType]->Render( iXPos + 173, iYPos + 53 );
		m_pIconBack->Render( iXPos + 173, iYPos + 135, UI_RENDER_MULTIPLY );
		m_pGradeIcon[m_iGradeType]->Render( iXPos + 173, iYPos + 135 );
	}

	if( m_pInfoBack && m_pRewardIcon[0] && m_pRewardIcon[1] )
	{
		m_pInfoBack->Render( iXPos + 69, iYPos + 277, UI_RENDER_MULTIPLY );
		m_pInfoBack->Render( iXPos + 173, iYPos + 277, UI_RENDER_MULTIPLY );
		m_pInfoBack->Render( iXPos + 277, iYPos + 277, UI_RENDER_MULTIPLY );

		m_pRankingIcon->Render( iXPos + 69, iYPos + 277 );
		m_pRewardIcon[0]->Render( iXPos + 173, iYPos + 277 );
		if( m_pSubIcon[0] )
			m_pSubIcon[0]->Render( iXPos + 173, iYPos + 277 );

		m_pRewardIcon[1]->Render( iXPos + 277, iYPos + 277 );
		if( m_pSubIcon[1] )
			m_pSubIcon[1]->Render( iXPos + 277, iYPos + 277 );
	}

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );

	kPrinter.SetTextColor( TCT_DEFAULT_GREEN );
	kPrinter.AddTextPiece( FONT_SIZE_13, "%s점", m_szLastExp );

	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, "의 누적 장군 경험치로 " );

	char szGradeName[MAX_PATH] = "";
	g_LevelMgr.GetGradeName( g_MyInfo.GetGradeLevel(), szGradeName, sizeof( szGradeName ), false, true );
	kPrinter.SetTextColor( TCT_DEFAULT_GREEN );
	kPrinter.AddTextPiece( FONT_SIZE_13, "%s 달성!", szGradeName );

	kPrinter.PrintFullText( iXPos + 173, iYPos + 181, TAT_CENTER );
	kPrinter.ClearList();

	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, "(달성가능 최고계급은 매주 장군 수에 따라 변동)" );
	kPrinter.PrintFullText( iXPos + 173, iYPos + 199, TAT_CENTER );
	kPrinter.ClearList();

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 69, iYPos + 323, FONT_SIZE_13, "장군 랭킹" );
	g_FontMgr.PrintText( iXPos + 173, iYPos + 323, FONT_SIZE_13, "획득 보상" );
	g_FontMgr.PrintText( iXPos + 277, iYPos + 323, FONT_SIZE_13, "획득 보상" );

	g_FontMgr.SetTextColor( TCT_DEFAULT_GREEN );
	g_FontMgr.PrintText( iXPos + 69, iYPos + 341, FONT_SIZE_13, "%d 위", g_MyInfo.GetUserRanking() );
	if( !m_szReward[0].IsEmpty() )
		g_FontMgr.PrintText( iXPos + 173, iYPos + 341, FONT_SIZE_13, "%s", m_szReward[0] );
	if( !m_szReward[1].IsEmpty() )
		g_FontMgr.PrintText( iXPos + 277, iYPos + 341, FONT_SIZE_13, "%s", m_szReward[1] );
}

void RelativeGradeBonusWnd::SetInfoToPacket( SP2Packet &rkPacket )
{
	if( !g_MyInfo.IsGeneralGrade() )
		return;

	m_iGradeType = g_MyInfo.GetGradeLevel() - g_LevelMgr.GetMaxGradeUp();
	
	SAFEDELETE( m_pRewardIcon[0] );
	SAFEDELETE( m_pRewardIcon[1] );
	SAFEDELETE( m_pSubIcon[0] );
	SAFEDELETE( m_pSubIcon[1] );
	m_szReward[0].Clear();
	m_szReward[1].Clear();

	int iLastExp = 0;
	rkPacket >> iLastExp;

	for( int i=0; i<2; ++i )
	{
		char szConvertNum[MAX_PATH] = "";
		Help::ConvertNumToStrComma( iLastExp, szConvertNum, sizeof( szConvertNum ) );
		m_szLastExp = szConvertNum;

		short iType;
		int iValue1, iValue2;
		rkPacket >> iType >> iValue1 >> iValue2;

		ioHashString szIconName = g_PresentMgr.GetPresentIconText( iType, iValue1, iValue2 );
		m_pRewardIcon[i] = g_UIImageSetMgr.CreateImageByFullName(szIconName);
		m_pSubIcon[i] = g_UIImageSetMgr.CreateImageByFullName( g_PresentMgr.GetPresentSubIconText( iType, iValue1, iValue2 ) );
		if( iType == PRESENT_PESO )
			m_szReward[i] = g_PresentMgr.GetPresentValue2Text( iType, iValue1, iValue2 );
		else
			m_szReward[i] = g_PresentMgr.GetPresentValue1Text( iType, iValue1, iValue2 );
	}

	if( !IsShow() )
		ShowWnd();
}