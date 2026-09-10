#include "StdAfx.h"

#include "../ioExtendSoundManager.h"
#include "newshopBoughtwnd.h"

NewShopBoughtWnd::NewShopBoughtWnd()
{
	m_pIcon          = NULL;
	m_pSubIcon = NULL;
	m_pNum           = NULL;
	m_pPlus          = NULL;
	m_pEffect        = NULL;

	m_pIconBack	= NULL;
	m_pResultIconBack = NULL;
	m_pCostumeMark = NULL;

	m_iPresentType   = 0;
	m_iPresentValue1 = 0;
	m_iPresentValue2 = 0;
	m_bAlarm         = false;

	m_iCurIconArray  = 0;

	m_fIconScale     = 0.0f;
	m_dwIconScaleTime= 0;
	m_dwRollingTime  = 0;
	m_dwRollingTotalTime = 0;

	m_dwRollingCheckTime = 0;
	m_dwRollingEndTime   = 0;
	m_dwScaleStartTime   = 0;

	m_eState = S_ICON_ROLLING;

	m_iCurEffectArray          = 0;
	m_dwRollingEffectTime      = 0;
	m_dwRollingEffectCheckTime = 0;

	m_pStateGreenFrm = NULL;
	m_pStateOrangeFrm = NULL;

	m_bResultState = false;
	m_nGradeType = 0;
	m_fGradeScale = FLOAT1;
}

NewShopBoughtWnd::~NewShopBoughtWnd()
{
	SAFEDELETE( m_pIcon );
	SAFEDELETE( m_pSubIcon );
	SAFEDELETE(m_pNum);
	SAFEDELETE(m_pPlus);
	SAFEDELETE(m_pEffect);

	SAFEDELETE(m_pIconBack);
	SAFEDELETE(m_pResultIconBack);
	SAFEDELETE( m_pCostumeMark );

	SAFEDELETE( m_pStateGreenFrm );
	SAFEDELETE( m_pStateOrangeFrm );

	m_vDescOneLineVec.clear();
	m_vDescTwoLineVec.clear();
	m_vIconList.clear();
	m_vSubIconList.clear();
	m_vRollingEffect.clear();
	m_vEndEfffect.clear();
	m_vGradeList.clear();
}

void NewShopBoughtWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_BUY_ADD_BTN:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
			if( m_pParent )
			{
				ioWnd *pBuyWnd = m_pParent->FindChildWnd( NewShopWnd::ID_BUY_WND );
				if( pBuyWnd )
				{
					pBuyWnd->ShowWnd();
					pBuyWnd->ShowChildWnd( NewShopBuyWnd::ID_FLASH_WND );
				}
			}
		}
		break;
	case ID_GO_BTN:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();

			MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));

			if( m_iPresentType == PRESENT_SOLDIER || m_iPresentType == PRESENT_EXTRAITEM_BOX || m_iPresentType == PRESENT_COSTUME_BOX )
			{
				if( pInvenWnd )
					pInvenWnd->ShowPresentTabDirect();
			}
			else if( m_iPresentType == PRESENT_EXTRAITEM )
			{
				if( pInvenWnd )
					pInvenWnd->ShowExtraItemTabDirect( g_MyInfo.GetClassType() );
			}
			// UI코스튬 관련 (구매완료 후 탭 이동)
			else if( m_iPresentType == PRESENT_COSTUME )
			{
				if( pInvenWnd )
					pInvenWnd->ShowCostumeTabDirect( g_MyInfo.GetClassType() );
			}
		}
		break;
	}
}

void NewShopBoughtWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_dwRollingTime      = xElement.GetIntAttribute_e( "Rolling_Time" );
	m_dwRollingTotalTime = xElement.GetIntAttribute_e( "Rolling_Total_Time" );

	m_fIconScale      = xElement.GetFloatAttribute_e( "Icon_Scale" );
	m_dwIconScaleTime = xElement.GetIntAttribute_e( "Icon_Scale_Time" );

	m_sRollingSound = xElement.GetStringAttribute_e( "Rolling_Sound" );
	m_sResultSound  = xElement.GetStringAttribute_e( "Result_Sound" );

	m_vDescOneLineVec.clear();
	m_vDescTwoLineVec.clear();

	enum { MAX_LOOP = 6, }; // PRESENT_SOLDIER...
	for (int i = 0; i < MAX_LOOP ; i++)
	{
		ioHashString sTemp;
		char szText[MAX_PATH]="";
		StringCbPrintf( szText, sizeof( szText ), "Type%d_Desc1_Text", i+1);
		sTemp = xElement.GetStringAttribute( szText );
		m_vDescOneLineVec.push_back( sTemp );

		StringCbPrintf( szText, sizeof( szText ), "Type%d_Desc2_Text", i+1);
		sTemp = xElement.GetStringAttribute( szText );
		m_vDescTwoLineVec.push_back( sTemp );
	}

	// effect
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

void NewShopBoughtWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	enum
	{
		ICON_X_OFFSET = 129, 
		ICON_Y_OFFSET = 105,

		TITLE_X_OFFSET = 129,
		TITLE_Y_OFFSET = 152,
		TITLE_GAP       = 23,

		DESC_X_OFFSET = 27,
		DESC_Y_OFFSET = 206,
		DESC_GAP      = 18,

		BOUGHT_TEXT_X_OFFSET = 128,
		BOUGHT_TEXT_Y_OFFSET = 402,
	};

	if( m_bResultState && m_pResultIconBack )
	{
		m_pResultIconBack->Render( iXPos+ICON_X_OFFSET, iYPos+ICON_Y_OFFSET, UI_RENDER_MULTIPLY );
	}
	else if( m_pIconBack )
	{
		m_pIconBack->Render( iXPos+ICON_X_OFFSET, iYPos+ICON_Y_OFFSET, UI_RENDER_MULTIPLY );
	}

	g_UIImageRenderMgr.RenderGradeIconBack( m_nGradeType, iXPos+ICON_X_OFFSET, iYPos+ICON_Y_OFFSET, m_fGradeScale );

	if( m_pIcon )
		m_pIcon->Render( iXPos+ICON_X_OFFSET, iYPos+ICON_Y_OFFSET, UI_RENDER_NORMAL, TFO_BILINEAR );
	if( m_pSubIcon )
		m_pSubIcon->Render( iXPos+ICON_X_OFFSET, iYPos+ICON_Y_OFFSET, UI_RENDER_NORMAL, TFO_BILINEAR );

	if ( m_bIsCostume && m_pCostumeMark )
		m_pCostumeMark->Render( iXPos+ICON_X_OFFSET, iYPos+ICON_Y_OFFSET, UI_RENDER_NORMAL, TFO_BILINEAR );

	if( m_iPresentType == PRESENT_EXTRAITEM )
	{
		if( m_eState == S_ICON_SCALING )
		{
			RenderReinforce( iXPos, iYPos, (m_iPresentValue2%PRESENT_EXTRAITEM_DIVISION_1)/PRESENT_EXTRAITEM_DIVISION_2 );

			char szName[MAX_PATH]="";
			int iMarkX = iXPos+ICON_X_OFFSET - 45;
			int iMarkY = iYPos+ICON_Y_OFFSET - 41;

			int iTextX = iMarkX+27;
			int iTextY = iMarkY+2;

			if( m_iTradeType == EET_NORMAL )
			{
				if( m_pStateOrangeFrm )
					m_pStateOrangeFrm->Render( iMarkX, iMarkY, UI_RENDER_NORMAL );

				SafeSprintf( szName, sizeof( szName ), STR(1) );
			}
			else if( m_iTradeType == EET_ENABLE )
			{
				if( m_pStateGreenFrm )
					m_pStateGreenFrm->Render( iMarkX, iMarkY, UI_RENDER_NORMAL );

				SafeSprintf( szName, sizeof( szName ), STR(2) );
			}

			g_FontMgr.SetTextStyle( TS_BOLD );
			g_FontMgr.SetAlignType( TAT_CENTER );
			g_FontMgr.SetBkColor( 0, 0, 0 );
			g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
			g_FontMgr.PrintText( iTextX, iTextY, FONT_SIZE_11, szName );
		}
	}
	else if( m_iPresentType == PRESENT_COSTUME )
	{
		if( m_eState == S_ICON_SCALING )
		{			
			char szName[MAX_PATH]="";
			int iMarkX = iXPos+ICON_X_OFFSET - 45;
			int iMarkY = iYPos+ICON_Y_OFFSET - 41;

			int iTextX = iMarkX+27;
			int iTextY = iMarkY+2;
			
			g_FontMgr.SetTextStyle( TS_BOLD );
			g_FontMgr.SetAlignType( TAT_CENTER );
			g_FontMgr.SetBkColor( 0, 0, 0 );
			g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
			g_FontMgr.PrintText( iTextX, iTextY, FONT_SIZE_11, szName );
		}
	}

	// Effect
	if( m_pEffect )
	{
		m_pEffect->SetScale( 2.0f );
		m_pEffect->Render( iXPos+ICON_X_OFFSET, iYPos+ICON_Y_OFFSET, UI_RENDER_ADD );
	}

	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );

	g_FontMgr.SetTextColor( 58,58,58 );
	g_FontMgr.PrintText( iXPos + TITLE_X_OFFSET, iYPos + TITLE_Y_OFFSET, FONT_SIZE_17, m_sTitle1.c_str() );

	g_FontMgr.SetTextColor( 22, 70, 178 );
	g_FontMgr.PrintText( iXPos + TITLE_X_OFFSET, iYPos + TITLE_Y_OFFSET + TITLE_GAP, FONT_SIZE_17, m_sTitle2.c_str() );

	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetTextColor( 58,58,58 );
	int iArray = m_iPresentType-1;
	if( COMPARE( iArray, 0, (int) m_vDescOneLineVec.size() ) )
		g_FontMgr.PrintText( iXPos + DESC_X_OFFSET, iYPos + DESC_Y_OFFSET, FONT_SIZE_12, m_vDescOneLineVec[iArray].c_str() );
	if( COMPARE( iArray, 0, (int) m_vDescTwoLineVec.size() ) )
		g_FontMgr.PrintText( iXPos + DESC_X_OFFSET, iYPos + DESC_Y_OFFSET + DESC_GAP, FONT_SIZE_12, m_vDescTwoLineVec[iArray].c_str() );

	ioWnd *pWnd = FindChildWnd( ID_BUY_ADD_BTN );
	if( pWnd && pWnd->IsShow() )
	{
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_GREEN );
		g_FontMgr.PrintText( iXPos + BOUGHT_TEXT_X_OFFSET, iYPos + BOUGHT_TEXT_Y_OFFSET, FONT_SIZE_17, STR(4) );

		g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
		g_FontMgr.PrintText( pWnd->GetDerivedPosX() + 102, pWnd->GetDerivedPosY() - 28, FONT_SIZE_12, STR(5) );
		g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
		g_FontMgr.PrintText( pWnd->GetDerivedPosX() + 102, pWnd->GetDerivedPosY() - 15, FONT_SIZE_12, STR(6) );
		return; // 구매된창이면 price 생략
	}
}

void NewShopBoughtWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	ioWnd::OnProcess( fTimePerSec );
	DWORD dwCurTime = FRAMEGETTIME();

	ProcessResult( dwCurTime );
	ProcessScale( dwCurTime );
	ProcessIcon( dwCurTime );
	RollingEffect( fTimePerSec );
}

void NewShopBoughtWnd::SetResultInfo( short iPresentType, int iPresentValue1, int iPresentValue2, bool bAlarm, int iTradeType )
{
	m_iPresentType   = iPresentType;
	m_iPresentValue1 = iPresentValue1;
	m_iPresentValue2 = iPresentValue2;
	m_bAlarm         = bAlarm;
	m_eState         = S_RESULT_SETTING;

	m_iTradeType = iTradeType;
	m_bResultState = false;
	m_nGradeType = 0;
	m_fGradeScale = FLOAT1;

	if ( m_iPresentType == PRESENT_COSTUME || m_iPresentType == PRESENT_COSTUME_BOX )
		m_bIsCostume = true;
	else
		m_bIsCostume = false;
}

void NewShopBoughtWnd::ProcessResult( DWORD dwCurTime )
{
	if( m_eState != S_RESULT_SETTING )
		return;

	if( m_dwRollingEndTime > dwCurTime )
		return;

	if( m_iPresentType == PRESENT_SOLDIER )
	{
		ioHashString szIcon = g_MyInfo.GetMySoldierIconName( m_iPresentValue1 );
		ioHashString szSubIcon = g_MyInfo.GetSoldierSubIconName( m_iPresentValue1 );
		if( !szIcon.IsEmpty() )
		{
			SAFEDELETE( m_pIcon );
			SAFEDELETE( m_pSubIcon );
			m_pIcon = g_UIImageSetMgr.CreateImageByFullName( szIcon );
			m_pSubIcon = g_UIImageSetMgr.CreateImageByFullName( szSubIcon );
		}
		m_nGradeType = g_UIImageRenderMgr.GetPowerUpGradeType( m_iPresentValue1 );
	}
	else if( m_iPresentType == PRESENT_EXTRAITEM )
	{
		const ioItem *pItem = g_ItemMaker.GetItemConst( m_iPresentValue1, __FUNCTION__ );
		if( pItem )
		{
			ioHashString szIcon = pItem->GetItemLargeIconName();
			if( !szIcon.IsEmpty() )
			{
				SAFEDELETE( m_pIcon );
				SAFEDELETE( m_pSubIcon );
				m_pIcon = g_UIImageSetMgr.CreateImageByFullName( szIcon );
			}
			m_nGradeType = pItem->GetGradeType();
		}
	}
	// UI코스튬 관련 (아이콘 얻기)
	else if( m_iPresentType == PRESENT_COSTUME )
	{
		ioHashString szIcon = g_CostumeInfoMgr.GetCostumeIcon( m_iPresentValue1 );
		if( !szIcon.IsEmpty() )
		{
			SAFEDELETE( m_pIcon );
			SAFEDELETE( m_pSubIcon );
			m_pIcon = g_UIImageSetMgr.CreateImageByFullName( szIcon );
		}
		m_nGradeType = g_CostumeInfoMgr.GetGradeType( m_iPresentValue1 );
	}

	m_iCurEffectArray = -1;
	m_dwRollingEffectCheckTime = 0;

	m_sTitle1 = g_PresentMgr.GetPresentValue1Text( m_iPresentType, m_iPresentValue1, m_iPresentValue2 );
	m_sTitle2 = g_PresentMgr.GetPresentValue2Text( m_iPresentType, m_iPresentValue1, m_iPresentValue2 );

	if( !m_sRollingSound.IsEmpty() )
		g_SoundMgr.StopSound( m_sRollingSound, 0 );

	if( !m_sResultSound.IsEmpty() )
		g_SoundMgr.PlaySound( m_sResultSound );

	HideChildWnd( ID_ROLLING_BTN );
	ShowChildWnd( ID_BUY_ADD_BTN );
	ShowChildWnd( ID_GO_BTN );
	m_dwScaleStartTime = dwCurTime;
	m_eState = S_ICON_SCALING;

	m_bResultState = true;
	g_ExSoundMgr.PlaySound( ExSound::EST_SHOP_BUY_SUCCESS );

	if( m_iPresentType == PRESENT_EXTRAITEM )
	{
		g_QuestMgr.QuestCompleteTerm( QC_EXTRA_ITEM_ACQUIRE, m_iPresentValue1, m_iPresentValue2 % 10000 );
	}
}

void NewShopBoughtWnd::ProcessScale( DWORD dwCurTime )
{
	if( m_eState != S_ICON_SCALING )
		return;

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
}

void NewShopBoughtWnd::ProcessIcon( DWORD dwCurTime )
{
	if( m_eState != S_ICON_ROLLING && m_eState != S_RESULT_SETTING )
		return;

	if( m_vIconList.empty() )
		return;

	if( m_dwRollingCheckTime != 0 && dwCurTime - m_dwRollingCheckTime < m_dwRollingTime )		
		return;

	SAFEDELETE( m_pIcon );
	SAFEDELETE( m_pSubIcon );

	m_iCurIconArray++;

	if( !COMPARE( m_iCurIconArray, 0, (int)m_vIconList.size() ) )
		m_iCurIconArray = 0;

	ioHashString szCurIcon = m_vIconList[m_iCurIconArray];
	ioHashString szSubCurIcon = m_vSubIconList[m_iCurIconArray];
	if( !szCurIcon.IsEmpty() )
	{
		m_pIcon = g_UIImageSetMgr.CreateImageByFullName( szCurIcon );
		m_pSubIcon = g_UIImageSetMgr.CreateImageByFullName( szSubCurIcon );
	}

	m_nGradeType = 0;
	if( COMPARE( m_iCurIconArray, 0, (int)m_vGradeList.size() ) )
		m_nGradeType = m_vGradeList[m_iCurIconArray];

	m_dwRollingCheckTime = dwCurTime;
}

void NewShopBoughtWnd::RollingEffect( float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();

	int iEffectListCnt = m_vRollingEffect.size();
	if( m_eState == S_ICON_SCALING )
		iEffectListCnt = m_vEndEfffect.size();

	if( iEffectListCnt == 0 )
	{
		SAFEDELETE( m_pEffect );
		return;
	}

	if( m_eState == S_ICON_SCALING )
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

void NewShopBoughtWnd::iwm_hide()
{
	if( !m_sRollingSound.IsEmpty() )
	{
		g_SoundMgr.StopSound( m_sRollingSound, 0 );
	}
}

void NewShopBoughtWnd::ShowBoughtWnd( short iPresentType, int iPresentValue1, int iPresentValue2 )
{
	ShowWnd();
	m_eState = S_ICON_ROLLING;

	m_bResultState = false;
	m_bIsCostume = false;

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwRollingEndTime = dwCurTime + m_dwRollingTotalTime;
	m_dwRollingCheckTime = 0;

	m_sTitle1 = "????";
	m_sTitle2 = "????";

	m_iCurIconArray = -1;
	SAFEDELETE( m_pIcon );
	SAFEDELETE( m_pSubIcon );

	// effect
	m_dwRollingEffectCheckTime = 0;
	m_iCurEffectArray = -1;
	SAFEDELETE(m_pEffect);

	ShowChildWnd( ID_ROLLING_BTN );
	HideChildWnd( ID_BUY_ADD_BTN );
	HideChildWnd( ID_GO_BTN );

	if( !m_sRollingSound.IsEmpty() )
		g_SoundMgr.PlaySound( m_sRollingSound, DSBVOLUME_MAX, PSM_LOOP | PSM_THREAD );

	m_vIconList.clear();
	m_vSubIconList.clear();
	m_vGradeList.clear();
	if( iPresentType == PRESENT_SOLDIER )
	{
		int iMax = g_ClassPrice.MaxClassPrice();
		for (int i = 0; i <  iMax; i++)
		{
			if( !g_ClassPrice.IsActiveClass( i ) )
				continue;

			int iSetIdx = g_ClassPrice.ArrayToSetItemIdx( i );
			const ioSetItemInfo *pInfo = g_SetItemInfoMgr.GetSetInfoByIdx( iSetIdx );
			if( !pInfo ) continue;

			int iClassType = pInfo->GetSetCode() - SET_ITEM_CODE;
			m_vIconList.push_back( g_MyInfo.GetMySoldierIconName( iClassType ) );
			m_vSubIconList.push_back( g_MyInfo.GetSoldierSubIconName( iClassType ) );
			m_vGradeList.push_back( g_UIImageRenderMgr.GetPowerUpGradeType( iClassType ) );
		}
	}
	else if( iPresentType == PRESENT_EXTRAITEM_BOX )
	{
		bool bPackage = false;
		RandomMachineInfo *pInfo = g_ExtraItemInfoMgr.GetRandomMachineInfo( iPresentValue1 );
		if( pInfo )
		{
			if( pInfo->IsPackage() )
			{
				bPackage = true;
				m_vIconList.push_back( pInfo->GetIconName() );
				m_vSubIconList.push_back( "" );
				m_vGradeList.push_back( pInfo->GetGradeType() );
			}
		}

		if( !bPackage )
		{
			ItemListInfoSortVector vTotalList;
			g_ExtraItemInfoMgr.GetRandomItemList( iPresentValue1, vTotalList );
			for(ItemListInfoSortVector::iterator iter = vTotalList.begin(); iter != vTotalList.end(); ++iter)
			{
				ItemListInfoSort &rkInfo = (*iter);
				const ioItem *pItem = g_ItemMaker.GetItemConst( rkInfo.m_iItemCode, __FUNCTION__ );
				if( !pItem )
					continue;
				m_vIconList.push_back( pItem->GetItemLargeIconName() );
				m_vSubIconList.push_back( "" );
				m_vGradeList.push_back( pItem->GetGradeType() );
			}
		}
	}
	// UI코스튬 관련 (구매 완료후 윈도우)
	else if( iPresentType == PRESENT_COSTUME_BOX )
	{		
		bool bPackage = false;
		CostumeShopInfo *pInfo = g_CostumeShopInfoMgr.GetCostumeMachineInfo( iPresentValue1 );
		if( pInfo )
		{
			if( pInfo->IsPackage() )
			{
				bPackage = true;
				m_vIconList.push_back( pInfo->GetIconName() );
				m_vSubIconList.push_back( "" );
				m_vGradeList.push_back( pInfo->GetGradeType() );
			}
		}

		if( !bPackage )
		{
			CostumeShopSubInfoList vTotalList;
			g_CostumeShopInfoMgr.GetCostumeItemList( iPresentValue1, vTotalList );

			CostumeShopSubInfoList::iterator iter = vTotalList.begin();
			for( ; iter!=vTotalList.end(); ++iter )
			{
				CostumeShopSubInfo &rkInfo = (*iter);
				CostumeInfo sCostumeInfo;
				if ( g_CostumeInfoMgr.GetCostumeInfo( rkInfo.m_nCostumeCode, sCostumeInfo ) )
				{
					m_vIconList.push_back( sCostumeInfo.m_szIconName );
					m_vSubIconList.push_back( "" );
					m_vGradeList.push_back( sCostumeInfo.m_nGradeType );
				}
			}
		}

		m_bIsCostume = true;
	}

	if( (int)m_vIconList.size() == 1)
	{
		m_dwRollingEndTime = dwCurTime + 200;
	}
}

void NewShopBoughtWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( strcmp(szType.c_str(), "Num") == 0 )
	{
		SAFEDELETE( m_pNum );
		m_pNum = pImage;
	}
	else if( szType == "Plus" )
	{
		SAFEDELETE( m_pPlus );
		m_pPlus = pImage;
	}
	else if( szType == "back_effect" )
	{
		SAFEDELETE( m_pIconBack );
		m_pIconBack = pImage;
	}
	else if( szType == "final_back_effect" )
	{
		SAFEDELETE( m_pResultIconBack );
		m_pResultIconBack = pImage;
	}
	else if( szType == "costume_image" )
	{
		SAFEDELETE( m_pCostumeMark );
		m_pCostumeMark = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}

void NewShopBoughtWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( pFrame )
	{
		if( szType == "StateOrangeFrm" )
		{
			SAFEDELETE( m_pStateOrangeFrm );
			m_pStateOrangeFrm = pFrame;
		}
		else if( szType == "StateGreenFrm" )
		{
			SAFEDELETE( m_pStateGreenFrm );
			m_pStateGreenFrm = pFrame;
		}
		else
		{	
			ioWnd::AddRenderFrame( szType, pFrame );
		}
	}
}

void NewShopBoughtWnd::RenderReinforce( int iXPos, int iYPos, int iReinforce )
{
	enum
	{
		REINFORCE_X	= 126,
		REINFORCE_Y	= 124,

		TEXT_SIZE	= 16,
		TEXT_GAP	= 4,
	};

	int iCurPos = 0;
	int iDefaultPos = iXPos+REINFORCE_X;

	int iTotalSize = 0;
	if( iReinforce < 10 )
		iTotalSize = TEXT_SIZE * 2 - TEXT_GAP;
	else if( iReinforce < 100 )
		iTotalSize = TEXT_SIZE * 3 - TEXT_GAP * 2;
	else
		iTotalSize = TEXT_SIZE * 4 - TEXT_GAP * 3;

	iCurPos = iDefaultPos - (iTotalSize / 2);
	if( m_pPlus )
		m_pPlus->Render( iCurPos, iYPos+REINFORCE_Y , UI_RENDER_NORMAL, TFO_BILINEAR );

	iCurPos += (TEXT_SIZE - TEXT_GAP);
	if( m_pNum )
		m_pNum->RenderNum( iCurPos, iYPos+REINFORCE_Y, iReinforce, -4.0f, FLOAT1, UI_RENDER_NORMAL, TFO_BILINEAR );
}