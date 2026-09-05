#include "StdAfx.h"
#include "LSLogicOperations.h"

 int TestOperation::Run(void * arg)
 {
	Tedata_* ttmp = (Tedata_*)arg;
	//printf("%d\n",ttmp->freeCount);
#if 0 
	 Tedata_* ttmp = (Tedata_*)arg;
	// printf("%d ",ttmp->serverId);
	 g_State()->IncrementTestCount();

	 if(g_State()->TestCount() >= 500000)
	 {
		 g_State()->PrintTime();
	 }

	 bool bfind = false;
//	 std::vector<std::pair<int,std::set<int>*>> &datas =   S_Test::instance()->m_threadDatas;
	 for(int i=0; i< datas.size(); ++i)
	 {
		 if(datas[i].first == ttmp->serverId)
		 {
			 auto rtval =	 datas[i].second->insert(ttmp->freeCount);
			 if(rtval.second == false)
			 {
				 ReportLOG.PrintTimeAndLog(0,"[%d]Error Date Duplecate(%d)\n",GetCurrentThreadId(),ttmp->freeCount);
				 printf("[%d]Error Date Duplecate(%d)\n",GetCurrentThreadId(),ttmp->freeCount);
			 }
			 bfind = true;
			 if(datas[i].second->size() > 1000000)
				datas[i].second->clear();
		 }
	 }
	 if(bfind == false)
	 {
		 std::set<int>* settmp = new std::set<int>;
		 std::pair<int,std::set<int>*> prset;// = new std::pair<int,std::set<int>*>;
		 prset.first = ttmp->serverId;
		 prset.second = settmp;
		 settmp->insert(ttmp->freeCount);
		 datas.push_back(prset);
	 }
	 #endif
	
	 return 0;
	 
 }

 int ReceiveOperation::Run(void * arg)
 {	 
	return 0;
 }

 int OnAccept::Run(void* arg)
 { 
	 OnAccept_* stdata = static_cast<OnAccept_*>(arg);
	 if(stdata->node)
	 {
		 g_ServerConnectMgr()->AddConnector(stdata->node);
		 g_ServerInfoMgr()->AddServerInfo(
			 stdata->node->GetSocket(),
			 ESOCKET::LS_CONNECTED,
			 stdata->node->m_ipAddr.c_str(),
			 stdata->node->m_port,
			 stdata->node->m_sendServerId);

		 SP2Packet pk(EPROTOCOL::LSPTK_CONNECT_REQUEST);
		 stdata->node->SendMessage(pk);

		 SchedulerOperation_ data;
		 data.opid			= EOPID::SCHEDULER;
		 data.eoperation	= ESCHEDULE::REQUESTSERVERINFO;
		 data.node			= g_Logic()->PScheduler();
		 PUTQFUNNC(data,g_Queue());
	 }
 
	 return 0;
 }

 int ReConnect::Run(void* arg)
 {
	 ReConnect_* stdata = static_cast<ReConnect_*>(arg);
	 SVRCONNECTINFO_ addr;
	 strcpy_s(addr.ipAddr,stdata->ipAddr);
	 addr.port			= stdata->serverPort;
	 addr.serverIndex	= stdata->serverIndex;
	 g_ConnectAssist()->PutConnectData(ECONNECTASSIST::CONNECT, addr);
	 return 0;
 }

 int SchedulerOperation::Run(void * arg)
 {
	 SchedulerOperation_* data = (SchedulerOperation_*)arg;
	 switch(data->eoperation)
	 {
	 case ESCHEDULE::SCHEDULEOP:
		 {
			 SchedulerNode* node = data->node;
			 node->Begin();
			 while(TRUE)
			 {
				 Schedule* schedule = node->GetSchedule();
				 if(schedule)
				 {
					 node->Call(schedule->GetCommand());
				 }
				 else
				 {
					 break;
				 }
			 }
			 node->End();
		 }
		 break;

	 case ESCHEDULE::GHOSTCLIENT:
		 {
			 g_ClientMgr()->DelGostClient();

		 }
		 break;

	 case ESCHEDULE::TIMEOUTCLIENT:
		 {
			 g_UserInfoMgr()->DelTimeOutClients();
		 }
		 break;

	 case ESCHEDULE::REQUESTSERVERINFO:
		 {
			 g_ServerConnectMgr()->RequestStatus();
		 }
		 break;

	 case ESCHEDULE::PING:
		 {
			 SP2Packet pk(EPROTOCOL::LSPTK_PING);
			 g_ClientMgr()->Broadcast(pk);
		 }
		 break;

	 case ESCHEDULE::SERVERPARTITION:
		 {
			 g_ServerConnectMgr()->RequestPartition();
		 }
		 break;
	 }
	
	 return 0;
 }

 int ChangeTickTime::Run(void * arg)
 {
	 ChangeTickTime_ *data = (ChangeTickTime_*)arg;
	 g_Config()->ReadLoadConfig();

	 data->code			= ITPK_SENDBUFFER_FLUSH_PROCESS;
	 data->nagleTime	= g_Config()->NagleTime();
	 data->node->ChangeTickValue(data->code,data->nagleTime);
	 return 0;
 }


 int TrySendServerInfo::Run(void* arg)
 {
	 if(m_firstState == false)
	 {
		 m_firstState = true;
		 TrySendServerInfo_* streq = (TrySendServerInfo_*)g_OPMemPool()->Pop(sizeof(TrySendServerInfo_));
		 streq->opid = EOPID::TRYSENDSERVERINFO;
		 g_TimerMgr()->AddTimer(true,g_Config()->RequestServerInfoTime()/2,streq);

	 }
	 g_UserInfoMgr()->TrySendSvrInfo();
	 
	 return 0;
 }