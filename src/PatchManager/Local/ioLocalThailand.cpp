#include "../stdafx.h"
#include ".\iolocalThailand.h"

ioLocalThailand::ioLocalThailand(void)
{
}

ioLocalThailand::~ioLocalThailand(void)
{
}

ioLocalManager::LocalType ioLocalThailand::GetType()
{
	return ioLocalManager::LCT_THAILAND;
}

void ioLocalThailand::GetPacPassword( OUT char *szPassword, IN const int iPasswordSize, IN const int iPasswordType /*=0*/  )
{
	if( iPasswordType == 0 )
	{
		// password : K3$dls49YU#$#eoE3054
		char szEncPassWord[MAX_PASSWORD]={ -75, 44, 59, 110, 49, 82, 88, -97, 31, 114, 35, 3, 101, -61, 3, 100, 110, 58, 42, 43 };
		EncryptDecryptData( szPassword, iPasswordSize, szEncPassWord, MAX_PASSWORD, true );
	}
	else if( iPasswordType == 1 )
	{
		// password : -_495IUEVJdlsl++32ed
		char szEncPassWord[MAX_PASSWORD]={ -45, 64, 43, 51, 104, 104, 57, -29, 16, 109, 100, 75, 53, -54, 71, 10, 110, 56, 122, 123 };
		EncryptDecryptData( szPassword, iPasswordSize, szEncPassWord, MAX_PASSWORD, true );
	}
}
