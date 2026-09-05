#pragma once
#include "io3DCommon.h"
#include "ioPrerequisites.h"
#include "ioHashString.h"

#include "ioResourceManager.h"

class ioResource;
class ioTextStream;
class ioMeshTrailer;
class ioAnimationEvent;

class __EX ioAnimationEventManager : public ioResourceManager, Singleton< ioAnimationEventManager >
{
protected:
	virtual ioResource* CreateImpl( const ioHashString &name, bool bThread );
	virtual void ReLoadImpl( const ioHashString &name );

public:
	virtual void NotifyLoadDone( ioStream *pStream, void *pParam, bool bLoaded );

public:
	void Initialize();
	void PrintCreateResult();

public:
	static ioAnimationEventManager& GetSingleton();

public:
	ioAnimationEvent* CreateAnimationEvent( const ioHashString& name, bool bThread );
	ioAnimationEvent* GetAnimationEventByName( const ioHashString& name ) ;

	bool ConverEventName( const ioHashString& AniFileName, ioHashString& szDestFileName );

	bool isAnimationEventFile( const ioHashString& AniFileName  );
	bool SpliteFileType(  IN const char *szSrcFileName,  OUT ioHashString& szDestFileName, OUT ioHashString& szDestFileExtension );
	void DestroyAnimationEvent();

protected:
	bool LoadFile( const ioHashString &name, ioAnimationEvent *pEvent, bool bThread );
	bool LoadFileInternal( ioAnimationEvent *pEvent, ioTextStream *pStream );

public:
	ioAnimationEventManager();
	virtual ~ioAnimationEventManager();
};

#define g_AnimationEventManager ioAnimationEventManager::GetSingleton()