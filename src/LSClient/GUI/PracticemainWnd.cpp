#include "StdAfx.h"
#include "PracticemainWnd.h"
#include "../GameStage/Mode/Practice/IoPracticeManager.h"
#include "../ChatMessageFunc.h"

PracticeBtn::PracticeBtn()
{
	m_pIconBack = NULL;
	m_pStar_1 = NULL;
	m_pStar_2 = NULL;
	m_pStar_3 = NULL;
	m_pStar_Gray1 = NULL;
	m_pStar_Gray2 = NULL;
	m_pStar_Gray3 = NULL;
	m_pSelectFrm = NULL;
	m_pOverFrm = NULL;
	m_iResult = 0;
	m_iModeIndex = 0;
	m_iMaxCount = 0;
	
	m_bSelcet = false;
}

PracticeBtn::~PracticeBtn()
{
	ClearDesc();
	SAFEDELETE( m_pIconBack );
	SAFEDELETE( m_pStar_Gray1 );
	SAFEDELETE( m_pStar_Gray2 );
	SAFEDELETE( m_pStar_Gray3 );
	SAFEDELETE( m_pStar_1 );
	SAFEDELETE( m_pStar_2 );
	SAFEDELETE( m_pStar_3 );
	SAFEDELETE(m_pSelectFrm);
	SAFEDELETE(m_pOverFrm);
}

void PracticeBtn::ClearDesc()
{
	for (int i = 0; i < MAX_DESC ; i++)
	{
		m_szDescArray[i].ClearList();
	}
}

void PracticeBtn::OnProcess( float fTimePerSec )
{

}

void PracticeBtn::OnRender()
{
	ioButton::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();
	
	if( m_pIconBack )
	{
		m_pIconBack->SetScale( FLOAT1, FLOAT1);
		m_pIconBack->Render( iXPos , iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
	}

	if(IsOvered())
	{
		if(m_pOverFrm)
			m_pOverFrm->Render( iXPos, iYPos, UI_RENDER_SCREEN );
	}
	if(m_bSelcet)
	{
		if( m_pSelectFrm )
			m_pSelectFrm->Render( iXPos, iYPos );
	}

	if(m_iModeIndex == 0)
		return;

	if( !m_szDescArray[0].IsEmpty() )
	{
		m_szDescArray[0].PrintFullText( iXPos+16, iYPos+87, TAT_LEFT );
	}

	if( !m_szDescArray[1].IsEmpty() )
		m_szDescArray[1].PrintFullText( iXPos+16, iYPos+137, TAT_LEFT );

	if( !m_szDescArray[2].IsEmpty() )
		m_szDescArray[2].PrintFullText( iXPos+297, iYPos+137, TAT_LEFT );

	if( m_pStar_Gray1 && m_iResult > 0)
	{
		m_pStar_Gray1->Render( iXPos, iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
		m_pStar_Gray2->Render( iXPos, iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
		m_pStar_Gray3->Render( iXPos, iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
	}
	if( m_pStar_1 && m_iResult > 2)
	{
		m_pStar_1->Render( iXPos, iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
		m_pStar_2->Render( iXPos, iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
		m_pStar_3->Render( iXPos, iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
	}
	else if( m_pStar_1 && m_iResult > 1)
	{
		m_pStar_1->Render( iXPos, iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
		m_pStar_2->Render( iXPos, iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
	}
	else if( m_pStar_1 && m_iResult > 0)
		m_pStar_1->Render( iXPos, iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
}

void PracticeBtn::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "IconBack1" )
	{
		SAFEDELETE( m_pIconBack );		
		m_pIconBack = pImage;
	}
	else if( szType == "Star1")
	{
		SAFEDELETE(m_pStar_1);
		m_pStar_1 = pImage;
	}
	else if( szType == "Star2")
	{
		SAFEDELETE(m_pStar_2);
		m_pStar_2 = pImage;
	}
	else if( szType == "Star3")
	{
		SAFEDELETE(m_pStar_3);
		m_pStar_3 = pImage;
	}
	else if( szType == "GrayStar1")
	{
		SAFEDELETE(m_pStar_Gray1);
		m_pStar_Gray1 = pImage;
	}
	else if( szType == "GrayStar2")
	{
		SAFEDELETE(m_pStar_Gray2);
		m_pStar_Gray2 = pImage;
	}
	else if( szType == "GrayStar3")
	{
		SAFEDELETE(m_pStar_Gray3);
		m_pStar_Gray3 = pImage;
	}
	else 
	{
		ioButton::AddRenderImage( szType, pImage );
	}
}

void PracticeBtn::iwm_create()
{
	DWORD dwID = GetID();
	switch(dwID)
	{
	case PracticeMainWnd::ID_PRACTICE_1:
		m_iModeIndex = 1;
		break;
	case PracticeMainWnd::ID_PRACTICE_2:
		m_iModeIndex = 2;
		break;
	case PracticeMainWnd::ID_PRACTICE_3:
		m_iModeIndex = 3;
		break;
	case PracticeMainWnd::ID_PRACTICE_4:
		m_iModeIndex = 4;
		break;
	case PracticeMainWnd::ID_PRACTICE_5:
		m_iModeIndex = 5;
		break;
	case PracticeMainWnd::ID_PRACTICE_6:
		m_iModeIndex = 6;
		break;
	case PracticeMainWnd::ID_PRACTICE_7:
		m_iModeIndex = 7;
		break;
	case PracticeMainWnd::ID_PRACTICE_8:
		m_iModeIndex = 8;
		break;
	default:
		m_iModeIndex = 0;
		break;
	}
	m_iMaxCount = g_PracticeMgr.GetPracticeAdmission(m_iModeIndex);

	ioINILoader kLoader( "config/sp2_practiceinfo.ini" );
	kLoader.SetTitle( "mode_info" );

	char szBuf[MAX_PATH]="";
	wsprintf( szBuf, "mode%d_title", m_iModeIndex );
	kLoader.LoadString(szBuf, "", szBuf, MAX_PATH);
	
	m_szDescArray[0].SetTextStyle( TS_NORMAL );
	m_szDescArray[0].SetBkColor( 0, 0, 0 );	
	m_szDescArray[0].SetTextColor( TCT_DEFAULT_WHITE);
	m_szDescArray[0].AddTextPiece( FONT_SIZE_22, g_PracticeMgr.GetPracticeTypeTitle(m_iModeIndex).c_str()); //str,%s


	m_szDescArray[1].SetTextStyle( TS_NORMAL );
	m_szDescArray[1].SetBkColor( 0, 0, 0 );	
	m_szDescArray[1].SetTextColor(167,167,167);
	wsprintf( szBuf, "mode%d_text", m_iModeIndex );
	kLoader.LoadString(szBuf, "", szBuf, MAX_PATH);
	m_szDescArray[1].AddTextPiece( FONT_SIZE_12, g_PracticeMgr.GetPracticeGuideDesc1(m_iModeIndex).c_str());
	
	m_szDescArray[2].SetTextStyle( TS_NORMAL );
	m_szDescArray[2].SetBkColor( 0, 0, 0 );	
	m_szDescArray[2].SetTextColor( 167, 167, 167);
	kLoader.LoadString("mode_count", "", szBuf, MAX_PATH);
	m_szCountText = szBuf;
	int iCount = g_PracticeMgr.GetPracticeInfo()[m_iModeIndex-1].iPracticeCount;
	wsprintf( szBuf, "%s %d/%d", m_szCountText.c_str(), iCount, m_iMaxCount);
	m_szDescArray[2].AddTextPiece( FONT_SIZE_12, szBuf );//%d %d

	m_vInventoryItemInfoList.clear();
	InventoryItemInfo kInfo;
	kInfo.m_iMainType = g_PracticeMgr.GetPracticeRewardCType(m_iModeIndex);;
	kInfo.m_iMagicCode = g_PracticeMgr.GetPracticeRewardValueC(m_iModeIndex);
	kInfo.m_iManualIndex = 0;
	kInfo.m_iPriorityOrder = 0;
	m_vInventoryItemInfoList.push_back(kInfo);
	kInfo.m_iMainType = g_PracticeMgr.GetPracticeRewardBType(m_iModeIndex);;
	kInfo.m_iMagicCode = g_PracticeMgr.GetPracticeRewardValueB(m_iModeIndex);
	kInfo.m_iManualIndex = 0;
	kInfo.m_iPriorityOrder = 0;
	m_vInventoryItemInfoList.push_back(kInfo);
	kInfo.m_iMainType = g_PracticeMgr.GetPracticeRewardAType(m_iModeIndex);;
	kInfo.m_iMagicCode = g_PracticeMgr.GetPracticeRewardValueA(m_iModeIndex);
	kInfo.m_iManualIndex = 0;
	kInfo.m_iPriorityOrder = 0;
	m_vInventoryItemInfoList.push_back(kInfo);
}

void PracticeBtn::iwm_show()
{
	char szBuf[MAX_PATH]="";
	int iCount = g_PracticeMgr.GetPracticeInfo()[m_iModeIndex-1].iPracticeCount;
//	LOG.PrintTimeAndLog( 0, "PracticeMainWnd::iwm_show iCount = %d",iCount );
	if(m_iModeIndex-1 < g_PracticeMgr.GetPracticeCount())
		wsprintf( szBuf, "%s %d/%d", m_szCountText.c_str(), iCount, m_iMaxCount);
	m_szDescArray[2].ClearList();
	m_szDescArray[2].AddTextPiece( FONT_SIZE_12, szBuf );

	m_iResult = g_PracticeMgr.GetPracticeInfo()[m_iModeIndex-1].iPracticeGrade;
}

void PracticeBtn::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( pFrame )
	{
		if( szType == "SelectFrm" )
		{
			SAFEDELETE( m_pSelectFrm );
			m_pSelectFrm = pFrame;
		}
		else if( szType == "OverAdd" )
		{
			SAFEDELETE( m_pOverFrm );
			m_pOverFrm = pFrame;
		} 
		else
		{	
			ioButton::AddRenderFrame( szType, pFrame );
		}
	}
}

void PracticeBtn::InitBG( int iBGindex )
{
	CString BackGroundStr = g_PracticeMgr.GetPracticeBtnImage(iBGindex+1);
	int iIndex = BackGroundStr.Find("#");
	CString NumberStr = BackGroundStr.Mid(iIndex+1, BackGroundStr.GetLength() - iIndex);
	CString ImageStr = BackGroundStr.Left(iIndex);
	if(m_pIconBack)
	{
		SAFEDELETE(m_pIconBack);
		m_pIconBack = NULL;
	}
	m_pIconBack = g_UIImageSetMgr.CreateImage( (LPCTSTR)ImageStr, (LPCTSTR)NumberStr );
}


PracticeMainWnd::PracticeMainWnd(void)
{
	m_pIconBack = NULL;
	m_pIcon = NULL;
	m_pLobby = NULL;
	m_iSelectBtn = ID_PRACTICE_1;
	m_bHaveEmpty = FALSE;
}


PracticeMainWnd::~PracticeMainWnd(void)
{
	ClearDesc();
	SAFEDELETE( m_pIconBack );
	SAFEDELETE( m_pIcon );
}

void PracticeMainWnd::ClearDesc()
{
}

void PracticeMainWnd::iwm_create()
{
	ioINILoader kLoader( "config/sp2_practiceinfo.ini" );
	kLoader.SetTitle( "common_text" );
	char szBuf[MAX_PATH]="";
	kLoader.LoadString("title", "", szBuf, MAX_PATH);
	m_szDescArray[0] = szBuf;
	kLoader.LoadString("text1", "", szBuf, MAX_PATH);
	m_szDescArray[1] = szBuf;	

	int iPage = g_PracticeMgr.GetPracticeCount();
	for( int i = 0; i < iPage; ++i )
	{
		ioWnd *pWnd = g_GUIMgr.AddWndByTemplate( ioHashString("PracticeBtn"), this, ID_PRACTICE_1 + i );
		if( pWnd )
		{
			pWnd->SetWndPos( 11, 78 + (i%2 * 176) );//11, 254
			static_cast<PracticeBtn*>(pWnd)->InitBG(i);
		}
	}
	if(iPage %2 ==0)
	{
		m_bHaveEmpty = TRUE;
		ioWnd *pWnd = FindChildWnd(ID_EMPTY_BTN);
		if(pWnd)
			pWnd->HideWnd();
	}

	if(iPage > 2)
		iPage = iPage % 2 > 0 ? iPage/2+1 : iPage/2;	

	m_iCurPage			= 1;
	m_iMaxPage			= iPage < 2 ? 1 : iPage;
}

void PracticeMainWnd::iwm_show()
{	
	m_iCurPage = 1;
	InitPracticeBtn(m_iCurPage);
	PracticeUiMove();
	if(m_pIcon && m_pIconBack)
	{
		m_pIconBack->SetScale( 0.85f, 0.85f );
		m_pIcon->SetScale( 0.85f, 0.85f );
	}
	
	for(int i = 0; i < g_PracticeMgr.GetPracticeCount(); ++i)
	{
		PracticeBtn *pBtn = dynamic_cast<PracticeBtn*>( FindChildWnd( i+101 ) );
		if(pBtn && i == 0)
		{
			pBtn->SetSelect(true);
/*
			if(m_pLobby)
			m_pLobby->SetModeIndex(1);*/
		}
		else
			pBtn->SetSelect(false);
	}
	

}

// 2019-07-11
void PracticeMainWnd::SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate )
{
	ioWnd::SetPositionToDefault( iParentWidth, iParentHeight, bChildMove, bCreate );	

	for( int i = 0; i < g_PracticeMgr.GetPracticeCount(); ++i )
	{			
		ioWnd *pWnd = FindChildWnd( ID_PRACTICE_1 + i );
		if( pWnd )
		{
			pWnd->SetWndPos( 11, 78 + (i%2 * 176) );//11, 254
			static_cast<PracticeBtn*>(pWnd)->InitBG(i);
		}
	}
}

void PracticeMainWnd::iwm_hide()
{
	
}

void PracticeMainWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{	
	case ID_PRE_PAGE_BTN:
		if( cmd == IOBN_BTNUP )
			PrevPage();
		break;
	case ID_NEXT_PAGE_BTN:
		if( cmd == IOBN_BTNUP )
			NextPage();
		break;
	case ID_PRACTICE_1:
	case ID_PRACTICE_2:
	case ID_PRACTICE_3:
	case ID_PRACTICE_4:
	case ID_PRACTICE_5:
	case ID_PRACTICE_6:
	case ID_PRACTICE_7:
	case ID_PRACTICE_8:
	case ID_EMPTY_BTN:
		if( cmd == IOBN_BTNUP)
		{
			m_pLobby->ShowWnd();
			CheckPracticeBtn( dwID );
		}
		break;
	default:
		if( cmd == IOBN_BTNUP)
			m_pLobby->HideWnd();
		break;
	}
}

void PracticeMainWnd::OnProcess( float fTimePerSec )
{

}

void PracticeMainWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_pIconBack )
	{
		m_iPosX = GetPosX();
		m_iPosY = GetYPos();		
		m_pIconBack->Render( iXPos + 362, iYPos + 35, UI_RENDER_MULTIPLY, TFO_BILINEAR );
	}

	if( m_pIcon )
		m_pIcon->Render( iXPos + 362, iYPos + 35, UI_RENDER_NORMAL, TFO_BILINEAR );

	if( !m_szDescArray[0].IsEmpty() )
	{
		IoString str;
		str.Set(0, 0, m_szDescArray[0].c_str(), FONT_SIZE_18, 290, TS_NORMAL, TCT_DEFAULT_BLUE, 0, TAT_LEFT, TVA_TOP, 3);
		str.OnRender( iXPos + 11, iYPos + 5 );
	}

	if( !m_szDescArray[1].IsEmpty() )
	{
		IoString str;
		str.Set(0, 0, m_szDescArray[1].c_str(), FONT_SIZE_12, 290, TS_NORMAL, TCT_DEFAULT_GRAY, 0, TAT_LEFT, TVA_TOP, 3);
		str.OnRender( iXPos + 11, iYPos + 36 );
	}
	
	int iRendPage = min( m_iCurPage, m_iMaxPage );
	char szBuf[MAX_PATH] = "";
	wsprintf( szBuf, "%d", iRendPage );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );	
	g_FontMgr.PrintText( iXPos + 201, iYPos + 435, FONT_SIZE_12, szBuf );
	
	
}

void PracticeMainWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "IconBack" )
	{
		SAFEDELETE( m_pIconBack );		
		m_pIconBack = pImage;
	}
	else if( szType == "Icon" )
	{
		SAFEDELETE( m_pIcon );
		m_pIcon = pImage;
	}
	else 
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void PracticeMainWnd::InitPracticeBtn(DWORD dwPage)
{
	if( m_bHaveEmpty )
	{
		ioWnd* pWnd = dynamic_cast<ioWnd*>( FindChildWnd( ID_EMPTY_BTN ) );
		if( pWnd )
		{
			if( dwPage == m_iMaxPage )		
				pWnd->ShowWnd();
			else
				pWnd->HideWnd();
		}
	}

	dwPage = ID_PRACTICE_1 + 2*dwPage;
	for( DWORD dwID = ID_PRACTICE_1; dwID < ID_PRACTICE_END; ++dwID )
	{
		PracticeBtn *pBtn = dynamic_cast<PracticeBtn*>( FindChildWnd( dwID ) );
		if( pBtn )
		{
			if( dwPage-2 <= dwID && dwID < dwPage )
				pBtn->ShowWnd();
			else
				pBtn->HideWnd();
		}
	}
}

void PracticeMainWnd::PrevPage()
{
	if(  0 < m_iCurPage - 1 )
	{
		--m_iCurPage;
		InitPracticeBtn(m_iCurPage );
	}
}

void PracticeMainWnd::NextPage()
{
	if( m_iCurPage +1 <= m_iMaxPage )
	{
		++m_iCurPage;
		InitPracticeBtn(m_iCurPage);
	}
}

void PracticeMainWnd::PracticeUiMove( int nAddYPos /*= 0 */ )
{
	m_iPageYPos = 435 + nAddYPos;
	ioWnd* pLeft = FindChildWnd( ID_PRE_PAGE_BTN );
	if( pLeft )
		pLeft->SetWndPos( pLeft->GetDefaultPosX(), m_iPageYPos );

	ioWnd* pRight = FindChildWnd( ID_NEXT_PAGE_BTN );
	if( pRight )
		pRight->SetWndPos( pRight->GetDefaultPosX(), m_iPageYPos );
}

void PracticeMainWnd::CheckPracticeBtn( DWORD dwID )
{
	if( dwID == ID_EMPTY_BTN )
		return;
	m_dwTabID = dwID;
	
	for( int i = ID_PRACTICE_1 ; i < ID_PRACTICE_END; ++i)
	{			
		PracticeBtn *pBtn = dynamic_cast<PracticeBtn*>( FindChildWnd( i ) );
		if(!pBtn)
			continue;
		pBtn->SetSelect(false);
		if(i == m_dwTabID)
		{
			m_pLobby->SetModeIndex(dwID-100);
			m_pLobby->SetRewardList(pBtn->GetItemInfo());
			pBtn->SetSelect(true);
			m_iSelectBtn = i;
		}

	}
}

void PracticeMainWnd::SetLobbyInfo( PracticeLobbyInfoWnd* pLobby )
{
	m_pLobby = pLobby;
	m_pLobby->SetModeIndex(1);
}

PracticeLobbyInfoWnd::PracticeLobbyInfoWnd()
{
	m_pImageBack = NULL;
	m_pRewardHelp = NULL;
	m_pRankHelp = NULL;
	m_pRewardBg	= NULL;
	m_pRewardIconBg = NULL;
	m_pRankBg = NULL;
	m_pRewardIcon = NULL;
	m_pStar = NULL;
	m_pGrayStar = NULL;
	m_dwIndex = 0;
	m_dwOveredIcon = 0;
	m_iCurPos = 0;
	m_dwBusyTime = 0;
	m_bBusy = false;
}

PracticeLobbyInfoWnd::~PracticeLobbyInfoWnd()
{
	SAFEDELETE(m_pImageBack);
	SAFEDELETE(m_pRewardHelp);
	SAFEDELETE(m_pRewardBg);
	SAFEDELETE(m_pRewardIconBg);
	SAFEDELETE(m_pRankHelp);
	SAFEDELETE(m_pRankBg);
	SAFEDELETE(m_pStar);
	SAFEDELETE(m_pGrayStar);
	ClearIconMap();
	//ClearDesc();
}

void PracticeLobbyInfoWnd::iwm_create()
{
	for(size_t i = 0; i < m_vInventoryItemInfoList.size(); ++i)
	{
		ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( m_vInventoryItemInfoList[i].m_iMagicCode );
		if( !pEtcItem ) 
			return;
		m_szIconName[i] = pEtcItem->GetIconName();
	}
	SetModeIndex(1);
}

void PracticeLobbyInfoWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch(dwID)
	{
	case ID_REWARD_HELP:
		if( cmd == IOWN_OVERED )
		{
			ShowChildWnd( ID_REWARD_HELP_TOOLTIP );
		}
		else if( cmd == IOWN_LEAVED )
		{
			HideChildWnd( ID_REWARD_HELP_TOOLTIP );
		}
		break;
	case ID_RANK_HELP:
		if( cmd == IOWN_OVERED )
		{
			ShowChildWnd( ID_RANKING_HELP_TOOLTIP );
		}
		else if( cmd == IOWN_LEAVED )
		{
			HideChildWnd( ID_RANKING_HELP_TOOLTIP );
		}
		break;
	case ID_START_BTN:
		if( cmd == IOBN_BTNUP)
		{
			SP2Packet kPacket( CTPK_PRACTICE_ENTER );
			kPacket << g_PracticeMgr.GetPracticeMap(m_dwIndex);
			kPacket << m_dwIndex;
			if( TCPNetwork::CheckSendToServer( kPacket ) )
				TCPNetwork::MouseBusy( true );
			g_PracticeMgr.SetPreIndex( m_dwIndex );
		}
		break;
	case ID_REWARD_BTN1:
		if( cmd == IOWN_OVERED )
			m_dwOveredIcon = dwID;
		else if( cmd == IOWN_LEAVED )
			m_dwOveredIcon = 0;
		break;
	case ID_REWARD_BTN2:
		if( cmd == IOWN_OVERED )
			m_dwOveredIcon = dwID;
		else if( cmd == IOWN_LEAVED )
			m_dwOveredIcon = 0;
		break;
	case ID_REWARD_BTN3:
		if( cmd == IOWN_OVERED )
			m_dwOveredIcon = dwID;
		else if( cmd == IOWN_LEAVED )
			m_dwOveredIcon = 0;
		break;
	case ID_RANK_BTN:
		if( cmd == IOBN_BTNUP)
		{
			PracticeRankingWnd *pRankingWnd = dynamic_cast<PracticeRankingWnd*>(g_GUIMgr.FindWnd(PRACTICE_RANKING_WND));
			if(pRankingWnd)
			{
				if(!pRankingWnd->IsShow())
					pRankingWnd->ShowWnd();
			}
		}
		break;
	}

}

void PracticeLobbyInfoWnd::iwm_wheel( int zDelta )
{
	if( !IsShow() )
	{
		ioWnd::iwm_wheel( zDelta );
	}
	else
	{
		if( g_App.IsMouseBusy() )
			return;

		ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_SCROLL );
		if( pScroll == NULL )
			return;

		if( !pScroll->IsShow() )
			return;

		if( zDelta == WHEEL_DELTA )
			pScroll->SetScrollPos( m_iCurPos - 1 );
		else if( zDelta == -WHEEL_DELTA )
			pScroll->SetScrollPos( m_iCurPos + 1 );
	}
}

void PracticeLobbyInfoWnd::iwm_vscroll( DWORD dwID, int curPos )
{
	ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_SCROLL );
	if( pScroll == NULL )
		return;

	if( !pScroll->IsShow() )
		return;

	m_iCurPos = curPos;
}


bool PracticeLobbyInfoWnd::GetCurItemInfo( OUT InventoryItemInfo &rkInfo, IN DWORD dwItemInfoID )
{
	//바꾸기
	if(!COMPARE(dwItemInfoID,ID_REWARD_BTN1, ID_REWARD_BTN3+1))
		return false;
	if(m_vInventoryItemInfoList.empty())
		return false;
	rkInfo = m_vInventoryItemInfoList[dwItemInfoID-ID_REWARD_BTN1];
	return true;
}

void PracticeLobbyInfoWnd::OnProcess( float fTimePerSec )
{
	int iMaxChatViewMsg = g_ChatMgr.GetMaxHelpViewMsg();
	for(int i = 0;i < iMaxChatViewMsg;i++)
	{
		IMsgItem *pMsgItem = g_ChatMgr.GetReverseChatMsgByFeature( i );
		if( !pMsgItem ) continue;

		m_ChatList.push_back( pMsgItem );

		if( pMsgItem->GetChatID() == NULL ) continue;
		if( pMsgItem->GetChatID()->IsEmpty() ) continue;
	}
	if(m_dwBusyTime > 0 && m_dwBusyTime + 300 < FRAMEGETTIME())
	{
		if(!g_App.GetMouse()->IsDragDropping())
			g_App.SetMouseBusy(false);
		m_bBusy = false;
	}
}

void PracticeLobbyInfoWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();
	
	g_FontMgr.SetTextStyle(TS_NORMAL);
	g_FontMgr.SetAlignType(TAT_LEFT);
	g_FontMgr.SetTextColor(TCT_DEFAULT_DARKGRAY);
	g_FontMgr.PrintTextWidthCut( iXPos + 17, iYPos + 58, FONT_SIZE_14, 172, m_szText[0].c_str() );
	
	if(m_pImageBack)
		m_pImageBack->Render( iXPos+17, iYPos+77, UI_RENDER_MULTIPLY );

	if(m_pRankHelp)	
		m_pRankHelp->Render( iXPos+48, iYPos+343, UI_RENDER_NORMAL_ADD_COLOR );
	
	if(m_pRewardBg)
		m_pRewardBg->Render(iXPos+17, iYPos+253,UI_RENDER_MULTIPLY);
	if(m_pRewardIconBg)
	{		
		m_pRewardIconBg->SetScale(0.7f,0.7f);
		m_pRewardIconBg->Render(iXPos+32+17,iYPos+253+30, UI_RENDER_MULTIPLY, TFO_BILINEAR );
		m_pRewardIconBg->Render(iXPos+86+17,iYPos+30+253, UI_RENDER_MULTIPLY, TFO_BILINEAR );
		m_pRewardIconBg->Render(iXPos+140+17,iYPos+30+253, UI_RENDER_MULTIPLY, TFO_BILINEAR );

		for(size_t i = 0; i < 3; ++i)
		{			
			m_pRewardIcon = GetImage(m_szIconName[i]);
			if(m_pRewardIcon)
			{
				m_pRewardIcon->SetScale(0.7f,0.7f);
				m_pRewardIcon->Render(iXPos+17+32+i*54, iYPos+253+30, UI_RENDER_NORMAL, TFO_BILINEAR );
			}
		}		
	}
	
	//행간 14
	g_FontMgr.PrintTextWidthCut( iXPos + 17, iYPos + 234, FONT_SIZE_14, 172, m_szText[1].c_str() );

	for( int i = 0; i < 8; i++ )
		m_szGuide.OnRenderLine( i + m_iCurPos, iXPos + 23, iYPos + 83 - m_szGuide.GetLineHeight(m_iCurPos) * m_iCurPos );

	g_FontMgr.SetAlignType(TAT_CENTER);
	g_FontMgr.PrintTextWidthCut( iXPos + 17+32, iYPos + 253+59, FONT_SIZE_11, 172, m_szText[2].c_str() );
	g_FontMgr.PrintTextWidthCut( iXPos + 17+86, iYPos + 253+59, FONT_SIZE_11, 172, m_szText[3].c_str() );
	g_FontMgr.PrintTextWidthCut( iXPos + 17+140, iYPos + 253+59, FONT_SIZE_11, 172, m_szText[4].c_str() );

	g_FontMgr.SetAlignType(TAT_LEFT);
	g_FontMgr.PrintTextWidthCut( iXPos + 17, iYPos + 343, FONT_SIZE_14, 172, m_szText[5].c_str() );


	{//랭킹 17,367 
		if(m_pRankBg)
		{
			m_pRankBg->Render(iXPos+17,iYPos+367, UI_RENDER_MULTIPLY);
			m_pRankBg->Render(iXPos+17,iYPos+367+42, UI_RENDER_MULTIPLY);
		}
		char szBuf[MAX_PATH];
		for(int i = 0; i < 3; ++i)
		{//서버에서 받아서 넣기
			g_FontMgr.SetAlignType(TAT_LEFT);
			wsprintf( szBuf, m_szRank.c_str(), i+1 );
			g_FontMgr.PrintTextWidthCut( iXPos + 17+6, iYPos + 367+4 + i*21 , FONT_SIZE_12, 68, szBuf );
			if(!g_PracticeMgr.GetRankerInfo(i).szName.IsEmpty())
			{
				g_FontMgr.SetAlignType(TAT_LEFT);
				g_FontMgr.PrintTextWidthCut( iXPos + 17+39, iYPos + 367+4 + i*21 , FONT_SIZE_12, 68, g_PracticeMgr.GetRankerInfo(i).szTime.c_str() );
				g_FontMgr.SetAlignType(TAT_RIGHT);
				g_FontMgr.PrintTextWidthCut( iXPos + 17+165, iYPos + 367+4 + i*21 , FONT_SIZE_12, 68, g_PracticeMgr.GetRankerInfo(i).szName.c_str() );
			}
			else
			{
				g_FontMgr.SetAlignType(TAT_LEFT);
				g_FontMgr.PrintTextWidthCut( iXPos + 17+39, iYPos + 367+4 + i*21 , FONT_SIZE_12, 68, "00:00.00" );
				g_FontMgr.SetAlignType(TAT_RIGHT);
				g_FontMgr.PrintTextWidthCut( iXPos + 17+165, iYPos + 367+4 + i*21 , FONT_SIZE_12, 68, m_szText[6].c_str() );
			}
		}
		g_FontMgr.SetAlignType(TAT_CENTER);
		g_FontMgr.SetTextColor(TCT_DEFAULT_BLUE);
		if( g_PracticeMgr.GetMyRankInfo().szPracticeTime == "00:00.00" )
			wsprintf( szBuf, m_szText[6].c_str());
		else
			wsprintf( szBuf, m_szText[7].c_str(), g_PracticeMgr.GetMyRankInfo().iPracticeRank, g_PracticeMgr.GetMyRankInfo().szPracticeTime );
		g_FontMgr.PrintTextWidthCut( iXPos + 101, iYPos + 441 , FONT_SIZE_15, 172, szBuf );
	}
	
	//수련장 ui
	int iMaxChatViewMsg = m_ChatList.size();
	for(int i = 0;i < iMaxChatViewMsg;i++)
	{
		IMsgItem *pMsgItem = m_ChatList[i];
		if( !pMsgItem ) continue;

		pMsgItem->DrawItem( iXPos - 20, iYPos + 80 + ( i * 13 ), false );	 // 마우스 오버시 채팅 컬러를 바꾸지 않는다.
	}
	m_ChatList.clear();
}

void PracticeLobbyInfoWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "Background" )
	{
		SAFEDELETE( m_pImageBack );
		m_pImageBack = pImage;
	}
	else if(szType == "RewardHelp")
	{
		SAFEDELETE(m_pRewardHelp);
		m_pRewardHelp = pImage;
	}
	else if(szType == "RankHelp")
	{
		SAFEDELETE(m_pRankHelp);
		m_pRankHelp = pImage;
	}
	else if(szType == "RewardBg")
	{
		SAFEDELETE(m_pRewardBg);
		m_pRewardBg = pImage;
	}
	else if(szType == "RewardIconBg1")
	{
		SAFEDELETE(m_pRewardIconBg);
		m_pRewardIconBg = pImage;
	}
	else if(szType == "RankTextBg")
	{
		SAFEDELETE(m_pRankBg);
		m_pRankBg = pImage;
	}
	else 
	{                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         
		ioWnd::AddRenderImage( szType, pImage );
	}
}

ioUIRenderImage* PracticeLobbyInfoWnd::GetImage( const ioHashString &szIconName )
{
	if ( szIconName.IsEmpty() )
		return NULL;

	PracticeIconMap::iterator iter = m_RewardIconMap.find( szIconName );
	if( iter != m_RewardIconMap.end() )
		return iter->second;

	ioUIRenderImage *pIcon = g_UIImageSetMgr.CreateImageByFullName( szIconName );
	if( pIcon )
	{
		m_RewardIconMap.insert( PracticeIconMap::value_type( szIconName, pIcon ) );
		return pIcon;
	}

	return NULL;
}

void PracticeLobbyInfoWnd::SetModeIndex( DWORD dwIndex )
{
	m_dwIndex = dwIndex;

	ioINILoader kLoader( "config/sp2_Practice_Lobby.ini" );
	kLoader.SetTitle("common");

	if(!m_bBusy)
	{
		SP2Packet kPacket( CTPK_PRACTICE_INDEXRANK );
		kPacket << g_PracticeMgr.GetPracticeMap(m_dwIndex);
		kPacket << m_dwIndex;
		if( TCPNetwork::CheckSendToServer( kPacket ) )
			TCPNetwork::MouseBusy( true );
		m_bBusy = true;
	}

	SetINIText(kLoader, dwIndex);

	m_dwBusyTime = FRAMEGETTIME();
	
	char szIconName[MAX_PATH] = "";
	
	//이거 지우고 예외처리해야함
	m_vInventoryItemInfoList.clear();
	InventoryItemInfo kInfo;
	kInfo.m_iMainType = g_PracticeMgr.GetPracticeRewardCType(dwIndex);;
	kInfo.m_iMagicCode = g_PracticeMgr.GetPracticeRewardValueC(dwIndex);
	kInfo.m_iManualIndex = 0;
	kInfo.m_iPriorityOrder = 0;
	m_vInventoryItemInfoList.push_back(kInfo);
	kInfo.m_iMainType = g_PracticeMgr.GetPracticeRewardBType(dwIndex);;
	kInfo.m_iMagicCode = g_PracticeMgr.GetPracticeRewardValueB(dwIndex);
	kInfo.m_iManualIndex = 0;
	kInfo.m_iPriorityOrder = 0;
	m_vInventoryItemInfoList.push_back(kInfo);
	kInfo.m_iMainType = g_PracticeMgr.GetPracticeRewardAType(dwIndex);;
	kInfo.m_iMagicCode = g_PracticeMgr.GetPracticeRewardValueA(dwIndex);
	kInfo.m_iManualIndex = 0;
	kInfo.m_iPriorityOrder = 0;
	m_vInventoryItemInfoList.push_back(kInfo);

	//std::reverse(m_vInventoryItemInfoList.begin(), m_vInventoryItemInfoList.end());

	for(size_t i = 0; i < m_vInventoryItemInfoList.size(); ++i)
	{
		ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( m_vInventoryItemInfoList[i].m_iMagicCode );
		if( !pEtcItem ) 
			continue;;
		m_szIconName[i] = pEtcItem->GetIconName();
	}

	
//	SetHelpText();
}

void PracticeLobbyInfoWnd::ClearIconMap()
{
	PracticeIconMap::iterator iCreate = m_RewardIconMap.begin();
	for( ; iCreate != m_RewardIconMap.end() ; ++iCreate )
		SAFEDELETE( iCreate->second );
	m_RewardIconMap.clear();
}

void PracticeLobbyInfoWnd::SetPlayStage( ioPlayStage *pPlayStage )
{
	m_pPlayStage = pPlayStage;
}

void PracticeLobbyInfoWnd::SetRewardList( vInventoryItemInfoList& vItemInfoList )
{
	m_vInventoryItemInfoList.clear();
	m_vInventoryItemInfoList = vItemInfoList;

	for(size_t i = 0; i < m_vInventoryItemInfoList.size(); ++i)
	{
		ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( m_vInventoryItemInfoList[i].m_iMagicCode );
		if( !pEtcItem ) 
			return;
		m_szIconName[i] = pEtcItem->GetIconName();

	}

	
}

void PracticeLobbyInfoWnd::SetINIText( ioINILoader kLoader, DWORD dwIndex )
{
	char szBuf[MAX_PATH];
	char szText[MAX_BUFFER_SIZE];

	ioWnd* pWnd = this;
	ioUITitle* pTitle = pWnd->FindChildWnd(ID_START_BTN)->GetTitle();
	kLoader.SetTitle( "common" );
	
	int iCount = g_PracticeMgr.GetPracticeInfo()[dwIndex-1].iPracticeCount;
	if( iCount > 0 )
		kLoader.LoadString("FreeTitle", "", szBuf, MAX_PATH);
	else
		kLoader.LoadString("PaidTitle", "", szBuf, MAX_PATH);
	pTitle->SetText(szBuf,"");

	sprintf_s( szBuf, "Practice%d", dwIndex);
	kLoader.SetTitle( szBuf );
	kLoader.LoadString("Title_text", "", szBuf, MAX_PATH);	

	pTitle = pWnd->GetTitle();
	pTitle->SetText(szBuf,"");

	kLoader.LoadString_e("Guide_text", "", szText, MAX_BUFFER_SIZE);
	m_szGuide.Set(0, 0, szText, FONT_SIZE_12, 165, TS_NORMAL, TCT_DEFAULT_DARKGRAY, 0, TAT_LEFT, TVA_TOP, 3);
	m_iCurPos = 0;

	int iLine = m_szGuide.GetLineSize();
	if( iLine > 8 )
	{
		m_szGuide.Set(0, 0, szText, FONT_SIZE_12, 150, TS_NORMAL, TCT_DEFAULT_DARKGRAY, 0, TAT_LEFT, TVA_TOP, 3);
		iLine = m_szGuide.GetLineSize();

		ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_SCROLL );
		if( pScroll )
		{
			pScroll->SetScrollRange( 0, iLine - 7 );
			pScroll->SetScrollPos( m_iCurPos );
			pScroll->ShowWnd();
		}
	}
	else
	{
		HideChildWnd( ID_SCROLL );
	}
}

void PracticeLobbyInfoWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	char szBuf[MAX_PATH]="";
	for( int i = 0; i < 8; ++i )
	{
		wsprintf( szBuf, "TextInfo%d", i+1);
		m_szText[i] = xElement.GetStringAttribute(szBuf);
	}
	wsprintf( szBuf, "Rank");
	m_szRank = xElement.GetStringAttribute(szBuf);
}

void PracticeLobbyInfoWnd::SetHelpText()
{
	ioComplexStringPrinter kPrinter;
	kPrinter.AddTextPiece(FONT_SIZE_12, "test 123");
	kPrinter.ClearList();
	//g_ChatMgr.SetMsg("", kPrinter);
	m_ChatList.clear();
}

PracticeRankingWnd::PracticeRankingWnd( void )
{
	m_pLine = NULL;
	m_pIcon = NULL;
	m_pIconBack = NULL;
	m_pRankTextBg1 = NULL;
	m_pRankTextBg2 = NULL;
	m_pRankTextBg3 = NULL;
	m_pRewardTextBg1 = NULL;
	m_pRewardTextBg2 = NULL;
	m_pRewardTextBg3 = NULL;
	m_pRewardIconBg = NULL;
	m_iCurPage = 1;
	m_iModeMax = g_PracticeMgr.GetPracticeCount();
	m_iMaxPage = m_iModeMax/5;
	if(m_iModeMax % 5 != 0)
		m_iMaxPage+=1;

	ioINILoader kLoader( "config/sp2_Practice_Lobby.ini" );
	
	
	char szBuf[MAX_PATH];

	for(int i = 0; i < m_iModeMax; ++i)
	{
		sprintf_s( szBuf, "Practice%d", i+1);
		kLoader.SetTitle(szBuf);
		sprintf_s( szBuf, "Title_text");
		kLoader.LoadString(szBuf,"",szBuf,MAX_PATH);
		m_szModeName[i] = szBuf;
	}

	SP2Packet kPacket( CTPK_PRACTICE_RANKING_LIST );
	TCPNetwork::SendToServer( kPacket );
}

PracticeRankingWnd::~PracticeRankingWnd( void )
{
	SAFEDELETE(m_pIconBack);
	SAFEDELETE(m_pLine);
	SAFEDELETE(m_pIcon);
	SAFEDELETE(m_pRankTextBg1);
	SAFEDELETE(m_pRankTextBg2);
	SAFEDELETE(m_pRankTextBg3);
	SAFEDELETE( m_pRewardTextBg1 );
	SAFEDELETE( m_pRewardTextBg2 );
	SAFEDELETE( m_pRewardTextBg3 );
	SAFEDELETE( m_pRewardIconBg );
	ClearIconMap();
}

void PracticeRankingWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "Line" )
	{
		SAFEDELETE( m_pLine );
		m_pLine = pImage;
	}
	else if( szType == "RankTextBg1" )
	{
		SAFEDELETE( m_pRankTextBg1 );
		m_pRankTextBg1 = pImage;
	}
	else if( szType == "RankTextBg2" )
	{
		SAFEDELETE( m_pRankTextBg2 );
		m_pRankTextBg2 = pImage;
	}
	else if( szType == "RankTextBg3" )
	{
		SAFEDELETE( m_pRankTextBg3 );
		m_pRankTextBg3 = pImage;
	}
	else if( szType == "RewardTextBg1" )
	{
		SAFEDELETE( m_pRewardTextBg1 );
		m_pRewardTextBg1 = pImage;
	}
	else if( szType == "RewardTextBg2" )
	{
		SAFEDELETE( m_pRewardTextBg2 );
		m_pRewardTextBg2 = pImage;
	}
	else if( szType == "RewardTextBg3" )
	{
		SAFEDELETE( m_pRewardTextBg3 );
		m_pRewardTextBg3 = pImage;
	}
	else if( szType == "RewardIconBg" )
	{
		SAFEDELETE(m_pRewardIconBg);
		m_pRewardIconBg = pImage;
	}
}

void PracticeRankingWnd::OnProcess( float fTimePerSec )
{
	ioWnd::OnProcess(fTimePerSec);
}

void PracticeRankingWnd::OnRender()
{
	ioWnd::OnRender();

	int iPosX = GetDerivedPosX();
	int iPosY = GetDerivedPosY();
	if(m_pLine)
		m_pLine->Render(iPosX,iPosY);

	RenderRankBoard(iPosX, iPosY);
	RenderRewardIconBoard(iPosX, iPosY);
	RenderRewardBoard(iPosX, iPosY);
}

void PracticeRankingWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch(dwID)
	{
	case ID_EXIT:
		if( cmd == IOBN_BTNUP)
		{
			if(pWnd->GetParent())
				pWnd->GetParent()->HideWnd();
		}
	case ID_PREV_BTN:
		if( cmd == IOBN_BTNUP)
			SetPage(false);
		break;
	case ID_NEXT_BTN:
		if( cmd == IOBN_BTNUP)
			SetPage(true);
		break;
	}
}

void PracticeRankingWnd::SetPage(bool bNext/*next = true*/)
{
	if(!bNext)
	{
		if(m_iCurPage <= 1)
		{
			m_iCurPage = 1;
			return;
		}
		else
			--m_iCurPage;
	}
	else
	{
		if(m_iCurPage >= m_iMaxPage)
		{
			m_iCurPage = m_iMaxPage;
			return;
		}
		else
			++m_iCurPage;
	}
}

void PracticeRankingWnd::RenderRankBoard( int iX, int iY )
{
	int iRankTextX = 17;
	int iRankTextY = 64;

	/*if(m_pRankTextBg3)//드래그 온 효과 안씀
		m_pRankTextBg3->Render(iPosX + iRankTextX, iPosY + iRankTextY+21, UI_RENDER_MULTIPLY);*/
	if(m_pRankTextBg1 && m_pRankTextBg2)
	{
		for(int i = 1; i < 6; i+=2)
		{
			m_pRankTextBg1->Render(iX + iRankTextX, iY + iRankTextY+21*i, UI_RENDER_MULTIPLY);
			m_pRankTextBg2->Render(iX   + iRankTextX, iY + iRankTextY+21*i, UI_RENDER_MULTIPLY);
		}
	}	
	g_FontMgr.SetTextStyle(TS_NORMAL);
	g_FontMgr.SetAlignType(TAT_LEFT);
	g_FontMgr.SetTextColor(TCT_DEFAULT_BLUE);
	g_FontMgr.PrintTextWidthCut( iX + iRankTextX, iY + iRankTextY, FONT_SIZE_14, 172, m_szTextInfo[0].c_str() );
	char szBuf[MAX_PATH] = "";
	wsprintf( szBuf, "%d", m_iCurPage );
	g_FontMgr.SetTextStyle(TS_NORMAL);
	g_FontMgr.SetAlignType(TAT_CENTER);
	g_FontMgr.SetTextColor(TCT_DEFAULT_DARKGRAY);
	g_FontMgr.PrintTextWidthCut( iX + iRankTextX+24+78, iY + iRankTextY+137, FONT_SIZE_12, 30, szBuf );

	for(int i = 0 ; i < 5; ++i)
	{
		//배열로 현재페이지 갯수를 보여주기
		char szBuf[MAX_PATH] = "";
		int iIndex = 5*(m_iCurPage-1)+i;
		
		if(iIndex >= m_iModeMax)
			wsprintf( szBuf, "");
		else
			wsprintf( szBuf, m_szModeName[iIndex].c_str());
		
		g_FontMgr.SetTextStyle(TS_NORMAL);
		g_FontMgr.SetAlignType(TAT_LEFT);
		g_FontMgr.SetTextColor(TCT_DEFAULT_DARKGRAY);
		g_FontMgr.PrintTextWidthCut( iX + iRankTextX+10, iY + iRankTextY+24+21*i, FONT_SIZE_12, 120, szBuf );
		//szText[i+m_iCurPage*5]
		int iRank = 0;
		if(iIndex >= m_iModeMax)
		{

		}
		else
		{
			iRank = g_PracticeMgr.GetMyPreRankInfo()[iIndex].iPracticeRank;
			if(iRank == 0)
				wsprintf( szBuf, "기록없음");
			else
				wsprintf( szBuf, m_szRank.c_str(), iRank );
		}

		//순위 or 없으면 - 
		g_FontMgr.SetAlignType(TAT_RIGHT);
		g_FontMgr.PrintTextWidthCut( iX + iRankTextX+193, iY + iRankTextY+24+21*i, FONT_SIZE_12, 120, szBuf );
	}
}

void PracticeRankingWnd::RenderRewardBoard( int iX, int iY )
{
	int iRewardTextX = 17;
	int iRewardTextY = 249;

	g_FontMgr.SetTextStyle(TS_NORMAL);
	g_FontMgr.SetAlignType(TAT_LEFT);
	g_FontMgr.SetTextColor(TCT_DEFAULT_BLUE);
	g_FontMgr.PrintTextWidthCut( iX + iRewardTextX, iY + iRewardTextY, FONT_SIZE_14, 172, m_szTextInfo[2].c_str() );

	if(m_pRewardTextBg1 && m_pRewardTextBg2)
	{
		for(int i = 0; i < 5; i+=2)
		{
			m_pRewardTextBg1->SetSize(146,21);
			m_pRewardTextBg1->Render(iX + iRewardTextX, iY + iRewardTextY+24+i*21, UI_RENDER_MULTIPLY);
			m_pRewardTextBg2->Render(iX + iRewardTextX, iY + iRewardTextY+24+i*21, UI_RENDER_MULTIPLY);
			m_pRewardTextBg1->SetSize(102,21);
			m_pRewardTextBg1->Render(iX + iRewardTextX + 147, iY + iRewardTextY+24+i*21, UI_RENDER_MULTIPLY);
			m_pRewardTextBg2->Render(iX + iRewardTextX + 147 , iY + iRewardTextY+24+i*21, UI_RENDER_MULTIPLY);
			m_pRewardTextBg1->SetSize(320,21);
			m_pRewardTextBg1->Render(iX + iRewardTextX + 250, iY + iRewardTextY+24+i*21, UI_RENDER_MULTIPLY);
			m_pRewardTextBg2->Render(iX + iRewardTextX + 250, iY + iRewardTextY+24+i*21, UI_RENDER_MULTIPLY);
		}
	}

	g_FontMgr.SetAlignType(TAT_CENTER);
	g_FontMgr.SetTextColor(TCT_DEFAULT_DARKGRAY);
	g_FontMgr.PrintTextWidthCut( iX + iRewardTextX+73, iY + iRewardTextY+27, FONT_SIZE_13, 120, m_szRewardTitle[0].c_str() );
	g_FontMgr.PrintTextWidthCut( iX + iRewardTextX+198, iY + iRewardTextY+27, FONT_SIZE_13, 74, m_szRewardTitle[1].c_str() );
	g_FontMgr.PrintTextWidthCut( iX + iRewardTextX+410, iY + iRewardTextY+27, FONT_SIZE_13, 304, m_szRewardTitle[2].c_str() );

	for(int i = 1; i < 5; ++i)
	{
		g_FontMgr.SetAlignType(TAT_LEFT);
		if(i == 1)
		{
			g_FontMgr.SetAlignType(TAT_CENTER);
			g_FontMgr.PrintTextWidthCut( iX + iRewardTextX+73, iY + iRewardTextY+27+i*21, FONT_SIZE_12, 50, m_szRewardInfo2[i-1].c_str() );
		}
		else
		{
			g_FontMgr.PrintTextWidthCut( iX + iRewardTextX+20, iY + iRewardTextY+27+i*21, FONT_SIZE_12, 50, m_szRewardInfo[i-1].c_str() );
			g_FontMgr.PrintTextWidthCut( iX + iRewardTextX+69, iY + iRewardTextY+27+i*21, FONT_SIZE_12, 8, "~" );
			g_FontMgr.SetAlignType(TAT_RIGHT);
			g_FontMgr.PrintTextWidthCut( iX + iRewardTextX+130, iY + iRewardTextY+27+i*21, FONT_SIZE_12, 50, m_szRewardInfo2[i-1].c_str() );
		}
		g_FontMgr.SetAlignType(TAT_LEFT);
		g_FontMgr.PrintTextWidthCut( iX + iRewardTextX+186, iY + iRewardTextY+27+i*21, FONT_SIZE_12, 74, m_szRewardNameInfo[i-1].c_str() );
		g_FontMgr.PrintTextWidthCut( iX + iRewardTextX+258, iY + iRewardTextY+27+i*21, FONT_SIZE_12, 304, m_szRewardDetail[i-1].c_str() );
	}
	g_FontMgr.SetTextColor(TCT_DEFAULT_GRAY);
	g_FontMgr.PrintTextWidthCut( iX + iRewardTextX, iY + iRewardTextY+135, FONT_SIZE_12, 570, m_szHelp.c_str() );
	
}

void PracticeRankingWnd::RenderRewardIconBoard( int iX, int iY )
{
	int iRewardIconX = 231;
	int iRewardIconY = 64;

	g_FontMgr.SetTextStyle(TS_NORMAL);
	g_FontMgr.SetAlignType(TAT_LEFT);
	g_FontMgr.SetTextColor(TCT_DEFAULT_BLUE);
	g_FontMgr.PrintTextWidthCut( iX + iRewardIconX, iY + iRewardIconY, FONT_SIZE_14, 172, m_szTextInfo[1].c_str() );

	g_FontMgr.SetAlignType(TAT_RIGHT);
	g_FontMgr.SetTextColor(TCT_DEFAULT_GRAY);
	ioHashString szEndDate="";
	ioHashString szStartDate="";
	char szBuf[MAX_PATH]="";
	if(!g_PracticeMgr.GetMyPreRankInfo().empty())
	{
		szEndDate = g_PracticeMgr.GetMyPreRankInfo()[0].strEndDate;
		szStartDate = g_PracticeMgr.GetMyPreRankInfo()[0].strStartDate;
		wsprintf( szBuf, "기준일 %s ~ %s", szStartDate.c_str(), szEndDate.c_str());
	}
	
	g_FontMgr.PrintTextWidthCut( iX + iRewardIconX+355, iY + iRewardIconY + 4, FONT_SIZE_12, 200, szBuf);
	
	if(m_pRewardIconBg)
	{
		for(int i = 0; i < 4; ++i)
		{
			m_pRewardIconBg->Render(iX+iRewardIconX+53+i*83,iY+iRewardIconY+70, UI_RENDER_MULTIPLY, TFO_BILINEAR );
			g_FontMgr.SetAlignType(TAT_CENTER);
			g_FontMgr.SetTextColor(TCT_DEFAULT_GRAY);
			g_FontMgr.PrintTextWidthCut( iX+iRewardIconX+53+i*83, iY + iRewardIconY+112, FONT_SIZE_12, 72, m_szRewardNameInfo[i].c_str() );//파일명 배열
			g_FontMgr.SetTextColor(TCT_DEFAULT_BLUE);
			int iCount = 0;
			auto it = m_mapReward.find(m_iItemCode[i]);
			if(it != m_mapReward.end())
				iCount = it->second;
			if(iCount == 0)			
				g_FontMgr.PrintTextWidthCut( iX+iRewardIconX+53+i*83, iY + iRewardIconY+126, FONT_SIZE_14, 72, "0개" );//개수				
			else
			{
				wsprintf( szBuf, "%d개", iCount);
				g_FontMgr.PrintTextWidthCut( iX+iRewardIconX+53+i*83, iY + iRewardIconY+126, FONT_SIZE_14, 72, szBuf );//개수				
			}



		}
	}

	for(size_t i = 0; i < 4; ++i)
	{			
		m_pIcon = GetImage(m_szIconName[i]);
		if(m_pIcon)
		{
			m_pIcon->Render(iX+iRewardIconX+53+i*83,iY+iRewardIconY+70, UI_RENDER_NORMAL, TFO_BILINEAR );
		}
	}
}

ioUIRenderImage* PracticeRankingWnd::GetImage( const ioHashString &szIconName )
{
	if ( szIconName.IsEmpty() )
		return NULL;

	PracticeIconMap::iterator iter = m_RewardIconMap.find( szIconName );
	if( iter != m_RewardIconMap.end() )
		return iter->second;

	ioUIRenderImage *pIcon = g_UIImageSetMgr.CreateImageByFullName( szIconName );
	if( pIcon )
	{
		m_RewardIconMap.insert( PracticeIconMap::value_type( szIconName, pIcon ) );
		return pIcon;
	}

	return NULL;
}

void PracticeRankingWnd::ClearIconMap()
{
	PracticeIconMap::iterator iCreate = m_RewardIconMap.begin();
	for( ; iCreate != m_RewardIconMap.end() ; ++iCreate )
		 ( iCreate->second );
	m_RewardIconMap.clear();
}

void PracticeRankingWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_mapReward.clear();
	char szBuf[MAX_PATH]="";
	for( int i = 0; i < 4; ++i )
	{
		wsprintf_e( szBuf, "RewardInfo%d", i+1);
		m_szRewardInfo[i] = xElement.GetStringAttribute(szBuf);
		wsprintf_e( szBuf, "RewardInfo_%d", i+1);
		m_szRewardInfo2[i] = xElement.GetStringAttribute(szBuf);
		wsprintf_e( szBuf, "RewardNameInfo%d", i+1);
		m_szRewardNameInfo[i] = xElement.GetStringAttribute(szBuf);
		wsprintf_e( szBuf, "RewardDetail%d", i+1);
		m_szRewardDetail[i] = xElement.GetStringAttribute(szBuf);
		wsprintf_e( szBuf, "List%d", i+1);
		m_szRewardTitle[i] = xElement.GetStringAttribute(szBuf);
		wsprintf_e( szBuf, "Item%d", i+1);
		m_iItemCode[i] = xElement.GetIntAttribute(szBuf);
		m_mapReward.insert( std::make_pair(m_iItemCode[i], 0) );
	}
	for( int i = 0 ; i < 3; ++i )
	{
		wsprintf( szBuf, "TextInfo%d", i+1);
		m_szTextInfo[i] =  xElement.GetStringAttribute(szBuf);
	}
	wsprintf_e( szBuf, "Rank");
	m_szRank = xElement.GetStringAttribute(szBuf);
	wsprintf_e( szBuf, "Help");
	m_szHelp = xElement.GetStringAttribute(szBuf);
}

void PracticeRankingWnd::iwm_show()
{

	//보상 갯수만큼, 서버에서 받아올 구조체로
	for(size_t i = 0; i < 4; ++i)
	{
		//여기 보상 아이콘 수정
		ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( m_iItemCode[i] );
		if( !pEtcItem ) 
			continue;
		m_szIconName[i] = pEtcItem->GetIconName();
	}

	m_mapReward.clear();
		
	for(size_t i = 0; i < g_PracticeMgr.GetMyPreRankInfo().size(); ++i)
	{
		//전시즌 보상받을 수련장 갯수
		size_t iPresentCount = g_PracticeMgr.GetMyPreRankInfo()[i].vecPresent.size();
		for( int j = 0; j < iPresentCount; ++j )
		{
			int iRewardCount = g_PracticeMgr.GetMyPreRankInfo()[i].vecPresent[j].iRewardCount;
			if(iRewardCount == 0)
				continue;
			int iCode = g_PracticeMgr.GetMyPreRankInfo()[i].vecPresent[j].iRewardCode;
			m_mapReward[iCode] += iRewardCount;
		}
	}
	
}
