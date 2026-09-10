#pragma once
#include "ioRenderTarget.h"

class ioRenderTargetManager : public Singleton< ioRenderTargetManager >
{
public:
	typedef std::map<int, ioRenderTarget*> RenderTargetMap;

protected:	
	RenderTargetMap	m_RenderTargetMap;
	int				m_RenderTargetIndex;

public:
	static ioRenderTargetManager& GetSingleton();
	
	int AddRenderTarget( ioRenderTarget* pTarget );
	void RemoveRenderTarget( int Index );

public:
	void OnRender();

public:
	ioRenderTargetManager();
	virtual ~ioRenderTargetManager();
};

#define g_RenderTargetMgr ioRenderTargetManager::GetSingleton()

