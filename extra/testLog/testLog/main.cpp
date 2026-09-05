#include "stdafx.h"
#include "ioLogTestThread.h"



void main()
{
	const int logCount = 100;

	ioLogTestThread* testThread = new ioLogTestThread[logCount];
	for(int i = 0; i < logCount; i++)
	{
		testThread[i].Begin();
	}

	printf("press enter to exit ");
	char ch = getchar();
	for(int i = 0; i < logCount; i++)
	{
		testThread[i].SetStart( FALSE );
	}
}