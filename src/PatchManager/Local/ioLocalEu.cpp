#include "../stdafx.h"
#include "ioLocalEU.h"


ioLocalEU::ioLocalEU(void)
{
}


ioLocalEU::~ioLocalEU(void)
{
}

ioLocalManager::LocalType ioLocalEU::GetType()
{
	return ioLocalManager::LCT_EU;
}

void ioLocalEU::GetPacPassword( OUT char *szPassword, IN const int iPasswordSize, IN const int iPasswordType /*=0*/ )
{
	if( iPasswordType == 0 )
	{
		// password : Efedf12-Asv
		char szEncPassWord[MAX_PASSWORD]={ -69, 121, 122, 110, 59, 16, 94, -117, 7, 84, 118, 39, 70, -90, 108, 33, 93, 10, 31, 31 };
		EncryptDecryptData( szPassword, iPasswordSize, szEncPassWord, MAX_PASSWORD, true );
	}
	else if( iPasswordType == 1 )
	{
		// password : fegG-24qw##4dfe52%3*
		char szEncPassWord[MAX_PASSWORD]={ -104, 122, 120, 77, 112, 19, 88, -41, 49, 4, 35, 19, 34, -64, 9, 20, 111, 47, 44, 53 };
		EncryptDecryptData( szPassword, iPasswordSize, szEncPassWord, MAX_PASSWORD, true );
	}
}
