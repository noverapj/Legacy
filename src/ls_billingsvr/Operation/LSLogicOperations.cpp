#include "../stdAfx.h"
#include "LSLogicOperations.h"
#include <atlcoll.h>
#include "../Channeling/ioChannelingNodeParent.h"
#include "../Channeling/ioChannelingNodeNexonBuy.h"
#include "../Channeling/ioChannelingNodeManager.h"
#include "../NodeInfo/ServerNodeManager.h"
#include "../NodeInfo/GoodsManager.h"
#include "../Network/GameServer.h"
#include "../Network/iocpHandler.h"
#include "../Network/ioPacketQueue.h"
#include "../MgameBillingServer/MgameBillingServer.h"
#include "../channeling/iochannelingnodemanager.h"
#include "../database/logdbclient.h"
#include "../Local/ioLocalManager.h"
#include "../Version.h"
#include "../Local/ioLocalParent.h"
#include "../ioProcessChecker.h"
#include "../ThailandOTPServer/ThailandOTPNodeManager.h"
#include "../WemadeBillingServer/WemadeBuyServer.h"
#include "../MonitoringServerNode/monitoringnodemanager.h"
#include "../ToonilandBillingServer/ToonilandBillingServer.h"
#include "../ThreadPool/ioThreadPool.h"
#include "../ThreadPool/NexonThreadPool.h"
 
#include "../EtcHelpFunc.h"
 
#include "../Operation/Manager.h"
#include "../Network/ConnectAssist.h"
 
#include "../NodeInfo/MgrToolNodeManager.h"

extern CLog LOG;
extern CLog ProcessLOG;

 int TestOperation::Run(void * arg)
 {

	
	return 0; 
 }

 int ReNexonInit::Run(void * arg)
 {	 
	return 0;
 }

 int OnAccept::Run(void* arg)
 { 
	
	 
	 return 0;
 }

 int OnAccept::MakePortIndex(const ConnectNode* node) // 사용 X
 {
 
	 return 0;
 }

 int OnConnect::Run(void* arg)
 {
	 OnConnect_* connectData = static_cast<OnConnect_*>(arg);
	 if(connectData)
	 {
		 if(connectData->node)
		 {
			 CConnectNode* node = connectData->node;
			 node->OnCreate();

			 if(!node->AfterCreate())
			 {
				 ProcessLOG.PrintTimeAndLog(0,"Type %d AfterCreate Fail",node->GetConnectType());
			 }
		 }
	 }
 
	 return 0;
 }

 int WriteProcessLog::Run(void * arg)
 {
	 char strText[256];
	 
	 ProcessLOG.PrintTimeAndLog(0,"////////////////////////////////////////////////////////////////////////////////////////////////////////");
	 //GLOBAL TIME
	 sprintf_s(strText,"GLOBAL TIME : %d",TIMEGETTIME());
	 ProcessLOG.PrintTimeAndLog(0,strText);
 
	 //Network Info
	
	 
	 sprintf_s(strText,"SERVER IP:%s PORT:%d ",  g_App.GetPublicIP().c_str(),g_App.GetPort() );
	 ProcessLOG.PrintTimeAndLog(0,strText);
 

	 //Thread Info
	 sprintf_s(strText,"THREAD COUNT: %d",ThreadManager::GetInstance()->GetHandleCount());
	 ProcessLOG.PrintTimeAndLog(0,strText);
 

	 //Connect Client Info
	 sprintf_s(strText,"JOIN SERVER: %d(%d)        ",g_ServerNodeManager.GetNodeSize(), g_ServerNodeManager.GetDestroyNodeSize());
	 ProcessLOG.PrintTimeAndLog(0,strText);
	  

	 //Remainder MemPool Info
	 sprintf_s(strText,"REMAINDER MEMPOOL: %d ServerMemCount",g_ServerNodeManager.RemainderNode());
	 ProcessLOG.PrintTimeAndLog(0,strText);
 
	 //RECV QUEUE
	 int usingCount[4], remainCount[4];
	 g_RecvQueue.GetPoolCount( usingCount, remainCount );
	 sprintf_s(strText,"RECV PACKET: %d:%d:%d:%d QUEUE", usingCount[0], usingCount[1], usingCount[2], usingCount[3] );
	 ProcessLOG.PrintTimeAndLog(0,strText);
 

	 //Remainder MemPool Info
	 sprintf_s(strText,"REMAINDER MEMPOOL: %d:%d:%d:%d PacketMemCount", remainCount[0], remainCount[1], remainCount[2], remainCount[3] );
	 ProcessLOG.PrintTimeAndLog(0,strText);
	 

	 //Log SERVER INFO
	 if( g_LogDBClient.IsActive() )
		 sprintf_s(strText,"LOG DB SERVER CONNECT");
	 else
		 sprintf_s(strText,"LOG DB SERVER DISCONNECT");
	 ProcessLOG.PrintTimeAndLog(0,strText);
	 


	 if( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
	 {
		 //Mgame SERVER INFO
		 if( g_MgameBillingServer.IsActive() )
			 sprintf_s(strText,"Mgame Billing SERVER CONNECT");
		 else
			 sprintf_s(strText,"Mgame Billing SERVER DISCONNECT");
		 ProcessLOG.PrintTimeAndLog(0,strText);

		 //WEMADE BUY SERVER INFO
		 if( g_WemadeBuyServer.IsActive() )
			 sprintf_s(strText,"Wemade Buy SERVER CONNECT");
		 else
			 sprintf_s(strText,"Wemade Buy SERVER DISCONNECT");
		 ProcessLOG.PrintTimeAndLog(0,strText);

		 //Tooniland SERVER INFO
		 if( g_ToonilandBillingServer.IsActive() )
			 sprintf_s(strText,"Tooniland Billing SERVER CONNECT");
		 else
			 sprintf_s(strText,"Tooniland Billing SERVER DISCONNECT");
		 ProcessLOG.PrintTimeAndLog(0,strText);



		 sprintf_s(strText,"UserInfo memPool: %d/%d ",g_UserInfoManager->GetMemoryPoolCount(),g_UserInfoManager->GetUserInfoCount());//kyg 여기에 유저인퐄운트 추가 
		 ProcessLOG.PrintTimeAndLog(0,strText);

		 sprintf_s(strText,"BillInfoManager memPool: %d/%d ",g_BillInfoManager->GetMemoryPoolCount(), g_BillInfoManager->GetBillInfoCount());
		 ProcessLOG.PrintTimeAndLog(0,strText);


		 if( g_NexonSessionServer.IsActive() )
			 sprintf_s(strText,"Nexon Session SERVER CONNECT");
		 else
			 sprintf_s(strText,"Nexon Session  SERVER DISCONNECT");
		 ProcessLOG.PrintTimeAndLog(0,strText);


		 // Thread Pool
		 sprintf_s(strText,"DataPool:%d(%d) : ActiveThread:%d/%d", g_ThreadPool.GetNodeSize(), g_ThreadPool.RemainderNode(), g_ThreadPool.GetActiveThreadCount(), g_ThreadPool.GetThreadCount() );
		 ProcessLOG.PrintTimeAndLog(0,strText);
		 
	 }
	

	 sprintf_s(strText,"VERSION: %s | %s", STRFILEVER, STRINTERNALNAME );
	 ProcessLOG.PrintTimeAndLog(0,strText);
 
	 ProcessLOG.PrintTimeAndLog(0,"////////////////////////////////////////////////////////////////////////////////////////////////////////");
	 
	return 0;
 }

 int SchedulerOperation::Run(void * arg)
 {
	 SchedulerOperation_* data = (SchedulerOperation_*)arg;
	 
	
	 return 0;
 }

 void SchedulerOperation::SendServerInfo()
 {
 
 }

 void SchedulerOperation::OnGhostCheck()
 {
	
 }


int TestCloseOperation::Run( void * arg )
{	
#if(_TEST)
	TestClosedata_* test_ = (TestClosedata_*)arg;	
	ServerNode* pServerNode = new ServerNode();
	pServerNode->SetActive(true);
	

	if( test_->serverId == 1 )
	{
		/*printf("유저 로그인\n");
		SP2Packet spPacket( BSTPK_LOGIN );
		spPacket << "LoingGUID1234";
		spPacket << "testUser";
		spPacket << (int)12345;
		spPacket << "172.20.20.110";
		spPacket << (int)1200;
		spPacket << (int)1000;
		spPacket.SetPosBegin();
		g_RecvQueue.InsertQueue( (DWORD)pServerNode, spPacket, PK_QUEUE_SESSION );*/
		
		//유럽은 빌링전에 유저 인덱스 먼저 받는다.

		printf("유저 로그인\n");
		
		SP2Packet spPacket( BSTPK_LOGIN );
		spPacket << "LoingGUID1234";	//billingguid
		spPacket << "nxessotst11";			//privateID
		spPacket << "NP12:auth99:60:16670068:gVj7VD7XEeR~ZOoR1kAsVBYjHZDDC8mT7SRWYD7Cu6Ke6CYYeUxrNOhH40rB3KFbBxQ7Z6BLbw9l0MCcF0ReoJfLmw0Zv7zNGFdkpZ7fkeFU3SHPwb1VD84Lzon8Y0lNwEGpdrW6~7oYHavPpcpacIDI98YiyrenwUFkww~KXeFh0k_NcGJvy7tpL~a9NHW9Eu80nRy63octBkaZbvCzHhRW4NS6L4uimZLxVdhFdoN9mPfbTqoUWLr8YlnPh9wpO~4v~HwDQrQIWzc08U6o62AIjM3dzwHiz5VlUDY4ND8T4AfTjigZixNAhGaFDg5zSNTzkp6fduCG13jxjvs";			//EncodePW
		spPacket << "172.20.20.110";	//public IP
		spPacket << BSTPK_LOGIN_RESULT;	//dwReturnMsg -> BSTPK_LOGIN_RESULT
		
		spPacket.SetPosBegin();

		g_RecvQueue.InsertQueue( (DWORD)pServerNode, spPacket, PK_QUEUE_SESSION );
	}
	else if( test_->serverId == 2 )
	{
		/*
		rkPacket >> iChannelingType >> szBillingGUID >> dwUserIndex >> szPrivateID >> szPublicID >> bSetUserMouse // 공통 사항
			 >> szNexonID;
		*/
		/*
		printf("GetCash()\n");
		SP2Packet getPacket( BSTPK_GET_CASH );
		getPacket << 100;
		getPacket << "getGUID1234";
		getPacket << (int)12345;
		getPacket << "testUser";
		getPacket << "testUser";
		getPacket << (int)1200;
		getPacket << "172.20.20.110";
		getPacket << false;
		getPacket.SetPosBegin();
		g_RecvQueue.InsertQueue( (DWORD)pServerNode, getPacket, PK_QUEUE_SESSION );
		*/
		for( int i = 0; i<2000; i++)
		{
			printf("GetCash()\n");
			SP2Packet getPacket( BSTPK_GET_CASH );
			getPacket << (int)0;
			getPacket << "getGUID1234";	//guid
			getPacket << (int)12345;	//index
			getPacket << "testUser";	//nickname
			getPacket << "testUser";	//nickname
			getPacket << false;
			getPacket << "lsedevtst1";
			getPacket.SetPosBegin();
			g_RecvQueue.InsertQueue( (DWORD)pServerNode, getPacket, PK_QUEUE_SESSION );
		}
		
	}

	else if( test_->serverId == 3 )
	{
		printf("OutputCash()\n");
		
		/*
		
	rkPacket >> iChannelingType >> szBillingGUID >> dwUserIndex >> szPublicID >> szPrivateID >> szUserIP >> iPayAmt >> iType // 공통사항
			>> szNexonID >> szUserName >> dwNexonOID >> userAge >> dwPaymentType;
		*/
		/*
		SP2Packet outPacket( BSTPK_OUTPUT_CASH );
		outPacket << 0;
		outPacket << "outputGUID1234";
		outPacket << (int)12345;
		outPacket << "testUser";
		outPacket << "testUser";
		outPacket << "172.20.20.110";
		outPacket << (int)9800;
		outPacket << (int)1;
		outPacket << (int)55;
		outPacket << (int)1;
		outPacket << (int)1;
		outPacket << (int)1;
		outPacket << (int)1;
		outPacket << (int)1;
		outPacket << (int)1200;
		outPacket << "lsedevtst1";
		outPacket << "testUserName";
		outPacket << (int)9876;
		outPacket << (int)20;
		outPacket << (int)13001;
		outPacket.SetPosBegin();
		g_RecvQueue.InsertQueue( (DWORD)pServerNode, outPacket, PK_QUEUE_SESSION );
		*/

		int nTime = GetTickCount();

//		for( int i = 0; i<300; i++)
		{
			/*
			PACKET_GUARD_VOID( kBillingPacket.Write((int) GetChannelingType()) );
			PACKET_GUARD_VOID( kBillingPacket.Write(m_szBillingGUID) );
			PACKET_GUARD_VOID( kBillingPacket.Write(GetUserIndex()) );
			PACKET_GUARD_VOID( kBillingPacket.Write(GetPublicID()) );
			PACKET_GUARD_VOID( kBillingPacket.Write(GetPrivateID()) );
			PACKET_GUARD_VOID( kBillingPacket.Write(GetPublicIP()) );
			
			PACKET_GUARD_VOID( kBillingPacket.Write(iBuyCash) );
			PACKET_GUARD_VOID( kBillingPacket.Write(OUTPUT_CASH_SOLDIER) );
			PACKET_GUARD_VOID( kBillingPacket.Write(kCharInfo.m_class_type) );
			PACKET_GUARD_VOID( kBillingPacket.Write(kCharInfo.m_kindred) );
			PACKET_GUARD_VOID( kBillingPacket.Write(kCharInfo.m_sex) );
			PACKET_GUARD_VOID( kBillingPacket.Write(kCharInfo.m_iLimitSecond) );
			PACKET_GUARD_VOID( kBillingPacket.Write((int)kCharInfo.m_ePeriodType) );

			rkPacket << g_App.GetCSPort();
			rkPacket << pUser->GetBillingUserKey();
			*/
			SP2Packet outPacket( BSTPK_OUTPUT_CASH );
			outPacket << 300;
			outPacket << "guid19908813";
			outPacket << (int)9800;
			outPacket << (int)12345;
			outPacket << "19908813";
			outPacket << "19908813";
			outPacket << "172.20.20.110";

			outPacket << 500;

			outPacket  << 3;
			for( int i = 0; i < 3; i++ )
			{
				outPacket << i;	//사용할 보너스캐쉬 INDEX
				outPacket << 3000;
			}


			outPacket << (int)1;	//TYPE
			outPacket << (int)55;	//용병
			outPacket << (int)1;	//종족
			outPacket << (int)1;	//성별
			outPacket << (int)1;	//기간제
			outPacket << (int)1;	//기간제
			
			outPacket << (int)14009;
			outPacket << "19908813";
			outPacket << "19908813";
			//outPacket << (int)9876;
			//outPacket << (int)20;
			//outPacket << (int)13001;
			outPacket.SetPosBegin();
			g_RecvQueue.InsertQueue( (DWORD)pServerNode, outPacket, PK_QUEUE_SESSION );
			//rkPacket >> iChannelingType >> szBillingGUID >> dwUserIndex >> szPublicID >> szPrivateID >> szUserIP >> iPayAmt >> iType; 
		}

		int nResult = GetTickCount() - nTime;
		printf("spending Time %d\r\n", nResult);
	}

#endif
	return 0;
}
 