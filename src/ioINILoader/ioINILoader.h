#pragma once

class ioINILoader
{
public:
	ioINILoader(void);
	ioINILoader( const TCHAR* fileName );
	ioINILoader( const TCHAR* path, const TCHAR* fileName );
	~ioINILoader(void);

	void Init();
	bool Startup( bool threadState );
	bool LoadFile( const TCHAR* fileName, const bool bCreate = false );
	bool ReloadFile( const TCHAR* fileName, const TCHAR* title = _T("") );
	bool IsLoadComplete();
	void RemoveFile( const TCHAR* fileName );

private:
	void MakeFileName( const TCHAR* fileName );

public:
	void SetFileName( const TCHAR* fileName );
	void SetTitle( const TCHAR* title );
	const TCHAR* GetFileName() const;
	const TCHAR* GetTitle() const;
	
public:
	bool LoadBool( const TCHAR* title, const TCHAR* keyName, const bool defaultValue );
	int32 LoadInt( const TCHAR* title, const TCHAR* keyName, const int32 defaultValue );
	float LoadFloat( const TCHAR* title, const TCHAR* keyName, const float defaultValue );
	void LoadString( const TCHAR* title, const TCHAR* keyName, const TCHAR* defaultValue, TCHAR* buf, const int32 bufLen );
	
public:
	bool LoadBool( const TCHAR* keyName, const bool defaultValue );
	int32 LoadInt( const TCHAR* keyName, const int32 defaultValue );
	float LoadFloat( const TCHAR* keyName, const float defaultValue );
	void LoadString( const TCHAR* keyName, const TCHAR* Default, TCHAR* buf, const int32 bufLen );
	
public:
	// 颇老俊 流立立辟
	bool ReadBool( const TCHAR* title, const TCHAR* keyName, const bool defaultValue );
	int32 ReadInt( const TCHAR* title, const TCHAR* keyName, const int32 defaultValue );
	float ReadFloat( const TCHAR* title, const TCHAR* keyName, const float defaultValue );
	void ReadString( const TCHAR* title, const TCHAR* keyName, const TCHAR* Default, TCHAR* buf, const int32 bufLen );

public:
	void SaveString( const TCHAR* title, const TCHAR* keyName, const TCHAR* buf );
	void SaveInt( const TCHAR* title, const TCHAR* keyName, const int32 value );
	void SaveFloat( const TCHAR* title, const TCHAR* keyName, const float value, const bool noLimit = false );
	void SaveBool( const TCHAR* title, const TCHAR* keyName, const bool value );
	
public:
	void SaveString( const TCHAR* keyName, const TCHAR* buf );
	void SaveInt( const TCHAR* keyName, const int32 value );
	void SaveFloat( const TCHAR* keyName, const float value, const bool noLimit = false );
	void SaveBool( const TCHAR* keyName, const bool value );
	
private:
	TCHAR m_fileName[MAX_PATH];
	TCHAR m_title[MAX_PATH];
};
