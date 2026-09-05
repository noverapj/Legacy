

#ifndef _ioUIFrameManager_h_
#define _ioUIFrameManager_h_

#include "io3DCommon.h"
#include "ioResourceManager.h"

class ioUIRenderImage;
class ioUIRenderFrame;
class ioXMLElement;

class __EX ioUIFrameManager
{
protected:
	typedef std::map< ioHashString, ioUIRenderFrame* > FrameTempleteMap;
	FrameTempleteMap m_FrameTempleteMap;

protected:
	typedef std::map< ioHashString, DWORD > MemoryMap;
	MemoryMap m_MemoryMap;
	MemoryMap m_CloneMemoryMap;

	StaticSwitch m_StaticSwitch;

public:
	void AddMem( const ioHashString& KeyName, DWORD dwMem );
	void AddCloneMem( const ioHashString& KeyName );

	void RemoveMem( const ioHashString& KeyName );
	void RemoveCloneMem( const ioHashString& KeyName );

	void GetMemStatics( DWORD& dwCount, DWORD& dwSize );
	void SetSwitch( int eSwitch ){ m_StaticSwitch = static_cast<StaticSwitch>( eSwitch ); }

public:
	bool LoadFile( const char *szFile );

protected:
	void ParseFrameList( ioXMLElement &xRootElement );
	void ParsingImage( ioXMLElement &rkImage, ioUIRenderFrame *pFrame );

public:
	ioUIRenderFrame* CreateFrame( ioXMLElement &rkFrame );
	ioUIRenderFrame* CloneFrame( const ioHashString &rkName );

	const ioUIRenderFrame* GetFrame( const ioHashString &rkName ) const;

	int GetNumFrame() const { return static_cast<int>(m_FrameTempleteMap.size()); }
	const char* GetFrameName( int index ) const;

protected:
	bool RegisterFrame( const ioHashString &rkName, ioUIRenderFrame *pFrame );
	void DestroyAllFrames();

public:
	ioUIFrameManager();
	virtual ~ioUIFrameManager();
};

#endif