
#include "StdAfx.h"

#include "../ioLadderTeamMgr.h"
#include "../ioShuffleRoomMgr.h"
#include "../ioRankBattleManager.h"

#include "GameOptionWnd.h"
#include "VoiceChatWnd.h"
#include "GlobalWnd.h"

VoiceChatItem::VoiceChatItem()
{
	m_sName.Clear();
	m_eVoiceType = VT_DISABLE;

	m_dwPreviousTime  = 0;
	m_dwCheckDuration = 0;
	m_fNameWidth      = 0.0f;

	m_pHead        = NULL;
	m_pHeadOutLine = NULL;
	m_pOverImg        = NULL;
	m_pOverOutLineImg = NULL;
	m_pDisable   = NULL;
	m_pWait      = NULL;
	m_pSoundWav1 = NULL;
	m_pSoundWav2 = NULL;
	m_pSoundWav3 = NULL;
}

VoiceChatItem::~VoiceChatItem()
{
	SAFEDELETE( m_pHead );
	SAFEDELETE( m_pHeadOutLine );
	SAFEDELETE( m_pOverImg );
	SAFEDELETE( m_pOverOutLineImg );
	SAFEDELETE( m_pDisable );
	SAFEDELETE( m_pWait );
	SAFEDELETE( m_pSoundWav1 );
	SAFEDELETE( m_pSoundWav2 );
	SAFEDELETE( m_pSoundWav3 );
}

void VoiceChatItem::OnProcess(float fTimePerSec )
{
	if(!IsShow())  return;

	PrecessVoiceType();
}


void VoiceChatItem::OnRender()
{
	ioButton::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_sName.IsEmpty() )
		return;

	enum
	{
		HEAD_XOFFSET = 0,
		HEAD_YOFFSET = 0,
		HEAD_TEXT_XOFFSET = 18,
		HEAD_TEXT_YOFFSET = 0,
		NAME_XOFFSET      = 37,
		NAME_YOFFSET      = 0,
		VOICE_XOFFSET = 1,
		VOICE_YOFFSET = 0,
		OVER_XOFFSET  = 0,
		OVER_YOFFSET  = -1,
		OVER_OUT_LINE_XOFFSET   = 0,
		OVER_OUT_LINE_YOFFSET_1 = -2,
		OVER_OUT_LINE_YOFFSET_2 = 17,

		OVER_ADD_WIDTH = 35,
		OVER_HIEGHT    = 18,
		OVER_OUT_LINE_HEIGHT = 1,
	};

	DWORD dwColor = 0;
	if( m_eVoiceType == VT_DISABLE || m_eVoiceType == VT_WAIT )
		dwColor = D3DCOLOR_XRGB( 128, 128, 128 );
	else
		dwColor = TCT_DEFAULT_WHITE;

	// over
	if( m_bOver || m_bClicked )
	{
		if( m_pOverImg )
		{
			m_pOverImg->SetSize( GetWidth() +  OVER_ADD_WIDTH, OVER_HIEGHT );
			m_pOverImg->Render( iXPos + OVER_XOFFSET, iYPos + OVER_YOFFSET, UI_RENDER_ADD );
		}

		if( m_pOverOutLineImg )
		{
			m_pOverOutLineImg->SetSize( GetWidth() +  OVER_ADD_WIDTH, OVER_OUT_LINE_HEIGHT );
			m_pOverOutLineImg->Render( iXPos + OVER_OUT_LINE_XOFFSET, iYPos + OVER_OUT_LINE_YOFFSET_1 );

			m_pOverOutLineImg->SetSize( GetWidth() +  OVER_ADD_WIDTH, OVER_OUT_LINE_HEIGHT );
			m_pOverOutLineImg->Render( iXPos + OVER_OUT_LINE_XOFFSET, iYPos + OVER_OUT_LINE_YOFFSET_2 );
		}
	}

	// head
	if( m_pHeadOutLine )
		m_pHeadOutLine->Render( iXPos + HEAD_XOFFSET, iYPos + HEAD_YOFFSET );
	if( m_pHead )
	{
		m_pHead->SetColor( dwColor );
		m_pHead->Render( iXPos + HEAD_XOFFSET, iYPos + HEAD_YOFFSET, UI_RENDER_NORMAL_ADD_COLOR );
	}

	ioHashString szHeadText;
	if( m_eVoiceType == VT_DISABLE )
		szHeadText = STR(1);
	else
		szHeadText = STR(2);

	g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( dwColor );
	g_FontMgr.PrintText(iXPos + HEAD_TEXT_XOFFSET, iYPos + HEAD_TEXT_YOFFSET, FONT_SIZE_12, szHeadText.c_str() );

	// name
	g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( dwColor );
	g_FontMgr.PrintText( iXPos + NAME_XOFFSET, iYPos + NAME_YOFFSET, FONT_SIZE_12, m_sName.c_str() );

	// voice type
	if( m_eVoiceType == VT_WAIT )
	{
		if(m_pWait)
			m_pWait->Render( iXPos + NAME_XOFFSET + m_fNameWidth + VOICE_XOFFSET, iYPos + VOICE_YOFFSET );
	}
	else if( m_eVoiceType == VT_DISABLE )
	{
		if(m_pDisable)
			m_pDisable->Render( iXPos + NAME_XOFFSET + m_fNameWidth + VOICE_XOFFSET, iYPos + VOICE_YOFFSET );
	}
	else if( m_eVoiceType == VT_WAVE_1 )
	{
		if(m_pSoundWav1)
			m_pSoundWav1->Render( iXPos + NAME_XOFFSET + m_fNameWidth + VOICE_XOFFSET, iYPos + VOICE_YOFFSET );
	}
	else if( m_eVoiceType == VT_WAVE_2 )
	{
		if(m_pSoundWav2)
			m_pSoundWav2->Render( iXPos + NAME_XOFFSET + m_fNameWidth + VOICE_XOFFSET, iYPos + VOICE_YOFFSET );
	}
	else if( m_eVoiceType == VT_WAVE_3 )
	{
		if(m_pSoundWav3)
			m_pSoundWav3->Render( iXPos + NAME_XOFFSET + m_fNameWidth + VOICE_XOFFSET, iYPos + VOICE_YOFFSET );
	}
}

void VoiceChatItem::ParseExtraInfo(ioXMLElement &xElement )
{
	m_dwCheckDuration = xElement.GetIntAttribute_e( "CheckDuration" );
}

void VoiceChatItem::SetInfo( const ioHashString &rkID, bool bDisable )
{
	m_sName      = rkID;
	m_fNameWidth = g_FontMgr.GetTextWidth( m_sName.c_str(), TS_OUTLINE_FULL_2X, FONT_SIZE_12 );

	if( bDisable )
		m_eVoiceType = VT_DISABLE;
	else
		m_eVoiceType = VT_WAIT;

	enum { HEAD_WIDTH = 37, SPEAKER_WIDTH = 19,  };
	SetSize( (int)m_fNameWidth + HEAD_WIDTH + SPEAKER_WIDTH, GetHeight() );
}

VoiceChatItem::VoiceType VoiceChatItem::GetWaveType( int iSoundPeak )
{
	if ( iSoundPeak < 10 )
		return VT_WAVE_1;
	else if(COMPARE(iSoundPeak, 10, 23))
		return VT_WAVE_2;
	else if(COMPARE(iSoundPeak, 23, 36))
		return VT_WAVE_3;
	else 
		return VT_WAVE_3;
}

void VoiceChatItem::AddRenderImage(const ioHashString &szType, ioUIRenderImage *pImage)
{
	if( szType == "Head" )
	{
		SAFEDELETE( m_pHead );
		m_pHead = pImage;
	}
	else if( szType == "HeadOutLine" )
	{
		SAFEDELETE( m_pHeadOutLine );
		m_pHeadOutLine = pImage;
	}
	else if( szType == "OverImg" )
	{
		SAFEDELETE( m_pOverImg );
		m_pOverImg = pImage;
		if( m_pOverImg )
		{
			m_pOverImg->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
			m_pOverImg->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
		}
	}
	else if( szType == "OverOutLineImg" )
	{
		SAFEDELETE( m_pOverOutLineImg );
		m_pOverOutLineImg = pImage;
		if( m_pOverOutLineImg )
		{
			m_pOverOutLineImg->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
			m_pOverOutLineImg->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
		}
	}
	else if(szType == "Disable")
	{
		SAFEDELETE( m_pDisable );
		m_pDisable = pImage;
	}
	else if(szType == "Wait")
	{
		SAFEDELETE( m_pWait );
		m_pWait = pImage;	
	}
	else if(szType == "SoundWav3")
	{
		SAFEDELETE( m_pSoundWav3 );
		m_pSoundWav3 = pImage;	
	}
	else if(szType == "SoundWav2")
	{
		SAFEDELETE( m_pSoundWav2 );
		m_pSoundWav2 = pImage;	
	}
	else if(szType == "SoundWav1")
	{
		SAFEDELETE( m_pSoundWav1 );
		m_pSoundWav1 = pImage;	
	}
	else
	{
		ioButton::AddRenderImage( szType, pImage);
	}
}

void VoiceChatItem::PrecessVoiceType()
{
	if( m_sName.IsEmpty() ) return;
	if( m_eVoiceType == VT_DISABLE ) return;

	if( ( FRAMEGETTIME() - m_dwPreviousTime ) < m_dwCheckDuration ) return;
	m_dwPreviousTime = FRAMEGETTIME();

	if( m_sName == g_MyInfo.GetPublicID() )
	{
		if( !g_VoiceChat.IsPlay(m_sName.c_str()) )
			m_eVoiceType = VT_WAIT;
		else
		{
			int iSoundPeak = g_VoiceChat.GetPeak( m_sName.c_str() );
			m_eVoiceType = GetWaveType( iSoundPeak );
		}
	}
	else
	{
		if( !g_VoiceChat.IsExistCaptureData() )
			m_eVoiceType = VT_WAIT;

		int iSoundPeak = -1;
		g_VoiceChat.PopCapturePeak( iSoundPeak );
		if( iSoundPeak != -1 )
			m_eVoiceType = GetWaveType( iSoundPeak );
	}
}
//--------------------------------------------------------------------------------------------------------------------------
VoiceChatOptionBtn::VoiceChatOptionBtn()
{
	m_pStrechNormal         = NULL;
	m_pStrechOverAndPush    = NULL;
}

VoiceChatOptionBtn::~VoiceChatOptionBtn()
{
	SAFEDELETE( m_pStrechNormal );
	SAFEDELETE( m_pStrechOverAndPush );
}

void VoiceChatOptionBtn::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "StrechNormal" )
	{
		SAFEDELETE( m_pStrechNormal );
		m_pStrechNormal = pImage;
	}
	else if( szType == "StrechOverAndPush" )
	{
		SAFEDELETE( m_pStrechOverAndPush );
		m_pStrechOverAndPush = pImage;
	}
	else
	{
		ioButton::AddRenderImage( szType, pImage );	
	}	
}

void VoiceChatOptionBtn::OnDrawNormal( int iXPos, int iYPos )
{
	ioButton::OnDrawNormal( iXPos, iYPos );
	if( m_pStrechNormal )
		m_pStrechNormal->Render( iXPos, iYPos, UI_RENDER_MULTIPLY );
}

void VoiceChatOptionBtn::OnDrawOvered( int iXPos, int iYPos )
{
	ioButton::OnDrawOvered( iXPos, iYPos );
	if( m_pStrechOverAndPush )
		m_pStrechOverAndPush->Render( iXPos, iYPos );
}

void VoiceChatOptionBtn::OnDrawPushed( int iXPos, int iYPos )
{
	ioButton::OnDrawPushed( iXPos, iYPos );
	if( m_pStrechOverAndPush )
		m_pStrechOverAndPush->Render( iXPos, iYPos );
}

void VoiceChatOptionBtn::UpdateImgSize()
{
	if( m_pStrechNormal )
		m_pStrechNormal->SetSize( GetWidth(), GetHeight() );
	if( m_pStrechOverAndPush )
		m_pStrechOverAndPush->SetSize( GetWidth(), GetHeight() );
}
//--------------------------------------------------------------------------------------------------------------------------
VoiceChatOptionWnd::VoiceChatOptionWnd()
{
}

VoiceChatOptionWnd::~VoiceChatOptionWnd()
{

}

void VoiceChatOptionWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )	return;

	// 다른 곳에서 클릭이 일어났는지 확인
	ioWnd *pPreClick = g_GUIMgr.GetPreClickWnd();
	if( !pPreClick )
		return;
	
	if( pPreClick == this )
		return;
	for (int i = 0; i < MAX_BTN ; i++)
	{
		if( pPreClick == FindChildWnd( ID_OPTION_BTN_1 + i) )
			return;
	}
			
	HideWnd();
}

void VoiceChatOptionWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_OPTION_BTN_1:
		if(cmd == IOBN_BTNUP)
		{
			g_App.OpenMannerWnd( m_sName.c_str(), true );
			HideWnd();
		}
		break;
	case ID_OPTION_BTN_2:
		if(cmd == IOBN_BTNUP)
		{
			VoiceChatWnd *pVoiceChat = dynamic_cast<VoiceChatWnd*> ( g_GUIMgr.FindWnd( VOICE_CHAT_WND ) );
			if( pVoiceChat )
				pVoiceChat->SetVoiceDisableEnable( m_sName );
			HideWnd();
		}
		break;
	}
}

void VoiceChatOptionWnd::SetInfo( const ioHashString &rsName, bool bDisable )
{
	m_sName = rsName;
	if( m_sName.IsEmpty() )
		return;

	// text
	float fTopWidth = 0.0f;
	for (int i = 0; i < MAX_BTN ; i++)
	{
		DWORD dwID = ID_OPTION_BTN_1 + i;
		ioWnd *pChild = FindChildWnd( dwID );
		if( !pChild )
			continue;	

		char szTitle[MAX_PATH]="";
		if( dwID == ID_OPTION_BTN_1 )
			SafeSprintf( szTitle, sizeof( szTitle ), STR(1), m_sName.c_str() );
		else
		{
			if( bDisable )
				StringCbCopy( szTitle, sizeof( szTitle ), STR(2) );
			else
				StringCbCopy( szTitle, sizeof( szTitle ), STR(3) );
		}
		pChild->SetTitleText( szTitle );

		float fCurWidth = g_FontMgr.GetTextWidth( szTitle, TS_NORMAL, FONT_SIZE_12 );
		if( fCurWidth > fTopWidth )
			fTopWidth = fCurWidth;
	}

	// wnd size
	enum { WND_ADD_SIZE = 24, BTN_ADD_SIZE = 10 };
	SetSize( fTopWidth + WND_ADD_SIZE, GetHeight() );
	for (int i = 0; i < MAX_BTN ; i++)
	{
		DWORD dwID = ID_OPTION_BTN_1 + i;
		VoiceChatOptionBtn *pChild = dynamic_cast<VoiceChatOptionBtn*> (  FindChildWnd( dwID ) );
		if( !pChild )
			continue;	
		pChild->SetSize(  fTopWidth + BTN_ADD_SIZE, pChild->GetHeight() );
		pChild->UpdateImgSize();
	}
}

void VoiceChatOptionWnd::iwm_show()
{
	m_sName.Clear();
}
//--------------------------------------------------------------------------------------------------------------------------

VoiceChatWnd::VoiceChatWnd()
{
	m_bInit        = false;
	m_pPlayStage   = NULL;
	m_iCurPage     = 0;
	m_iMaxPage     = 0;
	m_iDefaultX  = 0;
	m_iDefaultY  = 0;
}

VoiceChatWnd::~VoiceChatWnd()
{
	m_vVoiceInfoVec.clear();
	if( !COMPARE( m_iDefaultX, 0, Setting::Width() ) || 
		!COMPARE( m_iDefaultY, 0, Setting::Height() ) )
		return;

	g_GUIMgr.GUIPosSaveInt( "VoiceChatWnd", "XPos", m_iDefaultX ,POS_SAVE_VERSION );
	g_GUIMgr.GUIPosSaveInt( "VoiceChatWnd", "YPos", m_iDefaultY ,POS_SAVE_VERSION );
}

void VoiceChatWnd::SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate )
{
	float fPosGap = ( Setting::Height() - 600.0f ) / 10;

	int iTempDefXPos = m_iDefXPos;
	int iTempDefYPos = m_iDefYPos;

	m_iDefXPos = iTempDefXPos + fPosGap;
	m_iDefYPos = iTempDefYPos - fPosGap;

	ioMovingWnd::SetPositionToDefault( iParentWidth, iParentHeight, bChildMove, bCreate );

	m_iDefXPos = iTempDefXPos;
	m_iDefYPos = iTempDefYPos;

	if( !bCreate )
	{
		m_iDefaultX = GetXPos();
		m_iDefaultY = GetYPos();
		SetWndPos( m_iDefaultX, m_iDefaultY );
		UpdatePage( m_iCurPage );
	}
}

void VoiceChatWnd::iwm_create()
{
	int iXPos = g_GUIMgr.GUIPosLoadInt( "VoiceChatWnd", "XPos", GetXPos() ,POS_SAVE_VERSION );
	int iYPos = g_GUIMgr.GUIPosLoadInt( "VoiceChatWnd", "YPos", GetYPos() ,POS_SAVE_VERSION );

	if( COMPARE( iXPos, 0, Setting::Width() ) && 
		COMPARE( iYPos, 0, Setting::Height() ) )
	{
		SetWndPos( iXPos, iYPos );
	}

	m_iDefaultX = GetXPos();
	m_iDefaultY = GetYPos();
}

void VoiceChatWnd::iwm_show()
{
	SetCheckParentOutSide( true );
}

void VoiceChatWnd::iwm_hide()
{
	SetCheckParentOutSide( false );
}

void VoiceChatWnd::OnProcess(float fTimePerSec )
{
	InitByProcess();

	if( !IsShow() ) 
		return;
	ioWnd::OnProcess( fTimePerSec );

	if( g_KeyManager.IsVoiceChatKeyPress() )
		ChangeOptionAndSetVoice();

	// scroll show hide
	ioLSMouse *pMouse = g_App.GetMouse();
	if( !pMouse ) 
		return;

	ioWnd *pGlobalChat = g_GUIMgr.FindWnd( GLOBAL_CHAT_WND );
	if( !pGlobalChat ) 
		return;
	ioWnd *pInput = pGlobalChat->FindChildWnd( GlobalChatWnd::ID_CHAT_INPUT_WND );
	if( !pInput )
		return;

	enum { CHECK_WIDTH = 180, };
	RECT rcRect = { GetDerivedPosX(), GetDerivedPosY(), GetDerivedPosX() + CHECK_WIDTH, GetDerivedPosY() + GetHeight() };
	if( ::PtInRect( &rcRect, pMouse->GetMousePos() ) || pInput->IsShow() )
	{
		if( !m_vVoiceInfoVec.empty() )
			ShowChildWnd( ID_VERT_SCROLL );
	}
	else
		HideChildWnd( ID_VERT_SCROLL );

	ioMovingWnd::OnProcess( fTimePerSec );

	if( g_GUIMgr.IsDragMove( m_dwID ) || IsMoving() )
	{
		if( m_iDefaultX != GetXPos() )
		{
			if( COMPARE( GetXPos(), 0, (Setting::Width() - GetWidth() ) + 1 ) )
				m_iDefaultX = GetXPos();
		}

		if( m_iDefaultY != GetYPos() )
		{
			int iYPos = GetYPos();
			if( COMPARE( iYPos, 0, (Setting::Height() - GetHeight() ) + 1 ) )
				m_iDefaultY = iYPos;
		}
	}
}

void VoiceChatWnd::OnRender()
{
	ioWnd::OnRender();
}

void VoiceChatWnd::SetVoiceOn(const ioHashString &rkID)
{
	if( rkID.IsEmpty() )
		return;

	int iSize = m_vVoiceInfoVec.size();

	if( ioPlayMode::GetModeType() == MT_TRAINING || ioPlayMode::GetModeType() == MT_HEADQUARTERS || ioPlayMode::GetModeType() == MT_HOUSE )
	{
		if( iSize >= Help::GetPlazaMaxPlayer() * 2 )
			return;
	}
	else
	{
		if( iSize >= MAX_PLAYER * 2  )
			return;
	}

	for(vVoiceInfoVec::iterator iter = m_vVoiceInfoVec.begin(); iter != m_vVoiceInfoVec.end(); ++iter)
	{
	    VoiceInfo &rkInfo = (*iter);
		if( rkID == rkInfo.m_sName )
		{
			if( rkInfo.m_bDisable )
			{
				rkInfo.m_bDisable = false;
				SendVoiceInfo( rkID , true );
				UpdateScroll( false );
			}
			return;
		}
	}

	VoiceInfo kTemp;
	kTemp.m_sName    = rkID;
	kTemp.m_bDisable = false;
	m_vVoiceInfoVec.push_back( kTemp );
	SendVoiceInfo( rkID , true );
	UpdateScroll( true );
}

void VoiceChatWnd::SetVoiceOff(const ioHashString &rkID)
{
	for(vVoiceInfoVec::iterator iter = m_vVoiceInfoVec.begin(); iter != m_vVoiceInfoVec.end(); ++iter)
	{
		VoiceInfo &rkInfo = (*iter);
		if( rkID != rkInfo.m_sName )
			continue;
	    m_vVoiceInfoVec.erase( iter );
		SendVoiceInfo( rkID , false );
		UpdateScroll( true );
	    return;
	}
}

bool VoiceChatWnd::IsDisable(const ioHashString &rkID)
{
	for(vVoiceInfoVec::iterator iter = m_vVoiceInfoVec.begin(); iter != m_vVoiceInfoVec.end(); ++iter)
	{
		VoiceInfo &rkInfo = (*iter);
		if( rkID != rkInfo.m_sName )
			continue;
		return rkInfo.m_bDisable;
	}

	return false;
}

void VoiceChatWnd::AllOtherVoiceOff()
{
	VoiceInfo kMyInfo;
	for(vVoiceInfoVec::iterator iter = m_vVoiceInfoVec.begin(); iter != m_vVoiceInfoVec.end(); ++iter)
	{
		VoiceInfo &rkInfo = (*iter);
		if( g_MyInfo.GetPublicID() != rkInfo.m_sName )
			continue;
		kMyInfo = rkInfo;
		break;
	}
	m_vVoiceInfoVec.clear();
	
	if( !kMyInfo.m_sName.IsEmpty() )
		m_vVoiceInfoVec.push_back( kMyInfo );
	UpdateScroll( true );
}

void VoiceChatWnd::AllOtherVoiceDisable()
{
	for(vVoiceInfoVec::iterator iter = m_vVoiceInfoVec.begin(); iter != m_vVoiceInfoVec.end(); ++iter)
	{
		VoiceInfo &rkInfo = (*iter);
		if( g_MyInfo.GetPublicID() == rkInfo.m_sName )
			continue;
		rkInfo.m_bDisable = true;
		SendVoiceInfo( rkInfo.m_sName , false );
	}

	UpdateScroll( false );
}

void VoiceChatWnd::AllOtherVoiceEnable()
{
	for(vVoiceInfoVec::iterator iter = m_vVoiceInfoVec.begin(); iter != m_vVoiceInfoVec.end(); ++iter)
	{
		VoiceInfo &rkInfo = (*iter);
		if( g_MyInfo.GetPublicID() == rkInfo.m_sName )
			continue;
		rkInfo.m_bDisable = false;
		SendVoiceInfo( rkInfo.m_sName , true );
	}

	UpdateScroll( false );
}


void VoiceChatWnd::SetVoiceEnable( const ioHashString &rkID )
{
	for(vVoiceInfoVec::iterator iter = m_vVoiceInfoVec.begin(); iter != m_vVoiceInfoVec.end(); ++iter)
	{
		VoiceInfo &rkInfo = (*iter);
		if( rkID != rkInfo.m_sName )
			continue;
		rkInfo.m_bDisable = false;
		SendVoiceInfo( rkInfo.m_sName , true );
		break;
	}

	UpdateScroll( false );
}

void VoiceChatWnd::SetVoiceDisable(const ioHashString &rkID)
{
	for(vVoiceInfoVec::iterator iter = m_vVoiceInfoVec.begin(); iter != m_vVoiceInfoVec.end(); ++iter)
	{
		VoiceInfo &rkInfo = (*iter);
		if( rkID != rkInfo.m_sName )
			continue;
		rkInfo.m_bDisable = true;
		SendVoiceInfo( rkInfo.m_sName , false );
		break;
	}

	UpdateScroll( false );
}

void VoiceChatWnd::UpdateScroll( bool bSetRange )
{
	ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
	if( !pScroll ) return;

	if( bSetRange )
	{
		m_iCurPage = m_vVoiceInfoVec.size();
		pScroll->SetScrollRange( 0, m_vVoiceInfoVec.size() );
		m_iMaxPage = pScroll->GetMaxPos();
	}
	pScroll->SetScrollPos( m_iCurPage );
}

void VoiceChatWnd::SendVoiceInfo( const ioHashString &rsName, bool bOn )
{
	if( rsName.IsEmpty() )
		return;

	int iMsgType = 0;
	if( rsName == g_MyInfo.GetPublicID() )
	{
		if( bOn )
			iMsgType = ID_VOICE_ON;
		else
			iMsgType = ID_VOICE_OFF;
	}
	else
	{
		if( bOn )
			iMsgType = ID_VOICE_PERMIT;
		else
			iMsgType = ID_VOICE_REFUSE;
	}

	if( iMsgType == ID_VOICE_ON ||
		iMsgType == ID_VOICE_OFF )
	{
		if( !m_pPlayStage )
			return;

		if( g_BattleRoomMgr.IsBattleRoom() ||
			g_LadderTeamMgr.IsLadderTeam() ||
			m_pPlayStage->IsCustomPlazaMode() ||
			m_pPlayStage->GetModeType() == MT_HEADQUARTERS ||
			m_pPlayStage->GetModeType() == MT_HOUSE ) 
		{
			SP2Packet kPacket( CTPK_VOICE_INFO );
			kPacket << iMsgType;
			ioHashString szEmptyString; // 빈스트링이면 모두에게 전송
			kPacket << szEmptyString;
			TCPNetwork::SendToServer( kPacket );
		}

		if( iMsgType == ID_VOICE_ON )
		{
			#ifndef SHIPPING
			LOG.PrintTimeAndLog( 0, "%s CTPK_VOICE_INFO | ID_VOICE_ON |", __FUNCTION__ );
			#endif
			g_VoiceChat.StartCapture();
		}
		else if( iMsgType == ID_VOICE_OFF )
		{
			#ifndef SHIPPING
			LOG.PrintTimeAndLog( 0, "%s CTPK_VOICE_INFO | ID_VOICE_OFF |", __FUNCTION__);
			#endif
			g_VoiceChat.StopCapture();
		}
	}
	else if( iMsgType == ID_VOICE_PERMIT ||
		     iMsgType == ID_VOICE_REFUSE )
	{
		SP2Packet kPacket( CTPK_VOICE_INFO );
		kPacket << iMsgType;
		kPacket << rsName;
		TCPNetwork::SendToServer( kPacket );

		if( iMsgType == ID_VOICE_PERMIT )
		{
			#ifndef SHIPPING
			LOG.PrintTimeAndLog( 0, "%s CTPK_VOICE_INFO | ID_VOICE_PERMIT | %s", __FUNCTION__, rsName.c_str() );
			#endif
			g_VoiceChat.Stop(rsName.c_str());
			g_VoiceChat.Reset(rsName.c_str());
			g_VoiceDisableListManager.RemoveList( rsName );
		}
		else if( iMsgType == ID_VOICE_REFUSE )
		{
			#ifndef SHIPPING
			LOG.PrintTimeAndLog( 0, "%s CTPK_VOICE_INFO | ID_VOICE_REFUSE | %s", __FUNCTION__, rsName.c_str() );
			#endif
			g_VoiceDisableListManager.InsertList( rsName );
		}
	}
	else
		LOG.PrintTimeAndLog( 0, "%s error %d", __FUNCTION__, iMsgType );
}


void VoiceChatWnd::SetPlayStage( ioPlayStage *pPlayStage )
{
	m_pPlayStage = pPlayStage;
}

void VoiceChatWnd::SetAnnounceVoiceChat()
{
	if( !Setting::UseVoiceChat() ) return;
	if( !m_pPlayStage ) return;

	if( m_pPlayStage->GetModeType() == MT_MYROOM )
	{
		// no announce
	}
	else if( g_ShuffleRoomMgr.IsShuffleRoom() )
	{
		g_ChatMgr.SetSystemMsg( "오늘의모드에서는 음성채팅 불가능." );
	}
	else if( m_pPlayStage->GetModeType() != MT_HEADQUARTERS && 
		     m_pPlayStage->GetModeType() != MT_TRAINING	&& 
			 m_pPlayStage->GetModeType() != MT_HOUSE	&& 
			 !g_BattleRoomMgr.IsBattleRoom() && 
			 !g_LadderTeamMgr.IsLadderTeam() &&
			 !g_RankBattleMgr.IsRankBattlePlaying() )
	{
		g_ChatMgr.SetSystemMsg( STR(1) );
	}
	else if( m_pPlayStage->GetModeType() == MT_SURVIVAL && g_BattleRoomMgr.IsBattleRoom() )
	{
		if( g_GUIMgr.IsShow( PARTY_SHAM_BATTLE_WND ) || m_pPlayStage->GetVoiceChatType() == ioPlayStage::VCT_ALL )
			g_ChatMgr.SetSystemMsg( STR(2) );
		else
			g_ChatMgr.SetSystemMsg( STR(3) );
	}
	else if( m_pPlayStage->GetModeType() == MT_FIGHT_CLUB && g_BattleRoomMgr.IsBattleRoom() )
	{
		if( g_GUIMgr.IsShow( PARTY_SHAM_BATTLE_WND ) || m_pPlayStage->GetVoiceChatType() == ioPlayStage::VCT_ALL )
			g_ChatMgr.SetSystemMsg( STR(2) );
		else
			g_ChatMgr.SetSystemMsg( STR(10) );
	}
	else if( m_pPlayStage->GetModeType() != MT_HEADQUARTERS && m_pPlayStage->GetModeType() != MT_TRAINING && m_pPlayStage->GetModeType() != MT_HOUSE && g_BattleRoomMgr.IsBattleRoom() )
	{
		if( g_GUIMgr.IsShow( PARTY_SHAM_BATTLE_WND )  || m_pPlayStage->GetVoiceChatType() == ioPlayStage::VCT_ALL )
			g_ChatMgr.SetSystemMsg( STR(4) );
		else
			g_ChatMgr.SetSystemMsg( STR(5) );
	}
	else if( m_pPlayStage->GetModeType() != MT_HEADQUARTERS && m_pPlayStage->GetModeType() != MT_TRAINING && m_pPlayStage->GetModeType() != MT_HOUSE && g_LadderTeamMgr.IsLadderTeam() )
		g_ChatMgr.SetSystemMsg( STR(8) );
	else if( m_pPlayStage->GetModeType() == MT_HEADQUARTERS && !g_BattleRoomMgr.IsBattleRoom() && !g_LadderTeamMgr.IsLadderTeam() )
		g_ChatMgr.SetSystemMsg( "훈련소에서는 음성채팅 가능" );
	else if( m_pPlayStage->GetModeType() == MT_HOUSE && !g_BattleRoomMgr.IsBattleRoom() && !g_LadderTeamMgr.IsLadderTeam() )
		g_ChatMgr.SetSystemMsg( STR(9) );
	else if ( m_pPlayStage->IsCustomPlazaMode() && !g_BattleRoomMgr.IsBattleRoom() && !g_LadderTeamMgr.IsLadderTeam() )
		g_ChatMgr.SetSystemMsg( STR(6) );
	else if( !m_pPlayStage->IsCustomPlazaMode() && !g_BattleRoomMgr.IsBattleRoom() && !g_LadderTeamMgr.IsLadderTeam() )
		g_ChatMgr.SetSystemMsg( STR(7) );
}

void VoiceChatWnd::InitByProcess()
{
	if(m_bInit) 
		return;

	ShowWnd();
	UpdateScroll( true );
	m_bInit = true;

	m_vVoiceInfoVec.clear();
	g_App.ResetVoiceChatWithGUI();

	if(!Setting::UseVoiceChat())
		return;
	
	SetVoiceOn( g_MyInfo.GetPublicID() );
	SetAnnounceVoiceChat(); 
}

void VoiceChatWnd::UpdatePage( int iNewPage )
{
	m_iCurPage = iNewPage;

	for (int i = 0; i < MAX_ITEM ; i++)
		HideChildWnd( ID_ITEM_WND_1 + i );

	int iSize = m_vVoiceInfoVec.size();
	int iCnt  = 1;
	for (int i = 0; i < MAX_ITEM ; i++)
	{
		int iArray = (iSize - iCnt) - ( m_iMaxPage - m_iCurPage ); 
		if( !COMPARE( iArray, 0, iSize ) )
			break;
		VoiceChatItem *pItem = dynamic_cast<VoiceChatItem*> ( FindChildWnd( ID_ITEM_WND_2 - i) );
		if( pItem )
		{
			VoiceInfo &rkInfo = m_vVoiceInfoVec[iArray];
			pItem->ShowWnd();
			pItem->SetInfo( rkInfo.m_sName, rkInfo.m_bDisable  );
		}
		iCnt++;
	}
}

void VoiceChatWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_ITEM_WND_1:
	case ID_ITEM_WND_2:
		if(cmd == IOBN_BTNUP)
		{
			int iSize  = m_vVoiceInfoVec.size();
			int iArray = (iSize - 1) - ( m_iMaxPage - m_iCurPage ) + ( dwID-ID_ITEM_WND_2 );
			if( !COMPARE( iArray, 0, iSize ) )
				return;
			VoiceInfo kInfo = m_vVoiceInfoVec[iArray];
			
			ioLSMouse *pMouse = g_App.GetMouse();
			if( !pMouse ) 
				return;

			VoiceChatOptionWnd *pOptionWnd = dynamic_cast<VoiceChatOptionWnd*> ( g_GUIMgr.FindWnd( VOICE_CHAT_OPTION_WND ) );
			if( pOptionWnd )
			{
				pOptionWnd->ShowWnd();
				pOptionWnd->SetWndPos( pMouse->GetMousePos().x, pMouse->GetMousePos().y );
				pOptionWnd->SetInfo( kInfo.m_sName, kInfo.m_bDisable );
			}
		}
		break;
	}
}

void VoiceChatWnd::ChangeOptionAndSetVoice()
{
	GameOptionWnd *pOption = dynamic_cast<GameOptionWnd*> ( g_GUIMgr.FindWnd(GAME_OPTION_WND) );
	if(!pOption) 
		return;

	GameOptionVolumeTab *pVolume =	dynamic_cast<GameOptionVolumeTab*> ( pOption->FindChildWnd(GameOptionWnd::ID_VOLUME_TAB_WND) );
	if( !pVolume )
		return;

	pVolume->ChangeMic(); // 옵션이 변경되면서 SetVoiceXXX() 하게됨.
}

void VoiceChatWnd::SetVoiceDisableEnable( const ioHashString &rkID )
{
	if( IsDisable( rkID ) )
		SetVoiceEnable( rkID );
	else
		SetVoiceDisable( rkID );
}

void VoiceChatWnd::iwm_vscroll( DWORD dwID, int curPos )
{
	UpdatePage( curPos );
}

void VoiceChatWnd::iwm_wheel( int zDelta )
{
	if( !IsShow() )
		ioWnd::iwm_wheel( zDelta );
	else
	{ 
		// 휠 이벤트 처리를 했다면 부모로 이벤트를 날리지 않는다.
		if( zDelta == WHEEL_DELTA )
		{
			ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
			if( pScroll )
				pScroll->SetScrollPos( m_iCurPage - 1 );
			if( g_GUIMgr.GetPreOverWnd() )
			{
				ioMouse NullMouse;
				g_GUIMgr.GetPreOverWnd()->iwm_mouseover( NullMouse );
			}
		}
		else if( zDelta == -WHEEL_DELTA )
		{
			ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
			if( pScroll )
				pScroll->SetScrollPos( m_iCurPage + 1 );
			if( g_GUIMgr.GetPreOverWnd() )
			{
				ioMouse NullMouse;
				g_GUIMgr.GetPreOverWnd()->iwm_mouseover( NullMouse );
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<> ioVoiceDisableListManager* Singleton< ioVoiceDisableListManager >::ms_Singleton = 0;

ioVoiceDisableListManager::ioVoiceDisableListManager(void)
{
	m_vIDList.clear();
	m_bSave = false;
	m_vIDList.reserve(MAX_LIST);
}

ioVoiceDisableListManager::~ioVoiceDisableListManager(void)
{
	m_vIDList.clear();
}

void ioVoiceDisableListManager::LoadList()
{
	if(g_MyInfo.GetPublicID().IsEmpty()) return;

	char fName[MAX_PATH]="";
	StringCbPrintf( fName, sizeof(fName), "Save\\%s\\VoiceDisableList.ini",g_MyInfo.GetPublicID().c_str() );

	ioINILoader kLoader( fName, false );
	kLoader.SetTitle_e( "VoiceDisableList" );
	const int iMax = kLoader.LoadInt("Max", 0);

	for (int i = 0; i < iMax ; i++)
	{
		if(i >= MAX_LIST) break;

		char szKeyName[MAX_PATH]="";
		StringCbPrintf(szKeyName, sizeof(szKeyName), "%d", i+1);
		char szBuf[MAX_PATH]="";
		kLoader.LoadString(szKeyName, "", szBuf, sizeof(szBuf));
		if(strcmp(szBuf, "") == 0) continue;

		m_vIDList.push_back(szBuf);
	}
}

void ioVoiceDisableListManager::SaveList()
{
	if(!m_bSave) return;
	if(g_MyInfo.GetPublicID().IsEmpty()) return;

	const int iMax = m_vIDList.size();

	char fName[MAX_PATH]="";
	StringCbPrintf( fName, sizeof(fName), "Save\\%s\\VoiceDisableList.ini",g_MyInfo.GetPublicID().c_str() );

	ioINILoader kLoader( fName, false );
	kLoader.SetTitle_e( "VoiceDisableList" );
	kLoader.SaveInt("Max", iMax);

	for (int i = 0; i < iMax ; i++)
	{
		char szKeyName[MAX_PATH]="";
		StringCbPrintf(szKeyName, sizeof(szKeyName), "%d", i+1);
		kLoader.SaveString(szKeyName, m_vIDList[i].c_str());
	}
}

bool ioVoiceDisableListManager::InsertList( const ioHashString &szID )
{
	if( !g_App.IsRightID(szID.c_str()) ) return false;
	if( IsVoiceDisable( szID ) ) return false ;
	
	if( m_vIDList.size() >= MAX_LIST )
	{
		m_vIDList.erase( m_vIDList.begin() );
	}

	m_vIDList.push_back(szID);
	m_bSave = true;

	return true;
}


void ioVoiceDisableListManager::RemoveList( const ioHashString &szID )
{
	if( !g_App.IsRightID(szID.c_str()) ) return;

	ioHashStringVec::iterator iter = std::find( m_vIDList.begin(), m_vIDList.end(), szID );
	if( iter != m_vIDList.end() )
	{
		m_vIDList.erase( iter );
		m_bSave = true;
	}
}

bool ioVoiceDisableListManager::IsVoiceDisable( const ioHashString &szID )
{
	ioHashStringVec::iterator iter = std::find( m_vIDList.begin(), m_vIDList.end(), szID );
	if( iter != m_vIDList.end() )
		return true;
	else 
		return false;
}

ioVoiceDisableListManager& ioVoiceDisableListManager::GetSingleton()
{
	return Singleton< ioVoiceDisableListManager >::GetSingleton();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<> ioVoiceOffListManager* Singleton< ioVoiceOffListManager >::ms_Singleton = 0;

ioVoiceOffListManager::ioVoiceOffListManager()
{
	InitializeCriticalSection(&m_CriticalSection);
	m_OffInfoVec.clear();
}

ioVoiceOffListManager::~ioVoiceOffListManager()
{
	m_OffInfoVec.clear();
	DeleteCriticalSection(&m_CriticalSection);
}

void ioVoiceOffListManager::Add( const ioHashString &rszID )
{
	EnterCriticalSection(&m_CriticalSection);
	OffInfo kOffInfo;
	kOffInfo.m_szID      = rszID;
	kOffInfo.m_dwOffTime = FRAMEGETTIME();
	m_OffInfoVec.push_back( kOffInfo );
	LeaveCriticalSection(&m_CriticalSection);
}

void ioVoiceOffListManager::Delete( const ioHashString &rszID )
{
	EnterCriticalSection(&m_CriticalSection);
	for(OffInfoVec::iterator it = m_OffInfoVec.begin(); it != m_OffInfoVec.end(); ++it)
	{
		OffInfo &rOffInfo = (*it);
		if( rOffInfo.m_szID == rszID )
		{
			m_OffInfoVec.erase( it );
			LeaveCriticalSection(&m_CriticalSection);
			return;
		}
	}
	LeaveCriticalSection(&m_CriticalSection);
}

void ioVoiceOffListManager::ProcessDelete()
{
	EnterCriticalSection(&m_CriticalSection);
	if( m_OffInfoVec.empty() ) 
	{
		LeaveCriticalSection(&m_CriticalSection);
		return;
	}
	DWORD dwCurTime = FRAMEGETTIME();
	OffInfoVec::iterator iter = m_OffInfoVec.begin();
	while( iter != m_OffInfoVec.end() )
	{
		OffInfo &rOffInfo = (*iter);
		if( dwCurTime - rOffInfo.m_dwOffTime > LIVE_TIME )
			m_OffInfoVec.erase( iter );
		else
			++iter;
	}
	LeaveCriticalSection(&m_CriticalSection);
}

void ioVoiceOffListManager::DeleteAll()
{
	EnterCriticalSection(&m_CriticalSection);
	m_OffInfoVec.clear();
	LeaveCriticalSection(&m_CriticalSection);
}

bool ioVoiceOffListManager::IsHave( const ioHashString &rszID )
{
	EnterCriticalSection(&m_CriticalSection);
	for(OffInfoVec::iterator it = m_OffInfoVec.begin(); it != m_OffInfoVec.end(); ++it)
	{
		OffInfo &rOffInfo = (*it);
		if( rOffInfo.m_szID == rszID )
		{
			LeaveCriticalSection(&m_CriticalSection);
			return true;
		}
	}

	LeaveCriticalSection(&m_CriticalSection);
	return false;
}


ioVoiceOffListManager & ioVoiceOffListManager::GetSingleton()
{
	return Singleton< ioVoiceOffListManager >::GetSingleton();
}

