#include "StdAfx.h"

#include "../ioDateHelp.h"
#include "../ioSubscriptionMgr.h"
#include "../ioSoldierAwakeManager.h"
#include "../ioPowerUpManager.h"
#include "../ioExtendSoundManager.h"
#include "../ioAccessoryInfoManager.h"
#include "../ioUserSpiritManager.h"
#include "../ioUserMedalItem.h"

#include "ItemResellWnd.h"
#include "ioPullDownList.h"
#include "PetWnd.h"
#include "MyInventoryInfoWnd.h"
#include "UserInfoToolTip.h"
#include "UISoldierSelectWnd.h"
#include "ExpandMedalSlotWnd.h"
#include "EnablePowerupListWnd.h"
#include "ReinforceSelectWnd.h"
#include "ioNewMedalRandomBox.h"
#include "GradeAdjustmentWnd.h"
#include "ExtraItemFuncWnd.h"	// 2019-04-30
//////////////////////////////////////////////////////////////////////////
SlotIconBtn::SlotIconBtn()
{
	m_pStatePinkFrm= NULL;

	m_pIcon        = NULL;
	m_pSubIcon = NULL;
	m_pIconBack    = NULL;
	m_pIconDisable = NULL;
	m_pIconLock    = NULL;
	m_pIconLockOver= NULL;
	m_pIconEffect  = NULL;
	m_pCustomIcon  = NULL;
	m_pCostumeMark = NULL;

	m_pNumText = NULL;
	m_pPlusText = NULL;

	m_iNum          = 0;
	m_fScale        = FLOAT1;
	m_fBackScale    = FLOAT1;
	m_fDisableScale = FLOAT1;

	m_iPeriodType   = 0;
	m_dwMaleCustom  = 0;
	m_dwFemaleCustom= 0;

	m_iMagicCode = 0;
	m_iMagicCodeEx = 0;
	m_iMagicCodeEx2= 0;

	m_bLock = false;
	m_bIsCostume = false;
	m_bRenderGray = false;
	m_nGradeType = 0;
}

SlotIconBtn::~SlotIconBtn()
{
	SAFEDELETE( m_pStatePinkFrm );

	SAFEDELETE( m_pIcon );
	SAFEDELETE( m_pSubIcon );
	SAFEDELETE( m_pIconBack );
	SAFEDELETE( m_pIconDisable );
	SAFEDELETE( m_pIconLock );
	SAFEDELETE( m_pIconLockOver );
	SAFEDELETE( m_pIconEffect );
	SAFEDELETE( m_pCustomIcon );
	SAFEDELETE( m_pCostumeMark );

	SAFEDELETE( m_pNumText );
	SAFEDELETE( m_pPlusText );
}

void SlotIconBtn::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "StatePinkFrm" )
	{
		SAFEDELETE( m_pStatePinkFrm );
		m_pStatePinkFrm = pFrame;
	}
	else
	{
		ioButton::AddRenderFrame( szType, pFrame );	
	}		
}

void SlotIconBtn::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "IconBack" )
	{
		SAFEDELETE( m_pIconBack );
		m_pIconBack = pImage;
	}
	else if( szType == "IconDisable" )
	{
		SAFEDELETE( m_pIconDisable );
		m_pIconDisable = pImage;
	}
	else if( szType == "IconLock" )
	{
		SAFEDELETE( m_pIconLock );
		m_pIconLock = pImage;
	}
	else if( szType == "IconLockOver" )
	{
		SAFEDELETE( m_pIconLockOver );
		m_pIconLockOver = pImage;
	}
	else if( szType == "IconEffect" )
	{
		SAFEDELETE( m_pIconEffect );
		m_pIconEffect = pImage;
	}
	else if( szType == "custom_image" )
	{
		SAFEDELETE( m_pCustomIcon );
		m_pCustomIcon = pImage;
	}
	else if( szType == "costume_image" )
	{
		SAFEDELETE( m_pCostumeMark );
		m_pCostumeMark = pImage;
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
	else
	{
		ioButton::AddRenderImage( szType, pImage );	
	}	
}

void SlotIconBtn::OnDrawNormal( int iXPos, int iYPos )
{
	ioButton::OnDrawNormal( iXPos, iYPos );

	RenderIconEffect( iXPos, iYPos );
	RenderBack( iXPos, iYPos );
	RenderIcon( iXPos, iYPos , UI_RENDER_NORMAL );
	RenderNumInfo( iXPos, iYPos );
}

void SlotIconBtn::OnDrawOvered( int iXPos, int iYPos )
{
	ioButton::OnDrawOvered( iXPos, iYPos );

	RenderIconEffect( iXPos, iYPos );
	RenderBack( iXPos, iYPos );
	RenderIcon( iXPos, iYPos , UI_RENDER_NORMAL );
	RenderIcon( iXPos, iYPos , UI_RENDER_SCREEN );
	RenderNumInfo( iXPos, iYPos );
}

void SlotIconBtn::OnDrawPushed( int iXPos, int iYPos )
{
	ioButton::OnDrawPushed( iXPos, iYPos );

	RenderIconEffect( iXPos, iYPos );
	RenderBack( iXPos, iYPos );
	RenderIcon( iXPos, iYPos , UI_RENDER_NORMAL );
	RenderIcon( iXPos, iYPos , UI_RENDER_SCREEN );
	RenderNumInfo( iXPos, iYPos );
}

void SlotIconBtn::OnDrawDisabled( int iXPos, int iYPos )
{
	ioButton::OnDrawDisabled( iXPos, iYPos );

	if( m_pIcon )
	{
		RenderIconEffect( iXPos, iYPos );
		RenderBack( iXPos, iYPos );
		RenderIcon( iXPos, iYPos , UI_RENDER_GRAY );
		RenderNumInfo( iXPos, iYPos );
	}
	else
		RenderIconDisable( iXPos, iYPos );
}

void SlotIconBtn::RenderBack( int iXPos, int iYPos )
{
	int iNewXPos = iXPos + GetWidth() / 2;
	int iNewYPos = iYPos + GetHeight() / 2;

	// Icon
	if( IsLockIcon() && m_pIconLock )
	{
		m_pIconLock->SetScale( m_fBackScale );
		m_pIconLockOver->SetScale( m_fBackScale );
		int iWidth = m_pIconLock->GetWidth();
		int iHeight = m_pIconLock->GetHeight();

		if( m_fBackScale != FLOAT1 )
		{
			m_pIconLock->Render( iNewXPos - iWidth/2, iNewYPos - iHeight/2, UI_RENDER_NORMAL, TFO_BILINEAR );
			if( IsOvered() && m_pIconLockOver )
				m_pIconLockOver->Render( iNewXPos - m_pIconLockOver->GetWidth()/2,
										 iNewYPos - m_pIconLockOver->GetHeight()/2,
										 UI_RENDER_NORMAL, TFO_BILINEAR );
		}
		else
		{
			m_pIconLock->Render( iNewXPos - iWidth/2, iNewYPos - iHeight/2, UI_RENDER_NORMAL );
			if( IsOvered() && m_pIconLockOver )
				m_pIconLockOver->Render( iNewXPos - m_pIconLockOver->GetWidth()/2,
										 iNewYPos - m_pIconLockOver->GetHeight()/2,
										 UI_RENDER_NORMAL );
		}
	}
	else
	{
		if( m_pIconBack )
		{
			m_pIconBack->SetScale( m_fBackScale );

			if( m_fBackScale != FLOAT1 )
				m_pIconBack->Render( iNewXPos, iNewYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
			else
				m_pIconBack->Render( iNewXPos, iNewYPos, UI_RENDER_NORMAL );
		}
	}
}

void SlotIconBtn::RenderIconDisable( int iXPos, int iYPos )
{
	int iNewXPos = iXPos + GetWidth() / 2;
	int iNewYPos = iYPos + GetHeight() / 2;

	if( m_pIconDisable )
	{
		m_pIconDisable->SetScale( m_fDisableScale );

		if( m_fDisableScale != FLOAT1 )
			m_pIconDisable->Render( iNewXPos, iNewYPos, UI_RENDER_MULTIPLY, TFO_BILINEAR );
		else
			m_pIconDisable->Render( iNewXPos, iNewYPos, UI_RENDER_MULTIPLY );
	}
}

void SlotIconBtn::RenderIconEffect( int iXPos, int iYPos )
{
	if( !m_pIcon )
		return;

	int iNewXPos = iXPos + GetWidth() / 2;
	int iNewYPos = iYPos + GetHeight() / 2;

	// Icon
	if( m_pIconEffect )
	{
		m_pIconEffect->SetScale( m_fScale );

		if( m_fScale != FLOAT1 )
			m_pIconEffect->Render( iNewXPos, iNewYPos, UI_RENDER_MULTIPLY, TFO_BILINEAR );
		else
			m_pIconEffect->Render( iNewXPos, iNewYPos, UI_RENDER_MULTIPLY );
	}
}

void SlotIconBtn::RenderIcon( int iXPos, int iYPos, UIRenderType eType )
{
	enum 
	{
		X_TEXT_OFFSET   = 20,
		Y_TEXT_OFFSET   = 2,
	};

	int iNewXPos = iXPos + GetWidth() / 2;
	int iNewYPos = iYPos + GetHeight() / 2;

	if ( eType != UI_RENDER_SCREEN )
		g_UIImageRenderMgr.RenderGradeIconBack( m_nGradeType, iNewXPos, iNewYPos, m_fScale, eType );

	if( m_pIcon )
	{
		m_pIcon->SetScale( m_fScale );

		if( m_bRenderGray )
			eType = UI_RENDER_GRAY;

		if( m_fScale != FLOAT1 )
			m_pIcon->Render( iNewXPos, iNewYPos, eType, TFO_BILINEAR );
		else
			m_pIcon->Render( iNewXPos, iNewYPos, eType );

		if( m_pSubIcon )
		{
			m_pSubIcon->SetScale( m_fScale );
			m_pSubIcon->Render( iNewXPos, iNewYPos, eType, TFO_BILINEAR );
		}
	}

	if( m_dwMaleCustom > 0 || m_dwFemaleCustom > 0 )
	{
		if( m_pCustomIcon )
		{
			iNewXPos = iNewXPos + ( 7.0f * m_fScale );
			iNewYPos = iNewYPos - ( 43.0f * m_fScale );
			m_pCustomIcon->SetScale( m_fScale );
			if( m_fScale != FLOAT1 )
				m_pCustomIcon->Render( iNewXPos, iNewYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
			else
				m_pCustomIcon->Render( iNewXPos, iNewYPos );
		}
	}

	if( m_iPeriodType == EPT_GROW_TIME )
	{
		if( m_pStatePinkFrm )
			m_pStatePinkFrm->Render( iNewXPos - 36, iNewYPos - 36 );

		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
		g_FontMgr.PrintText( iNewXPos - 11, iNewYPos - 35, FONT_SIZE_11, STR(1), m_iNum );
	}

	// UI코스튬 관련 (코스튬 마크)
	if( m_bIsCostume && m_pCostumeMark )
	{
		m_pCostumeMark->SetScale( m_fScale );
		if( m_fScale != FLOAT1 )
			m_pCostumeMark->Render( iXPos + GetWidth()/2, iYPos + GetHeight()/2, eType, TFO_BILINEAR );
		else
			m_pCostumeMark->Render( iXPos + GetWidth()/2, iYPos + GetHeight()/2, eType );
	}
}

void SlotIconBtn::RenderNumInfo( int iXPos, int iYPos )
{
	if( !m_pNumText )
		return;
	if( !m_pPlusText )
		return;
	if( m_iNum <= 0 )
		return;

	int iNewXPos = iXPos + GetWidth() / 2;
	int iNewYPos = iYPos + GetHeight() / 2;

	// Num
	enum
	{
		REINFORCE_Y	= 14,

		NUM_SIZE	= 20,
		PLUS_SIZE	= 14,

		NUM_GAP		= 4,
	};
	
	int iCurPos = 0;
	int iDefaultPos = iNewXPos;
	int iCurYPos = iNewYPos+(REINFORCE_Y * m_fScale);
	int iTotalSize = 0;
	if( m_iNum < 10 )
		iTotalSize = PLUS_SIZE + NUM_SIZE - NUM_GAP;
	else if( m_iNum < 100 )
		iTotalSize = PLUS_SIZE + NUM_SIZE*2 - NUM_GAP*2;
	else
		iTotalSize = PLUS_SIZE + NUM_SIZE*3 - NUM_GAP*3;

	iCurPos = iDefaultPos - (iTotalSize / 2);
	
	// for user tool tip btn
	if( m_fScale == USERINFO_TOOLTIP_GROWTH_SCALE )
	{
		if( m_iNum < 10 )
			iCurPos += 2;
		else if( m_iNum < 100 )
			iCurPos += 5;
		else
			iCurPos += 8;
	}

	m_pPlusText->Render( iCurPos, iCurYPos );

	iCurPos += (PLUS_SIZE-NUM_GAP);

	m_pNumText->RenderNum( iCurPos, iCurYPos, m_iNum, -NUM_GAP );
}

bool SlotIconBtn::SetIcon( ioUIRenderImage *pIcon, float fScale, float fBackScale, float fDisableScale, int iPeriodType /* = 0 */, int iNum/* =0 */, DWORD dwMaleCustom /* = 0 */, DWORD dwFemaleCustom /* = 0  */, bool bIsCostume )
{
	m_iNum          = iNum;
	m_iPeriodType   = iPeriodType;
	m_fScale        = fScale;
	m_fBackScale    = fBackScale;
	m_fDisableScale = fDisableScale;
	m_dwMaleCustom  = dwMaleCustom;
	m_dwFemaleCustom= dwFemaleCustom;
	m_bIsCostume = bIsCostume;
	m_bRenderGray = false;
	m_nGradeType = 0;
	SAFEDELETE( m_pIcon );
	SAFEDELETE( m_pSubIcon );
	if( !pIcon )
	{
		m_iNum = 0;
		return false;
	}

	m_pIcon = pIcon;
	return true;
}

void SlotIconBtn::SetSubIcon( ioUIRenderImage *pIcon )
{
	SAFEDELETE( m_pSubIcon );
	m_pSubIcon = pIcon;
}

void SlotIconBtn::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	if( m_bOver && !m_bClicked )
	{
		ioLSMouse *pMouse = g_App.GetMouse();
		if( pMouse )
			pMouse->SetToolTipWndID( m_dwID );
	}	
}
//////////////////////////////////////////////////////////////////////////
MyInventoryInfoWnd::MyInventoryInfoWnd(void)
{
	m_CharInfo.m_class_type = 1;
	m_CharInfo.m_kindred	= 1;
	m_CharInfo.m_sex		= 1;
	m_CharInfo.m_beard	    = 1;
	m_CharInfo.m_face	    = 1;
	m_CharInfo.m_hair	    = 1;
	m_CharInfo.m_skin_color = 1;
	m_CharInfo.m_hair_color = 1;
	m_CharInfo.m_accessories= 1;
	m_CharInfo.m_underwear  = 1;

	m_pManual = NULL;

	m_pGaugeMainBack = NULL;
	m_pGaugeBack = NULL;
	m_pExpGauge = NULL;
	m_pPowerUpToolTip = NULL;

	m_bShowPowerUpToolTip = false;
}

MyInventoryInfoWnd::~MyInventoryInfoWnd(void)
{
	SAFEDELETE(m_pGaugeMainBack);
	SAFEDELETE(m_pGaugeBack);
	SAFEDELETE(m_pExpGauge);
	SAFEDELETE(m_pPowerUpToolTip);
}

void MyInventoryInfoWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "MainBack" )
	{
		SAFEDELETE( m_pGaugeMainBack );
		m_pGaugeMainBack = pImage;
	}
	else if( szType == "GaugeBack" )
	{
		SAFEDELETE( m_pGaugeBack );
		m_pGaugeBack = pImage;
	}
	else if( szType == "ExpGauge" )
	{
		SAFEDELETE( m_pExpGauge );
		m_pExpGauge = pImage;
	}
	else if( szType == "PowerUpToolTip" )
	{
		SAFEDELETE( m_pPowerUpToolTip );
		m_pPowerUpToolTip = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void MyInventoryInfoWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	ioWnd::ParseExtraInfo( xElement );

	m_dwPowerUpToolTipLeaveColor = ioStringConverter::ParseColor( xElement.GetStringAttribute( "PowerUpToolTipLeaveColor" ) );
	m_dwPowerUpToolTipOverColor = ioStringConverter::ParseColor( xElement.GetStringAttribute( "PowerUpToolTipOverColor" ) );
}

void MyInventoryInfoWnd::SetBlank()
{
	m_pManual = NULL;
	m_ItemInfo.Init();

	m_iClassType = 0;
	m_iMainType = 0;
	m_iResellType = 0;
	m_iPresentSlotIndex = 0;
	m_bSoldierTab = false;

	m_iCurLevel = 0;
	m_iCurLevelExp = 0;
	m_iNextLevelExp = 0;
	m_iExpRate = 0;

	m_dwDefaultActionBtn = 0;

	m_SoldierInfo[0].ClearList();
	m_SoldierInfo[1].ClearList();

	m_PresentInfo[0].ClearList();
	m_PresentInfo[1].ClearList();

	m_SubscriptionInfo[0].ClearList();
	m_SubscriptionInfo[1].ClearList();
	m_SubscriptionInfo[2].ClearList();

	m_EtcInfo.ClearList();

	m_bShowPowerUpToolTip = false;

	for( int i=0; i < 6; ++i )
	{
		m_DecoName[i].Clear();
	}

	HideChildWnd( ID_HIRE_BTN );
	HideChildWnd( ID_DISMISSAL_BTN );
	HideChildWnd( ID_DISASSEMBLE_BTN );

	HideChildWnd( ID_MY_SOLDIER_LIST_BTN );
	HideChildWnd( ID_MY_SOLDIER_LEFT_BTN );
	HideChildWnd( ID_MY_SOLDIER_RIGHT_BTN );
	HideChildWnd( ID_INNER_TAB1 );
	HideChildWnd( ID_INNER_TAB2 );
	HideChildWnd( ID_ETC_SHOP_BTN );
	HideChildWnd( ID_ITEM_SELL_BTN );
	HideChildWnd( ID_ETC_USE_BTN );
	HideChildWnd( ID_ETC_PRESENT_SEND_BTN );
	HideChildWnd( ID_ETC_USE_SMALL_BTN );
	HideChildWnd( ID_ITEM_ICON );
	HideChildWnd( ID_ITEM_RECV );
	HideChildWnd( ID_ITEM_RETR );
	HideChildWnd( ID_PCROOM_SOLDIER_CHANGE_BTN );
	HideChildWnd( ID_FREEDAY_SOLDIER_CHANGE_BTN );
	HideChildWnd( ID_PRESENT_DISASSEMBLE_BTN );

	//용병강화
	HideChildWnd( ID_POWERUP_BTN );
	HideChildWnd( ID_POWERUP_HIRE_BTN );
	HideChildWnd( ID_POWERUP_DISASSEMBLE_BTN );
	HideChildWnd( ID_POWERUP_DISMISSAL_BTN );

	HideChildWnd( ID_KINDRED_BTN );
	HideChildWnd( ID_HAIR_BTN );
	HideChildWnd( ID_HAIR_COLOR_BTN );
	HideChildWnd( ID_FACE_BTN );
	HideChildWnd( ID_SKIN_COLOR_BTN );
	HideChildWnd( ID_UNDERWEAR_BTN );

	HideChildWnd( ID_ETC_SOULSTONE_USE_BTN );
	HideChildWnd( ID_ETC_SOULSTONE_ENABLE_LIST );

	HideChildWnd( ID_SOLDIER_WND );	
	HideChildWnd( ID_COSTUME_SOLDIER_WND );
	
	// UI코스튬 관련 (코스튬 버튼 해제)
	HideChildWnd( ID_WEAPON_COSTUME_BTN );
	HideChildWnd( ID_ARMOR_COSTUME_BTN );
	HideChildWnd( ID_HELMET_COSTUME_BTN );
	HideChildWnd( ID_CLOAK_COSTUME_BTN );
}

void MyInventoryInfoWnd::SetSlodierInfo( int iClassType )
{
	m_iResellType = 0;
	m_iPresentSlotIndex = 0;
	m_iClassType = iClassType;
	m_bSoldierTab = true;

	m_iCurLevel = g_MyInfo.GetClassLevel(m_iClassType,false);
	m_iCurLevelExp = g_MyInfo.GetClassExpert(m_iClassType);
	m_iNextLevelExp = g_LevelMgr.GetNextLevelupExp(m_iCurLevel);
	m_iExpRate = g_MyInfo.GetExpRate(m_iClassType);

	DWORD dwCharIndex = g_MyInfo.GetCharIndexByClassType( m_iClassType );
	if( g_MyInfo.IsCharExerciseStyle( g_MyInfo.GetCharArray( dwCharIndex ), EXERCISE_RENTAL ) )
	{
		if( g_MyInfo.IsCharExerciseRentalInActive( g_MyInfo.GetCharArray( dwCharIndex ) ) == false )
			m_iCurLevelExp = m_iExpRate = 0;
	}

	m_ItemInfo.Init();

	m_pManual = NULL;

	m_SoldierInfo[0].ClearList();
	m_SoldierInfo[1].ClearList();

	m_PresentInfo[0].ClearList();
	m_PresentInfo[1].ClearList();

	m_SubscriptionInfo[0].ClearList();
	m_SubscriptionInfo[1].ClearList();
	m_SubscriptionInfo[2].ClearList();

	m_EtcInfo.ClearList();

	for( int i=0; i < 6; ++i )
	{
		m_DecoName[i].Clear();
	}

	bool bHire = false;
	if( g_MyInfo.GetCharIndexByClassType( iClassType ) > 0 )
		bHire = true;

	ioGrowthLevel *pLevel = g_MyInfo.GetGrowthLevel();
	if( pLevel )
	{
		char szBuf[MAX_PATH] = "";
		wsprintf_e( szBuf, "Lv%d %s", g_MyInfo.GetClassLevel( m_iClassType, false ), g_MyInfo.GetClassName( m_iClassType ) );

		int iHeroGrade = g_MyInfo.GetCharPowerUpGradeByClassType( m_iClassType );

		m_SoldierInfo[0].SetTextStyle( TS_NORMAL );
		m_SoldierInfo[0].SetBkColor( 0, 0, 0 );
		m_SoldierInfo[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
		m_SoldierInfo[0].AddTextPiece( FONT_SIZE_17, STR(1), szBuf );

		char szText[MAX_PATH]="";

		DWORD dwColor = TCT_DEFAULT_RED;
		int iCharArray = g_MyInfo.GetClassArray( m_iClassType );
		if( g_MyInfo.IsCharExerciseStyle( iCharArray, EXERCISE_RENTAL ) )
		{
			SafeSprintf( szText, sizeof( szText ), STR(9) );
		}
		else if( g_MyInfo.IsCharMortmain( m_iClassType ) )
		{
			if ( iHeroGrade == PUGT_NONE )
				SafeSprintf( szText, sizeof( szText ), STR(2) );
			else
			{
				ioHashString szGradeTitle = g_PowerUpManager.GetCharGradeTitle( m_iClassType, iHeroGrade );
				if ( !szGradeTitle.IsEmpty() )
					SafeSprintf( szText, sizeof( szText ), STR(10), szGradeTitle.c_str() );
			}
			dwColor = TCT_DEFAULT_BLUE;
		}
		else if( g_MyInfo.IsCharExerciseStyle( iCharArray, EXERCISE_GENERAL ) )
		{
			SafeSprintf( szText, sizeof( szText ), STR(3) );
		}
		else if( g_MyInfo.IsCharExerciseStyle( iCharArray, EXERCISE_PCROOM ) )
		{
			SafeSprintf( szText, sizeof( szText ), STR(4) );
		}
		else if( g_MyInfo.IsCharExerciseStyle( iCharArray, EXERCISE_EVENT ) )
		{
			SafeSprintf( szText, sizeof( szText ), STR(5) );
		}
		else
		{
			int iTotalLimitMinute = g_MyInfo.GetCharTotalSecond( iCharArray ) / 60;
			int iHour     = iTotalLimitMinute / 60;      
			int iMinute   = iTotalLimitMinute % 60;

			if( iHour == 0 )
				SafeSprintf( szText, sizeof( szText ), STR(6), iMinute );
			else if( iMinute == 0 )
				SafeSprintf( szText, sizeof( szText ), STR(7), iHour );
			else
				SafeSprintf( szText, sizeof( szText ), STR(8), iHour, iMinute );
		}

		m_SoldierInfo[1].ClearList();
		m_SoldierInfo[1].SetTextStyle( TS_NORMAL );
		m_SoldierInfo[1].SetBkColor( 0, 0, 0 );	
		m_SoldierInfo[1].SetTextColor( dwColor );
		m_SoldierInfo[1].AddTextPiece( FONT_SIZE_17, szText );
	}

	ShowChildWnd( ID_HIRE_BTN );
	
	if( g_MyInfo.IsCharExerciseStyle( g_MyInfo.GetCharArray( dwCharIndex ), EXERCISE_PCROOM ) )
	{
		if( g_MyInfo.IsUserEvent() )
			ShowChildWnd( ID_FREEDAY_SOLDIER_CHANGE_BTN );
		else if( g_MyInfo.GetPCRoomAuthority() > 0 )
			ShowChildWnd( ID_PCROOM_SOLDIER_CHANGE_BTN );
	}
	else
	{
		ShowChildWnd( ID_DISMISSAL_BTN );
		ShowChildWnd( ID_DISASSEMBLE_BTN );

		if( bHire )
		{
			SetChildActive( ID_DISMISSAL_BTN );
			SetChildActive( ID_DISASSEMBLE_BTN );
		}
		else
		{
			SetChildInActive( ID_DISMISSAL_BTN );
			SetChildInActive( ID_DISASSEMBLE_BTN );
		}
	}	
	
	SetPowerUpBtn( bHire );

	HideChildWnd( ID_ETC_SHOP_BTN );
	HideChildWnd( ID_ITEM_SELL_BTN );
	HideChildWnd( ID_ETC_USE_BTN );
	HideChildWnd( ID_ETC_PRESENT_SEND_BTN );
	HideChildWnd( ID_ETC_USE_SMALL_BTN );
	HideChildWnd( ID_ITEM_ICON );
	HideChildWnd( ID_ITEM_RECV );
	HideChildWnd( ID_ITEM_RETR );
	HideChildWnd( ID_PRESENT_DISASSEMBLE_BTN );

	HideChildWnd( ID_KINDRED_BTN );
	HideChildWnd( ID_HAIR_BTN );
	HideChildWnd( ID_HAIR_COLOR_BTN );
	HideChildWnd( ID_FACE_BTN );
	HideChildWnd( ID_SKIN_COLOR_BTN );
	HideChildWnd( ID_UNDERWEAR_BTN );

	HideChildWnd( ID_ETC_SOULSTONE_USE_BTN );
	HideChildWnd( ID_ETC_SOULSTONE_ENABLE_LIST );

	HideChildWnd( ID_SOLDIER_WND );
	HideChildWnd( ID_COSTUME_SOLDIER_WND );
	HideChildWnd( ID_INNER_TAB1 );
	ShowChildWnd( ID_INNER_TAB2 );

	// UI코스튬 관련 (코스튬 버튼 해제)
	HideChildWnd( ID_WEAPON_COSTUME_BTN );
	HideChildWnd( ID_ARMOR_COSTUME_BTN );
	HideChildWnd( ID_HELMET_COSTUME_BTN );
	HideChildWnd( ID_CLOAK_COSTUME_BTN );

	int iCharArray = g_MyInfo.GetClassArray( m_iClassType );
	if( COMPARE( iCharArray, 0, g_MyInfo.GetCharCount() ) )
		m_CharInfo = g_MyInfo.GetCharacter( iCharArray );
	else
		Help::GetDefaultCharInfo( m_CharInfo, m_iClassType );

	// Show
	// 코스튬 관련 (용병)
	NewShopHelpWnd *pCharWnd = dynamic_cast<NewShopHelpWnd *>( FindChildWnd( ID_SOLDIER_WND ) );
	if( pCharWnd )
	{
		pCharWnd->SetWndPos( 47, 115 );
		pCharWnd->SetZPosition( FLOAT500 );
		pCharWnd->SetChar( m_CharInfo );
		pCharWnd->SetEquip( false, false, false );
		pCharWnd->SetUserInfoToolTip( true );
		pCharWnd->ShowWnd();
	}

	ioWnd* pListBtn = FindChildWnd( ID_MY_SOLDIER_LIST_BTN );
	if( pListBtn )
	{
		pListBtn->ShowWnd();
		pListBtn->SetWndPos( 51, 97 );
	}
	ioWnd* pLeftBtn = FindChildWnd( ID_MY_SOLDIER_LEFT_BTN );
	if( pLeftBtn )
	{
		pLeftBtn->ShowWnd();
		pLeftBtn->SetWndPos( 27, 97 );
	}
	ioWnd* pRightBtn = FindChildWnd( ID_MY_SOLDIER_RIGHT_BTN );
	if( pRightBtn )
	{
		pRightBtn->ShowWnd();
		pRightBtn->SetWndPos( 203, 97 );
	}
}

void MyInventoryInfoWnd::SetPowerUpBtn( bool bHire )
{
	ioHashString szerror;
	bool bUsePowerUpBtn = false;
	m_bShowPowerUpToolTip = false;
	if ( bHire )
	{
		//대상이 강화 용병인지 체크
		const ioPowerUpManager::CharPowerUpInfo* pInfo = g_PowerUpManager.GetCharGradeInfo( m_iClassType );
		if ( pInfo )
		{
			//캐릭터를 보유하고 있는지 체크
			const CHARACTER& rkChar = g_MyInfo.GetCharacterToClassType( m_iClassType );
			if ( COMPARE( rkChar.m_eCharPowerUpType, PUGT_CHAR_GRADE1, min( pInfo->iPowerUpMax + 1, PUGT_CHAR_GRADE_MAX ) ) )
				m_bShowPowerUpToolTip = true;
			if( rkChar.m_class_type == m_iClassType )
			{
				//영구 고용 상태며 구매 용병인지 체크
				if ( rkChar.m_ePeriodType == CPT_MORTMAIN && rkChar.m_chExerciseStyle == EXERCISE_NONE)
				{
					//강화가 가능한지 체크
					int eNextGrade = rkChar.m_eCharPowerUpType + PUGT_CHAR_GRADE_BASE;
					// 2018. 12. 19. syh 최종 진화상태이어도 퇴화 기능으로 인해 버튼 활성화
					if( COMPARE( eNextGrade, PUGT_CHAR_GRADE1, min( pInfo->iPowerUpMax + 1, PUGT_CHAR_GRADE_MAX ) + 1 ) )
						bUsePowerUpBtn = true;
				}
			}
		}
	}
	else
	{
		//고용되진 않았지만 강화가 가능한 용병이라면.
		const ioPowerUpManager::CharPowerUpInfo* pInfo = g_PowerUpManager.GetCharGradeInfo( m_iClassType );
		if ( pInfo )
		{
			bUsePowerUpBtn = true;
			const ioClassExpert::CLASSSLOT* pClassSlot = g_MyInfo.GetClassExpertSlot( m_iClassType );
			if ( pClassSlot )
			{
				if ( COMPARE( pClassSlot->m_ePowerUpCharGrade, PUGT_CHAR_GRADE1, min( pInfo->iPowerUpMax + 1, PUGT_CHAR_GRADE_MAX ) ) )
					m_bShowPowerUpToolTip = true;
			}
		}
	}
	if (  bUsePowerUpBtn )
	{
		ShowChildWnd( ID_POWERUP_BTN );
		ShowChildWnd( ID_POWERUP_HIRE_BTN );
		ShowChildWnd( ID_POWERUP_DISASSEMBLE_BTN );
		ShowChildWnd( ID_POWERUP_DISMISSAL_BTN );
		HideChildWnd( ID_HIRE_BTN );
		HideChildWnd( ID_DISASSEMBLE_BTN );
		HideChildWnd( ID_DISMISSAL_BTN );
		if ( bHire )
		{
			SetChildActive( ID_POWERUP_BTN );
			SetChildActive( ID_POWERUP_HIRE_BTN );
			SetChildActive( ID_POWERUP_DISASSEMBLE_BTN );
			SetChildActive( ID_POWERUP_DISMISSAL_BTN );
		}
		else
		{
			SetChildInActive( ID_POWERUP_BTN );
			SetChildActive( ID_POWERUP_HIRE_BTN );
			SetChildInActive( ID_POWERUP_DISASSEMBLE_BTN );
			SetChildInActive( ID_POWERUP_DISMISSAL_BTN );
		}
	}
}

void MyInventoryInfoWnd::SetInvenItemInfo( int iClassType, const InventoryItemInfo &rkInfo )
{
	m_iResellType = 0;
	m_iPresentSlotIndex = 0;

	m_iCurLevel = 0;
	m_iCurLevelExp = 0;
	m_iNextLevelExp = 0;
	m_iExpRate = 0;

	m_ItemInfo = rkInfo;
	m_iClassType = iClassType;
	m_iMainType = rkInfo.m_iMainType;
	m_bSoldierTab = false;

	m_pManual = NULL;

	m_SoldierInfo[0].ClearList();
	m_SoldierInfo[1].ClearList();

	m_PresentInfo[0].ClearList();
	m_PresentInfo[1].ClearList();

	m_SubscriptionInfo[0].ClearList();
	m_SubscriptionInfo[1].ClearList();
	m_SubscriptionInfo[2].ClearList();

	m_EtcInfo.ClearList();

	for( int i=0; i < 6; ++i )
	{
		m_DecoName[i].Clear();
	}

	if( iClassType == -1 )
		m_iClassType = g_MyInfo.GetClassType();

	if( g_MyInfo.GetCharIndexByClassType( iClassType ) == 0 )
		m_iClassType = g_MyInfo.GetClassType();

	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*> ( GetParent() );
	if( pInvenWnd && iClassType != m_iClassType && m_iMainType != INVENTORY_ITEM_ETC )
	{
		pInvenWnd->ChangeClassType( m_iClassType );
		return;
	}

	switch( m_iMainType )
	{
	case INVENTORY_ITEM_DECO:
		SetDecoItemInfo( m_iClassType, rkInfo );
		break;
	case INVENTORY_ITEM_ETC:
		SetEtcItemInfo( m_iClassType, rkInfo );
		break;
	case INVENTORY_ITEM_PRESENT:
		SetPresentItemInfo( m_iClassType, rkInfo );
		break;
	case INVENTORY_ITEM_SUBSCRIPTION:
		SetSubscriptionItemInfo( m_iClassType, rkInfo );
		break;
		// UI코스튬 관련 (정보 UI설정)
	case INVENTORY_ITEM_COSTUME:
		SetCostumeInfo( m_iClassType, rkInfo );
		break;
	}
}

void MyInventoryInfoWnd::SetItemBlank( int iMainTab, int iSmallTab, int iClassType )
{
	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
	if( !pInvenWnd ) return;

	int iDefaultSmallTab = pInvenWnd->GetDefaultSmallTab( iMainTab );

	// UI코스튬 관련 (아이템이 없을때 설정)
	int iMainType = 0;
	switch( iMainTab )
	{
	case MyInventoryWnd::ID_DECO_TAB_BTN:
		iMainType = INVENTORY_ITEM_DECO;
		break;
	case MyInventoryWnd::ID_ETC_TAB_BTN:
		iMainType = INVENTORY_ITEM_ETC;
		break;
	case MyInventoryWnd::ID_PRESENT_TAB_BTN:
		iMainType = INVENTORY_ITEM_PRESENT;
		break;
	case MyInventoryWnd::ID_COSTUME_TAB_BTN:
		iMainType = INVENTORY_ITEM_COSTUME;
		break;
	}
	
	switch( iMainType )
	{
	case INVENTORY_ITEM_ETC:
	case INVENTORY_ITEM_PRESENT:
		SetBlank();
		return;
	}

	m_iResellType = 0;
	m_iPresentSlotIndex = 0;

	m_iCurLevel = 0;
	m_iCurLevelExp = 0;
	m_iNextLevelExp = 0;
	m_iExpRate = 0;

	m_iClassType = iClassType;
	m_iMainType = iMainType;
	m_bSoldierTab = false;

	m_pManual = NULL;
	m_ItemInfo.Init();

	m_SoldierInfo[0].ClearList();
	m_SoldierInfo[1].ClearList();

	m_PresentInfo[0].ClearList();
	m_PresentInfo[1].ClearList();

	m_SubscriptionInfo[0].ClearList();
	m_SubscriptionInfo[1].ClearList();
	m_SubscriptionInfo[2].ClearList();

	m_EtcInfo.ClearList();

	for( int i=0; i < 6; ++i )
	{
		m_DecoName[i].Clear();
	}

	if( iClassType == -1 )
	{
		m_iClassType = g_MyInfo.GetClassType();
	}

	switch( m_iMainType )
	{
	case INVENTORY_ITEM_DECO:
		SetDecoItemInfo( m_iClassType, m_ItemInfo );
		break;
		// UI코스튬 관련 (정보 UI 설정)
	case INVENTORY_ITEM_COSTUME:
		SetCostumeInfo( m_iClassType, m_ItemInfo );
		break;
	}
}

void MyInventoryInfoWnd::SetDecoItemInfo( int iClassType, const InventoryItemInfo &rkInfo )
{
	HideChildWnd( ID_HIRE_BTN );
	HideChildWnd( ID_DISMISSAL_BTN );
	HideChildWnd( ID_DISASSEMBLE_BTN );
	HideChildWnd( ID_PCROOM_SOLDIER_CHANGE_BTN );
	HideChildWnd( ID_FREEDAY_SOLDIER_CHANGE_BTN );

	HideChildWnd( ID_ETC_SHOP_BTN );
	HideChildWnd( ID_ITEM_SELL_BTN );
	HideChildWnd( ID_ETC_USE_BTN );
	HideChildWnd( ID_ETC_PRESENT_SEND_BTN );
	HideChildWnd( ID_ETC_USE_SMALL_BTN );
	HideChildWnd( ID_ITEM_ICON );
	HideChildWnd( ID_ITEM_RECV );
	HideChildWnd( ID_ITEM_RETR );
	HideChildWnd( ID_PRESENT_DISASSEMBLE_BTN );

	ShowChildWnd( ID_KINDRED_BTN );
	ShowChildWnd( ID_HAIR_BTN );
	ShowChildWnd( ID_HAIR_COLOR_BTN );
	ShowChildWnd( ID_FACE_BTN );
	ShowChildWnd( ID_SKIN_COLOR_BTN );
	ShowChildWnd( ID_UNDERWEAR_BTN );

	HideChildWnd( ID_POWERUP_BTN );
	HideChildWnd( ID_POWERUP_HIRE_BTN );
	HideChildWnd( ID_POWERUP_DISASSEMBLE_BTN );
	HideChildWnd( ID_POWERUP_DISMISSAL_BTN );

	HideChildWnd( ID_ETC_SOULSTONE_USE_BTN );
	HideChildWnd( ID_ETC_SOULSTONE_ENABLE_LIST );

	HideChildWnd( ID_SOLDIER_WND );
	HideChildWnd( ID_COSTUME_SOLDIER_WND );
	ShowChildWnd( ID_INNER_TAB1 );
	HideChildWnd( ID_INNER_TAB2 );

	// UI코스튬 관련 (코스튬 버튼 해제)
	HideChildWnd( ID_WEAPON_COSTUME_BTN );
	HideChildWnd( ID_ARMOR_COSTUME_BTN );
	HideChildWnd( ID_HELMET_COSTUME_BTN );
	HideChildWnd( ID_CLOAK_COSTUME_BTN );

	int iCharArray = g_MyInfo.GetClassArray( iClassType );
	if( COMPARE( iCharArray, 0, g_MyInfo.GetCharCount() ) )
		m_CharInfo = g_MyInfo.GetCharacter( iCharArray );
	else
		Help::GetDefaultCharInfo( m_CharInfo, iClassType );

	NewShopHelpWnd *pCharWnd = dynamic_cast<NewShopHelpWnd *>( FindChildWnd( ID_SOLDIER_WND ));
	if( pCharWnd )
	{
		// 코스튬 관련 (치장 관련 모두 해제)
		pCharWnd->SetWndPos( 47, 72 );
		pCharWnd->SetZPosition( 600.0f );
		pCharWnd->SetChar( m_CharInfo );
		pCharWnd->SetEquip( true, true, false );
		pCharWnd->SetUserInfoToolTip( false );
		pCharWnd->ShowWnd();
	}

	ioWnd* pListBtn = FindChildWnd( ID_MY_SOLDIER_LIST_BTN );
	if( pListBtn )
	{
		pListBtn->ShowWnd();
		pListBtn->SetWndPos( 51, 97 );
	}
	ioWnd* pLeftBtn = FindChildWnd( ID_MY_SOLDIER_LEFT_BTN );
	if( pLeftBtn )
	{
		pLeftBtn->ShowWnd();
		pLeftBtn->SetWndPos( 27, 97 );
	}
	ioWnd* pRightBtn = FindChildWnd( ID_MY_SOLDIER_RIGHT_BTN );
	if( pRightBtn )
	{
		pRightBtn->ShowWnd();
		pRightBtn->SetWndPos( 203, 97 );
	}

	if( iClassType <= 0 )
	{
		HideChildWnd( ID_KINDRED_BTN );
		HideChildWnd( ID_HAIR_BTN );
		HideChildWnd( ID_HAIR_COLOR_BTN );
		HideChildWnd( ID_FACE_BTN );
		HideChildWnd( ID_SKIN_COLOR_BTN );
		HideChildWnd( ID_UNDERWEAR_BTN );
		return;
	}

	InventoryIconBtn *pBtn = NULL;
	pBtn = dynamic_cast<InventoryIconBtn*>(FindChildWnd(ID_KINDRED_BTN));
	if( pBtn )
	{
		int iDecoType = UID_KINDRED;
		int iDecoCode = (int)GetRaceDetailType( m_CharInfo );

		bool bMale = true;
		// 여성 캐릭터 타입
		switch( iDecoCode )
		{
		case RDT_HUMAN_WOMAN:
		case RDT_ELF_WOMAN:
		case RDT_DWARF_WOMAN:
			bMale = false;
			break;
		}

		const DecoData *pData =  g_DecorationPrice.GetDecoDataPtr( m_CharInfo.m_sex - 1, iDecoType, iDecoCode );
		if( pData )
		{
			ioUIRenderImage *pImg = g_MyInfo.GetSoldierIcon( iClassType, bMale );
			ioUIRenderImage *pSubImg = g_MyInfo.GetSoldierSubIcon( iClassType );
			pBtn->SetIcon( pImg, FLOAT08, FLOAT08 );
			pBtn->SetSubIcon( pSubImg );
			pBtn->SetMarkType( INVENTORY_MARK_TYPE_NONE );
			pBtn->SetDecoInfo( m_CharInfo.m_sex - 1, iDecoType, iDecoCode );
			m_DecoName[0] = pData->m_szName;
		}
	}

	pBtn = dynamic_cast<InventoryIconBtn*>(FindChildWnd(ID_HAIR_BTN));
	if( pBtn )
	{
		int iDecoType = UID_HAIR;
		int iDecoCode = m_CharInfo.m_hair;

		const DecoData *pData =  g_DecorationPrice.GetDecoDataPtr( m_CharInfo.m_sex - 1, iDecoType, iDecoCode );
		if( pData )
		{
			ioUIRenderImage *pImg = g_UIImageSetMgr.CreateImageByFullName( pData->m_szIcon );
			pBtn->SetIcon( pImg, FLOAT08, FLOAT08 );
			pBtn->SetMarkType( INVENTORY_MARK_TYPE_NONE );
			pBtn->SetDecoInfo( m_CharInfo.m_sex - 1, iDecoType, iDecoCode );
			m_DecoName[1] = pData->m_szName;
		}
	}

	pBtn = dynamic_cast<InventoryIconBtn*>(FindChildWnd(ID_HAIR_COLOR_BTN));
	if( pBtn )
	{
		int iDecoType = UID_HAIR_COLOR;
		int iDecoCode = m_CharInfo.m_hair_color;

		const DecoData *pData =  g_DecorationPrice.GetDecoDataPtr( m_CharInfo.m_sex - 1, iDecoType, iDecoCode );
		if( pData )
		{
			ioUIRenderImage *pImg = g_UIImageSetMgr.CreateImageByFullName( pData->m_szIcon );
			pBtn->SetIcon( pImg, FLOAT08, FLOAT08 );
			pBtn->SetMarkType( INVENTORY_MARK_TYPE_NONE );
			pBtn->SetDecoInfo( m_CharInfo.m_sex - 1, iDecoType, iDecoCode );
			m_DecoName[2] = pData->m_szName;
		}
	}

	pBtn = dynamic_cast<InventoryIconBtn*>(FindChildWnd(ID_FACE_BTN));
	if( pBtn )
	{
		int iDecoType = UID_FACE;
		int iDecoCode = m_CharInfo.m_face;

		const DecoData *pData =  g_DecorationPrice.GetDecoDataPtr( m_CharInfo.m_sex - 1, iDecoType, iDecoCode );
		if( pData )
		{
			ioUIRenderImage *pImg = g_UIImageSetMgr.CreateImageByFullName( pData->m_szIcon );
			pBtn->SetIcon( pImg, FLOAT08, FLOAT08 );
			pBtn->SetMarkType( INVENTORY_MARK_TYPE_NONE );
			pBtn->SetDecoInfo( m_CharInfo.m_sex - 1, iDecoType, iDecoCode );
			m_DecoName[3] = pData->m_szName;
		}
	}

	pBtn = dynamic_cast<InventoryIconBtn*>(FindChildWnd(ID_SKIN_COLOR_BTN));
	if( pBtn )
	{
		int iDecoType = UID_SKIN_COLOR;
		int iDecoCode = m_CharInfo.m_skin_color;

		const DecoData *pData =  g_DecorationPrice.GetDecoDataPtr( m_CharInfo.m_sex - 1, iDecoType, iDecoCode );
		if( pData )
		{
			ioUIRenderImage *pImg = g_UIImageSetMgr.CreateImageByFullName( pData->m_szIcon );
			pBtn->SetIcon( pImg, FLOAT08, FLOAT08 );
			pBtn->SetMarkType( INVENTORY_MARK_TYPE_NONE );
			pBtn->SetDecoInfo( m_CharInfo.m_sex - 1, iDecoType, iDecoCode );
			m_DecoName[4] = pData->m_szName;
		}
	}

	pBtn = dynamic_cast<InventoryIconBtn*>(FindChildWnd(ID_UNDERWEAR_BTN));
	if( pBtn )
	{
		int iDecoType = UID_UNDERWEAR;
		int iDecoCode = m_CharInfo.m_underwear;

		const DecoData *pData =  g_DecorationPrice.GetDecoDataPtr( m_CharInfo.m_sex - 1, iDecoType, iDecoCode );
		if( pData )
		{
			ioUIRenderImage *pImg = g_UIImageSetMgr.CreateImageByFullName( pData->m_szIcon );
			pBtn->SetIcon( pImg, FLOAT08, FLOAT08 );
			pBtn->SetMarkType( INVENTORY_MARK_TYPE_NONE );
			pBtn->SetDecoInfo( m_CharInfo.m_sex - 1, iDecoType, iDecoCode );
			m_DecoName[5] = pData->m_szName;
		}
	}
}

void MyInventoryInfoWnd::SetEtcItemInfo( int iClassType, const InventoryItemInfo &rkInfo )
{
	int iMagicCode = rkInfo.m_iMagicCode;
	if( iMagicCode <= 0 ) return;

	HideChildWnd( ID_POWERUP_BTN );
	HideChildWnd( ID_POWERUP_HIRE_BTN );
	HideChildWnd( ID_POWERUP_DISASSEMBLE_BTN );
	HideChildWnd( ID_POWERUP_DISMISSAL_BTN );

	HideChildWnd( ID_HIRE_BTN );
	HideChildWnd( ID_DISMISSAL_BTN );
	HideChildWnd( ID_DISASSEMBLE_BTN );
	HideChildWnd( ID_PCROOM_SOLDIER_CHANGE_BTN );
	HideChildWnd( ID_FREEDAY_SOLDIER_CHANGE_BTN );

	HideChildWnd( ID_MY_SOLDIER_LIST_BTN );
	HideChildWnd( ID_MY_SOLDIER_LEFT_BTN );
	HideChildWnd( ID_MY_SOLDIER_RIGHT_BTN );
	HideChildWnd( ID_INNER_TAB1 );
	HideChildWnd( ID_INNER_TAB2 );
	HideChildWnd( ID_ETC_SHOP_BTN );
	HideChildWnd( ID_ITEM_SELL_BTN );
	HideChildWnd( ID_ETC_USE_BTN );
	HideChildWnd( ID_ETC_PRESENT_SEND_BTN );
	HideChildWnd( ID_ETC_USE_SMALL_BTN );
	HideChildWnd( ID_ITEM_ICON );
	HideChildWnd( ID_ITEM_RECV );
	HideChildWnd( ID_ITEM_RETR );
	HideChildWnd( ID_PRESENT_DISASSEMBLE_BTN );

	HideChildWnd( ID_KINDRED_BTN );
	HideChildWnd( ID_HAIR_BTN );
	HideChildWnd( ID_HAIR_COLOR_BTN );
	HideChildWnd( ID_FACE_BTN );
	HideChildWnd( ID_SKIN_COLOR_BTN );
	HideChildWnd( ID_UNDERWEAR_BTN );

	HideChildWnd( ID_ETC_SOULSTONE_USE_BTN );
	HideChildWnd( ID_ETC_SOULSTONE_ENABLE_LIST );

	HideChildWnd( ID_SOLDIER_WND );
	HideChildWnd( ID_COSTUME_SOLDIER_WND );
	HideChildWnd( ID_INNER_TAB2 );

	// UI코스튬 관련 (코스튬 버튼 해제)
	HideChildWnd( ID_WEAPON_COSTUME_BTN );
	HideChildWnd( ID_ARMOR_COSTUME_BTN );
	HideChildWnd( ID_HELMET_COSTUME_BTN );
	HideChildWnd( ID_CLOAK_COSTUME_BTN );

	ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( iMagicCode );
	if( !pEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s ioEtcItem == NULL , Type:%d", __FUNCTION__, iMagicCode );
		return;
	}

	ioUserEtcItem *pUserEtcItem =  g_MyInfo.GetUserEtcItem();
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s UserEtcItem == NULL , Type:%d", __FUNCTION__, iMagicCode );
		return;
	}

	ioUserEtcItem::ETCITEMSLOT kItemSlot;
	if( !pUserEtcItem->GetEtcItem( iMagicCode, kItemSlot ) )
	{
		LOG.PrintTimeAndLog( 0, "%s don't have , Type:%d", __FUNCTION__, iMagicCode );
		return;
	}

	if( pEtcItem->IsShop3DChar() )
	{
		int iCharArray = g_MyInfo.GetClassArray( iClassType );
		if( COMPARE( iCharArray, 0, g_MyInfo.GetCharCount() ) )
			m_CharInfo = g_MyInfo.GetCharacter( iCharArray );
		else
			Help::GetDefaultCharInfo( m_CharInfo, iClassType );

		if( COMPARE( pEtcItem->GetType(), ioEtcItem::EIT_ETC_MOTION1, ioEtcItem::EIT_ETC_MOTION100 + 1 ) ||
			COMPARE( pEtcItem->GetType(), ioEtcItem::EIT_ETC_MOTION101, ioEtcItem::EIT_ETC_MOTION400 + 1 ) )
		{
			NewShopHelpWnd *pCharWnd = dynamic_cast<NewShopHelpWnd *>( FindChildWnd( ID_SOLDIER_WND ));
			if( pCharWnd )
			{
				pCharWnd->SetWndPos( 47, 72 );
				pCharWnd->SetZPosition( 600.0f );
				pCharWnd->SetChar( m_CharInfo );
				pCharWnd->SetEquip( true, true, false );
				pCharWnd->SetCharAnimation( pEtcItem->GetType(), ( m_CharInfo.m_sex == 1 ? true : false ) );
				pCharWnd->SetUserInfoToolTip( false );
				pCharWnd->ShowWnd();
			}

			ioWnd* pListBtn = FindChildWnd( ID_MY_SOLDIER_LIST_BTN );
			if( pListBtn )
			{
				pListBtn->ShowWnd();
				pListBtn->SetWndPos( 51, 97 );
			}
			ioWnd* pLeftBtn = FindChildWnd( ID_MY_SOLDIER_LEFT_BTN );
			if( pLeftBtn )
			{
				pLeftBtn->ShowWnd();
				pLeftBtn->SetWndPos( 27, 97 );
			}
			ioWnd* pRightBtn = FindChildWnd( ID_MY_SOLDIER_RIGHT_BTN );
			if( pRightBtn )
			{
				pRightBtn->ShowWnd();
				pRightBtn->SetWndPos( 203, 97 );
			}
			ShowChildWnd( ID_INNER_TAB1 );

			InventoryIconBtn *pBtn = NULL;
			pBtn = dynamic_cast<InventoryIconBtn*>(FindChildWnd(ID_KINDRED_BTN));
			if( pBtn )
			{
				int iDecoType = UID_KINDRED;
				int iDecoCode = (int)GetRaceDetailType( m_CharInfo );

				bool bMale = true;
				// 여성 캐릭터 타입
				switch( iDecoCode )
				{
				case RDT_HUMAN_WOMAN:
				case RDT_ELF_WOMAN:
				case RDT_DWARF_WOMAN:
					bMale = false;
					break;
				}

				const DecoData *pData =  g_DecorationPrice.GetDecoDataPtr( m_CharInfo.m_sex - 1, iDecoType, iDecoCode );
				if( pData )
				{
					ioUIRenderImage *pImg = g_MyInfo.GetSoldierIcon( iClassType, bMale );
					ioUIRenderImage *pSubImg = g_MyInfo.GetSoldierSubIcon( iClassType );
					pBtn->SetIcon( pImg, FLOAT08, FLOAT08 );
					pBtn->SetSubIcon( pSubImg );
					pBtn->SetMarkType( INVENTORY_MARK_TYPE_NONE );
					pBtn->SetDecoInfo( m_CharInfo.m_sex - 1, iDecoType, iDecoCode );
					m_DecoName[0] = pData->m_szName;
				}
			}

			pBtn = dynamic_cast<InventoryIconBtn*>(FindChildWnd(ID_HAIR_BTN));
			if( pBtn )
			{
				int iDecoType = UID_HAIR;
				int iDecoCode = m_CharInfo.m_hair;

				const DecoData *pData =  g_DecorationPrice.GetDecoDataPtr( m_CharInfo.m_sex - 1, iDecoType, iDecoCode );
				if( pData )
				{
					ioUIRenderImage *pImg = g_UIImageSetMgr.CreateImageByFullName( pData->m_szIcon );
					pBtn->SetIcon( pImg, FLOAT08, FLOAT08 );
					pBtn->SetMarkType( INVENTORY_MARK_TYPE_NONE );
					pBtn->SetDecoInfo( m_CharInfo.m_sex - 1, iDecoType, iDecoCode );
					m_DecoName[1] = pData->m_szName;
				}
			}

			pBtn = dynamic_cast<InventoryIconBtn*>(FindChildWnd(ID_HAIR_COLOR_BTN));
			if( pBtn )
			{
				int iDecoType = UID_HAIR_COLOR;
				int iDecoCode = m_CharInfo.m_hair_color;

				const DecoData *pData =  g_DecorationPrice.GetDecoDataPtr( m_CharInfo.m_sex - 1, iDecoType, iDecoCode );
				if( pData )
				{
					ioUIRenderImage *pImg = g_UIImageSetMgr.CreateImageByFullName( pData->m_szIcon );
					pBtn->SetIcon( pImg, FLOAT08, FLOAT08 );
					pBtn->SetMarkType( INVENTORY_MARK_TYPE_NONE );
					pBtn->SetDecoInfo( m_CharInfo.m_sex - 1, iDecoType, iDecoCode );
					m_DecoName[2] = pData->m_szName;
				}
			}

			pBtn = dynamic_cast<InventoryIconBtn*>(FindChildWnd(ID_FACE_BTN));
			if( pBtn )
			{
				int iDecoType = UID_FACE;
				int iDecoCode = m_CharInfo.m_face;

				const DecoData *pData =  g_DecorationPrice.GetDecoDataPtr( m_CharInfo.m_sex - 1, iDecoType, iDecoCode );
				if( pData )
				{
					ioUIRenderImage *pImg = g_UIImageSetMgr.CreateImageByFullName( pData->m_szIcon );
					pBtn->SetIcon( pImg, FLOAT08, FLOAT08 );
					pBtn->SetMarkType( INVENTORY_MARK_TYPE_NONE );
					pBtn->SetDecoInfo( m_CharInfo.m_sex - 1, iDecoType, iDecoCode );
					m_DecoName[3] = pData->m_szName;
				}
			}

			pBtn = dynamic_cast<InventoryIconBtn*>(FindChildWnd(ID_SKIN_COLOR_BTN));
			if( pBtn )
			{
				int iDecoType = UID_SKIN_COLOR;
				int iDecoCode = m_CharInfo.m_skin_color;

				const DecoData *pData =  g_DecorationPrice.GetDecoDataPtr( m_CharInfo.m_sex - 1, iDecoType, iDecoCode );
				if( pData )
				{
					ioUIRenderImage *pImg = g_UIImageSetMgr.CreateImageByFullName( pData->m_szIcon );
					pBtn->SetIcon( pImg, FLOAT08, FLOAT08 );
					pBtn->SetMarkType( INVENTORY_MARK_TYPE_NONE );
					pBtn->SetDecoInfo( m_CharInfo.m_sex - 1, iDecoType, iDecoCode );
					m_DecoName[4] = pData->m_szName;
				}
			}

			pBtn = dynamic_cast<InventoryIconBtn*>(FindChildWnd(ID_UNDERWEAR_BTN));
			if( pBtn )
			{
				int iDecoType = UID_UNDERWEAR;
				int iDecoCode = m_CharInfo.m_underwear;

				const DecoData *pData =  g_DecorationPrice.GetDecoDataPtr( m_CharInfo.m_sex - 1, iDecoType, iDecoCode );
				if( pData )
				{
					ioUIRenderImage *pImg = g_UIImageSetMgr.CreateImageByFullName( pData->m_szIcon );
					pBtn->SetIcon( pImg, FLOAT08, FLOAT08 );
					pBtn->SetMarkType( INVENTORY_MARK_TYPE_NONE );
					pBtn->SetDecoInfo( m_CharInfo.m_sex - 1, iDecoType, iDecoCode );
					m_DecoName[5] = pData->m_szName;
				}
			}

			ShowChildWnd( ID_KINDRED_BTN );
			ShowChildWnd( ID_HAIR_BTN );
			ShowChildWnd( ID_HAIR_COLOR_BTN );
			ShowChildWnd( ID_FACE_BTN );
			ShowChildWnd( ID_SKIN_COLOR_BTN );
			ShowChildWnd( ID_UNDERWEAR_BTN );
		}
		else
		{
			NewShopHelpWnd *pCharWnd = dynamic_cast<NewShopHelpWnd *>( FindChildWnd( ID_SOLDIER_WND ));
			if( pCharWnd )
			{
				pCharWnd->SetWndPos( 47, 39 );
				pCharWnd->SetZPosition( 600.0f );
				pCharWnd->SetChar( m_CharInfo );
				pCharWnd->SetEquip( false, false, false );
				pCharWnd->SetChangeSkeleton( pEtcItem->GetType(), ( m_CharInfo.m_sex == 1 ? true : false ) );
				pCharWnd->SetUserInfoToolTip( false );
				pCharWnd->ShowWnd();
			}

			ioWnd* pListBtn = FindChildWnd( ID_MY_SOLDIER_LIST_BTN );
			if( pListBtn )
			{
				pListBtn->ShowWnd();
				pListBtn->SetWndPos( 51, 59 );
			}
			ioWnd* pLeftBtn = FindChildWnd( ID_MY_SOLDIER_LEFT_BTN );
			if( pLeftBtn )
			{
				pLeftBtn->ShowWnd();
				pLeftBtn->SetWndPos( 27, 59 );
			}
			ioWnd* pRightBtn = FindChildWnd( ID_MY_SOLDIER_RIGHT_BTN );
			if( pRightBtn )
			{
				pRightBtn->ShowWnd();
				pRightBtn->SetWndPos( 203, 59 );
			}

			if( pEtcItem->IsCanExtendUse() )
			{
				HideChildWnd( ID_ETC_SHOP_BTN );
				HideChildWnd( ID_ITEM_SELL_BTN );
				HideChildWnd( ID_ETC_USE_SMALL_BTN );

				ShowChildWnd( ID_ETC_USE_BTN );

				if( COMPARE( pEtcItem->GetType(), ioEtcItem::EIT_ETC_ITEM_TIME_GASHAPON1, ioEtcItem::EIT_ETC_ITEM_TIME_GASHAPON100 + 1 )	||
					COMPARE( pEtcItem->GetType(), ioEtcItem::EIT_ETC_ITEM_TIME_GASHAPON101, ioEtcItem::EIT_ETC_ITEM_TIME_GASHAPON300 + 1 )	||
					COMPARE( pEtcItem->GetType(), ioEtcItem::EIT_ETC_ITEM_TIME_GASHAPON301, ioEtcItem::EIT_ETC_ITEM_TIME_GASHAPON600 + 1 ) )
				{
					SetChildInActive( ID_ETC_USE_BTN );

					// 사용 불가 상태이면 
					ioUserEtcItem *pUserEtcItem = g_MyInfo.GetUserEtcItem();
					if( pUserEtcItem )
					{
						ioUserEtcItem::ETCITEMSLOT kSlot;
						if( pUserEtcItem->GetEtcItem( pEtcItem->GetType(), kSlot ) )
						{
							ioEtcItemTimeGashapon *pEtcItemTimeGashapon = dynamic_cast< ioEtcItemTimeGashapon * >( pEtcItem );
							if( pEtcItemTimeGashapon && pEtcItemTimeGashapon->IsBtnActive( pUserEtcItem ) )
								SetChildActive( ID_ETC_USE_BTN );
						}
					}					
				}
				else if( pEtcItem->GetType() == ioEtcItem::EIT_ETC_SOLDIER_EXP_BONUS ||
						 COMPARE( pEtcItem->GetType(), ioEtcItem::EIT_ETC_SOLDIER_EXP_ADD_001, ioEtcItem::EIT_ETC_SOLDIER_EXP_ADD_200 + 1 ) ||
						 COMPARE( pEtcItem->GetType(), ioEtcItem::EIT_ETC_SOLDIER_EXP_ADD_201, ioEtcItem::EIT_ETC_SOLDIER_EXP_ADD_2000 + 1 ) )
				{
					HideChildWnd( ID_ETC_USE_BTN );
					ShowChildWnd( ID_ETC_USE_SMALL_BTN );

					SetChildInActive( ID_ETC_USE_SMALL_BTN );
					ioUserEtcItem *pUserEtcItem = g_MyInfo.GetUserEtcItem();
					if( pUserEtcItem )
					{
						ioUserEtcItem::ETCITEMSLOT kSlot;
						if( pUserEtcItem->GetEtcItem( pEtcItem->GetType(), kSlot ) )
						{
							if( kSlot.GetDateExcludeValue2() == 0 )
								SetChildActive( ID_ETC_USE_SMALL_BTN );
						}
					}				

					ShowChildWnd( ID_ITEM_SELL_BTN );
					if( pEtcItem->IsCanSell() )
						SetChildActive( ID_ITEM_SELL_BTN );
					else
						SetChildInActive( ID_ITEM_SELL_BTN );
				}
				else
				{
					SetChildActive( ID_ETC_USE_BTN );
				}
			}
			else
			{
				ShowChildWnd( ID_ETC_SHOP_BTN );
				ShowChildWnd( ID_ITEM_SELL_BTN );

				if( pEtcItem->IsCanSell() )
					SetChildActive( ID_ITEM_SELL_BTN );
				else
					SetChildInActive( ID_ITEM_SELL_BTN );

				if( pEtcItem->IsBlockBuying() )
					SetChildInActive( ID_ETC_SHOP_BTN );
				else
					SetChildActive( ID_ETC_SHOP_BTN );

				HideChildWnd( ID_ETC_USE_BTN );
				HideChildWnd( ID_ETC_USE_SMALL_BTN );
			}

			m_EtcInfo.SetTextStyle( TS_NORMAL );
			m_EtcInfo.SetBkColor( 0, 0, 0 );
			m_EtcInfo.SetTextColor( TCT_DEFAULT_DARKGRAY );
			m_EtcInfo.AddTextPiece( FONT_SIZE_17, pEtcItem->GetName().c_str() );
		}
	}
	else
	{
		// icon btn
		ioUIRenderImage *pImg = g_UIImageSetMgr.CreateImageByFullName( pEtcItem->GetIconName() );
		ioUIRenderImage *pSubImg = g_UIImageSetMgr.CreateImageByFullName( pEtcItem->GetSubIconName() );
		InventoryIconBtn *pBtn = dynamic_cast< InventoryIconBtn* > ( FindChildWnd( ID_ITEM_ICON ) );
		if( !pBtn )
		{
			LOG.PrintTimeAndLog( 0, "%s pBtn == NULL , Type:%d", __FUNCTION__, iMagicCode );
		}
		else
		{
			pBtn->SetIcon( pImg, MY_INVENTORY_INFO_ETC_SCALE, MY_INVENTORY_INFO_BACK_SCALE );
			pBtn->SetSubIcon( pSubImg );
			pBtn->SetGradeType( pEtcItem->GetGradeType() );
			pBtn->SetMarkType( INVENTORY_MARK_TYPE_NONE );
			pBtn->SetActive();
		}

		// desc
		m_EtcInfo.SetTextStyle( TS_NORMAL );
		m_EtcInfo.SetBkColor( 0, 0, 0 );
		m_EtcInfo.SetTextColor( TCT_DEFAULT_DARKGRAY );
		m_EtcInfo.AddTextPiece( FONT_SIZE_17, pEtcItem->GetName().c_str() );

		ShowChildWnd( ID_ITEM_ICON );

		if( pEtcItem->IsCanExtendUse() )
		{
			HideChildWnd( ID_ETC_SHOP_BTN );
			HideChildWnd( ID_ITEM_SELL_BTN );
			HideChildWnd( ID_ETC_USE_SMALL_BTN );

			ShowChildWnd( ID_ETC_USE_BTN );

			if( COMPARE( pEtcItem->GetType(), ioEtcItem::EIT_ETC_ITEM_TIME_GASHAPON1, ioEtcItem::EIT_ETC_ITEM_TIME_GASHAPON100 + 1 )	||
				COMPARE( pEtcItem->GetType(), ioEtcItem::EIT_ETC_ITEM_TIME_GASHAPON101, ioEtcItem::EIT_ETC_ITEM_TIME_GASHAPON300 + 1 )	||
				COMPARE( pEtcItem->GetType(), ioEtcItem::EIT_ETC_ITEM_TIME_GASHAPON301, ioEtcItem::EIT_ETC_ITEM_TIME_GASHAPON600 + 1 ) )
			{
				SetChildInActive( ID_ETC_USE_BTN );

				// 사용 불가 상태이면 
				ioUserEtcItem *pUserEtcItem = g_MyInfo.GetUserEtcItem();
				if( pUserEtcItem )
				{
					ioUserEtcItem::ETCITEMSLOT kSlot;
					if( pUserEtcItem->GetEtcItem( pEtcItem->GetType(), kSlot ) )
					{
						ioEtcItemTimeGashapon *pEtcItemTimeGashapon = dynamic_cast< ioEtcItemTimeGashapon * >( pEtcItem );
						if( pEtcItemTimeGashapon && pEtcItemTimeGashapon->IsBtnActive( pUserEtcItem ) )
							SetChildActive( ID_ETC_USE_BTN );
					}
				}				
			}
			else if( pEtcItem->GetType() == ioEtcItem::EIT_ETC_SOLDIER_EXP_BONUS ||
					COMPARE( pEtcItem->GetType(), ioEtcItem::EIT_ETC_SOLDIER_EXP_ADD_001, ioEtcItem::EIT_ETC_SOLDIER_EXP_ADD_200 + 1 ) ||
					COMPARE( pEtcItem->GetType(), ioEtcItem::EIT_ETC_SOLDIER_EXP_ADD_201, ioEtcItem::EIT_ETC_SOLDIER_EXP_ADD_2000 + 1 ) )
			{
				HideChildWnd( ID_ETC_USE_BTN );
				ShowChildWnd( ID_ETC_USE_SMALL_BTN );

				SetChildInActive( ID_ETC_USE_SMALL_BTN );
				ioUserEtcItem *pUserEtcItem = g_MyInfo.GetUserEtcItem();
				if( pUserEtcItem )
				{
					ioUserEtcItem::ETCITEMSLOT kSlot;
					if( pUserEtcItem->GetEtcItem( pEtcItem->GetType(), kSlot ) )
					{
						if( kSlot.GetDateExcludeValue2() == 0 )
							SetChildActive( ID_ETC_USE_SMALL_BTN );
					}
				}				

				ShowChildWnd( ID_ITEM_SELL_BTN );
				if( pEtcItem->IsCanSell() )
					SetChildActive( ID_ITEM_SELL_BTN );
				else
					SetChildInActive( ID_ITEM_SELL_BTN );
			}
			else if( COMPARE( pEtcItem->GetType(), ioEtcItem::EIT_ETC_ITEM_SEND_PRESENT_1, ioEtcItem::EIT_ETC_ITEM_SEND_PRESENT_100 + 1 ) )
			{
				HideChildWnd( ID_ETC_USE_BTN );
				ShowChildWnd( ID_ETC_PRESENT_SEND_BTN );
			}
			else if( pEtcItem->GetType() == ioEtcItem::EIT_ETC_SOUL_STONE )
			{
				HideChildWnd( ID_ETC_USE_BTN );
				ShowChildWnd( ID_ETC_SOULSTONE_USE_BTN );
				ShowChildWnd( ID_ETC_SOULSTONE_ENABLE_LIST );
			}
			else
			{
				SetChildActive( ID_ETC_USE_BTN );
			}
		}
		else
		{
			ShowChildWnd( ID_ETC_SHOP_BTN );
			ShowChildWnd( ID_ITEM_SELL_BTN );

			if( pEtcItem->IsCanSell() )
				SetChildActive( ID_ITEM_SELL_BTN );
			else
				SetChildInActive( ID_ITEM_SELL_BTN );

			if( pEtcItem->IsBlockBuying() )
				SetChildInActive( ID_ETC_SHOP_BTN );
			else
				SetChildActive( ID_ETC_SHOP_BTN );

			HideChildWnd( ID_ETC_USE_BTN );
			HideChildWnd( ID_ETC_USE_SMALL_BTN );
		}
		m_pManual = ioEtcInvenManualMgr::GetSingletonPtr();
	}
}

void MyInventoryInfoWnd::SetPresentItemInfo( int iClassType, const InventoryItemInfo &rkInfo )
{
	int iMagicCode = rkInfo.m_iMagicCode;
	int iMagicCodeEx = rkInfo.m_iMagicCodeEx;
	if( iMagicCode <= 0 && iMagicCodeEx <= 0 ) return;

	HideChildWnd( ID_POWERUP_BTN );
	HideChildWnd( ID_POWERUP_HIRE_BTN );
	HideChildWnd( ID_POWERUP_DISASSEMBLE_BTN );
	HideChildWnd( ID_POWERUP_DISMISSAL_BTN );

	HideChildWnd( ID_HIRE_BTN );
	HideChildWnd( ID_DISMISSAL_BTN );
	HideChildWnd( ID_DISASSEMBLE_BTN );
	HideChildWnd( ID_PCROOM_SOLDIER_CHANGE_BTN );
	HideChildWnd( ID_FREEDAY_SOLDIER_CHANGE_BTN );

	HideChildWnd( ID_MY_SOLDIER_LIST_BTN );
	HideChildWnd( ID_MY_SOLDIER_LEFT_BTN );
	HideChildWnd( ID_MY_SOLDIER_RIGHT_BTN );
	HideChildWnd( ID_INNER_TAB1 );
	HideChildWnd( ID_INNER_TAB2 );
	HideChildWnd( ID_ETC_SHOP_BTN );
	ShowChildWnd( ID_ITEM_SELL_BTN );
	HideChildWnd( ID_ETC_USE_BTN );
	HideChildWnd( ID_ETC_PRESENT_SEND_BTN );
	HideChildWnd( ID_ETC_USE_SMALL_BTN );
	ShowChildWnd( ID_ITEM_ICON );
	ShowChildWnd( ID_ITEM_RECV );
	HideChildWnd( ID_ITEM_RETR );
	HideChildWnd( ID_PRESENT_DISASSEMBLE_BTN );

	HideChildWnd( ID_KINDRED_BTN );
	HideChildWnd( ID_HAIR_BTN );
	HideChildWnd( ID_HAIR_COLOR_BTN );
	HideChildWnd( ID_FACE_BTN );
	HideChildWnd( ID_SKIN_COLOR_BTN );
	HideChildWnd( ID_UNDERWEAR_BTN );

	HideChildWnd( ID_ETC_SOULSTONE_USE_BTN );
	HideChildWnd( ID_ETC_SOULSTONE_ENABLE_LIST );

	HideChildWnd( ID_SOLDIER_WND );
	HideChildWnd( ID_COSTUME_SOLDIER_WND );

	// UI코스튬 관련 (코스튬 버튼 해제)
	HideChildWnd( ID_WEAPON_COSTUME_BTN );
	HideChildWnd( ID_ARMOR_COSTUME_BTN );
	HideChildWnd( ID_HELMET_COSTUME_BTN );
	HideChildWnd( ID_CLOAK_COSTUME_BTN );

	SetChildActive( ID_ITEM_SELL_BTN );

	ioHashString szSendID;
	short sPresentType, sPresentMent;
	int iPresentValue1, iPresentValue2, iPresentValue3, iPresentValue4;
	DWORD dwLimitDate;

	g_PresentMgr.GetPresentDataToIndex( iMagicCode,
										iMagicCodeEx,
										szSendID,
										sPresentType,
										sPresentMent,
										iPresentValue1,
										iPresentValue2,
										iPresentValue3,
										iPresentValue4,
										dwLimitDate );

	m_dwDefaultActionBtn = ID_ITEM_RECV;

	// 아이콘
	switch( sPresentType )
	{
	case PRESENT_SOLDIER:
		if( g_MyInfo.IsCharMortmain( iPresentValue1 ) && iPresentValue2 == 0 )
		{
			HideChildWnd( ID_ITEM_RECV );
			ShowChildWnd( ID_PRESENT_DISASSEMBLE_BTN );
		}
		SetPresentSoldier( iPresentValue1, iPresentValue2 );
		break;
	case PRESENT_DECORATION:
		SetPresentDeco( iPresentValue1, iPresentValue2 );
		break;
	case PRESENT_ETC_ITEM:
		SetPresentEtcItem( iPresentValue1, iPresentValue2 );
		break;
	case PRESENT_PESO:
		SetPresentPeso( iPresentValue1, iPresentValue2 );
		break;
	case PRESENT_EXTRAITEM:
		SetPresentExtraItem( iPresentValue1, iPresentValue2, iPresentValue3, iPresentValue4, sPresentMent );
		break;
	case PRESENT_EXTRAITEM_BOX:
		SetPresentExtraItemBox( iPresentValue1, iPresentValue2 );
		break;
	case PRESENT_RANDOM_DECO:
		SetPresentRandomDeco( iPresentValue1, iPresentValue2 );
		break;
	case PRESENT_GRADE_EXP:
		SetPresentGradeExp( iPresentValue1, iPresentValue2 );
		break;
	case PRESENT_MEDALITEM:
		SetPresentMedalItem( iPresentValue1, iPresentValue2 );
		break;
	case PRESENT_ALCHEMIC_ITEM:
		break;
	case PRESENT_PET:
		SetPresentPet( iPresentValue1, iPresentValue2 );
		break;
	case PRESENT_COSTUME:
		SetPresentCostume( iPresentValue1, iPresentValue2 );
		break;
	case PRESENT_COSTUME_BOX:
		SetPresentCostumeBox( iPresentValue1, iPresentValue2 );
		break;
	case PRESENT_BONUS_CASH:
		SetPresentBonusCash( iPresentValue1, iPresentValue2 );
		break;
	case PRESENT_ACCESSORY:
		SetPresentAccessory( iPresentValue1, iPresentValue2 );
		break;
	case PRESENT_SPIRIT:
		SetPresentSpiritItem( iPresentValue1, iPresentValue2 );
		break;
	}

	// 첫 라인 : 이름
	m_PresentInfo[0].SetTextStyle( TS_NORMAL );
	m_PresentInfo[0].SetBkColor( 0, 0, 0 );	
	m_PresentInfo[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
	m_PresentInfo[0].AddTextPiece( FONT_SIZE_17, g_PresentMgr.GetPresentValue1Text( sPresentType, iPresentValue1, iPresentValue2 ).c_str() );

	// 두번째 라인 : 기간, 갯수 등등 추가 정보 
	m_PresentInfo[1].SetTextStyle( TS_NORMAL );
	m_PresentInfo[1].SetBkColor( 0, 0, 0 );
	m_PresentInfo[1].SetTextColor( TCT_DEFAULT_BLUE );

	char szUseInfo[MAX_PATH]="";
	if( sPresentType == PRESENT_MEDALITEM )
		SafeSprintf( szUseInfo, sizeof( szUseInfo ), STR(1),  g_PresentMgr.GetPresentValue2Text( sPresentType, iPresentValue1, iPresentValue2 ).c_str() );
	else if( sPresentType == PRESENT_PET )
		SafeSprintf( szUseInfo, sizeof( szUseInfo ), "Lv %d %s", iPresentValue2/10000, g_PresentMgr.GetPresentValue2Text( sPresentType, iPresentValue1, iPresentValue2 ).c_str() );
	else
		StringCbCopy( szUseInfo, sizeof( szUseInfo ), g_PresentMgr.GetPresentValue2Text( sPresentType, iPresentValue1, iPresentValue2 ).c_str() );
	m_PresentInfo[1].AddTextPiece( FONT_SIZE_17,  szUseInfo );

	m_pManual = ioPresentInvenManualMgr::GetSingletonPtr();
}

void MyInventoryInfoWnd::SetSubscriptionItemInfo( int iClassType, const InventoryItemInfo &rkInfo )
{
	int iMagicCode = rkInfo.m_iMagicCode;
	ioHashString szSubscriptionID = rkInfo.m_szSubscriptionID;
	if( iMagicCode <= 0 ) return;

	HideChildWnd( ID_POWERUP_BTN );
	HideChildWnd( ID_POWERUP_HIRE_BTN );
	HideChildWnd( ID_POWERUP_DISASSEMBLE_BTN );
	HideChildWnd( ID_POWERUP_DISMISSAL_BTN );

	HideChildWnd( ID_HIRE_BTN );
	HideChildWnd( ID_DISMISSAL_BTN );
	HideChildWnd( ID_DISASSEMBLE_BTN );
	HideChildWnd( ID_PCROOM_SOLDIER_CHANGE_BTN );
	HideChildWnd( ID_FREEDAY_SOLDIER_CHANGE_BTN );

	HideChildWnd( ID_MY_SOLDIER_LIST_BTN );
	HideChildWnd( ID_MY_SOLDIER_LEFT_BTN );
	HideChildWnd( ID_MY_SOLDIER_RIGHT_BTN );
	HideChildWnd( ID_INNER_TAB1 );
	HideChildWnd( ID_INNER_TAB2 );
	HideChildWnd( ID_ETC_SHOP_BTN );
	HideChildWnd( ID_ITEM_SELL_BTN );
	HideChildWnd( ID_ETC_USE_BTN );
	HideChildWnd( ID_ETC_PRESENT_SEND_BTN );
	HideChildWnd( ID_ETC_USE_SMALL_BTN );
	ShowChildWnd( ID_ITEM_ICON );
	ShowChildWnd( ID_ITEM_RECV );
	ShowChildWnd( ID_ITEM_RETR );
	HideChildWnd( ID_PRESENT_DISASSEMBLE_BTN );

	HideChildWnd( ID_KINDRED_BTN );
	HideChildWnd( ID_HAIR_BTN );
	HideChildWnd( ID_HAIR_COLOR_BTN );
	HideChildWnd( ID_FACE_BTN );
	HideChildWnd( ID_SKIN_COLOR_BTN );
	HideChildWnd( ID_UNDERWEAR_BTN );

	HideChildWnd( ID_ETC_SOULSTONE_USE_BTN );
	HideChildWnd( ID_ETC_SOULSTONE_ENABLE_LIST );

	HideChildWnd( ID_SOLDIER_WND );
	HideChildWnd( ID_COSTUME_SOLDIER_WND );

	// UI코스튬 관련 (코스튬 버튼 해제)
	HideChildWnd( ID_WEAPON_COSTUME_BTN );
	HideChildWnd( ID_ARMOR_COSTUME_BTN );
	HideChildWnd( ID_HELMET_COSTUME_BTN );
	HideChildWnd( ID_CLOAK_COSTUME_BTN );

	ioSubscriptionMgr::SubscriptionData kData;
	g_SubscriptionMgr.GetSubscriptionDataToIndex( iMagicCode, szSubscriptionID, kData );

	if ( kData.m_iSubscriptionBonusGold > 0 )
		SetChildInActive( ID_ITEM_RETR );
	else
		SetChildActive( ID_ITEM_RETR );
	
	m_dwDefaultActionBtn = ID_ITEM_RECV;

	// 아이콘
	switch( kData.m_iPresentType )
	{
	case PRESENT_SOLDIER:
		if( g_MyInfo.IsCharMortmain( kData.m_iPresentValue1 ) && kData.m_iPresentValue2 == 0 )
		{
			HideChildWnd( ID_ITEM_RECV );
			ShowChildWnd( ID_PRESENT_DISASSEMBLE_BTN );
		}
		SetPresentSoldier( kData.m_iPresentValue1, kData.m_iPresentValue2 );
		break;
	case PRESENT_DECORATION:
		SetPresentDeco( kData.m_iPresentValue1, kData.m_iPresentValue2 );
		break;
	case PRESENT_ETC_ITEM:
		SetPresentEtcItem( kData.m_iPresentValue1, kData.m_iPresentValue2 );
		break;
	case PRESENT_PESO:
		SetPresentPeso( kData.m_iPresentValue1, kData.m_iPresentValue2 );
		break;
	case PRESENT_EXTRAITEM:
		SetPresentExtraItem( kData.m_iPresentValue1, kData.m_iPresentValue1, 0, 0, kData.m_iMentType );
		break;
	case PRESENT_EXTRAITEM_BOX:
		SetPresentExtraItemBox( kData.m_iPresentValue1, kData.m_iPresentValue2 );
		break;
	case PRESENT_RANDOM_DECO:
		SetPresentRandomDeco( kData.m_iPresentValue1, kData.m_iPresentValue2 );
		break;
	case PRESENT_GRADE_EXP:
		SetPresentGradeExp( kData.m_iPresentValue1, kData.m_iPresentValue2 );
		break;
	case PRESENT_MEDALITEM:
		SetPresentMedalItem( kData.m_iPresentValue1, kData.m_iPresentValue2 );
		break;
	case PRESENT_ALCHEMIC_ITEM:
		break;
	case PRESENT_PET:
		SetPresentPet( kData.m_iPresentValue1, kData.m_iPresentValue2 );
		break;
	case PRESENT_COSTUME:
		SetPresentCostume( kData.m_iPresentValue1, kData.m_iPresentValue2 );
		break;
	case PRESENT_COSTUME_BOX:
		SetPresentCostumeBox( kData.m_iPresentValue1, kData.m_iPresentValue2 );
		break;
	case PRESENT_BONUS_CASH:
		SetPresentBonusCash( kData.m_iPresentValue1, kData.m_iPresentValue2 );
		break;
	case PRESENT_SPIRIT:
		SetPresentSpiritItem( kData.m_iPresentValue1, kData.m_iPresentValue2 );
		break;
	}

	ioHashString szValueText1 = g_PresentMgr.GetPresentValue1Text( kData.m_iPresentType, kData.m_iPresentValue1, kData.m_iPresentValue2 );
	ioHashString szValueText2 = g_PresentMgr.GetPresentValue2Text( kData.m_iPresentType, kData.m_iPresentValue1, kData.m_iPresentValue2 );

	// 첫 라인 : 이름
	m_PresentInfo[0].SetTextStyle( TS_NORMAL );
	m_PresentInfo[0].SetBkColor( 0, 0, 0 );	
	m_PresentInfo[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
	m_PresentInfo[0].AddTextPiece( FONT_SIZE_17, szValueText1.c_str() );

	// 두번째 라인 : 기간, 갯수 등등 추가 정보 
	m_PresentInfo[1].SetTextStyle( TS_NORMAL );
	m_PresentInfo[1].SetBkColor( 0, 0, 0 );
	m_PresentInfo[1].SetTextColor( TCT_DEFAULT_BLUE );

	char szUseInfo[MAX_PATH]="";
	if( kData.m_iPresentType == PRESENT_MEDALITEM )
		SafeSprintf( szUseInfo, sizeof( szUseInfo ), STR(1),  szValueText2.c_str() );
	else
		StringCbCopy( szUseInfo, sizeof( szUseInfo ), szValueText2.c_str() );
	m_PresentInfo[1].AddTextPiece( FONT_SIZE_17,  szUseInfo );

	// 청약 정보
	char szDateInfo[MAX_PATH]="";
	char szGoldInfo[MAX_PATH]="";
	char szBonusGoldInfo[MAX_PATH]="";

	short iYear   = 0;
	short iMonth  = 0;
	short iDay    = 0;
	short iHour   = 0;
	short iMinute = 0;

	time_t kBuyTime;
	if( g_SubscriptionMgr.CheckBuyDate( kData.m_dwLimitDate, kBuyTime ) )
	{
		struct tm pTime;
		localtime_s(&pTime, &kBuyTime);

		iYear   = (short)( pTime.tm_year + 1900 );
		iMonth  = (short)( pTime.tm_mon + 1 );
		iDay    = (short)( pTime.tm_mday );
		iHour   = (short)( pTime.tm_hour );
		iMinute = (short)( pTime.tm_min );
	}

	wsprintf( szDateInfo, "%d/%02d/%02d %02d:%02d", iYear, iMonth, iDay, iHour, iMinute );

	Help::ConvertNumToStrComma( kData.m_iSubscriptionGold, szGoldInfo, sizeof( szGoldInfo ) );
	Help::ConvertNumToStrComma( kData.m_iSubscriptionBonusGold, szBonusGoldInfo, sizeof( szBonusGoldInfo ) );

	m_SubscriptionInfo[0].SetTextStyle( TS_NORMAL );
	m_SubscriptionInfo[0].SetBkColor( 0, 0, 0 );
	m_SubscriptionInfo[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
	m_SubscriptionInfo[0].AddTextPiece( FONT_SIZE_12, "★구매날짜: %s", szDateInfo );

	m_SubscriptionInfo[1].SetTextStyle( TS_NORMAL );
	m_SubscriptionInfo[1].SetBkColor( 0, 0, 0 );
	m_SubscriptionInfo[1].SetTextColor( TCT_DEFAULT_DARKGRAY );
	m_SubscriptionInfo[1].AddTextPiece( FONT_SIZE_12, "★구매골드: %s", szGoldInfo );

	m_SubscriptionInfo[2].SetTextStyle( TS_NORMAL );
	m_SubscriptionInfo[2].SetBkColor( 0, 0, 0 );
	m_SubscriptionInfo[2].SetTextColor( TCT_DEFAULT_DARKGRAY );
	m_SubscriptionInfo[2].AddTextPiece( FONT_SIZE_12, "★구매보너스골드: %s", szBonusGoldInfo );

	m_pManual = ioPresentInvenManualMgr::GetSingletonPtr();
}

// UI코스튬 관련 (코스튬 장착)
void MyInventoryInfoWnd::SetCostumeInfo( int iClassType, const InventoryItemInfo &rkInfo )
{
	HideChildWnd( ID_HIRE_BTN );
	HideChildWnd( ID_DISMISSAL_BTN );
	HideChildWnd( ID_DISASSEMBLE_BTN );
	HideChildWnd( ID_PCROOM_SOLDIER_CHANGE_BTN );
	HideChildWnd( ID_FREEDAY_SOLDIER_CHANGE_BTN );

	HideChildWnd( ID_ETC_SHOP_BTN );
	HideChildWnd( ID_ITEM_SELL_BTN );
	HideChildWnd( ID_ETC_USE_BTN );
	HideChildWnd( ID_ETC_PRESENT_SEND_BTN );
	HideChildWnd( ID_ETC_USE_SMALL_BTN );
	HideChildWnd( ID_ITEM_ICON );
	HideChildWnd( ID_ITEM_RECV );
	HideChildWnd( ID_ITEM_RETR );
	HideChildWnd( ID_PRESENT_DISASSEMBLE_BTN );

	HideChildWnd( ID_KINDRED_BTN );
	HideChildWnd( ID_HAIR_BTN );
	HideChildWnd( ID_HAIR_COLOR_BTN );
	HideChildWnd( ID_FACE_BTN );
	HideChildWnd( ID_SKIN_COLOR_BTN );
	HideChildWnd( ID_UNDERWEAR_BTN );

	HideChildWnd( ID_POWERUP_BTN );
	HideChildWnd( ID_POWERUP_HIRE_BTN );
	HideChildWnd( ID_POWERUP_DISASSEMBLE_BTN );
	HideChildWnd( ID_POWERUP_DISMISSAL_BTN );
	
	HideChildWnd( ID_SOLDIER_WND );
	HideChildWnd( ID_COSTUME_SOLDIER_WND );
	ShowChildWnd( ID_INNER_TAB1 );
	HideChildWnd( ID_INNER_TAB2 );

	// UI코스튬 관련 (코스튬 버튼 해제)
	HideChildWnd( ID_WEAPON_COSTUME_BTN );
	ShowChildWnd( ID_ARMOR_COSTUME_BTN );
	ShowChildWnd( ID_HELMET_COSTUME_BTN );
	ShowChildWnd( ID_CLOAK_COSTUME_BTN );

	int iCharArray = g_MyInfo.GetClassArray( iClassType );
	if( COMPARE( iCharArray, 0, g_MyInfo.GetCharCount() ) )
		m_CharInfo = g_MyInfo.GetCharacter( iCharArray );
	else
		Help::GetDefaultCharInfo( m_CharInfo, iClassType );
	
	// 코스튬 관련 (장비 장착시 캐릭터)
	NewShopHelpWnd *pCharWnd = dynamic_cast<NewShopHelpWnd *>( FindChildWnd( ID_COSTUME_SOLDIER_WND ));
	if( pCharWnd )
	{
		pCharWnd->SetWndPos( 54, 145 );
		pCharWnd->SetZPosition( 470.0f );
		pCharWnd->SetChar( m_CharInfo );
		pCharWnd->SetEquip( false, false, true );
		pCharWnd->SetUserInfoToolTip( true );
		pCharWnd->ShowWnd();
	}

	ioWnd* pListBtn = FindChildWnd( ID_MY_SOLDIER_LIST_BTN );
	if( pListBtn )
	{
		pListBtn->ShowWnd();
		pListBtn->SetWndPos( 51, 97 );
	}
	ioWnd* pLeftBtn = FindChildWnd( ID_MY_SOLDIER_LEFT_BTN );
	if( pLeftBtn )
	{
		pLeftBtn->ShowWnd();
		pLeftBtn->SetWndPos( 27, 97 );
	}
	ioWnd* pRightBtn = FindChildWnd( ID_MY_SOLDIER_RIGHT_BTN );
	if( pRightBtn )
	{
		pRightBtn->ShowWnd();
		pRightBtn->SetWndPos( 203, 97 );
	}

	for ( int i=0; i<MAX_INVENTORY; i++ )
		SetCostumeSubInfo( i+ID_WEAPON_COSTUME_BTN, i, MY_INVENTORY_INFO_EXTRA_SCALE, MY_INVENTORY_INFO_BACK_SCALE, MY_INVENTORY_INFO_DISABLE_SCALE );
}

// UI코스튬 관련 (장착 버튼 설정)
void MyInventoryInfoWnd::SetCostumeSubInfo( DWORD dwWndID, int nSlotNum, float fScale, float fBackScale, float fDisableScale )
{
	ioUserCostume *pCostume = g_MyInfo.GetUserCostume();
	if ( !pCostume )
		return;

	SlotIconBtn *pBtn = dynamic_cast<SlotIconBtn*>(FindChildWnd(dwWndID));
	if( !pBtn )
		return;

	if ( !COMPARE( nSlotNum, 0, MAX_INVENTORY ) )
		return;

	int nSlotIndex = m_CharInfo.m_EquipCostume[nSlotNum].m_CostumeIdx;
	ioUIRenderImage *pImg = NULL;

	CostumeSlot kSlot;	
	if( m_CharInfo.m_chExerciseStyle == EXERCISE_RENTAL )
	{
		DWORD dwCharIndex = g_MyInfo.GetCharIndexByClassType( m_CharInfo.m_class_type );
		ioCharRentalData *pCharRentalData = g_MyInfo.GetCharRentalData();
		if( pCharRentalData && pCharRentalData->GetEquipCostume( dwCharIndex, kSlot, nSlotNum ) )
		{
			pImg = g_UIImageSetMgr.CreateImageByFullName( kSlot.m_IconName );

			pBtn->SetIcon( pImg, fScale, fBackScale, fDisableScale, kSlot.m_PeriodType, 0, kSlot.m_dwMaleCustom, kSlot.m_dwFemaleCustom, true );
			pBtn->SetMagicCode( -(nSlotNum+1) );
			pBtn->SetMagicCodeEx( kSlot.m_nCostumeCode );
		}
		else
		{
			pBtn->SetIcon( pImg, fScale, fBackScale, fDisableScale, kSlot.m_PeriodType, 0, kSlot.m_dwMaleCustom, kSlot.m_dwFemaleCustom );
			pBtn->SetMagicCode( 0 );
		}
	}
	else if( nSlotIndex > 0 && pCostume->GetCostumeSlot( nSlotIndex, kSlot ) )
	{
		pImg = g_UIImageSetMgr.CreateImageByFullName( kSlot.m_IconName );
		pBtn->SetIcon( pImg, fScale, fBackScale, fDisableScale, kSlot.m_PeriodType, 0, 0, 0, true );
		pBtn->SetMagicCode( nSlotIndex );
	}
	else
	{
		pBtn->SetIcon( pImg, fScale, fBackScale, fDisableScale, kSlot.m_PeriodType );
		pBtn->SetMagicCode( 0 );
	}

	pBtn->SetGradeType( g_CostumeInfoMgr.GetGradeType( kSlot.m_nCostumeCode ) );
}

void MyInventoryInfoWnd::CheckDefaultActionBtn()
{
	if( m_dwDefaultActionBtn == 0 )
		return;

	CheckRecvBtnAction();
}

void MyInventoryInfoWnd::ShowSubscriptionRetrCheckLimit()
{
	ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
	kPrinter[0].SetTextStyle( TS_NORMAL );
	kPrinter[0].SetBkColor( 0, 0, 0 );	
	kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter[0].AddTextPiece( FONT_SIZE_13, "본 상품은 구매한지" );

	kPrinter[1].SetTextStyle( TS_NORMAL );
	kPrinter[1].SetBkColor( 0, 0, 0 );	
	kPrinter[1].SetTextColor( TCT_DEFAULT_RED ); 
	kPrinter[1].AddTextPiece( FONT_SIZE_13,"7일이 경과" );
	kPrinter[1].SetTextColor( TCT_DEFAULT_DARKGRAY ); 
	kPrinter[1].AddTextPiece( FONT_SIZE_13,"하여 " );
	kPrinter[1].SetTextColor( TCT_DEFAULT_RED ); 
	kPrinter[1].AddTextPiece( FONT_SIZE_13,"청약철회가" );

	kPrinter[2].SetTextStyle( TS_NORMAL );
	kPrinter[2].SetBkColor( 0, 0, 0 );	
	kPrinter[2].SetTextColor( TCT_DEFAULT_RED ); 
	kPrinter[2].AddTextPiece( FONT_SIZE_13, "되지 않습니다." );

	g_GUIMgr.SetPrevMsgListBox( NULL, MB_OK, this, kPrinter );
}

void MyInventoryInfoWnd::ShowSubscriptionRetrCheck( int iIndex, const ioHashString& szSubscriptionID, int iSubscriptionGold, int iRetraGold )
{
	ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];

	kPrinter[0].SetTextStyle( TS_NORMAL );
	kPrinter[0].SetBkColor( 0, 0, 0 );	
	kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter[0].AddTextPiece( FONT_SIZE_13, "청약철회 하시겠습니까?" );

	kPrinter[1].SetTextStyle( TS_NORMAL );
	kPrinter[1].SetBkColor( 0, 0, 0 );
	kPrinter[1].SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter[1].AddTextPiece( FONT_SIZE_13, "보너스 골드 등과 같은 경우엔 정책에 따라" );

	kPrinter[2].SetTextStyle( TS_NORMAL );
	kPrinter[2].SetBkColor( 0, 0, 0 );	
	kPrinter[2].SetTextColor( TCT_DEFAULT_GRAY ); 
	kPrinter[2].AddTextPiece( FONT_SIZE_13, "환불 금액에 포함되지 않을 수도 있습니다." );

	g_GUIMgr.SetPrevMsgListBox( NULL, MB_YESNO, this, kPrinter );
}

void MyInventoryInfoWnd::CheckRecvBtnAction()
{
	switch( m_iMainType )
	{
	case INVENTORY_ITEM_PRESENT:
		{
			int iPresentIndex = m_ItemInfo.m_iMagicCode;
			int iPresentSlot  = m_ItemInfo.m_iMagicCodeEx;

			ioHashString szSendID;
			short sPresentType, sPresentMent;
			int iPresentValue1, iPresentValue2, iPresentValue3, iPresentValue4;
			DWORD dwLimitDate;

			g_PresentMgr.GetPresentDataToIndex( iPresentIndex, iPresentSlot, szSendID, sPresentType, sPresentMent, iPresentValue1, iPresentValue2, iPresentValue3, iPresentValue4, dwLimitDate );

			ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
			if( pLocal && pLocal->SetPresentRecvDisableMsg( sPresentType ) )
				return;
					
			if( sPresentType == PRESENT_EXTRAITEM || sPresentType == PRESENT_EXTRAITEM_BOX )
			{
				// 장비 제한 체크
				if( g_MyInfo.IsExtraItemFull( true ) )
					return;
			}
			else if( sPresentType == PRESENT_DECORATION || sPresentType == PRESENT_RANDOM_DECO )
			{
				// 치장 제한 체크
				if( g_MyInfo.IsDecorationFull( true ) )
					return;
			}
			else if( sPresentType == PRESENT_MEDALITEM )
			{
				if( g_MyInfo.IsMedalItemFull( true ) )
					return;
			}
			else if ( sPresentType == PRESENT_PET )
			{
				if ( g_MyInfo.IsPetFull( true ) )
					return;
			}
			else if ( sPresentType == PRESENT_COSTUME || sPresentType == PRESENT_COSTUME_BOX )
			{
				// UI코스튬 관련 (코스튬 슬롯 확인)
				if ( g_MyInfo.IsCostumeFull( true ) )
					return;
			}

			// 선물 받기 UI					
			if( sPresentType == PRESENT_RANDOM_DECO )
			{
				MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
				if( pInvenWnd )
					pInvenWnd->ShowDecoSelectWnd( iPresentIndex, iPresentSlot );
			}
			else if( sPresentType == PRESENT_ETC_ITEM )
			{
				ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( iPresentValue1 );
				if( !pEtcItem ) return;

				if( pEtcItem->IsDecorationMaxCheck() )
				{
					if( g_MyInfo.IsDecorationFull( true ) )
						return;
				}
						
				if( pEtcItem->IsExtraItemMaxCheck() )
				{
					if( g_MyInfo.IsExtraItemFull( true ) )
						return;
				}

				if( pEtcItem->IsMedalItemMaxCheck() )
				{
					if( g_MyInfo.IsMedalItemFull( true ) )
						return;
				}

				if( pEtcItem->IsCanPresentUse() )
				{
					ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
					kPrinter[0].SetTextStyle( TS_NORMAL );
					kPrinter[0].SetBkColor( 0, 0, 0 );	
					kPrinter[0].SetTextColor( TCT_DEFAULT_RED );
					kPrinter[0].AddTextPiece( FONT_SIZE_13, STR(2) );

					kPrinter[1].SetTextStyle( TS_NORMAL );
					kPrinter[1].SetBkColor( 0, 0, 0 );	
					kPrinter[1].SetTextColor( TCT_DEFAULT_DARKGRAY ); 
					kPrinter[1].AddTextPiece( FONT_SIZE_13,STR(3) );

					kPrinter[2].SetTextStyle( TS_NORMAL );
					kPrinter[2].SetBkColor( 0, 0, 0 );	
					kPrinter[2].SetTextColor( TCT_DEFAULT_DARKGRAY ); 
					kPrinter[2].AddTextPiece( FONT_SIZE_13, STR(4) );

					kPrinter[3].SetTextStyle( TS_NORMAL );
					kPrinter[3].SetBkColor( 0, 0, 0 );	
					kPrinter[3].SetTextColor( TCT_DEFAULT_DARKGRAY );
					kPrinter[3].AddTextPiece( FONT_SIZE_13, STR(5) );

					g_GUIMgr.SetPrevMsgListBox( NULL, MB_YESNO, this, kPrinter );
				}
				else if( iPresentIndex != 0 || iPresentSlot != 0 )
				{
					SP2Packet kPacket( CTPK_PRESENT_RECV );
					kPacket << iPresentIndex << iPresentSlot;
					TCPNetwork::SendToServer( kPacket );
					TCPNetwork::MouseBusy( true );
				}
			}
			else if( sPresentType == PRESENT_MEDALITEM )
			{
				const ioMedalItemInfoManager::ItemInfo *pItem = g_MedalItemMgr.GetItemInfo( iPresentValue1 );
				if( pItem && pItem->m_iSubMedalType == ioMedalItemInfoManager::MEDALTYPE_CUSTOM )
				{
					ioNewMedalRandomBox* pWnd = dynamic_cast<ioNewMedalRandomBox*>( g_GUIMgr.FindWnd( NEW_MEDAL_RANDOM_WND ) );
					if( pWnd )
						pWnd->ShowSelectStatus( iPresentIndex, iPresentSlot, iPresentValue1 );
				}
				else if( iPresentIndex != 0 || iPresentSlot != 0 )
				{
					SP2Packet kPacket( CTPK_PRESENT_RECV );
					kPacket << iPresentIndex << iPresentSlot << NORMAL_MEDAL;
					TCPNetwork::SendToServer( kPacket );
					TCPNetwork::MouseBusy( true );
				}
			}
			else
			{
				if( iPresentIndex != 0 || iPresentSlot != 0 )
				{
					SP2Packet kPacket( CTPK_PRESENT_RECV );
					kPacket << iPresentIndex << iPresentSlot;
					TCPNetwork::SendToServer( kPacket );
					TCPNetwork::MouseBusy( true );
				}
			}
		}
		break;
	case INVENTORY_ITEM_SUBSCRIPTION:
		{
			int iIndex = m_ItemInfo.m_iMagicCode;
			ioHashString szSubscriptionID = m_ItemInfo.m_szSubscriptionID;

			ioSubscriptionMgr::SubscriptionData kData;
			g_SubscriptionMgr.GetSubscriptionDataToIndex( iIndex, szSubscriptionID, kData );

			ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
			if( pLocal && pLocal->SetPresentRecvDisableMsg( kData.m_iPresentType ) )
				return;
					
			if( kData.m_iPresentType == PRESENT_EXTRAITEM || kData.m_iPresentType == PRESENT_EXTRAITEM_BOX )
			{
				// 장비 제한 체크
				if( g_MyInfo.IsExtraItemFull( true ) )
					return;
			}
			else if( kData.m_iPresentType == PRESENT_DECORATION || kData.m_iPresentType == PRESENT_RANDOM_DECO )
			{
				// 치장 제한 체크
				if( g_MyInfo.IsDecorationFull( true ) )
					return;
			}
			else if( kData.m_iPresentType == PRESENT_MEDALITEM )
			{
				if( g_MyInfo.IsMedalItemFull( true ) )
					return;
			}
			else if ( kData.m_iPresentType == PRESENT_PET )
			{
				if ( g_MyInfo.IsPetFull( true ) )
					return;
			}
			else if ( kData.m_iPresentType == PRESENT_COSTUME || kData.m_iPresentType == PRESENT_COSTUME_BOX )
			{
				// UI코스튬 관련 (코스튬 슬롯 확인)
				if ( g_MyInfo.IsCostumeFull( true ) )
					return;
			}

			// 선물 받기 UI					
			if( kData.m_iPresentType == PRESENT_RANDOM_DECO )
			{
				MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
				if( pInvenWnd )
					pInvenWnd->ShowDecoSelectWnd( iIndex, szSubscriptionID );
			}
			else if( kData.m_iPresentType == PRESENT_ETC_ITEM )
			{
				ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( kData.m_iPresentValue1 );
				if( !pEtcItem ) return;

				if( pEtcItem->IsDecorationMaxCheck() )
				{
					if( g_MyInfo.IsDecorationFull( true ) )
						return;
				}
						
				if( pEtcItem->IsExtraItemMaxCheck() )
				{
					if( g_MyInfo.IsExtraItemFull( true ) )
						return;
				}

				if( pEtcItem->IsMedalItemMaxCheck() )
				{
					if( g_MyInfo.IsMedalItemFull( true ) )
						return;
				}

				if( pEtcItem->IsCanPresentUse() )
				{
					ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
					kPrinter[0].SetTextStyle( TS_NORMAL );
					kPrinter[0].SetBkColor( 0, 0, 0 );	
					kPrinter[0].SetTextColor( TCT_DEFAULT_RED );
					kPrinter[0].AddTextPiece( FONT_SIZE_13, STR(2) );

					kPrinter[1].SetTextStyle( TS_NORMAL );
					kPrinter[1].SetBkColor( 0, 0, 0 );	
					kPrinter[1].SetTextColor( TCT_DEFAULT_DARKGRAY ); 
					kPrinter[1].AddTextPiece( FONT_SIZE_13,STR(3) );

					kPrinter[2].SetTextStyle( TS_NORMAL );
					kPrinter[2].SetBkColor( 0, 0, 0 );	
					kPrinter[2].SetTextColor( TCT_DEFAULT_DARKGRAY ); 
					kPrinter[2].AddTextPiece( FONT_SIZE_13, STR(4) );

					kPrinter[3].SetTextStyle( TS_NORMAL );
					kPrinter[3].SetBkColor( 0, 0, 0 );	
					kPrinter[3].SetTextColor( TCT_DEFAULT_DARKGRAY );
					kPrinter[3].AddTextPiece( FONT_SIZE_13, STR(5) );

					g_GUIMgr.SetPrevMsgListBox( NULL, MB_YESNO, this, kPrinter );
				}
				else
				{
					SP2Packet kPacket( CTPK_SUBSCRIPTION_RECV );
					kPacket << iIndex << szSubscriptionID;
					TCPNetwork::SendToServer( kPacket );
					TCPNetwork::MouseBusy( true );
				}
			}
			else
			{
				SP2Packet kPacket( CTPK_SUBSCRIPTION_RECV );
				kPacket << iIndex << szSubscriptionID;
				TCPNetwork::SendToServer( kPacket );
				TCPNetwork::MouseBusy( true );
			}
		}
		break;
	}
}

void MyInventoryInfoWnd::CheckRetractBtn()
{
	switch( m_iMainType )
	{
	case INVENTORY_ITEM_SUBSCRIPTION:
		{
			if( g_SubscriptionMgr.CheckSubscriptionRetractLimit( m_ItemInfo.m_iMagicCode, m_ItemInfo.m_szSubscriptionID ) )
			{
				// 서버로 전송
				SP2Packet kPacket( CTPK_SUBSCRIPTION_RETR_CHECK );
				kPacket << m_ItemInfo.m_iMagicCode << m_ItemInfo.m_szSubscriptionID;
				TCPNetwork::SendToServer( kPacket );
				TCPNetwork::MouseBusy( true );
			}
			else
			{
				ShowSubscriptionRetrCheckLimit();
			}
		}
		break;
	}
}

void MyInventoryInfoWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	enum 
	{
		DESC_X_OFFSET	= 27,
		DESC_Y_OFFSET	= 226,
		GAP_OFFSET		= 40,
	};

	RenderSoldierInfo();
	RenderDecoInfo();
	RenderEtcInfo();
	RenderPresentInfo();

	if( m_iMainType == INVENTORY_ITEM_SUBSCRIPTION )
		PrintManual( iXPos+DESC_X_OFFSET, iYPos+DESC_Y_OFFSET+GAP_OFFSET, FONT_SIZE_12 );
	else
		PrintManual( iXPos+DESC_X_OFFSET, iYPos+DESC_Y_OFFSET, FONT_SIZE_12 );
}

void MyInventoryInfoWnd::RenderSoldierInfo()
{
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	enum 
	{
		TITLE_X_OFFSET	= 129,
		TITLE_Y_OFFSET	= 357,

		LINE_GAP		= 23,

		EXP_TEXT_X		= 128,
		EXP_TEXT_Y		= 404,

		EXP_MAIN_X		= 55,
		EXP_MAIN_Y		= 419,

		EXP_GAUGE_X		= 57,
		EXP_GAUGE_Y		= 421,
	};

	char szText[MAX_PATH]="";
	DWORD dwColor = TCT_DEFAULT_RED;
	int iCharArray = g_MyInfo.GetClassArray( m_iClassType );

	ioHashString szPowerupName;
	const ioClassExpert::CLASSSLOT* pClassSlot = g_MyInfo.GetClassExpertSlot( m_iClassType );
	if ( pClassSlot && pClassSlot->m_ePowerUpCharGrade != PUGT_NONE )
		szPowerupName = g_PowerUpManager.GetCharGradeTitle( m_iClassType, pClassSlot->m_ePowerUpCharGrade );

	if( g_MyInfo.IsCharExerciseStyle( iCharArray, EXERCISE_RENTAL ) )
	{
		SafeSprintf( szText, sizeof( szText ), STR(12) );
	}
	else if( g_MyInfo.IsCharMortmain( m_iClassType ) )
	{
		int iHeroGrade = g_MyInfo.GetCharPowerUpGradeByClassType( m_iClassType );
		if ( iHeroGrade == PUGT_NONE )
			SafeSprintf( szText, sizeof( szText ), STR(5) );
		else
		{
			ioHashString szGradeTitle = g_PowerUpManager.GetCharGradeTitle( m_iClassType, iHeroGrade );
			if ( !szGradeTitle.IsEmpty() )
				SafeSprintf( szText, sizeof( szText ), STR(13), szGradeTitle.c_str() );
		}
		dwColor = TCT_DEFAULT_BLUE;
	}
	else if( g_MyInfo.IsCharExerciseStyle( iCharArray, EXERCISE_GENERAL ) )
	{
		SafeSprintf( szText, sizeof( szText ), STR(6) );
	}
	else if( g_MyInfo.IsCharExerciseStyle( iCharArray, EXERCISE_PCROOM ) )
	{
		if( g_MyInfo.IsUserEvent() )
			SafeSprintf( szText, sizeof( szText ), STR(14) );
		else if( g_MyInfo.GetPCRoomAuthority() > 0 )
			SafeSprintf( szText, sizeof( szText ), STR(7) );
	}
	else if( g_MyInfo.IsCharExerciseStyle( iCharArray, EXERCISE_EVENT ) )
	{
		SafeSprintf( szText, sizeof( szText ), STR(8) );
	}
	else
	{
		int iTotalLimitMinute = g_MyInfo.GetCharTotalSecond( iCharArray ) / 60;
		int iHour     = iTotalLimitMinute / 60;      
		int iMinute   = iTotalLimitMinute % 60;

		if( iHour == 0 )
		{
			if( szPowerupName.IsEmpty() )
				SafeSprintf( szText, sizeof( szText ), STR(9), iMinute );
			else			
				SafeSprintf( szText, sizeof( szText ), "%s(%d분)", szPowerupName.c_str(), iMinute );	
		}
		else if( iMinute == 0 )
		{
			if( szPowerupName.IsEmpty() )
				SafeSprintf( szText, sizeof( szText ), STR(10), iHour );
			else
				SafeSprintf( szText, sizeof( szText ), "%s(%d시간)", szPowerupName.c_str(), iHour );
		}
		else
		{
			if( szPowerupName.IsEmpty() )
				SafeSprintf( szText, sizeof( szText ), STR(11), iHour, iMinute );
			else
				SafeSprintf( szText, sizeof( szText ), "%s(%d시간 %d분)", szPowerupName.c_str(), iHour, iMinute );
		}
	}

	m_SoldierInfo[1].ClearList();
	m_SoldierInfo[1].SetTextStyle( TS_NORMAL );
	m_SoldierInfo[1].SetBkColor( 0, 0, 0 );	
	m_SoldierInfo[1].SetTextColor( dwColor );
	m_SoldierInfo[1].AddTextPiece( FONT_SIZE_17, szText );

	for( int i=0; i < 2; ++i )
	{
		if( m_SoldierInfo[i].IsEmpty() )
			break;

		m_SoldierInfo[i].PrintFullText( iXPos+SOLDIER_TITLE_X_OFFSET, iYPos+TITLE_Y_OFFSET+(SOLDIER_LINE_GAP*i), TAT_CENTER );
	}

	if( m_bShowPowerUpToolTip )
	{
		if( m_pPowerUpToolTip )
		{
			if ( !m_SoldierInfo[1].IsEmpty())
			{
				int iToolTipXPos = iXPos + SOLDIER_TITLE_X_OFFSET + m_SoldierInfo[1].GetFullWidth()/2 + POWERUP_TOOLTIP_OFFSET_X;
				m_pPowerUpToolTip->Render( iToolTipXPos, iYPos+TITLE_Y_OFFSET + POWERUP_TOOLTIP_OFFSET_Y + SOLDIER_LINE_GAP, UI_RENDER_NORMAL_ADD_COLOR );
			}
		}
	}

	// Exp
	if( m_iNextLevelExp > 0 )
	{
		ioComplexStringPrinter kPrinter;
		kPrinter.SetTextStyle( TS_NORMAL );
		kPrinter.SetBkColor( 0, 0, 0 );

		kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter.AddTextPiece( FONT_SIZE_13, STR(1), m_iCurLevel );

		if( m_iCurLevel < 100 )
		{
			kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
			kPrinter.AddTextPiece( FONT_SIZE_13, STR(2), m_iExpRate );

			kPrinter.SetTextColor( TCT_DEFAULT_RED );
			kPrinter.AddTextPiece( FONT_SIZE_13, STR(3), m_iCurLevelExp );

			kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
			kPrinter.AddTextPiece( FONT_SIZE_13, STR(4), m_iNextLevelExp );
		}
		else
		{
			kPrinter.AddTextPiece( FONT_SIZE_13, "  " );
			kPrinter.SetTextColor( TCT_DEFAULT_RED );
			kPrinter.AddTextPiece( FONT_SIZE_13, STR(3), m_iCurLevelExp );

			kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
			kPrinter.AddTextPiece( FONT_SIZE_13, STR(4), m_iNextLevelExp );
		}

		kPrinter.PrintFullText( iXPos+EXP_TEXT_X, iYPos+EXP_TEXT_Y, TAT_CENTER );

		// ExpGauge
		if( m_pGaugeMainBack )
			m_pGaugeMainBack->Render( iXPos+EXP_MAIN_X, iYPos+EXP_MAIN_Y );

		if( m_pGaugeBack )
			m_pGaugeBack->Render( iXPos+EXP_GAUGE_X, iYPos+EXP_GAUGE_Y );

		if( m_pExpGauge && m_iCurLevelExp > 0 )
		{
			float fRate = (float)m_iExpRate / 100;
			float fCurWidth = 144 * fRate;

			m_pExpGauge->RenderWidthCut( iXPos+EXP_GAUGE_X, iYPos+EXP_GAUGE_Y, 0, fCurWidth );
		}
	}
}

void MyInventoryInfoWnd::RenderDecoInfo()
{
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	enum 
	{
		DECO_NAME_X_OFFSET = 56,
		DECO_NAME_Y_OFFSET = 388,

		X_GAP			= 72,
		Y_GAP			= 78,
	};

	for( int j=0; j < 6; ++j )
	{
		if( m_DecoName[j].IsEmpty() )
			continue;

		int iNewX, iNewY;
		if( j < 3 )
		{
			iNewX = iXPos + DECO_NAME_X_OFFSET + (X_GAP*j);
			iNewY = iYPos + DECO_NAME_Y_OFFSET;

			g_FontMgr.SetTextStyle( TS_NORMAL );
			g_FontMgr.SetAlignType( TAT_CENTER );
			g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
			g_FontMgr.PrintTextWidthCut( iNewX, iNewY, FONT_SIZE_11, 57.0f, STR(1), m_DecoName[j].c_str() );
		}
		else
		{
			iNewX = iXPos + DECO_NAME_X_OFFSET + (X_GAP*(j-3));
			iNewY = iYPos + DECO_NAME_Y_OFFSET + Y_GAP;

			g_FontMgr.SetTextStyle( TS_NORMAL );
			g_FontMgr.SetAlignType( TAT_CENTER );
			g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
			g_FontMgr.PrintTextWidthCut( iNewX, iNewY, FONT_SIZE_11, 57.0f, STR(2), m_DecoName[j].c_str() );
		}
	}
}

void MyInventoryInfoWnd::RenderEtcInfo()
{
	if( m_iMainType != INVENTORY_ITEM_ETC )
		return;

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( !m_EtcInfo.IsEmpty() )
	{
		if( COMPARE( m_ItemInfo.m_iMagicCode, ioEtcItem::EIT_ETC_SKELETON_BIG, ioEtcItem::EIT_ETC_SKELETON_SMALL + 1 ) )
			m_EtcInfo.PrintFullText( iXPos + 129, iYPos + 312, TAT_CENTER );
		else
			m_EtcInfo.PrintFullText( iXPos + 129, iYPos + 152, TAT_CENTER );
	}
}

void MyInventoryInfoWnd::RenderPresentInfo()
{
	if( m_iMainType != INVENTORY_ITEM_PRESENT && m_iMainType != INVENTORY_ITEM_SUBSCRIPTION )
		return;

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	enum 
	{
		TITLE_X_OFFSET	= 129,
		TITLE_Y_OFFSET	= 152,

		DESC_X_OFFSET	= 27,
		DESC_Y_OFFSET	= 206,

		LINE_GAP		= 18,
	};

	if( !m_PresentInfo[0].IsEmpty() )
		m_PresentInfo[0].PrintFullText( iXPos+TITLE_X_OFFSET, iYPos+TITLE_Y_OFFSET, TAT_CENTER );

	if( !m_PresentInfo[1].IsEmpty() )
		m_PresentInfo[1].PrintFullText( iXPos+TITLE_X_OFFSET, iYPos+TITLE_Y_OFFSET+23, TAT_CENTER );

	if( m_iMainType == INVENTORY_ITEM_SUBSCRIPTION )
	{
		if( !m_SubscriptionInfo[0].IsEmpty() )
			m_SubscriptionInfo[0].PrintFullText( iXPos+DESC_X_OFFSET, iYPos+DESC_Y_OFFSET, TAT_LEFT );

		if( !m_SubscriptionInfo[1].IsEmpty() )
			m_SubscriptionInfo[1].PrintFullText( iXPos+DESC_X_OFFSET, iYPos+DESC_Y_OFFSET+20, TAT_LEFT );

		if( !m_SubscriptionInfo[2].IsEmpty() )
			m_SubscriptionInfo[2].PrintFullText( iXPos+DESC_X_OFFSET, iYPos+DESC_Y_OFFSET+40, TAT_LEFT );
	}
}

void MyInventoryInfoWnd::PrintManual( int a_iXPos, int a_iYPos , float fScale )
{
	if( !m_pManual )
		return;

	ioComplexStringPrinter kPrinter;
	int iMaxLine = m_pManual->GetMaxLine( m_ItemInfo.m_iManualIndex );
	for(int iLine = 1; iLine < iMaxLine; iLine++)
	{
		m_pManual->SetPrinter( m_ItemInfo.m_iManualIndex, iLine, fScale, kPrinter);
		a_iYPos += m_pManual->GetSpaceLine( m_ItemInfo.m_iManualIndex, iLine);
		kPrinter.PrintFullTextWidthCut( a_iXPos, a_iYPos, TAT_LEFT, DESC_WIDTH );
		kPrinter.ClearList();
	}
}

void MyInventoryInfoWnd::GetItemList( DWORD dwID, vNewShopPullDownItem &rkList )
{
	rkList.clear();
	if( dwID == ID_MY_SOLDIER_LIST_BTN )
	{
		// 고용중인 용병
		int iMaxClass =  g_ClassPrice.MaxClassPrice();
		for (int i = 0; i <  iMaxClass; i++)
		{
			int iCharArray = g_MyInfo.GetCharSlotIndexToArray( i );
			if( iCharArray == -1 ) continue;
			if( g_MyInfo.IsCharExercise( iCharArray ) ) continue;       

			int iSetIdx = g_MyInfo.GetClassType( iCharArray ) - 1;
			const ioSetItemInfo *pInfo = g_SetItemInfoMgr.GetSetInfoByIdx( iSetIdx );
			if( !pInfo ) continue;

			int iClassType = pInfo->GetSetCode() - SET_ITEM_CODE;

			NewShopPullDownItem kItem;
			kItem.m_iMagicCode = iClassType;

			char szName[MAX_PATH]="";
			StringCbPrintf( szName, sizeof( szName ), "Lv%d %s", g_MyInfo.GetClassLevel( iClassType, true ), g_MyInfo.GetClassName( iClassType, g_MyInfo.GetCharPowerUpGradeByClassType( iClassType ) ) );
			kItem.m_dwTextColor = 0xFF3A3A3A;
			kItem.m_szName      = szName;
			rkList.push_back( kItem );
		}

		// 고용중이지 않은 용병 추리기
		vNewShopPullDownItem kTmpList;

		if( m_bSoldierTab )
		{
			for (int i = 0; i <  iMaxClass; i++)
			{
				int iSetIdx = g_ClassPrice.ArrayToSetItemIdx( i );
				const ioSetItemInfo *pInfo = g_SetItemInfoMgr.GetSetInfoByIdx( iSetIdx );
				if( !pInfo ) continue;

				int iClassType = pInfo->GetSetCode() - SET_ITEM_CODE;
				if( g_MyInfo.GetCharIndexByClassType( iClassType ) > 0 )	// 고용중인 용병은 패스
					continue;

				if( !g_MyInfo.IsClassTypeInventory( iClassType ) )		// 한 번도 고용된적 없으면 패스
					continue;

				NewShopPullDownItem kItem;
				kItem.m_iMagicCode = iClassType;

				char szName[MAX_PATH]="";
				StringCbPrintf( szName, sizeof( szName ), "Lv%d %s", g_MyInfo.GetClassLevel(iClassType,true), g_MyInfo.GetClassName( iClassType, g_MyInfo.GetCharPowerUpGradeByClassType( iClassType ) ) );
				kItem.m_dwTextColor = 0xFF3A3A3A;
				kItem.m_szName      = szName;
				kTmpList.push_back( kItem );
			}
		}

		if( kTmpList.empty() ) return;

		std::sort( kTmpList.begin(), kTmpList.end(), NewShopPullDownItemSort() );

		// 고용중이지 않은 용병 추가하기
		int iListSize = kTmpList.size();
		for( int i=0; i < iListSize; ++i )
		{
			NewShopPullDownItem kItem;

			kItem.m_iMagicCode = kTmpList[i].m_iMagicCode;
			kItem.m_dwTextColor = kTmpList[i].m_dwTextColor;
			kItem.m_szName = kTmpList[i].m_szName;

			rkList.push_back( kItem );
		}
	}
}

void MyInventoryInfoWnd::iwm_create()
{
	g_GUIMgr.AddWnd( "XML/MyInventoryHelpWnd.xml", this );
	g_GUIMgr.AddWnd( "XML/messagebox4.xml", this );
	g_GUIMgr.AddWnd( "XML/myinventorycostumehelpwnd.xml", this );
}

void MyInventoryInfoWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_HIRE_BTN:
	case ID_POWERUP_HIRE_BTN:
		if( cmd == IOBN_BTNUP )
		{
			NewShopWnd *pShopWnd = dynamic_cast<NewShopWnd*>(g_GUIMgr.FindWnd(NEW_SHOP_WND));
			if( pShopWnd )
			{	
				int iClassType = m_iClassType;
				pShopWnd->ShowWnd();   // this wnd을 hide 시키므로 classtype을 저장해서 넣는다.

				if( g_ClassPrice.IsRecommendedClass( iClassType ) && g_SaleMgr.IsSaleByRecommended( ioSaleManager::IT_CLASS, iClassType ) )
					pShopWnd->SetDirectTab( NewShopWnd::ID_RECOMMEND_TAB_BTN, iClassType, 0 );
				else
					pShopWnd->SetDirectTab( NewShopWnd::ID_SOLDIER_TAB_BTN, iClassType, 0 );
			}
		}
		break;
	case ID_DISMISSAL_BTN:
	case ID_POWERUP_DISMISSAL_BTN:
		if( cmd == IOBN_BTNUP )
		{
			SoldierDismissalWnd *pDismissalWnd = dynamic_cast<SoldierDismissalWnd *>(g_GUIMgr.FindWnd( SOLDIER_DISMISSAL_WND ) );
			if( pDismissalWnd )
				pDismissalWnd->SetDismissalChar( m_iClassType );
		}
		break;
	case ID_DISASSEMBLE_BTN:
	case ID_POWERUP_DISASSEMBLE_BTN:
		if( cmd == IOBN_BTNUP )
		{
			SoldierDismissalWnd *pDismissalWnd = dynamic_cast<SoldierDismissalWnd *>(g_GUIMgr.FindWnd( SOLDIER_DISMISSAL_WND ) );
			if( pDismissalWnd )
				pDismissalWnd->SetDisassembleChar( m_iClassType );
		}
		break;
	case ID_MY_SOLDIER_LIST_BTN:
		if( cmd == IOBN_BTNUP )
		{
			UISoldierSelectWnd *pUISoldierWnd = dynamic_cast<UISoldierSelectWnd*> ( g_GUIMgr.FindWnd( UI_SOLDIERSELECT_WND ) );
			if( pUISoldierWnd )
			{
				pUISoldierWnd->ShowUISoldierSelectWnd( this );
				int iCharArray = g_MyInfo.GetClassArray( m_CharInfo.m_class_type );
				if( iCharArray != -1 )
					pUISoldierWnd->SetSelectCharArray( iCharArray );
			}
		}
		break;
	case ID_MY_SOLDIER_LEFT_BTN:
		if( cmd == IOBN_BTNUP )
		{
			int iSlotIdx = m_CharInfo.m_iSlotIndex;			
			int iCharArray = -1;
			while( iCharArray == -1 )
			{
				if( iSlotIdx > 0 )
					iSlotIdx--;
				else
					return;

				iCharArray = g_MyInfo.GetCharSlotIndexToArray( iSlotIdx );
			}
			UISoldierSelectWndBtnUp( iCharArray );
		}
		break;
	case ID_MY_SOLDIER_RIGHT_BTN:
		if( cmd == IOBN_BTNUP )
		{
			int iSlotIdx = m_CharInfo.m_iSlotIndex;			
			int iCharArray = -1;
			while( iCharArray == -1 )
			{
				if( iSlotIdx < g_MyInfo.GetCurMaxCharSlot() - 1 )
					iSlotIdx++;
				else
					return;

				iCharArray = g_MyInfo.GetCharSlotIndexToArray( iSlotIdx );
			}
			UISoldierSelectWndBtnUp( iCharArray );
		}
		break;
	case ID_ETC_SHOP_BTN:
		if( cmd == IOBN_BTNUP )
		{
			NewShopWnd *pShopWnd = dynamic_cast<NewShopWnd*>(g_GUIMgr.FindWnd(NEW_SHOP_WND));
			if( pShopWnd )
			{
				int iMagicCode = m_ItemInfo.m_iMagicCode; // shop show시 내아이템이 hide 되므로 m_ItemInfo.m_iMagicCode 초기화 되어서 저장후 셋팅
				pShopWnd->ShowWnd(); 

				int iOrder    = ORDER_DEFAULT;
				int iINIArray = 0;
				ioEtcItem* pEtcItem = g_EtcItemMgr.FindEtcItem( iMagicCode );
				for( int i = 0; i < pEtcItem->GetRecommandedShopOrderSize(); i++ )
				{
					if( pEtcItem->IsActive( i ) && pEtcItem->GetRecommandedShopOrder( i ) != ORDER_DEFAULT && iOrder == ORDER_DEFAULT )
					{
						iOrder = pEtcItem->GetRecommandedShopOrder( i );
						iINIArray = i;
					}
				}

				if( iOrder != ORDER_DEFAULT && g_SaleMgr.IsSaleByRecommended( ioSaleManager::IT_ETC, iMagicCode, iINIArray ) )
				{
					pShopWnd->SetDirectTab( NewShopWnd::ID_RECOMMEND_TAB_BTN, iMagicCode, 0 );
				}
				else
				{
					pShopWnd->SetDirectTab( NewShopWnd::ID_ETC_TAB_BTN, iMagicCode, 0 );
				}
			}
		}
		break;
	case ID_ITEM_SELL_BTN:
		if( cmd == IOBN_BTNUP )
		{
			if( m_iMainType == INVENTORY_ITEM_ETC )
				SetEtcItemResell();
			else if( m_iMainType == INVENTORY_ITEM_PRESENT )
				SetPresentItemResell();
		}
		break;
	case ID_ETC_USE_BTN:
	case ID_ETC_PRESENT_SEND_BTN:
	case ID_ETC_USE_SMALL_BTN:
		if( cmd == IOBN_BTNUP )
		{
			MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(GetParent());
			if( pInvenWnd )
			{
				pInvenWnd->SetEtcItemUseBtn( m_ItemInfo );
			}
		}
		break;
	case ID_ITEM_RECV:
		if( cmd == IOBN_BTNUP )
		{
			CheckRecvBtnAction();
		}
		break;
	case ID_PRESENT_DISASSEMBLE_BTN:
		if( cmd == IOBN_BTNUP )
		{
			SoldierDismissalWnd *pDismissalWnd = dynamic_cast<SoldierDismissalWnd *>(g_GUIMgr.FindWnd( SOLDIER_DISMISSAL_WND ) );
			if( pDismissalWnd )
				pDismissalWnd->SetDisassemblePresentChar( m_iMainType, m_ItemInfo.m_iMagicCode, m_ItemInfo.m_iMagicCodeEx, m_ItemInfo.m_szSubscriptionID );
		}
		break;
	case ID_ITEM_RETR:
		if( cmd == IOBN_BTNUP )
		{
			CheckRetractBtn();
		}
		break;
	case ID_KINDRED_BTN:
	case ID_HAIR_BTN:
	case ID_HAIR_COLOR_BTN:
	case ID_FACE_BTN:
	case ID_SKIN_COLOR_BTN:
	case ID_UNDERWEAR_BTN:
		if( cmd == IOBN_BTNUP )
		{
			int iDecoType = 0;
			int iDecoCode = 0;

			GetDecoInfo( dwID, iDecoType, iDecoCode );

			MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(GetParent());
			if( pInvenWnd )
			{
				pInvenWnd->SetReleaseDecoItem( iDecoType, iDecoCode );
			}
		}
		break;
	case ID_WEAPON_COSTUME_BTN:
	case ID_ARMOR_COSTUME_BTN:
	case ID_HELMET_COSTUME_BTN:
	case ID_CLOAK_COSTUME_BTN:
		if( cmd == IOBN_BTNUP )
		{
			if( m_CharInfo.m_chExerciseStyle == EXERCISE_RENTAL )
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "전투체험 용병으로는 불가능합니다." );
			else
			{
				int nSlot = dwID - ID_WEAPON_COSTUME_BTN;
				if ( COMPARE( nSlot, 0, MAX_INVENTORY ) )
				{
					int nSlotIndex = m_CharInfo.m_EquipCostume[nSlot].m_CostumeIdx;			

					MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(GetParent());
					if( pInvenWnd )
					{
						if( nSlotIndex > 0 )
							pInvenWnd->SetReleaseCostume( nSlotIndex );
					}
				}
			}
		}
		break;
	case MESSAGE_BOX: // g_GUIMgr.SetMsgBox() 
		break;
	case MESSAGE_BOX3: // g_GUIMgr.SetPrevMsgListBox()
		if( cmd == IOBN_BTNUP )
		{
			if( param == IDYES )
			{
				switch( m_iMainType )
				{
				case INVENTORY_ITEM_PRESENT:
					{
						int iPresentIndex = m_ItemInfo.m_iMagicCode;
						int iPresentSlot  = m_ItemInfo.m_iMagicCodeEx;
						if( iPresentIndex != 0 || iPresentSlot != 0 )
						{
							SP2Packet kPacket( CTPK_PRESENT_RECV );
							kPacket << iPresentIndex << iPresentSlot;
							TCPNetwork::SendToServer( kPacket );
							TCPNetwork::MouseBusy( true );
						}
					}
					break;
				case INVENTORY_ITEM_SUBSCRIPTION:
					{
						SP2Packet kPacket( CTPK_SUBSCRIPTION_RETR );
						kPacket << m_ItemInfo.m_iMagicCode << m_ItemInfo.m_szSubscriptionID;
						TCPNetwork::SendToServer( kPacket );
						TCPNetwork::MouseBusy( true );
					}
					break;
				default:
					{
						if( !m_AwakePullDownClick.IsClickNone() )
						{
							g_AwakeMgr.OnAwakeMenuEvent( m_AwakePullDownClick , m_iAwakeSelectArray );
						}
					}
				}
			}
		}
		break;
	case ITEM_RESELL_WND:
		if( cmd == IOBN_BTNUP )
		{
			if( param == ACST_SELL )
			{
				if( m_iMainType == INVENTORY_ITEM_ETC )
				{
					if( m_iResellType != 0 )		// 특별아이템 팔기
					{
						if( m_iResellType == ioEtcItem::EIT_ETC_FISHING_BAIT ||
							m_iResellType == ioEtcItem::EIT_ETC_FISHING_MOON_BAIT ||
							m_iResellType == ioEtcItem::EIT_ETC_FISHING_MOON_ROD ||
							m_iResellType == ioEtcItem::EIT_ETC_FISHING_ROD ||
							m_iResellType == ioEtcItem::EIT_ETC_PCROOM_FISHING_BAIT ||
							m_iResellType == ioEtcItem::EIT_ETC_PCROOM_FISHING_ROD )
						{
							// 낚시 아이템을 판매할 때 먼저 낚시를 해제한다.
							MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
							if( pInvenWnd && pInvenWnd->GetPlayStage() )
							{
								ioBaseChar *pOwner = pInvenWnd->GetPlayStage()->GetOwnerChar();
								if( pOwner )
								{
									pOwner->SetFishingEndState();
								}

								g_GUIMgr.HideWnd( FISHING_INVENTORY_WND );
							}
						}

						SP2Packet kPacket( CTPK_ETCITEM_SELL );
						kPacket << m_iResellType;
						TCPNetwork::SendToServer( kPacket );
						TCPNetwork::MouseBusy( true );
					}
					else
					{
						g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
					}
				}
				else if( m_iMainType == INVENTORY_ITEM_PRESENT )
				{
					if( m_iResellType != 0 || m_iPresentSlotIndex != 0 )
					{
						SP2Packet kPacket( CTPK_PRESENT_SELL );
						kPacket << m_iResellType << m_iPresentSlotIndex;
						TCPNetwork::SendToServer( kPacket );
						TCPNetwork::MouseBusy( true );
					}
				}
				m_iPresentSlotIndex = 0;
				m_iResellType = 0;
			}
		}
		break;
	case UI_SOLDIERSELECT_WND:
		if( cmd == IOBN_BTNUP )
		{
			UISoldierSelectWndBtnUp( param );
		}
		break;
	case ID_PCROOM_SOLDIER_CHANGE_BTN:
		{
			if( cmd == IOBN_BTNUP )
			{
				if( g_MyInfo.GetPCRoomAuthority() ==  FREEDAY_EVENT_CODE )
					g_GUIMgr.ShowWnd( USER_SELECT_HERO_BONUS_WND );
				else if( g_MyInfo.GetPCRoomAuthority() > 0 && !g_MyInfo.IsUserEvent() )
					g_GUIMgr.ShowWnd( EX_PCROOM_SOLDIER_BONUS_WND );
			}
		}
		break;
	case ID_FREEDAY_SOLDIER_CHANGE_BTN:
		{
			if( cmd == IOBN_BTNUP )
			{
				if( g_MyInfo.IsUserEvent() )
					g_GUIMgr.ShowWnd( USER_SELECT_HERO_BONUS_WND );
			}
		}
		break;
	case ID_ETC_SOULSTONE_USE_BTN :
		{
			if( cmd == IOBN_BTNUP )
			{
				GradeAdjustmentWnd *pWnd = dynamic_cast<GradeAdjustmentWnd*>( g_GUIMgr.FindWnd( GRADE_ADJUSTMENT_WND ) );
				if( pWnd )
				{
					if( pWnd->IsShow() )
					{
						pWnd->GoToTop();
					}
					else
					{
						pWnd->ShowSubWnd( ioPowerUpManager::PIT_HERO );
					}
				}
			}
		}
		break;
	case ID_ETC_SOULSTONE_ENABLE_LIST :
		{
			if( cmd == IOBN_BTNUP )
			{
				EnablePowerupListWnd *pEnablePowerupListWnd = dynamic_cast<EnablePowerupListWnd*>(g_GUIMgr.FindWnd(ENABLE_POWERUP_LIST_WND));
				if( pEnablePowerupListWnd )
				{
					pEnablePowerupListWnd->ShowItemList();
				}
			}
		}
		break;
	case ID_POWERUP_BTN:
		if( cmd == IOBN_BTNUP )
		{
			HideChildWnd( ID_POWERUP_TIP );
			int iCharArray = g_MyInfo.GetClassArray( m_iClassType );

			if( iCharArray == g_MyInfo.GetSelectCharArray() )
			{
				bool bCheck = true;
				MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
				if( pInvenWnd && pInvenWnd->GetPlayStage() )
				{
					ioMyRoomMode *pMyRoom = ToMyRoomMode( pInvenWnd->GetPlayStage()->GetPlayMode() );
					if( pMyRoom && pMyRoom->GetMyRoomType() == MRT_LOBBY )
						bCheck = false;
				}

				if( bCheck )
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, "출전중인 용병은#진화할 수 없습니다." );
					break;
				}
			}

			ioHashString szError;

			if ( !g_PowerUpManager.CheckEnableCharPowerUp( m_iClassType,szError) )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, szError.c_str() );
				break;
			}

			GradeAdjustmentWnd *pWnd = dynamic_cast<GradeAdjustmentWnd*>( g_GUIMgr.FindWnd( GRADE_ADJUSTMENT_WND ) );
			if( pWnd )
			{
				if( pWnd->IsShow() )
				{
					pWnd->GoToTop();
				}
				else
				{
					pWnd->ShowSubWnd( ioPowerUpManager::PIT_HERO, m_iClassType );
				}
			}
		}
		else if( cmd == IOWN_OVERED )
			ShowChildWnd( ID_POWERUP_TIP );
		else if( cmd == IOWN_LEAVED )
			HideChildWnd( ID_POWERUP_TIP );
		break;
	}
}

void MyInventoryInfoWnd::iwm_pulldown_event( const PullDownEvent& Event )
{
	int iCharrArarry = g_MyInfo.GetClassArray( m_CharInfo.m_class_type );

	//마우스 오버
	if( CLASSTYPEQUALS( OnOverPullDownEvent ) == CLASSTYPEQUALS( Event ) )
	{
		const OnOverPullDownEvent& OverEvent = dynamic_cast<const OnOverPullDownEvent&>( Event );
		if( OverEvent.IsImageMouseOver() )
		{
			g_AwakeMgr.OnAwakeMenuToolTipShow( OverEvent, iCharrArarry );
		}
	}
	//마우스 클릭
	else if( CLASSTYPEQUALS( ClickPullDownEvent ) == CLASSTYPEQUALS( Event ) )
	{
		const ClickPullDownEvent& ClickEvent = dynamic_cast<const ClickPullDownEvent&>( Event );
		if( ClickEvent.IsPulldownClick() )
		{			
			if( g_AwakeMgr.AwakeTryMessageBox( ClickEvent, iCharrArarry, this ) )
			{
				m_AwakePullDownClick	= ClickEvent;
				m_iAwakeSelectArray		= iCharrArarry;
			}
		}
	}
	//마우스 리브
	else if( CLASSTYPEQUALS( OnLeavePullDownEvent ) == CLASSTYPEQUALS( Event ) )
	{
		const OnLeavePullDownEvent& LeaveEvent = dynamic_cast<const OnLeavePullDownEvent&>( Event );
		if( LeaveEvent.IsImageMouseLeave() )
		{
			g_AwakeMgr.OnAwakeMenuToolTipHide( LeaveEvent, iCharrArarry );
		}
	}
}

void MyInventoryInfoWnd::iwm_show()
{
	vNewShopPullDownItem kItemList;
	GetItemList( ID_MY_SOLDIER_LIST_BTN, kItemList );

	if( kItemList.empty() )
		return;

	int iClassType = kItemList[0].m_iMagicCode;
	int iCharArray = g_MyInfo.GetClassArray( iClassType );

	if( COMPARE( iCharArray, 0, g_MyInfo.GetCharCount() ) )
		m_CharInfo = g_MyInfo.GetCharacter( iCharArray );
}

void MyInventoryInfoWnd::iwm_hide()
{
	//// 2019-04-30 null 값
	//ExtraItemFuncWnd *pExtraItemWnd = dynamic_cast<ExtraItemFuncWnd*>(FindChildWnd(MyInventoryWnd::ID_EXTRAITEMFUNC_WND));
	//if( NULL != pExtraItemWnd )
	//{
	//	pExtraItemWnd->SetRollingStopSound();
	//}

	SetBlank();
}

void MyInventoryInfoWnd::SetCharInfo( OUT CHARACTER &rkCharInfo, IN int iType, IN int iCode )
{
	switch( iType )
	{
	case UID_FACE:
		rkCharInfo.m_face = iCode;
		break;
	case UID_HAIR:
		rkCharInfo.m_hair = iCode;
		break;
	case UID_SKIN_COLOR:
		rkCharInfo.m_skin_color = iCode;
		break;
	case UID_HAIR_COLOR:
		rkCharInfo.m_hair_color = iCode;
		break;
	case UID_UNDERWEAR:
		rkCharInfo.m_underwear = iCode;
		break;
	case UID_KINDRED:
		{
			switch( iCode )
			{
			case RDT_HUMAN_MAN:
				rkCharInfo.m_kindred = 1;
				rkCharInfo.m_sex = 1;
				break;
			case RDT_HUMAN_WOMAN:
				rkCharInfo.m_kindred = 1;
				rkCharInfo.m_sex = 2;
				break;
			case RDT_ELF_MAN:
				rkCharInfo.m_kindred = 2;
				rkCharInfo.m_sex = 1;
				break;
			case RDT_ELF_WOMAN:
				rkCharInfo.m_kindred = 2;
				rkCharInfo.m_sex = 2;
				break;
			case RDT_DWARF_MAN:
				rkCharInfo.m_kindred = 3;
				rkCharInfo.m_sex = 1;
				break;
			case RDT_DWARF_WOMAN:
				rkCharInfo.m_kindred = 3;
				rkCharInfo.m_sex = 2;
				break;
			}
		}
		break;	
	default:
		break;
	}

	// 종족
	if( iType != UID_KINDRED ) 
		return;

	int iInvenType = 0;
	int iInvenCode = 0;
	GetInvenDecoInfo( iInvenType, iInvenCode, rkCharInfo.m_class_type, rkCharInfo.m_sex-1, iType, iCode );
	if( g_MyInfo.IsSlotItem( iInvenType, iInvenCode ) )
	{
		/* 장착 중인 치장 적용 */
		rkCharInfo.m_face = g_MyInfo.GetEquipItemCode( ( rkCharInfo.m_class_type * 100000 ) + ( ( rkCharInfo.m_sex - 1) * 1000 ) + UID_FACE );
		rkCharInfo.m_hair = g_MyInfo.GetEquipItemCode( ( rkCharInfo.m_class_type * 100000 ) + ( ( rkCharInfo.m_sex - 1) * 1000 ) + UID_HAIR );
		rkCharInfo.m_skin_color = g_MyInfo.GetEquipItemCode( ( rkCharInfo.m_class_type * 100000 ) + ( ( rkCharInfo.m_sex - 1) * 1000 ) + UID_SKIN_COLOR );
		rkCharInfo.m_hair_color = g_MyInfo.GetEquipItemCode( ( rkCharInfo.m_class_type * 100000 ) + ( ( rkCharInfo.m_sex - 1) * 1000 ) + UID_HAIR_COLOR );
		rkCharInfo.m_underwear = g_MyInfo.GetEquipItemCode( ( rkCharInfo.m_class_type * 100000 ) + ( ( rkCharInfo.m_sex - 1) * 1000 ) + UID_UNDERWEAR );
	}
	else
	{
		/* 종족 구매시 랜덤으로 받을 치장 적용 */
		rkCharInfo.m_face = g_DecorationPrice.GetDefaultDecoCode( rkCharInfo.m_sex - 1, UID_FACE, g_MyInfo.GetPrivateID().GetHashCode() + rkCharInfo.m_class_type + UID_FACE );
		rkCharInfo.m_hair = g_DecorationPrice.GetDefaultDecoCode( rkCharInfo.m_sex - 1, UID_HAIR, g_MyInfo.GetPrivateID().GetHashCode() + rkCharInfo.m_class_type + UID_HAIR );
		rkCharInfo.m_skin_color = g_DecorationPrice.GetDefaultDecoCode( rkCharInfo.m_sex - 1, UID_SKIN_COLOR, g_MyInfo.GetPrivateID().GetHashCode() + rkCharInfo.m_class_type + UID_SKIN_COLOR );
		rkCharInfo.m_hair_color = g_DecorationPrice.GetDefaultDecoCode( rkCharInfo.m_sex - 1, UID_HAIR_COLOR, g_MyInfo.GetPrivateID().GetHashCode() + rkCharInfo.m_class_type + UID_HAIR_COLOR );
		rkCharInfo.m_underwear = g_DecorationPrice.GetDefaultDecoCode( rkCharInfo.m_sex - 1, UID_UNDERWEAR, g_MyInfo.GetPrivateID().GetHashCode() + rkCharInfo.m_class_type + UID_UNDERWEAR );	
	}
}

void MyInventoryInfoWnd::GetInvenDecoInfo( OUT int &riInvenDecoType, OUT int &rInvenDecoCode, IN int iClassType, IN int iDecoSexType, IN int iDecoType, IN int iDecoCode )
{
	//고유 특성 1111 22 333 ( 세트, 종족(성별), 치장타입 )
	riInvenDecoType = ( iClassType * 100000) + ( iDecoSexType * 1000);

	// 종족은 무조건 남성만 존재하므로 iDecoSexType 0 남자 , 1 여자 ( g_MyInfo.IsSlotItem()에서도 여자 종족 성별은 구분하지 않음 )
	if( iDecoSexType == 1 && iDecoType == UID_KINDRED ) 
		riInvenDecoType -= 1000; 

	rInvenDecoCode = 0;
	switch( iDecoType )
	{
	case UID_FACE:
	case UID_HAIR:
	case UID_SKIN_COLOR:
	case UID_HAIR_COLOR:
	case UID_UNDERWEAR:
	case UID_KINDRED:
		{
			riInvenDecoType += iDecoType;
			rInvenDecoCode  =  iDecoCode;
		}
		break;
	case UID_CLASS:
		{
			int iEquipType = iDecoCode / 1000;
			int iEquipCode = iDecoCode % 1000;
			riInvenDecoType += iEquipType;
			rInvenDecoCode  =  iEquipCode;
		}
		break;	
	}
}

void MyInventoryInfoWnd::GetDecoInfo( DWORD dwID, OUT int &riDecoType, OUT int &riDecoCode )
{
	riDecoType = 0;
	riDecoCode = 0;

	InventoryIconBtn *pBtn = dynamic_cast<InventoryIconBtn*>(FindChildWnd(dwID));
	if( pBtn )
	{
		switch ( dwID )
		{
		case ID_KINDRED_BTN:
			riDecoType = UID_KINDRED;
			riDecoCode = (int)GetRaceDetailType( m_CharInfo );
			break;
		case ID_HAIR_BTN:
			riDecoType = UID_HAIR;
			riDecoCode = m_CharInfo.m_hair;
			break;
		case ID_HAIR_COLOR_BTN:
			riDecoType = UID_HAIR_COLOR;
			riDecoCode = m_CharInfo.m_hair_color;
			break;
		case ID_FACE_BTN:
			riDecoType = UID_FACE;
			riDecoCode = m_CharInfo.m_face;
			break;
		case ID_SKIN_COLOR_BTN:
			riDecoType = UID_SKIN_COLOR;
			riDecoCode = m_CharInfo.m_skin_color;
			break;
		case ID_UNDERWEAR_BTN:
			riDecoType = UID_UNDERWEAR;
			riDecoCode = m_CharInfo.m_underwear;
			break;
		}
	}
}

void MyInventoryInfoWnd::SetEtcItemResell()
{
	int iMagicCode = m_ItemInfo.m_iMagicCode;
	m_iResellType = 0;
	m_iPresentSlotIndex = 0;

	ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( iMagicCode );
	if( !pEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s ioEtcItem == NULL , Type:%d", __FUNCTION__, iMagicCode );
		return;
	}

	ioUserEtcItem *pUserEtcItem =  g_MyInfo.GetUserEtcItem();
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s UserEtcItem == NULL , Type:%d", __FUNCTION__, iMagicCode );
		return;
	}

	ioUserEtcItem::ETCITEMSLOT kItemSlot;
	if( !pUserEtcItem->GetEtcItem( iMagicCode, kItemSlot ) )
	{
		LOG.PrintTimeAndLog( 0, "%s don't have , Type:%d", __FUNCTION__, iMagicCode );
		return;
	}

	if( pEtcItem->OnSellBtnPush( iMagicCode, pUserEtcItem, this ) )
	{
		m_iResellType = iMagicCode;
		m_iPresentSlotIndex = m_ItemInfo.m_iMagicCodeEx;
	}	
}

void MyInventoryInfoWnd::SetPresentItemResell()
{
	ioHashString szSendID;
	short sPresentType, sPresentMent;
	int iPresentValue1, iPresentValue2, iPresentValue3, iPresentValue4;
	DWORD dwLimitDate;

	int iPresentIndex = m_ItemInfo.m_iMagicCode;
	int iPresentSlot  = m_ItemInfo.m_iMagicCodeEx;
	g_PresentMgr.GetPresentDataToIndex( iPresentIndex, iPresentSlot, szSendID, sPresentType, sPresentMent, iPresentValue1, iPresentValue2, iPresentValue3, iPresentValue4, dwLimitDate );

	m_iResellType = iPresentIndex;
	m_iPresentSlotIndex = iPresentSlot;

	ioHashString szItemName;
	ioHashString szItemInfo;
	ioHashString szIconName;
	ioHashString szSubIconName;

	char szConvertNum[MAX_PATH] = "";

	bool bIsCostume = false;
	int nGradeType = 0;

	switch( sPresentType )
	{
	case PRESENT_SOLDIER:
		{
			int iResellPeso  = 0;
			if( iPresentValue2 == 0 )
				iResellPeso  = g_ClassPrice.GetMortmainCharResllPeso( iPresentValue1 );
			else
				iResellPeso  = g_ClassPrice.GetTimeCharResellPeso( iPresentValue1, iPresentValue2 );

			// ItemInfo
			szItemName = g_PresentMgr.GetPresentValue1Text( sPresentType, iPresentValue1, iPresentValue2 );
			szItemInfo = g_PresentMgr.GetPresentValue2Text( sPresentType, iPresentValue1, iPresentValue2 );

			// Desc
			Help::ConvertNumToStrComma( iResellPeso, szConvertNum, sizeof( szConvertNum ) );

			szIconName = g_MyInfo.GetMySoldierIconName( iPresentValue1 );
			szSubIconName = g_MyInfo.GetSoldierSubIconName( iPresentValue1 );
			nGradeType = g_UIImageRenderMgr.GetPowerUpGradeType( iPresentValue1 );
		}
		break;
	case PRESENT_DECORATION:
		{
			// ItemInfo
			szItemName = g_PresentMgr.GetPresentValue1Text( sPresentType, iPresentValue1, iPresentValue2 );
			szItemInfo = g_PresentMgr.GetPresentValue2Text( sPresentType, iPresentValue1, iPresentValue2 );

			int iClassType = iPresentValue1 / 100000;
			int iSexType   = (iPresentValue1 % 100000) / 1000;
			int iDecoType  = iPresentValue1 % 1000;
			const DecoData &kData = g_DecorationPrice.GetDecoData( iSexType, iDecoType, iPresentValue2 );

			int iResellPeso  = kData.m_iSellPeso;		
			Help::ConvertNumToStrComma( iResellPeso, szConvertNum, sizeof( szConvertNum ) );

			szIconName = kData.m_szIcon;
		}
		break;
	case PRESENT_ETC_ITEM:
		{
			ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( iPresentValue1 );
			if( pEtcItem )
			{
				// ItemInfo
				szItemName = g_PresentMgr.GetPresentValue1Text( sPresentType, iPresentValue1, iPresentValue2 );
				szItemInfo = g_PresentMgr.GetPresentValue2Text( sPresentType, iPresentValue1, iPresentValue2 );

				int iResellPeso  = pEtcItem->GetSellPeso() * iPresentValue2;
				if( ( iPresentValue1 / ioEtcItem::USE_TYPE_CUT_VALUE ) == ioEtcItem::UT_DATE && iPresentValue2 == 0 )
				{
					iResellPeso = pEtcItem->GetMortmainSellPeso();
				}

				Help::ConvertNumToStrComma( iResellPeso, szConvertNum, sizeof( szConvertNum ) );

				szIconName = pEtcItem->GetIconName();
				szSubIconName = pEtcItem->GetSubIconName();
				nGradeType = pEtcItem->GetGradeType();
			}			
		}
		break;
	case PRESENT_PESO:
		{
			// ItemInfo
			szItemName = g_PresentMgr.GetPresentValue1Text( sPresentType, iPresentValue1, iPresentValue2 );
			szItemInfo = g_PresentMgr.GetPresentValue2Text( sPresentType, iPresentValue1, iPresentValue2 );

			Help::ConvertNumToStrComma( iPresentValue1, szConvertNum, sizeof( szConvertNum ) );

			szIconName = ioHashString( "UIIconPack13#quest_002" );
		}
		break;
	case PRESENT_EXTRAITEM:
		{
			// ItemInfo
			szItemName = g_PresentMgr.GetPresentValue1Text( sPresentType, iPresentValue1, iPresentValue2 );
			szItemInfo = g_PresentMgr.GetPresentValue2Text( sPresentType, iPresentValue1, iPresentValue2 );

			int iExtraValue2 = iPresentValue2 % PRESENT_EXTRAITEM_DIVISION_1;
			
			float fReturnPeso = 0.0f;
			int nReinforce = 0;
			int iItemLimitDate = 0;
			if( iPresentValue2 / PRESENT_EXTRAITEM_DIVISION_1 != 0 )
			{
				iItemLimitDate = (iPresentValue2 % PRESENT_EXTRAITEM_DIVISION_1) % PRESENT_EXTRAITEM_DIVISION_2;				
				nReinforce = (iPresentValue2 / PRESENT_EXTRAITEM_DIVISION_1) / 10000;
			}
			else
			{
				iItemLimitDate = iPresentValue2 % PRESENT_EXTRAITEM_DIVISION_2;				
				nReinforce = iPresentValue2 / 10000;
			}

			if( iItemLimitDate == 0 )
			{
				fReturnPeso = g_ExtraItemInfoMgr.GetMortmainItemSellPesoReinforce( nReinforce );
			}
			else
			{
				DWORD dwTotalTime = iItemLimitDate * 60;
				fReturnPeso = g_ExtraItemInfoMgr.GetTimeItemSellPesoReinforce( dwTotalTime, nReinforce );
				fReturnPeso = max( 0, fReturnPeso );
			}

			int iResellPeso = (int)fReturnPeso;
			Help::ConvertNumToStrComma( iResellPeso, szConvertNum, sizeof( szConvertNum ) );

			const ioItem *pItem = g_ItemMaker.GetItemConst( iPresentValue1, __FUNCTION__ );
			if( pItem )
			{
				szIconName = pItem->GetItemLargeIconName();
				nGradeType = pItem->GetGradeType();
			}
		}
		break;
	case PRESENT_EXTRAITEM_BOX:
		{
			// ItemInfo
			szItemName = g_PresentMgr.GetPresentValue1Text( sPresentType, iPresentValue1, iPresentValue2 );
			szItemInfo = g_PresentMgr.GetPresentValue2Text( sPresentType, iPresentValue1, iPresentValue2 );

			RandomMachineInfo *pInfo = g_ExtraItemInfoMgr.GetRandomMachineInfo( iPresentValue1 );
			if( pInfo )
			{
				szIconName = pInfo->GetIconName();
				Help::ConvertNumToStrComma( pInfo->GetSellPeso(), szConvertNum, sizeof( szConvertNum ) );
				nGradeType = pInfo->GetGradeType();
			}
		}
		break;
	case PRESENT_RANDOM_DECO:
		{
			// ItemInfo
			szItemName = g_PresentMgr.GetPresentValue1Text( sPresentType, iPresentValue1, iPresentValue2 );
			szItemInfo = g_PresentMgr.GetPresentValue2Text( sPresentType, iPresentValue1, iPresentValue2 );

			int iClassType = iPresentValue1 / 100000;
			int iSexType   = (iPresentValue1 % 100000) / 1000;
			int iDecoType  = iPresentValue1 % 1000;
			const DecoData &kData = g_DecorationPrice.GetDecoData( iSexType, iDecoType, iPresentValue2 );

			int iResellPeso  = kData.m_iSellPeso;		
			Help::ConvertNumToStrComma( iResellPeso, szConvertNum, sizeof( szConvertNum ) );

			szIconName = kData.m_szIcon;
		}
		break;
	case PRESENT_GRADE_EXP:
		{
			// ItemInfo
			szItemName = g_PresentMgr.GetPresentValue1Text( sPresentType, iPresentValue1, iPresentValue2 );
			szItemInfo = g_PresentMgr.GetPresentValue2Text( sPresentType, iPresentValue1, iPresentValue2 );

			Help::ConvertNumToStrComma( iPresentValue1, szConvertNum, sizeof( szConvertNum ) );
			
			szIconName = ioHashString( "UIIconPack13#quest_001" );
		}
		break;
	case PRESENT_MEDALITEM:
		{
			// ItemInfo
			szItemName = g_PresentMgr.GetPresentValue1Text( sPresentType, iPresentValue1, iPresentValue2 );
			szItemInfo = g_PresentMgr.GetPresentValue2Text( sPresentType, iPresentValue1, iPresentValue2 );

			const ioMedalItemInfoManager::ItemInfo *pItemInfo = g_MedalItemMgr.GetItemInfo( iPresentValue1 );
			if( pItemInfo )
			{
				szIconName = pItemInfo->m_sIcon;
				szSubIconName = pItemInfo->m_sSubIcon;
				if( iPresentValue2 == 0 )
				{
					Help::ConvertNumToStrComma( pItemInfo->m_iSellPeso, szConvertNum, sizeof( szConvertNum ) );
				}
				else
				{
					float fSellPeso = pItemInfo->m_iSellPeso;
					fSellPeso = fSellPeso / g_MedalItemMgr.GetSellPesoByMinute();
					fSellPeso *= iPresentValue2*60;
					Help::ConvertNumToStrComma( static_cast<int>(fSellPeso), szConvertNum, sizeof( szConvertNum ) );
				}
			}
		}
		break;
	case PRESENT_ALCHEMIC_ITEM:
		break;
	case PRESENT_PET:
		{
			// ItemInfo
			szItemName = g_PresentMgr.GetPresentValue1Text( sPresentType, iPresentValue1, iPresentValue2 );

			char szTemp[MAX_PATH] = "";
			int nPetLevel = iPresentValue2 / 10000;
			SafeSprintf( szTemp, sizeof( szTemp ), "Lv %d %s", nPetLevel, g_PresentMgr.GetPresentValue2Text( sPresentType, iPresentValue1, iPresentValue2 ) );
			szItemInfo = szTemp;

			int iPeso = g_PetInfoMgr.GetResellPeso();
			Help::ConvertNumToStrComma( iPeso, szConvertNum, sizeof(szConvertNum) );

			int nPetRank = iPresentValue2%10000;
			szIconName = g_PetInfoMgr.GetPetIcon( iPresentValue1, (PetRankType)nPetRank );
		}
		break;
	case PRESENT_COSTUME:
		{
			// UI코스튬 관련 (선물 팔기 설정)
			szItemName = g_PresentMgr.GetPresentValue1Text( sPresentType, iPresentValue1, iPresentValue2 );

			char szTemp[MAX_PATH] = "";
			SafeSprintf( szTemp, sizeof( szTemp ), "%s", g_PresentMgr.GetPresentValue2Text( sPresentType, iPresentValue1, iPresentValue2 ) );
			szItemInfo = szTemp;

			float fReturnPeso = 0.0f;
			int nItemLimitDate = iPresentValue2 % PRESENT_EXTRAITEM_DIVISION_2;		

			if( nItemLimitDate == 0 )
				fReturnPeso = g_CostumeShopInfoMgr.GetResellMortmainCostumePeso();
			else
				fReturnPeso = g_CostumeShopInfoMgr.GetResellTimeCostumeTimePeso();
			
			int iPeso = (int)fReturnPeso;			
			Help::ConvertNumToStrComma( iPeso, szConvertNum, sizeof(szConvertNum) );

			szIconName = g_CostumeInfoMgr.GetCostumeIcon( iPresentValue1 );
			nGradeType = g_CostumeInfoMgr.GetGradeType( iPresentValue1 );

			bIsCostume = true;
		}
		break;
	case PRESENT_COSTUME_BOX:
		{
			// ItemInfo
			szItemName = g_PresentMgr.GetPresentValue1Text( sPresentType, iPresentValue1, iPresentValue2 );
			szItemInfo = g_PresentMgr.GetPresentValue2Text( sPresentType, iPresentValue1, iPresentValue2 );

			CostumeShopInfo *pInfo = g_CostumeShopInfoMgr.GetCostumeMachineInfo( iPresentValue1 );
			if( pInfo )
			{
				szIconName = pInfo->GetIconName();
				nGradeType = pInfo->GetGradeType();
				Help::ConvertNumToStrComma( pInfo->GetSellPeso(), szConvertNum, sizeof( szConvertNum ) ); 
			}
		}
		break;
	case PRESENT_BONUS_CASH:
		{
			// ItemInfo
			szItemName = g_PresentMgr.GetPresentValue1Text( sPresentType, iPresentValue1, iPresentValue2 );
			szItemInfo = g_PresentMgr.GetPresentValue2Text( sPresentType, iPresentValue1, iPresentValue2 );

			Help::ConvertNumToStrComma( iPresentValue1, szConvertNum, sizeof( szConvertNum ) );

			szIconName = ioHashString( "UIIconPack101#Cash" );
		}
		break;
	case PRESENT_ACCESSORY:
		{
			// ItemInfo
			szItemName = g_PresentMgr.GetPresentValue1Text( sPresentType, iPresentValue1, iPresentValue2 );
			szItemInfo = g_PresentMgr.GetPresentValue2Text( sPresentType, iPresentValue1, iPresentValue2 );

			float fReturnPeso = 0.0f;
			//영구
			if( iPresentValue2 == 0 )
				fReturnPeso = g_AccessoryInfoMgr.GetResellMortmainAccessoryPeso();
			else
				fReturnPeso = g_AccessoryInfoMgr.GetResellTimeAccessoryTimePeso();

			int iResellPeso = (int)fReturnPeso;
			Help::ConvertNumToStrComma( iResellPeso, szConvertNum, sizeof( szConvertNum ) );

			const ioItem *pItem = g_ItemMaker.GetItemConst( iPresentValue1, __FUNCTION__ );
			if( pItem )
			{
				szIconName = pItem->GetItemLargeIconName();
				nGradeType = pItem->GetGradeType();
			}
		}
		break;
	case PRESENT_SPIRIT:
		{
			// ItemInfo
			szItemName = g_PresentMgr.GetPresentValue1Text( sPresentType, iPresentValue1, iPresentValue2 );
			szItemInfo = g_PresentMgr.GetPresentValue2Text( sPresentType, iPresentValue1, iPresentValue2 );

			int iPeso = iPresentValue2 * g_SpiritMgr.GetSellConst( iPresentValue1 );
			Help::ConvertNumToStrComma( iPeso, szConvertNum, sizeof(szConvertNum) );

			szIconName = g_SpiritMgr.GetIconName( iPresentValue1 );
		}
		break;
	default:
		// 예외처리
		return;
	}

	ioComplexStringPrinter kPrinter[ItemResellWnd::MAX_INFO_LINE];

	// ItemInfo
	kPrinter[0].SetTextStyle( TS_NORMAL );
	kPrinter[0].SetBkColor( 0, 0, 0 );
	kPrinter[0].SetTextColor( TCT_DEFAULT_RED );
	kPrinter[0].AddTextPiece( FONT_SIZE_17, STR(2), szItemName.c_str() );

	kPrinter[1].SetTextStyle( TS_NORMAL );
	kPrinter[1].SetBkColor( 0, 0, 0 );
	kPrinter[1].SetTextColor( TCT_DEFAULT_RED );
	kPrinter[1].AddTextPiece( FONT_SIZE_17, STR(3), szItemInfo.c_str() );

	// Desc
	kPrinter[2].SetTextStyle( TS_NORMAL );
	kPrinter[2].SetBkColor( 0, 0, 0 );
	kPrinter[2].SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter[2].AddTextPiece( FONT_SIZE_13, STR(4) );

	kPrinter[3].SetTextStyle( TS_NORMAL );
	kPrinter[3].SetBkColor( 0, 0, 0 );
	kPrinter[3].SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter[3].AddTextPiece( FONT_SIZE_13, STR(5) );
	kPrinter[3].SetTextColor( TCT_DEFAULT_BLUE );
	kPrinter[3].AddTextPiece( FONT_SIZE_13, STR(6), szConvertNum );

	kPrinter[4].SetTextStyle( TS_NORMAL );
	kPrinter[4].SetBkColor( 0, 0, 0 );
	kPrinter[4].SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter[4].AddTextPiece( FONT_SIZE_13, STR(7) );

	ItemResellWnd *pResellWnd = dynamic_cast<ItemResellWnd*>(g_GUIMgr.FindWnd(ITEM_RESELL_WND));
	if( pResellWnd )
	{
		ioUIRenderImage *pImg = g_UIImageSetMgr.CreateImageByFullName( szIconName );
		ioUIRenderImage *pSubImg = g_UIImageSetMgr.CreateImageByFullName( szSubIconName );
		pResellWnd->SetInfoAndShow( STR(8), pImg, pSubImg, 0, kPrinter, this, SELL_EDIT_NONE, ACST_SELL, bIsCostume, nGradeType );
	}
}

void MyInventoryInfoWnd::SetPresentSoldier( int iValue1, int iValue2 )
{
	// icon btn
	ioUIRenderImage *pImg = g_UIImageSetMgr.CreateImageByFullName( g_MyInfo.GetMySoldierIconName( iValue1 ) );
	ioUIRenderImage *pSubImg = g_UIImageSetMgr.CreateImageByFullName( g_MyInfo.GetSoldierSubIconName( iValue1 ) );
	InventoryIconBtn *pBtn = dynamic_cast< InventoryIconBtn* > ( FindChildWnd( ID_ITEM_ICON ) );
	if( pBtn )
	{
		pBtn->SetIcon( pImg, FLOAT1, FLOAT1 );
		pBtn->SetSubIcon( pSubImg );
		pBtn->SetGradeType( g_UIImageRenderMgr.GetPowerUpGradeType( iValue1 ) );
		pBtn->SetMarkType( INVENTORY_MARK_TYPE_NONE );
		pBtn->SetActive();
	}
}

void MyInventoryInfoWnd::SetPresentDeco( int iValue1, int iValue2 )
{
	int iClassType = iValue1 / 100000;
	int iSexType   = (iValue1 % 100000) / 1000;
	int iDecoType  = iValue1 % 1000;
	const DecoData &kData = g_DecorationPrice.GetDecoData( iSexType, iDecoType, iValue2 );

	// icon btn
	ioUIRenderImage *pImg = g_UIImageSetMgr.CreateImageByFullName( kData.m_szIcon );
	InventoryIconBtn *pBtn = dynamic_cast< InventoryIconBtn* > ( FindChildWnd( ID_ITEM_ICON ) );
	if( pBtn )
	{
		pBtn->SetIcon( pImg, FLOAT1, FLOAT1 );
		pBtn->SetMarkType( INVENTORY_MARK_TYPE_NONE );
		pBtn->SetActive();
	}
}

void MyInventoryInfoWnd::SetPresentRandomDeco( int iValue1, int iValue2 )
{
	int iClassType = iValue1 / 100000;
	int iSexType   = (iValue1 % 100000) / 1000;
	int iDecoType  = iValue1 % 1000;

	const DecoData &kData = g_DecorationPrice.GetDecoData( iSexType, iDecoType, iValue2 );

	// icon btn
	ioUIRenderImage *pImg = g_UIImageSetMgr.CreateImageByFullName( kData.m_szIcon );
	InventoryIconBtn *pBtn = dynamic_cast< InventoryIconBtn* > ( FindChildWnd( ID_ITEM_ICON ) );
	if( pBtn )
	{
		pBtn->SetIcon( pImg, FLOAT1, FLOAT1 );
		pBtn->SetMarkType( INVENTORY_MARK_TYPE_NONE );
		pBtn->SetActive();
	}
}

void MyInventoryInfoWnd::SetPresentEtcItem( int iValue1, int iValue2 )
{
	ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( iValue1 );
	if( !pEtcItem ) return;

	if( !CheckPresentSell( iValue1, iValue2 ) )
		SetChildActive( ID_ITEM_SELL_BTN );
	else
		SetChildInActive( ID_ITEM_SELL_BTN );

	// icon btn
	ioUIRenderImage *pImg = g_UIImageSetMgr.CreateImageByFullName( pEtcItem->GetIconName() );
	ioUIRenderImage *pSubImg = g_UIImageSetMgr.CreateImageByFullName( pEtcItem->GetSubIconName() );
	InventoryIconBtn *pBtn = dynamic_cast< InventoryIconBtn* > ( FindChildWnd( ID_ITEM_ICON ) );
	if( pBtn )
	{
		pBtn->SetIcon( pImg, FLOAT1, FLOAT1 );
		pBtn->SetSubIcon( pSubImg );
		pBtn->SetGradeType( pEtcItem->GetGradeType() );
		pBtn->SetMarkType( INVENTORY_MARK_TYPE_NONE );
		pBtn->SetActive();
	}
}

void MyInventoryInfoWnd::SetPresentPeso( int iValue1, int iValue2 )
{
	// icon btn
	ioUIRenderImage *pImg = g_UIImageSetMgr.CreateImageByFullName( ioHashString( "UIIconPack13#quest_002" ) );
	InventoryIconBtn *pBtn = dynamic_cast< InventoryIconBtn* > ( FindChildWnd( ID_ITEM_ICON ) );
	if( pBtn )
	{
		pBtn->SetIcon( pImg, FLOAT1, FLOAT1 );
		pBtn->SetMarkType( INVENTORY_MARK_TYPE_NONE );
		pBtn->SetActive();
	}
}

void MyInventoryInfoWnd::SetPresentExtraItem( int iValue1, int iValue2, int iValue3, int iValue4, short sPresentMent )
{
	// icon btn
	const ioItem *pItem = g_ItemMaker.GetItemConst( iValue1, __FUNCTION__ );
	if( pItem )
	{
		ioHashString szIcon = pItem->GetItemLargeIconName();
		if( !szIcon.IsEmpty() )
		{
			InventoryIconBtn *pBtn = dynamic_cast< InventoryIconBtn* > ( FindChildWnd( ID_ITEM_ICON ) );
			if( pBtn )
			{
				pBtn->SetIcon( g_UIImageSetMgr.CreateImageByFullName( szIcon ), FLOAT1, FLOAT1 );
				pBtn->SetGradeType( pItem->GetGradeType() );

				if( iValue3 > 0 || iValue4 > 0 )
					pBtn->SetExtraItemCustom( true );

				if( sPresentMent == PRESENT_TRADE_CANCEL_MENT || sPresentMent == PRESENT_TRADE_TIMEOUT_MENT )
				{
					pBtn->SetMarkType( INVENTORY_MARK_TYPE_TRADE_ENABLE );
					pBtn->SetActive();
				}
				else
				{
					pBtn->SetMarkType( INVENTORY_MARK_TYPE_NONE );
					pBtn->SetActive();
				}
			}
		}
	}
}

void MyInventoryInfoWnd::SetPresentExtraItemBox( int iValue1, int iValue2 )
{
	// icon btn
	RandomMachineInfo *pInfo = g_ExtraItemInfoMgr.GetRandomMachineInfo( iValue1 );
	if( pInfo )
	{
		ioHashString szIcon = pInfo->GetIconName();
		if( !szIcon.IsEmpty() )
		{
			InventoryIconBtn *pBtn = dynamic_cast< InventoryIconBtn* > ( FindChildWnd( ID_ITEM_ICON ) );
			if( pBtn )
			{
				pBtn->SetIcon( g_UIImageSetMgr.CreateImageByFullName( szIcon ), FLOAT1, FLOAT1 );
				pBtn->SetGradeType( pInfo->GetGradeType() );
				pBtn->SetMarkType( INVENTORY_MARK_TYPE_NONE );
				pBtn->SetActive();
			}
		}
	}
}

void MyInventoryInfoWnd::SetPresentGradeExp( int iValue1, int iValue2 )
{
	// icon btn
	ioUIRenderImage *pImg = g_UIImageSetMgr.CreateImageByFullName( ioHashString( "UIIconPack13#quest_001" ) );
	InventoryIconBtn *pBtn = dynamic_cast< InventoryIconBtn* > ( FindChildWnd( ID_ITEM_ICON ) );
	if( pBtn )
	{
		pBtn->SetIcon( pImg, FLOAT1, FLOAT1 );
		pBtn->SetMarkType( INVENTORY_MARK_TYPE_NONE );
		pBtn->SetActive();
	}
}

void MyInventoryInfoWnd::SetPresentMedalItem( int iValue1, int iValue2 )
{
	// icon btn
	const ioMedalItemInfoManager::ItemInfo *pItemInfo = g_MedalItemMgr.GetItemInfo( iValue1 );
	if( pItemInfo )
	{
		ioHashString szIcon = pItemInfo->m_sIcon;
		ioHashString szSubIcon = pItemInfo->m_sSubIcon;
		if( !szIcon.IsEmpty() )
		{
			InventoryIconBtn *pBtn = dynamic_cast< InventoryIconBtn* > ( FindChildWnd( ID_ITEM_ICON ) );
			if( pBtn )
			{
				pBtn->SetIcon( g_UIImageSetMgr.CreateImageByFullName( szIcon ), FLOAT1, FLOAT1 );
				pBtn->SetSubIcon( g_UIImageSetMgr.CreateImageByFullName( szSubIcon ) );
				pBtn->SetMarkType( INVENTORY_MARK_TYPE_NONE );
				pBtn->SetActive();
			}
		}
	}
}

void MyInventoryInfoWnd::SetPresentPet( int iValue1, int iValue2 )
{
	// icon btn
	int nPetRank = iValue2%10000;
	ioHashString szIcon = g_PetInfoMgr.GetPetIcon( iValue1, (PetRankType)nPetRank );
	if( !szIcon.IsEmpty() )
	{
		InventoryIconBtn *pBtn = dynamic_cast< InventoryIconBtn* > ( FindChildWnd( ID_ITEM_ICON ) );
		if( pBtn )
		{
			pBtn->SetIcon( g_UIImageSetMgr.CreateImageByFullName( szIcon ), FLOAT1, FLOAT1 );
			pBtn->SetMarkType( INVENTORY_MARK_TYPE_NONE );
			pBtn->SetActive();
		}
	}
}

// UI코스튬 관련 (선물 설정)
void MyInventoryInfoWnd::SetPresentCostume( int iValue1, int iValue2 )
{
	// icon btn
	ioHashString szIcon = g_CostumeInfoMgr.GetCostumeIcon( iValue1 );
	if( !szIcon.IsEmpty() )
	{
		InventoryIconBtn *pBtn = dynamic_cast< InventoryIconBtn* > ( FindChildWnd( ID_ITEM_ICON ) );
		if( pBtn )
		{
			pBtn->SetIcon( g_UIImageSetMgr.CreateImageByFullName( szIcon ), FLOAT1, FLOAT1 );
			pBtn->SetGradeType( g_CostumeInfoMgr.GetGradeType( iValue1 ) );
			pBtn->SetCostume( true );
			pBtn->SetMarkType( INVENTORY_MARK_TYPE_NONE );
			pBtn->SetActive();
		}
	}
}

void MyInventoryInfoWnd::SetPresentCostumeBox( int iValue1, int iValue2 )
{
	// icon btn
	CostumeShopInfo *pInfo = g_CostumeShopInfoMgr.GetCostumeMachineInfo( iValue1 );
	if( pInfo )
	{
		ioHashString szIcon = pInfo->GetIconName();
		if( !szIcon.IsEmpty() )
		{
			InventoryIconBtn *pBtn = dynamic_cast< InventoryIconBtn* > ( FindChildWnd( ID_ITEM_ICON ) );
			if( pBtn )
			{
				pBtn->SetIcon( g_UIImageSetMgr.CreateImageByFullName( szIcon ), FLOAT1, FLOAT1 );
				pBtn->SetGradeType( pInfo->GetGradeType() );
				pBtn->SetMarkType( INVENTORY_MARK_TYPE_NONE );
				pBtn->SetActive();
			}
		}
	}
}

void MyInventoryInfoWnd::SetPresentAccessory( int iValue1, int iValue2 )
{
	const ioItem *pItem = g_ItemMaker.GetItemConst( iValue1, __FUNCTION__ );
	if( pItem )
	{
		ioHashString szIcon = pItem->GetItemLargeIconName();
		if( !szIcon.IsEmpty() )
		{
			InventoryIconBtn *pBtn = dynamic_cast< InventoryIconBtn* > ( FindChildWnd( ID_ITEM_ICON ) );
			if( pBtn )
			{
				pBtn->SetIcon( g_UIImageSetMgr.CreateImageByFullName( szIcon ), FLOAT1, FLOAT1 );
				pBtn->SetGradeType( pItem->GetGradeType() );
				pBtn->SetMarkType( INVENTORY_MARK_TYPE_NONE );
				pBtn->SetActive();
			}
		}
	}
}

void MyInventoryInfoWnd::SetPresentSpiritItem( int iValue1, int iValue2 )
{
	// icon btn
	ioHashString szIcon = g_SpiritMgr.GetIconName( iValue1 );
	if( !szIcon.IsEmpty() )
	{
		InventoryIconBtn *pBtn = dynamic_cast< InventoryIconBtn* > ( FindChildWnd( ID_ITEM_ICON ) );
		if( pBtn )
		{
			pBtn->SetIcon( g_UIImageSetMgr.CreateImageByFullName( szIcon ), FLOAT1, FLOAT1 );
			pBtn->SetMarkType( INVENTORY_MARK_TYPE_NONE );
			pBtn->SetActive();
		}
	}
}

bool MyInventoryInfoWnd::CheckPresentSell( int iType, int iValue )
{
	ioUserEtcItem *pUserEtcItem = g_MyInfo.GetUserEtcItem();
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem is NULL.", __FUNCTION__ );
		return false;
	}

	ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( iType );
	if( !pEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pEtcItem is NULL. :%d", __FUNCTION__, iType );
		return false;
	}

	ioUserEtcItem::ETCITEMSLOT kItemSlot;
	bool bExist = pUserEtcItem->GetEtcItem( iType, kItemSlot );

	// check max
	ioEtcItem::UseType eUseType = (ioEtcItem::UseType) ( iType/ioEtcItem::USE_TYPE_CUT_VALUE );
	if( pEtcItem->GetMaxUse() != 0 ) // 특별한  max 조건
	{
		if( !pEtcItem->IsBuyCondition( kItemSlot.GetUse() ) )
			return false;
	}
	else if( pEtcItem->GetType() ==  ioEtcItem::EIT_ETC_CHAR_SLOT_EXTEND ) // 용병슬롯 over 체크
	{
		int iNewCnt = g_MyInfo.GetCurMaxCharSlot() + iValue;
		if( iNewCnt > g_MyInfo.GetLimiteMaxCharSlot() )
			return false;
	}
	else if( pEtcItem->GetType() == ioEtcItem::EIT_ETC_FISHING_BAIT ) 
	{
		int iNewCnt = 0;
		ioUserEtcItem::ETCITEMSLOT kSlot;
		if( pUserEtcItem->GetEtcItem( ioEtcItem::EIT_ETC_FISHING_BAIT, kSlot ) )
			iNewCnt = kSlot.GetUse();

		iNewCnt += iValue;
		if( iNewCnt > 100000 )
			return false;
	}
	else if( pEtcItem->GetType() == ioEtcItem::EIT_ETC_FISHING_MOON_BAIT ) 
	{
		int iNewCnt = 0;
		ioUserEtcItem::ETCITEMSLOT kSlot;
		if( pUserEtcItem->GetEtcItem( ioEtcItem::EIT_ETC_FISHING_MOON_BAIT, kSlot ) )
			iNewCnt = kSlot.GetUse();

		iNewCnt += iValue;
		if( iNewCnt > 100000 )
			return false;
	}
	else if( eUseType == ioEtcItem::UT_COUNT )
	{
		if(  (kItemSlot.GetUse() + iValue) > ioEtcItem::MAX_COUNT )
			return false;
	}
	else if( eUseType == ioEtcItem::UT_TIME )
	{
		if( (kItemSlot.GetUse() + iValue > ioEtcItem::MAX_TIME ) )
			return false;
	}
	else if( eUseType == ioEtcItem::UT_ETERNITY || eUseType == ioEtcItem::UT_ONCE )
	{
		if( kItemSlot.GetUse() != 0 )
			return false;
	}
	else if( eUseType == ioEtcItem::UT_DATE )
	{
		if( bExist && kItemSlot.m_iType == ioEtcItem::EIT_ETC_SOLDIER_EXP_BONUS )
		{
			return false;
		}
		else if( bExist && kItemSlot.m_iValue1 == 0 && kItemSlot.m_iValue2 == 0 )
		{
			return false;
		}
		else
		{
			time_t kLimitTime = DateHelp::ConvertSecondTime( kItemSlot.GetYear(), kItemSlot.GetMonth(), kItemSlot.GetDay(), kItemSlot.GetHour(), kItemSlot.GetMinute(), 0 );
			time_t kCurServerTime = g_MyInfo.GetServerDate();
			kCurServerTime += ( REALGETTIME()/1000 );// 현재까지의 클라이언트초를 넣는다.

			enum { DAY_SEC = 86400, };
			int iGapSec = kLimitTime - kCurServerTime + (iValue*DAY_SEC);
			if( ( iGapSec/DAY_SEC ) >= ioEtcItem::MAX_DAY )
				return false;
		}
	}
	else
	{
		LOG.PrintTimeAndLog( 0, "%s UseType is wrong. :%d", __FUNCTION__, (int) eUseType );
		return false;
	}

	return true;
}

void MyInventoryInfoWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	CheckPowerUpHeroInfo();
}

void MyInventoryInfoWnd::UISoldierSelectWndBtnUp( int iSoldierArray )
{
	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*> ( GetParent() );
	if( !pInvenWnd )
		return;
	
	int iClassType = g_MyInfo.GetClassType( iSoldierArray );
	if( iClassType == -1 )
		return;
	
	if( g_MyInfo.IsCharExerciseStyle( iSoldierArray, EXERCISE_PCROOM ) && pInvenWnd->GetCurTabID() == MyInventoryWnd::ID_DECO_TAB_BTN )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "PC방 용병은 선택 할 수 없습니다." );
		return;
	}

	pInvenWnd->ChangeClassType( iClassType );

	switch( m_iMainType )
	{
	case INVENTORY_ITEM_ETC:
	case INVENTORY_ITEM_PRESENT:
		SetInvenItemInfo( iClassType, m_ItemInfo );   // Reload
		break;
	}
}

void MyInventoryInfoWnd::SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate /*= false */ )
{
	RECT rcRect;
	::SetRect( &rcRect, 0, 0, 0, 0 );

	if( !GetParent() || m_bWidthParent )
		rcRect.right = iParentWidth;
	else
		rcRect.right = m_iDefWidth;

	if( !GetParent() || m_bHeightParent )
		rcRect.bottom = iParentHeight;
	else
		rcRect.bottom = m_iDefHeight;

	rcRect.left = m_iDefXPos;
	switch( m_XPosAlign )
	{
	case UI_CENTER_ALIGN:
		rcRect.left += ( iParentWidth - rcRect.right ) / 2;
		break;
	case UI_RATE_ALIGN:
		rcRect.left *= m_fScreenRateX;
		break;
	case UI_RIGHT_ALIGN:
		rcRect.left += iParentWidth - rcRect.right;
		break;
	}

	rcRect.top = m_iDefYPos;
	switch( m_YPosAlign )
	{
	case UI_CENTER_ALIGN:
		rcRect.top += ( iParentHeight - rcRect.bottom ) / 2;
		break;
	case UI_RATE_ALIGN:
		rcRect.top *= m_fScreenRateY;
		break;
	case UI_BOTTOM_ALIGN:
		rcRect.top += iParentHeight - rcRect.bottom;
		break;
	}

	rcRect.right += rcRect.left;
	rcRect.bottom += rcRect.top;

	SetWndRect( rcRect );

	if( bChildMove && HasChild() )
	{
		iParentWidth  = GetWidth();
		iParentHeight = GetHeight();

		m_bNowGoToTopReserving = true;

		ioWndList::iterator iter = m_ChildList.begin();
		for( ; iter!=m_ChildList.end() ; ++iter )
		{
			if ( (*iter)->GetID() != ID_SOLDIER_WND && (*iter)->GetID() != ID_COSTUME_SOLDIER_WND )
				(*iter)->SetPositionToDefault( iParentWidth, iParentHeight, bChildMove, bCreate );
		}

		m_bNowGoToTopReserving = false;
		ExecuteReservedChildGoToTop();
	}
}

void MyInventoryInfoWnd::CheckPowerUpHeroInfo()
{
	if ( m_bShowPowerUpToolTip )
	{
		if( !m_pPowerUpToolTip )
			return;

		ioLSMouse *pMouse = g_App.GetMouse();
		if( !pMouse )
			return;

		int iXPos = GetDerivedPosX() + SOLDIER_TITLE_X_OFFSET + m_SoldierInfo[1].GetFullWidth()/2 + POWERUP_TOOLTIP_OFFSET_X;
		int iYPos = GetDerivedPosY() + SOLDIER_TITLE_Y_OFFSET + POWERUP_TOOLTIP_OFFSET_Y + SOLDIER_LINE_GAP;

		RECT rect;
		rect.left	= iXPos;
		rect.top	= iYPos;
		rect.right	= iXPos + m_pPowerUpToolTip->GetWidth();
		rect.bottom	= iYPos + m_pPowerUpToolTip->GetHeight();
		
		if( ::PtInRect( &rect, pMouse->GetMousePos() ) )
		{
			if ( pMouse->IsLBtnDown() )
			{
				ManualListWnd *pManualWnd = dynamic_cast<ManualListWnd*> ( g_GUIMgr.FindWnd( MANUAL_LIST_WND ) );
				if( pManualWnd )
					pManualWnd->ShowWndByCurManual( MLN_POWERUP_MANUAL );
			}
			else
			{
				PowerUpToolTip* pToolTip = dynamic_cast< PowerUpToolTip* >(FindChildWnd( ID_CHAR_GRADE_TOOLTIP ) );
				if ( pToolTip && !pToolTip->IsShow() )
				{
					pToolTip->SetInfo( PowerUpToolTip::Power_Char, m_iClassType );
					pToolTip->ShowWnd();
				}
				m_pPowerUpToolTip->SetColor( m_dwPowerUpToolTipOverColor );
			}
		}
		else
		{
			HideChildWnd( ID_CHAR_GRADE_TOOLTIP );
			m_pPowerUpToolTip->SetColor( m_dwPowerUpToolTipLeaveColor );
		}
	}
}

void MyInventoryInfoWnd::SetPresentBonusCash( int iValue1, int iValue2 )
{
	// icon btn
	ioUIRenderImage *pImg = g_UIImageSetMgr.CreateImageByFullName( ioHashString( "UIIconPack101#Cash" ) );
	InventoryIconBtn *pBtn = dynamic_cast< InventoryIconBtn* > ( FindChildWnd( ID_ITEM_ICON ) );
	if( pBtn )
	{
		pBtn->SetIcon( pImg, FLOAT1, FLOAT1 );
		pBtn->SetMarkType( INVENTORY_MARK_TYPE_NONE );
		pBtn->SetActive();
	}
}
//////////////////////////////////////////////////////////////////////////
SoldierDismissalWnd::SoldierDismissalWnd()
{
	m_pBottomFrm = NULL;
	m_pEditFrm   = NULL;
	m_pIconBack  = NULL;
	m_pFeelIcon  = NULL;
	m_pCharIcon  = NULL;
	m_pCharSubIcon  = NULL;

	m_pPreEdit   = NULL;
	m_pDismissalEdit = NULL;
	m_iClassType = -1;
	m_nGradeType = 0;
}

SoldierDismissalWnd::~SoldierDismissalWnd()
{
	SAFEDELETE( m_pBottomFrm );
	SAFEDELETE( m_pEditFrm );
	SAFEDELETE( m_pIconBack );
	SAFEDELETE( m_pFeelIcon );
	SAFEDELETE( m_pCharIcon );
	SAFEDELETE( m_pCharSubIcon );
}

void SoldierDismissalWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "BottomFrm" )
	{
		SAFEDELETE( m_pBottomFrm );
		m_pBottomFrm = pFrame;
	}
	else if( szType == "EditFrm" )
	{
		SAFEDELETE( m_pEditFrm );
		m_pEditFrm = pFrame;
	}
	else
	{
		ioWnd::AddRenderFrame( szType, pFrame );
	}
}

void SoldierDismissalWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "IconBack" )
	{
		SAFEDELETE( m_pIconBack );
		m_pIconBack = pImage;
	}
	else if( szType == "FeelIcon" )
	{
		SAFEDELETE( m_pFeelIcon );
		m_pFeelIcon = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void SoldierDismissalWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_szDisassembleFail = xElement.GetStringAttribute_e( "DisassembleFail" );
}

void SoldierDismissalWnd::SetDismissalChar( int iClassType )
{
	m_iClassType = iClassType;
	m_DismissalType = SDT_DISMISSAL;
	m_nGradeType = g_UIImageRenderMgr.GetPowerUpGradeType( m_iClassType );

	if( !CheckDismissal() )
		return;

	SAFEDELETE( m_pCharIcon );
	SAFEDELETE( m_pCharSubIcon );
	m_pCharIcon = g_MyInfo.GetMySoldierIcon( m_iClassType );
	m_pCharSubIcon = g_MyInfo.GetSoldierSubIcon( m_iClassType );

	if( g_MyInfo.IsCharMortmain( m_iClassType ) )
	{	
		SetSize( GetWidth(), MORTMAIN_HEIGHT );
		ShowChildWnd( ID_DISMISSAL_EDIT );
	}
	else
	{
		SetSize( GetWidth(), NORMAL_HEIGHT );
		HideChildWnd( ID_DISMISSAL_EDIT );
	}

	ShowWnd();
}

void SoldierDismissalWnd::SetDisassembleChar( int iClassType )
{
	m_iClassType = iClassType;
	m_DismissalType = SDT_DISASSEMBLE;
	m_nGradeType = g_UIImageRenderMgr.GetPowerUpGradeType( m_iClassType );

	if( !CheckDisassemble() )
		return;

	SAFEDELETE( m_pCharIcon );	
	SAFEDELETE( m_pCharSubIcon );
	m_pCharIcon = g_MyInfo.GetMySoldierIcon( m_iClassType );
	m_pCharSubIcon = g_MyInfo.GetSoldierSubIcon( m_iClassType );

	if( g_MyInfo.IsCharMortmain( m_iClassType ) )
	{	
		SetSize( GetWidth(), DIS_MORTMAIN_HEIGHT );
		ShowChildWnd( ID_DISMISSAL_EDIT );
	}
	else
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, m_szDisassembleFail.c_str() );
		return;
	}

	ShowWnd();
}

void SoldierDismissalWnd::SetDisassemblePresentChar( int iType, int iPresentIndex, int iPresentSlot, ioHashString szID )
{
	m_iPresentIndex = iPresentIndex;
	m_iPresentSlot = iPresentSlot;
	m_szSubscriptionID = szID;
	if( iType == INVENTORY_ITEM_PRESENT )
		m_DismissalType = SDT_DISASSEMBLE_PRESENT;
	else if( iType == INVENTORY_ITEM_SUBSCRIPTION )
		m_DismissalType = SDT_DISASSEMBLE_SUBSCRIPTION;
	else
		return;

	ioHashString szSendID;
	short sPresentType, sPresentMent;
	int iPresentValue1, iPresentValue2, iPresentValue3, iPresentValue4;
	DWORD dwLimitDate;

	g_PresentMgr.GetPresentDataToIndex( iPresentIndex, iPresentSlot, szSendID, sPresentType, sPresentMent, iPresentValue1, iPresentValue2, iPresentValue3, iPresentValue4, dwLimitDate );
	if( iPresentValue2 != 0 )
		return;

	m_iClassType = iPresentValue1;
	m_nGradeType = g_UIImageRenderMgr.GetPowerUpGradeType( m_iClassType );

	SAFEDELETE( m_pCharIcon );	
	SAFEDELETE( m_pCharSubIcon );
	m_pCharIcon = g_MyInfo.GetMySoldierIcon( m_iClassType );
	m_pCharSubIcon = g_MyInfo.GetSoldierSubIcon( m_iClassType );

	SetSize( GetWidth(), DIS_PRESENT_HEIGHT );
	HideChildWnd( ID_DISMISSAL_EDIT );
	ShowWnd();
}

bool SoldierDismissalWnd::CheckDismissal()
{
	int iCharArray = -1;
	iCharArray = g_MyInfo.GetClassArray( m_iClassType );

	if( g_MyInfo.GetCharCount() <= 1 )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		return false;
	}

	if( g_MyInfo.IsTutorialUser() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(5) );
		return false;
	}

	if( iCharArray == g_MyInfo.GetSelectCharArray() )
	{
		bool bCheck = true;
		MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
		if( pInvenWnd && pInvenWnd->GetPlayStage() )
		{
			ioMyRoomMode *pMyRoom = ToMyRoomMode( pInvenWnd->GetPlayStage()->GetPlayMode() );
			if( pMyRoom && pMyRoom->GetMyRoomType() == MRT_LOBBY )
				bCheck = false;
		}

		if( bCheck )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
			return false;
		}
	}

	if( g_MyInfo.IsCharRentalToClassType( m_iClassType ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(6) );
		return false;
	}

	if( g_MyInfo.IsCharExerciseStyleToClassType( m_iClassType, EXERCISE_RENTAL ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(7) );
		return false;
	}	

	int iActiveChar = g_MyInfo.GetActiveCharCount();
	int iExerciseActiveChar = g_MyInfo.GetActiveExerciseCharCount();
	int iNoneExerciseActiveChar = iActiveChar - iExerciseActiveChar;
	if(  g_MyInfo.IsCharActive( iCharArray )     && 
		!g_MyInfo.IsCharExercise( iCharArray )   &&
		iNoneExerciseActiveChar == 1             )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
		return false;
	}

	if( iCharArray == -1 )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4) );
		return false;
	}

	return true;	
}

void SoldierDismissalWnd::SendDismissal()
{
	if( !CheckDismissal() )
		return;

	DWORD dwCharIndex = 0;
	dwCharIndex = g_MyInfo.GetCharIndex( g_MyInfo.GetClassArray( m_iClassType ) );

	if( dwCharIndex == 0 )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		LOG.PrintTimeAndLog( 0, "Send Dismissal Error" );
		return;
	}

	TCPNetwork::MouseBusy( true );
	SP2Packet kPacket( CTPK_CHAR_DELETE );
	kPacket << dwCharIndex;
	TCPNetwork::SendToServer( kPacket );
}

void SoldierDismissalWnd::iwm_show()
{
	switch( m_DismissalType )
	{
	case SDT_DISMISSAL:
		SetTitleText( "용병해고" );
		break;
	case SDT_DISASSEMBLE:
	case SDT_DISASSEMBLE_PRESENT:
	case SDT_DISASSEMBLE_SUBSCRIPTION:
		SetTitleText( "용병분해" );
		break;
	}

	if( m_pBottomFrm )
		m_pBottomFrm->SetPosition( 0, GetHeight() - BOTTOM_FRAME_GAP );

	ioWnd *pDismissalBtn = FindChildWnd( ID_DISMISSAL );
	if( pDismissalBtn )
	{
		switch( m_DismissalType )
		{
		case SDT_DISMISSAL:
			pDismissalBtn->SetTitleText( "해고" );
			break;
		case SDT_DISASSEMBLE:
		case SDT_DISASSEMBLE_PRESENT:
		case SDT_DISASSEMBLE_SUBSCRIPTION:
			pDismissalBtn->SetTitleText( "분해" );
			break;
		}

		pDismissalBtn->SetWndPos( pDismissalBtn->GetXPos(), GetHeight() - BUTTON_GAP );
	}

	ioWnd *pCancelBtn = FindChildWnd( ID_CANCEL );
	if( pCancelBtn )
		pCancelBtn->SetWndPos( pCancelBtn->GetXPos(), GetHeight() - BUTTON_GAP );

	ioEdit *pEdit = dynamic_cast<ioEdit*>( FindChildWnd( ID_DISMISSAL_EDIT ) );
	if( pEdit && pEdit->IsShow() )
	{
		pEdit->SetWndPos( pEdit->GetXPos(), GetHeight() - EDIT_GAP );

		m_pPreEdit = ioEdit::m_pKeyFocusEdit;
		pEdit->ClearString();
		pEdit->SetKeyFocus();
		g_InputBox.SetString( "" );

		m_pDismissalEdit = pEdit;
	}
}

void SoldierDismissalWnd::iwm_hide()
{
	if( m_pPreEdit )	
	{
		ioWnd *pParent = m_pPreEdit->GetParent();
		if( pParent )
		{
			if( pParent->IsShow() )
				((ioEdit*)m_pPreEdit)->SetKeyFocus();	
		}
	}	
	m_pPreEdit = NULL;
}

void SoldierDismissalWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	char szText1[MAX_PATH] = "";
	char szText2[MAX_PATH] = "";
	char szText3[MAX_PATH] = "";

	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
	case ID_CANCEL:
		if( cmd == IOBN_BTNUP )
			HideWnd();
		break;
	case ID_DISMISSAL:
		if( cmd == IOBN_BTNUP )
		{
			if( m_DismissalType != SDT_DISASSEMBLE_PRESENT && 
				m_DismissalType != SDT_DISASSEMBLE_SUBSCRIPTION && 
				g_MyInfo.IsCharMortmain( m_iClassType ) )
			{
				switch( m_DismissalType )
				{
				case SDT_DISMISSAL:
					StringCbCopy( szText1, sizeof(szText1), STR(1) );
					StringCbCopy( szText2, sizeof(szText2), STR(2) );
					StringCbCopy( szText3, sizeof(szText3), STR(3) );
					break;
				case SDT_DISASSEMBLE:
					StringCbCopy( szText1, sizeof(szText1), "영구용병분해를 입력하세요" );
					StringCbCopy( szText2, sizeof(szText2), "영구용병분해" );
					StringCbCopy( szText3, sizeof(szText3), "영구용병분해를 입력하세요" );
					break;
				}

				if( !m_pDismissalEdit || !m_pDismissalEdit->IsShow() )
				{
					if( m_DismissalType == SDT_DISMISSAL )
						g_GUIMgr.SetMsgBox( MB_OK, NULL, szText1 );
					return;
				}

				ioEdit *pEdit = dynamic_cast<ioEdit*>(m_pDismissalEdit);
				if( strcmp( pEdit->GetText(), szText2 ) != 0 )
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, szText3 );
					return;
				}
			}

			if( m_DismissalType == SDT_DISMISSAL )
				SendDismissal();
			else if( m_DismissalType == SDT_DISASSEMBLE )
				SendDisassemble();
			else
				SendDisassemblePresent();

			HideWnd();
		}
		break;
	case ID_DISMISSAL_EDIT:
		if( cmd == IOED_INPUT )
		{
			if( param == 0 || param == 1 )
			{
				ioEdit *pEdit = dynamic_cast<ioEdit*>(pWnd);
				pEdit->SetKeyFocus();
			}
		}
		break;
	}
}

void SoldierDismissalWnd::OnRender()
{
	ioWnd::OnRender();
	
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_pBottomFrm )
		m_pBottomFrm->Render( iXPos, iYPos, UI_RENDER_MULTIPLY );

	if( m_pIconBack )
		m_pIconBack->Render( iXPos + 125, iYPos + 98, UI_RENDER_MULTIPLY );

	g_UIImageRenderMgr.RenderGradeIconBack( m_nGradeType, iXPos + 125, iYPos + 98 );

	if( m_pCharIcon )
		m_pCharIcon->Render( iXPos + 125, iYPos + 98 );
	if( m_pCharSubIcon )
		m_pCharSubIcon->Render( iXPos + 125, iYPos + 98 );
	if( m_pFeelIcon )
		m_pFeelIcon->Render( iXPos + 125, iYPos + 98 );
	
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetTextColor( TCT_DEFAULT_RED );

	if( m_DismissalType == SDT_DISMISSAL )
		g_FontMgr.PrintText( iXPos + 125, iYPos + 146, FONT_SIZE_17, STR(1), g_MyInfo.GetClassName( m_iClassType, g_MyInfo.GetCharPowerUpGradeByClassType( m_iClassType ) ) );
	else
		g_FontMgr.PrintText( iXPos + 125, iYPos + 146, FONT_SIZE_17, "%s 분해", g_MyInfo.GetClassName( m_iClassType, g_MyInfo.GetCharPowerUpGradeByClassType( m_iClassType ) ) );

	ioComplexStringPrinter kPrinter;
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );

	if( m_DismissalType == SDT_DISMISSAL )
		kPrinter.AddTextPiece( FONT_SIZE_13, STR(2) );
	else
		kPrinter.AddTextPiece( FONT_SIZE_13, "용병 분해시 해당 용병은 사라지지만" );

	kPrinter.PrintFullText( iXPos + 125, iYPos + 169, TAT_CENTER );
	kPrinter.ClearList();

	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetTextColor( TCT_DEFAULT_GREEN );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(3) );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(4) );
	kPrinter.PrintFullText( iXPos + 125, iYPos + 189, TAT_CENTER );
	kPrinter.ClearList();

	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(5) );
	kPrinter.PrintFullText( iXPos + 125, iYPos + 209, TAT_CENTER );
	kPrinter.ClearList();

	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );

	char szConvertNum[MAX_PATH] = "";
	bool bMortmain = false;

	if( m_DismissalType == SDT_DISMISSAL )
	{
		kPrinter.AddTextPiece( FONT_SIZE_13, STR(6) );
		kPrinter.SetTextColor( TCT_DEFAULT_BLUE );

		int iResellPeso= 0;
		int iCharArray = g_MyInfo.GetClassArray( m_iClassType );
		if( g_MyInfo.IsCharMortmain( m_iClassType ) )
			iResellPeso  = g_ClassPrice.GetMortmainCharResllPeso( m_iClassType );
		else if( g_MyInfo.IsCharExercise( iCharArray ) )
			iResellPeso = 0;
		else
			iResellPeso  = g_ClassPrice.GetTimeCharResellPeso( m_iClassType, g_MyInfo.GetCharTotalSecond( iCharArray ) );

		Help::ConvertNumToStrComma( iResellPeso, szConvertNum, sizeof( szConvertNum ) );

		kPrinter.AddTextPiece( FONT_SIZE_13, STR(7), szConvertNum );
	}
	else
	{
		int iMinCnt = 0;
		int iCharArray = g_MyInfo.GetClassArray( m_iClassType );
		DWORD dwTime = g_MyInfo.GetCharTotalSecond(iCharArray) / 60 / 60;
		if( g_MyInfo.IsCharMortmain( m_iClassType ) )
			bMortmain = true;

		if ( bMortmain )
		{
			kPrinter.AddTextPiece( FONT_SIZE_13, STR(12) );
			kPrinter.PrintFullText( iXPos + 125, iYPos + 233, TAT_CENTER );
			kPrinter.ClearList();
			kPrinter.SetTextColor( TCT_DEFAULT_BLUE );
		}
		else
		{
			kPrinter.AddTextPiece( FONT_SIZE_13, "분해 : " );
			kPrinter.SetTextColor( TCT_DEFAULT_BLUE );
		}

		char szKey[MAX_PATH] = "";
		Help::ConvertNumToStrComma( g_SpiritMgr.GetDecomposeQuantity( m_iClassType ), szConvertNum, sizeof( szConvertNum ) );
		sprintf_e( szKey, "%s %s개획득", g_SpiritMgr.GetDecomposeSpiritName(m_iClassType).c_str(), szConvertNum );
		kPrinter.AddTextPiece( FONT_SIZE_13, szKey );
	}

	if ( bMortmain )
	{
		kPrinter.SetTextStyle( TS_NORMAL );
		kPrinter.SetBkColor( 0, 0, 0 );
		kPrinter.PrintFullText( iXPos + 125, iYPos + 253, TAT_CENTER );
		kPrinter.ClearList();

		kPrinter.SetTextStyle( TS_NORMAL );
		kPrinter.SetBkColor( 0, 0, 0 );
		kPrinter.SetTextColor( TCT_DEFAULT_BLUE );
		kPrinter.AddTextPiece( FONT_SIZE_13, "소울스톤 획득" );
		kPrinter.PrintFullText( iXPos + 125, iYPos + 272, TAT_CENTER );
		kPrinter.ClearList();
	}
	else
		kPrinter.PrintFullText( iXPos + 125, iYPos + 239, TAT_CENTER );

	kPrinter.ClearList();

	if( m_DismissalType == SDT_DISMISSAL )
	{
		kPrinter.SetBkColor( 0, 0, 0 );
		kPrinter.SetTextStyle( TS_NORMAL );
		kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
		kPrinter.AddTextPiece( FONT_SIZE_13, STR(8) );

		kPrinter.PrintFullText( iXPos + 125, iYPos + 259, TAT_CENTER );
		kPrinter.ClearList();
	}

	if( m_DismissalType != SDT_DISASSEMBLE_PRESENT &&
		m_DismissalType != SDT_DISASSEMBLE_SUBSCRIPTION && 
		g_MyInfo.IsCharMortmain( m_iClassType ) )
	{
		int iText1YPos = iYPos + 289;
		int iText2YPos = iYPos + 312;

		int iEditFrmXPos = iXPos + 17;
		int iEditFrmYPos = iYPos + 308;

		kPrinter.SetBkColor( 0, 0, 0 );
		kPrinter.SetTextStyle( TS_NORMAL );
		kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );

		kPrinter.AddTextPiece( FONT_SIZE_13, "[" );
		kPrinter.SetTextColor( TCT_DEFAULT_RED );

		if( m_DismissalType == SDT_DISMISSAL )
			kPrinter.AddTextPiece( FONT_SIZE_13, STR(9) );
		else
			kPrinter.AddTextPiece( FONT_SIZE_13, "영구용병분해" );

		kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );

		if( m_DismissalType == SDT_DISMISSAL )
			kPrinter.AddTextPiece( FONT_SIZE_13, STR(10) );
		else
		{
			kPrinter.AddTextPiece( FONT_SIZE_13, "] 입력후 분해를 누르세요" );
			iText1YPos = iYPos + 300;
			iEditFrmYPos = iYPos + 319;
		}
		
		kPrinter.PrintFullText( iXPos + 125, iText1YPos, TAT_CENTER );
		kPrinter.ClearList();

		if( m_pEditFrm )
			m_pEditFrm->Render( iEditFrmXPos, iEditFrmYPos );

		if( m_pDismissalEdit != ioEdit::m_pKeyFocusEdit )
		{
			kPrinter.SetBkColor( 0, 0, 0 );
			kPrinter.SetTextStyle( TS_NORMAL );
			kPrinter.SetTextColor( TCT_DEFAULT_LIGHTGRAY );

			if( m_DismissalType == SDT_DISMISSAL )
				kPrinter.AddTextPiece( FONT_SIZE_12, STR(11) );
			else
			{
				kPrinter.AddTextPiece( FONT_SIZE_12, "영구용병분해를 입력하세요" );
				iText2YPos = iYPos + 292;
			}

			kPrinter.PrintFullText( iXPos + 125, iText2YPos, TAT_CENTER );
			kPrinter.ClearList();
		}
	}
}

bool SoldierDismissalWnd::CheckDisassemble()
{
	int iCharArray = -1;
	iCharArray = g_MyInfo.GetClassArray( m_iClassType );

	if( g_MyInfo.GetCharCount() <= 1 )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "마지막 보유 용병은#분해할 수 없습니다." );
		return false;
	}

	if( g_MyInfo.IsTutorialUser() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "튜토리얼 중에는 용병 분해가#불가능합니다." );
		return false;
	}

	if( iCharArray == g_MyInfo.GetSelectCharArray() )
	{
		bool bCheck = true;
		MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
		if( pInvenWnd && pInvenWnd->GetPlayStage() )
		{
			ioMyRoomMode *pMyRoom = ToMyRoomMode( pInvenWnd->GetPlayStage()->GetPlayMode() );
			if( pMyRoom && pMyRoom->GetMyRoomType() == MRT_LOBBY )
				bCheck = false;
		}

		if( bCheck )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "출전중인 용병은#분해할 수 없습니다." );
			return false;
		}
	}

	if( g_MyInfo.IsCharRentalToClassType( m_iClassType ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "전투체험 설정이 되어있는 용병은#분해가 불가능합니다." );
		return false;
	}

	if( g_MyInfo.IsCharExerciseStyleToClassType( m_iClassType, EXERCISE_RENTAL ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "전투체험 용병으로는 불가능합니다." );
		return false;
	}	

	int iActiveChar = g_MyInfo.GetActiveCharCount();
	int iExerciseActiveChar = g_MyInfo.GetActiveExerciseCharCount();
	int iNoneExerciseActiveChar = iActiveChar - iExerciseActiveChar;
	if(  g_MyInfo.IsCharActive( iCharArray )     && 
		!g_MyInfo.IsCharExercise( iCharArray )   &&
		iNoneExerciseActiveChar == 1             )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "고용시간이 남아있는 마지막 용병은#분해할 수 없습니다." );
		return false;
	}

	if( iCharArray == -1 )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "보유하고 있지 않은 용병입니다." );
		return false;
	}

	// 용병 시간 체크
	int iTotalLimit = g_MyInfo.GetCharTotalSecond( iCharArray ) / 60 / 60;
	if( !g_MyInfo.IsCharMortmain( m_iClassType ) && iTotalLimit < 1 )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "남은 시간이 적어 분해할 수 없습니다" );
		return false;
	}

	return true;	
}

void SoldierDismissalWnd::SendDisassemble()
{
	if( !CheckDisassemble() )
		return;

	DWORD dwCharIndex = 0;
	dwCharIndex = g_MyInfo.GetCharIndex( g_MyInfo.GetClassArray( m_iClassType ) );

	if( dwCharIndex == 0 )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "예외 오류가 발생하였습니다." );
		return;
	}
	TCPNetwork::MouseBusy( true );
	SP2Packet kPacket( CTPK_SPIRIT_DECOMPOSE );
	kPacket << m_iClassType;
	TCPNetwork::SendToServer( kPacket );
}

void SoldierDismissalWnd::SendDisassemblePresent()
{
	if( m_DismissalType == SDT_DISASSEMBLE_PRESENT )
	{
		if( m_iPresentIndex != 0 || m_iPresentSlot != 0 )
		{
			SP2Packet kPacket( CTPK_PRESENT_DISASSEMBLE );
			kPacket << m_iPresentIndex << m_iPresentSlot;
			TCPNetwork::SendToServer( kPacket );
			TCPNetwork::MouseBusy( true );
		}
	}
	else if( m_DismissalType == SDT_DISASSEMBLE_SUBSCRIPTION )
	{
		SP2Packet kPacket( CTPK_SUBSCRIPTION_DISASSEMBLE );
		kPacket << m_iPresentIndex << m_szSubscriptionID;
		TCPNetwork::SendToServer( kPacket );
		TCPNetwork::MouseBusy( true );
	}
}
//////////////////////////////////////////////////////////////////////////
ItemRecvSellInfoWnd::ItemRecvSellInfoWnd()
{
	m_pBackEffect = NULL;
	m_pIcon	= NULL;
	m_pSubIcon = NULL;
	m_pNumText = NULL;
	m_pPlusText = NULL;
	m_pCustomIcon = NULL;
	m_pCostumeMark = NULL;

	m_bExtraItemCustom = false;
	m_bIsCostume = false;
	m_nGradeType = 0;
}

ItemRecvSellInfoWnd::~ItemRecvSellInfoWnd()
{
	SAFEDELETE(m_pBackEffect);
	SAFEDELETE(m_pIcon);
	SAFEDELETE(m_pSubIcon);
	SAFEDELETE(m_pNumText);
	SAFEDELETE(m_pPlusText);
	SAFEDELETE(m_pCustomIcon);
	SAFEDELETE(m_pCostumeMark);
}

void ItemRecvSellInfoWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "back_effect" )
	{
		SAFEDELETE( m_pBackEffect );
		m_pBackEffect = pImage;
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
	else if( szType == "custom_image" )
	{
		SAFEDELETE( m_pCustomIcon );
		m_pCustomIcon = pImage;
	}
	else if( szType == "costume_image" )
	{
		SAFEDELETE( m_pCostumeMark );
		m_pCostumeMark = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}


void ItemRecvSellInfoWnd::SetItemRecvSellInfo( int iCommandType, 
											   int iItemInfoType, 
											   const ioHashString szIconName, 
											   const ioHashString szSubIconName,
											   const ComplexStringPrinterVec& pTitleVec,
											   const ComplexStringPrinterVec& pDescVec,
											   int iClassType, 
											   int iReinforce, 
											   bool bExtraItemCustom,
											   bool bIsCostume,
											   int nGradeType  )
{
	m_CommandType = (CommandType)iCommandType;
	m_ItemInfoType = (ItemInfoType)iItemInfoType;
	m_bExtraItemCustom = bExtraItemCustom;
	m_bIsCostume = bIsCostume;
	m_nGradeType = nGradeType;

	SAFEDELETE( m_pIcon );
	SAFEDELETE( m_pSubIcon );
	m_pIcon = g_UIImageSetMgr.CreateImageByFullName( szIconName );
	m_pSubIcon = g_UIImageSetMgr.CreateImageByFullName( szSubIconName );

	m_vTitle = pTitleVec;
	m_vDesc = pDescVec;

	m_iClassType = iClassType;
	m_iReinforce = iReinforce;

	if( !IsShow() )
		ShowWnd();

	ReBatchButton();

	m_sDescMileage.Clear();
	g_ExSoundMgr.PlaySound( ExSound::EST_PRESENT_BOX_RESULT );
}

void ItemRecvSellInfoWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_GO:
		if( cmd == IOBN_BTNUP )
		{
			MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*> ( GetParent() );
			if( pInvenWnd && pInvenWnd->IsShow() )
			{
				HideWnd();

				switch( m_ItemInfoType )
				{
				case ITEM_SOLDIER:
					pInvenWnd->ShowSoldierTabDirect( m_iClassType );
					break;
				case ITEM_EXTRAITEM:
				case ITEM_MEDAL:
					pInvenWnd->ShowExtraItemTabDirect( -1 );
					break;
				case ITEM_DECO:
					pInvenWnd->ShowDecoTabDirect( -1 );
					break;
				case ITEM_ETCITEM:
					pInvenWnd->ShowEtcTabDirect();
					break;
				case ITEM_ALCHEMIC:
					pInvenWnd->ShowAlchemicTabDirect();
					break;
				case ITEM_PET:
					{
						pInvenWnd->HideWnd();
						PetWnd *pPetWnd = dynamic_cast<PetWnd*>( g_GUIMgr.FindWnd(PET_WND) );
						if ( pPetWnd )
							pPetWnd->ShowWnd();
					}
					break;
				case ITEM_COSTUME:
					pInvenWnd->ShowCostumeTabDirect( -1 );
					break;
				case ITEM_ACCESSORY:
					pInvenWnd->ShowAccessoryTabDirect( -1 );
					break;
				}
			}
		}
		break;
	}
}

void ItemRecvSellInfoWnd::OnRender()
{
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	ioWnd::OnRender();

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

	enum
	{
		REINFORCE_Y	= 14,

		TEXT_SIZE	= 14,
		TEXT_GAP	= 4,
	};

	if( m_pBackEffect )
	{
		m_pBackEffect->Render( iXPos+ICON_X, iYPos+ICON_Y, UI_RENDER_MULTIPLY );
	}

	g_UIImageRenderMgr.RenderGradeIconBack( m_nGradeType, iXPos+ICON_X, iYPos+ICON_Y );

	if( m_pIcon )
	{
		m_pIcon->Render( iXPos+ICON_X, iYPos+ICON_Y, UI_RENDER_NORMAL, TFO_BILINEAR );
		if( m_pSubIcon )
			m_pSubIcon->Render( iXPos+ICON_X, iYPos+ICON_Y, UI_RENDER_NORMAL, TFO_BILINEAR );
		if( m_bExtraItemCustom && m_pCustomIcon )
			m_pCustomIcon->Render( iXPos+ICON_X+7, iYPos+ICON_Y-43, UI_RENDER_NORMAL, TFO_BILINEAR );

		// UI코스튬 관련 (코스튬 마크)
		if( m_bIsCostume && m_pCostumeMark )
			m_pCostumeMark->Render( iXPos+ICON_X, iYPos+ICON_Y, UI_RENDER_NORMAL, TFO_BILINEAR );
	}

	// Reinforce
	if( m_pNumText && m_pPlusText && m_iReinforce > 0 )
	{
		int iCurPos = 0;
		int iNewXPos = iXPos+ICON_X;
		int iNewYPos = iYPos+ICON_Y;
		int iDefaultPos = iXPos+ICON_X;

		int iCurYPos = iNewYPos+REINFORCE_Y;

		int iTotalSize = 0;
		if( m_iReinforce < 10 )
			iTotalSize = TEXT_SIZE * 2 - TEXT_GAP;
		else if( m_iReinforce < 100 )
			iTotalSize = TEXT_SIZE * 3 - TEXT_GAP * 2;
		else
			iTotalSize = TEXT_SIZE * 4 - TEXT_GAP * 3;

		iCurPos = iDefaultPos - (iTotalSize / 2);
		m_pPlusText->Render( iCurPos, iCurYPos );

		iCurPos += (TEXT_SIZE - TEXT_GAP);
		m_pNumText->RenderNum( iCurPos, iCurYPos, m_iReinforce, -TEXT_GAP );
	}

	// Title
	for( int i = 0; i < (int)m_vTitle.size(); ++i )
	{
		if( m_vTitle[i].IsEmpty() )
			continue;

		m_vTitle[i].PrintFullText( iXPos+DESC_BIG_X, iYPos+DESC_BIG_Y+(i*23), TAT_CENTER );
	}

	// Desc
	for( int i = 0; i < (int)m_vDesc.size(); ++i )
	{
		if( m_vDesc[i].IsEmpty() )
			continue;

		m_vDesc[i].PrintFullText( iXPos+DESC_X, iYPos+DESC_Y+(i*18), TAT_LEFT );
	}

	if( !m_sDescMileage.IsEmpty() )
	{
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_ORANGE );
		g_FontMgr.PrintText( iXPos+DESC_X, iYPos+DESC_Y+(2*18), FONT_SIZE_17, m_sDescMileage.c_str() );
	}

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GREEN );

	switch( m_CommandType )
	{
	case ITEM_SELL:
		{
			switch( m_ItemInfoType )
			{
			case ITEM_SOLDIER:
				g_FontMgr.PrintText( iXPos+FINAL_DESC_X, iYPos+FINAL_DESC_Y, FONT_SIZE_17, STR(1) );
				break;
			case ITEM_EXTRAITEM:
			case ITEM_ETCITEM:
			case ITEM_PRESENT:
			case ITEM_ALCHEMIC:
			case ITEM_MEDAL:
			case ITEM_PET:
			case ITEM_COSTUME:
			case ITEM_ACCESSORY:
				g_FontMgr.PrintText( iXPos+FINAL_DESC_X, iYPos+FINAL_DESC_Y, FONT_SIZE_17, STR(2) );
				break;
			}
		}
		break;
	case ITEM_DISASSEMBLE:
		g_FontMgr.PrintText( iXPos+FINAL_DESC_X, iYPos+FINAL_DESC_Y, FONT_SIZE_17, "분해가 완료되었습니다" );
		break;
	default:
		g_FontMgr.PrintText( iXPos+FINAL_DESC_X, iYPos+FINAL_DESC_Y, FONT_SIZE_17, STR(3) );
		break;
	}
}

void ItemRecvSellInfoWnd::ReBatchButton()
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

	HideChildWnd(ID_GO);

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

	SetChildActive( ID_GO );
	switch( m_CommandType )
	{
	case ITEM_SELL:
	case ITEM_RECV_NOT_GO:
		SetChildInActive( ID_GO );
		break;
	}
}