#include "../stdafx.h"
#include ".\iolocaltaiwan.h"

ioLocalTaiwan::ioLocalTaiwan(void)
{
}

ioLocalTaiwan::~ioLocalTaiwan(void)
{
}

ioLocalManager::LocalType ioLocalTaiwan::GetType()
{
	return ioLocalManager::LCT_TAIWAN;
}

void ioLocalTaiwan::GetPacPassword( OUT char *szPassword, IN const int iPasswordSize, IN const int iPasswordType /*=0*/ )
{
	if( iPasswordType == 0 )
	{
		// password : iUT38#@49vnFdjf)(4sg
		char szEncPassWord[MAX_PASSWORD]={ -105, 74, 75, 57, 101, 2, 44, -110, 127, 81, 110, 97, 34, -52, 10, 8, 117, 62, 108, 120 };
		EncryptDecryptData( szPassword, iPasswordSize, szEncPassWord, MAX_PASSWORD, true );
	}
	else if( iPasswordType == 1 )
	{
		// password : Yi#weT%^903Unv0$2gfj
		char szEncPassWord[MAX_PASSWORD]={ -89, 118, 60, 125, 56, 117, 73, -8, 127, 23, 51, 114, 40, -48, 92, 5, 111, 109, 121, 117 };
		EncryptDecryptData( szPassword, iPasswordSize, szEncPassWord, MAX_PASSWORD, true );
	}
}
