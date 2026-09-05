
#include "StdAfx.h"

#include "../ioBingo.h"
#include "BingoMsgBox.h"

BingoMsgBox::BingoMsgBox()
{
	m_UseType = UT_NONE;
	m_pGetNumber = NULL;
	m_pShuffleNumber = NULL;
	m_pShuffleItem = NULL;
	m_pSpecialNumber = NULL;
	m_pInit = NULL;
	m_pIcon = NULL;
	m_pIconBg = NULL;
	m_pBottomFrm = NULL;
}

BingoMsgBox::~BingoMsgBox()
{
	SAFEDELETE( m_pGetNumber );
	SAFEDELETE( m_pShuffleNumber );
	SAFEDELETE( m_pShuffleItem );
	SAFEDELETE( m_pSpecialNumber );
	SAFEDELETE( m_pInit );
	SAFEDELETE( m_pIconBg );
	SAFEDELETE( m_pBottomFrm );
}

void BingoMsgBox::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "GetNumber" )
	{
		SAFEDELETE( m_pGetNumber );
		m_pGetNumber = pImage;
	}
	else if( szType == "ShuffleNumber" )
	{
		SAFEDELETE( m_pShuffleNumber );
		m_pShuffleNumber = pImage;
	}
	else if( szType == "ShuffleItem" )
	{
		SAFEDELETE( m_pShuffleItem );
		m_pShuffleItem = pImage;
	}
	else if( szType == "SpecialNumber" )
	{
		SAFEDELETE( m_pSpecialNumber );
		m_pSpecialNumber = pImage;
	}
	else if( szType == "Init" )
	{
		SAFEDELETE( m_pInit );
		m_pInit = pImage;
	}
	else if( szType == "IconBg" )
	{
		SAFEDELETE( m_pIconBg );
		m_pIconBg = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}

void BingoMsgBox::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "BottomFrm" )
	{
		SAFEDELETE( m_pBottomFrm );
		m_pBottomFrm = pFrame;
	}
	else
		ioWnd::AddRenderFrame( szType, pFrame );
}

void BingoMsgBox::iwm_show()
{
	HideChildWnd( ID_OK );
	HideChildWnd( ID_CANCEL );
	HideChildWnd( ID_INIT );
	HideChildWnd( ID_CLOSE );
	m_UseType = UT_NONE;
	m_pIcon = NULL;
}

void BingoMsgBox::iwm_hide()
{
	HideChildWnd( ID_OK );
	HideChildWnd( ID_CANCEL );
	HideChildWnd( ID_INIT );
	HideChildWnd( ID_CLOSE );
	m_pIcon = NULL;
}

void BingoMsgBox::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
	case ID_CANCEL:
	case ID_CLOSE:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;
	case ID_OK:
	case ID_INIT:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
			if( GetParent() )
			{
				GetParent()->iwm_command( this, cmd, static_cast<DWORD>(m_UseType) );
			}
		}
		break;
	}
}

bool BingoMsgBox::iwm_spacebar()
{
	ioButton *pOKBtn = (ioButton*)FindChildWnd( ID_OK );
	ioButton *pInitBtn = (ioButton*)FindChildWnd( ID_INIT );
	if( pOKBtn && pOKBtn->IsShow() )
	{
		iwm_command( pOKBtn, IOBN_BTNUP, 0 );
		return true;
	}
	else if( pInitBtn && pInitBtn->IsShow() )
	{
		iwm_command( pInitBtn, IOBN_BTNUP, 0 );
		return true;
	}
	
	return false;
}

//void BingoMsgBox::OnProcess( float fTimePerSec )
//{
//	if( !IsShow() )
//		return;
//}

void BingoMsgBox::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	enum { ICON_X = 151, ICON_Y = 98, BOTTOM_FRM_HEIGHT = 65, };

	if( m_pIconBg )
		m_pIconBg->Render( iXPos + ICON_X, iYPos + ICON_Y, UI_RENDER_MULTIPLY );

	if( m_pIcon )
		m_pIcon->Render( iXPos + ICON_X, iYPos + ICON_Y );

	if( m_pBottomFrm )
		m_pBottomFrm->Render( GetDerivedPosX(), GetDerivedPosY() + GetHeight() - BOTTOM_FRM_HEIGHT , UI_RENDER_MULTIPLY );

	PrintText( iXPos, iYPos );
}

void BingoMsgBox::PrintText( int iXPos, int iYPos )
{
	if( m_UseType == UT_INIT )
	{
		ioComplexStringPrinter kPrinter;
		kPrinter.SetTextStyle( TS_NORMAL );
		kPrinter.SetBkColor( 0, 0, 0 );
		kPrinter.SetTextColor( TCT_DEFAULT_RED );

		switch( m_iBingoType )
		{
		case ioBingo::BT_RAND:
			{
				kPrinter.AddTextPiece( FONT_SIZE_13, "모든 숫자가 초기화되며 바뀝니다." );
				kPrinter.PrintFullText( iXPos + 151, iYPos + 144, TAT_CENTER );
				kPrinter.ClearList();

				kPrinter.AddTextPiece( FONT_SIZE_13, "모든 보상도 초기화되지만, 바뀌진 않습니다." );
				kPrinter.PrintFullText( iXPos + 151, iYPos + 164, TAT_CENTER );
				kPrinter.ClearList();
			}
			break;
		case ioBingo::BT_FIXED:
			{
				kPrinter.AddTextPiece( FONT_SIZE_13, "모든 숫자와 보상이 초기화됩니다." );
				kPrinter.PrintFullText( iXPos + 151, iYPos + 144, TAT_CENTER );
				kPrinter.ClearList();

				kPrinter.AddTextPiece( FONT_SIZE_13, "초기화 이후 다시 진행 가능합니다." );
				kPrinter.PrintFullText( iXPos + 151, iYPos + 164, TAT_CENTER );
				kPrinter.ClearList();
			}
		}

		UpdateWndSize( WINDOW_WIDTH, WINDOW_HEIGHT - 20 );
	}
	else
	{
		if( m_iItemCount > 0 )
		{
			ioComplexStringPrinter kPrinter;
			kPrinter.SetTextStyle( TS_NORMAL );
			kPrinter.SetBkColor( 0, 0, 0 );

			kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
			kPrinter.AddTextPiece( FONT_SIZE_13, "보유 중인 [%s] 아이템을", m_szItemName );
			kPrinter.PrintFullText( iXPos + 151, iYPos + 144, TAT_CENTER );
			kPrinter.ClearList();

			kPrinter.AddTextPiece( FONT_SIZE_13, "사용하시겠습니까?" );
			kPrinter.PrintFullText( iXPos + 151, iYPos + 164, TAT_CENTER );
			kPrinter.ClearList();

			kPrinter.SetTextColor( TCT_DEFAULT_GREEN );
			kPrinter.AddTextPiece( FONT_SIZE_13, "( %d개 보유 )", m_iItemCount );
			kPrinter.PrintFullText( iXPos + 151, iYPos + 184, TAT_CENTER );
			kPrinter.ClearList();

			UpdateWndSize( WINDOW_WIDTH, WINDOW_HEIGHT );
		}
		else
		{
			ioComplexStringPrinter kPrinter;
			kPrinter.SetTextStyle( TS_NORMAL );
			kPrinter.SetBkColor( 0, 0, 0 );

			kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
			kPrinter.AddTextPiece( FONT_SIZE_13, "[%s] 아이템이 없습니다.", m_szItemName );
			kPrinter.PrintFullText( iXPos + 151, iYPos + 144, TAT_CENTER );
			kPrinter.ClearList();

			int iYGap = 164;
			ioBingo *pBingo = g_MyInfo.GetBingo();
			if( pBingo )
			{
				ioComplexStringPrinter kPrinterManual;
				for( int iLine=0; iLine<2; ++iLine )
				{
					pBingo->SetUIPrinter( m_UseType + 2, iLine, FONT_SIZE_13, kPrinterManual );
					kPrinterManual.PrintFullText( iXPos + 151, iYPos + iYGap, TAT_CENTER );
					kPrinterManual.ClearList();
					iYGap += 20;
				}
			}

			UpdateWndSize( WINDOW_WIDTH, WINDOW_HEIGHT );
		}
	}
}

void BingoMsgBox::UpdateWndSize( int iWidth, int iHeight )
{
	int iHeightGap = BUTTON_Y;
	if( iHeight != WINDOW_HEIGHT )
		iHeightGap += ( iHeight - WINDOW_HEIGHT );

	SetSize( iWidth, iHeight );
	SetScreenWndPos( (Setting::Width() - GetWidth() ) / 2, ( (Setting::Height() - GetHeight() ) / 2 ) - 20 );

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	ioWnd *pWnd = FindChildWnd(ID_OK);
	if( pWnd )
	{
		pWnd->SetScreenWndPos( iXPos + 17, iYPos + iHeightGap );
	}
	pWnd = FindChildWnd(ID_CANCEL);
	if( pWnd )
	{
		pWnd->SetScreenWndPos( iXPos + 152 , iYPos + iHeightGap );
	}
	pWnd = FindChildWnd(ID_INIT);
	if( pWnd )
	{
		pWnd->SetScreenWndPos( iXPos + 17, iYPos + iHeightGap );
	}
	pWnd = FindChildWnd(ID_CLOSE);
	if( pWnd )
	{
		pWnd->SetScreenWndPos( iXPos + 17, iYPos + iHeightGap );
	}
}

void BingoMsgBox::SetInfo( UseType eType, int iBingoType )
{
	if( !IsShow() )
		ShowWnd();

	m_iBingoType = iBingoType;
	m_UseType = eType;
	switch( m_UseType )
	{
	case UT_GET_NUMBER:
		{
			m_iItemCount = g_MyInfo.GetEtcCoinMoney( ioEtcItem::EIT_ETC_BINGO_NUMBER_GASHAPON );
			if( m_iItemCount <= 0 )
				ShowChildWnd( ID_CLOSE );
			else
			{
				ShowChildWnd( ID_OK );
				ShowChildWnd( ID_CANCEL );
				HideChildWnd( ID_INIT );
			}

			m_szItemName.Clear();
			m_pIcon = m_pGetNumber;

			ioEtcItem *pItem = g_EtcItemMgr.FindEtcItem( ioEtcItem::EIT_ETC_BINGO_NUMBER_GASHAPON );
			if( pItem )
			{
				m_szItemName = pItem->GetName();
				SetTitleText( m_szItemName.c_str() );
			}
		}
		break;
	case UT_SHUFFLE_NUMBER:
		{
			m_iItemCount = g_MyInfo.GetEtcCoinMoney( ioEtcItem::EIT_ETC_BINGO_SHUFFLE_NUMBER );
			if( m_iItemCount <= 0 )
				ShowChildWnd( ID_CLOSE );
			else
			{
				ShowChildWnd( ID_OK );
				ShowChildWnd( ID_CANCEL );
				HideChildWnd( ID_INIT );
			}

			m_szItemName.Clear();
			m_pIcon = m_pShuffleNumber;

			ioEtcItem *pItem = g_EtcItemMgr.FindEtcItem( ioEtcItem::EIT_ETC_BINGO_SHUFFLE_NUMBER );
			if( pItem )
			{
				m_szItemName = pItem->GetName();
				SetTitleText( m_szItemName.c_str() );
			}
		}
		break;
	case UT_SHUFFLE_REWARD:
		{
			m_iItemCount = g_MyInfo.GetEtcCoinMoney( ioEtcItem::EIT_ETC_BINGO_SHUFFLE_REWARD_ITEM );
			if( m_iItemCount <= 0 )
				ShowChildWnd( ID_CLOSE );
			else
			{
				ShowChildWnd( ID_OK );
				ShowChildWnd( ID_CANCEL );
				HideChildWnd( ID_INIT );
			}

			m_szItemName.Clear();
			m_pIcon = m_pShuffleItem;

			ioEtcItem *pItem = g_EtcItemMgr.FindEtcItem( ioEtcItem::EIT_ETC_BINGO_SHUFFLE_REWARD_ITEM );
			if( pItem )
			{
				m_szItemName = pItem->GetName();
				SetTitleText( m_szItemName.c_str() );
			}
		}
		break;
	case UT_SPECIAL_NUMBER:
		{
			m_iItemCount = g_MyInfo.GetEtcCoinMoney( ioEtcItem::EIT_ETC_BINGO_RANDOM_NUMBER_CLEAR );
			if( m_iItemCount <= 0 )
				ShowChildWnd( ID_CLOSE );
			else
			{
				ShowChildWnd( ID_OK );
				ShowChildWnd( ID_CANCEL );
				HideChildWnd( ID_INIT );
			}

			m_szItemName.Clear();
			m_pIcon = m_pSpecialNumber;

			ioEtcItem *pItem = g_EtcItemMgr.FindEtcItem( ioEtcItem::EIT_ETC_BINGO_RANDOM_NUMBER_CLEAR );
			if( pItem )
			{
				m_szItemName = pItem->GetName();
				SetTitleText( m_szItemName.c_str() );
			}
		}
		break;
	case UT_INIT:
		{
			HideChildWnd( ID_OK );
			HideChildWnd( ID_CLOSE );
			ShowChildWnd( ID_CANCEL );
			ShowChildWnd( ID_INIT );

			m_szItemName.Clear();
			m_pIcon = m_pInit;

			SetTitleText( "초기화" );
		}
		break;
	}

	SetScreenWndPos( (Setting::Width() - GetWidth() ) / 2, ( (Setting::Height() - GetHeight() ) / 2 ) - 20 );
}