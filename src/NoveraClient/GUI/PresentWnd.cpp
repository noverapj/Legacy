

#include "StdAfx.h"

#include "../ioUserSpiritManager.h"
#include "../ioExtendSoundManager.h"

#include "TotalResultMainWnd.h"
#include "PresentWnd.h"

PresentAlarmWnd::PresentAlarmWnd()
{
	m_pBottomFrm   = NULL;
	m_pStatePinkFrm= NULL;

	m_pPresentBack = NULL;
	m_pPresentIcon = NULL;
	m_pSubIcon = NULL;
	m_pPresentNum  = NULL;
	m_pPresentPlus = NULL;
	m_pPresentCustom = NULL;
	m_pCostumeMark = NULL;

	m_dwIndex = 0;    
	m_dwSlotIndex = 0;
	m_iPresentType = 0;   
	m_iPresentMent = 0;   
	m_iPresentValue1 = 0; 
	m_iPresentValue2 = 0;
	m_iPresentValue3 = 0; 
	m_iPresentValue4 = 0;
	m_dwLimitDate = 0;      

	m_dwHideDelayTime = 0;
	m_bRePosition = true;

	m_nGradeType = 0;
}

PresentAlarmWnd::~PresentAlarmWnd()
{
	SAFEDELETE( m_pBottomFrm );
	SAFEDELETE( m_pStatePinkFrm );
	SAFEDELETE( m_pPresentBack );
	SAFEDELETE( m_pPresentIcon );
	SAFEDELETE( m_pSubIcon );
	SAFEDELETE( m_pPresentNum );
	SAFEDELETE( m_pPresentPlus );
	SAFEDELETE( m_pPresentCustom );
	SAFEDELETE( m_pCostumeMark );
}

void PresentAlarmWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_EXIT:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
			g_PresentMgr.ClearAlarmPresentData( m_dwIndex, m_dwSlotIndex );			
		}
		break;
	case ID_LINK:
		if( cmd == IOBN_BTNUP )
		{
			MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
			if( pInvenWnd )
			{
				pInvenWnd->ShowPresentTabDirect();
			}

			HideWnd();
			g_PresentMgr.ClearAlarmPresentData( m_dwIndex, m_dwSlotIndex );
		}
		break;
	}
}

void PresentAlarmWnd::SetPresentIcon()
{
	SAFEDELETE( m_pPresentIcon );
	SAFEDELETE( m_pSubIcon );

	ioHashString szIconName = g_PresentMgr.GetPresentIconText(m_iPresentType, m_iPresentValue1, m_iPresentValue2);	
	m_pPresentIcon = g_UIImageSetMgr.CreateImageByFullName( szIconName );
	m_pSubIcon = g_UIImageSetMgr.CreateImageByFullName( g_PresentMgr.GetPresentSubIconText( m_iPresentType, m_iPresentValue1, m_iPresentValue2 ) );
}

void PresentAlarmWnd::ReSizePresentUI()
{
	// UI 사이즈 변경 
	int iMentSize = g_PresentMgr.GetMentMaxLine( m_iPresentMent );
	SetSize( GetWidth(), 283 + ( iMentSize * 18 ) );

	ioWnd *pBtnWnd = FindChildWnd( ID_LINK );
	if( pBtnWnd )
		pBtnWnd->SetWndPos( pBtnWnd->GetXPos(), GetHeight() - 48 );

	if( m_bRePosition )
	{
		SetWndPos( ( Setting::Width()/2 ) - (GetWidth()/2) , (Setting::Height()/2) - (GetHeight()/2)  );
	}
}

void PresentAlarmWnd::ShowPresentAlarmWnd()
{
	m_dwSlotIndex = 0;
	m_dwIndex = g_PresentMgr.FirstAlarmPresentData( IsSpecialAwardBlock(), m_dwSlotIndex );
	
	g_PresentMgr.GetPresentDataToIndex( m_dwIndex, m_dwSlotIndex, m_szSendID, m_iPresentType, m_iPresentMent, m_iPresentValue1,
										m_iPresentValue2, m_iPresentValue3, m_iPresentValue4, m_dwLimitDate );

	if( m_dwIndex == 0 && m_dwSlotIndex == 0 )
	{
		HideWnd();
		return;
	}
	else
	{
		ReSizePresentUI();
		SetPresentIcon();
		SetGradeType( m_iPresentType, m_iPresentValue1 );
	}

	// 낚시로 인해 낚은 아이템일 경우 Title Text 변경
	if( m_iPresentMent == PRESENT_FISHING_ITEM_SUCCESS )
		SetTitleText( STR(1) );
	else
		SetTitleText( STR(2) );

	g_ExSoundMgr.PlaySound( ExSound::EST_PRESENT_POPUP );
	ShowWnd();
}

void PresentAlarmWnd::ShowPresentAlarmWnd( const ioHashString szID, short iPresentType, short iPresentMent,
										   int iValue1, int iValue2, int iValue3, int iValue4 )
{
	m_dwIndex = 0;
	m_dwSlotIndex = 0;

	m_szSendID = szID;
	m_iPresentType = iPresentType;
	m_iPresentMent = iPresentMent;
	m_iPresentValue1 = iValue1;
	m_iPresentValue2 = iValue2;
	m_iPresentValue3 = iValue3;
	m_iPresentValue4 = iValue4;

	ReSizePresentUI();
	SetPresentIcon();
	SetGradeType( m_iPresentType, m_iPresentValue1 );
	ShowWnd();

	// 낚시로 인해 낚은 아이템일 경우 Title Text 변경
	if( iPresentMent == PRESENT_FISHING_ITEM_SUCCESS )
		SetTitleText( STR(1) );
	else
		SetTitleText( STR(2) );
}

void PresentAlarmWnd::SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate /* = false  */ )
{
	ioWnd::SetPositionToDefault( iParentWidth, iParentHeight, bChildMove, bCreate );
	if( !bCreate )
	{
		if( IsShow() )
		{
			ReSizePresentUI();
		}
	}
}

bool PresentAlarmWnd::iwm_esc()
{
	if( !IsShow() ) return false;
	if( ioWnd::iwm_esc() )
	{
		g_PresentMgr.ClearAlarmPresentData( m_dwIndex, m_dwSlotIndex );
		return true;
	}
	return false;
}

void PresentAlarmWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "BottomFrm" )
	{
		SAFEDELETE( m_pBottomFrm );
		m_pBottomFrm = pFrame;
	}
	else if( szType == "StatePinkFrm" )
	{
		SAFEDELETE( m_pStatePinkFrm );
		m_pStatePinkFrm = pFrame;
	}
	else
		ioWnd::AddRenderFrame( szType, pFrame );
}

void PresentAlarmWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "IconBack" )
	{
		SAFEDELETE( m_pPresentBack );
		m_pPresentBack = pImage;
	}
	else if( szType == "PresentNum" )
	{
		SAFEDELETE( m_pPresentNum );
		m_pPresentNum = pImage;
	}
	else if( szType == "PresentPlus" )
	{
		SAFEDELETE( m_pPresentPlus );
		m_pPresentPlus = pImage;
	}
	else if( szType == "custom_image" )
	{
		SAFEDELETE( m_pPresentCustom );
		m_pPresentCustom = pImage;
	}
	else if( szType == "costume_image" )
	{
		SAFEDELETE( m_pCostumeMark );
		m_pCostumeMark = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}

bool PresentAlarmWnd::IsSpecialAwardBlock()
{
	// 시상식 롤링중에는 특별상 선물을 표시하지 않음.
	TotalResultMainWnd *pTotalResultMainWnd = dynamic_cast<TotalResultMainWnd*>(g_GUIMgr.FindWnd( TOTAL_RESULT_MAIN_WND ));
	if( !pTotalResultMainWnd )
		return false;

	return pTotalResultMainWnd->IsPresentAlarmBlock();
}

bool PresentAlarmWnd::IsShowDelay()
{
	// 선물함을 보고 있을 때는 선물 알람 표시하지 않음
	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
	if( pInvenWnd && pInvenWnd->IsShowPresentTab() )
	{
		return true;
	}

	return g_MyInfo.IsTutorialUser();
}

void PresentAlarmWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
	{
		if( IsShowDelay() ) return;

		if( FRAMEGETTIME() - m_dwHideDelayTime < 100 ) return;

		if( g_PresentMgr.IsAlarmPresentData( IsSpecialAwardBlock() ) )
		{
			ShowPresentAlarmWnd();
		}
		else
		{
			m_bRePosition = true;
		}
		return;
	}

	ioMovingWnd::OnProcess( fTimePerSec );
}

void PresentAlarmWnd::OnRenderPresent( int iXPos, int iYPos )
{
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );

	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + GetWidth() / 2, iYPos + 144, FONT_SIZE_13, g_PresentMgr.GetPresentValue1Text( m_iPresentType, m_iPresentValue1, m_iPresentValue2 ).c_str() );
	
	// UI코스튬 관련 (선물 알람 설명)
	if ( m_iPresentType == PRESENT_PET )
		g_FontMgr.PrintText( iXPos + GetWidth() / 2, iYPos + 162, FONT_SIZE_13, "Lv %d %s", m_iPresentValue2/10000, g_PresentMgr.GetPresentValue2Text( m_iPresentType, m_iPresentValue1, m_iPresentValue2 ).c_str() );
	else
		g_FontMgr.PrintText( iXPos + GetWidth() / 2, iYPos + 162, FONT_SIZE_13, g_PresentMgr.GetPresentValue2Text( m_iPresentType, m_iPresentValue1, m_iPresentValue2 ).c_str() );
}

void PresentAlarmWnd::OnRenderPeso( int iXPos, int iYPos )
{
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );

	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + GetWidth() / 2, iYPos + 153, FONT_SIZE_13, g_PresentMgr.GetPresentValue2Text( m_iPresentType, m_iPresentValue1, m_iPresentValue2 ).c_str() );
}

void PresentAlarmWnd::OnRenderGradeExp( int iXPos, int iYPos )
{
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );

	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + GetWidth() / 2, iYPos + 153, FONT_SIZE_13, g_PresentMgr.GetPresentValue2Text( m_iPresentType, m_iPresentValue1, m_iPresentValue2 ).c_str() );
}

void PresentAlarmWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_pPresentBack )
		m_pPresentBack->Render( iXPos, iYPos, UI_RENDER_MULTIPLY );

	g_UIImageRenderMgr.RenderGradeIconBack( m_nGradeType, iXPos + GetWidth() / 2, iYPos + 98 );

	if( m_pPresentIcon )
		m_pPresentIcon->Render( iXPos + GetWidth() / 2, iYPos + 98 );
	if( m_pSubIcon )
		m_pSubIcon->Render( iXPos + GetWidth() / 2, iYPos + 98 );

	if( m_pBottomFrm )
		m_pBottomFrm->Render( iXPos, iYPos + ( GetHeight() - 65 ), UI_RENDER_MULTIPLY );

	switch( m_iPresentType )
	{
	case PRESENT_SOLDIER:
	case PRESENT_DECORATION:
	case PRESENT_ETC_ITEM:
	case PRESENT_MEDALITEM:
	case PRESENT_ALCHEMIC_ITEM:
	case PRESENT_PET:
	case PRESENT_SPIRIT:
		OnRenderPresent( iXPos, iYPos );
		break;
	case PRESENT_PESO:
	case PRESENT_BONUS_CASH:
		OnRenderPeso( iXPos, iYPos );
		break;
	case PRESENT_EXTRAITEM:
		{
			OnRenderPresent( iXPos, iYPos );
			// 성장 타입 표시
			if( m_iPresentValue2 / PRESENT_EXTRAITEM_DIVISION_1 != 0 )
			{
				if( m_pStatePinkFrm )
					m_pStatePinkFrm->Render( (iXPos + GetWidth() / 2) - 45, (iYPos + 98) - 41 );

				g_FontMgr.SetBkColor( 0, 0, 0 );
				g_FontMgr.SetTextStyle( TS_NORMAL );
				g_FontMgr.SetAlignType( TAT_CENTER );

				g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
				g_FontMgr.PrintText( (iXPos + GetWidth() / 2) - 20, (iYPos + 98) - 40, FONT_SIZE_11, STR(2), ( m_iPresentValue2 % PRESENT_EXTRAITEM_DIVISION_1 ) / PRESENT_EXTRAITEM_DIVISION_2 );
			}

			// 성장값 표시
			if( m_pPresentNum && m_pPresentPlus )
			{
				enum
				{
					TEXT_SIZE	= 15,
					TEXT_GAP	= 5,
					PLUS_SIZE   = 14,
				};
				int iReinforce = ( m_iPresentValue2 % PRESENT_EXTRAITEM_DIVISION_1 ) / PRESENT_EXTRAITEM_DIVISION_2;
				int iCurPos = 0;
				int iDefaultPos = iXPos + 103;

				int iTotalSize = PLUS_SIZE;      
				if( iReinforce < 10 )
					iTotalSize += TEXT_SIZE;
				else if( iReinforce < 100 )
					iTotalSize += TEXT_SIZE * 2;
				else
					iTotalSize += TEXT_SIZE * 3;

				iCurPos = iDefaultPos - (iTotalSize / 2);
				m_pPresentNum->RenderNum( iCurPos + ( PLUS_SIZE - ( TEXT_GAP - 1 ) ), iYPos + 112, iReinforce, -TEXT_GAP );
				m_pPresentPlus->Render( iCurPos, iYPos + 112 );
			}

			if( m_iPresentValue3 > 0 || m_iPresentValue4 > 0 )
			{
				if( m_pPresentCustom )
					m_pPresentCustom->Render( ( iXPos + GetWidth() / 2 ) + 7, iYPos + 55 );
			}
		}
		break;
	case PRESENT_EXTRAITEM_BOX:
		{
			OnRenderPresent( iXPos, iYPos );
		}
		break;
	case PRESENT_RANDOM_DECO:
		{
			OnRenderPresent( iXPos, iYPos );
		}
		break;
	case PRESENT_GRADE_EXP:
		OnRenderGradeExp( iXPos, iYPos );
		break;
	case PRESENT_COSTUME:
		{
			OnRenderPresent( iXPos, iYPos );
			
			if( m_iPresentValue3 > 0 || m_iPresentValue4 > 0 )
			{
				if( m_pPresentCustom )
					m_pPresentCustom->Render( ( iXPos + GetWidth() / 2 ) + 7, iYPos + 55 );
			}

			// UI코스튬 관련 (코스튬 마크)
			if( m_pCostumeMark )
				m_pCostumeMark->Render( iXPos + GetWidth() / 2, iYPos + 98 );
		}
		break;
	case PRESENT_COSTUME_BOX:
		OnRenderPresent( iXPos, iYPos );
		break;
	case PRESENT_ACCESSORY:
		OnRenderPresent( iXPos, iYPos );
		break;
	}

	int iMaxLine = g_PresentMgr.GetMentMaxLine( m_iPresentMent );
	for(int i = 0;i < iMaxLine;i++)
	{
		ioComplexStringPrinter kPrinter;
		kPrinter.SetTextStyle( TS_NORMAL );
		kPrinter.SetBkColor( 0, 0, 0 );	
		g_PresentMgr.GetMentLineText( m_iPresentMent, i, kPrinter );
		kPrinter.PrintFullText( iXPos + GetWidth() / 2, iYPos + 189 + ( i * 18 ), TAT_CENTER );
	}

	// HARDCODE : 거래소 거래완료시 구매자를 감추기 위해 예외처리.
	if( m_iPresentMent == PRESENT_TRADE_TIMEOUT_MENT || m_iPresentMent == PRESENT_TRADE_SELL_MENT )
		return;

	if( !m_szSendID.IsEmpty() )
	{
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetTextColor( TCT_DEFAULT_RED );
		g_FontMgr.PrintText( iXPos + GetWidth() / 2, iYPos + 189 + ( iMaxLine * 18 ), FONT_SIZE_13, STR(1), m_szSendID.c_str() );
	}
}

void PresentAlarmWnd::iwm_show()
{
	m_bRePosition = false;

	if( ioPlayMode::GetModeType() == MT_ARENA && g_BattleRoomMgr.IsBattleModePlaying() )
		SetChildInActive( ID_LINK );
	else
		SetChildActive( ID_LINK );
}

void PresentAlarmWnd::iwm_hide()
{
	m_dwHideDelayTime = FRAMEGETTIME();
}

void PresentAlarmWnd::SetGradeType( int nPresentType, int nPresentCode )
{
	m_nGradeType = g_PresentMgr.GetGradeType( nPresentType, nPresentCode );
}
//////////////////////////////////////////////////////////////////////////
PresentDeleteWnd::PresentDeleteWnd()
{
	m_pStatePinkFrm = NULL;

	m_pIconBack		= NULL;
	m_pPresentIcon	= NULL;
	m_pSubIcon = NULL;
	m_pPresentNum	= NULL;
	m_pPresentPlus	= NULL;
	m_pPresentCustom = NULL;
	m_pCostumeMark = NULL;

	m_dwPresentIndex = 0;   
	m_dwPresentSlotIndex = 0;
	m_iPresentType = 0;   
	m_iPresentMent = 0;   
	m_iPresentValue1 = 0; 
	m_iPresentValue2 = 0;
	m_iPresentValue3 = 0; 
	m_iPresentValue4 = 0;
	m_dwLimitDate = 0;      

	m_dwHideDelayTime = 0;
	m_nGradeType = 0;
}

PresentDeleteWnd::~PresentDeleteWnd()
{
	SAFEDELETE( m_pStatePinkFrm );
	SAFEDELETE( m_pIconBack );
	SAFEDELETE( m_pPresentIcon );
	SAFEDELETE( m_pSubIcon );
	SAFEDELETE( m_pPresentNum );
	SAFEDELETE( m_pPresentPlus );
	SAFEDELETE( m_pPresentCustom );
	SAFEDELETE( m_pCostumeMark );
}

void PresentDeleteWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "StatePinkFrm" )
	{
		SAFEDELETE( m_pStatePinkFrm );
		m_pStatePinkFrm = pFrame;
	}
	else
		ioWnd::AddRenderFrame( szType, pFrame );
}

void PresentDeleteWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "IconBack" )
	{
		SAFEDELETE( m_pIconBack );
		m_pIconBack = pImage;
	}
	else if( szType == "PresentNum" )
	{
		SAFEDELETE( m_pPresentNum );
		m_pPresentNum = pImage;
	}
	else if( szType == "PresentPlus" )
	{
		SAFEDELETE( m_pPresentPlus );
		m_pPresentPlus = pImage;
	}
	else if( szType == "custom_image" )
	{
		SAFEDELETE( m_pPresentCustom );
		m_pPresentCustom = pImage;
	}
	else if( szType == "costume_image" )
	{
		SAFEDELETE( m_pCostumeMark );
		m_pCostumeMark = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}

void PresentDeleteWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_EXIT:
	case ID_CLOSE:
		if( cmd == IOBN_BTNUP )
		{
			g_PresentMgr.RemoveDeletePresentData( m_dwPresentIndex, m_dwPresentSlotIndex );			
			HideWnd();
		}
		break;
	case ID_ALL_CLEAR:
		if( cmd == IOBN_BTNUP )
		{
			g_PresentMgr.AllRemoveDeletePresentData();
			HideWnd();
		}
		break;
	}
}

void PresentDeleteWnd::SetPresentIcon()
{
	SAFEDELETE( m_pPresentIcon );
	SAFEDELETE( m_pSubIcon );
	ioHashString szIconName = g_PresentMgr.GetPresentIconText(m_iPresentType, m_iPresentValue1, m_iPresentValue2);	
	m_pPresentIcon = g_UIImageSetMgr.CreateImageByFullName( szIconName );
	m_pSubIcon = g_UIImageSetMgr.CreateImageByFullName( g_PresentMgr.GetPresentSubIconText( m_iPresentType, m_iPresentValue1, m_iPresentValue2 ) );
}

void PresentDeleteWnd::iwm_show()
{
	m_dwPresentSlotIndex = 0;
	m_dwPresentIndex = g_PresentMgr.FirstDeletePresentData( m_dwPresentSlotIndex );
	g_PresentMgr.GetDeletePresentDataToIndex( m_dwPresentIndex, m_dwPresentSlotIndex, m_szSendID, m_iPresentType, m_iPresentMent, m_iPresentValue1,
											  m_iPresentValue2, m_iPresentValue3, m_iPresentValue4, m_dwLimitDate );
	if( m_dwPresentIndex == 0 && m_dwPresentSlotIndex == 0 )
	{
		HideWnd();
	}
	else
	{
		SetPresentIcon();
		SetGradeType( m_iPresentType, m_iPresentValue1 );
	}
}

void PresentDeleteWnd::iwm_hide()
{
	m_dwHideDelayTime = FRAMEGETTIME();
}	

bool PresentDeleteWnd::iwm_esc()
{
	if( !IsShow() ) return false;
	if( ioWnd::iwm_esc() )
	{
		g_PresentMgr.RemoveDeletePresentData( m_dwPresentIndex, m_dwPresentSlotIndex );
		return true;
	}
	return false;
}

void PresentDeleteWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
	{
		if( FRAMEGETTIME() - m_dwHideDelayTime < 100 )
			return;

		if( g_PresentMgr.IsDeletePresentData() )
		{
			ShowWnd();
		}
		return;
	}
	ioMovingWnd::OnProcess( fTimePerSec );
}

void PresentDeleteWnd::OnRenderPresent( int iXPos, int iYPos )
{
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintTextWidthCut( iXPos + 103, iYPos + 144, FONT_SIZE_13, 173.0f, STR(1), g_PresentMgr.GetPresentValue1Text( m_iPresentType, m_iPresentValue1, m_iPresentValue2 ).c_str(),
																					   g_PresentMgr.GetPresentValue2Text( m_iPresentType, m_iPresentValue1, m_iPresentValue2 ).c_str() );
}

void PresentDeleteWnd::OnRenderPeso( int iXPos, int iYPos )
{
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintTextWidthCut( iXPos + 103, iYPos + 144, FONT_SIZE_13, 173.0f, g_PresentMgr.GetPresentValue2Text( m_iPresentType, m_iPresentValue1, m_iPresentValue2 ).c_str() );
}

void PresentDeleteWnd::OnRenderGradeExp( int iXPos, int iYPos )
{
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintTextWidthCut( iXPos + 103, iYPos + 144, FONT_SIZE_13, 173.0f, g_PresentMgr.GetPresentValue2Text( m_iPresentType, m_iPresentValue1, m_iPresentValue2 ).c_str() );
}

void PresentDeleteWnd::OnRenderExtraItem( int iXPos, int iYPos )
{
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintTextWidthCut( iXPos + 103, iYPos + 144, FONT_SIZE_13, 173.0f, STR(1), 
						 g_PresentMgr.GetPresentValue1Text( m_iPresentType, m_iPresentValue1, m_iPresentValue2 ).c_str(), ( m_iPresentValue2 % PRESENT_EXTRAITEM_DIVISION_1 ) / PRESENT_EXTRAITEM_DIVISION_2, 
						 g_PresentMgr.GetPresentValue2Text( m_iPresentType, m_iPresentValue1, m_iPresentValue2 ).c_str() );
}

void PresentDeleteWnd::OnRenderExtraItemBox( int iXPos, int iYPos )
{
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintTextWidthCut( iXPos + 103, iYPos + 144, FONT_SIZE_13, 173.0f, STR(1), g_PresentMgr.GetPresentValue1Text( m_iPresentType, m_iPresentValue1, m_iPresentValue2 ).c_str() );
}

void PresentDeleteWnd::OnRenderCostumeBox( int iXPos, int iYPos )
{
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintTextWidthCut( iXPos + 103, iYPos + 144, FONT_SIZE_13, 173.0f, "%s", g_PresentMgr.GetPresentValue1Text( m_iPresentType, m_iPresentValue1, m_iPresentValue2 ).c_str() );
}

void PresentDeleteWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_pIconBack )
	{
		m_pIconBack->Render( iXPos, iYPos, UI_RENDER_MULTIPLY );
	}

	g_UIImageRenderMgr.RenderGradeIconBack( m_nGradeType, iXPos + 103, iYPos + 98 );

	if( m_pPresentIcon )
	{
		m_pPresentIcon->Render( iXPos + 103, iYPos + 98 );
	}
	if( m_pSubIcon )
		m_pSubIcon->Render( iXPos + 103, iYPos + 98 );

	switch( m_iPresentType )
	{
	case PRESENT_SOLDIER:
	case PRESENT_DECORATION:
	case PRESENT_ETC_ITEM:
	case PRESENT_MEDALITEM:
	case PRESENT_ALCHEMIC_ITEM:
	case PRESENT_PET:
	case PRESENT_SPIRIT:
		OnRenderPresent( iXPos, iYPos );
		break;
	case PRESENT_PESO:
	case PRESENT_BONUS_CASH:
		OnRenderPeso( iXPos, iYPos );
		break;
	case PRESENT_EXTRAITEM:
		{
			OnRenderExtraItem( iXPos, iYPos );
			// 성장 타입 표시
			if( m_iPresentValue2 / PRESENT_EXTRAITEM_DIVISION_1 != 0 )
			{
				if( m_pStatePinkFrm )
					m_pStatePinkFrm->Render( (iXPos + 103) - 45, (iYPos + 98) - 41 );

				g_FontMgr.SetBkColor( 0, 0, 0 );
				g_FontMgr.SetTextStyle( TS_NORMAL );
				g_FontMgr.SetAlignType( TAT_CENTER );

				g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
				g_FontMgr.PrintText( (iXPos + 103) - 20, (iYPos + 98) - 40, FONT_SIZE_11, STR(2), ( m_iPresentValue2 % PRESENT_EXTRAITEM_DIVISION_1 ) / PRESENT_EXTRAITEM_DIVISION_2 );
			}
			// 성장값 표시
			if( m_pPresentNum && m_pPresentPlus )
			{
				enum
				{
					TEXT_SIZE	= 15,
					TEXT_GAP	= 5,
					PLUS_SIZE   = 14,
				};
				int iReinforce = ( m_iPresentValue2 % PRESENT_EXTRAITEM_DIVISION_1 ) / PRESENT_EXTRAITEM_DIVISION_2;
				int iCurPos = 0;
				int iDefaultPos = iXPos + 103;

				int iTotalSize = PLUS_SIZE;      
				if( iReinforce < 10 )
					iTotalSize += TEXT_SIZE;
				else if( iReinforce < 100 )
					iTotalSize += TEXT_SIZE * 2;
				else
					iTotalSize += TEXT_SIZE * 3;

				iCurPos = iDefaultPos - (iTotalSize / 2);
				m_pPresentNum->RenderNum( iCurPos + ( PLUS_SIZE - ( TEXT_GAP - 1 ) ), iYPos + 112, iReinforce, -TEXT_GAP );
				m_pPresentPlus->Render( iCurPos, iYPos + 112 );
			}

			if( m_iPresentValue3 > 0 || m_iPresentValue4 > 0 )
			{
				if( m_pPresentCustom )
					m_pPresentCustom->Render( iXPos + 110, iYPos + 55 );
			}
		}
		break;
	case PRESENT_EXTRAITEM_BOX:
		OnRenderExtraItemBox( iXPos, iYPos );
		break;
	case PRESENT_RANDOM_DECO:
		OnRenderPresent( iXPos, iYPos );
		break;
	case PRESENT_GRADE_EXP:
		OnRenderGradeExp( iXPos, iYPos );
		break;
	case PRESENT_COSTUME:
		{
			OnRenderPresent( iXPos, iYPos );
			
			if( m_iPresentValue3 > 0 || m_iPresentValue4 > 0 )
			{
				if( m_pPresentCustom )
					m_pPresentCustom->Render( iXPos + 110, iYPos + 55 );
			}

			// UI코스튬 관련 (코스튬 마크)
			if( m_pCostumeMark )
				m_pCostumeMark->Render( iXPos + 103, iYPos + 98 );
		}
		break;
	case PRESENT_COSTUME_BOX:
		OnRenderCostumeBox( iXPos, iYPos );
		break;
	}

	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( iXPos + 103, iYPos + 162, FONT_SIZE_13, STR(1) );
}

void PresentDeleteWnd::SetGradeType( int nPresentType, int nPresentCode )
{
	m_nGradeType = g_PresentMgr.GetGradeType( nPresentType, nPresentCode );
}

PresentTimeCashAlarmWnd::PresentTimeCashAlarmWnd()
{
	m_pBottomFrm   = NULL;
	m_pStatePinkFrm= NULL;
	m_pPresentBack = NULL;
	m_pPresentIcon = NULL;
	m_pSubIcon = NULL;

	m_bRePosition = true;
	m_pManual = NULL;
	m_nAddCash = 0;
}

PresentTimeCashAlarmWnd::~PresentTimeCashAlarmWnd()
{
	SAFEDELETE( m_pBottomFrm );
	SAFEDELETE( m_pStatePinkFrm );
	SAFEDELETE( m_pPresentBack );
	SAFEDELETE( m_pPresentIcon );
	SAFEDELETE( m_pSubIcon );
}

void PresentTimeCashAlarmWnd::iwm_show()
{
	m_bRePosition = false;
}

void PresentTimeCashAlarmWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_EXIT:
	case ID_LINK:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();		
		}
		break;
	}
}

bool PresentTimeCashAlarmWnd::iwm_esc()
{
	if( !IsShow() ) 
		return false;

	if( ioWnd::iwm_esc() )
		return true;

	return false;
}

void PresentTimeCashAlarmWnd::SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate /* = false  */ )
{
	ioWnd::SetPositionToDefault( iParentWidth, iParentHeight, bChildMove, bCreate );
	if( !bCreate )
	{
		if( IsShow() )
			ReSizePresentUI();
	}
}

void PresentTimeCashAlarmWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "BottomFrm" )
	{
		SAFEDELETE( m_pBottomFrm );
		m_pBottomFrm = pFrame;
	}
	else if( szType == "StatePinkFrm" )
	{
		SAFEDELETE( m_pStatePinkFrm );
		m_pStatePinkFrm = pFrame;
	}
	else
		ioWnd::AddRenderFrame( szType, pFrame );
}

void PresentTimeCashAlarmWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "IconBack" )
	{
		SAFEDELETE( m_pPresentBack );
		m_pPresentBack = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}

void PresentTimeCashAlarmWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	ioMovingWnd::OnProcess( fTimePerSec );
}

void PresentTimeCashAlarmWnd::OnRender()
{
	if( !m_pManual )
		return;

	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_pPresentBack )
		m_pPresentBack->Render( iXPos, iYPos, UI_RENDER_MULTIPLY );
	
	if( m_pPresentIcon )
		m_pPresentIcon->Render( iXPos + GetWidth() / 2, iYPos + 98 );
	if( m_pSubIcon )
		m_pSubIcon->Render( iXPos + GetWidth() / 2, iYPos + 98 );

	if( m_pBottomFrm )
		m_pBottomFrm->Render( iXPos, iYPos + ( GetHeight() - 65 ), UI_RENDER_MULTIPLY );

	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );

	g_FontMgr.SetTextColor( TCT_DEFAULT_GOLD );
	g_FontMgr.PrintText( iXPos + GetWidth() / 2, iYPos + 144, FONT_SIZE_13, "%dG", m_nAddCash );

	PrintManual( iXPos + GetWidth() / 2, iYPos + 189, FONT_SIZE_12 );	
}

void PresentTimeCashAlarmWnd::ReSizePresentUI()
{
	if( !m_pManual )
		return;

	ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( ioEtcItem::EIT_ETC_TIME_CASH );
	if( !pEtcItem ) 
	{
		pEtcItem = g_EtcItemMgr.FindEtcItem( ioEtcItem::EIT_ETC_TIME_CASH2 );

		if(!pEtcItem)
			return;
	}

	int nSubManual = pEtcItem->GetInventorySubManual() - 1;
	int iMentSize = m_pManual->GetMaxLine( nSubManual );
	SetSize( GetWidth(), 283 + ( iMentSize * 18 ) );

	ioWnd *pBtnWnd = FindChildWnd( ID_LINK );
	if( pBtnWnd )
		pBtnWnd->SetWndPos( pBtnWnd->GetXPos(), GetHeight() - 48 );

	if( m_bRePosition )
		SetWndPos( ( Setting::Width()/2 ) - (GetWidth()/2) , (Setting::Height()/2) - (GetHeight()/2)  );
}

void PresentTimeCashAlarmWnd::PrintManual( int nXPos, int nYPos, float fScale )
{
	if( !m_pManual )
		return;

	ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( ioEtcItem::EIT_ETC_TIME_CASH );
	if( !pEtcItem ) 
	{
		pEtcItem = g_EtcItemMgr.FindEtcItem( ioEtcItem::EIT_ETC_TIME_CASH2 );

		if( !pEtcItem )
			return;
	}

	int nSubManual = pEtcItem->GetInventorySubManual() - 1;
	ioComplexStringPrinter kPrinter;
	int nMaxLine = m_pManual->GetMaxLine( nSubManual );
	for( int nLine=1; nLine<nMaxLine; nLine++ )
	{
		m_pManual->SetPrinter( nSubManual, nLine, fScale, kPrinter );
		nYPos += m_pManual->GetSpaceLine( nSubManual, nLine );
		kPrinter.PrintFullTextWidthCut( nXPos, nYPos, TAT_CENTER, DESC_WIDTH );
		kPrinter.ClearList();
	}

	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextColor( TCT_DEFAULT_RED );
	g_FontMgr.PrintText( nXPos, nYPos + 18, FONT_SIZE_13, "[개발자K]" );
}

void PresentTimeCashAlarmWnd::ShowPresentTimeCashAlarmWnd( int nAddCash )
{
	SAFEDELETE( m_pPresentIcon );
	SAFEDELETE( m_pSubIcon );

	ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( ioEtcItem::EIT_ETC_TIME_CASH );

	if( !pEtcItem )
	{
		pEtcItem = g_EtcItemMgr.FindEtcItem( ioEtcItem::EIT_ETC_TIME_CASH2 );

		if( !pEtcItem )
			return;
	}
	
	m_pPresentIcon = g_UIImageSetMgr.CreateImageByFullName( pEtcItem->GetIconName() );
	m_pSubIcon = g_UIImageSetMgr.CreateImageByFullName( pEtcItem->GetSubIconName() );

	m_pManual = ioEtcInvenSubManualMgr::GetSingletonPtr();
	m_nAddCash = nAddCash;

	ReSizePresentUI();
	ShowWnd();
}

//////////////////////////////////////////////////////////////////////////

PresentTimeCashReleaseWnd::PresentTimeCashReleaseWnd()
{
	m_pIconBack	= NULL;
	m_pIcon = NULL;
}

PresentTimeCashReleaseWnd::~PresentTimeCashReleaseWnd()
{
	SAFEDELETE( m_pIconBack );
	SAFEDELETE( m_pIcon );
}

bool PresentTimeCashReleaseWnd::iwm_esc()
{
	if( !IsShow() ) 
		return false;

	if( ioWnd::iwm_esc() )
		return true;

	return false;
}

void PresentTimeCashReleaseWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_X:
	case ID_OK:
		if( cmd == IOBN_BTNUP )
		{
			/*ioUserEtcItem *pUserEtcItem = g_MyInfo.GetUserEtcItem();
			if ( pUserEtcItem )
				pUserEtcItem->DeleteEtcItemZeroTime();*/
			HideWnd();
		}
		break;
	}
}

void PresentTimeCashReleaseWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "IconBack" )
	{
		SAFEDELETE( m_pIconBack );
		m_pIconBack = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}

void PresentTimeCashReleaseWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	ioMovingWnd::OnProcess( fTimePerSec );
}

void PresentTimeCashReleaseWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_pIconBack )
		m_pIconBack->Render( iXPos, iYPos, UI_RENDER_MULTIPLY );
	
	if( m_pIcon )
		m_pIcon->Render( iXPos + 103, iYPos + 98 );

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 103, iYPos + 144, FONT_SIZE_13, "%s", m_szItemName.c_str() );  
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( iXPos + 103, iYPos + 162, FONT_SIZE_13, "기간만료로 해제되었습니다" ); 
}

void PresentTimeCashReleaseWnd::ShowPresentTimeCashReleaseWnd( DWORD dwEtcItemCode )
{
	SAFEDELETE( m_pIcon );
	SAFEDELETE( m_pSubIcon );
	m_szItemName.Clear();

	m_dwEtcItemCode = dwEtcItemCode;
	ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( ioEtcItem::EIT_ETC_TIME_CASH );
	if( !pEtcItem )
	{
		pEtcItem = g_EtcItemMgr.FindEtcItem( ioEtcItem::EIT_ETC_TIME_CASH2 );
		if( !pEtcItem )
			return;
	}

	m_pIcon = g_UIImageSetMgr.CreateImageByFullName( pEtcItem->GetIconName() );
	m_pSubIcon = g_UIImageSetMgr.CreateImageByFullName( pEtcItem->GetSubIconName() );
	m_szItemName = pEtcItem->GetName();

	ShowWnd();
}