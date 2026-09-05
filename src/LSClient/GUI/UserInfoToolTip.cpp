
#include "StdAfx.h"

#include "../ioNpcChar.h"
#include "../ioSoldierAwakeManager.h"
#include "../ioRSoldierInfoManager.h"
#include "../ioPowerUpManager.h"
#include "../ioUserMedalItem.h"

#include "UserInfoToolTip.h"
#include "MyInventoryInfoWnd.h"
#include "TotalResultMainWnd.h"

UserInfoToolTip::vUserInfo UserInfoToolTip::m_vUserInfo;
DWORD UserInfoToolTip::m_dwMaxUserInfo = 0;

UserInfoToolTip::UserInfoToolTip() 
{
	m_dwMaxRefreshDelayTime = 0;
	m_dwMaxLiveTime         = 0;

	for( int i = 0;i < MAX_INFO_BOX;i++)
		m_pInfoBox[i] = NULL;

	for( int i = 0;i < MAX_INFO_LODING;i++)
		m_pInfoLoading[i] = NULL;

	m_dwInfoLoadTime		= 0;
	m_iInfoLoadArray		= 0;
	m_dwRefreshDelayTime	= 0;	
	
	m_pNoneTitleFrm = NULL;
	m_pPowerUpNAwakeTitleFrm = NULL;
	m_bExtraInfo = false;

	m_dwPackSendTime = 0;
}

UserInfoToolTip::~UserInfoToolTip()
{
	m_vGrowthCharIconVec.clear();

	for( int i = 0;i < MAX_INFO_BOX;i++)
		SAFEDELETE( m_pInfoBox[i]);

	for(int i = 0;i < MAX_INFO_LODING;i++)
		SAFEDELETE( m_pInfoLoading[i] );

	SAFEDELETE( m_pNoneTitleFrm );
	SAFEDELETE( m_pPowerUpNAwakeTitleFrm );
}

void UserInfoToolTip::OnProcess( float fTimePerSec )
{
	ioLSMouse *pMouse = g_App.GetMouse();
	if( !pMouse ) return;
	
	// show
	if( !IsShow() )
	{
		ioHashString sName;
		int          iType = 0;
		pMouse->GetToolTipID( sName, iType );
		if( sName.IsEmpty() )
			return;

		ShowWnd();
		int iXPos = 0;
		int iYPos = 0;
		Help::GetNotCutWndPos( pMouse->GetMousePos().x, pMouse->GetMousePos().y, GetWidth(), GetHeight(), iXPos, iYPos );
		SetWndPos( iXPos, iYPos );
		return;
	}

	// hide
	ioHashString sName;
	int          iType = 0;
	pMouse->GetToolTipID( sName, iType );
	if( sName.IsEmpty() )
	{
		HideWnd();
		return;
	}

	// pos
	pMouse->SetToolTipID( ioHashString(""), UserInfoToolTip::TT_GLOBAL ); // 초기화
	pMouse->SetToolTipCursor();
	int iXPos = 0;
	int iYPos = 0;
	Help::GetNotCutWndPos( pMouse->GetMousePos().x, pMouse->GetMousePos().y, GetWidth(), GetHeight(), iXPos, iYPos );
	SetWndPos( iXPos, iYPos );

	// loading
	if( m_dwRefreshDelayTime != 0  )
	{
		ProcessLoading();
	}
	m_bExtraInfo = false;
	// setinfo
	UserInfo kInfo;
	if( iType == TT_GLOBAL )
	{
		if( GetUserInfoMy( sName, iType, kInfo ) ) // UI 나를 선택
			SetUserInfoToolInfo( kInfo );
		else if( GetUserInfoRoom( sName, iType, kInfo ) ) // UI에서 룸에 있는 유저 선택
			SetUserInfoToolInfo( kInfo );
		else if( GetUserInfoNetwork( sName, kInfo ) )
			SetUserInfoToolInfo( kInfo );
	}
	else
	{
		if( GetUserInfoMy( sName, iType, kInfo ) )
			SetUserInfoToolInfo( kInfo );
		else if( GetUserInfoRoom( sName, iType, kInfo ) )
			SetUserInfoToolInfo( kInfo );
	}
}

void UserInfoToolTip::OnRender()
{
	ioWnd::OnRender();
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	int iExtra_Add_Y = 0;
	if( !m_bExtraInfo )
	{
		if( m_pNoneTitleFrm)
			m_pNoneTitleFrm->Render( iXPos, iYPos );
	}
	else
	{
		if( m_pPowerUpNAwakeTitleFrm)
			m_pPowerUpNAwakeTitleFrm->Render( iXPos, iYPos );
		iExtra_Add_Y = EXTRA_ADD_HEIGHT;

		for (int i = 0; i < MAX_GROWTH ; i++)
		{
			SlotIconBtn *pBtn = dynamic_cast<SlotIconBtn*> ( FindChildWnd( ID_GROWTH_1_BTN + i ) );
			if( pBtn && pBtn->IsShow() )
				pBtn->SetPosY( pBtn->GetPosY() + iExtra_Add_Y );
		}

		for (int i = 0; i < MAX_MEDAL ; i++)
		{
			SlotIconBtn *pBtn = dynamic_cast<SlotIconBtn*> ( FindChildWnd( ID_MADAL_1_BTN + i ) );
			if( pBtn && pBtn->IsShow() )
				pBtn->SetPosY( pBtn->GetPosY() + iExtra_Add_Y );
		}

		for ( int i=0; i<MAX_COSTUME; i++ )
		{
			SlotIconBtn *pBtn = dynamic_cast<SlotIconBtn*> ( FindChildWnd( ID_COSTUME_1_BTN + i ) );
			if( pBtn && pBtn->IsShow() )
				pBtn->SetPosY( pBtn->GetPosY() + iExtra_Add_Y );
		}

		for ( int i=0; i<MAX_ACC_INVENTORY; i++ )
		{
			SlotIconBtn *pBtn = dynamic_cast<SlotIconBtn*> ( FindChildWnd( ID_ACCESSORY_1_BTN + i ) );
			if( pBtn && pBtn->IsShow() )
				pBtn->SetPosY( pBtn->GetPosY() + iExtra_Add_Y );
		}
	}

	enum 
	{
		NAME_XOFFSET	= 87, 
		NAME_YOFFSET	= 10,

		CLASS_XOFFSET	= 87,
		CLASS_YOFFSET	= 26, 

		EXTRA_XOFFSET	= 87,
		EXTRA_YOFFSET	= 42, 

		GROWTH_XOFFSET	= 87,
		GROWTH_YOFFSET	= 52,

		COSTUME_XOFFSET	= 88,
		COSTUME_YOFFSET	= 153,

		ACCESSORY_XOFFSET	= 88,
		ACCESSORY_YOFFSET	= 224,

		//액세서리 관련
		MEDAL_XOFFSET	= 87,
		MEDAL_YOFFSET	= 297,
	};

	// title
	g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 12, 66, 111 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
	if( !m_sName.IsEmpty() )
		g_FontMgr.PrintTextWidthCut( iXPos + NAME_XOFFSET, iYPos + NAME_YOFFSET, FONT_SIZE_13, USERINFO_TOOLTIP_WIDTHCUT, m_sName.c_str() );

	if( !m_sClassInfo.IsEmpty() )
	{		
		g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
		g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_3X );
		g_FontMgr.PrintText( iXPos + CLASS_XOFFSET, iYPos + CLASS_YOFFSET, FONT_SIZE_11, m_sClassInfo.c_str() );
	}

	if( !m_sExtraInfo.IsEmpty() )
	{		
		g_FontMgr.SetTextColor( 0xFFFF7F7F );
		g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_3X );
		g_FontMgr.PrintText( iXPos + EXTRA_XOFFSET, iYPos + EXTRA_YOFFSET, FONT_SIZE_11, m_sExtraInfo.c_str() );
	}

	if( m_dwRefreshDelayTime != 0 )
	{
		// loading
		if( COMPARE( m_iInfoLoadArray, 0, MAX_INFO_LODING ) )
		{
			g_FontMgr.SetTextStyle( TS_NORMAL );
			g_FontMgr.SetAlignType( TAT_CENTER );
			g_FontMgr.SetBkColor( 0, 0, 0 );
			g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
			g_FontMgr.PrintText( iXPos + 87, iYPos + 96 + iExtra_Add_Y, FONT_SIZE_11, STR(3) );

			if( m_pInfoLoading[m_iInfoLoadArray] )
				m_pInfoLoading[m_iInfoLoadArray]->Render( iXPos + 63, iYPos + 113 );
		}
	}
	else
	{
		if( !m_sName.IsEmpty() )
		{
			// 육성, 메달
			g_FontMgr.SetTextStyle( TS_NORMAL );
			g_FontMgr.SetBkColor( 0, 0, 0 );
			g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
			g_FontMgr.PrintText( iXPos + GROWTH_XOFFSET, iYPos + GROWTH_YOFFSET + iExtra_Add_Y, FONT_SIZE_11, STR(1) );
			
			// UI코스튬 관련 (코스튬 UI 설정)
			g_FontMgr.PrintText( iXPos + COSTUME_XOFFSET, iYPos + COSTUME_YOFFSET + iExtra_Add_Y, FONT_SIZE_11, "장착코스튬" );
			g_FontMgr.PrintText( iXPos + ACCESSORY_XOFFSET, iYPos + ACCESSORY_YOFFSET + iExtra_Add_Y, FONT_SIZE_11, "장착액세서리" );
			g_FontMgr.PrintText( iXPos + MEDAL_XOFFSET, iYPos + MEDAL_YOFFSET + iExtra_Add_Y, FONT_SIZE_11, STR(2) );

			for( int i = 0;i < MAX_INFO_BOX;i++)
			{
				if( m_pInfoBox[i] )
					m_pInfoBox[i]->Render( iXPos, iYPos + iExtra_Add_Y );
			}
		}
	}
}

void UserInfoToolTip::OnRenderAfterChild()
{
	if ( m_bExtraInfo )
	{
		for (int i = 0; i < MAX_GROWTH ; i++)
		{
			SlotIconBtn *pBtn = dynamic_cast<SlotIconBtn*> ( FindChildWnd( ID_GROWTH_1_BTN + i ) );
			if( !pBtn )
				continue;

			if ( pBtn->IsShow() )
				pBtn->SetPosY( pBtn->GetPosY() - EXTRA_ADD_HEIGHT );
		}

		for (int i = 0; i < MAX_MEDAL ; i++)
		{
			SlotIconBtn *pBtn = dynamic_cast<SlotIconBtn*> ( FindChildWnd( ID_MADAL_1_BTN + i ) );
			if( !pBtn )
				continue;

			if ( pBtn->IsShow() )
				pBtn->SetPosY( pBtn->GetPosY() - EXTRA_ADD_HEIGHT );
		}

		for ( int i=0; i<MAX_COSTUME; i++ )
		{
			SlotIconBtn *pBtn = dynamic_cast<SlotIconBtn*> ( FindChildWnd( ID_COSTUME_1_BTN + i ) );
			if( !pBtn )
				continue;

			if ( pBtn->IsShow() )
				pBtn->SetPosY( pBtn->GetPosY() - EXTRA_ADD_HEIGHT );
		}

		for ( int i=0; i<MAX_ACC_INVENTORY; i++ )
		{
			SlotIconBtn *pBtn = dynamic_cast<SlotIconBtn*> ( FindChildWnd( ID_ACCESSORY_1_BTN + i ) );
			if( !pBtn )
				continue;

			if ( pBtn->IsShow() )
				pBtn->SetPosY( pBtn->GetPosY() - EXTRA_ADD_HEIGHT );
		}
	}
}

void UserInfoToolTip::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "InfoBox1" )
	{
		SAFEDELETE( m_pInfoBox[0] );
		m_pInfoBox[0] = pImage;
	}
	else if( szType == "InfoBox2" )
	{
		SAFEDELETE( m_pInfoBox[1] );
		m_pInfoBox[1] = pImage;
	}
	else if( szType == "InfoBox3" )
	{
		SAFEDELETE( m_pInfoBox[2] );
		m_pInfoBox[2] = pImage;
	}
	else if( szType == "InfoBox4" )
	{
		SAFEDELETE( m_pInfoBox[3] );
		m_pInfoBox[3] = pImage;
	}
	else if( szType == "InfoBox5" )
	{
		SAFEDELETE( m_pInfoBox[4] );
		m_pInfoBox[4] = pImage;
	}
	else if( szType == "InfoBox6" )
	{
		SAFEDELETE( m_pInfoBox[5] );
		m_pInfoBox[5] = pImage;
	}
	else if( szType == "InfoBox7" )
	{
		SAFEDELETE( m_pInfoBox[6] );
		m_pInfoBox[6] = pImage;
	}
	else if( szType == "InfoBox8" )
	{
		SAFEDELETE( m_pInfoBox[7] );
		m_pInfoBox[7] = pImage;
	}
	else if( szType == "InfoLoading1" )
	{
		SAFEDELETE( m_pInfoLoading[0] );
		m_pInfoLoading[0] = pImage;
	}
	else if( szType == "InfoLoading2" )
	{
		SAFEDELETE( m_pInfoLoading[1] );
		m_pInfoLoading[1] = pImage;
	}
	else if( szType == "InfoLoading3" )
	{
		SAFEDELETE( m_pInfoLoading[2] );
		m_pInfoLoading[2] = pImage;
	}
	else if( szType == "InfoLoading4" )
	{
		SAFEDELETE( m_pInfoLoading[3] );
		m_pInfoLoading[3] = pImage;
	}
	else if( szType == "InfoLoading5" )
	{
		SAFEDELETE( m_pInfoLoading[4] );
		m_pInfoLoading[4] = pImage;
	}
	else if( szType == "InfoLoading6" )
	{
		SAFEDELETE( m_pInfoLoading[5] );
		m_pInfoLoading[5] = pImage;
	}
	else if( szType == "InfoLoading7" )
	{
		SAFEDELETE( m_pInfoLoading[6] );
		m_pInfoLoading[6] = pImage;
	}
	else if( szType == "InfoLoading8" )
	{
		SAFEDELETE( m_pInfoLoading[7] );
		m_pInfoLoading[7] = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}

void UserInfoToolTip::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "NoneTitle" )
	{
		SAFEDELETE( m_pNoneTitleFrm );
		m_pNoneTitleFrm = pFrame;
	}
	else if( szType == "PowerUpNAwakeTitle" )
	{
		SAFEDELETE( m_pPowerUpNAwakeTitleFrm );
		m_pPowerUpNAwakeTitleFrm = pFrame;
	}
	else
		ioWnd::AddRenderFrame( szType, pFrame );
}

void UserInfoToolTip::SetUserInfoToolInfo( UserInfo &rkInfo )
{
	// title
	m_sName = rkInfo.m_sName;

	char szClassInfo[MAX_PATH] = "";

	bool bAwake = false;		//각성체크
	bool bPowerUp = false;	    //진화체크
	if ( rkInfo.m_iAwakeType != AWAKE_NONE )
		bAwake = true;

	if ( rkInfo.m_iPowerUpCharGrade != PUGT_NONE )
		bPowerUp = true;

	int iExtra_Add_Y = 0;
	
	if ( bAwake || bPowerUp )
	{
		m_bExtraInfo = true;
		iExtra_Add_Y = EXTRA_ADD_HEIGHT;
	}

	if( rkInfo.m_iClassType > 0 )
	{
		memset( szClassInfo, 0,  sizeof( szClassInfo ) );
		if( m_pPlayStage->GetModeType() == MT_ARENA )
			StringCbPrintf( szClassInfo, sizeof( szClassInfo ), "LV%d %s", 0, g_MyInfo.GetClassName( rkInfo.m_iClassType ) );
		else
			StringCbPrintf( szClassInfo, sizeof( szClassInfo ), "LV%d %s", rkInfo.m_iClassLevel, g_MyInfo.GetClassName( rkInfo.m_iClassType ) );
		m_sClassInfo = szClassInfo;

		memset( szClassInfo, 0,  sizeof( szClassInfo ) );
		if ( bAwake && bPowerUp )
		{
			std::string szAwakeName = g_AwakeMgr.GetAwakeName( (AwakeType)rkInfo.m_iAwakeType );
			ioHashString szPowerUpName;
			szPowerUpName  = g_PowerUpManager.GetCharGradeTitle( rkInfo.m_iClassType, rkInfo.m_iPowerUpCharGrade );
			StringCbPrintf( szClassInfo, sizeof( szClassInfo ), STR(1), szPowerUpName.c_str(), szAwakeName.c_str() );
		}
		else if ( bAwake || bPowerUp )
		{
			
			if ( bAwake )
			{
				std::string szAwakeName = g_AwakeMgr.GetAwakeName( (AwakeType)rkInfo.m_iAwakeType );
				StringCbPrintf( szClassInfo, sizeof( szClassInfo ), "(%s)", szAwakeName.c_str() );
			}
			else if( bPowerUp )
			{
				ioHashString szPowerUpName;
				szPowerUpName = g_PowerUpManager.GetCharGradeTitle( rkInfo.m_iClassType, rkInfo.m_iPowerUpCharGrade );
				StringCbPrintf( szClassInfo, sizeof( szClassInfo ), STR(2), szPowerUpName.c_str() );
			}
		}
		m_sExtraInfo = szClassInfo;
	}

	// growth
	int iGrowthLevelSize    = rkInfo.m_vGrowthLevelVec.size();
	int iGrowthCharIconSize = m_vGrowthCharIconVec.size();
	int iItemCodeSize       = rkInfo.m_vItemCodeVec.size();

	for (int i = 0; i < MAX_GROWTH ; i++)
	{
		SlotIconBtn *pBtn = dynamic_cast<SlotIconBtn*> ( FindChildWnd( ID_GROWTH_1_BTN + i ) );
		if( !pBtn )
			continue;

		ioUIRenderImage *pImg = NULL;
		int iReinforce = 0;

		if( i < MAX_ITEM_GROWTH )
		{
			DWORD dwItemCode = 0;
			if( COMPARE( i, 0, iItemCodeSize ) )
				dwItemCode = rkInfo.m_vItemCodeVec[i];

			if( rkInfo.m_iClassType <= 0 )
			{
				pBtn->ShowWnd();
				pBtn->SetIcon( NULL, USERINFO_TOOLTIP_NONE_ITEM_SCALE, USERINFO_TOOLTIP_NONE_ITEM_SCALE, USERINFO_TOOLTIP_NONE_ITEM_SCALE, 0 );
				pBtn->SetActive();
				continue;
			}

			if( dwItemCode <= 0 )
			{	
				pBtn->ShowWnd();
				pBtn->SetIcon( NULL, USERINFO_TOOLTIP_NONE_ITEM_SCALE, USERINFO_TOOLTIP_NONE_ITEM_SCALE, USERINFO_TOOLTIP_NONE_ITEM_SCALE, 0 );
				pBtn->SetActive();
				continue;
			}

			const ioItem *pItem = g_ItemMaker.GetItemConst( dwItemCode, __FUNCTION__ );
			if( !pItem )
			{
				pBtn->ShowWnd();
				pBtn->SetIcon( NULL, USERINFO_TOOLTIP_NONE_ITEM_SCALE, USERINFO_TOOLTIP_NONE_ITEM_SCALE, USERINFO_TOOLTIP_NONE_ITEM_SCALE, 0 );
				pBtn->SetActive();
				continue;
			}

			ioHashString szSkillName = pItem->GetSkillName();
			const ioSkill *pSkill = g_SkillMaker.GetSkillConst( szSkillName );
			if( !pSkill )
			{
				pBtn->ShowWnd();
				pBtn->SetIcon( NULL, USERINFO_TOOLTIP_NONE_ITEM_SCALE, USERINFO_TOOLTIP_NONE_ITEM_SCALE, USERINFO_TOOLTIP_NONE_ITEM_SCALE, 0 );
				pBtn->SetActive();
				continue;
			}

			pImg = g_UIImageSetMgr.CreateImageByFullName(  pSkill->GetIconName() );

			int iArray = i+MAX_ITEM_GROWTH;
			if( COMPARE( iArray, 0, iGrowthLevelSize ) )
				iReinforce = rkInfo.m_vGrowthLevelVec[iArray];
		}
		else
		{
			int iArray = i-MAX_ITEM_GROWTH;
			if( COMPARE( iArray, 0, iGrowthCharIconSize ) )
				pImg = g_UIImageSetMgr.CreateImageByFullName( m_vGrowthCharIconVec[iArray] );

			if( COMPARE( iArray, 0, iGrowthLevelSize ) )
				iReinforce = rkInfo.m_vGrowthLevelVec[iArray];
		}

		if( m_pPlayStage->GetModeType() == MT_ARENA )
			iReinforce = 0;

		pBtn->ShowWnd();
		pBtn->SetIcon( pImg, USERINFO_TOOLTIP_GROWTH_SCALE, USERINFO_TOOLTIP_GROWTH_SCALE, USERINFO_TOOLTIP_GROWTH_SCALE, 0, iReinforce );
		if( iReinforce <= 0 )
			pBtn->SetInActive();
		else
			pBtn->SetActive();
	}

	// UI코스튬 관련(캐릭터 툴팁 - 1부터 시작)
	for ( int i=1; i<MAX_COSTUME; i++ )
	{
		int nCostumeCode = 0;
		if( COMPARE( i, 0, MAX_INVENTORY ) && (int)rkInfo.m_vCostumeCodeVec.size() >= MAX_INVENTORY )
			nCostumeCode = rkInfo.m_vCostumeCodeVec[i];

		SlotIconBtn *pBtn = dynamic_cast<SlotIconBtn*> ( FindChildWnd( ID_COSTUME_1_BTN + i ) );
		if( !pBtn )
			continue;

		ioUIRenderImage *pImg = NULL;
		bool bIsCostume = false;
		ioHashString szIcon = g_CostumeInfoMgr.GetCostumeIcon( nCostumeCode );
		if( !szIcon.IsEmpty() )
		{
			pImg = g_UIImageSetMgr.CreateImageByFullName( szIcon );
			bIsCostume = true;
		}

		pBtn->ShowWnd();
		pBtn->SetIcon( pImg, USERINFO_TOOLTIP_COSTUME_SCALE, USERINFO_TOOLTIP_COSTUME_EMPTY_SCALE, USERINFO_TOOLTIP_COSTUME_DISABLE_SCALE, 0, 0, 0, 0, bIsCostume );
		pBtn->SetGradeType( g_CostumeInfoMgr.GetGradeType( nCostumeCode ) );
		pBtn->SetActive();     
	}

	// Accessory
	for ( int i=0; i<MAX_ACC_INVENTORY; i++ )
	{
		int iAccessoryCode = 0;
		if( (int)rkInfo.m_vAccessoryCodeVec.size() >= MAX_ACC_INVENTORY )
			iAccessoryCode = rkInfo.m_vAccessoryCodeVec[i];
		SlotIconBtn *pBtn = dynamic_cast<SlotIconBtn*> ( FindChildWnd( ID_ACCESSORY_1_BTN + i ) );
		if( !pBtn )
			continue;
		ioHashString szIcon = g_ItemMaker.GetItemLargeIcon( iAccessoryCode, 0 );
		ioUIRenderImage *pImg = NULL;
		if( !szIcon.IsEmpty() )
			pImg = g_UIImageSetMgr.CreateImageByFullName( szIcon );
		
		pBtn->ShowWnd();
		pBtn->SetIcon( pImg, USERINFO_TOOLTIP_ACCESSORY_SCALE, USERINFO_TOOLTIP_ACCESSORY_EMPTY_SCALE, USERINFO_TOOLTIP_ACCESSORY_DISABLE_SCALE, 0 );
		pBtn->SetActive();
	}

	// medal
	MedalSyncInfoVec vMedalItemTypeExceptNoneVec;
	for(MedalSyncInfoVec::iterator iter = rkInfo.m_vMedalItemTypeVec.begin(); iter != rkInfo.m_vMedalItemTypeVec.end(); ++iter)
	{
		MedalSyncInfo &riType = (*iter);
		if( riType.m_iItemType == 0 )
			continue;

		vMedalItemTypeExceptNoneVec.push_back( riType );
	}

	int iMedalSize = vMedalItemTypeExceptNoneVec.size();
	int iSlotNum   = g_MedalItemMgr.GetSlotNum( rkInfo.m_iClassLevel );
	int iHalfMedalSize = MAX_MEDAL/2; 
	bool bMedal3 = false;
	if( iMedalSize <= iHalfMedalSize )
		bMedal3 = true;

	ioExpandMedalSlotManager::ExpandMedalSlotVec vExpandMedalSlotVec;
	int iExpandSlotSize = rkInfo.m_vExMedalSlotVec.size();
	for( int i=0; i<iExpandSlotSize; ++i )
	{
		if( rkInfo.m_vExMedalSlotVec[i] >= iHalfMedalSize )
			bMedal3 = false;
	}

	int iCurMedal = 0;
	for (int i = 0; i < MAX_MEDAL ; i++)
	{
		SlotIconBtn *pBtn = dynamic_cast<SlotIconBtn*> ( FindChildWnd( ID_MADAL_1_BTN + i ) );
		if( !pBtn )
			continue;

		if( i < iMedalSize && i < iSlotNum )
		{
			const ioMedalItemInfoManager::ItemInfo *pItemInfo = g_MedalItemMgr.GetItemInfo( vMedalItemTypeExceptNoneVec[iCurMedal].m_iItemType );
			iCurMedal++;
			if( !pItemInfo )
			{
				pBtn->ShowWnd();
				pBtn->SetIcon( NULL, USERINFO_TOOLTIP_MEDAL_SCALE, USERINFO_TOOLTIP_MEDAL_EMPTY_SCALE, USERINFO_TOOLTIP_MEDAL_DISABLE_SCALE, 0 );
				pBtn->SetActive();     // empty slot
				continue;
			}

			ioUIRenderImage *pImg = g_UIImageSetMgr.CreateImageByFullName( pItemInfo->m_sIcon );
			ioUIRenderImage *pSubImg = g_UIImageSetMgr.CreateImageByFullName( pItemInfo->m_sSubIcon );

			pBtn->ShowWnd();
			pBtn->SetIcon( pImg, USERINFO_TOOLTIP_MEDAL_SCALE, USERINFO_TOOLTIP_MEDAL_EMPTY_SCALE, USERINFO_TOOLTIP_MEDAL_DISABLE_SCALE, 0 );
			pBtn->SetSubIcon( pSubImg );
			pBtn->SetActive();     
		}
		else if( iExpandSlotSize <= 0 )
		{
			if( bMedal3 && i >= iHalfMedalSize )
			{
				pBtn->HideWnd();
				continue;
			}

			pBtn->ShowWnd();
			pBtn->SetIcon( NULL, USERINFO_TOOLTIP_MEDAL_SCALE, USERINFO_TOOLTIP_MEDAL_EMPTY_SCALE, USERINFO_TOOLTIP_MEDAL_DISABLE_SCALE, 0 );
			if( i < iSlotNum )
				pBtn->SetActive();     // empty slot
			else 
				pBtn->SetInActive();   // X 표시
		}
		else
		{
			if( bMedal3 && i >= iHalfMedalSize )
			{
				pBtn->HideWnd();
				continue;
			}

			// 확장메달슬롯에 대한 처리.
			bool bExpand = false;
			for( int j=0; j<iExpandSlotSize; ++j )
			{
				if( rkInfo.m_vExMedalSlotVec[j] == i )
					bExpand = true;
			}

			if( bExpand )
			{
				if( iCurMedal < iMedalSize )
				{
					const ioMedalItemInfoManager::ItemInfo *pItemInfo = g_MedalItemMgr.GetItemInfo( vMedalItemTypeExceptNoneVec[iCurMedal].m_iItemType );
					iCurMedal++;
					if( !pItemInfo )
					{
						pBtn->ShowWnd();
						pBtn->SetIcon( NULL, USERINFO_TOOLTIP_MEDAL_SCALE, USERINFO_TOOLTIP_MEDAL_EMPTY_SCALE, USERINFO_TOOLTIP_MEDAL_DISABLE_SCALE, 0 );
						pBtn->SetActive();     // empty slot
						continue;
					}

					ioUIRenderImage *pImg = g_UIImageSetMgr.CreateImageByFullName( pItemInfo->m_sIcon );
					ioUIRenderImage *pSubImg = g_UIImageSetMgr.CreateImageByFullName( pItemInfo->m_sSubIcon );

					pBtn->ShowWnd();
					pBtn->SetIcon( pImg, USERINFO_TOOLTIP_MEDAL_SCALE, USERINFO_TOOLTIP_MEDAL_EMPTY_SCALE, USERINFO_TOOLTIP_MEDAL_DISABLE_SCALE, 0 );
					pBtn->SetSubIcon( pSubImg );
					pBtn->SetActive();
				}
				else
				{
					pBtn->ShowWnd();
					pBtn->SetIcon( NULL, USERINFO_TOOLTIP_MEDAL_SCALE, USERINFO_TOOLTIP_MEDAL_EMPTY_SCALE, USERINFO_TOOLTIP_MEDAL_DISABLE_SCALE, 0 );
					pBtn->SetActive();     // empty slot
				}
			}
			else
			{
				if( i < iSlotNum )
				{
					pBtn->ShowWnd();
					pBtn->SetIcon( NULL, USERINFO_TOOLTIP_MEDAL_SCALE, USERINFO_TOOLTIP_MEDAL_EMPTY_SCALE, USERINFO_TOOLTIP_MEDAL_DISABLE_SCALE, 0 );
					pBtn->SetActive();     // empty slot
				}
				else
				{
					pBtn->ShowWnd();
					pBtn->SetIcon( NULL, USERINFO_TOOLTIP_MEDAL_SCALE, USERINFO_TOOLTIP_MEDAL_EMPTY_SCALE, USERINFO_TOOLTIP_MEDAL_DISABLE_SCALE, 0 );
					pBtn->SetInActive();// X 표시
				}
			}
		}
	}

	// wnd size
	if( bMedal3 )
		SetSize( GetWidth(), HEIGHT_MEDAL3 + iExtra_Add_Y );
	else
		SetSize( GetWidth(), HEIGHT_MEDAL6 + iExtra_Add_Y );

	//Loading End
	m_dwRefreshDelayTime = 0;
}

void UserInfoToolTip::ParseExtraInfo( ioXMLElement &xElement )
{
	m_vGrowthCharIconVec.clear();

	int iMax = xElement.GetIntAttribute_e( "MaxGrowthCharIcon" );
	for (int i = 0; i < iMax ; i++)
	{
		char szKeyName[MAX_PATH]="";
		ZeroMemory( szKeyName, sizeof( szKeyName ) );
		StringCbPrintf( szKeyName, sizeof( szKeyName ), "GrowthCharIcon%d", i+1 );
		ioHashString sTemp  = xElement.GetStringAttribute( szKeyName );
		m_vGrowthCharIconVec.push_back( sTemp );
	}

	m_dwMaxRefreshDelayTime = xElement.GetIntAttribute_e( "MaxRefreshDelayTime" );
	m_dwMaxLiveTime         = xElement.GetIntAttribute_e( "MaxLiveTime" );
	m_dwMaxUserInfo         = xElement.GetIntAttribute_e( "MaxUserInfo" );
}

bool UserInfoToolTip::GetUserInfoRoom( IN const ioHashString &sName, IN int iType, OUT UserInfo &rkUserInfo )
{
	if( sName.IsEmpty() ) 
		return false;

	if( !m_pPlayStage ) 
		return false;

	ioBaseChar *pChar = m_pPlayStage->GetBaseChar( sName );

	if( !pChar )
	{
		// 본부에서 skilllist over시 본부의 sName은 뒤에 (1) 처럼 인자가 붙음.
		if( sName == g_MyInfo.GetPublicID() )
		{
			int iCharIndex = g_MyInfo.GetCharIndex( g_MyInfo.GetSelectCharArray() );
			pChar = m_pPlayStage->GetBaseChar( iCharIndex );
		}
	}

	if( !pChar )
		return false;

	int iClassType			= pChar->GetCharacterInfo().m_class_type;
	int iPowerUpCharGrade	= pChar->GetCharacterInfo().m_eCharPowerUpType;

	// growth
	ioGrowthLevel *pGrowthLevel = m_pPlayStage->GetGrowthLevel( sName );
	for( int i=0; i < MAX_CHAR_GROWTH; ++i )
	{
		int iLevel = 0;
		if( pGrowthLevel )
			iLevel = pGrowthLevel->GetCharGrowthLevel( iClassType, i, false );
		rkUserInfo.m_vGrowthLevelVec.push_back( iLevel );
	}

	for( int i=0; i < MAX_ITEM_GROWTH; ++i )
	{
		int iLevel = 0;
		if( pGrowthLevel )
			iLevel = pGrowthLevel->GetItemGrowthLevel( iClassType, i, false );
		rkUserInfo.m_vGrowthLevelVec.push_back( iLevel );
	}

	// equip
	for(int i = 0;i < MAX_INVENTORY;i++)
	{
		DWORD dwItemCode = 0;
		ioItem *pItem = pChar->GetEquipedItem( i );
		if( pItem )
		{
			dwItemCode = pItem->GetItemCode();
		}
		else
		{
			int iItemCode = 0;
			if( iPowerUpCharGrade != PUGT_NONE )
			{
				iItemCode = (DEFAULT_BASIC_ITEM_CODE * i) + ConvertPowerUpCharToItem( iPowerUpCharGrade ) + iClassType;
				if( !g_PowerUpManager.IsPowerUpGradeItem( iClassType, iItemCode ) || !g_ItemMaker.IsItem( iItemCode ) )
				{
					// 아이템은 어쌔신일때 12, 100012, 200012, 300012 형식, 
					dwItemCode = (DEFAULT_BASIC_ITEM_CODE * i) + iClassType;
				}
			}
			else
			{
				// 아이템은 어쌔신일때 12, 100012, 200012, 300012 형식, 
				dwItemCode = (DEFAULT_BASIC_ITEM_CODE * i) + iClassType;
			}
		}

		rkUserInfo.m_vItemCodeVec.push_back( dwItemCode );
	}

	TotalResultMainWnd *pResultMainWnd = dynamic_cast<TotalResultMainWnd *>(g_GUIMgr.FindWnd(TOTAL_RESULT_MAIN_WND));

	// extra item
	int iClassLevel = pChar->GetClassLevel();
	int iGrowthSize = rkUserInfo.m_vGrowthLevelVec.size();
	for (int i = 0; i < MAX_INVENTORY; i++)
	{
		int iItemReinforce = 0;	
		if( pResultMainWnd && pResultMainWnd->IsShow() )
		{
			iItemReinforce = pResultMainWnd->GetAwardCharItemReinforce( sName, i );
			iClassLevel    = pResultMainWnd->GetAwardCharLevel( sName );
		}
		else
			iItemReinforce = pChar->GetDefaultReinforce( i );	

		if( COMPARE( i, 0, iGrowthSize ) )
		{
			if ( iItemReinforce <= g_CompoundMaterialMgr.GetFixedFailReinforce() )
				rkUserInfo.m_vGrowthLevelVec[i] += iItemReinforce;
			else
				rkUserInfo.m_vGrowthLevelVec[i] += g_GrowthInfoMgr.GetReinforceAddStat( iItemReinforce );
		}
		int iArray = i+MAX_CHAR_GROWTH;
		if( COMPARE( iArray, 0, iGrowthSize ) )
		{
			if ( iItemReinforce <= g_CompoundMaterialMgr.GetFixedFailReinforce() )
				rkUserInfo.m_vGrowthLevelVec[iArray] += iItemReinforce;
			else
				rkUserInfo.m_vGrowthLevelVec[iArray] += g_GrowthInfoMgr.GetReinforceAddStat( iItemReinforce );
		}
	}

	// expand medal slot
	IntVec kExMedalSlotVec;
	kExMedalSlotVec.clear();
	pChar->GetExMedalSlot( kExMedalSlotVec );
	rkUserInfo.m_vExMedalSlotVec = kExMedalSlotVec;

	// medal
	if( pResultMainWnd && pResultMainWnd->IsShow() )
	{
		if( pResultMainWnd )
			pResultMainWnd->GetAwardMedalItemType( sName, rkUserInfo.m_vMedalItemTypeVec );
	}
	else
		pChar->GetMedalItemType( rkUserInfo.m_vMedalItemTypeVec );

	int iMedalSize = rkUserInfo.m_vMedalItemTypeVec.size();
	for (int i = 0; i < iMedalSize ; i++)
	{
		for (int j = 0; j < MAX_CHAR_GROWTH; j++)
		{
			int iArray = j+MAX_ITEM_GROWTH;
			if( COMPARE( j, 0, iGrowthSize ) && COMPARE( iArray, 0, iGrowthSize ) )
			{
				if( rkUserInfo.m_vMedalItemTypeVec[i].m_iCustomIndex == 0 )
				{
					rkUserInfo.m_vGrowthLevelVec[j] += g_MedalItemMgr.GetMedalItemGrowth( rkUserInfo.m_vMedalItemTypeVec[i].m_iItemType, true, j );
					rkUserInfo.m_vGrowthLevelVec[iArray] += g_MedalItemMgr.GetMedalItemGrowth( rkUserInfo.m_vMedalItemTypeVec[i].m_iItemType, false, j );
				}
				else
				{
					rkUserInfo.m_vGrowthLevelVec[j] += rkUserInfo.m_vMedalItemTypeVec[i].m_iStat[j];
					rkUserInfo.m_vGrowthLevelVec[iArray] += rkUserInfo.m_vMedalItemTypeVec[i].m_iStat[iArray];
				}
			}
		}
	}

	//펫 성장
	CurPetInfo sCurPetInfo;
	if( m_pPlayStage->GetModeType() == MT_HEADQUARTERS )
	{
		if( ToNpcChar( pChar ) )
		{
			ioHeadquartersMode *pHeadquartersMode = ToHeadquartersMode( m_pPlayStage->GetPlayMode() );
			if( pHeadquartersMode )
				m_pPlayStage->GetCurPetInfo( pHeadquartersMode->GetMasterName(), sCurPetInfo );
		}
		else
			m_pPlayStage->GetCurPetInfo( pChar->GetCharName(), sCurPetInfo );
	}
	else
		m_pPlayStage->GetCurPetInfo( pChar->GetCharName(), sCurPetInfo );

	if( sCurPetInfo.m_nPetCode > 0 )
	{
		for ( int i = 0; i < MAX_CHAR_GROWTH+MAX_ITEM_GROWTH; i++ )
			rkUserInfo.m_vGrowthLevelVec[i] += g_PetInfoMgr.GetTotalStat( sCurPetInfo.m_nPetCode, i, sCurPetInfo.m_nCurLevel );
	}	

	//각성
	for( int i = 0; i < MAX_CHAR_GROWTH; i++ )
	{
		if( COMPARE( i, 0, iGrowthSize ) )
			 pChar->GetAwakeCharGrowth( rkUserInfo.m_vGrowthLevelVec[i], i );
	}

	for( int i = 0; i < MAX_CHAR_GROWTH; i++ )
	{
		int iArray = i + MAX_ITEM_GROWTH;
		if( COMPARE( iArray, 0, iGrowthSize ) )
			pChar->GetAwakeCharGrowth( rkUserInfo.m_vGrowthLevelVec[iArray], i );
	}

	//R 용병
	if ( g_RSoldierInfoManager.IsRSoldierType( pChar->GetCharacterInfo() ) )
	{
		int iCurRCount = 0;
		if( m_pPlayStage->GetModeType() == MT_HEADQUARTERS )
		{
			if( ToNpcChar( pChar ) )
			{
				ioHeadquartersMode *pHeadquartersMode = ToHeadquartersMode( m_pPlayStage->GetPlayMode() );
				if( pHeadquartersMode )
				{
					ioBaseChar *pMasterChar = m_pPlayStage->GetBaseChar( pHeadquartersMode->GetMasterName() );
					if ( pMasterChar )
						iCurRCount = pMasterChar->GetCurRSoldierCount();
				}
			}
			else
				iCurRCount = pChar->GetCurRSoldierCount();
		}
		else
			iCurRCount = pChar->GetCurRSoldierCount();

		if ( iCurRCount != 0 )
		{
			for ( int i = 0; i < MAX_CHAR_GROWTH+MAX_ITEM_GROWTH; i++ )
				rkUserInfo.m_vGrowthLevelVec[i] += g_RSoldierInfoManager.GetSoldierAddStat( i, iCurRCount );
		}
	}

	if ( g_RSoldierInfoManager.IsSoldierSet( pChar->GetCharacterInfo() ) )
	{
		int nCurCharSet = 0;
		if( m_pPlayStage->GetModeType() == MT_HEADQUARTERS )
		{
			if( ToNpcChar( pChar ) )
			{
				ioHeadquartersMode *pHeadquartersMode = ToHeadquartersMode( m_pPlayStage->GetPlayMode() );
				if( pHeadquartersMode )
				{
					ioBaseChar *pMasterChar = m_pPlayStage->GetBaseChar( pHeadquartersMode->GetMasterName() );
					if ( pMasterChar )
						nCurCharSet = pMasterChar->GetCurSoldierSetCnt();
				}
			}
			else
				nCurCharSet = pChar->GetCurSoldierSetCnt();
		}
		else
			nCurCharSet = pChar->GetCurSoldierSetCnt();

		if ( nCurCharSet != 0 )
		{
			for ( int i = 0; i < MAX_CHAR_GROWTH+MAX_ITEM_GROWTH; i++ )
				rkUserInfo.m_vGrowthLevelVec[i] += g_RSoldierInfoManager.GetSoldierSetAddStat( i, iClassType, nCurCharSet );
		}
	}

	if( m_pPlayStage->GetModeType() == MT_HEADQUARTERS )
	{
		if( ToNpcChar( pChar ) )
		{
			ioHeadquartersMode *pHeadquartersMode = ToHeadquartersMode( m_pPlayStage->GetPlayMode() );
			if( pHeadquartersMode )
				rkUserInfo.m_sName = pHeadquartersMode->GetMasterName();
		}
		else
		{
			rkUserInfo.m_sName = sName;
		}
	}
	else
	{
		rkUserInfo.m_sName = sName;
	}

	const CHARACTER &rkChar = pChar->GetCharacterInfo();
	for(int i=0; i<MAX_INVENTORY; i++ )
	{
		int nCostumeCode = rkChar.m_EquipCostume[i].m_CostumeCode;
		rkUserInfo.m_vCostumeCodeVec.push_back( nCostumeCode );
	}

	for(int i=0; i<MAX_ACC_INVENTORY; i++ )
	{
		int iAccessoryCode = rkChar.m_EquipAccessory[i].m_iAccessoryCode;
		rkUserInfo.m_vAccessoryCodeVec.push_back( iAccessoryCode );
	}

	rkUserInfo.m_iClassLevel		= iClassLevel;
	rkUserInfo.m_iClassType			= iClassType;
	rkUserInfo.m_iAwakeType			= pChar->GetAwakeType();
	rkUserInfo.m_iPowerUpCharGrade	= iPowerUpCharGrade;

	return true;
}

bool UserInfoToolTip::GetUserInfoMy( IN const ioHashString &sName, IN int iType, OUT UserInfo &rkUserInfo )
{
	if( sName.IsEmpty() )
		return false;

	if( sName != g_MyInfo.GetPublicID() )
		return false;

	int iClassType = 0;
	if( iType == TT_GLOBAL )
	{
		iClassType = g_MyInfo.GetClassType();
	}
	else if( iType == TT_ROOM )
	{
		// GetUserInfoChar() 처리
		return false;
	}
	else // iType is ClassType
	{
		TotalResultMainWnd *pResultMainWnd = dynamic_cast<TotalResultMainWnd *>(g_GUIMgr.FindWnd(TOTAL_RESULT_MAIN_WND));
		if( pResultMainWnd && pResultMainWnd->IsShow() )
			return false; // GetUserInfoChar() 처리

		iClassType = iType;
	}

	if( g_MyInfo.GetCharIndexByClassType( iClassType ) == 0 )
		return false;

	g_MyInfo.GetMedalTypeAndGrowthLevel( iClassType, rkUserInfo.m_vMedalItemTypeVec, rkUserInfo.m_vGrowthLevelVec );
	ioExpandMedalSlotManager::ExpandMedalSlotVec kExMedalSlotVec;
	int iSize = g_ExpandMedalSlotMgr.GetExpandMedalSlot( iClassType, kExMedalSlotVec );
	for( int i=0; i<iSize; ++i )
	{
		rkUserInfo.m_vExMedalSlotVec.push_back( kExMedalSlotVec[i].m_iSlotNumber );
	}

	for (int i = 0; i < MAX_INVENTORY ; i++)
	{
		DWORD dwItemCode = 0;
		const ITEM_DATA &rkItem = g_MyInfo.GetCharItemToClassType( iClassType, i );
		dwItemCode = rkItem.m_item_code;
		rkUserInfo.m_vItemCodeVec.push_back( dwItemCode );
	}

	const CHARACTER &rkCHARACTER = g_MyInfo.GetCharacterToClassType( iClassType );
	for ( int i=0; i<MAX_INVENTORY; i++ )
	{
		int nCostumeCode = rkCHARACTER.m_EquipCostume[i].m_CostumeCode;
		rkUserInfo.m_vCostumeCodeVec.push_back( nCostumeCode );
	}

	for(int i=0; i<MAX_ACC_INVENTORY; i++ )
	{
		int iAccessoryCode = rkCHARACTER.m_EquipAccessory[i].m_iAccessoryCode;
		rkUserInfo.m_vAccessoryCodeVec.push_back( iAccessoryCode );
	}
	
	rkUserInfo.m_sName				= sName;
	rkUserInfo.m_iClassLevel		= g_MyInfo.GetClassLevel( iClassType, false );
	rkUserInfo.m_iClassType			= iClassType;
	rkUserInfo.m_iAwakeType			= g_MyInfo.GetAwakeType( iClassType );
	rkUserInfo.m_iPowerUpCharGrade	= g_MyInfo.GetCharPowerUpGradeByClassType( iClassType );

	return true;
}

void UserInfoToolTip::SetPlayStage( ioPlayStage *pStage )
{
	m_pPlayStage = pStage;
}

bool UserInfoToolTip::GetUserInfoNetwork( IN const ioHashString &rsName, OUT UserInfo &rkInfo )
{
	if( m_dwPackSendTime > FRAMEGETTIME() )
	{
		return false;
	}

	// find
	bool bRefesh = false;	
	for(vUserInfo::iterator iter = m_vUserInfo.begin(); iter != m_vUserInfo.end(); ++iter)
	{
		UserInfo *pInfo = *iter;
		if( !pInfo )
			continue;

		if( rsName != pInfo->m_sName )
			continue;
		
		if( pInfo->m_bMustSync )
		{
			bRefesh	= true;
			break;
		}

		//Create Time Check
		if( REALGETTIME() - pInfo->m_dwCreateTime > m_dwMaxLiveTime )
		{
			bRefesh	= true;
			break;
		}		

		//Refresh Check
		if( REALGETTIME() - pInfo->m_dwRefreshTime > m_dwMaxRefreshDelayTime )
		{
			bRefesh	= true;
			break;
		}

		rkInfo = *pInfo;
		return true;
	}

	// 0.5초 이내로 패킷을 보내지 못하게 값을 조정
	m_dwPackSendTime = FRAMEGETTIME() + 500;
	
	SP2Packet kPacket( CTPK_SIMPLE_USER_INFO_REFRESH );
	kPacket << rsName;
	TCPNetwork::SendToServer( kPacket );

	//Start Loading
	m_sName = rsName;
	m_sClassInfo.Clear();
	m_sExtraInfo.Clear();
	m_dwRefreshDelayTime = REALGETTIME();
	for (int i = 0; i < MAX_GROWTH ; i++)
		HideChildWnd( ID_GROWTH_1_BTN + i );
	for (int i = 0; i < MAX_MEDAL ; i++)
		HideChildWnd( ID_MADAL_1_BTN + i );
	for (int i = 0; i < MAX_COSTUME ; i++)
		HideChildWnd( ID_COSTUME_1_BTN + i );

	return false;
}

void UserInfoToolTip::InsertUserInfo( IN UserInfo &rkInfo )
{
	if( rkInfo.m_sName.IsEmpty() )
		return;

	// max
	int iSize = m_vUserInfo.size();
	if( iSize > (int) m_dwMaxUserInfo )
	{
		if( !m_vUserInfo.empty() )
		{
			vUserInfo::iterator iter = m_vUserInfo.begin();
			SAFEDELETE( *iter );
			m_vUserInfo.erase( iter );
		}
	}

	for(vUserInfo::iterator iter = m_vUserInfo.begin(); iter != m_vUserInfo.end(); ++iter)
	{
		UserInfo *pInfo = *iter;
		if( !pInfo )
			continue;
		if( pInfo->m_sName != rkInfo.m_sName )
			continue;

		*pInfo = rkInfo;
		return;
	}

	// insert
	UserInfo *pInfo = new UserInfo;
	if( !pInfo )
		return;

	*pInfo = rkInfo;
	m_vUserInfo.push_back( pInfo );
}

void UserInfoToolTip::RefreshUserInfo( const ioHashString& szName )
{
	if( szName.IsEmpty() )
		return;	

	for(vUserInfo::iterator iter = m_vUserInfo.begin(); iter != m_vUserInfo.end(); ++iter)
	{
		UserInfo *pInfo = *iter;
		if( pInfo && pInfo->m_sName == szName )
		{
			pInfo->m_bMustSync = true;
		}
	}
}

void UserInfoToolTip::Clear()
{
	for( vUserInfo::iterator iter = m_vUserInfo.begin(); iter != m_vUserInfo.end(); ++iter )
	{
		UserInfo *pInfo = *iter;
		SAFEDELETE( pInfo );
	}
	m_vUserInfo.clear();
}

void UserInfoToolTip::ProcessLoading()
{
	if( FRAMEGETTIME() - m_dwInfoLoadTime > 100 )
	{
		m_dwInfoLoadTime = FRAMEGETTIME();
		m_iInfoLoadArray++;

		if( m_iInfoLoadArray >= MAX_INFO_LODING )
			m_iInfoLoadArray = 0;
	}
}

void UserInfoToolTip::iwm_show()
{
	m_sName.Clear();
	m_sClassInfo.Clear();
	m_sExtraInfo.Clear();

	m_dwRefreshDelayTime = 0;

	for (int i = 0; i < MAX_GROWTH ; i++)
		HideChildWnd( ID_GROWTH_1_BTN + i );
	for (int i = 0; i < MAX_MEDAL ; i++)
		HideChildWnd( ID_MADAL_1_BTN + i );
	for (int i = 0; i < MAX_COSTUME ; i++)
		HideChildWnd( ID_COSTUME_1_BTN + i );
}