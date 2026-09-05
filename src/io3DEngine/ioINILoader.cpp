
#include "stdafx.h"

#include "ioINIParser.h"
#include "ioINILoader.h"
#include "iostringmanager.h"
#include <strsafe.h>

ioINILoader::ioINILoader()
{
	ZeroMemory( m_szFileName, MAX_PATH );
	ZeroMemory( m_szTitle, MAX_PATH );
#ifdef STRING_MANAGER 
	ZeroMemory( m_szStringMgrKeyName, MAX_PATH );
#endif 

	m_pParser = NULL;
}

ioINILoader::ioINILoader( const char *szFileName , bool bMemoryParsing )
{
	ZeroMemory( m_szFileName, MAX_PATH );
	ZeroMemory( m_szTitle, MAX_PATH );

	StringCbCopy( m_szFileName, sizeof( m_szFileName ), szFileName );
#ifdef STRING_MANAGER 
	ZeroMemory( m_szStringMgrKeyName, MAX_PATH );
	_splitpath( m_szFileName, NULL, NULL, m_szStringMgrKeyName, NULL );
#endif 

	m_pParser = NULL;

	if(bMemoryParsing)
	{
		DoMemoryParsing();
	}
}

ioINILoader::ioINILoader( const char *szPath, const char *szFileName , bool bMemoryParsing )
{
	ZeroMemory( m_szFileName, MAX_PATH );
	ZeroMemory( m_szTitle, MAX_PATH );

	StringCbPrintf( m_szFileName, sizeof( m_szFileName ), "%s/%s", szPath, szFileName );
#ifdef STRING_MANAGER 
	ZeroMemory( m_szStringMgrKeyName, MAX_PATH );
	_splitpath( m_szFileName, NULL, NULL, m_szStringMgrKeyName, NULL );
#endif 

	m_pParser = NULL;
	
	if(bMemoryParsing)
	{
		DoMemoryParsing();
	}
}

ioINILoader::ioINILoader( const ioINILoader &rhs ) : m_pParser( NULL )
{
	ZeroMemory( m_szFileName, MAX_PATH );
	ZeroMemory( m_szTitle, MAX_PATH );
	StringCbCopy( m_szFileName, sizeof( m_szFileName ), rhs.m_szFileName );
	StringCbCopy( m_szTitle, sizeof( m_szTitle ), rhs.m_szTitle );
#ifdef STRING_MANAGER 
	ZeroMemory( m_szStringMgrKeyName, MAX_PATH );
	StringCbCopy( m_szStringMgrKeyName, sizeof( m_szStringMgrKeyName ), rhs.m_szStringMgrKeyName );
#endif 

	if( rhs.m_pParser )
	{
		DoMemoryParsing();
	}
}

ioINILoader::~ioINILoader()
{
	SAFEDELETE( m_pParser );
}

void ioINILoader::SetFileName( const char *szFileName , bool bMemoryParsing )
{
	StringCbCopy( m_szFileName, sizeof( m_szFileName ), szFileName );
#ifdef STRING_MANAGER 
	_splitpath( m_szFileName, NULL, NULL, m_szStringMgrKeyName, NULL );
#endif 

	SAFEDELETE( m_pParser );

	if(bMemoryParsing)
	{
		DoMemoryParsing();
	}
}

void ioINILoader::SetTitle( const char *szTitle )
{
	StringCbCopy( m_szTitle, sizeof( m_szTitle ), szTitle );
}

bool ioINILoader::DoMemoryParsing()
{
	SAFEDELETE( m_pParser );

	m_pParser = new ioINIParser;
	if( m_pParser->ParsingFile( m_szFileName ) )
		return true;

	SAFEDELETE( m_pParser );

	return false;
}

bool ioINILoader::DoMemoryParsingFromMemory( ioTextStream &rkStream )
{
	SAFEDELETE( m_pParser );
	m_pParser = new ioINIParser;
	m_pParser->ParseINI( rkStream ); 
	return true;
}

bool ioINILoader::LoadBool( const char *szTitle, const char *szKeyName, bool bDefault )
{
	int iValue = 0;
	if( bDefault )
	{
		iValue = 1;
	}

	if( m_pParser )
	{
		const char *szValue = m_pParser->GetValue( szTitle, szKeyName );
		if( szValue )
		{
			iValue = atoi( szValue );
		}
	}
	else
	{
		iValue = GetPrivateProfileInt( szTitle, szKeyName, iValue, m_szFileName );
	}

	if( ioINIMemoryStatic::GetSingletonPtr() )
		ioINIMemoryStatic::GetSingleton().AddMem( m_szFileName, szTitle, sizeof( bool ) );

	if( iValue != 0 )
		return true;

	return false;

}

int ioINILoader::LoadInt( const char *szTitle, const char *szKeyName, int iDefault )
{
	int iValue = iDefault;
	if( m_pParser )
	{
		const char *szValue = m_pParser->GetValue( szTitle, szKeyName );
		if( szValue )
		{
			iValue = atoi( szValue );
		}
	}
	else
	{
		iValue = GetPrivateProfileInt( szTitle, szKeyName, iDefault, m_szFileName );
	}

	if( ioINIMemoryStatic::GetSingletonPtr() )
		ioINIMemoryStatic::GetSingleton().AddMem( m_szFileName, szTitle, sizeof( int ) );

	return iValue;
}

float ioINILoader::LoadFloat( const char *szTitle, const char *szKeyName, float fDefault )
{
	float fValue = fDefault;
	
	if( m_pParser )
	{
		const char *szValue = m_pParser->GetValue( szTitle, szKeyName );
		if( szValue )
			fValue = atof( szValue );
		else
			fValue = fDefault;
	}
	else
	{
		char szBuf[MAX_PATH];
		GetPrivateProfileString( szTitle, szKeyName, "", szBuf, MAX_PATH, m_szFileName );
		if( strcmp( szBuf, "" ) )
			fValue = (float)atof( szBuf );
		else
			fValue = fDefault;
	}


	if( ioINIMemoryStatic::GetSingletonPtr() )
		ioINIMemoryStatic::GetSingleton().AddMem( m_szFileName, szTitle, sizeof( float ) );

	return fValue;
}

void ioINILoader::LoadVector( const char *szTitle, D3DXVECTOR3 *pVec )
{
	pVec->x = LoadFloat( szTitle, "X", 0.0f );
	pVec->y = LoadFloat( szTitle, "Y", 0.0f );
	pVec->z = LoadFloat( szTitle, "Z", 0.0f );

	if( ioINIMemoryStatic::GetSingletonPtr() )
		ioINIMemoryStatic::GetSingleton().AddMem( m_szFileName, szTitle, sizeof( D3DXVECTOR3 ) );
}

void ioINILoader::LoadString( const char *szTitle,
							  const char *szKeyName,
							  const char *szDefault,
							  char *szBuf,
							  int iBufLen )
{
	if( m_pParser )
	{
		const char *szValue = m_pParser->GetValue( szTitle, szKeyName );
		if( szValue )
		{
			StringCbCopyN( szBuf, iBufLen, szValue, iBufLen - 1 );
#ifdef STRING_MANAGER 
			if( ioStringManager::GetSingletonPtr() )
				g_StringMgr.GetStringINI( m_szStringMgrKeyName, szTitle, szBuf, iBufLen );
#endif 			
		}
		else
		{
			StringCbCopyN( szBuf, iBufLen, szDefault, iBufLen - 1 );			
		}
	}
	else
	{
		GetPrivateProfileString( szTitle, szKeyName, szDefault, szBuf, iBufLen, m_szFileName );
#ifdef STRING_MANAGER 
		if( ioStringManager::GetSingletonPtr() )
			g_StringMgr.GetStringINI( m_szStringMgrKeyName, szTitle, szBuf, iBufLen );		
#endif 
	}

	if( ioINIMemoryStatic::GetSingletonPtr() )
		ioINIMemoryStatic::GetSingleton().AddMem( m_szFileName, szTitle, sizeof( char ) * strlen( szBuf ) );
}

bool ioINILoader::LoadBool( const char *szKeyName, bool bDefault )
{
	return LoadBool( m_szTitle, szKeyName, bDefault );
}

int ioINILoader::LoadInt( const char *szKeyName, int iDefault )
{
	return LoadInt( m_szTitle, szKeyName, iDefault );
}

float ioINILoader::LoadFloat( const char *szKeyName, float fDefault )
{
	return LoadFloat( m_szTitle, szKeyName, fDefault );
}

void ioINILoader::LoadVector( D3DXVECTOR3 *pVec )
{
	LoadVector( m_szTitle, pVec );
}

void ioINILoader::LoadString( const char *szKeyName,
							  const char *szDefault,
							  char *szBuf,
							  int iBufLen )
{
	LoadString( m_szTitle, szKeyName, szDefault, szBuf, iBufLen );
}

void ioINILoader::SaveString( const char *szTitle, const char *szKeyName, const char *szBuf )
{
	WritePrivateProfileString( szTitle, szKeyName, szBuf, m_szFileName );
}

void ioINILoader::SaveInt( const char *szTitle, const char *szKeyName, int iValue )
{
	char szBuf[MAX_PATH]="";
	StringCbPrintf( szBuf, sizeof( szBuf ), "%d", iValue );

	WritePrivateProfileString( szTitle, szKeyName, szBuf, m_szFileName );
}

void ioINILoader::SaveFloat( const char *szTitle, const char *szKeyName, float fValue, bool bNoLimit )
{
	char szBuf[MAX_PATH]="";

	if( bNoLimit )
	{
		StringCbPrintf( szBuf, sizeof( szBuf ), "%f", fValue );
	}
	else
	{
		StringCbPrintf( szBuf, sizeof( szBuf ), "%.2f", fValue );
	}

	WritePrivateProfileString( szTitle, szKeyName, szBuf, m_szFileName );
}

void ioINILoader::SaveBool( const char *szTitle, const char *szKeyName, bool bValue )
{
	if( bValue )
	{
		WritePrivateProfileString( szTitle, szKeyName, "1", m_szFileName );
	}
	else
	{
		WritePrivateProfileString( szTitle, szKeyName, "0", m_szFileName );
	}
}

void ioINILoader::SaveVector( const char *szTitle, const D3DXVECTOR3 &vVec )
{
	char szBuf[MAX_PATH]="";

	StringCbPrintf( szBuf, sizeof( szBuf ), "%.2f", vVec.x );
	WritePrivateProfileString( szTitle, "X", szBuf, m_szFileName );

	StringCbPrintf( szBuf, sizeof( szBuf ), "%.2f", vVec.y );
	WritePrivateProfileString( szTitle, "Y", szBuf, m_szFileName );

	StringCbPrintf( szBuf, sizeof( szBuf ), "%.2f", vVec.z );
	WritePrivateProfileString( szTitle, "Z", szBuf, m_szFileName );
}

void ioINILoader::SaveString( const char *szKeyName, const char *szBuf )
{
	SaveString( m_szTitle, szKeyName, szBuf );
}

void ioINILoader::SaveInt( const char *szKeyName, int iValue )
{
	SaveInt( m_szTitle, szKeyName, iValue );
}

void ioINILoader::SaveFloat( const char *szKeyName, float fValue, bool bNoLimit )
{
	SaveFloat( m_szTitle, szKeyName, fValue, bNoLimit );
}

void ioINILoader::SaveBool( const char *szKeyName, bool bValue )
{
	SaveBool( m_szTitle, szKeyName, bValue );
}

void ioINILoader::SaveVector( const D3DXVECTOR3 &vVec )
{
	SaveVector( m_szTitle, vVec );
}


int ioINILoader::GetNumTotalTitle() const
{
	if( m_pParser )
		return m_pParser->GetNumTotalTitle();

	return 0;
}

int ioINILoader::GetNumTotalKey( int iTitleIdx ) const
{
	if( m_pParser )
		return m_pParser->GetNumTotalKey( iTitleIdx );

	return 0;
}

const char* ioINILoader::GetTitle( int iIdx ) const
{
	if( m_pParser )
		return m_pParser->GetTitle( iIdx );

	return "";
}

const char* ioINILoader::GetKey( int iTitle, int iKey ) const
{
	if( m_pParser )
		return m_pParser->GetKey( iTitle, iKey );

	return "";
}

const char* ioINILoader::GetValue( int iTitle, int iKey ) const
{
	if( m_pParser )
		return m_pParser->GetValue( iTitle, iKey );

	return NULL;
}



