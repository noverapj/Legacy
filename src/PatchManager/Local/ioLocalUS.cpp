#include "../stdafx.h"
#include ".\iolocalus.h"

ioLocalUS::ioLocalUS(void)
{
}

ioLocalUS::~ioLocalUS(void)
{
}

ioLocalManager::LocalType ioLocalUS::GetType()
{
	return ioLocalManager::LCT_US;
}

void ioLocalUS::GetPacPassword( OUT char *szPassword, IN const int iPasswordSize, IN const int iPasswordType /*=0*/ )
{
	if( iPasswordType == 0 )
	{
		// password : eE39DkE!%E0
		// ÇÊ¸®ÇÉ¿ë : `~3>KmE!%E0 / char szEncPassWord[MAX_PASSWORD]={ -98, 97, 44, 52, 22, 76, 41, -121, 99, 98, 48, 39, 70, -90, 108, 33, 94, 10, 31, 31 };
		char szEncPassWord[MAX_PASSWORD]={ -101, 90, 44, 51, 25, 74, 41, -121, 99, 98, 48, 39, 70, -90, 108, 33, 93, 10, 31, 31 };
		EncryptDecryptData( szPassword, iPasswordSize, szEncPassWord, MAX_PASSWORD, true );
	}
	else if( iPasswordType == 1 )
	{
		// password : Eg%^io03UT$Cvf921-!$
		// ÇÊ¸®ÇÉ¿ë : s%N>?03UI$Cvf923-!$ / char szEncPassWord[MAX_PASSWORD]={ -115, 58, 81, 52, -69, 78, 92, -107, 19, 110, 36, 100, 48, -64, 85, 19, 110, 39, 62, 59 };
		char szEncPassWord[MAX_PASSWORD]={ -69, 120, 58, 84, 52, 78, 92, -107, 19, 115, 36, 100, 48, -64, 85, 19, 108, 39, 62, 59 };
		EncryptDecryptData( szPassword, iPasswordSize, szEncPassWord, MAX_PASSWORD, true );
	}
}
