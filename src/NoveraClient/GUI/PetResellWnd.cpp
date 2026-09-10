
#include "StdAfx.h"

#include "PetResellWnd.h"

#include "../ioComplexStringPrinter.h"

#include "PetWnd.h"
#include "PetIconBtn.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PetResellWnd::PetResellWnd()
{
	m_pSellMark = NULL;
	m_pBottomFrm = NULL;
	m_pPreEdit = NULL;
	m_pSellEdit = NULL;

	m_nPetCode = 0;
	m_nPetLevel = 0;
	m_nPetRank = 0;
	m_nPetSlotIndex = 0;
	m_szPetName.Clear();
}

PetResellWnd::~PetResellWnd()
{
	SAFEDELETE(m_pSellMark);
	SAFEDELETE(m_pBottomFrm);
}

void PetResellWnd::iwm_show()
{	
	ioEdit *pEdit = dynamic_cast<ioEdit*>( FindChildWnd( ID_SELL_EDIT ) );
	if( pEdit && pEdit->IsShow() )
	{
		m_pPreEdit = ioEdit::m_pKeyFocusEdit;
		pEdit->ClearString();
		pEdit->SetKeyFocus();
		pEdit->ShowWnd();

		g_InputBox.SetString( "" );

		m_pSellEdit = pEdit;
	}
}

void PetResellWnd::iwm_hide()
{
	if( m_pPreEdit )	
	{
		ioWnd *pParent = m_pPreEdit->GetParent();
		if( pParent )
		{
			if( pParent->IsShow() )
				((ioEdit*)m_pPreEdit)->SetKeyFocus();	
		}
	}	
	m_pPreEdit = NULL;

	m_nPetCode = 0;
	m_nPetLevel = 0;
	m_nPetRank = 0;

	m_szPetName.Clear();
}

void PetResellWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
	case ID_CANCEL:
		if( cmd == IOBN_BTNUP )
			HideWnd();
		break;
	case ID_SELL:
		if( cmd == IOBN_BTNUP )
		{	
			if( !m_pSellEdit || !m_pSellEdit->IsShow() )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
				return;
			}

			ioEdit *pEdit = dynamic_cast<ioEdit*>( m_pSellEdit );
			if( strcmp( pEdit->GetText(), STR(2) ) != 0 )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
				return;
			}

			HideWnd();
			
			//패킷 전송
			SP2Packet kPacket( CTPK_PET_SELL );
			kPacket << m_nPetSlotIndex;
			TCPNetwork::SendToServer( kPacket );
			TCPNetwork::MouseBusy( true );				
		}
		break;
	case ID_SELL_EDIT:
		if( cmd == IOED_INPUT )
		{
			if( param == 0 || param == 1 )
			{
				ioEdit *pEdit = dynamic_cast<ioEdit*>(pWnd);
				pEdit->SetKeyFocus();
			}
		}
		break;
	}
}

void PetResellWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "sell_mark" )
	{
		SAFEDELETE( m_pSellMark );
		m_pSellMark = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}

void PetResellWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "BottomFrm" )
	{
		SAFEDELETE( m_pBottomFrm );
		m_pBottomFrm = pFrame;
	}
	else
		ioWnd::AddRenderFrame( szType, pFrame );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PetResellWnd::OnRender()
{
	ioWnd::OnRender();

	int nXPos = GetDerivedPosX();
	int nYPos = GetDerivedPosY();

	RenderDesc( nXPos, nYPos );
	RenderManual( nXPos, nYPos );
}

void PetResellWnd::OnRenderAfterChild()
{
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	enum
	{
		ICON_X		= 125,
		ICON_Y		= 98,
	};

	if( m_pSellMark )
		m_pSellMark->Render( iXPos+ICON_X, iYPos+ICON_Y );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PetResellWnd::RenderDesc( int nXPos, int nYPos )
{
	enum
	{
		NAME_X_OFFSET = 125,
		NAME_Y_OFFSET = 145,

		RANK_X_OFFSET = 125,
		RANK_Y_OFFSET = 166,
		
		TEXT_X_OFFSET1 = 125,

		TEXT_Y_OFFSET1 = 255,
		TEXT_Y_OFFSET2 = 285,
	};

	ioHashString szRank = g_PetInfoMgr.GetRankName( m_nPetRank );

	if ( m_nPetCode <= 0 || m_nPetRank <= 0 || m_szPetName.IsEmpty() || szRank.IsEmpty() )
		return;

	if( m_pBottomFrm )
		m_pBottomFrm->Render( nXPos, nYPos, UI_RENDER_MULTIPLY );

	ioComplexStringPrinter kPrinter;
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextStyle( TS_NORMAL );
	
	//펫 레벨
	int nMaxPetLevel = g_PetInfoMgr.GetMaxLevel( (PetRankType)m_nPetRank );
	if ( m_nPetLevel < nMaxPetLevel )
	{
		kPrinter.SetTextColor( TCT_DEFAULT_RED );
		kPrinter.AddTextPiece( FONT_SIZE_18, STR(1), m_nPetLevel );
	}
	else
	{
		kPrinter.SetTextColor( TCT_DEFAULT_RED );
		kPrinter.AddTextPiece( FONT_SIZE_18, STR(1), m_nPetLevel );
		kPrinter.SetTextColor( TCT_DEFAULT_RED );
		kPrinter.AddTextPiece( FONT_SIZE_18, STR(2) );
	}

	//펫 이름
	kPrinter.SetTextColor( TCT_DEFAULT_RED );
	kPrinter.AddTextPiece( FONT_SIZE_18, STR(3), m_szPetName.c_str() );
	kPrinter.SetTextColor( TCT_DEFAULT_RED );
	kPrinter.PrintFullText( nXPos + NAME_X_OFFSET, nYPos+NAME_Y_OFFSET, TAT_CENTER );
	kPrinter.ClearList();
	kPrinter.SetTextColor( TCT_DEFAULT_RED );
	kPrinter.AddTextPiece( FONT_SIZE_18, STR(4), szRank.c_str() );
	kPrinter.PrintFullText( nXPos + RANK_X_OFFSET, nYPos+RANK_Y_OFFSET, TAT_CENTER );
	kPrinter.ClearList();

	//풀어주기 입력
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, "[" );
	kPrinter.SetTextColor( TCT_DEFAULT_RED );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(5) );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(6) );
	kPrinter.PrintFullText( nXPos + TEXT_X_OFFSET1, nYPos+TEXT_Y_OFFSET1, TAT_CENTER );
	kPrinter.ClearList();

	if( m_pSellEdit != ioEdit::m_pKeyFocusEdit )
	{
		kPrinter.SetBkColor( 0, 0, 0 );
		kPrinter.SetTextStyle( TS_NORMAL );
		kPrinter.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
		kPrinter.AddTextPiece( FONT_SIZE_12, STR(7) );
		kPrinter.PrintFullText( nXPos + TEXT_X_OFFSET1, nYPos+TEXT_Y_OFFSET2, TAT_CENTER );
		kPrinter.ClearList();
	}
}

void PetResellWnd::RenderManual( int nXPos, int nYPos )	
{
	enum
	{
		TEXT_X_OFFSET = 125,
		TEXT_Y_OFFSET1 = 197,
		TEXT_Y_OFFSET2 = 216,
		TEXT_Y_OFFSET3 = 235,
	};

	int nResellPeso = g_PetInfoMgr.GetResellPeso();
	char szConvertNum[MAX_PATH] = "";
	Help::ConvertNumToStrComma( nResellPeso, szConvertNum, sizeof( szConvertNum ) );

	ioComplexStringPrinter kPrinter;
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextStyle( TS_NORMAL );

	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(1) );
	kPrinter.PrintFullText( nXPos+TEXT_X_OFFSET, nYPos+TEXT_Y_OFFSET1, TAT_CENTER );
	kPrinter.ClearList();

	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(2) );
	kPrinter.SetTextColor( TCT_DEFAULT_BLUE );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(3), szConvertNum );
	kPrinter.PrintFullText( nXPos+TEXT_X_OFFSET, nYPos+TEXT_Y_OFFSET2, TAT_CENTER );
	kPrinter.ClearList();

	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(4) );
	kPrinter.PrintFullText( nXPos+TEXT_X_OFFSET, nYPos+TEXT_Y_OFFSET3, TAT_CENTER );
	kPrinter.ClearList();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PetResellWnd::SetIcon( int nPetCode, int nPetLevel, int nPetRank, int nPetSlotIndex, float fScale, float fBackScale )
{	
	if ( nPetCode <= 0 || nPetRank <= 0 )
		return;

	m_nPetCode = nPetCode;
	m_nPetLevel = nPetLevel;
	m_nPetRank = nPetRank;
	m_nPetSlotIndex = nPetSlotIndex;
	m_szPetName = g_PetInfoMgr.GetPetName( m_nPetCode, (PetRankType)m_nPetRank );

	PetIconBtn *pPetIconBtn = dynamic_cast<PetIconBtn*>(FindChildWnd( ID_PET_ICON ) );
	if( pPetIconBtn )
		pPetIconBtn->SetIcon( nPetCode, nPetLevel, nPetRank, fScale, fBackScale, false );

	ShowWnd();
}
