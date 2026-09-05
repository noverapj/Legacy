

#ifndef _ioAnimateFXManager_h_
#define _ioAnimateFXManager_h_

#include "io3DCommon.h"
#include "ioPrerequisites.h"
#include "ioResourceManager.h"

class ioResource;
class ioRcAnimateFX;
class ioAnimateFX;
class ioBinaryStream;

class __EX ioAnimateFXManager : public ioResourceManager, public Singleton< ioAnimateFXManager >
{
protected:
	typedef std::set< ioHashString > FXFileSet;
	FXFileSet m_FileNameSet;

protected:
	virtual ioResource* CreateImpl( const ioHashString &name, bool bThread );
	virtual void ReLoadImpl( const ioHashString &name );

public:
	void InitializeFileSet();

	ioAnimateFX* CreateFX( const ioHashString &rkAnimationName );
	ioRcAnimateFX* CreateAnimateFX( const ioHashString &name );
	ioRcAnimateFX* GetAnimateFXByName( const ioHashString &name );

protected:
	bool LoadFile( const ioHashString &name, ioRcAnimateFX *pRcFX );
	bool LoadFileInternal( ioRcAnimateFX *pRcFX, ioBinaryStream *pStream );
	bool IsAnimateFXFile( ioBinaryStream *pStream );

public:
	bool SaveFile( const ioHashString &rkName );

public:
	inline bool HasFxFileSet() const { return !m_FileNameSet.empty(); }

public:
	static ioAnimateFXManager& GetSingleton();

public:
	ioAnimateFXManager();
	virtual ~ioAnimateFXManager();
};

#define g_AnimateFXMgr ioAnimateFXManager::GetSingleton()

#endif
