
#include "StdAfx.h"

#include "BingoRightWnd.h"
#include "BingoWnd.h"
#include "ioBingoRewardWnd.h"
#include "BingoMsgBox.h"
#include "BingoHelpToolTip.h"
#include "../ioBingo.h"

BingoRightWnd::BingoRightWnd()
{
	m_WindowType = RWT_NONE;
	
	m_pUnderLine    = NULL;
	m_pTextBingo    = NULL;
	m_pTextAll      = NULL;
	m_pTextClear	= NULL;
	m_pTextNumber   = NULL;
	m_pTextCount    = NULL;
	m_pTextSelect   = NULL;
	m_pTextGet      = NULL;
	m_pRedBall      = NULL;
	m_pSubIcon = NULL;
	m_pLightBall    = NULL;
	m_pBallBg       = NULL;

	for( int i=0; i<10; ++i )
		m_pBallEffect[i] = NULL;

	m_dwCheckTime = 0;
	m_bLightIcon = false;

	m_BallAniState = BAS_NONE;
	m_dwBallAniCheckTime = 0;

	m_iLastGetNumber = 0;
	m_iBingoType	 = 0;

	m_vBallAfterImageList.clear();
}

BingoRightWnd::~BingoRightWnd()
{
	SAFEDELETE( m_pUnderLine );
	SAFEDELETE( m_pTextBingo );
	SAFEDELETE( m_pTextAll );
	SAFEDELETE( m_pTextClear );
	SAFEDELETE( m_pTextNumber );
	SAFEDELETE( m_pTextCount );
	SAFEDELETE( m_pTextSelect );
	SAFEDELETE( m_pTextGet );
	SAFEDELETE( m_pRedBall );
	SAFEDELETE( m_pSubIcon );
	SAFEDELETE( m_pLightBall );
	SAFEDELETE( m_pBallBg );
	for( int i=0; i<10; ++i )
		SAFEDELETE( m_pBallEffect[i] );
}

void BingoRightWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "TextUnderLine" )
	{
		SAFEDELETE( m_pUnderLine );
		m_pUnderLine = pImage;
	}
	else if( szType == "TextBingo" )
	{
		SAFEDELETE( m_pTextBingo );
		m_pTextBingo = pImage;
	}
	else if( szType == "TextAllBingo" )
	{
		SAFEDELETE( m_pTextAll );
		m_pTextAll = pImage;
	}
	else if( szType == "TextClear" )
	{
		SAFEDELETE( m_pTextClear );
		m_pTextClear = pImage;
	}
	else if( szType == "TextNumber" )
	{
		SAFEDELETE( m_pTextNumber );
		m_pTextNumber = pImage;
	}
	else if( szType == "TextCount" )
	{
		SAFEDELETE( m_pTextCount );
		m_pTextCount = pImage;
	}
	else if( szType == "TextSelect" )
	{
		SAFEDELETE( m_pTextSelect );
		m_pTextSelect = pImage;
	}
	else if( szType == "TextGet" )
	{
		SAFEDELETE( m_pTextGet );
		m_pTextGet = pImage;
	}
	else if( szType == "RedBall" )
	{
		SAFEDELETE( m_pRedBall );
		m_pRedBall = pImage;
	}
	else if( szType == "LightBall" )
	{
		SAFEDELETE( m_pLightBall );
		m_pLightBall = pImage;
	}
	else if( szType == "BallBg" )
	{
		SAFEDELETE( m_pBallBg );
		m_pBallBg = pImage;
	}
	else if( szType == "BallEffect1" )
	{
		SAFEDELETE( m_pBallEffect[0] );
		m_pBallEffect[0] = pImage;
	}
	else if( szType == "BallEffect2" )
	{
		SAFEDELETE( m_pBallEffect[1] );
		m_pBallEffect[1] = pImage;
	}
	else if( szType == "BallEffect3" )
	{
		SAFEDELETE( m_pBallEffect[2] );
		m_pBallEffect[2] = pImage;
	}
	else if( szType == "BallEffect4" )
	{
		SAFEDELETE( m_pBallEffect[3] );
		m_pBallEffect[3] = pImage;
	}
	else if( szType == "BallEffect5" )
	{
		SAFEDELETE( m_pBallEffect[4] );
		m_pBallEffect[4] = pImage;
	}
	else if( szType == "BallEffect6" )
	{
		SAFEDELETE( m_pBallEffect[5] );
		m_pBallEffect[5] = pImage;
	}
	else if( szType == "BallEffect7" )
	{
		SAFEDELETE( m_pBallEffect[6] );
		m_pBallEffect[6] = pImage;
	}
	else if( szType == "BallEffect8" )
	{
		SAFEDELETE( m_pBallEffect[7] );
		m_pBallEffect[7] = pImage;
	}
	else if( szType == "BallEffect9" )
	{
		SAFEDELETE( m_pBallEffect[8] );
		m_pBallEffect[8] = pImage;
	}
	else if( szType == "BallEffect10" )
	{
		SAFEDELETE( m_pBallEffect[9] );
		m_pBallEffect[9] = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}

void BingoRightWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_szBallAniSound1 = xElement.GetStringAttribute_e( "BallAniSound1" );
	m_szBallAniSound2 = xElement.GetStringAttribute_e( "BallAniSound2" );
	m_szBallAniSound3 = xElement.GetStringAttribute_e( "BallAniSound3" );
	m_szAllBingoSound = xElement.GetStringAttribute_e( "AllBingoSound" );
}

void BingoRightWnd::iwm_create()
{
	g_GUIMgr.AddWnd( "XML/bingo/bingomsgbox.xml", this );
}

void BingoRightWnd::iwm_show()
{
	m_iLastGetNumber = 0;
	ioBingo* pBingo = g_MyInfo.GetBingo();	
	if( pBingo )
	{
		m_iBingoType = pBingo->GetBingoType();
	}

	ioUIRenderImageList vImageList;
	vImageList.clear();
	for( int i=0; i<10; ++i )
		vImageList.push_back( m_pBallEffect[i] );

	m_BallEffect.Init();
	m_BallEffect.DestroySpriteList();
	m_BallEffect.AddSpriteList( vImageList );
	m_BallEffect.SetType( ioSprite::AT_PLAY_ONCE );
	m_BallEffect.SetAnimationGapTime( 100 );

	m_ShuffleNumberItemName.Clear();
	m_ShuffleRewardItemName.Clear();
	m_SpecialNumberItemName.Clear();
	ioEtcItem *pItem = NULL;
	pItem = g_EtcItemMgr.FindEtcItem( ioEtcItem::EIT_ETC_BINGO_SHUFFLE_NUMBER );
	if( pItem )
		m_ShuffleNumberItemName = pItem->GetName();
	pItem = g_EtcItemMgr.FindEtcItem( ioEtcItem::EIT_ETC_BINGO_SHUFFLE_REWARD_ITEM );
	if( pItem )
		m_ShuffleRewardItemName = pItem->GetName();
	pItem = g_EtcItemMgr.FindEtcItem( ioEtcItem::EIT_ETC_BINGO_RANDOM_NUMBER_CLEAR );
	if( pItem )
		m_SpecialNumberItemName = pItem->GetName();
}

void BingoRightWnd::iwm_hide()
{
	m_WindowType   = RWT_NONE;
	m_BallAniState = BAS_NONE;
	m_BallEffect.Init();
	m_vBallAfterImageList.clear();
}

void BingoRightWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	int iBingoType = 0;
	ioBingo *pBingo = g_MyInfo.GetBingo();
	if( pBingo )
		iBingoType = pBingo->GetBingoType();

	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_BINGO_START_BTN:
		if( cmd == IOBN_BTNUP )
		{
			SP2Packet kPacket( CTPK_BINGO_START );
			TCPNetwork::SendToServer( kPacket );

			if( m_iBingoType == ioBingo::BT_FIXED )
			{
				SetWindowType( BingoRightWnd::RWT_FIXED_PLAY );
			}
			else
			{
				SetWindowType( BingoRightWnd::RWT_RAND_PLAY );
			}
			SetInActvieChildBtn();
		}
		break;
	case ID_GET_NUMBER_BTN:
		if( cmd == IOBN_BTNUP )
		{
			if( g_MyInfo.GetEtcCoinMoney( ioEtcItem::EIT_ETC_BINGO_NUMBER_GASHAPON ) > 0 )
			{
				SP2Packet kPacket( CTPK_ETCITEM_USE );
				kPacket << ioEtcItem::EIT_ETC_BINGO_NUMBER_GASHAPON;
				TCPNetwork::SendToServer( kPacket );
				TCPNetwork::MouseBusy( true );
			}
			else
			{
				BingoMsgBox *pMsg = dynamic_cast<BingoMsgBox*>( FindChildWnd(ID_MESSAGE_BOX) );
				if( pMsg )
					pMsg->SetInfo( BingoMsgBox::UT_GET_NUMBER, iBingoType );
			}
		}
		break;
	case ID_SHUFFLE_NUMBER_BTN:
		if( cmd == IOBN_BTNUP )
		{
			BingoMsgBox *pMsg = dynamic_cast<BingoMsgBox*>( FindChildWnd(ID_MESSAGE_BOX) );
			if( pMsg )
				pMsg->SetInfo( BingoMsgBox::UT_SHUFFLE_NUMBER, iBingoType );
		}
		break;
	case ID_SHUFFLE_REWARD_BTN:
		if( cmd == IOBN_BTNUP )
		{
			BingoMsgBox *pMsg = dynamic_cast<BingoMsgBox*>( FindChildWnd(ID_MESSAGE_BOX) );
			if( pMsg )
				pMsg->SetInfo( BingoMsgBox::UT_SHUFFLE_REWARD, iBingoType );
		}
		break;
	case ID_GET_SPECIAL_NUMBER_BTN:
		if( cmd == IOBN_BTNUP )
		{
			BingoMsgBox *pMsg = dynamic_cast<BingoMsgBox*>( FindChildWnd(ID_MESSAGE_BOX) );
			if( pMsg )
				pMsg->SetInfo( BingoMsgBox::UT_SPECIAL_NUMBER, iBingoType );
		}
		break;
	case ID_INIT_BTN:
		if( cmd == IOBN_BTNUP )
		{
			BingoMsgBox *pMsg = dynamic_cast<BingoMsgBox*>( FindChildWnd(ID_MESSAGE_BOX) );
			if( pMsg )
				pMsg->SetInfo( BingoMsgBox::UT_INIT, iBingoType );
		}
		break;
	case ID_REWARD_LIST_BTN:
		if( cmd == IOBN_BTNUP )
		{
			ioBingoRewardWnd* pWnd = dynamic_cast<ioBingoRewardWnd*>( g_GUIMgr.FindWnd( BINGO_REWARD_WND ) );
			if( pWnd )
				pWnd->ShowWnd();
		}
		break;
	case ID_NEW_BINGO_BTN:
		if( cmd == IOBN_BTNUP )
		{
			SP2Packet kPacket( CTPK_BINGO_ALL_INIT );
			TCPNetwork::SendToServer( kPacket );

			if( m_iBingoType == ioBingo::BT_FIXED )
			{
				SetWindowType( BingoRightWnd::RWT_FIXED_PLAY );
			}
			else
			{
				SetWindowType( BingoRightWnd::RWT_RAND_PLAY );
			}
			SetInActvieChildBtn();
		}
	case ID_MESSAGE_BOX:
		if( cmd == IOBN_BTNUP )
		{
			switch( param )
			{
			case BingoMsgBox::UT_GET_NUMBER:
				{
					SP2Packet kPacket( CTPK_ETCITEM_USE );
					kPacket << ioEtcItem::EIT_ETC_BINGO_NUMBER_GASHAPON;
					TCPNetwork::SendToServer( kPacket );
					TCPNetwork::MouseBusy( true );
				}
				break;
			case BingoMsgBox::UT_SHUFFLE_NUMBER:
				{
					SP2Packet kPacket( CTPK_ETCITEM_USE );
					kPacket << ioEtcItem::EIT_ETC_BINGO_SHUFFLE_NUMBER;
					TCPNetwork::SendToServer( kPacket );

					SetInActvieChildBtn();
				}
				break;
			case BingoMsgBox::UT_SHUFFLE_REWARD:
				{
					SP2Packet kPacket( CTPK_ETCITEM_USE );
					kPacket << ioEtcItem::EIT_ETC_BINGO_SHUFFLE_REWARD_ITEM;
					TCPNetwork::SendToServer( kPacket );

					SetInActvieChildBtn();
				}
				break;
			case BingoMsgBox::UT_SPECIAL_NUMBER:
				{
					SP2Packet kPacket( CTPK_ETCITEM_USE );
					kPacket << ioEtcItem::EIT_ETC_BINGO_RANDOM_NUMBER_CLEAR;
					TCPNetwork::SendToServer( kPacket );
					TCPNetwork::MouseBusy( true );
				}
				break;
			case BingoMsgBox::UT_INIT:
				{
					SP2Packet kPacket( CTPK_BINGO_NUMBER_INIT );
					TCPNetwork::SendToServer( kPacket );

					SetInActvieChildBtn();
				}
				break;
			}
		}
		break;
	case ID_CLOSE_BTN:
		if( cmd == IOBN_BTNUP )
		{
			BingoWnd *pWnd = dynamic_cast<BingoWnd*>( GetParent() );
			if( pWnd )
				pWnd->HideWnd();
		}
	}
}

void BingoRightWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	int iGetNumberItemCnt     = g_MyInfo.GetEtcCoinMoney( ioEtcItem::EIT_ETC_BINGO_NUMBER_GASHAPON );
	int iShuffleNumberItemCnt = g_MyInfo.GetEtcCoinMoney( ioEtcItem::EIT_ETC_BINGO_SHUFFLE_NUMBER );
	int iShuffleRewardItemCnt = g_MyInfo.GetEtcCoinMoney( ioEtcItem::EIT_ETC_BINGO_SHUFFLE_REWARD_ITEM );
	int iRandomNumberItemCnt  = g_MyInfo.GetEtcCoinMoney( ioEtcItem::EIT_ETC_BINGO_RANDOM_NUMBER_CLEAR );

	char szBuf[MAX_PATH]="";
	ioWnd *pWnd = NULL;
	pWnd = FindChildWnd( ID_SHUFFLE_NUMBER_BTN );
	if( pWnd )
	{
		wsprintf_e( szBuf, "%s ¡¿ %d", m_ShuffleNumberItemName.c_str(), iShuffleNumberItemCnt );
		pWnd->SetTitleText( szBuf );
	}
	pWnd = FindChildWnd( ID_SHUFFLE_REWARD_BTN );
	if( pWnd )
	{
		ioHashString szItemName;
		ioEtcItem *pItem = g_EtcItemMgr.FindEtcItem( ioEtcItem::EIT_ETC_BINGO_SHUFFLE_NUMBER );
		if( pItem )
			szItemName = pItem->GetName();
		wsprintf_e( szBuf, "%s ¡¿ %d", m_ShuffleRewardItemName.c_str(), iShuffleRewardItemCnt );
		pWnd->SetTitleText( szBuf );
	}
	pWnd = FindChildWnd( ID_GET_SPECIAL_NUMBER_BTN );
	if( pWnd )
	{
		ioHashString szItemName;
		ioEtcItem *pItem = g_EtcItemMgr.FindEtcItem( ioEtcItem::EIT_ETC_BINGO_SHUFFLE_NUMBER );
		if( pItem )
			szItemName = pItem->GetName();
		wsprintf_e( szBuf, "%s ¡¿ %d", m_SpecialNumberItemName.c_str(), iRandomNumberItemCnt );
		pWnd->SetTitleText( szBuf );
	}

	ProcessBallAni();

	if( m_dwCheckTime < FRAMEGETTIME() )
	{
		m_bLightIcon ^= true;
		m_dwCheckTime = FRAMEGETTIME() + 500;
	}
}

void BingoRightWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	switch( m_WindowType )
	{
	case RWT_START:
		RenderStart( iXPos, iYPos );
		break;
	case RWT_RAND_PLAY:
		RenderRandPlay( iXPos, iYPos );
		break;
	case RWT_FIXED_PLAY:
		RenderFixedPlay( iXPos, iYPos );
		break;
	case RWT_ALL_BINGO:
		{
			ioBingo *pBingo = g_MyInfo.GetBingo();
			switch( pBingo->GetBingoType() )
			{
			case ioBingo::BT_RAND:
				RenderAllBingo( iXPos, iYPos, m_pTextBingo );
				break;
			case ioBingo::BT_FIXED:
				RenderAllBingo( iXPos, iYPos, m_pTextClear );
				break;
			}
		}
		break;
	}
}

void BingoRightWnd::OnRenderAfterChild()
{
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	RenderBallAni( iXPos, iYPos );
}

void BingoRightWnd::RenderStart( int x, int y )
{
	if( m_pUnderLine )
	{
		m_pUnderLine->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
		m_pUnderLine->SetSize( 187, 2 );
		for( int i=0; i<5; ++i )
		{
			m_pUnderLine->Render( x + 17, y + 271 + i * 20, UI_RENDER_MULTIPLY );
		}
	}

	if( m_pTextBingo )
		m_pTextBingo->Render( x + 110, y + 197 );

	int iXPos = x + 22;
	int iYPos = y + 255;

	ioBingo *pBingo = g_MyInfo.GetBingo();
	if( pBingo )
	{
		ioComplexStringPrinter kPrinter;
		for( int iLine=0; iLine<5; ++iLine )
		{
			pBingo->SetUIPrinter( ioBingo::UI_MANUAL_START_BINGO, iLine, FONT_SIZE_12, kPrinter );
			kPrinter.PrintFullText( iXPos, iYPos, TAT_LEFT );
			kPrinter.ClearList();
			iYPos += 20;
		}
	}
}

void BingoRightWnd::RenderRandPlay( int x, int y )
{
	enum { GAP_X = 6 };

	m_pUnderLine->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
	m_pUnderLine->SetSize( 3, 2 );
	for( int i=0; i<37; ++i )
	{
		m_pUnderLine->Render( x + 19 + i * 5, y + 263, UI_RENDER_MULTIPLY );
		m_pUnderLine->Render( x + 19 + i * 5, y + 372, UI_RENDER_MULTIPLY );
	}

	int iCount = g_MyInfo.GetEtcCoinMoney( ioEtcItem::EIT_ETC_BINGO_NUMBER_GASHAPON );
	iCount = min( iCount, 99 );

	int iNumWidth = m_pTextNumber->GetWidth()/10;
	int iTextWidth = m_pTextCount->GetWidth();
	int iTotalWidth;

	if( iCount >= 10 )
		iTotalWidth = iTextWidth + iNumWidth * 2 - GAP_X * 2;
	else
		iTotalWidth = iTextWidth + iNumWidth - GAP_X;

	int iXOffset = iTotalWidth / 2;
	int iCurXPos = x + 110 - iXOffset;
	if( iCount >= 10 )
	{
		m_pTextNumber->RenderNum( iCurXPos, y + 162, iCount / 10 );
		iCurXPos += ( iNumWidth - GAP_X );
	}

	m_pTextNumber->RenderNum( iCurXPos, y + 162, iCount % 10 );
	iCurXPos += ( iNumWidth - GAP_X );

	m_pTextCount->Render( iCurXPos, y + 161 );
}

void BingoRightWnd::RenderFixedPlay( int x, int y )
{
	if( m_pUnderLine )
	{
		m_pUnderLine->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
		m_pUnderLine->SetSize( 187, 2 );
		for( int i=0; i<5; ++i )
		{
			m_pUnderLine->Render( x + 17, y + 286 + i * 20, UI_RENDER_MULTIPLY );
		}
	}

	int iXPos = x + 22;
	int iYPos = y + 270;

	int iCount = 0;
	ioBingo *pBingo = g_MyInfo.GetBingo();
	if( pBingo )
	{
		ioComplexStringPrinter kPrinter;
		for( int iLine=0; iLine<5; ++iLine )
		{
			pBingo->SetUIPrinter( ioBingo::UI_MANUAL_FIXED_BINGO, iLine, FONT_SIZE_12, kPrinter );
			kPrinter.PrintFullText( iXPos, iYPos, TAT_LEFT );
			kPrinter.ClearList();
			iYPos += 20;
		}

		iCount = pBingo->GetBingoDummyNumberOfCurrent();
	}

	if( !m_pTextNumber || !m_pTextCount || !m_pTextSelect )
		return;
		
	iXPos = x + GetWidth()/2;
	float fScale    = m_pTextNumber->GetScale().x;
	int iNumCnt     = Help::GetNumberLength( iCount );
	int iNumWidth   = m_pTextNumber->GetWidth()/10 * fScale;
	int iTextWidth  = m_pTextCount->GetWidth();
	iXPos -= (iTextWidth + iNumWidth * iNumCnt)/2;
	
	m_pTextNumber->RenderNum( iXPos, y + 179, iCount, -5.0f, fScale, UI_RENDER_NORMAL, TFO_BILINEAR );
	int iOffset = iNumWidth * iNumCnt - iNumCnt * 5.0f + 3.0f;

	m_pTextCount->Render( iXPos + iOffset, y + 179 );
	m_pTextSelect->Render( x + 106, y + 217  );
}

void BingoRightWnd::RenderAllBingo( int x, int y, ioUIRenderImage* pTextImage )
{
	if( m_pTextAll && pTextImage )
	{
		m_pTextAll->Render( x + 110, y + 202 );
		pTextImage->Render( x + 110, y + 197 );
		if( m_bLightIcon )
		{
			m_pTextAll->Render( x + 110, y + 202, UI_RENDER_SCREEN );
			pTextImage->Render( x + 110, y + 197, UI_RENDER_SCREEN );
		}
	}

	if( m_pUnderLine )
	{
		m_pUnderLine->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
		m_pUnderLine->SetSize( 187, 2 );
		for( int i=0; i<5; ++i )
		{
			m_pUnderLine->Render( x + 17, y + 271 + i * 20, UI_RENDER_MULTIPLY );
		}
	}

	int iXPos = x + 22;
	int iYPos = y + 255; 
	ioBingo *pBingo = g_MyInfo.GetBingo();
	if( pBingo )
	{
		ioComplexStringPrinter kPrinter;
		for( int iLine=0; iLine<5; ++iLine )
		{
			pBingo->SetUIPrinter( ioBingo::UI_MANUAL_ALL_BINGO, iLine, FONT_SIZE_12, kPrinter );
			kPrinter.PrintFullText( iXPos, iYPos, TAT_LEFT );
			kPrinter.ClearList();
			iYPos+=20;
		}
	}
}

void BingoRightWnd::ProcessBallAni()
{
	if( m_BallAniState == BAS_NONE )
		return;

	if( m_dwBallAniCheckTime == 0 )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_BallAniState )
	{
	case BAS_POP:
		{
			if( m_dwBallAniCheckTime < dwCurTime )
			{
				m_dwBallAniCheckTime = dwCurTime + 1000;
				m_BallAniState = BAS_SHINING;
				m_BallEffect.SetPlay();
				g_SoundMgr.PlaySound( m_szBallAniSound1 );
			}
		}
		break;
	case BAS_SHINING:
		{
			if( m_dwBallAniCheckTime < dwCurTime )
			{
				m_dwBallAniCheckTime = dwCurTime + 500;
				m_BallAniState = BAS_WAIT;
			}
		}
		break;
	case BAS_WAIT:
		if( m_dwBallAniCheckTime < dwCurTime )
		{
			m_dwBallAniCheckTime = dwCurTime + 380;
			m_BallAniState = BAS_MOVING;
			g_SoundMgr.PlaySound( m_szBallAniSound2 );
		}
		break;
	case BAS_MOVING:
		{
			D3DXVECTOR2 vPos[6];
			vPos[0] = m_vBallPos;
			vPos[1] = D3DXVECTOR2( 0, 0 );
			vPos[2] = D3DXVECTOR2( 0, Setting::Height() );
			vPos[3] = D3DXVECTOR2( Setting::Width(), Setting::Height() );
			vPos[4] = D3DXVECTOR2( Setting::Width()*0.75f, Setting::Height()*0.2f );
			vPos[5] = m_vTargetPos;

			float fRate = FLOAT1;
			if( m_dwBallAniCheckTime >= FRAMEGETTIME() )
				fRate = FLOAT1 - ( ( m_dwBallAniCheckTime - FRAMEGETTIME() ) / 380.0f );

			BallAfterImage kImage( m_vCurPos, MAX_ALPHA_RATE * FLOAT08 );
			m_vBallAfterImageList.push_back( kImage );

			m_vCurPos = PointOnCubicBezier( vPos, fRate );

			if( m_dwBallAniCheckTime < dwCurTime )
			{
				m_dwBallAniCheckTime = dwCurTime + 200;
				m_BallAniState = BAS_NUMBER_EFFECT;

				BingoWnd *pWnd = dynamic_cast<BingoWnd*>( GetParent() );
				if( pWnd )
				{
					for( int i=BingoWnd::ID_NUMBER_BTN_1; i<BingoWnd::ID_NUMBER_BTN_25+1; ++i)
					{
						NumberButton *pBtn = dynamic_cast<NumberButton*>( pWnd->FindChildWnd(i) );
						if( pBtn && pBtn->GetNumber() == m_iLastGetNumber )
						{
							pBtn->SetClear();

							g_SoundMgr.PlaySound( m_szBallAniSound3 );

							ioBingo *pBingo = g_MyInfo.GetBingo();
							if( pBingo && pBingo->CheckAllBingo() )
								g_SoundMgr.PlaySound( m_szAllBingoSound );
						}
					}
				}

				ioBingo *pBingo = g_MyInfo.GetBingo();
				if( pBingo )
					pBingo->CheckBingoLine( m_iLastGetNumber );
			}
		}
		break;
	case BAS_NUMBER_EFFECT:
		{
			if( m_dwBallAniCheckTime < dwCurTime )
			{
				m_dwBallAniCheckTime = dwCurTime + 100;
				m_BallAniState = BAS_REWARD_EFFECT;
			}
		}
		break;
	case BAS_REWARD_EFFECT:
		m_BallAniState = BAS_NONE;
		break;
	}

	BallAfterImageList::iterator iter = m_vBallAfterImageList.begin();
	while( iter!=m_vBallAfterImageList.end() )
	{
		BallAfterImage &kInfo = *iter;
		if( kInfo.m_dwEndTime < FRAMEGETTIME() )
			iter = m_vBallAfterImageList.erase( iter );
		else
		{
			float fRate = ( kInfo.m_dwEndTime - FRAMEGETTIME() ) / 250.0f;
			kInfo.m_iAlpha = (MAX_ALPHA_RATE*0.5f)*fRate;
			kInfo.m_fScale = fRate;
			iter++;
		}
	}

	m_BallEffect.Process();
}

void BingoRightWnd::RenderBallAni( int x, int y )
{
	y -= 30;
	switch( m_BallAniState )
	{
	case BAS_POP:
		{
			if( m_pLightBall )
			{
				float fRate = static_cast<float>( m_dwBallAniCheckTime - FRAMEGETTIME() ) / 200.0f;
				int iYGap = 113 * fRate;
				int iAlpha = MAX_ALPHA_RATE - ( MAX_ALPHA_RATE * fRate );
				m_pLightBall->SetAlpha( iAlpha );
				m_pLightBall->Render( x + 111, y + 114 + iYGap );
			}
		}
		break;
	case BAS_SHINING:
		{
			if( m_pBallBg )
				m_pBallBg->Render( x + 111, y + 114, UI_RENDER_MULTIPLY );

			if( m_pRedBall )
			{
				m_pRedBall->SetScale( FLOAT1 );
				m_pRedBall->SetAlpha( MAX_ALPHA_RATE );
				m_pRedBall->Render( x + 111, y + 114 );
			}
			if( m_pSubIcon )
			{
				m_pSubIcon->SetScale( FLOAT1 );
				m_pSubIcon->SetAlpha( MAX_ALPHA_RATE );
				m_pSubIcon->Render( x + 111, y + 114 );
			}
			if( m_pTextGet )
			{
				m_pTextGet->SetAlpha( MAX_ALPHA_RATE );
				m_pTextGet->Render( x + 111, y + 75 );

				float fRate = static_cast<float>( m_dwBallAniCheckTime - FRAMEGETTIME() ) / FLOAT1000;
				int iAlpha = MAX_ALPHA_RATE * fRate;
				m_pTextGet->SetAlpha( iAlpha );
				m_pTextGet->SetColor( TCT_DEFAULT_WHITE );
				m_pTextGet->Render( x + 111, y + 75, UI_RENDER_NORMAL_ADD_COLOR );
			}

			switch( m_iBingoType )
			{
			case ioBingo::BT_RAND:
				{
					g_FontMgr.SetAlignType( TAT_CENTER );
					g_FontMgr.SetTextStyle( TS_NORMAL );
					g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
					g_FontMgr.SetBkColor( 0, 0, 0 );
					g_FontMgr.PrintText( x + 111, y + 103 , FONT_SIZE_20, "%d", m_iLastGetNumber );
				}
			break;
			}
			
		}
		break;
	case BAS_WAIT:
		{
			if( m_pBallBg )
				m_pBallBg->Render( x + 111, y + 114, UI_RENDER_MULTIPLY );

			if( m_pRedBall )
				m_pRedBall->Render( x + 111, y + 114 );
			if( m_pSubIcon )
				m_pSubIcon->Render( x + 111, y + 114 );

			if( m_pTextGet )
			{
				m_pTextGet->SetAlpha( MAX_ALPHA_RATE );
				m_pTextGet->Render( x + 111, y + 75 );
			}

			switch( m_iBingoType )
			{
			case ioBingo::BT_RAND:
				{
					g_FontMgr.SetAlignType( TAT_CENTER );
					g_FontMgr.SetTextStyle( TS_NORMAL );
					g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
					g_FontMgr.SetBkColor( 0, 0, 0 );
					g_FontMgr.PrintText( x + 111, y + 103 , FONT_SIZE_20, "%d", m_iLastGetNumber );
				}
				break;
			}
		}
		break;
	case BAS_MOVING:
		{
			int iXPos = m_vCurPos.x;
			int iYPos = m_vCurPos.y;

			if( m_pRedBall )
			{
				m_pRedBall->SetScale( FLOAT1 );
				m_pRedBall->SetAlpha( MAX_ALPHA_RATE );
				m_pRedBall->Render( iXPos, iYPos );
			}
			if( m_pSubIcon )
			{
				m_pSubIcon->SetScale( FLOAT1 );
				m_pSubIcon->SetAlpha( MAX_ALPHA_RATE );
				m_pSubIcon->Render( iXPos, iYPos );
			}
			switch( m_iBingoType )
			{
			case ioBingo::BT_RAND:
				{
					g_FontMgr.SetAlignType( TAT_CENTER );
					g_FontMgr.SetTextStyle( TS_NORMAL );
					g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
					g_FontMgr.SetBkColor( 0, 0, 0 );
					g_FontMgr.PrintText( iXPos, iYPos - 11, FONT_SIZE_20, "%d", m_iLastGetNumber );
				}
				break;
			}

		}
		break;
	case BAS_NUMBER_EFFECT:
		break;
	case BAS_REWARD_EFFECT:
		break;
	}

	int iSize = m_vBallAfterImageList.size();
	for( int i=0; i<iSize; ++i )
	{
		BallAfterImage &rkInfo = m_vBallAfterImageList[i];
		m_pRedBall->SetScale( rkInfo.m_fScale );
		m_pRedBall->SetAlpha( rkInfo.m_iAlpha );
		m_pRedBall->Render( rkInfo.m_vPosition.x, rkInfo.m_vPosition.y, UI_RENDER_NORMAL, TFO_BILINEAR );

		if( m_pSubIcon )
		{
			m_pSubIcon->SetScale( rkInfo.m_fScale );
			m_pSubIcon->SetAlpha( rkInfo.m_iAlpha );
			m_pSubIcon->Render( rkInfo.m_vPosition.x, rkInfo.m_vPosition.y, UI_RENDER_NORMAL, TFO_BILINEAR );
		}
	}

	m_BallEffect.Render( x + 111, y + 114, 2.0f, UI_RENDER_SCREEN, TFO_BILINEAR );
}

void BingoRightWnd::SetWindowType( RightWndType eType )
{
	if( m_WindowType == eType )
		return;

	m_WindowType = eType;
	switch( m_WindowType )
	{
	case RWT_START:
		{
			ShowChildWnd( ID_BINGO_START_BTN );

			HideChildWnd( ID_GET_NUMBER_BTN );
			HideChildWnd( ID_SHUFFLE_NUMBER_BTN );
			HideChildWnd( ID_SHUFFLE_REWARD_BTN );
			HideChildWnd( ID_GET_SPECIAL_NUMBER_BTN );
			HideChildWnd( ID_INIT_BTN );
			HideChildWnd( ID_REWARD_LIST_BTN );

			HideChildWnd( ID_NEW_BINGO_BTN );

			HideChildWnd( ID_HELP_BTN_1 );
			HideChildWnd( ID_HELP_BTN_2 );
			HideChildWnd( ID_HELP_BTN_3 );
			HideChildWnd( ID_CLOSE_BTN );
		}
		break;
	case RWT_RAND_PLAY:
		{
			HideChildWnd( ID_BINGO_START_BTN );

			ShowChildWnd( ID_GET_NUMBER_BTN );
			ShowChildWnd( ID_SHUFFLE_NUMBER_BTN );
			ShowChildWnd( ID_SHUFFLE_REWARD_BTN );
			ShowChildWnd( ID_GET_SPECIAL_NUMBER_BTN );
			ShowChildWnd( ID_INIT_BTN );
			ShowChildWnd( ID_REWARD_LIST_BTN );

			HideChildWnd( ID_NEW_BINGO_BTN );

			ShowChildWnd( ID_HELP_BTN_1 );
			ShowChildWnd( ID_HELP_BTN_2 );
			ShowChildWnd( ID_HELP_BTN_3 );
			HideChildWnd( ID_CLOSE_BTN );
		}
		break;
	case RWT_FIXED_PLAY:
		{
			HideChildWnd( ID_BINGO_START_BTN );

			HideChildWnd( ID_GET_NUMBER_BTN );
			HideChildWnd( ID_SHUFFLE_NUMBER_BTN );
			HideChildWnd( ID_SHUFFLE_REWARD_BTN );
			HideChildWnd( ID_GET_SPECIAL_NUMBER_BTN );
			ShowChildWnd( ID_INIT_BTN );
			ShowChildWnd( ID_REWARD_LIST_BTN );

			HideChildWnd( ID_NEW_BINGO_BTN );

			HideChildWnd( ID_HELP_BTN_1 );
			HideChildWnd( ID_HELP_BTN_2 );
			HideChildWnd( ID_HELP_BTN_3 );
			ShowChildWnd( ID_CLOSE_BTN );
		}
		break;
	case RWT_ALL_BINGO:
		{
			HideChildWnd( ID_BINGO_START_BTN );

			HideChildWnd( ID_GET_NUMBER_BTN );
			HideChildWnd( ID_SHUFFLE_NUMBER_BTN );
			HideChildWnd( ID_SHUFFLE_REWARD_BTN );
			HideChildWnd( ID_GET_SPECIAL_NUMBER_BTN );
			HideChildWnd( ID_INIT_BTN );
			HideChildWnd( ID_REWARD_LIST_BTN );

			ShowChildWnd( ID_NEW_BINGO_BTN );

			HideChildWnd( ID_HELP_BTN_1 );
			HideChildWnd( ID_HELP_BTN_2 );
			HideChildWnd( ID_HELP_BTN_3 );
			HideChildWnd( ID_CLOSE_BTN );
		}
		break;
	}
}

void BingoRightWnd::SetBallAni( int iNumber, ioBingo *pBingo, const ioHashString& szNewBallImage, const ioHashString& szSubImage )
{
	if( iNumber <= 0 )
		return;

	if( m_BallAniState != BAS_NONE && m_iLastGetNumber > 0 )
	{
		BingoWnd *pWnd = dynamic_cast<BingoWnd*>( GetParent() );
		if( pWnd )
		{
			for( int i=BingoWnd::ID_NUMBER_BTN_1; i<BingoWnd::ID_NUMBER_BTN_25+1; ++i)
			{
				NumberButton *pBtn = dynamic_cast<NumberButton*>( pWnd->FindChildWnd(i) );
				if( pBtn && pBtn->GetNumber() == m_iLastGetNumber )
				{
					pBtn->SetClear();
				}
			}
		}

		pBingo->CheckBingoLine( m_iLastGetNumber );
	}

	if( !szNewBallImage.IsEmpty() )
	{
		SAFEDELETE( m_pRedBall );
		SAFEDELETE( m_pSubIcon );
		m_pRedBall =  g_UIImageSetMgr.CreateImageByFullName( szNewBallImage );
		m_pSubIcon =  g_UIImageSetMgr.CreateImageByFullName( szSubImage );
	}

	m_BallAniState = BAS_POP;
	m_dwBallAniCheckTime = FRAMEGETTIME() + 200;
	m_iLastGetNumber = iNumber;
	m_vTargetPos = D3DXVECTOR2( 0.0f, 0.0f );
	m_vBallPos = m_vCurPos = D3DXVECTOR2( GetDerivedPosX() + 111, GetDerivedPosY() + 84 );

	BingoWnd *pWnd = dynamic_cast<BingoWnd*>( GetParent() );
	if( pWnd )
	{
		for( int i=BingoWnd::ID_NUMBER_BTN_1; i<BingoWnd::ID_NUMBER_BTN_25+1; ++i)
		{
			NumberButton *pBtn = dynamic_cast<NumberButton*>( pWnd->FindChildWnd(i) );
			if( pBtn && pBtn->GetNumber() == iNumber )
			{
				m_vTargetPos.x = pBtn->GetDerivedPosX() + 26;
				m_vTargetPos.y = pBtn->GetDerivedPosY() + 26;
			}
		}
	}
}

void BingoRightWnd::SetActvieChildBtn()
{
	SetChildActive( ID_GET_NUMBER_BTN );
	SetChildActive( ID_SHUFFLE_NUMBER_BTN );
	SetChildActive( ID_SHUFFLE_REWARD_BTN );
	SetChildActive( ID_GET_SPECIAL_NUMBER_BTN );
	SetChildActive( ID_INIT_BTN );
}

void BingoRightWnd::SetInActvieChildBtn()
{
	SetChildInActive( ID_GET_NUMBER_BTN );
	SetChildInActive( ID_SHUFFLE_NUMBER_BTN );
	SetChildInActive( ID_SHUFFLE_REWARD_BTN );
	SetChildInActive( ID_GET_SPECIAL_NUMBER_BTN );
	SetChildInActive( ID_INIT_BTN );
}

D3DXVECTOR2 BingoRightWnd::PointOnCubicBezier(D3DXVECTOR2* vP, float t)
{
	D3DXVECTOR2 vA0, vA1, vA2, vA3, vA4;
	D3DXVECTOR2 vQ0, vQ1, vQ2, vQ3;
	D3DXVECTOR2 vR0, vR1, vR2;
	D3DXVECTOR2 vJ0, vJ1;
	D3DXVECTOR2 vResult;

	vA0 = CalcLinear( vP[0], vP[1] , t );
	vA1 = CalcLinear( vP[1], vP[2] , t);
	vA2 = CalcLinear( vP[2], vP[3] , t);
	vA3 = CalcLinear( vP[3], vP[4] , t);
	vA4 = CalcLinear( vP[4], vP[5] , t);

	vQ0 = vA0 + ( vA1 - vA0 ) * t ;
	vQ1 = vA1 + ( vA2 - vA1 ) * t ;
	vQ2 = vA2 + ( vA3 - vA2 ) * t ;
	vQ3 = vA3 + ( vA4 - vA3 ) * t ;

	vR0 = vQ0 + ( vQ1 - vQ0 ) * t;
	vR1 = vQ1 + ( vQ2 - vQ1 ) * t;
	vR2 = vQ2 + ( vQ3 - vQ2 ) * t;

	vJ0 = vR0 + ( vR1 - vR0 ) * t;
	vJ1 = vR1 + ( vR2 - vR1 ) * t;

	vResult = vJ0 + ( vJ1 - vJ0 ) * t;

	return vResult;
}

D3DXVECTOR2 BingoRightWnd::CalcLinear( D3DXVECTOR2 vPos0, D3DXVECTOR2 vPos1, float t )
{
	return vPos0 + ( vPos1 - vPos0 ) * t;
}

//////////////////////////////////////////////////////////////////////////
BingoHelpButton::BingoHelpButton()
{

}

BingoHelpButton::~BingoHelpButton()
{

}

void BingoHelpButton::iwm_hide()
{
	HideChildWnd( ID_HELP_TOOL_TIP );
}

void BingoHelpButton::iwm_mouseover( const ioMouse& mouse )
{
	BingoHelpToolTip *pWnd = dynamic_cast<BingoHelpToolTip*>( FindChildWnd( ID_HELP_TOOL_TIP ) );
	if( pWnd )
		pWnd->SetInfo( m_dwID );
}

void BingoHelpButton::iwm_mouseleave( const ioMouse& mouse )
{
	HideChildWnd( ID_HELP_TOOL_TIP );
}

void BingoHelpButton::OnRender()
{
	ioButton::OnRender();
}