// ReportUtil.h: interface for the CReportUtil class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REPORTUTIL_H__BEC741CA_16FA_4ADD_AEDD_2797231CD35E__INCLUDED_)
#define AFX_REPORTUTIL_H__BEC741CA_16FA_4ADD_AEDD_2797231CD35E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CReportUtil  
{
	public:
	virtual long GetReportValue() = 0;
	virtual void InitReportValue() = 0;
};

class CUpdateUtil : CReportUtil
{
	public:
	static long m_value;

	public:
	virtual long GetReportValue(){ return m_value; }
	virtual void InitReportValue(){ m_value = 0; }
};

class CSelectUtil : CReportUtil
{
	public:
	static long m_value;
	
	public:
	virtual long GetReportValue(){ return m_value; }
	virtual void InitReportValue(){ m_value = 0; }
};
 
class CDeleteUtil : CReportUtil
{
	public:
	static long m_value;
	
	public:
	virtual long GetReportValue(){ return m_value; }
	virtual void InitReportValue(){ m_value = 0; }
};

class CInsertUtil : CReportUtil
{
	public:
	static long m_value;
	
	public:
	virtual long GetReportValue(){ return m_value; }
	virtual void InitReportValue(){ m_value = 0; }
};

class CSelectEX1Util : CReportUtil
{
public:
	static long m_value;

public:
	virtual long GetReportValue(){ return m_value; }
	virtual void InitReportValue(){ m_value = 0; }
};

 

#endif // !defined(AFX_REPORTUTIL_H__BEC741CA_16FA_4ADD_AEDD_2797231CD35E__INCLUDED_)
