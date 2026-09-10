

#include "StdAfx.h"

#include "../io3DEngine/ioWnd.h"
#include "../io3DEngine/ioScroll.h"
#include "../io3DEngine/ioEdit.h"
#include "../io3DEngine/ioButton.h"
#include "../io3DEngine/InputBox.h"
#include "../io3DEngine/ioUIImageSetManager.h"
#include "../io3DEngine/ioUIRenderImage.h"
#include "../io3DEngine/ioUIRenderFrame.h"
#include "../io3DEngine/ioFontManager.h"
#include "../io3DEngine/ioSoundManager.h"
#include "../io3DEngine/ioStringConverter.h"
#include "../io3DEngine/ioXMLElement.h"
#include "../io3DEngine/iostringmanager.h"
#include "../io3DEngine/ioMovingWnd.h"

#include "../TextColorTable.h"
#include "../Setting.h"
#include "../ioApplication.h"
#include "../ioMyInfo.h"
#include "../ioKeyManager.h"
#include "../DirectInput.h"
#include "../ioBattleRoomMgr.h"
#include "../NetworkWrappingFunc.h"
#include "../WndID.h"
#include "../ioMovieMaker.h"
#include "../EtcHelpFunc.h"
#include "../ioPlayStage.h"
#include "../ioPlayMode.h"
#include "../ioMyRoomMode.h"
#include "../ioBaseChar.h"

#include "ioSP2GUIManager.h"
#include "ioInterfaceBackground.h"


#include "FileListWnd.h"
#include "ioMessageBox.h"
#include "GameOptionWnd.h"
#include "ResolutionList.h"
#include "GameOptionSettingJoyKeyTab.h"
#include "GameOptionSettingTab.h"

GameOptionBasicWnd::GameOptionBasicWnd()
{
	m_DefaultSpec   = Setting::GetSpec();
	m_CurrentOption = Setting::GetSettingOption();
}

GameOptionBasicWnd::~GameOptionBasicWnd()
{
}

bool GameOptionBasicWnd::ApplyChangedOption()
{
	bool bOptionChanged = false;
	bool bExceptResolution = false;

	if( Setting::GetSpec() != m_DefaultSpec )
	{
		Setting::SetSpec( m_DefaultSpec );
		bOptionChanged = true;
	}

	if( m_CurrentOption.IsOptionChanged( Setting::GetSettingOption() ) )
	{
		SettingOption kBackUpOption = Setting::GetSettingOption();

		if( g_App.ApplyOptionChange( m_CurrentOption ) )
		{
			bExceptResolution = CheckResolutionChange( kBackUpOption );
		}
		else
		{
			// 디바이스리셋이 실패했기때문에 리셋관련 옵션은 기존 옵션으로 되돌림

			if( !g_App.ApplyOptionChange( kBackUpOption ) )
			{
				// 되돌림조차 실패한다면...어쩔수 없을듯..
			}

			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		}

		m_CurrentOption = Setting::GetSettingOption();
		UpdateByCurrentOption();

		bOptionChanged = true;
	}

	if( bOptionChanged )
	{
		SaveCurrentSetting( bExceptResolution );
	}

	return bOptionChanged;
}

bool GameOptionBasicWnd::CheckResolutionChange( const SettingOption &rkPreOption )
{
	return false;
}

void GameOptionBasicWnd::SaveCurrentSetting( bool bExceptResolution )
{
#ifdef SHIPPING
	Setting::SaveSetting( g_MyInfo.GetPublicID(), bExceptResolution );
#else
	if( !P2PNetwork::IsNetworkPlaying() )
		Setting::SaveSetting( "character1", bExceptResolution );
	else
		Setting::SaveSetting( g_MyInfo.GetPublicID(), bExceptResolution );
#endif
}

void GameOptionBasicWnd::SetButtonCheck( DWORD dwID, bool bCheck )
{
	ioWnd *pWnd = FindChildWnd( dwID );
	if( pWnd && pWnd->GetWndType() == IWT_CHECK_BOX )
	{
		ioCheckButton *pCheck = dynamic_cast< ioCheckButton* >( pWnd );
		pCheck->SetCheck( bCheck );
	}	
}

void GameOptionBasicWnd::iwm_show()
{
	m_DefaultSpec   = Setting::GetSpec();
	m_CurrentOption = Setting::GetSettingOption();

	UpdateByCurrentOption();
}

// ----------------------------------------------------------------------------------------------------------------------
GameOptionVolumeTab::GameOptionVolumeTab()
{
	m_iMicVolumePerMax = 0;
	
	m_pIconBack	 = NULL;
	m_pIconVolume= NULL;	
	m_pIconMic	 = NULL;
}

GameOptionVolumeTab::~GameOptionVolumeTab()
{
	SAFEDELETE( m_pIconBack );
	SAFEDELETE( m_pIconVolume );
	SAFEDELETE( m_pIconMic );
}

void GameOptionVolumeTab::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "IconBack" )
	{
		SAFEDELETE( m_pIconBack );
		m_pIconBack = pImage;
	}
	else if( szType == "IconVolume" )
	{
		SAFEDELETE( m_pIconVolume );
		m_pIconVolume = pImage;

	}
	else if( szType == "IconMic" )
	{
		SAFEDELETE( m_pIconMic );
		m_pIconMic = pImage;
	}
	else
	{
		GameOptionBasicWnd::AddRenderImage( szType, pImage );
	}
}

void GameOptionVolumeTab::iwm_vscroll( DWORD dwID, int curPos )
{
	//#define DSBVOLUME_MIN               -10000
	//#define DSBVOLUME_MAX               0

	// 음량은 실시간 갱신
	if(dwID == ID_HORZ_SCROLL_BGM)
	{
		if(m_CurrentOption.m_bUseBGM) // 혹시 유저가 사용안함이후에 설정될지 몰라서.
		{
			Setting::SetBGMVolume(curPos);
			g_SoundMgr.SetVolumeStream( Help::GetDSVolume(curPos, Help::VT_STREAM) );
			g_SoundMgr.SetOggVolume( Help::GetDSVolume(curPos, Help::VT_STREAM) );
		}
	}
	else if(dwID == ID_HORZ_SCROLL_EFFECT)
	{
		if( m_CurrentOption.m_bUseEffectSound )
		{
			Setting::SetEffectSoundVolume(curPos);
			g_SoundMgr.SetVolumeNormal( Help::GetDSVolume(curPos, Help::VT_NORMAL) );
			g_SoundMgr.SetVolume3D ( Help::GetDSVolume(curPos, Help::VT_3D) );
		}
	}
	else if(dwID == ID_HORZ_SCROLL_VOICE)
	{
		if( m_CurrentOption.m_bUseOtherVoiceChat )
		{
			Setting::SetVoiceVolume(curPos);
			g_VoiceChat.SetAllVolume( Help::GetDSVolume(curPos, Help::VT_NONE) );
		}
	}
	else if(dwID==ID_HORZ_SCROLL_MIC) // 윈도우 마이크볼륨과 연동되므로 셋팅 변수에 저장하지 않음.
	{
		if(m_CurrentOption.m_bUseMIC)
		{
			int dwVolume = g_VoiceChat.GetRecoderMinimum() + ( m_iMicVolumePerMax * curPos);
			g_VoiceChat.SetRecoderVolumeValue(dwVolume);
		}
	}
	else if( dwID == ID_HORZ_SCROLL_CUSTOM )
	{
		if( m_CurrentOption.m_bUseCustomSound )
		{
			Setting::SetCustomSoundVolume( curPos );
			g_SoundMgr.SetVolumePiece( Help::GetDSVolume( curPos, Help::VT_NONE ));
		}
	}
}

void GameOptionVolumeTab::iwm_show()
{
	m_DefaultSpec   = Setting::GetSpec();
	m_CurrentOption = Setting::GetSettingOption();

	// scroll setting
	ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_HORZ_SCROLL_BGM );
	if( pScroll )
	{
		pScroll->SetScrollRange( 0, Help::MAX_VOLUME ); 
		pScroll->SetScrollPos( Setting::GetBGMVolume());
	}

	pScroll = (ioScroll*)FindChildWnd( ID_HORZ_SCROLL_EFFECT );
	if( pScroll )
	{
		pScroll->SetScrollRange( 0, Help::MAX_VOLUME ); 
		pScroll->SetScrollPos( Setting::GetEffectSoundVolume());
	}

	pScroll = (ioScroll*)FindChildWnd( ID_HORZ_SCROLL_VOICE );
	if( pScroll )
	{
		pScroll->SetScrollRange( 0, Help::MAX_VOLUME ); 
		pScroll->SetScrollPos( Setting::GetVoiceVolume());
	}

	pScroll = (ioScroll*)FindChildWnd( ID_HORZ_SCROLL_CUSTOM );
	if( pScroll )
	{
		pScroll->SetScrollRange( 0, Help::MAX_VOLUME ); 
		pScroll->SetScrollPos( Setting::GetCustomSoundVolume());
	}


	if(g_VoiceChat.IsWorkRecordVolume())
	{
		ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_HORZ_SCROLL_MIC );
		if( pScroll )
		{
			pScroll->SetScrollRange( 0, Help::MAX_VOLUME ); 

			m_iMicVolumePerMax = (g_VoiceChat.GetRecoderMaximum() - g_VoiceChat.GetRecoderMinimum()) 
				/ (Help::MAX_VOLUME-1);
			DWORD dwVolume = 0;
			g_VoiceChat.GetRecoderVolumeValue(dwVolume);
			pScroll->SetScrollPos( (dwVolume+g_VoiceChat.GetRecoderMinimum()) % (Help::MAX_VOLUME));
		}
	}
	else
	{
		SetChildInActive(ID_MIC_BUTTON);
		SetChildInActive(ID_HORZ_SCROLL_MIC);
	}
	//

	ioWnd *pWnd = dynamic_cast<ioWnd*> ( FindChildWnd(ID_VOICE_CHAT) );
	if( g_BattleRoomMgr.IsObserverUser( g_MyInfo.GetPublicID() ) )
	{
		SetChildInActive( ID_VOICE_CHAT );
		if(pWnd)
		{
			pWnd->SetTitleText(STR(1));
		}
	}
	else if( g_MyInfo.GetBlockType() == BKT_LIMITATION )
	{
		SetChildInActive( ID_VOICE_CHAT );
		if(pWnd)
		{
			pWnd->SetTitleText(STR(2));
		}
	}
	else
	{
		SetChildActive( ID_VOICE_CHAT );
		if(pWnd)
		{
			pWnd->SetTitleText( STR(3) );
		}
	}
	
	SetChildInActive( ID_APPLY );
	UpdateByCurrentOption();
}


void GameOptionVolumeTab::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_BGM_BUTTON:
		if( cmd == IOBN_BTNUP )
		{
			m_CurrentOption.m_bUseBGM = !m_CurrentOption.m_bUseBGM;
			ApplyChangedOption();
		}
		break;
	case ID_EFFECT_BUTTON:
		if( cmd == IOBN_BTNUP )
		{
			m_CurrentOption.m_bUseEffectSound = !m_CurrentOption.m_bUseEffectSound;
			ApplyChangedOption();
		}
		break;
	case ID_OTHER_VOICE_BUTTON:
		if( cmd == IOBN_BTNUP )
			ChangeOtherVoice();
		break;
	case ID_MIC_BUTTON:
		if( cmd == IOBN_BTNUP )
			ChangeMic();
		break;
	case ID_CUSTOM_BUTTON:
		if( cmd == IOBN_BTNUP )
		{
			m_CurrentOption.m_bUseCustomSound = !m_CurrentOption.m_bUseCustomSound;
			ApplyChangedOption();
		}
		break;
	case ID_VOICE_CHAT:
		if( cmd == IOBN_BTNUP )
		{
			m_CurrentOption.m_bUseVoiceChat = !m_CurrentOption.m_bUseVoiceChat;
			if(m_CurrentOption.m_bUseVoiceChat)
				m_CurrentOption.m_bUseMIC = true;
			ApplyChangedOption();
		}
		break;
	case ID_COMMIT:
		if( cmd == IOBN_BTNUP )
		{
			if(m_pParent)
				m_pParent->HideWnd();
		}
		break;
	case ID_CANCEL:
		if( cmd == IOBN_BTNUP )
		{
			if(m_pParent)
				m_pParent->HideWnd();
		}
		break;
	case ID_PLAY_KILL_SOUND_BUTTON:
		if( cmd == IOBN_BTNUP )
		{
			m_CurrentOption.m_bPlayKillSound = !m_CurrentOption.m_bPlayKillSound;
			ApplyChangedOption();
		}
		break;
	case ID_PLAY_COMBO_SOUND_BUTTON:
		if( cmd == IOBN_BTNUP )
		{
			m_CurrentOption.m_bPlayComboSound = !m_CurrentOption.m_bPlayComboSound;
			ApplyChangedOption();
		}
		break;
	case ID_PLAY_EXTEND_SOUND_BUTTON:
		if( cmd == IOBN_BTNUP )
		{
			m_CurrentOption.m_bPlayExtendSound = !m_CurrentOption.m_bPlayExtendSound;
			ApplyChangedOption();
		}
		break;
	}
}

void GameOptionVolumeTab::UpdateByCurrentOption()
{
	SetButtonCheck( ID_BGM_BUTTON, m_CurrentOption.m_bUseBGM );
	SetButtonCheck( ID_EFFECT_BUTTON, m_CurrentOption.m_bUseEffectSound );
	SetButtonCheck( ID_OTHER_VOICE_BUTTON, m_CurrentOption.m_bUseOtherVoiceChat );
	SetButtonCheck( ID_MIC_BUTTON, m_CurrentOption.m_bUseMIC );
	SetButtonCheck( ID_VOICE_CHAT, m_CurrentOption.m_bUseVoiceChat );
	SetButtonCheck( ID_CUSTOM_BUTTON, m_CurrentOption.m_bUseCustomSound );
	SetButtonCheck( ID_PLAY_KILL_SOUND_BUTTON, m_CurrentOption.m_bPlayKillSound );
	SetButtonCheck( ID_PLAY_COMBO_SOUND_BUTTON, m_CurrentOption.m_bPlayComboSound );
	SetButtonCheck( ID_PLAY_EXTEND_SOUND_BUTTON, m_CurrentOption.m_bPlayExtendSound );

	if( m_CurrentOption.m_bUseBGM )
		SetChildActive(ID_HORZ_SCROLL_BGM);
	else
		SetChildInActive(ID_HORZ_SCROLL_BGM);

	if( m_CurrentOption.m_bUseEffectSound )
		SetChildActive(ID_HORZ_SCROLL_EFFECT);
	else
		SetChildInActive(ID_HORZ_SCROLL_EFFECT);

	if( m_CurrentOption.m_bUseOtherVoiceChat )
		SetChildActive(ID_HORZ_SCROLL_VOICE);
	else
		SetChildInActive(ID_HORZ_SCROLL_VOICE);

	if( m_CurrentOption.m_bUseVoiceChat )
	{
		SetChildActive( ID_MIC_BUTTON );

		if( m_CurrentOption.m_bUseMIC )
			SetChildActive(ID_HORZ_SCROLL_MIC);
		else
			SetChildInActive(ID_HORZ_SCROLL_MIC);
	}
	else
	{
		SetButtonCheck( ID_MIC_BUTTON, false );
		SetChildInActive( ID_MIC_BUTTON );
		SetChildInActive(ID_HORZ_SCROLL_MIC);
	}

	if( m_CurrentOption.m_bUseCustomSound )
		SetChildActive( ID_HORZ_SCROLL_CUSTOM );
	else
		SetChildInActive( ID_HORZ_SCROLL_CUSTOM );
}


void GameOptionVolumeTab::ChangeMic()
{
	if(IsShow()) 
	{
		m_CurrentOption.m_bUseMIC = !m_CurrentOption.m_bUseMIC;
		ApplyChangedOption();
	}
	else
	{
		// 초기화
		m_DefaultSpec   = Setting::GetSpec();
		m_CurrentOption = Setting::GetSettingOption();
		m_CurrentOption.m_bUseMIC = !m_CurrentOption.m_bUseMIC;
		ApplyChangedOption();
	}
}

void GameOptionVolumeTab::ChangeOtherVoice()
{
	if(IsShow()) 
	{
		m_CurrentOption.m_bUseOtherVoiceChat = !m_CurrentOption.m_bUseOtherVoiceChat;
		ApplyChangedOption();
	}
	else
	{
		// 초기화
		m_DefaultSpec   = Setting::GetSpec();
		m_CurrentOption = Setting::GetSettingOption();
		m_CurrentOption.m_bUseOtherVoiceChat = !m_CurrentOption.m_bUseOtherVoiceChat;
		ApplyChangedOption();
	}
}

void GameOptionVolumeTab::iwm_hide()
{
	ApplyChangedOption();
}

void GameOptionVolumeTab::iwm_create()
{
	if( g_MyInfo.GetBlockType() == BKT_LIMITATION )
		Setting::SetUseVoiceChat( false );
}

void GameOptionVolumeTab::ParseExtraInfo( ioXMLElement &xElement )
{
}

void GameOptionVolumeTab::OnRender()
{
	GameOptionBasicWnd::OnRender();
	
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();
	if( m_pIconBack )
	{
		m_pIconBack->SetScale( 0.30f );
		m_pIconBack->Render( iXPos + 30, iYPos + 27, UI_RENDER_MULTIPLY, TFO_BILINEAR );
		m_pIconBack->Render( iXPos + 30, iYPos + 219, UI_RENDER_MULTIPLY, TFO_BILINEAR );
	}
	if( m_pIconVolume )
	{
		m_pIconVolume->SetScale( 0.30f );
		m_pIconVolume->Render( iXPos + 30, iYPos + 27, UI_RENDER_NORMAL, TFO_BILINEAR );
	}
	if( m_pIconMic )
	{
		m_pIconMic->SetScale( 0.30f );
		m_pIconMic->Render( iXPos + 30, iYPos + 219, UI_RENDER_NORMAL, TFO_BILINEAR );
	}

	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 44, iYPos + 19, FONT_SIZE_14, "볼륨 설정" );
	g_FontMgr.PrintText( iXPos + 44, iYPos + 211, FONT_SIZE_14, "음성채팅 설정" );
}
//------------------------------------------------------------------------------------------------------------------------
GameSpecRadioBtn::GameSpecRadioBtn()
{
	m_pChecked = NULL;
}

GameSpecRadioBtn::~GameSpecRadioBtn()
{
	SAFEDELETE( m_pChecked );
}

void GameSpecRadioBtn::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "Checked" )
	{
		SAFEDELETE( m_pChecked );
		m_pChecked = pImage;
	}
	else
	{
		ioButton::AddRenderImage( szType, pImage );
	}
}

void GameSpecRadioBtn::OnRender()
{
	ioButton::OnRender();

	if( IsDisabled() || IsExDisabled() )
		return;

	GameOptionSpecTab *pParent = dynamic_cast<GameOptionSpecTab*>( GetParent() );
	if( pParent && pParent->GetDefaultSpecButtonID() == GetID() )
	{
		if( m_pChecked )
			m_pChecked->Render( GetDerivedPosX(), GetDerivedPosY() );
	}
}

GameOptionSpecTab::GameOptionSpecTab()
{
	m_iRestoreWidth  = 0;
	m_iRestoreHeight = 0;

	m_pPlayStage = NULL;
	m_pDot		 = NULL;
}

GameOptionSpecTab::~GameOptionSpecTab()
{
	SAFEDELETE( m_pDot );
}

void GameOptionSpecTab::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( strcmp(szType.c_str(), "Dot") == 0 )
	{
		SAFEDELETE( m_pDot );
		m_pDot = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

DWORD GameOptionSpecTab::GetDefaultSpecButtonID()
{
	switch( m_DefaultSpec )
	{
	case Setting::VERY_HIGH_SPEC:
		return ID_VERY_HIGH_SPEC;
	case Setting::HIGH_SPEC:
		return ID_HIGH_SPEC;
	case Setting::MIDDLE_SPEC:
		return ID_MIDDLE_SPEC;
	case Setting::LOW_SPEC:
		return ID_LOW_SPEC;
	case Setting::MINIMUM_SPEC:
		return ID_MINIMUM_SPEC;
	case Setting::CUSTOM_SPEC:
		return ID_CUSTOM_SPEC;
	}
	return 0;
}

void GameOptionSpecTab::SetAntiOptionEnable( bool bEnable )
{
	SetChildInActive( ID_ANTI_ALIASING_HIGH );
	SetChildInActive( ID_ANTI_ALIASING_MIDDLE );
	SetChildInActive( ID_ANTI_ALIASING_LOW );

	if( bEnable )
	{
		int iMaxCount = Setting::GetEnableMultiSampleCount();
		
		if( iMaxCount >= 3 )
			SetChildActive( ID_ANTI_ALIASING_HIGH );

		if( iMaxCount >= 2 )
			SetChildActive( ID_ANTI_ALIASING_MIDDLE );

		if( iMaxCount >= 1 )
			SetChildActive( ID_ANTI_ALIASING_LOW );
	}
}

void GameOptionSpecTab::UpdateByCurrentOption()
{
	if( Setting::GetEnableMultiSampleCount() <= 0 )
	{
		SetChildInActive( ID_VERY_HIGH_SPEC );
	}
	if( Setting::GetPixelShaderVersion() < 20 )
	{
		SetChildInActive( ID_VERY_HIGH_SPEC );
		SetChildInActive( ID_HIGH_SPEC );
	}
	if( Setting::GetPixelShaderVersion() < 10 )
	{
		SetChildInActive( ID_MIDDLE_SPEC );
		SetChildInActive( ID_LOW_SPEC );
	}

	UpdateScreenBoxText();

	SetButtonCheck( ID_SCREEN_GLOW, m_CurrentOption.m_bGlow );
	SetButtonCheck( ID_TRAIL_GLOW, m_CurrentOption.m_bTrailGlow );
	SetButtonCheck( ID_MAP_EFFECT, m_CurrentOption.m_bShowMapEffect );
	SetButtonCheck( ID_SHADOW, m_CurrentOption.m_bShadow );
	SetButtonCheck( ID_16BIT_FRAMEBUFFER, m_CurrentOption.m_bUse16bitFrameBuffer );
	SetButtonCheck( ID_SOFT_SKINNING, m_CurrentOption.m_bSoftSkinning );
	SetButtonCheck( ID_MAP_EDGE, m_CurrentOption.m_bUseMapEdge );
	SetButtonCheck( ID_OBJECT_EDGE, m_CurrentOption.m_bUseObjectEdge );
	SetButtonCheck( ID_SPEC_AUTO_CHECK, m_CurrentOption.m_bSpecAutoCheck );
	SetButtonCheck( ID_MOVIE_MODE, m_CurrentOption.m_bSwapCopy );

	SetAntiOptionEnable( true );
	switch( m_CurrentOption.m_AntiAliasingQuality )
	{
	case AQ_HIGH:
		CheckRadioButton( ID_ANTI_ALIASING_QUALITY, ID_ANTI_ALIASING_LOW, ID_ANTI_ALIASING_HIGH );	
		break;
	case AQ_MIDDLE:
		CheckRadioButton( ID_ANTI_ALIASING_QUALITY, ID_ANTI_ALIASING_LOW, ID_ANTI_ALIASING_MIDDLE );
		break;
	case AQ_LOW:
		CheckRadioButton( ID_ANTI_ALIASING_QUALITY, ID_ANTI_ALIASING_LOW, ID_ANTI_ALIASING_LOW );
		break;
	case AQ_NONE:
		CheckRadioButton( ID_ANTI_ALIASING_QUALITY, ID_ANTI_ALIASING_LOW, ID_ANTI_ALIASING_QUALITY );
		break;
	}

	switch( m_CurrentOption.m_LightQuality )
	{
	case LQ_HIGH:
		CheckRadioButton( ID_LIGHT_HIGH, ID_LIGHT_LOW, ID_LIGHT_HIGH );
		break;
	case LQ_MIDDLE:
		CheckRadioButton( ID_LIGHT_HIGH, ID_LIGHT_LOW, ID_LIGHT_MIDDLE );
		break;
	case LQ_LOW:
		CheckRadioButton( ID_LIGHT_HIGH, ID_LIGHT_LOW, ID_LIGHT_LOW );
		break;
	}

	switch( m_CurrentOption.m_TextureQuality )
	{
	case TQ_HIGH:
		CheckRadioButton( ID_TEXTURE_HIGH, ID_TEXTURE_LOW, ID_TEXTURE_HIGH );
		break;
	case TQ_MIDDLE:
		CheckRadioButton( ID_TEXTURE_HIGH, ID_TEXTURE_LOW, ID_TEXTURE_MIDDLE );
		break;
	case TQ_LOW:
		CheckRadioButton( ID_TEXTURE_HIGH, ID_TEXTURE_LOW, ID_TEXTURE_LOW );
		break;
	}

	switch( m_CurrentOption.m_EffectQuality )
	{
	case EQ_HIGH:
		CheckRadioButton( ID_EFFECT_HIGH, ID_EFFECT_LOW, ID_EFFECT_HIGH );
		break;
	case EQ_MIDDLE:
		CheckRadioButton( ID_EFFECT_HIGH, ID_EFFECT_LOW, ID_EFFECT_MIDDLE );
		break;
	case EQ_LOW:
		CheckRadioButton( ID_EFFECT_HIGH, ID_EFFECT_LOW, ID_EFFECT_LOW );
		break;
	}

	switch( m_CurrentOption.m_MapObjectQuality )
	{
	case MQ_HIGH:
		CheckRadioButton( ID_MAP_OBJECT_HIGH, ID_MAP_OBJECT_LOW, ID_MAP_OBJECT_HIGH );
		break;
	case MQ_MIDDLE:
		CheckRadioButton( ID_MAP_OBJECT_HIGH, ID_MAP_OBJECT_LOW, ID_MAP_OBJECT_MIDDLE );
		break;
	case MQ_LOW:
		CheckRadioButton( ID_MAP_OBJECT_HIGH, ID_MAP_OBJECT_LOW, ID_MAP_OBJECT_LOW );
		break;
	}

	if( Setting::GetPixelShaderVersion() < 20 )
	{
		SetChildInActive( ID_SCREEN_GLOW );
		SetChildInActive( ID_TRAIL_GLOW );
		SetChildInActive( ID_SHADOW );
		SetChildInActive( ID_FILTER_BOX );
	}

	if( Setting::GetPixelShaderVersion() < 10 )
	{
		SetChildInActive( ID_LIGHT_HIGH );
		SetChildInActive( ID_LIGHT_MIDDLE );
	}

	ChangeResolutionText( m_CurrentOption.m_iWidth, m_CurrentOption.m_iHeight );

	UpdateFilterBoxText();
	UpdateOverayBoxText();

	SetChildInActive( ID_APPLY );

	bool bCheckMode = false;
	if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
	{
		if( ioPlayMode::GetModeType() == MT_MYROOM )
		{
			bCheckMode = true;
		}		
	}
	else
	{
		if( ioPlayMode::GetModeType() == MT_MYROOM || ioPlayMode::GetModeType() == MT_TRAINING || ioPlayMode::GetModeType() == MT_HEADQUARTERS || ioPlayMode::GetModeType() == MT_HOUSE )
		{
			bCheckMode = true;
		}
	}

	if( bCheckMode )
	{		
		switch( ioMyRoomMode::GetMyRoomType() )
		{
		case MRT_LOBBY:
		case MRT_PRACTICE:		
			{
				SetChildWndStyleRemove( ID_BACKGROUND_CHANGE, IWS_EXACTIVE );
			}
			break;
		case MRT_FIND_MODE:
			{
				if( g_App.GetConnectedMoveMode() != ioApplication::CMM_LOBBY )
				{
					SetChildWndStyleRemove( ID_BACKGROUND_CHANGE, IWS_EXACTIVE );
				}
			}
			break;
		default:
			{
				SetChildWndStyleAdd( ID_BACKGROUND_CHANGE, IWS_EXACTIVE );
			}
			break;
		}
	}
	else
	{
		SetChildWndStyleAdd( ID_BACKGROUND_CHANGE, IWS_EXACTIVE );
	}
}

bool GameOptionSpecTab::CheckResolutionChange( const SettingOption &rkPreOption )
{
	if( !m_CurrentOption.m_bFullScreen )
		return false;

	m_iRestoreWidth  = rkPreOption.m_iWidth;
	m_iRestoreHeight = rkPreOption.m_iHeight;
	m_bRestoreFullScreen = rkPreOption.m_bFullScreen;

	if( m_iRestoreWidth != m_CurrentOption.m_iWidth ||
		m_iRestoreHeight != m_CurrentOption.m_iHeight ||
		m_bRestoreFullScreen != m_CurrentOption.m_bFullScreen )	// 이전에 창모드였다.
	{
		ResolutionConfirm *pConfirm = dynamic_cast<ResolutionConfirm*> (g_GUIMgr.FindWnd( RESOLUTION_CONFIRM_WND ));
		if( pConfirm )
		{
			pConfirm->OpenConfirm( this, m_CurrentOption.m_iWidth, m_CurrentOption.m_iHeight );
		}

		return true;	// 해상도는 지금 저장하지 말자.
	}

	return false;
}

void GameOptionSpecTab::SetCustomSpec()
{
	m_DefaultSpec = Setting::CUSTOM_SPEC;
}

void GameOptionSpecTab::UnCheckAutoSpec()
{
	m_CurrentOption.m_bSpecAutoCheck = false;
	SetButtonCheck( ID_SPEC_AUTO_CHECK, false );
}

void GameOptionSpecTab::SetSpecOption( DWORD dwID )
{
	switch( dwID )
	{
	case ID_VERY_HIGH_SPEC:
		m_DefaultSpec = Setting::VERY_HIGH_SPEC;
		break;
	case ID_HIGH_SPEC:
		m_DefaultSpec = Setting::HIGH_SPEC;
		break;
	case ID_MIDDLE_SPEC:
		m_DefaultSpec = Setting::MIDDLE_SPEC;
		break;
	case ID_LOW_SPEC:
		m_DefaultSpec = Setting::LOW_SPEC;
		break;
	case ID_MINIMUM_SPEC:
		m_DefaultSpec = Setting::MINIMUM_SPEC;
		break;
	case ID_CUSTOM_SPEC:
		m_DefaultSpec = Setting::CUSTOM_SPEC;
		break;
	}

	if( m_DefaultSpec != Setting::CUSTOM_SPEC )
	{
		Setting::SetOptionBySpec( m_CurrentOption, m_DefaultSpec );
	}

	UnCheckAutoSpec();

	UpdateByCurrentOption();
	SetChildActive( ID_APPLY );
}

void GameOptionSpecTab::SetOnOffOption( bool &bOption )
{
	bOption = !bOption;

	SetCustomSpec();
	UnCheckAutoSpec();

	SetChildActive( ID_APPLY );
}

void GameOptionSpecTab::SetAntiAliasingOption( DWORD dwID )
{
	CheckRadioButton( ID_ANTI_ALIASING_QUALITY, ID_ANTI_ALIASING_LOW, dwID );

	switch( dwID )
	{
	case ID_ANTI_ALIASING_QUALITY:
		m_CurrentOption.m_AntiAliasingQuality = AQ_NONE;
		break;
	case ID_ANTI_ALIASING_HIGH:
		m_CurrentOption.m_AntiAliasingQuality = AQ_HIGH;
		break;
	case ID_ANTI_ALIASING_MIDDLE:
		m_CurrentOption.m_AntiAliasingQuality = AQ_MIDDLE;
		break;
	case ID_ANTI_ALIASING_LOW:
		m_CurrentOption.m_AntiAliasingQuality = AQ_LOW;
		break;
	}

	if( m_CurrentOption.m_AntiAliasingQuality != AQ_NONE )
	{
		if( m_CurrentOption.m_bSwapCopy )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
			SetAntiAliasingOption( ID_ANTI_ALIASING_QUALITY );
			return;
		}
	}

	SetCustomSpec();
	UnCheckAutoSpec();
	SetChildActive( ID_APPLY );
}

void GameOptionSpecTab::SetTextureOption( DWORD dwID )
{
	CheckRadioButton( ID_TEXTURE_HIGH, ID_TEXTURE_LOW, dwID );

	switch( dwID )
	{
	case ID_TEXTURE_HIGH:
		m_CurrentOption.m_TextureQuality = TQ_HIGH;
		break;
	case ID_TEXTURE_MIDDLE:
		m_CurrentOption.m_TextureQuality = TQ_MIDDLE;
		break;
	case ID_TEXTURE_LOW:
		m_CurrentOption.m_TextureQuality = TQ_LOW;
		break;
	}

	SetCustomSpec();
	UnCheckAutoSpec();

	SetChildActive( ID_APPLY );
}

void GameOptionSpecTab::SetLightOption( DWORD dwID )
{
	CheckRadioButton( ID_LIGHT_HIGH, ID_LIGHT_LOW, dwID );

	switch( dwID )
	{
	case ID_LIGHT_HIGH:
		m_CurrentOption.m_LightQuality = LQ_HIGH;
		break;
	case ID_LIGHT_MIDDLE:
		m_CurrentOption.m_LightQuality = LQ_MIDDLE;
		break;
	case ID_LIGHT_LOW:
		m_CurrentOption.m_LightQuality = LQ_LOW;
		break;
	}

	SetCustomSpec();
	UnCheckAutoSpec();

	SetChildActive( ID_APPLY );
}

void GameOptionSpecTab::SetEffectOption( DWORD dwID )
{
	CheckRadioButton( ID_EFFECT_HIGH, ID_EFFECT_LOW, dwID );

	switch( dwID )
	{
	case ID_EFFECT_HIGH:
		m_CurrentOption.m_EffectQuality = EQ_HIGH;
		break;
	case ID_EFFECT_MIDDLE:
		m_CurrentOption.m_EffectQuality = EQ_MIDDLE;
		break;
	case ID_EFFECT_LOW:
		m_CurrentOption.m_EffectQuality = EQ_LOW;
		break;
	}

	SetCustomSpec();
	UnCheckAutoSpec();

	SetChildActive( ID_APPLY );
}

void GameOptionSpecTab::SetMapObjectOption( DWORD dwID )
{
	CheckRadioButton( ID_MAP_OBJECT_HIGH, ID_MAP_OBJECT_LOW, dwID );

	switch( dwID )
	{
	case ID_MAP_OBJECT_HIGH:
		m_CurrentOption.m_MapObjectQuality = MQ_HIGH;
		break;
	case ID_MAP_OBJECT_MIDDLE:
		m_CurrentOption.m_MapObjectQuality = MQ_MIDDLE;
		break;
	case ID_MAP_OBJECT_LOW:
		m_CurrentOption.m_MapObjectQuality = MQ_LOW;
		break;
	}

    SetCustomSpec();
	UnCheckAutoSpec();

	SetChildActive( ID_APPLY );
}

void GameOptionSpecTab::ToggleMovieMode()
{
	if( m_CurrentOption.m_AntiAliasingQuality != AQ_NONE &&
		!m_CurrentOption.m_bSwapCopy )	// 현재 false인가..? => 기능을 켜면 true가 되므로
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );

		ioCheckButton *pCheck = dynamic_cast<ioCheckButton*>( FindChildWnd( ID_MOVIE_MODE ) );
		if( pCheck )
		{
			pCheck->SetCheck( false );
		}

		return;
	}

	m_CurrentOption.m_bSwapCopy = !m_CurrentOption.m_bSwapCopy;

	SetCustomSpec();
	UnCheckAutoSpec();
	SetChildActive( ID_APPLY );
}

void GameOptionSpecTab::OpenScreenList( ioWnd *pOpenBtn )
{
	StringSelectList *pList = (StringSelectList*)FindChildWnd( ID_SCREEN_LIST );
	if( pList )
	{
		StringVector vNameList;
		vNameList.reserve( 2 );
		vNameList.push_back( std::string( STR(1) ) );
		vNameList.push_back( std::string( STR(2) ) );
		pList->OpenList( pOpenBtn, vNameList, (int)(!m_CurrentOption.m_bFullScreen), IO_SCREEN_MODE_CHANGE );
	}
}

void GameOptionSpecTab::SetChangeScreen( int iIndex )
{
	HideChildWnd( ID_SCREEN_LIST );

	bool bFullScreen = ( (iIndex == 0) ? true : false );
	if( m_CurrentOption.m_bFullScreen == bFullScreen )
		return;

	m_CurrentOption.m_bFullScreen = bFullScreen;

	UpdateScreenBoxText();

	SetCustomSpec();
	UnCheckAutoSpec();

	SetChildActive( ID_APPLY );
}

void GameOptionSpecTab::UpdateScreenBoxText()
{
	ioWnd *pWnd = FindChildWnd( ID_SCREEN_BOX );
	if( !pWnd )	return;

	if( m_CurrentOption.m_bFullScreen )
	{
		pWnd->SetTitleText( STR(1) );
	}
	else
	{
		pWnd->SetTitleText( STR(2) );
	}
}

void GameOptionSpecTab::OpenResolutionList( ioWnd *pOpenBtn )
{
	ResolutionList *pList = (ResolutionList*)FindChildWnd( ID_RESOLUTION_LIST );
	if( pList )
	{
		pList->OpenList( pOpenBtn,
						 m_CurrentOption.m_iWidth,
						 m_CurrentOption.m_iHeight );
	}
}

void GameOptionSpecTab::OpenFilterList( ioWnd *pOpenBtn )
{
	StringSelectList *pList = (StringSelectList*)FindChildWnd( ID_FILTER_LIST );
	if( pList )
	{
		StringVector vNameList;
		vNameList.reserve( 7 );

		vNameList.push_back( std::string( STR(1) ) );
		vNameList.push_back( std::string( "GRAY 40" ) );
		vNameList.push_back( std::string( "GRAY 80" ) );
		vNameList.push_back( std::string( "GRAY 100" ) );
		vNameList.push_back( std::string( "SEPIA 40" ) );
		vNameList.push_back( std::string( "SEPIA 80" ) );
		vNameList.push_back( std::string( "SEPIA 100" ) );

		pList->OpenList( pOpenBtn, vNameList, (int)m_CurrentOption.m_FilterType, IO_FILTER_CHANGE );
	}
}

void GameOptionSpecTab::SetChangeFilter( int iIndex )
{
	HideChildWnd( ID_FILTER_LIST );

	if( !COMPARE( iIndex, FILTER_NONE, FILTER_MAX ) )
		return;

	FilterType eNewFilter = (FilterType)iIndex;
	if( m_CurrentOption.m_FilterType == eNewFilter )
		return;

	m_CurrentOption.m_FilterType = eNewFilter;

	UpdateFilterBoxText();

	SetCustomSpec();
	UnCheckAutoSpec();

	SetChildActive( ID_APPLY );
}

void GameOptionSpecTab::UpdateFilterBoxText()
{
	ioWnd *pWnd = FindChildWnd( ID_FILTER_BOX );
	if( !pWnd )	return;

	switch( m_CurrentOption.m_FilterType )
	{
	case FILTER_NONE:
		pWnd->SetTitleText( STR(1) );
		break;
	case FILTER_GRAY40:
		pWnd->SetTitleText( "GRAY 40" );
		break;
	case FILTER_GRAY80:
		pWnd->SetTitleText( "GRAY 80" );
		break;
	case FILTER_GRAY100:
		pWnd->SetTitleText( "GRAY 100" );
		break;
	case FILTER_SEPIA40:
		pWnd->SetTitleText( "SEPIA 40" );		
		break;
	case FILTER_SEPIA80:
		pWnd->SetTitleText( "SEPIA 80" );
		break;
	case FILTER_SEPIA100:
		pWnd->SetTitleText( "SEPIA 100" );		
		break;
	}
}

void GameOptionSpecTab::OpenOverayList( ioWnd *pOpenBtn )
{
	StringSelectList *pList = (StringSelectList*)FindChildWnd( ID_OVERAY_LIST );
	if( pList )
	{
		StringVector vNameList;
		vNameList.reserve( 3 );

		vNameList.push_back( std::string( STR(1) ) );
		vNameList.push_back( std::string( "BLACK" ) );
		vNameList.push_back( std::string( "WHITE" ) );

		pList->OpenList( pOpenBtn, vNameList, (int)m_CurrentOption.m_OverayType, IO_OVERAY_CHANGE );
	}
}

void GameOptionSpecTab::SetChangeOveray( int iIndex )
{
	HideChildWnd( ID_OVERAY_LIST );

	if( !COMPARE( iIndex, OVERAY_NONE, OVERAY_MAX ) )
		return;

	OverayType eNewOveray = (OverayType)iIndex;
	if( m_CurrentOption.m_OverayType == eNewOveray )
		return;

	m_CurrentOption.m_OverayType = eNewOveray;

	UpdateOverayBoxText();

	SetCustomSpec();
	UnCheckAutoSpec();

	SetChildActive( ID_APPLY );
}

void GameOptionSpecTab::UpdateOverayBoxText()
{
	ioWnd *pWnd = FindChildWnd( ID_OVERAY_BOX );
	if( !pWnd )	return;

	switch( m_CurrentOption.m_OverayType )
	{
	case OVERAY_NONE:
		pWnd->SetTitleText( STR(1) );
		break;
	case OVERAY_BLACK:
		pWnd->SetTitleText( "BLACK" );
		break;
	case OVERAY_WHITE:
		pWnd->SetTitleText( "WHITE" );
		break;
	}
}

void GameOptionSpecTab::SetChangeResolution( int iIndex )
{
	HideChildWnd( ID_RESOLUTION_LIST );

	int iWidth, iHeight;
	if( !Setting::GetResolution( iIndex, iWidth, iHeight ) )
		return;

	// check Same Resolution?
	if( m_CurrentOption.m_iWidth == iWidth && m_CurrentOption.m_iHeight == iHeight )
		return;

	m_CurrentOption.m_iWidth  = iWidth;
	m_CurrentOption.m_iHeight = iHeight;
	ChangeResolutionText( iWidth, iHeight );

	SetCustomSpec();
	UnCheckAutoSpec();

	SetChildActive( ID_APPLY );
}

void GameOptionSpecTab::ChangeResolutionText( int iWidth, int iHeight )
{
	ioWnd *pResolution = FindChildWnd( ID_RESOLUTION_BOX );
	if( pResolution )
	{
		char szResolution[MAX_PATH];
		wsprintf( szResolution, "%d x %d", iWidth, iHeight );
		pResolution->SetTitleText( szResolution );
	}
}

void GameOptionSpecTab::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_VERY_HIGH_SPEC:
	case ID_HIGH_SPEC:
	case ID_MIDDLE_SPEC:
	case ID_LOW_SPEC:
	case ID_MINIMUM_SPEC:
	case ID_CUSTOM_SPEC:
		if( cmd == IOBN_BTNUP )
		{
			SetSpecOption( dwID );
		}
		break;
	case ID_SCREEN_BOX:
		if( cmd == IOBN_BTNUP )
		{
			OpenScreenList( pWnd );
		}
		break;
	case ID_RESOLUTION_BOX:
		if( cmd == IOBN_BTNUP )
		{
			OpenResolutionList( pWnd );
		}
		break;
	case ID_FILTER_BOX:
		if( cmd == IOBN_BTNUP )
		{
			OpenFilterList( pWnd );
		}
		break;
	case ID_OVERAY_BOX:
		if( cmd == IOBN_BTNUP )
		{
			OpenOverayList( pWnd );
		}
		break;
	case ID_ANTI_ALIASING_QUALITY:
	case ID_ANTI_ALIASING_HIGH:
	case ID_ANTI_ALIASING_MIDDLE:
	case ID_ANTI_ALIASING_LOW:
		if( cmd == IOBN_BTNDOWN )
		{
			SetAntiAliasingOption( dwID );
		}
		break;
	case ID_SPEC_AUTO_CHECK:
		if( cmd == IOBN_BTNUP )
		{
			m_CurrentOption.m_bSpecAutoCheck = !m_CurrentOption.m_bSpecAutoCheck;

			if( m_CurrentOption.m_bSpecAutoCheck )
			{
				m_DefaultSpec = Setting::CheckHardwareSpec();
				Setting::SetOptionBySpec( m_CurrentOption, m_DefaultSpec );
				UpdateByCurrentOption();
			}

			SetChildActive( ID_APPLY );
		}
		break;
	case ID_COMMIT:
		if( cmd == IOBN_BTNUP )
		{	
			ioButton *pApplyBtn = dynamic_cast< ioButton * >( FindChildWnd( ID_APPLY ) );
			if( pApplyBtn && !pApplyBtn->HasWndStyle( IWS_INACTIVE ) )
			{
				// 변경된 옵션이 있으면
				// 렉 유발 방지
				if( m_pPlayStage )
				{
					if( m_pPlayStage->GetModeType() != MT_MYROOM && m_pPlayStage->GetModeType() != MT_TRAINING && m_pPlayStage->GetModeType() != MT_HEADQUARTERS && m_pPlayStage->GetModeType() != MT_HOUSE )
					{
						//
						ioBaseChar *pOwnerChar = m_pPlayStage->GetOwnerChar();
						if( pOwnerChar )
						{
							switch( pOwnerChar->GetState() )
							{
							case CS_DELAY:
							case CS_ETCITEM_MOTION:
							case CS_SPECIAL_MOTION:
							case CS_DIE:
							case CS_READY:
							case CS_LOADING:
							case CS_OBSERVER:
							case CS_VIEW:
							case CS_ENDMOTION:
								break;
							default:
								g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
								return;
							}
						}
					}
				}
			}

			if( ApplyChangedOption() )
			{
				if( g_MovieMaker.IsMovieCapturing() )
					g_MovieMaker.ToggleMovieCapture();
			}

			if(m_pParent)
				m_pParent->HideWnd();
		}
		break;
	case ID_CANCEL:
		if( cmd == IOBN_BTNUP )
		{
			if(m_pParent)
				m_pParent->HideWnd();
		}
		break;
	case ID_APPLY:
		if( cmd == IOBN_BTNUP )
		{
			// 렉 유발 방지
			if( m_pPlayStage )
			{
				if( m_pPlayStage->GetModeType() != MT_MYROOM && m_pPlayStage->GetModeType() != MT_TRAINING && m_pPlayStage->GetModeType() != MT_HEADQUARTERS && m_pPlayStage->GetModeType() != MT_HOUSE )
				{
					//
					ioBaseChar *pOwnerChar = m_pPlayStage->GetOwnerChar();
					if( pOwnerChar )
					{
						switch( pOwnerChar->GetState() )
						{
						case CS_DELAY:
						case CS_ETCITEM_MOTION:
						case CS_SPECIAL_MOTION:
						case CS_DIE:
						case CS_READY:
						case CS_LOADING:
						case CS_OBSERVER:
						case CS_VIEW:
						case CS_ENDMOTION:
							break;
						default:
							g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
							return;
						}
					}
				}
			}

			if( ApplyChangedOption() )
			{
				if( g_MovieMaker.IsMovieCapturing() )
					g_MovieMaker.ToggleMovieCapture();
			}
			SetChildInActive( ID_APPLY );
		}
		break;
	case ID_BACKGROUND_CHANGE:
		if( cmd == IOBN_BTNUP )
		{
			g_GUIMgr.ShowWnd( BACKGROUND_CHANGE_WND );
		}
		else if( cmd == IOEX_BTNUP )
		{
			if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
			}
			else
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "배경화면 설정은 광장과 본부에서#가능합니다." );
			}
			
		}
		break;
	case ID_TRAIL_GLOW:
		if( cmd == IOBN_BTNUP )
		{
			SetOnOffOption( m_CurrentOption.m_bTrailGlow );
		}
		break;
	case ID_SCREEN_GLOW:
		if( cmd == IOBN_BTNUP )
		{
			SetOnOffOption( m_CurrentOption.m_bGlow );
		}
		break;
	case ID_MAP_EFFECT:
		if( cmd == IOBN_BTNUP )
		{
			SetOnOffOption( m_CurrentOption.m_bShowMapEffect );
		}
		break;
	case ID_SHADOW:
		if( cmd == IOBN_BTNUP )
		{
			SetOnOffOption( m_CurrentOption.m_bShadow );
		}
		break;
	case ID_16BIT_FRAMEBUFFER:
		if( cmd == IOBN_BTNUP )
		{
			SetOnOffOption( m_CurrentOption.m_bUse16bitFrameBuffer );
		}
		break;
	case ID_SOFT_SKINNING:
		if( cmd == IOBN_BTNUP )
		{
			SetOnOffOption( m_CurrentOption.m_bSoftSkinning );
		}
		break;
	case ID_MAP_EDGE:
		if( cmd == IOBN_BTNUP )
		{
			SetOnOffOption( m_CurrentOption.m_bUseMapEdge );
		}
		break;
	case ID_OBJECT_EDGE:
		if( cmd == IOBN_BTNUP )
		{
			SetOnOffOption( m_CurrentOption.m_bUseObjectEdge );
		}
		break;
	case ID_TEXTURE_HIGH:
	case ID_TEXTURE_MIDDLE:
	case ID_TEXTURE_LOW:
		if( cmd == IOBN_BTNDOWN )
		{
			SetTextureOption( dwID );
		}
		break;
	case ID_LIGHT_HIGH:
	case ID_LIGHT_MIDDLE:
	case ID_LIGHT_LOW:
		if( cmd == IOBN_BTNDOWN )
		{
			SetLightOption( dwID );
		}
		break;
	case ID_EFFECT_HIGH:
	case ID_EFFECT_MIDDLE:
	case ID_EFFECT_LOW:
		if( cmd == IOBN_BTNDOWN )
		{
			SetEffectOption( dwID );
		}
		break;
	case ID_MOVIE_MODE:
		if( cmd == IOBN_BTNUP )
		{
			ToggleMovieMode();
		}
		break;
	case ID_MAP_OBJECT_HIGH:
	case ID_MAP_OBJECT_MIDDLE:
	case ID_MAP_OBJECT_LOW:
		if( cmd == IOBN_BTNDOWN )
		{
			SetMapObjectOption( dwID );
		}
		break;
	case ID_RESOLUTION_LIST:
		if( cmd == IO_RESOLUTION_CHANGE )	
		{
			SetChangeResolution( param );
		}
		break;
	case ID_FILTER_LIST:
		if( cmd == IO_FILTER_CHANGE )	
		{
			SetChangeFilter( param );
		}
		break;
	case ID_OVERAY_LIST:
		if( cmd == IO_OVERAY_CHANGE )	
		{
			SetChangeOveray( param );
		}
		break;
	case ID_SCREEN_LIST:
		if( cmd == IO_SCREEN_MODE_CHANGE )
		{
			SetChangeScreen( param );
		}
		break;
	case RESOLUTION_CONFIRM_WND:
		if( cmd == IO_RESOLUTION_CONFIRM )
		{
			if( param == 1 )
			{
				SaveCurrentSetting( false );	// 현재 설정 저장
			}
			else
			{
				RestoreResolution();
			}
		}
		break;
	}
}

void GameOptionSpecTab::RestoreResolution()
{
	m_CurrentOption.m_iWidth  = m_iRestoreWidth;
	m_CurrentOption.m_iHeight = m_iRestoreHeight;
	m_CurrentOption.m_bFullScreen = m_bRestoreFullScreen;

	SetCustomSpec();
	UnCheckAutoSpec();

	Setting::SetSpec( m_DefaultSpec );
	g_App.ApplyOptionChange( m_CurrentOption );

	m_CurrentOption = Setting::GetSettingOption();
	UpdateByCurrentOption();

	SaveCurrentSetting( false );
}

void GameOptionSpecTab::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();
	if( m_pDot )
	{
		for(int i = 0;i < 68;i++)
		{
			m_pDot->Render( iXPos, iYPos + (i*5) );
		}
	}
}

//---------------------------------------------------------------------------------------------------------------------

GameOptionWnd::GameOptionWnd()
{
	m_dwCheckBtn = ID_SPEC_TAB_BTN;
	m_pPlayStage = NULL;
}

GameOptionWnd::~GameOptionWnd()
{

}

void GameOptionWnd::iwm_create()
{
	g_GUIMgr.AddWnd( "XML/GameOptionSpecTab.xml", this );
	g_GUIMgr.AddWnd( "XML/GameOptionVolumeTab.xml", this );
	g_GUIMgr.AddWnd( "XML/GameOptionSettingGameTab.xml", this );
	g_GUIMgr.AddWnd( "XML/gameoptionsettingmovieandshottab.xml", this );
	g_GUIMgr.AddWnd( "XML/gameoptionsettingkeytab.xml", this );
	g_GUIMgr.AddWnd( "XML/GameOptionSettingJoyKeyTab.xml", this );
}

void GameOptionWnd::iwm_show()
{
	GameOptionSettingGameTab *pGameChild = dynamic_cast<GameOptionSettingGameTab*> ( FindChildWnd( ID_GAME_TAB_WND ) ) ;
	if( pGameChild )
		pGameChild->Init();

	GameOptionMovieScreenShotTab *pMovieChild = dynamic_cast<GameOptionMovieScreenShotTab*> ( FindChildWnd( ID_MOVIE_SCREENSHOT_TAB_WND ) );
	if( pMovieChild )
		pMovieChild->Init();

	GameOptionSettingKeyTab *pKeyChild = dynamic_cast<GameOptionSettingKeyTab*> ( FindChildWnd( ID_KEYBOARD_TAB_WND ) ) ;
	if( pKeyChild )
		pKeyChild->Init();

	GameOptionSettingJoyKeyTab *pJoyKeyChild = dynamic_cast<GameOptionSettingJoyKeyTab*> ( FindChildWnd( ID_JOYPAD_TAB_WND ) ) ;
	if( pJoyKeyChild )
		pJoyKeyChild->Init();

	if( g_Input.IsEnableJoyStick() )
		SetChildActive( ID_JOYPAD_TAB_BTN );
	else
		SetChildInActive( ID_JOYPAD_TAB_BTN );

	if( g_App.IsOpenWebPageState() )	// 렉권 방지. 웹페이지가 열리지 않게 하는 조건과 동일하게 그래픽 옵션 변경도 막는다.
	{
		SetChildActive( ID_SPEC_TAB_BTN );
		ChangeWindow( ID_SPEC_TAB_BTN );
	}
	else
	{
		SetChildInActive( ID_SPEC_TAB_BTN );
		ChangeWindow( ID_VOLUME_TAB_BTN );
	}

#if defined( USE_GA )
	// WND_OPTION
	g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FWND%2FOPTION" );
#endif

}

void GameOptionWnd::iwm_hide()
{
	HideChildWnd( ID_SPEC_TAB_WND );
	HideChildWnd( ID_VOLUME_TAB_WND );
	HideChildWnd( ID_GAME_TAB_WND );
	HideChildWnd( ID_MOVIE_SCREENSHOT_TAB_WND );
	HideChildWnd( ID_KEYBOARD_TAB_WND );
	HideChildWnd( ID_JOYPAD_TAB_WND );
}

void GameOptionWnd::SetPlayStage( ioPlayStage *pPlayStage ) 
{
	m_pPlayStage = pPlayStage; 

	GameOptionSpecTab *pSpecTab = dynamic_cast< GameOptionSpecTab * >( FindChildWnd( ID_SPEC_TAB_WND ) );
	if( pSpecTab )
	{
		pSpecTab->SetPlayStage( m_pPlayStage );
	}
}

void GameOptionWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;
	case ID_UI_INIT:
		if( cmd == IOBN_BTNUP )
		{
			g_GUIMgr.SetUIPostionToDefault();
			if( m_pPlayStage )
				m_pPlayStage->MySoldierDefPos();

			ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
			kPrinter[0].SetTextStyle( TS_NORMAL );
			kPrinter[0].SetBkColor( 0, 0, 0 );	
			kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
			kPrinter[0].AddTextPiece( FONT_SIZE_13, STR(1) );

			kPrinter[1].SetTextStyle( TS_NORMAL );
			kPrinter[1].SetBkColor( 0, 0, 0 );	
			kPrinter[1].SetTextColor( TCT_DEFAULT_DARKGRAY );
			kPrinter[1].AddTextPiece( FONT_SIZE_13, STR(2) );

			g_GUIMgr.SetPrevMsgListBoxWithTitle( NULL, MB_OK, NULL, kPrinter, STR(3), STR(4), STR(5), STR(6) );
		}
		break;
	case ID_SPEC_TAB_BTN:
	case ID_VOLUME_TAB_BTN:
	case ID_GAME_TAB_BTN:
	case ID_MOVIE_SCREENSHOT_TAB_BTN:
	case ID_KEYBOARD_TAB_BTN:
	case ID_JOYPAD_TAB_BTN:
		if( cmd == IOBN_BTNDOWN )
		{
			ChangeWindow( dwID );
		}
		break;
	}
}

void GameOptionWnd::ChangeWindow( DWORD dwBtn )
{
	m_dwCheckBtn = dwBtn;

	CheckRadioButton( ID_SPEC_TAB_BTN, ID_JOYPAD_TAB_BTN, m_dwCheckBtn );

	HideChildWnd( ID_SPEC_TAB_WND );
	HideChildWnd( ID_VOLUME_TAB_WND );
	HideChildWnd( ID_GAME_TAB_WND );
	HideChildWnd( ID_MOVIE_SCREENSHOT_TAB_WND );
	HideChildWnd( ID_KEYBOARD_TAB_WND );
	HideChildWnd( ID_JOYPAD_TAB_WND );
	switch( m_dwCheckBtn )
	{
	case ID_SPEC_TAB_BTN:
		ShowChildWnd( ID_SPEC_TAB_WND );
		break;
	case ID_VOLUME_TAB_BTN:
		ShowChildWnd( ID_VOLUME_TAB_WND );
		break;
	case ID_GAME_TAB_BTN:
		ShowChildWnd( ID_GAME_TAB_WND );
		break;
	case ID_MOVIE_SCREENSHOT_TAB_BTN:
		ShowChildWnd( ID_MOVIE_SCREENSHOT_TAB_WND );
		break;
	case ID_KEYBOARD_TAB_BTN:
		ShowChildWnd( ID_KEYBOARD_TAB_WND );
		break;
	case ID_JOYPAD_TAB_BTN:
		ShowChildWnd( ID_JOYPAD_TAB_WND );
		break;
	}
}

void GameOptionWnd::SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate /*= false */ )
{
	ioMovingWnd::SetPositionToDefault( iParentWidth, iParentHeight, bChildMove, bCreate );
	
	if( !bCreate )
	{
		ChangeWindow( m_dwCheckBtn );
	}
}

void GameOptionWnd::ShowSelectTab( DWORD dwTabBtnID, DWORD dwChildTabBtnID )
{
	if( !IsShow() )
		ShowWnd();

	ioWnd *pTabBtn = FindChildWnd( dwTabBtnID );
	if( !pTabBtn )
		return;

	iwm_command( pTabBtn, IOBN_BTNDOWN, 0 );

	ioWnd *pTabWnd = NULL;
	if( dwTabBtnID == ID_SPEC_TAB_BTN )
		pTabWnd = FindChildWnd( ID_GAME_TAB_WND );
	else if( dwTabBtnID == ID_VOLUME_TAB_BTN )
		pTabWnd = FindChildWnd( ID_VOLUME_TAB_WND );
	else if( dwChildTabBtnID == ID_GAME_TAB_BTN )
		pTabWnd = FindChildWnd( ID_GAME_TAB_WND );

	if( !pTabWnd )
		return;

	ioWnd *pChildTabBtn = pTabWnd->FindChildWnd( dwChildTabBtnID );
	pTabWnd->iwm_command( pChildTabBtn, IOBN_BTNDOWN, 0 );
}

//---------------------------------------------------------------------------------------------------------------------
BackgroundChangeBtn::BackgroundChangeBtn()
{
	m_pDefaultBack = NULL;
	m_pStripBlack  = NULL;
	m_pLineFrame   = NULL;
	m_pCustomBack  = NULL;
}

BackgroundChangeBtn::~BackgroundChangeBtn()
{
	SAFEDELETE( m_pDefaultBack );
	SAFEDELETE( m_pStripBlack );
	SAFEDELETE( m_pLineFrame );
}

void BackgroundChangeBtn::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "DefaultBack" )
	{
		SAFEDELETE( m_pDefaultBack );
		m_pDefaultBack = pImage;
	}
	else if( szType == "StripBlack" )
	{
		SAFEDELETE( m_pStripBlack );
		m_pStripBlack = pImage;
	}
	else
	{
		ioButton::AddRenderImage( szType, pImage );
	}
}

void BackgroundChangeBtn::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "LineFrame" )
	{
		SAFEDELETE( m_pLineFrame );
		m_pLineFrame = pFrame;
	}
	else
	{
		ioButton::AddRenderFrame( szType, pFrame );
	}
}

void BackgroundChangeBtn::iwm_show()
{
	m_pCustomBack = NULL;
	switch( GetID() )
	{
	case BackgroundChangeWnd::ID_LOBBY_CHANGE:
		{
			m_pCustomBack = g_BackgroundMgr.GetBackgroundRes( BGT_LOBBY );
		}
		break;
	case BackgroundChangeWnd::ID_LOADING_CHANGE:
		{
			if( g_BackgroundMgr.IsCustomLoadingBack() )
				m_pCustomBack = g_BackgroundMgr.GetLoading( false, -1, 0 );
			else
				m_pCustomBack = m_pDefaultBack;
		}
		break;
	case BackgroundChangeWnd::ID_BATTLE_CHANGE:
		{
			m_pCustomBack = g_BackgroundMgr.GetBackgroundRes( BGT_BATTLE );
		}
		break;
	case BackgroundChangeWnd::ID_LADDER_CHANGE:
		{
			m_pCustomBack = g_BackgroundMgr.GetBackgroundRes( BGT_LADDER );
		}
		break;
	case BackgroundChangeWnd::ID_MODE_FIND_CHANGE:
		{
			m_pCustomBack = g_BackgroundMgr.GetBackgroundRes( BGT_MODE );
		}
		break;
	}
}

void BackgroundChangeBtn::OnRender()
{
	ioButton::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_pCustomBack )
	{
		m_pCustomBack->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
		m_pCustomBack->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
		m_pCustomBack->SetSize( GetWidth(), GetHeight() );
		m_pCustomBack->Render( iXPos, iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );

		if( IsOver() || IsClicked() )
			m_pCustomBack->Render( iXPos, iYPos, UI_RENDER_SCREEN, TFO_BILINEAR );
	}

	if( m_pStripBlack )
	{
		m_pStripBlack->SetAlpha( (float)MAX_ALPHA_RATE * 0.4f );
		m_pStripBlack->Render( iXPos, iYPos + 74 );
	}

	if( m_pLineFrame )
	{
		m_pLineFrame->Render( iXPos - 2, iYPos - 2 );
	}

	g_FontMgr.SetAlignType( TAT_RIGHT );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
	if( GetID() == BackgroundChangeWnd::ID_LOBBY_CHANGE )
	{
		g_FontMgr.PrintText( iXPos + 116, iYPos + 75, FONT_SIZE_12, STR(1) );
	}
	else if( GetID() == BackgroundChangeWnd::ID_LOADING_CHANGE )
	{
		g_FontMgr.PrintText( iXPos + 116, iYPos + 75, FONT_SIZE_12, STR(2) );
	}
	else if( GetID() == BackgroundChangeWnd::ID_BATTLE_CHANGE )
	{
		g_FontMgr.PrintText( iXPos + 116, iYPos + 75, FONT_SIZE_12, STR(3) );
	}
	else if( GetID() == BackgroundChangeWnd::ID_LADDER_CHANGE )
	{
		g_FontMgr.PrintText( iXPos + 116, iYPos + 75, FONT_SIZE_12, STR(4) );
	}
	else if( GetID() == BackgroundChangeWnd::ID_MODE_FIND_CHANGE )
	{
		if( g_App.GetConnectedMoveMode() == ioApplication::CMM_PLAZA )
		{
			g_FontMgr.PrintText( iXPos + 116, iYPos + 75, FONT_SIZE_12, "광장으로 퇴장" );
		}
		else
		{
			g_FontMgr.PrintText( iXPos + 116, iYPos + 75, FONT_SIZE_12, "본부로 퇴장" );
		}
	}
}

BackgroundChangeWnd::BackgroundChangeWnd()
{

}

BackgroundChangeWnd::~BackgroundChangeWnd()
{

}

void BackgroundChangeWnd::iwm_show()
{
	if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
	{
		HideChildWnd( ID_MODE_FIND_CHANGE );
		ShowChildWnd( ID_LOBBY_CHANGE );
	}
	else
	{
		ShowChildWnd( ID_MODE_FIND_CHANGE );
		HideChildWnd( ID_LOBBY_CHANGE );
	}

	ShowChildWnd( ID_LOADING_CHANGE );
	ShowChildWnd( ID_BATTLE_CHANGE );
	ShowChildWnd( ID_LADDER_CHANGE );
}

void BackgroundChangeWnd::iwm_hide()
{
	HideChildWnd( ID_LOBBY_CHANGE );
	HideChildWnd( ID_LOADING_CHANGE );
	HideChildWnd( ID_BATTLE_CHANGE );
	HideChildWnd( ID_LADDER_CHANGE );
	HideChildWnd( ID_MODE_FIND_CHANGE );
}

void BackgroundChangeWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
		if( cmd == IOBN_BTNUP )
			HideWnd();
		break;
	case ID_LOBBY_CHANGE:
		if( cmd == IOBN_BTNUP )
		{
			OpenFileListWnd( this, FileListWnd::RWT_LOBBY_BACK_CHANGE_WND );
		}
		break;
	case ID_LOADING_CHANGE:
		if( cmd == IOBN_BTNUP )
		{
			OpenFileListWnd( this, FileListWnd::RWT_LOADING_BACK_CHANGE_WND );
		}
		break;
	case ID_BATTLE_CHANGE:
		if( cmd == IOBN_BTNUP )
		{
			OpenFileListWnd( this, FileListWnd::RWT_BATTLE_BACK_CHANGE_WND );
		}
		break;
	case ID_LADDER_CHANGE:
		if( cmd == IOBN_BTNUP )
		{
			OpenFileListWnd( this, FileListWnd::RWT_LADDER_BACK_CHANGE_WND );
		}
		break;	
	case ID_MODE_FIND_CHANGE:
		if( cmd == IOBN_BTNUP )
		{
			//광장/로비 병합 버전이 아닐경우 작동 불가
			if( g_App.GetConnectedMoveMode() != ioApplication::CMM_LOBBY )
			{
				OpenFileListWnd( this, FileListWnd::RWT_MODE_FIND_BACK_CHANGE_WND );
			}
		}
		break;
	}
}

void BackgroundChangeWnd::OpenFileListWnd( ioWnd *pReturnBtn, int iRwtType )
{
	FileListWnd *pFileListWnd = dynamic_cast<FileListWnd*> (g_GUIMgr.FindWnd( BACKGROUND_FILE_LIST_WND ) );
	if( !pFileListWnd )
	{
		LOG.PrintTimeAndLog( 0, "%s Error FileListWnd", __FUNCTION__ );
		return;
	}
	HideWnd();

	char szCurPath[MAX_PATH*2]="";
	GetCurrentDirectory( sizeof( szCurPath ), szCurPath );
	char szDrive[MAX_PATH]="";
	_splitpath( szCurPath, szDrive, NULL, NULL, NULL );
	sprintf( szCurPath, "%s", szDrive );

	pFileListWnd->SetFileList( szCurPath, "jpg" );
	pFileListWnd->SetReturnInfo( pReturnBtn, (FileListWnd::ReturnWndType)iRwtType );
	pFileListWnd->ShowWnd();
}