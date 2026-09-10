#pragma once


#include "ioGameStage.h"

class ioCamera;
class ioRenderSystem;
class ioGameSceneManager;
class SP2Packet;

class ioLoginStage : public ioGameStage
{
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

public:
	virtual bool ProcessUDPPacket( SP2Packet &rkPacket );
	virtual bool ProcessTCPPacket( SP2Packet &rkPacket );

public:
	ioLoginStage( ioGameSceneManager *pSceneMgr );
	virtual ~ioLoginStage();
};

inline ioLoginStage* ToLoginStage( ioGameStage *pStage )
{
	if( !pStage || pStage->GetTypeID() != ioGameStage::GS_LOGIN )
		return NULL;

	return dynamic_cast< ioLoginStage* >( pStage );
}

