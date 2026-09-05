#include "StdAfx.h"

#include "../ioExcavationManager.h"
#include "../ioLadderTeamMgr.h"
#include "../ioDateHelp.h"
#include "../ioShuffleRoomMgr.h"
#include "../ioSubscriptionMgr.h"
#include "../ioMissionManager.h"
#include "../ioPriateRenewalRullet.h"
#include "../Housing/ioBlockSetupManager.h"
#include "../ioRankBattleManager.h"

#include "ioMemoManager.h"
#include "QuestWnd.h"
#include "AbuseProblemWnd.h"
#include "TotalResultMainWnd.h"
#include "HelpWnd.h"
#include "SoldierPracticeWnd.h"
#include "MessengerWnd.h"
#include "RollBookWnd.h"
#include "SoldierSelectWnd.h"
#include "BattleModeWnd.h"

MenuBase::MenuBase()
{
	m_eMenuStyle		= MS_NONE;

	m_pStartNormal		= NULL;
	m_pStartOver		= NULL;
	m_pStartOverAdd		= NULL;
	m_pStartPush		= NULL;

	m_pMiddleNormal		= NULL;
	m_pMiddleOver		= NULL;
	m_pMiddleOverAdd	= NULL;
	m_pMiddlePush		= NULL;

	m_pEndNormal		= NULL;
	m_pEndOver			= NULL;
	m_pEndOverAdd		= NULL;
	m_pEndPush			= NULL;
}

MenuBase::~MenuBase()
{
	SAFEDELETE( m_pStartNormal );
	SAFEDELETE( m_pStartOver );
	SAFEDELETE( m_pStartOverAdd );
	SAFEDELETE( m_pStartPush );

	SAFEDELETE( m_pMiddleNormal );
	SAFEDELETE( m_pMiddleOver );
	SAFEDELETE( m_pMiddleOverAdd );
	SAFEDELETE( m_pMiddlePush );

	SAFEDELETE( m_pEndNormal );
	SAFEDELETE( m_pEndOver );
	SAFEDELETE( m_pEndOverAdd );
	SAFEDELETE( m_pEndPush );
}

void MenuBase::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "StartNormal" )
	{
		SAFEDELETE( m_pStartNormal );
		m_pStartNormal = pFrame;
	}
	else if( szType == "StartOver" )
	{
		SAFEDELETE( m_pStartOver );
		m_pStartOver = pFrame;
	}
	else if( szType == "StartOverAdd" )
	{
		SAFEDELETE( m_pStartOverAdd );
		m_pStartOverAdd = pFrame;
	}
	else if( szType == "StartPush" )
	{
		SAFEDELETE( m_pStartPush );
		m_pStartPush = pFrame;
	}
	else if( szType == "MiddleNormal" )
	{
		SAFEDELETE( m_pMiddleNormal );
		m_pMiddleNormal = pFrame;
	}
	else if( szType == "MiddleOver" )
	{
		SAFEDELETE( m_pMiddleOver );
		m_pMiddleOver = pFrame;
	}
	else if( szType == "MiddleOverAdd" )
	{
		SAFEDELETE( m_pMiddleOverAdd );
		m_pMiddleOverAdd = pFrame;
	}
	else if( szType == "MiddlePush" )
	{
		SAFEDELETE( m_pMiddlePush );
		m_pMiddlePush = pFrame;
	}
	else if( szType == "EndNormal" )
	{
		SAFEDELETE( m_pEndNormal );
		m_pEndNormal = pFrame;
	}
	else if( szType == "EndOver" )
	{
		SAFEDELETE( m_pEndOver );
		m_pEndOver = pFrame;
	}
	else if( szType == "EndOverAdd" )
	{
		SAFEDELETE( m_pEndOverAdd );
		m_pEndOverAdd = pFrame;
	}
	else if( szType == "EndPush" )
	{
		SAFEDELETE( m_pEndPush );
		m_pEndPush = pFrame;
	}
	else
	{
		ioButton::AddRenderFrame( szType, pFrame );	
	}	
}

void MenuBase::OnDrawNormal( int iXPos, int iYPos )
{
	switch( m_eMenuStyle )
	{
	case MS_START:
		{
			if( m_pStartNormal )
				m_pStartNormal->Render( iXPos, iYPos, UI_RENDER_NORMAL );
		}
		break;
	case MS_MIDDLE:
		{
			if( m_pMiddleNormal )
				m_pMiddleNormal->Render( iXPos, iYPos, UI_RENDER_NORMAL );
		}
		break;
	case MS_END:
		{
			if( m_pEndNormal )
				m_pEndNormal->Render( iXPos, iYPos, UI_RENDER_NORMAL );
		}
		break;
	default:
		ioButton::OnDrawNormal( iXPos, iYPos );
		break;
	}
}

void MenuBase::OnDrawOvered( int iXPos, int iYPos )
{
	switch( m_eMenuStyle )
	{
	case MS_START:
		{
			if( m_pStartOver )
				m_pStartOver->Render( iXPos, iYPos, UI_RENDER_NORMAL );
			OnDrawOveredAdd( iXPos, iYPos );
		}
		break;
	case MS_MIDDLE:
		{
			if( m_pMiddleOver )
				m_pMiddleOver->Render( iXPos, iYPos, UI_RENDER_NORMAL );
			OnDrawOveredAdd( iXPos, iYPos );
		}
		break;
	case MS_END:
		{
			if( m_pEndOver )
				m_pEndOver->Render( iXPos, iYPos, UI_RENDER_NORMAL );
			OnDrawOveredAdd( iXPos, iYPos );
		}
		break;
	default:
		ioButton::OnDrawOvered( iXPos, iYPos );
		break;
	}
}

void MenuBase::OnDrawOveredAdd( int iXPos, int iYPos )
{
	switch( m_eMenuStyle )
	{
	case MS_START:
		{
			if( m_pStartOverAdd )
				m_pStartOverAdd->Render( iXPos, iYPos, UI_RENDER_ADD );
		}
		break;
	case MS_MIDDLE:
		{
			if( m_pMiddleOverAdd )
				m_pMiddleOverAdd->Render( iXPos, iYPos, UI_RENDER_ADD );
		}
		break;
	case MS_END:
		{
			if( m_pEndOverAdd )
				m_pEndOverAdd->Render( iXPos, iYPos, UI_RENDER_ADD );
		}
		break;
	default:
		ioButton::OnDrawOveredAdd( iXPos, iYPos );
		break;
	}
}

void MenuBase::OnDrawPushed( int iXPos, int iYPos )
{
	switch( m_eMenuStyle )
	{
	case MS_START:
		{
			if( m_pStartPush )
				m_pStartPush->Render( iXPos, iYPos, UI_RENDER_NORMAL );
		}
		break;
	case MS_MIDDLE:
		{
			if( m_pMiddlePush )
				m_pMiddlePush->Render( iXPos, iYPos, UI_RENDER_NORMAL );
		}
		break;
	case MS_END:
		{
			if( m_pEndPush )
				m_pEndPush->Render( iXPos, iYPos, UI_RENDER_NORMAL );
		}
		break;
	default:
		ioButton::OnDrawPushed( iXPos, iYPos );
		break;
	}
}
//////////////////////
MenuBtn::MenuBtn()
{
	m_pTooltipLeft			= NULL;
	m_pTooltipLeftIn		= NULL;
	m_pTooltipCenter		= NULL;
	m_pTooltipRightIn		= NULL;
	m_pTooltipRight			= NULL;

	m_pRedTooltip			= NULL;
	m_pRedTooltipText		= NULL;
	m_pOakToolTip			= NULL;

	m_pBtnIconStart			= NULL;
	m_pBtnIconEnd			= NULL;
		
	for( int i = 0; i < TE_EFFECT_MAX; ++i  )
		m_pRedEffect[i] = NULL;

	for( int i = 0; i < TE_EXCLAMATION_MARK_MAX; ++i  )
		m_pExclamationEffect[i] = NULL;

	m_dwCurrentTime			= 0;
	m_iEventItemCnt			= 0;

	m_iGrowCurrentCount		= 0;
	m_iGrowMaxCount			= 0;

	m_bUseSparkle			= false;
	m_bOver					= false;

	m_dwOverTimer			= 0;
	m_dwOverTicTime			= 0;
	m_byOverFrmPrintCount	= 0;

	m_dwStartOutline		= 0;
	m_iRedEffectIndex		= 0;

	for( int i = 0; i < TE_EXCLAMATION_MARK_MAX; ++i  )
		m_vExclamationTimeList[i] = 0;
	m_dwExclamationEffectEndTime = 0;
}

MenuBtn::~MenuBtn()
{
	SAFEDELETE( m_pTooltipLeft );
	SAFEDELETE( m_pTooltipLeftIn );
	SAFEDELETE( m_pTooltipCenter );
	SAFEDELETE( m_pTooltipRightIn );
	SAFEDELETE( m_pTooltipRight );

	SAFEDELETE( m_pRedTooltip );
	SAFEDELETE( m_pRedTooltipText );
	SAFEDELETE( m_pOakToolTip );
	for( int i = 0; i < TE_EFFECT_MAX; ++i  )
		SAFEDELETE( m_pRedEffect[i] );

	for( int i = 0; i < TE_EXCLAMATION_MARK_MAX; ++i  )
		SAFEDELETE( m_pExclamationEffect[i] );

	SAFEDELETE( m_pBtnIconStart );
	SAFEDELETE( m_pBtnIconEnd );
}

void MenuBtn::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "TooltipLeft" )
	{
		SAFEDELETE( m_pTooltipLeft );
		m_pTooltipLeft = pImage;
	}
	else if( szType == "TooltipLeftIn" )
	{
		SAFEDELETE( m_pTooltipLeftIn );
		m_pTooltipLeftIn = pImage;
	}
	else if( szType == "TooltipCenter" )
	{
		SAFEDELETE( m_pTooltipCenter );
		m_pTooltipCenter = pImage;
	}
	else if( szType == "TooltipRightIn" )
	{
		SAFEDELETE( m_pTooltipRightIn );
		m_pTooltipRightIn = pImage;
	}
	else if( szType == "TooltipRight" )
	{
		SAFEDELETE( m_pTooltipRight );
		m_pTooltipRight = pImage;
	}
	else if( szType == "RedTooltip" )
	{
		SAFEDELETE( m_pRedTooltip );
		m_pRedTooltip = pImage;
	}
	else if( szType == "RedTooltipText" )
	{
		SAFEDELETE( m_pRedTooltipText );
		m_pRedTooltipText = pImage;
	}
	else if( szType == "BtnIconStart" )
	{
		SAFEDELETE( m_pBtnIconStart );
		m_pBtnIconStart = pImage;
	}
	else if( szType == "BtnIconEnd" )
	{
		SAFEDELETE( m_pBtnIconEnd );
		m_pBtnIconEnd = pImage;
	}	
	else if( szType == "RedEffect01" )
	{
		SAFEDELETE( m_pRedEffect[0] );
		m_pRedEffect[0] = pImage;
	}	
	else if( szType == "RedEffect02" )
	{
		SAFEDELETE( m_pRedEffect[1] );
		m_pRedEffect[1] = pImage;
	}
	else if( szType == "OakToolTip" )
	{
		SAFEDELETE( m_pOakToolTip );
		m_pOakToolTip = pImage;
	}
	else if( szType == "ExclamationMark01" )
	{
		SAFEDELETE( m_pExclamationEffect[0] );
		m_pExclamationEffect[0] = pImage;
	}	
	else if( szType == "ExclamationMark02" )
	{
		SAFEDELETE( m_pExclamationEffect[1] );
		m_pExclamationEffect[1] = pImage;
	}
	else if( szType == "ExclamationMark03" )
	{
		SAFEDELETE( m_pExclamationEffect[2] );
		m_pExclamationEffect[2] = pImage;
	}	
	else if( szType == "ExclamationMark04" )
	{
		SAFEDELETE( m_pExclamationEffect[3] );
		m_pExclamationEffect[3] = pImage;
	}
	else
	{
		MenuBase::AddRenderImage( szType, pImage );
	}
}

void MenuBtn::OnProcess( float fTimePerSec )
{
	ErrorReport::SetPosition( 3002, 1 );

	if( !IsShow() )
		return;

	ErrorReport::SetPosition( 3002, 2 );
	DWORD dwCurTime = FRAMEGETTIME();
	if(  dwCurTime - m_dwCurrentTime > 1000 )
	{
		m_dwCurrentTime = dwCurTime;

		switch( GetID() )
		{
		case HelpWnd::ID_DISPLAY:
		case HelpWnd::ID_PRACTICE:
		case HelpWnd::ID_FISHING:
		case HelpWnd::ID_EXCAVATION:
		case HelpWnd::ID_HOUSING_SETUP:
		case HelpWnd::ID_PRESENT:
		case HelpWnd::ID_PET:
			break;
		case HelpWnd::ID_SHOP:
			ErrorReport::SetPosition( 3002, 3 );
			{
				// 이벤트 상품
				NewShopWnd *pShopWnd = dynamic_cast<NewShopWnd*>(g_GUIMgr.FindWnd(NEW_SHOP_WND));
				if( pShopWnd )
				{
					m_iEventItemCnt = pShopWnd->GetTotalEventItemCnt();
				}
			}
			break;	
		case HelpWnd::ID_MYITEM:
			{
				m_szGashaponInfo.Clear();
				m_szGashaponName.Clear();

				m_iGrowCurrentCount = 0;
				m_iGrowMaxCount = 0;

				// 시간 가챠폰
				ioEtcItemTimeGashapon *pTimeGashapon = dynamic_cast< ioEtcItemTimeGashapon * >( g_EtcItemMgr.FindEtcItem( g_MyInfo.GetTimeGashaponEtcItem() ) );
				if( pTimeGashapon )
				{
					ioUserEtcItem::ETCITEMSLOT kSlot;
					ioUserEtcItem *pUserEtcItem = g_MyInfo.GetUserEtcItem();
					if( pUserEtcItem && pUserEtcItem->GetEtcItem( pTimeGashapon->GetType(), kSlot ) )
					{
						ioHashString szInfo1;
						DWORD dwInfo1Color, dwInfo2Color;
						pUserEtcItem->GetUseInfo( pTimeGashapon->GetType(), szInfo1, m_szGashaponInfo, dwInfo1Color, dwInfo2Color );
						m_szGashaponName = pTimeGashapon->GetName();
					}
				}
				else if( g_MyInfo.GetTimeGrowingCount() + g_MyInfo.GetTimeGrowthPossibleCount() > 0 && HelpWnd::IsMoveStateShow() )					
				{
					// 용병 성장	
					m_iGrowCurrentCount = g_MyInfo.GetTimeGrowingCount();
					m_iGrowMaxCount     = g_MyInfo.GetTimeGrowthPossibleCount();
				}
			}
			break;	
		case HelpWnd::ID_MISSION_QUEST:
			{
				// 미션창이 켜져 있을 경우
				if( g_GUIMgr.IsShow( MISSION_QUEST_MAIN_WND ) )
				{
					//이펙트 상태이면 End 시켜준다.
					if ( IsSparkling() )
						SetEndSparkle();
				}
				// 미션창이 닫혀 있을 경우
				else
				{
					//이펙트 체크
					bool bStartSparkle = false;
					for ( int i=MDT_DAILY ; i<MDT_MAX ; ++i )
					{
						//보상 받기가 가능한 미션이 한 개라도 있을 경우
						if ( g_MissionManager.GetAttinCountToType( i ) > 0 )
						{
							bStartSparkle = true;
							break;
						}
					}

					//보상 받기가 가능한 미션이 있고
					if ( bStartSparkle )
					{
						//현재 이펙트 상태가 아니라면
						if ( !IsSparkling() )
							SetStartSparkle();	//이펙트 스타트
					}
					//보상 받기가 가능한 미션이 없고
					else
					{
						//이펙트 상태이면 End 시켜준다.
						if ( IsSparkling() )
							SetEndSparkle();	//이펙트 종료
					}
				}				
			}
			break;		
		}
	}
	
	if ( m_bUseSparkle )
	{
		if( dwCurTime - m_dwOverTimer > m_dwOverTicTime )
		{
			m_dwOverTimer = dwCurTime;
			m_bOver = !m_bOver;
		}
	}

	if( GetID() == HelpWnd::ID_SHOP )
	{
		if( m_dwStartOutline + TE_EFFECT_TIME < FRAMEGETTIME() )
		{
			GoToTop();
			m_dwStartOutline = FRAMEGETTIME();
			m_iRedEffectIndex++;
			if( !COMPARE( m_iRedEffectIndex, 0, TE_EFFECT_MAX ) )
				m_iRedEffectIndex = 0;
		}
	}
	else if ( GetID() == HelpWnd::ID_OAK )
		OnOakToolTipProcess();

	ErrorReport::SetPosition( 3002, 4 );
}

void MenuBtn::OnOakToolTipProcess()
{
	if ( !HelpWnd::IsShowOakToolTip() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwExclamationEffectEndTime > 0 && 
		m_dwExclamationEffectEndTime < dwCurTime )
	{
		m_iExclamationEffectIndex==(TE_EXCLAMATION_MARK_MAX-1) ? m_iExclamationEffectIndex=0 : m_iExclamationEffectIndex++;
		m_dwExclamationEffectEndTime = dwCurTime + m_vExclamationTimeList[m_iExclamationEffectIndex];
	}
}

void MenuBtn::OnRender()
{
	float fXPos, fYPos;
	fXPos = GetDerivedPosX();
	fYPos = GetDerivedPosY();

	MenuBase::OnRender();
	
	switch( GetID() )
	{
	case HelpWnd::ID_DISPLAY:
	case HelpWnd::ID_ROLLBOOK:
		break;
	case HelpWnd::ID_PRACTICE:
		{
			if( !IsDisabled() && !IsExDisabled() )
			{
				HelpWnd *pParent = dynamic_cast<HelpWnd*>( GetParent() );
				//UJ 130902, 최적화: IsPractice()는 매우 비싸다
				if( pParent && pParent->IsPractice() && m_pBtnIconEnd )
					m_pBtnIconEnd->Render( fXPos, fYPos );
				else if( m_pBtnIconStart )
					m_pBtnIconStart->Render( fXPos, fYPos );
			}
		}
		break;
	case HelpWnd::ID_FISHING:
		{
			if( !IsDisabled() )
			{
				HelpWnd *pParent = dynamic_cast<HelpWnd*>( GetParent() );
				if( pParent && pParent->IsFishing() && m_pBtnIconEnd )
					m_pBtnIconEnd->Render( fXPos, fYPos );
				else if( m_pBtnIconStart )
					m_pBtnIconStart->Render( fXPos, fYPos );
			}
		}
		break;
	case HelpWnd::ID_EXCAVATION:
		{
			if( !IsDisabled() )
			{
				HelpWnd *pParent = dynamic_cast<HelpWnd*>( GetParent() );
				if( pParent && pParent->IsExcavation() && m_pBtnIconEnd )
					m_pBtnIconEnd->Render( fXPos, fYPos );
				else if( m_pBtnIconStart )
					m_pBtnIconStart->Render( fXPos, fYPos );
			}
		}
		break;
	case HelpWnd::ID_HOUSING_SETUP:
		{
			if( !IsDisabled() )
			{
				HelpWnd *pParent = dynamic_cast<HelpWnd*>( GetParent() );
				if( pParent && pParent->IsHousing() && m_pBtnIconEnd )
					m_pBtnIconEnd->Render( fXPos, fYPos );
				else if( m_pBtnIconStart )
					m_pBtnIconStart->Render( fXPos, fYPos );
			}
		}
		break;
	case HelpWnd::ID_OAK:
		{
			if( !IsDisabled() )
			{
				HelpWnd *pParent = dynamic_cast<HelpWnd*>( GetParent() );
				if( pParent && pParent->IsOak() && m_pBtnIconEnd )
					m_pBtnIconEnd->Render( fXPos, fYPos );
				else if( m_pBtnIconStart )
					m_pBtnIconStart->Render( fXPos, fYPos );
			}
		}
		break;
	case HelpWnd::ID_SHOP:
		{
			g_FontMgr.SetAlignType( TAT_LEFT );
			g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_3X );
			g_FontMgr.SetBkColor( 18, 48, 78 );
			
			if( IsDisabled() || IsExDisabled() )
                g_FontMgr.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
			else
				g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );

			g_FontMgr.PrintText( fXPos + 23, fYPos + 6, FONT_SIZE_13, STR(1) );

			WORD wCustomKey = g_KeyManager.GetCurrentShopKey();
			if( wCustomKey != 0 )
				g_FontMgr.PrintText( fXPos + 6, fYPos + 13, FONT_SIZE_10, STR(2), g_KeyManager.GetKeyText( wCustomKey ) );		
		}
		break;	
	case HelpWnd::ID_MYITEM:
		{
			g_FontMgr.SetAlignType( TAT_LEFT );
			g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_3X );
			g_FontMgr.SetBkColor( 18, 48, 78 );

			if( IsDisabled() || IsExDisabled() )
				g_FontMgr.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
			else
				g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );

			g_FontMgr.PrintText( fXPos + 23, fYPos + 6, FONT_SIZE_13, STR(7) );

			WORD wCustomKey = g_KeyManager.GetCurrentGrowthKey();
			if( wCustomKey != 0 )
				g_FontMgr.PrintText( fXPos + 6, fYPos + 13, FONT_SIZE_10, STR(8), g_KeyManager.GetKeyText( wCustomKey ) );
		}
		break;	
	case HelpWnd::ID_PRESENT:
		{
			g_FontMgr.SetAlignType( TAT_LEFT );
			g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_3X );
			g_FontMgr.SetBkColor( 18, 48, 78 );

			if( IsDisabled() || IsExDisabled() )
				g_FontMgr.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
			else
				g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );

			g_FontMgr.PrintText( fXPos + 23, fYPos + 6, FONT_SIZE_13, STR(9) );

			WORD wCustomKey = g_KeyManager.GetCurrentPresentKey();
			if( wCustomKey != 0 )
				g_FontMgr.PrintText( fXPos + 6, fYPos + 13, FONT_SIZE_10, STR(10), g_KeyManager.GetKeyText( wCustomKey ) );
		}
		break;
	case HelpWnd::ID_MISSION_QUEST:
		{
			g_FontMgr.SetAlignType( TAT_LEFT );
			g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_3X );
			g_FontMgr.SetBkColor( 18, 48, 78 );

			if( IsDisabled() || IsExDisabled() )
				g_FontMgr.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
			else
				g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );

			g_FontMgr.PrintText( fXPos + 23, fYPos + 6, FONT_SIZE_13, "미션" );

			WORD wCustomKey = g_KeyManager.GetCurrentQuestKey();
			if( wCustomKey != 0 )
				g_FontMgr.PrintText( fXPos + 6, fYPos + 13, FONT_SIZE_10, STR(14), g_KeyManager.GetKeyText( wCustomKey ) );

			if( m_bOver )
			{
				for ( int i=0 ; i<(int)m_byOverFrmPrintCount ; ++i )
				{
					OnDrawOveredAdd( fXPos, fYPos );
				}
			}
		}
		break;
	case HelpWnd::ID_PET:
		{
			g_FontMgr.SetAlignType( TAT_LEFT );
			g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_3X );
			g_FontMgr.SetBkColor( 18, 48, 78 );

			if( IsDisabled() || IsExDisabled() )
				g_FontMgr.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
			else
				g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );

			g_FontMgr.PrintText( fXPos + 23, fYPos + 6, FONT_SIZE_13, STR(19) );	

			WORD wCustomKey = g_KeyManager.GetCurrentPetKey();
			if( wCustomKey != 0 )
				g_FontMgr.PrintText( fXPos + 6, fYPos + 13, FONT_SIZE_10, STR(14), g_KeyManager.GetKeyText( wCustomKey ) );
		}
		break;
	}
}

void MenuBtn::OnRenderTooltip()
{
	float fXPos, fYPos;
	fXPos = GetDerivedPosX();
	fYPos = GetDerivedPosY();

	switch( GetID() )
	{
	case HelpWnd::ID_DISPLAY:
	case HelpWnd::ID_PRACTICE:
	case HelpWnd::ID_FISHING:
	case HelpWnd::ID_EXCAVATION:
	case HelpWnd::ID_HOUSING_SETUP:
	case HelpWnd::ID_MISSION_QUEST:
	case HelpWnd::ID_PET:
		break;
	case HelpWnd::ID_SHOP:
		{
			const CTime* pCloseTime = g_SpecialShopMgr.GetCloseTime();
			if( g_SpecialShopMgr.IsSpecialShopOpen() && HelpWnd::IsMoveStateShow() && pCloseTime )
			{
				//비밀 상점 오픈 광고					
				CTimeSpan CloseGap = (*pCloseTime) - DateHelp::GetCurrTime();
				OnRenderTooltipSecretShopOpen( fXPos + GetWidth()/2, fYPos + GetHeight()/2, CloseGap );
			}
			else
			{
				// 이벤트 상품
				if( m_iEventItemCnt > 0 && HelpWnd::IsMoveStateShow() )
				{
					OnRenderTooltipEvent( fXPos, fYPos );
				}
			}
		}
		break;	
	case HelpWnd::ID_MYITEM:
		{
			// 시간 가챠폰
			if( !m_szGashaponName.IsEmpty() && HelpWnd::IsMoveStateShow() )
			{
				OnRenderTooltipTimeGashapon( fXPos, fYPos );
			}
			else if( m_iGrowCurrentCount + m_iGrowMaxCount > 0 && HelpWnd::IsMoveStateShow() )		// 용병 성장				
			{
				OnRenderTooltipSoliderGrowth( fXPos, fYPos );
			}
		}
		break;	
	case HelpWnd::ID_PRESENT:
		{
			if( g_PresentMgr.GetMaxPresent() > 0 && HelpWnd::IsMoveStateShow() )
			{
				OnRenderTooltipTimePresent( fXPos, fYPos );
			}
		}
		break;
	case HelpWnd::ID_ROLLBOOK:
		{
			if ( HelpWnd::IsMoveStateShow() )
			{
				OnRenderTooltipTimeRollBook( fXPos, fYPos );
			}
		}
		break;
	case HelpWnd::ID_OAK:
		if ( HelpWnd::IsMoveStateShow() && HelpWnd::IsShowOakToolTip() )
		{
			OnRenderTooltipOak( fXPos, fYPos );
		}
		break;
	}
}

void MenuBtn::OnRenderTooltipSecretShopOpen( float fXPos, float fYPos, const CTimeSpan& CloseGap )
{
	if( m_pRedTooltip )
		m_pRedTooltip->Render( fXPos - 71, fYPos - 45 );

	if( m_pRedEffect[m_iRedEffectIndex] )
		m_pRedEffect[m_iRedEffectIndex]->Render( fXPos - 69, fYPos - 42, UI_RENDER_ADD );

	if( CloseGap.GetDays() == 0 && CloseGap.GetHours() == 0 && CloseGap.GetMinutes() < 1 )
	{
		ioComplexStringPrinter kMentPrinter;
		kMentPrinter.SetTextStyle( TS_NORMAL );
		kMentPrinter.SetTextColor( 0xFFFFF150 );
		kMentPrinter.AddTextPiece( FONT_SIZE_16, "마감임박" );
		kMentPrinter.PrintFullText( fXPos - 30, fYPos - 34, TAT_CENTER );
	}
	else
	{
		if( m_pRedTooltipText )
			m_pRedTooltipText->Render( fXPos - 64, fYPos - 39 );

		ioComplexStringPrinter kTimePrinter;
		kTimePrinter.SetTextStyle( TS_NORMAL );
		kTimePrinter.SetTextColor( TCT_DEFAULT_WHITE );

		if( 0 < CloseGap.GetDays() )
		{
			TIME_NUMBER( szHour, CloseGap.GetHours() + CloseGap.GetDays() * 24 );
			TIME_NUMBER( szMin,	 CloseGap.GetMinutes() );
			TIME_NUMBER( szSec,	 CloseGap.GetSeconds() );
			kTimePrinter.AddTextPiece( FONT_SIZE_12, "%s:%s:%s", szHour, szMin, szSec );
			kTimePrinter.PrintFullText( fXPos - 30, fYPos - 26, TAT_CENTER );
		}
		else
		{
			TIME_NUMBER( szHour, CloseGap.GetHours() );
			TIME_NUMBER( szMin,	 CloseGap.GetMinutes() );
			TIME_NUMBER( szSec,	 CloseGap.GetSeconds() );
			kTimePrinter.AddTextPiece( FONT_SIZE_12, "%s:%s:%s", szHour, szMin, szSec );
			kTimePrinter.PrintFullText( fXPos - 30, fYPos - 26, TAT_CENTER );
		}
	}
}

void MenuBtn::OnRenderTooltipEvent( float fXPos, float fYPos )
{
	ioComplexStringPrinter kMentPrinter;
	ioComplexStringPrinter kCountPrinter;

	kMentPrinter.SetTextStyle( TS_NORMAL );
	kMentPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kMentPrinter.AddTextPiece( FONT_SIZE_12, "이벤트 상품" );

	kCountPrinter.SetTextStyle( TS_NORMAL );
	kCountPrinter.SetTextColor( TCT_DEFAULT_RED );
	kCountPrinter.AddTextPiece( FONT_SIZE_12, "%d개", m_iEventItemCnt );

	float fTextWidth = kMentPrinter.GetFullWidth() + 20;
	if( fTextWidth < kCountPrinter.GetFullWidth() )
		fTextWidth = kCountPrinter.GetFullWidth() + 20;

	fYPos -= 28;
	OnRenderTooltipBar( ( fXPos + 7 ) - ( fTextWidth / 2 ), fYPos - 6, fTextWidth );
	kMentPrinter.PrintFullText( fXPos + 14, fYPos + 4, TAT_CENTER );
	kCountPrinter.PrintFullText( fXPos + 14, fYPos + 17, TAT_CENTER );
}

void MenuBtn::OnRenderTooltipTimeGashapon( float fXPos, float fYPos )
{
	ioComplexStringPrinter kMentPrinter;
	ioComplexStringPrinter kCountPrinter;
	kMentPrinter.SetTextStyle( TS_NORMAL );
	kMentPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kMentPrinter.AddTextPiece( FONT_SIZE_12, " %s ",  m_szGashaponInfo.c_str() );

	kCountPrinter.SetTextStyle( TS_NORMAL );
	kCountPrinter.SetTextColor( TCT_DEFAULT_RED );
	kCountPrinter.AddTextPiece( FONT_SIZE_12, " %s ", m_szGashaponName.c_str() );

	float fTextWidth = kMentPrinter.GetFullWidth() + 20;
	if( fTextWidth < kCountPrinter.GetFullWidth() )
		fTextWidth = kCountPrinter.GetFullWidth() + 20;

	fYPos -= 28;
	OnRenderTooltipBar( ( fXPos + 7 ) - ( fTextWidth / 2 ), fYPos - 6, fTextWidth );
	kMentPrinter.PrintFullText( fXPos + 14, fYPos + 4, TAT_CENTER );
	kCountPrinter.PrintFullText( fXPos + 14, fYPos + 17, TAT_CENTER );
}

void MenuBtn::OnRenderTooltipSoliderGrowth( float fXPos, float fYPos )
{
	ioComplexStringPrinter kMentPrinter;
	ioComplexStringPrinter kCountPrinter;

	kMentPrinter.SetTextStyle( TS_NORMAL );
	kMentPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kMentPrinter.AddTextPiece( FONT_SIZE_12, "육성중 용병" );

	kCountPrinter.SetTextStyle( TS_NORMAL );
	kCountPrinter.SetTextColor( TCT_DEFAULT_RED );
	kCountPrinter.AddTextPiece( FONT_SIZE_12, "%d / %d명", m_iGrowCurrentCount, m_iGrowMaxCount );

	float fTextWidth = kMentPrinter.GetFullWidth() + 20;
	if( fTextWidth < kCountPrinter.GetFullWidth() )
		fTextWidth = kCountPrinter.GetFullWidth() + 20;

	fYPos -= 28;
	OnRenderTooltipBar( ( fXPos + 7 ) - ( fTextWidth / 2 ), fYPos - 6, fTextWidth );
	kMentPrinter.PrintFullText( fXPos + 14, fYPos + 4, TAT_CENTER );
	kCountPrinter.PrintFullText( fXPos + 14, fYPos + 17, TAT_CENTER );
}

void MenuBtn::OnRenderTooltipTimePresent( float fXPos, float fYPos )
{
	int iMaxPresent = g_PresentMgr.GetMaxPresent();
	int iMaxSubcription = g_SubscriptionMgr.GetMaxSubscription();
	ioComplexStringPrinter kMentPrinter;
	ioComplexStringPrinter kCountPrinter;

	kMentPrinter.SetTextStyle( TS_NORMAL );
	kMentPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kMentPrinter.AddTextPiece( FONT_SIZE_12, "안받은 상품" );

	kCountPrinter.SetTextStyle( TS_NORMAL );
	kCountPrinter.SetTextColor( TCT_DEFAULT_RED );
	kCountPrinter.AddTextPiece( FONT_SIZE_12, "%d개", iMaxPresent + iMaxSubcription );

	float fTextWidth = kMentPrinter.GetFullWidth() + 20;
	if( fTextWidth < kCountPrinter.GetFullWidth() )
		fTextWidth = kCountPrinter.GetFullWidth() + 20;

	fYPos -= 28;
	OnRenderTooltipBar( ( fXPos + 7 ) - ( fTextWidth / 2 ), fYPos - 6, fTextWidth );
	kMentPrinter.PrintFullText( fXPos + 14, fYPos + 4, TAT_CENTER );
	kCountPrinter.PrintFullText( fXPos + 14, fYPos + 17, TAT_CENTER );
}

void MenuBtn::OnRenderTooltipTimeRollBook( float fXPos, float fYPos )
{
	RollBookWnd* pRollBook = dynamic_cast<RollBookWnd*>( g_GUIMgr.FindWnd( ROLLBOOK_WND ) );
	if ( pRollBook && pRollBook->IsRequestTime() )
	{
		ioComplexStringPrinter kMentPrinter;
		ioComplexStringPrinter kCountPrinter;

		kMentPrinter.SetTextStyle( TS_NORMAL );
		kMentPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
		kMentPrinter.AddTextPiece( FONT_SIZE_12, "안받은 상품" );

		kCountPrinter.SetTextStyle( TS_NORMAL );
		kCountPrinter.SetTextColor( TCT_DEFAULT_RED );
		kCountPrinter.AddTextPiece( FONT_SIZE_12, "1개" );

		float fTextWidth = kMentPrinter.GetFullWidth() + 20;
		if( fTextWidth < kCountPrinter.GetFullWidth() )
			fTextWidth = kCountPrinter.GetFullWidth() + 20;

		fYPos -= 28;
		OnRenderTooltipBar( ( fXPos ) - ( fTextWidth / 2 ), fYPos - 6, fTextWidth );
		kMentPrinter.PrintFullText( fXPos + 6, fYPos + 4, TAT_CENTER );
		kCountPrinter.PrintFullText( fXPos + 6, fYPos + 17, TAT_CENTER );
	}
}

void MenuBtn::OnRenderTooltipOak( float fXPos, float fYPos )
{
	enum { TOOLTIP_X = 38, TOOLTIP_Y = 34,
			EXCLAMATION_X = 17,EXCLAMATION_Y = 20 };
	if( m_pOakToolTip )	
		m_pOakToolTip->Render( fXPos - TOOLTIP_X, fYPos - TOOLTIP_Y );

	if ( m_pExclamationEffect[m_iExclamationEffectIndex] )
		m_pExclamationEffect[m_iExclamationEffectIndex]->Render( fXPos - EXCLAMATION_X, fYPos - EXCLAMATION_Y );
}

void MenuBtn::OnRenderTooltipBar( int iXPos, int iYPos, float fTextWidth )
{
	if( !m_pTooltipLeft || !m_pTooltipLeftIn || !m_pTooltipCenter || !m_pTooltipRightIn || !m_pTooltipRight )
		return;

	// 15(L) / 19(C) / 15(R)
	int iSplitWidth = (fTextWidth - 35) / 2;

	m_pTooltipLeft->Render( iXPos, iYPos );
	iXPos += m_pTooltipLeft->GetWidth();	

	m_pTooltipLeftIn->SetSize( iSplitWidth, m_pTooltipLeftIn->GetHeight() );
	m_pTooltipLeftIn->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
	m_pTooltipLeftIn->Render( iXPos, iYPos );
	iXPos += iSplitWidth;

	m_pTooltipCenter->Render( iXPos, iYPos );
	iXPos += m_pTooltipCenter->GetWidth();

	m_pTooltipRightIn->SetSize( iSplitWidth, m_pTooltipRightIn->GetHeight() );
	m_pTooltipRightIn->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
	m_pTooltipRightIn->Render( iXPos, iYPos );
	iXPos += iSplitWidth;

	m_pTooltipRight->Render( iXPos, iYPos );
	iXPos += m_pTooltipRight->GetWidth();
}

void MenuBtn::SetStartSparkle()
{
	m_bUseSparkle = true;
	m_bOver = false;
	m_dwOverTimer = FRAMEGETTIME();
}

void MenuBtn::SetEndSparkle()
{
	m_bUseSparkle = false;
	m_bOver = false;
}

void MenuBtn::ParseExtraInfo( ioXMLElement &xElement )
{
	MenuBase::ParseExtraInfo( xElement );
	m_dwOverTicTime = xElement.GetIntAttribute_e( "OverTicTime" );
	m_byOverFrmPrintCount = (BYTE)xElement.GetIntAttribute_e( "OverFrmPrintCount" );
	m_vExclamationTimeList[0] = xElement.GetIntAttribute_e( "ExclamationTime01");
	m_vExclamationTimeList[1] = xElement.GetIntAttribute_e( "ExclamationTime02");
	m_vExclamationTimeList[2] = xElement.GetIntAttribute_e( "ExclamationTime03");
	m_vExclamationTimeList[3] = xElement.GetIntAttribute_e( "ExclamationTime04");
}

void MenuBtn::iwm_show()
{
	if( GetID() == HelpWnd::ID_OAK )
	{
		m_iExclamationEffectIndex = 0;
		m_dwExclamationEffectEndTime = FRAMEGETTIME() + m_vExclamationTimeList[m_iExclamationEffectIndex];
	}
}
//////////////////////////////////////////////////////////////////////////
bool MessengerBtn::sg_bMessenger = false;
MessengerBtn::MessengerBtn()
{
	m_bScreen		= false;
	m_bBackMessenger= sg_bMessenger;
	m_dwScreenTime  = 0;

	m_pTooltipLeft = NULL;
	m_pTooltipLeftIn = NULL;
	m_pTooltipCenter = NULL;
	m_pTooltipRightIn = NULL;
	m_pTooltipRight = NULL;
}

MessengerBtn::~MessengerBtn()
{
	SAFEDELETE( m_pTooltipLeft );
	SAFEDELETE( m_pTooltipLeftIn );
	SAFEDELETE( m_pTooltipCenter );
	SAFEDELETE( m_pTooltipRightIn );
	SAFEDELETE( m_pTooltipRight );
}

void MessengerBtn::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	// 마우스 보이지 않을때
	if( !g_App.IsVisibleMouse() )
	{
		m_bOver = false;
	}

	if( m_bBackMessenger != sg_bMessenger )
		m_dwScreenTime = FRAMEGETTIME();

	m_bBackMessenger = sg_bMessenger;
	if( m_bBackMessenger )
	{
		if( m_bScreen )
		{
			if( FRAMEGETTIME() - m_dwScreenTime > 500 )
			{
				m_dwScreenTime = FRAMEGETTIME();
				m_bScreen      = false;
			}
		}	
		else
		{
			if( FRAMEGETTIME() - m_dwScreenTime > 8000 )
			{
				m_dwScreenTime = FRAMEGETTIME();
				m_bScreen      = true;
			}
		}
	}
	else
		m_bScreen = false;
}

void MessengerBtn::OnDrawNormal( int iXPos, int iYPos )
{
	if( m_bScreen )
		ioButton::OnDrawOvered( iXPos, iYPos );
	else
		ioButton::OnDrawNormal( iXPos, iYPos );
}

void MessengerBtn::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "TooltipLeft" )
	{
		SAFEDELETE( m_pTooltipLeft );
		m_pTooltipLeft = pImage;
	}
	else if( szType == "TooltipLeftIn" )
	{
		SAFEDELETE( m_pTooltipLeftIn );
		m_pTooltipLeftIn = pImage;
	}
	else if( szType == "TooltipCenter" )
	{
		SAFEDELETE( m_pTooltipCenter );
		m_pTooltipCenter = pImage;
	}
	else if( szType == "TooltipRightIn" )
	{
		SAFEDELETE( m_pTooltipRightIn );
		m_pTooltipRightIn = pImage;
	}
	else if( szType == "TooltipRight" )
	{
		SAFEDELETE( m_pTooltipRight );
		m_pTooltipRight = pImage;
	}
	else
	{
		ioButton::AddRenderImage( szType, pImage );
	}
}

void MessengerBtn::OnRender()
{
	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	ioButton::OnRender();

	int iOnlineGuild  = 0;
	ioGuildData *pGuildData = g_GuildInfo.GetGuildData( g_GuildInfo.GetGuildIndex() );
	if( pGuildData )
	{
		iOnlineGuild = max( 0, (int)pGuildData->GetOnlineUserSize() - 1 );
	}
	int iOnlineFriend = g_FriendManager.GetOnlineNodeSize();
	int iNewMemoCount = g_MemoManager.GetNotViewMemoCount();
	int iRecvCloverCount = 0;
	ioClover *pClover = g_MyInfo.GetClover();
	if( pClover )
		iRecvCloverCount = pClover->GetRecvCloverCount();

	if( iOnlineGuild + iOnlineFriend + iNewMemoCount + iRecvCloverCount > 0 && HelpWnd::IsMoveStateShow() )
	{
		ioComplexStringPrinter kFriendPrinter;
		ioComplexStringPrinter kMemoPrinter;

		kFriendPrinter.SetTextStyle( TS_NORMAL );
		kFriendPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
		kFriendPrinter.AddTextPiece( FONT_SIZE_12, STR(2) );
		kFriendPrinter.SetTextColor( TCT_DEFAULT_RED );
		kFriendPrinter.AddTextPiece( FONT_SIZE_12, STR(3), iOnlineFriend, iOnlineGuild );

		kMemoPrinter.SetTextStyle( TS_NORMAL );
		kMemoPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
		kMemoPrinter.AddTextPiece( FONT_SIZE_12, "클로버/쪽지 " );
		kMemoPrinter.SetTextColor( TCT_DEFAULT_RED );
		kMemoPrinter.AddTextPiece( FONT_SIZE_12, "%d/%d개", iRecvCloverCount, iNewMemoCount );

		float fTextWidth = kFriendPrinter.GetFullWidth();
		if( fTextWidth < kMemoPrinter.GetFullWidth() )
			fTextWidth = kMemoPrinter.GetFullWidth();
		
		iXPos -= ( fTextWidth - 15 );
		iYPos -= 28;
		OnRenderTooltip( iXPos, iYPos - 6, fTextWidth );
		kFriendPrinter.PrintFullText( iXPos + fTextWidth + 9, iYPos + 4, TAT_RIGHT );
		kMemoPrinter.PrintFullText( iXPos + fTextWidth + 9, iYPos + 17, TAT_RIGHT );
	}
}

void MessengerBtn::OnRenderTooltip( int iXPos, int iYPos, float fTextWidth )
{
	if( !m_pTooltipLeft || !m_pTooltipLeftIn || !m_pTooltipCenter || !m_pTooltipRightIn || !m_pTooltipRight ) return;

	m_pTooltipLeft->Render( iXPos - 7, iYPos );
	m_pTooltipLeftIn->SetSize( fTextWidth + 4, m_pTooltipLeftIn->GetHeight() );
	m_pTooltipLeftIn->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
	m_pTooltipLeftIn->Render( iXPos + 8, iYPos );
	m_pTooltipCenter->Render( iXPos + fTextWidth - 7, iYPos );
	m_pTooltipRight->Render( iXPos + fTextWidth + 12, iYPos );
}
//////////////////////////////////////////////////////////////////////////
PlazaStateBtn::PlazaStateBtn()
{
	m_pNormalCommunity = NULL;
	m_pDisableCommunity= NULL;
	m_pNormalPractice  = NULL;
	m_pDisablePractice = NULL;

	m_pPlayStage = NULL;
}

PlazaStateBtn::~PlazaStateBtn()
{
	SAFEDELETE( m_pNormalCommunity );
	SAFEDELETE( m_pDisableCommunity );
	SAFEDELETE( m_pNormalPractice );
	SAFEDELETE( m_pDisablePractice );
}

void PlazaStateBtn::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "NormalCommunity" )
	{
		SAFEDELETE( m_pNormalCommunity );
		m_pNormalCommunity = pImage;
	}
	else if( szType == "DisableCommunity" )
	{
		SAFEDELETE( m_pDisableCommunity );
		m_pDisableCommunity = pImage;
	}
	else if( szType == "NormalPractice" )
	{
		SAFEDELETE( m_pNormalPractice );
		m_pNormalPractice = pImage;
	}
	else if( szType == "DisablePractice" )
	{
		SAFEDELETE( m_pDisablePractice );
		m_pDisablePractice = pImage;
	}
	else
	{
		MenuBase::AddRenderImage( szType, pImage );
	}
}

void PlazaStateBtn::SetPlayStage( ioPlayStage *pPlayStage )
{
	m_pPlayStage = pPlayStage;
}

void PlazaStateBtn::OnRender()
{
	MenuBase::OnRender();

	if( !m_pPlayStage ) return;

	ioBaseChar *pOwnerChar = m_pPlayStage->GetOwnerChar();
	if( !pOwnerChar ) return;

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( pOwnerChar->GetChatModeState() == CMS_NONE )
	{
		if( IsDisabled() || IsExDisabled() )
		{
			if( m_pDisableCommunity )
				m_pDisableCommunity->Render( iXPos, iYPos );
		}
		else
		{
			if( m_pNormalCommunity )
				m_pNormalCommunity->Render( iXPos, iYPos );
		}
	}
	else if( pOwnerChar->GetChatModeState() == CMS_CHANGE_CHAT )
	{
		if( m_pDisablePractice )
			m_pDisablePractice->Render( iXPos, iYPos );
	}
	else if( pOwnerChar->GetChatModeState() == CMS_CHAT )
	{
		if( IsDisabled() || IsExDisabled() )
		{
			if( m_pDisablePractice )
				m_pDisablePractice->Render( iXPos, iYPos );
		}
		else
		{
			if( m_pNormalPractice )
				m_pNormalPractice->Render( iXPos, iYPos );
		}
	}
	else if( pOwnerChar->GetChatModeState() == CMS_CHANGE_NORMAL )
	{
		if( m_pDisableCommunity )
			m_pDisableCommunity->Render( iXPos, iYPos );
	}
}
//////////////////////////////////////////////////////////////////////////
DWORD HelpWnd::sg_dwMoveState = HelpWnd::MOVE_STATE_SHOW;
float HelpWnd::sg_fMessengerXPos = 0.0f;
float HelpWnd::sg_fMessengerYPos = 0.0f;
float HelpWnd::sg_fQuestXPos = 0.0f;
float HelpWnd::sg_fQuestYPos = 0.0f;
bool HelpWnd::sg_bShowOakToolTip = true;
HelpWnd::HelpWnd() : m_pPlayStage(NULL)
{
	m_pBarLeftMultiply = NULL;
	m_pBarCenterMultiply = NULL;
	m_pBarRightMultiply = NULL;
	m_pBarLeft = NULL;
	m_pBarCenter = NULL;
	m_pBarRight = NULL;
	m_pHideBar = NULL;
	m_pHideIcon = NULL;
	m_ePrevModeType = MT_NONE;
	m_ePrevPlazaType= PT_NONE;

	m_dwMoveStateDelayTIme = 0;
	m_dwMoveStateTime = 0;
	m_fMoveWndYPos = 0.0f;

	m_bTutorial = false;

	m_dwShowKeepTime = 12000;
	m_dwMoveStateStartDelayTIme = FRAMEGETTIME();
}

HelpWnd::~HelpWnd()
{
	SAFEDELETE( m_pBarLeftMultiply );
	SAFEDELETE( m_pBarCenterMultiply );
	SAFEDELETE( m_pBarRightMultiply );
	SAFEDELETE( m_pBarLeft );
	SAFEDELETE( m_pBarCenter );
	SAFEDELETE( m_pBarRight );
	SAFEDELETE( m_pHideBar );
	SAFEDELETE( m_pHideIcon );

	sg_dwMoveState = MOVE_STATE_SHOW;
}

void HelpWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "BarLeftMultiply" )
	{
		SAFEDELETE( m_pBarLeftMultiply );
		m_pBarLeftMultiply = pImage;
	}
	else if( szType == "BarCenterMultiply" )
	{
		SAFEDELETE( m_pBarCenterMultiply );
		m_pBarCenterMultiply = pImage;
	}
	else if( szType == "BarRightMultiply" )
	{
		SAFEDELETE( m_pBarRightMultiply );
		m_pBarRightMultiply = pImage;
	}
	else if( szType == "BarLeft" )
	{
		SAFEDELETE( m_pBarLeft );
		m_pBarLeft = pImage;
	}
	else if( szType == "BarCenter" )
	{
		SAFEDELETE( m_pBarCenter );
		m_pBarCenter = pImage;
	}
	else if( szType == "BarRight" )
	{
		SAFEDELETE( m_pBarRight );
		m_pBarRight = pImage;
	}
	else if( szType == "HideBar" )
	{
		SAFEDELETE( m_pHideBar );
		m_pHideBar = pImage;
	}
	else if( szType == "HideIcon" )
	{
		SAFEDELETE( m_pHideIcon );
		m_pHideIcon = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void HelpWnd::SetPlayStage( ioPlayStage *pPlayStage )
{
	m_pPlayStage = pPlayStage;

	PlazaStateBtn *pPlazaStateBtn = dynamic_cast<PlazaStateBtn*>( FindChildWnd( ID_PLAZA_STATE ) );
	if( pPlazaStateBtn )
		pPlazaStateBtn->SetPlayStage( pPlayStage );

	ShowWnd();
}

bool HelpWnd::IsPractice()
{
	//UJ 130902, 최적화: IsPractice()는 매우 비싸다, 좀더 싸게 체크할수있도록 변경
	if( !m_pPlayStage ) 
		return false;
	else if( m_pPlayStage->GetModeType() != MT_HEADQUARTERS ) 
		return false;
	else if( g_TutorialMgr.IsTutorial() )
		return false;

	return g_GUIMgr.IsShow( PRACTICE_HELP_WND );
}

bool HelpWnd::IsFishing()
{
	if( !m_pPlayStage ) return false;

	ioBaseChar *pOwnerChar = m_pPlayStage->GetOwnerChar();
	if( !pOwnerChar ) return false;

	return ( pOwnerChar->GetState() == CS_FISHING );
}

bool HelpWnd::IsExcavation()
{
	return g_GUIMgr.IsShow( EXCAVATION_WND );
}

bool HelpWnd::IsHousing()
{
	return g_GUIMgr.IsShow( HOUSING_BLOCK_SETUP_WND );
}

bool HelpWnd::IsOak()
{
	return g_GUIMgr.IsShow( PRIATE_RULLET_RENEWAL_WND );
}

void HelpWnd::iwm_show()
{
	if( m_pPlayStage )
	{
		ReCreateWindow();
	}
	else
	{
		HideWnd();
	}
}

void HelpWnd::iwm_gototop()
{
	// 항상 메신저 툴팁이 상위에 위치한다.
	MessengerPopup *pTooltipWnd = dynamic_cast<MessengerPopup *>(g_GUIMgr.FindWnd( MESSENGER_POPUP ));
	if( pTooltipWnd && pTooltipWnd->IsShow() )
		pTooltipWnd->GoToTop();

	// 항상 퀘스트 툴팁이 상위에 위치한다.
	QuestToolTip *pQuestTooltipWnd = dynamic_cast<QuestToolTip*>(g_GUIMgr.FindWnd( QUEST_TOOLTIP ));
	if( pQuestTooltipWnd && pQuestTooltipWnd->IsShow() )
		pQuestTooltipWnd->GoToTop();
}

void HelpWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_GAME_CLOSE:
	case ID_GAME_CLOSE2:
		if( cmd == IOBN_BTNUP )
		{
			g_App.AltF4ExitProcess();
		}
		break;
	case ID_BACK_STAGE:
		if( cmd == IOBN_BTNUP || cmd == IOEX_BTNUP )
		{
			switch( ioPlayMode::GetModeType() )
			{
			case MT_TRAINING:
			case MT_HOUSE:
				{
					// 로비로 이동
					if( P2PNetwork::IsNetworkPlaying() )
					{
						g_App.AltF4ExitProcess();
					}
				}
				break;
			case MT_HEADQUARTERS:
				{
					if( g_MyInfo.IsTutorialUser() )
					{
						if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
						{
							g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
						}
						else
						{
							g_GUIMgr.SetMsgBox( MB_OK, NULL, "튜토리얼을 완료하면#광장으로 이동이 가능합니다." );
						}
					}
					else
					{
						g_App.AltF4ExitProcess();
					}
				}
				break;
			case MT_MYROOM:
				{
					switch( ioMyRoomMode::GetMyRoomType() )
					{
					case MRT_PRACTICE:
						{
							if( g_MyInfo.IsTutorialUser() )
							{
								if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
								{
									g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
								}
								else
								{
									g_GUIMgr.SetMsgBox( MB_OK, NULL, "튜토리얼을 완료하면#광장으로 이동이 가능합니다." );
								}
							}
							else
							{
								g_App.AltF4ExitProcess();
							}
						}
						break;
					case MRT_BATTLE:
					case MRT_LADDER:
					case MRT_SHUFFLE:
					case MRT_RANKBATTLE:
						{
							g_App.AltF4ExitProcess();
						}
						break;
					}
				}
				break;
			default:
				{
					g_App.AltF4ExitProcess();
				}
				break;
			}
		}
		break;
	case ID_ROOM_INFO:
		if( cmd == IOBN_BTNUP || cmd == IOEX_BTNUP )
		{
			switch( ioPlayMode::GetModeType() )
			{
			case MT_TRAINING:
				{
					if( g_GUIMgr.IsShow( PLAZA_MAIN_WND ) )
						g_GUIMgr.HideWnd( PLAZA_MAIN_WND );
					else
						g_GUIMgr.ShowWnd( PLAZA_MAIN_WND );
				}
				break;
			case MT_HEADQUARTERS:
				{
					// 방정보
					if( g_MyInfo.IsTutorialUser() )
					{
						g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
					}
					else if( g_MyInfo.IsBankruptcy() || g_MyInfo.IsCharBankruptcy() )
					{
						g_GUIMgr.SetMsgBox( MB_OK, NULL, "사용 가능한 용병을 구입하시면 #방 정보 이용이 가능합니다." );
					}
					else
					{
						if( g_GUIMgr.IsShow( HEADQUARTERS_MAIN_WND ) )
							g_GUIMgr.HideWnd( HEADQUARTERS_MAIN_WND );
						else
							g_GUIMgr.ShowWnd( HEADQUARTERS_MAIN_WND );
					}
				}
				break;
			case MT_HOUSE:
				{
					// 방정보
					if( g_MyInfo.IsTutorialUser() )
					{
						g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
					}
					else if( g_MyInfo.IsBankruptcy() || g_MyInfo.IsCharBankruptcy() )
					{
						g_GUIMgr.SetMsgBox( MB_OK, NULL, "사용 가능한 용병을 구입하시면 #방 정보 이용이 가능합니다." );
					}
					else
					{
						if( g_GUIMgr.IsShow( HOUSE_MAIN_WND ) )
							g_GUIMgr.HideWnd( HOUSE_MAIN_WND );
						else
							g_GUIMgr.ShowWnd( HOUSE_MAIN_WND );
					}
				}
				break;
			default:
				{
					if( g_BattleRoomMgr.IsBattleRoom() )
					{
						if( g_GUIMgr.IsShow( PARTY_BATTLE_SIMPLE_WND ) )
							g_GUIMgr.HideWnd( PARTY_BATTLE_SIMPLE_WND );
						else
							g_GUIMgr.ShowWnd( PARTY_BATTLE_SIMPLE_WND );
					}
					else if( g_LadderTeamMgr.IsLadderTeam() )
					{
						if( g_GUIMgr.IsShow( MY_LADDER_TEAM_INFO_WND ) )
							g_GUIMgr.HideWnd( MY_LADDER_TEAM_INFO_WND );
						else
							g_GUIMgr.ShowWnd( MY_LADDER_TEAM_INFO_WND );
					}
				}
				break;
			}
		}
		break;
	case ID_DISPLAY:
		if( cmd == IOBN_BTNUP || cmd == IOEX_BTNUP )
		{
			DisplayProcess();
		}
		break;
	case ID_PRACTICE:
		if( cmd == IOBN_BTNUP || cmd == IOEX_BTNUP )
		{
			PracticeProcess();
		}
		break;
	case ID_FISHING:
		if( cmd == IOBN_BTNUP || cmd == IOEX_BTNUP )
		{
			FishingProcess();
		}
		break;
	case ID_EXCAVATION:
		if( cmd == IOBN_BTNUP || cmd == IOEX_BTNUP )
		{
			ExcavationeProcess();
		}
		break;
	case ID_HOUSING_SETUP:
		if( cmd == IOBN_BTNUP || cmd == IOEX_BTNUP )
		{
			HousingSetUpProcess();
		}
		break;
	case ID_PLAZA_STATE:
		if( cmd == IOBN_BTNUP || cmd == IOEX_BTNUP )
		{
			PlazaStateProcess();
		}
		break;
	case ID_SHOP:
		if( cmd == IOBN_BTNUP || cmd == IOEX_BTNUP )
		{
			ShopProcess();
		}
		break;
	case ID_MYITEM:
		if( cmd == IOBN_BTNUP || cmd == IOEX_BTNUP )
		{
			MyItemProcess();
		}
		break;
	case ID_PRESENT:
		if( cmd == IOBN_BTNUP || cmd == IOEX_BTNUP )
		{
			PresentProcess();
		}
		break;
	case ID_MISSION_QUEST:
		if( cmd == IOBN_BTNUP )
		{
			QuestProcess();
		}
		break;
	case ID_PET:
		if( cmd == IOBN_BTNUP || cmd == IOEX_BTNUP )
		{
			PetProcess();
		}
		break;
	case ID_MANUAL:
		if( cmd == IOBN_BTNUP )
		{
			ManualProcess();
		}
		break;
	case ID_ROLLBOOK:
		if( cmd == IOBN_BTNUP )
		{
			RollBookProcess();
		}
		else if ( cmd == IOEX_BTNUP )
		{
			if ( g_MyInfo.IsTutorialUser() )
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "튜토리얼을 완료하면#출석부 이용이 가능합니다." );
			else if( g_MyInfo.GetEntryType() == ET_TERMINATION || g_MyInfo.GetEntryType() == ET_TEMPORARY ||
				g_MyInfo.IsExpertEntryTemporary() )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "출석부는 정회원 전환 시 확인할 수 있습니다." );
				g_App.OpenFormalityEntryPage();
			}
			else if ( g_MyInfo.GetEntryType() == ET_TEMPORARY )
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "출석부는 정회원 전환 시 확인할 수 있습니다." );
			else
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "로비로 이동시 확인 가능합니다." );
		}
		break;
	case ID_MINI_HOME:
		if( cmd == IOBN_BTNUP )
		{
			g_App.OpenHeadQuaterPage( g_MyInfo.GetPublicID().c_str() );
		}
		break;
	case ID_OPTION:
		if( cmd == IOBN_BTNUP )
		{
			OptionProcess();
		}
		break;
	case ID_MESSENGER:
		if( cmd == IOBN_BTNUP || cmd == IOEX_BTNUP )
		{
			MessengerProcess();
		}
		break;
	case ID_OAK:
		if( cmd == IOBN_BTNUP )
		{
			OakProcess();
		}
		break;
	}
}

void HelpWnd::ReCreateWindow()
{
	SetWndPos( POS_X_GAP, (Setting::Height() - POS_Y_GAP) + m_fMoveWndYPos );
	SetSize( Setting::Width() - POS_W_GAP, POS_H_GAP );

	HideChildWnd( ID_GAME_CLOSE );
	HideChildWnd( ID_GAME_CLOSE2 );
	HideChildWnd( ID_BACK_STAGE );
	HideChildWnd( ID_ROOM_INFO );
	HideChildWnd( ID_DISPLAY );
	HideChildWnd( ID_PRACTICE );
	HideChildWnd( ID_FISHING );
	HideChildWnd( ID_EXCAVATION );
	HideChildWnd( ID_HOUSING_SETUP );
	HideChildWnd( ID_PLAZA_STATE );
	HideChildWnd( ID_OPTION );
	HideChildWnd( ID_OAK );
	
	switch( ioPlayMode::GetModeType() )
	{
	case MT_MYROOM:
		{
			switch( ioMyRoomMode::GetMyRoomType() )
			{
			case MRT_FIND_MODE:				
				ShowChildWnd( ID_GAME_CLOSE2 );
			case MRT_LOBBY:
				ShowChildWnd( ID_GAME_CLOSE );
				break;
			case MRT_PRACTICE:
				if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
				{
					ShowChildWnd( ID_BACK_STAGE );
				}
				else
				{
					ShowChildWnd( ID_GAME_CLOSE2 );
				}
				ShowChildWnd( ID_PRACTICE );
				break;
			default:
				if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
				{
					ShowChildWnd( ID_BACK_STAGE );
				}
				else
				{
					ShowChildWnd( ID_GAME_CLOSE2 );
				}
				break;
			}
		}
        break;
	case MT_HEADQUARTERS:
		{
			if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
			{
				ShowChildWnd( ID_BACK_STAGE );
			}
			else
			{
				ShowChildWnd( ID_GAME_CLOSE2 );
			}
			
			ShowChildWnd( ID_ROOM_INFO );
			ShowChildWnd( ID_DISPLAY );
			ShowChildWnd( ID_PRACTICE );
		}
		break;
	case MT_HOUSE:
		{
			if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
			{
				ShowChildWnd( ID_BACK_STAGE );
			}
			else
			{
				ShowChildWnd( ID_GAME_CLOSE2 );
			}

			ShowChildWnd( ID_ROOM_INFO );
			ShowChildWnd( ID_HOUSING_SETUP );
		}
		break;
	case MT_TRAINING:
		{
			if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
			{
				ShowChildWnd( ID_BACK_STAGE );
			}
			else
			{
				ShowChildWnd( ID_GAME_CLOSE2 );				
			}

			ShowChildWnd( ID_ROOM_INFO );
			ShowChildWnd( ID_FISHING );
			ShowChildWnd( ID_EXCAVATION );
			
			if( m_pPlayStage )
			{
				if( m_pPlayStage->GetPlazaModeType() == PT_GUILD )
				{					
					ShowChildWnd( ID_HOUSING_SETUP );
					HideChildWnd( ID_EXCAVATION );
				}
				else if( m_pPlayStage->GetPlazaModeType() == PT_COMMUNITY )
				{
					ShowChildWnd( ID_PLAZA_STATE );
				}
				m_ePrevPlazaType = m_pPlayStage->GetPlazaModeType();
			}
		}
		break;
	default:
		{
			if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
			{
				ShowChildWnd( ID_BACK_STAGE );
			}
			else
			{
				ShowChildWnd( ID_GAME_CLOSE2 );
			}

			if( !g_BattleRoomMgr.IsTournamentModeType() && !g_ShuffleRoomMgr.IsShuffleRoom() && !g_RankBattleMgr.IsRankBattlePlaying() )
				ShowChildWnd( ID_ROOM_INFO );
		}
		break;
	}

	if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
	{
		ShowChildWnd( ID_OPTION ); 
	}

	ioPriateRenewalRullet* pRullet = g_MyInfo.GetPriateRenewalRullet();
	if ( pRullet && pRullet->UsingOakPriate() )
		ShowChildWnd( ID_OAK );

	ReCreateBtn();

	m_ePrevModeType = ioPlayMode::GetModeType();

	if( g_MyInfo.IsTutorialUser() )
	{
		SetChildWndStyleAdd( ID_BACK_STAGE, IWS_EXACTIVE );
		SetChildWndStyleAdd( ID_GAME_CLOSE, IWS_EXACTIVE );
		SetChildWndStyleAdd( ID_GAME_CLOSE2, IWS_EXACTIVE );
		SetChildWndStyleAdd( ID_ROOM_INFO, IWS_EXACTIVE );
		SetChildWndStyleAdd( ID_DISPLAY, IWS_EXACTIVE );
		SetChildWndStyleAdd( ID_PRACTICE, IWS_EXACTIVE );
		SetChildWndStyleAdd( ID_SHOP, IWS_EXACTIVE );
		SetChildWndStyleAdd( ID_MYITEM, IWS_EXACTIVE );
		SetChildWndStyleAdd( ID_MESSENGER, IWS_EXACTIVE );
		SetChildWndStyleAdd( ID_PRESENT, IWS_EXACTIVE );
		SetChildWndStyleAdd( ID_PET, IWS_EXACTIVE );
		SetChildWndStyleAdd( ID_ROLLBOOK, IWS_EXACTIVE );

		m_bTutorial = true;
	}
	else if( g_MyInfo.IsBankruptcy() || g_MyInfo.IsCharBankruptcy() )
	{
		SetChildWndStyleAdd( ID_ROOM_INFO, IWS_EXACTIVE );
		SetChildWndStyleAdd( ID_MESSENGER, IWS_EXACTIVE );
	}
	else
	{
		SetChildWndStyleRemove( ID_ROOM_INFO, IWS_EXACTIVE );
		SetChildWndStyleRemove( ID_MESSENGER, IWS_EXACTIVE );
	}

	if( g_MyInfo.GetEntryType() == ET_TERMINATION || g_MyInfo.IsExpertEntryTemporary() || g_MyInfo.GetEntryType() == ET_TEMPORARY )
	{
		SetChildWndStyleAdd( ID_ROLLBOOK, IWS_EXACTIVE );
	}
}

void HelpWnd::ReCreateBtn()
{
	int i = 0;
	enum{ STEP_X = 48, STEP1_W = 23, STEP2_W = 88, };
	
	int iFirstBtnCnt = 0;

	std::vector<MenuBase*> MenuList;

	// 방정보, 훈련, 낚시, 탐사, (연습or대화)
	for( i = ID_ROOM_INFO; i < ID_OAK + 1; i++ )
	{
		ioWnd *pRePosBtn = FindChildWnd( i );
		if( pRePosBtn && pRePosBtn->IsShow() )
		{
			pRePosBtn->SetWndPos( STEP_X + iFirstBtnCnt * STEP1_W, 7 );
			iFirstBtnCnt++;

			MenuBase* pBtn = dynamic_cast<MenuBase*>( pRePosBtn );
			if( pBtn )
				MenuList.push_back( pBtn );
		}
	}

	if( 1 < MenuList.size() )
	{
		for( int iter = 0; iter < (int)MenuList.size(); ++iter )
		{
			if( iter == 0 )
				MenuList[iter]->SetMenuStyle( MenuBase::MS_START );
			else if( iter == (int)MenuList.size() - 1 )
				MenuList[iter]->SetMenuStyle( MenuBase::MS_END );
			else
				MenuList[iter]->SetMenuStyle( MenuBase::MS_MIDDLE );
		}
	}
	else if( 1 == MenuList.size() )
		MenuList.front()->SetMenuStyle( MenuBase::MS_NONE );

	int iRealXPos = STEP_X;
	if( iFirstBtnCnt != 0 )
		iRealXPos += ( iFirstBtnCnt * STEP1_W ) + 2;      // 한개라도 있으면 2픽셀 증가-_-;

	if( 0 < (int)MenuList.size() )
		iRealXPos += 6;

	// 상점, 내아이템, 선물함, 퀘스트
	for( i = ID_SHOP; i < ID_PET + 1; i++ )
	{
		ioWnd *pRePosBtn = FindChildWnd( i );
		if( pRePosBtn && pRePosBtn->IsShow() )
		{
			pRePosBtn->SetWndPos( iRealXPos + ((i-ID_SHOP) * STEP2_W), 7 );

			if( i == ID_MISSION_QUEST )
			{
				sg_fQuestXPos = pRePosBtn->GetDerivedPosX();
				sg_fQuestYPos = pRePosBtn->GetDerivedPosY();
			}

			if( i != ID_MISSION_QUEST )
			{
				if( ioPlayMode::GetModeType() == MT_ARENA && g_BattleRoomMgr.IsBattleModePlaying() )
				{
					pRePosBtn->SetInActive();
				}
				else
					pRePosBtn->SetActive();
			}
		}
	}

	//메신저
	iRealXPos = GetWidth() - 41;
	ioWnd *pRePosBtn = FindChildWnd( ID_MESSENGER );
	if( pRePosBtn )
	{
		pRePosBtn->SetWndPos( iRealXPos, 7 );
		sg_fMessengerXPos = pRePosBtn->GetDerivedPosX();
		sg_fMessengerYPos = pRePosBtn->GetDerivedPosY();
		iRealXPos -= 8;
	}

	//출석부 ~ 옵션 까지
	for( i = ID_OPTION; i >= ID_ROLLBOOK; --i )
	{
		ioWnd *pRePosBtn = FindChildWnd( i );
		if( pRePosBtn && pRePosBtn->IsShow() )
		{
			iRealXPos -= pRePosBtn->GetWidth();
			pRePosBtn->SetWndPos( iRealXPos, 7 );
		}
	}
}

void HelpWnd::SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate /* = false  */ )
{
	ioWnd::SetPositionToDefault( iParentWidth, iParentHeight, bChildMove, bCreate );

	ReCreateWindow();
}

void HelpWnd::DisplayProcess()
{
	if( g_App.GetConnectedMoveMode() != ioApplication::CMM_LOBBY )
	{
		if( g_GUIMgr.IsShow( LOBBY_QUICK_EXIT_WND ) )
			g_GUIMgr.HideWnd( LOBBY_QUICK_EXIT_WND );
	}

	if( !g_MyInfo.IsTutorialUser() )
	{
		if( m_pPlayStage )
		{
			ioHeadquartersMode *pHeadquartersMode = ToHeadquartersMode( m_pPlayStage->GetPlayMode() );
			if( pHeadquartersMode )
			{
				if( pHeadquartersMode->GetMasterName() != g_MyInfo.GetPublicID() )
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
					return;
				}
				else if( pHeadquartersMode->GetRecordPlayUserCnt() > 1 )
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
					return;
				}
			}
		}

		if( g_GUIMgr.IsShow( SOLDIER_DISPLAY_SELECT_WND ) )
			g_GUIMgr.HideWnd( SOLDIER_DISPLAY_SELECT_WND );
		else
			g_GUIMgr.ShowWnd( SOLDIER_DISPLAY_SELECT_WND );
	}
	else
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
	}
}

void HelpWnd::PracticeProcess()
{
	if( g_App.GetConnectedMoveMode() != ioApplication::CMM_LOBBY )
	{
		if( g_GUIMgr.IsShow( LOBBY_QUICK_EXIT_WND ) )
			g_GUIMgr.HideWnd( LOBBY_QUICK_EXIT_WND );
	}

	if( !g_MyInfo.IsTutorialUser() )
	{
		if( m_pPlayStage )
		{
			ioHeadquartersMode *pHeadquartersMode = ToHeadquartersMode( m_pPlayStage->GetPlayMode() );
			if( pHeadquartersMode )
			{
				if( pHeadquartersMode->GetMasterName() != g_MyInfo.GetPublicID() )
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
					return;
				}
				else if( pHeadquartersMode->GetRecordPlayUserCnt() > 1 )
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
					return;
				}
			}
		}

		PracticeHelpWnd *pPracticeHelp = dynamic_cast<PracticeHelpWnd *>( g_GUIMgr.FindWnd(PRACTICE_HELP_WND) );
		if( !pPracticeHelp ) return;

		if( pPracticeHelp->IsShow() )
			pPracticeHelp->HideWnd();
		else if( g_GUIMgr.IsShow( SOLDIER_PRACTICE_SELECT_WND ) )
			g_GUIMgr.HideWnd( SOLDIER_PRACTICE_SELECT_WND );
		else
			g_GUIMgr.ShowWnd( SOLDIER_PRACTICE_SELECT_WND );
	}
	else
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
	}
}

void HelpWnd::FishingProcess()
{
	if( g_App.GetConnectedMoveMode() != ioApplication::CMM_LOBBY )
	{
		if( g_GUIMgr.IsShow( LOBBY_QUICK_EXIT_WND ) )
			g_GUIMgr.HideWnd( LOBBY_QUICK_EXIT_WND );
	}

	if( g_GUIMgr.IsShow( EXCAVATION_WND ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		return;
	}

	if( !g_GUIMgr.IsShow( FISHING_INVENTORY_WND ) )
	{
		SP2Packet kPacket( CTPK_FISHING );
		kPacket << FISHING_OPEN;
		TCPNetwork::SendToServer( kPacket );
	}
	else
	{
		if( IsFishing() )
		{
			// 낚시 중이면 낚시 캔슬하고 낚시 인벤토리 닫기
			g_GUIMgr.HideWnd( FISHING_INVENTORY_WND );
		}
		else if( m_pPlayStage )
		{
			ioBaseChar *pOwnerChar = m_pPlayStage->GetOwnerChar();
			if( pOwnerChar )
			{
				// 낚시중이 아니면 낚시 시도
				if( pOwnerChar->EnableFishing( true ) )
				{
					pOwnerChar->SetFishingState( true );
				}
			}
		}
	}
}

void HelpWnd::ExcavationeProcess()
{
	if( !m_pPlayStage )
		return;

	if( !g_ExcavationMgr.IsExcavating() )
		g_ExcavationMgr.OnExcavationBtnUp( m_pPlayStage->GetOwnerChar() );
	else
		g_ExcavationMgr.SendReleaseKit();
}

void HelpWnd::HousingSetUpProcess()
{
	if( !m_pPlayStage )
		return;

	g_BlockSetupManager.HousingSetUpProcess();
}

void HelpWnd::PlazaStateProcess()
{
	g_ChatMacroMgr.MacroProcess( STR(1), true );
}

void HelpWnd::PresentProcess()
{
	if( g_App.GetConnectedMoveMode() != ioApplication::CMM_LOBBY )
	{
		if( g_GUIMgr.IsShow( LOBBY_QUICK_EXIT_WND ) )
			g_GUIMgr.HideWnd( LOBBY_QUICK_EXIT_WND );
	}

	if( g_MyInfo.GetEntryType() == ET_TERMINATION || g_MyInfo.GetEntryType() == ET_TEMPORARY ||
		g_MyInfo.IsExpertEntryTemporary() )
	{
		g_App.OpenFormalityEntryPage();
	}
	else if( !g_MyInfo.IsTutorialUser() )
	{
		if( ioPlayMode::GetModeType() != MT_ARENA || ( ioPlayMode::GetModeType() == MT_ARENA && !g_BattleRoomMgr.IsBattleModePlaying() ) )
		{
			MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
			if( pInvenWnd )
			{
				if( pInvenWnd->IsShowPresentTab() )
					pInvenWnd->HideWnd();
				else
					pInvenWnd->ShowSubscriptionTabDirect();
			}
		}
	}
	else
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
	}
}

void HelpWnd::ShopProcess()
{
	if( g_App.GetConnectedMoveMode() != ioApplication::CMM_LOBBY )
	{
		if( g_GUIMgr.IsShow( LOBBY_QUICK_EXIT_WND ) )
			g_GUIMgr.HideWnd( LOBBY_QUICK_EXIT_WND );
	}

	if( g_MyInfo.GetEntryType() == ET_TERMINATION || g_MyInfo.GetEntryType() == ET_TEMPORARY ||
		g_MyInfo.IsExpertEntryTemporary() )
	{
		g_App.OpenFormalityEntryPage();
	}
	else if( !g_MyInfo.IsTutorialUser() )
	{
		if( ioPlayMode::GetModeType() != MT_ARENA || ( ioPlayMode::GetModeType() == MT_ARENA && !g_BattleRoomMgr.IsBattleModePlaying() ) )
		{
			if( g_GUIMgr.IsShow( NEW_SHOP_WND ) )
			{
				g_GUIMgr.HideWnd( NEW_SHOP_WND );
			}
			else
			{
				g_GUIMgr.ShowWnd( NEW_SHOP_WND );
			}
		}
	}
	else
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
	}
}

void HelpWnd::MyItemProcess()
{
	if( g_App.GetConnectedMoveMode() != ioApplication::CMM_LOBBY )
	{
		if( g_GUIMgr.IsShow( LOBBY_QUICK_EXIT_WND ) )
			g_GUIMgr.HideWnd( LOBBY_QUICK_EXIT_WND );
	}

	if( g_MyInfo.GetEntryType() == ET_TERMINATION || g_MyInfo.GetEntryType() == ET_TEMPORARY ||
		g_MyInfo.IsExpertEntryTemporary() )
	{
		g_App.OpenFormalityEntryPage();
	}
	else if( !g_MyInfo.IsTutorialUser() )
	{
		if( ioPlayMode::GetModeType() != MT_ARENA || ( ioPlayMode::GetModeType() == MT_ARENA && !g_BattleRoomMgr.IsBattleModePlaying() ) )
		{
			if( g_GUIMgr.IsShow( MY_INVENTORY_WND ) )
				g_GUIMgr.HideWnd( MY_INVENTORY_WND );
			else
			{
				g_GUIMgr.ShowWnd( MY_INVENTORY_WND );
			}
		}
	}
	else
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
	}
}

void HelpWnd::QuestProcess()
{
	if( g_App.GetConnectedMoveMode() != ioApplication::CMM_LOBBY )
	{
		if( g_GUIMgr.IsShow( LOBBY_QUICK_EXIT_WND ) )
			g_GUIMgr.HideWnd( LOBBY_QUICK_EXIT_WND );
	}

	if( g_GUIMgr.IsShow( MISSION_QUEST_MAIN_WND ) )
		g_GUIMgr.HideWnd( MISSION_QUEST_MAIN_WND );
	else
		g_GUIMgr.ShowWnd( MISSION_QUEST_MAIN_WND );
}

void HelpWnd::PetProcess()
{
	if( g_App.GetConnectedMoveMode() != ioApplication::CMM_LOBBY )
	{
		if( g_GUIMgr.IsShow( LOBBY_QUICK_EXIT_WND ) )
			g_GUIMgr.HideWnd( LOBBY_QUICK_EXIT_WND );
	}

	if( g_MyInfo.GetEntryType() == ET_TERMINATION || g_MyInfo.GetEntryType() == ET_TEMPORARY ||
		g_MyInfo.IsExpertEntryTemporary() )
	{
		g_App.OpenFormalityEntryPage();
	}
	else if( !g_MyInfo.IsTutorialUser() )
	{
		if( ioPlayMode::GetModeType() != MT_ARENA || ( ioPlayMode::GetModeType() == MT_ARENA && !g_BattleRoomMgr.IsBattleModePlaying() ) )
		{
			ioUserPet *pUserPet = g_MyInfo.GetUserPet();
			if( pUserPet )
			{
				if ( pUserPet->GetPetSlotCnt() <= 0 )
				{
					if( g_GUIMgr.IsShow( PET_WND ) )
						g_GUIMgr.HideWnd( PET_WND );
					else
					{
						if( g_GUIMgr.IsShow( PET_EMPTY_WND ) )
							g_GUIMgr.HideWnd( PET_EMPTY_WND );
						else
							g_GUIMgr.ShowWnd( PET_EMPTY_WND );
					}
				}
				else
				{
					if( g_GUIMgr.IsShow( PET_WND ) )
						g_GUIMgr.HideWnd( PET_WND );
					else
						g_GUIMgr.ShowWnd( PET_WND );
				}
			}
		}
	}
	else
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
	}
}

void HelpWnd::RollBookProcess()
{
	if( g_App.GetConnectedMoveMode() != ioApplication::CMM_LOBBY )
	{
		if( g_GUIMgr.IsShow( LOBBY_QUICK_EXIT_WND ) )
			g_GUIMgr.HideWnd( LOBBY_QUICK_EXIT_WND );
	}
	
	if( g_GUIMgr.IsShow( ROLLBOOK_WND ) )
		g_GUIMgr.HideWnd( ROLLBOOK_WND );
	else
	{
		RollBookWnd* pWnd = dynamic_cast<RollBookWnd*>( g_GUIMgr.FindWnd( ROLLBOOK_WND ) );
		if ( pWnd )
			pWnd->ShowRollBookWnd();
	}
}

void HelpWnd::ManualProcess()
{
	if( g_App.GetConnectedMoveMode() != ioApplication::CMM_LOBBY )
	{
		if( g_GUIMgr.IsShow( LOBBY_QUICK_EXIT_WND ) )
			g_GUIMgr.HideWnd( LOBBY_QUICK_EXIT_WND );
	}

	if( g_GUIMgr.IsShow( MANUAL_LIST_WND ) )
		g_GUIMgr.HideWnd( MANUAL_LIST_WND );
	else
		g_GUIMgr.ShowWnd( MANUAL_LIST_WND );
}

void HelpWnd::OptionProcess()
{
	if( g_App.GetConnectedMoveMode() != ioApplication::CMM_LOBBY )
	{
		if( g_GUIMgr.IsShow( LOBBY_QUICK_EXIT_WND ) )
			g_GUIMgr.HideWnd( LOBBY_QUICK_EXIT_WND );
	}

	if( g_GUIMgr.IsShow( GAME_OPTION_WND ) )
	{
		//JSM. 단축키 지정창이 떠있을때에는 Hide 시키지 않음(하이드가 되면 단축키 지정이 불가능)
		if(! g_GUIMgr.IsShow( GAME_OPTION_SETTING_KEYINPUT_WND ) )		
		{
			g_GUIMgr.HideWnd( GAME_OPTION_WND );
		}
	}
	else
	{
		g_GUIMgr.ShowWnd( GAME_OPTION_WND );
	}
}

void HelpWnd::MessengerProcess()
{
	if( g_App.GetConnectedMoveMode() != ioApplication::CMM_LOBBY )
	{
		if( g_GUIMgr.IsShow( LOBBY_QUICK_EXIT_WND ) )
			g_GUIMgr.HideWnd( LOBBY_QUICK_EXIT_WND );
	}

	if( g_MyInfo.GetEntryType() == ET_TERMINATION || g_MyInfo.GetEntryType() == ET_TEMPORARY ||
		g_MyInfo.IsExpertEntryTemporary() )
	{
		g_App.OpenFormalityEntryPage();
	}
	else
	{
		if( g_MyInfo.IsTutorialUser() )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
			return;
		}
		else if( g_MyInfo.IsBankruptcy() || g_MyInfo.IsCharBankruptcy() )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "사용 가능한 용병을 구입하시면 #메신저 이용이 가능합니다." );
			return;
		}

		if( g_GUIMgr.IsShow( MESSENGER_WND ) )
			g_GUIMgr.HideWnd( MESSENGER_WND );
		else
			g_GUIMgr.ShowWnd( MESSENGER_WND );			
	}
}

void HelpWnd::EscKeyProcess()
{
	if( !IsShow() )
		return;
		
	ioWnd *pPreClick = g_GUIMgr.GetPreClickWnd();

	if( pPreClick == NULL								&&
		!g_GUIMgr.IsShow( QUEST_OCCUR_STATE_WND )		&&
		!g_GUIMgr.IsShow( QUEST_ATTAIN_STATE_WND )		&&
		!g_GUIMgr.IsShow( EX_PCROOM_AUTHORITY_WND )		&&
		!g_GUIMgr.IsShow( EX_PCROOM_SOLDIER_BONUS_WND )	&&
		!g_GUIMgr.IsShow( ATTENDANCE_WND ) 				&&
		!g_GUIMgr.IsShow( USER_SELECT_HERO_BONUS_WND ) )
	{
		if( g_App.GetConnectedMoveMode() != ioApplication::CMM_LOBBY )
		{
			g_GUIMgr.ShowWnd( LOBBY_QUICK_EXIT_WND );
		}
		else
		{
			g_App.AltF4ExitProcess();
		}
	}

	if( sg_dwMoveState == MOVE_STATE_SHOW )
		return;

	sg_dwMoveState = MOVE_STATE_SHOW;
	m_dwMoveStateDelayTIme = m_dwMoveStateTime = m_dwMoveStateStartDelayTIme = FRAMEGETTIME();
	m_dwShowKeepTime = 5000;
}

void HelpWnd::OakProcess()
{
	if( g_App.GetConnectedMoveMode() != ioApplication::CMM_LOBBY )
	{
		if( g_GUIMgr.IsShow( LOBBY_QUICK_EXIT_WND ) )
			g_GUIMgr.HideWnd( LOBBY_QUICK_EXIT_WND );
	}

	if( g_GUIMgr.IsShow( PRIATE_RULLET_RENEWAL_WND ) )
		g_GUIMgr.HideWnd( PRIATE_RULLET_RENEWAL_WND );
	else
	{
		g_GUIMgr.ShowWnd( PRIATE_RULLET_RENEWAL_WND );
		//UI를 한번이라도 클릭 했다면 Oak Tool Tip을 더이상 보여주지 않는다!
		if ( sg_bShowOakToolTip )
			sg_bShowOakToolTip = false;
	}
}

void HelpWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;	

	if( ( m_ePrevModeType != ioPlayMode::GetModeType() ) || ( m_pPlayStage && m_pPlayStage->GetPlazaModeType() != m_ePrevPlazaType ) )
	{
		ReCreateWindow();
	}

	bool bLobbyLock = false;
	if( g_MyInfo.GetLobbyLockType() != LobbyLockProblemWnd::LT_NONE )
	{
		if( ioPlayMode::GetModeType() == MT_MYROOM )
			bLobbyLock = true;       // 로비 잠김
	}

	if( !bLobbyLock && !g_GUIMgr.IsUIKeyBlock() && !g_GUIMgr.IsShowSettingKeyWnd() && !g_App.IsMouseBusy() )
	{
		if( g_KeyManager.IsHelpKeyPress() )
		{
			ManualProcess();
		}
		else if( g_KeyManager.IsShopKeyPress() )
		{
			ShopProcess();
		}
		else if( g_KeyManager.IsGrowthKeyPress() )
		{
			MyItemProcess();
		}
		else if( g_KeyManager.IsQuestKeyPress() )
		{
			QuestProcess();
		}
		else if( g_KeyManager.IsPresentKeyPress() )
		{
			PresentProcess();
		}
		else if( g_KeyManager.IsMyHomePageKeyPress() )
		{
			ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
			if( !pLocal )
				return;
			if( pLocal->IsMyHomePage() )
				g_App.OpenHeadQuaterPage( g_MyInfo.GetPublicID().c_str() );
		}
		else if( g_KeyManager.IsOptionKeyPress() )
		{
			OptionProcess();
		}
		else if( g_KeyManager.IsMessengerKeyPress() )
		{
			MessengerProcess();
		}
		else if ( g_KeyManager.IsPetKeyPress() )
		{
			PetProcess();
		}
		else if ( g_KeyManager.IsBattleModeTagSwitchPress() )
		{
			// 배틀모드 대기자일때
			if( !g_BattleRoomMgr.CheckUserBattleFightFlag( g_MyInfo.GetPublicID() ) && !g_BattleRoomMgr.IsObserverUser( g_MyInfo.GetPublicID() ) )
			{
				BattleModeFunctionWnd *pBattleModeFuncWnd = dynamic_cast<BattleModeFunctionWnd*>(g_GUIMgr.FindWnd( BATTLE_MODE_FUNCTION_WND ));
				if( pBattleModeFuncWnd )
					pBattleModeFuncWnd->SwitchingTag( true );
			}
		}
		else if ( g_KeyManager.IsBattleModeSoldierSelectPress() )
		{
			// 배틀모드 대기자일때
			if( !g_BattleRoomMgr.CheckUserBattleFightFlag( g_MyInfo.GetPublicID() ) && !g_BattleRoomMgr.IsObserverUser( g_MyInfo.GetPublicID() ) )
			{
				BattleModeFunctionWnd *pBattleModeFuncWnd = dynamic_cast<BattleModeFunctionWnd*>(g_GUIMgr.FindWnd( BATTLE_MODE_FUNCTION_WND ));
				if( pBattleModeFuncWnd )
					pBattleModeFuncWnd->SelectSoldier();
			}
		}
		else if ( g_KeyManager.IsBattleModeEntryPress() )
		{
			// 배틀모드 대기자일때
			if( !g_BattleRoomMgr.CheckUserBattleFightFlag( g_MyInfo.GetPublicID() ) && !g_BattleRoomMgr.IsObserverUser( g_MyInfo.GetPublicID() ) )
			{
				BattleModeFunctionWnd *pBattleModeFuncWnd = dynamic_cast<BattleModeFunctionWnd*>(g_GUIMgr.FindWnd( BATTLE_MODE_FUNCTION_WND ));
				if( pBattleModeFuncWnd )
					pBattleModeFuncWnd->Entry();
			}
		}
	}

	OnProcessMove();

	if( m_bTutorial && !g_MyInfo.IsTutorialUser() )
	{
		SetChildWndStyleRemove( ID_BACK_STAGE, IWS_EXACTIVE );
		SetChildWndStyleRemove( ID_GAME_CLOSE, IWS_EXACTIVE );
		SetChildWndStyleRemove( ID_GAME_CLOSE2, IWS_EXACTIVE );
		SetChildWndStyleRemove( ID_ROOM_INFO, IWS_EXACTIVE );
		SetChildWndStyleRemove( ID_DISPLAY, IWS_EXACTIVE );
		SetChildWndStyleRemove( ID_PRACTICE, IWS_EXACTIVE );
		SetChildWndStyleRemove( ID_SHOP, IWS_EXACTIVE );
		SetChildWndStyleRemove( ID_MYITEM, IWS_EXACTIVE );
		SetChildWndStyleRemove( ID_MESSENGER, IWS_EXACTIVE );
		SetChildWndStyleRemove( ID_PRESENT, IWS_EXACTIVE );
		SetChildWndStyleRemove( ID_PET, IWS_EXACTIVE );
		m_bTutorial = false;
	}
}

void HelpWnd::OnProcessMove()
{
	// 길드본부/개인본부/훈련소/광장이면 항상 나와있음
	if( ioPlayMode::GetModeType() == MT_MYROOM || ioPlayMode::GetModeType() == MT_TRAINING || ioPlayMode::GetModeType() == MT_HEADQUARTERS || ioPlayMode::GetModeType() == MT_HOUSE )
		return;

	// 전투 종료된 상태면 항상 나와있음
	if( TotalResultMainWnd::IsHelpBarShow() )
	{
		if( sg_dwMoveState == MOVE_STATE_SHOW && m_dwMoveStateTime == 0 )
			return;
		else if( sg_dwMoveState != MOVE_STATE_SHOW )
		{
			sg_dwMoveState = MOVE_STATE_SHOW;
			m_dwMoveStateDelayTIme = m_dwMoveStateTime = FRAMEGETTIME();
		}
	}

	if( m_dwMoveStateTime == 0 )
	{
		if( sg_dwMoveState == MOVE_STATE_SHOW )
		{
			if( FRAMEGETTIME() - m_dwMoveStateDelayTIme < 2000 )
				return;
		}
		else
		{
			if( FRAMEGETTIME() - m_dwMoveStateDelayTIme < 1000 )
				return;
		}

		// 처음 입장시 9초뒤에 사라진다.
		if( FRAMEGETTIME() - m_dwMoveStateStartDelayTIme < m_dwShowKeepTime )
			return;

		// 판단
		switch( sg_dwMoveState )
		{
		case MOVE_STATE_SHOW:
			{
				// 감춰야하는지 확인
				ioLSMouse *pMouse = g_App.GetMouse();
				RECT rcRect = { GetXPos(), Setting::Height() - POS_Y_GAP, GetXPos() + GetWidth(), Setting::Height() };
				if( !g_App.IsVisibleMouse() || !::PtInRect( &rcRect, pMouse->GetMousePos() ) )
				{
					sg_dwMoveState = MOVE_STATE_HIDE;
					m_dwMoveStateDelayTIme = m_dwMoveStateTime = FRAMEGETTIME();
				}
			}
			break;
		case MOVE_STATE_HIDE:
			{
				// 나와야하는지 확인
				if( g_App.IsVisibleMouse() )
				{
					ioLSMouse *pMouse = g_App.GetMouse();
					RECT rcRect = { GetXPos(), Setting::Height() - 10, GetXPos() + GetWidth(), Setting::Height() };
					if( ::PtInRect( &rcRect, pMouse->GetMousePos() ) )
					{
						sg_dwMoveState = MOVE_STATE_SHOW;
						m_dwMoveStateDelayTIme = m_dwMoveStateTime = FRAMEGETTIME();
					}
				}
			}
			break;
		}
	}
	else
	{
		// 애니메이션 프로세스
		switch( sg_dwMoveState )
		{
		case MOVE_STATE_SHOW:
			{
				DWORD dwTimeGap = FRAMEGETTIME() - m_dwMoveStateTime;
				if( dwTimeGap < ANI_FRAME_TIME )
				{
					float fRate = FLOAT1 - ( sinf( ( D3DX_PI / 2 ) * ( (float)dwTimeGap / ANI_FRAME_TIME ) ) );
					m_fMoveWndYPos = (float)MAX_ANI_YPOS * fRate;
				}
				else
				{
					m_dwMoveStateTime = 0;
					m_fMoveWndYPos    = 0.0f;					
				}
				SetWndPos( POS_X_GAP, (Setting::Height() - POS_Y_GAP) + m_fMoveWndYPos );
				
				ioWnd *pRePosBtn = FindChildWnd( ID_MESSENGER );
				if( pRePosBtn )
				{
					sg_fMessengerXPos = pRePosBtn->GetDerivedPosX();
					sg_fMessengerYPos = pRePosBtn->GetDerivedPosY();
				}
				pRePosBtn = FindChildWnd( ID_MISSION_QUEST );
				if( pRePosBtn )
				{
					sg_fQuestXPos = pRePosBtn->GetDerivedPosX();
					sg_fQuestYPos = pRePosBtn->GetDerivedPosY();
				}
			}
			break;
		case MOVE_STATE_HIDE:
			{
				DWORD dwTimeGap = FRAMEGETTIME() - m_dwMoveStateTime;
				if( dwTimeGap < ANI_FRAME_TIME )
				{
					float fRate = sinf( ( D3DX_PI / 2 ) * ( (float)dwTimeGap / ANI_FRAME_TIME ) );
					m_fMoveWndYPos = (float)MAX_ANI_YPOS * fRate;
				}
				else
				{
					m_dwMoveStateTime = 0;
					m_fMoveWndYPos    = MAX_ANI_YPOS;					
				}
				SetWndPos( POS_X_GAP, (Setting::Height() - POS_Y_GAP) + m_fMoveWndYPos );				

				ioWnd *pRePosBtn = FindChildWnd( ID_MESSENGER );
				if( pRePosBtn )
				{
					sg_fMessengerXPos = pRePosBtn->GetDerivedPosX();
					sg_fMessengerYPos = pRePosBtn->GetDerivedPosY();
				}
				pRePosBtn = FindChildWnd( ID_MISSION_QUEST );
				if( pRePosBtn )
				{
					sg_fQuestXPos = pRePosBtn->GetDerivedPosX();
					sg_fQuestYPos = pRePosBtn->GetDerivedPosY();
				}
			}
			break;
		}
	}	
}

void HelpWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	OnRenderBar( iXPos - 14, iYPos - 6, Setting::Width() - 108 );
}

void HelpWnd::OnRenderBar( int iXPos, int iYPos, int iWidth )
{
	if( !m_pBarLeftMultiply || !m_pBarCenterMultiply || !m_pBarRightMultiply || 
		!m_pBarLeft || !m_pBarCenter || !m_pBarRight || !m_pHideBar || !m_pHideIcon ) return;

	m_pBarLeftMultiply->Render( iXPos, iYPos, UI_RENDER_MULTIPLY );		
	m_pBarCenterMultiply->SetSize( iWidth, m_pBarCenterMultiply->GetHeight() );
	m_pBarCenterMultiply->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
	m_pBarCenterMultiply->Render( iXPos + m_pBarLeftMultiply->GetWidth(), iYPos, UI_RENDER_MULTIPLY );
	m_pBarRightMultiply->Render( iXPos + m_pBarLeftMultiply->GetWidth() + iWidth, iYPos, UI_RENDER_MULTIPLY );

	m_pBarLeft->Render( iXPos, iYPos );		
	m_pBarCenter->SetSize( iWidth, m_pBarCenter->GetHeight() );
	m_pBarCenter->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
	m_pBarCenter->Render( iXPos + m_pBarLeft->GetWidth(), iYPos );
	m_pBarRight->Render( iXPos + m_pBarLeft->GetWidth() + iWidth, iYPos );
}

void HelpWnd::OnRenderAfterChild()
{
	for ( int i=ID_ROOM_INFO ; i<= ID_OPTION ; ++i )
	{
		MenuBtn* pMenuBtn = dynamic_cast<MenuBtn*>( FindChildWnd( i ) );
		if ( pMenuBtn && pMenuBtn->IsShow() )
		{
			pMenuBtn->OnRenderTooltip();
		}
	}
}

bool HelpWnd::QuestGuide( int iBtnID, float &rfXPos, float &rfYPos )
{
	if( sg_dwMoveState == MOVE_STATE_HIDE )
		return false;

	ioWnd *pBtn = FindChildWnd( iBtnID );
	if( pBtn && pBtn->IsShow() )
	{
		rfXPos = pBtn->GetDerivedPosX() + ( pBtn->GetWidth() / 2 );
		rfYPos = pBtn->GetDerivedPosY();
		return true;
	}
	return false;
}