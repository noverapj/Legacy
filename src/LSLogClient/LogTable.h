// LogTable.h: interface for the CLogTable class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOGTABLE_H__D45981F3_8134_4B16_9A63_B33C62A9069E__INCLUDED_)
#define AFX_LOGTABLE_H__D45981F3_8134_4B16_9A63_B33C62A9069E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAX_LOG_TABLE 30

class CLogTable  
{
protected:
	static CLogTable *sg_Instance;
public:
	static CLogTable &GetInstance();
	static void ReleaseInstance();

private:
	typedef std::map<ioHashString, CLog* > LogTableMap;
	LogTableMap m_LogTableMap;

private:
	void AddLogName(const ioHashString &rkLogName, const ioHashString &rkLogContents, int iErrorNumber );

public:
	void WriteLogContents(const ioHashString &rkLogName, const ioHashString &rkLogContents, int iErrorNumber);
	void ClearAllLogName();

private: /* Singleton Class */
	CLogTable();
	virtual ~CLogTable();
};

#define g_LogTable CLogTable::GetInstance()
#endif // !defined(AFX_LOGTABLE_H__D45981F3_8134_4B16_9A63_B33C62A9069E__INCLUDED_)
