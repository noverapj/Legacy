#include "StdAfx.h"
#include "BonusCashWnd.h"

BonusCashWnd::BonusCashWnd(void)
{
	m_dwCurButtonID = 0;
}

BonusCashWnd::~BonusCashWnd(void)
{
}

void BonusCashWnd::iwm_show()
{
	m_dwCurButtonID = ID_PROGRESS_BTN;
	CheckRadioButton( ID_PROGRESS_BTN, ID_EXPRIATION_BTN, m_dwCurButtonID );

	ShowChildWnd( ID_PROGRESS_INFO_WND );
}

void BonusCashWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
		if( cmd == IOBN_BTNUP )
			HideWnd();
		break;
	case ID_PROGRESS_BTN:
		if( cmd == IOBN_BTNUP )
		{
			m_dwCurButtonID = ID_PROGRESS_BTN;
			CheckRadioButton( ID_PROGRESS_BTN, ID_EXPRIATION_BTN, m_dwCurButtonID );
			HideChildWnd( ID_EXPRIATION_INFO_WND );
			ShowChildWnd( ID_PROGRESS_INFO_WND );
		}
		break;
	case ID_EXPRIATION_BTN:
		if( cmd == IOBN_BTNUP )
		{
			m_dwCurButtonID = ID_EXPRIATION_BTN;
			CheckRadioButton( ID_PROGRESS_BTN, ID_EXPRIATION_BTN, m_dwCurButtonID );
			HideChildWnd( ID_PROGRESS_INFO_WND );
			ShowChildWnd( ID_EXPRIATION_INFO_WND );
		}
		break;
	}
}

void BonusCashWnd::OnRender()
{
	ioWnd::OnRender();
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	enum
	{
		TEXT1_X		= 216,
		TEXT1_Y		= 353,
		TEXT2_X		= 216,
		TEXT2_Y		= 373,
	};

	if ( m_dwCurButtonID == ID_PROGRESS_BTN )
	{
		ioComplexStringPrinter kPrinter;
		kPrinter.SetTextStyle( TS_NORMAL );
		kPrinter.SetBkColor( 0, 0, 0 );
		kPrinter.SetTextColor( TCT_DEFAULT_RED );
		kPrinter.AddTextPiece( FONT_SIZE_13, "내 총 보너스 골드 : %d 골드", g_MyInfo.GetBonusCash() );
		kPrinter.PrintFullText( iXPos + TEXT1_X, iYPos + TEXT1_Y, TAT_CENTER );
		kPrinter.ClearList();

		kPrinter.SetTextStyle( TS_NORMAL );
		kPrinter.SetBkColor( 0, 0, 0 );
		kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
		kPrinter.AddTextPiece( FONT_SIZE_13, "보너스 골드는 만료일이 짧을수록 먼저 소진됩니다." );
		kPrinter.PrintFullText( iXPos + TEXT2_X, iYPos + TEXT2_Y, TAT_CENTER );
	}
	else if ( m_dwCurButtonID == ID_EXPRIATION_BTN )
	{
		ioComplexStringPrinter kPrinter;
		kPrinter.SetTextStyle( TS_NORMAL );
		kPrinter.SetBkColor( 0, 0, 0 );
		kPrinter.SetTextColor( TCT_DEFAULT_RED );
		kPrinter.AddTextPiece( FONT_SIZE_13, "내 총 보너스 골드 : %d 골드", g_MyInfo.GetBonusCash() );
		kPrinter.PrintFullText( iXPos + TEXT1_X, iYPos + TEXT1_Y, TAT_CENTER );
		kPrinter.ClearList();

		kPrinter.SetTextStyle( TS_NORMAL );
		kPrinter.SetBkColor( 0, 0, 0 );
		kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
		kPrinter.AddTextPiece( FONT_SIZE_13, "만료내역은 최대 30개까지 저장됩니다." );
		kPrinter.PrintFullText( iXPos + TEXT2_X, iYPos + TEXT2_Y, TAT_CENTER );
	}
	
}

void BonusCashWnd::iwm_hide()
{
	HideChildWnd( ID_PROGRESS_INFO_WND );
	HideChildWnd( ID_EXPRIATION_INFO_WND );
}

////////////////////////////////////////////////////////////////////////////
BonusCashBtn::BonusCashBtn( void )
{
	m_bData = false;
}

BonusCashBtn::~BonusCashBtn( void )
{

}

bool BonusCashBtn::SetInfo( const ioBonusCashManager::BonusCashInfo& rkInfo, bool bProgress )
{
	if ( rkInfo.empty() )
	{
		m_bData = false;
		return false;
	}

	for ( int i=0; i<MAX_TEXT_INFO ; ++i )
		m_CashInfoList[i].ClearList();
	
	m_CashInfoList[0].SetTextStyle( TS_NORMAL );
	m_CashInfoList[0].SetBkColor( 0, 0, 0 );
	if ( bProgress )
		m_CashInfoList[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
	else
		m_CashInfoList[0].SetTextColor( TCT_DEFAULT_GRAY );
	m_CashInfoList[0].AddTextPiece( FONT_SIZE_12, "%d.%d.%d (%02d:%02d까지)", rkInfo.ExpiredTime.GetYear(), rkInfo.ExpiredTime.GetMonth(), rkInfo.ExpiredTime.GetDay(), rkInfo.ExpiredTime.GetHour(), rkInfo.ExpiredTime.GetMinute() );

	m_CashInfoList[1].SetTextStyle( TS_NORMAL );
	m_CashInfoList[1].SetBkColor( 0, 0, 0 );
	if ( bProgress )
		m_CashInfoList[1].SetTextColor( D3DCOLOR_XRGB( 141, 87, 4 ) );
	else
		m_CashInfoList[1].SetTextColor( TCT_DEFAULT_GRAY );
	m_CashInfoList[1].AddTextPiece( FONT_SIZE_12, "%d 골드", rkInfo.iAcquisitionCash );

	m_CashInfoList[2].SetTextStyle( TS_NORMAL );
	m_CashInfoList[2].SetBkColor( 0, 0, 0 );
	if ( bProgress )
		m_CashInfoList[2].SetTextColor( D3DCOLOR_XRGB( 141, 87, 4 ) );
	else
		m_CashInfoList[2].SetTextColor( TCT_DEFAULT_GRAY );
	m_CashInfoList[2].AddTextPiece( FONT_SIZE_12, "%d 골드", rkInfo.iRemainingCash );

	m_bData = true;
	return true;
}

void BonusCashBtn::OnRender()
{
	ioWnd::OnRender();

	if ( !m_bData )
		return;

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	enum
	{
		TEXT1_X		=	6,
		TEXT2_X		=	251,
		TEXT3_X		=	370,
		TEXT_UP_Y	=	4,
	};

	m_CashInfoList[0].PrintFullText( iXPos + TEXT1_X, iYPos + TEXT_UP_Y, TAT_LEFT );
	m_CashInfoList[1].PrintFullText( iXPos + TEXT2_X, iYPos + TEXT_UP_Y, TAT_RIGHT );
	m_CashInfoList[2].PrintFullText( iXPos + TEXT3_X, iYPos + TEXT_UP_Y, TAT_RIGHT );
}

////////////////////////////////////////////////////////////////////////////
BonusCashProgressWnd::BonusCashProgressWnd( void )
{
	m_iCurPos   = 0;
}

BonusCashProgressWnd::~BonusCashProgressWnd( void )
{

}

void BonusCashProgressWnd::iwm_show()
{
	m_iCurPos = 0;
	ioScroll *pScroll = dynamic_cast<ioScroll*>(FindChildWnd( ID_VERT_SCROLL ));
	if( pScroll )
	{
		pScroll->SetScrollRange( 0, g_BonusCashMgr.GetAliveBonusCashInfoSize() );
		pScroll->SetScrollPos( m_iCurPos );
		pScroll->ShowWnd();
	}
}

void BonusCashProgressWnd::OnRender()
{
	ioWnd::OnRender();
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	enum
	{
		TEXT1_X		=	23,
		TEXT2_X		=	267,
		TEXT3_X		=	386,
		TEXT_UP_Y	=	0,

		TEXT4_X		=	204,
		TEXT4_Y		=	105,
	};

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "만료일자" );
	kPrinter.PrintFullText( iXPos + TEXT1_X, iYPos + TEXT_UP_Y, TAT_LEFT );
	kPrinter.ClearList();

	kPrinter.AddTextPiece( FONT_SIZE_12, "획득금액" );
	kPrinter.PrintFullText( iXPos + TEXT2_X, iYPos + TEXT_UP_Y, TAT_RIGHT );
	kPrinter.ClearList();

	kPrinter.AddTextPiece( FONT_SIZE_12, "남은금액" );
	kPrinter.PrintFullText( iXPos + TEXT3_X, iYPos + TEXT_UP_Y, TAT_RIGHT );
	kPrinter.ClearList();

	int iSize = g_BonusCashMgr.GetAliveBonusCashInfoSize();
	if ( iSize <= 0 )
	{
		kPrinter.SetTextStyle( TS_NORMAL );
		kPrinter.SetBkColor( 0, 0, 0 );
		kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter.AddTextPiece( FONT_SIZE_12, "진행중인 보너스 골드 내역이 없습니다. " );
		kPrinter.PrintFullText( iXPos + TEXT4_X, iYPos + TEXT4_Y, TAT_CENTER );
		return;
	}

	for ( int i=0; i<ID_BONUSCASH_INFO10 ; ++i )
	{
		BonusCashBtn *pBtn = dynamic_cast<BonusCashBtn*>(FindChildWnd(i));
		if ( !pBtn )
			continue;

		int iListPos = m_iCurPos + i;
		pBtn->SetInfo( g_BonusCashMgr.GetConstAliveBonusCashInfoByArray(iListPos), true );
	}
}

void BonusCashProgressWnd::iwm_wheel( int zDelta )
{
	if( !IsShow() )
		ioWnd::iwm_wheel( zDelta );
	else
	{
		if( g_App.IsMouseBusy() )
			return;

		ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
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

void BonusCashProgressWnd::iwm_vscroll( DWORD dwID, int curPos )
{
	ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
	if( pScroll == NULL )
		return;
	if( !pScroll->IsShow() )
		return;

	m_iCurPos = curPos;
}

////////////////////////////////////////////////////////////////////////////
BonusCashExpireWnd::BonusCashExpireWnd( void )
{
	m_iCurPos   = 0;
}

BonusCashExpireWnd::~BonusCashExpireWnd( void )
{

}

void BonusCashExpireWnd::iwm_show()
{
	m_iCurPos   = 0;
	ioScroll *pScroll = dynamic_cast<ioScroll*>(FindChildWnd( ID_VERT_SCROLL ));
	if( pScroll )
	{
		pScroll->SetScrollRange( 0, g_BonusCashMgr.GetExpireBonusCashInfoSize() );
		pScroll->SetScrollPos( m_iCurPos );
		pScroll->ShowWnd();
	}
}

void BonusCashExpireWnd::OnRender()
{
	ioWnd::OnRender();
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	enum
	{
		TEXT1_X		=	23,
		TEXT2_X		=	267,
		TEXT3_X		=	386,
		TEXT_UP_Y	=	0,

		TEXT4_X		=	204,
		TEXT4_Y		=	105,
	};

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "만료일자" );
	kPrinter.PrintFullText( iXPos + TEXT1_X, iYPos + TEXT_UP_Y, TAT_LEFT );
	kPrinter.ClearList();

	kPrinter.AddTextPiece( FONT_SIZE_12, "획득금액" );
	kPrinter.PrintFullText( iXPos + TEXT2_X, iYPos + TEXT_UP_Y, TAT_RIGHT );
	kPrinter.ClearList();

	kPrinter.AddTextPiece( FONT_SIZE_12, "남은금액" );
	kPrinter.PrintFullText( iXPos + TEXT3_X, iYPos + TEXT_UP_Y, TAT_RIGHT );
	kPrinter.ClearList();

	int iSize = g_BonusCashMgr.GetExpireBonusCashInfoSize();
	if ( iSize <= 0 )
	{
		kPrinter.SetTextStyle( TS_NORMAL );
		kPrinter.SetBkColor( 0, 0, 0 );
		kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter.AddTextPiece( FONT_SIZE_12, "기간 만료 내역이 없습니다. " );
		kPrinter.PrintFullText( iXPos + TEXT4_X, iYPos + TEXT4_Y, TAT_CENTER );
		return;
	}

	for ( int i=0; i<ID_BONUSCASH_INFO10 ; ++i )
	{
		BonusCashBtn *pBtn = dynamic_cast<BonusCashBtn*>(FindChildWnd(i));
		if ( !pBtn )
			continue;
		int iListPos = m_iCurPos + i;
		pBtn->SetInfo( g_BonusCashMgr.GetConstExpireBonusCashInfoByArray(iListPos), false );
	}
}

void BonusCashExpireWnd::iwm_wheel( int zDelta )
{
	if( !IsShow() )
		ioWnd::iwm_wheel( zDelta );
	else
	{
		if( g_App.IsMouseBusy() )
			return;

		ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
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

void BonusCashExpireWnd::iwm_vscroll( DWORD dwID, int curPos )
{
	ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
	if( pScroll == NULL )
		return;
	if( !pScroll->IsShow() )
		return;

	m_iCurPos = curPos;
}
