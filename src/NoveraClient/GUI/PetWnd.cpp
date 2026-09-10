
#include "StdAfx.h"

#include "PetWnd.h"

#include "../ioComplexStringPrinter.h"
#include "../ioPlayStage.h"
#include "../ioExtendSoundManager.h"

#include "PetViewWnd.h"
#include "PetResellWnd.h"
#include "PetIconBtn.h"
#include "PetEatWnd.h"
#include "PetCompoundWnd.h"
#include "PetResellResultWnd.h"

#include "MyInventoryWnd.h"
#include "NewShopWnd.h"

//////////////////////////////////////////////////////////////////////////
PetWnd::PetWnd()
{
	m_pPlayStage = NULL;
	m_pNameLine = NULL;
	m_pGaugeMainBack = NULL;
	m_pGaugeBack  = NULL;
	m_pGauge      = NULL;
	m_pNewGauge      = NULL;
	m_pGrowthBackGray   = NULL;
	m_pGrowthBackLight  = NULL;
	m_pExpText = NULL;
	m_pExpNumText = NULL;
	m_pPerText = NULL;

	Init();
}

PetWnd::~PetWnd()
{
	SAFEDELETE( m_pNameLine );
	SAFEDELETE( m_pGaugeMainBack );
	SAFEDELETE( m_pGaugeBack );
	SAFEDELETE( m_pGauge );
	SAFEDELETE( m_pNewGauge );
	SAFEDELETE( m_pGrowthBackGray );
	SAFEDELETE( m_pGrowthBackLight );
	SAFEDELETE( m_pExpText );
	SAFEDELETE( m_pExpNumText );
	SAFEDELETE( m_pPerText );
}

void PetWnd::iwm_create()
{
	g_GUIMgr.AddWnd( "XML/peteatwnd.xml", this );
	g_GUIMgr.AddWnd( "XML/petcompoundwnd.xml", this );
	g_GUIMgr.AddWnd( "XML/petresellresultwnd.xml", this );
}

void PetWnd::iwm_show()
{	
	enum 
	{ 
		Y_OFFSET = -20 
	};

	int nXPos = ( Setting::Width() - GetWidth() ) / 2;
	int nYPos = ( Setting::Height() - GetHeight() ) / 2 + Y_OFFSET;

	NewShopWnd *pShopWnd = dynamic_cast<NewShopWnd*>( g_GUIMgr.FindWnd( NEW_SHOP_WND ) );
	if( pShopWnd && pShopWnd->IsShow() )
		pShopWnd->HideWnd();

	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>( g_GUIMgr.FindWnd( MY_INVENTORY_WND ) );
	if( pInvenWnd && pInvenWnd->IsShow() )
		pInvenWnd->HideWnd();

	ioMovingWnd::SetWndPos( nXPos, nYPos );

	ShowMainMenu();
	ResetPetWnd( true );

	//펫 뷰 초기화
	PetViewWnd *pPetViewWnd = dynamic_cast<PetViewWnd*>( FindChildWnd( ID_PETVIEW ));
	if( pPetViewWnd )
	{
		pPetViewWnd->CreateEffectDummy();
		pPetViewWnd->ShowWnd();
	}

#if defined( USE_GA )
	// WND_PET
	g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FWND%2FPET" );
#endif

	if( g_MyInfo.GetChannelingType() == CNT_NEXON )
	{
		SetChildInActive( ID_CHARGE_GOLD );
	}
}

void PetWnd::iwm_hide()
{	
	HideChildWnd( PET_RESELL_RESULT_WND );
	HideChildWnd( PET_COMPOUND_WND );
	HideChildWnd( PET_EAT_WND );
	HideChildWnd( ID_PETVIEW );
	Init();
}

void PetWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();	
	switch( dwID )
	{
	case ID_CHARGE_GOLD:
		if( cmd == IOBN_BTNUP )
		{
			if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
			{
				if( m_pPlayStage && m_pPlayStage->GetModeType() != MT_MYROOM )
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
					return;
				}
			}
			else
			{
				if( m_pPlayStage && m_pPlayStage->GetModeType() != MT_TRAINING && m_pPlayStage->GetModeType() != MT_HEADQUARTERS && m_pPlayStage->GetModeType() != MT_HOUSE )
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, "골드충전은 광장이나 본부에서만 가능합니다." );
					return;
				}
			}

			ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
			if( pLocal )
				pLocal->OpenFillCashPage();
		}
		break;
	case ID_SHOP:
		if( cmd == IOBN_BTNUP )
		{
			NewShopWnd *pShopWnd = dynamic_cast<NewShopWnd*>(g_GUIMgr.FindWnd(NEW_SHOP_WND));
			if( pShopWnd )
				pShopWnd->ShowEtcItemTab();
		}
		break;
	case ID_EXIT:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;
	case ID_ACTIVE:
		if( cmd == IOBN_BTNUP )
		{
			if ( m_nSelectSlotIndex == 0 )
				return;

			//패킷 전송
			SP2Packet kPacket( CTPK_PET_CHANGE );
			kPacket << m_nSelectSlotIndex;
			kPacket << true;
			TCPNetwork::SendToServer( kPacket );
			TCPNetwork::MouseBusy( true );
		}
		break;
	case ID_INACTIVE:
		if( cmd == IOBN_BTNUP )
		{	
			if ( m_nSelectSlotIndex == 0 )
				return;

			//패킷 전송
			SP2Packet kPacket( CTPK_PET_CHANGE );
			kPacket << m_nSelectSlotIndex;
			kPacket << false;
			TCPNetwork::SendToServer( kPacket );
			TCPNetwork::MouseBusy( true );
		}
		break;
	case ID_REMOVE:
		{
			if( cmd == IOBN_BTNUP )
			{
				PetResellWnd *pPetResellWnd = dynamic_cast<PetResellWnd*>(g_GUIMgr.FindWnd( PET_RESELL_WND ));
				if ( pPetResellWnd )
					pPetResellWnd->SetIcon( m_nCurPetCode, m_nCurPetLv, m_nCurRank, m_nSelectSlotIndex, FLOAT1, FLOAT1 );
			}
			else if ( cmd == IOEX_BTNUP )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
				return;
			}
		}
		break;
	case ID_EAT:
		{
			if( cmd == IOBN_BTNUP )
			{
				HideChildWnd( ID_EAT_TIP );
				SetEatPet();
			}
			else if ( cmd == IOEX_BTNUP )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
				return;
			}
			else if( cmd == IOWN_OVERED )
			{
				ShowChildWnd( ID_EAT_TIP );
			}
			else if( cmd == IOWN_LEAVED )
			{
				HideChildWnd( ID_EAT_TIP );
			}
		}
		break;
	case ID_COMPOUND:
		{
			if( cmd == IOBN_BTNUP )
			{
				HideChildWnd( ID_COMPOUND_TIP );
				SetCompoundPet();
			}
			else if ( cmd == IOEX_BTNUP )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4) );
				return;
			}
			else if( cmd == IOWN_OVERED )
			{
				ShowChildWnd( ID_COMPOUND_TIP );
			}
			else if( cmd == IOWN_LEAVED )
			{
				HideChildWnd( ID_COMPOUND_TIP );
			}
		}
		break;
	}

	if ( COMPARE( dwID, ID_PETSLOT1, ID_PETSLOT20+1 ) )
	{
		if ( cmd == IOBN_BTNUP )
		{
			SelectPet( dwID );
		}
	}
}

void PetWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "NameLine" )
	{
		SAFEDELETE( m_pNameLine );
		m_pNameLine = pImage;
	}
	else if( szType == "ExpText" )
	{
		SAFEDELETE( m_pExpText );
		m_pExpText = pImage;
	}
	else if( szType == "ExpNumText" )
	{
		SAFEDELETE( m_pExpNumText );
		m_pExpNumText = pImage;
	}
	else if( szType == "PerText" )
	{
		SAFEDELETE( m_pPerText );
		m_pPerText = pImage;
	}
	else if( szType == "GrowthBackGray" )
	{
		SAFEDELETE( m_pGrowthBackGray );
		m_pGrowthBackGray = pImage;
	}
	else if( szType == "GrowthBackLight" )
	{
		SAFEDELETE( m_pGrowthBackLight );
		m_pGrowthBackLight = pImage;
	}
	else if( szType == "MainBack" )
	{
		SAFEDELETE( m_pGaugeMainBack );
		m_pGaugeMainBack = pImage;
	}
	else if( szType == "GaugeBack" )
	{
		SAFEDELETE( m_pGaugeBack );
		m_pGaugeBack = pImage;
	}
	else if( szType == "Gauge" )
	{
		SAFEDELETE( m_pGauge );
		m_pGauge = pImage;
	}
	else if( szType == "NewGauge" )
	{
		SAFEDELETE( m_pNewGauge );
		m_pNewGauge = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );	
}

void PetWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	ioWnd::AddRenderFrame( szType, pFrame );
}

void PetWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_dwCheckExpGaugeUpTime = xElement.GetIntAttribute_e( "ExpGaugeUp_Time" );
	m_fExpGaugeSpeed = xElement.GetFloatAttribute_e( "ExpGaugeUp_Speed" );

	m_szExpUpSound = xElement.GetStringAttribute( "ExpUp_Sound" );
	m_szLevelUpSound = xElement.GetStringAttribute( "LevelUp_Sound" );
	m_szRankUpSound = xElement.GetStringAttribute( "RankUp_Sound" );
	m_szCompoundSound = xElement.GetStringAttribute( "Compound_Sound" );

	m_szExpUpEffect = xElement.GetStringAttribute( "ExpUp_Effect" );
	m_szLevelUpEffect = xElement.GetStringAttribute( "LevelUp_Effect" );
	m_szRankUpEffect = xElement.GetStringAttribute( "RankUp_Effect" );
	m_szCompoundEffect = xElement.GetStringAttribute( "Compound_Effect" );
}

void PetWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;	

	ioMovingWnd::OnProcess( fTimePerSec );	

	ExpGaugeUp();
}

void PetWnd::OnRender()
{
	ioWnd::OnRender();

	int nXPos = GetDerivedPosX();
	int nYPos = GetDerivedPosY();

	RenderTitle( nXPos, nYPos );
	RenderInfo( nXPos, nYPos );
	RenderStat( nXPos, nYPos );
	
	if ( !m_bEatResultExp )
		RenderLevelInfo( nXPos, nYPos );
	else
		RenderEatResultExp( nXPos, nYPos );
}

void PetWnd::RenderLevelInfo( int nXPos, int nYPos )
{
	if ( m_nCurPetCode <= 0 )
		return;

	if( !m_pGaugeMainBack || !m_pGaugeBack || !m_pGauge )
		return;

	enum 
	{		
		GAUGE_X_OFFSET = 542,
		GAUGE_Y_OFFSET = 302,
		GAUGE_BACK_OFFSET = 2,

		MANUAL_X = 627,
		MANUAL_Y = 303,
	};

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );

	if ( m_nCurPetLv < m_nMaxPetLv )
	{
		//게이지
		int nGaugeWidth = m_pGauge->GetWidth() * (float)min( FLOAT1, m_fSelectPetLevelRate );
		m_pGaugeMainBack->Render( nXPos + GAUGE_X_OFFSET - GAUGE_BACK_OFFSET, nYPos + GAUGE_Y_OFFSET - GAUGE_BACK_OFFSET );
		m_pGaugeBack->Render( nXPos + GAUGE_X_OFFSET, nYPos + GAUGE_Y_OFFSET );
		m_pGauge->RenderWidthCut( nXPos + GAUGE_X_OFFSET, nYPos + GAUGE_Y_OFFSET, 0, nGaugeWidth );
		
		//Exp
		RenderExpNum( nXPos, nYPos, m_fSelectPetLevelRate );
	}
	else
	{
		kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter.AddTextPiece( FONT_SIZE_12, STR(1) );	
		kPrinter.PrintFullText( nXPos + MANUAL_X, nYPos + MANUAL_Y, TAT_CENTER );
		kPrinter.ClearList();
	}	
}

void PetWnd::RenderEatResultExp( int nXPos, int nYPos )
{
	if ( m_nCurPetCode <= 0 )
		return;

	if( !m_pGaugeMainBack || !m_pGaugeBack || !m_pNewGauge || !m_pGauge || m_nMaxExp <= 0 )
		return;

	enum 
	{		
		GAUGE_X_OFFSET = 542,
		GAUGE_Y_OFFSET = 302,
		GAUGE_BACK_OFFSET = 2,
		
		MANUAL_X = 627,
		MANUAL_Y = 303,
	};

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );

	if ( m_nCurPetLv < m_nMaxPetLv )
	{
		//게이지
		int nNewGaugeWidth = m_pNewGauge->GetWidth() * (float)min( FLOAT1, m_fNewExpRate );
		int nPreGaugeWidth = m_pGauge->GetWidth() * (float)min( FLOAT1, m_fPreExpRate );

		//렌더 순서
		m_pGaugeMainBack->Render( nXPos + GAUGE_X_OFFSET - GAUGE_BACK_OFFSET, nYPos + GAUGE_Y_OFFSET - GAUGE_BACK_OFFSET );
		m_pGaugeBack->Render( nXPos + GAUGE_X_OFFSET, nYPos + GAUGE_Y_OFFSET );
		m_pNewGauge->RenderWidthCut( nXPos + GAUGE_X_OFFSET, nYPos + GAUGE_Y_OFFSET, 0, nNewGaugeWidth );
		m_pGauge->RenderWidthCut( nXPos + GAUGE_X_OFFSET, nYPos + GAUGE_Y_OFFSET, 0, nPreGaugeWidth );

		//Exp
		RenderExpNum( nXPos, nYPos, m_fNewExpRate );
	}
	else
	{
		kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter.AddTextPiece( FONT_SIZE_12, STR(1) );	
		kPrinter.PrintFullText( nXPos + MANUAL_X, nYPos + MANUAL_Y, TAT_CENTER );
		kPrinter.ClearList();
	}	
}

void PetWnd::RenderExpNum( int nXPos, int nYPos, float fExpRate )
{
	if ( !m_pExpText || !m_pExpNumText || !m_pPerText )
		return;

	enum
	{
		EXP_X_OFFSET  = 630,
		EXP_Y_OFFSET  = 305,
		EXP_TEXT_SIZE = 35,
		EXP_NUM_SIZE = 12,
		PER_SIZE	 = 12,
		NUM_GAP		 = 4,
	};

	int nExpRate = (int)(fExpRate * FLOAT100);
	int nCurXPos = 0;
	int nCurYPos = nYPos + EXP_Y_OFFSET;

	int nTotalSize = 0;
	int nNumOffset = 0;
	if( nExpRate < 10 )
	{
		nNumOffset = (EXP_NUM_SIZE - NUM_GAP);
		nTotalSize = EXP_TEXT_SIZE + PER_SIZE + nNumOffset;
	}
	else if( nExpRate < 100 )
	{
		nNumOffset = (EXP_NUM_SIZE - NUM_GAP)*2;
		nTotalSize = EXP_TEXT_SIZE + PER_SIZE + nNumOffset;
	}
	else
	{
		nNumOffset = (EXP_NUM_SIZE - NUM_GAP)*3;
		nTotalSize = EXP_TEXT_SIZE + PER_SIZE + nNumOffset;
	}

	nCurXPos = nXPos - (nTotalSize / 2) + EXP_X_OFFSET;

	m_pExpText->Render( nCurXPos, nCurYPos, UI_RENDER_ADD );

	nCurXPos += EXP_TEXT_SIZE;
	m_pExpNumText->RenderNum( nCurXPos, nCurYPos, nExpRate, 0.0f, FLOAT1, UI_RENDER_ADD );

	nCurXPos += nNumOffset;
	m_pPerText->Render( nCurXPos, nCurYPos, UI_RENDER_ADD );
}

void PetWnd::RenderInfo( int nXPos, int nYPos )
{
	enum 
	{		
		NAME_OFFSET_X = 629,
		NAME_OFFSET_Y = 268,	

		LINE_OFFSET_X = 529,
		LINE_OFFSET_Y = 271,

		RANK_OFFSET_X = 627,
		RANK_OFFSET_Y = 281,
	};

	if ( m_nCurPetCode <= 0 )
		return;

	if ( !m_pNameLine )
		return;

	m_pNameLine->Render( nXPos+LINE_OFFSET_X, nYPos+LINE_OFFSET_Y );

	ioHashString szRank = g_PetInfoMgr.GetRankName( m_nCurRank );

	if ( m_szPetName.IsEmpty() || szRank.IsEmpty() )
		return;

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	//레벨
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );		
	kPrinter.AddTextPiece( FONT_SIZE_12, STR(1) );

	if ( m_nCurPetLv < m_nMaxPetLv )
	{
		kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );	
		kPrinter.AddTextPiece( FONT_SIZE_12, STR(2), m_nCurPetLv );	
	}
	else
	{
		kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );	
		kPrinter.AddTextPiece( FONT_SIZE_12, STR(2), m_nCurPetLv );	
		kPrinter.SetTextColor( TCT_DEFAULT_RED );
		kPrinter.AddTextPiece( FONT_SIZE_12, STR(3) );	
	}	

	//펫 이름
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );		
	kPrinter.AddTextPiece( FONT_SIZE_12, STR(4), m_szPetName.c_str() );	
	kPrinter.PrintFullText( nXPos + NAME_OFFSET_X, nYPos + NAME_OFFSET_Y, TAT_CENTER );
	kPrinter.ClearList();
	//펫 현재 랭크
	kPrinter.SetTextColor( TCT_DEFAULT_BLUE );		
	kPrinter.AddTextPiece( FONT_SIZE_12, STR(5), szRank.c_str() );	
	kPrinter.PrintFullText( nXPos + RANK_OFFSET_X, nYPos + RANK_OFFSET_Y, TAT_CENTER );
	kPrinter.ClearList();
}

void PetWnd::RenderStat( int nXPos, int nYPos  )
{
	if ( m_nCurPetCode <= 0 )
		return;

	if( !m_pGrowthBackGray || !m_pGrowthBackLight )
		return;

	enum 
	{
		MAX_BACK = 4,

		STAT_OFFSET_X = 526,
		STAT_OFFSET_Y = 327,

		BACK_WIDTH = 101,
		BACK_HEIGHT = 23,

		LEFT_BACK_XOFFSET    = 0,
		RIGHT_BACK_XOFFSET   = 103,
		BACK_YOFFSET         = 0,
		YGAP                 = 23,

		LEFT_TEXT_XOFFSET    = 5,
		RIGHT_TEXT_XOFFSET   = 108,
		TEXT_YOFFSET = 5,
		
		GROWTH_XOFFSET = 95,
	};

	// back
	POINT ptGrayOffset[MAX_BACK]={ LEFT_BACK_XOFFSET, BACK_YOFFSET, RIGHT_BACK_XOFFSET, BACK_YOFFSET, 
		LEFT_BACK_XOFFSET, BACK_YOFFSET+(YGAP*2), RIGHT_BACK_XOFFSET, BACK_YOFFSET+(YGAP*2) };
	POINT ptLightOffset[MAX_BACK]={ LEFT_BACK_XOFFSET, BACK_YOFFSET+YGAP, RIGHT_BACK_XOFFSET, BACK_YOFFSET+YGAP, 
		LEFT_BACK_XOFFSET, BACK_YOFFSET+(YGAP*3), RIGHT_BACK_XOFFSET, BACK_YOFFSET+(YGAP*3) };

	for ( int i=0; i<MAX_BACK; i++ )
	{
		m_pGrowthBackGray->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
		m_pGrowthBackGray->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
		m_pGrowthBackGray->SetSize( BACK_WIDTH, BACK_HEIGHT );
		m_pGrowthBackGray->Render( nXPos + ptGrayOffset[i].x+STAT_OFFSET_X, nYPos + ptGrayOffset[i].y+STAT_OFFSET_Y, UI_RENDER_MULTIPLY );

		m_pGrowthBackLight->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
		m_pGrowthBackLight->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
		m_pGrowthBackLight->SetSize( BACK_WIDTH, BACK_HEIGHT );
		m_pGrowthBackLight->Render( nXPos + ptLightOffset[i].x+STAT_OFFSET_X, nYPos + ptLightOffset[i].y+STAT_OFFSET_Y, UI_RENDER_MULTIPLY );
	}	

	for ( int i=0; i<MAX_CHAR_GROWTH+MAX_ITEM_GROWTH; i++ )
	{	
		ioComplexStringPrinter kPrinter;
		kPrinter.SetTextStyle( TS_NORMAL);
		kPrinter.SetBkColor( 0, 0, 0 );	
		if( m_nGrowthStat[i] == 0 )
		{
			kPrinter.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
			kPrinter.AddTextPiece( FONT_SIZE_12, STR(i+1) );
		}
		else
		{
			kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
			kPrinter.AddTextPiece( FONT_SIZE_12, STR(i+1) );
		}		

		if ( i < MAX_ITEM_GROWTH )
			kPrinter.PrintFullText( nXPos+RIGHT_TEXT_XOFFSET+STAT_OFFSET_X, nYPos+TEXT_YOFFSET+(YGAP*i)+STAT_OFFSET_Y, TAT_LEFT );
		else
			kPrinter.PrintFullText( nXPos+LEFT_TEXT_XOFFSET+STAT_OFFSET_X, nYPos+TEXT_YOFFSET+(YGAP*(i-MAX_ITEM_GROWTH))+STAT_OFFSET_Y, TAT_LEFT );
		kPrinter.ClearList();

		if( m_nGrowthStat[i] != 0 )
		{
			kPrinter.SetTextColor( TCT_DEFAULT_GREEN );
			if( m_nGrowthStat[i] < 100 )
				kPrinter.AddTextPiece( FONT_SIZE_12, STR(9), m_nGrowthStat[i] );
			else
				kPrinter.AddTextPiece( FONT_SIZE_12, STR(10), m_nGrowthStat[i] );
			if ( i < MAX_ITEM_GROWTH )
				kPrinter.PrintFullText( nXPos+GROWTH_XOFFSET+RIGHT_BACK_XOFFSET+STAT_OFFSET_X, nYPos+TEXT_YOFFSET+(YGAP*i)+STAT_OFFSET_Y, TAT_RIGHT );
			else
				kPrinter.PrintFullText( nXPos+GROWTH_XOFFSET+STAT_OFFSET_X, nYPos+TEXT_YOFFSET+(YGAP*(i-MAX_ITEM_GROWTH))+STAT_OFFSET_Y, TAT_RIGHT );	
			kPrinter.ClearList();
		}
	}
}

void PetWnd::RenderTitle( int nXPos, int nYPos  )
{
	enum 
	{ 
		X_OFFSET = 517, 
		Y_OFFSET = 14, 
	};

	char szPeso[MAX_PATH]="";
	char szCash[MAX_PATH]="";
	Help::ConvertNumToStrComma( g_MyInfo.GetMoney(), szPeso, sizeof( szPeso ) );
	Help::ConvertNumToStrComma( g_MyInfo.GetTotalCash(), szCash, sizeof( szCash ) );

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_OUTLINE_FULL_2X );
	kPrinter.SetBkColor( 12, 66, 111 );
	kPrinter.SetTextColor( 130, 198, 255 );		
	kPrinter.AddTextPiece( FONT_SIZE_12, STR(1) );
	kPrinter.AddTextPiece( FONT_SIZE_12, szPeso );
	kPrinter.SetTextColor( TCT_DEFAULT_LIGHTGRAY );		
	kPrinter.AddTextPiece( FONT_SIZE_12, " / " );
	kPrinter.SetTextColor( 255, 188, 26 );		
	kPrinter.AddTextPiece( FONT_SIZE_12, STR(2) );
	kPrinter.AddTextPiece( FONT_SIZE_12, szCash );
	kPrinter.PrintFullText( nXPos + X_OFFSET, nYPos + Y_OFFSET, TAT_RIGHT );
	kPrinter.ClearList();
}

//활성화
void PetWnd::SetActivePet()
{
	if ( m_nSelectSlotIndex <= 0 )
		return;		
	
	m_nCurEquipPetCode = 0;
	RefreshPetIcon( false );

	if ( m_nCurEquipPetCode > 0 )
	{
		HideChildWnd( ID_ACTIVE );
		ShowChildWnd( ID_INACTIVE );

		SetChildWndStyleAdd( ID_REMOVE, IWS_EXACTIVE );
		SetChildTitleOffset( ID_REMOVE, false );

		if ( m_nCurPetLv >= m_nMaxPetLv )
		{
			SetChildWndStyleAdd( ID_EAT, IWS_EXACTIVE );
			SetChildTitleOffset( ID_EAT, false );
		}
		else
		{
			SetChildWndStyleRemove( ID_EAT, IWS_EXACTIVE );
			SetChildTitleOffset( ID_EAT, true );
		}
	}

	SetChildWndStyleAdd( ID_COMPOUND, IWS_EXACTIVE );
	SetChildTitleOffset( ID_COMPOUND, false );
}

//비활성화
void PetWnd::SetInActivePet()
{
	if ( m_nSelectSlotIndex <= 0 )
		return;		

	m_nCurEquipPetCode = 0;
	SetSelectPetSlot( m_nSelectSlotIndex );

	RefreshPetIcon( false );
	SelectSlot( false );

	ShowChildWnd( ID_ACTIVE );
	HideChildWnd( ID_INACTIVE );

	if ( m_nCurPetLv >= m_nMaxPetLv )
	{
		SetChildWndStyleAdd( ID_EAT, IWS_EXACTIVE );
		SetChildTitleOffset( ID_EAT, false );

		SetChildWndStyleRemove( ID_COMPOUND, IWS_EXACTIVE );
		SetChildTitleOffset( ID_COMPOUND, true );
	}
	else
	{
		SetChildWndStyleRemove( ID_EAT, IWS_EXACTIVE );
		SetChildTitleOffset( ID_EAT, true );

		SetChildWndStyleAdd( ID_COMPOUND, IWS_EXACTIVE );
		SetChildTitleOffset( ID_COMPOUND, false );
	}

	SetChildWndStyleRemove( ID_REMOVE, IWS_EXACTIVE );
	SetChildTitleOffset( ID_REMOVE, true );
}

//제거
void PetWnd::SetRemovePet( int nPetCode, int nPetRank, int nPetLevel )
{
	if ( m_nSelectSlotIndex == 0 )
		return;

	RefreshPetIcon( true );
	Init();
	HideMainMenu();
	HideChildWnd( ID_PETVIEW );

	PetResellResultWnd *pPetResellResultWnd = dynamic_cast<PetResellResultWnd*>(FindChildWnd( PET_RESELL_RESULT_WND ));
	if ( pPetResellResultWnd )
		pPetResellResultWnd->SetResellResult( nPetCode, nPetRank, nPetLevel );
}

//먹이주기
void PetWnd::SetEatPet()
{
	if ( m_nSelectSlotIndex <= 0 )
		return;
		
	HideMainMenu();

	PetEatWnd *pPetEatWnd = dynamic_cast<PetEatWnd*>(( FindChildWnd( PET_EAT_WND ) ));
	if ( pPetEatWnd )
		pPetEatWnd->SelectPetSlot( m_nSelectSlotIndex );
}

//합성하기
void PetWnd::SetCompoundPet()
{
	if ( m_nSelectSlotIndex <= 0 )
		return;

	HideMainMenu();

	PetCompoundWnd *pPetCompoundWnd = dynamic_cast<PetCompoundWnd*>(( FindChildWnd( PET_COMPOUND_WND ) ));
	if ( pPetCompoundWnd )
		pPetCompoundWnd->SelectPetSlot( m_nSelectSlotIndex );
}

//플레이스테이지 설정
void PetWnd::SetPlayStage( ioPlayStage *pPlayStage )
{
	m_pPlayStage = pPlayStage;
}

//펫 슬롯 선택 설정
void PetWnd::SetSelectPetSlot( int nSlotIndex )
{
	if ( nSlotIndex <= 0 )
		return;

	m_nSetSelectIndex = nSlotIndex;
}

//초기화
void PetWnd::Init()
{	
	m_nSelectSlotIndex = 0;
	m_nCurEquipPetCode = 0;
	m_nSetSelectIndex = 0;

	m_nCurPetCode = 0;
	m_nCurPetLv = 0;
	m_nMaxPetLv = 0;
	m_nCurRank = 0;
	m_nCurPetExp = 0;
	m_fSelectPetLevelRate = 0.0f;
	m_szPetName.Clear();

	for ( int i=0; i<MAX_CHAR_GROWTH+MAX_ITEM_GROWTH; i++ )
		m_nGrowthStat[i] = 0;

	m_bEatResultExp = false;
	
	m_nMaxExp = 0;
	m_fNewExpRate = 0.0f;
	m_fPreExpRate = 0.0f;
	m_dwEquipWndID = 0;
	m_dwExpGaugeUpTime = 0;
}

//아이콘 갱신
void PetWnd::RefreshPetIcon( bool bSelect )
{
	ioUserPet *pUserPet = g_MyInfo.GetUserPet();
	if ( !pUserPet )
		return;

	pUserPet->SlotSort();

	for ( int i=0; i<ioUserPet::MAX_SLOT_CNT; i++ )
	{
		PetIconBtn *pPetIcon = dynamic_cast<PetIconBtn*>( FindChildWnd( i+ID_PETSLOT1 ));
		if ( pPetIcon )
		{
			pPetIcon->RemoveIcon();

			PetSlot sPetSlot;
			if ( pUserPet->GetPetSlotArray( i, sPetSlot ) )
			{
				pPetIcon->SetSlotIndex( sPetSlot.m_nIndex );
				pPetIcon->SetCurExp( sPetSlot.m_CurExp );
				pPetIcon->SetEquip( sPetSlot.m_bEquip );
				pPetIcon->SetIcon( sPetSlot.m_nPetCode, sPetSlot.m_nCurLevel, sPetSlot.m_PetRankType, FLOAT08, FLOAT08, true );

				if ( sPetSlot.m_bEquip )
				{
					pPetIcon->SetSelect( true );
					m_nCurEquipPetCode = sPetSlot.m_nPetCode;
					m_nCurRank = sPetSlot.m_PetRankType;
					m_dwEquipWndID = i+ID_PETSLOT1;
				}
			}
		}		
	}
	
	//선택 펫이 있다면
	if ( bSelect && m_nSetSelectIndex > 0 )
	{
		if ( m_dwEquipWndID > 0 )
		{
			PetIconBtn *pPetIcon = dynamic_cast<PetIconBtn*>( FindChildWnd( m_dwEquipWndID ));
			if ( pPetIcon )
			{
				if ( m_nSetSelectIndex != pPetIcon->GetSlotIndex() )
					pPetIcon->SetSelect( false );
			}
		}
	}
}

//펫 선택
void PetWnd::SelectPet( DWORD dwID )
{
	//아이콘 선택을 확인 (펫코드가 없으면 종료)
	PetIconBtn *pPetIcon = dynamic_cast<PetIconBtn*>( FindChildWnd( dwID ));
	if ( pPetIcon )
	{
		int nPetCode = pPetIcon->GetPetCode();
		int nPetRank = pPetIcon->GetPetRank();
		int nSelectSlotIndex = pPetIcon->GetSlotIndex();

		if ( nPetCode <= 0 || nPetRank ==0 || nSelectSlotIndex <= 0 )
			return;

		pPetIcon->SetSelect( true );
		m_nSelectSlotIndex = nSelectSlotIndex;
	}

	//선택 해제
	for ( int i=0; i<ioUserPet::MAX_SLOT_CNT; i++ )
	{
		if ( dwID == i+ID_PETSLOT1 )
			continue;

		PetIconBtn *pPetIcon = dynamic_cast<PetIconBtn*>( FindChildWnd( i+ID_PETSLOT1 ));
		if ( pPetIcon )
			pPetIcon->SetSelect( false );
	}

	m_bEatResultExp = false;
	ShowMainMenu();
	HideChildWnd( PET_EAT_WND );
	HideChildWnd( PET_COMPOUND_WND );
	HideChildWnd( PET_RESELL_RESULT_WND );

	SetPetInfo( true );
	SetPetView( m_nCurPetCode, m_nCurRank );

	g_GUIMgr.HideWnd( PET_TOOLTIP );
}

//첫 번째 슬롯 선택
void PetWnd::SelectFirstSlot()
{
	//첫번째 슬롯 강제 선택
	PetIconBtn *pPetIcon = dynamic_cast<PetIconBtn*>( FindChildWnd( ID_PETSLOT1 ));
	if ( pPetIcon )
	{
		int nPetCode = pPetIcon->GetPetCode();
		int nPetRank = pPetIcon->GetPetRank();
		int nSelectSlotIndex = pPetIcon->GetSlotIndex();

		if ( nPetCode <= 0 || nPetRank ==0 || nSelectSlotIndex <= 0 )
			return;

		pPetIcon->SetSelect( true );
		m_nSelectSlotIndex = nSelectSlotIndex;

		PetViewWnd *pPetViewWnd = dynamic_cast<PetViewWnd*>( FindChildWnd( ID_PETVIEW ));
		if( pPetViewWnd )
		{
			ioHashString szPetViewAni = g_PetInfoMgr.GetPetViewAni( nPetCode, (PetRankType)nPetRank );
			pPetViewWnd->CreatePet( nPetCode, nPetRank );
			pPetViewWnd->SetLoopAni( szPetViewAni );
			pPetViewWnd->SetPetScale( nPetCode, nPetRank );
		}
	}
}

//슬롯 선택
void PetWnd::SelectSlot( bool bInitAni )
{
	if ( m_nSetSelectIndex <= 0 )
		return;

	for ( int i=0; i<ioUserPet::MAX_SLOT_CNT; i++ )
	{
		PetIconBtn *pPetIcon = dynamic_cast<PetIconBtn*>( FindChildWnd( i+ID_PETSLOT1 ));
		if ( pPetIcon )
		{
			int nPetCode = pPetIcon->GetPetCode();
			int nPetRank = pPetIcon->GetPetRank();
			int nSelectSlotIndex = pPetIcon->GetSlotIndex();

			if ( nPetCode <= 0 || nPetRank <= 0 || nSelectSlotIndex <= 0 )
				continue;
			
			if ( m_nSetSelectIndex == nSelectSlotIndex )
			{
				pPetIcon->SetSelect( true );
				m_nSelectSlotIndex = nSelectSlotIndex;

				if ( bInitAni )
				{
					PetViewWnd *pPetViewWnd = dynamic_cast<PetViewWnd*>( FindChildWnd( ID_PETVIEW ));
					if( pPetViewWnd )
					{
						ioHashString szPetViewAni = g_PetInfoMgr.GetPetViewAni( nPetCode, (PetRankType)nPetRank );
						pPetViewWnd->CreatePet( nPetCode, nPetRank );
						pPetViewWnd->SetLoopAni( szPetViewAni );
						pPetViewWnd->SetPetScale( nPetCode, nPetRank );
					}
				}
				return;
			}
		}
	}
}

//펫 뷰 설정
void PetWnd::SetPetView( int nPetCode, int nPetRank )
{
	//현재 장착 펫 얻어서 설정
	PetViewWnd *pPetViewWnd = dynamic_cast<PetViewWnd*>( FindChildWnd( ID_PETVIEW ));
	if( pPetViewWnd )
	{
		if ( nPetCode <= 0 || nPetRank <= 0 )
		{
			pPetViewWnd->DestroyPet();
			return;
		}

		ioHashString szPetViewAni = g_PetInfoMgr.GetPetViewAni( nPetCode, (PetRankType)nPetRank );
		pPetViewWnd->CreatePet( nPetCode, nPetRank );
		pPetViewWnd->SetLoopAni( szPetViewAni );
		pPetViewWnd->SetPetScale( nPetCode, nPetRank );
		pPetViewWnd->ShowWnd();
	}
}

//펫 정보 갱신
void PetWnd::SetPetInfo( bool bSelect )
{
	ioUserPet *pUserPet = g_MyInfo.GetUserPet();
	if ( !pUserPet )
		return;

	PetSlot sPetSlot;

	if ( bSelect )
	{
		//슬롯 번호로 데이터 얻기
		if ( !pUserPet->GetPetSlot( m_nSelectSlotIndex, sPetSlot ) )
			return;	
	}
	else
	{
		//부화 펫이 있다면
		if ( m_nSetSelectIndex > 0 )
		{
			//슬롯 번호로 데이터 얻기
			if ( !pUserPet->GetPetSlot( m_nSelectSlotIndex, sPetSlot ) )
				return;
		}
		else
		{
			//장착 중인 펫이 없다면
			if ( m_nCurEquipPetCode == 0 )
			{
				//슬롯 번호로 데이터 얻기
				if ( !pUserPet->GetPetSlot( m_nSelectSlotIndex, sPetSlot ) )
					return;
			}
			else
			{
				//장착 중이면 장착 펫 데이터 얻기
				if ( !pUserPet->GetPetSlotEquip( sPetSlot ) )
					return;		
			}
		}
	}

	m_nCurPetCode = sPetSlot.m_nPetCode;
	m_nCurPetLv = sPetSlot.m_nCurLevel;
	m_nCurPetExp = sPetSlot.m_CurExp;
	m_nCurRank = sPetSlot.m_PetRankType;
	m_szPetName = sPetSlot.m_Name;

	m_nMaxPetLv = g_PetInfoMgr.GetMaxLevel( (PetRankType)m_nCurRank );

	int m_nMaxPetExp = g_PetInfoMgr.GetMaxExp( m_nCurPetLv, (PetRankType)m_nCurRank );

	//경험치 퍼센트 설정
	if ( m_nMaxPetExp > 0 )
		m_fSelectPetLevelRate = (float)m_nCurPetExp/m_nMaxPetExp;
	else
		m_fSelectPetLevelRate = 0.0f;

	m_fSelectPetLevelRate = min( FLOAT1, m_fSelectPetLevelRate );

	for ( int i=0; i<MAX_CHAR_GROWTH+MAX_ITEM_GROWTH; i++ )
		m_nGrowthStat[i] = g_PetInfoMgr.GetTotalStat( m_nCurPetCode, i, m_nCurPetLv );

	if ( sPetSlot.m_bEquip )
	{
		HideChildWnd( ID_ACTIVE );
		ShowChildWnd( ID_INACTIVE );

		SetChildWndStyleAdd( ID_REMOVE, IWS_EXACTIVE );
		SetChildTitleOffset( ID_REMOVE, false );

		m_nSelectSlotIndex = sPetSlot.m_nIndex;
	}
	else
	{
		ShowChildWnd( ID_ACTIVE );
		SetChildActive( ID_ACTIVE );
		HideChildWnd( ID_INACTIVE );
		SetChildWndStyleRemove( ID_REMOVE, IWS_EXACTIVE );
		SetChildTitleOffset( ID_REMOVE, true );
	}

	if ( m_nCurPetLv >= m_nMaxPetLv )
	{
		SetChildWndStyleAdd( ID_EAT, IWS_EXACTIVE );
		SetChildTitleOffset( ID_EAT, false );

		if ( !sPetSlot.m_bEquip )
		{
			SetChildWndStyleRemove( ID_COMPOUND, IWS_EXACTIVE );
			SetChildTitleOffset( ID_COMPOUND, true );
		}
		else
		{
			SetChildWndStyleAdd( ID_COMPOUND, IWS_EXACTIVE );
			SetChildTitleOffset( ID_COMPOUND, false );
		}
	}
	else
	{
		SetChildWndStyleRemove( ID_EAT, IWS_EXACTIVE );
		SetChildTitleOffset( ID_EAT, true );

		SetChildWndStyleAdd( ID_COMPOUND, IWS_EXACTIVE );
		SetChildTitleOffset( ID_COMPOUND, false );
	}
}

//리셋 윈도우
void PetWnd::ResetPetWnd( bool bInitAni )
{
	//슬롯이 비어 있으면 숨김
	if ( !CheckSlot() )
		return;

	RefreshPetIcon( true );

	if ( m_nSetSelectIndex <= 0 )
	{
		//장착 중인 펫이 없다면
		if ( m_nCurEquipPetCode == 0 )
			SelectFirstSlot();
		else
			//장착 펫 설정
			SetPetView( m_nCurEquipPetCode, m_nCurRank );
	}
	else
		SelectSlot( bInitAni );

	//펫 정보 설정
	SetPetInfo( false );
}

//슬롯 확인
bool PetWnd::CheckSlot()
{
	//슬롯이 비어 있으면 숨김
	ioUserPet *pUserPet = g_MyInfo.GetUserPet();
	if( pUserPet )
	{
		if ( pUserPet->GetPetSlotCnt() <= 0 )
		{
			HideWnd();
			g_GUIMgr.ShowWnd( PET_EMPTY_WND );
			return false;
		}		
	}
	return true;
}

//먹이주기 결과 경험치 설정
void PetWnd::SetEatResultExp( int nPreLevel, int nNewLevel, int nPetRank, int nPreExp, int nCurExp )
{
	//최대 경험치 얻기
	m_nMaxExp = g_PetInfoMgr.GetMaxExp( nNewLevel, (PetRankType)nPetRank );

	if ( m_nMaxExp > 0 )
	{
		m_fNewExpRate = ( (float)nCurExp / m_nMaxExp );
		m_fPreExpRate = ( (float)nPreExp / m_nMaxExp );
	}

	m_fNewExpRate = min( FLOAT1, m_fNewExpRate );

	if ( nPreLevel != nNewLevel )
		m_fPreExpRate = 0.0f;

	m_fPreExpRate = min( FLOAT1, m_fPreExpRate );

	m_bEatResultExp = true;

	m_dwExpGaugeUpTime = FRAMEGETTIME();
}

void PetWnd::ExpGaugeUp()
{
	if ( !m_bEatResultExp || m_fPreExpRate == m_fNewExpRate || m_dwExpGaugeUpTime == 0 )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwExpGaugeUpTime + m_dwCheckExpGaugeUpTime > dwCurTime )
		return;

	if ( m_fPreExpRate < m_fNewExpRate )
		m_fPreExpRate += ( m_fExpGaugeSpeed * g_FrameTimer.GetSecPerFrame() );
	else
		m_fPreExpRate = m_fNewExpRate;
}

//먹이주기 결과 설정
void PetWnd::OnPetEatResultInfo( int nPetCode, int nPetRank, int nPreLevel, int nNewLevel, int nPreExp, int nCurExp )
{
	//애니메이션 설정
	PetViewWnd *pPetViewWnd = dynamic_cast<PetViewWnd*>(FindChildWnd( ID_PETVIEW ));
	if ( pPetViewWnd && pPetViewWnd->IsShow() )
	{
		if ( nPreLevel != nNewLevel )
		{
			pPetViewWnd->DummyAttachEffect( m_szLevelUpEffect );

			g_SoundMgr.PlaySound( m_szLevelUpSound, DSBVOLUME_MAX, PSM_THREAD );
		}
		else
		{
			pPetViewWnd->DummyAttachEffect( m_szExpUpEffect );

			g_SoundMgr.PlaySound( m_szExpUpSound, DSBVOLUME_MAX, PSM_THREAD );
		}
	}

	SetEatResultExp( nPreLevel, nNewLevel, nPetRank, nPreExp, nCurExp );	

	if ( nNewLevel >= m_nMaxPetLv )
	{
		//최대 레벨이 되었다면 먹이주기 불가
		PetEatWnd *pPetEatWnd = dynamic_cast<PetEatWnd*>( FindChildWnd( PET_EAT_WND ) );
		if ( pPetEatWnd && pPetEatWnd->IsShow() )
			pPetEatWnd->ApplyMaxLv();

		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
	}
}

//먹이주기 결과 적용
void PetWnd::OnPetEatResultApply( int nPetIndex )
{
	SetSelectPetSlot( nPetIndex );
	RefreshPetIcon( true );
	SelectSlot( false );
	SetPetInfo( true );
	HideMainMenu();
}

//합성 결과 설정
void PetWnd::OnPetCompoundResultInfo( int nPetCode, int nPetRank, bool bResultType )
{
	//애니메이션 설정
	PetViewWnd *pPetViewWnd = dynamic_cast<PetViewWnd*>(FindChildWnd( ID_PETVIEW ));
	if ( pPetViewWnd && pPetViewWnd->IsShow() )
	{
		pPetViewWnd->CreatePet( nPetCode, nPetRank );
		pPetViewWnd->SetPetScale( nPetCode, nPetRank );

		if ( bResultType )
		{
			//랭크업
			ioHashString szAni = g_PetInfoMgr.GetPetViewAni( nPetCode, (PetRankType)nPetRank );
			if ( !szAni.IsEmpty() )
				pPetViewWnd->SetLoopAni( szAni );

			pPetViewWnd->DummyAttachEffect( m_szRankUpEffect );

			if( g_ExSoundMgr.IsSound( ExSound::EST_PET_MIX_SUCESS ) )
			{
				g_ExSoundMgr.PlaySound( ExSound::EST_PET_MIX_SUCESS );
			}
			else
			{
				g_SoundMgr.PlaySound( m_szRankUpSound, DSBVOLUME_MAX, PSM_THREAD );
			}
		}
		else
		{
			//새로운 펫
			ioHashString szAni = g_PetInfoMgr.GetPetViewAni( nPetCode, (PetRankType)nPetRank );
			if ( !szAni.IsEmpty() )
				pPetViewWnd->SetLoopAni( szAni );

			pPetViewWnd->DummyAttachEffect( m_szCompoundEffect );

			g_SoundMgr.PlaySound( m_szCompoundSound, DSBVOLUME_MAX, PSM_THREAD );
		}
	}

	//UI 변경
	PetCompoundWnd *pPetCompoundWnd = dynamic_cast<PetCompoundWnd*>( FindChildWnd( PET_COMPOUND_WND ) );
	if ( pPetCompoundWnd && pPetCompoundWnd->IsShow() )
		pPetCompoundWnd->ApplyCompoundResult();
}

//합성 결과 적용
void PetWnd::OnPetCompoundResultApply( int nPetIndex )
{
	SetSelectPetSlot( nPetIndex );
	RefreshPetIcon( true );
	SelectSlot( false );
	SetPetInfo( true );
	HideMainMenu();
}

//알 결과 적용
void PetWnd::OnPetAddResultApply( int nPetIndex )
{
	HideChildWnd( PET_COMPOUND_WND );
	HideChildWnd( PET_EAT_WND );
	HideChildWnd( PET_RESELL_RESULT_WND );

	ShowMainMenu();
	SetSelectPetSlot( nPetIndex );
	RefreshPetIcon( true );
	SelectSlot( true );
	SetPetInfo( true );
}

//UI 재설정
void PetWnd::ResetMainMenu()
{
	if ( m_nSelectSlotIndex <= 0 )
		return;

	m_nSetSelectIndex = m_nSelectSlotIndex;

	ShowMainMenu();
	ResetPetWnd( false );

	m_fNewExpRate = 0.0f;
	m_fPreExpRate = 0.0f;
	m_nMaxExp = 0;
	m_bEatResultExp = false;
}

//UI 설정
void PetWnd::HideMainMenu()
{
	HideChildWnd( ID_ACTIVE );
	HideChildWnd( ID_INACTIVE );
	HideChildWnd( ID_EAT );
	HideChildWnd( ID_COMPOUND );
	HideChildWnd( ID_REMOVE );
}

void PetWnd::ShowMainMenu()
{
	ShowChildWnd( ID_ACTIVE );
	ShowChildWnd( ID_INACTIVE );
	ShowChildWnd( ID_EAT );
	ShowChildWnd( ID_COMPOUND );
	ShowChildWnd( ID_REMOVE );
}

void PetWnd::SetChildTitleOffset( DWORD dwWndID, bool bActive )
{
	enum
	{
		Title_Active_X = 33,
		Title_Active_Y = 2,

		Title_InActive_X = 31,
		Title_InActive_Y = 1,
	};

	ioWnd *pFindBtn = dynamic_cast<ioWnd*>(FindChildWnd( dwWndID ));
	if ( pFindBtn )
	{
		if ( bActive )
		{
			pFindBtn->SetTitleOffsetX( Title_Active_X );
			pFindBtn->SetTitleOffsetY( Title_Active_Y );
		}
		else
		{
			pFindBtn->SetTitleOffsetX( Title_InActive_X );
			pFindBtn->SetTitleOffsetY( Title_InActive_Y );
		}
	}
}