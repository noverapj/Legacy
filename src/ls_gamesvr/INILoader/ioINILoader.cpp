

#include "stdafx.h"

#include "ioINIParser.h"
#include "ioINILoader.h"
#include <strsafe.h>

ioINILoader::ioINILoader()
{
	ZeroMemory( m_szFileName, MAX_PATH );
	ZeroMemory( m_szTitle, MAX_PATH );
	m_pParser = NULL;
}

ioINILoader::ioINILoader( const char *szFileName , bool bMemoryParsing )
{
	ZeroMemory( m_szFileName, MAX_PATH );
	ZeroMemory( m_szTitle, MAX_PATH );

	GetCurrentDirectory( MAX_PATH, m_szFileName );
	StringCbCat( m_szFileName, sizeof( m_szFileName ), "\\" );
	StringCbCat( m_szFileName, sizeof( m_szFileName ), szFileName );
	
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
	m_pParser = NULL;
	
	if(bMemoryParsing)
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

	return fValue;
}

void ioINILoader::LoadVector( const char *szTitle, Vector3 &vVec )
{
	vVec.x = LoadFloat( szTitle, "X", 0.0f );
	vVec.y = LoadFloat( szTitle, "Y", 0.0f );
	vVec.z = LoadFloat( szTitle, "Z", 0.0f );
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
			StringCbCopyN( szBuf, iBufLen, szValue, iBufLen - 1  );
		}
		else
		{
			StringCbCopyN( szBuf, iBufLen, szDefault, iBufLen - 1  );
		}
	}
	else
	{
		GetPrivateProfileString( szTitle, szKeyName, szDefault, szBuf, iBufLen, m_szFileName );
	}
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

void ioINILoader::LoadVector( Vector3 &vVec )
{
	LoadVector( m_szTitle, vVec );
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

void ioINILoader::SaveVector( const char *szTitle, const Vector3 &vVec )
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

void ioINILoader::SaveVector( const Vector3 &vVec )
{
	SaveVector( m_szTitle, vVec );
}