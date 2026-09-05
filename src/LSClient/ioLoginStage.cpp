
#include "stdafx.h"

#include "ioLoginStage.h"

ioLoginStage::ioLoginStage( ioGameSceneManager *pSceneMgr )
: ioGameStage( pSceneMgr )
{
}

ioLoginStage::~ioLoginStage()
{
	ClearStage();
}

bool ioLoginStage::InitStage()
{
	InitGUI();
	InitCameraController();

	return true;
}

void ioLoginStage::InitGUI()
{
}

void ioLoginStage::InitCameraController()
{
	ioLookAtCameraController *pCtrl = new ioLookAtCameraController;
	pCtrl->SetPosition( D3DXVECTOR3( 0.0f, 0.0f, 0.0f ) );
	pCtrl->SetLookAt( D3DXVECTOR3( 0.0f, 0.0f, 200.0f ) );
	pCtrl->SetSpeed( 0.0f, 0.0f, 0.0f );
	g_App.GetCamera()->SetController( pCtrl );
}

void ioLoginStage::UpdatePreLoop( ioCamera *pCamera )
{
}

void ioLoginStage::UpdateLoop( ioCamera *pCamera )
{
}

void ioLoginStage::UpdateAfterLoop( ioCamera *pCamera )
{
	ioLookAtCameraInput kInput;
	pCamera->Update( &kInput );
}

void ioLoginStage::Render( ioRenderSystem *pSystem )
{
	pSystem->ClearBack( D3DCLEAR_ZBUFFER );
	if( pSystem->BeginScene() )
	{
		m_pSceneMgr->RenderVisibleObject();
		pSystem->EndScene();
	}

	pSystem->SetFogEnable( false );

	m_pSceneMgr->RenderPostProcess();
	g_GUIMgr.RenderGUI();
}

void ioLoginStage::ClearStage()
{
	g_GUIMgr.ClearAllGUI();
}

ioGameStage::GameStage ioLoginStage::GetTypeID() const
{
	return GS_LOGIN;
}

const char* ioLoginStage::GetTypeText() const
{
	return "GS_LOGIN";
}

bool ioLoginStage::ProcessUDPPacket( SP2Packet &rkPacket )
{
	return false;
}

bool ioLoginStage::ProcessTCPPacket( SP2Packet &rkPacket )
{
	return false;
}

