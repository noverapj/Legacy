

#ifndef _ioMaterialManager_h_
#define _ioMaterialManager_h_

#include "io3DCommon.h"
#include "ioPrerequisites.h"
#include "ioMaterialSerializer.h"

class ioMaterial;
class ioMaterialScriptModifier;

class __EX ioMaterialManager : public Singleton< ioMaterialManager >
{
protected:
	typedef std::map< ioHashString, ioMaterial* > MaterialScriptMap;
	MaterialScriptMap m_MaterialScriptMap;
	ioMaterialSerializer m_Serializer;

	typedef std::map < ioHashString, int > SubMaterialCountMap;
	SubMaterialCountMap m_SubMaterialCountMap;

	ioMaterialScriptModifier *m_pScriptModifier;
	ioMaterialScriptModifier *m_pCloneModifier;

	char m_szStartDir[MAX_PATH];
	int  m_iUnNamedCnt;

	bool		m_bUseNearest;

public:
	const bool& GetNearest() { return m_bUseNearest; }

public:
	ioMaterial* CreateMaterial();
	ioMaterial* CreateMaterial( const ioHashString &name );

	ioMaterial* CloneMaterial( const ioHashString &name );
	ioMaterial* GetMaterial( const ioHashString &name );

public:
	int  ParseScript( const ioHashString &szScriptName );
	void SaveScript( std::vector< ioMaterial* > &rkList, const char *szFullPath );

public:
	// 첫 로딩시 적용
	void SetLoadScriptModifier( ioMaterialScriptModifier *pModifier );
	// 매터리얼 생성시 적용
	void SetCloneScriptModifier( ioMaterialScriptModifier *pModifier );

	void ApplyLoadScriptModifier( ioMaterial *pMaterial );
	void ApplyCloneScriiptModifier( ioMaterial *pMaterial );

public:
	void ModifyLoadedScript(ioMaterialScriptModifier *pModifier );

	ioMaterialScriptModifier* GetScriptModifier() const { return m_pScriptModifier; }
	ioMaterialScriptModifier* GetCloneModifier() const { return m_pCloneModifier; }

public:
	void SetStartDir( const char *szPath );
	const char* GetStartDir() const;

public:
	static ioMaterialManager& GetSingleton();

public:
	ioMaterialManager();
	virtual ~ioMaterialManager();
};

#define g_MaterialMgr ioMaterialManager::GetSingleton()

#endif
