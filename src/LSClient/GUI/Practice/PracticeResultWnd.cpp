#include <stdafx.h>
#include "PracticeResultWnd.h"
#include "../../GameStage/Mode/Practice/IoPracticeManager.h"
#include "../../DataHeaders/LSC_PresentReward_info.h"

PracticeResultWnd::PracticeResultWnd()
{
	m_pClear = NULL;
	m_pFail = NULL;
	m_pStamp = NULL;	
	m_pBackGround = NULL;
	m_pStarYellow = NULL;
	m_pStarGray = NULL;
	m_pItemIconBg = NULL;
	m_pRewardIcon = NULL;

	m_iResultStar = 0;
	
	m_dwStarTime = 0;
	m_dwDelay = 0;
	m_dwSpeed = 0;
	m_iSoundCount = 0;
}

PracticeResultWnd::~PracticeResultWnd()
{
	SAFEDELETE(m_pBackGround);
	SAFEDELETE(m_pStarYellow);
	SAFEDELETE(m_pStarGray);
	SAFEDELETE(m_pClear);
	SAFEDELETE(m_pFail);
	SAFEDELETE(m_pItemIconBg);
	SAFEDELETE(m_pStamp);
	ClearIconMap();
}

void PracticeResultWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_szRetryBtn = xElement.GetStringAttribute("RetryBtn");
	m_szExitBtn = xElement.GetStringAttribute("ExitBtn");
	m_dwDelay = xElement.GetIntAttribute_e("Delay");
	m_dwSpeed = xElement.GetIntAttribute_e("Speed");

	char szBuf[MAX_PATH]="";
	for( int i = 0; i < 3; ++i )
	{
		wsprintf( szBuf, "TextInfo%d", i+1);
		m_szRewardHelp[i] = xElement.GetStringAttribute(szBuf);
	}
	m_szSuccessSound = xElement.GetStringAttribute_e("SuccessSound");
	m_szFailSound = xElement.GetStringAttribute_e("FailSound");
	m_szStarSound = xElement.GetStringAttribute_e("StarSound");
}

void PracticeResultWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "Clear")
	{
		SAFEDELETE( m_pClear );
		m_pClear = pImage;
	}
	else if( szType == "Fail")
	{
		SAFEDELETE( m_pFail );
		m_pFail = pImage;
	}
	else if( szType == "Stamp")
	{
		SAFEDELETE( m_pStamp );
		m_pStamp = pImage;
	}
	else if( szType == "Bg")
	{
		SAFEDELETE(m_pBackGround);
		m_pBackGround = pImage;
	}
	else if( szType == "StarYellow")
	{
		SAFEDELETE(m_pStarYellow);
		m_pStarYellow = pImage;
	}
	else if( szType == "StarGray")
	{
		SAFEDELETE(m_pStarGray);
		m_pStarGray = pImage;
	}
	else if( szType == "ItemIconBg")
	{
		SAFEDELETE(m_pItemIconBg);
		m_pItemIconBg = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void PracticeResultWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;
	ioWnd::OnProcess( fTimePerSec );
	ioButton* pExitBtn = (ioButton*)FindChildWnd( ID_EXIT );
	if( pExitBtn )
	{
		char szReamin[MAX_PATH] = {0,};
		DWORD dwReaminTime = 0;
		DWORD dwCurTime = FRAMEGETTIME();
		if( ( m_dwStarTime + 30000 ) < dwCurTime )
		{
			SP2Packet kPacket( CTPK_EXIT_ROOM );
			if( TCPNetwork::CheckSendToServer( kPacket ) )
			{
				TCPNetwork::MouseBusy( true );
			}
		}
		else
			dwReaminTime = (m_dwStarTime + 30000) - dwCurTime;

		int iReaminTime = (int)dwReaminTime / 1000;
		sprintf( szReamin, "%s...%d (ESC)", m_szExitBtn.c_str(), iReaminTime );
		pExitBtn->SetTitleText(szReamin);
	}



}

void PracticeResultWnd::OnRender()
{
	int iPosX = GetDerivedPosX();
	int iPosY = GetDerivedPosY();

	ioWnd::OnRender();

	if(m_pBackGround)
	{
		m_pBackGround->SetScale(1.6f, 1.1f);
		m_pBackGround->Render( iPosX, iPosY );
	}

	if(m_pStarGray)
	{
		m_pStarGray->Render( iPosX + 129, iPosY + 102 );
		m_pStarGray->Render( iPosX + 245, iPosY + 102 );
		m_pStarGray->Render( iPosX + 187, iPosY + 97 );
	}

	if(m_iResultStar > 0)
	{
		if(m_pStarYellow)
		{
			float fScale = GetStarScale( m_dwStarTime + m_dwDelay );
			m_pStarYellow->SetScale( fScale );
			m_pStarYellow->Render( iPosX + 187, iPosY + 97, UI_RENDER_NORMAL, TFO_BILINEAR );		

			if( fScale > 0.0f && fScale <= 1.1f && m_iResultStar - m_iSoundCount == 0)
			{
				g_SoundMgr.PlaySound( m_szStarSound );
				--m_iSoundCount;
			}
		}
	}

	if(m_iResultStar > 1)
	{
		if(m_pStarYellow)
		{
			float fScale = GetStarScale( m_dwStarTime + m_dwDelay*2 );			
			m_pStarYellow->SetScale( fScale );
			m_pStarYellow->Render( iPosX + 129, iPosY + 102, UI_RENDER_NORMAL, TFO_BILINEAR );

			if( fScale > 0.0f && fScale <= 1.1f && m_iResultStar - m_iSoundCount == 1)
			{
				g_SoundMgr.PlaySound( m_szStarSound );
				--m_iSoundCount;
			}
		}
	}

	if(m_iResultStar >2)
	{
		if(m_pStarYellow)
		{
			float fScale = GetStarScale( m_dwStarTime + m_dwDelay*3 );
			m_pStarYellow->SetScale( fScale );
			m_pStarYellow->Render( iPosX + 245, iPosY + 102, UI_RENDER_NORMAL, TFO_BILINEAR );

			if( fScale > 0.0f && fScale <= 1.1f && m_iResultStar - m_iSoundCount == 2)
			{
				g_SoundMgr.PlaySound( m_szStarSound );
				--m_iSoundCount;
			}
		}
	}


	
	if(m_iResultStar > 0)
	{
		if(m_pClear)
		{
			float fScale = GetStarScale( m_dwStarTime + m_dwDelay*(m_iResultStar+1) );

			m_pClear->SetScale( fScale );
			m_pClear->Render( iPosX, iPosY, UI_RENDER_NORMAL, TFO_BILINEAR );
		}
	}
	else
	{
		if(m_pFail)
		{
			float fScale = GetStarScale( m_dwStarTime + m_dwDelay );

			m_pFail->SetScale( fScale );
			m_pFail->Render( iPosX, iPosY, UI_RENDER_NORMAL, TFO_BILINEAR );
		}
	}
	
	RenderReward( iPosX+17, iPosY+195 );
	char szBuf[MAX_PATH] = {0,};	
	IoString str;
	if(m_iResultStar > 0)
		str.Set(0, 0, m_szRewardHelp[1].c_str(), FONT_SIZE_14, 300, TS_NORMAL, TCT_DEFAULT_BLUE, 0, TAT_CENTER, TVA_TOP, 0);
	else
		str.Set(0, 0, m_szRewardHelp[2].c_str(), FONT_SIZE_14, 300, TS_NORMAL, TCT_DEFAULT_RED, 0, TAT_CENTER, TVA_TOP, 0);
	str.OnRender( iPosX + 187, iPosY + 344 );	
	
}

float PracticeResultWnd::GetStarScale( float dwTime )
{
	if( FRAMEGETTIME() < dwTime )
		return 0.f;

	int iGapTime = FRAMEGETTIME() - dwTime;

	float fRate = 1.0f;
	int iAlphaRate = MAX_ALPHA_RATE;
	fRate = Help::GetAniRateByTime( iGapTime, m_dwSpeed, Help::RATE_NORMAL );
	float fScaleRate = 5.f * ( 1.f - fRate );

	return max( 1.f, fScaleRate );
}

void PracticeResultWnd::iwm_show()
{
	g_GUIMgr.HideWnd( MINIMAP_WND );
	g_GUIMgr.HideWnd( TIME_SCORE_WND );
	g_GUIMgr.HideWnd( SKILL_LIST_WND );
	g_GUIMgr.HideWnd( PRACTICE_INFO_WND );
	g_GUIMgr.HideWnd( COUNT_UP_WND );
	g_GUIMgr.HideWnd( RETRY_MODE_WND );
	//g_GUIMgr.HideWnd( MINI_SOLDIER_SELECT_WND );

	SetRewardIconName();

	m_dwStarTime = FRAMEGETTIME();

	ioButton* pRetryBtn = (ioButton*)FindChildWnd( ID_RETRY );
	if( pRetryBtn )
	{
		char szRetry[MAX_PATH] = {0,};
		sprintf( szRetry, "%s (SPACE)", m_szRetryBtn.c_str() );
		pRetryBtn->SetTitleText(szRetry);
	}	
}

void PracticeResultWnd::iwm_hide()
{
	StopSound();
}

void PracticeResultWnd::iwm_command(ioWnd *pWnd, int cmd, DWORD param)
{
	if(!pWnd) return;

	DWORD dwID = pWnd->GetID();


	switch( dwID )
	{
	case ID_RETRY:
		{
			if( cmd == IOBN_BTNUP )
			{
				int iIndex = g_PracticeMgr.GetPreIndex();
				SP2Packet kPacket( CTPK_PRACTICE_ENTER );
				kPacket << g_PracticeMgr.GetPracticeMap( iIndex );
				kPacket << iIndex;
				TCPNetwork::SendToServer( kPacket );


			}
		}
		break;
	case ID_OUT:
	case ID_EXIT:
		{
			if( cmd == IOBN_BTNUP )
			{
				SP2Packet kPacket( CTPK_EXIT_ROOM );
				kPacket << EXIT_ROOM_LOBBY << 0 << true;
				TCPNetwork::SendToServer( kPacket );
					//TCPNetwork::MouseBusy( true );
			}
		}
		break;
	}
	
}

bool PracticeResultWnd::iwm_spacebar()
{
	if( !IsShow() )
		return false;;

	if( g_App.IsMouseBusy() )
		return false;

	int iIndex = g_PracticeMgr.GetPreIndex();
	SP2Packet kPacket( CTPK_PRACTICE_ENTER );
	kPacket << g_PracticeMgr.GetPracticeMap( iIndex );
	kPacket << iIndex;
	TCPNetwork::SendToServer( kPacket );

	return true;
}

bool PracticeResultWnd::iwm_esc()
{
	if( !IsShow() )
		return false;
	
	if( g_App.IsMouseBusy() )
		return false;

	SP2Packet kPacket( CTPK_EXIT_ROOM );
	kPacket << EXIT_ROOM_LOBBY << 0 << true;
	TCPNetwork::SendToServer( kPacket );

	return true;
}

void PracticeResultWnd::SetResult( int iPracticeIndex, int iResultStar, DWORDVec &vRewardList )
{ 
	m_iResultStar = iResultStar;
	m_iSoundCount = iResultStar;

	DWORD dwGrade = g_PracticeMgr.GetPracticeRecord( iPracticeIndex );
	int iSize = vRewardList.size();

	if( m_iResultStar == 0 )
		g_SoundMgr.PlaySound( m_szFailSound );
	else
		g_SoundMgr.PlaySound( m_szSuccessSound );

	IoUIText *pText = NULL;
	char cKey[MAX_PATH] = {0,};

	sprintf_s( cKey, "%s", "RewardText" );
	pText = GetText( cKey );

	if( !pText )
		return;

	if( ioPracticeManager::E_GRADE_A == dwGrade )
	{
		if( iSize == 0 )
		{
			pText->SetText( m_szAlreadyReward.c_str() );
			m_RewardString.SetByUIText( pText, 0, -m_iYPosGap );

			ioWnd *pWnd = FindChildWnd( ID_RETRY );
			if( pWnd )
				pWnd->SetWndPos( pWnd->GetXPos(), m_iButtonYpos - m_iYPosGap );
			
			pWnd = FindChildWnd( ID_EXIT );
			if( pWnd )
				pWnd->SetWndPos( pWnd->GetXPos(), m_iButtonYpos - m_iYPosGap );
			return;
		}
	}

	if( iSize == 0 )
	{
		pText->SetText( m_szFailReward.c_str() );
		m_RewardString.SetByUIText( pText, 0, 0 );
	}
	else
	{
		pText->SetText( m_szGetReward.c_str() );
		m_RewardString.SetByUIText( pText, 0, 0 );
	}
}

void PracticeResultWnd::StopSound()
{
	if( !m_szSuccessSound.IsEmpty() )
		g_SoundMgr.StopSound( m_szSuccessSound, 0 );

	if( !m_szFailSound.IsEmpty() )
		g_SoundMgr.StopSound( m_szFailSound, 0 );

	if( !m_szStarSound.IsEmpty() )
		g_SoundMgr.StopSound( m_szStarSound, 0 );
}

void PracticeResultWnd::RenderReward( int iPosX, int iPosY )
{
	if(m_pItemIconBg)
	{
		m_pItemIconBg->Render( iPosX+71, iPosY+48, UI_RENDER_MULTIPLY );
		m_pItemIconBg->Render( iPosX+170, iPosY+48, UI_RENDER_MULTIPLY );
		m_pItemIconBg->Render( iPosX+269, iPosY+48, UI_RENDER_MULTIPLY );
	}
	for(int i = 0 ; i < 3; ++i)
	{
		m_pRewardIcon = GetImage(m_szRewardCode[i]);
		if(m_pRewardIcon && m_pStamp)
		{
			if(m_iResultStar > i)
			{
				m_pRewardIcon->Render( iPosX+71+i*99, iPosY+48, UI_RENDER_NORMAL );
				m_pStamp->Render( iPosX+71+i*99, iPosY+48, UI_RENDER_NORMAL );
			}
			else
				m_pRewardIcon->Render( iPosX+71+i*99, iPosY+48, UI_RENDER_GRAY );
		}
		
	}	

	char szBuf[MAX_PATH] = {0,};	
	IoString str;
	for(int i = 0; i < 3; ++i)
	{
		wsprintf( szBuf, m_szRewardHelp[0].c_str(), i+1 );
		str.Set(0, 0, szBuf, FONT_SIZE_12, 60, TS_NORMAL, TCT_DEFAULT_DARKGRAY, 0, TAT_CENTER, TVA_TOP, 0);
		str.OnRender( iPosX + 71 + i*99, iPosY + 85 );
		str.Set(0, 0, m_szRewardName[i].c_str(), FONT_SIZE_13, 60, TS_NORMAL, TCT_DEFAULT_DARKGRAY, 0, TAT_CENTER, TVA_TOP, 0);
		str.OnRender( iPosX + 71 + i*99, iPosY + 98 );
	}
}

ioUIRenderImage* PracticeResultWnd::GetImage( const ioHashString &szIconName )
{
	if ( szIconName.IsEmpty() )
		return NULL;

	PracticeIconMap::iterator iter = m_RewardIconMap.find( szIconName );
	if( iter != m_RewardIconMap.end() )
		return iter->second;

	ioUIRenderImage *pIcon = g_UIImageSetMgr.CreateImageByFullName( szIconName );
	if( pIcon )
	{
		m_RewardIconMap.insert( PracticeIconMap::value_type( szIconName, pIcon ) );
		return pIcon;
	}

	return NULL;
}

void PracticeResultWnd::ClearIconMap()
{
	PracticeIconMap::iterator iCreate = m_RewardIconMap.begin();
	for( ; iCreate != m_RewardIconMap.end() ; ++iCreate )
		SAFEDELETE( iCreate->second );
	m_RewardIconMap.clear();
}

void PracticeResultWnd::SetRewardIconName()
{
	short iIndex = g_PracticeMgr.GetPreIndex();
	int iType = g_PracticeMgr.GetPracticeRewardAType(iIndex);
	ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( g_PracticeMgr.GetPracticeRewardValueC(iIndex) );
	if( !pEtcItem )
		return;
	m_szRewardCode[0] = pEtcItem->GetIconName();
	m_szRewardName[0] = pEtcItem->GetName();
	pEtcItem = g_EtcItemMgr.FindEtcItem( g_PracticeMgr.GetPracticeRewardValueB(iIndex) );
	m_szRewardCode[1] = pEtcItem->GetIconName();
	m_szRewardName[1] = pEtcItem->GetName();
	pEtcItem = g_EtcItemMgr.FindEtcItem( g_PracticeMgr.GetPracticeRewardValueA(iIndex) );
	m_szRewardCode[2] = pEtcItem->GetIconName();
	m_szRewardName[2] = pEtcItem->GetName();
}

PracticeUpdateRank::PracticeUpdateRank()
{
	m_pBackGround = NULL;
	m_pNum_Blue = NULL;
	m_pScore = NULL;
	m_iCurrentRank = 0;
	m_iUpdateRank = 0;
	m_dwStarTime = 0;
	m_pResultEffect = NULL;
}

PracticeUpdateRank::~PracticeUpdateRank()
{
	SAFEDELETE( m_pBackGround );
	SAFEDELETE( m_pNum_Blue );
	SAFEDELETE( m_pScore );
}

void PracticeUpdateRank::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "Bg")
	{
		SAFEDELETE( m_pBackGround );
		m_pBackGround = pImage;
	}
	else if( szType == "Num_Blue")
	{
		SAFEDELETE( m_pNum_Blue );
		m_pNum_Blue = pImage;
	}
	else if( szType == "score_text")
	{
		SAFEDELETE( m_pScore );
		m_pScore = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void PracticeUpdateRank::ParseExtraInfo( ioXMLElement &xElement )
{

	m_iEffectPosX = xElement.GetIntAttribute_e( "EffectPosX" );
	m_iEffectPosY = xElement.GetIntAttribute_e( "EffectPosY" );

	m_szExitBtn = xElement.GetStringAttribute("ExitBtn");
	m_szNewRecord = xElement.GetStringAttribute("TextInfo1");
	m_szUpdate = xElement.GetStringAttribute("TextInfo2");
	for(int i = 0; i < 3; ++i)
	{
		char szBuf[MAX_PATH] = "";
		wsprintf( szBuf, "TextUpdateInfo%d", i+1);
		m_szFromTo[i] = xElement.GetStringAttribute(szBuf);
	}
}

void PracticeUpdateRank::OnRender()
{
	ioWnd::OnRender();

	int iPosX = GetDerivedPosX();
	int iPosY = GetDerivedPosY();
	if(m_pBackGround)
	m_pBackGround->Render( iPosX+OFFSET_X, iPosY+OFFSET_Y, UI_RENDER_MULTIPLY );

	RenderRank( iPosX+OFFSET_X, iPosY+OFFSET_Y );

	RenderText( iPosX+OFFSET_X, iPosY+OFFSET_Y );
	

}

void PracticeUpdateRank::iwm_show()
{
	m_dwStarTime = FRAMEGETTIME();
	m_pResultEffect = GetEffect("ResultEffect");

	ioButton* pExitBtn = (ioButton*)FindChildWnd( ID_EXIT );
	if( pExitBtn )
	{
		char szExit[MAX_PATH] = {0,};
		sprintf( szExit, "%s(ESC)", m_szExitBtn.c_str() );
		pExitBtn->SetTitleText(szExit);
	}	
	SetResult();
}

void PracticeUpdateRank::SetUpdateRank( int iRank, int iUpdateRank )
{
	m_iCurrentRank = iRank;
	m_iUpdateRank = iUpdateRank;
}

void PracticeUpdateRank::OnProcess( float fTimePerSec )
{

	ioButton* pExitBtn = (ioButton*)FindChildWnd( ID_EXIT );
	if(!this->IsShow())
		return;
	ioWnd::OnProcess( fTimePerSec );

	if(m_iCurrentRank > 99999)
		m_iCurrentRank = 99999;
	if(m_iUpdateRank > 99999)
		m_iUpdateRank = 99999;

	if( pExitBtn )
	{
		char szReamin[MAX_PATH] = {0,};
		DWORD dwReaminTime = 0;
		DWORD dwCurTime = FRAMEGETTIME();
		if( ( m_dwStarTime + 5000 ) < dwCurTime )
		{
			HideWnd();
		}
		else
			dwReaminTime = (m_dwStarTime + 5000) - dwCurTime;

		int iReaminTime = (int)dwReaminTime / 1000;
		sprintf( szReamin, "%s(ESC) ...%d", m_szExitBtn.c_str(), iReaminTime );
		pExitBtn->SetTitleText(szReamin);
	}
	
}

void PracticeUpdateRank::RenderRank( int iPosX, int iPosY )
{
	DWORD iRank = m_iUpdateRank;//m_iCurrentRank;
	iRank = iRank > 99999 ? 99999:iRank;
	if( iRank > 9999 )
	{
		m_pNum_Blue->RenderNum( iPosX - 84, iPosY -41, iRank / 10000);
		m_pNum_Blue->RenderNum( iPosX - 58, iPosY -41, iRank % 10000 / 1000);
		m_pNum_Blue->RenderNum( iPosX - 32, iPosY -41, iRank % 1000 / 100);
		m_pNum_Blue->RenderNum( iPosX - 6, iPosY -41, iRank % 100 / 10);
		m_pNum_Blue->RenderNum( iPosX + 21, iPosY -41, iRank % 10);
		m_pScore->Render( iPosX + 48, iPosY - 31 );
	}
	else if(iRank > 999)
	{
		m_pNum_Blue->RenderNum( iPosX - 69, iPosY -41, iRank / 1000);
		m_pNum_Blue->RenderNum( iPosX - 42, iPosY -41, iRank % 1000 / 100);
		m_pNum_Blue->RenderNum( iPosX - 17, iPosY -41, iRank % 100 / 10);
		m_pNum_Blue->RenderNum( iPosX + 10, iPosY -41, iRank % 10);
		m_pScore->Render( iPosX + 38, iPosY - 31 );
	}
	else if(iRank > 99)
	{
		m_pNum_Blue->RenderNum( iPosX - 55, iPosY -41, iRank / 100);
		m_pNum_Blue->RenderNum( iPosX - 29, iPosY -41, iRank % 100 / 10);
		m_pNum_Blue->RenderNum( iPosX - 3, iPosY -41, iRank % 10);
		m_pScore->Render( iPosX + 24, iPosY - 31 );
	}
	else if(iRank > 9)
	{
		m_pNum_Blue->RenderNum( iPosX - 42, iPosY -41, iRank / 10);
		m_pNum_Blue->RenderNum( iPosX - 15, iPosY -41, iRank % 10);
		m_pScore->Render( iPosX + 11, iPosY - 31 );
	}
	else
	{
		m_pNum_Blue->RenderNum( iPosX - 29, iPosY -41, iRank % 10);
		m_pScore->Render( iPosX - 1, iPosY - 31 );
	}	
}

void PracticeUpdateRank::RenderText( int iPosX, int iPosY )
{
	IoString str;	
	//텍스트 받을것
	str.Set(0, 0, m_szNewRecord.c_str(), FONT_SIZE_16, 400, TS_OUTLINE_2X, TCT_DEFAULT_SKY, 0, TAT_CENTER, TVA_TOP, 0);
	str.OnRender( iPosX, iPosY -87 );

	//str.Set(0, 0, m_szUpdate.c_str(), FONT_SIZE_16, 325, TS_OUTLINE_2X, TCT_DEFAULT_GRAY, 0, TAT_CENTER, TVA_TOP, 0);
	//str.OnRender( iPosX - 47, iPosY + 33 );
	//
	//str.Set(0, 0, "00:00.00", FONT_SIZE_22, 325, TS_OUTLINE_2X, TCT_DEFAULT_WHITE, 0, TAT_CENTER, TVA_TOP, 0);
	//str.OnRender( iPosX + 31 , iPosY + 30 );

	char szBuf[MAX_PATH]="";
	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_OUTLINE_2X );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_SKY );	
	wsprintf( szBuf, m_szFromTo[0].c_str(), m_iCurrentRank);
	kPrinter.AddTextPiece( FONT_SIZE_16, szBuf );

	wsprintf( szBuf, m_szFromTo[1].c_str(), (m_iCurrentRank - m_iUpdateRank));
	kPrinter.SetTextColor( TCT_DEFAULT_WHITE );
	kPrinter.AddTextPiece( FONT_SIZE_16, szBuf );
	kPrinter.SetTextColor( TCT_DEFAULT_SKY );
	kPrinter.AddTextPiece( FONT_SIZE_16, m_szFromTo[2].c_str() );
	kPrinter.PrintFullText( iPosX , iPosY + 59, TAT_CENTER );
	kPrinter.ClearList();
}

void PracticeUpdateRank::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	if(!pWnd) return;

	DWORD dwID = pWnd->GetID();

	if( cmd == IOBN_BTNUP )
	{
		switch( dwID )
		{
		case ID_EXIT:
			{
				this->HideWnd();
			}
			break;
		}
	}
}

void PracticeUpdateRank::iwm_hide()
{
	if(m_pResultEffect)
		m_pResultEffect->Stop();
}

void PracticeUpdateRank::SetResult()
{
	if( m_pResultEffect )
	{
		m_pResultEffect->Start();
		m_pResultEffect->SetPosX( m_iEffectPosX - m_pResultEffect->GetWidth() / 2 );
		m_pResultEffect->SetPosY( m_iEffectPosY - m_pResultEffect->GetHeight() / 2 );
	}
}