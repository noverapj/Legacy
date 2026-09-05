#pragma once

#include "ioPDH.h"
#include "Iprtrmib.h"

// data for nework info
typedef DWORD (_stdcall *TGetIfTable) 
(
  MIB_IFTABLE *pIfTable,  // buffer for interface table 
  ULONG *pdwSize,         // size of buffer
  BOOL bOrder             // sort the table by index?
);

typedef DWORD (_stdcall *TGetNumberOfInterfaces) 
(
  PDWORD pdwNumIf  // pointer to number of interfaces
);


class ioNetwork : public ioPDH
{
public:
	ioNetwork(void);
	~ioNetwork(void);

	void Init();
	void Destroy();

protected:
	BOOL Startup();
	void Cleanup();

	void GetInterface();
	void GetTraffic(DWORD &receivedBytes, DWORD &sentBytes);

public:
	BOOL GetNetworks(UNITS& units);

protected:
	DWORD m_receivedBytes, m_sentBytes;
	DWORD m_timeStamp;
	DWORD m_adapters;
	MIB_IFTABLE* m_ifTable;
};

