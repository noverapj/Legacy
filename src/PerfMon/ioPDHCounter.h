#pragma once

#define CNTR_CPU						_T("\\Processor(_Total)\\% Processor Time")
#define CNTR_CPU1						_T("\\Processor(0)\\% Processor Time")
#define CNTR_CPU2						_T("\\Processor(1)\\% Processor Time")
#define CNTR_CPU3						_T("\\Processor(2)\\% Processor Time")
#define CNTR_CPU4						_T("\\Processor(3)\\% Processor Time")
#define CNTR_CPU5						_T("\\Processor(4)\\% Processor Time")
#define CNTR_CPU6						_T("\\Processor(5)\\% Processor Time")
#define CNTR_CPU7						_T("\\Processor(6)\\% Processor Time")
#define CNTR_CPU8						_T("\\Processor(7)\\% Processor Time")

#define CNTR_DISK_FREESPACE				_T("\\LogicalDisk(C:)\\% Free Space")
#define CNTR_DISK_IDLETIME				_T("\\LogicalDisk(C:)\\% Idle Time")
#define CNTR_DISK_READTIME				_T("\\LogicalDisk(C:)\\% Disk Read Time")
#define CNTR_DISK_WRITETIME				_T("\\LogicalDisk(C:)\\% Disk Write Time")
#define CNTR_DISK_TIME					_T("\\LogicalDisk(C:)\\% Disk Time")

#define CNTR_MEMORY_COMMITTED_BYTES		_T("\\Memory\\Committed Bytes")
#define CNTR_MEMORY_AVAIL_KB			_T("\\Memory\\Available KBytes")
#define CNTR_MEMORY_AVAIL_MB			_T("\\Memory\\Available MBytes")
#define CNTR_MEMORY_COMMITTED_PERCENT	_T("\\Memory\\% Committed Bytes In Use")
#define CNTR_MEMORY_LIMIT_BYTES			_T("\\Memory\\Commit Limit")

#define CNTR_NETWORKTOTAL_BYTES			_T("\\Network Interface(*)\\Bytes Total/sec")
#define CNTR_NETWORKRECV_BYTES			_T("\\Network Interface(*)\\Bytes Received/sec")
#define CNTR_NETWORKSEND_BYTES			_T("\\Network Interface(*)\\Bytes Sent/sec")
#define CNTR_NETWORK_BANDWIDTH			_T("\\Network Interface(*)\\Current Bandwidth")

const int MAX_RAW_VALUES = 20;

// PDH용 구조체
struct PDHCOUNTERSTRUCT 
{
	int index;				// The index of this counter, returned by AddCounter()
	//LONG m_value;			// The current value of this counter
	LONGLONG m_value;			// The current value of this counter
    HCOUNTER m_counter;      // Handle to the counter - given to use by PDH Library
    int m_nextIndex;         // element to get the next raw value
    int m_oldestIndex;       // element containing the oldes raw value
    int m_rawCount;          // number of elements containing raw values
    PDH_RAW_COUNTER m_rawValue[MAX_RAW_VALUES]; // Ring buffer to contain raw values
};


typedef std::vector<int> UNITS;