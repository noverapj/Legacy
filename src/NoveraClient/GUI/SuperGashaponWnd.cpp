#include "StdAfx.h"

#include "SuperGashaponWnd.h"
#include "../ioSuperGashaponMgr.h"
#include "../ioExtendSoundManager.h"

SuperGashaponWnd::SuperGashaponWnd()
{
	m_pIcon				= NULL;
	m_pSubIcon = NULL;
	m_pNum				= NULL;
	m_pPlus				= NULL;
	m_pEffect			= NULL;

	m_pIconBack			= NULL;
	m_pResultIconBack	= NULL;
	m_pGoodsNoneIcon	= NULL;
	m_pGoodsBackIcon	= NULL;
	m_pCostumeMark = NULL;
	m_pCostumeMarkSmall = NULL;
	m_CurrBoughtIndex	= 0;
	m_iCurIconArray		= 0;

	m_fIconScale		 = 0.0f;
	m_dwIconScaleTime	 = 0;
	m_dwRollingTime		 = 0;
	m_dwRollingTotalTime = 0;

	m_dwRollingCheckTime = 0;
	m_dwRollingEndTime   = 0;
	m_dwScaleStartTime   = 0;
	m_dwScaleEndTime	 = 0;
	m_dwCurVertPos		 = 0;

	m_eState			  = S_ROLLING;
	m_SuperGashponUseType = SUPERGASHPON_BUY;
	m_bSound			  = false;

	m_iCurEffectArray          = 0;
	m_dwRollingEffectTime      = 0;
	m_dwRollingEffectCheckTime = 0;
	m_bIsCostume = false;

	m_pStateGreenFrm = NULL;
	m_pStateOrangeFrm = NULL;
	m_nGradeType = 0;
}

SuperGashaponWnd::~SuperGashaponWnd()
{
	DestoryInfo();

	SAFEDELETE( m_pNum );
	SAFEDELETE( m_pPlus );
	SAFEDELETE( m_pEffect );

	SAFEDELETE( m_pIconBack );
	SAFEDELETE( m_pResultIconBack );
	SAFEDELETE( m_pGoodsNoneIcon );
	SAFEDELETE( m_pGoodsBackIcon );
	SAFEDELETE( m_pCostumeMark );
	SAFEDELETE( m_pCostumeMarkSmall );

	SAFEDELETE( m_pStateGreenFrm );
	SAFEDELETE( m_pStateOrangeFrm );
	
	m_vIconList.clear();
	m_vCostume.clear();
	m_vRollingEffect.clear();
	m_dwRollingTime = 0;
	m_vEndEfffect.clear();
}

void SuperGashaponWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
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
	else if( szType == "GoodsNoneIcon" )
	{
		SAFEDELETE( m_pGoodsNoneIcon );
		m_pGoodsNoneIcon = pImage;
		if( m_pGoodsNoneIcon )
			m_pGoodsNoneIcon->SetScale( FLOAT05, FLOAT05 );
	}
	else if( szType == "GoodsBackIcon" )
	{
		SAFEDELETE( m_pGoodsBackIcon );
		m_pGoodsBackIcon = pImage;
		if( m_pGoodsBackIcon )
			m_pGoodsBackIcon->SetScale( FLOAT05, FLOAT05 );
	}
	else if( szType == "costume_image" )
	{
		SAFEDELETE( m_pCostumeMark );
		m_pCostumeMark = pImage;
	}
	else if( szType == "costume_small_image" )
	{
		SAFEDELETE( m_pCostumeMarkSmall );
		m_pCostumeMarkSmall = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}

void SuperGashaponWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
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

void SuperGashaponWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_dwRollingTime      = xElement.GetIntAttribute_e( "Rolling_Time" );
	m_dwRollingTotalTime = xElement.GetIntAttribute_e( "Default_Rolling_Total_Time" );

	m_fIconScale		 = xElement.GetFloatAttribute_e( "Icon_Scale" );
	m_dwIconScaleTime	 = xElement.GetIntAttribute_e( "Icon_Scale_Time" );

	m_sRollingSound		 = xElement.GetStringAttribute_e( "Rolling_Sound" );
	m_sResultSound		 = xElement.GetStringAttribute_e( "Result_Sound" );

	ParseExtraEffectInfo( xElement );
	ParseExtraEndEffectInfo( xElement );
}

void SuperGashaponWnd::ParseExtraEffectInfo( ioXMLElement &xElement )
{
	// effect
	m_dwRollingEffectTime = xElement.GetIntAttribute_e( "Effect_Gap_Time" );
	m_vRollingEffect.clear();

	int iMax = xElement.GetIntAttribute_e( "Max_Rolling_Effect" );
	m_vRollingEffect.reserve( iMax );
	for ( int i = 0; i < iMax ; ++i )
	{
		char szName[MAX_PATH]="";
		StringCbPrintf( szName, sizeof( szName ), "Rolling_Effect_%d", i+1 );
		ioHashString szEffect = xElement.GetStringAttribute( szName );
		if( szEffect == "" )
			break;
		m_vRollingEffect.push_back( szEffect );
	}
}

void SuperGashaponWnd::ParseExtraEndEffectInfo( ioXMLElement &xElement )
{
	m_vEndEfffect.clear();
	int iMax = xElement.GetIntAttribute_e( "Max_End_Effect" );
	m_vEndEfffect.reserve( iMax );
	for ( int i = 0; i < iMax ; ++i )
	{
		char szName[MAX_PATH]="";
		StringCbPrintf_e( szName, sizeof( szName ), "End_Effect_%d", i+1 );
		ioHashString szEffect = xElement.GetStringAttribute( szName );
		if( szEffect == "" )
			break;
		m_vEndEfffect.push_back( szEffect );
	}
}

void SuperGashaponWnd::iwm_hide()
{
	if( !m_sRollingSound.IsEmpty() )
	{
		g_SoundMgr.StopSound( m_sRollingSound, 0 );
	}

	if( m_bSound && m_SuperGashponUseType != SUPERGASHPON_BUY )
	{
		m_bSound = false;
		g_ExSoundMgr.PlaySound( ExSound::EST_GASHAPON_GET );
	}

	DestoryInfo();
	ResetVertScroll();
	m_bIsCostume = false;

	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>( g_GUIMgr.FindWnd(MY_INVENTORY_WND) );
	if( pInvenWnd && pInvenWnd->IsShow() && m_SuperGashponUseType == SUPERGASHPON_USE )
		pInvenWnd->ChangeMainTab( MyInventoryWnd::ID_PRESENT_TAB_BTN );
}

void SuperGashaponWnd::DestoryInfo()
{
	SAFEDELETE( m_pIcon );
	SAFEDELETE( m_pSubIcon );
	BuyGoodsVec::iterator iter = m_BuyGoodsVec.begin();
	for( ; iter != m_BuyGoodsVec.end(); ++iter )
	{
		BuyGoods& rkGoods = *iter;
		SAFEDELETE( rkGoods.m_pGoodsIcon );
		SAFEDELETE( rkGoods.m_pSubIcon );
	}

	m_BuyGoodsVec.clear();
	m_ShopBoughtInfoVec.clear();
}

void SuperGashaponWnd::ShowSuperGashaponWnd( short iPresentType, int iPresentValue1, int iPresentValue2, SuperGashponUseType UseType )
{
	m_eState = S_ROLLING;
	m_SuperGashponUseType = UseType;
	m_bSound = true;
	m_iCurEffectArray = 0;
	m_dwCurVertPos	  = 0;
	m_nGradeType = 0;
	m_fGradeScale = FLOAT1;

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwRollingEndTime = dwCurTime + m_dwRollingTotalTime;
	m_dwRollingCheckTime = 0;

	m_sTitle1 = "????";
	m_sTitle2 = "????";

	m_iCurIconArray = -1;
	SAFEDELETE( m_pIcon );
	SAFEDELETE( m_pSubIcon );
	m_bIsCostume = false;

	// effect
	m_dwRollingEffectCheckTime = 0;
	SAFEDELETE(m_pEffect);

	ShowChildWnd( ID_ROLLING_BTN );
	HideChildWnd( ID_BUY_ADD_BTN );
	HideChildWnd( ID_GO_BTN );
	HideChildWnd( ID_VIEW_BTN );

	if( !m_sRollingSound.IsEmpty() )
		g_SoundMgr.PlaySound( m_sRollingSound, DSBVOLUME_MAX, PSM_LOOP | PSM_THREAD );

	m_vIconList.clear();
	m_vCostume.clear();
	m_vGradeList.clear();
	m_vIconList = g_SuperGashaponMgr.GetRollIconVec( iPresentValue1 );
	m_vCostume = g_SuperGashaponMgr.GetCostumeVec( iPresentValue1 );
	m_vGradeList = g_SuperGashaponMgr.GetGradeVec( iPresentValue1 );
	DWORD dwRollingTotalTime = g_SuperGashaponMgr.GetRollingTime( iPresentValue1 );
	if( dwRollingTotalTime != 0)
		m_dwRollingTotalTime = dwRollingTotalTime;

	if( (int)m_vIconList.size() == 1)
	{
		m_dwRollingEndTime = dwCurTime + 200;
	}

	SetVertScroll();
	ShowWnd();
}
void SuperGashaponWnd::ShowAddBuy()
{
	NewShopWnd *pNewShopWnd = dynamic_cast<NewShopWnd*> ( g_GUIMgr.FindWnd( NEW_SHOP_WND ) );
	if( pNewShopWnd )
	{
		ioWnd *pBuyWnd = pNewShopWnd->FindChildWnd( NewShopWnd::ID_BUY_WND );
		if( pBuyWnd )
			pBuyWnd->ShowWnd();
	}
}

SuperGashaponWnd::ShopBoughtInfo& SuperGashaponWnd::GetBoughtInfo( UINT Index )
{
	static ShopBoughtInfo newInfo;
	if( m_ShopBoughtInfoVec.size() <= Index )	
		return newInfo;

	return m_ShopBoughtInfoVec[Index];
}

void SuperGashaponWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_BUY_ADD_BTN:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
			ShowAddBuy();
		}
		break;
	case ID_GO_BTN:
	case ID_VIEW_BTN:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();

			MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>( g_GUIMgr.FindWnd(MY_INVENTORY_WND) );
			if( pInvenWnd )
				pInvenWnd->ShowPresentTabDirect();
		}
		break;
	}
}

void SuperGashaponWnd::iwm_vscroll(DWORD dwID, int curPos )
{
	if( dwID == ID_VERT_SCROLL )
	{
		m_dwCurVertPos = curPos;
	}
}

void SuperGashaponWnd::SetVertScroll()
{
	//스크롤 설정
	ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
	if( pScroll )
	{		
		pScroll->SetScrollRange( 0, m_BuyGoodsVec.size() );
		pScroll->SetScrollPos( m_dwCurVertPos );
	}
}

void SuperGashaponWnd::ResetVertScroll()
{
	//스크롤 설정
	ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
	if( pScroll )
	{
		pScroll->SetScrollRange( 0, 0 );
		pScroll->SetScrollPos( 0 );
	}
}

void SuperGashaponWnd::AddResultInfo( short iPresentType, int iPresentValue1, int iPresentValue2, int iTradeType )
{
	ShopBoughtInfo info;
	info.m_iPresentType   = iPresentType;
	info.m_iPresentValue1 = iPresentValue1;
	info.m_iPresentValue2 = iPresentValue2;
	info.m_iTradeType	  = iTradeType;

	m_ShopBoughtInfoVec.push_back( info );
}

void SuperGashaponWnd::AddBuyGoodsList( const ShopBoughtInfo& rkInfo, const ioHashString& szTitle, const ioHashString& szSubTitle )
{
	ioHashString szImage = g_PresentMgr.GetPresentIconText( rkInfo.m_iPresentType, rkInfo.m_iPresentValue1, rkInfo.m_iPresentValue2 );
	ioHashString szSubImage = g_PresentMgr.GetPresentSubIconText( rkInfo.m_iPresentType, rkInfo.m_iPresentValue1, rkInfo.m_iPresentValue2 );
	if( strcmp( szImage.c_str(), "None" ) == 0 )
	{
		LOG.PrintTimeAndLog(0 ,"error func : %s, icon name empty", __FUNCTION__  );
		return;
	}

	BuyGoods Good;
	Good.m_pGoodsIcon = g_UIImageSetMgr.CreateImageByFullName( szImage );
	Good.m_pSubIcon = g_UIImageSetMgr.CreateImageByFullName( szSubImage );
	if( rkInfo.m_iPresentType == PRESENT_EXTRAITEM )
	{
		char szBuffer[MAX_PATH];
		int iReinForce = (rkInfo.m_iPresentValue2 % PRESENT_EXTRAITEM_DIVISION_1) / PRESENT_EXTRAITEM_DIVISION_2;
		sprintf_s_e( szBuffer, "%s + %d", szTitle.c_str(), iReinForce );
		Good.m_szTitle = szBuffer;
	}
	else
	{
		Good.m_szTitle = szTitle;
	}
	Good.m_szSubTitle = szSubTitle;

	if( rkInfo.m_iPresentType == PRESENT_COSTUME || rkInfo.m_iPresentType == PRESENT_COSTUME_BOX )
		Good.m_bIsCostume = true;

	if( Good.m_pGoodsIcon )
		Good.m_pGoodsIcon->SetScale( FLOAT05 );
	if( Good.m_pSubIcon )
		Good.m_pSubIcon->SetScale( FLOAT05 );
	if( m_pCostumeMarkSmall )
		m_pCostumeMarkSmall->SetScale( FLOAT05 );
	
	Good.m_nGradeType = g_PresentMgr.GetGradeType( rkInfo.m_iPresentType, rkInfo.m_iPresentValue1 );
	m_BuyGoodsVec.push_back( Good );

	m_dwCurVertPos = max( 0, m_BuyGoodsVec.size() );
	SetVertScroll();
}

void SuperGashaponWnd::ShowResultSuperGashaponWnd( DWORD dwEtcItemCode, DWORD dwPresentIndex, int iProductType )
{
	m_BuyGoodsVec.clear();
	m_ShopBoughtInfoVec.clear();
	m_CurrBoughtIndex = 0;
	m_dwCurVertPos	  = 0;
	
	ioSuperGashaponMgr::SuperGashaponPackage rkPresent;
	if( iProductType == SUPER_GASHPON_MAIN )
		rkPresent = g_SuperGashaponMgr.GetSuperGashaponPackage( dwEtcItemCode, dwPresentIndex );
	else
		rkPresent = g_SuperGashaponMgr.GetSuperGashaponSubPackage( dwEtcItemCode, dwPresentIndex );
	
	ioSuperGashaponMgr::vSuperGashaponElement::const_iterator iter = rkPresent.m_vSuperGashaponElement.begin();
	for( ; iter < rkPresent.m_vSuperGashaponElement.end(); ++iter )
	{
		ioSuperGashaponMgr::SuperGashaponElement rkElement = *iter;
		AddResultInfo( rkElement.m_iPresentType, rkElement.m_iPresentValue1, rkElement.m_iPresentValue2, EET_DISABLE );
	}
}

bool SuperGashaponWnd::IsRolling()
{
	if( m_eState == S_ROLLING )
		return true;

	return false;
}

void SuperGashaponWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	ioWnd::OnProcess( fTimePerSec );

	DWORD dwCurTime = FRAMEGETTIME();
	switch( m_eState )
	{
	case S_ROLLING:
		{
			ProcessRollingIcon( dwCurTime );
			CheckRollingEnd( dwCurTime );
			ChangeRollingEffect( m_vRollingEffect, true );
		}
		break;
	case S_UPDATE_INFO:
		{
			ProcessGoodsInfo( dwCurTime );
		}
		break;
	case S_UP_SCALING:
		{
			ProcessUpScale( dwCurTime );
			ChangeRollingEffect( m_vEndEfffect );
		}
		break;
	case S_DOWN_SCALING:
		{
			ProcessDownScale( dwCurTime );
			ChangeRollingEffect( m_vEndEfffect );
			ProcessScaleEndCheck( dwCurTime );
		}
		break;
	case S_END:
		{
			ChangeRollingEffect( m_vEndEfffect );

			if( m_bSound )
			{
				m_bSound = false;
				if( m_SuperGashponUseType == SUPERGASHPON_BUY )
					g_ExSoundMgr.PlaySound( ExSound::EST_SHOP_BUY_SUCCESS );
				else
					g_ExSoundMgr.PlaySound( ExSound::EST_GASHAPON_GET );
			}
		}
	}
}

void SuperGashaponWnd::ProcessRollingIcon( DWORD dwCurTime )
{
	if( m_vIconList.empty() )
		return;

	if( m_dwRollingCheckTime != 0 && dwCurTime - m_dwRollingCheckTime < m_dwRollingTime )
		return;

	SAFEDELETE( m_pIcon );
	SAFEDELETE( m_pSubIcon );
	m_bIsCostume = false;
	m_iCurIconArray++;

	if( !COMPARE( m_iCurIconArray, 0, (int)m_vIconList.size() ) )
		m_iCurIconArray = 0;

	ioHashString szCurIcon = m_vIconList[m_iCurIconArray];
	if( !szCurIcon.IsEmpty() )
	{
		m_pIcon = g_UIImageSetMgr.CreateImageByFullName( szCurIcon );
	}

	m_bIsCostume = m_vCostume[m_iCurIconArray];
	m_dwRollingCheckTime = dwCurTime;

	m_nGradeType = 0;
	if( COMPARE( m_iCurIconArray, 0, (int)m_vGradeList.size() ) )
		m_nGradeType = m_vGradeList[m_iCurIconArray];
}

void SuperGashaponWnd::CheckRollingEnd( DWORD dwCurTime )
{
	if( m_dwRollingEndTime > dwCurTime )
		return;

	if( m_ShopBoughtInfoVec.empty() )
	{
		m_eState = S_END;
		ShowAddBuy();
		HideWnd();
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "구매에 실패 하였습니다.# 잠시후 다시 시도해주세요" );
		return;
	}
	else
	{
		m_eState = S_UPDATE_INFO;
	}
}

void SuperGashaponWnd::ChangeRollingEffect( ioHashStringVec& vEffect, bool bLoop )
{	
	if( bLoop && vEffect.size() <= m_iCurEffectArray )
		m_iCurEffectArray = 0;

	if( !bLoop && vEffect.size() <= m_iCurEffectArray )
	{
		SAFEDELETE( m_pEffect );
		return;
	}

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwRollingEffectCheckTime == 0 || dwCurTime - m_dwRollingEffectCheckTime > m_dwRollingEffectTime )
	{
		SAFEDELETE( m_pEffect );
		ioHashString szEffect = vEffect[m_iCurEffectArray++];

		if( !szEffect.IsEmpty() )
			m_pEffect = g_UIImageSetMgr.CreateImageByFullName( szEffect );		

		m_dwRollingEffectCheckTime = dwCurTime;
	}
}

void SuperGashaponWnd::ProcessGoodsInfo( DWORD dwCurTime )
{
	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>( g_GUIMgr.FindWnd(MY_INVENTORY_WND) );

	if( m_CurrBoughtIndex < m_ShopBoughtInfoVec.size() - 1 )
	{
		const ShopBoughtInfo& rkInfo = GetBoughtInfo( m_CurrBoughtIndex );
		UpdateGoodsMainInfo( dwCurTime, rkInfo );
		AddBuyGoodsList( rkInfo, m_sTitle1, m_sTitle2 );

		//인벤토리에 임시로 받은 상품 등록
		if( pInvenWnd && pInvenWnd->IsShow() && m_SuperGashponUseType == SUPERGASHPON_USE )
			pInvenWnd->AddItemInfoVecFromPresent( MyInventoryWnd::ID_PRESENT_TAB_BTN, rkInfo.m_iPresentValue1, rkInfo.m_iPresentValue2 );
	}
	else if( m_CurrBoughtIndex == m_ShopBoughtInfoVec.size() - 1 )
	{
		const ShopBoughtInfo& rkInfo = GetBoughtInfo( m_CurrBoughtIndex );
		UpdateGoodsMainInfo( dwCurTime, rkInfo );
		AddBuyGoodsList( rkInfo, m_sTitle1, m_sTitle2 );

		HideChildWnd( ID_ROLLING_BTN );
		ShowChildWnd( ID_BUY_ADD_BTN );
		switch( m_SuperGashponUseType )
		{
		case SUPERGASHPON_USE:
			ShowChildWnd( ID_VIEW_BTN );
			break;
		case SUPERGASHPON_BUY:
			ShowChildWnd( ID_GO_BTN );
			break;
		}

		m_eState = S_END;

		//마지막 상품이 되었을때 인벤토리 갱신
		if( pInvenWnd && pInvenWnd->IsShow() && m_SuperGashponUseType == SUPERGASHPON_USE )
		{
			pInvenWnd->ChangeMainTab( MyInventoryWnd::ID_PRESENT_TAB_BTN );
			pInvenWnd->SetFirstBtnInfo();
		}
	}
}

void SuperGashaponWnd::UpdateGoodsMainInfo( DWORD dwCurTime, const ShopBoughtInfo& rkInfo )
{
	SAFEDELETE( m_pIcon );
	SAFEDELETE( m_pSubIcon );
	m_bIsCostume = false;
	m_pIcon = g_UIImageSetMgr.CreateImageByFullName( g_PresentMgr.GetPresentIconText( rkInfo.m_iPresentType, rkInfo.m_iPresentValue1, rkInfo.m_iPresentValue2 ) );
	m_pSubIcon = g_UIImageSetMgr.CreateImageByFullName( g_PresentMgr.GetPresentSubIconText( rkInfo.m_iPresentType, rkInfo.m_iPresentValue1, rkInfo.m_iPresentValue2 ) );
	m_dwRollingEffectCheckTime = 0;

	m_sTitle1 = g_PresentMgr.GetPresentValue1Text( rkInfo.m_iPresentType, rkInfo.m_iPresentValue1, rkInfo.m_iPresentValue2 );
	m_sTitle2 = g_PresentMgr.GetPresentValue2Text( rkInfo.m_iPresentType, rkInfo.m_iPresentValue1, rkInfo.m_iPresentValue2 );

	if( !m_sRollingSound.IsEmpty() )
		g_SoundMgr.StopSound( m_sRollingSound, 0 );

	if( !m_sResultSound.IsEmpty() )
		g_SoundMgr.PlaySound( m_sResultSound );

	m_dwScaleStartTime = dwCurTime;
	m_eState = S_UP_SCALING;
	m_iCurEffectArray = 0;

	if( rkInfo.m_iPresentType == PRESENT_EXTRAITEM )
	{
		g_QuestMgr.QuestCompleteTerm( QC_EXTRA_ITEM_ACQUIRE, rkInfo.m_iPresentValue1, rkInfo.m_iPresentValue2 % 10000 );
	}

	if ( rkInfo.m_iPresentType == PRESENT_COSTUME || rkInfo.m_iPresentType == PRESENT_COSTUME_BOX )
		m_bIsCostume = true;
	
	m_nGradeType = g_PresentMgr.GetGradeType( rkInfo.m_iPresentType, rkInfo.m_iPresentValue1 );
}

void SuperGashaponWnd::ProcessUpScale( DWORD dwCurTime )
{
	float fScale = FLOAT1;
	m_fGradeScale = FLOAT1;
	DWORD dwGapTime = dwCurTime - m_dwScaleStartTime;
	if( dwGapTime < m_dwIconScaleTime )
	{
		float fScaleRate = (float)dwGapTime / (float)m_dwIconScaleTime;
		fScale = m_fIconScale * ( FLOAT1 - fScaleRate ) + FLOAT1 * fScaleRate;
		m_dwScaleEndTime = dwCurTime;
		m_fGradeScale = fScale;
	}
	else
	{
		m_eState = S_DOWN_SCALING;
	}

	if( m_pIcon )
		m_pIcon->SetScale( fScale );
	if( m_pSubIcon )
		m_pSubIcon->SetScale( fScale );
	if ( m_pCostumeMark )
		m_pCostumeMark->SetScale( fScale );
}

void SuperGashaponWnd::ProcessDownScale( DWORD dwCurTime )
{
	float fScale = FLOAT1;
	m_fGradeScale = FLOAT1;
	DWORD dwStartTime = m_dwScaleEndTime + 800;
	if( dwStartTime < dwCurTime )
	{
		float fScaleRate = min( FLOAT1, (float)(dwCurTime - dwStartTime) / 200.0f );
		fScale = FLOAT1 - (0.15f * sin(fScaleRate));
		m_fGradeScale = fScale;
	}

	if( m_pIcon )
		m_pIcon->SetScale( fScale );
	if( m_pSubIcon )
		m_pSubIcon->SetScale( fScale );
	if ( m_pCostumeMark )
		m_pCostumeMark->SetScale( fScale );
}

void SuperGashaponWnd::ProcessScaleEndCheck( DWORD dwCurTime )
{	
	if( m_dwScaleEndTime + 1000 < dwCurTime )
	{
		m_CurrBoughtIndex++;
		m_eState = S_UPDATE_INFO;
	}
}

void SuperGashaponWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	OnIconRender( iXPos, iYPos );
	OnExtraItemRender( iXPos, iYPos );
	OnCostumeRender( iXPos, iYPos );
	OnEffectRender( iXPos, iYPos );
	OnTtitleRender( iXPos, iYPos );
	OnBuyGoodsRender( iXPos, iYPos );

	switch( m_SuperGashponUseType )
	{
	case SUPERGASHPON_USE: 
		OnMentPresentRender( iXPos, iYPos );
		break;
	case SUPERGASHPON_BUY:
		OnMentBuyRender( iXPos, iYPos );
		break;
	}
}

void SuperGashaponWnd::OnIconRender( int iXPos, int iYPos )
{
	if( S_ROLLING < m_eState && m_pResultIconBack )
	{
		m_pResultIconBack->SetScale( FLOAT1, FLOAT1 );
		m_pResultIconBack->Render( iXPos + ICON_X_OFFSET, iYPos + ICON_Y_OFFSET, UI_RENDER_MULTIPLY );
	}
	else if( m_pIconBack )
	{
		m_pIconBack->Render( iXPos + ICON_X_OFFSET, iYPos + ICON_Y_OFFSET, UI_RENDER_MULTIPLY );
	}

	g_UIImageRenderMgr.RenderGradeIconBack( m_nGradeType, iXPos + ICON_X_OFFSET, iYPos + ICON_Y_OFFSET, m_fGradeScale );

	if( m_pIcon )
		m_pIcon->Render( iXPos + ICON_X_OFFSET, iYPos + ICON_Y_OFFSET, UI_RENDER_NORMAL, TFO_BILINEAR );
	if( m_pSubIcon )
		m_pSubIcon->Render( iXPos + ICON_X_OFFSET, iYPos + ICON_Y_OFFSET, UI_RENDER_NORMAL, TFO_BILINEAR );
}

void SuperGashaponWnd::OnExtraItemRender( int iXPos, int iYPos )
{
	const ShopBoughtInfo& rkInfo = GetBoughtInfo( m_CurrBoughtIndex );
	if( rkInfo.m_iPresentType != PRESENT_EXTRAITEM )
		return;

	OnReinforceRender( iXPos, iYPos, (rkInfo.m_iPresentValue2 % PRESENT_EXTRAITEM_DIVISION_1) / PRESENT_EXTRAITEM_DIVISION_2 );

	char szName[MAX_PATH]="";
	int iMarkX = iXPos+ICON_X_OFFSET - 45;
	int iMarkY = iYPos+ICON_Y_OFFSET - 41;

	int iTextX = iMarkX+27;
	int iTextY = iMarkY+2;

	if( rkInfo.m_iTradeType == EET_NORMAL )
	{
		if( m_pStateOrangeFrm )
			m_pStateOrangeFrm->Render( iMarkX, iMarkY, UI_RENDER_NORMAL );

		SafeSprintf( szName, sizeof( szName ), "봉인가능" );
	}
	else if( rkInfo.m_iTradeType == EET_ENABLE )
	{
		if( m_pStateGreenFrm )
			m_pStateGreenFrm->Render( iMarkX, iMarkY, UI_RENDER_NORMAL );

		SafeSprintf( szName, sizeof( szName ), "봉인" );
	}

	g_FontMgr.SetTextStyle( TS_BOLD );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
	g_FontMgr.PrintText( iTextX, iTextY, FONT_SIZE_11, szName );
}

void SuperGashaponWnd::OnReinforceRender( int iXPos, int iYPos, int iReinforce )
{
	if( S_ROLLING == m_eState )
		return;

	int iCurPos = 0;
	int iDefaultPos = iXPos+REINFORCE_X;

	int iTotalSize = 0;
	if( iReinforce < 10 )
		iTotalSize = REINFORCE_TEXT_SIZE * 2 - REINFORCE_TEXT_GAP;
	else if( iReinforce < 100 )
		iTotalSize = REINFORCE_TEXT_SIZE * 3 - REINFORCE_TEXT_GAP * 2;
	else
		iTotalSize = REINFORCE_TEXT_SIZE * 4 - REINFORCE_TEXT_GAP * 3;

	iCurPos = iDefaultPos - (iTotalSize / 2);
	if( m_pPlus )
		m_pPlus->Render( iCurPos, iYPos+REINFORCE_Y , UI_RENDER_NORMAL, TFO_BILINEAR );

	iCurPos += (REINFORCE_TEXT_SIZE - REINFORCE_TEXT_GAP);
	if( m_pNum )
		m_pNum->RenderNum( iCurPos, iYPos+REINFORCE_Y, iReinforce, -4.0f, FLOAT1, UI_RENDER_NORMAL, TFO_BILINEAR );
}

void SuperGashaponWnd::OnEffectRender( int iXPos, int iYPos )
{
	if( !m_pEffect )
		return;

	m_pEffect->SetScale( 2.0f );
	m_pEffect->Render( iXPos + ICON_X_OFFSET, iYPos + ICON_Y_OFFSET, UI_RENDER_ADD );	
}

void SuperGashaponWnd::OnTtitleRender( int iXPos, int iYPos )
{
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );

	g_FontMgr.SetTextColor( 58,58,58 );
	g_FontMgr.PrintText( iXPos + TITLE_X_OFFSET, iYPos + TITLE_Y_OFFSET, FONT_SIZE_17, m_sTitle1.c_str() );

	g_FontMgr.SetTextColor( 22, 70, 178 );
	g_FontMgr.PrintText( iXPos + TITLE_X_OFFSET, iYPos + TITLE_Y_OFFSET + TITLE_GAP, FONT_SIZE_17, m_sTitle2.c_str() );
}

void SuperGashaponWnd::OnMentBuyRender( int iXPos, int iYPos )
{
	ioWnd *pWnd = FindChildWnd( ID_BUY_ADD_BTN );
	if( pWnd && pWnd->IsShow() )
	{
		ioComplexStringPrinter kPrinter;
		kPrinter.SetTextStyle( TS_NORMAL );
		kPrinter.SetBkColor( 0, 0, 0 );
		kPrinter.SetTextColor( TCT_DEFAULT_GREEN );
		kPrinter.AddTextPiece( FONT_SIZE_17, "구매가 완료되었습니다" );
		kPrinter.PrintFullText( iXPos + BOUGHT_TEXT_X_OFFSET, iYPos + BOUGHT_TEXT_Y_OFFSET, TAT_CENTER );
		kPrinter.ClearList();

		kPrinter.SetTextStyle( TS_NORMAL );
		kPrinter.SetBkColor( 0, 0, 0 );
		kPrinter.SetTextColor( TCT_DEFAULT_RED );
		kPrinter.AddTextPiece( FONT_SIZE_12, "선물함" );
		kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
		kPrinter.AddTextPiece( FONT_SIZE_12, "에서 확인하세요" );
		kPrinter.PrintFullText( pWnd->GetDerivedPosX() + 102, pWnd->GetDerivedPosY() - 36, TAT_CENTER );
		kPrinter.ClearList();

		kPrinter.SetTextStyle( TS_NORMAL );
		kPrinter.SetBkColor( 0, 0, 0 );
		kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
		kPrinter.AddTextPiece( FONT_SIZE_12, "본상품은 청약철회 제외 상품입니다" );
		kPrinter.PrintFullText( pWnd->GetDerivedPosX() + 102, pWnd->GetDerivedPosY() - 20, TAT_CENTER );
	}
}

void SuperGashaponWnd::OnBuyGoodsRender( int iXPos, int iYPos )
{
	int iYOffset = 0;
	UINT iStart = m_dwCurVertPos;
	for( UINT i = 0;  i < BUY_GOOD_MAX; ++i, ++iStart )
	{
		if( iStart < m_BuyGoodsVec.size() )
		{
			BuyGoods Goods = m_BuyGoodsVec[iStart];

			if( m_pGoodsBackIcon )
				m_pGoodsBackIcon->Render( iXPos + GOODS_X_OFFSET, iYPos + iYOffset + GOODS_Y_OFFSET, UI_RENDER_MULTIPLY, TFO_BILINEAR );

			g_UIImageRenderMgr.RenderGradeIconBack( Goods.m_nGradeType, iXPos + GOODS_X_OFFSET, iYPos + iYOffset + GOODS_Y_OFFSET, FLOAT05 );

			if( Goods.m_pGoodsIcon )
				Goods.m_pGoodsIcon->Render( iXPos + GOODS_X_OFFSET, iYPos + iYOffset + GOODS_Y_OFFSET, UI_RENDER_NORMAL, TFO_BILINEAR );
			if( Goods.m_pSubIcon )
				Goods.m_pSubIcon->Render( iXPos + GOODS_X_OFFSET, iYPos + iYOffset + GOODS_Y_OFFSET, UI_RENDER_NORMAL, TFO_BILINEAR );
			// UI코스튬 관련 (코스튬 마크)
			if ( Goods.m_bIsCostume && m_pCostumeMarkSmall )
				m_pCostumeMarkSmall->Render( iXPos + GOODS_X_OFFSET, iYPos + iYOffset + GOODS_Y_OFFSET, UI_RENDER_NORMAL, TFO_BILINEAR );

			g_FontMgr.SetAlignType( TAT_LEFT );
			g_FontMgr.SetTextStyle( TS_NORMAL );
			g_FontMgr.SetBkColor( 0, 0, 0 );
			g_FontMgr.SetTextColor( 58,58,58 );
			g_FontMgr.PrintText( iXPos + GOODS_TEXT_X_OFFSET, iYPos + iYOffset + GOODS_TEXT_Y_OFFSET, FONT_SIZE_12, Goods.m_szTitle.c_str() );

			g_FontMgr.SetTextColor( 22, 70, 178 );
			g_FontMgr.PrintText( iXPos + GOODS_TEXT_X_OFFSET, iYPos + iYOffset + GOODS_TEXT_Y_OFFSET + GOODS_TEXT_BETWEEN, FONT_SIZE_12, Goods.m_szSubTitle.c_str() );			
		}
		else
		{
			if( m_pGoodsNoneIcon )
				m_pGoodsNoneIcon->Render( iXPos + GOODS_X_OFFSET, iYPos + iYOffset + GOODS_Y_OFFSET, UI_RENDER_MULTIPLY, TFO_BILINEAR );
		}
		iYOffset += GOODS_GAP;
	}
}

void SuperGashaponWnd::OnMentPresentRender( int iXPos, int iYPos )
{

}

void SuperGashaponWnd::OnCostumeRender( int iXPos, int iYPos )
{
	if ( !m_bIsCostume )
		return;

	// UI코스튬 관련 (코스튬 마크)
	if( m_pCostumeMark )
		m_pCostumeMark->Render( iXPos + ICON_X_OFFSET, iYPos + ICON_Y_OFFSET, UI_RENDER_NORMAL, TFO_BILINEAR );
}