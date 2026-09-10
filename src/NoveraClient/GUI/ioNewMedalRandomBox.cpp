
#include "StdAfx.h"

#include "ioNewMedalRandomBox.h"
#include "../ioExtendSoundManager.h"
//////////////////////////////////////////////////////////////////////////
ioNewMedalRandomBox::ioNewMedalRandomBox()
{
	m_pIcon	= NULL;
	m_pSubIcon = NULL;
	m_pBackEffect = NULL;
	m_pFinalBackEffect = NULL;
	m_pOverInfoBox = NULL;
	m_pLoopEffect = NULL;
	m_pResultEffect = NULL;

	m_iState = 0;
	m_iEnableSelectStat = 0;
	m_iSelectedStat = 0;
	m_iPresentIndex = 0;
	m_iPresentSlot = 0;
	m_iMedalCode = 0;
	m_bResult = false;
	m_bFinalResult = false;
}

ioNewMedalRandomBox::~ioNewMedalRandomBox()
{
	SAFEDELETE(m_pIcon);
	SAFEDELETE(m_pSubIcon);
	SAFEDELETE(m_pBackEffect);
	SAFEDELETE(m_pFinalBackEffect);
	SAFEDELETE(m_pOverInfoBox);
}

void ioNewMedalRandomBox::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "back_effect" )
	{
		SAFEDELETE( m_pBackEffect );
		m_pBackEffect = pImage;
	}
	else if( szType == "final_back_effect" )
	{
		SAFEDELETE( m_pFinalBackEffect );
		m_pFinalBackEffect = pImage;
	}
	else if( szType == "overinfobox" )
	{
		SAFEDELETE( m_pOverInfoBox );
		m_pOverInfoBox = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}

void ioNewMedalRandomBox::ParseExtraInfo( ioXMLElement &xElement )
{
	m_dwRollingTotalTime = xElement.GetIntAttribute_e( "Rolling_Total_Time" );
	m_szRollingSound = xElement.GetStringAttribute_e( "Rolling_Sound" );
	m_szResultSound = xElement.GetStringAttribute_e( "Result_Sound" );
	m_szTitleSubText = xElement.GetStringAttribute_e( "TitleSubText" );
	m_szSelectText = xElement.GetStringAttribute_e( "SelectText" );
	m_szLoopText = xElement.GetStringAttribute_e( "LoopText" );
	m_szResultText = xElement.GetStringAttribute_e( "ResultText" );
	m_szRecvStatErrMsg = xElement.GetStringAttribute_e( "Recv_Stat_Error_Message" );

	m_iEffectPosX = xElement.GetIntAttribute_e( "EffectPosX" );
	m_iEffectPosY = xElement.GetIntAttribute_e( "EffectPosY" );

	char szBuf[MAX_PATH]="";
	for (int i = 0; i < CHECKBTN_COUNT; i++)
	{
		wsprintf_e( szBuf, "CheckText%d", i+1 );
		m_vCheckText[i] = xElement.GetStringAttribute( szBuf );

		wsprintf_e( szBuf, "CheckIcon%d", i+1 );
		m_vCheckIcon[i] = xElement.GetStringAttribute( szBuf );
	}
}

void ioNewMedalRandomBox::ShowSelectStatus( int iPresentIndex, int iPresentSlot, int iMedalCode )
{
	if( iPresentIndex == 0 && iPresentSlot == 0 )
		return;

	m_bResult = false;
	m_bFinalResult = false;
	m_iSelectedStat = 0;
	m_iPresentIndex = iPresentIndex;
	m_iPresentSlot = iPresentSlot;
	m_iMedalCode = iMedalCode;

	int iMinPoint = 5;
	int iMaxPoint = 15;
	SAFEDELETE(m_pIcon);
	SAFEDELETE(m_pSubIcon);
	const ioMedalItemInfoManager::ItemInfo *pMedalItem = g_MedalItemMgr.GetItemInfo( iMedalCode );
	if( pMedalItem )
	{
		if( pMedalItem->m_iSubMedalType != ioMedalItemInfoManager::MEDALTYPE_CUSTOM )
			return;

		m_pIcon = g_UIImageSetMgr.CreateImageByFullName( pMedalItem->m_sIcon );
		m_pSubIcon = g_UIImageSetMgr.CreateImageByFullName( pMedalItem->m_sSubIcon );
		m_ItemName = pMedalItem->m_sName;
		m_iEnableSelectStat = pMedalItem->m_iSelectStat;

		iMinPoint = pMedalItem->m_iMinStatPoint;
		iMaxPoint = pMedalItem->m_iMaxStatPoint;
	}
	for(int i = 0;i < CHECKBTN_COUNT;i++)
	{
		ioNewMedalBtn *pCheck = static_cast<ioNewMedalBtn*>( FindChildWnd( ID_CHECKBTN + i ) );
		if( pCheck )
		{
			pCheck->SetCheck( false );
			pCheck->SetFinalStatus( 0, SELECT_BTN );
			pCheck->SetRandomRange( iMinPoint, iMaxPoint );
		}
	}
	ReBatchButton( SELECT_BTN );
	if( !IsShow() )
		ShowWnd();
}

void ioNewMedalRandomBox::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	ioWnd::OnProcess( fTimePerSec );

	RollingIcon( fTimePerSec );
}

void ioNewMedalRandomBox::RollingIcon( float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_bResult && !m_bFinalResult && m_dwRollingEndTime < dwCurTime )
		SetFinalResult();
}

void ioNewMedalRandomBox::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();
	if( m_iState == RESULT_BTN )
	{
		if( m_pFinalBackEffect )
			m_pFinalBackEffect->Render( iXPos + 180, iYPos + 91, UI_RENDER_MULTIPLY );
	}
	else
	{
		if( m_pBackEffect )
			m_pBackEffect->Render( iXPos + 180, iYPos + 91, UI_RENDER_MULTIPLY );
	}

	if( m_pIcon )
		m_pIcon->Render( iXPos + 180, iYPos + 91, UI_RENDER_NORMAL, TFO_BILINEAR );
	if( m_pSubIcon )
		m_pSubIcon->Render( iXPos + 180, iYPos + 91, UI_RENDER_NORMAL, TFO_BILINEAR );

	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 180, iYPos + 136, FONT_SIZE_16, m_ItemName.c_str() );

	char szBuf[MAX_PATH]="";
	wsprintf( szBuf, m_szTitleSubText.c_str(), m_iEnableSelectStat - m_iSelectedStat );
	g_FontMgr.SetTextColor( TCT_DEFAULT_RED );
	g_FontMgr.PrintText( iXPos + 180, iYPos + 160, FONT_SIZE_16, szBuf );

	if( m_pOverInfoBox )
	{
		m_pOverInfoBox->SetReverseFormatting( ioUIRenderImage::RF_NONE_REVERSE );
		m_pOverInfoBox->Render( iXPos + 17, iYPos + 198 );
		m_pOverInfoBox->SetReverseFormatting( ioUIRenderImage::RF_HORZ_REVERSE );
		m_pOverInfoBox->Render( iXPos + 288, iYPos + 198 );
	}

	g_FontMgr.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
	if( m_iState == SELECT_BTN )
	{
		wsprintf( szBuf, m_szSelectText.c_str(), m_iEnableSelectStat );
		g_FontMgr.PrintText( iXPos + 180, iYPos + 192, FONT_SIZE_12, szBuf );
	}
	else if( m_iState == ROLLING_BTN )
	{
		g_FontMgr.PrintText( iXPos + 180, iYPos + 192, FONT_SIZE_12, m_szLoopText.c_str() );
	}
	else if( m_iState == RESULT_BTN )
	{
		g_FontMgr.PrintText( iXPos + 180, iYPos + 192, FONT_SIZE_12, m_szResultText.c_str() );
	}
}

void ioNewMedalRandomBox::SetBtnPos()
{
	int XIndex = 0;
	int YIndex = 0;
	for(int i = 0;i < CHECKBTN_COUNT;i++)
	{
		ioWnd *pCheck = FindChildWnd( ID_CHECKBTN + i );
		if( pCheck )
		{
			pCheck->SetWndPos( 19 + 82 * XIndex, 295 - 81 * YIndex );
			XIndex++;
			if( XIndex == CHECKBTN_COUNT / 2 )
			{
				XIndex = 0;
				YIndex++;
			}
		}
	}
	ioWnd *pWnd = FindChildWnd( ID_LOCK );
	if( pWnd )
		pWnd->GoToTop();
}

void ioNewMedalRandomBox::iwm_create()
{
	for(int i = 0;i < CHECKBTN_COUNT;i++)
	{
		ioNewMedalBtn *pCheck = static_cast<ioNewMedalBtn*>( g_GUIMgr.AddWndByTemplate( ioHashString("ioNewMedalBtn"), this, ID_CHECKBTN + i ) );
		if( pCheck )
		{
			ioUIRenderImage *pIcon = g_UIImageSetMgr.CreateImageByFullName( m_vCheckIcon[i] );
			pCheck->SetIconTitle( pIcon, m_vCheckText[i] );
		}
	}
	ioWnd *pWnd = FindChildWnd( ID_LOCK );
	if( pWnd )
		pWnd->GoToTop();
}

void ioNewMedalRandomBox::iwm_show()
{
	SetBtnPos();

	m_pLoopEffect = GetEffect("LoopEffect");
	m_pResultEffect = GetEffect("ResultEffect");
}

void ioNewMedalRandomBox::iwm_hide()
{
	if( m_pLoopEffect )
		m_pLoopEffect->Stop();
	if(m_pResultEffect)
		m_pResultEffect->Stop();

	if( !m_szRollingSound.IsEmpty() )
		g_SoundMgr.StopSound( m_szRollingSound, 0 );
}

void ioNewMedalRandomBox::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_EXIT:
		if( cmd == IOBN_BTNUP )
			HideWnd();
		break;

	case ID_SELECT:
	case ID_RETRY:
		if( cmd == IOBN_BTNUP )
			SetRolling();
		break;

	case ID_CONFIRM:
		if( cmd == IOBN_BTNUP )
			SetConfirm();
		break;
	case ID_START:
		if( cmd == IOBN_BTNUP )
			ShowSelectStatus( m_iPresentIndex, m_iPresentSlot, m_iMedalCode );
		break;
	}
	if( COMPARE( dwID, ID_CHECKBTN, ID_CHECKBTN + CHECKBTN_COUNT ) )
	{
		if( cmd == IOBN_BTNUP )
		{
			ioNewMedalBtn *pCheck = static_cast<ioNewMedalBtn*>( pWnd );
			if( pCheck->IsChecked() )
			{
				if( m_iEnableSelectStat > m_iSelectedStat )
				{
					m_iSelectedStat++;
					if( m_iSelectedStat == m_iEnableSelectStat )
						SetChildActive( ID_SELECT );

					SetChildActive( ID_START );
				}
				else
				{
					pCheck->SetCheck( false );
				}
			}
			else
			{
				m_iSelectedStat--;
				if( m_iSelectedStat != m_iEnableSelectStat )
					SetChildInActive( ID_SELECT );
				if( m_iSelectedStat == 0 )
					SetChildInActive( ID_START );
			}
		}
	}
}

void ioNewMedalRandomBox::SetRolling()
{
	m_dwRollingEndTime = FRAMEGETTIME() + m_dwRollingTotalTime;
	m_bFinalResult = false;
	if( m_pLoopEffect )
	{
		if( !m_pLoopEffect->IsLive() )
			m_pLoopEffect->Start();

		m_pLoopEffect->SetPosX( m_iEffectPosX - m_pLoopEffect->GetWidth() / 2 );
		m_pLoopEffect->SetPosY( m_iEffectPosY - m_pLoopEffect->GetHeight() / 2 );
	}

	ReBatchButton( ROLLING_BTN );
	if( !m_szRollingSound.IsEmpty() )
		g_SoundMgr.PlaySound( m_szRollingSound, DSBVOLUME_MAX, PSM_LOOP | PSM_THREAD );

	bool bChecked[CHECKBTN_COUNT];
	for(int i = 0;i < CHECKBTN_COUNT;i++)
	{
		m_iFinalStatus[i] = 0;
		ioNewMedalBtn *pCheck = static_cast<ioNewMedalBtn*>( FindChildWnd( ID_CHECKBTN + i ) );
		if( pCheck )
		{
			pCheck->SetFinalStatus( 0, ROLLING_BTN );
			if( pCheck->IsChecked() )
				bChecked[i] = true;
			else
				bChecked[i] = false;
		}
	}

	if( m_iPresentIndex != 0 || m_iPresentSlot != 0 )
	{
		SP2Packet kPacket( CTPK_PRESENT_RECV );
		kPacket << m_iPresentIndex << m_iPresentSlot << CUSTOM_MEDAL_REQUEST;
		for(int i = 0;i < CHECKBTN_COUNT;i++)
			kPacket << bChecked[i];

		TCPNetwork::SendToServer( kPacket );
		TCPNetwork::MouseBusy( true );
	}
}

void ioNewMedalRandomBox::SetConfirm()
{
	if( m_iPresentIndex != 0 || m_iPresentSlot != 0 )
	{
		SP2Packet kPacket( CTPK_PRESENT_RECV );
		kPacket << m_iPresentIndex << m_iPresentSlot << CUSTOM_MEDAL_DONE;
		for(int i = 0;i < CHECKBTN_COUNT;i++)
			kPacket << m_iFinalStatus[i];

		TCPNetwork::SendToServer( kPacket );
		TCPNetwork::MouseBusy( true );
	}

	HideWnd();
}

void ioNewMedalRandomBox::SetFinalResult()
{
	m_bFinalResult = true;
	ReBatchButton( RESULT_BTN );

	if( m_pResultEffect )
	{
		if( m_pLoopEffect )
			m_pLoopEffect->Stop();
		
		m_pResultEffect->Start();
		m_pResultEffect->SetPosX( m_iEffectPosX - m_pResultEffect->GetWidth() / 2 );
		m_pResultEffect->SetPosY( m_iEffectPosY - m_pResultEffect->GetHeight() / 2 );
	}
	if( !m_szRollingSound.IsEmpty() )
		g_SoundMgr.StopSound( m_szRollingSound, 0 );

	if( g_ExSoundMgr.IsSound( ExSound::EST_GASHAPON_GET ) )
		g_ExSoundMgr.PlaySound( ExSound::EST_GASHAPON_GET );
	else
	{
		if( !m_szResultSound.IsEmpty() )
			g_SoundMgr.PlaySound( m_szResultSound );
	}
	for(int i = 0;i < CHECKBTN_COUNT;i++)
	{
		ioNewMedalBtn *pCheck = static_cast<ioNewMedalBtn*>( FindChildWnd( ID_CHECKBTN + i ) );
		if( pCheck )
			pCheck->SetFinalStatus( m_iFinalStatus[i], RESULT_BTN );
	}
}

void ioNewMedalRandomBox::ReBatchButton( int iType )
{
	m_iState = iType;
	switch( iType )
	{
	case SELECT_BTN:
		ShowChildWnd(ID_SELECT);
		HideChildWnd(ID_ROLLING);
		HideChildWnd(ID_CONFIRM);
		HideChildWnd(ID_RETRY);
		HideChildWnd(ID_LOCK);
		ShowChildWnd(ID_START);
		SetChildInActive( ID_SELECT );
		SetChildInActive( ID_START );
		break;
	case ROLLING_BTN:
		HideChildWnd(ID_SELECT);
		ShowChildWnd(ID_ROLLING);
		HideChildWnd(ID_CONFIRM);
		HideChildWnd(ID_RETRY);
		ShowChildWnd(ID_LOCK);
		HideChildWnd(ID_START);
		SetChildInActive(ID_ROLLING);
		break;
	case RESULT_BTN:
		HideChildWnd(ID_SELECT);
		HideChildWnd(ID_ROLLING);
		ShowChildWnd(ID_CONFIRM);
		ShowChildWnd(ID_RETRY);
		ShowChildWnd(ID_LOCK);
		ShowChildWnd(ID_START);
		SetChildActive( ID_START );
		break;
	}
}

void ioNewMedalRandomBox::SetStatus( int iPresentIndex, int iPresentSlot, int iStat[8] )
{
	if( iPresentIndex != m_iPresentIndex || iPresentSlot != m_iPresentSlot )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, m_szRecvStatErrMsg.c_str() );
		HideWnd();
		return;
	}
	m_bResult = true;

	for( int i = 0; i < 8; i++ )
		m_iFinalStatus[i] = iStat[i];
}
//////////////////////////////////////////////////////////////////////////
ioNewMedalBtn::ioNewMedalBtn()
{
	m_pIcon = NULL;
	m_pIconBack = NULL;
	m_pNumText = NULL;
	m_pPlusText = NULL;
	m_pSelectFrm = NULL;
	m_pCheckFrm = NULL;

	m_iStatue = 0;
	m_dwStatusAniStartTime = 0;
	m_iRandomMin = 1;
	m_iRandomMax = 99;
}

ioNewMedalBtn::~ioNewMedalBtn()
{
	SAFEDELETE( m_pIcon );
	SAFEDELETE( m_pIconBack );
	SAFEDELETE( m_pNumText );
	SAFEDELETE( m_pPlusText );
	SAFEDELETE( m_pSelectFrm );
	SAFEDELETE( m_pCheckFrm );
}

void ioNewMedalBtn::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "SelectFrm" )
	{
		SAFEDELETE( m_pSelectFrm );
		m_pSelectFrm = pFrame;
	}
	else if( szType == "Check" )
	{
		SAFEDELETE( m_pCheckFrm );
		m_pCheckFrm = pFrame;
	}
	else
	{
		ioCheckButton::AddRenderFrame( szType, pFrame );
	}
}

void ioNewMedalBtn::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "IconBack" )
	{
		SAFEDELETE( m_pIconBack );
		m_pIconBack = pImage;
		m_pIconBack->SetScale( 0.64f );
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
		ioCheckButton::AddRenderImage( szType, pImage );
	}
}

void ioNewMedalBtn::ParseExtraInfo( ioXMLElement &xElement )
{
	m_dwStatusAniTime = xElement.GetIntAttribute_e( "status_ani_time" );
	m_dwStatusEffectGapTime = xElement.GetIntAttribute_e( "status_effect_gap_time" );

	ioCheckButton::ParseExtraInfo( xElement );
}

void ioNewMedalBtn::SetIconTitle( ioUIRenderImage *pIcon, ioHashString szTitle )
{
	m_szTitleText = szTitle;
	SAFEDELETE( m_pIcon );
	if( pIcon )
	{
		m_pIcon = pIcon;
		m_pIcon->SetScale( 0.64f );
	}
}

void ioNewMedalBtn::SetFinalStatus( int iValue, int iType )
{
	m_iType = iType;
	m_iStatue = iValue;
	m_dwStatusAniStartTime = FRAMEGETTIME();
}

void ioNewMedalBtn::OnRender()
{
	ioCheckButton::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();
	if( IsChecked() && m_pCheckFrm )
		m_pCheckFrm->Render( iXPos, iYPos );

	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	if( IsChecked() )
		g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	else
		g_FontMgr.SetTextColor( TCT_DEFAULT_LIGHTGRAY );

	g_FontMgr.PrintText( iXPos + 40, iYPos + 60, FONT_SIZE_12, m_szTitleText.c_str() );

	if( m_pIconBack )
		m_pIconBack->Render( iXPos + 39, iYPos + 30, UI_RENDER_MULTIPLY, TFO_BILINEAR );

	UIRenderType eType = UI_RENDER_NORMAL;
	if( !IsChecked() )
		eType = UI_RENDER_GRAY;

	if( m_pIcon )
		m_pIcon->Render( iXPos + 39, iYPos + 30, eType, TFO_BILINEAR );

	int iRenderNum = 0;
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_iType == ioNewMedalRandomBox::ROLLING_BTN && IsChecked() )
	{
		if( m_iStatue == 0 || dwCurTime > m_dwStatusAniTime + m_dwStatusAniStartTime )
		{
			m_dwStatusAniStartTime = dwCurTime;

			m_iStatue = m_iRandomMin + rand() % ( m_iRandomMax - m_iRandomMin + 1 );
		}
		iRenderNum = m_iStatue;
	}
	else if( m_iType == ioNewMedalRandomBox::RESULT_BTN && m_dwStatusAniStartTime + m_dwStatusEffectGapTime < dwCurTime )
		iRenderNum = m_iStatue;

	if( iRenderNum != 0 )
	{
		int iNewXPos = iXPos;
		if( iRenderNum < 10 )
			iNewXPos += 22;
		else
			iNewXPos += 15;

		if( m_pPlusText )
			m_pPlusText->Render( iNewXPos, iYPos + 28 );
		if( m_pNumText )
			m_pNumText->RenderNum( iNewXPos + 15, iYPos + 28, iRenderNum, -7 );
	}
	if( IsChecked() && m_pSelectFrm )
		m_pSelectFrm->Render( iXPos, iYPos );
}

void ioNewMedalBtn::SetRandomRange( int iMin, int iMax )
{
	m_iRandomMin = iMin;
	m_iRandomMax = iMax;
}