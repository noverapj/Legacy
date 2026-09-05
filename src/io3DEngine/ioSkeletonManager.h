

#ifndef _ioSkeletonManager_h_
#define _ioSkeletonManager_h_

#include "io3DCommon.h"
#include "ioPrerequisites.h"

class ioResource;
class ioRcSkeleton;
class ioSkeleton;

class ioBinaryStream;

class __EX ioSkeletonManager : public ioResourceManager, public Singleton< ioSkeletonManager >
{
protected:
	virtual ioResource* CreateImpl( const ioHashString &name, bool bThread );
	virtual void ReLoadImpl( const ioHashString &name );

public:
	ioRcSkeleton* CreateSkeleton( const ioHashString &name );
	ioRcSkeleton* GetSkeletonByName( const ioHashString &name );

protected:
	bool LoadFile( const ioHashString &name, ioRcSkeleton* &pRcSkel );
	bool IsSkeletonFile( ioBinaryStream &rkStream );

public:
	ioSkeleton* CreateInstance( const ioHashString &name );

public:
	static ioSkeletonManager& GetSingleton();

public:
	ioSkeletonManager();
	virtual ~ioSkeletonManager();
};

#endif
