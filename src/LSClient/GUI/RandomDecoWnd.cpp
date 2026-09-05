
#include "StdAfx.h"

#include "RandomDecoWnd.h"
#include "../ioExtendSoundManager.h"
//////////////////////////////////////////////////////////////////////////
RandomDecoWnd::RandomDecoWnd()
{
	m_pBackEffect = NULL;
	m_pFinalBackEffect = NULL;
	m_pIcon	= NULL;
	m_pEffect = NULL;

	m_bResult = false;
	m_bFinalResult = false;

	m_iPresentType   = 0;
	m_iPresentValue1 = 0;
	m_iPresentValue2 = 0;
	m_iPresentValue3 = 0;
	m_iPresentValue4 = 0;
	m_bAlarm         = false;
	m_iPresentPeso   = 0;

	m_pManual = NULL;
}

RandomDecoWnd::~RandomDecoWnd()
{
	SAFEDELETE(m_pBackEffect);
	SAFEDELETE(m_pFinalBackEffect);
	SAFEDELETE(m_pIcon);
	SAFEDELETE(m_pEffect);

	m_vIconListM.clear();
	m_vIconListW.clear();

	m_vRollingEffect.clear();
	m_vEndEfffect.clear();
}

void RandomDecoWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
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

void RandomDecoWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_dwRollingTime = xElement.GetIntAttribute_e( "Rolling_Time" );
	m_dwRollingTotalTime = xElement.GetIntAttribute_e( "Rolling_Total_Time" );

	m_fIconScale = xElement.GetFloatAttribute_e( "Icon_Scale" );
	m_dwIconScaleTime = xElement.GetIntAttribute_e( "Icon_Scale_Time" );

	m_szRollingSound = xElement.GetStringAttribute_e( "Rolling_Sound" );
	m_szResultSound = xElement.GetStringAttribute_e( "Result_Sound" );

	m_vIconListM.clear();
	int iMax = xElement.GetIntAttribute_e( "Max_Icon_Name" );
	for (int i = 0; i < iMax ; i++)
	{
		char szName[MAX_PATH]="";
		StringCbPrintf_e( szName, sizeof( szName ), "Icon_Name_%d", i+1 );
		ioHashString szIconName = xElement.GetStringAttribute( szName );
		if( szIconName == "" )
			break;
		m_vIconListM.push_back( szIconName );
	}

	m_vIconListW.clear();
	iMax = xElement.GetIntAttribute_e( "Max_Icon_Name_W" );
	for (int i = 0; i < iMax ; i++)
	{
		char szName[MAX_PATH]="";
		StringCbPrintf_e( szName, sizeof( szName ), "Icon_Name_W_%d", i+1 );
		ioHashString szIconName = xElement.GetStringAttribute( szName );
		if( szIconName == "" )
			break;
		m_vIconListW.push_back( szIconName );
	}

	m_dwRollingEffectTime = xElement.GetIntAttribute_e( "Effect_Gap_Time" );
	m_vRollingEffect.clear();

	iMax = xElement.GetIntAttribute_e( "Max_Rolling_Effect" );
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
	iMax = xElement.GetIntAttribute_e( "Max_End_Effect" );
	m_vEndEfffect.reserve( iMax );
	for (int i = 0; i < iMax ; i++)
	{
		char szName[MAX_PATH]="";
		StringCbPrintf_e( szName, sizeof( szName ), "End_Effect_%d", i+1 );
		ioHashString szEffect = xElement.GetStringAttribute( szName );
		if( szEffect == "" )
			break;
		m_vEndEfffect.push_back( szEffect );
	}
}

void RandomDecoWnd::ShowRandomBox( bool bMan )
{
	m_pManual = ioEtcInvenSubManualMgr::GetSingletonPtr();

	ShowWnd();

	m_bMan = bMan;

	m_bResult = false;
	m_bFinalResult = false;
	m_iPresentType   = 0;
	m_iPresentValue1 = 0;
	m_iPresentValue2 = 0;
	m_iPresentValue3 = 0;
	m_iPresentValue4 = 0;
	m_bAlarm         = false;
	m_iPresentPeso   = 0;

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwRollingEndTime = dwCurTime + m_dwRollingTotalTime;
	m_dwRollingCheckTime = 0;
	m_dwRollingEffectCheckTime = 0;

	m_iCurIconArray = -1;
	m_iCurEffectArray = -1;

	SAFEDELETE(m_pIcon);
	SAFEDELETE(m_pEffect);

	ReBatchButton( ROLLING_BTN );

	if( !m_szRollingSound.IsEmpty() )
		g_SoundMgr.PlaySound( m_szRollingSound, DSBVOLUME_MAX, PSM_LOOP | PSM_THREAD );
}

void RandomDecoWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	ioWnd::OnProcess( fTimePerSec );

	RollingIcon( fTimePerSec );
	RollingEffect( fTimePerSec );
}

void RandomDecoWnd::RollingIcon( float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();

	int iIconListCnt = m_vIconListM.size();
	if( !m_bMan )
		iIconListCnt = m_vIconListW.size();

	if( m_bResult && m_dwRollingEndTime < dwCurTime  )			// RollingEnd
	{
		// Icon 설정
		if( !m_bFinalResult )		// 한 번만 하기 위해
		{
			SetPresentIcon();
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

		if( !m_bFinalResult )
		{
			m_bFinalResult = true;
			ReBatchButton( RESULT_BTN );

			if( !m_szRollingSound.IsEmpty() )
				g_SoundMgr.StopSound( m_szRollingSound, 0 );

			if( g_ExSoundMgr.IsSound( ExSound::EST_GASHAPON_GET ) )
			{
				g_ExSoundMgr.PlaySound( ExSound::EST_GASHAPON_GET );
			}
			else
			{
				if( !m_szResultSound.IsEmpty() )
					g_SoundMgr.PlaySound( m_szResultSound );
			}

			m_iCurEffectArray = -1;
			m_dwRollingEffectCheckTime = 0;
		}
	}
	else if( iIconListCnt > 0 )				// Rolling
	{
		if( m_dwRollingCheckTime == 0 || dwCurTime - m_dwRollingCheckTime > m_dwRollingTime )		// 교체
		{
			SAFEDELETE( m_pIcon );

			m_iCurIconArray++;

			if( !COMPARE( m_iCurIconArray, 0, iIconListCnt ) )
				m_iCurIconArray = 0;

			ioHashString szCurIcon = m_vIconListM[m_iCurIconArray];
			if( !m_bMan )
				szCurIcon = m_vIconListW[m_iCurIconArray];

			if( !szCurIcon.IsEmpty() )
			{
				m_pIcon = g_UIImageSetMgr.CreateImageByFullName( szCurIcon );
			}

			m_dwRollingCheckTime = dwCurTime;
		}
	}
}

void RandomDecoWnd::RollingEffect( float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();

	int iEffectListCnt = m_vRollingEffect.size();
	if( m_bFinalResult )
		iEffectListCnt = m_vEndEfffect.size();

	if( iEffectListCnt == 0 )
	{
		SAFEDELETE( m_pEffect );
		return;
	}

	if( m_bFinalResult )
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

			ioHashString szEffect = m_vEndEfffect[m_iCurEffectArray];

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

void RandomDecoWnd::OnRender()
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
	if( m_bFinalResult )
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

	if( m_bFinalResult )
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
		g_FontMgr.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
		g_FontMgr.PrintText( iXPos+DESC_X, iYPos+DESC_Y, FONT_SIZE_12, STR(2) );

		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
		g_FontMgr.PrintText( iXPos+DESC_X, iYPos+DESC_Y+18, FONT_SIZE_12, STR(4) );
	}
	
	// Text
	ioComplexStringPrinter kPrinter[2];

	ioHashString szName = "?????";
	ioHashString szDesc = "?????";

	if( m_bFinalResult )
	{
		szName = g_PresentMgr.GetPresentValue1Text( m_iPresentType, m_iPresentValue1, m_iPresentValue2 );

		if(  !g_PresentMgr.GetPresentValue2Text( m_iPresentType, m_iPresentValue1, m_iPresentValue2 ).IsEmpty() )
		{
			if( m_iPresentPeso == 0 )
				szDesc = g_PresentMgr.GetPresentValue2Text( m_iPresentType, m_iPresentValue1, m_iPresentValue2 );
			else
			{
				char szBuf[MAX_PATH] = "";
				SafeSprintf( szBuf, sizeof( szBuf ), STR(3), g_PresentMgr.GetPresentValue2Text( m_iPresentType, m_iPresentValue1, m_iPresentValue2 ).c_str(), m_iPresentPeso );
				szDesc = szBuf;
			}
		}
	}

	// Name
	kPrinter[0].ClearList();
	kPrinter[0].SetTextStyle( TS_NORMAL );
	kPrinter[0].SetBkColor( 0, 0, 0 );	
	kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter[0].AddTextPiece( FONT_SIZE_17, szName.c_str() );
	kPrinter[0].PrintFullText( iXPos+DESC_BIG_X, iYPos+DESC_BIG_Y, TAT_CENTER );

	kPrinter[1].ClearList();
	kPrinter[1].SetTextStyle( TS_NORMAL );
	kPrinter[1].SetBkColor( 0, 0, 0 );
	kPrinter[1].SetTextColor( TCT_DEFAULT_BLUE );
	kPrinter[1].AddTextPiece( FONT_SIZE_17, szDesc.c_str() );
	kPrinter[1].PrintFullText( iXPos+DESC_BIG_X, iYPos+DESC_BIG_Y+23, TAT_CENTER );
}

void RandomDecoWnd::iwm_hide()
{
	if( !m_szRollingSound.IsEmpty() )
	{
		g_SoundMgr.StopSound( m_szRollingSound, 0 );
	}
}

void RandomDecoWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_GO:
		if( cmd == IOBN_BTNUP )
		{
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
				int iType = ioEtcItem::EIT_ETC_RANDOM_DECO_W;
				if( m_bMan )
				{
					iType = ioEtcItem::EIT_ETC_RANDOM_DECO_M;
				}

				ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( iType );
				if( !pEtcItem )
				{
					LOG.PrintTimeAndLog( 0, "%s pEtcItem == NULL : %d", __FUNCTION__, iType );
					return;
				}
				HideWnd();

				pEtcItem->OnUseBtnPush( iType, pUserEtcItem );
			}
		}
		break;
	}
}

void RandomDecoWnd::ReBatchButton( int iType )
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
		// 남캐 치장보급
		if( m_bMan )
		{
			if( !pUserEtcItem->GetEtcItem( ioEtcItem::EIT_ETC_RANDOM_DECO_M, kSlot) )
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
		}
		else	// 여캐 치장보급
		{
			if( !pUserEtcItem->GetEtcItem( ioEtcItem::EIT_ETC_RANDOM_DECO_W, kSlot) )
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
		}
		break;
	}
}

void RandomDecoWnd::OnResultInfo( short iPresentType, int iPresentValue1, int iPresentValue2, int iPresentValue3, int iPresentValue4, bool bAlram, int iPresentPeso )
{
	m_bResult   = true;

	m_iPresentType   = iPresentType;
	m_iPresentValue1 = iPresentValue1;
	m_iPresentValue2 = iPresentValue2;
	m_iPresentValue3 = iPresentValue3;
	m_iPresentValue4 = iPresentValue4;
	m_bAlarm         = bAlram;
	m_iPresentPeso   = iPresentPeso;
}

void RandomDecoWnd::SetPresentIcon()
{
	SAFEDELETE( m_pIcon );
	switch( m_iPresentType )
	{
	case PRESENT_RANDOM_DECO:
		{
			int iSexType   = (m_iPresentValue1 % 100000) / 1000;
			int iDecoType  = m_iPresentValue1 % 1000;
			const DecoData &kData = g_DecorationPrice.GetDecoData( iSexType, iDecoType, m_iPresentValue2 );
			m_pIcon = g_UIImageSetMgr.CreateImageByFullName( kData.m_szIcon );
		}
		break;
	}
}

bool RandomDecoWnd::IsRolling()
{
	if( m_bResult && !m_bFinalResult )
		return true;

	return false;
}

void RandomDecoWnd::PrintManual( int a_iXPos, int a_iYPos , float fScale )
{
	if( !m_pManual )
		return;

	ioEtcItem *pItem = NULL;
	if( m_bMan )
		pItem = g_EtcItemMgr.FindEtcItem( ioEtcItem::EIT_ETC_RANDOM_DECO_M );
	else
		pItem = g_EtcItemMgr.FindEtcItem( ioEtcItem::EIT_ETC_RANDOM_DECO_W );

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