

#ifndef _ioResourceManager_h_
#define _ioResourceManager_h_

#include "io3DCommon.h"
#include "ioPrerequisites.h"
#include "ioDataProcessor.h"

class ioResource;
class ioStream;

enum StaticSwitch
{
	SATICS_OFF,
	SATICS_ON,
};

class __EX ioResourceManager : public ioDataProcessor
{
protected:
	typedef std::map< ioHashString, ioResource* > ResourceMap;
	ResourceMap	m_Resource;

	ErasePolicy m_DefaultErasePolicy;
	char	m_szStartDir[MAX_PATH];

protected:
	typedef std::map< ioHashString, DWORD > MemoryMap;
	MemoryMap m_MemoryMap;

	StaticSwitch m_StaticSwitch;

public:
	void AddMem( const ioHashString& KeyName, DWORD dwMem );
	void RemoveMem( const ioHashString& KeyName );
	void GetMemStatics( DWORD& dwCount, DWORD& dwSize );
	void SetSwitch( int eSwitch ){ m_StaticSwitch = static_cast<StaticSwitch>( eSwitch ); }

public:
	virtual void PrepareLoading( const ioHashString &name, bool bThread = false );

public:
	virtual ioResource* Create( const ioHashString &name, bool bThread = false );
	virtual ioResource* GetByName( const ioHashString &name );

	virtual void Destroy( ioResource *pRes );
	virtual void ForceDestroy( ioResource *pRes );
	virtual void DestroyAll();
	virtual void DestroyNoneRefResource( ErasePolicy ePolicy );

	virtual void PrintAllResources();

protected:
	virtual ioResource* CreateImpl( const ioHashString &name, bool bThread ) = 0;
	virtual void RegisterImpl( ioResource *pRes );
	virtual void ReLoadImpl( const ioHashString &name ) = 0;

public:	// ioDataProcessor
	virtual void NotifyLoadDone( ioStream *pStream, void *pParam, bool bLoaded );

public:
	void SetStartDir( const char *szPath );
	void SetDefaultErasePolicy( ErasePolicy ePolicy );

	inline const char* GetStartDir() const { return m_szStartDir; }

public:
	ioResourceManager();
	virtual ~ioResourceManager();
};

#endif
