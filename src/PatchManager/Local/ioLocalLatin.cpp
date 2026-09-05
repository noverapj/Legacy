#include "../stdafx.h"
#include ".\ioLocalLatin.h"


ioLocalLatin::ioLocalLatin(void)
{
}


ioLocalLatin::~ioLocalLatin(void)
{
}

ioLocalManager::LocalType ioLocalLatin::GetType()
{
	return ioLocalManager::LCT_LATIN;
}

void ioLocalLatin::GetPacPassword( OUT char *szPassword, IN const int iPasswordSize, IN const int iPasswordType /*=0*/  )
{
	if( iPasswordType == 0 )
	{
		// password 0: dus!qhdaksl
		char szEncPassWord[MAX_PASSWORD]={ -102, 106, 108, 43, 44, 73, 8, -57, 45, 84, 108, 39, 70, -90, 108, 33, 93, 10, 31, 31 };
		EncryptDecryptData( szPassword, iPasswordSize, szEncPassWord, MAX_PASSWORD, true );
	}
	else if( iPasswordType == 1 )
	{
		// password 1: tkdjqqn!dlfEhrqkfhgo
		char szEncPassWord[MAX_PASSWORD]={ -118, 116, 123, 96, 44, 80, 2, -121, 34, 75, 102, 98, 46, -44, 29, 74, 59, 98, 120, 112 };
		EncryptDecryptData( szPassword, iPasswordSize, szEncPassWord, MAX_PASSWORD, true );
	}
}
