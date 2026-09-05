#include "../stdafx.h"
#include ".\iolocalJapan.h"

ioLocalJapan::ioLocalJapan(void)
{
}

ioLocalJapan::~ioLocalJapan(void)
{
}

ioLocalManager::LocalType ioLocalJapan::GetType()
{
	return ioLocalManager::LCT_JAPAN;
}

void ioLocalJapan::GetPacPassword( OUT char *szPassword, IN const int iPasswordSize, IN const int iPasswordType /*=0*/  )
{
	if( iPasswordType == 0 )
	{
		// password : EDgei%^df930%#fj!_=]
		char szEncPassWord[MAX_PASSWORD]={ -69, 91, 120, 111, 52, 4, 50, -62, 32, 30, 51, 23, 99, -123, 10, 75, 124, 85, 34, 66 };
		EncryptDecryptData( szPassword, iPasswordSize, szEncPassWord, MAX_PASSWORD, true );
	}
	else if( iPasswordType == 1 )
	{
		// password : @7$gjTRreie][!323O++
		char szEncPassWord[MAX_PASSWORD]={ -66, 40, 59, 109, 55, 117, 62, -44, 35, 78, 101, 122, 29, -121, 95, 19, 110, 69, 52, 52 };
		EncryptDecryptData( szPassword, iPasswordSize, szEncPassWord, MAX_PASSWORD, true );
	}
}
