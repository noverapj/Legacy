
#include "stdafx.h"

#include "ioMemFile.h"
#include <vorbis/vorbisfile.h>

size_t ReadOggCallBack(void *ptr, size_t size, size_t nmemb, void *datasource)
{
	ioMemFile *pMemFile = NULL;
	pMemFile = (ioMemFile*)datasource;

	int iReadSize = 0;
	int iRetSize = 0;

	iReadSize = size * nmemb;

	iRetSize = pMemFile->Read( (unsigned char *)ptr, iReadSize );

	return (size_t)( iRetSize / size );
}

int SeekOggCallBack(void *datasource, ogg_int64_t offset, int whence)
{
	ioMemFile *pMemFile = NULL;
	pMemFile = (ioMemFile*)datasource;

	int iNewPos = 0;
	int iFileSize = pMemFile->GetMemFileSize();
	int iOffset = (int)offset;
	int iCurPos = pMemFile->GetCurPos();

	switch( whence )
	{
	case SEEK_SET:
		iNewPos = iOffset;

		if( iOffset < 0)
			iNewPos = -1;
		else if( iFileSize < iOffset )
			iNewPos = -1;
		break;
	case SEEK_CUR:
		iNewPos = iCurPos + iOffset;

		if( iNewPos > iFileSize)
			iNewPos = -1;		
		else if( iNewPos < 0 )
			iNewPos = -1;
		break;
	case SEEK_END:
		iNewPos = iCurPos + iOffset;
		if( iOffset > 0 )
			iNewPos = -1;
		else if( iNewPos > iFileSize )
			iNewPos = -1;
		break;
	}

	if( -1 != iNewPos)
	{
		pMemFile->SetCurPos(iNewPos);
		return 0;
	}

	return -1;
}

int CloseOggCallBack(void *datasource)
{
	return 0;
}

long TellOggCallBack(void *datasource)
{
	ioMemFile *pMemFile = (ioMemFile*)datasource;
	return pMemFile->GetCurPos();
}

DWORD ReadOggData( OggVorbis_File *pVF, char *pBuffer, DWORD dwReadSize, ioMemFile *pMemFile )
{
	DWORD dwReadData = 0;
	int iRet = 1;
	
	while( iRet != 0 && dwReadData < dwReadSize )
	{
		iRet = ov_read( pVF,
						pBuffer + dwReadData,
						dwReadSize - dwReadData,
						0,
						2,	// 16bit
						1,	// signed data
						(int*)pMemFile );

		dwReadData += iRet;
	}

	return dwReadData;
}