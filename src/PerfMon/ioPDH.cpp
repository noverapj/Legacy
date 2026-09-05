#include "stdafx.h"
#include "ioPDH.h"

  void PDHError(PDH_STATUS result)
  {
      HANDLE hPdhLibrary = NULL;
      TCHAR* pMessage = NULL;
      DWORD_PTR pArgs[] = { (DWORD_PTR)_T("<collectionname>") };
      DWORD dwErrorCode = result;
  
      hPdhLibrary = LoadLibrary(_T("pdh.dll"));
      if (NULL == hPdhLibrary)
      {
          _tprintf(_T("LoadLibrary failed with %lu\r\n"), GetLastError());
          return;
      }
  
      // Use the arguments array if the message contains insertion points, or you
      // can use FORMAT_MESSAGE_IGNORE_INSERTS to ignore the insertion points.
  
      if (!FormatMessage(FORMAT_MESSAGE_FROM_HMODULE |
                         FORMAT_MESSAGE_ALLOCATE_BUFFER |
                         /*FORMAT_MESSAGE_IGNORE_INSERTS |*/
                         FORMAT_MESSAGE_ARGUMENT_ARRAY,
                         hPdhLibrary, 
                         dwErrorCode,
                         0,  
                         pMessage, 
                         0, 
                         //NULL))
                         (va_list*)pArgs))
      {
          _tprintf(_T("Format message failed with 0x%x\r\n"), GetLastError());
          return;
      }
  
      _tprintf(_T("Formatted message: %s\r\n"), pMessage);
      LocalFree(pMessage);
  }
  


ioPDH::ioPDH(void) : m_index(0), m_error(0), m_query(NULL)
{
	Init();
}

ioPDH::~ioPDH(void)
{
	Destroy();
}

void ioPDH::Init()
{
}

void ioPDH::Destroy()
{
	Cleanup();
}

BOOL ioPDH::Startup()
{
	if(NULL == m_query)
	{
		if(PdhOpenQuery(NULL, 1, &m_query) != ERROR_SUCCESS)
		{
			SetError( GetLastError() );
			return FALSE;
		}
	}
	return TRUE;
}

void ioPDH::Cleanup()
{
	if(NULL != m_query)
	{
		PdhCloseQuery(&m_query);
		m_query = NULL;
	}
}

BOOL ioPDH::AddCounter(const TCHAR *counterName)
{
	PDHCOUNTERSTRUCT* pdhCounter = new PDHCOUNTERSTRUCT;
	if(!pdhCounter) return FALSE;

	// add to current query
	if(PdhAddCounter(m_query, counterName, (DWORD)pdhCounter, &(pdhCounter->m_counter)) != ERROR_SUCCESS)
	{
		SetError( GetLastError() );
		delete pdhCounter;
		return FALSE;
	}

	// insert counter into array(s)
	pdhCounter->index		= m_index++;
	pdhCounter->m_value		= 0;
	pdhCounter->m_nextIndex	= 0;
	pdhCounter->m_oldestIndex = 0;
	pdhCounter->m_rawCount	= 0;

	m_counters.push_back(pdhCounter);
	return TRUE;
}

BOOL ioPDH::CollectQuery()
{
	if(PdhCollectQueryData(m_query) != ERROR_SUCCESS)
	{
		SetError( GetLastError() );
		return FALSE;
	}
	return TRUE;
}

BOOL ioPDH::UpdateValue(PDHCOUNTERSTRUCT* pdhCounter)
{
	PDH_FMT_COUNTERVALUE pdhFormattedValue;

	//if (PdhGetFormattedCounterValue(pdhCounter->m_counter, PDH_FMT_LONG, NULL, &pdhFormattedValue) != ERROR_SUCCESS)
	PDH_STATUS result = PdhGetFormattedCounterValue(pdhCounter->m_counter, PDH_FMT_LARGE, NULL, &pdhFormattedValue);
	if (result != ERROR_SUCCESS)
	{
		SetError( result );
		return FALSE;
	}

	// test the value for validity
	if (pdhFormattedValue.CStatus != ERROR_SUCCESS)
	{
		SetError( GetLastError() );
		return FALSE;
	}

	// set value
	pdhCounter->m_value = pdhFormattedValue.largeValue;
	return TRUE;
}

BOOL ioPDH::UpdateRawValue(PDHCOUNTERSTRUCT* pdhCounter)
{
    PPDH_RAW_COUNTER ppdhRawCounter;

    // Assign the next value into the array
    ppdhRawCounter = &(pdhCounter->m_rawValue[pdhCounter->m_nextIndex]);

	if(PdhGetRawCounterValue(pdhCounter->m_counter, NULL, ppdhRawCounter) != ERROR_SUCCESS)
	{
		SetError( GetLastError() );
		return FALSE;
	}
	
    pdhCounter->m_rawCount	= min(pdhCounter->m_rawCount + 1, MAX_RAW_VALUES);
    pdhCounter->m_nextIndex	= (pdhCounter->m_nextIndex + 1) % MAX_RAW_VALUES;

    if(pdhCounter->m_rawCount >= MAX_RAW_VALUES)
        pdhCounter->m_oldestIndex = pdhCounter->m_nextIndex;

	return TRUE;
}

BOOL ioPDH::GetStatistics(long *valueMin, long *valueMax, long *valueMean, int index)
{
	PDH_STATISTICS pdhStats;
	PDHCOUNTERSTRUCT* pdhCounter = GetCounter(index);
	if(!pdhCounter) return FALSE;

	if(PdhComputeCounterStatistics(
		pdhCounter->m_counter, 
		PDH_FMT_LONG, 
		pdhCounter->m_oldestIndex, 
		pdhCounter->m_rawCount, 
		pdhCounter->m_rawValue, 
		&pdhStats) != ERROR_SUCCESS) 
	{
		SetError( GetLastError() );
		return FALSE;
	}

	// set values
	if (pdhStats.min.CStatus != ERROR_SUCCESS)
		*valueMin = 0;
	else
		*valueMin = pdhStats.min.longValue;

	if (pdhStats.max.CStatus != ERROR_SUCCESS)
		*valueMax = 0;
	else
		*valueMax = pdhStats.max.longValue;

	if (pdhStats.mean.CStatus != ERROR_SUCCESS)
		*valueMean = 0;
	else
		*valueMean = pdhStats.mean.longValue;

	return TRUE;
}

LONGLONG ioPDH::GetValue(int index)
{
	PDHCOUNTERSTRUCT* pdhCounter = GetCounter(index);
	if (!pdhCounter)					return -999L;

	// update the value
	if (!UpdateValue(pdhCounter))		return -999L;
	if (!UpdateRawValue(pdhCounter))	return -999L;

	return pdhCounter->m_value;
}

PDHCOUNTERSTRUCT* ioPDH::GetCounter(int index)
{
	for(unsigned int i = 0 ; i < m_counters.size() ; i++)
	{
		if(m_counters[i]->index == index)
		{
			return m_counters[i];
		}
	}
	return NULL;
}

