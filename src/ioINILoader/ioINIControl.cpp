#include "StdAfx.h"
#include "ioINIControl.h"



ioINIControl::ioINIControl(void) : m_loadComplete(false), m_threadState(false)
{
	Init();
}


ioINIControl::~ioINIControl(void)
{
	Destroy();
}

void ioINIControl::Init()
{
	m_fileMap.clear();
}

void ioINIControl::Destroy()
{
	// container
	FILEMAP::iterator iter		= m_fileMap.begin();
	FILEMAP::iterator iterEnd	= m_fileMap.end();

	while( iter != iterEnd )
	{
		SECTIONMAP::iterator	sectionIter		= ( (*iter).second )->begin();
		SECTIONMAP::iterator	sectionIterEnd	= ( (*iter).second )->end();

		while( sectionIter != sectionIterEnd )
		{
			delete (*sectionIter).second;
			++sectionIter;
		}

		delete (*iter).second;
		m_fileMap.erase( iter );
	}
}

bool ioINIControl::LoadFile( const TCHAR* fileName, const bool reload /*= false*/, const bool create /*=false*/ )
{
	LoadBegin();

	tstring name( fileName );
	transform( name.begin(), name.end(), name.begin(), tolower );
	
	// Check
	if( FindFileName( name ) == true )
	{
		if( reload == true )
		{
			//----------------------------------
			// Reload
			//----------------------------------

			// 삭제 후
			RemoveFileName( name );

			// INI Load..
			return Parse( name );
		}
		else
		{
			//----------------------------------
			// Reload 아님. 재사용.
			//----------------------------------

			// load state
			LoadEnd();
		}
	}
	else
	{
		return Parse( name, create );
	}

	return true;
}

bool ioINIControl::ReloadSection( const TCHAR* fileName, const TCHAR* section )
{
	tstring name( fileName );
	transform( name.begin(), name.end(), name.begin(), tolower );

	// fileName
	FILEMAP::iterator iter = m_fileMap.find( name );
	if( iter != m_fileMap.end() )
	{
		tstring sectionName( section );
		transform( sectionName.begin(), sectionName.end(), sectionName.begin(), tolower );

		// Section
		SECTIONMAP::iterator	sectionIter = ( (*iter).second )->find( sectionName );
		if( sectionIter != ( (*iter).second )->end() )
		{
			delete (*sectionIter).second;
			( (*iter).second )->erase( sectionIter++ );
		
			// section parse
			return ParseSection( name, sectionName, (*iter).second );
		}
	}

	return false;
}

bool ioINIControl::ParseSection( tstring& fileName, tstring& sectionName, SECTIONMAP* sectionMap )
{
	// chunk
	if( m_dataChunk.AllocateFromFile( fileName.c_str() ) == false )
		return false;
	
	// parse section
	tstring line, lastline;

	sectionName = _T( "[" ) + sectionName + _T( "]" );
	transform( sectionName.begin(), sectionName.end(), sectionName.begin(), tolower );

	while( ! m_dataChunk.IsEOF() )
	{
		if( lastline.empty() || lastline.at( 0 ) != _T( '[' ) )
		{
			m_dataChunk.GetLine( line );
		}
		else
		{
			line = lastline;
			lastline.at( 0 ) = _T( '\0' );
		}

		if( line.length() == 0 || line.at( 0 ) != _T( '[' ) )
			continue;

		transform( line.begin(), line.end(), line.begin(), tolower );
		if( line != sectionName )
			continue;

		lastline = ParseTitle( line, sectionMap );
		return true;
	}

	return false;
}

bool ioINIControl::FindFileName( tstring& fileName )
{
	FILEMAP::iterator iter = m_fileMap.find( fileName );
	if( iter != m_fileMap.end() )
	{
		return true;
	}
	return false;
}

void ioINIControl::LoadBegin()
{
	m_loadComplete = false;
}

void ioINIControl::LoadEnd()
{
	m_loadComplete = true;
}

bool ioINIControl::Startup( bool threadState )
{
	if( !m_threadState )
	{
		SetRunState(threadState);
		SetThreadState(threadState);

		ioINIWriter::Init();	

		if(ioINIWriter::Begin())
			return true;
	}
	return false;
}

void ioINIControl::Run()
{
	ioINIWriter::Run();
}

void ioINIControl::SetThreadState( bool threadState )
{
	m_threadState = threadState;
}

void ioINIControl::EndThreadState()
{
	SetRunState(false);
}

bool ioINIControl::Parse( tstring& fileName, bool bCreate /*= false*/ )
{
	LoadBegin();

	// chunk
	if( m_dataChunk.AllocateFromFile( fileName.c_str(), bCreate ) == false )
		return false;
	
	// section
	SECTIONMAP* sectionMap = new SECTIONMAP;
	if( ! sectionMap )
		return false;

	// parse
	ParseINI( sectionMap );
	
	// set member
	SetMemberData( fileName, sectionMap );
	
	// load state
	LoadEnd();
	
	// Return
	return true;
}

void ioINIControl::SetMemberData( tstring& fileName, SECTIONMAP* sectionMap )
{
	// insert
	InsertContainer( fileName, sectionMap );
}

void ioINIControl::InsertContainer( tstring& fileName, SECTIONMAP* sectionMap )
{
	// insert map
	m_fileMap.insert( map< tstring, SECTIONMAP* >::value_type( fileName, sectionMap ) );
}

void ioINIControl::ParseINI( SECTIONMAP* sectionMap )
{
	tstring line, lastline;

	while( ! m_dataChunk.IsEOF() )
	{
		if( lastline.empty() || lastline.at( 0 ) != _T( '[' ) )
		{
			m_dataChunk.GetLine( line );
		}
		else
		{
			line = lastline;
			lastline.at( 0 ) = _T( '\0' );
		}

		if( line.length() == 0 || line.at( 0 ) != _T( '[' ) )
			continue;

		lastline = ParseTitle( line, sectionMap );
	}
}

tstring ioINIControl::ParseTitle( tstring& title, SECTIONMAP* sectionMap )
{
	tstring line;

	KEYMAP* keyMap = new KEYMAP;
	if( ! keyMap )
		return line;
	
	while( ! m_dataChunk.IsEOF() )
	{
		m_dataChunk.GetLine( line );

		if( line.length() == 0 || line.at( 0 ) == _T( ';' ) )
			continue;

		if( line.at( 0 ) == _T( '[' ) )	// New Title
			break;

		ParseKey( line, keyMap );
	}
	
	if( ! keyMap->empty() )
	{
		tstring titleText = title.substr( 1, title.length() - 2 );
		transform( titleText.begin(), titleText.end(), titleText.begin(), tolower );
		sectionMap->insert( SECTIONMAP::value_type( titleText, keyMap ) );
	}
	else
	{
		delete keyMap;
	}

	return line;
}

void ioINIControl::ParseKey( const tstring& line, KEYMAP* keyMap )
{
	const STRINGVECTOR& stringParams = Split( line, _T( "=" ), 1 );
	if( stringParams.size() != 2 || stringParams[0].empty() )
		return;

	tstring keyName, value;

	tstring::size_type i, size;
	size = stringParams[0].length();
	for( i = size - 1 ; i >= 0 ; i-- )
	{
		if( stringParams[0].at(i) != _T( ' ' ) &&
			stringParams[0].at(i) != _T( '\t' ) &&
			stringParams[0].at(i) != _T( '\r' ) )
		{
			keyName = stringParams[0].substr( 0, i+1 );
			break;
		}
	}

	size = stringParams[1].length();
	for(i=0 ; i<size ; i++ )
	{
		if( stringParams[1].at(i) != _T( ' ' ) &&
			stringParams[1].at(i) != _T( '\t' ) &&
			stringParams[1].at(i) != _T( '\r' ) )
		{
			value = stringParams[1].substr( i );
			break;
		}
	}

	if( !keyName.empty() && !value.empty() )
	{
		transform( keyName.begin(), keyName.end(), keyName.begin(), tolower );
		keyMap->insert( KEYMAP::value_type( keyName, value ) );
	}
}

STRINGVECTOR ioINIControl::Split( const tstring& param, const tstring& delims, int32 maxSplit /*= 0*/ )
{
	STRINGVECTOR ret;
	int32 numSplits = 0;

	size_t start, pos;
	start = 0;

	do
	{
		pos = param.find_first_of( delims, start );
		if( pos == start )
		{
			start = pos + 1;
		}
		else if( pos == param.npos || ( maxSplit && numSplits == maxSplit ) )
		{
			ret.push_back( param.substr(start) );
			break;
		}
		else
		{
			ret.push_back( param.substr( start, pos - start ) );
			start = pos + 1;
		}

		start = param.find_first_not_of( delims, start );
		++numSplits;
	}while( pos != param.npos );

	return ret;
}

void ioINIControl::RemoveFileName( tstring& fileName )
{
	FILEMAP::iterator iter = m_fileMap.find( fileName );
	if( iter != m_fileMap.end() )
	{
		SECTIONMAP::iterator	sectionIter		= ( (*iter).second )->begin();
		SECTIONMAP::iterator	sectionIterEnd	= ( (*iter).second )->end();

		while( sectionIter != sectionIterEnd )
		{
			delete (*sectionIter).second;
			++sectionIter;
		}

		delete (*iter).second;
		m_fileMap.erase( iter );
	}
}

const SECTIONMAP* ioINIControl::FindSectionMap( const TCHAR* fileName )
{
	tstring name( fileName );
	transform( name.begin(), name.end(), name.begin(), tolower );

	FILEMAP::const_iterator iter = m_fileMap.find( name );
	if( iter != m_fileMap.end() )
	{
		return (*iter).second;
	}
	return NULL;
}

KEYMAP* ioINIControl::FindKeyMap( const TCHAR* section, const SECTIONMAP* sectionList )
{
	tstring sectionString = section;
	transform( sectionString.begin(), sectionString.end(), sectionString.begin(), tolower );

	// section map
	SECTIONMAP::const_iterator sectionIter = sectionList->find( sectionString );
	if( sectionIter != sectionList->end() )
	{
		return (*sectionIter).second;
	}

	return NULL;
}

KEYMAP::iterator ioINIControl::FindValue( const TCHAR* keyName, KEYMAP* keyList )
{
	tstring keyString = keyName;
	transform( keyString.begin(), keyString.end(), keyString.begin(), tolower );

	// value
	return keyList->find( keyString );
}

// load
const bool ioINIControl::LoadBool( const TCHAR* fileName, const TCHAR* section, const TCHAR* keyName, const bool defaultValue )
{
	// section
	const SECTIONMAP* sectionList = FindSectionMap( fileName );
	if( sectionList == NULL )
		return defaultValue;

	// key
	KEYMAP* keyList = FindKeyMap( section, sectionList );
	if( keyList == NULL )
		return defaultValue;
		
	// value
	KEYMAP::iterator keyIter = FindValue( keyName, keyList );
	if( keyIter == keyList->end() )
		return defaultValue;

	return _ttoi( keyIter->second.c_str() ) != 0 ? true : false;
}

const int32 ioINIControl::LoadInt( const TCHAR* fileName, const TCHAR* section, const TCHAR* keyName, const int32 defaultValue )
{
	// section
	const SECTIONMAP* sectionList = FindSectionMap( fileName );
	if( sectionList == NULL )
		return defaultValue;

	// key
	KEYMAP* keyList = FindKeyMap( section, sectionList );
	if( keyList == NULL )
		return defaultValue;

	// value
	KEYMAP::iterator keyIter = FindValue( keyName, keyList );
	if( keyIter == keyList->end() )
		return defaultValue;

	return _ttoi( keyIter->second.c_str() );
}

const float ioINIControl::LoadFloat( const TCHAR* fileName, const TCHAR* section, const TCHAR* keyName, const float defaultValue )
{
	// section
	const SECTIONMAP* sectionList = FindSectionMap( fileName );
	if( sectionList == NULL )
		return defaultValue;

	// key
	KEYMAP* keyList = FindKeyMap( section, sectionList );
	if( keyList == NULL )
		return defaultValue;

	// value
	KEYMAP::iterator keyIter = FindValue( keyName, keyList );
	if( keyIter == keyList->end() )
		return defaultValue;

	return static_cast< float >( _tstof( keyIter->second.c_str() ) );
}

void ioINIControl::LoadString( const TCHAR* fileName, const TCHAR* section, const TCHAR* keyName, const TCHAR* Default, TCHAR* buf, const int32 bufLen )
{
	// section
	const SECTIONMAP* sectionList = FindSectionMap( fileName );
	if( sectionList == NULL )
	{
		StringCbCopyN( buf, bufLen, Default, bufLen - 1  );
		return;
	}

	// key
	KEYMAP* keyList = FindKeyMap( section, sectionList );
	if( keyList == NULL )
	{
		StringCbCopyN( buf, bufLen, Default, bufLen - 1  );
		return;
	}

	// value
	KEYMAP::iterator keyIter = FindValue( keyName, keyList );
	if( keyIter == keyList->end() )
	{
		StringCbCopyN( buf, bufLen, Default, bufLen - 1  );
		return;
	}

	// Set Value
	StringCbCopyN( buf, bufLen, keyIter->second.c_str(), bufLen - 1  );
}

const bool ioINIControl::ReadBool( const TCHAR* fileName, const TCHAR* section, const TCHAR* keyName, const bool defaultValue )
{
	int32 resultValue = 0;

	if( defaultValue )
		resultValue = 1;
	
	resultValue = GetPrivateProfileInt( section, keyName, resultValue, fileName );

	return resultValue != 0 ? true : false;
}

const int32 ioINIControl::ReadInt( const TCHAR* fileName, const TCHAR* section, const TCHAR* keyName, const int32 defaultValue )
{
	return GetPrivateProfileInt( section, keyName, defaultValue, fileName );
}

const float ioINIControl::ReadFloat( const TCHAR* fileName, const TCHAR* section, const TCHAR* keyName, const float defaultValue )
{
	float resultValue = 0;

	TCHAR szBuf[ MAX_PATH ] = { 0, };
	GetPrivateProfileString( section, keyName, _T(""), szBuf, MAX_PATH, fileName );
	if( _tcscmp( szBuf, _T("") ) )
		resultValue = static_cast< float >( _tstof( szBuf ) );
	else
		resultValue = defaultValue;

	return resultValue;
}

void ioINIControl::ReadString( const TCHAR* fileName, const TCHAR* section, const TCHAR* keyName, const TCHAR* defaultValue, TCHAR* buf, const int32 bufLen )
{
	GetPrivateProfileString( section, keyName, defaultValue, buf, bufLen, fileName );
}


// save
KEYMAP::iterator ioINIControl::InsertValue( const TCHAR* keyName, KEYMAP* keyList )
{
	tstring keySring = keyName;
	transform( keySring.begin(), keySring.end(), keySring.begin(), tolower );

	keyList->insert( KEYMAP::value_type(keySring, "0") );

	return FindValue(keyName, keyList);
}

KEYMAP* ioINIControl::InsertKeyMap( SECTIONMAP* section, const TCHAR* sectionName )
{
	KEYMAP* mKeyMap = new KEYMAP;

	tstring sectionString = sectionName;
	transform( sectionString.begin(), sectionString.end(), sectionString.begin(), tolower );

	section->insert( SECTIONMAP::value_type(sectionString, mKeyMap) );
	return mKeyMap;
}

void ioINIControl::SaveString( const TCHAR* fileName, const TCHAR* section, const TCHAR* key, const TCHAR* data )
{
	// file save
	WritePrivateProfileString( section, key, data, fileName );

	// section
	const SECTIONMAP* sectionList = FindSectionMap( fileName );
	if( sectionList == NULL )
		return;

	// key
	KEYMAP* keyList = FindKeyMap( section, sectionList );
	if( keyList == NULL )
		keyList = InsertKeyMap((SECTIONMAP*)sectionList, section);

	// value
	KEYMAP::iterator keyIter = FindValue( key, keyList );
	if( keyIter == keyList->end() )
		keyIter = InsertValue(key, keyList);

	// memory save
	(*keyIter).second = data;
}

void ioINIControl::SaveInt( const TCHAR* fileName, const TCHAR* section, const TCHAR* key, const int32 value )
{
	TCHAR szBuf[MAX_PATH]= _T( "" );
	StringCbPrintf( szBuf, sizeof( szBuf ), _T( "%d" ), value );

	// section
	const SECTIONMAP* sectionList = FindSectionMap( fileName );
	if( sectionList == NULL )
		return;

	// key
	KEYMAP* keyList = FindKeyMap( section, sectionList );
	if( keyList == NULL )
		keyList = InsertKeyMap((SECTIONMAP*)sectionList, section);

	// value
	KEYMAP::iterator keyIter = FindValue( key, keyList );
	if( keyIter == keyList->end() )
		keyIter = InsertValue(key, keyList);

	// 
	(*keyIter).second = szBuf;

	if(GetThreadState())
	{
		ioININode* node = GetDataNode();			
		if( node )
		{
			if( !node->SetData( fileName, section, key, value ) )
				return;
			
			PushNode(node);
		}
		else
		{
			WritePrivateProfileString( section, key, szBuf, fileName );
		}
	}
	else
	{
		// file save
		WritePrivateProfileString( section, key, szBuf, fileName );
	}
}

void ioINIControl::SaveFloat( const TCHAR* fileName, const TCHAR* section, const TCHAR* key, const float value, const bool noLimit /*= false*/ )
{
	TCHAR szBuf[MAX_PATH]= _T( "" );

	if( noLimit )
	{
		StringCbPrintf( szBuf, sizeof( szBuf ), _T( "%f" ), value );
	}
	else
	{
		StringCbPrintf( szBuf, sizeof( szBuf ), _T( "%.2f" ), value );
	}

	// section
	const SECTIONMAP* sectionList = FindSectionMap( fileName );
	if( sectionList == NULL )
		return;

	// key
	KEYMAP* keyList = FindKeyMap( section, sectionList );
	if( keyList == NULL )
		keyList = InsertKeyMap((SECTIONMAP*)sectionList, section);

	// value
	KEYMAP::iterator keyIter = FindValue( key, keyList );
	if( keyIter == keyList->end() )
		keyIter = InsertValue(key, keyList);

	// 
	(*keyIter).second = szBuf;

	if(GetThreadState())
	{
		ioININode* node = GetDataNode();
		if( node )
		{
			if(noLimit)
			{
				if( !node->SetData( fileName, section, key, value, 3 ) )
					return;
			}
			else
			{
				if( !node->SetData( fileName, section, key, value, 4 ) )
					return;
			}	
			PushNode(node);
		}
		else
		{
			WritePrivateProfileString( section, key, szBuf, fileName );
		}
	}
	else
	{
		// file save
		WritePrivateProfileString( section, key, szBuf, fileName );
	}	
}

void ioINIControl::SaveBool( const TCHAR* fileName, const TCHAR* section, const TCHAR* key, const bool value )
{
	// section
	const SECTIONMAP* sectionList = FindSectionMap( fileName );
	if( sectionList == NULL )
		return;

	// key
	KEYMAP* keyList = FindKeyMap( section, sectionList );
	if( keyList == NULL )
		keyList = InsertKeyMap((SECTIONMAP*)sectionList, section);

	// value
	KEYMAP::iterator keyIter = FindValue( key, keyList );
	if( keyIter == keyList->end() )
		keyIter = InsertValue(key, keyList);

	// 
	(*keyIter).second = value == true ? _T( "1" ) : _T( "0" );

	if(GetThreadState())
	{
		ioININode* node = GetDataNode();
		if( node )
		{
			if( !node->SetData( fileName, section, key, value ) )
				return;

			PushNode(node);
		}
		else
		{
			if( value )
			{
				WritePrivateProfileString( section, key, _T( "1" ), fileName );
			}
			else
			{
				WritePrivateProfileString( section, key, _T( "0" ), fileName );
			}
		}
	}
	else
	{
		// file save
		if( value )
		{
			WritePrivateProfileString( section, key, _T( "1" ), fileName );
		}
		else
		{
			WritePrivateProfileString( section, key, _T( "0" ), fileName );
		}
	}
}
