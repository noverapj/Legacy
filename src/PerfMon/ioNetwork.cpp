#include "stdafx.h"
#include "ioNetwork.h"


TGetIfTable GetIfTable;
TGetNumberOfInterfaces GetNumberOfInterfaces;


ioNetwork::ioNetwork(void) : m_ifTable(NULL), m_adapters(0), m_receivedBytes(0), m_sentBytes(0), m_timeStamp(0)
{
	Init();
}

ioNetwork::~ioNetwork(void)
{
	Destroy();
}

void ioNetwork::Init()
{
	Startup();
}

void ioNetwork::Destroy()
{
	Cleanup();
}

BOOL ioNetwork::Startup()
{
	// PDH
	//ioPDH::Startup();

	//BOOL result = AddCounter(CNTR_NETWORKTOTAL_BYTES);
	//result = AddCounter(CNTR_NETWORKRECV_BYTES);
	//result = AddCounter(CNTR_NETWORKSEND_BYTES);
	//result = AddCounter(CNTR_NETWORK_BANDWIDTH);

	// pgl api
	GetIfTable = (TGetIfTable)GetProcAddress( LoadLibrary(_T("Iphlpapi.dll")), (LPCSTR)("GetIfTable") );
	GetNumberOfInterfaces = (TGetNumberOfInterfaces)GetProcAddress( LoadLibrary(_T("Iphlpapi.dll")), (LPCSTR)("GetNumberOfInterfaces") );

	if( !GetIfTable || !GetNumberOfInterfaces )
	{
		return FALSE;
	}

	ULONG code = GetIfTable( m_ifTable,	&m_adapters, TRUE );
	if( code == 122 /* The data area passed to a system call is too small.*/ )
	{
		m_ifTable = new MIB_IFTABLE[m_adapters];
		return (!m_ifTable) ? FALSE : TRUE;
	}
	return TRUE;
}

void ioNetwork::Cleanup()
{
	PdhCloseQuery(&m_query);
}

void ioNetwork::GetInterface()
{
	char szDesc[ MAXLEN_IFDESCR ];

	GetIfTable( m_ifTable, &m_adapters, TRUE );
	DWORD count = m_ifTable->dwNumEntries;

	for(unsigned int i = 0 ; i < count ; i++)
	{
		MIB_IFROW table = m_ifTable->table[ i ];

		memcpy( szDesc, table.bDescr, table.dwDescrLen );
		szDesc[ table.dwDescrLen ] = 0;

		//printf( "interface desc    : %s\n", szDesc );
		//printf( "received data     : %d\n", table.dwInOctets );
		//printf( "sent data         : %d\n", table.dwOutOctets );

		memset( szDesc, 0x00, MAXLEN_IFDESCR );
	}
}

void ioNetwork::GetTraffic( DWORD &receivedBytes, DWORD &sentBytes )
{
	//	network
	GetIfTable(m_ifTable, &m_adapters, TRUE);
	DWORD count	= m_ifTable->dwNumEntries;

	DWORD inbound	= 0;
	DWORD outbound	= 0;

	for( DWORD i = count-1 ; i > 0 ; i-- )
	{
		MIB_IFROW table = m_ifTable->table[i];
		inbound		+= table.dwInOctets;
		outbound	+= table.dwOutOctets;
	}

	if(0 == m_timeStamp)
	{
		m_timeStamp = GetTickCount();
		m_receivedBytes	= inbound;
		m_sentBytes		= outbound;
		receivedBytes	= 0;
		sentBytes		= 0;
	}
	else
	{
		DWORD current	= GetTickCount();
		double elapsed	= (current - m_timeStamp) / 1000.0;
		receivedBytes	= static_cast<DWORD>((inbound - m_receivedBytes) / elapsed);
		sentBytes		= static_cast<DWORD>((outbound - m_sentBytes) / elapsed);

		m_receivedBytes	= inbound;
		m_sentBytes		= outbound;
		m_timeStamp		= current;
	}
}

BOOL ioNetwork::GetNetworks(UNITS& units)
{
	SetError( 0 );

	DWORD receivedBytes, sentBytes;
	GetTraffic(receivedBytes, sentBytes);

	units.push_back(receivedBytes / 1024);
	units.push_back(sentBytes / 1024);
	//// collect
	//if(!CollectQuery())
	//	return FALSE;

	//// update counters
	//for(int i = 0 ; i < GetCounters() ; i++)
	//{
	//	int network = GetValue(m_counters[i]->index);
	//	if( network == -999L )
	//	{
	//		return FALSE;
	//	}

	//	units.push_back(network);
	//}
	return TRUE;
}

