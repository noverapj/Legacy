
#include "StdAfx.h"

#include "ItemMaterialCompoundResultWnd.h"
#include "../ioExtendSoundManager.h"

ItemMaterialCompoundResultWnd::ItemMaterialCompoundResultWnd()
{
	m_pIcon = NULL;
	m_pEffect = NULL;

	m_pBackEffect = NULL;
	m_pFinalBackEffect = NULL;

	m_pNumText = NULL;
	m_pPlusText = NULL;

	m_pGaugeMainBack = NULL;
	m_pGaugeBack  = NULL;
	m_pNewGauge      = NULL;
	m_pPreGauge      = NULL;

	m_bResult = false;
	m_bSuccess = false;

	m_nTargetSlot = 0;
	m_nPreReinforce = 0;
	m_nResultReinforce = 0;
	m_nItemCode = 0;

	m_pManual = NULL;

	m_nMaxFailExp = 0;
	m_fNewFailExpRate = 0.0f;
	m_fPreFailExpRate = 0.0f;
	m_fGradeScale = FLOAT1;
}

ItemMaterialCompoundResultWnd::~ItemMaterialCompoundResultWnd()
{
	SAFEDELETE( m_pIcon );
	SAFEDELETE( m_pEffect );

	SAFEDELETE( m_pBackEffect );
	SAFEDELETE( m_pFinalBackEffect );

	SAFEDELETE( m_pNumText );
	SAFEDELETE( m_pPlusText );

	SAFEDELETE( m_pGaugeMainBack );
	SAFEDELETE( m_pGaugeBack );
	SAFEDELETE( m_pNewGauge );
	SAFEDELETE( m_pPreGauge );

	m_vRollingEffect.clear();
	m_vExtraRollingEffect.clear();
	m_vEndEfffect_S.clear();
	m_vEndEfffect_ExtraS.clear();
	m_vEndEfffect_F.clear();
}

void ItemMaterialCompoundResultWnd::iwm_hide()
{
	if( !m_szRollingSound.IsEmpty() )
		g_SoundMgr.StopSound( m_szRollingSound, 0 );

	if( !m_szSuccessSound.IsEmpty() )
		g_SoundMgr.StopSound( m_szSuccessSound, 0 );

	if( !m_szExtraSuccessSound.IsEmpty() )
		g_SoundMgr.StopSound( m_szExtraSuccessSound, 0 );

	if( !m_szFailSound.IsEmpty() )
		g_SoundMgr.StopSound( m_szFailSound, 0 );

	g_QuestMgr.ProcessCompleteReserve();
}

void ItemMaterialCompoundResultWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
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
				pInvenWnd->ShowExtraItemTabDirect( g_MyInfo.GetClassType() );
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

void ItemMaterialCompoundResultWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
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
	else if( szType == "NewGauge" )
	{
		SAFEDELETE( m_pNewGauge );
		m_pNewGauge = pImage;
	}
	else if( szType == "PreGauge" )
	{
		SAFEDELETE( m_pPreGauge );
		m_pPreGauge = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}

void ItemMaterialCompoundResultWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	char szKeyName[MAX_PATH]="";
	ioHashString szTemp;
	
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
	StringCbPrintf_e( szKeyName, sizeof( szKeyName ), "extra_success_sound" );
	m_szExtraSuccessSound = xElement.GetStringAttribute( szKeyName );

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

	m_vExtraRollingEffect.clear();
	iMax = xElement.GetIntAttribute_e( "Max_Extra_Rolling_Effect" );
	m_vExtraRollingEffect.reserve( iMax );
	for (int i = 0; i < iMax ; i++)
	{
		char szName[MAX_PATH]="";
		StringCbPrintf( szName, sizeof( szName ), "Extra_Rolling_Effect_%d", i+1 );
		ioHashString szEffect = xElement.GetStringAttribute( szName );
		if( szEffect == "" )
			break;
		m_vExtraRollingEffect.push_back( szEffect );
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

	m_vEndEfffect_ExtraS.clear();
	iMax = xElement.GetIntAttribute_e( "Max_Extra_Success_Effect" );
	m_vEndEfffect_ExtraS.reserve( iMax );
	for (int i = 0; i < iMax ; i++)
	{
		char szName[MAX_PATH]="";
		StringCbPrintf( szName, sizeof( szName ), "Extra_Success_Effect_%d", i+1 );
		ioHashString szEffect = xElement.GetStringAttribute( szName );
		if( szEffect == "" )
			break;
		m_vEndEfffect_ExtraS.push_back( szEffect );
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

void ItemMaterialCompoundResultWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) 
		return;

	ioWnd::OnProcess( fTimePerSec );

	RollingEffect( fTimePerSec );
	UpdateCompoundInfo();
}

void ItemMaterialCompoundResultWnd::OnRender()
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

	if( m_bResult && m_pNumText && m_pPlusText && m_nResultReinforce > 0 )
	{
		int iCurPos = 0;
		int iNewYPos = iYPos+ICON_Y;
		int iDefaultPos = iXPos+ICON_X;

		int iCurYPos = iNewYPos+REINFORCE_Y;

		int iTotalSize = 0;
		if( m_nResultReinforce < 10 )
			iTotalSize = TEXT_SIZE * 2 - TEXT_GAP;
		else if( m_nResultReinforce < 100 )
			iTotalSize = TEXT_SIZE * 3 - TEXT_GAP * 2;
		else
			iTotalSize = TEXT_SIZE * 4 - TEXT_GAP * 3;

		iCurPos = iDefaultPos - (iTotalSize / 2);
		m_pPlusText->Render( iCurPos, iCurYPos );

		iCurPos += (TEXT_SIZE - TEXT_GAP);
		m_pNumText->RenderNum( iCurPos, iCurYPos, m_nResultReinforce, -TEXT_GAP );
	}

	if( m_bResult )
	{
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
			int iGap = m_nResultReinforce - m_nPreReinforce;
			if ( m_nResultReinforce <= g_CompoundMaterialMgr.GetFixedFailReinforce() )
				SafeSprintf( szBuf, sizeof( szBuf ), STR(3), iGap );
			else
				SafeSprintf( szBuf, sizeof( szBuf ), STR(4), iGap );
		}
		else
		{
			if ( m_nResultReinforce < g_CompoundMaterialMgr.GetFixedFailReinforce() )
				SafeSprintf( szBuf, sizeof( szBuf ), STR(5) );
			else
				SafeSprintf( szBuf, sizeof( szBuf ), STR(6) );
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

		//실패 게이지 표시
		RenderFailExp( iXPos, iYPos );
	}
}

void ItemMaterialCompoundResultWnd::ReBatchButton( int iType )
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

void ItemMaterialCompoundResultWnd::RollingEffect( float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();

	int iEffectListCnt = 0;
	
	//이펙트 개수 확인
	if( m_bResult )
	{
		if( m_bSuccess )
		{
			if ( m_nPreReinforce < g_CompoundMaterialMgr.GetFixedFailReinforce() )
				iEffectListCnt = m_vEndEfffect_S.size();
			else 
				iEffectListCnt = m_vEndEfffect_ExtraS.size();
		}
		else
			iEffectListCnt = m_vEndEfffect_F.size();
	}
	else
	{
		if ( m_nPreReinforce < g_CompoundMaterialMgr.GetFixedFailReinforce() )
			iEffectListCnt = m_vRollingEffect.size();
		else
			iEffectListCnt = m_vExtraRollingEffect.size();
	}

	if( iEffectListCnt == 0 )
	{
		SAFEDELETE( m_pEffect );
		return;
	}

	if( m_bResult )
	{
		//최대가 되면 이펙트 삭제
		if( m_iCurEffectArray == iEffectListCnt - 1 )
		{
			SAFEDELETE( m_pEffect );
			return;
		}

		if( m_dwRollingEffectCheckTime == 0 || m_dwRollingEffectCheckTime + m_dwRollingEffectTime < dwCurTime )		// 교체
		{
			SAFEDELETE( m_pEffect );

			m_iCurEffectArray++;

			if( !COMPARE( m_iCurEffectArray, 0, iEffectListCnt ) )
				m_iCurEffectArray = 0;

			ioHashString szEffect;
			if( m_bSuccess )
			{
				if ( m_nPreReinforce < g_CompoundMaterialMgr.GetFixedFailReinforce() )
					szEffect = m_vEndEfffect_S[m_iCurEffectArray];
				else 
					szEffect = m_vEndEfffect_ExtraS[m_iCurEffectArray];
			}
			else
				szEffect = m_vEndEfffect_F[m_iCurEffectArray];

			if( !szEffect.IsEmpty() )
				m_pEffect = g_UIImageSetMgr.CreateImageByFullName( szEffect );

			m_dwRollingEffectCheckTime = dwCurTime;
		}
	}
	else
	{
		if( m_dwRollingEffectCheckTime == 0 || m_dwRollingEffectCheckTime + m_dwRollingEffectTime < dwCurTime )		// 교체
		{
			SAFEDELETE( m_pEffect );

			m_iCurEffectArray++;

			if( !COMPARE( m_iCurEffectArray, 0, iEffectListCnt ) )
				m_iCurEffectArray = 0;

			ioHashString szEffect = "";
			if ( m_nPreReinforce < g_CompoundMaterialMgr.GetFixedFailReinforce() )
				szEffect= m_vRollingEffect[m_iCurEffectArray];
			else
				szEffect= m_vExtraRollingEffect[m_iCurEffectArray];

			if( !szEffect.IsEmpty() )
				m_pEffect = g_UIImageSetMgr.CreateImageByFullName( szEffect );

			m_dwRollingEffectCheckTime = dwCurTime;
		}
	}
}
//실패경험치 렌더
void ItemMaterialCompoundResultWnd::RenderFailExp( int iXPos, int iYPos )
{
	if( !m_pGaugeMainBack || !m_pGaugeBack || !m_pNewGauge || !m_pPreGauge )
		return;

	enum 
	{		
		GAUGE_X_OFFSET = 57,
		GAUGE_Y_OFFSET = 217,
		GAUGE_BACK_OFFSET = 2,

		TEXT_X_OFFSET1 = 129,
		TEXT_Y_OFFSET1 = 199,

		TEXT_X_OFFSET2  = 129,
		TEXT_Y_OFFSET2  = 199,
	};
	
	//실패시 문구
	if ( !m_bSuccess )
	{
		if ( m_nMaxFailExp > 0 )
		{
			ioComplexStringPrinter kPrinter;
			kPrinter.SetTextStyle( TS_NORMAL );
			kPrinter.SetBkColor( 0, 0, 0 );
			kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
			kPrinter.AddTextPiece( FONT_SIZE_12, "실패 경험치 %.2f％", m_fNewFailExpRate * FLOAT100 );	
			kPrinter.PrintFullText( iXPos + TEXT_X_OFFSET1, iYPos + TEXT_Y_OFFSET1, TAT_CENTER );
			kPrinter.ClearList();	
		
			//게이지
			int nNewGaugeWidth = m_pNewGauge->GetWidth() * (float)min( FLOAT1, m_fNewFailExpRate );
			int nPreGaugeWidth = m_pNewGauge->GetWidth() * (float)min( FLOAT1, m_fPreFailExpRate );

			//렌더 순서
			m_pGaugeMainBack->Render( iXPos + GAUGE_X_OFFSET - GAUGE_BACK_OFFSET, iYPos + GAUGE_Y_OFFSET - GAUGE_BACK_OFFSET );
			m_pGaugeBack->Render( iXPos + GAUGE_X_OFFSET, iYPos + GAUGE_Y_OFFSET );
			m_pNewGauge->RenderWidthCut( iXPos + GAUGE_X_OFFSET, iYPos + GAUGE_Y_OFFSET, 0, nNewGaugeWidth );
			m_pPreGauge->RenderWidthCut( iXPos + GAUGE_X_OFFSET, iYPos + GAUGE_Y_OFFSET, 0, nPreGaugeWidth );
		}
		else
		{
			ioComplexStringPrinter kPrinter;
			kPrinter.SetTextStyle( TS_NORMAL );
			kPrinter.SetBkColor( 0, 0, 0 );
			kPrinter.SetTextColor( TCT_DEFAULT_RED );
			kPrinter.AddTextPiece( FONT_SIZE_17, STR(2), g_CompoundMaterialMgr.GetFixedFailReinforce() );
			kPrinter.PrintFullText( iXPos + TEXT_X_OFFSET2, iYPos + TEXT_Y_OFFSET2, TAT_CENTER );
			kPrinter.ClearList();	
		}
	}	
}

void ItemMaterialCompoundResultWnd::UpdateCompoundInfo()
{
	DWORD dwCurTime = FRAMEGETTIME();
	
	// RollingEnd
	if( m_dwRollingEndTime < dwCurTime )			
	{
		if( !m_bResult )
		{
			if ( m_pIcon )
				m_dwScaleStartTime = dwCurTime;

			ReBatchButton( RESULT_BTN );

			if( !m_szRollingSound.IsEmpty() )
				g_SoundMgr.StopSound( m_szRollingSound, 0 );

			if( m_bSuccess )
			{
				if( g_ExSoundMgr.IsSound( ExSound::EST_REINFORCE_SUCCESS ) )
				{
					g_ExSoundMgr.PlaySound( ExSound::EST_REINFORCE_SUCCESS );
				}
				else
				{
					if( !m_szSuccessSound.IsEmpty() && m_nResultReinforce <= g_CompoundMaterialMgr.GetFixedFailReinforce() )
						g_SoundMgr.PlaySound( m_szSuccessSound, DSBVOLUME_MAX, PSM_THREAD );
					else if( !m_szExtraSuccessSound.IsEmpty() && m_nResultReinforce > g_CompoundMaterialMgr.GetFixedFailReinforce() )
						g_SoundMgr.PlaySound( m_szExtraSuccessSound, DSBVOLUME_MAX, PSM_THREAD );
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
			m_bResult = true;
			g_QuestMgr.QuestCompleteTerm( QC_EXTRA_ITEM_REINFORCE_SUCCESS, m_nResultReinforce, m_nItemCode );
		}
		
		if( m_pIcon )
		{
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

			m_pIcon->SetScale( fScale );
		}
	}
}

void ItemMaterialCompoundResultWnd::SetCompoundResultInfo( bool bSuccess, int nTargetSlot, int nReinforce, DWORD dwCode )
{
	m_bResult = false;
	m_bSuccess = bSuccess;
	m_nTargetSlot = nTargetSlot;
	m_nResultReinforce = nReinforce;

	m_nPreReinforce = 0;

	m_dwEtcItemCode = dwCode;
	m_nGradeType = 0;
	m_fGradeScale = FLOAT1;

	m_pManual = ioEtcInvenSubManualMgr::GetSingletonPtr();

	SAFEDELETE(m_pIcon);
	SAFEDELETE(m_pEffect);

	ShowWnd();

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwRollingEndTime = dwCurTime + m_dwRollingTotalTime;
	m_dwRollingEffectCheckTime = 0;

	m_iCurEffectArray = -1;

	ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
	if( pExtraItem )
	{
		EXTRAITEMSLOT kSlot;
		if( pExtraItem->GetExtraItem( m_nTargetSlot, kSlot ) )
		{
			m_nPreReinforce = kSlot.m_iReinforce;

			m_pIcon = g_UIImageSetMgr.CreateImageByFullName( kSlot.m_IconName );
			m_ItemName = kSlot.m_Name;	
			m_nItemCode = kSlot.m_iItemCode;

			const ioItem *pItem = g_ItemMaker.GetItemConst( m_nItemCode, __FUNCTION__ );
			if ( pItem )
				m_nGradeType = pItem->GetGradeType();
		}
	}

	ReBatchButton( ROLLING_BTN );

	if( !m_szRollingSound.IsEmpty() )
		g_SoundMgr.PlaySound( m_szRollingSound, DSBVOLUME_MAX, PSM_LOOP | PSM_THREAD );

	g_QuestMgr.QuestCompleteTermReserve( QC_EXTRA_ITEM_REINFORCE_SUCCESS, m_nResultReinforce, m_nItemCode );

#if defined( USE_GA )
	char chLabel[32] = {0,};

	if ( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
		sprintf_e( chLabel, "%d", m_nItemCode );
	else
		SafeSprintf( chLabel, sizeof(chLabel), "%1", m_nItemCode );

	// ITEM_ENHANCE_MATERIAL
	g_HttpMng.GA_EventTracking( g_MyInfo.GetUserIndex()
		, "Item"
		, "Material"
		, chLabel
		, 1
		, "%2FITEM%2FENHANCE%2FMATERIAL" );
#endif
}

void ItemMaterialCompoundResultWnd::SetFailExp( int nPreFailExp, int nCurFailExp, int nItemCode )
{
	//최대 실패 경험치 얻기
	m_nMaxFailExp = g_CompoundMaterialMgr.GetMaxFailExp( m_nResultReinforce, m_dwEtcItemCode );

	if ( m_nMaxFailExp > 0 )
	{
		m_fNewFailExpRate = ( (float)nCurFailExp / m_nMaxFailExp );
		m_fPreFailExpRate = ( (float)nPreFailExp / m_nMaxFailExp );
	}

	m_fNewFailExpRate = min( FLOAT1, m_fNewFailExpRate );
	m_fPreFailExpRate = min( FLOAT1, m_fPreFailExpRate );
}