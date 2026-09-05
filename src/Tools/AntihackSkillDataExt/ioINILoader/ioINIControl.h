#pragma once

// typedef
typedef std::basic_string<TCHAR> tstring;
typedef std::vector< tstring > STRINGVECTOR;
typedef map< tstring, tstring > KEYMAP;
typedef map< tstring, KEYMAP* > SECTIONMAP;
typedef map< tstring, SECTIONMAP* > FILEMAP;

#include "ioINIWriter.h"
#include "ioDataChunk.h"

class ioINIControl : public ioINIWriter
{
public:
	ioINIControl(void);
	~ioINIControl(void);
	
private:
	void Init();
	void Destroy();
	bool FindFileName( tstring& fileName );
	void LoadBegin();
	void LoadEnd();

	bool Parse( tstring& fileName );
	void ParseINI( SECTIONMAP* sectionMap );
	tstring ParseTitle( tstring& title, SECTIONMAP* sectionMap );
	void ParseKey( const tstring& line, KEYMAP* keyMap );
	STRINGVECTOR Split( const tstring& param, const tstring& delims, int32 maxSplit = 0 );
	void SetMemberData( tstring& fileName, SECTIONMAP* sectionMap );
	void InsertContainer( tstring& fileName, SECTIONMAP* sectionMap );
	bool ParseSection( tstring& fileName, tstring& sectionName, SECTIONMAP* sectionMap );

	// get pointer
	const SECTIONMAP* FindSectionMap( const TCHAR* fileName );
	KEYMAP* FindKeyMap( const TCHAR* section, const SECTIONMAP* sectionList );
	KEYMAP::iterator FindValue( const TCHAR* keyName, KEYMAP* keyList );
	KEYMAP* InsertKeyMap( SECTIONMAP* section, const TCHAR* sectionName );
	KEYMAP::iterator InsertValue( const TCHAR* keyName, KEYMAP* keyList );

public:
	bool Startup( bool threadState );
	void Run();
	void EndThreadState();
	void SetThreadState( bool threadState );
	bool GetThreadState() const	{ return m_threadState; }

public:
	bool LoadFile( const TCHAR* fileName, const bool reload = false );
	bool ReloadSection( const TCHAR* fileName, const TCHAR* section );

	bool IsLoadComplete() const		{ return m_loadComplete; }
	void RemoveFileName( tstring& fileName );

public:
	// load
	const bool LoadBool( const TCHAR* fileName, const TCHAR* section, const TCHAR* keyName, const bool defaultValue );
	const int32 LoadInt( const TCHAR* fileName, const TCHAR* section, const TCHAR* keyName, const int32 defaultValue );
	const float LoadFloat( const TCHAR* fileName, const TCHAR* section, const TCHAR* keyName, const float defaultValue );
	void LoadString( const TCHAR* fileName, const TCHAR* section, const TCHAR* keyName, const TCHAR* Default, TCHAR* buf, const int32 bufLen );

	// file read
	const bool ReadBool( const TCHAR* fileName, const TCHAR* section, const TCHAR* keyName, const bool defaultValue );
	const int32 ReadInt( const TCHAR* fileName, const TCHAR* section, const TCHAR* keyName, const int32 defaultValue );
	const float ReadFloat( const TCHAR* fileName, const TCHAR* section, const TCHAR* keyName, const float defaultValue );
	void ReadString( const TCHAR* fileName, const TCHAR* section, const TCHAR* keyName, const TCHAR* defaultValue, TCHAR* buf, const int32 bufLen );

	// save
	void SaveString( const TCHAR* fileName, const TCHAR* section, const TCHAR* key, const TCHAR* data );
	void SaveInt( const TCHAR* fileName, const TCHAR* section, const TCHAR* key, const int32 value );
	void SaveFloat( const TCHAR* fileName, const TCHAR* section, const TCHAR* key, const float value, const bool noLimit = false );
	void SaveBool( const TCHAR* fileName, const TCHAR* section, const TCHAR* key, const bool value );

private:
	bool m_loadComplete;
	FILEMAP m_fileMap;

	ioDataChunk m_dataChunk;
	bool m_threadState;
};

#define g_INIControl cSingleton< ioINIControl >::GetInstance()
