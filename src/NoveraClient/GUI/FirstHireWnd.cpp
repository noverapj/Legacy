

#include "StdAfx.h"
#include "FirstHireWnd.h"

///////////////////////////////////////////////////////////////////////////
int BankruptcyCharWnd::m_iClassSelect = 0;
BankruptcyCharWnd::BankruptcyCharWnd()
{
	m_pBackGround  = NULL;
	m_pSoldierIcon = NULL;
	m_pSoldierSubIcon = NULL;
}

BankruptcyCharWnd::~BankruptcyCharWnd()
{
	SAFEDELETE( m_pBackGround );
	SAFEDELETE( m_pSoldierIcon );
	SAFEDELETE( m_pSoldierSubIcon );
}

void BankruptcyCharWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "BackGround" )
	{
		SAFEDELETE( m_pBackGround );
		m_pBackGround = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void BankruptcyCharWnd::UpdateFirstClass()
{
	if( m_iClassSelect == 0 )
	{
		DWORDVec dwFirstClassList;
		int iSize = g_SetItemInfoMgr.GetTotalSetCount();
		for (int i = 0; i < iSize ; i++)
		{
			if( g_ClassPrice.IsCashOnlyByArray( i ) )
				continue;

			if( !g_ClassPrice.IsActiveClass( i ) )
				continue;

			int iItemCode = g_ClassPrice.GetClassType( i );
			const ioSetItemInfo *pSetItemInfo = g_SetItemInfoMgr.GetSetInfoByItemCode( iItemCode );

			if( !pSetItemInfo )
				continue;

			if( pSetItemInfo->GetPackageType() != ioSetItemInfo::PT_NORMAL )
				continue;

			if( g_MyInfo.IsCanBuyItem( pSetItemInfo ) )
				dwFirstClassList.push_back( pSetItemInfo->GetSetCode() - SET_ITEM_CODE );
		}

		if( dwFirstClassList.empty() )
		{
			HideWnd();
		}
		else
		{
			 int iSize = dwFirstClassList.size();
			 int iRand = rand()%iSize;
			 if( COMPARE( iRand , 0, iSize ) )
				m_iClassSelect = dwFirstClassList[iRand];
		}
	}

	if( m_iClassSelect != 0 )
	{
		// 지급 용병
		SAFEDELETE( m_pSoldierIcon );
		SAFEDELETE( m_pSoldierSubIcon );
		m_pSoldierIcon = g_MyInfo.GetSoldierIcon( m_iClassSelect, true );
		m_pSoldierSubIcon = g_MyInfo.GetSoldierSubIcon( m_iClassSelect );
		m_szClassName  = g_MyInfo.GetClassName( m_iClassSelect );

		// 내용
		m_szHelpVec.clear();
		ioINILoader_e kLoader( "config/sp2_bankruptcy.ini" );
		kLoader.SetTitle_e( "help" );
		int max_help = kLoader.LoadInt_e( "max_help", 0 );
		char szKeyName[MAX_PATH]="";
		for (int i = 0; i < max_help ; i++)
		{
			ZeroMemory(szKeyName, sizeof( szKeyName ) );
			StringCbPrintf_e( szKeyName, sizeof(szKeyName), "desc_%d", i+1);

			char szBuf[MAX_PATH]="";
			kLoader.LoadString( szKeyName, "", szBuf, MAX_PATH );
			m_szHelpVec.push_back( ioHashString( szBuf ) );
		}

		// 버튼
		ioButton *pFirsHireBtn = (ioButton*)FindChildWnd( ID_RECEIVE_CHAR_BTN );
		if( pFirsHireBtn )
		{
			char szTitle[MAX_PATH] = "";
			SafeSprintf( szTitle, sizeof( szTitle ), STR(1), m_szClassName.c_str(), g_ClassPrice.GetBankruptcyLimit() / 3600 );
			pFirsHireBtn->SetTitleText( szTitle );
		}
	}
}

void BankruptcyCharWnd::iwm_show()
{
	UpdateFirstClass();
}	

void BankruptcyCharWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_RECEIVE_CHAR_BTN:
		if( cmd == IOBN_BTNUP)
		{	
			if( !g_MyInfo.IsBankruptcy() )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
				HideWnd();
			}
			else if( g_MyInfo.GetClassArrayExceptExercise( m_iClassSelect ) != -1 )
			{
				if( g_MyInfo.IsCharActive( g_MyInfo.GetClassArray( m_iClassSelect ) ) )
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
					return;
				}

				SP2Packet kPacket( CTPK_CHAR_EXTEND );
				kPacket << CHAR_EXTEND_BANKRUPTCY;
				kPacket << g_MyInfo.GetCharIndex( g_MyInfo.GetClassArray( m_iClassSelect ) );
				kPacket << g_ClassPrice.GetBankruptcyLimit();
				kPacket << false; // bCash
				TCPNetwork::SendToServer( kPacket );
				g_App.SetMouseBusy( true );
				g_App.ResetConnectedMoveMode();

				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
				HideWnd();
			}
			else if( g_MyInfo.GetCharCount() >= g_MyInfo.GetCurMaxCharSlot() )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4) );
				HideWnd();
			}
			else
			{
				SP2Packet kPacket( CTPK_CHAR_CREATE );
				kPacket << CHAR_CREATE_BANKRUPTCY; 
				kPacket << m_iClassSelect;
				kPacket << 1;      // 종족 인간
				kPacket	<< 1;      // 성별 남자
				kPacket << g_ClassPrice.GetBankruptcyLimit();
				kPacket << 0;     // iPrice
				kPacket << false; // bCash
				kPacket << CPT_TIME;
				TCPNetwork::SendToServer( kPacket );
				g_App.SetMouseBusy( true );
				g_App.ResetConnectedMoveMode();

				HideWnd();
				m_iClassSelect = 0;
			}
		}
		break;
	}
}

void BankruptcyCharWnd::OnRender()
{
	ioWnd::OnRender();
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_pBackGround )
	{
		m_pBackGround->Render( iXPos + 124, iYPos + 97, UI_RENDER_MULTIPLY );
		if( m_pSoldierIcon )
			m_pSoldierIcon->Render( iXPos + 124, iYPos + 97 );
		if( m_pSoldierSubIcon )
			m_pSoldierSubIcon->Render( iXPos + 124, iYPos + 97 );
	}

	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	for(int i = 0;i < (int)m_szHelpVec.size();i++)
	{
		g_FontMgr.PrintText( iXPos + 124, iYPos + 149 + i * 18, FONT_SIZE_13, m_szHelpVec[i].c_str() );
	}
}