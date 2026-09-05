#pragma once

class ioCamera;
class ioRenderSystem;
class ioGameSceneManager;
class SP2Packet;

class ioGameStage
{
public:
	enum GameStage
	{
		GS_NONE,
		GS_LOGIN,
		GS_LOBBY,
		GS_FIND_MODE,
		GS_PLAY,
		GS_HACKING,
	};

protected:
	ioGameSceneManager	*m_pSceneMgr;

public:
	virtual bool InitStage() = 0;
	virtual void Render( ioRenderSystem *pSystem ) = 0;
	virtual void ClearStage() = 0;

public:
	virtual void UpdatePreLoop( ioCamera *pCamera ) = 0;
	virtual void UpdateLoop( ioCamera *pCamera ) = 0;
	virtual void UpdateAfterLoop( ioCamera *pCamera ) = 0;

public:
	virtual GameStage GetTypeID() const;
	virtual const char* GetTypeText() const;

public:
	virtual bool ProcessUDPPacket( SP2Packet &rkPacket ) = 0;
	virtual bool ProcessTCPPacket( SP2Packet &rkPacket ) = 0;

public:
	ioGameStage( ioGameSceneManager *pSceneMgr );
	virtual ~ioGameStage();
};

