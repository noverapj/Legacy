#pragma once

class TimeReport
{
protected:
	int m_iCheckFrameCount;
	int m_iCurFrameCount;

	DWORD m_dwPreStartTime;
	bool m_bReportOn;

	typedef std::vector<DWORD> FrameTimeList;
	
	struct PointInfo
	{
		FrameTimeList *pList;
		DWORD dwTotal;
		DWORD dwAverage;
	};

	typedef std::map< int, PointInfo > FrameTable;
	FrameTable m_FrameTable;

public:
	void SetReportOnOff( bool bOn, int iMaxFrame = 100 );

public:
	void StartFrame();
	void AddPoint( int iPoint );
	void EndFrame();

public:
	bool IsShow() const { return m_bReportOn; }
	int  GetPointCount() const;
	bool GetPointInfo( int iIndex, char *szInfo ) const;

private:
	void ClearResource();

public:
	TimeReport();
	~TimeReport();
};

