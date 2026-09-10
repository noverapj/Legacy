#include "StdAfx.h"
#include "MatchingGameWnd.h"
#include "../ioMatchingGame.h"

MatchingGameCardButton::MatchingGameCardButton()
{
	m_pCardBack	= NULL;
	m_pCardImage	= NULL;
	m_pOpenEffect	= NULL;
	m_pMatchEffect	= NULL;
}

MatchingGameCardButton::~MatchingGameCardButton()
{
	SAFEDELETE( m_pCardBack );
	SAFEDELETE( m_pOpenEffect );
	SAFEDELETE( m_pMatchEffect );
}

void	MatchingGameCardButton::ParseExtraInfo( ioXMLElement &xElement )
{
	ioButton::ParseExtraInfo( xElement );

	m_dwCardOpenEffectTime = xElement.GetIntAttribute_e( "Opening_Effect_Gap_Time" );
	m_vCardOpenEffect.clear();
	int iMax = xElement.GetIntAttribute_e( "Max_Card_Opening_Effect" );
	m_vCardOpenEffect.reserve( iMax );
	for (int i = 0; i < iMax ; i++)
	{
		char szName[MAX_PATH]="";
		StringCbPrintf_e( szName, sizeof( szName ), "Card_Opening_Effect_%d", i+1 );
		ioHashString szEffect = xElement.GetStringAttribute( szName );
		if( szEffect == "" )
			break;
		m_vCardOpenEffect.push_back( szEffect );
	}

	m_dwCardMatchEffectTime = xElement.GetIntAttribute_e( "Matching_Effect_Gap_Time" );
	m_vCardMatchEffect.clear();
	iMax = xElement.GetIntAttribute_e( "Max_Card_Matching_Effect" );
	m_vCardMatchEffect.reserve( iMax );
	for (int i = 0; i < iMax ; i++)
	{
		char szName[MAX_PATH]="";
		StringCbPrintf_e( szName, sizeof( szName ), "Card_Matching_Effect_%d", i+1 );
		ioHashString szEffect = xElement.GetStringAttribute( szName );
		if( szEffect == "" )
			break;
		m_vCardMatchEffect.push_back( szEffect );
	}

	m_iCardOpeningTime	= xElement.GetIntAttribute_e( "Card_Opening_Time" );
	m_iCardOpenTime		= xElement.GetIntAttribute_e( "Card_Open_Time" );
}

void	MatchingGameCardButton::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "CardBack" )
	{
		SAFEDELETE( m_pCardBack );
		m_pCardBack = pImage;
	}
	else
	{
		ioButton::AddRenderImage( szType, pImage );
	}
}

void	MatchingGameCardButton::SetStatus( int iStatus )
{
	if( COMPARE( m_Status, MGCBS_CLOSED, MGCBS_CLOSING + 1 ) )
	{
		m_Status	= iStatus;
		m_dwStatusChangeTime	= REALGETTIME();

		if( m_Status == MGCBS_OPENING )
		{
			MatchingGameWnd*	pWnd	= dynamic_cast<MatchingGameWnd*>( g_GUIMgr.FindWnd( MATCHING_GAME_WND ) );
			if( pWnd )
				pWnd->SetButtonActivate( this );
		}
	}
}

int		MatchingGameCardButton::GetStatus()
{
	return	m_Status;
}

void	MatchingGameCardButton::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	ioMatchingGame*	pMatchingGame	= g_MyInfo.GetMatchingGame();
	MatchingGameWnd*	pWnd	= dynamic_cast<MatchingGameWnd*>( g_GUIMgr.FindWnd( MATCHING_GAME_WND ) );

	if( pWnd && !pWnd->IsShow() )
		return;

	if( pMatchingGame )
	{
		int	iCardNum	= pMatchingGame->GetCellCardNum( GetID() - MatchingGameWnd::ID_CARD_SELECT1 );
		if( iCardNum != ioMatchingGame::CARD_NUMBER_NOT_OPENED && m_pCardImage == NULL )
		{
			if( pWnd )
			{
				SetCardImage( pWnd->GetCardImage( iCardNum ) );
				SetStatus( MGCBS_OPENING );
			}
		}
	}

	switch( m_Status )
	{
	case MGCBS_OPENING:
		{
			if( REALGETTIME() - m_dwStatusChangeTime > m_iCardOpeningTime )
				SetStatus( MGCBS_OPENED );

			CardOpenEffect( fTimePerSec );
		}
		break;

	case MGCBS_OPENED:
		{
			ioMatchingGame*	pMatchingGame	= g_MyInfo.GetMatchingGame();
			if( pMatchingGame )
			{
				if( pWnd )				
				{
					MatchingGameCardButton*	pSecondButton	= pWnd->GetActivatedButton( false );

					if( pSecondButton != this )
					{
						if( pSecondButton == NULL || ( pSecondButton && pSecondButton->GetStatus() <= MGCBS_OPENING ) )
						{
							m_dwStatusChangeTime	= REALGETTIME();
						}
						else if( pSecondButton && pSecondButton->GetStatus() == MGCBS_OPENED )
						{
							m_dwStatusChangeTime	= pSecondButton->GetStatusChangeTime();
						}
					}
				}
			
				if( REALGETTIME() - m_dwStatusChangeTime > m_iCardOpenTime )
				{
					int	iCurrentCell	= GetID() - MatchingGameWnd::ID_CARD_SELECT1;

					if( pMatchingGame->IsCellOpened( iCurrentCell ) == false )
						SetStatus( MGCBS_CLOSING );
					else
					{
						if( pWnd )
						{
							MatchingGameCardButton*	pFirstButton	= pWnd->GetActivatedButton();
							MatchingGameCardButton*	pSecondButton	= pWnd->GetActivatedButton( false );

							if( pFirstButton )
							{
								pFirstButton->SetStatus( MGCBS_MATCHED );
								pWnd->ResetButtonActivate();

								ioHashString	szSound	= pWnd->GetMatchingSuccessSound();

								if( !szSound.IsEmpty() )
									g_SoundMgr.PlaySound( szSound );
							}

							if( pSecondButton )
							{
								pSecondButton->SetStatus( MGCBS_MATCHED );
								pWnd->ResetButtonActivate( false );
							}
						}
					}
				}
			}

			m_iCurEffectArray	= -1;
		}
		break;

	case MGCBS_CLOSING:
		{
			//if( REALGETTIME() - m_dwStatusChangeTime > OPEN_CLOSE_PROCESS_TIME )
			{
				SetStatus( MGCBS_CLOSED );

				MatchingGameWnd*	pWnd	= dynamic_cast<MatchingGameWnd*>( g_GUIMgr.FindWnd( MATCHING_GAME_WND ) );
				if( pWnd )
				{
					if( pWnd->GetActivatedButton() == this )
						pWnd->ResetButtonActivate();
					else
						pWnd->ResetButtonActivate( false );
				}
			}
		}
		break;

	case MGCBS_MATCHED:
		{
			CardMatchEffect( fTimePerSec );
		}
		break;
	}
}

void	MatchingGameCardButton::OnRender()
{
	ioButton::OnRender();

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	ioMatchingGame*	pMatchingGame	= g_MyInfo.GetMatchingGame();
	bool	bOpened	= pMatchingGame->IsCellOpened( GetID() - MatchingGameWnd::ID_CARD_SELECT1 );

	switch( m_Status )
	{
	case MGCBS_CLOSED:
		{
			if( m_pCardBack )
			{
				m_pCardBack->SetScale( 0.81f, 0.81f );
				m_pCardBack->Render( iXPos, iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
			}
		}
		break;

	case MGCBS_OPENED:
	case MGCBS_MATCHED:
		{
			if( m_pCardImage )
			{
				m_pCardImage->SetScale( 0.81f, 0.81f );
				m_pCardImage->Render( iXPos + 26, iYPos + 26, UI_RENDER_NORMAL, TFO_BILINEAR );
			}
		}
		break;

	case MGCBS_OPENING:
	case MGCBS_CLOSING:
		{

		}
		break;
	}
}

void	MatchingGameCardButton::OnRenderAfterChild()
{
	ioWnd::OnRenderAfterChild();

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	if( m_pOpenEffect )
	{
		m_pOpenEffect->Render( iXPos + 26.0f, iYPos + 26.0f, UI_RENDER_NORMAL );
	}

	if( m_pMatchEffect )
	{
		m_pMatchEffect->Render( iXPos + 26.0f, iYPos + 26.0f, UI_RENDER_NORMAL );
	}
}

void	MatchingGameCardButton::SetCardImage( ioUIRenderImage* pImage )
{
	if( !m_pCardImage )
		m_pCardImage	= pImage;
}

void	MatchingGameCardButton::ResetCardImage()
{
	m_pCardImage	= NULL;
}

void	MatchingGameCardButton::Reset()
{
	ResetCardImage();

	m_iCurEffectArray	= -1;
	m_dwCardOpenEffectCheckTime	= 0;
	m_dwCardMatchEffectCheckTime	= 0;
	SAFEDELETE( m_pOpenEffect );
	SAFEDELETE( m_pMatchEffect );

	m_Status	= MGCBS_CLOSED;
}

void	MatchingGameCardButton::CardOpenEffect( float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();

	int iEffectListCnt = m_vCardOpenEffect.size();
	if( iEffectListCnt == 0 )
	{
		SAFEDELETE( m_pOpenEffect );
		return;
	}

	if( m_iCurEffectArray == iEffectListCnt-1 )
	{
		SAFEDELETE( m_pOpenEffect );
		return;
	}

	if( m_Status != MGCBS_OPENING )
	{
		SAFEDELETE( m_pOpenEffect );
		return;
	}

	if( m_dwCardOpenEffectCheckTime == 0 || dwCurTime - m_dwCardOpenEffectCheckTime > m_dwCardOpenEffectTime )		// 교체
	{
		SAFEDELETE( m_pOpenEffect );

		m_iCurEffectArray++;

		if( !COMPARE( m_iCurEffectArray, 0, iEffectListCnt ) )
			m_iCurEffectArray = 0;

		ioHashString szEffect = m_vCardOpenEffect[m_iCurEffectArray];

		if( !szEffect.IsEmpty() )
		{
			m_pOpenEffect = g_UIImageSetMgr.CreateImageByFullName( szEffect );
		}

		m_dwCardOpenEffectCheckTime = dwCurTime;
	}
}

void	MatchingGameCardButton::CardMatchEffect( float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();

	int iEffectListCnt = m_vCardMatchEffect.size();
	if( iEffectListCnt == 0 )
	{
		SAFEDELETE( m_pMatchEffect );
		return;
	}

	if( m_iCurEffectArray == iEffectListCnt-1 )
	{
		SAFEDELETE( m_pMatchEffect );
		return;
	}

	if( m_dwCardMatchEffectCheckTime == 0 || dwCurTime - m_dwCardMatchEffectCheckTime > m_dwCardMatchEffectTime )		// 교체
	{
		SAFEDELETE( m_pMatchEffect );

		m_iCurEffectArray++;

		if( !COMPARE( m_iCurEffectArray, 0, iEffectListCnt ) )
			m_iCurEffectArray = 0;

		ioHashString szEffect = m_vCardMatchEffect[m_iCurEffectArray];

		if( !szEffect.IsEmpty() )
		{
			m_pMatchEffect = g_UIImageSetMgr.CreateImageByFullName( szEffect );
		}

		m_dwCardMatchEffectCheckTime = dwCurTime;
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------
MatchingGameWnd::MatchingGameWnd()
{
	m_pLucky1Icon	= NULL;
	m_pLucky2Icon	= NULL;
	m_CardIconList.clear();

	m_pNormalTicket	= NULL;
	m_pPremiumTicket	= NULL;
	m_pMissionEnd	= NULL;
	m_pMissionCheck	= NULL;
	m_pMissionEnable	= NULL;
	m_pGaugeBarLeft	= NULL;
	m_pGaugeBarCenter	= NULL;
	m_pGaugeBarRight	= NULL;
	m_pEffect	= NULL;
	m_pMissionRewardIcon	= NULL;
	m_pButtonImage	= NULL;
	m_pComplete	= NULL;
	m_pCountCircle	= NULL;

	for( int i = 0; i < MAX_REWARD_ICON; i++ )
	{
		m_pRewardIcon[i]	= NULL;
	}

	for( int i = 0; i < COUNT_DOWN_MAX; i++ )
	{
		m_pCountDown[i]	= NULL;
	}

	m_pMainTitle	= NULL;
	m_pMissionTitle	= NULL;
	m_pRewardTitle	= NULL;
	m_pGameDisbled	= NULL;

	m_bCountDown	= false;
}

MatchingGameWnd::~MatchingGameWnd()
{
	SAFEDELETE( m_pLucky1Icon );
	SAFEDELETE( m_pLucky2Icon );

	for( DWORD i = 0; i < m_CardIconList.size(); i++ )
	{
		SAFEDELETE( m_CardIconList[i] )
	}
	m_CardIconList.clear();

	SAFEDELETE( m_pNormalTicket );
	SAFEDELETE( m_pPremiumTicket );
	SAFEDELETE( m_pMissionEnd );
	SAFEDELETE( m_pMissionCheck );
	SAFEDELETE( m_pMissionEnable );
	SAFEDELETE( m_pGaugeBarLeft );
	SAFEDELETE( m_pGaugeBarCenter );
	SAFEDELETE( m_pGaugeBarRight );
	SAFEDELETE( m_pEffect );
	SAFEDELETE( m_pMissionRewardIcon );
	SAFEDELETE( m_pButtonImage );
	SAFEDELETE( m_pComplete );
	SAFEDELETE( m_pCountCircle );

	for( int i = 0; i < MAX_REWARD_ICON; i++ )
	{
		SAFEDELETE( m_pRewardIcon[i] );
	}

	for( int i = 0; i < COUNT_DOWN_MAX; i++ )
	{
		SAFEDELETE( m_pCountDown[i] );
	}

	SAFEDELETE( m_pMainTitle );
	SAFEDELETE( m_pMissionTitle );
	SAFEDELETE( m_pRewardTitle );
	SAFEDELETE( m_pGameDisbled );
}

void	MatchingGameWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "MainTitle" )
	{
		SAFEDELETE( m_pMainTitle );
		m_pMainTitle = pFrame;
	}
	else if( szType == "MissionTitle" )
	{
		SAFEDELETE( m_pMissionTitle );
		m_pMissionTitle = pFrame;
	}
	else if( szType == "RewardTitle" )
	{
		SAFEDELETE( m_pRewardTitle );
		m_pRewardTitle = pFrame;
	}
	else if( szType == "GameDisabled" )
	{
		SAFEDELETE( m_pGameDisbled );
		m_pGameDisbled	= pFrame;
	}
	else
	{
		ioWnd::AddRenderFrame( szType, pFrame );
	}
}

void	MatchingGameWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	char szBuff[MAX_PATH];
	strcpy_s( szBuff, "CardImage" );

	if( szType == "Lucky1" )
	{
		SAFEDELETE( m_pLucky1Icon );
		m_pLucky1Icon	= pImage;
	}
	else if( szType == "Lucky2" )
	{
		SAFEDELETE( m_pLucky2Icon );
		m_pLucky2Icon	= pImage;
	}
	else if( szType == "NormalTicket" )
	{
		SAFEDELETE( m_pNormalTicket );
		m_pNormalTicket	= pImage;
	}
	else if( szType == "PremiumTicket" )
	{
		SAFEDELETE( m_pPremiumTicket );
		m_pPremiumTicket	= pImage;
	}
	else if( !strncmp( szBuff, szType.c_str(), 9 ) )
	{
		m_CardIconList.push_back( pImage );
	}
	else if( szType == "LuckyCard1" )
	{
		m_CardIconList.push_back( pImage );
	}
	else if( szType == "LuckyCard2" )
	{
		m_CardIconList.push_back( pImage );
	}
	else if( szType == "TimeOver" )
	{
		SAFEDELETE( m_pMissionEnd );
		m_pMissionEnd	= pImage;
	}
	else if( szType == "MissionCheck" )
	{
		SAFEDELETE( m_pMissionCheck );
		m_pMissionCheck	= pImage;
	}
	else if( szType == "MissionEnable" )
	{
		SAFEDELETE( m_pMissionEnable );
		m_pMissionEnable	= pImage;
	}
	else if( szType == "GaugeBarLeft" )
	{
		SAFEDELETE( m_pGaugeBarLeft );
		m_pGaugeBarLeft	= pImage;
	}
	else if( szType == "GaugeBarCenter" )
	{
		SAFEDELETE( m_pGaugeBarCenter );
		m_pGaugeBarCenter	= pImage;
	}
	else if( szType == "GaugeBarRight" )
	{
		SAFEDELETE( m_pGaugeBarRight );
		m_pGaugeBarRight	= pImage;
	}
	else if( szType == "ButtonImage" )
	{
		SAFEDELETE( m_pButtonImage );
		m_pButtonImage	= pImage;
	}
	else if( szType == "Complete" )
	{
		SAFEDELETE( m_pComplete );
		m_pComplete	= pImage;
	}
	else if( szType == "Count_3" )
	{
		SAFEDELETE( m_pCountDown[3] );
		m_pCountDown[3]	= pImage;
	}
	else if( szType == "Count_2" )
	{
		SAFEDELETE( m_pCountDown[2] );
		m_pCountDown[2]	= pImage;
	}
	else if( szType == "Count_1" )
	{
		SAFEDELETE( m_pCountDown[1] );
		m_pCountDown[1]	= pImage;
	}
	else if( szType == "Count_0" )
	{
		SAFEDELETE( m_pCountDown[0] );
		m_pCountDown[0]	= pImage;
	}
	else if( szType == "Count_Circle" )
	{
		SAFEDELETE( m_pCountCircle );
		m_pCountCircle	= pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void	MatchingGameWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	ioMatchingGame*	pMatchingGame	= g_MyInfo.GetMatchingGame();
	if( !pMatchingGame )
		return;

	char szBuf[MAX_PATH] = "";
	IoUIText*	pText;
	pText	= GetText( "Matching_Count" );
	if( pText )
	{
		wsprintf( szBuf, m_Text[TEXT_MATCHING_COUNT].c_str(), pMatchingGame->GetCardMatchCount() );
		pText->SetText( szBuf );
	}

	if( pMatchingGame->GetGameStatus() <= ioMatchingGame::MGS_GAME_PREPARE )
	{
		Reset();
	}
	else if( pMatchingGame->GetGameStatus() == ioMatchingGame::MGS_GAME_INIT )
	{
		ioButton*	pGameStart	= dynamic_cast<ioButton*>( FindChildWnd( ID_GAME_START ) );
		ioButton*	pPremiumStart	= dynamic_cast<ioButton*>( FindChildWnd( ID_PREMIUM_GAME_START ) );
		ioButton*	pConfirm	= dynamic_cast<ioButton*>( FindChildWnd( ID_CONFIRM ) );
		
		if( pConfirm )
			pConfirm->HideWnd();

		for( DWORD dwID = ID_CARD_SELECT1; dwID <= ID_CARD_SELECT30; dwID++ )
		{
			MatchingGameCardButton*	pButton	= dynamic_cast<MatchingGameCardButton*>( FindChildWnd( dwID ) );
			if( pButton )
				pButton->HideWnd();
		}

		if( !m_bCountDown )
		{
			if( pGameStart )
			{
				if( pMatchingGame->GetTicketCount() <= 0 )
					pGameStart->SetInActive();
				else
					pGameStart->SetActive();

				pGameStart->ShowWnd();
			}

			if( pPremiumStart )
			{
				if( pMatchingGame->GetTicketCount( true ) <= 0 )
					pPremiumStart->SetInActive();
				else
					pPremiumStart->SetActive();

				pPremiumStart->ShowWnd();
			}
		}
		else
		{
			if( pGameStart )
				pGameStart->HideWnd();

			if( pPremiumStart )
				pPremiumStart->HideWnd();

			if( REALGETTIME() - m_dwCountDownTime >= COUNT_DOWN_TIME && !m_bStartSend )
			{
				pMatchingGame->SendGameStart( m_bIsPremium );
				m_bStartSend	= true;

				return;
			}

			int	iCurArray	= COUNT_DOWN_MAX - ( ( REALGETTIME() - m_dwCountDownTime ) / 1000 ) - 1;
			if( iCurArray < 0 )
				iCurArray	= 0;

			if( m_iCurCountArray != iCurArray )
			{
				g_SoundMgr.PlaySound( m_szSoundCountDown[iCurArray] );
				m_iCurCountArray	= iCurArray;
			}
		}

		m_dwRemainedTime	= pMatchingGame->GetTotalTime();
		m_dwStartTime	= REALGETTIME();		
	}
	else if( pMatchingGame->GetGameStatus() == ioMatchingGame::MGS_GAME_CLOSING )
	{
		for( DWORD dwID = ID_CARD_SELECT1; dwID <= ID_CARD_SELECT30; dwID++ )
		{
			MatchingGameCardButton*	pButton	= dynamic_cast<MatchingGameCardButton*>( FindChildWnd( dwID ) );
			if( pButton && pButton->IsShow() )
				pButton->HideWnd();
		}

		ioButton*	pConfirm	= dynamic_cast<ioButton*>( FindChildWnd( ID_CONFIRM ) );

		if( pConfirm )
			pConfirm->ShowWnd();

		RewardEffect( fTimePerSec );

		if( !m_szSoundMatchingAllSuccess.IsEmpty() && m_bPlayAllSuccessSound )
		{
			m_uidAllSuccessSoundID	= g_SoundMgr.PlaySound( m_szSoundMatchingAllSuccess );
			m_bPlayAllSuccessSound	= false;
		}

		if( !m_szSoundGetReward.IsEmpty() && pMatchingGame->IsMissionComplete() && !g_SoundMgr.IsPlayingSound( m_szSoundMatchingAllSuccess, m_uidAllSuccessSoundID ) && m_bPlayMissionComplete )
		{
			g_SoundMgr.PlaySound( m_szSoundGetReward );
			m_bPlayMissionComplete	= false;
		}
	}
	else
	{
		ioButton*	pGameStart	= dynamic_cast<ioButton*>( FindChildWnd( ID_GAME_START ) );
		ioButton*	pPremiumStart	= dynamic_cast<ioButton*>( FindChildWnd( ID_PREMIUM_GAME_START ) );
		ioButton*	pConfirm	= dynamic_cast<ioButton*>( FindChildWnd( ID_CONFIRM ) );

		if( pGameStart )
			pGameStart->HideWnd();

		if( pPremiumStart )
			pPremiumStart->HideWnd();

		if( pConfirm )
			pConfirm->HideWnd();

		for( DWORD dwID = ID_CARD_SELECT1; dwID <= ID_CARD_SELECT30; dwID++ )
		{
			MatchingGameCardButton*	pButton	= dynamic_cast<MatchingGameCardButton*>( FindChildWnd( dwID ) );
			if( pButton && !pButton->IsShow() )
				pButton->ShowWnd();
		}

		if( REALGETTIME() - m_dwStartTime >= pMatchingGame->GetTotalTime() )
			m_dwRemainedTime	= 0;
		else
			m_dwRemainedTime	= pMatchingGame->GetTotalTime() - ( REALGETTIME() - m_dwStartTime );

		pText	= GetText( "Limit_Name" );
		if( pText )
		{
			wsprintf( szBuf, m_Text[TEXT_LIMIT_TIME].c_str(), m_dwRemainedTime / 1000, ( m_dwRemainedTime % 1000 ) / 10 );
			pText->SetText( szBuf );
		}

		m_bCountDown	= false;
	}
}

void	MatchingGameWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_Text[TEXT_MISSION_SUCCESS]	= xElement.GetStringAttribute_e( "MissionSuccess" );
	m_Text[TEXT_MISSION_FAILED]	= xElement.GetStringAttribute_e( "MissionFail" );
	m_Text[TEXT_MISSION_DESCRIPTION]	= xElement.GetStringAttribute_e( "MissionDescription" );
	m_Text[TEXT_MISSION_REWARD]	= xElement.GetStringAttribute_e( "MissionReward" );
	m_Text[TEXT_LIMIT_TIME]	= xElement.GetStringAttribute_e( "LimitTime" );
	m_Text[TEXT_NORMAL_TICKET]	= xElement.GetStringAttribute_e( "NormalTicket" );
	m_Text[TEXT_PREMIUM_TICKET]	= xElement.GetStringAttribute_e( "PremiumTicket" );
	m_Text[TEXT_MATCHING_COUNT]	= xElement.GetStringAttribute_e( "MatchingCount" );
	m_Text[TEXT_GAME_EXIT_WARNING]	= xElement.GetStringAttribute_e( "GameExitWarning" );

	m_dwRewardEffectTime = xElement.GetIntAttribute_e( "Effect_Gap_Time" );
	m_vGameEndEffect.clear();
	int iMax = xElement.GetIntAttribute_e( "Max_Result_Effect" );
	m_vGameEndEffect.reserve( iMax );
	char szName[MAX_PATH]="";
	for (int i = 0; i < iMax ; i++)
	{
		StringCbPrintf_e( szName, sizeof( szName ), "Result_Effect_%d", i+1 );
		ioHashString szEffect = xElement.GetStringAttribute( szName );
		if( szEffect == "" )
			break;
		m_vGameEndEffect.push_back( szEffect );
	}

	m_szSoundMatchingSuccess	= xElement.GetStringAttribute_e( "SoundMatchingSuccess" );
	m_szSoundMatchingAllSuccess	= xElement.GetStringAttribute_e( "SoundMatchingAllSuccess" );
	m_szSoundGetReward			= xElement.GetStringAttribute_e( "SoundGetReward" );
	for( int i = 0; i < COUNT_DOWN_MAX; i++ )
	{
		StringCbPrintf_e( szName, sizeof( szName ), "SoundCountDown%d", i );
		m_szSoundCountDown[i]	= xElement.GetStringAttribute( szName );
	}
}

void	MatchingGameWnd::iwm_show()
{
	Reset();
}

void	MatchingGameWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	if( pWnd == dynamic_cast<ioMessageBox*>( g_GUIMgr.FindWnd( MESSAGE_BOX ) ) )
	{
		if( param == IDYES && cmd == IOBN_BTNUP )
		{
			ioMatchingGame* matchingGame = g_MyInfo.GetMatchingGame();
			if( matchingGame )
			{
				matchingGame->SendEndGame();
				HideWnd();
			}
		}
	}

	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_GAME_START:
		{
			if( cmd == IOBN_BTNUP )
			{
				m_bIsPremium	= false;
				m_bCountDown	= true;
				m_dwCountDownTime	= REALGETTIME();
			}
		}
		break;

	case ID_PREMIUM_GAME_START:
		{
			if( cmd == IOBN_BTNUP )
			{
				m_bIsPremium	= true;
				m_bCountDown	= true;
				m_dwCountDownTime	= REALGETTIME();
			}
		}
		break;

	case ID_EXIT:
		{
			if( cmd == IOBN_BTNUP )
			{
				g_GUIMgr.SetMsgBox( MB_YESNO, this, m_Text[TEXT_GAME_EXIT_WARNING].c_str() );
			}
		}
		break;

	case ID_CONFIRM:
		{
			if( cmd == IOBN_BTNUP )
			{
				ioMatchingGame* matchingGame = g_MyInfo.GetMatchingGame();
				if( matchingGame )
				{
					matchingGame->SendOpenGame();
				}
			}
		}
		break;

	default:
		{
			if( COMPARE( dwID, ID_CARD_SELECT1, ID_CARD_SELECT30 + 1 ) )
			{
				if( cmd == IOBN_BTNUP )
				{
					ioMatchingGame* pMatchingGame	= g_MyInfo.GetMatchingGame();
					if( pMatchingGame )
					{
						if( pMatchingGame->GetGameStatus() == ioMatchingGame::MGS_GAME_PLAYING && pMatchingGame->GetCellStatus( dwID - ID_CARD_SELECT1 ) != ioMatchingGame::CARD_MATCHED_DONE )
						{
							MatchingGameCardButton*	pButton	= dynamic_cast<MatchingGameCardButton*>( FindChildWnd( dwID ) );

							if( m_pSecondButton == NULL && m_pFirstButton != pButton )
							{
								if( pMatchingGame->SelectCell( dwID - ID_CARD_SELECT1 ) )
								{
									if( pButton )
									{
										pButton->SetStatus( MatchingGameCardButton::MGCBS_OPENING );
									}
								}
							}
						}
					}
				}
			}
		}
		break;
	}
}

bool	MatchingGameWnd::iwm_esc()
{
	if( HasWndStyle( IWS_EXIT_ESC ) )
	{
		g_GUIMgr.SetMsgBox( MB_YESNO, this, m_Text[TEXT_GAME_EXIT_WARNING].c_str() );

		return true;
	}
	return false;
}

void	MatchingGameWnd::OnRender()
{
	enum
	{
		LUCK_ICON_OFFSET_X	= 75
	};

	ioWnd::OnRender();

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	if( m_pMainTitle )
		m_pMainTitle->Render( iXPos, iYPos, UI_RENDER_MULTIPLY );
	if( m_pMissionTitle )
		m_pMissionTitle->Render( iXPos, iYPos, UI_RENDER_MULTIPLY );
	if( m_pRewardTitle )
		m_pRewardTitle->Render( iXPos, iYPos, UI_RENDER_MULTIPLY );

	ioMatchingGame*	pMatchingGame	= g_MyInfo.GetMatchingGame();
	if( pMatchingGame )
	{
		ioMatchingGame::MissionInfo*	pMissionInfo	= pMatchingGame->GetMissionInfo();
		if( pMissionInfo )
		{
			int	iTotalCount	= 0;
			if( pMissionInfo->m_iMissionType == ioMatchingGame::MISSION_TYPE_3 )
			{
				iTotalCount	= ioMatchingGame::MISSION_COUNT_TYPE_3;
			}
			else if( pMissionInfo->m_iMissionType == ioMatchingGame::MISSION_TYPE_4 )
			{
				iTotalCount	= ioMatchingGame::MISSION_COUNT_TYPE_4;
			}

			int	iCount	= 0;
			
			if( iTotalCount == ioMatchingGame::MISSION_COUNT_TYPE_4 )
			{
				for( ; iCount < pMissionInfo->m_iLucky1Node; iCount++ )
				{
					m_pLucky1Icon->Render( iXPos + 482.0f + ( LUCK_ICON_OFFSET_X * iCount ), iYPos + 123.0f );
				}
				for( ; iCount < iTotalCount; iCount++ )
				{
					m_pLucky2Icon->Render( iXPos + 482.0f + ( LUCK_ICON_OFFSET_X * iCount ), iYPos + 123.0f );
				}

				for( int i = 0; i < pMissionInfo->m_iLucky1CompletedNode; i++ )
				{
					m_pMissionCheck->Render( iXPos + 482.0f + ( LUCK_ICON_OFFSET_X * i ), iYPos + 123.0f );
				}
				for( int i = pMissionInfo->m_iLucky1Node; i < pMissionInfo->m_iLucky1Node + pMissionInfo->m_iLucky2CompletedNode; i++ )
				{
					m_pMissionCheck->Render( iXPos + 482.0f + ( LUCK_ICON_OFFSET_X * i ), iYPos + 123.0f );
				}
			}
			else
			{
				for( ; iCount < pMissionInfo->m_iLucky1Node; iCount++ )
				{
					m_pLucky1Icon->Render( iXPos + 520.0f + ( LUCK_ICON_OFFSET_X * iCount ), iYPos + 123.0f );
				}
				for( ; iCount < iTotalCount; iCount++ )
				{
					m_pLucky2Icon->Render( iXPos + 520.0f + ( LUCK_ICON_OFFSET_X * iCount ), iYPos + 123.0f );
				}

				for( int i = 0; i < pMissionInfo->m_iLucky1CompletedNode; i++ )
				{
					m_pMissionCheck->Render( iXPos + 520.0f + ( LUCK_ICON_OFFSET_X * i ), iYPos + 123.0f );
				}
				for( int i = pMissionInfo->m_iLucky1Node; i < pMissionInfo->m_iLucky1Node + pMissionInfo->m_iLucky2CompletedNode; i++ )
				{
					m_pMissionCheck->Render( iXPos + 520.0f + ( LUCK_ICON_OFFSET_X * i ), iYPos + 123.0f );
				}
			}

			if( m_pMissionRewardIcon )
			{
				m_pMissionRewardIcon->Render( iXPos + 490.0f, iYPos + 207.0f );
			}
		}

		enum
		{
			GAUGE_EDGE_SIZE	= 9
		};

		int	iCardCount	= pMatchingGame->GetCardMatchCount();
		int	iGaugeSize	= Gauge_Max_Length / Gauge_Max_Count * iCardCount;
		int	iGaugeCenterSize	= iGaugeSize - ( GAUGE_EDGE_SIZE * 2 );

		if( m_pGaugeBarLeft && m_pGaugeBarCenter && m_pGaugeBarRight && iGaugeSize > 0 )
		{
			m_pGaugeBarLeft->Render( iXPos + 455.0f, iYPos + 321.0f );
			m_pGaugeBarRight->Render( iXPos + 455.0f + iGaugeSize - GAUGE_EDGE_SIZE, iYPos + 321.0f );
			if( iGaugeCenterSize > 0 )
			{
				m_pGaugeBarCenter->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
				m_pGaugeBarCenter->SetSize( iGaugeCenterSize, 22 );
				m_pGaugeBarCenter->Render( iXPos + 455.0f + GAUGE_EDGE_SIZE, iYPos + 321.0f );
			}
		}

		m_pRewardIcon[0]->SetScale( 0.6f, 0.6f );
		m_pRewardIcon[0]->Render( iXPos + 504.0f, iYPos + 330.0f, UI_RENDER_NORMAL, TFO_BILINEAR );
		m_pRewardIcon[1]->SetScale( 0.6f, 0.6f );
		m_pRewardIcon[1]->Render( iXPos + 598.0f, iYPos + 330.0f, UI_RENDER_NORMAL, TFO_BILINEAR );
		m_pRewardIcon[2]->SetScale( 0.6f, 0.6f );
		m_pRewardIcon[2]->Render( iXPos + 710.0f, iYPos + 330.0f, UI_RENDER_NORMAL, TFO_BILINEAR );

		if( iCardCount == CARD_MATCH_COUNT_CHECK_3 )
		{
			m_pMissionCheck->Render( iXPos + 503.0f, iYPos + 327.0f );
			m_pMissionCheck->Render( iXPos + 597.0f, iYPos + 327.0f );
			m_pMissionCheck->Render( iXPos + 709.0f, iYPos + 327.0f );
		}
		else if( iCardCount >= CARD_MATCH_COUNT_CHECK_2 )
		{
			m_pMissionCheck->Render( iXPos + 503.0f, iYPos + 327.0f );
			m_pMissionCheck->Render( iXPos + 597.0f, iYPos + 327.0f );
			m_pMissionEnable->Render( iXPos + 709.0f, iYPos + 327.0f );
		}
		else if( iCardCount >= CARD_MATCH_COUNT_CHECK_1 )
		{
			m_pMissionCheck->Render( iXPos + 503.0f, iYPos + 327.0f );
			m_pMissionEnable->Render( iXPos + 597.0f, iYPos + 327.0f );
		}
		else
		{
			m_pMissionEnable->Render( iXPos + 503.0f, iYPos + 327.0f );
		}

		if( pMatchingGame->GetGameStatus() <= ioMatchingGame::MGS_GAME_INIT )
		{
			if( !m_bCountDown )
			{
				for( int i = 0; i < MAX_NUMOF_BUTTON; i++ )
				{
					m_pButtonImage->SetScale( 0.81f, 0.81f );
					m_pButtonImage->Render( iXPos + 56.0f + ( 64.0f * ( i % 6 ) ), iYPos + 115.0f + ( 60.0f * ( i / 6 ) ), UI_RENDER_NORMAL, TFO_BILINEAR );
				}

				//m_pGameDisbled->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
				//m_pGameDisbled->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
				m_pGameDisbled->SetSize( 408, 344 );
				//m_pGameDisbled->SetAlpha( 180 );
				m_pGameDisbled->Render( iXPos + 12.0f, iYPos + 49.0f, UI_RENDER_NORMAL );

				m_pNormalTicket->Render( iXPos + 132.0f, iYPos + 183.0f );
				m_pPremiumTicket->Render( iXPos + 304.0f, iYPos + 183.0f );

				char szBuf[MAX_PATH] = "";
				wsprintf( szBuf, m_Text[TEXT_NORMAL_TICKET].c_str(), pMatchingGame->GetTicketCount() );
				m_Printer[PRINTER_NORMAL_TICKET].ClearList();
				m_Printer[PRINTER_NORMAL_TICKET].SetTextStyle( TS_NORMAL );
				m_Printer[PRINTER_NORMAL_TICKET].SetTextColor( TCT_DEFAULT_WHITE );
				m_Printer[PRINTER_NORMAL_TICKET].SetBkColor( 51, 51, 51 );
				m_Printer[PRINTER_NORMAL_TICKET].SetTextStyle( TS_OUTLINE_2X );
				m_Printer[PRINTER_NORMAL_TICKET].AddTextPiece( FONT_SIZE_14, szBuf );
				m_Printer[PRINTER_NORMAL_TICKET].PrintFullText( iXPos + 131.0f, iYPos + 226.0f, TAT_CENTER );

				wsprintf( szBuf, m_Text[TEXT_PREMIUM_TICKET].c_str(), pMatchingGame->GetTicketCount( true ) );
				m_Printer[PRINTER_PREMIUM_TICKET].ClearList();
				m_Printer[PRINTER_PREMIUM_TICKET].SetTextStyle( TS_NORMAL );
				m_Printer[PRINTER_PREMIUM_TICKET].SetTextColor( TCT_DEFAULT_WHITE );
				m_Printer[PRINTER_PREMIUM_TICKET].SetBkColor( 51, 51, 51 );
				m_Printer[PRINTER_PREMIUM_TICKET].SetTextStyle( TS_OUTLINE_2X );
				m_Printer[PRINTER_PREMIUM_TICKET].AddTextPiece( FONT_SIZE_14, szBuf );
				m_Printer[PRINTER_PREMIUM_TICKET].PrintFullText( iXPos + 301.0f, iYPos + 226.0f, TAT_CENTER );
			}
			else
			{
				for( int i = 0; i < MAX_NUMOF_BUTTON; i++ )
				{
					m_pButtonImage->SetScale( 0.81f, 0.81f );
					m_pButtonImage->Render( iXPos + 56.0f + ( 64.0f * ( i % 6 ) ), iYPos + 115.0f + ( 60.0f * ( i / 6 ) ), UI_RENDER_NORMAL, TFO_BILINEAR );
				}

				if( REALGETTIME() - m_dwCountDownTime <= COUNT_DOWN_TIME )
				{
					float fNumCurScale = cosf( (D3DX_PI/2) *  ( (float)( ( REALGETTIME() - m_dwCountDownTime ) % 1000 ) / 500 ) );

					if( fNumCurScale <= 0.5f )
					{
						if( m_pCountCircle )
						{
							m_pCountCircle->SetScale( ( fNumCurScale - 0.5f ) * -1.0f );
							m_pCountCircle->Render( iXPos + 217.0f, iYPos + 186.0f, UI_RENDER_NORMAL, TFO_BILINEAR );
						}

						if( fNumCurScale <= 0.0f )
						{
							if( m_pCountCircle )
							{
								m_pCountCircle->SetScale( ( fNumCurScale * -1.5f ) );
								m_pCountCircle->Render( iXPos + 217.0f, iYPos + 186.0f, UI_RENDER_NORMAL, TFO_BILINEAR );

							}
						}

						fNumCurScale	= 0.5f;
					}

					if( m_pCountDown[m_iCurCountArray] )
					{
						m_pCountDown[m_iCurCountArray]->SetScale( fNumCurScale );
						m_pCountDown[m_iCurCountArray]->Render( iXPos + 217.0f, iYPos + 186.0f, UI_RENDER_NORMAL, TFO_BILINEAR );
					}
				}
			}
		}
		else if( pMatchingGame->GetGameStatus() == ioMatchingGame::MGS_GAME_CLOSING )
		{
			for( int i = 0; i < MAX_NUMOF_BUTTON; i++ )
			{
				m_pButtonImage->SetScale( 0.81f, 0.81f );
				m_pButtonImage->Render( iXPos + 56.0f + ( 64.0f * ( i % 6 ) ), iYPos + 115.0f + ( 60.0f * ( i / 6 ) ), UI_RENDER_NORMAL, TFO_BILINEAR );
			}

			//m_pGameDisbled->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
			//m_pGameDisbled->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
			m_pGameDisbled->SetSize( 408, 344 );
			//m_pGameDisbled->SetAlpha( 180 );
			m_pGameDisbled->Render( iXPos + 12.0f, iYPos + 49.0f, UI_RENDER_NORMAL );

			m_pMissionEnd->Render( iXPos + 217.0f, iYPos + 122.0f );

			if( iCardCount == CARD_MATCH_COUNT_CHECK_3 )
			{
				m_pRewardIcon[2]->SetScale( 1.0f, 1.0f );
				m_pRewardIcon[2]->Render( iXPos + 217.0f, iYPos + 186.0f );
			}
			else if( iCardCount < CARD_MATCH_COUNT_CHECK_3 && iCardCount >= CARD_MATCH_COUNT_CHECK_2 )
			{
				m_pRewardIcon[1]->SetScale( 1.0f, 1.0f );
				m_pRewardIcon[1]->Render( iXPos + 217.0f, iYPos + 186.0f );
			}
			else if( iCardCount >= CARD_MATCH_COUNT_CHECK_1 )
			{
				m_pRewardIcon[0]->SetScale( 1.0f, 1.0f );
				m_pRewardIcon[0]->Render( iXPos + 217.0f, iYPos + 186.0f );
			}
			else
			{
				m_pRewardIcon[0]->SetScale( 1.0f, 1.0f );
				m_pRewardIcon[0]->Render( iXPos + 217.0f, iYPos + 186.0f, UI_RENDER_GRAY );
			}

			if( pMatchingGame->IsMissionComplete() && !m_bPlayMissionComplete )
			{
				if( m_pComplete )
					m_pComplete->Render( iXPos + 605.0f, iYPos + 202.0f );
			}

			m_Printer[PRINTER_SUCCESS_FAIL].ClearList();
			m_Printer[PRINTER_SUCCESS_FAIL].SetTextStyle( TS_NORMAL );
			m_Printer[PRINTER_SUCCESS_FAIL].SetTextColor( TCT_DEFAULT_WHITE );
			m_Printer[PRINTER_SUCCESS_FAIL].SetBkColor( 51, 51, 51 );
			m_Printer[PRINTER_SUCCESS_FAIL].SetTextStyle( TS_OUTLINE );
			if( pMatchingGame->GetCardMatchCount() >= CARD_MATCH_COUNT_CHECK_1 )
				m_Printer[PRINTER_SUCCESS_FAIL].AddTextPiece( FONT_SIZE_14, m_Text[TEXT_MISSION_SUCCESS].c_str() );
			else
				m_Printer[PRINTER_SUCCESS_FAIL].AddTextPiece( FONT_SIZE_14, m_Text[TEXT_MISSION_FAILED].c_str() );
			m_Printer[PRINTER_SUCCESS_FAIL].PrintFullText( iXPos + 217.0f, iYPos + 233.0f, TAT_CENTER );

			m_Printer[PRINTER_DESCRIPTION].ClearList();
			m_Printer[PRINTER_DESCRIPTION].SetTextStyle( TS_NORMAL );
			m_Printer[PRINTER_DESCRIPTION].SetTextColor( TCT_DEFAULT_GRAY3 );
			m_Printer[PRINTER_DESCRIPTION].SetBkColor( 51, 51, 51 );
			m_Printer[PRINTER_DESCRIPTION].SetTextStyle( TS_OUTLINE );
			m_Printer[PRINTER_DESCRIPTION].AddTextPiece( FONT_SIZE_11, m_Text[TEXT_MISSION_DESCRIPTION].c_str() );
			m_Printer[PRINTER_DESCRIPTION].PrintFullText( iXPos + 217.0f, iYPos + 336.0f, TAT_CENTER );
		}
	}
}

void	MatchingGameWnd::OnRenderAfterChild()
{
	ioWnd::OnRenderAfterChild();

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	if( m_pEffect )
	{
		m_pEffect->Render( iXPos + 221.0f, iYPos + 188.0f, UI_RENDER_ADD );
	}
}

void	MatchingGameWnd::Reset()
{
	for( DWORD dwID = ID_CARD_SELECT1; dwID <= ID_CARD_SELECT30; dwID++ )
	{
		MatchingGameCardButton*	pButton	= dynamic_cast<MatchingGameCardButton*>( FindChildWnd( dwID ) );
		if( pButton )
			pButton->Reset();
	}

	m_pFirstButton	= NULL;
	m_pSecondButton	= NULL;

	m_iCurEffectArray	= -1;
	m_dwRewardEffectCheckTime	= 0;

	m_bCountDown	= false;
	m_iCurCountArray	= 0;

	m_bStartSend	= false;

	m_bPlayAllSuccessSound	= true;
	m_bPlayMissionComplete	= true;

	ioMatchingGame*	pMatchingGame	= g_MyInfo.GetMatchingGame();
	if( pMatchingGame )
	{
		ioMatchingGame::MissionInfo*	pMissionInfo	= pMatchingGame->GetMissionInfo();

		if( pMissionInfo )
		{
			SAFEDELETE( m_pMissionRewardIcon );
			m_pMissionRewardIcon = g_UIImageSetMgr.CreateImageByFullName( pMissionInfo->m_szRewardInfo );

			IoUIText*	pText;
			char	szBuf[MAX_PATH];
			pText	= GetText( "Mission_Reward" );
			if( pText )
			{
				wsprintf( szBuf, m_Text[TEXT_MISSION_REWARD].c_str(), pMissionInfo->m_szRewardName.c_str() );
				pText->SetText( szBuf );
			}
		}

		for( int i = 0; i < MAX_REWARD_ICON; i++ )
		{
			SAFEDELETE( m_pRewardIcon[i] );

			m_pRewardIcon[i]	= g_UIImageSetMgr.CreateImageByFullName( pMatchingGame->GetRewardIconName( i ) );
		}
	}

	char	szBuf[MAX_PATH];
	IoUIText*	pText	= GetText( "Limit_Name" );
	if( pText )
	{
		wsprintf( szBuf, m_Text[TEXT_LIMIT_TIME].c_str(), 0, 0 );
		pText->SetText( szBuf );
	}
}

void	MatchingGameWnd::RewardEffect( float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();

	int iEffectListCnt = m_vGameEndEffect.size();
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

	/*if( m_iCurEffectArray == 0 )
	{
		if( !m_szResultSound.IsEmpty() )
			g_SoundMgr.PlaySound( m_szResultSound );
	}*/

	if( m_dwRewardEffectCheckTime == 0 || dwCurTime - m_dwRewardEffectCheckTime > m_dwRewardEffectTime )		// 교체
	{
		SAFEDELETE( m_pEffect );

		m_iCurEffectArray++;

		if( !COMPARE( m_iCurEffectArray, 0, iEffectListCnt ) )
			m_iCurEffectArray = 0;

		ioHashString szEffect = m_vGameEndEffect[m_iCurEffectArray];

		if( !szEffect.IsEmpty() )
		{
			m_pEffect = g_UIImageSetMgr.CreateImageByFullName( szEffect );
		}

		m_dwRewardEffectCheckTime = dwCurTime;
	}
}

ioUIRenderImage*	MatchingGameWnd::GetCardImage( int iCardNum )
{
	ioUIRenderImage*	pImage	= NULL;

	if( COMPARE( iCardNum, 0, MAX_NUMOF_ICON ) )
		pImage	= m_CardIconList[iCardNum];

	return	pImage;
}

MatchingGameCardButton*	MatchingGameWnd::GetButton( int iButton )
{
	MatchingGameCardButton*	pButton	= NULL;

	if( iButton >= 1 && iButton <= MAX_NUMOF_BUTTON )
		pButton	= dynamic_cast<MatchingGameCardButton*>( FindChildWnd( iButton + ID_CARD_SELECT1 ) );

	return	pButton;
}

void	MatchingGameWnd::SetButtonActivate(  MatchingGameCardButton* pButton )
{
	if( m_pFirstButton == NULL )
		m_pFirstButton	= pButton;
	else if( m_pSecondButton == NULL )
		m_pSecondButton	= pButton;
}

void	MatchingGameWnd::ResetButtonActivate( bool bIsFirst )
{
	if( bIsFirst )
		m_pFirstButton	= NULL;
	else
		m_pSecondButton	= NULL;
}

MatchingGameCardButton*	MatchingGameWnd::GetActivatedButton( bool bIsFirst /* = true */ )
{
	if( bIsFirst )
		return	m_pFirstButton;
	else
		return	m_pSecondButton;
}