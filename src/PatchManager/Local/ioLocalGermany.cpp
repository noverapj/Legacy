#include "../stdafx.h"
#include ".\ioLocalGermany.h"

ioLocalGermany::ioLocalGermany(void)
{
}

ioLocalGermany::~ioLocalGermany(void)
{
}

ioLocalManager::LocalType ioLocalGermany::GetType()
{
	return ioLocalManager::LCT_GERMANY;
}

void ioLocalGermany::GetPacPassword( OUT char *szPassword, IN const int iPasswordSize, IN const int iPasswordType /*=0*/ )
{
	if( iPasswordType == 0 )
	{
		// password : +=Rt80kKoe1~%#OT93Nk
		char szEncPassWord[MAX_PASSWORD]={ -43, 34, 77, 126, 101, 17, 7, -19, 41, 66, 49, 89, 99, -123, 35, 117, 100, 57, 81, 116 };
		EncryptDecryptData( szPassword, iPasswordSize, szEncPassWord, MAX_PASSWORD, true );
	}
	else if( iPasswordType == 1 )
	{
		// password 1: XrFrI0%3BF%!0Dcx$30-
		//char szEncPassWord[MAX_PASSWORD]={ -90, 109, 89, 120, 20, 17, 73, -107, 4, 97, 37, 6, 118, -30, 15, 89, 121, 57, 47, 50 };
		//EncryptDecryptData( szPassword, iPasswordSize, szEncPassWord, MAX_PASSWORD, true );
	}
}
