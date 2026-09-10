#pragma once


#include "ioGameStage.h"
#include "CharacterInfo.h"

class ioCamera;
class ioRenderSystem;
class ioGameSceneManager;
class SP2Packet;
class ioEntityGroup;

class ioLobbyStage : public ioGameStage
{
protected:
	ioHashString m_LobbySound;

public:
	virtual bool InitStage();
	virtual void Render( ioRenderSystem *pSystem );
	virtual void ClearStage();

public:
	virtual void SetShadowMapEnable( bool bEnable ){};

public:
	virtual void UpdatePreLoop( ioCamera *pCamera );
	virtual void UpdateLoop( ioCamera *pCamera );
	virtual void UpdateAfterLoop( ioCamera *pCamera );

public:
	virtual GameStage GetTypeID() const;
	virtual const char* GetTypeText() const;

protected:
	void InitGUI();
	void InitCameraController();

protected:
	void ProcessFunctionKey();

public:
	virtual bool ProcessUDPPacket( SP2Packet &rkPacket );
	virtual bool ProcessTCPPacket( SP2Packet &rkPacket );

protected:
	void OnCreateRoom( SP2Packet &rkPacket );
	void OnJoinRoom( SP2Packet &rkPacket );
	void OnLogOut( SP2Packet &rkPacket );
	void OnSyncTime( SP2Packet &rkPacket );
	
public:
	ioLobbyStage( ioGameSceneManager *pSceneMgr );
	virtual ~ioLobbyStage();
};

inline ioLobbyStage* ToLobbyStage( ioGameStage *pStage )
{
	if( !pStage || pStage->GetTypeID() != ioGameStage::GS_LOBBY )
		return NULL;

	return dynamic_cast< ioLobbyStage* >( pStage );
}

