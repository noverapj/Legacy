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
		PUTQFUNNC(op,g_Queue());
	 

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
			for(int i=0; i<2; ++i)
				boost::thread(&ioTestClass::testfunc,this);
			break;
		case '1':
			{	
				printf("--------------------------PrintSendRecvCount-------------------------------\n");
				g_ServerConnectMgr()->Print();
				printf("---------------------------------END---------------------------------------\n");
				  
			}
			break;
		case '2':
			{
				printf("--------------------------PrintServerInfo-------------------------------\n");
				g_ServerInfoMgr()->Print();
				printf("------------------------------END--------------------------------------\n");
			}
			break;
		case 'q':
			return;
			break;
		default://이거 오퍼레이션화 해볼까
			int now = std::clock();
			int ntime = now - g_State()->Now();
			ntime = ntime / 1000;
			printf("\n\t\t >> LoginServer(%s:%d)  << \n",g_Config()->SIpAddr().c_str(),g_Config()->NPort());
			printf("\t\t >> Accept Count :%d(%d) \n",(g_State()->AcceptCount() / ntime),g_State()->AcceptCount());
			printf("\t\t >> dllAccept count :%d(%d)\n",(g_State()->DllAcceptCount() / ntime),g_State()->DllAcceptCount());
			printf("\t\t >> Close  Count : %d\n",g_State()->CloseCount());
			printf("\t\t >> OPPool Count : %d\n",g_State()->OPPoolCount());
			printf("\t\t >> ClientsManager Count : %d\n",g_ClientMgr()->GetMemPoolSize());
			printf("\t\t >> RealConnetUser Count : %d\n",g_UserInfoMgr()->GetSize());
			printf("\t\t >> ServerConnector Count : %d\n",g_ConnectAssist()->GetMemPoolSize());
			printf("\t\t >> Packet QUEUE Count : %d\n",g_Queue()->GetSize());
			printf("\t\t >> Avg OnClientPacket : %d(%d)\n",g_State()->OnClientCount() / ntime,g_State()->OnClientCount());
			printf("\t\t>> 1.PrintServer[send||recv] Info \n");
			printf("\t\t>> 2.PrintServer[User]Info \n");
			printf("\t\t>> 3.PrintServer[SendCount]Info \n");


		}


	}
}