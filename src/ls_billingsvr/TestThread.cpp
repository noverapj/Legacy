#include "stdafx.h"
#include "TestThread.h"
#include "Operation/LSLogicOperations.h"
#include "Network/ioPacketQueue.h"



TestThread::TestThread(void)
{
}


TestThread::~TestThread(void)
{
}

void TestThread::Run()
{
#if(_TEST)
	{

		int a = 0;
		while(1)
		{
			scanf("%d",&a);
			switch(a)
			{
			case 1:
				{
				/*printf("SendReLoginPacket Test\n");
				Tedata_ test_;
				test_.serverId = 33;
				OPERATIONRUN(test_);
				printf("printUserInfo\n");*/
					
					TestClosedata_ test;
					test.serverId = 1;
					OPERATIONRUN(test);
				}
				break;
			
			case 2:
				{
					TestClosedata_ test;
					test.serverId = 2;
					OPERATIONRUN(test);
				}
				break;

			case 3:
				{
					TestClosedata_ test;
					test.serverId = 3;
					OPERATIONRUN(test);
				}
				break;

			case 4:
				{
					TestClosedata_ test;
					test.serverId = 4;
					OPERATIONRUN(test);
				}
				break;

			case 5:
				{
					TestClosedata_ test;
					test.serverId = 5;
					OPERATIONRUN(test);
				}
				break;
			case 6:
				{
					TestClosedata_ test;
					test.serverId = 6;
					OPERATIONRUN(test);
				}
				break;
			case 7:
				{
					TestClosedata_ test;
					test.serverId = 7;
					OPERATIONRUN(test);
				}
				break;

			case 8:
				{
					TestClosedata_ test;
					test.serverId = 8;
					OPERATIONRUN(test);
				}
				break;
			
			case 10:
				{
					TestClosedata_ test;
					test.serverId = 10;
					OPERATIONRUN(test);
				}
				break;
			case 11:
				{
					TestClosedata_ test;
					test.serverId = 11;
					OPERATIONRUN(test);
				}
				break;
			}
		}
	}
#endif
}
