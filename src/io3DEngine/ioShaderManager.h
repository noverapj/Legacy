

#ifndef _ioShanderManager_h_
#define _ioShanderManager_h_

#include "io3DCommon.h"
#include "ioPrerequisites.h"
#include "ioResourceManager.h"

class ioResource;
class ioShader;
class ioTextStream;

#include "ioShaderDefine.h"
#include "ioShaderGroup.h"

class __EX ioShaderManager : public ioResourceManager, public Singleton< ioShaderManager >
{
protected:
	IDirect3DDevice9	*m_pD3Device;

	typedef std::map< ioHashString, ioShaderGroup* > ShaderGroupMap;
	ShaderGroupMap m_ShaderGroupMap;
	
public:
	virtual void DestroyAll();

protected:
	virtual ioResource* CreateImpl( const ioHashString &name, bool bThread );
	virtual void ReLoadImpl( const ioHashString &name );

public:
	ioShader* CreateShader( const ioHashString &name );
	ioShader* GetShaderByName( const ioHashString &name );

public:
	ioShaderGroup* LoadShaderGroup( const ioHashString &name );
	ioShaderGroup* GetShaderGroup( const ioHashString &name );
	void UnLoadShaderGroup( ioShaderGroup *pGrp );

public:
	void OnLostDevice();
	void OnResetDevice();

protected:
	ID3DXEffect* LoadEffect( const ioHashString &name );
	void LoadGroup( ioShaderGroup *pGrp );
	void ParseScript( ioShaderGroup *pGrp, ioTextStream &rkStream );
	void ParseProperty( ioShaderGroup *pGrp, ioTextStream &rkStream );
	void ParseProgram( ioShader *pShader, ioTextStream &rkStream );

public:
	static ioShaderManager& GetSingleton();

public:
	ioShaderManager( IDirect3DDevice9 *pDevice );
	virtual ~ioShaderManager();
};

#define g_ShaderMgr ioShaderManager::GetSingleton()

#endif