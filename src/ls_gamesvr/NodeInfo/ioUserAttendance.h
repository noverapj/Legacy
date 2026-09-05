#pragma once
#include "ioDBDataController.h"

class ioUserAttendance : public ioDBDataController
{
protected:
	typedef std::vector<DWORD> AttendanceRecord;
	AttendanceRecord m_AttendanceRecord;
	
	bool m_bSelectedAttendanceDB;
	DWORD m_dwConnectTime;

public:
	virtual void Initialize( User *pUser );
	virtual void DBtoData( CQueryResultData *query_data );	
	virtual void FillMoveData( SP2Packet &rkPacket );
	virtual void ApplyMoveData( SP2Packet &rkPacket, bool bDummyNode = false  );
	
	//사용 안함
	virtual bool DBtoNewIndex( DWORD dwIndex ){ return false; }
	virtual void SaveData(){}

public:
	bool IsThisMonthFirstConnect();
	bool IsTodayAttendanceCheck();
	bool HasDay( CTime Time );

public:
	void CheckTodayAttendance( SP2Packet& rkPacket );

public:
	void OnSelectAttendanceRecord();
	void OnResultSelectAttendanceRecord();

public:
	void SendAttendanceRecord();

public:
	void OnMacroAttendancePrevMonth( int iDayCount );
	void OnMacroAttendanceAddDay( int iDayCount );
	void OnMacroShowAttendanceWnd();
	void OnMacroShowAttendanceReset();
	void OnMacroAttendanceDateModify( int iYear, int iMonth, int iDay );

public:
	ioUserAttendance();
	virtual ~ioUserAttendance();
};