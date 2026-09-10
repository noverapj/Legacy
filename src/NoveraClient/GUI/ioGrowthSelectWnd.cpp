
#include "StdAfx.h"

#include "ioGrowthSelectWnd.h"
#include "../ioSoldierAwakeManager.h"
#include "../ioRSoldierInfoManager.h"
#include "../ioPowerUpManager.h"

//////////////////////////////////////////////////////////////////////////
ioGrowthSelectWnd::ioGrowthSelectWnd()
{
	m_iClassType = 0;

	m_pPointImg = NULL;
	m_pBoxLeft = NULL;
	m_pBoxTop = NULL;
	m_pDarkStrip = NULL;
	m_pLightStrip = NULL;
	m_pGaugeBack = NULL;
	m_pGaugeGray = NULL;
	m_pGaugeGreen = NULL;

	for( int i = 0; i < CUR_GROWTH_CNT; i++ )
	{
		m_pCharIcon[i] = NULL;
		m_pSkillIcon[i] = NULL;
	}

	m_pInnerFrm	= NULL;	// 2020-08-07
}

ioGrowthSelectWnd::~ioGrowthSelectWnd()
{
	SAFEDELETE( m_pPointImg );
	SAFEDELETE( m_pBoxLeft );
	SAFEDELETE( m_pBoxTop );
	SAFEDELETE( m_pDarkStrip );
	SAFEDELETE( m_pLightStrip );
	SAFEDELETE( m_pGaugeBack );
	SAFEDELETE( m_pGaugeGray );
	SAFEDELETE( m_pGaugeGreen );

	for( int i = 0; i < CUR_GROWTH_CNT; i++ )
	{
		SAFEDELETE( m_pCharIcon[i] );
		SAFEDELETE( m_pSkillIcon[i] );
	}

	SAFEDELETE( m_pInnerFrm );	// 2020-08-07
}

void ioGrowthSelectWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "InnerFrm" )
	{
		SAFEDELETE( m_pInnerFrm );
		m_pInnerFrm = pFrame;
	}
	else
	{
		ioWnd::AddRenderFrame( szType, pFrame );
	}
}

void ioGrowthSelectWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "P_Img" )
	{
		SAFEDELETE( m_pPointImg );
		m_pPointImg = pImage;
	}
	else if( szType == "box_left" )
	{
		SAFEDELETE( m_pBoxLeft );
		m_pBoxLeft = pImage;
	}
	else if( szType == "box_top" )
	{
		SAFEDELETE( m_pBoxTop );
		m_pBoxTop = pImage;
	}
	else if( szType == "dark_strip" )
	{
		SAFEDELETE( m_pDarkStrip );
		m_pDarkStrip = pImage;
	}
	else if( szType == "light_strip" )
	{
		SAFEDELETE( m_pLightStrip );
		m_pLightStrip = pImage;
	}
	else if( szType == "gauge_back" )
	{
		SAFEDELETE( m_pGaugeBack );
		m_pGaugeBack = pImage;
	}
	else if( szType == "gauge_gray" )
	{
		SAFEDELETE( m_pGaugeGray );
		m_pGaugeGray = pImage;
	}
	else if( szType == "gauge_green" )
	{
		SAFEDELETE( m_pGaugeGreen );
		m_pGaugeGreen = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}

void ioGrowthSelectWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	char szKeyName[MAX_PATH]="";
	ioHashString szTemp;

	for (int i = 0; i < CUR_GROWTH_CNT ; i++)
	{
		// CharGrowth IconName
		ZeroMemory( szKeyName, sizeof( szKeyName ) );
		StringCbPrintf_e( szKeyName, sizeof( szKeyName ), "CharIcon%d", i+1 );
		szTemp  = xElement.GetStringAttribute( szKeyName );

		SAFEDELETE( m_pCharIcon[i] );
		m_pCharIcon[i] = g_UIImageSetMgr.CreateImageByFullName(szTemp);
		if( m_pCharIcon[i] )
			m_pCharIcon[i]->SetScale( 0.24f );

		// Char, Item Growth Text
		ZeroMemory( szKeyName, sizeof( szKeyName ) );
		StringCbPrintf_e( szKeyName, sizeof( szKeyName ), "CharGrowth%d", i+1 );
		szTemp  = xElement.GetStringAttribute( szKeyName );
		m_CharGrowthText.push_back( szTemp );

		ZeroMemory( szKeyName, sizeof( szKeyName ) );
		StringCbPrintf_e( szKeyName, sizeof( szKeyName ), "ItemGrowth%d", i+1 );
		szTemp  = xElement.GetStringAttribute( szKeyName );
		m_ItemGrowthText.push_back( szTemp );
	}
	for (int i = 0; i < 10 ; i++)
	{
		ZeroMemory( szKeyName, sizeof( szKeyName ) );
		StringCbPrintf_e( szKeyName, sizeof( szKeyName ), "BeforeRenderText%d", i+1 );
		szTemp  = xElement.GetStringAttribute( szKeyName );
		m_szBeforeRenderText[i] = szTemp;
	}
	for (int i = 0; i < 5 ; i++)
	{
		ZeroMemory( szKeyName, sizeof( szKeyName ) );
		StringCbPrintf_e( szKeyName, sizeof( szKeyName ), "PointPesoText%d", i+1 );
		szTemp  = xElement.GetStringAttribute( szKeyName );
		m_szPointPesoText[i] = szTemp;
	}
	m_szCurGrowthText = xElement.GetStringAttribute_e( "CurGrowthText" );
	m_szGrowthText = xElement.GetStringAttribute_e( "GrowthText" );
	m_szConfirmText = xElement.GetStringAttribute_e( "ConfirmText" );
	m_szNoMoneyText = xElement.GetStringAttribute_e( "NoMoneyText" );
}

void ioGrowthSelectWnd::SetGrowthSelectInfo( int iClassType )
{
	m_iClassType = iClassType;
	m_iMaxLevel = g_MyInfo.GetClassLevel( m_iClassType, true );
	m_iMaxLevel = min( m_iMaxLevel, g_GrowthInfoMgr.GetMaxLevel() );

	if( !IsShow() )
		ShowWnd();

	UpdateInfo();

	// icon
	InventoryIconBtn *pBtn = dynamic_cast< InventoryIconBtn* > ( FindChildWnd( ID_ICON_BTN ) );
	if( pBtn )
	{
		ioUIRenderImage *pImg = g_UIImageSetMgr.CreateImageByFullName(g_MyInfo.GetMySoldierIconName(m_iClassType));
		ioUIRenderImage *pSubImg = g_UIImageSetMgr.CreateImageByFullName( g_MyInfo.GetSoldierSubIconName( m_iClassType ) );
		pBtn->SetIcon( pImg, FLOAT1, FLOAT1 );
		pBtn->SetSubIcon( pSubImg );
		pBtn->SetGradeType( g_UIImageRenderMgr.GetPowerUpGradeType( m_iClassType ) );
	}
}

void ioGrowthSelectWnd::UpdateInfo()
{
	char szBuf[MAX_PATH] = "";

	m_ClassInfo[0].ClearList();
	m_ClassInfo[1].ClearList();

	ioGrowthLevel *pLevel = g_MyInfo.GetGrowthLevel();
	if( !pLevel )
	{
		return;
	}

	bool bHired = false;
	int iCharArray = g_MyInfo.GetClassArray( m_iClassType );
	if( g_MyInfo.IsCharMortmain( m_iClassType ) )
		bHired = true;
	else if( g_MyInfo.IsCharExerciseStyle( iCharArray, EXERCISE_GENERAL ) )
		bHired = true;
	else if( g_MyInfo.IsCharExerciseStyle( iCharArray, EXERCISE_PCROOM ) )
		bHired = true;
	else if( g_MyInfo.IsCharExerciseStyle( iCharArray, EXERCISE_EVENT ) )
		bHired = true;
	else if( g_MyInfo.IsCharExerciseStyle( iCharArray, EXERCISE_EVENT ) )
		bHired = true;
	else if( g_MyInfo.IsCharExerciseStyle( iCharArray, EXERCISE_RENTAL) )
		bHired = true;
	else
	{
		if( g_MyInfo.GetCharTotalSecond( iCharArray ) > 0 )
			bHired = true;
	}
	
	wsprintf_e( szBuf, "Lv%d %s", g_MyInfo.GetClassLevel( m_iClassType, false ), g_MyInfo.GetClassName( m_iClassType ) );
	if( g_FontMgr.GetTextWidth( szBuf, TS_NORMAL, FONT_SIZE_13 ) >= 117 )
	{
		char szTextWidthCut[MAX_PATH];
		Help::StringCutFun( FONT_SIZE_13, 112, TS_NORMAL, szTextWidthCut, MAX_PATH, szBuf );
		sprintf_s( szBuf, "%s...", szTextWidthCut );
	}

	m_ClassInfo[0].SetTextStyle( TS_NORMAL );
	m_ClassInfo[0].SetBkColor( 0, 0, 0 );
	if( bHired )
		m_ClassInfo[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
	else
		m_ClassInfo[0].SetTextColor( TCT_DEFAULT_GRAY );

	m_ClassInfo[0].AddTextPiece( FONT_SIZE_13, szBuf );

	ioGrowthInfo *pInfo = NULL;
	ioUserPet *pUserPet = g_MyInfo.GetUserPet();
	// Skill
	int i;
	int iRealItemCode = 0;
	for( i=0; i < CUR_GROWTH_CNT; ++i )
	{
		m_iCharLevelUp[i] = 0;
		m_iItemLevelUp[i] = 0;

		// GrowthInfoNum
		if( iCharArray != -1 )
		{
			const ITEM_DATA &kItem = g_MyInfo.GetCharItem( iCharArray, i );
			iRealItemCode = kItem.m_item_code;
		}
		else
		{
			iRealItemCode = i*DEFAULT_BASIC_ITEM_CODE + m_iClassType;
		}

		int iGrowthInfo = 0;
		const ioItem *pItem = g_ItemMaker.GetItemConst( iRealItemCode, __FUNCTION__ );
		if( pItem )
		{
			ioHashString szSkillName = pItem->GetSkillName();
			const ioSkill *pSkill = g_SkillMaker.GetSkillConst( szSkillName );
			if( pSkill )
			{
				ioHashString szSkillIcon = pSkill->GetIconName();
				SAFEDELETE( m_pSkillIcon[i] );
				m_pSkillIcon[i] = g_UIImageSetMgr.CreateImageByFullName(szSkillIcon);
				if( m_pSkillIcon[i] )
					m_pSkillIcon[i]->SetScale( 0.24f );
			}

			iGrowthInfo = pItem->GetGrowthInfoNum( 1 );
			m_ItemGrowth[i] = iGrowthInfo;
		}

		// GrowthLevel
		ioHashString szName;
		int iGrowthType = 0;

		m_ItemLevel[i] = 0;
		pInfo = g_GrowthInfoMgr.GetGrowthInfo( m_ItemGrowth[i] );
		if( pInfo )
		{
			szName = pInfo->GetGrowthInfoName();
			iGrowthType = pInfo->GetGrowthType();
			m_ItemLevel[i] = pLevel->GetItemGrowthLevel( m_iClassType, i, false );
		}
		m_EquipItemGrowthText[i].Clear();

		switch( iGrowthType )
		{
		case GT_CHAR_ATTACK:
		case GT_CHAR_DEFENSE:
		case GT_CHAR_MOVE_SPEED:
		case GT_CHAR_DROP_DAMAGE:
		case GT_SKILL_ATTACK:
		case GT_SKILL_MOVE_SPEED:
		case GT_SKILL_DROP_DAMAGE:
		case GT_SKILL_DEFENSE:
			m_EquipItemGrowthText[i] = szName;
			break;
		}

		m_CharGrowth[i] = ioBaseChar::m_vGrowthInfoNum[i];

		m_CharLevel[i] = 0;
		pInfo = g_GrowthInfoMgr.GetGrowthInfo( m_CharGrowth[i] );
		if( pInfo )
			m_CharLevel[i] = pLevel->GetCharGrowthLevel( m_iClassType, i, false );

		const ITEM_DATA &kItem = g_MyInfo.GetCharItem( iCharArray, i );
		int iItemReinforce = kItem.m_item_reinforce;
		if ( iItemReinforce <= g_CompoundMaterialMgr.GetFixedFailReinforce() )
		{
			m_ItemTotalLevel[i] = m_ItemLevel[i] + iItemReinforce;
			m_CharTotalLevel[i] = m_CharLevel[i] + iItemReinforce;
		}
		else
		{
			m_ItemTotalLevel[i] = m_ItemLevel[i] + g_GrowthInfoMgr.GetReinforceAddStat( iItemReinforce );
			m_CharTotalLevel[i] = m_CharLevel[i] + g_GrowthInfoMgr.GetReinforceAddStat( iItemReinforce );
		}

		MedalSyncInfoVec vMedalItemTypeVec;
		g_MyInfo.GetMedalItemTypeVec( vMedalItemTypeVec, m_iClassType );
		int iMedalSize = vMedalItemTypeVec.size();
		for (int j = 0; j < iMedalSize; j++)
		{
			if( vMedalItemTypeVec[i].m_iCustomIndex == 0 )
			{
				m_ItemTotalLevel[i] += g_MedalItemMgr.GetMedalItemGrowth( vMedalItemTypeVec[j].m_iItemType, false, i );	
				m_CharTotalLevel[i] += g_MedalItemMgr.GetMedalItemGrowth( vMedalItemTypeVec[j].m_iItemType, true, i );	
			}
			else
			{
				m_ItemTotalLevel[i] += vMedalItemTypeVec[j].m_iStat[i + MAX_CHAR_GROWTH];
				m_CharTotalLevel[i] += vMedalItemTypeVec[j].m_iStat[i];
			}
		}

		PetSlot sPetSlot;
		if( pUserPet )
			pUserPet->GetPetSlotEquip( sPetSlot );

		if( sPetSlot.m_nPetCode > 0 )
		{
			m_ItemTotalLevel[i] += g_PetInfoMgr.GetTotalStat( sPetSlot.m_nPetCode, i + MAX_ITEM_GROWTH, sPetSlot.m_nCurLevel );
			m_CharTotalLevel[i] += g_PetInfoMgr.GetTotalStat( sPetSlot.m_nPetCode, i, sPetSlot.m_nCurLevel );
		}

		//각성
		m_ItemTotalLevel[i] += g_AwakeMgr.GetAwakeItemGrowth( static_cast<AwakeType>( g_MyInfo.GetAwakeType( m_iClassType ) ), i );
		m_CharTotalLevel[i] += g_AwakeMgr.GetAwakeCharGrowth( static_cast<AwakeType>( g_MyInfo.GetAwakeType( m_iClassType ) ), i );
		
		const CHARACTER& rkCharInfo = g_MyInfo.GetCharacterToClassType( m_iClassType );
		if( g_RSoldierInfoManager.IsRSoldierType( rkCharInfo ) )
		{
			m_ItemTotalLevel[i] += g_RSoldierInfoManager.GetSoldierAddStat( i + MAX_ITEM_GROWTH, g_MyInfo.GetRSoldierPossessionCount() );
			m_CharTotalLevel[i] += g_RSoldierInfoManager.GetSoldierAddStat( i, g_MyInfo.GetRSoldierPossessionCount() );
		}

		if( g_RSoldierInfoManager.IsSoldierSet( rkCharInfo ) )
		{
			m_ItemTotalLevel[i] += g_RSoldierInfoManager.GetSoldierSetAddStat( i + MAX_ITEM_GROWTH, rkCharInfo.m_class_type, g_MyInfo.GetSoldierSetCnt( g_RSoldierInfoManager.GetSoldierSetNum( rkCharInfo.m_class_type ) ) );
			m_CharTotalLevel[i] += g_RSoldierInfoManager.GetSoldierSetAddStat( i, rkCharInfo.m_class_type, g_MyInfo.GetSoldierSetCnt( g_RSoldierInfoManager.GetSoldierSetNum( rkCharInfo.m_class_type ) ) );
		}
	}

	m_iNeedPeso = 0;
	m_iUsePoint = 0;
	m_iCurPoint = pLevel->GetCharGrowthPoint( m_iClassType );
	if( g_MyInfo.IsCharExerciseStyle( iCharArray, EXERCISE_RENTAL ) )
		m_iCurPoint = 0;

	ResetBtns();
}

void ioGrowthSelectWnd::BeforeRenderDesc()
{
	char szText[MAX_PATH]="";

	DWORD dwColor = TCT_DEFAULT_RED;
	bool bHired = false;
	int iCharArray = g_MyInfo.GetClassArray( m_iClassType );

	//용병진화관련
	ioHashString szPowerupName;
	const ioClassExpert::CLASSSLOT* pClassSlot = g_MyInfo.GetClassExpertSlot( m_iClassType );
	if ( pClassSlot && pClassSlot->m_ePowerUpCharGrade != PUGT_NONE )
		szPowerupName = g_PowerUpManager.GetCharGradeTitle( m_iClassType, pClassSlot->m_ePowerUpCharGrade );

	if( g_MyInfo.IsCharExerciseStyle( iCharArray, EXERCISE_RENTAL ) )
	{
		SafeSprintf( szText, sizeof( szText ), m_szBeforeRenderText[7].c_str() );
	}
	else if( g_MyInfo.IsCharMortmain( m_iClassType ) )
	{		
		if( szPowerupName.IsEmpty() )
			SafeSprintf( szText, sizeof( szText ), m_szBeforeRenderText[0].c_str() );
		else
			SafeSprintf( szText, sizeof( szText ), m_szBeforeRenderText[8].c_str(), szPowerupName.c_str() );

		dwColor = TCT_DEFAULT_BLUE;
	}
	else if( g_MyInfo.IsCharExerciseStyle( iCharArray, EXERCISE_GENERAL ) )
	{
		if( szPowerupName.IsEmpty() )		
			SafeSprintf( szText, sizeof( szText ), m_szBeforeRenderText[1].c_str() );		
		else
			SafeSprintf( szText, sizeof( szText ), "%s(%s)", szPowerupName.c_str(), m_szBeforeRenderText[1].c_str() );
	}
	else if( g_MyInfo.IsCharExerciseStyle( iCharArray, EXERCISE_PCROOM ) && !g_MyInfo.IsUserEvent() )
	{
			SafeSprintf( szText, sizeof( szText ), m_szBeforeRenderText[2].c_str() );
	}
	else if( g_MyInfo.IsCharExerciseStyle( iCharArray, EXERCISE_PCROOM ) && g_MyInfo.IsUserEvent() )
	{
		if( szPowerupName.IsEmpty() )
			SafeSprintf( szText, sizeof( szText ), m_szBeforeRenderText[9].c_str() );
		else
			SafeSprintf( szText, sizeof( szText ), "%s(%s)", szPowerupName.c_str(), m_szBeforeRenderText[9].c_str() );
	}
	else if( g_MyInfo.IsCharExerciseStyle( iCharArray, EXERCISE_EVENT ) )
	{
		if( szPowerupName.IsEmpty() )
			SafeSprintf( szText, sizeof( szText ), m_szBeforeRenderText[3].c_str() );
		else
			SafeSprintf( szText, sizeof( szText ), "%s(%s)", szPowerupName.c_str(), m_szBeforeRenderText[3].c_str() );
	}
	else
	{
		if( g_MyInfo.GetCharTotalSecond( iCharArray ) <= 0 )
			dwColor = TCT_DEFAULT_GRAY;

		int iTotalLimitMinute = g_MyInfo.GetCharTotalSecond( iCharArray ) / 60;
		int iHour     = iTotalLimitMinute / 60;      
		int iMinute   = iTotalLimitMinute % 60;

		if( iHour == 0 )
		{
			SafeSprintf( szText, sizeof( szText ), m_szBeforeRenderText[4].c_str(), iMinute );
			if( !szPowerupName.IsEmpty() )
				SafeSprintf( szText, sizeof( szText ), "%s(%s)", szPowerupName.c_str(), szText );	
		}
		else if( iMinute == 0 )
		{
			SafeSprintf( szText, sizeof( szText ), m_szBeforeRenderText[5].c_str(), iHour );
			if( !szPowerupName.IsEmpty() )
				SafeSprintf( szText, sizeof( szText ), "%s(%s)", szPowerupName.c_str(), szText );
		}
		else
		{
			SafeSprintf( szText, sizeof( szText ), m_szBeforeRenderText[6].c_str(), iHour, iMinute );
			if( !szPowerupName.IsEmpty() )
				SafeSprintf( szText, sizeof( szText ), "%s(%s)", szPowerupName.c_str(), szText );
		}
	}

	m_ClassInfo[1].ClearList();
	m_ClassInfo[1].SetTextStyle( TS_NORMAL );
	m_ClassInfo[1].SetTextColor( dwColor );
	m_ClassInfo[1].AddTextPiece( FONT_SIZE_13, szText );
}

void ioGrowthSelectWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();


	// 2020-08-07
	if( m_pInnerFrm )
	{
		m_pInnerFrm->Render( iXPos, iYPos );
	}

	BeforeRenderDesc();

	m_ClassInfo[0].PrintFullText( iXPos+97, iYPos+145, TAT_CENTER );
	m_ClassInfo[1].PrintFullText( iXPos+97, iYPos+165, TAT_CENTER );

	char szNeedPeso[MAX_PATH];
	char szPeso[MAX_PATH], szBuf[MAX_PATH];

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.PrintText( iXPos+275, iYPos+64, FONT_SIZE_16, m_szPointPesoText[0].c_str() );

	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.PrintText( iXPos+190, iYPos+89, FONT_SIZE_13, m_szPointPesoText[1].c_str() );
	g_FontMgr.PrintText( iXPos+190, iYPos+109, FONT_SIZE_13, m_szPointPesoText[2].c_str() );
	g_FontMgr.PrintText( iXPos+190, iYPos+131, FONT_SIZE_13, m_szPointPesoText[3].c_str() );
	g_FontMgr.PrintText( iXPos+190, iYPos+151, FONT_SIZE_13, m_szPointPesoText[4].c_str() );
		
	g_FontMgr.SetAlignType( TAT_RIGHT );
	g_FontMgr.SetTextColor( TCT_DEFAULT_RED );	

	wsprintf_e( szBuf, "%d", m_iCurPoint );
	g_FontMgr.PrintText( iXPos+345, iYPos+89, FONT_SIZE_13, szBuf );

	wsprintf_e( szBuf, "%d", m_iUsePoint );
	g_FontMgr.PrintText( iXPos+345, iYPos+109, FONT_SIZE_13, szBuf );

	if( m_pPointImg )
	{
		m_pPointImg->Render( iXPos+348, iYPos+89 );
		m_pPointImg->Render( iXPos+348, iYPos+109 );
	}
	
	g_FontMgr.SetAlignType( TAT_RIGHT );
	g_FontMgr.SetTextColor( TCT_DEFAULT_BLUE );

	Help::ConvertNumToStrComma( g_MyInfo.GetMoney(), szPeso, sizeof( szPeso ) );
	g_FontMgr.PrintText( iXPos+360, iYPos+131, FONT_SIZE_13, STR(3), szPeso );

	Help::ConvertNumToStrComma( m_iNeedPeso, szNeedPeso, sizeof( szNeedPeso ) );
	g_FontMgr.PrintText( iXPos+360, iYPos+151, FONT_SIZE_12, STR(3), szNeedPeso );

	if( m_pBoxLeft && m_pBoxTop )
	{
		m_pBoxLeft->SetReverseFormatting( ioUIRenderImage::RF_NONE_REVERSE );
		m_pBoxLeft->Render( iXPos+34, iYPos+207 );
		m_pBoxLeft->Render( iXPos+184, iYPos+207 );

		m_pBoxTop->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
		m_pBoxTop->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
		m_pBoxTop->SetSize( 30, 10 );			
		m_pBoxTop->Render( iXPos+41, iYPos+207 );
		m_pBoxTop->Render( iXPos+135, iYPos+207 );
		m_pBoxTop->Render( iXPos+191, iYPos+207 );
		m_pBoxTop->Render( iXPos+329, iYPos+207 );

		m_pBoxLeft->SetReverseFormatting( ioUIRenderImage::RF_HORZ_REVERSE );
		m_pBoxLeft->Render( iXPos+165, iYPos+207 );
		m_pBoxLeft->Render( iXPos+359, iYPos+207 );
	}
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextColor( 167, 167, 167 );
	g_FontMgr.PrintText( iXPos+99, iYPos+203, FONT_SIZE_11,  m_szCurGrowthText.c_str() );
	g_FontMgr.PrintText( iXPos+272, iYPos+203, FONT_SIZE_11,  m_szGrowthText.c_str() );

	enum
	{
		STRIP_X		= 34,
		STRIP_X2	= 184,
		STRIP_Y		= 227,
		STRIP_GAP	= 21,
	};

	if( m_pDarkStrip && m_pLightStrip )
	{
		m_pDarkStrip->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
		m_pDarkStrip->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
		m_pLightStrip->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
		m_pLightStrip->SetVertFormatting( ioUIRenderImage::VF_STRETCH );

		m_pDarkStrip->SetSize( 138, 21 );
		m_pLightStrip->SetSize( 138, 21 );
		for( int i=0; i < 4; ++i )
		{
			m_pDarkStrip->Render( iXPos+STRIP_X, iYPos+STRIP_Y+(STRIP_GAP*i*2), UI_RENDER_MULTIPLY );
			m_pLightStrip->Render( iXPos+STRIP_X, iYPos+STRIP_Y+(STRIP_GAP*(i*2+1)), UI_RENDER_MULTIPLY );
		}

		m_pDarkStrip->SetSize( 182, 21 );
		m_pLightStrip->SetSize( 182, 21 );
		for( int i=0; i < 4; ++i )
		{
			m_pDarkStrip->Render( iXPos+STRIP_X2, iYPos+STRIP_Y+(STRIP_GAP*i*2), UI_RENDER_MULTIPLY );
			m_pLightStrip->Render( iXPos+STRIP_X2, iYPos+STRIP_Y+(STRIP_GAP*(i*2+1)), UI_RENDER_MULTIPLY );
		}

		g_FontMgr.SetAlignType( TAT_LEFT );
		for( int i=0; i < 4; ++i )
		{
			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );

			m_pSkillIcon[i]->Render( iXPos+STRIP_X+13, iYPos+STRIP_Y+(STRIP_GAP*i)+11, UI_RENDER_NORMAL, TFO_BILINEAR );
			m_pCharIcon[i]->Render( iXPos+STRIP_X+13, iYPos+STRIP_Y+(STRIP_GAP*(i+4))+11, UI_RENDER_NORMAL, TFO_BILINEAR );
			
			if( m_EquipItemGrowthText[i].IsEmpty() )
				g_FontMgr.PrintText( iXPos+STRIP_X+24, iYPos+STRIP_Y+(STRIP_GAP*i)+4, FONT_SIZE_12,  m_ItemGrowthText[i].c_str() );
			else
				g_FontMgr.PrintText( iXPos+STRIP_X+24, iYPos+STRIP_Y+(STRIP_GAP*i)+4, FONT_SIZE_12,  m_EquipItemGrowthText[i].c_str() );

			char szBuff[MAX_PATH]="";
			wsprintf_e( szBuff, "%03d", m_ItemTotalLevel[i] );
			g_FontMgr.PrintText( iXPos+STRIP_X+114, iYPos+STRIP_Y+(STRIP_GAP*i)+4, FONT_SIZE_12, szBuff );
			
			g_FontMgr.PrintText( iXPos+STRIP_X+24, iYPos+STRIP_Y+(STRIP_GAP*(i+4))+4, FONT_SIZE_12,  m_CharGrowthText[i].c_str() );
			wsprintf_e( szBuff, "%03d", m_CharTotalLevel[i] );
			g_FontMgr.PrintText( iXPos+STRIP_X+114, iYPos+STRIP_Y+(STRIP_GAP*(i+4))+4, FONT_SIZE_12,  szBuff );
			
			if( m_iItemLevelUp[i] > 0 )
				g_FontMgr.SetTextColor( TCT_DEFAULT_GREEN );
			else
				g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );

			wsprintf_e( szBuff, "%03d", m_iItemLevelUp[i] );
			g_FontMgr.PrintText( iXPos+STRIP_X+155, iYPos+STRIP_Y+(STRIP_GAP*i)+4, FONT_SIZE_12, szBuff );

			if( m_iCharLevelUp[i] > 0 )
				g_FontMgr.SetTextColor( TCT_DEFAULT_GREEN );
			else
				g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );

			wsprintf_e( szBuff, "%03d", m_iCharLevelUp[i] );
			g_FontMgr.PrintText( iXPos+STRIP_X+155, iYPos+STRIP_Y+(STRIP_GAP*(i+4))+4, FONT_SIZE_12, szBuff );
		
			if( m_pGaugeBack )
			{
				m_pGaugeBack->Render( iXPos+STRIP_X+181, iYPos+STRIP_Y+(STRIP_GAP*(i+4))+4 );
				m_pGaugeBack->Render( iXPos+STRIP_X+181, iYPos+STRIP_Y+(STRIP_GAP*i)+4 );

				if( m_iMaxLevel == 0 ) 
					continue;

				if( m_iItemLevelUp[i] > 0 && m_pGaugeGreen )
					m_pGaugeGreen->RenderWidthCut( iXPos+STRIP_X+181, iYPos+STRIP_Y+(STRIP_GAP*i)+4, 0, m_pGaugeGreen->GetWidth() * (m_iItemLevelUp[i] + m_ItemLevel[i] ) / m_iMaxLevel );
				else if( m_pGaugeGray )
					m_pGaugeGray->RenderWidthCut( iXPos+STRIP_X+181, iYPos+STRIP_Y+(STRIP_GAP*i)+4, 0, m_pGaugeGray->GetWidth() * m_ItemLevel[i] / m_iMaxLevel );

				if( m_iCharLevelUp[i] > 0 && m_pGaugeGreen )
					m_pGaugeGreen->RenderWidthCut( iXPos+STRIP_X+181, iYPos+STRIP_Y+(STRIP_GAP*(i+4))+4, 0, m_pGaugeGreen->GetWidth() * (m_iCharLevelUp[i] + m_CharLevel[i] ) / m_iMaxLevel );
				else if( m_pGaugeGray )
					m_pGaugeGray->RenderWidthCut( iXPos+STRIP_X+181, iYPos+STRIP_Y+(STRIP_GAP*(i+4))+4, 0, m_pGaugeGray->GetWidth() * m_CharLevel[i] / m_iMaxLevel );
			}
		}
	}
}

void ioGrowthSelectWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_X:
		if( cmd == IOBN_BTNUP )
			HideWnd();
		break;

	case ID_CONFIRM:
		if( cmd == IOBN_BTNUP )
		{
			if( m_iNeedPeso <= g_MyInfo.GetMoney() )
				g_GUIMgr.SetMsgBox( MB_YESNO, this, m_szConfirmText.c_str() );
			else
				g_GUIMgr.SetMsgBox( MB_OK, NULL, m_szNoMoneyText.c_str() );
		}
		break;

	case ID_ITEM_UP1:
	case ID_ITEM_UP2:
	case ID_ITEM_UP3:
	case ID_ITEM_UP4:
		if( cmd == IOBN_BTNUP )
		{
			int iNeedPoint = g_GrowthInfoMgr.GetGrowthUpNeedPoint( false );
			if( m_iCurPoint < iNeedPoint )
				return;

			int iIndex = dwID - ID_ITEM_UP1;
			if( m_ItemLevel[iIndex] + m_iItemLevelUp[iIndex] >= m_iMaxLevel )
				return;

			m_iItemLevelUp[iIndex]++;
			m_iCurPoint -= iNeedPoint;
			m_iUsePoint += iNeedPoint;
			m_iNeedPeso += g_GrowthInfoMgr.GetGrowthUpNeedPeso( false, m_ItemLevel[iIndex] + m_iItemLevelUp[iIndex] );
			ResetBtns();
		}
		break;

	case ID_CHAR_UP1:
	case ID_CHAR_UP2:
	case ID_CHAR_UP3:
	case ID_CHAR_UP4:
		if( cmd == IOBN_BTNUP )
		{
			int iNeedPoint = g_GrowthInfoMgr.GetGrowthUpNeedPoint( true );
			if( m_iCurPoint < iNeedPoint )
				return;

			int iIndex = dwID - ID_CHAR_UP1;
			if( m_CharLevel[iIndex] + m_iCharLevelUp[iIndex] >= m_iMaxLevel )
				return;

			m_iCharLevelUp[iIndex]++;
			m_iCurPoint -= iNeedPoint;
			m_iUsePoint += iNeedPoint;
			m_iNeedPeso += g_GrowthInfoMgr.GetGrowthUpNeedPeso( true, m_CharLevel[iIndex] + m_iCharLevelUp[iIndex] );
			ResetBtns();
		}
		break;

	case ID_ITEM_DOWN1:
	case ID_ITEM_DOWN2:
	case ID_ITEM_DOWN3:
	case ID_ITEM_DOWN4:
		if( cmd == IOBN_BTNUP )
		{
			int iIndex = dwID - ID_ITEM_DOWN1;
			if( m_iItemLevelUp[iIndex] == 0 )
				return;

			int iNeedPoint = g_GrowthInfoMgr.GetGrowthUpNeedPoint( false );
			m_iNeedPeso -= g_GrowthInfoMgr.GetGrowthUpNeedPeso( false, m_ItemLevel[iIndex] + m_iItemLevelUp[iIndex] );
			m_iItemLevelUp[iIndex]--;
			m_iCurPoint += iNeedPoint;
			m_iUsePoint -= iNeedPoint;
			ResetBtns();
		}
		break;

	case ID_CHAR_DOWN1:
	case ID_CHAR_DOWN2:
	case ID_CHAR_DOWN3:
	case ID_CHAR_DOWN4:
		if( cmd == IOBN_BTNUP )
		{
			int iIndex = dwID - ID_CHAR_DOWN1;
			if( m_iCharLevelUp[iIndex] == 0 )
				return;

			int iNeedPoint = g_GrowthInfoMgr.GetGrowthUpNeedPoint( true );
			m_iNeedPeso -= g_GrowthInfoMgr.GetGrowthUpNeedPeso( true, m_CharLevel[iIndex] + m_iCharLevelUp[iIndex] );
			m_iCharLevelUp[iIndex]--;
			m_iCurPoint += iNeedPoint;
			m_iUsePoint -= iNeedPoint;
			ResetBtns();
		}
		break;

	case MESSAGE_BOX:
		if( cmd == IOBN_BTNUP )
		{
			if( param == IDYES )
				Confirm();		
		}
		break;
	}
}

void ioGrowthSelectWnd::ResetBtns()
{
	for( int i = 0; i < CUR_GROWTH_CNT; i++ )
	{
		int iItemNeedPoint = g_GrowthInfoMgr.GetGrowthUpNeedPoint( false );
		if( m_iCurPoint < iItemNeedPoint || m_iItemLevelUp[i] + m_ItemLevel[i] >= m_iMaxLevel )
			SetChildInActive( ID_ITEM_UP1 + i );
		else
			SetChildActive( ID_ITEM_UP1 + i );

		if( m_iItemLevelUp[i] == 0 )
			SetChildInActive( ID_ITEM_DOWN1 + i );
		else
			SetChildActive( ID_ITEM_DOWN1 + i );

		int iCharNeedPoint = g_GrowthInfoMgr.GetGrowthUpNeedPoint( true );
		if( m_iCurPoint < iCharNeedPoint || m_iCharLevelUp[i] + m_CharLevel[i] >= m_iMaxLevel )
			SetChildInActive( ID_CHAR_UP1 + i );
		else
			SetChildActive( ID_CHAR_UP1 + i );

		if( m_iCharLevelUp[i] == 0 )
			SetChildInActive( ID_CHAR_DOWN1 + i );
		else
			SetChildActive( ID_CHAR_DOWN1 + i );
	}
	if( m_iUsePoint == 0 )
		SetChildInActive( ID_CONFIRM );
	else
		SetChildActive( ID_CONFIRM );
}

void ioGrowthSelectWnd::Confirm()
{
	SP2Packet kPacket( CTPK_LEVEL_GROWTH );
	kPacket << m_iClassType;
	for( int i = 0; i < CUR_GROWTH_CNT; i++ )
		kPacket << m_iCharLevelUp[i];
	for( int i = 0; i < CUR_GROWTH_CNT; i++ )
		kPacket << m_iItemLevelUp[i];

	TCPNetwork::SendToServer( kPacket );

	g_App.SetMouseBusy( true );
}