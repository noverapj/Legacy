

#include "stdafx.h"
#include "TimeReport.h"

TimeReport::TimeReport()
{
	m_iCheckFrameCount = 100;
	m_iCurFrameCount   = 0;

	m_dwPreStartTime = 0;
	m_bReportOn = false;
}

TimeReport::~TimeReport()
{
	ClearResource();
}

void TimeReport::SetReportOnOff( bool bOn, int iMaxFrame )
{
	m_iCheckFrameCount = iMaxFrame;
	m_iCurFrameCount = 0;

	m_dwPreStartTime = timeGetTime();
	m_bReportOn = bOn;

	ClearResource();
}

void TimeReport::StartFrame()
{
	if( !m_bReportOn )	return;

	m_dwPreStartTime = timeGetTime();
}

void TimeReport::AddPoint( int iPoint )
{
	if( !m_bReportOn )	return;

	DWORD dwFrameGap = timeGetTime() - m_dwPreStartTime;

	FrameTimeList *pList = NULL;

	FrameTable::iterator iter = m_FrameTable.find( iPoint );
	if( iter == m_FrameTable.end() )
	{
		pList = new FrameTimeList;
		pList->reserve( m_iCheckFrameCount );

		PointInfo kInfo;
		kInfo.pList = pList;
		kInfo.dwTotal = 0;
		kInfo.dwAverage = 0;

		m_FrameTable.insert( FrameTable::value_type( iPoint, kInfo ) );
	}
	else
	{
		pList = iter->second.pList;
	}

	pList->push_back( dwFrameGap );

	m_dwPreStartTime = timeGetTime();
}

void TimeReport::EndFrame()
{
	if( !m_bReportOn )	return;

	m_iCurFrameCount++;

	if( m_iCheckFrameCount > m_iCurFrameCount )
		return;

	FrameTable::iterator iter;
	for( iter = m_FrameTable.begin() ; iter!=m_FrameTable.end() ; ++iter )
	{
		PointInfo &rkInfo = iter->second;

		FrameTimeList *pList = rkInfo.pList;
		if( pList->empty() )
			continue;

		rkInfo.dwTotal = 0;

		int iTimeCount = pList->size();
		for( int i=0 ; i<iTimeCount ; i++ )
		{
			rkInfo.dwTotal += (*pList)[i];
		}

		rkInfo.dwAverage = rkInfo.dwTotal / iTimeCount;

		pList->clear();
	}

	m_iCurFrameCount = 0;
}

void TimeReport::ClearResource()
{
	FrameTable::iterator iter;
	for( iter=m_FrameTable.begin() ; iter!=m_FrameTable.end() ; ++iter )
	{
		delete iter->second.pList;
	}
	m_FrameTable.clear();
}

int TimeReport::GetPointCount() const
{
	return m_FrameTable.size();
}

bool TimeReport::GetPointInfo( int iIndex, char *szInfo ) const
{
	if( !COMPARE( iIndex, 0, GetPointCount() ) )
		return false;

	FrameTable::const_iterator iter = m_FrameTable.begin();
	std::advance( iter, iIndex );

	const PointInfo &rkInfo = iter->second;

	sprintf( szInfo, "%d : Total : %d, Avg : %d", iter->first, rkInfo.dwTotal, rkInfo.dwAverage );
	return true;
}


