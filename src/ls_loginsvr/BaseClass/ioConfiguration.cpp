#include "StdAfx.h"
#include "../UserDefineSingleton.h"
#include "../../ioINILoader/ioINILoader.h"
#include "ioConfiguration.h"
#include <boost/lexical_cast.hpp>

ioConfiguration::ioConfiguration(void) : m_allblock(FALSE), m_checkUserTimeout(2500), m_checkPingTime(1000*180), m_partitionTimer(1000*60*5), m_whiteListOn(FALSE)
{
}


ioConfiguration::~ioConfiguration(void)
{
}


bool ioConfiguration::Init()
{
	/************************************************************************/
	/* AccetorInfo                                                          */
	/************************************************************************/
	const char* szINI = GetINI()->c_str();

	int port = GetPrivateProfileInt("Default", "Port", 40005, szINI);	
	int partition = GetPrivateProfileInt("Default", "Partition", 0, szINI);	
	GetPrivateProfileString("Default", "Log", "MLOG", m_logFolder, sizeof(m_logFolder), szINI);	

	NPort( port );
	SetPartition( partition );
	SIpAddr("0.0.0.0");

	m_blackList.Load( "ls_blacklist.ini", TRUE );
	m_whiteList.Load( "ls_whitelist.ini" );

	std::string szValue;
	for(int i=0; i< 100000; i++)
	{
		TCHAR key[80], value[256];

		sprintf_s(key, _T("%d"),i);
		GetPrivateProfileString("Default", key, "", value, sizeof(value), szINI);	
		if(strlen(value) == 0) break;
			
		szValue = value;

		std::vector<std::string> tokens;
		Tokenize(szValue, tokens, ",");
		if(tokens.size() != 3) break;

		SVRCONNECTINFO_ serverInfo;
		serverInfo.serverIndex = i+1;
		strcpy_s(serverInfo.serverName, tokens[0].c_str());
		strcpy_s(serverInfo.ipAddr, tokens[1].c_str());
		serverInfo.port = boost::lexical_cast<int>(tokens[2]);

		m_vServerAddr.push_back(serverInfo);
	}

	ioINILoader kLoader( "ls_config_login.ini" );

	try
	{
		NWorkerCount( kLoader.LoadInt( "COMMONINFO", "WORKERCOUNT", 5 ) );
		NQueueFirstRecvQueue( kLoader.LoadInt( "COMMONINFO", "FIRSTRECVQUEUE", 1000 ) );

		/************************************************************************/
		/* Server Connect Info                                                                     */
		/************************************************************************/
		kLoader.SetTitle( "GAMESERVERCONNECTORINFO" );
		ConnectTime( kLoader.LoadInt( "RECONNECTTIME", 3000 ) );

		ServerFullCount( kLoader.LoadInt( "GAMESERVERCONNECTORINFO", "SERVERFULLCOUNT" , 1000 ) );
		RequestServerInfoTime( kLoader.LoadInt( "GAMESERVERCONNECTORINFO", "REQUESTSERVERINFOTIME", 3000 ) );
		/************************************************************************/
		/* Common                                                                     */
		/************************************************************************/
		NCMgrRecvSize( kLoader.LoadInt( "CLIENTNODEINFO", "MAXRECV", DEFAULT_BUFFER*2+1 ) );
		NCMgrSendSize( kLoader.LoadInt( "CLIENTNODEINFO", "MAXSEND", DEFAULT_BUFFER ) );
		NCMgrMaxPool( kLoader.LoadInt( "CLIENTNODEINFO", "MAXPOOL", 10000 ) );
		/************************************************************************/
		/* ProcessChecker                                                                     */
		/************************************************************************/
		CheckerPassTime( kLoader.LoadInt( "PacketChecker", "CheckerPassTime", 5 ) );
		MaxLogCount( kLoader.LoadInt( "PacketChecker", "MaxLogCount", 10 ) );
		LogTime( kLoader.LoadInt( "ProcessChecker", "LogTime", 10000 ) );
		/************************************************************************/
		/* nagle                                                                     */
		/************************************************************************/
		NagleTime( kLoader.LoadInt( "NAGLE", "Nagle_Time", 30 ) );
		/************************************************************************/
		/* ClientInfo                                                                     */
		/************************************************************************/
		ClientGhostTime( kLoader.LoadInt( "CLIENTNODEINFO", "GHOSTCLIENTTIME", 3000 ) );
		ScanClientGhostTime( kLoader.LoadInt( "CLIENTNODEINFO", "SCANGHOSTCLIENTTIME", 2500 ) );
		MaxWaitSecond( kLoader.LoadInt( "CLIENTNODEINFO", "MAXWAIT", 100 ) );
		UseSecurity( kLoader.LoadInt( "CLIENTNODEINFO", "USESECURITY", 1 ) );
		UserInfoMax( kLoader.LoadInt( "CLIENTNODEINFO", "USERINFOMAX", 5000 ) );

		TCHAR buffer[1024];
		gethostname(buffer,1024);
		struct hostent *fHost;
		fHost = gethostbyname(buffer);
		sprintf(buffer, "%d.%d.%d.%d", 
			((struct in_addr *)(fHost->h_addr))->S_un.S_un_b.s_b1,
			((struct in_addr *)(fHost->h_addr))->S_un.S_un_b.s_b2,
			((struct in_addr *)(fHost->h_addr))->S_un.S_un_b.s_b3,
			((struct in_addr *)(fHost->h_addr))->S_un.S_un_b.s_b4
			);
		strcpy_s(m_IP,buffer);
	}
	catch(boost::bad_lexical_cast &e)
	{
		LOG.PrintTimeAndLog(0,"ConfigFile Error[%s]",e.what());
		return false;
	}
	return true;
}

void ioConfiguration::SetDefault()
{
	 

}

void ioConfiguration::ReadLoadConfig()
{
	/************************************************************************/
	/* nagle                                                                     */
	/************************************************************************/
	ioINILoader kLoader;
	kLoader.ReloadFile( "ls_config_login.ini" );

	NagleTime( kLoader.LoadInt( "NAGLE", "Nagle_Time", 50 ) );
}

void ioConfiguration::Tokenize( const std::string& str,std::vector<string>& tokens, const std::string& delimiters /*= " "*/ )
{
	// Skip delimiters at beginning.
	std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
	// Find first "non-delimiter".
	std::string::size_type pos     = str.find_first_of(delimiters, lastPos);

	while (std::string::npos != pos || std::string::npos != lastPos)
	{
		// Found a token, add it to the vector.
		tokens.push_back(str.substr(lastPos, pos - lastPos));
		// Skip delimiters.  Note the "not_of"
		lastPos = str.find_first_not_of(delimiters, pos);
		// Find next "non-delimiter"
		pos = str.find_first_of(delimiters, lastPos);
	}
}

BOOL ioConfiguration::CheckBlackList(char* IP)
{
	if(m_blackList.IsActive())
	{
		return m_blackList.Find(IP) ? TRUE : FALSE;
	}
	return FALSE;
}

BOOL ioConfiguration::CheckWhiteList(char* IP)
{
	if(m_whiteList.IsActive() && IsWhiteListOn() )
	{
		return m_whiteList.Find(IP) ? TRUE : FALSE;
	}
	return TRUE;
}
