#include "StdAfx.h"
#include "ioTestClass.h"


 
ioTestClass::ioTestClass(void)
{
	
}


ioTestClass::~ioTestClass(void)
{
}

void ioTestClass::Run()
{
	boost::thread tTest(&ioTestClass::svc,this);
	//tTest.join();
}

void ioTestClass::testfunc()
{
	int i = 0;
	while(1)
	{
 		Tedata_ op;/* = (Tedata_*)g_OPMemPool()->Pop(sizeof(Tedata_));*/
 		op.opid = 0;
		op.serverId = GetCurrentThreadId();
		op.freeCount = i;
		PUTQFUNNC(op,(g_Queue()));
		++i;
		if(i > 20000000)
			i = 0;
		Sleep(1);

	}
}

void ioTestClass::svc()
{
	int a = 0;
	while(1)
	{
		a = getchar();
 
		switch(a)
		{
		case '0':
			for(int i=0; i<10; ++i)
				boost::thread(&ioTestClass::testfunc,this);
			break;
		case '1':
			{	
				Tedata_ op;
				op.opid = 0;
				op.serverId =1;
				 
				PUTQFUNNC(op,g_Queue());
				  
			}
			break;
		case '2':
			{
			 
			}
			break;
		case '3':
			{
				 

			}
			break;
		case 'q':
			return;
			break;
		default://이거 오퍼레이션화 해볼까
			int now = std::clock();
			int ntime = now - g_State()->GetTimeNow();
			ntime = ntime / 1000;
			printf("\n\t\t >> RelayServer(%s:%d)  << \n",g_Config()->GetSIpAddr().c_str(),g_Config()->GetPort());
			printf("\t\t >> OPPool Count : %d\n",g_State()->GetOPPoolCount());
			printf("\t\t >> ServerConnector Count : %d\n",g_ServerConnectMgr()->GetNodeSize());
			printf("\t\t >> Packet QUEUE Count : %d\n",g_Queue()->GetSize());
			printf("\t\t >> UDP Node PoolCount :%d\n",g_UDPNode()->GetBufferPoolTotalCount());
			printf("\t\t >> Room :%d User: %d\n",g_State()->RoomCount(),g_State()->GetUserCount());
			printf("\t\t >> Send Count : %d\n",g_State()->GetTestCount());
			printf("\t\t >> Send Count : %d\n",g_State()->nSend());
		 
		//	LOG.GetFucton() =  [](char* s)->int{g_State(); printf("%d\n",g_State()->nSend());return 1; };
			//LOG.Test();
			//aa = [](std::string const * s) -> int [return s.Size();];
		}


	}
}