// IniLibMultiThreadTest.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"

const int count = 10;

int _tmain(int argc, _TCHAR* argv[])
{
	ioINITestThread* testThread = new ioINITestThread[ count ];

	for(int i = 0; i < count; i++)
	{
		testThread[i].Begin();
	}
	
	cout << "end..." << endl;
	cin.ignore();

	return 0;
}

