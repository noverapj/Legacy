#include "StdAfx.h"
#include "LSLogicOperations.h"
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

 int TestOperation::Run(void * arg)
 {
	Tedata_* ttmp = (Tedata_*)arg;
	//printf("%d\n",ttmp->freeCount);
#if 0 
	 Tedata_* ttmp = (Tedata_*)arg;
	// printf("%d ",ttmp->serverId);
	 g_State()->IncrementTestCount();

	 if(g_State()->GetTestCount() >= 500000)
	 {
		 g_State()->PrintTime();

	 }
#endif
#if 0 
	 bool bfind = false;
	 std::vector<std::pair<int,std::set<int>*>> &datas =   S_Test::instance()->m_threadDatas;
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
	 
	 printf("TestOP : %d\n",ttmp->serverId++);

	 g_UDPNode()->InsertUserInfo(1,"aa",33,"33",33);
	 g_UDPNode()->DelUserInfoByServerID(33);
	 return 0;
	 
 }

 int ReceiveOperation::Run(void * arg)
 {	 
	return 0;
 }

 int OnAccept::Run(void* arg)
 { 
	 OnAccept_* stData = static_cast<OnAccept_*>(arg);
	 if(stData->node)
	 {
		g_ServerConnectMgr()->AddClient(stData->node); //아이피의 마지막 번대 + port 정버로 
		//여기서 인덱스 부여 
		
		SP2Packet pk(Protocols::RSPTK_ON_CONNECT);
		char sendPublicIP[STR_IP_MAX];
		const std::vector<int>* udpPorts = g_UDPNode()->GetUDP_port();
		strcpy_s(sendPublicIP,g_Config()->GetPublicIP());
		pk << sendPublicIP;
		pk << g_Config()->GetPort(); //아직 작업 안됀 게임서버들과의 호환을 위해서 
		pk << (int)udpPorts->size();

		for(UINT i=0; i< udpPorts->size();++i)
		{
			pk << udpPorts->at(i);
		}
		stData->node->OnCreate();
		if(stData->node->AfterCreate())
			stData->node->SendMessage(pk);
		else
			LOG.PrintTimeAndLog(0,"Error OnAccept");
		LOG.PrintTimeAndLog(0,"Public IP : %s:%d",g_Config()->GetPublicIP(),g_Config()->GetPort());
	 }
	 return 0;
 }

 int OnAccept::MakePortIndex(const GameServerNode* node) // 사용 X
 {
#if 0 
	 int udpPortSize = g_UDPNode()->GetUDP_port()->size();
	 std::string serverIpAddr = node->ServerAddress();
	 typedef vector< std::string > split_vector_type;
	 split_vector_type splitIpAddrs;
	 boost::split(splitIpAddrs,serverIpAddr,boost::is_any_of("."),boost::token_compress_on);
	 int tmpPortNum = boost::lexical_cast<int>(splitIpAddrs[splitIpAddrs.size()-1]);
	 tmpPortNum += node->ServerPort() +1;
	 Debug("GmaerServer Port: %d RelayServer Port : %d\n ",node->ServerPort(),tmpPortNum % udpPortSize);
	 int resultValue = tmpPortNum % udpPortSize;
#endif
	 return 0;
 }

 int ReConnect::Run(void* arg)
 {
	 ReConnect_* stdata = static_cast<ReConnect_*>(arg);
	 SVRCONNECTINFO_ addr;
	 strcpy_s(addr.ipAddr,stdata->ipAddr);
	 addr.port			= stdata->serverPort;
	 addr.serverIndex	= stdata->serverIndex;
	 g_ConnectAssist()->PutConnectData(ConnectAssistTypes::CONNECT, addr);
	 return 0;
 }

 int SchedulerOperation::Run(void * arg)
 {
	 SchedulerOperation_* data = (SchedulerOperation_*)arg;
	 switch(data->eoperation)
	 {
	 case ScheduleTypes::SCHEDULEOP:
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
	 case ScheduleTypes::ONPING:
		 {
			 SP2Packet pk(Protocols::LSPTK_PING);
			 g_MonitorMgr()->Broadcast(pk);

		 }
		 break;
	 case ScheduleTypes::ONSENDSERVERINFO:
		 {
			 //kyg 여기에 자기 자신의 서버 인포를 넘김 
			 //printf("ONSENDSERVERINFO");
			 SendServerInfo();
		 }
		 break;
	 case ScheduleTypes::USERGHOSTCHECK:
		 {
			 OnGhostCheck();

		 }
		 break;
	 case ScheduleTypes::USERKINGCHECK:
		 {

		 }
		 break;
	 case ScheduleTypes::REPORT:
		 {
			 g_State()->Set64DropCount(g_UDPNode()->Get64DropCount());
			 g_State()->Set256DropCount(g_UDPNode()->Get256DropCount());
			 g_State()->Set1024DropCount(g_UDPNode()->Get1024DropCount());
			 g_State()->Set64UsingCount(g_UDPNode()->GetBuffer64PoolCount());
			 g_State()->Set256UsingCount(g_UDPNode()->GetBuffer256PoolCount());
			 g_State()->Set1024UsingCount(g_UDPNode()->GetBuffer1024PoolCount());

			 ReportLOG.PrintTimeAndLog(0,"(User:%d|Room:%d)UD:[%05d]:[%05d]:[%05d]  UR:[%05d]:[%05d]:[%05d]",
				 g_State()->GetUserCount(),g_State()->RoomCount(),
				 g_State()->Get64DropCount(),g_State()->Get256DropCount(),g_State()->Get1024DropCount(),
				 g_State()->Get64UsingCount(),g_State()->Get256UsingCount(),g_State()->Get1024UsingCount());

			 ReportLOG.PrintTimeAndLog(0,"UR:[%04d]:[%04d]:[%04d]", g_UDPNode()->Get64RemainderCount(),g_UDPNode()->Get256RemainderCount(),g_UDPNode()->Get1024RemainderCount());
			 Debug("UD:[%05d]:[%05d]:[%05d]  UR:[%05d]:[%05d]:[%05d]\n",
				 g_State()->Get64DropCount(),g_State()->Get256DropCount(),g_State()->Get1024DropCount(),
				 g_State()->Get64UsingCount(),g_State()->Get256UsingCount(),g_State()->Get1024UsingCount());
			ReportLOG.PrintTimeAndLog(0,"UDP Total COunt : %d Server Connector : %d", g_UDPNode()->GetBufferPoolTotalCount(),g_ServerConnectMgr()->GetNodeSize());
			Debug("UDP BufferPoolTotal COunt : %d Server Connector : %d\n", g_UDPNode()->GetBufferPoolTotalCount(),g_ServerConnectMgr()->GetNodeSize());
		 }
		 break;
	 case ScheduleTypes::SENDBUFFERFLUSH:
		 {
			 g_ServerConnectMgr()->SendBufferFlush();
			 g_MonitorMgr()->SendBufferFlush();
		 }
		 break;
 #if(_DEBUG)
	 case ScheduleTypes::TEST:
		 {
			 ReportLOG.PrintTimeAndLog(0,"SendCount : %010d(%010d)QueCount : %04d\n",g_State()->nSend(),g_State()->nSend()/10,g_Queue()->GetSize());
			 g_State()->SetSendCount(0);
		 }
		 break;
 #endif
	 }
	
	 return 0;
 }

 void SchedulerOperation::SendServerInfo()
 {
	 SP2Packet pk(Protocols::RSPTK_ON_CONTROL);
	 int ctpye = ControlTypes::RS_INFO;
	 pk << ctpye; 
	 SendRelayInfo_ stData;
	 strcpy_s(stData.m_ipAddr , g_Config()->GetPublicIP());
	 stData.m_port = g_Config()->GetPort();
	 stData.m_userCount = g_State()->GetUserCount();
	 stData.m_roomCount = g_State()->RoomCount();
	 stData.m_serverCount = g_ServerConnectMgr()->GetNodeSize();
	 stData.m_64DropCount =    g_State()->Get64DropCount();
	 stData.m_256DropCount =  g_State()->Get256DropCount();
	 stData.m_1024DropCount =  g_State()->Get1024DropCount();
	 stData.m_64UsingCount = g_State()->Get64UsingCount();
	 stData.m_256UsingCount = g_State()->Get256UsingCount();
	 stData.m_1024UsingCount = g_State()->Get1024UsingCount();
	 pk << stData;
	 g_ServerConnectMgr()->SendMessageAllNode(pk);
 }

 void SchedulerOperation::OnGhostCheck()
 {
	g_UDPNode()->GhostCheck();
 }

 int ChangeTickTime::Run(void * arg)
 {
	 ChangeTickTime_ *data = (ChangeTickTime_*)arg;
	 g_Config()->ReadLoadConfig();
	 data->code = ITPK_SENDBUFFER_FLUSH_PROCESS;
	 data->nagleTime = g_Config()->GetNagleTime();
	 data->node->ChangeTickValue(data->code,data->nagleTime);
	 return 0;
 }


