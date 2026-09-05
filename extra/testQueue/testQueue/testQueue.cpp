// testQueue.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include <iostream>
#include <set>
#include <conio.h>
#include "TestMPSCQueue.h"
//#include "TestIOCPQueue.h"
#include "TestMPMCQueue.h"
#include "TestWFQueue.h"
#include "TestMPMCMemoryPool.h"


INT G_TESTCOUNT = 1000000;
LONG G_NUMBER = 0;
BOOL G_TERMINATE =  FALSE; 

int _tmain(int argc, _TCHAR* argv[])
{
 
	while(1)
	{
		printf("\t\t ===========================================\t\t\n");
		printf("\t\t >> [0] Run MPSC QueueTest\n");
		printf("\t\t >> [1] Run IOCP QueueTest\n");
		printf("\t\t >> [2] Run MPMC QueueTest\n");
		printf("\t\t >> [3] Run MPMC MemoryPoolTest\n");
		printf("\t\t >> [4] Run WF QueueTest\n");
		printf("\t\t >> [q] Quit TestQueue Program\n");
		printf("\t\t >> ========================================\t\t\n\n");

		printf("\t\t >> Input : ");

		char c;
		std::cin >> c;

		switch(c)
		{
		case 'Q':
		case 'q':
			{
				break;
			}
			break;
		case '0':
			{
				int count;

				printf("Thread Count : ");
				scanf_s("%d", &count);

				for(int i = 0 ; i < 20 ; i++)
				{
					TestMPSCQueue* m = new TestMPSCQueue(count);
					m->Run();
					delete m;
				}
			}
			break;
		case '1':
			{
				//TestIOCPQueue* m = new TestIOCPQueue;
				//m->Run();
				//delete m;

			}
			break;
		case '2':
			{
				TestMPMCQueue* m = new TestMPMCQueue;
				m->Run();
				delete m;

			}
			break;
		case '3':
			{
				TestMPMCMemoryPool* m = new TestMPMCMemoryPool;
				m->Run();
				delete m;

			}
			break;
		 
		case '4':
			{
				int count;

				printf("Thread Count : ");
				scanf_s("%d", &count);

				for(int i = 0 ; i < 20 ; i++)
				{
					TestWFQueue* m = new TestWFQueue(count);
					m->Run();
					delete m;
				}
			}
			break;

		}
	}
	return 0;
}

