
#include "StdAfx.h"

#include "ShopItemMovieWnd.h"

#include "ioFlashPlayer.h"

ShopItemMovieWnd::ShopItemMovieWnd()
{
	m_pDescAllBg = NULL;
	m_pDescFirstBg = NULL;
	m_nType = DT_NONE;
}

ShopItemMovieWnd::~ShopItemMovieWnd()
{
	SAFEDELETE( m_pDescAllBg );
	SAFEDELETE( m_pDescFirstBg );
}

void ShopItemMovieWnd::iwm_show()
{	
	SetLoadFlash( "resource/swf/loading.swf" );
	SetDescShow();
}

void ShopItemMovieWnd::iwm_hide()
{
}

void ShopItemMovieWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
		if( cmd == IOBN_BTNUP )
			HideWnd();
		break;
	}
}

void ShopItemMovieWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "Desc_All" )
	{
		SAFEDELETE( m_pDescAllBg );
		m_pDescAllBg = pImage;
	}
	else if( szType == "Desc_First" )
	{
		SAFEDELETE( m_pDescFirstBg );
		m_pDescFirstBg = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}

void ShopItemMovieWnd::OnRender()
{
	ioWnd::OnRender();
		
	int nXPos = GetDerivedPosX();
	int nYPos = GetDerivedPosY();
	
	RenderDescBG( nXPos, nYPos );
}

void ShopItemMovieWnd::SetLoadFlash( const ioHashString& szFileName )
{
	ioFlashPlayer *pFlashWnd = dynamic_cast<ioFlashPlayer*>( FindChildWnd( ID_FLASH_WND ) );
	if ( pFlashWnd )
	{
		if ( pFlashWnd->Load( szFileName.c_str() ) )
		{
			//투명모드
			//pFlashWnd->SetFlashAlphaMode( true );
			pFlashWnd->SetFlashBKColor( 0 );
			pFlashWnd->ShowWnd();
		}
		else
			pFlashWnd->HideWnd();
	}
}

void ShopItemMovieWnd::SetResetFlash()
{
	ioFlashPlayer *pFlashWnd = dynamic_cast<ioFlashPlayer*>( FindChildWnd( ID_FLASH_WND ) );
	if ( pFlashWnd )
		pFlashWnd->Load( "resource/swf/loading.swf" );
}

void ShopItemMovieWnd::SetSelectDesc( int nArray, DWORD dwItemCode )
{
	if ( !COMPARE (ID_SUB_DESC1_WND+nArray, ID_SUB_DESC1_WND, ID_SUB_DESC_MAX ) )
		return;

	ShopItemDescWnd *pShopItemDescWnd = dynamic_cast<ShopItemDescWnd*>(FindChildWnd( ID_SUB_DESC1_WND + nArray ));
	if ( pShopItemDescWnd )
		pShopItemDescWnd->SetDesc( dwItemCode );
}

void ShopItemMovieWnd::SetSelectType( int nType )
{
	if ( !COMPARE( nType, DT_NONE, DT_ALL+1 ) )
		m_nType = DT_NONE;
	else
		m_nType = nType;
}

void ShopItemMovieWnd::SetDescShow()
{
	enum
	{
		DESC_X = 10,
		DESC_Y = 167,
		DESC_GAP_HEIGHT = 42,

		WND_HEIGHT_NONE = 200,
		WND_HEIGHT_ONE = 227,
		WND_HEIGHT_ALL = 358,

		WND_HEIGHT_OFFSET_Y = -20,
	};
	
	int nCenterX = Setting::HalfWidth();
	int nCenterY = Setting::HalfHeight();

	switch( m_nType )
	{
	case DT_NONE:
		{
			HideChildWnd( ID_SUB_DESC1_WND );
			HideChildWnd( ID_SUB_DESC2_WND );
			HideChildWnd( ID_SUB_DESC3_WND );
			HideChildWnd( ID_SUB_DESC4_WND );

			SetSize( GetWidth(), WND_HEIGHT_NONE );
			SetScreenWndPos( nCenterX - GetWidth()/2, nCenterY - GetHeight()/2 + WND_HEIGHT_OFFSET_Y );
		}
		break;
	case DT_FIRST:
		{
			ShopItemDescWnd *pShopItemDescWnd = dynamic_cast<ShopItemDescWnd*>(FindChildWnd( ID_SUB_DESC1_WND ));
			if ( pShopItemDescWnd )
			{
				pShopItemDescWnd->SetWndPos( DESC_X, DESC_Y );
				pShopItemDescWnd->ShowWnd();
			}

			HideChildWnd( ID_SUB_DESC2_WND );
			HideChildWnd( ID_SUB_DESC3_WND );
			HideChildWnd( ID_SUB_DESC4_WND );

			SetSize( GetWidth(), WND_HEIGHT_ONE );
			SetScreenWndPos( nCenterX - GetWidth()/2, nCenterY - GetHeight()/2 + WND_HEIGHT_OFFSET_Y );
		}
		break;
	case DT_ALL:
		{
			for ( int i=0; i<4; ++i )
			{
				ShopItemDescWnd *pShopItemDescWnd = dynamic_cast<ShopItemDescWnd*>(FindChildWnd( ID_SUB_DESC1_WND+i ));
				if ( pShopItemDescWnd )
				{
					pShopItemDescWnd->SetWndPos( DESC_X, DESC_Y + i*DESC_GAP_HEIGHT );
					pShopItemDescWnd->ShowWnd();
				}
			}
			SetSize( GetWidth(), WND_HEIGHT_ALL );			
			SetScreenWndPos( nCenterX - GetWidth()/2, nCenterY - GetHeight()/2 + WND_HEIGHT_OFFSET_Y );
		}
		break;
	}
}

void ShopItemMovieWnd::SetDownFail()
{
	SetLoadFlash( "resource/swf/download.swf" );
}

void ShopItemMovieWnd::RenderDescBG( int nXpos, int nYpos )
{
	if ( !m_pDescAllBg || !m_pDescFirstBg )
		return;
	
	switch( m_nType )
	{
	case DT_FIRST:
		m_pDescFirstBg->Render( nXpos, nYpos );
		break;
	case DT_ALL:
		m_pDescAllBg->Render( nXpos, nYpos );
		break;
	}
}

/////////////////////////////////////////////////////////////////////////////////////

ShopItemDescWnd::ShopItemDescWnd()
{
	m_pIcon = NULL;
	m_pPart = NULL;
	m_pInput = NULL;
}

ShopItemDescWnd::~ShopItemDescWnd()
{
	SAFEDELETE( m_pIcon );
	SAFEDELETE( m_pPart );
	SAFEDELETE( m_pInput );
}

void ShopItemDescWnd::iwm_show()
{	
}

void ShopItemDescWnd::iwm_hide()
{	
}

void ShopItemDescWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_szWeaponPart = xElement.GetStringAttribute( "Weapon_Part" );
	m_szArmorPart = xElement.GetStringAttribute( "Armor_Part" );
	m_szHeadPart = xElement.GetStringAttribute( "Head_Part" );
	m_szCloakPart = xElement.GetStringAttribute( "Cloak_Part" );

	m_szPassiveInput = xElement.GetStringAttribute( "Passive_Input" );
	m_szWeaponInput = xElement.GetStringAttribute( "Weapon_Input" );
	m_szArmorInput = xElement.GetStringAttribute( "Armor_Input" );
	m_szHeadInput = xElement.GetStringAttribute( "Head_Input" );
	m_szCloakInput = xElement.GetStringAttribute( "Cloak_Input" );
}

void ShopItemDescWnd::OnRender()
{
	ioWnd::OnRender();

	enum
	{
		ICON_OFFSET_X = 33,
		ICON_OFFSET_Y = 21,

		PART_OFFSET_X = 257,
		PART_OFFSET_Y = 5,

		INPUT_OFFSET_X = 279,
		INPUT_OFFSET_Y = 5,

		SKILL_NAME_X = 60,
		SKILL_NAME_Y = 7,

		SKILL_DESC_X = 60,
		SKILL_DESC_Y = 22,
	};

	int nXpos = GetDerivedPosX();
	int nYpos = GetDerivedPosY();

	// Icon
	if( m_pIcon )
		m_pIcon->Render( nXpos+ICON_OFFSET_X, nYpos+ICON_OFFSET_Y, UI_RENDER_NORMAL, TFO_BILINEAR );

	if ( m_pPart)
		m_pPart->Render( nXpos+PART_OFFSET_X, nYpos+PART_OFFSET_Y, UI_RENDER_NORMAL );

	// Input
	if ( m_pInput)
		m_pInput->Render( nXpos+INPUT_OFFSET_X, nYpos+INPUT_OFFSET_Y, UI_RENDER_NORMAL );

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );

	//스킬 이름
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetTextColor( D3DCOLOR_XRGB( 198, 218, 231 ) );
	g_FontMgr.PrintTextWidthCut( nXpos + SKILL_NAME_X, nYpos + SKILL_NAME_Y, FONT_SIZE_12, 252, m_szSkillName.c_str() );
	
	//스킬 설명
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetTextColor( D3DCOLOR_XRGB( 98, 145, 178 ) );
	g_FontMgr.PrintTextWidthCut( nXpos + SKILL_DESC_X, nYpos + SKILL_DESC_Y, FONT_SIZE_11, 252, m_szSkillDesc.c_str() );
}

void ShopItemDescWnd::SetDesc( DWORD dwItemCode )
{
	const ioItem *pItem = g_ItemMaker.GetItemConst( dwItemCode, __FUNCTION__ );
	if ( !pItem )
		return;

	ioHashString szSkillName = pItem->GetSkillName();
	const ioSkill *pSkill = g_SkillMaker.GetSkillConst( szSkillName );
	if ( !pSkill )
		return;

	//스킬 이름, 스킬 설명
	ioHashString szSkillDescFull = pSkill->GetDescFullName();
	if ( !szSkillDescFull.IsEmpty() )
		m_szSkillName = szSkillDescFull;
	else
		m_szSkillName = pSkill->GetDescName();

	m_szSkillDesc = pSkill->GetDesc();

	SAFEDELETE( m_pIcon );
	SAFEDELETE( m_pPart );
	SAFEDELETE( m_pInput );

	//스킬 아이콘 설정
	ioHashString szIconName = pSkill->GetIconName();
	m_pIcon = g_UIImageSetMgr.CreateImageByFullName( szIconName );
	if ( m_pIcon )
		m_pIcon->SetScale( 0.6f, 0.6f );
	else
	{
		LOG.PrintTimeAndLog( 0, "%s Icon is not exist :%s", __FUNCTION__, szIconName );
	}
	
	//입력 아이콘 설정
	int nConfirmCode = dwItemCode / IC_WeaponCode;
	if ( nConfirmCode != IC_YES )
	{
		nConfirmCode = dwItemCode / IC_NoWeaponCode;
		switch( nConfirmCode )
		{
		case IC_Weapon:
			m_pPart = g_UIImageSetMgr.CreateImageByFullName( m_szWeaponPart );
			m_pInput = g_UIImageSetMgr.CreateImageByFullName( m_szWeaponInput );
			break;
		case IC_Armor:
			m_pPart = g_UIImageSetMgr.CreateImageByFullName( m_szArmorPart );
			m_pInput = g_UIImageSetMgr.CreateImageByFullName( m_szArmorInput );
			break;
		case IC_Head:
			m_pPart = g_UIImageSetMgr.CreateImageByFullName( m_szHeadPart );
			m_pInput = g_UIImageSetMgr.CreateImageByFullName( m_szHeadInput );
			break;
		case IC_Cloak:
			m_pPart = g_UIImageSetMgr.CreateImageByFullName( m_szCloakPart );
			m_pInput = g_UIImageSetMgr.CreateImageByFullName( m_szCloakInput );
			break;
		}
	}
	else
	{
		m_pPart = g_UIImageSetMgr.CreateImageByFullName( m_szWeaponPart );
		m_pInput = g_UIImageSetMgr.CreateImageByFullName( m_szWeaponInput );
	}

	if ( pSkill->GetTypeID() == ST_PASSIVE )
	{
		SAFEDELETE( m_pInput );
		m_pInput = g_UIImageSetMgr.CreateImageByFullName( m_szPassiveInput );
	}
}