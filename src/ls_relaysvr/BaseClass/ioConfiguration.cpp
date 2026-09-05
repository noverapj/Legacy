#include "StdAfx.h"
#include "../UserDefineSingleton.h"
#include "ioConfiguration.h"
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#define ini_load_lib

#ifdef ini_load_lib
	#include "../../ioINILoader/ioINILoader.h"

	#ifdef _DEBUG
		#pragma comment( lib, "../../lib/INID.lib" )
	#else if RELEASE
		#pragma comment( lib, "../../lib/INI.lib" )
	#endif
#else
	#include <boost/property_tree/ptree.hpp>
	#include <boost/property_tree/ini_parser.hpp>
#endif


ioConfiguration::ioConfiguration(void) : m_allblock(FALSE)
{
}

ioConfiguration::~ioConfiguration(void)
{
}

bool ioConfiguration::Init()
{
	const char* szINI = GetINI()->c_str();
	ioINILoader kLoader( "ls_config_relay.ini" );
	

	GetPrivateProfileString("Default", "Log", "MLOG", m_logFolder, sizeof(m_logFolder), szINI);	
	try
	{
		/************************************************************************/
		/* AccetorInfo                                                          */
		/************************************************************************/ 
		AcceptorInfoLoad(szINI);

		/************************************************************************/
		/* Server Connect Info                                                                     */
		/************************************************************************/
		ServerConnectInfoLoad(kLoader);

		/************************************************************************/
		/* Common                                                                     */
		/************************************************************************/
		CommonInfoLoad(kLoader);

		/************************************************************************/
		/* ProcessChecker                                                                     */
		/************************************************************************/
		ProcessCheckerInfoLoad(kLoader);

		/************************************************************************/
		/* nagle                                                                     */
		/************************************************************************/
		SetNagleTime( kLoader.LoadInt( "NAGLE", "Nagle_Time", 30 ) );

		/************************************************************************/
		/* ClientInfo                                                                     */
		/************************************************************************/
		ClientInfoLoad(kLoader);
	}
	catch(boost::bad_lexical_cast &e)
	{
		LOG.PrintTimeAndLog(0,"ConfigFile Error[%s]",e.what());
		return false;
	}
	return true;
}

void ioConfiguration::AcceptorInfoLoad( const char* szINI )
{
	std::vector<std::string> portTokens;
	std::string strTmpPorts;
	TCHAR szTmpPort[128];
	GetPrivateProfileString("Default", "Port", "55001", szTmpPort, sizeof(szTmpPort), szINI);	
	strTmpPorts = szTmpPort;
	Tokenize(strTmpPorts,portTokens,",");
	for(UINT i = 0; i< portTokens.size(); ++i)
	{
		int udpPort = boost::lexical_cast<int>(portTokens[i]);
		m_udpPorts.push_back(udpPort);
	}

	if(m_udpPorts.empty())
		SetPort( 55001 );
	else
		SetPort(m_udpPorts[0]);

	SetSIpAddr("0.0.0.0");

	std::string szValue;
	for(int i=1; i< 100000; i++)
	{
		TCHAR key[80], value[256];

		sprintf_s(key, _T("%d"),i);
		GetPrivateProfileString("Default", key, "", value, sizeof(value), szINI);	
		//
		if(strlen(value) == 0) break;

		szValue = value;

		std::vector<std::string> tokens;
		Tokenize(szValue, tokens, ",");
		for(UINT j=0; j< tokens.size(); ++j)
			boost::algorithm::trim(tokens[j]);
		if(tokens.size() != 3) break;

		SVRCONNECTINFO_ serverInfo;
		serverInfo.serverIndex = i;
		strcpy_s(serverInfo.serverName, tokens[0].c_str());
		strcpy_s(serverInfo.ipAddr, tokens[1].c_str());
		serverInfo.port = boost::lexical_cast<int>(tokens[2]);

		m_serverAddrs.push_back(serverInfo);
	}
}

void ioConfiguration::ServerConnectInfoLoad( ioINILoader &kLoader )
{
	kLoader.SetTitle( "RELAYSERVERINFO" );

	SetConnectTime( kLoader.LoadInt( "RECONNECTTIME", 3000 ) );
	SetServerFullCount( kLoader.LoadInt( "RELAYSERVERINFO", "SERVERFULLCOUNT" , 1000 ) );
	SetRoomMaxCount(kLoader.LoadInt("RELAYSERVERINFO","ROOMMAX",3000));
	SetUserInfoMax(kLoader.LoadInt("RELAYSERVERINFO","USERINFOMAX",5000));
}

void ioConfiguration::CommonInfoLoad( ioINILoader &kLoader )
{
	m_workercount = kLoader.LoadInt( "COMMONINFO", "WORKERCOUNT", 5 );
	SetQueueFirstRecvQueue( kLoader.LoadInt( "COMMONINFO", "FIRSTRECVQUEUE", 1000 ) );
	SetPrivateIPFirstByte(kLoader.LoadInt("COMMONINFO","PRIVATEIPFIRSTBYTE",192));

	SetConnectMgrRecvSize( kLoader.LoadInt( "MONITORNODEINFO", "MAXRECV", DEFAULT_BUFFER*2+1 ) );
	SetConnectMgrSendSize( kLoader.LoadInt( "MONITORNODEINFO", "MAXSEND", DEFAULT_BUFFER ) );
	SetConnectMgrMaxPool( kLoader.LoadInt( "MONITORNODEINFO", "MAXPOOL", 10000 ) );
}

void ioConfiguration::ProcessCheckerInfoLoad( ioINILoader &kLoader )
{
	SetMaxLogCount( kLoader.LoadInt( "PacketChecker", "MaxLogCount", 10 ) );
	SetLogTime( kLoader.LoadInt( "ProcessChecker", "LogTime", 10000 ) );
}

void ioConfiguration::ClientInfoLoad( ioINILoader &kLoader )
{
	SetUseSecurity( kLoader.LoadInt( "MONITORNODEINFO", "USESECURITY", 0 ) );


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
	GetLocalIpAddress();
	m_dwIP = StrToDwordIP( m_publicIP );
}

void ioConfiguration::SetDefault()
{
	 

}

void ioConfiguration::ReadLoadConfig()
{
	/************************************************************************/
	/* nagle                                                                     */
	/************************************************************************/
	ioINILoader kLoader( "ls_config_relay.ini" );
	SetNagleTime( kLoader.LoadInt( "NAGLE", "Nagle_Time", 30 ) );
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

void ioConfiguration::GetMyIP( std::string &ipAddr )
{
	ipAddr = m_IpAddr;
}

bool ioConfiguration::GetLocalIpAddress()
{
	char szHostName[MAX_PATH];
	ZeroMemory( szHostName, sizeof( szHostName ) );
	gethostname(szHostName, sizeof(szHostName));
	LPHOSTENT lpstHostent = gethostbyname(szHostName);

	if ( !lpstHostent ) 
	{ 
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"GetLocalIpAddress::%s lpstHostend == NULL.", __FUNCTION__ );
		return false;
	}
	enum { MAX_LOOP = 100, };
	LPIN_ADDR lpstInAddr = NULL;
	std::vector<std::string> ipaddrs;

	if( lpstHostent->h_addrtype == AF_INET )
	{
		for (int i = 0; i < MAX_LOOP ; i++) // 100개까지 NIC 확인
		{
			lpstInAddr = (LPIN_ADDR)* lpstHostent->h_addr_list;

			if( lpstInAddr == NULL )
				break;

			char szTemp[MAX_PATH]="";
			strcpy_s( szTemp, inet_ntoa(*lpstInAddr) );
			ipaddrs.push_back(szTemp);

			lpstHostent->h_addr_list++;
		}
	}
	if(!COMPARE(ipaddrs.size(),1,3))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "GetLocalIpAddress::%s Size Error %d", __FUNCTION__, ipaddrs.size() );
		return false;

	}

	if(ipaddrs.size() == 1)
	{
		if(ipaddrs[0].empty())
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "GetLocalIpAddress::%s Size Error %d", __FUNCTION__, ipaddrs.size() );
			return false;
		}
		strcpy_s(m_publicIP,ipaddrs[0].c_str());
		strcpy_s(m_privateIP,ipaddrs[0].c_str());
	}

	for(UINT i=0; i< ipaddrs.size(); ++i)
	{
		if(ipaddrs[i].empty())
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "GetLocalIpAddress::%s Size Error %d", __FUNCTION__, ipaddrs.size() );
			return false;
		}
		if( atoi( ipaddrs[i].c_str()) != GetPrivateIPFirstByte() )//iPrivateIPFirstByte )
		{
			strcpy_s(m_publicIP,ipaddrs[i].c_str());
		}
		else
		{
			strcpy_s(m_privateIP,ipaddrs[i].c_str());
		}
	}
	return true;
}

DWORD ioConfiguration::StrToDwordIP(char* publicIP)
{
	int  count       = 0;
	int  cut_ip		 = 0;
	char szCut_ip[4][4];
	memset(szCut_ip,0,sizeof(szCut_ip));
	int  len	     = strlen(publicIP);
	for(int i = 0;i < len;i++)
	{
		if(publicIP[i] == '.')
		{
			count = 0;
			cut_ip++;
		}
		else
			szCut_ip[cut_ip][count++] = publicIP[i];
	}
	DWORD rtval = (DWORD)(atoi(szCut_ip[0])<<24) | (DWORD)(atoi(szCut_ip[1])<<16) | (DWORD)(atoi(szCut_ip[2])<<8) | atoi(szCut_ip[3]);
	return rtval;
}

