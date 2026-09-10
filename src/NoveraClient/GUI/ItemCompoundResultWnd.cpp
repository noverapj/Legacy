
#include "StdAfx.h"

#include "ItemCompoundResultWnd.h"

#include "../ioExtendSoundManager.h"
#include "../ioAccessoryInfoManager.h"

//////////////////////////////////////////////////////////////////////////
ItemCompoundResultWnd::ItemCompoundResultWnd()
{
	m_pIcon = NULL;
	m_pEffect = NULL;

	m_pBackEffect = NULL;
	m_pFinalBackEffect = NULL;

	m_pNumText = NULL;
	m_pPlusText = NULL;


	m_bResult = false;
	m_bSuccess = false;

	m_iTargetSlot = 0;
	m_iPreReinforce = 0;
	m_iResultReinforce = 0;
	m_iItemCode = 0;

	m_pManual = NULL;

	m_nGradeType = 0;
	m_fGradeScale = FLOAT1;
}

ItemCompoundResultWnd::~ItemCompoundResultWnd()
{
	SAFEDELETE( m_pIcon );
	SAFEDELETE( m_pEffect );

	SAFEDELETE( m_pBackEffect );
	SAFEDELETE( m_pFinalBackEffect );

	SAFEDELETE( m_pNumText );
	SAFEDELETE( m_pPlusText );

	m_vRollingEffect.clear();
	m_vEndEfffect_S.clear();
	m_vEndEfffect_F.clear();
}

void ItemCompoundResultWnd::iwm_hide()
{
	if( !m_szRollingSound.IsEmpty() )
		g_SoundMgr.StopSound( m_szRollingSound, 0 );

	if( !m_szSuccessSound.IsEmpty() )
		g_SoundMgr.StopSound( m_szSuccessSound, 0 );

	if( !m_szBigSuccessSound.IsEmpty() )
		g_SoundMgr.StopSound( m_szBigSuccessSound, 0 );

	if( !m_szFailSound.IsEmpty() )
		g_SoundMgr.StopSound( m_szFailSound, 0 );

	g_QuestMgr.ProcessCompleteReserve();
}

void ItemCompoundResultWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "back_effect" )
	{
		SAFEDELETE( m_pBackEffect );
		m_pBackEffect = pImage;
	}
	else if( szType == "final_back_effect" )
	{
		SAFEDELETE( m_pFinalBackEffect );
		m_pFinalBackEffect = pImage;
	}
	else if( szType == "NumText" )
	{
		SAFEDELETE( m_pNumText );
		m_pNumText = pImage;
	}
	else if( szType == "PlusText" )
	{
		SAFEDELETE( m_pPlusText );
		m_pPlusText = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}

void ItemCompoundResultWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	char szKeyName[MAX_PATH]="";
	ioHashString szTemp;

	ZeroMemory( szKeyName, sizeof( szKeyName ) );
	StringCbPrintf_e( szKeyName, sizeof( szKeyName ), "rolling_gap" );
	m_dwRollingTime = xElement.GetIntAttribute( szKeyName );

	ZeroMemory( szKeyName, sizeof( szKeyName ) );
	StringCbPrintf_e( szKeyName, sizeof( szKeyName ), "rolling_duration" );
	m_dwRollingTotalTime = xElement.GetIntAttribute( szKeyName );

	ZeroMemory( szKeyName, sizeof( szKeyName ) );
	StringCbPrintf_e( szKeyName, sizeof( szKeyName ), "Icon_Scale" );
	m_fIconScale = xElement.GetFloatAttribute( szKeyName );

	ZeroMemory( szKeyName, sizeof( szKeyName ) );
	StringCbPrintf_e( szKeyName, sizeof( szKeyName ), "Icon_Scale_Time" );
	m_dwIconScaleTime = xElement.GetIntAttribute( szKeyName );

	ZeroMemory( szKeyName, sizeof( szKeyName ) );
	StringCbPrintf_e( szKeyName, sizeof( szKeyName ), "rolling_sound" );
	m_szRollingSound = xElement.GetStringAttribute( szKeyName );

	ZeroMemory( szKeyName, sizeof( szKeyName ) );
	StringCbPrintf_e( szKeyName, sizeof( szKeyName ), "success_sound" );
	m_szSuccessSound = xElement.GetStringAttribute( szKeyName );

	ZeroMemory( szKeyName, sizeof( szKeyName ) );
	StringCbPrintf_e( szKeyName, sizeof( szKeyName ), "big_success_sound" );
	m_szBigSuccessSound = xElement.GetStringAttribute( szKeyName );

	ZeroMemory( szKeyName, sizeof( szKeyName ) );
	StringCbPrintf_e( szKeyName, sizeof( szKeyName ), "fail_sound" );
	m_szFailSound = xElement.GetStringAttribute( szKeyName );


	m_dwRollingEffectTime = xElement.GetIntAttribute_e( "Effect_Gap_Time" );
	m_vRollingEffect.clear();

	int iMax = xElement.GetIntAttribute_e( "Max_Rolling_Effect" );
	m_vRollingEffect.reserve( iMax );
	for (int i = 0; i < iMax ; i++)
	{
		char szName[MAX_PATH]="";
		StringCbPrintf( szName, sizeof( szName ), "Rolling_Effect_%d", i+1 );
		ioHashString szEffect = xElement.GetStringAttribute( szName );
		if( szEffect == "" )
			break;
		m_vRollingEffect.push_back( szEffect );
	}

	m_vEndEfffect_S.clear();
	iMax = xElement.GetIntAttribute_e( "Max_Success_Effect" );
	m_vEndEfffect_S.reserve( iMax );
	for (int i = 0; i < iMax ; i++)
	{
		char szName[MAX_PATH]="";
		StringCbPrintf( szName, sizeof( szName ), "Success_Effect_%d", i+1 );
		ioHashString szEffect = xElement.GetStringAttribute( szName );
		if( szEffect == "" )
			break;
		m_vEndEfffect_S.push_back( szEffect );
	}

	m_vEndEfffect_F.clear();
	iMax = xElement.GetIntAttribute_e( "Max_Fail_Effect" );
	m_vEndEfffect_F.reserve( iMax );
	for (int i = 0; i < iMax ; i++)
	{
		char szName[MAX_PATH]="";
		StringCbPrintf( szName, sizeof( szName ), "Fail_Effect_%d", i+1 );
		ioHashString szEffect = xElement.GetStringAttribute( szName );
		if( szEffect == "" )
			break;
		m_vEndEfffect_F.push_back( szEffect );
	}
}

void ItemCompoundResultWnd::SetCompoundResultInfo( bool bSuccess, int iTargetSlot, int iReinforce, DWORD dwCode )
{
	m_bResult = false;
	m_bSuccess = bSuccess;
	m_iTargetSlot = iTargetSlot;
	m_iResultReinforce = iReinforce;

	m_iPreReinforce = 0;

	m_dwEtcItemCode = dwCode;

	m_pManual = ioEtcInvenSubManualMgr::GetSingletonPtr();

	SAFEDELETE(m_pIcon);
	SAFEDELETE(m_pEffect);

	ShowWnd();

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwRollingEndTime = dwCurTime + m_dwRollingTotalTime;
	m_dwRollingCheckTime = 0;
	m_dwRollingEffectCheckTime = 0;

	m_iCurEffectArray = -1;

	m_nGradeType = 0;
	m_fGradeScale = FLOAT1;

	ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
	if( pExtraItem )
	{
		EXTRAITEMSLOT kSlot;
		if( pExtraItem->GetExtraItem( m_iTargetSlot, kSlot ) )
		{
			m_iPreReinforce = kSlot.m_iReinforce;

			m_pIcon = g_UIImageSetMgr.CreateImageByFullName( kSlot.m_IconName );
			m_ItemName = kSlot.m_Name;	
			m_iItemCode= kSlot.m_iItemCode;

			const ioItem *pItem = g_ItemMaker.GetItemConst( kSlot.m_iItemCode, __FUNCTION__ );
			if( pItem )
				m_nGradeType= pItem->GetGradeType();
		}
	}

	ReBatchButton( ROLLING_BTN );

	if( !m_szRollingSound.IsEmpty() )
		g_SoundMgr.PlaySound( m_szRollingSound, DSBVOLUME_MAX, PSM_LOOP | PSM_THREAD );

	g_QuestMgr.QuestCompleteTermReserve( QC_EXTRA_ITEM_REINFORCE_SUCCESS, m_iResultReinforce, m_iItemCode );

#if defined( USE_GA )
	char chLabel[32] = {0,};

	if ( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
		sprintf_e( chLabel, "%d", m_iItemCode );
	else
		SafeSprintf( chLabel, sizeof(chLabel), "%1", m_iItemCode );

	// ITEM_ENHANCE_ITEM
	g_HttpMng.GA_EventTracking( g_MyInfo.GetUserIndex()
		, "Item"
		, "Item"
		, chLabel
		, 1
		, "%2FITEM%2FENHANCE%2FITEM" );
#endif
}

void ItemCompoundResultWnd::OnProcess( float fTimePerSec )
{
	ioWnd::OnProcess( fTimePerSec );

	if( !IsShow() ) return;

	RollingEffect( fTimePerSec );

	UpdateCompoundInfo();
}

void ItemCompoundResultWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	enum
	{
		ICON_X		= 129,
		ICON_Y		= 105,

		DESC_BIG_X	= 129,
		DESC_BIG_Y	= 152,

		DESC_X		= 27,
		DESC_Y		= 206,

		FINAL_DESC_X	= 128,
		FINAL_DESC_Y	= 421,
	};

	// Back
	if( m_bResult )
	{
		if( m_pFinalBackEffect )
			m_pFinalBackEffect->Render( iXPos+ICON_X, iYPos+ICON_Y, UI_RENDER_MULTIPLY );
	}
	else
	{
		if( m_pBackEffect )
			m_pBackEffect->Render( iXPos+ICON_X, iYPos+ICON_Y, UI_RENDER_MULTIPLY );
	}

	g_UIImageRenderMgr.RenderGradeIconBack( m_nGradeType, iXPos+ICON_X, iYPos+ICON_Y, m_fGradeScale );

	// Icon
	if( m_pIcon )
	{
		m_pIcon->Render( iXPos+ICON_X, iYPos+ICON_Y, UI_RENDER_NORMAL, TFO_BILINEAR );
	}

	// Effect
	if( m_pEffect )
	{
		m_pEffect->SetScale( 2.0f );
		m_pEffect->Render( iXPos+ICON_X, iYPos+ICON_Y, UI_RENDER_ADD );
	}

	// Reinforce
	enum
	{
		REINFORCE_Y	= 14,

		TEXT_SIZE	= 14,
		TEXT_GAP	= 4,
	};

	if( m_bResult && m_pNumText && m_pPlusText && m_iResultReinforce > 0 )
	{
		int iCurPos = 0;
		int iNewXPos = iXPos+ICON_X;
		int iNewYPos = iYPos+ICON_Y;
		int iDefaultPos = iXPos+ICON_X;

		int iCurYPos = iNewYPos+REINFORCE_Y;

		int iTotalSize = 0;
		if( m_iResultReinforce < 10 )
			iTotalSize = TEXT_SIZE * 2 - TEXT_GAP;
		else if( m_iResultReinforce < 100 )
			iTotalSize = TEXT_SIZE * 3 - TEXT_GAP * 2;
		else
			iTotalSize = TEXT_SIZE * 4 - TEXT_GAP * 3;

		iCurPos = iDefaultPos - (iTotalSize / 2);
		m_pPlusText->Render( iCurPos, iCurYPos );

		iCurPos += (TEXT_SIZE - TEXT_GAP);
		m_pNumText->RenderNum( iCurPos, iCurYPos, m_iResultReinforce, -TEXT_GAP );
	}

	if( m_bResult )
	{
		PrintManual( iXPos+DESC_X, iYPos+DESC_Y, FONT_SIZE_12 );

		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_GREEN );
		g_FontMgr.PrintText( iXPos+FINAL_DESC_X, iYPos+FINAL_DESC_Y, FONT_SIZE_17, STR(1) );
	}
	else
	{
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
		g_FontMgr.PrintText( iXPos+DESC_X, iYPos+DESC_Y, FONT_SIZE_12, STR(2) );
	}

	// Name
	ioComplexStringPrinter kPrinter[2];

	ioHashString szName = m_ItemName;
	ioHashString szDesc = "?????";

	kPrinter[0].ClearList();
	kPrinter[0].SetTextStyle( TS_NORMAL );
	kPrinter[0].SetBkColor( 0, 0, 0 );	
	kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter[0].AddTextPiece( FONT_SIZE_17, szName.c_str() );
	kPrinter[0].PrintFullText( iXPos+DESC_BIG_X, iYPos+DESC_BIG_Y, TAT_CENTER );

	if( m_bResult )
	{
		char szBuf[MAX_PATH] = "";
		if( m_bSuccess )
		{
			int iGap = m_iResultReinforce - m_iPreReinforce;
			SafeSprintf( szBuf, sizeof( szBuf ), STR(3), iGap );
		}
		else
		{
			int iGap = m_iPreReinforce - m_iResultReinforce;
			SafeSprintf( szBuf, sizeof( szBuf ), STR(4), iGap );
		}

		szDesc = szBuf;

		kPrinter[1].ClearList();
		kPrinter[1].SetTextStyle( TS_NORMAL );
		kPrinter[1].SetBkColor( 0, 0, 0 );
		if( m_bSuccess )
			kPrinter[1].SetTextColor( TCT_DEFAULT_BLUE );
		else
			kPrinter[1].SetTextColor( TCT_DEFAULT_RED );
		kPrinter[1].AddTextPiece( FONT_SIZE_17, szDesc.c_str() );
		kPrinter[1].PrintFullText( iXPos+DESC_BIG_X, iYPos+DESC_BIG_Y+23, TAT_CENTER );
	}
}

void ItemCompoundResultWnd::UpdateCompoundInfo()
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwRollingEndTime < dwCurTime )			// RollingEnd
	{
		// Icon 설정
		if( m_pIcon && !m_bResult )		// 한 번만 하기 위해
		{
			m_dwScaleStartTime = dwCurTime;
		}

		// IconScale 설정
		float fScale = FLOAT1;
		m_fGradeScale = FLOAT1;
		DWORD dwGapTime = dwCurTime - m_dwScaleStartTime;
		if( dwGapTime < m_dwIconScaleTime )
		{
			float fScaleRate = (float)dwGapTime / (float)m_dwIconScaleTime;
			fScale = m_fIconScale * ( FLOAT1 - fScaleRate ) + FLOAT1 * fScaleRate;
			m_fGradeScale = fScale;
		}

		if( m_pIcon )
			m_pIcon->SetScale( fScale );

		if( !m_bResult )
		{
			m_bResult = true;
			ReBatchButton( RESULT_BTN );

			if( !m_szRollingSound.IsEmpty() )
				g_SoundMgr.StopSound( m_szRollingSound, 0 );

			if( m_bSuccess )
			{
				bool bBigSuccess = false;
				int iGapReinforce = m_iResultReinforce - m_iPreReinforce;
				if( iGapReinforce == g_CompoundMgr.GetCompoundReinforce( CMT_BIG, m_dwEtcItemCode ) )
					bBigSuccess = true;
				if( g_ExSoundMgr.IsSound( ExSound::EST_REINFORCE_SUCCESS ) )
				{
					g_ExSoundMgr.PlaySound( ExSound::EST_REINFORCE_SUCCESS );
				}
				else
				{
					if( !bBigSuccess && !m_szSuccessSound.IsEmpty() )
						g_SoundMgr.PlaySound( m_szSuccessSound, DSBVOLUME_MAX, PSM_THREAD );
					else if( bBigSuccess && !m_szBigSuccessSound.IsEmpty() )
						g_SoundMgr.PlaySound( m_szBigSuccessSound, DSBVOLUME_MAX, PSM_THREAD );
				}
			}
			else
			{
				if( g_ExSoundMgr.IsSound( ExSound::EST_REINFORCE_FAIL ) )
				{
					g_ExSoundMgr.PlaySound( ExSound::EST_REINFORCE_FAIL );
				}
				else
				{
					if( !m_szFailSound.IsEmpty() )
						g_SoundMgr.PlaySound( m_szFailSound, DSBVOLUME_MAX, PSM_THREAD );
				}
			}

			m_iCurEffectArray = -1;
			m_dwRollingEffectCheckTime = 0;

			g_QuestMgr.QuestCompleteTerm( QC_EXTRA_ITEM_REINFORCE_SUCCESS, m_iResultReinforce, m_iItemCode );
		}
	}
	else
	{
		if( m_dwRollingCheckTime == 0 || dwCurTime - m_dwRollingCheckTime > m_dwRollingTime )
		{
			m_dwRollingCheckTime = dwCurTime;
		}
	}
}

void ItemCompoundResultWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_GO:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();

			MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
			if( pInvenWnd )
			{
				pInvenWnd->ShowExtraItemTabDirect( g_MyInfo.GetClassType() );
			}
		}
		break;
	case ID_USE:
		if( cmd == IOBN_BTNUP )
		{
			ioUserEtcItem *pUserEtcItem =  g_MyInfo.GetUserEtcItem();
			if( pUserEtcItem )
			{
				ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( m_dwEtcItemCode );
				if( !pEtcItem )
				{
					LOG.PrintTimeAndLog( 0, "%s pEtcItem == NULL : %d", __FUNCTION__, m_dwEtcItemCode );
					return;
				}
				HideWnd();

				pEtcItem->OnUseBtnPush( m_dwEtcItemCode, pUserEtcItem );
			}
		}
		break;
	}
}

void ItemCompoundResultWnd::PrintManual( int a_iXPos, int a_iYPos , float fScale )
{
	if( !m_pManual )
		return;

	ioEtcItem *pItem = NULL;
	pItem = g_EtcItemMgr.FindEtcItem( m_dwEtcItemCode );

	if( !pItem ) return;

	int iSubManual = pItem->GetInventorySubManual() - 1;
	ioComplexStringPrinter kPrinter;
	int iMaxLine = m_pManual->GetMaxLine( iSubManual );
	for(int iLine = 1; iLine < iMaxLine; iLine++)
	{
		m_pManual->SetPrinter( iSubManual, iLine, fScale, kPrinter);
		a_iYPos += m_pManual->GetSpaceLine( iSubManual, iLine);
		kPrinter.PrintFullTextWidthCut( a_iXPos, a_iYPos, TAT_LEFT, DESC_WIDTH );
		kPrinter.ClearList();
	}
}

void ItemCompoundResultWnd::RollingEffect( float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();

	int iEffectListCnt = m_vRollingEffect.size();
	if( m_bResult )
	{
		if( m_bSuccess )
			iEffectListCnt = m_vEndEfffect_S.size();
		else
			iEffectListCnt = m_vEndEfffect_F.size();
	}

	if( iEffectListCnt == 0 )
	{
		SAFEDELETE( m_pEffect );
		return;
	}

	if( m_bResult )
	{
		if( m_iCurEffectArray == iEffectListCnt-1 )
		{
			SAFEDELETE( m_pEffect );
			return;
		}

		if( m_dwRollingEffectCheckTime == 0 || dwCurTime - m_dwRollingEffectCheckTime > m_dwRollingEffectTime )		// 교체
		{
			SAFEDELETE( m_pEffect );

			m_iCurEffectArray++;

			if( !COMPARE( m_iCurEffectArray, 0, iEffectListCnt ) )
				m_iCurEffectArray = 0;

			ioHashString szEffect;
			if( m_bSuccess )
				szEffect = m_vEndEfffect_S[m_iCurEffectArray];
			else
				szEffect = m_vEndEfffect_F[m_iCurEffectArray];


			if( !szEffect.IsEmpty() )
			{
				m_pEffect = g_UIImageSetMgr.CreateImageByFullName( szEffect );
			}

			m_dwRollingEffectCheckTime = dwCurTime;
		}
	}
	else
	{
		if( m_dwRollingEffectCheckTime == 0 || dwCurTime - m_dwRollingEffectCheckTime > m_dwRollingEffectTime )		// 교체
		{
			SAFEDELETE( m_pEffect );

			m_iCurEffectArray++;

			if( !COMPARE( m_iCurEffectArray, 0, iEffectListCnt ) )
				m_iCurEffectArray = 0;

			ioHashString szEffect = m_vRollingEffect[m_iCurEffectArray];

			if( !szEffect.IsEmpty() )
			{
				m_pEffect = g_UIImageSetMgr.CreateImageByFullName( szEffect );
			}

			m_dwRollingEffectCheckTime = dwCurTime;
		}
	}
}

void ItemCompoundResultWnd::ReBatchButton( int iType )
{
	enum
	{
		ONE_BTN_SIZE	= 204,
		TWO_BTN_SIZE	= 102,

		ONE_BTN_X		= 27,
		TWO_BTN_X		= 131,

		ONE_TEXT_X		= 102,
		TWO_TEXT_X		= 51,
	};

	switch( iType )
	{
	case ROLLING_BTN:
		ShowChildWnd(ID_ROLLING);
		SetChildInActive(ID_ROLLING);

		HideChildWnd(ID_USE);
		HideChildWnd(ID_GO);
		break;
	case RESULT_BTN:
		HideChildWnd(ID_ROLLING);
		HideChildWnd(ID_USE);
		HideChildWnd(ID_GO);

		// 예외처리
		ioUserEtcItem *pUserEtcItem = g_MyInfo.GetUserEtcItem();
		if( !pUserEtcItem )
		{
			ioButton *pButton = dynamic_cast<ioButton*>(FindChildWnd(ID_GO));
			if( pButton )
			{
				pButton->SetSize( ONE_BTN_SIZE, pButton->GetHeight() );
				pButton->SetWndPos( ONE_BTN_X, pButton->GetYPos() );

				ioUITitle *pTitle =  pButton->GetTitle();
				if( pTitle )
				{
					pTitle->SetOffset( ONE_TEXT_X, pTitle->GetTextYOffSet() );
				}

				pButton->ShowWnd();
			}
			break;
		}

		ioUserEtcItem::ETCITEMSLOT kSlot;
		if( !pUserEtcItem->GetEtcItem( m_dwEtcItemCode, kSlot) )
		{
			ioButton *pButton = dynamic_cast<ioButton*>(FindChildWnd(ID_GO));
			if( pButton )
			{
				pButton->SetSize( ONE_BTN_SIZE, pButton->GetHeight() );
				pButton->SetWndPos( ONE_BTN_X, pButton->GetYPos() );

				ioUITitle *pTitle =  pButton->GetTitle();
				if( pTitle )
				{
					pTitle->SetOffset( ONE_TEXT_X, pTitle->GetTextYOffSet() );
				}

				pButton->ShowWnd();
			}
		}
		else
		{
			ioButton *pButton = dynamic_cast<ioButton*>(FindChildWnd(ID_USE));
			if( pButton )
			{
				pButton->SetSize( TWO_BTN_SIZE, pButton->GetHeight() );
				pButton->SetWndPos( ONE_BTN_X, pButton->GetYPos() );

				ioUITitle *pTitle =  pButton->GetTitle();
				if( pTitle )
				{
					pTitle->SetOffset( TWO_TEXT_X, pTitle->GetTextYOffSet() );
				}

				pButton->ShowWnd();
			}

			pButton = dynamic_cast<ioButton*>(FindChildWnd(ID_GO));
			if( pButton )
			{
				pButton->SetSize( TWO_BTN_SIZE, pButton->GetHeight() );
				pButton->SetWndPos( TWO_BTN_X, pButton->GetYPos() );

				ioUITitle *pTitle =  pButton->GetTitle();
				if( pTitle )
				{
					pTitle->SetOffset( TWO_TEXT_X, pTitle->GetTextYOffSet() );
				}

				pButton->ShowWnd();
			}
		}
		break;
	}
}
//////////////////////////////////////////////////////////////////////////
ItemGrowthCatalystResultWnd::ItemGrowthCatalystResultWnd()
{
	m_pIcon = NULL;
	m_pEffect = NULL;

	m_pBackEffect = NULL;
	m_pFinalBackEffect = NULL;

	m_pNumText = NULL;
	m_pPlusText = NULL;
	
	m_pCatalystMark = NULL;

	m_bResult = false;
	m_bSuccess = false;

	m_iTargetSlot = 0;
	m_iPreReinforce = 0;
	m_iResultReinforce = 0;
	m_iItemCode = 0;
	m_PeriodType = 0;

	m_pManual = NULL;
}

ItemGrowthCatalystResultWnd::~ItemGrowthCatalystResultWnd()
{
	SAFEDELETE( m_pIcon );
	SAFEDELETE( m_pEffect );

	SAFEDELETE( m_pBackEffect );
	SAFEDELETE( m_pFinalBackEffect );

	SAFEDELETE( m_pNumText );
	SAFEDELETE( m_pPlusText );

	SAFEDELETE( m_pCatalystMark );

	m_vRollingEffect.clear();
	m_vEndEfffect_S.clear();
	m_vEndEfffect_F.clear();
}

void ItemGrowthCatalystResultWnd::iwm_hide()
{
	if( !m_szRollingSound.IsEmpty() )
		g_SoundMgr.StopSound( m_szRollingSound, 0 );

	if( !m_szSuccessSound.IsEmpty() )
		g_SoundMgr.StopSound( m_szSuccessSound, 0 );

	if( !m_szBigSuccessSound.IsEmpty() )
		g_SoundMgr.StopSound( m_szBigSuccessSound, 0 );

	if( !m_szFailSound.IsEmpty() )
		g_SoundMgr.StopSound( m_szFailSound, 0 );
}

void ItemGrowthCatalystResultWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "back_effect" )
	{
		SAFEDELETE( m_pBackEffect );
		m_pBackEffect = pImage;
	}
	else if( szType == "final_back_effect" )
	{
		SAFEDELETE( m_pFinalBackEffect );
		m_pFinalBackEffect = pImage;
	}
	else if( szType == "NumText" )
	{
		SAFEDELETE( m_pNumText );
		m_pNumText = pImage;
	}
	else if( szType == "PlusText" )
	{
		SAFEDELETE( m_pPlusText );
		m_pPlusText = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}

void ItemGrowthCatalystResultWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "CatalystMark" )
	{
		SAFEDELETE( m_pCatalystMark );
		m_pCatalystMark = pFrame;
	}
	else
	{
		ioWnd::AddRenderFrame( szType, pFrame );
	}
}

void ItemGrowthCatalystResultWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	char szKeyName[MAX_PATH]="";
	ioHashString szTemp;

	ZeroMemory( szKeyName, sizeof( szKeyName ) );
	StringCbPrintf_e( szKeyName, sizeof( szKeyName ), "rolling_gap" );
	m_dwRollingTime = xElement.GetIntAttribute( szKeyName );

	ZeroMemory( szKeyName, sizeof( szKeyName ) );
	StringCbPrintf_e( szKeyName, sizeof( szKeyName ), "rolling_duration" );
	m_dwRollingTotalTime = xElement.GetIntAttribute( szKeyName );

	ZeroMemory( szKeyName, sizeof( szKeyName ) );
	StringCbPrintf_e( szKeyName, sizeof( szKeyName ), "Icon_Scale" );
	m_fIconScale = xElement.GetFloatAttribute( szKeyName );

	ZeroMemory( szKeyName, sizeof( szKeyName ) );
	StringCbPrintf_e( szKeyName, sizeof( szKeyName ), "Icon_Scale_Time" );
	m_dwIconScaleTime = xElement.GetIntAttribute( szKeyName );

	ZeroMemory( szKeyName, sizeof( szKeyName ) );
	StringCbPrintf_e( szKeyName, sizeof( szKeyName ), "rolling_sound" );
	m_szRollingSound = xElement.GetStringAttribute( szKeyName );

	ZeroMemory( szKeyName, sizeof( szKeyName ) );
	StringCbPrintf_e( szKeyName, sizeof( szKeyName ), "success_sound" );
	m_szSuccessSound = xElement.GetStringAttribute( szKeyName );

	ZeroMemory( szKeyName, sizeof( szKeyName ) );
	StringCbPrintf_e( szKeyName, sizeof( szKeyName ), "big_success_sound" );
	m_szBigSuccessSound = xElement.GetStringAttribute( szKeyName );

	ZeroMemory( szKeyName, sizeof( szKeyName ) );
	StringCbPrintf_e( szKeyName, sizeof( szKeyName ), "fail_sound" );
	m_szFailSound = xElement.GetStringAttribute( szKeyName );
	
	m_dwRollingEffectTime = xElement.GetIntAttribute_e( "Effect_Gap_Time" );
	m_vRollingEffect.clear();

	int iMax = xElement.GetIntAttribute_e( "Max_Rolling_Effect" );
	m_vRollingEffect.reserve( iMax );
	for (int i = 0; i < iMax ; i++)
	{
		char szName[MAX_PATH]="";
		StringCbPrintf( szName, sizeof( szName ), "Rolling_Effect_%d", i+1 );
		ioHashString szEffect = xElement.GetStringAttribute( szName );
		if( szEffect == "" )
			break;
		m_vRollingEffect.push_back( szEffect );
	}

	m_vEndEfffect_S.clear();
	iMax = xElement.GetIntAttribute_e( "Max_Success_Effect" );
	m_vEndEfffect_S.reserve( iMax );
	for (int i = 0; i < iMax ; i++)
	{
		char szName[MAX_PATH]="";
		StringCbPrintf( szName, sizeof( szName ), "Success_Effect_%d", i+1 );
		ioHashString szEffect = xElement.GetStringAttribute( szName );
		if( szEffect == "" )
			break;
		m_vEndEfffect_S.push_back( szEffect );
	}

	m_vEndEfffect_F.clear();
	iMax = xElement.GetIntAttribute_e( "Max_Fail_Effect" );
	m_vEndEfffect_F.reserve( iMax );
	for (int i = 0; i < iMax ; i++)
	{
		char szName[MAX_PATH]="";
		StringCbPrintf( szName, sizeof( szName ), "Fail_Effect_%d", i+1 );
		ioHashString szEffect = xElement.GetStringAttribute( szName );
		if( szEffect == "" )
			break;
		m_vEndEfffect_F.push_back( szEffect );
	}
}

void ItemGrowthCatalystResultWnd::SetGrowthCatalystResultInfo( bool bSuccess, int iTargetSlot, int iReinforce, int iTargetPeriodType, DWORD dwCode )
{
	m_bResult = false;
	m_bSuccess = bSuccess;
	m_iTargetSlot = iTargetSlot;
	m_iResultReinforce = iReinforce;
	m_PeriodType = iTargetPeriodType;

	m_iPreReinforce = 0;

	m_dwEtcItemCode = dwCode;

	m_pManual = ioEtcInvenSubManualMgr::GetSingletonPtr();

	SAFEDELETE(m_pIcon);
	SAFEDELETE(m_pEffect);

	ShowWnd();

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwRollingEndTime = dwCurTime + m_dwRollingTotalTime;
	m_dwRollingCheckTime = 0;
	m_dwRollingEffectCheckTime = 0;

	m_iCurEffectArray = -1;

	ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
	if( pExtraItem )
	{
		EXTRAITEMSLOT kSlot;
		if( pExtraItem->GetExtraItem( m_iTargetSlot, kSlot ) )
		{
			m_iPreReinforce = kSlot.m_iReinforce;

			m_pIcon = g_UIImageSetMgr.CreateImageByFullName( kSlot.m_IconName );
			m_ItemName = kSlot.m_Name;	
			m_iItemCode= kSlot.m_iItemCode;
		}
	}
}

void ItemGrowthCatalystResultWnd::SetGrowthCatalystDelayInfo( int iTargetSlot )
{
	m_bResult = false;
	m_iTargetSlot = iTargetSlot;
	
	m_iPreReinforce = 0;

	m_pManual = ioEtcInvenSubManualMgr::GetSingletonPtr();

	SAFEDELETE(m_pIcon);
	SAFEDELETE(m_pEffect);

	ShowWnd();

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwRollingEndTime = dwCurTime + 60000;
	m_dwRollingCheckTime = 0;
	m_dwRollingEffectCheckTime = 0;

	m_iCurEffectArray = -1;

	ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
	if( pExtraItem )
	{
		EXTRAITEMSLOT kSlot;
		if( pExtraItem->GetExtraItem( m_iTargetSlot, kSlot ) )
		{
			m_iPreReinforce = kSlot.m_iReinforce;

			m_pIcon = g_UIImageSetMgr.CreateImageByFullName( kSlot.m_IconName );
			m_ItemName = kSlot.m_Name;	
			m_iItemCode= kSlot.m_iItemCode;
		}
	}

	ReBatchButton( ROLLING_BTN );

	if( !m_szRollingSound.IsEmpty() )
		g_SoundMgr.PlaySound( m_szRollingSound, DSBVOLUME_MAX, PSM_LOOP | PSM_THREAD );
}

void ItemGrowthCatalystResultWnd::OnProcess( float fTimePerSec )
{
	ioWnd::OnProcess( fTimePerSec );

	if( !IsShow() ) return;

	RollingEffect( fTimePerSec );

	UpdateGrowthCatalystInfo();
}

void ItemGrowthCatalystResultWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	enum
	{
		ICON_X		= 129,
		ICON_Y		= 105,

		DESC_BIG_X	= 129,
		DESC_BIG_Y	= 152,

		DESC_X		= 27,
		DESC_Y		= 206,

		FINAL_DESC_X	= 128,
		FINAL_DESC_Y	= 421,
	};

	// Back
	if( m_bResult )
	{
		if( m_pFinalBackEffect )
			m_pFinalBackEffect->Render( iXPos+ICON_X, iYPos+ICON_Y, UI_RENDER_MULTIPLY );
	}
	else
	{
		if( m_pBackEffect )
			m_pBackEffect->Render( iXPos+ICON_X, iYPos+ICON_Y, UI_RENDER_MULTIPLY );
	}

	// Icon
	if( m_pIcon )
	{
		m_pIcon->Render( iXPos+ICON_X, iYPos+ICON_Y, UI_RENDER_NORMAL, TFO_BILINEAR );
	}

	// Effect
	if( m_pEffect )
	{
		m_pEffect->SetScale( 2.0f );
		m_pEffect->Render( iXPos+ICON_X, iYPos+ICON_Y, UI_RENDER_ADD );
	}

	// Reinforce
	enum
	{
		REINFORCE_Y	= 14,

		TEXT_SIZE	= 14,
		TEXT_GAP	= 4,
	};

	if( m_bResult && m_pNumText && m_pPlusText && m_iResultReinforce > 0 )
	{
		int iCurPos = 0;
		int iNewXPos = iXPos+ICON_X;
		int iNewYPos = iYPos+ICON_Y;
		int iDefaultPos = iXPos+ICON_X;

		int iCurYPos = iNewYPos+REINFORCE_Y;

		int iTotalSize = 0;
		if( m_iResultReinforce < 10 )
			iTotalSize = TEXT_SIZE * 2 - TEXT_GAP;
		else if( m_iResultReinforce < 100 )
			iTotalSize = TEXT_SIZE * 3 - TEXT_GAP * 2;
		else
			iTotalSize = TEXT_SIZE * 4 - TEXT_GAP * 3;

		iCurPos = iDefaultPos - (iTotalSize / 2);
		m_pPlusText->Render( iCurPos, iCurYPos );

		iCurPos += (TEXT_SIZE - TEXT_GAP);
		m_pNumText->RenderNum( iCurPos, iCurYPos, m_iResultReinforce, -TEXT_GAP );		
	}

	if( m_bResult && m_PeriodType == EPT_GROW_TIME )
	{
		if( m_pCatalystMark )
		{
			m_pCatalystMark->Render( (iXPos+ICON_X) - 45, (iYPos+ICON_Y) - 41 );

			g_FontMgr.SetTextStyle( TS_NORMAL );
			g_FontMgr.SetAlignType( TAT_CENTER );
			g_FontMgr.SetBkColor( 0, 0, 0 );
			g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
			g_FontMgr.PrintText( (iXPos+ICON_X) - 20, (iYPos+ICON_Y) - 40, FONT_SIZE_11, STR(1), m_iResultReinforce  );
		}
	}

	if( m_bResult )
	{
		PrintManual( iXPos+DESC_X, iYPos+DESC_Y, FONT_SIZE_12 );

		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_GREEN );
		g_FontMgr.PrintText( iXPos+FINAL_DESC_X, iYPos+FINAL_DESC_Y, FONT_SIZE_17, STR(2) );
	}
	else
	{
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
		g_FontMgr.PrintText( iXPos+DESC_X, iYPos+DESC_Y, FONT_SIZE_12, STR(3) );
	}

	// Name
	ioComplexStringPrinter kPrinter[2];

	ioHashString szName = m_ItemName;
	ioHashString szDesc = "?????";

	kPrinter[0].ClearList();
	kPrinter[0].SetTextStyle( TS_NORMAL );
	kPrinter[0].SetBkColor( 0, 0, 0 );	
	kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter[0].AddTextPiece( FONT_SIZE_17, szName.c_str() );
	kPrinter[0].PrintFullText( iXPos+DESC_BIG_X, iYPos+DESC_BIG_Y, TAT_CENTER );

	if( m_bResult )
	{
		char szBuf[MAX_PATH] = "";
		if( m_bSuccess )
		{
			if( m_PeriodType == EPT_MORTMAIN )
				SafeSprintf( szBuf, sizeof( szBuf ), STR(4) );
			else
			{
				int iGap = m_iResultReinforce - m_iPreReinforce;
				SafeSprintf( szBuf, sizeof( szBuf ), STR(5), iGap );
			}
		}
		else
		{
			int iGap = m_iPreReinforce - m_iResultReinforce;
			SafeSprintf( szBuf, sizeof( szBuf ), STR(6), iGap );
		}

		szDesc = szBuf;

		kPrinter[1].ClearList();
		kPrinter[1].SetTextStyle( TS_NORMAL );
		kPrinter[1].SetBkColor( 0, 0, 0 );
		if( m_bSuccess )
			kPrinter[1].SetTextColor( TCT_DEFAULT_BLUE );
		else
			kPrinter[1].SetTextColor( TCT_DEFAULT_RED );
		kPrinter[1].AddTextPiece( FONT_SIZE_17, szDesc.c_str() );
		kPrinter[1].PrintFullText( iXPos+DESC_BIG_X, iYPos+DESC_BIG_Y+23, TAT_CENTER );
	}
}

void ItemGrowthCatalystResultWnd::UpdateGrowthCatalystInfo()
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwRollingEndTime < dwCurTime )			// RollingEnd
	{
		// Icon 설정
		if( m_pIcon && !m_bResult )		// 한 번만 하기 위해
		{
			m_dwScaleStartTime = dwCurTime;
		}

		// IconScale 설정
		float fScale = FLOAT1;
		DWORD dwGapTime = dwCurTime - m_dwScaleStartTime;
		if( dwGapTime < m_dwIconScaleTime )
		{
			float fScaleRate = (float)dwGapTime / (float)m_dwIconScaleTime;
			fScale = m_fIconScale * ( FLOAT1 - fScaleRate ) + FLOAT1 * fScaleRate;
		}

		if( m_pIcon )
			m_pIcon->SetScale( fScale );

		if( !m_bResult )
		{
			m_bResult = true;
			ReBatchButton( RESULT_BTN );

			if( !m_szRollingSound.IsEmpty() )
				g_SoundMgr.StopSound( m_szRollingSound, 0 );

			if( m_bSuccess )
			{
				if( !m_szSuccessSound.IsEmpty() )
					g_SoundMgr.PlaySound( m_szSuccessSound, DSBVOLUME_MAX, PSM_THREAD );
				else if( !m_szBigSuccessSound.IsEmpty() )
					g_SoundMgr.PlaySound( m_szBigSuccessSound, DSBVOLUME_MAX, PSM_THREAD );
			}
			else
			{
				if( !m_szFailSound.IsEmpty() )
					g_SoundMgr.PlaySound( m_szFailSound, DSBVOLUME_MAX, PSM_THREAD );
			}

			m_iCurEffectArray = -1;
			m_dwRollingEffectCheckTime = 0;
		}
	}
	else
	{
		if( m_dwRollingCheckTime == 0 || dwCurTime - m_dwRollingCheckTime > m_dwRollingTime )
		{
			m_dwRollingCheckTime = dwCurTime;
		}
	}
}

void ItemGrowthCatalystResultWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_GO:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();

			MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
			if( pInvenWnd )
			{
				pInvenWnd->ShowExtraItemTabDirect( g_MyInfo.GetClassType() );
			}
		}
		break;
	case ID_USE:
		if( cmd == IOBN_BTNUP )
		{
			ioUserEtcItem *pUserEtcItem =  g_MyInfo.GetUserEtcItem();
			if( pUserEtcItem )
			{
				ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( m_dwEtcItemCode );
				if( !pEtcItem )
				{
					LOG.PrintTimeAndLog( 0, "%s pEtcItem == NULL : %d", __FUNCTION__, m_dwEtcItemCode );
					return;
				}
				HideWnd();

				pEtcItem->OnUseBtnPush( m_dwEtcItemCode, pUserEtcItem );
			}
		}
		break;
	}
}

void ItemGrowthCatalystResultWnd::PrintManual( int a_iXPos, int a_iYPos , float fScale )
{
	if( !m_pManual )
		return;

	ioEtcItem *pItem = NULL;
	pItem = g_EtcItemMgr.FindEtcItem( m_dwEtcItemCode );

	if( !pItem ) return;

	int iSubManual = pItem->GetInventorySubManual() - 1;
	ioComplexStringPrinter kPrinter;
	int iMaxLine = m_pManual->GetMaxLine( iSubManual );
	for(int iLine = 1; iLine < iMaxLine; iLine++)
	{
		m_pManual->SetPrinter( iSubManual, iLine, fScale, kPrinter);
		a_iYPos += m_pManual->GetSpaceLine( iSubManual, iLine);
		kPrinter.PrintFullTextWidthCut( a_iXPos, a_iYPos, TAT_LEFT, DESC_WIDTH );
		kPrinter.ClearList();
	}
}

void ItemGrowthCatalystResultWnd::RollingEffect( float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();

	int iEffectListCnt = m_vRollingEffect.size();
	if( m_bResult )
	{
		if( m_bSuccess )
			iEffectListCnt = m_vEndEfffect_S.size();
		else
			iEffectListCnt = m_vEndEfffect_F.size();
	}

	if( iEffectListCnt == 0 )
	{
		SAFEDELETE( m_pEffect );
		return;
	}

	if( m_bResult )
	{
		if( m_iCurEffectArray == iEffectListCnt-1 )
		{
			SAFEDELETE( m_pEffect );
			return;
		}

		if( m_dwRollingEffectCheckTime == 0 || dwCurTime - m_dwRollingEffectCheckTime > m_dwRollingEffectTime )		// 교체
		{
			SAFEDELETE( m_pEffect );

			m_iCurEffectArray++;

			if( !COMPARE( m_iCurEffectArray, 0, iEffectListCnt ) )
				m_iCurEffectArray = 0;

			ioHashString szEffect;
			if( m_bSuccess )
				szEffect = m_vEndEfffect_S[m_iCurEffectArray];
			else
				szEffect = m_vEndEfffect_F[m_iCurEffectArray];


			if( !szEffect.IsEmpty() )
			{
				m_pEffect = g_UIImageSetMgr.CreateImageByFullName( szEffect );
			}

			m_dwRollingEffectCheckTime = dwCurTime;
		}
	}
	else
	{
		if( m_dwRollingEffectCheckTime == 0 || dwCurTime - m_dwRollingEffectCheckTime > m_dwRollingEffectTime )		// 교체
		{
			SAFEDELETE( m_pEffect );

			m_iCurEffectArray++;

			if( !COMPARE( m_iCurEffectArray, 0, iEffectListCnt ) )
				m_iCurEffectArray = 0;

			ioHashString szEffect = m_vRollingEffect[m_iCurEffectArray];

			if( !szEffect.IsEmpty() )
			{
				m_pEffect = g_UIImageSetMgr.CreateImageByFullName( szEffect );
			}

			m_dwRollingEffectCheckTime = dwCurTime;
		}
	}
}

void ItemGrowthCatalystResultWnd::ReBatchButton( int iType )
{
	enum
	{
		ONE_BTN_SIZE	= 204,
		TWO_BTN_SIZE	= 102,

		ONE_BTN_X		= 27,
		TWO_BTN_X		= 131,

		ONE_TEXT_X		= 102,
		TWO_TEXT_X		= 51,
	};

	switch( iType )
	{
	case ROLLING_BTN:
		ShowChildWnd(ID_ROLLING);
		SetChildInActive(ID_ROLLING);

		HideChildWnd(ID_USE);
		HideChildWnd(ID_GO);
		break;
	case RESULT_BTN:
		HideChildWnd(ID_ROLLING);
		HideChildWnd(ID_USE);
		HideChildWnd(ID_GO);

		// 예외처리
		ioUserEtcItem *pUserEtcItem = g_MyInfo.GetUserEtcItem();
		if( !pUserEtcItem )
		{
			ioButton *pButton = dynamic_cast<ioButton*>(FindChildWnd(ID_GO));
			if( pButton )
			{
				pButton->SetSize( ONE_BTN_SIZE, pButton->GetHeight() );
				pButton->SetWndPos( ONE_BTN_X, pButton->GetYPos() );

				ioUITitle *pTitle =  pButton->GetTitle();
				if( pTitle )
				{
					pTitle->SetOffset( ONE_TEXT_X, pTitle->GetTextYOffSet() );
				}

				pButton->ShowWnd();
			}
			break;
		}

		ioUserEtcItem::ETCITEMSLOT kSlot;
		if( !pUserEtcItem->GetEtcItem( m_dwEtcItemCode, kSlot) )
		{
			ioButton *pButton = dynamic_cast<ioButton*>(FindChildWnd(ID_GO));
			if( pButton )
			{
				pButton->SetSize( ONE_BTN_SIZE, pButton->GetHeight() );
				pButton->SetWndPos( ONE_BTN_X, pButton->GetYPos() );

				ioUITitle *pTitle =  pButton->GetTitle();
				if( pTitle )
				{
					pTitle->SetOffset( ONE_TEXT_X, pTitle->GetTextYOffSet() );
				}

				pButton->ShowWnd();
			}
		}
		else
		{
			ioButton *pButton = dynamic_cast<ioButton*>(FindChildWnd(ID_USE));
			if( pButton )
			{
				pButton->SetSize( TWO_BTN_SIZE, pButton->GetHeight() );
				pButton->SetWndPos( ONE_BTN_X, pButton->GetYPos() );

				ioUITitle *pTitle =  pButton->GetTitle();
				if( pTitle )
				{
					pTitle->SetOffset( TWO_TEXT_X, pTitle->GetTextYOffSet() );
				}

				pButton->ShowWnd();
			}

			pButton = dynamic_cast<ioButton*>(FindChildWnd(ID_GO));
			if( pButton )
			{
				pButton->SetSize( TWO_BTN_SIZE, pButton->GetHeight() );
				pButton->SetWndPos( TWO_BTN_X, pButton->GetYPos() );

				ioUITitle *pTitle =  pButton->GetTitle();
				if( pTitle )
				{
					pTitle->SetOffset( TWO_TEXT_X, pTitle->GetTextYOffSet() );
				}

				pButton->ShowWnd();
			}
		}
		break;
	}
}
//////////////////////////////////////////////////////////////////////////
ItemCompoundExResultWnd::ItemCompoundExResultWnd()
{
	m_pIcon = NULL;
	m_pEffect = NULL;

	m_pBackEffect = NULL;
	m_pFinalBackEffect = NULL;

	m_pNumText = NULL;
	m_pPlusText = NULL;

	m_bResult = false;
	m_bSuccess = false;

	m_iTargetSlot = 0;
	m_iPreReinforce = 0;
	m_iResultReinforce = 0;
	m_iItemCode = 0;

	m_pManual = NULL;

	m_nGradeType = 0;
	m_fGradeScale = FLOAT1;
}

ItemCompoundExResultWnd::~ItemCompoundExResultWnd()
{
	ErrorReport::SetPosition( 1500, 1 );
	SAFEDELETE( m_pIcon );
	ErrorReport::SetPosition( 1500, 2 );
	SAFEDELETE( m_pEffect );
	ErrorReport::SetPosition( 1500, 3 );

	SAFEDELETE( m_pBackEffect );
	ErrorReport::SetPosition( 1500, 4 );
	SAFEDELETE( m_pFinalBackEffect );
	ErrorReport::SetPosition( 1500, 5 );

	SAFEDELETE( m_pNumText );
	ErrorReport::SetPosition( 1500, 6 );
	SAFEDELETE( m_pPlusText );
	ErrorReport::SetPosition( 1500, 7 );

	m_vRollingEffect.clear();
	ErrorReport::SetPosition( 1500, 8 );
	m_vEndEfffect_S.clear();
	ErrorReport::SetPosition( 1500, 9 );
	m_vEndEfffect_F.clear();
	ErrorReport::SetPosition( 1500, 10 );
}

void ItemCompoundExResultWnd::iwm_hide()
{
	if( !m_szRollingSound.IsEmpty() )
		g_SoundMgr.StopSound( m_szRollingSound, 0 );

	if( !m_szSuccessSound.IsEmpty() )
		g_SoundMgr.StopSound( m_szSuccessSound, 0 );

	if( !m_szBigSuccessSound.IsEmpty() )
		g_SoundMgr.StopSound( m_szBigSuccessSound, 0 );

	if( !m_szFailSound.IsEmpty() )
		g_SoundMgr.StopSound( m_szFailSound, 0 );

	g_QuestMgr.ProcessCompleteReserve();
}

void ItemCompoundExResultWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "back_effect" )
	{
		SAFEDELETE( m_pBackEffect );
		m_pBackEffect = pImage;
	}
	else if( szType == "final_back_effect" )
	{
		SAFEDELETE( m_pFinalBackEffect );
		m_pFinalBackEffect = pImage;
	}
	else if( szType == "NumText" )
	{
		SAFEDELETE( m_pNumText );
		m_pNumText = pImage;
	}
	else if( szType == "PlusText" )
	{
		SAFEDELETE( m_pPlusText );
		m_pPlusText = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}

void ItemCompoundExResultWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	char szKeyName[MAX_PATH]="";
	ioHashString szTemp;

	ZeroMemory( szKeyName, sizeof( szKeyName ) );
	StringCbPrintf_e( szKeyName, sizeof( szKeyName ), "rolling_gap" );
	m_dwRollingTime = xElement.GetIntAttribute( szKeyName );

	ZeroMemory( szKeyName, sizeof( szKeyName ) );
	StringCbPrintf_e( szKeyName, sizeof( szKeyName ), "rolling_duration" );
	m_dwRollingTotalTime = xElement.GetIntAttribute( szKeyName );

	ZeroMemory( szKeyName, sizeof( szKeyName ) );
	StringCbPrintf_e( szKeyName, sizeof( szKeyName ), "Icon_Scale" );
	m_fIconScale = xElement.GetFloatAttribute( szKeyName );

	ZeroMemory( szKeyName, sizeof( szKeyName ) );
	StringCbPrintf_e( szKeyName, sizeof( szKeyName ), "Icon_Scale_Time" );
	m_dwIconScaleTime = xElement.GetIntAttribute( szKeyName );

	ZeroMemory( szKeyName, sizeof( szKeyName ) );
	StringCbPrintf_e( szKeyName, sizeof( szKeyName ), "rolling_sound" );
	m_szRollingSound = xElement.GetStringAttribute( szKeyName );

	ZeroMemory( szKeyName, sizeof( szKeyName ) );
	StringCbPrintf_e( szKeyName, sizeof( szKeyName ), "success_sound" );
	m_szSuccessSound = xElement.GetStringAttribute( szKeyName );

	ZeroMemory( szKeyName, sizeof( szKeyName ) );
	StringCbPrintf_e( szKeyName, sizeof( szKeyName ), "big_success_sound" );
	m_szBigSuccessSound = xElement.GetStringAttribute( szKeyName );

	ZeroMemory( szKeyName, sizeof( szKeyName ) );
	StringCbPrintf_e( szKeyName, sizeof( szKeyName ), "fail_sound" );
	m_szFailSound = xElement.GetStringAttribute( szKeyName );
	
	m_dwRollingEffectTime = xElement.GetIntAttribute_e( "Effect_Gap_Time" );
	m_vRollingEffect.clear();

	int iMax = xElement.GetIntAttribute_e( "Max_Rolling_Effect" );
	m_vRollingEffect.reserve( iMax );
	for (int i = 0; i < iMax ; i++)
	{
		char szName[MAX_PATH]="";
		StringCbPrintf( szName, sizeof( szName ), "Rolling_Effect_%d", i+1 );
		ioHashString szEffect = xElement.GetStringAttribute( szName );
		if( szEffect == "" )
			break;
		m_vRollingEffect.push_back( szEffect );
	}

	m_vEndEfffect_S.clear();
	iMax = xElement.GetIntAttribute_e( "Max_Success_Effect" );
	m_vEndEfffect_S.reserve( iMax );
	for (int i = 0; i < iMax ; i++)
	{
		char szName[MAX_PATH]="";
		StringCbPrintf( szName, sizeof( szName ), "Success_Effect_%d", i+1 );
		ioHashString szEffect = xElement.GetStringAttribute( szName );
		if( szEffect == "" )
			break;
		m_vEndEfffect_S.push_back( szEffect );
	}

	m_vEndEfffect_F.clear();
	iMax = xElement.GetIntAttribute_e( "Max_Fail_Effect" );
	m_vEndEfffect_F.reserve( iMax );
	for (int i = 0; i < iMax ; i++)
	{
		char szName[MAX_PATH]="";
		StringCbPrintf( szName, sizeof( szName ), "Fail_Effect_%d", i+1 );
		ioHashString szEffect = xElement.GetStringAttribute( szName );
		if( szEffect == "" )
			break;
		m_vEndEfffect_F.push_back( szEffect );
	}
}

void ItemCompoundExResultWnd::SetItemCompoundExInfo( bool bSuccess, int iTargetSlot, int iReinforce, DWORD dwCode )
{
	m_bResult = false;
	m_bSuccess = bSuccess;
	m_iTargetSlot = iTargetSlot;
	m_iResultReinforce = iReinforce;

	m_iPreReinforce = 0;

	m_dwEtcItemCode = dwCode;

	m_pManual = ioEtcInvenSubManualMgr::GetSingletonPtr();

	SAFEDELETE(m_pIcon);
	SAFEDELETE(m_pEffect);

	ShowWnd();

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwRollingEndTime = dwCurTime + m_dwRollingTotalTime;
	m_dwRollingCheckTime = 0;
	m_dwRollingEffectCheckTime = 0;

	m_iCurEffectArray = -1;

	m_nGradeType = 0;
	m_fGradeScale = FLOAT1;

	ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
	if( pExtraItem )
	{
		EXTRAITEMSLOT kSlot;
		if( pExtraItem->GetExtraItem( m_iTargetSlot, kSlot ) )
		{
			m_iPreReinforce = kSlot.m_iReinforce;

			m_pIcon = g_UIImageSetMgr.CreateImageByFullName( kSlot.m_IconName );
			m_ItemName = kSlot.m_Name;	
			m_iItemCode= kSlot.m_iItemCode;
			
			const ioItem *pItem = g_ItemMaker.GetItemConst( kSlot.m_iItemCode, __FUNCTION__ );
			if( pItem )
				m_nGradeType= pItem->GetGradeType();
		}
	}

	ReBatchButton( ROLLING_BTN );

	if( !m_szRollingSound.IsEmpty() )
		g_SoundMgr.PlaySound( m_szRollingSound, DSBVOLUME_MAX, PSM_LOOP | PSM_THREAD );

	g_QuestMgr.QuestCompleteTermReserve( QC_EXTRA_ITEM_REINFORCE_SUCCESS, m_iResultReinforce, m_iItemCode );

#if defined( USE_GA )
	char chLabel[32] = {0,};

	if ( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
		sprintf_e( chLabel, "%d", m_iItemCode );
	else
		SafeSprintf( chLabel, sizeof(chLabel), "%1", m_iItemCode );

	// ITEM_ENHANCE_CARD
	g_HttpMng.GA_EventTracking( g_MyInfo.GetUserIndex()
		, "Item"
		, "Card"
		, chLabel
		, 1
		, "%2FITEM%2FENHANCE%2FCARD" );
#endif
}

void ItemCompoundExResultWnd::OnProcess( float fTimePerSec )
{
	ioWnd::OnProcess( fTimePerSec );

	if( !IsShow() ) return;

	RollingEffect( fTimePerSec );

	UpdateItemCompoundExInfo();
}

void ItemCompoundExResultWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	enum
	{
		ICON_X		= 129,
		ICON_Y		= 105,

		DESC_BIG_X	= 129,
		DESC_BIG_Y	= 152,

		DESC_X		= 27,
		DESC_Y		= 206,

		FINAL_DESC_X	= 128,
		FINAL_DESC_Y	= 421,
	};

	// Back
	if( m_bResult )
	{
		if( m_pFinalBackEffect )
			m_pFinalBackEffect->Render( iXPos+ICON_X, iYPos+ICON_Y, UI_RENDER_MULTIPLY );
	}
	else
	{
		if( m_pBackEffect )
			m_pBackEffect->Render( iXPos+ICON_X, iYPos+ICON_Y, UI_RENDER_MULTIPLY );
	}

	g_UIImageRenderMgr.RenderGradeIconBack( m_nGradeType, iXPos+ICON_X, iYPos+ICON_Y, m_fGradeScale );

	// Icon
	if( m_pIcon )
	{
		m_pIcon->Render( iXPos+ICON_X, iYPos+ICON_Y, UI_RENDER_NORMAL, TFO_BILINEAR );
	}

	// Effect
	if( m_pEffect )
	{
		m_pEffect->SetScale( 2.0f );
		m_pEffect->Render( iXPos+ICON_X, iYPos+ICON_Y, UI_RENDER_ADD );
	}

	// Reinforce
	enum
	{
		REINFORCE_Y	= 14,

		TEXT_SIZE	= 14,
		TEXT_GAP	= 4,
	};

	if( m_bResult && m_pNumText && m_pPlusText && m_iResultReinforce > 0 )
	{
		int iCurPos = 0;
		int iNewXPos = iXPos+ICON_X;
		int iNewYPos = iYPos+ICON_Y;
		int iDefaultPos = iXPos+ICON_X;

		int iCurYPos = iNewYPos+REINFORCE_Y;

		int iTotalSize = 0;
		if( m_iResultReinforce < 10 )
			iTotalSize = TEXT_SIZE * 2 - TEXT_GAP;
		else if( m_iResultReinforce < 100 )
			iTotalSize = TEXT_SIZE * 3 - TEXT_GAP * 2;
		else
			iTotalSize = TEXT_SIZE * 4 - TEXT_GAP * 3;

		iCurPos = iDefaultPos - (iTotalSize / 2);
		m_pPlusText->Render( iCurPos, iCurYPos );

		iCurPos += (TEXT_SIZE - TEXT_GAP);
		m_pNumText->RenderNum( iCurPos, iCurYPos, m_iResultReinforce, -TEXT_GAP );		
	}

	if( m_bResult )
	{
		PrintManual( iXPos+DESC_X, iYPos+DESC_Y, FONT_SIZE_12 );

		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_GREEN );
		g_FontMgr.PrintText( iXPos+FINAL_DESC_X, iYPos+FINAL_DESC_Y, FONT_SIZE_17, STR(1) );
	}
	else
	{
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
		g_FontMgr.PrintText( iXPos+DESC_X, iYPos+DESC_Y, FONT_SIZE_12, STR(2) );
	}

	// Name
	ioComplexStringPrinter kPrinter[2];

	ioHashString szName = m_ItemName;
	ioHashString szDesc = "?????";

	kPrinter[0].ClearList();
	kPrinter[0].SetTextStyle( TS_NORMAL );
	kPrinter[0].SetBkColor( 0, 0, 0 );	
	kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter[0].AddTextPiece( FONT_SIZE_17, szName.c_str() );
	kPrinter[0].PrintFullText( iXPos+DESC_BIG_X, iYPos+DESC_BIG_Y, TAT_CENTER );

	if( m_bResult )
	{
		char szBuf[MAX_PATH] = "";
		if( m_bSuccess )
		{
			int iGap = m_iResultReinforce - m_iPreReinforce;
			SafeSprintf( szBuf, sizeof( szBuf ), STR(3), iGap );
		}
		else
		{
			SafeSprintf( szBuf, sizeof( szBuf ), STR(4) );
		}

		szDesc = szBuf;

		kPrinter[1].ClearList();
		kPrinter[1].SetTextStyle( TS_NORMAL );
		kPrinter[1].SetBkColor( 0, 0, 0 );
		if( m_bSuccess )
			kPrinter[1].SetTextColor( TCT_DEFAULT_BLUE );
		else
			kPrinter[1].SetTextColor( TCT_DEFAULT_RED );
		kPrinter[1].AddTextPiece( FONT_SIZE_17, szDesc.c_str() );
		kPrinter[1].PrintFullText( iXPos+DESC_BIG_X, iYPos+DESC_BIG_Y+23, TAT_CENTER );
	}
}

void ItemCompoundExResultWnd::UpdateItemCompoundExInfo()
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwRollingEndTime < dwCurTime )			// RollingEnd
	{
		// Icon 설정
		if( m_pIcon && !m_bResult )		// 한 번만 하기 위해
		{
			m_dwScaleStartTime = dwCurTime;
		}

		// IconScale 설정
		float fScale = FLOAT1;
		m_fGradeScale = FLOAT1;
		DWORD dwGapTime = dwCurTime - m_dwScaleStartTime;
		if( dwGapTime < m_dwIconScaleTime )
		{
			float fScaleRate = (float)dwGapTime / (float)m_dwIconScaleTime;
			fScale = m_fIconScale * ( FLOAT1 - fScaleRate ) + FLOAT1 * fScaleRate;
			m_fGradeScale = fScale;
		}

		if( m_pIcon )
			m_pIcon->SetScale( fScale );

		if( !m_bResult )
		{
			m_bResult = true;
			ReBatchButton( RESULT_BTN );

			if( !m_szRollingSound.IsEmpty() )
				g_SoundMgr.StopSound( m_szRollingSound, 0 );

			if( m_bSuccess )
			{
				if( !m_szSuccessSound.IsEmpty() )
					g_SoundMgr.PlaySound( m_szSuccessSound, DSBVOLUME_MAX, PSM_THREAD );
				else if( !m_szBigSuccessSound.IsEmpty() )
					g_SoundMgr.PlaySound( m_szBigSuccessSound, DSBVOLUME_MAX, PSM_THREAD );
			}
			else
			{
				if( !m_szFailSound.IsEmpty() )
					g_SoundMgr.PlaySound( m_szFailSound, DSBVOLUME_MAX, PSM_THREAD );
			}

			m_iCurEffectArray = -1;
			m_dwRollingEffectCheckTime = 0;

			g_QuestMgr.QuestCompleteTerm( QC_EXTRA_ITEM_REINFORCE_SUCCESS, m_iResultReinforce, m_iItemCode );
		}
	}
	else
	{
		if( m_dwRollingCheckTime == 0 || dwCurTime - m_dwRollingCheckTime > m_dwRollingTime )
		{
			m_dwRollingCheckTime = dwCurTime;
		}
	}
}

void ItemCompoundExResultWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_GO:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();

			MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
			if( pInvenWnd )
			{
				pInvenWnd->ShowExtraItemTabDirect( g_MyInfo.GetClassType() );
			}
		}
		break;
	case ID_USE:
		if( cmd == IOBN_BTNUP )
		{
			ioUserEtcItem *pUserEtcItem =  g_MyInfo.GetUserEtcItem();
			if( pUserEtcItem )
			{
				ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( m_dwEtcItemCode );
				if( !pEtcItem )
				{
					LOG.PrintTimeAndLog( 0, "%s pEtcItem == NULL : %d", __FUNCTION__, m_dwEtcItemCode );
					return;
				}
				HideWnd();

				pEtcItem->OnUseBtnPush( m_dwEtcItemCode, pUserEtcItem );
			}
		}
		break;
	}
}

void ItemCompoundExResultWnd::PrintManual( int a_iXPos, int a_iYPos , float fScale )
{
	if( !m_pManual )
		return;

	ioEtcItem *pItem = NULL;
	pItem = g_EtcItemMgr.FindEtcItem( m_dwEtcItemCode );

	if( !pItem ) return;

	int iSubManual = pItem->GetInventorySubManual() - 1;
	ioComplexStringPrinter kPrinter;
	int iMaxLine = m_pManual->GetMaxLine( iSubManual );
	for(int iLine = 1; iLine < iMaxLine; iLine++)
	{
		m_pManual->SetPrinter( iSubManual, iLine, fScale, kPrinter);
		a_iYPos += m_pManual->GetSpaceLine( iSubManual, iLine);
		kPrinter.PrintFullTextWidthCut( a_iXPos, a_iYPos, TAT_LEFT, DESC_WIDTH );
		kPrinter.ClearList();
	}
}

void ItemCompoundExResultWnd::RollingEffect( float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();

	int iEffectListCnt = m_vRollingEffect.size();
	if( m_bResult )
	{
		if( m_bSuccess )
			iEffectListCnt = m_vEndEfffect_S.size();
		else
			iEffectListCnt = m_vEndEfffect_F.size();
	}

	if( iEffectListCnt == 0 )
	{
		SAFEDELETE( m_pEffect );
		return;
	}

	if( m_bResult )
	{
		if( m_iCurEffectArray == iEffectListCnt-1 )
		{
			SAFEDELETE( m_pEffect );
			return;
		}

		if( m_dwRollingEffectCheckTime == 0 || dwCurTime - m_dwRollingEffectCheckTime > m_dwRollingEffectTime )		// 교체
		{
			SAFEDELETE( m_pEffect );

			m_iCurEffectArray++;

			if( !COMPARE( m_iCurEffectArray, 0, iEffectListCnt ) )
				m_iCurEffectArray = 0;

			ioHashString szEffect;
			if( m_bSuccess )
				szEffect = m_vEndEfffect_S[m_iCurEffectArray];
			else
				szEffect = m_vEndEfffect_F[m_iCurEffectArray];


			if( !szEffect.IsEmpty() )
			{
				m_pEffect = g_UIImageSetMgr.CreateImageByFullName( szEffect );
			}

			m_dwRollingEffectCheckTime = dwCurTime;
		}
	}
	else
	{
		if( m_dwRollingEffectCheckTime == 0 || dwCurTime - m_dwRollingEffectCheckTime > m_dwRollingEffectTime )		// 교체
		{
			SAFEDELETE( m_pEffect );

			m_iCurEffectArray++;

			if( !COMPARE( m_iCurEffectArray, 0, iEffectListCnt ) )
				m_iCurEffectArray = 0;

			ioHashString szEffect = m_vRollingEffect[m_iCurEffectArray];

			if( !szEffect.IsEmpty() )
			{
				m_pEffect = g_UIImageSetMgr.CreateImageByFullName( szEffect );
			}

			m_dwRollingEffectCheckTime = dwCurTime;
		}
	}
}

void ItemCompoundExResultWnd::ReBatchButton( int iType )
{
	enum
	{
		ONE_BTN_SIZE	= 204,
		TWO_BTN_SIZE	= 102,

		ONE_BTN_X		= 27,
		TWO_BTN_X		= 131,

		ONE_TEXT_X		= 102,
		TWO_TEXT_X		= 51,
	};

	switch( iType )
	{
	case ROLLING_BTN:
		ShowChildWnd(ID_ROLLING);
		SetChildInActive(ID_ROLLING);

		HideChildWnd(ID_USE);
		HideChildWnd(ID_GO);
		break;
	case RESULT_BTN:
		HideChildWnd(ID_ROLLING);
		HideChildWnd(ID_USE);
		HideChildWnd(ID_GO);

		// 예외처리
		ioUserEtcItem *pUserEtcItem = g_MyInfo.GetUserEtcItem();
		if( !pUserEtcItem )
		{
			ioButton *pButton = dynamic_cast<ioButton*>(FindChildWnd(ID_GO));
			if( pButton )
			{
				pButton->SetSize( ONE_BTN_SIZE, pButton->GetHeight() );
				pButton->SetWndPos( ONE_BTN_X, pButton->GetYPos() );

				ioUITitle *pTitle =  pButton->GetTitle();
				if( pTitle )
				{
					pTitle->SetOffset( ONE_TEXT_X, pTitle->GetTextYOffSet() );
				}

				pButton->ShowWnd();
			}
			break;
		}

		ioUserEtcItem::ETCITEMSLOT kSlot;
		if( !pUserEtcItem->GetEtcItem( m_dwEtcItemCode, kSlot) )
		{
			ioButton *pButton = dynamic_cast<ioButton*>(FindChildWnd(ID_GO));
			if( pButton )
			{
				pButton->SetSize( ONE_BTN_SIZE, pButton->GetHeight() );
				pButton->SetWndPos( ONE_BTN_X, pButton->GetYPos() );

				ioUITitle *pTitle =  pButton->GetTitle();
				if( pTitle )
				{
					pTitle->SetOffset( ONE_TEXT_X, pTitle->GetTextYOffSet() );
				}

				pButton->ShowWnd();
			}
		}
		else
		{
			ioButton *pButton = dynamic_cast<ioButton*>(FindChildWnd(ID_USE));
			if( pButton )
			{
				pButton->SetSize( TWO_BTN_SIZE, pButton->GetHeight() );
				pButton->SetWndPos( ONE_BTN_X, pButton->GetYPos() );

				ioUITitle *pTitle =  pButton->GetTitle();
				if( pTitle )
				{
					pTitle->SetOffset( TWO_TEXT_X, pTitle->GetTextYOffSet() );
				}

				pButton->ShowWnd();
			}

			pButton = dynamic_cast<ioButton*>(FindChildWnd(ID_GO));
			if( pButton )
			{
				pButton->SetSize( TWO_BTN_SIZE, pButton->GetHeight() );
				pButton->SetWndPos( TWO_BTN_X, pButton->GetYPos() );

				ioUITitle *pTitle =  pButton->GetTitle();
				if( pTitle )
				{
					pTitle->SetOffset( TWO_TEXT_X, pTitle->GetTextYOffSet() );
				}

				pButton->ShowWnd();
			}
		}
		break;
	}
}
//////////////////////////////////////////////////////////////////////////
ItemLuckyCoinResultWnd::ItemLuckyCoinResultWnd()
{
	m_pIcon = NULL;
	m_pEffect = NULL;

	m_pFinalBackEffect = NULL;
	m_pCostumeMark = NULL;

	m_bResult = false;
	m_bSuccess = false;

	m_iExchangeIndex = -1;
	m_iExchangeManual= 0;
	m_bExchangeCostume = false;

	m_pManual = NULL;

	m_nGradeType = 0;
}

ItemLuckyCoinResultWnd::~ItemLuckyCoinResultWnd()
{
	SAFEDELETE( m_pIcon );
	SAFEDELETE( m_pEffect );

	SAFEDELETE( m_pFinalBackEffect );

	SAFEDELETE( m_pCostumeMark );
	m_vEndEfffect_S.clear();
}

void ItemLuckyCoinResultWnd::iwm_hide()
{
	if( !m_szSuccessSound.IsEmpty() )
		g_SoundMgr.StopSound( m_szSuccessSound, 0 );
}

void ItemLuckyCoinResultWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "final_back_effect" )
	{
		SAFEDELETE( m_pFinalBackEffect );
		m_pFinalBackEffect = pImage;
	}
	else if( szType == "costume_image" )
	{
		SAFEDELETE( m_pCostumeMark );
		m_pCostumeMark = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}

void ItemLuckyCoinResultWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	char szKeyName[MAX_PATH]="";
	ioHashString szTemp;

	ZeroMemory( szKeyName, sizeof( szKeyName ) );
	StringCbPrintf_e( szKeyName, sizeof( szKeyName ), "rolling_gap" );
	m_dwRollingTime = xElement.GetIntAttribute( szKeyName );

	ZeroMemory( szKeyName, sizeof( szKeyName ) );
	StringCbPrintf_e( szKeyName, sizeof( szKeyName ), "rolling_duration" );
	m_dwRollingTotalTime = xElement.GetIntAttribute( szKeyName );

	ZeroMemory( szKeyName, sizeof( szKeyName ) );
	StringCbPrintf_e( szKeyName, sizeof( szKeyName ), "Icon_Scale" );
	m_fIconScale = xElement.GetFloatAttribute( szKeyName );

	ZeroMemory( szKeyName, sizeof( szKeyName ) );
	StringCbPrintf_e( szKeyName, sizeof( szKeyName ), "Icon_Scale_Time" );
	m_dwIconScaleTime = xElement.GetIntAttribute( szKeyName );

	ZeroMemory( szKeyName, sizeof( szKeyName ) );
	StringCbPrintf_e( szKeyName, sizeof( szKeyName ), "success_sound" );
	m_szSuccessSound = xElement.GetStringAttribute( szKeyName );

	m_dwRollingEffectTime = xElement.GetIntAttribute_e( "Effect_Gap_Time" );

	m_vEndEfffect_S.clear();
	int iMax = xElement.GetIntAttribute_e( "Max_Success_Effect" );
	m_vEndEfffect_S.reserve( iMax );
	for (int i = 0; i < iMax ; i++)
	{
		char szName[MAX_PATH]="";
		StringCbPrintf( szName, sizeof( szName ), "Success_Effect_%d", i+1 );
		ioHashString szEffect = xElement.GetStringAttribute( szName );
		if( szEffect == "" )
			break;
		m_vEndEfffect_S.push_back( szEffect );
	}
}

void ItemLuckyCoinResultWnd::SetItemLuckyCoinResultInfo( DWORD dwCode, int iSelectIndex )
{
	m_bResult = true;
	m_bSuccess = true;
	m_dwEtcItemCode = dwCode;
	ioEtcItemLuckyCoin *pEtcItem = dynamic_cast< ioEtcItemLuckyCoin * > (g_EtcItemMgr.FindEtcItem(m_dwEtcItemCode));
	if( pEtcItem == NULL ) return;

	m_pManual = ioEtcInvenSubManualMgr::GetSingletonPtr();

	m_nGradeType = 0;

	SAFEDELETE(m_pIcon);
	SAFEDELETE(m_pEffect);

	ShowWnd();

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwRollingEndTime = dwCurTime;
	m_dwRollingCheckTime = 0;
	m_dwRollingEffectCheckTime = 0;

	m_iCurEffectArray = -1;
	m_iExchangeIndex = iSelectIndex;
	m_bExchangeCostume = false;

	pEtcItem->GetExchangeResultData( m_iExchangeIndex, m_iExchangeManual, m_szExchangeName, m_szExchangeIcon, m_bExchangeCostume, m_nGradeType );

	m_pIcon = g_UIImageSetMgr.CreateImageByFullName( m_szExchangeIcon );

	ReBatchButton( RESULT_BTN );

	if( !m_szSuccessSound.IsEmpty() )
		g_SoundMgr.PlaySound( m_szSuccessSound, DSBVOLUME_MAX, PSM_THREAD );
}

void ItemLuckyCoinResultWnd::OnProcess( float fTimePerSec )
{
	ioWnd::OnProcess( fTimePerSec );

	if( !IsShow() ) return;

	RollingEffect( fTimePerSec );
}

void ItemLuckyCoinResultWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	enum
	{
		ICON_X		= 129,
		ICON_Y		= 248,
		FINAL_DESC_X	= 128,
		FINAL_DESC_Y	= 421,
	};

	// Back
	if( m_pFinalBackEffect )
		m_pFinalBackEffect->Render( iXPos+2, iYPos+39 );

	g_UIImageRenderMgr.RenderGradeIconBack( m_nGradeType, iXPos+ICON_X, iYPos+ICON_Y );

	// Icon
	if( m_pIcon )
	{
		m_pIcon->Render( iXPos+ICON_X, iYPos+ICON_Y, UI_RENDER_NORMAL, TFO_BILINEAR );
	}

	// UI코스튬 관련 (코스튬 마크)
	if( m_bExchangeCostume && m_pCostumeMark )
		m_pCostumeMark->Render( iXPos+ICON_X, iYPos+ICON_Y, UI_RENDER_NORMAL, TFO_BILINEAR );

	// Effect
	if( m_pEffect )
	{
		m_pEffect->SetScale( 2.0f );
		m_pEffect->Render( iXPos+ICON_X, iYPos+ICON_Y, UI_RENDER_ADD );
	}

	// Reinforce
	enum
	{
		REINFORCE_Y	= 14,

		TEXT_SIZE	= 14,
		TEXT_GAP	= 4,
	};

	if( m_bResult )
	{
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_GREEN );
		g_FontMgr.PrintText( iXPos+FINAL_DESC_X, iYPos+FINAL_DESC_Y, FONT_SIZE_17, STR(1) );
	}

	// Name
	ioComplexStringPrinter kPrinter;

	ioHashString szName = m_szExchangeName;

	kPrinter.ClearList();
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );	
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_17, szName.c_str() );
	kPrinter.PrintFullText( iXPos+131, iYPos+308, TAT_CENTER );

	PrintManual( iXPos+33, iYPos+352, FONT_SIZE_12 );
}

void ItemLuckyCoinResultWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_GO:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();

			MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
			if( pInvenWnd )
			{
				pInvenWnd->ShowPresentTabDirect();
			}
		}
		break;
	case ID_USE:
		if( cmd == IOBN_BTNUP )
		{
			ioUserEtcItem *pUserEtcItem =  g_MyInfo.GetUserEtcItem();
			if( pUserEtcItem )
			{
				ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( m_dwEtcItemCode );
				if( !pEtcItem )
				{
					LOG.PrintTimeAndLog( 0, "%s pEtcItem == NULL : %d", __FUNCTION__, m_dwEtcItemCode );
					return;
				}
				HideWnd();

				pEtcItem->OnUseBtnPush( m_dwEtcItemCode, pUserEtcItem );
			}
		}
		break;
	}
}

void ItemLuckyCoinResultWnd::PrintManual( int a_iXPos, int a_iYPos , float fScale )
{
	if( !m_pManual ) return;
	if( m_iExchangeManual == -1 ) return;

	int iSubManual = m_iExchangeManual - 1;
	ioComplexStringPrinter kPrinter;
	int iMaxLine = m_pManual->GetMaxLine( iSubManual );
	for(int iLine = 1; iLine < iMaxLine; iLine++)
	{
		m_pManual->SetPrinter( iSubManual, iLine, fScale, kPrinter);
		a_iYPos += m_pManual->GetSpaceLine( iSubManual, iLine);
		kPrinter.PrintFullTextWidthCut( a_iXPos, a_iYPos, TAT_LEFT, DESC_WIDTH );
		kPrinter.ClearList();
	}
}

void ItemLuckyCoinResultWnd::RollingEffect( float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();

	int iEffectListCnt = m_vEndEfffect_S.size();

	if( iEffectListCnt == 0 )
	{
		SAFEDELETE( m_pEffect );
		return;
	}

	if( m_iCurEffectArray == iEffectListCnt-1 )
	{
		SAFEDELETE( m_pEffect );
		return;
	}

	if( m_dwRollingEffectCheckTime == 0 || dwCurTime - m_dwRollingEffectCheckTime > m_dwRollingEffectTime )		// 교체
	{
		SAFEDELETE( m_pEffect );

		m_iCurEffectArray++;

		if( !COMPARE( m_iCurEffectArray, 0, iEffectListCnt ) )
			m_iCurEffectArray = 0;

		ioHashString szEffect = m_vEndEfffect_S[m_iCurEffectArray];

		if( !szEffect.IsEmpty() )
		{
			m_pEffect = g_UIImageSetMgr.CreateImageByFullName( szEffect );
		}

		m_dwRollingEffectCheckTime = dwCurTime;
	}
}

void ItemLuckyCoinResultWnd::ReBatchButton( int iType )
{
	enum
	{
		ONE_BTN_SIZE	= 204,
		TWO_BTN_SIZE	= 102,

		ONE_BTN_X		= 27,
		TWO_BTN_X		= 131,

		ONE_TEXT_X		= 102,
		TWO_TEXT_X		= 51,
	};

	switch( iType )
	{
	case ROLLING_BTN:
		ShowChildWnd(ID_ROLLING);
		SetChildInActive(ID_ROLLING);

		HideChildWnd(ID_USE);
		HideChildWnd(ID_GO);
		break;
	case RESULT_BTN:
		HideChildWnd(ID_ROLLING);
		HideChildWnd(ID_USE);
		HideChildWnd(ID_GO);

		// 예외처리
		ioUserEtcItem *pUserEtcItem = g_MyInfo.GetUserEtcItem();
		if( !pUserEtcItem )
		{
			ioButton *pButton = dynamic_cast<ioButton*>(FindChildWnd(ID_GO));
			if( pButton )
			{
				pButton->SetSize( ONE_BTN_SIZE, pButton->GetHeight() );
				pButton->SetWndPos( ONE_BTN_X, pButton->GetYPos() );

				ioUITitle *pTitle =  pButton->GetTitle();
				if( pTitle )
				{
					pTitle->SetOffset( ONE_TEXT_X, pTitle->GetTextYOffSet() );
				}

				pButton->ShowWnd();
			}
			break;
		}

		ioUserEtcItem::ETCITEMSLOT kSlot;
		if( !pUserEtcItem->GetEtcItem( m_dwEtcItemCode, kSlot) )
		{
			ioButton *pButton = dynamic_cast<ioButton*>(FindChildWnd(ID_GO));
			if( pButton )
			{
				pButton->SetSize( ONE_BTN_SIZE, pButton->GetHeight() );
				pButton->SetWndPos( ONE_BTN_X, pButton->GetYPos() );

				ioUITitle *pTitle =  pButton->GetTitle();
				if( pTitle )
				{
					pTitle->SetOffset( ONE_TEXT_X, pTitle->GetTextYOffSet() );
				}

				pButton->ShowWnd();
			}
		}
		else
		{
			ioButton *pButton = dynamic_cast<ioButton*>(FindChildWnd(ID_USE));
			if( pButton )
			{
				pButton->SetSize( TWO_BTN_SIZE, pButton->GetHeight() );
				pButton->SetWndPos( ONE_BTN_X, pButton->GetYPos() );

				ioUITitle *pTitle =  pButton->GetTitle();
				if( pTitle )
				{
					pTitle->SetOffset( TWO_TEXT_X, pTitle->GetTextYOffSet() );
				}

				pButton->ShowWnd();
			}

			pButton = dynamic_cast<ioButton*>(FindChildWnd(ID_GO));
			if( pButton )
			{
				pButton->SetSize( TWO_BTN_SIZE, pButton->GetHeight() );
				pButton->SetWndPos( TWO_BTN_X, pButton->GetYPos() );

				ioUITitle *pTitle =  pButton->GetTitle();
				if( pTitle )
				{
					pTitle->SetOffset( TWO_TEXT_X, pTitle->GetTextYOffSet() );
				}

				pButton->ShowWnd();
			}
		}
		break;
	}
}
//////////////////////////////////////////////////////////////////////////
ItemRainbowMixerResultWnd::ItemRainbowMixerResultWnd()
{
	m_pIcon = NULL;
	m_pEffect = NULL;

	m_pFinalBackEffect = NULL;

	m_bResult = false;
	m_bSuccess = false;

	m_pManual = NULL;
}

ItemRainbowMixerResultWnd::~ItemRainbowMixerResultWnd()
{
	SAFEDELETE( m_pIcon );
	SAFEDELETE( m_pEffect );

	SAFEDELETE( m_pFinalBackEffect );

	m_vEndEfffect_S.clear();
}

void ItemRainbowMixerResultWnd::iwm_hide()
{
	if( !m_szSuccessSound.IsEmpty() )
		g_SoundMgr.StopSound( m_szSuccessSound, 0 );
}

void ItemRainbowMixerResultWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "final_back_effect" )
	{
		SAFEDELETE( m_pFinalBackEffect );
		m_pFinalBackEffect = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}

void ItemRainbowMixerResultWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	char szKeyName[MAX_PATH]="";
	ioHashString szTemp;

	ZeroMemory( szKeyName, sizeof( szKeyName ) );
	StringCbPrintf_e( szKeyName, sizeof( szKeyName ), "rolling_gap" );
	m_dwRollingTime = xElement.GetIntAttribute( szKeyName );

	ZeroMemory( szKeyName, sizeof( szKeyName ) );
	StringCbPrintf_e( szKeyName, sizeof( szKeyName ), "rolling_duration" );
	m_dwRollingTotalTime = xElement.GetIntAttribute( szKeyName );

	ZeroMemory( szKeyName, sizeof( szKeyName ) );
	StringCbPrintf_e( szKeyName, sizeof( szKeyName ), "Icon_Scale" );
	m_fIconScale = xElement.GetFloatAttribute( szKeyName );

	ZeroMemory( szKeyName, sizeof( szKeyName ) );
	StringCbPrintf_e( szKeyName, sizeof( szKeyName ), "Icon_Scale_Time" );
	m_dwIconScaleTime = xElement.GetIntAttribute( szKeyName );

	ZeroMemory( szKeyName, sizeof( szKeyName ) );
	StringCbPrintf_e( szKeyName, sizeof( szKeyName ), "success_sound" );
	m_szSuccessSound = xElement.GetStringAttribute( szKeyName );

	m_dwRollingEffectTime = xElement.GetIntAttribute_e( "Effect_Gap_Time" );

	m_vEndEfffect_S.clear();
	int iMax = xElement.GetIntAttribute_e( "Max_Success_Effect" );
	m_vEndEfffect_S.reserve( iMax );
	for (int i = 0; i < iMax ; i++)
	{
		char szName[MAX_PATH]="";
		StringCbPrintf( szName, sizeof( szName ), "Success_Effect_%d", i+1 );
		ioHashString szEffect = xElement.GetStringAttribute( szName );
		if( szEffect == "" )
			break;
		m_vEndEfffect_S.push_back( szEffect );
	}
}

void ItemRainbowMixerResultWnd::SetInfoAndShow( DWORD dwCode, int iSelectIndex )
{
	m_bResult = true;
	m_bSuccess = true;
	m_dwEtcItemCode = dwCode;
	ioEtcItemRainbowMixer *pEtcItem = dynamic_cast< ioEtcItemRainbowMixer * > (g_EtcItemMgr.FindEtcItem(m_dwEtcItemCode));
	if( pEtcItem == NULL ) return;

	m_pManual = ioEtcInvenSubManualMgr::GetSingletonPtr();

	SAFEDELETE(m_pIcon);
	SAFEDELETE(m_pEffect);

	ShowWnd();

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwRollingEndTime = dwCurTime;
	m_dwRollingCheckTime = 0;
	m_dwRollingEffectCheckTime = 0;

	m_iCurEffectArray = -1;
	ioHashString kPresentIcon;
	pEtcItem->GetPatternResultData( iSelectIndex + 1, kPresentIcon, m_szPresentName );

	m_pIcon = g_UIImageSetMgr.CreateImageByFullName( kPresentIcon );

	ReBatchButton( RESULT_BTN );

	if( !m_szSuccessSound.IsEmpty() )
		g_SoundMgr.PlaySound( m_szSuccessSound );
}

void ItemRainbowMixerResultWnd::OnProcess( float fTimePerSec )
{
	ioWnd::OnProcess( fTimePerSec );

	if( !IsShow() ) return;

	RollingEffect( fTimePerSec );
}

void ItemRainbowMixerResultWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	enum
	{
		ICON_X		= 129,
		ICON_Y		= 105,
		FINAL_DESC_X	= 128,
		FINAL_DESC_Y	= 421,
	};

	// Back
	if( m_pFinalBackEffect )
		m_pFinalBackEffect->Render( iXPos+ICON_X, iYPos+ICON_Y, UI_RENDER_MULTIPLY );

	// Icon
	if( m_pIcon )
	{
		m_pIcon->Render( iXPos+ICON_X, iYPos+ICON_Y, UI_RENDER_NORMAL, TFO_BILINEAR );
	}

	// Effect
	if( m_pEffect )
	{
		m_pEffect->SetScale( 2.0f );
		m_pEffect->Render( iXPos+ICON_X, iYPos+ICON_Y, UI_RENDER_ADD );
	}

	// Reinforce
	enum
	{
		REINFORCE_Y	= 14,

		TEXT_SIZE	= 14,
		TEXT_GAP	= 4,
	};

	if( m_bResult )
	{
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_GREEN );
		g_FontMgr.PrintText( iXPos+FINAL_DESC_X, iYPos+FINAL_DESC_Y, FONT_SIZE_17, STR(1) );
	}

	// Name
	ioComplexStringPrinter kPrinter;

	ioHashString szName = m_szPresentName;

	kPrinter.ClearList();
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );	
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_17, szName.c_str() );
	kPrinter.PrintFullText( iXPos+129, iYPos+152, TAT_CENTER );

	PrintManual( iXPos+27, iYPos+206, FONT_SIZE_12 );
}

void ItemRainbowMixerResultWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_GO:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();

			MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
			if( pInvenWnd )
			{
				pInvenWnd->ShowPresentTabDirect();
			}
		}
		break;
	case ID_USE:
		if( cmd == IOBN_BTNUP )
		{
			ioUserEtcItem *pUserEtcItem =  g_MyInfo.GetUserEtcItem();
			if( pUserEtcItem )
			{
				ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( m_dwEtcItemCode );
				if( !pEtcItem )
				{
					LOG.PrintTimeAndLog( 0, "%s pEtcItem == NULL : %d", __FUNCTION__, m_dwEtcItemCode );
					return;
				}
				HideWnd();

				pEtcItem->OnUseBtnPush( m_dwEtcItemCode, pUserEtcItem );
			}
		}
		break;
	}
}

void ItemRainbowMixerResultWnd::PrintManual( int a_iXPos, int a_iYPos , float fScale )
{
	if( !m_pManual ) return;
	
	ioEtcItemRainbowMixer *pEtcItem = dynamic_cast< ioEtcItemRainbowMixer * >(g_EtcItemMgr.FindEtcItem( m_dwEtcItemCode ));

	if( !pEtcItem ) return;

	int iSubManual = pEtcItem->GetSubResultManual() - 1;
	ioComplexStringPrinter kPrinter;
	int iMaxLine = m_pManual->GetMaxLine( iSubManual );
	for(int iLine = 1; iLine < iMaxLine; iLine++)
	{
		m_pManual->SetPrinter( iSubManual, iLine, fScale, kPrinter);
		a_iYPos += m_pManual->GetSpaceLine( iSubManual, iLine);
		kPrinter.PrintFullTextWidthCut( a_iXPos, a_iYPos, TAT_LEFT, DESC_WIDTH );
		kPrinter.ClearList();
	}
}

void ItemRainbowMixerResultWnd::RollingEffect( float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();

	int iEffectListCnt = m_vEndEfffect_S.size();

	if( iEffectListCnt == 0 )
	{
		SAFEDELETE( m_pEffect );
		return;
	}

	if( m_iCurEffectArray == iEffectListCnt-1 )
	{
		SAFEDELETE( m_pEffect );
		return;
	}

	if( m_dwRollingEffectCheckTime == 0 || dwCurTime - m_dwRollingEffectCheckTime > m_dwRollingEffectTime )		// 교체
	{
		SAFEDELETE( m_pEffect );

		m_iCurEffectArray++;

		if( !COMPARE( m_iCurEffectArray, 0, iEffectListCnt ) )
			m_iCurEffectArray = 0;

		ioHashString szEffect = m_vEndEfffect_S[m_iCurEffectArray];

		if( !szEffect.IsEmpty() )
		{
			m_pEffect = g_UIImageSetMgr.CreateImageByFullName( szEffect );
		}

		m_dwRollingEffectCheckTime = dwCurTime;
	}
}

void ItemRainbowMixerResultWnd::ReBatchButton( int iType )
{
	enum
	{
		ONE_BTN_SIZE	= 204,
		TWO_BTN_SIZE	= 102,

		ONE_BTN_X		= 27,
		TWO_BTN_X		= 131,

		ONE_TEXT_X		= 102,
		TWO_TEXT_X		= 51,
	};

	switch( iType )
	{
	case ROLLING_BTN:
		ShowChildWnd(ID_ROLLING);
		SetChildInActive(ID_ROLLING);

		HideChildWnd(ID_USE);
		HideChildWnd(ID_GO);
		break;
	case RESULT_BTN:
		HideChildWnd(ID_ROLLING);
		HideChildWnd(ID_USE);
		HideChildWnd(ID_GO);

		// 예외처리
		ioUserEtcItem *pUserEtcItem = g_MyInfo.GetUserEtcItem();
		if( !pUserEtcItem )
		{
			ioButton *pButton = dynamic_cast<ioButton*>(FindChildWnd(ID_GO));
			if( pButton )
			{
				pButton->SetSize( ONE_BTN_SIZE, pButton->GetHeight() );
				pButton->SetWndPos( ONE_BTN_X, pButton->GetYPos() );

				ioUITitle *pTitle =  pButton->GetTitle();
				if( pTitle )
				{
					pTitle->SetOffset( ONE_TEXT_X, pTitle->GetTextYOffSet() );
				}

				pButton->ShowWnd();
			}
			break;
		}

		ioUserEtcItem::ETCITEMSLOT kSlot;
		if( !pUserEtcItem->GetEtcItem( m_dwEtcItemCode, kSlot) )
		{
			ioButton *pButton = dynamic_cast<ioButton*>(FindChildWnd(ID_GO));
			if( pButton )
			{
				pButton->SetSize( ONE_BTN_SIZE, pButton->GetHeight() );
				pButton->SetWndPos( ONE_BTN_X, pButton->GetYPos() );

				ioUITitle *pTitle =  pButton->GetTitle();
				if( pTitle )
				{
					pTitle->SetOffset( ONE_TEXT_X, pTitle->GetTextYOffSet() );
				}

				pButton->ShowWnd();
			}
		}
		else
		{
			ioButton *pButton = dynamic_cast<ioButton*>(FindChildWnd(ID_USE));
			if( pButton )
			{
				pButton->SetSize( TWO_BTN_SIZE, pButton->GetHeight() );
				pButton->SetWndPos( ONE_BTN_X, pButton->GetYPos() );

				ioUITitle *pTitle =  pButton->GetTitle();
				if( pTitle )
				{
					pTitle->SetOffset( TWO_TEXT_X, pTitle->GetTextYOffSet() );
				}

				pButton->ShowWnd();
			}

			pButton = dynamic_cast<ioButton*>(FindChildWnd(ID_GO));
			if( pButton )
			{
				pButton->SetSize( TWO_BTN_SIZE, pButton->GetHeight() );
				pButton->SetWndPos( TWO_BTN_X, pButton->GetYPos() );

				ioUITitle *pTitle =  pButton->GetTitle();
				if( pTitle )
				{
					pTitle->SetOffset( TWO_TEXT_X, pTitle->GetTextYOffSet() );
				}

				pButton->ShowWnd();
			}
		}
		break;
	}
}
//////////////////////////////////////////////////////////////////////////
ItemLostSagaMixerResultWnd::ItemLostSagaMixerResultWnd()
{
	m_pIcon = NULL;
	m_pEffect = NULL;

	m_pFinalBackEffect = NULL;

	m_bResult = false;
	m_bSuccess = false;

	m_pManual = NULL;
}

ItemLostSagaMixerResultWnd::~ItemLostSagaMixerResultWnd()
{
	SAFEDELETE( m_pIcon );
	SAFEDELETE( m_pEffect );

	SAFEDELETE( m_pFinalBackEffect );

	m_vEndEfffect_S.clear();
}

void ItemLostSagaMixerResultWnd::iwm_hide()
{
	if( !m_szSuccessSound.IsEmpty() )
		g_SoundMgr.StopSound( m_szSuccessSound, 0 );
}

void ItemLostSagaMixerResultWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "final_back_effect" )
	{
		SAFEDELETE( m_pFinalBackEffect );
		m_pFinalBackEffect = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}

void ItemLostSagaMixerResultWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	char szKeyName[MAX_PATH]="";
	ioHashString szTemp;

	ZeroMemory( szKeyName, sizeof( szKeyName ) );
	StringCbPrintf_e( szKeyName, sizeof( szKeyName ), "rolling_gap" );
	m_dwRollingTime = xElement.GetIntAttribute( szKeyName );

	ZeroMemory( szKeyName, sizeof( szKeyName ) );
	StringCbPrintf_e( szKeyName, sizeof( szKeyName ), "rolling_duration" );
	m_dwRollingTotalTime = xElement.GetIntAttribute( szKeyName );

	ZeroMemory( szKeyName, sizeof( szKeyName ) );
	StringCbPrintf_e( szKeyName, sizeof( szKeyName ), "Icon_Scale" );
	m_fIconScale = xElement.GetFloatAttribute( szKeyName );

	ZeroMemory( szKeyName, sizeof( szKeyName ) );
	StringCbPrintf_e( szKeyName, sizeof( szKeyName ), "Icon_Scale_Time" );
	m_dwIconScaleTime = xElement.GetIntAttribute( szKeyName );

	ZeroMemory( szKeyName, sizeof( szKeyName ) );
	StringCbPrintf_e( szKeyName, sizeof( szKeyName ), "success_sound" );
	m_szSuccessSound = xElement.GetStringAttribute( szKeyName );

	m_dwRollingEffectTime = xElement.GetIntAttribute_e( "Effect_Gap_Time" );

	m_vEndEfffect_S.clear();
	int iMax = xElement.GetIntAttribute_e( "Max_Success_Effect" );
	m_vEndEfffect_S.reserve( iMax );
	for (int i = 0; i < iMax ; i++)
	{
		char szName[MAX_PATH]="";
		StringCbPrintf( szName, sizeof( szName ), "Success_Effect_%d", i+1 );
		ioHashString szEffect = xElement.GetStringAttribute( szName );
		if( szEffect == "" )
			break;
		m_vEndEfffect_S.push_back( szEffect );
	}
}

void ItemLostSagaMixerResultWnd::SetInfoAndShow( DWORD dwCode, int iSelectIndex )
{
	m_bResult = true;
	m_bSuccess = true;
	m_dwEtcItemCode = dwCode;
	ioEtcItemLostSagaMixer *pEtcItem = dynamic_cast< ioEtcItemLostSagaMixer * > (g_EtcItemMgr.FindEtcItem(m_dwEtcItemCode));
	if( pEtcItem == NULL ) return;

	m_pManual = ioEtcInvenSubManualMgr::GetSingletonPtr();

	SAFEDELETE(m_pIcon);
	SAFEDELETE(m_pEffect);

	ShowWnd();

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwRollingEndTime = dwCurTime;
	m_dwRollingCheckTime = 0;
	m_dwRollingEffectCheckTime = 0;

	m_iCurEffectArray = -1;
	ioHashString kPresentIcon;
	pEtcItem->GetPatternResultData( iSelectIndex + 1, kPresentIcon, m_szPresentName );

	m_pIcon = g_UIImageSetMgr.CreateImageByFullName( kPresentIcon );

	ReBatchButton( RESULT_BTN );

	if( !m_szSuccessSound.IsEmpty() )
		g_SoundMgr.PlaySound( m_szSuccessSound );
}

void ItemLostSagaMixerResultWnd::OnProcess( float fTimePerSec )
{
	ioWnd::OnProcess( fTimePerSec );

	if( !IsShow() ) return;

	RollingEffect( fTimePerSec );
}

void ItemLostSagaMixerResultWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	enum
	{
		ICON_X		= 129,
		ICON_Y		= 105,
		FINAL_DESC_X	= 128,
		FINAL_DESC_Y	= 421,
	};

	// Back
	if( m_pFinalBackEffect )
		m_pFinalBackEffect->Render( iXPos+ICON_X, iYPos+ICON_Y, UI_RENDER_MULTIPLY );

	// Icon
	if( m_pIcon )
	{
		m_pIcon->Render( iXPos+ICON_X, iYPos+ICON_Y, UI_RENDER_NORMAL, TFO_BILINEAR );
	}

	// Effect
	if( m_pEffect )
	{
		m_pEffect->SetScale( 2.0f );
		m_pEffect->Render( iXPos+ICON_X, iYPos+ICON_Y, UI_RENDER_ADD );
	}

	// Reinforce
	enum
	{
		REINFORCE_Y	= 14,

		TEXT_SIZE	= 14,
		TEXT_GAP	= 4,
	};

	if( m_bResult )
	{
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_GREEN );
		g_FontMgr.PrintText( iXPos+FINAL_DESC_X, iYPos+FINAL_DESC_Y, FONT_SIZE_17, STR(1) );
	}

	// Name
	ioComplexStringPrinter kPrinter;

	ioHashString szName = m_szPresentName;

	kPrinter.ClearList();
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );	
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_17, szName.c_str() );
	kPrinter.PrintFullText( iXPos+129, iYPos+152, TAT_CENTER );

	PrintManual( iXPos+27, iYPos+206, FONT_SIZE_12 );
}

void ItemLostSagaMixerResultWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_GO:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();

			MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
			if( pInvenWnd )
			{
				pInvenWnd->ShowPresentTabDirect();
			}
		}
		break;
	case ID_USE:
		if( cmd == IOBN_BTNUP )
		{
			ioUserEtcItem *pUserEtcItem =  g_MyInfo.GetUserEtcItem();
			if( pUserEtcItem )
			{
				ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( m_dwEtcItemCode );
				if( !pEtcItem )
				{
					LOG.PrintTimeAndLog( 0, "%s pEtcItem == NULL : %d", __FUNCTION__, m_dwEtcItemCode );
					return;
				}
				HideWnd();

				pEtcItem->OnUseBtnPush( m_dwEtcItemCode, pUserEtcItem );
			}
		}
		break;
	}
}

void ItemLostSagaMixerResultWnd::PrintManual( int a_iXPos, int a_iYPos , float fScale )
{
	if( !m_pManual ) return;

	ioEtcItemLostSagaMixer *pEtcItem = dynamic_cast< ioEtcItemLostSagaMixer * >(g_EtcItemMgr.FindEtcItem( m_dwEtcItemCode ));

	if( !pEtcItem ) return;

	int iSubManual = pEtcItem->GetSubResultManual() - 1;
	ioComplexStringPrinter kPrinter;
	int iMaxLine = m_pManual->GetMaxLine( iSubManual );
	for(int iLine = 1; iLine < iMaxLine; iLine++)
	{
		m_pManual->SetPrinter( iSubManual, iLine, fScale, kPrinter);
		a_iYPos += m_pManual->GetSpaceLine( iSubManual, iLine);
		kPrinter.PrintFullTextWidthCut( a_iXPos, a_iYPos, TAT_LEFT, DESC_WIDTH );
		kPrinter.ClearList();
	}
}

void ItemLostSagaMixerResultWnd::RollingEffect( float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();

	int iEffectListCnt = m_vEndEfffect_S.size();

	if( iEffectListCnt == 0 )
	{
		SAFEDELETE( m_pEffect );
		return;
	}

	if( m_iCurEffectArray == iEffectListCnt-1 )
	{
		SAFEDELETE( m_pEffect );
		return;
	}

	if( m_dwRollingEffectCheckTime == 0 || dwCurTime - m_dwRollingEffectCheckTime > m_dwRollingEffectTime )		// 교체
	{
		SAFEDELETE( m_pEffect );

		m_iCurEffectArray++;

		if( !COMPARE( m_iCurEffectArray, 0, iEffectListCnt ) )
			m_iCurEffectArray = 0;

		ioHashString szEffect = m_vEndEfffect_S[m_iCurEffectArray];

		if( !szEffect.IsEmpty() )
		{
			m_pEffect = g_UIImageSetMgr.CreateImageByFullName( szEffect );
		}

		m_dwRollingEffectCheckTime = dwCurTime;
	}
}

void ItemLostSagaMixerResultWnd::ReBatchButton( int iType )
{
	enum
	{
		ONE_BTN_SIZE	= 204,
		TWO_BTN_SIZE	= 102,

		ONE_BTN_X		= 27,
		TWO_BTN_X		= 131,

		ONE_TEXT_X		= 102,
		TWO_TEXT_X		= 51,
	};

	switch( iType )
	{
	case ROLLING_BTN:
		ShowChildWnd(ID_ROLLING);
		SetChildInActive(ID_ROLLING);

		HideChildWnd(ID_USE);
		HideChildWnd(ID_GO);
		break;
	case RESULT_BTN:
		HideChildWnd(ID_ROLLING);
		HideChildWnd(ID_USE);
		HideChildWnd(ID_GO);

		// 예외처리
		ioUserEtcItem *pUserEtcItem = g_MyInfo.GetUserEtcItem();
		if( !pUserEtcItem )
		{
			ioButton *pButton = dynamic_cast<ioButton*>(FindChildWnd(ID_GO));
			if( pButton )
			{
				pButton->SetSize( ONE_BTN_SIZE, pButton->GetHeight() );
				pButton->SetWndPos( ONE_BTN_X, pButton->GetYPos() );

				ioUITitle *pTitle =  pButton->GetTitle();
				if( pTitle )
				{
					pTitle->SetOffset( ONE_TEXT_X, pTitle->GetTextYOffSet() );
				}

				pButton->ShowWnd();
			}
			break;
		}

		ioUserEtcItem::ETCITEMSLOT kSlot;
		if( !pUserEtcItem->GetEtcItem( m_dwEtcItemCode, kSlot) )
		{
			ioButton *pButton = dynamic_cast<ioButton*>(FindChildWnd(ID_GO));
			if( pButton )
			{
				pButton->SetSize( ONE_BTN_SIZE, pButton->GetHeight() );
				pButton->SetWndPos( ONE_BTN_X, pButton->GetYPos() );

				ioUITitle *pTitle =  pButton->GetTitle();
				if( pTitle )
				{
					pTitle->SetOffset( ONE_TEXT_X, pTitle->GetTextYOffSet() );
				}

				pButton->ShowWnd();
			}
		}
		else
		{
			ioButton *pButton = dynamic_cast<ioButton*>(FindChildWnd(ID_USE));
			if( pButton )
			{
				pButton->SetSize( TWO_BTN_SIZE, pButton->GetHeight() );
				pButton->SetWndPos( ONE_BTN_X, pButton->GetYPos() );

				ioUITitle *pTitle =  pButton->GetTitle();
				if( pTitle )
				{
					pTitle->SetOffset( TWO_TEXT_X, pTitle->GetTextYOffSet() );
				}

				pButton->ShowWnd();
			}

			pButton = dynamic_cast<ioButton*>(FindChildWnd(ID_GO));
			if( pButton )
			{
				pButton->SetSize( TWO_BTN_SIZE, pButton->GetHeight() );
				pButton->SetWndPos( TWO_BTN_X, pButton->GetYPos() );

				ioUITitle *pTitle =  pButton->GetTitle();
				if( pTitle )
				{
					pTitle->SetOffset( TWO_TEXT_X, pTitle->GetTextYOffSet() );
				}

				pButton->ShowWnd();
			}
		}
		break;
	}
}
//////////////////////////////////////////////////////////////////////////
AccessoryCompoundResultWnd::AccessoryCompoundResultWnd()
{
	m_pIcon = NULL;
	m_pEffect = NULL;
	m_pBackEffect = NULL;
	m_pFinalBackEffect = NULL;

	m_bResult = false;

	m_iTargetIndex = 0;
	m_iCompoundCode = 0;
	m_iCompoundValue = 0;
	m_iItemCode = 0;

	m_pManual = NULL;
}

AccessoryCompoundResultWnd::~AccessoryCompoundResultWnd()
{
	SAFEDELETE( m_pIcon );
	SAFEDELETE( m_pEffect );
	SAFEDELETE( m_pBackEffect );
	SAFEDELETE( m_pFinalBackEffect );

	m_vRollingEffect.clear();
	m_vEndEfffect.clear();
}

void AccessoryCompoundResultWnd::iwm_hide()
{
	if( !m_szRollingSound.IsEmpty() )
		g_SoundMgr.StopSound( m_szRollingSound, 0 );

	if( !m_szSuccessSound.IsEmpty() )
		g_SoundMgr.StopSound( m_szSuccessSound, 0 );

	g_QuestMgr.ProcessCompleteReserve();
}

void AccessoryCompoundResultWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "back_effect" )
	{
		SAFEDELETE( m_pBackEffect );
		m_pBackEffect = pImage;
	}
	else if( szType == "final_back_effect" )
	{
		SAFEDELETE( m_pFinalBackEffect );
		m_pFinalBackEffect = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}

void AccessoryCompoundResultWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	char szKeyName[MAX_PATH]="";
	ioHashString szTemp;

	ZeroMemory( szKeyName, sizeof( szKeyName ) );
	StringCbPrintf_e( szKeyName, sizeof( szKeyName ), "rolling_gap" );
	m_dwRollingTime = xElement.GetIntAttribute( szKeyName );

	ZeroMemory( szKeyName, sizeof( szKeyName ) );
	StringCbPrintf_e( szKeyName, sizeof( szKeyName ), "rolling_duration" );
	m_dwRollingTotalTime = xElement.GetIntAttribute( szKeyName );

	ZeroMemory( szKeyName, sizeof( szKeyName ) );
	StringCbPrintf_e( szKeyName, sizeof( szKeyName ), "Icon_Scale" );
	m_fIconScale = xElement.GetFloatAttribute( szKeyName );

	ZeroMemory( szKeyName, sizeof( szKeyName ) );
	StringCbPrintf_e( szKeyName, sizeof( szKeyName ), "Icon_Scale_Time" );
	m_dwIconScaleTime = xElement.GetIntAttribute( szKeyName );

	ZeroMemory( szKeyName, sizeof( szKeyName ) );
	StringCbPrintf_e( szKeyName, sizeof( szKeyName ), "rolling_sound" );
	m_szRollingSound = xElement.GetStringAttribute( szKeyName );

	ZeroMemory( szKeyName, sizeof( szKeyName ) );
	StringCbPrintf_e( szKeyName, sizeof( szKeyName ), "success_sound" );
	m_szSuccessSound = xElement.GetStringAttribute( szKeyName );

	ZeroMemory( szKeyName, sizeof( szKeyName ) );
	StringCbPrintf_e( szKeyName, sizeof( szKeyName ), "rolling_msg" );
	m_szRollingMsg = xElement.GetStringAttribute( szKeyName );

	ZeroMemory( szKeyName, sizeof( szKeyName ) );
	StringCbPrintf_e( szKeyName, sizeof( szKeyName ), "result_msg" );
	m_szResultMsg = xElement.GetStringAttribute( szKeyName );

	m_dwRollingEffectTime = xElement.GetIntAttribute_e( "Effect_Gap_Time" );
	m_vRollingEffect.clear();

	int iMax = xElement.GetIntAttribute_e( "Max_Rolling_Effect" );
	m_vRollingEffect.reserve( iMax );
	for (int i = 0; i < iMax ; i++)
	{
		char szName[MAX_PATH]="";
		StringCbPrintf( szName, sizeof( szName ), "Rolling_Effect_%d", i+1 );
		ioHashString szEffect = xElement.GetStringAttribute( szName );
		if( szEffect == "" )
			break;

		m_vRollingEffect.push_back( szEffect );
	}

	m_vEndEfffect.clear();
	iMax = xElement.GetIntAttribute_e( "Max_Success_Effect" );
	m_vEndEfffect.reserve( iMax );
	for (int i = 0; i < iMax ; i++)
	{
		char szName[MAX_PATH]="";
		StringCbPrintf( szName, sizeof( szName ), "Success_Effect_%d", i+1 );
		ioHashString szEffect = xElement.GetStringAttribute( szName );
		if( szEffect == "" )
			break;

		m_vEndEfffect.push_back( szEffect );
	}
}

void AccessoryCompoundResultWnd::SetCompoundResultInfo( int iTargetIndex, int iCompoundCode, int iCompoundValue, DWORD dwCode )
{
	SAFEDELETE(m_pIcon);
	SAFEDELETE(m_pEffect);

	m_bResult = false;
	m_iTargetIndex = iTargetIndex;
	m_iCompoundCode = iCompoundCode;
	m_iCompoundValue = iCompoundValue;
	m_dwEtcItemCode = dwCode;
	m_pManual = ioEtcInvenSubManualMgr::GetSingletonPtr();
	
	DWORD dwCurTime = FRAMEGETTIME();
	m_dwRollingEndTime = dwCurTime + m_dwRollingTotalTime;
	m_dwRollingCheckTime = 0;
	m_dwRollingEffectCheckTime = 0;
	m_iCurEffectArray = -1;

	ioUserAccessory *pAccessory = g_MyInfo.GetUserAccessory();
	if( pAccessory )
	{
		AccessorySlot kSlot;
		if( pAccessory->GetAccessorySlot( iTargetIndex, kSlot ) )
		{
			m_pIcon = g_UIImageSetMgr.CreateImageByFullName( kSlot.m_IconName );
			m_ItemName = kSlot.m_Name;	
			m_iItemCode= kSlot.m_iAccessoryCode;
		}
	}
	if( !m_szRollingSound.IsEmpty() )
		g_SoundMgr.PlaySound( m_szRollingSound, DSBVOLUME_MAX, PSM_LOOP | PSM_THREAD );

	g_QuestMgr.QuestCompleteTermReserve( QC_ACCESSORY_COMPOUND_SUCCESS, m_iItemCode );

	ReBatchButton( ROLLING_BTN );
	ShowWnd();
}

void AccessoryCompoundResultWnd::OnProcess( float fTimePerSec )
{
	ioWnd::OnProcess( fTimePerSec );

	if( !IsShow() ) return;

	RollingEffect( fTimePerSec );
	UpdateCompoundInfo();
}

void AccessoryCompoundResultWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	enum
	{
		ICON_X		= 129,
		ICON_Y		= 105,

		DESC_BIG_X	= 129,
		DESC_BIG_Y	= 152,

		DESC_X		= 27,
		DESC_Y		= 206,

		FINAL_DESC_X	= 128,
		FINAL_DESC_Y	= 421,
	};

	// Back
	if( m_bResult )
	{
		if( m_pFinalBackEffect )
			m_pFinalBackEffect->Render( iXPos+ICON_X, iYPos+ICON_Y, UI_RENDER_MULTIPLY );
	}
	else
	{
		if( m_pBackEffect )
			m_pBackEffect->Render( iXPos+ICON_X, iYPos+ICON_Y, UI_RENDER_MULTIPLY );
	}

	// Icon
	if( m_pIcon )
	{
		m_pIcon->Render( iXPos+ICON_X, iYPos+ICON_Y, UI_RENDER_NORMAL, TFO_BILINEAR );
	}

	// Effect
	if( m_pEffect )
	{
		m_pEffect->SetScale( 2.0f );
		m_pEffect->Render( iXPos+ICON_X, iYPos+ICON_Y, UI_RENDER_ADD );
	}

	if( m_bResult )
	{
		PrintManual( iXPos+DESC_X, iYPos+DESC_Y, FONT_SIZE_12 );

		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetTextColor( TCT_DEFAULT_GREEN );
		g_FontMgr.PrintText( iXPos+FINAL_DESC_X, iYPos+FINAL_DESC_Y, FONT_SIZE_17, m_szResultMsg.c_str() );
	}
	else
	{
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
		g_FontMgr.PrintText( iXPos+DESC_X, iYPos+DESC_Y, FONT_SIZE_12, m_szRollingMsg.c_str() );
	}

	enum 
	{
		INFO_LEFT_X_OFFSET = 34,
		INFO_LEFT_Y_OFFSET = 352,

		INFO_GAP = 18,
	};
	ioHashString szDesc = "?????";
	if( m_bResult )
	{
		AccessoryCompoundInfo cInfo = g_AccessoryInfoMgr.FindCompoundInfo( m_iCompoundCode );
		if( cInfo.m_iCompoundCode != 0 )
		{
			szDesc = cInfo.m_szName;

			int nMax = cInfo.m_vToolTipTextList.size();
			if ( !COMPARE( nMax, 0, COMPOUND_EFFECT_DESC_SIZE + 1 ) )
				return;

			float fRanNum = m_iCompoundValue / FLOAT1000;
			ioComplexStringPrinter kEffectPrinter[COMPOUND_EFFECT_DESC_SIZE];
			for ( int i=0 ; i < nMax ; ++i )
			{
				kEffectPrinter[i].ClearList();
				kEffectPrinter[i].SetTextStyle( TS_NORMAL );
				if( i == 0 )
					kEffectPrinter[i].SetTextColor( TCT_DEFAULT_GREEN );
				else
					kEffectPrinter[i].SetTextColor( TCT_DEFAULT_DARKGRAY );

				if ( i == 1 )
					kEffectPrinter[i].AddTextPiece( FONT_SIZE_12, cInfo.m_vToolTipTextList[i].c_str(), fRanNum );
				else
					kEffectPrinter[i].AddTextPiece( FONT_SIZE_12, cInfo.m_vToolTipTextList[i].c_str() );

				kEffectPrinter[i].PrintFullText( iXPos+INFO_LEFT_X_OFFSET, iYPos+INFO_LEFT_Y_OFFSET + INFO_GAP * i, TAT_LEFT );
			}
		}
	}
	ioComplexStringPrinter kPrinter[2];
	kPrinter[0].ClearList();
	kPrinter[0].SetTextStyle( TS_NORMAL );
	kPrinter[0].SetTextColor( TCT_DEFAULT_BLUE );
	kPrinter[0].AddTextPiece( FONT_SIZE_17, szDesc.c_str() );
	kPrinter[0].PrintFullText( iXPos+DESC_BIG_X, iYPos+DESC_BIG_Y, TAT_CENTER );

	kPrinter[1].ClearList();
	kPrinter[1].SetTextStyle( TS_NORMAL );
	kPrinter[1].SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter[1].AddTextPiece( FONT_SIZE_17, m_ItemName.c_str() );
	kPrinter[1].PrintFullText( iXPos+DESC_BIG_X, iYPos+DESC_BIG_Y+23, TAT_CENTER );
}

void AccessoryCompoundResultWnd::UpdateCompoundInfo()
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwRollingEndTime < dwCurTime )			// RollingEnd
	{
		// Icon 설정
		if( m_pIcon && !m_bResult )		// 한 번만 하기 위해
		{
			m_dwScaleStartTime = dwCurTime;
		}

		// IconScale 설정
		float fScale = FLOAT1;
		DWORD dwGapTime = dwCurTime - m_dwScaleStartTime;
		if( dwGapTime < m_dwIconScaleTime )
		{
			float fScaleRate = (float)dwGapTime / (float)m_dwIconScaleTime;
			fScale = m_fIconScale * ( FLOAT1 - fScaleRate ) + FLOAT1 * fScaleRate;
		}

		if( m_pIcon )
			m_pIcon->SetScale( fScale );

		if( !m_bResult )
		{
			m_bResult = true;
			ReBatchButton( RESULT_BTN );

			if( !m_szRollingSound.IsEmpty() )
				g_SoundMgr.StopSound( m_szRollingSound, 0 );

			g_SoundMgr.PlaySound( m_szSuccessSound, DSBVOLUME_MAX, PSM_THREAD );

			m_iCurEffectArray = -1;
			m_dwRollingEffectCheckTime = 0;

			g_QuestMgr.QuestCompleteTerm( QC_ACCESSORY_COMPOUND_SUCCESS, m_iItemCode );
		}
	}
	else
	{
		if( m_dwRollingCheckTime == 0 || dwCurTime - m_dwRollingCheckTime > m_dwRollingTime )
		{
			m_dwRollingCheckTime = dwCurTime;
		}
	}
}

void AccessoryCompoundResultWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_GO:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();

			MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
			if( pInvenWnd )
				pInvenWnd->ShowAccessoryTabDirect( g_MyInfo.GetClassType() );
		}
		break;
	}
}

void AccessoryCompoundResultWnd::PrintManual( int a_iXPos, int a_iYPos , float fScale )
{
	if( !m_pManual )
		return;

	ioEtcItem *pItem = g_EtcItemMgr.FindEtcItem( m_dwEtcItemCode );
	if( !pItem ) return;

	int iSubManual = pItem->GetInventorySubManual() - 1;
	ioComplexStringPrinter kPrinter;
	int iMaxLine = m_pManual->GetMaxLine( iSubManual );
	for(int iLine = 1; iLine < iMaxLine; iLine++)
	{
		m_pManual->SetPrinter( iSubManual, iLine, fScale, kPrinter);
		a_iYPos += m_pManual->GetSpaceLine( iSubManual, iLine);
		kPrinter.PrintFullTextWidthCut( a_iXPos, a_iYPos, TAT_LEFT, DESC_WIDTH );
		kPrinter.ClearList();
	}
}

void AccessoryCompoundResultWnd::RollingEffect( float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();
	int iEffectListCnt = m_vRollingEffect.size();
	if( m_bResult )
	{
		iEffectListCnt = m_vEndEfffect.size();
	}

	if( iEffectListCnt == 0 )
	{
		SAFEDELETE( m_pEffect );
		return;
	}

	if( m_bResult )
	{
		if( m_iCurEffectArray == iEffectListCnt-1 )
		{
			SAFEDELETE( m_pEffect );
			return;
		}

		if( m_dwRollingEffectCheckTime == 0 || dwCurTime - m_dwRollingEffectCheckTime > m_dwRollingEffectTime )		// 교체
		{
			m_iCurEffectArray++;
			if( !COMPARE( m_iCurEffectArray, 0, iEffectListCnt ) )
				m_iCurEffectArray = 0;

			SAFEDELETE( m_pEffect );
			ioHashString szEffect = m_vEndEfffect[m_iCurEffectArray];
			if( !szEffect.IsEmpty() )
				m_pEffect = g_UIImageSetMgr.CreateImageByFullName( szEffect );

			m_dwRollingEffectCheckTime = dwCurTime;
		}
	}
	else
	{
		if( m_dwRollingEffectCheckTime == 0 || dwCurTime - m_dwRollingEffectCheckTime > m_dwRollingEffectTime )		// 교체
		{
			m_iCurEffectArray++;
			if( !COMPARE( m_iCurEffectArray, 0, iEffectListCnt ) )
				m_iCurEffectArray = 0;

			SAFEDELETE( m_pEffect );
			ioHashString szEffect = m_vRollingEffect[m_iCurEffectArray];
			if( !szEffect.IsEmpty() )
				m_pEffect = g_UIImageSetMgr.CreateImageByFullName( szEffect );

			m_dwRollingEffectCheckTime = dwCurTime;
		}
	}
}

void AccessoryCompoundResultWnd::ReBatchButton( int iType )
{
	switch( iType )
	{
	case ROLLING_BTN:
		ShowChildWnd(ID_ROLLING);
		SetChildInActive(ID_ROLLING);
		HideChildWnd(ID_GO);
		break;
	case RESULT_BTN:
		HideChildWnd(ID_ROLLING);
		ShowChildWnd(ID_GO);
		break;
	}
}