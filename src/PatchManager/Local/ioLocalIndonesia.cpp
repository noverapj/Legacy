#include "../stdafx.h"
#include ".\ioLocalIndonesia.h"

ioLocalIndonesia::ioLocalIndonesia(void)
{
}

ioLocalIndonesia::~ioLocalIndonesia(void)
{
}

ioLocalManager::LocalType ioLocalIndonesia::GetType()
{
	return ioLocalManager::LCT_INDONESIA;
}

void ioLocalIndonesia::GetPacPassword( OUT char *szPassword, IN const int iPasswordSize, IN const int iPasswordType /*=0*/ )
{
	if( iPasswordType == 0 )
	{
		// password : T*$f40FRjfoe*(fl304d
		char szEncPassWord[MAX_PASSWORD]={ -86, 53, 59, 108, 105, 17, 42, -12, 44, 65, 111, 66, 108, -114, 10, 77, 110, 58, 43, 123 };
		EncryptDecryptData( szPassword, iPasswordSize, szEncPassWord, MAX_PASSWORD, true );
	}
	else if( iPasswordType == 1 )
	{
		// password : Mfe$%2049eFeodk*&31Z
		char szEncPassWord[MAX_PASSWORD]={ -77, 121, 122, 46, 120, 19, 92, -110, 127, 66, 70, 66, 41, -62, 7, 11, 123, 57, 46, 69 };
		EncryptDecryptData( szPassword, iPasswordSize, szEncPassWord, MAX_PASSWORD, true );
	}
}
