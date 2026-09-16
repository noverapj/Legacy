#include "stdafx.h"
#include ".\iostringmanager.h"
#include <strsafe.h>

#ifndef TEXT_PIECE_LOAD
	#include "ioStream.h"
	#include "ioResourceLoader.h"
#endif

template<> ioStringManager* Singleton< ioStringManager >::ms_Singleton = 0;

ioStringManager::ioStringManager(void)
{
}

ioStringManager::~ioStringManager(void)
{
	m_AppStringMap.clear();
	m_UiStringMap.clear();
	m_ConfigStringMap.clear();
}

ioStringManager & ioStringManager::GetSingleton()
{
	return Singleton< ioStringManager >::GetSingleton();
}

ioStringManager::StringInfoMap& ioStringManager::GetTable( FileNamePrefix eFileNamePrefix )
{
	switch( eFileNamePrefix )
	{
	case FNP_XML:
		return m_UiStringMap;
	case FNP_INI:
		return m_ConfigStringMap;
	case FNP_EXE:
	default:
		return m_AppStringMap;
	}
}

ioStringManager::StringInfoMap& ioStringManager::GetTableByKey( const char *szKey )
{
	if( strnicmp( szKey, "xml_", 4 ) == 0 )
		return m_UiStringMap;
	else if( strnicmp( szKey, "ini_", 4 ) == 0 )
		return m_ConfigStringMap;
	return m_AppStringMap;
}

char * ioStringManager::GetString( const char *szFileName, const char *szSecondKeyName, int iNum, FileNamePrefix eFileNamePrefix )
{
	static char szKeyName[MAX_PATH]="";
	if( eFileNamePrefix == FNP_EXE )
		StringCbPrintf( szKeyName, sizeof( szKeyName ), "exe_%s_%d", szFileName, iNum );
	else if( eFileNamePrefix == FNP_XML )
		StringCbPrintf( szKeyName, sizeof( szKeyName ), "xml_%s_%d", szFileName, iNum );
	else if( eFileNamePrefix == FNP_INI )
		StringCbPrintf( szKeyName, sizeof( szKeyName ), "ini_%s::%s_%d", szFileName, szSecondKeyName, iNum );
	else
		return szKeyName;

	strlwr( szKeyName );

	StringInfoMap &kTable = GetTable( eFileNamePrefix );
	StringInfoMap::iterator iter = kTable.find( szKeyName );
	if( iter != kTable.end() )
		return (char*)iter->second.c_str();

	return szKeyName;
}


int ioStringManager::GetStringMgrTextNum( const char *szText )
{
	if( !strstr( szText, "STR(" ) )
		return 0;

	int iSize = (int)strlen( szText );
	int iNum = 0;
	for (int i = 0; i < iSize ; i++)
	{
		if( szText[i] == '(' )
		{
			iNum = atoi( &szText[i+1] );
			break;
		}
	}

	if( iNum == 0 )
		return 0;

	return iNum;
}

const char *ioStringManager::GetStringXML( const char *szKey, const char *szText )
{
	int iNum = GetStringMgrTextNum( szText );
	if( iNum == 0 )
		return szText;

	return GetString( szKey, "", iNum, FNP_XML );
}

void ioStringManager::GetStringINI( IN const char *szKey, IN const char *szSecondKey, OUT char *szText, IN int iTextSize )
{
	int iNum = GetStringMgrTextNum( szText );
	if( iNum == 0 )
		return;

	StringCbCopy( szText, iTextSize, GetString( szKey, szSecondKey,  iNum, FNP_INI ) );
}

const char* ioStringManager::GetStringByID(const char* key)
{
	char cKey[MAX_PATH] = {0,};

	int nSize = strlen(key);

	for(int i = 0; i < nSize; ++i)
	{
		if(key[i] <= 'Z' && key[i] >= 'A')
		{
			cKey[i] = key[i]+32;
		}else{
			cKey[i] = key[i];
		}
	}

	StringInfoMap &kTable = GetTableByKey( cKey );
	StringInfoMap::iterator it = kTable.find(ioHashString(cKey));
	if(it == kTable.end())
	{
		return key;
	}

	return it->second.c_str();
}

void ioStringManager::GetStringByID(const char* key, char* out, int len)
{

	StringInfoMap &kTable = GetTableByKey( key );
	StringInfoMap::iterator it = kTable.find(ioHashString(key));
	if(it == kTable.end())
	{
		sprintf_s(out, len, "%s", key);
		return;
	}

	sprintf_s(out, len, "%s", it->second.c_str());
}

void ioStringManager::LoadData( const char *szPath, const char *szFileName, FileNamePrefix eTable, const char *szMemTextList /* = NULL */, bool bLoadMemText /* = false */ )
{
	StringInfoMap &kTable = GetTable( eTable );
	kTable.clear();
	
	char szFullPath[MAX_PATH] ="";
#ifndef EXTRACT_HANGEUL_TOOL
	StringCbPrintf( szFullPath, sizeof( szFullPath ), "%s/%s", szPath, szFileName );
#else
	StringCbPrintf( szFullPath, sizeof( szFullPath ), "%s%s", szPath, szFileName );
#endif

	BYTE *pPtr      = NULL;
	int iBufferSize = 0;
#ifndef TEXT_PIECE_LOAD
	ioTextStream kStream;
	if( bLoadMemText )
	{
		pPtr        = (BYTE*)szMemTextList;
		iBufferSize = strlen(szMemTextList);
	}
	else
	{
		if( !g_ResourceLoader.LoadStream( szFullPath, &kStream ) )
		{
			LOG.PrintTimeAndLog( 0, "%s - %s Load Failed", __FUNCTION__, szFileName );
			return;
		}

		pPtr = kStream.GetPtr();
		iBufferSize= kStream.GetSize();
	}
#else
	pPtr        = (BYTE*)szMemTextList;
	iBufferSize = strlen(szMemTextList);

	if( iBufferSize <= 0 )
	{
		return;
	}
#endif

	enum MarkState
	{
		MS_KEY      = 1,
		MS_TEXT     = 2,
		MS_TEXT_END = 3,
	};

	MarkState eState = MS_TEXT_END;

	enum LeadByteState
	{
		LBS_LEAD       = 1,
		LBS_LEAD_AFTER = 2,
		LBS_NOT_LEAD   = 3,
	};

	LeadByteState eLeadState = LBS_NOT_LEAD;

	int iLine = 0;
	int iCnt  = 0;
	enum { PATH_SIZE = MAX_PATH*2, };
	char szKey[PATH_SIZE+1]="";
	char szText[PATH_SIZE+1]="";
	for (int i = 0; i < iBufferSize ; i++)
	{
		if( eLeadState == LBS_NOT_LEAD || eLeadState == LBS_LEAD_AFTER ) 
		{

			if( ioText::IsLeadByte( (BYTE)pPtr[i] ) )

				eLeadState = LBS_LEAD;
			else
				eLeadState = LBS_NOT_LEAD;
		}
		else if( eLeadState == LBS_LEAD )
			eLeadState = LBS_LEAD_AFTER;

		if( pPtr[i] == '|' && eLeadState == LBS_NOT_LEAD )
		{
			if( eState == MS_TEXT_END )
			{
				eState = MS_KEY;
				continue;
			}
			else if( eState == MS_KEY )
			{
				eState = MS_TEXT;
				if( COMPARE( iCnt , 0 , PATH_SIZE ) )
					szKey[iCnt]=NULL;

				iCnt = 0;
				continue;
			}
			else if( eState == MS_TEXT )
			{
				eState = MS_TEXT_END;
				if( COMPARE( iCnt , 0 , PATH_SIZE ) )
					szText[iCnt]=NULL;
				iCnt = 0;

#ifndef EXTRACT_HANGEUL_TOOL
				strlwr( szKey );
#endif
				StringInfoMap::iterator iter = kTable.find( szKey );
				if( iter != kTable.end() )
				{
					if( !bLoadMemText )
						LOG.PrintTimeAndLog( 0, "%s duplicate Text :Line%d:%s" , __FUNCTION__, iLine, szKey );
					continue;
				}

				kTable.insert( StringInfoMap::value_type( ioHashString( szKey ), ioHashString( szText ) ) );
				continue;
			}
		}
		else if( pPtr[i] == 13 && pPtr[i+1] == 10 ) // 줄바꿈
		{
			if( !bLoadMemText )
			{
				iLine++;
				i++;

				if( eState == MS_TEXT_END )
					continue;

				eState = MS_TEXT_END;
				if( COMPARE( iCnt , 0 , PATH_SIZE ) )
					szText[iCnt]=NULL;
				iCnt = 0;
				if( !bLoadMemText )
					LOG.PrintTimeAndLog( 0, "%s Error Text : Line%d:%s" , __FUNCTION__, iLine, szText );
				continue;
			}
		}

		if( eState == MS_KEY )
		{
			if( COMPARE( iCnt , 0 , PATH_SIZE ) )
			{
				szKey[iCnt] = pPtr[i];
				iCnt++;
			}
		}
		else if( eState == MS_TEXT )
		{
			if( COMPARE( iCnt , 0 , PATH_SIZE ) )
			{
				// 92
				if( pPtr[i] == '\\' ) // 텍스트의 \r과 \n을 실재 개행 문자로 변경
				{
					// 114
					if( pPtr[i+1] == 'r' )
					{
						szText[iCnt]=13; // \r
						iCnt++;
						i++;
					}
					// 110
					else if( pPtr[i+1] == 'n')
					{
						szText[iCnt]=10; // \n
						iCnt++;
						i++;
					}
					else
					{
						szText[iCnt] = pPtr[i];
						iCnt++;
					}
				}
				else
				{
					szText[iCnt] = pPtr[i];
					iCnt++;
				}
			}
		}

	}
}

#ifndef EXTRACT_HANGEUL_TOOL
void ioStringManager::WrtieTextList( const char *szFileName )
{
	FILE *pFile = fopen( szFileName, "wb" );
	if( !pFile )
		return;

	StringInfoMap *pTables[] = { &m_AppStringMap, &m_UiStringMap, &m_ConfigStringMap };
	for( int i=0 ; i<3 ; i++ )
	{
		for( StringInfoMap::iterator iter = pTables[i]->begin() ; iter != pTables[i]->end() ; ++iter )
		{
			fprintf( pFile, "|%s|%s|\r\n", iter->first.c_str(), iter->second.c_str() );
		}
	}

	fclose( pFile );
}

bool ioStringManager::InsertTextList( const char *szFunc, int iFuncNum, const char *szText )
{
	char szKey[MAX_PATH]="";
	StringCbPrintf( szKey, sizeof( szKey ),  "EXE_%s_%d", szFunc, iFuncNum );

	StringInfoMap &kTable = GetTable( FNP_EXE );
	StringInfoMap::iterator iter = kTable.find( szKey );
	if( iter != kTable.end() )
		return false;

	kTable.insert( StringInfoMap::value_type( ioHashString( szKey ), ioHashString( szText ) ) );
	return true;
}
#endif
