
#include "StdAfx.h"

#include "../io3DEngine/ioINILoader.h"
#include "../io3DEngine/ioWnd.h"
#include "../io3DEngine/ioFontManager.h"
#include "../io3DEngine/ioUIRenderer.h"
#include "../io3DEngine/ioMath.h"
#include "../io3DEngine/ioUIImageSetManager.h"
#include "../io3DEngine/ioUIRenderFrame.h"
#include "../io3DEngine/ioUIRenderImage.h"
#include "../io3DEngine/HelpFunc.h"

#include "../io3DEngine/ioXMLDocument.h"
#include "../io3DEngine/ioXMLElement.h"
#include "../io3DEngine/iostringmanager.h"
#include "../io3DEngine/safesprintf.h"

#include "../WndID.h"
#include "../ioTournamentManager.h"
#include "../ioTournamentTeamManager.h"
#include "../ioDateHelp.h"

#include "CustomTournamentCreateWnd.h"
#include "TournamentDetailMainWnd.h"
#include "TournamentMainWnd.h"
#include "LobbyMainWnd.h"
#include "ModeMapSelectList.h"
#include "QuickLobbyMenu.h"
#include "ioPullDownList.h"

CustomTournamentListBtn::CustomTournamentListBtn()
{
	m_pEmptyBack  = NULL;
	m_pSelectBack = NULL;
	m_pBannerMark = NULL;

	m_bActive = false;
	m_bSelect = false;
	m_bDisable = false;
}

CustomTournamentListBtn::~CustomTournamentListBtn()
{
	SAFEDELETE( m_pEmptyBack );
	SAFEDELETE( m_pSelectBack );
}

void CustomTournamentListBtn::SetBannerMark( ioUIRenderImage* pBannerMark, DWORD dwIndex )
{
	m_pBannerMark = pBannerMark;

	if( pBannerMark == NULL )
		SetInActive();
	else
		SetActive();

	m_dwIndex = dwIndex;
}

void CustomTournamentListBtn::SetSelect( bool bSelect, bool bDisable )
{ 
	m_bSelect	= bSelect;
	m_bDisable	= bDisable;
}

void CustomTournamentListBtn::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "EmptyBack" )
	{
		SAFEDELETE( m_pEmptyBack );
		m_pEmptyBack = pImage;
	}
	else if( szType == "SelectBack" )
	{
		SAFEDELETE( m_pSelectBack );
		m_pSelectBack = pImage;
	}
	else
	{
		ioButton::AddRenderImage( szType, pImage );
	}
}

void CustomTournamentListBtn::OnRender()
{
	ioButton::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_bSelect && m_pSelectBack )
		m_pSelectBack->Render( iXPos, iYPos );

	if( HasWndStyle(IWS_INACTIVE) )
	{
		if( m_pEmptyBack )
			m_pEmptyBack->Render( iXPos, iYPos );
	}
	else
	{
		if( m_bDisable )
		{
			if( m_pBannerMark )
				m_pBannerMark->Render( iXPos, iYPos, UI_RENDER_GRAY_LIGHT );
		}
		else
		{
			if( m_pBannerMark )
				m_pBannerMark->Render( iXPos, iYPos );
		}
	}
}

//////////////////////////////////////////////////////////////////////////

CustomTournamentCreateWnd::CustomTournamentCreateWnd()
{
	m_pDarkLine					= NULL;
	m_pLightLine				= NULL;
	m_pManualMark				= NULL;
	m_pDevideDot				= NULL;
	m_pModeHelpTooltip			= NULL;

	m_CurTournamentUserNumber	= 0;
	m_dwEtcItemType				= 0;
}

CustomTournamentCreateWnd::~CustomTournamentCreateWnd()
{
	SAFEDELETE( m_pDarkLine );
	SAFEDELETE( m_pLightLine );
	SAFEDELETE( m_pManualMark );
	SAFEDELETE( m_pDevideDot );

	BannerSetList::iterator iter = m_vCustomBannerList.begin();
	for( ; iter != m_vCustomBannerList.end(); ++iter )
	{
		BannerSet kSet = (*iter);
		SAFEDELETE( kSet.m_pBanner );
		SAFEDELETE( kSet.m_pBannerMark );
	}
	m_vCustomBannerList.clear();
}

void CustomTournamentCreateWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "DarkLine" )
	{
		SAFEDELETE( m_pDarkLine );
		m_pDarkLine = pImage;
	}
	else if( szType == "LightLine" )
	{
		SAFEDELETE( m_pLightLine );
		m_pLightLine = pImage;
	}
	else if( szType == "ManualMark" )
	{
		SAFEDELETE( m_pManualMark );
		m_pManualMark = pImage;
	}
	else if( szType == "WndDevideStrip" )
	{
		SAFEDELETE( m_pDevideDot );
		m_pDevideDot = pImage;
	}
	else
	{
		ioMovingWnd::AddRenderImage( szType, pImage );
	}
}

void CustomTournamentCreateWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_dwTournamentHelpID = xElement.GetFloatAttribute_e( "TournamentHelpID" );
}

void CustomTournamentCreateWnd::CreateCustomData()
{
	ioINILoader_e kLoader( "config/sp2_custom_tournament.ini" );
	kLoader.SetTitle_e( "default" );
	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];

	//
	m_iDefaultBannerMarkIndex = kLoader.LoadInt_e( "banner_mark_default_index", 1 );
	m_iDefaultBannerMainIndex = kLoader.LoadInt_e( "banner_main_default_index", 1 );
	m_iDefaultBannerMarkIndex = max( m_iDefaultBannerMarkIndex,	1 );
	m_iDefaultBannerMainIndex = max( m_iDefaultBannerMainIndex, 1 );
	int iMaxBanner = kLoader.LoadInt_e( "banner_max", 0 );	

	for( int i = 1; i <= iMaxBanner; ++i )
	{
		BannerSet kData;
		kData.m_Index = i;

		sprintf_s_e( szKey, "banner_name%d", i );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kData.m_szBannerName = szBuf;

		sprintf_s_e( szKey, "mark_banner%d", i );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kData.m_pBannerMark = g_UIImageSetMgr.CreateImageByFullName( szBuf );

		sprintf_s_e( szKey, "main_banner%d", i );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kData.m_pBanner = g_UIImageSetMgr.CreateImageByFullName( szBuf );

		m_vCustomBannerList.push_back( kData );
	}

	//
	m_iDefaultTournamentScaleIndex = kLoader.LoadInt_e("tournament_scale_default_index", 0);
	int iTournamentMin = kLoader.LoadInt_e("tournament_scale_min", 1);
	int iTournamentMax = kLoader.LoadInt_e("tournament_scale_max", 100);

	if( m_iDefaultTournamentScaleIndex < iTournamentMin && 0 != m_iDefaultTournamentScaleIndex )
		m_iDefaultTournamentScaleIndex = 0;

	if( 0 < iTournamentMax )
	{
		m_vTournamentScaleList.clear();
		m_vTournamentScaleList.reserve( iTournamentMax + 1 );
	}

	//선택안함 세팅
	{
		TournamentScale kData;
		kData.m_Index = 0;
		kData.m_szMenuName = "선택안함";
		m_vTournamentScaleList.push_back( kData );
	}

	for( int i = iTournamentMin; i <= iTournamentMax; ++i )
	{
		TournamentScale kData;
		kData.m_Index = i;

		int iRound = (int)pow(2.0, (double)i);
		sprintf_s_e( szBuf, "%d강(%d회전)", iRound, i );
		kData.m_szMenuName = szBuf;
		kData.m_iTournamentRound = i;

		m_vTournamentScaleList.push_back( kData );
	}

	//
	m_iDefaultTournamentTypeIndex = kLoader.LoadInt_e( "tournament_type_default_index", 0 );
	iTournamentMax = kLoader.LoadInt_e("tournament_type_max", 0);
	if( 0 < iTournamentMax )
	{
		m_vMatchTypeList.clear();
		m_vMatchTypeList.reserve( iTournamentMax );
	}

	//선택안함 세팅
	{
		TournamentMatchType kData;
		kData.m_szMenuName ="선택안함";
		m_vMatchTypeList.push_back( kData );
	}

	for( int i = 0; i < iTournamentMax; ++i )
	{
		TournamentMatchType kData;
		sprintf_s_e( szKey, "tournament_type%d_menu", i + 1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kData.m_szMenuName = szBuf;

		sprintf_s_e( szKey, "tournament_type%d_index", i + 1 );
		kData.m_iTournamentMatchType = kLoader.LoadInt(szKey, 0 );

		sprintf_s_e( szKey, "tournament_type%d_help_index", i + 1 );
		kData.m_iHelpIndex = kLoader.LoadInt(szKey, -1 );

		sprintf_s_e( szKey, "tournament_type%d_manual_index", i + 1 );
		kData.m_iManualIndex = kLoader.LoadInt(szKey, -1 );

		sprintf_s_e( szKey, "tournament_type%d_min_user", i + 1 );
		kData.m_iMinUser = kLoader.LoadInt(szKey, 0 );

		sprintf_s_e( szKey, "tournament_type%d_max_user", i + 1 );
		kData.m_iMaxUser = kLoader.LoadInt(szKey, 0 );

		sprintf_s_e( szKey, "tournament_type%d_default_user", i + 1 );
		int iUser = kLoader.LoadInt(szKey, 0 );
		iUser = min( iUser, kData.m_iMaxUser );
		iUser = max( iUser, kData.m_iMinUser );
		kData.m_iDefaultUser = iUser;

		m_vMatchTypeList.push_back( kData );
	}

	//
	m_iDefaultTournamentProgressIndex = kLoader.LoadInt_e("tournament_progress_default_index", 0);
	iTournamentMax = kLoader.LoadInt_e("tournament_progress_max", 0);
	if( 0 < iTournamentMax )
	{
		m_vTournamentProgressTypeList.clear();
		m_vTournamentProgressTypeList.reserve( iTournamentMax );
	}

	//선택안함 세팅
	{
		TournamentProgressType kData;
		kData.m_szMenuName = "선택안함";
		kData.m_Type = 0;
		m_vTournamentProgressTypeList.push_back( kData );
	}	

	for( int i = 0; i < iTournamentMax; ++i )
	{
		TournamentProgressType kData;
		kData.m_Type = min( TournamentNode::CPT_OFFLINE, i+1 );

		sprintf_s_e( szKey, "tournament_progress%d_menu", i + 1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kData.m_szMenuName = szBuf;

		sprintf_s_e( szKey, "tournament_progress%d_sub_menu", i + 1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kData.m_szSubMenuName = szBuf;

		sprintf_s_e( szKey, "tournament_progress%d_help_index", i + 1 );
		kData.m_iHelpIndex = kLoader.LoadInt(szKey, -1 );

		sprintf_s_e( szKey, "tournament_progress%d_manual_index", i + 1 );
		kData.m_iManualIndex = kLoader.LoadInt(szKey, -1 );

		m_vTournamentProgressTypeList.push_back( kData );
	}
}

BannerSet& CustomTournamentCreateWnd::FindBannerSet( int Index )
{
	static BannerSet kNone;
	BannerSetList::iterator iter = m_vCustomBannerList.begin();
	for( ; iter != m_vCustomBannerList.end(); ++iter )
	{
		BannerSet kSet = (*iter);
		if( kSet.m_Index == Index )
			return (*iter);
	}

	return kNone;
}

TournamentScale& CustomTournamentCreateWnd::FindTournamentScale( int Index )
{
	static TournamentScale kNone;
	TournamentScaleList::iterator iter = m_vTournamentScaleList.begin();
	for( ; iter != m_vTournamentScaleList.end(); ++iter )
	{
		TournamentScale kValue = (*iter);
		if( kValue.m_Index == Index )
			return (*iter);
	}

	return kNone;
}

TournamentMatchType& CustomTournamentCreateWnd::FindTournamentType( int iMatchType )
{
	static TournamentMatchType kNone;
	TournamentMatchTypeList::iterator iter = m_vMatchTypeList.begin();
	for( ; iter != m_vMatchTypeList.end(); ++iter )
	{
		TournamentMatchType kValue = (*iter);

		if( kValue.m_iTournamentMatchType == iMatchType )
			return (*iter);
	}

	return kNone;
}

TournamentProgressType& CustomTournamentCreateWnd::FindTournamentProgressType( int iType )
{
	static TournamentProgressType kNone;
	TournamentProgressTypeList::iterator iter = m_vTournamentProgressTypeList.begin();
	for( ; iter != m_vTournamentProgressTypeList.end(); ++iter )
	{
		TournamentProgressType kValue = (*iter);

		if( kValue.m_Type == iType )
			return (*iter);
	}

	return kNone;
}

void CustomTournamentCreateWnd::CreateBannerMarkList()
{
	StringScrollListWnd *pList = dynamic_cast<StringScrollListWnd*>( FindChildWnd( ID_BANNERMARK_LIST ) );
	if( pList )
	{
		BannerSetList::iterator iter = m_vCustomBannerList.begin();
		for( ; iter != m_vCustomBannerList.end(); ++iter )
		{
			BannerSet kSet = (*iter);
			pList->AddList( ID_BANNERMARK_LIST, kSet.m_Index, kSet.m_szBannerName, NULL );
		}
	}
}

void CustomTournamentCreateWnd::CreateBannerList()
{
	StringScrollListWnd *pList = dynamic_cast<StringScrollListWnd*>( FindChildWnd( ID_BANNER_LIST ) );
	if( pList )
	{
		BannerSetList::iterator iter = m_vCustomBannerList.begin();
		for( ; iter != m_vCustomBannerList.end(); ++iter )
		{
			BannerSet kSet = (*iter);
			pList->AddList( ID_BANNER_LIST, kSet.m_Index, kSet.m_szBannerName, NULL );
		}
	}
}

void CustomTournamentCreateWnd::CreateTournamentScaleList()
{
	StringScrollListWnd *pList = dynamic_cast<StringScrollListWnd*>( FindChildWnd( ID_TOURNAMENT_SCALE_LIST ) );
	if( pList )
	{
		TournamentScaleList::iterator iter = m_vTournamentScaleList.begin();
		for( ; iter != m_vTournamentScaleList.end(); ++iter)
		{
			TournamentScale kValue = (*iter);
			pList->AddList( ID_TOURNAMENT_SCALE_LIST, kValue.m_Index, kValue.m_szMenuName.c_str(), NULL );
		}
	}
}

void CustomTournamentCreateWnd::CreateTournamentTypeList()
{
	ioPullDownList *pList = dynamic_cast<ioPullDownList*>( FindChildWnd( ID_TOURNAMENT_TYPE_LIST ) );
	if( !pList )
		return;

	TournamentMatchTypeList::iterator iter = m_vMatchTypeList.begin();
	for( int i = 0; iter != m_vMatchTypeList.end(); ++iter, ++i )
	{		
		TournamentMatchType kValue = (*iter);
		TournamentTypeSinglePullDownItem* pPullDown = new TournamentTypeSinglePullDownItem;

		if( i == 0 )
			pPullDown->SetEnableImageRender( false );

		pPullDown->m_szTitle		= kValue.m_szMenuName.c_str();
		pPullDown->m_iHelpIndex		= kValue.m_iHelpIndex;
		pPullDown->m_iManualIndex	= kValue.m_iManualIndex;
		pPullDown->m_iMatchType		= kValue.m_iTournamentMatchType;
		pPullDown->m_pMarkReg		= g_ModeSelectInfo.GetOrderMarkUIResource( 0 );
		pList->AddList( pPullDown );
	}
}

void CustomTournamentCreateWnd::CreateTournamentUserNumberList()
{	
	StringScrollListWnd *pList = dynamic_cast<StringScrollListWnd*>( FindChildWnd( ID_TOURNAMENT_USERNUMBER_LIST ) );
	if( pList )
	{
		if( m_CurTournamentType.m_iTournamentMatchType == 0 )
			return;

		for( int i = m_CurTournamentType.m_iMinUser; i <= m_CurTournamentType.m_iMaxUser; ++i )
		{
			char szMenu[MAX_PATH];
			sprintf_s_e( szMenu, "%d인", i );
			pList->AddList( ID_TOURNAMENT_USERNUMBER_LIST, i, szMenu, NULL );
		}
	}
}

void CustomTournamentCreateWnd::CreateTournamentProgressList()
{
	ioPullDownList *pList = dynamic_cast<ioPullDownList*>( FindChildWnd( ID_TOURNAMENT_PROGRESS_LIST ) );
	if( !pList )
		return;

	TournamentProgressTypeList::iterator iter = m_vTournamentProgressTypeList.begin();
	for( int i = 0; iter != m_vTournamentProgressTypeList.end(); ++iter, ++i )
	{
		TournamentProgressType kValue = (*iter);
		if( i == 0 )
		{
			SinglePullDownItem* pPullDown =  new SinglePullDownItem;
			if( pPullDown )
			{
				pPullDown->m_szTitle = kValue.m_szMenuName.c_str();
				pPullDown->m_iHeight = 21;				
				pPullDown->SetEnableImageRender( false );
				pList->AddList( pPullDown );
			}
		}
		else
		{
			TournamentTypeDoublePullDownItem* pPullDown = new TournamentTypeDoublePullDownItem;
			if( pPullDown )
			{
				pPullDown->m_szTitle		= kValue.m_szMenuName.c_str();
				pPullDown->m_szSubTitle		= kValue.m_szSubMenuName.c_str();
				pPullDown->m_iHelpIndex		= kValue.m_iHelpIndex;
				pPullDown->m_iManualIndex	= kValue.m_iManualIndex;
				pPullDown->m_pMarkReg		= g_ModeSelectInfo.GetOrderMarkUIResource( 0 );

				pList->AddList( pPullDown );
			}
		}
	}
}

void CustomTournamentCreateWnd::OpenBannerMarkList( ioWnd *pOpenWnd )
{	
	StringScrollListWnd *pList = dynamic_cast<StringScrollListWnd *>( FindChildWnd( ID_BANNERMARK_LIST ) );
	if( pList )
	{
		pList->SetList( pOpenWnd, pOpenWnd->GetXPos(), pOpenWnd->GetYPos() + 24 );
		CreateBannerMarkList();
		pList->ShowListWnd();
	}
}

void CustomTournamentCreateWnd::OpenBannerList( ioWnd *pOpenWnd )
{
	StringScrollListWnd *pList = dynamic_cast<StringScrollListWnd *>( FindChildWnd( ID_BANNER_LIST ) );
	if( pList )
	{
		pList->SetList( pOpenWnd, pOpenWnd->GetXPos(), pOpenWnd->GetYPos() + 24 );
		CreateBannerList();
		pList->ShowListWnd();
	}
}

void CustomTournamentCreateWnd::OpenTournamentScaleList( ioWnd *pOpenWnd )
{
	StringScrollListWnd *pList = dynamic_cast<StringScrollListWnd *>( FindChildWnd( ID_TOURNAMENT_SCALE_LIST ) );
	if( pList )
	{
		pList->SetList( pOpenWnd, pOpenWnd->GetXPos(), pOpenWnd->GetYPos() + 24 );
		CreateTournamentScaleList();
		pList->ShowListWnd();
	}
}

void CustomTournamentCreateWnd::OpenTournamentTypeList( ioWnd *pOpenWnd )
{

	ioPullDownList *pList = dynamic_cast<ioPullDownList *>( FindChildWnd( ID_TOURNAMENT_TYPE_LIST ) );
	if( pList )
	{
		pList->OpenList( pOpenWnd );
		CreateTournamentTypeList();
	}
}

void CustomTournamentCreateWnd::OpenTournamentUserNumberList( ioWnd *pOpenWnd )
{
	StringScrollListWnd *pList = dynamic_cast<StringScrollListWnd *>( FindChildWnd( ID_TOURNAMENT_USERNUMBER_LIST ) );
	if( pList )
	{
		pList->SetList( pOpenWnd, pOpenWnd->GetXPos() - 105, pOpenWnd->GetYPos() + 24 );
		CreateTournamentUserNumberList();
		pList->ShowListWnd();
	}
}

void CustomTournamentCreateWnd::OpenTournamentProgressList( ioWnd *pOpenWnd )
{
	ioPullDownList *pList = dynamic_cast<ioPullDownList *>( FindChildWnd( ID_TOURNAMENT_PROGRESS_LIST ) );
	if( pList )
	{
		pList->OpenList( pOpenWnd );
		CreateTournamentProgressList();
	}
}

void CustomTournamentCreateWnd::SetBannerMark( int Index )
{
	m_CurBannerMark = FindBannerSet( (int)Index );

	ioWnd* pWnd = dynamic_cast<ioWnd*>( FindChildWnd( ID_BANNERMARK_BTN ) );
	if( pWnd )
	{
		pWnd->SetTitleText( m_CurBannerMark.m_szBannerName.c_str(), "", 0.0f );

		CustomTournamentViewWnd* pViewWnd = dynamic_cast<CustomTournamentViewWnd*>( FindChildWnd( ID_VIEW_WND ) );
		if( pViewWnd )
			pViewWnd->SetSelectBannerMark( m_CurBannerMark );		
	}
}

void CustomTournamentCreateWnd::SetBanner( int Index )
{
	m_CurBanner = FindBannerSet( (int)Index );

	ioWnd* pWnd = dynamic_cast<ioWnd*>( FindChildWnd( ID_BANNER_BTN ) );
	if( pWnd )
	{
		pWnd->SetTitleText( m_CurBanner.m_szBannerName.c_str(), "", 0.0f );

		CustomTournamentViewWnd* pViewWnd = dynamic_cast<CustomTournamentViewWnd*>( FindChildWnd( ID_VIEW_WND ) );
		if( pViewWnd )
			pViewWnd->SetSelectBanner( m_CurBanner );
	}
}

void CustomTournamentCreateWnd::SetTournamentScale( int Index )
{
	m_CurTournamentScale = FindTournamentScale( (int)Index );

	ioWnd* pWnd = dynamic_cast<ioWnd*>( FindChildWnd( ID_TOURNAMENT_SCALE_BTN ) );
	if( pWnd )
	{
		pWnd->SetTitleText( m_CurTournamentScale.m_szMenuName.c_str(), "", 0.0f );		

		CustomTournamentViewWnd* pViewWnd = dynamic_cast<CustomTournamentViewWnd*>( FindChildWnd( ID_VIEW_WND ) );
		if( pViewWnd )
			pViewWnd->SetTournamentScale( m_CurTournamentScale.m_szMenuName );		
	}

	CustomTournamentScheduleWnd* pScheduleWnd = dynamic_cast<CustomTournamentScheduleWnd*>( FindChildWnd( ID_SCHEDULE_WND ) );
	if( pScheduleWnd )	
		pScheduleWnd->SetTournamentSchedule( m_CurTournamentScale.m_iTournamentRound );
}

void CustomTournamentCreateWnd::SetTournamentType( int Index )
{
	ioWnd* pWnd = dynamic_cast<ioWnd*>( FindChildWnd( ID_TOURNAMENT_TYPE_BTN ) );

	m_CurTournamentType = FindTournamentType( (int)Index );

	if( pWnd )
	{
		pWnd->SetTitleText( m_CurTournamentType.m_szMenuName.c_str(), "", 0.0f );

		CustomTournamentViewWnd* pViewWnd = dynamic_cast<CustomTournamentViewWnd*>( FindChildWnd( ID_VIEW_WND ) );
		if( pViewWnd )
			pViewWnd->SetTournamentType( m_CurTournamentType.m_szMenuName, m_CurTournamentUserNumber );
	}
}

void CustomTournamentCreateWnd::SetTournamentUserNumber( int iNumber )
{
	ioWnd* pWnd = dynamic_cast<ioWnd*>( FindChildWnd( ID_TOURNAMENT_USERNUMBER_BTN ) );

	char szMenu[MAX_PATH];
	if( iNumber == 0 )	
		m_CurTournamentUserNumber = m_CurTournamentType.m_iDefaultUser;
	else 	
		m_CurTournamentUserNumber = iNumber;

	if( m_CurTournamentUserNumber == 0 )
	{
		sprintf_s( szMenu, "?명" );
	}
	else
	{
		sprintf_s_e( szMenu, "%d인", m_CurTournamentUserNumber );
	}

	if( pWnd )
	{
		pWnd->SetTitleText( szMenu, "", 0.0f );
		CustomTournamentViewWnd* pViewWnd = dynamic_cast<CustomTournamentViewWnd*>( FindChildWnd( ID_VIEW_WND ) );
		if( pViewWnd )
			pViewWnd->SetTournamentType( m_CurTournamentType.m_szMenuName, m_CurTournamentUserNumber );
	}
}

void CustomTournamentCreateWnd::SetTournamentProgress( int Index )
{
	ioWnd* pWnd = FindChildWnd( ID_TOURNAMENT_PROGRESS_BTN );

	m_CurTournamentProgressType = FindTournamentProgressType( (int)Index );

	if( pWnd )
		pWnd->SetTitleText( m_CurTournamentProgressType.m_szMenuName.c_str(), "", 0.0f );

	CustomTournamentViewWnd* pViewWnd = dynamic_cast<CustomTournamentViewWnd*>( FindChildWnd( ID_VIEW_WND ) );
	if( pViewWnd )
		pViewWnd->SetTournamentProgress( m_CurTournamentProgressType.m_Type );

	CustomTournamentScheduleWnd* pScheduleWnd = dynamic_cast<CustomTournamentScheduleWnd*>( FindChildWnd( ID_SCHEDULE_WND ) );
	if( pScheduleWnd )	
		pScheduleWnd->SetTournamentProgress( m_CurTournamentProgressType.m_Type );
}

void CustomTournamentCreateWnd::SetTournamentSchedule()
{
	CustomTournamentScheduleWnd* pScheduleWnd = dynamic_cast<CustomTournamentScheduleWnd*>( FindChildWnd( ID_SCHEDULE_WND ) );
	if( pScheduleWnd )
	{	
		ScheduleSetVec TournamentSetVec = pScheduleWnd->GetTounamentScheduleSetVec();		
		ScheduleSet kNone;
		kNone.m_CalendarSet.SetClear();
		m_CurTournamentScheduleVec.clear();
		m_CurTournamentScheduleVec.push_back( *TournamentSetVec.rbegin() );

		if( m_CurTournamentProgressType.m_Type == TournamentNode::CPT_ONLINE )
		{
			for( int i = 0; i < TournamentHelp::TOURNAMENT_MAX - 1; ++i )
			{
				if( i < (int)TournamentSetVec.size() - 1 )
				{				
					m_CurTournamentScheduleVec.push_back( TournamentSetVec[i] );
				}
				else
				{
					m_CurTournamentScheduleVec.push_back( kNone );
				}
			}
		}
		else if(  m_CurTournamentProgressType.m_Type == TournamentNode::CPT_OFFLINE )
		{
			for( int i = 0; i < TournamentHelp::TOURNAMENT_MAX - 1; ++i )
			{
				m_CurTournamentScheduleVec.push_back( kNone );
			}
		}
	}

	CustomTournamentViewWnd* pViewWnd = dynamic_cast<CustomTournamentViewWnd*>( FindChildWnd( ID_VIEW_WND ) );
	if( pViewWnd )
	{
		pViewWnd->SetTournamentType( m_CurTournamentType.m_szMenuName, m_CurTournamentUserNumber );
	}
}

void CustomTournamentCreateWnd::SetDateTournamentView()
{
	CustomTournamentScheduleWnd* pScheduleWnd = dynamic_cast<CustomTournamentScheduleWnd*>( FindChildWnd( ID_SCHEDULE_WND ) );
	CustomTournamentViewWnd* pViewWnd = dynamic_cast<CustomTournamentViewWnd*>( FindChildWnd( ID_VIEW_WND ) );
	if( pViewWnd && pScheduleWnd )
	{
		ScheduleSetVec TournamentSetVec = pScheduleWnd->GetTounamentScheduleSetVec();	
		if( !TournamentSetVec.empty() )
		{
			if( m_CurTournamentProgressType.m_Type == TournamentNode::CPT_ONLINE )
			{
				ScheduleSet KSet = *TournamentSetVec.begin();
				pViewWnd->SetTournamentDate( KSet.m_CalendarSet );
			}
			else
			{
				ioCalendarSet kEmptySet;							
				pViewWnd->SetTournamentDate( kEmptySet );
			}
		}
	}
}

bool CustomTournamentCreateWnd::IsOnlyEngHanNumText( const char *szText )
{
	int iSize = strlen( szText );
	if( iSize < 1 )	return false;

	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( !pLocal ) 
		return false;

	for(int i = 0;i < iSize;i++)
	{
		if( !COMPARE( szText[i], 'A', 'Z'+1) && !COMPARE( szText[i], 'a', 'z'+1 ) && !COMPARE( szText[i], '0', '9'+1 ) && szText[i] != '-' )
		{
			if( i < iSize - 1 )
			{
				if( pLocal->IsCheckKorean() )
				{
					// 한글 깨진 경우
					if( (byte)szText[i] == 0xa4 && (byte)szText[i+1] >= 0xa1 && (byte)szText[i+1] <= 0xd3 )
					{
						return false;
					}
					if( (byte)szText[i] >= 0xb0 && (byte)szText[i] <= 0xc8 && (byte)szText[i+1] >= 0xa1 && (byte)szText[i+1] <= 0xfe )
					{
						i++;
						continue;
					}
				}
				else
				{
#if defined( SRC_OVERSEAS )

#if defined( MULTI_BYTE_CHECK )
					if( IsDBCSLeadByteEx( COUNTRY_CODE_PAGE, (byte)szText[i] ) )
#else
					if( false && IsDBCSLeadByteEx( COUNTRY_CODE_PAGE, (byte)szText[i] ) )
#endif

#else
					if( IsDBCSLeadByte( (byte)szText[i] ) )
#endif
					{
						i++;
						continue;
					}
				}
			}
			return false;
		}
	}
	return true;
}

void CustomTournamentCreateWnd::InitEditWnd()
{
	ioEdit* pEditWnd = (ioEdit*)FindChildWnd( ID_TOURNAMENT_EDIT );	
	if( !pEditWnd )
		return;

	pEditWnd->SetKeyFocus();
	pEditWnd->ClearString();
	g_InputBox.SetString( "" );	
	pEditWnd->KillKeyFocus();

	m_szTournamentName.Clear();
}

void CustomTournamentCreateWnd::KillKeyFoucs()
{
	ioEdit* pEditWnd = (ioEdit*)FindChildWnd( ID_TOURNAMENT_EDIT );	
	if( !pEditWnd )
		return;

	pEditWnd->KillKeyFocus();
}

void CustomTournamentCreateWnd::CheckActiveCreate()
{
	if( m_CurBannerMark.m_Index != 0 && 
		m_CurBanner.m_Index != 0 &&
		m_CurTournamentScale.m_Index != 0 &&
		m_CurTournamentType.m_iTournamentMatchType != 0 &&
		m_CurTournamentUserNumber != 0 &&
		m_CurTournamentProgressType.m_Type != 0 )
	{		
		SetChildWndStyleRemove( ID_CREATE_BTN, IWS_EXACTIVE );
	}
	else
	{		
		SetChildWndStyleAdd( ID_CREATE_BTN, IWS_EXACTIVE );
	}	
}

bool CustomTournamentCreateWnd::CheckEnableTournament()
{
	ioEdit* pEditWnd = (ioEdit*)FindChildWnd( ID_TOURNAMENT_EDIT );	
	if( !pEditWnd )
		return false;
	else 
		m_szTournamentName = pEditWnd->GetText();

	if( m_szTournamentName.IsEmpty() )
	{
		InitEditWnd();
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "대회 이름을 입력하세요" );		
		return false;
	}

	if( !g_App.IsRightID( m_szTournamentName.c_str() ) )
	{
		InitEditWnd();
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "잘못된 대회 이름입니다" );
		return false;
	}

	ioHashString szNotMakeString;
	if( g_App.IsNotMakeID( m_szTournamentName.c_str(), szNotMakeString ) )
	{
		InitEditWnd();
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "사용이 금지된 이름입니다(%s)", szNotMakeString.c_str() );		
		return false;
	}

	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal && !pLocal->IsRightNewID( m_szTournamentName.c_str() ) )
	{
		InitEditWnd();
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "사용이 금지된 이름입니다" );
		return false;
	}

	char szSlangName[MAX_PATH] = "";
	sprintf( szSlangName, "%s", g_SlangUtil.ConvertString( m_szTournamentName.c_str() ) );
	if( m_szTournamentName != szSlangName )
	{
		InitEditWnd();
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "사용이 금지된 이름입니다(1)" );		
		return false;
	}

	if( !IsOnlyEngHanNumText( m_szTournamentName.c_str() ) )
	{
		InitEditWnd();
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "사용이 금지된 이름입니다(2)" );			
		return false;
	}

	if( g_SlangUtil.IsLimitedGuildName( m_szTournamentName.c_str() ) )
	{
		InitEditWnd();
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "사용이 금지된 이름입니다(3)" );		
		return false;
	}

	if( m_CurBannerMark.m_Index == 0 )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "대회배너(대)를 선택해주세요" );
		return false;
	}
	else if( m_CurBanner.m_Index == 0 )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "대회배너(소)를 선택해주세요" );
		return false;
	}
	else if( m_CurTournamentScale.m_Index == 0 )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "대회규모를 선택해주세요" );
		return false;
	}
	else if( m_CurTournamentType.m_iTournamentMatchType == 0 )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "대회종류를 선택해주세요" );
		return false;
	}
	else if( m_CurTournamentUserNumber == 0 )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "대회종류의 인원수를 선택해주세요" );
		return false;
	}
	else if( m_CurTournamentProgressType.m_Type == 0 )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "대회성격을 선택해주세요" );
		return false;
	}
	else if( m_CurTournamentScheduleVec.empty() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "대회일정을 선택해주세요" );
		return false;
	}
	else if( !m_CurTournamentScheduleVec.empty() && m_CurTournamentProgressType.m_Type != TournamentNode::CPT_OFFLINE )
	{
		ioCalendarSet kNowDate;
		kNowDate.SetTodayServerDate();
		ScheduleSetVec::iterator iter = m_CurTournamentScheduleVec.begin();
		for( int i = 0; iter != m_CurTournamentScheduleVec.end(); ++iter, ++i )
		{
			if( m_CurTournamentScale.m_iTournamentRound + 3 <= i )
				continue;		

			ScheduleSet& rkSet = *iter;
			ioCalendarSet TourDate( rkSet.m_CalendarSet, DT_LAST_MIN);

			if( TourDate.GetDate() == 0 )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "대회 일정 선택이 완료되지 않았습니다. # 대회일정을 선택해주세요" );
				return false;
			}
			else if( TourDate < kNowDate )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "대회 일정이 현재 시각보다 # 이전 일정이 존재합니다." );
				return false;
			}
		}
	}

	return true;
}

void CustomTournamentCreateWnd::TournamentStartMessage()
{
	if( CheckEnableTournament() )
	{
		CustomTournamentScheduleWnd* pWnd = dynamic_cast<CustomTournamentScheduleWnd*>( FindChildWnd( ID_SCHEDULE_WND ) );
		if( pWnd )
		{
			ioCalendarSet rkStartDate = pWnd->GetTournamentStart();
			ioCalendarSet rkEndDate	  = pWnd->GetTournamentEnd();
			if( m_CurTournamentProgressType.m_Type == TournamentNode::CPT_OFFLINE )
				rkStartDate.SetTodayServerDate();

			ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];

			kPrinter[0].SetTextStyle( TS_NORMAL );
			kPrinter[0].SetBkColor( 0, 0, 0 );
			kPrinter[0].SetTextColor( TCT_DEFAULT_GRAY );
			kPrinter[0].AddTextPiece( FONT_SIZE_13, "%s 대회 시작하시겠습니까?", m_szTournamentName.c_str() );

			kPrinter[1].SetTextStyle( TS_NORMAL );
			kPrinter[1].SetBkColor( 0, 0, 0 );
			kPrinter[1].SetTextColor( TCT_DEFAULT_GREEN );
			kPrinter[1].AddTextPiece( FONT_SIZE_13, "[대회 시작 : %d/%d/%d(%s) %d:%d]", rkStartDate.m_iYear, rkStartDate.m_iMonth, rkStartDate.m_iDay, DateHelp::GetWeekDayString( rkStartDate.m_iDayOfWeek ), rkStartDate.m_iHour, rkStartDate.m_iMin );

			kPrinter[2].SetTextStyle( TS_NORMAL );
			kPrinter[2].SetBkColor( 0, 0, 0 );
			kPrinter[2].SetTextColor( TCT_DEFAULT_GREEN );
			kPrinter[2].AddTextPiece( FONT_SIZE_13, "[대회 종료 : %d/%d/%d(%s) %d:%d]", rkEndDate.m_iYear, rkEndDate.m_iMonth, rkEndDate.m_iDay, DateHelp::GetWeekDayString( rkEndDate.m_iDayOfWeek ), rkEndDate.m_iHour, rkEndDate.m_iMin );

			if( 30 > pWnd->GetTournamentStartRemainTime() && m_CurTournamentProgressType.m_Type == TournamentNode::CPT_ONLINE )
			{
				kPrinter[3].SetTextStyle( TS_NORMAL );
				kPrinter[3].SetBkColor( 0, 0, 0 );
				kPrinter[3].SetTextColor( TCT_DEFAULT_RED );
				kPrinter[3].AddTextPiece( FONT_SIZE_13, "모집마감까지 %d분밖에 안남았습니다.", pWnd->GetTournamentStartRemainTime() );
			}
			g_GUIMgr.SetPrevMsgListBox( NULL, MB_YESNO, this, kPrinter );
		}
	}
}

void CustomTournamentCreateWnd::SendCreateTournament()
{
	SP2Packet kPacket( CTPK_ETCITEM_USE );
	kPacket << m_dwEtcItemType;

	ScheduleSetVec::const_iterator iter = m_CurTournamentScheduleVec.begin();
	if( iter != m_CurTournamentScheduleVec.end() )
	{
		ScheduleSet kSet = *iter;
		iter++;
		DWORD dwEndDate = kSet.m_CalendarSet.GetDate();
		kPacket << dwEndDate;
	}
	else
	{
		kPacket << 0;
	}

	kPacket << m_szTournamentName;							//대회명(hashstring)
	kPacket << m_CurTournamentScale.m_iTournamentRound;		//맥스라운드(대회 토너먼트 라운드, 2^n의 n값)(short)
	kPacket << m_CurBanner.m_Index;							//배너2 : 하단의 큰배너(int)
	kPacket << m_CurBannerMark.m_Index;						//배너1 : 상단의 작은 배너(int)
	kPacket << m_CurTournamentType.m_iTournamentMatchType;	//모드타입 : BMT 인덱스 값(int)
	kPacket << m_CurTournamentUserNumber;					//팀 최대인원(byte)
	kPacket << m_CurTournamentProgressType.m_Type;			//토너먼트진행방식(byte) 0 : 선택안함, 1 온라인형, 2 수동진행

	for( int i = 0; i < TournamentHelp::TOURNAMENT_MAX; ++i, ++iter )
	{
		if( iter != m_CurTournamentScheduleVec.end() )
		{
			ScheduleSet kSet = *iter;
			DWORD dwDate = kSet.m_CalendarSet.GetDate();
			kPacket << dwDate;
		}
		else
		{			
			kPacket << 0;
		}		
	}

	TCPNetwork::SendToServer( kPacket );
	TCPNetwork::MouseBusy( true );
	HideWnd();
}

void CustomTournamentCreateWnd::UseEtcItem( DWORD dwEtcItemType )
{
	m_dwEtcItemType = dwEtcItemType;
	ShowWnd();
}

void CustomTournamentCreateWnd::iwm_create()
{
	g_GUIMgr.AddWnd( "XML/customtournamentviewwnd.xml" , this );
	g_GUIMgr.AddWnd( "XML/customtournamentschedulewnd.xml" , this );

	CreateCustomData();
}

void CustomTournamentCreateWnd::iwm_show()
{
	SetBannerMark( m_iDefaultBannerMarkIndex );
	SetBanner( m_iDefaultBannerMainIndex );
	SetTournamentScale( m_iDefaultTournamentScaleIndex );
	SetTournamentType( m_iDefaultTournamentTypeIndex );
	SetTournamentUserNumber();
	SetTournamentProgress( m_iDefaultTournamentProgressIndex );
	ShowChildWnd( ID_SCHEDULE_WND );

	CheckActiveCreate();
	InitEditWnd();

	if( m_dwEtcItemType == ioEtcItem::EIT_ETC_TOURNAMENT_PREMIUM_CREATE )
	{
		SetChildWndStyleRemove( ID_BANNERLOAD_BTN, IWS_EXACTIVE );
		SetChildWndStyleRemove( ID_BANNERMARKLOAD_BTN, IWS_EXACTIVE );
		ioButton* pButton = dynamic_cast<ioButton*>( FindChildWnd( ID_BANNERLOAD_BTN ) );
		if( pButton )
			pButton->SetTitleColor( 0xFF3A3A3A, 0xFF215AA2 );

		pButton = dynamic_cast<ioButton*>( FindChildWnd( ID_BANNERMARKLOAD_BTN ) );
		if( pButton )
			pButton->SetTitleColor( 0xFF3A3A3A, 0xFF215AA2 );
	}
	else
	{
		SetChildWndStyleAdd( ID_BANNERLOAD_BTN, IWS_EXACTIVE );
		SetChildWndStyleAdd( ID_BANNERMARKLOAD_BTN, IWS_EXACTIVE );

		ioButton* pButton = dynamic_cast<ioButton*>( FindChildWnd( ID_BANNERLOAD_BTN ) );
		if( pButton )
			pButton->SetTitleColor( 0xFFA7A7A7, 0xFF215AA2 );

		pButton = dynamic_cast<ioButton*>( FindChildWnd( ID_BANNERMARKLOAD_BTN ) );
		if( pButton )
			pButton->SetTitleColor( 0xFFA7A7A7, 0xFF215AA2 );
	}
}

void CustomTournamentCreateWnd::iwm_hide()
{
	KillKeyFoucs();

	ModeHelpTooltip *pModeHelpTooltip = dynamic_cast< ModeHelpTooltip * >( m_pModeHelpTooltip );	
	if( pModeHelpTooltip )
	{
		pModeHelpTooltip->SetModeHelpTooltip( NULL, "", -1 );
	}
}

void CustomTournamentCreateWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_CLOSE_BTN:
		{
			if( cmd == IOBN_BTNUP )
			{
				KillKeyFoucs();
				HideWnd();
			}
		}
		break;
	case ID_HELP_BTN:
		{
			if( cmd == IOBN_BTNUP )
			{
				KillKeyFoucs();
				ManualListWnd *pManualWnd = dynamic_cast<ManualListWnd*> ( g_GUIMgr.FindWnd( MANUAL_LIST_WND ) );
				if( pManualWnd )
					pManualWnd->ShowWndByCurManual( m_dwTournamentHelpID );
			}
		}
		break;
	case ID_BANNERMARK_BTN:
		{
			if( cmd == IOBN_BTNUP )
			{
				KillKeyFoucs();
				OpenBannerMarkList( pWnd );
			}
			else if( cmd == ID_BANNERMARK_LIST )
			{
				KillKeyFoucs();
				SetBannerMark( (int)param );
				CheckActiveCreate();
			}
		}
		break;
	case ID_BANNERMARKLOAD_BTN:
		{
			if( cmd == IOBN_BTNUP )
			{
				KillKeyFoucs();
			}
			else if( cmd == IOEX_BTNUP )
			{
				KillKeyFoucs();
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "배너(소) 불러오기 기능은 # 프리미엄 대회 생성권만 가능합니다. # (추후 판매 예정)" );
			}
		}
		break;	
	case ID_BANNER_BTN:
		{
			if( cmd == IOBN_BTNUP )
			{
				KillKeyFoucs();
				OpenBannerList( pWnd );
			}
			else if( cmd == ID_BANNER_LIST )
			{
				KillKeyFoucs();
				SetBanner( (int)param );
				CheckActiveCreate();
			}
		}
		break;
	case ID_BANNERLOAD_BTN:
		{
			if( cmd == IOBN_BTNUP )
			{
				KillKeyFoucs();
			}
			else if( cmd == IOEX_BTNUP )
			{
				KillKeyFoucs();
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "배너(대) 불러오기 기능은 # 프리미엄 대회 생성권만 가능합니다. # (추후 판매 예정)" );
			}
		}
		break;
	case ID_TOURNAMENT_SCALE_BTN:
		{
			if( cmd == IOBN_BTNUP )
			{
				KillKeyFoucs();
				OpenTournamentScaleList( pWnd );
			}
			else if( cmd == ID_TOURNAMENT_SCALE_LIST )
			{
				KillKeyFoucs();
				SetTournamentScale( (int)param );
				SetDateTournamentView();
				CheckActiveCreate();
			}
		}
		break;
	case ID_TOURNAMENT_TYPE_BTN:
		{
			if( cmd == IOBN_BTNUP )
			{
				KillKeyFoucs();
				OpenTournamentTypeList( pWnd );
			}
		}
		break;
	case ID_TOURNAMENT_USERNUMBER_BTN:
		{
			if( cmd == IOBN_BTNUP )
			{
				KillKeyFoucs();

				if( m_CurTournamentUserNumber <= 0 )
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, "대회종류를 먼저 선택해주세요" );
				}
				else
				{
					OpenTournamentUserNumberList( pWnd );
				}
			}
			else if( cmd == ID_TOURNAMENT_USERNUMBER_LIST )
			{
				KillKeyFoucs();
				SetTournamentUserNumber( (int)param );
				CheckActiveCreate();
			}
		}
		break;
	case ID_TOURNAMENT_PROGRESS_BTN:
		{
			if( cmd == IOBN_BTNUP )
			{
				KillKeyFoucs();
				OpenTournamentProgressList( pWnd );
			}
		}
		break;
	case ID_REGULRATION_BTN:
		{
			if( cmd == IOBN_BTNUP )
			{
				KillKeyFoucs();
				g_GUIMgr.ShowWnd( TOURNAMENT_RULE_AGREE_CONFIRM_WND );
			}
		}
		break;
	case ID_CREATE_BTN:
		{
			KillKeyFoucs();
			SetTournamentSchedule();

			if( cmd == IOEX_BTNUP )
			{
				CheckEnableTournament();
			}
			else if( cmd == IOBN_BTNUP )
			{
				ioCheckButton* pCheck = dynamic_cast<ioCheckButton*>( FindChildWnd( ID_REGULRATION_CHECK ) );
				if( pCheck && pCheck->IsChecked() )
				{
					TournamentStartMessage();
				}
				else
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, "대회 운영규칙에 동의하셔야 대회생성이 가능 합니다.# [대회 운영 규칙] 클릭하시면 내용확인이 가능합니다." );
				}
			}
		}
		break;
	case MESSAGE_BOX:
	case MESSAGE_BOX3:
		if( cmd == IOBN_BTNUP )
		{
			KillKeyFoucs();

			if( param == IDYES )
			{
				SendCreateTournament();
				HideWnd();
			}
		}
		break;
	case ID_SCHEDULE_WND:
		{
			if( param == 1 )			
				SetDateTournamentView();

			KillKeyFoucs();
		}
		break;
	}
}

void CustomTournamentCreateWnd::iwm_pulldown_event( const PullDownEvent& Event )
{
	const ioPullDownList* pCaller = Event.GetEventCaller();	
	if( !pCaller )
		return;

	//마우스 오버
	if( CLASSTYPEQUALS( OnOverPullDownEvent ) == CLASSTYPEQUALS( Event ) )
	{
		const OnOverPullDownEvent& OverEvent = dynamic_cast<const OnOverPullDownEvent&>( Event );
		if( OverEvent.IsImageMouseOver() )
		{
			//대회 모드 타입
			TournamentTypeSinglePullDownItem* pSingle = dynamic_cast<TournamentTypeSinglePullDownItem*>( pCaller->GetPullDown( OverEvent.iOverIdx ) );
			if( pSingle )
			{
				OnModeHelpToolTip( this, pSingle->m_szTitle.c_str(), pSingle->m_iHelpIndex );
				return;
			}

			//대회 진행 종류
			TournamentTypeDoublePullDownItem* pDouble = dynamic_cast<TournamentTypeDoublePullDownItem*>( pCaller->GetPullDown( OverEvent.iOverIdx ) );
			if( pDouble )
			{
				OnModeHelpToolTip( this, pDouble->m_szTitle.c_str(), pDouble->m_iHelpIndex );
				return;
			}
		}
	}
	//마우스 클릭
	else if( CLASSTYPEQUALS( ClickPullDownEvent ) == CLASSTYPEQUALS( Event ) )
	{
		const ClickPullDownEvent& ClickEvent = dynamic_cast<const ClickPullDownEvent&>( Event );
		if( ClickEvent.IsPulldownClick() )
		{
			KillKeyFoucs();

			//대회 모드 타입
			TournamentTypeSinglePullDownItem* pSingle = dynamic_cast<TournamentTypeSinglePullDownItem*>( pCaller->GetPullDown( ClickEvent.iClickIdx ) );
			if( pSingle )
			{
				SetTournamentType( (int)pSingle->m_iMatchType );
				SetTournamentUserNumber();
				CheckActiveCreate();
				return;
			}

			//대회 진행 종류
			TournamentTypeDoublePullDownItem* pDouble = dynamic_cast<TournamentTypeDoublePullDownItem*>( pCaller->GetPullDown( ClickEvent.iClickIdx ) );
			if( pDouble )
			{
				SetTournamentProgress( (int)pDouble->m_iIndex );
				SetDateTournamentView();
				CheckActiveCreate();
				return;
			}
		}
		else if( ClickEvent.IsImageClick() )
		{
			//대회 모드 타입
			TournamentTypeSinglePullDownItem* pSingle = dynamic_cast<TournamentTypeSinglePullDownItem*>( pCaller->GetPullDown( ClickEvent.iClickIdx ) );
			if( pSingle )
			{
				OnTooltipEvent( pSingle->m_iManualIndex );
				return;
			}

			//대회 진행 종류
			TournamentTypeDoublePullDownItem* pDouble = dynamic_cast<TournamentTypeDoublePullDownItem*>( pCaller->GetPullDown( ClickEvent.iClickIdx ) );
			if( pDouble )
			{
				OnTooltipEvent( pDouble->m_iManualIndex );
				return;
			}
		}
	}
	//마우스 리브
	else if( CLASSTYPEQUALS( OnLeavePullDownEvent ) == CLASSTYPEQUALS( Event ) )
	{
		const OnLeavePullDownEvent& LeaveEvent = dynamic_cast<const OnLeavePullDownEvent&>( Event );
		if( LeaveEvent.IsImageMouseLeave() )
		{
			OnModeHelpToolTip(  NULL, "", -1 );	
		}
	}
}


void CustomTournamentCreateWnd::OnTooltipEvent( int iManualIndex )
{
	ManualListWnd *pManualWnd = dynamic_cast<ManualListWnd*> ( g_GUIMgr.FindWnd( MANUAL_LIST_WND ) );
	if( pManualWnd )
		pManualWnd->ShowWndByCurManual( iManualIndex );
}

void CustomTournamentCreateWnd::OnModeHelpToolTip( ioWnd* pCallWnd, const ioHashString& szTtitle, int iHelpIdx )
{
	if( m_pModeHelpTooltip == NULL )
		m_pModeHelpTooltip = dynamic_cast<ModeHelpTooltip*>( FindChildWnd( ID_MODE_HELP_TOOLTIP ) );

	ModeHelpTooltip *pModeHelpTooltip = dynamic_cast<ModeHelpTooltip*>( m_pModeHelpTooltip );
	if( pModeHelpTooltip )
		pModeHelpTooltip->SetModeHelpTooltip( pCallWnd, szTtitle, iHelpIdx );
}

void CustomTournamentCreateWnd::OnProcess( float fTimePerSec )
{
}

void CustomTournamentCreateWnd::OnRender()
{
	ioMovingWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	OnDevideWndDotRender( iXPos, iYPos );
	OnBackLineRender( iXPos, iYPos );
	OnRegulationRender( iXPos, iYPos );
}

void CustomTournamentCreateWnd::OnBackLineRender( int iXPos, int iYPos )
{
	if( !m_pDarkLine || !m_pLightLine )
		return;

	int iOffset = 24;
	for( int i = 0; i <= MAX_BACKLINE; ++i )
	{
		if( i % 2 == 0 )
			m_pDarkLine->Render( iXPos + 442, iYPos + 59 + iOffset * i, UI_RENDER_MULTIPLY );
		else
			m_pLightLine->Render( iXPos + 442, iYPos + 59 + iOffset * i, UI_RENDER_MULTIPLY );
	}

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintTextWidthCut( iXPos + 447, iYPos + 63, FONT_SIZE_13, 103.0f, "대회이름" );

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintTextWidthCut( iXPos + 447, iYPos + 87, FONT_SIZE_13, 103.0f, "대회배너(대)" );

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintTextWidthCut( iXPos + 447, iYPos + 111, FONT_SIZE_13, 103.0f, "대회배너(소)" );

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintTextWidthCut( iXPos + 447, iYPos + 135, FONT_SIZE_13, 103.0f, "대회규모" );

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintTextWidthCut( iXPos + 447, iYPos + 159, FONT_SIZE_13, 103.0f, "대회종류" );

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintTextWidthCut( iXPos + 447, iYPos + 183, FONT_SIZE_13, 103.0f, "대회성격" );
}

void CustomTournamentCreateWnd::OnRegulationRender( int iXPos, int iYPos )
{
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintTextWidthCut( iXPos + 545, iYPos + 424, FONT_SIZE_13, 103.0f, "에 동의합니다." );
}

void CustomTournamentCreateWnd::OnDevideWndDotRender( int iXPos, int iYPos )
{
	if( !m_pDevideDot )
		return;

	for( DWORD i = 0; i < 84; ++i )
	{
		m_pDevideDot->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
		m_pDevideDot->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
		m_pDevideDot->SetSize( 2, 3 );
		m_pDevideDot->Render( iXPos, iYPos + 5 * i );
	}
}

//////////////////////////////////////////////////////////////////////////

CustomTournamentViewWnd::CustomTournamentViewWnd()
{
	m_pMainBannerFrm		= NULL;
	m_pScheduleOpenGray		= NULL;
	m_pScheduleOpenOrange	= NULL;
	m_pScheduleCloseGray	= NULL;
	m_pScheduleCloseOrange	= NULL;

	m_pHorzLine  = NULL;
	m_pVertLine  = NULL;
	m_pTextBack  = NULL;
	m_pTodayMark = NULL;

	m_pBanner = NULL;

	m_pExtraExplainBack = NULL;

	m_iUserNumber = 0;

	m_dwManualTournament = 0;
}

CustomTournamentViewWnd::~CustomTournamentViewWnd()
{
	SAFEDELETE( m_pMainBannerFrm );
	SAFEDELETE( m_pScheduleOpenGray );
	SAFEDELETE( m_pScheduleOpenOrange );
	SAFEDELETE( m_pScheduleCloseGray );
	SAFEDELETE( m_pScheduleCloseOrange );

	SAFEDELETE( m_pHorzLine );
	SAFEDELETE( m_pVertLine );
	SAFEDELETE( m_pTextBack );
	SAFEDELETE( m_pTodayMark );	

	SAFEDELETE( m_pExtraExplainBack );
}

void CustomTournamentViewWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "MainBannerFrm" )
	{
		SAFEDELETE( m_pMainBannerFrm );
		m_pMainBannerFrm = pFrame;
	}
	else if( szType == "ScheduleOpenGray" )
	{
		SAFEDELETE( m_pScheduleOpenGray );
		m_pScheduleOpenGray = pFrame;
	}
	else if( szType == "ScheduleOpenOrange" )
	{
		SAFEDELETE( m_pScheduleOpenOrange );
		m_pScheduleOpenOrange = pFrame;
	}
	else if( szType == "ScheduleCloseGray" )
	{
		SAFEDELETE( m_pScheduleCloseGray );
		m_pScheduleCloseGray = pFrame;
	}
	else if( szType == "ScheduleCloseOrange" )
	{
		SAFEDELETE( m_pScheduleCloseOrange );
		m_pScheduleCloseOrange = pFrame;
	}
	else 
	{
		ioWnd::AddRenderFrame( szType, pFrame );
	}
}

void CustomTournamentViewWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "HorzLine" )
	{
		SAFEDELETE( m_pHorzLine );
		m_pHorzLine = pImage;
	}
	else if( szType == "VertLine" )
	{
		SAFEDELETE( m_pVertLine );
		m_pVertLine = pImage;
	}
	else if( szType == "TextBack" )
	{
		SAFEDELETE( m_pTextBack );
		m_pTextBack = pImage;
	}
	else if( szType == "TodayMark" )
	{
		SAFEDELETE( m_pTodayMark );
		m_pTodayMark = pImage;
	}
	else if( szType == "ExtraExplainBack" )
	{
		SAFEDELETE( m_pExtraExplainBack );
		m_pExtraExplainBack = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void CustomTournamentViewWnd::SetSelectBanner( BannerSet& rkBanner )
{
	m_pBanner = rkBanner.m_pBanner;
}

void CustomTournamentViewWnd::SetSelectBannerMark( BannerSet& rkBanner )
{
	CustomTournamentListBtn* pWnd = dynamic_cast<CustomTournamentListBtn*>( FindChildWnd( ID_BANNER_MARK_BTN1 ) );
	if( pWnd )
	{
		pWnd->SetBannerMark( rkBanner.m_pBannerMark, rkBanner.m_Index );
		pWnd->SetSelect( true, false );
	}

	for( int ID = ID_BANNER_MARK_BTN2; ID <= ID_BANNER_MARK_BTN12; ++ID )
	{
		CustomTournamentListBtn* pWnd = dynamic_cast<CustomTournamentListBtn*>( FindChildWnd( ID ) );
		if( pWnd )
		{	
			pWnd->SetSelect( false, false );
			pWnd->SetBannerMark( NULL, 0 );
		}
	}
}

void CustomTournamentViewWnd::SetTournamentName( ioHashString& szName )
{
	m_szTournamentName = szName;
}

void CustomTournamentViewWnd::SetTournamentScale( ioHashString& szScale )
{
	m_szTournamentScale = szScale;
}

void CustomTournamentViewWnd::SetTournamentType( ioHashString& szType, int iNumber )
{
	m_szTournamentType = szType;
	m_iUserNumber = iNumber;
}

void CustomTournamentViewWnd::SetTournamentProgress( DWORD dwManualTournament )
{
	m_dwManualTournament = dwManualTournament;
}

void CustomTournamentViewWnd::SetTournamentDate( ioCalendarSet& Date )
{
	m_TournamentDate.SetClear();
	m_TournamentDate = Date;
}

void CustomTournamentViewWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;
}

void CustomTournamentViewWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	OnMainBannerRender( iXPos, iYPos );
	OnExplainRender( iXPos, iYPos );
	OnExtraExplainRender( iXPos, iYPos );
	OnScheduleRender( iXPos, iYPos );

	//
	if( m_pMainBannerFrm )
	{
		m_pMainBannerFrm->Render( iXPos + 27, iYPos + 55 );
	}
}

void CustomTournamentViewWnd::OnMainBannerRender( int iXPos, int iYPos )
{
	if( m_pBanner )
	{
		m_pBanner->Render( iXPos + 30, iYPos + 58 );
	}
}

void CustomTournamentViewWnd::OnExplainRender( int iXPos, int iYPos )
{
	if( m_pTextBack )
	{
		m_pTextBack->Render( iXPos + 30, iYPos + 329 );
	}

	if( m_pHorzLine )
	{
		m_pHorzLine->Render( iXPos + 30, iYPos + 328 );
	}

	if( m_pVertLine )
	{
		m_pVertLine->Render( iXPos + 157, iYPos + 339 );
		m_pVertLine->Render( iXPos + 279, iYPos + 339 );
	}

	//
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( 255, 255, 255 );
	g_FontMgr.PrintText( iXPos + 96, iYPos + 344, FONT_SIZE_12, "대회규모" );

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( 255, 180, 0 );
	g_FontMgr.PrintText( iXPos + 96, iYPos + 358, FONT_SIZE_12, "%s", m_szTournamentScale.c_str() );

	//
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( 255, 255, 255 );
	g_FontMgr.PrintText( iXPos + 218, iYPos + 344, FONT_SIZE_12, "대회종류" );

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( 255, 180, 0 );

	if( 0 < m_iUserNumber )
		g_FontMgr.PrintText( iXPos + 218, iYPos + 358, FONT_SIZE_12, "%s %d인", m_szTournamentType.c_str(), m_iUserNumber );
	else
		g_FontMgr.PrintText( iXPos + 218, iYPos + 358, FONT_SIZE_12, "%s", m_szTournamentType.c_str() );

	//
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( 255, 255, 255 );

	g_FontMgr.PrintText( iXPos + 340, iYPos + 344, FONT_SIZE_12, "모집마감" );

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( 255, 180, 0 );

	if( m_dwManualTournament == TournamentNode::CPT_OFFLINE )
	{
		g_FontMgr.PrintText( iXPos + 340, iYPos + 358, FONT_SIZE_12, "수동진행" );
	}
	else
	{
		if( m_TournamentDate.isEmpty() )
			g_FontMgr.PrintText( iXPos + 340, iYPos + 358, FONT_SIZE_12, "선택안함" );
		else
			g_FontMgr.PrintText( iXPos + 340, iYPos + 358, FONT_SIZE_12, "%02d/%02d %02d:%02d", m_TournamentDate.m_iMonth, m_TournamentDate.m_iDay, m_TournamentDate.m_iHour, m_TournamentDate.m_iMin );
	}
}

void CustomTournamentViewWnd::OnExtraExplainRender( int iXPos, int iYPos )
{
	if( m_pHorzLine )
	{
		m_pHorzLine->Render( iXPos + 30, iYPos + 415 );
	}

	if( m_pExtraExplainBack )
	{
		m_pExtraExplainBack->Render( iXPos + 30, iYPos + 416 );
	}

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( D3DCOLOR_XRGB( 255, 60, 0 ) );
	kPrinter.AddTextPiece( FONT_SIZE_12, "포상설정, 공지작성, 대회참가권유" );

	kPrinter.SetTextColor( D3DCOLOR_XRGB( 227, 196, 129 ) );
	kPrinter.AddTextPiece( FONT_SIZE_12, "는 대회를 만든 후 가능합니다." );
	kPrinter.PrintFullText( iXPos + 218, iYPos + 421, TAT_CENTER );
	kPrinter.ClearList();
}
void CustomTournamentViewWnd::OnScheduleRender( int iXPos, int iYPos )
{
	if( m_dwManualTournament == TournamentNode::CPT_ONLINE )
	{
		if( m_pScheduleCloseGray )
		{
			m_pScheduleCloseGray->SetSize( 99, 17 );
			m_pScheduleCloseGray->Render( iXPos + 304, iYPos + 385 );
		}

		if( m_pScheduleOpenGray )
		{
			m_pScheduleOpenGray->SetSize( 102, 17 );
			m_pScheduleOpenGray->Render( iXPos + 213, iYPos + 385 );
		}

		if( m_pScheduleOpenOrange )
		{
			m_pScheduleOpenOrange->SetSize( 190, 17 );
			m_pScheduleOpenOrange->Render( iXPos + 34, iYPos + 385 );
		}

		//
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( 255, 255, 255 );
		g_FontMgr.PrintTextWidthCut( iXPos + 128, iYPos + 387, FONT_SIZE_11, 173.0f, "%s 대회 모집", m_szTournamentName.c_str() );

		//
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( 255, 255, 255 );
		g_FontMgr.PrintTextWidthCut( iXPos + 265, iYPos + 387, FONT_SIZE_11, 80.0f, "대진발표" );

		//
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( 255, 255, 255 );
		g_FontMgr.PrintTextWidthCut( iXPos + 355, iYPos + 387, FONT_SIZE_11, 80.0f, "대회기간" );
	}	
	else if( m_dwManualTournament == TournamentNode::CPT_OFFLINE )
	{
		if( m_pScheduleCloseOrange )
		{
			m_pScheduleCloseOrange->SetSize( 369, 17 );
			m_pScheduleCloseOrange->Render( iXPos + 32, iYPos + 385 );
		}

		//
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( 255, 255, 255 );
		g_FontMgr.PrintTextWidthCut( iXPos + 218, iYPos + 387, FONT_SIZE_11, 355.0f, "%s 대회(오프라인형)", m_szTournamentName.c_str()  );
	}
	else
	{
		if( m_pScheduleCloseGray )
		{
			m_pScheduleCloseGray->SetSize( 369, 17 );
			m_pScheduleCloseGray->Render( iXPos + 32, iYPos + 385 );
		}

		//
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( 255, 255, 255 );
		g_FontMgr.PrintTextWidthCut( iXPos + 218, iYPos + 387, FONT_SIZE_11, 355.0f, "선택안함" );
	}
}

//////////////////////////////////////////////////////////////////////////

CustomTournamentScheduleBtn::CustomTournamentScheduleBtn()
{
	m_pActiveIcon   = NULL;
	m_pInActiveIcon = NULL;

	m_iDefaultXPos = 0;
	m_iDefaultYPos = 0;
	m_iTourPos = -1;
}

CustomTournamentScheduleBtn::~CustomTournamentScheduleBtn()
{
	SAFEDELETE( m_pActiveIcon );
	SAFEDELETE( m_pInActiveIcon );
}

void CustomTournamentScheduleBtn::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "ActiveIcon" )
	{
		SAFEDELETE( m_pActiveIcon );
		m_pActiveIcon = pImage;
	}
	else if( szType == "InActiveIcon" )
	{
		SAFEDELETE( m_pInActiveIcon );
		m_pInActiveIcon = pImage;
	}
	else
	{
		ioButton::AddRenderImage( szType, pImage );
	}
}

void CustomTournamentScheduleBtn::iwm_create()
{
	m_iDefaultXPos = GetXPos();
	m_iDefaultYPos = GetYPos();
}

void CustomTournamentScheduleBtn::SetOffset( int iXOffset, int iYOffset )
{	
	SetWndPos( m_iDefaultXPos + iXOffset, m_iDefaultYPos + iYOffset );
}

void CustomTournamentScheduleBtn::OnRender()
{
	ioButton::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( HasWndStyle(IWS_INACTIVE) )
	{
		if( m_pInActiveIcon )
			m_pInActiveIcon->Render( iXPos, iYPos );
	}
	else
	{
		if( m_pActiveIcon )
			m_pActiveIcon->Render( iXPos, iYPos );
	}
}

//////////////////////////////////////////////////////////////////////////

CustomTournamentScheduleWnd::CustomTournamentScheduleWnd()
{
	m_iTournamentMaxRound     = 0;
	m_iScheduleGapTime		  = 0;
	m_iCurPos			      = 0;
	m_iPeriodEnableTime       = 0;
	m_iModifyReservationParam = 0;

	m_iMatchStartHour	= 0;
	m_iMatchEndHour		= 0;
	m_iTournamentPeriod = 0;

	m_TournamentProgressType = 0;
}

CustomTournamentScheduleWnd::~CustomTournamentScheduleWnd()
{
}

void CustomTournamentScheduleWnd::iwm_create()
{
	CreateCustomData();
}

void CustomTournamentScheduleWnd::iwm_show()
{
	m_StartCalendar.SetCalendarCompareType( DT_LAST_MIN );
	m_StartCalendar.SetClear();
	m_StartCalendar.SetTodayServerDate();
}

void CustomTournamentScheduleWnd::CreateCustomData()
{
	ioINILoader_e kLoader( "config/sp2_custom_tournament.ini" );
	kLoader.SetTitle_e( "scheudle" );
	m_iTournamentPeriod = kLoader.LoadInt_e( "tounament_period", 14 );
	m_iTournamentPeriod = max( 1, m_iTournamentPeriod );

	m_iScheduleGapTime	= kLoader.LoadInt_e( "schedule_gap_time", 30 );	
	m_iMatchStartHour	= kLoader.LoadInt_e( "schedule_match_start_hour", 9 );
	m_iMatchEndHour		= kLoader.LoadInt_e( "schedule_match_end_hour", 23 );	
}

void CustomTournamentScheduleWnd::ChangeTimeText( int Number, ioHashString& szNumber )
{
	char szBuffer[MAX_PATH];
	if( 1 < GetNumberCount(Number) )
		sprintf_s_e( szBuffer, "%d", Number );
	else
		sprintf_s( szBuffer, "0%d", Number );

	szNumber = szBuffer;
}

void CustomTournamentScheduleWnd::ChangeDateText( const ioCalendarSet& rkDate, ioHashString& szDestDate )
{
	ioHashString szMonth;
	ioHashString szDay;
	ioHashString szHour;
	ioHashString szMin;
	char szBuffer[MAX_PATH];
	ChangeTimeText( rkDate.m_iMonth, szMonth );
	ChangeTimeText( rkDate.m_iDay, szDay );
	ChangeTimeText( rkDate.m_iHour, szHour );
	ChangeTimeText( rkDate.m_iMin, szMin );

	sprintf_s_e( szBuffer, "%d/%s/%s(%s) %s:%s", rkDate.m_iYear, szMonth.c_str(),	szDay.c_str(), DateHelp::GetWeekDayString( rkDate.m_iDayOfWeek ), szHour.c_str(), szMin.c_str() );
	szDestDate = szBuffer;
}

void CustomTournamentScheduleWnd::InitScroll()
{
	ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );

	if( !pScroll )
		return;

	if( m_vScheduleSet.size() <= MAX_SCHEDULE )
		pScroll->HideWnd();
	else
		pScroll->ShowWnd();

	pScroll->SetScrollPos( 0 );
	pScroll->SetScrollRange( 0, m_vScheduleSet.size() );
	m_iCurPos = 0;
}

void CustomTournamentScheduleWnd::InitButton()
{
	int iPos = m_iCurPos+1;
	for( unsigned int i = ID_SCHEDULE_BTN1; i <=ID_SCHEDULE_BTN7; ++i, ++iPos )
	{
		CustomTournamentScheduleBtn* pButton = dynamic_cast<CustomTournamentScheduleBtn*>( FindChildWnd( i ) );

		if(!pButton)
			continue;

		if( i <=  m_vScheduleSet.size() )
		{
			if( m_vScheduleSet.size() <= MAX_SCHEDULE )
				pButton->SetOffset( 20 , 0 );
			else
				pButton->SetOffset( 0 , 0 );

			pButton->SetTourPos( iPos );
			pButton->ShowWnd();
		}
		else
		{
			pButton->HideWnd();
		}
	}
}

void CustomTournamentScheduleWnd::SetTournamentProgress( BYTE ProgressType )
{
	m_TournamentProgressType = ProgressType;
	if( ProgressType == TournamentNode::CPT_ONLINE )
		SetActive();
	else
		SetInActive();
}

void CustomTournamentScheduleWnd::SetActive()
{
	for( unsigned int i = ID_SCHEDULE_BTN1; i <=ID_SCHEDULE_BTN7; ++i )
	{
		CustomTournamentScheduleBtn* pButton = dynamic_cast<CustomTournamentScheduleBtn*>( FindChildWnd( i ) );
		if(pButton)
			pButton->SetActive();
	}

	CheckSequenceInActive();
}
void CustomTournamentScheduleWnd::SetInActive()
{
	for( unsigned int i = ID_SCHEDULE_BTN1; i <=ID_SCHEDULE_BTN7; ++i )
	{
		CustomTournamentScheduleBtn* pButton = dynamic_cast<CustomTournamentScheduleBtn*>( FindChildWnd( i ) );
		if(pButton)
			pButton->SetInActive();
	}	
}

void CustomTournamentScheduleWnd::CheckSequenceInActive()
{
	for( unsigned int i = ID_SCHEDULE_BTN1; i <=ID_SCHEDULE_BTN7; ++i )
	{
		CustomTournamentScheduleBtn* pButton = dynamic_cast<CustomTournamentScheduleBtn*>( FindChildWnd( i ) );
		if( pButton )
		{
			ScheduleSet& kPrevSet = FindScheduleSet( pButton->GetTourPos() - 1 );
			if( m_TournamentProgressType == TournamentNode::CPT_ONLINE )
			{
				if( kPrevSet.m_CalendarSet.isEmpty() && pButton->GetTourPos() != TournamentNode::STATE_TEAM_APP )
				{
					pButton->SetInActive();
				}
				else
				{
					pButton->SetActive();
				}
			}
			else
			{
				pButton->SetInActive();
			}			
		}
	}	
}

void CustomTournamentScheduleWnd::SetTournamentSchedule( int iTournamentRound )
{
	m_vScheduleSet.clear();
	m_iTournamentMaxRound = iTournamentRound;

	//모집마감
	{
		ScheduleSet kSet;
		kSet.m_szMenuName = "모집마감";
		kSet.m_iPos = TournamentNode::STATE_TEAM_APP;
		kSet.m_CalendarSet.SetClear();

		kSet.m_szTitle = "모집마감 날짜와 시간을 선택하세요";
		kSet.m_szMentList.clear();
		kSet.m_szMentList.push_back( "모집마감일 까지만 참가를 받게 되니 기간을 충분히 확보하세요." );
		kSet.m_szMentList.push_back( "모든 대회 일정들은 2주 내에만 지정할 수 있습니다." );

		m_vScheduleSet.push_back( kSet );
	}

	//대진발표
	{
		ScheduleSet kSet;
		kSet.m_szMenuName = "대진발표";
		kSet.m_iPos = TournamentNode::STATE_TEAM_DELAY;
		kSet.m_CalendarSet.SetClear();

		kSet.m_szTitle = "대진발표 날짜와 시간을 선택하세요";
		kSet.m_szMentList.clear();
		kSet.m_szMentList.push_back( "모집마감일로 부터 대진발표일까지 주최자가 직접 대진 배정을" );
		kSet.m_szMentList.push_back( "할 수 있습니다.대진 배정을 놓치면 랜덤배정됩니다." );

		m_vScheduleSet.push_back( kSet );
	}

	char szMenu[MAX_PATH];
	char szTitle[MAX_PATH];

	for( int i = 0; i < m_iTournamentMaxRound; ++i )
	{
		if( i == m_iTournamentMaxRound-1 )
		{
			sprintf_s_e( szMenu, "결승");
			sprintf_s_e( szTitle, "결승 날짜와 시간을 선택하세요");
		}
		else
		{
			int iRound = Help::TournamentCurrentRoundWithTeam( (int)pow( 2.0, (double)m_iTournamentMaxRound ), i );
			sprintf_s_e( szMenu,  "%d강", iRound );
			sprintf_s_e( szTitle, "%d강 날짜와 시간을 선택하세요", iRound );
		}

		ScheduleSet kSet;
		kSet.m_szMenuName = szMenu;
		kSet.m_iPos = TournamentNode::STATE_TOURNAMENT + i;
		kSet.m_CalendarSet.SetClear();

		ioHashString szStartHour, szEndHour;
		ChangeTimeText( m_iMatchStartHour, szStartHour );
		ChangeTimeText( m_iMatchEndHour, szEndHour );
		char szBuff[MAX_PATH];
		sprintf_s_e( szBuff, "경기 시간은 %s:00 ~ %s: 00만 가능합니다.", szStartHour.c_str(), szEndHour.c_str() );

		kSet.m_szTitle = szTitle;
		kSet.m_szMentList.clear();
		kSet.m_szMentList.push_back( szBuff );
		sprintf_s( szBuff, "경기 일정은 최소 %d분 정도의 간격을 두어야 합니다.", m_iScheduleGapTime );
		kSet.m_szMentList.push_back( szBuff );
		m_vScheduleSet.push_back( kSet );
	}

	//대회종료
	{
		ScheduleSet kSet;
		kSet.m_szMenuName = "대회종료";
		kSet.m_iPos = TournamentNode::STATE_TOURNAMENT + iTournamentRound;
		kSet.m_CalendarSet.SetClear();

		kSet.m_szTitle = "대회종료 날짜와 시간을 선택하세요";
		kSet.m_szMentList.clear();
		kSet.m_szMentList.push_back( "대회 종료일에 주최하신 대회가 삭제됩니다." );
		kSet.m_szMentList.push_back( "대회 결과를 즐길 수 있는 기간을 충분히 확보하세요." );

		m_vScheduleSet.push_back( kSet );
	}

	InitScroll();
	InitButton();
}

ScheduleSet& CustomTournamentScheduleWnd::FindScheduleSet( int iPos )
{
	static ScheduleSet kNone;
	ScheduleSetVec::iterator iter = m_vScheduleSet.begin();
	for( ; iter != m_vScheduleSet.end(); ++iter )
	{
		ScheduleSet& kSet = (*iter);
		if( kSet.m_iPos == iPos )
		{
			return kSet;
		}
	}

	return kNone;
}

const ioCalendarSet& CustomTournamentScheduleWnd::GetTournamentStart()
{
	ScheduleSet& rkTournamentSet = m_vScheduleSet.front();
	return rkTournamentSet.m_CalendarSet;
}

const ioCalendarSet& CustomTournamentScheduleWnd::GetTournamentEnd()
{
	ScheduleSet& rkTournamentSet = m_vScheduleSet.back();
	return rkTournamentSet.m_CalendarSet;
}

int CustomTournamentScheduleWnd::GetTournamentStartRemainTime()
{
	ioCalendarSet StartCalendar( DT_LAST_MIN );
	StartCalendar.SetTodayServerDate();

	ScheduleSet& rkTournamentSet = FindScheduleSet( TournamentNode::STATE_TEAM_APP );
	ioCalendarSet kTournamentDate( rkTournamentSet.m_CalendarSet, DT_LAST_MIN );

	if( StartCalendar < kTournamentDate )
		return kTournamentDate - StartCalendar;
	else
		return 0;
}

void CustomTournamentScheduleWnd::SelectScheduleSetAfterClear( int iStartPos )
{
	ScheduleSetVec::iterator iter = m_vScheduleSet.begin();
	for( ; iter != m_vScheduleSet.end(); ++iter )
	{
		ScheduleSet& kSet = (*iter);
		if( iStartPos < kSet.m_iPos )
		{
			kSet.m_CalendarSet.SetClear();
		}
	}
}

void CustomTournamentScheduleWnd::SelectScheduleSetBeforeClear( int iEndPos )
{
	ScheduleSetVec::iterator iter = m_vScheduleSet.begin();
	for( ; iter != m_vScheduleSet.end(); ++iter )
	{
		ScheduleSet& kSet = (*iter);
		if( kSet.m_iPos < iEndPos )
		{
			kSet.m_CalendarSet.SetClear();
		}
	}
}

void CustomTournamentScheduleWnd::OpenTournamentCalendar( int iPos )
{
	CalendarWnd* pWnd = dynamic_cast<CalendarWnd*>( g_GUIMgr.FindWnd( CUSTOM_TOURNAMENT_CALENDER_WND ) );
	if( pWnd )
	{
		ScheduleSet& kCurSet = FindScheduleSet( iPos );
		if( kCurSet.m_CalendarSet.isEmpty() )
		{
			pWnd->InitCalendar( this, iPos, kCurSet.m_szTitle, kCurSet.m_szMentList, m_StartCalendar, m_iTournamentPeriod );
			pWnd->ShowWnd();
		}
		else
		{
			pWnd->ModifyCalendar( this, iPos, kCurSet.m_szTitle, kCurSet.m_szMentList, kCurSet.m_CalendarSet );
			pWnd->ShowWnd();
		}
	}
}

void CustomTournamentScheduleWnd::SetTournamentCalendar( ioCalendarSet& kSelectDateSet, ScheduleSet& kPrevSet, ScheduleSet& kCurSet )
{
	ioCalendarSet kStartDateSet = m_StartCalendar;
	ioCalendarSet kPrevDateSet  = kPrevSet.m_CalendarSet;

	//분단위로 비교가 가능하도록 변환
	kStartDateSet.SetCalendarCompareType( DT_LAST_MIN );
	kSelectDateSet.SetCalendarCompareType( DT_LAST_MIN );
	kPrevDateSet.SetCalendarCompareType( DT_LAST_MIN );

	//대회경기일정 일떄
	if( TournamentNode::STATE_TOURNAMENT <= kCurSet.m_iPos && kCurSet.m_iPos < TournamentNode::STATE_TOURNAMENT + m_iTournamentMaxRound )
	{
		if( kSelectDateSet.m_iHour < m_iMatchStartHour || m_iMatchEndHour <= kSelectDateSet.m_iHour )
		{
			ioHashString szStartHour, szStartMin, szEndHour, szEndMin;
			ChangeTimeText( m_iMatchStartHour, szStartHour );
			ChangeTimeText( m_iMatchEndHour, szEndHour );
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "경기 시간은 # %s:00 ~ %s:00만 가능합니다", szStartHour.c_str(), szEndHour.c_str() );
			return;
		}
	}

	//
	int iGapTime = kSelectDateSet - kPrevDateSet;
	if( kPrevDateSet.isEmpty() )
	{
		iGapTime = kSelectDateSet - kStartDateSet;
	}

	if( m_iScheduleGapTime <= iGapTime && iGapTime <= m_iTournamentPeriod * 24 * 60 )
	{
		kCurSet.m_CalendarSet = kSelectDateSet;
		g_GUIMgr.HideWnd( CUSTOM_TOURNAMENT_CALENDER_WND );

		if( GetParent() )
			GetParent()->iwm_command( this, GetID(), true );

		CheckSequenceInActive();
	}
	else if( 0 <= iGapTime && kPrevSet.m_iPos == 0 )
	{
		kCurSet.m_CalendarSet = kSelectDateSet;
		g_GUIMgr.HideWnd( CUSTOM_TOURNAMENT_CALENDER_WND );

		if( GetParent() )
			GetParent()->iwm_command( this, GetID(), true );

		CheckSequenceInActive();
	}
	else
	{
		if( kSelectDateSet < kStartDateSet )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "모든 대회일정들은 # 2주 내에만 지정할 수 있습니다.");
		}
		else if( iGapTime < m_iScheduleGapTime )
		{
			if( kCurSet.m_iPos == TournamentNode::STATE_TOURNAMENT )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, " 최초 경기 일정은 # 대진 배정일로부터 최소 %d분 이후의 시간이어야 합니다.", m_iScheduleGapTime );
			}
			else if( kCurSet.m_iPos == TournamentNode::STATE_TEAM_DELAY )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, " 대진배정일은 # 모집마감일로부터 최소 %d분 이후의 시간이어야 합니다.", m_iScheduleGapTime );
			}
			else
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "경기일정은 # 최소 %d분 이후의 시간이어야 합니다.", m_iScheduleGapTime );
			}
		}
		else if( m_iTournamentPeriod * 24 * 60 <= iGapTime )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "모든 대회일정들은 # 2주 내에만 지정할 수 있습니다.");
		}
		else
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "잘못된 대회일정입니다.");
		}
	}
}

bool CustomTournamentScheduleWnd::CheckNextSchedule( ScheduleSet& kNextSet, ioCalendarSet& kSelectCalendar, DWORD param )
{
	ioCalendarSet kSelect( kSelectCalendar, DT_LAST_MIN );
	ioCalendarSet kNext( kNextSet.m_CalendarSet, DT_LAST_MIN );

	if( kNext.isEmpty() )
		return false;

	if( kNext < kSelect )
	{
		SetResetMessage( kNextSet.m_iPos );
		m_iModifyReservationParam      = param;
		m_ModifyReservationCalendarSet = kSelectCalendar;
		return true;
	}

	int iNextGap = kNext - kSelect;
	if( !kNext.isEmpty() && iNextGap < m_iScheduleGapTime )
	{
		SetResetMessage( kNextSet.m_iPos );
		m_iModifyReservationParam      = param;
		m_ModifyReservationCalendarSet = kSelectCalendar;
		return true;
	}

	return false;
}

void CustomTournamentScheduleWnd::SetResetMessage( int iPos )
{
	if( iPos == TournamentNode::STATE_TEAM_DELAY )
	{
		g_GUIMgr.SetMsgBox( MB_YESNO, this, "대진배정과의 일정간격이 짧습니다. # 이후 일정들을 초기화 하시겠습니까?");
	}
	else if( iPos == TournamentNode::STATE_TOURNAMENT + m_iTournamentMaxRound )
	{
		g_GUIMgr.SetMsgBox( MB_YESNO, this, "대회종료와의 일정간격이 짧습니다. # 이후 일정을 초기화 하시겠습니까?");
	}
	else if( iPos >= TournamentNode::STATE_TOURNAMENT )
	{
		int iNextPos = iPos - TournamentNode::STATE_TOURNAMENT;
		int iRound = Help::TournamentCurrentRoundWithTeam( (int)pow( 2.0, (double)m_iTournamentMaxRound ), iNextPos );

		g_GUIMgr.SetMsgBox( MB_YESNO, this, "%d강과의 일정간격이 짧습니다. # 이후 일정들을 초기화 하시겠습니까?", iRound );
	}
	else
	{
		g_GUIMgr.SetMsgBox( MB_YESNO, this, "일정들을 초기화 하시겠습니까?");
	}
}

ScheduleSetVec& CustomTournamentScheduleWnd::GetTounamentScheduleSetVec()
{
	if( m_TournamentProgressType == TournamentNode::CPT_OFFLINE )
	{
		ScheduleSet& rkSchedule = *m_vScheduleSet.rbegin();
		rkSchedule.m_CalendarSet = m_StartCalendar + m_iTournamentPeriod * 24 * 60;
	}

	return m_vScheduleSet;
}

void CustomTournamentScheduleWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_SCHEDULE_BTN1:
	case ID_SCHEDULE_BTN2:
	case ID_SCHEDULE_BTN3:
	case ID_SCHEDULE_BTN4:
	case ID_SCHEDULE_BTN5:
	case ID_SCHEDULE_BTN6:
	case ID_SCHEDULE_BTN7:
		{
			if( cmd == IOBN_BTNUP )
			{
				CustomTournamentScheduleBtn* pBtn = dynamic_cast<CustomTournamentScheduleBtn*>( pWnd );
				if( pBtn )
					OpenTournamentCalendar( pBtn->GetTourPos() );

				if( GetParent() )
					GetParent()->iwm_command( this, GetID(), 0 );
			}
		}
		break;
	case CUSTOM_TOURNAMENT_CALENDER_WND:
		{
			if( cmd == IOCM_USER_DEFINED )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "모든 대회일정들은 # 2주 내에만 지정할 수 있습니다.");
			}
			else
			{
				CalendarWnd* pWnd = dynamic_cast<CalendarWnd*>( g_GUIMgr.FindWnd( CUSTOM_TOURNAMENT_CALENDER_WND ) );
				if( pWnd )
				{
					ioCalendarSet kSelectDateSet;
					pWnd->GetSelectDate( kSelectDateSet );

					if( kSelectDateSet.isEmpty() )
					{
						g_GUIMgr.SetMsgBox( MB_OK, NULL, "날짜와 시간을 선택 해주세요");
					}
					else
					{	
						ScheduleSet& kNextSet = FindScheduleSet( (int)param + 1 );
						if( CheckNextSchedule( kNextSet, kSelectDateSet, param ) )
							return;

						ScheduleSet& kCurrSet = FindScheduleSet( (int)param );
						ScheduleSet& kPrevSet = FindScheduleSet( (int)param - 1 );
						SetTournamentCalendar( kSelectDateSet, kPrevSet, kCurrSet );
					}
				}
			}
		}
		break;
	case MESSAGE_BOX: //이후 일정을 초기화 시켯을때
		if( cmd == IOBN_BTNUP )
		{
			if( param != IDYES )
				return;

			if( m_iModifyReservationParam == 0 )
				return;

			SelectScheduleSetAfterClear( m_iModifyReservationParam );
			ScheduleSet& kPrevSet = FindScheduleSet( (int)m_iModifyReservationParam - 1 );
			ScheduleSet& kCurSet  = FindScheduleSet( (int)m_iModifyReservationParam );
			SetTournamentCalendar( m_ModifyReservationCalendarSet, kPrevSet, kCurSet );
		}
		break;
	}
}

void CustomTournamentScheduleWnd::iwm_wheel( int zDelta )
{
	if( !IsShow() )
		ioWnd::iwm_wheel( zDelta );
	else
	{
		if( g_App.IsMouseBusy() ) return;

		ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
		if( pScroll == NULL ) return;
		if( !pScroll->IsShow() ) return;

		if( zDelta == WHEEL_DELTA )
		{
			pScroll->SetScrollPos( m_iCurPos - 1 );
			pScroll->SetScrollRange( 0, m_vScheduleSet.size() );
		}
		else if( zDelta == -WHEEL_DELTA )
		{
			pScroll->SetScrollPos( m_iCurPos + 1 );
			pScroll->SetScrollRange( 0, m_vScheduleSet.size() );
		}
	}
}

void CustomTournamentScheduleWnd::iwm_vscroll( DWORD dwID, int curPos )
{
	ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
	if( pScroll == NULL ) return;
	if( !pScroll->IsShow() ) return;

	m_iCurPos = curPos;
	InitButton();
	CheckSequenceInActive();
}

void CustomTournamentScheduleWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	OnScheduleRender( iXPos, iYPos );
}

void CustomTournamentScheduleWnd::OnScheduleRender( int iXPos, int iYPos )
{
	int iXOffset = 0;
	int iYOffset = 0;

	if( m_vScheduleSet.size() <= MAX_SCHEDULE )
		iXOffset = 25;

	unsigned int iStart = m_iCurPos;
	unsigned int iEnd = iStart + MAX_SCHEDULE;
	unsigned int iSize = m_vScheduleSet.size();

	for( unsigned int i = iStart; i < iEnd; ++i )
	{
		if( iSize <= i )
			break;

		ScheduleSet& kSet = m_vScheduleSet[i];

		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );

		if( m_TournamentProgressType == TournamentNode::CPT_OFFLINE )
		{
			g_FontMgr.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
			g_FontMgr.PrintTextWidthCut( iXPos + 5, iYPos + 4 + iYOffset, FONT_SIZE_13, 103.0f, kSet.m_szMenuName.c_str() );
		}
		else
		{
			g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
			g_FontMgr.PrintTextWidthCut( iXPos + 5, iYPos + 4 + iYOffset, FONT_SIZE_13, 103.0f, kSet.m_szMenuName.c_str() );
		}

		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_RIGHT );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );

		if( m_TournamentProgressType == TournamentNode::CPT_OFFLINE )
		{
			if( iSize-1 == i )
			{
				ioCalendarSet EndDate = m_StartCalendar + m_iTournamentPeriod * 24 * 60;
				ioHashString szDate;
				ChangeDateText( EndDate, szDate );
				g_FontMgr.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
				g_FontMgr.PrintTextWidthCut( iXPos + 232 + iXOffset, iYPos + 4 + iYOffset, FONT_SIZE_13, 175.0f, "%s", szDate.c_str() );
			}
			else
			{
				g_FontMgr.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
				g_FontMgr.PrintTextWidthCut( iXPos + 232 + iXOffset, iYPos + 4 + iYOffset, FONT_SIZE_13, 175.0f, "주최자 수동진행" );				
			}
		}
		else if( m_TournamentProgressType == 0 )
		{
			g_FontMgr.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
			g_FontMgr.PrintTextWidthCut( iXPos + 232 + iXOffset, iYPos + 4 + iYOffset, FONT_SIZE_13, 175.0f, "선택안함" );
		}
		else
		{
			if( kSet.m_CalendarSet.isEmpty() )
			{
				g_FontMgr.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
				g_FontMgr.PrintTextWidthCut( iXPos + 232 + iXOffset, iYPos + 4 + iYOffset, FONT_SIZE_13, 175.0f, "선택안함" );
			}
			else
			{
				ioHashString szDate;
				ChangeDateText( kSet.m_CalendarSet, szDate );
				g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
				g_FontMgr.PrintTextWidthCut( iXPos + 232 + iXOffset, iYPos + 4 + iYOffset, FONT_SIZE_13, 175.0f, "%s", szDate.c_str() );
			}
		}

		iYOffset += 24;
	}
}

//////////////////////////////////////////////////////////////////////////
CustomTournamentCreateResultWnd::CustomTournamentCreateResultWnd()
{
	m_pMarkBack = NULL;
	m_pMark     = NULL;

	m_dwTournamentIndex = 0;
}

CustomTournamentCreateResultWnd::~CustomTournamentCreateResultWnd()
{
	SAFEDELETE( m_pMarkBack );
	SAFEDELETE( m_pMark );
}

void CustomTournamentCreateResultWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "MarkBack" )
	{
		SAFEDELETE( m_pMarkBack );
		m_pMarkBack = pImage;
	}
	else if( szType == "Mark" )
	{
		SAFEDELETE( m_pMark );
		m_pMark = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void CustomTournamentCreateResultWnd::SetCustomTournamentData( DWORD dwTournamentIndex )
{
	m_dwTournamentIndex = dwTournamentIndex;
	g_TournamentMgr.CreateTournament( m_dwTournamentIndex ); //결과가 도착했을떈 토너먼트가 없음으로 생성	
	ShowWnd();
}

void CustomTournamentCreateResultWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
	case ID_CLOSE:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;
	case ID_INFO:
		if( cmd == IOBN_BTNUP )
		{
			TournamentDetailMainWnd *pInfoWnd = dynamic_cast< TournamentDetailMainWnd * >( g_GUIMgr.FindWnd( TOURNAMENT_INFO ) );
			if( pInfoWnd )
			{
				pInfoWnd->SetTournamentData( m_dwTournamentIndex );
			}

			if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
			{
				LobbyMainWnd *pLobbyMainWnd = dynamic_cast<LobbyMainWnd*>( g_GUIMgr.FindWnd( LOBBY_MAIN_WND ) );
				if( pLobbyMainWnd )			
					pLobbyMainWnd->SetTournamentLink( m_dwTournamentIndex );
			}
			else
			{
				QuickLobbyMenu* pMenu = dynamic_cast<QuickLobbyMenu*>( g_GUIMgr.FindWnd( LOBBY_QUICK_MENU ) );
				if( pMenu )
					pMenu->SetTournamentLink( m_dwTournamentIndex );
			}

			HideWnd();
		}
		break;
	}
}

void CustomTournamentCreateResultWnd::OnRender()
{
	ioWnd::OnRender();
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_pMarkBack )
		m_pMarkBack->Render( iXPos + 151, iYPos + 98, UI_RENDER_MULTIPLY, TFO_BILINEAR );

	if( m_pMark )
		m_pMark->Render( iXPos + 151, iYPos + 98, UI_RENDER_NORMAL, TFO_BILINEAR );

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintTextWidthCut( iXPos + 151, iYPos + 145,FONT_SIZE_17, 268.0f, "%s 대회생성", m_szCustomTournamentTtitle.c_str() );

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( iXPos + 151, iYPos + 169, FONT_SIZE_13, "대회팀 모집을 시작하였습니다." );

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GREEN );
	kPrinter.AddTextPiece( FONT_SIZE_13, "포상설정, 공지작성, 대회참가권유 설정");

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, "을" );
	kPrinter.PrintFullText( iXPos + 151, iYPos + 189, TAT_CENTER );

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( iXPos + 151, iYPos + 209, FONT_SIZE_13, "대회정보창에서 할 수 있습니다." );
}