

#include "stdafx.h"

#include "ioLobbyStage.h"

#include "ioCreateChar.h"
#include "ioItemRight.h"

#include "GUI/HelpWnd.h"
#include "GUI/TimeGateSearch.h"
#include "GUI/ManualWnd.h"

ioLobbyStage::ioLobbyStage( ioGameSceneManager *pSceneMgr )
: ioGameStage( pSceneMgr )
{
}

ioLobbyStage::~ioLobbyStage()
{
	ClearStage();	
}

bool ioLobbyStage::InitStage()
{	
	InitGUI();
	InitCameraController();

	g_MyInfo.ClearCharDiedInfo();
	g_MyInfo.ClearCharDiedPenaltyInfo();
	g_MyInfo.ClearCharJoinedInfo();

	g_AbuseMgr.Initialize();

	ioINILoader_e kLoader( "config/sp2_sound.ini" );
	kLoader.SetTitle_e( "sound_file_name" );

	char szBuf[MAX_PATH], szName[MAX_PATH];
	kLoader.LoadString_e( "lobby_sound", "", szBuf, MAX_PATH );

	wsprintf( szName, "Resource/wave/%s", szBuf );
	m_LobbySound = szName;

	return true;
}

void ioLobbyStage::InitGUI()
{
	g_GUIMgr.CheckGUIPosSave();

	HelpWnd *pHelpWnd = dynamic_cast<HelpWnd *>(g_GUIMgr.AddWnd( "XML/HelpWnd.xml" ));
	if( pHelpWnd ) 
	{
		pHelpWnd->SetPlayStage( NULL );
	}

	g_GUIMgr.AddWnd( "XML/GameOptionWnd.xml" );
	g_GUIMgr.AddWnd( "XML/DisconnectWnd.xml" );
	g_GUIMgr.AddWnd( "XML/PartyInvitedWnd.xml" );

	if( g_MyInfo.GetUserState() == 0 )
	{
		// 최초 용병 구입.
		g_MyInfo.SetFirstSolderBonus();
	}
}

void ioLobbyStage::InitCameraController()
{
	ioLookAtCameraController *pCtrl = new ioLookAtCameraController;
	pCtrl->SetPosition( D3DXVECTOR3( 0.0f, 0.0f, 0.0f ) );
	pCtrl->SetLookAt( D3DXVECTOR3( 0.0f, 0.0f, 200.0f ) );
	pCtrl->SetSpeed( 0.0f, 0.0f, 0.0f );
	g_App.GetCamera()->SetController( pCtrl );	
}

void ioLobbyStage::UpdatePreLoop( ioCamera *pCamera )
{
	g_GlobalTimer.Process();
}

void ioLobbyStage::UpdateLoop( ioCamera *pCamera )
{
	
}

void ioLobbyStage::UpdateAfterLoop( ioCamera *pCamera )
{
	ioLookAtCameraInput kInput;
	pCamera->Update( &kInput );

	ProcessFunctionKey();
	
	g_AbuseMgr.ProcessLobbyStage();
}

void ioLobbyStage::Render( ioRenderSystem *pSystem )
{
	pSystem->ClearBack( D3DCLEAR_ZBUFFER );
	pSystem->SetFogEnable( false );

	g_GUIMgr.RenderGUI();
}

void ioLobbyStage::ClearStage()
{
	g_SoundMgr.StopSound( m_LobbySound, 0 );
	g_GUIMgr.ClearAllGUI();
}

void ioLobbyStage::ProcessFunctionKey()
{
	if( ioWnd::m_pTopModalWnd )       //모달 윈도우가 있으면 키입력 불가.
		return;
}

ioGameStage::GameStage ioLobbyStage::GetTypeID() const
{
	return GS_LOBBY;
}

const char* ioLobbyStage::GetTypeText() const
{
	return "GS_LOBBY";
}

bool ioLobbyStage::ProcessUDPPacket( SP2Packet &rkPacket )
{
	switch( rkPacket.GetPacketID() )
	{
	case SUPK_SYNCTIME:
		OnSyncTime( rkPacket );
		return true;
	}

	return false;
}

void ioLobbyStage::OnSyncTime( SP2Packet &rkPacket )
{
	// 외침 가능 여부
	bool bWholeChatOn;
	rkPacket >> bWholeChatOn;
	Help::SetWholeChatOn( bWholeChatOn );
	Help::SetWholeRainbowChatOn( bWholeChatOn );

	// 핑 시간
	g_GlobalTimer.SetGlobalTime( rkPacket );
}

bool ioLobbyStage::ProcessTCPPacket( SP2Packet &rkPacket )
{
	switch( rkPacket.GetPacketID() )
	{
	case STPK_CREATE_ROOM:
		OnCreateRoom( rkPacket );
		return true;
	case STPK_JOIN_ROOM:
		OnJoinRoom( rkPacket );
		return true;
	case STPK_LOGOUT:
		OnLogOut( rkPacket );
		return true;
	}

	return false;
}

void ioLobbyStage::OnCreateRoom( SP2Packet &rkPacket )
{
	int iResult = 0;
	int iModeType = 0;
	int iSubNum = 0;

	rkPacket >> iResult;
	rkPacket >> iModeType;
	rkPacket >> iSubNum;

	switch( iResult )
	{
	case CREATE_ROOM_OK:
		{
		}		
		break;
	case CREATE_ROOM_NOT:
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		break;
	case CREATE_ROOM_FULL:
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
		break;
	case CREATE_ROOM_NOT_SOLDIER:
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
		break;
	}

	g_App.SetMouseBusy( false );
}

void ioLobbyStage::OnJoinRoom( SP2Packet &rkPacket )
{
	int iResult = 0;
	int iModeType = 0;
	int iSubNum = 0;
	int iRoomNum = -1;

	rkPacket >> iResult;
	rkPacket >> iModeType;
	rkPacket >> iSubNum;
	rkPacket >> iRoomNum;

	switch( iResult )
	{
	case JOIN_ROOM_OK:
		{
		}	
		break;
	case JOIN_ROOM_NOT:
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		break;
	case JOIN_ROOM_FULL:
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
		break;
	case JOIN_ROOM_NOT_SOLDIER:
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );		
		break;
	}

	g_App.SetMouseBusy( false );
}

void ioLobbyStage::OnLogOut( SP2Packet &rkPacket )
{
	LOG.PrintTimeAndLog( 0, "ExitProgram - 12" );


	g_App.SetExitProgram();
}