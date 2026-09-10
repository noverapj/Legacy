
#include "StdAfx.h"

#include "ExpandMedalSlotResultWnd.h"
/////////////////////////////////////////////////////////////////////
MedalSlotResultBtn::MedalSlotResultBtn(void)
{
	m_pIcon = NULL;
	m_pSubIcon = NULL;
	m_pIconBack = NULL;
	m_pIconDisable = NULL;

	m_fIconScale = FLOAT1;
	m_fIconBackScale = FLOAT1;
	m_fIconDisableScale = FLOAT1;
}

MedalSlotResultBtn::~MedalSlotResultBtn(void)
{
	SAFEDELETE(m_pIcon);
	SAFEDELETE(m_pSubIcon);
	SAFEDELETE(m_pIconBack);
	SAFEDELETE(m_pIconDisable);

	m_fIconScale = FLOAT1;
	m_fIconBackScale = FLOAT1;
	m_fIconDisableScale = FLOAT1;
}

void MedalSlotResultBtn::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "IconBack" )
	{
		SAFEDELETE(m_pIconBack);
		m_pIconBack = pImage;
	}
	else if( szType == "IconDisable" )
	{
		SAFEDELETE(m_pIconDisable);
		m_pIconDisable = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );	
	}
}

void MedalSlotResultBtn::OnDrawNormal( int iXPos, int iYPos )
{
	ioButton::OnDrawNormal( iXPos, iYPos );

	RenderBack( iXPos, iYPos );
	RenderIcon( iXPos, iYPos , UI_RENDER_NORMAL );
}

void MedalSlotResultBtn::OnDrawOvered( int iXPos, int iYPos )
{
	ioButton::OnDrawOvered( iXPos, iYPos );

	RenderBack( iXPos, iYPos );
	RenderIcon( iXPos, iYPos , UI_RENDER_NORMAL );
}

void MedalSlotResultBtn::OnDrawPushed( int iXPos, int iYPos )
{
	ioButton::OnDrawPushed( iXPos, iYPos );

	RenderBack( iXPos, iYPos );
	RenderIcon( iXPos, iYPos , UI_RENDER_NORMAL );
}

void MedalSlotResultBtn::OnDrawDisabled( int iXPos, int iYPos )
{
	ioButton::OnDrawDisabled( iXPos, iYPos );

	RenderIconDisable( iXPos, iYPos );
}

void MedalSlotResultBtn::RenderIcon(int iXPos, int iYPos, UIRenderType eType)
{
	if( !m_pIcon )	return;

	int iNewXPos = iXPos + GetWidth() / 2;
	int iNewYPos = iYPos + GetHeight() / 2;

	m_pIcon->SetScale( m_fIconScale );
	if( m_fIconScale != FLOAT1 )
		m_pIcon->Render( iNewXPos, iNewYPos, eType, TFO_BILINEAR );
	else
		m_pIcon->Render( iNewXPos, iNewYPos, eType );

	if( m_pSubIcon )
	{
		m_pSubIcon->SetScale( m_fIconScale );
		m_pSubIcon->Render( iNewXPos, iNewYPos, eType, TFO_BILINEAR );
	}
}

void MedalSlotResultBtn::RenderBack(int iXPos, int iYPos )
{
	if( !m_pIconBack )	return;

	int iNewXPos = iXPos + GetWidth() / 2;
	int iNewYPos = iYPos + GetHeight() / 2;

	m_pIconBack->SetScale( m_fIconBackScale );
	if( m_fIconBackScale != FLOAT1 )
		m_pIconBack->Render( iNewXPos, iNewYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
	else
		m_pIconBack->Render( iNewXPos, iNewYPos, UI_RENDER_NORMAL );
}

void MedalSlotResultBtn::RenderIconDisable( int iXPos, int iYPos )
{
	if( !m_pIconDisable )	return;

	int iNewXPos = iXPos + GetWidth() / 2;
	int iNewYPos = iYPos + GetHeight() / 2;

	m_pIconDisable->SetScale( m_fIconDisableScale );

	if( m_fIconDisableScale != FLOAT1 )
		m_pIconDisable->Render( iNewXPos, iNewYPos, UI_RENDER_MULTIPLY, TFO_BILINEAR );
	else
		m_pIconDisable->Render( iNewXPos, iNewYPos, UI_RENDER_MULTIPLY );
}

void MedalSlotResultBtn::SetIcon( ioUIRenderImage* pIcon, float fIconScale, float fIconBackScale, float fIconDisableScale )
{
	SAFEDELETE( m_pIcon );
	SAFEDELETE( m_pSubIcon );
	m_pIcon = pIcon;

	m_fIconScale = fIconScale;
	m_fIconBackScale = fIconBackScale;
	m_fIconDisableScale = fIconDisableScale;
}

void MedalSlotResultBtn::SetSubIcon( ioUIRenderImage *pIcon )
{
	SAFEDELETE( m_pSubIcon );
	m_pSubIcon = pIcon;
}
/////////////////////////////////////////////////////////////////////
ExpandMedalSlotResultWnd::ExpandMedalSlotResultWnd(void)
{
	m_iClassType = 0;

	m_CharInfo.Init();

	m_pEffect = NULL;
	m_EffectPos.Init();
}

ExpandMedalSlotResultWnd::~ExpandMedalSlotResultWnd(void)
{
	SAFEDELETE( m_pEffect );
	m_EffectPos.Init();
}

void ExpandMedalSlotResultWnd::iwm_create()
{
	g_GUIMgr.AddWnd( "XML/MyInventoryHelpWnd.xml", this );
}

void ExpandMedalSlotResultWnd::iwm_show()
{
	UpdateData();
}

void ExpandMedalSlotResultWnd::iwm_hide()
{
	m_iClassType = 0;
}

void ExpandMedalSlotResultWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
		case ID_VIEW_SOLDIER:
			if( cmd == IOBN_BTNUP )
			{
				MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
				if( pInvenWnd )
					pInvenWnd->ShowSoldierTabDirect( m_iClassType );

				HideWnd();
			}
			break;
	}
}

void ExpandMedalSlotResultWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	ShowChildWnd( ID_MEDAL_ITEM_1_BTN );
	ShowChildWnd( ID_MEDAL_ITEM_2_BTN );
	ShowChildWnd( ID_MEDAL_ITEM_3_BTN );
	ShowChildWnd( ID_MEDAL_ITEM_4_BTN );
	ShowChildWnd( ID_MEDAL_ITEM_5_BTN );
	ShowChildWnd( ID_MEDAL_ITEM_6_BTN );

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GREEN );
	g_FontMgr.PrintText( iXPos+128, iYPos+421, FONT_SIZE_17, STR(1) );
}

void ExpandMedalSlotResultWnd::OnRenderAfterChild()
{
	ioWnd::OnRenderAfterChild();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	// Effect
	if( m_pEffect && m_EffectPos.x > 0 && m_EffectPos.y > 0 )
	{
		m_pEffect->SetScale( 1.7f );
		m_pEffect->Render( iXPos + m_EffectPos.x - 1, iYPos + m_EffectPos.y - 1, UI_RENDER_ADD );
	}
}

void ExpandMedalSlotResultWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	ioWnd::OnProcess( fTimePerSec );

	RollingEffect( fTimePerSec );
}

void ExpandMedalSlotResultWnd::UpdateData()
{
	int iCharArray = g_MyInfo.GetClassArray( m_iClassType );
	if( COMPARE( iCharArray, 0, g_MyInfo.GetCharCount() ) )
		m_CharInfo = g_MyInfo.GetCharacter( iCharArray );

	NewShopHelpWnd *pCharWnd = dynamic_cast<NewShopHelpWnd *>( FindChildWnd( ID_SOLDIER_WND ));
	if( pCharWnd )
	{
		pCharWnd->SetWndPos( 47, 6 );
		pCharWnd->SetZPosition( 600.0f );
		pCharWnd->SetChar( m_CharInfo );
		pCharWnd->SetEquip( false, false, false );
		pCharWnd->SetUserInfoToolTip( true );
		pCharWnd->ShowWnd();
	}

	// 메달 정보 셋팅
	MedalSyncInfoVec vItemTypeVec;
	g_MyInfo.GetMedalItemTypeVec( vItemTypeVec, m_iClassType );
	
	ioExpandMedalSlotManager::ExpandMedalSlotVec vExpandMedalSlot;
	vExpandMedalSlot.clear();
	int iExpandMedalSlotNum = g_ExpandMedalSlotMgr.GetExpandMedalSlot( m_iClassType, vExpandMedalSlot );
	int iSlotNum = g_MedalItemMgr.GetSlotNum( g_MyInfo.GetClassLevel(m_iClassType,false) );

	int iTotalSlotNum = iSlotNum + iExpandMedalSlotNum;
	int iCurMedal = 0;
	for (int i = 0; i < ioMedalItemInfoManager::MAX_SLOT_NUM ; i++)
	{
		MedalSlotResultBtn *pBtn = dynamic_cast<MedalSlotResultBtn*> ( FindChildWnd( ID_MEDAL_ITEM_1_BTN + i ) );
		if( !pBtn )
			continue;

		if( i < iSlotNum  )
		{
			ioUIRenderImage *pIcon = NULL;
			ioUIRenderImage *pSubIcon = NULL;
			char szTitle[MAX_PATH]="";
			if( COMPARE( i, 0, (int) vItemTypeVec.size() ) )
			{
				const ioMedalItemInfoManager::ItemInfo *pItemInfo = g_MedalItemMgr.GetItemInfo( vItemTypeVec[iCurMedal].m_iItemType );
				if( !pItemInfo )
					continue;
				pIcon = g_UIImageSetMgr.CreateImageByFullName( pItemInfo->m_sIcon );
				pSubIcon = g_UIImageSetMgr.CreateImageByFullName( pItemInfo->m_sSubIcon );
				StringCbCopy( szTitle, sizeof( szTitle ), pItemInfo->m_sName.c_str() );
				iCurMedal++;
			}
			else
			{
				SafeSprintf( szTitle, sizeof( szTitle ), STR(1), i+1 );
			}

			pBtn->SetIcon( pIcon, MEDAL_ICON_SCALE, MEDAL_ICON_BACK_SCALE, MEDAL_ICON_DISABLE_SCALE );
			pBtn->SetSubIcon( pSubIcon );
			pBtn->SetActive();
			pBtn->SetTitleText( szTitle );
			ioUITitle *pTitle = pBtn->GetTitle();
			if( pTitle )
			{
				pTitle->SetWidthCutSize(59.0f);
				pTitle->SetPrintTextStyle( ioUITitle::PTS_WIDTHCUT );
			}
		}
		else if( iExpandMedalSlotNum > 0 )
		{
			ioUIRenderImage *pIcon = NULL;
			ioUIRenderImage *pSubIcon = NULL;
			char szTitle[MAX_PATH]="";
			bool bExpand = false;
			for( int j=0; j<iExpandMedalSlotNum; ++j )
			{
				if( vExpandMedalSlot[j].m_iSlotNumber == i )
				{
					if( COMPARE( iCurMedal, 0, (int) vItemTypeVec.size() ) )
					{
						const ioMedalItemInfoManager::ItemInfo *pItemInfo = g_MedalItemMgr.GetItemInfo( vItemTypeVec[iCurMedal].m_iItemType );
						if( !pItemInfo )	continue;
						pIcon = g_UIImageSetMgr.CreateImageByFullName( pItemInfo->m_sIcon );
						pSubIcon = g_UIImageSetMgr.CreateImageByFullName( pItemInfo->m_sSubIcon );

						if( vExpandMedalSlot[j].IsMortmain() )
						{
							pBtn->SetTitleColor( 0xFF6E6E6E, 0 );
							StringCbCopy( szTitle, sizeof( szTitle ), pItemInfo->m_sName.c_str() );
						}
						else
						{
							pBtn->SetTitleColor( 0xFFFF0000, 0 );
							g_ExpandMedalSlotMgr.CheckRemainTime( vExpandMedalSlot[j].m_iClassType, vExpandMedalSlot[j].m_iSlotNumber, szTitle, sizeof(szTitle) );
						}

						iCurMedal++;
					}
					else
					{
						if( vExpandMedalSlot[j].IsMortmain() )
						{
							pBtn->SetTitleColor( 0xFF6E6E6E, 0 );
							SafeSprintf( szTitle, sizeof( szTitle ), STR(2), i+1 );
						}
						else
						{
							pBtn->SetTitleColor( 0xFFFF0000, 0 );
							g_ExpandMedalSlotMgr.CheckRemainTime( vExpandMedalSlot[j].m_iClassType, vExpandMedalSlot[j].m_iSlotNumber, szTitle, sizeof(szTitle) );
						}
					}
					bExpand = true;
				}
			}

			if( bExpand )
			{
				pBtn->SetIcon( pIcon, MEDAL_ICON_SCALE, MEDAL_ICON_BACK_SCALE, MEDAL_ICON_DISABLE_SCALE );
				pBtn->SetSubIcon( pSubIcon );
				pBtn->SetActive();
			}
			else
			{
				pBtn->SetIcon( NULL , MEDAL_ICON_SCALE, MEDAL_ICON_BACK_SCALE, MEDAL_ICON_DISABLE_SCALE );
				pBtn->SetInActive();
				SafeSprintf( szTitle, sizeof( szTitle ), STR(3), g_MedalItemMgr.GetLevelOver( i + 1 ) );
				pBtn->SetTitleColor( 0xFF6E6E6E, 0 );
			}

			pBtn->SetTitleText( szTitle );
			ioUITitle *pTitle = pBtn->GetTitle();
			if( pTitle )
			{
				pTitle->SetWidthCutSize(59.0f);
				pTitle->SetPrintTextStyle( ioUITitle::PTS_WIDTHCUT );
			}
		}
		else
		{
			pBtn->SetIcon( NULL , MEDAL_ICON_SCALE, MEDAL_ICON_BACK_SCALE, MEDAL_ICON_DISABLE_SCALE );
			pBtn->SetInActive();
			char szTitle[MAX_PATH]="";
			SafeSprintf( szTitle, sizeof( szTitle ), STR(4), g_MedalItemMgr.GetLevelOver( i + 1 ) );
			pBtn->SetTitleColor( 0xFF6E6E6E, 0 );
			pBtn->SetTitleText( szTitle );
		}
	}
}

void ExpandMedalSlotResultWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_szResultSound = xElement.GetStringAttribute_e( "Result_Sound" );

	m_dwRollingEffectTime = xElement.GetIntAttribute_e( "Effect_Gap_Time" );

	m_vEndEfffect.clear();
	int iMax = xElement.GetIntAttribute_e( "Max_Result_Effect" );
	m_vEndEfffect.reserve( iMax );
	for (int i = 0; i < iMax ; i++)
	{
		char szName[MAX_PATH]="";
		StringCbPrintf_e( szName, sizeof( szName ), "Result_Effect_%d", i+1 );
		ioHashString szEffect = xElement.GetStringAttribute( szName );
		if( szEffect == "" )
			break;
		m_vEndEfffect.push_back( szEffect );
	}
}

void ExpandMedalSlotResultWnd::SetInfo( int iClassType, int iSlotNumber )
{
	m_iClassType = iClassType;
	ShowWnd();

	m_iCurEffectArray = -1;

	MedalSlotResultBtn *pBtn = dynamic_cast<MedalSlotResultBtn*> ( FindChildWnd( iSlotNumber + ID_MEDAL_ITEM_1_BTN ) );
	if( pBtn )
		m_EffectPos.SetPosition( pBtn->GetXPos() + pBtn->GetWidth()/2 , pBtn->GetYPos() + pBtn->GetHeight()/2 );
}

void ExpandMedalSlotResultWnd::RollingEffect( float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();

	int iEffectListCnt = m_vEndEfffect.size();
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

	if( m_iCurEffectArray == 0 )
	{
		if( !m_szResultSound.IsEmpty() )
			g_SoundMgr.PlaySound( m_szResultSound );
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