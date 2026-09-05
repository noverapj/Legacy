
#include "StdAfx.h"

#include "../ioExtendSoundManager.h"
#include "GashaponWnd.h"
#include "GashaponPresentListWnd.h"
//////////////////////////////////////////////////////////////////////////
GashaponWnd::GashaponWnd()
{
	m_pIcon	= NULL;
	m_pSubIcon = NULL;
	m_pEffect = NULL;

	m_pBackEffect = NULL;
	m_pFinalBackEffect = NULL;
	m_pCostumeMark = NULL;

	m_pManual = NULL;

	m_bResult = false;
	m_bFinalResult = false;

	m_iPresentType   = 0;
	m_iPresentValue1 = 0;
	m_iPresentValue2 = 0;
	m_iPresentValue3 = 0;
	m_iPresentValue4 = 0;
	m_bAlarm         = false;
	m_iPresentPeso   = 0;

	m_dwEtcItemType  = 0;
	m_dwPossessEtcItemType = 0;
	m_bIsCostume = false;
	m_nGradeType = 0;
	m_fGradeScale = FLOAT1;
}

GashaponWnd::~GashaponWnd()
{
	SAFEDELETE(m_pBackEffect);
	SAFEDELETE(m_pFinalBackEffect);

	SAFEDELETE(m_pIcon);
	SAFEDELETE(m_pSubIcon);
	SAFEDELETE(m_pEffect);
	SAFEDELETE( m_pCostumeMark );

	for (int i = 0; i < MAX_GASHAPON ; i++)
	{
		m_vIconList[i].clear();
		m_vCostumeList[i].clear();
		m_vGradeTypeList[i].clear();
		m_vIconChangePresentTypeVec[i].clear();
		m_vIconChangePresentValue1Vec[i].clear();
		m_vIconChangePresentValue2Vec[i].clear();
		m_vIconChangePresentValue3Vec[i].clear();
		m_vIconChangePresentValue4Vec[i].clear();
		m_vIconChangeIconVec[i].clear();
	}
}

void GashaponWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
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
	else if( szType == "costume_image" )
	{
		SAFEDELETE( m_pCostumeMark );
		m_pCostumeMark = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}

void GashaponWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_dwRollingTime = xElement.GetIntAttribute_e( "Rolling_Time" );
	DWORD dwRollingTotalTime = xElement.GetIntAttribute_e( "Default_Rolling_Total_Time" );

	m_fIconScale = xElement.GetFloatAttribute_e( "Icon_Scale" );
	m_dwIconScaleTime = xElement.GetIntAttribute_e( "Icon_Scale_Time" );

	m_szRollingSound = xElement.GetStringAttribute_e( "Rolling_Sound" );
	m_szResultSound = xElement.GetStringAttribute_e( "Result_Sound" );

	for (int iGashaponCnt = 0; iGashaponCnt < MAX_GASHAPON ; iGashaponCnt++)
	{
		char szName[MAX_PATH]="";
		StringCbPrintf_e( szName, sizeof( szName ), "Gashapon%d_Desc1_Text", iGashaponCnt+1 );
		m_szDesc1[iGashaponCnt] = xElement.GetStringAttribute( szName );
		StringCbPrintf_e( szName, sizeof( szName ), "Gashapon%d_Desc2_Text", iGashaponCnt+1 );
		m_szDesc2[iGashaponCnt] = xElement.GetStringAttribute( szName );
		StringCbPrintf_e( szName, sizeof( szName ), "Gashapon%d_Title_Text", iGashaponCnt+1 );
		m_szTitle[iGashaponCnt] = xElement.GetStringAttribute( szName );

		m_vIconList[iGashaponCnt].clear();
		m_vCostumeList[iGashaponCnt].clear();
		m_vGradeTypeList[iGashaponCnt].clear();
		StringCbPrintf_e( szName, sizeof( szName ), "Gashapon%d_Max_Icon_Name", iGashaponCnt+1 );
		int iMax = xElement.GetIntAttribute( szName );
		for (int i = 0; i < iMax ; i++)
		{
			char szName[MAX_PATH]="";
			StringCbPrintf_e( szName, sizeof( szName ), "Gashapon%d_Icon_Name_%d", iGashaponCnt+1, i+1 );
			ioHashString szIconName = xElement.GetStringAttribute( szName );
			if( szIconName == "" )
				break;
			m_vIconList[iGashaponCnt].push_back( szIconName );

			StringCbPrintf_e( szName, sizeof( szName ), "Gashapon%d_Costume_%d", iGashaponCnt+1, i+1 );
			bool bIsCostume = xElement.GetBoolAttribute( szName );
			m_vCostumeList[iGashaponCnt].push_back( bIsCostume );

			StringCbPrintf_e( szName, sizeof( szName ), "Gashapon%d_Grade_%d", iGashaponCnt+1, i+1 );
			int nGradeType = xElement.GetIntAttribute( szName );
			m_vGradeTypeList[iGashaponCnt].push_back( nGradeType );
		}	

		m_dwRollingTotalTime[ iGashaponCnt ] = xElement.GetIntAttribute_e( "Gashapon%d_Rolling_Total_Time" );
		// 0 이면 xml에서 위 이름 검색이 되지 않은 것으로 간주되어 기본 값 1500으로 셋팅
		if( m_dwRollingTotalTime[ iGashaponCnt ] == 0 )
			m_dwRollingTotalTime[ iGashaponCnt ] = dwRollingTotalTime;
	}

	m_dwRollingEffectTime = xElement.GetIntAttribute_e( "Effect_Gap_Time" );
	m_vRollingEffect.clear();

	int iMax = xElement.GetIntAttribute_e( "Max_Rolling_Effect" );
	m_vRollingEffect.reserve( iMax );
	for (int i = 0; i < iMax ; i++)
	{
		char szName[MAX_PATH]="";
		StringCbPrintf_e( szName, sizeof( szName ), "Rolling_Effect_%d", i+1 );
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

	// icon change
	for (int iGashaponCnt = 0; iGashaponCnt < MAX_GASHAPON ; iGashaponCnt++)
	{
		char szName[MAX_PATH]="";
		m_vIconChangePresentTypeVec[iGashaponCnt].clear();
		m_vIconChangePresentValue1Vec[iGashaponCnt].clear();
		m_vIconChangePresentValue2Vec[iGashaponCnt].clear();
		m_vIconChangePresentValue3Vec[iGashaponCnt].clear();
		m_vIconChangePresentValue4Vec[iGashaponCnt].clear();
		m_vIconChangeIconVec[iGashaponCnt].clear();

		StringCbPrintf_e( szName, sizeof( szName ), "Gashapon%d_Max_Change_Icon", iGashaponCnt+1 );
		int iMax = xElement.GetIntAttribute( szName );
		for (int i = 0; i < iMax ; i++)
		{
			ZeroMemory( szName, sizeof( szName ) );
			StringCbPrintf_e( szName, sizeof( szName ), "Gashapon%d_Change%d_Type", iGashaponCnt+1, i+1 );
			int iPresentType = xElement.GetIntAttribute( szName );
			if( iPresentType == 0 )
				break;
			m_vIconChangePresentTypeVec[iGashaponCnt].push_back( iPresentType );

			ZeroMemory( szName, sizeof( szName ) );
			StringCbPrintf_e( szName, sizeof( szName ), "Gashapon%d_Change%d_Value1", iGashaponCnt+1, i+1 );
			m_vIconChangePresentValue1Vec[iGashaponCnt].push_back( xElement.GetIntAttribute( szName ) );

			ZeroMemory( szName, sizeof( szName ) );
			StringCbPrintf_e( szName, sizeof( szName ), "Gashapon%d_Change%d_Value2", iGashaponCnt+1, i+1 );
			m_vIconChangePresentValue2Vec[iGashaponCnt].push_back( xElement.GetIntAttribute( szName ) );

			m_vIconChangePresentValue3Vec[iGashaponCnt].push_back( 0 );
			m_vIconChangePresentValue4Vec[iGashaponCnt].push_back( 0 );

			ZeroMemory( szName, sizeof( szName ) );
			StringCbPrintf_e( szName, sizeof( szName ), "Gashapon%d_Change%d_Icon", iGashaponCnt+1, i+1 );
			ioHashString szIconName = xElement.GetStringAttribute( szName );
			m_vIconChangeIconVec[iGashaponCnt].push_back( szIconName );
		}	
	}
}

void GashaponWnd::ShowRandomBox( DWORD dwPossessEtcItemType, DWORD dwEtcItemType )
{
	m_dwEtcItemType = dwEtcItemType;
	m_dwPossessEtcItemType = dwPossessEtcItemType;
	m_pManual = ioEtcInvenSubManualMgr::GetSingletonPtr();

	ShowWnd();

	m_bResult = false;
	m_bFinalResult = false;
	m_iPresentType   = 0;
	m_iPresentValue1 = 0;
	m_iPresentValue2 = 0;
	m_iPresentValue3 = 0;
	m_iPresentValue4 = 0;
	m_bAlarm         = false;
	m_iPresentPeso   = 0;
	m_bIsCostume = false;
	m_nGradeType = 0;
	m_fGradeScale = FLOAT1;

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwRollingEndTime = dwCurTime + m_dwRollingTotalTime[ GetGashaponArray() ];
	m_dwRollingCheckTime = 0;
	m_dwRollingEffectCheckTime = 0;

	m_iCurIconArray = -1;
	m_iCurEffectArray = -1;

	SAFEDELETE(m_pIcon);
	SAFEDELETE(m_pSubIcon);
	SAFEDELETE(m_pEffect);

	ReBatchButton( ROLLING_BTN );

	if(  !m_szTitle[GetGashaponArray()].IsEmpty() )
		SetTitleText( m_szTitle[GetGashaponArray()].c_str() );

	if( !m_szRollingSound.IsEmpty() )
		g_SoundMgr.PlaySound( m_szRollingSound, DSBVOLUME_MAX, PSM_LOOP | PSM_THREAD );
}

void GashaponWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	ioWnd::OnProcess( fTimePerSec );

	RollingIcon( fTimePerSec );
	RollingEffect( fTimePerSec );
}

void GashaponWnd::RollingIcon( float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();

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
		if( m_pSubIcon )
			m_pSubIcon->SetScale( fScale );
		if ( m_pCostumeMark )
			m_pCostumeMark->SetScale( fScale );

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

			// 선물함 활성화
			MyInventoryWnd *pMyInventoryWnd = dynamic_cast<MyInventoryWnd*>( g_GUIMgr.FindWnd( MY_INVENTORY_WND ) );
			if( pMyInventoryWnd && pMyInventoryWnd->IsShowPresentTab() )
				pMyInventoryWnd->UpdateTab( false, false );
		}
	}
	else if( (int)m_vIconList[GetGashaponArray()].size() > 0 )				// Rolling
	{
		if( m_dwRollingCheckTime == 0 || dwCurTime - m_dwRollingCheckTime > m_dwRollingTime )		// 교체
		{
			SAFEDELETE( m_pIcon );
			SAFEDELETE( m_pSubIcon );

			m_iCurIconArray++;

			if( !COMPARE( m_iCurIconArray, 0, (int)m_vIconList[GetGashaponArray()].size() ) )
				m_iCurIconArray = 0;

			ioHashString szCurIcon = m_vIconList[GetGashaponArray()][m_iCurIconArray];
			if( !szCurIcon.IsEmpty() )
			{
				m_pIcon = g_UIImageSetMgr.CreateImageByFullName( szCurIcon );

				m_bIsCostume = m_vCostumeList[GetGashaponArray()][m_iCurIconArray];
				m_nGradeType = m_vGradeTypeList[GetGashaponArray()][m_iCurIconArray];
			}

			m_dwRollingCheckTime = dwCurTime;
		}
	}
}

void GashaponWnd::RollingEffect( float fTimePerSec )
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

void GashaponWnd::OnRender()
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

	g_UIImageRenderMgr.RenderGradeIconBack( m_nGradeType, iXPos+ICON_X, iYPos+ICON_Y, m_fGradeScale );

	// Icon
	if( m_pIcon )
	{
		m_pIcon->Render( iXPos+ICON_X, iYPos+ICON_Y, UI_RENDER_NORMAL, TFO_BILINEAR );
	}
	if( m_pSubIcon )
		m_pSubIcon->Render( iXPos+ICON_X, iYPos+ICON_Y, UI_RENDER_NORMAL, TFO_BILINEAR );

	// UI코스튬 관련 (코스튬 마크)
	if( m_bIsCostume && m_pCostumeMark )
		m_pCostumeMark->Render( iXPos+ICON_X, iYPos+ICON_Y, UI_RENDER_NORMAL, TFO_BILINEAR );

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
		g_FontMgr.PrintText( iXPos+FINAL_DESC_X, iYPos+FINAL_DESC_Y, FONT_SIZE_17, m_szDesc1[GetGashaponArray()].c_str() );
	}
	else
	{
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
		g_FontMgr.PrintText( iXPos+DESC_X, iYPos+DESC_Y, FONT_SIZE_12, STR(6) );

		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
		g_FontMgr.PrintText( iXPos+DESC_X, iYPos+DESC_Y+18, FONT_SIZE_12, STR(7) );
	}

	// Text
	ioComplexStringPrinter kPrinter[2];

	ioHashString szName = "?????";
	ioHashString szDesc = "?????";

	if( m_bFinalResult )
	{
		if(  !g_PresentMgr.GetPresentValue1Text( m_iPresentType, m_iPresentValue1, m_iPresentValue2 ).IsEmpty() )
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
	kPrinter[0].SetTextStyle( TS_NORMAL );
	kPrinter[0].SetBkColor( 0, 0, 0 );	
	kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter[0].AddTextPiece( FONT_SIZE_17, szName.c_str() );
	kPrinter[0].PrintFullText( iXPos+DESC_BIG_X, iYPos+DESC_BIG_Y, TAT_CENTER );

	kPrinter[1].SetTextStyle( TS_NORMAL );
	kPrinter[1].SetBkColor( 0, 0, 0 );	
	kPrinter[1].SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter[1].AddTextPiece( FONT_SIZE_17, szDesc.c_str() );
	kPrinter[1].PrintFullText( iXPos+DESC_BIG_X, iYPos+DESC_BIG_Y+23, TAT_CENTER );
}

void GashaponWnd::iwm_hide()
{
	if( !m_szRollingSound.IsEmpty() )
	{
		g_SoundMgr.StopSound( m_szRollingSound, 0 );
	}
}

void GashaponWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
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
				ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( m_dwPossessEtcItemType );
				if( !pEtcItem )
				{
					LOG.PrintTimeAndLog( 0, "%s pEtcItem == NULL : %d", __FUNCTION__, m_dwPossessEtcItemType );
					return;
				}
				HideWnd();

				pEtcItem->OnUseBtnPush( m_dwPossessEtcItemType, pUserEtcItem );
			}
		}
		break;
	}
}

void GashaponWnd::ReBatchButton( int iType )
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

		bool bPresentShow = false;
		// 선물함이 무조건 활성화되어있으면 아이템 보기 버튼만 활성 --
		MyInventoryWnd *pMyInventoryWnd = dynamic_cast<MyInventoryWnd*>( g_GUIMgr.FindWnd( MY_INVENTORY_WND ) );
		if( pMyInventoryWnd && pMyInventoryWnd->IsShowPresentTab() )
			bPresentShow = true;

		// 예외처리
		ioUserEtcItem *pUserEtcItem = g_MyInfo.GetUserEtcItem();
		if( bPresentShow || !pUserEtcItem )
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
		if( bPresentShow || !pUserEtcItem->GetEtcItem( m_dwPossessEtcItemType, kSlot) )
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

void GashaponWnd::OnResultInfo( short iPresentType, int iPresentValue1, int iPresentValue2, int iPresentValue3, int iPresentValue4, bool bAlram, int iPresentPeso )
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

void GashaponWnd::iwm_show()
{
	if( !GashaponPresentListWnd::GetPresentTypeList().empty() )
		SetGashaponList( GashaponPresentListWnd::GetPresentTypeList(), GashaponPresentListWnd::GetPresentValue1List(), GashaponPresentListWnd::GetPresentValue2List(), GashaponPresentListWnd::GetPresentValue3List(), GashaponPresentListWnd::GetPresentValue4List() );
}

void GashaponWnd::SetPresentIcon()
{
	SAFEDELETE( m_pIcon );
	SAFEDELETE( m_pSubIcon );
	m_bIsCostume = false;

	bool bIconChange = false;
	ioHashString szIconName;
	ioHashString szSubIconName;

	// change icon
	int iMax = m_vIconChangePresentTypeVec[GetGashaponArray()].size();
	int iValue1Size = m_vIconChangePresentValue1Vec[GetGashaponArray()].size();
	int iValue2Size = m_vIconChangePresentValue2Vec[GetGashaponArray()].size();
	int iValue3Size = m_vIconChangePresentValue3Vec[GetGashaponArray()].size();
	int iValue4Size = m_vIconChangePresentValue4Vec[GetGashaponArray()].size();
	int iIconSize = m_vIconChangeIconVec[GetGashaponArray()].size();

	for (int i = 0; i < iMax ; i++)
	{
		if( i >= iValue1Size )
			break;
		if( i >= iValue2Size )
			break;
		if( i >= iValue3Size )
			break;
		if( i >= iValue4Size )
			break;
		if( i >= iIconSize )
			break;

		if( m_iPresentType   != m_vIconChangePresentTypeVec[GetGashaponArray()][i] )
			continue;
		if( m_iPresentValue1 != m_vIconChangePresentValue1Vec[GetGashaponArray()][i] )
			continue;
		if( m_iPresentValue2 != m_vIconChangePresentValue2Vec[GetGashaponArray()][i] )
			continue;
		if( m_iPresentValue3 != m_vIconChangePresentValue3Vec[GetGashaponArray()][i] )
			continue;
		if( m_iPresentValue4 != m_vIconChangePresentValue4Vec[GetGashaponArray()][i] )
			continue;

		szIconName  = m_vIconChangeIconVec[GetGashaponArray()][i];
		if( !szIconName.IsEmpty() )
			bIconChange = true;
		break;
	}

	if( !bIconChange )
	{
		szIconName = g_PresentMgr.GetPresentIconText(m_iPresentType, m_iPresentValue1, m_iPresentValue2);
		szSubIconName = g_PresentMgr.GetPresentSubIconText(m_iPresentType, m_iPresentValue1, m_iPresentValue2);
	}
	if( !szIconName.IsEmpty() )
	{
		m_pIcon = g_UIImageSetMgr.CreateImageByFullName( szIconName );
		m_pSubIcon = g_UIImageSetMgr.CreateImageByFullName( szSubIconName );
	}

	if ( m_iPresentType == PRESENT_COSTUME || m_iPresentType == PRESENT_COSTUME_BOX )
		m_bIsCostume = true;

	m_nGradeType = g_PresentMgr.GetGradeType( m_iPresentType, m_iPresentValue1 );
	m_fGradeScale = FLOAT1;
}

void GashaponWnd::SetGashaponList( IntVec &rPresentTypeList, IntVec &rPresentValue1List, IntVec &rPresentValue2List, IntVec &rPresentValue3List, IntVec &rPresentValue4List )
{
	// 네트워크로 셋팅되는 값은 13번 어레이에 강제 셋팅
	m_vIconList[13].clear();
	m_vCostumeList[13].clear();
	m_vGradeTypeList[13].clear();

	int iSize = rPresentTypeList.size();

	if( iSize != rPresentValue1List.size() || 
		iSize != rPresentValue2List.size() ||
		iSize != rPresentValue3List.size() || 
		iSize != rPresentValue4List.size() )
	{
		LOG.PrintTimeAndLog( 0, "%s Size Error [%d/%d/%d]", __FUNCTION__, iSize, rPresentValue1List.size(), rPresentValue2List.size() );
		return;
	}

	for (int i = 0; i < iSize ; i++)
	{
		ioHashString szIconName = g_PresentMgr.GetPresentIconText(rPresentTypeList[i], rPresentValue1List[i], rPresentValue2List[i]);
		if( szIconName.IsEmpty() )
			continue;

		m_vIconList[13].push_back( szIconName );
		
		if ( rPresentTypeList[i] == PRESENT_COSTUME || rPresentTypeList[i] == PRESENT_COSTUME_BOX )
			m_vCostumeList[13].push_back( true );

		int nGradeType = g_PresentMgr.GetGradeType( rPresentTypeList[i], rPresentValue1List[i] );
		m_vGradeTypeList[13].push_back( nGradeType );
	}
}

int GashaponWnd::GetGashaponArray()
{
	if( m_dwEtcItemType == ioEtcItem::EIT_ETC_GASHAPON )
	{
		return 0;
	}
	else if( COMPARE( m_dwEtcItemType, ioEtcItem::EIT_ETC_GASHAPON2,  ioEtcItem::EIT_ETC_GASHAPON13 + 1) )
	{
		return 1 + ( m_dwEtcItemType - ioEtcItem::EIT_ETC_GASHAPON2 );
	}
	else if( COMPARE( m_dwEtcItemType, ioEtcItem::EIT_ETC_GASHAPON14, ioEtcItem::EIT_ETC_GASHAPON53 + 1) )
	{
		return 13 + ( m_dwEtcItemType - ioEtcItem::EIT_ETC_GASHAPON14 );
	}
	else if( COMPARE( m_dwEtcItemType, ioEtcItem::EIT_ETC_GASHAPON54, ioEtcItem::EIT_ETC_GASHAPON253 + 1 ) )
	{
		return 53 + ( m_dwEtcItemType - ioEtcItem::EIT_ETC_GASHAPON54 );
	}
	else if( COMPARE( m_dwEtcItemType, ioEtcItem::EIT_ETC_ITEM_TIME_GASHAPON1, ioEtcItem::EIT_ETC_ITEM_TIME_GASHAPON100 + 1 ) )
	{
		return 253 + m_dwEtcItemType - ioEtcItem::EIT_ETC_ITEM_TIME_GASHAPON1 ;
	}
	else if( COMPARE( m_dwEtcItemType, ioEtcItem::EIT_ETC_GASHAPON254, ioEtcItem::EIT_ETC_GASHAPON553 + 1 ) )
	{
		return 353 + ( m_dwEtcItemType - ioEtcItem::EIT_ETC_GASHAPON254 );
	}
	else if( COMPARE( m_dwEtcItemType, ioEtcItem::EIT_ETC_ITEM_TIME_GASHAPON101, ioEtcItem::EIT_ETC_ITEM_TIME_GASHAPON300 + 1 ) )
	{
		return 653 + ( m_dwEtcItemType - ioEtcItem::EIT_ETC_ITEM_TIME_GASHAPON101 );	
	}
	else if( COMPARE( m_dwEtcItemType, ioEtcItem::EIT_ETC_GASHAPON554, ioEtcItem::EIT_ETC_GASHAPON853 + 1 ) )
	{
		return 953 + ( m_dwEtcItemType - ioEtcItem::EIT_ETC_GASHAPON554 );
	}
	else if( COMPARE( m_dwEtcItemType, ioEtcItem::EIT_ETC_ITEM_TIME_GASHAPON301, ioEtcItem::EIT_ETC_ITEM_TIME_GASHAPON600 + 1 ) )
	{
		return 1253 + ( m_dwEtcItemType - ioEtcItem::EIT_ETC_ITEM_TIME_GASHAPON301 );
	}

	return 0;
}

void GashaponWnd::PrintManual( int a_iXPos, int a_iYPos , float fScale )
{
	if( !m_pManual )
		return;

	ioEtcItem *pItem = pItem = g_EtcItemMgr.FindEtcItem( m_dwEtcItemType );

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

bool GashaponWnd::IsRolling()
{
	if( m_bResult && !m_bFinalResult )
		return true;

	return false;
}