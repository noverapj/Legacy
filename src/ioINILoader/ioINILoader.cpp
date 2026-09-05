#include "StdAfx.h"
#include "ioINILoader.h"
#include "ioINIControl.h"


ioINILoader::ioINILoader(void)
{
	Init();
}

ioINILoader::ioINILoader( const TCHAR* fileName )
{
	Init();

	LoadFile( fileName );
}

ioINILoader::ioINILoader( const TCHAR* path, const TCHAR* fileName )
{
	Init();

	tstring filePath = path;
	transform( filePath.begin(), filePath.end(), filePath.begin(), tolower );
	tstring name = fileName;
	transform( name.begin(), name.end(), name.begin(), tolower );

	// Set fileName
	StringCbPrintf( m_fileName, sizeof( m_fileName ), _T("%s/%s"), filePath.c_str(), name.c_str() );

	LoadFile( GetFileName() );
}

ioINILoader::~ioINILoader(void)
{
}

void ioINILoader::Init()
{
	ZeroMemory( m_fileName, MAX_PATH );
	ZeroMemory( m_title, MAX_PATH );
}

bool ioINILoader::Startup( bool threadState )
{
	return g_INIControl->Startup( threadState );
}

bool ioINILoader::LoadFile( const TCHAR* fileName, const bool bCreate /*=false*/ )
{
	MakeFileName( fileName );

	return g_INIControl->LoadFile( GetFileName(), false, bCreate );
}

bool ioINILoader::ReloadFile( const TCHAR* fileName, const TCHAR* title /*= _T("")*/ )
{
	MakeFileName( fileName );

	if( _tcscmp( title, _T( "" ) ) == 0 )
		return g_INIControl->LoadFile( GetFileName(), true );
	else
		return g_INIControl->ReloadSection( GetFileName(), title );
}

bool ioINILoader::IsLoadComplete()
{
	return g_INIControl->IsLoadComplete();
}

// for ini checker( MFC tool )
void ioINILoader::RemoveFile( const TCHAR* fileName )
{
	MakeFileName( fileName );

	tstring name( GetFileName() );
	transform( name.begin(), name.end(), name.begin(), tolower );

	g_INIControl->RemoveFileName( name );
}

void ioINILoader::MakeFileName( const TCHAR* fileName )
{
	GetCurrentDirectory( MAX_PATH, m_fileName );
	StringCbCat( m_fileName, sizeof( m_fileName ), _T("\\") );
	StringCbCat( m_fileName, sizeof( m_fileName ), fileName );
}

void ioINILoader::SetFileName( const TCHAR* fileName )
{
	Init();

	MakeFileName( fileName );
}

void ioINILoader::SetTitle( const TCHAR* title )
{
	StringCbCopy( m_title, sizeof( m_title ), title );
}

const TCHAR* ioINILoader::GetFileName() const
{
	return m_fileName;
}

const TCHAR* ioINILoader::GetTitle() const
{
	return m_title;
}

bool ioINILoader::LoadBool( const TCHAR* title, const TCHAR* keyName, const bool defaultValue )
{
	return g_INIControl->LoadBool( GetFileName(), title, keyName, defaultValue );
}

int32 ioINILoader::LoadInt( const TCHAR* title, const TCHAR* keyName, const int32 defaultValue )
{
	return g_INIControl->LoadInt( GetFileName(), title, keyName, defaultValue );
}

float ioINILoader::LoadFloat( const TCHAR* title, const TCHAR* keyName, const float defaultValue )
{
	return g_INIControl->LoadFloat( GetFileName(), title, keyName, defaultValue );
}

void ioINILoader::LoadString( const TCHAR* title, const TCHAR* keyName, const TCHAR* Default, TCHAR* buf, const int32 bufLen )
{
	g_INIControl->LoadString( GetFileName(), title, keyName, Default, buf, bufLen );
}

bool ioINILoader::LoadBool( const TCHAR* keyName, const bool defaultValue )
{
	return LoadBool( GetTitle(), keyName, defaultValue );
}

int32 ioINILoader::LoadInt( const TCHAR* keyName, const int32 defaultValue )
{
	return LoadInt( GetTitle(), keyName, defaultValue );
}

float ioINILoader::LoadFloat( const TCHAR* keyName, const float defaultValue )
{
	return LoadFloat( GetTitle(), keyName, defaultValue );
}

void ioINILoader::LoadString( const TCHAR* keyName, const TCHAR* Default, TCHAR* buf, const int32 bufLen )
{
	LoadString( GetTitle(), keyName, Default, buf, bufLen );
}

bool ioINILoader::ReadBool( const TCHAR* title, const TCHAR* keyName, const bool defaultValue )
{
	return g_INIControl->ReadBool( GetFileName(), title, keyName, defaultValue );
}

int32 ioINILoader::ReadInt( const TCHAR* title, const TCHAR* keyName, const int32 defaultValue )
{
	return g_INIControl->ReadInt( GetFileName(), title, keyName, defaultValue );
}

float ioINILoader::ReadFloat( const TCHAR* title, const TCHAR* keyName, const float defaultValue )
{
	return g_INIControl->ReadFloat( GetFileName(), title, keyName, defaultValue );
}

void ioINILoader::ReadString( const TCHAR* title, const TCHAR* keyName, const TCHAR* DefaultValue, TCHAR* buf, const int32 bufLen )
{
	g_INIControl->ReadString( GetFileName(), title, keyName, DefaultValue, buf, bufLen );
}

// save
void ioINILoader::SaveString( const TCHAR* title, const TCHAR* keyName, const TCHAR* buf )
{	
	g_INIControl->SaveString( GetFileName(), title, keyName, buf );
}

void ioINILoader::SaveInt( const TCHAR* title, const TCHAR* keyName, const int32 value )
{
	g_INIControl->SaveInt( GetFileName(), title, keyName, value );
}

void ioINILoader::SaveFloat( const TCHAR* title, const TCHAR* keyName, const float value, const bool noLimit )
{
	g_INIControl->SaveFloat( GetFileName(), title, keyName, value, noLimit );
}

void ioINILoader::SaveBool( const TCHAR* title, const TCHAR* keyName, const bool value )
{
	g_INIControl->SaveBool( GetFileName(), title, keyName, value );
}

void ioINILoader::SaveString( const TCHAR* keyName, const TCHAR* buf )
{
	g_INIControl->SaveString( GetFileName(), GetTitle(), keyName, buf );
}

void ioINILoader::SaveInt( const TCHAR* keyName, const int32 value )
{
	g_INIControl->SaveInt( GetFileName(), GetTitle(), keyName, value );	
}

void ioINILoader::SaveFloat( const TCHAR* keyName, const float value, const bool noLimit )
{
	g_INIControl->SaveFloat( GetFileName(), GetTitle(), keyName, value, noLimit );
}

void ioINILoader::SaveBool( const TCHAR* keyName, const bool value )
{
	g_INIControl->SaveBool( GetFileName(), GetTitle(), keyName, value );
}
