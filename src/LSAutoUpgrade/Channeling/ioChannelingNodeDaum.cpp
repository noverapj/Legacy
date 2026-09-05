#include "../stdafx.h"
#include ".\iochannelingnodedaum.h"
#include <strsafe.h>

ioChannelingNodeDaum::ioChannelingNodeDaum(void)
{
}

ioChannelingNodeDaum::~ioChannelingNodeDaum(void)
{
}

ChannelingType ioChannelingNodeDaum::GetType()
{
	return CNT_DAUM;
}

bool ioChannelingNodeDaum::IsShowDaumWindow( const char *szRootDir )
{
	char szFullPath[MAX_PATH*2]="";
	StringCbPrintf( szFullPath, sizeof( szFullPath ), "%s\\full.zip", szRootDir );

	// is exist
	FILE *pFile = fopen( szFullPath , "rt" );
	if( pFile == NULL )
		return true;
	fclose( pFile );
	return false;
}
