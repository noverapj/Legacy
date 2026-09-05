#include "../stdafx.h"
#include ".\iolocalChina.h"

ioLocalChina::ioLocalChina(void)
{
}

ioLocalChina::~ioLocalChina(void)
{
}

ioLocalManager::LocalType ioLocalChina::GetType()
{
	return ioLocalManager::LCT_CHINA;
}

void ioLocalChina::GetPacPassword( OUT char *szPassword, IN const int iPasswordSize, IN const int iPasswordType /*=0*/  )
{
	if( iPasswordType == 0 )
	{
		// password : -)4TRfkl-41$%dgkrm05
		char szEncPassWord[MAX_PASSWORD]={ -45, 54, 43, 94, 15, 71, 7, -54, 107, 19, 49, 3, 99, -62, 11, 74, 47, 103, 47, 42 };
		EncryptDecryptData( szPassword, iPasswordSize, szEncPassWord, MAX_PASSWORD, true );
	}
	else if( iPasswordType == 1 )
	{
		// password : |059rtuGReowo@##tkg0
		char szEncPassWord[MAX_PASSWORD]={ -126, 47, 42, 51, 47, 85, 25, -31, 20, 66, 111, 80, 41, -26, 79, 2, 41, 97, 120, 47 };
		EncryptDecryptData( szPassword, iPasswordSize, szEncPassWord, MAX_PASSWORD, true );
	}
}
