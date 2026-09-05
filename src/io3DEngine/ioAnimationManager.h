

#ifndef _ioAnimationManager_h_
#define _ioAnimationManager_h_

#include "io3DCommon.h"
#include "ioPrerequisites.h"
#include "ioResourceManager.h"

class ioResource;
class ioRcAnimation;
class ioBinaryStream;
class ioMeshTrailer;
class ioAnimation;

class __EX ioAnimationManager : public ioResourceManager, public Singleton< ioAnimationManager >
{
protected:
	virtual ioResource* CreateImpl( const ioHashString &name, bool bThread );
	virtual void ReLoadImpl( const ioHashString &name );

public:
	virtual void NotifyLoadDone( ioStream *pStream, void *pParam, bool bLoaded );

public:
	void InitMemoryPoolSize( int iAniCount, int iTrackCount );

public:
	ioRcAnimation* CreateAnimation( const ioHashString &name, bool bThread = false );
	ioRcAnimation* GetAnimationByName( const ioHashString &name );

public:
	ioAnimation* CreateInstance( const ioHashString &name, bool bThread = false );

protected:
	bool LoadFile( const ioHashString &name, ioRcAnimation *pRcAni, bool bThread );
	bool LoadFileInternal( ioRcAnimation *pRcAni, ioBinaryStream *pStream );
	bool IsAnimationFile( ioBinaryStream *pStream );

public:
	static ioAnimationManager& GetSingleton();

public:
	ioAnimationManager();
	virtual ~ioAnimationManager();
};

#define g_AnimationMgr ioAnimationManager::GetSingleton()

#endif
