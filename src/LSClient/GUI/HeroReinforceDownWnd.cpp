#include "StdAfx.h"
#include "HeroReinforceDownWnd.h"

#include "GradeAdjustmentWnd.h"
#include "ReinforceSelectWnd.h"
#include "../ioPowerUpManager.h"
#include "../ioPlayStage.h"

HeroReinforceDownWnd::HeroReinforceDownWnd( void )
{
	m_pDarkTextBack = NULL;
	m_pLightTextBack = NULL;
	m_pSoldierInfoBG = NULL;
	m_CharInfo.Init();
}

HeroReinforceDownWnd::~HeroReinforceDownWnd( void )
{
	SAFEDELETE( m_pDarkTextBack );
	SAFEDELETE( m_pLightTextBack );
	SAFEDELETE( m_pSoldierInfoBG );
	m_CharInfo.Init();
}

void HeroReinforceDownWnd::iwm_show()
{
	ShowChildWnd( HeroReinforceDownWnd::ID_SOLDIER_WND );
}

void HeroReinforceDownWnd::iwm_hide()
{
	HideChildWnd( HeroReinforceDownWnd::ID_SOLDIER_WND );
}

void HeroReinforceDownWnd::ClearList()
{
	m_SoldierInfo.Clear();
	m_SoldierReinforceInfo.Release();
	m_SoulStoneCountText.Release();
	m_SoldierSelectPrinter.ClearList();
}

void HeroReinforceDownWnd::SetSoldierInfo( int iClassType )
{
	if ( g_MyInfo.GetClassArray( iClassType ) == -1 )
	{
		LOG.PrintTimeAndLog( 0, "HeroReinforceDownWnd::SetSoldierInfo - iClassType is not exist" );
		return;
	}

	m_CharInfo.Init();
	m_CharInfo = g_MyInfo.GetCharacterToClassType( iClassType );
	NewShopHelpWnd *pCharWnd = dynamic_cast<NewShopHelpWnd *>( FindChildWnd( HeroReinforceDownWnd::ID_SOLDIER_WND ) );
	if( pCharWnd )
	{
		pCharWnd->SetZPosition( FLOAT500 );
		pCharWnd->SetChar( m_CharInfo );
		pCharWnd->SetEquip( false, false, false );
		pCharWnd->SetUserInfoToolTip( true );
		pCharWnd->SetCharScale( 0.85f );
	}
	
	int iSoulStoneCount = 0;
	ioUserEtcItem *pUserEtcItem = g_MyInfo.GetUserEtcItem();
	if( pUserEtcItem )
	{
		ioUserEtcItem::ETCITEMSLOT kSlot;
		if( pUserEtcItem->GetEtcItem( ioEtcItem::EIT_ETC_SOUL_STONE, kSlot ) )
			iSoulStoneCount = kSlot.m_iValue1;
	}

	ClearList();	
	char szText[MAX_PATH];
	if( m_CharInfo.m_eCharPowerUpType != PUGT_NONE )
	{
		ioHashString szGradeTitle = g_PowerUpManager.GetCharGradeTitle( m_CharInfo.m_class_type, m_CharInfo.m_eCharPowerUpType );
		if ( szGradeTitle.IsEmpty() )
			return;

		SafeSprintf( szText, sizeof( szText ), STR(4), szGradeTitle.c_str() );
		m_SoldierInfo = szText;
	}
	
	m_SoldierReinforceInfo.Set( 0, 0, g_PowerUpManager.GetCharGardeString( m_CharInfo.m_class_type ).c_str(), FONT_SIZE_12, 194, TS_NORMAL, TCT_DEFAULT_DARKGRAY, 0, TAT_LEFT, TVA_TOP, 8);

	wsprintf( szText, m_szSoulStoneText.c_str(), iSoulStoneCount );
	m_SoulStoneCountText.Set( 0, 0, szText, FONT_SIZE_11, 200, TS_NORMAL, 0xFF6A6869, 0, TAT_LEFT, TVA_TOP, 3);
	
	const ioHashString& szClassName = g_MyInfo.GetClassName( m_CharInfo.m_class_type ) ;
	SafeSprintf( szText, sizeof( szText ), "Lv%d %s", g_MyInfo.GetClassLevel( m_CharInfo.m_class_type, false ), szClassName.c_str() );

	m_SoldierSelectPrinter.SetTextStyle( TS_NORMAL );
	m_SoldierSelectPrinter.SetBkColor( 0, 0, 0 );
	m_SoldierSelectPrinter.SetTextColor( g_UIImageRenderMgr.GetGradeTextColor( g_UIImageRenderMgr.GetPowerUpGradeType( iClassType ) ) );
	m_SoldierSelectPrinter.AddTextPiece( FONT_SIZE_12, szText );
}

void HeroReinforceDownWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();	
	switch( dwID )
	{
	case ID_ENABLE_REINFORECE_SOLDIER_SELECT_BTN:
		if( cmd == IOBN_BTNUP )
		{
			GradeAdjustmentWnd *pWnd = dynamic_cast<GradeAdjustmentWnd*>( g_GUIMgr.FindWnd( GRADE_ADJUSTMENT_WND ) );
			if( pWnd )
			{
				if( pWnd->IsShow() )
					pWnd->GoToTop();
				else
					pWnd->ShowSubWnd( ioPowerUpManager::PIT_HERO, m_CharInfo.m_class_type );
			}
			else
				LOG.PrintTimeAndLog( 0, "pWnd == Null" );
		}
		break;

	case ID_REINFORCE_BTN:
		if( cmd == IOBN_BTNUP )
		{
			int iCharArray = g_MyInfo.GetClassArray( m_CharInfo.m_class_type );
			if( iCharArray == g_MyInfo.GetSelectCharArray() )
			{
				bool bCheck = true;
				MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
				if( pInvenWnd && pInvenWnd->GetPlayStage() )
				{
					ioMyRoomMode *pMyRoom = ToMyRoomMode( pInvenWnd->GetPlayStage()->GetPlayMode() );
					if( pMyRoom && pMyRoom->GetMyRoomType() == MRT_LOBBY )
						bCheck = false;
				}

				if( bCheck )
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, "출전중인 용병은#진화할 수 없습니다." );
					break;
				}
			}

			int iUseMtrlCount,iCurMtrlCount;
			if( !g_PowerUpManager.CheckEnableSoulStone( ioPowerUpManager::PIT_HERO ,m_CharInfo.m_class_type, iUseMtrlCount, iCurMtrlCount ) )
			{
				SoulStoneBuyWnd *pSoulstoneBuyWnd = dynamic_cast<SoulStoneBuyWnd*> ( g_GUIMgr.FindWnd( SOULSTONE_BUY_WND ) );
				if ( pSoulstoneBuyWnd )
					pSoulstoneBuyWnd->ShowBuyWnd( iUseMtrlCount, iCurMtrlCount );
				break;
			}

			ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
			char szBuf[MAX_PATH];
			memset( szBuf, 0, sizeof(szBuf) );
			kPrinter[0].SetTextStyle( TS_NORMAL );
			kPrinter[0].SetBkColor( 0, 0, 0 );
			kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
			SafeSprintf( szBuf, sizeof( szBuf ), STR(1), iUseMtrlCount );
			kPrinter[0].AddTextPiece( FONT_SIZE_13, szBuf );

			kPrinter[1].SetTextStyle( TS_NORMAL );
			kPrinter[1].SetBkColor( 0, 0, 0 );
			kPrinter[1].SetTextColor( TCT_DEFAULT_GRAY );
			kPrinter[1].AddTextPiece( FONT_SIZE_13, STR(2) );

			g_GUIMgr.SetPrevMsgListBox( NULL, MB_YESNO, this, kPrinter );
		}
		break;
	case MESSAGE_BOX3:
		{
			if( cmd == IOBN_BTNUP )
			{
				if( param == IDYES )
				{
					int iClassType	= m_CharInfo.m_class_type;
					int iUseMtrlCount,iCurMtrlCount;
					if( !g_PowerUpManager.CheckEnableSoulStone( ioPowerUpManager::PIT_HERO ,iClassType, iUseMtrlCount, iCurMtrlCount ) )
					{
						SoulStoneBuyWnd *pSoulstoneBuyWnd = dynamic_cast<SoulStoneBuyWnd*> ( g_GUIMgr.FindWnd( SOULSTONE_BUY_WND ) );
						if ( pSoulstoneBuyWnd )
							pSoulstoneBuyWnd->ShowBuyWnd( iUseMtrlCount, iCurMtrlCount );
						break;
					}

					ioHashString szError;
					if( g_PowerUpManager.CheckEnableCharPowerUp( iClassType, szError ) )
					{
						//CheckEnableCharPowerUp함수에서 유효한 값인지 체크됨
						int iCharIndex = g_MyInfo.GetCharIndexByClassType( iClassType );
						ioEtcItemSoulStone* pEtcItem = dynamic_cast<ioEtcItemSoulStone*>( g_EtcItemMgr.FindEtcItem( g_PowerUpManager.GetMtrlEtcItemCode() ) );
						if( pEtcItem )
						{
							pEtcItem->OnUseGradeAdjustment( iCharIndex, PUTT_CHAR );
						}
					}
				}
			}
		}
		break;
	}
}

void HeroReinforceDownWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "DarkTextBack" )
	{
		SAFEDELETE( m_pDarkTextBack );
		if( pImage )
		{
			m_pDarkTextBack = pImage;
			m_pDarkTextBack->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
			m_pDarkTextBack->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
			m_pDarkTextBack->SetSize( 204, 21 );
		}
	}
	else if( szType == "LightTextBack" )
	{
		SAFEDELETE( m_pLightTextBack );
		if( pImage )
		{
			m_pLightTextBack = pImage;
			m_pLightTextBack->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
			m_pLightTextBack->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
			m_pLightTextBack->SetSize( 204, 21 );
		}
	}
	else if( szType == "SoldierInfoBG" )
	{
		SAFEDELETE( m_pSoldierInfoBG );
		m_pSoldierInfoBG = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}

void HeroReinforceDownWnd::OnRenderAfterChild()
{
	int nXPos = GetDerivedPosX();
	int nYPos = GetDerivedPosY();
	enum 
	{	
		SOULDIER_SELECT_OFFSET_X = 131,
		SOULDIER_SELECT_OFFSET_Y = 428,
	};

	if( m_pDarkTextBack )
	{
		m_pDarkTextBack->Render( nXPos + 27, nYPos + 273, UI_RENDER_MULTIPLY );
		m_pDarkTextBack->Render( nXPos + 27, nYPos + 315, UI_RENDER_MULTIPLY );
		m_pDarkTextBack->Render( nXPos + 27, nYPos + 357, UI_RENDER_MULTIPLY );
		m_pDarkTextBack->Render( nXPos + 27, nYPos + 399, UI_RENDER_MULTIPLY );
	}
	if( m_pLightTextBack )
	{
		m_pLightTextBack->Render( nXPos + 27, nYPos + 294, UI_RENDER_MULTIPLY );
		m_pLightTextBack->Render( nXPos + 27, nYPos + 336, UI_RENDER_MULTIPLY );
		m_pLightTextBack->Render( nXPos + 27, nYPos + 378, UI_RENDER_MULTIPLY );
	}
	if( !m_SoldierInfo.IsEmpty() )
	{
		if( m_pSoldierInfoBG )
			m_pSoldierInfoBG->Render( nXPos + 27, nYPos + 250, UI_RENDER_MULTIPLY );

		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
		g_FontMgr.PrintText( nXPos + 129, nYPos + 252, FONT_SIZE_13, m_SoldierInfo.c_str() );
	}
	m_SoldierSelectPrinter.PrintFullText( nXPos+SOULDIER_SELECT_OFFSET_X, nYPos+SOULDIER_SELECT_OFFSET_Y, TAT_CENTER );
	m_SoldierReinforceInfo.OnRender( nXPos + 34, nYPos + 277 );
	m_SoulStoneCountText.OnRender( nXPos + 35, nYPos + 64 );
}

void HeroReinforceDownWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_szSoulStoneText = xElement.GetStringAttribute_e( "soulstonetext" );
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
HeroReinforceDoneWnd::HeroReinforceDoneWnd( void )
{
	m_pDarkTextBack = NULL;
	m_pLightTextBack = NULL;
	m_pSoldierInfoBG = NULL;

	m_szEffectName.Clear();
	m_szSuccessSound.Clear();
}

HeroReinforceDoneWnd::~HeroReinforceDoneWnd( void )
{
	SAFEDELETE( m_pDarkTextBack );
	SAFEDELETE( m_pLightTextBack );
	SAFEDELETE( m_pSoldierInfoBG );
}

void HeroReinforceDoneWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "DarkTextBack" )
	{
		SAFEDELETE( m_pDarkTextBack );
		if( pImage )
		{
			m_pDarkTextBack = pImage;
			m_pDarkTextBack->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
			m_pDarkTextBack->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
			m_pDarkTextBack->SetSize( 204, 21 );
		}
	}
	else if( szType == "LightTextBack" )
	{
		SAFEDELETE( m_pLightTextBack );
		if( pImage )
		{
			m_pLightTextBack = pImage;
			m_pLightTextBack->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
			m_pLightTextBack->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
			m_pLightTextBack->SetSize( 204, 21 );
		}
	}
	else if( szType == "SoldierInfoBG" )
	{
		SAFEDELETE( m_pSoldierInfoBG );
		m_pSoldierInfoBG = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}

void HeroReinforceDoneWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_szEffectName = xElement.GetStringAttribute_e( "powerup_effect" );
	m_szSuccessSound = xElement.GetStringAttribute_e( "success_sound" );
	m_szSoulStoneText = xElement.GetStringAttribute_e( "soulstonetext" );
}

void HeroReinforceDoneWnd::ClearList()
{
	m_SoldierInfo.Clear();
	m_ReinforcedSoldierInfo.Release();
	m_SoulStoneCountText.Release();
	m_ReinforcedPrinter.ClearList();
}

void HeroReinforceDoneWnd::iwm_show()
{
	ShowChildWnd( HeroReinforceDoneWnd::ID_SOLDIER_WND );
}

void HeroReinforceDoneWnd::iwm_hide()
{
	HideChildWnd( HeroReinforceDoneWnd::ID_SOLDIER_WND );
	if( !m_szSuccessSound.IsEmpty() )
		g_SoundMgr.StopSound( m_szSuccessSound, 0 );
}

void HeroReinforceDoneWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();	
	switch( dwID )
	{
	case ID_REINFORECE_AGAIN_BTN:
		{
			if( cmd == IOBN_BTNUP )
			{
				int iCharArray = g_MyInfo.GetClassArray( m_CharInfo.m_class_type );
				if( iCharArray == g_MyInfo.GetSelectCharArray() )
				{
					bool bCheck = true;
					MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
					if( pInvenWnd && pInvenWnd->GetPlayStage() )
					{
						ioMyRoomMode *pMyRoom = ToMyRoomMode( pInvenWnd->GetPlayStage()->GetPlayMode() );
						if( pMyRoom && pMyRoom->GetMyRoomType() == MRT_LOBBY )
							bCheck = false;
					}

					if( bCheck )
					{
						g_GUIMgr.SetMsgBox( MB_OK, NULL, "출전중인 용병은#진화할 수 없습니다." );
						break;
					}
				}

				int iUseMtrlCount,iCurMtrlCount;
				if( !g_PowerUpManager.CheckEnableSoulStone( ioPowerUpManager::PIT_HERO ,m_CharInfo.m_class_type, iUseMtrlCount, iCurMtrlCount ) )
				{
					SoulStoneBuyWnd *pSoulstoneBuyWnd = dynamic_cast<SoulStoneBuyWnd*> ( g_GUIMgr.FindWnd( SOULSTONE_BUY_WND ) );
					if ( pSoulstoneBuyWnd )
						pSoulstoneBuyWnd->ShowBuyWnd( iUseMtrlCount, iCurMtrlCount );
					break;
				}

				ioHashString szError;
				if( !g_PowerUpManager.CheckEnableCharPowerUp( m_CharInfo.m_class_type, szError ) )
				{
					g_GUIMgr.SetMsgBox( MB_OK, this, szError.c_str() );
					break;
				}

				ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
				char szBuf[MAX_PATH];
				memset( szBuf, 0, sizeof(szBuf) );
				kPrinter[0].SetTextStyle( TS_NORMAL );
				kPrinter[0].SetBkColor( 0, 0, 0 );
				kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
				SafeSprintf( szBuf, sizeof( szBuf ), STR(1), iUseMtrlCount );
				kPrinter[0].AddTextPiece( FONT_SIZE_13, szBuf );

				kPrinter[1].SetTextStyle( TS_NORMAL );
				kPrinter[1].SetBkColor( 0, 0, 0 );
				kPrinter[1].SetTextColor( TCT_DEFAULT_GRAY );
				kPrinter[1].AddTextPiece( FONT_SIZE_13, STR(2) );

				g_GUIMgr.SetPrevMsgListBox( NULL, MB_YESNO, this, kPrinter );
			}
		}
		break;
	case ID_SEE_HERO_BTN:
		{
			if( cmd == IOBN_BTNUP )
			{
				MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*> ( GetParent() );
				if( pInvenWnd && pInvenWnd->IsShow() )
				{
					HideWnd();
					pInvenWnd->ShowSoldierTabDirect( m_CharInfo.m_class_type );
				}
			}
		}
	case MESSAGE_BOX3:
		{
			if( cmd == IOBN_BTNUP )
			{
				if( param == IDYES )
				{
					int iClassType	= m_CharInfo.m_class_type;	
					int iUseMtrlCount,iCurMtrlCount;
					if ( !g_PowerUpManager.CheckEnableSoulStone( ioPowerUpManager::PIT_HERO, iClassType, iUseMtrlCount, iCurMtrlCount ) )
					{
						SoulStoneBuyWnd *pSoulstoneBuyWnd = dynamic_cast<SoulStoneBuyWnd*> ( g_GUIMgr.FindWnd( SOULSTONE_BUY_WND ) );
						if ( pSoulstoneBuyWnd )
							pSoulstoneBuyWnd->ShowBuyWnd( iUseMtrlCount, iCurMtrlCount );
						break;
					}
					ioHashString szError;
					if( g_PowerUpManager.CheckEnableCharPowerUp( iClassType, szError ) )
					{
						//CheckEnableCharPowerUp함수에서 유효한 값인지 체크됨
						int iCharIndex = g_MyInfo.GetCharIndexByClassType( iClassType );
						ioEtcItemSoulStone* pEtcItem = dynamic_cast<ioEtcItemSoulStone*>( g_EtcItemMgr.FindEtcItem( g_PowerUpManager.GetMtrlEtcItemCode() ) );
						if( pEtcItem )
						{
							pEtcItem->OnUseGradeAdjustment( iCharIndex, PUTT_CHAR );
						}
					}
				}
			}
		}
		break;
	}
}

void HeroReinforceDoneWnd::OnRenderAfterChild()
{
	int nXPos = GetDerivedPosX();
	int nYPos = GetDerivedPosY();

	enum 
	{	
		REINFORCED_OFFSET_X = 128,
		REINFORCED_OFFSET_Y = 424,
	};

	if( m_pDarkTextBack )
	{
		m_pDarkTextBack->Render( nXPos + 27, nYPos + 273, UI_RENDER_MULTIPLY );
		m_pDarkTextBack->Render( nXPos + 27, nYPos + 315, UI_RENDER_MULTIPLY );
		m_pDarkTextBack->Render( nXPos + 27, nYPos + 357, UI_RENDER_MULTIPLY );
		m_pDarkTextBack->Render( nXPos + 27, nYPos + 399, UI_RENDER_MULTIPLY );
	}
	if( m_pLightTextBack )
	{
		m_pLightTextBack->Render( nXPos + 27, nYPos + 294, UI_RENDER_MULTIPLY );
		m_pLightTextBack->Render( nXPos + 27, nYPos + 336, UI_RENDER_MULTIPLY );
		m_pLightTextBack->Render( nXPos + 27, nYPos + 378, UI_RENDER_MULTIPLY );
	}
	if( !m_SoldierInfo.IsEmpty() )
	{
		if( m_pSoldierInfoBG )
			m_pSoldierInfoBG->Render( nXPos + 27, nYPos + 250, UI_RENDER_MULTIPLY );

		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
		g_FontMgr.PrintText( nXPos + 129, nYPos + 252, FONT_SIZE_13, m_SoldierInfo.c_str() );
	}
	m_ReinforcedSoldierInfo.OnRender( nXPos + 34, nYPos + 277 );
	m_SoulStoneCountText.OnRender( nXPos + 35, nYPos + 64 );

	if ( !m_ReinforcedPrinter.IsEmpty() )
		m_ReinforcedPrinter.PrintFullText( nXPos+REINFORCED_OFFSET_X, nYPos+REINFORCED_OFFSET_Y, TAT_CENTER );
}

void HeroReinforceDoneWnd::SetSoldierInfo( int iClassType )
{
	if ( g_MyInfo.GetClassArray( iClassType ) == -1 )
	{
		LOG.PrintTimeAndLog( 0, "HeroReinforceDoneWnd::UpdateSoldierInfo - iClassType is not exist" );
		HideChildWnd( HeroReinforceDoneWnd::ID_SOLDIER_WND );
		return;
	}

	m_CharInfo.Init();
	m_CharInfo = g_MyInfo.GetCharacterToClassType( iClassType );
	NewShopHelpWnd *pCharWnd = dynamic_cast<NewShopHelpWnd *>( FindChildWnd( HeroReinforceDoneWnd::ID_SOLDIER_WND ) );
	if( pCharWnd )
	{
		pCharWnd->SetZPosition( FLOAT500 );
		pCharWnd->SetChar( m_CharInfo );
		pCharWnd->SetEquip( false, false, false );
		pCharWnd->SetUserInfoToolTip( true );
		pCharWnd->SetCharScale( 0.85f );
		if ( !m_szEffectName.IsEmpty() )
		{
			pCharWnd->CharAttachEffect( m_szEffectName );
			if( !m_szSuccessSound.IsEmpty() )
				g_SoundMgr.PlaySound( m_szSuccessSound, DSBVOLUME_MAX, PSM_THREAD );
		}
	}

	int iSoulStoneCount = 0;
	ioUserEtcItem *pUserEtcItem = g_MyInfo.GetUserEtcItem();
	if( pUserEtcItem )
	{
		ioUserEtcItem::ETCITEMSLOT kSlot;
		if( pUserEtcItem->GetEtcItem( ioEtcItem::EIT_ETC_SOUL_STONE, kSlot ) )
			iSoulStoneCount = kSlot.m_iValue1;
	}

	ClearList();
	ioHashString szGradeTitle = g_PowerUpManager.GetCharGradeTitle( m_CharInfo.m_class_type, m_CharInfo.m_eCharPowerUpType );
	if ( szGradeTitle.IsEmpty() )
		return;

	char szText[MAX_PATH];
	if( m_CharInfo.m_eCharPowerUpType != PUGT_NONE )
	{
		SafeSprintf( szText, sizeof( szText ), STR(2), szGradeTitle.c_str() );
		m_SoldierInfo = szText;
	}

	m_ReinforcedSoldierInfo.Set( 0, 0, g_PowerUpManager.GetCharGardedString( m_CharInfo.m_class_type, m_CharInfo.m_eCharPowerUpType ).c_str(), FONT_SIZE_12, 194, TS_NORMAL, TCT_DEFAULT_DARKGRAY, 0, TAT_LEFT, TVA_TOP, 8);

	wsprintf( szText, m_szSoulStoneText.c_str(), iSoulStoneCount );
	m_SoulStoneCountText.Set( 0, 0, szText, FONT_SIZE_11, 200, TS_NORMAL, 0xFF6A6869, 0, TAT_LEFT, TVA_TOP, 3);

	SafeSprintf( szText, sizeof( szText ), STR(4),  szGradeTitle.c_str() );
	m_ReinforcedPrinter.SetTextStyle( TS_NORMAL );
	m_ReinforcedPrinter.SetBkColor( 0, 0, 0 );	
	m_ReinforcedPrinter.SetTextColor( TCT_DEFAULT_GREEN );
	m_ReinforcedPrinter.AddTextPiece( FONT_SIZE_17, szText );
}
