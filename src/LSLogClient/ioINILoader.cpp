

#include "stdafx.h"

#include "ioINILoader.h"

ioINILoader::ioINILoader()
{
	ZeroMemory( m_szFileName, MAX_PATH );
	ZeroMemory( m_szTitle, MAX_PATH );
}

ioINILoader::ioINILoader( const char *szFileName )
{
	ZeroMemory( m_szFileName, MAX_PATH );
	GetCurrentDirectory( MAX_PATH, m_szFileName );

	strcat( m_szFileName, "\\" );
	strcat( m_szFileName, szFileName );
	ZeroMemory( m_szTitle, MAX_PATH );
}

ioINILoader::ioINILoader( const char *szPath, const char *szFileName )
{
	wsprintf( m_szFileName, "%s/%s", szPath, szFileName );
}

void ioINILoader::SetFileName( const char *szFileName )
{
	strcpy( m_szFileName, szFileName );
}

void ioINILoader::SetTitle( const char *szTitle )
{
	strcpy( m_szTitle, szTitle );
}

bool ioINILoader::LoadBool( const char *szTitle, const char *szKeyName, bool bDefault )
{
	int iValue = 0;
	if( bDefault )
	{
		iValue = 1;
	}

	iValue = GetPrivateProfileInt( szTitle, szKeyName, iValue, m_szFileName );

	if( iValue != 0 )
		return true;

	return false;

}

int ioINILoader::LoadInt( const char *szTitle, const char *szKeyName, int iDefault )
{
	return GetPrivateProfileInt( szTitle, szKeyName, iDefault, m_szFileName );
}

float ioINILoader::LoadFloat( const char *szTitle, const char *szKeyName, float fDefault )
{
	char szDefault[MAX_PATH];
	sprintf( szDefault, "%f", fDefault );

	char szBuf[MAX_PATH];
	GetPrivateProfileString( szTitle, szKeyName, szDefault, szBuf, MAX_PATH, m_szFileName );

	return (float)atof(szBuf);
}

void ioINILoader::LoadVector( const char *szTitle, Vector3 &vVec )
{
	char szTemp[MAX_PATH];

	GetPrivateProfileString( szTitle, "X", "0.0f", szTemp, MAX_PATH, m_szFileName );
	vVec.x = (float)atof(szTemp);

	GetPrivateProfileString( szTitle, "Y", "0.0f", szTemp, MAX_PATH, m_szFileName );
	vVec.y = (float)atof(szTemp);

	GetPrivateProfileString( szTitle, "Z", "0.0f", szTemp, MAX_PATH, m_szFileName );
	vVec.z = (float)atof(szTemp);
}

void ioINILoader::LoadString( const char *szTitle,
							  const char *szKeyName,
							  const char *szDefault,
							  char *szBuf,
							  int iBufLen )
{
	GetPrivateProfileString( szTitle, szKeyName, szDefault, szBuf, iBufLen, m_szFileName );
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
	char szBuf[MAX_PATH];
	wsprintf( szBuf, "%d", iValue );

	WritePrivateProfileString( szTitle, szKeyName, szBuf, m_szFileName );
}

void ioINILoader::SaveFloat( const char *szTitle, const char *szKeyName, float fValue, bool bNoLimit )
{
	char szBuf[MAX_PATH];

	if( bNoLimit )
	{
		sprintf( szBuf, "%f", fValue );
	}
	else
	{
		sprintf( szBuf, "%.2f", fValue );
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
	char szBuf[MAX_PATH];

	sprintf( szBuf, "%.2f", vVec.x );
	WritePrivateProfileString( szTitle, "X", szBuf, m_szFileName );

	sprintf( szBuf, "%.2f", vVec.y );
	WritePrivateProfileString( szTitle, "Y", szBuf, m_szFileName );

	sprintf( szBuf, "%.2f", vVec.z );
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